/*
////////////////////////////////////////////////////////////////////////////////////////
///                                                                                  ///
///  Copyright(C) 2023 Shigeo Kobayashi (shigeo@tinyforest.jp). All Rights Reserved. ///
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
   #define BIT64
   #define BIT_SIZE 64
 #else
   #define BIT32
   #define BIT_SIZE 32
 #endif
#else
 #define LINUX
 #ifdef __x86_64__
   #define BIT64
   #define BIT_SIZE 64
 #else
  #define BIT32
  #define BIT_SIZE 32
 #endif
#endif

#ifndef BIT_SIZE
#error ***** Undefined C++ compiler. *****
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

#ifdef WINDOWS /**** WINDOWS ***/
  #define EXPORT(t)  __declspec(dllexport) t __stdcall
#endif         /*--- WINDOWS ---*/

#ifdef LINUX   /**** LINUX   ****/
  #define EXPORT(t) __attribute__((visibility ("default")))  t
#endif         /*--- LINUX   ---*/

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
typedef int  (*FSA_ERROR_HANDLER) (
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
EXPORT(int) FsaWriteStream(FSA_HANDLE h, U_INT i, void* buff,U_LONG from, U_INT bytes);
EXPORT(int) FsaReadStream (FSA_HANDLE h, U_INT i, void* buff,U_LONG from, U_INT bytes);
EXPORT(int) FsaTrimStream(FSA_HANDLE h, U_INT i, U_LONG from);
EXPORT(int) FsaClose(FSA_HANDLE* h);

/*
 *     Utility/Information routines .
 */
EXPORT(int) FsaGetStreamSize(FSA_HANDLE h, U_INT i, U_LONG* size);
EXPORT(int) FsaGetMaxStreamCount(FSA_HANDLE h, U_INT *cs);
EXPORT(int) FsaExtendMaxStreamCount(FSA_HANDLE h, U_INT cs);
EXPORT(int) FsaGetFsaTag(FSA_HANDLE h, U_LONG* tag);
EXPORT(int) FsaSetFsaTag(FSA_HANDLE h, U_LONG  tag);
EXPORT(int) FsaGetStreamTag(FSA_HANDLE h, U_INT i, U_LONG* tag);
EXPORT(int) FsaSetStreamTag(FSA_HANDLE h, U_INT i, U_LONG  tag);
EXPORT(int) FsaSetErrHandler(FSA_HANDLE h, FSA_ERROR_HANDLER f);
EXPORT(int) FsaGetErrHandler(FSA_HANDLE h, FSA_ERROR_HANDLER *pf);
EXPORT(int) FsaGetOpenMode(FSA_HANDLE h, unsigned char *pmode);


#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __FSA_H__ */
