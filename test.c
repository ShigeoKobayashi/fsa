#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "fsa.h"

#define TRACE(s)  {printf(" -- TRACE(%s:%d)[",__FILE__,__LINE__);printf s;printf("]\n");}
#define ASSERT(f) if(!(f)) {printf("ASSERTION FAILED");(void)getchar();}

void ErrorFunc(
	FSA_HANDLE  handle,
	const char* function,     /* Function name an error detected */
	int         fsa_code,     /* The error code that FSA function detected(if this is ERROR_FSA_UNDEFINED,then infomations
								 after err_no may be undefined. */
	int         err_no,       /* copy of C system's errno. Use C standard function strerror(err_no) for detail. */
	const char* msg,          /* The error message that FSA function can describe. */
	const char* source_file,  /* The source file the error occured. */
	int         source_line   /* The source line the error occured. */
) {
	printf(" Error: f=%s code=%d c_er=%d msg=%s file=%s line=%d\n", function, fsa_code, err_no, msg, source_file, source_line);
	if(err_no!=0) printf("        c error=%d:%s\n",err_no,strerror(err_no));
	(void)getchar();
}

#define V_TYPE unsigned short
int main()
{
	FSA_HANDLE h;
	U_LONG offset;
	U_INT max_i=1024*500;
	V_TYPE ch1,ch2;
	U_INT i;

	FsaOpen(&h, "test.fsa", 'T');
	FsaSetErrHandler(h, ErrorFunc);

	for (i= 0; i < max_i; ++i) {
		ch1 = (V_TYPE)i;
		FsaGetStreamSize(h, 0, &offset);
		FsaWriteStream(h, 0, &ch1, offset+1, sizeof(ch1));
		FsaGetStreamSize(h, 3, &offset);
		FsaWriteStream(h, 3, &ch1, offset, sizeof(ch1));
	}
	FsaClose(&h);

	FsaOpen(&h, "test.fsa", 'R');
	FsaSetErrHandler(h, ErrorFunc);
	for (i = 0; i < max_i; ++i) {
		ch1 = (V_TYPE)i;
		ch2 = 0;
		FsaReadStream(h, 0, &ch2, (U_LONG)i * sizeof(ch2), sizeof(ch2));
		ASSERT(ch1==ch2);
		ch2 = 0;
		FsaReadStream(h, 3, &ch2, (U_LONG)i * sizeof(ch2), sizeof(ch2));
		ASSERT(ch1 == ch2);
	}
	FsaClose(&h);
}
