/*
This is all experimental. Everything is subject to change. Feedback welcome on the API.

There's no real functionality going on here. You can create a window and that's basically it. Not
everything is properly integrated yet. Config files are not yet automatically loaded.

There is no high level API yet so there's a bit of manual work to do. The general idea is that you
have one "engine" to many "clients". The engine is just miscellaneous global stuff, whereas the
client would be the game, editing tools, etc. The client is where the window is created.

To enter the main loop, use `e_engine_run()`. Implement the `onStep()` callback to actually
do stuff. This is where you'd probably step your clients.
*/

#ifndef e_h
#define e_h

#define E_STRINGIFY(x)     #x
#define E_XSTRINGIFY(x)    E_STRINGIFY(x)

#define E_VERSION_MAJOR    0
#define E_VERSION_MINOR    0
#define E_VERSION_PATCH    1
#define E_VERSION_STRING   E_XSTRINGIFY(E_VERSION_MAJOR) "." E_XSTRINGIFY(E_VERSION_MINOR) "." E_XSTRINGIFY(E_VERSION_REVISION)

#include <stddef.h> /* size_t */
#include <stdarg.h> /* va_list */
#include <math.h>

#if defined(_WIN32)
    #define E_WIN32
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

#ifndef E_API
#ifdef __cplusplus
#define E_API extern "C"
#else
#define E_API extern
#endif
#endif

#ifdef _MSC_VER
    #define E_INLINE __forceinline
#elif defined(__GNUC__)
    /*
    I've had a bug report where GCC is emitting warnings about functions possibly not being inlineable. This warning happens when
    the __attribute__((always_inline)) attribute is defined without an "inline" statement. I think therefore there must be some
    case where "__inline__" is not always defined, thus the compiler emitting these warnings. When using -std=c89 or -ansi on the
    command line, we cannot use the "inline" keyword and instead need to use "__inline__". In an attempt to work around this issue
    I am using "__inline__" only when we're compiling in strict ANSI mode.
    */
    #if defined(__STRICT_ANSI__)
        #define E_GNUC_INLINE_HINT __inline__
    #else
        #define E_GNUC_INLINE_HINT inline
    #endif

    #if (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 2)) || defined(__clang__)
        #define E_INLINE E_GNUC_INLINE_HINT __attribute__((always_inline))
    #else
        #define E_INLINE E_GNUC_INLINE_HINT
    #endif
#elif defined(__WATCOMC__)
    #define E_INLINE __inline
#else
    #define E_INLINE
#endif

#if defined(__has_attribute)
    #if __has_attribute(format)
        #define E_ATTRIBUTE_FORMAT(fmt, va) __attribute__((format(printf, fmt, va)))
    #endif
#endif
#ifndef E_ATTRIBUTE_FORMAT
#define E_ATTRIBUTE_FORMAT(fmt, va)
#endif


#if defined(__LP64__) || defined(_WIN64) || (defined(__x86_64__) && !defined(__ILP32__)) || defined(_M_X64) || defined(__ia64) || defined(_M_IA64) || defined(__aarch64__) || defined(_M_ARM64) || defined(__powerpc64__)
    #define E_SIZEOF_PTR    8
#else
    #define E_SIZEOF_PTR    4
#endif

#if E_SIZEOF_PTR == 8
    #define E_64BIT
#else
    #define E_32BIT
#endif

#if defined(E_USE_STDINT)
    #include <stdint.h>
    typedef int8_t                  e_int8;
    typedef uint8_t                 e_uint8;
    typedef int16_t                 e_int16;
    typedef uint16_t                e_uint16;
    typedef int32_t                 e_int32;
    typedef uint32_t                e_uint32;
    typedef int64_t                 e_int64;
    typedef uint64_t                e_uint64;
#else
    typedef   signed char           e_int8;
    typedef unsigned char           e_uint8;
    typedef   signed short          e_int16;
    typedef unsigned short          e_uint16;
    typedef   signed int            e_int32;
    typedef unsigned int            e_uint32;
    #if defined(_MSC_VER) && !defined(__clang__)
        typedef   signed __int64    e_int64;
        typedef unsigned __int64    e_uint64;
    #else
        #if defined(__clang__) || (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)))
            #pragma GCC diagnostic push
            #pragma GCC diagnostic ignored "-Wlong-long"
            #if defined(__clang__)
                #pragma GCC diagnostic ignored "-Wc++11-long-long"
            #endif
        #endif
        typedef   signed long long  e_int64;
        typedef unsigned long long  e_uint64;
        #if defined(__clang__) || (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)))
            #pragma GCC diagnostic pop
        #endif
    #endif
#endif  /* MA_USE_STDINT */

#if E_SIZEOF_PTR == 8
    typedef unsigned long long e_uintptr;
    typedef long long          e_intptr;
#else
    typedef unsigned int       e_uintptr;
    typedef int                e_intptr;
#endif

typedef unsigned char e_bool8;
typedef unsigned int  e_bool32;
#define E_TRUE  1
#define E_FALSE 0

typedef void* e_handle;
typedef void* e_ptr;

#define E_INT64_MAX ((e_int64)(((e_uint64)0x7FFFFFFF << 32) | 0xFFFFFFFF))

typedef enum
{
    /* Engine-specific non-error codes. */
    E_HAS_MORE_OUTPUT = 102,    /* Some stream has more output data to be read, but there's not enough room in the output buffer. */
    E_NEEDS_MORE_INPUT = 100,   /* Some stream needs more input data before it can be processed. */

    /* General non-error codes. */
    E_DONE = 1,
    E_SUCCESS = 0,

    /* General error codes. */
    E_ERROR = -1,
    E_INVALID_ARGS = -2,
    E_INVALID_OPERATION = -3,
    E_OUT_OF_MEMORY = -4,
    E_OUT_OF_RANGE = -5,
    E_ACCESS_DENIED = -6,
    E_DOES_NOT_EXIST = -7,
    E_ALREADY_EXISTS = -8,
    E_TOO_MANY_OPEN_FILES = -9,
    E_INVALID_FILE = -10,
    E_TOO_BIG = -11,
    E_PATH_TOO_LONG = -12,
    E_NAME_TOO_LONG = -13,
    E_NOT_DIRECTORY = -14,
    E_IS_DIRECTORY = -15,
    E_DIRECTORY_NOT_EMPTY = -16,
    E_AT_END = -17,
    E_NO_SPACE = -18,
    E_BUSY = -19,
    E_IO_ERROR = -20,
    E_INTERRUPT = -21,
    E_UNAVAILABLE = -22,
    E_ALREADY_IN_USE = -23,
    E_BAD_ADDRESS = -24,
    E_BAD_SEEK = -25,
    E_BAD_PIPE = -26,
    E_DEADLOCK = -27,
    E_TOO_MANY_LINKS = -28,
    E_NOT_IMPLEMENTED = -29,
    E_NO_MESSAGE = -30,
    E_BAD_MESSAGE = -31,
    E_NO_DATA_AVAILABLE = -32,
    E_INVALID_DATA = -33,
    E_TIMEOUT = -34,
    E_NO_NETWORK = -35,
    E_NOT_UNIQUE = -36,
    E_NOT_SOCKET = -37,
    E_NO_ADDRESS = -38,
    E_BAD_PROTOCOL = -39,
    E_PROTOCOL_UNAVAILABLE = -40,
    E_PROTOCOL_NOT_SUPPORTED = -41,
    E_PROTOCOL_FAMILY_NOT_SUPPORTED = -42,
    E_ADDRESS_FAMILY_NOT_SUPPORTED = -43,
    E_SOCKET_NOT_SUPPORTED = -44,
    E_CONNECTION_RESET = -45,
    E_ALREADY_CONNECTED = -46,
    E_NOT_CONNECTED = -47,
    E_CONNECTION_REFUSED = -48,
    E_NO_HOST = -49,
    E_IN_PROGRESS = -50,
    E_CANCELLED = -51,
    E_MEMORY_ALREADY_MAPPED = -52,

    /* Engine-specific error codes. */
    E_CHECKSUM_MISMATCH = -100,
    E_BACKEND_DISABLED = -101
} e_result;

typedef enum
{
    E_GRAPHICS_BACKEND_UNKNOWN,
    E_GRAPHICS_BACKEND_OPENGL,
    E_GRAPHICS_BACKEND_VULKAN,
    E_GRAPHICS_BACKEND_CUSTOM,

    /* For counting the number of stock graphics backends. */
    E_GRAPHICS_BACKEND_COUNT,
} e_graphics_backend;

typedef enum
{
    E_FORMAT_UNKNOWN,
    E_FORMAT_R32_FLOAT,
    E_FORMAT_R32G32_FLOAT,
    E_FORMAT_R32G32B32_FLOAT,
    E_FORMAT_R32G32B32A32_FLOAT,
    E_FORMAT_R8_UNORM,
    E_FORMAT_R8G8_UNORM,
    E_FORMAT_R8G8B8_UNORM,
    E_FORMAT_R8G8B8A8_UNORM,
    E_FORMAT_D24_UNORM_S8_UINT
} e_format;


typedef struct
{
    int left;
    int top;
    int right;
    int bottom;
} e_recti;

typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} e_color;

static E_INLINE e_color e_rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    e_color c;
    c.r = r;
    c.g = g;
    c.b = b;
    c.a = a;
    return c;
}

static E_INLINE e_uint32 e_color_to_uint32(e_color c)
{
    return ((unsigned int)c.a << 24) | ((unsigned int)c.b << 16) | ((unsigned int)c.g << 8) | (unsigned int)c.r;
}



E_API const char* e_result_description(e_result result);


/* dlopen, etc. with e_handle as the library handle. */

/*
e_proc is annoying because when compiling with GCC we get pendantic warnings about converting
between `void*` and `void (*)()`. We can't use `void (*)()` with MSVC however, because we'll get
warning C4191 about "type cast between incompatible function types". To work around this I'm going
to use a different data type depending on the compiler.
*/
#if defined(__GNUC__)
typedef void (* e_proc)(void);
#else
typedef void* e_proc;
#endif

E_API e_handle e_dlopen(const char* pFilePath);
E_API void     e_dlclose(e_handle hLibrary);
E_API e_proc   e_dlsym(e_handle hLibrary, const char* pSymbol);
E_API e_result e_dlerror(char* pOutMessage, size_t messageSizeInBytes);


/* BEG e_allocation_callbacks.h */
typedef struct e_allocation_callbacks
{
    void* pUserData;
    void* (* onMalloc )(size_t sz, void* pUserData);
    void* (* onRealloc)(void* p, size_t sz, void* pUserData);
    void  (* onFree   )(void* p, void* pUserData);
} e_allocation_callbacks;

E_API void* e_malloc(size_t sz, const e_allocation_callbacks* pAllocationCallbacks);
E_API void* e_calloc(size_t sz, const e_allocation_callbacks* pAllocationCallbacks);
E_API void* e_realloc(void* p, size_t sz, const e_allocation_callbacks* pAllocationCallbacks);
E_API void  e_free(void* p, const e_allocation_callbacks* pAllocationCallbacks);
/* END e_allocation_callbacks.h */


/* BEG e_misc.h */
E_API void e_sort(void* pList, size_t count, size_t stride, int (*compareProc)(void*, const void*, const void*), void* pUserData);

E_API void* e_binary_search(const void* pKey, const void* pList, size_t count, size_t stride, int (*compareProc)(void*, const void*, const void*), void* pUserData);
E_API void* e_linear_search(const void* pKey, const void* pList, size_t count, size_t stride, int (*compareProc)(void*, const void*, const void*), void* pUserData);
E_API void* e_sorted_search(const void* pKey, const void* pList, size_t count, size_t stride, int (*compareProc)(void*, const void*, const void*), void* pUserData);
/* END e_misc.h */



/* BEG e_timer.h */
typedef struct
{
    e_int64 counter;
    double counterD;
} e_timer;

E_API void e_timer_init(e_timer* pTimer);
E_API double e_timer_get_time_in_seconds(e_timer* pTimer);
/* END e_timer.h */



/* BEG e_net.h */
/*
The low-level socket library should be a direct mapping to the underlying socket API. The problem
child here is Windows due to annyoing compilation errors when including winsock2.h and windows.h,
and the need for linking to to the ws2_32 library.

We're going to namespace our socket library as "e_net". On Unix this is just a direct mapping to
the standard socket API and structures. On Windows, we'll redeclare our own versions of the
necessary structures and link to the ws2_32 library at runtime in e_net_init(). The e_net_socket()
will be declared as extern here, and set to the appropriate value in e_net_init().
*/
#if defined(_WIN32)
typedef e_uintptr E_SOCKET;

struct e_timeval    /* Might be able to move this out of the network section. */
{
    long tv_sec;
    long tv_usec;
};

struct e_addrinfo
{
    e_int32 ai_flags;
    e_int32 ai_family;
    e_int32 ai_socktype;
    e_int32 ai_protocol;
    size_t ai_addrlen;
    char *ai_canonname;
    struct e_sockaddr *ai_addr;
    struct e_addrinfo *ai_next;
};

struct e_sockaddr
{
    e_uint16 sa_family;
    char sa_data[14];
};

struct e_in_addr
{
    union
    {
        struct { e_uint8 s_b1, s_b2, s_b3, s_b4; } un_b;
        struct { e_uint16 s_w1, s_w2; } un_w;
        e_uint32 addr;
    } un;
};

struct e_sockaddr_in
{
    e_uint16 sin_family;
    e_uint16 sin_port;
    struct e_in_addr sin_addr;
    char sin_zero[8];
};

struct e_in6_addr
{
    union
    {
        e_uint8  u6_addr8[16];
        e_uint16 u6_addr16[8];
        e_uint32 u6_addr32[4];
    } u6_addr;
};

struct e_sockaddr_in6
{
    e_uint16 sin6_family;
    e_uint16 sin6_port;
    e_uint32 sin6_flowinfo;
    struct e_in6_addr sin6_addr;
    e_uint32 sin6_scope_id;
};

typedef struct
{
    unsigned int fd_count;
    E_SOCKET fd_array[64];
} e_fd_set;

#define E_WSAAPI            __stdcall
#define E_INVALID_SOCKET    ((E_SOCKET)(~0))

#define E_AF_UNSPEC         0
#define E_AF_UNIX           1
#define E_AF_INET           2
#define E_AF_INET6          23

#define E_SOCK_STREAM       1
#define E_SOCK_DGRAM        2
#define E_SOCK_RAW          3
#define E_SOCK_RDM          4
#define E_SOCK_SEQPACKET    5

typedef int                 (E_WSAAPI * e_pfn_socket)(int af, int type, int protocol);
typedef int                 (E_WSAAPI * e_pfn_closesocket)(E_SOCKET s);
typedef int                 (E_WSAAPI * e_pfn_shutdown)(E_SOCKET s, int how);
typedef int                 (E_WSAAPI * e_pfn_bind)(E_SOCKET s, const struct e_sockaddr* name, int namelen);
typedef int                 (E_WSAAPI * e_pfn_listen)(E_SOCKET s, int backlog);
typedef E_SOCKET            (E_WSAAPI * e_pfn_accept)(E_SOCKET s, struct e_sockaddr* addr, int* addrlen);
typedef int                 (E_WSAAPI * e_pfn_connect)(E_SOCKET s, const struct e_sockaddr* name, int namelen);
typedef int                 (E_WSAAPI * e_pfn_send)(E_SOCKET s, const char* buf, int len, int flags);
typedef int                 (E_WSAAPI * e_pfn_recv)(E_SOCKET s, char* buf, int len, int flags);
typedef int                 (E_WSAAPI * e_pfn_ioctlsocket)(E_SOCKET s, long cmd, unsigned long* argp);
typedef int                 (E_WSAAPI * e_pfn_select)(int nfds, e_fd_set* readfds, e_fd_set* writefds, e_fd_set* exceptfds, const struct e_timeval* timeout);
typedef int                 (E_WSAAPI * e_pfn_sendto)(E_SOCKET s, const char* buf, int len, int flags, const struct e_sockaddr* to, int tolen);
typedef int                 (E_WSAAPI * e_pfn_recvfrom)(E_SOCKET s, char* buf, int len, int flags, struct e_sockaddr* from, int* fromlen);
typedef int                 (E_WSAAPI * e_pfn_gethostname)(char* name, int namelen);
typedef int                 (E_WSAAPI * e_pfn_getnameinfo)(const struct e_sockaddr* sa, e_uint32 salen, char* host, e_uint32 hostlen, char* serv, e_uint32 servlen, int flags);
typedef int                 (E_WSAAPI * e_pfn_inet_pton)(int af, const char* src, void* dst);
typedef const char*         (E_WSAAPI * e_pfn_inet_ntop)(int af, const void* src, char* dst, e_uint32 size);
typedef unsigned short      (E_WSAAPI * e_pfn_htons)(unsigned short hostshort);
typedef unsigned short      (E_WSAAPI * e_pfn_ntohs)(unsigned short netshort);
typedef unsigned long       (E_WSAAPI * e_pfn_htonl)(unsigned long hostlong);
typedef unsigned long       (E_WSAAPI * e_pfn_ntohl)(unsigned long netlong);
typedef int                 (E_WSAAPI * e_pfn_getaddrinfo)(const char* node, const char* service, const struct e_addrinfo* hints, struct e_addrinfo** res);
typedef void                (E_WSAAPI * e_pfn_freeaddrinfo)(struct e_addrinfo* ai);

extern e_pfn_socket         e_net_socket;
extern e_pfn_closesocket    e_net_closesocket;
extern e_pfn_shutdown       e_net_shutdown;
extern e_pfn_bind           e_net_bind;
extern e_pfn_listen         e_net_listen;
extern e_pfn_accept         e_net_accept;
extern e_pfn_connect        e_net_connect;
extern e_pfn_send           e_net_send;
extern e_pfn_recv           e_net_recv;
extern e_pfn_ioctlsocket    e_net_ioctlsocket;
extern e_pfn_select         e_net_select;
extern e_pfn_sendto         e_net_sendto;
extern e_pfn_recvfrom       e_net_recvfrom;
extern e_pfn_gethostname    e_net_gethostname;
extern e_pfn_getnameinfo    e_net_getnameinfo;
extern e_pfn_inet_pton      e_net_inet_pton;
extern e_pfn_inet_ntop      e_net_inet_ntop;
extern e_pfn_htons          e_net_htons;
extern e_pfn_ntohs          e_net_ntohs;
extern e_pfn_htonl          e_net_htonl;
extern e_pfn_ntohl          e_net_ntohl;
extern e_pfn_getaddrinfo    e_net_getaddrinfo;
extern e_pfn_freeaddrinfo   e_net_freeaddrinfo;
#else
#include <sys/socket.h>
#include <netinet/in.h> /* sockaddr_in */
#include <arpa/inet.h>  /* inet_pton */
#include <netdb.h>      /* getaddrinfo, freeaddrinfo */

#define e_addrinfo          addrinfo
#define e_sockaddr          sockaddr
#define e_sockaddr_in       sockaddr_in
#define e_sockaddr_in6      sockaddr_in6
#define e_fd_set            fd_set

typedef int                 E_SOCKET;
#define E_INVALID_SOCKET    INVALID_SOCKET

#define E_AF_UNSPEC         AF_UNSPEC
#define E_AF_UNIX           AF_UNIX
#define E_AF_INET           AF_INET
#define E_AF_INET6          AF_INET6

#define E_SOCK_STREAM       SOCK_STREAM
#define E_SOCK_DGRAM        SOCK_DGRAM
#define E_SOCK_RAW          SOCK_RAW
#define E_SOCK_RDM          SOCK_RDM
#define E_SOCK_SEQPACKET    SOCK_SEQPACKET

#define e_net_socket        socket
#define e_net_closesocket   closesocket
#define e_net_shutdown      shutdown
#define e_net_bind          bind
#define e_net_listen        listen
#define e_net_accept        accept
#define e_net_connect       connect
#define e_net_send          send
#define e_net_recv          recv
#define e_net_ioctlsocket   ioctlsocket
#define e_net_select        select
#define e_net_sendto        sendto
#define e_net_recvfrom      recvfrom
#define e_net_gethostname   gethostname
#define e_net_getnameinfo   getnameinfo
#define e_net_inet_pton     inet_pton
#define e_net_inet_ntop     inet_ntop
#define e_net_htons         htons
#define e_net_ntohs         ntohs
#define e_net_htonl         htonl
#define e_net_ntohl         ntohl
#define e_net_getaddrinfo   getaddrinfo
#define e_net_freeaddrinfo  freeaddrinfo
#endif

#define E_SOCKET_ERROR      (-1)
#define E_AI_PASSIVE        0x00000001

E_API e_result e_net_init(void);
E_API void e_net_uninit(void);
E_API int e_net_get_last_error(void);
E_API int e_net_set_non_blocking(E_SOCKET socket, e_bool32 blocking);
/* END e_net.h */



/* BEG e_threading_header.h */
#if defined(E_POSIX)
    #ifndef E_USE_PTHREAD
    #define E_USE_PTHREAD
    #endif

    /*
    This is, hopefully, a temporary measure to get compilation working with the -std=c89 switch on
    GCC and Clang. Unfortunately without this we get errors about the following functions not being
    declared:

        pthread_mutexattr_settype()

    I am not sure yet how a fallback would work for pthread_mutexattr_settype(). It may just be
    that it's fundamentally not compatible without explicit pthread support which would make the
    _XOPEN_SOURCE define mandatory. Needs further investigation.

    In addition, pthread_mutex_timedlock() is only available since 2001 which is only enabled if
    _XOPEN_SOURCE is defined to something >= 600. If this is not the case, a suboptimal fallback
    will be used instead which calls pthread_mutex_trylock() in a loop, with a sleep after each
    loop iteration. By setting _XOPEN_SOURCE here we reduce the likelyhood of users accidentally
    falling back to the suboptimal fallback.

    I'm setting this to the latest version here (700) just in case this file is included at the top
    of a source file which later on depends on some POSIX functions from later revisions.
    */
    #ifndef _XOPEN_SOURCE
    #define _XOPEN_SOURCE   700
    #else
        #if _XOPEN_SOURCE < 500
        #error _XOPEN_SOURCE must be >= 500. c89thread is not usable.
        #endif
    #endif

    #ifndef E_NO_PTHREAD_IN_HEADER
        #include <pthread.h>
        typedef pthread_t           e_pthread_t;
        typedef pthread_mutex_t     e_pthread_mutex_t;
        typedef pthread_cond_t      e_pthread_cond_t;
    #else
        typedef e_uintptr           e_pthread_t;
        typedef union               e_pthread_mutex_t { char __data[40]; e_uint64 __alignment; } e_pthread_mutex_t;
        typedef union               e_pthread_cond_t  { char __data[48]; e_uint64 __alignment; } e_pthread_cond_t;
    #endif
#endif

#include <time.h>   /* For timespec. */

#ifndef TIME_UTC
#define TIME_UTC    1
#endif

#if (defined(_MSC_VER) && _MSC_VER < 1900) || defined(__DMC__)  /* 1900 = Visual Studio 2015 */
struct timespec
{
    time_t tv_sec;
    long tv_nsec;
};
#endif
/* END e_threading_header.h */


/* BEG e_thread.h */
/* e_thread */
#if defined(E_WIN32)
typedef e_handle    e_thread;  /* HANDLE, CreateThread() */
#else
typedef e_pthread_t e_thread;
#endif

typedef int (* e_thread_start_callback)(void*);

typedef struct
{
    void* pUserData;
    void (* onEntry)(void* pUserData);
    void (* onExit)(void* pUserData);
} e_entry_exit_callbacks;

E_API e_result e_thread_create_ex(e_thread* thr, e_thread_start_callback func, void* arg, const e_entry_exit_callbacks* pEntryExitCallbacks, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_result e_thread_create(e_thread* thr, e_thread_start_callback func, void* arg);
E_API e_bool32 e_thread_equal(e_thread lhs, e_thread rhs);
E_API e_thread e_thread_current(void);
E_API e_result e_thread_sleep(const struct timespec* duration, struct timespec* remaining);
E_API void e_thread_yield(void);
E_API void e_thread_exit(int res);
E_API e_result e_thread_detach(e_thread thr);
E_API e_result e_thread_join(e_thread thr, int* res);


/* e_mutex */
#if defined(E_WIN32)
typedef struct
{
    e_handle handle;    /* HANDLE, CreateMutex(), CreateEvent() */
    int type;
} e_mutex;
#else
typedef e_pthread_mutex_t e_mutex;
#endif

enum
{
    E_MUTEX_TYPE_PLAIN     = 0x00000000,
    E_MUTEX_TYPE_TIMED     = 0x00000001,
    E_MUTEX_TYPE_RECURSIVE = 0x00000002
};

E_API e_result e_mutex_init(e_mutex* mutex, int type);
E_API void e_mutex_destroy(e_mutex* mutex);
E_API e_result e_mutex_lock(e_mutex* mutex);
E_API e_result e_mutex_timedlock(e_mutex* mutex, const struct timespec* time_point);
E_API e_result e_mutex_trylock(e_mutex* mutex);
E_API e_result e_mutex_unlock(e_mutex* mutex);


/* e_cond */
#if defined(E_WIN32)
/* Not implemented. */
typedef void*            e_cond;
#else
typedef e_pthread_cond_t e_cond;
#endif

E_API e_result e_cond_init(e_cond* cnd);
E_API void e_cond_destroy(e_cond* cnd);
E_API e_result e_cond_signal(e_cond* cnd);
E_API e_result e_cond_broadcast(e_cond* cnd);
E_API e_result e_cond_wait(e_cond* cnd, e_mutex* mtx);
E_API e_result e_cond_timedwait(e_cond* cnd, e_mutex* mtx, const struct timespec* time_point);


/* e_semaphore */
#if defined(E_WIN32)
typedef e_handle e_semaphore;
#else
typedef struct
{
    int value;
    int valueMax;
    e_pthread_mutex_t lock;
    e_pthread_cond_t cond;
} e_semaphore;
#endif

E_API e_result e_semaphore_init(e_semaphore* sem, int value, int valueMax);
E_API void e_semaphore_destroy(e_semaphore* sem);
E_API e_result e_semaphore_wait(e_semaphore* sem);
E_API e_result e_semaphore_timedwait(e_semaphore* sem, const struct timespec* time_point);
E_API e_result e_semaphore_post(e_semaphore* sem);


/* e_syncevent */
#if defined(E_WIN32)
typedef e_handle e_syncevent;
#else
typedef struct
{
    int value;
    e_pthread_mutex_t lock;
    e_pthread_cond_t cond;
} e_syncevent;
#endif

E_API e_result e_syncevent_init(e_syncevent* evnt);
E_API void e_syncevent_destroy(e_syncevent* evnt);
E_API e_result e_syncevent_wait(e_syncevent* evnt);
E_API e_result e_syncevent_timedwait(e_syncevent* evnt, const struct timespec* time_point);
E_API e_result e_syncevent_signal(e_syncevent* evnt);
/* END e_thread.h */



/* BEG e_threading.h */
#if 0
typedef int (* e_thread_start_callback)(void* arg);

typedef struct e_thread_config e_thread_config;
typedef struct e_thread        e_thread;

struct e_thread_config
{
    e_thread_start_callback func;
    void* arg;
};

E_API e_thread_config e_thread_config_init(e_thread_start_callback func, void* arg);

E_API e_result e_thread_init(const e_thread_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_thread** ppThread);
E_API e_result e_thread_join(e_thread* pThread, int* pExitCode);


typedef struct e_mutex e_mutex;

E_API size_t e_mutex_alloc_size(void);
E_API e_result e_mutex_init_preallocated(e_mutex* pMutex);
E_API e_result e_mutex_init(const e_allocation_callbacks* pAllocationCallbacks, e_mutex** ppMutex);
E_API void e_mutex_uninit(e_mutex* pMutex, const e_allocation_callbacks* pAllocationCallbacks);
E_API void e_mutex_lock(e_mutex* pMutex);
E_API void e_mutex_unlock(e_mutex* pMutex);
#endif
/* END e_threading.h */



/* BEG e_stream.h */
/*
Streams.

The feeding of input and output data is done via a stream.

To implement a custom stream, such as a memory stream, or a file stream, you need to extend from
`e_stream` and implement `e_stream_vtable`. You can access your custom data by casting the
`e_stream` to your custom type.

The stream vtable can support both reading and writing, but it doesn't need to support both at
the same time. If one is not supported, simply leave the relevant `read` or `write` callback as
`NULL`, or have them return E_NOT_IMPLEMENTED.
*/
typedef enum e_seek_origin
{
    E_SEEK_SET = 0,
    E_SEEK_CUR = 1,
    E_SEEK_END = 2
} e_seek_origin;

typedef struct e_stream_vtable e_stream_vtable;
typedef struct e_stream        e_stream;

struct e_stream_vtable
{
    e_result (* read                )(e_stream* pStream, void* pDst, size_t bytesToRead, size_t* pBytesRead);
    e_result (* write               )(e_stream* pStream, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten);
    e_result (* seek                )(e_stream* pStream, e_int64 offset, e_seek_origin origin);
    e_result (* tell                )(e_stream* pStream, e_int64* pCursor);
    size_t   (* duplicate_alloc_size)(e_stream* pStream);                                 /* Optional. Returns the allocation size of the stream. When not defined, duplicating is disabled. */
    e_result (* duplicate           )(e_stream* pStream, e_stream* pDuplicatedStream);    /* Optional. Duplicate the stream. */
    void     (* uninit              )(e_stream* pStream);                                 /* Optional. Uninitialize the stream. */
};

struct e_stream
{
    const e_stream_vtable* pVTable;
};

E_API e_result e_stream_init(const e_stream_vtable* pVTable, e_stream* pStream);
E_API e_result e_stream_read(e_stream* pStream, void* pDst, size_t bytesToRead, size_t* pBytesRead);
E_API e_result e_stream_write(e_stream* pStream, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten);
E_API e_result e_stream_seek(e_stream* pStream, e_int64 offset, e_seek_origin origin);
E_API e_result e_stream_tell(e_stream* pStream, e_int64* pCursor);
E_API e_result e_stream_writef(e_stream* pStream, const char* fmt, ...) E_ATTRIBUTE_FORMAT(2, 3);
E_API e_result e_stream_writef_ex(e_stream* pStream, const e_allocation_callbacks* pAllocationCallbacks, const char* fmt, ...) E_ATTRIBUTE_FORMAT(3, 4);
E_API e_result e_stream_writefv(e_stream* pStream, const char* fmt, va_list args);
E_API e_result e_stream_writefv_ex(e_stream* pStream, const e_allocation_callbacks* pAllocationCallbacks, const char* fmt, va_list args);

/*
Duplicates a stream.

This will allocate the new stream on the heap. The caller is responsible for freeing the stream
with `e_stream_delete_duplicate()` when it's no longer needed.
*/
E_API e_result e_stream_duplicate(e_stream* pStream, const e_allocation_callbacks* pAllocationCallbacks, e_stream** ppDuplicatedStream);

/*
Deletes a duplicated stream.

Do not use this for a stream that was not duplicated with `e_stream_duplicate()`.
*/
E_API void e_stream_delete_duplicate(e_stream* pDuplicatedStream, const e_allocation_callbacks* pAllocationCallbacks);

/*
Helper functions for reading the entire contents of a stream, starting from the current cursor position. Free
the returned pointer with e_free().

The format (E_STREAM_DATA_FORMAT_TEXT or E_STREAM_DATA_FORMAT_BINARY) is used to determine whether or not a null terminator should be
appended to the end of the data.

For flexiblity in case the backend does not support cursor retrieval or positioning, the data will be read
in fixed sized chunks.
*/
typedef enum e_stream_data_format
{
    E_STREAM_DATA_FORMAT_TEXT,
    E_STREAM_DATA_FORMAT_BINARY
} e_stream_data_format;

E_API e_result e_stream_read_to_end(e_stream* pStream, e_stream_data_format format, const e_allocation_callbacks* pAllocationCallbacks, void** ppData, size_t* pDataSize);
/* END e_stream.h */

/* BEG e_memory_stream.h */
/*
Memory streams support both reading and writing within the same stream. To only support read-only
mode, use e_memory_stream_init_readonly(). With this you can pass in a standard data/size pair.

If you need writing support, use e_memory_stream_init_write(). When writing data, the stream will
output to a buffer that is owned by the stream. When you need to access the data, do so by
inspecting the pointer directly with `stream.write.pData` and `stream.write.dataSize`. This mode
also supports reading.

You can overwrite data by seeking to the required location and then just writing like normal. To
append data, just seek to the end:

    e_memory_stream_seek(pStream, 0, E_SEEK_END);

The memory stream need not be uninitialized in read-only mode. In write mode you can use
`e_memory_stream_uninit()` to free the data. Alternatively you can just take ownership of the
buffer and free it yourself with `e_free()`.

Below is an example for write mode.

    ```c
    e_memory_stream stream;
    e_memory_stream_init_write(NULL, &stream);
    
    // Write some data to the stream...
    e_memory_stream_write(&stream, pSomeData, someDataSize, NULL);
    
    // Do something with the data.
    do_something_with_my_data(stream.write.pData, stream.write.dataSize);
    ```

To free the data, you can use `e_memory_stream_uninit()`, or you can take ownership of the data
and free it yourself with `e_free()`:

    ```c
    e_memory_stream_uninit(&stream);
    ```

Or to take ownership:

    ```c
    size_t dataSize;
    void* pData = e_memory_stream_take_ownership(&stream, &dataSize);
    ```

With the above, `pData` will be the pointer to the data and `dataSize` will be the size of the data
and you will be responsible for deleting the buffer with `e_free()`.


Read mode is simpler:

    ```c
    e_memory_stream stream;
    e_memory_stream_init_readonly(pData, dataSize, &stream);

    // Read some data.
    e_memory_stream_read(&stream, &myBuffer, bytesToRead, NULL);
    ```

There is only one cursor. As you read and write the cursor will move forward. If you need to
read and write from different locations from the same e_memory_stream object, you need to
seek before doing your read or write. You cannot read and write at the same time across
multiple threads for the same e_memory_stream object.
*/
typedef struct e_memory_stream e_memory_stream;

struct e_memory_stream
{
    e_stream base;
    void** ppData;      /* Will be set to &readonly.pData in readonly mode. */
    size_t* pDataSize;  /* Will be set to &readonly.dataSize in readonly mode. */
    struct
    {
        const void* pData;
        size_t dataSize;
    } readonly;
    struct
    {
        void* pData;        /* Will only be set in write mode. */
        size_t dataSize;
        size_t dataCap;
    } write;
    size_t cursor;
    e_allocation_callbacks allocationCallbacks; /* This is copied from the allocation callbacks passed in from e_memory_stream_init(). Only used in write mode. */
};

E_API e_result e_memory_stream_init_write(const e_allocation_callbacks* pAllocationCallbacks, e_memory_stream* pStream);
E_API e_result e_memory_stream_init_readonly(const void* pData, size_t dataSize, e_memory_stream* pStream);
E_API void e_memory_stream_uninit(e_memory_stream* pStream);    /* Only needed for write mode. This will free the internal pointer so make sure you've done what you need to do with it. */
E_API e_result e_memory_stream_read(e_memory_stream* pStream, void* pDst, size_t bytesToRead, size_t* pBytesRead);
E_API e_result e_memory_stream_write(e_memory_stream* pStream, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten);
E_API e_result e_memory_stream_seek(e_memory_stream* pStream, e_int64 offset, int origin);
E_API e_result e_memory_stream_tell(e_memory_stream* pStream, size_t* pCursor);
E_API e_result e_memory_stream_remove(e_memory_stream* pStream, size_t offset, size_t size);
E_API e_result e_memory_stream_truncate(e_memory_stream* pStream);
E_API void* e_memory_stream_take_ownership(e_memory_stream* pStream, size_t* pSize);  /* Takes ownership of the buffer. The caller is responsible for freeing the buffer with e_free(). Only valid in write mode. */
/* END e_memory_stream.h */



/* BEG e_deflate.h */
enum
{
    E_DEFLATE_FLAG_PARSE_ZLIB_HEADER = 1,
    E_DEFLATE_FLAG_HAS_MORE_INPUT = 2,
    E_DEFLATE_FLAG_USING_NON_WRAPPING_OUTPUT_BUF = 4,
    E_DEFLATE_FLAG_COMPUTE_ADLER32 = 8
};

enum
{
    E_DEFLATE_MAX_HUFF_TABLES    = 3,
    E_DEFLATE_MAX_HUFF_SYMBOLS_0 = 288,
    E_DEFLATE_MAX_HUFF_SYMBOLS_1 = 32,
    E_DEFLATE_MAX_HUFF_SYMBOLS_2 = 19,
    E_DEFLATE_FAST_LOOKUP_BITS   = 10,
    E_DEFLATE_FAST_LOOKUP_SIZE   = 1 << E_DEFLATE_FAST_LOOKUP_BITS
};

typedef struct
{
    e_uint8 codeSize[E_DEFLATE_MAX_HUFF_SYMBOLS_0];
    e_int16 lookup[E_DEFLATE_FAST_LOOKUP_SIZE];
    e_int16 tree[E_DEFLATE_MAX_HUFF_SYMBOLS_0 * 2];
} e_deflate_huff_table;

#ifdef E_64BIT
    typedef e_uint64 e_deflate_bitBufferfer;
#else
    typedef e_uint32 e_deflate_bitBufferfer;
#endif

typedef struct e_deflate_decompressor
{
    e_uint32 state;
    e_uint32 bitCount;
    e_uint32 zhdr0;
    e_uint32 zhdr1;
    e_uint32 zAdler32;
    e_uint32 final;
    e_uint32 type;
    e_uint32 checkAdler32;
    e_uint32 dist;
    e_uint32 counter;
    e_uint32 extraCount;
    e_uint32 tableSizes[E_DEFLATE_MAX_HUFF_TABLES];
    e_deflate_bitBufferfer bitBuffer;
    size_t distFromOutBufStart;
    e_deflate_huff_table tables[E_DEFLATE_MAX_HUFF_TABLES];
    e_uint8 rawHeader[4];
    e_uint8 lenCodes[E_DEFLATE_MAX_HUFF_SYMBOLS_0 + E_DEFLATE_MAX_HUFF_SYMBOLS_1 + 137];
} e_deflate_decompressor;

E_API e_result e_deflate_decompressor_init(e_deflate_decompressor* pDecompressor);
E_API e_result e_deflate_decompress(e_deflate_decompressor* pDecompressor, const e_uint8* pInputBuffer, size_t* pInputBufferSize, e_uint8* pOutputBufferStart, e_uint8* pOutputBufferNext, size_t* pOutputBufferSize, e_uint32 flags);
/* END e_deflate.h */





/* BEG e_fs.h */
typedef struct e_fs_vtable         e_fs_vtable;
typedef struct e_fs_config         e_fs_config;
typedef struct e_fs                e_fs;
typedef struct e_fs_iterator       e_fs_iterator;       /* For iterating over files in a directory (not recursive). File system's must extend from this struct. */
typedef struct e_file              e_file;
typedef struct e_file_info         e_file_info;

typedef struct e_archive_vtable    e_archive_vtable;
typedef struct e_archive           e_archive;
typedef struct e_archive_extension e_archive_extension; /* For internal use only. Used for mapping extensions to archive types. */

typedef enum
{
    E_OPEN_MODE_READ     = 0x01,
    E_OPEN_MODE_WRITE    = 0x02,
    E_OPEN_MODE_APPEND   = 0x04,    /* Used with E_OPEN_MODE_WRITE. If specified, writing will append to the end. If E_OPEN_MODE_WRITE is not specified, this is ignored. */
    E_OPEN_MODE_TRUNCATE = 0x08,    /* Used with E_OPEN_MODE_WRITE. If specified, and the file exists, the file will be truncated to length 0. Cannot be used with E_OPEN_MODE_APPEND. */
} e_open_mode;

struct e_file_info
{
    e_uint64 size;
    e_uint64 lastModifiedTime;
    e_uint64 lastAccessTime;
    e_bool32 directory;
};

struct e_fs_iterator
{
    e_fs* pFS;
    const e_fs_vtable* pFSVTable;   /* In case pFS is null. */
    void* pFSVTableUserData;        /* In case pFS is null. */
    const char* pName;              /* Must be null terminated. The FS implementation is responsible for manageing the memory allocation, but it would normally be allocated at the end of the struct. */
    size_t nameLen;
    e_file_info info;
};

struct e_fs_vtable
{
    e_result       (* file_alloc_size)(void* pUserData, size_t* pSize);
    e_result       (* open           )(void* pUserData, e_fs* pFS, const char* pFilePath, int openMode, const e_allocation_callbacks* pAllocationCallbacks, e_file* pFile);
    void           (* close          )(void* pUserData, e_file* pFile, const e_allocation_callbacks* pAllocationCallbacks);
    e_result       (* read           )(void* pUserData, e_file* pFile, void* pDst, size_t bytesToRead, size_t* pBytesRead);
    e_result       (* write          )(void* pUserData, e_file* pFile, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten);
    e_result       (* seek           )(void* pUserData, e_file* pFile, e_int64 offset, e_seek_origin origin);
    e_result       (* tell           )(void* pUserData, e_file* pFile, e_int64* pCursor);
    e_result       (* flush          )(void* pUserData, e_file* pFile);
    e_result       (* info           )(void* pUserData, e_file* pFile, e_file_info* pInfo);
    e_fs_iterator* (* first_file     )(void* pUserData, e_fs* pFS, const char* pDirectoryPath, size_t directoryPathLen, const e_allocation_callbacks* pAllocationCallbacks);
    e_fs_iterator* (* next_file      )(void* pUserData, e_fs_iterator* pIterator, const e_allocation_callbacks* pAllocationCallbacks);  /* <-- Must return null when there are no more files. In this case, free_iterator must be called internally. */
    void           (* free_iterator  )(void* pUserData, e_fs_iterator* pIterator, const e_allocation_callbacks* pAllocationCallbacks);  /* <-- Free the `e_fs_iterator` object here since `first_file` and `next_file` were the ones who allocated it. Also do any uninitialization routines. */
};

struct e_fs_config
{
    const e_fs_vtable* pVTable;   /* Can be null, in which case it will use defaults which is just the platform's standard file IO. */
    void* pVTableUserData;
};

E_API e_fs_config e_fs_config_init(const e_fs_vtable* pVTable, void* pVTableUserData);


struct e_archive_extension
{
    e_archive_vtable* pArchiveVTable;
    void* pArchiveVTableUserData;
    char pExtension[16];   /* Null terminated. */
};

typedef struct
{
    e_archive* pArchive;
    char* pFilePath;   /* Null terminated. */
} e_fs_opened_archive;

struct e_fs
{
    const e_fs_vtable* pVTable;
    void* pVTableUserData;
    e_archive_extension* pArchiveExtensions;
    size_t archiveExtensionCount;
    e_fs_opened_archive* pOpenedArchives;
    size_t openedArchiveCount;
    size_t openedArchiveCap;
    e_bool32 freeOnUninit;
};

struct e_file
{
    e_stream stream;
    e_fs* pFS;  /* Can be null in which case the system's standard file IO routines will be used. */
    const e_fs_vtable* pVTable;
    void* pVTableUserData;
};

E_API e_result e_fs_init_preallocated(const e_fs_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_fs* pFS);
E_API e_result e_fs_init(const e_fs_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_fs** ppFS);
E_API void e_fs_uninit(e_fs* pFS, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_result e_fs_open(e_fs* pFS, const char* pFilePath, int openMode, const e_allocation_callbacks* pAllocationCallbacks, e_file** ppFile);
E_API void e_fs_close(e_file* pFile, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_result e_fs_read(e_file* pFile, void* pDst, size_t bytesToRead, size_t* pBytesRead);
E_API e_result e_fs_write(e_file* pFile, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten);
E_API e_result e_fs_seek(e_file* pFile, e_int64 offset, e_seek_origin origin);
E_API e_result e_fs_tell(e_file* pFile, e_int64* pCursor);
E_API e_result e_fs_flush(e_file* pFile);
E_API e_result e_fs_info(e_file* pFile, e_file_info* pInfo);
E_API e_stream* e_fs_file_stream(e_file* pFile);
E_API e_fs* e_fs_get(e_file* pFile);
E_API e_fs_iterator* e_fs_first(e_fs* pFS, const char* pDirectoryPath, size_t directoryPathLen, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_fs_iterator* e_fs_next(e_fs_iterator* pIterator, const e_allocation_callbacks* pAllocationCallbacks);
E_API void e_fs_free_iterator(e_fs_iterator* pIterator, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_result e_fs_register_archive_extension(e_fs* pFS, e_archive_vtable* pArchiveVTable, void* pArchiveVTableUserData, const char* pExtension, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_bool32 e_fs_is_path_archive(e_fs* pFS, const char* pFilePath, size_t filePathLen);

E_API e_result e_fs_open_and_read(e_fs* pFS, const char* pFile, void** ppData, size_t* pSize, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_result e_fs_open_and_read_text(e_fs* pFS, const char* pFilePath, char** ppStr, size_t* pLength, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_result e_fs_open_and_write(e_fs* pFS, const char* pFilePath, const void* pData, size_t dataSize, const e_allocation_callbacks* pAllocationCallbacks);

/*
Helper function for gathering the file names in a directory. Returned names will be relative to the
specified directory. Free pppFileNames with e_free(). Do not free ppFileNameLengths or ppFileInfos.

Returned strings will be null terminated.

Strings will be sorted by name. Duplicates will be removed (duplicates will be possible when multiple
base paths are specified).
*/
E_API e_result e_fs_gather_files_in_directory(e_fs* pFS, const char* pDirectoryPath, size_t directoryPathLen, const e_allocation_callbacks* pAllocationCallbacks, char*** pppFileNames, size_t** ppFileNameLengths, e_file_info** ppFileInfos, size_t* pFileCount);
/* END e_fs.h */



/* BEG e_archive.h */

/*
Archives are file systems which means they need to implement the file system vtable. There are also
some additional callbacks that are required specifically for archives, particularly around loading
the archive from a stream.
*/
struct e_archive_vtable
{
    e_fs_vtable fs;
    e_result (* archive_alloc_size)(void* pUserData, size_t* pSize);
    e_result (* init              )(void* pUserData, e_stream* pStream, const e_allocation_callbacks* pAllocationCallbacks, e_archive* pArchive);
    void     (* uninit            )(void* pUserData, e_archive* pArchive, const e_allocation_callbacks* pAllocationCallbacks);
};

struct e_archive
{
    e_fs fs;    /* Archives are file systems. This must be the first member. */
    const e_archive_vtable* pVTable;
    void* pVTableUserData;
    e_stream* pStream;
    e_file* pArchiveFile;   /* This will be non-null if the archive was initailized with e_archive_init_from_file(). */
};

E_API e_result e_archive_init(const e_archive_vtable* pVTable, void* pVTableUserData, e_stream* pStream, const e_allocation_callbacks* pAllocationCallbacks, e_archive** ppArchive);
E_API e_result e_archive_init_from_file(const e_archive_vtable* pVTable, void* pVTableUserData, e_fs* pFS, const char* pFilePath, const e_allocation_callbacks* pAllocationCallbacks, e_archive** ppArchive);
E_API void e_archive_uninit(e_archive* pArchive, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_fs* e_archive_fs(e_archive* pArchive);
E_API e_stream* e_archive_stream(e_archive* pArchive);
E_API e_result e_archive_open(e_archive* pArchive, const char* pFilePath, int openMode, const e_allocation_callbacks* pAllocationCallbacks, e_file** ppFile);
E_API void e_archive_close(e_file* pFile, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_result e_archive_read(e_file* pFile, void* pDst, size_t bytesToRead, size_t* pBytesRead);
E_API e_result e_archive_write(e_file* pFile, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten);
E_API e_result e_archive_seek(e_file* pFile, e_int64 offset, e_seek_origin origin);
E_API e_result e_archive_tell(e_file* pFile, e_int64* pCursor);
E_API e_result e_archive_flush(e_file* pFile);
E_API e_result e_archive_info(e_file* pFile, e_file_info* pInfo);
E_API e_archive* e_archive_get(e_file* pFile);
E_API e_fs_iterator* e_archive_first(e_archive* pArchive, const char* pDirectoryPath, size_t directoryPathLen, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_fs_iterator* e_archive_next(e_fs_iterator* pIterator, const e_allocation_callbacks* pAllocationCallbacks);
E_API void e_archive_free_iterator(e_fs_iterator* pIterator, const e_allocation_callbacks* pAllocationCallbacks);
/* END e_archive.h */



/* BEG e_zip.h */
typedef struct e_zip e_zip;

E_API e_archive_vtable* e_zip_vtable(void);

E_API e_result e_zip_init(e_stream* pStream, const e_allocation_callbacks* pAllocationCallbacks, e_zip** ppZip);
E_API e_result e_zip_init_from_file(e_fs* pFS, const char* pFilePath, const e_allocation_callbacks* pAllocationCallbacks, e_zip** ppZip);
E_API void e_zip_uninit(e_zip* pZip, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_result e_zip_open(e_zip* pZip, const char* pFilePath, int openMode, const e_allocation_callbacks* pAllocationCallbacks, e_file** ppFile);
E_API void e_zip_close(e_file* pFile, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_result e_zip_read(e_file* pFile, void* pDst, size_t bytesToRead, size_t* pBytesRead);
E_API e_result e_zip_write(e_file* pFile, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten);
E_API e_result e_zip_seek(e_file* pFile, e_int64 offset, e_seek_origin origin);
E_API e_result e_zip_tell(e_file* pFile, e_int64* pCursor);
E_API e_result e_zip_flush(e_file* pFile);
E_API e_result e_zip_info(e_file* pFile, e_file_info* pInfo);
E_API e_fs_iterator* e_zip_first(e_zip* pZip, const char* pDirectoryPath, size_t directoryPathLen, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_fs_iterator* e_zip_next(e_fs_iterator* pIterator, const e_allocation_callbacks* pAllocationCallbacks);
E_API void e_zip_free_iterator(e_fs_iterator* pIterator, const e_allocation_callbacks* pAllocationCallbacks);
/* END e_zip.h */



/* BEG e_log.h */
typedef enum
{
    E_LOG_LEVEL_DEBUG   = 4,
    E_LOG_LEVEL_INFO    = 3,
    E_LOG_LEVEL_WARNING = 2,
    E_LOG_LEVEL_ERROR   = 1
} e_log_level;

const char* e_log_level_to_string(e_log_level level);


typedef struct e_log           e_log;
typedef struct e_log_callbacks e_log_callbacks;

typedef void (* e_log_callback_proc)(void* pUserData, e_log_level level, const char* pMessage);

struct e_log_callbacks
{
    void* pUserData;
    e_log_callback_proc onLog;
};


struct e_log
{
    e_log_callbacks* pCallbacks;
    size_t callbackCount;
    e_allocation_callbacks allocationCallbacks;    /* Need to store these persistently because e_log_postv() might need to allocate a buffer on the heap. */
    e_mutex mutex;    /* For simplifying thread-safety for custom logging callbacks. */
};

E_API e_result e_log_init(const e_allocation_callbacks* pAllocationCallbacks, e_log** ppLog);
E_API void e_log_uninit(e_log* pLog, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_result e_log_register_callback(e_log* pLog, e_log_callback_proc onLog, void* pUserData, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_result e_log_post(e_log* pLog, e_log_level level, const char* pMessage);
E_API e_result e_log_postv(e_log* pLog, e_log_level level, const char* pFormat, va_list args);
E_API e_result e_log_postf(e_log* pLog, e_log_level level, const char* pFormat, ...) E_ATTRIBUTE_FORMAT(3, 4);
/* END e_log.h */



/* BEG e_script.h */
typedef void e_script; /* This is actually a lua_State*. You can just cast this and plug it into any Lua API. */

E_API e_result e_script_init(const e_allocation_callbacks* pAllocationCallbacks, e_script** ppScript);
E_API void e_script_uninit(e_script* pScript, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_result e_script_load(e_script* pScript, e_stream* pStream, const char* pName, e_log* pLog);
E_API e_result e_script_load_file(e_script* pScript, e_fs* pFS, const char* pFilePath, const e_allocation_callbacks* pAllocationCallbacks, e_log* pLog);
/* END e_script.h */


/* BEG e_config_file.h */
typedef struct e_config_file e_config_file;

struct e_config_file
{
    void* pLuaState;    /* The lua_State object. Cast this to lua_State to use it. */
    e_allocation_callbacks allocationCallbacks; /* These are copied from pAllocationCallbacks in the call to e_config_file_init(). Needs to be persistent they'll be referenced internally by the Lua state. */
};

E_API e_result e_config_file_init(const e_allocation_callbacks* pAllocationCallbacks, e_config_file* pConfigFile);
E_API void e_config_file_uninit(e_config_file* pConfigFile, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_result e_config_file_load(e_config_file* pConfigFile, e_stream* pStream, const char* pName, const e_allocation_callbacks* pAllocationCallbacks, e_log* pLog);
E_API e_result e_config_file_load_file(e_config_file* pConfigFile, e_fs* pFS, const char* pFilePath, const e_allocation_callbacks* pAllocationCallbacks, e_log* pLog);
E_API e_result e_config_file_get_string(e_config_file* pConfigFile, const char* pSection, const char* pName, const e_allocation_callbacks* pAllocationCallbacks, char** ppValue);   /* Free the returned string with e_free(). */
E_API e_result e_config_file_get_int(e_config_file* pConfigFile, const char* pSection, const char* pName, int* pValue);
E_API e_result e_config_file_get_uint(e_config_file* pConfigFile, const char* pSection, const char* pName, unsigned int* pValue);
E_API e_result e_config_file_get_int64(e_config_file* pConfigFile, const char* pSection, const char* pName, e_int64* pValue);
E_API e_result e_config_file_get_uint64(e_config_file* pConfigFile, const char* pSection, const char* pName, e_uint64* pValue);
/* END e_config_file.h */



/* BEG e_image.h */
typedef struct e_image_loader_vtable e_image_loader_vtable;

struct e_image_loader_vtable
{
    e_result (* load)(void* pUserData, e_stream* pStream, const e_allocation_callbacks* pAllocationCallbacks, void** ppData, e_uint32* pSizeX, e_uint32* pSizeY, e_format* pFormat);
};

E_API e_result e_load_image(e_image_loader_vtable* pVTable, void* pUserData, e_stream* pStream, const e_allocation_callbacks* pAllocationCallbacks, void** ppData, e_uint32* pSizeX, e_uint32* pSizeY, e_format* pFormat);
E_API e_result e_load_image_from_file(e_image_loader_vtable* pVTable, void* pUserData, e_fs* pFS, const char* pFilePath, const e_allocation_callbacks* pAllocationCallbacks, void** ppData, e_uint32* pSizeX, e_uint32* pSizeY, e_format* pFormat);
/* END e_image.h */



/* BEG e_font.h */
typedef struct e_font          e_font;
typedef struct e_font_config   e_font_config;
typedef struct e_font_metrics  e_font_metrics;
typedef struct e_glyph_metrics e_glyph_metrics;

struct e_font_metrics
{
    e_int32 ascent;
    e_int32 descent;
    e_int32 lineGap;
};

struct e_glyph_metrics
{
    e_int32 sizeX;
    e_int32 sizeY;
    e_int32 bearingX;
    e_int32 bearingY;
    e_int32 advanceX;
    e_int32 advanceY;
};

struct e_font_config
{
    e_log* pLog;
    e_fs* pFS;
    const char* pFilePath;  /* Set to NULL if the font is being loaded using logical settings. In this case it will be loaded by the operating system. When set, will be loaded directly from a TTF file. */
};

E_API e_font_config e_font_config_init(void);
E_API e_font_config e_font_config_init_file(e_fs* pFS, const char* pFilePath);

E_API e_result e_font_init(const e_font_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_font** ppFont);
E_API void e_font_uninit(e_font* pFont, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_uint32 e_font_get_glyph_index(e_font* pFont, e_uint32 codePoint);
E_API float e_font_get_scale(e_font* pFont, float pixelHeight);
E_API void e_font_get_metrics(e_font* pFont, float scale, e_font_metrics* pMetrics);
E_API void e_font_get_glyph_metrics(e_font* pFont, float scale, e_uint32 glyphIndex, e_glyph_metrics* pMetrics);
E_API e_int32 e_font_get_kerning(e_font* pFont, float scale, e_uint32 glyphIndex1, e_uint32 glyphIndex2);
E_API void e_font_get_glyph_bitmap(e_font* pFont, float scale, e_uint32 glyphIndex, e_uint32 bitmapSizeX, e_uint32 bitmapSizeY, e_uint32 stride, e_uint8* pBitmap);
/* END e_font.h */



/* BEG e_ui.h */
typedef struct e_ui          e_ui;
typedef struct e_ui_renderer e_ui_renderer;

typedef enum
{
    E_UI_EVENT_NONE,

    /* Data Retrieval. */
    E_UI_EVENT_GET_STYLE,
    E_UI_EVENT_GET_POSITION,
    E_UI_EVENT_GET_SIZE,

    /* Operations. */
    E_UI_EVENT_UPDATE,  /* Update the UI. */
    E_UI_EVENT_RENDER,  /* Render the UI. */

    /* User Input. */
    E_UI_EVENT_MOUSE_DOWN
} e_ui_event_type;

typedef enum
{
    /* Child Direction. This controls how children will be laid out. */
    E_UI_LAYOUT_DIRECTION_TOP_TO_BOTTOM = 0x00000001,
    E_UI_LAYOUT_DIRECTION_BOTTOM_TO_TOP = 0x00000002,
    E_UI_LAYOUT_DIRECTION_LEFT_TO_RIGHT = 0x00000004,
    E_UI_LAYOUT_DIRECTION_RIGHT_TO_LEFT = 0x00000008,
    E_UI_LAYOUT_DIRECTION_HORIZONTAL    = E_UI_LAYOUT_DIRECTION_LEFT_TO_RIGHT,
    E_UI_LAYOUT_DIRECTION_VERTICAL      = E_UI_LAYOUT_DIRECTION_TOP_TO_BOTTOM,
    E_UI_LAYOUT_DIRECTION_DEFAULT       = E_UI_LAYOUT_DIRECTION_TOP_TO_BOTTOM,

    /* Child Alignment. */
    E_UI_LAYOUT_ALIGNMENT_TOP           = 0x00000010,
    E_UI_LAYOUT_ALIGNMENT_BOTTOM        = 0x00000020,
    E_UI_LAYOUT_ALIGNMENT_LEFT          = 0x00000040,
    E_UI_LAYOUT_ALIGNMENT_RIGHT         = 0x00000080,
    E_UI_LAYOUT_ALIGNMENT_CENTER_V      = 0x00000100,
    E_UI_LAYOUT_ALIGNMENT_CENTER_H      = 0x00000200,
    E_UI_LAYOUT_ALIGNMENT_DEFAULT       = E_UI_LAYOUT_ALIGNMENT_TOP | E_UI_LAYOUT_ALIGNMENT_LEFT,

    /* Child Wrapping. */
    E_UI_LAYOUT_WRAP_MODE_NONE          = 0x00000400,
    E_UI_LAYOUT_WRAP_MODE_WRAP          = 0x00000800,
    E_UI_LAYOUT_WRAP_MODE_DEFAULT       = E_UI_LAYOUT_WRAP_MODE_NONE,

    /* Self Sizing. */
    E_UI_LAYOUT_SIZING_MODE_FIT_CONTENT = 0x00001000,   /* Fit to the size of the content. */
    E_UI_LAYOUT_SIZING_MODE_FILL        = 0x00002000,   /* Fill any remaining space. */
    E_UI_LAYOUT_SIZING_MODE_RATIO       = 0x00004000,   /* Fill a percentage of the parent. */
    E_UI_LAYOUT_SIZING_MODE_FIXED       = 0x00008000,   /* Fixed sized. */
    E_UI_LAYOUT_SIZING_MODE_DEFAULT     = E_UI_LAYOUT_SIZING_MODE_FIT_CONTENT,

    /* Self Positioning. */
    E_UI_LAYOUT_POSITIONING_AUTO        = 0x00010000,   /* The control is positioned based on layout rules. */
    E_UI_LAYOUT_POSITIONING_FREE        = 0x00020000,   /* The control is positioned freely. */
    E_UI_LAYOUT_POSITIONING_DEFAULT     = E_UI_LAYOUT_POSITIONING_AUTO,

    /* Default */
    E_UI_LAYOUT_DEFAULT                 = E_UI_LAYOUT_DIRECTION_DEFAULT | E_UI_LAYOUT_ALIGNMENT_DEFAULT | E_UI_LAYOUT_WRAP_MODE_DEFAULT | E_UI_LAYOUT_SIZING_MODE_DEFAULT | E_UI_LAYOUT_POSITIONING_DEFAULT
} e_ui_layout_flags;

typedef struct
{
    e_ui_layout_flags layoutFlags;
    int spacing;    /* The spacing between children. */
    e_color bgColor;
    e_color fgColor;
    e_color borderColor;
    e_recti borderSize;
    e_recti padding;
    e_recti margin;
} e_ui_style;

typedef struct
{
    e_ui* pUI;
    e_ui_event_type type;
    union
    {
        struct
        {
            e_ui_style style;
        } getStyle;
        struct
        {
            int posX;
            int posY;
        } getPosition;
        struct
        {
            int sizeX;
            int sizeY;
        } getSize;

        struct
        {
            double dt;
        } update;
        struct
        {
            void* pRenderState;
        } render;
    } data;
} e_ui_event;

typedef e_result (* e_ui_callback)(e_ui_event* pEvent);

struct e_ui_renderer
{
    void* pUserData;
    e_result (* rect)(void* pUserData, const e_recti rect, const e_color color);
    e_result (* text)(void* pUserData, const char* pText, const e_recti rect, const e_color color);
};

struct e_ui
{
    e_ui_callback callback;
    void* pUserData;
    e_ui* pParent;
    e_ui* pFirstChild;
    e_ui* pLastChild;
    e_ui* pPrevSibling;
    e_ui* pNextSibling;

    /* The evaluated position and size. These are used for layout. Will be recalculated when a control is invalidated. */
    int evaluatedPosX;
    int evaluatedPosY;
    int evaluatedSizeX;
    int evaluatedSizeY;
};

E_API e_result e_ui_init(e_ui* pUI, e_ui_callback callback, void* pUserData);
E_API void e_ui_uninit(e_ui* pUI);
E_API void e_ui_invalidate(e_ui* pUI);                          /* Invalidates the UI and all of its children. This tells the UI system that the layout has changed and that the control needs to be redrawn. Can be called once at a base level control. */
E_API void e_ui_update(e_ui* pUI, double dt);                   /* This will step through the UI and update the layout and process any animations. */
E_API void e_ui_render(e_ui* pUI, e_ui_renderer* pRenderer);    /* This will render the UI. You should call this once on a top level UI. */
/* END e_ui.h */



/* BEG e_engine.h */
typedef enum
{
    E_ENGINE_FLAG_NO_GRAPHICS = 0x01,   /* Will also disable the graphics sub-system in clients. */
    E_ENGINE_FLAG_NO_AUDIO    = 0x02,   /* Will also disable the audio sub-system in clients. */
    E_ENGINE_FLAG_NO_OPENGL   = 0x04,   /* Disables glbind, and by extension, the default OpenGL renderer used by clients. */
    E_ENGINE_FLAG_NO_VULKAN   = 0x08,   /* Disables vkbind, and by extension, the default Vulkan renderer used by clients. */
    E_ENGINE_FLAG_NO_NETWORK  = 0x10    /* Disables the network sub-system in clients. Useful if you want to use your own network system such as ENet. */
} e_engine_flags;

typedef struct e_engine_vtable e_engine_vtable;
typedef struct e_engine_config e_engine_config;
typedef struct e_engine        e_engine;

struct e_engine_vtable
{
    e_result (* onStep)(void* pUserData, e_engine* pEngine, double dt);
};

struct e_engine_config
{
    void* pUserData;
    int argc;
    const char** argv;
    unsigned int flags;
    e_engine_vtable* pVTable;
    void* pVTableUserData;
    e_log* pLog;
    e_fs_vtable* pFSVTable;
    void* pFSVTableUserData;
    const char* pConfigFilePath;
};

E_API e_engine_config e_engine_config_init(int argc, const char** argv, unsigned int flags, e_engine_vtable* pVTable, void* pVTableUserData);


struct e_engine
{
    void* pUserData;
    unsigned int flags;
    int argc;
    const char** argv;
    e_engine_vtable* pVTable;
    void* pVTableUserData;
    e_log* pLog;
    e_bool8 isOwnerOfLog;
    e_fs fs;
    e_config_file configFile;
    e_timer timer;  /* For calculating delta times. */
    double lastTimeInSeconds;
    void* pGL;  /* Cast to GLBapi* to access OpenGL functions. */
    void* pVK;  /* Cast to VkbAPI* to access Vulkan functions. */
};

E_API e_result e_engine_init(const e_engine_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_engine** ppEngine);
E_API void e_engine_uninit(e_engine* pEngine, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_log* e_engine_get_log(e_engine* pEngine);
E_API e_result e_engine_run(e_engine* pEngine);
E_API e_result e_engine_exit(e_engine* pEngine, int exitCode);  /* Exits the main loop. */
E_API e_result e_engine_set_blocking_mode(e_engine* pEngine, e_bool32 blocking);    /* Sets the blocking mode of the main loop. */
E_API e_fs* e_engine_get_file_system(e_engine* pEngine);
static E_INLINE e_fs* e_engine_get_fs(e_engine* pEngine) { return e_engine_get_file_system(pEngine); }
E_API e_config_file* e_engine_get_config_file(e_engine* pEngine);
E_API void e_engine_reset_timer(e_engine* pEngine);
E_API e_bool32 e_engine_is_graphics_backend_supported(const e_engine* pEngine, e_graphics_backend backend);
E_API void* e_engine_get_glapi(const e_engine* pEngine);
E_API void* e_engine_get_vkapi(const e_engine* pEngine);
/* END e_engine.h */




/* BEG e_window.h */
typedef enum
{
    E_WINDOW_FLAG_FULLSCREEN = 0x01,
    E_WINDOW_FLAG_HIDDEN     = 0x02,
    E_WINDOW_FLAG_OPENGL     = 0x10     /* Configures the window to enable drawing to it via OpenGL. Does not create a rendering context. Will always be configured to use double buffering. */
} e_window_flags;

typedef enum
{
    E_EVENT_NONE,
    E_EVENT_CLOSE,
    E_EVENT_PAINT,
    E_EVENT_SIZE,
    E_EVENT_MOVE,
    E_EVENT_CURSOR_MOVE,
    E_EVENT_CURSOR_BUTTON_DOWN,
    E_EVENT_CURSOR_BUTTON_UP,
    E_EVENT_CURSOR_BUTTON_DOUBLE_CLICK,
    E_EVENT_CURSOR_WHEEL,
    E_EVENT_KEY_DOWN,
    E_EVENT_KEY_UP,
    E_EVENT_CHARACTER
} e_event_type;

#define E_CURSOR_BUTTON_LEFT   0
#define E_CURSOR_BUTTON_RIGHT  1
#define E_CURSOR_BUTTON_MIDDLE 2
#define E_CURSOR_BUTTON_4      3
#define E_CURSOR_BUTTON_5      4

#define E_KEY_UNKNOWN          0x00
#define E_KEY_BACKSPACE        0x08
#define E_KEY_TAB              0x09
#define E_KEY_ENTER            0x0D
#define E_KEY_LSHIFT           0xA0
#define E_KEY_RSHIFT           0xA1
#define E_KEY_LCONTROL         0xA2
#define E_KEY_RCONTROL         0xA3
#define E_KEY_LALT             0xA4
#define E_KEY_RALT             0xA5
#define E_KEY_PAUSE            0x13
#define E_KEY_CAPS_LOCK        0x14
#define E_KEY_ESCAPE           0x1B
#define E_KEY_SPACE            0x20
#define E_KEY_PAGE_UP          0x21
#define E_KEY_PAGE_DOWN        0x22
#define E_KEY_END              0x23
#define E_KEY_HOME             0x24
#define E_KEY_LEFT             0x25
#define E_KEY_UP               0x26
#define E_KEY_RIGHT            0x27
#define E_KEY_DOWN             0x28
#define E_KEY_PRINT_SCREEN     0x2C
#define E_KEY_INSERT           0x2D
#define E_KEY_DELETE           0x2E
#define E_KEY_0                0x30
#define E_KEY_1                0x31
#define E_KEY_2                0x32
#define E_KEY_3                0x33
#define E_KEY_4                0x34
#define E_KEY_5                0x35
#define E_KEY_6                0x36
#define E_KEY_7                0x37
#define E_KEY_8                0x38
#define E_KEY_9                0x39
#define E_KEY_A                0x41
#define E_KEY_B                0x42
#define E_KEY_C                0x43
#define E_KEY_D                0x44
#define E_KEY_E                0x45
#define E_KEY_F                0x46
#define E_KEY_G                0x47
#define E_KEY_H                0x48
#define E_KEY_I                0x49
#define E_KEY_J                0x4A
#define E_KEY_K                0x4B
#define E_KEY_L                0x4C
#define E_KEY_M                0x4D
#define E_KEY_N                0x4E
#define E_KEY_O                0x4F
#define E_KEY_P                0x50
#define E_KEY_Q                0x51
#define E_KEY_R                0x52
#define E_KEY_S                0x53
#define E_KEY_T                0x54
#define E_KEY_U                0x55
#define E_KEY_V                0x56
#define E_KEY_W                0x57
#define E_KEY_X                0x58
#define E_KEY_Y                0x59
#define E_KEY_Z                0x5A
#define E_KEY_LWIN             0x5B
#define E_KEY_RWIN             0x5C
#define E_KEY_NUMPAD_0         0x60
#define E_KEY_NUMPAD_1         0x61
#define E_KEY_NUMPAD_2         0x62
#define E_KEY_NUMPAD_3         0x63
#define E_KEY_NUMPAD_4         0x64
#define E_KEY_NUMPAD_5         0x65
#define E_KEY_NUMPAD_6         0x66
#define E_KEY_NUMPAD_7         0x67
#define E_KEY_NUMPAD_8         0x68
#define E_KEY_NUMPAD_9         0x69
#define E_KEY_MULTIPLY         0x6A
#define E_KEY_ADD              0x6B
#define E_KEY_SUBTRACT         0x6D
#define E_KEY_DECIMAL          0x6E
#define E_KEY_DIVIDE           0x6F
#define E_KEY_F1               0x70
#define E_KEY_F2               0x71
#define E_KEY_F3               0x72
#define E_KEY_F4               0x73
#define E_KEY_F5               0x74
#define E_KEY_F6               0x75
#define E_KEY_F7               0x76
#define E_KEY_F8               0x77
#define E_KEY_F9               0x78
#define E_KEY_F10              0x79
#define E_KEY_F11              0x7A
#define E_KEY_F12              0x7B
#define E_KEY_NUM_LOCK         0x90
#define E_KEY_SCROLL_LOCK      0x91
#define E_KEY_SEMICOLON        0xBA
#define E_KEY_EQUAL            0xBB
#define E_KEY_COMMA            0xBC
#define E_KEY_MINUS            0xBD
#define E_KEY_PERIOD           0xBE
#define E_KEY_SLASH            0xBF
#define E_KEY_BACKTICK         0xC0
#define E_KEY_LEFT_BRACKET     0xDB
#define E_KEY_BACKSLASH        0xDC
#define E_KEY_RIGHT_BRACKET    0xDD
#define E_KEY_QUOTE            0xDE


typedef struct e_platform_window e_platform_window; /* Platform-specific window object. This is defined in the implementation on a per-platform basis. */

typedef struct e_window_vtable e_window_vtable;
typedef struct e_window_config e_window_config;
typedef struct e_window        e_window;
typedef struct e_window_buffer e_window_buffer; /* For software rendering. */
typedef struct e_event         e_event;

struct e_window_buffer
{
    unsigned int sizeX;
    unsigned int sizeY;
    void* pData;            /* Always 0xAABBGGRR. Always aligned to 16-bytes. Each row will be padded to a multiple of 16 bytes. Linear layout. Use e_window_buffer_stride() to get the row stride in bytes. */
};

E_API unsigned int e_window_buffer_stride(const e_window_buffer* pBuffer);

struct e_event
{
    e_event_type type;
    union
    {
        struct
        {
            e_window* pWindow;
        } any;
        struct
        {
            e_window* pWindow;
        } close;
        struct
        {
            e_window* pWindow;
        } paint;
        struct
        {
            e_window* pWindow;
            int x;
            int y;
        } size;
        struct
        {
            e_window* pWindow;
            int x;
            int y;
        } move;
        struct
        {
            e_window* pWindow;
            int x;
            int y;
        } cursorMove;
        struct
        {
            e_window* pWindow;
            int button;
            int x;
            int y;
        } cursorButtonDown, cursorButtonUp, cursorButtonDoubleClick;
        struct
        {
            e_window* pWindow;
            int delta;
        } cursorWheel;
        struct
        {
            e_window* pWindow;
            e_uint32 key;   /* This is a virtual key code, not a UTF-32 character. Use the WINDOW_EVENT_CHARACTER event to check for UTF-32 character inputs for text boxes or whatnot. */
            e_bool8 isAutoRepeat;
        } keyDown, keyUp;
        struct
        {
            e_window* pWindow;
            e_uint32 utf32;
            e_bool8 isAutoRepeat;
        } character;
    } data;
};

struct e_window_vtable
{
    e_result (* onEvent)(void* pUserData, e_window* pWindow, e_event* pEvent);
};

struct e_window_config
{
    e_engine* pEngine;
    void* pUserData;
    const char* pTitle;
    int posX;
    int posY;
    unsigned int sizeX; /* The size of the client area. Set this to your game's resolution. */
    unsigned int sizeY; /* The size of the client area. Set this to your game's resolution. */
    unsigned int flags; /* A combination of E_WINDOW_FLAG_* */
    e_window_vtable* pVTable;
    void* pVTableUserData;
};

E_API e_window_config e_window_config_init(e_engine* pEngine, const char* pTitle, int posX, int posY, unsigned int sizeX, unsigned int sizeY, unsigned int flags, e_window_vtable* pVTable, void* pVTableUserData);


typedef enum
{
    /* Win32 */
    E_PLATFORM_OBJECT_WIN32_HWND,
    E_PLATFORM_OBJECT_WIN32_HDC,

    /* X11 */
    E_PLATFORM_OBJECT_XLIB_DISPLAY,
    E_PLATFORM_OBJECT_XLIB_WINDOW,
    E_PLATFORM_OBJECT_XLIB_VISUAL_INFO,

    /* EGL */
    E_PLATFORM_OBJECT_EGL_DISPLAY,
    E_PLATFORM_OBJECT_EGL_CONTEXT,
    E_PLATFORM_OBJECT_EGL_SURFACE
} e_platform_object_type;

struct e_window
{
    e_engine* pEngine;
    void* pUserData;
    e_window_vtable* pVTable;
    void* pVTableUserData;
    e_bool32 freeOnUninit;

    /*
    A window is inherently platform-specific. The platform-specific part is stored at the end of
    this structure, but it's abstracted away so we don't have to expose platform-specific code
    in the header.

    The content of this section is allocated at the end of this structure. This way we can do the
    allocation of the window as a single memory allocation just to make it cleaner.
    */
    e_platform_window* pPlatformWindow; /* This will point to the end of the struct. You shouldn't need to access this directly. It's used internally by the engine. */
};

E_API e_result e_window_alloc_size(const e_window_config* pConfig, size_t* pSize);
E_API e_result e_window_init_preallocated(const e_window_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_window* pWindow);
E_API e_result e_window_init(const e_window_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_window** ppWindow);
E_API void e_window_uninit(e_window* pWindow, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_engine* e_window_get_engine(const e_window* pWindow);
E_API void* e_window_get_user_data(const e_window* pWindow);
E_API e_window_vtable* e_window_get_vtable(const e_window* pWindow);
E_API void* e_window_get_platform_object(const e_window* pWindow, e_platform_object_type type);
E_API e_result e_window_default_event_handler(e_window* pWindow, e_event* pEvent);
E_API e_result e_window_capture_cursor(e_window* pWindow);
E_API e_result e_window_release_cursor(e_window* pWindow);
E_API e_result e_window_set_cursor_position(e_window* pWindow, int cursorPosX, int cursorPosY);
E_API e_result e_window_get_cursor_position(e_window* pWindow, int* pCursorPosX, int* pCursorPosY);
E_API e_result e_window_show_cursor(e_window* pWindow);
E_API e_result e_window_hide_cursor(e_window* pWindow);
E_API e_result e_window_pin_cursor(e_window* pWindow, int cursorPosX, int cursorPosY);
E_API e_result e_window_unpin_cursor(e_window* pWindow);
E_API e_result e_window_post_close_event(e_window* pWindow);
E_API e_result e_window_next_buffer(e_window* pWindow, unsigned int bufferSizeX, unsigned int bufferSizeY, e_window_buffer* pBuffer);   /* For software rendering. Swaps buffers, returns information about the new back buffer. */
/* END e_window.h */



/* BEG e_math.h */
#define E_PI    3.14159265358979323846
#define E_PIF   3.14159265358979323846f

E_INLINE double e_sqrtd (double x) { return sqrt(x); }
E_INLINE float  e_sqrtf (float  x) { return (float)e_sqrtd(x); }
E_INLINE float  e_rsqrtf(float  x) { return 1 / e_sqrtf(x); }   /* <-- This can be optimized. See miniaudio's implementation. */

E_INLINE float  e_lerpf(float a, float b, float t) { return a + (b - a) * t; }


typedef struct
{
    float x, y;
} e_vec2f;

typedef struct
{
    float x, y, z;
} e_vec3f;

typedef struct
{
    float x, y, z, w;
} e_vec4f;

typedef struct
{
    e_vec4f c[4];   /* "c" for column. */
} e_mat4f;

typedef struct
{
    float x, y, z, w;
} e_quatf;


E_INLINE float e_degrees(float radians) { return radians * 57.29577951308232087685f; }
E_INLINE float e_radians(float degrees) { return degrees *  0.01745329251994329577f; }

E_INLINE e_vec2f e_vec2f_2f(float x, float y)                   { e_vec2f result = {x, y}; return result; }
E_INLINE e_vec3f e_vec3f_3f(float x, float y, float z)          { e_vec3f result = {x, y, z}; return result; }

E_INLINE e_vec4f e_vec4f_4f(float x, float y, float z, float w) { e_vec4f result = {x, y, z, w}; return result; }
E_INLINE e_vec4f e_vec4f_vec3f_1f(e_vec3f v, float w)           { return e_vec4f_4f(v.x, v.y, v.z, w); }
E_INLINE e_vec4f e_vec4f_zero(void)                             { return e_vec4f_4f(0, 0, 0, 0); }
E_INLINE e_vec4f e_vec4f_add(e_vec4f a, e_vec4f b)              { return e_vec4f_4f(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
E_INLINE e_vec4f e_vec4f_sub(e_vec4f a, e_vec4f b)              { return e_vec4f_4f(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }
E_INLINE e_vec4f e_vec4f_mul(e_vec4f a, e_vec4f b)              { return e_vec4f_4f(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w); }
E_INLINE e_vec4f e_vec4f_mul_1f(e_vec4f a, float b)             { return e_vec4f_4f(a.x * b,   a.y * b,   a.z * b,   a.w * b  ); }
E_INLINE e_vec4f e_vec4f_div(e_vec4f a, e_vec4f b)              { return e_vec4f_4f(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w); }
E_INLINE float   e_vec4f_dot(e_vec4f a, e_vec4f b)              { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }
E_INLINE float   e_vec4f_len(e_vec4f a)                         { return e_sqrtf(e_vec4f_dot(a, a)); }
E_INLINE e_vec4f e_vec4f_normalize(e_vec4f a)                   { return e_vec4f_mul_1f(a, e_rsqrtf(e_vec4f_dot(a, a))); }
E_INLINE e_vec4f e_vec4f_mul_mat4f(e_vec4f v, e_mat4f m)        { return e_vec4f_4f(e_vec4f_dot(m.c[0], v), e_vec4f_dot(m.c[1], v), e_vec4f_dot(m.c[2], v), e_vec4f_dot(m.c[3], v)); }
E_INLINE e_vec4f e_vec4f_negate(e_vec4f v)                      { return e_vec4f_4f(-v.x, -v.y, -v.z, -v.w); }
E_INLINE e_vec4f e_vec4f_cross(e_vec4f a, e_vec4f b)            { return e_vec4f_4f(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x, 0); }

E_INLINE e_vec4f e_vec4f_from_pitch_yaw(float pitch, float yaw)
{
    float cp = cosf(pitch);
    float sp = sinf(pitch);
    float cy = cosf(yaw);
    float sy = sinf(yaw);

    return e_vec4f_4f
    (
        sy*cp,
        -sp,
        -cy*cp,
        0
    );
}


E_INLINE e_mat4f e_mat4f_init(e_vec4f c0, e_vec4f c1, e_vec4f c2, e_vec4f c3) { e_mat4f result; result.c[0] = c0; result.c[1] = c1; result.c[2] = c2; result.c[3] = c3; return result; }
E_INLINE e_mat4f e_mat4f_identity(void)                         { return e_mat4f_init(e_vec4f_4f(1, 0, 0, 0), e_vec4f_4f(0,  1, 0, 0), e_vec4f_4f(0, 0, 1,    0), e_vec4f_4f(0, 0, 0,    1)); }
E_INLINE e_mat4f e_mat4f_vulkan_clip_correction(void)           { return e_mat4f_init(e_vec4f_4f(1, 0, 0, 0), e_vec4f_4f(0, -1, 0, 0), e_vec4f_4f(0, 0, 0.5f, 0), e_vec4f_4f(0, 0, 0.5f, 1)); }
E_INLINE e_mat4f e_mat4f_3x3(e_mat4f m)                         { return e_mat4f_init(m.c[0], m.c[1], m.c[2], e_vec4f_4f(0, 0, 0, 0)); }

E_INLINE e_mat4f e_mat4f_mul(e_mat4f a, e_mat4f b)
{
    return e_mat4f_init
    (
        e_vec4f_4f(
            a.c[0].x*b.c[0].x + a.c[1].x*b.c[0].y + a.c[2].x*b.c[0].z + a.c[3].x*b.c[0].w,
            a.c[0].y*b.c[0].x + a.c[1].y*b.c[0].y + a.c[2].y*b.c[0].z + a.c[3].y*b.c[0].w,
            a.c[0].z*b.c[0].x + a.c[1].z*b.c[0].y + a.c[2].z*b.c[0].z + a.c[3].z*b.c[0].w,
            a.c[0].w*b.c[0].x + a.c[1].w*b.c[0].y + a.c[2].w*b.c[0].z + a.c[3].w*b.c[0].w
        ),
        e_vec4f_4f(
            a.c[0].x*b.c[1].x + a.c[1].x*b.c[1].y + a.c[2].x*b.c[1].z + a.c[3].x*b.c[1].w,
            a.c[0].y*b.c[1].x + a.c[1].y*b.c[1].y + a.c[2].y*b.c[1].z + a.c[3].y*b.c[1].w,
            a.c[0].z*b.c[1].x + a.c[1].z*b.c[1].y + a.c[2].z*b.c[1].z + a.c[3].z*b.c[1].w,
            a.c[0].w*b.c[1].x + a.c[1].w*b.c[1].y + a.c[2].w*b.c[1].z + a.c[3].w*b.c[1].w
        ),
        e_vec4f_4f(
            a.c[0].x*b.c[2].x + a.c[1].x*b.c[2].y + a.c[2].x*b.c[2].z + a.c[3].x*b.c[2].w,
            a.c[0].y*b.c[2].x + a.c[1].y*b.c[2].y + a.c[2].y*b.c[2].z + a.c[3].y*b.c[2].w,
            a.c[0].z*b.c[2].x + a.c[1].z*b.c[2].y + a.c[2].z*b.c[2].z + a.c[3].z*b.c[2].w,
            a.c[0].w*b.c[2].x + a.c[1].w*b.c[2].y + a.c[2].w*b.c[2].z + a.c[3].w*b.c[2].w
        ),
        e_vec4f_4f(
            a.c[0].x*b.c[3].x + a.c[1].x*b.c[3].y + a.c[2].x*b.c[3].z + a.c[3].x*b.c[3].w,
            a.c[0].y*b.c[3].x + a.c[1].y*b.c[3].y + a.c[2].y*b.c[3].z + a.c[3].y*b.c[3].w,
            a.c[0].z*b.c[3].x + a.c[1].z*b.c[3].y + a.c[2].z*b.c[3].z + a.c[3].z*b.c[3].w,
            a.c[0].w*b.c[3].x + a.c[1].w*b.c[3].y + a.c[2].w*b.c[3].z + a.c[3].w*b.c[3].w
        )
    );
}

E_INLINE e_vec4f e_mat4f_mul_vec4(e_mat4f m, e_vec4f v)
{
    return e_vec4f_4f(
        m.c[0].x*v.x + m.c[1].x*v.y + m.c[2].x*v.z + m.c[3].x*v.w,
        m.c[0].y*v.x + m.c[1].y*v.y + m.c[2].y*v.z + m.c[3].y*v.w,
        m.c[0].z*v.x + m.c[1].z*v.y + m.c[2].z*v.z + m.c[3].z*v.w,
        m.c[0].w*v.x + m.c[1].w*v.y + m.c[2].w*v.z + m.c[3].w*v.w
    );
}

E_INLINE e_mat4f e_mat4f_translate(e_vec4f translation) { return e_mat4f_init(e_vec4f_4f(1, 0, 0, 0), e_vec4f_4f(0, 1, 0, 0), e_vec4f_4f(0, 0, 1, 0), e_vec4f_4f(translation.x, translation.y, translation.z, 1)); }
E_INLINE e_mat4f e_mat4f_scale(e_vec4f scale)           { return e_mat4f_init(e_vec4f_4f(scale.x, 0, 0, 0), e_vec4f_4f(0, scale.y, 0, 0), e_vec4f_4f(0, 0, scale.z, 0), e_vec4f_4f(0, 0, 0, scale.w)); }

E_INLINE e_mat4f e_mat4f_rotate(float angleInRadians, e_vec3f axis)
{
    float c = cosf(angleInRadians);
    float s = sinf(angleInRadians);

    return e_mat4f_init
    (
        e_vec4f_4f((axis.x * axis.x) * (1 - c) + c,            (axis.x * axis.y) * (1 - c) - (axis.z * s), (axis.x * axis.z) * (1 - c) + (axis.y * s), 0),
        e_vec4f_4f((axis.y * axis.x) * (1 - c) + (axis.z * s), (axis.y * axis.y) * (1 - c) + c,            (axis.y * axis.z) * (1 - c) - (axis.x * s), 0),
        e_vec4f_4f((axis.z * axis.x) * (1 - c) - (axis.y * s), (axis.z * axis.y) * (1 - c) + (axis.x * s), (axis.z * axis.z) * (1 - c) + c,            0),
        e_vec4f_4f(0,                                          0,                                          0,                                          1)
    );
}

E_INLINE e_mat4f e_mat4f_from_euler_angles(float pitch, float yaw, float roll)
{
    e_mat4f rz;
    e_mat4f ry;
    e_mat4f rx;

    float cx = cosf(pitch);
    float sx = sinf(pitch);
    float cy = cosf(yaw);
    float sy = sinf(yaw);
    float cz = cosf(roll);
    float sz = sinf(roll);

    /* Must be compatible with OpenGL, where pitch is rotation about X, yaw is Y and roll is Z. */
    rz = e_mat4f_init
    (
        e_vec4f_4f(cz, -sz, 0, 0),
        e_vec4f_4f(sz,  cz, 0, 0),
        e_vec4f_4f(0,    0, 1, 0),
        e_vec4f_4f(0,    0, 0, 1)
    );

    ry = e_mat4f_init
    (
        e_vec4f_4f(cy, 0, sy, 0),
        e_vec4f_4f(0,  1, 0,  0),
        e_vec4f_4f(-sy, 0, cy, 0),
        e_vec4f_4f(0,  0, 0,  1)
    );

    rx = e_mat4f_init
    (
        e_vec4f_4f(1, 0,  0,   0),
        e_vec4f_4f(0, cx, -sx, 0),
        e_vec4f_4f(0, sx,  cx, 0),
        e_vec4f_4f(0, 0,  0,   1)
    );

    return e_mat4f_mul(rx, e_mat4f_mul(ry, rz));
}

E_INLINE e_mat4f e_mat4f_ortho(float left, float right, float bottom, float top, float znear, float zfar)
{
    return e_mat4f_init
    (
        e_vec4f_4f(2 / (right - left), 0, 0,  0),
        e_vec4f_4f(0, 2 / (top - bottom), 0,  0),
        e_vec4f_4f(0, 0, -2 / (zfar - znear), 0),
        e_vec4f_4f(-((right + left) / (right - left)), -((top + bottom) / (top - bottom)), -((zfar + znear) / (zfar - znear)), 1)
    );
}

E_INLINE e_mat4f e_mat4f_perspective(float fovy, float aspect, float znear, float zfar)
{
    float f = (float)tan(E_PI/2 - fovy/2);
    return e_mat4f_init
    (
        e_vec4f_4f(f / aspect, 0, 0, 0),
        e_vec4f_4f(0, f, 0, 0),
        e_vec4f_4f(0, 0,     (zfar + znear) / (znear - zfar), -1),
        e_vec4f_4f(0, 0, (2 * zfar * znear) / (znear - zfar),  0)
    );
}

E_INLINE e_mat4f e_mat4f_inverse(e_mat4f m)
{
    float s0 = m.c[0].x * m.c[1].y - m.c[1].x * m.c[0].y;
    float s1 = m.c[0].x * m.c[1].z - m.c[1].x * m.c[0].z;
    float s2 = m.c[0].x * m.c[1].w - m.c[1].x * m.c[0].w;
    float s3 = m.c[0].y * m.c[1].z - m.c[1].y * m.c[0].z;
    float s4 = m.c[0].y * m.c[1].w - m.c[1].y * m.c[0].w;
    float s5 = m.c[0].z * m.c[1].w - m.c[1].z * m.c[0].w;

    float c5 = m.c[2].z * m.c[3].w - m.c[3].z * m.c[2].w;
    float c4 = m.c[2].y * m.c[3].w - m.c[3].y * m.c[2].w;
    float c3 = m.c[2].y * m.c[3].z - m.c[3].y * m.c[2].z;
    float c2 = m.c[2].x * m.c[3].w - m.c[3].x * m.c[2].w;
    float c1 = m.c[2].x * m.c[3].z - m.c[3].x * m.c[2].z;
    float c0 = m.c[2].x * m.c[3].y - m.c[3].x * m.c[2].y;

    float invdet;
    float det = (s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0);
    if (det <= 1e-10) {
        return e_mat4f_identity();
    }

    invdet = 1 / det;

    return e_mat4f_init
    (
        e_vec4f_4f(( m.c[1].y * c5 - m.c[1].z * c4 + m.c[1].w * c3) * invdet,
                   (-m.c[0].y * c5 + m.c[0].z * c4 - m.c[0].w * c3) * invdet,
                   ( m.c[3].y * s5 - m.c[3].z * s4 + m.c[3].w * s3) * invdet,
                   (-m.c[2].y * s5 + m.c[2].z * s4 - m.c[2].w * s3) * invdet),

        e_vec4f_4f((-m.c[1].x * c5 + m.c[1].z * c2 - m.c[1].w * c1) * invdet,
                   ( m.c[0].x * c5 - m.c[0].z * c2 + m.c[0].w * c1) * invdet,
                   (-m.c[3].x * s5 + m.c[3].z * s2 - m.c[3].w * s1) * invdet,
                   ( m.c[2].x * s5 - m.c[2].z * s2 + m.c[2].w * s1) * invdet),

        e_vec4f_4f(( m.c[1].x * c4 - m.c[1].y * c2 + m.c[1].w * c0) * invdet,
                   (-m.c[0].x * c4 + m.c[0].y * c2 - m.c[0].w * c0) * invdet,
                   ( m.c[3].x * s4 - m.c[3].y * s2 + m.c[3].w * s0) * invdet,
                   (-m.c[2].x * s4 + m.c[2].y * s2 - m.c[2].w * s0) * invdet),

        e_vec4f_4f((-m.c[1].x * c3 + m.c[1].y * c1 - m.c[1].z * c0) * invdet,
                   ( m.c[0].x * c3 - m.c[0].y * c1 + m.c[0].z * c0) * invdet,
                   (-m.c[3].x * s3 + m.c[3].y * s1 - m.c[3].z * s0) * invdet,
                   ( m.c[2].x * s3 - m.c[2].y * s1 + m.c[2].z * s0) * invdet)
    );
}

E_INLINE e_mat4f e_mat4f_transpose(e_mat4f m)
{
    return e_mat4f_init
    (
        e_vec4f_4f(m.c[0].x, m.c[1].x, m.c[2].x, m.c[3].x),
        e_vec4f_4f(m.c[0].y, m.c[1].y, m.c[2].y, m.c[3].y),
        e_vec4f_4f(m.c[0].z, m.c[1].z, m.c[2].z, m.c[3].z),
        e_vec4f_4f(m.c[0].w, m.c[1].w, m.c[2].w, m.c[3].w)
    );
}

E_INLINE e_mat4f e_mat4f_normal(e_mat4f m)
{
    return e_mat4f_transpose(e_mat4f_inverse(e_mat4f_3x3(m)));
}


E_INLINE e_quatf e_quatf_4f(float x, float y, float z, float w) { e_quatf result = {x, y, z, w}; return result; }
E_INLINE e_quatf e_quatf_identity(void)                         { return e_quatf_4f(0, 0, 0, 1); }
E_INLINE e_quatf e_quatf_mul(e_quatf a, e_quatf b)              { return e_quatf_4f(a.x*b.w + a.w*b.x + a.y*b.z - a.z*b.y, a.y*b.w + a.w*b.y + a.z*b.x - a.x*b.z, a.z*b.w + a.w*b.z + a.x*b.y - a.y*b.x, a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z); }
E_INLINE e_quatf e_quatf_mul_1f(e_quatf a, float b)             { return e_quatf_4f(a.x*b, a.y*b, a.z*b, a.w*b); }
E_INLINE e_quatf e_quatf_div(e_quatf a, e_quatf b)              { return e_quatf_4f(a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w); }
E_INLINE float   e_quatf_dot(e_quatf a, e_quatf b)              { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }
E_INLINE e_quatf e_quatf_conjugate(e_quatf a)                   { return e_quatf_4f(-a.x, -a.y, -a.z, a.w); }
E_INLINE e_quatf e_quatf_normalize(e_quatf a)                   { return e_quatf_mul_1f(a, e_rsqrtf(e_quatf_dot(a, a))); }

E_INLINE e_quatf e_quatf_from_axis_angle(e_vec3f axis, float angleInRadians)
{
    float s = sinf(angleInRadians / 2);
    return e_quatf_4f(axis.x * s, axis.y * s, axis.z * s, cosf(angleInRadians / 2));
}

E_INLINE e_quatf e_quatf_from_euler_angles(float pitch, float yaw, float roll)
{
    float cy = cosf(yaw * 0.5f);
    float sy = sinf(yaw * 0.5f);
    float cp = cosf(pitch * 0.5f);
    float sp = sinf(pitch * 0.5f);
    float cr = cosf(roll * 0.5f);
    float sr = sinf(roll * 0.5f);

    return e_quatf_4f
    (
        cy * cp * sr - sy * sp * cr,
        sy * cp * sr + cy * sp * cr,
        sy * cp * cr - cy * sp * sr,
        cy * cp * cr + sy * sp * sr
    );
}

E_INLINE e_mat4f e_quatf_to_mat4f(e_quatf q)
{
    float x2 = q.x * q.x;
    float y2 = q.y * q.y;
    float z2 = q.z * q.z;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float yz = q.y * q.z;
    float wx = q.w * q.x;
    float wy = q.w * q.y;
    float wz = q.w * q.z;

    return e_mat4f_init
    (
        e_vec4f_4f(1 - 2 * (y2 + z2), 2 * (xy - wz), 2 * (xz + wy), 0),
        e_vec4f_4f(2 * (xy + wz), 1 - 2 * (x2 + z2), 2 * (yz - wx), 0),
        e_vec4f_4f(2 * (xz - wy), 2 * (yz + wx), 1 - 2 * (x2 + y2), 0),
        e_vec4f_4f(0, 0, 0, 1)
    );
}

E_INLINE e_quatf e_quatf_from_mat4f(e_mat4f m)
{
    e_quatf q;
    float t;
    float r;
    
    t = m.c[0].x + m.c[1].y + m.c[2].z;

    if (t > 0) {
        r = e_sqrtf(1 + t);
        q.w = 0.5f * r;
        r = 0.5f / r;
        q.x = (m.c[1].z - m.c[2].y) * r;
        q.y = (m.c[2].x - m.c[0].z) * r;
        q.z = (m.c[0].y - m.c[1].x) * r;
    } else {
        if (m.c[0].x > m.c[1].y && m.c[0].x > m.c[2].z) {
            r = e_sqrtf(1 + m.c[0].x - m.c[1].y - m.c[2].z);
            q.x = 0.5f * r;
            r = 0.5f / r;
            q.y = (m.c[0].y + m.c[1].x) * r;
            q.z = (m.c[2].x + m.c[0].z) * r;
            q.w = (m.c[1].z - m.c[2].y) * r;
        } else if (m.c[1].y > m.c[2].z) {
            r = e_sqrtf(1 + m.c[1].y - m.c[0].x - m.c[2].z);
            q.y = 0.5f * r;
            r = 0.5f / r;
            q.z = (m.c[1].z + m.c[2].y) * r;
            q.x = (m.c[0].y + m.c[1].x) * r;
            q.w = (m.c[2].x - m.c[0].z) * r;
        } else {
            r = e_sqrtf(1 + m.c[2].z - m.c[0].x - m.c[1].y);
            q.z = 0.5f * r;
            r = 0.5f / r;
            q.x = (m.c[2].x + m.c[0].z) * r;
            q.y = (m.c[1].z + m.c[2].y) * r;
            q.w = (m.c[0].y - m.c[1].x) * r;
        }
    }

    return q;
}

E_INLINE e_quatf e_quatf_lookat(e_vec3f eye, e_vec3f target, e_vec3f up)
{
    e_vec4f f = e_vec4f_normalize(e_vec4f_sub(e_vec4f_vec3f_1f(target, 0), e_vec4f_vec3f_1f(eye, 0)));
    e_vec4f s = e_vec4f_normalize(e_vec4f_cross(f, e_vec4f_vec3f_1f(up, 0)));
    e_vec4f u = e_vec4f_cross(s, f);

    e_mat4f m = e_mat4f_init
    (
        e_vec4f_4f(s.x, u.x, -f.x, 0),
        e_vec4f_4f(s.y, u.y, -f.y, 0),
        e_vec4f_4f(s.z, u.z, -f.z, 0),
        e_vec4f_4f(0, 0, 0, 1)
    );

    return e_quatf_from_mat4f(m);
}
/* END e_math.h */




/* === BEG e_input.h === */
#ifndef E_MAX_CURSOR_BUTTONS
#define E_MAX_CURSOR_BUTTONS    8
#endif

#define E_BUTTON_STATE_UP       0
#define E_BUTTON_STATE_DOWN     1

#define E_KEY_STATE_UP          0
#define E_KEY_STATE_DOWN        1

#define E_INPUT_CHARACTER_BUFFER_CAP 128

typedef struct e_input_config e_input_config;
typedef struct e_input        e_input;

struct e_input_config
{
    int _unused;
};

E_API e_input_config e_input_config_init(void);


#define E_KEY_STATE_FLAG_UP         0x01
#define E_KEY_STATE_FLAG_DOWN       0x02

#define E_MAX_KEYS_DOWN             16

typedef struct
{
    e_uint32 key;
    e_uint32 stateFlags;
} e_key_state;

struct e_input
{
    int prevAbsoluteCursorPosX;
    int prevAbsoluteCursorPosY;
    int currentAbsoluteCursorPosX;
    int currentAbsoluteCursorPosY;
    int cursorButtonStates[E_MAX_CURSOR_BUTTONS];
    int prevCursorButtonStates[E_MAX_CURSOR_BUTTONS];
    int cursorWheelDelta;
    e_uint32 keysDown[E_MAX_KEYS_DOWN];
    e_uint32 keysDownCount;
    e_uint32 prevKeysDown[E_MAX_KEYS_DOWN];
    e_uint32 prevKeysDownCount;
    e_key_state keyPressedStates[E_MAX_KEYS_DOWN];    /* When empty, key was neither pressed nor released. */
    e_uint32 keyPressedStateCount;
    e_uint32 characters[E_INPUT_CHARACTER_BUFFER_CAP];
    e_uint32 characterCount;
    e_uint32 characterIndex;    /* The index of the next character in the buffer. */
    e_bool32 freeOnUninit;
};

E_API e_result e_input_alloc_size(const e_input_config* pConfig, size_t* pSize);
E_API e_result e_input_init_preallocated(const e_input_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_input* pInput);
E_API e_result e_input_init(const e_input_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_input** ppInput);
E_API void e_input_uninit(e_input* pInput, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_result e_input_step(e_input* pInput);   /* This should be called at the *end* of your frame. It will clear the state of the input system for things like mouse wheel deltas. */
E_API e_result e_input_add_cursor_delta_position(e_input* pInput, int posX, int posY);
E_API e_result e_input_set_absolute_cursor_position(e_input* pInput, int deltaX, int deltaY);
E_API e_result e_input_get_absolute_cursor_position(e_input* pInput, int* pPosX, int* pPosY);
E_API e_result e_input_set_prev_absolute_cursor_position(e_input* pInput, int prevCursorPosX, int prevCursorPosY);
E_API e_bool32 e_input_has_cursor_moved(e_input* pInput);
E_API void e_input_get_cursor_move_delta(e_input* pInput, int* pDeltaX, int* pDeltaY);
E_API int  e_input_get_cursor_move_delta_x(e_input* pInput);
E_API int  e_input_get_cursor_move_delta_y(e_input* pInput);
E_API void e_input_set_cursor_button_state(e_input* pInput, e_uint32 buttonIndex, int state);
E_API int  e_input_get_cursor_button_state(e_input* pInput, e_uint32 buttonIndex);
E_API e_bool32 e_input_was_cursor_button_pressed(e_input* pInput, e_uint32 buttonIndex);
E_API e_bool32 e_input_was_cursor_button_released(e_input* pInput, e_uint32 buttonIndex);
E_API void e_input_set_cursor_wheel_delta(e_input* pInput, int delta);
E_API int  e_input_get_cursor_wheel_delta(e_input* pInput);
E_API void e_input_set_key_down(e_input* pInput, e_uint32 key);
E_API void e_input_set_key_up(e_input* pInput, e_uint32 key);
E_API e_bool32 e_input_was_key_pressed(e_input* pInput, e_uint32 key);
E_API e_bool32 e_input_was_key_released(e_input* pInput, e_uint32 key);
E_API e_bool32 e_input_is_key_down(e_input* pInput, e_uint32 key);
E_API void e_input_enqueue_character(e_input* pInput, e_uint32 utf32);
E_API e_uint32 e_input_dequeue_character(e_input* pInput); /* Will return 0 if there are no more characters buffered. */
/* END e_input.h */




/* BEG e_graphics.h */
typedef struct e_graphics_vtable                        e_graphics_vtable;
typedef struct e_graphics_config                        e_graphics_config;
typedef struct e_graphics                               e_graphics;

typedef struct e_graphics_device_config                 e_graphics_device_config;
typedef struct e_graphics_device                        e_graphics_device;

typedef struct e_graphics_surface_config                e_graphics_surface_config;
typedef struct e_graphics_surface                       e_graphics_surface;

typedef struct e_graphics_device_info                   e_graphics_device_info;

typedef unsigned int                                    e_graphics_device_id;
#define E_DEFAULT_GRAPHICS_DEVICE_ID                    0

struct e_graphics_vtable
{
    /* e_graphics */
    const char* (* get_name                        )(void* pUserData);
    e_result    (* alloc_size                      )(void* pUserData, const e_graphics_config* pConfig, size_t* pSize);
    e_result    (* init                            )(void* pUserData, e_graphics* pGraphics, const e_graphics_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks);
    void        (* uninit                          )(void* pUserData, e_graphics* pGraphics, const e_allocation_callbacks* pAllocationCallbacks);
    e_result    (* get_devices                     )(void* pUserData, e_graphics* pGraphics, const e_allocation_callbacks* pAllocationCallbacks, size_t* pDeviceCount, e_graphics_device_info* pDeviceInfos);
    e_result    (* set_surface                     )(void* pUserData, e_graphics* pGraphics, e_graphics_surface* pSurface);
    e_result    (* present_surface                 )(void* pUserData, e_graphics* pGraphics, e_graphics_surface* pSurface);

    /* e_graphics_device */
    e_result    (* device_alloc_size               )(void* pUserData, const e_graphics_device_config* pConfig, size_t* pSize);
    e_result    (* device_init                     )(void* pUserData, e_graphics_device* pDevice, const e_graphics_device_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks);
    void        (* device_uninit                   )(void* pUserData, e_graphics_device* pDevice, const e_allocation_callbacks* pAllocationCallbacks);

    /* e_graphics_surface */
    e_result    (* surface_alloc_size              )(void* pUserData, const e_graphics_surface_config* pConfig, size_t* pSize);
    e_result    (* surface_init                    )(void* pUserData, e_graphics_surface* pSurface, const e_graphics_surface_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks);
    void        (* surface_uninit                  )(void* pUserData, e_graphics_surface* pSurface, const e_allocation_callbacks* pAllocationCallbacks);
    e_result    (* surface_refresh                 )(void* pUserData, e_graphics_surface* pSurface, const e_allocation_callbacks* pAllocationCallbacks);  /* <-- Rebuild your swapchain here. This will be called in response to the surface being resized. */
};


typedef enum
{
    E_GRAPHICS_DEVICE_FAMILY_FLAG_GRAPHICS = 0x01,
    E_GRAPHICS_DEVICE_FAMILY_FLAG_COMPUTE  = 0x02
} e_graphics_device_families;

struct e_graphics_device_info
{
    char name[256];         /* A fixed array so we can avoid memory management. */
    e_graphics_device_id id;
    unsigned int families;  /* For determining if the device supports compute. */
};


struct e_graphics_config
{
    e_engine* pEngine;
    e_graphics_backend backend; /* Set to E_GRAPHICS_BACKEND_UNKNOWN (default) to select the best available option. The order will be as defined in the e_graphics_backend enum. */
    e_graphics_vtable* pVTable; /* Can specify a custom vtable if you want to use your own backend. Set the backend to E_GRAPHICS_BACKEND_CUSTOM to always use this, otherwise it'll only be used if all stock backends fail. */
    void* pVTableUserData;
    void* pUserData;
};

E_API e_graphics_config e_graphics_config_init(e_engine* pEngine);


struct e_graphics
{
    e_engine* pEngine;
    e_graphics_backend backend;
    const e_graphics_vtable* pVTable;
    void* pVTableUserData;
    void* pUserData;
};

E_API e_result e_graphics_init(const e_graphics_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_graphics** ppGraphics);
E_API void e_graphics_uninit(e_graphics* pGraphics, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_engine* e_graphics_get_engine(const e_graphics* pGraphics);
E_API e_graphics_backend e_graphics_get_backend(const e_graphics* pGraphics);
E_API e_log* e_graphics_get_log(const e_graphics* pGraphics);
E_API e_result e_graphics_get_devices(e_graphics* pGraphics, const e_allocation_callbacks* pAllocationCallbacks, size_t* pDeviceCount, e_graphics_device_info* pDeviceInfos);
E_API e_result e_graphics_set_surface(e_graphics* pGraphics, e_graphics_surface* pSurface);
E_API e_result e_graphics_present_surface(e_graphics* pGraphics, e_graphics_surface* pSurface);


struct e_graphics_device_config
{
    e_graphics* pGraphics;
    e_graphics_device_id deviceID;  /* Set this to 0 to use the default graphics device. */
};

E_API e_graphics_device_config e_graphics_device_config_init(e_graphics* pGraphics);


struct e_graphics_device
{
    e_graphics* pGraphics;
    e_bool32 freeOnUninit;
};

E_API e_result e_graphics_device_alloc_size(const e_graphics_device_config* pConfig, size_t* pSize);
E_API e_result e_graphics_device_init_preallocated(const e_graphics_device_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_graphics_device* pDevice);
E_API e_result e_graphics_device_init(const e_graphics_device_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_graphics_device** ppDevice);
E_API void e_graphics_device_uninit(e_graphics_device* pDevice, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_graphics* e_graphics_device_get_graphics(const e_graphics_device* pDevice);
E_API e_log* e_graphics_device_get_log(e_graphics_device* pDevice);


struct e_graphics_surface_config
{
    e_graphics_device* pDevice;
    e_window* pWindow;
};

E_API e_graphics_surface_config e_graphics_surface_config_init(e_graphics_device* pDevice, e_window* pWindow);


struct e_graphics_surface
{
    e_graphics* pGraphics;
    e_graphics_device* pDevice;
    e_window* pWindow;
    e_bool32 freeOnUninit;
};

E_API e_result e_graphics_surface_alloc_size(const e_graphics_surface_config* pConfig, size_t* pSize);
E_API e_result e_graphics_surface_init_preallocated(const e_graphics_surface_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_graphics_surface* pSurface);
E_API e_result e_graphics_surface_init(const e_graphics_surface_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_graphics_surface** ppSurface);
E_API void e_graphics_surface_uninit(e_graphics_surface* pSurface, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_result e_graphics_surface_refresh(e_graphics_surface* pSurface, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_graphics* e_graphics_surface_get_graphics(const e_graphics_surface* pSurface);
E_API e_graphics_device* e_graphics_surface_get_device(const e_graphics_surface* pSurface);
E_API e_log* e_graphics_surface_get_log(e_graphics_surface* pSurface);
/* END e_graphics.h */



/* BEG e_gui.h */
typedef struct e_gui_config e_gui_config;
typedef struct e_gui e_gui; /* The main GUI object. All GUI objects are a e_gui object. */

typedef enum
{
    E_GUI_EVENT_TYPE_NONE,
    E_GUI_EVENT_TYPE_PAINT,
    E_GUI_EVENT_TYPE_CURSOR_MOVE,
    E_GUI_EVENT_TYPE_CURSOR_BUTTON_DOWN,
    E_GUI_EVENT_TYPE_CURSOR_BUTTON_UP,
    E_GUI_EVENT_TYPE_CURSOR_BUTTON_DOUBLE_CLICK
} e_gui_event_type;

#define E_GUI_EVENT_BASE_MEMBERS \
    e_gui_event_type type

typedef struct
{
    E_GUI_EVENT_BASE_MEMBERS;
} e_gui_event_paint;

typedef struct
{
    E_GUI_EVENT_BASE_MEMBERS;
    int x;
    int y;
} e_gui_event_cursor_move;

typedef struct
{
    E_GUI_EVENT_BASE_MEMBERS;
    int x;
    int y;
    int button;
} e_gui_event_cursor_button;

typedef union
{
    E_GUI_EVENT_BASE_MEMBERS;
    e_gui_event_paint paint;
    e_gui_event_cursor_move cursorMove;
    e_gui_event_cursor_button cursorButtonDown;
    e_gui_event_cursor_button cursorButtonUp;
    e_gui_event_cursor_button cursorButtonDoubleClick;
} e_gui_event;


typedef e_result (* e_gui_event_proc)(void* pUserData, e_gui* pGUI, const e_gui_event* pEvent);

struct e_gui_config
{
    e_gui_event_proc onEvent;
    void* pUserData;
};

E_API e_gui_config e_gui_config_init(e_gui_event_proc onEvent, void* pUserData);


struct e_gui
{
    e_gui_event_proc onEvent;
    void* pUserData;
};

E_API e_result e_gui_init(const e_gui_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_gui* pGUI);
E_API void e_gui_uninit(e_gui* pGUI, const e_allocation_callbacks* pAllocationCallbacks);
/* END e_gui.h */


/* BEG e_client.h */
typedef enum
{
    E_CLIENT_FLAG_NO_WINDOW     = 0x01,   /* Does not create a window. This also disables graphics. */
    E_CLIENT_FLAG_NO_GRAPHICS   = 0x02,   /* Disables the graphics sub-system. Useful if you want to implement your own graphics system rather than using the default. */
    E_CLIENT_FLAG_NO_AUDIO      = 0x04,   /* Disables the audio sub-system. */
    E_CLIENT_FLAG_OPENGL_WINDOW = 0x10    /* Enables OpenGL on the window. Only used when graphics are disabled by the engine or the client (E_CLIENT_NO_GRAPHICS). */
} e_client_flags;

typedef struct e_client_vtable e_client_vtable;
typedef struct e_client_config e_client_config;
typedef struct e_client        e_client;


struct e_client_vtable
{
    e_result (* onEvent)(void* pUserData, e_client* pClient, e_event* pEvent);
    e_result (* onStep )(void* pUserData, e_client* pClient, double dt);
};


struct e_client_config
{
    e_engine* pEngine;
    const char* pConfigFileSection; /* The section in the config to read properties from, such as resolution, preferred graphics backend, etc. */
    void* pUserData;
    e_client_vtable* pVTable;
    void* pVTableUserData;
    const char* pWindowTitle;
    unsigned int resolutionX;   /* Only used if the resolution is not specified in a config file. */
    unsigned int resolutionY;
    unsigned int flags;
    e_graphics_device_id graphicsDeviceID;
    e_graphics_backend graphicsBackend;
};

E_API e_client_config e_client_config_init(e_engine* pEngine, const char* pConfigFileSection);


struct e_client
{
    e_engine* pEngine;
    void* pUserData;
    e_client_vtable* pVTable;
    void* pVTableUserData;
    unsigned int flags;
    e_window* pWindow;
    e_uint32 windowSizeX;
    e_uint32 windowSizeY;
    e_bool32 windowResized;
    e_graphics* pGraphics;
    e_graphics_device* pGraphicsDevice;
    e_graphics_surface* pGraphicsSurface;
    e_input* pInput;
    e_bool32 isCursorPinned;
    const char* pConfigSection;
    e_allocation_callbacks allocationCallbacks;
    e_bool32 freeOnUninit;
};

E_API e_result e_client_alloc_size(const e_client_config* pConfig, size_t* pSize);
E_API e_result e_client_init_preallocated(const e_client_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_client* pClient);
E_API e_result e_client_init(const e_client_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_client** ppClient);
E_API void e_client_uninit(e_client* pClient, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_engine* e_client_get_engine(e_client* pClient);
E_API e_log* e_client_get_log(e_client* pClient);
E_API e_window* e_client_get_window(e_client* pClient);
E_API e_result e_client_on_window_resize(e_client* pClient, e_uint32 sizeX, e_uint32 sizeY);        /* Use this to tell the client that the window has been resized. */
E_API e_bool32 e_client_get_window_size(e_client* pClient, e_uint32* pSizeX, e_uint32* pSizeY);     /* Returns true if the window has been resized since the last step. pSizeX and pSizeY will always be filled with the correct window size, even when false has returned. */
E_API e_input* e_client_get_input(e_client* pClient);
E_API e_result e_client_default_event_handler(e_client* pClient, e_event* pEvent);           /* You will usually want to call this in response to all events from the client. If you don't, you'll need to implement certain functionality yourself. See the implementation for details. */
E_API e_result e_client_post_close_event(e_client* pClient);                                 /* Posts a close event to the client. */
E_API e_result e_client_update_input_from_event(e_client* pClient, const e_event* pEvent);
E_API e_result e_client_step(e_client* pClient, double dt);
E_API e_result e_client_step_input(e_client* pClient);
E_API e_bool32 e_client_has_cursor_moved(e_client* pClient);
E_API e_result e_client_get_absolute_cursor_position(e_client* pClient, int* pPosX, int* pPosY);
E_API e_result e_client_capture_cursor(e_client* pClient);
E_API e_result e_client_release_cursor(e_client* pClient);
E_API e_result e_client_pin_cursor(e_client* pClient, int pinnedCursorPosX, int pinnedCursorPosY);
E_API e_result e_client_unpin_cursor(e_client* pClient);
E_API e_result e_client_show_cursor(e_client* pClient);
E_API e_result e_client_hide_cursor(e_client* pClient);
/* END e_client.h */



/* BEG e_editor.h */
typedef struct e_editor_config e_editor_config;
typedef struct e_editor        e_editor;

struct e_editor_config
{
    e_engine* pEngine;  /* The engine instance that owns the editor. */
};

E_API e_editor_config e_editor_config_init(e_engine* pEngine);


struct e_editor
{
    e_engine* pEngine;
};

E_API e_result e_editor_init(const e_editor_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_editor** ppEditor);
E_API void e_editor_uninit(e_editor* pEditor, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_result e_editor_run(e_editor* pEditor);
E_API e_result e_editor_show(e_editor* pEditor);
E_API e_result e_editor_hide(e_editor* pEditor);
/* END e_editor.h */

#endif  /* e_h */
