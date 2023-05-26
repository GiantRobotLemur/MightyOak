//! @file Ag/GLAPI/WGL_API.hpp
//! @brief A file generated from the OpenGL API schema definition to create
//! C++ 17 language bindings. In the case of this file, for Windows OpenGL
//! (WGL) extensions.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __0AE3D5F1_596C_46B2_8613_C0DB4AA3727F_INCLUDED__
#define __0AE3D5F1_596C_46B2_8613_C0DB4AA3727F_INCLUDED__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "BaseTypes.hpp"

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
DECLARE_HANDLE(HGPUNV);
DECLARE_HANDLE(HPBUFFERARB);
DECLARE_HANDLE(HPBUFFEREXT);
DECLARE_HANDLE(HPVIDEODEV);
DECLARE_HANDLE(HVIDEOINPUTDEVICENV);
DECLARE_HANDLE(HVIDEOOUTPUTDEVICENV);
struct _GPU_DEVICE {
    DWORD  cb;
    CHAR   DeviceName[32];
    CHAR   DeviceString[128];
    DWORD  Flags;
    RECT   rcVirtualScreen;
};
typedef struct _GPU_DEVICE *PGPU_DEVICE;

namespace wgl {
////////////////////////////////////////////////////////////////////////////////
// Enumeration Types
////////////////////////////////////////////////////////////////////////////////
// Values shared with GL. Do not allocate additional values in this range.
struct Global {
    static constexpr GLenum AccessReadOnly                             = 0x00000000; // WGL_ACCESS_READ_ONLY_NV
    static constexpr GLenum ContextReleaseBehaviorNone                 =          0; // WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB
    static constexpr GLenum FontLines                                  =          0; // WGL_FONT_LINES
    static constexpr GLenum AccessReadWrite                            = 0x00000001; // WGL_ACCESS_READ_WRITE_NV
    static constexpr GLenum ContextCoreProfile                         = 0x00000001; // WGL_CONTEXT_CORE_PROFILE_BIT_ARB
    static constexpr GLenum ContextDebug                               = 0x00000001; // WGL_CONTEXT_DEBUG_BIT_ARB
    static constexpr GLenum FontPolygons                               =          1; // WGL_FONT_POLYGONS
    static constexpr GLenum FrontColorBuffer                           = 0x00000001; // WGL_FRONT_COLOR_BUFFER_BIT_ARB
    static constexpr GLenum ImageBufferMinAccessI3D                    = 0x00000001; // WGL_IMAGE_BUFFER_MIN_ACCESS_I3D
    static constexpr GLenum SwapMainPlane                              = 0x00000001; // WGL_SWAP_MAIN_PLANE
    static constexpr GLenum AccessWriteDiscard                         = 0x00000002; // WGL_ACCESS_WRITE_DISCARD_NV
    static constexpr GLenum BackColorBuffer                            = 0x00000002; // WGL_BACK_COLOR_BUFFER_BIT_ARB
    static constexpr GLenum ContextCompatibilityProfile                = 0x00000002; // WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB
    static constexpr GLenum ContextForwardCompatible                   = 0x00000002; // WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
    static constexpr GLenum ImageBufferLockI3D                         = 0x00000002; // WGL_IMAGE_BUFFER_LOCK_I3D
    static constexpr GLenum SwapOverlay1                               = 0x00000002; // WGL_SWAP_OVERLAY1
    static constexpr GLenum ContextEsProfile                           = 0x00000004; // WGL_CONTEXT_ES_PROFILE_BIT_EXT
    static constexpr GLenum ContextES2Profile                          = 0x00000004; // WGL_CONTEXT_ES2_PROFILE_BIT_EXT
    static constexpr GLenum ContextRobustAccess                        = 0x00000004; // WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB
    static constexpr GLenum DepthBuffer                                = 0x00000004; // WGL_DEPTH_BUFFER_BIT_ARB
    static constexpr GLenum SwapOverlay2                               = 0x00000004; // WGL_SWAP_OVERLAY2
    static constexpr GLenum ContextResetIsolation                      = 0x00000008; // WGL_CONTEXT_RESET_ISOLATION_BIT_ARB
    static constexpr GLenum StencilBuffer                              = 0x00000008; // WGL_STENCIL_BUFFER_BIT_ARB
    static constexpr GLenum SwapOverlay3                               = 0x00000008; // WGL_SWAP_OVERLAY3
    static constexpr GLenum SwapOverlay4                               = 0x00000010; // WGL_SWAP_OVERLAY4
    static constexpr GLenum SwapOverlay5                               = 0x00000020; // WGL_SWAP_OVERLAY5
    static constexpr GLenum SwapOverlay6                               = 0x00000040; // WGL_SWAP_OVERLAY6
    static constexpr GLenum SwapOverlay7                               = 0x00000080; // WGL_SWAP_OVERLAY7
    static constexpr GLenum SwapOverlay8                               = 0x00000100; // WGL_SWAP_OVERLAY8
    static constexpr GLenum SwapOverlay9                               = 0x00000200; // WGL_SWAP_OVERLAY9
    static constexpr GLenum SwapOverlay10                              = 0x00000400; // WGL_SWAP_OVERLAY10
    static constexpr GLenum SwapOverlay11                              = 0x00000800; // WGL_SWAP_OVERLAY11
    static constexpr GLenum SwapOverlay12                              = 0x00001000; // WGL_SWAP_OVERLAY12
    static constexpr GLenum GPUVendor                                  =     0x1F00; // WGL_GPU_VENDOR_AMD
    static constexpr GLenum GPURendererString                          =     0x1F01; // WGL_GPU_RENDERER_STRING_AMD
    static constexpr GLenum GPUOpenglVersionString                     =     0x1F02; // WGL_GPU_OPENGL_VERSION_STRING_AMD
    static constexpr GLenum NumberPixelFormats                         =     0x2000; // WGL_NUMBER_PIXEL_FORMATS_ARB
    static constexpr GLenum SwapOverlay13                              = 0x00002000; // WGL_SWAP_OVERLAY13
    static constexpr GLenum DrawToWindow                               =     0x2001; // WGL_DRAW_TO_WINDOW_ARB
    static constexpr GLenum DrawToBitmap                               =     0x2002; // WGL_DRAW_TO_BITMAP_ARB
    static constexpr GLenum Acceleration                               =     0x2003; // WGL_ACCELERATION_ARB
    static constexpr GLenum NeedPalette                                =     0x2004; // WGL_NEED_PALETTE_ARB
    static constexpr GLenum NeedSystemPalette                          =     0x2005; // WGL_NEED_SYSTEM_PALETTE_ARB
    static constexpr GLenum SwapLayerBuffers                           =     0x2006; // WGL_SWAP_LAYER_BUFFERS_ARB
    static constexpr GLenum SwapMethod                                 =     0x2007; // WGL_SWAP_METHOD_ARB
    static constexpr GLenum NumberOverlays                             =     0x2008; // WGL_NUMBER_OVERLAYS_ARB
    static constexpr GLenum NumberUnderlays                            =     0x2009; // WGL_NUMBER_UNDERLAYS_ARB
    static constexpr GLenum Transparent                                =     0x200A; // WGL_TRANSPARENT_ARB
    static constexpr GLenum TransparentValue                           =     0x200B; // WGL_TRANSPARENT_VALUE_EXT
    static constexpr GLenum ShareDepth                                 =     0x200C; // WGL_SHARE_DEPTH_ARB
    static constexpr GLenum ShareStencil                               =     0x200D; // WGL_SHARE_STENCIL_ARB
    static constexpr GLenum ShareAccum                                 =     0x200E; // WGL_SHARE_ACCUM_ARB
    static constexpr GLenum SupportGDI                                 =     0x200F; // WGL_SUPPORT_GDI_ARB
    static constexpr GLenum SupportOpenGL                              =     0x2010; // WGL_SUPPORT_OPENGL_ARB
    static constexpr GLenum DoubleBuffer                               =     0x2011; // WGL_DOUBLE_BUFFER_ARB
    static constexpr GLenum Stereo                                     =     0x2012; // WGL_STEREO_ARB
    static constexpr GLenum PixelType                                  =     0x2013; // WGL_PIXEL_TYPE_ARB
    static constexpr GLenum ColorBits                                  =     0x2014; // WGL_COLOR_BITS_ARB
    static constexpr GLenum RedBits                                    =     0x2015; // WGL_RED_BITS_ARB
    static constexpr GLenum RedShift                                   =     0x2016; // WGL_RED_SHIFT_ARB
    static constexpr GLenum GreenBits                                  =     0x2017; // WGL_GREEN_BITS_ARB
    static constexpr GLenum GreenShift                                 =     0x2018; // WGL_GREEN_SHIFT_ARB
    static constexpr GLenum BlueBits                                   =     0x2019; // WGL_BLUE_BITS_ARB
    static constexpr GLenum BlueShift                                  =     0x201A; // WGL_BLUE_SHIFT_ARB
    static constexpr GLenum AlphaBits                                  =     0x201B; // WGL_ALPHA_BITS_ARB
    static constexpr GLenum AlphaShift                                 =     0x201C; // WGL_ALPHA_SHIFT_ARB
    static constexpr GLenum AccumBits                                  =     0x201D; // WGL_ACCUM_BITS_ARB
    static constexpr GLenum AccumRedBits                               =     0x201E; // WGL_ACCUM_RED_BITS_ARB
    static constexpr GLenum AccumGreenBits                             =     0x201F; // WGL_ACCUM_GREEN_BITS_ARB
    static constexpr GLenum AccumBlueBits                              =     0x2020; // WGL_ACCUM_BLUE_BITS_ARB
    static constexpr GLenum AccumAlphaBits                             =     0x2021; // WGL_ACCUM_ALPHA_BITS_ARB
    static constexpr GLenum DepthBits                                  =     0x2022; // WGL_DEPTH_BITS_ARB
    static constexpr GLenum StencilBits                                =     0x2023; // WGL_STENCIL_BITS_ARB
    static constexpr GLenum AuxBuffers                                 =     0x2024; // WGL_AUX_BUFFERS_ARB
    static constexpr GLenum NoAcceleration                             =     0x2025; // WGL_NO_ACCELERATION_ARB
    static constexpr GLenum GenericAcceleration                        =     0x2026; // WGL_GENERIC_ACCELERATION_ARB
    static constexpr GLenum FullAcceleration                           =     0x2027; // WGL_FULL_ACCELERATION_ARB
    static constexpr GLenum SwapExchange                               =     0x2028; // WGL_SWAP_EXCHANGE_ARB
    static constexpr GLenum SwapCopy                                   =     0x2029; // WGL_SWAP_COPY_ARB
    static constexpr GLenum SwapUndefined                              =     0x202A; // WGL_SWAP_UNDEFINED_ARB
    static constexpr GLenum TypeRGBA                                   =     0x202B; // WGL_TYPE_RGBA_ARB
    static constexpr GLenum TypeColorindex                             =     0x202C; // WGL_TYPE_COLORINDEX_ARB
    static constexpr GLenum DrawToPbuffer                              =     0x202D; // WGL_DRAW_TO_PBUFFER_ARB
    static constexpr GLenum MaxPbufferPixelS                           =     0x202E; // WGL_MAX_PBUFFER_PIXELS_ARB
    static constexpr GLenum MaxPbufferWidth                            =     0x202F; // WGL_MAX_PBUFFER_WIDTH_ARB
    static constexpr GLenum MaxPbufferHeight                           =     0x2030; // WGL_MAX_PBUFFER_HEIGHT_ARB
    static constexpr GLenum OptimalPbufferWidth                        =     0x2031; // WGL_OPTIMAL_PBUFFER_WIDTH_EXT
    static constexpr GLenum OptimalPbufferHeight                       =     0x2032; // WGL_OPTIMAL_PBUFFER_HEIGHT_EXT
    static constexpr GLenum PbufferLargest                             =     0x2033; // WGL_PBUFFER_LARGEST_ARB
    static constexpr GLenum PbufferWidth                               =     0x2034; // WGL_PBUFFER_WIDTH_ARB
    static constexpr GLenum PbufferHeight                              =     0x2035; // WGL_PBUFFER_HEIGHT_ARB
    static constexpr GLenum PbufferLost                                =     0x2036; // WGL_PBUFFER_LOST_ARB
    static constexpr GLenum TransparentRedValue                        =     0x2037; // WGL_TRANSPARENT_RED_VALUE_ARB
    static constexpr GLenum TransparentGreenValue                      =     0x2038; // WGL_TRANSPARENT_GREEN_VALUE_ARB
    static constexpr GLenum TransparentBlueValue                       =     0x2039; // WGL_TRANSPARENT_BLUE_VALUE_ARB
    static constexpr GLenum TransparentAlphaValue                      =     0x203A; // WGL_TRANSPARENT_ALPHA_VALUE_ARB
    static constexpr GLenum TransparentIndexValue                      =     0x203B; // WGL_TRANSPARENT_INDEX_VALUE_ARB
    static constexpr GLenum DepthFloat                                 =     0x2040; // WGL_DEPTH_FLOAT_EXT
    static constexpr GLenum SampleBuffersARB                           =     0x2041; // WGL_SAMPLE_BUFFERS_ARB
    static constexpr GLenum CoverageSampleS                            =     0x2042; // WGL_COVERAGE_SAMPLES_NV
    static constexpr GLenum SampleSARB                                 =     0x2042; // WGL_SAMPLES_ARB
    static constexpr GLenum ErrorInvalidPixelType                      =     0x2043; // ERROR_INVALID_PIXEL_TYPE_ARB
    static constexpr GLenum GenlockSourceMultiviewI3D                  =     0x2044; // WGL_GENLOCK_SOURCE_MULTIVIEW_I3D
    static constexpr GLenum GenlockSourceExternalSyncI3D               =     0x2045; // WGL_GENLOCK_SOURCE_EXTERNAL_SYNC_I3D
    static constexpr GLenum GenlockSourceExternalFieldI3D              =     0x2046; // WGL_GENLOCK_SOURCE_EXTERNAL_FIELD_I3D
    static constexpr GLenum GenlockSourceExternalTtlI3D                =     0x2047; // WGL_GENLOCK_SOURCE_EXTERNAL_TTL_I3D
    static constexpr GLenum GenlockSourceDigitalSyncI3D                =     0x2048; // WGL_GENLOCK_SOURCE_DIGITAL_SYNC_I3D
    static constexpr GLenum GenlockSourceDigitalFieldI3D               =     0x2049; // WGL_GENLOCK_SOURCE_DIGITAL_FIELD_I3D
    static constexpr GLenum GenlockSourceEdgeFallingI3D                =     0x204A; // WGL_GENLOCK_SOURCE_EDGE_FALLING_I3D
    static constexpr GLenum GenlockSourceEdgeRisingI3D                 =     0x204B; // WGL_GENLOCK_SOURCE_EDGE_RISING_I3D
    static constexpr GLenum GenlockSourceEdgeBothI3D                   =     0x204C; // WGL_GENLOCK_SOURCE_EDGE_BOTH_I3D
    static constexpr GLenum GammaTableSizeI3D                          =     0x204E; // WGL_GAMMA_TABLE_SIZE_I3D
    static constexpr GLenum GammaExcludeDesktopI3D                     =     0x204F; // WGL_GAMMA_EXCLUDE_DESKTOP_I3D
    static constexpr GLenum DigitalVideoCursorAlphaFramebufferI3D      =     0x2050; // WGL_DIGITAL_VIDEO_CURSOR_ALPHA_FRAMEBUFFER_I3D
    static constexpr GLenum DigitalVideoCursorAlphaValueI3D            =     0x2051; // WGL_DIGITAL_VIDEO_CURSOR_ALPHA_VALUE_I3D
    static constexpr GLenum DigitalVideoCursorIncludedI3D              =     0x2052; // WGL_DIGITAL_VIDEO_CURSOR_INCLUDED_I3D
    static constexpr GLenum DigitalVideoGammaCorrectedI3D              =     0x2053; // WGL_DIGITAL_VIDEO_GAMMA_CORRECTED_I3D
    static constexpr GLenum ErrorIncompatibleDeviceContextS            =     0x2054; // ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB
    static constexpr GLenum StereoEmitterEnable3DL                     =     0x2055; // WGL_STEREO_EMITTER_ENABLE_3DL
    static constexpr GLenum StereoEmitterDisable3DL                    =     0x2056; // WGL_STEREO_EMITTER_DISABLE_3DL
    static constexpr GLenum StereoPolarityNormal3DL                    =     0x2057; // WGL_STEREO_POLARITY_NORMAL_3DL
    static constexpr GLenum StereoPolarityInvert3DL                    =     0x2058; // WGL_STEREO_POLARITY_INVERT_3DL
    static constexpr GLenum SampleBuffers3DFX                          =     0x2060; // WGL_SAMPLE_BUFFERS_3DFX
    static constexpr GLenum SampleS3DFX                                =     0x2061; // WGL_SAMPLES_3DFX
    static constexpr GLenum BindToTextureRGB                           =     0x2070; // WGL_BIND_TO_TEXTURE_RGB_ARB
    static constexpr GLenum BindToTextureRGBA                          =     0x2071; // WGL_BIND_TO_TEXTURE_RGBA_ARB
    static constexpr GLenum TextureFormat                              =     0x2072; // WGL_TEXTURE_FORMAT_ARB
    static constexpr GLenum TextureTarget                              =     0x2073; // WGL_TEXTURE_TARGET_ARB
    static constexpr GLenum MipmapTexture                              =     0x2074; // WGL_MIPMAP_TEXTURE_ARB
    static constexpr GLenum TextureRGB                                 =     0x2075; // WGL_TEXTURE_RGB_ARB
    static constexpr GLenum TextureRGBA                                =     0x2076; // WGL_TEXTURE_RGBA_ARB
    static constexpr GLenum NoTexture                                  =     0x2077; // WGL_NO_TEXTURE_ARB
    static constexpr GLenum TextureCubeMap                             =     0x2078; // WGL_TEXTURE_CUBE_MAP_ARB
    static constexpr GLenum Texture1D                                  =     0x2079; // WGL_TEXTURE_1D_ARB
    static constexpr GLenum Texture2D                                  =     0x207A; // WGL_TEXTURE_2D_ARB
    static constexpr GLenum MipmapLevel                                =     0x207B; // WGL_MIPMAP_LEVEL_ARB
    static constexpr GLenum CubeMapFace                                =     0x207C; // WGL_CUBE_MAP_FACE_ARB
    static constexpr GLenum TextureCubeMapPositiveX                    =     0x207D; // WGL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB
    static constexpr GLenum TextureCubeMapNegativeX                    =     0x207E; // WGL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB
    static constexpr GLenum TextureCubeMapPositiveY                    =     0x207F; // WGL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB
    static constexpr GLenum TextureCubeMapNegativeY                    =     0x2080; // WGL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB
    static constexpr GLenum TextureCubeMapPositiveZ                    =     0x2081; // WGL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB
    static constexpr GLenum TextureCubeMapNegativeZ                    =     0x2082; // WGL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB
    static constexpr GLenum FrontLeft                                  =     0x2083; // WGL_FRONT_LEFT_ARB
    static constexpr GLenum FrontRight                                 =     0x2084; // WGL_FRONT_RIGHT_ARB
    static constexpr GLenum BackLeft                                   =     0x2085; // WGL_BACK_LEFT_ARB
    static constexpr GLenum BackRight                                  =     0x2086; // WGL_BACK_RIGHT_ARB
    static constexpr GLenum Aux0                                       =     0x2087; // WGL_AUX0_ARB
    static constexpr GLenum Aux1                                       =     0x2088; // WGL_AUX1_ARB
    static constexpr GLenum Aux2                                       =     0x2089; // WGL_AUX2_ARB
    static constexpr GLenum Aux3                                       =     0x208A; // WGL_AUX3_ARB
    static constexpr GLenum Aux4                                       =     0x208B; // WGL_AUX4_ARB
    static constexpr GLenum Aux5                                       =     0x208C; // WGL_AUX5_ARB
    static constexpr GLenum Aux6                                       =     0x208D; // WGL_AUX6_ARB
    static constexpr GLenum Aux7                                       =     0x208E; // WGL_AUX7_ARB
    static constexpr GLenum Aux8                                       =     0x208F; // WGL_AUX8_ARB
    static constexpr GLenum Aux9                                       =     0x2090; // WGL_AUX9_ARB
    static constexpr GLenum ContextMajorVersion                        =     0x2091; // WGL_CONTEXT_MAJOR_VERSION_ARB
    static constexpr GLenum ContextMinorVersion                        =     0x2092; // WGL_CONTEXT_MINOR_VERSION_ARB
    static constexpr GLenum ContextLayerPlane                          =     0x2093; // WGL_CONTEXT_LAYER_PLANE_ARB
    static constexpr GLenum ContextFlags                               =     0x2094; // WGL_CONTEXT_FLAGS_ARB
    static constexpr GLenum ErrorInvalidVersion                        =     0x2095; // ERROR_INVALID_VERSION_ARB
    static constexpr GLenum ErrorInvalidProfile                        =     0x2096; // ERROR_INVALID_PROFILE_ARB
    static constexpr GLenum ContextReleaseBehavior                     =     0x2097; // WGL_CONTEXT_RELEASE_BEHAVIOR_ARB
    static constexpr GLenum ContextReleaseBehaviorFlush                =     0x2098; // WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB
    static constexpr GLenum BindToTextureRectangleRGB                  =     0x20A0; // WGL_BIND_TO_TEXTURE_RECTANGLE_RGB_NV
    static constexpr GLenum BindToTextureRectangleRGBA                 =     0x20A1; // WGL_BIND_TO_TEXTURE_RECTANGLE_RGBA_NV
    static constexpr GLenum TextureRectangleNV                         =     0x20A2; // WGL_TEXTURE_RECTANGLE_NV
    static constexpr GLenum BindToTextureDepth                         =     0x20A3; // WGL_BIND_TO_TEXTURE_DEPTH_NV
    static constexpr GLenum BindToTextureRectangleDepth                =     0x20A4; // WGL_BIND_TO_TEXTURE_RECTANGLE_DEPTH_NV
    static constexpr GLenum DepthTextureFormat                         =     0x20A5; // WGL_DEPTH_TEXTURE_FORMAT_NV
    static constexpr GLenum TextureDepthComponent                      =     0x20A6; // WGL_TEXTURE_DEPTH_COMPONENT_NV
    static constexpr GLenum DepthComponent                             =     0x20A7; // WGL_DEPTH_COMPONENT_NV
    static constexpr GLenum TypeRGBAUnsignedFloat                      =     0x20A8; // WGL_TYPE_RGBA_UNSIGNED_FLOAT_EXT
    static constexpr GLenum FramebufferSRGBCapable                     =     0x20A9; // WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB
    static constexpr GLenum ContextMultigpuAttrib                      =     0x20AA; // WGL_CONTEXT_MULTIGPU_ATTRIB_NV
    static constexpr GLenum ContextMultigpuAttribSingle                =     0x20AB; // WGL_CONTEXT_MULTIGPU_ATTRIB_SINGLE_NV
    static constexpr GLenum ContextMultigpuAttribAfr                   =     0x20AC; // WGL_CONTEXT_MULTIGPU_ATTRIB_AFR_NV
    static constexpr GLenum ContextMultigpuAttribMulticast             =     0x20AD; // WGL_CONTEXT_MULTIGPU_ATTRIB_MULTICAST_NV
    static constexpr GLenum ContextMultigpuAttribMultiDisplayMulticast =     0x20AE; // WGL_CONTEXT_MULTIGPU_ATTRIB_MULTI_DISPLAY_MULTICAST_NV
    static constexpr GLenum FloatComponents                            =     0x20B0; // WGL_FLOAT_COMPONENTS_NV
    static constexpr GLenum BindToTextureRectangleFloatR               =     0x20B1; // WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_R_NV
    static constexpr GLenum BindToTextureRectangleFloatRg              =     0x20B2; // WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_RG_NV
    static constexpr GLenum BindToTextureRectangleFloatRGB             =     0x20B3; // WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_RGB_NV
    static constexpr GLenum BindToTextureRectangleFloatRGBA            =     0x20B4; // WGL_BIND_TO_TEXTURE_RECTANGLE_FLOAT_RGBA_NV
    static constexpr GLenum TextureFloatR                              =     0x20B5; // WGL_TEXTURE_FLOAT_R_NV
    static constexpr GLenum TextureFloatRg                             =     0x20B6; // WGL_TEXTURE_FLOAT_RG_NV
    static constexpr GLenum TextureFloatRGB                            =     0x20B7; // WGL_TEXTURE_FLOAT_RGB_NV
    static constexpr GLenum TextureFloatRGBA                           =     0x20B8; // WGL_TEXTURE_FLOAT_RGBA_NV
    static constexpr GLenum ColorSampleS                               =     0x20B9; // WGL_COLOR_SAMPLES_NV
    static constexpr GLenum BindToVideoRGB                             =     0x20C0; // WGL_BIND_TO_VIDEO_RGB_NV
    static constexpr GLenum BindToVideoRGBA                            =     0x20C1; // WGL_BIND_TO_VIDEO_RGBA_NV
    static constexpr GLenum BindToVideoRGBAndDepth                     =     0x20C2; // WGL_BIND_TO_VIDEO_RGB_AND_DEPTH_NV
    static constexpr GLenum VideoOutColor                              =     0x20C3; // WGL_VIDEO_OUT_COLOR_NV
    static constexpr GLenum VideoOutAlpha                              =     0x20C4; // WGL_VIDEO_OUT_ALPHA_NV
    static constexpr GLenum VideoOutDepth                              =     0x20C5; // WGL_VIDEO_OUT_DEPTH_NV
    static constexpr GLenum VideoOutColorAndAlpha                      =     0x20C6; // WGL_VIDEO_OUT_COLOR_AND_ALPHA_NV
    static constexpr GLenum VideoOutColorAndDepth                      =     0x20C7; // WGL_VIDEO_OUT_COLOR_AND_DEPTH_NV
    static constexpr GLenum VideoOutFrame                              =     0x20C8; // WGL_VIDEO_OUT_FRAME
    static constexpr GLenum VideoOutField1                             =     0x20C9; // WGL_VIDEO_OUT_FIELD_1
    static constexpr GLenum VideoOutField2                             =     0x20CA; // WGL_VIDEO_OUT_FIELD_2
    static constexpr GLenum VideoOutStackedFields1_2                   =     0x20CB; // WGL_VIDEO_OUT_STACKED_FIELDS_1_2
    static constexpr GLenum VideoOutStackedFields2_1                   =     0x20CC; // WGL_VIDEO_OUT_STACKED_FIELDS_2_1
    static constexpr GLenum UniqueId                                   =     0x20CE; // WGL_UNIQUE_ID_NV
    static constexpr GLenum NumVideoCaptureSlots                       =     0x20CF; // WGL_NUM_VIDEO_CAPTURE_SLOTS_NV
    static constexpr GLenum ErrorIncompatibleAffinityMasks             =     0x20D0; // ERROR_INCOMPATIBLE_AFFINITY_MASKS_NV
    static constexpr GLenum ErrorMissingAffinityMask                   =     0x20D1; // ERROR_MISSING_AFFINITY_MASK_NV
    static constexpr GLenum NumVideoSlots                              =     0x20F0; // WGL_NUM_VIDEO_SLOTS_NV
    static constexpr GLenum TypeRGBAFloat                              =     0x21A0; // WGL_TYPE_RGBA_FLOAT_ARB
    static constexpr GLenum GPUFastestTargetGPUs                       =     0x21A2; // WGL_GPU_FASTEST_TARGET_GPUS_AMD
    static constexpr GLenum GPURam                                     =     0x21A3; // WGL_GPU_RAM_AMD
    static constexpr GLenum GPUClock                                   =     0x21A4; // WGL_GPU_CLOCK_AMD
    static constexpr GLenum GPUNumPipes                                =     0x21A5; // WGL_GPU_NUM_PIPES_AMD
    static constexpr GLenum TextureRectangleATI                        =     0x21A5; // WGL_TEXTURE_RECTANGLE_ATI - Duplicates unrelated WGL_GPU_NUM_PIPES_AMD
    static constexpr GLenum GPUNumSIMD                                 =     0x21A6; // WGL_GPU_NUM_SIMD_AMD
    static constexpr GLenum GPUNumRB                                   =     0x21A7; // WGL_GPU_NUM_RB_AMD
    static constexpr GLenum GPUNumSPI                                  =     0x21A8; // WGL_GPU_NUM_SPI_AMD
    static constexpr GLenum ColorspaceSRGB                             =     0x3089; // WGL_COLORSPACE_SRGB_EXT
    static constexpr GLenum ColorspaceLinear                           =     0x308A; // WGL_COLORSPACE_LINEAR_EXT
    static constexpr GLenum Colorspace                                 =     0x309D; // WGL_COLORSPACE_EXT
    static constexpr GLenum ContextOpenglNoError                       =     0x31B3; // WGL_CONTEXT_OPENGL_NO_ERROR_ARB
    static constexpr GLenum SwapOverlay14                              = 0x00004000; // WGL_SWAP_OVERLAY14
    static constexpr GLenum SwapOverlay15                              = 0x00008000; // WGL_SWAP_OVERLAY15
    static constexpr GLenum LoseContextOnReset                         =     0x8252; // WGL_LOSE_CONTEXT_ON_RESET_ARB
    static constexpr GLenum ContextResetNotificationStrategy           =     0x8256; // WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB
    static constexpr GLenum NoResetNotification                        =     0x8261; // WGL_NO_RESET_NOTIFICATION_ARB
    static constexpr GLenum ContextProfileMask                         =     0x9126; // WGL_CONTEXT_PROFILE_MASK_ARB
    static constexpr GLenum SwapUnderlay1                              = 0x00010000; // WGL_SWAP_UNDERLAY1
    static constexpr GLenum SwapUnderlay2                              = 0x00020000; // WGL_SWAP_UNDERLAY2
    static constexpr GLenum SwapUnderlay3                              = 0x00040000; // WGL_SWAP_UNDERLAY3
    static constexpr GLenum SwapUnderlay4                              = 0x00080000; // WGL_SWAP_UNDERLAY4
    static constexpr GLenum SwapUnderlay5                              = 0x00100000; // WGL_SWAP_UNDERLAY5
    static constexpr GLenum SwapUnderlay6                              = 0x00200000; // WGL_SWAP_UNDERLAY6
    static constexpr GLenum SwapUnderlay7                              = 0x00400000; // WGL_SWAP_UNDERLAY7
    static constexpr GLenum SwapUnderlay8                              = 0x00800000; // WGL_SWAP_UNDERLAY8
    static constexpr GLenum SwapUnderlay9                              = 0x01000000; // WGL_SWAP_UNDERLAY9
    static constexpr GLenum SwapUnderlay10                             = 0x02000000; // WGL_SWAP_UNDERLAY10
    static constexpr GLenum SwapUnderlay11                             = 0x04000000; // WGL_SWAP_UNDERLAY11
    static constexpr GLenum SwapUnderlay12                             = 0x08000000; // WGL_SWAP_UNDERLAY12
    static constexpr GLenum SwapUnderlay13                             = 0x10000000; // WGL_SWAP_UNDERLAY13
    static constexpr GLenum SwapUnderlay14                             = 0x20000000; // WGL_SWAP_UNDERLAY14
    static constexpr GLenum SwapUnderlay15                             = 0x40000000; // WGL_SWAP_UNDERLAY15
}; // struct Global

////////////////////////////////////////////////////////////////////////////////
// API Command Groups
////////////////////////////////////////////////////////////////////////////////
// Main API specification.
struct WGL {
    // Public Members

    // WGL 1.0
    int (APIENTRY *ChoosePixelFormat)(HDC hDc, const PIXELFORMATDESCRIPTOR *pPfd);
    int (APIENTRY *DescribePixelFormat)(HDC hdc, int ipfd, UINT cjpfd,
                                        const PIXELFORMATDESCRIPTOR *ppfd);
    UINT (APIENTRY *GetEnhMetaFilePixelFormat)(HENHMETAFILE hemf,
                                               const PIXELFORMATDESCRIPTOR *ppfd);
    int (APIENTRY *GetPixelFormat)(HDC hdc);
    BOOL (APIENTRY *SetPixelFormat)(HDC hdc, int ipfd,
                                    const PIXELFORMATDESCRIPTOR *ppfd);
    BOOL (APIENTRY *SwapBuffers)(HDC hdc);
    BOOL (APIENTRY *wglCopyContext)(HGLRC hglrcSrc, HGLRC hglrcDst, UINT mask);
    HGLRC (APIENTRY *wglCreateContext)(HDC hDc);
    HGLRC (APIENTRY *wglCreateLayerContext)(HDC hDc, int level);
    BOOL (APIENTRY *wglDeleteContext)(HGLRC oldContext);
    BOOL (APIENTRY *wglDescribeLayerPlane)(HDC hDc, int pixelFormat,
                                           int layerPlane, UINT nBytes,
                                           const LAYERPLANEDESCRIPTOR *plpd);
    HGLRC (APIENTRY *wglGetCurrentContext)();
    HDC (APIENTRY *wglGetCurrentDC)();
    int (APIENTRY *wglGetLayerPaletteEntries)(HDC hdc, int iLayerPlane,
                                              int iStart, int cEntries,
                                              const COLORREF *pcr);
    PROC (APIENTRY *wglGetProcAddress)(LPCSTR lpszProc);
    BOOL (APIENTRY *wglMakeCurrent)(HDC hDc, HGLRC newContext);
    BOOL (APIENTRY *wglRealizeLayerPalette)(HDC hdc, int iLayerPlane,
                                            BOOL bRealize);
    int (APIENTRY *wglSetLayerPaletteEntries)(HDC hdc, int iLayerPlane,
                                              int iStart, int cEntries,
                                              const COLORREF *pcr);
    BOOL (APIENTRY *wglShareLists)(HGLRC hrcSrvShare, HGLRC hrcSrvSource);
    BOOL (APIENTRY *wglSwapLayerBuffers)(HDC hdc, UINT fuFlags);
    BOOL (APIENTRY *wglUseFontBitmaps)(HDC hDC, DWORD first, DWORD count,
                                       DWORD listBase);
    BOOL (APIENTRY *wglUseFontBitmapsA)(HDC hDC, DWORD first, DWORD count,
                                        DWORD listBase);
    BOOL (APIENTRY *wglUseFontBitmapsW)(HDC hDC, DWORD first, DWORD count,
                                        DWORD listBase);
    BOOL (APIENTRY *wglUseFontOutlines)(HDC hDC, DWORD first, DWORD count,
                                        DWORD listBase, FLOAT deviation,
                                        FLOAT extrusion, int format,
                                        LPGLYPHMETRICSFLOAT lpgmf);
    BOOL (APIENTRY *wglUseFontOutlinesA)(HDC hDC, DWORD first, DWORD count,
                                         DWORD listBase, FLOAT deviation,
                                         FLOAT extrusion, int format,
                                         LPGLYPHMETRICSFLOAT lpgmf);
    BOOL (APIENTRY *wglUseFontOutlinesW)(HDC hDC, DWORD first, DWORD count,
                                         DWORD listBase, FLOAT deviation,
                                         FLOAT extrusion, int format,
                                         LPGLYPHMETRICSFLOAT lpgmf);

    // Construction
    WGL();

    // Operations
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct WGL

// WGL_3DL_stereo_control extension API specification.
struct _3DLStereoControl {
    // Public Members
    BOOL (APIENTRY *wglSetStereoEmitterState3DL)(HDC hDC, UINT uState);

    // Construction
    _3DLStereoControl();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct _3DLStereoControl

// WGL_AMD_gpu_association extension API specification.
struct AMDGPUAssociation {
    // Public Members
    VOID (APIENTRY *wglBlitContextFramebuffer)(HGLRC dstCtx, GLint srcX0,
                                               GLint srcY0, GLint srcX1,
                                               GLint srcY1, GLint dstX0,
                                               GLint dstY0, GLint dstX1,
                                               GLint dstY1, GLbitfield mask,
                                               GLenum filter);
    HGLRC (APIENTRY *wglCreateAssociatedContext)(UINT id);
    HGLRC (APIENTRY *wglCreateAssociatedContextAttribs)(UINT id,
                                                        HGLRC hShareContext,
                                                        const int *attribList);
    BOOL (APIENTRY *wglDeleteAssociatedContext)(HGLRC hglrc);
    UINT (APIENTRY *wglGetContextGPUID)(HGLRC hglrc);
    HGLRC (APIENTRY *wglGetCurrentAssociatedContext)();
    UINT (APIENTRY *wglGetGPUIDs)(UINT maxCount, UINT *ids);
    INT (APIENTRY *wglGetGPUInfo)(UINT id, INT property, GLenum dataType,
                                  UINT size, void *data);
    BOOL (APIENTRY *wglMakeAssociatedContextCurrent)(HGLRC hglrc);

    // Construction
    AMDGPUAssociation();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct AMDGPUAssociation

// WGL_ARB_buffer_region extension API specification.
struct ARBBufferRegion {
    // Public Members
    HANDLE (APIENTRY *wglCreateBufferRegion)(HDC hDC, int iLayerPlane,
                                             UINT uType);
    VOID (APIENTRY *wglDeleteBufferRegion)(HANDLE hRegion);
    BOOL (APIENTRY *wglRestoreBufferRegion)(HANDLE hRegion, int x, int y,
                                            int width, int height, int xSrc,
                                            int ySrc);
    BOOL (APIENTRY *wglSaveBufferRegion)(HANDLE hRegion, int x, int y, int width,
                                         int height);

    // Construction
    ARBBufferRegion();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct ARBBufferRegion

// WGL_ARB_create_context extension API specification.
struct ARBCreateContext {
    // Public Members
    HGLRC (APIENTRY *wglCreateContextAttribs)(HDC hDC, HGLRC hShareContext,
                                              const int *attribList);

    // Construction
    ARBCreateContext();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct ARBCreateContext

// WGL_ARB_extensions_string extension API specification.
struct ARBExtensionsString {
    // Public Members
    const char *(APIENTRY *wglGetExtensionsString)(HDC hdc);

    // Construction
    ARBExtensionsString();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct ARBExtensionsString

// WGL_ARB_make_current_read extension API specification.
struct ARBMakeCurrentRead {
    // Public Members
    HDC (APIENTRY *wglGetCurrentReadDC)();
    BOOL (APIENTRY *wglMakeContextCurrent)(HDC hDrawDC, HDC hReadDC, HGLRC hglrc);

    // Construction
    ARBMakeCurrentRead();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct ARBMakeCurrentRead

// WGL_ARB_pbuffer extension API specification.
struct ARBPbuffer {
    // Public Members
    HPBUFFERARB (APIENTRY *wglCreatePbuffer)(HDC hDC, int iPixelFormat,
                                             int iWidth, int iHeight,
                                             const int *piAttribList);
    BOOL (APIENTRY *wglDestroyPbuffer)(HPBUFFERARB hPbuffer);
    HDC (APIENTRY *wglGetPbufferDC)(HPBUFFERARB hPbuffer);
    BOOL (APIENTRY *wglQueryPbuffer)(HPBUFFERARB hPbuffer, int iAttribute,
                                     int *piValue);
    int (APIENTRY *wglReleasePbufferDC)(HPBUFFERARB hPbuffer, HDC hDC);

    // Construction
    ARBPbuffer();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct ARBPbuffer

// WGL_ARB_pixel_format extension API specification.
struct ARBPixelFormat {
    // Public Members
    BOOL (APIENTRY *wglChoosePixelFormat)(HDC hdc, const int *piAttribIList,
                                          const FLOAT *pfAttribFList,
                                          UINT nMaxFormats, int *piFormats,
                                          UINT *nNumFormats);
    BOOL (APIENTRY *wglGetPixelFormatAttribfv)(HDC hdc, int iPixelFormat,
                                               int iLayerPlane, UINT nAttributes,
                                               const int *piAttributes,
                                               FLOAT *pfValues);
    BOOL (APIENTRY *wglGetPixelFormatAttribiv)(HDC hdc, int iPixelFormat,
                                               int iLayerPlane, UINT nAttributes,
                                               const int *piAttributes,
                                               int *piValues);

    // Construction
    ARBPixelFormat();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct ARBPixelFormat

// WGL_ARB_render_texture extension API specification.
struct ARBRenderTexture {
    // Public Members
    BOOL (APIENTRY *wglBindTexImage)(HPBUFFERARB hPbuffer, int iBuffer);
    BOOL (APIENTRY *wglReleaseTexImage)(HPBUFFERARB hPbuffer, int iBuffer);
    BOOL (APIENTRY *wglSetPbufferAttrib)(HPBUFFERARB hPbuffer,
                                         const int *piAttribList);

    // Construction
    ARBRenderTexture();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct ARBRenderTexture

// WGL_EXT_display_color_table extension API specification.
struct EXTDisplayColorTable {
    // Public Members
    GLboolean (APIENTRY *wglBindDisplayColorTable)(GLushort id);
    GLboolean (APIENTRY *wglCreateDisplayColorTable)(GLushort id);
    VOID (APIENTRY *wglDestroyDisplayColorTable)(GLushort id);
    GLboolean (APIENTRY *wglLoadDisplayColorTable)(const GLushort *table,
                                                   GLuint length);

    // Construction
    EXTDisplayColorTable();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct EXTDisplayColorTable

// WGL_EXT_extensions_string extension API specification.
struct EXTExtensionsString {
    // Public Members
    const char *(APIENTRY *wglGetExtensionsString)();

    // Construction
    EXTExtensionsString();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct EXTExtensionsString

// WGL_EXT_make_current_read extension API specification.
struct EXTMakeCurrentRead {
    // Public Members
    HDC (APIENTRY *wglGetCurrentReadDC)();
    BOOL (APIENTRY *wglMakeContextCurrent)(HDC hDrawDC, HDC hReadDC, HGLRC hglrc);

    // Construction
    EXTMakeCurrentRead();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct EXTMakeCurrentRead

// WGL_EXT_pbuffer extension API specification.
struct EXTPbuffer {
    // Public Members
    HPBUFFEREXT (APIENTRY *wglCreatePbuffer)(HDC hDC, int iPixelFormat,
                                             int iWidth, int iHeight,
                                             const int *piAttribList);
    BOOL (APIENTRY *wglDestroyPbuffer)(HPBUFFEREXT hPbuffer);
    HDC (APIENTRY *wglGetPbufferDC)(HPBUFFEREXT hPbuffer);
    BOOL (APIENTRY *wglQueryPbuffer)(HPBUFFEREXT hPbuffer, int iAttribute,
                                     int *piValue);
    int (APIENTRY *wglReleasePbufferDC)(HPBUFFEREXT hPbuffer, HDC hDC);

    // Construction
    EXTPbuffer();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct EXTPbuffer

// WGL_EXT_pixel_format extension API specification.
struct EXTPixelFormat {
    // Public Members
    BOOL (APIENTRY *wglChoosePixelFormat)(HDC hdc, const int *piAttribIList,
                                          const FLOAT *pfAttribFList,
                                          UINT nMaxFormats, int *piFormats,
                                          UINT *nNumFormats);
    BOOL (APIENTRY *wglGetPixelFormatAttribfv)(HDC hdc, int iPixelFormat,
                                               int iLayerPlane, UINT nAttributes,
                                               int *piAttributes,
                                               FLOAT *pfValues);
    BOOL (APIENTRY *wglGetPixelFormatAttribiv)(HDC hdc, int iPixelFormat,
                                               int iLayerPlane, UINT nAttributes,
                                               int *piAttributes, int *piValues);

    // Construction
    EXTPixelFormat();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct EXTPixelFormat

// WGL_EXT_swap_control extension API specification.
struct EXTSwapControl {
    // Public Members
    int (APIENTRY *wglGetSwapInterval)();
    BOOL (APIENTRY *wglSwapInterval)(int interval);

    // Construction
    EXTSwapControl();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct EXTSwapControl

// WGL_I3D_digital_video_control extension API specification.
struct I3DDigitalVideoControl {
    // Public Members
    BOOL (APIENTRY *wglGetDigitalVideoParameters)(HDC hDC, int iAttribute,
                                                  int *piValue);
    BOOL (APIENTRY *wglSetDigitalVideoParameters)(HDC hDC, int iAttribute,
                                                  const int *piValue);

    // Construction
    I3DDigitalVideoControl();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct I3DDigitalVideoControl

// WGL_I3D_gamma extension API specification.
struct I3DGamma {
    // Public Members
    BOOL (APIENTRY *wglGetGammaTable)(HDC hDC, int iEntries, USHORT *puRed,
                                      USHORT *puGreen, USHORT *puBlue);
    BOOL (APIENTRY *wglGetGammaTableParameters)(HDC hDC, int iAttribute,
                                                int *piValue);
    BOOL (APIENTRY *wglSetGammaTable)(HDC hDC, int iEntries, const USHORT *puRed,
                                      const USHORT *puGreen,
                                      const USHORT *puBlue);
    BOOL (APIENTRY *wglSetGammaTableParameters)(HDC hDC, int iAttribute,
                                                const int *piValue);

    // Construction
    I3DGamma();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct I3DGamma

// WGL_I3D_genlock extension API specification.
struct I3DGenlock {
    // Public Members
    BOOL (APIENTRY *wglDisableGenlock)(HDC hDC);
    BOOL (APIENTRY *wglEnableGenlock)(HDC hDC);
    BOOL (APIENTRY *wglGenlockSampleRate)(HDC hDC, UINT uRate);
    BOOL (APIENTRY *wglGenlockSourceDelay)(HDC hDC, UINT uDelay);
    BOOL (APIENTRY *wglGenlockSourceEdge)(HDC hDC, UINT uEdge);
    BOOL (APIENTRY *wglGenlockSource)(HDC hDC, UINT uSource);
    BOOL (APIENTRY *wglGetGenlockSampleRate)(HDC hDC, UINT *uRate);
    BOOL (APIENTRY *wglGetGenlockSourceDelay)(HDC hDC, UINT *uDelay);
    BOOL (APIENTRY *wglGetGenlockSourceEdge)(HDC hDC, UINT *uEdge);
    BOOL (APIENTRY *wglGetGenlockSource)(HDC hDC, UINT *uSource);
    BOOL (APIENTRY *wglIsEnabledGenlock)(HDC hDC, BOOL *pFlag);
    BOOL (APIENTRY *wglQueryGenlockMaxSourceDelay)(HDC hDC, UINT *uMaxLineDelay,
                                                   UINT *uMaxPixelDelay);

    // Construction
    I3DGenlock();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct I3DGenlock

// WGL_I3D_image_buffer extension API specification.
struct I3DImageBuffer {
    // Public Members
    BOOL (APIENTRY *wglAssociateImageBufferEvents)(HDC hDC, const HANDLE *pEvent,
                                                   const LPVOID *pAddress,
                                                   const DWORD *pSize,
                                                   UINT count);
    LPVOID (APIENTRY *wglCreateImageBuffer)(HDC hDC, DWORD dwSize, UINT uFlags);
    BOOL (APIENTRY *wglDestroyImageBuffer)(HDC hDC, LPVOID pAddress);
    BOOL (APIENTRY *wglReleaseImageBufferEvents)(HDC hDC, const LPVOID *pAddress,
                                                 UINT count);

    // Construction
    I3DImageBuffer();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct I3DImageBuffer

// WGL_I3D_swap_frame_lock extension API specification.
struct I3DSwapFrameLock {
    // Public Members
    BOOL (APIENTRY *wglDisableFrameLock)();
    BOOL (APIENTRY *wglEnableFrameLock)();
    BOOL (APIENTRY *wglIsEnabledFrameLock)(BOOL *pFlag);
    BOOL (APIENTRY *wglQueryFrameLockMaster)(BOOL *pFlag);

    // Construction
    I3DSwapFrameLock();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct I3DSwapFrameLock

// WGL_I3D_swap_frame_usage extension API specification.
struct I3DSwapFrameUsage {
    // Public Members
    BOOL (APIENTRY *wglBeginFrameTracking)();
    BOOL (APIENTRY *wglEndFrameTracking)();
    BOOL (APIENTRY *wglGetFrameUsage)(float *pUsage);
    BOOL (APIENTRY *wglQueryFrameTracking)(DWORD *pFrameCount,
                                           DWORD *pMissedFrames,
                                           float *pLastMissedUsage);

    // Construction
    I3DSwapFrameUsage();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct I3DSwapFrameUsage

// WGL_NV_DX_interop extension API specification.
struct NVDxInterop {
    // Public Members
    BOOL (APIENTRY *wglDXCloseDevice)(HANDLE hDevice);
    BOOL (APIENTRY *wglDXLockObjects)(HANDLE hDevice, GLint count,
                                      HANDLE *hObjects);
    BOOL (APIENTRY *wglDXObjectAccess)(HANDLE hObject, GLenum access);
    HANDLE (APIENTRY *wglDXOpenDevice)(void *dxDevice);
    HANDLE (APIENTRY *wglDXRegisterObject)(HANDLE hDevice, void *dxObject,
                                           GLuint name, GLenum type,
                                           GLenum access);
    BOOL (APIENTRY *wglDXSetResourceShareHandle)(void *dxObject,
                                                 HANDLE shareHandle);
    BOOL (APIENTRY *wglDXUnlockObjects)(HANDLE hDevice, GLint count,
                                        HANDLE *hObjects);
    BOOL (APIENTRY *wglDXUnregisterObject)(HANDLE hDevice, HANDLE hObject);

    // Construction
    NVDxInterop();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct NVDxInterop

// WGL_NV_copy_image extension API specification.
struct NVCopyImage {
    // Public Members
    BOOL (APIENTRY *wglCopyImageSubData)(HGLRC hSrcRC, GLuint srcName,
                                         GLenum srcTarget, GLint srcLevel,
                                         GLint srcX, GLint srcY, GLint srcZ,
                                         HGLRC hDstRC, GLuint dstName,
                                         GLenum dstTarget, GLint dstLevel,
                                         GLint dstX, GLint dstY, GLint dstZ,
                                         GLsizei width, GLsizei height,
                                         GLsizei depth);

    // Construction
    NVCopyImage();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct NVCopyImage

// WGL_NV_delay_before_swap extension API specification.
struct NVDelayBeforeSwap {
    // Public Members
    BOOL (APIENTRY *wglDelayBeforeSwap)(HDC hDC, GLfloat seconds);

    // Construction
    NVDelayBeforeSwap();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct NVDelayBeforeSwap

// WGL_NV_gpu_affinity extension API specification.
struct NVGPUAffinity {
    // Public Members
    HDC (APIENTRY *wglCreateAffinityDC)(const HGPUNV *phGpuList);
    BOOL (APIENTRY *wglDeleteDC)(HDC hdc);
    BOOL (APIENTRY *wglEnumGpuDevices)(HGPUNV hGpu, UINT iDeviceIndex,
                                       PGPU_DEVICE lpGpuDevice);
    BOOL (APIENTRY *wglEnumGpusFromAffinityDC)(HDC hAffinityDC, UINT iGpuIndex,
                                               HGPUNV *hGpu);
    BOOL (APIENTRY *wglEnumGpus)(UINT iGpuIndex, HGPUNV *phGpu);

    // Construction
    NVGPUAffinity();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct NVGPUAffinity

// WGL_NV_present_video extension API specification.
struct NVPresentVideo {
    // Public Members
    BOOL (APIENTRY *wglBindVideoDevice)(HDC hDc, unsigned int uVideoSlot,
                                        HVIDEOOUTPUTDEVICENV hVideoDevice,
                                        const int *piAttribList);
    int (APIENTRY *wglEnumerateVideoDevices)(HDC hDc,
                                             HVIDEOOUTPUTDEVICENV *phDeviceList);
    BOOL (APIENTRY *wglQueryCurrentContext)(int iAttribute, int *piValue);

    // Construction
    NVPresentVideo();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct NVPresentVideo

// WGL_NV_swap_group extension API specification.
struct NVSwapGroup {
    // Public Members
    BOOL (APIENTRY *wglBindSwapBarrier)(GLuint group, GLuint barrier);
    BOOL (APIENTRY *wglJoinSwapGroup)(HDC hDC, GLuint group);
    BOOL (APIENTRY *wglQueryFrameCount)(HDC hDC, GLuint *count);
    BOOL (APIENTRY *wglQueryMaxSwapGroups)(HDC hDC, GLuint *maxGroups,
                                           GLuint *maxBarriers);
    BOOL (APIENTRY *wglQuerySwapGroup)(HDC hDC, GLuint *group, GLuint *barrier);
    BOOL (APIENTRY *wglResetFrameCount)(HDC hDC);

    // Construction
    NVSwapGroup();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct NVSwapGroup

// WGL_NV_vertex_array_range extension API specification.
struct NVVertexArrayRange {
    // Public Members
    void *(APIENTRY *wglAllocateMemory)(GLsizei size, GLfloat readfreq,
                                        GLfloat writefreq, GLfloat priority);
    void (APIENTRY *wglFreeMemory)(void *pointer);

    // Construction
    NVVertexArrayRange();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct NVVertexArrayRange

// WGL_NV_video_capture extension API specification.
struct NVVideoCapture {
    // Public Members
    BOOL (APIENTRY *wglBindVideoCaptureDevice)(UINT uVideoSlot,
                                               HVIDEOINPUTDEVICENV hDevice);
    UINT (APIENTRY *wglEnumerateVideoCaptureDevices)(HDC hDc,
                                                     HVIDEOINPUTDEVICENV *phDeviceList);
    BOOL (APIENTRY *wglLockVideoCaptureDevice)(HDC hDc,
                                               HVIDEOINPUTDEVICENV hDevice);
    BOOL (APIENTRY *wglQueryVideoCaptureDevice)(HDC hDc,
                                                HVIDEOINPUTDEVICENV hDevice,
                                                int iAttribute, int *piValue);
    BOOL (APIENTRY *wglReleaseVideoCaptureDevice)(HDC hDc,
                                                  HVIDEOINPUTDEVICENV hDevice);

    // Construction
    NVVideoCapture();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct NVVideoCapture

// WGL_NV_video_output extension API specification.
struct NVVideoOutput {
    // Public Members
    BOOL (APIENTRY *wglBindVideoImage)(HPVIDEODEV hVideoDevice,
                                       HPBUFFERARB hPbuffer, int iVideoBuffer);
    BOOL (APIENTRY *wglGetVideoDevice)(HDC hDC, int numDevices,
                                       HPVIDEODEV *hVideoDevice);
    BOOL (APIENTRY *wglGetVideoInfo)(HPVIDEODEV hpVideoDevice,
                                     unsigned long *pulCounterOutputPbuffer,
                                     unsigned long *pulCounterOutputVideo);
    BOOL (APIENTRY *wglReleaseVideoDevice)(HPVIDEODEV hVideoDevice);
    BOOL (APIENTRY *wglReleaseVideoImage)(HPBUFFERARB hPbuffer, int iVideoBuffer);
    BOOL (APIENTRY *wglSendPbufferToVideo)(HPBUFFERARB hPbuffer, int iBufferType,
                                           unsigned long *pulCounterPbuffer,
                                           BOOL bBlock);

    // Construction
    NVVideoOutput();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct NVVideoOutput

// WGL_OML_sync_control extension API specification.
struct OMLSyncControl {
    // Public Members
    BOOL (APIENTRY *wglGetMscRate)(HDC hdc, INT32 *numerator, INT32 *denominator);
    BOOL (APIENTRY *wglGetSyncValues)(HDC hdc, INT64 *ust, INT64 *msc,
                                      INT64 *sbc);
    INT64 (APIENTRY *wglSwapBuffersMsc)(HDC hdc, INT64 target_msc, INT64 divisor,
                                        INT64 remainder);
    INT64 (APIENTRY *wglSwapLayerBuffersMsc)(HDC hdc, INT fuPlanes,
                                             INT64 target_msc, INT64 divisor,
                                             INT64 remainder);
    BOOL (APIENTRY *wglWaitForMsc)(HDC hdc, INT64 target_msc, INT64 divisor,
                                   INT64 remainder, INT64 *ust, INT64 *msc,
                                   INT64 *sbc);
    BOOL (APIENTRY *wglWaitForSbc)(HDC hdc, INT64 target_sbc, INT64 *ust,
                                   INT64 *msc, INT64 *sbc);

    // Construction
    OMLSyncControl();

    // Operations
    bool isPresent(const gl::APIResolver *resolver) const;
    void resolveEntryPoints(const gl::APIResolver *resolver);
}; // struct OMLSyncControl


} // namespace wgl

#endif // ifndef __0AE3D5F1_596C_46B2_8613_C0DB4AA3727F_INCLUDED__
