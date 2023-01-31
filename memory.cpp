///////////////////////////////////////////////////////////////////////
//
// Copyright(C) 2016 Shigeo Kobayashi. All Rights Reserved.
// 
///////////////////////////////////////////////////////////////////////
//
#include "stdafx.h"
#include "memory.h"

//
// Basic memory handler.
//

static int mem_counter = 0;


//
// Memory allocation
void *
	CMemory::MemAlloc(U_INT cb)
{
	ASSERT(cb>0);

	void *p = calloc(cb,sizeof(char));
	CHECK(p!=NULL, ERROR_MEMORY_ALLOC,"MemAlloc() failed.");
	mem_counter++;
	return p;
}

void *
	CMemory::MemReAlloc(void *p,U_INT n)
{
	ASSERT(p!=NULL);
	void *ps = realloc(p,n);
	CHECK(ps!=NULL, ERROR_MEMORY_ALLOC,"MemReAlloc() failed.");
	return ps;
}

void 
	CMemory::MemFree(void **p)
{
	void *pm;
	if(p ==NULL) return;
	pm = *p;
	if(pm==NULL) return;
	mem_counter--;
	ASSERT(mem_counter>=0);
	free(pm);
	*p = NULL;
}

void 
	CMemory::MemCheck()
{
	ASSERT(mem_counter==0);
}

char *
	CMemory::StrAllocCopy(char *sin)
{
	char *st;
	int len=strlen(sin);
	st = (char*)MemAlloc(len+2);
	strcpy(st,sin);
	return st;
}

void 
	CMemory::StrLimitCopy(char *so,int maxo,const char *si)
{
	int  cb = maxo-1;
	char ch;
	
	for(int i=0;i<cb;++i)
	{
		ch = *si++;
		if(ch==0) break;
		*so++ = ch;
	}
	*so = 0;
}
