#ifndef e_c
#define e_c

#include "e.h"

#include <stdlib.h>
#include <string.h> /* For memset(). */
#include <assert.h>
#include <math.h>

#include <stdio.h>  /* For printing to stdout. */

#if defined(_WIN32)
    #define E_WINDOWS
#else
    #define E_POSIX

    #ifdef __unix__
        #define E_UNIX
        #ifdef __ORBIS__
            #define E_ORBIS
        #elif defined(__PROSPERO__)
            #define E_PROSPERO
        #elif defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
            #define E_BSD
        #endif
    #endif
    #ifdef __linux__
        #define E_LINUX
    #endif
    #ifdef __APPLE__
        #define E_APPLE
    #endif
    #ifdef __ANDROID__
        #define E_ANDROID
    #endif
    #ifdef __EMSCRIPTEN__
        #define E_EMSCRIPTEN
    #endif
    #if defined(__NX__)
        #define E_NX
    #endif

    #if defined(E_UNIX) && (defined(E_LINUX) || defined(E_BSD))
        #define E_DESKTOP_UNIX
    #endif
#endif


#if defined(SIZE_MAX)
    #define E_SIZE_MAX  SIZE_MAX
#else
    #define E_SIZE_MAX  0xFFFFFFFF  /* When SIZE_MAX is not defined by the standard library just default to the maximum 32-bit unsigned integer. */
#endif


/* No Vulkan with Emscripten. */
#if defined(E_EMSCRIPTEN)
    #ifndef E_NO_VULKAN
    #define E_NO_VULKAN
    #endif
#endif

/* Temporary external dependencies. Eventually these will all be amalgamated. */
#ifndef E_NO_OPENGL
    /* Don't use vkbind with Emscripten. We'll use the GLES3 headers directly with Emscripten. Including vkbind.h will result in redefinition errors. */
    #ifndef E_EMSCRIPTEN
        #define GLBIND_IMPLEMENTATION
        #include "external/glbind/glbind.h"
    #endif
#endif

#ifndef E_NO_VULKAN
    #if defined(E_WINDOWS)
    #define VK_USE_PLATFORM_WIN32_KHR
    #endif

    #define VKBIND_NO_GLOBAL_API
    #define VKBIND_IMPLEMENTATION
    #include "external/vkbind/vkbind.h"
#endif

#define C89STR_IMPLEMENTATION
#include "external/c89str/c89str.h"

/* Suppress a compile-time warning on Emscripten about threading. Shouldn't much matter since threading isn't really a thing with the Emscripten build. */
#if defined(__EMSCRIPTEN__)
#define C89ATOMIC_SUPPRESS_FALLBACK_WARNING
#endif
#define C89THREAD_IMPLEMENTATION
#include "external/c89thread/c89thread.h"

#define ARGV_IMPLEMENTATION
#include "external/argv/argv.h"

#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable:4244)   /* warning C4244: '=': conversion from 'int' to 'lu_byte', possible loss of data */
    #pragma warning(disable:4267)   /* '=': conversion from 'size_t' to 'lua_Integer', possible loss of data */
    #pragma warning(disable:4297)   /* 'luaD_throw': function assumed not to throw an exception but does */
    #pragma warning(disable:4310)   /* cast truncates constant value */    
    #pragma warning(disable:4324)   /* 'lua_longjmp': structure was padded due to alignment specifier */
    #pragma warning(disable:4334)   /* '<<': result of 32-bit shift implicitly converted to 64 bits (was 64-bit shift intended?) */
    #pragma warning(disable:4701)   /* potentially uninitialized local variable '' used */
    #pragma warning(disable:4702)   /* unreachable code */
    #pragma warning(disable:4709)   /* comma operator within array index expression */
#endif
#define LUA_USE_C89
#define LUA_IMPL
#include "external/minilua/minilua.h"
#if defined(_MSC_VER)
    #pragma warning(push)
#endif

#define CGLTF_IMPLEMENTATION
#include "external/cgltf/cgltf.h"



#define E_DEFAULT_CONFIG_FILE_PATH  "config.lua"
#define E_DEFAULT_RESOLUTION_X      1280
#define E_DEFAULT_RESOLUTION_Y      720



#define E_UNUSED(x) (void)x

#ifndef E_MALLOC
#define E_MALLOC(sz) malloc((sz))
#endif

#ifndef E_REALLOC
#define E_REALLOC(p, sz) realloc((p), (sz))
#endif

#ifndef E_FREE
#define E_FREE(p) free((p))
#endif

static void e_zero_memory_default(void* p, size_t sz)
{
    if (sz > 0) {
        memset(p, 0, sz);
    }
}

#ifndef E_ZERO_MEMORY
#define E_ZERO_MEMORY(p, sz) e_zero_memory_default((p), (sz))
#endif

#ifndef E_COPY_MEMORY
#define E_COPY_MEMORY(dst, src, sz) memcpy((dst), (src), (sz))
#endif

#ifndef E_MOVE_MEMORY
#define E_MOVE_MEMORY(dst, src, sz) memmove((dst), (src), (sz))
#endif

#ifndef E_ASSERT
#define E_ASSERT(condition) assert(condition)
#endif

#define E_ZERO_OBJECT(p)           E_ZERO_MEMORY((p), sizeof(*(p)))
#define E_COUNTOF(x)               (sizeof(x) / sizeof(x[0]))
#define E_MAX(x, y)                (((x) > (y)) ? (x) : (y))
#define E_MIN(x, y)                (((x) < (y)) ? (x) : (y))
#define E_ABS(x)                   (((x) > 0) ? (x) : -(x))
#define E_CLAMP(x, lo, hi)         (E_MAX((lo), E_MIN((x), (hi))))
#define E_OFFSET_PTR(p, offset)    (((unsigned char*)(p)) + (offset))
#define E_ALIGN(x, a)              ((x + (a-1)) & ~(a-1))
#define E_ALIGN_64(x)              E_ALIGN(x, 8)



E_API const char* e_result_description(e_result result)
{
    switch (result)
    {
        case E_SUCCESS:                       return "No error";
        case E_ERROR:                         return "Unknown error";
        case E_INVALID_ARGS:                  return "Invalid argument";
        case E_INVALID_OPERATION:             return "Invalid operation";
        case E_OUT_OF_MEMORY:                 return "Out of memory";
        case E_OUT_OF_RANGE:                  return "Out of range";
        case E_ACCESS_DENIED:                 return "Permission denied";
        case E_DOES_NOT_EXIST:                return "Resource does not exist";
        case E_ALREADY_EXISTS:                return "Resource already exists";
        case E_TOO_MANY_OPEN_FILES:           return "Too many open files";
        case E_INVALID_FILE:                  return "Invalid file";
        case E_TOO_BIG:                       return "Too large";
        case E_PATH_TOO_LONG:                 return "Path too long";
        case E_NAME_TOO_LONG:                 return "Name too long";
        case E_NOT_DIRECTORY:                 return "Not a directory";
        case E_IS_DIRECTORY:                  return "Is a directory";
        case E_DIRECTORY_NOT_EMPTY:           return "Directory not empty";
        case E_AT_END:                        return "At end";
        case E_NO_SPACE:                      return "No space available";
        case E_BUSY:                          return "Device or resource busy";
        case E_IO_ERROR:                      return "Input/output error";
        case E_INTERRUPT:                     return "Interrupted";
        case E_UNAVAILABLE:                   return "Resource unavailable";
        case E_ALREADY_IN_USE:                return "Resource already in use";
        case E_BAD_ADDRESS:                   return "Bad address";
        case E_BAD_SEEK:                      return "Illegal seek";
        case E_BAD_PIPE:                      return "Broken pipe";
        case E_DEADLOCK:                      return "Deadlock";
        case E_TOO_MANY_LINKS:                return "Too many links";
        case E_NOT_IMPLEMENTED:               return "Not implemented";
        case E_NO_MESSAGE:                    return "No message of desired type";
        case E_BAD_MESSAGE:                   return "Invalid message";
        case E_NO_DATA_AVAILABLE:             return "No data available";
        case E_INVALID_DATA:                  return "Invalid data";
        case E_TIMEOUT:                       return "Timeout";
        case E_NO_NETWORK:                    return "Network unavailable";
        case E_NOT_UNIQUE:                    return "Not unique";
        case E_NOT_SOCKET:                    return "Socket operation on non-socket";
        case E_NO_ADDRESS:                    return "Destination address required";
        case E_BAD_PROTOCOL:                  return "Protocol wrong type for socket";
        case E_PROTOCOL_UNAVAILABLE:          return "Protocol not available";
        case E_PROTOCOL_NOT_SUPPORTED:        return "Protocol not supported";
        case E_PROTOCOL_FAMILY_NOT_SUPPORTED: return "Protocol family not supported";
        case E_ADDRESS_FAMILY_NOT_SUPPORTED:  return "Address family not supported";
        case E_SOCKET_NOT_SUPPORTED:          return "Socket type not supported";
        case E_CONNECTION_RESET:              return "Connection reset";
        case E_ALREADY_CONNECTED:             return "Already connected";
        case E_NOT_CONNECTED:                 return "Not connected";
        case E_CONNECTION_REFUSED:            return "Connection refused";
        case E_NO_HOST:                       return "No host";
        case E_IN_PROGRESS:                   return "Operation in progress";
        case E_CANCELLED:                     return "Operation cancelled";
        case E_MEMORY_ALREADY_MAPPED:         return "Memory already mapped";
        default:                              return "Unknown error";
    }
}


static e_result e_result_from_errno(int error)
{
    switch (error)
    {
        case 0:      return E_SUCCESS;
        case ENOENT: return E_DOES_NOT_EXIST;
        case ENOMEM: return E_OUT_OF_MEMORY;
        case EINVAL: return E_INVALID_ARGS;
        default: break;
    }

    /* Fall back to a generic error. */
    return E_ERROR;
}



static int e_vsnprintf(char* buf, size_t count, const char* fmt, va_list args)
{
    return c89str_vsnprintf(buf, count, fmt, args);
}

static int e_snprintf(char* buf, size_t count, const char* fmt, ...)
{
    int result;
    va_list args;

    va_start(args, fmt);
    {
        result = e_vsnprintf(buf, count, fmt, args);
    }
    va_end(args);

    return result;
}





static e_result e_window_handle_event(e_window* pWindow, e_window_event* pEvent)
{
    E_ASSERT(pEvent != NULL);
    E_ASSERT(pWindow != NULL);
    E_ASSERT(pWindow->pVTable != NULL);
    E_ASSERT(pWindow->pVTable->onEvent != NULL);

    return pWindow->pVTable->onEvent(pWindow->pVTableUserData, pWindow, pEvent);
}

static e_window_event e_window_event_init(e_window_event_type type)
{
    e_window_event e;

    e.type = type;

    return e;
}


/*
Platform-Specific Section
*/
static e_result e_platform_init(void);
static e_result e_platform_uninit(void);
static void* e_platform_get_object(e_platform_object_type type);

static size_t   e_platform_window_sizeof(void);
static e_result e_platform_window_init_preallocated(const e_window_config* pConfig, e_window* pOwnerWindow, const e_allocation_callbacks* pAllocationCallbacks, e_platform_window* pWindow);
static e_result e_platform_window_uninit(e_platform_window* pWindow, const e_allocation_callbacks* pAllocationCallbacks);
static void* e_platform_window_get_object(const e_platform_window* pWindow, e_platform_object_type type);    /* Return null when an internal object is not supported. */
static e_result e_platform_window_set_size(e_platform_window* pWindow, unsigned int sizeX, unsigned int sizeY);

typedef e_result (* e_platform_main_loop_iteration_callback)(void* pUserData);
static e_result e_platform_main_loop(int* pExitCode, e_platform_main_loop_iteration_callback iterationCallback, void* pUserData);
static e_result e_platform_exit_main_loop(int exitCode);

#if defined(E_WINDOWS)
#include <windows.h>

#define E_PLATFORM_WINDOW_CLASS_NAME    L"e_window_class"

static LRESULT e_platform_default_window_proc_win32(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef enum
{
    TA_PROCESS_DPI_UNAWARE = 0,
    TA_PROCESS_SYSTEM_DPI_AWARE = 1,
    TA_PROCESS_PER_MONITOR_DPI_AWARE = 2
} E_PROCESS_DPI_AWARENESS;

typedef enum
{
    TA_MDT_EFFECTIVE_DPI = 0,
    TA_MDT_ANGULAR_DPI = 1,
    TA_MDT_RAW_DPI = 2,
    TA_MDT_DEFAULT = TA_MDT_EFFECTIVE_DPI
} E_MONITOR_DPI_TYPE;

typedef BOOL    (__stdcall * E_PFN_SetProcessDPIAware)     (void);
typedef HRESULT (__stdcall * E_PFN_SetProcessDpiAwareness) (E_PROCESS_DPI_AWARENESS);
typedef HRESULT (__stdcall * E_PFN_GetDpiForMonitor)       (HMONITOR hmonitor, E_MONITOR_DPI_TYPE dpiType, UINT *dpiX, UINT *dpiY);


static void e_make_dpi_aware_win32(void)
{
    e_bool32 fallBackToDiscouragedAPI = E_FALSE;
    HMODULE hSHCoreDLL;

    // We can't call SetProcessDpiAwareness() directly because otherwise on versions of Windows < 8.1 we'll get an error at load time about
    // a missing DLL.
    hSHCoreDLL = LoadLibraryW(L"shcore.dll");
    if (hSHCoreDLL != NULL) {
        E_PFN_SetProcessDpiAwareness _SetProcessDpiAwareness = (E_PFN_SetProcessDpiAwareness)GetProcAddress(hSHCoreDLL, "SetProcessDpiAwareness");
        if (_SetProcessDpiAwareness != NULL) {
            if (_SetProcessDpiAwareness(TA_PROCESS_PER_MONITOR_DPI_AWARE) != S_OK) {
                fallBackToDiscouragedAPI = E_TRUE;
            }
        } else {
            fallBackToDiscouragedAPI = E_TRUE;
        }

        FreeLibrary(hSHCoreDLL);
    } else {
        fallBackToDiscouragedAPI = E_TRUE;
    }

    if (fallBackToDiscouragedAPI) {
        HMODULE hUser32DLL = LoadLibraryW(L"user32.dll");
        if (hUser32DLL != NULL) {
            E_PFN_SetProcessDPIAware _SetProcessDPIAware = (E_PFN_SetProcessDPIAware)GetProcAddress(hUser32DLL, "SetProcessDPIAware");
            if (_SetProcessDPIAware != NULL) {
                _SetProcessDPIAware();
            }

            FreeLibrary(hUser32DLL);
        }
    }
}

static e_result e_platform_init(void)
{
    WNDCLASSEXW wc;

    /*
    The Windows operating system likes to automatically change the size of the game window when DPI scaling is
    used. For example, if the user has their DPI set to 200%, the operating system will try to be helpful and
    automatically resize every window by 200%. The size of the window controls the resolution the game runs at,
    but we want that resolution to be set explicitly via something like an options menu. Thus, we don't want the
    operating system to be changing the size of the window to anything other than what we explicitly request. To
    do this, we just tell the operation system that it shouldn't do DPI scaling and that we'll do it ourselves
    manually.
    */
    e_make_dpi_aware_win32();

    ZeroMemory(&wc, sizeof(wc));
    wc.cbSize        = sizeof(wc);
    wc.cbWndExtra    = sizeof(void*);
    wc.lpfnWndProc   = (WNDPROC)e_platform_default_window_proc_win32;
    wc.lpszClassName = E_PLATFORM_WINDOW_CLASS_NAME;
    wc.hCursor       = LoadCursorW(NULL, MAKEINTRESOURCEW(32512));
    wc.style         = CS_OWNDC | CS_DBLCLKS;
    if (!RegisterClassExW(&wc)) {
        return E_ERROR;
    }

    return E_SUCCESS;
}

static e_result e_platform_uninit(void)
{
    UnregisterClassW(E_PLATFORM_WINDOW_CLASS_NAME, GetModuleHandleW(NULL));
    return E_SUCCESS;
}

static void* e_platform_get_object(e_platform_object_type type)
{
#if 0
    switch (type)
    {
    default: break;
    }
#else
    (void)type;
#endif

    /* Getting here means we're not aware of the object type. */
    return NULL;
}


struct e_platform_window
{
    e_window* pOwnerWindow;
    HWND hWnd;
    HDC hDC;    /* Can use GetDC() instead, but in my experience that has tended to be extremely slow. */
};

static size_t e_platform_window_sizeof(void)
{
    return sizeof(e_platform_window);
}

static e_result e_platform_window_init_preallocated(const e_window_config* pConfig, e_window* pOwnerWindow, const e_allocation_callbacks* pAllocationCallbacks, e_platform_window* pWindow)
{
    DWORD dwExStyle;
    DWORD dwStyle;
    const WCHAR* pTitleW = NULL;
    WCHAR* pTitleBuffer = NULL;
    int titleBufferCap;

    E_ASSERT(pConfig != NULL);
    E_ASSERT(pWindow != NULL);

    pWindow->pOwnerWindow = pOwnerWindow;

    /* TODO: Handle different window flags like fullscreen mode. */
    dwExStyle = 0;
    dwStyle = WS_OVERLAPPEDWINDOW;

    if ((pConfig->flags & E_WINDOW_FLAG_HIDDEN) == 0) {
        dwStyle |= WS_VISIBLE;
    }


    /* Convert the title to multi-byte so we can support more than just ASCII. */
    titleBufferCap = MultiByteToWideChar(CP_UTF8, 0, pConfig->pTitle, -1, NULL, 0);
    if (titleBufferCap == 0) {
        pTitleW = L"Game";
    } else {
        pTitleBuffer = (WCHAR*)e_malloc(titleBufferCap * sizeof(*pTitleBuffer), pAllocationCallbacks);
        if (pTitleBuffer == NULL) {
            return E_OUT_OF_MEMORY;
        }

        MultiByteToWideChar(CP_UTF8, 0, pConfig->pTitle, -1, pTitleBuffer, titleBufferCap);
        pTitleW = pTitleBuffer;
    }

    E_ASSERT(pTitleW != NULL);

    pWindow->hWnd = CreateWindowExW(dwExStyle, E_PLATFORM_WINDOW_CLASS_NAME, pTitleW, dwStyle, CW_USEDEFAULT, CW_USEDEFAULT, pConfig->sizeX, pConfig->sizeY, NULL, NULL, NULL, NULL);
    if (pWindow->hWnd == NULL) {
        return E_ERROR; /* Failed to create the window. */
    }

    /* If we used a buffer for the title we'll need to make sure we free it here since we don't need it anymore. */
    if (pTitleBuffer != NULL) {
        e_free(pTitleBuffer, pAllocationCallbacks);
        pTitleBuffer = NULL;    /* Don't want to be using this variable from here on out. */
    }

    /* We need to associate the e_platform_window with the HWND so we can access it from the event handler. */
    SetWindowLongPtrW(pWindow->hWnd, 0, (LONG_PTR)pWindow);

    /*
    At this point the window is created but we need to make some adjustments. The size of the window should be
    that of the client area, but currently it'll be slightly smaller because of the window border. We need to
    expand it a little bit so the client area is exactly the size we asked for.
    */
    e_platform_window_set_size(pWindow, pConfig->sizeX, pConfig->sizeY);

    /* Cache the device context because GetDC() tends to be slow. */
    pWindow->hDC = GetDC(pWindow->hWnd);

    /*
    If we're requesting OpenGL support we'll need to set the pixel format. We shouldn't need the OpenGL headers
    for this particular part, but we'll need them for when we create a rendering context.
    */
    if ((pConfig->flags & E_WINDOW_FLAG_OPENGL) != 0) {
        PIXELFORMATDESCRIPTOR pfd;
        int pixelFormat;

        memset(&pfd, 0, sizeof(pfd));
        pfd.nSize        = sizeof(pfd);
        pfd.nVersion     = 1;
        pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType   = PFD_TYPE_RGBA;
        pfd.cStencilBits = 8;
        pfd.cDepthBits   = 24;
        pfd.cColorBits   = 32;
        pixelFormat = ChoosePixelFormat(pWindow->hDC, &pfd);
        if (pixelFormat == 0) {
            /*
            We could not find a pixel format. This will prevent us from using the window with OpenGL. Since we've
            explicity asked for OpenGL support, the best option is to abort with an error.
            */
            DestroyWindow(pWindow->hWnd);
            return E_ERROR;
        }

        SetPixelFormat(pWindow->hDC, pixelFormat, &pfd);
    }

    return E_SUCCESS;
}

static e_result e_platform_window_uninit(e_platform_window* pWindow, const e_allocation_callbacks* pAllocationCallbacks)
{
    E_ASSERT(pWindow != NULL);
    E_UNUSED(pAllocationCallbacks);

    DestroyWindow(pWindow->hWnd);

    return E_SUCCESS;
}

static void* e_platform_window_get_object(const e_platform_window* pWindow, e_platform_object_type type)
{
    E_ASSERT(pWindow != NULL);

    switch (type)
    {
    case E_PLATFORM_OBJECT_WIN32_HWND: return (void*)pWindow->hWnd;
    case E_PLATFORM_OBJECT_WIN32_HDC:  return (void*)pWindow->hDC;
    default: break;
    }

    return NULL;
}

static e_result e_platform_window_set_size(e_platform_window* pWindow, unsigned int sizeX, unsigned int sizeY)
{
    /*
    We use a single API to both move and resize the window. The size of the client area (the game's resolution) needs to be calculated
    based on the size of the border area of the window.
    */
    UINT swpflags = SWP_NOZORDER | SWP_NOMOVE;
    RECT windowRect;
    RECT clientRect;
    int windowSizeX;
    int windowSizeY;
    int windowPosX = 0;
    int windowPosY = 0;

    E_ASSERT(pWindow != NULL);

    GetWindowRect(pWindow->hWnd, &windowRect);
    GetClientRect(pWindow->hWnd, &clientRect);

    windowSizeX = (int)sizeX + ((windowRect.right - windowRect.left) - (clientRect.right - clientRect.left));
    windowSizeY = (int)sizeY + ((windowRect.bottom - windowRect.top) - (clientRect.bottom - clientRect.top));

    if (!SetWindowPos(pWindow->hWnd, NULL, windowPosX, windowPosY, windowSizeX, windowSizeY, swpflags)) {
        return E_ERROR;    /* An error occurred when trying to move/resize the window. */
    }

    return E_SUCCESS;
}


static LRESULT e_platform_default_window_proc_win32(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    e_platform_window* pWindow = (e_platform_window*)GetWindowLongPtrW(hWnd, 0);
    if (pWindow != NULL) {
        e_window_event e;

        switch (msg)
        {
            case WM_CLOSE:
            {
                e = e_window_event_init(E_WINDOW_EVENT_CLOSE);
                e_window_handle_event(pWindow->pOwnerWindow, &e);
                return 0;
            };

            default: break;
        }
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}



static e_result e_platform_main_loop(int* pExitCode, e_platform_main_loop_iteration_callback iterationCallback, void* pUserData)
{
    int exitCode = 0;
    MSG msg;
    e_bool32 blocking = E_FALSE;

    E_ASSERT(pExitCode != NULL);
    E_ASSERT(iterationCallback != NULL);

    /*
    Need to think about how to handle the main loop here.

    When a game is running, the loop needs to run at full speed. However, when only an editor is
    running, we don't want to do that because otherwise a CPU core will be constantly pinned at
    100% usage.

    Might need some kind of mechanism so allow switching between blocking and non-blocking.
    */
    blocking = E_FALSE;

    for (;;) {
        BOOL result;

        if (blocking) {
            result = GetMessageA(&msg, NULL, 0, 0);
            if (result == -1) {
                *pExitCode = -1;
                return E_ERROR; /* Unknown error. */
            }
            if (result == 0) {
                exitCode = (int)msg.wParam;
                break;
            }

            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        } else {
            e_bool32 receivedQuitMessage = E_FALSE;

            while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    receivedQuitMessage = E_TRUE;
                    exitCode = (int)msg.wParam;
                    break;
                }

                TranslateMessage(&msg);
                DispatchMessageA(&msg);
            }

            if (receivedQuitMessage) {
                break;
            }
        }
        
        /* What do we do with the result code? */
        iterationCallback(pUserData);
    }

    *pExitCode = exitCode;
    return E_SUCCESS;
}

static e_result e_platform_exit_main_loop(int exitCode)
{
    PostQuitMessage(exitCode);
    return E_SUCCESS;
}
#endif  /* E_WINDOWS */


#if defined(E_EMSCRIPTEN)
#include <emscripten.h>
#include <EGL/egl.h>

static EGLDisplay e_gDisplayEGL = EGL_NO_DISPLAY;
static EGLint e_gEGLVersionMajor;
static EGLint e_gEGLVersionMinor;

/* TODO: Update this to take a pointer to a log object. */
static e_result e_platform_init(void)
{
    /* TODO: Should reference count this for robustness. The Emscripten build will be single threaded so can just use a simple counter. No need to atomics or locking or anything. */
    e_gDisplayEGL = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (e_gDisplayEGL == EGL_NO_DISPLAY) {
        /* TODO: Log this. */
        return E_ERROR;
    }

    if (!eglInitialize(e_gDisplayEGL, &e_gEGLVersionMajor, &e_gEGLVersionMinor)) {
        /* TODO: Log this. */
        return E_ERROR;
    }

    /* TODO: Log the EGL version as info. */

    return E_SUCCESS;
}

static e_result e_platform_uninit(void)
{
    eglTerminate(e_gDisplayEGL);

    return E_SUCCESS;
}

static void* e_platform_get_object(e_platform_object_type type)
{
    switch (type)
    {
    case E_PLATFORM_OBJECT_EGL_DISPLAY: return (void*)e_gDisplayEGL;
    default: break;
    }

    return NULL;
}


struct e_platform_window
{
    e_window* pOwnerWindow;
};

static size_t e_platform_window_sizeof(void)
{
    return sizeof(e_platform_window);
}

static e_result e_platform_window_init_preallocated(const e_window_config* pConfig, e_window* pOwnerWindow, const e_allocation_callbacks* pAllocationCallbacks, e_platform_window* pWindow)
{
    E_ASSERT(pWindow != NULL);
    E_ASSERT(pConfig != NULL);
    E_UNUSED(pAllocationCallbacks);

    E_ZERO_OBJECT(pWindow);
    pWindow->pOwnerWindow = pOwnerWindow;

    /* We don't actually do anything here for Emscripten. All the real work is done in the graphics surface. */
    
    return E_SUCCESS;
}

static e_result e_platform_window_uninit(e_platform_window* pWindow, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pWindow == NULL) {
        return E_INVALID_ARGS;
    }

    E_UNUSED(pAllocationCallbacks);

    return E_SUCCESS;
}

static void* e_platform_window_get_object(const e_platform_window* pWindow, e_platform_object_type type)
{
    switch (type)
    {
    case E_PLATFORM_OBJECT_EGL_DISPLAY: return e_platform_get_object(type);
    default: break;
    }

    return NULL;
}

static e_result e_platform_window_set_size(e_platform_window* pWindow, unsigned int sizeX, unsigned int sizeY)
{
    if (pWindow == NULL) {
        return E_INVALID_ARGS;
    }

    /* Resizing the window programatically is not currently supported. */
    E_UNUSED(sizeX);
    E_UNUSED(sizeY);

    return E_INVALID_OPERATION;
}


static int e_gEmscriptenExitCode = 0;
static e_platform_main_loop_iteration_callback e_gMainLoopCallback;
static void* e_gMainLoopUserData;

static void e_platform_main_loop_iteration_emscripten()
{
    e_result result;

    E_UNUSED(time); /* We'll calculate the delta time some other way. */

    E_ASSERT(e_gMainLoopCallback != NULL);
    result = e_gMainLoopCallback(e_gMainLoopUserData);
    (void)result;   /* I still haven't figured out what to do with the result code. Should we use it to determine whether or not the loop should terminate. */
}

static e_result e_platform_main_loop(int* pExitCode, e_platform_main_loop_iteration_callback iterationCallback, void* pUserData)
{
    E_ASSERT(iterationCallback != NULL);

    e_gMainLoopCallback = iterationCallback;
    e_gMainLoopUserData = pUserData;

    emscripten_set_main_loop(e_platform_main_loop_iteration_emscripten, 0, EM_TRUE);

    E_ASSERT(pExitCode != NULL);
    *pExitCode = e_gEmscriptenExitCode;

    return E_SUCCESS;
}

static e_result e_platform_exit_main_loop(int exitCode)
{
    e_gEmscriptenExitCode = exitCode;
    emscripten_cancel_main_loop();

    return E_SUCCESS;
}
#endif  /* E_EMSCRIPTEN */



/* Default allocation callbacks. */
static void* e_malloc_default(size_t sz, void* pUserData)
{
    E_UNUSED(pUserData);
    return E_MALLOC(sz);
}

static void* e_realloc_default(void* p, size_t sz, void* pUserData)
{
    E_UNUSED(pUserData);
    return E_REALLOC(p, sz);
}

static void e_free_default(void* p, void* pUserData)
{
    E_UNUSED(pUserData);
    E_FREE(p);
}

static e_allocation_callbacks gDefaultAllocationCallbacks = 
{
    NULL,
    e_malloc_default,
    e_realloc_default,
    e_free_default
};


static e_allocation_callbacks e_allocation_callbacks_init_copy(const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pAllocationCallbacks != NULL) {
        return *pAllocationCallbacks;
    } else {
        e_allocation_callbacks callbacks;

        callbacks.pUserData = NULL;
        callbacks.onMalloc  = e_malloc_default;
        callbacks.onRealloc = e_realloc_default;
        callbacks.onFree    = e_free_default;

        return callbacks;
    }
}



E_API void* e_malloc(size_t sz, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pAllocationCallbacks != NULL) {
        if (pAllocationCallbacks->onMalloc != NULL) {
            return pAllocationCallbacks->onMalloc(sz, pAllocationCallbacks->pUserData);
        } else {
            return NULL;    /* Do not fall back to the default implementation. */
        }
    } else {
        return e_malloc_default(sz, NULL);
    }
}

E_API void* e_calloc(size_t sz, const e_allocation_callbacks* pAllocationCallbacks)
{
    void* p = e_malloc(sz, pAllocationCallbacks);
    if (p != NULL) {
        E_ZERO_MEMORY(p, sz);
    }

    return p;
}

E_API void* e_realloc(void* p, size_t sz, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pAllocationCallbacks != NULL) {
        if (pAllocationCallbacks->onRealloc != NULL) {
            return pAllocationCallbacks->onRealloc(p, sz, pAllocationCallbacks->pUserData);
        } else {
            return NULL;    /* Do not fall back to the default implementation. */
        }
    } else {
        return e_realloc_default(p, sz, NULL);
    }
}

E_API void e_free(void* p, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (p == NULL) {
        return;
    }

    if (pAllocationCallbacks != NULL) {
        if (pAllocationCallbacks->onFree != NULL) {
            pAllocationCallbacks->onFree(p, pAllocationCallbacks->pUserData);
        } else {
            return; /* Do no fall back to the default implementation. */
        }
    } else {
        e_free_default(p, NULL);
    }
}



#define E_ALIGNED_MALLOC_HEADER_SIZE    sizeof(void*) + sizeof(e_uintptr)

E_API void* e_aligned_malloc(size_t sz, size_t alignment, const e_allocation_callbacks* pAllocationCallbacks)
{
    size_t extraBytes;
    void* pUnaligned;
    void* pAligned;

    if (alignment == 0) {
        return 0;
    }

    extraBytes = alignment-1 + E_ALIGNED_MALLOC_HEADER_SIZE;

    pUnaligned = e_malloc(sz + extraBytes, pAllocationCallbacks);
    if (pUnaligned == NULL) {
        return NULL;
    }

    pAligned = (void*)(((e_uintptr)pUnaligned + extraBytes) & ~((e_uintptr)(alignment-1)));
    ((void**)pAligned)[-1] = pUnaligned;
    ((void**)pAligned)[-2] = (void*)sz;     /* For e_aligned_realloc(). */

    return pAligned;
}

/* e_aligned_realloc() must be called with a pointer that was created with either e_aligned_malloc() or e_aligned_realloc(). */
E_API void* e_aligned_realloc(void* p, size_t sz, size_t alignment, const e_allocation_callbacks* pAllocationCallbacks)
{
    void* pOldUnaligned;
    void* pOldAligned;
    void* pNewUnaligned;
    void* pNewAligned;
    size_t extraBytes;

    if (p == NULL) {
        return e_aligned_malloc(sz, alignment, pAllocationCallbacks);
    }
    
    if (alignment == 0) {
        return 0;
    }

    pOldAligned   = p;
    pOldUnaligned = ((void**)p)[-1];

    extraBytes = alignment-1 + E_ALIGNED_MALLOC_HEADER_SIZE;

    pNewUnaligned = e_realloc(pOldUnaligned, sz + extraBytes, pAllocationCallbacks);
    if (pNewUnaligned == NULL) {
        return NULL;
    }

    /* Getting here means the allocation is different. We'll need to move the memory down a bit to make it aligned. */
    pNewAligned = (void*)(((e_uintptr)pNewUnaligned + extraBytes) & ~((e_uintptr)(alignment-1)));
    ((void**)pNewAligned)[-1] = pNewUnaligned;
    ((void**)pNewAligned)[-2] = (void*)sz;

    /* Move the memory down so it starts at the aligned position. No need to do this if the old and new pointers are the same. */
    if (pNewUnaligned != pOldUnaligned) {
        void* pDst = pNewAligned;
        void* pSrc = (unsigned char*)pNewUnaligned + E_ALIGNED_MALLOC_HEADER_SIZE + ((e_uintptr)pOldAligned - ((e_uintptr)pOldUnaligned + E_ALIGNED_MALLOC_HEADER_SIZE));
        E_MOVE_MEMORY(pDst, pSrc, sz);
    }

    return pNewAligned;
}

E_API void e_aligned_free(void* p, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (p == NULL) {
        return;
    }

    e_free(((void**)p)[-1], pAllocationCallbacks);
}




static e_result e_result_from_c89thread(int result)
{
    switch (result)
    {
    case c89thrd_success:  return E_SUCCESS;
    case c89thrd_nomem:    return E_OUT_OF_MEMORY;
    case c89thrd_timedout: return E_TIMEOUT;
    case c89thrd_busy:     return E_BUSY;
    default: break;
    }

    /* Fall back to a generic error. */
    return E_ERROR;
}

static void* e_c89thread_allocation_callbacks_malloc(size_t sz, void* pUserData)
{
    return e_malloc(sz, (const e_allocation_callbacks*)pUserData);
}

static void* e_c89thread_allocation_callbacks_realloc(void* p, size_t sz, void* pUserData)
{
    return e_realloc(p, sz, (const e_allocation_callbacks*)pUserData);
}

static void e_c89thread_allocation_callbacks_free(void* p, void* pUserData)
{
    e_free(p, (const e_allocation_callbacks*)pUserData);
}

static c89thread_allocation_callbacks c_c89thread_allocation_callbacks_init(const e_allocation_callbacks* pAllocationCallbacks)
{
    c89thread_allocation_callbacks c89threadAllocationCallbacks;

    c89threadAllocationCallbacks.pUserData = (void*)pAllocationCallbacks;
    c89threadAllocationCallbacks.onMalloc  = e_c89thread_allocation_callbacks_malloc;
    c89threadAllocationCallbacks.onRealloc = e_c89thread_allocation_callbacks_realloc;
    c89threadAllocationCallbacks.onFree    = e_c89thread_allocation_callbacks_free;

    return c89threadAllocationCallbacks;
}



/* ==== BEG e_threading.c ==== */
E_API e_thread_config e_thread_config_init(e_thread_start_callback func, void* arg)
{
    e_thread_config config;

    E_ZERO_OBJECT(&config);
    config.func = func;
    config.arg  = arg;

    return config;
}

E_API e_result e_thread_init(const e_thread_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_thread** ppThread)
{
    e_result result;
    e_thread* pThread;
    c89thread_allocation_callbacks c89threadAllocationCallbacks = c_c89thread_allocation_callbacks_init(pAllocationCallbacks);

    if (ppThread == NULL) {
        return E_INVALID_ARGS;
    }

    *ppThread = NULL;

    if (pConfig == NULL) {
        return E_INVALID_ARGS;
    }

    pThread = (e_thread*)e_malloc(sizeof(c89thrd_t), pAllocationCallbacks);
    if (pThread == NULL) {
        return E_OUT_OF_MEMORY;
    }

    result = e_result_from_c89thread(c89thrd_create_ex((c89thrd_t*)pThread, pConfig->func, pConfig->arg, NULL, &c89threadAllocationCallbacks));
    if (result != E_SUCCESS) {
        e_free(pThread, pAllocationCallbacks);
        return result;
    }

    *ppThread = pThread;
    return E_SUCCESS;
}

E_API e_result e_thread_join(e_thread* pThread, int* pExitCode)
{
    if (pThread == NULL) {
        return E_INVALID_ARGS;
    }

    return e_result_from_c89thread(c89thrd_join(*(c89thrd_t*)pThread, pExitCode));
}



E_API e_result e_mutex_init(const e_allocation_callbacks* pAllocationCallbacks, e_mutex** ppMutex)
{
    e_result result;
    e_mutex* pMutex;

    if (ppMutex == NULL) {
        return E_INVALID_ARGS;
    }

    *ppMutex = NULL;

    pMutex = (e_mutex*)e_malloc(sizeof(c89mtx_t), pAllocationCallbacks);
    if (pMutex == NULL) {
        return E_OUT_OF_MEMORY;
    }

    result = e_result_from_c89thread(c89mtx_init((c89mtx_t*)pMutex, c89mtx_plain));
    if (result != E_SUCCESS) {
        e_free(pMutex, pAllocationCallbacks);
        return result;
    }

    *ppMutex = pMutex;
    return E_SUCCESS;
}

E_API void e_mutex_uninit(e_mutex* pMutex, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pMutex == NULL) {
        return;
    }

    c89mtx_destroy((c89mtx_t*)pMutex);
    e_free(pMutex, pAllocationCallbacks);
}

E_API void e_mutex_lock(e_mutex* pMutex)
{
    c89mtx_lock((c89mtx_t*)pMutex);
}

E_API void e_mutex_unlock(e_mutex* pMutex)
{
    c89mtx_unlock((c89mtx_t*)pMutex);
}
/* ==== END e_threading.c ==== */



/* ==== BEG e_stream.c ==== */
E_API e_result e_stream_init(const e_stream_vtable* pVTable, void* pVTableUserData, e_stream* pStream)
{
    if (pStream == NULL) {
        return E_INVALID_ARGS;
    }

    E_ZERO_OBJECT(pStream);

    if (pVTable == NULL) {
        return E_INVALID_ARGS;
    }

    pStream->pVTable         = pVTable;
    pStream->pVTableUserData = pVTableUserData;

    return E_SUCCESS;
}

E_API e_result e_stream_read(e_stream* pStream, void* pDst, size_t bytesToRead, size_t* pBytesRead)
{
    e_result result;
    size_t bytesRead;

    if (pBytesRead != NULL) {
        *pBytesRead = 0;
    }

    if (pStream == NULL || pDst == NULL) {
        return E_INVALID_ARGS;
    }

    E_ASSERT(pStream->pVTable != NULL);

    if (pStream->pVTable->read == NULL) {
        return E_NOT_IMPLEMENTED;
    }

    result = pStream->pVTable->read(pStream->pVTableUserData, pDst, bytesToRead, &bytesRead);
    if (result != E_SUCCESS) {
        return result;
    }

    /*
    If the user did not specify an output variable for the number of bytes read it must mean they
    are expecting the exact number of bytes requested, because otherwise how would they know how
    to handle the case where less bytes are available than expected? Therefore, when NULL is passed
    in for this parameter we're going to return an error if we were unable to read the specified
    number of bytes.
    */
    if (pBytesRead == NULL) {
        if (bytesRead != bytesToRead) {
            return E_ERROR;
        }
    }

    if (pBytesRead != NULL) {
        *pBytesRead = bytesRead;
    }

    return E_SUCCESS;
}

E_API e_result e_stream_write(e_stream* pStream, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten)
{
    e_result result;
    size_t bytesWritten;

    if (pBytesWritten != NULL) {
        *pBytesWritten = 0;
    }

    if (pStream == NULL || pSrc == NULL) {
        return E_INVALID_ARGS;
    }

    E_ASSERT(pStream->pVTable != NULL);

    if (pStream->pVTable->write == NULL) {
        return E_NOT_IMPLEMENTED;
    }

    result = pStream->pVTable->write(pStream->pVTableUserData, pSrc, bytesToWrite, &bytesWritten);
    if (result != E_SUCCESS) {
        return result;
    }

    /*
    As with reading, if the caller did not specify an output value for the number of bytes written
    we must assume it's all or nothing and return an error if the number of bytes written does not
    match the number of bytes requested to be written.
    */
    if (pBytesWritten == NULL) {
        if (bytesWritten != bytesToWrite) {
            return E_ERROR;
        }
    }

    if (pBytesWritten != NULL) {
        *pBytesWritten = bytesWritten;
    }

    return E_SUCCESS;
}

E_API e_result e_stream_seek(e_stream* pStream, e_int64 offset, e_seek_origin origin)
{
    if (pStream == NULL) {
        return E_INVALID_ARGS;
    }

    E_ASSERT(pStream->pVTable != NULL);

    if (pStream->pVTable->seek == NULL) {
        return E_NOT_IMPLEMENTED;
    }

    return pStream->pVTable->seek(pStream->pVTableUserData, offset, origin);
}

E_API e_result e_stream_tell(e_stream* pStream, e_int64* pCursor)
{
    if (pCursor == NULL) {
        return E_INVALID_ARGS;
    }

    *pCursor = 0;

    if (pStream == NULL) {
        return E_INVALID_ARGS;
    }

    E_ASSERT(pStream->pVTable != NULL);

    if (pStream->pVTable->tell == NULL) {
        return E_NOT_IMPLEMENTED;
    }

    return pStream->pVTable->tell(pStream->pVTableUserData, pCursor);
}



static e_result e_memory_stream_read_callback(void* pUserData, void* pDst, size_t bytesToRead, size_t* pBytesRead)
{
    return e_memory_stream_read((e_memory_stream*)pUserData, pDst, bytesToRead, pBytesRead);
}

static e_result e_memory_stream_write_callback(void* pUserData, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten)
{
    return e_memory_stream_write((e_memory_stream*)pUserData, pSrc, bytesToWrite, pBytesWritten);
}

static e_result e_memory_stream_seek_callback(void* pUserData, e_int64 offset, e_seek_origin origin)
{
    return e_memory_stream_seek((e_memory_stream*)pUserData, offset, origin);
}

static e_result e_memory_stream_tell_callback(void* pUserData, e_int64* pCursor)
{
    e_result result;
    size_t cursor;

    result = e_memory_stream_tell((e_memory_stream*)pUserData, &cursor);
    if (result != E_SUCCESS) {
        return result;
    }

    if (cursor > INT64_MAX) {    /* <-- INT64_MAX may not be defined on some compilers. Need to check this. Can easily define this ourselves. */
        return E_ERROR;
    }

    *pCursor = (e_int64)cursor;

    return E_SUCCESS;
}

static e_stream_vtable e_gMemoryStreamVTable =
{
    e_memory_stream_read_callback,
    e_memory_stream_write_callback,
    e_memory_stream_seek_callback,
    e_memory_stream_tell_callback
};

E_API e_result e_memory_stream_init_write(void** ppData, size_t* pDataSize, const e_allocation_callbacks* pAllocationCallbacks, e_memory_stream* pStream)
{
    e_result result;

    if (pStream == NULL) {
        return E_INVALID_ARGS;
    }

    E_ZERO_OBJECT(pStream);

    if (ppData == NULL || pDataSize == NULL) {
        return E_INVALID_ARGS;
    }

    result = e_stream_init(&e_gMemoryStreamVTable, pStream, &pStream->base);
    if (result != E_SUCCESS) {
        return result;
    }

    pStream->ppData     = ppData;
    pStream->pDataSize  = pDataSize;
    pStream->cursor     = 0;
    pStream->dataCap    = 0;
    pStream->allocationCallbacks = e_allocation_callbacks_init_copy(pAllocationCallbacks);

    *pStream->ppData    = NULL;
    *pStream->pDataSize = 0;

    return E_SUCCESS;
}

E_API e_result e_memory_stream_init_readonly(const void* pData, size_t dataSize, e_memory_stream* pStream)
{
    e_result result;

    if (pStream == NULL) {
        return E_INVALID_ARGS;
    }

    E_ZERO_OBJECT(pStream);

    if (pData == NULL) {
        return E_INVALID_ARGS;
    }

    result = e_stream_init(&e_gMemoryStreamVTable, pStream, &pStream->base);
    if (result != E_SUCCESS) {
        return result;
    }

    pStream->readonly.pData    = pData;
    pStream->readonly.dataSize = dataSize;

    pStream->ppData    = (void**)&pStream->readonly.pData;
    pStream->pDataSize = &pStream->readonly.dataSize;
    pStream->cursor    = 0;
    pStream->dataCap   = 0;

    return E_SUCCESS;
}

E_API e_result e_memory_stream_read(e_memory_stream* pStream, void* pDst, size_t bytesToRead, size_t* pBytesRead)
{
    size_t bytesAvailable;
    size_t bytesRead;

    if (pBytesRead != NULL) {
        *pBytesRead = 0;
    }

    if (pStream == NULL) {
        return E_INVALID_ARGS;
    }

    E_ASSERT(pStream->cursor <= *pStream->pDataSize); /* If this is triggered it means there a bug in the stream reader. The cursor has gone beyong the end of the buffer. */

    bytesAvailable = *pStream->pDataSize - pStream->cursor;
    if (bytesAvailable == 0) {
        return E_AT_END;    /* Must return E_AT_END if we're sitting at the end of the file, even when bytesToRead is 0. */
    }

    bytesRead = E_MIN(bytesAvailable, bytesToRead);

    /* The destination can be null in which case this acts as a seek. */
    if (pDst != NULL) {
        E_COPY_MEMORY(pDst, E_OFFSET_PTR(*pStream->ppData, pStream->cursor), bytesRead);
    }

    pStream->cursor += bytesRead;
    
    if (pBytesRead != NULL) {
        *pBytesRead = bytesRead;
    }

    return E_SUCCESS;
}

E_API e_result e_memory_stream_write(e_memory_stream* pStream, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten)
{
    size_t newSize;

    if (pBytesWritten != NULL) {
        *pBytesWritten = 0;
    }

    if (pStream == NULL || pSrc == NULL) {
        return E_INVALID_ARGS;
    }

    /* Cannot write in read-only mode. */
    if (pStream->readonly.pData != NULL) {
        return E_INVALID_OPERATION;
    }

    newSize = *pStream->pDataSize + bytesToWrite;
    if (newSize > pStream->dataCap) {
        /* Need to resize. */
        void* pNewBuffer;
        size_t newCap;

        newCap = E_MAX(newSize, pStream->dataCap * 2);
        pNewBuffer = e_realloc(*pStream->ppData, newCap, &pStream->allocationCallbacks);
        if (pNewBuffer == NULL) {
            return E_OUT_OF_MEMORY;
        }

        *pStream->ppData = pNewBuffer;
        pStream->dataCap = newCap;
    }

    E_ASSERT(newSize <= pStream->dataCap);

    E_COPY_MEMORY(E_OFFSET_PTR(*pStream->ppData, *pStream->pDataSize), pSrc, bytesToWrite);
    *pStream->pDataSize = newSize;

    if (pBytesWritten != NULL) {
        *pBytesWritten = bytesToWrite;  /* We always write all or nothing here. */
    }

    return E_SUCCESS;
}

E_API e_result e_memory_stream_seek(e_memory_stream* pStream, e_int64 offset, e_seek_origin origin)
{
    if (pStream == NULL) {
        return E_INVALID_ARGS;
    }

    if (E_ABS(offset) > E_SIZE_MAX) {
        return E_INVALID_ARGS;  /* Trying to seek too far. This will never happen on 64-bit builds. */
    }

    /*
    The seek binary - it works or it doesn't. There's no clamping to the end or anything like that. The
    seek point is either valid or invalid.
    */
    if (origin == E_SEEK_ORIGIN_CURRENT) {
        if (offset > 0) {
            /* Moving forward. */
            size_t bytesRemaining = *pStream->pDataSize - pStream->cursor;
            if (bytesRemaining < (size_t)offset) {
                return E_BAD_SEEK;  /* Trying to seek beyond the end of the buffer. */
            }

            pStream->cursor += (size_t)offset;
        } else {
            /* Moving backwards. */
            size_t absoluteOffset = E_ABS(offset);
            if (absoluteOffset > pStream->cursor) {
                return E_BAD_SEEK;  /* Trying to seek prior to the start of the buffer. */
            }

            pStream->cursor -= absoluteOffset;
        }
    } else if (origin == E_SEEK_ORIGIN_START) {
        if (offset < 0) {
            return E_BAD_SEEK;  /* Trying to seek prior to the start of the buffer.. */
        }

        if ((size_t)offset > *pStream->pDataSize) {
            return E_BAD_SEEK;
        }

        pStream->cursor = (size_t)offset;
    } else if (origin == E_SEEK_ORIGIN_END) {
        if (offset > 0) {
            return E_BAD_SEEK;  /* Trying to seek beyond the end of the buffer. */
        }

        if ((size_t)E_ABS(offset) > *pStream->pDataSize) {
            return E_BAD_SEEK;
        }

        pStream->cursor = *pStream->pDataSize - (size_t)E_ABS(offset);
    } else {
        return E_INVALID_ARGS;
    }

    return E_SUCCESS;
}

E_API e_result e_memory_stream_tell(e_memory_stream* pStream, size_t* pCursor)
{
    if (pCursor == NULL) {
        return E_INVALID_ARGS;
    }

    *pCursor = 0;

    if (pStream == NULL) {
        return E_INVALID_ARGS;
    }

    *pCursor = pStream->cursor;

    return E_SUCCESS;
}

E_API e_result e_memory_stream_remove(e_memory_stream* pStream, size_t offset, size_t size)
{
    void* pDst;
    void* pSrc;
    size_t tailSize;

    if (pStream == NULL) {
        return E_INVALID_ARGS;
    }

    if ((offset + size) > *pStream->pDataSize) {
        return E_INVALID_ARGS;
    }

    /* The cursor needs to be moved. */
    if (pStream->cursor > offset) {
        if (pStream->cursor >= (offset + size)) {
            pStream->cursor -= size;
        } else {
            pStream->cursor  = offset;
        }
    }

    pDst = E_OFFSET_PTR(*pStream->ppData, offset);
    pSrc = E_OFFSET_PTR(*pStream->ppData, offset + size);
    tailSize = *pStream->pDataSize - (offset + size);

    E_MOVE_MEMORY(pDst, pSrc, tailSize);
    *pStream->pDataSize -= size;

    return E_SUCCESS;
}

E_API e_result e_memory_stream_truncate(e_memory_stream* pStream)
{
    if (pStream == NULL) {
        return E_INVALID_ARGS;
    }

    return e_memory_stream_remove(pStream, pStream->cursor, (*pStream->pDataSize - pStream->cursor));
}
/* ==== END e_stream.c ==== */



/* ==== BEG e_fs.c ==== */
#include <sys/types.h>
#include <sys/stat.h>

typedef struct
{
    e_file base;
    FILE* pFILE;
} e_file_default;


static e_result e_fopen(FILE** ppFile, const char* pFilePath, const char* pOpenMode)
{
#if defined(_MSC_VER) && _MSC_VER >= 1400
    errno_t err;
#endif

    if (ppFile != NULL) {
        *ppFile = NULL;  /* Safety. */
    }

    if (pFilePath == NULL || pOpenMode == NULL || ppFile == NULL) {
        return E_INVALID_ARGS;
    }

#if defined(_MSC_VER) && _MSC_VER >= 1400
    err = fopen_s(ppFile, pFilePath, pOpenMode);
    if (err != 0) {
        return e_result_from_errno(err);
    }
#else
#if defined(_WIN32) || defined(__APPLE__)
    *ppFile = fopen(pFilePath, pOpenMode);
#else
    #if defined(_FILE_OFFSET_BITS) && _FILE_OFFSET_BITS == 64 && defined(_LARGEFILE64_SOURCE)
        *ppFile = fopen64(pFilePath, pOpenMode);
    #else
        *ppFile = fopen(pFilePath, pOpenMode);
    #endif
#endif
    if (*ppFile == NULL) {
        e_result result = e_result_from_errno(errno);
        if (result == E_SUCCESS) {
            result = E_ERROR;   /* Just a safety check to make sure we never ever return success when pFile == NULL. */
        }

        return result;
    }
#endif

    return E_SUCCESS;
}


static e_result e_fs_alloc_size_default(void* pUserData, size_t* pSize)
{
    E_ASSERT(pSize != NULL);
    E_UNUSED(pUserData);

    *pSize = sizeof(e_file_default);

    return E_SUCCESS;
}

static e_result e_fs_open_default(void* pUserData, e_fs* pFS, const char* pFilePath, e_open_mode openMode, const e_allocation_callbacks* pAllocationCallbacks, e_file* pFile)
{
    e_file_default* pFileDefault = (e_file_default*)pFile;
    e_result result;
    char openModeStr[4];

    E_ASSERT(pFile != NULL);
    E_UNUSED(pUserData);
    E_UNUSED(pFS);
    E_UNUSED(pAllocationCallbacks);

    if ((openMode & E_OPEN_MODE_WRITE) != 0) {
        if ((openMode & E_OPEN_MODE_READ) != 0) {
            /* Read and write. */
            if ((openMode & E_OPEN_MODE_APPEND) != 0) {
                c89str_strcpy_s(openModeStr, sizeof(openModeStr), "a+b");   /* Read-and-write, appending. */
            } else if ((openMode & E_OPEN_MODE_TRUNCATE) != 0) {
                c89str_strcpy_s(openModeStr, sizeof(openModeStr), "w+b");   /* Read-and-write, truncating. */
            } else {
                c89str_strcpy_s(openModeStr, sizeof(openModeStr), "r+b");   /* Read-and-write, overwriting. */
            }
        } else {
            /* Write-only. */
            if ((openMode & E_OPEN_MODE_APPEND) != 0) {
                c89str_strcpy_s(openModeStr, sizeof(openModeStr), "ab");    /* Write-only, appending. */
            } else if ((openMode & E_OPEN_MODE_TRUNCATE) != 0) {
                c89str_strcpy_s(openModeStr, sizeof(openModeStr), "wb");    /* Write-only, truncating. */
            } else {
                c89str_strcpy_s(openModeStr, sizeof(openModeStr), "r+b");   /* Write-only, overwriting. Need to use the "+" option here because there does not appear to be an option for a write-only overwrite mode. */
            }
        }
    } else {
        if ((openMode & E_OPEN_MODE_READ) != 0) {
            c89str_strcpy_s(openModeStr, sizeof(openModeStr), "rb");    /* Read-only. */
        } else {
            return E_INVALID_ARGS;
        }
    }

    result = e_fopen(&pFileDefault->pFILE, pFilePath, openModeStr);
    if (result != E_SUCCESS) {
        return result;
    }

    return E_SUCCESS;
}

static void e_fs_close_default(void* pUserData, e_file* pFile, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_file_default* pFileDefault = (e_file_default*)pFile;

    E_ASSERT(pFile != NULL);
    E_UNUSED(pUserData);
    E_UNUSED(pAllocationCallbacks);

    fclose(pFileDefault->pFILE);
}

static e_result e_fs_read_default(void* pUserData, e_file* pFile, void* pDst, size_t bytesToRead, size_t* pBytesRead)
{
    e_file_default* pFileDefault = (e_file_default*)pFile;
    size_t result;

    E_ASSERT(pFile != NULL);
    E_ASSERT(pDst != NULL);
    E_ASSERT(pBytesRead != NULL);
    E_UNUSED(pUserData);

    result = fread(pDst, 1, bytesToRead, pFileDefault->pFILE);

    *pBytesRead = result;

    if (result != bytesToRead) {
        if (result == 0 && feof(pFileDefault->pFILE)) {
            return E_AT_END;
        } else {
            return e_result_from_errno(ferror(pFileDefault->pFILE));
        }
    }

    return E_SUCCESS;
}

static e_result e_fs_write_default(void* pUserData, e_file* pFile, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten)
{
    e_file_default* pFileDefault = (e_file_default*)pFile;
    size_t result;

    E_ASSERT(pFile != NULL);
    E_UNUSED(pUserData);

    result = fwrite(pSrc, 1, bytesToWrite, pFileDefault->pFILE);

    *pBytesWritten = result;

    if (result != bytesToWrite) {
        return e_result_from_errno(ferror(pFileDefault->pFILE));
    }

    return E_SUCCESS;
}

static e_result e_fs_seek_default(void* pUserData, e_file* pFile, e_int64 offset, e_seek_origin origin)
{
    e_file_default* pFileDefault = (e_file_default*)pFile;
    int result;
    int whence;

    E_ASSERT(pFile != NULL);
    E_UNUSED(pUserData);

    if (origin == E_SEEK_ORIGIN_START) {
        whence = SEEK_SET;
    } else if (origin == E_SEEK_ORIGIN_END) {
        whence = SEEK_END;
    } else {
        whence = SEEK_CUR;
    }

#if defined(_WIN32)
    #if defined(_MSC_VER) && _MSC_VER > 1200
        result = _fseeki64(pFileDefault->pFILE, offset, whence);
    #else
        /* No _fseeki64() so restrict to 31 bits. */
        if (origin > 0x7FFFFFFF) {
            return E_OUT_OF_RANGE;
        }

        result = fseek(pFileDefault->pFILE, (int)offset, whence);
    #endif
#else
    result = fseek(pFileDefault->pFILE, (long int)offset, whence);
#endif
    if (result != 0) {
        return E_ERROR;
    }

    return E_SUCCESS;
}

static e_result e_fs_tell_default(void* pUserData, e_file* pFile, e_int64* pCursor)
{
    e_file_default* pFileDefault = (e_file_default*)pFile;
    e_int64 result;

    E_ASSERT(pFile != NULL);
    E_ASSERT(pCursor != NULL);
    E_UNUSED(pUserData);

#if defined(_WIN32)
    #if defined(_MSC_VER) && _MSC_VER > 1200
        result = _ftelli64(pFileDefault->pFILE);
    #else
        result = ftell(pFileDefault->pFILE);
    #endif
#else
    result = ftell(pFileDefault->pFILE);
#endif

    *pCursor = result;

    return E_SUCCESS;
}

static e_result e_fs_flush_default(void* pUserData, e_file* pFile)
{
    e_file_default* pFileDefault = (e_file_default*)pFile;

    E_ASSERT(pFile != NULL);
    E_UNUSED(pUserData);

    if (fflush(pFileDefault->pFILE) != 0) {
        return e_result_from_errno(ferror(pFileDefault->pFILE));
    }

    return E_SUCCESS;
}


#if !defined(_MSC_VER) && !((defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 1) || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)) && !defined(E_BSD)
int fileno(FILE *stream);
#endif

static e_result e_fs_info_default(void* pUserData, e_file* pFile, e_file_info* pInfo)
{
    e_file_default* pFileDefault = (e_file_default*)pFile;
    int fd;
    struct stat info;

    E_ASSERT(pFile != NULL);
    E_UNUSED(pUserData);

#if defined(_MSC_VER)
    fd = _fileno(pFileDefault->pFILE);
#else
    fd =  fileno(pFileDefault->pFILE);
#endif

    if (fstat(fd, &info) != 0) {
        return e_result_from_errno(errno);
    }

    pInfo->size = info.st_size;

    return E_SUCCESS;
}

static e_fs_vtable e_gDefaultFSVTable =
{
    e_fs_alloc_size_default,
    e_fs_open_default,
    e_fs_close_default,
    e_fs_read_default,
    e_fs_write_default,
    e_fs_seek_default,
    e_fs_tell_default,
    e_fs_flush_default,
    e_fs_info_default
};


static e_result e_file_stream_read(void* pUserData, void* pDst, size_t bytesToRead, size_t* pBytesRead)
{
    return e_fs_read((e_file*)pUserData, pDst, bytesToRead, pBytesRead);
}

static e_result e_file_stream_write(void* pUserData, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten)
{
    return e_fs_write((e_file*)pUserData, pSrc, bytesToWrite, pBytesWritten);
}

static e_result e_file_stream_seek(void* pUserData, e_int64 offset, e_seek_origin origin)
{
    return e_fs_seek((e_file*)pUserData, offset, origin);
}

static e_result e_file_stream_tell(void* pUserData, e_int64* pCursor)
{
    return e_fs_tell((e_file*)pUserData, pCursor);
}

static e_stream_vtable e_gFileStreamVTable =
{
    e_file_stream_read,
    e_file_stream_write,
    e_file_stream_seek,
    e_file_stream_tell
};


E_API e_fs_config e_fs_config_init(const e_fs_vtable* pVTable, void* pVTableUserData)
{
    e_fs_config config;

    E_ZERO_OBJECT(&config);
    config.pVTable         = pVTable;
    config.pVTableUserData = pVTableUserData;

    return config;
}


E_API e_result e_fs_init_preallocated(const e_fs_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_fs* pFS)
{
    e_fs_config config;

    /* Not using allocation callbacks right now, but leaving here in case it's needed later. */
    E_UNUSED(pAllocationCallbacks);

    if (pFS == NULL) {
        return E_INVALID_ARGS;
    }

    E_ZERO_OBJECT(pFS);

    if (pConfig != NULL) {
        config = *pConfig;
        pConfig = NULL;
    } else {
        config = e_fs_config_init(NULL, NULL);
    }

    /* If we weren't given a vtable we need to use the default. Make sure the vtable is always set. */
    if (config.pVTable == NULL) {
        config.pVTable = &e_gDefaultFSVTable;
        config.pVTableUserData = NULL;
    }

    pFS->pVTable = config.pVTable;
    pFS->pVTableUserData = config.pVTableUserData;

    /* Must always have a vtable. */
    E_ASSERT(pFS->pVTable != NULL);

    return E_SUCCESS;
}

E_API e_result e_fs_init(const e_fs_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_fs** ppFS)
{
    e_result result;
    e_fs* pFS;

    if (ppFS == NULL) {
        return E_INVALID_ARGS;
    }

    *ppFS = NULL;

    pFS = (e_fs*)e_malloc(sizeof(*pFS), pAllocationCallbacks);
    if (pFS == NULL) {
        return E_OUT_OF_MEMORY;
    }

    result = e_fs_init_preallocated(pConfig, pAllocationCallbacks, pFS);
    if (result != E_SUCCESS) {
        e_free(pFS, pAllocationCallbacks);
        return result;
    }

    pFS->freeOnUninit = E_TRUE;

    *ppFS = pFS;
    return E_SUCCESS;
}

E_API void e_fs_uninit(e_fs* pFS, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pFS == NULL) {
        return;
    }

    if (pFS->freeOnUninit) {
        e_free(pFS, pAllocationCallbacks);
    }
}

E_API e_result e_fs_open(e_fs* pFS, const char* pFilePath, e_open_mode openMode, const e_allocation_callbacks* pAllocationCallbacks, e_file** ppFile)
{
    e_result result;
    size_t allocSize;
    e_file* pFile;
    const e_fs_vtable* pVTable;
    void* pVTableUserData;

    if (ppFile == NULL) {
        return E_INVALID_ARGS;
    }

    *ppFile = NULL; /* For safety in case an error occurs. Want to make sure the caller doesn't try calling into an invalid file handle. */

    if (c89str_is_null_or_empty(pFilePath)) {
        return E_INVALID_ARGS;
    }

    /* Some quick open mode validation. */
    if (openMode == 0) {
        return E_INVALID_ARGS;  /* No open mode specified. */
    }
    if ((openMode & E_OPEN_MODE_READ) == 0 && (openMode & E_OPEN_MODE_WRITE) == 0) {
        return E_INVALID_ARGS;  /* Must specify at least one of READ or WRITE. */
    }
    if ((openMode & E_OPEN_MODE_APPEND) != 0 && (openMode & E_OPEN_MODE_TRUNCATE) != 0) {
        return E_INVALID_ARGS;  /* Both APPEND and TRUNCATE have been specified which does not make sense. */
    }

    /* The file system can be null in which case the default is used and will work just like normal fopen(). */
    if (pFS == NULL) {
        pVTable = &e_gDefaultFSVTable;
        pVTableUserData = NULL;
    } else {
        pVTable = pFS->pVTable;
        pVTableUserData = pFS->pVTableUserData;
    }

    /* We should be able to assume that we always have a vtable at this point. */
    E_ASSERT(pVTable != NULL);

    result = pVTable->file_alloc_size(pVTableUserData, &allocSize);
    if (result != E_SUCCESS) {
        return result;  /* Failed to retrieve the size fo the e_file allocation. */
    }

    pFile = (e_file*)e_calloc(allocSize, pAllocationCallbacks);
    if (pFile == NULL) {
        return E_OUT_OF_MEMORY;
    }

    result = pVTable->open(pVTableUserData, pFS, pFilePath, openMode, pAllocationCallbacks, pFile);
    if (result != E_SUCCESS) {
        e_free(pFile, pAllocationCallbacks);
        return result;
    }

    /* We need to make sure the file is given the vtable that was used to initialize it. This is because pFS is allowed to be null. */
    pFile->pVTable = pVTable;
    pFile->pVTableUserData = pVTableUserData;

    /* Files are streams which means they can be plugged into anything that takes a e_stream pointer. We need to get this set up now. */
    result = e_stream_init(&e_gFileStreamVTable, pFile, &pFile->stream);
    if (result != E_SUCCESS) {
        e_fs_close(pFile, pAllocationCallbacks);
        return result;
    }

    *ppFile = pFile;

    return E_SUCCESS;
}

E_API void e_fs_close(e_file* pFile, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pFile == NULL) {
        return;
    }

    E_ASSERT(pFile->pVTable != NULL);
    pFile->pVTable->close(pFile->pVTableUserData, pFile, pAllocationCallbacks);

    e_free(pFile, pAllocationCallbacks);
}

E_API e_result e_fs_read(e_file* pFile, void* pDst, size_t bytesToRead, size_t* pBytesRead)
{
    e_result result;
    size_t bytesRead;

    if (pFile == NULL) {
        return E_INVALID_ARGS;
    }

    E_ASSERT(pFile->pVTable != NULL);
    result = pFile->pVTable->read(pFile->pVTableUserData, pFile, pDst, bytesToRead, &bytesRead);
    if (result != E_SUCCESS) {
        return result;
    }

    if (pBytesRead != NULL) {
        *pBytesRead = bytesRead;
    }

    return E_SUCCESS;
}

E_API e_result e_fs_write(e_file* pFile, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten)
{
    e_result result;
    size_t bytesWritten;

    if (pFile == NULL) {
        return E_INVALID_ARGS;
    }

    E_ASSERT(pFile->pVTable != NULL);
    result = pFile->pVTable->write(pFile->pVTableUserData, pFile, pSrc, bytesToWrite, &bytesWritten);
    if (result != E_SUCCESS) {
        return result;
    }

    if (pBytesWritten != NULL) {
        *pBytesWritten = bytesWritten;
    }

    return E_SUCCESS;
}

E_API e_result e_fs_seek(e_file* pFile, e_int64 offset, e_seek_origin origin)
{
    e_result result;
    
    if (pFile == NULL) {
        return E_INVALID_ARGS;
    }

    E_ASSERT(pFile->pVTable != NULL);
    result = pFile->pVTable->seek(pFile->pVTableUserData, pFile, offset, origin);
    if (result != E_SUCCESS) {
        return result;
    }

    return result;
}

E_API e_result e_fs_tell(e_file* pFile, e_int64* pCursor)
{
    e_result result;

    if (pCursor == NULL) {
        return E_INVALID_ARGS;
    }

    *pCursor = 0;

    if (pFile == NULL) {
        return E_INVALID_ARGS;
    }

    E_ASSERT(pFile->pVTable != NULL);
    result = pFile->pVTable->tell(pFile->pVTableUserData, pFile, pCursor);
    if (result != E_SUCCESS) {
        return result;
    }

    return E_SUCCESS;
}

E_API e_result e_fs_flush(e_file* pFile)
{
    e_result result;

    if (pFile == NULL) {
        return E_INVALID_ARGS;
    }

    E_ASSERT(pFile->pVTable != NULL);
    result = pFile->pVTable->flush(pFile->pVTableUserData, pFile);
    if (result != E_SUCCESS) {
        return result;
    }

    return E_SUCCESS;
}

E_API e_result e_fs_info(e_file* pFile, e_file_info* pInfo)
{
    e_result result;

    if (pInfo == NULL) {
        return E_INVALID_ARGS;
    }

    E_ZERO_OBJECT(pInfo);

    if (pFile == NULL) {
        return E_INVALID_ARGS;
    }

    E_ASSERT(pFile->pVTable != NULL);
    result = pFile->pVTable->info(pFile->pVTableUserData, pFile, pInfo);
    if (result != E_SUCCESS) {
        return result;
    }

    return E_SUCCESS;
}

E_API e_stream* e_fs_file_stream(e_file* pFile)
{
    return &pFile->stream;
}

E_API e_fs* e_fs_get(e_file* pFile)
{
    return pFile->pFS;
}

static e_result e_fs_open_and_read_with_extra_byte(e_fs* pFS, const char* pFilePath, void** ppData, size_t* pSize, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_result result;
    e_file* pFile;
    e_file_info info;
    void* pData;
    size_t bytesRead;

    if (ppData == NULL) {
        return E_INVALID_ARGS;
    }

    *ppData = NULL;

    result = e_fs_open(pFS, pFilePath, E_OPEN_MODE_READ, pAllocationCallbacks, &pFile);
    if (result != E_SUCCESS) {
        return result;
    }

    result = e_fs_info(pFile, &info);
    if (result != E_SUCCESS) {
        return result;
    }

    if (info.size > E_SIZE_MAX) {
        e_fs_close(pFile, pAllocationCallbacks);
        return E_OUT_OF_MEMORY; /* The file is too big to be loaded fully into memory. */
    }

    pData = e_malloc(info.size + 1, pAllocationCallbacks);
    if (pData == NULL) {
        e_fs_close(pFile, pAllocationCallbacks);
        return E_OUT_OF_MEMORY;
    }

    result = e_fs_read(pFile, pData, info.size, &bytesRead);
    e_fs_close(pFile, pAllocationCallbacks);

    if (result != E_SUCCESS) {  /* <-- This is checking the result of e_fs_read(). */
        e_free(pData, pAllocationCallbacks);
        return result;
    }

    /* The extra byte needs to be cleared to zero for safety and consistent output. It also allows use to use this for null terminating text files. */
    ((char*)pData)[bytesRead] = '\0';

    if (pSize != NULL) {
        *pSize = bytesRead;
    }

    *ppData = pData;

    return E_SUCCESS;
}

E_API e_result e_fs_open_and_read(e_fs* pFS, const char* pFilePath, void** ppData, size_t* pSize, const e_allocation_callbacks* pAllocationCallbacks)
{
    return e_fs_open_and_read_with_extra_byte(pFS, pFilePath, ppData, pSize, pAllocationCallbacks);
}

E_API e_result e_fs_open_and_read_text(e_fs* pFS, const char* pFilePath, char** ppStr, size_t* pLength, const e_allocation_callbacks* pAllocationCallbacks)
{
    return e_fs_open_and_read_with_extra_byte(pFS, pFilePath, (void**)ppStr, pLength, pAllocationCallbacks);
}
/* ==== END e_fs.c ==== */



/* ==== BEG e_archive.c ==== */
E_API e_result e_archive_init(const e_archive_vtable* pVTable, void* pVTableUserData, e_stream* pStream, const e_allocation_callbacks* pAllocationCallbacks, e_archive** ppArchive)
{
    e_result result;
    e_archive* pArchive;
    size_t allocSize;
    e_fs_config fsConfig;

    if (ppArchive == NULL) {
        return E_INVALID_ARGS;
    }

    *ppArchive = NULL;

    if (pVTable == NULL || pStream == NULL) {
        return E_INVALID_ARGS;
    }

    if (pVTable->archive_alloc_size == NULL || pVTable->init == NULL || pVTable->uninit == NULL) {
        return E_INVALID_ARGS;
    }

    /* The first thing we need to do is allocate memory for the archive. */
    result = pVTable->archive_alloc_size(pVTableUserData, &allocSize);
    if (result != E_SUCCESS) {
        return result;  /* Failed to retrieve the size somehow. */
    }

    pArchive = (e_archive*)e_malloc(allocSize, pAllocationCallbacks);
    if (pArchive == NULL) {
        return E_OUT_OF_MEMORY;
    }

    pArchive->pVTable = pVTable;
    pArchive->pVTableUserData = pVTableUserData;
    pArchive->pStream = pStream;

    /*
    With memory allocated we can now initialize the archive. An archive is a file system, so we
    need to initialize that first.
    */
    fsConfig = e_fs_config_init(&pVTable->fs, pVTableUserData);

    result = e_fs_init_preallocated(&fsConfig, pAllocationCallbacks, &pArchive->fs);
    if (result != E_SUCCESS) {
        e_free(pArchive, pAllocationCallbacks);
        return result;
    }

    /* With the file system initialized we'll need to initialize the rest. */
    result = pVTable->init(pVTableUserData, pStream, pAllocationCallbacks, pArchive);
    if (result != E_SUCCESS) {
        e_fs_uninit(&pArchive->fs, pAllocationCallbacks);
        e_free(pArchive, pAllocationCallbacks);
        return result;
    }

    *ppArchive = pArchive;
    return E_SUCCESS;
}

E_API e_result e_archive_init_from_file(const e_archive_vtable* pVTable, void* pVTableUserData, e_fs* pFS, const char* pFilePath, const e_allocation_callbacks* pAllocationCallbacks, e_archive** ppArchive)
{
    e_result result;
    e_file* pFile;

    if (ppArchive == NULL) {
        return E_INVALID_ARGS;
    }

    *ppArchive = NULL;

    result = e_fs_open(pFS, pFilePath, E_OPEN_MODE_READ, pAllocationCallbacks, &pFile);
    if (result != E_SUCCESS) {
        return result;  /* Failed to open the file. */
    }

    result = e_archive_init(pVTable, pVTableUserData, e_fs_file_stream(pFile), pAllocationCallbacks, ppArchive);
    if (result != E_SUCCESS) {
        e_fs_close(pFile, pAllocationCallbacks);
        return result;
    }

    (*ppArchive)->pArchiveFile = pFile;
    return E_SUCCESS;
}

E_API void e_archive_uninit(e_archive* pArchive, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_file* pArchiveFile;

    if (pArchive == NULL) {
        return;
    }

    pArchiveFile = pArchive->pArchiveFile;

    E_ASSERT(pArchive->pVTable->uninit != NULL);
    pArchive->pVTable->uninit(pArchive->pVTableUserData, pArchive, pAllocationCallbacks);

    e_fs_uninit(&pArchive->fs, pAllocationCallbacks);
    e_free(pArchive, pAllocationCallbacks);

    if (pArchiveFile != NULL) {
        e_fs_close(pArchiveFile, pAllocationCallbacks);
    }
}

E_API e_fs* e_archive_fs(e_archive* pArchive)
{
    if (pArchive == NULL) {
        return NULL;
    }

    return &pArchive->fs;
}

E_API e_stream* e_archive_stream(e_archive* pArchive)
{
    if (pArchive == NULL) {
        return NULL;
    }

    return pArchive->pStream;
}

E_API e_result e_archive_open(e_archive* pArchive, const char* pFilePath, e_open_mode openMode, const e_allocation_callbacks* pAllocationCallbacks, e_file** ppFile)
{
    return e_fs_open((e_fs*)pArchive, pFilePath, openMode, pAllocationCallbacks, ppFile);
}

E_API void e_archive_close(e_file* pFile, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_fs_close(pFile, pAllocationCallbacks);
}

E_API e_result e_archive_read(e_file* pFile, void* pDst, size_t bytesToRead, size_t* pBytesRead)
{
    return e_fs_read(pFile, pDst, bytesToRead, pBytesRead);
}

E_API e_result e_archive_write(e_file* pFile, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten)
{
    return e_fs_write(pFile, pSrc, bytesToWrite, pBytesWritten);
}

E_API e_result e_archive_seek(e_file* pFile, e_int64 offset, e_seek_origin origin)
{
    return e_fs_seek(pFile, offset, origin);
}

E_API e_result e_archive_tell(e_file* pFile, e_int64* pCursor)
{
    return e_fs_tell(pFile, pCursor);
}

E_API e_result e_archive_flush(e_file* pFile)
{
    return e_fs_flush(pFile);
}

E_API e_result e_archive_info(e_file* pFile, e_file_info* pInfo)
{
    return e_fs_info(pFile, pInfo);
}

E_API e_archive* e_archive_get(e_file* pFile)
{
    if (pFile == NULL) {
        return NULL;
    }

    return (e_archive*)e_fs_get(pFile);
}
/* ==== END e_archive.c ==== */



/* ==== BEG e_log.c ==== */
const char* e_log_level_to_string(e_log_level level)
{
    switch (level)
    {
    case E_LOG_LEVEL_ERROR:   return "ERROR";
    case E_LOG_LEVEL_WARNING: return "WARNING";
    case E_LOG_LEVEL_INFO:    return "INFO";
    case E_LOG_LEVEL_DEBUG:   return "DEBUG";
    default: break;
    }

    /* Should never get here. */
    return "ERROR";
}


#ifndef E_NO_DEFAULT_LOG_CALLBACK
    #if defined(E_EMSCRIPTEN)
        static void e_log_callback_default(void* pUserData, e_log_level level, const char* pMessage)
        {
            int flags = EM_LOG_CONSOLE;
            if (level == E_LOG_LEVEL_DEBUG) {
                flags |= EM_LOG_DEBUG;
            }
            if (level == E_LOG_LEVEL_INFO) {
                flags |= EM_LOG_INFO;
            }
            if (level == E_LOG_LEVEL_WARNING) {
                flags |= EM_LOG_WARN;
            }
            if (level == E_LOG_LEVEL_ERROR) {
                flags |= EM_LOG_ERROR;
            }

            E_UNUSED(pUserData);

            emscripten_log(flags, "%s\n", pMessage);
        }
    #else
        #include <stdio.h>

        static void e_log_callback_default(void* pUserData, e_log_level level, const char* pMessage)
        {
            E_UNUSED(pUserData);
            printf("%s: %s\n", e_log_level_to_string(level), pMessage);
        }
    #endif
#endif  /* E_NO_DEFAULT_LOG_CALLBACK */


E_API e_result e_log_init(const e_allocation_callbacks* pAllocationCallbacks, e_log** ppLog)
{
    e_result result;
    e_log* pLog;

    if (ppLog == NULL) {
        return E_OUT_OF_MEMORY;
    }

    *ppLog = NULL;

    pLog = (e_log*)e_calloc(sizeof(*pLog), pAllocationCallbacks);
    if (pLog == NULL) {
        return E_OUT_OF_MEMORY;
    }

    /* We need a persistent copy of the allocation callbacks so we can do dynamic allocations when posting long log messages if necessary. */
    pLog->allocationCallbacks = e_allocation_callbacks_init_copy(pAllocationCallbacks);

    /* We want a mutex to keep our logging thread-safe. */
    result = e_mutex_init(pAllocationCallbacks, &pLog->pMutex);
    if (result != E_SUCCESS) {
        e_free(pLog, pAllocationCallbacks);
        return result;
    }

    /* Register a default callback unless we've been requested not to at compile-time. */
    #ifndef E_NO_DEFAULT_LOG_CALLBACK
    {
        e_log_register_callback(pLog, e_log_callback_default, NULL, pAllocationCallbacks);
    }
    #endif

    *ppLog = pLog;
    return E_SUCCESS;
}

E_API void e_log_uninit(e_log* pLog, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pLog == NULL) {
        return;
    }

    e_mutex_uninit(pLog->pMutex, pAllocationCallbacks);
    e_free(pLog, pAllocationCallbacks);
}

static e_result e_log_register_callback_nolock(e_log* pLog, e_log_callback_proc onLog, void* pUserData, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_log_callbacks* pNewCallbacks;

    /*
    Since this function should be called a very infrequent number of times, I'm keeping this
    simple and just doing a realloc() each time.
    */
    pNewCallbacks = (e_log_callbacks*)e_realloc(pLog->pCallbacks, sizeof(*pNewCallbacks) * (pLog->callbackCount + 1), pAllocationCallbacks);
    if (pNewCallbacks == NULL) {
        return E_OUT_OF_MEMORY;
    }

    pNewCallbacks[pLog->callbackCount].onLog = onLog;
    pNewCallbacks[pLog->callbackCount].pUserData = pUserData;

    pLog->pCallbacks = pNewCallbacks;
    pLog->callbackCount += 1;

    return E_SUCCESS;
}

E_API e_result e_log_register_callback(e_log* pLog, e_log_callback_proc onLog, void* pUserData, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_result result;

    if (pLog == NULL || onLog == NULL) {
        return E_INVALID_ARGS;
    }

    e_mutex_lock(pLog->pMutex);
    {
        result = e_log_register_callback_nolock(pLog, onLog, pUserData, pAllocationCallbacks);
    }
    e_mutex_unlock(pLog->pMutex);

    return result;
}

E_API e_result e_log_post(e_log* pLog, e_log_level level, const char* pMessage)
{
    if (pLog == NULL) {
        return E_INVALID_ARGS;
    }

    E_ASSERT(pMessage != NULL); /* Should not be hitting this. If you're triggering this it means you're passing in NULL for your message. That's not allowed and most likely an error. */
    if (pMessage == NULL) {
        return E_INVALID_ARGS;
    }

    e_mutex_lock(pLog->pMutex);
    {
        size_t iLog;
        for (iLog = 0; iLog < pLog->callbackCount; iLog += 1) {
            if (pLog->pCallbacks[iLog].onLog) {
                pLog->pCallbacks[iLog].onLog(pLog->pCallbacks[iLog].pUserData, level, pMessage);
            }
        }
    }
    e_mutex_unlock(pLog->pMutex);

    return E_SUCCESS;
}

E_API e_result e_log_postv(e_log* pLog, e_log_level level, const char* pFormat, va_list args)
{
    e_result result;
    int length;
    char  pFormattedMessageStack[1024];
    char* pFormattedMessageHeap = NULL;

    /* First try formatting into our fixed sized stack allocated buffer. If this is too small we'll fall back to a heap allocation. */
    length = e_vsnprintf(pFormattedMessageStack, sizeof(pFormattedMessageStack), pFormat, args);
    if (length < 0) {
        return E_INVALID_OPERATION; /* An error occured when trying to convert the buffer. */
    }

    if ((size_t)length < sizeof(pFormattedMessageStack)) {
        /* The string was written to the stack. */
        result = e_log_post(pLog, level, pFormattedMessageStack);
    } else {
        /* The stack buffer was too small, try the heap. */
        pFormattedMessageHeap = (char*)e_malloc((size_t)length + 1, &pLog->allocationCallbacks);
        if (pFormattedMessageHeap == NULL) {
            return E_OUT_OF_MEMORY;
        }

        length = e_vsnprintf(pFormattedMessageHeap, (size_t)length + 1, pFormat, args);
        if (length < 0) {
            e_free(pFormattedMessageHeap, &pLog->allocationCallbacks);
            return E_INVALID_OPERATION;
        }

        result = e_log_post(pLog, level, pFormattedMessageHeap);
        e_free(pFormattedMessageHeap, &pLog->allocationCallbacks);
    }

    return result;
}

E_API e_result e_log_postf(e_log* pLog, e_log_level level, const char* pFormat, ...)
{
    e_result result;
    va_list args;

    if (pLog == NULL || pFormat == NULL) {
        return E_INVALID_ARGS;
    }

    va_start(args, pFormat);
    {
        result = e_log_postv(pLog, level, pFormat, args);
    }
    va_end(args);

    return result;
}
/* ==== END e_log.c ==== */




/* ==== BEG e_config_file.c ==== */
static e_result e_result_from_lua(int result)
{
    switch (result)
    {
        case LUA_OK:     return E_SUCCESS;
        case LUA_ERRERR: return E_ERROR;
        case LUA_ERRMEM: return E_OUT_OF_MEMORY;
        default: break;
    }

    /* Fall back to a generic error. */
    return E_ERROR;
}


static void* e_lua_alloc(void* ud, void* ptr, size_t osize, size_t nsize)
{
    const e_allocation_callbacks* pAllocationCallbacks = (const e_allocation_callbacks*)ud;
    E_UNUSED(osize);    /* We don't need the old size. We leave it to the allocation callbacks themselves to track that. */

    /*
    Lua uses the same callback for both allocations and frees. When the new size is zero, the
    pointer needs to be freed(). If it's non-zero we need to do a realloc().
    */
    if (nsize == 0) {
        e_free(ptr, pAllocationCallbacks);
        return NULL;
    } else {
        return e_realloc(ptr, nsize, pAllocationCallbacks);
    }
}


typedef struct
{
    e_stream* pStream;
    char buffer[1024];
} e_lua_read_state;

static const char* e_lua_read(lua_State* L, void* ud, size_t* size)
{
    /*
    The callback Lua uses for reading is absolutely terrible. You have to return the data as a
    `const char*`. Really?! This means we need to store a buffer somewhere, read into that,
    and then return a pointer to that buffer. How ridiculous.
    */
    e_lua_read_state* pState = (e_lua_read_state*)ud;
    E_ASSERT(pState != NULL);
    E_ASSERT(size   != NULL);
    E_UNUSED(L);

    *size = 0;

    if (e_stream_read(pState->pStream, pState->buffer, sizeof(pState->buffer), size) != E_SUCCESS) {
        return NULL;
    }

    return pState->buffer;
}


E_API e_result e_config_file_init(const e_allocation_callbacks* pAllocationCallbacks, e_config_file* pConfigFile)
{
    lua_State* pLua;

    if (pConfigFile == NULL) {
        return E_INVALID_ARGS;
    }

    E_ZERO_OBJECT(pConfigFile);

    /* A persistent copy of the allocation callbacks needs to be made to ensure they stay valid for the life of the Lua state. */
    pConfigFile->allocationCallbacks = e_allocation_callbacks_init_copy(pAllocationCallbacks);

    pLua = lua_newstate(e_lua_alloc, &pConfigFile->allocationCallbacks);
    if (pLua == NULL) {
        return E_OUT_OF_MEMORY;
    }

    pConfigFile->pLuaState = pLua;

    return E_SUCCESS;
}

E_API void e_config_file_uninit(e_config_file* pConfigFile, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pConfigFile == NULL) {
        return;
    }

    /*
    Lua stores a persistent copy of the allocation callback that we specified at initialization
    time. Therefore there's no need to actually do anything with our allocation callbacks.
    */
    E_UNUSED(pAllocationCallbacks);

    lua_close((lua_State*)pConfigFile->pLuaState);
}


static void e_config_file_merge_tables(lua_State* pDst, lua_State* pSrc, const char* pName, e_log* pLog)
{
    E_ASSERT(pDst != NULL);
    E_ASSERT(pSrc != NULL);

    /* The tables that we're merging should be sitting at the top of stack for both Lua states. */
    lua_pushnil(pSrc);

    /* Enumerate over each of the items in the source table. */
    while (lua_next(pSrc, -2)) {
        /* We'll support keys of both strings and integers. Anything else needs to be ignored. */
        if (lua_isinteger(pSrc, -2)) {
            lua_pushinteger(pDst, lua_tointeger(pSrc, -2));
        } else if (lua_isstring(pSrc, -2)) {
            lua_pushstring(pDst, lua_tostring(pSrc, -2));
        } else {
            /* Not a supported key type. Ignore it. */
            e_log_postf(pLog, E_LOG_LEVEL_WARNING, "Config '%s': Key of type '%s' is not supported in configs. Ignoring.", pName, lua_typename(pSrc, lua_type(pSrc, -2)));
            lua_pop(pDst, 1);
            continue;
        }

        /*
        The source will have it's key pushed to the stack, so now we need to push the value. This
        is the complicated part because the value could be another table. In this case we need to
        call this function recursively.
        */
        if (lua_isinteger(pSrc, -1)) {
            lua_pushinteger(pDst, lua_tointeger(pSrc, -1));
        } else if (lua_isnumber(pSrc, -1)) {
            lua_pushnumber(pDst, lua_tonumber(pSrc, -1));
        } else if (lua_isboolean(pSrc, -1)) {
            lua_pushboolean(pDst, lua_toboolean(pSrc, -1));
        } else if (lua_istable(pSrc, -1)) {
            /*
            If the table already exists in the destination we just use the existing table (it
            cannot be overwritten). Otherwise we just create a new table.

            The key will already be on the stack, however we need to push it again because
            lua_gettable() will pop the key from the stack. We will need the key again later when
            we call lua_settable() later on.
            */
            lua_pushvalue(pDst, -1);
            lua_gettable(pDst, -3);

            /*
            At this point the top of the stack will either be nil, or some other type. If it's not
            a table we just overwrite it.
            */
            if (lua_istable(pDst, -1)) {
                /* It's a table. It needs to be merged. */
                e_config_file_merge_tables(pDst, pSrc, pName, pLog);
            } else {
                /* It's not a table. We need to overwrite it with a brand new table. */
                lua_pop(pDst, 1);   /* Get rid of the nil that was pushed from lua_gettable(). */
                lua_createtable(pDst, 0, 0);
                e_config_file_merge_tables(pDst, pSrc, pName, pLog);
            }
        } else if (lua_isstring(pSrc, -1)) {
            lua_pushstring(pDst, lua_tostring(pSrc, -1));
        } else {
            e_log_postf(pLog, E_LOG_LEVEL_WARNING, "Config '%s': Value of type '%s' is not supported in configs. Ignoring.", pName, lua_typename(pSrc, lua_type(pSrc, -2)));
            lua_pop(pDst, 1);   /* Pop the key from the destination stack. */
            lua_pop(pSrc, 1);   /* Pop the value from the source stack in preparation for the next iteration .*/
            continue;
        }

        /* At this point the source will have it's key and value on the stack and we can now apply it. */
        lua_settable(pDst, -3);

        /* Pop the value and continue iteration of the input table. */
        lua_pop(pSrc, 1);
    }
}

E_API e_result e_config_file_load(e_config_file* pConfigFile, e_stream* pStream, const char* pName, const e_allocation_callbacks* pAllocationCallbacks, e_log* pLog)
{
    /*
    A complication with our configs is that we don't want to replace config settings, but rather we
    want to merge them.

    We want to support the ability to load from multiple config files, but we want to have it so
    config files don't overwrite tables. For example, say we have this base config:

        game =
        {
            title = "Game";
            resolutionX = 1980;
            resolutionY = 1080;
        }

    But then a mod wants to change only the name and then maybe include some mod-specific stuff,
    they might want to do something like this:

        game =
        {
            title = "My Mod";
            modSetting1 = "something";
            modSetting2 = "something else";
        }

    The second script will run, but only the title has changed. However, because the "game" table
    is being assigned, it will overwrite the entire table and we'll lose the other settings.
    Instead what needs to happen is the tables need to be merged so that the base settings are
    maintained.

    To achieve all of this, we'll load the script in a separate Lua state. Then we'll iterate over
    each global and recursively write out each member to the primary Lua state. This is slow, but
    should result in a fairly robust configuration system.
    */
    e_result result;
    e_lua_read_state readState;
    lua_State* pSecondaryLua;

    if (pConfigFile == NULL || pStream == NULL) {
        return E_INVALID_ARGS;
    }

    if (pName == NULL) {
        pName = "";
    }

    pSecondaryLua = lua_newstate(e_lua_alloc, (void*)pAllocationCallbacks);
    if (pSecondaryLua == NULL) {
        return E_OUT_OF_MEMORY;
    }

    /*
    The stream needs to be loaded into the Lua state. We can do this efficiently with lua_load().
    We don't want to use lua_loadstring() here because it's unnecessarily inefficient.
    */
    readState.pStream = pStream;
    result = e_result_from_lua(lua_load(pSecondaryLua, e_lua_read, &readState, pName, NULL));
    if (result == E_SUCCESS) {
        result = e_result_from_lua(lua_pcall(pSecondaryLua, 0, LUA_MULTRET, 0));
    }

    /* If we failed to load we'll need to post the error to the log. */
    if (result != E_SUCCESS) {
        const char* pErrorString = lua_tostring(pSecondaryLua, lua_gettop(pSecondaryLua));
        if (pErrorString != NULL) {
            e_log_postf(pLog, E_LOG_LEVEL_ERROR, "Error loading config \"%s\": %s", pName, pErrorString);
        }

        lua_close(pSecondaryLua);
        return result;
    }

    /*
    Getting here means we successfully passed the Lua script. We should now be able to scan over it
    and write everything out to the primary script.

    Note that we're only merging basic variables (strings, numbers and booleans). Functions will
    not be transferred.
    */
    lua_pushglobaltable((lua_State*)pConfigFile->pLuaState);
    lua_pushglobaltable(pSecondaryLua);
    {
        e_config_file_merge_tables((lua_State*)pConfigFile->pLuaState, pSecondaryLua, pName, pLog);
    }
    lua_pop((lua_State*)pConfigFile->pLuaState, 1);
    lua_pop(pSecondaryLua, 1);

    /* Getting here means we're done. */
    lua_close(pSecondaryLua);
    return E_SUCCESS;
}

E_API e_result e_config_file_load_file(e_config_file* pConfigFile, e_fs* pFS, const char* pFilePath, const e_allocation_callbacks* pAllocationCallbacks, e_log* pLog)
{
    e_result result;
    e_file* pFile;

    result = e_fs_open(pFS, pFilePath, E_OPEN_MODE_READ, pAllocationCallbacks, &pFile);
    if (result != E_SUCCESS) {
        e_log_postf(pLog, E_LOG_LEVEL_ERROR, "Could not open \"%s\". %s.\n", pFilePath, e_result_description(result));
    }

    result = e_config_file_load(pConfigFile, e_fs_file_stream(pFile), pFilePath, pAllocationCallbacks, pLog);
    e_fs_close(pFile, pAllocationCallbacks);

    return result;
}

static e_result e_config_file_get_value(e_config_file* pConfigFile, const char* pSection, const char* pName)
{
    lua_State* pLua;
    int oldTop;
    size_t iPart;
    const char* pParts[] =
    {
        pSection,
        pName
    };

    E_ASSERT(pConfigFile != NULL);
    E_ASSERT(pName != NULL);

    pLua = (lua_State*)pConfigFile->pLuaState;
    E_ASSERT(pLua != NULL);

    oldTop = lua_gettop(pLua);

    /*
    What we're really doing is retrieving a value equal to "[pSection].[pName]" or simply "[pName]"
    if no section is specified.
    */
    lua_pushglobaltable(pLua);

    for (iPart = 0; iPart < E_COUNTOF(pParts); iPart += 1) {
        const char* pPart = pParts[iPart];
        size_t offset = 0;

        if (pPart == NULL) {
            continue;
        }

        /* For each component in the part. */
        while (pPart[offset] != '\0') {
            const char* pSegmentBeg = pPart + offset;
            const char* pSegmentEnd = pSegmentBeg;
            
            /* Find the end of the segment. */
            for (;;) {
                if (pPart[offset] == '\0' || pPart[offset] == '.') {
                    /* Found the end of the segment. */
                    break;
                } else {
                    /* Not the end. Keep going. */
                    offset += 1;
                }
            }

            pSegmentEnd = pPart + offset;
            if (pSegmentEnd == pSegmentBeg) {
                /* Not a valid segment. We didn't move forward. Abort. */
                break;
            }

            /*printf("TESTING: '%.*s'\n", (unsigned int)(pSegmentEnd - pSegmentBeg), pSegmentBeg);*/

            /* If the item on the stack at -1 is not a table we cannot continue. */
            if (!lua_istable(pLua, -1)) {
                break;
            }

            /* Getting here means we should have the end of the segment. */
            lua_pushlstring(pLua, pSegmentBeg, (pSegmentEnd - pSegmentBeg));
            lua_gettable(pLua, -2);

            /* Abort if there is nothing in the table with the specified name. */
            if (lua_isnoneornil(pLua, -1)) {
                break;
            }

            /* Skip past the delimiter in preparation for the next iteration. */
            if (pPart[offset] != '\0') {
                offset += 1;
            }
        }
    }

    /* If we don't have a valid top item we need to abort with an error. */
    if (lua_isnoneornil(pLua, -1)) {
        lua_pop(pLua, lua_gettop(pLua) - oldTop);
        return E_DOES_NOT_EXIST;
    }

    return E_SUCCESS;
}

E_API e_result e_config_file_get_string(e_config_file* pConfigFile, const char* pSection, const char* pName, const e_allocation_callbacks* pAllocationCallbacks, char** ppValue)
{
    e_result result = E_SUCCESS;
    lua_State* pLua;
    char* pValue;
    int prevTop;

    if (ppValue != NULL) {
        *ppValue = NULL;
    }

    if (pConfigFile == NULL) {
        return E_INVALID_ARGS;
    }

    if (pName == NULL) {
        return E_INVALID_ARGS;  /* Must have a name. */
    }

    pLua = (lua_State*)pConfigFile->pLuaState;
    if (pLua == NULL) {
        return E_INVALID_OPERATION;
    }

    prevTop = lua_gettop(pLua);

    result = e_config_file_get_value(pConfigFile, pSection, pName);
    if (result != E_SUCCESS) {
        return result;
    }

    /* The value will be sitting at the top of the stack. */
    if (lua_isstring(pLua, -1)) {
        size_t valueLen;
        const char* pValueFromLua = lua_tolstring(pLua, -1, &valueLen);
        if (pValueFromLua != NULL) {
            /* We need to allocate a dynamic string for safety. */
            pValue = (char*)e_malloc(valueLen + 1, pAllocationCallbacks);
            if (pValue == NULL) {
                result = E_OUT_OF_MEMORY;
                goto done;
            }

            result = e_result_from_errno(c89str_strcpy_s(pValue, valueLen + 1, pValueFromLua));
            if (result != E_SUCCESS) {
                e_free(pValue, pAllocationCallbacks);
                goto done;
            }
        } else {
            pValue = NULL;
            result = E_ERROR;   /* Don't think this should ever happen. */
        }
    } else {
        /* Not a string. We're doing strong typing here, so don't want to convert numbers and booleans. */
        pValue = NULL;
        result = E_INVALID_DATA;
    }

done:
    lua_pop(pLua, lua_gettop(pLua) - prevTop);  /* Restores the stack. */

    if (result != E_SUCCESS) {
        return result;
    }

    if (ppValue != NULL) {
        *ppValue = pValue;
    }

    return E_SUCCESS;
}

E_API e_result e_config_file_get_int(e_config_file* pConfigFile, const char* pSection, const char* pName, int* pValue)
{
    e_result result = E_SUCCESS;
    lua_State* pLua;
    int prevTop;
    lua_Integer value;
    int isNumber = 0;

    if (pValue != NULL) {
        *pValue = 0;
    }

    if (pConfigFile == NULL) {
        return E_INVALID_ARGS;
    }

    if (pName == NULL) {
        return E_INVALID_ARGS;  /* Must have a name. */
    }

    pLua = (lua_State*)pConfigFile->pLuaState;
    if (pLua == NULL) {
        return E_INVALID_OPERATION;
    }

    prevTop = lua_gettop(pLua);

    result = e_config_file_get_value(pConfigFile, pSection, pName);
    if (result != E_SUCCESS) {
        return result;
    }

    value = lua_tointegerx(pLua, -1, &isNumber);

    /* Make sure the stack is restored. */
    lua_pop(pLua, lua_gettop(pLua) - prevTop);

    if (isNumber == 0) {
        result = E_INVALID_DATA;
    }

    if (pValue != NULL) {
        *pValue = (int)value;
    }

    return result;
}

E_API e_result e_config_file_get_uint(e_config_file* pConfigFile, const char* pSection, const char* pName, unsigned int* pValue)
{
    e_result result;
    int value;

    if (pValue != NULL) {
        *pValue = 0;
    }

    result = e_config_file_get_int(pConfigFile, pSection, pName, &value);
    if (result != E_SUCCESS) {
        return result;
    }

    if (pValue != NULL) {
        *pValue = (unsigned int)value;
    }

    return E_SUCCESS;
}
/* ==== END e_config_file.c ==== */



/* ==== BEG e_engine.c ==== */
/* Helper for retrieving the GLBapi object from the engine. This is just a cast. */
#if !defined(E_NO_OPENGL) && !defined(E_EMSCRIPTEN)
static GLBapi* e_engine_gl(const e_engine* pEngine)
{
    E_ASSERT(pEngine != NULL);

    if ((pEngine->flags & E_ENGINE_FLAG_NO_OPENGL) != 0) {
        return NULL;    /* OpenGL disabled. */
    }

    /* Could we lazily initialize this? If it fails, just set the E_ENGINE_FLAG_NO_OPENGL flag so we don't try again? */
    return (GLBapi*)pEngine->pGL;
}
#endif

#if! defined(E_NO_VULKAN)
static VkbAPI* e_engine_vk(const e_engine* pEngine)
{
    E_ASSERT(pEngine != NULL);

    if ((pEngine->flags & E_ENGINE_FLAG_NO_VULKAN) != 0) {
        return NULL;    /* Vulkan disabled. */
    }

    /* Could we lazily initialize this? If it fails, just set the E_ENGINE_FLAG_NO_VULKAN flag so we don't try again? */
    return (VkbAPI*)pEngine->pVK;
}

static e_result e_result_from_vk(VkResult result)
{
    switch (result)
    {
        case VK_SUCCESS:                    return E_SUCCESS;
        case VK_ERROR_OUT_OF_HOST_MEMORY:   return E_OUT_OF_MEMORY;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY: return E_OUT_OF_MEMORY;
        default: break;
    }

    /* Fallback to a generic error. */
    return E_ERROR;
}
#endif



E_API e_engine_config e_engine_config_init(int argc, char** argv, unsigned int flags, e_engine_vtable* pVTable)
{
    e_engine_config config;

    E_ZERO_OBJECT(&config);
    config.argc    = argc;
    config.argv    = argv;
    config.flags   = flags;
    config.pVTable = pVTable;

    return config;
}


E_API e_result e_engine_init(const e_engine_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_engine** ppEngine)
{
    e_result result;
    e_engine* pEngine;
    size_t allocationSize;
    unsigned int flags;
    e_log* pLog;
    e_bool8 isOwnerOfLog;
    e_fs_config fsConfig;

#if !defined(E_NO_OPENGL)
    size_t glbindOffset = 0;
#endif
#if !defined(E_NO_VULKAN)
    size_t vkbindOffset = 0;
#endif

    if (pConfig == NULL) {
        return E_INVALID_ARGS;
    }

    if (ppEngine == NULL) {
        return E_INVALID_ARGS;
    }

    flags = pConfig->flags;


    /* We want a log as soon as possible so we can start logging errors. */
    if (pConfig->pLog == NULL) {
        result = e_log_init(pAllocationCallbacks, &pLog);
        if (result != E_SUCCESS) {
            return result;
        }

        isOwnerOfLog = E_TRUE;
    } else {
        pLog = pConfig->pLog;
        isOwnerOfLog = E_FALSE;
    }


    /* From this point out, do not use pConfig->flags. Use the local flags variable instead. */

    /* Force some flags depending on compile-time disabled features. */
    #if defined(E_NO_OPENGL)
    {
        flags |= E_ENGINE_FLAG_NO_OPENGL;
    }
    #endif
    #if defined(E_NO_VULKAN)
    {
        flags |= E_ENGINE_FLAG_NO_VULKAN;
    }
    #endif


    /* Make sure the platform is initialized first. */
    /* TODO: Reference count this. */
    result = e_platform_init();
    if (result != E_SUCCESS) {
        e_log_postf(pLog, E_LOG_LEVEL_ERROR, "Failed to initialize platform layer.");
        return result;
    }

    allocationSize = sizeof(*pEngine);
    #if !defined(E_NO_OPENGL)
    {
        #if !defined(E_EMSCRIPTEN)
            if ((flags & E_ENGINE_FLAG_NO_OPENGL) == 0) {
                glbindOffset = allocationSize;
                allocationSize += sizeof(GLBapi);
            }
        #endif
    }
    #endif

    #if !defined(E_NO_VULKAN)
    {
        if ((flags & E_ENGINE_FLAG_NO_VULKAN) == 0) {
            vkbindOffset = allocationSize;
            allocationSize += sizeof(VkbAPI);
        }
    }
    #endif

    pEngine = (e_engine*)e_calloc(allocationSize, pAllocationCallbacks);
    if (pEngine == NULL) {
        return E_OUT_OF_MEMORY;
    }

    pEngine->pUserData       = pConfig->pUserData;
    pEngine->argc            = pConfig->argc;
    pEngine->argv            = pConfig->argv;
    pEngine->flags           = flags;
    pEngine->pVTable         = pConfig->pVTable;
    pEngine->pVTableUserData = pConfig->pVTableUserData;
    pEngine->pLog            = pLog;
    pEngine->isOwnerOfLog    = isOwnerOfLog;
    pEngine->pGL             = NULL;
    pEngine->pVK             = NULL;

    /* We need a file system so we can load stuff like the config file. */
    fsConfig = e_fs_config_init(pConfig->pFSVTable, pConfig->pFSVTableUserData);

    result = e_fs_init_preallocated(&fsConfig, pAllocationCallbacks, &pEngine->fs);
    if (result != E_SUCCESS) {
        return result;
    }

    /* Now that our file system is set up we can load our config. */
    result = e_config_file_init(pAllocationCallbacks, &pEngine->configFile);
    if (result != E_SUCCESS) {
        e_fs_uninit(&pEngine->fs, pAllocationCallbacks);
        return result;
    }

    /*
    We'll try loading a default config from the working directory. This is not a critical error if
    it fails, but we'll post a warning about it.
    */
    result = e_config_file_load_file(&pEngine->configFile, &pEngine->fs, E_DEFAULT_CONFIG_FILE_PATH, pAllocationCallbacks, pEngine->pLog);
    if (result != E_SUCCESS) {
        e_log_postf(pEngine->pLog, E_LOG_LEVEL_WARNING, "Failed to load default config file '%s'.", E_DEFAULT_CONFIG_FILE_PATH);
    }


    #ifndef E_NO_OPENGL
    {
        if ((flags & E_ENGINE_FLAG_NO_OPENGL) == 0) {
            pEngine->pGL = E_OFFSET_PTR(pEngine, glbindOffset);

            /* We need to initialize vkbind. If this fails, it's not critical, but we need to post a log. */
            #ifndef E_EMSCRIPTEN
            {
                GLenum resultGL = glbInit((GLBapi*)pEngine->pGL, NULL);
                if (resultGL != GL_NO_ERROR) {
                    /* Initialization of glbind failed. Not the end of the world because another backend might be available, but make sure we don't try using it. */
                    e_log_postf(pLog, E_LOG_LEVEL_WARNING, "Cannot use OpenGL because glbind failed to initialize.");
                    pEngine->pGL = NULL;    /* Can't have people trying to use OpenGL. */
                    pEngine->flags |= E_ENGINE_FLAG_NO_OPENGL;
                }

                /*
                We need to bind to global scope because that's what we'll be using with Emscripten. Later on we
                may do a system where we do the opposite and just take the function pointers and set them in the
                glbind object. We might integrate support directly into glbind at some point once I figure out
                how everything interacts with each other.
                */
                glbBindAPI((const GLBapi*)pEngine->pGL);
            }
            #else
            {
                /*
                Nothing to actually do here for Emscripten. The EGL context and surface will be created as part
                of the graphics surface.
                */
            }
            #endif
        }
    }
    #endif
    #ifndef E_NO_VULKAN
    {
        if ((flags & E_ENGINE_FLAG_NO_VULKAN) == 0) {
            pEngine->pVK = E_OFFSET_PTR(pEngine, vkbindOffset);

            /* We need to initialize vkbind. If this fails, it's not critical, but we need to post a log. */
            {
                VkResult resultVK = vkbInit((VkbAPI*)pEngine->pVK);
                if (resultVK != VK_SUCCESS) {
                    /* Initialization of glbind failed. Not the end of the world because another backend might be available, but make sure we don't try using it. */
                    e_log_postf(pLog, E_LOG_LEVEL_WARNING, "Cannot use Vulkan because vkbind failed to initialize.");
                    pEngine->pVK = NULL;
                    pEngine->flags |= E_ENGINE_FLAG_NO_VULKAN;
                }
            }
        }
    }
    #endif

    *ppEngine = pEngine;
    return E_SUCCESS;
}

E_API void e_engine_uninit(e_engine* pEngine, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pEngine == NULL) {
        return;
    }

    #ifndef E_NO_OPENGL
    {
        #ifndef E_EMSCRIPTEN
            if (pEngine->pGL != NULL) {
                glbUninit();
            }
        #endif
    }
    #endif
    #ifndef E_NO_VULKAN
    {
        if (pEngine->pVK != NULL) {
            vkbUninit();
        }
    }
    #endif

    e_free(pEngine, pAllocationCallbacks);

    /* Uninitialize the global platform-specific stuff last. */
    /* TODO: Reference count this. */
    e_platform_uninit();
}

E_API e_log* e_engine_get_log(e_engine* pEngine)
{
    if (pEngine == NULL) {
        return NULL;
    }

    return pEngine->pLog;
}

static e_result e_engine_step_callback(void* pUserData)
{
    e_engine* pEngine = (e_engine*)pUserData;
    E_ASSERT(pEngine != NULL);

    if (pEngine->pVTable == NULL || pEngine->pVTable->onStep == NULL) {
        return E_INVALID_OPERATION;
    }

    /* TODO: Calculate delta time. */
    return pEngine->pVTable->onStep(pEngine->pVTableUserData, pEngine, 0);
}

E_API e_result e_engine_run(e_engine* pEngine)
{
    e_result result;
    int exitCode;

    if (pEngine == NULL) {
        return E_INVALID_ARGS;
    }

    result = e_platform_main_loop(&exitCode, e_engine_step_callback, (void*)pEngine);
    if (result != E_SUCCESS) {
        return result;
    }

    return E_SUCCESS;
}

E_API e_result e_engine_exit(e_engine* pEngine, int exitCode)
{
    if (pEngine == NULL) {
        return E_INVALID_ARGS;
    }

    return e_platform_exit_main_loop(exitCode);
}

E_API e_fs* e_engine_get_file_system(e_engine* pEngine)
{
    if (pEngine == NULL) {
        return NULL;
    }

    return &pEngine->fs;
}

E_API e_config_file* e_engine_get_config_file(e_engine* pEngine)
{
    if (pEngine == NULL) {
        return NULL;
    }

    return &pEngine->configFile;
}

E_API e_bool32 e_engine_is_graphics_backend_supported(const e_engine* pEngine, e_graphics_backend backend)
{
    if (pEngine == NULL) {
        return E_FALSE;
    }

    if (backend == E_GRAPHICS_BACKEND_CUSTOM) {
        return E_TRUE;  /* Custom backends work independently of the engine. */
    }

    if (backend == E_GRAPHICS_BACKEND_OPENGL) {
        return ((pEngine->flags & E_ENGINE_FLAG_NO_OPENGL) == 0 && pEngine->pGL != NULL);
    }

    if (backend == E_GRAPHICS_BACKEND_VULKAN) {
        return ((pEngine->flags & E_ENGINE_FLAG_NO_VULKAN) == 0 && pEngine->pVK != NULL);
    }

    return E_FALSE;
}

E_API void* e_engine_get_glapi(const e_engine* pEngine)
{
    if (pEngine == NULL) {
        return NULL;
    }

    return pEngine->pGL;
}

E_API void* e_engine_get_vkapi(const e_engine* pEngine)
{
    if (pEngine == NULL) {
        return NULL;
    }

    return pEngine->pVK;
}
/* ==== END e_engine.c ==== */



/* ==== BEG e_window.c ==== */
E_API e_window_config e_window_config_init(e_engine* pEngine, const char* pTitle, int posX, int posY, unsigned int sizeX, unsigned int sizeY, unsigned int flags, e_window_vtable* pVTable)
{
    e_window_config config;

    if (pTitle == NULL) {
        pTitle = "Game";
    }

    E_ZERO_OBJECT(&config);
    config.pEngine = pEngine;
    config.pTitle  = pTitle;
    config.posX    = posX;
    config.posY    = posY;
    config.sizeX   = sizeX;
    config.sizeY   = sizeY;
    config.flags   = flags;
    config.pVTable = pVTable;

    return config;
}


typedef struct
{
    size_t size;
    size_t platformWindowOffset;
} e_window_alloc_layout;

static e_result e_window_get_alloc_layout(const e_window_config* pConfig, e_window_alloc_layout* pLayout)
{
    E_ASSERT(pConfig != NULL);
    E_ASSERT(pLayout != NULL);
    E_UNUSED(pConfig);

    pLayout->size  = E_ALIGN_64(sizeof(e_window));

    pLayout->platformWindowOffset = pLayout->size;
    pLayout->size += E_ALIGN_64(e_platform_window_sizeof());

    return E_SUCCESS;
}

E_API e_result e_window_alloc_size(const e_window_config* pConfig, size_t* pSize)
{
    e_result result;
    e_window_alloc_layout layout;

    if (pSize == NULL) {
        return E_INVALID_ARGS;
    }

    *pSize = 0;

    if (pConfig == NULL) {
        return E_INVALID_ARGS;
    }

    result = e_window_get_alloc_layout(pConfig, &layout);
    if (result != E_SUCCESS) {
        return result;
    }

    *pSize = layout.size;
    return E_SUCCESS;
}

E_API e_result e_window_init_preallocated(const e_window_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_window* pWindow)
{
    e_result result;
    e_window_alloc_layout layout;

    if (pWindow == NULL || pConfig == NULL) {
        return E_INVALID_ARGS;
    }

    result = e_window_get_alloc_layout(pConfig, &layout);
    if (result != E_SUCCESS) {
        return result;
    }

    pWindow->pEngine         = pConfig->pEngine;
    pWindow->pUserData       = pConfig->pUserData;
    pWindow->pVTable         = pConfig->pVTable;
    pWindow->pVTableUserData = pConfig->pVTableUserData;

    /* The platform-specific part needs to be initialized. */
    pWindow->pPlatformWindow = (e_platform_window*)E_OFFSET_PTR(pWindow, layout.platformWindowOffset);
    result = e_platform_window_init_preallocated(pConfig, pWindow, pAllocationCallbacks, pWindow->pPlatformWindow);
    if (result != E_SUCCESS) {
        e_log_postf(e_engine_get_log(pConfig->pEngine), E_LOG_LEVEL_ERROR, "Failed to create platform window.");
        return result;
    }

    return E_SUCCESS;
}

E_API e_result e_window_init(const e_window_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_window** ppWindow)
{
    e_result result;
    e_window* pWindow;
    size_t allocationSize;

    if (ppWindow == NULL) {
        return E_INVALID_ARGS;
    }

    *ppWindow = NULL;

    result = e_window_alloc_size(pConfig, &allocationSize);
    if (result != E_SUCCESS) {
        return result;
    }

    E_ASSERT(allocationSize >= sizeof(e_window));

    pWindow = (e_window*)e_calloc(allocationSize, pAllocationCallbacks);
    if (pWindow == NULL) {
        return E_OUT_OF_MEMORY;
    }

    result = e_window_init_preallocated(pConfig, pAllocationCallbacks, pWindow);
    if (result != E_SUCCESS) {
        e_free(pWindow, pAllocationCallbacks);
        return result;
    }

    pWindow->freeOnUninit = E_TRUE;

    *ppWindow = pWindow;
    return E_SUCCESS;
}

E_API void e_window_uninit(e_window* pWindow, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pWindow == NULL) {
        return;
    }

    /* Uninitialize the platform-specific part first. */
    e_platform_window_uninit(pWindow->pPlatformWindow, pAllocationCallbacks);

    if (pWindow->freeOnUninit) {
        e_free(pWindow, pAllocationCallbacks);
    }
}

E_API e_engine* e_window_get_engine(const e_window* pWindow)
{
    if (pWindow == NULL) {
        return NULL;
    }

    return pWindow->pEngine;
}

E_API void* e_window_get_user_data(const e_window* pWindow)
{
    if (pWindow == NULL) {
        return NULL;
    }

    return pWindow->pUserData;
}

E_API e_window_vtable* e_window_get_vtable(const e_window* pWindow)
{
    if (pWindow == NULL) {
        return NULL;
    }

    return pWindow->pVTable;
}

E_API void* e_window_get_platform_object(const e_window* pWindow, e_platform_object_type type)
{
    return e_platform_window_get_object(pWindow->pPlatformWindow, type);
}
/* ==== END e_window.c ==== */




/* ==== BEG e_graphics.c ==== */
/*
OpenGL backend.
*/
#if !defined(E_NO_OPENGL)
#if defined(E_EMSCRIPTEN)
#include <GLES3/gl32.h>
#endif

typedef struct
{
    e_graphics graphics;    /* Must be the first item so we can cast. */
} e_graphics_opengl;

typedef struct
{
    e_graphics_surface rt;
    
    /* We need to use a platform-specific rendering context. */
    union
    {
    #if defined(E_WINDOWS)
        struct
        {
            HGLRC hRC;  /* Used for window render targets. */
        } win32;
    #endif
    #if defined(E_DESKTOP_UNIX)
        struct
        {
            XDisplay* pDisplay;
        } x11;
    #endif
    #if defined(E_EMSCRIPTEN)
        struct
        {
            EGLContext context;
            EGLSurface surface;
        } egl;
    #endif
    } platform;
} e_graphics_surface_opengl;


typedef struct
{
    size_t size;
} e_graphics_opengl_alloc_layout;

static e_result e_graphics_opengl_get_alloc_layout(const e_graphics_config* pConfig, e_graphics_opengl_alloc_layout* pLayout)
{
    E_ASSERT(pConfig != NULL);
    E_ASSERT(pLayout != NULL);

    pLayout->size = E_ALIGN_64(sizeof(e_graphics_opengl));

    return E_SUCCESS;
}

static e_result e_graphics_opengl_alloc_size(void* pUserData, const e_graphics_config* pConfig, size_t* pSize)
{
    e_result result;
    e_graphics_opengl_alloc_layout layout;

    E_ASSERT(pSize   != NULL);
    E_ASSERT(pConfig != NULL);
    E_UNUSED(pUserData);

    result = e_graphics_opengl_get_alloc_layout(pConfig, &layout);
    if (result != E_SUCCESS) {
        return result;
    }

    *pSize = layout.size;
    return E_SUCCESS;
}

static e_result e_graphics_opengl_init(void* pUserData, e_graphics* pGraphics, const e_graphics_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_graphics_opengl* pGraphicsOpenGL = (e_graphics_opengl*)pGraphics;
    e_result result;
    e_graphics_opengl_alloc_layout layout;

    E_ASSERT(pGraphics != NULL);
    E_ASSERT(pConfig   != NULL);
    E_UNUSED(pUserData);

    (void)pAllocationCallbacks;

    result = e_graphics_opengl_get_alloc_layout(pConfig, &layout);
    if (result != E_SUCCESS) {
        return result;
    }

    /* TODO: Implement me. */
    (void)pGraphicsOpenGL;

    return E_SUCCESS;
}

static void e_graphics_opengl_uninit(void* pUserData, e_graphics* pGraphics, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_graphics_opengl* pGraphicsOpenGL = (e_graphics_opengl*)pGraphics;

    E_ASSERT(pGraphics != NULL);
    E_UNUSED(pUserData);

    (void)pAllocationCallbacks;

    /* TODO: Implement me. */
    (void)pGraphicsOpenGL;
}

static e_result e_graphics_opengl_get_devices(void* pUserData, e_graphics* pGraphics, const e_allocation_callbacks* pAllocationCallbacks, size_t* pDeviceCount, e_graphics_device_info* pDeviceInfos)
{
    E_ASSERT(pGraphics    != NULL);
    E_ASSERT(pDeviceCount != NULL);
    E_UNUSED(pUserData);
    E_UNUSED(pAllocationCallbacks);

    /* We're only support a default graphics device. I'm not aware of a way to query any of these details. */
    if (pDeviceInfos != NULL) {
        size_t deviceCap = *pDeviceCount;

        if (deviceCap >= 1) {
            c89str_strcpy_s(pDeviceInfos[0].name, sizeof(pDeviceInfos[0].name), "Default Graphics Device");
            pDeviceInfos[0].id = 0;
            pDeviceInfos[0].families = E_GRAPHICS_DEVICE_FAMILY_FLAG_GRAPHICS;  /* We might be able to query this based on the supported version of OpenGL. */
        }
    }

    *pDeviceCount = 1;
    return E_SUCCESS;
}

static e_result e_graphics_opengl_set_surface(void* pUserData, e_graphics* pGraphics, e_graphics_surface* pSurface)
{
    e_graphics_surface_opengl* pSurfaceOpenGL = (e_graphics_surface_opengl*)pSurface;

    E_ASSERT(pGraphics != NULL);
    E_UNUSED(pUserData);

#if defined(E_WINDOWS)
    e_engine_gl(pSurface->pGraphics->pEngine)->wglMakeCurrent((HDC)e_window_get_platform_object(pSurface->pWindow, E_PLATFORM_OBJECT_WIN32_HDC), pSurfaceOpenGL->platform.win32.hRC);
#endif
#if defined(E_DESKTOP_UNIX)

#endif
#if defined(E_EMSCRIPTEN)
    {
        EGLDisplay displayEGL = (EGLDisplay)e_window_get_platform_object(pSurface->pWindow, E_PLATFORM_OBJECT_EGL_DISPLAY);
        eglMakeCurrent(displayEGL, pSurfaceOpenGL->platform.egl.surface, pSurfaceOpenGL->platform.egl.surface, pSurfaceOpenGL->platform.egl.context);
    }
    
#endif

    return E_SUCCESS;
}

static e_result e_graphics_opengl_present_surface(void* pUserData, e_graphics* pGraphics, e_graphics_surface* pSurface)
{
    E_ASSERT(pGraphics != NULL);
    E_UNUSED(pUserData);

#if defined(E_WINDOWS)
    SwapBuffers((HDC)e_window_get_platform_object(pSurface->pWindow, E_PLATFORM_OBJECT_WIN32_HDC));
#endif
#if defined(E_DESKTOP_UNIX)

#endif
#if defined(E_EMSCRIPTEN)
    {
        EGLDisplay displayEGL = (EGLDisplay)e_window_get_platform_object(pSurface->pWindow, E_PLATFORM_OBJECT_EGL_DISPLAY);
        eglSwapBuffers(displayEGL, ((e_graphics_surface_opengl*)pSurface)->platform.egl.surface);
    }
    
#endif

    return E_SUCCESS;
}



static e_result e_graphics_surface_opengl_alloc_size(void* pUserData, const e_graphics_surface_config* pConfig, size_t* pSize)
{
    E_ASSERT(pConfig != NULL);
    E_ASSERT(pSize   != NULL);
    E_UNUSED(pUserData);

    *pSize = sizeof(e_graphics_surface_opengl);

    return E_SUCCESS;
}

static e_result e_graphics_surface_opengl_init(void* pUserData, e_graphics_surface* pSurface, const e_graphics_surface_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_graphics_surface_opengl* pSurfaceOpenGL = (e_graphics_surface_opengl*)pSurface;

    E_ASSERT(pConfig != NULL);
    E_ASSERT(pSurface     != NULL);
    E_UNUSED(pUserData);

    (void)pAllocationCallbacks;

    /* There's different ways to create a surface depending on the platform. */
    #if defined(E_WINDOWS)
    {
        GLBapi* pGL = e_engine_gl(pSurface->pGraphics->pEngine);
        E_ASSERT(pGL != NULL);

        pSurfaceOpenGL->platform.win32.hRC = pGL->wglCreateContext((HDC)e_window_get_platform_object(pSurface->pWindow, E_PLATFORM_OBJECT_WIN32_HDC));
        if (pSurfaceOpenGL->platform.win32.hRC == NULL) {
            e_log_postf(e_graphics_get_log(pSurface->pGraphics), E_LOG_LEVEL_ERROR, "Failed to create HGLRC.");
            return E_ERROR;
        }

        return E_SUCCESS;
    }
    #endif

    #if defined(E_DESKTOP_UNIX)
    {
        /* TODO: Implement me. */
        return E_RROR;
    }
    #endif

    #if defined(E_EMSCRIPTEN)
    {
        EGLConfig configEGL;
        EGLint configCount;
        EGLint const pConfigAttributesEGL[] =
        {
            EGL_RED_SIZE,   8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE,  8,
            EGL_ALPHA_SIZE, 8,
            EGL_NONE
        };
	    EGLint const pContextConfigEGL[] =
        {
            EGL_CONTEXT_CLIENT_VERSION, 2,  /* Is this the WebGL version, or the GLES version? Assuming when this is run on web it means the WebGL version? How would I make this force GLES 3 on non web platforms? */
            EGL_NONE
        };
        EGLDisplay displayEGL = (EGLDisplay)e_platform_get_object(E_PLATFORM_OBJECT_EGL_DISPLAY);
        EGLContext contextEGL;
        EGLSurface surfaceEGL;

        if (!eglChooseConfig(displayEGL, pConfigAttributesEGL, &configEGL, 1, &configCount)) {
            e_log_postf(e_graphics_get_log(pSurface->pGraphics), E_LOG_LEVEL_ERROR, "Could not find EGL context.");
            return E_ERROR;
        }

        contextEGL = eglCreateContext(displayEGL, configEGL, EGL_NO_CONTEXT, pContextConfigEGL);
        if (contextEGL == NULL) {
            e_log_postf(e_graphics_get_log(pSurface->pGraphics), E_LOG_LEVEL_ERROR, "Failed to create EGL context.");
            return E_ERROR;
        }

        surfaceEGL = eglCreateWindowSurface(displayEGL, configEGL, 0, NULL);
        if (surfaceEGL == NULL) {
            eglDestroyContext(displayEGL, contextEGL);
            e_log_postf(e_graphics_get_log(pSurface->pGraphics), E_LOG_LEVEL_ERROR, "Failed to create EGL window surface.");
            return E_ERROR;
        }

        pSurfaceOpenGL->platform.egl.context = contextEGL;
        pSurfaceOpenGL->platform.egl.surface = surfaceEGL;

        return E_SUCCESS;
    }
    #endif
}

static void e_graphics_surface_opengl_uninit(void* pUserData, e_graphics_surface* pSurface, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_graphics_surface_opengl* pSurfaceOpenGL = (e_graphics_surface_opengl*)pSurface;

    E_ASSERT(pSurface != NULL);
    E_UNUSED(pUserData);

    (void)pAllocationCallbacks;

    /* Check if it's a window render target. If so, it needs to be deleted using platform-specific code. */
    #if defined(E_WINDOWS)
    {
        if (pSurfaceOpenGL->platform.win32.hRC != NULL) {
            e_engine_gl(pSurface->pGraphics->pEngine)->wglDeleteContext(pSurfaceOpenGL->platform.win32.hRC);
        }
    }
    #endif

    #if defined(E_DESKTOP_UNIX)
    {
        /* TODO: Implement me. */
    }
    #endif

    #if defined(E_EMSCRIPTEN)
    {
        EGLDisplay displayEGL = (EGLDisplay)e_platform_get_object(E_PLATFORM_OBJECT_EGL_DISPLAY);
        eglDestroySurface(displayEGL, pSurfaceOpenGL->platform.egl.surface);
        eglDestroySurface(displayEGL, pSurfaceOpenGL->platform.egl.context);
    }
    #endif
}



static e_graphics_vtable e_gGraphicsBackendVTable_OpenGL =
{
    e_graphics_opengl_alloc_size,
    e_graphics_opengl_init,
    e_graphics_opengl_uninit,
    e_graphics_opengl_get_devices,
    e_graphics_opengl_set_surface,
    e_graphics_opengl_present_surface,

    e_graphics_surface_opengl_alloc_size,
    e_graphics_surface_opengl_init,
    e_graphics_surface_opengl_uninit
};
#endif  /* E_NO_OPENGL */


/*
Vulkan backend.
*/
#if !defined(E_NO_VULKAN)
typedef struct e_graphics_vulkan         e_graphics_vulkan;
typedef struct e_graphics_surface_vulkan e_graphics_surface_vulkan;

struct e_graphics_vulkan
{
    e_graphics graphics;    /* Must be the first item so we can cast. */
    VkInstance instanceVK;
    VkDebugUtilsMessengerEXT debugMessengerVK;
    VkbAPI vk;              /* Instance-specific APIs. */
};

struct e_graphics_surface_vulkan
{
    e_graphics_surface rt;
    VkDevice deviceVK;
    VkQueue graphicsQueueVK;    /* At the momemnt we're only using a single graphics queue. Might extend this to have a separate transfer and compute queues later. */
    VkSurfaceKHR surfaceVK;
    VkSwapchainKHR swapchainVK;
    VkSemaphore swapchainSemaphoreVK;   /* For synchronizing swapchain image swaps. */
    uint32_t graphicsQueueFamilyIndex;
    uint32_t currentSwapchainImageIndex;
    VkbAPI vk;                          /* Device-specific APIs. */
};


typedef struct
{
    size_t size;
} e_graphics_vulkan_alloc_layout;

static e_result e_graphics_vulkan_get_alloc_layout(const e_graphics_config* pConfig, e_graphics_vulkan_alloc_layout* pLayout)
{
    E_ASSERT(pConfig != NULL);
    E_ASSERT(pLayout != NULL);

    pLayout->size = E_ALIGN_64(sizeof(e_graphics_vulkan));

    return E_SUCCESS;
}

static e_result e_graphics_vulkan_alloc_size(void* pUserData, const e_graphics_config* pConfig, size_t* pSize)
{
    e_result result;
    e_graphics_vulkan_alloc_layout layout;

    E_ASSERT(pSize   != NULL);
    E_ASSERT(pConfig != NULL);
    E_UNUSED(pUserData);

    result = e_graphics_vulkan_get_alloc_layout(pConfig, &layout);
    if (result != E_SUCCESS) {
        return result;
    }

    *pSize = layout.size;
    return E_SUCCESS;
}



static void* VKAPI_PTR e_graphics_malloc_vk(void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
    E_UNUSED(allocationScope);
    return e_aligned_malloc(size, alignment, (e_allocation_callbacks*)pUserData);
}

static void* VKAPI_PTR e_graphics_realloc_vk(void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope)
{
    E_UNUSED(allocationScope);
    return e_aligned_realloc(pOriginal, size, alignment, (e_allocation_callbacks*)pUserData);
}

static void VKAPI_PTR e_graphics_free_vk(void* pUserData, void* pMemory)
{
    e_aligned_free(pMemory, (e_allocation_callbacks*)pUserData);
}

static VkAllocationCallbacks e_graphics_VkAllocationCallbacks_init(const e_allocation_callbacks* pAllocationCallbacks)
{
    VkAllocationCallbacks allocationCallbacksVK;

    E_ZERO_OBJECT(&allocationCallbacksVK);
    allocationCallbacksVK.pUserData             = (void*)pAllocationCallbacks;
    allocationCallbacksVK.pfnAllocation         = e_graphics_malloc_vk;
    allocationCallbacksVK.pfnReallocation       = e_graphics_realloc_vk;
    allocationCallbacksVK.pfnFree               = e_graphics_free_vk;
    allocationCallbacksVK.pfnInternalAllocation = NULL;
    allocationCallbacksVK.pfnInternalFree       = NULL;

    return allocationCallbacksVK;
}


static e_log_level e_log_level_from_vk(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity)
{
    switch (messageSeverity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: return E_LOG_LEVEL_DEBUG;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    return E_LOG_LEVEL_INFO;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: return E_LOG_LEVEL_WARNING;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   return E_LOG_LEVEL_ERROR;
        default: return E_LOG_LEVEL_ERROR;
    }
}

static VkBool32 VKAPI_PTR e_graphics_vulkan_debug_log_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    e_graphics* pGraphics = (e_graphics*)pUserData;
    const char* pSeverity;
    const char* pType;

    switch (messageSeverity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: pSeverity = "VERBOSE"; break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    pSeverity = "INFO";    break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: pSeverity = "WARNING"; break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   pSeverity = "ERROR";   break;
        default: pSeverity = "ERROR"; break;
    }

    switch (messageTypes)
    {
        case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:                pType = "GENERAL"; break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:             pType = "VALIDATION"; break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:            pType = "PERFORMANCE"; break;
        case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT: pType = "DEVICE ADDRESS BINDING"; break;
        default: pType = "GENERAL"; break;
    }

    e_log_postf(e_graphics_get_log(pGraphics), e_log_level_from_vk(messageSeverity), "[VULKAN] [%s, %s]: %s\n", pSeverity, pType, pCallbackData->pMessage);

    return VK_FALSE;
}


static e_result e_graphics_vulkan_init(void* pUserData, e_graphics* pGraphics, const e_graphics_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_graphics_vulkan* pGraphicsVulkan = (e_graphics_vulkan*)pGraphics;
    e_result result;
    e_graphics_vulkan_alloc_layout layout;
    VkResult resultVK;
    VkAllocationCallbacks allocationCallbacksVK;
    VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo;
    VkInstanceCreateInfo instanceInfo;

    E_ASSERT(pGraphics != NULL);
    E_ASSERT(pConfig   != NULL);
    E_UNUSED(pUserData);

    (void)pAllocationCallbacks;

    result = e_graphics_vulkan_get_alloc_layout(pConfig, &layout);
    if (result != E_SUCCESS) {
        return result;
    }

    allocationCallbacksVK = e_graphics_VkAllocationCallbacks_init(pAllocationCallbacks);


    /*
    We want to set up the VkDebugUtilsMessengerCreateInfoEXT structure before creating the instance. This
    way we can pass it into the `pNext` variable in VkInstanceCreateInfo so that we can capture any issues
    during initialization time.
    */
    debugMessengerInfo.sType           = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugMessengerInfo.pNext           = NULL;
    debugMessengerInfo.flags           = 0;
    debugMessengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
    debugMessengerInfo.messageType     = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugMessengerInfo.pfnUserCallback = e_graphics_vulkan_debug_log_callback;
    debugMessengerInfo.pUserData       = pGraphics;


    /*
    We need a Vulkan instance. These are the layers we'd like, none of which are mandatory.

        VK_LAYER_KHRONOS_validation

    These are the extensions we require. Initialization must fail if we do not have these extensions:

        VK_KHR_surface
        VK_KHR_win32_surface (Windows Only)

    These are the extension that we'd like, but are not mandatory. We don't want initialization to
    fail if these extensions are missing.

        EXT_debug_utils
    */
    {
        const char* pDesiredLayers[] =
        {
            "VK_LAYER_KHRONOS_validation"
        };

        const char* pEnabledLayers[E_COUNTOF(pDesiredLayers)];
        uint32_t enabledLayerCount = 0;


        const char* pRequiredExtensions[] =
        {
            VK_KHR_SURFACE_EXTENSION_NAME,
        #if defined(E_WINDOWS)
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME
        #endif
        };

        const char* pDesiredExtensions[] =
        {
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME
        };

        const char* pEnabledExtensions[E_COUNTOF(pRequiredExtensions) + E_COUNTOF(pDesiredExtensions)];
        uint32_t enabledExtensionCount = 0;

        

        /*
        The parts below are where we determine the layers and extensions to enable. Vulkan will fail
        initialization if any required layer or extension is unavailable so we could just naively add
        our required ones to the list without checking, however I want to expicitly check these so
        we can output information to the log to give us some intelligence as to what's missing.
        */

        /* Layers. */
        {
            VkLayerProperties* pSupportedLayers;
            uint32_t supportedLayerCount;

            enabledLayerCount = 0;

            resultVK = e_engine_vk(pGraphics->pEngine)->vkEnumerateInstanceLayerProperties(&supportedLayerCount, NULL);
            if (resultVK == VK_SUCCESS) {
                pSupportedLayers = (VkLayerProperties*)e_malloc(sizeof(*pSupportedLayers) * supportedLayerCount, pAllocationCallbacks);
                if (pSupportedLayers == NULL) {
                    return E_OUT_OF_MEMORY;
                }

                resultVK = e_engine_vk(pGraphics->pEngine)->vkEnumerateInstanceLayerProperties(&supportedLayerCount, pSupportedLayers);
                if (resultVK == VK_SUCCESS) {
                    /*
                    At this point we have our list of supported layers so we can now compare this against
                    those that we'd like to enable.
                    */
                    uint32_t iDesiredLayer;
                    for (iDesiredLayer = 0; iDesiredLayer < E_COUNTOF(pDesiredLayers); iDesiredLayer += 1) {
                        uint32_t iSupportedLayer;
                        for (iSupportedLayer = 0; iSupportedLayer < supportedLayerCount; iSupportedLayer += 1) {
                            if (strcmp(pDesiredLayers[iDesiredLayer], pSupportedLayers[iSupportedLayer].layerName) == 0) {
                                pEnabledLayers[enabledLayerCount] = pDesiredLayers[iDesiredLayer];
                                enabledLayerCount += 1;
                            }
                        }
                    }
                } else {
                    e_log_postf(e_graphics_get_log(pGraphics), E_LOG_LEVEL_WARNING, "Failed to retrieve Vulkan instance layer properties. vkEnumerateInstanceLayerProperties() returned error code %d.", resultVK);
                }

                e_free(pSupportedLayers, pAllocationCallbacks);
            } else {
                /* This should never be happening. Log a warning. */
                e_log_postf(e_graphics_get_log(pGraphics), E_LOG_LEVEL_WARNING, "Failed to retrieve supported layer count for Vulkan. vkEnumerateInstanceLayerProperties() returned error code %d.", resultVK);
            }
        }

        /* Extensions. */
        {
            VkExtensionProperties* pSupportedExtensions;
            uint32_t supportedExtensionCount;

            enabledExtensionCount = 0;

            resultVK = e_engine_vk(pGraphics->pEngine)->vkEnumerateInstanceExtensionProperties(NULL, &supportedExtensionCount, NULL);
            if (resultVK != VK_SUCCESS) {
                e_log_postf(e_graphics_get_log(pGraphics), E_LOG_LEVEL_ERROR, "Failed to retrieve supported extension count for Vulkan. vkEnumerateInstanceExtensionProperties() returned error code %d.", resultVK);
                return e_result_from_vk(resultVK);
            }

            pSupportedExtensions = (VkExtensionProperties*)e_malloc(sizeof(*pSupportedExtensions) * supportedExtensionCount, pAllocationCallbacks);
            if (pSupportedExtensions == NULL) {
                return E_OUT_OF_MEMORY;
            }

            resultVK = e_engine_vk(pGraphics->pEngine)->vkEnumerateInstanceExtensionProperties(NULL, &supportedExtensionCount, pSupportedExtensions);
            if (resultVK != VK_SUCCESS) {
                e_log_postf(e_graphics_get_log(pGraphics), E_LOG_LEVEL_ERROR, "Failed to retrieve Vulkan extension properties. vkEnumerateInstanceExtensionProperties() returned error code %d.", resultVK);
                return e_result_from_vk(resultVK);
            }

            /*
            At this point we have our list of supported extensions so we can now compare this against
            those that we'd like to enable.
            */
            {
                uint32_t iRequiredExtension;
                uint32_t iDesiredExtension;
                e_bool32 foundAllRequiredExtensions = E_TRUE;

                /*
                For all of our required extensions we always add them to the list of enabled extensions. That
                way if we mess up the logic below it'll be caught by Vulkan itself. We don't want to risk
                accidentally not adding a required extension to our required extensions list.
                */
                for (iRequiredExtension = 0; iRequiredExtension < E_COUNTOF(pRequiredExtensions); iRequiredExtension += 1) {
                    e_bool32 found = E_FALSE;
                    uint32_t iSupportedExtension;

                    /* Always enable the extension regardless of the check below. */
                    pEnabledExtensions[enabledExtensionCount] = pRequiredExtensions[iRequiredExtension];
                    enabledExtensionCount += 1;

                    /*
                    Run through and check if the extension exists so we can log any that don't. The remaining
                    logic in this block is specifically for logging and nothing else.
                    */
                    for (iSupportedExtension = 0; iSupportedExtension < supportedExtensionCount; iSupportedExtension += 1) {
                        if (strcmp(pRequiredExtensions[iRequiredExtension], pSupportedExtensions[iSupportedExtension].extensionName) == 0) {        
                            found = E_TRUE;
                        }
                    }

                    if (!found) {
                        foundAllRequiredExtensions = E_FALSE;
                        e_log_postf(e_graphics_get_log(pGraphics), E_LOG_LEVEL_ERROR, "Required Vulkan extension \"%s\" is not supported.", pRequiredExtensions[iRequiredExtension]);
                    }
                }

                /* Don't bother continuing if we didn't find all of our required extensions. */
                if (!foundAllRequiredExtensions) {
                    e_log_postf(e_graphics_get_log(pGraphics), E_LOG_LEVEL_ERROR, "Failed to initialize Vulkan because not all required extensions are supported.");
                    return E_ERROR;
                }

                /* Optional extensions. */
                for (iDesiredExtension = 0; iDesiredExtension < E_COUNTOF(pDesiredExtensions); iDesiredExtension += 1) {
                    uint32_t iSupportedExtension;
                    for (iSupportedExtension = 0; iSupportedExtension < supportedExtensionCount; iSupportedExtension += 1) {
                        if (strcmp(pDesiredExtensions[iDesiredExtension], pSupportedExtensions[iSupportedExtension].extensionName) == 0) {
                            pEnabledExtensions[enabledExtensionCount] = pDesiredExtensions[iDesiredExtension];
                            enabledExtensionCount += 1;
                        }
                    }
                }
            }

            e_free(pSupportedExtensions, pAllocationCallbacks);
        }


        /*
        At this point we have our layers and extensions that we'd like to enable which means we
        can now go ahead and initialize the Vulkan instance.
        */
        instanceInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pNext                   = &debugMessengerInfo; /* <-- This will allow us to capture messages that happen during initialization of the instance. */
        instanceInfo.flags                   = 0;
        instanceInfo.pApplicationInfo        = NULL;
        instanceInfo.enabledLayerCount       = enabledLayerCount;
        instanceInfo.ppEnabledLayerNames     = pEnabledLayers;
        instanceInfo.enabledExtensionCount   = enabledExtensionCount;
        instanceInfo.ppEnabledExtensionNames = pEnabledExtensions;

        resultVK = e_engine_vk(pGraphics->pEngine)->vkCreateInstance(&instanceInfo, &allocationCallbacksVK, &pGraphicsVulkan->instanceVK);
        if (resultVK != VK_SUCCESS) {
            e_log_postf(e_graphics_get_log(pGraphics), E_LOG_LEVEL_ERROR, "Failed to create Vulkan instance. vkCreateInstance() returned %d.", resultVK);
            return e_result_from_vk(resultVK);
        }

        /*
        At this point we have our instance which means we can retrieve some instance-specific
        function pointers. From here on our we'll want to reference these function pointers,
        unless we're doing something on a per-device level in which case the e_surface object
        will be where we draw the function pointers from.
        */
        pGraphicsVulkan->vk = *e_engine_vk(pGraphics->pEngine);
        resultVK = vkbInitInstanceAPI(pGraphicsVulkan->instanceVK, &pGraphicsVulkan->vk);
        if (resultVK != VK_SUCCESS) {
            e_log_postf(e_graphics_get_log(pGraphics), E_LOG_LEVEL_ERROR, "Failed to retrieve instance-specific Vulkan function pointers.");
            e_engine_vk(pGraphics->pEngine)->vkDestroyInstance(pGraphicsVulkan->instanceVK, &allocationCallbacksVK);
            return e_result_from_vk(resultVK);
        }


        /* Now that the instance has been created we can set up our logging callbacks. */
        resultVK = pGraphicsVulkan->vk.vkCreateDebugUtilsMessengerEXT(pGraphicsVulkan->instanceVK, &debugMessengerInfo, &allocationCallbacksVK, &pGraphicsVulkan->debugMessengerVK);
        if (resultVK != VK_SUCCESS) {
            e_log_postf(e_graphics_get_log(pGraphics), E_LOG_LEVEL_WARNING, "Failed to create Vulkan debug messenger. Vulkan debug logging is disabled.");
        }
    }

    return E_SUCCESS;
}

static void e_graphics_vulkan_uninit(void* pUserData, e_graphics* pGraphics, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_graphics_vulkan* pGraphicsVulkan = (e_graphics_vulkan*)pGraphics;
    VkAllocationCallbacks allocationCallbacksVK;

    E_ASSERT(pGraphics != NULL);
    E_UNUSED(pUserData);

    allocationCallbacksVK = e_graphics_VkAllocationCallbacks_init(pAllocationCallbacks);

    pGraphicsVulkan->vk.vkDestroyInstance(pGraphicsVulkan->instanceVK, &allocationCallbacksVK);
}

static e_result e_graphics_vulkan_get_devices(void* pUserData, e_graphics* pGraphics, const e_allocation_callbacks* pAllocationCallbacks, size_t* pDeviceCount, e_graphics_device_info* pDeviceInfos)
{
    e_graphics_vulkan* pGraphicsVulkan = (e_graphics_vulkan*)pGraphics;
    VkResult resultVK;
    VkPhysicalDevice* pDevicesVK;
    uint32_t deviceCount;

    E_ASSERT(pGraphics    != NULL);
    E_ASSERT(pDeviceCount != NULL);
    E_UNUSED(pUserData);

    resultVK = pGraphicsVulkan->vk.vkEnumeratePhysicalDevices(pGraphicsVulkan->instanceVK, &deviceCount, NULL);
    if (resultVK != VK_SUCCESS) {
        e_log_postf(e_graphics_get_log(pGraphics), E_LOG_LEVEL_ERROR, "Failed to retrieve physical device count. vkEnumeratePhysicalDevices() returned %d.", resultVK);
        return e_result_from_vk(resultVK);
    }

    /* If we're retrieving the actual information we'll need to allocate a buffer so we can ensure we have enough space. */
    if (pDeviceInfos != NULL) {
        size_t deviceCap = *pDeviceCount;
        uint32_t iDevice;

        pDevicesVK = (VkPhysicalDevice*)e_malloc(sizeof(*pDevicesVK) * deviceCap, pAllocationCallbacks);
        if (pDevicesVK == NULL) {
            return E_OUT_OF_MEMORY;
        }

        deviceCount = (uint32_t)deviceCap;
        resultVK = pGraphicsVulkan->vk.vkEnumeratePhysicalDevices(pGraphicsVulkan->instanceVK, &deviceCount, pDevicesVK);
        if (resultVK != VK_SUCCESS) {
            e_log_postf(e_graphics_get_log(pGraphics), E_LOG_LEVEL_ERROR, "Failed to retrieve physical devices. vkEnumeratePhysicalDevices() returned %d.", resultVK);
            e_free(pDevicesVK, pAllocationCallbacks);
            return e_result_from_vk(resultVK);
        }

        /* We have the devices, so now fill out the necessary information. */
        for (iDevice = 0; iDevice < deviceCount; iDevice += 1) {
            VkPhysicalDeviceProperties properties;
            pGraphicsVulkan->vk.vkGetPhysicalDeviceProperties(pDevicesVK[iDevice], &properties);

            c89str_strcpy_s(pDeviceInfos[iDevice].name, sizeof(pDeviceInfos[iDevice].name), properties.deviceName);
            pDeviceInfos[iDevice].id = properties.deviceID;

            /* To determine the family flags with Vulkan we need to look at the queue families. */
            {
                VkQueueFamilyProperties* pQueueFamilyProperties;
                uint32_t queueFamilyCount = 0;

                pDeviceInfos[iDevice].families = 0;

                pGraphicsVulkan->vk.vkGetPhysicalDeviceQueueFamilyProperties(pDevicesVK[iDevice], &queueFamilyCount, NULL);
                if (queueFamilyCount > 0) {
                    uint32_t iQueueFamily;

                    pQueueFamilyProperties = (VkQueueFamilyProperties*)e_malloc(sizeof(*pQueueFamilyProperties) * queueFamilyCount, pAllocationCallbacks);
                    if (pQueueFamilyProperties == NULL) {
                        e_free(pDevicesVK, pAllocationCallbacks);
                        return E_OUT_OF_MEMORY;
                    }

                    pGraphicsVulkan->vk.vkGetPhysicalDeviceQueueFamilyProperties(pDevicesVK[iDevice], &queueFamilyCount, pQueueFamilyProperties);

                    /* At this point we have the queue family information and we can just check the queue flags to determine the families. */
                    for (iQueueFamily = 0; iQueueFamily < queueFamilyCount; iQueueFamily += 1) {
                        if ((pQueueFamilyProperties[iQueueFamily].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
                            pDeviceInfos[iDevice].families |= E_GRAPHICS_DEVICE_FAMILY_FLAG_GRAPHICS;
                        }
                        if ((pQueueFamilyProperties[iQueueFamily].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) {
                            pDeviceInfos[iDevice].families |= E_GRAPHICS_DEVICE_FAMILY_FLAG_COMPUTE;
                        }
                    }
                } else {
                    /* Failed to retrieve any queue families. */
                    pDeviceInfos[iDevice].families = 0;
                }
            }
        }

        e_free(pDevicesVK, pAllocationCallbacks);
    }

    *pDeviceCount = (size_t)deviceCount;
    return E_SUCCESS;
}

static e_result e_graphics_vulkan_set_surface(void* pUserData, e_graphics* pGraphics, e_graphics_surface* pSurface)
{
    e_graphics_surface_vulkan* pSurfaceVulkan = (e_graphics_surface_vulkan*)pSurface;

    E_ASSERT(pGraphics != NULL);
    E_UNUSED(pUserData);

    /* You don't really "set" a surface in Vulkan. */
    E_UNUSED(pSurfaceVulkan);

    return E_SUCCESS;
}

static e_result e_graphics_vulkan_present_surface(void* pUserData, e_graphics* pGraphics, e_graphics_surface* pSurface)
{
    e_graphics_surface_vulkan* pSurfaceVulkan = (e_graphics_surface_vulkan*)pSurface;
    VkResult resultVK;
    VkPresentInfoKHR presentInfo;

    E_ASSERT(pGraphics != NULL);
    E_UNUSED(pUserData);

    presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext              = NULL;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores    = &pSurfaceVulkan->swapchainSemaphoreVK;
    presentInfo.swapchainCount     = 1;
    presentInfo.pSwapchains        = &pSurfaceVulkan->swapchainVK;
    presentInfo.pImageIndices      = &pSurfaceVulkan->currentSwapchainImageIndex;
    presentInfo.pResults           = NULL;
    resultVK = pSurfaceVulkan->vk.vkQueuePresentKHR(pSurfaceVulkan->graphicsQueueVK, &presentInfo);
    if (resultVK != VK_SUCCESS) {
        e_log_postf(e_graphics_get_log(pGraphics), E_LOG_LEVEL_ERROR, "Failed to present Vulkan surface. vkQueuePresentKHR() returned %d.", resultVK);
        return e_result_from_vk(resultVK);
    }

    /*
    Now that the queue has been presented we can acquire the next image. I'm not sure if this is the
    right place for it. Need to check if there's a more efficient place to put this.
    */
    resultVK = pSurfaceVulkan->vk.vkAcquireNextImageKHR(pSurfaceVulkan->deviceVK, pSurfaceVulkan->swapchainVK, UINT64_MAX, pSurfaceVulkan->swapchainSemaphoreVK, VK_NULL_HANDLE, &pSurfaceVulkan->currentSwapchainImageIndex);
    if (resultVK != VK_SUCCESS && resultVK != VK_SUBOPTIMAL_KHR) {
        e_log_postf(e_graphics_get_log(pGraphics), E_LOG_LEVEL_ERROR, "Failed to acquire next swapchain image. vkAcquireNextImageKHR() returned %d.", resultVK);
        return e_result_from_vk(resultVK);
    }

    if (resultVK == VK_SUBOPTIMAL_KHR) {
        e_log_postf(e_graphics_get_log(pGraphics), E_LOG_LEVEL_WARNING, "Suboptimal swapchain image.");
    }

    return E_SUCCESS;
}





static e_result e_graphics_surface_vulkan_alloc_size(void* pUserData, const e_graphics_surface_config* pConfig, size_t* pSize)
{
    E_ASSERT(pConfig != NULL);
    E_ASSERT(pSize   != NULL);
    E_UNUSED(pUserData);

    *pSize = sizeof(e_graphics_surface_vulkan);

    return E_SUCCESS;
}

static e_result e_graphics_surface_vulkan_init_VkDevice(e_graphics_surface* pSurface, const e_graphics_surface_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, VkPhysicalDevice physicalDeviceVK, VkSurfaceKHR surfaceVK)
{
    e_graphics_surface_vulkan* pSurfaceVulkan = (e_graphics_surface_vulkan*)pSurface;
    e_graphics_vulkan* pGraphicsVulkan = (e_graphics_vulkan*)pSurface->pGraphics;
    VkResult resultVK;
    VkAllocationCallbacks allocationCallbacksVK;
    VkQueueFamilyProperties* pQueueFamilyProperties;
    uint32_t queueFamilyCount;
    uint32_t iQueueFamily;
    uint32_t selectedQueueFamilyIndex_Graphics = (uint32_t)-1;
    VkBool32 isSurfaceAndDeviceCompatible;
    VkSurfaceCapabilitiesKHR surfaceCaps;
    VkDevice deviceVK;
    VkQueue graphicsQueueVK;

    E_ASSERT(pConfig != NULL);
    E_ASSERT(pSurface != NULL);
    E_ASSERT(physicalDeviceVK != NULL);
    E_ASSERT(surfaceVK != 0);
    E_UNUSED(pConfig);

    allocationCallbacksVK = e_graphics_VkAllocationCallbacks_init(pAllocationCallbacks);

    /* The first thing to do is find a queue index. We just care about the first one that supports graphics. */
    pGraphicsVulkan->vk.vkGetPhysicalDeviceQueueFamilyProperties(physicalDeviceVK, &queueFamilyCount, NULL);

    pQueueFamilyProperties = (VkQueueFamilyProperties*)e_malloc(sizeof(*pQueueFamilyProperties) * queueFamilyCount, pAllocationCallbacks);
    if (pQueueFamilyProperties == NULL) {
        return E_OUT_OF_MEMORY;
    }

    pGraphicsVulkan->vk.vkGetPhysicalDeviceQueueFamilyProperties(physicalDeviceVK, &queueFamilyCount, pQueueFamilyProperties);
    
    for (iQueueFamily = 0; iQueueFamily < queueFamilyCount; iQueueFamily += 1) {
        if ((pQueueFamilyProperties[iQueueFamily].queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
            selectedQueueFamilyIndex_Graphics = iQueueFamily;
            break;
        }
    }

    e_free(pQueueFamilyProperties, pAllocationCallbacks);
    pQueueFamilyProperties = NULL;

    if (selectedQueueFamilyIndex_Graphics == (uint32_t)-1) {
        e_log_postf(e_graphics_surface_get_log(pSurface), E_LOG_LEVEL_WARNING, "Could not find a queue family supporting graphics.");
        return E_ERROR;
    }

    /* Getting here means we've found an appropriate queue family. We can now check if the surface is compatible with the device. */
    resultVK = pGraphicsVulkan->vk.vkGetPhysicalDeviceSurfaceSupportKHR(physicalDeviceVK, selectedQueueFamilyIndex_Graphics, surfaceVK, &isSurfaceAndDeviceCompatible);
    if (resultVK != VK_SUCCESS) {
        e_log_postf(e_graphics_surface_get_log(pSurface), E_LOG_LEVEL_WARNING, "Surface and device are not compatible.");
        return e_result_from_vk(resultVK);
    }

    /*
    Getting here means the surface and device are compatible. One last compatibility check is to
    ensure the surface supports at least two images so we can do double buffering.
    */
    resultVK = pGraphicsVulkan->vk.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDeviceVK, surfaceVK, &surfaceCaps);
    if (resultVK != VK_SUCCESS) {
        e_log_postf(e_graphics_surface_get_log(pSurface), E_LOG_LEVEL_WARNING, "Failed to retrieve surface capabilities.");
        return e_result_from_vk(resultVK);  /* Failed to retrieve surface caps. Abort. */
    }


    /*
    If we've made it this far it means the surface and the device are compatible. We can now go
    ahead and create the device.
    */
    {
        float pQueuePriorities[1];
        VkDeviceQueueCreateInfo pQueueInfos[1];
        VkPhysicalDeviceFeatures physicalDeviceFeatures;
        const char* pEnabledDeviceExtensions[] = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        uint32_t enabledDeviceExtensionCount = E_COUNTOF(pEnabledDeviceExtensions);
        VkDeviceCreateInfo deviceInfo;

        /*
        Vulkan wants us to specify our queues at initialization time. For now we're only using a single
        graphics queue, but if we wanted to later on do some multithreaded queue construction, or some
        kind of compute stuff, we might want to look at changing this.
        */
        pQueuePriorities[0] = 1;

        pQueueInfos[0].sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        pQueueInfos[0].pNext            = NULL;
        pQueueInfos[0].flags            = 0;
        pQueueInfos[0].queueFamilyIndex = selectedQueueFamilyIndex_Graphics;
        pQueueInfos[0].queueCount       = 1;
        pQueueInfos[0].pQueuePriorities = pQueuePriorities;


        /*
        When initializing a device we need to specify a feature set that we need. For now I'm just keeping
        this simple and enabling anything that is supported by the hardware. It may, however, be better to
        set this properly so that if the device doesn't support something we need it'll fail cleanly.
        */
        pGraphicsVulkan->vk.vkGetPhysicalDeviceFeatures(physicalDeviceVK, &physicalDeviceFeatures);

        deviceInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceInfo.pNext                   = NULL;
        deviceInfo.flags                   = 0;
        deviceInfo.queueCreateInfoCount    = sizeof(pQueueInfos) / sizeof(pQueueInfos[0]);
        deviceInfo.pQueueCreateInfos       = pQueueInfos;
        deviceInfo.enabledLayerCount       = 0;
        deviceInfo.ppEnabledLayerNames     = NULL;
        deviceInfo.enabledExtensionCount   = enabledDeviceExtensionCount;
        deviceInfo.ppEnabledExtensionNames = pEnabledDeviceExtensions;
        deviceInfo.pEnabledFeatures        = &physicalDeviceFeatures;  /* <-- Setting this to NULL is equivalent to disabling all features. */

        resultVK = pGraphicsVulkan->vk.vkCreateDevice(physicalDeviceVK, &deviceInfo, &allocationCallbacksVK, &deviceVK);
        if (resultVK != VK_SUCCESS) {
            e_log_postf(e_graphics_surface_get_log(pSurface), E_LOG_LEVEL_ERROR, "Failed to create Vulkan device. vkCreateDevice() returned %d.", resultVK);
            return e_result_from_vk(resultVK);  /* Failed to create the device object. */
        }

        /* We need device-specific function pointers for Vulkan. */
        pSurfaceVulkan->vk = pGraphicsVulkan->vk;
        resultVK = vkbInitDeviceAPI(deviceVK, &pSurfaceVulkan->vk);
        if (resultVK != VK_SUCCESS) {
            e_log_postf(e_graphics_surface_get_log(pSurface), E_LOG_LEVEL_ERROR, "Failed to retrieve device-specific Vulkan function pointers.");
            return e_result_from_vk(resultVK);
        }

        /* Now that we have a device we can retrieve the queues from there for later use. */
        pSurfaceVulkan->vk.vkGetDeviceQueue(deviceVK, selectedQueueFamilyIndex_Graphics, 0, &graphicsQueueVK);
    }

    pSurfaceVulkan->deviceVK = deviceVK;
    pSurfaceVulkan->graphicsQueueVK = graphicsQueueVK;
    pSurfaceVulkan->graphicsQueueFamilyIndex = selectedQueueFamilyIndex_Graphics;

    return E_SUCCESS;
}

static e_result e_graphics_surface_vulkan_init(void* pUserData, e_graphics_surface* pSurface, const e_graphics_surface_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_graphics_surface_vulkan* pSurfaceVulkan = (e_graphics_surface_vulkan*)pSurface;
    e_graphics_vulkan* pGraphicsVulkan = (e_graphics_vulkan*)pSurface->pGraphics;
    e_result result = E_ERROR;
    VkResult resultVK;
    VkAllocationCallbacks allocationCallbacksVK;

    E_ASSERT(pConfig  != NULL);
    E_ASSERT(pSurface != NULL);
    E_UNUSED(pUserData);

    allocationCallbacksVK = e_graphics_VkAllocationCallbacks_init(pAllocationCallbacks);

    /*
    The surface is a heavy weight object. It includes a surface, swapchain and device. The first
    thing to create is the surface, which is the platform-specific part.
    */
    #if defined(E_WINDOWS)
    {
        VkWin32SurfaceCreateInfoKHR surfaceInfo;
        surfaceInfo.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceInfo.pNext     = NULL;
        surfaceInfo.flags     = 0;
        surfaceInfo.hwnd      = (HWND)e_window_get_platform_object(pSurface->pWindow, E_PLATFORM_OBJECT_WIN32_HWND);
        surfaceInfo.hinstance = (HINSTANCE)GetWindowLongPtr(surfaceInfo.hwnd, GWLP_HINSTANCE);
        
        resultVK = pGraphicsVulkan->vk.vkCreateWin32SurfaceKHR(pGraphicsVulkan->instanceVK, &surfaceInfo, &allocationCallbacksVK, &pSurfaceVulkan->surfaceVK);
        if (resultVK != VK_SUCCESS) {
            e_log_postf(e_graphics_surface_get_log(pSurface), E_LOG_LEVEL_ERROR, "Failed to create Vulkan surface. vkCreateWin32SurfaceKHR() returned %d.", resultVK);
            return e_result_from_vk(resultVK);
        }
    }
    #endif


    /*
    At this point we should be done with the platform-specific stuff. Now that we have our surface
    we can create the device. The device we pick depends on the device ID that we passed into the
    surface config.

    If the requested device ID is non-zero we need to try using the specified device. If this fails
    we need to abort with an error. Otherwise, if the ID is zero, we need to determine which device
    to use ourselves in which case we'll use the first device with a graphics queue which is compatible
    with the surface we just created above. If we can't find a device we just abort.
    */
    {
        VkPhysicalDevice* pPhysicalDevices;
        uint32_t physicalDeviceCount;
        uint32_t selectedDeviceIndex = (uint32_t)-1;    /* Initialize to -1 so we can identify whether or not a device has been selected. */
        uint32_t iPhysicalDevice;

        resultVK = pGraphicsVulkan->vk.vkEnumeratePhysicalDevices(pGraphicsVulkan->instanceVK, &physicalDeviceCount, NULL);
        if (resultVK != VK_SUCCESS) {
            e_log_postf(e_graphics_surface_get_log(pSurface), E_LOG_LEVEL_ERROR, "Failed to create retrieve physical device count. vkEnumeratePhysicalDevices() returned %d.", resultVK);
            result = e_result_from_vk(resultVK);
            goto error0;
        }

        pPhysicalDevices = (VkPhysicalDevice*)e_malloc(sizeof(*pPhysicalDevices) * physicalDeviceCount, pAllocationCallbacks);
        if (pPhysicalDevices == NULL) {
            result = E_OUT_OF_MEMORY;
            goto error0;
        }

        resultVK = pGraphicsVulkan->vk.vkEnumeratePhysicalDevices(pGraphicsVulkan->instanceVK, &physicalDeviceCount, pPhysicalDevices);
        if (resultVK != VK_SUCCESS) {
            e_log_postf(e_graphics_surface_get_log(pSurface), E_LOG_LEVEL_ERROR, "Failed to enumerate physical devices. vkEnumeratePhysicalDevices() returned %d.", resultVK);
            e_free(pPhysicalDevices, pAllocationCallbacks);
            result = e_result_from_vk(resultVK);
            goto error0;
        }

        /* We have our physical device information so now it's time to pick an appropriate device. */
        if (pConfig->deviceID != 0) {
            /* Using a specific device. Match it against the ID. */
            for (iPhysicalDevice = 0; iPhysicalDevice < physicalDeviceCount; iPhysicalDevice += 1) {
                VkPhysicalDeviceProperties properties;
                pGraphicsVulkan->vk.vkGetPhysicalDeviceProperties(pPhysicalDevices[iPhysicalDevice], &properties);

                if (properties.deviceID == pConfig->deviceID) {
                    selectedDeviceIndex = iPhysicalDevice;
                    break;
                }
            }
        }

        if (selectedDeviceIndex != (uint32_t)-1) {
            result = e_graphics_surface_vulkan_init_VkDevice(pSurface, pConfig, pAllocationCallbacks, pPhysicalDevices[selectedDeviceIndex], pSurfaceVulkan->surfaceVK);
            if (result != E_SUCCESS) {
                /* We failed to initialize the device. We should now reset the index so that we trigger the fallback. */
                selectedDeviceIndex = (uint32_t)-1;
            }
        }

        /*
        If at this point we still don't have a device ID we'll need to just try using the first one
        that supports graphics and is compatible with our surface.
        */
        if (selectedDeviceIndex == (uint32_t)-1) {
            if (pConfig->deviceID != 0) {
                /* User has requested a specific device, but that device could not be found. Falling back to defaults. Log a warning to let them know about this. */
                e_log_postf(e_graphics_surface_get_log(pSurface), E_LOG_LEVEL_WARNING, "Specific device requested (id %d), but it could not be found. Falling back to defaults.", pConfig->deviceID);
            }

            for (iPhysicalDevice = 0; iPhysicalDevice < physicalDeviceCount; iPhysicalDevice += 1) {
                VkPhysicalDeviceProperties properties;
                pGraphicsVulkan->vk.vkGetPhysicalDeviceProperties(pPhysicalDevices[iPhysicalDevice], &properties);

                result = e_graphics_surface_vulkan_init_VkDevice(pSurface, pConfig, pAllocationCallbacks, pPhysicalDevices[iPhysicalDevice], pSurfaceVulkan->surfaceVK);
                if (result == E_SUCCESS) {
                    selectedDeviceIndex = iPhysicalDevice;
                    break;
                }
            }

            /* If at this point we *still* don't have a device we need to abort. */
            if (selectedDeviceIndex == (uint32_t)-1) {
                e_log_postf(e_graphics_surface_get_log(pSurface), E_LOG_LEVEL_ERROR, "Could not find appropriate Vulkan device for surface.");
                result = E_ERROR;
                goto error0;
            }
        }

        /* Swapchain. */
        {
            /*
            When creating the swapchain we need to specify the format of the images that make up
            the swapchain. To do this we need to query the supported formats.
            */
            VkFormat pAllowedSurfaceFormats[] =
            {
                VK_FORMAT_R8G8B8A8_UNORM,
                VK_FORMAT_B8G8R8A8_UNORM
            };

            VkSurfaceFormatKHR* pSupportedSurfaceFormats;
            uint32_t supportedSurfaceFormatCount;
            uint32_t selectedSurfaceFormatIndex = (uint32_t)-1; /* Set to -1 so we can identify whether or not a format was picked. */
            uint32_t iSupportedSurfaceFormat;

            resultVK = pGraphicsVulkan->vk.vkGetPhysicalDeviceSurfaceFormatsKHR(pPhysicalDevices[selectedDeviceIndex], pSurfaceVulkan->surfaceVK, &supportedSurfaceFormatCount, NULL);
            if (resultVK != VK_SUCCESS) {
                e_log_postf(e_graphics_surface_get_log(pSurface), E_LOG_LEVEL_ERROR, "Failed to retrieve physical device surface format count. vkGetPhysicalDeviceSurfaceFormatsKHR() returned %d.", resultVK);
                result = e_result_from_vk(resultVK);
                goto error1;
            }

            pSupportedSurfaceFormats = (VkSurfaceFormatKHR*)e_malloc(sizeof(*pSupportedSurfaceFormats) * supportedSurfaceFormatCount, pAllocationCallbacks);
            if (pSupportedSurfaceFormats == NULL) {
                result = E_OUT_OF_MEMORY;
                goto error1;
            }

            resultVK = pGraphicsVulkan->vk.vkGetPhysicalDeviceSurfaceFormatsKHR(pPhysicalDevices[selectedDeviceIndex], pSurfaceVulkan->surfaceVK, &supportedSurfaceFormatCount, pSupportedSurfaceFormats);
            if (resultVK != VK_SUCCESS) {
                e_log_postf(e_graphics_surface_get_log(pSurface), E_LOG_LEVEL_ERROR, "Failed to retrieve physical device surface formats. vkGetPhysicalDeviceSurfaceFormatsKHR() returned %d.", resultVK);
                e_free(pSupportedSurfaceFormats, pAllocationCallbacks);
                result = e_result_from_vk(resultVK);
                goto error1;
            }

            /*
            Now we need to pick our preferred format. I'm going to run with the assumption that the
            formats are listed in the order that the Vulkan implementation prefers. We'll just go
            ahead and pick the first supported format that is in our list of allowed formats.
            */
            for (iSupportedSurfaceFormat = 0; iSupportedSurfaceFormat < supportedSurfaceFormatCount; iSupportedSurfaceFormat += 1) {
                uint32_t iAllowedSurfaceFormat;
                for (iAllowedSurfaceFormat = 0; iAllowedSurfaceFormat < E_COUNTOF(pAllowedSurfaceFormats); iAllowedSurfaceFormat += 1) {
                    if (pSupportedSurfaceFormats[iSupportedSurfaceFormat].format == pAllowedSurfaceFormats[iAllowedSurfaceFormat]) {
                        selectedSurfaceFormatIndex = iSupportedSurfaceFormat;
                        break;
                    }
                }
            }

            /* If we weren't able to find a supported format we'll need to abort. */
            if (selectedSurfaceFormatIndex == (uint32_t)-1) {
                e_log_postf(e_graphics_surface_get_log(pSurface), E_LOG_LEVEL_ERROR, "Could not find an appropriate surface format.");
                e_free(pSupportedSurfaceFormats, pAllocationCallbacks);
                result = E_ERROR;
                goto error1;
            }

            /*
            Getting here means we know what surface format to use. We can now create the swapchain.
            When creating the swapchain, it'll ask us for the size of the images. Since this swapchain
            is going to be associated with the surface we want it to be the same size. To get the
            size we need to retrieve it from surface with vkGetPhysicalDeviceSurfaceCapabilitiesKHR().
            */
            /* TODO: Make this a function so we can easily recreate the swapchain when the window is resized. */
            {
                VkSwapchainCreateInfoKHR swapchainInfo;

                VkSurfaceCapabilitiesKHR surfaceCaps;
                resultVK = pGraphicsVulkan->vk.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pPhysicalDevices[selectedDeviceIndex], pSurfaceVulkan->surfaceVK, &surfaceCaps);
                if (resultVK != VK_SUCCESS) {
                    e_free(pSupportedSurfaceFormats, pAllocationCallbacks);
                    result = e_result_from_vk(resultVK);
                    goto error1;
                }

                swapchainInfo.sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
                swapchainInfo.pNext                 = NULL;
                swapchainInfo.flags                 = 0;
                swapchainInfo.surface               = pSurfaceVulkan->surfaceVK;
                swapchainInfo.minImageCount         = 2;                                    /* Set this to 2 for double buffering. Triple buffering would be 3, etc. */
                swapchainInfo.imageFormat           = pSupportedSurfaceFormats[selectedSurfaceFormatIndex].format;      /* The format we selected earlier. */
                swapchainInfo.imageColorSpace       = pSupportedSurfaceFormats[selectedSurfaceFormatIndex].colorSpace;  /* The color space we selected earlier. */
                swapchainInfo.imageExtent           = surfaceCaps.currentExtent;            /* The size of the images of the swapchain. Keep this the same size as the surface. */
                swapchainInfo.imageArrayLayers      = 1;                                    /* I'm not sure in what situation you would ever want to set this to anything other than 1. */
                swapchainInfo.imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                swapchainInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
                swapchainInfo.queueFamilyIndexCount = 0;                                    /* Only used when imageSharingMode is VK_SHARING_MODE_CONCURRENT. */
                swapchainInfo.pQueueFamilyIndices   = NULL;                                 /* Only used when imageSharingMode is VK_SHARING_MODE_CONCURRENT. */
                swapchainInfo.preTransform          = surfaceCaps.currentTransform;         /* Rotations (90 degree increments) and flips. Just use the current transform from the surface and move on. */
                swapchainInfo.compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
                swapchainInfo.presentMode           = VK_PRESENT_MODE_FIFO_KHR;             /* This is what controls vsync. FIFO must always be supported, so use it by default. */
                swapchainInfo.clipped               = VK_TRUE;                              /* Set this to true if you're only displaying to a window. */
                swapchainInfo.oldSwapchain          = VK_NULL_HANDLE;                       /* You would set this if you're creating a new swapchain to replace an old one, such as when resizing a window. */

                resultVK = pSurfaceVulkan->vk.vkCreateSwapchainKHR(pSurfaceVulkan->deviceVK, &swapchainInfo, NULL, &pSurfaceVulkan->swapchainVK);
                if (result != VK_SUCCESS) {
                    e_log_postf(e_graphics_surface_get_log(pSurface), E_LOG_LEVEL_ERROR, "Failed to create swapchain. vkCreateSwapchainKHR() returned %d.", resultVK);
                    e_free(pSupportedSurfaceFormats, pAllocationCallbacks);
                    result = e_result_from_vk(resultVK);
                    goto error1;
                }
            }

            e_free(pSupportedSurfaceFormats, pAllocationCallbacks);
            pSupportedSurfaceFormats = NULL;

            /*
            When we swap images in the swapchain we need to synchronize them with a semaphore. We'll
            go ahead and create that here.
            */
            {
                VkSemaphoreCreateInfo semaphoreInfo;
                semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
                semaphoreInfo.pNext = 0;
                semaphoreInfo.flags = 0;

                resultVK = pSurfaceVulkan->vk.vkCreateSemaphore(pSurfaceVulkan->deviceVK, &semaphoreInfo, NULL, &pSurfaceVulkan->swapchainSemaphoreVK);
                if (resultVK != VK_SUCCESS) {
                    e_log_postf(e_graphics_surface_get_log(pSurface), E_LOG_LEVEL_ERROR, "Failed to create swapchain semaphore. vkCreateSemaphore() returned %d.", resultVK);
                    result = e_result_from_vk(resultVK);
                    goto error2;
                }
            }

            /*
            Now that we've got the semaphore we can go ahead and acquire the first image. We need to
            do this so we can get the index of the next image in the swapchain.
            */
            resultVK = pSurfaceVulkan->vk.vkAcquireNextImageKHR(pSurfaceVulkan->deviceVK, pSurfaceVulkan->swapchainVK, UINT64_MAX, pSurfaceVulkan->swapchainSemaphoreVK, VK_NULL_HANDLE, &pSurfaceVulkan->currentSwapchainImageIndex);
            if (resultVK != VK_SUCCESS && resultVK != VK_SUBOPTIMAL_KHR) {
                /* TODO: Post an error. */
                result = e_result_from_vk(resultVK);
                goto error3;
            }

            if (resultVK == VK_SUBOPTIMAL_KHR) {
                /* TODO: Post a warning. */
            }
        }
    }

    return E_SUCCESS;

error3:
    pSurfaceVulkan->vk.vkDestroySemaphore(pSurfaceVulkan->deviceVK, pSurfaceVulkan->swapchainSemaphoreVK, &allocationCallbacksVK);
error2:
    pSurfaceVulkan->vk.vkDestroySwapchainKHR(pSurfaceVulkan->deviceVK, pSurfaceVulkan->swapchainVK, &allocationCallbacksVK);
error1:
    pGraphicsVulkan->vk.vkDestroyDevice(pSurfaceVulkan->deviceVK, &allocationCallbacksVK);
error0:
    #if defined(E_WINDOWS)
    {
        pGraphicsVulkan->vk.vkDestroySurfaceKHR(pGraphicsVulkan->instanceVK, pSurfaceVulkan->surfaceVK, &allocationCallbacksVK);
    }
    #endif

    return result;
}

static void e_graphics_surface_vulkan_uninit(void* pUserData, e_graphics_surface* pSurface, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_graphics_surface_vulkan* pSurfaceVulkan = (e_graphics_surface_vulkan*)pSurface;
    e_graphics_vulkan* pGraphicsVulkan = (e_graphics_vulkan*)pSurface->pGraphics;
    VkAllocationCallbacks allocationCallbacksVK;

    E_ASSERT(pSurface != NULL);
    E_UNUSED(pUserData);

    allocationCallbacksVK = e_graphics_VkAllocationCallbacks_init(pAllocationCallbacks);

    pGraphicsVulkan->vk.vkDestroyDevice(pSurfaceVulkan->deviceVK, &allocationCallbacksVK);
    pGraphicsVulkan->vk.vkDestroySurfaceKHR(pGraphicsVulkan->instanceVK, pSurfaceVulkan->surfaceVK, &allocationCallbacksVK);
}

static e_graphics_vtable e_gGraphicsBackendVTable_Vulkan =
{
    e_graphics_vulkan_alloc_size,
    e_graphics_vulkan_init,
    e_graphics_vulkan_uninit,
    e_graphics_vulkan_get_devices,
    e_graphics_vulkan_set_surface,
    e_graphics_vulkan_present_surface,

    e_graphics_surface_vulkan_alloc_size,
    e_graphics_surface_vulkan_init,
    e_graphics_surface_vulkan_uninit
};
#endif


/* Retrieves the VTable of the given stock graphics backend. */
static e_graphics_vtable* e_graphics_get_backend_vtable(e_graphics_backend backend)
{
    switch (backend)
    {
        case E_GRAPHICS_BACKEND_OPENGL:
        {
        #if !defined(E_NO_OPENGL)
            return &e_gGraphicsBackendVTable_OpenGL;
        #else
            return NULL;
        #endif
        }

        case E_GRAPHICS_BACKEND_VULKAN:
        {
        #if !defined(E_NO_VULKAN)
            return &e_gGraphicsBackendVTable_Vulkan;
        #else
            return NULL;
        #endif
        }
        default: break;
    }

    return NULL;
}



E_API e_graphics_config e_graphics_config_init(e_engine* pEngine)
{
    e_graphics_config config;

    E_ZERO_OBJECT(&config);
    config.pEngine = pEngine;
    config.backend = E_GRAPHICS_BACKEND_UNKNOWN;

    return config;
}


E_API e_result e_graphics_alloc_size(const e_graphics_config* pConfig, size_t* pSize)
{
    if (pSize == NULL) {
        return E_INVALID_ARGS;
    }

    *pSize = 0;

    if (pConfig == NULL) {
        return E_INVALID_ARGS;
    }

    *pSize = sizeof(e_client);

    return E_SUCCESS;
}

static e_result e_graphics_init_from_vtable(const e_graphics_config* pConfig, const e_graphics_vtable* pVTable, void* pUserData, const e_allocation_callbacks* pAllocationCallbacks, e_graphics** ppGraphics)
{
    e_result result;
    e_graphics* pGraphics;
    size_t allocationSize;

    E_ASSERT(ppGraphics != NULL);
    E_ASSERT(pConfig    != NULL);
    E_ASSERT(pVTable    != NULL);

    *ppGraphics = NULL;

    if (pVTable->alloc_size == NULL || pVTable->init == NULL) {
        return E_ERROR; /* Necessary callbacks not implemented. */
    }

    result = pVTable->alloc_size(pUserData, pConfig, &allocationSize);
    if (result != E_SUCCESS) {
        return result;  /* Failed to retrieve necessary allocation size. */
    }

    pGraphics = (e_graphics*)e_calloc(allocationSize, pAllocationCallbacks);
    if (pGraphics == NULL) {
        return E_OUT_OF_MEMORY;
    }

    pGraphics->pEngine = pConfig->pEngine;

    result = pVTable->init(pUserData, pGraphics, pConfig, pAllocationCallbacks);
    if (result != E_SUCCESS) {
        e_free(pGraphics, pAllocationCallbacks);
        return result;
    }

    pGraphics->pVTable         = pVTable;
    pGraphics->pVTableUserData = pUserData;

    *ppGraphics = pGraphics;
    return E_SUCCESS;
}

static e_result e_graphics_init_by_backend(const e_graphics_config* pConfig, e_graphics_backend backend, const e_allocation_callbacks* pAllocationCallbacks, e_graphics** ppGraphics)
{
    e_result result;
    e_graphics* pGraphics;
    const e_graphics_vtable* pVTable;
    void* pVTableUserData;

    E_ASSERT(ppGraphics != NULL);
    E_ASSERT(pConfig    != NULL);
    E_ASSERT(backend    != E_GRAPHICS_BACKEND_UNKNOWN);

    *ppGraphics = NULL;

    if (backend == E_GRAPHICS_BACKEND_CUSTOM) {
        pVTable         = pConfig->pVTable;
        pVTableUserData = pConfig->pVTableUserData;
    } else {
        pVTable         = e_graphics_get_backend_vtable(backend);
        pVTableUserData = NULL;
    }

    if (pVTable == NULL) {
        return E_ERROR; /* Couldn't find an appropriate vtable. */
    }

    if (!e_engine_is_graphics_backend_supported(pConfig->pEngine, backend)) {
        return E_ERROR; /* The engine doesn't support this backend. */
    }

    result = e_graphics_init_from_vtable(pConfig, pVTable, pVTableUserData, pAllocationCallbacks, &pGraphics);
    if (result != E_SUCCESS) {
        return result;
    }

    pGraphics->backend = backend;

    *ppGraphics = pGraphics;
    return E_SUCCESS;
}


E_API e_result e_graphics_init(const e_graphics_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_graphics** ppGraphics)
{
    e_result result;
    e_graphics* pGraphics;

    if (ppGraphics == NULL) {
        return E_INVALID_ARGS;
    }

    *ppGraphics = NULL;

    if (pConfig == NULL) {
        return E_INVALID_ARGS;
    }

    if (pConfig->backend == E_GRAPHICS_BACKEND_UNKNOWN) {
        /* We need to enumerate over every backend and try them in order that they're declared in the enum. We might want to make this configurable later. */
        int iGraphicsBackend;
        for (iGraphicsBackend = 1; iGraphicsBackend < E_GRAPHICS_BACKEND_COUNT; iGraphicsBackend += 1) {
            result = e_graphics_init_by_backend(pConfig, (e_graphics_backend)iGraphicsBackend, pAllocationCallbacks, &pGraphics);
            if (result == E_SUCCESS) {
                break;  /* Found one. */
            } else {
                e_log_postf(e_engine_get_log(pConfig->pEngine), E_LOG_LEVEL_WARNING, "Failed to initialize graphics backend %d.", iGraphicsBackend);   /* TODO: Add e_graphics_backend_get_name() and log that instead. */
            }
        }

        /* If we get here and we still don't have a graphics backend we need to abort. */
        if (pGraphics == NULL) {
            e_log_postf(e_engine_get_log(pConfig->pEngine), E_LOG_LEVEL_ERROR, "Failed to initialize graphics sub-system. No backend could be initialized.");
            return E_ERROR;
        }
    } else {
        result = e_graphics_init_by_backend(pConfig, pConfig->backend, pAllocationCallbacks, &pGraphics);
        if (result != E_SUCCESS) {
            e_log_postf(e_engine_get_log(pConfig->pEngine), E_LOG_LEVEL_ERROR, "Failed to initailize graphics backend.");   /* TODO: Log the backend name. Use e_graphics_backend_get_name(). */
            return result;
        }
    }

    /* Getting here means we should have a graphics object. */
    *ppGraphics = pGraphics;
    return E_SUCCESS;
}

E_API void e_graphics_uninit(e_graphics* pGraphics, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pGraphics == NULL) {
        return;
    }

    if (pGraphics->pVTable != NULL && pGraphics->pVTable->uninit != NULL) {
        pGraphics->pVTable->uninit(pGraphics->pUserData, pGraphics, pAllocationCallbacks);
    }

    e_free(pGraphics, pAllocationCallbacks);
}

E_API e_engine* e_graphics_get_engine(const e_graphics* pGraphics)
{
    if (pGraphics == NULL) {
        return NULL;
    }

    return pGraphics->pEngine;
}

E_API e_graphics_backend e_graphics_get_backend(const e_graphics* pGraphics)
{
    if (pGraphics == NULL) {
        return E_GRAPHICS_BACKEND_UNKNOWN;
    }

    return pGraphics->backend;
}

E_API e_log* e_graphics_get_log(const e_graphics* pGraphics)
{
    if (pGraphics == NULL) {
        return NULL;
    }

    return e_engine_get_log(pGraphics->pEngine);
}

E_API e_result e_graphics_get_devices(e_graphics* pGraphics, const e_allocation_callbacks* pAllocationCallbacks, size_t* pDeviceCount, e_graphics_device_info* pDeviceInfos)
{
    e_result result;

    if (pGraphics == NULL || pDeviceCount == NULL) {
        return E_INVALID_ARGS;
    }

    if (pGraphics->pVTable->get_devices == NULL) {
        return E_NOT_IMPLEMENTED;
    }

    result = pGraphics->pVTable->get_devices(pGraphics->pVTableUserData, pGraphics, pAllocationCallbacks, pDeviceCount, pDeviceInfos);
    if (result != E_SUCCESS) {
        return result;
    }

    return E_SUCCESS;
}

E_API e_result e_graphics_set_surface(e_graphics* pGraphics, e_graphics_surface* pSurface)
{
    e_result result;

    if (pGraphics == NULL) {
        return E_INVALID_ARGS;
    }

    if (pGraphics->pVTable->set_surface != NULL) {
        result = pGraphics->pVTable->set_surface(pGraphics->pVTableUserData, pGraphics, pSurface);
        if (result != E_SUCCESS) {
            return result;
        }
    }

    return E_SUCCESS;
}

E_API e_result e_graphics_present_surface(e_graphics* pGraphics, e_graphics_surface* pSurface)
{
    e_result result;

    if (pGraphics == NULL) {
        return E_INVALID_ARGS;
    }

    if (pGraphics->pVTable->present_surface != NULL) {
        result = pGraphics->pVTable->present_surface(pGraphics->pVTableUserData, pGraphics, pSurface);
        if (result != E_SUCCESS) {
            return result;
        }
    }

    return E_SUCCESS;
}



E_API e_graphics_surface_config e_graphics_surface_config_init(e_graphics* pGraphics, e_window* pWindow)
{
    e_graphics_surface_config config;

    E_ZERO_OBJECT(&config);
    config.pGraphics = pGraphics;
    config.pWindow   = pWindow;

    return config;
}


E_API e_result e_graphics_surface_alloc_size(const e_graphics_surface_config* pConfig, size_t* pSize)
{
    e_result result;
    size_t size;

    if (pSize == NULL) {
        return E_INVALID_ARGS;
    }

    *pSize = 0;

    if (pConfig == NULL || pConfig->pGraphics == NULL) {
        return E_INVALID_ARGS;
    }

    if (pConfig->pGraphics->pVTable->surface_alloc_size == NULL) {
        return E_NOT_IMPLEMENTED;
    }

    result = pConfig->pGraphics->pVTable->surface_alloc_size(pConfig->pGraphics->pVTableUserData, pConfig, &size);
    if (result != E_SUCCESS) {
        return result;  /* Failed to retrieve the size of the allocation. */
    }

    *pSize = size;
    return E_SUCCESS;
}

E_API e_result e_graphics_surface_init_preallocated(const e_graphics_surface_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_graphics_surface* pSurface)
{
    e_result result;

    if (pSurface == NULL || pConfig == NULL || pConfig->pGraphics == NULL) {
        return E_INVALID_ARGS;
    }

    pSurface->pGraphics = pConfig->pGraphics;
    pSurface->pWindow   = pConfig->pWindow;

    if (pSurface->pGraphics->pVTable->surface_init == NULL) {
        return E_NOT_IMPLEMENTED;
    }

    result = pSurface->pGraphics->pVTable->surface_init(pSurface->pGraphics->pVTableUserData, pSurface, pConfig, pAllocationCallbacks);
    if (result != E_SUCCESS) {
        return result;
    }

    return E_SUCCESS;
}

E_API e_result e_graphics_surface_init(const e_graphics_surface_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_graphics_surface** ppSurface)
{
    e_result result;
    e_graphics_surface* pSurface;
    size_t allocationSize;

    if (ppSurface == NULL) {
        return E_INVALID_ARGS;
    }

    *ppSurface = NULL;

    if (pConfig == NULL) {
        return E_INVALID_ARGS;
    }

    result = pConfig->pGraphics->pVTable->surface_alloc_size(pConfig->pGraphics->pVTableUserData, pConfig, &allocationSize);
    if (result != E_SUCCESS) {
        return result;
    }

    E_ASSERT(allocationSize >= sizeof(e_graphics_surface));

    pSurface = (e_graphics_surface*)e_calloc(allocationSize, pAllocationCallbacks);
    if (pSurface == NULL) {
        return E_OUT_OF_MEMORY;
    }

    result = e_graphics_surface_init_preallocated(pConfig, pAllocationCallbacks, pSurface);
    if (result != E_SUCCESS) {
        e_log_postf(e_graphics_get_log(pConfig->pGraphics), E_LOG_LEVEL_ERROR, "Failed to initialize graphics surface.");
        e_free(pSurface, pAllocationCallbacks);
        return result;
    }

    pSurface->freeOnUninit = E_TRUE;

    *ppSurface = pSurface;
    return E_SUCCESS;
}

E_API void e_graphics_surface_uninit(e_graphics_surface* pSurface, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pSurface == NULL) {
        return;
    }

    if (pSurface->pGraphics->pVTable->surface_uninit != NULL) {
        pSurface->pGraphics->pVTable->surface_uninit(pSurface->pGraphics->pVTableUserData, pSurface, pAllocationCallbacks);
    }

    if (pSurface->freeOnUninit) {
        e_free(pSurface, pAllocationCallbacks);
    }
}

E_API e_log* e_graphics_surface_get_log(e_graphics_surface* pSurface)
{
    if (pSurface == NULL) {
        return NULL;
    }

    return e_graphics_get_log(pSurface->pGraphics);
}
/* ==== END e_graphics.c ==== */




/* ==== BEG e_client.c ==== */
static e_result e_client_window_event_callback(void* pUserData, e_window* pWindow, e_window_event* pEvent)
{
    (void)pUserData;
    (void)pWindow;
    (void)pEvent;

    return E_SUCCESS;
}

static e_window_vtable e_gClientWindowVTable =
{
    e_client_window_event_callback
};



E_API e_client_config e_client_config_init(e_engine* pEngine, const char* pConfigFileSection)
{
    e_client_config config;

    E_ZERO_OBJECT(&config);
    config.pEngine = pEngine;
    config.pConfigFileSection = pConfigFileSection;    

    return config;
}


typedef struct
{
    size_t size;
    size_t windowOffset;
} e_client_alloc_layout;

static e_result e_client_get_alloc_layout(const e_client_config* pConfig, e_client_alloc_layout* pLayout)
{
    E_ASSERT(pConfig != NULL);
    E_ASSERT(pLayout != NULL);

    pLayout->size  = E_ALIGN_64(sizeof(e_client));

    pLayout->windowOffset = pLayout->size;
    pLayout->size += E_ALIGN_64(sizeof(e_client));

    return E_SUCCESS;
}

E_API e_result e_client_alloc_size(const e_client_config* pConfig, size_t* pSize)
{
    e_result result;
    e_client_alloc_layout layout;

    if (pSize == NULL) {
        return E_INVALID_ARGS;
    }

    *pSize = 0;

    if (pConfig == NULL) {
        return E_INVALID_ARGS;
    }

    result = e_client_get_alloc_layout(pConfig, &layout);
    if (result != E_SUCCESS) {
        return result;
    }

    *pSize = layout.size;
    return E_SUCCESS;
}

E_API e_result e_client_init_preallocated(const e_client_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_client* pClient)
{
    e_result result;
    e_client_alloc_layout layout;
    e_window_config windowConfig;

    if (pClient == NULL || pConfig == NULL) {
        return E_INVALID_ARGS;
    }

    result = e_client_get_alloc_layout(pConfig, &layout);
    if (result != E_SUCCESS) {
        return result;
    }

    pClient->pEngine         = pConfig->pEngine;
    pClient->pUserData       = pConfig->pUserData;
    pClient->pVTable         = pConfig->pVTable;
    pClient->pVTableUserData = pConfig->pVTableUserData;
    pClient->flags           = pConfig->flags;
    pClient->pWindow         = NULL;  /* Will be initialized below. */
    pClient->pConfigSection  = pConfig->pConfigFileSection;

    /* If we don't have a window, we don't have any graphics either. */
    if ((pClient->flags & E_CLIENT_FLAG_NO_WINDOW) != 0) {
        pClient->flags |= E_CLIENT_FLAG_NO_GRAPHICS;
    }

    /* The window need to be initialized. This has been preallocated and is sitting at the end of the pClient object. */
    if ((pClient->flags & E_CLIENT_FLAG_NO_WINDOW) == 0) {
        unsigned int resolutionX;
        unsigned int resolutionY;
        const char* pTitle;
        char* pTitleFromConfig = NULL;

        pClient->pWindow = (e_window*)E_OFFSET_PTR(pClient, layout.windowOffset);

        pTitle = pConfig->pWindowTitle;
        if (pTitle == NULL) {
            if (e_config_file_get_string(e_engine_get_config_file(pClient->pEngine), pClient->pConfigSection, "title", pAllocationCallbacks, &pTitleFromConfig) == E_SUCCESS) {
                pTitle = pTitleFromConfig;
            } else {
                pTitle = "Unnamed Client";
            }
        }

        resolutionX = pConfig->resolutionX;
        if (resolutionX == 0) {
            unsigned int valueFromConfig;
            if (e_config_file_get_uint(e_engine_get_config_file(pClient->pEngine), pClient->pConfigSection, "display.resolutionX", &valueFromConfig) == E_SUCCESS) {
                resolutionX = valueFromConfig;
            } else {
                resolutionX = E_DEFAULT_RESOLUTION_X;
            }
        }

        resolutionY = pConfig->resolutionY;
        if (resolutionY == 0) {
            unsigned int valueFromConfig;
            if (e_config_file_get_uint(e_engine_get_config_file(pClient->pEngine), pClient->pConfigSection, "display.resolutionY", &valueFromConfig) == E_SUCCESS) {
                resolutionY = valueFromConfig;
            } else {
                resolutionY = E_DEFAULT_RESOLUTION_Y;
            }
        }


        windowConfig = e_window_config_init(pClient->pEngine, pTitle, 0, 0, resolutionX, resolutionY, E_WINDOW_FLAG_OPENGL, &e_gClientWindowVTable);

        result = e_window_init_preallocated(&windowConfig, pAllocationCallbacks, pClient->pWindow);
        if (result != E_SUCCESS) {
            e_log_postf(e_engine_get_log(pConfig->pEngine), E_LOG_LEVEL_ERROR, "Failed to initialize client window.");
            return result;
        }

        if (pTitleFromConfig != NULL) {
            e_free(pTitleFromConfig, pAllocationCallbacks);
        }
    }

    if ((pClient->flags & E_CLIENT_FLAG_NO_GRAPHICS) == 0) {
        e_graphics_config graphicsConfig;
        e_graphics_surface_config surfaceConfig;

        E_ASSERT(pClient->pWindow != NULL); /* We should never be getting here if we don't have a window. */

        graphicsConfig = e_graphics_config_init(pConfig->pEngine);
        graphicsConfig.backend = pConfig->graphicsBackend;

        result = e_graphics_init(&graphicsConfig, pAllocationCallbacks, &pClient->pGraphics);
        if (result != E_SUCCESS) {
            e_log_postf(e_engine_get_log(pConfig->pEngine), E_LOG_LEVEL_ERROR, "Failed to initialize graphics sub-system for client.");
            return result;
        }

        /* We need a surface to connect our window to the graphics system. */
        surfaceConfig = e_graphics_surface_config_init(pClient->pGraphics, pClient->pWindow);

        result = e_graphics_surface_init(&surfaceConfig, pAllocationCallbacks, &pClient->pWindowRT);
        if (result != E_SUCCESS) {
            e_log_postf(e_engine_get_log(pConfig->pEngine), E_LOG_LEVEL_ERROR, "Failed to initialize graphics surface for client.");
            e_graphics_uninit(pClient->pGraphics, pAllocationCallbacks);
            return result;
        }
    }

    return E_SUCCESS;
}

E_API e_result e_client_init(const e_client_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_client** ppClient)
{
    e_result result;
    e_client* pClient;
    size_t allocationSize;

    if (ppClient == NULL) {
        return E_INVALID_ARGS;
    }

    *ppClient = NULL;

    if (pConfig == NULL) {
        return E_INVALID_ARGS;
    }

    result = e_client_alloc_size(pConfig, &allocationSize);
    if (result != E_SUCCESS) {
        return result;
    }

    E_ASSERT(allocationSize >= sizeof(e_client));

    pClient = (e_client*)e_calloc(allocationSize, pAllocationCallbacks);
    if (pClient == NULL) {
        return E_OUT_OF_MEMORY;
    }

    result = e_client_init_preallocated(pConfig, pAllocationCallbacks, pClient);
    if (result != E_SUCCESS) {
        e_free(pClient, pAllocationCallbacks);
        return result;
    }

    pClient->freeOnUninit = E_TRUE;

    *ppClient = pClient;
    return E_SUCCESS;
}

E_API void e_client_uninit(e_client* pClient, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pClient == NULL) {
        return;
    }

    e_window_uninit(pClient->pWindow, pAllocationCallbacks);

    if (pClient->freeOnUninit) {
        e_free(pClient, pAllocationCallbacks);
    }
}

E_API e_engine* e_client_get_engine(e_client* pClient)
{
    if (pClient == NULL) {
        return NULL;
    }

    return pClient->pEngine;
}

E_API e_log* e_client_get_log(e_client* pClient)
{
    if (pClient == NULL) {
        return NULL;
    }

    return e_engine_get_log(pClient->pEngine);
}


static e_result e_client_step_default(void* pUserData, e_client* pClient, double dt)
{
    E_UNUSED(pUserData);

    (void)dt;

    /* Draw stuff. */
    e_graphics_set_surface(pClient->pGraphics, pClient->pWindowRT);
    {
        glClearColor(0.5f, 0.5f, 1.0f, 0);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    e_graphics_present_surface(pClient->pGraphics, pClient->pWindowRT);

    return E_SUCCESS;
}

E_API e_result e_client_step(e_client* pClient, double dt)
{
    e_result result;

    if (pClient == NULL) {
        return E_INVALID_ARGS;
    }

    if (pClient->pVTable != NULL && pClient->pVTable->onStep != NULL) {
        result = pClient->pVTable->onStep(pClient->pVTableUserData, pClient, dt);
    } else {
        result = e_client_step_default(NULL, pClient, dt);
    }

    if (result != E_SUCCESS) {
        return result;
    }

    return E_SUCCESS;
}
/* ==== END e_client.c ==== */




/* ==== BEG e_editor.c ==== */
E_API e_editor_config e_editor_config_init(e_engine* pEngine)
{
    e_editor_config config;

    E_ZERO_OBJECT(&config);
    config.pEngine = pEngine;

    return config;
}


E_API e_result e_editor_init(const e_editor_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_editor** ppEditor)
{
    (void)pConfig;
    (void)pAllocationCallbacks;
    (void)ppEditor;

    return E_SUCCESS;
}

E_API void e_editor_uninit(e_editor* pEditor, const e_allocation_callbacks* pAllocationCallbacks)
{
    (void)pEditor;
    (void)pAllocationCallbacks;
}

E_API e_result e_editor_run(e_editor* pEditor)
{
    (void)pEditor;

    return E_SUCCESS;
}

E_API e_result e_editor_show(e_editor* pEditor)
{
    (void)pEditor;

    return E_SUCCESS;
}

E_API e_result e_editor_hide(e_editor* pEditor)
{
    (void)pEditor;

    return E_SUCCESS;
}
/* ==== END e_editor.c ==== */


#endif  /* e_c */