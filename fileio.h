//
//****************************************************************************
//*                                                                          *
//* Å@Copyright (C) Shigeo Kobayashi, 2022.									 *
//*                 All rights reserved.									 *
//*                                                                          *
//****************************************************************************
//
//  All OS dependent I/O routines are in fileio.h and fileio.cpp.
//
#ifndef ___FILEIO_H___
#define ___FILEIO_H___

#include "memory.h"

//
// Basic file I/O class.
//
class CFileIo : public CMemory
{
public:
	CFileIo()
	{
		m_hFile      = NULL;
		OpenMode     = 0;
//		m_szFile[0]  = 0;
	}

	~CFileIo()
	{
		FIoClose();
	}

	void     FIoOpen(const char *szPath,char chMode);
	int      FIoExist(const char *pszPath);
	void     FIoClose();
	void     FIoRead(void *pBuf,U_INT   cb);
	void     FIoWrite(void *pBuf,U_INT   cb);
	void     FIoSeek(U_LONG  cb);
	void	 FIoSeekSet();
	void	 FIoSeekEnd();

	U_LONG   FIoSize();
	void     FIoFlush();
	int      FIoIsReadOnly()     {return OpenMode=='R';}
	int      FIoIsNewFile()      {return OpenMode == 'N'; };
	int      FIoIsOpened()       {return m_hFile!=NULL;};
	char     FIoOpenedMode()     {return OpenMode; };

private:
	char     OpenMode;      // Open mode char.
	FILE   *m_hFile;          // file handle
//	char    m_szFile[MAX_PATH_SIZE];
};


#endif //___FILEIO_H___
