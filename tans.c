/*
 *    tans (simple FSA test program)     
 *           Copyright(C) 2023 Shigeo Kobayashi. All Rights Reserved.
 */

#define _CRT_SECURE_NO_WARNINGS 1  /* May be for Windows only */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include "fsa.h"

/* Error handler definition. */
int ErrorFunc(
	/* All arguments are set by FSA */
	FSA_HANDLE  handle,       /* FSA_HANDLE currently running.   */
	const char* function,     /* FSA function name the error detected */
	int         fsa_code,     /* FSA error code(see fsa.h) that FSA function detected.
									 (If this is FSA_ERROR_UNDEFINED,then infomations after err_no may be undefined. */
	int         err_no,       /* copy of C system's errno. Use C standard function strerror(err_no) for detail. */
	const char* msg,          /* The error message that FSA function can describe. */
	const char* source_file,  /* The source file the error occured. */
	int         source_line   /* The source line the error occured. */
	)
{
	char ch;
	printf(" FSA Error: f=%s code=%d c_er=%d msg=%s file=%s line=%d\n", function, fsa_code, err_no, msg, source_file, source_line);
	if(err_no!=0) printf("        c error=%d:%s\n",err_no,strerror(err_no));
	printf("Continue or exit (c|e)");
	do {
		printf("?");
		ch = toupper(getchar());
	} while (ch != 'C' && ch != 'E');
	if (ch == 'E') {
		FsaClose(&handle);
		exit(EXIT_FAILURE);
	}
	return fsa_code;
}

#define  MAX_BUFFER  2048
unsigned char gBuffer[MAX_BUFFER];
FSA_HANDLE    gh;
char          gchMode = 0;
char          gCommand;
int           gIndex;
char          gFile[1024];
int           gRead = 0;

U_LONG        gTansId;
const char    gId[] = { 'T','A','N','S',0,0,0,0 };

int Remove(char* path)
{
	struct stat statBuf;
	if (stat(path, &statBuf) == 0) {
		char ch;
		printf("File %s to be opened with 'T' mode exists, OK to truncate (y|n)",path);
		do {
			printf("?");
			ch = toupper(getchar());
		} while (ch != 'Y' && ch != 'N');
		if (ch == 'N') return 0;
	}
	return 1; /* OK to delete */
}

void Help()
{
	printf("Command line: tans (t|n|w|r|?) [FSA_file]\n");
	printf("   (t|n|w|r) can be capital letter:\n");
	printf("    t ... Create FSA_file for read and write mode.\n");
	printf("          If the file exists,then it is truncated.\n");
	printf("    n ... Create new FSA_file for read and write mode.\n");
	printf("          If the file exists,then the command fails.\n");
	printf("    w ... Open existing FSA_file for read and write mode.\n");
	printf("          If the file does not exist,then the command fails.\n");
	printf("    r ... Open existing FSA_file for read only mode.\n");
	printf("          If the file does not exist,then the command fails.\n");
	printf("    ? ... Print this help.\n");
	printf("\n");
	printf("  Instructions for tans: ");
	printf("   [l|i|e|w|r|q] can be capital letter:\n");
	printf("    > l          ... List stored file names with the stream index.\n");
	printf("    > i          ... Print the stream index range of the FSA_file.\n");
	printf("    > e n        ... Extend the stream index size at least n.\n");
	printf("    > w i [file] ... Write contents of the i-th stream to file.\n");
	printf("                     If the file is not given,then stored file name will be used instead.\n");
	printf("    > r i file   ... Write contents of the file to the i-th stream.\n");
	printf("    > q          ... Quit the program(tans).\n");
	printf("    > ?          ... Print this help.\n");
}

void PrintIndex()
{
	int mi;
	FsaGetMaxStreamCount(gh,&mi);
	printf(" Index range: 0 - %d\n", mi - 1);
}

int GetIndex(char *pcmd, char** pfile)
{
	int ix = 0;
	char ch = *(++pcmd);
	if (!isdigit(ch)) {	printf("Error: index not given.\n"); return -1;}
	do {ix = ix * 10 + (ch - '0');} while (isdigit(ch = *(++pcmd)));
	*pfile = pcmd;
	return ix;
}

void PrintList() 
{
	U_INT  max;
	U_INT  i,cb;
	U_LONG size;
	FsaGetMaxStreamCount(gh, &max);
	for (i = 0; i < max; ++i) {
		FsaGetStreamSize(gh,i, &size);
		if (size <= 0) continue;
		cb = (size < 512) ? (U_INT)size : 512;
		FsaReadStream(gh, i,gBuffer,0,cb);
		cb = strlen(gBuffer)+1;
		printf("   %d) %s[%lld bytes]\n", i, gBuffer, size - cb);
	}
}

void Extend(char* pcmd)
{
	char* pfile;
	if (gRead) { printf("Error: FSA file opened read only."); return; }
	int mi = GetIndex(pcmd, &pfile);
	if (mi < 0) {PrintIndex(); return;}
	FsaExtendMaxStreamCount(gh, mi);
	FsaGetMaxStreamCount(gh, &mi);
	printf(" New index range: 0 - %d\n", mi - 1);
}

void Write(char *pcmd)
{
	FILE* fp;
	char* pfile;
	U_LONG size,from;
	U_INT  nc;
	int ix = GetIndex(pcmd, &pfile);
	if (ix < 0) return;
	FsaGetStreamSize(gh, ix, &size);
	if (size <= 0) {printf("Error: Index %d is empty\n)", ix); return;}
	FsaReadStream(gh, ix, gBuffer, 0, (size>1024)?1024: (U_INT)size);
	if (!(pfile[0])) {
		if (!Remove(gBuffer)) return;
		fp = fopen(gBuffer, "wb");
		if (!fp) { printf("Error: File %s can not be opened.\n", gBuffer); return; }
	}
	else {
		if (!Remove(pfile)) return;
		fp = fopen(pfile, "wb");
		if (!fp) { printf("Error: File %s can not be opened.\n", pfile); return; }
	}
	nc = strlen(gBuffer);
	FsaGetStreamSize(gh, ix, &size);
	from  = (U_LONG)nc + 1;
	size -= from;
	while (size > 0) {
		nc = MAX_BUFFER;
		if (nc > size) nc = (U_INT)size;
		FsaReadStream(gh, ix, gBuffer, from, nc);
		if (fwrite(gBuffer, 1, nc, fp) != nc) { printf("Error: Write error!"); break; }
		size -= nc;
		from += nc;
	}
	fclose(fp);
}

void Read(char *pcmd) 
{
	FILE*  fp;
	char*  pfile;
	U_LONG size;
	char   ch;
	int    nc;
	int ix = GetIndex(pcmd,&pfile);
	if (ix < 0) return;
	if(!(pfile[0])) { printf("Error: File name is not given.\n"); return; }
	if (gRead) { printf("Error: FSA file opened read only."); return; }
	fp = fopen(pfile, "rb");
	if (!fp) { printf("Error: File %s can not be opened.\n", pfile); return; }
	FsaGetStreamSize(gh, ix, &size);
	if (size > 0) {
		printf("Index %d is already occupied,overwrite (y/n)", ix);
		ch = getchar();
		if (ch != 'Y' && ch != 'y') {fclose(fp); return;	}
		FsaTrimStream(gh, ix, 0);
	}
	FsaWriteStream(gh, ix, pfile, 0, strlen(pfile) + 1);
	while((nc=fread(gBuffer,1, MAX_BUFFER,fp))== MAX_BUFFER) {
		FsaGetStreamSize(gh, ix, &size);
		FsaWriteStream(gh, ix, gBuffer, size, MAX_BUFFER);
	}
	if (nc > 0) {
		FsaGetStreamSize(gh, ix, &size);
		FsaWriteStream(gh, ix, gBuffer, size, nc);
	}
	fclose(fp);
}

int main(int argc, char* argv[])
{
	U_LONG id=0;
	char ch;
	int nc=0;
	int i;

	memcpy(&gTansId,gId,sizeof(gTansId));

	if (argc <= 1) {printf("Error: argument insufficient!\n");return 0;}
	if (argc == 2 || (argv[1])[0] == '?') {
		Help();
		return 0;
	}

	if (argc != 3 || strlen(argv[1])!=1) {
		printf("tans (T|t|N|n|R|r|W|w|?) [tans file]: illegal argument.\n");
		return 0;
	}

	switch (toupper(argv[1][0]))
	{
	case 'T': gchMode = 'T'; break;
	case 'N': gchMode = 'N'; break;
	case 'W': gchMode = 'W'; break;
	case 'R': gchMode = 'R'; gRead = 1; break;
	default:
		printf("tans (T|t|N|n|R|r|W|w) <tans file>: illegal argument.\n");
		return 0;
	}

	if (gchMode == 'T' && Remove(argv[2]) == 0) return 0;

	if (FsaOpen(&gh, argv[2], gchMode) != 0) {
		printf("ERROR: FSA file [%s] can not be opened with open mode='%c'.\n", argv[2], gchMode);
		return 0;
	}

	FsaSetErrHandler(gh, ErrorFunc);
	if (gchMode == 'T' || gchMode == 'N') {
		FsaSetFsaTag(gh, gTansId);
	}
	else {
		FsaGetFsaTag(gh, &id);
		if (id != gTansId) {
			printf("Error: specified file[%s] is not a tans file.\n", argv[2]);
			FsaClose(&gh);
			return 0;
		}
	}

	while (1) {
		printf("\n > "); 
		fgets(gBuffer, sizeof(gBuffer), stdin);
		nc = 0;i = 0;while ((ch=gBuffer[i++])!=0) {if(isspace(ch)) continue;gBuffer[nc++]=ch;}
		ch = toupper(gBuffer[0]);
		gBuffer[nc] = 0;
		if (ch=='Q') break;
		switch (ch) {
		case 'L': PrintList(); break;
		case 'I': PrintIndex(); break;
		case 'E': Extend(gBuffer); break;
		case 'W': Write(gBuffer); break;
		case 'R': Read(gBuffer); break;
		case '?': Help(); break;
		}
	}
	FsaClose(&gh);
}
