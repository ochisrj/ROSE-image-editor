#pragma once

// Central command enum for the Photoshop-style menu bar.
// Menu items and global keyboard shortcuts both emit these commands;
// App::Dispatch() routes each command to its action.
//
// NOTE: This file is a UI *template*. Every command exists so that the menu
// bar structure matches Adobe Photoshop 1:1, but the actions themselves are
// stubs that print "not implemented yet" on the status bar. To make a command
// functional, add a case in App::Execute() inside appstate.cpp.
enum class Cmd
{
    // ---- File ----
    FileNew,                        // Ctrl+N
    FileOpen,                       // Ctrl+O
    FileBrowseInBridge,             // Alt+Ctrl+O
    FileOpenAs,                     // Alt+Shift+Ctrl+O
    FileOpenAsSmartObject,
    FileClose,                      // Ctrl+W
    FileCloseAll,                   // Alt+Ctrl+W
    FileCloseAndGoToBridge,         // Shift+Ctrl+W
    FileSave,                       // Ctrl+S
    FileSaveAs,                     // Ctrl+Shift+S
    FileSaveCopy,                   // Alt+Ctrl+S
    FileRevert,                     // F12
    FileQuickExportPNG,
    FileExportAs,                   // Alt+Shift+Ctrl+W
    FileExportPreferences,
    FileSaveForWeb,                 // Alt+Shift+Ctrl+S
    FileRenderVideo,
    FileGenerateImageAssets,
    FilePlaceEmbedded,
    FilePlaceLinked,
    FilePackage,
    FileBatch,
    FileCreateDroplet,
    FileCropAndStraightenPhotos,
    FileContactSheetII,
    FilePDFPresentation,
    FilePhotomerge,
    FileImageProcessor,
    FileDeleteAllEmptyLayers,
    FileLoadFilesIntoStack,
    FileScriptsBrowse,
    FilePrint,                      // Ctrl+P
    FilePrintOneCopy,               // Alt+Shift+Ctrl+P
    FileExit,                       // Alt+F4

    // ---- Edit ----
    EditUndo,                       // Ctrl+Z
    EditRedo,                       // Ctrl+Shift+Z / Ctrl+Y
    EditToggleLastState,            // Alt+Ctrl+Z
    EditFade,                       // Shift+Ctrl+F
    EditCut,                        // Ctrl+X
    EditCopy,                       // Ctrl+C
    EditCopyMerged,                 // Shift+Ctrl+C
    EditPaste,                      // Ctrl+V
    EditPasteInPlace,               // Shift+Ctrl+V
    EditPasteInto,                  // Alt+Shift+Ctrl+V
    EditPasteOutside,
    EditClear,
    EditSearch,                     // Ctrl+F
    EditCheckSpelling,
    EditFindAndReplaceText,
    EditFill,                       // Shift+F5
    EditStroke,
    EditContentAwareFill,
    EditContentAwareScale,          // Alt+Shift+Ctrl+C
    EditFreeTransform,              // Ctrl+T
    EditTransformAgain,             // Shift+Ctrl+T
    EditTransformScale,
    EditTransformRotate,
    EditTransformSkew,
    EditTransformDistort,
    EditTransformPerspective,
    EditTransformWarp,
    EditTransformSplitWarpHorizontally,
    EditTransformSplitWarpVertically,
    EditTransformRotate180,
    EditTransformRotate90CW,
    EditTransformRotate90CCW,
    EditTransformFlipHorizontal,
    EditTransformFlipVertical,
    EditAutoAlignLayers,
    EditAutoBlendLayers,
    EditDefineBrushPreset,
    EditDefinePatternPreset,
    EditDefineCustomShapePreset,
    EditPurgeClipboard,
    EditPurgeHistories,
    EditPurgeAll,
    EditColorSettings,              // Shift+Ctrl+K
    EditKeyboardShortcuts,          // Alt+Shift+Ctrl+K
    EditMenus,                      // Alt+Shift+Ctrl+M
    EditPreferencesGeneral,         // Ctrl+K
    EditPreferencesPerformance,
    EditPreferencesScratchDisks,
    EditPreferencesDisplay,
    EditPreferencesUnitsRulers,

    // ---- Image ----
    ImageModeBitmap,
    ImageModeGrayscale,
    ImageModeDuotone,
    ImageModeIndexed,
    ImageModeRGB,
    ImageModeCMYK,
    ImageModeLab,
    ImageModeMultichannel,
    ImageBits8,
    ImageBits16,
    ImageBits32,
    ImageAdjustBrightnessContrast,
    ImageAdjustLevels,              // Ctrl+L
    ImageAdjustCurves,              // Ctrl+M
    ImageAdjustExposure,
    ImageAdjustVibrance,
    ImageAdjustHueSaturation,       // Ctrl+U
    ImageAdjustColorBalance,        // Ctrl+B
    ImageAdjustBlackWhite,          // Alt+Shift+Ctrl+B
    ImageAdjustPhotoFilter,
    ImageAdjustChannelMixer,
    ImageAdjustColorLookup,
    ImageAdjustInvert,              // Ctrl+I
    ImageAdjustPosterize,
    ImageAdjustThreshold,
    ImageAdjustGradientMap,
    ImageAdjustSelectiveColor,
    ImageAdjustShadowsHighlights,
    ImageAdjustHDRToning,
    ImageAdjustDesaturate,          // Shift+Ctrl+U
    ImageAdjustMatchColor,
    ImageAdjustReplaceColor,
    ImageAdjustEqualize,
    ImageAutoTone,                  // Shift+Ctrl+N
    ImageAutoContrast,              // Alt+Shift+Ctrl+N
    ImageAutoColor,                 // Shift+Ctrl+B
    ImageSize,                      // Alt+Ctrl+I
    CanvasSize,                     // Alt+Ctrl+C
    ImageRotate180,
    ImageRotate90CW,
    ImageRotate90CCW,
    ImageRotateArbitrary,
    ImageFlipHorizontal,
    ImageFlipVertical,
    ImageCrop,
    ImageTrim,
    ImageRevealAll,
    ImageDuplicate,
    ImageApplyImage,
    ImageCalculations,
    ImageVariablesDataSets,
    ImageAnalysisScaleMarker,
    ImageAnalysisRulerTool,

    // ---- Layer ----
    LayerNewLayer,                  // Shift+Ctrl+N
    LayerNewLayerFromBackground,
    LayerNewGroup,                  // Ctrl+G
    LayerNewGroupFromLayers,
    LayerNewArtboard,
    LayerDuplicate,                 // Ctrl+J
    LayerDeleteLayer,               // Delete
    LayerDeleteHiddenLayers,
    LayerQuickExportPNG,
    LayerExportAs,
    LayerStyleBlendingOptions,
    LayerStyleDropShadow,
    LayerStyleInnerShadow,
    LayerStyleOuterGlow,
    LayerStyleInnerGlow,
    LayerStyleBevelEmboss,
    LayerStyleSatin,
    LayerStyleColorOverlay,
    LayerStyleGradientOverlay,
    LayerStylePatternOverlay,
    LayerStyleStroke,
    LayerStyleCopy,
    LayerStylePaste,
    LayerStyleClear,
    LayerNewFillLayerSolidColor,
    LayerNewFillLayerGradient,
    LayerNewFillLayerPattern,
    LayerNewAdjustmentLayerLevels,
    LayerNewAdjustmentLayerCurves,
    LayerNewAdjustmentLayerHueSaturation,
    LayerNewAdjustmentLayerColorBalance,
    LayerNewAdjustmentLayerBlackWhite,
    LayerNewAdjustmentLayerChannelMixer,
    LayerNewAdjustmentLayerGradientMap,
    LayerNewAdjustmentLayerPhotoFilter,
    LayerContentOptions,
    LayerMaskRevealAll,
    LayerMaskHideAll,
    LayerMaskRevealSelection,
    LayerMaskHideSelection,
    LayerMaskDelete,
    LayerMaskApply,
    LayerMaskEnable,
    LayerMaskLink,
    LayerVectorMaskRevealAll,
    LayerVectorMaskHideAll,
    LayerVectorMaskCurrentPath,
    LayerVectorMaskDelete,
    LayerVectorMaskEnable,
    LayerVectorMaskLink,
    LayerCreateClippingMask,         // Alt+Ctrl+G
    LayerReleaseClippingMask,       // Alt+Ctrl+G
    LayerSmartObjectConvert,
    LayerSmartObjectRasterize,
    LayerRasterizeLayer,
    LayerRasterizeStyle,
    LayerRasterizeVectorMask,
    LayerRasterizeAllLayers,
    LayerNewLayerBasedOnSlice,
    LayerGroupLayers,               // Ctrl+G
    LayerUngroupLayers,             // Shift+Ctrl+G
    LayerHideLayers,
    LayerArrangeBringToFront,       // Shift+Ctrl+]
    LayerArrangeBringForward,       // Ctrl+]
    LayerArrangeSendBackward,       // Ctrl+[
    LayerArrangeSendToBack,         // Shift+Ctrl+[
    LayerArrangeReverse,
    LayerCombineShapesUnite,
    LayerCombineShapesIntersect,
    LayerCombineShapesSubtract,
    LayerCombineShapesExclude,
    LayerCombineShapesMergeShapes,
    LayerAlignTopEdges,
    LayerAlignVerticalCenters,
    LayerAlignBottomEdges,
    LayerAlignLeftEdges,
    LayerAlignHorizontalCenters,
    LayerAlignRightEdges,
    LayerDistributeTopEdges,
    LayerDistributeVerticalCenters,
    LayerDistributeBottomEdges,
    LayerDistributeLeftEdges,
    LayerDistributeHorizontalCenters,
    LayerDistributeRightEdges,
    LayerLockLayers,
    LayerMergeDown,                 // Ctrl+E
    LayerMergeVisible,              // Shift+Ctrl+E
    LayerFlatten,
    LayerMattingDefringe,
    LayerMattingRemoveBlackMatte,
    LayerMattingRemoveWhiteMatte,

    // ---- Select ----
    SelectAll,                      // Ctrl+A
    SelectDeselect,                 // Ctrl+D
    SelectReselect,                 // Shift+Ctrl+D
    SelectInverse,                  // Shift+Ctrl+I
    SelectAllLayers,                // Alt+Ctrl+A
    SelectDeselectLayers,
    SelectFindLayers,               // Alt+Shift+Ctrl+F
    SelectIsolateLayers,
    SelectColorRange,
    SelectFocusArea,
    SelectSubject,
    SelectSky,
    SelectAndMask,                  // Alt+Ctrl+R
    SelectModifyBorder,
    SelectModifySmooth,
    SelectModifyExpand,
    SelectModifyContract,
    SelectModifyFeather,            // Shift+F6
    SelectGrow,
    SelectSimilar,
    SelectTransformSelection,
    SelectQuickMaskMode,            // Q
    SelectSaveSelection,
    SelectLoadSelection,

    // ---- Filter ----
    FilterLastFilter,               // Ctrl+F / Alt+Ctrl+F
    FilterConvertForSmartFilters,
    FilterGallery,
    FilterAdaptiveWideAngle,        // Alt+Shift+Ctrl+A
    FilterCameraRaw,                // Shift+Ctrl+A
    FilterLensCorrection,           // Shift+Ctrl+R
    FilterLiquify,                  // Shift+Ctrl+X
    FilterVanishPoint,              // Alt+Ctrl+V
    Filter3DGenerateBumpMap,
    Filter3DGenerateNormalMap,
    FilterBlurAverage,
    FilterBlurBlur,
    FilterBlurBlurMore,
    FilterBlurBoxBlur,
    FilterBlurGaussianBlur,
    FilterBlurLensBlur,
    FilterBlurMotionBlur,
    FilterBlurRadialBlur,
    FilterBlurSurfaceBlur,
    FilterBlurGalleryFieldBlur,
    FilterBlurGalleryIrisBlur,
    FilterBlurGalleryTiltShift,
    FilterDistortDisplace,
    FilterDistortPinch,
    FilterDistortPolarCoordinates,
    FilterDistortRipple,
    FilterDistortShear,
    FilterDistortSpherize,
    FilterDistortTwirl,
    FilterDistortWave,
    FilterDistortZigZag,
    FilterNoiseAddNoise,
    FilterNoiseDespeckle,
    FilterNoiseDustScratches,
    FilterNoiseMedian,
    FilterNoiseReduceNoise,
    FilterPixelateColorHalftone,
    FilterPixelateCrystallize,
    FilterPixelateFacet,
    FilterPixelateMosaic,
    FilterPixelateMezzotint,
    FilterPixelatePointillize,
    FilterRenderClouds,
    FilterRenderDifferenceClouds,
    FilterRenderFibers,
    FilterRenderLensFlare,
    FilterRenderLightingEffects,
    FilterSharpenSharpen,
    FilterSharpenSharpenEdges,
    FilterSharpenSharpenMore,
    FilterSharpenSmartSharpen,
    FilterSharpenUnsharpMask,
    FilterStylizeDiffuse,
    FilterStylizeEmboss,
    FilterStylizeExtrude,
    FilterStylizeFindEdges,
    FilterStylizeOilPaint,
    FilterStylizeSolarize,
    FilterStylizeTiles,
    FilterStylizeTraceContour,
    FilterStylizeWind,
    FilterOtherCustom,
    FilterOtherHighPass,
    FilterOtherMaximum,
    FilterOtherMinimum,
    FilterOtherOffset,

    // ---- View ----
    ViewProofSetupCustom,
    ViewProofSetupWorkingCMYK,
    ViewProofSetupMacintoshRGB,
    ViewProofSetupInternetStandardRGB,
    ViewProofColors,                // Ctrl+Y
    ViewGamutWarning,               // Shift+Ctrl+Y
    ViewPixelAspectRatioSquare,
    ViewPixelAspectRatioD1VTSAspect,
    ViewPixelAspectRatioD1PAL,
    ViewPixelAspectRatioHDV1080,
    ViewPixelAspectRatioReset,
    ViewZoomIn,                     // Ctrl++
    ViewZoomOut,                    // Ctrl+-
    ViewFitScreen,                  // Ctrl+0
    ViewActualPixels,               // Ctrl+1 / Alt+Ctrl+0
    ViewPrintSize,
    ViewScreenModeStandard,
    ViewScreenModeFullWithMenu,     // F
    ViewScreenModeFull,             // F
    ViewShowTargetPath,
    ViewShowGrid,                   // Ctrl+'
    ViewShowGuides,                 // Ctrl+;
    ViewShowCanvasGuides,
    ViewShowRulers,                 // Ctrl+R
    ViewShowPixelGrid,
    ViewShowLayerEdges,
    ViewShowSelectionEdges,         // Ctrl+H
    ViewSnap,                       // Shift+Ctrl+;
    ViewSnapToGuides,
    ViewSnapToGrid,
    ViewSnapToLayers,
    ViewSnapToSlices,
    ViewSnapToDocumentBounds,
    ViewSnapToAll,
    ViewSnapToNone,
    ViewGuideNew,
    ViewGuideNewLayout,
    ViewGuideLockGuides,            // Alt+Ctrl+;
    ViewGuideClearGuides,

    // ---- Window ----
    WindowArrangeTileVertically,
    WindowArrangeTileHorizontally,
    WindowArrangeConsolidateTabs,
    WindowArrangeFloatInWindow,
    WindowArrangeFloatAllInWindows,
    WorkspaceDefault,               // Essentials
    Workspace3D,
    WorkspaceGraphicWeb,
    WorkspaceMotion,
    WorkspacePainting,
    WorkspacePhotography,
    WorkspaceResetCurrent,
    WorkspaceNew,
    WindowExtensions,
    WindowToggle3D,
    WindowToggleActions,            // Alt+F9
    WindowToggleAdjustments,
    WindowToggleBrushSettings,      // F5
    WindowToggleBrushes,
    WindowToggleChannels,
    WindowToggleCharacter,
    WindowToggleColor,              // F6
    WindowToggleHistory,
    WindowToggleInfo,               // F8
    WindowToggleLayers,             // F7
    WindowToggleNavigator,
    WindowToggleParagraph,
    WindowToggleProperties,
    WindowTogglePaths,
    WindowToggleViewport,
    WindowToggleSwatches,
    WindowToggleTimeline,
    WindowToggleToolSettings,
    WindowToggleTools,
    WindowApplicationFrame,
    WindowOptions,
    WindowTools,

    // ---- Help ----
    HelpDocumentation,              // F1
    HelpTutorials,
    HelpHandsOnTutorials,
    HelpAbout,
    HelpSystemInfo,
    HelpManageAccount,
    HelpSignOut,

    COUNT
};
