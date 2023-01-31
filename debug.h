/*
 *
 * Debugging tools.
 *
 * Copyright(C) 2018 by Shigeo Kobayashi(shigeo@tinyforest.jp)
 *
 */
#ifndef __INC_DEBUG__
#define  __INC_DEBUG__

#include <stdio.h>

#ifdef _DEBUG
void T_(int f, const char* psz, ...);
extern void AssertProc(int f,const char *file,int line);
#define ASSERT(f) AssertProc(f,__FILE__,__LINE__)

// use as TRACE(("Val="%d",Val))  NEW LINE automatically added!!
#define TRACE(s)  {printf(" -- TRACE(%s:%d) -- \t[",__FILE__,__LINE__);printf s;printf("]\n");}
#else
#define ASSERT(f)
#define TRACE(s)
#endif

#endif
