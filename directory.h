//
//****************************************************************************
//*                                                                          *
//*            Copyright (C) Shigeo Kobayashi, 2022.	                     *
//*                           All rights reserved.                           *
//*                                                                          *
//****************************************************************************
//

#ifndef ___DIRECTORY_H___
#define ___DIRECTORY_H___

#include "header.h"

//
// Stream directory management I/O class.
//
class CDirectory : public CHeader
{
public:
	CDirectory()
	{
	};

	~CDirectory()
	{
	}

#ifdef _DEBUG
	void DebugPrint()
	{
		TRACE(("  -- Stream info --"));
		for (U_INT i = 0; i < HeaderMaxStreams(); ++i)
		{
			TRACE(( "  %d) size=%lld",i,DirGetStreamSize(i) ));
		}
	};
#endif

	void  DirWriteElement(U_INT ix)
	{
		U_INT ixb = HeaderGetDirIndex() + ix / HeaderStreamsInBlock(); // block number having the stream ix.
		U_INT ixs = ix % HeaderStreamsInBlock();                       // relatie stream position in the block ixb.
		BIoWriteToCache(ixb, nullptr, ixs*sizeof(directory_element), sizeof(directory_element));
	};

	directory_element* DirGetElement(U_INT ix)
	{
		ASSERT(ix >= 0 && ix < HeaderMaxStreams());
		directory_element* p = (directory_element*)BIoGetCache(HeaderGetDirIndex() + ix / HeaderStreamsInBlock());
		return &(p[ix % HeaderStreamsInBlock()]);
	};

	U_INT DirGetSatBlockCount(U_INT ix) {
		return DirGetElement(ix)->sat_block_count;
	};

	void DirAddSatBlockCount(U_INT ix, U_INT c) {
		directory_element* p = DirGetElement(ix);
		p->sat_block_count += c;
		DirWriteElement(ix);
	};

	U_LONG DirGetTag(U_INT ix) {
		return DirGetElement(ix)->tag;
	};

	void DirSetTag(U_INT ix, U_LONG tag)
	{
		DirGetElement(ix)->tag = tag;
		DirWriteElement(ix);
		HeaderFlush();
	};

	U_INT DirGetSatIndex(U_INT ix) {
		return DirGetElement(ix)->sat_index;
	};

	void DirSetSatIndex(U_INT ix, U_INT ixs) {
		DirGetElement(ix)->sat_index = ixs;
		DirWriteElement(ix);
	};

	U_LONG DirGetStreamSize(U_INT ix) {
		return DirGetElement(ix)->stream_size;
	};

	void DirSetStreamSize(U_INT ix,U_LONG s) {
		DirGetElement(ix)->stream_size = s;
		DirWriteElement(ix);
	};

	void DirAddStreamSize(U_INT ix, U_LONG s) {
		DirGetElement(ix)->stream_size += s;
		DirWriteElement(ix);
	};

};

#endif //___DIRECTORY_H___
