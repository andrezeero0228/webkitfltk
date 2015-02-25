/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// This all-in-one cpp file cuts down on template bloat to allow us to build our Windows release build.

#include "HTMLElementFactory.cpp"
#include "HTMLEntityTable.cpp"
#if ENABLE(WEBGL)
#include "JSANGLEInstancedArrays.cpp"
#endif
#include "JSAbstractWorker.cpp"
#include "JSAnimationEvent.cpp"
#include "JSAttr.cpp"
#include "JSBarProp.cpp"
#include "JSBeforeLoadEvent.cpp"
#include "JSBeforeUnloadEvent.cpp"
#include "JSBlob.cpp"
#include "JSCanvasGradient.cpp"
#include "JSCanvasPattern.cpp"
#include "JSCanvasRenderingContext.cpp"
#include "JSCanvasRenderingContext2D.cpp"
#if ENABLE(WEBGL)
#include "JSEXTBlendMinMax.cpp"
#include "JSEXTFragDepth.cpp"
#include "JSEXTShaderTextureLOD.cpp"
#include "JSEXTTextureFilterAnisotropic.cpp"
#include "JSEXTsRGB.cpp"
#include "JSOESElementIndexUint.cpp"
#include "JSOESStandardDerivatives.cpp"
#include "JSOESTextureFloat.cpp"
#include "JSOESTextureFloatLinear.cpp"
#include "JSOESTextureHalfFloat.cpp"
#include "JSOESTextureHalfFloatLinear.cpp"
#include "JSOESVertexArrayObject.cpp"
#include "JSWebGL2RenderingContext.cpp"
#include "JSWebGLActiveInfo.cpp"
#include "JSWebGLBuffer.cpp"
#include "JSWebGLCompressedTextureATC.cpp"
#include "JSWebGLCompressedTexturePVRTC.cpp"
#include "JSWebGLCompressedTextureS3TC.cpp"
#include "JSWebGLContextAttributes.cpp"
#include "JSWebGLContextEvent.cpp"
#include "JSWebGLDebugRendererInfo.cpp"
#include "JSWebGLDebugShaders.cpp"
#include "JSWebGLDepthTexture.cpp"
#include "JSWebGLDrawBuffers.cpp"
#include "JSWebGLFramebuffer.cpp"
#include "JSWebGLLoseContext.cpp"
#include "JSWebGLProgram.cpp"
#include "JSWebGLQuery.cpp"
#include "JSWebGLRenderbuffer.cpp"
#include "JSWebGLRenderingContext.cpp"
#include "JSWebGLRenderingContextBase.cpp"
#include "JSWebGLSampler.cpp"
#include "JSWebGLShader.cpp"
#include "JSWebGLShaderPrecisionFormat.cpp" 
#include "JSWebGLSync.cpp"
#include "JSWebGLTexture.cpp"
#include "JSWebGLTransformFeedback.cpp"
#include "JSWebGLUniformLocation.cpp"
#include "JSWebGLVertexArrayObject.cpp"
#include "JSWebGLVertexArrayObjectOES.cpp"
#endif
#include "JSCDATASection.cpp"
#include "JSCharacterData.cpp"
#include "JSClientRect.cpp"
#include "JSClientRectList.cpp"
#include "JSCloseEvent.cpp"
#include "JSCommandLineAPIHost.cpp"
#include "JSComment.cpp"
#include "JSCompositionEvent.cpp"
#include "JSCoordinates.cpp"
#include "JSCounter.cpp"
#include "JSCrypto.cpp"
#include "JSCSSCharsetRule.cpp"
#if ENABLE(FONT_LOAD_EVENTS)
#include "JSCSSFontFaceLoadEvent.cpp"
#endif
#include "JSCSSFontFaceRule.cpp"
#include "JSCSSImportRule.cpp"
#include "JSCSSKeyframeRule.cpp"
#include "JSCSSKeyframesRule.cpp"
#include "JSCSSMediaRule.cpp"
#include "JSCSSPageRule.cpp"
#include "JSCSSPrimitiveValue.cpp"
#include "JSCSSRule.cpp"
#include "JSCSSRuleList.cpp"
#include "JSCSSStyleDeclaration.cpp"
#include "JSCSSStyleRule.cpp"
#include "JSCSSStyleSheet.cpp"
#include "JSCSSSupportsRule.cpp"
#include "JSCSSValue.cpp"
#include "JSCSSValueList.cpp"
#include "JSCustomEvent.cpp"
#include "JSDatabase.cpp"
#include "JSDatabaseCallback.cpp"
#include "JSDataTransfer.cpp"
#include "JSDataTransferItem.cpp"
#include "JSDataTransferItemList.cpp"
#include "JSDedicatedWorkerGlobalScope.cpp"
#include "JSDeviceOrientationEvent.cpp"
#include "JSDocument.cpp"
#include "JSDocumentFragment.cpp"
#include "JSDocumentType.cpp"
#include "JSDOMApplicationCache.cpp"
#include "JSDOMCoreException.cpp"
#include "JSDOMError.cpp"
#include "JSDOMFormData.cpp"
#include "JSDOMImplementation.cpp"
#include "JSDOMMimeType.cpp"
#include "JSDOMMimeTypeArray.cpp"
#include "JSDOMNamedFlowCollection.cpp"
#include "JSDOMParser.cpp"
#include "JSDOMPath.cpp"
#include "JSDOMPlugin.cpp"
#include "JSDOMPluginArray.cpp"
#include "JSDOMSecurityPolicy.cpp"
#include "JSDOMSelection.cpp"
#include "JSDOMSettableTokenList.cpp"
#include "JSDOMStringList.cpp"
#include "JSDOMStringMap.cpp"
#include "JSDOMTokenList.cpp"
#include "JSDOMURL.cpp"
#include "JSDOMWindow.cpp"
#include "JSDOMWindowCSS.cpp"
#include "JSElement.cpp"
#include "JSEntity.cpp"
#include "JSEntityReference.cpp"
#include "JSErrorEvent.cpp"
#include "JSEvent.cpp"
#include "JSEventException.cpp"
#include "JSEventSource.cpp"
#include "JSEventTarget.cpp"
#include "JSFile.cpp"
#include "JSFileError.cpp"
#include "JSFileException.cpp"
#include "JSFileList.cpp"
#include "JSFileReader.cpp"
#include "JSFileReaderSync.cpp"
#include "JSFocusEvent.cpp"
#if ENABLE(FONT_LOAD_EVENTS)
#include "JSFontLoader.cpp"
#endif
#include "JSGeolocation.cpp"
#include "JSGeoposition.cpp"
#include "JSHashChangeEvent.cpp"
#include "JSHistory.cpp"
#include "JSHTMLAllCollection.cpp"
#include "JSHTMLAnchorElement.cpp"
#include "JSHTMLAppletElement.cpp"
#include "JSHTMLAreaElement.cpp"
#if ENABLE(ATTACHMENT_ELEMENT)
#include "JSHTMLAttachmentElement.cpp"
#endif
#include "JSHTMLAudioElement.cpp"
#include "JSHTMLBaseElement.cpp"
#include "JSHTMLBaseFontElement.cpp"
#include "JSHTMLBodyElement.cpp"
#include "JSHTMLBRElement.cpp"
#include "JSHTMLButtonElement.cpp"
#include "JSHTMLCanvasElement.cpp"
#include "JSHTMLCollection.cpp"
#include "JSHTMLDataListElement.cpp"
#include "JSHTMLDetailsElement.cpp"
#include "JSHTMLDirectoryElement.cpp"
#include "JSHTMLDivElement.cpp"
#include "JSHTMLDListElement.cpp"
#include "JSHTMLDocument.cpp"
#include "JSHTMLElement.cpp"
#include "JSHTMLElementWrapperFactory.cpp"
#include "JSHTMLEmbedElement.cpp"
#include "JSHTMLFieldSetElement.cpp"
#include "JSHTMLFontElement.cpp"
#include "JSHTMLFormControlsCollection.cpp"
#include "JSHTMLFormElement.cpp"
#include "JSHTMLFrameElement.cpp"
#include "JSHTMLFrameSetElement.cpp"
#include "JSHTMLHeadElement.cpp"
#include "JSHTMLHeadingElement.cpp"
#include "JSHTMLHRElement.cpp"
#include "JSHTMLHtmlElement.cpp"
#include "JSHTMLIFrameElement.cpp"
#include "JSHTMLImageElement.cpp"
#include "JSHTMLInputElement.cpp"
#include "JSHTMLKeygenElement.cpp"
#include "JSHTMLLabelElement.cpp"
#include "JSHTMLLegendElement.cpp"
#include "JSHTMLLIElement.cpp"
#include "JSHTMLLinkElement.cpp"
#include "JSHTMLMapElement.cpp"
#include "JSHTMLMarqueeElement.cpp"
#include "JSHTMLMediaElement.cpp"
#include "JSHTMLMenuElement.cpp"
#include "JSHTMLMetaElement.cpp"
#include "JSHTMLMeterElement.cpp"
#include "JSHTMLModElement.cpp"
#include "JSHTMLObjectElement.cpp"
#include "JSHTMLOListElement.cpp"
#include "JSHTMLOptGroupElement.cpp"
#include "JSHTMLOptionElement.cpp"
#include "JSHTMLOptionsCollection.cpp"
#include "JSHTMLOutputElement.cpp"
#include "JSHTMLParagraphElement.cpp"
#include "JSHTMLParamElement.cpp"
#include "JSHTMLPreElement.cpp"
#include "JSHTMLProgressElement.cpp"
#include "JSHTMLQuoteElement.cpp"
#include "JSHTMLScriptElement.cpp"
#include "JSHTMLSelectElement.cpp"
#include "JSHTMLSourceElement.cpp"
#include "JSHTMLSpanElement.cpp"
#include "JSHTMLStyleElement.cpp"
#include "JSHTMLTableCaptionElement.cpp"
#include "JSHTMLTableCellElement.cpp"
#include "JSHTMLTableColElement.cpp"
#include "JSHTMLTableElement.cpp"
#include "JSHTMLTableRowElement.cpp"
#include "JSHTMLTableSectionElement.cpp"
#if ENABLE(TEMPLATE_ELEMENT)
#include "JSHTMLTemplateElement.cpp"
#endif
#include "JSHTMLTextAreaElement.cpp"
#include "JSHTMLTitleElement.cpp"
#include "JSHTMLUnknownElement.cpp"
#include "JSHTMLUListElement.cpp"
#include "JSHTMLVideoElement.cpp"
#include "JSIDBAny.cpp"
#include "JSIDBCursor.cpp"
#include "JSIDBDatabase.cpp"
#include "JSIDBFactory.cpp"
#include "JSIDBIndex.cpp"
#include "JSIDBKeyRange.cpp"
#include "JSIDBObjectStore.cpp"
#include "JSIDBRequest.cpp"
#include "JSIDBTransaction.cpp"
#include "JSImageData.cpp"
#include "JSInspectorFrontendHost.cpp"
#include "JSKeyboardEvent.cpp"
#include "JSLocation.cpp"
#include "JSMediaController.cpp"
#if ENABLE(MEDIA_CONTROLS_SCRIPT)
#include "JSMediaControlsHost.cpp"
#endif
#include "JSMediaError.cpp"
#include "JSMediaList.cpp"
#if ENABLE(ENCRYPTED_MEDIA_V2)
#include "JSMediaKeyError.cpp"
#include "JSMediaKeyMessageEvent.cpp"
#include "JSMediaKeyNeededEvent.cpp"
#include "JSMediaKeySession.cpp"
#include "JSMediaKeys.cpp"
#endif
#include "JSMediaQueryList.cpp"
#include "JSMediaQueryListListener.cpp"
#include "JSMessageChannel.cpp"
#include "JSMessageEvent.cpp"
#include "JSMessagePort.cpp"
#include "JSMouseEvent.cpp"
#include "JSMutationEvent.cpp"
#include "JSMutationObserver.cpp"
#include "JSMutationRecord.cpp"
#include "JSNamedNodeMap.cpp"
#include "JSNavigator.cpp"
#include "JSNode.cpp"
#include "JSNodeFilter.cpp"
#include "JSNodeIterator.cpp"
#include "JSNodeList.cpp"
#include "JSNotificationCenter.cpp"
#include "JSNotification.cpp"
#include "JSOverflowEvent.cpp"
#include "JSPageTransitionEvent.cpp"
#include "JSPerformance.cpp"
#include "JSPerformanceNavigation.cpp"
#include "JSPerformanceTiming.cpp"
#include "JSPopStateEvent.cpp"
#include "JSPositionCallback.cpp"
#include "JSPositionError.cpp"
#include "JSPositionErrorCallback.cpp"
#include "JSProcessingInstruction.cpp"
#include "JSProgressEvent.cpp"
#include "JSRadioNodeList.cpp"
#include "JSRange.cpp"
#include "JSRangeException.cpp"
#if ENABLE(STREAMS_API)
#include "JSReadableStream.cpp"
#endif
#include "JSRect.cpp"
#include "JSRequestAnimationFrameCallback.cpp"
#include "JSRGBColor.cpp"
#include "JSScreen.cpp"
#include "JSScriptProfile.cpp"
#include "JSScriptProfileNode.cpp"
#include "JSSQLError.cpp"
#include "JSSQLException.cpp"
#include "JSSQLResultSet.cpp"
#include "JSSQLResultSetRowList.cpp"
#include "JSSQLStatementCallback.cpp"
#include "JSSQLStatementErrorCallback.cpp"
#include "JSSQLTransaction.cpp"
#include "JSSQLTransactionCallback.cpp"
#include "JSSQLTransactionErrorCallback.cpp"
#include "JSSVGAElement.cpp"
#include "JSStorage.cpp"
#include "JSStorageErrorCallback.cpp"
#include "JSStorageEvent.cpp"
#include "JSStorageInfo.cpp"
#include "JSStorageQuota.cpp"
#include "JSStorageQuotaCallback.cpp"
#include "JSStorageUsageCallback.cpp"
#include "JSStringCallback.cpp"
#include "JSStyleMedia.cpp"
#include "JSStyleSheet.cpp"
#include "JSStyleSheetList.cpp"
#include "JSSVGAltGlyphDefElement.cpp"
#include "JSSVGAltGlyphElement.cpp"
#include "JSSVGAltGlyphItemElement.cpp"
#include "JSSVGAngle.cpp"
#include "JSSVGAnimateColorElement.cpp"
#include "JSSVGAnimatedAngle.cpp"
#include "JSSVGAnimatedBoolean.cpp"
#include "JSSVGAnimatedEnumeration.cpp"
#include "JSSVGAnimatedInteger.cpp"
#include "JSSVGAnimatedLength.cpp"
#include "JSSVGAnimatedLengthList.cpp"
#include "JSSVGAnimatedNumber.cpp"
#include "JSSVGAnimatedNumberList.cpp"
#include "JSSVGAnimatedPreserveAspectRatio.cpp"
#include "JSSVGAnimatedRect.cpp"
#include "JSSVGAnimatedString.cpp"
#include "JSSVGAnimatedTransformList.cpp"
#include "JSSVGAnimateElement.cpp"
#include "JSSVGAnimateMotionElement.cpp"
#include "JSSVGAnimateTransformElement.cpp"
#include "JSSVGAnimationElement.cpp"
#include "JSSVGCircleElement.cpp"
#include "JSSVGClipPathElement.cpp"
#include "JSSVGColor.cpp"
#include "JSSVGComponentTransferFunctionElement.cpp"
#include "JSSVGCursorElement.cpp"
#include "JSSVGDefsElement.cpp"
#include "JSSVGDescElement.cpp"
#include "JSSVGDocument.cpp"
#include "JSSVGElement.cpp"
#include "JSSVGEllipseElement.cpp"
#include "JSSVGException.cpp"
#include "JSSVGFEBlendElement.cpp"
#include "JSSVGFEColorMatrixElement.cpp"
#include "JSSVGFEComponentTransferElement.cpp"
#include "JSSVGFECompositeElement.cpp"
#include "JSSVGFEConvolveMatrixElement.cpp"
#include "JSSVGFEDiffuseLightingElement.cpp"
#include "JSSVGFEDisplacementMapElement.cpp"
#include "JSSVGFEDistantLightElement.cpp"
#include "JSSVGFEDropShadowElement.cpp"
#include "JSSVGFEFloodElement.cpp"
#include "JSSVGFEFuncAElement.cpp"
#include "JSSVGFEFuncBElement.cpp"
#include "JSSVGFEFuncGElement.cpp"
#include "JSSVGFEFuncRElement.cpp"
#include "JSSVGFEGaussianBlurElement.cpp"
#include "JSSVGFEImageElement.cpp"
#include "JSSVGFEMergeElement.cpp"
#include "JSSVGFEMergeNodeElement.cpp"
#include "JSSVGFEMorphologyElement.cpp"
#include "JSSVGFEOffsetElement.cpp"
#include "JSSVGFEPointLightElement.cpp"
#include "JSSVGFESpecularLightingElement.cpp"
#include "JSSVGFESpotLightElement.cpp"
#include "JSSVGFETileElement.cpp"
#include "JSSVGFETurbulenceElement.cpp"
#include "JSSVGFilterElement.cpp"
#include "JSSVGFontElement.cpp"
#include "JSSVGFontFaceElement.cpp"
#include "JSSVGFontFaceFormatElement.cpp"
#include "JSSVGFontFaceNameElement.cpp"
#include "JSSVGFontFaceSrcElement.cpp"
#include "JSSVGFontFaceUriElement.cpp"
#include "JSSVGForeignObjectElement.cpp"
#include "JSSVGGElement.cpp"
#include "JSSVGGlyphElement.cpp"
#include "JSSVGGlyphRefElement.cpp"
#include "JSSVGGradientElement.cpp"
#include "JSSVGGraphicsElement.cpp"
#include "JSSVGHKernElement.cpp"
#include "JSSVGImageElement.cpp"
#include "JSSVGLength.cpp"
#include "JSSVGLengthList.cpp"
#include "JSSVGLinearGradientElement.cpp"
#include "JSSVGLineElement.cpp"
#include "JSSVGMPathElement.cpp"
#include "JSSVGMarkerElement.cpp"
#include "JSSVGMaskElement.cpp"
#include "JSSVGMatrix.cpp"
#include "JSSVGMetadataElement.cpp"
#include "JSSVGMissingGlyphElement.cpp"
#include "JSSVGNumber.cpp"
#include "JSSVGNumberList.cpp"
#include "JSSVGPaint.cpp"
#include "JSSVGPathElement.cpp"
#include "JSSVGPathSeg.cpp"
#include "JSSVGPathSegArcAbs.cpp"
#include "JSSVGPathSegArcRel.cpp"
#include "JSSVGPathSegClosePath.cpp"
#include "JSSVGPathSegCurvetoCubicAbs.cpp"
#include "JSSVGPathSegCurvetoCubicRel.cpp"
#include "JSSVGPathSegCurvetoCubicSmoothAbs.cpp"
#include "JSSVGPathSegCurvetoCubicSmoothRel.cpp"
#include "JSSVGPathSegCurvetoQuadraticAbs.cpp"
#include "JSSVGPathSegCurvetoQuadraticRel.cpp"
#include "JSSVGPathSegCurvetoQuadraticSmoothAbs.cpp"
#include "JSSVGPathSegCurvetoQuadraticSmoothRel.cpp"
#include "JSSVGPathSegLinetoAbs.cpp"
#include "JSSVGPathSegLinetoHorizontalAbs.cpp"
#include "JSSVGPathSegLinetoHorizontalRel.cpp"
#include "JSSVGPathSegLinetoRel.cpp"
#include "JSSVGPathSegLinetoVerticalAbs.cpp"
#include "JSSVGPathSegLinetoVerticalRel.cpp"
#include "JSSVGPathSegList.cpp"
#include "JSSVGPathSegMovetoAbs.cpp"
#include "JSSVGPathSegMovetoRel.cpp"
#include "JSSVGPatternElement.cpp"
#include "JSSVGPoint.cpp"
#include "JSSVGPointList.cpp"
#include "JSSVGPolygonElement.cpp"
#include "JSSVGPolylineElement.cpp"
#include "JSSVGPreserveAspectRatio.cpp"
#include "JSSVGRadialGradientElement.cpp"
#include "JSSVGRect.cpp"
#include "JSSVGRectElement.cpp"
#include "JSSVGRenderingIntent.cpp"
#include "JSSVGSVGElement.cpp"
#include "JSSVGScriptElement.cpp"
#include "JSSVGSetElement.cpp"
#include "JSSVGStopElement.cpp"
#include "JSSVGStringList.cpp"
#include "JSSVGStyleElement.cpp"
#include "JSSVGSwitchElement.cpp"
#include "JSSVGSymbolElement.cpp"
#include "JSSVGTextContentElement.cpp"
#include "JSSVGTextElement.cpp"
#include "JSSVGTextPathElement.cpp"
#include "JSSVGTextPositioningElement.cpp"
#include "JSSVGTitleElement.cpp"
#include "JSSVGTransform.cpp"
#include "JSSVGTransformList.cpp"
#include "JSSVGTRefElement.cpp"
#include "JSSVGTSpanElement.cpp"
#include "JSSVGUnitTypes.cpp"
#include "JSSVGUseElement.cpp"
#include "JSSVGVKernElement.cpp"
#include "JSSVGViewElement.cpp"
#include "JSSVGViewSpec.cpp"
#include "JSSVGZoomAndPan.cpp"
#include "JSSVGZoomEvent.cpp"
#include "JSText.cpp"
#include "JSTextEvent.cpp"
#include "JSTextMetrics.cpp"
#if ENABLE(VIDEO_TRACK)
#include "JSAudioTrack.cpp"
#include "JSAudioTrackList.cpp"
#include "JSDataCue.cpp"
#include "JSHTMLTrackElement.cpp"
#include "JSTextTrack.cpp"
#include "JSTextTrackCue.cpp"
#include "JSTextTrackCueList.cpp"
#include "JSTextTrackList.cpp"
#include "JSTrackEvent.cpp"
#include "JSVTTCue.cpp"
#include "JSVideoTrack.cpp"
#include "JSVideoTrackList.cpp"
#if ENABLE(WEBVTT_REGIONS)
#include "JSVTTRegion.cpp"
#include "JSVTTRegionList.cpp"
#endif
#endif
#include "JSTimeRanges.cpp"
#include "JSTouch.cpp"
#include "JSTouchEvent.cpp"
#include "JSTouchList.cpp"
#include "JSTransitionEvent.cpp"
#include "JSTreeWalker.cpp"
#include "JSUIEvent.cpp"
#include "JSValidityState.cpp"
#include "JSVoidCallback.cpp"
#include "JSWebKitAnimationEvent.cpp"
#include "JSWebKitCSSFilterValue.cpp"
#include "JSWebKitCSSTransformValue.cpp"
#include "JSWebKitCSSMatrix.cpp"
#include "JSWebKitCSSRegionRule.cpp"
#include "JSWebKitNamedFlow.cpp"
#include "JSWebKitPoint.cpp"
#include "JSWebKitTransitionEvent.cpp"
#include "JSWebSocket.cpp"
#include "JSWheelEvent.cpp"
#include "JSWorker.cpp"
#include "JSWorkerGlobalScope.cpp"
#include "JSWorkerLocation.cpp"
#include "JSWorkerNavigator.cpp"
#include "JSXMLHttpRequest.cpp"
#include "JSXMLHttpRequestException.cpp"
#include "JSXMLHttpRequestProgressEvent.cpp"
#include "JSXMLHttpRequestUpload.cpp"
#include "JSXMLSerializer.cpp"
#include "JSXPathEvaluator.cpp"
#include "JSXPathException.cpp"
#include "JSXPathExpression.cpp"
#include "JSXPathNSResolver.cpp"
#include "JSXPathResult.cpp"
#include "JSXSLTProcessor.cpp"
#include "PlugInsResourcesData.cpp"
#include "UserAgentStyleSheetsData.cpp"

// On MSVC, including StaticConstructors.h causes all global objects not to be
// automatically initialized by the C runtime. This is useful in some specific
// cases (e.g., the *Names.cpp files), but can be dangerous in others. We don't
// want StaticConstructors.h to "pollute" all the source files we #include here
// accidentally, so we'll throw an error whenever any file includes it.
#ifdef StaticConstructors_h
#error Do not include any file in DerivedSources.cpp that includes StaticConstructors.h
#endif
