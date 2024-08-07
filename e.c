#ifndef e_c
#define e_c

#include "e.h"

#include <stdlib.h>
#include <string.h> /* For memset(). */
#include <assert.h>
#include <math.h>

#include <stdio.h>  /* For printing to stdout. */

#if defined(_WIN32)
#else
#include <fcntl.h>
#endif


/* No Vulkan with Emscripten. */
#if defined(E_EMSCRIPTEN)
    #ifndef E_NO_VULKAN
    #define E_NO_VULKAN
    #endif
#endif

/* Temporary external dependencies. Eventually these will all be amalgamated. */
#ifndef E_NO_OPENGL
    /* Don't use glbind with Emscripten. We'll use the GLES3 headers directly with Emscripten. Including glbind.h will result in redefinition errors. */
    #ifndef E_EMSCRIPTEN
        #define GLBIND_IMPLEMENTATION
        #define GLBIND_NO_XLIB_HEADERS
        #include "external/glbind/glbind.h"
    #endif
#endif

#ifndef E_NO_VULKAN
    #if defined(E_WIN32)
    #define VK_USE_PLATFORM_WIN32_KHR
    #endif

    /* TODO: We need to somehow make this configurable. Not all POSIX systems will necessarily be using X. */
    #if defined(E_POSIX)
    #define VK_USE_PLATFORM_XLIB_KHR
    #endif

    #define VKBIND_IMPLEMENTATION
    #define VKBIND_NO_XLIB_HEADERS    
    #define VKBIND_NO_GLOBAL_API
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

#if defined(_MSC_VER) && !defined(__clang__)
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
#elif defined(__clang__) || defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic" /* Too many pedantic errors with Lua. Just disable them. */
#endif
#if defined(E_POSIX)
    #define LUA_USE_POSIX
#endif
/*#define LUA_USE_C89*/ /* If we enable LUA_USE_C89 we will not get support for 64-bit integers. */
#define LUA_IMPL
#include "external/minilua/minilua.h"
#if defined(_MSC_VER)
    #pragma warning(push)
#elif defined(__clang__) || defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

#define CGLTF_IMPLEMENTATION
#include "external/cgltf/cgltf.h"

#ifndef E_NO_STB_IMAGE
#ifndef E_NO_STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#endif
#include "external/stb/stb_image.h"
#endif

#ifndef E_NO_STB_RECT_PACK_IMPLEMENTATION
#define STB_RECT_PACK_IMPLEMENTATION
#endif
#include "external/stb/stb_rect_pack.h"

#ifndef E_NO_STB_TRUETYPE_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#endif
#include "external/stb/stb_truetype.h"

#ifndef E_DEFAULT_CONFIF_FILE_PATH
#define E_DEFAULT_CONFIG_FILE_PATH  "config.lua"
#endif

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

#define E_READ_LE16(p) ((e_uint32)(((const e_uint8*)(p))[0]) | ((e_uint32)(((const e_uint8*)(p))[1]) << 8U))
#define E_READ_LE32(p) ((e_uint32)(((const e_uint8*)(p))[0]) | ((e_uint32)(((const e_uint8*)(p))[1]) << 8U) | ((e_uint32)(((const e_uint8*)(p))[2]) << 16U) | ((e_uint32)(((const e_uint8*)(p))[3]) << 24U))
#define E_READ_LE64(p) ((e_uint64)(((const e_uint8*)(p))[0]) | ((e_uint64)(((const e_uint8*)(p))[1]) << 8U) | ((e_uint64)(((const e_uint8*)(p))[2]) << 16U) | ((e_uint64)(((const e_uint8*)(p))[3]) << 24U) | ((e_uint64)(((const e_uint8*)(p))[4]) << 32U) | ((e_uint64)(((const e_uint8*)(p))[5]) << 40U) | ((e_uint64)(((const e_uint8*)(p))[6]) << 48U) | ((e_uint64)(((const e_uint8*)(p))[7]) << 56U))


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



E_API int e_vsnprintf(char* buf, size_t count, const char* fmt, va_list args)
{
    return c89str_vsnprintf(buf, count, fmt, args);
}

E_API int e_snprintf(char* buf, size_t count, const char* fmt, ...)
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


#define e_round_up_to_nearest_4(value)  ((value +  3) & ~3)
#define e_round_up_to_nearest_16(value) ((value + 15) & ~15)


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


E_API e_allocation_callbacks e_allocation_callbacks_init_default(void)
{
    e_allocation_callbacks allocationCallbacks;

    allocationCallbacks.pUserData = NULL;
    allocationCallbacks.onMalloc  = e_malloc_default;
    allocationCallbacks.onRealloc = e_realloc_default;
    allocationCallbacks.onFree    = e_free_default;

    return allocationCallbacks;
}

static e_allocation_callbacks e_allocation_callbacks_init_copy(const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pAllocationCallbacks != NULL) {
        return *pAllocationCallbacks;
    } else {
        return e_allocation_callbacks_init_default();
    }
}

static c89str_allocation_callbacks e_allocation_callbacks_to_c89str(const e_allocation_callbacks* pAllocationCallbacks)
{
    c89str_allocation_callbacks allocationCallbacks;

    if (pAllocationCallbacks != NULL) {
        allocationCallbacks.pUserData = pAllocationCallbacks->pUserData;
        allocationCallbacks.onMalloc  = pAllocationCallbacks->onMalloc;
        allocationCallbacks.onRealloc = pAllocationCallbacks->onRealloc;
        allocationCallbacks.onFree    = pAllocationCallbacks->onFree;
    } else {
        allocationCallbacks.pUserData = NULL;
        allocationCallbacks.onMalloc  = e_malloc_default;
        allocationCallbacks.onRealloc = e_realloc_default;
        allocationCallbacks.onFree    = e_free_default;
    }

    return allocationCallbacks;
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




static e_result e_window_handle_event(e_window* pWindow, e_event* pEvent)
{
    e_result result;

    E_ASSERT(pEvent != NULL);
    E_ASSERT(pWindow != NULL);

    if (pWindow->pVTable == NULL || pWindow->pVTable->onEvent == NULL) {
        result = E_NOT_IMPLEMENTED;
    } else {
        result = pWindow->pVTable->onEvent(pWindow->pVTableUserData, pWindow, pEvent);
    }

    if (result == E_NOT_IMPLEMENTED) {
        return e_window_default_event_handler(pWindow, pEvent);
    } else {
        return result;
    }
}

static e_event e_window_event_init(e_event_type type, e_window* pWindow)
{
    e_event e;

    e.type = type;
    e.data.any.pWindow = pWindow;

    return e;
}


/*
Platform-Specific Section
*/
static e_result e_platform_init(void);
static e_result e_platform_uninit(void);
static void* e_platform_get_object(e_platform_object_type type);

static size_t e_platform_window_sizeof(void);
static e_result e_platform_window_init_preallocated(const e_window_config* pConfig, e_window* pOwnerWindow, const e_allocation_callbacks* pAllocationCallbacks, e_platform_window* pWindow);
static e_result e_platform_window_uninit(e_platform_window* pWindow, const e_allocation_callbacks* pAllocationCallbacks);
static void* e_platform_window_get_object(const e_platform_window* pWindow, e_platform_object_type type);    /* Return null when an internal object is not supported. */
static e_result e_platform_window_set_size(e_platform_window* pWindow, unsigned int sizeX, unsigned int sizeY);
static e_result e_platform_window_capture_cursor(e_platform_window* pWindow);
static e_result e_platform_window_release_cursor(e_platform_window* pWindow);
static e_result e_platform_window_set_cursor_position(e_platform_window* pWindow, int cursorPosX, int cursorPosY);
static e_result e_platform_window_get_cursor_position(e_platform_window* pWindow, int* pCursorPosX, int* pCursorPosY);
static e_result e_platform_window_show_cursor(e_platform_window* pWindow);
static e_result e_platform_window_hide_cursor(e_platform_window* pWindow);
static e_result e_platform_window_pin_cursor(e_platform_window* pWindow, int cursorPosX, int cursorPosY);
static e_result e_platform_window_unpin_cursor(e_platform_window* pWindow);
static e_result e_platform_window_post_close_event(e_platform_window* pWindow);
static e_result e_platform_window_next_buffer(e_platform_window* pWindow, unsigned int bufferSizeX, unsigned int bufferSizeY, e_window_buffer* pBuffer);

typedef e_result (* e_platform_main_loop_iteration_callback)(void* pUserData);
static e_result e_platform_main_loop(int* pExitCode, e_platform_main_loop_iteration_callback iterationCallback, void* pUserData);
static e_result e_platform_exit_main_loop(int exitCode);
static e_result e_platform_wake_main_loop(void);    /* This is used to wake the main loop from a blocking state. You should never normally need to use this. */
static e_result e_platform_set_main_loop_blocking(e_bool32 blocking);


#if defined(E_WIN32)
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

/* Need to do runtime linking of Gdi32.dll. */
HMODULE hGdi32DLL = NULL;
typedef int  (__stdcall * E_PFN_ChoosePixelFormat)(HDC hdc, const PIXELFORMATDESCRIPTOR *ppfd);
typedef BOOL (__stdcall * E_PFN_SetPixelFormat)   (HDC hdc, int format, const PIXELFORMATDESCRIPTOR *ppfd);
typedef BOOL (__stdcall * E_PFN_SwapBuffers)      (HDC hdc);

static E_PFN_ChoosePixelFormat  e_ChoosePixelFormat = NULL;
static E_PFN_SetPixelFormat     e_SetPixelFormat    = NULL;
static E_PFN_SwapBuffers        e_SwapBuffers       = NULL;

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
    do this, we just tell the operating system that it shouldn't do DPI scaling and that we'll do it ourselves
    manually.
    */
    e_make_dpi_aware_win32();


    /* Need to do runtime linking of ChoosePixelFormat, SetPixelFormat and SwapBuffers. */
    hGdi32DLL = LoadLibraryW(L"gdi32.dll");
    if (hGdi32DLL == NULL) {
        return E_ERROR;
    }

    e_ChoosePixelFormat = (E_PFN_ChoosePixelFormat)GetProcAddress(hGdi32DLL, "ChoosePixelFormat");
    e_SetPixelFormat    = (E_PFN_SetPixelFormat)   GetProcAddress(hGdi32DLL, "SetPixelFormat");
    e_SwapBuffers       = (E_PFN_SwapBuffers)      GetProcAddress(hGdi32DLL, "SwapBuffers");


    /* Default window class. */
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

    if (hGdi32DLL != NULL) {
        FreeLibrary(hGdi32DLL);
        hGdi32DLL = NULL;
    }

    return E_SUCCESS;
}

#if 0
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
#endif


static e_uint32 e_key_from_win32(unsigned int key)
{
    switch (key)
    {
        case VK_BACK:       return E_KEY_BACKSPACE;
        case VK_TAB:        return E_KEY_TAB;
        case VK_CLEAR:      return 0x0C;
        case VK_RETURN:     return E_KEY_ENTER;
        case VK_LSHIFT:     return E_KEY_LSHIFT;
        case VK_RSHIFT:     return E_KEY_RSHIFT;
        case VK_LCONTROL:   return E_KEY_LCONTROL;
        case VK_RCONTROL:   return E_KEY_RCONTROL;
        case VK_LMENU:      return E_KEY_LALT;
        case VK_RMENU:      return E_KEY_RALT;
        case VK_SHIFT:      return E_KEY_LSHIFT;
        case VK_CONTROL:    return E_KEY_LCONTROL;
        case VK_MENU:       return E_KEY_LALT;
        case VK_PAUSE:      return E_KEY_PAUSE;
        case VK_CAPITAL:    return E_KEY_CAPS_LOCK;
        case VK_ESCAPE:     return E_KEY_ESCAPE;
        case VK_SPACE:      return E_KEY_SPACE;
        case VK_PRIOR:      return E_KEY_PAGE_UP;
        case VK_NEXT:       return E_KEY_PAGE_DOWN;
        case VK_END:        return E_KEY_END;
        case VK_HOME:       return E_KEY_HOME;
        case VK_LEFT:       return E_KEY_LEFT;
        case VK_UP:         return E_KEY_UP;
        case VK_RIGHT:      return E_KEY_RIGHT;
        case VK_DOWN:       return E_KEY_DOWN;
        case VK_SNAPSHOT:   return E_KEY_PRINT_SCREEN;
        case VK_INSERT:     return E_KEY_INSERT;
        case VK_DELETE:     return E_KEY_DELETE;
        case VK_HELP:       return 0x2F;
        case VK_LWIN:       return E_KEY_LWIN;
        case VK_RWIN:       return E_KEY_RWIN;
        case VK_APPS:       return 0x5D;
        case VK_SLEEP:      return 0x5F;
        case VK_NUMPAD0:    return E_KEY_NUMPAD_0;
        case VK_NUMPAD1:    return E_KEY_NUMPAD_1;
        case VK_NUMPAD2:    return E_KEY_NUMPAD_2;
        case VK_NUMPAD3:    return E_KEY_NUMPAD_3;
        case VK_NUMPAD4:    return E_KEY_NUMPAD_4;
        case VK_NUMPAD5:    return E_KEY_NUMPAD_5;
        case VK_NUMPAD6:    return E_KEY_NUMPAD_6;
        case VK_NUMPAD7:    return E_KEY_NUMPAD_7;
        case VK_NUMPAD8:    return E_KEY_NUMPAD_8;
        case VK_NUMPAD9:    return E_KEY_NUMPAD_9;
        case VK_MULTIPLY:   return E_KEY_MULTIPLY;
        case VK_ADD:        return E_KEY_ADD;
        case VK_SEPARATOR:  return 0x6C;
        case VK_SUBTRACT:   return E_KEY_SUBTRACT;
        case VK_DECIMAL:    return E_KEY_DECIMAL;
        case VK_DIVIDE:     return E_KEY_DIVIDE;
        case VK_F1:         return E_KEY_F1;
        case VK_F2:         return E_KEY_F2;
        case VK_F3:         return E_KEY_F3;
        case VK_F4:         return E_KEY_F4;
        case VK_F5:         return E_KEY_F5;
        case VK_F6:         return E_KEY_F6;
        case VK_F7:         return E_KEY_F7;
        case VK_F8:         return E_KEY_F8;
        case VK_F9:         return E_KEY_F9;
        case VK_F10:        return E_KEY_F10;
        case VK_F11:        return E_KEY_F11;
        case VK_F12:        return E_KEY_F12;
        case VK_NUMLOCK:    return E_KEY_NUM_LOCK;
        case VK_SCROLL:     return E_KEY_SCROLL_LOCK;
        case VK_OEM_1:      return E_KEY_SEMICOLON;
        case VK_OEM_PLUS:   return E_KEY_EQUAL;
        case VK_OEM_COMMA:  return E_KEY_COMMA;
        case VK_OEM_MINUS:  return E_KEY_MINUS;
        case VK_OEM_PERIOD: return E_KEY_PERIOD;
        case VK_OEM_2:      return E_KEY_SLASH;
        case VK_OEM_3:      return E_KEY_BACKTICK;
        case VK_OEM_4:      return E_KEY_LEFT_BRACKET;
        case VK_OEM_5:      return E_KEY_BACKSLASH;
        case VK_OEM_6:      return E_KEY_RIGHT_BRACKET;
        case VK_OEM_7:      return E_KEY_QUOTE;
        case '0':           return E_KEY_0;
        case '1':           return E_KEY_1;
        case '2':           return E_KEY_2;
        case '3':           return E_KEY_3;
        case '4':           return E_KEY_4;
        case '5':           return E_KEY_5;
        case '6':           return E_KEY_6;
        case '7':           return E_KEY_7;
        case '8':           return E_KEY_8;
        case '9':           return E_KEY_9;
        case 'A':           return E_KEY_A;
        case 'B':           return E_KEY_B;
        case 'C':           return E_KEY_C;
        case 'D':           return E_KEY_D;
        case 'E':           return E_KEY_E;
        case 'F':           return E_KEY_F;
        case 'G':           return E_KEY_G;
        case 'H':           return E_KEY_H;
        case 'I':           return E_KEY_I;
        case 'J':           return E_KEY_J;
        case 'K':           return E_KEY_K;
        case 'L':           return E_KEY_L;
        case 'M':           return E_KEY_M;
        case 'N':           return E_KEY_N;
        case 'O':           return E_KEY_O;
        case 'P':           return E_KEY_P;
        case 'Q':           return E_KEY_Q;
        case 'R':           return E_KEY_R;
        case 'S':           return E_KEY_S;
        case 'T':           return E_KEY_T;
        case 'U':           return E_KEY_U;
        case 'V':           return E_KEY_V;
        case 'W':           return E_KEY_W;
        case 'X':           return E_KEY_X;
        case 'Y':           return E_KEY_Y;
        case 'Z':           return E_KEY_Z;
        default: break;
    }

    /* Getting here means we don't have a proper mapping. Just return the key as-is. */
    return key;
}


struct e_platform_window
{
    e_window* pOwnerWindow;
    HWND hWnd;
    HDC hDC;                    /* Can use GetDC() instead, but in my experience that has tended to be extremely slow. */
    HDC hBufferDC;              /* We need a separate HDC for software rendering. hBufferDIB will be permanently selected into hBufferDC. hBufferDC will be used with StretchBlt(). */
    HANDLE hBufferDIB;          /* For software rendering. We'll use this with StretchBlt() to output the buffer to the screen. */
    void* pBufferDIBData;       /* The underlying buffer containing the data of hBufferDIB. */
    void* pBufferDIBDataIntermediary;   /* This is an aligned intermediary buffer for when pBufferDIBData does not meet our alignment requirements. */
    unsigned int bufferSizeX;
    unsigned int bufferSizeY;
    int clientSizeX;            /* So we don't have to use GetClientRect() in each call to e_platform_window_next_buffer(). */
    int clientSizeY;
    HCURSOR hDefaultCursor;
    HCURSOR hCurrentCursor;
    HCURSOR hOldCursor;
    e_bool32 isCursorHidden;
    e_bool32 isCursorPinned;
    e_bool32 ignoreNextMouseMoveEvent;
    int pinnedCursorPosX;
    int pinnedCursorPosY;
    e_uint16 utf16Hi;           /* The high surrogate pair in a UTF-16 surrogate pair. Used with WM_CHAR. */
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
        pixelFormat = e_ChoosePixelFormat(pWindow->hDC, &pfd);
        if (pixelFormat == 0) {
            /*
            We could not find a pixel format. This will prevent us from using the window with OpenGL. Since we've
            explicity asked for OpenGL support, the best option is to abort with an error.
            */
            DestroyWindow(pWindow->hWnd);
            return E_ERROR;
        }

        e_SetPixelFormat(pWindow->hDC, pixelFormat, &pfd);
    }

    /* Some other default states. */
    pWindow->isCursorHidden = E_FALSE;
    pWindow->hDefaultCursor = LoadCursorW(NULL, MAKEINTRESOURCEW(32512));
    pWindow->hCurrentCursor = pWindow->hDefaultCursor;

    return E_SUCCESS;
}

static e_result e_platform_window_uninit(e_platform_window* pWindow, const e_allocation_callbacks* pAllocationCallbacks)
{
    E_ASSERT(pWindow != NULL);
    E_UNUSED(pAllocationCallbacks);

    DestroyWindow(pWindow->hWnd);

    if (pWindow->hBufferDC != NULL) {
        DeleteDC(pWindow->hBufferDC);
    }

    if (pWindow->hBufferDIB != NULL) {
        DeleteObject(pWindow->hBufferDIB);
    }

    if (pWindow->pBufferDIBDataIntermediary != NULL) {
        e_aligned_free(pWindow->pBufferDIBDataIntermediary, NULL);
    }

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

static e_result e_platform_window_capture_cursor(e_platform_window* pWindow)
{
    SetCapture(pWindow->hWnd);
    return E_SUCCESS;
}

static e_result e_platform_window_release_cursor(e_platform_window* pWindow)
{
    (void)pWindow;

    ReleaseCapture();
    return E_SUCCESS;
}

static e_result e_platform_window_set_cursor_position(e_platform_window* pWindow, int cursorPosX, int cursorPosY)
{
    POINT pt;

    pt.x = cursorPosX;
    pt.y = cursorPosY;
    ClientToScreen(pWindow->hWnd, &pt);
    SetCursorPos(pt.x, pt.y);

    return E_SUCCESS;
}

static e_result e_platform_window_get_cursor_position(e_platform_window* pWindow, int* pCursorPosX, int* pCursorPosY)
{
    POINT pt;

    GetCursorPos(&pt);
    ScreenToClient(pWindow->hWnd, &pt);

    if (pCursorPosX != NULL) {
        *pCursorPosX = pt.x;
    }
    if (pCursorPosY != NULL) {
        *pCursorPosY = pt.y;
    }

    return E_SUCCESS;
}

static e_result e_platform_window_show_cursor(e_platform_window* pWindow)
{
    /*
    We don't use ShowCursor() here. We want this to be per-window which means we'll actually want to do it
    in WM_SETCURSOR based on a variable.
    */
    pWindow->isCursorHidden = E_FALSE;
    pWindow->hCurrentCursor = pWindow->hDefaultCursor;

    /*
    The cursor is usually set with SetCursor() in WM_SETCURSOR, but unfortunately that will not get fired
    if the mouse has been captured. Therefore, if the mouse has been captured by this window, we need to
    set the set the cursor here.
    */
    if (GetCapture() == pWindow->hWnd) {
        SetCursor(pWindow->hCurrentCursor);
    }

    return E_SUCCESS;
}

static e_result e_platform_window_hide_cursor(e_platform_window* pWindow)
{
    pWindow->isCursorHidden = E_TRUE;
    pWindow->hCurrentCursor = NULL;

    /* Like when showing the cursor, if the mouse is captured we'll need to hide it here. */
    if (GetCapture() == pWindow->hWnd) {
        pWindow->hOldCursor = SetCursor(pWindow->hCurrentCursor);
    } else {
        /*
        The mouse is not captured, but it's still possible that WM_SETCURSOR is not fired right away which
        might happen when the cursor is hidden in response to a mouse click. WM_SETCURSOR is fired when the
        mouse moves, not when the mouse is clicked. To deal with this, we will check if the cursor is over
        the client area of the window and if it is we'll set the cursor to NULL.
        */
        POINT pt;
        RECT clientRect;

        GetCursorPos(&pt);
        ScreenToClient(pWindow->hWnd, &pt);
        GetClientRect(pWindow->hWnd, &clientRect);

        if ((pt.x >= clientRect.left && pt.x < clientRect.right && pt.y >= clientRect.top && pt.y < clientRect.bottom) && GetForegroundWindow() == pWindow->hWnd) {
            pWindow->hOldCursor = SetCursor(pWindow->hCurrentCursor);
        }
    }

    return E_SUCCESS;
}

static e_result e_platform_window_pin_cursor(e_platform_window* pWindow, int cursorPosX, int cursorPosY)
{
    POINT pt;
   
    pWindow->isCursorPinned = E_TRUE;
    pWindow->pinnedCursorPosX = cursorPosX;
    pWindow->pinnedCursorPosY = cursorPosY;

    pt.x = cursorPosX;
    pt.y = cursorPosY;
    ClientToScreen(pWindow->hWnd, &pt);

    pWindow->ignoreNextMouseMoveEvent = E_TRUE;
    SetCursorPos(pt.x, pt.y);

    return E_SUCCESS;
}

static e_result e_platform_window_unpin_cursor(e_platform_window* pWindow)
{
    pWindow->isCursorPinned = E_FALSE;
    return E_SUCCESS;
}

static e_result e_platform_window_post_close_event(e_platform_window* pWindow)
{
    PostMessageW(pWindow->hWnd, WM_CLOSE, 0, 0);
    return E_SUCCESS;
}

static e_result e_platform_window_next_buffer(e_platform_window* pWindow, unsigned int bufferSizeX, unsigned int bufferSizeY, e_window_buffer* pBuffer)
{
    E_ASSERT(pWindow != NULL);
    E_ASSERT(pBuffer != NULL);

    /* If we have a DIB, present it. */
    if (pWindow->hBufferDIB) {
        unsigned int x;
        unsigned int y;

        if (pWindow->pBufferDIBDataIntermediary == NULL) {
            /* Fast path. We need only convert from 0xAABBGGRR (e) to 0xAARRGGBB (GDI). We can assume a clean alignment. */
            e_uint32* pBuffer32 = (e_uint32*)pWindow->pBufferDIBData;

            for (y = 0; y < pWindow->bufferSizeY; y += 1) {
                for (x = 0; x < pWindow->bufferSizeX; x += 4) {
                    pBuffer32[0] = (pBuffer32[0] & 0xFF00FF00) | ((pBuffer32[0] & 0x00FF0000) >> 16) | ((pBuffer32[0] & 0x000000FF) << 16);
                    pBuffer32[1] = (pBuffer32[1] & 0xFF00FF00) | ((pBuffer32[1] & 0x00FF0000) >> 16) | ((pBuffer32[1] & 0x000000FF) << 16);
                    pBuffer32[2] = (pBuffer32[2] & 0xFF00FF00) | ((pBuffer32[2] & 0x00FF0000) >> 16) | ((pBuffer32[2] & 0x000000FF) << 16);
                    pBuffer32[3] = (pBuffer32[3] & 0xFF00FF00) | ((pBuffer32[3] & 0x00FF0000) >> 16) | ((pBuffer32[3] & 0x000000FF) << 16);
                    pBuffer32 += 4;
                }
            }
        } else {
            /*
            Slow path. Here we need to copy of the contents of the intermediary buffer to the DIB buffer. We'll
            also shuffle the color components in the same pass.
            */
            e_uint32* pSrc = (e_uint32*)pWindow->pBufferDIBDataIntermediary;
            e_uint32* pDst = (e_uint32*)pWindow->pBufferDIBData;

            for (y = 0; y < pWindow->bufferSizeY; y += 1) {
                for (x = 0; x < pWindow->bufferSizeX; x += 1) {
                    pDst[x] = (pSrc[x] & 0xFF00FF00) | ((pSrc[x] & 0x00FF0000) >> 16) | ((pSrc[x] & 0x000000FF) << 16);
                }

                pSrc += e_round_up_to_nearest_4(pWindow->bufferSizeX);
                pDst += pWindow->bufferSizeX;
            }
        }

        /* Getting here means we have a buffer and we can now present it using StretchBlt(). */
        StretchBlt(pWindow->hDC, 0, 0, pWindow->clientSizeX, pWindow->clientSizeY, pWindow->hBufferDC, 0, 0, pWindow->bufferSizeX, pWindow->bufferSizeY, SRCCOPY);

        /*
        MSDN suggests we should use GdiFlush() to ensure GDI is done with our buffer before trying to use
        it again.
        */
        //GdiFlush();
    }


    /*
    If the size of the buffer has changed, recreate it. We can do this by simply deleting everything and resetting
    it back to null, and then just fall through to the section below which will do the recreation.
    */
    if (pWindow->bufferSizeX != bufferSizeX || pWindow->bufferSizeY != bufferSizeY) {
        if (pWindow->hBufferDC != NULL) {
            DeleteDC(pWindow->hBufferDC);
            pWindow->hBufferDC = NULL;
        }

        if (pWindow->hBufferDIB != NULL) {
            DeleteObject(pWindow->hBufferDIB);
            pWindow->hBufferDIB  = NULL;
            pWindow->bufferSizeX = 0;
            pWindow->bufferSizeY = 0;
        }

        if (pWindow->pBufferDIBDataIntermediary != NULL) {
            e_aligned_free(pWindow->pBufferDIBDataIntermediary, NULL);
            pWindow->pBufferDIBDataIntermediary = NULL;
        }
    }


    /* If we don't have a DIB, create one. */
    if (pWindow->hBufferDIB == NULL) {
        /*
        Getting here means it's our first time calling this function and we need to create a new buffer. Note that
        the first time we call this function we do not do any buffer presentation because there is no valid back
        buffer to actually present. Therefore we'll be returning early from this branch.

        We need to use CreateDIBSection() here. Our alignment requirements are as follows:

            1) 16 bytes alignment
            2) The row stride in bytes must be a multiple of 16

        If any of these two requirements are not met, we'll need to make use of our own self-managed intermediary
        buffer which meets the requirements.

        Although it's not formally documented, in practice CreateDIBSection() should return a buffer that is aligned
        to 16 bytes. If it does not, we'll fall back to our intermediary buffer.

        For the second requirement, we need only check if the width is a multiple 4, and if not, we'll need to use
        the intermediary.
        */
        BITMAPINFO bi;
        
        /* The first thing we do is create the DIB. */
        E_ZERO_OBJECT(&bi);
        bi.bmiHeader.biSize        =  sizeof(BITMAPINFOHEADER);
        bi.bmiHeader.biWidth       =  bufferSizeX;
        bi.bmiHeader.biHeight      = -(int)bufferSizeY;
        bi.bmiHeader.biPlanes      =  1;
        bi.bmiHeader.biBitCount    =  32;
        bi.bmiHeader.biCompression =  BI_RGB;

        pWindow->hBufferDIB = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, &pWindow->pBufferDIBData, NULL, 0);
        if (pWindow->hBufferDIB == NULL) {
            return E_ERROR; /* Failed to create the DIB section. */
        }

        /* Now we need to determine if we need to use an intermediary buffer. */
        if (((e_uintptr)pWindow->pBufferDIBData & 15) != 0 ||   /* <-- 16 bytes alignment. */
            ((bufferSizeX & 0x03) != 0))                        /* <-- Width must be a multiple of 4. */
        {
            /* Getting here means we do not meet the necessary alignment requirements and we need to use an intermediary buffer which does. */
            pWindow->pBufferDIBDataIntermediary = e_aligned_malloc((e_round_up_to_nearest_16(bufferSizeX * 4) * bufferSizeY), 16, NULL);
            if (pWindow->pBufferDIBDataIntermediary == NULL) {
                DeleteObject(pWindow->hBufferDIB);
                pWindow->hBufferDIB = NULL;

                return E_OUT_OF_MEMORY;
            }
        }

        pWindow->hBufferDC = CreateCompatibleDC(NULL);
        if (pWindow->hBufferDC == NULL) {
            DeleteObject(pWindow->hBufferDIB);
            pWindow->hBufferDIB = NULL;

            e_aligned_free(pWindow->pBufferDIBDataIntermediary, NULL);
            pWindow->pBufferDIBDataIntermediary = NULL;

            return E_ERROR; /* Failed to create compatible DC. */
        }

        SelectObject(pWindow->hBufferDC, pWindow->hBufferDIB);

        pWindow->bufferSizeX = bufferSizeX;
        pWindow->bufferSizeY = bufferSizeY;
    }

    pBuffer->sizeX = pWindow->bufferSizeX;
    pBuffer->sizeY = pWindow->bufferSizeY;
    pBuffer->pData = (pWindow->pBufferDIBDataIntermediary != NULL) ? pWindow->pBufferDIBDataIntermediary : pWindow->pBufferDIBData;

    return E_SUCCESS;
}


static LRESULT e_platform_default_window_proc_win32(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    e_platform_window* pWindow = (e_platform_window*)GetWindowLongPtrW(hWnd, 0);
    if (pWindow != NULL) {
        e_event e;

        switch (msg)
        {
            case WM_CLOSE:
            {
                e = e_window_event_init(E_EVENT_CLOSE, pWindow->pOwnerWindow);
                e_window_handle_event(pWindow->pOwnerWindow, &e);
                return 0;
            };

            case WM_SIZE:
            {
                pWindow->clientSizeX = LOWORD(lParam);
                pWindow->clientSizeY = HIWORD(lParam);

                e = e_window_event_init(E_EVENT_SIZE, pWindow->pOwnerWindow);
                e.data.size.x = pWindow->clientSizeX;
                e.data.size.y = pWindow->clientSizeY;
                e_window_handle_event(pWindow->pOwnerWindow, &e);
            } break;

            case WM_SETCURSOR:
            {
                /*
                TODO: This is fired every time the mouse moves. I wonder if we should do our own checks
                to determine if calling SetCursor() is required in order to perhaps improve performance.
                */
                if (LOWORD(lParam) == HTCLIENT) {
                    if (pWindow->hCurrentCursor == NULL) {
                        pWindow->hOldCursor = SetCursor(NULL);
                    } else {
                        if (pWindow->hOldCursor == NULL) {
                            SetCursor(pWindow->hDefaultCursor);
                        } else {
                            SetCursor(pWindow->hOldCursor);
                        }
                    }

                    return 0;
                }
            } break;

            case WM_MOVE:
            {
                e = e_window_event_init(E_EVENT_MOVE, pWindow->pOwnerWindow);
                e.data.move.x = (short)LOWORD(lParam);
                e.data.move.y = (short)HIWORD(lParam);
                e_window_handle_event(pWindow->pOwnerWindow, &e);
            } break;

            case WM_MOUSEMOVE:
            {
                if (pWindow->ignoreNextMouseMoveEvent) {
                    pWindow->ignoreNextMouseMoveEvent = E_FALSE;
                } else {
                    /* If the cursor is pinned we report delta movements. */
                    int cursorPosX;
                    int cursorPosY;

                    if (pWindow->isCursorPinned) {
                        POINT pt;

                        cursorPosX = (short)LOWORD(lParam) - pWindow->pinnedCursorPosX;
                        cursorPosY = (short)HIWORD(lParam) - pWindow->pinnedCursorPosY;

                        /* Move the cursor back to it's pinned location. */
                        pt.x = pWindow->pinnedCursorPosX;
                        pt.y = pWindow->pinnedCursorPosY;
                        ClientToScreen(pWindow->hWnd, &pt);

                        /*
                        I thought SetCursorPos() was supposed to send a WM_MOUSEMOVE event. When trying to ignore the next event
                        I get jerky mouse movement. Need to investigate this one. I was expecting the SetCursorPos() call to send
                        WM_MOUSEMOVE to the reverse of the delta movement thereby essentially resetting it, but that doesn't
                        seem to be happening.
                        */
                        /*pWindow->ignoreNextMouseMoveEvent = E_TRUE;*/
                        SetCursorPos(pt.x, pt.y);
                    } else {
                        cursorPosX = (short)LOWORD(lParam);
                        cursorPosY = (short)HIWORD(lParam);
                    }

                    e = e_window_event_init(E_EVENT_CURSOR_MOVE, pWindow->pOwnerWindow);
                    e.data.cursorMove.x = (short)cursorPosX;
                    e.data.cursorMove.y = (short)cursorPosY;
                    e_window_handle_event(pWindow->pOwnerWindow, &e);
                }
            } break;

            /* Mouse buttons. */
            case WM_LBUTTONDOWN:
            {
                e = e_window_event_init(E_EVENT_CURSOR_BUTTON_DOWN, pWindow->pOwnerWindow);
                e.data.cursorButtonDown.x = (short)LOWORD(lParam);
                e.data.cursorButtonDown.y = (short)HIWORD(lParam);
                e.data.cursorButtonDown.button = E_CURSOR_BUTTON_LEFT;
                e_window_handle_event(pWindow->pOwnerWindow, &e);
            } break;
            case WM_RBUTTONDOWN:
            {
                e = e_window_event_init(E_EVENT_CURSOR_BUTTON_DOWN, pWindow->pOwnerWindow);
                e.data.cursorButtonDown.x = (short)LOWORD(lParam);
                e.data.cursorButtonDown.y = (short)HIWORD(lParam);
                e.data.cursorButtonDown.button = E_CURSOR_BUTTON_RIGHT;
                e_window_handle_event(pWindow->pOwnerWindow, &e);
            } break;
            case WM_MBUTTONDOWN:
            {
                e = e_window_event_init(E_EVENT_CURSOR_BUTTON_DOWN, pWindow->pOwnerWindow);
                e.data.cursorButtonDown.x = (short)LOWORD(lParam);
                e.data.cursorButtonDown.y = (short)HIWORD(lParam);
                e.data.cursorButtonDown.button = E_CURSOR_BUTTON_MIDDLE;
                e_window_handle_event(pWindow->pOwnerWindow, &e);
            } break;
            case WM_XBUTTONDOWN:
            {
                e = e_window_event_init(E_EVENT_CURSOR_BUTTON_DOWN, pWindow->pOwnerWindow);
                e.data.cursorButtonDown.x = (short)LOWORD(lParam);
                e.data.cursorButtonDown.y = (short)HIWORD(lParam);
                e.data.cursorButtonDown.button = (HIWORD(wParam) == XBUTTON1) ? E_CURSOR_BUTTON_4 : E_CURSOR_BUTTON_5;
                e_window_handle_event(pWindow->pOwnerWindow, &e);
            } break;

            case WM_LBUTTONUP:
            {
                e = e_window_event_init(E_EVENT_CURSOR_BUTTON_UP, pWindow->pOwnerWindow);
                e.data.cursorButtonUp.x = (short)LOWORD(lParam);
                e.data.cursorButtonUp.y = (short)HIWORD(lParam);
                e.data.cursorButtonUp.button = E_CURSOR_BUTTON_LEFT;
                e_window_handle_event(pWindow->pOwnerWindow, &e);
            } break;
            case WM_RBUTTONUP:
            {
                e = e_window_event_init(E_EVENT_CURSOR_BUTTON_UP, pWindow->pOwnerWindow);
                e.data.cursorButtonUp.x = (short)LOWORD(lParam);
                e.data.cursorButtonUp.y = (short)HIWORD(lParam);
                e.data.cursorButtonUp.button = E_CURSOR_BUTTON_RIGHT;
                e_window_handle_event(pWindow->pOwnerWindow, &e);
            } break;
            case WM_MBUTTONUP:
            {
                e = e_window_event_init(E_EVENT_CURSOR_BUTTON_UP, pWindow->pOwnerWindow);
                e.data.cursorButtonUp.x = (short)LOWORD(lParam);
                e.data.cursorButtonUp.y = (short)HIWORD(lParam);
                e.data.cursorButtonUp.button = E_CURSOR_BUTTON_MIDDLE;
                e_window_handle_event(pWindow->pOwnerWindow, &e);
            } break;
            case WM_XBUTTONUP:
            {
                e = e_window_event_init(E_EVENT_CURSOR_BUTTON_UP, pWindow->pOwnerWindow);
                e.data.cursorButtonUp.x = (short)LOWORD(lParam);
                e.data.cursorButtonUp.y = (short)HIWORD(lParam);
                e.data.cursorButtonUp.button = (HIWORD(wParam) == XBUTTON1) ? E_CURSOR_BUTTON_4 : E_CURSOR_BUTTON_5;
                e_window_handle_event(pWindow->pOwnerWindow, &e);
            } break;

            /* Mouse wheel. */
            case WM_MOUSEWHEEL:
            {
                e = e_window_event_init(E_EVENT_CURSOR_WHEEL, pWindow->pOwnerWindow);
                e.data.cursorWheel.delta = GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
                e_window_handle_event(pWindow->pOwnerWindow, &e);
            } break;

        #if 0
            case WM_INPUT:
            {
                UINT dwSize = 0;

                GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
                if (dwSize > 0) {
                    RAWINPUT* pRawInput = (RAWINPUT*)malloc(dwSize);
                    if (pRawInput != NULL) {
                        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, pRawInput, &dwSize, sizeof(RAWINPUTHEADER)) == dwSize) {
                            if (pRawInput->header.dwType == RIM_TYPEMOUSE) {
                                e = e_window_event_init(E_EVENT_CURSOR_MOVE, pWindow->pOwnerWindow);
                                e.data.cursorMove.x = pRawInput->data.mouse.lLastX;
                                e.data.cursorMove.y = pRawInput->data.mouse.lLastY;
                                e_window_handle_event(pWindow->pOwnerWindow, &e);
                            }
                        }

                        free(pRawInput);
                    }
                }
            } break;
        #endif

            case WM_KEYDOWN:
            {
                e = e_window_event_init(E_EVENT_KEY_DOWN, pWindow->pOwnerWindow);
                e.data.keyDown.key = e_key_from_win32((unsigned int)wParam);
                e.data.keyDown.isAutoRepeat = (lParam & (1 << 30)) != 0;
                e_window_handle_event(pWindow->pOwnerWindow, &e);
            } break;

            case WM_KEYUP:
            {
                e = e_window_event_init(E_EVENT_KEY_UP, pWindow->pOwnerWindow);
                e.data.keyUp.key = e_key_from_win32((unsigned int)wParam);
                e_window_handle_event(pWindow->pOwnerWindow, &e);
            } break;

            case WM_CHAR:
            {
                /* This needs to capture surrogate pairs. */
                e_uint16 utf16 = (e_uint16)wParam;
                e_uint32 utf32;

                if (utf16 >= 0xD800 && utf16 <= 0xDBFF) {
                    pWindow->utf16Hi = utf16;
                } else {
                    if (utf16 >= 0xDC00 && utf16 <= 0xDFFF) {
                        /* It's a low surrogate. Combine it with the high, and then post an event. */
                        utf32 = 0x10000 + (((e_uint32)pWindow->utf16Hi - 0xD800) << 10) + ((e_uint32)utf16 - 0xDC00);
                    } else {
                        /* It's just a regular UTF-16 character. */
                        utf32 = utf16;
                    }

                    e = e_window_event_init(E_EVENT_CHARACTER, pWindow->pOwnerWindow);
                    e.data.character.utf32        = (e_uint32)wParam;
                    e.data.character.isAutoRepeat = (lParam & (1 << 30)) != 0;
                    e_window_handle_event(pWindow->pOwnerWindow, &e);
                }
            } break;

            default: break;
        }
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}


static e_bool32 e_gUseBlockingMainLoop = E_FALSE;

static e_result e_platform_main_loop(int* pExitCode, e_platform_main_loop_iteration_callback iterationCallback, void* pUserData)
{
    int exitCode = 0;
    MSG msg;

    E_ASSERT(pExitCode != NULL);
    E_ASSERT(iterationCallback != NULL);

    for (;;) {
        BOOL result;

        if (e_gUseBlockingMainLoop) {
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
        
        /* Call the iteration callback. */
        if (iterationCallback(pUserData) != E_SUCCESS) {
            break;
        }
    }

    *pExitCode = exitCode;
    return E_SUCCESS;
}

static e_result e_platform_exit_main_loop(int exitCode)
{
    PostQuitMessage(exitCode);
    return E_SUCCESS;
}

static e_result e_platform_wake_main_loop(void)
{
    PostMessageW(NULL, WM_NULL, 0, 0);
    return E_SUCCESS;
}

static e_result e_platform_set_main_loop_blocking(e_bool32 blocking)
{
    /* Very minor optimization. Don't do anything if the blocking mode is not changing. This saves us from posting a message to the main loop. */
    if (e_gUseBlockingMainLoop == blocking) {
        return E_SUCCESS;
    }

    e_gUseBlockingMainLoop = blocking;
    
    /* The loop needs to be woken up in case it's currently blocking. */
    return e_platform_wake_main_loop();
}
#endif  /* E_WIN32 */


#if defined(E_POSIX)
/*
We need to declare our own equivalents of the public Xlib data structures that we're using here so
we can avoid including Xlib.h. They should be named the same, but with the "e_" prefix to avoid
any potential collisions.
*/
#define e_None                      0
#define e_AllocNone                 0

#define e_CWBorderPixel             (1 << 3)
#define e_CWColormap                (1 << 13)
#define e_CWEventMask               (1 << 15)

#define e_InputOutput               1

/* SubstructureRedirectMask | SubstructureNotifyMask */
#define e_NoEventMask               0
#define e_KeyPressMask              (1 << 0)
#define e_KeyReleaseMask            (1 << 1)
#define e_ButtonPressMask           (1 << 2)
#define e_ButtonReleaseMask         (1 << 3)
#define e_EnterWindowMask           (1 << 4)
#define e_LeaveWindowMask           (1 << 5)
#define e_PointerMotionMask         (1 << 6)
#define e_PointerMotionHintMask     (1 << 7)
#define e_Button1MotionMask         (1 << 8)
#define e_Button2MotionMask         (1 << 9)
#define e_Button3MotionMask         (1 << 10)
#define e_Button4MotionMask         (1 << 11)
#define e_Button5MotionMask         (1 << 12)
#define e_ButtonMotionMask          (1 << 13)
#define e_KeymapStateMask           (1 << 14)
#define e_ExposureMask              (1 << 15)
#define e_VisibilityChangeMask      (1 << 16)
#define e_StructureNotifyMask       (1 << 17)
#define e_ResizeRedirectMask        (1 << 18)
#define e_SubstructureNotifyMask    (1 << 19)
#define e_SubstructureRedirectMask  (1 << 20)
#define e_FocusChangeMask           (1 << 21)
#define e_PropertyChangeMask        (1 << 22)
#define e_ColormapChangeMask        (1 << 23)
#define e_OwnerGrabButtonMask       (1 << 24)

#define e_KeyPress                  2
#define e_KeyRelease                3
#define e_ButtonPress               4
#define e_ButtonRelease             5
#define e_MotionNotify              6
#define e_EnterNotify               7
#define e_LeaveNotify               8
#define e_FocusIn                   9
#define e_FocusOut                  10
#define e_CreateNotify              16
#define e_DestroyNotify             17
#define e_UnmapNotify               18
#define e_MapNotify                 19
#define e_ConfigureNotify           22
#define e_PropertyNotify            28
#define e_ClientMessage             33
#define e_GenericEvent              35

#define e_ShiftMask                 (1 << 0)
#define e_LockMask                  (1 << 1)
#define e_ControlMask               (1 << 2)
#define e_Mod1Mask                  (1 << 3)
#define e_Mod2Mask                  (1 << 4)
#define e_Mod3Mask                  (1 << 5)
#define e_Mod4Mask                  (1 << 6)
#define e_Mod5Mask                  (1 << 7)

#define e_ShiftMapIndex             0
#define e_LockMapIndex              1
#define e_ControlMapIndex           2
#define e_Mod1MapIndex              3
#define e_Mod2MapIndex              4
#define e_Mod3MapIndex              5
#define e_Mod4MapIndex              6
#define e_Mod5MapIndex              7

#define e_Button1Mask               (1 << 8)
#define e_Button2Mask               (1 << 9)
#define e_Button3Mask               (1 << 10)
#define e_Button4Mask               (1 << 11)
#define e_Button5Mask               (1 << 12)
#define e_AnyModifier               (1 << 15)

#define e_Button1                   1
#define e_Button2                   2
#define e_Button3                   3
#define e_Button4                   4
#define e_Button5                   5

#define e_TrueColor                 4
#define e_DirectColor               5

#define e_CurrentTime               0

#define e_GrabModeSync              0
#define e_GrabModeAsync             1

typedef struct e_XDisplay           e_Display;
typedef struct e_XScreen            e_Screen;
typedef struct e_XVisual            e_Visual;

typedef void*                       e_XPointer;

typedef unsigned long               e_VisualID;
typedef unsigned long               e_XID;
typedef unsigned long               e_Atom;
typedef unsigned long               e_Time;
typedef unsigned char               e_KeyCode;
typedef int                         e_Bool;

typedef e_XID                       e_Window;
typedef e_XID                       e_Colormap;
typedef e_XID                       e_Pixmap;
typedef e_XID                       e_Cursor;
typedef e_XID                       e_Font;
typedef e_XID                       e_XContext;

typedef struct e_XVisualInfo
{
    e_Visual* visual;
    e_VisualID visualid;
    int screen;
    int depth;
    int _class;
    unsigned long red_mask;
    unsigned long green_mask;
    unsigned long blue_mask;
    int colormap_size;
    int bits_per_rgb;
} e_XVisualInfo;

typedef struct e_XSetWindowAttributes
{
    e_Pixmap background_pixmap;
    unsigned long background_pixel;
    e_Pixmap border_pixmap;
    unsigned long border_pixel;
    int bit_gravity;
    int win_gravity;
    int backing_store;
    unsigned long backing_planes;
    unsigned long backing_pixel;
    e_Bool save_under;
    long event_mask;
    long do_not_propagate_mask;
    e_Bool override_redirect;
    e_Colormap colormap;
    e_Cursor cursor;
} e_XSetWindowAttributes;

typedef struct e_XWindowAttributes
{
    int x;
    int y;
    int width;
    int height;
    int border_width;
    int depth;
    e_Visual* visual;
    e_Window root;
    int c_class;
    int bit_gravity;
    int win_gravity;
    int backing_store;
    unsigned long backing_planes;
    unsigned long backing_pixel;
    e_Bool save_under;
    e_Colormap colormap;
    e_Bool map_installed;
    int map_state;
    long all_event_masks;
    long your_event_mask;
    long do_not_propagate_mask;
    e_Bool override_redirect;
    e_Screen* screen;
} e_XWindowAttributes;

typedef struct e_XColor
{
    unsigned long pixel;
    unsigned short red, green, blue;
    char flags;
    char pad;
} e_XColor;

/* We need to declare our own version of XEvent. We only declare the sub-structures of the events we actually use. */
typedef union e_XEvent
{
    int type;

    struct
    {
        int type;
        unsigned long serial;
        e_Bool send_event;
        e_Display* display;
        e_Window window;
    } xany;

    struct
    {
        int type;
        unsigned long serial;
        e_Bool send_event;
        e_Display* display;
        e_Window window;
        e_Window root;
        e_Window subwindow;
        e_Time time;
        int x;
        int y;
        int x_root;
        int y_root;
        unsigned int state;
        unsigned int keycode;
        e_Bool same_screen;
    } xkey;

    struct
    {
        int type;
        unsigned long serial;
        e_Bool send_event;
        e_Display* display;
        e_Window window;
        e_Window root;
        e_Window subwindow;
        e_Time time;
        int x;
        int y;
        int x_root;
        int y_root;
        unsigned int state;
        unsigned int button;
        e_Bool same_screen;
    } xbutton;

    struct
    {
        int type;
        unsigned long serial;
        e_Bool send_event;
        e_Display* display;
        e_Window window;
        e_Window root;
        e_Window subwindow;
        e_Time time;
        int x;
        int y;
        int x_root;
        int y_root;
        unsigned int state;
        char is_hint;
        e_Bool same_screen;
    } xmotion;

    struct
    {
        int type;
        unsigned long serial;
        e_Bool send_event;
        e_Display* display;
        e_Window window;
        int mode;
        int detail;
    } xfocus;

    struct
    {
        int type;
        unsigned long serial;
        e_Bool send_event;
        e_Display* display;
        e_Window parent;
        e_Window window;
        int x;
        int y;
        int width;
        int height;
        int border_width;
        e_Bool override_redirect;
    } xcreatewindow;
    struct
    {
        int type;
        unsigned long serial;
        e_Bool send_event;
        e_Display* display;
        e_Window event;
        e_Window window;
    } xdestroywindow;

    struct
    {
        int type;
        unsigned long serial;
        e_Bool send_event;
        e_Display* display;
        e_Window event;
        e_Window window;
        e_Bool from_configure;
    } xunmap;
    struct
    {
        int type;
        unsigned long serial;
        e_Bool send_event;
        e_Display* display;
        e_Window event;
        e_Window window;
        e_Bool override_redirect;
    } xmap;

    struct
    {
        int type;
        unsigned long serial;
        e_Bool send_event;
        e_Display* display;
        e_Window event;
        e_Window window;
        int x;
        int y;
        int width;
        int height;
        int border_width;
        e_Window above;
        e_Bool override_redirect;
    } xconfigure;

    struct
    {
        int type;
        unsigned long serial;
        e_Bool send_event;
        e_Display* display;
        e_Window window;
        e_Atom atom;
        e_Time time;
        int state;
    } xproperty;

    struct
    {
        int type;
        unsigned long serial;
        e_Bool send_event;
        e_Display* display;
        e_Window window;
        e_Atom message_type;
        int format;
        union
        {
            char  b[20];
            short s[10];
            long  l[5];
        } data;
    } xclient;

    long pad[24];
} e_XEvent;


/* OpenGL */
typedef e_XVisualInfo* (* e_pfn_glXChooseVisual)(e_Display* pDisplay, int screen, int* pAttribList);

static e_handle e_gOpenGLSO = NULL;
static e_pfn_glXChooseVisual e_glXChooseVisual;

/* Xlib */
typedef e_Display* (* e_pfn_XOpenDisplay         )(const char* pDisplayName);
typedef int        (* e_pfn_XCloseDisplay        )(e_Display* pDisplay);
typedef int        (* e_pfn_XMatchVisualInfo     )(e_Display* pDisplay, int screen, int depth, int _class, e_XVisualInfo* pVisualInfo);
typedef e_Colormap (* e_pfn_XCreateColormap      )(e_Display* pDisplay, e_Window w, e_Visual* pVisual, int alloc);
typedef int        (* e_pfn_XFreeColormap        )(e_Display* pDisplay, e_Colormap colormap);
typedef e_Window   (* e_pfn_XCreateWindow        )(e_Display* pDisplay, e_Window parent, int x, int y, unsigned int width, unsigned int height, unsigned int borderWidth, int depth, unsigned int _class, e_Visual* pVisual, unsigned long valuemask, e_XSetWindowAttributes* pAttributes);
typedef int        (* e_pfn_XDestroyWindow       )(e_Display* pDisplay, e_Window w);
typedef int        (* e_pfn_XMapWindow           )(e_Display* pDisplay, e_Window w);
typedef int        (* e_pfn_XStoreName           )(e_Display* pDisplay, e_Window w, const char* pWindowName);
typedef int        (* e_pfn_XResizeWindow        )(e_Display* pDisplay, e_Window w, unsigned int width, unsigned int height);
typedef int        (* e_pfn_XGetWindowAttributes )(e_Display* pDisplay, e_Window w, e_XWindowAttributes* pWindowAttributes);
typedef int        (* e_pfn_XSaveContext         )(e_Display* pDisplay, e_XID rid, e_XContext context, const e_XPointer pPointer);
typedef int        (* e_pfn_XDeleteContext       )(e_Display* pDisplay, e_XID rid, e_XContext context);
typedef int        (* e_pfn_XFindContext         )(e_Display* pDisplay, e_XID rid, e_XContext context, e_XPointer* ppPointer);
typedef int        (* e_pfn_XSetWMProtocols      )(e_Display* pDisplay, e_Window w, e_Atom* pProtocols, int count);
typedef e_Atom     (* e_pfn_XInternAtom          )(e_Display* pDisplay, const char* pAtomName, e_bool32 onlyIfExists);
typedef int        (* e_pfn_XPending             )(e_Display* pDisplay);
typedef int        (* e_pfn_XNextEvent           )(e_Display* pDisplay, e_XEvent* pEvent);
typedef int        (* e_pfn_XSendEvent           )(e_Display* pDisplay, e_Window w, e_bool32 propagate, long eventMask, e_XEvent* pEvent);
typedef e_Window   (* e_pfn_XRootWindow          )(e_Display* pDisplay, int screenNumber);
typedef int        (* e_pfn_XDefaultScreen       )(e_Display* pDisplay);
typedef int        (* e_pfn_XDefaultDepth        )(e_Display* pDisplay, int screenNumber);
typedef int        (* e_pfn_XGrabPointer         )(e_Display* pDisplay, e_Window grabWindow, e_Bool ownerEvents, unsigned int eventMask, int pointerMode, int keyboardMode, e_Window confineTo, e_Cursor cursor, e_Time time);
typedef int        (* e_pfn_XUngrabPointer       )(e_Display* pDisplay, e_Time time);
typedef int        (* e_pfn_XWarpPointer         )(e_Display* pDisplay, e_Window src_w, e_Window dest_w, int src_x, int src_y, unsigned int src_width, unsigned int src_height, int dest_x, int dest_y);
typedef e_Bool     (* e_pfn_XQueryPointer        )(e_Display* pDisplay, e_Window w, e_Window* pRoot, e_Window* pChild, int* pRootX, int* pRootY, int* pWinX, int* pWinY, unsigned int* pKeysButtons);
typedef e_Pixmap   (* e_pfn_XCreateBitmapFromData)(e_Display* pDisplay, e_Window w, const char* pData, unsigned int width, unsigned int height);
typedef int        (* e_pfn_XFreePixmap          )(e_Display* pDisplay, e_Pixmap pixmap);
typedef e_Cursor   (* e_pfn_XCreatePixmapCursor  )(e_Display* pDisplay, e_Pixmap source, e_Pixmap mask, e_XColor* pForeground, e_XColor* pBackground, unsigned int x, unsigned int y);
typedef int        (* e_pfn_XFreeCursor          )(e_Display* pDisplay, e_Cursor cursor);
typedef int        (* e_pfn_XDefineCursor        )(e_Display* pDisplay, e_Window w, e_Cursor cursor);
typedef int        (* e_pfn_XUndefineCursor      )(e_Display* pDisplay, e_Window w);
typedef int        (* e_pfn_XFree                )(void* pData);

static e_handle e_gXlibSO = NULL;
static e_pfn_XOpenDisplay          e_XOpenDisplay;
static e_pfn_XCloseDisplay         e_XCloseDisplay;
static e_pfn_XMatchVisualInfo      e_XMatchVisualInfo;
static e_pfn_XCreateColormap       e_XCreateColormap;
static e_pfn_XFreeColormap         e_XFreeColormap;
static e_pfn_XCreateWindow         e_XCreateWindow;
static e_pfn_XDestroyWindow        e_XDestroyWindow;
static e_pfn_XMapWindow            e_XMapWindow;
static e_pfn_XStoreName            e_XStoreName;
static e_pfn_XResizeWindow         e_XResizeWindow;
static e_pfn_XGetWindowAttributes  e_XGetWindowAttributes;
static e_pfn_XSaveContext          e_XSaveContext;
static e_pfn_XDeleteContext        e_XDeleteContext;
static e_pfn_XFindContext          e_XFindContext;
static e_pfn_XSetWMProtocols       e_XSetWMProtocols;
static e_pfn_XInternAtom           e_XInternAtom;
static e_pfn_XPending              e_XPending;
static e_pfn_XNextEvent            e_XNextEvent;
static e_pfn_XSendEvent            e_XSendEvent;
static e_pfn_XRootWindow           e_XRootWindow;
static e_pfn_XDefaultScreen        e_XDefaultScreen;
static e_pfn_XDefaultDepth         e_XDefaultDepth;
static e_pfn_XGrabPointer          e_XGrabPointer;
static e_pfn_XUngrabPointer        e_XUngrabPointer;
static e_pfn_XWarpPointer          e_XWarpPointer;
static e_pfn_XQueryPointer         e_XQueryPointer;
static e_pfn_XCreateBitmapFromData e_XCreateBitmapFromData;
static e_pfn_XFreePixmap           e_XFreePixmap;
static e_pfn_XCreatePixmapCursor   e_XCreatePixmapCursor;
static e_pfn_XFreeCursor           e_XFreeCursor;
static e_pfn_XDefineCursor         e_XDefineCursor;
static e_pfn_XUndefineCursor       e_XUndefineCursor;
static e_pfn_XFree                 e_XFree;


/* We're going to use a global Display object because it just makes everything so much simpler. */
static e_Display* e_gDisplay = NULL;
static e_Atom e_gWMProtocolsAtom     = 0;
static e_Atom e_gWMDeleteWindowAtom  = 0;
static e_Atom e_gWMQuitAtom          = 0;
static e_Pixmap e_gBlankCursorSource = 0;
static e_Cursor e_gBlankCursor       = 0;

static e_result e_platform_uninit(void)
{
    /* Xlib */
    e_XFreeCursor(e_gDisplay, e_gBlankCursor);
    e_XFreePixmap(e_gDisplay, e_gBlankCursorSource);

    if (e_gDisplay != NULL) {
        e_XCloseDisplay(e_gDisplay);
    }

    if (e_gXlibSO != NULL) {
        e_dlclose(e_gXlibSO);
        e_gXlibSO = NULL;
    }

    /* OpenGL */
    if (e_gOpenGLSO != NULL) {
        e_dlclose(e_gOpenGLSO);
        e_gOpenGLSO = NULL;
    }

    return E_SUCCESS;
}

static e_result e_platform_init(void)
{
    /* OpenGL. Allow this to continue initialization if libGL is not installed. */
    {
        const char* pOpenGLSONames[] = {
            "libGL.so.1",
            "libGL.so"
        };

        /* Try loading our OpenGL library using our priority list above. */
        for (size_t i = 0; i < E_COUNTOF(pOpenGLSONames); ++i) {
            e_gOpenGLSO = e_dlopen(pOpenGLSONames[i]);
            if (e_gOpenGLSO != NULL) {
                break;
            }
        }

        if (e_gOpenGLSO != NULL) {
            e_glXChooseVisual = (e_pfn_glXChooseVisual)e_dlsym(e_gOpenGLSO, "glXChooseVisual");
        }
    }

    /* TODO: Need to support different backends here. May want to support Wayland. Might need a compile time macro for this. */

    /* Xlib */
    {
        const char* pXlibSONames[] = {
            "libX11.so.6",
            "libX11.so"
        };
    
        /* Try loading our Xlib library using our priority list above. */
        for (size_t i = 0; i < E_COUNTOF(pXlibSONames); ++i) {
            e_gXlibSO = e_dlopen(pXlibSONames[i]);
            if (e_gXlibSO != NULL) {
                break;
            }
        }
    
        if (e_gXlibSO == NULL) {
            e_platform_uninit();
            return E_ERROR; /* Failed to load the Xlib library. */
        }
    
        e_XOpenDisplay          = (e_pfn_XOpenDisplay         )e_dlsym(e_gXlibSO, "XOpenDisplay");
        e_XCloseDisplay         = (e_pfn_XCloseDisplay        )e_dlsym(e_gXlibSO, "XCloseDisplay");
        e_XMatchVisualInfo      = (e_pfn_XMatchVisualInfo     )e_dlsym(e_gXlibSO, "XMatchVisualInfo");
        e_XCreateColormap       = (e_pfn_XCreateColormap      )e_dlsym(e_gXlibSO, "XCreateColormap");
        e_XFreeColormap         = (e_pfn_XFreeColormap        )e_dlsym(e_gXlibSO, "XFreeColormap");
        e_XCreateWindow         = (e_pfn_XCreateWindow        )e_dlsym(e_gXlibSO, "XCreateWindow");
        e_XDestroyWindow        = (e_pfn_XDestroyWindow       )e_dlsym(e_gXlibSO, "XDestroyWindow");
        e_XMapWindow            = (e_pfn_XMapWindow           )e_dlsym(e_gXlibSO, "XMapWindow");
        e_XStoreName            = (e_pfn_XStoreName           )e_dlsym(e_gXlibSO, "XStoreName");
        e_XResizeWindow         = (e_pfn_XResizeWindow        )e_dlsym(e_gXlibSO, "XResizeWindow");
        e_XGetWindowAttributes  = (e_pfn_XGetWindowAttributes )e_dlsym(e_gXlibSO, "XGetWindowAttributes");
        e_XSaveContext          = (e_pfn_XSaveContext         )e_dlsym(e_gXlibSO, "XSaveContext");
        e_XDeleteContext        = (e_pfn_XDeleteContext       )e_dlsym(e_gXlibSO, "XDeleteContext");
        e_XFindContext          = (e_pfn_XFindContext         )e_dlsym(e_gXlibSO, "XFindContext");
        e_XSetWMProtocols       = (e_pfn_XSetWMProtocols      )e_dlsym(e_gXlibSO, "XSetWMProtocols");
        e_XInternAtom           = (e_pfn_XInternAtom          )e_dlsym(e_gXlibSO, "XInternAtom");
        e_XPending              = (e_pfn_XPending             )e_dlsym(e_gXlibSO, "XPending");
        e_XNextEvent            = (e_pfn_XNextEvent           )e_dlsym(e_gXlibSO, "XNextEvent");
        e_XSendEvent            = (e_pfn_XSendEvent           )e_dlsym(e_gXlibSO, "XSendEvent");
        e_XRootWindow           = (e_pfn_XRootWindow          )e_dlsym(e_gXlibSO, "XRootWindow");
        e_XDefaultScreen        = (e_pfn_XDefaultScreen       )e_dlsym(e_gXlibSO, "XDefaultScreen");
        e_XDefaultDepth         = (e_pfn_XDefaultDepth        )e_dlsym(e_gXlibSO, "XDefaultDepth");
        e_XGrabPointer          = (e_pfn_XGrabPointer         )e_dlsym(e_gXlibSO, "XGrabPointer");
        e_XUngrabPointer        = (e_pfn_XUngrabPointer       )e_dlsym(e_gXlibSO, "XUngrabPointer");
        e_XWarpPointer          = (e_pfn_XWarpPointer         )e_dlsym(e_gXlibSO, "XWarpPointer");
        e_XQueryPointer         = (e_pfn_XQueryPointer        )e_dlsym(e_gXlibSO, "XQueryPointer");
        e_XCreateBitmapFromData = (e_pfn_XCreateBitmapFromData)e_dlsym(e_gXlibSO, "XCreateBitmapFromData");
        e_XFreePixmap           = (e_pfn_XFreePixmap          )e_dlsym(e_gXlibSO, "XFreePixmap");
        e_XCreatePixmapCursor   = (e_pfn_XCreatePixmapCursor  )e_dlsym(e_gXlibSO, "XCreatePixmapCursor");
        e_XFreeCursor           = (e_pfn_XFreeCursor          )e_dlsym(e_gXlibSO, "XFreeCursor");
        e_XDefineCursor         = (e_pfn_XDefineCursor        )e_dlsym(e_gXlibSO, "XDefineCursor");
        e_XUndefineCursor       = (e_pfn_XUndefineCursor      )e_dlsym(e_gXlibSO, "XUndefineCursor");
        e_XFree                 = (e_pfn_XFree                )e_dlsym(e_gXlibSO, "XFree");

        /* Create our display object. */
        e_gDisplay = e_XOpenDisplay(NULL);
        if (e_gDisplay == NULL) {
            e_platform_uninit();
            return E_ERROR; /* Failed to open the display. */
        }

        /* Create our atoms. */
        e_gWMProtocolsAtom    = e_XInternAtom(e_gDisplay, "WM_PROTOCOLS",     E_FALSE);
        e_gWMDeleteWindowAtom = e_XInternAtom(e_gDisplay, "WM_DELETE_WINDOW", E_FALSE);
        e_gWMQuitAtom         = e_XInternAtom(e_gDisplay, "WM_QUIT",          E_FALSE);

        /* We need an invisible cursor. */
        {
            static char bits[] = {0, 0, 0, 0, 0, 0, 0, 0};
            e_XColor black;

            black.red   = 0;
            black.green = 0;
            black.blue  = 0;

            e_gBlankCursorSource = e_XCreateBitmapFromData(e_gDisplay, e_XRootWindow(e_gDisplay, e_XDefaultScreen(e_gDisplay)), bits, 1, 1);
            e_gBlankCursor = e_XCreatePixmapCursor(e_gDisplay, e_gBlankCursorSource, e_gBlankCursorSource, &black, &black, 0, 0);
        }
    }

    return E_SUCCESS;
}

static void* e_platform_get_object(e_platform_object_type type)
{
    switch (type)
    {
        case E_PLATFORM_OBJECT_XLIB_DISPLAY: return e_gDisplay;
        default: break;
    }

    return NULL;
}




struct e_platform_window
{
    e_window* pOwnerWindow;
    e_Window window;
    e_Colormap colormap;
    e_XVisualInfo* pGLVisualInfo; /* Will be NULL if OpenGL is not being used. */
    int sizeX;  /* The size will be updated in response to the ConfigureNotify event. */
    int sizeY;
    e_bool32 isCursorHidden;
    e_bool32 isCursorPinned;
    e_bool32 ignoreNextMouseMoveEvent;
    int pinnedCursorPosX;
    int pinnedCursorPosY;
};

static size_t e_platform_window_sizeof(void)
{
    return sizeof(e_platform_window);
}

static e_result e_platform_window_init_preallocated(const e_window_config* pConfig, e_window* pOwnerWindow, const e_allocation_callbacks* pAllocationCallbacks, e_platform_window* pWindow)
{
    e_XSetWindowAttributes attr;
    e_XVisualInfo defaultVisualInfo;  /* Used when OpenGL is not being used. */
    e_XVisualInfo* pVisualInfo = NULL;

    E_ASSERT(pWindow != NULL);
    E_UNUSED(pAllocationCallbacks);

    /* The owner window is the cross platform e_window object that encompasses the platform-specific window (what we're creating now). */
    pWindow->pOwnerWindow = pOwnerWindow;

    /* The first thing we need is a display. */
    e_gDisplay = e_XOpenDisplay(NULL);
    if (e_gDisplay == NULL) {
        return E_ERROR; /* Failed to open the display. */
    }

    if ((pConfig->flags & E_WINDOW_FLAG_OPENGL) == 0) {
        /* OpenGL is not being used. We need to use the default visual. */
        if (e_XMatchVisualInfo(e_gDisplay, e_XDefaultScreen(e_gDisplay), e_XDefaultDepth(e_gDisplay, e_XDefaultScreen(e_gDisplay)), e_TrueColor, &defaultVisualInfo) == 0) {
            return E_ERROR; /* Failed to find a compatible visual. */
        }

        pVisualInfo = &defaultVisualInfo;
        pWindow->pGLVisualInfo = NULL;
    } else {
        /* OpenGL is being requested. The visual must be compatible. */
        if (e_gOpenGLSO != NULL) {
            static int attribs[] = {
                GLX_RGBA,
                GLX_RED_SIZE,      8,
                GLX_GREEN_SIZE,    8,
                GLX_BLUE_SIZE,     8,
                GLX_ALPHA_SIZE,    8,
                GLX_DEPTH_SIZE,    24,
                GLX_STENCIL_SIZE,  8,
                GLX_DOUBLEBUFFER,
                e_None, e_None
            };

            pVisualInfo = e_glXChooseVisual(e_gDisplay, e_XDefaultScreen(e_gDisplay), attribs);
            if (pVisualInfo == NULL) {
                return E_ERROR; /* Failed to find a compatible visual. */
            }

            pWindow->pGLVisualInfo = pVisualInfo;
        } else {
            return E_ERROR; /* OpenGL not available. */
        }
    }

    /* Now that we have the visual we can create the colormap. */
    pWindow->colormap = e_XCreateColormap(e_gDisplay, e_XRootWindow(e_gDisplay, pVisualInfo->screen), pVisualInfo->visual, e_AllocNone);
    if (pWindow->colormap == 0) {
        return E_ERROR; /* Failed to create the colormap. */
    }

    /* Now we can create the window. */
    attr.colormap     = pWindow->colormap;
    attr.border_pixel = 0;
    attr.event_mask   = e_ExposureMask | e_KeyPressMask | e_KeyReleaseMask | e_ButtonPressMask | e_ButtonReleaseMask | e_PointerMotionMask | e_StructureNotifyMask;

    pWindow->window = e_XCreateWindow(e_gDisplay, e_XRootWindow(e_gDisplay, pVisualInfo->screen), 0, 0, pConfig->sizeX, pConfig->sizeY, 0, pVisualInfo->depth, e_InputOutput, pVisualInfo->visual, e_CWBorderPixel | e_CWColormap | e_CWEventMask, &attr);
    if (pWindow->window == 0) {
        e_XFreeColormap(e_gDisplay, pWindow->colormap);
        return E_ERROR; /* Failed to create the window. */
    }

    /* We need to set the WM_DELETE_WINDOW message. */
    e_XSetWMProtocols(e_gDisplay, pWindow->window, &e_gWMDeleteWindowAtom, 1);

    /* We need to set the window title. */
    e_XStoreName(e_gDisplay, pWindow->window, pConfig->pTitle);

    /* We need to assign our e_window* to the window. */
    e_XSaveContext(e_gDisplay, pWindow->window, 0, (e_XPointer)pWindow);

    /* Show the window unless the caller has explicitly requested that it be hidden. */
    if ((pConfig->flags & E_WINDOW_FLAG_HIDDEN) == 0) {
        e_XMapWindow(e_gDisplay, pWindow->window);
    }

    /* We need to retrieve the initial size of the window so we can intelligently handle the resize event. */
    {
        e_XWindowAttributes windowAttribs;
        e_XGetWindowAttributes(e_gDisplay, pWindow->window, &windowAttribs);

        pWindow->sizeX = windowAttribs.width;
        pWindow->sizeY = windowAttribs.height;
    }

    return E_SUCCESS;
}

static e_result e_platform_window_uninit(e_platform_window* pWindow, const e_allocation_callbacks* pAllocationCallbacks)
{
    E_ASSERT(pWindow != NULL);
    E_UNUSED(pAllocationCallbacks);

    e_XDestroyWindow(e_gDisplay, pWindow->window);
    e_XFreeColormap(e_gDisplay, pWindow->colormap);

    return E_SUCCESS;
}

static void* e_platform_window_get_object(const e_platform_window* pWindow, e_platform_object_type type)
{
    (void)pWindow;
    (void)type;

    switch (type)
    {
        case E_PLATFORM_OBJECT_XLIB_DISPLAY:     return e_platform_get_object(type);
        case E_PLATFORM_OBJECT_XLIB_WINDOW:      return (void*)(e_uintptr)pWindow->window;
        case E_PLATFORM_OBJECT_XLIB_VISUAL_INFO: return pWindow->pGLVisualInfo;
        default: break;
    }

    return NULL;
}

static e_result e_platform_window_set_size(e_platform_window* pWindow, unsigned int sizeX, unsigned int sizeY)
{
    E_ASSERT(pWindow != NULL);

    e_XResizeWindow(e_gDisplay, pWindow->window, sizeX, sizeY);

    return E_SUCCESS;
}

static e_result e_platform_window_capture_cursor(e_platform_window* pWindow)
{
    e_XGrabPointer(e_gDisplay, pWindow->window, E_TRUE, e_ButtonPressMask | e_ButtonReleaseMask | e_PointerMotionMask, e_GrabModeAsync, e_GrabModeAsync, pWindow->window, e_None, e_CurrentTime);
    return E_SUCCESS;
}

static e_result e_platform_window_release_cursor(e_platform_window* pWindow)
{
    (void)pWindow;

    e_XUngrabPointer(e_gDisplay, e_CurrentTime);
    return E_SUCCESS;
}

static e_result e_platform_window_set_cursor_position(e_platform_window* pWindow, int cursorPosX, int cursorPosY)
{
    e_XWarpPointer(e_gDisplay, e_None, pWindow->window, 0, 0, 0, 0, cursorPosX, cursorPosY);
    return E_SUCCESS;
}

static e_result e_platform_window_get_cursor_position(e_platform_window* pWindow, int* pCursorPosX, int* pCursorPosY)
{
    e_Window rootWindow;
    e_Window childWindow;
    int rootX;
    int rootY;
    unsigned int mask;

    if (e_XQueryPointer(e_gDisplay, pWindow->window, &rootWindow, &childWindow, &rootX, &rootY, pCursorPosX, pCursorPosY, &mask) == 0) {
        return E_ERROR;
    }

    return E_SUCCESS;
}

static e_result e_platform_window_show_cursor(e_platform_window* pWindow)
{
    e_XUndefineCursor(e_gDisplay, pWindow->window);
    return E_SUCCESS;
}

static e_result e_platform_window_hide_cursor(e_platform_window* pWindow)
{
    e_XDefineCursor(e_gDisplay, pWindow->window, e_gBlankCursor);
    return E_SUCCESS;
}

static e_result e_platform_window_pin_cursor(e_platform_window* pWindow, int cursorPosX, int cursorPosY)
{
    pWindow->isCursorPinned = E_TRUE;
    pWindow->pinnedCursorPosX = cursorPosX;
    pWindow->pinnedCursorPosY = cursorPosY;
    pWindow->ignoreNextMouseMoveEvent = E_TRUE;

    e_XWarpPointer(e_gDisplay, e_None, pWindow->window, 0, 0, 0, 0, cursorPosX, cursorPosY);

    return E_SUCCESS;
}

static e_result e_platform_window_unpin_cursor(e_platform_window* pWindow)
{
    pWindow->isCursorPinned = E_FALSE;
    return E_SUCCESS;
}

static e_result e_platform_window_post_close_event(e_platform_window* pWindow)
{
    e_XEvent x11Event;

    E_ZERO_OBJECT(&x11Event);
    x11Event.xclient.type         = e_ClientMessage;
    x11Event.xclient.window       = pWindow->window;
    x11Event.xclient.message_type = e_gWMProtocolsAtom;
    x11Event.xclient.format       = 32;
    x11Event.xclient.data.l[0]    = (long)e_gWMDeleteWindowAtom;

    e_XSendEvent(e_gDisplay, x11Event.xclient.window, E_FALSE, e_NoEventMask, (e_XEvent*)&x11Event);

    return E_SUCCESS;
}

static e_result e_platform_window_next_buffer(e_platform_window* pWindow, unsigned int bufferSizeX, unsigned int bufferSizeY, e_window_buffer* pBuffer)
{
    /* TODO: Implement me. */
    (void)pWindow;
    (void)bufferSizeX;
    (void)bufferSizeY;
    (void)pBuffer;

    return E_NOT_IMPLEMENTED;
}


static e_bool32 e_gUseBlockingMainLoop = E_FALSE;

static e_result e_platform_main_loop(int* pExitCode, e_platform_main_loop_iteration_callback iterationCallback, void* pUserData)
{
    E_ASSERT(pExitCode != NULL);
    E_ASSERT(iterationCallback != NULL);

    for (;;) {
        e_event e;
        e_XEvent x11Event;
        e_bool32 hasEvent = E_FALSE;
        e_bool32 receivedQuitMessage = E_FALSE;

        /* Check for events. */
        if (e_gUseBlockingMainLoop) {
            e_XNextEvent(e_gDisplay, &x11Event);
            hasEvent = E_TRUE;
        } else {
            /* Use e_XPending() to determine whether or not we have a message to handle. */
            if (e_XPending(e_gDisplay) > 0) {
                e_XNextEvent(e_gDisplay, &x11Event);
                hasEvent = E_TRUE;
            }
        }

        if (hasEvent) {
            e_platform_window* pPlatformWindow = NULL;
            e_window* pWindow = NULL;

            /* We need to get the `e_window` pointer from the Xlib Window object using e_XFindContext. */
            e_XFindContext(e_gDisplay, x11Event.xany.window, 0, (e_XPointer*)&pPlatformWindow);
            if (pPlatformWindow != NULL) {
                pWindow = pPlatformWindow->pOwnerWindow;
            }

            switch (x11Event.type)
            {
                /* Handle window close. */
                case e_ClientMessage:
                {
                    if (x11Event.xclient.message_type == e_gWMProtocolsAtom) {
                        if (x11Event.xclient.data.l[0] == (long)e_gWMDeleteWindowAtom) {
                            e = e_window_event_init(E_EVENT_CLOSE, pWindow);
                            e_window_handle_event(pWindow, &e);
                        }
                    } else if (x11Event.xclient.message_type == e_gWMQuitAtom) {
                        *pExitCode = x11Event.xclient.data.l[0];
                        receivedQuitMessage = E_TRUE;
                    }
                } break;

                case e_ConfigureNotify:
                {
                    /* The window has been resized. */
                    if (pPlatformWindow != NULL) {
                        /* Check the for resize. */
                        {
                            int eventSizeX = x11Event.xconfigure.width;
                            int eventSizeY = x11Event.xconfigure.height;

                            if (pPlatformWindow->sizeX != eventSizeX || pPlatformWindow->sizeY != eventSizeY) {
                                pPlatformWindow->sizeX = eventSizeX;
                                pPlatformWindow->sizeY = eventSizeY;

                                e = e_window_event_init(E_EVENT_SIZE, pWindow);
                                e.data.size.x = eventSizeX;
                                e.data.size.y = eventSizeY;
                                e_window_handle_event(pWindow, &e);
                            }
                        }
                    }
                } break;
            }

            if (receivedQuitMessage) {
                break;
            }
        }

        /* Call the iteration callback. */
        if (iterationCallback(pUserData) != E_SUCCESS) {
            break;
        }
    }

    return E_SUCCESS;
}

static e_result e_platform_exit_main_loop(int exitCode)
{
    /* Post our custom quit message to the main loop. */
    e_XEvent x11Event;

    E_ZERO_OBJECT(&x11Event);
    x11Event.xclient.type         = e_ClientMessage;
    x11Event.xclient.window       = e_XRootWindow(e_gDisplay, e_XDefaultScreen(e_gDisplay));
    x11Event.xclient.message_type = e_gWMQuitAtom;
    x11Event.xclient.format       = 32;
    x11Event.xclient.data.l[0]    = exitCode;

    if (e_XSendEvent(e_gDisplay, x11Event.xclient.window, E_FALSE, e_SubstructureRedirectMask | e_SubstructureNotifyMask, (e_XEvent*)&x11Event) == 0) {
        return E_ERROR;
    }

    return E_SUCCESS;
}

static e_result e_platform_wake_main_loop(void)
{
    /* We might be using a blocking loop. We'll need to post an event to wake it up. */
    e_XEvent x11Event;

    E_ZERO_OBJECT(&x11Event);
    x11Event.xclient.type         = e_ClientMessage;
    x11Event.xclient.window       = e_XRootWindow(e_gDisplay, e_XDefaultScreen(e_gDisplay));
    x11Event.xclient.message_type = e_None;
    x11Event.xclient.format       = 32;
    x11Event.xclient.data.l[0]    = 0;

    e_XSendEvent(e_gDisplay, x11Event.xclient.window, E_FALSE, e_NoEventMask, (e_XEvent*)&x11Event);

    return E_SUCCESS;
}

static e_result e_platform_set_main_loop_blocking(e_bool32 blocking)
{
    /* Very minor optimization. Don't do anything if the blocking mode is not changing. This saves us from posting a message to the main loop. */
    if (e_gUseBlockingMainLoop == blocking) {
        return E_SUCCESS;
    }

    e_gUseBlockingMainLoop = blocking;
    
    /* The loop needs to be woken up in case it's currently blocking. */
    return e_platform_wake_main_loop();
}
#endif  /* E_POSIX */


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



E_API e_handle e_dlopen(const char* pFilePath)
{
#if defined(E_WIN32)
    return (e_handle)LoadLibraryA(pFilePath);
#else
    return (e_handle)dlopen(pFilePath, RTLD_LAZY);
#endif
}

E_API void e_dlclose(e_handle hLibrary)
{
#if defined(E_WIN32)
    FreeLibrary((HMODULE)hLibrary);
#else
    dlclose(hLibrary);
#endif
}

E_API e_proc e_dlsym(e_handle hLibrary, const char* pSymbol)
{
#if defined(E_WIN32)
    return (e_proc)GetProcAddress((HMODULE)hLibrary, pSymbol);
#else
    /* When compiling with -Wpedantic we'll get warnings about converting from a void* to a function pointer. It's safe - just disable the warning. */
    #if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wpedantic"
    #endif
    return (e_proc)dlsym(hLibrary, pSymbol);
    #if defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))
        #pragma GCC diagnostic pop
    #endif
#endif
}

E_API e_result e_dlerror(char* pOutMessage, size_t messageSizeInBytes)
{
#if defined(E_WIN32)
    DWORD errorCode = GetLastError();
    if (errorCode == 0) {
        return E_SUCCESS;
    }

    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), pOutMessage, (DWORD)messageSizeInBytes, NULL);
    return E_ERROR;
#else
    const char* pError = dlerror();
    if (pError == NULL) {
        return E_SUCCESS;
    }

    c89str_strncpy_s(pOutMessage, messageSizeInBytes, pError, messageSizeInBytes);
    return E_ERROR;
#endif
}



/* ==== BEG e_misc.c ==== */
static E_INLINE void e_swap(void* a, void* b, size_t sz)
{
    char* _a = (char*)a;
    char* _b = (char*)b;

    while (sz > 0) {
        char temp = *_a;
        *_a++ = *_b;
        *_b++ = temp;
        sz -= 1;
    }
}

E_API void e_sort(void* pList, size_t count, size_t stride, int (*compareProc)(void*, const void*, const void*), void* pUserData)
{
    /* Simple insert sort for now. Will improve on this later. */
    size_t i;
    size_t j;

    for (i = 1; i < count; i += 1) {
        for (j = i; j > 0; j -= 1) {
            void* pA = (char*)pList + (j - 1) * stride;
            void* pB = (char*)pList + j * stride;

            if (compareProc(pUserData, pA, pB) <= 0) {
                break;
            }

            e_swap(pA, pB, stride);
        }
    }
}

E_API void* e_binary_search(const void* pKey, const void* pList, size_t count, size_t stride, int (*compareProc)(void*, const void*, const void*), void* pUserData)
{
    size_t iStart;
    size_t iEnd;
    size_t iMid;

    if (count == 0) {
        return NULL;
    }

    iStart = 0;
    iEnd = count - 1;

    while (iStart <= iEnd) {
        int compareResult;

        iMid = iStart + (iEnd - iStart) / 2;

        compareResult = compareProc(pUserData, pKey, (char*)pList + (iMid * stride));
        if (compareResult < 0) {
            iEnd = iMid - 1;
        } else if (compareResult > 0) {
            iStart = iMid + 1;
        } else {
            return (void*)((char*)pList + (iMid * stride));
        }
    }

    return NULL;
}

E_API void* e_linear_search(const void* pKey, const void* pList, size_t count, size_t stride, int (*compareProc)(void*, const void*, const void*), void* pUserData)
{
    size_t i;

    for (i = 0; i < count; i+= 1) {
        int compareResult = compareProc(pUserData, pKey, (char*)pList + (i * stride));
        if (compareResult == 0) {
            return (void*)((char*)pList + (i * stride));
        }
    }

    return NULL;
}

E_API void* e_sorted_search(const void* pKey, const void* pList, size_t count, size_t stride, int (*compareProc)(void*, const void*, const void*), void* pUserData)
{
    const size_t threshold = 10;

    if (count < threshold) {
        return e_linear_search(pKey, pList, count, stride, compareProc, pUserData);
    } else {
        return e_binary_search(pKey, pList, count, stride, compareProc, pUserData);
    }
}
/* ==== END e_misc.c ==== */



/* ==== BEG e_timer.h ==== */
#if defined(E_WIN32) && !defined(E_POSIX)
    static LARGE_INTEGER e_gTimerFrequency;   /* <-- Initialized to zero since it's static. */
    E_API void e_timer_init(e_timer* pTimer)
    {
        LARGE_INTEGER counter;

        if (e_gTimerFrequency.QuadPart == 0) {
            QueryPerformanceFrequency(&e_gTimerFrequency);
        }

        QueryPerformanceCounter(&counter);
        pTimer->counter = counter.QuadPart;
    }

    E_API double e_timer_get_time_in_seconds(e_timer* pTimer)
    {
        LARGE_INTEGER counter;
        if (!QueryPerformanceCounter(&counter)) {
            return 0;
        }

        return (double)(counter.QuadPart - pTimer->counter) / e_gTimerFrequency.QuadPart;
    }
#elif defined(E_APPLE) && (__MAC_OS_X_VERSION_MIN_REQUIRED < 101200)
    static e_uint64 e_gTimerFrequency = 0;
    E_API void e_timer_init(e_timer* pTimer)
    {
        mach_timebase_info_data_t baseTime;
        mach_timebase_info(&baseTime);
        e_gTimerFrequency = (baseTime.denom * 1e9) / baseTime.numer;

        pTimer->counter = mach_absolute_time();
    }

    E_API double e_timer_get_time_in_seconds(e_timer* pTimer)
    {
        e_uint64 newTimeCounter = mach_absolute_time();
        e_uint64 oldTimeCounter = pTimer->counter;

        return (newTimeCounter - oldTimeCounter) / e_gTimerFrequency;
    }
#elif defined(E_EMSCRIPTEN)
    E_API MA_INLINE void e_timer_init(e_timer* pTimer)
    {
        pTimer->counterD = emscripten_get_now();
    }

    E_API MA_INLINE double e_timer_get_time_in_seconds(e_timer* pTimer)
    {
        return (emscripten_get_now() - pTimer->counterD) / 1000;    /* Emscripten is in milliseconds. */
    }
#else
    #if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 199309L
        #if defined(CLOCK_MONOTONIC)
            #define MA_CLOCK_ID CLOCK_MONOTONIC
        #else
            #define MA_CLOCK_ID CLOCK_REALTIME
        #endif

        E_API void e_timer_init(e_timer* pTimer)
        {
            struct timespec newTime;
            clock_gettime(MA_CLOCK_ID, &newTime);

            pTimer->counter = (newTime.tv_sec * 1000000000) + newTime.tv_nsec;
        }

        E_API double e_timer_get_time_in_seconds(e_timer* pTimer)
        {
            e_uint64 newTimeCounter;
            e_uint64 oldTimeCounter;

            struct timespec newTime;
            clock_gettime(MA_CLOCK_ID, &newTime);

            newTimeCounter = (newTime.tv_sec * 1000000000) + newTime.tv_nsec;
            oldTimeCounter = pTimer->counter;

            return (newTimeCounter - oldTimeCounter) / 1000000000.0;
        }
    #else
        E_API void e_timer_init(e_timer* pTimer)
        {
            struct timeval newTime;
            gettimeofday(&newTime, NULL);

            pTimer->counter = (newTime.tv_sec * 1000000) + newTime.tv_usec;
        }

        E_API double e_timer_get_time_in_seconds(e_timer* pTimer)
        {
            e_uint64 newTimeCounter;
            e_uint64 oldTimeCounter;

            struct timeval newTime;
            gettimeofday(&newTime, NULL);

            newTimeCounter = (newTime.tv_sec * 1000000) + newTime.tv_usec;
            oldTimeCounter = pTimer->counter;

            return (newTimeCounter - oldTimeCounter) / 1000000.0;
        }
    #endif
#endif
/* ==== END e_timer.h ==== */



/* ==== BEG e_net.c ==== */
/*
Unfortunately the Windows ecosystem does not work well because winsock2.h conflicts with windows.h.
Since a simple build system is one of the primary goals of this project, we're going to have to do
our own namespaced declarations of the functions and data structures we need.

An added complication here is that we would normally need to link to ws2_32.dll, but again, since
we're trying to keep the build system simple we'll need to do some runtime linking with the Windows
build.
*/
#if defined(E_WIN32)
typedef struct E_WSADATA
{
    e_uint16 wVersion;
    e_uint16 wHighVersion;
#if defined(_WIN64)
    e_uint16 iMaxSockets;
    e_uint16 iMaxUdpDg;
    char *lpVendorInfo;
    char szDescription[257];
    char szSystemStatus[129];
#else
    char szDescription[257];
    char szSystemStatus[129];
    e_uint16 iMaxSockets;
    e_uint16 iMaxUdpDg;
    char *lpVendorInfo;
#endif
} E_WSADATA;
#endif


/* We need to declare a few function pointer types for runtime linking on Windows. */
#if defined(E_WIN32)
typedef int (E_WSAAPI * e_pfn_WSAGetLastError)(void);
typedef int (E_WSAAPI * e_pfn_WSACleanup)(void);
typedef int (E_WSAAPI * e_pfn_WSAStartup)(e_uint16 wVersionRequested, E_WSADATA* lpWSAData);

static e_pfn_WSAGetLastError    e_WSAGetLastError;
static e_pfn_WSACleanup         e_WSACleanup;
static e_pfn_WSAStartup         e_WSAStartup;

e_pfn_socket         e_net_socket;
e_pfn_closesocket    e_net_closesocket;
e_pfn_shutdown       e_net_shutdown;
e_pfn_bind           e_net_bind;
e_pfn_listen         e_net_listen;
e_pfn_accept         e_net_accept;
e_pfn_connect        e_net_connect;
e_pfn_send           e_net_send;
e_pfn_recv           e_net_recv;
e_pfn_ioctlsocket    e_net_ioctlsocket;
e_pfn_select         e_net_select;
e_pfn_sendto         e_net_sendto;
e_pfn_recvfrom       e_net_recvfrom;
e_pfn_gethostname    e_net_gethostname;
e_pfn_getnameinfo    e_net_getnameinfo;
e_pfn_inet_pton      e_net_inet_pton;
e_pfn_inet_ntop      e_net_inet_ntop;
e_pfn_htons          e_net_htons;
e_pfn_ntohs          e_net_ntohs;
e_pfn_htonl          e_net_htonl;
e_pfn_ntohl          e_net_ntohl;
e_pfn_getaddrinfo    e_net_getaddrinfo;
e_pfn_freeaddrinfo   e_net_freeaddrinfo;

/* The function below is used to dynamically load the WinSock library. It needs to be reference counted. */
static int e_gWinSockInitCount = 0;
static E_WSADATA e_gWinSockData;
static e_handle hWinSockDLL = NULL;

static e_result e_winsock_init()
{
    if (e_gWinSockInitCount == 0) {
        E_ASSERT(hWinSockDLL == NULL);

        hWinSockDLL = e_dlopen("ws2_32.dll");
        if (hWinSockDLL == NULL) {
            return E_ERROR;
        }
    
        e_WSAGetLastError = (e_pfn_WSAGetLastError)e_dlsym(hWinSockDLL, "WSAGetLastError");
        e_WSACleanup      = (e_pfn_WSACleanup)     e_dlsym(hWinSockDLL, "WSACleanup");
        e_WSAStartup      = (e_pfn_WSAStartup)     e_dlsym(hWinSockDLL, "WSAStartup");
        e_net_socket      = (e_pfn_socket)         e_dlsym(hWinSockDLL, "socket");
        e_net_closesocket = (e_pfn_closesocket)    e_dlsym(hWinSockDLL, "closesocket");
        e_net_shutdown    = (e_pfn_shutdown)       e_dlsym(hWinSockDLL, "shutdown");
        e_net_bind        = (e_pfn_bind)           e_dlsym(hWinSockDLL, "bind");
        e_net_listen      = (e_pfn_listen)         e_dlsym(hWinSockDLL, "listen");
        e_net_accept      = (e_pfn_accept)         e_dlsym(hWinSockDLL, "accept");
        e_net_connect     = (e_pfn_connect)        e_dlsym(hWinSockDLL, "connect");
        e_net_send        = (e_pfn_send)           e_dlsym(hWinSockDLL, "send");
        e_net_recv        = (e_pfn_recv)           e_dlsym(hWinSockDLL, "recv");
        e_net_ioctlsocket = (e_pfn_ioctlsocket)    e_dlsym(hWinSockDLL, "ioctlsocket");
        e_net_select      = (e_pfn_select)         e_dlsym(hWinSockDLL, "select");
        e_net_sendto      = (e_pfn_sendto)         e_dlsym(hWinSockDLL, "sendto");
        e_net_recvfrom    = (e_pfn_recvfrom)       e_dlsym(hWinSockDLL, "recvfrom");
        e_net_gethostname = (e_pfn_gethostname)    e_dlsym(hWinSockDLL, "gethostname");
        e_net_getnameinfo = (e_pfn_getnameinfo)    e_dlsym(hWinSockDLL, "getnameinfo");
        e_net_inet_pton   = (e_pfn_inet_pton)      e_dlsym(hWinSockDLL, "inet_pton");
        e_net_inet_ntop   = (e_pfn_inet_ntop)      e_dlsym(hWinSockDLL, "inet_ntop");
        e_net_htons       = (e_pfn_htons)          e_dlsym(hWinSockDLL, "htons");
        e_net_ntohs       = (e_pfn_ntohs)          e_dlsym(hWinSockDLL, "ntohs");
        e_net_htonl       = (e_pfn_htonl)          e_dlsym(hWinSockDLL, "htonl");
        e_net_ntohl       = (e_pfn_ntohl)          e_dlsym(hWinSockDLL, "ntohl");
        e_net_getaddrinfo = (e_pfn_getaddrinfo)    e_dlsym(hWinSockDLL, "getaddrinfo");
        e_net_freeaddrinfo= (e_pfn_freeaddrinfo)   e_dlsym(hWinSockDLL, "freeaddrinfo");
    
        /* The WinSock library should be loaded at this point. */
        if (e_WSAStartup(MAKEWORD(2, 2), &e_gWinSockData) != 0) {
            e_dlclose(hWinSockDLL);
            return E_ERROR;
        }
    }

    e_gWinSockInitCount += 1;

    return E_SUCCESS;
}

static void e_winsock_uninit()
{
    if (e_gWinSockInitCount == 0) {
        E_ASSERT(E_FALSE);  /* init/uninit mismatch. */
        return;
    }

    if (e_gWinSockInitCount == 1) {
        E_ASSERT(hWinSockDLL != NULL);
    
        e_WSACleanup();
        e_dlclose(hWinSockDLL);
    }

    e_gWinSockInitCount -= 1;
}
#endif

E_API e_result e_net_init(void)
{
#if defined(E_WIN32)
    return e_winsock_init();
#else
    return E_SUCCESS;
#endif
}

E_API void e_net_uninit(void)
{
#if defined(E_WIN32)
    e_winsock_uninit();
#else
    /* Nothing to do. */
#endif
}

E_API int e_net_get_last_error(void)
{
#if defined(E_WIN32)
    int err = e_WSAGetLastError();
    switch (err) {
        case WSAEWOULDBLOCK: return EWOULDBLOCK;
        default: return err;
    }
#else
    return errno;
#endif
}

E_API int e_net_set_non_blocking(E_SOCKET socket, e_bool32 nonBlocking)
{
#if defined(E_WIN32)
    unsigned long value = nonBlocking ? 1 : 0;
    return e_net_ioctlsocket(socket, FIONBIO, &value);
#else
    int flags = fcntl(socket, F_GETFL, 0);
    if (flags == -1) {
        return -1;
    }

    if (nonBlocking) {
        flags |=  O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }

    return fcntl(socket, F_SETFL, flags);
#endif
}
/* ==== END e_net.c ==== */



/* ==== BEG e_threading.c ==== */
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



struct e_mutex
{
    c89mtx_t mtx;
    e_bool32 freeOnUninit;
};

E_API size_t e_mutex_alloc_size()
{
    return sizeof(e_mutex);
}

E_API e_result e_mutex_init_preallocated(e_mutex* pMutex)
{
    e_result result;

    if (pMutex == NULL) {
        return E_INVALID_ARGS;
    }

    result = e_result_from_c89thread(c89mtx_init(&pMutex->mtx, c89mtx_plain));
    if (result != E_SUCCESS) {
        return result;
    }

    return E_SUCCESS;
}

E_API e_result e_mutex_init(const e_allocation_callbacks* pAllocationCallbacks, e_mutex** ppMutex)
{
    e_result result;
    e_mutex* pMutex;

    if (ppMutex == NULL) {
        return E_INVALID_ARGS;
    }

    *ppMutex = NULL;

    pMutex = (e_mutex*)e_malloc(e_mutex_alloc_size(), pAllocationCallbacks);
    if (pMutex == NULL) {
        return E_OUT_OF_MEMORY;
    }

    result = e_mutex_init_preallocated(pMutex);
    if (result != E_SUCCESS) {
        e_free(pMutex, pAllocationCallbacks);
        return result;
    }

    pMutex->freeOnUninit = E_TRUE;

    *ppMutex = pMutex;
    return E_SUCCESS;
}

E_API void e_mutex_uninit(e_mutex* pMutex, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pMutex == NULL) {
        return;
    }

    c89mtx_destroy((c89mtx_t*)pMutex);

    if (pMutex->freeOnUninit) {
        e_free(pMutex, pAllocationCallbacks);
    }
}

E_API void e_mutex_lock(e_mutex* pMutex)
{
    c89mtx_lock(&pMutex->mtx);
}

E_API void e_mutex_unlock(e_mutex* pMutex)
{
    c89mtx_unlock(&pMutex->mtx);
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




/* ==== BEG e_deflate.c ==== */
/*
This is all taken from the old public domain version of miniz.c but restyled for consistency with
the rest of the code base.
*/
#ifdef _MSC_VER
   #define E_DEFLATE_MACRO_END while (0, 0)
#else
   #define E_DEFLATE_MACRO_END while (0)
#endif

#define E_DEFLATE_CR_BEGIN switch(pDecompressor->state) { case 0:
#define E_DEFLATE_CR_RETURN(stateIndex, result) do { status = result; pDecompressor->state = stateIndex; goto common_exit; case stateIndex:; } E_DEFLATE_MACRO_END
#define E_DEFLATE_CR_RETURN_FOREVER(stateIndex, result) do { for (;;) { E_DEFLATE_CR_RETURN(stateIndex, result); } } E_DEFLATE_MACRO_END
#define E_DEFLATE_CR_FINISH }

/*
TODO: If the caller has indicated that there's no more input, and we attempt to read beyond the input buf, then something is wrong with the input because the inflator never
reads ahead more than it needs to. Currently E_DEFLATE_GET_BYTE() pads the end of the stream with 0's in this scenario.
*/
#define E_DEFLATE_GET_BYTE(stateIndex, c) do { \
    if (pInputBufferCurrent >= pInputBufferEnd) { \
        for (;;) { \
            if (flags & E_DEFLATE_FLAG_HAS_MORE_INPUT) { \
                E_DEFLATE_CR_RETURN(stateIndex, E_NEEDS_MORE_INPUT); \
                if (pInputBufferCurrent < pInputBufferEnd) { \
                    c = *pInputBufferCurrent++; \
                    break; \
                } \
            } else { \
                c = 0; \
                break; \
            } \
        } \
    } else c = *pInputBufferCurrent++; } E_DEFLATE_MACRO_END

#define E_DEFLATE_NEED_BITS(stateIndex, n) do { unsigned int c; E_DEFLATE_GET_BYTE(stateIndex, c); bitBuffer |= (((e_deflate_bitBufferfer)c) << bitCount); bitCount += 8; } while (bitCount < (unsigned int)(n))
#define E_DEFLATE_SKIP_BITS(stateIndex, n) do { if (bitCount < (unsigned int)(n)) { E_DEFLATE_NEED_BITS(stateIndex, n); } bitBuffer >>= (n); bitCount -= (n); } E_DEFLATE_MACRO_END
#define E_DEFLATE_GET_BITS(stateIndex, b, n) do { if (bitCount < (unsigned int)(n)) { E_DEFLATE_NEED_BITS(stateIndex, n); } b = bitBuffer & ((1 << (n)) - 1); bitBuffer >>= (n); bitCount -= (n); } E_DEFLATE_MACRO_END

/*
E_DEFLATE_HUFF_BITBUF_FILL() is only used rarely, when the number of bytes remaining in the input buffer falls below 2.
It reads just enough bytes from the input stream that are needed to decode the next Huffman code (and absolutely no more). It works by trying to fully decode a
Huffman code by using whatever bits are currently present in the bit buffer. If this fails, it reads another byte, and tries again until it succeeds or until the
bit buffer contains >=15 bits (deflate's max. Huffman code size).
*/
#define E_DEFLATE_HUFF_BITBUF_FILL(stateIndex, pHuff) \
    do { \
        temp = (pHuff)->lookup[bitBuffer & (E_DEFLATE_FAST_LOOKUP_SIZE - 1)]; \
        if (temp >= 0) { \
            codeLen = temp >> 9; \
            if ((codeLen) && (bitCount >= codeLen)) { \
                break; \
            } \
        } else if (bitCount > E_DEFLATE_FAST_LOOKUP_BITS) { \
            codeLen = E_DEFLATE_FAST_LOOKUP_BITS; \
            do { \
               temp = (pHuff)->tree[~temp + ((bitBuffer >> codeLen++) & 1)]; \
            } while ((temp < 0) && (bitCount >= (codeLen + 1))); \
            if (temp >= 0) {\
                break; \
            } \
        } \
        E_DEFLATE_GET_BYTE(stateIndex, c); \
        bitBuffer |= (((e_deflate_bitBufferfer)c) << bitCount); \
        bitCount += 8; \
    } while (bitCount < 15);

/*
E_DEFLATE_HUFF_DECODE() decodes the next Huffman coded symbol. It's more complex than you would initially expect because the zlib API expects the decompressor to never read
beyond the final byte of the deflate stream. (In other words, when this macro wants to read another byte from the input, it REALLY needs another byte in order to fully
decode the next Huffman code.) Handling this properly is particularly important on raw deflate (non-zlib) streams, which aren't followed by a byte aligned adler-32.
The slow path is only executed at the very end of the input buffer.
*/
#define E_DEFLATE_HUFF_DECODE(stateIndex, sym, pHuff) do { \
    int temp; \
    unsigned int codeLen; \
    unsigned int c; \
    if (bitCount < 15) { \
        if ((pInputBufferEnd - pInputBufferCurrent) < 2) { \
            E_DEFLATE_HUFF_BITBUF_FILL(stateIndex, pHuff); \
        } else { \
            bitBuffer |= (((e_deflate_bitBufferfer)pInputBufferCurrent[0]) << bitCount) | (((e_deflate_bitBufferfer)pInputBufferCurrent[1]) << (bitCount + 8)); \
            pInputBufferCurrent += 2; \
            bitCount += 16; \
        } \
    } \
    if ((temp = (pHuff)->lookup[bitBuffer & (E_DEFLATE_FAST_LOOKUP_SIZE - 1)]) >= 0) { \
        codeLen = temp >> 9, temp &= 511; \
    } \
    else { \
        codeLen = E_DEFLATE_FAST_LOOKUP_BITS; do { temp = (pHuff)->tree[~temp + ((bitBuffer >> codeLen++) & 1)]; } while (temp < 0); \
    } sym = temp; bitBuffer >>= codeLen; bitCount -= codeLen; } E_DEFLATE_MACRO_END


#define e_deflate_init(r) do { (r)->state = 0; } E_DEFLATE_MACRO_END
#define e_deflate_get_adler32(r) (r)->checkAdler32

E_API e_result e_deflate_decompressor_init(e_deflate_decompressor* pDecompressor)
{
    if (pDecompressor == NULL) {
        return E_INVALID_ARGS;
    }

    e_deflate_init(pDecompressor);

    return E_SUCCESS;
}

E_API e_result e_deflate_decompress(e_deflate_decompressor* pDecompressor, const e_uint8* pInputBuffer, size_t* pInputBufferSize, e_uint8* pOutputBufferStart, e_uint8* pOutputBufferNext, size_t* pOutputBufferSize, const e_uint32 flags)
{
    static const int sLengthBase[31] =
    {
        3,  4,  5,  6,   7,   8,   9,   10,  11,  13,
        15, 17, 19, 23,  27,  31,  35,  43,  51,  59,
        67, 83, 99, 115, 131, 163, 195, 227, 258, 0,
        0
    };
    static const int sLengthExtra[31] =
    {
        0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 2, 2, 2, 2,
        3, 3, 3, 3, 4, 4, 4, 4,
        5, 5, 5, 5, 0, 0, 0
    };
    static const int sDistBase[32] =
    {
        1,   2,   3,   4,   5,    7,    9,    13,   17,   25,   33,   49,    65,    97,    129, 193,
        257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577, 0,   0
    };
    static const int sDistExtra[32] =
    {
        0,  0,  0,  0,  1,  1,  2,  2,
        3,  3,  4,  4,  5,  5,  6,  6,
        7,  7,  8,  8,  9,  9,  10, 10,
        11, 11, 12, 12, 13, 13
    };
    static const e_uint8 sLengthDeZigZag[19] =
    {
        16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
    };
    static const int sMinTableSizes[3] = { 257, 1, 4 };

    e_result status = E_ERROR;
    e_uint32 bitCount;
    e_uint32 dist;
    e_uint32 counter;
    e_uint32 extraCount;
    e_deflate_bitBufferfer bitBuffer;
    const e_uint8* pInputBufferCurrent = pInputBuffer;
    const e_uint8* const pInputBufferEnd = pInputBuffer + *pInputBufferSize;
    e_uint8 *pOutputBufferCurrent = pOutputBufferNext;
    e_uint8 *const pOutputBufferEnd = pOutputBufferNext + *pOutputBufferSize;
    size_t outputBufferSizeMask = (flags & E_DEFLATE_FLAG_USING_NON_WRAPPING_OUTPUT_BUF) ? (size_t)-1 : ((pOutputBufferNext - pOutputBufferStart) + *pOutputBufferSize) - 1, distFromOutBufStart;

    /* Ensure the output buffer's size is a power of 2, unless the output buffer is large enough to hold the entire output file (in which case it doesn't matter). */
    if (((outputBufferSizeMask + 1) & outputBufferSizeMask) || (pOutputBufferNext < pOutputBufferStart)) {
        *pInputBufferSize = *pOutputBufferSize = 0;
        return E_INVALID_ARGS;
    }
    
    bitCount = pDecompressor->bitCount; bitBuffer = pDecompressor->bitBuffer; dist = pDecompressor->dist; counter = pDecompressor->counter; extraCount = pDecompressor->extraCount; distFromOutBufStart = pDecompressor->distFromOutBufStart;
    E_DEFLATE_CR_BEGIN
    
    bitBuffer = bitCount = dist = counter = extraCount = pDecompressor->zhdr0 = pDecompressor->zhdr1 = 0;
    pDecompressor->zAdler32 = pDecompressor->checkAdler32 = 1;

    if (flags & E_DEFLATE_FLAG_PARSE_ZLIB_HEADER) {
        E_DEFLATE_GET_BYTE(1, pDecompressor->zhdr0);
        E_DEFLATE_GET_BYTE(2, pDecompressor->zhdr1);
        
        counter = (((pDecompressor->zhdr0 * 256 + pDecompressor->zhdr1) % 31 != 0) || (pDecompressor->zhdr1 & 32) || ((pDecompressor->zhdr0 & 15) != 8));
        
        if (!(flags & E_DEFLATE_FLAG_USING_NON_WRAPPING_OUTPUT_BUF)) {
            counter |= (((1U << (8U + (pDecompressor->zhdr0 >> 4))) > 32768U) || ((outputBufferSizeMask + 1) < (size_t)(1U << (8U + (pDecompressor->zhdr0 >> 4)))));
        }
        
        if (counter) {
            E_DEFLATE_CR_RETURN_FOREVER(36, E_ERROR);
        }
    }

    do {
        E_DEFLATE_GET_BITS(3, pDecompressor->final, 3); pDecompressor->type = pDecompressor->final >> 1;

        if (pDecompressor->type == 0) {
            E_DEFLATE_SKIP_BITS(5, bitCount & 7);

            for (counter = 0; counter < 4; ++counter) {
                if (bitCount) {
                    E_DEFLATE_GET_BITS(6, pDecompressor->rawHeader[counter], 8);
                } else {
                    E_DEFLATE_GET_BYTE(7, pDecompressor->rawHeader[counter]);
                }
            }

            if ((counter = (pDecompressor->rawHeader[0] | (pDecompressor->rawHeader[1] << 8))) != (unsigned int)(0xFFFF ^ (pDecompressor->rawHeader[2] | (pDecompressor->rawHeader[3] << 8)))) {
                E_DEFLATE_CR_RETURN_FOREVER(39, E_ERROR);
            }

            while ((counter) && (bitCount)) {
                E_DEFLATE_GET_BITS(51, dist, 8);

                while (pOutputBufferCurrent >= pOutputBufferEnd) {
                    E_DEFLATE_CR_RETURN(52, E_HAS_MORE_OUTPUT);
                }

                *pOutputBufferCurrent++ = (e_uint8)dist;
                counter--;
            }

            while (counter) {
                size_t n;
                
                while (pOutputBufferCurrent >= pOutputBufferEnd) {
                    E_DEFLATE_CR_RETURN(9, E_HAS_MORE_OUTPUT);
                }

                while (pInputBufferCurrent >= pInputBufferEnd) {
                    if (flags & E_DEFLATE_FLAG_HAS_MORE_INPUT) {
                        E_DEFLATE_CR_RETURN(38, E_NEEDS_MORE_INPUT);
                    } else {
                        E_DEFLATE_CR_RETURN_FOREVER(40, E_ERROR);
                    }
                }

                n = E_MIN(E_MIN((size_t)(pOutputBufferEnd - pOutputBufferCurrent), (size_t)(pInputBufferEnd - pInputBufferCurrent)), counter);
                E_COPY_MEMORY(pOutputBufferCurrent, pInputBufferCurrent, n); pInputBufferCurrent += n; pOutputBufferCurrent += n; counter -= (unsigned int)n;
            }
        }
        else if (pDecompressor->type == 3) {
            E_DEFLATE_CR_RETURN_FOREVER(10, E_ERROR);
        } else {
            if (pDecompressor->type == 1) {
                e_uint8 *p = pDecompressor->tables[0].codeSize;
                unsigned int i;

                pDecompressor->tableSizes[0] = 288;
                pDecompressor->tableSizes[1] = 32;
                memset(pDecompressor->tables[1].codeSize, 5, 32);

                for (i = 0; i <= 143; ++i) {
                    *p++ = 8;
                }
                
                for (; i <= 255; ++i) {
                    *p++ = 9;
                }
                
                for (; i <= 279; ++i) {
                    *p++ = 7;
                }
                
                for (; i <= 287; ++i) {
                    *p++ = 8;
                }
            } else {
                for (counter = 0; counter < 3; counter++) {
                    E_DEFLATE_GET_BITS(11, pDecompressor->tableSizes[counter], "\05\05\04"[counter]);
                    pDecompressor->tableSizes[counter] += sMinTableSizes[counter];
                }

                E_ZERO_MEMORY(&pDecompressor->tables[2].codeSize, sizeof(pDecompressor->tables[2].codeSize));

                for (counter = 0; counter < pDecompressor->tableSizes[2]; counter++) {
                    unsigned int s;
                    E_DEFLATE_GET_BITS(14, s, 3);
                    pDecompressor->tables[2].codeSize[sLengthDeZigZag[counter]] = (e_uint8)s;
                }

                pDecompressor->tableSizes[2] = 19;
            }

            for (; (int)pDecompressor->type >= 0; pDecompressor->type--) {
                int tree_next;
                int tree_cur;
                e_deflate_huff_table *pTable;
                unsigned int i;
                unsigned int j;
                unsigned int usedSyms;
                unsigned int total;
                unsigned int symIndex;
                unsigned int nextCode[17];
                unsigned int totalSyms[16];
                
                pTable = &pDecompressor->tables[pDecompressor->type];
                
                E_ZERO_MEMORY(totalSyms, sizeof(totalSyms));
                E_ZERO_MEMORY(pTable->lookup, sizeof(pTable->lookup));
                E_ZERO_MEMORY(pTable->tree, sizeof(pTable->tree));

                for (i = 0; i < pDecompressor->tableSizes[pDecompressor->type]; ++i) {
                    totalSyms[pTable->codeSize[i]]++;
                }

                usedSyms = 0;
                total = 0;
                nextCode[0] = nextCode[1] = 0;

                for (i = 1; i <= 15; ++i) {
                    usedSyms += totalSyms[i];
                    nextCode[i + 1] = (total = ((total + totalSyms[i]) << 1));
                }

                if ((65536 != total) && (usedSyms > 1)) {
                    E_DEFLATE_CR_RETURN_FOREVER(35, E_ERROR);
                }

                for (tree_next = -1, symIndex = 0; symIndex < pDecompressor->tableSizes[pDecompressor->type]; ++symIndex) {
                    unsigned int revCode = 0;
                    unsigned int l;
                    unsigned int curCode;
                    unsigned int codeSize = pTable->codeSize[symIndex];
                    
                    if (!codeSize) {
                        continue;
                    }

                    curCode = nextCode[codeSize]++;
                    
                    for (l = codeSize; l > 0; l--, curCode >>= 1) {
                        revCode = (revCode << 1) | (curCode & 1);
                    }

                    if (codeSize <= E_DEFLATE_FAST_LOOKUP_BITS) {
                        e_int16 k = (e_int16)((codeSize << 9) | symIndex);
                        
                        while (revCode < E_DEFLATE_FAST_LOOKUP_SIZE) {
                            pTable->lookup[revCode] = k;
                            revCode += (1 << codeSize);
                        }
                        
                        continue;
                    }

                    if (0 == (tree_cur = pTable->lookup[revCode & (E_DEFLATE_FAST_LOOKUP_SIZE - 1)])) {
                        pTable->lookup[revCode & (E_DEFLATE_FAST_LOOKUP_SIZE - 1)] = (e_int16)tree_next;
                        tree_cur = tree_next;
                        tree_next -= 2;
                    }

                    revCode >>= (E_DEFLATE_FAST_LOOKUP_BITS - 1);

                    for (j = codeSize; j > (E_DEFLATE_FAST_LOOKUP_BITS + 1); j--) {
                        tree_cur -= ((revCode >>= 1) & 1);

                        if (!pTable->tree[-tree_cur - 1]) {
                            pTable->tree[-tree_cur - 1] = (e_int16)tree_next; tree_cur = tree_next; tree_next -= 2;
                        } else {
                            tree_cur = pTable->tree[-tree_cur - 1];
                        }
                    }

                    tree_cur -= ((revCode >>= 1) & 1);
                    pTable->tree[-tree_cur - 1] = (e_int16)symIndex;
                }

                if (pDecompressor->type == 2) {
                    for (counter = 0; counter < (pDecompressor->tableSizes[0] + pDecompressor->tableSizes[1]); ) {
                        unsigned int s;
                        
                        E_DEFLATE_HUFF_DECODE(16, dist, &pDecompressor->tables[2]);
                        
                        if (dist < 16) {
                            pDecompressor->lenCodes[counter++] = (e_uint8)dist;
                            continue;
                        }

                        if ((dist == 16) && (!counter)) {
                            E_DEFLATE_CR_RETURN_FOREVER(17, E_ERROR);
                        }

                        extraCount = "\02\03\07"[dist - 16];
                        E_DEFLATE_GET_BITS(18, s, extraCount);
                        s += "\03\03\013"[dist - 16];
                        memset(pDecompressor->lenCodes + counter, (dist == 16) ? pDecompressor->lenCodes[counter - 1] : 0, s);
                        counter += s;
                    }

                    if ((pDecompressor->tableSizes[0] + pDecompressor->tableSizes[1]) != counter) {
                        E_DEFLATE_CR_RETURN_FOREVER(21, E_ERROR);
                    }

                    E_COPY_MEMORY(pDecompressor->tables[0].codeSize, pDecompressor->lenCodes, pDecompressor->tableSizes[0]); E_COPY_MEMORY(pDecompressor->tables[1].codeSize, pDecompressor->lenCodes + pDecompressor->tableSizes[0], pDecompressor->tableSizes[1]);
                }
            }
            for (;;) {
                e_uint8 *pSrc;
                for (;;) {
                    if (((pInputBufferEnd - pInputBufferCurrent) < 4) || ((pOutputBufferEnd - pOutputBufferCurrent) < 2)) {
                        E_DEFLATE_HUFF_DECODE(23, counter, &pDecompressor->tables[0]);
                        
                        if (counter >= 256) {
                          break;
                        }
                        
                        while (pOutputBufferCurrent >= pOutputBufferEnd) {
                            E_DEFLATE_CR_RETURN(24, E_HAS_MORE_OUTPUT);
                        }
                    
                        *pOutputBufferCurrent++ = (e_uint8)counter;
                    } else {
                        int sym2;
                        unsigned int codeLen;
#ifdef E_64BIT      
                        if (bitCount < 30) {
                            bitBuffer |= (((e_deflate_bitBufferfer)E_READ_LE32(pInputBufferCurrent)) << bitCount);
                            pInputBufferCurrent += 4;
                            bitCount += 32;
                        }
#else               
                        if (bitCount < 15) {
                            bitBuffer |= (((e_deflate_bitBufferfer)E_READ_LE16(pInputBufferCurrent)) << bitCount);
                            pInputBufferCurrent += 2;
                            bitCount += 16;
                        }
#endif              
                        if ((sym2 = pDecompressor->tables[0].lookup[bitBuffer & (E_DEFLATE_FAST_LOOKUP_SIZE - 1)]) >= 0) {
                            codeLen = sym2 >> 9;
                        } else {
                            codeLen = E_DEFLATE_FAST_LOOKUP_BITS;

                            do {
                                sym2 = pDecompressor->tables[0].tree[~sym2 + ((bitBuffer >> codeLen++) & 1)];
                            } while (sym2 < 0);
                        }
                    
                        counter = sym2;
                        bitBuffer >>= codeLen;
                        bitCount -= codeLen;
                    
                        if (counter & 256) {
                            break;
                        }
                    
#ifndef E_64BIT     
                        if (bitCount < 15) {
                            bitBuffer |= (((e_deflate_bitBufferfer)E_READ_LE16(pInputBufferCurrent)) << bitCount);
                            pInputBufferCurrent += 2;
                            bitCount += 16;
                        }
#endif              
                        if ((sym2 = pDecompressor->tables[0].lookup[bitBuffer & (E_DEFLATE_FAST_LOOKUP_SIZE - 1)]) >= 0) {
                            codeLen = sym2 >> 9;
                        } else {
                            codeLen = E_DEFLATE_FAST_LOOKUP_BITS;

                            do {
                                sym2 = pDecompressor->tables[0].tree[~sym2 + ((bitBuffer >> codeLen++) & 1)];
                            } while (sym2 < 0);
                        }
                    
                        bitBuffer >>= codeLen; bitCount -= codeLen;
                        
                        pOutputBufferCurrent[0] = (e_uint8)counter;
                        if (sym2 & 256) {
                            pOutputBufferCurrent++;
                            counter = sym2;
                            break;
                        }

                        pOutputBufferCurrent[1] = (e_uint8)sym2;
                        pOutputBufferCurrent += 2;
                    }
                }
                
                if ((counter &= 511) == 256) {
                    break;
                }
                
                extraCount = sLengthExtra[counter - 257];
                counter = sLengthBase[counter - 257];
                
                if (extraCount) {
                    unsigned int extraBits;
                    E_DEFLATE_GET_BITS(25, extraBits, extraCount);
                    counter += extraBits;
                }
                
                E_DEFLATE_HUFF_DECODE(26, dist, &pDecompressor->tables[1]);
                
                extraCount = sDistExtra[dist];
                dist = sDistBase[dist];
                
                if (extraCount) {
                    unsigned int extraBits;
                    E_DEFLATE_GET_BITS(27, extraBits, extraCount);
                    dist += extraBits;
                }
                
                distFromOutBufStart = pOutputBufferCurrent - pOutputBufferStart;
                if ((dist > distFromOutBufStart) && (flags & E_DEFLATE_FLAG_USING_NON_WRAPPING_OUTPUT_BUF)) {
                    E_DEFLATE_CR_RETURN_FOREVER(37, E_ERROR);
                }
                
                pSrc = pOutputBufferStart + ((distFromOutBufStart - dist) & outputBufferSizeMask);
                
                if ((E_MAX(pOutputBufferCurrent, pSrc) + counter) > pOutputBufferEnd) {
                    while (counter--) {
                        while (pOutputBufferCurrent >= pOutputBufferEnd) {
                            E_DEFLATE_CR_RETURN(53, E_HAS_MORE_OUTPUT);
                        }
                        
                        *pOutputBufferCurrent++ = pOutputBufferStart[(distFromOutBufStart++ - dist) & outputBufferSizeMask];
                    }
                
                    continue;
                }
                
                do {
                    pOutputBufferCurrent[0] = pSrc[0];
                    pOutputBufferCurrent[1] = pSrc[1];
                    pOutputBufferCurrent[2] = pSrc[2];
                    pOutputBufferCurrent += 3;
                    pSrc += 3;
                } while ((int)(counter -= 3) > 2);
                
                if ((int)counter > 0) {
                    pOutputBufferCurrent[0] = pSrc[0];
                
                    if ((int)counter > 1) {
                        pOutputBufferCurrent[1] = pSrc[1];
                    }
                
                    pOutputBufferCurrent += counter;
                }
            }
        }
    } while (!(pDecompressor->final & 1));

    if (flags & E_DEFLATE_FLAG_PARSE_ZLIB_HEADER) {
        E_DEFLATE_SKIP_BITS(32, bitCount & 7);
        
        for (counter = 0; counter < 4; ++counter) {
            unsigned int s;
            if (bitCount) {
                E_DEFLATE_GET_BITS(41, s, 8);
            } else {
                E_DEFLATE_GET_BYTE(42, s);
            }
            
            pDecompressor->zAdler32 = (pDecompressor->zAdler32 << 8) | s;
        }
    }

    E_DEFLATE_CR_RETURN_FOREVER(34, E_SUCCESS);
    E_DEFLATE_CR_FINISH

common_exit:
    pDecompressor->bitCount = bitCount;
    pDecompressor->bitBuffer = bitBuffer;
    pDecompressor->dist = dist;
    pDecompressor->counter = counter;
    pDecompressor->extraCount = extraCount;
    pDecompressor->distFromOutBufStart = distFromOutBufStart;

    *pInputBufferSize  = pInputBufferCurrent  - pInputBuffer;
    *pOutputBufferSize = pOutputBufferCurrent - pOutputBufferNext;

    if ((flags & (E_DEFLATE_FLAG_PARSE_ZLIB_HEADER | E_DEFLATE_FLAG_COMPUTE_ADLER32)) && (status >= 0)) {
        const e_uint8* ptr = pOutputBufferNext;
        size_t buf_len = *pOutputBufferSize;
        e_uint32 s1 = pDecompressor->checkAdler32 & 0xffff;
        e_uint32 s2 = pDecompressor->checkAdler32 >> 16;
        size_t block_len = buf_len % 5552;

        while (buf_len) {
            e_uint32 i;

            for (i = 0; i + 7 < block_len; i += 8, ptr += 8) {
                s1 += ptr[0], s2 += s1; s1 += ptr[1], s2 += s1; s1 += ptr[2], s2 += s1; s1 += ptr[3], s2 += s1;
                s1 += ptr[4], s2 += s1; s1 += ptr[5], s2 += s1; s1 += ptr[6], s2 += s1; s1 += ptr[7], s2 += s1;
            }

            for (; i < block_len; ++i) {
                s1 += *ptr++, s2 += s1;
            }

            s1 %= 65521U;
            s2 %= 65521U;
            buf_len -= block_len;
            block_len = 5552;
        }

        pDecompressor->checkAdler32 = (s2 << 16) + s1;

        if ((status == E_SUCCESS) && (flags & E_DEFLATE_FLAG_PARSE_ZLIB_HEADER) && (pDecompressor->checkAdler32 != pDecompressor->zAdler32)) {
            status = E_CHECKSUM_MISMATCH;
        }
    }

    return status;
}
/* ==== END e_deflate.c ==== */





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

static e_result e_fs_open_default(void* pUserData, e_fs* pFS, const char* pFilePath, int openMode, const e_allocation_callbacks* pAllocationCallbacks, e_file* pFile)
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



/* Unfortunately file iteration is platform-specific. */
#if defined(E_WIN32)
typedef struct
{
    e_fs_iterator iterator;
    HANDLE hFind;
} e_fs_iterator_default;

static void e_fs_free_iterator_default(void* pUserData, e_fs_iterator* pIterator, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_fs_iterator_default* pIteratorDefault = (e_fs_iterator_default*)pIterator;

    E_ASSERT(pIteratorDefault != NULL);
    E_UNUSED(pUserData);

    FindClose(pIteratorDefault->hFind);
    e_free(pIteratorDefault, pAllocationCallbacks);
}

static e_fs_iterator* e_fs_first_default(void* pUserData, e_fs* pFS, const char* pDirectoryPath, size_t directoryPathLen, const e_allocation_callbacks* pAllocationCallbacks)
{
    c89str_allocation_callbacks cstr89AllocationCallbacks = e_allocation_callbacks_to_c89str(pAllocationCallbacks);
    wchar_t* queryW = NULL;
    size_t queryWLen;
    c89str query = NULL;
    size_t queryLen;
    HANDLE hFind;
    WIN32_FIND_DATAW findData;
    size_t fileNameLenIncludingNullTerminator;
    e_fs_iterator_default* pIterator;

    E_ASSERT(pDirectoryPath != NULL);
    E_UNUSED(pUserData);

    /*
    A few notes here. We want to use FindFirstFileW() and not FindFirstFileA() for two reasons:

        1) We want to support Unicode paths for non-English languages
        2) We want to support paths longer than MAX_PATH

    To support longer file paths, we need to prepend "\\?\" to the path.

    For FindFirstFileW() to work, we need to ensure we normalize our slashes to backslashes. We also need to ensure
    we remove the trailing slash.

    In addition, FindFirstFileW() uses wildcards to determine what to search for. We need to append "\*" to the end
    of the query so that everything is returned.

    Our iteration system does not include the "." and ".." directories, so they'll need to be skipped as well.

    EDIT: It turns out that it appears that \\?\ can only be used for absolute paths. For now just not supporting
    long file paths.
    */

    /* First thing is to append the directory path we originally specified. */
    query = c89str_newn(&cstr89AllocationCallbacks, pDirectoryPath, directoryPathLen);
    if (query == NULL) {
        return NULL;
    }

    /* Next we need to normalize the slashes. */
    query = c89str_replace_all(query, &cstr89AllocationCallbacks, "/", 1, "\\", 1);    /* This will not fail. */

    /* Next we need to remove the trailing slash. */
    queryLen = c89str_len(query);  /* This will not fail. */
    if (queryLen > 0) {
        if (query[queryLen-1] == '/' || query[queryLen-1] == '\\') {
            query = c89str_remove(query, &cstr89AllocationCallbacks, queryLen-1, 1);    /* This will not fail. */
        }
    }

    /* Now we need to append the wildcard. */
    query = c89str_catn(query, &cstr89AllocationCallbacks, "\\*", 2);
    if (e_result_from_errno(c89str_result(query)) != E_SUCCESS) {
        c89str_delete(query, &cstr89AllocationCallbacks);
        return NULL;
    }

    /*
    Disabling long file paths for now. See note above. Should probably check if the path is absolute, and if
    so prepend "\\?\".
    */
#if 0
    /* Now we need to prepend the "\\?\" to the path. */
    query = c89str_prependn(query, &cstr89AllocationCallbacks, "\\\\?\\", 4);
    if (e_result_from_errno(c89str_result(query)) != E_SUCCESS) {
        c89str_delete(query, &cstr89AllocationCallbacks);
        return NULL;
    }
#endif

    /*
    Before we can call FindFirstFileW() we need to convert the string to wide characters using
    MultiByteToWideChar(). The first step is to calculate the length.
    */
    queryWLen = MultiByteToWideChar(CP_UTF8, 0, query, -1, NULL, 0);    /* -1 because our string is null terminated. */
    if (queryWLen == 0) {
        c89str_delete(query, &cstr89AllocationCallbacks);
        return NULL;
    }

    queryW = (wchar_t*)e_malloc(sizeof(*queryW) * (queryWLen+1), pAllocationCallbacks); /* +1 for the null terminator. */
    if (queryW == NULL) {
        c89str_delete(query, &cstr89AllocationCallbacks);
        return NULL;
    }

    queryWLen = MultiByteToWideChar(CP_UTF8, 0, query, -1, queryW, (int)queryWLen); /* -1 because our string is null terminated. */
    if (queryWLen == 0) {
        e_free(queryW, pAllocationCallbacks);
        return NULL;
    }

    /* We're done with the UT8-8 query. From here on out queryW will be used. */
    c89str_delete(query, &cstr89AllocationCallbacks);
    query = NULL;

    /*
    We can now call into FindFirstFileW(). We can't allocate the iterator until we know the name of
    the first file because we'll be allocating the memory for the name at the end of the struct.
    */
    hFind = FindFirstFileW(queryW, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        e_free(queryW, pAllocationCallbacks);
        return NULL;
    }

    /* We're done with the wide character query. */
    e_free(queryW, pAllocationCallbacks);
    queryW = NULL;

    /* Before we can allocate the iterator we need to convert the file name to UTF-8. */
    fileNameLenIncludingNullTerminator = WideCharToMultiByte(CP_UTF8, 0, findData.cFileName, -1, NULL, 0, NULL, NULL); /* -1 because our string is null terminated. */
    if (fileNameLenIncludingNullTerminator == 0) {
        FindClose(hFind);
        return NULL;
    }

    /* Now that we have the length of the file name, we can allocate the iterator. */
    pIterator = (e_fs_iterator_default*)e_malloc(sizeof(*pIterator) + fileNameLenIncludingNullTerminator, pAllocationCallbacks);
    if (pIterator == NULL) {
        FindClose(hFind);
        return NULL;
    }

    pIterator->iterator.pFS = pFS;

    /* The name will be stored at the end of the struct. */
    pIterator->iterator.pName = (char*)pIterator + sizeof(*pIterator);
    pIterator->iterator.nameLen = fileNameLenIncludingNullTerminator - 1; /* -1 because we don't want to include the null terminator. */

    /* We can now convert the file name to UTF-8. */
    fileNameLenIncludingNullTerminator = WideCharToMultiByte(CP_UTF8, 0, findData.cFileName, -1, (char*)pIterator->iterator.pName, (int)fileNameLenIncludingNullTerminator, NULL, NULL); /* -1 because our string is null terminated. */
    if (fileNameLenIncludingNullTerminator == 0) {
        e_free(pIterator, pAllocationCallbacks);
        FindClose(hFind);
        return NULL;
    }

    /* The file info will be located in fileData. We'll need to copy over the relevant details. */
    E_ZERO_OBJECT(&pIterator->iterator.info);
    pIterator->iterator.info.directory        = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    pIterator->iterator.info.size             = ((e_uint64)findData.nFileSizeHigh << 32) | findData.nFileSizeLow;
    pIterator->iterator.info.lastModifiedTime = ((e_uint64)findData.ftLastWriteTime.dwHighDateTime  << 32) | findData.ftLastWriteTime.dwLowDateTime;
    pIterator->iterator.info.lastAccessTime   = ((e_uint64)findData.ftLastAccessTime.dwHighDateTime << 32) | findData.ftLastAccessTime.dwLowDateTime;

    /* Now just write out hFind item to the iterator so we have a hold of it for later and we're finally done. */
    pIterator->hFind = hFind;

    return (e_fs_iterator*)pIterator;
}

static e_fs_iterator* e_fs_next_default(void* pUserData, e_fs_iterator* pIterator, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_fs_iterator_default* pIteratorDefault = (e_fs_iterator_default*)pIterator;
    WIN32_FIND_DATAW findData;

    E_ASSERT(pIteratorDefault != NULL);
    E_UNUSED(pUserData);

    /* We need to call FindNextFileW() to get the next file. */
    if (FindNextFileW(pIteratorDefault->hFind, &findData)) {
        e_fs_iterator_default* pNewIteratorDefault;

        /* We need to convert the file name to UTF-8. We'll first need to grab the length. */
        int fileNameLenIncludingNullTerminator = WideCharToMultiByte(CP_UTF8, 0, findData.cFileName, -1, NULL, 0, NULL, NULL); /* -1 because our string is null terminated. */
        if (fileNameLenIncludingNullTerminator == 0) {
            return NULL;
        }

        /* We need to reallocate the iterator to accomodate the length of the new file name. */
        pNewIteratorDefault = (e_fs_iterator_default*)e_realloc(pIterator, sizeof(*pNewIteratorDefault) + fileNameLenIncludingNullTerminator, pAllocationCallbacks);
        if (pNewIteratorDefault == NULL) {
            return NULL;
        }

        pNewIteratorDefault->iterator.pName = (char*)pNewIteratorDefault + sizeof(*pNewIteratorDefault);
        pNewIteratorDefault->iterator.nameLen = fileNameLenIncludingNullTerminator - 1; /* -1 because we don't want to include the null terminator. */

        /* We can now convert the file name to UTF-8. */
        fileNameLenIncludingNullTerminator = WideCharToMultiByte(CP_UTF8, 0, findData.cFileName, -1, (char*)pNewIteratorDefault->iterator.pName, (int)fileNameLenIncludingNullTerminator, NULL, NULL); /* -1 because our string is null terminated. */
        if (fileNameLenIncludingNullTerminator == 0) {
            return NULL;
        }

        /* The file info will be located in fileData. We'll need to copy over the relevant details. */
        E_ZERO_OBJECT(&pNewIteratorDefault->iterator.info);
        pNewIteratorDefault->iterator.info.directory        = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        pNewIteratorDefault->iterator.info.size             = ((e_uint64)findData.nFileSizeHigh << 32) | findData.nFileSizeLow;
        pNewIteratorDefault->iterator.info.lastModifiedTime = ((e_uint64)findData.ftLastWriteTime.dwHighDateTime  << 32) | findData.ftLastWriteTime.dwLowDateTime;
        pNewIteratorDefault->iterator.info.lastAccessTime   = ((e_uint64)findData.ftLastAccessTime.dwHighDateTime << 32) | findData.ftLastAccessTime.dwLowDateTime;

        return (e_fs_iterator*)pNewIteratorDefault;
    }

    /* Getting here means the iterator is done. We can uninitialize it and return null. */
    e_fs_free_iterator_default(pUserData, pIterator, pAllocationCallbacks);

    return NULL;
}
#endif

#if defined(E_POSIX)
#include <dirent.h>

typedef struct
{
    e_fs_iterator iterator;
    DIR* dir;
    char* pFullFilePath;        /* Points to the end of the structure. */
    size_t directoryPathLen;    /* The length of the directory section. */
} e_fs_iterator_default;

static void e_fs_free_iterator_default(void* pUserData, e_fs_iterator* pIterator, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_fs_iterator_default* pIteratorDefault = (e_fs_iterator_default*)pIterator;

    E_ASSERT(pIteratorDefault != NULL);
    E_UNUSED(pUserData);

    closedir(pIteratorDefault->dir);
    e_free(pIteratorDefault, pAllocationCallbacks);
}

static e_fs_iterator* e_fs_first_default(void* pUserData, e_fs* pFS, const char* pDirectoryPath, size_t directoryPathLen, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_fs_iterator_default* pIteratorDefault;
    struct dirent* info;
    struct stat statInfo;
    size_t fileNameLen;

    E_ASSERT(pDirectoryPath != NULL);
    E_UNUSED(pUserData);
    E_UNUSED(pFS);

    /*
    Our input string isn't necessarily null terminated so we'll need to make a copy. This isn't
    the end of the world because we need to keep a copy of it anyway for when we need to stat
    the file for information like it's size.

    To do this we're going to allocate memory for our iterator which will include space for the
    directory path. Then we copy the directory path into the allocated memory and point the
    pFullFilePath member of the iterator to it. Then we call opendir(). Once that's done we
    can go to the first file and reallocate the iterator to make room for the file name portion,
    including the separating slash. Then we copy the file name portion over to the buffer.
    */

    /* The first step is to calculate the length of the path if we need to. */
    if (directoryPathLen == (size_t)-1) {
        directoryPathLen = c89str_strlen(pDirectoryPath);
    }

    /*
    Now that we know the length of the directory we can allocate space for the iterator. The
    directory path will be placed at the end of the structure.
    */
    pIteratorDefault = (e_fs_iterator_default*)e_malloc(sizeof(*pIteratorDefault) + directoryPathLen + 1, pAllocationCallbacks);    /* +1 for null terminator. */
    if (pIteratorDefault == NULL) {
        return NULL;
    }

    /* Point pFullFilePath to the end of structure to where the path is located. */
    pIteratorDefault->pFullFilePath = (char*)pIteratorDefault + sizeof(*pIteratorDefault);
    pIteratorDefault->directoryPathLen = directoryPathLen;

    /* We can now copy over the directory path. This will null terminate the path which will allow us to call opendir(). */
    c89str_strncpy_s(pIteratorDefault->pFullFilePath, directoryPathLen + 1, pDirectoryPath, directoryPathLen);

    /* We can now open the directory. */
    pIteratorDefault->dir = opendir(pIteratorDefault->pFullFilePath);
    if (pIteratorDefault->dir == NULL) {
        e_free(pIteratorDefault, pAllocationCallbacks);
        return NULL;
    }


    /* We now need to get information about the first file. */
    info = readdir(pIteratorDefault->dir);
    if (info == NULL) {
        e_fs_free_iterator_default(pUserData, (e_fs_iterator*)pIteratorDefault, pAllocationCallbacks);
        return NULL;
    }

    fileNameLen = c89str_strlen(info->d_name);

    /*
    Now that we have the file name we need to append it to the full file path in the iterator. To do
    this we need to reallocate the iterator to account for the length of the file name, including the
    separating slash.
    */
    {
        e_fs_iterator_default* pNewIteratorDefault = (e_fs_iterator_default*)e_realloc(pIteratorDefault, sizeof(*pIteratorDefault) + directoryPathLen + 1 + fileNameLen + 1, pAllocationCallbacks);    /* +1 for null terminator. */
        if (pNewIteratorDefault == NULL) {
            e_fs_free_iterator_default(pUserData, (e_fs_iterator*)pIteratorDefault, pAllocationCallbacks);
            return NULL;
        }

        pIteratorDefault = pNewIteratorDefault;
    }

    /* Memory has been allocated. Copy over the separating slash and file name. */
    pIteratorDefault->pFullFilePath = (char*)pIteratorDefault + sizeof(*pIteratorDefault);
    pIteratorDefault->pFullFilePath[directoryPathLen] = '/';
    c89str_strcpy(pIteratorDefault->pFullFilePath + directoryPathLen + 1, info->d_name);

    /* The pFileName member of the base iterator needs to be set to the file name. */
    pIteratorDefault->iterator.pName   = pIteratorDefault->pFullFilePath + directoryPathLen + 1;
    pIteratorDefault->iterator.nameLen = fileNameLen;

    /* We can now get the file information. */
    if (stat(pIteratorDefault->pFullFilePath, &statInfo) != 0) {
        e_fs_free_iterator_default(pUserData, (e_fs_iterator*)pIteratorDefault, pAllocationCallbacks);
        return NULL;
    }

    E_ZERO_OBJECT(&pIteratorDefault->iterator.info);
    pIteratorDefault->iterator.info.size             = statInfo.st_size;
    pIteratorDefault->iterator.info.lastModifiedTime = statInfo.st_mtime;
    pIteratorDefault->iterator.info.lastAccessTime   = statInfo.st_atime;
    pIteratorDefault->iterator.info.directory        = S_ISDIR(statInfo.st_mode) != 0;

    return (e_fs_iterator*)pIteratorDefault;
}

static e_fs_iterator* e_fs_next_default(void* pUserData, e_fs_iterator* pIterator, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_fs_iterator_default* pIteratorDefault = (e_fs_iterator_default*)pIterator;
    struct dirent* info;
    struct stat statInfo;
    size_t fileNameLen;

    E_ASSERT(pIteratorDefault != NULL);
    E_UNUSED(pUserData);

    /* We need to get information about the next file. */
    info = readdir(pIteratorDefault->dir);
    if (info == NULL) {
        e_fs_free_iterator_default(pUserData, (e_fs_iterator*)pIteratorDefault, pAllocationCallbacks);
        return NULL;    /* The end of the directory. */
    }

    fileNameLen = c89str_strlen(info->d_name);

    /* We need to reallocate the iterator to account for the new file name. */
    {
        e_fs_iterator_default* pNewIteratorDefault = (e_fs_iterator_default*)e_realloc(pIteratorDefault, sizeof(*pIteratorDefault) + pIteratorDefault->directoryPathLen + 1 + fileNameLen + 1, pAllocationCallbacks);    /* +1 for null terminator. */
        if (pNewIteratorDefault == NULL) {
            e_fs_free_iterator_default(pUserData, (e_fs_iterator*)pIteratorDefault, pAllocationCallbacks);
            return NULL;
        }

        pIteratorDefault = pNewIteratorDefault;
    }

    /* Memory has been allocated. Copy over the file name. */
    pIteratorDefault->pFullFilePath = (char*)pIteratorDefault + sizeof(*pIteratorDefault);
    c89str_strcpy(pIteratorDefault->pFullFilePath + pIteratorDefault->directoryPathLen + 1, info->d_name);

    /* The pFileName member of the base iterator needs to be set to the file name. */
    pIteratorDefault->iterator.pName   = pIteratorDefault->pFullFilePath + pIteratorDefault->directoryPathLen + 1;
    pIteratorDefault->iterator.nameLen = fileNameLen;

    /* We can now get the file information. */
    if (stat(pIteratorDefault->pFullFilePath, &statInfo) != 0) {
        e_fs_free_iterator_default(pUserData, (e_fs_iterator*)pIteratorDefault, pAllocationCallbacks);
        return NULL;
    }

    E_ZERO_OBJECT(&pIteratorDefault->iterator.info);
    pIteratorDefault->iterator.info.size             = statInfo.st_size;
    pIteratorDefault->iterator.info.lastModifiedTime = statInfo.st_mtime;
    pIteratorDefault->iterator.info.lastAccessTime   = statInfo.st_atime;
    pIteratorDefault->iterator.info.directory        = S_ISDIR(statInfo.st_mode) != 0;

    return (e_fs_iterator*)pIteratorDefault;
}
#endif


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
    e_fs_info_default,
    e_fs_first_default,
    e_fs_next_default,
    e_fs_free_iterator_default
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

    /* Opened archives need to be closed. */
    while (pFS->openedArchiveCount > 0) {
        pFS->openedArchiveCount -= 1;
        e_archive_uninit(pFS->pOpenedArchives[pFS->openedArchiveCount].pArchive, pAllocationCallbacks);
        e_free(pFS->pOpenedArchives[pFS->openedArchiveCount].pFilePath, pAllocationCallbacks);
    }
    e_free(pFS->pOpenedArchives, pAllocationCallbacks);

    /* Archive extensions need to be freed. */
    e_free(pFS->pArchiveExtensions, pAllocationCallbacks);

    if (pFS->freeOnUninit) {
        e_free(pFS, pAllocationCallbacks);
    }
}

static e_result e_fs_open_archive(e_fs* pFS, const char* pArchiveFilePath, size_t archiveFilePathLen, const e_allocation_callbacks* pAllocationCallbacks, e_archive** ppArchive)
{
    /* If the archive is already open, just return the existing archive. */
    e_result result;
    size_t iArchive;
    size_t iArchiveExtension;
    /* */ char* pArchiveFilePathCopy;
    const char* pArchiveFilePathExtension;
    size_t archiveFilePathExtensionLen;
    e_archive_extension* pArchiveExtension = NULL;
    e_archive* pArchive;

    for (iArchive = 0; iArchive < pFS->openedArchiveCount; iArchive += 1) {
        if (c89str_strncmp(pFS->pOpenedArchives[iArchive].pFilePath, pArchiveFilePath, archiveFilePathLen) == 0) {
            *ppArchive = pFS->pOpenedArchives[iArchive].pArchive;
            return E_SUCCESS;
        }
    }

    /*
    Getting here means the archive isn't already open. We need to open it and add it to our
    internal list. In order to open the archive we need to know what vtable to use. To determine
    the vtable we need to inspect the extension.
    */
    if (archiveFilePathLen == (size_t)-1) {
        archiveFilePathLen = c89str_strlen(pArchiveFilePath);
    }

    pArchiveFilePathExtension = c89str_path_extension(pArchiveFilePath, archiveFilePathLen);
    if (pArchiveFilePathExtension == NULL) {
        return E_INVALID_ARGS;  /* No extension. */
    }

    archiveFilePathExtensionLen = archiveFilePathLen - (pArchiveFilePathExtension - pArchiveFilePath);

    for (iArchiveExtension = 0; iArchiveExtension < pFS->archiveExtensionCount; iArchiveExtension += 1) {
        size_t extensionLen = c89str_strlen(pFS->pArchiveExtensions[iArchiveExtension].pExtension);
        if (extensionLen != archiveFilePathExtensionLen) {
            continue;   /* Extension lengths are different. Cannot be this one. */
        }

        if (c89str_strnicmp(pArchiveFilePathExtension, pFS->pArchiveExtensions[iArchiveExtension].pExtension, extensionLen) != 0) {
            continue;   /* Extensions don't match. */
        }

        /* We have a match. */
        pArchiveExtension = &pFS->pArchiveExtensions[iArchiveExtension];
        break;
    }

    if (pArchiveExtension == NULL) {
        return E_DOES_NOT_EXIST;    /* Couldn't find a matching extension for this archive. Don't think we should ever hit this in practice because it should be checked at a higher level. */
    }

    /* At this point we should have the vtable so we can now try opening it. The path needs to be null terminated. */
    pArchiveFilePathCopy = (char*)e_malloc(archiveFilePathLen + 1, pAllocationCallbacks);
    if (pArchiveFilePathCopy == NULL) {
        return E_OUT_OF_MEMORY;
    }

    c89str_strncpy_s(pArchiveFilePathCopy, archiveFilePathLen + 1, pArchiveFilePath, archiveFilePathLen);

    result = e_archive_init_from_file(pArchiveExtension->pArchiveVTable, pArchiveExtension->pArchiveVTableUserData, pFS, pArchiveFilePathCopy, pAllocationCallbacks, &pArchive);
    if (result != E_SUCCESS) {
        e_free(pArchiveFilePathCopy, pAllocationCallbacks);
        return result;
    }

    /* The new archive should inherit any archive extensions so it can work recursively. */
    for (iArchiveExtension = 0; iArchiveExtension < pFS->archiveExtensionCount; iArchiveExtension += 1) {
        result = e_fs_register_archive_extension((e_fs*)pArchive, pFS->pArchiveExtensions[iArchiveExtension].pArchiveVTable, pFS->pArchiveExtensions[iArchiveExtension].pArchiveVTableUserData, pFS->pArchiveExtensions[iArchiveExtension].pExtension, pAllocationCallbacks);
        if (result != E_SUCCESS) {
            e_archive_uninit(pArchive, pAllocationCallbacks);
            e_free(pArchiveFilePathCopy, pAllocationCallbacks);
            return result;
        }
    }

    /* We need to add the archive to our internal list. */
    if (pFS->openedArchiveCount == pFS->openedArchiveCap) {
        e_fs_opened_archive* pNewOpenedArchives;
        size_t newCapacity = pFS->openedArchiveCap * 2;
        if (newCapacity == 0) {
            newCapacity = 1;
        }
    
        pNewOpenedArchives = (e_fs_opened_archive*)e_realloc(pFS->pOpenedArchives, sizeof(*pNewOpenedArchives) * newCapacity, pAllocationCallbacks);
        if (pNewOpenedArchives == NULL) {
            e_archive_uninit(pArchive, pAllocationCallbacks);
            e_free(pArchiveFilePathCopy, pAllocationCallbacks);
            return E_OUT_OF_MEMORY;
        }
    
        pFS->pOpenedArchives = pNewOpenedArchives;
        pFS->openedArchiveCap = newCapacity;
    }

    pFS->pOpenedArchives[pFS->openedArchiveCount].pArchive = pArchive;
    pFS->pOpenedArchives[pFS->openedArchiveCount].pFilePath = pArchiveFilePathCopy;
    pFS->openedArchiveCount += 1;

    *ppArchive = pArchive;
    return E_SUCCESS;
}

static e_result e_fs_open_from_archive(e_fs* pFS, const char* pFilePath, int openMode, const e_allocation_callbacks* pAllocationCallbacks, e_file** ppFile)
{
    /*
    This is the main function we use for opening from an archive. We need to iterate over each
    segment in the file path and then iterate over the files within that directory. For each
    file in the directory that looks like an archive, we need to open that archive and try opening
    the file from there.
    */
    e_result result;
    c89str_path_iterator iFilePathSegment;

    /* Start iterating over each segment in the file path. */
    result = e_result_from_errno(c89str_path_first(pFilePath, (size_t)-1, &iFilePathSegment));
    if (result != E_SUCCESS) {
        return result;
    }

    do
    {
        /*
        If this segment of the path points to an explicit archive then we *must* try loading from
        the archive at all times. We cannot be attempting to search all other archives or else
        we'll risk opening the wrong file.
        */
        if (e_fs_is_path_archive(pFS, iFilePathSegment.pFullPath + iFilePathSegment.segmentOffset, iFilePathSegment.segmentLength)) {
            /* It's an explicit archive. We must try loading from this path. */
            e_archive* pArchive;
            result = e_fs_open_archive(pFS, iFilePathSegment.pFullPath, iFilePathSegment.segmentOffset + iFilePathSegment.segmentLength, pAllocationCallbacks, &pArchive);
            if (result != E_SUCCESS) {
                return result;  /* Failed to open the archive. We cannot continue since this was explicitly asked for. */
            }

            /* We have the archive. We need to try opening the file from it. */
            result = e_fs_open((e_fs*)pArchive, iFilePathSegment.pFullPath + iFilePathSegment.segmentOffset + iFilePathSegment.segmentLength + 1, openMode, pAllocationCallbacks, ppFile);
            if (result != E_SUCCESS) {
                return result;  /* Failed to open the file. */
            }

            return E_SUCCESS;
        } else {
            /* This part of the segment is not asking for an explicit archive. We need to iterate over all the archives in this directory and try loading from each one. */
            e_fs_iterator* pFileIterator;
            for (pFileIterator = e_fs_first(pFS, iFilePathSegment.pFullPath, iFilePathSegment.segmentOffset + iFilePathSegment.segmentLength, pAllocationCallbacks); pFileIterator != NULL; pFileIterator = e_fs_next(pFileIterator, pAllocationCallbacks)) {
                if (e_fs_is_path_archive(pFS, pFileIterator->pName, pFileIterator->nameLen)) {
                    /* It's an archive. Try loading from it. */
                    e_file* pFile;
                    e_archive* pArchive;

                    /* To load from the archive we actually need to construct a string. */
                    char  pArchiveFilePathStack[1024];
                    char* pArchiveFilePathHeap = NULL;
                    char* pArchiveFilePath;

                    /* Try using the stack buffer first. */
                    if (c89str_snprintf(pArchiveFilePathStack, sizeof(pArchiveFilePathStack), "%.*s/%.*s", (int)(iFilePathSegment.segmentLength + iFilePathSegment.segmentOffset), iFilePathSegment.pFullPath, (int)pFileIterator->nameLen, pFileIterator->pName) < (int)sizeof(pArchiveFilePathStack)) {
                        /* It does not fit in the stack buffer. Fall back to a heap allocation. */
                        pArchiveFilePath = pArchiveFilePathStack;
                    } else {
                        pArchiveFilePathHeap = (char*)e_malloc(iFilePathSegment.segmentOffset + iFilePathSegment.segmentLength + 1 + pFileIterator->nameLen + 1, pAllocationCallbacks);
                        if (pArchiveFilePathHeap == NULL) {
                            return E_OUT_OF_MEMORY;
                        }

                        c89str_sprintf(pArchiveFilePathHeap, "%.*s/%.*s", (int)(iFilePathSegment.segmentLength + iFilePathSegment.segmentOffset), iFilePathSegment.pFullPath, (int)pFileIterator->nameLen, pFileIterator->pName);
                        pArchiveFilePath = pArchiveFilePathHeap;
                    }

                    result = e_fs_open_archive(pFS, pArchiveFilePath, (size_t)-1, pAllocationCallbacks, &pArchive);
                    e_free(pArchiveFilePathHeap, pAllocationCallbacks);

                    if (result == E_SUCCESS) {
                        /* We have the archive. We need to try opening the file from it. */
                        result = e_fs_open((e_fs*)pArchive, iFilePathSegment.pFullPath + iFilePathSegment.segmentOffset + iFilePathSegment.segmentLength + 1, openMode, pAllocationCallbacks, &pFile);
                        if (result == E_SUCCESS) {
                            /* The file was opened successfully. We're done. */
                            *ppFile = pFile;
                            return E_SUCCESS;
                        } else {
                            /* Getting here means the file could not be found in this archive. Keep searching. */
                        }
                    } else {
                        /* Getting here means we couldn't open the archive. */
                    }
                } else {
                    /* It's not an archive. Keep searching. */
                }
            }
        }
    } while (e_result_from_errno(c89str_path_next(&iFilePathSegment)) == E_SUCCESS);

    /* Getting here means we could not find the file. */
    return E_DOES_NOT_EXIST;
}

E_API e_result e_fs_open(e_fs* pFS, const char* pFilePath, int openMode, const e_allocation_callbacks* pAllocationCallbacks, e_file** ppFile)
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

        /*
        If we failed to open the file because it doesn't exist we need to try loading it from an
        archive, but only if we're not trying to open the file in write mode. We're currently only
        supporting read mode with archives.
        */
        if (result == E_DOES_NOT_EXIST && (openMode & E_OPEN_MODE_WRITE) == 0) {
            result = e_fs_open_from_archive(pFS, pFilePath, openMode, pAllocationCallbacks, ppFile);
        }

        return result;
    }

    /* Getting here means we were able to open the file directly from this FS (not from an archive - that will have handled earlier in a separate path). */

    /* We need to make sure the file is given the vtable that was used to initialize it. This is because pFS is allowed to be null. */
    pFile->pFS = pFS;
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

E_API e_fs_iterator* e_fs_first(e_fs* pFS, const char* pDirectoryPath, size_t directoryPathLen, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_fs_iterator* pIterator;
    const e_fs_vtable* pVTable = &e_gDefaultFSVTable;
    void* pVTableUserData = NULL;

    if (pFS != NULL) {
        pVTable         = pFS->pVTable;
        pVTableUserData = pFS->pVTableUserData;
    }

    if (pDirectoryPath == NULL) {
        pDirectoryPath = "";
    }

    if (pVTable->first_file == NULL) {
        return NULL;
    }

    pIterator = pVTable->first_file(pVTableUserData, pFS, pDirectoryPath, directoryPathLen, pAllocationCallbacks);

    /* Just make double sure the FS information is set in case the backend doesn't do it. */
    if (pIterator != NULL) {
        pIterator->pFS = pFS;
        pIterator->pFSVTable = pVTable;
        pIterator->pFSVTableUserData = pVTableUserData;
    }

    /* We want to skip over any "." and ".." directories. */
    while (pIterator != NULL && (c89str_strcmp(pIterator->pName, ".") == 0 || c89str_strcmp(pIterator->pName, "..") == 0)) {
        pIterator = e_fs_next(pIterator, pAllocationCallbacks);
    }

    return pIterator;
}

E_API e_fs_iterator* e_fs_next(e_fs_iterator* pIterator, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pIterator == NULL) {
        return NULL;
    }

    if (pIterator->pFSVTable->next_file == NULL) {
        return NULL;
    }

    /* We don't want to include any "." and ".." directories. */
    do
    {
        pIterator = pIterator->pFSVTable->next_file(pIterator->pFSVTableUserData, pIterator, pAllocationCallbacks);
    } while (pIterator != NULL && (c89str_strcmp(pIterator->pName, ".") == 0 || c89str_strcmp(pIterator->pName, "..") == 0));

    return pIterator;
}

E_API void e_fs_free_iterator(e_fs_iterator* pIterator, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pIterator == NULL) {
        return;
    }

    if (pIterator->pFSVTable->free_iterator == NULL) {
        return;
    }

    pIterator->pFSVTable->free_iterator(pIterator->pFSVTableUserData, pIterator, pAllocationCallbacks);
}

E_API e_result e_fs_register_archive_extension(e_fs* pFS, e_archive_vtable* pArchiveVTable, void* pArchiveVTableUserData, const char* pExtension, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_archive_extension* pNewExtension;
    size_t extensionLen;

    if (pFS == NULL || pArchiveVTable == NULL || pExtension == NULL || pExtension[0] == '\0') {
        return E_INVALID_ARGS;
    }

    /* The length of the extension cannot exceed the buffer we'll be storing it in. */
    extensionLen = c89str_strlen(pExtension);
    if (extensionLen+1 > sizeof(pNewExtension->pExtension)) {   /* +1 to account for the null terminator. */
        return E_INVALID_ARGS;
    }

    /* We need to append the extension to the end of the list. */
    pNewExtension = (e_archive_extension*)e_realloc(pFS->pArchiveExtensions, sizeof(*pFS->pArchiveExtensions) * (pFS->archiveExtensionCount + 1), pAllocationCallbacks);
    if (pNewExtension == NULL) {
        return E_OUT_OF_MEMORY;
    }

    pFS->pArchiveExtensions = pNewExtension;

    /* We can now initialize the new extension. */
    pNewExtension[pFS->archiveExtensionCount].pArchiveVTable = pArchiveVTable;
    pNewExtension[pFS->archiveExtensionCount].pArchiveVTableUserData = pArchiveVTableUserData;
    c89str_strncpy_s(pNewExtension[pFS->archiveExtensionCount].pExtension, sizeof(pNewExtension[pFS->archiveExtensionCount].pExtension), pExtension, extensionLen);

    /* We're done so we can commit the new extension by incrementing the counter. */
    pFS->archiveExtensionCount += 1;

    return E_SUCCESS;
}

E_API e_bool32 e_fs_is_path_archive(e_fs* pFS, const char* pFilePath, size_t filePathLen)
{
    size_t iExtension;

    if (pFS == NULL || pFilePath == NULL) {
        return E_FALSE;
    }

    if (filePathLen == 0 || filePathLen == (size_t)-1) {
        filePathLen = c89str_strlen(pFilePath);
    }

    /* We need to loop through each extension and check if the file path ends with it. */
    for (iExtension = 0; iExtension < pFS->archiveExtensionCount; ++iExtension) {
        size_t extensionLen = c89str_strlen(pFS->pArchiveExtensions[iExtension].pExtension);

        /* The extension must be shorter than the file path. */
        if (extensionLen > filePathLen) {
            continue;
        }

        /* We need to check if the file path ends with the extension. We're going case-sensitive here for the momemnt. I'm not sure yet what the correct approach would be. */
        if (c89str_strnicmp(pFilePath + (filePathLen - extensionLen), pFS->pArchiveExtensions[iExtension].pExtension, extensionLen) == 0) {
            return E_TRUE;
        }
    }

    return E_FALSE;
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

E_API e_result e_fs_open_and_write(e_fs* pFS, const char* pFilePath, const void* pData, size_t dataSize, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_result result;
    e_file* pFile;
    
    result = e_fs_open(pFS, pFilePath, E_OPEN_MODE_WRITE | E_OPEN_MODE_TRUNCATE, pAllocationCallbacks, &pFile);
    if (result != E_SUCCESS) {
        return result;
    }

    result = e_fs_write(pFile, pData, dataSize, NULL);
    e_fs_close(pFile, pAllocationCallbacks);

    if (result != E_SUCCESS) {    
        return result;
    }

    return E_SUCCESS;
}


E_API e_result e_fs_gather_files_in_directory(e_fs* pFS, const char* pDirectoryPath, size_t directoryPathLen, const e_allocation_callbacks* pAllocationCallbacks, char*** pppFileNames, size_t** ppFileNameLengths, e_file_info** ppFileInfos, size_t* pFileCount)
{
    e_fs_iterator* pFileIterator;
    char** ppFileNames;
    size_t* pFileNameLengths;
    e_file_info* pFileInfos;
    size_t fileCount;
    char* pData = NULL;     /* We do everything with a single allocation (resized with realloc()). This is a pointer to that allocation. */
    size_t dataSize = 0;    /* The total size of the data buffer. */
    size_t dataCap = 0;     /* The total allocation size of the data buffer. */
    size_t totalFileNameLength = 0; /* The total length of all file names in the buffer. */
    size_t iFile;
    char* pRunningFileName;

    if (pppFileNames == NULL || pFileCount == NULL) {
        return E_INVALID_ARGS;
    }

    *pppFileNames = NULL;
    *pFileCount = 0;

    if (pFS == NULL) {
        return E_INVALID_ARGS;
    }

    if (pDirectoryPath == NULL) {
        pDirectoryPath = "";
    }

    /* We need to loop over each file in the directory and add it to the list. */
    ppFileNames      = NULL;
    pFileNameLengths = NULL;
    pFileInfos       = NULL;
    fileCount        = 0;

    /*
    We allocate a single buffer to store all of the info. It's structured such that the file paths are at the front, then an array of
    char* pointers, then an array of size_t lengths, and then an array of e_file_info structures.

    The buffer is structure like the following:

        [char* pointers] [size_t lengths] [e_file_info] [file names]
    */

    for (pFileIterator = e_fs_first(pFS, pDirectoryPath, directoryPathLen, pAllocationCallbacks); pFileIterator != NULL; pFileIterator = e_fs_next(pFileIterator, pAllocationCallbacks)) {
        const char* pName;
        size_t nameLen;
        size_t nameAndInfoDataSize;

        /* We need to skip over "." and ".." directories. */
        if (c89str_strcmp(pFileIterator->pName, ".") == 0 || c89str_strcmp(pFileIterator->pName, "..") == 0) {
            continue;
        }

        pName   = pFileIterator->pName;
        nameLen = pFileIterator->nameLen;
        
        /* Resize the buffer if necessary. We need room for the name, it's null terminator, it's length, and the file info. */
        nameAndInfoDataSize = sizeof(char*) + nameLen + 1 + sizeof(size_t) + sizeof(e_file_info);
        if (pData == NULL || dataSize + nameAndInfoDataSize > dataCap) {
            size_t newDataCap;
            char* pNewData;

            newDataCap = dataCap * 2;
            if (newDataCap < dataSize + nameAndInfoDataSize) {
                newDataCap = dataSize + nameAndInfoDataSize;
            }

            pNewData = (char*)e_realloc(pData, newDataCap, pAllocationCallbacks);
            if (pNewData == NULL) {
                e_free(pData, pAllocationCallbacks);
                return E_OUT_OF_MEMORY;
            }

            pData = pNewData;
            dataCap = newDataCap;
        }

        /*
        The actual file name content is stored at the end of the buffer. The first thing to do is move that down to the end so we
        don't overwrite anything.
        */
        E_MOVE_MEMORY(
            pData + ((fileCount+1) * (sizeof(char*) + sizeof(size_t) + sizeof(e_file_info))),
            pData + ((fileCount  ) * (sizeof(char*) + sizeof(size_t) + sizeof(e_file_info))),
            totalFileNameLength + (fileCount * sizeof(char))
        );

        /* We need to copy the name into the buffer. We can use totalFileNameLength with fileCount to calculate the insertion position. */
        c89str_strcpy(pData + ((fileCount+1) * (sizeof(char*) + sizeof(size_t) + sizeof(e_file_info))) + totalFileNameLength + (fileCount * sizeof(char)), pName);


        /*
        With the name copied over we can now insert the file info. This is a similar process - we need to move the
        existing items down, and then insert the new one. 
        */
        E_MOVE_MEMORY(
            pData + ((fileCount+1) * (sizeof(char*) + sizeof(size_t))),
            pData + ((fileCount  ) * (sizeof(char*) + sizeof(size_t))),
            fileCount * sizeof(e_file_info)
        );

        /* We need to insert the file info. */
        E_COPY_MEMORY(
            pData + ((fileCount+1) * (sizeof(char*) + sizeof(size_t))) + (fileCount * sizeof(e_file_info)),
            &pFileIterator->info,
            sizeof(e_file_info)
        );


        /* Now the same for the size array. */
        E_MOVE_MEMORY(
            pData + ((fileCount+1) * sizeof(char*)),
            pData + ((fileCount  ) * sizeof(char*)),
            fileCount * sizeof(size_t)
        );

        /* We need to insert the size. */
        E_COPY_MEMORY(
            pData + ((fileCount+1) * sizeof(char*)) + (fileCount * sizeof(size_t)),
            &nameLen,
            sizeof(size_t)
        );


        totalFileNameLength += nameLen;
        fileCount += 1;
        dataSize += nameAndInfoDataSize;
    }

    if (pData == NULL) {
        /* There are no files in the directory. */
        return E_SUCCESS;
    }

    /* We now need to do a second pass to set up our char* pointers. */
    ppFileNames      = (char**      )(pData);
    pFileNameLengths = (size_t*     )(pData + (fileCount * sizeof(char*)));
    pFileInfos       = (e_file_info*)(pData + (fileCount * sizeof(char*)) + (fileCount * sizeof(size_t)));
    pRunningFileName = pData + (fileCount * (sizeof(char*) + sizeof(size_t) + sizeof(e_file_info)));

    E_ASSERT(ppFileNames != NULL);
    for (iFile = 0; iFile < fileCount; iFile += 1) {
        ppFileNames[iFile] = pRunningFileName;
        pRunningFileName += pFileNameLengths[iFile] + 1;    /* +1 to get past null terminator. */
    }


    /*
    Now we need to sort our arrays by name. We cannot use e_qsort() because we need to sort multiple arrays.

    TODO: Make this a quick sort.
    */
    for (iFile = 0; iFile < fileCount; iFile += 1) {
        size_t jFile;
        for (jFile = iFile+1; jFile < fileCount; jFile += 1) {
            if (c89str_strcmp(ppFileNames[iFile], ppFileNames[jFile]) > 0) {
                char* pTempName;
                size_t tempNameLen;
                e_file_info tempFileInfo;

                pTempName = ppFileNames[iFile];
                ppFileNames[iFile] = ppFileNames[jFile];
                ppFileNames[jFile] = pTempName;

                tempNameLen = pFileNameLengths[iFile];
                pFileNameLengths[iFile] = pFileNameLengths[jFile];
                pFileNameLengths[jFile] = tempNameLen;

                tempFileInfo = pFileInfos[iFile];
                pFileInfos[iFile] = pFileInfos[jFile];
                pFileInfos[jFile] = tempFileInfo;
            }
        }
    }


    /* Remove any duplicates. */
    for (iFile = 0; iFile < fileCount-1; ) {
        if (c89str_strcmp(ppFileNames[iFile], ppFileNames[iFile+1]) == 0) {
            /* Duplicate. */
            E_MOVE_MEMORY(ppFileNames      + iFile, ppFileNames      + iFile + 1, (fileCount - iFile - 1) * sizeof(char*));
            E_MOVE_MEMORY(pFileNameLengths + iFile, pFileNameLengths + iFile + 1, (fileCount - iFile - 1) * sizeof(size_t));
            E_MOVE_MEMORY(pFileInfos       + iFile, pFileInfos       + iFile + 1, (fileCount - iFile - 1) * sizeof(e_file_info));

            fileCount -= 1;
        } else {
            iFile += 1;
        }
    }

    if (pppFileNames != NULL) {
        *pppFileNames = ppFileNames;
    }
    if (ppFileNameLengths != NULL) {
        *ppFileNameLengths = pFileNameLengths;
    }
    if (ppFileInfos != NULL) {
        *ppFileInfos = pFileInfos;
    }
    if (pFileCount != NULL) {
        *pFileCount = fileCount;
    }

    return E_SUCCESS;
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

E_API e_result e_archive_open(e_archive* pArchive, const char* pFilePath, int openMode, const e_allocation_callbacks* pAllocationCallbacks, e_file** ppFile)
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

E_API e_fs_iterator* e_archive_first(e_archive* pArchive, const char* pDirectoryPath, size_t directoryPathLen, const e_allocation_callbacks* pAllocationCallbacks)
{
    return e_fs_first((e_fs*)pArchive, pDirectoryPath, directoryPathLen, pAllocationCallbacks);
}

E_API e_fs_iterator* e_archive_next(e_fs_iterator* pIterator, const e_allocation_callbacks* pAllocationCallbacks)
{
    return e_fs_next(pIterator, pAllocationCallbacks);
}

E_API void e_archive_free_iterator(e_fs_iterator* pIterator, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_fs_free_iterator(pIterator, pAllocationCallbacks);
}
/* ==== END e_archive.c ==== */



/* ==== BEG e_zip.c ==== */
/* The cache size must be at least 32K, and a power of 2. */
#ifndef E_ZIP_CACHE_SIZE_IN_BYTES
#define E_ZIP_CACHE_SIZE_IN_BYTES               32768
#endif

#ifndef E_ZIP_COMPRESSED_CACHE_SIZE_IN_BYTES
#define E_ZIP_COMPRESSED_CACHE_SIZE_IN_BYTES    4096
#endif

#define E_ZIP_EOCD_SIGNATURE                    0x06054b50
#define E_ZIP_EOCD64_SIGNATURE                  0x06064b50
#define E_ZIP_EOCD64_LOCATOR_SIGNATURE          0x07064b50
#define E_ZIP_CD_FILE_HEADER_SIGNATURE          0x02014b50


typedef struct e_zip_file     e_zip_file;
typedef struct e_zip_iterator e_zip_iterator;
typedef struct e_zip_cd_node  e_zip_cd_node;

typedef enum
{
    E_ZIP_COMPRESSION_METHOD_STORE   = 0,
    E_ZIP_COMPRESSION_METHOD_DEFLATE = 8
} e_zip_compression_method;

typedef struct
{
    const char* pPath;
    size_t pathLen;
    e_uint16 compressionMethod;
    e_uint64 compressedSize;
    e_uint64 uncompressedSize;
    e_uint64 fileOffset;            /* The offset in bytes from the start of the archive file. */
    e_bool32 directory;
} e_zip_file_info;

typedef struct
{
    size_t offsetInBytes;           /* The offset in bytes of the item relative to the start of the central directory. */
} e_zip_index;

struct e_zip_cd_node
{
    size_t iFile;                   /* Will be undefined for non leaf nodes. */
    const char* pName;
    size_t nameLen;
    size_t childCount;
    e_zip_cd_node* pChildren;
    size_t _descendantRangeBeg;     /* Only used for building the CD node graph. */
    size_t _descendantRangeEnd;     /* Only used for building the CD node graph. */
    size_t _descendantPrefixLen;    /* Only used for building the CD node graph. */
};

struct e_zip_iterator
{
    e_fs_iterator iterator;
    e_zip* pZip;
    e_zip_cd_node* pDirectoryNode;
    size_t iChild;
};

struct e_zip_file
{
    e_file file;
    e_zip_file_info info;
    e_uint64 absoluteCursorUncompressed;
    e_uint64 absoluteCursorCompressed;      /* The position of the cursor in the compressed data. */
    e_deflate_decompressor decompressor;    /* Only used for compressed files. */
    size_t cacheCap;                        /* The capacity of the cache. Never changes. */
    size_t cacheSize;                       /* The number of valid bytes in the cache. Can be less than the capacity, but never more. Will be less when holding the tail end fo the file data. */
    size_t cacheCursor;                     /* The cursor within the cache. The cache size minus the cursor defines how much data remains in the cache. */
    unsigned char* pCache;                  /* Cache must be at least 32K. Stores uncompressed data. Offset of _pCacheData. */
    size_t compressedCacheCap;              /* The capacity of the compressed cache. Never changes. */
    size_t compressedCacheSize;             /* The number of valid bytes in the compressed cache. Can be less than the capacity, but never more. Will be less when holding the tail end fo the file data. */
    size_t compressedCacheCursor;           /* The cursor within the compressed cache. The compressed cache size minus the cursor defines how much data remains in the compressed cache. */
    unsigned char* pCompressedCache;        /* Only used for compressed files. */
    unsigned char _pCacheData[1];           /* Contains the data of pCache and pCompressedCache. */
};

struct e_zip
{
    e_archive archive;                      /* A Zip is an archive, which itself is a file system. Therefore a e_zip object can be plugged into any e_fs API. */
    size_t fileCount;                       /* Total number of records in the central directory. */
    e_mutex* pLock;                         /* For mutual exclusion when reading file data across multiple threads. This is an offset of pHeap. */
    size_t centralDirectorySize;            /* Size in bytes of the central directory. */
    void* pCentralDirectory;                /* Offset of pHeap. */
    e_zip_index* pIndex;                    /* Offset of pHeap. There will be fileCount items in this array, and each item is sorted by the file path of each item. */
    e_zip_cd_node* pCDRootNode;             /* The root node of our accelerated central directory data structure. */
    void* pHeap;                            /* A single heap allocation for storing the central directory and index. */
};



static void e_zip_lock(e_zip* pZip)
{
    e_mutex_lock(pZip->pLock);
}

static void e_zip_unlock(e_zip* pZip)
{
    e_mutex_unlock(pZip->pLock);
}



typedef struct
{
    const char* str;
    size_t len;
} e_refstring;

static int e_binary_search_zip_cd_node_compare(void* pUserData, const void* pKey, const void* pVal)
{
    const e_refstring* pRefString = (const e_refstring*)pKey;
    const e_zip_cd_node* pNode = (const e_zip_cd_node*)pVal;
    int compareResult;

    (void)pUserData;

    compareResult = c89str_strncmp(pRefString->str, pNode->pName, E_MIN(pRefString->len, pNode->nameLen));
    if (compareResult == 0 && pRefString->len != pNode->nameLen) {
        compareResult = (pRefString->len < pNode->nameLen) ? -1 : 1;
    }

    return compareResult;
}

static e_zip_cd_node* e_zip_cd_node_find_child(e_zip_cd_node* pParent, const char* pChildName, size_t childNameLen)
{
    e_refstring str;
    str.str = pChildName;
    str.len = childNameLen;

    return (e_zip_cd_node*)e_sorted_search(&str, pParent->pChildren, pParent->childCount, sizeof(*pParent->pChildren), e_binary_search_zip_cd_node_compare, NULL);
}


static e_result e_archive_alloc_size_zip(void* pUserData, size_t* pSize)
{
    E_UNUSED(pUserData);
    E_ASSERT(pSize != NULL);

    *pSize = sizeof(e_zip);
    return E_SUCCESS;
}

static e_result e_zip_get_file_info_by_record_offset(e_zip* pZip, size_t offset, e_zip_file_info* pInfo)
{
    e_uint16 filePathLen;
    const unsigned char* pCentralDirectoryRecord;

    E_ASSERT(pZip  != NULL);
    E_ASSERT(pInfo != NULL);

    E_ZERO_OBJECT(pInfo);

    pCentralDirectoryRecord = (const unsigned char*)E_OFFSET_PTR(pZip->pCentralDirectory, offset);

    /* Check that we're not going to overflow the central directory. */
    if (offset + 46 > pZip->centralDirectorySize) {   /* 46 is the offset of the file path. */
        return E_INVALID_FILE;  /* Look like an invalid central directory. */
    }

    /* Grab the length of the file path. */
    filePathLen = ((e_uint16)pCentralDirectoryRecord[29] << 8) | pCentralDirectoryRecord[28];

    /* Now we can move to the file path, again making sure we have enough room for the file path. */
    if (offset + 46 + filePathLen > pZip->centralDirectorySize) {
        return E_INVALID_FILE;  /* Looks like an invalid central directory. */
    }

    pInfo->pPath   = (const char*)(pCentralDirectoryRecord + 46);
    pInfo->pathLen = filePathLen;

    /* We can determine if the entry is a directory by checking if the path ends in a slash. */
    if (pInfo->pPath[pInfo->pathLen-1] == '/' || pInfo->pPath[pInfo->pathLen-1] == '\\') {
        pInfo->directory = E_TRUE;
    }

    /* Compression method. */
    pInfo->compressionMethod = ((e_uint16)pCentralDirectoryRecord[11] << 8) | pCentralDirectoryRecord[10];

    /* Get the size of the file. */
    pInfo->compressedSize   = ((e_uint32)pCentralDirectoryRecord[23] << 24) | ((e_uint32)pCentralDirectoryRecord[22] << 16) | ((e_uint32)pCentralDirectoryRecord[21] << 8) | (e_uint32)pCentralDirectoryRecord[20];
    pInfo->uncompressedSize = ((e_uint32)pCentralDirectoryRecord[27] << 24) | ((e_uint32)pCentralDirectoryRecord[26] << 16) | ((e_uint32)pCentralDirectoryRecord[25] << 8) | (e_uint32)pCentralDirectoryRecord[24];

    /* File offset. */
    pInfo->fileOffset = ((e_uint32)pCentralDirectoryRecord[45] << 24) | ((e_uint32)pCentralDirectoryRecord[44] << 16) | ((e_uint32)pCentralDirectoryRecord[43] << 8) | pCentralDirectoryRecord[42];


    /*
    Load Zip64 data if necessary. It's in the extra data. The extra data is made up of a
    number of blocks. Each block has a 2 byte ID and a 2 byte size. When reading from
    each block, we need to make sure we don't try reading beyond the reported size of
    the extra data.
    
    The Zip64 data will be stored in a block with the ID of 0x0001. The presence of each
    member inside this block is conditional to whether or not it's set to 0xFFFFFFFF in
    the main part of the central directory record.
    */
    if (pInfo->compressedSize == 0xFFFFFFFF || pInfo->uncompressedSize == 0xFFFFFFFF || pInfo->fileOffset == 0xFFFFFFFF) {
        e_uint16 extraDataSize   = ((e_uint16)pCentralDirectoryRecord[31] << 8) | pCentralDirectoryRecord[30];
        e_uint16 extraDataOffset = 0;

        const unsigned char* pExtraData = (const unsigned char*)(pCentralDirectoryRecord + 46 + filePathLen);

        /* For each chunk in the extra data. */
        for (;;) {
            e_uint16 chunkID;
            e_uint16 chunkSize;

            if (extraDataOffset == extraDataSize) {
                break;  /* We're done. */
            }

            if (extraDataOffset > extraDataSize) {
                return E_INVALID_FILE;  /* We've somehow read past the extra data. Abort. */
            }

            if ((extraDataSize - extraDataOffset) < 4) {
                return E_INVALID_FILE;  /* Not enough data in the extra data to read the chunk header. */
            }

            chunkID   = ((e_uint16)pExtraData[extraDataOffset+1] << 8) | pExtraData[extraDataOffset+0];
            chunkSize = ((e_uint16)pExtraData[extraDataOffset+3] << 8) | pExtraData[extraDataOffset+2];

            /* Increment the offset to make it easy to parse the data in the next section. */
            extraDataOffset += 4;

            if ((extraDataSize - extraDataOffset) < chunkSize) {
                return E_INVALID_FILE;  /* Not enough data in the extra data to read the chunk. */
            }

            if (chunkID == 0x0001) {
                /* Zip64 data. */
                e_uint32 chunkLocalOffset = 0;

                if (pInfo->uncompressedSize == 0xFFFFFFFF) {
                    if (chunkLocalOffset + 8 > chunkSize) {
                        return E_INVALID_FILE;  /* Not enough data in the chunk. */
                    }

                    pInfo->uncompressedSize = ((e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+7] << 56) | ((e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+6] << 48) | ((e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+5] << 40) | ((e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+4] << 32) | ((e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+3] << 24) | ((e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+2] << 16) | ((e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+1] << 8) | (e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+0];
                    chunkLocalOffset += 8;
                }

                if (pInfo->compressedSize == 0xFFFFFFFF) {
                    if (chunkLocalOffset + 8 > chunkSize) {
                        return E_INVALID_FILE;  /* Not enough data in the chunk. */
                    }

                    pInfo->compressedSize = ((e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+7] << 56) | ((e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+6] << 48) | ((e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+5] << 40) | ((e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+4] << 32) | ((e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+3] << 24) | ((e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+2] << 16) | ((e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+1] << 8) | (e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+0];
                    chunkLocalOffset += 8;
                }

                if (pInfo->fileOffset == 0xFFFFFFFF) {
                    if (chunkLocalOffset + 8 > chunkSize) {
                        return E_INVALID_FILE;  /* Not enough data in the chunk. */
                    }

                    pInfo->fileOffset = ((e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+7] << 56) | ((e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+6] << 48) | ((e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+5] << 40) | ((e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+4] << 32) | ((e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+3] << 24) | ((e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+2] << 16) | ((e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+1] << 8) | (e_uint64)pExtraData[extraDataOffset+chunkLocalOffset+0];
                    chunkLocalOffset += 8;
                }
            }

            extraDataOffset += chunkSize;
        }
    }

    return E_SUCCESS;
}

static const char* e_zip_get_file_path_by_record_offset(e_zip* pZip, size_t offset, size_t* pLength)
{
    e_uint16 length;
    const char* pCentralDirectoryRecord;

    E_ASSERT(pLength != NULL);

    *pLength = 0;

    pCentralDirectoryRecord = (const char*)E_OFFSET_PTR(pZip->pCentralDirectory, offset);

    /* Check that we're not going to overflow the central directory. */
    if (offset + 46 > pZip->centralDirectorySize) {   /* 46 is the offset of the file path. */
        return NULL;    /* Look like an invalid central directory. */
    }

    /* Grab the length of the file. */
    length = ((e_uint16)pCentralDirectoryRecord[29] << 8) | pCentralDirectoryRecord[28];

    /* Now we can move to the file path, again making sure we have enough room for the file path. */
    if (offset + 46 + length > pZip->centralDirectorySize) {
        return NULL;    /* Looks like an invalid central directory. */
    }

    /* We now how enough information to get the file path. */
    *pLength = length;
    return pCentralDirectoryRecord + 46;
}

static e_result e_zip_find_file_by_path(e_zip* pZip, const char* pFilePath, size_t filePathLen, size_t* pFileIndex)
{
    e_result result;
    c89str_path_iterator pathIterator;
    e_zip_cd_node* pCurrentNode;

    E_ASSERT(pZip       != NULL);
    E_ASSERT(pFilePath  != NULL);
    E_ASSERT(pFileIndex != NULL);

    /* Start at the root node. */
    pCurrentNode = pZip->pCDRootNode;

    result = e_result_from_errno(c89str_path_first(pFilePath, filePathLen, &pathIterator));
    if (result != E_SUCCESS) {
        return result;  /* Probably trying to find the root item which does not have an index. */
    }

    for (;;) {
        e_zip_cd_node* pChildNode;
        
        pChildNode = e_zip_cd_node_find_child(pCurrentNode, pathIterator.pFullPath + pathIterator.segmentOffset, pathIterator.segmentLength);
        if (pChildNode == NULL) {
            return E_DOES_NOT_EXIST;
        }

        pCurrentNode = pChildNode;

        result = e_result_from_errno(c89str_path_next(&pathIterator));
        if (result != E_SUCCESS) {
            /* We've reached the end. The file we're on must be the file index. */
            *pFileIndex = pCurrentNode->iFile;
            return E_SUCCESS;
        }
    }

    /* Should never get here. */
    return E_DOES_NOT_EXIST;
}

static e_result e_zip_get_file_info_by_path(e_zip* pZip, const char* pFilePath, size_t filePathLen, e_zip_file_info* pInfo)
{
    e_result result;
    size_t iFile;

    E_ASSERT(pZip      != NULL);
    E_ASSERT(pFilePath != NULL);
    E_ASSERT(pInfo     != NULL);

    result = e_zip_find_file_by_path(pZip, pFilePath, filePathLen, &iFile);
    if (result != E_SUCCESS) {
        return result;  /* Most likely the file could not be found. */
    }

    return e_zip_get_file_info_by_record_offset(pZip, pZip->pIndex[iFile].offsetInBytes, pInfo);
}

static int e_zip_qsort_compare(void* pUserData, const void* a, const void* b)
{
    e_zip* pZip = (e_zip*)pUserData;
    const e_zip_index* pZipIndex0 = (const e_zip_index*)a;
    const e_zip_index* pZipIndex1 = (const e_zip_index*)b;
    size_t fileNameLen0;
    const char* pFileName0;
    size_t fileNameLen1;
    const char* pFileName1;
    int compareResult;

    E_ASSERT(pZip != NULL);

    pFileName0 = e_zip_get_file_path_by_record_offset(pZip, pZipIndex0->offsetInBytes, &fileNameLen0);
    if (pFileName0 == NULL) {
        pFileName0 = "";    /* File couldn't be found. Just treat it as an empty string. */
    }

    pFileName1 = e_zip_get_file_path_by_record_offset(pZip, pZipIndex1->offsetInBytes, &fileNameLen1);
    if (pFileName1 == NULL) {
        pFileName1 = "";    /* File couldn't be found. Just treat it as an empty string. */
    }

    compareResult = strncmp(pFileName0, pFileName1, E_MIN(fileNameLen0, fileNameLen1));
    if (compareResult == 0 && fileNameLen0 != fileNameLen1) {
        /* The strings are the same up to the length of the shorter string. The shorter string is considered to be less than the longer string. */
        compareResult = (fileNameLen0 < fileNameLen1) ? -1 : 1;
    }

    return compareResult;
}

static void e_zip_cd_node_build(e_zip* pZip, e_zip_cd_node** ppRunningChildrenPointer, e_zip_cd_node* pNode)
{
    size_t iFile;
    size_t iChild;

    E_ASSERT(pZip  != NULL);
    E_ASSERT(pNode != NULL);
    E_ASSERT(pNode->_descendantRangeEnd <= pZip->fileCount);
    E_ASSERT(ppRunningChildrenPointer != NULL);

    pNode->childCount = 0;
    pNode->pChildren  = *ppRunningChildrenPointer;

    /*
    We need to loop through our file range and add any direct children first. Then once that's
    done, we iterate over each child node and fill them out recursively.
    */
    for (iFile = pNode->_descendantRangeBeg; iFile < pNode->_descendantRangeEnd; iFile += 1) {
        const char* pFullFilePath;
        size_t fullFilePathLen;
        const char* pShortFilePath;
        size_t shortFilePathLen;
        c89str_path_iterator shortFilePathIterator;

        pFullFilePath = e_zip_get_file_path_by_record_offset(pZip, pZip->pIndex[iFile].offsetInBytes, &fullFilePathLen);
        if (pFullFilePath == NULL) {
            continue;   /* Should never happen. Just ignore the file if we couldn't find it by the given offset. */
        }

        /* If the full file path length is equal to the descendant prefix length, it means it's a leaf node. */
        E_ASSERT(fullFilePathLen >= pNode->_descendantPrefixLen);
        if (fullFilePathLen == pNode->_descendantPrefixLen) {
            continue;
        }

        /* The short file path is simply the full file path without the descendant prefix. */
        pShortFilePath   = pFullFilePath   + pNode->_descendantPrefixLen;
        shortFilePathLen = fullFilePathLen - pNode->_descendantPrefixLen;

        /* Make sure we're not sitting on a path separator. */
        if (pShortFilePath[0] == '\\' || pShortFilePath[0] == '/') {
            pShortFilePath   += 1;
            shortFilePathLen -= 1;
        }

        /*
        Now we need to check if we need to add a child. Because this main file listing is sorted,
        we need only check the last child item. If it's not equal, we have a new child item.
        */
        if (c89str_path_first(pShortFilePath, shortFilePathLen, &shortFilePathIterator) == C89STR_SUCCESS) {
            if (pNode->childCount == 0 || pNode->pChildren[pNode->childCount-1].nameLen != shortFilePathIterator.segmentLength || c89str_strncmp(pNode->pChildren[pNode->childCount-1].pName, shortFilePathIterator.pFullPath + shortFilePathIterator.segmentOffset, E_MIN(pNode->pChildren[pNode->childCount-1].nameLen, shortFilePathIterator.segmentLength)) != 0) {
                /* Child doesn't exist. Need to add it to the list. */
                pNode->pChildren[pNode->childCount].iFile                = iFile;
                pNode->pChildren[pNode->childCount].pName                = shortFilePathIterator.pFullPath + shortFilePathIterator.segmentOffset;
                pNode->pChildren[pNode->childCount].nameLen              = shortFilePathIterator.segmentLength;
                pNode->pChildren[pNode->childCount]._descendantRangeBeg  = iFile;
                pNode->pChildren[pNode->childCount]._descendantRangeEnd  = pNode->_descendantRangeEnd;
                pNode->pChildren[pNode->childCount]._descendantPrefixLen = (fullFilePathLen - shortFilePathLen) + shortFilePathIterator.segmentLength;

                /* Update the end range for the previous child if we have one. */
                if (pNode->childCount > 0) {
                    pNode->pChildren[pNode->childCount-1]._descendantRangeEnd = iFile;
                }

                pNode->childCount         += 1;
                *ppRunningChildrenPointer += 1;
            }
        } else {
            /*
            Couldn't get the first segment. This probably means we found an explicit directory
            listing. We just ignore it.
            */
            if (pNode->childCount > 0) {
                pNode->pChildren[pNode->childCount-1]._descendantRangeEnd = iFile;
            }
        }
    }

    /* We've initialized each of the child nodes. We now need to recursively process them. */
    for (iChild = 0; iChild < pNode->childCount; iChild += 1) {
        e_zip_cd_node_build(pZip, ppRunningChildrenPointer, &pNode->pChildren[iChild]);
    }
}

static e_result e_archive_init_zip(void* pUserData, e_stream* pStream, const e_allocation_callbacks* pAllocationCallbacks, e_archive* pArchive)
{
    e_zip* pZip = (e_zip*)pArchive;
    e_result result;
    e_uint32 sig;
    int eocdPositionFromEnd;
    e_uint16 cdRecordCount16;
    e_uint64 cdRecordCount64;
    e_uint32 cdSizeInBytes32;
    e_uint64 cdSizeInBytes64;
    e_uint32 cdOffset32;
    e_uint64 cdOffset64;

    E_UNUSED(pUserData);
    E_ASSERT(pZip != NULL);

    /*
    The correct way to load a Zip file is to read from the central directory. The end of the
    central directory is the first thing we need to find and is sitting at the end of the file. The
    most efficient way to find this is to look for the end of central directory signature. The
    EOCD record is at least 22 bytes, but may be larger if there is a comment. The maximum size
    will be 22 + 65535.

    The way we'll do it is we'll first assume there is no comment and try reading from byte -22
    starting from the end. If the first 4 bytes are equal to the EOCD signature we'll treat that as
    the start of the EOCD and read from there. If this fails it probably means there's a comment in
    which case we'll go to byte -(22 + 65535) and scan from there.
    */
    result = e_stream_seek(pStream, -22, E_SEEK_ORIGIN_END);
    if (result != E_SUCCESS) {
        return result;  /* Failed to seek to our EOCD. This cannot be a valid Zip file. */
    }

    result = e_stream_read(pStream, &sig, 4, NULL);
    if (result != E_SUCCESS) {
        return result;
    }

    if (sig == E_ZIP_EOCD_SIGNATURE) {
        /* Found the EOCD. It's at position -22. */
        eocdPositionFromEnd = -22;
    } else {
        /*
        The EOCD record is not located at position -22. There might be a comment which means the
        EOCD signature is sitting further up the file. The comment has a maximum if 65535
        characters, so we'll start searching from -(22 + 65535).
        */
        result = e_stream_seek(pStream, -(22 + 65535), E_SEEK_ORIGIN_END);
        if (result != E_SUCCESS) {
            /*
            We failed the seek, but it most likely means we were just trying to seek to far back in
            which case we can just fall back to a seek to position 0.
            */
            result = e_stream_seek(pStream, 0, E_SEEK_ORIGIN_START);
            if (result != E_SUCCESS) {
                return result;
            }
        }

        /*
        We now need to scan byte-by-byte until we find the signature. We could allocate this on the
        stack, but that takes a little bit too much stack space that I feel comfortable with. We
        could also allocate a buffer on the heap, but that just needlessly inefficient. Instead
        we'll run in a loop and read into a 4K stack allocated buffer.
        */
        {
            unsigned char buffer[4096];
            size_t bufferCursor;
            size_t bufferSize;
            size_t totalBytesScanned;

            totalBytesScanned = 0;
            bufferCursor = 0;
            for (;;) {
                size_t bufferRemaining;
                e_bool32 foundEOCD = E_FALSE;

                result = e_stream_read(pStream, buffer + bufferCursor, sizeof(buffer) - bufferCursor, &bufferSize);
                if (result != E_SUCCESS) {
                    return E_INVALID_FILE;  /* If we get here it most likely means we've reached the end of the file. In any case, we're can't continue. */
                }

                /* Make sure we account for the offset when determining the buffer size. */
                bufferSize += bufferCursor;
                if (bufferSize < 4) {
                    return E_INVALID_FILE;  /* Didn't read enough data. Not even enough to read a full signature. */
                }

                for (; bufferCursor <= (bufferSize - 4); bufferCursor += 1) {
                    /* Is it safe to do unaligned access like this on all platforms? Safer to do a byte-by-byte comparison? */
                    if (*(e_uint32*)(buffer + bufferCursor) == E_ZIP_EOCD_SIGNATURE) {
                        /* The signature has been found. */
                        foundEOCD = E_TRUE;
                        break;
                    }
                }

                totalBytesScanned += bufferCursor;

                if (foundEOCD) {
                    /*
                    We found the EOCD. A complication here is that the stream's cursor won't
                    be sitting in the correct location because we were reading in chunks.
                    */
                    eocdPositionFromEnd = -(22 + 65535) + (int)totalBytesScanned;   /* Safe cast to int. */

                    result = e_stream_seek(pStream, eocdPositionFromEnd + 4, E_SEEK_ORIGIN_END);  /* +4 so go just past the signatures. */
                    if (result != E_SUCCESS) {
                        return result;
                    }

                    /* Just setting the signature here to keep the state of our local variables consistent. */
                    sig = E_ZIP_EOCD_SIGNATURE;

                    /* Get out of the chunk loop. */
                    break;
                }

                /*
                Getting here means we didn't find the signature in this chunk. We need to move the
                cursor back and read another chunk.
                */
                bufferRemaining = bufferSize - bufferCursor;
                E_MOVE_MEMORY(buffer, buffer + bufferCursor, bufferRemaining);
                bufferCursor = bufferRemaining;
            }
        }
    }

    /*
    Getting here means we must have found the EOCD record. We can now parse it. The EOCD will give
    us information that we could use to determine if it's a Zip64 record.

    We're ignoring the disk properties. Split Zip files are not being supported here.
    */
    result = e_stream_seek(pStream, 2 + 2 + 2, E_SEEK_ORIGIN_CURRENT);  /* Skip past disk stuff. */
    if (result != E_SUCCESS) {
        return E_INVALID_FILE;
    }

    result = e_stream_read(pStream, &cdRecordCount16, 2, NULL);
    if (result != E_SUCCESS) {
        return E_INVALID_FILE;
    }

    result = e_stream_read(pStream, &cdSizeInBytes32, 4, NULL);
    if (result != E_SUCCESS) {
        return E_INVALID_FILE;
    }

    result = e_stream_read(pStream, &cdOffset32, 4, NULL);
    if (result != E_SUCCESS) {
        return E_INVALID_FILE;
    }

    /*
    The last part will be the comment. We don't care about this, and this is the last part of the
    file so we just leave it.
    */

    /*
    We'll now need to decide if we need to read some Zip64 information. To determine this we just
    need to look at the content of the record count, size and offset.
    */
    if (cdRecordCount16 == 0xFFFF || cdSizeInBytes32 == 0xFFFFFFFF || cdOffset32 == 0xFFFFFFFF) {
        /*
        It's a Zip64 file. We need to find the EOCD64 locator which will be 20 bytes before the EOCD
        that we just read. If we can't find the EOCD64 locator we'll just abort.
        */
        e_uint64 eocd64SizeInBytes;
        e_int64 eocd64OffsetInBytes;

        result = e_stream_seek(pStream, eocdPositionFromEnd - 20, E_SEEK_ORIGIN_END);
        if (result != E_SUCCESS) {
            return E_INVALID_FILE;
        }

        result = e_stream_read(pStream, &sig, 4, NULL);
        if (result != E_SUCCESS) {
            return E_INVALID_FILE;
        }

        if (sig != E_ZIP_EOCD64_LOCATOR_SIGNATURE) {
            /* TODO: We can try falling back to a method that scans for the EOCD64. Would work just like the regular EOCD that we just read. */
            return E_INVALID_FILE;  /* Couldn't find the EOCD64 locator. Abort. */
        }

        /* We don't use the next 4 bytes so skip it. */
        result = e_stream_seek(pStream, 4, E_SEEK_ORIGIN_CURRENT);
        if (result != E_SUCCESS) {
            return E_INVALID_FILE;
        }

        /* The next 8 bytes contains the offset to the EOCD64. */
        result = e_stream_read(pStream, &eocd64OffsetInBytes, 8, NULL);
        if (result != E_SUCCESS) {
            return E_INVALID_FILE;
        }

        /*
        The next 4 bytes contains the number of disks. We're not supporting split Zip files, so we
        don't need to care about this. Just seek straight to the EOCD64 record.
        */
        result = e_stream_seek(pStream, eocd64OffsetInBytes, E_SEEK_ORIGIN_START);
        if (result != E_SUCCESS) {
            return E_INVALID_FILE;
        }


        /* Getting here means we've found the EOCD64. We can now parse it. */
        result = e_stream_read(pStream, &sig, 4, NULL);
        if (result != E_SUCCESS) {
            return E_INVALID_FILE;
        }

        if (sig != E_ZIP_EOCD64_SIGNATURE) {
            return E_INVALID_FILE;  /* Couldn't find the EOCD64. Abort. */
        }

        /* Getting here means we've found the EOCD64 locator. The next 8 bytes contains the size of the EOCD64 minus 12. */
        result = e_stream_read(pStream, &eocd64SizeInBytes, 8, NULL);
        if (result != E_SUCCESS) {
            return E_INVALID_FILE;
        }

        /* The EOCD64 must be at least 36 bytes. */
        if (eocd64SizeInBytes < 36) {
            return E_INVALID_FILE;
        }

        /* We can skip past everything up to the record count, which is 20 bytes. */
        result = e_stream_seek(pStream, 20, E_SEEK_ORIGIN_CURRENT);
        if (result != E_SUCCESS) {
            return E_INVALID_FILE;
        }

        /* The next three items are the record count, size in bytes and offset, which are all 8 bytes. */
        result = e_stream_read(pStream, &cdRecordCount64, 8, NULL);
        if (result != E_SUCCESS) {
            return E_INVALID_FILE;
        }

        result = e_stream_read(pStream, &cdSizeInBytes64, 8, NULL);
        if (result != E_SUCCESS) {
            return E_INVALID_FILE;
        }

        result = e_stream_read(pStream, &cdOffset64, 8, NULL);
        if (result != E_SUCCESS) {
            return E_INVALID_FILE;
        }
        
        if (cdRecordCount64 > E_SIZE_MAX) {
            return E_TOO_BIG;   /* Too many records. Will never fit what we need in memory. */
        }
        if (cdSizeInBytes64 > E_SIZE_MAX) {
            return E_TOO_BIG;   /* Central directory is too big to fit into memory. */
        }

        pZip->fileCount = cdRecordCount64;
        
    } else {
        /* It's a 32-bit archive. */
        pZip->fileCount = cdRecordCount16;
        pZip->centralDirectorySize = cdSizeInBytes32;

        cdSizeInBytes64 = cdSizeInBytes32;
        cdOffset64 = cdOffset32;
    }

    /* We need to seek to the start of the central directory and read it's contents. */
    result = e_stream_seek(pStream, cdOffset64, E_SEEK_ORIGIN_START);
    if (result != E_SUCCESS) {
        return E_INVALID_FILE;
    }

    /* At this point we'll be sitting on the central directory. */

    /*
    We don't parse the central directory here. Instead we just allocate a block of memory and read
    straight into that. Then when we need to load a file we just iterate over the central
    directory.
    */
    if (cdSizeInBytes64 > E_SIZE_MAX) {
        return E_TOO_BIG;   /* The size of the central directory is too big. */
    }

    pZip->centralDirectorySize = (size_t)cdSizeInBytes64;


    pZip->pHeap = e_malloc(e_mutex_alloc_size() + pZip->centralDirectorySize + (sizeof(*pZip->pIndex) * pZip->fileCount), pAllocationCallbacks);
    if (pZip->pHeap == NULL) {
        return E_OUT_OF_MEMORY;
    }

    pZip->pLock             = (e_mutex*    )E_OFFSET_PTR(pZip->pHeap, 0);
    pZip->pCentralDirectory =               E_OFFSET_PTR(pZip->pHeap, e_mutex_alloc_size());
    pZip->pIndex            = (e_zip_index*)E_OFFSET_PTR(pZip->pHeap, e_mutex_alloc_size() + pZip->centralDirectorySize);
    pZip->pCDRootNode       = NULL; /* <-- This will be set later. */

    result = e_stream_read(pStream, pZip->pCentralDirectory, pZip->centralDirectorySize, NULL);
    if (result != E_SUCCESS) {
        return E_INVALID_FILE;
    }

    /* Build the index. It needs to be sorted by name. We'll treat this as case-sensitive. */
    {
        e_memory_stream cdStream;
        size_t iFile;
        size_t nodeUpperBoundCount;

        result = e_memory_stream_init_readonly(pZip->pCentralDirectory, pZip->centralDirectorySize, &cdStream);
        if (result != E_SUCCESS) {
            e_free(pZip->pHeap, pAllocationCallbacks);
            return result;
        }

        for (iFile = 0; iFile < pZip->fileCount; iFile += 1) {
            size_t fileOffset;
            e_uint16 fileNameLen;
            e_uint16 extraLen;
            e_uint16 commentLen;

            result = e_memory_stream_tell(&cdStream, &fileOffset);
            if (result != E_SUCCESS) {
                e_free(pZip->pHeap, pAllocationCallbacks);
                return result;
            }

            pZip->pIndex[iFile].offsetInBytes = fileOffset;


            /*
            We need to seek to the next item. To do this we need to retrieve the lengths of the
            variable-length fields. These start from offset 28.
            */
            result = e_memory_stream_seek(&cdStream, 28, E_SEEK_ORIGIN_CURRENT);
            if (result != E_SUCCESS) {
                e_free(pZip->pHeap, pAllocationCallbacks);
                return result;
            }

            result = e_memory_stream_read(&cdStream, &fileNameLen, 2, NULL);
            if (result != E_SUCCESS) {
                e_free(pZip->pHeap, pAllocationCallbacks);
                return result;
            }

            result = e_memory_stream_read(&cdStream, &extraLen, 2, NULL);
            if (result != E_SUCCESS) {
                e_free(pZip->pHeap, pAllocationCallbacks);
                return result;
            }

            result = e_memory_stream_read(&cdStream, &commentLen, 2, NULL);
            if (result != E_SUCCESS) {
                e_free(pZip->pHeap, pAllocationCallbacks);
                return result;
            }

            /* We have the necessary information we need to move past this record. */
            result = e_memory_stream_seek(&cdStream, fileOffset + 46 + fileNameLen + extraLen + commentLen, E_SEEK_ORIGIN_START);
            if (result != E_SUCCESS) {
                e_free(pZip->pHeap, pAllocationCallbacks);
                return result;
            }
        }

        /*
        TODO: Look at some real-world Zip archives from various archivers (7zip, Windows, etc.) and
        check how the sorting looks before our explicit sort. If most real-world archives are already
        mostly sorted, it might be more efficient to just do a simple insertion sort.
        */
        e_sort(pZip->pIndex, pZip->fileCount, sizeof(e_zip_index), e_zip_qsort_compare, pZip);

        /* Testing. */
        #if 0
        {
            size_t i;
            for (i = 0; i < pZip->fileCount; i += 1) {
                size_t nameLen;
                const char* pName = e_zip_get_file_path_by_record_offset(pZip, pZip->pIndex[i].offsetInBytes, &nameLen);

                printf("File name = %.*s\n", (int)nameLen, pName);
            }
        }
        #endif

        /*
        We're going to build an accelerated data structure for the central directory. Nothing over
        the top - just a simple tree based on directory names.

        It's just a graph. Each node in the graph is either a directory or a file. Leaf nodes can
        possibly be files or an empty directory which means a flag is required to indicate whether
        or not the node is a directory. Sub-folders and files are just child nodes. Children are
        sorted by name to allow for fast lookups.

        The items in the central directory has already been sorted thanks to the index that we
        constructed above. If we just iterate linearly based on that index everything should be
        sorted naturally.

        The graph is constructed in two passes. The first pass simply counts the number of nodes so
        we can allocate a single block of memory. The second pass fills the data.
        */

        /*
        The first pass is just to count the number of nodes so we can allocate some memory in one
        chunk. We start the count at one to accommodate for the root node. This pass is not
        necessarily calculating an exact count, but instead it calculates an upper bound count. The
        reason for this is how directories are handled. Sometimes they are listed explicitly, but I
        have seen cases where they're not. If we could guarantee all folders were explicitly listed
        we would be able to avoid this pass.

        We can take advantage of the fact that the file listing has been sorted. For each entry we
        just compare the path with the previous one, and for every segment in the new path that's
        different we increment the counter (it should always be at least one since the file name
        itself should always be different).
        */
        {
            const char* pPrevPath;
            size_t prevPathLen;
            
            /* Consider the root directory to be the previous path. */
            pPrevPath = "";
            prevPathLen = 0;

            /* Start the count at 1 to account for the root node. */
            nodeUpperBoundCount = 1;

            for (iFile = 0; iFile < pZip->fileCount; iFile += 1) {
                const char* pFilePath;
                size_t filePathLen;

                pFilePath = e_zip_get_file_path_by_record_offset(pZip, pZip->pIndex[iFile].offsetInBytes, &filePathLen);
                if (pFilePath == NULL) {
                    continue;   /* Just skip the file if we can't get the name. Should never happen. */
                }

                /*
                Now that we have the file path all we need to do is compare is to the previous path
                and increment the counter for every segment in the current path that is different
                to the previous path. We'll need to use a path iterator for each of these.
                */
                {
                    c89str_path_iterator nextIterator;
                    c89str_path_iterator prevIterator;

                    c89str_path_first(pFilePath, filePathLen, &nextIterator);   /* <-- This should never fail. */

                    if (c89str_path_first(pPrevPath, prevPathLen, &prevIterator) == C89STR_SUCCESS) {
                        /*
                        First just move the next iterator forward until we reach the end of the previous
                        iterator, or if the segments differ between the two.
                        */
                        for (;;) {
                            if (c89str_path_iterators_compare(&nextIterator, &prevIterator) != 0) {
                                break;  /* Iterators don't match. */
                            }

                            /* Getting here means the segments match. We need to move to the next one. */
                            if (c89str_path_next(&nextIterator) != C89STR_SUCCESS) {
                                break;  /* We reached the end of the next iterator before the previous. The only difference will be the file name. */
                            }

                            if (c89str_path_next(&prevIterator) != C89STR_SUCCESS) {
                                break;  /* We reached the end of the prev iterator. Get out of the loop. */
                            }
                        }
                    }

                    /* Increment the counter to account for the segment that the next iterator is currently sitting on. */
                    nodeUpperBoundCount += 1;

                    /* Now we need to increment the counter for every new segment. */
                    while (c89str_path_next(&nextIterator) == C89STR_SUCCESS) {
                        nodeUpperBoundCount += 1;
                    }
                }

                /* Getting here means we're done with the count for this item. Move to the next one. */
                pPrevPath = pFilePath;
                prevPathLen = filePathLen;
            }
        }

        /*
        Now that we've got the count we can go ahead and resize our heap allocation. It's important
        to remember to update our pointers here.
        */
        {
            void* pNewHeap = e_realloc(pZip->pHeap, e_mutex_alloc_size() + pZip->centralDirectorySize + (sizeof(*pZip->pIndex) * pZip->fileCount) + (sizeof(*pZip->pCDRootNode) * nodeUpperBoundCount), pAllocationCallbacks);
            if (pNewHeap == NULL) {
                e_free(pZip->pHeap, pAllocationCallbacks);
                return E_OUT_OF_MEMORY;
            }

            pZip->pHeap = pNewHeap;
            pZip->pLock             = (e_mutex*      )E_OFFSET_PTR(pZip->pHeap, 0);
            pZip->pCentralDirectory =                 E_OFFSET_PTR(pZip->pHeap, e_mutex_alloc_size());
            pZip->pIndex            = (e_zip_index*  )E_OFFSET_PTR(pZip->pHeap, e_mutex_alloc_size() + pZip->centralDirectorySize);
            pZip->pCDRootNode       = (e_zip_cd_node*)E_OFFSET_PTR(pZip->pHeap, e_mutex_alloc_size() + pZip->centralDirectorySize + (sizeof(*pZip->pIndex) * pZip->fileCount));
        }

        /*
        Memory has been allocated so we can now fill it out. This is slightly tricky because we want
        to do it in a single pass with a single memory allocation. Each node will hold a pointer to
        an array which will contain their children. The size of this array is unknown at this point
        so we need to come up with a system that allows us to fill each node in order.

        Fortunately our file listing is sorted which gives us a good start. We want to fill out
        higher level nodes first and then move down to leaf nodes. We're going to run through the
        file listing in sorted order. For the current file path, we need to look at it's directory
        structure. For each segment of the directory there will be a node. For each of these
        segments we'll run an inner loop that adds child nodes for each file that shares the same
        prefix.

        To put simply, for each node, we need to attach all of it's children before the child nodes
        themselves have been filled with their children. We can do this recursively. The first node
        we're filling is the root node.
        */
        {
            e_zip_cd_node* pRunningChildrenPointer = &pZip->pCDRootNode[1];

            /* The root node needs to be set up first. */
            pZip->pCDRootNode->pName                = "";
            pZip->pCDRootNode->nameLen              = 0;
            pZip->pCDRootNode->_descendantRangeBeg  = 0;
            pZip->pCDRootNode->_descendantRangeEnd  = pZip->fileCount;
            pZip->pCDRootNode->_descendantPrefixLen = 0;

            e_zip_cd_node_build(pZip, &pRunningChildrenPointer, pZip->pCDRootNode);
        }
    }

    /*
    We need a mutex to make reading from multiple files across different threads thread-safe. This
    needs to be done last because we can't be changing the address of the mutex, which may happen
    due to the realloc() above.
    */
    result = e_mutex_init_preallocated(pZip->pLock);
    if (result != E_SUCCESS) {
        /*
        Initialization of the mutex failed, but it's not really a critical error. It just means it
        won't be thread safe. In practice, this will only happen on single-threaded platforms which
        we'll never be running on anyway.
        */
    }

    /* That's all we need to do for now. */
    return E_SUCCESS;
}

static void e_archive_uninit_zip(void* pUserData, e_archive* pArchive, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_zip* pZip = (e_zip*)pArchive;

    E_ASSERT(pZip != NULL);
    E_UNUSED(pUserData);

    e_free(pZip->pHeap, pAllocationCallbacks);
}

static e_result e_archive_file_alloc_size_zip(void* pUserData, size_t* pSize)
{
    E_UNUSED(pUserData);

    *pSize = sizeof(e_zip_file) + E_ZIP_CACHE_SIZE_IN_BYTES + E_ZIP_COMPRESSED_CACHE_SIZE_IN_BYTES;
    return E_SUCCESS;
}

static e_result e_archive_open_zip(void* pUserData, e_fs* pFS, const char* pFilePath, int openMode, const e_allocation_callbacks* pAllocationCallbacks, e_file* pFile)
{
    e_zip* pZip = (e_zip*)pFS;
    e_zip_file* pZipFile = (e_zip_file*)pFile;
    e_result result;

    E_UNUSED(pUserData);
    (void)pAllocationCallbacks;

    E_ZERO_OBJECT(pZipFile);

    /* Write mode is currently unsupported. */
    if ((openMode & (E_OPEN_MODE_WRITE | E_OPEN_MODE_APPEND | E_OPEN_MODE_TRUNCATE)) != 0) {
        return E_INVALID_OPERATION;
    }

    /* We need to find the file info by it's path. */
    result = e_zip_get_file_info_by_path(pZip, pFilePath, (size_t)-1, &pZipFile->info);
    if (result != E_SUCCESS) {
        return result;  /* Probably not found. */
    }

    /* We can't be trying to open a directory. */
    if (pZipFile->info.directory) {
        return E_IS_DIRECTORY;
    }

    /* Validate the compression method. We're only supporting Store and Deflate. */
    if (pZipFile->info.compressionMethod != E_ZIP_COMPRESSION_METHOD_STORE && pZipFile->info.compressionMethod != E_ZIP_COMPRESSION_METHOD_DEFLATE) {
        return E_INVALID_FILE;
    }

    /* Make double sure the cursor is at the start. */
    pZipFile->absoluteCursorUncompressed = 0;
    pZipFile->cacheCap = E_ZIP_CACHE_SIZE_IN_BYTES;

    /*
    We allocated memory for a compressed cache, even when the file is not compressed. Make use
    of this memory if the file is not compressed.
    */
    if (pZipFile->info.compressionMethod == E_ZIP_COMPRESSION_METHOD_STORE) {
        pZipFile->cacheCap          += E_ZIP_COMPRESSED_CACHE_SIZE_IN_BYTES;
        pZipFile->compressedCacheCap = 0;
    } else {
        pZipFile->compressedCacheCap = E_ZIP_COMPRESSED_CACHE_SIZE_IN_BYTES;
    }

    pZipFile->pCache           = pZipFile->_pCacheData;
    pZipFile->pCompressedCache = pZipFile->_pCacheData + pZipFile->cacheCap;

    /*
    We need to move the file offset forward so that it's pointing to the first byte of the actual
    data. It's currently sitting at the top of the local header which isn't really useful for us.
    To move forward we need to get the length of the file path and the extra data and seek past
    the local header.
    */
    e_zip_lock(pZip);
    {
        e_stream* pArchiveStream;
        e_uint16 fileNameLen;
        e_uint16 extraLen;

        pArchiveStream = e_archive_stream(&pZip->archive);

        result = e_stream_seek(pArchiveStream, pZipFile->info.fileOffset + 26, E_SEEK_ORIGIN_START);
        if (result != E_SUCCESS) {
            e_zip_unlock(pZip);
            return result;
        }

        result = e_stream_read(pArchiveStream, &fileNameLen, 2, NULL);
        if (result != E_SUCCESS) {
            e_zip_unlock(pZip);
            return result;
        }

        result = e_stream_read(pArchiveStream, &extraLen, 2, NULL);
        if (result != E_SUCCESS) {
            e_zip_unlock(pZip);
            return result;
        }

        pZipFile->info.fileOffset += (e_uint32)30 + fileNameLen + extraLen;
    }
    e_zip_unlock(pZip);


    /* Initialize the decompressor if necessary. */
    if (pZipFile->info.compressionMethod == E_ZIP_COMPRESSION_METHOD_DEFLATE) {
        result = e_deflate_decompressor_init(&pZipFile->decompressor);
        if (result != E_SUCCESS) {
            return result;
        }
    }


    return E_SUCCESS;
}

static void e_archive_close_zip(void* pUserData, e_file* pFile, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_zip_file* pZipFile = (e_zip_file*)pFile;

    E_ASSERT(pZipFile != NULL);
    E_UNUSED(pUserData);

    /* Nothing to do here. */
    (void)pZipFile;
    (void)pAllocationCallbacks;
}

static e_result e_archive_read_zip_store(e_zip_file* pZipFile, void* pDst, size_t bytesToRead, size_t* pBytesRead)
{
    e_result result;
    e_zip* pZip;
    e_uint64 bytesRemainingInFile;
    size_t bytesRead;

    E_ASSERT(pZipFile   != NULL);
    E_ASSERT(pBytesRead != NULL);

    pZip = (e_zip*)pZipFile->file.pFS;

    bytesRemainingInFile = pZipFile->info.uncompressedSize - pZipFile->absoluteCursorUncompressed;
    if (bytesToRead > bytesRemainingInFile) {
        bytesToRead = (size_t)bytesRemainingInFile;
    }

    bytesRead = 0;

    /*
    Before we spend any time locking the archive we should read from the cache. If the cache gets
    exhausted before we've finished reading, we can then lock the archive and ready the rest of
    the data.
    */
    {
        size_t bytesRemainingInCache = pZipFile->cacheSize - pZipFile->cacheCursor;
        size_t bytesToReadFromCache = bytesToRead;
        if (bytesToReadFromCache > bytesRemainingInCache) {
            bytesToReadFromCache = bytesRemainingInCache;
        }

        E_COPY_MEMORY(pDst, pZipFile->pCache + pZipFile->cacheCursor, bytesToReadFromCache);
        pZipFile->cacheCursor += bytesToReadFromCache;

        bytesRead = bytesToReadFromCache;
    }

    if (bytesRead < bytesToRead) {
        /*
        There's more data to read. If there's more data remaining than the cache capacity, we
        simply load some data straight into the output buffer. Any remainder we load into the
        cache and then read from that.
        */
        e_zip_lock(pZip);
        {
            size_t bytesRemainingToRead = bytesToRead - bytesRead;
            size_t bytesToReadFromArchive;

            result = e_stream_seek(e_archive_stream(&pZip->archive), pZipFile->info.fileOffset + (pZipFile->absoluteCursorUncompressed + bytesRead), E_SEEK_ORIGIN_START);
            if (result != E_SUCCESS) {
                e_zip_unlock(pZip);
                return result;
            }

            if (bytesRemainingToRead > pZipFile->cacheCap) {
                size_t bytesReadFromArchive;

                bytesToReadFromArchive = (bytesRemainingToRead / pZipFile->cacheCap) * pZipFile->cacheCap;

                result = e_stream_read(e_archive_stream(&pZip->archive), E_OFFSET_PTR(pDst, bytesRead), bytesToReadFromArchive, &bytesReadFromArchive);
                if (result != E_SUCCESS) {
                    e_zip_unlock(pZip);
                    return result;
                }

                bytesRead += bytesReadFromArchive;
                bytesRemainingToRead -= bytesReadFromArchive; 
            }

            /*
            At this point we should have less than the cache capacity remaining to read. We need to
            read into the cache, and then read any leftover from it.
            */
            if (bytesRemainingToRead > 0) {
                E_ASSERT(bytesRemainingToRead < pZipFile->cacheCap);

                result = e_stream_read(e_archive_stream(&pZip->archive), pZipFile->pCache, E_MIN(pZipFile->cacheCap, (pZipFile->info.uncompressedSize - (pZipFile->absoluteCursorUncompressed + bytesRead))), &pZipFile->cacheSize);
                if (result != E_SUCCESS) {
                    e_zip_unlock(pZip);
                    return result;
                }

                pZipFile->cacheCursor = 0;

                E_COPY_MEMORY(E_OFFSET_PTR(pDst, bytesRead), pZipFile->pCache + pZipFile->cacheCursor, bytesRemainingToRead);
                pZipFile->cacheCursor += bytesRemainingToRead;

                bytesRead += bytesRemainingToRead;
            }
        }
        e_zip_unlock(pZip);
    }

    pZipFile->absoluteCursorUncompressed += bytesRead;

    /* We're done. */
    *pBytesRead = bytesRead;
    return E_SUCCESS;
}

static e_result e_archive_read_zip_deflate(e_zip_file* pZipFile, void* pDst, size_t bytesToRead, size_t* pBytesRead)
{
    e_result result;
    e_zip* pZip;
    e_uint64 uncompressedBytesRemainingInFile;
    size_t uncompressedBytesRead;

    E_ASSERT(pZipFile != NULL);
    E_ASSERT(pBytesRead != NULL);

    pZip = (e_zip*)pZipFile->file.pFS;

    uncompressedBytesRemainingInFile = pZipFile->info.uncompressedSize - pZipFile->absoluteCursorUncompressed;
    if (bytesToRead > uncompressedBytesRemainingInFile) {
        bytesToRead = (size_t)uncompressedBytesRemainingInFile;
    }

    uncompressedBytesRead = 0;


    /*
    The way reading works for deflate is that we need to read from the cache until it's exhausted,
    and then refill it and read from it again. We need to do this until we've read the requested
    number of bytes.
    */
    for (;;) {
        /* Read from the cache first. */
        size_t bytesRemainingInCache = pZipFile->cacheSize - pZipFile->cacheCursor;
        size_t bytesToReadFromCache = bytesToRead - uncompressedBytesRead;
        if (bytesToReadFromCache > bytesRemainingInCache) {
            bytesToReadFromCache = bytesRemainingInCache;
        }

        E_COPY_MEMORY(E_OFFSET_PTR(pDst, uncompressedBytesRead), pZipFile->pCache + pZipFile->cacheCursor, bytesToReadFromCache);
        pZipFile->cacheCursor += bytesToReadFromCache;

        uncompressedBytesRead += bytesToReadFromCache;

        /* If we've read the requested number of bytes we can stop. */
        if (uncompressedBytesRead == bytesToRead) {
            break;
        }

        
        /*
        Getting here means we've exchausted the cache but still have more data to read. We now need
        to refill the cache and read from it again.

        This needs to be run in a loop because we may need to read multiple times to get enough input
        data to fill the entire output cache, which must be at least 32KB.
        */
        pZipFile->cacheCursor = 0;
        pZipFile->cacheSize   = 0;

        for (;;) {
            size_t compressedBytesRead;
            size_t compressedBytesToRead;
            int decompressFlags = E_DEFLATE_FLAG_HAS_MORE_INPUT;    /* The default stance is that we have more input available. */
            e_result decompressResult;

            /* If we've already read the entire compressed file we need to set the flag to indicate there is no more input. */
            if (pZipFile->absoluteCursorCompressed == pZipFile->info.compressedSize) {
                decompressFlags &= ~E_DEFLATE_FLAG_HAS_MORE_INPUT;
            }

            /*
            We need only lock while we read the compressed data into our cache. We don't need to keep
            the archive locked while we do the decompression phase.

            We need only read more input data from the stream if we've run out of data in the
            compressed cache.
            */
            if (pZipFile->compressedCacheSize == 0) {
                E_ASSERT(pZipFile->compressedCacheCursor == 0); /* The cursor should never go past the size. */

                e_zip_lock(pZip);
                {
                    /* Make sure we're positioned correctly in the stream before we read. */
                    result = e_stream_seek(e_archive_stream(&pZip->archive), pZipFile->info.fileOffset + pZipFile->absoluteCursorCompressed, E_SEEK_ORIGIN_START);
                    if (result != E_SUCCESS) {
                        e_zip_unlock(pZip);
                        return result;
                    }

                    /*
                    Read the compressed data into the cache. The number of compressed bytes we read needs
                    to be clamped to the number of bytes remaining in the file and the number of bytes
                    remaining in the cache.
                    */
                    compressedBytesToRead = E_MIN(pZipFile->compressedCacheCap - pZipFile->compressedCacheCursor, (pZipFile->info.compressedSize - pZipFile->absoluteCursorCompressed));

                    result = e_stream_read(e_archive_stream(&pZip->archive), pZipFile->pCompressedCache + pZipFile->compressedCacheCursor, compressedBytesToRead, &compressedBytesRead);
                    /*
                    We'll inspect the result later after we've escaped from the locked section just to
                    keep the lock as small as possible.
                    */

                    pZipFile->absoluteCursorCompressed += compressedBytesRead;
                }
                e_zip_unlock(pZip);

                /* If we've reached the end of the compressed data, we need to set a flag which we later pass through to the decompressor. */
                if (result == E_AT_END && compressedBytesRead < compressedBytesToRead) {
                    decompressFlags &= ~E_DEFLATE_FLAG_HAS_MORE_INPUT;
                }

                if (result != E_SUCCESS && result != E_AT_END) {
                    return result;  /* Failed to read the compressed data. */
                }

                pZipFile->compressedCacheSize += compressedBytesRead;
            }


            /*
            At this point we should have the compressed data. Here is where we decompress it into
            the cache. We need to set up a few parameters here. The input buffer needs to start from
            the current cursor position of the compressed cache. The input size is the number of
            bytes in the compressed cache between the cursor and the end of the cache. The output
            buffer is from the current cursor position.
            */
            {
                size_t inputBufferSize = pZipFile->compressedCacheSize - pZipFile->compressedCacheCursor;
                size_t outputBufferSize = pZipFile->cacheCap - pZipFile->cacheSize;

                decompressResult = e_deflate_decompress(&pZipFile->decompressor, pZipFile->pCompressedCache + pZipFile->compressedCacheCursor, &inputBufferSize, pZipFile->pCache, pZipFile->pCache + pZipFile->cacheSize, &outputBufferSize, decompressFlags);
                if (decompressResult < 0) {
                    return E_ERROR; /* Failed to decompress the data. */
                }

                /* Move our input cursors forward since we've just consumed some input. */
                pZipFile->compressedCacheCursor += inputBufferSize;

                /* We've just generated some uncompressed data, so push out the size of the cache to accommodate it. */
                pZipFile->cacheSize += outputBufferSize;

                /*
                If the compressed cache has been fully exhausted we need to reset it so more data
                can be read from the stream.
                */
                if (pZipFile->compressedCacheCursor == pZipFile->compressedCacheSize) {
                    pZipFile->compressedCacheCursor = 0;
                    pZipFile->compressedCacheSize   = 0;
                }

                /*
                We need to inspect the result of the decompression to determine how to continue. If
                we've reached the end we need only break from the inner loop.
                */
                if (decompressResult == E_NEEDS_MORE_INPUT) {
                    continue;   /* Do another round of reading and decompression. */
                } else {
                    break;      /* We've reached the end of the compressed data or the output buffer is full. */
                }
            }
        }
    }

    pZipFile->absoluteCursorUncompressed += uncompressedBytesRead;

    /* We're done. */
    *pBytesRead = uncompressedBytesRead;
    return E_SUCCESS;
}

static e_result e_archive_read_zip(void* pUserData, e_file* pFile, void* pDst, size_t bytesToRead, size_t* pBytesRead)
{
    e_zip_file* pZipFile = (e_zip_file*)pFile;

    E_ASSERT(pZipFile != NULL);
    E_UNUSED(pUserData);

    if (pZipFile->info.compressionMethod == E_ZIP_COMPRESSION_METHOD_STORE) {
        return e_archive_read_zip_store(pZipFile, pDst, bytesToRead, pBytesRead);
    } else if (pZipFile->info.compressionMethod == E_ZIP_COMPRESSION_METHOD_DEFLATE) {
        return e_archive_read_zip_deflate(pZipFile, pDst, bytesToRead, pBytesRead);
    } else {
        return E_INVALID_FILE;  /* Should never get here. */
    }

    return E_SUCCESS;
}

static e_result e_archive_write_zip(void* pUserData, e_file* pFile, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten)
{
    E_UNUSED(pUserData);
    
    /* Writing not supported. */
    (void)pFile;
    (void)pSrc;
    (void)bytesToWrite;
    (void)pBytesWritten;
    return E_NOT_IMPLEMENTED;
}

static e_result e_archive_seek_zip(void* pUserData, e_file* pFile, e_int64 offset, e_seek_origin origin)
{
    e_zip_file* pZipFile = (e_zip_file*)pFile;
    e_uint64 newAbsoluteCursor;

    E_ASSERT(pZipFile != NULL);
    E_UNUSED(pUserData);

    /*
    We can do fast seeking if we are moving within the cache. Otherwise we just move the cursor and
    clear the cache. The next time we read, it'll see that the cache is empty which will trigger a
    fresh read of data from the archive stream.
    */
    if (origin == E_SEEK_ORIGIN_CURRENT) {
        if (offset > 0) {
            newAbsoluteCursor = pZipFile->absoluteCursorUncompressed + (e_uint64)offset;
        } else {
            if ((e_uint64)E_ABS(offset) > pZipFile->absoluteCursorUncompressed) {
                return E_BAD_SEEK;  /* Trying to seek to before the start of the file. */
            }

            newAbsoluteCursor = pZipFile->absoluteCursorUncompressed - E_ABS(offset);
        }
    } else if (origin == E_SEEK_ORIGIN_START) {
        if (offset < 0) {
            return E_BAD_SEEK;  /* Trying to seek to before the start of the file. */
        }

        newAbsoluteCursor = (e_uint64)offset;
    } else if (origin == E_SEEK_ORIGIN_END) {
        if (offset > 0) {
            return E_BAD_SEEK;  /* Trying to seek beyond the end of the file. */
        }

        if ((e_uint64)E_ABS(offset) > pZipFile->absoluteCursorUncompressed) {
            return E_BAD_SEEK;  /* Trying to seek to before the start of the file. */
        }

        newAbsoluteCursor = pZipFile->info.uncompressedSize - E_ABS(offset);
    } else {
        return E_INVALID_ARGS;
    }

    if (newAbsoluteCursor > pZipFile->info.uncompressedSize) {
        return E_BAD_SEEK;  /* Trying to seek beyond the end of the file. */
    }

    if (newAbsoluteCursor > pZipFile->absoluteCursorUncompressed) {
        /* Moving forward. */
        e_uint64 delta = newAbsoluteCursor - pZipFile->absoluteCursorUncompressed;
        if (delta <= (pZipFile->cacheSize - pZipFile->cacheCursor)) {
            pZipFile->cacheCursor += (size_t)delta; /* Safe cast. */
            pZipFile->absoluteCursorUncompressed = newAbsoluteCursor;
            return E_SUCCESS;
        } else {
            /* Seeking beyond the cache. Fall through. */
        }
    } else {
        /* Moving backward. */
        e_uint64 delta = pZipFile->absoluteCursorUncompressed - newAbsoluteCursor;
        if (delta <= pZipFile->cacheCursor) {
            pZipFile->cacheCursor -= (size_t)delta;
            pZipFile->absoluteCursorUncompressed = newAbsoluteCursor;
            return E_SUCCESS;
        } else {
            /* Seeking beyond the cache. Fall through. */
        }
    }

    /* Getting here means we're seeking beyond the cache. Just clear it. The next read will read in fresh data. */
    pZipFile->cacheSize   = 0;
    pZipFile->cacheCursor = 0;

    /*
    Seeking is more complicated for compressed files. We need to actually read to the seek point.
    There is no seek table to accelerate this.
    */
    if (pZipFile->info.compressionMethod != E_ZIP_COMPRESSION_METHOD_STORE) {
        pZipFile->compressedCacheCursor = 0;
        pZipFile->compressedCacheSize   = 0;

        /*
        When seeking backwards we need to move everything back to the start and then just
        read-and-discard until we reach the end.
        */
        if (pZipFile->absoluteCursorUncompressed > newAbsoluteCursor) {
            pZipFile->absoluteCursorUncompressed = 0;
            pZipFile->absoluteCursorCompressed   = 0;
        }

        /* Now we just keep reading until we get to the seek point. */
        while (pZipFile->absoluteCursorUncompressed < newAbsoluteCursor) {
            e_uint8 temp[4096];
            size_t bytesToRead;
            size_t bytesRead;
            e_result result;

            bytesToRead = newAbsoluteCursor - pZipFile->absoluteCursorUncompressed;
            if (bytesToRead > sizeof(temp)) {
                bytesToRead = sizeof(temp);
            }
            
            result = e_archive_read_zip(pUserData, pFile, temp, bytesToRead, &bytesRead);
            if (result != E_SUCCESS) {
                return result;
            }

            pZipFile->absoluteCursorUncompressed += bytesRead;

            if (bytesRead == 0) {
                return E_BAD_SEEK;  /* Trying to seek beyond the end of the file. */
            }
        }
    }

    /* Make sure the absolute cursor is set to the new position. */
    pZipFile->absoluteCursorUncompressed = newAbsoluteCursor;

    return E_SUCCESS;
}

static e_result e_archive_tell_zip(void* pUserData, e_file* pFile, e_int64* pCursor)
{
    e_zip_file* pZipFile = (e_zip_file*)pFile;

    E_ASSERT(pZipFile != NULL);
    E_UNUSED(pUserData);

    *pCursor = pZipFile->absoluteCursorUncompressed;
    return E_SUCCESS;
}

static e_result e_archive_flush_zip(void* pUserData, e_file* pFile)
{
    e_zip_file* pZipFile = (e_zip_file*)pFile;

    E_ASSERT(pZipFile != NULL);
    E_UNUSED(pUserData);
    E_UNUSED(pZipFile);

    /* Nothing to do here. */

    return E_SUCCESS;
}

static e_result e_archive_info_zip(void* pUserData, e_file* pFile, e_file_info* pInfo)
{
    e_zip_file* pZipFile = (e_zip_file*)pFile;

    E_ASSERT(pZipFile != NULL);
    E_UNUSED(pUserData);

    pInfo->size      = pZipFile->info.uncompressedSize;
    pInfo->directory = E_FALSE; /* An opened file should never be a directory. */
    
    return E_SUCCESS;
}


static void e_zip_iterator_init(e_zip* pZip, e_zip_cd_node* pChild, e_zip_iterator* pIterator)
{
    e_zip_file_info info;

    E_ASSERT(pIterator != NULL);
    E_ASSERT(pChild    != NULL);
    E_ASSERT(pZip      != NULL);

    pIterator->pZip = pZip;

    /* Name. */
    c89str_strncpy_s((char*)pIterator + sizeof(*pIterator), pChild->nameLen + 1, pChild->pName, pChild->nameLen);
    pIterator->iterator.pName   = (const char*)pIterator + sizeof(*pIterator);
    pIterator->iterator.nameLen = pChild->nameLen;

    /* Info. */
    E_ZERO_OBJECT(&pIterator->iterator.info);

    if (pChild->childCount > 0) {
        /* The node has children. Must be a directory. */
        pIterator->iterator.info.directory = E_TRUE;
    } else {
        /* The node does not have children. Could still be a directory. */
        e_zip_get_file_info_by_record_offset(pZip, pZip->pIndex[pChild->iFile].offsetInBytes, &info);
        pIterator->iterator.info.directory = info.directory;

        if (!pIterator->iterator.info.directory) {
            pIterator->iterator.info.size = info.uncompressedSize;
        }
    }
}

static e_fs_iterator* e_archive_first_zip(void* pUserData, e_fs* pFS, const char* pDirectoryPath, size_t directoryPathLen, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_zip* pZip = (e_zip*)pFS;
    e_zip_iterator* pIterator;
    c89str_path_iterator directoryPathIterator;
    e_zip_cd_node* pCurrentNode;

    E_UNUSED(pUserData);

    if (pZip == NULL) {
        return NULL;
    }

    if (pDirectoryPath == NULL) {
        pDirectoryPath = "";
    }

    /* Skip past any leading slash. */
    if (pDirectoryPath[0] == '/' || pDirectoryPath[0] == '\\') {
        pDirectoryPath += 1;
    }


    /* Always start from the root node. */
    pCurrentNode = pZip->pCDRootNode;

    /*
    All we need to do is find the node the corresponds to the specified directory path. To do this
    we just iterate over each segment in the path and get the children one after the other.
    */
    if (e_result_from_errno(c89str_path_first(pDirectoryPath, directoryPathLen, &directoryPathIterator)) == E_SUCCESS) {
        for (;;) {
            /* Try finding the child node. If this cannot be found, the directory does not exist. */
            e_zip_cd_node* pChildNode;

            pChildNode = e_zip_cd_node_find_child(pCurrentNode, directoryPathIterator.pFullPath + directoryPathIterator.segmentOffset, directoryPathIterator.segmentLength);
            if (pChildNode == NULL) {
                return NULL;    /* Does not exist. */
            }

            pCurrentNode = pChildNode;
            
            /* Go to the next segment if we have one. */
            if (e_result_from_errno(c89str_path_next(&directoryPathIterator)) != E_SUCCESS) {
                break;  /* Nothing left in the directory path. We have what we're looking for. */
            }
        }
    } else {
        /*
        We failed to initialize the path iterator which can only mean we were given an empty path
        in which case it should be treated as the root directory. The node will already be set to
        the root node at this point so there's nothing more do to.
        */
        E_ASSERT(pCurrentNode == pZip->pCDRootNode);
    }

    /* If the current node does not have any children, there is no first item and therefore nothing to return. */
    if (pCurrentNode->childCount == 0) {
        return NULL;
    }

    /*
    Now that we've found the node we have enough information to allocate the iterator. We allocate
    room for a copy of the name so we can null terminate it.
    */
    pIterator = (e_zip_iterator*)e_realloc(NULL, sizeof(*pIterator) + pCurrentNode->pChildren[0].nameLen + 1, pAllocationCallbacks);
    if (pIterator == NULL) {
        return NULL;
    }

    e_zip_iterator_init(pZip, &pCurrentNode->pChildren[0], pIterator);

    /* Internal variables for iteration. */
    pIterator->pDirectoryNode = pCurrentNode;
    pIterator->iChild         = 0;

    return (e_fs_iterator*)pIterator;
}

static e_fs_iterator* e_archive_next_zip(void* pUserData, e_fs_iterator* pIterator, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_zip_iterator* pIteratorZip = (e_zip_iterator*)pIterator;
    e_zip_iterator* pNewIteratorZip;
    

    E_UNUSED(pUserData);

    if (pIteratorZip == NULL) {
        return NULL;
    }

    /* All we're doing is going to the next child. If there's nothing left we just free the iterator and return null. */
    pIteratorZip->iChild += 1;
    if (pIteratorZip->iChild >= pIteratorZip->pDirectoryNode->childCount) {
        e_free(pIteratorZip, pAllocationCallbacks);
        return NULL;    /* Nothing left. */
    }

    /* Getting here means there's another child to iterate. */
    pNewIteratorZip = (e_zip_iterator*)e_realloc(pIteratorZip, sizeof(*pIteratorZip) + pIteratorZip->pDirectoryNode->pChildren[pIteratorZip->iChild].nameLen + 1, pAllocationCallbacks);
    if (pNewIteratorZip == NULL) {
        e_free(pIteratorZip, pAllocationCallbacks);
        return NULL;    /* Out of memory. */
    }

    e_zip_iterator_init(pNewIteratorZip->pZip, &pNewIteratorZip->pDirectoryNode->pChildren[pNewIteratorZip->iChild], pNewIteratorZip);

    return (e_fs_iterator*)pNewIteratorZip;
}

static void e_archive_free_iterator_zip(void* pUserData, e_fs_iterator* pIterator, const e_allocation_callbacks* pAllocationCallbacks)
{
    E_UNUSED(pUserData);
    e_free(pIterator, pAllocationCallbacks);
}

static e_archive_vtable e_gArchiveVTableZip =
{
    /* e_fs */
    {
        e_archive_file_alloc_size_zip,
        e_archive_open_zip,
        e_archive_close_zip,
        e_archive_read_zip,
        e_archive_write_zip,
        e_archive_seek_zip,
        e_archive_tell_zip,
        e_archive_flush_zip,
        e_archive_info_zip,
        e_archive_first_zip,
        e_archive_next_zip,
        e_archive_free_iterator_zip
    },

    /* e_archive */
    e_archive_alloc_size_zip,
    e_archive_init_zip,
    e_archive_uninit_zip
};

E_API e_archive_vtable* e_zip_vtable()
{
    return &e_gArchiveVTableZip;
}

E_API e_result e_zip_init(e_stream* pStream, const e_allocation_callbacks* pAllocationCallbacks, e_zip** ppZip)
{
    return e_archive_init(&e_gArchiveVTableZip, NULL, pStream, pAllocationCallbacks, (e_archive**)ppZip);
}

E_API e_result e_zip_init_from_file(e_fs* pFS, const char* pFilePath, const e_allocation_callbacks* pAllocationCallbacks, e_zip** ppZip)
{
    return e_archive_init_from_file(&e_gArchiveVTableZip, NULL, pFS, pFilePath, pAllocationCallbacks, (e_archive**)ppZip);
}

E_API void e_zip_uninit(e_zip* pZip, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_archive_uninit((e_archive*)pZip, pAllocationCallbacks);
}

E_API e_result e_zip_open(e_zip* pZip, const char* pFilePath, int openMode, const e_allocation_callbacks* pAllocationCallbacks, e_file** ppFile)
{
    return e_archive_open((e_archive*)pZip, pFilePath, openMode, pAllocationCallbacks, ppFile);
}

E_API void e_zip_close(e_file* pFile, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_archive_close(pFile, pAllocationCallbacks);
}

E_API e_result e_zip_read(e_file* pFile, void* pDst, size_t bytesToRead, size_t* pBytesRead)
{
    return e_archive_read(pFile, pDst, bytesToRead, pBytesRead);
}

E_API e_result e_zip_write(e_file* pFile, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten)
{
    return e_archive_write(pFile, pSrc, bytesToWrite, pBytesWritten);
}

E_API e_result e_zip_seek(e_file* pFile, e_int64 offset, e_seek_origin origin)
{
    return e_archive_seek(pFile, offset, origin);
}

E_API e_result e_zip_tell(e_file* pFile, e_int64* pCursor)
{
    return e_archive_tell(pFile, pCursor);
}

E_API e_result e_zip_flush(e_file* pFile)
{
    return e_archive_flush(pFile);
}

E_API e_result e_zip_info(e_file* pFile, e_file_info* pInfo)
{
    return e_archive_info(pFile, pInfo);
}

E_API e_fs_iterator* e_zip_first(e_zip* pZip, const char* pDirectoryPath, size_t directoryPathLen, const e_allocation_callbacks* pAllocationCallbacks)
{
    return e_archive_first((e_archive*)pZip, pDirectoryPath, directoryPathLen, pAllocationCallbacks);
}

E_API e_fs_iterator* e_zip_next(e_fs_iterator* pIterator, const e_allocation_callbacks* pAllocationCallbacks)
{
    return e_archive_next(pIterator, pAllocationCallbacks);
}

E_API void e_zip_free_iterator(e_fs_iterator* pIterator, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_archive_free_iterator(pIterator, pAllocationCallbacks);
}
/* ==== END e_zip.c ==== */



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



/* ==== BEG e_script.h ==== */
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

static lua_State* e_script_to_lua(e_script* pScript)
{
    return (lua_State*)pScript;
}


E_API e_result e_script_init(const e_allocation_callbacks* pAllocationCallbacks, e_script** ppScript)
{
    lua_State* pLuaState;

    if (ppScript == NULL) {
        return E_INVALID_ARGS;
    }

    *ppScript = NULL;

    pLuaState = lua_newstate(e_lua_alloc, (void*)pAllocationCallbacks);
    if (pLuaState == NULL) {
        return E_OUT_OF_MEMORY;
    }

    *ppScript = pLuaState;
    return E_SUCCESS;
}

E_API void e_script_uninit(e_script* pScript, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pScript == NULL) {
        return;
    }

    /*
    Lua stores a persistent copy of the allocation callback that we specified at initialization
    time. Therefore there's no need to actually do anything with our allocation callbacks.
    */
    E_UNUSED(pAllocationCallbacks);

    lua_close(e_script_to_lua(pScript));
}

E_API e_result e_script_load(e_script* pScript, e_stream* pStream, const char* pName, e_log* pLog)
{
    e_result result;
    e_lua_read_state readState;

    if (pScript == NULL || pStream == NULL) {
        return E_INVALID_ARGS;
    }

    readState.pStream = pStream;

    result = e_result_from_lua(lua_load(e_script_to_lua(pScript), e_lua_read, &readState, pName, NULL));
    if (result == E_SUCCESS) {
        result = e_result_from_lua(lua_pcall(e_script_to_lua(pScript), 0, LUA_MULTRET, 0));
    }

    if (result != E_SUCCESS) {
        const char* pErrorString = lua_tostring(e_script_to_lua(pScript), lua_gettop(e_script_to_lua(pScript)));
        if (pErrorString != NULL) {
            e_log_postf(pLog, E_LOG_LEVEL_ERROR, "Error loading config \"%s\": %s", pName, pErrorString);
        }

        return result;
    }

    return E_SUCCESS;
}

E_API e_result e_script_load_file(e_script* pScript, e_fs* pFS, const char* pFilePath, const e_allocation_callbacks* pAllocationCallbacks, e_log* pLog)
{
    e_result result;
    e_file* pFile;

    result = e_fs_open(pFS, pFilePath, E_OPEN_MODE_READ, pAllocationCallbacks, &pFile);
    if (result != E_SUCCESS) {
        return result;
    }

    result = e_script_load(pScript, e_fs_file_stream(pFile), pFilePath, pLog);
    e_fs_close(pFile, pAllocationCallbacks);

    return result;
}
/* ==== END e_script.h ==== */



/* ==== BEG e_config_file.c ==== */
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
        e_log_postf(pLog, E_LOG_LEVEL_ERROR, "Could not open \"%s\". %s.", pFilePath, e_result_description(result));
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
            const char* pSegmentEnd;
            
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

static e_result e_config_file_get_lua_Integer(e_config_file* pConfigFile, const char* pSection, const char* pName, lua_Integer* pValue)
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
        *pValue = value;
    }

    return result;
}

E_API e_result e_config_file_get_int(e_config_file* pConfigFile, const char* pSection, const char* pName, int* pValue)
{
    e_result result = E_SUCCESS;
    lua_Integer value;

    if (pValue != NULL) {
        *pValue = 0;
    }

    result = e_config_file_get_lua_Integer(pConfigFile, pSection, pName, &value);
    if (result != E_SUCCESS) {
        return result;
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

E_API e_result e_config_file_get_int64(e_config_file* pConfigFile, const char* pSection, const char* pName, e_int64* pValue)
{
    e_result result = E_SUCCESS;
    lua_Integer value;

    if (pValue != NULL) {
        *pValue = 0;
    }

    result = e_config_file_get_lua_Integer(pConfigFile, pSection, pName, &value);
    if (result != E_SUCCESS) {
        return result;
    }

    if (pValue != NULL) {
        *pValue = (e_int64)value;
    }

    return result;
}

E_API e_result e_config_file_get_uint64(e_config_file* pConfigFile, const char* pSection, const char* pName, e_uint64* pValue)
{
    e_result result;
    e_int64 value;

    if (pValue != NULL) {
        *pValue = 0;
    }

    result = e_config_file_get_int64(pConfigFile, pSection, pName, &value);
    if (result != E_SUCCESS) {
        return result;
    }

    if (pValue != NULL) {
        *pValue = (e_uint64)value;
    }

    return E_SUCCESS;

}
/* ==== END e_config_file.c ==== */



/* ==== BEG e_image.h ==== */
#ifndef E_NO_STB_IMAGE
typedef struct
{
    e_stream* pStream;
    e_bool32 atEnd;
} e_stb_image_callback_data;

static int e_stb_image_read(void* pUserData, char* pData, int size)
{
    e_stb_image_callback_data* pCallbackData = (e_stb_image_callback_data*)pUserData;
    e_result result;
    size_t bytesRead;

    result = e_stream_read(pCallbackData->pStream, pData, size, &bytesRead);
    if (result != E_SUCCESS || bytesRead < (size_t)size) {
        pCallbackData->atEnd = E_TRUE;
    } else {
        pCallbackData->atEnd = E_FALSE;
    }

    return (int)bytesRead;
}

static void e_stb_image_skip(void* pUserData, int n)
{
    e_stream_seek(((e_stb_image_callback_data*)pUserData)->pStream, n, E_SEEK_ORIGIN_CURRENT);
}

static int e_stb_image_eof(void* pUserData)
{
    return ((e_stb_image_callback_data*)pUserData)->atEnd;
}

static stbi_io_callbacks e_stb_image_callbacks =
{
    e_stb_image_read,
    e_stb_image_skip,
    e_stb_image_eof
};

static e_result e_load_image_stb(void* pUserData, e_stream* pStream, const e_allocation_callbacks* pAllocationCallbacks, void** ppData, e_uint32* pSizeX, e_uint32* pSizeY, e_format* pFormat)
{
    int x, y, n;
    unsigned char* pData;
    e_stb_image_callback_data callbacksData;

    E_UNUSED(pUserData);
    E_UNUSED(pAllocationCallbacks); /* Don't know how to use allocation callbacks with stb_image. */

    callbacksData.pStream = pStream;
    callbacksData.atEnd   = E_FALSE;

    pData = stbi_load_from_callbacks(&e_stb_image_callbacks, &callbacksData, &x, &y, &n, 0);
    if (pData == NULL) {
        return E_ERROR;
    }

    *ppData = pData;
    *pSizeX = (e_uint32)x;
    *pSizeY = (e_uint32)y;

    if (n == 1) {
        *pFormat = E_FORMAT_R8_UNORM;
    } else if (n == 2) {
        *pFormat = E_FORMAT_R8G8_UNORM;
    } else if (n == 3) {
        *pFormat = E_FORMAT_R8G8B8_UNORM;
    } else if (n == 4) {
        *pFormat = E_FORMAT_R8G8B8A8_UNORM;
    } else {
        *pFormat = E_FORMAT_UNKNOWN;
    }

    return E_SUCCESS;
}

static e_image_loader_vtable e_image_loader_stb_vtable =
{
    e_load_image_stb
};
#endif

E_API e_result e_load_image(e_image_loader_vtable* pVTable, void* pUserData, e_stream* pStream, const e_allocation_callbacks* pAllocationCallbacks, void** ppData, e_uint32* pSizeX, e_uint32* pSizeY, e_format* pFormat)
{
    if (ppData != NULL) {
        *ppData = NULL;
    }

    if (pSizeX != NULL) {
        *pSizeX = 0;
    }
    if (pSizeY != NULL) {
        *pSizeY = 0;
    }

    if (pFormat != NULL) {
        *pFormat = E_FORMAT_UNKNOWN;
    }

    if (pStream == NULL || ppData == NULL || pSizeX == NULL || pSizeY == NULL || pFormat == NULL) {
        return E_INVALID_ARGS;
    }

    if (pVTable == NULL) {
        #ifndef E_NO_STB_IMAGE
        {
            pVTable = &e_image_loader_stb_vtable;
        }
        #else
        {
            return E_INVALID_ARGS;
        }
        #endif
    }

    if (pVTable->load == NULL) {
        return E_INVALID_ARGS;
    }

    return pVTable->load(pUserData, pStream, pAllocationCallbacks, ppData, pSizeX, pSizeY, pFormat);
}

E_API e_result e_load_image_from_file(e_image_loader_vtable* pVTable, void* pUserData, e_fs* pFS, const char* pFilePath, const e_allocation_callbacks* pAllocationCallbacks, void** ppData, e_uint32* pSizeX, e_uint32* pSizeY, e_format* pFormat)
{
    e_result result;
    e_file* pFile;

    if (ppData != NULL) {
        *ppData = NULL;
    }

    if (pSizeX != NULL) {
        *pSizeX = 0;
    }
    if (pSizeY != NULL) {
        *pSizeY = 0;
    }

    if (pFormat != NULL) {
        *pFormat = E_FORMAT_UNKNOWN;
    }

    result = e_fs_open(pFS, pFilePath, E_OPEN_MODE_READ, pAllocationCallbacks, &pFile);
    if (result != E_SUCCESS) {
        return result;
    }

    result = e_load_image(pVTable, pUserData, &pFile->stream, pAllocationCallbacks, ppData, pSizeX, pSizeY, pFormat);
    e_fs_close(pFile, pAllocationCallbacks);

    if (result != E_SUCCESS) {
        return result;
    }

    return E_SUCCESS;
}
/* ==== END e_image.h ==== */



/* ==== BEG e_font.c ==== */
E_API e_font_config e_font_config_init()
{
    e_font_config config;

    E_ZERO_OBJECT(&config);

    return config;
}

E_API e_font_config e_font_config_init_file(e_fs* pFS, const char* pFilePath)
{
    e_font_config config;

    config = e_font_config_init();
    config.pFS = pFS;
    config.pFilePath = pFilePath;

    return config;
}


struct e_font
{
    unsigned char* pTTFData; /* Will only be set if the font was loaded from a file. This is the raw data of the TTF file. Will point just past the end of the struct. Do not free this separately. */
    size_t ttfDataSize;
    stbtt_fontinfo fontInfo;
};


static e_result e_font_init_log(const e_font_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_font** ppFont)
{
    E_ASSERT(ppFont  != NULL);
    E_ASSERT(pConfig != NULL);

    /* Not yet implemented. */
    (void)ppFont;
    (void)pAllocationCallbacks;
    (void)pConfig;
    return E_ERROR;
}

static e_result e_font_init_file(const e_font_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_font** ppFont)
{
    e_font* pFont = NULL;
    e_result result;
    e_file* pFile;
    e_file_info fileInfo;

    E_ASSERT(ppFont  != NULL);
    E_ASSERT(pConfig != NULL);

    /* stb_truetype requires us to provide a buffer containing the raw data of the file. */
    result = e_fs_open(pConfig->pFS, pConfig->pFilePath, E_OPEN_MODE_READ, pAllocationCallbacks, &pFile);
    if (result != E_SUCCESS) {
        e_log_postf(pConfig->pLog, E_LOG_LEVEL_ERROR, "Failed to open font file '%s'. %s.", pConfig->pFilePath, e_result_description(result));
        return result;
    }

    result = e_fs_info(pFile, &fileInfo);
    if (result != E_SUCCESS) {
        e_log_postf(pConfig->pLog, E_LOG_LEVEL_ERROR, "Failed to get info for font file '%s'. %s.", pConfig->pFilePath, e_result_description(result));
        e_fs_close(pFile, pAllocationCallbacks);
        return result;
    }

    pFont = (e_font*)e_malloc(sizeof(*pFont) + fileInfo.size, pAllocationCallbacks);
    if (pFont == NULL) {
        e_log_postf(pConfig->pLog, E_LOG_LEVEL_ERROR, "Failed to allocate memory for font file '%s'.", pConfig->pFilePath);
        e_fs_close(pFile, pAllocationCallbacks);
        return E_OUT_OF_MEMORY;
    }

    /* The font data is sitting at the end of the font structure. */
    pFont->pTTFData    = (unsigned char*)(pFont + 1);
    pFont->ttfDataSize = fileInfo.size;

    result = e_fs_read(pFile, pFont->pTTFData, fileInfo.size, NULL);
    if (result != E_SUCCESS) {
        e_log_postf(pConfig->pLog, E_LOG_LEVEL_ERROR, "Failed to read font file '%s'. %s.", pConfig->pFilePath, e_result_description(result));
        e_fs_close(pFile, pAllocationCallbacks);
        e_free(pFont, pAllocationCallbacks);
        return result;
    }

    /* Now that we've got the data in memory we can close the file. */
    e_fs_close(pFile, pAllocationCallbacks);

    /* At this point we have enough information to load the file via stb_truetype. */
    if (stbtt_InitFont(&pFont->fontInfo, pFont->pTTFData, 0) == 0) {
        e_log_postf(pConfig->pLog, E_LOG_LEVEL_ERROR, "Failed to load font file '%s'.", pConfig->pFilePath);
        e_free(pFont, pAllocationCallbacks);
        return E_ERROR;
    }

    /* Done. */
    *ppFont = pFont;
    return E_SUCCESS;
}

E_API e_result e_font_init(const e_font_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_font** ppFont)
{
    if (ppFont == NULL) {
        return E_INVALID_ARGS;
    }

    *ppFont = NULL;

    if (pConfig == NULL) {
        return E_INVALID_ARGS;
    }

    if (pConfig->pFilePath != NULL) {
        return e_font_init_file(pConfig, pAllocationCallbacks, ppFont);
    } else {
        return e_font_init_log(pConfig, pAllocationCallbacks, ppFont);
    }
}

E_API void e_font_uninit(e_font* pFont, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pFont == NULL) {
        return;
    }

    e_free(pFont, pAllocationCallbacks);
}

E_API e_uint32 e_font_get_glyph_index(e_font* pFont, e_uint32 codePoint)
{
    if (pFont == NULL) {
        return 0;
    }

    return (e_uint32)stbtt_FindGlyphIndex(&pFont->fontInfo, codePoint);
}

E_API float e_font_get_scale(e_font* pFont, float pixelHeight)
{
    if (pFont == NULL) {
        return 0;
    }

    return stbtt_ScaleForPixelHeight(&pFont->fontInfo, pixelHeight);
}

E_API void e_font_get_metrics(e_font* pFont, float scale, e_font_metrics* pMetrics)
{
    int ascent;
    int descent;
    int lineGap;

    if (pMetrics == NULL) {
        return;
    }

    E_ZERO_OBJECT(&pMetrics);

    if (pFont == NULL) {
        return;
    }

    stbtt_GetFontVMetrics(&pFont->fontInfo, &ascent, &descent, &lineGap);

    /* TODO: Check the rounding rules for this. */
    pMetrics->ascent  = (e_int32)(ascent * scale);
    pMetrics->descent = (e_int32)(descent * scale);
    pMetrics->lineGap = (e_int32)(lineGap * scale);
}

E_API void e_font_get_glyph_metrics(e_font* pFont, float scale, e_uint32 glyphIndex, e_glyph_metrics* pMetrics)
{
    int advanceWidth;
    int leftSideBearing;
    int x0, y0, x1, y1;

    if (pMetrics == NULL) {
        return;
    }

    E_ZERO_OBJECT(&pMetrics);

    if (pFont == NULL) {
        return;
    }

    stbtt_GetGlyphHMetrics(&pFont->fontInfo, glyphIndex, &advanceWidth, &leftSideBearing);
    stbtt_GetGlyphBitmapBox(&pFont->fontInfo, glyphIndex, scale, scale, &x0, &y0, &x1, &y1);

    /* TODO: Check scaling rounding rules. */
    pMetrics->sizeX    = (e_int32)(x1 - x0);
    pMetrics->sizeY    = (e_int32)(y1 - y0);
    pMetrics->bearingX = (e_int32)(leftSideBearing * scale);
    pMetrics->bearingY = 0;
    pMetrics->advanceX = (e_int32)(advanceWidth * scale);
    pMetrics->advanceY = 0;
}

E_API e_int32 e_font_get_kerning(e_font* pFont, float scale, e_uint32 glyphIndex1, e_uint32 glyphIndex2)
{
    if (pFont == NULL) {
        return 0;
    }

    /* TODO: Check scaling rounding rules. */
    return (e_int32)(stbtt_GetGlyphKernAdvance(&pFont->fontInfo, glyphIndex1, glyphIndex2) * scale);
}

E_API void e_font_get_glyph_bitmap(e_font* pFont, float scale, e_uint32 glyphIndex, e_uint32 bitmapSizeX, e_uint32 bitmapSizeY, e_uint32 stride, e_uint8* pBitmap)
{
    if (pFont == NULL || pBitmap == NULL) {
        return;
    }

    stbtt_MakeGlyphBitmap(&pFont->fontInfo, pBitmap, (int)bitmapSizeX, (int)bitmapSizeY, (int)stride, scale, scale, glyphIndex);
}
/* ==== END e_font.c ==== */


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



E_API e_engine_config e_engine_config_init(int argc, const char** argv, unsigned int flags, e_engine_vtable* pVTable, void* pVTableUserData)
{
    e_engine_config config;

    E_ZERO_OBJECT(&config);
    config.argc            = argc;
    config.argv            = argv;
    config.flags           = flags;
    config.pVTable         = pVTable;
    config.pVTableUserData = pVTableUserData;
    config.pConfigFilePath = E_DEFAULT_CONFIG_FILE_PATH;

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
    result = e_config_file_load_file(&pEngine->configFile, &pEngine->fs, pConfig->pConfigFilePath, pAllocationCallbacks, pEngine->pLog);
    if (result != E_SUCCESS) {
        e_log_postf(pEngine->pLog, E_LOG_LEVEL_WARNING, "Failed to load default config file '%s'.", pConfig->pConfigFilePath);
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

    /* Initialize our networking sub-system. This is only required for Windows, but we'll do it generically for all platforms. */
    if ((flags & E_ENGINE_FLAG_NO_NETWORK) == 0) {
        result = e_net_init();
        if (result != E_SUCCESS) {
            e_log_postf(pLog, E_LOG_LEVEL_WARNING, "Networking sub-system failed to initialize. Networking may be unavailable.");
        }
    }

    /* Timer. */
    e_timer_init(&pEngine->timer);
    pEngine->lastTimeInSeconds = e_timer_get_time_in_seconds(&pEngine->timer);

    *ppEngine = pEngine;
    return E_SUCCESS;
}

E_API void e_engine_uninit(e_engine* pEngine, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pEngine == NULL) {
        return;
    }

    e_net_uninit();

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
    double currentTimeInSeconds = 0;
    double dt = 0;
    e_engine* pEngine = (e_engine*)pUserData;

    E_ASSERT(pEngine != NULL);

    if (pEngine->pVTable == NULL || pEngine->pVTable->onStep == NULL) {
        return E_INVALID_OPERATION;
    }

    currentTimeInSeconds = e_timer_get_time_in_seconds(&pEngine->timer);
    dt = currentTimeInSeconds - pEngine->lastTimeInSeconds;
    pEngine->lastTimeInSeconds = currentTimeInSeconds;

    return pEngine->pVTable->onStep(pEngine->pVTableUserData, pEngine, dt);
}

E_API e_result e_engine_run(e_engine* pEngine)
{
    e_result result;
    int exitCode;

    if (pEngine == NULL) {
        return E_INVALID_ARGS;
    }

    e_engine_reset_timer(pEngine);

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

E_API e_result e_engine_set_blocking_mode(e_engine* pEngine, e_bool32 blocking)
{
    if (pEngine == NULL) {
        return E_INVALID_ARGS;
    }

    return e_platform_set_main_loop_blocking(blocking);

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

E_API void e_engine_reset_timer(e_engine* pEngine)
{
    if (pEngine == NULL) {
        return;
    }

    pEngine->lastTimeInSeconds = e_timer_get_time_in_seconds(&pEngine->timer);
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
E_API unsigned int e_window_buffer_stride(const e_window_buffer* pBuffer)
{
    if (pBuffer == NULL) {
        return 0;
    }

    return e_round_up_to_nearest_16(pBuffer->sizeX * 4);
}


E_API e_window_config e_window_config_init(e_engine* pEngine, const char* pTitle, int posX, int posY, unsigned int sizeX, unsigned int sizeY, unsigned int flags, e_window_vtable* pVTable, void* pVTableUserData)
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
    config.pVTableUserData = pVTableUserData;

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

E_API e_result e_window_default_event_handler(e_window* pWindow, e_event* pEvent)
{
    /* NOTE: This function must never return E_NOT_IMPLEMENTED. */

    (void)pWindow;
    (void)pEvent;

    return E_SUCCESS;
}

E_API e_result e_window_capture_cursor(e_window* pWindow)
{
    if (pWindow == NULL) {
        return E_INVALID_ARGS;
    }

    return e_platform_window_capture_cursor(pWindow->pPlatformWindow);
}

E_API e_result e_window_release_cursor(e_window* pWindow)
{
    if (pWindow == NULL) {
        return E_INVALID_ARGS;
    }

    return e_platform_window_release_cursor(pWindow->pPlatformWindow);
}

E_API e_result e_window_set_cursor_position(e_window* pWindow, int cursorPosX, int cursorPosY)
{
    if (pWindow == NULL) {
        return E_INVALID_ARGS;
    }

    return e_platform_window_set_cursor_position(pWindow->pPlatformWindow, cursorPosX, cursorPosY);
}

E_API e_result e_window_get_cursor_position(e_window* pWindow, int* pCursorPosX, int* pCursorPosY)
{
    if (pWindow == NULL) {
        return E_INVALID_ARGS;
    }

    return e_platform_window_get_cursor_position(pWindow->pPlatformWindow, pCursorPosX, pCursorPosY);
}

E_API e_result e_window_show_cursor(e_window* pWindow)
{
    if (pWindow == NULL) {
        return E_INVALID_ARGS;
    }

    return e_platform_window_show_cursor(pWindow->pPlatformWindow);
}

E_API e_result e_window_hide_cursor(e_window* pWindow)
{
    if (pWindow == NULL) {
        return E_INVALID_ARGS;
    }

    return e_platform_window_hide_cursor(pWindow->pPlatformWindow);
}

E_API e_result e_window_pin_cursor(e_window* pWindow, int cursorPosX, int cursorPosY)
{
    if (pWindow == NULL) {
        return E_INVALID_ARGS;
    }

    return e_platform_window_pin_cursor(pWindow->pPlatformWindow, cursorPosX, cursorPosY);
}

E_API e_result e_window_unpin_cursor(e_window* pWindow)
{
    if (pWindow == NULL) {
        return E_INVALID_ARGS;
    }

    return e_platform_window_unpin_cursor(pWindow->pPlatformWindow);
}

E_API e_result e_window_post_close_event(e_window* pWindow)
{
    if (pWindow == NULL) {
        return E_INVALID_ARGS;
    }

    return e_platform_window_post_close_event(pWindow->pPlatformWindow);
}

E_API e_result e_window_next_buffer(e_window* pWindow, unsigned int bufferSizeX, unsigned int bufferSizeY, e_window_buffer* pBuffer)
{
    if (pBuffer == NULL) {
        return E_INVALID_ARGS;
    }

    E_ZERO_OBJECT(pBuffer);

    if (pWindow == NULL) {
        return E_INVALID_ARGS;
    }

    return e_platform_window_next_buffer(pWindow->pPlatformWindow, bufferSizeX, bufferSizeY, pBuffer);
}
/* ==== END e_window.c ==== */




/* === BEG e_input.c === */
E_API e_input_config e_input_config_init(void)
{
    e_input_config config;

    E_ZERO_OBJECT(&config);

    return config;
}


typedef struct
{
   size_t size;
} e_input_alloc_layout;

E_API e_result e_input_get_alloc_layout(const e_input_config* pConfig, e_input_alloc_layout* pLayout)
{
    E_ASSERT(pConfig != NULL);
    E_ASSERT(pLayout != NULL);

    pLayout->size = E_ALIGN_64(sizeof(e_input));

    return E_SUCCESS;
}

E_API e_result e_input_alloc_size(const e_input_config* pConfig, size_t* pSize)
{
    e_result result;
    e_input_alloc_layout layout;

    E_ASSERT(pSize   != NULL);
    E_ASSERT(pConfig != NULL);

    result = e_input_get_alloc_layout(pConfig, &layout);
    if (result != E_SUCCESS) {
        return result;
    }

    *pSize = layout.size;
    return E_SUCCESS;
}

E_API e_result e_input_init_preallocated(const e_input_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_input* pInput)
{
    e_result result;
    e_input_alloc_layout layout;

    E_UNUSED(pAllocationCallbacks);

    if (pInput == NULL) {
       return E_INVALID_ARGS;
    }

    result = e_input_get_alloc_layout(pConfig, &layout);
    if (result != E_SUCCESS) {
        return result;
    }

    E_ASSERT(layout.size >= sizeof(e_input));

    E_ZERO_MEMORY(pInput, layout.size);

    pInput->prevAbsoluteCursorPosX    = MAX_INT;
    pInput->prevAbsoluteCursorPosY    = MAX_INT;
    pInput->currentAbsoluteCursorPosX = MAX_INT;
    pInput->currentAbsoluteCursorPosY = MAX_INT;

    return E_SUCCESS;
}

E_API e_result e_input_init(const e_input_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_input** ppInput)
{
    e_result result;
    size_t allocSize;
    e_input* pInput;
    e_input_config defaultConfig;

    if (ppInput == NULL) {
        return E_INVALID_ARGS;
    }

    *ppInput = NULL;

    if (pConfig == NULL) {
        defaultConfig = e_input_config_init();
        pConfig = &defaultConfig;
    }

    result = e_input_alloc_size(pConfig, &allocSize);
    if (result != E_SUCCESS) {
        return result;
    }

    pInput = (e_input*)e_malloc(allocSize, pAllocationCallbacks);
    if (pInput == NULL) {
        return E_OUT_OF_MEMORY;
    }

    result = e_input_init_preallocated(pConfig, pAllocationCallbacks, pInput);
    if (result != E_SUCCESS) {
        e_free(pInput, pAllocationCallbacks);
        return result;
    }

    pInput->freeOnUninit = E_TRUE;

    *ppInput = pInput;
    return E_SUCCESS;
}

E_API void e_input_uninit(e_input* pInput, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pInput == NULL) {
        return;
    }

    if (pInput->freeOnUninit) {
        e_free(pInput, pAllocationCallbacks);
    }
}

E_API e_result e_input_step(e_input* pInput)
{
    /* This function normalizes the input data so that the next frame can get accurate input data for things like cursor deltas and whether or not the cursor have moved. */
    if (pInput == NULL) {
        return E_INVALID_ARGS;
    }

    pInput->prevAbsoluteCursorPosX = pInput->currentAbsoluteCursorPosX;
    pInput->prevAbsoluteCursorPosY = pInput->currentAbsoluteCursorPosY;

    /* Set the previous mouse button states. */
    memcpy(pInput->prevCursorButtonStates, pInput->cursorButtonStates, sizeof(pInput->cursorButtonStates));

    /* Reset mouse wheel deltas. */
    pInput->cursorWheelDelta = 0;


    /* Keyboard input needs to be reset. */
    
    /* Copy over the previous keys down, but don't clear the current list. They'll be cleared out when the keys are released. */
    memcpy(pInput->prevKeysDown, pInput->keysDown, sizeof(pInput->keysDown));
    pInput->prevKeysDownCount = pInput->keysDownCount;

    memset(pInput->keyPressedStates, 0, sizeof(pInput->keyPressedStates));
    pInput->keyPressedStateCount  = 0;
    pInput->characterCount = 0;

    return E_SUCCESS;
}

E_API e_result e_input_add_cursor_delta_position(e_input* pInput, int deltaX, int deltaY)
{
    if (pInput == NULL) {
        return E_INVALID_ARGS;
    }

    pInput->currentAbsoluteCursorPosX += deltaX;
    pInput->currentAbsoluteCursorPosY += deltaY;

    return E_SUCCESS;
}

E_API e_result e_input_set_absolute_cursor_position(e_input* pInput, int posX, int posY)
{
    if (pInput == NULL) {
        return E_INVALID_ARGS;
    }

    pInput->currentAbsoluteCursorPosX = posX;
    pInput->currentAbsoluteCursorPosY = posY;

    return E_SUCCESS;
}

E_API e_result e_input_get_absolute_cursor_position(e_input* pInput, int* pPosX, int* pPosY)
{
    if (pPosX != NULL) {
        *pPosX = 0;
    }
    if (pPosY != NULL) {
        *pPosY = 0;
    }

    if (pInput == NULL || pPosX == NULL || pPosY == NULL) {
        return E_INVALID_ARGS;
    }

    *pPosX = pInput->currentAbsoluteCursorPosX;
    *pPosY = pInput->currentAbsoluteCursorPosY;

    return E_SUCCESS;
}

E_API e_result e_input_set_prev_absolute_cursor_position(e_input* pInput, int prevCursorPosX, int prevCursorPosY)
{
    if (pInput == NULL) {
        return E_INVALID_ARGS;
    }

    pInput->prevAbsoluteCursorPosX = prevCursorPosX;
    pInput->prevAbsoluteCursorPosY = prevCursorPosY;

    return E_SUCCESS;
}

E_API e_bool32 e_input_has_cursor_moved(e_input* pInput)
{
    if (pInput == NULL) {
        return E_FALSE;
    }

    return pInput->prevAbsoluteCursorPosX != pInput->currentAbsoluteCursorPosX ||
           pInput->prevAbsoluteCursorPosY != pInput->currentAbsoluteCursorPosY;
}

E_API void e_input_get_cursor_move_delta(e_input* pInput, int* pDeltaX, int* pDeltaY)
{
    if (pInput == NULL) {
        return;
    }

    if (pDeltaX != NULL) {
        *pDeltaX = pInput->currentAbsoluteCursorPosX - pInput->prevAbsoluteCursorPosX;
    }
    if (pDeltaY != NULL) {
        *pDeltaY = pInput->currentAbsoluteCursorPosY - pInput->prevAbsoluteCursorPosY;
    }
}

E_API int e_input_get_cursor_move_delta_x(e_input* pInput)
{
    if (pInput == NULL) {
        return 0;
    }

    return pInput->currentAbsoluteCursorPosX - pInput->prevAbsoluteCursorPosX;
}

E_API int e_input_get_cursor_move_delta_y(e_input* pInput)
{
    if (pInput == NULL) {
        return 0;
    }

    return pInput->currentAbsoluteCursorPosY - pInput->prevAbsoluteCursorPosY;
}

E_API void e_input_set_cursor_button_state(e_input* pInput, e_uint32 buttonIndex, int state)
{
    if (pInput == NULL || buttonIndex >= E_MAX_CURSOR_BUTTONS) {
        return;
    }

    pInput->cursorButtonStates[buttonIndex] = state;
}

E_API int e_input_get_cursor_button_state(e_input* pInput, e_uint32 buttonIndex)
{
    if (pInput == NULL || buttonIndex >= E_MAX_CURSOR_BUTTONS) {
        return 0;
    }

    return pInput->cursorButtonStates[buttonIndex];
}

E_API e_bool32 e_input_was_cursor_button_pressed(e_input* pInput, e_uint32 buttonIndex)
{
    if (pInput == NULL || buttonIndex >= E_MAX_CURSOR_BUTTONS) {
        return E_FALSE;
    }

    return pInput->cursorButtonStates[buttonIndex] == E_BUTTON_STATE_DOWN && pInput->prevCursorButtonStates[buttonIndex] == E_BUTTON_STATE_UP;
}

E_API e_bool32 e_input_was_cursor_button_released(e_input* pInput, e_uint32 buttonIndex)
{
    if (pInput == NULL || buttonIndex >= E_MAX_CURSOR_BUTTONS) {
        return E_FALSE;
    }

    return pInput->cursorButtonStates[buttonIndex] == E_BUTTON_STATE_UP && pInput->prevCursorButtonStates[buttonIndex] == E_BUTTON_STATE_DOWN;
}

E_API void e_input_set_cursor_wheel_delta(e_input* pInput, int delta)
{
    if (pInput == NULL) {
        return;
    }

    pInput->cursorWheelDelta = delta;
}

E_API int e_input_get_cursor_wheel_delta(e_input* pInput)
{
    if (pInput == NULL) {
        return 0;
    }

    return pInput->cursorWheelDelta;
}

E_API void e_input_set_key_down(e_input* pInput, e_uint32 key)
{
    e_uint32 iKey;

    if (pInput == NULL) {
        return;
    }

    /* If there's no room in the key down buffer we'll need to just drop this key. */
    if (pInput->keysDownCount >= E_COUNTOF(pInput->keysDown)) {
        return;
    }

    /*
    The key needs to be added to the key down list. It should not be possible to press a key more than once so assume
    the caller is calling this and e_input_set_key_up() properly.
    */
    pInput->keysDown[pInput->keysDownCount] = key;
    pInput->keysDownCount += 1;


    /*
    The key pressed/released state needs to be tracked. This will allow us to check if the key was pressed
    in situations where it was pressed and released between calls to e_input_step().
    */

    /* Find the key. */
    for (iKey = 0; iKey < pInput->keyPressedStateCount; ++iKey) {
        if (pInput->keyPressedStates[iKey].key == key) {
            break;
        }
    }

    if (iKey >= E_COUNTOF(pInput->keyPressedStates)) {
        return;
    }

    if (iKey == pInput->keyPressedStateCount) {
        pInput->keyPressedStateCount += 1;
    }

    pInput->keyPressedStates[iKey].key = key;
    pInput->keyPressedStates[iKey].stateFlags |= E_KEY_STATE_FLAG_DOWN;
}

E_API void e_input_set_key_up(e_input* pInput, e_uint32 key)
{
    e_uint32 iKey;

    if (pInput == NULL) {
        return;
    }

    /* We just need to clear out the key from the currently downed key list. */
    for (iKey = 0; iKey < pInput->keysDownCount; iKey += 1) {
        if (pInput->keysDown[iKey] == key) {
            /* Move the other keys down. */
            for (; iKey < pInput->keysDownCount - 1; iKey += 1) {
                pInput->keysDown[iKey] = pInput->keysDown[iKey + 1];
            }
            pInput->keysDownCount -= 1;

            break;
        }
    }

    /* As with set_key_down(), we need to apply a separate flag to track the pressed/released state. */
    for (iKey = 0; iKey < pInput->keyPressedStateCount; ++iKey) {
        if (pInput->keyPressedStates[iKey].key == key) {
            break;
        }
    }

    if (iKey >= E_COUNTOF(pInput->keyPressedStates)) {
        return;
    }

    if (iKey == pInput->keyPressedStateCount) {
        pInput->keyPressedStateCount += 1;
    }

    pInput->keyPressedStates[iKey].key = key;
    pInput->keyPressedStates[iKey].stateFlags |= E_KEY_STATE_FLAG_UP;
}

E_API e_bool32 e_input_was_key_pressed(e_input* pInput, e_uint32 key)
{
    e_uint32 iKey;

    if (pInput == NULL) {
        return E_FALSE;
    }

    /* Find the key. */
    for (iKey = 0; iKey < pInput->keyPressedStateCount; ++iKey) {
        if (pInput->keyPressedStates[iKey].key == key) {
            return (pInput->keyPressedStates[iKey].stateFlags & E_KEY_STATE_FLAG_DOWN) != 0;
        }
    }

    /* Getting here means the key's state isn't in the buffer which means it musn't have been pressed this frame. */
    return E_FALSE;
}

E_API e_bool32 e_input_was_key_released(e_input* pInput, e_uint32 key)
{
    e_uint32 iKey;

    if (pInput == NULL) {
        return E_FALSE;
    }

    /* Find the key. */
    for (iKey = 0; iKey < pInput->keyPressedStateCount; ++iKey) {
        if (pInput->keyPressedStates[iKey].key == key) {
            return (pInput->keyPressedStates[iKey].stateFlags & E_KEY_STATE_FLAG_UP) != 0;
        }
    }

    /* Getting here means the key's state isn't in the buffer which means it musn't have been released this frame. */
    return E_FALSE;
}

E_API e_bool32 e_input_is_key_down(e_input* pInput, e_uint32 key)
{
    e_uint32 iKey;

    if (pInput == NULL) {
        return E_FALSE;
    }

    /* Find the key. */
    for (iKey = 0; iKey < pInput->keysDownCount; ++iKey) {
        if (pInput->keysDown[iKey] == key) {
            return E_TRUE;
        }
    }

    /* Getting here means the key couldn't be found in the list of downed keys. */
    return E_FALSE;
}

E_API void e_input_enqueue_character(e_input* pInput, e_uint32 utf32)
{
    e_uint32 iNextCharacter;

    if (pInput == NULL) {
        return;
    }

    iNextCharacter = (pInput->characterIndex + pInput->characterCount) % E_COUNTOF(pInput->characters);
    pInput->characters[iNextCharacter] = utf32;
    pInput->characterCount += 1;
}

E_API e_uint32 e_input_dequeue_character(e_input* pInput)
{
    e_uint32 utf32;

    if (pInput == NULL) {
        return 0;
    }

    if (pInput->characterCount == 0) {
        return 0;
    }

    utf32 = pInput->characters[pInput->characterIndex];

    pInput->characterCount -= 1;
    pInput->characterIndex = (pInput->characterIndex + 1) % E_COUNTOF(pInput->characters);

    return utf32;

}
/* === END e_input.c === */



/* ==== BEG e_graphics.c ==== */
static const char* e_graphics_get_backend_name(e_graphics_backend backend); /* Implemented near the bottom of this section. */

/*
OpenGL backend.
*/
#if !defined(E_NO_OPENGL)
#if defined(E_EMSCRIPTEN)
#include <GLES3/gl32.h>
#endif

typedef struct
{
    e_graphics base;    /* Must be the first item so we can cast. */
} e_graphics_opengl;

typedef struct
{
    e_graphics_device base; /* Must be the first item so we can cast. */

    /* Platform-specific rendering context. */
    union
    {
    #if defined(E_WIN32)
        struct
        {
            HGLRC hRC;  /* Used for window render targets. */
        } win32;
    #endif
    #if defined(E_DESKTOP_UNIX)
        struct
        {
            GLXContext hRC;
        } x11;
    #endif
    #if defined(E_EMSCRIPTEN)
        struct
        {
            EGLContext context;
        } egl;
    #endif
    } platform;
} e_graphics_device_opengl;

typedef struct
{
    e_graphics_surface base;
} e_graphics_surface_opengl;


const char* e_graphics_opengl_get_name(void* pUserData)
{
    E_UNUSED(pUserData);
    return e_graphics_get_backend_name(E_GRAPHICS_BACKEND_OPENGL);
}


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

    /* Note: glbind has already been initialized by e_engine_init(). */

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

    (void)pSurfaceOpenGL;

#if 0
#if defined(E_WIN32)
    e_engine_gl(pSurface->pGraphics->pEngine)->wglMakeCurrent((HDC)e_window_get_platform_object(pSurface->pWindow, E_PLATFORM_OBJECT_WIN32_HDC), pSurfaceOpenGL->platform.win32.hRC);
#endif
#if defined(E_DESKTOP_UNIX)
    e_engine_gl(pSurface->pGraphics->pEngine)->glXMakeCurrent((glbind_Display*)e_window_get_platform_object(pSurface->pWindow, E_PLATFORM_OBJECT_XLIB_DISPLAY), (GLXDrawable)e_window_get_platform_object(pSurface->pWindow, E_PLATFORM_OBJECT_XLIB_WINDOW), pSurfaceOpenGL->platform.x11.hRC);
#endif
#if defined(E_EMSCRIPTEN)
    {
        EGLDisplay displayEGL = (EGLDisplay)e_window_get_platform_object(pSurface->pWindow, E_PLATFORM_OBJECT_EGL_DISPLAY);
        eglMakeCurrent(displayEGL, pSurfaceOpenGL->platform.egl.surface, pSurfaceOpenGL->platform.egl.surface, pSurfaceOpenGL->platform.egl.context);
    }
    
#endif
#endif

    return E_SUCCESS;
}

static e_result e_graphics_opengl_present_surface(void* pUserData, e_graphics* pGraphics, e_graphics_surface* pSurface)
{
    E_ASSERT(pGraphics != NULL);
    E_UNUSED(pUserData);

    (void)pSurface;

#if 0
#if defined(E_WIN32)
    e_SwapBuffers((HDC)e_window_get_platform_object(pSurface->pWindow, E_PLATFORM_OBJECT_WIN32_HDC));
#endif
#if defined(E_DESKTOP_UNIX)
    e_engine_gl(pSurface->pGraphics->pEngine)->glXSwapBuffers((glbind_Display*)e_window_get_platform_object(pSurface->pWindow, E_PLATFORM_OBJECT_XLIB_DISPLAY), (glbind_Window)e_window_get_platform_object(pSurface->pWindow, E_PLATFORM_OBJECT_XLIB_WINDOW));
#endif
#if defined(E_EMSCRIPTEN)
    {
        EGLDisplay displayEGL = (EGLDisplay)e_window_get_platform_object(pSurface->pWindow, E_PLATFORM_OBJECT_EGL_DISPLAY);
        eglSwapBuffers(displayEGL, ((e_graphics_surface_opengl*)pSurface)->platform.egl.surface);
    }
#endif
#endif

    return E_SUCCESS;
}



static e_result e_graphics_device_opengl_alloc_size(void* pUserData, const e_graphics_device_config* pConfig, size_t* pSize)
{
    E_ASSERT(pConfig != NULL);
    E_ASSERT(pSize   != NULL);
    E_UNUSED(pUserData);

    *pSize = sizeof(e_graphics_device_opengl);

    return E_SUCCESS;
}

static e_result e_graphics_device_opengl_init(void* pUserData, e_graphics_device* pDevice, const e_graphics_device_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_graphics_device_opengl* pDeviceOpenGL = (e_graphics_device_opengl*)pDevice;

    E_ASSERT(pConfig != NULL);
    E_ASSERT(pDevice != NULL);
    E_UNUSED(pUserData);

    (void)pAllocationCallbacks;

    if (pConfig->deviceID != E_DEFAULT_GRAPHICS_DEVICE_ID) {
        return E_INVALID_ARGS;  /* We're only supporting default devices in OpenGL. */
    }

    /* Different ways of initializing a rendering context depending on the platform. */
    #if defined(E_WIN32)
    {
        GLBapi* pGL = e_engine_gl(pDevice->pGraphics->pEngine);
        E_ASSERT(pGL != NULL);

        pDeviceOpenGL->platform.win32.hRC = pGL->wglCreateContext(glbGetDC());
        if (pDeviceOpenGL->platform.win32.hRC == NULL) {
            e_log_postf(e_graphics_get_log(pDevice->pGraphics), E_LOG_LEVEL_ERROR, "Failed to create HGLRC.");
            return E_ERROR;
        }

        return E_SUCCESS;
    }
    #endif

    #if defined(E_DESKTOP_UNIX)
    {
        GLBapi* pGL = e_engine_gl(pDevice->pGraphics->pEngine);
        E_ASSERT(pGL != NULL);

        pDeviceOpenGL->platform.x11.hRC = pGL->glXCreateContext(glbGetDisplay(), glbGetFBVisualInfo(), NULL, GL_TRUE);
        if (pDeviceOpenGL->platform.x11.hRC == NULL) {
            e_log_postf(e_graphics_get_log(pDevice->pGraphics), E_LOG_LEVEL_ERROR, "Failed to create GLXContext.");
            return E_ERROR;
        }

        return E_SUCCESS;
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

        if (!eglChooseConfig(displayEGL, pConfigAttributesEGL, &configEGL, 1, &configCount)) {
            e_log_postf(e_graphics_get_log(pSurface->pGraphics), E_LOG_LEVEL_ERROR, "Could not find EGL context.");
            return E_ERROR;
        }

        contextEGL = eglCreateContext(displayEGL, configEGL, EGL_NO_CONTEXT, pContextConfigEGL);
        if (contextEGL == NULL) {
            e_log_postf(e_graphics_get_log(pSurface->pGraphics), E_LOG_LEVEL_ERROR, "Failed to create EGL context.");
            return E_ERROR;
        }

        pDeviceOpenGL->platform.egl.context = contextEGL;

        return E_SUCCESS;
    }
    #endif
}

static void e_graphics_device_opengl_uninit(void* pUserData, e_graphics_device* pDevice, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_graphics_device_opengl* pDeviceOpenGL = (e_graphics_device_opengl*)pDevice;

    E_ASSERT(pDevice != NULL);
    E_UNUSED(pUserData);

    (void)pAllocationCallbacks;

    /* Check if it's a window render target. If so, it needs to be deleted using platform-specific code. */
    #if defined(E_WIN32)
    {
        if (pDeviceOpenGL->platform.win32.hRC != NULL) {
            e_engine_gl(pDevice->pGraphics->pEngine)->wglDeleteContext(pDeviceOpenGL->platform.win32.hRC);
        }
    }
    #endif

    #if defined(E_DESKTOP_UNIX)
    {
        if (pDeviceOpenGL->platform.x11.hRC != NULL) {
            e_engine_gl(pDevice->pGraphics->pEngine)->glXDestroyContext(glbGetDisplay(), pDeviceOpenGL->platform.x11.hRC);
        }
    }
    #endif

    #if defined(E_EMSCRIPTEN)
    {
        eglDestroyContext((EGLDisplay)e_platform_get_object(E_PLATFORM_OBJECT_EGL_DISPLAY), pDeviceOpenGL->platform.egl.context);
    }
    #endif
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

    E_ASSERT(pConfig  != NULL);
    E_ASSERT(pSurface != NULL);
    E_UNUSED(pUserData);

    (void)pAllocationCallbacks;

    /* There's different ways to create a surface depending on the platform. */
    #if defined(E_WIN32)
    {
        (void)pSurfaceOpenGL;
        return E_SUCCESS;
    }
    #endif

    #if defined(E_DESKTOP_UNIX)
    {
        (void)pSurfaceOpenGL;
        return E_SUCCESS;
    }
    #endif

    #if defined(E_EMSCRIPTEN)
    {
        EGLSurface surfaceEGL;

        surfaceEGL = eglCreateWindowSurface((EGLDisplay)e_platform_get_object(E_PLATFORM_OBJECT_EGL_DISPLAY), configEGL, 0, NULL);
        if (surfaceEGL == NULL) {
            e_log_postf(e_graphics_get_log(pSurface->pGraphics), E_LOG_LEVEL_ERROR, "Failed to create EGL window surface.");
            return E_ERROR;
        }

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
    #if defined(E_WIN32)
    {
        (void)pSurfaceOpenGL;
    }
    #endif

    #if defined(E_DESKTOP_UNIX)
    {
        (void)pSurfaceOpenGL;
    }
    #endif

    #if defined(E_EMSCRIPTEN)
    {
        eglDestroySurface((EGLDisplay)e_platform_get_object(E_PLATFORM_OBJECT_EGL_DISPLAY), pSurfaceOpenGL->platform.egl.surface);
    }
    #endif
}

static e_result e_graphics_surface_opengl_refresh(void* pUserData, e_graphics_surface* pSurface, const e_allocation_callbacks* pAllocationCallbacks)
{
    E_UNUSED(pUserData);
    E_UNUSED(pSurface);
    E_UNUSED(pAllocationCallbacks);

    /* There's nothing to do for OpenGL. This is all handled automatically by the driver. */

    return E_SUCCESS;
}



static e_graphics_vtable e_gGraphicsBackendVTable_OpenGL =
{
    e_graphics_opengl_get_name,
    e_graphics_opengl_alloc_size,
    e_graphics_opengl_init,
    e_graphics_opengl_uninit,
    e_graphics_opengl_get_devices,
    e_graphics_opengl_set_surface,
    e_graphics_opengl_present_surface,

    e_graphics_device_opengl_alloc_size,
    e_graphics_device_opengl_init,
    e_graphics_device_opengl_uninit,

    e_graphics_surface_opengl_alloc_size,
    e_graphics_surface_opengl_init,
    e_graphics_surface_opengl_uninit,
    e_graphics_surface_opengl_refresh
};
#endif  /* E_NO_OPENGL */


/*
Vulkan backend.
*/
#if !defined(E_NO_VULKAN)
typedef struct e_graphics_vulkan         e_graphics_vulkan;
typedef struct e_graphics_device_vulkan  e_graphics_device_vulkan;
typedef struct e_graphics_surface_vulkan e_graphics_surface_vulkan;

struct e_graphics_vulkan
{
    e_graphics base;    /* Must be the first item so we can cast. */
    VkInstance instanceVK;
    VkDebugUtilsMessengerEXT debugMessengerVK;
    VkbAPI vk;              /* Instance-specific APIs. */
};

struct e_graphics_device_vulkan
{
    e_graphics_device base;     /* Must be the first item so we can cast. */
    VkPhysicalDevice physicalDeviceVK;
    VkDevice deviceVK;
    VkQueue graphicsQueueVK;
    VkQueue computeQueueVK;
    VkQueue transferQueueVK;
    uint32_t graphicsQueueFamilyIndex;
    uint32_t computeQueueFamilyIndex;
    uint32_t transferQueueFamilyIndex;
    VkbAPI vk;  /* Device-specific APIs. */
};

struct e_graphics_surface_vulkan
{
    e_graphics_surface base;
    VkSurfaceKHR surfaceVK;
    VkSwapchainKHR swapchainVK;
    VkSemaphore swapchainSemaphoreVK;   /* For synchronizing swapchain image swaps. */
    uint32_t currentSwapchainImageIndex;
};


const char* e_graphics_vulkan_get_name(void* pUserData)
{
    E_UNUSED(pUserData);
    return e_graphics_get_backend_name(E_GRAPHICS_BACKEND_VULKAN);
}


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
        #if defined(E_WIN32)
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
    /*
    e_graphics_surface_vulkan* pSurfaceVulkan = (e_graphics_surface_vulkan*)pSurface;
    VkResult resultVK;
    VkPresentInfoKHR presentInfo;
    */

    E_ASSERT(pGraphics != NULL);
    E_UNUSED(pUserData);

    (void)pSurface;

#if 0
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
#endif


    return E_SUCCESS;
}



static e_result e_graphics_device_vulkan_alloc_size(void* pUserData, const e_graphics_device_config* pConfig, size_t* pSize)
{
    E_ASSERT(pConfig != NULL);
    E_ASSERT(pSize   != NULL);
    E_UNUSED(pUserData);

    *pSize = sizeof(e_graphics_device_vulkan);

    return E_SUCCESS;
}


static e_result e_graphics_device_vulkan_init(void* pUserData, e_graphics_device* pDevice, const e_graphics_device_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_graphics_device_vulkan* pDeviceVulkan = (e_graphics_device_vulkan*)pDevice;
    e_graphics_vulkan* pGraphicsVulkan = (e_graphics_vulkan*)pDevice->pGraphics;
    VkResult resultVK;
    VkAllocationCallbacks allocationCallbacksVK;

    E_ASSERT(pConfig != NULL);
    E_ASSERT(pDevice != NULL);
    E_UNUSED(pUserData);

    allocationCallbacksVK = e_graphics_VkAllocationCallbacks_init(pAllocationCallbacks);

    /* Physical Device. */
    {
        uint32_t physicalDeviceCount;
        VkPhysicalDevice* pPhysicalDevicesVK;

        resultVK = pGraphicsVulkan->vk.vkEnumeratePhysicalDevices(pGraphicsVulkan->instanceVK, &physicalDeviceCount, NULL);
        if (resultVK != VK_SUCCESS) {
            e_log_postf(e_graphics_get_log(pDevice->pGraphics), E_LOG_LEVEL_ERROR, "Failed to enumerate Vulkan physical devices. vkEnumeratePhysicalDevices() returned %d.", resultVK);
            return e_result_from_vk(resultVK);
        }

        pPhysicalDevicesVK = (VkPhysicalDevice*)e_malloc(sizeof(*pPhysicalDevicesVK) * physicalDeviceCount, pAllocationCallbacks);
        if (pPhysicalDevicesVK == NULL) {
            return E_OUT_OF_MEMORY;
        }

        resultVK = pGraphicsVulkan->vk.vkEnumeratePhysicalDevices(pGraphicsVulkan->instanceVK, &physicalDeviceCount, pPhysicalDevicesVK);
        if (resultVK != VK_SUCCESS) {
            e_log_postf(e_graphics_get_log(pDevice->pGraphics), E_LOG_LEVEL_ERROR, "Failed to enumerate Vulkan physical devices. vkEnumeratePhysicalDevices() returned %d.", resultVK);
            e_free(pPhysicalDevicesVK, pAllocationCallbacks);
            return e_result_from_vk(resultVK);
        }

        /*
        At this point we have a list of physical devices and now we need to pick one. We want to use the one with
        the device ID that was specified in the config, or the default one if none was specified.
        */
        if (pConfig->deviceID == E_DEFAULT_GRAPHICS_DEVICE_ID) {
            /* TODO: Pick the first device that supports graphics. Will need to inspect the queue families with vkGetPhysicalDeviceQueueFamilyProperties(). See get_devices() above.  */
            pDeviceVulkan->physicalDeviceVK = pPhysicalDevicesVK[0];
        } else {
            uint32_t iPhysicalDevice;
            for (iPhysicalDevice = 0; iPhysicalDevice < physicalDeviceCount; iPhysicalDevice += 1) {
                VkPhysicalDeviceProperties properties;
                pGraphicsVulkan->vk.vkGetPhysicalDeviceProperties(pPhysicalDevicesVK[iPhysicalDevice], &properties);

                if (properties.deviceID == pConfig->deviceID) {
                    pDeviceVulkan->physicalDeviceVK = pPhysicalDevicesVK[iPhysicalDevice];
                    break;
                }
            }
        }

        e_free(pPhysicalDevicesVK, pAllocationCallbacks);
    }

    /* At this point we have a physical device. We can now create the VkDevice object. */
    {
        VkQueueFamilyProperties* pQueueFamilyProperties;
        uint32_t queueFamilyCount;
        uint32_t iQueueFamily;
        float pQueuePriorities[1];
        VkDeviceQueueCreateInfo pQueueInfos[3];
        uint32_t queueCount = 0;  /* Must be initialized to 0. Will be a maximum of 3. */
        VkPhysicalDeviceFeatures physicalDeviceFeatures;
        const char* pEnabledDeviceExtensions[] = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        uint32_t enabledDeviceExtensionCount = E_COUNTOF(pEnabledDeviceExtensions);
        VkDeviceCreateInfo deviceInfo;


        /*
        We need to specify the queues we want to create. We're going to use a simplified model where
        we only have a single queue for each category: graphics, computer and transfer. Sometimes
        these queues will be the same and sometimes they will be different.

        All graphics queue families support transfer, but we'll try finding a dedicated transfer
        queue so we can make use of it for async host-to-device transfers.
        */
        pGraphicsVulkan->vk.vkGetPhysicalDeviceQueueFamilyProperties(pDeviceVulkan->physicalDeviceVK, &queueFamilyCount, NULL);

        pQueueFamilyProperties = (VkQueueFamilyProperties*)e_malloc(sizeof(*pQueueFamilyProperties) * queueFamilyCount, pAllocationCallbacks);
        if (pQueueFamilyProperties == NULL) {
            return E_OUT_OF_MEMORY;
        }

        pGraphicsVulkan->vk.vkGetPhysicalDeviceQueueFamilyProperties(pDeviceVulkan->physicalDeviceVK, &queueFamilyCount, pQueueFamilyProperties);

        /* Initialize our queue indices to -1 so we can identifier whether or not we've found one. */
        pDeviceVulkan->graphicsQueueFamilyIndex = (uint32_t)-1;
        pDeviceVulkan->computeQueueFamilyIndex  = (uint32_t)-1;
        pDeviceVulkan->transferQueueFamilyIndex = (uint32_t)-1;
    
        /*
        First we need to find a graphics queue. We'll use the first one we find. We'll do the same
        for compute. We're going to prefer queues that support both graphics and compute, but if
        that's not possible we'll use separate queues.
        */
        for (iQueueFamily = 0; iQueueFamily < queueFamilyCount; iQueueFamily += 1) {
            if ((pQueueFamilyProperties[iQueueFamily].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)) == (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)) {
                pDeviceVulkan->graphicsQueueFamilyIndex = iQueueFamily;
                pDeviceVulkan->computeQueueFamilyIndex  = iQueueFamily;
                break;
            }
        }

        /* If we didn't find a queue family that supports both graphics and compute, we'll try to find separate queue families. */
        if (pDeviceVulkan->graphicsQueueFamilyIndex == (uint32_t)-1) {
            for (iQueueFamily = 0; iQueueFamily < queueFamilyCount; iQueueFamily += 1) {
                if ((pQueueFamilyProperties[iQueueFamily].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
                    pDeviceVulkan->graphicsQueueFamilyIndex = iQueueFamily;
                    break;
                }
            }
        }

        if (pDeviceVulkan->graphicsQueueFamilyIndex == (uint32_t)-1) {
            for (iQueueFamily = 0; iQueueFamily < queueFamilyCount; iQueueFamily += 1) {
                if ((pQueueFamilyProperties[iQueueFamily].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) {
                    pDeviceVulkan->computeQueueFamilyIndex = iQueueFamily;
                    break;
                }
            }
        }

        /* We'll try to find a dedicated transfer queue. If we can't find one, we just use the graphics queue. */
        for (iQueueFamily = 0; iQueueFamily < queueFamilyCount; iQueueFamily += 1) {
            if ((pQueueFamilyProperties[iQueueFamily].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0 && (pQueueFamilyProperties[iQueueFamily].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT)) == 0) {
                pDeviceVulkan->transferQueueFamilyIndex = iQueueFamily;
                break;
            }
        }

        if (pDeviceVulkan->transferQueueFamilyIndex == (uint32_t)-1) {
            pDeviceVulkan->transferQueueFamilyIndex = pDeviceVulkan->graphicsQueueFamilyIndex;  /* Couldn't find a dedicated transfer queue. Use the graphics queue. */
        }


        e_free(pQueueFamilyProperties, pAllocationCallbacks);
        pQueueFamilyProperties = NULL;

        /* We must have at least a graphics and transfer queue. */
        if (pDeviceVulkan->graphicsQueueFamilyIndex == (uint32_t)-1) {
            e_log_postf(e_graphics_device_get_log(pDevice), E_LOG_LEVEL_WARNING, "Could not find a queue family supporting graphics.");
            return E_ERROR;
        }
        
        /* All graphics queues support transfer, so at this point we must also have a transfer queue index. If not, there's an error in the logic above. */
        E_ASSERT(pDeviceVulkan->transferQueueFamilyIndex != (uint32_t)-1);




        /*
        We'll be creating one queue for each unique queue family index.
        */
        pQueuePriorities[0] = 1;

        /* Start with the graphics queue. */
        pQueueInfos[0].sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        pQueueInfos[0].pNext            = NULL;
        pQueueInfos[0].flags            = 0;
        pQueueInfos[0].queueFamilyIndex = pDeviceVulkan->graphicsQueueFamilyIndex;
        pQueueInfos[0].queueCount       = 1;
        pQueueInfos[0].pQueuePriorities = pQueuePriorities;
        queueCount += 1;

        /* Compute, if it's different to the graphics queue. */
        if (pDeviceVulkan->computeQueueFamilyIndex != pDeviceVulkan->graphicsQueueFamilyIndex) {
            pQueueInfos[queueCount] = pQueueInfos[0];
            pQueueInfos[queueCount].queueFamilyIndex = pDeviceVulkan->computeQueueFamilyIndex;
            queueCount += 1;
        }

        /* Transfer, if it's different to the graphics queue. */
        if (pDeviceVulkan->transferQueueFamilyIndex != pDeviceVulkan->graphicsQueueFamilyIndex) {
            pQueueInfos[queueCount] = pQueueInfos[0];
            pQueueInfos[queueCount].queueFamilyIndex = pDeviceVulkan->transferQueueFamilyIndex;
            queueCount += 1;
        }
        


        /*
        When initializing a device we need to specify a feature set that we need. For now I'm just keeping
        this simple and enabling anything that is supported by the hardware. It may, however, be better to
        set this properly so that if the device doesn't support something we need it'll fail cleanly.
        */
        pGraphicsVulkan->vk.vkGetPhysicalDeviceFeatures(pDeviceVulkan->physicalDeviceVK, &physicalDeviceFeatures);

        deviceInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceInfo.pNext                   = NULL;
        deviceInfo.flags                   = 0;
        deviceInfo.queueCreateInfoCount    = queueCount;
        deviceInfo.pQueueCreateInfos       = pQueueInfos;
        deviceInfo.enabledLayerCount       = 0;
        deviceInfo.ppEnabledLayerNames     = NULL;
        deviceInfo.enabledExtensionCount   = enabledDeviceExtensionCount;
        deviceInfo.ppEnabledExtensionNames = pEnabledDeviceExtensions;
        deviceInfo.pEnabledFeatures        = &physicalDeviceFeatures;  /* <-- Setting this to NULL is equivalent to disabling all features. */

        resultVK = pGraphicsVulkan->vk.vkCreateDevice(pDeviceVulkan->physicalDeviceVK, &deviceInfo, &allocationCallbacksVK, &pDeviceVulkan->deviceVK);
        if (resultVK != VK_SUCCESS) {
            e_log_postf(e_graphics_device_get_log(pDevice), E_LOG_LEVEL_ERROR, "Failed to create Vulkan device. vkCreateDevice() returned %d.", resultVK);
            return e_result_from_vk(resultVK);  /* Failed to create the device object. */
        }


        /* We need device-specific function pointers for Vulkan. */
        pDeviceVulkan->vk = pGraphicsVulkan->vk;
        resultVK = vkbInitDeviceAPI(pDeviceVulkan->deviceVK, &pDeviceVulkan->vk);
        if (resultVK != VK_SUCCESS) {
            e_log_postf(e_graphics_device_get_log(pDevice), E_LOG_LEVEL_ERROR, "Failed to retrieve device-specific Vulkan function pointers.");
            return e_result_from_vk(resultVK);
        }

        /* Now that we have a device we can retrieve the queues for later use. */
        pDeviceVulkan->vk.vkGetDeviceQueue(pDeviceVulkan->deviceVK, pDeviceVulkan->graphicsQueueFamilyIndex, 0, &pDeviceVulkan->graphicsQueueVK);
        pDeviceVulkan->vk.vkGetDeviceQueue(pDeviceVulkan->deviceVK, pDeviceVulkan->computeQueueFamilyIndex,  0, &pDeviceVulkan->computeQueueVK);
        pDeviceVulkan->vk.vkGetDeviceQueue(pDeviceVulkan->deviceVK, pDeviceVulkan->transferQueueFamilyIndex, 0, &pDeviceVulkan->transferQueueVK);
    }

    return E_SUCCESS;
}

static void e_graphics_device_vulkan_uninit(void* pUserData, e_graphics_device* pDevice, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_graphics_device_vulkan* pDeviceVulkan = (e_graphics_device_vulkan*)pDevice;
    VkAllocationCallbacks allocationCallbacksVK;

    E_UNUSED(pUserData);

    if (pDeviceVulkan->deviceVK != VK_NULL_HANDLE) {
        allocationCallbacksVK = e_graphics_VkAllocationCallbacks_init(pAllocationCallbacks);

        pDeviceVulkan->vk.vkDestroyDevice(pDeviceVulkan->deviceVK, &allocationCallbacksVK);
        pDeviceVulkan->deviceVK = VK_NULL_HANDLE;
    }
}



static e_result e_graphics_surface_vulkan_alloc_size(void* pUserData, const e_graphics_surface_config* pConfig, size_t* pSize)
{
    E_ASSERT(pConfig != NULL);
    E_ASSERT(pSize   != NULL);
    E_UNUSED(pUserData);

    *pSize = sizeof(e_graphics_surface_vulkan);

    return E_SUCCESS;
}

static e_result e_graphics_surface_vulkan_init(void* pUserData, e_graphics_surface* pSurface, const e_graphics_surface_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_graphics_surface_vulkan* pSurfaceVulkan = (e_graphics_surface_vulkan*)pSurface;
    e_graphics_device_vulkan* pDeviceVulkan = (e_graphics_device_vulkan*)pSurface->pDevice;
    e_graphics_vulkan* pGraphicsVulkan = (e_graphics_vulkan*)pSurface->pGraphics;
    e_result result = E_ERROR;
    VkResult resultVK;
    VkAllocationCallbacks allocationCallbacksVK;

    E_ASSERT(pConfig  != NULL);
    E_ASSERT(pSurface != NULL);
    E_UNUSED(pUserData);

    allocationCallbacksVK = e_graphics_VkAllocationCallbacks_init(pAllocationCallbacks);

    /*
    The first step is to create the VkSurfaceKHR object which is the platform-specific part. We create
    this from the window that was passed into the surface config.
    */
    #if defined(E_WIN32)
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
    /* TODO: Linux, macOS, etc. */

    /* Now that we have our surface we need to check that it's compatible with our device. */
    {
        VkBool32 isSupported;
        VkSurfaceCapabilitiesKHR surfaceCaps;
        resultVK = pGraphicsVulkan->vk.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pDeviceVulkan->physicalDeviceVK, pSurfaceVulkan->surfaceVK, &surfaceCaps);
        if (resultVK != VK_SUCCESS) {
            e_log_postf(e_graphics_surface_get_log(pSurface), E_LOG_LEVEL_ERROR, "Failed to retrieve surface capabilities. vkGetPhysicalDeviceSurfaceCapabilitiesKHR() returned %d.", resultVK);
            result = e_result_from_vk(resultVK);
            goto error0;
        }

        if (surfaceCaps.maxImageCount < 2) {
            e_log_postf(e_graphics_surface_get_log(pSurface), E_LOG_LEVEL_ERROR, "The surface and device combination do not support at least two images (required for double buffering).");
            result = e_result_from_vk(resultVK);
            goto error0;
        }

        /* The physical device needs to support outputting to our surface. To determine support, we need the physical device and queue family. */
        resultVK = pGraphicsVulkan->vk.vkGetPhysicalDeviceSurfaceSupportKHR(pDeviceVulkan->physicalDeviceVK, pDeviceVulkan->graphicsQueueFamilyIndex, pSurfaceVulkan->surfaceVK, &isSupported);
        if (resultVK != VK_SUCCESS) {
            e_log_postf(e_graphics_surface_get_log(pSurface), E_LOG_LEVEL_ERROR, "Failed to retrieve surface support. vkGetPhysicalDeviceSurfaceSupportKHR() returned %d.", resultVK);
            result = e_result_from_vk(resultVK);
            goto error0;
        }

        if (!isSupported) {
            e_log_postf(e_graphics_surface_get_log(pSurface), E_LOG_LEVEL_ERROR, "The surface and device combination do not support outputting to the surface.");
            result = e_result_from_vk(resultVK);
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

        resultVK = pGraphicsVulkan->vk.vkGetPhysicalDeviceSurfaceFormatsKHR(pDeviceVulkan->physicalDeviceVK, pSurfaceVulkan->surfaceVK, &supportedSurfaceFormatCount, NULL);
        if (resultVK != VK_SUCCESS) {
            e_log_postf(e_graphics_surface_get_log(pSurface), E_LOG_LEVEL_ERROR, "Failed to retrieve physical device surface format count. vkGetPhysicalDeviceSurfaceFormatsKHR() returned %d.", resultVK);
            result = e_result_from_vk(resultVK);
            goto error0;
        }

        pSupportedSurfaceFormats = (VkSurfaceFormatKHR*)e_malloc(sizeof(*pSupportedSurfaceFormats) * supportedSurfaceFormatCount, pAllocationCallbacks);
        if (pSupportedSurfaceFormats == NULL) {
            result = E_OUT_OF_MEMORY;
            goto error0;
        }

        resultVK = pGraphicsVulkan->vk.vkGetPhysicalDeviceSurfaceFormatsKHR(pDeviceVulkan->physicalDeviceVK, pSurfaceVulkan->surfaceVK, &supportedSurfaceFormatCount, pSupportedSurfaceFormats);
        if (resultVK != VK_SUCCESS) {
            e_log_postf(e_graphics_surface_get_log(pSurface), E_LOG_LEVEL_ERROR, "Failed to retrieve physical device surface formats. vkGetPhysicalDeviceSurfaceFormatsKHR() returned %d.", resultVK);
            e_free(pSupportedSurfaceFormats, pAllocationCallbacks);
            result = e_result_from_vk(resultVK);
            goto error0;
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
            goto error0;
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
            resultVK = pGraphicsVulkan->vk.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(pDeviceVulkan->physicalDeviceVK, pSurfaceVulkan->surfaceVK, &surfaceCaps);
            if (resultVK != VK_SUCCESS) {
                e_free(pSupportedSurfaceFormats, pAllocationCallbacks);
                result = e_result_from_vk(resultVK);
                goto error0;
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

            resultVK = pDeviceVulkan->vk.vkCreateSwapchainKHR(pDeviceVulkan->deviceVK, &swapchainInfo, &allocationCallbacksVK, &pSurfaceVulkan->swapchainVK);
            if (resultVK != VK_SUCCESS) {
                e_log_postf(e_graphics_surface_get_log(pSurface), E_LOG_LEVEL_ERROR, "Failed to create swapchain. vkCreateSwapchainKHR() returned %d.", resultVK);
                e_free(pSupportedSurfaceFormats, pAllocationCallbacks);
                result = e_result_from_vk(resultVK);
                goto error0;
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

            resultVK = pDeviceVulkan->vk.vkCreateSemaphore(pDeviceVulkan->deviceVK, &semaphoreInfo, &allocationCallbacksVK, &pSurfaceVulkan->swapchainSemaphoreVK);
            if (resultVK != VK_SUCCESS) {
                e_log_postf(e_graphics_surface_get_log(pSurface), E_LOG_LEVEL_ERROR, "Failed to create swapchain semaphore. vkCreateSemaphore() returned %d.", resultVK);
                result = e_result_from_vk(resultVK);
                goto error1;
            }
        }

        /*
        Now that we've got the semaphore we can go ahead and acquire the first image. We need to
        do this so we can get the index of the next image in the swapchain.
        */
        resultVK = pDeviceVulkan->vk.vkAcquireNextImageKHR(pDeviceVulkan->deviceVK, pSurfaceVulkan->swapchainVK, UINT64_MAX, pSurfaceVulkan->swapchainSemaphoreVK, VK_NULL_HANDLE, &pSurfaceVulkan->currentSwapchainImageIndex);
        if (resultVK != VK_SUCCESS && resultVK != VK_SUBOPTIMAL_KHR) {
            /* TODO: Post an error. */
            result = e_result_from_vk(resultVK);
            goto error2;
        }

        if (resultVK == VK_SUBOPTIMAL_KHR) {
            /* TODO: Post a warning. */
        }
    }

    return E_SUCCESS;

error2:
    pDeviceVulkan->vk.vkDestroySemaphore(pDeviceVulkan->deviceVK, pSurfaceVulkan->swapchainSemaphoreVK, &allocationCallbacksVK);
error1:
    pDeviceVulkan->vk.vkDestroySwapchainKHR(pDeviceVulkan->deviceVK, pSurfaceVulkan->swapchainVK, &allocationCallbacksVK);
error0:
    #if defined(E_WIN32)
    {
        pGraphicsVulkan->vk.vkDestroySurfaceKHR(pGraphicsVulkan->instanceVK, pSurfaceVulkan->surfaceVK, &allocationCallbacksVK);
    }
    #endif
    /* TODO: Linus, macOS, etc. */

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

    pGraphicsVulkan->vk.vkDestroySurfaceKHR(pGraphicsVulkan->instanceVK, pSurfaceVulkan->surfaceVK, &allocationCallbacksVK);
}

static e_result e_graphics_surface_vulkan_refresh(void* pUserData, e_graphics_surface* pSurface, const e_allocation_callbacks* pAllocationCallbacks)
{
    e_graphics_surface_vulkan* pSurfaceVulkan = (e_graphics_surface_vulkan*)pSurface;
    VkAllocationCallbacks allocationCallbacksVK;

    E_ASSERT(pSurface != NULL);
    E_UNUSED(pUserData);

    allocationCallbacksVK = e_graphics_VkAllocationCallbacks_init(pAllocationCallbacks);

    /* TODO: Here is where we should re-create the swapchain. Don't forget to pass in the previous swapchain here. */
    (void)pSurfaceVulkan;
    (void)allocationCallbacksVK;

    return E_SUCCESS;
}


static e_graphics_vtable e_gGraphicsBackendVTable_Vulkan =
{
    e_graphics_vulkan_get_name,
    e_graphics_vulkan_alloc_size,
    e_graphics_vulkan_init,
    e_graphics_vulkan_uninit,
    e_graphics_vulkan_get_devices,
    e_graphics_vulkan_set_surface,
    e_graphics_vulkan_present_surface,

    e_graphics_device_vulkan_alloc_size,
    e_graphics_device_vulkan_init,
    e_graphics_device_vulkan_uninit,

    e_graphics_surface_vulkan_alloc_size,
    e_graphics_surface_vulkan_init,
    e_graphics_surface_vulkan_uninit,
    e_graphics_surface_vulkan_refresh
};
#endif

/* Retrieves the VTable of the given stock graphics backend. */
static const e_graphics_vtable* e_graphics_get_backend_vtable(e_graphics_backend backend)
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

#if 0
static const char* e_graphics_get_backend_name_from_vtable(const e_graphics_vtable* pVTable, void* pVTableUserData)
{
    if (pVTable == NULL || pVTable->get_name == NULL) {
        return "Unknown";
    }

    return pVTable->get_name(pVTableUserData);
}
#endif

static const char* e_graphics_get_backend_name(e_graphics_backend backend)
{
    switch (backend)
    {
        case E_GRAPHICS_BACKEND_OPENGL: return "OpenGL";
        case E_GRAPHICS_BACKEND_VULKAN: return "Vulkan";
        case E_GRAPHICS_BACKEND_CUSTOM: return "Custom";
        case E_GRAPHICS_BACKEND_UNKNOWN:
        default:
            return "Unknown";
    }
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
        if (backend == E_GRAPHICS_BACKEND_CUSTOM) {
            return E_INVALID_ARGS;      /* Custom backend specified, but no vtable provided. */
        } else {
            return E_BACKEND_DISABLED;  /* Couldn't find an appropriate vtable. Not being able to find a stock backend's vtable means it's been disabled at compile time. */
        }
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
                /* Don't post a warning if the backend has been disabled at compile time. In this case it should feel as though it doesn't even exist. */
                if (result != E_BACKEND_DISABLED) {
                    e_log_postf(e_engine_get_log(pConfig->pEngine), E_LOG_LEVEL_WARNING, "Failed to initialize graphics backend %s.", e_graphics_get_backend_name((e_graphics_backend)iGraphicsBackend));
                }
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
            e_log_postf(e_engine_get_log(pConfig->pEngine), E_LOG_LEVEL_ERROR, "Failed to initailize graphics backend.");
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



E_API e_graphics_device_config e_graphics_device_config_init(e_graphics* pGraphics)
{
    e_graphics_device_config config;

    E_ZERO_OBJECT(&config);
    config.pGraphics = pGraphics;

    return config;
}


E_API e_result e_graphics_device_alloc_size(const e_graphics_device_config* pConfig, size_t* pSize)
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

    if (pConfig->pGraphics->pVTable->device_alloc_size == NULL) {
        return E_NOT_IMPLEMENTED;
    }

    result = pConfig->pGraphics->pVTable->device_alloc_size(pConfig->pGraphics->pVTableUserData, pConfig, &size);
    if (result != E_SUCCESS) {
        return result;  /* Failed to retrieve the size of the allocation. */
    }

    *pSize = size;
    return E_SUCCESS;
}

E_API e_result e_graphics_device_init_preallocated(const e_graphics_device_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_graphics_device* pDevice)
{
    e_result result;

    if (pDevice == NULL) {
        return E_INVALID_ARGS;
    }

    if (pConfig == NULL || pConfig->pGraphics == NULL) {
        return E_INVALID_ARGS;
    }

    pDevice->pGraphics = pConfig->pGraphics;

    if (pConfig->pGraphics->pVTable->device_init == NULL) {
        return E_NOT_IMPLEMENTED;
    }

    result = pConfig->pGraphics->pVTable->device_init(pConfig->pGraphics->pVTableUserData, pDevice, pConfig, pAllocationCallbacks);
    if (result != E_SUCCESS) {
        return result;
    }

    return E_SUCCESS;
}

E_API e_result e_graphics_device_init(const e_graphics_device_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_graphics_device** ppDevice)
{
    e_result result;
    e_graphics_device* pDevice;
    size_t allocationSize;

    if (ppDevice == NULL) {
        return E_INVALID_ARGS;
    }

    *ppDevice = NULL;

    result = e_graphics_device_alloc_size(pConfig, &allocationSize);
    if (result != E_SUCCESS) {
        return result;
    }

    E_ASSERT(allocationSize >= sizeof(e_graphics_device));

    pDevice = (e_graphics_device*)e_calloc(allocationSize, pAllocationCallbacks);
    if (pDevice == NULL) {
        return E_OUT_OF_MEMORY;
    }

    result = e_graphics_device_init_preallocated(pConfig, pAllocationCallbacks, pDevice);
    if (result != E_SUCCESS) {
        e_free(*ppDevice, pAllocationCallbacks);
        return result;
    }

    pDevice->freeOnUninit = E_TRUE;

    *ppDevice = pDevice;
    return E_SUCCESS;
}

E_API void e_graphics_device_uninit(e_graphics_device* pDevice, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pDevice == NULL) {
        return;
    }

    if (pDevice->pGraphics->pVTable->device_uninit != NULL) {
        pDevice->pGraphics->pVTable->device_uninit(pDevice->pGraphics->pVTableUserData, pDevice, pAllocationCallbacks);
    }

    if (pDevice->freeOnUninit) {
        e_free(pDevice, pAllocationCallbacks);
    }
}

E_API e_graphics* e_graphics_device_get_graphics(const e_graphics_device* pDevice)
{
    if (pDevice == NULL) {
        return NULL;
    }

    return pDevice->pGraphics;
}

E_API e_log* e_graphics_device_get_log(e_graphics_device* pDevice)
{
    if (pDevice == NULL) {
        return NULL;
    }

    return e_graphics_get_log(e_graphics_device_get_graphics(pDevice));
}



E_API e_graphics_surface_config e_graphics_surface_config_init(e_graphics_device* pDevice, e_window* pWindow)
{
    e_graphics_surface_config config;

    E_ZERO_OBJECT(&config);
    config.pDevice = pDevice;
    config.pWindow = pWindow;

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

    if (pConfig == NULL || pConfig->pDevice == NULL) {
        return E_INVALID_ARGS;
    }

    if (pConfig->pDevice->pGraphics->pVTable->surface_alloc_size == NULL) {
        return E_NOT_IMPLEMENTED;
    }

    result = pConfig->pDevice->pGraphics->pVTable->surface_alloc_size(pConfig->pDevice->pGraphics->pVTableUserData, pConfig, &size);
    if (result != E_SUCCESS) {
        return result;  /* Failed to retrieve the size of the allocation. */
    }

    *pSize = size;
    return E_SUCCESS;
}

E_API e_result e_graphics_surface_init_preallocated(const e_graphics_surface_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_graphics_surface* pSurface)
{
    e_result result;

    if (pSurface == NULL || pConfig == NULL || pConfig->pDevice == NULL) {
        return E_INVALID_ARGS;
    }

    pSurface->pGraphics = pConfig->pDevice->pGraphics;
    pSurface->pDevice   = pConfig->pDevice;
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

    if (pConfig->pDevice == NULL) {
        return E_INVALID_ARGS;
    }

    result = pConfig->pDevice->pGraphics->pVTable->surface_alloc_size(pConfig->pDevice->pGraphics->pVTableUserData, pConfig, &allocationSize);
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
        e_log_postf(e_graphics_get_log(pConfig->pDevice->pGraphics), E_LOG_LEVEL_ERROR, "Failed to initialize graphics surface.");
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

E_API e_result e_graphics_surface_refresh(e_graphics_surface* pSurface, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pSurface == NULL) {
        return E_INVALID_ARGS;
    }

    if (pSurface->pGraphics->pVTable->surface_refresh == NULL) {
        return E_NOT_IMPLEMENTED;
    }

    return pSurface->pGraphics->pVTable->surface_refresh(pSurface->pGraphics->pVTableUserData, pSurface, pAllocationCallbacks);
}

E_API e_graphics* e_graphics_surface_get_graphics(const e_graphics_surface* pSurface)
{
    return e_graphics_device_get_graphics(e_graphics_surface_get_device(pSurface));
}

E_API e_graphics_device* e_graphics_surface_get_device(const e_graphics_surface* pSurface)
{
    if (pSurface == NULL) {
        return NULL;
    }

    return pSurface->pDevice;
}

E_API e_log* e_graphics_surface_get_log(e_graphics_surface* pSurface)
{
    return e_graphics_device_get_log(e_graphics_surface_get_device(pSurface));
}
/* ==== END e_graphics.c ==== */




/* ==== BEG e_client.c ==== */
static e_result e_client_handle_event(e_client* pClient, e_event* pEvent)
{
    e_result result;

    E_ASSERT(pEvent  != NULL);
    E_ASSERT(pClient != NULL);

    if (pClient->pVTable == NULL || pClient->pVTable->onEvent == NULL) {
        result = E_NOT_IMPLEMENTED;
    } else {
        result = pClient->pVTable->onEvent(pClient->pVTableUserData, pClient, pEvent);
    }

    if (result == E_NOT_IMPLEMENTED) {
        return e_client_default_event_handler(pClient, pEvent);
    } else {
        return result;
    }
}

static e_result e_client_window_event_callback(void* pUserData, e_window* pWindow, e_event* pEvent)
{
    e_client_handle_event((e_client*)pUserData, pEvent);

    /* Make sure the event is handled by the window system's default event handler. */
    return e_window_default_event_handler(pWindow, pEvent);
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
    config.graphicsDeviceID = E_DEFAULT_GRAPHICS_DEVICE_ID;

    return config;
}


typedef struct
{
    size_t size;
    size_t inputOffset;
} e_client_alloc_layout;

static e_input_config e_input_config_init_from_client_config(const e_client_config* pClientConfig)
{
    e_input_config inputConfig = e_input_config_init();

    /* TODO: Do something with this when the time comes. */
    (void)pClientConfig;

    return inputConfig;
}

static e_result e_client_get_alloc_layout(const e_client_config* pConfig, e_client_alloc_layout* pLayout)
{
    e_result result;

    E_ASSERT(pConfig != NULL);
    E_ASSERT(pLayout != NULL);

    pLayout->size = E_ALIGN_64(sizeof(e_client));

    pLayout->inputOffset = pLayout->size;
    {
        e_input_config inputConfig;
        size_t inputSize;

        inputConfig = e_input_config_init_from_client_config(pConfig);
        result = e_input_alloc_size(&inputConfig, &inputSize);
        if (result != E_SUCCESS) {
            return result;
        }

        pLayout->size += E_ALIGN_64(inputSize);
    }
    

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

    E_ZERO_MEMORY(pClient, layout.size);

    pClient->pEngine         = pConfig->pEngine;
    pClient->pUserData       = pConfig->pUserData;
    pClient->pVTable         = pConfig->pVTable;
    pClient->pVTableUserData = pConfig->pVTableUserData;
    pClient->flags           = pConfig->flags;
    pClient->pInput          = NULL;    /* Will be initialized below. */
    pClient->pConfigSection  = pConfig->pConfigFileSection;
    pClient->allocationCallbacks = e_allocation_callbacks_init_copy(pAllocationCallbacks);

    /* If we don't have a window, we don't have any graphics either. */
    if ((pClient->flags & E_CLIENT_FLAG_NO_WINDOW) != 0) {
        pClient->flags |= E_CLIENT_FLAG_NO_GRAPHICS;
    }

    /* The window need to be initialized. */
    if ((pClient->flags & E_CLIENT_FLAG_NO_WINDOW) == 0) {
        unsigned int resolutionX;
        unsigned int resolutionY;
        unsigned int windowFlags;
        const char* pTitle;
        char* pTitleFromConfig = NULL;

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

        windowFlags = 0;
        if ((pConfig->pEngine->flags & E_ENGINE_FLAG_NO_OPENGL) == 0 || (pConfig->flags & E_CLIENT_FLAG_OPENGL_WINDOW) != 0) {
            windowFlags |= E_WINDOW_FLAG_OPENGL;
        }

        windowConfig = e_window_config_init(pClient->pEngine, pTitle, 0, 0, resolutionX, resolutionY, windowFlags, &e_gClientWindowVTable, pClient);

        result = e_window_init(&windowConfig, pAllocationCallbacks, &pClient->pWindow);
        if (result != E_SUCCESS) {
            e_log_postf(e_engine_get_log(pConfig->pEngine), E_LOG_LEVEL_ERROR, "Failed to initialize client window.");
            return result;
        }

        if (pTitleFromConfig != NULL) {
            e_free(pTitleFromConfig, pAllocationCallbacks);
        }

        pClient->windowSizeX = resolutionX;
        pClient->windowSizeY = resolutionY;
        pClient->windowResized = E_TRUE;    /* Always set this to true so that e_client_has_window_resized() always returns true at least once which will give applications a chance to do an initialize setup for their graphics system. */
    } else {
        pClient->windowResized = E_FALSE;
    }

    if ((pClient->flags & E_CLIENT_FLAG_NO_GRAPHICS) == 0) {
        e_graphics_config graphicsConfig;
        e_graphics_device_config deviceConfig;
        e_graphics_surface_config surfaceConfig;

        E_ASSERT(pClient->pWindow != NULL); /* We should never be getting here if we don't have a window. */


        /* We need to initialize the graphics sub-system. */
        graphicsConfig = e_graphics_config_init(pConfig->pEngine);
        graphicsConfig.backend = pConfig->graphicsBackend;

        result = e_graphics_init(&graphicsConfig, pAllocationCallbacks, &pClient->pGraphics);
        if (result != E_SUCCESS) {
            e_log_postf(e_engine_get_log(pConfig->pEngine), E_LOG_LEVEL_ERROR, "Failed to initialize graphics sub-system for client.");
            goto error0;
        }


        /*
        Now that we have our graphics system initialized we can create our device. We use the device ID that
        was passed into the config, which defaults to the default graphics device.
        */
        deviceConfig = e_graphics_device_config_init(pClient->pGraphics);
        deviceConfig.deviceID = pConfig->graphicsDeviceID;

        result = e_graphics_device_init(&deviceConfig, pAllocationCallbacks, &pClient->pGraphicsDevice);
        if (result != E_SUCCESS) {
            e_log_postf(e_engine_get_log(pConfig->pEngine), E_LOG_LEVEL_ERROR, "Failed to initialize graphics device for client.");
            goto error1;
        }


        /* We need a surface to connect our window to the graphics system. */
        surfaceConfig = e_graphics_surface_config_init(pClient->pGraphicsDevice, pClient->pWindow);

        result = e_graphics_surface_init(&surfaceConfig, pAllocationCallbacks, &pClient->pGraphicsSurface);
        if (result != E_SUCCESS) {
            e_log_postf(e_engine_get_log(pConfig->pEngine), E_LOG_LEVEL_ERROR, "Failed to initialize graphics surface for client.");
            goto error2;
        }
    }

    /* Input. */
    {
        e_input_config inputConfig = e_input_config_init_from_client_config(pConfig);

        pClient->pInput = (e_input*)E_OFFSET_PTR(pClient, layout.inputOffset);

        result = e_input_init_preallocated(&inputConfig, pAllocationCallbacks, pClient->pInput);
        if (result != E_SUCCESS) {
            e_log_postf(e_engine_get_log(pConfig->pEngine), E_LOG_LEVEL_ERROR, "Failed to initialize input sub-system for client.");
            goto error3;
        }
    }

    return E_SUCCESS;


/*error4:
    e_input_uninit(pClient->pInput, pAllocationCallbacks);*/
error3:
    if ((pClient->flags & E_CLIENT_FLAG_NO_GRAPHICS) == 0) {
        e_graphics_surface_uninit(pClient->pGraphicsSurface, pAllocationCallbacks);
    }
error2:
    if ((pClient->flags & E_CLIENT_FLAG_NO_GRAPHICS) == 0) {
        e_graphics_device_uninit(pClient->pGraphicsDevice, pAllocationCallbacks);
    }
error1:
    if ((pClient->flags & E_CLIENT_FLAG_NO_GRAPHICS) == 0) {
        e_graphics_uninit(pClient->pGraphics, pAllocationCallbacks);
    }
error0:
    if ((pClient->flags & E_CLIENT_FLAG_NO_WINDOW) == 0) {
        e_window_uninit(pClient->pWindow, pAllocationCallbacks);
    }

    return result;
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

    e_input_uninit(pClient->pInput, pAllocationCallbacks);
    pClient->pInput = NULL;

    if ((pClient->flags & E_CLIENT_FLAG_NO_GRAPHICS) == 0) {
        e_graphics_surface_uninit(pClient->pGraphicsSurface, pAllocationCallbacks);
        pClient->pGraphicsSurface = NULL;

        e_graphics_device_uninit(pClient->pGraphicsDevice, pAllocationCallbacks);
        pClient->pGraphicsDevice = NULL;

        e_graphics_uninit(pClient->pGraphics, pAllocationCallbacks);
        pClient->pGraphics = NULL;
    }

    if ((pClient->flags & E_CLIENT_FLAG_NO_WINDOW) == 0) {
        e_window_uninit(pClient->pWindow, pAllocationCallbacks);
        pClient->pWindow = NULL;
    }

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

E_API e_window* e_client_get_window(e_client* pClient)
{
    if (pClient == NULL) {
        return NULL;
    }

    return pClient->pWindow;
}

E_API e_result e_client_on_window_resize(e_client* pClient, e_uint32 sizeX, e_uint32 sizeY)
{
    if (pClient == NULL) {
        return E_INVALID_ARGS;
    }

    pClient->windowSizeX = sizeX;
    pClient->windowSizeY = sizeY;
    pClient->windowResized = E_TRUE;

    return E_SUCCESS;
}

E_API e_bool32 e_client_get_window_size(e_client* pClient, e_uint32* pSizeX, e_uint32* pSizeY)
{
    if (pClient == NULL) {
        return E_FALSE;
    }

    if (pSizeX != NULL) {
        *pSizeX = pClient->windowSizeX;
    }

    if (pSizeY != NULL) {
        *pSizeY = pClient->windowSizeY;
    }

    return pClient->windowResized;
}

E_API e_input* e_client_get_input(e_client* pClient)
{
    if (pClient == NULL) {
        return NULL;
    }

    return pClient->pInput;
}

E_API e_result e_client_default_event_handler(e_client* pClient, e_event* pEvent)
{
    /* NOTE: This function must never return E_NOT_IMPLEMENTED. */

    if (pClient == NULL || pEvent == NULL) {
        return E_INVALID_ARGS;
    }

    /* Pass the message into our input handler. */
    e_client_update_input_from_event(pClient, pEvent);

    switch (pEvent->type)
    {
        case E_EVENT_CLOSE:
        {
            /*
            This WINDOW_CLOSE event will be fired when the user closes the window via the OS. By default
            we'll just exit from the engine's loop which will cause the engine to shut down. This is
            useful as a default action, but a real application might want to handle this themselves and
            show a confirmation prompt or something.
            */
            e_engine_exit(pClient->pEngine, 0);
        } break;

        case E_EVENT_SIZE:
        {
            /* Tell the client that the window has been resized. */
            e_client_on_window_resize(pClient, (e_uint32)pEvent->data.size.x, (e_uint32)pEvent->data.size.y);

            /*
            When the window is resized we need to also resize the surface. If we don't do this, the surface's
            swapchain can be put into an invalid state and graphics will break.

            When resizing the surface we don't actually pass in the new size of the window. Instead, the
            required size of the swapchain is calculated internally.
            */
            e_graphics_surface_refresh(pClient->pGraphicsSurface, &pClient->allocationCallbacks);
        } break;

        default: break;
    }

    return E_SUCCESS;
}

E_API e_result e_client_post_close_event(e_client* pClient)
{
    return e_window_post_close_event(e_client_get_window(pClient));
}

E_API e_result e_client_update_input_from_event(e_client* pClient, const e_event* pEvent)
{
    if (pClient == NULL || pEvent == NULL) {
        return E_INVALID_ARGS;
    }

    switch (pEvent->type)
    {
        case E_EVENT_CURSOR_MOVE:
        {
            if (pClient->isCursorPinned) {
                e_input_add_cursor_delta_position(pClient->pInput, pEvent->data.cursorMove.x, pEvent->data.cursorMove.y);
            } else {
                e_input_set_absolute_cursor_position(pClient->pInput, pEvent->data.cursorMove.x, pEvent->data.cursorMove.y);
            }
        } break;

        case E_EVENT_CURSOR_BUTTON_DOWN:
        {
            e_input_set_cursor_button_state(pClient->pInput, pEvent->data.cursorButtonDown.button, E_BUTTON_STATE_DOWN);
        } break;

        case E_EVENT_CURSOR_BUTTON_UP:
        {
            e_input_set_cursor_button_state(pClient->pInput, pEvent->data.cursorButtonUp.button, E_BUTTON_STATE_UP);
        } break;

        case E_EVENT_CURSOR_WHEEL:
        {
            e_input_set_cursor_wheel_delta(pClient->pInput, pEvent->data.cursorWheel.delta);
        } break;

        case E_EVENT_KEY_DOWN:
        {
            /* Our input system doesn't use auto-repeat so ignore any auto-repeated keys. */
            if (!pEvent->data.keyDown.isAutoRepeat) {
                e_input_set_key_down(pClient->pInput, pEvent->data.keyDown.key);
            }
        } break;

        case E_EVENT_KEY_UP:
        {
            e_input_set_key_up(pClient->pInput, pEvent->data.keyUp.key);
        } break;

        case E_EVENT_CHARACTER:
        {
            e_input_enqueue_character(pClient->pInput, pEvent->data.character.utf32);
        } break;
        
        default: break;
    }

    /*
    NOTE: Always returning E_SUCCESS here even when the event type isn't handled. We want this function to
    be able to be called without the caller having to check the event type.
    */
    return E_SUCCESS;
}


static e_result e_client_step_default(void* pUserData, e_client* pClient, double dt)
{
    E_UNUSED(pUserData);

    (void)dt;

    /* Draw stuff. */
    e_graphics_set_surface(pClient->pGraphics, pClient->pGraphicsSurface);
    {
        //glClearColor(0.5f, 0.5f, 1.0f, 0);
        //glClear(GL_COLOR_BUFFER_BIT);
    }
    e_graphics_present_surface(pClient->pGraphics, pClient->pGraphicsSurface);

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

    /* Reset the window resize state. */
    pClient->windowResized = E_FALSE;

    /* Step the input state last. */
    e_client_step_input(pClient);

    /* After stepping the input, if we have a pinned mouse cursor we'll want to position it back to it's location. */
    if (pClient->isCursorPinned) {
        /* When the cursor is pinned make sure the absolute position is reset back to 0. */
        e_input_set_absolute_cursor_position(pClient->pInput, 0, 0);
        e_input_set_prev_absolute_cursor_position(pClient->pInput, 0, 0);
    }

    if (result != E_SUCCESS) {
        return result;
    }

    return E_SUCCESS;
}

E_API e_result e_client_step_input(e_client* pClient)
{
    return e_input_step(e_client_get_input(pClient));
}

E_API e_bool32 e_client_has_cursor_moved(e_client* pClient)
{
    return e_input_has_cursor_moved(e_client_get_input(pClient));
}

E_API e_result e_client_get_absolute_cursor_position(e_client* pClient, int* pPosX, int* pPosY)
{
    return e_input_get_absolute_cursor_position(e_client_get_input(pClient), pPosX, pPosY);
}

E_API e_result e_client_capture_cursor(e_client* pClient)
{
    if (pClient == NULL) {
        return E_INVALID_ARGS;
    }

    return e_window_capture_cursor(pClient->pWindow);
}

E_API e_result e_client_release_cursor(e_client* pClient)
{
    if (pClient == NULL) {
        return E_INVALID_ARGS;
    }

    return e_window_release_cursor(pClient->pWindow);
}

E_API e_result e_client_pin_cursor(e_client* pClient, int pinnedCursorPosX, int pinnedCursorPosY)
{
    if (pClient == NULL) {
        return E_INVALID_ARGS;
    }

    pClient->isCursorPinned = E_TRUE;

    /*
    When the cursor is pinned the window system will report delta positions. We need to pretend that the previous
    mouse position is at coordinates 0,0 so that calculating the "relative" mouse positions work properly based
    on deltas.
    */
    e_input_set_absolute_cursor_position(pClient->pInput, 0, 0);
    e_input_set_prev_absolute_cursor_position(pClient->pInput, 0, 0);

    /* Now pin the cursor via the window system. From here on our the move position will be reported using deltas. */
    e_window_pin_cursor(pClient->pWindow, pinnedCursorPosX, pinnedCursorPosY);

    return E_SUCCESS;
}

E_API e_result e_client_unpin_cursor(e_client* pClient)
{
    if (pClient == NULL) {
        return E_INVALID_ARGS;
    }

    pClient->isCursorPinned = E_FALSE;
    e_window_unpin_cursor(pClient->pWindow);

    return E_SUCCESS;
}

E_API e_result e_client_show_cursor(e_client* pClient)
{
    if (pClient == NULL) {
        return E_INVALID_ARGS;
    }

    return e_window_show_cursor(pClient->pWindow);
}

E_API e_result e_client_hide_cursor(e_client* pClient)
{
    if (pClient == NULL) {
        return E_INVALID_ARGS;
    }

    return e_window_hide_cursor(pClient->pWindow);
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