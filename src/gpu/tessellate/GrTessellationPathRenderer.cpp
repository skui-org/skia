/*
 * Copyright 2019 Google LLC.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "src/gpu/tessellate/GrTessellationPathRenderer.h"

#include "include/pathops/SkPathOps.h"
#include "src/core/SkIPoint16.h"
#include "src/core/SkPathPriv.h"
#include "src/gpu/GrClip.h"
#include "src/gpu/GrMemoryPool.h"
#include "src/gpu/GrRecordingContextPriv.h"
#include "src/gpu/GrRenderTargetContext.h"
#include "src/gpu/GrSurfaceContextPriv.h"
#include "src/gpu/geometry/GrStyledShape.h"
#include "src/gpu/ops/GrFillRectOp.h"
#include "src/gpu/tessellate/GrDrawAtlasPathOp.h"
#include "src/gpu/tessellate/GrTessellatePathOp.h"
#include "src/gpu/tessellate/GrWangsFormula.h"

constexpr static SkISize kAtlasInitialSize{512, 512};
constexpr static int kMaxAtlasSize = 2048;

// The atlas is only used for small-area paths, which means at least one dimension of every path is
// guaranteed to be quite small. So if we transpose tall paths, then every path will have a small
// height, which lends very well to efficient pow2 atlas packing.
constexpr static auto kAtlasAlgorithm = GrDynamicAtlas::RectanizerAlgorithm::kPow2;

// Ensure every path in the atlas falls in or below the 128px high rectanizer band.
constexpr static int kMaxAtlasPathHeight = 128;

GrTessellationPathRenderer::GrTessellationPathRenderer(const GrCaps& caps)
        : fAtlas(GrColorType::kAlpha_8, GrDynamicAtlas::InternalMultisample::kYes,
                 kAtlasInitialSize, std::min(kMaxAtlasSize, caps.maxPreferredRenderTargetSize()),
                 caps, kAtlasAlgorithm) {
    this->initAtlasFlags(*caps.shaderCaps());
}

void GrTessellationPathRenderer::initAtlasFlags(const GrShaderCaps& shaderCaps) {
    fStencilAtlasFlags = OpFlags::kStencilOnly | OpFlags::kDisableHWTessellation;
    fMaxAtlasPathWidth = fAtlas.maxAtlasSize() / 2;
    // The atlas usually does better with hardware tessellation. If hardware tessellation is
    // supported, we choose a max atlas path width that is guaranteed to never require more
    // tessellation segments than are supported by the hardware.
    if (!shaderCaps.tessellationSupport()) {
        return;
    }
    // Since we limit the area of paths in the atlas to kMaxAtlasPathHeight^2, taller paths can't
    // get very wide anyway. Find the tallest path whose width is limited by
    // GrWangsFormula::worst_case_cubic() rather than the max area constraint, and use that for our
    // max atlas path width.
    //
    // Solve the following equation for w:
    //
    //     GrWangsFormula::worst_case_cubic(kLinearizationIntolerance, w, kMaxAtlasPathHeight^2 / w)
    //              == maxTessellationSegments
    //
    float k = GrWangsFormula::cubic_k(kLinearizationIntolerance);
    float h = kMaxAtlasPathHeight;
    float s = shaderCaps.maxTessellationSegments();
    // Quadratic formula from Numerical Recipes in C:
    //
    //     q = -1/2 [b + sign(b) sqrt(b*b - 4*a*c)]
    //     x1 = q/a
    //     x2 = c/q
    //
    // float a = 1;  // 'a' is always 1 in our specific equation.
    float b = -s*s*s*s / (4*k*k);  // Always negative.
    float c = h*h*h*h;  // Always positive.
    float det = b*b - 4*1*c;
    if (det <= 0) {
        // maxTessellationSegments is too small for any path whose area == kMaxAtlasPathHeight^2.
        // (This is unexpected because the GL spec mandates a minimum of 64 segments.)
        SkDebugf("WARNING: maxTessellationSegments seems too low. (%i)\n",
                 shaderCaps.maxTessellationSegments());
        return;
    }
    float q = -.5f * (b - std::sqrt(det));  // Always positive.
    // The two roots represent the width^2 and height^2 of the tallest rectangle that is limited by
    // GrWangsFormula::worst_case_cubic().
    float r0 = q;  // Always positive.
    float r1 = c/q;  // Always positive.
    float worstCaseWidth = std::sqrt(std::max(r0, r1));
#ifdef SK_DEBUG
    float worstCaseHeight = std::sqrt(std::min(r0, r1));
    // Verify the above equation worked as expected. It should have found a width and height whose
    // area == kMaxAtlasPathHeight^2.
    SkASSERT(SkScalarNearlyEqual(worstCaseHeight * worstCaseWidth, h*h, 1));
    // Verify GrWangsFormula::worst_case_cubic() still works as we expect. The worst case number of
    // segments for this bounding box should be maxTessellationSegments.
    SkASSERT(SkScalarNearlyEqual(GrWangsFormula::worst_case_cubic(
            kLinearizationIntolerance, worstCaseWidth, worstCaseHeight), s, 1));
#endif
    fStencilAtlasFlags &= ~OpFlags::kDisableHWTessellation;
    fMaxAtlasPathWidth = std::min(fMaxAtlasPathWidth, (int)worstCaseWidth);
}

GrPathRenderer::CanDrawPath GrTessellationPathRenderer::onCanDrawPath(
        const CanDrawPathArgs& args) const {
    if (!args.fShape->style().isSimpleFill() || args.fShape->inverseFilled() ||
        args.fViewMatrix->hasPerspective()) {
        return CanDrawPath::kNo;
    }
    if (GrAAType::kCoverage == args.fAAType) {
        SkASSERT(1 == args.fProxy->numSamples());
        if (!args.fProxy->canUseMixedSamples(*args.fCaps)) {
            return CanDrawPath::kNo;
        }
    }
    SkPath path;
    args.fShape->asPath(&path);
    if (SkPathPriv::ConicWeightCnt(path)) {
        return CanDrawPath::kNo;
    }
    return CanDrawPath::kYes;
}

bool GrTessellationPathRenderer::onDrawPath(const DrawPathArgs& args) {
    GrRenderTargetContext* renderTargetContext = args.fRenderTargetContext;
    GrOpMemoryPool* pool = args.fContext->priv().opMemoryPool();
    const GrShaderCaps& shaderCaps = *args.fContext->priv().caps()->shaderCaps();

    SkPath path;
    args.fShape->asPath(&path);

    SkRect devBounds;
    args.fViewMatrix->mapRect(&devBounds, path.getBounds());

    // See if the path is small and simple enough to atlas instead of drawing directly.
    //
    // NOTE: The atlas uses alpha8 coverage even for msaa render targets. We could theoretically
    // render the sample mask to an integer texture, but such a scheme would probably require
    // GL_EXT_post_depth_coverage, which appears to have low adoption.
    SkIRect devIBounds;
    SkIPoint16 locationInAtlas;
    bool transposedInAtlas;
    if (this->tryAddPathToAtlas(*args.fContext->priv().caps(), *args.fViewMatrix, path, devBounds,
                                args.fAAType, &devIBounds, &locationInAtlas, &transposedInAtlas)) {
#ifdef SK_DEBUG
        // If using hardware tessellation in the atlas, make sure the max number of segments is
        // sufficient for this path. fMaxAtlasPathWidth should have been tuned for this to always be
        // the case.
        if (!(fStencilAtlasFlags & OpFlags::kDisableHWTessellation)) {
            int worstCaseNumSegments = GrWangsFormula::worst_case_cubic(kLinearizationIntolerance,
                                                                        devIBounds.width(),
                                                                        devIBounds.height());
            SkASSERT(worstCaseNumSegments <= shaderCaps.maxTessellationSegments());
        }
#endif
        auto op = pool->allocate<GrDrawAtlasPathOp>(
                renderTargetContext->numSamples(), sk_ref_sp(fAtlas.textureProxy()),
                devIBounds, locationInAtlas, transposedInAtlas, *args.fViewMatrix,
                std::move(args.fPaint));
        renderTargetContext->addDrawOp(args.fClip, std::move(op));
        return true;
    }

    auto drawPathFlags = OpFlags::kNone;

    // Find the worst-case log2 number of line segments that a curve in this path might need to be
    // divided into.
    int worstCaseResolveLevel = GrWangsFormula::worst_case_cubic_log2(kLinearizationIntolerance,
                                                                      devBounds.width(),
                                                                      devBounds.height());
    if (worstCaseResolveLevel > kMaxResolveLevel) {
        // The path is too large for our internal indirect draw shaders. Crop it to the viewport.
        SkPath viewport;
        viewport.addRect(SkRect::MakeIWH(renderTargetContext->width(),
                                         renderTargetContext->height()).makeOutset(1, 1));
        // Perform the crop in device space so it's a simple rect-path intersection.
        path.transform(*args.fViewMatrix);
        if (!Op(viewport, path, kIntersect_SkPathOp, &path)) {
            // The crop can fail if the PathOps encounter NaN or infinities. Return true
            // because drawing nothing is acceptable behavior for FP overflow.
            return true;
        }
        // Transform the path back to its own local space.
        SkMatrix inverse;
        if (!args.fViewMatrix->invert(&inverse)) {
            return true;  // Singular view matrix. Nothing would have drawn anyway. Return true.
        }
        path.transform(inverse);
        path.setIsVolatile(true);
        args.fViewMatrix->mapRect(&devBounds, path.getBounds());
        worstCaseResolveLevel = GrWangsFormula::worst_case_cubic_log2(kLinearizationIntolerance,
                                                                      devBounds.width(),
                                                                      devBounds.height());
        // kMaxResolveLevel should be large enough to tessellate paths the size of any screen we
        // might encounter.
        SkASSERT(worstCaseResolveLevel <= kMaxResolveLevel);
    }

    if ((1 << worstCaseResolveLevel) > shaderCaps.maxTessellationSegments()) {
        // The path is too large for hardware tessellation; a curve in this bounding box could
        // potentially require more segments than are supported by the hardware. Fall back on
        // indirect draws.
        drawPathFlags |= OpFlags::kDisableHWTessellation;
    }

    auto op = pool->allocate<GrTessellatePathOp>(*args.fViewMatrix, path, std::move(args.fPaint),
                                                 args.fAAType, drawPathFlags);
    renderTargetContext->addDrawOp(args.fClip, std::move(op));
    return true;
}

bool GrTessellationPathRenderer::tryAddPathToAtlas(
        const GrCaps& caps, const SkMatrix& viewMatrix, const SkPath& path, const SkRect& devBounds,
        GrAAType aaType, SkIRect* devIBounds, SkIPoint16* locationInAtlas,
        bool* transposedInAtlas) {
    if (!caps.multisampleDisableSupport() && GrAAType::kNone == aaType) {
        return false;
    }

    // Atlas paths require their points to be transformed on the CPU and copied into an "uber path".
    // Check if this path has too many points to justify this extra work.
    if (path.countPoints() > 200) {
        return false;
    }

    // Transpose tall paths in the atlas. Since we limit ourselves to small-area paths, this
    // guarantees that every atlas entry has a small height, which lends very well to efficient pow2
    // atlas packing.
    devBounds.roundOut(devIBounds);
    int maxDimenstion = devIBounds->width();
    int minDimension = devIBounds->height();
    *transposedInAtlas = minDimension > maxDimenstion;
    if (*transposedInAtlas) {
        std::swap(minDimension, maxDimenstion);
    }

    // Check if the path is too large for an atlas. Since we use "minDimension" for height in the
    // atlas, limiting to kMaxAtlasPathHeight^2 pixels guarantees height <= kMaxAtlasPathHeight.
    if (maxDimenstion * minDimension > kMaxAtlasPathHeight * kMaxAtlasPathHeight ||
        maxDimenstion > fMaxAtlasPathWidth) {
        return false;
    }

    if (!fAtlas.addRect(maxDimenstion, minDimension, locationInAtlas)) {
        return false;
    }

    SkMatrix atlasMatrix = viewMatrix;
    if (*transposedInAtlas) {
        std::swap(atlasMatrix[0], atlasMatrix[3]);
        std::swap(atlasMatrix[1], atlasMatrix[4]);
        float tx=atlasMatrix.getTranslateX(), ty=atlasMatrix.getTranslateY();
        atlasMatrix.setTranslateX(ty - devIBounds->y() + locationInAtlas->x());
        atlasMatrix.setTranslateY(tx - devIBounds->x() + locationInAtlas->y());
    } else {
        atlasMatrix.postTranslate(locationInAtlas->x() - devIBounds->x(),
                                  locationInAtlas->y() - devIBounds->y());
    }

    // Concatenate this path onto our uber path that matches its fill and AA types.
    SkPath* uberPath = this->getAtlasUberPath(path.getFillType(), GrAAType::kNone != aaType);
    uberPath->moveTo(locationInAtlas->x(), locationInAtlas->y());  // Implicit moveTo(0,0).
    uberPath->addPath(path, atlasMatrix);
    return true;
}

void GrTessellationPathRenderer::onStencilPath(const StencilPathArgs& args) {
    SkPath path;
    args.fShape->asPath(&path);

    GrAAType aaType = (GrAA::kYes == args.fDoStencilMSAA) ? GrAAType::kMSAA : GrAAType::kNone;

    auto op = args.fContext->priv().opMemoryPool()->allocate<GrTessellatePathOp>(
            *args.fViewMatrix, path, GrPaint(), aaType, OpFlags::kStencilOnly);
    args.fRenderTargetContext->addDrawOp(args.fClip, std::move(op));
}

void GrTessellationPathRenderer::preFlush(GrOnFlushResourceProvider* onFlushRP,
                                          const uint32_t* opsTaskIDs, int numOpsTaskIDs) {
    if (!fAtlas.drawBounds().isEmpty()) {
        this->renderAtlas(onFlushRP);
        fAtlas.reset(kAtlasInitialSize, *onFlushRP->caps());
    }
    for (SkPath& path : fAtlasUberPaths) {
        path.reset();
    }
}

constexpr static GrUserStencilSettings kTestStencil(
    GrUserStencilSettings::StaticInit<
        0x0000,
        GrUserStencilTest::kNotEqual,
        0xffff,
        GrUserStencilOp::kKeep,
        GrUserStencilOp::kKeep,
        0xffff>());

constexpr static GrUserStencilSettings kTestAndResetStencil(
    GrUserStencilSettings::StaticInit<
        0x0000,
        GrUserStencilTest::kNotEqual,
        0xffff,
        GrUserStencilOp::kZero,
        GrUserStencilOp::kKeep,
        0xffff>());

void GrTessellationPathRenderer::renderAtlas(GrOnFlushResourceProvider* onFlushRP) {
    auto rtc = fAtlas.instantiate(onFlushRP);
    if (!rtc) {
        return;
    }

    // Add ops to stencil the atlas paths.
    for (auto antialias : {false, true}) {
        for (auto fillType : {SkPathFillType::kWinding, SkPathFillType::kEvenOdd}) {
            SkPath* uberPath = this->getAtlasUberPath(fillType, antialias);
            if (uberPath->isEmpty()) {
                continue;
            }
            uberPath->setFillType(fillType);
            GrAAType aaType = (antialias) ? GrAAType::kMSAA : GrAAType::kNone;
            auto op = onFlushRP->opMemoryPool()->allocate<GrTessellatePathOp>(
                    SkMatrix::I(), *uberPath, GrPaint(), aaType, fStencilAtlasFlags);
            rtc->addDrawOp(nullptr, std::move(op));
        }
    }

    // Finally, draw a fullscreen rect to convert our stencilled paths into alpha coverage masks.
    auto fillRectFlags = GrFillRectOp::InputFlags::kNone;

    // This will be the final op in the renderTargetContext. So if Ganesh is planning to discard the
    // stencil values anyway, then we might not actually need to reset the stencil values back to 0.
    bool mustResetStencil = !onFlushRP->caps()->discardStencilValuesAfterRenderPass();

    if (rtc->numSamples() <= 1) {
        // We are mixed sampled. We need to enable conservative raster and ensure stencil values get
        // reset in order to avoid artifacts along the diagonal of the atlas.
        fillRectFlags |= GrFillRectOp::InputFlags::kConservativeRaster;
        mustResetStencil = true;
    }

    SkRect coverRect = SkRect::MakeIWH(fAtlas.drawBounds().width(), fAtlas.drawBounds().height());
    const GrUserStencilSettings* stencil;
    if (mustResetStencil) {
        // Outset the cover rect in case there are T-junctions in the path bounds.
        coverRect.outset(1, 1);
        stencil = &kTestAndResetStencil;
    } else {
        stencil = &kTestStencil;
    }

    GrQuad coverQuad(coverRect);
    DrawQuad drawQuad{coverQuad, coverQuad, GrQuadAAFlags::kAll};

    GrPaint paint;
    paint.setColor4f(SK_PMColor4fWHITE);

    auto coverOp = GrFillRectOp::Make(rtc->surfPriv().getContext(), std::move(paint),
                                      GrAAType::kMSAA, &drawQuad, stencil, fillRectFlags);
    rtc->addDrawOp(nullptr, std::move(coverOp));

    if (rtc->asSurfaceProxy()->requiresManualMSAAResolve()) {
        onFlushRP->addTextureResolveTask(sk_ref_sp(rtc->asTextureProxy()),
                                         GrSurfaceProxy::ResolveFlags::kMSAA);
    }
}
