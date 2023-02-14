
//
//****************************************************************************
//*                                                                          *
//*              Copyright (C) Shigeo Kobayashi, 2022.	                     *
//*                            All rights reserved.	                         *
//*                                                                          *
//****************************************************************************
//
//
// standard c/c++ includes
//

#ifdef _WIN32
//
// ====== WINDOWS specific part =========
//
#pragma once
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#endif // WINDOWS

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

#include "error.h"
#include "debug.h"

#include "fsa.h"

#include "blockio.h"
#include "fileio.h"
#include "header.h"
#include "directory.h"
#include "sat.h"
#include "stream.h"

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


