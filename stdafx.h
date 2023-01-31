
//
//****************************************************************************
//*                                                                          *
//* Å@Copyright (C) Shigeo Kobayashi, 2022.									 *
//*                 All rights reserved.									 *
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

#include "fsa.h"
#include "error.h"

#include "debug.h"
#include "blockio.h"
#include "fileio.h"
#include "header.h"
#include "directory.h"
#include "sat.h"
#include "stream.h"
