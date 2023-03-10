///////////////////////////////////////////////////////////////////////
//
// Copyright(C) 2022 Shigeo Kobayashi. All Rights Reserved.
// 
///////////////////////////////////////////////////////////////////////
//
//

#include "stdafx.h"
#include "fileio.h"

// C I/O routines
#ifdef WINDOWS
#define STRUCT_STAT       struct _stat64
#define STAT(path,st)	  _stat64(path,st)
#define OPEN(path,mode)   fopen(path,mode)
#define CLOSE(f)          fclose(f)
#define READ(buf,cb,f)    fread(buf,1,cb,f)
#define WRITE(buf,cb,f)   fwrite(buf,1,cb,f)
#define SEEK(f,cb,pos)    _fseeki64(f,cb,pos)
#define TELL(f)           _ftelli64(f);
#define FLUSH(f)          fflush(f)
#endif

#ifdef LINUX
#define STRUCT_STAT       struct stat64
#define STAT(path,st)	  stat64(path,st)
#define OPEN(path,mode)   fopen64(path,mode)
#define CLOSE(f)          fclose(f)
#define READ(buf,cb,f)    fread(buf,1,cb,f)
#define WRITE(buf,cb,f)   fwrite(buf,1,cb,f)
#define SEEK(f,cb,pos)    fseeko64(f,cb,pos)
#define TELL(f)           ftello64(f);
#define FLUSH(f)          fflush(f)
#endif


//
// returns 1 if the path specified is there,otherwise returns 0.
//
int CFileIo::FIoExist(const char *pszPath)
{
	STRUCT_STAT st;
	int i = STAT(pszPath,&st);
	if(i==0) return 1; // TRUE
	return 0;
}

//
// Opens the file specified by szPath withe open mode chMode.
//  chMode: Open mode
//    'R' ... opens existing file for read only mode.
//    'W' ... opens exisitng file for read and write mode.
//    'N' ... opens/creates non-existing file for read and write mode.
//    'T' ... opens non-existing or existing file for read and write mode.
//            existing file is truncated.
//            Lower case is accepted.
//
void CFileIo::FIoOpen(const char *szPath,char chMode)
{
	const char *pszMode = "";
   	CHECK(m_hFile==NULL,ERROR_FILE_DUPLICATED_OPEN,"FIoOpen(): Already opede().");

	//
	// Open file according to thw specified open mode.
	m_hFile = NULL;
	if(chMode=='T' || chMode=='t') {
		// truncation mode
		pszMode = "w+b";
		chMode = 'T'; 
	} else if(chMode=='N' || chMode=='n') {
		// New File 
		CHECK(FIoExist(szPath),ERROR_FILE_OPEN_MODE,"FIoOpen(): Existing file for 'N'");
		pszMode = "w+b";
		chMode = 'N';
	} else if(chMode=='R' || chMode=='r') {
        // Open existing file for reading.
		pszMode = "rb";
		chMode = 'R';
	} else if(chMode=='W' || chMode=='w') {
        // Open existing for for read and write mode.
		pszMode = "r+b";
		chMode = 'W';
    } else {
        ER_THROW(this,ERROR_FILE_OPEN_MODE,"FIoOpen(): Undefined open mode."); // illegal open mode
    }

	m_hFile = OPEN(szPath,pszMode);
	CHECK(m_hFile!=NULL,ERROR_FILE_OPEN,"FIoOpen(): Open failed.");
	// keep file path and open mode.
	OpenMode = chMode;
}

void CFileIo::FIoClose()
{
    if(m_hFile==NULL) return;
	TRACE(("FIoClose: File size=%lld",FIoSize()));
	CLOSE(m_hFile);
	m_hFile = NULL;
}

void CFileIo::FIoRead(void *pBuf,U_INT cb)
{
	CHECK((U_INT)READ(pBuf,cb,m_hFile)==cb,ERROR_FILE_READ,"FIoRead(): Failed.");
}

void CFileIo::FIoWrite(void *pBuf,U_INT cb)
{
	CHECK((U_INT)WRITE(pBuf,cb,m_hFile)==cb,ERROR_FILE_WRITE,"FIoWrite(): Failed/");
}

void CFileIo::FIoSeek(U_LONG cb)
{
	CHECK(SEEK(m_hFile,cb,SEEK_SET)==0,ERROR_FILE_SEEK,"FIoSeek(): Failed.");
}

void CFileIo::FIoSeekSet()
{
	SEEK(m_hFile, 0, SEEK_SET);
}

void CFileIo::FIoSeekEnd()
{
	SEEK(m_hFile, 0, SEEK_END);
}

//
// returns current file size(if error,then returns -1 or thows exception). 
U_LONG  CFileIo::FIoSize()
{
	SEEK(m_hFile,0,SEEK_END);
	return TELL(m_hFile);
}

void CFileIo::FIoFlush()
{
	FLUSH(m_hFile);
}
