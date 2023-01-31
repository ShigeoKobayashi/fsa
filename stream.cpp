//
//****************************************************************************
//*                                                                          *
//* Å@Copyright (C) Shigeo Kobayashi, 2022.									 *
//*                 All rights reserved.									 *
//*                                                                          *
//****************************************************************************
//
#include "stdafx.h"
#include "stream.h"

void CStream::Open(const char* file, const char mode)
{
	FIoOpen(file, mode);
	if (FIoIsNewFile()) {
		HeaderCreate();
		BIoInitCache(HeaderBlockSize());
		HeaderWrite();
		HeaderAddBlocks(1);
	}
	else {
		// Existing file
		HeaderRead();
		HeaderCheck();
		BIoInitCache(HeaderBlockSize());
	}
}

void CStream::Write(U_INT ix, void* buff, U_LONG from,U_INT bytes)
{
	CHECK(from <= DirGetStreamSize(ix), ERROR_FSA_WRITE, "Write starts from out of range!");
	CHECK((S_INT)bytes > 0, ERROR_FSA_WRITE, "Negative number of bytes written is illegal.");

	U_INT    ixb      = (U_INT)BIoCompBlock(from);
	U_INT    ixs      = (U_INT)BIoCompOffset(from);

	DATA_PTR pb = (DATA_PTR)buff;
	U_INT cb = bytes;
	while (cb > 0) {
		U_INT data_block = GetDataBlock(ix,ixb++);
		U_INT    c = cb;
		if (ixs + c > BIoBlockSize())  c = BIoBlockSize() - ixs;
		BIoWriteToCache(data_block, pb, ixs, (U_INT)c);
		DirAddStreamSize(ix, c);
		ixs    = 0;
		pb    += c;
		cb    -= c;
	}
	BIoFlushCache();
	HeaderFlush();
}

void CStream::Read(U_INT ix,void *buff,U_LONG from, U_INT bc)
{
	CHECK((S_INT)bc > 0, ERROR_FSA_READ, "Negative number of bytes read is illegal.");
	CHECK((from + bc) <= DirGetStreamSize(ix), ERROR_FSA_READ, "Read starts from out of range!");

	U_INT    ixb      = (U_INT)BIoCompBlock(from);
	U_INT    ixs      = (U_INT)BIoCompOffset(from);

	DATA_PTR pb = (DATA_PTR)buff;
	while (bc > 0) {
		U_INT data_block = GetDataBlock(ix,ixb++);
		DATA_PTR p       = BIoGetCache(data_block);
		U_INT          c = bc;
		if (ixs + c > BIoBlockSize())  c = BIoBlockSize() - ixs;
		memcpy(pb,p+ixs, (U_INT)c);
		ixs = 0;
		pb += c;
		bc -= c;
	}
}

void CStream::Close()
{
#ifdef _DEBUG
	DebugPrint();
#endif
	FIoClose();
}

// returns data index of ib-th block of the ix-th stream.
// ib is the relative block location counted from the top of the stream block ix.
U_INT CStream::GetDataBlock(U_INT ix,U_INT ib)
{
	U_INT  sat_index    = DirGetSatIndex(ix);

	// current data block count of the stream ix (non-zero element count of the SAT).
	U_INT max_data_block = SatMaxDataBlockCount(ix);
	if (ib == max_data_block) {
		// Need to extend Sat
		U_INT cur_sat_blocks = DirGetSatBlockCount(ix);
		U_INT ix_new = HeaderAddBlocks(cur_sat_blocks + 1);
		// Copy SAT to new AREA
		BIoCopyBlocks(sat_index, ix_new, cur_sat_blocks);
		for (U_INT i = 0; i < cur_sat_blocks; ++i) {
			HeaderDelBlock(sat_index + i);
		}
		sat_index = ix_new;
		DirSetSatIndex(ix, sat_index);
		DirAddSatBlockCount(ix, 1);
	}
	sat_index += (ib * sizeof(sat_element)) / BIoBlockSize();
	sat_element *ps = (sat_element*)BIoGetCache(sat_index);
	ib %= (BIoBlockSize()/ sizeof(sat_element));
	if (ps[ib].data_block_index == 0) {
		// data block not yet allocated.
		ps[ib].data_block_index = HeaderGetFreeBlock();
		BIoWriteToCache(sat_index, (DATA_PTR) & (ps[ib]), (U_INT)ib*sizeof(ps[0]), sizeof(ps[0]));
	}
	return ps[ib].data_block_index;
}
 
