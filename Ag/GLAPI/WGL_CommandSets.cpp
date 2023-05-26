//! @file GLAPI/WGL_CommandSets.cpp
//! @brief The definition of structures defining the C++ language bindings
//! to the Win32-specific implementation of the OpenGL API generated from the
//! API registry.
//! @author GiantRobotLemur@na-se.co.uk
//! @date 2022-2023
//! @copyright This file is part of the Mighty Oak project which is released
//! under LGPL 3 license. See LICENSE file at the repository root or go to
//! https://github.com/GiantRobotLemur/MightyOak for full license details.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Dependent Header Files
////////////////////////////////////////////////////////////////////////////////
#include <stdexcept>
#include <string>
#include <type_traits>
#include "Ag/GLAPI/WGL_CommandSets.hpp"

namespace wgl {

namespace {
////////////////////////////////////////////////////////////////////////////////
// Local Functions
////////////////////////////////////////////////////////////////////////////////
// Convert an enum class type to original type.
template<typename T>
typename std::underlying_type<T>::type toScalar(T value)
{
    return static_cast<std::underlying_type<T>::type>(value);
}

// Converts a pointer to an enum class to a pointer to the original type.
template<typename T>
typename std::underlying_type<T>::type *toScalarPtr(T *value)
{
    return reinterpret_cast<std::underlying_type<T>::type *>(value);
}

// Converts a read-only pointer to an enum class to a pointer to the original type.
template<typename T>
typename const std::underlying_type<T>::type *toScalarPtr(const T *value)
{
    return reinterpret_cast<const std::underlying_type<T>::type *>(value);
}

} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////
// BaseAPI Member Functions
////////////////////////////////////////////////////////////////////////////////
BaseAPI::BaseAPI() :
    _getError(nullptr)
{
}

void BaseAPI::beforeCommand(const char *commandName, const void *entryPoint) const
{
    if (entryPoint == nullptr)
    {
        // TODO: Throw NotSupportedException.
        std::string message;

        message.assign("The command '");
        message.append(commandName);
        message.append("()' is not supported.");
        throw std::runtime_error(message);
    }
}

void BaseAPI::afterCommand(const char *commandName) const
{
    auto errorCode = (_getError == nullptr) ? 0 : _getError();

    if (errorCode != 0)
    {
        // Flush additional errors.
        // TODO: Gather all error codes in order to report them.
        while (_getError() != 0) { }

        // TODO: Throw SubsystemErrorException.
        std::string message;

        message.assign("The command '");
        message.append(commandName);
        message.append("()' failed.");
        throw std::runtime_error(message);
    }
}

void BaseAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _getError = resolver->getErrorReporterProc();
    }
}

////////////////////////////////////////////////////////////////////////////////
// BaseExtension Member Functions
////////////////////////////////////////////////////////////////////////////////
BaseExtension::BaseExtension(const char *extName) :
    _extName(extName),
    _isPresent(false)
{
}

bool BaseExtension::isPresent() const
{
    return _isPresent;
}

void BaseExtension::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _isPresent = resolver->isExtensionPresent(_extName);

        // Call the base class.
        BaseAPI::resolve(resolver);
    }
}

////////////////////////////////////////////////////////////////////////////////
// WGLAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Constructs an unresolved command set for the core API.
WGLAPI::WGLAPI()
{
}

// Grant read-only access to the underlying function pointers.
const WGL &WGLAPI::getRawAPI() const
{
    return _api;
}

// Resolve core API entry points.
void WGLAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseAPI::resolve(resolver);
    }
}

// Calls ChoosePixelFormat().
int WGLAPI::choosePixelFormat(HDC hDc, const PIXELFORMATDESCRIPTOR *pPfd) const
{
    static const char *fnName = "ChoosePixelFormat";

    beforeCommand(fnName, _api.ChoosePixelFormat);
    int result = _api.ChoosePixelFormat(hDc, pPfd);
    afterCommand(fnName);

    return result;
}

// Calls DescribePixelFormat().
int WGLAPI::describePixelFormat(HDC hdc, int ipfd, UINT cjpfd,
                                const PIXELFORMATDESCRIPTOR *ppfd) const
{
    static const char *fnName = "DescribePixelFormat";

    beforeCommand(fnName, _api.DescribePixelFormat);
    int result = _api.DescribePixelFormat(hdc, ipfd, cjpfd, ppfd);
    afterCommand(fnName);

    return result;
}

// Calls GetEnhMetaFilePixelFormat().
UINT WGLAPI::getEnhMetaFilePixelFormat(HENHMETAFILE hemf,
                                       const PIXELFORMATDESCRIPTOR *ppfd) const
{
    static const char *fnName = "GetEnhMetaFilePixelFormat";

    beforeCommand(fnName, _api.GetEnhMetaFilePixelFormat);
    UINT result = _api.GetEnhMetaFilePixelFormat(hemf, ppfd);
    afterCommand(fnName);

    return result;
}

// Calls GetPixelFormat().
int WGLAPI::getPixelFormat(HDC hdc) const
{
    static const char *fnName = "GetPixelFormat";

    beforeCommand(fnName, _api.GetPixelFormat);
    int result = _api.GetPixelFormat(hdc);
    afterCommand(fnName);

    return result;
}

// Calls SetPixelFormat().
BOOL WGLAPI::setPixelFormat(HDC hdc, int ipfd, const PIXELFORMATDESCRIPTOR *ppfd) const
{
    static const char *fnName = "SetPixelFormat";

    beforeCommand(fnName, _api.SetPixelFormat);
    BOOL result = _api.SetPixelFormat(hdc, ipfd, ppfd);
    afterCommand(fnName);

    return result;
}

// Calls SwapBuffers().
BOOL WGLAPI::swapBuffers(HDC hdc) const
{
    static const char *fnName = "SwapBuffers";

    beforeCommand(fnName, _api.SwapBuffers);
    BOOL result = _api.SwapBuffers(hdc);
    afterCommand(fnName);

    return result;
}

// Calls wglCopyContext().
BOOL WGLAPI::copyContext(HGLRC hglrcSrc, HGLRC hglrcDst, UINT mask) const
{
    static const char *fnName = "wglCopyContext";

    beforeCommand(fnName, _api.wglCopyContext);
    BOOL result = _api.wglCopyContext(hglrcSrc, hglrcDst, mask);
    afterCommand(fnName);

    return result;
}

// Calls wglCreateContext().
HGLRC WGLAPI::createContext(HDC hDc) const
{
    static const char *fnName = "wglCreateContext";

    beforeCommand(fnName, _api.wglCreateContext);
    HGLRC result = _api.wglCreateContext(hDc);
    afterCommand(fnName);

    return result;
}

// Calls wglCreateLayerContext().
HGLRC WGLAPI::createLayerContext(HDC hDc, int level) const
{
    static const char *fnName = "wglCreateLayerContext";

    beforeCommand(fnName, _api.wglCreateLayerContext);
    HGLRC result = _api.wglCreateLayerContext(hDc, level);
    afterCommand(fnName);

    return result;
}

// Calls wglDeleteContext().
BOOL WGLAPI::deleteContext(HGLRC oldContext) const
{
    static const char *fnName = "wglDeleteContext";

    beforeCommand(fnName, _api.wglDeleteContext);
    BOOL result = _api.wglDeleteContext(oldContext);
    afterCommand(fnName);

    return result;
}

// Calls wglDescribeLayerPlane().
BOOL WGLAPI::describeLayerPlane(HDC hDc, int pixelFormat, int layerPlane,
                                UINT nBytes, const LAYERPLANEDESCRIPTOR *plpd) const
{
    static const char *fnName = "wglDescribeLayerPlane";

    beforeCommand(fnName, _api.wglDescribeLayerPlane);
    BOOL result = _api.wglDescribeLayerPlane(hDc, pixelFormat, layerPlane,
                                             nBytes, plpd);
    afterCommand(fnName);

    return result;
}

// Calls wglGetCurrentContext().
HGLRC WGLAPI::getCurrentContext() const
{
    static const char *fnName = "wglGetCurrentContext";

    beforeCommand(fnName, _api.wglGetCurrentContext);
    HGLRC result = _api.wglGetCurrentContext();
    afterCommand(fnName);

    return result;
}

// Calls wglGetCurrentDC().
HDC WGLAPI::getCurrentDC() const
{
    static const char *fnName = "wglGetCurrentDC";

    beforeCommand(fnName, _api.wglGetCurrentDC);
    HDC result = _api.wglGetCurrentDC();
    afterCommand(fnName);

    return result;
}

// Calls wglGetLayerPaletteEntries().
int WGLAPI::getLayerPaletteEntries(HDC hdc, int iLayerPlane, int iStart,
                                   int cEntries, const COLORREF *pcr) const
{
    static const char *fnName = "wglGetLayerPaletteEntries";

    beforeCommand(fnName, _api.wglGetLayerPaletteEntries);
    int result = _api.wglGetLayerPaletteEntries(hdc, iLayerPlane, iStart,
                                                cEntries, pcr);
    afterCommand(fnName);

    return result;
}

// Calls wglGetProcAddress().
PROC WGLAPI::getProcAddress(LPCSTR lpszProc) const
{
    static const char *fnName = "wglGetProcAddress";

    beforeCommand(fnName, _api.wglGetProcAddress);
    PROC result = _api.wglGetProcAddress(lpszProc);
    afterCommand(fnName);

    return result;
}

// Calls wglMakeCurrent().
BOOL WGLAPI::makeCurrent(HDC hDc, HGLRC newContext) const
{
    static const char *fnName = "wglMakeCurrent";

    beforeCommand(fnName, _api.wglMakeCurrent);
    BOOL result = _api.wglMakeCurrent(hDc, newContext);
    afterCommand(fnName);

    return result;
}

// Calls wglRealizeLayerPalette().
BOOL WGLAPI::realizeLayerPalette(HDC hdc, int iLayerPlane, BOOL bRealize) const
{
    static const char *fnName = "wglRealizeLayerPalette";

    beforeCommand(fnName, _api.wglRealizeLayerPalette);
    BOOL result = _api.wglRealizeLayerPalette(hdc, iLayerPlane, bRealize);
    afterCommand(fnName);

    return result;
}

// Calls wglSetLayerPaletteEntries().
int WGLAPI::setLayerPaletteEntries(HDC hdc, int iLayerPlane, int iStart,
                                   int cEntries, const COLORREF *pcr) const
{
    static const char *fnName = "wglSetLayerPaletteEntries";

    beforeCommand(fnName, _api.wglSetLayerPaletteEntries);
    int result = _api.wglSetLayerPaletteEntries(hdc, iLayerPlane, iStart,
                                                cEntries, pcr);
    afterCommand(fnName);

    return result;
}

// Calls wglShareLists().
BOOL WGLAPI::shareLists(HGLRC hrcSrvShare, HGLRC hrcSrvSource) const
{
    static const char *fnName = "wglShareLists";

    beforeCommand(fnName, _api.wglShareLists);
    BOOL result = _api.wglShareLists(hrcSrvShare, hrcSrvSource);
    afterCommand(fnName);

    return result;
}

// Calls wglSwapLayerBuffers().
BOOL WGLAPI::swapLayerBuffers(HDC hdc, UINT fuFlags) const
{
    static const char *fnName = "wglSwapLayerBuffers";

    beforeCommand(fnName, _api.wglSwapLayerBuffers);
    BOOL result = _api.wglSwapLayerBuffers(hdc, fuFlags);
    afterCommand(fnName);

    return result;
}

// Calls wglUseFontBitmaps().
BOOL WGLAPI::useFontBitmaps(HDC hDC, DWORD first, DWORD count, DWORD listBase) const
{
    static const char *fnName = "wglUseFontBitmaps";

    beforeCommand(fnName, _api.wglUseFontBitmaps);
    BOOL result = _api.wglUseFontBitmaps(hDC, first, count, listBase);
    afterCommand(fnName);

    return result;
}

// Calls wglUseFontBitmapsA().
BOOL WGLAPI::useFontBitmapsA(HDC hDC, DWORD first, DWORD count, DWORD listBase) const
{
    static const char *fnName = "wglUseFontBitmapsA";

    beforeCommand(fnName, _api.wglUseFontBitmapsA);
    BOOL result = _api.wglUseFontBitmapsA(hDC, first, count, listBase);
    afterCommand(fnName);

    return result;
}

// Calls wglUseFontBitmapsW().
BOOL WGLAPI::useFontBitmapsW(HDC hDC, DWORD first, DWORD count, DWORD listBase) const
{
    static const char *fnName = "wglUseFontBitmapsW";

    beforeCommand(fnName, _api.wglUseFontBitmapsW);
    BOOL result = _api.wglUseFontBitmapsW(hDC, first, count, listBase);
    afterCommand(fnName);

    return result;
}

// Calls wglUseFontOutlines().
BOOL WGLAPI::useFontOutlines(HDC hDC, DWORD first, DWORD count, DWORD listBase,
                             FLOAT deviation, FLOAT extrusion, int format,
                             LPGLYPHMETRICSFLOAT lpgmf) const
{
    static const char *fnName = "wglUseFontOutlines";

    beforeCommand(fnName, _api.wglUseFontOutlines);
    BOOL result = _api.wglUseFontOutlines(hDC, first, count, listBase, deviation,
                                          extrusion, format, lpgmf);
    afterCommand(fnName);

    return result;
}

// Calls wglUseFontOutlinesA().
BOOL WGLAPI::useFontOutlinesA(HDC hDC, DWORD first, DWORD count, DWORD listBase,
                              FLOAT deviation, FLOAT extrusion, int format,
                              LPGLYPHMETRICSFLOAT lpgmf) const
{
    static const char *fnName = "wglUseFontOutlinesA";

    beforeCommand(fnName, _api.wglUseFontOutlinesA);
    BOOL result = _api.wglUseFontOutlinesA(hDC, first, count, listBase,
                                           deviation, extrusion, format, lpgmf);
    afterCommand(fnName);

    return result;
}

// Calls wglUseFontOutlinesW().
BOOL WGLAPI::useFontOutlinesW(HDC hDC, DWORD first, DWORD count, DWORD listBase,
                              FLOAT deviation, FLOAT extrusion, int format,
                              LPGLYPHMETRICSFLOAT lpgmf) const
{
    static const char *fnName = "wglUseFontOutlinesW";

    beforeCommand(fnName, _api.wglUseFontOutlinesW);
    BOOL result = _api.wglUseFontOutlinesW(hDC, first, count, listBase,
                                           deviation, extrusion, format, lpgmf);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// _3DLStereoControlAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_3DL_stereo_control extension.
_3DLStereoControlAPI::_3DLStereoControlAPI() :
    BaseExtension("WGL_3DL_stereo_control")
{
}

// Grant read-only access to the underlying function pointers.
const _3DLStereoControl &_3DLStereoControlAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_3DL_stereo_control entry points.
void _3DLStereoControlAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglSetStereoEmitterState3DL().
BOOL _3DLStereoControlAPI::setStereoEmitterState3DL(HDC hDC, UINT uState) const
{
    static const char *fnName = "wglSetStereoEmitterState3DL";

    beforeCommand(fnName, _api.wglSetStereoEmitterState3DL);
    BOOL result = _api.wglSetStereoEmitterState3DL(hDC, uState);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// AMDGPUAssociationAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_AMD_gpu_association extension.
AMDGPUAssociationAPI::AMDGPUAssociationAPI() :
    BaseExtension("WGL_AMD_gpu_association")
{
}

// Grant read-only access to the underlying function pointers.
const AMDGPUAssociation &AMDGPUAssociationAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_AMD_gpu_association entry points.
void AMDGPUAssociationAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglBlitContextFramebufferAMD().
VOID AMDGPUAssociationAPI::blitContextFramebuffer(HGLRC dstCtx, GLint srcX0,
                                                  GLint srcY0, GLint srcX1,
                                                  GLint srcY1, GLint dstX0,
                                                  GLint dstY0, GLint dstX1,
                                                  GLint dstY1, GLbitfield mask,
                                                  GLenum filter) const
{
    static const char *fnName = "wglBlitContextFramebufferAMD";

    beforeCommand(fnName, _api.wglBlitContextFramebuffer);
    _api.wglBlitContextFramebuffer(dstCtx, srcX0, srcY0, srcX1, srcY1, dstX0,
                                   dstY0, dstX1, dstY1, mask, filter);
    afterCommand(fnName);
}

// Calls wglCreateAssociatedContextAMD().
HGLRC AMDGPUAssociationAPI::createAssociateDContext(UINT id) const
{
    static const char *fnName = "wglCreateAssociatedContextAMD";

    beforeCommand(fnName, _api.wglCreateAssociatedContext);
    HGLRC result = _api.wglCreateAssociatedContext(id);
    afterCommand(fnName);

    return result;
}

// Calls wglCreateAssociatedContextAttribsAMD().
HGLRC AMDGPUAssociationAPI::createAssociateDContextAttribs(UINT id,
                                                           HGLRC hShareContext,
                                                           const int *attribList) const
{
    static const char *fnName = "wglCreateAssociatedContextAttribsAMD";

    beforeCommand(fnName, _api.wglCreateAssociatedContextAttribs);
    HGLRC result = _api.wglCreateAssociatedContextAttribs(id, hShareContext,
                                                          attribList);
    afterCommand(fnName);

    return result;
}

// Calls wglDeleteAssociatedContextAMD().
BOOL AMDGPUAssociationAPI::deleteAssociateDContext(HGLRC hglrc) const
{
    static const char *fnName = "wglDeleteAssociatedContextAMD";

    beforeCommand(fnName, _api.wglDeleteAssociatedContext);
    BOOL result = _api.wglDeleteAssociatedContext(hglrc);
    afterCommand(fnName);

    return result;
}

// Calls wglGetContextGPUIDAMD().
UINT AMDGPUAssociationAPI::getContextGpuid(HGLRC hglrc) const
{
    static const char *fnName = "wglGetContextGPUIDAMD";

    beforeCommand(fnName, _api.wglGetContextGPUID);
    UINT result = _api.wglGetContextGPUID(hglrc);
    afterCommand(fnName);

    return result;
}

// Calls wglGetCurrentAssociatedContextAMD().
HGLRC AMDGPUAssociationAPI::getCurrentAssociateDContext() const
{
    static const char *fnName = "wglGetCurrentAssociatedContextAMD";

    beforeCommand(fnName, _api.wglGetCurrentAssociatedContext);
    HGLRC result = _api.wglGetCurrentAssociatedContext();
    afterCommand(fnName);

    return result;
}

// Calls wglGetGPUIDsAMD().
UINT AMDGPUAssociationAPI::getGpuiDs(UINT maxCount, UINT *ids) const
{
    static const char *fnName = "wglGetGPUIDsAMD";

    beforeCommand(fnName, _api.wglGetGPUIDs);
    UINT result = _api.wglGetGPUIDs(maxCount, ids);
    afterCommand(fnName);

    return result;
}

// Calls wglGetGPUInfoAMD().
INT AMDGPUAssociationAPI::getGPUInfo(UINT id, INT property, GLenum dataType,
                                     UINT size, void *data) const
{
    static const char *fnName = "wglGetGPUInfoAMD";

    beforeCommand(fnName, _api.wglGetGPUInfo);
    INT result = _api.wglGetGPUInfo(id, property, dataType, size, data);
    afterCommand(fnName);

    return result;
}

// Calls wglMakeAssociatedContextCurrentAMD().
BOOL AMDGPUAssociationAPI::makeAssociateDContextCurrent(HGLRC hglrc) const
{
    static const char *fnName = "wglMakeAssociatedContextCurrentAMD";

    beforeCommand(fnName, _api.wglMakeAssociatedContextCurrent);
    BOOL result = _api.wglMakeAssociatedContextCurrent(hglrc);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// ARBBufferRegionAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_ARB_buffer_region extension.
ARBBufferRegionAPI::ARBBufferRegionAPI() :
    BaseExtension("WGL_ARB_buffer_region")
{
}

// Grant read-only access to the underlying function pointers.
const ARBBufferRegion &ARBBufferRegionAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_ARB_buffer_region entry points.
void ARBBufferRegionAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglCreateBufferRegionARB().
HANDLE ARBBufferRegionAPI::createBufferRegion(HDC hDC, int iLayerPlane,
                                              UINT uType) const
{
    static const char *fnName = "wglCreateBufferRegionARB";

    beforeCommand(fnName, _api.wglCreateBufferRegion);
    HANDLE result = _api.wglCreateBufferRegion(hDC, iLayerPlane, uType);
    afterCommand(fnName);

    return result;
}

// Calls wglDeleteBufferRegionARB().
VOID ARBBufferRegionAPI::deleteBufferRegion(HANDLE hRegion) const
{
    static const char *fnName = "wglDeleteBufferRegionARB";

    beforeCommand(fnName, _api.wglDeleteBufferRegion);
    _api.wglDeleteBufferRegion(hRegion);
    afterCommand(fnName);
}

// Calls wglRestoreBufferRegionARB().
BOOL ARBBufferRegionAPI::restoreBufferRegion(HANDLE hRegion, int x, int y,
                                             int width, int height, int xSrc,
                                             int ySrc) const
{
    static const char *fnName = "wglRestoreBufferRegionARB";

    beforeCommand(fnName, _api.wglRestoreBufferRegion);
    BOOL result = _api.wglRestoreBufferRegion(hRegion, x, y, width, height, xSrc,
                                              ySrc);
    afterCommand(fnName);

    return result;
}

// Calls wglSaveBufferRegionARB().
BOOL ARBBufferRegionAPI::saveBufferRegion(HANDLE hRegion, int x, int y,
                                          int width, int height) const
{
    static const char *fnName = "wglSaveBufferRegionARB";

    beforeCommand(fnName, _api.wglSaveBufferRegion);
    BOOL result = _api.wglSaveBufferRegion(hRegion, x, y, width, height);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// ARBCreateContextAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_ARB_create_context extension.
ARBCreateContextAPI::ARBCreateContextAPI() :
    BaseExtension("WGL_ARB_create_context")
{
}

// Grant read-only access to the underlying function pointers.
const ARBCreateContext &ARBCreateContextAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_ARB_create_context entry points.
void ARBCreateContextAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglCreateContextAttribsARB().
HGLRC ARBCreateContextAPI::createContextAttribs(HDC hDC, HGLRC hShareContext,
                                                const int *attribList) const
{
    static const char *fnName = "wglCreateContextAttribsARB";

    beforeCommand(fnName, _api.wglCreateContextAttribs);
    HGLRC result = _api.wglCreateContextAttribs(hDC, hShareContext, attribList);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// ARBExtensionsStringAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_ARB_extensions_string extension.
ARBExtensionsStringAPI::ARBExtensionsStringAPI() :
    BaseExtension("WGL_ARB_extensions_string")
{
}

// Grant read-only access to the underlying function pointers.
const ARBExtensionsString &ARBExtensionsStringAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_ARB_extensions_string entry points.
void ARBExtensionsStringAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglGetExtensionsStringARB().
const char *ARBExtensionsStringAPI::getExtensionsString(HDC hdc) const
{
    static const char *fnName = "wglGetExtensionsStringARB";

    beforeCommand(fnName, _api.wglGetExtensionsString);
    const char *result = _api.wglGetExtensionsString(hdc);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// ARBMakeCurrentReadAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_ARB_make_current_read extension.
ARBMakeCurrentReadAPI::ARBMakeCurrentReadAPI() :
    BaseExtension("WGL_ARB_make_current_read")
{
}

// Grant read-only access to the underlying function pointers.
const ARBMakeCurrentRead &ARBMakeCurrentReadAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_ARB_make_current_read entry points.
void ARBMakeCurrentReadAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglGetCurrentReadDCARB().
HDC ARBMakeCurrentReadAPI::getCurrentReadDC() const
{
    static const char *fnName = "wglGetCurrentReadDCARB";

    beforeCommand(fnName, _api.wglGetCurrentReadDC);
    HDC result = _api.wglGetCurrentReadDC();
    afterCommand(fnName);

    return result;
}

// Calls wglMakeContextCurrentARB().
BOOL ARBMakeCurrentReadAPI::makeContextCurrent(HDC hDrawDC, HDC hReadDC,
                                               HGLRC hglrc) const
{
    static const char *fnName = "wglMakeContextCurrentARB";

    beforeCommand(fnName, _api.wglMakeContextCurrent);
    BOOL result = _api.wglMakeContextCurrent(hDrawDC, hReadDC, hglrc);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// ARBPbufferAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_ARB_pbuffer extension.
ARBPbufferAPI::ARBPbufferAPI() :
    BaseExtension("WGL_ARB_pbuffer")
{
}

// Grant read-only access to the underlying function pointers.
const ARBPbuffer &ARBPbufferAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_ARB_pbuffer entry points.
void ARBPbufferAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglCreatePbufferARB().
HPBUFFERARB ARBPbufferAPI::createPbuffer(HDC hDC, int iPixelFormat, int iWidth,
                                         int iHeight, const int *piAttribList) const
{
    static const char *fnName = "wglCreatePbufferARB";

    beforeCommand(fnName, _api.wglCreatePbuffer);
    HPBUFFERARB result = _api.wglCreatePbuffer(hDC, iPixelFormat, iWidth,
                                               iHeight, piAttribList);
    afterCommand(fnName);

    return result;
}

// Calls wglDestroyPbufferARB().
BOOL ARBPbufferAPI::destroyPbuffer(HPBUFFERARB hPbuffer) const
{
    static const char *fnName = "wglDestroyPbufferARB";

    beforeCommand(fnName, _api.wglDestroyPbuffer);
    BOOL result = _api.wglDestroyPbuffer(hPbuffer);
    afterCommand(fnName);

    return result;
}

// Calls wglGetPbufferDCARB().
HDC ARBPbufferAPI::getPbufferDC(HPBUFFERARB hPbuffer) const
{
    static const char *fnName = "wglGetPbufferDCARB";

    beforeCommand(fnName, _api.wglGetPbufferDC);
    HDC result = _api.wglGetPbufferDC(hPbuffer);
    afterCommand(fnName);

    return result;
}

// Calls wglQueryPbufferARB().
BOOL ARBPbufferAPI::queryPbuffer(HPBUFFERARB hPbuffer, int iAttribute,
                                 int *piValue) const
{
    static const char *fnName = "wglQueryPbufferARB";

    beforeCommand(fnName, _api.wglQueryPbuffer);
    BOOL result = _api.wglQueryPbuffer(hPbuffer, iAttribute, piValue);
    afterCommand(fnName);

    return result;
}

// Calls wglReleasePbufferDCARB().
int ARBPbufferAPI::releasePbufferDC(HPBUFFERARB hPbuffer, HDC hDC) const
{
    static const char *fnName = "wglReleasePbufferDCARB";

    beforeCommand(fnName, _api.wglReleasePbufferDC);
    int result = _api.wglReleasePbufferDC(hPbuffer, hDC);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// ARBPixelFormatAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_ARB_pixel_format extension.
ARBPixelFormatAPI::ARBPixelFormatAPI() :
    BaseExtension("WGL_ARB_pixel_format")
{
}

// Grant read-only access to the underlying function pointers.
const ARBPixelFormat &ARBPixelFormatAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_ARB_pixel_format entry points.
void ARBPixelFormatAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglChoosePixelFormatARB().
BOOL ARBPixelFormatAPI::choosePixelFormat(HDC hdc, const int *piAttribIList,
                                          const FLOAT *pfAttribFList,
                                          UINT nMaxFormats, int *piFormats,
                                          UINT *nNumFormats) const
{
    static const char *fnName = "wglChoosePixelFormatARB";

    beforeCommand(fnName, _api.wglChoosePixelFormat);
    BOOL result = _api.wglChoosePixelFormat(hdc, piAttribIList, pfAttribFList,
                                            nMaxFormats, piFormats, nNumFormats);
    afterCommand(fnName);

    return result;
}

// Calls wglGetPixelFormatAttribfvARB().
BOOL ARBPixelFormatAPI::getPixelFormatAttribFV(HDC hdc, int iPixelFormat,
                                               int iLayerPlane, UINT nAttributes,
                                               const int *piAttributes,
                                               FLOAT *pfValues) const
{
    static const char *fnName = "wglGetPixelFormatAttribfvARB";

    beforeCommand(fnName, _api.wglGetPixelFormatAttribfv);
    BOOL result = _api.wglGetPixelFormatAttribfv(hdc, iPixelFormat, iLayerPlane,
                                                 nAttributes, piAttributes,
                                                 pfValues);
    afterCommand(fnName);

    return result;
}

// Calls wglGetPixelFormatAttribivARB().
BOOL ARBPixelFormatAPI::getPixelFormatAttribIV(HDC hdc, int iPixelFormat,
                                               int iLayerPlane, UINT nAttributes,
                                               const int *piAttributes,
                                               int *piValues) const
{
    static const char *fnName = "wglGetPixelFormatAttribivARB";

    beforeCommand(fnName, _api.wglGetPixelFormatAttribiv);
    BOOL result = _api.wglGetPixelFormatAttribiv(hdc, iPixelFormat, iLayerPlane,
                                                 nAttributes, piAttributes,
                                                 piValues);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// ARBRenderTextureAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_ARB_render_texture extension.
ARBRenderTextureAPI::ARBRenderTextureAPI() :
    BaseExtension("WGL_ARB_render_texture")
{
}

// Grant read-only access to the underlying function pointers.
const ARBRenderTexture &ARBRenderTextureAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_ARB_render_texture entry points.
void ARBRenderTextureAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglBindTexImageARB().
BOOL ARBRenderTextureAPI::bindTexImage(HPBUFFERARB hPbuffer, int iBuffer) const
{
    static const char *fnName = "wglBindTexImageARB";

    beforeCommand(fnName, _api.wglBindTexImage);
    BOOL result = _api.wglBindTexImage(hPbuffer, iBuffer);
    afterCommand(fnName);

    return result;
}

// Calls wglReleaseTexImageARB().
BOOL ARBRenderTextureAPI::releaseTexImage(HPBUFFERARB hPbuffer, int iBuffer) const
{
    static const char *fnName = "wglReleaseTexImageARB";

    beforeCommand(fnName, _api.wglReleaseTexImage);
    BOOL result = _api.wglReleaseTexImage(hPbuffer, iBuffer);
    afterCommand(fnName);

    return result;
}

// Calls wglSetPbufferAttribARB().
BOOL ARBRenderTextureAPI::setPbufferAttrib(HPBUFFERARB hPbuffer,
                                           const int *piAttribList) const
{
    static const char *fnName = "wglSetPbufferAttribARB";

    beforeCommand(fnName, _api.wglSetPbufferAttrib);
    BOOL result = _api.wglSetPbufferAttrib(hPbuffer, piAttribList);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// EXTDisplayColorTableAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_EXT_display_color_table extension.
EXTDisplayColorTableAPI::EXTDisplayColorTableAPI() :
    BaseExtension("WGL_EXT_display_color_table")
{
}

// Grant read-only access to the underlying function pointers.
const EXTDisplayColorTable &EXTDisplayColorTableAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_EXT_display_color_table entry points.
void EXTDisplayColorTableAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglBindDisplayColorTableEXT().
GLboolean EXTDisplayColorTableAPI::bindDisplayColorTable(GLushort id) const
{
    static const char *fnName = "wglBindDisplayColorTableEXT";

    beforeCommand(fnName, _api.wglBindDisplayColorTable);
    GLboolean result = _api.wglBindDisplayColorTable(id);
    afterCommand(fnName);

    return result;
}

// Calls wglCreateDisplayColorTableEXT().
GLboolean EXTDisplayColorTableAPI::createDisplayColorTable(GLushort id) const
{
    static const char *fnName = "wglCreateDisplayColorTableEXT";

    beforeCommand(fnName, _api.wglCreateDisplayColorTable);
    GLboolean result = _api.wglCreateDisplayColorTable(id);
    afterCommand(fnName);

    return result;
}

// Calls wglDestroyDisplayColorTableEXT().
VOID EXTDisplayColorTableAPI::destroyDisplayColorTable(GLushort id) const
{
    static const char *fnName = "wglDestroyDisplayColorTableEXT";

    beforeCommand(fnName, _api.wglDestroyDisplayColorTable);
    _api.wglDestroyDisplayColorTable(id);
    afterCommand(fnName);
}

// Calls wglLoadDisplayColorTableEXT().
GLboolean EXTDisplayColorTableAPI::loadDisplayColorTable(const GLushort *table,
                                                         GLuint length) const
{
    static const char *fnName = "wglLoadDisplayColorTableEXT";

    beforeCommand(fnName, _api.wglLoadDisplayColorTable);
    GLboolean result = _api.wglLoadDisplayColorTable(table, length);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// EXTExtensionsStringAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_EXT_extensions_string extension.
EXTExtensionsStringAPI::EXTExtensionsStringAPI() :
    BaseExtension("WGL_EXT_extensions_string")
{
}

// Grant read-only access to the underlying function pointers.
const EXTExtensionsString &EXTExtensionsStringAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_EXT_extensions_string entry points.
void EXTExtensionsStringAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglGetExtensionsStringEXT().
const char *EXTExtensionsStringAPI::getExtensionsString() const
{
    static const char *fnName = "wglGetExtensionsStringEXT";

    beforeCommand(fnName, _api.wglGetExtensionsString);
    const char *result = _api.wglGetExtensionsString();
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// EXTMakeCurrentReadAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_EXT_make_current_read extension.
EXTMakeCurrentReadAPI::EXTMakeCurrentReadAPI() :
    BaseExtension("WGL_EXT_make_current_read")
{
}

// Grant read-only access to the underlying function pointers.
const EXTMakeCurrentRead &EXTMakeCurrentReadAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_EXT_make_current_read entry points.
void EXTMakeCurrentReadAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglGetCurrentReadDCEXT().
HDC EXTMakeCurrentReadAPI::getCurrentReadDC() const
{
    static const char *fnName = "wglGetCurrentReadDCEXT";

    beforeCommand(fnName, _api.wglGetCurrentReadDC);
    HDC result = _api.wglGetCurrentReadDC();
    afterCommand(fnName);

    return result;
}

// Calls wglMakeContextCurrentEXT().
BOOL EXTMakeCurrentReadAPI::makeContextCurrent(HDC hDrawDC, HDC hReadDC,
                                               HGLRC hglrc) const
{
    static const char *fnName = "wglMakeContextCurrentEXT";

    beforeCommand(fnName, _api.wglMakeContextCurrent);
    BOOL result = _api.wglMakeContextCurrent(hDrawDC, hReadDC, hglrc);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// EXTPbufferAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_EXT_pbuffer extension.
EXTPbufferAPI::EXTPbufferAPI() :
    BaseExtension("WGL_EXT_pbuffer")
{
}

// Grant read-only access to the underlying function pointers.
const EXTPbuffer &EXTPbufferAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_EXT_pbuffer entry points.
void EXTPbufferAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglCreatePbufferEXT().
HPBUFFEREXT EXTPbufferAPI::createPbuffer(HDC hDC, int iPixelFormat, int iWidth,
                                         int iHeight, const int *piAttribList) const
{
    static const char *fnName = "wglCreatePbufferEXT";

    beforeCommand(fnName, _api.wglCreatePbuffer);
    HPBUFFEREXT result = _api.wglCreatePbuffer(hDC, iPixelFormat, iWidth,
                                               iHeight, piAttribList);
    afterCommand(fnName);

    return result;
}

// Calls wglDestroyPbufferEXT().
BOOL EXTPbufferAPI::destroyPbuffer(HPBUFFEREXT hPbuffer) const
{
    static const char *fnName = "wglDestroyPbufferEXT";

    beforeCommand(fnName, _api.wglDestroyPbuffer);
    BOOL result = _api.wglDestroyPbuffer(hPbuffer);
    afterCommand(fnName);

    return result;
}

// Calls wglGetPbufferDCEXT().
HDC EXTPbufferAPI::getPbufferDC(HPBUFFEREXT hPbuffer) const
{
    static const char *fnName = "wglGetPbufferDCEXT";

    beforeCommand(fnName, _api.wglGetPbufferDC);
    HDC result = _api.wglGetPbufferDC(hPbuffer);
    afterCommand(fnName);

    return result;
}

// Calls wglQueryPbufferEXT().
BOOL EXTPbufferAPI::queryPbuffer(HPBUFFEREXT hPbuffer, int iAttribute,
                                 int *piValue) const
{
    static const char *fnName = "wglQueryPbufferEXT";

    beforeCommand(fnName, _api.wglQueryPbuffer);
    BOOL result = _api.wglQueryPbuffer(hPbuffer, iAttribute, piValue);
    afterCommand(fnName);

    return result;
}

// Calls wglReleasePbufferDCEXT().
int EXTPbufferAPI::releasePbufferDC(HPBUFFEREXT hPbuffer, HDC hDC) const
{
    static const char *fnName = "wglReleasePbufferDCEXT";

    beforeCommand(fnName, _api.wglReleasePbufferDC);
    int result = _api.wglReleasePbufferDC(hPbuffer, hDC);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// EXTPixelFormatAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_EXT_pixel_format extension.
EXTPixelFormatAPI::EXTPixelFormatAPI() :
    BaseExtension("WGL_EXT_pixel_format")
{
}

// Grant read-only access to the underlying function pointers.
const EXTPixelFormat &EXTPixelFormatAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_EXT_pixel_format entry points.
void EXTPixelFormatAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglChoosePixelFormatEXT().
BOOL EXTPixelFormatAPI::choosePixelFormat(HDC hdc, const int *piAttribIList,
                                          const FLOAT *pfAttribFList,
                                          UINT nMaxFormats, int *piFormats,
                                          UINT *nNumFormats) const
{
    static const char *fnName = "wglChoosePixelFormatEXT";

    beforeCommand(fnName, _api.wglChoosePixelFormat);
    BOOL result = _api.wglChoosePixelFormat(hdc, piAttribIList, pfAttribFList,
                                            nMaxFormats, piFormats, nNumFormats);
    afterCommand(fnName);

    return result;
}

// Calls wglGetPixelFormatAttribfvEXT().
BOOL EXTPixelFormatAPI::getPixelFormatAttribFV(HDC hdc, int iPixelFormat,
                                               int iLayerPlane, UINT nAttributes,
                                               int *piAttributes,
                                               FLOAT *pfValues) const
{
    static const char *fnName = "wglGetPixelFormatAttribfvEXT";

    beforeCommand(fnName, _api.wglGetPixelFormatAttribfv);
    BOOL result = _api.wglGetPixelFormatAttribfv(hdc, iPixelFormat, iLayerPlane,
                                                 nAttributes, piAttributes,
                                                 pfValues);
    afterCommand(fnName);

    return result;
}

// Calls wglGetPixelFormatAttribivEXT().
BOOL EXTPixelFormatAPI::getPixelFormatAttribIV(HDC hdc, int iPixelFormat,
                                               int iLayerPlane, UINT nAttributes,
                                               int *piAttributes, int *piValues) const
{
    static const char *fnName = "wglGetPixelFormatAttribivEXT";

    beforeCommand(fnName, _api.wglGetPixelFormatAttribiv);
    BOOL result = _api.wglGetPixelFormatAttribiv(hdc, iPixelFormat, iLayerPlane,
                                                 nAttributes, piAttributes,
                                                 piValues);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// EXTSwapControlAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_EXT_swap_control extension.
EXTSwapControlAPI::EXTSwapControlAPI() :
    BaseExtension("WGL_EXT_swap_control")
{
}

// Grant read-only access to the underlying function pointers.
const EXTSwapControl &EXTSwapControlAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_EXT_swap_control entry points.
void EXTSwapControlAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglGetSwapIntervalEXT().
int EXTSwapControlAPI::getSwapInterval() const
{
    static const char *fnName = "wglGetSwapIntervalEXT";

    beforeCommand(fnName, _api.wglGetSwapInterval);
    int result = _api.wglGetSwapInterval();
    afterCommand(fnName);

    return result;
}

// Calls wglSwapIntervalEXT().
BOOL EXTSwapControlAPI::swapInterval(int interval) const
{
    static const char *fnName = "wglSwapIntervalEXT";

    beforeCommand(fnName, _api.wglSwapInterval);
    BOOL result = _api.wglSwapInterval(interval);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// I3DDigitalVideoControlAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_I3D_digital_video_control extension.
I3DDigitalVideoControlAPI::I3DDigitalVideoControlAPI() :
    BaseExtension("WGL_I3D_digital_video_control")
{
}

// Grant read-only access to the underlying function pointers.
const I3DDigitalVideoControl &I3DDigitalVideoControlAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_I3D_digital_video_control entry points.
void I3DDigitalVideoControlAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglGetDigitalVideoParametersI3D().
BOOL I3DDigitalVideoControlAPI::getDigitalVideoParameters(HDC hDC,
                                                          int iAttribute,
                                                          int *piValue) const
{
    static const char *fnName = "wglGetDigitalVideoParametersI3D";

    beforeCommand(fnName, _api.wglGetDigitalVideoParameters);
    BOOL result = _api.wglGetDigitalVideoParameters(hDC, iAttribute, piValue);
    afterCommand(fnName);

    return result;
}

// Calls wglSetDigitalVideoParametersI3D().
BOOL I3DDigitalVideoControlAPI::setDigitalVideoParameters(HDC hDC,
                                                          int iAttribute,
                                                          const int *piValue) const
{
    static const char *fnName = "wglSetDigitalVideoParametersI3D";

    beforeCommand(fnName, _api.wglSetDigitalVideoParameters);
    BOOL result = _api.wglSetDigitalVideoParameters(hDC, iAttribute, piValue);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// I3DGammaAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_I3D_gamma extension.
I3DGammaAPI::I3DGammaAPI() :
    BaseExtension("WGL_I3D_gamma")
{
}

// Grant read-only access to the underlying function pointers.
const I3DGamma &I3DGammaAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_I3D_gamma entry points.
void I3DGammaAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglGetGammaTableI3D().
BOOL I3DGammaAPI::getGammaTable(HDC hDC, int iEntries, USHORT *puRed,
                                USHORT *puGreen, USHORT *puBlue) const
{
    static const char *fnName = "wglGetGammaTableI3D";

    beforeCommand(fnName, _api.wglGetGammaTable);
    BOOL result = _api.wglGetGammaTable(hDC, iEntries, puRed, puGreen, puBlue);
    afterCommand(fnName);

    return result;
}

// Calls wglGetGammaTableParametersI3D().
BOOL I3DGammaAPI::getGammaTableParameters(HDC hDC, int iAttribute, int *piValue) const
{
    static const char *fnName = "wglGetGammaTableParametersI3D";

    beforeCommand(fnName, _api.wglGetGammaTableParameters);
    BOOL result = _api.wglGetGammaTableParameters(hDC, iAttribute, piValue);
    afterCommand(fnName);

    return result;
}

// Calls wglSetGammaTableI3D().
BOOL I3DGammaAPI::setGammaTable(HDC hDC, int iEntries, const USHORT *puRed,
                                const USHORT *puGreen, const USHORT *puBlue) const
{
    static const char *fnName = "wglSetGammaTableI3D";

    beforeCommand(fnName, _api.wglSetGammaTable);
    BOOL result = _api.wglSetGammaTable(hDC, iEntries, puRed, puGreen, puBlue);
    afterCommand(fnName);

    return result;
}

// Calls wglSetGammaTableParametersI3D().
BOOL I3DGammaAPI::setGammaTableParameters(HDC hDC, int iAttribute,
                                          const int *piValue) const
{
    static const char *fnName = "wglSetGammaTableParametersI3D";

    beforeCommand(fnName, _api.wglSetGammaTableParameters);
    BOOL result = _api.wglSetGammaTableParameters(hDC, iAttribute, piValue);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// I3DGenlockAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_I3D_genlock extension.
I3DGenlockAPI::I3DGenlockAPI() :
    BaseExtension("WGL_I3D_genlock")
{
}

// Grant read-only access to the underlying function pointers.
const I3DGenlock &I3DGenlockAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_I3D_genlock entry points.
void I3DGenlockAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglDisableGenlockI3D().
BOOL I3DGenlockAPI::disableGenlock(HDC hDC) const
{
    static const char *fnName = "wglDisableGenlockI3D";

    beforeCommand(fnName, _api.wglDisableGenlock);
    BOOL result = _api.wglDisableGenlock(hDC);
    afterCommand(fnName);

    return result;
}

// Calls wglEnableGenlockI3D().
BOOL I3DGenlockAPI::enableGenlock(HDC hDC) const
{
    static const char *fnName = "wglEnableGenlockI3D";

    beforeCommand(fnName, _api.wglEnableGenlock);
    BOOL result = _api.wglEnableGenlock(hDC);
    afterCommand(fnName);

    return result;
}

// Calls wglGenlockSampleRateI3D().
BOOL I3DGenlockAPI::genlockSampleRate(HDC hDC, UINT uRate) const
{
    static const char *fnName = "wglGenlockSampleRateI3D";

    beforeCommand(fnName, _api.wglGenlockSampleRate);
    BOOL result = _api.wglGenlockSampleRate(hDC, uRate);
    afterCommand(fnName);

    return result;
}

// Calls wglGenlockSourceDelayI3D().
BOOL I3DGenlockAPI::genlockSourceDelay(HDC hDC, UINT uDelay) const
{
    static const char *fnName = "wglGenlockSourceDelayI3D";

    beforeCommand(fnName, _api.wglGenlockSourceDelay);
    BOOL result = _api.wglGenlockSourceDelay(hDC, uDelay);
    afterCommand(fnName);

    return result;
}

// Calls wglGenlockSourceEdgeI3D().
BOOL I3DGenlockAPI::genlockSourceEdge(HDC hDC, UINT uEdge) const
{
    static const char *fnName = "wglGenlockSourceEdgeI3D";

    beforeCommand(fnName, _api.wglGenlockSourceEdge);
    BOOL result = _api.wglGenlockSourceEdge(hDC, uEdge);
    afterCommand(fnName);

    return result;
}

// Calls wglGenlockSourceI3D().
BOOL I3DGenlockAPI::genlockSource(HDC hDC, UINT uSource) const
{
    static const char *fnName = "wglGenlockSourceI3D";

    beforeCommand(fnName, _api.wglGenlockSource);
    BOOL result = _api.wglGenlockSource(hDC, uSource);
    afterCommand(fnName);

    return result;
}

// Calls wglGetGenlockSampleRateI3D().
BOOL I3DGenlockAPI::getGenlockSampleRate(HDC hDC, UINT *uRate) const
{
    static const char *fnName = "wglGetGenlockSampleRateI3D";

    beforeCommand(fnName, _api.wglGetGenlockSampleRate);
    BOOL result = _api.wglGetGenlockSampleRate(hDC, uRate);
    afterCommand(fnName);

    return result;
}

// Calls wglGetGenlockSourceDelayI3D().
BOOL I3DGenlockAPI::getGenlockSourceDelay(HDC hDC, UINT *uDelay) const
{
    static const char *fnName = "wglGetGenlockSourceDelayI3D";

    beforeCommand(fnName, _api.wglGetGenlockSourceDelay);
    BOOL result = _api.wglGetGenlockSourceDelay(hDC, uDelay);
    afterCommand(fnName);

    return result;
}

// Calls wglGetGenlockSourceEdgeI3D().
BOOL I3DGenlockAPI::getGenlockSourceEdge(HDC hDC, UINT *uEdge) const
{
    static const char *fnName = "wglGetGenlockSourceEdgeI3D";

    beforeCommand(fnName, _api.wglGetGenlockSourceEdge);
    BOOL result = _api.wglGetGenlockSourceEdge(hDC, uEdge);
    afterCommand(fnName);

    return result;
}

// Calls wglGetGenlockSourceI3D().
BOOL I3DGenlockAPI::getGenlockSource(HDC hDC, UINT *uSource) const
{
    static const char *fnName = "wglGetGenlockSourceI3D";

    beforeCommand(fnName, _api.wglGetGenlockSource);
    BOOL result = _api.wglGetGenlockSource(hDC, uSource);
    afterCommand(fnName);

    return result;
}

// Calls wglIsEnabledGenlockI3D().
BOOL I3DGenlockAPI::isEnabledGenlock(HDC hDC, BOOL *pFlag) const
{
    static const char *fnName = "wglIsEnabledGenlockI3D";

    beforeCommand(fnName, _api.wglIsEnabledGenlock);
    BOOL result = _api.wglIsEnabledGenlock(hDC, pFlag);
    afterCommand(fnName);

    return result;
}

// Calls wglQueryGenlockMaxSourceDelayI3D().
BOOL I3DGenlockAPI::queryGenlockMaxSourceDelay(HDC hDC, UINT *uMaxLineDelay,
                                               UINT *uMaxPixelDelay) const
{
    static const char *fnName = "wglQueryGenlockMaxSourceDelayI3D";

    beforeCommand(fnName, _api.wglQueryGenlockMaxSourceDelay);
    BOOL result = _api.wglQueryGenlockMaxSourceDelay(hDC, uMaxLineDelay,
                                                     uMaxPixelDelay);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// I3DImageBufferAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_I3D_image_buffer extension.
I3DImageBufferAPI::I3DImageBufferAPI() :
    BaseExtension("WGL_I3D_image_buffer")
{
}

// Grant read-only access to the underlying function pointers.
const I3DImageBuffer &I3DImageBufferAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_I3D_image_buffer entry points.
void I3DImageBufferAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglAssociateImageBufferEventsI3D().
BOOL I3DImageBufferAPI::associateImageBufferEvents(HDC hDC, const HANDLE *pEvent,
                                                   const LPVOID *pAddress,
                                                   const DWORD *pSize,
                                                   UINT count) const
{
    static const char *fnName = "wglAssociateImageBufferEventsI3D";

    beforeCommand(fnName, _api.wglAssociateImageBufferEvents);
    BOOL result = _api.wglAssociateImageBufferEvents(hDC, pEvent, pAddress,
                                                     pSize, count);
    afterCommand(fnName);

    return result;
}

// Calls wglCreateImageBufferI3D().
LPVOID I3DImageBufferAPI::createImageBuffer(HDC hDC, DWORD dwSize, UINT uFlags) const
{
    static const char *fnName = "wglCreateImageBufferI3D";

    beforeCommand(fnName, _api.wglCreateImageBuffer);
    LPVOID result = _api.wglCreateImageBuffer(hDC, dwSize, uFlags);
    afterCommand(fnName);

    return result;
}

// Calls wglDestroyImageBufferI3D().
BOOL I3DImageBufferAPI::destroyImageBuffer(HDC hDC, LPVOID pAddress) const
{
    static const char *fnName = "wglDestroyImageBufferI3D";

    beforeCommand(fnName, _api.wglDestroyImageBuffer);
    BOOL result = _api.wglDestroyImageBuffer(hDC, pAddress);
    afterCommand(fnName);

    return result;
}

// Calls wglReleaseImageBufferEventsI3D().
BOOL I3DImageBufferAPI::releaseImageBufferEvents(HDC hDC, const LPVOID *pAddress,
                                                 UINT count) const
{
    static const char *fnName = "wglReleaseImageBufferEventsI3D";

    beforeCommand(fnName, _api.wglReleaseImageBufferEvents);
    BOOL result = _api.wglReleaseImageBufferEvents(hDC, pAddress, count);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// I3DSwapFrameLockAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_I3D_swap_frame_lock extension.
I3DSwapFrameLockAPI::I3DSwapFrameLockAPI() :
    BaseExtension("WGL_I3D_swap_frame_lock")
{
}

// Grant read-only access to the underlying function pointers.
const I3DSwapFrameLock &I3DSwapFrameLockAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_I3D_swap_frame_lock entry points.
void I3DSwapFrameLockAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglDisableFrameLockI3D().
BOOL I3DSwapFrameLockAPI::disableFrameLock() const
{
    static const char *fnName = "wglDisableFrameLockI3D";

    beforeCommand(fnName, _api.wglDisableFrameLock);
    BOOL result = _api.wglDisableFrameLock();
    afterCommand(fnName);

    return result;
}

// Calls wglEnableFrameLockI3D().
BOOL I3DSwapFrameLockAPI::enableFrameLock() const
{
    static const char *fnName = "wglEnableFrameLockI3D";

    beforeCommand(fnName, _api.wglEnableFrameLock);
    BOOL result = _api.wglEnableFrameLock();
    afterCommand(fnName);

    return result;
}

// Calls wglIsEnabledFrameLockI3D().
BOOL I3DSwapFrameLockAPI::isEnabledFrameLock(BOOL *pFlag) const
{
    static const char *fnName = "wglIsEnabledFrameLockI3D";

    beforeCommand(fnName, _api.wglIsEnabledFrameLock);
    BOOL result = _api.wglIsEnabledFrameLock(pFlag);
    afterCommand(fnName);

    return result;
}

// Calls wglQueryFrameLockMasterI3D().
BOOL I3DSwapFrameLockAPI::queryFrameLockMaster(BOOL *pFlag) const
{
    static const char *fnName = "wglQueryFrameLockMasterI3D";

    beforeCommand(fnName, _api.wglQueryFrameLockMaster);
    BOOL result = _api.wglQueryFrameLockMaster(pFlag);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// I3DSwapFrameUsageAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_I3D_swap_frame_usage extension.
I3DSwapFrameUsageAPI::I3DSwapFrameUsageAPI() :
    BaseExtension("WGL_I3D_swap_frame_usage")
{
}

// Grant read-only access to the underlying function pointers.
const I3DSwapFrameUsage &I3DSwapFrameUsageAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_I3D_swap_frame_usage entry points.
void I3DSwapFrameUsageAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglBeginFrameTrackingI3D().
BOOL I3DSwapFrameUsageAPI::beginFrameTracking() const
{
    static const char *fnName = "wglBeginFrameTrackingI3D";

    beforeCommand(fnName, _api.wglBeginFrameTracking);
    BOOL result = _api.wglBeginFrameTracking();
    afterCommand(fnName);

    return result;
}

// Calls wglEndFrameTrackingI3D().
BOOL I3DSwapFrameUsageAPI::endFrameTracking() const
{
    static const char *fnName = "wglEndFrameTrackingI3D";

    beforeCommand(fnName, _api.wglEndFrameTracking);
    BOOL result = _api.wglEndFrameTracking();
    afterCommand(fnName);

    return result;
}

// Calls wglGetFrameUsageI3D().
BOOL I3DSwapFrameUsageAPI::getFrameUsage(float *pUsage) const
{
    static const char *fnName = "wglGetFrameUsageI3D";

    beforeCommand(fnName, _api.wglGetFrameUsage);
    BOOL result = _api.wglGetFrameUsage(pUsage);
    afterCommand(fnName);

    return result;
}

// Calls wglQueryFrameTrackingI3D().
BOOL I3DSwapFrameUsageAPI::queryFrameTracking(DWORD *pFrameCount,
                                              DWORD *pMissedFrames,
                                              float *pLastMissedUsage) const
{
    static const char *fnName = "wglQueryFrameTrackingI3D";

    beforeCommand(fnName, _api.wglQueryFrameTracking);
    BOOL result = _api.wglQueryFrameTracking(pFrameCount, pMissedFrames,
                                             pLastMissedUsage);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// NVDxInteropAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_NV_DX_interop extension.
NVDxInteropAPI::NVDxInteropAPI() :
    BaseExtension("WGL_NV_DX_interop")
{
}

// Grant read-only access to the underlying function pointers.
const NVDxInterop &NVDxInteropAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_NV_DX_interop entry points.
void NVDxInteropAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglDXCloseDeviceNV().
BOOL NVDxInteropAPI::dxCloseDevice(HANDLE hDevice) const
{
    static const char *fnName = "wglDXCloseDeviceNV";

    beforeCommand(fnName, _api.wglDXCloseDevice);
    BOOL result = _api.wglDXCloseDevice(hDevice);
    afterCommand(fnName);

    return result;
}

// Calls wglDXLockObjectsNV().
BOOL NVDxInteropAPI::dxLockObjects(HANDLE hDevice, GLint count, HANDLE *hObjects) const
{
    static const char *fnName = "wglDXLockObjectsNV";

    beforeCommand(fnName, _api.wglDXLockObjects);
    BOOL result = _api.wglDXLockObjects(hDevice, count, hObjects);
    afterCommand(fnName);

    return result;
}

// Calls wglDXObjectAccessNV().
BOOL NVDxInteropAPI::dxObjectAccess(HANDLE hObject, GLenum access) const
{
    static const char *fnName = "wglDXObjectAccessNV";

    beforeCommand(fnName, _api.wglDXObjectAccess);
    BOOL result = _api.wglDXObjectAccess(hObject, access);
    afterCommand(fnName);

    return result;
}

// Calls wglDXOpenDeviceNV().
HANDLE NVDxInteropAPI::dxOpenDevice(void *dxDevice) const
{
    static const char *fnName = "wglDXOpenDeviceNV";

    beforeCommand(fnName, _api.wglDXOpenDevice);
    HANDLE result = _api.wglDXOpenDevice(dxDevice);
    afterCommand(fnName);

    return result;
}

// Calls wglDXRegisterObjectNV().
HANDLE NVDxInteropAPI::dxRegisterObject(HANDLE hDevice, void *dxObject,
                                        GLuint name, GLenum type, GLenum access) const
{
    static const char *fnName = "wglDXRegisterObjectNV";

    beforeCommand(fnName, _api.wglDXRegisterObject);
    HANDLE result = _api.wglDXRegisterObject(hDevice, dxObject, name, type,
                                             access);
    afterCommand(fnName);

    return result;
}

// Calls wglDXSetResourceShareHandleNV().
BOOL NVDxInteropAPI::dxSetResourceShareHandle(void *dxObject, HANDLE shareHandle) const
{
    static const char *fnName = "wglDXSetResourceShareHandleNV";

    beforeCommand(fnName, _api.wglDXSetResourceShareHandle);
    BOOL result = _api.wglDXSetResourceShareHandle(dxObject, shareHandle);
    afterCommand(fnName);

    return result;
}

// Calls wglDXUnlockObjectsNV().
BOOL NVDxInteropAPI::dxUnlockObjects(HANDLE hDevice, GLint count,
                                     HANDLE *hObjects) const
{
    static const char *fnName = "wglDXUnlockObjectsNV";

    beforeCommand(fnName, _api.wglDXUnlockObjects);
    BOOL result = _api.wglDXUnlockObjects(hDevice, count, hObjects);
    afterCommand(fnName);

    return result;
}

// Calls wglDXUnregisterObjectNV().
BOOL NVDxInteropAPI::dxUnregisterObject(HANDLE hDevice, HANDLE hObject) const
{
    static const char *fnName = "wglDXUnregisterObjectNV";

    beforeCommand(fnName, _api.wglDXUnregisterObject);
    BOOL result = _api.wglDXUnregisterObject(hDevice, hObject);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// NVCopyImageAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_NV_copy_image extension.
NVCopyImageAPI::NVCopyImageAPI() :
    BaseExtension("WGL_NV_copy_image")
{
}

// Grant read-only access to the underlying function pointers.
const NVCopyImage &NVCopyImageAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_NV_copy_image entry points.
void NVCopyImageAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglCopyImageSubDataNV().
BOOL NVCopyImageAPI::copyImageSubData(HGLRC hSrcRC, GLuint srcName,
                                      GLenum srcTarget, GLint srcLevel,
                                      GLint srcX, GLint srcY, GLint srcZ,
                                      HGLRC hDstRC, GLuint dstName,
                                      GLenum dstTarget, GLint dstLevel,
                                      GLint dstX, GLint dstY, GLint dstZ,
                                      GLsizei width, GLsizei height,
                                      GLsizei depth) const
{
    static const char *fnName = "wglCopyImageSubDataNV";

    beforeCommand(fnName, _api.wglCopyImageSubData);
    BOOL result = _api.wglCopyImageSubData(hSrcRC, srcName, srcTarget, srcLevel,
                                           srcX, srcY, srcZ, hDstRC, dstName,
                                           dstTarget, dstLevel, dstX, dstY, dstZ,
                                           width, height, depth);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// NVDelayBeforeSwapAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_NV_delay_before_swap extension.
NVDelayBeforeSwapAPI::NVDelayBeforeSwapAPI() :
    BaseExtension("WGL_NV_delay_before_swap")
{
}

// Grant read-only access to the underlying function pointers.
const NVDelayBeforeSwap &NVDelayBeforeSwapAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_NV_delay_before_swap entry points.
void NVDelayBeforeSwapAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglDelayBeforeSwapNV().
BOOL NVDelayBeforeSwapAPI::delayBeforeSwap(HDC hDC, GLfloat seconds) const
{
    static const char *fnName = "wglDelayBeforeSwapNV";

    beforeCommand(fnName, _api.wglDelayBeforeSwap);
    BOOL result = _api.wglDelayBeforeSwap(hDC, seconds);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// NVGPUAffinityAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_NV_gpu_affinity extension.
NVGPUAffinityAPI::NVGPUAffinityAPI() :
    BaseExtension("WGL_NV_gpu_affinity")
{
}

// Grant read-only access to the underlying function pointers.
const NVGPUAffinity &NVGPUAffinityAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_NV_gpu_affinity entry points.
void NVGPUAffinityAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglCreateAffinityDCNV().
HDC NVGPUAffinityAPI::createAffinityDC(const HGPUNV *phGpuList) const
{
    static const char *fnName = "wglCreateAffinityDCNV";

    beforeCommand(fnName, _api.wglCreateAffinityDC);
    HDC result = _api.wglCreateAffinityDC(phGpuList);
    afterCommand(fnName);

    return result;
}

// Calls wglDeleteDCNV().
BOOL NVGPUAffinityAPI::deleteDC(HDC hdc) const
{
    static const char *fnName = "wglDeleteDCNV";

    beforeCommand(fnName, _api.wglDeleteDC);
    BOOL result = _api.wglDeleteDC(hdc);
    afterCommand(fnName);

    return result;
}

// Calls wglEnumGpuDevicesNV().
BOOL NVGPUAffinityAPI::enumGpuDevices(HGPUNV hGpu, UINT iDeviceIndex,
                                      PGPU_DEVICE lpGpuDevice) const
{
    static const char *fnName = "wglEnumGpuDevicesNV";

    beforeCommand(fnName, _api.wglEnumGpuDevices);
    BOOL result = _api.wglEnumGpuDevices(hGpu, iDeviceIndex, lpGpuDevice);
    afterCommand(fnName);

    return result;
}

// Calls wglEnumGpusFromAffinityDCNV().
BOOL NVGPUAffinityAPI::enumGpusFromAffinityDC(HDC hAffinityDC, UINT iGpuIndex,
                                              HGPUNV *hGpu) const
{
    static const char *fnName = "wglEnumGpusFromAffinityDCNV";

    beforeCommand(fnName, _api.wglEnumGpusFromAffinityDC);
    BOOL result = _api.wglEnumGpusFromAffinityDC(hAffinityDC, iGpuIndex, hGpu);
    afterCommand(fnName);

    return result;
}

// Calls wglEnumGpusNV().
BOOL NVGPUAffinityAPI::enumGpus(UINT iGpuIndex, HGPUNV *phGpu) const
{
    static const char *fnName = "wglEnumGpusNV";

    beforeCommand(fnName, _api.wglEnumGpus);
    BOOL result = _api.wglEnumGpus(iGpuIndex, phGpu);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// NVPresentVideoAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_NV_present_video extension.
NVPresentVideoAPI::NVPresentVideoAPI() :
    BaseExtension("WGL_NV_present_video")
{
}

// Grant read-only access to the underlying function pointers.
const NVPresentVideo &NVPresentVideoAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_NV_present_video entry points.
void NVPresentVideoAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglBindVideoDeviceNV().
BOOL NVPresentVideoAPI::bindVideoDevice(HDC hDc, unsigned int uVideoSlot,
                                        HVIDEOOUTPUTDEVICENV hVideoDevice,
                                        const int *piAttribList) const
{
    static const char *fnName = "wglBindVideoDeviceNV";

    beforeCommand(fnName, _api.wglBindVideoDevice);
    BOOL result = _api.wglBindVideoDevice(hDc, uVideoSlot, hVideoDevice,
                                          piAttribList);
    afterCommand(fnName);

    return result;
}

// Calls wglEnumerateVideoDevicesNV().
int NVPresentVideoAPI::enumerateVideoDevices(HDC hDc,
                                             HVIDEOOUTPUTDEVICENV *phDeviceList) const
{
    static const char *fnName = "wglEnumerateVideoDevicesNV";

    beforeCommand(fnName, _api.wglEnumerateVideoDevices);
    int result = _api.wglEnumerateVideoDevices(hDc, phDeviceList);
    afterCommand(fnName);

    return result;
}

// Calls wglQueryCurrentContextNV().
BOOL NVPresentVideoAPI::queryCurrentContext(int iAttribute, int *piValue) const
{
    static const char *fnName = "wglQueryCurrentContextNV";

    beforeCommand(fnName, _api.wglQueryCurrentContext);
    BOOL result = _api.wglQueryCurrentContext(iAttribute, piValue);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// NVSwapGroupAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_NV_swap_group extension.
NVSwapGroupAPI::NVSwapGroupAPI() :
    BaseExtension("WGL_NV_swap_group")
{
}

// Grant read-only access to the underlying function pointers.
const NVSwapGroup &NVSwapGroupAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_NV_swap_group entry points.
void NVSwapGroupAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglBindSwapBarrierNV().
BOOL NVSwapGroupAPI::bindSwapBarrier(GLuint group, GLuint barrier) const
{
    static const char *fnName = "wglBindSwapBarrierNV";

    beforeCommand(fnName, _api.wglBindSwapBarrier);
    BOOL result = _api.wglBindSwapBarrier(group, barrier);
    afterCommand(fnName);

    return result;
}

// Calls wglJoinSwapGroupNV().
BOOL NVSwapGroupAPI::joinSwapGroup(HDC hDC, GLuint group) const
{
    static const char *fnName = "wglJoinSwapGroupNV";

    beforeCommand(fnName, _api.wglJoinSwapGroup);
    BOOL result = _api.wglJoinSwapGroup(hDC, group);
    afterCommand(fnName);

    return result;
}

// Calls wglQueryFrameCountNV().
BOOL NVSwapGroupAPI::queryFrameCount(HDC hDC, GLuint *count) const
{
    static const char *fnName = "wglQueryFrameCountNV";

    beforeCommand(fnName, _api.wglQueryFrameCount);
    BOOL result = _api.wglQueryFrameCount(hDC, count);
    afterCommand(fnName);

    return result;
}

// Calls wglQueryMaxSwapGroupsNV().
BOOL NVSwapGroupAPI::queryMaxSwapGroups(HDC hDC, GLuint *maxGroups,
                                        GLuint *maxBarriers) const
{
    static const char *fnName = "wglQueryMaxSwapGroupsNV";

    beforeCommand(fnName, _api.wglQueryMaxSwapGroups);
    BOOL result = _api.wglQueryMaxSwapGroups(hDC, maxGroups, maxBarriers);
    afterCommand(fnName);

    return result;
}

// Calls wglQuerySwapGroupNV().
BOOL NVSwapGroupAPI::querySwapGroup(HDC hDC, GLuint *group, GLuint *barrier) const
{
    static const char *fnName = "wglQuerySwapGroupNV";

    beforeCommand(fnName, _api.wglQuerySwapGroup);
    BOOL result = _api.wglQuerySwapGroup(hDC, group, barrier);
    afterCommand(fnName);

    return result;
}

// Calls wglResetFrameCountNV().
BOOL NVSwapGroupAPI::resetFrameCount(HDC hDC) const
{
    static const char *fnName = "wglResetFrameCountNV";

    beforeCommand(fnName, _api.wglResetFrameCount);
    BOOL result = _api.wglResetFrameCount(hDC);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// NVVertexArrayRangeAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_NV_vertex_array_range extension.
NVVertexArrayRangeAPI::NVVertexArrayRangeAPI() :
    BaseExtension("WGL_NV_vertex_array_range")
{
}

// Grant read-only access to the underlying function pointers.
const NVVertexArrayRange &NVVertexArrayRangeAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_NV_vertex_array_range entry points.
void NVVertexArrayRangeAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglAllocateMemoryNV().
void *NVVertexArrayRangeAPI::allocateMemory(GLsizei size, GLfloat readfreq,
                                            GLfloat writefreq, GLfloat priority) const
{
    static const char *fnName = "wglAllocateMemoryNV";

    beforeCommand(fnName, _api.wglAllocateMemory);
    void *result = _api.wglAllocateMemory(size, readfreq, writefreq, priority);
    afterCommand(fnName);

    return result;
}

// Calls wglFreeMemoryNV().
void NVVertexArrayRangeAPI::freeMemory(void *pointer) const
{
    static const char *fnName = "wglFreeMemoryNV";

    beforeCommand(fnName, _api.wglFreeMemory);
    _api.wglFreeMemory(pointer);
    afterCommand(fnName);
}

////////////////////////////////////////////////////////////////////////////////
// NVVideoCaptureAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_NV_video_capture extension.
NVVideoCaptureAPI::NVVideoCaptureAPI() :
    BaseExtension("WGL_NV_video_capture")
{
}

// Grant read-only access to the underlying function pointers.
const NVVideoCapture &NVVideoCaptureAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_NV_video_capture entry points.
void NVVideoCaptureAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglBindVideoCaptureDeviceNV().
BOOL NVVideoCaptureAPI::bindVideoCaptureDevice(UINT uVideoSlot,
                                               HVIDEOINPUTDEVICENV hDevice) const
{
    static const char *fnName = "wglBindVideoCaptureDeviceNV";

    beforeCommand(fnName, _api.wglBindVideoCaptureDevice);
    BOOL result = _api.wglBindVideoCaptureDevice(uVideoSlot, hDevice);
    afterCommand(fnName);

    return result;
}

// Calls wglEnumerateVideoCaptureDevicesNV().
UINT NVVideoCaptureAPI::enumerateVideoCaptureDevices(HDC hDc,
                                                     HVIDEOINPUTDEVICENV *phDeviceList) const
{
    static const char *fnName = "wglEnumerateVideoCaptureDevicesNV";

    beforeCommand(fnName, _api.wglEnumerateVideoCaptureDevices);
    UINT result = _api.wglEnumerateVideoCaptureDevices(hDc, phDeviceList);
    afterCommand(fnName);

    return result;
}

// Calls wglLockVideoCaptureDeviceNV().
BOOL NVVideoCaptureAPI::lockVideoCaptureDevice(HDC hDc,
                                               HVIDEOINPUTDEVICENV hDevice) const
{
    static const char *fnName = "wglLockVideoCaptureDeviceNV";

    beforeCommand(fnName, _api.wglLockVideoCaptureDevice);
    BOOL result = _api.wglLockVideoCaptureDevice(hDc, hDevice);
    afterCommand(fnName);

    return result;
}

// Calls wglQueryVideoCaptureDeviceNV().
BOOL NVVideoCaptureAPI::queryVideoCaptureDevice(HDC hDc,
                                                HVIDEOINPUTDEVICENV hDevice,
                                                int iAttribute, int *piValue) const
{
    static const char *fnName = "wglQueryVideoCaptureDeviceNV";

    beforeCommand(fnName, _api.wglQueryVideoCaptureDevice);
    BOOL result = _api.wglQueryVideoCaptureDevice(hDc, hDevice, iAttribute,
                                                  piValue);
    afterCommand(fnName);

    return result;
}

// Calls wglReleaseVideoCaptureDeviceNV().
BOOL NVVideoCaptureAPI::releaseVideoCaptureDevice(HDC hDc,
                                                  HVIDEOINPUTDEVICENV hDevice) const
{
    static const char *fnName = "wglReleaseVideoCaptureDeviceNV";

    beforeCommand(fnName, _api.wglReleaseVideoCaptureDevice);
    BOOL result = _api.wglReleaseVideoCaptureDevice(hDc, hDevice);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// NVVideoOutputAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_NV_video_output extension.
NVVideoOutputAPI::NVVideoOutputAPI() :
    BaseExtension("WGL_NV_video_output")
{
}

// Grant read-only access to the underlying function pointers.
const NVVideoOutput &NVVideoOutputAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_NV_video_output entry points.
void NVVideoOutputAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglBindVideoImageNV().
BOOL NVVideoOutputAPI::bindVideoImage(HPVIDEODEV hVideoDevice,
                                      HPBUFFERARB hPbuffer, int iVideoBuffer) const
{
    static const char *fnName = "wglBindVideoImageNV";

    beforeCommand(fnName, _api.wglBindVideoImage);
    BOOL result = _api.wglBindVideoImage(hVideoDevice, hPbuffer, iVideoBuffer);
    afterCommand(fnName);

    return result;
}

// Calls wglGetVideoDeviceNV().
BOOL NVVideoOutputAPI::getVideoDevice(HDC hDC, int numDevices,
                                      HPVIDEODEV *hVideoDevice) const
{
    static const char *fnName = "wglGetVideoDeviceNV";

    beforeCommand(fnName, _api.wglGetVideoDevice);
    BOOL result = _api.wglGetVideoDevice(hDC, numDevices, hVideoDevice);
    afterCommand(fnName);

    return result;
}

// Calls wglGetVideoInfoNV().
BOOL NVVideoOutputAPI::getVideoInfo(HPVIDEODEV hpVideoDevice,
                                    unsigned long *pulCounterOutputPbuffer,
                                    unsigned long *pulCounterOutputVideo) const
{
    static const char *fnName = "wglGetVideoInfoNV";

    beforeCommand(fnName, _api.wglGetVideoInfo);
    BOOL result = _api.wglGetVideoInfo(hpVideoDevice, pulCounterOutputPbuffer,
                                       pulCounterOutputVideo);
    afterCommand(fnName);

    return result;
}

// Calls wglReleaseVideoDeviceNV().
BOOL NVVideoOutputAPI::releaseVideoDevice(HPVIDEODEV hVideoDevice) const
{
    static const char *fnName = "wglReleaseVideoDeviceNV";

    beforeCommand(fnName, _api.wglReleaseVideoDevice);
    BOOL result = _api.wglReleaseVideoDevice(hVideoDevice);
    afterCommand(fnName);

    return result;
}

// Calls wglReleaseVideoImageNV().
BOOL NVVideoOutputAPI::releaseVideoImage(HPBUFFERARB hPbuffer, int iVideoBuffer) const
{
    static const char *fnName = "wglReleaseVideoImageNV";

    beforeCommand(fnName, _api.wglReleaseVideoImage);
    BOOL result = _api.wglReleaseVideoImage(hPbuffer, iVideoBuffer);
    afterCommand(fnName);

    return result;
}

// Calls wglSendPbufferToVideoNV().
BOOL NVVideoOutputAPI::sendPbufferToVideo(HPBUFFERARB hPbuffer, int iBufferType,
                                          unsigned long *pulCounterPbuffer,
                                          BOOL bBlock) const
{
    static const char *fnName = "wglSendPbufferToVideoNV";

    beforeCommand(fnName, _api.wglSendPbufferToVideo);
    BOOL result = _api.wglSendPbufferToVideo(hPbuffer, iBufferType,
                                             pulCounterPbuffer, bBlock);
    afterCommand(fnName);

    return result;
}

////////////////////////////////////////////////////////////////////////////////
// OMLSyncControlAPI Member Function Definitions
////////////////////////////////////////////////////////////////////////////////
// Construct an unresolved command set for the WGL_OML_sync_control extension.
OMLSyncControlAPI::OMLSyncControlAPI() :
    BaseExtension("WGL_OML_sync_control")
{
}

// Grant read-only access to the underlying function pointers.
const OMLSyncControl &OMLSyncControlAPI::getRawAPI() const
{
    return _api;
}

// Resolve the WGL_OML_sync_control entry points.
void OMLSyncControlAPI::resolve(const gl::APIResolver *resolver)
{
    if (resolver != nullptr)
    {
        _api.resolveEntryPoints(resolver);

        // Call the base class.
        BaseExtension::resolve(resolver);
    }
}

// Calls wglGetMscRateOML().
BOOL OMLSyncControlAPI::getMscRate(HDC hdc, INT32 *numerator, INT32 *denominator) const
{
    static const char *fnName = "wglGetMscRateOML";

    beforeCommand(fnName, _api.wglGetMscRate);
    BOOL result = _api.wglGetMscRate(hdc, numerator, denominator);
    afterCommand(fnName);

    return result;
}

// Calls wglGetSyncValuesOML().
BOOL OMLSyncControlAPI::getSyncValues(HDC hdc, INT64 *ust, INT64 *msc,
                                      INT64 *sbc) const
{
    static const char *fnName = "wglGetSyncValuesOML";

    beforeCommand(fnName, _api.wglGetSyncValues);
    BOOL result = _api.wglGetSyncValues(hdc, ust, msc, sbc);
    afterCommand(fnName);

    return result;
}

// Calls wglSwapBuffersMscOML().
INT64 OMLSyncControlAPI::swapBuffersMsc(HDC hdc, INT64 target_msc, INT64 divisor,
                                        INT64 remainder) const
{
    static const char *fnName = "wglSwapBuffersMscOML";

    beforeCommand(fnName, _api.wglSwapBuffersMsc);
    INT64 result = _api.wglSwapBuffersMsc(hdc, target_msc, divisor, remainder);
    afterCommand(fnName);

    return result;
}

// Calls wglSwapLayerBuffersMscOML().
INT64 OMLSyncControlAPI::swapLayerBuffersMsc(HDC hdc, INT fuPlanes,
                                             INT64 target_msc, INT64 divisor,
                                             INT64 remainder) const
{
    static const char *fnName = "wglSwapLayerBuffersMscOML";

    beforeCommand(fnName, _api.wglSwapLayerBuffersMsc);
    INT64 result = _api.wglSwapLayerBuffersMsc(hdc, fuPlanes, target_msc,
                                               divisor, remainder);
    afterCommand(fnName);

    return result;
}

// Calls wglWaitForMscOML().
BOOL OMLSyncControlAPI::waitForMsc(HDC hdc, INT64 target_msc, INT64 divisor,
                                   INT64 remainder, INT64 *ust, INT64 *msc,
                                   INT64 *sbc) const
{
    static const char *fnName = "wglWaitForMscOML";

    beforeCommand(fnName, _api.wglWaitForMsc);
    BOOL result = _api.wglWaitForMsc(hdc, target_msc, divisor, remainder, ust,
                                     msc, sbc);
    afterCommand(fnName);

    return result;
}

// Calls wglWaitForSbcOML().
BOOL OMLSyncControlAPI::waitForSbc(HDC hdc, INT64 target_sbc, INT64 *ust,
                                   INT64 *msc, INT64 *sbc) const
{
    static const char *fnName = "wglWaitForSbcOML";

    beforeCommand(fnName, _api.wglWaitForSbc);
    BOOL result = _api.wglWaitForSbc(hdc, target_sbc, ust, msc, sbc);
    afterCommand(fnName);

    return result;
}

} // namespace wgl

