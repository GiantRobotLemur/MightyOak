//! @file GLAPI/WGL_APIs.cpp
//! @brief The definition of structures defining the C-language dynamic binding
//! to the Win32-specific implementation of the OpenGL API generated from the
//! API registry.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Required Header Files
////////////////////////////////////////////////////////////////////////////////
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <utility>
#include "Ag/GLAPI/WGL_API.hpp"

namespace wgl {

namespace {

// A value type representing and entry point to resolve
using EntryOffset = std::pair<size_t, const char *>;

// A base class for functors which access API function pointers.
struct PerEntryPoint
{
private:
    void *_basePtr;

protected:
    // Gets a reference to the function pointer described by the entry.
    void *&getEntryPoint(const EntryOffset &entry)
    {
        return *reinterpret_cast<void **>(reinterpret_cast<uint8_t *>(_basePtr) +
                                          entry.first);
    }

public:
    // Constructs an object to access function pointers in a specified structure.
    PerEntryPoint(void *basePtr) :
        _basePtr(basePtr)
    {
    }
};

// A functor which sets function pointers to null.
struct ClearEntryPoint : public PerEntryPoint
{
    ClearEntryPoint(void *basePtr) :
        PerEntryPoint(basePtr)
    {
    }

    void operator()(const EntryOffset &entry)
    {
        getEntryPoint(entry) = nullptr;
    }
};

// A functor which resolves function pointers at runtime.
struct ResolveEntryPoint : public PerEntryPoint
{
private:
    const gl::APIResolver *_resolver;
public:
    ResolveEntryPoint(void *basePtr, const gl::APIResolver *resolver) :
        PerEntryPoint(basePtr),
        _resolver(resolver)
    {
    }

    void operator()(const EntryOffset &entry)
    {
        getEntryPoint(entry) = _resolver->resolveEntryPoint(entry.second);
    }
};

////////////////////////////////////////////////////////////////////////////////
// Local Data
////////////////////////////////////////////////////////////////////////////////
static const EntryOffset __WGLEntryPoints[] = {
    { offsetof(WGL, ChoosePixelFormat), "ChoosePixelFormat" },
    { offsetof(WGL, DescribePixelFormat), "DescribePixelFormat" },
    { offsetof(WGL, GetEnhMetaFilePixelFormat), "GetEnhMetaFilePixelFormat" },
    { offsetof(WGL, GetPixelFormat), "GetPixelFormat" },
    { offsetof(WGL, SetPixelFormat), "SetPixelFormat" },
    { offsetof(WGL, SwapBuffers), "SwapBuffers" },
    { offsetof(WGL, wglCopyContext), "wglCopyContext" },
    { offsetof(WGL, wglCreateContext), "wglCreateContext" },
    { offsetof(WGL, wglCreateLayerContext), "wglCreateLayerContext" },
    { offsetof(WGL, wglDeleteContext), "wglDeleteContext" },
    { offsetof(WGL, wglDescribeLayerPlane), "wglDescribeLayerPlane" },
    { offsetof(WGL, wglGetCurrentContext), "wglGetCurrentContext" },
    { offsetof(WGL, wglGetCurrentDC), "wglGetCurrentDC" },
    { offsetof(WGL, wglGetLayerPaletteEntries), "wglGetLayerPaletteEntries" },
    { offsetof(WGL, wglGetProcAddress), "wglGetProcAddress" },
    { offsetof(WGL, wglMakeCurrent), "wglMakeCurrent" },
    { offsetof(WGL, wglRealizeLayerPalette), "wglRealizeLayerPalette" },
    { offsetof(WGL, wglSetLayerPaletteEntries), "wglSetLayerPaletteEntries" },
    { offsetof(WGL, wglShareLists), "wglShareLists" },
    { offsetof(WGL, wglSwapLayerBuffers), "wglSwapLayerBuffers" },
    { offsetof(WGL, wglUseFontBitmaps), "wglUseFontBitmaps" },
    { offsetof(WGL, wglUseFontBitmapsA), "wglUseFontBitmapsA" },
    { offsetof(WGL, wglUseFontBitmapsW), "wglUseFontBitmapsW" },
    { offsetof(WGL, wglUseFontOutlines), "wglUseFontOutlines" },
    { offsetof(WGL, wglUseFontOutlinesA), "wglUseFontOutlinesA" },
    { offsetof(WGL, wglUseFontOutlinesW), "wglUseFontOutlinesW" },
};

static const size_t __WGLEntryCount = sizeof(__WGLEntryPoints) /
    sizeof(__WGLEntryPoints[0]);

static const EntryOffset ___3DLStereoControlEntryPoints[] = {
    { offsetof(_3DLStereoControl, wglSetStereoEmitterState3DL), "wglSetStereoEmitterState3DL" },
};

static const size_t ___3DLStereoControlEntryCount = sizeof(___3DLStereoControlEntryPoints) /
    sizeof(___3DLStereoControlEntryPoints[0]);

static const EntryOffset __AMDGPUAssociationEntryPoints[] = {
    { offsetof(AMDGPUAssociation, wglBlitContextFramebuffer), "wglBlitContextFramebufferAMD" },
    { offsetof(AMDGPUAssociation, wglCreateAssociatedContext), "wglCreateAssociatedContextAMD" },
    { offsetof(AMDGPUAssociation, wglCreateAssociatedContextAttribs), "wglCreateAssociatedContextAttribsAMD" },
    { offsetof(AMDGPUAssociation, wglDeleteAssociatedContext), "wglDeleteAssociatedContextAMD" },
    { offsetof(AMDGPUAssociation, wglGetContextGPUID), "wglGetContextGPUIDAMD" },
    { offsetof(AMDGPUAssociation, wglGetCurrentAssociatedContext), "wglGetCurrentAssociatedContextAMD" },
    { offsetof(AMDGPUAssociation, wglGetGPUIDs), "wglGetGPUIDsAMD" },
    { offsetof(AMDGPUAssociation, wglGetGPUInfo), "wglGetGPUInfoAMD" },
    { offsetof(AMDGPUAssociation, wglMakeAssociatedContextCurrent), "wglMakeAssociatedContextCurrentAMD" },
};

static const size_t __AMDGPUAssociationEntryCount = sizeof(__AMDGPUAssociationEntryPoints) /
    sizeof(__AMDGPUAssociationEntryPoints[0]);

static const EntryOffset __ARBBufferRegionEntryPoints[] = {
    { offsetof(ARBBufferRegion, wglCreateBufferRegion), "wglCreateBufferRegionARB" },
    { offsetof(ARBBufferRegion, wglDeleteBufferRegion), "wglDeleteBufferRegionARB" },
    { offsetof(ARBBufferRegion, wglRestoreBufferRegion), "wglRestoreBufferRegionARB" },
    { offsetof(ARBBufferRegion, wglSaveBufferRegion), "wglSaveBufferRegionARB" },
};

static const size_t __ARBBufferRegionEntryCount = sizeof(__ARBBufferRegionEntryPoints) /
    sizeof(__ARBBufferRegionEntryPoints[0]);

static const EntryOffset __ARBCreateContextEntryPoints[] = {
    { offsetof(ARBCreateContext, wglCreateContextAttribs), "wglCreateContextAttribsARB" },
};

static const size_t __ARBCreateContextEntryCount = sizeof(__ARBCreateContextEntryPoints) /
    sizeof(__ARBCreateContextEntryPoints[0]);

static const EntryOffset __ARBExtensionsStringEntryPoints[] = {
    { offsetof(ARBExtensionsString, wglGetExtensionsString), "wglGetExtensionsStringARB" },
};

static const size_t __ARBExtensionsStringEntryCount = sizeof(__ARBExtensionsStringEntryPoints) /
    sizeof(__ARBExtensionsStringEntryPoints[0]);

static const EntryOffset __ARBMakeCurrentReadEntryPoints[] = {
    { offsetof(ARBMakeCurrentRead, wglGetCurrentReadDC), "wglGetCurrentReadDCARB" },
    { offsetof(ARBMakeCurrentRead, wglMakeContextCurrent), "wglMakeContextCurrentARB" },
};

static const size_t __ARBMakeCurrentReadEntryCount = sizeof(__ARBMakeCurrentReadEntryPoints) /
    sizeof(__ARBMakeCurrentReadEntryPoints[0]);

static const EntryOffset __ARBPbufferEntryPoints[] = {
    { offsetof(ARBPbuffer, wglCreatePbuffer), "wglCreatePbufferARB" },
    { offsetof(ARBPbuffer, wglDestroyPbuffer), "wglDestroyPbufferARB" },
    { offsetof(ARBPbuffer, wglGetPbufferDC), "wglGetPbufferDCARB" },
    { offsetof(ARBPbuffer, wglQueryPbuffer), "wglQueryPbufferARB" },
    { offsetof(ARBPbuffer, wglReleasePbufferDC), "wglReleasePbufferDCARB" },
};

static const size_t __ARBPbufferEntryCount = sizeof(__ARBPbufferEntryPoints) /
    sizeof(__ARBPbufferEntryPoints[0]);

static const EntryOffset __ARBPixelFormatEntryPoints[] = {
    { offsetof(ARBPixelFormat, wglChoosePixelFormat), "wglChoosePixelFormatARB" },
    { offsetof(ARBPixelFormat, wglGetPixelFormatAttribfv), "wglGetPixelFormatAttribfvARB" },
    { offsetof(ARBPixelFormat, wglGetPixelFormatAttribiv), "wglGetPixelFormatAttribivARB" },
};

static const size_t __ARBPixelFormatEntryCount = sizeof(__ARBPixelFormatEntryPoints) /
    sizeof(__ARBPixelFormatEntryPoints[0]);

static const EntryOffset __ARBRenderTextureEntryPoints[] = {
    { offsetof(ARBRenderTexture, wglBindTexImage), "wglBindTexImageARB" },
    { offsetof(ARBRenderTexture, wglReleaseTexImage), "wglReleaseTexImageARB" },
    { offsetof(ARBRenderTexture, wglSetPbufferAttrib), "wglSetPbufferAttribARB" },
};

static const size_t __ARBRenderTextureEntryCount = sizeof(__ARBRenderTextureEntryPoints) /
    sizeof(__ARBRenderTextureEntryPoints[0]);

static const EntryOffset __EXTDisplayColorTableEntryPoints[] = {
    { offsetof(EXTDisplayColorTable, wglBindDisplayColorTable), "wglBindDisplayColorTableEXT" },
    { offsetof(EXTDisplayColorTable, wglCreateDisplayColorTable), "wglCreateDisplayColorTableEXT" },
    { offsetof(EXTDisplayColorTable, wglDestroyDisplayColorTable), "wglDestroyDisplayColorTableEXT" },
    { offsetof(EXTDisplayColorTable, wglLoadDisplayColorTable), "wglLoadDisplayColorTableEXT" },
};

static const size_t __EXTDisplayColorTableEntryCount = sizeof(__EXTDisplayColorTableEntryPoints) /
    sizeof(__EXTDisplayColorTableEntryPoints[0]);

static const EntryOffset __EXTExtensionsStringEntryPoints[] = {
    { offsetof(EXTExtensionsString, wglGetExtensionsString), "wglGetExtensionsStringEXT" },
};

static const size_t __EXTExtensionsStringEntryCount = sizeof(__EXTExtensionsStringEntryPoints) /
    sizeof(__EXTExtensionsStringEntryPoints[0]);

static const EntryOffset __EXTMakeCurrentReadEntryPoints[] = {
    { offsetof(EXTMakeCurrentRead, wglGetCurrentReadDC), "wglGetCurrentReadDCEXT" },
    { offsetof(EXTMakeCurrentRead, wglMakeContextCurrent), "wglMakeContextCurrentEXT" },
};

static const size_t __EXTMakeCurrentReadEntryCount = sizeof(__EXTMakeCurrentReadEntryPoints) /
    sizeof(__EXTMakeCurrentReadEntryPoints[0]);

static const EntryOffset __EXTPbufferEntryPoints[] = {
    { offsetof(EXTPbuffer, wglCreatePbuffer), "wglCreatePbufferEXT" },
    { offsetof(EXTPbuffer, wglDestroyPbuffer), "wglDestroyPbufferEXT" },
    { offsetof(EXTPbuffer, wglGetPbufferDC), "wglGetPbufferDCEXT" },
    { offsetof(EXTPbuffer, wglQueryPbuffer), "wglQueryPbufferEXT" },
    { offsetof(EXTPbuffer, wglReleasePbufferDC), "wglReleasePbufferDCEXT" },
};

static const size_t __EXTPbufferEntryCount = sizeof(__EXTPbufferEntryPoints) /
    sizeof(__EXTPbufferEntryPoints[0]);

static const EntryOffset __EXTPixelFormatEntryPoints[] = {
    { offsetof(EXTPixelFormat, wglChoosePixelFormat), "wglChoosePixelFormatEXT" },
    { offsetof(EXTPixelFormat, wglGetPixelFormatAttribfv), "wglGetPixelFormatAttribfvEXT" },
    { offsetof(EXTPixelFormat, wglGetPixelFormatAttribiv), "wglGetPixelFormatAttribivEXT" },
};

static const size_t __EXTPixelFormatEntryCount = sizeof(__EXTPixelFormatEntryPoints) /
    sizeof(__EXTPixelFormatEntryPoints[0]);

static const EntryOffset __EXTSwapControlEntryPoints[] = {
    { offsetof(EXTSwapControl, wglGetSwapInterval), "wglGetSwapIntervalEXT" },
    { offsetof(EXTSwapControl, wglSwapInterval), "wglSwapIntervalEXT" },
};

static const size_t __EXTSwapControlEntryCount = sizeof(__EXTSwapControlEntryPoints) /
    sizeof(__EXTSwapControlEntryPoints[0]);

static const EntryOffset __I3DDigitalVideoControlEntryPoints[] = {
    { offsetof(I3DDigitalVideoControl, wglGetDigitalVideoParameters), "wglGetDigitalVideoParametersI3D" },
    { offsetof(I3DDigitalVideoControl, wglSetDigitalVideoParameters), "wglSetDigitalVideoParametersI3D" },
};

static const size_t __I3DDigitalVideoControlEntryCount = sizeof(__I3DDigitalVideoControlEntryPoints) /
    sizeof(__I3DDigitalVideoControlEntryPoints[0]);

static const EntryOffset __I3DGammaEntryPoints[] = {
    { offsetof(I3DGamma, wglGetGammaTable), "wglGetGammaTableI3D" },
    { offsetof(I3DGamma, wglGetGammaTableParameters), "wglGetGammaTableParametersI3D" },
    { offsetof(I3DGamma, wglSetGammaTable), "wglSetGammaTableI3D" },
    { offsetof(I3DGamma, wglSetGammaTableParameters), "wglSetGammaTableParametersI3D" },
};

static const size_t __I3DGammaEntryCount = sizeof(__I3DGammaEntryPoints) /
    sizeof(__I3DGammaEntryPoints[0]);

static const EntryOffset __I3DGenlockEntryPoints[] = {
    { offsetof(I3DGenlock, wglDisableGenlock), "wglDisableGenlockI3D" },
    { offsetof(I3DGenlock, wglEnableGenlock), "wglEnableGenlockI3D" },
    { offsetof(I3DGenlock, wglGenlockSampleRate), "wglGenlockSampleRateI3D" },
    { offsetof(I3DGenlock, wglGenlockSourceDelay), "wglGenlockSourceDelayI3D" },
    { offsetof(I3DGenlock, wglGenlockSourceEdge), "wglGenlockSourceEdgeI3D" },
    { offsetof(I3DGenlock, wglGenlockSource), "wglGenlockSourceI3D" },
    { offsetof(I3DGenlock, wglGetGenlockSampleRate), "wglGetGenlockSampleRateI3D" },
    { offsetof(I3DGenlock, wglGetGenlockSourceDelay), "wglGetGenlockSourceDelayI3D" },
    { offsetof(I3DGenlock, wglGetGenlockSourceEdge), "wglGetGenlockSourceEdgeI3D" },
    { offsetof(I3DGenlock, wglGetGenlockSource), "wglGetGenlockSourceI3D" },
    { offsetof(I3DGenlock, wglIsEnabledGenlock), "wglIsEnabledGenlockI3D" },
    { offsetof(I3DGenlock, wglQueryGenlockMaxSourceDelay), "wglQueryGenlockMaxSourceDelayI3D" },
};

static const size_t __I3DGenlockEntryCount = sizeof(__I3DGenlockEntryPoints) /
    sizeof(__I3DGenlockEntryPoints[0]);

static const EntryOffset __I3DImageBufferEntryPoints[] = {
    { offsetof(I3DImageBuffer, wglAssociateImageBufferEvents), "wglAssociateImageBufferEventsI3D" },
    { offsetof(I3DImageBuffer, wglCreateImageBuffer), "wglCreateImageBufferI3D" },
    { offsetof(I3DImageBuffer, wglDestroyImageBuffer), "wglDestroyImageBufferI3D" },
    { offsetof(I3DImageBuffer, wglReleaseImageBufferEvents), "wglReleaseImageBufferEventsI3D" },
};

static const size_t __I3DImageBufferEntryCount = sizeof(__I3DImageBufferEntryPoints) /
    sizeof(__I3DImageBufferEntryPoints[0]);

static const EntryOffset __I3DSwapFrameLockEntryPoints[] = {
    { offsetof(I3DSwapFrameLock, wglDisableFrameLock), "wglDisableFrameLockI3D" },
    { offsetof(I3DSwapFrameLock, wglEnableFrameLock), "wglEnableFrameLockI3D" },
    { offsetof(I3DSwapFrameLock, wglIsEnabledFrameLock), "wglIsEnabledFrameLockI3D" },
    { offsetof(I3DSwapFrameLock, wglQueryFrameLockMaster), "wglQueryFrameLockMasterI3D" },
};

static const size_t __I3DSwapFrameLockEntryCount = sizeof(__I3DSwapFrameLockEntryPoints) /
    sizeof(__I3DSwapFrameLockEntryPoints[0]);

static const EntryOffset __I3DSwapFrameUsageEntryPoints[] = {
    { offsetof(I3DSwapFrameUsage, wglBeginFrameTracking), "wglBeginFrameTrackingI3D" },
    { offsetof(I3DSwapFrameUsage, wglEndFrameTracking), "wglEndFrameTrackingI3D" },
    { offsetof(I3DSwapFrameUsage, wglGetFrameUsage), "wglGetFrameUsageI3D" },
    { offsetof(I3DSwapFrameUsage, wglQueryFrameTracking), "wglQueryFrameTrackingI3D" },
};

static const size_t __I3DSwapFrameUsageEntryCount = sizeof(__I3DSwapFrameUsageEntryPoints) /
    sizeof(__I3DSwapFrameUsageEntryPoints[0]);

static const EntryOffset __NVDxInteropEntryPoints[] = {
    { offsetof(NVDxInterop, wglDXCloseDevice), "wglDXCloseDeviceNV" },
    { offsetof(NVDxInterop, wglDXLockObjects), "wglDXLockObjectsNV" },
    { offsetof(NVDxInterop, wglDXObjectAccess), "wglDXObjectAccessNV" },
    { offsetof(NVDxInterop, wglDXOpenDevice), "wglDXOpenDeviceNV" },
    { offsetof(NVDxInterop, wglDXRegisterObject), "wglDXRegisterObjectNV" },
    { offsetof(NVDxInterop, wglDXSetResourceShareHandle), "wglDXSetResourceShareHandleNV" },
    { offsetof(NVDxInterop, wglDXUnlockObjects), "wglDXUnlockObjectsNV" },
    { offsetof(NVDxInterop, wglDXUnregisterObject), "wglDXUnregisterObjectNV" },
};

static const size_t __NVDxInteropEntryCount = sizeof(__NVDxInteropEntryPoints) /
    sizeof(__NVDxInteropEntryPoints[0]);

static const EntryOffset __NVCopyImageEntryPoints[] = {
    { offsetof(NVCopyImage, wglCopyImageSubData), "wglCopyImageSubDataNV" },
};

static const size_t __NVCopyImageEntryCount = sizeof(__NVCopyImageEntryPoints) /
    sizeof(__NVCopyImageEntryPoints[0]);

static const EntryOffset __NVDelayBeforeSwapEntryPoints[] = {
    { offsetof(NVDelayBeforeSwap, wglDelayBeforeSwap), "wglDelayBeforeSwapNV" },
};

static const size_t __NVDelayBeforeSwapEntryCount = sizeof(__NVDelayBeforeSwapEntryPoints) /
    sizeof(__NVDelayBeforeSwapEntryPoints[0]);

static const EntryOffset __NVGPUAffinityEntryPoints[] = {
    { offsetof(NVGPUAffinity, wglCreateAffinityDC), "wglCreateAffinityDCNV" },
    { offsetof(NVGPUAffinity, wglDeleteDC), "wglDeleteDCNV" },
    { offsetof(NVGPUAffinity, wglEnumGpuDevices), "wglEnumGpuDevicesNV" },
    { offsetof(NVGPUAffinity, wglEnumGpusFromAffinityDC), "wglEnumGpusFromAffinityDCNV" },
    { offsetof(NVGPUAffinity, wglEnumGpus), "wglEnumGpusNV" },
};

static const size_t __NVGPUAffinityEntryCount = sizeof(__NVGPUAffinityEntryPoints) /
    sizeof(__NVGPUAffinityEntryPoints[0]);

static const EntryOffset __NVPresentVideoEntryPoints[] = {
    { offsetof(NVPresentVideo, wglBindVideoDevice), "wglBindVideoDeviceNV" },
    { offsetof(NVPresentVideo, wglEnumerateVideoDevices), "wglEnumerateVideoDevicesNV" },
    { offsetof(NVPresentVideo, wglQueryCurrentContext), "wglQueryCurrentContextNV" },
};

static const size_t __NVPresentVideoEntryCount = sizeof(__NVPresentVideoEntryPoints) /
    sizeof(__NVPresentVideoEntryPoints[0]);

static const EntryOffset __NVSwapGroupEntryPoints[] = {
    { offsetof(NVSwapGroup, wglBindSwapBarrier), "wglBindSwapBarrierNV" },
    { offsetof(NVSwapGroup, wglJoinSwapGroup), "wglJoinSwapGroupNV" },
    { offsetof(NVSwapGroup, wglQueryFrameCount), "wglQueryFrameCountNV" },
    { offsetof(NVSwapGroup, wglQueryMaxSwapGroups), "wglQueryMaxSwapGroupsNV" },
    { offsetof(NVSwapGroup, wglQuerySwapGroup), "wglQuerySwapGroupNV" },
    { offsetof(NVSwapGroup, wglResetFrameCount), "wglResetFrameCountNV" },
};

static const size_t __NVSwapGroupEntryCount = sizeof(__NVSwapGroupEntryPoints) /
    sizeof(__NVSwapGroupEntryPoints[0]);

static const EntryOffset __NVVertexArrayRangeEntryPoints[] = {
    { offsetof(NVVertexArrayRange, wglAllocateMemory), "wglAllocateMemoryNV" },
    { offsetof(NVVertexArrayRange, wglFreeMemory), "wglFreeMemoryNV" },
};

static const size_t __NVVertexArrayRangeEntryCount = sizeof(__NVVertexArrayRangeEntryPoints) /
    sizeof(__NVVertexArrayRangeEntryPoints[0]);

static const EntryOffset __NVVideoCaptureEntryPoints[] = {
    { offsetof(NVVideoCapture, wglBindVideoCaptureDevice), "wglBindVideoCaptureDeviceNV" },
    { offsetof(NVVideoCapture, wglEnumerateVideoCaptureDevices), "wglEnumerateVideoCaptureDevicesNV" },
    { offsetof(NVVideoCapture, wglLockVideoCaptureDevice), "wglLockVideoCaptureDeviceNV" },
    { offsetof(NVVideoCapture, wglQueryVideoCaptureDevice), "wglQueryVideoCaptureDeviceNV" },
    { offsetof(NVVideoCapture, wglReleaseVideoCaptureDevice), "wglReleaseVideoCaptureDeviceNV" },
};

static const size_t __NVVideoCaptureEntryCount = sizeof(__NVVideoCaptureEntryPoints) /
    sizeof(__NVVideoCaptureEntryPoints[0]);

static const EntryOffset __NVVideoOutputEntryPoints[] = {
    { offsetof(NVVideoOutput, wglBindVideoImage), "wglBindVideoImageNV" },
    { offsetof(NVVideoOutput, wglGetVideoDevice), "wglGetVideoDeviceNV" },
    { offsetof(NVVideoOutput, wglGetVideoInfo), "wglGetVideoInfoNV" },
    { offsetof(NVVideoOutput, wglReleaseVideoDevice), "wglReleaseVideoDeviceNV" },
    { offsetof(NVVideoOutput, wglReleaseVideoImage), "wglReleaseVideoImageNV" },
    { offsetof(NVVideoOutput, wglSendPbufferToVideo), "wglSendPbufferToVideoNV" },
};

static const size_t __NVVideoOutputEntryCount = sizeof(__NVVideoOutputEntryPoints) /
    sizeof(__NVVideoOutputEntryPoints[0]);

static const EntryOffset __OMLSyncControlEntryPoints[] = {
    { offsetof(OMLSyncControl, wglGetMscRate), "wglGetMscRateOML" },
    { offsetof(OMLSyncControl, wglGetSyncValues), "wglGetSyncValuesOML" },
    { offsetof(OMLSyncControl, wglSwapBuffersMsc), "wglSwapBuffersMscOML" },
    { offsetof(OMLSyncControl, wglSwapLayerBuffersMsc), "wglSwapLayerBuffersMscOML" },
    { offsetof(OMLSyncControl, wglWaitForMsc), "wglWaitForMscOML" },
    { offsetof(OMLSyncControl, wglWaitForSbc), "wglWaitForSbcOML" },
};

static const size_t __OMLSyncControlEntryCount = sizeof(__OMLSyncControlEntryPoints) /
    sizeof(__OMLSyncControlEntryPoints[0]);

} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// WGL Member Definitions
////////////////////////////////////////////////////////////////////////////////
WGL::WGL()
{
    std::for_each_n(__WGLEntryPoints, __WGLEntryCount,
                    ClearEntryPoint(this));
}

void WGL::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__WGLEntryPoints, __WGLEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// _3DLStereoControl Member Definitions
////////////////////////////////////////////////////////////////////////////////
_3DLStereoControl::_3DLStereoControl()
{
    std::for_each_n(___3DLStereoControlEntryPoints, ___3DLStereoControlEntryCount,
                    ClearEntryPoint(this));
}

bool _3DLStereoControl::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_3DL_stereo_control");
}

void _3DLStereoControl::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(___3DLStereoControlEntryPoints, ___3DLStereoControlEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// AMDGPUAssociation Member Definitions
////////////////////////////////////////////////////////////////////////////////
AMDGPUAssociation::AMDGPUAssociation()
{
    std::for_each_n(__AMDGPUAssociationEntryPoints, __AMDGPUAssociationEntryCount,
                    ClearEntryPoint(this));
}

bool AMDGPUAssociation::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_AMD_gpu_association");
}

void AMDGPUAssociation::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__AMDGPUAssociationEntryPoints, __AMDGPUAssociationEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBBufferRegion Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBBufferRegion::ARBBufferRegion()
{
    std::for_each_n(__ARBBufferRegionEntryPoints, __ARBBufferRegionEntryCount,
                    ClearEntryPoint(this));
}

bool ARBBufferRegion::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_ARB_buffer_region");
}

void ARBBufferRegion::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__ARBBufferRegionEntryPoints, __ARBBufferRegionEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBCreateContext Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBCreateContext::ARBCreateContext()
{
    std::for_each_n(__ARBCreateContextEntryPoints, __ARBCreateContextEntryCount,
                    ClearEntryPoint(this));
}

bool ARBCreateContext::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_ARB_create_context");
}

void ARBCreateContext::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__ARBCreateContextEntryPoints, __ARBCreateContextEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBExtensionsString Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBExtensionsString::ARBExtensionsString()
{
    std::for_each_n(__ARBExtensionsStringEntryPoints, __ARBExtensionsStringEntryCount,
                    ClearEntryPoint(this));
}

bool ARBExtensionsString::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_ARB_extensions_string");
}

void ARBExtensionsString::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__ARBExtensionsStringEntryPoints, __ARBExtensionsStringEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBMakeCurrentRead Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBMakeCurrentRead::ARBMakeCurrentRead()
{
    std::for_each_n(__ARBMakeCurrentReadEntryPoints, __ARBMakeCurrentReadEntryCount,
                    ClearEntryPoint(this));
}

bool ARBMakeCurrentRead::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_ARB_make_current_read");
}

void ARBMakeCurrentRead::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__ARBMakeCurrentReadEntryPoints, __ARBMakeCurrentReadEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBPbuffer Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBPbuffer::ARBPbuffer()
{
    std::for_each_n(__ARBPbufferEntryPoints, __ARBPbufferEntryCount,
                    ClearEntryPoint(this));
}

bool ARBPbuffer::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_ARB_pbuffer");
}

void ARBPbuffer::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__ARBPbufferEntryPoints, __ARBPbufferEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBPixelFormat Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBPixelFormat::ARBPixelFormat()
{
    std::for_each_n(__ARBPixelFormatEntryPoints, __ARBPixelFormatEntryCount,
                    ClearEntryPoint(this));
}

bool ARBPixelFormat::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_ARB_pixel_format");
}

void ARBPixelFormat::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__ARBPixelFormatEntryPoints, __ARBPixelFormatEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// ARBRenderTexture Member Definitions
////////////////////////////////////////////////////////////////////////////////
ARBRenderTexture::ARBRenderTexture()
{
    std::for_each_n(__ARBRenderTextureEntryPoints, __ARBRenderTextureEntryCount,
                    ClearEntryPoint(this));
}

bool ARBRenderTexture::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_ARB_render_texture");
}

void ARBRenderTexture::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__ARBRenderTextureEntryPoints, __ARBRenderTextureEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTDisplayColorTable Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTDisplayColorTable::EXTDisplayColorTable()
{
    std::for_each_n(__EXTDisplayColorTableEntryPoints, __EXTDisplayColorTableEntryCount,
                    ClearEntryPoint(this));
}

bool EXTDisplayColorTable::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_EXT_display_color_table");
}

void EXTDisplayColorTable::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__EXTDisplayColorTableEntryPoints, __EXTDisplayColorTableEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTExtensionsString Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTExtensionsString::EXTExtensionsString()
{
    std::for_each_n(__EXTExtensionsStringEntryPoints, __EXTExtensionsStringEntryCount,
                    ClearEntryPoint(this));
}

bool EXTExtensionsString::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_EXT_extensions_string");
}

void EXTExtensionsString::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__EXTExtensionsStringEntryPoints, __EXTExtensionsStringEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTMakeCurrentRead Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTMakeCurrentRead::EXTMakeCurrentRead()
{
    std::for_each_n(__EXTMakeCurrentReadEntryPoints, __EXTMakeCurrentReadEntryCount,
                    ClearEntryPoint(this));
}

bool EXTMakeCurrentRead::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_EXT_make_current_read");
}

void EXTMakeCurrentRead::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__EXTMakeCurrentReadEntryPoints, __EXTMakeCurrentReadEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTPbuffer Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTPbuffer::EXTPbuffer()
{
    std::for_each_n(__EXTPbufferEntryPoints, __EXTPbufferEntryCount,
                    ClearEntryPoint(this));
}

bool EXTPbuffer::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_EXT_pbuffer");
}

void EXTPbuffer::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__EXTPbufferEntryPoints, __EXTPbufferEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTPixelFormat Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTPixelFormat::EXTPixelFormat()
{
    std::for_each_n(__EXTPixelFormatEntryPoints, __EXTPixelFormatEntryCount,
                    ClearEntryPoint(this));
}

bool EXTPixelFormat::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_EXT_pixel_format");
}

void EXTPixelFormat::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__EXTPixelFormatEntryPoints, __EXTPixelFormatEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// EXTSwapControl Member Definitions
////////////////////////////////////////////////////////////////////////////////
EXTSwapControl::EXTSwapControl()
{
    std::for_each_n(__EXTSwapControlEntryPoints, __EXTSwapControlEntryCount,
                    ClearEntryPoint(this));
}

bool EXTSwapControl::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_EXT_swap_control");
}

void EXTSwapControl::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__EXTSwapControlEntryPoints, __EXTSwapControlEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// I3DDigitalVideoControl Member Definitions
////////////////////////////////////////////////////////////////////////////////
I3DDigitalVideoControl::I3DDigitalVideoControl()
{
    std::for_each_n(__I3DDigitalVideoControlEntryPoints, __I3DDigitalVideoControlEntryCount,
                    ClearEntryPoint(this));
}

bool I3DDigitalVideoControl::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_I3D_digital_video_control");
}

void I3DDigitalVideoControl::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__I3DDigitalVideoControlEntryPoints, __I3DDigitalVideoControlEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// I3DGamma Member Definitions
////////////////////////////////////////////////////////////////////////////////
I3DGamma::I3DGamma()
{
    std::for_each_n(__I3DGammaEntryPoints, __I3DGammaEntryCount,
                    ClearEntryPoint(this));
}

bool I3DGamma::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_I3D_gamma");
}

void I3DGamma::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__I3DGammaEntryPoints, __I3DGammaEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// I3DGenlock Member Definitions
////////////////////////////////////////////////////////////////////////////////
I3DGenlock::I3DGenlock()
{
    std::for_each_n(__I3DGenlockEntryPoints, __I3DGenlockEntryCount,
                    ClearEntryPoint(this));
}

bool I3DGenlock::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_I3D_genlock");
}

void I3DGenlock::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__I3DGenlockEntryPoints, __I3DGenlockEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// I3DImageBuffer Member Definitions
////////////////////////////////////////////////////////////////////////////////
I3DImageBuffer::I3DImageBuffer()
{
    std::for_each_n(__I3DImageBufferEntryPoints, __I3DImageBufferEntryCount,
                    ClearEntryPoint(this));
}

bool I3DImageBuffer::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_I3D_image_buffer");
}

void I3DImageBuffer::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__I3DImageBufferEntryPoints, __I3DImageBufferEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// I3DSwapFrameLock Member Definitions
////////////////////////////////////////////////////////////////////////////////
I3DSwapFrameLock::I3DSwapFrameLock()
{
    std::for_each_n(__I3DSwapFrameLockEntryPoints, __I3DSwapFrameLockEntryCount,
                    ClearEntryPoint(this));
}

bool I3DSwapFrameLock::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_I3D_swap_frame_lock");
}

void I3DSwapFrameLock::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__I3DSwapFrameLockEntryPoints, __I3DSwapFrameLockEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// I3DSwapFrameUsage Member Definitions
////////////////////////////////////////////////////////////////////////////////
I3DSwapFrameUsage::I3DSwapFrameUsage()
{
    std::for_each_n(__I3DSwapFrameUsageEntryPoints, __I3DSwapFrameUsageEntryCount,
                    ClearEntryPoint(this));
}

bool I3DSwapFrameUsage::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_I3D_swap_frame_usage");
}

void I3DSwapFrameUsage::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__I3DSwapFrameUsageEntryPoints, __I3DSwapFrameUsageEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVDxInterop Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVDxInterop::NVDxInterop()
{
    std::for_each_n(__NVDxInteropEntryPoints, __NVDxInteropEntryCount,
                    ClearEntryPoint(this));
}

bool NVDxInterop::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_NV_DX_interop");
}

void NVDxInterop::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__NVDxInteropEntryPoints, __NVDxInteropEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVCopyImage Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVCopyImage::NVCopyImage()
{
    std::for_each_n(__NVCopyImageEntryPoints, __NVCopyImageEntryCount,
                    ClearEntryPoint(this));
}

bool NVCopyImage::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_NV_copy_image");
}

void NVCopyImage::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__NVCopyImageEntryPoints, __NVCopyImageEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVDelayBeforeSwap Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVDelayBeforeSwap::NVDelayBeforeSwap()
{
    std::for_each_n(__NVDelayBeforeSwapEntryPoints, __NVDelayBeforeSwapEntryCount,
                    ClearEntryPoint(this));
}

bool NVDelayBeforeSwap::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_NV_delay_before_swap");
}

void NVDelayBeforeSwap::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__NVDelayBeforeSwapEntryPoints, __NVDelayBeforeSwapEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVGPUAffinity Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVGPUAffinity::NVGPUAffinity()
{
    std::for_each_n(__NVGPUAffinityEntryPoints, __NVGPUAffinityEntryCount,
                    ClearEntryPoint(this));
}

bool NVGPUAffinity::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_NV_gpu_affinity");
}

void NVGPUAffinity::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__NVGPUAffinityEntryPoints, __NVGPUAffinityEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVPresentVideo Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVPresentVideo::NVPresentVideo()
{
    std::for_each_n(__NVPresentVideoEntryPoints, __NVPresentVideoEntryCount,
                    ClearEntryPoint(this));
}

bool NVPresentVideo::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_NV_present_video");
}

void NVPresentVideo::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__NVPresentVideoEntryPoints, __NVPresentVideoEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVSwapGroup Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVSwapGroup::NVSwapGroup()
{
    std::for_each_n(__NVSwapGroupEntryPoints, __NVSwapGroupEntryCount,
                    ClearEntryPoint(this));
}

bool NVSwapGroup::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_NV_swap_group");
}

void NVSwapGroup::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__NVSwapGroupEntryPoints, __NVSwapGroupEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVVertexArrayRange Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVVertexArrayRange::NVVertexArrayRange()
{
    std::for_each_n(__NVVertexArrayRangeEntryPoints, __NVVertexArrayRangeEntryCount,
                    ClearEntryPoint(this));
}

bool NVVertexArrayRange::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_NV_vertex_array_range");
}

void NVVertexArrayRange::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__NVVertexArrayRangeEntryPoints, __NVVertexArrayRangeEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVVideoCapture Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVVideoCapture::NVVideoCapture()
{
    std::for_each_n(__NVVideoCaptureEntryPoints, __NVVideoCaptureEntryCount,
                    ClearEntryPoint(this));
}

bool NVVideoCapture::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_NV_video_capture");
}

void NVVideoCapture::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__NVVideoCaptureEntryPoints, __NVVideoCaptureEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// NVVideoOutput Member Definitions
////////////////////////////////////////////////////////////////////////////////
NVVideoOutput::NVVideoOutput()
{
    std::for_each_n(__NVVideoOutputEntryPoints, __NVVideoOutputEntryCount,
                    ClearEntryPoint(this));
}

bool NVVideoOutput::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_NV_video_output");
}

void NVVideoOutput::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__NVVideoOutputEntryPoints, __NVVideoOutputEntryCount,
                    ResolveEntryPoint(this, resolver));
}

////////////////////////////////////////////////////////////////////////////////
// OMLSyncControl Member Definitions
////////////////////////////////////////////////////////////////////////////////
OMLSyncControl::OMLSyncControl()
{
    std::for_each_n(__OMLSyncControlEntryPoints, __OMLSyncControlEntryCount,
                    ClearEntryPoint(this));
}

bool OMLSyncControl::isPresent(const gl::APIResolver *resolver) const
{
    return resolver->isExtensionPresent("WGL_OML_sync_control");
}

void OMLSyncControl::resolveEntryPoints(const gl::APIResolver *resolver)
{
    std::for_each_n(__OMLSyncControlEntryPoints, __OMLSyncControlEntryCount,
                    ResolveEntryPoint(this, resolver));
}

} // namespace wgl
