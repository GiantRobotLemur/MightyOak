//! @file Ag/GLAPI/WGL_CommandSets.hpp
//! @brief A file generated from the OpenGL API schema definition to create
//! C++ 17 language bindings. In the case of this file, for C++ wrappers of
//! Windows OpenGL (WGL) extensions.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

#ifndef __69715089_2592_4213_BA85_9662D7A21643_INCLUDED__
#define __69715089_2592_4213_BA85_9662D7A21643_INCLUDED__

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include "WGL_API.hpp"

namespace wgl {

////////////////////////////////////////////////////////////////////////////////
// Data Type Declarations
////////////////////////////////////////////////////////////////////////////////
// A base class for a wrapper to a Core API.
class BaseAPI
{
protected:
    // Construction
    BaseAPI();
public:
    virtual ~BaseAPI() = default;

    // Operations
    virtual void resolve(const gl::APIResolver *resolver);

protected:
    // Internal Functions
    void beforeCommand(const char *commandName, const void *entryPoint) const;
    void afterCommand(const char *commandName) const;
private:
    // Internal Fields
    unsigned int (APIENTRY *_getError)();
};

// A base class for a wrapper to an extension API.
class BaseExtension : public BaseAPI
{
protected:
    // Construction
    BaseExtension(const char *extName);

public:
    virtual ~BaseExtension() = default;

    // Accessors
    bool isPresent() const;

    // Operations
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    // Internal Fields
    const char *_extName;
    bool _isPresent;
};

// Manages the entry points for the main API.
class WGLAPI : public BaseAPI
{
public:
    // Construction
    WGLAPI();

    // Accessors
    const WGL &getRawAPI() const;

    // Operations

    // WGL 1.0
    int choosePixelFormat(HDC hDc, const PIXELFORMATDESCRIPTOR *pPfd) const;
    int describePixelFormat(HDC hdc, int ipfd, UINT cjpfd,
                            const PIXELFORMATDESCRIPTOR *ppfd) const;
    UINT getEnhMetaFilePixelFormat(HENHMETAFILE hemf,
                                   const PIXELFORMATDESCRIPTOR *ppfd) const;
    int getPixelFormat(HDC hdc) const;
    BOOL setPixelFormat(HDC hdc, int ipfd, const PIXELFORMATDESCRIPTOR *ppfd) const;
    BOOL swapBuffers(HDC hdc) const;
    BOOL copyContext(HGLRC hglrcSrc, HGLRC hglrcDst, UINT mask) const;
    HGLRC createContext(HDC hDc) const;
    HGLRC createLayerContext(HDC hDc, int level) const;
    BOOL deleteContext(HGLRC oldContext) const;
    BOOL describeLayerPlane(HDC hDc, int pixelFormat, int layerPlane,
                            UINT nBytes, const LAYERPLANEDESCRIPTOR *plpd) const;
    HGLRC getCurrentContext() const;
    HDC getCurrentDC() const;
    int getLayerPaletteEntries(HDC hdc, int iLayerPlane, int iStart,
                               int cEntries, const COLORREF *pcr) const;
    PROC getProcAddress(LPCSTR lpszProc) const;
    BOOL makeCurrent(HDC hDc, HGLRC newContext) const;
    BOOL realizeLayerPalette(HDC hdc, int iLayerPlane, BOOL bRealize) const;
    int setLayerPaletteEntries(HDC hdc, int iLayerPlane, int iStart,
                               int cEntries, const COLORREF *pcr) const;
    BOOL shareLists(HGLRC hrcSrvShare, HGLRC hrcSrvSource) const;
    BOOL swapLayerBuffers(HDC hdc, UINT fuFlags) const;
    BOOL useFontBitmaps(HDC hDC, DWORD first, DWORD count, DWORD listBase) const;
    BOOL useFontBitmapsA(HDC hDC, DWORD first, DWORD count, DWORD listBase) const;
    BOOL useFontBitmapsW(HDC hDC, DWORD first, DWORD count, DWORD listBase) const;
    BOOL useFontOutlines(HDC hDC, DWORD first, DWORD count, DWORD listBase,
                         FLOAT deviation, FLOAT extrusion, int format,
                         LPGLYPHMETRICSFLOAT lpgmf) const;
    BOOL useFontOutlinesA(HDC hDC, DWORD first, DWORD count, DWORD listBase,
                          FLOAT deviation, FLOAT extrusion, int format,
                          LPGLYPHMETRICSFLOAT lpgmf) const;
    BOOL useFontOutlinesW(HDC hDC, DWORD first, DWORD count, DWORD listBase,
                          FLOAT deviation, FLOAT extrusion, int format,
                          LPGLYPHMETRICSFLOAT lpgmf) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    WGL _api;
};

// Manages the entry points for the WGL_3DL_stereo_control extension API.
class _3DLStereoControlAPI : public BaseExtension
{
public:
    // Construction
    _3DLStereoControlAPI();

    // Accessors
    const _3DLStereoControl &getRawAPI() const;

    // Operations
    BOOL setStereoEmitterState3DL(HDC hDC, UINT uState) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    _3DLStereoControl _api;
};

// Manages the entry points for the WGL_AMD_gpu_association extension API.
class AMDGPUAssociationAPI : public BaseExtension
{
public:
    // Construction
    AMDGPUAssociationAPI();

    // Accessors
    const AMDGPUAssociation &getRawAPI() const;

    // Operations
    VOID blitContextFramebuffer(HGLRC dstCtx, GLint srcX0, GLint srcY0,
                                GLint srcX1, GLint srcY1, GLint dstX0,
                                GLint dstY0, GLint dstX1, GLint dstY1,
                                GLbitfield mask, GLenum filter) const;
    HGLRC createAssociateDContext(UINT id) const;
    HGLRC createAssociateDContextAttribs(UINT id, HGLRC hShareContext,
                                         const int *attribList) const;
    BOOL deleteAssociateDContext(HGLRC hglrc) const;
    UINT getContextGpuid(HGLRC hglrc) const;
    HGLRC getCurrentAssociateDContext() const;
    UINT getGpuiDs(UINT maxCount, UINT *ids) const;
    INT getGPUInfo(UINT id, INT property, GLenum dataType, UINT size, void *data) const;
    BOOL makeAssociateDContextCurrent(HGLRC hglrc) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    AMDGPUAssociation _api;
};

// Manages the entry points for the WGL_ARB_buffer_region extension API.
class ARBBufferRegionAPI : public BaseExtension
{
public:
    // Construction
    ARBBufferRegionAPI();

    // Accessors
    const ARBBufferRegion &getRawAPI() const;

    // Operations
    HANDLE createBufferRegion(HDC hDC, int iLayerPlane, UINT uType) const;
    VOID deleteBufferRegion(HANDLE hRegion) const;
    BOOL restoreBufferRegion(HANDLE hRegion, int x, int y, int width, int height,
                             int xSrc, int ySrc) const;
    BOOL saveBufferRegion(HANDLE hRegion, int x, int y, int width, int height) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    ARBBufferRegion _api;
};

// Manages the entry points for the WGL_ARB_create_context extension API.
class ARBCreateContextAPI : public BaseExtension
{
public:
    // Construction
    ARBCreateContextAPI();

    // Accessors
    const ARBCreateContext &getRawAPI() const;

    // Operations
    HGLRC createContextAttribs(HDC hDC, HGLRC hShareContext,
                               const int *attribList) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    ARBCreateContext _api;
};

// Manages the entry points for the WGL_ARB_extensions_string extension API.
class ARBExtensionsStringAPI : public BaseExtension
{
public:
    // Construction
    ARBExtensionsStringAPI();

    // Accessors
    const ARBExtensionsString &getRawAPI() const;

    // Operations
    const char *getExtensionsString(HDC hdc) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    ARBExtensionsString _api;
};

// Manages the entry points for the WGL_ARB_make_current_read extension API.
class ARBMakeCurrentReadAPI : public BaseExtension
{
public:
    // Construction
    ARBMakeCurrentReadAPI();

    // Accessors
    const ARBMakeCurrentRead &getRawAPI() const;

    // Operations
    HDC getCurrentReadDC() const;
    BOOL makeContextCurrent(HDC hDrawDC, HDC hReadDC, HGLRC hglrc) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    ARBMakeCurrentRead _api;
};

// Manages the entry points for the WGL_ARB_pbuffer extension API.
class ARBPbufferAPI : public BaseExtension
{
public:
    // Construction
    ARBPbufferAPI();

    // Accessors
    const ARBPbuffer &getRawAPI() const;

    // Operations
    HPBUFFERARB createPbuffer(HDC hDC, int iPixelFormat, int iWidth, int iHeight,
                              const int *piAttribList) const;
    BOOL destroyPbuffer(HPBUFFERARB hPbuffer) const;
    HDC getPbufferDC(HPBUFFERARB hPbuffer) const;
    BOOL queryPbuffer(HPBUFFERARB hPbuffer, int iAttribute, int *piValue) const;
    int releasePbufferDC(HPBUFFERARB hPbuffer, HDC hDC) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    ARBPbuffer _api;
};

// Manages the entry points for the WGL_ARB_pixel_format extension API.
class ARBPixelFormatAPI : public BaseExtension
{
public:
    // Construction
    ARBPixelFormatAPI();

    // Accessors
    const ARBPixelFormat &getRawAPI() const;

    // Operations
    BOOL choosePixelFormat(HDC hdc, const int *piAttribIList,
                           const FLOAT *pfAttribFList, UINT nMaxFormats,
                           int *piFormats, UINT *nNumFormats) const;
    BOOL getPixelFormatAttribFV(HDC hdc, int iPixelFormat, int iLayerPlane,
                                UINT nAttributes, const int *piAttributes,
                                FLOAT *pfValues) const;
    BOOL getPixelFormatAttribIV(HDC hdc, int iPixelFormat, int iLayerPlane,
                                UINT nAttributes, const int *piAttributes,
                                int *piValues) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    ARBPixelFormat _api;
};

// Manages the entry points for the WGL_ARB_render_texture extension API.
class ARBRenderTextureAPI : public BaseExtension
{
public:
    // Construction
    ARBRenderTextureAPI();

    // Accessors
    const ARBRenderTexture &getRawAPI() const;

    // Operations
    BOOL bindTexImage(HPBUFFERARB hPbuffer, int iBuffer) const;
    BOOL releaseTexImage(HPBUFFERARB hPbuffer, int iBuffer) const;
    BOOL setPbufferAttrib(HPBUFFERARB hPbuffer, const int *piAttribList) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    ARBRenderTexture _api;
};

// Manages the entry points for the WGL_EXT_display_color_table extension API.
class EXTDisplayColorTableAPI : public BaseExtension
{
public:
    // Construction
    EXTDisplayColorTableAPI();

    // Accessors
    const EXTDisplayColorTable &getRawAPI() const;

    // Operations
    GLboolean bindDisplayColorTable(GLushort id) const;
    GLboolean createDisplayColorTable(GLushort id) const;
    VOID destroyDisplayColorTable(GLushort id) const;
    GLboolean loadDisplayColorTable(const GLushort *table, GLuint length) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    EXTDisplayColorTable _api;
};

// Manages the entry points for the WGL_EXT_extensions_string extension API.
class EXTExtensionsStringAPI : public BaseExtension
{
public:
    // Construction
    EXTExtensionsStringAPI();

    // Accessors
    const EXTExtensionsString &getRawAPI() const;

    // Operations
    const char *getExtensionsString() const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    EXTExtensionsString _api;
};

// Manages the entry points for the WGL_EXT_make_current_read extension API.
class EXTMakeCurrentReadAPI : public BaseExtension
{
public:
    // Construction
    EXTMakeCurrentReadAPI();

    // Accessors
    const EXTMakeCurrentRead &getRawAPI() const;

    // Operations
    HDC getCurrentReadDC() const;
    BOOL makeContextCurrent(HDC hDrawDC, HDC hReadDC, HGLRC hglrc) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    EXTMakeCurrentRead _api;
};

// Manages the entry points for the WGL_EXT_pbuffer extension API.
class EXTPbufferAPI : public BaseExtension
{
public:
    // Construction
    EXTPbufferAPI();

    // Accessors
    const EXTPbuffer &getRawAPI() const;

    // Operations
    HPBUFFEREXT createPbuffer(HDC hDC, int iPixelFormat, int iWidth, int iHeight,
                              const int *piAttribList) const;
    BOOL destroyPbuffer(HPBUFFEREXT hPbuffer) const;
    HDC getPbufferDC(HPBUFFEREXT hPbuffer) const;
    BOOL queryPbuffer(HPBUFFEREXT hPbuffer, int iAttribute, int *piValue) const;
    int releasePbufferDC(HPBUFFEREXT hPbuffer, HDC hDC) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    EXTPbuffer _api;
};

// Manages the entry points for the WGL_EXT_pixel_format extension API.
class EXTPixelFormatAPI : public BaseExtension
{
public:
    // Construction
    EXTPixelFormatAPI();

    // Accessors
    const EXTPixelFormat &getRawAPI() const;

    // Operations
    BOOL choosePixelFormat(HDC hdc, const int *piAttribIList,
                           const FLOAT *pfAttribFList, UINT nMaxFormats,
                           int *piFormats, UINT *nNumFormats) const;
    BOOL getPixelFormatAttribFV(HDC hdc, int iPixelFormat, int iLayerPlane,
                                UINT nAttributes, int *piAttributes,
                                FLOAT *pfValues) const;
    BOOL getPixelFormatAttribIV(HDC hdc, int iPixelFormat, int iLayerPlane,
                                UINT nAttributes, int *piAttributes,
                                int *piValues) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    EXTPixelFormat _api;
};

// Manages the entry points for the WGL_EXT_swap_control extension API.
class EXTSwapControlAPI : public BaseExtension
{
public:
    // Construction
    EXTSwapControlAPI();

    // Accessors
    const EXTSwapControl &getRawAPI() const;

    // Operations
    int getSwapInterval() const;
    BOOL swapInterval(int interval) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    EXTSwapControl _api;
};

// Manages the entry points for the WGL_I3D_digital_video_control extension API.
class I3DDigitalVideoControlAPI : public BaseExtension
{
public:
    // Construction
    I3DDigitalVideoControlAPI();

    // Accessors
    const I3DDigitalVideoControl &getRawAPI() const;

    // Operations
    BOOL getDigitalVideoParameters(HDC hDC, int iAttribute, int *piValue) const;
    BOOL setDigitalVideoParameters(HDC hDC, int iAttribute, const int *piValue) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    I3DDigitalVideoControl _api;
};

// Manages the entry points for the WGL_I3D_gamma extension API.
class I3DGammaAPI : public BaseExtension
{
public:
    // Construction
    I3DGammaAPI();

    // Accessors
    const I3DGamma &getRawAPI() const;

    // Operations
    BOOL getGammaTable(HDC hDC, int iEntries, USHORT *puRed, USHORT *puGreen,
                       USHORT *puBlue) const;
    BOOL getGammaTableParameters(HDC hDC, int iAttribute, int *piValue) const;
    BOOL setGammaTable(HDC hDC, int iEntries, const USHORT *puRed,
                       const USHORT *puGreen, const USHORT *puBlue) const;
    BOOL setGammaTableParameters(HDC hDC, int iAttribute, const int *piValue) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    I3DGamma _api;
};

// Manages the entry points for the WGL_I3D_genlock extension API.
class I3DGenlockAPI : public BaseExtension
{
public:
    // Construction
    I3DGenlockAPI();

    // Accessors
    const I3DGenlock &getRawAPI() const;

    // Operations
    BOOL disableGenlock(HDC hDC) const;
    BOOL enableGenlock(HDC hDC) const;
    BOOL genlockSampleRate(HDC hDC, UINT uRate) const;
    BOOL genlockSourceDelay(HDC hDC, UINT uDelay) const;
    BOOL genlockSourceEdge(HDC hDC, UINT uEdge) const;
    BOOL genlockSource(HDC hDC, UINT uSource) const;
    BOOL getGenlockSampleRate(HDC hDC, UINT *uRate) const;
    BOOL getGenlockSourceDelay(HDC hDC, UINT *uDelay) const;
    BOOL getGenlockSourceEdge(HDC hDC, UINT *uEdge) const;
    BOOL getGenlockSource(HDC hDC, UINT *uSource) const;
    BOOL isEnabledGenlock(HDC hDC, BOOL *pFlag) const;
    BOOL queryGenlockMaxSourceDelay(HDC hDC, UINT *uMaxLineDelay,
                                    UINT *uMaxPixelDelay) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    I3DGenlock _api;
};

// Manages the entry points for the WGL_I3D_image_buffer extension API.
class I3DImageBufferAPI : public BaseExtension
{
public:
    // Construction
    I3DImageBufferAPI();

    // Accessors
    const I3DImageBuffer &getRawAPI() const;

    // Operations
    BOOL associateImageBufferEvents(HDC hDC, const HANDLE *pEvent,
                                    const LPVOID *pAddress, const DWORD *pSize,
                                    UINT count) const;
    LPVOID createImageBuffer(HDC hDC, DWORD dwSize, UINT uFlags) const;
    BOOL destroyImageBuffer(HDC hDC, LPVOID pAddress) const;
    BOOL releaseImageBufferEvents(HDC hDC, const LPVOID *pAddress, UINT count) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    I3DImageBuffer _api;
};

// Manages the entry points for the WGL_I3D_swap_frame_lock extension API.
class I3DSwapFrameLockAPI : public BaseExtension
{
public:
    // Construction
    I3DSwapFrameLockAPI();

    // Accessors
    const I3DSwapFrameLock &getRawAPI() const;

    // Operations
    BOOL disableFrameLock() const;
    BOOL enableFrameLock() const;
    BOOL isEnabledFrameLock(BOOL *pFlag) const;
    BOOL queryFrameLockMaster(BOOL *pFlag) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    I3DSwapFrameLock _api;
};

// Manages the entry points for the WGL_I3D_swap_frame_usage extension API.
class I3DSwapFrameUsageAPI : public BaseExtension
{
public:
    // Construction
    I3DSwapFrameUsageAPI();

    // Accessors
    const I3DSwapFrameUsage &getRawAPI() const;

    // Operations
    BOOL beginFrameTracking() const;
    BOOL endFrameTracking() const;
    BOOL getFrameUsage(float *pUsage) const;
    BOOL queryFrameTracking(DWORD *pFrameCount, DWORD *pMissedFrames,
                            float *pLastMissedUsage) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    I3DSwapFrameUsage _api;
};

// Manages the entry points for the WGL_NV_DX_interop extension API.
class NVDxInteropAPI : public BaseExtension
{
public:
    // Construction
    NVDxInteropAPI();

    // Accessors
    const NVDxInterop &getRawAPI() const;

    // Operations
    BOOL dxCloseDevice(HANDLE hDevice) const;
    BOOL dxLockObjects(HANDLE hDevice, GLint count, HANDLE *hObjects) const;
    BOOL dxObjectAccess(HANDLE hObject, GLenum access) const;
    HANDLE dxOpenDevice(void *dxDevice) const;
    HANDLE dxRegisterObject(HANDLE hDevice, void *dxObject, GLuint name,
                            GLenum type, GLenum access) const;
    BOOL dxSetResourceShareHandle(void *dxObject, HANDLE shareHandle) const;
    BOOL dxUnlockObjects(HANDLE hDevice, GLint count, HANDLE *hObjects) const;
    BOOL dxUnregisterObject(HANDLE hDevice, HANDLE hObject) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    NVDxInterop _api;
};

// Manages the entry points for the WGL_NV_copy_image extension API.
class NVCopyImageAPI : public BaseExtension
{
public:
    // Construction
    NVCopyImageAPI();

    // Accessors
    const NVCopyImage &getRawAPI() const;

    // Operations
    BOOL copyImageSubData(HGLRC hSrcRC, GLuint srcName, GLenum srcTarget,
                          GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ,
                          HGLRC hDstRC, GLuint dstName, GLenum dstTarget,
                          GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ,
                          GLsizei width, GLsizei height, GLsizei depth) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    NVCopyImage _api;
};

// Manages the entry points for the WGL_NV_delay_before_swap extension API.
class NVDelayBeforeSwapAPI : public BaseExtension
{
public:
    // Construction
    NVDelayBeforeSwapAPI();

    // Accessors
    const NVDelayBeforeSwap &getRawAPI() const;

    // Operations
    BOOL delayBeforeSwap(HDC hDC, GLfloat seconds) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    NVDelayBeforeSwap _api;
};

// Manages the entry points for the WGL_NV_gpu_affinity extension API.
class NVGPUAffinityAPI : public BaseExtension
{
public:
    // Construction
    NVGPUAffinityAPI();

    // Accessors
    const NVGPUAffinity &getRawAPI() const;

    // Operations
    HDC createAffinityDC(const HGPUNV *phGpuList) const;
    BOOL deleteDC(HDC hdc) const;
    BOOL enumGpuDevices(HGPUNV hGpu, UINT iDeviceIndex, PGPU_DEVICE lpGpuDevice) const;
    BOOL enumGpusFromAffinityDC(HDC hAffinityDC, UINT iGpuIndex, HGPUNV *hGpu) const;
    BOOL enumGpus(UINT iGpuIndex, HGPUNV *phGpu) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    NVGPUAffinity _api;
};

// Manages the entry points for the WGL_NV_present_video extension API.
class NVPresentVideoAPI : public BaseExtension
{
public:
    // Construction
    NVPresentVideoAPI();

    // Accessors
    const NVPresentVideo &getRawAPI() const;

    // Operations
    BOOL bindVideoDevice(HDC hDc, unsigned int uVideoSlot,
                         HVIDEOOUTPUTDEVICENV hVideoDevice,
                         const int *piAttribList) const;
    int enumerateVideoDevices(HDC hDc, HVIDEOOUTPUTDEVICENV *phDeviceList) const;
    BOOL queryCurrentContext(int iAttribute, int *piValue) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    NVPresentVideo _api;
};

// Manages the entry points for the WGL_NV_swap_group extension API.
class NVSwapGroupAPI : public BaseExtension
{
public:
    // Construction
    NVSwapGroupAPI();

    // Accessors
    const NVSwapGroup &getRawAPI() const;

    // Operations
    BOOL bindSwapBarrier(GLuint group, GLuint barrier) const;
    BOOL joinSwapGroup(HDC hDC, GLuint group) const;
    BOOL queryFrameCount(HDC hDC, GLuint *count) const;
    BOOL queryMaxSwapGroups(HDC hDC, GLuint *maxGroups, GLuint *maxBarriers) const;
    BOOL querySwapGroup(HDC hDC, GLuint *group, GLuint *barrier) const;
    BOOL resetFrameCount(HDC hDC) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    NVSwapGroup _api;
};

// Manages the entry points for the WGL_NV_vertex_array_range extension API.
class NVVertexArrayRangeAPI : public BaseExtension
{
public:
    // Construction
    NVVertexArrayRangeAPI();

    // Accessors
    const NVVertexArrayRange &getRawAPI() const;

    // Operations
    void *allocateMemory(GLsizei size, GLfloat readfreq, GLfloat writefreq,
                         GLfloat priority) const;
    void freeMemory(void *pointer) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    NVVertexArrayRange _api;
};

// Manages the entry points for the WGL_NV_video_capture extension API.
class NVVideoCaptureAPI : public BaseExtension
{
public:
    // Construction
    NVVideoCaptureAPI();

    // Accessors
    const NVVideoCapture &getRawAPI() const;

    // Operations
    BOOL bindVideoCaptureDevice(UINT uVideoSlot, HVIDEOINPUTDEVICENV hDevice) const;
    UINT enumerateVideoCaptureDevices(HDC hDc, HVIDEOINPUTDEVICENV *phDeviceList) const;
    BOOL lockVideoCaptureDevice(HDC hDc, HVIDEOINPUTDEVICENV hDevice) const;
    BOOL queryVideoCaptureDevice(HDC hDc, HVIDEOINPUTDEVICENV hDevice,
                                 int iAttribute, int *piValue) const;
    BOOL releaseVideoCaptureDevice(HDC hDc, HVIDEOINPUTDEVICENV hDevice) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    NVVideoCapture _api;
};

// Manages the entry points for the WGL_NV_video_output extension API.
class NVVideoOutputAPI : public BaseExtension
{
public:
    // Construction
    NVVideoOutputAPI();

    // Accessors
    const NVVideoOutput &getRawAPI() const;

    // Operations
    BOOL bindVideoImage(HPVIDEODEV hVideoDevice, HPBUFFERARB hPbuffer,
                        int iVideoBuffer) const;
    BOOL getVideoDevice(HDC hDC, int numDevices, HPVIDEODEV *hVideoDevice) const;
    BOOL getVideoInfo(HPVIDEODEV hpVideoDevice,
                      unsigned long *pulCounterOutputPbuffer,
                      unsigned long *pulCounterOutputVideo) const;
    BOOL releaseVideoDevice(HPVIDEODEV hVideoDevice) const;
    BOOL releaseVideoImage(HPBUFFERARB hPbuffer, int iVideoBuffer) const;
    BOOL sendPbufferToVideo(HPBUFFERARB hPbuffer, int iBufferType,
                            unsigned long *pulCounterPbuffer, BOOL bBlock) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    NVVideoOutput _api;
};

// Manages the entry points for the WGL_OML_sync_control extension API.
class OMLSyncControlAPI : public BaseExtension
{
public:
    // Construction
    OMLSyncControlAPI();

    // Accessors
    const OMLSyncControl &getRawAPI() const;

    // Operations
    BOOL getMscRate(HDC hdc, INT32 *numerator, INT32 *denominator) const;
    BOOL getSyncValues(HDC hdc, INT64 *ust, INT64 *msc, INT64 *sbc) const;
    INT64 swapBuffersMsc(HDC hdc, INT64 target_msc, INT64 divisor,
                         INT64 remainder) const;
    INT64 swapLayerBuffersMsc(HDC hdc, INT fuPlanes, INT64 target_msc,
                              INT64 divisor, INT64 remainder) const;
    BOOL waitForMsc(HDC hdc, INT64 target_msc, INT64 divisor, INT64 remainder,
                    INT64 *ust, INT64 *msc, INT64 *sbc) const;
    BOOL waitForSbc(HDC hdc, INT64 target_sbc, INT64 *ust, INT64 *msc,
                    INT64 *sbc) const;

    // Overrides
    virtual void resolve(const gl::APIResolver *resolver) override;

private:
    OMLSyncControl _api;
};

} // namespace wgl


#endif // ifndef __69715089_2592_4213_BA85_9662D7A21643_INCLUDED__
