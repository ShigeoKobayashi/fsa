/*
 *
 * Error handling tools.
 *
 * Copyright(C) 2022 by Shigeo Kobayashi(shigeo@tinyforest.jp)
 *
 */
#ifndef __INC_ERROR__
#define  __INC_ERROR__

#include "stdafx.h"

#define ER_THROW(h,e,s) throw new CErrException(h,e,s,__FILE__,__LINE__)
#define CHECK(f,e,s) if(!(f)) ER_THROW((void *)this,e,s);

 /*
   Error handling.
 */
 // Error handler

class CErrException
{
public:
	CErrException(void* h, int code, const char *desc,const char* file_, int line_)
	{

#ifdef _DEBUG
		printf(" ***** CDbgException(%d,%s,%d) ******\n", code, file_, line_);
		(void)getchar();
#endif
		handle        = h;
		fsa_code      = code;
		msg           = desc;
		source_file   = file_;
		source_line   = line_;
		err_no        = errno;
	};

	~CErrException()
	{
	};

public:
	void* handle;
	int   fsa_code;
	const char* msg;
	const char* source_file;
	int   source_line;
	int   err_no;
};

#endif
