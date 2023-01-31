/*
////////////////////////////////////////////////////////////////////////////////////////
///                                                                                  ///
///  Copyright(C) 2022 Shigeo Kobayashi (shigeo@tinyforest.jp). All Rights Reserved. ///
///                                                                                  ///
////////////////////////////////////////////////////////////////////////////////////////
*/

#ifndef __FSA_H__
#define __FSA_H__

/*
 * Find environment 
 */
#undef BIT_SIZE
#ifdef _WIN32
 #define WINDOWS
 #ifdef _WIN64
   /* 64bit Windows */
   #define BIT64
   #define BIT_SIZE 64
 #else
   /* 32bit Windows */
   #define BIT32
   #define BIT_SIZE 32
 #endif
#else
 #define LINUX
 #ifdef __x86_64__
   /* 64bit Linux */
   #define BIT64
   #define BIT_SIZE 64
 #else
  /* 32bit Linux */
  #define BIT32
  #define BIT_SIZE 32
 #endif
#endif

#ifndef BIT_SIZE
#error ***** Undefined Windows or Linux C++ compiler. *****
#endif

#define S_SHORT           short
#define U_SHORT  unsigned short
#define S_INT             int
#define U_INT    unsigned int
#define S_LONG            long long int
#define U_LONG   unsigned long long int

#define Nil      ((U_LONG )0)
#define IsNil(a) (((U_LONG )a)==Nil)

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef WINDOWS /**** WINDOWS ****/
  /* WINDOWS specific part (Same define's must be defined for other platforms.) */
  #define EXPORT(t)  __declspec(dllexport) t __stdcall
  /* Note:
   __stdcall __cdecl and C#
   ------------------------
   WIN32 APIs use __stdcall(which can't be used for vararg functions) with .def file representation.
   __stdcall without .def file changes function name exported in .lib file.
   __cdecl (c compiler default) never changes function name exported but consumes more memories than __stdcall.
   C# [Dllexport] atrribute uses __stdcall in default.
   To call __cdecl functions from C#, use CallingConvention.Cdecl like "[DllImport("MyDLL.dll", CallingConvention = CallingConvention.Cdecl)]".
  */
#endif /**** WINDOWS ****/

#ifdef LINUX /******** LINUX ********/
  /* gcc/g++ specific part ==> compiled with '-fPIC -fvisibility=hidden' option.
     -fvisibility=hidden option hides everything except explicitly declared as exported API just like
     as Windows' dll.
  */
  #define EXPORT(t) __attribute__((visibility ("default")))  t
#endif /**** LINUX ****/

/* 
 *   Error codes.  
 */
#define ERROR_MEMORY_ALLOC          -1   /*  Memory allocation error */
#define ERROR_FILE_OPEN             -2   /*  File open error   */
#define ERROR_FILE_DUPLICATED_OPEN  -3   /*  Duplicated open (file already opened can not be re-opened) */
#define ERROR_FILE_OPEN_MODE        -4   /*  Invalid open mode specified */
#define ERROR_FILE_READ             -5   /*  File read failed  */
#define ERROR_FILE_WRITE            -6   /*  File write failed */
#define ERROR_FILE_SEEK             -7   /*  File seek failed  */
#define ERROR_FSA_ILLEGAL           -8   /*  Illegal file opened (not a FSA file) */
#define ERROR_FSA_READ              -9   /*  Read error */
#define ERROR_FSA_WRITE            -10   /*  Write error */
#define ERROR_FSA_STREAM           -11   /*  Illegal operation(stream index out of range or access beyond the limit). */
#define ERROR_FSA_UNDEFINED       -999   /*  undefined ERROR! */

#define FSA_HANDLE void *

/* Error handler */
typedef void   (*ErrHandler) (
        FSA_HANDLE  handle,
        const char *function,     /* Function name an error detected */
        int         fsa_code,     /* The error code that FSA function detected(if this is ERROR_FSA_UNDEFINED,then infomations
                                     after err_no may be undefined. */
        int         err_no,       /* copy of C system's errno. Use C standard function strerror(err_no) for detail. */
        const char* msg,          /* The error message that FSA function can describe. */
        const char* source_file,  /* The source file the error occured. */
        int         source_line   /* The source line the error occured. */
    );
/*
 *     Open/Close & I/O operations.
 */
EXPORT(int) FsaOpen(FSA_HANDLE *h,const char* file, const char mode);
EXPORT(int) FsaWriteStream(FSA_HANDLE h, U_INT i, void* buff,U_LONG from, S_INT bytes);
EXPORT(int) FsaReadStream (FSA_HANDLE h, U_INT i, void* buff,U_LONG from, S_INT bytes);
EXPORT(int) FsaClose(FSA_HANDLE* h);

/*
 *     Utility/Info routines .
 */
EXPORT(int) FsaGetStreamSize(FSA_HANDLE h, U_INT i, U_LONG* size);
EXPORT(int) FsaGetMaxStreamCount(FSA_HANDLE h, U_INT *cs);
EXPORT(int) FsaExtendMaxStreamCount(FSA_HANDLE h, U_INT cs);

EXPORT(int) FsaGetHeaderTag(FSA_HANDLE h, U_LONG* tag);
EXPORT(int) FsaSetHeaderTag(FSA_HANDLE h, U_LONG  tag);
EXPORT(int) FsaGetStreamTag(FSA_HANDLE h, U_INT i, U_LONG* tag);
EXPORT(int) FsaSetStreamTag(FSA_HANDLE h, U_INT i, U_LONG  tag);
EXPORT(int) FsaSetErrHandler(FSA_HANDLE h, ErrHandler f);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __FSA_H__ */
