/*
 *
 * Debugging tools.
 *
 * Copyright(C) 2018 by Shigeo Kobayashi(shigeo@tinyforest.jp)
 *
 */
#include "stdafx.h"
#include "debug.h"

#ifdef _DEBUG
void AssertProc(int f,const char *file,int line)
{
	if(f) return;
	printf("** Assertion failed %s:%d **\n",file,line);
	(void)getchar();
}


//
// if f==TRUE then nothing is done,or print arguments after f in printf format.
//
void T_(int f, const char* psz, ...)
{
	va_list arg;

	if (f) return;
	va_start(arg, psz);
	vprintf(psz, arg);
	va_end(arg);
}

#endif
