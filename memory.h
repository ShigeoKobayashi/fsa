//
//****************************************************************************
//*                                                                          *
//* Å@Copyright (C) Shigeo Kobayashi, 2016.									 *
//*                 All rights reserved.									 *
//*                                                                          *
//****************************************************************************
//
//
#ifndef ___INC_MEMORY_H___
#define ___INC_MEMORY_H___

// #define MAX_PATH_SIZE _MAX_PATH + 1


#define SZ_ALLOC_COPY(st,so) {int len=strlen(so); st=(char*)MemAlloc(sizeof(char)*(len+2));if(st) strcpy_s(st,len+1,so);}

//
// Utility routines in dllmain.cpp
//

class CMemory 
{
public:
	CMemory()
	{
		m_erCode = 0;
		m_errno  = 0;
	};
	
	void *MemAlloc(U_INT cb);
	void *MemReAlloc(void *p,U_INT n);
	void  MemFree(void **p);
	void  MemCheck(); // Confirm if every is memory freed.
	char *StrAllocCopy(char *sin);
	void  StrLimitCopy(char *so,int maxo,const char *si);

private:
	int   m_erCode;
	int   m_errno;
};



#endif //___BT_BASE_H___
