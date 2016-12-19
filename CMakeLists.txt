##
#
# The MIT License (MIT)
#
# Copyright © 2017-2019 Ruben Van Boxem
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

cmake_minimum_required (VERSION 3.1)
project(skia)
set (CMAKE_CXX_STANDARD_REQUIRED 17)

if(UNIX AND NOT APPLE)
  find_package(Freetype)
endif()

if(EXPAT_FOUND AND ZLIB_FOUND)
  set(SKIA_ENABLE_PDF TRUE)
endif()

# Because wow Skia, just wow.
if(CMAKE_COMPILER_IS_GNUCXX OR ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
  add_compile_options(-Wno-conversion
                      -Wno-pedantic
                      -Wno-missing-field-initializers
                      -Wno-sign-compare
                      -Wno-unused-parameter
                      -Wno-deprecated-declarations
                      -Wno-narrowing)
  if(NOT ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 7)
      add_compile_options(-Wimplicit-fallthrough=0
                          -Wformat=0
                          -Wno-format-extra-args
                          -Wno-int-in-bool-context
                          -Wno-strict-aliasing
                          -Wno-unused
                          -Wno-ignored-attributes
                          -Wno-unknown-pragmas)
      if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 8)
        add_compile_options(-Wno-sizeof-pointer-memaccess
                            -Wno-class-memaccess
                            -Wno-stringop-overflow
                            -Wno-array-bounds
                            -Wno-restrict
                            -Wno-stringop-truncation
                            -Wno-cast-function-type)
        if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 9)
          add_compile_options(-Wno-redundant-move
                              -Wno-deprecated-copy)
        endif()
      endif()
    endif()
    add_compile_options(-Wno-maybe-uninitialized)
  endif()
elseif(MSVC)
  add_compile_options(/wd4100
                      /wd4125
                      /wd4127
                      /wd4189
                      /wd4201
                      /wd4244
                      /wd4245
                      /wd4251
                      /wd4267
                      /wd4291
                      /wd4310
                      /wd4324
                      /wd4389
                      /wd4456
                      /wd4457
                      /wd4459
                      /wd4505
                      /wd4554
                      /wd4589
                      /wd4611
                      /wd4701
                      /wd4702
                      /wd4703
                      /wd4706
                      /wd4800
                      )
endif()

set(skia_sources
  # src/android/SkAndroidFrameworkUtils.cpp
  # src/android/SkAnimatedImage.cpp
  # src/android/SkBitmapRegionCodec.cpp
  # src/android/SkBitmapRegionDecoder.cpp

  # src/atlastext/SkAtlasTextContext.cpp
  # src/atlastext/SkAtlasTextTarget.cpp
  # src/atlastext/SkInternalAtlasTextContext.cpp

  # src/c/sk_effects.cpp
  # src/c/sk_imageinfo.cpp
  # src/c/sk_paint.cpp
  # src/c/sk_surface.cpp

  # src/codec/SkAndroidCodec.cpp
  # src/codec/SkAndroidCodecAdapter
    src/codec/SkBmpBaseCodec.cpp
    src/codec/SkBmpCodec.cpp
    src/codec/SkBmpMaskCodec.cpp
    src/codec/SkBmpRLECodec.cpp
    src/codec/SkBmpStandardCodec.cpp
    src/codec/SkCodec.cpp
    src/codec/SkCodecImageGenerator.cpp
    src/codec/SkColorTable.cpp
    src/codec/SkEncodedInfo.cpp
    src/codec/SkGifCodec.cpp
  # src/codec/SkHeifCodec.cpp # does not define any new symbols
    src/codec/SkIcoCodec.cpp
    src/codec/SkJpegCodec.cpp
    src/codec/SkJpegDecoderMgr.cpp
    src/codec/SkJpegUtility.cpp
    src/codec/SkMasks.cpp
    src/codec/SkMaskSwizzler.cpp
    src/codec/SkParseEncodedOrigin.cpp
    src/codec/SkPngCodec.cpp
  # src/codec/SkRawCodec.cpp
    src/codec/SkSampledCodec.cpp
    src/codec/SkSampler.cpp
    src/codec/SkStreamBuffer.cpp
    src/codec/SkSwizzler.cpp
    src/codec/SkWbmpCodec.cpp
  # src/codec/SkWebpCodec.cpp
  # src/codec/SkWuffsCodec.cpp

    src/core/SkAAClip.cpp
    src/core/SkAlphaRuns.cpp
    src/core/SkAnalyticEdge.cpp
    src/core/SkAnnotation.cpp
    src/core/SkArenaAlloc.cpp
    src/core/SkATrace.cpp
    src/core/SkAutoPixmapStorage.cpp
    src/core/SkBBHFactory.cpp
    src/core/SkBigPicture.cpp
    src/core/SkBitmap.cpp
    src/core/SkBitmapCache.cpp
    src/core/SkBitmapController.cpp
    src/core/SkBitmapDevice.cpp
    src/core/SkBitmapProcState.cpp
    src/core/SkBitmapProcState_matrixProcs.cpp
    src/core/SkBlendMode.cpp
    src/core/SkBlitRow_D32.cpp
    src/core/SkBlitter.cpp
    src/core/SkBlitter_A8.cpp
    src/core/SkBlitter_ARGB32.cpp
    src/core/SkBlitter_RGB565.cpp
    src/core/SkBlitter_Sprite.cpp
    src/core/SkBlurMask.cpp
    src/core/SkBlurMF.cpp
    src/core/SkBuffer.cpp
    src/core/SkCachedData.cpp
    src/core/SkCanvas.cpp
    src/core/SkCanvasPriv.cpp
    src/core/SkClipStack.cpp
    src/core/SkClipStackDevice.cpp
    src/core/SkColor.cpp
    src/core/SkColorFilter.cpp
    src/core/SkColorFilter_Matrix.cpp
    src/core/SkColorSpace.cpp
    src/core/SkColorSpaceXformSteps.cpp
    src/core/SkContourMeasure.cpp
    src/core/SkConvertPixels.cpp
    src/core/SkCpu.cpp
    src/core/SkCubicClipper.cpp
    src/core/SkCubicMap.cpp
    src/core/SkData.cpp
    src/core/SkDataTable.cpp
  # src/core/SkDebug.cpp # does not define any new symbols
    src/core/SkDeferredDisplayList.cpp
    src/core/SkDeferredDisplayListRecorder.cpp
    src/core/SkDeque.cpp
    src/core/SkDescriptor.cpp
    src/core/SkDevice.cpp
    src/core/SkDistanceFieldGen.cpp
    src/core/SkDocument.cpp
    src/core/SkDraw.cpp
    src/core/SkDraw_atlas.cpp
    src/core/SkDraw_text.cpp
    src/core/SkDraw_vertices.cpp
    src/core/SkDrawable.cpp
    src/core/SkDrawLooper.cpp
    src/core/SkDrawShadowInfo.cpp
    src/core/SkEdge.cpp
    src/core/SkEdgeBuilder.cpp
    src/core/SkEdgeClipper.cpp
    src/core/SkExecutor.cpp
    src/core/SkFlattenable.cpp
    src/core/SkFont.cpp
    src/core/SkFontDescriptor.cpp
    src/core/SkFontLCDConfig.cpp
    src/core/SkFontMgr.cpp
    src/core/SkFontStream.cpp
  # src/core/SkForceCPlusPlusLinking.cpp
    src/core/SkGaussFilter.cpp
    src/core/SkGeometry.cpp
    src/core/SkGlobalInitialization_core.cpp
    src/core/SkGlyph.cpp
    src/core/SkGlyphBuffer.cpp
    src/core/SkGlyphRun.cpp
    src/core/SkGlyphRunPainter.cpp
    src/core/SkGpuBlurUtils.cpp
    src/core/SkGraphics.cpp
    src/core/SkHalf.cpp
    src/core/SkICC.cpp
    src/core/SkImageFilter.cpp
    src/core/SkImageFilterCache.cpp
    src/core/SkImageFilterTypes.cpp
    src/core/SkImageGenerator.cpp
    src/core/SkImageInfo.cpp
    src/core/SkLatticeIter.cpp
    src/core/SkLineClipper.cpp
    src/core/SkLocalMatrixImageFilter.cpp
    src/core/SkMallocPixelRef.cpp
    src/core/SkMask.cpp
    src/core/SkMaskBlurFilter.cpp
    src/core/SkMaskCache.cpp
    src/core/SkMaskFilter.cpp
    src/core/SkMaskGamma.cpp
    src/core/SkMath.cpp
    src/core/SkMatrix.cpp
    src/core/SkMatrix44.cpp
    src/core/SkMatrixImageFilter.cpp
    src/core/SkMD5.cpp
    src/core/SkMiniRecorder.cpp
    src/core/SkMipMap.cpp
    src/core/SkModeColorFilter.cpp
    src/core/SkMultiPictureDraw.cpp
    src/core/SkNormalFlatSource.cpp
    src/core/SkNormalMapSource.cpp
    src/core/SkNormalSource.cpp
    src/core/SkOpts.cpp
    src/core/SkOverdrawCanvas.cpp
    src/core/SkPaint.cpp
    src/core/SkPaintPriv.cpp
    src/core/SkPath.cpp
    src/core/SkPath_serial.cpp
    src/core/SkPathEffect.cpp
    src/core/SkPathMeasure.cpp
    src/core/SkPathRef.cpp
    src/core/SkPicture.cpp
    src/core/SkPictureData.cpp
    src/core/SkPictureFlat.cpp
    src/core/SkPictureImageGenerator.cpp
    src/core/SkPicturePlayback.cpp
    src/core/SkPictureRecord.cpp
    src/core/SkPictureRecorder.cpp
    src/core/SkPixelRef.cpp
    src/core/SkPixmap.cpp
    src/core/SkPoint.cpp
    src/core/SkPoint3.cpp
    src/core/SkPromiseImageTexture.cpp
    src/core/SkPtrRecorder.cpp
    src/core/SkQuadClipper.cpp
    src/core/SkRasterClip.cpp
    src/core/SkRasterPipeline.cpp
    src/core/SkRasterPipelineBlitter.cpp
    src/core/SkReadBuffer.cpp
    src/core/SkRecord.cpp
    src/core/SkRecordDraw.cpp
    src/core/SkRecordedDrawable.cpp
    src/core/SkRecorder.cpp
    src/core/SkRecordOpts.cpp
    src/core/SkRecords.cpp
    src/core/SkRect.cpp
    src/core/SkRegion.cpp
    src/core/SkRegion_path.cpp
    src/core/SkRemoteGlyphCache.cpp
    src/core/SkResourceCache.cpp
    src/core/SkRRect.cpp
    src/core/SkRTree.cpp
    src/core/SkRWBuffer.cpp
    src/core/SkScalar.cpp
    src/core/SkScalerContext.cpp
    src/core/SkScan.cpp
    src/core/SkScan_AAAPath.cpp
    src/core/SkScan_Antihair.cpp
    src/core/SkScan_AntiPath.cpp
    src/core/SkScan_Hairline.cpp
    src/core/SkScan_Path.cpp
    src/core/SkSemaphore.cpp
    src/core/SkSharedMutex.cpp
    src/core/SkSpecialImage.cpp
    src/core/SkSpecialSurface.cpp
    src/core/SkSpinlock.cpp
    src/core/SkSpriteBlitter_ARGB32.cpp
    src/core/SkSpriteBlitter_RGB565.cpp
    src/core/SkStream.cpp
    src/core/SkStrike.cpp
    src/core/SkStrikeCache.cpp
    src/core/SkStrikeForGPU.cpp
    src/core/SkStrikeSpec.cpp
    src/core/SkString.cpp
    src/core/SkStringUtils.cpp
    src/core/SkStroke.cpp
    src/core/SkStrokeRec.cpp
    src/core/SkStrokerPriv.cpp
    src/core/SkSurfaceCharacterization.cpp
    src/core/SkSwizzle.cpp
    src/core/SkTaskGroup.cpp
    src/core/SkTextBlob.cpp
    src/core/SkThreadID.cpp
    src/core/SkTime.cpp
    src/core/SkTLS.cpp
    src/core/SkTSearch.cpp
    src/core/SkTypeface.cpp
    src/core/SkTypeface_remote.cpp
    src/core/SkTypefaceCache.cpp
    src/core/SkUnPreMultiply.cpp
    src/core/SkUtils.cpp
  # src/core/SkUtilsArm.cpp
    src/core/SkVertices.cpp
    src/core/SkVertState.cpp
    src/core/SkVM.cpp
    src/core/SkVMBlitter.cpp
    src/core/SkWriteBuffer.cpp
    src/core/SkWriter32.cpp
    src/core/SkXfermode.cpp
    src/core/SkXfermodeInterpretation.cpp
    src/core/SkYUVASizeInfo.cpp
    src/core/SkYUVMath.cpp
    src/core/SkYUVPlanesCache.cpp

    src/effects/imagefilters/SkAlphaThresholdFilter.cpp
    src/effects/imagefilters/SkArithmeticImageFilter.cpp
    src/effects/imagefilters/SkBlurImageFilter.cpp
    src/effects/imagefilters/SkColorFilterImageFilter.cpp
    src/effects/imagefilters/SkComposeImageFilter.cpp
    src/effects/imagefilters/SkDisplacementMapEffect.cpp
    src/effects/imagefilters/SkDropShadowImageFilter.cpp
    src/effects/imagefilters/SkImageFilters.cpp
    src/effects/imagefilters/SkImageSource.cpp
    src/effects/imagefilters/SkLightingImageFilter.cpp
    src/effects/imagefilters/SkMagnifierImageFilter.cpp
    src/effects/imagefilters/SkMatrixConvolutionImageFilter.cpp
    src/effects/imagefilters/SkMergeImageFilter.cpp
    src/effects/imagefilters/SkMorphologyImageFilter.cpp
    src/effects/imagefilters/SkOffsetImageFilter.cpp
    src/effects/imagefilters/SkPaintImageFilter.cpp
    src/effects/imagefilters/SkPictureImageFilter.cpp
    src/effects/imagefilters/SkTileImageFilter.cpp
    src/effects/imagefilters/SkXfermodeImageFilter.cpp
    src/effects/Sk1DPathEffect.cpp
    src/effects/Sk2DPathEffect.cpp
    src/effects/SkColorMatrix.cpp
    src/effects/SkColorMatrixFilter.cpp
    src/effects/SkCornerPathEffect.cpp
    src/effects/SkDashPathEffect.cpp
    src/effects/SkDiscretePathEffect.cpp
    src/effects/SkEmbossMask.cpp
    src/effects/SkEmbossMaskFilter.cpp
    src/effects/SkHighContrastFilter.cpp
    src/effects/SkLayerDrawLooper.cpp
    src/effects/SkLumaColorFilter.cpp
    src/effects/SkOpPathEffect.cpp
    src/effects/SkOverdrawColorFilter.cpp
    src/effects/SkPackBits.cpp
    src/effects/SkShaderMaskFilter.cpp
    src/effects/SkTableColorFilter.cpp
    src/effects/SkTableMaskFilter.cpp
    src/effects/SkTrimPathEffect.cpp

    src/fonts/SkFontMgr_indirect.cpp
    src/fonts/SkRemotableFontMgr.cpp

    src/gpu/ccpr/GrCCAtlas.cpp
    src/gpu/ccpr/GrCCClipPath.cpp
    src/gpu/ccpr/GrCCClipProcessor.cpp
    src/gpu/ccpr/GrCCConicShader.cpp
    src/gpu/ccpr/GrCCCoverageProcessor.cpp
    src/gpu/ccpr/GrCCCubicShader.cpp
    src/gpu/ccpr/GrCCDrawPathsOp.cpp
    src/gpu/ccpr/GrCCFiller.cpp
    src/gpu/ccpr/GrCCFillGeometry.cpp
    src/gpu/ccpr/GrCCPathCache.cpp
    src/gpu/ccpr/GrCCPathProcessor.cpp
    src/gpu/ccpr/GrCCPerFlushResources.cpp
    src/gpu/ccpr/GrCCQuadraticShader.cpp
    src/gpu/ccpr/GrCCStrokeGeometry.cpp
    src/gpu/ccpr/GrCCStroker.cpp
    src/gpu/ccpr/GrCoverageCountingPathRenderer.cpp
  # src/gpu/ccpr/GrCoverageCountingPathRenderer_none.cpp
    src/gpu/ccpr/GrGSCoverageProcessor.cpp
    src/gpu/ccpr/GrOctoBounds.cpp
    src/gpu/ccpr/GrSampleMaskProcessor.cpp
    src/gpu/ccpr/GrStencilAtlasOp.cpp
    src/gpu/ccpr/GrVSCoverageProcessor.cpp
  # src/gpu/dawn/GrDawnBuffer.cpp
  # src/gpu/dawn/GrDawnCaps.cpp
  # src/gpu/dawn/GrDawnGpu.cpp
  # src/gpu/dawn/GrDawnOpsRenderPass.cpp
  # src/gpu/dawn/GrDawnProgramBuilder.cpp
  # src/gpu/dawn/GrDawnProgramDataManager.cpp
  # src/gpu/dawn/GrDawnRenderTarget.cpp
  # src/gpu/dawn/GrDawnRingBuffer.cpp
  # src/gpu/dawn/GrDawnStagingManager.cpp
  # src/gpu/dawn/GrDawnStencilAttachment.cpp
  # src/gpu/dawn/GrDawnTexture.cpp
  # src/gpu/dawn/GrDawnTextureRenderTarget.cpp
  # src/gpu/dawn/GrDawnUniformHandler.cpp
  # src/gpu/dawn/GrDawnUtil.cpp
    src/gpu/effects/generated/GrAARectEffect.cpp
    src/gpu/effects/generated/GrAlphaThresholdFragmentProcessor.cpp
    src/gpu/effects/generated/GrBlurredEdgeFragmentProcessor.cpp
    src/gpu/effects/generated/GrCircleBlurFragmentProcessor.cpp
    src/gpu/effects/generated/GrCircleEffect.cpp
    src/gpu/effects/generated/GrColorMatrixFragmentProcessor.cpp
    src/gpu/effects/generated/GrComposeLerpEffect.cpp
    src/gpu/effects/generated/GrComposeLerpRedEffect.cpp
    src/gpu/effects/generated/GrConfigConversionEffect.cpp
    src/gpu/effects/generated/GrConstColorProcessor.cpp
    src/gpu/effects/generated/GrEllipseEffect.cpp
    src/gpu/effects/generated/GrLumaColorFilterEffect.cpp
    src/gpu/effects/generated/GrMagnifierEffect.cpp
    src/gpu/effects/generated/GrMixerEffect.cpp
    src/gpu/effects/generated/GrOverrideInputFragmentProcessor.cpp
    src/gpu/effects/generated/GrPremulInputFragmentProcessor.cpp
    src/gpu/effects/generated/GrRectBlurEffect.cpp
    src/gpu/effects/generated/GrRRectBlurEffect.cpp
    src/gpu/effects/generated/GrSaturateProcessor.cpp
    src/gpu/effects/generated/GrSimpleTextureEffect.cpp
    src/gpu/effects/GrBezierEffect.cpp
    src/gpu/effects/GrBicubicEffect.cpp
    src/gpu/effects/GrBitmapTextGeoProc.cpp
    src/gpu/effects/GrConvexPolyEffect.cpp
    src/gpu/effects/GrCoverageSetOpXP.cpp
    src/gpu/effects/GrCustomXfermode.cpp
    src/gpu/effects/GrDisableColorXP.cpp
    src/gpu/effects/GrDistanceFieldGeoProc.cpp
    src/gpu/effects/GrGaussianConvolutionFragmentProcessor.cpp
    src/gpu/effects/GrMatrixConvolutionEffect.cpp
    src/gpu/effects/GrOvalEffect.cpp
    src/gpu/effects/GrPorterDuffXferProcessor.cpp
    src/gpu/effects/GrRRectEffect.cpp
    src/gpu/effects/GrShadowGeoProc.cpp
    src/gpu/effects/GrSkSLFP.cpp
    src/gpu/effects/GrSRGBEffect.cpp
    src/gpu/effects/GrTextureDomain.cpp
    src/gpu/effects/GrXfermodeFragmentProcessor.cpp
    src/gpu/effects/GrYUVtoRGBEffect.cpp
    src/gpu/geometry/GrPathUtils.cpp
    src/gpu/geometry/GrQuad.cpp
    src/gpu/geometry/GrQuadUtils.cpp
    src/gpu/geometry/GrShape.cpp
  # src/gpu/gl/android/GrGLMakeNativeInterface_android.cpp
    src/gpu/gl/builders/GrGLProgramBuilder.cpp
    src/gpu/gl/builders/GrGLShaderStringBuilder.cpp
  # src/gpu/gl/egl/GrGLMakeNativeInterface_egl.cpp
  # src/gpu/gl/glfw/GrGLMakeNativeInterface_glfw.cpp
  # src/gpu/gl/glx/GrGLMakeNativeInterface_glx.cpp
  # src/gpu/gl/iOS/GrGLMakeNativeInterface_iOS.cpp
  # src/gpu/gl/mac/GrGLMakeNativeInterface_mac.cpp
  # src/gpu/gl/win/GrGLMakeNativeInterface_win.cpp
    src/gpu/gl/GrGLAssembleGLESInterfaceAutogen.cpp
    src/gpu/gl/GrGLAssembleGLInterfaceAutogen.cpp
    src/gpu/gl/GrGLAssembleHelpers.cpp
    src/gpu/gl/GrGLAssembleInterface.cpp
    src/gpu/gl/GrGLAssembleWebGLInterfaceAutogen.cpp
    src/gpu/gl/GrGLBuffer.cpp
    src/gpu/gl/GrGLCaps.cpp
    src/gpu/gl/GrGLContext.cpp
    src/gpu/gl/GrGLExtensions.cpp
    src/gpu/gl/GrGLGLSL.cpp
    src/gpu/gl/GrGLGpu.cpp
    src/gpu/gl/GrGLGpuProgramCache.cpp
    src/gpu/gl/GrGLInterfaceAutogen.cpp
    src/gpu/gl/GrGLMakeNativeInterface_none.cpp
    src/gpu/gl/GrGLOpsRenderPass.cpp
    src/gpu/gl/GrGLPath.cpp
    src/gpu/gl/GrGLPathRendering.cpp
    src/gpu/gl/GrGLProgram.cpp
    src/gpu/gl/GrGLProgramDataManager.cpp
    src/gpu/gl/GrGLRenderTarget.cpp
    src/gpu/gl/GrGLSemaphore.cpp
    src/gpu/gl/GrGLStencilAttachment.cpp
    src/gpu/gl/GrGLTexture.cpp
    src/gpu/gl/GrGLTextureRenderTarget.cpp
    src/gpu/gl/GrGLTypesPriv.cpp
    src/gpu/gl/GrGLUniformHandler.cpp
    src/gpu/gl/GrGLUtil.cpp
    src/gpu/gl/GrGLVaryingHandler.cpp
    src/gpu/gl/GrGLVertexArray.cpp
    src/gpu/glsl/GrGLSL.cpp
    src/gpu/glsl/GrGLSLBlend.cpp
    src/gpu/glsl/GrGLSLFragmentProcessor.cpp
    src/gpu/glsl/GrGLSLFragmentShaderBuilder.cpp
    src/gpu/glsl/GrGLSLGeometryProcessor.cpp
    src/gpu/glsl/GrGLSLPrimitiveProcessor.cpp
    src/gpu/glsl/GrGLSLProgramBuilder.cpp
    src/gpu/glsl/GrGLSLProgramDataManager.cpp
    src/gpu/glsl/GrGLSLShaderBuilder.cpp
    src/gpu/glsl/GrGLSLUtil.cpp
    src/gpu/glsl/GrGLSLVarying.cpp
    src/gpu/glsl/GrGLSLVertexGeoBuilder.cpp
    src/gpu/glsl/GrGLSLXferProcessor.cpp
    src/gpu/gradients/generated/GrClampedGradientEffect.cpp
    src/gpu/gradients/generated/GrDualIntervalGradientColorizer.cpp
    src/gpu/gradients/generated/GrLinearGradientLayout.cpp
    src/gpu/gradients/generated/GrRadialGradientLayout.cpp
    src/gpu/gradients/generated/GrSingleIntervalGradientColorizer.cpp
    src/gpu/gradients/generated/GrSweepGradientLayout.cpp
    src/gpu/gradients/generated/GrTextureGradientColorizer.cpp
    src/gpu/gradients/generated/GrTiledGradientEffect.cpp
    src/gpu/gradients/generated/GrTwoPointConicalGradientLayout.cpp
    src/gpu/gradients/generated/GrUnrolledBinaryGradientColorizer.cpp
    src/gpu/gradients/GrGradientBitmapCache.cpp
    src/gpu/gradients/GrGradientShader.cpp
    src/gpu/mock/GrMockCaps.cpp
    src/gpu/mock/GrMockGpu.cpp
    src/gpu/mock/GrMockTypes.cpp
  # src/gpu/mtl/GrMtlBuffer.mm
  # src/gpu/mtl/GrMtlCaps.mm
  # src/gpu/mtl/GrMtlCommandBuffer.mm
  # src/gpu/mtl/GrMtlDepthStencil.mm
  # src/gpu/mtl/GrMtlGpu.mm
  # src/gpu/mtl/GrMtlOpsRenderPass.mm
  # src/gpu/mtl/GrMtlPipelineState.mm
  # src/gpu/mtl/GrMtlPipelineStateBuilder.mm
  # src/gpu/mtl/GrMtlPipelineStateDataManager.mm
  # src/gpu/mtl/GrMtlRenderTarget.mm
  # src/gpu/mtl/GrMtlResourceProvider.mm
  # src/gpu/mtl/GrMtlSampler.mm
  # src/gpu/mtl/GrMtlSemaphore.mm
  # src/gpu/mtl/GrMtlStencilAttachment.mm
  # src/gpu/mtl/GrMtlTexture.mm
  # src/gpu/mtl/GrMtlTexturedRenderTarget.mm
  # src/gpu/mtl/GrMtlTrampoline.mm
  # src/gpu/mtl/GrMtlUniformHandler.mm
  # src/gpu/mtl/GrMtlUtil.mm
  # src/gpu/mtl/GrMtlVaryingHandler.mm
    src/gpu/ops/GrAAConvexPathRenderer.cpp
    src/gpu/ops/GrAAConvexTessellator.cpp
    src/gpu/ops/GrAAHairLinePathRenderer.cpp
    src/gpu/ops/GrAALinearizingConvexPathRenderer.cpp
    src/gpu/ops/GrAtlasTextOp.cpp
    src/gpu/ops/GrClearOp.cpp
    src/gpu/ops/GrClearStencilClipOp.cpp
    src/gpu/ops/GrDashLinePathRenderer.cpp
    src/gpu/ops/GrDashOp.cpp
    src/gpu/ops/GrDebugMarkerOp.cpp
    src/gpu/ops/GrDefaultPathRenderer.cpp
    src/gpu/ops/GrDrawableOp.cpp
    src/gpu/ops/GrDrawAtlasOp.cpp
    src/gpu/ops/GrDrawPathOp.cpp
    src/gpu/ops/GrDrawVerticesOp.cpp
    src/gpu/ops/GrFillRectOp.cpp
    src/gpu/ops/GrFillRRectOp.cpp
    src/gpu/ops/GrLatticeOp.cpp
    src/gpu/ops/GrMeshDrawOp.cpp
    src/gpu/ops/GrOp.cpp
    src/gpu/ops/GrOvalOpFactory.cpp
    src/gpu/ops/GrQuadPerEdgeAA.cpp
    src/gpu/ops/GrRegionOp.cpp
    src/gpu/ops/GrShadowRRectOp.cpp
    src/gpu/ops/GrSimpleMeshDrawOpHelper.cpp
    src/gpu/ops/GrSmallPathRenderer.cpp
    src/gpu/ops/GrStencilAndCoverPathRenderer.cpp
    src/gpu/ops/GrStencilPathOp.cpp
    src/gpu/ops/GrStrokeRectOp.cpp
    src/gpu/ops/GrTessellatingPathRenderer.cpp
    src/gpu/ops/GrTextureOp.cpp
    src/gpu/text/GrAtlasManager.cpp
    src/gpu/text/GrDistanceFieldAdjustTable.cpp
    src/gpu/text/GrSDFMaskFilter.cpp
    src/gpu/text/GrStrikeCache.cpp
    src/gpu/text/GrTextBlob.cpp
    src/gpu/text/GrTextBlobCache.cpp
    src/gpu/text/GrTextBlobVertexRegenerator.cpp
    src/gpu/text/GrTextContext.cpp
  # src/gpu/vk/GrVkAMDMemoryAllocator.cpp
  # src/gpu/vk/GrVkBuffer.cpp
  # src/gpu/vk/GrVkBufferView.cpp
  # src/gpu/vk/GrVkCaps.cpp
  # src/gpu/vk/GrVkCommandBuffer.cpp
  # src/gpu/vk/GrVkCommandPool.cpp
  # src/gpu/vk/GrVkDescriptorPool.cpp
  # src/gpu/vk/GrVkDescriptorSet.cpp
  # src/gpu/vk/GrVkDecriptorSetManager.cpp
  # src/gpu/vk/GrVkExtensions.cpp
  # src/gpu/vk/GrVkFrameBuffer.cpp
  # src/gpu/vk/GrVkGpu.cpp
  # src/gpu/vk/GrVkImage.cpp
  # src/gpu/vk/GrVkImageView.cpp
  # src/gpu/vk/GrVkIndexBuffer.cpp
  # src/gpu/vk/GrVkInterface.cpp
  # src/gpu/vk/GrVkMemory.cpp
  # src/gpu/vk/GrVkOpsRenderPass.cpp
  # src/gpu/vk/GrVkPipeline.cpp
  # src/gpu/vk/GrVkPipelineState.cpp
  # src/gpu/vk/GrVkPipelineStateBuilder.cpp
  # src/gpu/vk/GrVkPipelineStateCache.cpp
  # src/gpu/vk/GrVkPipelineStateDataManager.cpp
  # src/gpu/vk/GrVkRenderPass.cpp
  # src/gpu/vk/GrVkRenderTarget.cpp
  # src/gpu/vk/GrVkResourceProvider.cpp
  # src/gpu/vk/GrVkSampler.cpp
  # src/gpu/vk/GrVkSamplerYcbcrConversion.cpp
  # src/gpu/vk/grVkSecondaryCBDrawContext.cpp
  # src/gpu/vk/GrVkSemaphore.cpp
  # src/gpu/vk/GrVkStencilAttachment.cpp
  # src/gpu/vk/GrVkTexture.cpp
  # src/gpu/vk/GrVkTextureRenderTarget.cpp
  # src/gpu/vk/GrVkTransferBuffer.cpp
  # src/gpu/vk/GrVkTypesPriv.cpp
  # src/gpu/vk/GrVkUniformBuffer.cpp
  # src/gpu/vk/GrVkUniformHandler.cpp
  # src/gpu/vk/GrVkUtil.cpp
  # src/gpu/vk/GrVkVaryingHandler.cpp
  # src/gpu/vk/GrVkVertexBuffer.cpp
  # src/gpu/GrAHardwareBufferImageGenerator.cpp # does not define any new symbols
    src/gpu/GrAHardwareBufferUtils.cpp
    src/gpu/GrAuditTrail.cpp
    src/gpu/GrBackendSurface.cpp
    src/gpu/GrBackendTextureImageGenerator.cpp
    src/gpu/GrBitmapTextureMaker.cpp
    src/gpu/GrBlurUtils.cpp
    src/gpu/GrBufferAllocPool.cpp
    src/gpu/GrCaps.cpp
    src/gpu/GrClientMappedBufferManager.cpp
    src/gpu/GrClipStackClip.cpp
    src/gpu/GrColorInfo.cpp
    src/gpu/GrColorSpaceXform.cpp
    src/gpu/GrCopyRenderTask.cpp
    src/gpu/GrContext.cpp
    src/gpu/GrContext_Base.cpp
    src/gpu/GrContextPriv.cpp
    src/gpu/GrContextThreadSafeProxy.cpp
    src/gpu/GrDataUtils.cpp
    src/gpu/GrDDLContext.cpp
    src/gpu/GrDefaultGeoProcFactory.cpp
    src/gpu/GrDistanceFieldGenFromVector.cpp
    src/gpu/GrDrawingManager.cpp
    src/gpu/GrDrawOpAtlas.cpp
  # src/gpu/GrDrawOpTest.cpp # defines no new symbols
    src/gpu/GrDriverBugWorkarounds.cpp
    src/gpu/GrFixedClip.cpp
    src/gpu/GrFragmentProcessor.cpp
    src/gpu/GrGpu.cpp
    src/gpu/GrGpuBuffer.cpp
    src/gpu/GrGpuResource.cpp
    src/gpu/GrImageContext.cpp
    src/gpu/GrImageTextureMaker.cpp
    src/gpu/GrLegacyDirectContext.cpp
    src/gpu/GrMemoryPool.cpp
    src/gpu/GrOnFlushResourceProvider.cpp
    src/gpu/GrOpFlushState.cpp
    src/gpu/GrOpsRenderPass.cpp
    src/gpu/GrOpsTask.cpp
    src/gpu/GrPaint.cpp
    src/gpu/GrPath.cpp
    src/gpu/GrPathProcessor.cpp
    src/gpu/GrPathRenderer.cpp
    src/gpu/GrPathRendererChain.cpp
    src/gpu/GrPathRendering.cpp
  # src/gpu/GrPathRendering_none.cpp
    src/gpu/GrPipeline.cpp
    src/gpu/GrPrimitiveProcessor.cpp
    src/gpu/GrProcessor.cpp
    src/gpu/GrProcessorAnalysis.cpp
    src/gpu/GrProcessorSet.cpp
  # src/gpu/GrProcessorUnitTest.cpp # defined no new symbols
    src/gpu/GrProgramDesc.cpp
    src/gpu/GrProgramInfo.cpp
    src/gpu/GrProxyProvider.cpp
    src/gpu/GrRecordingContext.cpp
    src/gpu/GrRectanizer_pow2.cpp
    src/gpu/GrRectanizer_skyline.cpp
    src/gpu/GrReducedClip.cpp
    src/gpu/GrRenderTarget.cpp
    src/gpu/GrRenderTargetContext.cpp
    src/gpu/GrRenderTargetProxy.cpp
    src/gpu/GrRenderTask.cpp
    src/gpu/GrResourceAllocator.cpp
    src/gpu/GrResourceCache.cpp
    src/gpu/GrResourceProvider.cpp
    src/gpu/GrSamplePatternDictionary.cpp
    src/gpu/GrShaderCaps.cpp
    src/gpu/GrShaderUtils.cpp
    src/gpu/GrShaderVar.cpp
    src/gpu/GrSoftwarePathRenderer.cpp
    src/gpu/GrStencilAttachment.cpp
    src/gpu/GrStencilSettings.cpp
    src/gpu/GrStyle.cpp
    src/gpu/GrSurface.cpp
    src/gpu/GrSurfaceContext.cpp
    src/gpu/GrSurfaceProxy.cpp
    src/gpu/GrSwizzle.cpp
    src/gpu/GrSWMaskHelper.cpp
    src/gpu/GrTessellator.cpp
  # src/gpu/GrTestUtils.cpp # defines no new symbols
    src/gpu/GrTexture.cpp
    src/gpu/GrTextureAdjuster.cpp
    src/gpu/GrTextureContext.cpp
    src/gpu/GrTextureMaker.cpp
    src/gpu/GrTextureProducer.cpp
    src/gpu/GrTextureProxy.cpp
    src/gpu/GrTextureRenderTargetProxy.cpp
    src/gpu/GrTextureResolveRenderTask.cpp
    src/gpu/GrTransferFromRenderTask.cpp
    src/gpu/GrWaitRenderTask.cpp
    src/gpu/GrXferProcessor.cpp
    src/gpu/GrYUVProvider.cpp
    src/gpu/SkGpuDevice.cpp
    src/gpu/SkGpuDevice_drawTexture.cpp
    src/gpu/SkGr.cpp

    src/image/SkImage.cpp
    src/image/SkImage_Gpu.cpp
    src/image/SkImage_GpuBase.cpp
    src/image/SkImage_GpuYUVA.cpp
    src/image/SkImage_Lazy.cpp
    src/image/SkImage_Raster.cpp
    src/image/SkSurface.cpp
    src/image/SkSurface_Raster.cpp
    src/image/SkSurface_Gpu.cpp

    src/images/SkImageEncoder.cpp
  # src/images/SkJpegEncoder.cpp # defines no new symbols
    src/images/SkJPEGWriteUtility.cpp
    src/images/SkPngEncoder.cpp
  # src/images/SkWebpEncoder.cpp

    src/lazy/SkDiscardableMemoryPool.cpp

  # src/opts/SkOpts_avx.cpp
  # src/opts/SkOpts_crc32.cpp
    src/opts/SkOpts_hsw.cpp
  # src/opts/SkOpts_sse41.cpp
  # src/opts/SkOpts_sse42.cpp
  # src/opts/SkOpts_ssse3.cpp

    src/pathops/SkAddIntersections.cpp
    src/pathops/SkDConicLineIntersection.cpp
    src/pathops/SkDCubicLineIntersection.cpp
    src/pathops/SkDCubicToQuads.cpp
    src/pathops/SkDLineIntersection.cpp
    src/pathops/SkDQuadLineIntersection.cpp
    src/pathops/SkIntersections.cpp
    src/pathops/SkOpAngle.cpp
    src/pathops/SkOpBuilder.cpp
    src/pathops/SkOpCoincidence.cpp
    src/pathops/SkOpContour.cpp
    src/pathops/SkOpCubicHull.cpp
    src/pathops/SkOpEdgeBuilder.cpp
    src/pathops/SkOpSegment.cpp
    src/pathops/SkOpSpan.cpp
    src/pathops/SkPathOpsAsWinding.cpp
    src/pathops/SkPathOpsCommon.cpp
    src/pathops/SkPathOpsConic.cpp
    src/pathops/SkPathOpsCubic.cpp
    src/pathops/SkPathOpsCurve.cpp
    src/pathops/SkPathOpsDebug.cpp
    src/pathops/SkPathOpsLine.cpp
    src/pathops/SkPathOpsOp.cpp
    src/pathops/SkPathOpsQuad.cpp
    src/pathops/SkPathOpsRect.cpp
    src/pathops/SkPathOpsSimplify.cpp
    src/pathops/SkPathOpsTightBounds.cpp
    src/pathops/SkPathOpsTSect.cpp
    src/pathops/SkPathOpsTypes.cpp
    src/pathops/SkPathOpsWinding.cpp
    src/pathops/SkPathWriter.cpp
    src/pathops/SkReduceOrder.cpp

    src/pdf/SkClusterator.cpp
    src/pdf/SkDeflate.cpp
  # src/pdf/SkDocument_PDF_None.cpp
  # src/pdf/SkJpegInfo.cpp # already defined in SkJpegCodec
    src/pdf/SkKeyedImage.cpp
    src/pdf/SkPDFBitmap.cpp
    src/pdf/SkPDFDevice.cpp
    src/pdf/SkPDFDocument.cpp
    src/pdf/SkPDFFont.cpp
    src/pdf/SkPDFFormXObject.cpp
    src/pdf/SkPDFGradientShader.cpp
    src/pdf/SkPDFGraphicStackState.cpp
    src/pdf/SkPDFGraphicState.cpp
    src/pdf/SkPDFMakeCIDGlyphWidthsArray.cpp
    src/pdf/SkPDFMakeToUnicodeCmap.cpp
    src/pdf/SkPDFMetadata.cpp
    src/pdf/SkPDFResourceDict.cpp
    src/pdf/SkPDFShader.cpp
    src/pdf/SkPDFSubsetFont.cpp
    src/pdf/SkPDFTag.cpp
    src/pdf/SkPDFType1Font.cpp
    src/pdf/SkPDFTypes.cpp
    src/pdf/SkPDFUtils.cpp

  # src/ports/SkDebug_android.cpp
  # src/ports/SkDebug_stdio.cpp
  # src/ports/SkDebug_win.cpp
    src/ports/SkDiscardableMemory_none.cpp
  # src/ports/SkFontConfigInterface.cpp
  # src/ports/SkFontConfigInterface_direct.cpp
  # src/ports/SkFontConfigInterface_direct_factory.cpp
  # src/ports/SkFontHost_FreeType.cpp
  # src/ports/SkFontHost_FreeType_common.cpp
  # src/ports/SkFontHost_mac.cpp
  # src/ports/SkFontHost_win.cpp
  # src/ports/SkFontMgr_android.cpp
  # src/ports/SkFontMgr_android_factory.cpp
  # src/ports/SkFontMgr_android_parser.cpp
    src/ports/SkFontMgr_custom.cpp
    src/ports/SkFontMgr_custom_directory.cpp
  # src/ports/SkFontMgr_custom_directory_factory.cpp
  # src/ports/SkFontMgr_custom_embedded_factory.cpp
  # src/ports/SkFontMgr_custom_embedded.cpp
  # src/ports/SkFontMgr_custom_empty_factory.cpp
  # src/ports/SkFontMgr_custom_empty.cpp
  # src/ports/SkFontMgr_empty_factory.cpp
  # src/ports/SkFontMgr_fontconfig.cpp
  # src/ports/SkFontMgr_fontconfig_factory.cpp
  # src/ports/SkFontMgr_FontConfigInterface.cpp
  # src/ports/SkFontMgr_FontConfigInterface_factory.cpp
  # src/ports/SkFontMgr_fuchsia.cpp
  # src/ports/SkFontMgr_win_dw_factory.cpp
  # src/ports/SkFontMgr_win_dw.cpp
    src/ports/SkGlobalInitialization_default.cpp
  # src/ports/SkImageEncoder_CG.cpp
  # src/ports/SkImageEncoder_WIC.cpp
  # src/ports/SkImageGenerator_none.cpp
    src/ports/SkImageGenerator_skia.cpp
  # src/ports/SkImageGeneratorCG.cpp
  # src/ports/SkImageGeneratorWIC.cpp
    src/ports/SkMemory_malloc.cpp
  # src/ports/SkMemory_mozalloc.cpp
  # src/ports/SkOSFile_posix.cpp
    src/ports/SkOSFile_stdio.cpp
  # src/ports/SkOSFile_win.cpp
  # src/ports/SkOSLibrary_posix.cpp
  # src/ports/SkOSLibrary_win.cpp
  # src/ports/SkRemotableFontMgr_win_dw.cpp
  # src/ports/SkScalerContext_win_dw.cpp
  # src/ports/SkTLS_none.cpp
  # src/ports/SkTLS_pthread.cpp
  # src/ports/SkTLS_win.cpp
  # src/ports/SkTypeface_win_dw.cpp

    src/sfnt/SkOTTable_name.cpp
    src/sfnt/SkOTUtils.cpp

    src/shaders/gradients/Sk4fGradientBase.cpp
    src/shaders/gradients/Sk4fLinearGradient.cpp
    src/shaders/gradients/SkGradientShader.cpp
    src/shaders/gradients/SkLinearGradient.cpp
    src/shaders/gradients/SkRadialGradient.cpp
    src/shaders/gradients/SkSweepGradient.cpp
    src/shaders/gradients/SkTwoPointConicalGradient.cpp
    src/shaders/SkBitmapProcShader.cpp
    src/shaders/SkColorFilterShader.cpp
    src/shaders/SkColorShader.cpp
    src/shaders/SkComposeShader.cpp
    src/shaders/SkImageShader.cpp
    src/shaders/SkLightingShader.cpp
    src/shaders/SkLights.cpp
    src/shaders/SkLocalMatrixShader.cpp
    src/shaders/SkPerlinNoiseShader.cpp
    src/shaders/SkPictureShader.cpp
    src/shaders/SkRTShader.cpp
    src/shaders/SkShader.cpp

    src/sksl/ir/SkSLSetting.cpp
    src/sksl/ir/SkSLSymbolTable.cpp
    src/sksl/ir/SkSLType.cpp
    src/sksl/ir/SkSLVariableReference.cpp
  # src/sksl/lex/Main.cpp
    src/sksl/lex/NFA.cpp
    src/sksl/lex/RegexNode.cpp
    src/sksl/lex/RegexParser.cpp
    src/sksl/SkSLASTNode.cpp
    src/sksl/SkSLByteCode.cpp
    src/sksl/SkSLByteCodeGenerator.cpp
    src/sksl/SkSLCFGGenerator.cpp
    src/sksl/SkSLCompiler.cpp
    src/sksl/SkSLCPPCodeGenerator.cpp
    src/sksl/SkSLCPPUniformCTypes.cpp
    src/sksl/SkSLGLSLCodeGenerator.cpp
    src/sksl/SkSLHCodeGenerator.cpp
    src/sksl/SkSLIRGenerator.cpp
    src/sksl/SkSLLexer.cpp
  # src/sksl/SkSLMain.cpp
    src/sksl/SkSLMetalCodeGenerator.cpp
    src/sksl/SkSLOutputStream.cpp
    src/sksl/SkSLParser.cpp
    src/sksl/SkSLPipelineStageCodeGenerator.cpp
    src/sksl/SkSLSectionAndParameterHelper.cpp
    src/sksl/SkSLSPIRVCodeGenerator.cpp
    src/sksl/SkSLString.cpp
    src/sksl/SkSLUtil.cpp

    src/svg/SkSVGCanvas.cpp
    src/svg/SkSVGDevice.cpp

  # src/utils/mac/SkCreateCGImageRef.cpp
  # src/utils/mac/SkStream_mac.cpp
  # src/utils/win/SkAutoCoInitialize.cpp
  # src/utils/win/SkDWrite.cpp
  # src/utils/win/SkDWriteFontFileStream.cpp
  # src/utils/win/SkDWriteGeometrySink.cpp
  # src/utils/win/SkHRESULT.cpp
  # src/utils/win/SkIStream.cpp
  # src/utils/win/SkWGL_win.cpp
    src/utils/Sk3D.cpp
    src/utils/SkAnimCodecPlayer.cpp
    src/utils/SkBase64.cpp
    src/utils/SkCamera.cpp
    src/utils/SkCanvasStack.cpp
    src/utils/SkCanvasStateUtils.cpp
    src/utils/SkCharToGlyphCache.cpp
    src/utils/SkDashPath.cpp
    src/utils/SkEventTracer.cpp
    src/utils/SkFloatToDecimal.cpp
    src/utils/SkFrontBufferedStream.cpp
    src/utils/SkInterpolator.cpp
    src/utils/SkJSON.cpp
    src/utils/SkJSONWriter.cpp
  # src/utils/SkLua.cpp
  # src/utils/SkLuaCanvas.cpp
    src/utils/SkMatrix22.cpp
    src/utils/SkMultiPictureDocument.cpp
    src/utils/SkNullCanvas.cpp
    src/utils/SkNWayCanvas.cpp
    src/utils/SkOSPath.cpp
    src/utils/SkPaintFilterCanvas.cpp
    src/utils/SkParse.cpp
    src/utils/SkParseColor.cpp
    src/utils/SkParsePath.cpp
    src/utils/SkPatchUtils.cpp
    src/utils/SkPolyUtils.cpp
    src/utils/SkShadowTessellator.cpp
    src/utils/SkShadowUtils.cpp
    src/utils/SkTextUtils.cpp
  # src/utils/SkThreadUtils_pthread.cpp
  # src/utils/SkThreadUtils_win.cpp
    src/utils/SkUTF.cpp
    src/utils/SkWhitelistTypefaces.cpp

    src/xml/SkDOM.cpp
    src/xml/SkXMLParser.cpp
    src/xml/SkXMLWriter.cpp

  # xps/SkXPSDevice.cpp
  # xps/SkXPSDocument.cpp

    third_party/etc1/etc1.cpp

    third_party/gif/SkGifImageReader.cpp

    third_party/icu/SkLoadICU.cpp

    third_party/skcms/skcms.cc

  # third_party/vulkanmemoryallocator/GrVulkanMemoryAllocator.cpp
    )

add_definitions(-DSK_HAS_PNG_LIBRARY)
if(LUA_FOUND)
  list(APPEND skia_sources
              src/utils/SkLua.cpp
              src/utils/SkLuaCanvas.cpp
              )
  list(APPEND OTHER_LIBRARIES ${LUA_LIBRARIES})
endif()
if(WEBP_FOUND)
  list(APPEND skia_sources
              src/codec/SkWebpAdapterCodec.cpp
              src/codec/SkWebpCodec.cpp
              src/images/SkWEBPImageEncoder.cpp
              )
  add_definitions(-DSK_HAS_WEBP_LIBRARY)
  list(APPEND OTHER_LIBRARIES ${WEBP_LIBRARIES})
endif()
if(VULKAN_FOUND)
    list(APPEND skia_sources
                src/gpu/vk/GrVkBackendContext.cpp
                src/gpu/vk/GrVkBuffer.cpp
                src/gpu/vk/GrVkCaps.cpp
                src/gpu/vk/GrVkCommandBuffer.cpp
                src/gpu/vk/GrVkCopyManager.cpp
                src/gpu/vk/GrVkCopyPipeline.cpp
                src/gpu/vk/GrVkDescriptorPool.cpp
                src/gpu/vk/GrVkDescriptorSet.cpp
                src/gpu/vk/GrVkDescriptorSetManager.cpp
                src/gpu/vk/GrVkExtensions.cpp
                src/gpu/vk/GrVkFrameBuffer.cpp
                src/gpu/vk/GrVkGpu.cpp
                src/gpu/vk/GrVkGpuCommandBuffer.cpp
                src/gpu/vk/GrVkImage.cpp
                src/gpu/vk/GrVkImageView.cpp
                src/gpu/vk/GrVkIndexBuffer.cpp
                src/gpu/vk/GrVkInterface.cpp
                src/gpu/vk/GrVkMemory.cpp
                src/gpu/vk/GrVkPipeline.cpp
                src/gpu/vk/GrVkPipelineState.cpp
                src/gpu/vk/GrVkPipelineStateBuilder.cpp
                src/gpu/vk/GrVkPipelineStateCache.cpp
                src/gpu/vk/GrVkPipelineStateDataManager.cpp
                src/gpu/vk/GrVkRenderPass.cpp
                src/gpu/vk/GrVkRenderTarget.cpp
                src/gpu/vk/GrVkResourceProvider.cpp
                src/gpu/vk/GrVkSampler.cpp
                src/gpu/vk/GrVkStencilAttachment.cpp
                src/gpu/vk/GrVkTexture.cpp
                src/gpu/vk/GrVkTextureRenderTarget.cpp
                src/gpu/vk/GrVkTransferBuffer.cpp
                src/gpu/vk/GrVkUniformBuffer.cpp
                src/gpu/vk/GrVkUniformHandler.cpp
                src/gpu/vk/GrVkUtil.cpp
                src/gpu/vk/GrVkVaryingHandler.cpp
                src/gpu/vk/GrVkVertexBuffer.cpp
                )
  list(APPEND OTHER_LIBRARIES ${VULKAN_LIBRARIES})
endif()

# OS specific files
if(WIN32)
  list(APPEND skia_sources
            # src/gpu/gl/win/GrGLMakeNativeInterface_win.cpp
              src/ports/SkDebug_win.cpp
              src/ports/SkFontHost_win.cpp
              src/ports/SkFontMgr_win_dw.cpp
              src/ports/SkFontMgr_win_dw_factory.cpp
              src/ports/SkOSFile_win.cpp
              src/ports/SkOSLibrary_win.cpp
              src/ports/SkRemotableFontMgr_win_dw.cpp
              src/ports/SkScalerContext_win_dw.cpp
              src/utils/SkThreadUtils_win.cpp
              src/ports/SkTLS_win.cpp
              src/ports/SkTypeface_win_dw.cpp
              src/utils/win/SkAutoCoInitialize.cpp
              src/utils/win/SkDWrite.cpp
              src/utils/win/SkDWriteFontFileStream.cpp
              src/utils/win/SkDWriteGeometrySink.cpp
              src/utils/win/SkHRESULT.cpp
              src/utils/win/SkIStream.cpp
              src/utils/win/SkWGL_win.cpp
              )
endif()
if(UNIX AND NOT MINGW)
  list(APPEND skia_sources
              src/ports/SkDebug_stdio.cpp
              src/ports/SkOSFile_posix.cpp
              src/ports/SkOSLibrary_posix.cpp
              src/utils/SkThreadUtils_pthread.cpp
              src/ports/SkTLS_pthread.cpp
             )
  if(APPLE)
    list(APPEND skia_sources
                #src/gpu/gl/mac/GrGLMakeNativeInterface_mac.cpp
                src/ports/SkFontHost_mac.cpp
                src/ports/SkImageEncoder_CG.cpp
                src/ports/SkImageGeneratorCG.cpp
                src/utils/mac/SkStream_mac.cpp
               )
    if(ARM)
      list(APPEND skia_sources
                  #src/gpu/gl/iOS/GrGLMakeNativeInterface_iOS.cpp
                  )
    endif()
  elseif(ANDROID)
    list(APPEND skia_sources
                src/codec/SkAndroidCodec.cpp
                src/codec/SkSampledCodec.cpp
                #src/gpu/gl/android/GrGLMakeNativeInterface_android.cpp
                src/ports/SkDebug_android.cpp
                src/ports/SkFontHost_android.cpp
                src/ports/SkFontMgr_android.cpp
                src/ports/SkFontMgr_android_factory.cpp
                src/ports/SkFontMgr_android_parser.cpp
                )
  else() # currently assume linux
    list(APPEND skia_sources
                src/ports/SkFontConfigInterface_direct_factory.cpp
                src/ports/SkFontConfigInterface_direct.cpp
                src/ports/SkFontConfigInterface.cpp
                src/ports/SkFontHost_FreeType_common.cpp
                src/ports/SkFontHost_FreeType.cpp
                src/ports/SkFontMgr_fontconfig_factory.cpp
                src/ports/SkFontMgr_fontconfig.cpp
                src/ports/SkFontMgr_FontConfigInterface_factory.cpp
                src/ports/SkFontMgr_FontConfigInterface.cpp
               )
  endif()
endif()

# CPU specific files
if(NOT ARM)
  list(APPEND skia_sources
              src/opts/SkOpts_avx.cpp
              src/opts/SkOpts_crc32.cpp
              src/opts/SkOpts_sse41.cpp
              src/opts/SkOpts_sse42.cpp
              src/opts/SkOpts_ssse3.cpp
              )
  if(MINGW)
    set_source_files_properties(third_party/skcms/skcms.cc PROPERTIES COMPILE_DEFINITIONS SKCMS_PORTABLE)
  endif()
  if((NOT MSVC) OR (${CMAKE_CXX_COMPILER_ID} MATCHES "Clang"))
    set_source_files_properties(src/core/SkCpu.cpp PROPERTIES COMPILE_FLAGS -mavx)
  endif()
elseif(ARM)
  list(APPEND skia_sources
              src/core/SkUtilsArm.cpp
              src/opts/SkBitmapProcState_arm_neon.cpp
              src/opts/SkBitmapProcState_matrixProcs_neon.cpp
              src/opts/SkBitmapProcState_opts_arm.cpp
              src/opts/SkBlitMask_opts_arm.cpp
              src/opts/SkBlitMask_opts_arm_neon.cpp
              src/opts/SkBlitRow_opts_arm.cpp
              src/opts/SkBlitRow_opts_arm_neon.cpp
              )
endif()

file(GLOB_RECURSE skia_headers
                  src/*.h
                  include/*.h)
if(SKIA_SHARED)
  add_library(skia SHARED
                   ${skia_sources} ${skia_headers}
                   )
  target_compile_definitions(skia PRIVATE -DSKIA_DLL)
elseif(SKIA_STATIC)
  add_library(skia STATIC
                   ${skia_sources} ${skia_headers}
                   )
endif()
target_compile_definitions(skia PRIVATE SKIA_IMPLEMENTATION=1)
target_compile_definitions(skia PUBLIC $<$<CONFIG:Debug>:SK_DEBUG>)
target_link_libraries(skia PUBLIC ${OTHER_LIBRARIES}
                                  png expat zlib gif jpeg-turbo
                                  $<$<NOT:$<PLATFORM_ID:Windows>>:-ldl>)

if(WIN32)
  target_link_libraries(skia PUBLIC usp10)
  target_compile_definitions(skia PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
elseif(APPLE)
  target_link_libraries(skia PRIVATE "-framework CoreText"
                                     "-framework CoreGraphics"
                                     "-framework Foundation")
elseif(UNIX AND NOT MINGW)
  target_link_libraries(skia PUBLIC ${FREETYPE_LIBRARIES} fontconfig)
  target_include_directories(skia PRIVATE ${FREETYPE_INCLUDE_DIRS})
endif()
target_include_directories(skia SYSTEM
                             PRIVATE
                               #.
                               #include/private
                               #src/atlastext
                               #src/codec
                               #src/compute
                               #src/core
                               #src/fonts
                               #src/gpu
                               #src/image
                               #src/images
                               #src/lazy
                               #src/opts
                               #src/pathops
                               #src/pdf
                               #src/ports
                               #src/sfnt
                               #src/shaders
                               #src/shaders/gradients
                               #src/sksl
                               #src/utils
                               #src/utils/win
                               #src/xml
                               #third_party/gif
                               include/third_party/skcms
                             PUBLIC
                               .
                               #include/atlastext
                               #include/codec
                               #include/config
                               #include/core
                               #include/docs
                               #include/effects
                               #include/encode
                               #include/gpu
                               #include/gpu/gl
                               #include/gpu/mock
                               #include/gpu/mtl
                               #include/gpu/vk
                               #include/pathops
                               #include/ports
                               #include/svg
                               #include/utils
                               #include/utils/mac
                               )

## tests
#function(skia_test test_name)
#  add_executable(${test_name} tests/${test_name}.cpp)
#  target_include_directories(${test_name} PUBLIC tests)
#  target_link_libraries(${test_name} skia)
#  add_test(remove/${test_name} ${CMAKE_COMMAND} -E remove ${test_name}${CMAKE_EXECUTABLE_SUFFIX})
#  add_test(build/${test_name} ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${test_name})
#  add_test(NAME    run/${test_name}
#           COMMAND ${test_name})
#  set_tests_properties(run/${test_name} PROPERTIES DEPENDS build/${test_name})
#  #TODO add intertest dependencies
#  #BROKEN Each runtime_test is actually 3 tests, use below to make it one test
##  add_test(${test_dir}/${test_name} ${CMAKE_COMMAND} -E remove ${test_dir}/${test_name}${CMAKE_EXECUTABLE_SUFFIX}
##                                 && ${CMAKE_COMMAND} --build --target ${test_name})
##                                 && ${CMAKE_CURRENT_LIST_DIR}/${test_dir}/${test_name}${CMAKE_EXECUTABLE_SUFFIX})

#endfunction()

#enable_testing()
#skia_test( AAClipTest )
#target_include_directories( AAClipTest PUBLIC src/core tools/gpu include/private )
#skia_test( AnnotationTest )
#target_include_directories( AnnotationTest PUBLIC tools/gpu include/private )
