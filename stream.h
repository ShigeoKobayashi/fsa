//
//****************************************************************************
//*                                                                          *
//* Å@Copyright (C) Shigeo Kobayashi, 2022.									 *
//*                 All rights reserved.									 *
//*                                                                          *
//****************************************************************************
//

#ifndef ___STREAM_H___
#define ___STREAM_H___

#include "directory.h"

//
// Stream I/O class.
//
class CStream : public CSat
{
public:
	CStream()
	{
		m_err_handler = nullptr;;
	}

	~CStream()
	{
	}

	void Open (const char *file,const char mode);
	void Close();
	void Read (U_INT ix, void* buff, U_LONG from, U_INT bytes);
	void Write(U_INT ix, void* buff, U_LONG from, U_INT bytes);
	void SetErrHandler(ErrHandler h) {m_err_handler = h;};
	ErrHandler GetErrHandler()       {return m_err_handler;};

private:
	// Get the pointer to the cache of ib-th block of the ix-th stream.
	// ib is the relative block location counted from the top of the stream block.
	U_INT    GetDataBlock(U_INT ix,U_INT ib);

private:
	ErrHandler  m_err_handler;
};

#endif //___STREEAM_H___