#include <GLXW/glxw_wgl.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>

static void* open_libgl(void)
{
    HMODULE libgl;
    libgl = LoadLibraryA("opengl32.dll");
    return (void*)libgl;
}

static void close_libgl(void* libgl)
{
    FreeLibrary((HMODULE)libgl);
}

static void *get_proc(void *libgl, const char *proc)
{
    void *res;

    res = wglGetProcAddress(proc);
    if (!res)
        res = GetProcAddress((HMODULE)libgl, proc);
    return res;
}
#else
#include <dlfcn.h>

#ifndef __APPLE__
typedef void (*__GLXextFuncPtrX)(void);
extern __GLXextFuncPtrX glXGetProcAddress (const GLubyte *);
#endif

static void* open_libgl(void)
{
    void *libgl = dlopen(
#ifdef __APPLE__
        "/System/Library/Frameworks/OpenGL.framework/OpenGL"
#else
        "libGL.so.1"
#endif
        , RTLD_LAZY | RTLD_GLOBAL);
    return (void*)libgl;
}

static void close_libgl(void* libgl)
{
    dlclose(libgl);
}

static void *get_proc(void *libgl, const char *proc)
{
    void *res = 0;

#ifndef __APPLE__
    res = glXGetProcAddress((const unsigned char *) proc);
#endif
    if (!res)
        res = dlsym(libgl, proc);
    return res;
}
#endif

static void load_procs(void *libgl, struct glxw_wgl *ctx);
struct glxw_wgl *glxw_wgl = 0;

int glxwInitWGLCtx(struct glxw_wgl *ctx)
{
    void *libgl;
    if((libgl = open_libgl()))
    {
        load_procs(libgl, ctx);
        close_libgl(libgl);
        return 0;
    }
    return -1;
}

int glxwInitWGL(void)
{
    static struct glxw_wgl ctx;
    if(glxw_wgl || glxwInitWGLCtx(&ctx) == 0)
    {
        glxw_wgl = &ctx;
        return 0;
    }

    return -1;
}

static void load_procs(void *libgl, struct glxw_wgl *ctx)
{
ctx->_wglCreateBufferRegionARB = (PFNWGLCREATEBUFFERREGIONARBPROC)get_proc(libgl, "wglCreateBufferRegionARB");
ctx->_wglDeleteBufferRegionARB = (PFNWGLDELETEBUFFERREGIONARBPROC)get_proc(libgl, "wglDeleteBufferRegionARB");
ctx->_wglSaveBufferRegionARB = (PFNWGLSAVEBUFFERREGIONARBPROC)get_proc(libgl, "wglSaveBufferRegionARB");
ctx->_wglRestoreBufferRegionARB = (PFNWGLRESTOREBUFFERREGIONARBPROC)get_proc(libgl, "wglRestoreBufferRegionARB");
ctx->_wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)get_proc(libgl, "wglCreateContextAttribsARB");
ctx->_wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)get_proc(libgl, "wglGetExtensionsStringARB");
ctx->_wglMakeContextCurrentARB = (PFNWGLMAKECONTEXTCURRENTARBPROC)get_proc(libgl, "wglMakeContextCurrentARB");
ctx->_wglGetCurrentReadDCARB = (PFNWGLGETCURRENTREADDCARBPROC)get_proc(libgl, "wglGetCurrentReadDCARB");
ctx->_wglCreatePbufferARB = (PFNWGLCREATEPBUFFERARBPROC)get_proc(libgl, "wglCreatePbufferARB");
ctx->_wglGetPbufferDCARB = (PFNWGLGETPBUFFERDCARBPROC)get_proc(libgl, "wglGetPbufferDCARB");
ctx->_wglReleasePbufferDCARB = (PFNWGLRELEASEPBUFFERDCARBPROC)get_proc(libgl, "wglReleasePbufferDCARB");
ctx->_wglDestroyPbufferARB = (PFNWGLDESTROYPBUFFERARBPROC)get_proc(libgl, "wglDestroyPbufferARB");
ctx->_wglQueryPbufferARB = (PFNWGLQUERYPBUFFERARBPROC)get_proc(libgl, "wglQueryPbufferARB");
ctx->_wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)get_proc(libgl, "wglGetPixelFormatAttribivARB");
ctx->_wglGetPixelFormatAttribfvARB = (PFNWGLGETPIXELFORMATATTRIBFVARBPROC)get_proc(libgl, "wglGetPixelFormatAttribfvARB");
ctx->_wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)get_proc(libgl, "wglChoosePixelFormatARB");
ctx->_wglBindTexImageARB = (PFNWGLBINDTEXIMAGEARBPROC)get_proc(libgl, "wglBindTexImageARB");
ctx->_wglReleaseTexImageARB = (PFNWGLRELEASETEXIMAGEARBPROC)get_proc(libgl, "wglReleaseTexImageARB");
ctx->_wglSetPbufferAttribARB = (PFNWGLSETPBUFFERATTRIBARBPROC)get_proc(libgl, "wglSetPbufferAttribARB");
ctx->_wglSetStereoEmitterState3DL = (PFNWGLSETSTEREOEMITTERSTATE3DLPROC)get_proc(libgl, "wglSetStereoEmitterState3DL");
ctx->_wglGetGPUIDsAMD = (PFNWGLGETGPUIDSAMDPROC)get_proc(libgl, "wglGetGPUIDsAMD");
ctx->_wglGetGPUInfoAMD = (PFNWGLGETGPUINFOAMDPROC)get_proc(libgl, "wglGetGPUInfoAMD");
ctx->_wglGetContextGPUIDAMD = (PFNWGLGETCONTEXTGPUIDAMDPROC)get_proc(libgl, "wglGetContextGPUIDAMD");
ctx->_wglCreateAssociatedContextAMD = (PFNWGLCREATEASSOCIATEDCONTEXTAMDPROC)get_proc(libgl, "wglCreateAssociatedContextAMD");
ctx->_wglCreateAssociatedContextAttribsAMD = (PFNWGLCREATEASSOCIATEDCONTEXTATTRIBSAMDPROC)get_proc(libgl, "wglCreateAssociatedContextAttribsAMD");
ctx->_wglDeleteAssociatedContextAMD = (PFNWGLDELETEASSOCIATEDCONTEXTAMDPROC)get_proc(libgl, "wglDeleteAssociatedContextAMD");
ctx->_wglMakeAssociatedContextCurrentAMD = (PFNWGLMAKEASSOCIATEDCONTEXTCURRENTAMDPROC)get_proc(libgl, "wglMakeAssociatedContextCurrentAMD");
ctx->_wglGetCurrentAssociatedContextAMD = (PFNWGLGETCURRENTASSOCIATEDCONTEXTAMDPROC)get_proc(libgl, "wglGetCurrentAssociatedContextAMD");
ctx->_wglBlitContextFramebufferAMD = (PFNWGLBLITCONTEXTFRAMEBUFFERAMDPROC)get_proc(libgl, "wglBlitContextFramebufferAMD");
ctx->_wglCreateDisplayColorTableEXT = (PFNWGLCREATEDISPLAYCOLORTABLEEXTPROC)get_proc(libgl, "wglCreateDisplayColorTableEXT");
ctx->_wglLoadDisplayColorTableEXT = (PFNWGLLOADDISPLAYCOLORTABLEEXTPROC)get_proc(libgl, "wglLoadDisplayColorTableEXT");
ctx->_wglBindDisplayColorTableEXT = (PFNWGLBINDDISPLAYCOLORTABLEEXTPROC)get_proc(libgl, "wglBindDisplayColorTableEXT");
ctx->_wglDestroyDisplayColorTableEXT = (PFNWGLDESTROYDISPLAYCOLORTABLEEXTPROC)get_proc(libgl, "wglDestroyDisplayColorTableEXT");
ctx->_wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)get_proc(libgl, "wglGetExtensionsStringEXT");
ctx->_wglMakeContextCurrentEXT = (PFNWGLMAKECONTEXTCURRENTEXTPROC)get_proc(libgl, "wglMakeContextCurrentEXT");
ctx->_wglGetCurrentReadDCEXT = (PFNWGLGETCURRENTREADDCEXTPROC)get_proc(libgl, "wglGetCurrentReadDCEXT");
ctx->_wglCreatePbufferEXT = (PFNWGLCREATEPBUFFEREXTPROC)get_proc(libgl, "wglCreatePbufferEXT");
ctx->_wglGetPbufferDCEXT = (PFNWGLGETPBUFFERDCEXTPROC)get_proc(libgl, "wglGetPbufferDCEXT");
ctx->_wglReleasePbufferDCEXT = (PFNWGLRELEASEPBUFFERDCEXTPROC)get_proc(libgl, "wglReleasePbufferDCEXT");
ctx->_wglDestroyPbufferEXT = (PFNWGLDESTROYPBUFFEREXTPROC)get_proc(libgl, "wglDestroyPbufferEXT");
ctx->_wglQueryPbufferEXT = (PFNWGLQUERYPBUFFEREXTPROC)get_proc(libgl, "wglQueryPbufferEXT");
ctx->_wglGetPixelFormatAttribivEXT = (PFNWGLGETPIXELFORMATATTRIBIVEXTPROC)get_proc(libgl, "wglGetPixelFormatAttribivEXT");
ctx->_wglGetPixelFormatAttribfvEXT = (PFNWGLGETPIXELFORMATATTRIBFVEXTPROC)get_proc(libgl, "wglGetPixelFormatAttribfvEXT");
ctx->_wglChoosePixelFormatEXT = (PFNWGLCHOOSEPIXELFORMATEXTPROC)get_proc(libgl, "wglChoosePixelFormatEXT");
ctx->_wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)get_proc(libgl, "wglSwapIntervalEXT");
ctx->_wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)get_proc(libgl, "wglGetSwapIntervalEXT");
ctx->_wglGetDigitalVideoParametersI3D = (PFNWGLGETDIGITALVIDEOPARAMETERSI3DPROC)get_proc(libgl, "wglGetDigitalVideoParametersI3D");
ctx->_wglSetDigitalVideoParametersI3D = (PFNWGLSETDIGITALVIDEOPARAMETERSI3DPROC)get_proc(libgl, "wglSetDigitalVideoParametersI3D");
ctx->_wglGetGammaTableParametersI3D = (PFNWGLGETGAMMATABLEPARAMETERSI3DPROC)get_proc(libgl, "wglGetGammaTableParametersI3D");
ctx->_wglSetGammaTableParametersI3D = (PFNWGLSETGAMMATABLEPARAMETERSI3DPROC)get_proc(libgl, "wglSetGammaTableParametersI3D");
ctx->_wglGetGammaTableI3D = (PFNWGLGETGAMMATABLEI3DPROC)get_proc(libgl, "wglGetGammaTableI3D");
ctx->_wglSetGammaTableI3D = (PFNWGLSETGAMMATABLEI3DPROC)get_proc(libgl, "wglSetGammaTableI3D");
ctx->_wglEnableGenlockI3D = (PFNWGLENABLEGENLOCKI3DPROC)get_proc(libgl, "wglEnableGenlockI3D");
ctx->_wglDisableGenlockI3D = (PFNWGLDISABLEGENLOCKI3DPROC)get_proc(libgl, "wglDisableGenlockI3D");
ctx->_wglIsEnabledGenlockI3D = (PFNWGLISENABLEDGENLOCKI3DPROC)get_proc(libgl, "wglIsEnabledGenlockI3D");
ctx->_wglGenlockSourceI3D = (PFNWGLGENLOCKSOURCEI3DPROC)get_proc(libgl, "wglGenlockSourceI3D");
ctx->_wglGetGenlockSourceI3D = (PFNWGLGETGENLOCKSOURCEI3DPROC)get_proc(libgl, "wglGetGenlockSourceI3D");
ctx->_wglGenlockSourceEdgeI3D = (PFNWGLGENLOCKSOURCEEDGEI3DPROC)get_proc(libgl, "wglGenlockSourceEdgeI3D");
ctx->_wglGetGenlockSourceEdgeI3D = (PFNWGLGETGENLOCKSOURCEEDGEI3DPROC)get_proc(libgl, "wglGetGenlockSourceEdgeI3D");
ctx->_wglGenlockSampleRateI3D = (PFNWGLGENLOCKSAMPLERATEI3DPROC)get_proc(libgl, "wglGenlockSampleRateI3D");
ctx->_wglGetGenlockSampleRateI3D = (PFNWGLGETGENLOCKSAMPLERATEI3DPROC)get_proc(libgl, "wglGetGenlockSampleRateI3D");
ctx->_wglGenlockSourceDelayI3D = (PFNWGLGENLOCKSOURCEDELAYI3DPROC)get_proc(libgl, "wglGenlockSourceDelayI3D");
ctx->_wglGetGenlockSourceDelayI3D = (PFNWGLGETGENLOCKSOURCEDELAYI3DPROC)get_proc(libgl, "wglGetGenlockSourceDelayI3D");
ctx->_wglQueryGenlockMaxSourceDelayI3D = (PFNWGLQUERYGENLOCKMAXSOURCEDELAYI3DPROC)get_proc(libgl, "wglQueryGenlockMaxSourceDelayI3D");
ctx->_wglCreateImageBufferI3D = (PFNWGLCREATEIMAGEBUFFERI3DPROC)get_proc(libgl, "wglCreateImageBufferI3D");
ctx->_wglDestroyImageBufferI3D = (PFNWGLDESTROYIMAGEBUFFERI3DPROC)get_proc(libgl, "wglDestroyImageBufferI3D");
ctx->_wglAssociateImageBufferEventsI3D = (PFNWGLASSOCIATEIMAGEBUFFEREVENTSI3DPROC)get_proc(libgl, "wglAssociateImageBufferEventsI3D");
ctx->_wglReleaseImageBufferEventsI3D = (PFNWGLRELEASEIMAGEBUFFEREVENTSI3DPROC)get_proc(libgl, "wglReleaseImageBufferEventsI3D");
ctx->_wglEnableFrameLockI3D = (PFNWGLENABLEFRAMELOCKI3DPROC)get_proc(libgl, "wglEnableFrameLockI3D");
ctx->_wglDisableFrameLockI3D = (PFNWGLDISABLEFRAMELOCKI3DPROC)get_proc(libgl, "wglDisableFrameLockI3D");
ctx->_wglIsEnabledFrameLockI3D = (PFNWGLISENABLEDFRAMELOCKI3DPROC)get_proc(libgl, "wglIsEnabledFrameLockI3D");
ctx->_wglQueryFrameLockMasterI3D = (PFNWGLQUERYFRAMELOCKMASTERI3DPROC)get_proc(libgl, "wglQueryFrameLockMasterI3D");
ctx->_wglGetFrameUsageI3D = (PFNWGLGETFRAMEUSAGEI3DPROC)get_proc(libgl, "wglGetFrameUsageI3D");
ctx->_wglBeginFrameTrackingI3D = (PFNWGLBEGINFRAMETRACKINGI3DPROC)get_proc(libgl, "wglBeginFrameTrackingI3D");
ctx->_wglEndFrameTrackingI3D = (PFNWGLENDFRAMETRACKINGI3DPROC)get_proc(libgl, "wglEndFrameTrackingI3D");
ctx->_wglQueryFrameTrackingI3D = (PFNWGLQUERYFRAMETRACKINGI3DPROC)get_proc(libgl, "wglQueryFrameTrackingI3D");
ctx->_wglDXSetResourceShareHandleNV = (PFNWGLDXSETRESOURCESHAREHANDLENVPROC)get_proc(libgl, "wglDXSetResourceShareHandleNV");
ctx->_wglDXOpenDeviceNV = (PFNWGLDXOPENDEVICENVPROC)get_proc(libgl, "wglDXOpenDeviceNV");
ctx->_wglDXCloseDeviceNV = (PFNWGLDXCLOSEDEVICENVPROC)get_proc(libgl, "wglDXCloseDeviceNV");
ctx->_wglDXRegisterObjectNV = (PFNWGLDXREGISTEROBJECTNVPROC)get_proc(libgl, "wglDXRegisterObjectNV");
ctx->_wglDXUnregisterObjectNV = (PFNWGLDXUNREGISTEROBJECTNVPROC)get_proc(libgl, "wglDXUnregisterObjectNV");
ctx->_wglDXObjectAccessNV = (PFNWGLDXOBJECTACCESSNVPROC)get_proc(libgl, "wglDXObjectAccessNV");
ctx->_wglDXLockObjectsNV = (PFNWGLDXLOCKOBJECTSNVPROC)get_proc(libgl, "wglDXLockObjectsNV");
ctx->_wglDXUnlockObjectsNV = (PFNWGLDXUNLOCKOBJECTSNVPROC)get_proc(libgl, "wglDXUnlockObjectsNV");
ctx->_wglCopyImageSubDataNV = (PFNWGLCOPYIMAGESUBDATANVPROC)get_proc(libgl, "wglCopyImageSubDataNV");
ctx->_wglDelayBeforeSwapNV = (PFNWGLDELAYBEFORESWAPNVPROC)get_proc(libgl, "wglDelayBeforeSwapNV");
ctx->_wglEnumGpusNV = (PFNWGLENUMGPUSNVPROC)get_proc(libgl, "wglEnumGpusNV");
ctx->_wglEnumGpuDevicesNV = (PFNWGLENUMGPUDEVICESNVPROC)get_proc(libgl, "wglEnumGpuDevicesNV");
ctx->_wglCreateAffinityDCNV = (PFNWGLCREATEAFFINITYDCNVPROC)get_proc(libgl, "wglCreateAffinityDCNV");
ctx->_wglEnumGpusFromAffinityDCNV = (PFNWGLENUMGPUSFROMAFFINITYDCNVPROC)get_proc(libgl, "wglEnumGpusFromAffinityDCNV");
ctx->_wglDeleteDCNV = (PFNWGLDELETEDCNVPROC)get_proc(libgl, "wglDeleteDCNV");
ctx->_wglEnumerateVideoDevicesNV = (PFNWGLENUMERATEVIDEODEVICESNVPROC)get_proc(libgl, "wglEnumerateVideoDevicesNV");
ctx->_wglBindVideoDeviceNV = (PFNWGLBINDVIDEODEVICENVPROC)get_proc(libgl, "wglBindVideoDeviceNV");
ctx->_wglQueryCurrentContextNV = (PFNWGLQUERYCURRENTCONTEXTNVPROC)get_proc(libgl, "wglQueryCurrentContextNV");
ctx->_wglJoinSwapGroupNV = (PFNWGLJOINSWAPGROUPNVPROC)get_proc(libgl, "wglJoinSwapGroupNV");
ctx->_wglBindSwapBarrierNV = (PFNWGLBINDSWAPBARRIERNVPROC)get_proc(libgl, "wglBindSwapBarrierNV");
ctx->_wglQuerySwapGroupNV = (PFNWGLQUERYSWAPGROUPNVPROC)get_proc(libgl, "wglQuerySwapGroupNV");
ctx->_wglQueryMaxSwapGroupsNV = (PFNWGLQUERYMAXSWAPGROUPSNVPROC)get_proc(libgl, "wglQueryMaxSwapGroupsNV");
ctx->_wglQueryFrameCountNV = (PFNWGLQUERYFRAMECOUNTNVPROC)get_proc(libgl, "wglQueryFrameCountNV");
ctx->_wglResetFrameCountNV = (PFNWGLRESETFRAMECOUNTNVPROC)get_proc(libgl, "wglResetFrameCountNV");
ctx->_wglAllocateMemoryNV = (PFNWGLALLOCATEMEMORYNVPROC)get_proc(libgl, "wglAllocateMemoryNV");
ctx->_wglFreeMemoryNV = (PFNWGLFREEMEMORYNVPROC)get_proc(libgl, "wglFreeMemoryNV");
ctx->_wglBindVideoCaptureDeviceNV = (PFNWGLBINDVIDEOCAPTUREDEVICENVPROC)get_proc(libgl, "wglBindVideoCaptureDeviceNV");
ctx->_wglEnumerateVideoCaptureDevicesNV = (PFNWGLENUMERATEVIDEOCAPTUREDEVICESNVPROC)get_proc(libgl, "wglEnumerateVideoCaptureDevicesNV");
ctx->_wglLockVideoCaptureDeviceNV = (PFNWGLLOCKVIDEOCAPTUREDEVICENVPROC)get_proc(libgl, "wglLockVideoCaptureDeviceNV");
ctx->_wglQueryVideoCaptureDeviceNV = (PFNWGLQUERYVIDEOCAPTUREDEVICENVPROC)get_proc(libgl, "wglQueryVideoCaptureDeviceNV");
ctx->_wglReleaseVideoCaptureDeviceNV = (PFNWGLRELEASEVIDEOCAPTUREDEVICENVPROC)get_proc(libgl, "wglReleaseVideoCaptureDeviceNV");
ctx->_wglGetVideoDeviceNV = (PFNWGLGETVIDEODEVICENVPROC)get_proc(libgl, "wglGetVideoDeviceNV");
ctx->_wglReleaseVideoDeviceNV = (PFNWGLRELEASEVIDEODEVICENVPROC)get_proc(libgl, "wglReleaseVideoDeviceNV");
ctx->_wglBindVideoImageNV = (PFNWGLBINDVIDEOIMAGENVPROC)get_proc(libgl, "wglBindVideoImageNV");
ctx->_wglReleaseVideoImageNV = (PFNWGLRELEASEVIDEOIMAGENVPROC)get_proc(libgl, "wglReleaseVideoImageNV");
ctx->_wglSendPbufferToVideoNV = (PFNWGLSENDPBUFFERTOVIDEONVPROC)get_proc(libgl, "wglSendPbufferToVideoNV");
ctx->_wglGetVideoInfoNV = (PFNWGLGETVIDEOINFONVPROC)get_proc(libgl, "wglGetVideoInfoNV");
ctx->_wglGetSyncValuesOML = (PFNWGLGETSYNCVALUESOMLPROC)get_proc(libgl, "wglGetSyncValuesOML");
ctx->_wglGetMscRateOML = (PFNWGLGETMSCRATEOMLPROC)get_proc(libgl, "wglGetMscRateOML");
ctx->_wglSwapBuffersMscOML = (PFNWGLSWAPBUFFERSMSCOMLPROC)get_proc(libgl, "wglSwapBuffersMscOML");
ctx->_wglSwapLayerBuffersMscOML = (PFNWGLSWAPLAYERBUFFERSMSCOMLPROC)get_proc(libgl, "wglSwapLayerBuffersMscOML");
ctx->_wglWaitForMscOML = (PFNWGLWAITFORMSCOMLPROC)get_proc(libgl, "wglWaitForMscOML");
ctx->_wglWaitForSbcOML = (PFNWGLWAITFORSBCOMLPROC)get_proc(libgl, "wglWaitForSbcOML");
}
