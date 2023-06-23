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

#include <stddef.h> /* size_t */
#include <stdarg.h> /* va_list */
#include <math.h>

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

#if defined(MA_USE_STDINT)
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
#else
    typedef unsigned int       e_uintptr;
#endif

typedef unsigned char e_bool8;
typedef unsigned int  e_bool32;
#define E_TRUE  1
#define E_FALSE 0

typedef void* e_handle;
typedef void* e_ptr;

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
    E_FORMAT_R8G8B8A8_UNORM,
    E_FORMAT_D24_UNORM_S8_UINT,
} e_format;


/* dlopen, etc. with e_handle as the library handle. */
E_API e_handle e_dlopen(const char* pFilePath);
E_API void     e_dlclose(e_handle hLibrary);
E_API void*    e_dlsym(e_handle hLibrary, const char* pSymbol);
E_API e_result e_dlerror(char* pOutMessage, size_t messageSizeInBytes);


/* Allocation callbacks must be thread-safe. */
typedef struct e_allocation_callbacks
{
    void* pUserData;
    void* (* onMalloc)(size_t sz, void* pUserData);
    void* (* onRealloc)(void* p, size_t sz, void* pUserData);
    void  (* onFree)(void* p, void* pUserData);
} e_allocation_callbacks;

E_API void* e_malloc(size_t sz, const e_allocation_callbacks* pAllocationCallbacks);
E_API void* e_calloc(size_t sz, const e_allocation_callbacks* pAllocationCallbacks);
E_API void* e_realloc(void* p, size_t sz, const e_allocation_callbacks* pAllocationCallbacks);
E_API void  e_free(void* p, const e_allocation_callbacks* pAllocationCallbacks);



/* ==== BEG e_misc.h ==== */
E_API void e_qsort_s(void* pList, size_t count, size_t stride, int (*compareProc)(void*, const void*, const void*), void* pUserData);

E_API void* e_binary_search(const void* pKey, const void* pList, size_t count, size_t stride, int (*compareProc)(void*, const void*, const void*), void* pUserData);
E_API void* e_linear_search(const void* pKey, const void* pList, size_t count, size_t stride, int (*compareProc)(void*, const void*, const void*), void* pUserData);
E_API void* e_sorted_search(const void* pKey, const void* pList, size_t count, size_t stride, int (*compareProc)(void*, const void*, const void*), void* pUserData);
/* ==== END e_misc.h ==== */



/* ==== BEG e_net.h ==== */
#if defined(_WIN32)
    typedef e_uintptr e_socket_handle;
#else
    typedef int       e_socket_handle;
#endif

E_API e_result e_net_init(void);
E_API void e_net_uninit(void);
/* ==== END e_net.h ==== */



/* ==== BEG e_threading.h ==== */
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

E_API size_t e_mutex_alloc_size();
E_API e_result e_mutex_init_preallocated(e_mutex* pMutex);
E_API e_result e_mutex_init(const e_allocation_callbacks* pAllocationCallbacks, e_mutex** ppMutex);
E_API void e_mutex_uninit(e_mutex* pMutex, const e_allocation_callbacks* pAllocationCallbacks);
E_API void e_mutex_lock(e_mutex* pMutex);
E_API void e_mutex_unlock(e_mutex* pMutex);
/* ==== END e_threading.h ==== */



/* ==== BEG e_stream.h ==== */
typedef enum
{
    E_SEEK_ORIGIN_CURRENT,
    E_SEEK_ORIGIN_START,
    E_SEEK_ORIGIN_END
} e_seek_origin;

typedef struct e_stream_vtable e_stream_vtable;
typedef struct e_stream        e_stream;

struct e_stream_vtable
{
    e_result (* read )(void* pUserData, void* pDst, size_t bytesToRead, size_t* pBytesRead);
    e_result (* write)(void* pUserData, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten);
    e_result (* seek )(void* pUserData, e_int64 offset, e_seek_origin origin);
    e_result (* tell )(void* pUserData, e_int64* pCursor);
};


struct e_stream
{
    const e_stream_vtable* pVTable;
    void* pVTableUserData;
};

E_API e_result e_stream_init(const e_stream_vtable* pVTable, void* pVTableUserData, e_stream* pStream);
E_API e_result e_stream_read(e_stream* pStream, void* pDst, size_t bytesToRead, size_t* pBytesRead);
E_API e_result e_stream_write(e_stream* pStream, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten);
E_API e_result e_stream_seek(e_stream* pStream, e_int64 offset, e_seek_origin origin);
E_API e_result e_stream_tell(e_stream* pStream, e_int64* pCursor);


/*
Memory streams support both reading and writing within the same stream. To only support read-only
mode, use e_memory_stream_init_readonly(). With this you can pass in a standard data/size pair.

If you need writing support, use e_memory_stream_init_write(). This takes a pointer to a pointer
to the buffer. As the stream writes data, it will dynamically expand the buffer as required. This
mode also supports reading.

You can overwrite data by seeking to the required location and then just writing like normal. To
append data, just seek to the end:

    e_memory_stream_seek(pStream, 0, E_SEEK_ORIGIN_END);

Write mode does not support initialization against an existing buffer. For example, you cannot
allocate a buffer externally, and then plug it into a memory stream to append more data to it.
The reason for this is that the memory stream cannot know if the original buffer is compatible
for resizing.

The memory stream need not be uninitialized. Simply destroy your `e_memory_stream` object when
it's no longer needed. In write mode, the buffer you passed into the config will contain the
data. Free this data with `e_free()` when it's no longer required.

Below is an example for write mode.

    ```c
    void* pData;
    size_t dataSize;

    e_memory_stream stream;
    e_memory_stream_init_write(&pData, &dataSize, NULL, &stream);
    
    // Write some data to the stream...
    e_memory_stream_write(&stream, pSomeData, someDataSize, NULL);
    
    // Do something with the data.
    do_something_with_my_data(pData, dataSize);

    // Free the data when it's no longer needed. I'll be stored in pData and dataSize.
    e_free(pData, NULL);
    ```

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
    size_t cursor;
    size_t dataCap; /* Only used in write mode. Keeps track of the capacity of the *ppData buffer. */
    e_allocation_callbacks allocationCallbacks; /* This is copied from the allocation callbacks passed in from e_memory_stream_init(). Only used in write mode. */
};

E_API e_result e_memory_stream_init_write(void** ppData, size_t* pDataSize, const e_allocation_callbacks* pAllocationCallbacks, e_memory_stream* pStream);
E_API e_result e_memory_stream_init_readonly(const void* pData, size_t dataSize, e_memory_stream* pStream);
E_API e_result e_memory_stream_read(e_memory_stream* pStream, void* pDst, size_t bytesToRead, size_t* pBytesRead);
E_API e_result e_memory_stream_write(e_memory_stream* pStream, const void* pSrc, size_t bytesToWrite, size_t* pBytesWritten);
E_API e_result e_memory_stream_seek(e_memory_stream* pStream, e_int64 offset, e_seek_origin origin);
E_API e_result e_memory_stream_tell(e_memory_stream* pStream, size_t* pCursor);
E_API e_result e_memory_stream_remove(e_memory_stream* pStream, size_t offset, size_t size);
E_API e_result e_memory_stream_truncate(e_memory_stream* pStream);
/* ==== END e_stream.h ==== */



/* ==== BEG e_deflate.h ==== */
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
/* ==== END e_deflate.h ==== */





/* ==== BEG e_fs.h ==== */
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
/* ==== END e_fs.h ==== */



/* ==== BEG e_archive.h ==== */

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
/* ==== END e_archive.h ==== */



/* ==== BEG e_zip.h ==== */
typedef struct e_zip e_zip;

E_API e_archive_vtable* e_zip_vtable();

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
/* ==== END e_zip.h ==== */



/* ==== BEG e_log.h ==== */
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
    e_mutex* pMutex;    /* For simplifying thread-safety for custom logging callbacks. */
};

E_API e_result e_log_init(const e_allocation_callbacks* pAllocationCallbacks, e_log** ppLog);
E_API void e_log_uninit(e_log* pLog, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_result e_log_register_callback(e_log* pLog, e_log_callback_proc onLog, void* pUserData, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_result e_log_post(e_log* pLog, e_log_level level, const char* pMessage);
E_API e_result e_log_postv(e_log* pLog, e_log_level level, const char* pFormat, va_list args);
E_API e_result e_log_postf(e_log* pLog, e_log_level level, const char* pFormat, ...) E_ATTRIBUTE_FORMAT(3, 4);
/* ==== END e_log.h ==== */



/* ==== BEG e_config_file.h ==== */
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
E_API e_result e_config_file_get_string(e_config_file* pConfigFile, const char* pSection, const char* pName, const e_allocation_callbacks* pAllocationCallbacks, char** ppValue);
E_API e_result e_config_file_get_int(e_config_file* pConfigFile, const char* pSection, const char* pName, int* pValue);
E_API e_result e_config_file_get_uint(e_config_file* pConfigFile, const char* pSection, const char* pName, unsigned int* pValue);
/* ==== END e_config_file.h ==== */





/* ==== BEG e_engine.h ==== */
typedef enum
{
    E_ENGINE_FLAG_NO_GRAPHICS = 0x01,   /* Will also disable the graphics sub-system in clients. */
    E_ENGINE_FLAG_NO_AUDIO    = 0x02,   /* Will also disable the audio sub-system in clients. */
    E_ENGINE_FLAG_NO_OPENGL   = 0x04,   /* Disables glbind, and by extension, the default OpenGL renderer used by clients. */
    E_ENGINE_FLAG_NO_VULKAN   = 0x08    /* Disables vkbind, and by extension, the default Vulkan renderer used by clients. */
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
    char** argv;
    unsigned int flags;
    e_engine_vtable* pVTable;
    void* pVTableUserData;
    e_log* pLog;
    e_fs_vtable* pFSVTable;
    void* pFSVTableUserData;
};

E_API e_engine_config e_engine_config_init(int argc, char** argv, unsigned int flags, e_engine_vtable* pVTable, void* pVTableUserData);


struct e_engine
{
    void* pUserData;
    unsigned int flags;
    int argc;
    char** argv;
    e_engine_vtable* pVTable;
    void* pVTableUserData;
    e_log* pLog;
    e_bool8 isOwnerOfLog;
    e_fs fs;
    e_config_file configFile;
    void* pGL;  /* Cast to GLBapi* to access OpenGL functions. */
    void* pVK;  /* Cast to VkbAPI* to access Vulkan functions. */
};

E_API e_result e_engine_init(const e_engine_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_engine** ppEngine);
E_API void e_engine_uninit(e_engine* pEngine, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_log* e_engine_get_log(e_engine* pEngine);
E_API e_result e_engine_run(e_engine* pEngine);
E_API e_result e_engine_exit(e_engine* pEngine, int exitCode);  /* Exits the main loop. */
E_API e_fs* e_engine_get_file_system(e_engine* pEngine);
E_API e_config_file* e_engine_get_config_file(e_engine* pEngine);
E_API e_bool32 e_engine_is_graphics_backend_supported(const e_engine* pEngine, e_graphics_backend backend);
E_API void* e_engine_get_glapi(const e_engine* pEngine);
E_API void* e_engine_get_vkapi(const e_engine* pEngine);
/* ==== END e_engine.h ==== */




/* ==== BEG e_window.h ==== */
typedef enum
{
    E_WINDOW_FLAG_FULLSCREEN = 0x01,
    E_WINDOW_FLAG_HIDDEN     = 0x02,
    E_WINDOW_FLAG_OPENGL     = 0x10     /* Configures the window to enable drawing to it via OpenGL. Does not create a rendering context. Will always be configured to use double buffering. */
} e_window_flags;

typedef enum
{
    E_WINDOW_EVENT_NONE,
    E_WINDOW_EVENT_CLOSE,
    E_WINDOW_EVENT_PAINT,
    E_WINDOW_EVENT_SIZE,
    E_WINDOW_EVENT_MOVE
} e_window_event_type;

typedef struct e_platform_window e_platform_window; /* Platform-specific window object. This is defined in the implementation on a per-platform basis. */

typedef struct e_window_event  e_window_event;
typedef struct e_window_vtable e_window_vtable;
typedef struct e_window_config e_window_config;
typedef struct e_window        e_window;

struct e_window_event
{
    e_window_event_type type;
    union
    {
        struct
        {
            int _unused;
        } close;
        struct
        {
            int _unused;
        } paint;
        struct
        {
            int x;
            int y;
        } size;
        struct
        {
            int x;
            int y;
        } move;
    } data;
};

struct e_window_vtable
{
    e_result (* onEvent)(void* pUserData, e_window* pWindow, e_window_event* pEvent);
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
/* ==== END e_window.h ==== */



/* ==== BEG e_math.h ==== */
#define E_PI    3.14159265358979323846
#define E_PIF   3.14159265358979323846f

E_INLINE double e_sqrtd (double x) { return sqrt(x); }
E_INLINE float  e_sqrtf (float  x) { return (float)e_sqrtd(x); }
E_INLINE float  e_rsqrtf(float  x) { return 1 / e_sqrtf(x); }   /* <-- This can be optimized. See miniaudio's implementation. */


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
} e_quat4f;


E_INLINE float e_degrees(float radians) { return radians * 57.29577951308232087685f; }
E_INLINE float e_radians(float degrees) { return degrees *  0.01745329251994329577f; }


E_INLINE e_vec4f e_vec4f_4f(float x, float y, float z, float w) { e_vec4f result = {x, y, z, w}; return result; }
E_INLINE e_vec4f e_vec4f_zero()                                 { return e_vec4f_4f(0, 0, 0, 0); }
E_INLINE e_vec4f e_vec4f_add(e_vec4f a, e_vec4f b)              { return e_vec4f_4f(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w); }
E_INLINE e_vec4f e_vec4f_sub(e_vec4f a, e_vec4f b)              { return e_vec4f_4f(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w); }
E_INLINE e_vec4f e_vec4f_mul(e_vec4f a, e_vec4f b)              { return e_vec4f_4f(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w); }
E_INLINE e_vec4f e_vec4f_mul_1f(e_vec4f a, float b)             { return e_vec4f_4f(a.x * b,   a.y * b,   a.z * b,   a.w * b  ); }
E_INLINE e_vec4f e_vec4f_div(e_vec4f a, e_vec4f b)              { return e_vec4f_4f(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w); }
E_INLINE float   e_vec4f_dot(e_vec4f a, e_vec4f b)              { return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w; }
E_INLINE float   e_vec4f_len(e_vec4f a)                         { return e_sqrtf(e_vec4f_dot(a, a)); }
E_INLINE e_vec4f e_vec4f_normalize(e_vec4f a)                   { return e_vec4f_mul_1f(a, e_rsqrtf(e_vec4f_dot(a, a))); }
E_INLINE e_vec4f e_vec4f_mul_mat4f(e_vec4f v, e_mat4f m)        { return e_vec4f_4f(e_vec4f_dot(m.c[0], v), e_vec4f_dot(m.c[1], v), e_vec4f_dot(m.c[2], v), e_vec4f_dot(m.c[3], v)); }


E_INLINE e_mat4f e_mat4f_init(e_vec4f c0, e_vec4f c1, e_vec4f c2, e_vec4f c3) { e_mat4f result; result.c[0] = c0; result.c[1] = c1; result.c[2] = c2; result.c[3] = c3; return result; }
E_INLINE e_mat4f e_mat4f_identity()                             { return e_mat4f_init(e_vec4f_4f(1, 0, 0, 0), e_vec4f_4f(0, 1, 0, 0), e_vec4f_4f(0, 0, 1, 0), e_vec4f_4f(0, 0, 0, 1)); }
E_INLINE e_mat4f e_mat4f_vulkan_clip_correction()               { return e_mat4f_init(e_vec4f_4f(1, 0, 0, 0), e_vec4f_4f(0, -1, 0, 0), e_vec4f_4f(0, 0, 0.5f, 0), e_vec4f_4f(0, 0, 0.5f, 1)); }
E_INLINE e_mat4f e_mat4f_translate(e_vec4f translation)         { return e_mat4f_init(e_vec4f_4f(1, 0, 0, 0), e_vec4f_4f(0, 1, 0, 0), e_vec4f_4f(0, 0, 1, 0), translation); }
E_INLINE e_mat4f mat4_scale(e_vec4f scale)                      { return e_mat4f_init(e_vec4f_4f(scale.x, 0, 0, 0), e_vec4f_4f(0, scale.y, 0, 0), e_vec4f_4f(0, 0, scale.z, 0), e_vec4f_4f(0, 0, 0, scale.w)); }

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

E_INLINE e_mat4f mat4_ortho(float left, float right, float bottom, float top, float znear, float zfar)
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
/* ==== END e_math.h ==== */





/* ==== BEG e_graphics.h ==== */
typedef enum
{
    E_GRAPHICS_SHADER_STAGE_VERTEX          = 0x00000001,
    E_GRAPHICS_SHADER_STAGE_TESS_CONTROL    = 0x00000002,
    E_GRAPHICS_SHADER_STAGE_TESS_EVALUATION = 0x00000004,
    E_GRAPHICS_SHADER_STAGE_GEOMETRY        = 0x00000008,
    E_GRAPHICS_SHADER_STAGE_FRAGMENT        = 0x00000010,
    E_GRAPHICS_SHADER_STAGE_COMPUTE         = 0x00000020
} e_graphics_shader_stage;

typedef enum
{
    E_VERTEX_INPUT_CLASS_VERTEX,
    E_VERTEX_INPUT_CLASS_INSTANCE
} e_vertex_input_class;

typedef enum
{
    E_PRIMITIVE_TOPOLOGY_POINT_LIST,
    E_PRIMITIVE_TOPOLOGY_LINE_LIST,
    E_PRIMITIVE_TOPOLOGY_LINE_STRIP,
    E_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    E_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
    E_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,
    E_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY,
    E_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY,
    E_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY,
    E_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY,
    E_PRIMITIVE_TOPOLOGY_PATCH_LIST
} e_primitive_topology;

typedef enum
{
    E_POLYGON_MODE_FILL,
    E_POLYGON_MODE_LINE,
    E_POLYGON_MODE_POINT
} e_polygon_mode;

typedef enum
{
    E_CULL_MODE_NONE,
    E_CULL_MODE_FRONT,
    E_CULL_MODE_BACK,
    E_CULL_MODE_FRONT_AND_BACK
} e_cull_mode;

typedef enum
{
    E_FRONT_FACE_CLOCKWISE,
    E_FRONT_FACE_COUNTER_CLOCKWISE
} e_front_face;

typedef enum
{
    E_COMPARE_OP_NEVER,
    E_COMPARE_OP_LESS,
    E_COMPARE_OP_EQUAL,
    E_COMPARE_OP_LESS_OR_EQUAL,
    E_COMPARE_OP_GREATER,
    E_COMPARE_OP_NOT_EQUAL,
    E_COMPARE_OP_GREATER_OR_EQUAL,
    E_COMPARE_OP_ALWAYS
} e_compare_op;

typedef enum
{
    E_STENCIL_OP_KEEP,
    E_STENCIL_OP_ZERO,
    E_STENCIL_OP_REPLACE,
    E_STENCIL_OP_INCREMENT_AND_CLAMP,
    E_STENCIL_OP_DECREMENT_AND_CLAMP,
    E_STENCIL_OP_INVERT,
    E_STENCIL_OP_INCREMENT_AND_WRAP,
    E_STENCIL_OP_DECREMENT_AND_WRAP
} e_stencil_op;

typedef enum
{
    E_BLEND_FACTOR_ZERO,
    E_BLEND_FACTOR_ONE,
    E_BLEND_FACTOR_SRC_COLOR,
    E_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
    E_BLEND_FACTOR_DST_COLOR,
    E_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
    E_BLEND_FACTOR_SRC_ALPHA,
    E_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    E_BLEND_FACTOR_DST_ALPHA,
    E_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
    E_BLEND_FACTOR_CONSTANT_COLOR,
    E_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
    E_BLEND_FACTOR_CONSTANT_ALPHA,
    E_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA,
    E_BLEND_FACTOR_SRC_ALPHA_SATURATE,
    E_BLEND_FACTOR_SRC1_COLOR,
    E_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR,
    E_BLEND_FACTOR_SRC1_ALPHA,
    E_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA
} e_blend_factor;

typedef enum
{
    E_BLEND_OP_ADD,
    E_BLEND_OP_SUBTRACT,
    E_BLEND_OP_REVERSE_SUBTRACT,
    E_BLEND_OP_MIN,
    E_BLEND_OP_MAX
} e_blend_op;

typedef enum
{
    E_LOGIC_OP_CLEAR,
    E_LOGIC_OP_AND,
    E_LOGIC_OP_AND_REVERSE,
    E_LOGIC_OP_COPY,
    E_LOGIC_OP_AND_INVERTED,
    E_LOGIC_OP_NO_OP,
    E_LOGIC_OP_XOR,
    E_LOGIC_OP_OR,
    E_LOGIC_OP_NOR,
    E_LOGIC_OP_EQUIVALENT,
    E_LOGIC_OP_INVERT,
    E_LOGIC_OP_OR_REVERSE,
    E_LOGIC_OP_COPY_INVERTED,
    E_LOGIC_OP_OR_INVERTED,
    E_LOGIC_OP_NAND,
    E_LOGIC_OP_SET
} e_logic_op;

typedef enum
{
    E_DESCRIPTOR_TYPE_SAMPLER,
    E_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
    E_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
    E_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    E_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
    E_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
    E_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    E_DESCRIPTOR_TYPE_STORAGE_BUFFER,
    E_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
    E_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
    E_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
} e_descriptor_type;

typedef enum
{
    E_IMAGE_LAYOUT_UNDEFINED,
    E_IMAGE_LAYOUT_GENERAL,
    E_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    E_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
    E_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
    E_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
    E_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
    E_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    E_IMAGE_LAYOUT_PREINITIALIZED,
    E_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL,
    E_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL,
    E_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
    E_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL,
    E_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL,
    E_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL,
    E_IMAGE_LAYOUT_READ_ONLY_OPTIMAL,
    E_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
    E_IMAGE_LAYOUT_PRESENT_SRC
} e_image_layout;

typedef enum
{
    E_ATTACHMENT_LOAD_OP_LOAD,
    E_ATTACHMENT_LOAD_OP_CLEAR,
    E_ATTACHMENT_LOAD_OP_DONT_CARE
} e_attachment_load_op;

typedef enum
{
    E_ATTACHMENT_STORE_OP_STORE,
    E_ATTACHMENT_STORE_OP_DONT_CARE
} e_attachment_store_op;

typedef enum
{
    E_PIPELINE_STAGE_TOP_OF_PIPE = 0x00000001,
    E_PIPELINE_STAGE_DRAW_INDIRECT = 0x00000002,
    E_PIPELINE_STAGE_VERTEX_INPUT = 0x00000004,
    E_PIPELINE_STAGE_VERTEX_SHADER = 0x00000008,
    E_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER = 0x00000010,
    E_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER = 0x00000020,
    E_PIPELINE_STAGE_GEOMETRY_SHADER = 0x00000040,
    E_PIPELINE_STAGE_FRAGMENT_SHADER = 0x00000080,
    E_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS = 0x00000100,
    E_PIPELINE_STAGE_LATE_FRAGMENT_TESTS = 0x00000200,
    E_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT = 0x00000400,
    E_PIPELINE_STAGE_COMPUTE_SHADER = 0x00000800,
    E_PIPELINE_STAGE_TRANSFER = 0x00001000,
    E_PIPELINE_STAGE_BOTTOM_OF_PIPE = 0x00002000,
    E_PIPELINE_STAGE_HOST = 0x00004000,
    E_PIPELINE_STAGE_ALL_GRAPHICS = 0x00008000,
    E_PIPELINE_STAGE_ALL_COMMANDS = 0x00010000,
} e_graphics_pipeline_stage;

typedef enum
{
    E_ACCESS_INDIRECT_COMMAND_READ = 0x00000001,
    E_ACCESS_INDEX_READ = 0x00000002,
    E_ACCESS_VERTEX_ATTRIBUTE_READ = 0x00000004,
    E_ACCESS_UNIFORM_READ = 0x00000008,
    E_ACCESS_INPUT_ATTACHMENT_READ = 0x00000010,
    E_ACCESS_SHADER_READ = 0x00000020,
    E_ACCESS_SHADER_WRITE = 0x00000040,
    E_ACCESS_COLOR_ATTACHMENT_READ = 0x00000080,
    E_ACCESS_COLOR_ATTACHMENT_WRITE = 0x00000100,
    E_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ = 0x00000200,
    E_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE = 0x00000400,
    E_ACCESS_TRANSFER_READ = 0x00000800,
    E_ACCESS_TRANSFER_WRITE = 0x00001000,
    E_ACCESS_HOST_READ = 0x00002000,
    E_ACCESS_HOST_WRITE = 0x00004000,
    E_ACCESS_MEMORY_READ = 0x00008000,
    E_ACCESS_MEMORY_WRITE = 0x00010000
} e_graphics_access_flag;

typedef enum
{
    E_DEPENDENCY_BY_REGION = 0x00000001,
    E_DEPENDENCY_VIEW_LOCAL = 0x00000002,
    E_DEPENDENCY_DEVICE_GROUP = 0x00000004
} e_graphics_dependency_flag;

/* The position semantic must be index 0. Other semantics are optional, but common ones for fixed function pipelines are pre-defined here. */
#define E_GRAPHICS_SHADER_SEMANTIC_POSITION 0
#define E_GRAPHICS_SHADER_SEMANTIC_NORMAL   1
#define E_GRAPHICS_SHADER_SEMANTIC_COLOR    2
#define E_GRAPHICS_SHADER_SEMANTIC_TEXCOORD 3

/* Color components for write masks. */
#define E_GRAPHICS_COLOR_COMPONENT_R        0x1
#define E_GRAPHICS_COLOR_COMPONENT_G        0x2
#define E_GRAPHICS_COLOR_COMPONENT_B        0x4
#define E_GRAPHICS_COLOR_COMPONENT_A        0x8
#define E_GRAPHICS_COLOR_COMPONENT_ALL      (E_GRAPHICS_COLOR_COMPONENT_R | E_GRAPHICS_COLOR_COMPONENT_G | E_GRAPHICS_COLOR_COMPONENT_B | E_GRAPHICS_COLOR_COMPONENT_A)


typedef struct e_graphics_vtable                        e_graphics_vtable;
typedef struct e_graphics_config                        e_graphics_config;
typedef struct e_graphics                               e_graphics;

typedef struct e_graphics_device_config                 e_graphics_device_config;
typedef struct e_graphics_device                        e_graphics_device;

typedef struct e_graphics_surface_config                e_graphics_surface_config;
typedef struct e_graphics_surface                       e_graphics_surface;

typedef struct e_graphics_descriptor_set_layout_config  e_graphics_descriptor_set_layout_config;
typedef struct e_graphics_descriptor_set_layout         e_graphics_descriptor_set_layout;
typedef struct e_graphics_descriptor_set                e_graphics_descriptor_set;

typedef struct e_graphics_descriptor_pool_config        e_graphics_descriptor_pool_config;
typedef struct e_graphics_descriptor_pool               e_graphics_descriptor_pool;

typedef struct e_graphics_render_pass_config            e_graphics_render_pass_config;
typedef struct e_graphics_render_pass                   e_graphics_render_pass;

typedef struct e_graphics_pipeline_config               e_graphics_pipeline_config;
typedef struct e_graphics_pipeline                      e_graphics_pipeline;

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

    /* e_graphics_render_pass */
    e_result    (* render_pass_alloc_size          )(void* pUserData, const e_graphics_render_pass_config* pConfig, size_t* pSize);
    e_result    (* render_pass_init                )(void* pUserData, e_graphics_render_pass* pRenderPass, const e_graphics_render_pass_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks);
    void        (* render_pass_uninit              )(void* pUserData, e_graphics_render_pass* pRenderPass, const e_allocation_callbacks* pAllocationCallbacks);

    /* e_graphics_descriptor_set_layout */
    e_result    (* descriptor_set_layout_alloc_size)(void* pUserData, const e_graphics_descriptor_set_layout_config* pConfig, size_t* pSize);
    e_result    (* descriptor_set_layout_init      )(void* pUserData, e_graphics_descriptor_set_layout* pDescriptorSetLayout, const e_graphics_descriptor_set_layout_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks);
    void        (* descriptor_set_layout_uninit    )(void* pUserData, e_graphics_descriptor_set_layout* pDescriptorSetLayout, const e_allocation_callbacks* pAllocationCallbacks);

    /* e_graphics_descriptor_pool */
    e_result    (* descriptor_pool_alloc_size      )(void* pUserData, const e_graphics_descriptor_pool_config* pConfig, size_t* pSize);
    e_result    (* descriptor_pool_init            )(void* pUserData, e_graphics_descriptor_pool* pDescriptorPool, const e_graphics_descriptor_pool_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks);
    void        (* descriptor_pool_uninit          )(void* pUserData, e_graphics_descriptor_pool* pDescriptorPool, const e_allocation_callbacks* pAllocationCallbacks);
    e_result    (* descriptor_pool_allocate        )(void* pUserData, e_graphics_descriptor_pool* pDescriptorPool, const e_graphics_descriptor_set_layout** pDescriptorSetLayouts, e_graphics_descriptor_set** ppDescriptorSets, const e_allocation_callbacks* pAllocationCallbacks);
    void        (* descriptor_pool_free            )(void* pUserData, e_graphics_descriptor_pool* pDescriptorPool, const e_graphics_descriptor_set** pDescriptorSets, const e_allocation_callbacks* pAllocationCallbacks);

    /* e_graphics_pipeline */                      
    e_result    (* pipeline_alloc_size             )(void* pUserData, const e_graphics_pipeline_config* pConfig, size_t* pSize);
    e_result    (* pipeline_init                   )(void* pUserData, e_graphics_pipeline* pPipeline, const e_graphics_pipeline_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks);
    void        (* pipeline_uninit                 )(void* pUserData, e_graphics_pipeline* pPipeline, const e_allocation_callbacks* pAllocationCallbacks);
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



typedef enum
{
    E_SUBPASS_DESCRIPTION_FLAG_NONE = 0x00000000
} e_graphics_subpass_description_flags;

typedef enum
{
    E_GRAPHICS_PIPELINE_BIND_POINT_GRAPHICS = 0,
    E_GRAPHICS_PIPELINE_BIND_POINT_COMPUTE = 1
} e_graphics_pipeline_bind_point;

typedef enum
{
    E_GRAPHICS_ATTACHMENT_FLAG_NONE = 0x00000000,
    E_GRAPHICS_ATTACHMENT_FLAG_MAY_ALIAS = 0x00000001
} e_graphics_attachment_flags;


typedef struct
{
    e_uint32 flags;
    e_format format;
    e_uint32 samples;
    e_attachment_load_op loadOp;
    e_attachment_store_op storeOp;
    e_attachment_load_op stencilLoadOp;
    e_attachment_store_op stencilStoreOp;
    e_image_layout initialLayout;
    e_image_layout finalLayout;
} e_graphics_attachment_description;

E_API e_graphics_attachment_description e_graphics_attachment_description_init_color(e_format format, e_uint32 samples, e_attachment_load_op loadOp, e_attachment_store_op storeOp, e_image_layout initialLayout, e_image_layout finalLayout);
E_API e_graphics_attachment_description e_graphics_attachment_description_init_depth_stencil(e_format format, e_uint32 samples, e_attachment_load_op depthLoadOp, e_attachment_store_op depthStoreOp, e_attachment_load_op stencilLoadOp, e_attachment_store_op stencilStoreOp, e_image_layout initialLayout, e_image_layout finalLayout);


typedef struct
{
    e_uint32 attachmentIndex;
    e_image_layout layout;
} e_graphics_attachment_reference;

E_API e_graphics_attachment_reference e_graphics_attachment_reference_init(e_uint32 attachmentIndex, e_image_layout layout);


typedef struct
{
    e_uint32 flags;
    e_graphics_pipeline_bind_point pipelineBindPoint;
    e_graphics_attachment_reference* pInputAttachments;
    e_uint32 inputAttachmentCount;
    e_graphics_attachment_reference* pColorAttachments;
    e_uint32 colorAttachmentCount;
    e_graphics_attachment_reference* pResolveAttachments;      /* Optional. If non-null, must have the same number of items as pColorAttachments. */
    e_graphics_attachment_reference* pDepthStencilAttachment;
    e_uint32* pPreserveAttachments;
    e_uint32 preserveAttachmentCount;
} e_graphics_subpass_config;

typedef struct
{
    e_uint32 srcSubpass;
    e_uint32 dstSubpass;
    e_uint32 srcStageMask;
    e_uint32 dstStageMask;
    e_uint32 srcAccessMask;
    e_uint32 dstAccessMask;
    e_uint32 dependencyFlags;
} e_graphics_subpass_dependency_config;

struct e_graphics_render_pass_config
{
    e_graphics_device* pDevice;
    e_graphics_attachment_description* pAttachments;
    e_uint32 attachmentCount;
    e_graphics_subpass_config* pSubpasses;
    e_uint32 subpassCount;
    e_graphics_subpass_dependency_config* pDependencies;
    e_uint32 dependencyCount;
};

E_API e_graphics_render_pass_config e_graphics_render_pass_config_init(e_graphics_device* pDevice);


struct e_graphics_render_pass
{
    e_graphics_device* pDevice;
    e_bool32 freeOnUninit;
};

E_API e_result e_graphics_render_pass_alloc_size(const e_graphics_render_pass_config* pConfig, size_t* pSize);
E_API e_result e_graphics_render_pass_init_preallocated(const e_graphics_render_pass_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_graphics_render_pass* pRenderPass);
E_API e_result e_graphics_render_pass_init(const e_graphics_render_pass_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_graphics_render_pass** ppRenderPass);
E_API void e_graphics_render_pass_uninit(e_graphics_render_pass* pRenderPass, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_graphics_device* e_graphics_render_pass_get_device(const e_graphics_render_pass* pRenderPass);




struct e_graphics_descriptor_set_layout
{
    e_graphics_device* pDevice;
    e_bool32 freeOnUninit;
};


typedef enum
{
    E_GRAPHICS_SHADER_FORMAT_NATIVE = 0,    /* Assumes the backend's native shader format. It's assumed that the caller will be providing valid shader code. */
    E_GRAPHICS_SHADER_FORMAT_SPIRV  = 1,
    E_GRAPHICS_SHADER_FORMAT_GLSL   = 2
} e_graphics_shader_format;

typedef struct
{
    e_graphics_shader_stage stage;
    e_graphics_shader_format format;
    const void* pShaderCode;
    size_t shaderCodeSize;
    const char* pEntryPoint;
} e_graphics_pipeline_stage_config;

E_API e_graphics_pipeline_stage_config e_graphics_pipeline_stage_config_init(e_graphics_shader_stage stage, e_graphics_shader_format format, const void* pShaderCode, size_t shaderCodeSize, const char* pEntryPoint);



typedef struct
{
    e_uint32 binding;
    e_uint32 stride;
    e_vertex_input_class inputClass;
    e_uint32 instanceStepRate;
} e_graphics_vertex_input_binding_config;

E_API e_graphics_vertex_input_binding_config e_graphics_vertex_input_binding_config_init_ex(e_uint32 binding, e_uint32 stride, e_vertex_input_class inputClass, e_uint32 instanceStepRate);
E_API e_graphics_vertex_input_binding_config e_graphics_vertex_input_binding_config_init(e_uint32 binding, e_uint32 stride);


typedef struct
{
    e_uint32 location;  /* Location of the attribute in the vertex shader. This is the equivalent to Direct3Ds semantics. The position must be location 0. Corresponds to "layout(location = n) ..." in GLSL. */
    e_uint32 binding;   /* The binding slot of the vertex buffer. Equivalent to Direct3Ds "InputSlot". */
    e_format format;
    e_uint32 offset;
} e_graphics_vertex_input_config;

E_API e_graphics_vertex_input_config e_graphics_vertex_input_config_init(e_uint32 location, e_uint32 binding, e_format format, e_uint32 offset);


typedef struct
{
    e_bool32 enableBlend;           /* Defaults to false. */
    e_blend_factor srcColorFactor;  /* Defaults to E_BLEND_FACTOR_ONE. */
    e_blend_factor dstColorFactor;  /* Defaults to E_BLEND_FACTOR_ZERO. */
    e_blend_op colorOp;             /* Defaults to E_BLEND_OP_ADD. */
    e_blend_factor srcAlphaFactor;  /* Defaults to E_BLEND_FACTOR_ONE. */
    e_blend_factor dstAlphaFactor;  /* Defaults to E_BLEND_FACTOR_ZERO. */
    e_blend_op alphaOp;             /* Defaults to E_BLEND_OP_ADD. */
    e_uint32 colorWriteMask;        /* Defaults to E_COLOR_COMPONENT_ALL. */
} e_graphics_color_blend_config;

typedef struct
{
    e_uint32 binding;
    e_descriptor_type descriptorType;
    e_uint32 descriptorCount;
    e_uint32 shaderStages;
} e_graphics_descriptor_set_layout_binding_config;


struct e_graphics_pipeline_config
{
    e_graphics_device* pDevice;
    e_graphics_pipeline_stage_config* pStages;
    e_uint32 stageCount;
    e_graphics_vertex_input_binding_config* pVertexInputBindings;
    e_uint32 vertexInputBindingCount;
    e_graphics_vertex_input_config* pVertexInputs;
    e_uint32 vertexInputCount;
    struct
    {
        e_primitive_topology topology;      /* Defaults to E_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST */
        e_bool32 enablePrimitiveRestart;    /* Defaults to false. */
    } inputAssembly;
    struct
    {
        e_uint32 patchControlPoints;        /* Defaults to 0. Must be set explicitly if you're using tessellation. */
    } tesselation;
    struct
    {
        e_bool32 enableDepthClamp;          /* Defaults to false. */
        e_bool32 enableRasterizerDiscard;   /* Defaults to false. */
        e_polygon_mode polygonMode;         /* Defaults to E_POLYGON_MODE_FILL */
        e_cull_mode cullMode;               /* Defaults to E_CULL_MODE_BACK */
        e_front_face frontFace;             /* Defaults to E_FRONT_FACE_COUNTER_CLOCKWISE */
        e_bool32 enableDepthBias;           /* Defaults to false. */
        float depthBiasConstantFactor;      /* Defaults to 0. */
        float depthBiasClamp;               /* Defaults to 1. */
        float depthBiasSlopeFactor;         /* Defaults to 0. */
    } rasterization;
    struct
    {
        e_uint32 rasterizationSamples;      /* The number of samples to use for rasterization. Must be a power of 2. Defaults to 1. */
        e_bool32 enableSampleShading;       /* Enables multisample shading. Defaults to false. */
        e_uint32 minSampleShading;          /* The minimum fraction of sample shading. Defaults to 1. Only used if enableSampleShading is set to true. */
        e_bool32 enableAlphaToCoverage;     /* Defaults to false. */
        e_bool32 enableAlphaToOne;          /* Defaults to false. */
    } multisample;
    struct
    {
        e_bool32 enableDepthTest;           /* Defaults to false. */
        e_bool32 enableDepthWrite;          /* Defaults to false. */
        e_compare_op depthCompareOp;        /* Defaults to E_COMPARE_OP_LESS_OR_EQUAL */
        e_bool32 enableDepthBoundsTest;     /* Defaults to false. */
        float minDepthBounds;               /* Defaults to 0. */
        float maxDepthBounds;               /* Defaults to 1. */
        e_bool32 enableStencilTest;         /* Defaults to false. */
        struct
        {
            e_stencil_op failOp;            /* Defaults to E_STENCIL_OP_KEEP */
            e_stencil_op passOp;            /* Defaults to E_STENCIL_OP_KEEP */
            e_stencil_op depthFailOp;       /* Defaults to E_STENCIL_OP_KEEP */
            e_compare_op compareOp;         /* Defaults to E_COMPARE_OP_ALWAYS */
            e_uint32 compareMask;           /* Defaults to 0xFFFFFFFF */
            e_uint32 writeMask;             /* Defaults to 0xFFFFFFFF */
            e_uint32 reference;             /* Defaults to 0 */
        } front;
        struct
        {
            e_stencil_op failOp;            /* Defaults to E_STENCIL_OP_KEEP */
            e_stencil_op passOp;            /* Defaults to E_STENCIL_OP_KEEP */
            e_stencil_op depthFailOp;       /* Defaults to E_STENCIL_OP_KEEP */
            e_compare_op compareOp;         /* Defaults to E_COMPARE_OP_ALWAYS */
            e_uint32 compareMask;           /* Defaults to 0xFFFFFFFF */
            e_uint32 writeMask;             /* Defaults to 0xFFFFFFFF */
            e_uint32 reference;             /* Defaults to 0 */
        } back;
    } depthStencil;
    struct
    {
        e_graphics_color_blend_config* pColorBlendConfigs;
        e_uint32 colorBlendConfigCount;
        e_bool32 enableLogicOp;             /* Defaults to false. */
        e_logic_op logicOp;                 /* Defaults to E_LOGIC_OP_COPY. */
        float blendConstants[4];            /* Defaults to { 0, 0, 0, 0 }. */
    } colorBlend;
    e_graphics_descriptor_set_layout* pDescriptorSetLayouts;
    e_uint32 descriptorSetLayoutCount;
    e_graphics_render_pass* pRenderPass;    /* Cannot be null. */
    e_uint32 subpassIndex;                  /* Defaults to 0. */
};

E_API e_graphics_pipeline_config e_graphics_pipeline_config_init(e_graphics_device* pDevice);


struct e_graphics_pipeline
{
    e_graphics_device* pDevice;
    e_bool32 freeOnUninit;
};

E_API e_result e_graphics_pipeline_alloc_size(const e_graphics_pipeline_config* pConfig, size_t* pSize);
E_API e_result e_graphics_pipeline_init_preallocated(const e_graphics_pipeline_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_graphics_pipeline* pPipeline);
E_API e_result e_graphics_pipeline_init(const e_graphics_pipeline_config* pConfig, const e_allocation_callbacks* pAllocationCallbacks, e_graphics_pipeline** ppPipeline);
E_API void e_graphics_pipeline_uninit(e_graphics_pipeline* pPipeline, const e_allocation_callbacks* pAllocationCallbacks);
E_API e_graphics_device* e_graphics_pipeline_get_device(const e_graphics_pipeline* pPipeline);
/* ==== END e_graphics.h ==== */




/* ==== BEG e_client.h ==== */
typedef enum
{
    E_CLIENT_FLAG_NO_WINDOW     = 0x01,   /* Does not create a window. This also disables graphics. */
    E_CLIENT_FLAG_NO_GRAPHICS   = 0x02,   /* Disables the graphics sub-system. Useful if you want to implement your own graphics system rather than using the default. */
    E_CLIENT_FLAG_NO_AUDIO      = 0x04,   /* Disables the audio sub-system. */
    E_CLIENT_FLAG_WINDOW_OPENGL = 0x10    /* Enables OpenGL on the window. Only used when graphics are disabled by the engine or the client (E_CLIENT_NO_GRAPHICS). */
} e_client_flags;

typedef struct e_client_vtable e_client_vtable;
typedef struct e_client_config e_client_config;
typedef struct e_client        e_client;


typedef enum
{
    E_CLIENT_EVENT_WINDOW_CLOSE,
    E_CLIENT_EVENT_WINDOW_SIZE,
    E_CLIENT_EVENT_WINDOW_MOVE,
    E_CLIENT_EVENT_CURSOR_MOVE,
    E_CLIENT_EVENT_CURSOR_BUTTON_DOWN,
    E_CLIENT_EVENT_CURSOR_BUTTON_UP,
} e_client_event_type;

typedef struct
{
    e_client_event_type type;
    struct
    {
        struct
        {
            e_window* pWindow;
        } windowClose;
        struct
        {
            e_window* pWindow;
            int x;
            int y;
        } windowSize, windowMove;
        struct
        {
            e_window* pWindow;
            int x;
            int y;
        } cursorMove;
        struct
        {
            e_window* pWindow;
            int x;
            int y;
            int button;
            unsigned int flags;
        } cursorButtonDown, cursorButtonUp;
    } data;
} e_client_event;


struct e_client_vtable
{
    e_result (* onEvent)(void* pUserData, e_client* pClient, e_client_event* pEvent);
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
    e_graphics* pGraphics;
    e_graphics_device* pGraphicsDevice;
    e_graphics_surface* pGraphicsSurface;
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
E_API e_result e_client_default_event_handler(e_client* pClient, e_client_event* pEvent);
E_API e_result e_client_step(e_client* pClient, double dt);
/* ==== END e_client.h ==== */



/* ==== BEG e_editor.h ==== */
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
/* ==== END e_editor.h ==== */

#endif  /* e_h */
