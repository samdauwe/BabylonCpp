#include <GLXW/glxw_glx.h>

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

static void load_procs(void *libgl, struct glxw_glx *ctx);
struct glxw_glx *glxw_glx = 0;

int glxwInitGLXCtx(struct glxw_glx *ctx)
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

int glxwInitGLX(void)
{
    static struct glxw_glx ctx;
    if(glxw_glx || glxwInitGLXCtx(&ctx) == 0)
    {
        glxw_glx = &ctx;
        return 0;
    }

    return -1;
}

static void load_procs(void *libgl, struct glxw_glx *ctx)
{
ctx->_glXGetFBConfigs = (PFNGLXGETFBCONFIGSPROC)get_proc(libgl, "glXGetFBConfigs");
ctx->_glXChooseFBConfig = (PFNGLXCHOOSEFBCONFIGPROC)get_proc(libgl, "glXChooseFBConfig");
ctx->_glXGetFBConfigAttrib = (PFNGLXGETFBCONFIGATTRIBPROC)get_proc(libgl, "glXGetFBConfigAttrib");
ctx->_glXGetVisualFromFBConfig = (PFNGLXGETVISUALFROMFBCONFIGPROC)get_proc(libgl, "glXGetVisualFromFBConfig");
ctx->_glXCreateWindow = (PFNGLXCREATEWINDOWPROC)get_proc(libgl, "glXCreateWindow");
ctx->_glXDestroyWindow = (PFNGLXDESTROYWINDOWPROC)get_proc(libgl, "glXDestroyWindow");
ctx->_glXCreatePixmap = (PFNGLXCREATEPIXMAPPROC)get_proc(libgl, "glXCreatePixmap");
ctx->_glXDestroyPixmap = (PFNGLXDESTROYPIXMAPPROC)get_proc(libgl, "glXDestroyPixmap");
ctx->_glXCreatePbuffer = (PFNGLXCREATEPBUFFERPROC)get_proc(libgl, "glXCreatePbuffer");
ctx->_glXDestroyPbuffer = (PFNGLXDESTROYPBUFFERPROC)get_proc(libgl, "glXDestroyPbuffer");
ctx->_glXQueryDrawable = (PFNGLXQUERYDRAWABLEPROC)get_proc(libgl, "glXQueryDrawable");
ctx->_glXCreateNewContext = (PFNGLXCREATENEWCONTEXTPROC)get_proc(libgl, "glXCreateNewContext");
ctx->_glXMakeContextCurrent = (PFNGLXMAKECONTEXTCURRENTPROC)get_proc(libgl, "glXMakeContextCurrent");
ctx->_glXGetCurrentReadDrawable = (PFNGLXGETCURRENTREADDRAWABLEPROC)get_proc(libgl, "glXGetCurrentReadDrawable");
ctx->_glXQueryContext = (PFNGLXQUERYCONTEXTPROC)get_proc(libgl, "glXQueryContext");
ctx->_glXSelectEvent = (PFNGLXSELECTEVENTPROC)get_proc(libgl, "glXSelectEvent");
ctx->_glXGetSelectedEvent = (PFNGLXGETSELECTEDEVENTPROC)get_proc(libgl, "glXGetSelectedEvent");
ctx->_glXCreateContextAttribsARB = (PFNGLXCREATECONTEXTATTRIBSARBPROC)get_proc(libgl, "glXCreateContextAttribsARB");
ctx->_glXGetGPUIDsAMD = (PFNGLXGETGPUIDSAMDPROC)get_proc(libgl, "glXGetGPUIDsAMD");
ctx->_glXGetGPUInfoAMD = (PFNGLXGETGPUINFOAMDPROC)get_proc(libgl, "glXGetGPUInfoAMD");
ctx->_glXGetContextGPUIDAMD = (PFNGLXGETCONTEXTGPUIDAMDPROC)get_proc(libgl, "glXGetContextGPUIDAMD");
ctx->_glXCreateAssociatedContextAMD = (PFNGLXCREATEASSOCIATEDCONTEXTAMDPROC)get_proc(libgl, "glXCreateAssociatedContextAMD");
ctx->_glXCreateAssociatedContextAttribsAMD = (PFNGLXCREATEASSOCIATEDCONTEXTATTRIBSAMDPROC)get_proc(libgl, "glXCreateAssociatedContextAttribsAMD");
ctx->_glXDeleteAssociatedContextAMD = (PFNGLXDELETEASSOCIATEDCONTEXTAMDPROC)get_proc(libgl, "glXDeleteAssociatedContextAMD");
ctx->_glXMakeAssociatedContextCurrentAMD = (PFNGLXMAKEASSOCIATEDCONTEXTCURRENTAMDPROC)get_proc(libgl, "glXMakeAssociatedContextCurrentAMD");
ctx->_glXGetCurrentAssociatedContextAMD = (PFNGLXGETCURRENTASSOCIATEDCONTEXTAMDPROC)get_proc(libgl, "glXGetCurrentAssociatedContextAMD");
ctx->_glXBlitContextFramebufferAMD = (PFNGLXBLITCONTEXTFRAMEBUFFERAMDPROC)get_proc(libgl, "glXBlitContextFramebufferAMD");
ctx->_glXGetCurrentDisplayEXT = (PFNGLXGETCURRENTDISPLAYEXTPROC)get_proc(libgl, "glXGetCurrentDisplayEXT");
ctx->_glXQueryContextInfoEXT = (PFNGLXQUERYCONTEXTINFOEXTPROC)get_proc(libgl, "glXQueryContextInfoEXT");
ctx->_glXGetContextIDEXT = (PFNGLXGETCONTEXTIDEXTPROC)get_proc(libgl, "glXGetContextIDEXT");
ctx->_glXImportContextEXT = (PFNGLXIMPORTCONTEXTEXTPROC)get_proc(libgl, "glXImportContextEXT");
ctx->_glXFreeContextEXT = (PFNGLXFREECONTEXTEXTPROC)get_proc(libgl, "glXFreeContextEXT");
ctx->_glXSwapIntervalEXT = (PFNGLXSWAPINTERVALEXTPROC)get_proc(libgl, "glXSwapIntervalEXT");
ctx->_glXBindTexImageEXT = (PFNGLXBINDTEXIMAGEEXTPROC)get_proc(libgl, "glXBindTexImageEXT");
ctx->_glXReleaseTexImageEXT = (PFNGLXRELEASETEXIMAGEEXTPROC)get_proc(libgl, "glXReleaseTexImageEXT");
ctx->_glXGetAGPOffsetMESA = (PFNGLXGETAGPOFFSETMESAPROC)get_proc(libgl, "glXGetAGPOffsetMESA");
ctx->_glXCopySubBufferMESA = (PFNGLXCOPYSUBBUFFERMESAPROC)get_proc(libgl, "glXCopySubBufferMESA");
ctx->_glXCreateGLXPixmapMESA = (PFNGLXCREATEGLXPIXMAPMESAPROC)get_proc(libgl, "glXCreateGLXPixmapMESA");
ctx->_glXQueryCurrentRendererIntegerMESA = (PFNGLXQUERYCURRENTRENDERERINTEGERMESAPROC)get_proc(libgl, "glXQueryCurrentRendererIntegerMESA");
ctx->_glXQueryCurrentRendererStringMESA = (PFNGLXQUERYCURRENTRENDERERSTRINGMESAPROC)get_proc(libgl, "glXQueryCurrentRendererStringMESA");
ctx->_glXQueryRendererIntegerMESA = (PFNGLXQUERYRENDERERINTEGERMESAPROC)get_proc(libgl, "glXQueryRendererIntegerMESA");
ctx->_glXQueryRendererStringMESA = (PFNGLXQUERYRENDERERSTRINGMESAPROC)get_proc(libgl, "glXQueryRendererStringMESA");
ctx->_glXReleaseBuffersMESA = (PFNGLXRELEASEBUFFERSMESAPROC)get_proc(libgl, "glXReleaseBuffersMESA");
ctx->_glXSet3DfxModeMESA = (PFNGLXSET3DFXMODEMESAPROC)get_proc(libgl, "glXSet3DfxModeMESA");
ctx->_glXGetSwapIntervalMESA = (PFNGLXGETSWAPINTERVALMESAPROC)get_proc(libgl, "glXGetSwapIntervalMESA");
ctx->_glXSwapIntervalMESA = (PFNGLXSWAPINTERVALMESAPROC)get_proc(libgl, "glXSwapIntervalMESA");
ctx->_glXCopyBufferSubDataNV = (PFNGLXCOPYBUFFERSUBDATANVPROC)get_proc(libgl, "glXCopyBufferSubDataNV");
ctx->_glXNamedCopyBufferSubDataNV = (PFNGLXNAMEDCOPYBUFFERSUBDATANVPROC)get_proc(libgl, "glXNamedCopyBufferSubDataNV");
ctx->_glXCopyImageSubDataNV = (PFNGLXCOPYIMAGESUBDATANVPROC)get_proc(libgl, "glXCopyImageSubDataNV");
ctx->_glXDelayBeforeSwapNV = (PFNGLXDELAYBEFORESWAPNVPROC)get_proc(libgl, "glXDelayBeforeSwapNV");
ctx->_glXEnumerateVideoDevicesNV = (PFNGLXENUMERATEVIDEODEVICESNVPROC)get_proc(libgl, "glXEnumerateVideoDevicesNV");
ctx->_glXBindVideoDeviceNV = (PFNGLXBINDVIDEODEVICENVPROC)get_proc(libgl, "glXBindVideoDeviceNV");
ctx->_glXJoinSwapGroupNV = (PFNGLXJOINSWAPGROUPNVPROC)get_proc(libgl, "glXJoinSwapGroupNV");
ctx->_glXBindSwapBarrierNV = (PFNGLXBINDSWAPBARRIERNVPROC)get_proc(libgl, "glXBindSwapBarrierNV");
ctx->_glXQuerySwapGroupNV = (PFNGLXQUERYSWAPGROUPNVPROC)get_proc(libgl, "glXQuerySwapGroupNV");
ctx->_glXQueryMaxSwapGroupsNV = (PFNGLXQUERYMAXSWAPGROUPSNVPROC)get_proc(libgl, "glXQueryMaxSwapGroupsNV");
ctx->_glXQueryFrameCountNV = (PFNGLXQUERYFRAMECOUNTNVPROC)get_proc(libgl, "glXQueryFrameCountNV");
ctx->_glXResetFrameCountNV = (PFNGLXRESETFRAMECOUNTNVPROC)get_proc(libgl, "glXResetFrameCountNV");
ctx->_glXBindVideoCaptureDeviceNV = (PFNGLXBINDVIDEOCAPTUREDEVICENVPROC)get_proc(libgl, "glXBindVideoCaptureDeviceNV");
ctx->_glXEnumerateVideoCaptureDevicesNV = (PFNGLXENUMERATEVIDEOCAPTUREDEVICESNVPROC)get_proc(libgl, "glXEnumerateVideoCaptureDevicesNV");
ctx->_glXLockVideoCaptureDeviceNV = (PFNGLXLOCKVIDEOCAPTUREDEVICENVPROC)get_proc(libgl, "glXLockVideoCaptureDeviceNV");
ctx->_glXQueryVideoCaptureDeviceNV = (PFNGLXQUERYVIDEOCAPTUREDEVICENVPROC)get_proc(libgl, "glXQueryVideoCaptureDeviceNV");
ctx->_glXReleaseVideoCaptureDeviceNV = (PFNGLXRELEASEVIDEOCAPTUREDEVICENVPROC)get_proc(libgl, "glXReleaseVideoCaptureDeviceNV");
ctx->_glXGetVideoDeviceNV = (PFNGLXGETVIDEODEVICENVPROC)get_proc(libgl, "glXGetVideoDeviceNV");
ctx->_glXReleaseVideoDeviceNV = (PFNGLXRELEASEVIDEODEVICENVPROC)get_proc(libgl, "glXReleaseVideoDeviceNV");
ctx->_glXBindVideoImageNV = (PFNGLXBINDVIDEOIMAGENVPROC)get_proc(libgl, "glXBindVideoImageNV");
ctx->_glXReleaseVideoImageNV = (PFNGLXRELEASEVIDEOIMAGENVPROC)get_proc(libgl, "glXReleaseVideoImageNV");
ctx->_glXSendPbufferToVideoNV = (PFNGLXSENDPBUFFERTOVIDEONVPROC)get_proc(libgl, "glXSendPbufferToVideoNV");
ctx->_glXGetVideoInfoNV = (PFNGLXGETVIDEOINFONVPROC)get_proc(libgl, "glXGetVideoInfoNV");
ctx->_glXGetSyncValuesOML = (PFNGLXGETSYNCVALUESOMLPROC)get_proc(libgl, "glXGetSyncValuesOML");
ctx->_glXGetMscRateOML = (PFNGLXGETMSCRATEOMLPROC)get_proc(libgl, "glXGetMscRateOML");
ctx->_glXSwapBuffersMscOML = (PFNGLXSWAPBUFFERSMSCOMLPROC)get_proc(libgl, "glXSwapBuffersMscOML");
ctx->_glXWaitForMscOML = (PFNGLXWAITFORMSCOMLPROC)get_proc(libgl, "glXWaitForMscOML");
ctx->_glXWaitForSbcOML = (PFNGLXWAITFORSBCOMLPROC)get_proc(libgl, "glXWaitForSbcOML");
ctx->_glXQueryHyperpipeNetworkSGIX = (PFNGLXQUERYHYPERPIPENETWORKSGIXPROC)get_proc(libgl, "glXQueryHyperpipeNetworkSGIX");
ctx->_glXHyperpipeConfigSGIX = (PFNGLXHYPERPIPECONFIGSGIXPROC)get_proc(libgl, "glXHyperpipeConfigSGIX");
ctx->_glXQueryHyperpipeConfigSGIX = (PFNGLXQUERYHYPERPIPECONFIGSGIXPROC)get_proc(libgl, "glXQueryHyperpipeConfigSGIX");
ctx->_glXDestroyHyperpipeConfigSGIX = (PFNGLXDESTROYHYPERPIPECONFIGSGIXPROC)get_proc(libgl, "glXDestroyHyperpipeConfigSGIX");
ctx->_glXBindHyperpipeSGIX = (PFNGLXBINDHYPERPIPESGIXPROC)get_proc(libgl, "glXBindHyperpipeSGIX");
ctx->_glXQueryHyperpipeBestAttribSGIX = (PFNGLXQUERYHYPERPIPEBESTATTRIBSGIXPROC)get_proc(libgl, "glXQueryHyperpipeBestAttribSGIX");
ctx->_glXHyperpipeAttribSGIX = (PFNGLXHYPERPIPEATTRIBSGIXPROC)get_proc(libgl, "glXHyperpipeAttribSGIX");
ctx->_glXQueryHyperpipeAttribSGIX = (PFNGLXQUERYHYPERPIPEATTRIBSGIXPROC)get_proc(libgl, "glXQueryHyperpipeAttribSGIX");
ctx->_glXCreateGLXPbufferSGIX = (PFNGLXCREATEGLXPBUFFERSGIXPROC)get_proc(libgl, "glXCreateGLXPbufferSGIX");
ctx->_glXDestroyGLXPbufferSGIX = (PFNGLXDESTROYGLXPBUFFERSGIXPROC)get_proc(libgl, "glXDestroyGLXPbufferSGIX");
ctx->_glXQueryGLXPbufferSGIX = (PFNGLXQUERYGLXPBUFFERSGIXPROC)get_proc(libgl, "glXQueryGLXPbufferSGIX");
ctx->_glXSelectEventSGIX = (PFNGLXSELECTEVENTSGIXPROC)get_proc(libgl, "glXSelectEventSGIX");
ctx->_glXGetSelectedEventSGIX = (PFNGLXGETSELECTEDEVENTSGIXPROC)get_proc(libgl, "glXGetSelectedEventSGIX");
ctx->_glXBindSwapBarrierSGIX = (PFNGLXBINDSWAPBARRIERSGIXPROC)get_proc(libgl, "glXBindSwapBarrierSGIX");
ctx->_glXQueryMaxSwapBarriersSGIX = (PFNGLXQUERYMAXSWAPBARRIERSSGIXPROC)get_proc(libgl, "glXQueryMaxSwapBarriersSGIX");
ctx->_glXJoinSwapGroupSGIX = (PFNGLXJOINSWAPGROUPSGIXPROC)get_proc(libgl, "glXJoinSwapGroupSGIX");
ctx->_glXBindChannelToWindowSGIX = (PFNGLXBINDCHANNELTOWINDOWSGIXPROC)get_proc(libgl, "glXBindChannelToWindowSGIX");
ctx->_glXChannelRectSGIX = (PFNGLXCHANNELRECTSGIXPROC)get_proc(libgl, "glXChannelRectSGIX");
ctx->_glXQueryChannelRectSGIX = (PFNGLXQUERYCHANNELRECTSGIXPROC)get_proc(libgl, "glXQueryChannelRectSGIX");
ctx->_glXQueryChannelDeltasSGIX = (PFNGLXQUERYCHANNELDELTASSGIXPROC)get_proc(libgl, "glXQueryChannelDeltasSGIX");
ctx->_glXChannelRectSyncSGIX = (PFNGLXCHANNELRECTSYNCSGIXPROC)get_proc(libgl, "glXChannelRectSyncSGIX");
ctx->_glXCushionSGI = (PFNGLXCUSHIONSGIPROC)get_proc(libgl, "glXCushionSGI");
ctx->_glXMakeCurrentReadSGI = (PFNGLXMAKECURRENTREADSGIPROC)get_proc(libgl, "glXMakeCurrentReadSGI");
ctx->_glXGetCurrentReadDrawableSGI = (PFNGLXGETCURRENTREADDRAWABLESGIPROC)get_proc(libgl, "glXGetCurrentReadDrawableSGI");
ctx->_glXSwapIntervalSGI = (PFNGLXSWAPINTERVALSGIPROC)get_proc(libgl, "glXSwapIntervalSGI");
ctx->_glXGetVideoSyncSGI = (PFNGLXGETVIDEOSYNCSGIPROC)get_proc(libgl, "glXGetVideoSyncSGI");
ctx->_glXWaitVideoSyncSGI = (PFNGLXWAITVIDEOSYNCSGIPROC)get_proc(libgl, "glXWaitVideoSyncSGI");
ctx->_glXGetTransparentIndexSUN = (PFNGLXGETTRANSPARENTINDEXSUNPROC)get_proc(libgl, "glXGetTransparentIndexSUN");
}
