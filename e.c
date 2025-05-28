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


/* Silence a warning with Emscripten. */
#if defined(E_EMSCRIPTEN)
#define E_SUPPRESS_MUTEX_TIMEDLOCK_FALLBACK_WARNING
#endif


/* BEG e_va_copy.c */
#ifndef e_va_copy
    #if !defined(_MSC_VER) || _MSC_VER >= 1800
        #if (defined(__GNUC__) && __GNUC__ < 3)
            #define e_va_copy(dst, src) ((dst) = (src))    /* This is untested. Not sure if this is correct for old GCC. */
        #else
            #define e_va_copy(dst, src) va_copy((dst), (src))
        #endif
    #else
        #define e_va_copy(dst, src) ((dst) = (src))
    #endif
#endif
/* END e_va_copy.c */


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

#define E_ROUND_UP(value, multiple_PowerOfTwo2) (((value) + ((multiple_PowerOfTwo2) - 1)) & ~((multiple_PowerOfTwo2) - 1))
#define E_ROUND_UP_4(value)        E_ROUND_UP(value, 4)
#define E_ROUND_UP_16(value)       E_ROUND_UP(value, 16)


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
        case 0:       return E_SUCCESS;
        case ENOENT:  return E_DOES_NOT_EXIST;
        case EEXIST:  return E_ALREADY_EXISTS;
        case ENOTDIR: return E_NOT_DIRECTORY;
        case ENOMEM:  return E_OUT_OF_MEMORY;
        case EINVAL:  return E_INVALID_ARGS;
        default: break;
    }

    /* Fall back to a generic error. */
    return E_ERROR;
}



/* BEG e_basic_strings.c */
E_API size_t e_strlen(const char* src)
{
    const char* end;

    E_ASSERT(src != NULL);
    
    end = src;
    while (end[0] != '\0') {
        end += 1;
    }

    return end - src;
}

E_API char* e_strcpy(char* dst, const char* src)
{
    char* dstorig;

    E_ASSERT(dst != NULL);
    E_ASSERT(src != NULL);

    dstorig = dst;

    /* No, we're not using this garbage: while (*dst++ = *src++); */
    for (;;) {
        *dst = *src;
        if (*src == '\0') {
            break;
        }

        dst += 1;
        src += 1;
    }

    return dstorig;
}

E_API int e_strncpy(char* dst, const char* src, size_t count)
{
    size_t maxcount;
    size_t i;

    if (dst == 0) {
        return EINVAL;
    }
    if (src == 0) {
        dst[0] = '\0';
        return EINVAL;
    }

    maxcount = count;

    for (i = 0; i < maxcount && src[i] != '\0'; ++i) {
        dst[i] = src[i];
    }

    if (src[i] == '\0' || i == count || count == ((size_t)-1)) {
        dst[i] = '\0';
        return 0;
    }

    dst[0] = '\0';
    return ERANGE;
}

E_API int e_strcpy_s(char* dst, size_t dstCap, const char* src)
{
    size_t i;

    if (dst == 0) {
        return EINVAL;
    }
    if (dstCap == 0) {
        return ERANGE;
    }
    if (src == 0) {
        dst[0] = '\0';
        return EINVAL;
    }

    for (i = 0; i < dstCap && src[i] != '\0'; ++i) {
        dst[i] = src[i];
    }

    if (i < dstCap) {
        dst[i] = '\0';
        return 0;
    }

    dst[0] = '\0';
    return ERANGE;
}

E_API int e_strncpy_s(char* dst, size_t dstCap, const char* src, size_t count)
{
    size_t maxcount;
    size_t i;

    if (dst == 0) {
        return EINVAL;
    }
    if (dstCap == 0) {
        return EINVAL;
    }
    if (src == 0) {
        dst[0] = '\0';
        return EINVAL;
    }

    maxcount = count;
    if (count == ((size_t)-1) || count >= dstCap) {        /* -1 = _TRUNCATE */
        maxcount = dstCap - 1;
    }

    for (i = 0; i < maxcount && src[i] != '\0'; ++i) {
        dst[i] = src[i];
    }

    if (src[i] == '\0' || i == count || count == ((size_t)-1)) {
        dst[i] = '\0';
        return 0;
    }

    dst[0] = '\0';
    return ERANGE;
}

E_API int e_strcat_s(char* dst, size_t dstCap, const char* src)
{
    char* dstorig;

    if (dst == 0) {
        return EINVAL;
    }
    if (dstCap == 0) {
        return ERANGE;
    }
    if (src == 0) {
        dst[0] = '\0';
        return EINVAL;
    }

    dstorig = dst;

    while (dstCap > 0 && dst[0] != '\0') {
        dst    += 1;
        dstCap -= 1;
    }

    if (dstCap == 0) {
        return EINVAL;  /* Unterminated. */
    }

    while (dstCap > 0 && src[0] != '\0') {
        *dst++ = *src++;
        dstCap -= 1;
    }

    if (dstCap > 0) {
        dst[0] = '\0';
    } else {
        dstorig[0] = '\0';
        return ERANGE;
    }

    return 0;
}

E_API int e_strncat_s(char* dst, size_t dstCap, const char* src, size_t count)
{
    char* dstorig;

    if (dst == 0) {
        return EINVAL;
    }
    if (dstCap == 0) {
        return ERANGE;
    }
    if (src == 0) {
        return EINVAL;
    }

    dstorig = dst;

    while (dstCap > 0 && dst[0] != '\0') {
        dst    += 1;
        dstCap -= 1;
    }

    if (dstCap == 0) {
        return EINVAL;  /* Unterminated. */
    }

    if (count == ((size_t)-1)) {        /* _TRUNCATE */
        count = dstCap - 1;
    }

    while (dstCap > 0 && src[0] != '\0' && count > 0) {
        *dst++ = *src++;
        dstCap -= 1;
        count  -= 1;
    }

    if (dstCap > 0) {
        dst[0] = '\0';
    } else {
        dstorig[0] = '\0';
        return ERANGE;
    }

    return 0;
}

E_API int e_itoa_s(int value, char* dst, size_t dstCap, int radix)
{
    int sign;
    unsigned int valueU;
    char* dstEnd;

    if (dst == NULL || dstCap == 0) {
        return EINVAL;
    }
    if (radix < 2 || radix > 36) {
        dst[0] = '\0';
        return EINVAL;
    }

    sign = (value < 0 && radix == 10) ? -1 : 1;     /* The negative sign is only used when the base is 10. */

    if (value < 0) {
        valueU = -value;
    } else {
        valueU = value;
    }

    dstEnd = dst;
    do
    {
        int remainder = valueU % radix;
        if (remainder > 9) {
            *dstEnd = (char)((remainder - 10) + 'a');
        } else {
            *dstEnd = (char)(remainder + '0');
        }

        dstEnd += 1;
        dstCap -= 1;
        valueU /= radix;
    } while (dstCap > 0 && valueU > 0);

    if (dstCap == 0) {
        dst[0] = '\0';
        return EINVAL;  /* Ran out of room in the output buffer. */
    }

    if (sign < 0) {
        *dstEnd++ = '-';
        dstCap -= 1;
    }

    if (dstCap == 0) {
        dst[0] = '\0';
        return EINVAL;  /* Ran out of room in the output buffer. */
    }

    *dstEnd = '\0';


    /* At this point the string will be reversed. */
    dstEnd -= 1;
    while (dst < dstEnd) {
        char temp = *dst;
        *dst = *dstEnd;
        *dstEnd = temp;

        dst += 1;
        dstEnd -= 1;
    }

    return 0;
}

E_API int e_strcmp(const char* str1, const char* str2)
{
    if (str1 == str2) return  0;

    /* These checks differ from the standard implementation. It's not important, but I prefer it just for sanity. */
    if (str1 == NULL) return -1;
    if (str2 == NULL) return  1;

    for (;;) {
        if (str1[0] == '\0') {
            break;
        }

        if (str1[0] != str2[0]) {
            break;
        }

        str1 += 1;
        str2 += 1;
    }

    return ((unsigned char*)str1)[0] - ((unsigned char*)str2)[0];
}

E_API int e_strncmp(const char* str1, const char* str2, size_t maxLen)
{
    if (str1 == str2) return  0;

    /* These checks differ from the standard implementation. It's not important, but I prefer it just for sanity. */
    if (str1 == NULL) return -1;
    if (str2 == NULL) return  1;

    /* This function still needs to check for null terminators even though the length has been specified. */
    for (;;) {
        if (maxLen == 0) {
            break;
        }

        if (str1[0] == '\0') {
            break;
        }

        if (str1[0] != str2[0]) {
            break;
        }

        str1 += 1;
        str2 += 1;
        maxLen -= 1;
    }

    if (maxLen == 0) {
        return 0;
    }

    return ((unsigned char*)str1)[0] - ((unsigned char*)str2)[0];
}

E_API int e_stricmp_ascii(const char* str1, const char* str2)
{
    if (str1 == NULL || str2 == NULL) {
        return 0;
    }

    while (*str1 != '\0' && *str2 != '\0') {
        int c1 = (int)*str1;
        int c2 = (int)*str2;
    
        if (c1 >= 'A' && c1 <= 'Z') {
            c1 += 'a' - 'A';
        }
        if (c2 >= 'A' && c2 <= 'Z') {
            c2 += 'a' - 'A';
        }
    
        if (c1 != c2) {
            return c1 - c2;
        }
    
        str1 += 1;
        str2 += 1;
    }

    if (*str1 == '\0' && *str2 == '\0') {
        return 0;
    } else if (*str1 == '\0') {
        return -1;
    } else {
        return 1;
    }
}

E_API int e_strnicmp_ascii(const char* str1, const char* str2, size_t count)
{
    if (str1 == NULL || str2 == NULL) {
        return 0;
    }

    while (*str1 != '\0' && *str2 != '\0' && count > 0) {
        int c1 = (int)*str1;
        int c2 = (int)*str2;
    
        if (c1 >= 'A' && c1 <= 'Z') {
            c1 += 'a' - 'A';
        }
        if (c2 >= 'A' && c2 <= 'Z') {
            c2 += 'a' - 'A';
        }
       
        if (c1 != c2) {
            return c1 - c2;
        }
       
        str1  += 1;
        str2  += 1;
        count -= 1;
    }

    if (count == 0) {
        return 0;
    } else if (*str1 == '\0' && *str2 == '\0') {
        return 0;
    } else if (*str1 == '\0') {
        return -1;
    } else {
        return 1;
    }
}

E_API int e_stricmp(const char* str1, const char* str2)
{
    /* We will use the standard implementations of stricmp() and strcasecmp() if they are available. */
#if defined(_MSC_VER) && _MSC_VER >= 1400
    return _stricmp(str1, str2);
#elif defined(__GNUC__) && defined(__USE_GNU)
    return strcasecmp(str1, str2);
#else
    /* It would be good if we could use a custom implementation based on the Unicode standard here. Would require a lot of work to get that right, however. */
    return e_stricmp_ascii(str1, str2);
#endif
}

E_API int e_strnicmp(const char* str1, const char* str2, size_t count)
{
    /* We will use the standard implementations of strnicmp() and strncasecmp() if they are available. */
#if defined(_MSC_VER) && _MSC_VER >= 1400
    return _strnicmp(str1, str2, count);
#elif defined(__GNUC__) && defined(__USE_GNU)
    return strncasecmp(str1, str2, count);
#else
    /* It would be good if we could use a custom implementation based on the Unicode standard here. Would require a lot of work to get that right, however. */
    return e_strnicmp_ascii(str1, str2, count);
#endif
}
/* END e_basic_strings.c */



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



/* BEG e_allocation_callbacks.c */
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


static e_allocation_callbacks e_allocation_callbacks_init_default(void)
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
/* END e_allocation_callbacks.c */



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

                pSrc += E_ROUND_UP_4(pWindow->bufferSizeX);
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
            pWindow->pBufferDIBDataIntermediary = e_aligned_malloc((E_ROUND_UP_16(bufferSizeX * 4) * bufferSizeY), 16, NULL);
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



/* BEG e_misc.c */
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
/* END e_misc.c */



/* BEG e_timer.h */
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
/* END e_timer.h */



/* BEG e_net.c */
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
/* END e_net.c */


/* BEG e_thread.c */
/* Win32 */
#if defined(E_WIN32)
#include <windows.h>
#include <limits.h> /* For LONG_MAX */

#ifndef E_MALLOC
#define E_MALLOC(sz)        HeapAlloc(GetProcessHeap(), 0, (sz))
#endif

#ifndef E_REALLOC
#define E_REALLOC(p, sz)    (((sz) > 0) ? ((p) ? HeapReAlloc(GetProcessHeap(), 0, (p), (sz)) : HeapAlloc(GetProcessHeap(), 0, (sz))) : ((VOID*)(size_t)(HeapFree(GetProcessHeap(), 0, (p)) & 0)))
#endif

#ifndef E_FREE
#define E_FREE(p)           HeapFree(GetProcessHeap(), 0, (p))
#endif

static int e_thread_result_from_GetLastError(DWORD error)
{
    switch (error)
    {
        case ERROR_SUCCESS:             return E_SUCCESS;
        case ERROR_NOT_ENOUGH_MEMORY:   return E_OUT_OF_MEMORY;
        case ERROR_SEM_TIMEOUT:         return E_TIMEOUT;
        case ERROR_BUSY:                return E_BUSY;
        default: break;
    }

    return E_INVALID_ARGS;
}


static time_t e_timespec_to_milliseconds(const struct timespec ts)
{
    LONGLONG milliseconds;

    milliseconds = ((ts.tv_sec * 1000) + (ts.tv_nsec / 1000000));
    if ((ts.tv_nsec % 1000000) != 0) {
        milliseconds += 1; /* We truncated a sub-millisecond amount of time. Add an extra millisecond to meet the minimum duration requirement. */
    }

    return (time_t)milliseconds;
}

static time_t e_timespec_diff_milliseconds(const struct timespec tsA, const struct timespec tsB)
{
    return (unsigned int)e_timespec_to_milliseconds(e_timespec_diff(tsA, tsB));
}


typedef struct
{
    e_thread_start_callback func;
    void* arg;
    e_entry_exit_callbacks entryExitCallbacks;
    e_allocation_callbacks allocationCallbacks;
    int usingCustomAllocator;
} e_thread_start_data_win32;

static unsigned long WINAPI e_thread_start_win32(void* pUserData)
{
    e_thread_start_data_win32* pStartData = (e_thread_start_data_win32*)pUserData;
    e_entry_exit_callbacks entryExitCallbacks;
    e_thread_start_callback func;
    void* arg;
    unsigned long result;

    entryExitCallbacks = pStartData->entryExitCallbacks;
    if (entryExitCallbacks.onEntry != NULL) {
        entryExitCallbacks.onEntry(entryExitCallbacks.pUserData);
    }

    /* Make sure we make a copy of the start data here. That way we can free pStartData straight away (it was allocated in e_thread_create()). */
    func = pStartData->func;
    arg  = pStartData->arg;

    /* We should free the data pointer before entering into the start function. That way when e_thread_exit() is called we don't leak. */
    e_free(pStartData, (pStartData->usingCustomAllocator) ? NULL : &pStartData->allocationCallbacks);

    result = (unsigned long)func(arg);

    if (entryExitCallbacks.onExit != NULL) {
        entryExitCallbacks.onExit(entryExitCallbacks.pUserData);
    }

    return result;
}

E_API e_result e_thread_create_ex(e_thread* thr, e_thread_start_callback func, void* arg, const e_entry_exit_callbacks* pEntryExitCallbacks, const e_allocation_callbacks* pAllocationCallbacks)
{
    HANDLE hThread;
    e_thread_start_data_win32* pData;    /* <-- Needs to be allocated on the heap to ensure the data doesn't get trashed before the thread is entered. */
    DWORD threadID; /* Not used. Needed for passing into CreateThread(). Without this it'll fail on Windows 98. */

    if (thr == NULL) {
        return E_INVALID_ARGS;
    }

    *thr = NULL;    /* Safety. */

    if (func == NULL) {
        return E_INVALID_ARGS;
    }

    pData = (e_thread_start_data_win32*)e_malloc(sizeof(*pData), pAllocationCallbacks);   /* <-- This will be freed when e_thread_start_win32() is entered. */
    if (pData == NULL) {
        return E_OUT_OF_MEMORY;
    }

    pData->func = func;
    pData->arg  = arg;

    if (pEntryExitCallbacks != NULL) {
        pData->entryExitCallbacks = *pEntryExitCallbacks;
    } else {
        pData->entryExitCallbacks.onEntry   = NULL;
        pData->entryExitCallbacks.onExit    = NULL;
        pData->entryExitCallbacks.pUserData = NULL;
    }

    if (pAllocationCallbacks != NULL) {
        pData->allocationCallbacks  = *pAllocationCallbacks;
        pData->usingCustomAllocator = 1;
    } else {
        pData->allocationCallbacks.onMalloc  = NULL;
        pData->allocationCallbacks.onRealloc = NULL;
        pData->allocationCallbacks.onFree    = NULL;
        pData->allocationCallbacks.pUserData = NULL;
        pData->usingCustomAllocator = 0;
    }

    hThread = CreateThread(NULL, 0, e_thread_start_win32, pData, 0, &threadID);
    if (hThread == NULL) {
        e_free(pData, pAllocationCallbacks);
        return e_thread_result_from_GetLastError(GetLastError());
    }

    *thr = (e_thread)hThread;

    return E_SUCCESS;
}

E_API e_result e_thread_create(e_thread* thr, e_thread_start_callback func, void* arg)
{
    return e_thread_create_ex(thr, func, arg, NULL, NULL);
}

E_API e_bool32 e_thread_equal(e_thread lhs, e_thread rhs)
{
    /*
    Annoyingly, GetThreadId() is not defined for Windows XP. Need to conditionally enable this. I'm
    not sure how to do this any other way, so I'm falling back to a simple handle comparison. I don't
    think this is right, though. If anybody has any suggestions, let me know.

    TODO: In e_thread_create_ex(), we're getting the threadID from CreateThread() but not using it.
    Could we make use of that? When GetThreadId() is not available, maybe fall back to that?
    */
#if defined(_WIN32_WINNT) && _WIN32_WINNT >= 0x0502
    return GetThreadId((HANDLE)lhs) == GetThreadId((HANDLE)rhs);
#else
    return lhs == rhs;
#endif
}

E_API e_thread e_thread_current(void)
{
    return (e_thread)GetCurrentThread();
}

E_API e_result e_thread_sleep(const struct timespec* duration, struct timespec* remaining)
{
    /*
    Sleeping is annoyingly complicated in C11. Nothing crazy or anything, but it's not just a simple
    millisecond sleep. These are the rules:
    
        * On success, return 0
        * When the sleep is interupted due to a signal, return -1
        * When any other error occurs, return some other negative value.
        * When the sleep is interupted, the `remaining` output parameter needs to be filled out with
          the remaining time.

    In order to detect a signal, we can use SleepEx(). This only has a resolution of 1 millisecond,
    however (this is true for everything on Windows). SleepEx() will return WAIT_IO_COMPLETION if
    some I/O completion event occurs. This is the best we'll get on Windows, I think.

    In order to calculate the value to place into `remaining`, we need to get the time before sleeping
    and then get the time after the sleeping. We'll then have enough information to calculate the
    difference which will be our remining. This is only required when the `remaining` parameter is not
    NULL. Unfortunately we cannot use timespec_get() here because it doesn't have good support with
    MinGW. We'll instead use Windows' high resolution performance counter which is supported back to
    Windows 2000.
    */
    static LARGE_INTEGER frequency;
    LARGE_INTEGER start;
    DWORD sleepResult;
    DWORD sleepMilliseconds;

    if (duration == NULL) {
        return E_INVALID_ARGS;
    }

    start.QuadPart = 0;

    if (remaining != NULL) {
        if (frequency.QuadPart == 0) {
            if (QueryPerformanceFrequency(&frequency) == FALSE) {
                frequency.QuadPart = 0; /* Just to be sure... */
                return E_INVALID_ARGS;
            }
        }

        if (QueryPerformanceCounter(&start) == FALSE) {
            return E_INVALID_ARGS;   /* Failed to retrieve the start time. */
        }
    }

    sleepMilliseconds = (DWORD)((duration->tv_sec * 1000) + (duration->tv_nsec / 1000000));

    /*
    A small, but important detail here. The C11 spec states that thrd_sleep() should sleep for a
    *minimum* of the specified duration. In the above calculation we converted nanoseconds to
    milliseconds, however this requires a division which may truncate a non-zero sub-millisecond
    amount of time. We need to add an extra millisecond to meet the minimum duration requirement if
    indeed we truncated.
    */
    if ((duration->tv_nsec % 1000000) != 0) {
        sleepMilliseconds += 1; /* We truncated a sub-millisecond amount of time. Add an extra millisecond to meet the minimum duration requirement. */
    }
    
    sleepResult = SleepEx(sleepMilliseconds, TRUE); /* <-- Make this sleep alertable so we can detect WAIT_IO_COMPLETION and return -1. */
    if (sleepResult == 0) {
        if (remaining != NULL) {
            remaining->tv_sec  = 0;
            remaining->tv_nsec = 0;
        }

        return E_SUCCESS;
    }

    /*
    Getting here means we didn't sleep for the specified amount of time. We need to fill `remaining`.
    To do this, we need to find out out much time has elapsed and then offset that will the requested
    duration. This is the hard part of the process because we need to convert to and from timespec.
    */
    if (remaining != NULL) {
        LARGE_INTEGER end;
        if (QueryPerformanceCounter(&end)) {
            LARGE_INTEGER elapsed;
            elapsed.QuadPart = end.QuadPart - start.QuadPart;

            /*
            The remaining amount of time is the requested duration, minus the elapsed time. This section warrents an explanation.

            The section below is converting between our performance counters and timespec structures. Just above we calculated the
            amount of the time that has elapsed since sleeping. By subtracting the requested duration from the elapsed duration,
            we'll be left with the remaining duration.

            The first thing we do is convert the requested duration to a LARGE_INTEGER which will be based on the performance counter
            frequency we retrieved earlier. The Windows high performance counters are based on seconds, so a counter divided by the
            frequency will give you the representation in seconds. By multiplying the counter by 1000 before the division by the
            frequency you'll have a result in milliseconds, etc.

            Once the remainder has be calculated based on the high performance counters, it's converted to the timespec structure
            which is just the reverse.
            */
            {
                LARGE_INTEGER durationCounter;
                LARGE_INTEGER remainingCounter;

                durationCounter.QuadPart = ((duration->tv_sec * frequency.QuadPart) + ((duration->tv_nsec * frequency.QuadPart) / 1000000000));
                if (durationCounter.QuadPart > elapsed.QuadPart) {
                    remainingCounter.QuadPart = durationCounter.QuadPart - elapsed.QuadPart;
                } else {
                    remainingCounter.QuadPart = 0;   /* For safety. Ensures we don't go negative. */
                }

                remaining->tv_sec  = (time_t)((remainingCounter.QuadPart * 1)          / frequency.QuadPart);
                remaining->tv_nsec =  (long)(((remainingCounter.QuadPart * 1000000000) / frequency.QuadPart) - (remaining->tv_sec * (LONGLONG)1000000000));
            }
        } else {
            remaining->tv_sec  = 0; /* Just for safety. */
            remaining->tv_nsec = 0;
        }
    }

    if (sleepResult == WAIT_IO_COMPLETION) {
        return E_INTERRUPT;  /* -1 */
    } else {
        return E_INVALID_ARGS;   /* "other negative value if an error occurred." */
    }
}

E_API void e_thread_yield(void)
{
    Sleep(0);
}

E_API void e_thread_exit(int res)
{
    ExitThread((DWORD)res);
}

E_API e_result e_thread_detach(e_thread thr)
{
    /*
    The documentation for thrd_detach() says explicitly that any error should return thrd_error.
    We'll do the same, so make sure e_thread_result_from_GetLastError() is not used here.
    */
    BOOL result;

    result = CloseHandle((HANDLE)thr);
    if (!result) {
        return E_INVALID_ARGS;
    }

    return E_SUCCESS;
}

E_API e_result e_thread_join(e_thread thr, int* res)
{
    /*
    Like thrd_detach(), the documentation for thrd_join() says to return thrd_success or thrd_error.
    Therefore, make sure e_thread_result_from_GetLastError() is not used here.

    In Win32, waiting for the thread to complete and retrieving the result is done as two separate
    steps.
    */

    /* Wait for the thread. */
    if (WaitForSingleObject((HANDLE)thr, INFINITE) == WAIT_FAILED) {
        return E_INVALID_ARGS;   /* Wait failed. */
    }

    /* Retrieve the result code if required. */
    if (res != NULL) {
        DWORD exitCode;
        if (GetExitCodeThread((HANDLE)thr, &exitCode) == FALSE) {
            return E_INVALID_ARGS;
        }

        *res = (int)exitCode;
    }

    /*
    It's not entirely clear from the documentation for thrd_join() as to whether or not the thread
    handle should be closed at this point. I think it makes sense to close it here, as I don't recall
    ever seeing a pattern or joining a thread, and then explicitly closing the thread handle. I think
    joining should be an implicit detach.
    */
    return e_thread_detach(thr);
}


E_API e_result e_mutex_init(e_mutex* mutex, int type)
{
    HANDLE hMutex;

    if (mutex == NULL) {
        return E_INVALID_ARGS;
    }

    /* Initialize the object to zero for safety. */
    mutex->handle = NULL;
    mutex->type   = 0;

    /*
    CreateMutex() will create a thread-aware mutex (allowing recursiveness), whereas an auto-reset
    event (CreateEvent()) is not thread-aware and will deadlock (will not allow recursiveness). In
    Win32 I'm making all mutex's timeable.
    */
    if ((type & E_MUTEX_TYPE_RECURSIVE) != 0) {
        hMutex = CreateMutexA(NULL, FALSE, NULL);
    } else {
        hMutex = CreateEventA(NULL, FALSE, TRUE, NULL);
    }

    if (hMutex == NULL) {
        return e_thread_result_from_GetLastError(GetLastError());
    }

    mutex->handle = (e_handle)hMutex;
    mutex->type   = type;

    return E_SUCCESS;
}

E_API void e_mutex_destroy(e_mutex* mutex)
{
    if (mutex == NULL) {
        return;
    }

    CloseHandle((HANDLE)mutex->handle);
}

E_API e_result e_mutex_lock(e_mutex* mutex)
{
    DWORD result;

    if (mutex == NULL) {
        return E_INVALID_ARGS;
    }

    result = WaitForSingleObject((HANDLE)mutex->handle, INFINITE);
    if (result != WAIT_OBJECT_0) {
        return E_INVALID_ARGS;
    }

    return E_SUCCESS;
}

E_API e_result e_mutex_timedlock(e_mutex* mutex, const struct timespec* time_point)
{
    DWORD result;

    if (mutex == NULL || time_point == NULL) {
        return E_INVALID_ARGS;
    }

    result = WaitForSingleObject((HANDLE)mutex->handle, (DWORD)e_timespec_diff_milliseconds(*time_point, e_timespec_now()));
    if (result != WAIT_OBJECT_0) {
        if (result == WAIT_TIMEOUT) {
            return E_TIMEOUT;
        }

        return E_INVALID_ARGS;
    }

    return E_SUCCESS;
}

E_API e_result e_mutex_trylock(e_mutex* mutex)
{
    DWORD result;

    if (mutex == NULL) {
        return E_INVALID_ARGS;
    }

    result = WaitForSingleObject((HANDLE)mutex->handle, 0);
    if (result != WAIT_OBJECT_0) {
        return E_BUSY;
    }

    return E_SUCCESS;
}

E_API e_result e_mutex_unlock(e_mutex* mutex)
{
    BOOL result;

    if (mutex == NULL) {
        return E_INVALID_ARGS;
    }

    if ((mutex->type & E_MUTEX_TYPE_RECURSIVE) != 0) {
        result = ReleaseMutex((HANDLE)mutex->handle);
    } else {
        result = SetEvent((HANDLE)mutex->handle);
    }

    if (!result) {
        return E_INVALID_ARGS;
    }

    return E_SUCCESS;
}



E_API e_result e_semaphore_init(e_semaphore* sem, int value, int valueMax)
{
    HANDLE hSemaphore;

    if (sem == NULL || valueMax == 0 || value > valueMax) {
        return E_INVALID_ARGS;
    }

    *sem = NULL;

    hSemaphore = CreateSemaphore(NULL, value, valueMax, NULL);
    if (hSemaphore == NULL) {
        return E_INVALID_ARGS;
    }

    *sem = hSemaphore;

    return E_SUCCESS;
}

E_API void e_semaphore_destroy(e_semaphore* sem)
{
    if (sem == NULL) {
        return;
    }

    CloseHandle((HANDLE)*sem);
}

E_API e_result e_semaphore_wait(e_semaphore* sem)
{
    DWORD result;

    if (sem == NULL) {
        return E_INVALID_ARGS;
    }

    result = WaitForSingleObject((HANDLE)*sem, INFINITE);
    if (result != WAIT_OBJECT_0) {
        return E_INVALID_ARGS;
    }

    return E_SUCCESS;
}

E_API e_result e_semaphore_timedwait(e_semaphore* sem, const struct timespec* time_point)
{
    DWORD result;

    if (sem == NULL) {
        return E_INVALID_ARGS;
    }

    result = WaitForSingleObject((HANDLE)*sem, (DWORD)e_timespec_diff_milliseconds(*time_point, e_timespec_now()));
    if (result != WAIT_OBJECT_0) {
        if (result == WAIT_TIMEOUT) {
            return E_TIMEOUT;
        }

        return E_INVALID_ARGS;
    }

    return E_SUCCESS;
}

E_API e_result e_semaphore_post(e_semaphore* sem)
{
    BOOL result;

    if (sem == NULL) {
        return E_INVALID_ARGS;
    }

    result = ReleaseSemaphore((HANDLE)*sem, 1, NULL);
    if (!result) {
        return E_INVALID_ARGS;
    }

    return E_SUCCESS;
}



E_API e_result e_syncevent_init(e_syncevent* evnt)
{
    HANDLE hEvent;

    if (evnt == NULL) {
        return E_INVALID_ARGS;
    }

    *evnt = NULL;

    hEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
    if (hEvent == NULL) {
        return E_INVALID_ARGS;
    }

    *evnt = hEvent;

    return E_SUCCESS;
}

E_API void e_syncevent_destroy(e_syncevent* evnt)
{
    if (evnt == NULL) {
        return;
    }

    CloseHandle((HANDLE)*evnt);
}

E_API e_result e_syncevent_wait(e_syncevent* evnt)
{
    DWORD result;

    if (evnt == NULL) {
        return E_INVALID_ARGS;
    }

    result = WaitForSingleObject((HANDLE)*evnt, INFINITE);
    if (result != WAIT_OBJECT_0) {
        return E_INVALID_ARGS;
    }

    return E_SUCCESS;
}

E_API e_result e_syncevent_timedwait(e_syncevent* evnt, const struct timespec* time_point)
{
    DWORD result;

    if (evnt == NULL) {
        return E_INVALID_ARGS;
    }

    result = WaitForSingleObject((HANDLE)*evnt, (DWORD)e_timespec_diff_milliseconds(*time_point, e_timespec_now()));
    if (result != WAIT_OBJECT_0) {
        if (result == WAIT_TIMEOUT) {
            return E_TIMEOUT;
        }

        return E_INVALID_ARGS;
    }

    return E_SUCCESS;
}

E_API e_result e_syncevent_signal(e_syncevent* evnt)
{
    BOOL result;

    if (evnt == NULL) {
        return E_INVALID_ARGS;
    }

    result = SetEvent((HANDLE)*evnt);
    if (!result) {
        return E_INVALID_ARGS;
    }

    return E_SUCCESS;
}
#endif

/* POSIX */
#if defined(E_POSIX)
#include <pthread.h>
#include <stdlib.h>     /* For malloc(), realloc(), free(). */
#include <errno.h>      /* For errno_t. */
#include <sys/time.h>   /* For timeval. */

#ifndef E_MALLOC
#define E_MALLOC(sz)        malloc(sz)
#endif

#ifndef E_REALLOC
#define E_REALLOC(p, sz)    realloc(p, sz)
#endif

#ifndef E_FREE
#define E_FREE(p)           free(p)
#endif


static int e_thread_result_from_errno(int e)
{
    switch (e)
    {
        case 0:         return E_SUCCESS;
        case ENOMEM:    return E_OUT_OF_MEMORY;
        case ETIME:     return E_TIMEOUT;
        case ETIMEDOUT: return E_TIMEOUT;
        case EBUSY:     return E_BUSY;
    }

    return E_INVALID_ARGS;
}


typedef struct
{
    e_thread_start_callback func;
    void* arg;
    e_entry_exit_callbacks entryExitCallbacks;
    e_allocation_callbacks allocationCallbacks;
    int usingCustomAllocator;
} e_thread_start_data_posix;

static void* e_thread_start_posix(void* pUserData)
{
    e_thread_start_data_posix* pStartData = (e_thread_start_data_posix*)pUserData;
    e_entry_exit_callbacks entryExitCallbacks;
    e_thread_start_callback func;
    void* arg;
    void* result;

    entryExitCallbacks = pStartData->entryExitCallbacks;
    if (entryExitCallbacks.onEntry != NULL) {
        entryExitCallbacks.onEntry(entryExitCallbacks.pUserData);
    }

    /* Make sure we make a copy of the start data here. That way we can free pStartData straight away (it was allocated in e_thread_create()). */
    func = pStartData->func;
    arg  = pStartData->arg;

    /* We should free the data pointer before entering into the start function. That way when e_thread_exit() is called we don't leak. */
    e_free(pStartData, (pStartData->usingCustomAllocator) ? NULL : &pStartData->allocationCallbacks);

    result = (void*)(e_intptr)func(arg);

    if (entryExitCallbacks.onExit != NULL) {
        entryExitCallbacks.onExit(entryExitCallbacks.pUserData);
    }

    return result;
}

E_API e_result e_thread_create_ex(e_thread* thr, e_thread_start_callback func, void* arg, const e_entry_exit_callbacks* pEntryExitCallbacks, const e_allocation_callbacks* pAllocationCallbacks)
{
    int result;
    e_thread_start_data_posix* pData;
    pthread_t thread;

    if (thr == NULL) {
        return E_INVALID_ARGS;
    }

    *thr = 0;   /* Safety. */

    if (func == NULL) {
        return E_INVALID_ARGS;
    }

    pData = (e_thread_start_data_posix*)e_malloc(sizeof(*pData), pAllocationCallbacks);   /* <-- This will be freed when e_thread_start_posix() is entered. */
    if (pData == NULL) {
        return E_OUT_OF_MEMORY;
    }

    pData->func = func;
    pData->arg  = arg;

    if (pEntryExitCallbacks != NULL) {
        pData->entryExitCallbacks = *pEntryExitCallbacks;
    } else {
        pData->entryExitCallbacks.onEntry   = NULL;
        pData->entryExitCallbacks.onExit    = NULL;
        pData->entryExitCallbacks.pUserData = NULL;
    }

    if (pAllocationCallbacks != NULL) {
        pData->allocationCallbacks  = *pAllocationCallbacks;
        pData->usingCustomAllocator = 1;
    } else {
        pData->allocationCallbacks.onMalloc  = NULL;
        pData->allocationCallbacks.onRealloc = NULL;
        pData->allocationCallbacks.onFree    = NULL;
        pData->allocationCallbacks.pUserData = NULL;
        pData->usingCustomAllocator = 0;
    }

    result = pthread_create(&thread, NULL, e_thread_start_posix, pData);
    if (result != 0) {
        e_free(pData, pAllocationCallbacks);
        return e_thread_result_from_errno(errno);
    }

    *thr = thread;

    return E_SUCCESS;
}

E_API e_result e_thread_create(e_thread* thr, e_thread_start_callback func, void* arg)
{
    return e_thread_create_ex(thr, func, arg, NULL, NULL);
}

E_API e_bool32 e_thread_equal(e_thread lhs, e_thread rhs)
{
    return pthread_equal(lhs, rhs);
}

E_API e_thread e_thread_current(void)
{
    return pthread_self();
}

E_API e_result e_thread_sleep(const struct timespec* duration, struct timespec* remaining)
{
    /*
    The documentation for thrd_sleep() mentions nanosleep(), so we'll go ahead and use that if it's
    available. Otherwise we'll fallback to select() and use a similar algorithm to what we use with
    the Windows build. We need to keep in mind the requirement to handle signal interrupts.
    */
    int result;

#if defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 199309L
    result = nanosleep(duration, remaining);
    if (result != 0) {
        if (result == EINTR) {
            return E_INTERRUPT;
        }

        return E_INVALID_ARGS;    
    }
#else
    /*
    We need to fall back to select(). We'll use e_timespec_get() to retrieve the time before and after
    for the purpose of diffing.
    */
    struct timeval tv;
    struct timespec tsBeg;
    struct timespec tsEnd;

    if (duration == NULL) {
        return E_INVALID_ARGS;
    }

    /*
    We need to grab the time before the wait. This will be diff'd with the time after waiting to
    produce the remaining amount.
    */
    if (remaining != NULL) {
        result = e_timespec_get(&tsBeg, TIME_UTC);
        if (result == 0) {
            return E_INVALID_ARGS;   /* Failed to retrieve the start time. */
        }
    }

    tv.tv_sec  = duration->tv_sec;
    tv.tv_usec = duration->tv_nsec / 1000;

    /*
    We need to sleep for the *minimum* of `duration`. Our nanoseconds-to-microseconds conversion
    above may have truncated some nanoseconds, so we'll need to add a microsecond to compensate.
    */
    if ((duration->tv_nsec % 1000) != 0) {
        tv.tv_usec += 1;
        if (tv.tv_usec > 1000000) {
            tv.tv_usec = 0;
            tv.tv_sec += 1;
        }
    }

    result = select(0, NULL, NULL, NULL, &tv);
    if (result == 0) {
        if (remaining != NULL) {
            remaining->tv_sec  = 0;
            remaining->tv_nsec = 0;
        }

        return E_SUCCESS;
    }

    /* Getting here means didn't wait the whole time. We'll need to grab the diff. */
    if (remaining != NULL) {
        if (e_timespec_get(&tsEnd, TIME_UTC) != 0) {
            *remaining = e_timespec_diff(tsEnd, tsBeg);
        } else {
            /* Failed to get the end time, somehow. Shouldn't ever happen. */
            remaining->tv_sec  = 0;
            remaining->tv_nsec = 0;
        }
    }

    if (result == EINTR) {
        return E_INTERRUPT;
    } else {
        return E_INVALID_ARGS;
    }
#endif
    
    return E_SUCCESS;
}

E_API void e_thread_yield(void)
{
    sched_yield();
}

E_API void e_thread_exit(int res)
{
    pthread_exit((void*)(e_intptr)res);
}

E_API e_result e_thread_detach(e_thread thr)
{
    /*
    The documentation for thrd_detach() explicitly says E_SUCCESS if successful or E_INVALID_ARGS
    for any other error. Don't use e_thread_result_from_errno() here.
    */
    int result = pthread_detach(thr);
    if (result != 0) {
        return E_INVALID_ARGS;
    }

    return E_SUCCESS;
}

E_API e_result e_thread_join(e_thread thr, int* res)
{
    /* Same rules apply here as thrd_detach() with respect to the return value. */
    void* retval;
    int result = pthread_join(thr, &retval);
    if (result != 0) {
        return E_INVALID_ARGS;   
    }

    if (res != NULL) {
        *res = (int)(e_intptr)retval;
    }

    return E_SUCCESS;
}



E_API e_result e_mutex_init(e_mutex* mutex, int type)
{
    int result;
    pthread_mutexattr_t attr;   /* For specifying whether or not the mutex is recursive. */

    if (mutex == NULL) {
        return E_INVALID_ARGS;
    }

    pthread_mutexattr_init(&attr);
    if ((type & E_MUTEX_TYPE_RECURSIVE) != 0) {
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    } else {
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);     /* Will deadlock. Consistent with Win32. */
    }

    result = pthread_mutex_init((pthread_mutex_t*)mutex, &attr);
    pthread_mutexattr_destroy(&attr);

    if (result != 0) {
        return E_INVALID_ARGS;
    }

    return E_SUCCESS;
}

E_API void e_mutex_destroy(e_mutex* mutex)
{
    if (mutex == NULL) {
        return;
    }

    pthread_mutex_destroy((pthread_mutex_t*)mutex);
}

E_API e_result e_mutex_lock(e_mutex* mutex)
{
    int result;

    if (mutex == NULL) {
        return E_INVALID_ARGS;
    }

    result = pthread_mutex_lock((pthread_mutex_t*)mutex);
    if (result != 0) {
        return E_INVALID_ARGS;
    }

    return E_SUCCESS;
}


/* I'm not entirely sure what the best wait time would be, so making it configurable. Defaulting to 1 microsecond. */
#ifndef E_TIMEDLOCK_WAIT_TIME_IN_NANOSECONDS
#define E_TIMEDLOCK_WAIT_TIME_IN_NANOSECONDS    1000
#endif

static int e_pthread_mutex_timedlock(pthread_mutex_t* mutex, const struct timespec* time_point)
{
#if defined(__USE_XOPEN2K) && !defined(__APPLE__)
    return pthread_mutex_timedlock((pthread_mutex_t*)mutex, time_point);
#else
    /*
    Fallback implementation for when pthread_mutex_timedlock() is not avaialble. This is just a
    naive loop which waits a bit of time before continuing.
    */
    #if !defined(E_SUPPRESS_MUTEX_TIMEDLOCK_FALLBACK_WARNING) && !defined(__APPLE__)
        #warning pthread_mutex_timedlock() is unavailable. Falling back to a suboptimal implementation. Set _XOPEN_SOURCE to >= 600 to use the native implementation of pthread_mutex_timedlock(). Use E_SUPPRESS_MUTEX_TIMEDLOCK_FALLBACK_WARNING to suppress this warning.
    #endif

    int result;

    if (time_point == NULL) {
        return E_INVALID_ARGS;
    }

    for (;;) {
        result = pthread_mutex_trylock((pthread_mutex_t*)mutex);
        if (result == EBUSY) {
            struct timespec tsNow;
            e_timespec_get(&tsNow, TIME_UTC);

            if (e_timespec_cmp(tsNow, *time_point) > 0) {
                result = ETIMEDOUT;
                break;
            } else {
                /* Have not yet timed out. Need to wait a bit and then try again. */
                e_thread_sleep_timespec(e_timespec_nanoseconds(E_TIMEDLOCK_WAIT_TIME_IN_NANOSECONDS));
                continue;
            }
        } else {
            break;
        }
    }

    if (result == 0) {
        return E_SUCCESS;
    } else {
        if (result == ETIMEDOUT) {
            return E_TIMEOUT;
        } else {
            return E_INVALID_ARGS;
        }
    }
#endif
}

E_API e_result e_mutex_timedlock(e_mutex* mutex, const struct timespec* time_point)
{
    int result;

    if (mutex == NULL) {
        return E_INVALID_ARGS;
    }

    result = e_pthread_mutex_timedlock((pthread_mutex_t*)mutex, time_point);
    if (result != 0) {
        if (result == ETIMEDOUT) {
            return E_TIMEOUT;
        }

        return E_INVALID_ARGS;
    }

    return E_SUCCESS;
}

E_API e_result e_mutex_trylock(e_mutex* mutex)
{
    int result;

    if (mutex == NULL) {
        return E_INVALID_ARGS;
    }

    result = pthread_mutex_trylock((pthread_mutex_t*)mutex);
    if (result != 0) {
        if (result == EBUSY) {
            return E_BUSY;
        }

        return E_INVALID_ARGS;
    }

    return E_SUCCESS;
}

E_API e_result e_mutex_unlock(e_mutex* mutex)
{
    int result;

    if (mutex == NULL) {
        return E_INVALID_ARGS;
    }

    result = pthread_mutex_unlock((pthread_mutex_t*)mutex);
    if (result != 0) {
        return E_INVALID_ARGS;
    }

    return E_SUCCESS;
}



E_API e_result e_semaphore_init(e_semaphore* sem, int value, int valueMax)
{
    int result;

    if (sem == NULL || valueMax == 0 || value > valueMax) {
        return E_INVALID_ARGS;
    }

    sem->value    = value;
    sem->valueMax = valueMax;

    result = pthread_mutex_init((pthread_mutex_t*)&sem->lock, NULL);
    if (result != 0) {
        return e_thread_result_from_errno(result);  /* Failed to create mutex. */
    }

    result = pthread_cond_init((pthread_cond_t*)&sem->cond, NULL);
    if (result != 0) {
        pthread_mutex_destroy((pthread_mutex_t*)&sem->lock);
        return e_thread_result_from_errno(result);  /* Failed to create condition variable. */
    }

    return E_SUCCESS;
}

E_API void e_semaphore_destroy(e_semaphore* sem)
{
    if (sem == NULL) {
        return;
    }

    pthread_cond_destroy((pthread_cond_t*)&sem->cond);
    pthread_mutex_destroy((pthread_mutex_t*)&sem->lock);
}

E_API e_result e_semaphore_wait(e_semaphore* sem)
{
    int result;

    if (sem == NULL) {
        return E_INVALID_ARGS;
    }

    result = pthread_mutex_lock((pthread_mutex_t*)&sem->lock);
    if (result != 0) {
        return E_INVALID_ARGS;
    }

    /* We need to wait on a condition variable before escaping. We can't return from this function until the semaphore has been signaled. */
    while (sem->value == 0) {
        pthread_cond_wait((pthread_cond_t*)&sem->cond, (pthread_mutex_t*)&sem->lock);
    }

    sem->value -= 1;
    pthread_mutex_unlock((pthread_mutex_t*)&sem->lock);

    return E_SUCCESS;
}

E_API e_result e_semaphore_timedwait(e_semaphore* sem, const struct timespec* time_point)
{
    int result;

    if (sem == NULL) {
        return E_INVALID_ARGS;
    }

    result = e_pthread_mutex_timedlock((pthread_mutex_t*)&sem->lock, time_point);
    if (result != 0) {
        if (result == ETIMEDOUT) {
            return E_TIMEOUT;
        }

        return E_INVALID_ARGS;
    }

    /* We need to wait on a condition variable before escaping. We can't return from this function until the semaphore has been signaled. */
    while (sem->value == 0) {
        result = pthread_cond_timedwait((pthread_cond_t*)&sem->cond, (pthread_mutex_t*)&sem->lock, time_point);
        if (result == ETIMEDOUT) {
            pthread_mutex_unlock((pthread_mutex_t*)&sem->lock);
            return E_TIMEOUT;
        }
    }

    sem->value -= 1;

    pthread_mutex_unlock((pthread_mutex_t*)&sem->lock);
    return E_SUCCESS;
}

E_API e_result e_semaphore_post(e_semaphore* sem)
{
    int result;

    if (sem == NULL) {
        return E_INVALID_ARGS;
    }

    result = pthread_mutex_lock((pthread_mutex_t*)&sem->lock);
    if (result != 0) {
        return E_INVALID_ARGS;
    }

    if (sem->value < sem->valueMax) {
        sem->value += 1;
        pthread_cond_signal((pthread_cond_t*)&sem->cond);
        result = E_SUCCESS;
    } else {
        result = E_INVALID_ARGS;
    }

    pthread_mutex_unlock((pthread_mutex_t*)&sem->lock);
    return result;
}



E_API e_result e_syncevent_init(e_syncevent* evnt)
{
    int result;

    if (evnt == NULL) {
        return E_INVALID_ARGS;
    }

    evnt->value = 0;

    result = pthread_mutex_init((pthread_mutex_t*)&evnt->lock, NULL);
    if (result != 0) {
        return e_thread_result_from_errno(result);  /* Failed to create mutex. */
    }

    result = pthread_cond_init((pthread_cond_t*)&evnt->cond, NULL);
    if (result != 0) {
        pthread_mutex_destroy((pthread_mutex_t*)&evnt->lock);
        return e_thread_result_from_errno(result);  /* Failed to create condition variable. */
    }

    return E_SUCCESS;
}

E_API void e_syncevent_destroy(e_syncevent* evnt)
{
    if (evnt == NULL) {
        return;
    }

    pthread_cond_destroy((pthread_cond_t*)&evnt->cond);
    pthread_mutex_destroy((pthread_mutex_t*)&evnt->lock);
}

E_API e_result e_syncevent_wait(e_syncevent* evnt)
{
    int result;

    if (evnt == NULL) {
        return E_INVALID_ARGS;
    }

    result = pthread_mutex_lock((pthread_mutex_t*)&evnt->lock);
    if (result != 0) {
        return E_INVALID_ARGS;
    }

    while (evnt->value == 0) {
        pthread_cond_wait((pthread_cond_t*)&evnt->cond, (pthread_mutex_t*)&evnt->lock);
    }
    evnt->value = 0;  /* Auto-reset. */

    pthread_mutex_unlock((pthread_mutex_t*)&evnt->lock);
    return E_SUCCESS;
}

E_API e_result e_syncevent_timedwait(e_syncevent* evnt, const struct timespec* time_point)
{
    int result;

    if (evnt == NULL) {
        return E_INVALID_ARGS;
    }

    result = e_pthread_mutex_timedlock((pthread_mutex_t*)&evnt->lock, time_point);
    if (result != 0) {
        if (result == ETIMEDOUT) {
            return E_TIMEOUT;
        }

        return E_INVALID_ARGS;
    }

    while (evnt->value == 0) {
        result = pthread_cond_timedwait((pthread_cond_t*)&evnt->cond, (pthread_mutex_t*)&evnt->lock, time_point);
        if (result == ETIMEDOUT) {
            pthread_mutex_unlock((pthread_mutex_t*)&evnt->lock);
            return E_TIMEOUT;
        }
    }
    evnt->value = 0;  /* Auto-reset. */

    pthread_mutex_unlock((pthread_mutex_t*)&evnt->lock);
    return E_SUCCESS;
}

E_API e_result e_syncevent_signal(e_syncevent* evnt)
{
    int result;

    if (evnt == NULL) {
        return E_INVALID_ARGS;
    }

    result = pthread_mutex_lock((pthread_mutex_t*)&evnt->lock);
    if (result != 0) {
        return E_INVALID_ARGS;
    }

    evnt->value = 1;
    pthread_cond_signal((pthread_cond_t*)&evnt->cond);

    pthread_mutex_unlock((pthread_mutex_t*)&evnt->lock);
    return E_SUCCESS;
}
#endif
/* END e_thread.c */



/* BEG e_stream.c */
E_API e_result e_stream_init(const e_stream_vtable* pVTable, e_stream* pStream)
{
    if (pStream == NULL) {
        return E_INVALID_ARGS;
    }

    pStream->pVTable = pVTable;

    if (pVTable == NULL) {
        return E_INVALID_ARGS;
    }

    return E_SUCCESS;
}

E_API e_result e_stream_read(e_stream* pStream, void* pDst, size_t bytesToRead, size_t* pBytesRead)
{
    size_t bytesRead;
    e_result result;

    if (pBytesRead != NULL) {
        *pBytesRead = 0;
    }

    if (pStream == NULL) {
        return E_INVALID_ARGS;
    }

    if (pStream->pVTable->read == NULL) {
        return E_NOT_IMPLEMENTED;
    }

    bytesRead = 0;
    result = pStream->pVTable->read(pStream, pDst, bytesToRead, &bytesRead);

    if (pBytesRead != NULL) {
        *pBytesRead = bytesRead;
    } else {
        /*
        The caller has not specified a destination for the bytes read. If we didn't output the exact
        number of bytes as requested we'll need to report an error.
        */
        if (result == E_SUCCESS && bytesRead != bytesToRead) {
            result = E_ERROR;
        }
    }

    return result;
}

E_API e_result e_stream_write(e_stream* pStream, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten)
{
    size_t bytesWritten;
    e_result result;

    if (pBytesWritten != NULL) {
        *pBytesWritten = 0;
    }

    if (pStream == NULL) {
        return E_INVALID_ARGS;
    }

    if (pStream->pVTable->write == NULL) {
        return E_NOT_IMPLEMENTED;
    }

    bytesWritten = 0;
    result = pStream->pVTable->write(pStream, pSrc, bytesToWrite, &bytesWritten);

    if (pBytesWritten != NULL) {
        *pBytesWritten = bytesWritten;
    }

    return result;
}

E_API e_result e_stream_writef(e_stream* pStream, const char* fmt, ...)
{
    va_list args;
    e_result result;

    va_start(args, fmt);
    result = e_stream_writefv(pStream, fmt, args);
    va_end(args);

    return result;
}

E_API e_result e_stream_writef_ex(e_stream* pStream, const e_allocation_callbacks* pAllocationCallbacks, const char* fmt, ...)
{
    va_list args;
    e_result result;

    va_start(args, fmt);
    result = e_stream_writefv_ex(pStream, pAllocationCallbacks, fmt, args);
    va_end(args);

    return result;
}

E_API e_result e_stream_writefv(e_stream* pStream, const char* fmt, va_list args)
{
    return e_stream_writefv_ex(pStream, NULL, fmt, args);
}

E_API e_result e_stream_writefv_ex(e_stream* pStream, const e_allocation_callbacks* pAllocationCallbacks, const char* fmt, va_list args)
{
    e_result result;
    int strLen;
    char pStrStack[1024];
    va_list args2;

    if (pStream == NULL || fmt == NULL) {
        return E_INVALID_ARGS;
    }

    e_va_copy(args2, args);
    {
        strLen = e_vsnprintf(pStrStack, sizeof(pStrStack), fmt, args2);
    }
    va_end(args2);

    if (strLen < 0) {
        return E_ERROR;    /* Encoding error. */
    }

    if (strLen < (int)sizeof(pStrStack)) {
        /* Stack buffer is big enough. Output straight to the file. */
        result = e_stream_write(pStream, pStrStack, strLen, NULL);
    } else {
        /* Stack buffer is not big enough. Allocate space on the heap. */
        char* pStrHeap = NULL;

        pStrHeap = (char*)e_malloc(strLen + 1, pAllocationCallbacks);
        if (pStrHeap == NULL) {
            return E_OUT_OF_MEMORY;
        }

        e_vsnprintf(pStrHeap, strLen + 1, fmt, args);
        result = e_stream_write(pStream, pStrHeap, strLen, NULL);

        e_free(pStrHeap, pAllocationCallbacks);
    }

    return result;
}

E_API e_result e_stream_seek(e_stream* pStream, e_int64 offset, e_seek_origin origin)
{
    if (pStream == NULL) {
        return E_INVALID_ARGS;
    }

    if (pStream->pVTable->seek == NULL) {
        return E_NOT_IMPLEMENTED;
    }

    return pStream->pVTable->seek(pStream, offset, origin);
}

E_API e_result e_stream_tell(e_stream* pStream, e_int64* pCursor)
{
    if (pCursor == NULL) {
        return E_INVALID_ARGS;  /* It does not make sense to call this without a variable to receive the cursor position. */
    }

    *pCursor = 0;   /* <-- In case an error happens later. */

    if (pStream == NULL) {
        return E_INVALID_ARGS;
    }

    if (pStream->pVTable->tell == NULL) {
        return E_NOT_IMPLEMENTED;
    }

    return pStream->pVTable->tell(pStream, pCursor);
}

E_API e_result e_stream_duplicate(e_stream* pStream, const e_allocation_callbacks* pAllocationCallbacks, e_stream** ppDuplicatedStream)
{
    e_result result;
    e_stream* pDuplicatedStream;

    if (ppDuplicatedStream == NULL) {
        return E_INVALID_ARGS;
    }

    *ppDuplicatedStream = NULL;

    if (pStream == NULL) {
        return E_INVALID_ARGS;
    }

    if (pStream->pVTable->duplicate_alloc_size == NULL || pStream->pVTable->duplicate == NULL) {
        return E_NOT_IMPLEMENTED;
    }

    pDuplicatedStream = (e_stream*)e_calloc(pStream->pVTable->duplicate_alloc_size(pStream), pAllocationCallbacks);
    if (pDuplicatedStream == NULL) {
        return E_OUT_OF_MEMORY;
    }

    result = e_stream_init(pStream->pVTable, pDuplicatedStream);
    if (result != E_SUCCESS) {
        e_free(pDuplicatedStream, pAllocationCallbacks);
        return result;
    }

    result = pStream->pVTable->duplicate(pStream, pDuplicatedStream);
    if (result != E_SUCCESS) {
        e_free(pDuplicatedStream, pAllocationCallbacks);
        return result;
    }

    *ppDuplicatedStream = pDuplicatedStream;

    return E_SUCCESS;
}

E_API void e_stream_delete_duplicate(e_stream* pDuplicatedStream, const e_allocation_callbacks* pAllocationCallbacks)
{
    if (pDuplicatedStream == NULL) {
        return;
    }

    if (pDuplicatedStream->pVTable->uninit != NULL) {
        pDuplicatedStream->pVTable->uninit(pDuplicatedStream);
    }

    e_free(pDuplicatedStream, pAllocationCallbacks);
}


E_API e_result e_stream_read_to_end(e_stream* pStream, e_stream_data_format format, const e_allocation_callbacks* pAllocationCallbacks, void** ppData, size_t* pDataSize)
{
    e_result result = E_SUCCESS;
    size_t dataSize = 0;
    size_t dataCap  = 0;
    void* pData = NULL;

    if (ppData != NULL) {
        *ppData = NULL;
    }
    if (pDataSize != NULL) {
        *pDataSize = 0;
    }

    if (pStream == NULL || ppData == NULL) {
        return E_INVALID_ARGS;
    }

    /* Read in a loop into a dynamically increasing buffer. */
    for (;;) {
        size_t chunkSize = 4096;
        size_t bytesRead;

        if (dataSize + chunkSize > dataCap) {
            void* pNewData;
            size_t newCap = dataCap * 2;
            if (newCap == 0) {
                newCap = chunkSize;
            }

            pNewData = e_realloc(pData, newCap, pAllocationCallbacks);
            if (pNewData == NULL) {
                e_free(pData, pAllocationCallbacks);
                return E_OUT_OF_MEMORY;
            }

            pData = pNewData;
            dataCap = newCap;
        }

        /* At this point there should be enough data in the buffer for the next chunk. */
        result = e_stream_read(pStream, E_OFFSET_PTR(pData, dataSize), chunkSize, &bytesRead);
        dataSize += bytesRead;

        if (result != E_SUCCESS || bytesRead < chunkSize) {
            break;
        }
    }

    /* If we're opening in text mode, we need to append a null terminator. */
    if (format == E_STREAM_DATA_FORMAT_TEXT) {
        if (dataSize >= dataCap) {
            void* pNewData;
            pNewData = e_realloc(pData, dataSize + 1, pAllocationCallbacks);
            if (pNewData == NULL) {
                e_free(pData, pAllocationCallbacks);
                return E_OUT_OF_MEMORY;
            }

            pData = pNewData;
        }

        ((char*)pData)[dataSize] = '\0';
    }

    *ppData = pData;

    if (pDataSize != NULL) {
        *pDataSize = dataSize;
    }

    /* Make sure the caller is aware of any errors. */
    if (result != E_SUCCESS && result != E_AT_END) {
        return result;
    } else {
        return E_SUCCESS;
    }
}
/* END e_stream.c */


/* BEG e_memory_stream.c */
static e_result e_memory_stream_read_internal(e_stream* pStream, void* pDst, size_t bytesToRead, size_t* pBytesRead)
{
    return e_memory_stream_read((e_memory_stream*)pStream, pDst, bytesToRead, pBytesRead);
}

static e_result e_memory_stream_write_internal(e_stream* pStream, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten)
{
    return e_memory_stream_write((e_memory_stream*)pStream, pSrc, bytesToWrite, pBytesWritten);
}

static e_result e_memory_stream_seek_internal(e_stream* pStream, e_int64 offset, e_seek_origin origin)
{
    return e_memory_stream_seek((e_memory_stream*)pStream, offset, origin);
}

static e_result e_memory_stream_tell_internal(e_stream* pStream, e_int64* pCursor)
{
    e_result result;
    size_t cursor;

    result = e_memory_stream_tell((e_memory_stream*)pStream, &cursor);
    if (result != E_SUCCESS) {
        return result;
    }

    if (cursor > E_INT64_MAX) {    /* <-- INT64_MAX may not be defined on some compilers. Need to check this. Can easily define this ourselves. */
        return E_ERROR;
    }

    *pCursor = (e_int64)cursor;

    return E_SUCCESS;
}

static size_t e_memory_stream_duplicate_alloc_size_internal(e_stream* pStream)
{
    (void)pStream;
    return sizeof(e_memory_stream);
}

static e_result e_memory_stream_duplicate_internal(e_stream* pStream, e_stream* pDuplicatedStream)
{
    e_memory_stream* pMemoryStream;

    pMemoryStream = (e_memory_stream*)pStream;
    E_ASSERT(pMemoryStream != NULL);

    *pDuplicatedStream = *pStream;

    /* Slightly special handling for write mode. Need to make a copy of the output buffer. */
    if (pMemoryStream->write.pData != NULL) {
        void* pNewData = e_malloc(pMemoryStream->write.dataCap, &pMemoryStream->allocationCallbacks);
        if (pNewData == NULL) {
            return E_OUT_OF_MEMORY;
        }

        E_COPY_MEMORY(pNewData, pMemoryStream->write.pData, pMemoryStream->write.dataSize);

        pMemoryStream->write.pData = pNewData;

        pMemoryStream->ppData    = &pMemoryStream->write.pData;
        pMemoryStream->pDataSize = &pMemoryStream->write.dataSize;
    } else {
        pMemoryStream->ppData    = (void**)&pMemoryStream->readonly.pData;
        pMemoryStream->pDataSize = &pMemoryStream->readonly.dataSize;
    }

    return E_SUCCESS;
}

static void e_memory_stream_uninit_internal(e_stream* pStream)
{
    e_memory_stream_uninit((e_memory_stream*)pStream);
}

static e_stream_vtable e_gStreamVTableMemory =
{
    e_memory_stream_read_internal,
    e_memory_stream_write_internal,
    e_memory_stream_seek_internal,
    e_memory_stream_tell_internal,
    e_memory_stream_duplicate_alloc_size_internal,
    e_memory_stream_duplicate_internal,
    e_memory_stream_uninit_internal
};


E_API e_result e_memory_stream_init_write(const e_allocation_callbacks* pAllocationCallbacks, e_memory_stream* pStream)
{
    e_result result;

    if (pStream == NULL) {
        return E_INVALID_ARGS;
    }

    E_ZERO_OBJECT(pStream);

    result = e_stream_init(&e_gStreamVTableMemory, &pStream->base);
    if (result != E_SUCCESS) {
        return result;
    }

    pStream->write.pData    = NULL;
    pStream->write.dataSize = 0;
    pStream->write.dataCap  = 0;
    pStream->allocationCallbacks = e_allocation_callbacks_init_copy(pAllocationCallbacks);

    pStream->ppData    = &pStream->write.pData;
    pStream->pDataSize = &pStream->write.dataSize;

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

    result = e_stream_init(&e_gStreamVTableMemory, &pStream->base);
    if (result != E_SUCCESS) {
        return result;
    }

    pStream->readonly.pData    = pData;
    pStream->readonly.dataSize = dataSize;

    pStream->ppData    = (void**)&pStream->readonly.pData;
    pStream->pDataSize = &pStream->readonly.dataSize;

    return E_SUCCESS;
}

E_API void e_memory_stream_uninit(e_memory_stream* pStream)
{
    if (pStream == NULL) {
        return;
    }

    if (pStream->write.pData != NULL) {
        e_free(pStream->write.pData, &pStream->allocationCallbacks);
    }
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
    if (newSize > pStream->write.dataCap) {
        /* Need to resize. */
        void* pNewBuffer;
        size_t newCap;

        newCap = E_MAX(newSize, pStream->write.dataCap * 2);
        pNewBuffer = e_realloc(*pStream->ppData, newCap, &pStream->allocationCallbacks);
        if (pNewBuffer == NULL) {
            return E_OUT_OF_MEMORY;
        }

        *pStream->ppData = pNewBuffer;
        pStream->write.dataCap = newCap;
    }

    E_ASSERT(newSize <= pStream->write.dataCap);

    E_COPY_MEMORY(E_OFFSET_PTR(*pStream->ppData, *pStream->pDataSize), pSrc, bytesToWrite);
    *pStream->pDataSize = newSize;

    if (pBytesWritten != NULL) {
        *pBytesWritten = bytesToWrite;  /* We always write all or nothing here. */
    }

    return E_SUCCESS;
}

E_API e_result e_memory_stream_seek(e_memory_stream* pStream, e_int64 offset, int origin)
{
    e_int64 newCursor;

    if (pStream == NULL) {
        return E_INVALID_ARGS;
    }

    if ((e_uint64)E_ABS(offset) > E_SIZE_MAX) {
        return E_INVALID_ARGS;  /* Trying to seek too far. This will never happen on 64-bit builds. */
    }

    newCursor = pStream->cursor;

    if (origin == E_SEEK_SET) {
        newCursor = 0;
    } else if (origin == E_SEEK_CUR) {
        newCursor = (e_int64)pStream->cursor;
    } else if (origin == E_SEEK_END) {
        newCursor = (e_int64)*pStream->pDataSize;
    } else {
        E_ASSERT(!"Invalid seek origin");
        return E_INVALID_ARGS;
    }

    newCursor += offset;

    if (newCursor < 0) {
        return E_BAD_SEEK;  /* Trying to seek prior to the start of the buffer. */
    }
    if ((size_t)newCursor > *pStream->pDataSize) {
        return E_BAD_SEEK;  /* Trying to seek beyond the end of the buffer. */
    }

    pStream->cursor = (size_t)newCursor;

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

E_API void* e_memory_stream_take_ownership(e_memory_stream* pStream, size_t* pSize)
{
    void* pData;

    if (pStream == NULL) {
        return NULL;
    }

    pData = *pStream->ppData;
    if (pSize != NULL) {
        *pSize = *pStream->pDataSize;
    }

    pStream->write.pData    = NULL;
    pStream->write.dataSize = 0;
    pStream->write.dataCap  = 0;

    return pData;

}
/* END e_memory_stream.c */



/* BEG e_path.c */
E_API e_result e_path_first(const char* pPath, size_t pathLen, e_path_iterator* pIterator)
{
    if (pIterator == NULL) {
        return E_INVALID_ARGS;
    }

    E_ZERO_OBJECT(pIterator);

    if (pPath == NULL || pPath[0] == '\0' || pathLen == 0) {
        return E_INVALID_ARGS;
    }

    pIterator->pFullPath      = pPath;
    pIterator->fullPathLength = pathLen;
    pIterator->segmentOffset  = 0;
    pIterator->segmentLength  = 0;

    /* We need to find the first separator, or the end of the string. */
    while (pIterator->segmentLength < pathLen && pPath[pIterator->segmentLength] != '\0' && (pPath[pIterator->segmentLength] != '\\' && pPath[pIterator->segmentLength] != '/')) {
        pIterator->segmentLength += 1;
    }

    return E_SUCCESS;
}

E_API e_result e_path_last(const char* pPath, size_t pathLen, e_path_iterator* pIterator)
{
    if (pIterator == NULL) {
        return E_INVALID_ARGS;
    }

    E_ZERO_OBJECT(pIterator);

    if (pathLen == 0 || pPath == NULL || pPath[0] == '\0') {
        return E_INVALID_ARGS;
    }

    if (pathLen == (size_t)-1) {
        pathLen = strlen(pPath);
    }

    /* Little trick here. Not *quite* as optimal as it could be, but just go to the end of the string, and then go to the previous segment. */
    pIterator->pFullPath      = pPath;
    pIterator->fullPathLength = pathLen;
    pIterator->segmentOffset  = pathLen;
    pIterator->segmentLength  = 0;

    /* We need to find the last separator, or the beginning of the string. */
    while (pIterator->segmentLength < pathLen && pPath[pIterator->segmentOffset - 1] != '\0' && (pPath[pIterator->segmentOffset - 1] != '\\' && pPath[pIterator->segmentOffset - 1] != '/')) {
        pIterator->segmentOffset -= 1;
        pIterator->segmentLength += 1;
    }

    return E_SUCCESS;
}

E_API e_result e_path_next(e_path_iterator* pIterator)
{
    if (pIterator == NULL) {
        return E_INVALID_ARGS;
    }

    E_ASSERT(pIterator->pFullPath != NULL);

    /* Move the offset to the end of the previous segment and reset the length. */
    pIterator->segmentOffset = pIterator->segmentOffset + pIterator->segmentLength;
    pIterator->segmentLength = 0;

    /* If we're at the end of the string, we're done. */
    if (pIterator->segmentOffset >= pIterator->fullPathLength || pIterator->pFullPath[pIterator->segmentOffset] == '\0') {
        return E_AT_END;
    }

    /* At this point we should be sitting on a separator. The next character starts the next segment. */
    pIterator->segmentOffset += 1;

    /* Now we need to find the next separator or the end of the path. This will be the end of the segment. */
    for (;;) {
        if (pIterator->segmentOffset + pIterator->segmentLength >= pIterator->fullPathLength || pIterator->pFullPath[pIterator->segmentOffset + pIterator->segmentLength] == '\0') {
            break;  /* Reached the end of the path. */
        }

        if (pIterator->pFullPath[pIterator->segmentOffset + pIterator->segmentLength] == '\\' || pIterator->pFullPath[pIterator->segmentOffset + pIterator->segmentLength] == '/') {
            break;  /* Found a separator. This marks the end of the next segment. */
        }

        pIterator->segmentLength += 1;
    }

    return E_SUCCESS;
}

E_API e_result e_path_prev(e_path_iterator* pIterator)
{
    if (pIterator == NULL) {
        return E_INVALID_ARGS;
    }

    E_ASSERT(pIterator->pFullPath != NULL);

    if (pIterator->segmentOffset == 0) {
        return E_AT_END;  /* If we're already at the start it must mean we're finished iterating. */
    }

    pIterator->segmentLength = 0;

    /*
    The start of the segment of the current iterator should be sitting just before a separator. We
    need to move backwards one step. This will become the end of the segment we'll be returning.
    */
    pIterator->segmentOffset = pIterator->segmentOffset - 1;
    pIterator->segmentLength = 0;

    /* Just keep scanning backwards until we find a separator or the start of the path. */
    for (;;) {
        if (pIterator->segmentOffset == 0) {
            break;
        }

        if (pIterator->pFullPath[pIterator->segmentOffset - 1] == '\\' || pIterator->pFullPath[pIterator->segmentOffset - 1] == '/') {
            break;
        }

        pIterator->segmentOffset -= 1;
        pIterator->segmentLength += 1;
    }

    return E_SUCCESS;
}

E_API e_bool32 e_path_is_first(const e_path_iterator* pIterator)
{
    if (pIterator == NULL) {
        return E_FALSE;
    }

    return pIterator->segmentOffset == 0;

}

E_API e_bool32 e_path_is_last(const e_path_iterator* pIterator)
{
    if (pIterator == NULL) {
        return E_FALSE;
    }

    if (pIterator->fullPathLength == E_NULL_TERMINATED) {
        return pIterator->pFullPath[pIterator->segmentOffset + pIterator->segmentLength] == '\0';
    } else {
        return pIterator->segmentOffset + pIterator->segmentLength == pIterator->fullPathLength;
    }
}

E_API int e_path_iterators_compare(const e_path_iterator* pIteratorA, const e_path_iterator* pIteratorB)
{
    E_ASSERT(pIteratorA != NULL);
    E_ASSERT(pIteratorB != NULL);

    if (pIteratorA->pFullPath == pIteratorB->pFullPath && pIteratorA->segmentOffset == pIteratorB->segmentOffset && pIteratorA->segmentLength == pIteratorB->segmentLength) {
        return 0;
    }

    return e_strncmp(pIteratorA->pFullPath + pIteratorA->segmentOffset, pIteratorB->pFullPath + pIteratorB->segmentOffset, E_MIN(pIteratorA->segmentLength, pIteratorB->segmentLength));
}

E_API int e_path_compare(const char* pPathA, size_t pathALen, const char* pPathB, size_t pathBLen)
{
    e_path_iterator iPathA;
    e_path_iterator iPathB;
    e_result result;

    if (pPathA == NULL && pPathB == NULL) {
        return 0;
    }

    if (pPathA == NULL) {
        return -1;
    }
    if (pPathB == NULL) {
        return +1;
    }

    result = e_path_first(pPathA, pathALen, &iPathA);
    if (result != E_SUCCESS) {
        return -1;
    }

    result = e_path_first(pPathB, pathBLen, &iPathB);
    if (result != E_SUCCESS) {
        return +1;
    }

    /* We just keep iterating until we find a mismatch or reach the end of one of the paths. */
    for (;;) {
        int cmp;

        cmp = e_path_iterators_compare(&iPathA, &iPathB);
        if (cmp != 0) {
            return cmp;
        }

        if (e_path_is_last(&iPathA) && e_path_is_last(&iPathB)) {
            return 0;   /* Both paths are the same. */
        }

        result = e_path_next(&iPathA);
        if (result != E_SUCCESS) {
            return -1;
        }

        result = e_path_next(&iPathB);
        if (result != E_SUCCESS) {
            return +1;
        }
    }

    return 0;
}

E_API const char* e_path_file_name(const char* pPath, size_t pathLen)
{
    /* The file name is just the last segment. */
    e_result result;
    e_path_iterator last;

    result = e_path_last(pPath, pathLen, &last);
    if (result != E_SUCCESS) {
        return NULL;
    }

    if (last.segmentLength == 0) {
        return NULL;
    }
    
    return last.pFullPath + last.segmentOffset;
}

E_API int e_path_directory(char* pDst, size_t dstCap, const char* pPath, size_t pathLen)
{
    const char* pFileName;

    pFileName = e_path_file_name(pPath, pathLen);
    if (pFileName == NULL) {
        return -1;
    }

    if (pFileName == pPath) {
        if (pDst != NULL && dstCap > 0) {
            pDst[0] = '\0';
        }

        return 0;   /* The path is just a file name. */
    } else {
        const char* pDirEnd = pFileName - 1;
        size_t dirLen = (size_t)(pDirEnd - pPath);

        if (pDst != NULL && dstCap > 0) {
            size_t bytesToCopy = E_MIN(dstCap - 1, dirLen);
            if (bytesToCopy > 0 && pDst != pPath) {
                E_MOVE_MEMORY(pDst, pPath, bytesToCopy);
            }

            pDst[bytesToCopy] = '\0';
        }

        if (dirLen > (size_t)-1) {
            return -1;  /* Too long. */
        }

        return (int)dirLen;
    }
}

E_API const char* e_path_extension(const char* pPath, size_t pathLen)
{
    const char* pDot = NULL;
    const char* pLastSlash = NULL;
    size_t i;

    if (pPath == NULL) {
        return NULL;
    }

    /* We need to find the last dot after the last slash. */
    for (i = 0; i < pathLen; ++i) {
        if (pPath[i] == '\0') {
            break;
        }

        if (pPath[i] == '.') {
            pDot = pPath + i;
        } else if (pPath[i] == '\\' || pPath[i] == '/') {
            pLastSlash = pPath + i;
        }
    }

    /* If the last dot is after the last slash, we've found it. Otherwise, it's not there and we need to return null. */
    if (pDot != NULL && pDot > pLastSlash) {
        return pDot + 1;
    } else {
        return NULL;
    }
}

E_API e_bool32 e_path_extension_equal(const char* pPath, size_t pathLen, const char* pExtension, size_t extensionLen)
{
    if (pPath == NULL || pExtension == NULL) {
        return E_FALSE;
    }

    if (extensionLen == E_NULL_TERMINATED) {
        extensionLen = strlen(pExtension);
    }

    if (pathLen == E_NULL_TERMINATED) {
        pathLen = strlen(pPath);
    }

    if (extensionLen >= pathLen) {
        return E_FALSE;
    }

    if (pPath[pathLen - extensionLen - 1] != '.') {
        return E_FALSE;
    }

    return e_strnicmp(pPath + pathLen - extensionLen, pExtension, extensionLen) == 0;
}

E_API const char* e_path_trim_base(const char* pPath, size_t pathLen, const char* pBasePath, size_t basePathLen)
{
    e_path_iterator iPath;
    e_path_iterator iBase;
    e_result result;

    if (basePathLen != E_NULL_TERMINATED && pathLen < basePathLen) {
        return NULL;
    }

    if (basePathLen == 0 || pBasePath == NULL || pBasePath[0] == '\0') {
        return pPath;
    }

    result = e_path_first(pPath, pathLen, &iPath);
    if (result != E_SUCCESS) {
        return NULL;
    }

    result = e_path_first(pBasePath, basePathLen, &iBase);
    if (result != E_SUCCESS) {
        return NULL;
    }

    /* We just keep iterating until we find a mismatch or reach the end of the base path. */
    for (;;) {
        if (iPath.segmentLength != iBase.segmentLength) {
            return NULL;
        }

        if (e_strncmp(iPath.pFullPath + iPath.segmentOffset, iBase.pFullPath + iBase.segmentOffset, iPath.segmentLength) != 0) {
            return NULL;
        }

        result = e_path_next(&iBase);
        if (result != E_SUCCESS) {
            e_path_next(&iPath);   /* Move to the next segment in the path to ensure our iterators are in sync. */
            break;
        }

        result = e_path_next(&iPath);
        if (result != E_SUCCESS) {
            return NULL;    /* If we hit this it means the we've reached the end of the path before the base and therefore we don't match. */
        }
    }

    /* Getting here means we got to the end of the base path without finding a mismatched segment which means the path begins with the base. */
    return iPath.pFullPath + iPath.segmentOffset;
}

E_API e_bool32 e_path_begins_with(const char* pPath, size_t pathLen, const char* pBasePath, size_t basePathLen)
{
    return e_path_trim_base(pPath, pathLen, pBasePath, basePathLen) != NULL;
}

E_API int e_path_append(char* pDst, size_t dstCap, const char* pBasePath, size_t basePathLen, const char* pPathToAppend, size_t pathToAppendLen)
{
    size_t dstLen = 0;

    if (pBasePath == NULL) {
        pBasePath = "";
        basePathLen = 0;
    }

    if (pPathToAppend == NULL) {
        pPathToAppend = "";
        pathToAppendLen = 0;
    }

    if (basePathLen == E_NULL_TERMINATED) {
        basePathLen = strlen(pBasePath);
    }

    if (pathToAppendLen == E_NULL_TERMINATED) {
        pathToAppendLen = strlen(pPathToAppend);
    }


    /* Do not include the separator if we have one. */
    if (basePathLen > 0 && (pBasePath[basePathLen - 1] == '\\' || pBasePath[basePathLen - 1] == '/')) {
        basePathLen -= 1;
    }


    /*
    We don't want to be appending a separator if the base path is empty. Otherwise we'll end up with
    a leading slash.
    */
    if (basePathLen > 0) {
        /* Base path. */
        if (pDst != NULL) {
            size_t bytesToCopy = E_MIN(basePathLen, dstCap);
            
            if (bytesToCopy > 0) {
                if (bytesToCopy == dstCap) {
                    bytesToCopy -= 1;   /* Need to leave room for the null terminator. */
                }

                /* Don't move the base path if we're appending in-place. */
                if (pDst != pBasePath) {
                    E_COPY_MEMORY(pDst, pBasePath, E_MIN(basePathLen, dstCap));
                }
            }

            pDst   += bytesToCopy;
            dstCap -= bytesToCopy;
        }
        dstLen += basePathLen;

        /* Separator. */
        if (pDst != NULL) {
            if (dstCap > 1) {   /* Need to leave room for the separator. */
                pDst[0] = '/';
                pDst += 1;
                dstCap -= 1;
            }
        }
        dstLen += 1;    
    }
    

    /* Path to append. */
    if (pDst != NULL) {
        size_t bytesToCopy = E_MIN(pathToAppendLen, dstCap);
        
        if (bytesToCopy > 0) {
            if (bytesToCopy == dstCap) {
                bytesToCopy -= 1;   /* Need to leave room for the null terminator. */
            }

            E_COPY_MEMORY(pDst, pPathToAppend, bytesToCopy);
            pDst[bytesToCopy] = '\0';
        }
    }
    dstLen += pathToAppendLen;


    if (dstLen > 0x7FFFFFFF) {
        return -1;  /* Path is too long to convert to an int. */
    }

    return (int)dstLen;
}

E_API int e_path_normalize(char* pDst, size_t dstCap, const char* pPath, size_t pathLen, unsigned int options)
{
    e_path_iterator iPath;
    e_result result;
    e_bool32 allowLeadingBackNav = E_TRUE;
    e_path_iterator stack[256];    /* The size of this array controls the maximum number of components supported by this function. We're not doing any heap allocations here. Might add this later if necessary. */
    int top = 0;    /* Acts as a counter for the number of valid items in the stack. */
    int leadingBackNavCount = 0;
    int dstLen = 0;

    if (pPath == NULL) {
        pPath = "";
        pathLen = 0;
    }

    if (pDst != NULL && dstCap > 0) {
        pDst[0] = '\0';
    }

    /* Get rid of the empty case just to make our life easier below. */
    if (pathLen == 0 || pPath[0] == '\0') {
        return 0;
    }

    result = e_path_first(pPath, pathLen, &iPath);
    if (result != E_SUCCESS) {
        return -1;  /* Should never hit this because we did an empty string test above. */
    }

    /* We have a special case for when the result starts with "/". */
    if (iPath.segmentLength == 0) {
        allowLeadingBackNav = E_FALSE; /* When the path starts with "/" we cannot allow a leading ".." in the output path. */

        if (pDst != NULL && dstCap > 0) {
            pDst[0] = '/';
            pDst   += 1;
            dstCap -= 1;
        }
        dstLen += 1;

        /* Get past the root. */
        result = e_path_next(&iPath);
        if (result != E_SUCCESS) {
            return dstLen;
        }
    }

    if ((options & E_NO_ABOVE_ROOT_NAVIGATION) != 0) {
        allowLeadingBackNav = E_FALSE;
    }

    for (;;) {
        /* Everything in this control block should goto a section below or abort early. */
        {
            if (iPath.segmentLength == 0 || (iPath.segmentLength == 1 && iPath.pFullPath[iPath.segmentOffset] == '.')) {
                /* It's either an empty segment or ".". These are ignored. */
                goto next_segment;
            } else if (iPath.segmentLength == 2 && iPath.pFullPath[iPath.segmentOffset] == '.' && iPath.pFullPath[iPath.segmentOffset + 1] == '.') {
                /* It's a ".." segment. We need to either pop an entry from the stack, or if there is no way to go further back, push the "..". */
                if (top > leadingBackNavCount) {
                    top -= 1;
                    goto next_segment;
                } else {
                    /* In this case the path is trying to navigate above the root. This is not always allowed. */
                    if (!allowLeadingBackNav) {
                        return -1;
                    }

                    leadingBackNavCount += 1;
                    goto push_segment;
                }
            } else {
                /* It's a regular segment. These always need to be pushed onto the stack. */
                goto push_segment;
            }
        }

    push_segment:
        if (top < (int)E_COUNTOF(stack)) {
            stack[top] = iPath;
            top += 1;
        } else {
            return -1;  /* Ran out of room in "stack". */
        }

    next_segment:
        result = e_path_next(&iPath);
        if (result != E_SUCCESS) {
            break;
        }
    }

    /* At this point we should have a stack of items. Now we can construct the output path. */
    {
        int i = 0;
        for (i = 0; i < top; i += 1) {
            size_t segLen = stack[i].segmentLength;

            if (pDst != NULL && dstCap > segLen) {
                E_COPY_MEMORY(pDst, stack[i].pFullPath + stack[i].segmentOffset, segLen);
                pDst   += segLen;
                dstCap -= segLen;
            }
            dstLen += (int)segLen;

            /* Separator. */
            if (i + 1 < top) {
                if (pDst != NULL && dstCap > 0) {
                    pDst[0] = '/';
                    pDst   += 1;
                    dstCap -= 1;
                }
                dstLen += 1;
            }
        }
    }

    /* Null terminate. */
    if (pDst != NULL && dstCap > 0) {
        pDst[0] = '\0';
    }

    return dstLen;
}
/* END e_path.c */



/* BEG e_deflate.c */
#define E_DEFLATE_READ_LE16(p) ((e_uint32)(((const e_uint8*)(p))[0]) | ((e_uint32)(((const e_uint8*)(p))[1]) << 8U))
#define E_DEFLATE_READ_LE32(p) ((e_uint32)(((const e_uint8*)(p))[0]) | ((e_uint32)(((const e_uint8*)(p))[1]) << 8U) | ((e_uint32)(((const e_uint8*)(p))[2]) << 16U) | ((e_uint32)(((const e_uint8*)(p))[3]) << 24U))
#define E_DEFLATE_READ_LE64(p) ((e_uint64)(((const e_uint8*)(p))[0]) | ((e_uint64)(((const e_uint8*)(p))[1]) << 8U) | ((e_uint64)(((const e_uint8*)(p))[2]) << 16U) | ((e_uint64)(((const e_uint8*)(p))[3]) << 24U) | ((e_uint64)(((const e_uint8*)(p))[4]) << 32U) | ((e_uint64)(((const e_uint8*)(p))[5]) << 40U) | ((e_uint64)(((const e_uint8*)(p))[6]) << 48U) | ((e_uint64)(((const e_uint8*)(p))[7]) << 56U))

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

#define E_DEFLATE_NEED_BITS(stateIndex, n) do { unsigned int c; E_DEFLATE_GET_BYTE(stateIndex, c); bitBuffer |= (((e_deflate_bitbuf)c) << bitCount); bitCount += 8; } while (bitCount < (unsigned int)(n))
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
        bitBuffer |= (((e_deflate_bitbuf)c) << bitCount); \
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
            bitBuffer |= (((e_deflate_bitbuf)pInputBufferCurrent[0]) << bitCount) | (((e_deflate_bitbuf)pInputBufferCurrent[1]) << (bitCount + 8)); \
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

E_API e_result e_deflate_decompress(e_deflate_decompressor* pDecompressor, const e_uint8* pInputBuffer, size_t* pInputBufferSize, e_uint8* pOutputBufferStart, e_uint8* pOutputBufferNext, size_t* pOutputBufferSize, e_uint32 flags)
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
    e_deflate_bitbuf bitBuffer;
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
                            bitBuffer |= (((e_deflate_bitbuf)E_DEFLATE_READ_LE32(pInputBufferCurrent)) << bitCount);
                            pInputBufferCurrent += 4;
                            bitCount += 32;
                        }
#else               
                        if (bitCount < 15) {
                            bitBuffer |= (((e_deflate_bitbuf)E_DEFLATE_READ_LE16(pInputBufferCurrent)) << bitCount);
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
                            bitBuffer |= (((e_deflate_bitbuf)E_DEFLATE_READ_LE16(pInputBufferCurrent)) << bitCount);
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
/* END e_deflate.c */





/* BEG e_fs.c */
#if defined(_WIN32)
#include <shlobj.h>

#ifndef CSIDL_LOCAL_APPDATA
#define CSIDL_LOCAL_APPDATA 0x001C
#endif
#ifndef CSIDL_PROFILE
#define CSIDL_PROFILE       0x0028
#endif


/*
A helper for retrieving the directory containing the executable. We use this as a fall back for when
a system folder cannot be used (usually ancient versions of Windows).
*/
HRESULT e_get_executable_directory_win32(char* pPath)
{
    DWORD result;

    result = GetModuleFileNameA(NULL, pPath, 260);
    if (result == 260) {
        return ERROR_INSUFFICIENT_BUFFER;
    }

    e_path_directory(pPath, 260, pPath, result);

    return ERROR_SUCCESS;
}

/*
A simple wrapper to get a folder path. Mainly used to hide away some messy compatibility workarounds
for different versions of Windows.

The `pPath` pointer must be large enough to store at least 260 characters.
*/
HRESULT e_get_folder_path_win32(char* pPath, int nFolder)
{
    HRESULT hr;

    E_ASSERT(pPath != NULL);

    /*
    Using SHGetSpecialFolderPath() here for compatibility with Windows 95/98. This has been deprecated
    and the successor is SHGetFolderPath(), which itself has been deprecated in favour of the Known
    Folder API.

    If something comes up and SHGetSpecialFolderPath() stops working (unlikely), we could instead try
    using SHGetFolderPath(), like this:

        SHGetFolderPathA(NULL, nFolder, NULL, SHGFP_TYPE_CURRENT, pPath);

    If that also stops working, we would need to use the Known Folder API which I'm unfamiliar with.
    */

    hr = SHGetSpecialFolderPathA(NULL, pPath, nFolder, 0);
    if (FAILED(hr)) {
        /*
        If this fails it could be because we're calling this from an old version of Windows. We'll
        check for known folder types and do a fall back.
        */
        if (nFolder == CSIDL_LOCAL_APPDATA) {
            hr = SHGetSpecialFolderPathA(NULL, pPath, CSIDL_APPDATA, 0);
            if (FAILED(hr)) {
                hr = e_get_executable_directory_win32(pPath);
            }
        } else if (nFolder == CSIDL_PROFILE) {
            /*
            Old versions of Windows don't really have the notion of a user folder. In this case
            we'll just use the executable directory.
            */
            hr = e_get_executable_directory_win32(pPath);
        }
    }

    return hr;
}
#else
#include <pwd.h>
#include <unistd.h> /* For getuid() */

static const char* e_sysdir_home(void)
{
    const char* pHome;
    struct passwd* pPasswd;

    pHome = getenv("HOME");
    if (pHome != NULL) {
        return pHome;
    }

    /* Fallback to getpwuid(). */
    pPasswd = getpwuid(getuid());
    if (pPasswd != NULL) {
        return pPasswd->pw_dir;
    }

    return NULL;
}

static size_t e_sysdir_home_subdir(const char* pSubDir, char* pDst, size_t dstCap)
{
    const char* pHome = e_sysdir_home();
    if (pHome != NULL) {
        size_t homeLen = strlen(pHome);
        size_t subDirLen = strlen(pSubDir);
        size_t fullLength = homeLen + 1 + subDirLen;

        if (fullLength < dstCap) {
            E_COPY_MEMORY(pDst, pHome, homeLen);
            pDst[homeLen] = '/';
            E_COPY_MEMORY(pDst + homeLen + 1, pSubDir, subDirLen);
            pDst[fullLength] = '\0';
        }

        return fullLength;
    }

    return 0;
}
#endif

E_API size_t e_sysdir(e_sysdir_type type, char* pDst, size_t dstCap)
{
    size_t fullLength = 0;

    #if defined(_WIN32)
    {
        HRESULT hr;
        char pPath[260];

        switch (type)
        {
            case E_SYSDIR_HOME:
            {
                hr = e_get_folder_path_win32(pPath, CSIDL_PROFILE);
                if (SUCCEEDED(hr)) {
                    fullLength = strlen(pPath);
                    if (pDst != NULL && fullLength < dstCap) {
                        E_COPY_MEMORY(pDst, pPath, fullLength);
                        pDst[fullLength] = '\0';
                    }
                }
            } break;

            case E_SYSDIR_TEMP:
            {
                fullLength = GetTempPathA(sizeof(pPath), pPath);
                if (fullLength > 0) {
                    fullLength -= 1;  /* Remove the trailing slash. */

                    if (pDst != NULL && fullLength < dstCap) {
                        E_COPY_MEMORY(pDst, pPath, fullLength);
                        pDst[fullLength] = '\0';
                    }
                }
            } break;

            case E_SYSDIR_CONFIG:
            {
                hr = e_get_folder_path_win32(pPath, CSIDL_APPDATA);
                if (SUCCEEDED(hr)) {
                    fullLength = strlen(pPath);
                    if (pDst != NULL && fullLength < dstCap) {
                        E_COPY_MEMORY(pDst, pPath, fullLength);
                        pDst[fullLength] = '\0';
                    }
                }
            } break;

            case E_SYSDIR_DATA:
            {
                hr = e_get_folder_path_win32(pPath, CSIDL_LOCAL_APPDATA);
                if (SUCCEEDED(hr)) {
                    fullLength = strlen(pPath);
                    if (pDst != NULL && fullLength < dstCap) {
                        E_COPY_MEMORY(pDst, pPath, fullLength);
                        pDst[fullLength] = '\0';
                    }
                }
            } break;

            case E_SYSDIR_CACHE:
            {
                /* There's no proper known folder for caches. We'll just use %LOCALAPPDATA%\Cache. */
                hr = e_get_folder_path_win32(pPath, CSIDL_LOCAL_APPDATA);
                if (SUCCEEDED(hr)) {
                    const char* pCacheSuffix = "\\Cache";
                    size_t localAppDataLen = strlen(pPath);
                    size_t cacheSuffixLen = strlen(pCacheSuffix);
                    fullLength = localAppDataLen + cacheSuffixLen;

                    if (pDst != NULL && fullLength < dstCap) {
                        E_COPY_MEMORY(pDst, pPath, localAppDataLen);
                        E_COPY_MEMORY(pDst + localAppDataLen, pCacheSuffix, cacheSuffixLen);
                        pDst[fullLength] = '\0';
                    }
                }
            } break;

            default:
            {
                E_ASSERT(!"Unknown system directory type.");
            } break;
        }

        /* Normalize the path to use forward slashes. */
        if (pDst != NULL && fullLength < dstCap) {
            size_t i;

            for (i = 0; i < fullLength; i += 1) {
                if (pDst[i] == '\\') {
                    pDst[i] = '/';
                }
            }
        }
    }
    #else
    {
        switch (type)
        {
            case E_SYSDIR_HOME:
            {
                const char* pHome = e_sysdir_home();
                if (pHome != NULL) {
                    fullLength = strlen(pHome);
                    if (pDst != NULL && fullLength < dstCap) {
                        E_COPY_MEMORY(pDst, pHome, fullLength);
                        pDst[fullLength] = '\0';
                    }
                }
            } break;

            case E_SYSDIR_TEMP:
            {
                const char* pTemp = getenv("TMPDIR");
                if (pTemp != NULL) {
                    fullLength = strlen(pTemp);
                    if (pDst != NULL && fullLength < dstCap) {
                        E_COPY_MEMORY(pDst, pTemp, fullLength);
                        pDst[fullLength] = '\0';
                    }
                } else {
                    /* Fallback to /tmp. */
                    const char* pTmp = "/tmp";
                    fullLength = strlen(pTmp);
                    if (pDst != NULL && fullLength < dstCap) {
                        E_COPY_MEMORY(pDst, pTmp, fullLength);
                        pDst[fullLength] = '\0';
                    }
                }
            } break;

            case E_SYSDIR_CONFIG:
            {
                const char* pConfig = getenv("XDG_CONFIG_HOME");
                if (pConfig != NULL) {
                    fullLength = strlen(pConfig);
                    if (pDst != NULL && fullLength < dstCap) {
                        E_COPY_MEMORY(pDst, pConfig, fullLength);
                        pDst[fullLength] = '\0';
                    }
                } else {
                    /* Fallback to ~/.config. */
                    fullLength = e_sysdir_home_subdir(".config", pDst, dstCap);
                }
            } break;

            case E_SYSDIR_DATA:
            {
                const char* pData = getenv("XDG_DATA_HOME");
                if (pData != NULL) {
                    fullLength = strlen(pData);
                    if (pDst != NULL && fullLength < dstCap) {
                        E_COPY_MEMORY(pDst, pData, fullLength);
                        pDst[fullLength] = '\0';
                    }
                } else {
                    /* Fallback to ~/.local/share. */
                    fullLength = e_sysdir_home_subdir(".local/share", pDst, dstCap);
                }
            } break;

            case E_SYSDIR_CACHE:
            {
                const char* pCache = getenv("XDG_CACHE_HOME");
                if (pCache != NULL) {
                    fullLength = strlen(pCache);
                    if (pDst != NULL && fullLength < dstCap) {
                        E_COPY_MEMORY(pDst, pCache, fullLength);
                        pDst[fullLength] = '\0';
                    }
                } else {
                    /* Fallback to ~/.cache. */
                    fullLength = e_sysdir_home_subdir(".cache", pDst, dstCap);
                }
            } break;

            default:
            {
                E_ASSERT(!"Unknown system directory type.");
            } break;
        }
    }
    #endif

    return fullLength;
}


E_API e_result e_mktmp(const char* pPrefix, char* pTmpPath, size_t tmpPathCap, int options)
{
    size_t baseDirLen;
    const char* pPrefixName;
    const char* pPrefixDir;
    size_t prefixDirLen;

    if (pTmpPath == NULL) {
        return E_INVALID_ARGS;
    }

    pTmpPath[0] = '\0';  /* Safety. */

    if (tmpPathCap == 0) {
        return E_INVALID_ARGS;
    }

    if (pPrefix == NULL) {
        pPrefix = "";
    }

    if (pPrefix[0] == '\0') {
        pPrefix = "e_fs";
    }

    /* The caller must explicitly specify whether or not a file or directory is being created. */
    if ((options & (E_MKTMP_DIR | E_MKTMP_FILE)) == 0) {
        return E_INVALID_ARGS;
    }

    /* It's not allowed for both DIR and FILE to be set. */
    if ((options & E_MKTMP_DIR) != 0 && (options & E_MKTMP_FILE) != 0) {
        return E_INVALID_ARGS;
    }

    /* The prefix is not allowed to have any ".." segments and cannot start with "/". */
    if (strstr(pPrefix, "..") != NULL || pPrefix[0] == '/') {
        return E_INVALID_ARGS;
    }

    /* We first need to grab the directory of the system's base temp directory. */
    baseDirLen = e_sysdir(E_SYSDIR_TEMP, pTmpPath, tmpPathCap);
    if (baseDirLen == 0) {
        return E_ERROR;    /* Failed to retrieve the base temp directory. Cannot create a temp file. */
    }

    /* Now we need to append the directory part of the prefix. */
    pPrefixName = e_path_file_name(pPrefix, E_NULL_TERMINATED);
    E_ASSERT(pPrefixName != NULL);

    if (pPrefixName == pPrefix) {
        /* No directory. */
        pPrefixDir = "";
        prefixDirLen = 0;
    } else {
        /* We have a directory. */
        pPrefixDir = pPrefix;
        prefixDirLen = (size_t)(pPrefixName - pPrefix);
        prefixDirLen -= 1; /* Remove the trailing slash from the prefix directory. */
    }

    if (prefixDirLen > 0) {
        if (e_strcat_s(pTmpPath, tmpPathCap, "/") != 0) {
            return E_PATH_TOO_LONG;
        }
    }

    if (e_strncat_s(pTmpPath, tmpPathCap, pPrefixDir, prefixDirLen) != 0) {
        return E_PATH_TOO_LONG;
    }

    /* Create the directory structure if necessary. */
    if ((options & E_NO_CREATE_DIRS) == 0) {
        e_fs_mkdir(NULL, pTmpPath, E_IGNORE_MOUNTS);
    }

    /* Now we can append the between the directory part and the name part. */
    if (e_strcat_s(pTmpPath, tmpPathCap, "/") != 0) {
        return E_PATH_TOO_LONG;
    }

    /* We're now ready for the platform specific part. */
    #if defined(_WIN32)
    {
        /*
        We're using GetTempFileName(). This is annoying because of two things. First, it requires that
        path separators be backslashes. Second, it does not take a capacity parameter so we need to
        ensure the output buffer is at least MAX_PATH (260) bytes long.
        */
        char pTmpPathWin[MAX_PATH];
        size_t i;

        for (i = 0; pTmpPath[i] != '\0'; i += 1) {
            if (pTmpPath[i] == '/') {
                pTmpPath[i] = '\\';
            }
        }

        if (GetTempFileNameA(pTmpPath, pPrefixName, 0, pTmpPathWin) == 0) {
            return e_result_from_errno(GetLastError());
        }

        /*
        NOTE: At this point the operating system will have created the file. If any error occurs from here
        we need to remember to delete it.
        */

        if (e_strcpy_s(pTmpPath, tmpPathCap, pTmpPathWin) != 0) {
            DeleteFileA(pTmpPathWin);
            return E_PATH_TOO_LONG;
        }

        /*
        If we're creating a folder the process is to delete the file that the OS just created and create a new
        folder in it's place.
        */
        if ((options & E_MKTMP_DIR) != 0) {
            /* We're creating a temp directory. Delete the file and create a folder in it's place. */
            DeleteFileA(pTmpPathWin);

            if (CreateDirectoryA(pTmpPathWin, NULL) == 0) {
                return e_result_from_errno(GetLastError());
            }
        } else {
            /* We're creating a temp file. The OS will have already created the file in GetTempFileNameA() so no need to create it explicitly. */
        }

        /* Finally we need to convert our back slashes to forward slashes. */
        for (i = 0; pTmpPath[i] != '\0'; i += 1) {
            if (pTmpPath[i] == '\\') {
                pTmpPath[i] = '/';
            }
        }
    }
    #else
    {
        /* Append the file name part. */
        if (e_strcat_s(pTmpPath, tmpPathCap, pPrefixName) != 0) {
            return E_PATH_TOO_LONG;
        }

        /* Append the random part. */
        if (e_strcat_s(pTmpPath, tmpPathCap, "XXXXXX") != 0) {
            return E_PATH_TOO_LONG;
        }

        /* At this point the full path has been constructed. We can now create the file or directory. */
        if ((options & E_MKTMP_DIR) != 0) {
            /* We're creating a temp directory. */
            if (mkdtemp(pTmpPath) == NULL) {
                return e_result_from_errno(errno);
            }
        } else {
            /* We're creating a temp file. */
            int fd = mkstemp(pTmpPath);
            if (fd == -1) {
                return e_result_from_errno(errno);
            }

            close(fd);
        }
    }
    #endif

    return E_SUCCESS;
}


static size_t e_fs_backend_alloc_size(const e_fs_backend* pBackend, const void* pBackendConfig)
{
    E_ASSERT(pBackend != NULL);

    if (pBackend->alloc_size == NULL) {
        return 0;
    } else {
        return pBackend->alloc_size(pBackendConfig);
    }
}

static e_result e_fs_backend_init(const e_fs_backend* pBackend, e_fs* pFS, const void* pBackendConfig, e_stream* pStream)
{
    E_ASSERT(pBackend != NULL);

    if (pBackend->init == NULL) {
        return E_NOT_IMPLEMENTED;
    } else {
        return pBackend->init(pFS, pBackendConfig, pStream);
    }
}

static void e_fs_backend_uninit(const e_fs_backend* pBackend, e_fs* pFS)
{
    E_ASSERT(pBackend != NULL);

    if (pBackend->uninit == NULL) {
        return;
    } else {
        pBackend->uninit(pFS);
    }
}

static e_result e_fs_backend_ioctl(const e_fs_backend* pBackend, e_fs* pFS, int command, void* pArgs)
{
    E_ASSERT(pBackend != NULL);

    if (pBackend->ioctl == NULL) {
        return E_NOT_IMPLEMENTED;
    } else {
        return pBackend->ioctl(pFS, command, pArgs);
    }
}

static e_result e_fs_backend_remove(const e_fs_backend* pBackend, e_fs* pFS, const char* pFilePath)
{
    E_ASSERT(pBackend != NULL);

    if (pBackend->remove == NULL) {
        return E_NOT_IMPLEMENTED;
    } else {
        return pBackend->remove(pFS, pFilePath);
    }
}

static e_result e_fs_backend_rename(const e_fs_backend* pBackend, e_fs* pFS, const char* pOldName, const char* pNewName)
{
    E_ASSERT(pBackend != NULL);

    if (pBackend->remove == NULL) {
        return E_NOT_IMPLEMENTED;
    } else {
        return pBackend->rename(pFS, pOldName, pNewName);
    }
}

static e_result e_fs_backend_mkdir(const e_fs_backend* pBackend, e_fs* pFS, const char* pPath)
{
    E_ASSERT(pBackend != NULL);

    if (pBackend->mkdir == NULL) {
        return E_NOT_IMPLEMENTED;
    } else {
        return pBackend->mkdir(pFS, pPath);
    }
}

static e_result e_fs_backend_info(const e_fs_backend* pBackend, e_fs* pFS, const char* pPath, int openMode, e_file_info* pInfo)
{
    E_ASSERT(pBackend != NULL);

    if (pBackend->info == NULL) {
        return E_NOT_IMPLEMENTED;
    } else {
        return pBackend->info(pFS, pPath, openMode, pInfo);
    }
}

static size_t e_fs_backend_file_alloc_size(const e_fs_backend* pBackend, e_fs* pFS)
{
    E_ASSERT(pBackend != NULL);

    if (pBackend->file_alloc_size == NULL) {
        return 0;
    } else {
        return pBackend->file_alloc_size(pFS);
    }
}

static e_result e_fs_backend_file_open(const e_fs_backend* pBackend, e_fs* pFS, e_stream* pStream, const char* pFilePath, int openMode, e_file* pFile)
{
    E_ASSERT(pBackend != NULL);

    if (pBackend->file_open == NULL) {
        return E_NOT_IMPLEMENTED;
    } else {
        return pBackend->file_open(pFS, pStream, pFilePath, openMode, pFile);
    }
}

static e_result e_fs_backend_file_open_handle(const e_fs_backend* pBackend, e_fs* pFS, void* hBackendFile, e_file* pFile)
{
    E_ASSERT(pBackend != NULL);

    if (pBackend->file_open_handle == NULL) {
        return E_NOT_IMPLEMENTED;
    } else {
        return pBackend->file_open_handle(pFS, hBackendFile, pFile);
    }
}

static void e_fs_backend_file_close(const e_fs_backend* pBackend, e_file* pFile)
{
    E_ASSERT(pBackend != NULL);

    if (pBackend->file_close == NULL) {
        return;
    } else {
        pBackend->file_close(pFile);
    }
}

static e_result e_fs_backend_file_read(const e_fs_backend* pBackend, e_file* pFile, void* pDst, size_t bytesToRead, size_t* pBytesRead)
{
    E_ASSERT(pBackend != NULL);

    if (pBackend->file_read == NULL) {
        return E_NOT_IMPLEMENTED;
    } else {
        return pBackend->file_read(pFile, pDst, bytesToRead, pBytesRead);
    }
}

static e_result e_fs_backend_file_write(const e_fs_backend* pBackend, e_file* pFile, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten)
{
    E_ASSERT(pBackend != NULL);

    if (pBackend->file_write == NULL) {
        return E_NOT_IMPLEMENTED;
    } else {
        return pBackend->file_write(pFile, pSrc, bytesToWrite, pBytesWritten);
    }
}

static e_result e_fs_backend_file_seek(const e_fs_backend* pBackend, e_file* pFile, e_int64 offset, e_seek_origin origin)
{
    E_ASSERT(pBackend != NULL);

    if (pBackend->file_seek == NULL) {
        return E_NOT_IMPLEMENTED;
    } else {
        return pBackend->file_seek(pFile, offset, origin);
    }
}

static e_result e_fs_backend_file_tell(const e_fs_backend* pBackend, e_file* pFile, e_int64* pCursor)
{
    E_ASSERT(pBackend != NULL);

    if (pBackend->file_tell == NULL) {
        return E_NOT_IMPLEMENTED;
    } else {
        return pBackend->file_tell(pFile, pCursor);
    }
}

static e_result e_fs_backend_file_flush(const e_fs_backend* pBackend, e_file* pFile)
{
    E_ASSERT(pBackend != NULL);

    if (pBackend->file_flush == NULL) {
        return E_NOT_IMPLEMENTED;
    } else {
        return pBackend->file_flush(pFile);
    }
}

static e_result e_fs_backend_file_info(const e_fs_backend* pBackend, e_file* pFile, e_file_info* pInfo)
{
    E_ASSERT(pBackend != NULL);

    if (pBackend->file_info == NULL) {
        return E_NOT_IMPLEMENTED;
    } else {
        return pBackend->file_info(pFile, pInfo);
    }
}

static e_result e_fs_backend_file_duplicate(const e_fs_backend* pBackend, e_file* pFile, e_file* pDuplicatedFile)
{
    E_ASSERT(pBackend != NULL);

    if (pBackend->file_duplicate == NULL) {
        return E_NOT_IMPLEMENTED;
    } else {
        return pBackend->file_duplicate(pFile, pDuplicatedFile);
    }
}

static e_fs_iterator* e_fs_backend_first(const e_fs_backend* pBackend, e_fs* pFS, const char* pDirectoryPath, size_t directoryPathLen)
{
    E_ASSERT(pBackend != NULL);

    if (pBackend->first == NULL) {
        return NULL;
    } else {
        e_fs_iterator* pIterator;
        
        pIterator = pBackend->first(pFS, pDirectoryPath, directoryPathLen);
        
        /* Just make double sure the FS information is set in case the backend doesn't do it. */
        if (pIterator != NULL) {
            pIterator->pFS = pFS;
        }

        return pIterator;
    }
}

static e_fs_iterator* e_fs_backend_next(const e_fs_backend* pBackend, e_fs_iterator* pIterator)
{
    E_ASSERT(pBackend != NULL);

    if (pBackend->next == NULL) {
        return NULL;
    } else {
        return pBackend->next(pIterator);
    }
}

static void e_fs_backend_free_iterator(const e_fs_backend* pBackend, e_fs_iterator* pIterator)
{
    E_ASSERT(pBackend != NULL);

    if (pBackend->free_iterator == NULL) {
        return;
    } else {
        pBackend->free_iterator(pIterator);
    }
}


/*
This is the maximum number of ureferenced opened archive files that will be kept in memory
before garbage collection of those archives is triggered.
*/
#ifndef E_DEFAULT_ARCHIVE_GC_THRESHOLD
#define E_DEFAULT_ARCHIVE_GC_THRESHOLD 10
#endif

#define E_IS_OPAQUE(mode)      ((mode & E_OPAQUE ) == E_OPAQUE )
#define E_IS_VERBOSE(mode)     ((mode & E_VERBOSE) == E_VERBOSE)
#define E_IS_TRANSPARENT(mode) (!E_IS_OPAQUE(mode) && !E_IS_VERBOSE(mode))

E_API e_fs_config e_config_init_default(void)
{
    e_fs_config config;

    E_ZERO_OBJECT(&config);

    return config;
}

E_API e_fs_config e_fs_config_init(const e_fs_backend* pBackend, void* pBackendConfig, e_stream* pStream)
{
    e_fs_config config = e_config_init_default();
    config.pBackend       = pBackend;
    config.pBackendConfig = pBackendConfig;
    config.pStream        = pStream;

    return config;
}

typedef struct e_opened_archive
{
    e_fs* pArchive;
    char pPath[1];
} e_opened_archive;

typedef struct e_mount_point
{
    size_t pathOff;                     /* Points to a null terminated string containing the mounted path starting from the first byte after this struct. */
    size_t pathLen;
    size_t mountPointOff;               /* Points to a null terminated string containing the mount point starting from the first byte after this struct. */
    size_t mountPointLen;
    e_fs* pArchive;                       /* Can be null in which case the mounted path is a directory. */
    e_bool32 closeArchiveOnUnmount;    /* If set to true, the archive FS will be closed when the mount point is unmounted. */
    e_bool32 padding;
} e_mount_point;

typedef struct e_mount_list e_mount_list;

struct e_fs
{
    const e_fs_backend* pBackend;
    e_stream* pStream;
    e_allocation_callbacks allocationCallbacks;
    void* pArchiveTypes;    /* One heap allocation containing all extension registrations. Needs to be parsed in order to enumerate them. Structure is [const e_fs_backend*][extension][null-terminator][padding (aligned to E_SIZEOF_PTR)] */
    size_t archiveTypesAllocSize;
    e_bool32 isOwnerOfArchiveTypes;
    size_t backendDataSize;
    e_on_refcount_changed_proc onRefCountChanged;
    void* pRefCountChangedUserData;
    e_mutex archiveLock;     /* For use with e_open_archive() and e_close_archive(). */
    void* pOpenedArchives;  /* One heap allocation. Structure is [e_fs*][refcount (size_t)][path][null-terminator][padding (aligned to E_SIZEOF_PTR)] */
    size_t openedArchivesSize;
    size_t openedArchivesCap;
    size_t archiveGCThreshold;
    e_mount_list* pReadMountPoints;
    e_mount_list* pWriteMountPoints;
    e_mutex refLock;
    e_uint32 refCount;        /* Incremented when a file is opened, decremented when a file is closed. */
};

typedef struct e_file
{
    e_stream stream; /* Files are streams. This must be the first member so it can be cast. */
    e_fs* pFS;
    e_stream* pStreamForBackend;   /* The stream for use by the backend. Different to `stream`. This is a duplicate of the stream used by `pFS` so the backend can do reading. */
    size_t backendDataSize;
} e_file;

typedef enum e_mount_priority
{
    E_MOUNT_PRIORITY_HIGHEST = 0,
    E_MOUNT_PRIORITY_LOWEST  = 1
} e_mount_priority;


static void e_gc_archives_nolock(e_fs* pFS, int policy); /* Defined further down in the file. */


static size_t e_mount_point_size(size_t pathLen, size_t mountPointLen)
{
    return E_ALIGN(sizeof(e_mount_point) + pathLen + 1 + mountPointLen + 1, E_SIZEOF_PTR);
}



static size_t e_mount_list_get_header_size(void)
{
    return sizeof(size_t)*2;
}

static size_t e_mount_list_get_alloc_size(const e_mount_list* pList)
{
    if (pList == NULL) {
        return 0;
    }

    return *(size_t*)E_OFFSET_PTR(pList, 0);
}

static size_t e_mount_list_get_alloc_cap(const e_mount_list* pList)
{
    if (pList == NULL) {
        return 0;
    }

    return *(size_t*)E_OFFSET_PTR(pList, 1 * sizeof(size_t));
}

static void e_mount_list_set_alloc_size(e_mount_list* pList, size_t newSize)
{
    E_ASSERT(pList != NULL);
    *(size_t*)E_OFFSET_PTR(pList, 0) = newSize;
}

static void e_mount_list_set_alloc_cap(e_mount_list* pList, size_t newCap)
{
    E_ASSERT(pList != NULL);
    *(size_t*)E_OFFSET_PTR(pList, 1 * sizeof(size_t)) = newCap;
}


typedef struct e_mount_list_iterator
{
    const char* pPath;
    const char* pMountPointPath;
    e_fs* pArchive; /* Can be null. */
    struct
    {
        e_mount_list* pList;
        e_mount_point* pMountPoint;
        size_t cursor;
    } internal;
} e_mount_list_iterator;

static e_result e_mount_list_iterator_resolve_members(e_mount_list_iterator* pIterator, size_t cursor)
{
    E_ASSERT(pIterator != NULL);

    if (cursor >= e_mount_list_get_alloc_size(pIterator->internal.pList)) {
        return E_AT_END;
    }

    pIterator->internal.cursor      = cursor;
    pIterator->internal.pMountPoint = (e_mount_point*)E_OFFSET_PTR(pIterator->internal.pList, e_mount_list_get_header_size() + pIterator->internal.cursor);
    E_ASSERT(pIterator->internal.pMountPoint != NULL);

    /* The content of the paths are stored at the end of the structure. */
    pIterator->pPath           = (const char*)E_OFFSET_PTR(pIterator->internal.pMountPoint, sizeof(e_mount_point) + pIterator->internal.pMountPoint->pathOff);
    pIterator->pMountPointPath = (const char*)E_OFFSET_PTR(pIterator->internal.pMountPoint, sizeof(e_mount_point) + pIterator->internal.pMountPoint->mountPointOff);
    pIterator->pArchive        = pIterator->internal.pMountPoint->pArchive;

    return E_SUCCESS;
}

static e_bool32 e_mount_list_at_end(const e_mount_list_iterator* pIterator)
{
    E_ASSERT(pIterator != NULL);

    return (pIterator->internal.cursor >= e_mount_list_get_alloc_size(pIterator->internal.pList));
}

static e_result e_mount_list_first(e_mount_list* pList, e_mount_list_iterator* pIterator)
{
    E_ASSERT(pIterator != NULL);

    E_ZERO_OBJECT(pIterator);
    pIterator->internal.pList = pList;

    if (e_mount_list_get_alloc_size(pList) == 0) {
        return E_AT_END;   /* No mount points. */
    }

    return e_mount_list_iterator_resolve_members(pIterator, 0);
}

static e_result e_mount_list_next(e_mount_list_iterator* pIterator)
{
    size_t newCursor;

    E_ASSERT(pIterator != NULL);

    /* We can't continue if the list is at the end or else we'll overrun the cursor. */
    if (e_mount_list_at_end(pIterator)) {
        return E_AT_END;
    }

    /* Move the cursor forward. If after advancing the cursor we are at the end we're done and we can free the mount point iterator and return. */
    newCursor = pIterator->internal.cursor + e_mount_point_size(pIterator->internal.pMountPoint->pathLen, pIterator->internal.pMountPoint->mountPointLen);
    E_ASSERT(newCursor <= e_mount_list_get_alloc_size(pIterator->internal.pList)); /* <-- If this assert fails, there's a bug in the packing of the structure.*/

    return e_mount_list_iterator_resolve_members(pIterator, newCursor);
}

static e_mount_list* e_mount_list_alloc(e_mount_list* pList, const char* pPathToMount, const char* pMountPoint, e_mount_priority priority, const e_allocation_callbacks* pAllocationCallbacks, e_mount_point** ppMountPoint)
{
    e_mount_point* pNewMountPoint = NULL;
    size_t pathToMountLen;
    size_t mountPointLen;
    size_t mountPointAllocSize;

    E_ASSERT(ppMountPoint != NULL);
    *ppMountPoint = NULL;

    pathToMountLen = strlen(pPathToMount);
    mountPointLen  = strlen(pMountPoint);
    mountPointAllocSize = e_mount_point_size(pathToMountLen, mountPointLen);

    if (e_mount_list_get_alloc_cap(pList) < e_mount_list_get_alloc_size(pList) + mountPointAllocSize) {
        size_t newCap;
        e_mount_list* pNewList;

        newCap = e_mount_list_get_alloc_cap(pList) * 2;
        if (newCap < e_mount_list_get_alloc_size(pList) + mountPointAllocSize) {
            newCap = e_mount_list_get_alloc_size(pList) + mountPointAllocSize;
        }

        pNewList = (e_mount_list*)e_realloc(pList, e_mount_list_get_header_size() + newCap, pAllocationCallbacks); /* Need room for leading size and cap variables. */
        if (pNewList == NULL) {
            return NULL;
        }

        /* Little bit awkward, but if the list is fresh we'll want to clear everything to zero. */
        if (pList == NULL) {
            E_ZERO_MEMORY(pNewList, e_mount_list_get_header_size());
        }

        pList = (e_mount_list*)pNewList;
        e_mount_list_set_alloc_cap(pList, newCap);
    }

    /*
    Getting here means we should have enough room in the buffer. Now we need to use the priority to determine where
    we're going to place the new entry within the buffer.
    */
    if (priority == E_MOUNT_PRIORITY_LOWEST) {
        /* The new entry goes to the end of the list. */
        pNewMountPoint = (e_mount_point*)E_OFFSET_PTR(pList, e_mount_list_get_header_size() + e_mount_list_get_alloc_size(pList));
    } else if (priority == E_MOUNT_PRIORITY_HIGHEST) {
        /* The new entry goes to the start of the list. We'll need to move everything down. */
        E_MOVE_MEMORY(E_OFFSET_PTR(pList, e_mount_list_get_header_size() + mountPointAllocSize), E_OFFSET_PTR(pList, e_mount_list_get_header_size()), e_mount_list_get_alloc_size(pList));
        pNewMountPoint = (e_mount_point*)E_OFFSET_PTR(pList, e_mount_list_get_header_size());
    } else {
        E_ASSERT(!"Unknown mount priority.");
        return NULL;
    }

    e_mount_list_set_alloc_size(pList, e_mount_list_get_alloc_size(pList) + mountPointAllocSize);

    /* Now we can fill out the details of the new mount point. */
    pNewMountPoint->pathOff       = 0;                  /* The path is always the first byte after the struct. */
    pNewMountPoint->pathLen       = pathToMountLen;
    pNewMountPoint->mountPointOff = pathToMountLen + 1; /* The mount point is always the first byte after the path to mount. */
    pNewMountPoint->mountPointLen = mountPointLen;

    memcpy(E_OFFSET_PTR(pNewMountPoint, sizeof(e_mount_point) + pNewMountPoint->pathOff),       pPathToMount, pathToMountLen + 1);
    memcpy(E_OFFSET_PTR(pNewMountPoint, sizeof(e_mount_point) + pNewMountPoint->mountPointOff), pMountPoint,  mountPointLen  + 1);

    *ppMountPoint = pNewMountPoint;
    return pList;
}

static e_result e_mount_list_remove(e_mount_list* pList, e_mount_point* pMountPoint)
{
    size_t mountPointAllocSize = e_mount_point_size(pMountPoint->pathLen, pMountPoint->mountPointLen);
    size_t newMountPointsAllocSize = e_mount_list_get_alloc_size(pList) - mountPointAllocSize;

    E_MOVE_MEMORY
    (
        pMountPoint,
        E_OFFSET_PTR(pList, e_mount_list_get_header_size() + mountPointAllocSize),
        e_mount_list_get_alloc_size(pList) - ((e_uintptr)pMountPoint - (e_uintptr)E_OFFSET_PTR(pList, e_mount_list_get_header_size())) - mountPointAllocSize
    );

    e_mount_list_set_alloc_size(pList, newMountPointsAllocSize);

    return E_SUCCESS;
}



static const e_fs_backend* e_get_backend_or_default(const e_fs* pFS)
{
    if (pFS == NULL) {
        return E_FS_STDIO;
    } else {
        return pFS->pBackend;
    }
}

typedef struct e_registered_backend_iterator
{
    const e_fs* pFS;
    size_t cursor;
    const e_fs_backend* pBackend;
    void* pBackendConfig;
    const char* pExtension;
    size_t extensionLen;
} e_registered_backend_iterator;

E_API e_result e_file_open_or_info(e_fs* pFS, const char* pFilePath, int openMode, e_file** ppFile, e_file_info* pInfo);
static e_result e_next_registered_backend(e_registered_backend_iterator* pIterator);

static e_result e_first_registered_backend(e_fs* pFS, e_registered_backend_iterator* pIterator)
{
    E_ASSERT(pFS       != NULL);
    E_ASSERT(pIterator != NULL);

    E_ZERO_OBJECT(pIterator);
    pIterator->pFS = pFS;

    return e_next_registered_backend(pIterator);
}

static e_result e_next_registered_backend(e_registered_backend_iterator* pIterator)
{
    E_ASSERT(pIterator != NULL);

    if (pIterator->cursor >= pIterator->pFS->archiveTypesAllocSize) {
        return E_AT_END;
    }

    pIterator->pBackend       = *(const e_fs_backend**)E_OFFSET_PTR(pIterator->pFS->pArchiveTypes, pIterator->cursor);
    pIterator->pBackendConfig =  NULL;   /* <-- I'm not sure how to deal with backend configs with this API. Putting this member in the iterator in case I want to support this later. */
    pIterator->pExtension     =  (const char*       )E_OFFSET_PTR(pIterator->pFS->pArchiveTypes, pIterator->cursor + sizeof(e_fs_backend*));
    pIterator->extensionLen   =  strlen(pIterator->pExtension);

    pIterator->cursor += E_ALIGN(sizeof(e_fs_backend*) + pIterator->extensionLen + 1, E_SIZEOF_PTR);

    return E_SUCCESS;
}



static e_opened_archive* e_find_opened_archive(e_fs* pFS, const char* pArchivePath, size_t archivePathLen)
{
    size_t cursor;

    if (pFS == NULL) {
        return NULL;
    }

    E_ASSERT(pArchivePath != NULL);
    E_ASSERT(archivePathLen > 0);

    cursor = 0;
    while (cursor < pFS->openedArchivesSize) {
        e_opened_archive* pOpenedArchive = (e_opened_archive*)E_OFFSET_PTR(pFS->pOpenedArchives, cursor);

        if (e_strncmp(pOpenedArchive->pPath, pArchivePath, archivePathLen) == 0) {
            return pOpenedArchive;
        }

        /* Getting here means this archive is not the one we're looking for. */
        cursor += E_ALIGN(sizeof(e_fs*) + sizeof(size_t) + strlen(pOpenedArchive->pPath) + 1, E_SIZEOF_PTR);
    }

    /* If we get here it means we couldn't find the archive by it's name. */
    return NULL;
}

#if 0
static e_opened_archive* e_find_opened_archive_by_fs(e_fs* pFS, e_fs* pArchive)
{
    size_t cursor;

    if (pFS == NULL) {
        return NULL;
    }

    E_ASSERT(pArchive != NULL);

    cursor = 0;
    while (cursor < pFS->openedArchivesSize) {
        e_opened_archive* pOpenedArchive = (e_opened_archive*)E_OFFSET_PTR(pFS->pOpenedArchives, cursor);

        if (pOpenedArchive->pArchive == pArchive) {
            return pOpenedArchive;
        }

        /* Getting here means this archive is not the one we're looking for. */
        cursor += E_ALIGN(sizeof(e_fs*) + sizeof(size_t) + strlen(pOpenedArchive->pPath) + 1, E_SIZEOF_PTR);
    }

    /* If we get here it means we couldn't find the archive. */
    return NULL;
}
#endif

static e_result e_add_opened_archive(e_fs* pFS, e_fs* pArchive, const char* pArchivePath, size_t archivePathLen)
{
    size_t openedArchiveSize;
    e_opened_archive* pOpenedArchive;

    E_ASSERT(pFS          != NULL);
    E_ASSERT(pArchive     != NULL);
    E_ASSERT(pArchivePath != NULL);

    if (archivePathLen == E_NULL_TERMINATED) {
        archivePathLen = strlen(pArchivePath);
    }

    openedArchiveSize = E_ALIGN(sizeof(e_fs*) + sizeof(size_t) + archivePathLen + 1, E_SIZEOF_PTR);

    if (pFS->openedArchivesSize + openedArchiveSize > pFS->openedArchivesCap) {
        size_t newOpenedArchivesCap;
        void* pNewOpenedArchives;

        newOpenedArchivesCap = pFS->openedArchivesCap * 2;
        if (newOpenedArchivesCap < pFS->openedArchivesSize + openedArchiveSize) {
            newOpenedArchivesCap = pFS->openedArchivesSize + openedArchiveSize;
        }

        pNewOpenedArchives = e_realloc(pFS->pOpenedArchives, newOpenedArchivesCap, e_fs_get_allocation_callbacks(pFS));
        if (pNewOpenedArchives == NULL) {
            return E_OUT_OF_MEMORY;
        }

        pFS->pOpenedArchives   = pNewOpenedArchives;
        pFS->openedArchivesCap = newOpenedArchivesCap;
    }

    /* If we get here we should have enough room in the buffer to store the new archive details. */
    E_ASSERT(pFS->openedArchivesSize + openedArchiveSize <= pFS->openedArchivesCap);

    pOpenedArchive = (e_opened_archive*)E_OFFSET_PTR(pFS->pOpenedArchives, pFS->openedArchivesSize);
    pOpenedArchive->pArchive = pArchive;
    e_strncpy(pOpenedArchive->pPath, pArchivePath, archivePathLen);

    pFS->openedArchivesSize += openedArchiveSize;

    return E_SUCCESS;
}

static e_result e_remove_opened_archive(e_fs* pFS, e_opened_archive* pOpenedArchive)
{
    /* This is a simple matter of doing a memmove() to move memory down. pOpenedArchive should be an offset of pFS->pOpenedArchives. */
    size_t openedArchiveSize;

    openedArchiveSize = E_ALIGN(sizeof(e_opened_archive*) + sizeof(size_t) + strlen(pOpenedArchive->pPath) + 1, E_SIZEOF_PTR);

    E_ASSERT(((e_uintptr)pOpenedArchive + openedArchiveSize) >  ((e_uintptr)pFS->pOpenedArchives));
    E_ASSERT(((e_uintptr)pOpenedArchive + openedArchiveSize) <= ((e_uintptr)pFS->pOpenedArchives + pFS->openedArchivesSize));

    E_MOVE_MEMORY(pOpenedArchive, E_OFFSET_PTR(pOpenedArchive, openedArchiveSize), (size_t)((((e_uintptr)pFS->pOpenedArchives + pFS->openedArchivesSize)) - ((e_uintptr)pOpenedArchive + openedArchiveSize)));
    pFS->openedArchivesSize -= openedArchiveSize;

    return E_SUCCESS;
}


static size_t e_archive_type_sizeof(const e_archive_type* pArchiveType)
{
    return E_ALIGN(sizeof(pArchiveType->pBackend) + strlen(pArchiveType->pExtension) + 1, E_SIZEOF_PTR);
}


static e_mount_point* e_find_best_write_mount_point(e_fs* pFS, const char* pPath, const char** ppMountPointPath, const char** ppSubPath)
{
    /*
    This is a bit different from read mounts because we want to use the mount point that most closely
    matches the start of the file path. Consider, for example, the following mount points:

        - config
        - config/global

    If we're trying to open "config/global/settings.cfg" we want to use the "config/global" mount
    point, not the "config" mount point. This is because the "config/global" mount point is more
    specific and therefore more likely to be the correct one.

    We'll need to iterate over every mount point and keep track of the mount point with the longest
    prefix that matches the start of the file path.
    */
    e_result result;
    e_mount_list_iterator iMountPoint;
    e_mount_point* pBestMountPoint = NULL;
    const char* pBestMountPointPath = NULL;
    const char* pBestMountPointFileSubPath = NULL;
    
    for (result = e_mount_list_first(pFS->pWriteMountPoints, &iMountPoint); result == E_SUCCESS; result = e_mount_list_next(&iMountPoint)) {
        const char* pFileSubPath = e_path_trim_base(pPath, E_NULL_TERMINATED, iMountPoint.pMountPointPath, E_NULL_TERMINATED);
        if (pFileSubPath == NULL) {
            continue;   /* The file path doesn't start with this mount point so skip. */
        }

        if (pBestMountPointFileSubPath == NULL || strlen(pFileSubPath) < strlen(pBestMountPointFileSubPath)) {
            pBestMountPoint = iMountPoint.internal.pMountPoint;
            pBestMountPointPath = iMountPoint.pPath;
            pBestMountPointFileSubPath = pFileSubPath;
        }
    }

    if (ppMountPointPath != NULL) {
        *ppMountPointPath = pBestMountPointPath;
    }
    if (ppSubPath != NULL) {
        *ppSubPath = pBestMountPointFileSubPath;
    }

    return pBestMountPoint;
}


E_API e_result e_fs_init(const e_fs_config* pConfig, e_fs** ppFS)
{
    e_fs* pFS;
    e_fs_config defaultConfig;
    const e_fs_backend* pBackend = NULL;
    size_t backendDataSizeInBytes = 0;
    e_int64 initialStreamCursor = -1;
    size_t archiveTypesAllocSize = 0;
    size_t iArchiveType;
    e_result result;

    if (ppFS == NULL) {
        return E_INVALID_ARGS;
    }

    *ppFS = NULL;

    if (pConfig == NULL) {
        defaultConfig = e_config_init_default();
        pConfig = &defaultConfig;
    }

    pBackend = pConfig->pBackend;
    if (pBackend == NULL) {
        pBackend = E_FS_STDIO;
    }

    /* If the backend is still null at this point it means the default backend has been disabled. */
    if (pBackend == NULL) {
        return E_INVALID_ARGS;
    }

    backendDataSizeInBytes = e_fs_backend_alloc_size(pBackend, pConfig->pBackendConfig);

    /* We need to allocate space for the archive types which we place just after the "e_fs" struct. After that will be the backend data. */
    for (iArchiveType = 0; iArchiveType < pConfig->archiveTypeCount; iArchiveType += 1) {
        archiveTypesAllocSize += e_archive_type_sizeof(&pConfig->pArchiveTypes[iArchiveType]);
    }

    pFS = (e_fs*)e_calloc(sizeof(e_fs) + archiveTypesAllocSize + backendDataSizeInBytes, pConfig->pAllocationCallbacks);
    if (pFS == NULL) {
        return E_OUT_OF_MEMORY;
    }

    pFS->pBackend              = pBackend;
    pFS->pStream               = pConfig->pStream; /* <-- This is allowed to be null, which will be the case for standard OS file system APIs like stdio. Streams are used for things like archives like Zip files, or in-memory file systems. */
    pFS->refCount              = 1;
    pFS->allocationCallbacks   = e_allocation_callbacks_init_copy(pConfig->pAllocationCallbacks);
    pFS->backendDataSize       = backendDataSizeInBytes;
    pFS->onRefCountChanged     = pConfig->onRefCountChanged;
    pFS->pRefCountChangedUserData = pConfig->pRefCountChangedUserData;
    pFS->isOwnerOfArchiveTypes = E_TRUE;
    pFS->archiveGCThreshold    = E_DEFAULT_ARCHIVE_GC_THRESHOLD;
    pFS->archiveTypesAllocSize = archiveTypesAllocSize;
    pFS->pArchiveTypes         = (void*)E_OFFSET_PTR(pFS, sizeof(e_fs));

    /* Archive types. */
    if (pConfig->archiveTypeCount > 0) {
        size_t cursor = 0;

        for (iArchiveType = 0; iArchiveType < pConfig->archiveTypeCount; iArchiveType += 1) {
            size_t extensionLength = strlen(pConfig->pArchiveTypes[iArchiveType].pExtension);

            E_COPY_MEMORY(E_OFFSET_PTR(pFS->pArchiveTypes, cursor                      ), &pConfig->pArchiveTypes[iArchiveType].pBackend,   sizeof(e_fs_backend*));
            E_COPY_MEMORY(E_OFFSET_PTR(pFS->pArchiveTypes, cursor + sizeof(e_fs_backend*)),  pConfig->pArchiveTypes[iArchiveType].pExtension, extensionLength + 1);

            cursor += e_archive_type_sizeof(&pConfig->pArchiveTypes[iArchiveType]);
        }
    } else {
        pFS->pArchiveTypes = NULL;
        pFS->archiveTypesAllocSize = 0;
    }

    /*
    If we were initialized with a stream we need to make sure we have a lock for it. This is needed for
    archives which might have multiple files accessing a stream across different threads. The archive
    will need to lock the stream so it doesn't get all mixed up between threads.
    */
    if (pConfig->pStream != NULL) {
        /* We want to grab the initial cursor of the stream so we can restore it in the case of an error. */
        if (e_stream_tell(pConfig->pStream, &initialStreamCursor) != E_SUCCESS) {
            initialStreamCursor = -1;
        }
    }

    /*
    We need a mutex for e_open_archive() and e_close_archive(). This needs to be recursive because
    during garbage collection we may end up closing archives in archives.
    */
    e_mutex_init(&pFS->archiveLock, E_MUTEX_TYPE_RECURSIVE);

    /*
    We need a mutex for the reference counting. This is needed because we may have multiple threads
    opening and closing files at the same time.
    */
    e_mutex_init(&pFS->refLock, E_MUTEX_TYPE_RECURSIVE);

    /* We're now ready to initialize the backend. */
    result = e_fs_backend_init(pBackend, pFS, pConfig->pBackendConfig, pConfig->pStream);
    if (result != E_NOT_IMPLEMENTED) {
        if (result != E_SUCCESS) {
            /*
            If we have a stream and the backend failed to initialize, it's possible that the cursor of the stream
            was moved as a result. To keep this as clean as possible, we're going to seek the cursor back to the
            initial position.
            */
            if (pConfig->pStream != NULL && initialStreamCursor != -1) {
                e_stream_seek(pConfig->pStream, initialStreamCursor, E_SEEK_SET);
            }

            e_free(pFS, e_fs_get_allocation_callbacks(pFS));
            return result;
        }
    } else {
        /* Getting here means the backend does not implement an init() function. This is not mandatory so we just assume successful.*/
        result = E_SUCCESS;
    }

    *ppFS = pFS;
    return E_SUCCESS;
}

E_API void e_fs_uninit(e_fs* pFS)
{
    if (pFS == NULL) {
        return;
    }

    /*
    We'll first garbage collect all archives. This should uninitialize any archives that are
    still open but have no references. After this call any archives that are still being
    referenced will remain open. Not quite sure what to do in this situation, but for now
    I'll check if any archives are still open and throw an assert. Not sure if this is
    overly aggressive - feedback welcome.
    */
    e_fs_gc_archives(pFS, E_GC_POLICY_FULL);

    /*
    A correct program should explicitly close their files. The reference count should be 1 when
    calling this function if the program is correct.
    */
    #if !defined(E_ENABLE_OPENED_FILES_ASSERT)
    {
        if (e_refcount(pFS) > 1) {
            E_ASSERT(!"You have outstanding opened files. You must close all files before uninitializing the e_fs object.");    /* <-- If you hit this assert but you're absolutely sure you've closed all your files, please submit a bug report with a reproducible test case. */
        }
    }
    #endif


    e_fs_backend_uninit(pFS->pBackend, pFS);

    e_free(pFS->pReadMountPoints, &pFS->allocationCallbacks);
    pFS->pReadMountPoints = NULL;

    e_free(pFS->pWriteMountPoints, &pFS->allocationCallbacks);
    pFS->pWriteMountPoints = NULL;

    e_free(pFS->pOpenedArchives, &pFS->allocationCallbacks);
    pFS->pOpenedArchives = NULL;

    e_mutex_destroy(&pFS->refLock);
    e_mutex_destroy(&pFS->archiveLock);

    e_free(pFS, &pFS->allocationCallbacks);
}

E_API e_result e_fs_ioctl(e_fs* pFS, int request, void* pArg)
{
    if (pFS == NULL) {
        return E_INVALID_ARGS;
    }

    return e_fs_backend_ioctl(pFS->pBackend, pFS, request, pArg);
}

E_API e_result e_fs_remove(e_fs* pFS, const char* pFilePath)
{
    if (pFS == NULL || pFilePath == NULL) {
        return E_INVALID_ARGS;
    }

    return e_fs_backend_remove(pFS->pBackend, pFS, pFilePath);
}

E_API e_result e_fs_rename(e_fs* pFS, const char* pOldName, const char* pNewName)
{
    if (pFS == NULL || pOldName == NULL || pNewName == NULL) {
        return E_INVALID_ARGS;
    }

    return e_fs_backend_rename(pFS->pBackend, pFS, pOldName, pNewName);
}

E_API e_result e_fs_mkdir(e_fs* pFS, const char* pPath, int options)
{
    char pRunningPathStack[1024];
    char* pRunningPathHeap = NULL;
    char* pRunningPath = pRunningPathStack;
    size_t runningPathLen = 0;
    e_path_iterator iSegment;
    const e_fs_backend* pBackend;
    e_mount_point* pMountPoint = NULL;
    const char* pMountPointPath = NULL;
    const char* pMountPointSubPath = NULL;

    pBackend = e_get_backend_or_default(pFS);

    if (pBackend == NULL) {
        return E_INVALID_ARGS;
    }

    if (pPath == NULL) {
        return E_INVALID_ARGS;
    }

    /* If we're using the default file system, ignore mount points since there's no real notion of them. */
    if (pFS == NULL) {
        options |= E_IGNORE_MOUNTS;
    }

    /* If we're using mount points we'll want to find the best one from our input path. */
    if ((options & E_IGNORE_MOUNTS) != 0) {
        pMountPoint = NULL;
        pMountPointPath = "";
        pMountPointSubPath = pPath;
    } else {
        pMountPoint = e_find_best_write_mount_point(pFS, pPath, &pMountPointPath, &pMountPointSubPath);
        if (pMountPoint == NULL) {
            return E_INVALID_FILE; /* Couldn't find a mount point. */
        }
    }


    /* We need to iterate over each segment and create the directory. If any of these fail we'll need to abort. */
    if (e_path_first(pMountPointSubPath, E_NULL_TERMINATED, &iSegment) != E_SUCCESS) {
        return E_SUCCESS;  /* It's an empty path. */
    }


    /* We need to pre-fill our running path with the mount point. */
    runningPathLen = strlen(pMountPointPath);
    if (runningPathLen + 1 >= sizeof(pRunningPathStack)) {
        pRunningPathHeap = (char*)e_malloc(runningPathLen + 1 + 1, e_fs_get_allocation_callbacks(pFS));
        if (pRunningPathHeap == NULL) {
            return E_OUT_OF_MEMORY;
        }

        pRunningPath = pRunningPathHeap;
    }

    E_COPY_MEMORY(pRunningPath, pMountPointPath, runningPathLen);
    pRunningPath[runningPathLen] = '\0';

    /* We need to make sure we have a trailing slash. */
    if (runningPathLen > 0 && pRunningPath[runningPathLen - 1] != '/') {
        pRunningPath[runningPathLen] = '/';
        runningPathLen += 1;
        pRunningPath[runningPathLen] = '\0';
    }


    for (;;) {
        e_result result;

        if (runningPathLen + iSegment.segmentLength + 1 + 1 >= sizeof(pRunningPathStack)) {
            if (pRunningPath == pRunningPathStack) {
                pRunningPathHeap = (char*)e_malloc(runningPathLen + iSegment.segmentLength + 1 + 1, e_fs_get_allocation_callbacks(pFS));
                if (pRunningPathHeap == NULL) {
                    return E_OUT_OF_MEMORY;
                }

                E_COPY_MEMORY(pRunningPathHeap, pRunningPathStack, runningPathLen);
                pRunningPath = pRunningPathHeap;
            } else {
                char* pNewRunningPathHeap;

                pNewRunningPathHeap = (char*)e_realloc(pRunningPathHeap, runningPathLen + iSegment.segmentLength + 1 + 1, e_fs_get_allocation_callbacks(pFS));
                if (pNewRunningPathHeap == NULL) {
                    e_free(pRunningPathHeap, e_fs_get_allocation_callbacks(pFS));
                    return E_OUT_OF_MEMORY;
                }

                pRunningPath = pNewRunningPathHeap;
            }
        }

        E_COPY_MEMORY(pRunningPath + runningPathLen, iSegment.pFullPath + iSegment.segmentOffset, iSegment.segmentLength);
        runningPathLen += iSegment.segmentLength;
        pRunningPath[runningPathLen] = '\0';

        result = e_fs_backend_mkdir(pBackend, pFS, pRunningPath);

        /* We just pretend to be successful if the directory already exists. */
        if (result == E_ALREADY_EXISTS) {
            result = E_SUCCESS;
        }

        if (result != E_SUCCESS) {
            if (pRunningPathHeap != NULL) {
                e_free(pRunningPathHeap, e_fs_get_allocation_callbacks(pFS));
            }

            return result;
        }

        pRunningPath[runningPathLen] = '/';
        runningPathLen += 1;

        result = e_path_next(&iSegment);
        if (result != E_SUCCESS) {
            break;
        }
    }

    if (pRunningPathHeap != NULL) {
        e_free(pRunningPathHeap, e_fs_get_allocation_callbacks(pFS));
    }

    return E_SUCCESS;
}

E_API e_result e_fs_info(e_fs* pFS, const char* pPath, int openMode, e_file_info* pInfo)
{
    if (pInfo == NULL) {
        return E_INVALID_ARGS;
    }

    E_ZERO_OBJECT(pInfo);

    return e_file_open_or_info(pFS, pPath, openMode, NULL, pInfo);
}

E_API e_stream* e_fs_get_stream(e_fs* pFS)
{
    if (pFS == NULL) {
        return NULL;
    }

    return pFS->pStream;
}

E_API const e_allocation_callbacks* e_fs_get_allocation_callbacks(e_fs* pFS)
{
    if (pFS == NULL) {
        return NULL;
    }

    return &pFS->allocationCallbacks;
}

E_API void* e_fs_get_backend_data(e_fs* pFS)
{
    size_t offset = sizeof(e_fs);

    if (pFS == NULL) {
        return NULL;
    }

    if (pFS->isOwnerOfArchiveTypes) {
        offset += pFS->archiveTypesAllocSize;
    }

    return E_OFFSET_PTR(pFS, offset);
}

E_API size_t e_fs_get_backend_data_size(e_fs* pFS)
{
    if (pFS == NULL) {
        return 0;
    }

    return pFS->backendDataSize;
}


static void e_on_refcount_changed(e_fs* pFS, e_uint32 newRefCount, e_uint32 oldRefCount)
{
    if (pFS->onRefCountChanged != NULL) {
        pFS->onRefCountChanged(pFS->pRefCountChangedUserData, pFS, newRefCount, oldRefCount);
    }
}

E_API e_fs* e_ref(e_fs* pFS)
{
    e_uint32 newRefCount;
    e_uint32 oldRefCount;

    if (pFS == NULL) {
        return NULL;
    }

    e_mutex_lock(&pFS->refLock);
    {
        oldRefCount = pFS->refCount;
        newRefCount = pFS->refCount + 1;

        pFS->refCount = newRefCount;

        e_on_refcount_changed(pFS, newRefCount, oldRefCount);
    }
    e_mutex_unlock(&pFS->refLock);

    return pFS;
}

E_API e_uint32 e_unref(e_fs* pFS)
{
    e_uint32 newRefCount;
    e_uint32 oldRefCount;

    if (pFS == NULL) {
        return 0;
    }

    if (pFS->refCount == 1) {
        #if !defined(E_ENABLE_OPENED_FILES_ASSERT)
        {
            E_ASSERT(!"ref/funref mismatch. Ensure all e_ref() calls are matched with e_unref() calls.");
        }
        #endif
        return pFS->refCount;
    }

    e_mutex_lock(&pFS->refLock);
    {
        oldRefCount = pFS->refCount;
        newRefCount = pFS->refCount - 1;

        pFS->refCount = newRefCount;

        e_on_refcount_changed(pFS, newRefCount, oldRefCount);
    }
    e_mutex_unlock(&pFS->refLock);

    return newRefCount;
}

E_API e_uint32 e_refcount(e_fs* pFS)
{
    e_uint32 refCount;

    if (pFS == NULL) {
        return 0;
    }

    e_mutex_lock(&pFS->refLock);
    {
        refCount = pFS->refCount;
    }
    e_mutex_unlock(&pFS->refLock);

    return refCount;
}


static void e_on_refcount_changed_internal(void* pUserData, e_fs* pFS, e_uint32 newRefCount, e_uint32 oldRefCount)
{
    e_fs* pOwnerFS;

    (void)pUserData;
    (void)pFS;
    (void)newRefCount;
    (void)oldRefCount;

    pOwnerFS = (e_fs*)pUserData;
    E_ASSERT(pOwnerFS != NULL);

    if (newRefCount == 1) {
        /* In this case there are no more files referencing this archive. We'll want to do some garbage collection. */
        e_fs_gc_archives(pOwnerFS, E_GC_POLICY_THRESHOLD);
    }
}

static e_result e_open_archive_nolock(e_fs* pFS, const e_fs_backend* pBackend, void* pBackendConfig, const char* pArchivePath, size_t archivePathLen, int openMode, e_fs** ppArchive)
{
    e_result result;
    e_fs* pArchive;
    e_fs_config archiveConfig;
    e_file* pArchiveFile;
    char pArchivePathNTStack[1024];
    char* pArchivePathNTHeap = NULL;    /* <-- Must be initialized to null. */
    char* pArchivePathNT;
    e_opened_archive* pOpenedArchive;

    /*
    The first thing to do is check if the archive has already been opened. If so, we just increment
    the reference count and return the already-loaded e_fs object.
    */
    pOpenedArchive = e_find_opened_archive(pFS, pArchivePath, archivePathLen);
    if (pOpenedArchive != NULL) {
        pArchive = pOpenedArchive->pArchive;
    } else {
        /*
        Getting here means the archive is not cached. We'll need to open it. Unfortunately our path is
        not null terminated so we'll need to do that now. We'll try to avoid a heap allocation if we
        can.
        */
        if (archivePathLen == E_NULL_TERMINATED) {
            pArchivePathNT = (char*)pArchivePath;   /* <-- Safe cast. We won't be modifying this. */
        } else {
            if (archivePathLen >= sizeof(pArchivePathNTStack)) {
                pArchivePathNTHeap = (char*)e_malloc(archivePathLen + 1, e_fs_get_allocation_callbacks(pFS));
                if (pArchivePathNTHeap == NULL) {
                    return E_OUT_OF_MEMORY;
                }

                pArchivePathNT = pArchivePathNTHeap;
            } else {
                pArchivePathNT = pArchivePathNTStack;
            }

            E_COPY_MEMORY(pArchivePathNT, pArchivePath, archivePathLen);
            pArchivePathNT[archivePathLen] = '\0';
        }

        result = e_file_open(pFS, pArchivePathNT, openMode, &pArchiveFile);
        if (result != E_SUCCESS) {
            e_free(pArchivePathNTHeap, e_fs_get_allocation_callbacks(pFS));
            return result;
        }

        archiveConfig = e_fs_config_init(pBackend, pBackendConfig, e_file_get_stream(pArchiveFile));
        archiveConfig.pAllocationCallbacks = e_fs_get_allocation_callbacks(pFS);
        archiveConfig.onRefCountChanged = e_on_refcount_changed_internal;
        archiveConfig.pRefCountChangedUserData = pFS;   /* The user data is always the e_fs object that owns this archive. */

        result = e_fs_init(&archiveConfig, &pArchive);
        e_free(pArchivePathNTHeap, e_fs_get_allocation_callbacks(pFS));

        if (result != E_SUCCESS) { /* <-- This is the result of e_fs_init().*/
            e_file_close(pArchiveFile);
            return result;
        }

        /*
        We need to support the ability to open archives within archives. To do this, the archive e_fs
        object needs to inherit the registered archive types. Fortunately this is easy because we do
        this as one single allocation which means we can just reference it directly. The API has a
        restriction that archive type registration cannot be modified after a file has been opened.
        */
        pArchive->pArchiveTypes         = pFS->pArchiveTypes;
        pArchive->archiveTypesAllocSize = pFS->archiveTypesAllocSize;
        pArchive->isOwnerOfArchiveTypes = E_FALSE;

        /* Add the new archive to the cache. */
        result = e_add_opened_archive(pFS, pArchive, pArchivePath, archivePathLen);
        if (result != E_SUCCESS) {
            e_fs_uninit(pArchive);
            e_file_close(pArchiveFile);
            return result;
        }
    }

    E_ASSERT(pArchive != NULL);

    *ppArchive = ((openMode & E_NO_INCREMENT_REFCOUNT) == 0) ? e_ref(pArchive) : pArchive;
    return E_SUCCESS;
}

E_API e_result e_open_archive_ex(e_fs* pFS, const e_fs_backend* pBackend, void* pBackendConfig, const char* pArchivePath, size_t archivePathLen, int openMode, e_fs** ppArchive)
{
    e_result result;

    if (ppArchive == NULL) {
        return E_INVALID_ARGS;
    }

    *ppArchive = NULL;

    if (pFS == NULL || pBackend == NULL || pArchivePath == NULL || archivePathLen == 0) {
        return E_INVALID_ARGS;
    }

    /*
    It'd be nice to be able to resolve the path here to eliminate any "." and ".." segments thereby
    making the path always consistent for a given archive. However, I cannot think of a way to do
    this robustly without having a backend-specific function like a `resolve()` or whatnot. The
    problem is that the path might be absolute, or it might be relative, and to get it right,
    parcticularly when dealing with different operating systems' ways of specifying an absolute
    path, you really need to have the support of the backend. I might add support for this later.
    */

    e_mutex_lock(&pFS->archiveLock);
    {
        result = e_open_archive_nolock(pFS, pBackend, pBackendConfig, pArchivePath, archivePathLen, openMode, ppArchive);
    }
    e_mutex_unlock(&pFS->archiveLock);

    return result;
}

E_API e_result e_open_archive(e_fs* pFS, const char* pArchivePath, int openMode, e_fs** ppArchive)
{
    e_result backendIteratorResult;
    e_registered_backend_iterator iBackend;
    e_result result;

    if (ppArchive == NULL) {
        return E_INVALID_ARGS;
    }

    *ppArchive = NULL;  /* Safety. */

    if (pFS == NULL || pArchivePath == NULL) {
        return E_INVALID_ARGS;
    }

    /*
    There can be multiple backends registered to the same extension. We just iterate over each one in order
    and use the first that works.
    */
    result = E_NO_BACKEND;
    for (backendIteratorResult = e_first_registered_backend(pFS, &iBackend); backendIteratorResult == E_SUCCESS; backendIteratorResult = e_next_registered_backend(&iBackend)) {
        if (e_path_extension_equal(pArchivePath, E_NULL_TERMINATED, iBackend.pExtension, iBackend.extensionLen)) {
            result = e_open_archive_ex(pFS, iBackend.pBackend, iBackend.pBackendConfig, pArchivePath, E_NULL_TERMINATED, openMode, ppArchive);
            if (result == E_SUCCESS) {
                return E_SUCCESS;
            }
        }
    }

    /* Failed to open from any archive backend. */
    return result;
}

E_API void e_close_archive(e_fs* pArchive)
{
    e_uint32 newRefCount;

    if (pArchive == NULL) {
        return;
    }

    /* In e_open_archive() we incremented the reference count. Now we need to decrement it. */
    newRefCount = e_unref(pArchive);

    /*
    If the reference count of the archive is 1 it means we don't currently have any files opened. We should
    look at garbage collecting.
    */
    if (newRefCount == 1) {
        /*
        This is a bit hacky and should probably change. When we initialized the archive in e_open_archive() we set the user
        data of the onRefCountChanged callback to be the e_fs object that owns this archive. We'll just use that to fire the
        garbage collection process.
        */
        e_fs* pArchiveOwnerFS = (e_fs*)pArchive->pRefCountChangedUserData;
        E_ASSERT(pArchiveOwnerFS != NULL);

        e_fs_gc_archives(pArchiveOwnerFS, E_GC_POLICY_THRESHOLD);
    }
}

static void e_gc_archives_nolock(e_fs* pFS, int policy)
{
    size_t unreferencedCount = 0;
    size_t collectionCount = 0;
    size_t cursor = 0;

    E_ASSERT(pFS != NULL);

    /*
    If we're doing a full garbage collection we need to recursively run the garbage collection process
    on opened archives.
    */
    if ((policy & E_GC_POLICY_FULL) != 0) {
        cursor = 0;
        while (cursor < pFS->openedArchivesSize) {
            e_opened_archive* pOpenedArchive = (e_opened_archive*)E_OFFSET_PTR(pFS->pOpenedArchives, cursor);
            E_ASSERT(pOpenedArchive != NULL);

            e_fs_gc_archives(pOpenedArchive->pArchive, policy);
            cursor += E_ALIGN(sizeof(e_fs*) + sizeof(size_t) + strlen(pOpenedArchive->pPath) + 1, E_SIZEOF_PTR);
        }
    }


    /* The first thing to do is count how many unreferenced archives there are. */
    cursor = 0;
    while (cursor < pFS->openedArchivesSize) {
        e_opened_archive* pOpenedArchive = (e_opened_archive*)E_OFFSET_PTR(pFS->pOpenedArchives, cursor);

        if (e_refcount(pOpenedArchive->pArchive) == 1) {
            unreferencedCount += 1;
        }

        /* Getting here means this archive is not the one we're looking for. */
        cursor += E_ALIGN(sizeof(e_fs*) + sizeof(size_t) + strlen(pOpenedArchive->pPath) + 1, E_SIZEOF_PTR);
    }

    /* Now we need to determine how many archives we should unload. */
    if ((policy & E_GC_POLICY_THRESHOLD) != 0) {
        if (unreferencedCount > e_fs_get_archive_gc_threshold(pFS)) {
            collectionCount = unreferencedCount - e_fs_get_archive_gc_threshold(pFS);
        } else {
            collectionCount = 0;    /* We're below the threshold. Don't collect anything. */
        }
    } else if ((policy & E_GC_POLICY_FULL) != 0) {
        collectionCount = unreferencedCount;
    } else {
        E_ASSERT(!"Invalid GC policy.");
    }

    /* Now we need to unload the archives. */
    cursor = 0;
    while (collectionCount > 0 && cursor < pFS->openedArchivesSize) {
        e_opened_archive* pOpenedArchive = (e_opened_archive*)E_OFFSET_PTR(pFS->pOpenedArchives, cursor);

        if (e_refcount(pOpenedArchive->pArchive) == 1) {
            e_file* pArchiveFile;

            /* For our cached archives, the stream should always be a file. */
            pArchiveFile = (e_file*)pOpenedArchive->pArchive->pStream;
            E_ASSERT(pArchiveFile != NULL);

            e_fs_uninit(pOpenedArchive->pArchive);
            e_file_close(pArchiveFile);

            /* We can remove the archive from the list only after it's been closed. */
            e_remove_opened_archive(pFS, pOpenedArchive);

            collectionCount -= 1;

            /* Note that we're not advancing the cursor here because we just removed this entry. */
        } else {
            cursor += E_ALIGN(sizeof(e_fs*) + sizeof(size_t) + strlen(pOpenedArchive->pPath) + 1, E_SIZEOF_PTR);
        }
    }
}

E_API void e_fs_gc_archives(e_fs* pFS, int policy)
{
    if (pFS == NULL) {
        return;
    }

    if (policy == 0 || ((policy & E_GC_POLICY_THRESHOLD) != 0 && (policy & E_GC_POLICY_FULL) != 0)) {
        return; /* Invalid policy. Must specify E_GC_POLICY_THRESHOLD or E_GC_POLICY_FULL, but not both. */
    }

    e_mutex_lock(&pFS->archiveLock);
    {
        e_gc_archives_nolock(pFS, policy);
    }
    e_mutex_unlock(&pFS->archiveLock);
}

E_API void e_fs_set_archive_gc_threshold(e_fs* pFS, size_t threshold)
{
    if (pFS == NULL) {
        return;
    }

    pFS->archiveGCThreshold = threshold;
}

E_API size_t e_fs_get_archive_gc_threshold(e_fs* pFS)
{
    if (pFS == NULL) {
        return 0;
    }

    return pFS->archiveGCThreshold;
}


static size_t e_file_duplicate_alloc_size(e_fs* pFS)
{
    return sizeof(e_file) + e_fs_backend_file_alloc_size(e_get_backend_or_default(pFS), pFS);
}

static void e_file_preinit_no_stream(e_file* pFile, e_fs* pFS, size_t backendDataSize)
{
    E_ASSERT(pFile != NULL);

    pFile->pFS = pFS;
    pFile->backendDataSize = backendDataSize;
}

static void e_file_uninit(e_file* pFile);


static e_result e_file_stream_read(e_stream* pStream, void* pDst, size_t bytesToRead, size_t* pBytesRead)
{
    return e_file_read((e_file*)pStream, pDst, bytesToRead, pBytesRead);
}

static e_result e_file_stream_write(e_stream* pStream, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten)
{
    return e_file_write((e_file*)pStream, pSrc, bytesToWrite, pBytesWritten);
}

static e_result e_file_stream_seek(e_stream* pStream, e_int64 offset, e_seek_origin origin)
{
    return e_file_seek((e_file*)pStream, offset, origin);
}

static e_result e_file_stream_tell(e_stream* pStream, e_int64* pCursor)
{
    return e_file_tell((e_file*)pStream, pCursor);
}

static size_t e_file_stream_alloc_size(e_stream* pStream)
{
    return e_file_duplicate_alloc_size(e_file_get_fs((e_file*)pStream));
}

static e_result e_file_stream_duplicate(e_stream* pStream, e_stream* pDuplicatedStream)
{
    e_result result;
    e_file* pStreamFile = (e_file*)pStream;
    e_file* pDuplicatedStreamFile = (e_file*)pDuplicatedStream;

    E_ASSERT(pStreamFile != NULL);
    E_ASSERT(pDuplicatedStreamFile != NULL);

    /* The stream will already have been initialized at a higher level in e_stream_duplicate(). */
    e_file_preinit_no_stream(pDuplicatedStreamFile, e_file_get_fs(pStreamFile), pStreamFile->backendDataSize);

    result = e_fs_backend_file_duplicate(e_get_backend_or_default(pStreamFile->pFS), pStreamFile, pDuplicatedStreamFile);
    if (result != E_SUCCESS) {
        return result;
    }

    return E_SUCCESS;
}

static void e_file_stream_uninit(e_stream* pStream)
{
    /* We need to uninitialize the file, but *not* free it. Freeing will be done at a higher level in e_stream_delete_duplicate(). */
    e_file_uninit((e_file*)pStream);
}

static e_stream_vtable e_file_stream_vtable =
{
    e_file_stream_read,
    e_file_stream_write,
    e_file_stream_seek,
    e_file_stream_tell,
    e_file_stream_alloc_size,
    e_file_stream_duplicate,
    e_file_stream_uninit
};




static const e_fs_backend* e_file_get_backend(e_file* pFile)
{
    return e_get_backend_or_default(e_file_get_fs(pFile));
}

static e_result e_open_or_info_from_archive(e_fs* pFS, const char* pFilePath, int openMode, e_file** ppFile, e_file_info* pInfo)
{
    /*
    NOTE: A lot of return values are E_DOES_NOT_EXIST. This is because this function will only be called
    in response to a E_DOES_NOT_EXIST in the first call to e_file_open() which makes this a logical result.
    */
    e_result result;
    e_path_iterator iFilePathSeg;
    e_path_iterator iFilePathSegLast;

    E_ASSERT(pFS != NULL);

    /*
    We can never open from an archive if we're opening in opaque mode. This mode is intended to
    be used such that only files exactly represented in the file system can be opened.
    */
    if (E_IS_OPAQUE(openMode)) {    
        return E_DOES_NOT_EXIST;
    }

    /* If no archive types have been configured we can abort early. */
    if (pFS->archiveTypesAllocSize == 0) {
        return E_DOES_NOT_EXIST;
    }

    /*
    We need to iterate over each segment in the path and then iterate over each file in that folder and
    check for archives. If we find an archive we simply try loading from that. Note that if the path
    segment itself points to an archive, we *must* open it from that archive because the caller has
    explicitly asked for that archive.
    */
    if (e_path_first(pFilePath, E_NULL_TERMINATED, &iFilePathSeg) != E_SUCCESS) {
        return E_DOES_NOT_EXIST;
    }

    /* Grab the last part of the file path so we can check if we're up to the file name. */
    e_path_last(pFilePath, E_NULL_TERMINATED, &iFilePathSegLast);

    do
    {
        e_result backendIteratorResult;
        e_registered_backend_iterator iBackend;
        e_bool32 isArchive = E_FALSE;

        /* Skip over "." and ".." segments. */
        if (e_strncmp(iFilePathSeg.pFullPath, ".", iFilePathSeg.segmentLength) == 0) {
            continue;
        }
        if (e_strncmp(iFilePathSeg.pFullPath, "..", iFilePathSeg.segmentLength) == 0) {
            continue;
        }

        /* If an archive has been explicitly listed in the path, we must try loading from that. */
        for (backendIteratorResult = e_first_registered_backend(pFS, &iBackend); backendIteratorResult == E_SUCCESS; backendIteratorResult = e_next_registered_backend(&iBackend)) {
            if (e_path_extension_equal(iFilePathSeg.pFullPath + iFilePathSeg.segmentOffset, iFilePathSeg.segmentLength, iBackend.pExtension, iBackend.extensionLen)) {
                isArchive = E_TRUE;

                /* This path points to an explicit archive. If this is the file we're trying to actually load, we'll want to handle that too. */
                if (e_path_iterators_compare(&iFilePathSeg, &iFilePathSegLast) == 0) {
                    /*
                    The archive file itself is the last segment in the path which means that's the file
                    we're actually trying to load. We shouldn't need to try opening this here because if
                    it existed and was able to be opened, it should have been done so at a higher level.
                    */
                    return E_DOES_NOT_EXIST;
                } else {
                    e_fs* pArchive;

                    result = e_open_archive_ex(pFS, iBackend.pBackend, iBackend.pBackendConfig, iFilePathSeg.pFullPath, iFilePathSeg.segmentOffset + iFilePathSeg.segmentLength, E_NO_INCREMENT_REFCOUNT | E_OPAQUE | openMode, &pArchive);
                    if (result != E_SUCCESS) {
                        /*
                        We failed to open the archive. If it's due to the archive not existing we just continue searching. Otherwise
                        a proper error code needs to be returned.
                        */
                        if (result != E_DOES_NOT_EXIST) {
                            return result;
                        } else {
                            continue;
                        }
                    }

                    result = e_file_open_or_info(pArchive, iFilePathSeg.pFullPath + iFilePathSeg.segmentOffset + iFilePathSeg.segmentLength + 1, openMode, ppFile, pInfo);
                    if (result != E_SUCCESS) {
                        if (e_refcount(pArchive) == 1) { e_fs_gc_archives(pFS, E_GC_POLICY_THRESHOLD); }
                        return result;
                    }

                    if (ppFile == NULL) {
                        /* We were only grabbing file info. We can garbage collect the archive straight away if necessary. */
                        if (e_refcount(pArchive) == 1) { e_fs_gc_archives(pFS, E_GC_POLICY_THRESHOLD); }
                    }

                    return E_SUCCESS;
                }
            }
        }

        /* If the path has an extension of an archive, but we still manage to get here, it means the archive doesn't exist. */
        if (isArchive) {
            return E_DOES_NOT_EXIST;
        }

        /*
        Getting here means this part of the path does not look like an archive. We will assume it's a folder and try
        iterating it using opaque mode to get the contents.
        */
        if (E_IS_VERBOSE(openMode)) {
            /*
            The caller has requested opening in verbose mode. In this case we don't want to be scanning for
            archives. Instead, any archives will be explicitly listed in the path. We just skip this path in
            this case.
            */
            continue;
        } else {
            /*
            Getting here means we're opening in transparent mode. We'll need to search for archives and check
            them one by one. This is the slow path.

            To do this we opaquely iterate over each file in the currently iterated file path. If any of these
            files are recognized as archives, we'll load up that archive and then try opening the file from
            there. If it works we return, otherwise we unload that archive and keep trying.
            */
            e_fs_iterator* pIterator;

            for (pIterator = e_fs_backend_first(e_get_backend_or_default(pFS), pFS, iFilePathSeg.pFullPath, iFilePathSeg.segmentOffset + iFilePathSeg.segmentLength); pIterator != NULL; pIterator = e_fs_backend_next(e_get_backend_or_default(pFS), pIterator)) {
                for (backendIteratorResult = e_first_registered_backend(pFS, &iBackend); backendIteratorResult == E_SUCCESS; backendIteratorResult = e_next_registered_backend(&iBackend)) {
                    if (e_path_extension_equal(pIterator->pName, pIterator->nameLen, iBackend.pExtension, iBackend.extensionLen)) {
                        /* Looks like an archive. We can load this one up and try opening from it. */
                        e_fs* pArchive;
                        char pArchivePathNTStack[1024];
                        char* pArchivePathNTHeap = NULL;    /* <-- Must be initialized to null. */
                        char* pArchivePathNT;
                        size_t archivePathLen;

                        archivePathLen = iFilePathSeg.segmentOffset + iFilePathSeg.segmentLength + 1 + pIterator->nameLen;
                        if (archivePathLen >= sizeof(pArchivePathNTStack)) {
                            pArchivePathNTHeap = (char*)e_malloc(archivePathLen + 1, e_fs_get_allocation_callbacks(pFS));
                            if (pArchivePathNTHeap == NULL) {
                                e_fs_backend_free_iterator(e_get_backend_or_default(pFS), pIterator);
                                return E_OUT_OF_MEMORY;
                            }

                            pArchivePathNT = pArchivePathNTHeap;
                        } else {
                            pArchivePathNT = pArchivePathNTStack;
                        }

                        E_COPY_MEMORY(pArchivePathNT, iFilePathSeg.pFullPath, iFilePathSeg.segmentOffset + iFilePathSeg.segmentLength);
                        pArchivePathNT[iFilePathSeg.segmentOffset + iFilePathSeg.segmentLength] = '/';
                        E_COPY_MEMORY(pArchivePathNT + iFilePathSeg.segmentOffset + iFilePathSeg.segmentLength + 1, pIterator->pName, pIterator->nameLen);
                        pArchivePathNT[archivePathLen] = '\0';

                        /* At this point we've constructed the archive name and we can now open it. */
                        result = e_open_archive_ex(pFS, iBackend.pBackend, iBackend.pBackendConfig, pArchivePathNT, E_NULL_TERMINATED, E_NO_INCREMENT_REFCOUNT | E_OPAQUE | openMode, &pArchive);
                        e_free(pArchivePathNTHeap, e_fs_get_allocation_callbacks(pFS));

                        if (result != E_SUCCESS) { /* <-- This is checking the result of e_open_archive_ex(). */
                            continue;   /* Failed to open this archive. Keep looking. */
                        }

                        /*
                        Getting here means we've successfully opened the archive. We can now try opening the file
                        from there. The path we load from will be the next segment in the path.
                        */
                        result = e_file_open_or_info(pArchive, iFilePathSeg.pFullPath + iFilePathSeg.segmentOffset + iFilePathSeg.segmentLength + 1, openMode, ppFile, pInfo);  /* +1 to skip the separator. */
                        if (result != E_SUCCESS) {
                            if (e_refcount(pArchive) == 1) { e_fs_gc_archives(pFS, E_GC_POLICY_THRESHOLD); }
                            continue;  /* Failed to open the file. Keep looking. */
                        }

                        /* The iterator is no longer required. */
                        e_fs_backend_free_iterator(e_get_backend_or_default(pFS), pIterator);
                        pIterator = NULL;

                        if (ppFile == NULL) {
                            /* We were only grabbing file info. We can garbage collect the archive straight away if necessary. */
                            if (e_refcount(pArchive) == 1) { e_fs_gc_archives(pFS, E_GC_POLICY_THRESHOLD); }
                        }

                        /* Getting here means we successfully opened the file. We're done. */
                        return E_SUCCESS;
                    }
                }

                /*
                Getting here means this file could not be loaded from any registered archive types. Just move on
                to the next file.
                */
            }

            /*
            Getting here means we couldn't find the file within any known archives in this directory. From here
            we just move onto the segment segment in the path and keep looking.
            */
        }
    } while (e_path_next(&iFilePathSeg) == E_SUCCESS);
    
    /* Getting here means we reached the end of the path and never did find the file. */
    return E_DOES_NOT_EXIST;
}

static void e_file_free(e_file** ppFile)
{
    e_file* pFile;

    if (ppFile == NULL) {
        return;
    }

    pFile = *ppFile;
    if (pFile == NULL) {
        return;
    }

    e_unref(pFile->pFS);
    e_free(pFile, e_fs_get_allocation_callbacks(pFile->pFS));

    *ppFile = NULL;
}

static e_result e_file_alloc(e_fs* pFS, e_file** ppFile)
{
    e_file* pFile;
    e_result result;
    const e_fs_backend* pBackend;
    size_t backendDataSizeInBytes = 0;

    E_ASSERT(ppFile != NULL);
    E_ASSERT(*ppFile == NULL);  /* <-- File must not already be allocated when calling this. */

    pBackend = e_get_backend_or_default(pFS);
    E_ASSERT(pBackend != NULL);

    backendDataSizeInBytes = e_fs_backend_file_alloc_size(pBackend, pFS);

    pFile = (e_file*)e_calloc(sizeof(e_file) + backendDataSizeInBytes, e_fs_get_allocation_callbacks(pFS));
    if (pFile == NULL) {
        return E_OUT_OF_MEMORY;
    }

    /* A file is a stream. */
    result = e_stream_init(&e_file_stream_vtable, &pFile->stream);
    if (result != 0) {
        e_free(pFile, e_fs_get_allocation_callbacks(pFS));
        return result;
    }

    pFile->pFS = pFS;
    pFile->backendDataSize = backendDataSizeInBytes;

    /* The reference count of the e_fs object needs to be incremented. It'll be decremented in e_file_free(). */
    e_ref(pFS);

    *ppFile = pFile;
    return E_SUCCESS;
}

static e_result e_file_alloc_if_necessary(e_fs* pFS, e_file** ppFile)
{
    E_ASSERT(ppFile != NULL);

    if (*ppFile == NULL) {
        return e_file_alloc(pFS, ppFile);
    } else {
        return E_SUCCESS;
    }
}

static e_result e_file_alloc_if_necessary_and_open_or_info(e_fs* pFS, const char* pFilePath, int openMode, e_file** ppFile, e_file_info* pInfo)
{
    e_result result;
    const e_fs_backend* pBackend;

    pBackend = e_get_backend_or_default(pFS);
    if (pBackend == NULL) {
        return E_INVALID_ARGS;
    }

    if (ppFile != NULL) {
        result = e_file_alloc_if_necessary(pFS, ppFile);
        if (result != E_SUCCESS) {
            *ppFile = NULL;
            return result;
        }
    }

    /*
    Take a copy of the file system's stream if necessary. We only need to do this if we're opening the file, and if
    the owner `e_fs` object `pFS` itself has a stream.
    */
    if (pFS != NULL && ppFile != NULL) {
        e_stream* pFSStream = pFS->pStream;
        if (pFSStream != NULL) {
            result = e_stream_duplicate(pFSStream, e_fs_get_allocation_callbacks(pFS), &(*ppFile)->pStreamForBackend);
            if (result != E_SUCCESS) {
                e_file_free(ppFile);
                return result;
            }
        }
    }

    /*
    This is the lowest level opening function. We never want to look at mounts when opening from here. The input
    file path should already be prefixed with the mount point.

    UPDATE: Actually don't want to explicitly append E_IGNORE_MOUNTS here because it can affect the behavior of
    passthrough style backends. Some backends, particularly E_SUB, will call straight into the owner `e_fs` object
    which might depend on those mounts being handled for correct behaviour.
    */
    /*openMode |= E_IGNORE_MOUNTS;*/

    if (ppFile != NULL) {
        /* Create the directory structure if necessary. */
        if ((openMode & E_WRITE) != 0 && (openMode & E_NO_CREATE_DIRS) == 0) {
            char pDirPathStack[1024];
            char* pDirPathHeap = NULL;
            char* pDirPath;
            int dirPathLen;

            dirPathLen = e_path_directory(pDirPathStack, sizeof(pDirPathStack), pFilePath, E_NULL_TERMINATED);
            if (dirPathLen >= (int)sizeof(pDirPathStack)) {
                pDirPathHeap = (char*)e_malloc(dirPathLen + 1, e_fs_get_allocation_callbacks(pFS));
                if (pDirPathHeap == NULL) {
                    e_stream_delete_duplicate((*ppFile)->pStreamForBackend, e_fs_get_allocation_callbacks(pFS));
                    e_file_free(ppFile);
                    return E_OUT_OF_MEMORY;
                }

                dirPathLen = e_path_directory(pDirPathHeap, dirPathLen + 1, pFilePath, E_NULL_TERMINATED);
                if (dirPathLen < 0) {
                    e_stream_delete_duplicate((*ppFile)->pStreamForBackend, e_fs_get_allocation_callbacks(pFS));
                    e_file_free(ppFile);
                    e_free(pDirPathHeap, e_fs_get_allocation_callbacks(pFS));
                    return E_ERROR;    /* Should never hit this. */
                }

                pDirPath = pDirPathHeap;
            } else {
                pDirPath = pDirPathStack;
            }

            result = e_fs_mkdir(pFS, pDirPath, E_IGNORE_MOUNTS);
            if (result != E_SUCCESS) {
                e_stream_delete_duplicate((*ppFile)->pStreamForBackend, e_fs_get_allocation_callbacks(pFS));
                e_file_free(ppFile);
                return result;
            }
        }

        result = e_fs_backend_file_open(pBackend, pFS, (*ppFile)->pStreamForBackend, pFilePath, openMode, *ppFile);

        if (result != E_SUCCESS) {
            e_stream_delete_duplicate((*ppFile)->pStreamForBackend, e_fs_get_allocation_callbacks(pFS));
        }

        /* Grab the info from the opened file if we're also grabbing that. */
        if (result == E_SUCCESS && pInfo != NULL) {
            e_fs_backend_file_info(pBackend, *ppFile, pInfo);
        }
    } else {
        if (pInfo != NULL) {
            result = e_fs_backend_info(pBackend, pFS, pFilePath, openMode, pInfo);
        } else {
            result = E_INVALID_ARGS;
        }
    }

    if (!E_IS_OPAQUE(openMode) && (openMode & E_WRITE) == 0) {
        /*
        If we failed to open the file because it doesn't exist we need to try loading it from an
        archive. We can only do this if the file is being loaded by an explicitly initialized e_fs
        object.
        */
        if (pFS != NULL && (result == E_DOES_NOT_EXIST || result == E_NOT_DIRECTORY)) {
            if (ppFile != NULL) {
                e_file_free(ppFile);
            }

            result = e_open_or_info_from_archive(pFS, pFilePath, openMode, ppFile, pInfo);
        }
    }

    return result;
}

static e_result e_validate_path(const char* pPath, size_t pathLen, int mode)
{
    if ((mode & E_NO_SPECIAL_DIRS) != 0) {
        e_path_iterator iPathSeg;
        e_result result;

        for (result = e_path_first(pPath, pathLen, &iPathSeg); result == E_SUCCESS; result = e_path_next(&iPathSeg)) {
            if (e_strncmp(iPathSeg.pFullPath, ".", iPathSeg.segmentLength) == 0) {
                return E_INVALID_ARGS;
            }

            if (e_strncmp(iPathSeg.pFullPath, "..", iPathSeg.segmentLength) == 0) {
                return E_INVALID_ARGS;
            }
        }
    }

    return E_SUCCESS;
}

E_API e_result e_file_open_or_info(e_fs* pFS, const char* pFilePath, int openMode, e_file** ppFile, e_file_info* pInfo)
{
    e_result result;
    e_result mountPointIerationResult;

    if (ppFile == NULL && pInfo == NULL) {
        return E_INVALID_ARGS;
    }

    if (pFilePath == NULL) {
        return E_INVALID_ARGS;
    }

    /* The open mode cannot be 0 when opening a file. It can only be 0 when retrieving info. */
    if (ppFile != NULL && openMode == 0) {
        return E_INVALID_ARGS;
    }

    result = e_validate_path(pFilePath, E_NULL_TERMINATED, openMode);
    if (result != E_SUCCESS) {
        return result;
    }

    if ((openMode & E_WRITE) != 0) {
        /* Opening in write mode. */
        if (pFS != NULL) {
            e_mount_point* pBestMountPoint = NULL;
            const char* pBestMountPointPath = NULL;
            const char* pBestMountPointFileSubPath = NULL;
            
            pBestMountPoint = e_find_best_write_mount_point(pFS, pFilePath, &pBestMountPointPath, &pBestMountPointFileSubPath);
            if (pBestMountPoint != NULL) {
                char pActualPathStack[1024];
                char* pActualPathHeap = NULL;
                char* pActualPath;
                int actualPathLen;
                char pActualPathCleanStack[1024];
                char* pActualPathCleanHeap = NULL;
                char* pActualPathClean;
                int actualPathCleanLen;
                unsigned int cleanOptions = (openMode & E_NO_ABOVE_ROOT_NAVIGATION);            

                /* If the mount point starts with a root segment, i.e. "/", we cannot allow navigation above that. */
                if (pBestMountPointPath[0] == '/' || pBestMountPointPath[0] == '\\') {
                    cleanOptions |= E_NO_ABOVE_ROOT_NAVIGATION;
                }


                /* Here is where we append the cleaned sub-path to the mount points actual path. */
                actualPathLen = e_path_append(pActualPathStack, sizeof(pActualPathStack), pBestMountPointPath, pBestMountPoint->pathLen, pBestMountPointFileSubPath, E_NULL_TERMINATED);
                if (actualPathLen > 0 && (size_t)actualPathLen >= sizeof(pActualPathStack)) {
                    /* Not enough room on the stack. Allocate on the heap. */
                    pActualPathHeap = (char*)e_malloc(actualPathLen + 1, e_fs_get_allocation_callbacks(pFS));
                    if (pActualPathHeap == NULL) {
                        return E_OUT_OF_MEMORY;
                    }

                    e_path_append(pActualPathHeap, actualPathLen + 1, pBestMountPointPath, pBestMountPoint->pathLen, pBestMountPointFileSubPath, E_NULL_TERMINATED);  /* <-- This should never fail. */
                    pActualPath = pActualPathHeap;
                } else {
                    pActualPath = pActualPathStack;
                }


                /* Now we need to clean the path. */
                actualPathCleanLen = e_path_normalize(pActualPathCleanStack, sizeof(pActualPathCleanStack), pActualPath, E_NULL_TERMINATED, cleanOptions);
                if (actualPathCleanLen < 0) {
                    e_free(pActualPathHeap, e_fs_get_allocation_callbacks(pFS));
                    return E_INVALID_OPERATION;    /* Most likely violating E_NO_ABOVE_ROOT_NAVIGATION. */
                }

                if (actualPathCleanLen >= (int)sizeof(pActualPathCleanStack)) {
                    pActualPathCleanHeap = (char*)e_malloc(actualPathCleanLen + 1, e_fs_get_allocation_callbacks(pFS));
                    if (pActualPathCleanHeap == NULL) {
                        e_free(pActualPathHeap, e_fs_get_allocation_callbacks(pFS));
                        return E_OUT_OF_MEMORY;
                    }

                    e_path_normalize(pActualPathCleanHeap, actualPathCleanLen + 1, pActualPath, E_NULL_TERMINATED, cleanOptions);    /* <-- This should never fail. */
                    pActualPathClean = pActualPathCleanHeap;
                } else {
                    pActualPathClean = pActualPathCleanStack;
                }

                e_free(pActualPathHeap, e_fs_get_allocation_callbacks(pFS));
                pActualPathHeap = NULL;


                /* We now have enough information to open the file. */
                result = e_file_alloc_if_necessary_and_open_or_info(pFS, pActualPathClean, openMode, ppFile, pInfo);

                e_free(pActualPathCleanHeap, e_fs_get_allocation_callbacks(pFS));
                pActualPathCleanHeap = NULL;

                if (result == E_SUCCESS) {
                    return E_SUCCESS;
                } else {
                    return E_DOES_NOT_EXIST;   /* Couldn't find the file from the best mount point. */
                }
            } else {
                return E_DOES_NOT_EXIST;   /* Couldn't find an appropriate mount point. */
            }
        } else {
            /*
            No "e_fs" object was supplied. Open using the default backend without using mount points. This is as if you were
            opening a file using `fopen()`.
            */
            if ((openMode & E_ONLY_MOUNTS) == 0) {
                return e_file_alloc_if_necessary_and_open_or_info(pFS, pFilePath, openMode, ppFile, pInfo);
            } else {
                /*
                Getting here means only the mount points can be used to open the file (cannot open straight from
                the file system natively).
                */
                return E_DOES_NOT_EXIST;
            }
        }
    } else {
        /* Opening in read mode. */
        e_mount_list_iterator iMountPoint;

        if (pFS != NULL && (openMode & E_IGNORE_MOUNTS) == 0) {
            for (mountPointIerationResult = e_mount_list_first(pFS->pReadMountPoints, &iMountPoint); mountPointIerationResult == E_SUCCESS; mountPointIerationResult = e_mount_list_next(&iMountPoint)) {
                /*
                The first thing to do is check if the start of our file path matches the mount point. If it
                doesn't match we just skip to the next mount point.
                */
                char  pFileSubPathCleanStack[1024];
                char* pFileSubPathCleanHeap = NULL;
                char* pFileSubPathClean;
                int fileSubPathCleanLen;
                unsigned int cleanOptions = (openMode & E_NO_ABOVE_ROOT_NAVIGATION);

                const char* pFileSubPath = e_path_trim_base(pFilePath, E_NULL_TERMINATED, iMountPoint.pMountPointPath, E_NULL_TERMINATED);
                if (pFileSubPath == NULL) {
                    continue;
                }


                /* If the mount point starts with a root segment, i.e. "/", we cannot allow navigation above that. */
                if (iMountPoint.pMountPointPath[0] == '/' || iMountPoint.pMountPointPath[0] == '\\') {
                    cleanOptions |= E_NO_ABOVE_ROOT_NAVIGATION;
                }

                /* We need to clean the file sub-path, but can skip it if E_NO_SPECIAL_DIRS is specified since it's implied. */
                if ((openMode & E_NO_SPECIAL_DIRS) == 0) {
                    fileSubPathCleanLen = e_path_normalize(pFileSubPathCleanStack, sizeof(pFileSubPathCleanStack), pFileSubPath, E_NULL_TERMINATED, cleanOptions);
                    if (fileSubPathCleanLen < 0) {
                        continue;    /* Most likely violating E_NO_ABOVE_ROOT_NAVIGATION. Keep looking. */
                    }

                    if (fileSubPathCleanLen >= (int)sizeof(pFileSubPathCleanStack)) {
                        pFileSubPathCleanHeap = (char*)e_malloc(fileSubPathCleanLen + 1, e_fs_get_allocation_callbacks(pFS));
                        if (pFileSubPathCleanHeap == NULL) {
                            return E_OUT_OF_MEMORY;
                        }

                        e_path_normalize(pFileSubPathCleanHeap, fileSubPathCleanLen + 1, pFileSubPath, E_NULL_TERMINATED, cleanOptions);    /* <-- This should never fail. */
                        pFileSubPathClean = pFileSubPathCleanHeap;
                    } else {
                        pFileSubPathClean = pFileSubPathCleanStack;
                    }
                } else {
                    pFileSubPathClean = (char*)pFileSubPath;  /* Safe cast. Will not be modified past this point. */
                    fileSubPathCleanLen = (int)strlen(pFileSubPathClean);
                }


                /* The mount point could either be a directory or an archive. Both of these require slightly different handling. */
                if (iMountPoint.pArchive != NULL) {
                    /* The mount point is an archive. This is the simpler case. We just load the file directly from the archive. */
                    result = e_file_open_or_info(iMountPoint.pArchive, pFileSubPathClean, openMode, ppFile, pInfo);
                    if (result == E_SUCCESS) {
                        return E_SUCCESS;
                    } else {
                        /* Failed to load from this archive. Keep looking. */
                    }
                } else {
                    /* The mount point is a directory. We need to combine the sub-path with the mount point's original path and then load the file. */
                    char  pActualPathStack[1024];
                    char* pActualPathHeap = NULL;
                    char* pActualPath;
                    int actualPathLen;

                    actualPathLen = e_path_append(pActualPathStack, sizeof(pActualPathStack), iMountPoint.pPath, E_NULL_TERMINATED, pFileSubPathClean, fileSubPathCleanLen);
                    if (actualPathLen > 0 && (size_t)actualPathLen >= sizeof(pActualPathStack)) {
                        /* Not enough room on the stack. Allocate on the heap. */
                        pActualPathHeap = (char*)e_malloc(actualPathLen + 1, e_fs_get_allocation_callbacks(pFS));
                        if (pActualPathHeap == NULL) {
                            return E_OUT_OF_MEMORY;
                        }

                        e_path_append(pActualPathHeap, actualPathLen + 1, iMountPoint.pPath, E_NULL_TERMINATED, pFileSubPathClean, fileSubPathCleanLen);   /* <-- This should never fail. */
                        pActualPath = pActualPathHeap;
                    } else {
                        pActualPath = pActualPathStack;
                    }

                    result = e_file_alloc_if_necessary_and_open_or_info(pFS, pActualPath, openMode, ppFile, pInfo);

                    if (pActualPathHeap != NULL) {
                        e_free(pActualPathHeap, e_fs_get_allocation_callbacks(pFS));
                        pActualPathHeap = NULL;
                    }

                    if (result == E_SUCCESS) {
                        return E_SUCCESS;
                    } else {
                        /* Failed to load from this directory. Keep looking. */
                    }
                }
            }
        }

        /* If we get here it means we couldn't find the file from our search paths. Try opening directly. */
        if ((openMode & E_ONLY_MOUNTS) == 0) {
            result = e_file_alloc_if_necessary_and_open_or_info(pFS, pFilePath, openMode, ppFile, pInfo);
            if (result == E_SUCCESS) {
                return E_SUCCESS;
            }
        } else {
            /*
            Getting here means only the mount points can be used to open the file (cannot open straight from
            the file system natively) and the file was unable to be opened from any of them. We need to
            return an error in this case.
            */
            result = E_DOES_NOT_EXIST;
        }

        /* Getting here means we couldn't open the file from any mount points, nor could we open it directly. */
        if (ppFile != NULL) {
            e_file_free(ppFile);
        }
    }

    E_ASSERT(result != E_SUCCESS);
    return result;
}

E_API e_result e_file_open(e_fs* pFS, const char* pFilePath, int openMode, e_file** ppFile)
{
    if (ppFile == NULL) {
        return E_INVALID_ARGS;
    }

    *ppFile = NULL;

    if ((openMode & E_TEMP) == E_TEMP) {
        /*
        We're creating a temporary file. We can use e_mktmp() to generate a file path for us. The
        input path will act as the prefix.

        We'll use a stack allocation for the temporary file path. We can make this more robust later
        by checking for E_PATH_TOO_LONG and allocating on the heap if necessary.
        */
        char pTmpPath[4096];
        e_result result;

        result = e_mktmp(pFilePath, pTmpPath, sizeof(pTmpPath), E_MKTMP_FILE);
        if (result != E_SUCCESS) {
            return result;
        }

        return e_file_open_or_info(pFS, pTmpPath, openMode | E_IGNORE_MOUNTS, ppFile, NULL);
    } else {
        return e_file_open_or_info(pFS, pFilePath, openMode, ppFile, NULL);
    }
}

E_API e_result e_file_open_from_handle(e_fs* pFS, void* hBackendFile, e_file** ppFile)
{
    e_result result;

    if (ppFile == NULL) {
        return E_INVALID_ARGS;
    }

    *ppFile = NULL;

    result = e_file_alloc_if_necessary(pFS, ppFile);
    if (result != E_SUCCESS) {
        *ppFile = NULL;
        return result;
    }

    result = e_fs_backend_file_open_handle(e_get_backend_or_default(pFS), pFS, hBackendFile, *ppFile);
    if (result != E_SUCCESS) {
        e_file_free(ppFile);
        return result;
    }

    return E_SUCCESS;
}

static void e_file_uninit(e_file* pFile)
{
    e_fs_backend_file_close(e_get_backend_or_default(e_file_get_fs(pFile)), pFile);
}

E_API void e_file_close(e_file* pFile)
{
    const e_fs_backend* pBackend = e_file_get_backend(pFile);

    E_ASSERT(pBackend != NULL);
    (void)pBackend;

    if (pFile == NULL) {
        return;
    }

    e_file_uninit(pFile);

    if (pFile->pStreamForBackend != NULL) {
        e_stream_delete_duplicate(pFile->pStreamForBackend, e_fs_get_allocation_callbacks(pFile->pFS));
    }

    e_file_free(&pFile);
}

E_API e_result e_file_read(e_file* pFile, void* pDst, size_t bytesToRead, size_t* pBytesRead)
{
    e_result result;
    size_t bytesRead;
    const e_fs_backend* pBackend;

    if (pBytesRead != NULL) {
        *pBytesRead = 0;
    }

    if (pFile == NULL || pDst == NULL) {
        return E_INVALID_ARGS;
    }

    pBackend = e_file_get_backend(pFile);
    E_ASSERT(pBackend != NULL);

    bytesRead = 0;  /* <-- Just in case the backend doesn't clear this to zero. */
    result = e_fs_backend_file_read(pBackend, pFile, pDst, bytesToRead, &bytesRead);

    if (pBytesRead != NULL) {
        *pBytesRead = bytesRead;
    }

    if (result != E_SUCCESS) {
        /* We can only return E_AT_END if the number of bytes read was 0. */
        if (result == E_AT_END) {
            if (bytesRead > 0) {
                result = E_SUCCESS;
            }
        }

        return result;
    }

    /*
    If pBytesRead is null it means the caller will never be able to tell exactly how many bytes were read. In this
    case, if we didn't read the exact number of bytes that were requested we'll need to return an error.
    */
    if (pBytesRead == NULL) {
        if (bytesRead != bytesToRead) {
            return E_ERROR;
        }
    }

    return E_SUCCESS;
}

E_API e_result e_file_write(e_file* pFile, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten)
{
    e_result result;
    size_t bytesWritten;
    const e_fs_backend* pBackend;

    if (pBytesWritten != NULL) {
        *pBytesWritten = 0;
    }

    if (pFile == NULL || pSrc == NULL) {
        return E_INVALID_ARGS;
    }

    pBackend = e_file_get_backend(pFile);
    E_ASSERT(pBackend != NULL);

    bytesWritten = 0;  /* <-- Just in case the backend doesn't clear this to zero. */
    result = e_fs_backend_file_write(pBackend, pFile, pSrc, bytesToWrite, &bytesWritten);

    if (pBytesWritten != NULL) {
        *pBytesWritten = bytesWritten;
    }

    /*
    As with reading, if the caller passes in null for pBytesWritten we need to return an error if
    the exact number of bytes couldn't be written.
    */
    if (pBytesWritten == NULL) {
        if (bytesWritten != bytesToWrite) {
            return E_ERROR;
        }
    }

    return result;
}

E_API e_result e_file_writef(e_file* pFile, const char* fmt, ...)
{
    va_list args;
    e_result result;

    va_start(args, fmt);
    result = e_file_writefv(pFile, fmt, args);
    va_end(args);

    return result;
}

E_API e_result e_file_writefv(e_file* pFile, const char* fmt, va_list args)
{
    return e_stream_writefv(e_file_get_stream(pFile), fmt, args);
}

E_API e_result e_file_seek(e_file* pFile, e_int64 offset, e_seek_origin origin)
{
    const e_fs_backend* pBackend;

    if (pFile == NULL) {
        return E_INVALID_ARGS;
    }

    pBackend = e_file_get_backend(pFile);
    E_ASSERT(pBackend != NULL);

    return e_fs_backend_file_seek(pBackend, pFile, offset, origin);
}

E_API e_result e_file_tell(e_file* pFile, e_int64* pOffset)
{
    const e_fs_backend* pBackend;

    if (pOffset == NULL) {
        return E_INVALID_ARGS;  /* Doesn't make sense to be calling this without an output parameter. */
    }

    *pOffset = 0;

    if (pFile == NULL) {
        return E_INVALID_ARGS;
    }

    pBackend = e_file_get_backend(pFile);
    E_ASSERT(pBackend != NULL);

    return e_fs_backend_file_tell(pBackend, pFile, pOffset);
}

E_API e_result e_file_flush(e_file* pFile)
{
    const e_fs_backend* pBackend;

    if (pFile == NULL) {
        return E_INVALID_ARGS;
    }

    pBackend = e_file_get_backend(pFile);
    E_ASSERT(pBackend != NULL);

    return e_fs_backend_file_flush(pBackend, pFile);
}

E_API e_result e_file_get_info(e_file* pFile, e_file_info* pInfo)
{
    const e_fs_backend* pBackend;

    if (pInfo == NULL) {
        return E_INVALID_ARGS;  /* It doesn't make sense to call this without an info parameter. */
    }

    memset(pInfo, 0, sizeof(*pInfo));

    if (pFile == NULL) {
        return E_INVALID_ARGS;
    }

    pBackend = e_file_get_backend(pFile);
    E_ASSERT(pBackend != NULL);

    return e_fs_backend_file_info(pBackend, pFile, pInfo);
}

E_API e_result e_file_duplicate(e_file* pFile, e_file** ppDuplicate)
{
    e_result result;

    if (ppDuplicate == NULL) {
        return E_INVALID_ARGS;
    }

    *ppDuplicate = NULL;

    if (pFile == NULL) {
        return E_INVALID_ARGS;
    }

    result = e_file_alloc(pFile->pFS, ppDuplicate);
    if (result != E_SUCCESS) {
        return result;
    }

    return e_fs_backend_file_duplicate(e_get_backend_or_default(e_file_get_fs(pFile)), pFile, *ppDuplicate);
}

E_API void* e_file_get_backend_data(e_file* pFile)
{
    if (pFile == NULL) {
        return NULL;
    }

    return E_OFFSET_PTR(pFile, sizeof(e_file));
}

E_API size_t e_file_get_backend_data_size(e_file* pFile)
{
    if (pFile == NULL) {
        return 0;
    }

    return pFile->backendDataSize;
}

E_API e_stream* e_file_get_stream(e_file* pFile)
{
    return (e_stream*)pFile;
}

E_API e_fs* e_file_get_fs(e_file* pFile)
{
    if (pFile == NULL) {
        return NULL;
    }

    return pFile->pFS;
}


typedef struct e_iterator_item
{
    size_t nameLen;
    e_file_info info;
} e_iterator_item;

typedef struct e_iterator_internal
{
    e_fs_iterator base;
    size_t itemIndex;   /* The index of the current item we're iterating. */
    size_t itemCount;
    size_t itemDataSize;
    size_t dataSize;
    size_t allocSize;
    e_iterator_item** ppItems;
} e_iterator_internal;

static size_t e_iterator_item_sizeof(size_t nameLen)
{
    return E_ALIGN(sizeof(e_iterator_item) + nameLen + 1, E_SIZEOF_PTR);   /* +1 for the null terminator. */
}

static char* e_iterator_item_name(e_iterator_item* pItem)
{
    return (char*)pItem + sizeof(*pItem);
}

static void e_iterator_internal_resolve_public_members(e_iterator_internal* pIterator)
{
    E_ASSERT(pIterator != NULL);

    pIterator->base.pName   = e_iterator_item_name(pIterator->ppItems[pIterator->itemIndex]);
    pIterator->base.nameLen = pIterator->ppItems[pIterator->itemIndex]->nameLen;
    pIterator->base.info    = pIterator->ppItems[pIterator->itemIndex]->info;
}

static e_iterator_item* e_iterator_internal_find(e_iterator_internal* pIterator, const char* pName)
{
    /*
    We cannot use ppItems here because this function will be called before that has been set up. Instead we need
    to use a cursor and run through each item linearly. This is unsorted.
    */
    size_t iItem;
    size_t cursor = 0;

    for (iItem = 0; iItem < pIterator->itemCount; iItem += 1) {
        e_iterator_item* pItem = (e_iterator_item*)E_OFFSET_PTR(pIterator, sizeof(e_iterator_internal) + cursor);
        if (e_strncmp(e_iterator_item_name(pItem), pName, pItem->nameLen) == 0) {
            return pItem;
        }

        cursor += e_iterator_item_sizeof(pItem->nameLen);
    }

    return NULL;
}

static e_iterator_internal* e_iterator_internal_append(e_iterator_internal* pIterator, e_fs_iterator* pOther, e_fs* pFS, int mode)
{
    size_t newItemSize;
    e_iterator_item* pNewItem;

    E_ASSERT(pOther != NULL);

    /* Skip over any "." and ".." entries. */
    if ((pOther->pName[0] == '.' && pOther->pName[1] == 0) || (pOther->pName[0] == '.' && pOther->pName[1] == '.' && pOther->pName[2] == 0)) {
        return pIterator;
    }


    /* If we're in transparent mode, we don't want to add any archives. Instead we want to open them and iterate them recursively. */
    (void)mode;


    /* Check if the item already exists. If so, skip it. */
    if (pIterator != NULL) {
        pNewItem = e_iterator_internal_find(pIterator, pOther->pName);
        if (pNewItem != NULL) {
            return pIterator;   /* Already exists. Skip it. */
        }
    }

    /* At this point we're ready to append the item. */
    newItemSize = e_iterator_item_sizeof(pOther->nameLen);
    if (pIterator == NULL || pIterator->dataSize + newItemSize + sizeof(e_iterator_item*) > pIterator->allocSize) {
        e_iterator_internal* pNewIterator;
        size_t newAllocSize;

        if (pIterator == NULL) {
            newAllocSize = 4096;
            if (newAllocSize < (sizeof(*pIterator) + newItemSize + sizeof(e_iterator_item*))) {
                newAllocSize = (sizeof(*pIterator) + newItemSize + sizeof(e_iterator_item*));
            }
        } else {
            newAllocSize = pIterator->allocSize * 2;
            if (newAllocSize < (pIterator->dataSize + newItemSize + sizeof(e_iterator_item*))) {
                newAllocSize = (pIterator->dataSize + newItemSize + sizeof(e_iterator_item*));
            }
        }

        pNewIterator = (e_iterator_internal*)e_realloc(pIterator, newAllocSize, e_fs_get_allocation_callbacks(pFS));
        if (pNewIterator == NULL) {
            return pIterator;
        }

        if (pIterator == NULL) {
            E_ZERO_MEMORY(pNewIterator, sizeof(e_iterator_internal));
            pNewIterator->dataSize = sizeof(e_iterator_internal);
        }

        pIterator = pNewIterator;
        pIterator->allocSize = newAllocSize;
    }

    /* We can now copy the information over to the information. */
    pNewItem = (e_iterator_item*)E_OFFSET_PTR(pIterator, sizeof(e_iterator_internal) + pIterator->itemDataSize);
    E_COPY_MEMORY(e_iterator_item_name(pNewItem), pOther->pName, pOther->nameLen + 1);   /* +1 for the null terminator. */
    pNewItem->nameLen = pOther->nameLen;
    pNewItem->info    = pOther->info;

    pIterator->itemDataSize += newItemSize;
    pIterator->dataSize     += newItemSize + sizeof(e_iterator_item*);
    pIterator->itemCount    += 1;

    return pIterator;
}


static int e_iterator_item_compare(void* pUserData, const void* pA, const void* pB)
{
    e_iterator_item* pItemA = *(e_iterator_item**)pA;
    e_iterator_item* pItemB = *(e_iterator_item**)pB;
    const char* pNameA = e_iterator_item_name(pItemA);
    const char* pNameB = e_iterator_item_name(pItemB);
    int compareResult;

    (void)pUserData;

    compareResult = e_strncmp(pNameA, pNameB, E_MIN(pItemA->nameLen, pItemB->nameLen));
    if (compareResult == 0) {
        if (pItemA->nameLen < pItemB->nameLen) {
            compareResult = -1;
        } else if (pItemA->nameLen > pItemB->nameLen) {
            compareResult =  1;
        }
    }

    return compareResult;
}

static void e_iterator_internal_sort(e_iterator_internal* pIterator)
{
    e_sort(pIterator->ppItems, pIterator->itemCount, sizeof(e_iterator_item*), e_iterator_item_compare, NULL);
}

static e_iterator_internal* e_iterator_internal_gather(e_iterator_internal* pIterator, const e_fs_backend* pBackend, e_fs* pFS, const char* pDirectoryPath, size_t directoryPathLen, int mode)
{
    e_result result;
    e_fs_iterator* pInnerIterator;

    E_ASSERT(pBackend != NULL);

    /* Regular files take priority. */
    for (pInnerIterator = e_fs_backend_first(pBackend, pFS, pDirectoryPath, directoryPathLen); pInnerIterator != NULL; pInnerIterator = e_fs_backend_next(pBackend, pInnerIterator)) {
        pIterator = e_iterator_internal_append(pIterator, pInnerIterator, pFS, mode);
    }

    /* Now we need to gather from archives, but only if we're not in opaque mode. */
    if (pFS != NULL && !E_IS_OPAQUE(mode)) {
        e_path_iterator iDirPathSeg;

        /* If no archive types have been configured we can abort early. */
        if (pFS->archiveTypesAllocSize == 0) {
            return pIterator;
        }

        /*
        Just like when opening a file we need to inspect each segment of the path. For each segment
        we need to check for archives. This is where transparent mode becomes very slow because it
        needs to scan every archive. For opaque mode we need only check for explicitly listed archives
        in the search path.
        */
        if (e_path_first(pDirectoryPath, directoryPathLen, &iDirPathSeg) != E_SUCCESS) {
            return pIterator;
        }

        do
        {
            e_result backendIteratorResult;
            e_registered_backend_iterator iBackend;
            e_bool32 isArchive = E_FALSE;
            size_t dirPathRemainingLen;

            /* Skip over "." and ".." segments. */
            if (e_strncmp(iDirPathSeg.pFullPath, ".", iDirPathSeg.segmentLength) == 0) {
                continue;
            }
            if (e_strncmp(iDirPathSeg.pFullPath, "..", iDirPathSeg.segmentLength) == 0) {
                continue;
            }

            if (e_path_is_last(&iDirPathSeg)) {
                dirPathRemainingLen = 0;
            } else {
                if (directoryPathLen == E_NULL_TERMINATED) {
                    dirPathRemainingLen = E_NULL_TERMINATED;
                } else {
                    dirPathRemainingLen = directoryPathLen - (iDirPathSeg.segmentOffset + iDirPathSeg.segmentLength + 1);
                }
            }

            /* If an archive has been explicitly listed in the path, we must try iterating from that. */
            for (backendIteratorResult = e_first_registered_backend(pFS, &iBackend); backendIteratorResult == E_SUCCESS; backendIteratorResult = e_next_registered_backend(&iBackend)) {
                if (e_path_extension_equal(iDirPathSeg.pFullPath + iDirPathSeg.segmentOffset, iDirPathSeg.segmentLength, iBackend.pExtension, iBackend.extensionLen)) {
                    e_fs* pArchive;
                    e_fs_iterator* pArchiveIterator;

                    isArchive = E_TRUE;

                    result = e_open_archive_ex(pFS, iBackend.pBackend, iBackend.pBackendConfig, iDirPathSeg.pFullPath, iDirPathSeg.segmentOffset + iDirPathSeg.segmentLength, E_READ | E_IGNORE_MOUNTS | mode, &pArchive);
                    if (result != E_SUCCESS) {
                        /*
                        We failed to open the archive. If it's due to the archive not existing we just continue searching. Otherwise
                        we just bomb out.
                        */
                        if (result != E_DOES_NOT_EXIST) {
                            e_close_archive(pArchive);
                            return pIterator;
                        } else {
                            continue;
                        }
                    }

                    if (dirPathRemainingLen == 0) {
                        pArchiveIterator = e_fs_first_ex(pArchive, "", 0, mode);
                    } else {
                        pArchiveIterator = e_fs_first_ex(pArchive, iDirPathSeg.pFullPath + iDirPathSeg.segmentOffset + iDirPathSeg.segmentLength + 1, dirPathRemainingLen, mode);
                    }

                    while (pArchiveIterator != NULL) {
                        pIterator = e_iterator_internal_append(pIterator, pArchiveIterator, pFS, mode);
                        pArchiveIterator = e_fs_next(pArchiveIterator);
                    }

                    e_close_archive(pArchive);
                    break;
                }
            }

            /* If the path has an extension of an archive, but we still manage to get here, it means the archive doesn't exist. */
            if (isArchive) {
                return pIterator;
            }

            /*
            Getting here means this part of the path does not look like an archive. We will assume it's a folder and try
            iterating it using opaque mode to get the contents.
            */
            if (E_IS_VERBOSE(mode)) {
                /*
                The caller has requested opening in verbose mode. In this case we don't want to be scanning for
                archives. Instead, any archives will be explicitly listed in the path. We just skip this path in
                this case.
                */
                continue;
            } else {
                /*
                Getting here means we're in transparent mode. We'll need to search for archives and check them one
                by one. This is the slow path.

                To do this we opaquely iterate over each file in the currently iterated file path. If any of these
                files are recognized as archives, we'll load up that archive and then try iterating from there.
                */
                for (pInnerIterator = e_fs_backend_first(pBackend, pFS, iDirPathSeg.pFullPath, iDirPathSeg.segmentOffset + iDirPathSeg.segmentLength); pInnerIterator != NULL; pInnerIterator = e_fs_backend_next(pBackend, pInnerIterator)) {
                    for (backendIteratorResult = e_first_registered_backend(pFS, &iBackend); backendIteratorResult == E_SUCCESS; backendIteratorResult = e_next_registered_backend(&iBackend)) {
                        if (e_path_extension_equal(pInnerIterator->pName, pInnerIterator->nameLen, iBackend.pExtension, iBackend.extensionLen)) {
                            /* Looks like an archive. We can load this one up and try iterating from it. */
                            e_fs* pArchive;
                            e_fs_iterator* pArchiveIterator;
                            char pArchivePathNTStack[1024];
                            char* pArchivePathNTHeap = NULL;    /* <-- Must be initialized to null. */
                            char* pArchivePathNT;
                            size_t archivePathLen;

                            archivePathLen = iDirPathSeg.segmentOffset + iDirPathSeg.segmentLength + 1 + pInnerIterator->nameLen;
                            if (archivePathLen >= sizeof(pArchivePathNTStack)) {
                                pArchivePathNTHeap = (char*)e_malloc(archivePathLen + 1, e_fs_get_allocation_callbacks(pFS));
                                if (pArchivePathNTHeap == NULL) {
                                    e_fs_backend_free_iterator(pBackend, pInnerIterator);
                                    return pIterator;
                                }

                                pArchivePathNT = pArchivePathNTHeap;
                            } else {
                                pArchivePathNT = pArchivePathNTStack;
                            }

                            E_COPY_MEMORY(pArchivePathNT, iDirPathSeg.pFullPath, iDirPathSeg.segmentOffset + iDirPathSeg.segmentLength);
                            pArchivePathNT[iDirPathSeg.segmentOffset + iDirPathSeg.segmentLength] = '/';
                            E_COPY_MEMORY(pArchivePathNT + iDirPathSeg.segmentOffset + iDirPathSeg.segmentLength + 1, pInnerIterator->pName, pInnerIterator->nameLen);
                            pArchivePathNT[archivePathLen] = '\0';

                            /* At this point we've constructed the archive name and we can now open it. */
                            result = e_open_archive_ex(pFS, iBackend.pBackend, iBackend.pBackendConfig, pArchivePathNT, E_NULL_TERMINATED, E_READ | E_IGNORE_MOUNTS | mode, &pArchive);
                            e_free(pArchivePathNTHeap, e_fs_get_allocation_callbacks(pFS));

                            if (result != E_SUCCESS) { /* <-- This is checking the result of e_open_archive_ex(). */
                                continue;   /* Failed to open this archive. Keep looking. */
                            }


                            if (dirPathRemainingLen == 0) {
                                pArchiveIterator = e_fs_first_ex(pArchive, "", 0, mode);
                            } else {
                                pArchiveIterator = e_fs_first_ex(pArchive, iDirPathSeg.pFullPath + iDirPathSeg.segmentOffset + iDirPathSeg.segmentLength + 1, dirPathRemainingLen, mode);
                            }

                            while (pArchiveIterator != NULL) {
                                pIterator = e_iterator_internal_append(pIterator, pArchiveIterator, pFS, mode);
                                pArchiveIterator = e_fs_next(pArchiveIterator);
                            }

                            e_close_archive(pArchive);
                            break;
                        }
                    }
                }
            }
        } while (e_path_next(&iDirPathSeg) == E_SUCCESS);
    }

    return pIterator;
}

E_API e_fs_iterator* e_fs_first_ex(e_fs* pFS, const char* pDirectoryPath, size_t directoryPathLen, int mode)
{
    e_iterator_internal* pIterator = NULL;  /* This is the iterator we'll eventually be returning. */
    const e_fs_backend* pBackend;
    e_fs_iterator* pBackendIterator;
    e_result result;
    size_t cursor;
    size_t iItem;
    
    if (pDirectoryPath == NULL) {
        pDirectoryPath = "";
    }

    result = e_validate_path(pDirectoryPath, directoryPathLen, mode);
    if (result != E_SUCCESS) {
        return NULL;    /* Invalid path. */
    }

    pBackend = e_get_backend_or_default(pFS);
    if (pBackend == NULL) {
        return NULL;
    }

    if (directoryPathLen == E_NULL_TERMINATED) {
        directoryPathLen = strlen(pDirectoryPath);
    }

    /*
    The first thing we need to do is gather files and directories from the backend. This needs to be done in the
    same order that we attempt to load files for reading:

        1) From all mounts.
        2) Directly from the file system.

    With each of the steps above, the relevant open mode flags must be respected as well because we want iteration
    to be consistent with what would happen when opening files.
    */
    /* Gather files. */
    {
        e_result mountPointIerationResult;
        e_mount_list_iterator iMountPoint;

        /* Check mount points. */
        if (pFS != NULL && (mode & E_IGNORE_MOUNTS) == 0) {
            for (mountPointIerationResult = e_mount_list_first(pFS->pReadMountPoints, &iMountPoint); mountPointIerationResult == E_SUCCESS; mountPointIerationResult = e_mount_list_next(&iMountPoint)) {
                /*
                Just like when opening a file, we need to check that the directory path starts with the mount point. If it
                doesn't match we just skip to the next mount point.
                */
                char  pDirSubPathCleanStack[1024];
                char* pDirSubPathCleanHeap = NULL;
                char* pDirSubPathClean;
                int dirSubPathCleanLen;
                unsigned int cleanOptions = (mode & E_NO_ABOVE_ROOT_NAVIGATION);

                size_t dirSubPathLen;
                const char* pDirSubPath = e_path_trim_base(pDirectoryPath, directoryPathLen, iMountPoint.pMountPointPath, E_NULL_TERMINATED);
                if (pDirSubPath == NULL) {
                    continue;
                }

                dirSubPathLen = directoryPathLen - (size_t)(pDirSubPath - pDirectoryPath); 


                /* If the mount point starts with a root segment, i.e. "/", we cannot allow navigation above that. */
                if (iMountPoint.pMountPointPath[0] == '/' || iMountPoint.pMountPointPath[0] == '\\') {
                    cleanOptions |= E_NO_ABOVE_ROOT_NAVIGATION;
                }

                /* We need to clean the file sub-path, but can skip it if E_NO_SPECIAL_DIRS is specified since it's implied. */
                if ((mode & E_NO_SPECIAL_DIRS) == 0) {
                    dirSubPathCleanLen = e_path_normalize(pDirSubPathCleanStack, sizeof(pDirSubPathCleanStack), pDirSubPath, dirSubPathLen, cleanOptions);
                    if (dirSubPathCleanLen < 0) {
                        continue;    /* Most likely violating E_NO_ABOVE_ROOT_NAVIGATION. */
                    }

                    if (dirSubPathCleanLen >= (int)sizeof(pDirSubPathCleanStack)) {
                        pDirSubPathCleanHeap = (char*)e_malloc(dirSubPathCleanLen + 1, e_fs_get_allocation_callbacks(pFS));
                        if (pDirSubPathCleanHeap == NULL) {
                            return NULL;    /* Out of memory. */
                        }

                        e_path_normalize(pDirSubPathCleanHeap, dirSubPathCleanLen + 1, pDirSubPath, dirSubPathLen, cleanOptions);    /* <-- This should never fail. */
                        pDirSubPathClean = pDirSubPathCleanHeap;
                    } else {
                        pDirSubPathClean = pDirSubPathCleanStack;
                    }
                } else {
                    pDirSubPathClean = (char*)pDirSubPath;  /* Safe cast. Will not be modified past this point. */
                    dirSubPathCleanLen = (int)dirSubPathLen;
                }

                
                if (iMountPoint.pArchive != NULL) {
                    /* The mount point is an archive. We need to iterate over the contents of the archive. */
                    pBackendIterator = e_fs_first_ex(iMountPoint.pArchive, pDirSubPathClean, dirSubPathCleanLen, mode);
                    while (pBackendIterator != NULL) {
                        pIterator = e_iterator_internal_append(pIterator, pBackendIterator, pFS, mode);
                        pBackendIterator = e_fs_next(pBackendIterator);
                    }
                } else {
                    /*
                    The mount point is a directory. We need to construct a path that is the concatenation of the mount point's internal path and
                    our input directory. This is the path we'll be using to iterate over the contents of the directory.
                    */
                    char pInterpolatedPathStack[1024];
                    char* pInterpolatedPathHeap = NULL;
                    char* pInterpolatedPath;
                    int interpolatedPathLen;

                    interpolatedPathLen = e_path_append(pInterpolatedPathStack, sizeof(pInterpolatedPathStack), iMountPoint.pPath, E_NULL_TERMINATED, pDirSubPathClean, dirSubPathCleanLen);
                    if (interpolatedPathLen > 0 && (size_t)interpolatedPathLen >= sizeof(pInterpolatedPathStack)) {
                        /* Not enough room on the stack. Allocate on the heap. */
                        pInterpolatedPathHeap = (char*)e_malloc(interpolatedPathLen + 1, e_fs_get_allocation_callbacks(pFS));
                        if (pInterpolatedPathHeap == NULL) {
                            e_fs_free_iterator((e_fs_iterator*)pIterator);
                            return NULL;    /* Out of memory. */
                        }

                        e_path_append(pInterpolatedPathHeap, interpolatedPathLen + 1, iMountPoint.pPath, E_NULL_TERMINATED, pDirSubPathClean, dirSubPathCleanLen);    /* <-- This should never fail. */
                        pInterpolatedPath = pInterpolatedPathHeap;
                    } else {
                        pInterpolatedPath = pInterpolatedPathStack;
                    }

                    pIterator = e_iterator_internal_gather(pIterator, pBackend, pFS, pInterpolatedPath, E_NULL_TERMINATED, mode);

                    if (pInterpolatedPathHeap != NULL) {
                        e_free(pInterpolatedPathHeap, e_fs_get_allocation_callbacks(pFS));
                        pInterpolatedPathHeap = NULL;
                    }
                }
            }
        }

        /* Check for files directly in the file system. */
        if ((mode & E_ONLY_MOUNTS) == 0) {
            pIterator = e_iterator_internal_gather(pIterator, pBackend, pFS, pDirectoryPath, directoryPathLen, mode);
        }
    }

    /* If after the gathering step we don't have an iterator we can just return null. It just means nothing was found. */
    if (pIterator == NULL) {
        return NULL;
    }

    /* Set up pointers. The list of pointers is located at the end of the array. */
    pIterator->ppItems = (e_iterator_item**)E_OFFSET_PTR(pIterator, pIterator->dataSize - (pIterator->itemCount * sizeof(e_iterator_item*)));

    cursor = 0;
    for (iItem = 0; iItem < pIterator->itemCount; iItem += 1) {
        pIterator->ppItems[iItem] = (e_iterator_item*)E_OFFSET_PTR(pIterator, sizeof(e_iterator_internal) + cursor);
        cursor += e_iterator_item_sizeof(pIterator->ppItems[iItem]->nameLen);
    }

    /* We want to sort items in the iterator to make it consistent across platforms. */
    e_iterator_internal_sort(pIterator);

    /* Post-processing setup. */
    pIterator->base.pFS  = pFS;
    pIterator->itemIndex = 0;
    e_iterator_internal_resolve_public_members(pIterator);

    return (e_fs_iterator*)pIterator;
}

E_API e_fs_iterator* e_fs_first(e_fs* pFS, const char* pDirectoryPath, int mode)
{
    return e_fs_first_ex(pFS, pDirectoryPath, E_NULL_TERMINATED, mode);
}

E_API e_fs_iterator* e_fs_next(e_fs_iterator* pIterator)
{
    e_iterator_internal* pIteratorInternal = (e_iterator_internal*)pIterator;

    if (pIteratorInternal == NULL) {
        return NULL;
    }

    pIteratorInternal->itemIndex += 1;

    if (pIteratorInternal->itemIndex == pIteratorInternal->itemCount) {
        e_fs_free_iterator(pIterator);
        return NULL;
    }

    e_iterator_internal_resolve_public_members(pIteratorInternal);

    return pIterator;
}

E_API void e_fs_free_iterator(e_fs_iterator* pIterator)
{
    if (pIterator == NULL) {
        return;
    }

    e_free(pIterator, e_fs_get_allocation_callbacks(pIterator->pFS));
}


static e_result e_mount_read(e_fs* pFS, const char* pActualPath, const char* pVirtualPath, int options)
{
    e_result result;
    e_mount_list_iterator iterator;
    e_result iteratorResult;
    e_mount_list* pMountPoints;
    e_mount_point* pNewMountPoint;
    e_file_info fileInfo;
    int openMode;

    E_ASSERT(pFS != NULL);
    E_ASSERT(pActualPath != NULL);
    E_ASSERT(pVirtualPath != NULL);
    E_ASSERT((options & E_READ) == E_READ);

    /*
    The first thing we're going to do is check for duplicates. We allow for the same path to be mounted
    to different mount points, and different paths to be mounted to the same mount point, but we don't
    want to have any duplicates where the same path is mounted to the same mount point.
    */
    for (iteratorResult = e_mount_list_first(pFS->pReadMountPoints, &iterator); iteratorResult == E_SUCCESS; iteratorResult = e_mount_list_next(&iterator)) {
        if (strcmp(pActualPath, iterator.pPath) == 0 && strcmp(pVirtualPath, iterator.pMountPointPath) == 0) {
            return E_SUCCESS;  /* Just pretend we're successful. */
        }
    }

    /*
    Getting here means we're not mounting a duplicate so we can now add it. We'll be either adding it to
    the end of the list, or to the beginning of the list depending on the priority.
    */
    pMountPoints = e_mount_list_alloc(pFS->pReadMountPoints, pActualPath, pVirtualPath, ((options & E_LOWEST_PRIORITY) == E_LOWEST_PRIORITY) ? E_MOUNT_PRIORITY_LOWEST : E_MOUNT_PRIORITY_HIGHEST, e_fs_get_allocation_callbacks(pFS), &pNewMountPoint);
    if (pMountPoints == NULL) {
        return E_OUT_OF_MEMORY;
    }

    pFS->pReadMountPoints = pMountPoints;

    /*
    We need to determine if we're mounting a directory or an archive. If it's an archive, we need to
    open it.
    */
    openMode = E_READ | E_VERBOSE;

    /* Must use e_fs_backend_info() instead of e_fs_info() because otherwise e_fs_info() will attempt to read from mounts when we're in the process of trying to add one (this function). */
    result = e_fs_backend_info(e_get_backend_or_default(pFS), pFS, (pActualPath[0] != '\0') ? pActualPath : ".", E_IGNORE_MOUNTS, &fileInfo);
    if (result != E_SUCCESS) {
        return result;
    }

    if (fileInfo.directory) {
        pNewMountPoint->pArchive = NULL;
        pNewMountPoint->closeArchiveOnUnmount = E_FALSE;
    } else {
        result = e_open_archive(pFS, pActualPath, openMode, &pNewMountPoint->pArchive);
        if (result != E_SUCCESS) {
            return result;
        }

        pNewMountPoint->closeArchiveOnUnmount = E_TRUE;
    }

    return E_SUCCESS;
}

E_API e_result e_unmount_read(e_fs* pFS, const char* pActualPath, int options)
{
    e_result iteratorResult;
    e_mount_list_iterator iterator;

    if (pFS == NULL || pActualPath == NULL) {
        return E_INVALID_ARGS;
    }

    E_UNUSED(options);

    for (iteratorResult = e_mount_list_first(pFS->pReadMountPoints, &iterator); iteratorResult == E_SUCCESS && !e_mount_list_at_end(&iterator); /*iteratorResult = e_mount_list_next(&iterator)*/) {
        if (strcmp(pActualPath, iterator.pPath) == 0) {
            if (iterator.internal.pMountPoint->closeArchiveOnUnmount) {
                e_close_archive(iterator.pArchive);
            }

            e_mount_list_remove(pFS->pReadMountPoints, iterator.internal.pMountPoint);

            /*
            Since we just removed this item we don't want to advance the cursor. We do, however, need to re-resolve
            the members in preparation for the next iteration.
            */
            e_mount_list_iterator_resolve_members(&iterator, iterator.internal.cursor);
        } else {
            iteratorResult = e_mount_list_next(&iterator);
        }
    }

    return E_SUCCESS;
}

static e_result e_fs_mount_write(e_fs* pFS, const char* pActualPath, const char* pVirtualPath, int options)
{
    e_mount_list_iterator iterator;
    e_result iteratorResult;
    e_mount_point* pNewMountPoint;
    e_mount_list* pMountList;

    if (pFS == NULL || pActualPath == NULL) {
        return E_INVALID_ARGS;
    }

    if (pVirtualPath == NULL) {
        pVirtualPath = "";
    }

    /* Like with regular read mount points we'll want to check for duplicates. */
    for (iteratorResult = e_mount_list_first(pFS->pWriteMountPoints, &iterator); iteratorResult == E_SUCCESS; iteratorResult = e_mount_list_next(&iterator)) {
        if (strcmp(pActualPath, iterator.pPath) == 0 && strcmp(pVirtualPath, iterator.pMountPointPath) == 0) {
            return E_SUCCESS;  /* Just pretend we're successful. */
        }
    }

    /* Getting here means we're not mounting a duplicate so we can now add it. */
    pMountList = e_mount_list_alloc(pFS->pWriteMountPoints, pActualPath, pVirtualPath, ((options & E_LOWEST_PRIORITY) == E_LOWEST_PRIORITY) ? E_MOUNT_PRIORITY_LOWEST : E_MOUNT_PRIORITY_HIGHEST, e_fs_get_allocation_callbacks(pFS), &pNewMountPoint);
    if (pMountList == NULL) {
        return E_OUT_OF_MEMORY;
    }

    pFS->pWriteMountPoints = pMountList;
    
    /* We don't support mounting archives. Explicitly disable this. */
    pNewMountPoint->pArchive = NULL;
    pNewMountPoint->closeArchiveOnUnmount = E_FALSE;

    /* Since we'll be wanting to write out files to the mount point we should ensure the folder actually exists. */
    if ((options & E_NO_CREATE_DIRS) == 0) {
        e_fs_mkdir(pFS, pActualPath, E_IGNORE_MOUNTS);
    }

    return E_SUCCESS;
}

static e_result e_fs_unmount_write(e_fs* pFS, const char* pActualPath, int options)
{
    e_result iteratorResult;
    e_mount_list_iterator iterator;

    E_ASSERT(pFS != NULL);
    E_ASSERT(pActualPath != NULL);

    E_UNUSED(options);

    for (iteratorResult = e_mount_list_first(pFS->pWriteMountPoints, &iterator); iteratorResult == E_SUCCESS; /*iteratorResult = e_mount_list_next(&iterator)*/) {
        if (strcmp(pActualPath, iterator.pPath) == 0) {
            e_mount_list_remove(pFS->pWriteMountPoints, iterator.internal.pMountPoint);

            /*
            Since we just removed this item we don't want to advance the cursor. We do, however, need to re-resolve
            the members in preparation for the next iteration.
            */
            e_mount_list_iterator_resolve_members(&iterator, iterator.internal.cursor);
        } else {
            iteratorResult = e_mount_list_next(&iterator);
        }
    }

    return E_SUCCESS;
}


E_API e_result e_fs_mount(e_fs* pFS, const char* pActualPath, const char* pVirtualPath, int options)
{
    if (pFS == NULL || pActualPath == NULL) {
        return E_INVALID_ARGS;
    }

    if (pVirtualPath == NULL) {
        pVirtualPath = "";
    }

    /* At least READ or WRITE must be specified. */
    if ((options & (E_READ | E_WRITE)) == 0) {
        return E_INVALID_ARGS;
    }

    if ((options & E_READ) == E_READ) {
        e_result result = e_mount_read(pFS, pActualPath, pVirtualPath, options);
        if (result != E_SUCCESS) {
            return result;
        }
    }

    if ((options & E_WRITE) == E_WRITE) {
        e_result result = e_fs_mount_write(pFS, pActualPath, pVirtualPath, options);
        if (result != E_SUCCESS) {
            return result;
        }
    }

    return E_SUCCESS;
}

E_API e_result e_fs_unmount(e_fs* pFS, const char* pPathToMount_NotMountPoint, int options)
{
    e_result result;

    if (pFS == NULL || pPathToMount_NotMountPoint == NULL) {
        return E_INVALID_ARGS;
    }

    if ((options & E_READ) == E_READ) {
        result = e_unmount_read(pFS, pPathToMount_NotMountPoint, options);
        if (result != E_SUCCESS) {
            return result;
        }
    }

    if ((options & E_WRITE) == E_WRITE) {
        result = e_fs_unmount_write(pFS, pPathToMount_NotMountPoint, options);
        if (result != E_SUCCESS) {
            return result;
        }
    }

    return E_SUCCESS;
}

static size_t e_sysdir_append(e_sysdir_type type, char* pDst, size_t dstCap, const char* pSubDir)
{
    size_t sysDirLen;
    size_t subDirLen;
    size_t totalLen;

    if (pDst == NULL || pSubDir == NULL) {
        return 0;
    }

    sysDirLen = e_sysdir(type, pDst, dstCap);
    if (sysDirLen == 0) {
        return 0;   /* Failed to retrieve the system directory. */
    }

    subDirLen = strlen(pSubDir);

    totalLen = sysDirLen + 1 + subDirLen;   /* +1 for the separator. */
    if (totalLen < dstCap) {
        pDst[sysDirLen] = '/';
        E_COPY_MEMORY(pDst + sysDirLen + 1, pSubDir, subDirLen);
        pDst[totalLen] = '\0';
    }

    return totalLen;
}

E_API e_result e_mount_sysdir(e_fs* pFS, e_sysdir_type type, const char* pSubDir, const char* pVirtualPath, int options)
{
    char  pPathToMountStack[1024];
    char* pPathToMountHeap = NULL;
    char* pPathToMount;
    size_t pathToMountLen;
    e_result result;

    if (pFS == NULL) {
        return E_INVALID_ARGS;
    }

    if (pVirtualPath == NULL) {
        pVirtualPath = "";
    }

    /*
    We're enforcing a sub-directory with this function to encourage applications to use good
    practice with with directory structures.
    */
    if (pSubDir == NULL || pSubDir[0] == '\0') {
        return E_INVALID_ARGS;
    }

    pathToMountLen = e_sysdir_append(type, pPathToMountStack, sizeof(pPathToMountStack), pSubDir);
    if (pathToMountLen == 0) {
        return E_ERROR;    /* Failed to retrieve the system directory. */
    }

    if (pathToMountLen < sizeof(pPathToMountStack)) {
        pPathToMount = pPathToMountStack;
    } else {
        pathToMountLen += 1;    /* +1 for the null terminator. */

        pPathToMountHeap = (char*)e_malloc(pathToMountLen, e_fs_get_allocation_callbacks(pFS));
        if (pPathToMountHeap == NULL) {
            return E_OUT_OF_MEMORY;
        }

        e_sysdir_append(type, pPathToMountHeap, pathToMountLen, pSubDir);
        pPathToMount = pPathToMountHeap;
    }

    /* At this point we should have the path we want to mount. Now we can do the actual mounting. */
    result = e_fs_mount(pFS, pPathToMount, pVirtualPath, options);
    e_free(pPathToMountHeap, e_fs_get_allocation_callbacks(pFS));

    return result;
}

E_API e_result e_unmount_sysdir(e_fs* pFS, e_sysdir_type type, const char* pSubDir, int options)
{
    char  pPathToMountStack[1024];
    char* pPathToMountHeap = NULL;
    char* pPathToMount;
    size_t pathToMountLen;
    e_result result;

    if (pFS == NULL) {
        return E_INVALID_ARGS;
    }

    /*
    We're enforcing a sub-directory with this function to encourage applications to use good
    practice with with directory structures.
    */
    if (pSubDir == NULL || pSubDir[0] == '\0') {
        return E_INVALID_ARGS;
    }

    pathToMountLen = e_sysdir_append(type, pPathToMountStack, sizeof(pPathToMountStack), pSubDir);
    if (pathToMountLen == 0) {
        return E_ERROR;    /* Failed to retrieve the system directory. */
    }

    if (pathToMountLen < sizeof(pPathToMountStack)) {
        pPathToMount = pPathToMountStack;
    } else {
        pathToMountLen += 1;    /* +1 for the null terminator. */

        pPathToMountHeap = (char*)e_malloc(pathToMountLen, e_fs_get_allocation_callbacks(pFS));
        if (pPathToMountHeap == NULL) {
            return E_OUT_OF_MEMORY;
        }

        e_sysdir_append(type, pPathToMountHeap, pathToMountLen, pSubDir);
        pPathToMount = pPathToMountHeap;
    }

    /* At this point we should have the path we want to mount. Now we can do the actual mounting. */
    result = e_fs_unmount(pFS, pPathToMount, options);

    e_free(pPathToMountHeap, e_fs_get_allocation_callbacks(pFS));
    return result;
}

E_API e_result e_fs_mount_fs(e_fs* pFS, e_fs* pOtherFS, const char* pVirtualPath, int options)
{
    e_result iteratorResult;
    e_mount_list_iterator iterator;
    e_mount_list* pMountPoints;
    e_mount_point* pNewMountPoint;

    if (pFS == NULL || pOtherFS == NULL) {
        return E_INVALID_ARGS;
    }

    if (pVirtualPath == NULL) {
        pVirtualPath = "";
    }

    /* We don't support write mode when mounting an FS. */
    if ((options & E_WRITE) == E_WRITE) {
        return E_INVALID_ARGS;
    }

    /*
    We don't allow duplicates. An archive can be bound to multiple mount points, but we don't want to have the same
    archive mounted to the same mount point multiple times.
    */
    for (iteratorResult = e_mount_list_first(pFS->pReadMountPoints, &iterator); iteratorResult == E_SUCCESS; iteratorResult = e_mount_list_next(&iterator)) {
        if (pOtherFS == iterator.pArchive && strcmp(pVirtualPath, iterator.pMountPointPath) == 0) {
            /* File system is already mounted to the virtual path. Just pretend we're successful. */
            e_ref(pOtherFS);
            return E_SUCCESS;
        }
    }

    /*
    Getting here means we're not mounting a duplicate so we can now add it. We'll be either adding it to
    the end of the list, or to the beginning of the list depending on the priority.
    */
    pMountPoints = e_mount_list_alloc(pFS->pReadMountPoints, "", pVirtualPath, ((options & E_LOWEST_PRIORITY) == E_LOWEST_PRIORITY) ? E_MOUNT_PRIORITY_LOWEST : E_MOUNT_PRIORITY_HIGHEST, e_fs_get_allocation_callbacks(pFS), &pNewMountPoint);
    if (pMountPoints == NULL) {
        return E_OUT_OF_MEMORY;
    }

    pFS->pReadMountPoints = pMountPoints;

    pNewMountPoint->pArchive = e_ref(pOtherFS);
    pNewMountPoint->closeArchiveOnUnmount = E_FALSE;

    return E_SUCCESS;
}

E_API e_result e_fs_unmount_fs(e_fs* pFS, e_fs* pOtherFS, int options)
{
    e_result iteratorResult;
    e_mount_list_iterator iterator;

    if (pFS == NULL || pOtherFS == NULL) {
        return E_INVALID_ARGS;
    }

    E_UNUSED(options);

    for (iteratorResult = e_mount_list_first(pFS->pReadMountPoints, &iterator); iteratorResult == E_SUCCESS; iteratorResult = e_mount_list_next(&iterator)) {
        if (iterator.pArchive == pOtherFS) {
            e_mount_list_remove(pFS->pReadMountPoints, iterator.internal.pMountPoint);
            e_unref(pOtherFS);
            return E_SUCCESS;
        }
    }

    return E_SUCCESS;
}


E_API e_result e_file_read_to_end(e_file* pFile, e_stream_data_format format, void** ppData, size_t* pDataSize)
{
    return e_stream_read_to_end(e_file_get_stream(pFile), format, e_fs_get_allocation_callbacks(e_file_get_fs(pFile)), ppData, pDataSize);
}

E_API e_result e_file_open_and_read(e_fs* pFS, const char* pFilePath, e_stream_data_format format, void** ppData, size_t* pDataSize)
{
    e_result result;
    e_file* pFile;

    if (pFilePath == NULL || ppData == NULL || pDataSize == NULL) {
        return E_INVALID_ARGS;
    }

    result = e_file_open(pFS, pFilePath, E_READ, &pFile);
    if (result != E_SUCCESS) {
        return result;
    }

    result = e_file_read_to_end(pFile, format, ppData, pDataSize);

    e_file_close(pFile);

    return result;
}

E_API e_result e_file_open_and_write(e_fs* pFS, const char* pFilePath, void* pData, size_t dataSize)
{
    e_result result;
    e_file* pFile;

    if (pFilePath == NULL) {
        return E_INVALID_ARGS;
    }

    /* The data pointer can be null, but only if the data size is 0. In this case the file is just made empty which is a valid use case. */
    if (pData == NULL && dataSize > 0) {
        return E_INVALID_ARGS;
    }

    result = e_file_open(pFS, pFilePath, E_TRUNCATE, &pFile);
    if (result != E_SUCCESS) {
        return result;
    }

    if (dataSize > 0) {
        result = e_file_write(pFile, pData, dataSize, NULL);
    }

    e_file_close(pFile);

    return result;
}



/******************************************************************************
*
*
* stdio Backend
*
*
******************************************************************************/
#ifndef E_NO_STDIO
#include <stdio.h>
#include <wchar.h>      /* For wcstombs(). */
#include <sys/stat.h>

#if defined(_WIN32)
#include <direct.h>     /* For _mkdir() */
#endif

#ifndef S_ISDIR
#define S_ISDIR(m) (((m) & _S_IFMT) == _S_IFDIR)
#endif

#ifndef S_ISLNK
    #if defined(_WIN32)
        #define S_ISLNK(m) (0)
    #else
        #define S_ISLNK(m) (((m) & _S_IFMT) == _S_IFLNK)
    #endif
#endif

static int e_fopen(FILE** ppFile, const char* pFilePath, const char* pOpenMode)
{
#if defined(_MSC_VER) && _MSC_VER >= 1400
    int err;
#endif

    if (ppFile != NULL) {
        *ppFile = NULL;  /* Safety. */
    }

    if (pFilePath == NULL || pOpenMode == NULL || ppFile == NULL) {
        return EINVAL;
    }

#if defined(_MSC_VER) && _MSC_VER >= 1400
    err = fopen_s(ppFile, pFilePath, pOpenMode);
    if (err != 0) {
        return err;
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
        int result = errno;
        if (result == 0) {
            result = ENOENT;   /* Just a safety check to make sure we never ever return success when pFile == NULL. */
        }

        return result;
    }
#endif

    return E_SUCCESS;
}

/*
_wfopen() isn't always available in all compilation environments.

    * Windows only.
    * MSVC seems to support it universally as far back as VC6 from what I can tell (haven't checked further back).
    * MinGW-64 (both 32- and 64-bit) seems to support it.
    * MinGW wraps it in !defined(__STRICT_ANSI__).
    * OpenWatcom wraps it in !defined(_NO_EXT_KEYS).

This can be reviewed as compatibility issues arise. The preference is to use _wfopen_s() and _wfopen() as opposed to the wcsrtombs()
fallback, so if you notice your compiler not detecting this properly I'm happy to look at adding support.
*/
#if defined(_WIN32)
    #if defined(_MSC_VER) || defined(__MINGW64__) || (!defined(__STRICT_ANSI__) && !defined(_NO_EXT_KEYS))
        #define E_HAS_WFOPEN
    #endif
#endif

int e_wfopen(FILE** ppFile, const wchar_t* pFilePath, const wchar_t* pOpenMode)
{
    if (ppFile != NULL) {
        *ppFile = NULL;  /* Safety. */
    }

    if (pFilePath == NULL || pOpenMode == NULL || ppFile == NULL) {
        return E_INVALID_ARGS;
    }

    #if defined(E_HAS_WFOPEN)
    {
        /* Use _wfopen() on Windows. */
        #if defined(_MSC_VER) && _MSC_VER >= 1400
        {
            errno_t err = _wfopen_s(ppFile, pFilePath, pOpenMode);
            if (err != 0) {
                return err;
            }
        }
        #else
        {
            *ppFile = _wfopen(pFilePath, pOpenMode);
            if (*ppFile == NULL) {
                return errno;
            }
        }
        #endif
    }
    #else
    {
        /*
        Use fopen() on anything other than Windows. Requires a conversion. This is annoying because fopen() is locale specific. The only real way I can
        think of to do this is with wcsrtombs(). Note that wcstombs() is apparently not thread-safe because it uses a static global mbstate_t object for
        maintaining state. I've checked this with -std=c89 and it works, but if somebody get's a compiler error I'll look into improving compatibility.
        */
        mbstate_t mbs;
        size_t lenMB;
        const wchar_t* pFilePathTemp = pFilePath;
        char* pFilePathMB = NULL;
        char pOpenModeMB[32] = {0};

        /* Get the length first. */
        E_ZERO_OBJECT(&mbs);
        lenMB = wcsrtombs(NULL, &pFilePathTemp, 0, &mbs);
        if (lenMB == (size_t)-1) {
            return errno;
        }

        pFilePathMB = (char*)e_malloc(lenMB + 1, NULL);
        if (pFilePathMB == NULL) {
            return ENOMEM;
        }

        pFilePathTemp = pFilePath;
        E_ZERO_OBJECT(&mbs);
        wcsrtombs(pFilePathMB, &pFilePathTemp, lenMB + 1, &mbs);

        /* The open mode should always consist of ASCII characters so we should be able to do a trivial conversion. */
        {
            size_t i = 0;
            for (;;) {
                if (pOpenMode[i] == 0) {
                    pOpenModeMB[i] = '\0';
                    break;
                }

                pOpenModeMB[i] = (char)pOpenMode[i];
                i += 1;
            }
        }

        *ppFile = fopen(pFilePathMB, pOpenModeMB);

        e_free(pFilePathMB, NULL);

        if (*ppFile == NULL) {
            return errno;
        }
    }
    #endif

    return 0;
}


static e_file_info e_file_info_from_stat(struct stat* pStat)
{
    e_file_info info;

    E_ZERO_OBJECT(&info);
    info.size             = pStat->st_size;
    info.lastAccessTime   = pStat->st_atime;
    info.lastModifiedTime = pStat->st_mtime;
    info.directory        = S_ISDIR(pStat->st_mode) != 0;
    info.symlink          = S_ISLNK(pStat->st_mode) != 0;

    return info;
}

#if defined(_WIN32)
static e_uint64 e_FILETIME_to_unix(const FILETIME* pFT)
{
    ULARGE_INTEGER li;

    li.HighPart = pFT->dwHighDateTime;
    li.LowPart  = pFT->dwLowDateTime;

    return (e_uint64)(li.QuadPart / 10000000UL - 11644473600UL);   /* Convert from Windows epoch to Unix epoch. */
}

static e_file_info e_file_info_from_WIN32_FIND_DATAW(const WIN32_FIND_DATAW* pFD)
{
    e_file_info info;

    E_ZERO_OBJECT(&info);
    info.size             = ((e_uint64)pFD->nFileSizeHigh << 32) | (e_uint64)pFD->nFileSizeLow;
    info.lastModifiedTime = e_FILETIME_to_unix(&pFD->ftLastWriteTime);
    info.lastAccessTime   = e_FILETIME_to_unix(&pFD->ftLastAccessTime);
    info.directory        = (pFD->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)     != 0;
    info.symlink          = (pFD->dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0;

    return info;
}
#endif


typedef struct e_stdio_registered_file
{
    size_t pathLen;
    FILE* pFile;
} e_stdio_registered_file;

typedef struct e_stdio
{
    int _unused;
} e_stdio;

static size_t e_alloc_size_stdio(const void* pBackendConfig)
{
    E_UNUSED(pBackendConfig);

    return sizeof(e_stdio);
}

static e_result e_init_stdio(e_fs* pFS, const void* pBackendConfig, e_stream* pStream)
{
    E_UNUSED(pFS);
    E_UNUSED(pBackendConfig);
    E_UNUSED(pStream);

    return E_SUCCESS;
}

static void e_uninit_stdio(e_fs* pFS)
{
    E_UNUSED(pFS);
    return;
}

static e_result e_ioctl_stdio(e_fs* pFS, int op, void* pArgs)
{
    E_UNUSED(pFS);
    E_UNUSED(op);
    E_UNUSED(pArgs);

    /* Not used by the stdio backend. */
    return E_INVALID_OPERATION;
}

static e_result e_remove_stdio(e_fs* pFS, const char* pFilePath)
{
    int result = remove(pFilePath);
    if (result != 0) {
        return e_result_from_errno(errno);
    }

    E_UNUSED(pFS);

    return E_SUCCESS;
}

static e_result e_rename_stdio(e_fs* pFS, const char* pOldName, const char* pNewName)
{
    int result = rename(pOldName, pNewName);
    if (result != 0) {
        return e_result_from_errno(errno);
    }

    E_UNUSED(pFS);

    return E_SUCCESS;
}


#if defined(_WIN32)
static e_result e_mkdir_stdio_win32(const char* pPath)
{
    int result;

    /* If it's a drive letter segment just pretend it's successful. */
    if (pPath[0] >= 'a' && pPath[0] <= 'z' || pPath[0] >= 'A' && pPath[0] <= 'Z') {
        if (pPath[1] == ':' && pPath[2] == '\0') {
            return E_SUCCESS;
        }
    }
    
    result = _mkdir(pPath);
    if (result != 0) {
        return e_result_from_errno(errno);
    }

    return E_SUCCESS;
}
#else
static e_result e_mkdir_stdio_posix(const char* pPath)
{
    int result = mkdir(pPath, S_IRWXU);
    if (result != 0) {
        return e_result_from_errno(errno);
    }

    return E_SUCCESS;
}
#endif

static e_result e_mkdir_stdio(e_fs* pFS, const char* pPath)
{
    e_result result;

    E_UNUSED(pFS);

#if defined(_WIN32)
    result = e_mkdir_stdio_win32(pPath);
#else
    result = e_mkdir_stdio_posix(pPath);
#endif

    if (result == E_DOES_NOT_EXIST) {
        result =  E_SUCCESS;
    }

    return result;
}

static e_result e_info_stdio(e_fs* pFS, const char* pPath, int openMode, e_file_info* pInfo)
{
    /* We don't want to use stat() with Win32 because, from what I can tell, there's no way to determine if it's a symbolic link. S_IFLNK does not seem to be defined. */
    #if defined(_WIN32)
    {
        int pathLen;
        wchar_t  pPathWStack[1024];
        wchar_t* pPathWHeap = NULL;
        wchar_t* pPathW;
        HANDLE hFind;
        WIN32_FIND_DATAW fd;

        /* Use Win32 to convert from UTF-8 to wchar_t. */
        pathLen = MultiByteToWideChar(CP_UTF8, 0, pPath, -1, NULL, 0);
        if (pathLen == 0) {
            return e_result_from_errno(GetLastError());
        }

        if (pathLen <= (int)E_COUNTOF(pPathWStack)) {
            pPathW = pPathWStack;
        } else {
            pPathWHeap = (wchar_t*)e_malloc(pathLen * sizeof(wchar_t), e_fs_get_allocation_callbacks(pFS));  /* pathLen includes the null terminator. */
            if (pPathWHeap == NULL) {
                return E_OUT_OF_MEMORY;
            }

            pPathW = pPathWHeap;
        }

        MultiByteToWideChar(CP_UTF8, 0, pPath, -1, pPathW, pathLen);

        hFind = FindFirstFileW(pPathW, &fd);

        e_free(pPathWHeap, e_fs_get_allocation_callbacks(pFS));
        pPathWHeap = NULL;

        *pInfo = e_file_info_from_WIN32_FIND_DATAW(&fd);
    }
    #else
    {
        struct stat info;

        E_UNUSED(pFS);

        if (stat(pPath, &info) != 0) {
            return e_result_from_errno(errno);
        }

        *pInfo = e_file_info_from_stat(&info);
    }
    #endif

    (void)openMode;

    return E_SUCCESS;
}


typedef struct e_file_stdio
{
    FILE* pFile;
    char openMode[4];   /* For duplication. */
    e_bool32 isRegisteredOrHandle; /* When set to true, will not be closed with e_file_close(). */
} e_file_stdio;

static size_t e_file_alloc_size_stdio(e_fs* pFS)
{
    E_UNUSED(pFS);
    return sizeof(e_file_stdio);
}

static e_result e_file_open_stdio(e_fs* pFS, e_stream* pStream, const char* pPath, int openMode, e_file* pFile)
{
    e_file_stdio* pFileStdio;
    int result;

    E_UNUSED(pFS);
    E_UNUSED(pStream);

    pFileStdio = (e_file_stdio*)e_file_get_backend_data(pFile);
    if (pFileStdio == NULL) {
        return E_INVALID_ARGS;
    }
    
    if ((openMode & E_WRITE) != 0) {
        if ((openMode & E_READ) != 0) {
            /* Read and write. */
            if ((openMode & E_APPEND) == E_APPEND) {
                pFileStdio->openMode[0] = 'a'; pFileStdio->openMode[1] = '+'; pFileStdio->openMode[2] = 'b'; pFileStdio->openMode[3] = 0;   /* Read-and-write, appending. */
            } else if ((openMode & E_OVERWRITE) == E_OVERWRITE) {
                pFileStdio->openMode[0] = 'r'; pFileStdio->openMode[1] = '+'; pFileStdio->openMode[2] = 'b'; pFileStdio->openMode[3] = 0;   /* Read-and-write, overwriting. */
            } else {
                pFileStdio->openMode[0] = 'w'; pFileStdio->openMode[1] = '+'; pFileStdio->openMode[2] = 'b'; pFileStdio->openMode[3] = 0;   /* Read-and-write, truncating. */
            }
        } else {
            /* Write-only. */
            if ((openMode & E_APPEND) == E_APPEND) {
                pFileStdio->openMode[0] = 'a'; pFileStdio->openMode[1] = 'b'; pFileStdio->openMode[2] = 0; /* Write-only, appending. */
            } else if ((openMode & E_OVERWRITE) == E_OVERWRITE) {
                pFileStdio->openMode[0] = 'r'; pFileStdio->openMode[1] = '+'; pFileStdio->openMode[2] = 'b'; pFileStdio->openMode[3] = 0;   /* Write-only, overwriting. Need to use the "+" option here because there does not appear to be an option for a write-only overwrite mode. */
            } else {
                pFileStdio->openMode[0] = 'w'; pFileStdio->openMode[1] = 'b'; pFileStdio->openMode[2] = 0; /* Write-only, truncating. */
            }
        }
    } else {
        if ((openMode & E_READ) != 0) {
            pFileStdio->openMode[0] = 'r'; pFileStdio->openMode[1] = 'b'; pFileStdio->openMode[2] = 0;    /* Read-only. */
        } else {
            return E_INVALID_ARGS;
        }
    }

    #if defined(_WIN32) && defined(E_HAS_WFOPEN)
    {
        size_t i;
        int pathLen;
        wchar_t  pOpenModeW[4];
        wchar_t  pFilePathWStack[1024];
        wchar_t* pFilePathWHeap = NULL;
        wchar_t* pFilePathW;

        /* Use Win32 to convert from UTF-8 to wchar_t. */
        pathLen = MultiByteToWideChar(CP_UTF8, 0, pPath, -1, NULL, 0);
        if (pathLen > 0) {
            if (pathLen <= (int)E_COUNTOF(pFilePathWStack)) {
                pFilePathW = pFilePathWStack;
            } else {
                pFilePathWHeap = (wchar_t*)e_malloc(pathLen * sizeof(wchar_t), e_fs_get_allocation_callbacks(pFS));
                if (pFilePathWHeap == NULL) {
                    return E_OUT_OF_MEMORY;
                }

                pFilePathW = pFilePathWHeap;
            }

            MultiByteToWideChar(CP_UTF8, 0, pPath, -1, pFilePathW, pathLen);
            
            for (i = 0; i < E_COUNTOF(pOpenModeW); i += 1) {
                pOpenModeW[i] = (wchar_t)pFileStdio->openMode[i];
            }

            result = e_wfopen(&pFileStdio->pFile, pFilePathW, pOpenModeW);

            e_free(pFilePathWHeap, e_fs_get_allocation_callbacks(pFS));
            pFilePathWHeap = NULL;

            if (result == 0) {
                return E_SUCCESS;
            }
        }
    }
    #endif

    /* Getting here means we're either not opening with wfopen(), or wfopen() failed (or the conversion from char to wchar_t). */
    result = e_fopen(&pFileStdio->pFile, pPath, pFileStdio->openMode);
    if (result != 0) {
        return e_result_from_errno(result);
    }

    return E_SUCCESS;
}

static e_result e_file_open_handle_stdio(e_fs* pFS, void* hBackendFile, e_file* pFile)
{
    e_file_stdio* pFileStdio;

    E_UNUSED(pFS);

    pFileStdio = (e_file_stdio*)e_file_get_backend_data(pFile);
    if (pFileStdio == NULL) {
        return E_INVALID_ARGS;
    }
    
    pFileStdio->pFile = (FILE*)hBackendFile;
    pFileStdio->isRegisteredOrHandle = E_TRUE;

    return E_SUCCESS;
}

static void e_file_close_stdio(e_file* pFile)
{
    e_file_stdio* pFileStdio = (e_file_stdio*)e_file_get_backend_data(pFile);
    if (pFileStdio == NULL) {
        return;
    }

    if (!pFileStdio->isRegisteredOrHandle) {
        fclose(pFileStdio->pFile);
    }
}

static e_result e_file_read_stdio(e_file* pFile, void* pDst, size_t bytesToRead, size_t* pBytesRead)
{
    size_t bytesRead;
    e_file_stdio* pFileStdio;

    /* These were all validated at a higher level. */
    E_ASSERT(pFile      != NULL);
    E_ASSERT(pDst       != NULL);
    E_ASSERT(pBytesRead != NULL);

    pFileStdio = (e_file_stdio*)e_file_get_backend_data(pFile);
    E_ASSERT(pFileStdio != NULL);

    bytesRead = fread(pDst, 1, bytesToRead, pFileStdio->pFile);

    *pBytesRead = bytesRead;
    
    /* If the value returned by fread is less than the bytes requested, it was either EOF or an error. We don't return EOF unless the number of bytes read is 0. */
    if (bytesRead != bytesToRead) {
        if (feof(pFileStdio->pFile)) {
            if (bytesRead == 0) {
                return E_AT_END;
            }
        } else {
            return e_result_from_errno(ferror(pFileStdio->pFile));
        }
    }

    return E_SUCCESS;
}

static e_result e_file_write_stdio(e_file* pFile, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten)
{
    size_t bytesWritten;
    e_file_stdio* pFileStdio;

    /* These were all validated at a higher level. */
    E_ASSERT(pFile         != NULL);
    E_ASSERT(pSrc          != NULL);
    E_ASSERT(pBytesWritten != NULL);

    pFileStdio = (e_file_stdio*)e_file_get_backend_data(pFile);
    E_ASSERT(pFileStdio != NULL);

    bytesWritten = fwrite(pSrc, 1, bytesToWrite, pFileStdio->pFile);

    *pBytesWritten = bytesWritten;

    if (bytesWritten != bytesToWrite) {
        return e_result_from_errno(ferror(pFileStdio->pFile));
    }

    return E_SUCCESS;
}

static e_result e_file_seek_stdio(e_file* pFile, e_int64 offset, e_seek_origin origin)
{
    e_file_stdio* pFileStdio;
    int result;
    int whence;

    /* These were all validated at a higher level. */
    E_ASSERT(pFile != NULL);
    
    pFileStdio = (e_file_stdio*)e_file_get_backend_data(pFile);
    E_ASSERT(pFileStdio != NULL);

    if (origin == E_SEEK_SET) {
        whence = SEEK_SET;
    } else if (origin == E_SEEK_END) {
        whence = SEEK_END;
    } else {
        whence = SEEK_CUR;
    }

#if defined(_WIN32)
    #if defined(_MSC_VER) && _MSC_VER > 1200
        result = _fseeki64(pFileStdio->pFile, offset, whence);
    #else
        /* No _fseeki64() so restrict to 31 bits. */
        if (origin > 0x7FFFFFFF) {
            return E_OUT_OF_RANGE;
        }

        result = fseek(pFileStdio->pFile, (int)offset, whence);
    #endif
#else
    result = fseek(pFileStdio->pFile, (long int)offset, whence);
#endif
    if (result != 0) {
        return e_result_from_errno(errno);
    }

    return E_SUCCESS;
}

static e_result e_file_tell_stdio(e_file* pFile, e_int64* pCursor)
{
    e_file_stdio* pFileStdio;
    e_int64 result;

    /* These were all validated at a higher level. */
    E_ASSERT(pFile   != NULL);
    E_ASSERT(pCursor != NULL);

    pFileStdio = (e_file_stdio*)e_file_get_backend_data(pFile);
    E_ASSERT(pFileStdio != NULL);

#if defined(_WIN32)
    #if defined(_MSC_VER) && _MSC_VER > 1200
        result = _ftelli64(pFileStdio->pFile);
    #else
        result = ftell(pFileStdio->pFile);
    #endif
#else
    result = ftell(pFileStdio->pFile);
#endif

    *pCursor = result;

    return E_SUCCESS;
}

static e_result e_file_flush_stdio(e_file* pFile)
{
    e_file_stdio* pFileStdio;
    int result;

    /* These were all validated at a higher level. */
    E_ASSERT(pFile != NULL);

    pFileStdio = (e_file_stdio*)e_file_get_backend_data(pFile);
    E_ASSERT(pFileStdio != NULL);

    result = fflush(pFileStdio->pFile);
    if (result != 0) {
        return e_result_from_errno(ferror(pFileStdio->pFile));
    }

    return E_SUCCESS;
}


/* Please submit a bug report if you get an error about fileno(). */
#if !defined(_MSC_VER) && !((defined(_POSIX_C_SOURCE) && _POSIX_C_SOURCE >= 1) || defined(_XOPEN_SOURCE) || defined(_POSIX_SOURCE)) && !(defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__))
int fileno(FILE *stream);
#endif

static e_result e_file_info_stdio(e_file* pFile, e_file_info* pInfo)
{
    e_file_stdio* pFileStdio;
    int fd;
    struct stat info;

    /* These were all validated at a higher level. */
    E_ASSERT(pFile != NULL);
    E_ASSERT(pInfo != NULL);

    pFileStdio = (e_file_stdio*)e_file_get_backend_data(pFile);
    E_ASSERT(pFileStdio != NULL);

#if defined(_MSC_VER)
    fd = _fileno(pFileStdio->pFile);
#else
    fd =  fileno(pFileStdio->pFile);
#endif

    if (fstat(fd, &info) != 0) {
        return e_result_from_errno(ferror(pFileStdio->pFile));
    }

    *pInfo = e_file_info_from_stat(&info);

    return E_SUCCESS;
}

/* Iteration is platform-specific. */
#define E_STDIO_MIN_ITERATOR_ALLOCATION_SIZE 1024

#if defined(_WIN32)
#include <fcntl.h>
#include <io.h>

E_API e_result e_file_duplicate_stdio(e_file* pFile, e_file* pDuplicatedFile)
{
    e_file_stdio* pFileStdio;
    e_file_stdio* pDuplicatedFileStdio;
    int fd;
    int fdDuplicate;
    HANDLE hFile;
    HANDLE hFileDuplicate;

    pFileStdio = (e_file_stdio*)e_file_get_backend_data(pFile);
    E_ASSERT(pFileStdio != NULL);

    pDuplicatedFileStdio = (e_file_stdio*)e_file_get_backend_data(pDuplicatedFile);
    E_ASSERT(pDuplicatedFileStdio != NULL);

    fd = _fileno(pFileStdio->pFile);
    if (fd == -1) {
        return e_result_from_errno(errno);
    }

    hFile = (HANDLE)_get_osfhandle(fd);
    if (hFile == INVALID_HANDLE_VALUE) {
        return e_result_from_errno(errno);
    }

    if (!DuplicateHandle(GetCurrentProcess(), hFile, GetCurrentProcess(), &hFileDuplicate, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
        return e_result_from_errno(GetLastError());
    }

    fdDuplicate = _open_osfhandle((e_intptr)hFileDuplicate, _O_RDONLY);
    if (fdDuplicate == -1) {
        CloseHandle(hFileDuplicate);
        return e_result_from_errno(errno);
    }

    pDuplicatedFileStdio->pFile = _fdopen(fdDuplicate, pFileStdio->openMode);
    if (pDuplicatedFileStdio->pFile == NULL) {
        _close(fdDuplicate);
        return e_result_from_errno(errno);
    }

    return E_SUCCESS;
}


typedef struct e_iterator_stdio
{
    e_fs_iterator iterator;
    HANDLE hFind;
} e_iterator_stdio;

E_API void e_free_iterator_stdio(e_fs_iterator* pIterator)
{
    e_iterator_stdio* pIteratorStdio = (e_iterator_stdio*)pIterator;

    FindClose(pIteratorStdio->hFind);
    e_free(pIteratorStdio, e_fs_get_allocation_callbacks(pIterator->pFS));
}

static e_fs_iterator* e_iterator_stdio_resolve(e_iterator_stdio* pIteratorStdio, e_fs* pFS, HANDLE hFind, const WIN32_FIND_DATAW* pFD)
{
    e_iterator_stdio* pNewIteratorStdio;
    size_t allocSize;
    int nameLen;

    /*
    The name is stored at the end of the struct. In order to know how much memory to allocate we'll
    need to calculate the length of the name.
    */
    nameLen = WideCharToMultiByte(CP_UTF8, 0, pFD->cFileName, -1, NULL, 0, NULL, NULL);
    if (nameLen == 0) {
        e_free_iterator_stdio((e_fs_iterator*)pIteratorStdio);
        return NULL;
    }

    allocSize = E_MAX(sizeof(e_iterator_stdio) + nameLen, E_STDIO_MIN_ITERATOR_ALLOCATION_SIZE);    /* "nameLen" includes the null terminator. 1KB just to try to avoid excessive internal reallocations inside realloc(). */

    pNewIteratorStdio = (e_iterator_stdio*)e_realloc(pIteratorStdio, allocSize, e_fs_get_allocation_callbacks(pFS));
    if (pNewIteratorStdio == NULL) {
        e_free_iterator_stdio((e_fs_iterator*)pIteratorStdio);
        return NULL;
    }

    pNewIteratorStdio->iterator.pFS = pFS;
    pNewIteratorStdio->hFind        = hFind;

    /* Name. */
    pNewIteratorStdio->iterator.pName   = (char*)pNewIteratorStdio + sizeof(e_iterator_stdio);
    pNewIteratorStdio->iterator.nameLen = (size_t)nameLen - 1;  /* nameLen includes the null terminator. */
    WideCharToMultiByte(CP_UTF8, 0, pFD->cFileName, -1, (char*)pNewIteratorStdio->iterator.pName, nameLen, NULL, NULL);  /* const-cast is safe here. */

    /* Info. */
    pNewIteratorStdio->iterator.info = e_file_info_from_WIN32_FIND_DATAW(pFD);

    return (e_fs_iterator*)pNewIteratorStdio;
}

E_API e_fs_iterator* e_first_stdio(e_fs* pFS, const char* pDirectoryPath, size_t directoryPathLen)
{
    size_t i;
    int queryLen;
    int cbMultiByte;
    wchar_t  pQueryStack[1024];
    wchar_t* pQueryHeap = NULL;
    wchar_t* pQuery;
    HANDLE hFind;
    WIN32_FIND_DATAW fd;

    /* An empty path means the current directory. Win32 will want us to specify "." in this case. */
    if (pDirectoryPath == NULL || pDirectoryPath[0] == '\0') {
        pDirectoryPath = ".";
        directoryPathLen = 1;
    }

    if (directoryPathLen == E_NULL_TERMINATED) {
        cbMultiByte = -1;
    } else {
        if (directoryPathLen > 0xFFFFFFFF) {
            return NULL;
        }

        cbMultiByte = (int)directoryPathLen;
    }

    /* When iterating over files using Win32 you specify a wildcard pattern. The "+ 3" you see in the code below is for the wildcard pattern. We also need to make everything a backslash. */
    queryLen = MultiByteToWideChar(CP_UTF8, 0, pDirectoryPath, cbMultiByte, NULL, 0);
    if (queryLen == 0) {
        return NULL;
    }

    if ((queryLen + 3) > (int)E_COUNTOF(pQueryStack)) {
        pQueryHeap = (wchar_t*)e_malloc((queryLen + 3) * sizeof(wchar_t), e_fs_get_allocation_callbacks(pFS));
        if (pQueryHeap == NULL) {
            return NULL;
        }

        pQuery = pQueryHeap;
    }
    else {
        pQuery = pQueryStack;
    }

    MultiByteToWideChar(CP_UTF8, 0, pDirectoryPath, cbMultiByte, pQuery, queryLen);

    if (directoryPathLen == E_NULL_TERMINATED) {
        queryLen -= 1;  /* Remove the null terminator. Will not include the null terminator if the input string is not null terminated, hence why this is inside the conditional. */
    }

    /* Remove the trailing slash, if any. */
    if (pQuery[queryLen - 1] == L'\\' || pQuery[queryLen - 1] == L'/') {
        queryLen -= 1;
    }

    pQuery[queryLen + 0] = L'\\';
    pQuery[queryLen + 1] = L'*';
    pQuery[queryLen + 2] = L'\0';

    /* Convert to backslashes. */
    for (i = 0; i < (size_t)queryLen; i += 1) {
        if (pQuery[i] == L'/') {
            pQuery[i] = L'\\';
        }
    }

    hFind = FindFirstFileW(pQuery, &fd);
    e_free(pQueryHeap, e_fs_get_allocation_callbacks(pFS));

    if (hFind == INVALID_HANDLE_VALUE) {
        return NULL;
    }

    return e_iterator_stdio_resolve(NULL, pFS, hFind, &fd);
}

E_API e_fs_iterator* e_next_stdio(e_fs_iterator* pIterator)
{
    e_iterator_stdio* pIteratorStdio = (e_iterator_stdio*)pIterator;
    WIN32_FIND_DATAW fd;

    if (!FindNextFileW(pIteratorStdio->hFind, &fd)) {
        e_free_iterator_stdio(pIterator);
        return NULL;
    }

    return e_iterator_stdio_resolve(pIteratorStdio, pIterator->pFS, pIteratorStdio->hFind, &fd);
}
#else
#include <unistd.h>
#include <dirent.h>

E_API e_result e_file_duplicate_stdio(e_file* pFile, e_file* pDuplicatedFile)
{
    e_file_stdio* pFileStdio;
    e_file_stdio* pDuplicatedFileStdio;
    FILE* pDuplicatedFileHandle;

    /* These were all validated at a higher level. */
    E_ASSERT(pFile           != NULL);
    E_ASSERT(pDuplicatedFile != NULL);

    pFileStdio = (e_file_stdio*)e_file_get_backend_data(pFile);
    E_ASSERT(pFileStdio != NULL);

    pDuplicatedFileStdio = (e_file_stdio*)e_file_get_backend_data(pDuplicatedFile);
    E_ASSERT(pDuplicatedFileStdio != NULL);

    pDuplicatedFileHandle = fdopen(dup(fileno(pFileStdio->pFile)), pFileStdio->openMode);
    if (pDuplicatedFileHandle == NULL) {
        return e_result_from_errno(errno);
    }

    pDuplicatedFileStdio->pFile = pDuplicatedFileHandle;
    E_COPY_MEMORY(pDuplicatedFileStdio->openMode, pFileStdio->openMode, sizeof(pFileStdio->openMode));

    return E_SUCCESS;
}


typedef struct e_iterator_stdio
{
    e_fs_iterator iterator;
    DIR* pDir;
    char* pFullFilePath;        /* Points to the end of the structure. */
    size_t directoryPathLen;    /* The length of the directory section. */
} e_iterator_stdio;

E_API void e_free_iterator_stdio(e_fs_iterator* pIterator)
{
    e_iterator_stdio* pIteratorStdio = (e_iterator_stdio*)pIterator;

    E_ASSERT(pIteratorStdio != NULL);

    closedir(pIteratorStdio->pDir);
    e_free(pIteratorStdio, e_fs_get_allocation_callbacks(pIterator->pFS));
}

E_API e_fs_iterator* e_first_stdio(e_fs* pFS, const char* pDirectoryPath, size_t directoryPathLen)
{
    e_iterator_stdio* pIteratorStdio;
    struct dirent* info;
    struct stat statInfo;
    size_t fileNameLen;

    E_ASSERT(pDirectoryPath != NULL);

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

    if (directoryPathLen == 0 || pDirectoryPath[0] == '\0') {
        directoryPathLen = 1;
        pDirectoryPath = ".";
    }

    /* The first step is to calculate the length of the path if we need to. */
    if (directoryPathLen == (size_t)-1) {
        directoryPathLen = strlen(pDirectoryPath);
    }


    /*
    Now that we know the length of the directory we can allocate space for the iterator. The
    directory path will be placed at the end of the structure.
    */
    pIteratorStdio = (e_iterator_stdio*)e_malloc(E_MAX(sizeof(*pIteratorStdio) + directoryPathLen + 1, E_STDIO_MIN_ITERATOR_ALLOCATION_SIZE), e_fs_get_allocation_callbacks(pFS));    /* +1 for null terminator. */
    if (pIteratorStdio == NULL) {
        return NULL;
    }

    /* Point pFullFilePath to the end of structure to where the path is located. */
    pIteratorStdio->pFullFilePath = (char*)pIteratorStdio + sizeof(*pIteratorStdio);
    pIteratorStdio->directoryPathLen = directoryPathLen;

    /* We can now copy over the directory path. This will null terminate the path which will allow us to call opendir(). */
    e_strncpy_s(pIteratorStdio->pFullFilePath, directoryPathLen + 1, pDirectoryPath, directoryPathLen);

    /* We can now open the directory. */
    pIteratorStdio->pDir = opendir(pIteratorStdio->pFullFilePath);
    if (pIteratorStdio->pDir == NULL) {
        e_free(pIteratorStdio, e_fs_get_allocation_callbacks(pFS));
        return NULL;
    }

    /* We now need to get information about the first file. */
    info = readdir(pIteratorStdio->pDir);
    if (info == NULL) {
        closedir(pIteratorStdio->pDir);
        e_free(pIteratorStdio, e_fs_get_allocation_callbacks(pFS));
        return NULL;
    }

    fileNameLen = strlen(info->d_name);

    /*
    Now that we have the file name we need to append it to the full file path in the iterator. To do
    this we need to reallocate the iterator to account for the length of the file name, including the
    separating slash.
    */
    {
        e_iterator_stdio* pNewIteratorStdio= (e_iterator_stdio*)e_realloc(pIteratorStdio, E_MAX(sizeof(*pIteratorStdio) + directoryPathLen + 1 + fileNameLen + 1, E_STDIO_MIN_ITERATOR_ALLOCATION_SIZE), e_fs_get_allocation_callbacks(pFS));    /* +1 for null terminator. */
        if (pNewIteratorStdio == NULL) {
            closedir(pIteratorStdio->pDir);
            e_free(pIteratorStdio, e_fs_get_allocation_callbacks(pFS));
            return NULL;
        }

        pIteratorStdio = pNewIteratorStdio;
    }

    /* Memory has been allocated. Copy over the separating slash and file name. */
    pIteratorStdio->pFullFilePath = (char*)pIteratorStdio + sizeof(*pIteratorStdio);
    pIteratorStdio->pFullFilePath[directoryPathLen] = '/';
    e_strcpy(pIteratorStdio->pFullFilePath + directoryPathLen + 1, info->d_name);

    /* The pFileName member of the base iterator needs to be set to the file name. */
    pIteratorStdio->iterator.pName   = pIteratorStdio->pFullFilePath + directoryPathLen + 1;
    pIteratorStdio->iterator.nameLen = fileNameLen;

    /* We can now get the file information. */
    if (stat(pIteratorStdio->pFullFilePath, &statInfo) != 0) {
        closedir(pIteratorStdio->pDir);
        e_free(pIteratorStdio, e_fs_get_allocation_callbacks(pFS));
        return NULL;
    }

    pIteratorStdio->iterator.info = e_file_info_from_stat(&statInfo);

    return (e_fs_iterator*)pIteratorStdio;
}

E_API e_fs_iterator* e_next_stdio(e_fs_iterator* pIterator)
{
    e_iterator_stdio* pIteratorStdio = (e_iterator_stdio*)pIterator;
    struct dirent* info;
    struct stat statInfo;
    size_t fileNameLen;

    E_ASSERT(pIteratorStdio != NULL);

    /* We need to get information about the next file. */
    info = readdir(pIteratorStdio->pDir);
    if (info == NULL) {
        e_free_iterator_stdio((e_fs_iterator*)pIteratorStdio);
        return NULL;    /* The end of the directory. */
    }

    fileNameLen = strlen(info->d_name);

    /* We need to reallocate the iterator to account for the new file name. */
    {
        e_iterator_stdio* pNewIteratorStdio = (e_iterator_stdio*)e_realloc(pIteratorStdio, E_MAX(sizeof(*pIteratorStdio) + pIteratorStdio->directoryPathLen + 1 + fileNameLen + 1, E_STDIO_MIN_ITERATOR_ALLOCATION_SIZE), e_fs_get_allocation_callbacks(pIterator->pFS));    /* +1 for null terminator. */
        if (pNewIteratorStdio == NULL) {
            e_free_iterator_stdio((e_fs_iterator*)pIteratorStdio);
            return NULL;
        }

        pIteratorStdio = pNewIteratorStdio;
    }

    /* Memory has been allocated. Copy over the file name. */
    pIteratorStdio->pFullFilePath = (char*)pIteratorStdio + sizeof(*pIteratorStdio);
    e_strcpy(pIteratorStdio->pFullFilePath + pIteratorStdio->directoryPathLen + 1, info->d_name);

    /* The pFileName member of the base iterator needs to be set to the file name. */
    pIteratorStdio->iterator.pName   = pIteratorStdio->pFullFilePath + pIteratorStdio->directoryPathLen + 1;
    pIteratorStdio->iterator.nameLen = fileNameLen;

    /* We can now get the file information. */
    if (stat(pIteratorStdio->pFullFilePath, &statInfo) != 0) {
        e_free_iterator_stdio((e_fs_iterator*)pIteratorStdio);
        return NULL;
    }

    pIteratorStdio->iterator.info = e_file_info_from_stat(&statInfo);

    return (e_fs_iterator*)pIteratorStdio;
}
#endif

e_fs_backend e_stdio_backend =
{
    e_alloc_size_stdio,
    e_init_stdio,
    e_uninit_stdio,
    e_ioctl_stdio,
    e_remove_stdio,
    e_rename_stdio,
    e_mkdir_stdio,
    e_info_stdio,
    e_file_alloc_size_stdio,
    e_file_open_stdio,
    e_file_open_handle_stdio,
    e_file_close_stdio,
    e_file_read_stdio,
    e_file_write_stdio,
    e_file_seek_stdio,
    e_file_tell_stdio,
    e_file_flush_stdio,
    e_file_info_stdio,
    e_file_duplicate_stdio,
    e_first_stdio,
    e_next_stdio,
    e_free_iterator_stdio
};
const e_fs_backend* E_FS_STDIO = &e_stdio_backend;
#else
const e_fs_backend* E_FS_STDIO = NULL;
#endif
/* END e_fs.c */



/* BEG e_fs_zip.c */
#ifndef E_ZIP_CACHE_SIZE_IN_BYTES
#define E_ZIP_CACHE_SIZE_IN_BYTES              32768
#endif

#ifndef E_ZIP_COMPRESSED_CACHE_SIZE_IN_BYTES
#define E_ZIP_COMPRESSED_CACHE_SIZE_IN_BYTES   4096
#endif

#define E_ZIP_EOCD_SIGNATURE                   0x06054b50
#define E_ZIP_EOCD64_SIGNATURE                 0x06064b50
#define E_ZIP_EOCD64_LOCATOR_SIGNATURE         0x07064b50
#define E_ZIP_CD_FILE_HEADER_SIGNATURE         0x02014b50

#define E_ZIP_COMPRESSION_METHOD_STORE         0
#define E_ZIP_COMPRESSION_METHOD_DEFLATE       8


typedef struct e_zip_cd_node e_zip_cd_node;
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

typedef struct e_zip_index
{
    size_t offsetInBytes;           /* The offset in bytes of the item relative to the start of the central directory. */
} e_zip_index;

typedef struct e_zip
{
    size_t fileCount;               /* Total number of records in the central directory. */
    size_t centralDirectorySize;    /* Size in bytes of the central directory. */
    void* pCentralDirectory;        /* Offset of pHeap. */
    e_zip_index* pIndex;           /* Offset of pHeap. There will be fileCount items in this array, and each item is sorted by the file path of each item. */
    e_zip_cd_node* pCDRootNode;    /* The root node of our accelerated central directory data structure. */
    void* pHeap;                    /* A single heap allocation for storing the central directory and index. */
} e_zip;

typedef struct e_zip_file_info
{
    const char* pPath;
    size_t pathLen;
    e_uint16 compressionMethod;
    e_uint64 compressedSize;
    e_uint64 uncompressedSize;
    e_uint64 fileOffset;            /* The offset in bytes from the start of the archive file. */
    e_bool32 directory;
} e_zip_file_info;

static size_t e_alloc_size_zip(const void* pBackendConfig)
{
    (void)pBackendConfig;

    return sizeof(e_zip);
}


typedef struct e_zip_refstring
{
    const char* str;
    size_t len;
} e_zip_refstring;

static int e_zip_binary_search_zip_cd_node_compare(void* pUserData, const void* pKey, const void* pVal)
{
    const e_zip_refstring* pRefString = (const e_zip_refstring*)pKey;
    const e_zip_cd_node* pNode = (const e_zip_cd_node*)pVal;
    int compareResult;

    (void)pUserData;

    compareResult = strncmp(pRefString->str, pNode->pName, E_MIN(pRefString->len, pNode->nameLen));
    if (compareResult == 0 && pRefString->len != pNode->nameLen) {
        compareResult = (pRefString->len < pNode->nameLen) ? -1 : 1;
    }

    return compareResult;
}

static e_zip_cd_node* e_zip_cd_node_find_child(e_zip_cd_node* pParent, const char* pChildName, size_t childNameLen)
{
    e_zip_refstring str;
    str.str = pChildName;
    str.len = childNameLen;

    return (e_zip_cd_node*)e_sorted_search(&str, pParent->pChildren, pParent->childCount, sizeof(*pParent->pChildren), e_zip_binary_search_zip_cd_node_compare, NULL);
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

static e_result e_zip_find_file_by_path(e_zip* pZip, const e_allocation_callbacks* pAllocationCallbacks, const char* pFilePath, size_t filePathLen, size_t* pFileIndex)
{
    e_result result;
    e_path_iterator pathIterator;
    e_zip_cd_node* pCurrentNode;
    char  pFilePathCleanStack[1024];
    char* pFilePathCleanHeap = NULL;
    char* pFilePathClean;
    int filePathCleanLen;

    E_ASSERT(pZip       != NULL);
    E_ASSERT(pFilePath  != NULL);
    E_ASSERT(pFileIndex != NULL);

    if (filePathLen == 0) {
        return E_INVALID_ARGS; /* The path is empty. */
    }

    /* Skip past the root item if any. */
    if (pFilePath[0] == '/' || pFilePath[0] == '\\') {
        pFilePath += 1;
        if (filePathLen > 0) {
            filePathLen -= 1;
        }
    }

    /* The path must be clean of any special directories. We'll have to clean the path with e_path_normalize(). */
    filePathCleanLen = e_path_normalize(pFilePathCleanStack, sizeof(pFilePathCleanStack), pFilePath, filePathLen, E_NO_ABOVE_ROOT_NAVIGATION);
    if (filePathCleanLen < 0) {
        return E_DOES_NOT_EXIST;
    }

    if (filePathCleanLen > (int)sizeof(pFilePathCleanStack)) {
        pFilePathCleanHeap = (char*)e_malloc(filePathCleanLen + 1, pAllocationCallbacks);
        if (pFilePathCleanHeap == NULL) {
            return E_OUT_OF_MEMORY;
        }

        e_path_normalize(pFilePathCleanHeap, filePathCleanLen + 1, pFilePath, filePathLen, E_NO_ABOVE_ROOT_NAVIGATION); /* <-- This should never fail. */
        pFilePathClean = pFilePathCleanHeap;
    } else {
        pFilePathClean = pFilePathCleanStack;
    }

    /* Start at the root node. */
    pCurrentNode = pZip->pCDRootNode;

    result = e_result_from_errno(e_path_first(pFilePathClean, (size_t)filePathCleanLen, &pathIterator));
    if (result == E_SUCCESS) {
        /* Reset the error code for safety. The loop below will be setting it to a proper value. */
        result = E_DOES_NOT_EXIST;
        for (;;) {
            e_zip_cd_node* pChildNode;
            
            pChildNode = e_zip_cd_node_find_child(pCurrentNode, pathIterator.pFullPath + pathIterator.segmentOffset, pathIterator.segmentLength);
            if (pChildNode == NULL) {
                result = E_DOES_NOT_EXIST;
                break;
            }

            pCurrentNode = pChildNode;

            result = e_result_from_errno(e_path_next(&pathIterator));
            if (result != E_SUCCESS) {
                /* We've reached the end. The file we're on must be the file index. */
                *pFileIndex = pCurrentNode->iFile;

                result = E_SUCCESS;
                break;
            }
        }
    } else {
        result = E_DOES_NOT_EXIST;
    }

    e_free(pFilePathCleanHeap, pAllocationCallbacks);
    return result;
}

static e_result e_zip_get_file_info_by_path(e_zip* pZip, const e_allocation_callbacks* pAllocationCallbacks, const char* pFilePath, size_t filePathLen, e_zip_file_info* pInfo)
{
    e_result result;
    size_t iFile;

    E_ASSERT(pZip      != NULL);
    E_ASSERT(pFilePath != NULL);
    E_ASSERT(pInfo     != NULL);

    result = e_zip_find_file_by_path(pZip, pAllocationCallbacks, pFilePath, filePathLen, &iFile);
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
        e_path_iterator shortFilePathIterator;

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
        if (e_path_first(pShortFilePath, shortFilePathLen, &shortFilePathIterator) == E_SUCCESS) {
            if (pNode->childCount == 0 || pNode->pChildren[pNode->childCount-1].nameLen != shortFilePathIterator.segmentLength || strncmp(pNode->pChildren[pNode->childCount-1].pName, shortFilePathIterator.pFullPath + shortFilePathIterator.segmentOffset, E_MIN(pNode->pChildren[pNode->childCount-1].nameLen, shortFilePathIterator.segmentLength)) != 0) {
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


static e_result e_init_zip(e_fs* pFS, const void* pBackendConfig, e_stream* pStream)
{
    e_zip* pZip;
    
    e_result result;
    e_uint32 sig;
    int eocdPositionFromEnd;
    e_uint16 cdRecordCount16;
    e_uint64 cdRecordCount64;
    e_uint32 cdSizeInBytes32;
    e_uint64 cdSizeInBytes64;
    e_uint32 cdOffset32;
    e_uint64 cdOffset64;

    /* No need for a backend config. Maybe use this later for passwords if we ever add support for that? */
    (void)pBackendConfig;

    if (pStream == NULL) {
        return E_INVALID_OPERATION;    /* Most likely the FS is being opened without a stream. */
    }
    
    pZip = (e_zip*)e_fs_get_backend_data(pFS);
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
    result = e_stream_seek(pStream, -22, E_SEEK_END);
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
        result = e_stream_seek(pStream, -(22 + 65535), E_SEEK_END);
        if (result != E_SUCCESS) {
            /*
            We failed the seek, but it most likely means we were just trying to seek to far back in
            which case we can just fall back to a seek to position 0.
            */
            result = e_stream_seek(pStream, 0, E_SEEK_SET);
            if (result != E_SUCCESS) {
                return result;
            }
        }

        /*
        We now need to scan byte-by-byte until we find the signature. We could allocate this on the
        stack, but that takes a little bit too much stack space than I feel comfortable with. We
        could also allocate a buffer on the heap, but that's just needlessly inefficient. Instead
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

                    result = e_stream_seek(pStream, eocdPositionFromEnd + 4, E_SEEK_END);  /* +4 so go just past the signatures. */
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
    result = e_stream_seek(pStream, 2 + 2 + 2, E_SEEK_CUR);  /* Skip past disk stuff. */
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

        result = e_stream_seek(pStream, eocdPositionFromEnd - 20, E_SEEK_END);
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
        result = e_stream_seek(pStream, 4, E_SEEK_CUR);
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
        result = e_stream_seek(pStream, eocd64OffsetInBytes, E_SEEK_SET);
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
        result = e_stream_seek(pStream, 20, E_SEEK_CUR);
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
            return E_TOO_BIG;  /* Too many records. Will never fit what we need in memory. */
        }
        if (cdSizeInBytes64 > E_SIZE_MAX) {
            return E_TOO_BIG;  /* Central directory is too big to fit into memory. */
        }

        pZip->fileCount = (size_t)cdRecordCount64;  /* Safe cast. Checked above. */
        
    } else {
        /* It's a 32-bit archive. */
        pZip->fileCount = cdRecordCount16;
        pZip->centralDirectorySize = cdSizeInBytes32;

        cdSizeInBytes64 = cdSizeInBytes32;
        cdOffset64 = cdOffset32;
    }

    /* We need to seek to the start of the central directory and read it's contents. */
    result = e_stream_seek(pStream, cdOffset64, E_SEEK_SET);
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


    pZip->pHeap = e_malloc(E_ALIGN(pZip->centralDirectorySize, E_SIZEOF_PTR) + (sizeof(*pZip->pIndex) * pZip->fileCount), e_fs_get_allocation_callbacks(pFS));
    if (pZip->pHeap == NULL) {
        return E_OUT_OF_MEMORY;
    }

    pZip->pCentralDirectory =                E_OFFSET_PTR(pZip->pHeap, 0);
    pZip->pIndex            = (e_zip_index*)E_OFFSET_PTR(pZip->pHeap, E_ALIGN(pZip->centralDirectorySize, E_SIZEOF_PTR));
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
            e_free(pZip->pHeap, e_fs_get_allocation_callbacks(pFS));
            return result;
        }

        for (iFile = 0; iFile < pZip->fileCount; iFile += 1) {
            size_t fileOffset;
            e_uint16 fileNameLen;
            e_uint16 extraLen;
            e_uint16 commentLen;

            result = e_memory_stream_tell(&cdStream, &fileOffset);
            if (result != E_SUCCESS) {
                e_free(pZip->pHeap, e_fs_get_allocation_callbacks(pFS));
                return result;
            }

            pZip->pIndex[iFile].offsetInBytes = fileOffset;


            /*
            We need to seek to the next item. To do this we need to retrieve the lengths of the
            variable-length fields. These start from offset 28.
            */
            result = e_memory_stream_seek(&cdStream, 28, E_SEEK_CUR);
            if (result != E_SUCCESS) {
                e_free(pZip->pHeap, e_fs_get_allocation_callbacks(pFS));
                return result;
            }

            result = e_memory_stream_read(&cdStream, &fileNameLen, 2, NULL);
            if (result != E_SUCCESS) {
                e_free(pZip->pHeap, e_fs_get_allocation_callbacks(pFS));
                return result;
            }

            result = e_memory_stream_read(&cdStream, &extraLen, 2, NULL);
            if (result != E_SUCCESS) {
                e_free(pZip->pHeap, e_fs_get_allocation_callbacks(pFS));
                return result;
            }

            result = e_memory_stream_read(&cdStream, &commentLen, 2, NULL);
            if (result != E_SUCCESS) {
                e_free(pZip->pHeap, e_fs_get_allocation_callbacks(pFS));
                return result;
            }

            /* We have the necessary information we need to move past this record. */
            result = e_memory_stream_seek(&cdStream, fileOffset + 46 + fileNameLen + extraLen + commentLen, E_SEEK_SET);
            if (result != E_SUCCESS) {
                e_free(pZip->pHeap, e_fs_get_allocation_callbacks(pFS));
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
                    e_path_iterator nextIterator;
                    e_path_iterator prevIterator;

                    e_path_first(pFilePath, filePathLen, &nextIterator);   /* <-- This should never fail. */

                    if (e_path_first(pPrevPath, prevPathLen, &prevIterator) == E_SUCCESS) {
                        /*
                        First just move the next iterator forward until we reach the end of the previous
                        iterator, or if the segments differ between the two.
                        */
                        for (;;) {
                            if (e_path_iterators_compare(&nextIterator, &prevIterator) != 0) {
                                break;  /* Iterators don't match. */
                            }

                            /* Getting here means the segments match. We need to move to the next one. */
                            if (e_path_next(&nextIterator) != E_SUCCESS) {
                                break;  /* We reached the end of the next iterator before the previous. The only difference will be the file name. */
                            }

                            if (e_path_next(&prevIterator) != E_SUCCESS) {
                                break;  /* We reached the end of the prev iterator. Get out of the loop. */
                            }
                        }
                    }

                    /* Increment the counter to account for the segment that the next iterator is currently sitting on. */
                    nodeUpperBoundCount += 1;

                    /* Now we need to increment the counter for every new segment. */
                    while (e_path_next(&nextIterator) == E_SUCCESS) {
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
            void* pNewHeap = e_realloc(pZip->pHeap, E_ALIGN(pZip->centralDirectorySize, E_SIZEOF_PTR) + (sizeof(*pZip->pIndex) * pZip->fileCount) + (sizeof(*pZip->pCDRootNode) * nodeUpperBoundCount), e_fs_get_allocation_callbacks(pFS));
            if (pNewHeap == NULL) {
                e_free(pZip->pHeap, e_fs_get_allocation_callbacks(pFS));
                return E_OUT_OF_MEMORY;
            }

            pZip->pHeap = pNewHeap;
            pZip->pCentralDirectory =                  E_OFFSET_PTR(pZip->pHeap, 0);
            pZip->pIndex            = (e_zip_index*  )E_OFFSET_PTR(pZip->pHeap, E_ALIGN(pZip->centralDirectorySize, E_SIZEOF_PTR));
            pZip->pCDRootNode       = (e_zip_cd_node*)E_OFFSET_PTR(pZip->pHeap, E_ALIGN(pZip->centralDirectorySize, E_SIZEOF_PTR) + (sizeof(*pZip->pIndex) * pZip->fileCount));
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

    return E_SUCCESS;
}

static void e_uninit_zip(e_fs* pFS)
{
    e_zip* pZip = (e_zip*)e_fs_get_backend_data(pFS);
    E_ASSERT(pZip != NULL);

    e_free(pZip->pHeap, e_fs_get_allocation_callbacks(pFS));
    return;
}

static e_result e_ioctl_zip(e_fs* pFS, int op, void* pArg)
{
    e_zip* pZip = (e_zip*)e_fs_get_backend_data(pFS);
    E_ASSERT(pZip != NULL);

    (void)pZip;
    (void)op;
    (void)pArg;

    return E_NOT_IMPLEMENTED;
}

static e_result e_info_zip(e_fs* pFS, const char* pPath, int openMode, e_file_info* pInfo)
{
    e_result result;
    e_zip* pZip;
    e_zip_file_info info;

    (void)openMode;
    
    pZip = (e_zip*)e_fs_get_backend_data(pFS);
    E_ASSERT(pZip != NULL);

    result = e_zip_get_file_info_by_path(pZip, e_fs_get_allocation_callbacks(pFS), pPath, (size_t)-1, &info);
    if (result != E_SUCCESS) {
        return result;  /* Probably not found. */
    }

    pInfo->size      = info.uncompressedSize;
    pInfo->directory = info.directory;

    return E_SUCCESS;
}


typedef struct e_iterator_zip
{
    e_fs_iterator iterator;
    e_zip* pZip;
    e_zip_cd_node* pDirectoryNode;
    size_t iChild;
} e_iterator_zip;

typedef struct e_file_zip
{
    e_stream* pStream;                       /* Duplicated from the main file system stream. Freed with e_stream_delete_duplicate(). */
    e_zip_file_info info;
    e_uint64 absoluteCursorUncompressed;
    e_uint64 absoluteCursorCompressed;         /* The position of the cursor in the compressed data. */
    e_deflate_decompressor decompressor; /* Only used for compressed files. */
    size_t cacheCap;                            /* The capacity of the cache. Never changes. */
    size_t cacheSize;                           /* The number of valid bytes in the cache. Can be less than the capacity, but never more. Will be less when holding the tail end fo the file data. */
    size_t cacheCursor;                         /* The cursor within the cache. The cache size minus the cursor defines how much data remains in the cache. */
    unsigned char* pCache;                      /* Cache must be at least 32K. Stores uncompressed data. Stored at the end of the struct. */
    size_t compressedCacheCap;                  /* The capacity of the compressed cache. Never changes. */
    size_t compressedCacheSize;                 /* The number of valid bytes in the compressed cache. Can be less than the capacity, but never more. Will be less when holding the tail end fo the file data. */
    size_t compressedCacheCursor;               /* The cursor within the compressed cache. The compressed cache size minus the cursor defines how much data remains in the compressed cache. */
    unsigned char* pCompressedCache;            /* Only used for compressed files. */
} e_file_zip;

static size_t e_file_alloc_size_zip(e_fs* pFS)
{
    (void)pFS;
    return sizeof(e_file_zip) + E_ZIP_CACHE_SIZE_IN_BYTES + E_ZIP_COMPRESSED_CACHE_SIZE_IN_BYTES;
}

static e_result e_file_open_zip(e_fs* pFS, e_stream* pStream, const char* pPath, int openMode, e_file* pFile)
{
    e_zip* pZip;
    e_file_zip* pZipFile;
    e_result result;

    pZip = (e_zip*)e_fs_get_backend_data(pFS);
    E_ASSERT(pZip != NULL);

    pZipFile = (e_file_zip*)e_file_get_backend_data(pFile);
    E_ASSERT(pZipFile != NULL);

    /* Write mode is currently unsupported. */
    if ((openMode & (E_WRITE | E_APPEND | E_OVERWRITE | E_TRUNCATE)) != 0) {
        return E_INVALID_OPERATION;
    }

    pZipFile->pStream = pStream;

    /* We need to find the file info by it's path. */
    result = e_zip_get_file_info_by_path(pZip, e_fs_get_allocation_callbacks(pFS), pPath, (size_t)-1, &pZipFile->info);
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

    pZipFile->pCache           = (unsigned char*)E_OFFSET_PTR(pZipFile, sizeof(e_file_zip));
    pZipFile->pCompressedCache = (unsigned char*)E_OFFSET_PTR(pZipFile, sizeof(e_file_zip) + pZipFile->cacheCap);

    /*
    We need to move the file offset forward so that it's pointing to the first byte of the actual
    data. It's currently sitting at the top of the local header which isn't really useful for us.
    To move forward we need to get the length of the file path and the extra data and seek past
    the local header.
    */
    {
        e_uint16 fileNameLen;
        e_uint16 extraLen;

        result = e_stream_seek(pZipFile->pStream, pZipFile->info.fileOffset + 26, E_SEEK_SET);
        if (result != E_SUCCESS) {
            return result;
        }

        result = e_stream_read(pZipFile->pStream, &fileNameLen, 2, NULL);
        if (result != E_SUCCESS) {
            return result;
        }

        result = e_stream_read(pZipFile->pStream, &extraLen, 2, NULL);
        if (result != E_SUCCESS) {
            return result;
        }

        pZipFile->info.fileOffset += (e_uint32)30 + fileNameLen + extraLen;
    }


    /* Initialize the decompressor if necessary. */
    if (pZipFile->info.compressionMethod == E_ZIP_COMPRESSION_METHOD_DEFLATE) {
        result = e_deflate_decompressor_init(&pZipFile->decompressor);
        if (result != E_SUCCESS) {
            return result;
        }
    }


    return E_SUCCESS;
}

static e_result e_file_open_handle_zip(e_fs* pFS, void* hBackendFile, e_file* pFile)
{
    (void)pFS;
    (void)hBackendFile;
    (void)pFile;

    return E_NOT_IMPLEMENTED;
}

static void e_file_close_zip(e_file* pFile)
{
    /* Nothing to do. */
    (void)pFile;
}


static e_result e_file_read_zip_store(e_fs* pFS, e_file_zip* pZipFile, void* pDst, size_t bytesToRead, size_t* pBytesRead)
{
    e_result result;
    e_uint64 bytesRemainingInFile;
    size_t bytesRead;

    E_ASSERT(pZipFile   != NULL);
    E_ASSERT(pBytesRead != NULL);

    (void)pFS;

    bytesRemainingInFile = pZipFile->info.uncompressedSize - pZipFile->absoluteCursorUncompressed;
    if (bytesRemainingInFile == 0) {
        return E_AT_END;   /* Nothing left to read. Must return E_AT_END. */
    }

    if (bytesToRead > bytesRemainingInFile) {
        bytesToRead = (size_t)bytesRemainingInFile;
    }

    bytesRead = 0;

    /* Read from the cache first. */
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
        size_t bytesRemainingToRead = bytesToRead - bytesRead;
        size_t bytesToReadFromArchive;

        result = e_stream_seek(pZipFile->pStream, pZipFile->info.fileOffset + (pZipFile->absoluteCursorUncompressed + bytesRead), E_SEEK_SET);
        if (result != E_SUCCESS) {
            return result;
        }

        if (bytesRemainingToRead > pZipFile->cacheCap) {
            size_t bytesReadFromArchive;

            bytesToReadFromArchive = (bytesRemainingToRead / pZipFile->cacheCap) * pZipFile->cacheCap;

            result = e_stream_read(pZipFile->pStream, E_OFFSET_PTR(pDst, bytesRead), bytesToReadFromArchive, &bytesReadFromArchive);
            if (result != E_SUCCESS) {
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

            result = e_stream_read(pZipFile->pStream, pZipFile->pCache, (size_t)E_MIN(pZipFile->cacheCap, (pZipFile->info.uncompressedSize - (pZipFile->absoluteCursorUncompressed + bytesRead))), &pZipFile->cacheSize); /* Safe cast to size_t because reading will be clamped to bytesToRead. */
            if (result != E_SUCCESS) {
                return result;
            }

            pZipFile->cacheCursor = 0;

            E_COPY_MEMORY(E_OFFSET_PTR(pDst, bytesRead), pZipFile->pCache + pZipFile->cacheCursor, bytesRemainingToRead);
            pZipFile->cacheCursor += bytesRemainingToRead;

            bytesRead += bytesRemainingToRead;
        }
    }

    pZipFile->absoluteCursorUncompressed += bytesRead;

    /* We're done. */
    *pBytesRead = bytesRead;
    return E_SUCCESS;
}

static e_result e_file_read_zip_deflate(e_fs* pFS, e_file_zip* pZipFile, void* pDst, size_t bytesToRead, size_t* pBytesRead)
{
    e_result result;
    e_uint64 uncompressedBytesRemainingInFile;
    size_t uncompressedBytesRead;

    E_ASSERT(pZipFile != NULL);
    E_ASSERT(pBytesRead != NULL);

    (void)pFS;

    uncompressedBytesRemainingInFile = pZipFile->info.uncompressedSize - pZipFile->absoluteCursorUncompressed;
    if (uncompressedBytesRemainingInFile == 0) {
        return E_AT_END;   /* Nothing left to read. Must return E_AT_END. */
    }

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

                /* Make sure we're positioned correctly in the stream before we read. */
                result = e_stream_seek(pZipFile->pStream, pZipFile->info.fileOffset + pZipFile->absoluteCursorCompressed, E_SEEK_SET);
                if (result != E_SUCCESS) {
                    return result;
                }

                /*
                Read the compressed data into the cache. The number of compressed bytes we read needs
                to be clamped to the number of bytes remaining in the file and the number of bytes
                remaining in the cache.
                */
                compressedBytesToRead = (size_t)E_MIN(pZipFile->compressedCacheCap - pZipFile->compressedCacheCursor, (pZipFile->info.compressedSize - pZipFile->absoluteCursorCompressed));

                result = e_stream_read(pZipFile->pStream, pZipFile->pCompressedCache + pZipFile->compressedCacheCursor, compressedBytesToRead, &compressedBytesRead);
                /*
                We'll inspect the result later after we've escaped from the locked section just to
                keep the lock as small as possible.
                */

                pZipFile->absoluteCursorCompressed += compressedBytesRead;

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

static e_result e_file_read_zip(e_file* pFile, void* pDst, size_t bytesToRead, size_t* pBytesRead)
{
    e_file_zip* pZipFile;

    pZipFile = (e_file_zip*)e_file_get_backend_data(pFile);
    E_ASSERT(pZipFile != NULL);

    if (pZipFile->info.compressionMethod == E_ZIP_COMPRESSION_METHOD_STORE) {
        return e_file_read_zip_store(e_file_get_fs(pFile), pZipFile, pDst, bytesToRead, pBytesRead);
    } else if (pZipFile->info.compressionMethod == E_ZIP_COMPRESSION_METHOD_DEFLATE) {
        return e_file_read_zip_deflate(e_file_get_fs(pFile), pZipFile, pDst, bytesToRead, pBytesRead);
    } else {
        return E_INVALID_FILE;  /* Should never get here. */
    }
}

static e_result e_file_write_zip(e_file* pFile, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten)
{
    /* Write not supported. */
    (void)pFile;
    (void)pSrc;
    (void)bytesToWrite;
    (void)pBytesWritten;
    return E_NOT_IMPLEMENTED;
}

static e_result e_file_seek_zip(e_file* pFile, e_int64 offset, e_seek_origin origin)
{
    e_file_zip* pZipFile;
    e_int64 newSeekTarget;
    e_uint64 newAbsoluteCursor;

    pZipFile = (e_file_zip*)e_file_get_backend_data(pFile);
    E_ASSERT(pZipFile != NULL);

    if (origin == E_SEEK_SET) {
        newSeekTarget = 0;
    } else if (origin == E_SEEK_CUR) {
        newSeekTarget = pZipFile->absoluteCursorUncompressed;
    } else if (origin == E_SEEK_END) {
        newSeekTarget = pZipFile->info.uncompressedSize;
    } else {
        E_ASSERT(!"Invalid seek origin.");
        return E_INVALID_ARGS;
    }

    newSeekTarget += offset;
    if (newSeekTarget < 0) {
        return E_BAD_SEEK;  /* Trying to seek before the start of the file. */
    }
    if ((e_uint64)newSeekTarget > pZipFile->info.uncompressedSize) {
        return E_BAD_SEEK;  /* Trying to seek beyond the end of the file. */
    }

    newAbsoluteCursor = (e_uint64)newSeekTarget;

    /*
    We can do fast seeking if we are moving within the cache. Otherwise we just move the cursor and
    clear the cache. The next time we read, it'll see that the cache is empty which will trigger a
    fresh read of data from the archive stream.
    */
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

            /* The decompressor needs to be reset. */
            e_deflate_decompressor_init(&pZipFile->decompressor);
        }

        /* Now we just keep reading until we get to the seek point. */
        while (pZipFile->absoluteCursorUncompressed < newAbsoluteCursor) {  /* <-- absoluteCursorUncompressed will be incremented by e_file_read_zip(). */
            e_uint8 temp[4096];
            e_uint64 bytesToRead;
            size_t bytesRead;
            e_result result;

            bytesToRead = newAbsoluteCursor - pZipFile->absoluteCursorUncompressed;
            if (bytesToRead > sizeof(temp)) {
                bytesToRead = sizeof(temp);
            }
            
            bytesRead = 0;
            result = e_file_read_zip(pFile, temp, (size_t)bytesToRead, &bytesRead);    /* Safe cast to size_t because the bytes to read will be clamped to sizeof(temp). */
            if (result != E_SUCCESS) {
                return result;
            }

            if (bytesRead == 0) {
                return E_BAD_SEEK;  /* Trying to seek beyond the end of the file. */
            }
        }
    }

    /* Make sure the absolute cursor is set to the new position. */
    pZipFile->absoluteCursorUncompressed = newAbsoluteCursor;

    return E_SUCCESS;
}

static e_result e_file_tell_zip(e_file* pFile, e_int64* pCursor)
{
    e_file_zip* pZipFile = (e_file_zip*)e_file_get_backend_data(pFile);

    E_ASSERT(pZipFile != NULL);
    E_ASSERT(pCursor  != NULL);

    *pCursor = pZipFile->absoluteCursorUncompressed;
    return E_SUCCESS;
}

static e_result e_file_flush_zip(e_file* pFile)
{
    /* Nothing to do. */
    (void)pFile;
    return E_SUCCESS;
}

static e_result e_file_info_zip(e_file* pFile, e_file_info* pInfo)
{
    e_file_zip* pZipFile = (e_file_zip*)e_file_get_backend_data(pFile);

    E_ASSERT(pZipFile != NULL);
    E_ASSERT(pInfo    != NULL);

    pInfo->size      = pZipFile->info.uncompressedSize;
    pInfo->directory = E_FALSE; /* An opened file should never be a directory. */
    
    return E_SUCCESS;
}

static e_result e_file_duplicate_zip(e_file* pFile, e_file* pDuplicatedFile)
{
    e_file_zip* pZipFile;
    e_file_zip* pDuplicatedZipFile;

    pZipFile = (e_file_zip*)e_file_get_backend_data(pFile);
    E_ASSERT(pZipFile != NULL);

    pDuplicatedZipFile = (e_file_zip*)e_file_get_backend_data(pDuplicatedFile);
    E_ASSERT(pDuplicatedZipFile != NULL);

    /* We should be able to do this with a simple memcpy. */
    E_COPY_MEMORY(pDuplicatedZipFile, pZipFile, e_file_alloc_size_zip(e_file_get_fs(pFile)));

    return E_SUCCESS;
}

static void e_iterator_zip_init(e_zip* pZip, e_zip_cd_node* pChild, e_iterator_zip* pIterator)
{
    e_zip_file_info info;

    E_ASSERT(pIterator != NULL);
    E_ASSERT(pChild    != NULL);
    E_ASSERT(pZip      != NULL);

    pIterator->pZip = pZip;

    /* Name. */
    e_strncpy_s((char*)pIterator + sizeof(*pIterator), pChild->nameLen + 1, pChild->pName, pChild->nameLen);
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


/*
We use a minimum allocation size for iterators as an attempt to avoid the need for internal
reallocations during realloc().
*/
#define E_ZIP_MIN_ITERATOR_ALLOCATION_SIZE 1024

E_API e_fs_iterator* e_first_zip(e_fs* pFS, const char* pDirectoryPath, size_t directoryPathLen)
{
    e_zip* pZip;
    e_iterator_zip* pIterator;
    e_path_iterator directoryPathIterator;
    e_zip_cd_node* pCurrentNode;
    char  pDirectoryPathCleanStack[1024];
    char* pDirectoryPathCleanHeap = NULL;
    char* pDirectoryPathClean;
    int directoryPathCleanLen;

    pZip = (e_zip*)e_fs_get_backend_data(pFS);
    E_ASSERT(pZip != NULL);

    if (pDirectoryPath == NULL) {
        pDirectoryPath = "";
    }

    /* Skip past any leading slash. */
    if (pDirectoryPath[0] == '/' || pDirectoryPath[0] == '\\') {
        pDirectoryPath += 1;
        if (directoryPathLen > 0) {
            directoryPathLen -= 1;
        }
    }

    /* The path must be clean of any special directories. We'll have to clean the path with e_path_normalize(). */
    directoryPathCleanLen = e_path_normalize(pDirectoryPathCleanStack, sizeof(pDirectoryPathCleanStack), pDirectoryPath, directoryPathLen, E_NO_ABOVE_ROOT_NAVIGATION);
    if (directoryPathCleanLen < 0) {
        return NULL;
    }

    if (directoryPathCleanLen > (int)sizeof(pDirectoryPathCleanStack)) {
        pDirectoryPathCleanHeap = (char*)e_malloc(directoryPathCleanLen + 1, e_fs_get_allocation_callbacks(pFS));
        if (pDirectoryPathCleanHeap == NULL) {
            return NULL;    /* Out of memory. */
        }

        e_path_normalize(pDirectoryPathCleanHeap, directoryPathCleanLen + 1, pDirectoryPath, directoryPathLen, E_NO_ABOVE_ROOT_NAVIGATION);   /* <-- This should never fail. */
        pDirectoryPathClean = pDirectoryPathCleanHeap;
    } else {
        pDirectoryPathClean = pDirectoryPathCleanStack;
    }

    /* Always start from the root node. */
    pCurrentNode = pZip->pCDRootNode;

    /*
    All we need to do is find the node the corresponds to the specified directory path. To do this
    we just iterate over each segment in the path and get the children one after the other.
    */
    if (e_result_from_errno(e_path_first(pDirectoryPathClean, directoryPathCleanLen, &directoryPathIterator)) == E_SUCCESS) {
        for (;;) {
            /* Try finding the child node. If this cannot be found, the directory does not exist. */
            e_zip_cd_node* pChildNode;

            pChildNode = e_zip_cd_node_find_child(pCurrentNode, directoryPathIterator.pFullPath + directoryPathIterator.segmentOffset, directoryPathIterator.segmentLength);
            if (pChildNode == NULL) {
                e_free(pDirectoryPathCleanHeap, e_fs_get_allocation_callbacks(pFS));
                return NULL;    /* Does not exist. */
            }

            pCurrentNode = pChildNode;
            
            /* Go to the next segment if we have one. */
            if (e_result_from_errno(e_path_next(&directoryPathIterator)) != E_SUCCESS) {
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

    /* The heap allocation of the clean path is no longer needed, if we have one. */
    e_free(pDirectoryPathCleanHeap, e_fs_get_allocation_callbacks(pFS));

    /* If the current node does not have any children, there is no first item and therefore nothing to return. */
    if (pCurrentNode->childCount == 0) {
        return NULL;
    }

    /*
    Now that we've found the node we have enough information to allocate the iterator. We allocate
    room for a copy of the name so we can null terminate it.
    */
    pIterator = (e_iterator_zip*)e_realloc(NULL, E_MAX(sizeof(*pIterator) + pCurrentNode->pChildren[0].nameLen + 1, E_ZIP_MIN_ITERATOR_ALLOCATION_SIZE), e_fs_get_allocation_callbacks(pFS));
    if (pIterator == NULL) {
        return NULL;
    }

    e_iterator_zip_init(pZip, &pCurrentNode->pChildren[0], pIterator);

    /* Internal variables for iteration. */
    pIterator->pDirectoryNode = pCurrentNode;
    pIterator->iChild         = 0;

    return (e_fs_iterator*)pIterator;
}

E_API e_fs_iterator* e_next_zip(e_fs_iterator* pIterator)
{
    e_iterator_zip* pIteratorZip = (e_iterator_zip*)pIterator;
    e_iterator_zip* pNewIteratorZip;

    if (pIteratorZip == NULL) {
        return NULL;
    }

    /* All we're doing is going to the next child. If there's nothing left we just free the iterator and return null. */
    pIteratorZip->iChild += 1;
    if (pIteratorZip->iChild >= pIteratorZip->pDirectoryNode->childCount) {
        e_free(pIteratorZip, e_fs_get_allocation_callbacks(pIterator->pFS));
        return NULL;    /* Nothing left. */
    }

    /* Getting here means there's another child to iterate. */
    pNewIteratorZip = (e_iterator_zip*)e_realloc(pIteratorZip, E_MAX(sizeof(*pIteratorZip) + pIteratorZip->pDirectoryNode->pChildren[pIteratorZip->iChild].nameLen + 1, E_ZIP_MIN_ITERATOR_ALLOCATION_SIZE), e_fs_get_allocation_callbacks(pIterator->pFS));
    if (pNewIteratorZip == NULL) {
        e_free(pIteratorZip, e_fs_get_allocation_callbacks(pIterator->pFS));
        return NULL;    /* Out of memory. */
    }

    e_iterator_zip_init(pNewIteratorZip->pZip, &pNewIteratorZip->pDirectoryNode->pChildren[pNewIteratorZip->iChild], pNewIteratorZip);

    return (e_fs_iterator*)pNewIteratorZip;
}

E_API void e_free_iterator_zip(e_fs_iterator* pIterator)
{
    e_free(pIterator, e_fs_get_allocation_callbacks(pIterator->pFS));
}


e_fs_backend e_zip_backend =
{
    e_alloc_size_zip,
    e_init_zip,
    e_uninit_zip,
    e_ioctl_zip,
    NULL,   /* remove */
    NULL,   /* rename */
    NULL,   /* mkdir */
    e_info_zip,
    e_file_alloc_size_zip,
    e_file_open_zip,
    e_file_open_handle_zip,
    e_file_close_zip,
    e_file_read_zip,
    e_file_write_zip,
    e_file_seek_zip,
    e_file_tell_zip,
    e_file_flush_zip,
    e_file_info_zip,
    e_file_duplicate_zip,
    e_first_zip,
    e_next_zip,
    e_free_iterator_zip
};
const e_fs_backend* E_FS_ZIP = &e_zip_backend;
/* END e_fs_zip.c */


/* BEG e_fs_sub.c */
typedef struct e_sub
{
    e_fs* pOwnerFS;
    char* pRootDir;   /* Points to the end of the structure. */
    size_t rootDirLen;
} e_sub;

typedef struct e_file_sub
{
    e_file* pActualFile;
} e_file_sub;


typedef struct e_sub_path
{
    char  pFullPathStack[1024];
    char* pFullPathHeap;
    char* pFullPath;
    int fullPathLen;
} e_sub_path;

static e_result e_sub_path_init(e_fs* pFS, const char* pPath, size_t pathLen, e_sub_path* pSubFSPath)
{
    e_sub* pSubFS;
    char  pPathCleanStack[1024];
    char* pPathCleanHeap = NULL;
    char* pPathClean;
    size_t pathCleanLen;

    E_ASSERT(pFS        != NULL);
    E_ASSERT(pPath      != NULL);
    E_ASSERT(pSubFSPath != NULL);

    E_ZERO_OBJECT(pSubFSPath);   /* Safety. */

    /* We first have to clean the path, with a strict requirement that we fail if attempting to navigate above the root. */
    pathCleanLen = e_path_normalize(pPathCleanStack, sizeof(pPathCleanStack), pPath, pathLen, E_NO_ABOVE_ROOT_NAVIGATION);
    if (pathCleanLen <= 0) {
        return E_DOES_NOT_EXIST;   /* Almost certainly because we're trying to navigate above the root directory. */
    }

    if (pathCleanLen >= sizeof(pPathCleanStack)) {
        pPathCleanHeap = (char*)e_malloc(pathCleanLen + 1, e_fs_get_allocation_callbacks(pFS));
        if (pPathCleanHeap == NULL) {
            return E_OUT_OF_MEMORY;
        }

        e_path_normalize(pPathCleanHeap, pathCleanLen + 1, pPath, pathLen, E_NO_ABOVE_ROOT_NAVIGATION);    /* This will never fail. */
        pPathClean = pPathCleanHeap;
    } else {
        pPathClean = pPathCleanStack;
    }

    /* Now that the input path has been cleaned we need only append it to the base path. */
    pSubFS = (e_sub*)e_fs_get_backend_data(pFS);
    E_ASSERT(pSubFS != NULL);

    pSubFSPath->fullPathLen = e_path_append(pSubFSPath->pFullPathStack, sizeof(pSubFSPath->pFullPathStack), pSubFS->pRootDir, pSubFS->rootDirLen, pPathClean, pathCleanLen);
    if (pSubFSPath->fullPathLen < 0) {
        e_free(pPathCleanHeap, e_fs_get_allocation_callbacks(pFS));
        return E_ERROR;    /* Should never hit this, but leaving here for safety. */
    }

    if (pSubFSPath->fullPathLen >= (int)sizeof(pSubFSPath->pFullPathStack)) {
        pSubFSPath->pFullPathHeap = (char*)e_malloc(pSubFSPath->fullPathLen + 1, e_fs_get_allocation_callbacks(pFS));
        if (pSubFSPath->pFullPathHeap == NULL) {
            e_free(pPathCleanHeap, e_fs_get_allocation_callbacks(pFS));
            return E_OUT_OF_MEMORY;
        }

        e_path_append(pSubFSPath->pFullPathHeap, pSubFSPath->fullPathLen + 1, pSubFS->pRootDir, pSubFS->rootDirLen, pPathClean, pathCleanLen);    /* This will never fail. */
        pSubFSPath->pFullPath = pSubFSPath->pFullPathHeap;
    } else {
        pSubFSPath->pFullPath = pSubFSPath->pFullPathStack;
    }

    return E_SUCCESS;
}

static void e_sub_path_uninit(e_sub_path* pSubFSPath)
{
    if (pSubFSPath->pFullPathHeap != NULL) {
        e_free(pSubFSPath->pFullPathHeap, e_fs_get_allocation_callbacks(NULL));
    }

    E_ZERO_OBJECT(pSubFSPath);
}


static size_t e_alloc_size_sub(const void* pBackendConfig)
{
    e_sub_config* pSubFSConfig = (e_sub_config*)pBackendConfig;

    if (pSubFSConfig == NULL) {
        return 0;   /* The sub config must be specified. */
    }

    /* We include a copy of the path with the main allocation. */
    return sizeof(e_sub) + strlen(pSubFSConfig->pRootDir) + 1 + 1;   /* +1 for trailing slash and +1 for null terminator. */
}

static e_result e_init_sub(e_fs* pFS, const void* pBackendConfig, e_stream* pStream)
{
    e_sub_config* pSubFSConfig = (e_sub_config*)pBackendConfig;
    e_sub* pSubFS;

    E_ASSERT(pFS != NULL);
    E_UNUSED(pStream);

    if (pSubFSConfig == NULL) {
        return E_INVALID_ARGS; /* Must have a config. */
    }

    if (pSubFSConfig->pOwnerFS == NULL) {
        return E_INVALID_ARGS; /* Must have an owner FS. */
    }

    if (pSubFSConfig->pRootDir == NULL) {
        return E_INVALID_ARGS; /* Must have a root directory. */
    }

    pSubFS = (e_sub*)e_fs_get_backend_data(pFS);
    E_ASSERT(pFS != NULL);

    pSubFS->pOwnerFS   = pSubFSConfig->pOwnerFS;
    pSubFS->pRootDir   = (char*)(pSubFS + 1);
    pSubFS->rootDirLen = strlen(pSubFSConfig->pRootDir);

    e_strcpy(pSubFS->pRootDir, pSubFSConfig->pRootDir);

    /* Append a trailing slash if necessary. */
    if (pSubFS->pRootDir[pSubFS->rootDirLen - 1] != '/') {
        pSubFS->pRootDir[pSubFS->rootDirLen] = '/';
        pSubFS->pRootDir[pSubFS->rootDirLen + 1] = '\0';
        pSubFS->rootDirLen += 1;
    }

    return E_SUCCESS;
}

static void e_uninit_sub(e_fs* pFS)
{
    /* Nothing to do here. */
    E_UNUSED(pFS);
}

static e_result e_ioctl_sub(e_fs* pFS, int op, void* pArgs)
{
    e_sub* pSubFS;

    pSubFS = (e_sub*)e_fs_get_backend_data(pFS);
    E_ASSERT(pSubFS != NULL);

    return e_fs_ioctl(pSubFS->pOwnerFS, op, pArgs);
}

static e_result e_remove_sub(e_fs* pFS, const char* pFilePath)
{
    e_result result;
    e_sub* pSubFS;
    e_sub_path subPath;

    pSubFS = (e_sub*)e_fs_get_backend_data(pFS);
    E_ASSERT(pSubFS != NULL);

    result = e_sub_path_init(pFS, pFilePath, E_NULL_TERMINATED, &subPath);
    if (result != E_SUCCESS) {
        return result;
    }

    result = e_fs_remove(pSubFS->pOwnerFS, subPath.pFullPath);
    e_sub_path_uninit(&subPath);

    return result;
}

static e_result e_rename_sub(e_fs* pFS, const char* pOldName, const char* pNewName)
{
    e_result result;
    e_sub* pSubFS;
    e_sub_path subPathOld;
    e_sub_path subPathNew;

    pSubFS = (e_sub*)e_fs_get_backend_data(pFS);
    E_ASSERT(pSubFS != NULL);

    result = e_sub_path_init(pFS, pOldName, E_NULL_TERMINATED, &subPathOld);
    if (result != E_SUCCESS) {
        return result;
    }

    result = e_sub_path_init(pFS, pNewName, E_NULL_TERMINATED, &subPathNew);
    if (result != E_SUCCESS) {
        e_sub_path_uninit(&subPathOld);
        return result;
    }

    result = e_fs_rename(pSubFS->pOwnerFS, subPathOld.pFullPath, subPathNew.pFullPath);

    e_sub_path_uninit(&subPathOld);
    e_sub_path_uninit(&subPathNew);

    return result;
}

static e_result e_mkdir_sub(e_fs* pFS, const char* pPath)
{
    e_result result;
    e_sub* pSubFS;
    e_sub_path subPath;

    pSubFS = (e_sub*)e_fs_get_backend_data(pFS);
    E_ASSERT(pSubFS != NULL);

    result = e_sub_path_init(pFS, pPath, E_NULL_TERMINATED, &subPath);
    if (result != E_SUCCESS) {
        return result;
    }

    result = e_fs_mkdir(pSubFS->pOwnerFS, subPath.pFullPath, E_IGNORE_MOUNTS);
    e_sub_path_uninit(&subPath);

    return result;
}

static e_result e_info_sub(e_fs* pFS, const char* pPath, int openMode, e_file_info* pInfo)
{
    e_result result;
    e_sub* pSubFS;
    e_sub_path subPath;

    pSubFS = (e_sub*)e_fs_get_backend_data(pFS);
    E_ASSERT(pSubFS != NULL);

    result = e_sub_path_init(pFS, pPath, E_NULL_TERMINATED, &subPath);
    if (result != E_SUCCESS) {
        return result;
    }

    result = e_fs_info(pSubFS->pOwnerFS, subPath.pFullPath, openMode, pInfo);
    e_sub_path_uninit(&subPath);

    return result;
}

static size_t e_file_alloc_size_sub(e_fs* pFS)
{
    E_UNUSED(pFS);
    return sizeof(e_file_sub);
}

static e_result e_file_open_sub(e_fs* pFS, e_stream* pStream, const char* pFilePath, int openMode, e_file* pFile)
{
    e_result result;
    e_sub_path subPath;
    e_sub* pSubFS;
    e_file_sub* pSubFSFile;

    E_UNUSED(pStream);

    result = e_sub_path_init(pFS, pFilePath, E_NULL_TERMINATED, &subPath);
    if (result != E_SUCCESS) {
        return result;
    }

    pSubFS = (e_sub*)e_fs_get_backend_data(pFS);
    E_ASSERT(pSubFS != NULL);

    pSubFSFile = (e_file_sub*)e_file_get_backend_data(pFile);
    E_ASSERT(pSubFSFile != NULL);

    result = e_file_open(pSubFS->pOwnerFS, subPath.pFullPath, openMode, &pSubFSFile->pActualFile);
    e_sub_path_uninit(&subPath);

    return result;
}

static e_result e_file_open_handle_sub(e_fs* pFS, void* hBackendFile, e_file* pFile)
{
    e_sub* pSubFS;
    e_file_sub* pSubFSFile;

    pSubFS = (e_sub*)e_fs_get_backend_data(pFS);
    E_ASSERT(pSubFS != NULL);

    pSubFSFile = (e_file_sub*)e_file_get_backend_data(pFile);
    E_ASSERT(pSubFSFile != NULL);

    return e_file_open_from_handle(pSubFS->pOwnerFS, hBackendFile, &pSubFSFile->pActualFile);
}

static void e_file_close_sub(e_file* pFile)
{
    e_file_sub* pSubFSFile = (e_file_sub*)e_file_get_backend_data(pFile);
    E_ASSERT(pSubFSFile != NULL);

    e_file_close(pSubFSFile->pActualFile);
}

static e_result e_file_read_sub(e_file* pFile, void* pDst, size_t bytesToRead, size_t* pBytesRead)
{
    e_file_sub* pSubFSFile = (e_file_sub*)e_file_get_backend_data(pFile);
    E_ASSERT(pSubFSFile != NULL);

    return e_file_read(pSubFSFile->pActualFile, pDst, bytesToRead, pBytesRead);
}

static e_result e_file_write_sub(e_file* pFile, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten)
{
    e_file_sub* pSubFSFile = (e_file_sub*)e_file_get_backend_data(pFile);
    E_ASSERT(pSubFSFile != NULL);

    return e_file_write(pSubFSFile->pActualFile, pSrc, bytesToWrite, pBytesWritten);
}

static e_result e_file_seek_sub(e_file* pFile, e_int64 offset, e_seek_origin origin)
{
    e_file_sub* pSubFSFile = (e_file_sub*)e_file_get_backend_data(pFile);
    E_ASSERT(pSubFSFile != NULL);
    
    return e_file_seek(pSubFSFile->pActualFile, offset, origin);
}

static e_result e_file_tell_sub(e_file* pFile, e_int64* pCursor)
{
    e_file_sub* pSubFSFile = (e_file_sub*)e_file_get_backend_data(pFile);
    E_ASSERT(pSubFSFile != NULL);
    
    return e_file_tell(pSubFSFile->pActualFile, pCursor);
}

static e_result e_file_flush_sub(e_file* pFile)
{
    e_file_sub* pSubFSFile = (e_file_sub*)e_file_get_backend_data(pFile);
    E_ASSERT(pSubFSFile != NULL);
    
    return e_file_flush(pSubFSFile->pActualFile);
}

static e_result e_file_info_sub(e_file* pFile, e_file_info* pInfo)
{
    e_file_sub* pSubFSFile = (e_file_sub*)e_file_get_backend_data(pFile);
    E_ASSERT(pSubFSFile != NULL);
    
    return e_file_get_info(pSubFSFile->pActualFile, pInfo);
}

static e_result e_file_duplicate_sub(e_file* pFile, e_file* pDuplicatedFile)
{
    e_file_sub* pSubFSFile;
    e_file_sub* pSubFSFileDuplicated;

    pSubFSFile = (e_file_sub*)e_file_get_backend_data(pFile);
    E_ASSERT(pSubFSFile != NULL);

    pSubFSFileDuplicated = (e_file_sub*)e_file_get_backend_data(pDuplicatedFile);
    E_ASSERT(pSubFSFileDuplicated != NULL);
    
    return e_file_duplicate(pSubFSFile->pActualFile, &pSubFSFileDuplicated->pActualFile);
}

static e_fs_iterator* e_first_sub(e_fs* pFS, const char* pDirectoryPath, size_t directoryPathLen)
{
    e_result result;
    e_sub* pSubFS;
    e_sub_path subPath;
    e_fs_iterator* pIterator;

    pSubFS = (e_sub*)e_fs_get_backend_data(pFS);
    E_ASSERT(pSubFS != NULL);

    result = e_sub_path_init(pFS, pDirectoryPath, directoryPathLen, &subPath);
    if (result != E_SUCCESS) {
        return NULL;
    }

    pIterator = e_fs_first(pSubFS->pOwnerFS, subPath.pFullPath, subPath.fullPathLen);
    e_sub_path_uninit(&subPath);

    return pIterator;
}

static e_fs_iterator* e_next_sub(e_fs_iterator* pIterator)
{
    return e_fs_next(pIterator);
}

static void e_free_iterator_sub(e_fs_iterator* pIterator)
{
    e_fs_free_iterator(pIterator);
}

e_fs_backend e_sub_backend =
{
    e_alloc_size_sub,
    e_init_sub,
    e_uninit_sub,
    e_ioctl_sub,
    e_remove_sub,
    e_rename_sub,
    e_mkdir_sub,
    e_info_sub,
    e_file_alloc_size_sub,
    e_file_open_sub,
    e_file_open_handle_sub,
    e_file_close_sub,
    e_file_read_sub,
    e_file_write_sub,
    e_file_seek_sub,
    e_file_tell_sub,
    e_file_flush_sub,
    e_file_info_sub,
    e_file_duplicate_sub,
    e_first_sub,
    e_next_sub,
    e_free_iterator_sub
};
const e_fs_backend* E_FS_SUB = &e_sub_backend;
/* END e_fs_sub.c */



/* BEG e_log.c */
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
    result = e_mutex_init(&pLog->mutex, E_MUTEX_TYPE_PLAIN);
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

    e_mutex_destroy(&pLog->mutex);
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

    e_mutex_lock(&pLog->mutex);
    {
        result = e_log_register_callback_nolock(pLog, onLog, pUserData, pAllocationCallbacks);
    }
    e_mutex_unlock(&pLog->mutex);

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

    e_mutex_lock(&pLog->mutex);
    {
        size_t iLog;
        for (iLog = 0; iLog < pLog->callbackCount; iLog += 1) {
            if (pLog->pCallbacks[iLog].onLog) {
                pLog->pCallbacks[iLog].onLog(pLog->pCallbacks[iLog].pUserData, level, pMessage);
            }
        }
    }
    e_mutex_unlock(&pLog->mutex);

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
/* END e_log.c */



/* BEG e_script.h */
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

E_API e_result e_script_load_file(e_script* pScript, e_fs* pFS, const char* pFilePath, e_log* pLog)
{
    e_result result;
    e_file* pFile;

    result = e_file_open(pFS, pFilePath, E_READ, &pFile);
    if (result != E_SUCCESS) {
        return result;
    }

    result = e_script_load(pScript, e_file_get_stream(pFile), pFilePath, pLog);
    e_file_close(pFile);

    return result;
}
/* END e_script.h */



/* BEG e_config_file.c */
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

    result = e_file_open(pFS, pFilePath, E_READ, &pFile);
    if (result != E_SUCCESS) {
        e_log_postf(pLog, E_LOG_LEVEL_ERROR, "Could not open \"%s\". %s.", pFilePath, e_result_description(result));
    }

    result = e_config_file_load(pConfigFile, e_file_get_stream(pFile), pFilePath, pAllocationCallbacks, pLog);
    e_file_close(pFile);

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
/* END e_config_file.c */



/* BEG e_image.h */
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
    e_stream_seek(((e_stb_image_callback_data*)pUserData)->pStream, n, E_SEEK_CUR);
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

    result = e_file_open(pFS, pFilePath, E_READ, &pFile);
    if (result != E_SUCCESS) {
        return result;
    }

    result = e_load_image(pVTable, pUserData, &pFile->stream, pAllocationCallbacks, ppData, pSizeX, pSizeY, pFormat);
    e_file_close(pFile);

    if (result != E_SUCCESS) {
        return result;
    }

    return E_SUCCESS;
}
/* END e_image.h */



/* BEG e_font.c */
E_API e_font_config e_font_config_init(void)
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
    result = e_file_open(pConfig->pFS, pConfig->pFilePath, E_READ, &pFile);
    if (result != E_SUCCESS) {
        e_log_postf(pConfig->pLog, E_LOG_LEVEL_ERROR, "Failed to open font file '%s'. %s.", pConfig->pFilePath, e_result_description(result));
        return result;
    }

    result = e_file_get_info(pFile, &fileInfo);
    if (result != E_SUCCESS) {
        e_log_postf(pConfig->pLog, E_LOG_LEVEL_ERROR, "Failed to get info for font file '%s'. %s.", pConfig->pFilePath, e_result_description(result));
        e_file_close(pFile);
        return result;
    }

    pFont = (e_font*)e_malloc(sizeof(*pFont) + fileInfo.size, pAllocationCallbacks);
    if (pFont == NULL) {
        e_log_postf(pConfig->pLog, E_LOG_LEVEL_ERROR, "Failed to allocate memory for font file '%s'.", pConfig->pFilePath);
        e_file_close(pFile);
        return E_OUT_OF_MEMORY;
    }

    /* The font data is sitting at the end of the font structure. */
    pFont->pTTFData    = (unsigned char*)(pFont + 1);
    pFont->ttfDataSize = fileInfo.size;

    result = e_file_read(pFile, pFont->pTTFData, fileInfo.size, NULL);
    if (result != E_SUCCESS) {
        e_log_postf(pConfig->pLog, E_LOG_LEVEL_ERROR, "Failed to read font file '%s'. %s.", pConfig->pFilePath, e_result_description(result));
        e_file_close(pFile);
        e_free(pFont, pAllocationCallbacks);
        return result;
    }

    /* Now that we've got the data in memory we can close the file. */
    e_file_close(pFile);

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
/* END e_font.c */


/* BEG e_engine.c */
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
    fsConfig = e_fs_config_init(NULL, NULL, NULL);

    result = e_fs_init(&fsConfig, &pEngine->pFS);
    if (result != E_SUCCESS) {
        return result;
    }

    /* Now that our file system is set up we can load our config. */
    result = e_config_file_init(pAllocationCallbacks, &pEngine->configFile);
    if (result != E_SUCCESS) {
        e_fs_uninit(pEngine->pFS);
        return result;
    }

    /*
    We'll try loading a default config from the working directory. This is not a critical error if
    it fails, but we'll post a warning about it.
    */
    result = e_config_file_load_file(&pEngine->configFile, pEngine->pFS, pConfig->pConfigFilePath, pAllocationCallbacks, pEngine->pLog);
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

    return pEngine->pFS;
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
/* END e_engine.c */



/* BEG e_window.c */
E_API unsigned int e_window_buffer_stride(const e_window_buffer* pBuffer)
{
    if (pBuffer == NULL) {
        return 0;
    }

    return E_ROUND_UP_16(pBuffer->sizeX * 4);
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
/* END e_window.c */




/* BEG e_input.c */
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
/* END e_input.c */



/* BEG e_graphics.c */
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
/* END e_graphics.c */




/* BEG e_client.c */
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
/* END e_client.c */




/* BEG e_editor.c */
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
/* END e_editor.c */


#endif  /* e_c */
