//
//****************************************************************************
//*                                                                          *
//*                 Copyright (C) Shigeo Kobayashi, 2022.                    *
//*                               All rights reserved.                       *
//*                                                                          *
//****************************************************************************
//

#ifndef ___SAT_H___
#define ___SAT_H___

#include "directory.h"

//
// (SAT) Stream allocation table management I/O class.
//
class CSat : public CDirectory
{
public:
	CSat()
	{
	};


	~CSat()
	{
	}

	/*
	struct sat_element {
		U_INT data_block_index;        // the position of a data block.
	};
	*/
	U_INT SatMaxDataBlockCount(U_INT ix)
	{
		return (DirGetSatBlockCount(ix)* BIoBlockSize()) / sizeof(sat_element);
	};

	U_INT SatGetDataBlockCount(U_INT ix)
	{
		return (U_INT)((DirGetStreamSize(ix) + BIoBlockSize() - 1) / BIoBlockSize());
	};

	U_INT SatGetDatIndex(U_INT ix_sat, U_INT ix_offset)
	{
		if (ix_offset > HeaderStreamsInBlock()) {
			U_INT nb = (ix_offset + HeaderStreamsInBlock() - 1) / HeaderStreamsInBlock();
			ix_sat    += nb;
			ix_offset -= nb * HeaderStreamsInBlock();
		}
		sat_element *ps = (sat_element*)BIoGetCache(ix_sat);
		return ps->data_block_index;
	};

	void  SatSetDatIndex(U_INT ix_sat, U_INT ix_offset,U_INT ix_dat)
	{
		if (ix_offset > HeaderStreamsInBlock()) {
			U_INT nb = (ix_offset + HeaderStreamsInBlock() - 1) / HeaderStreamsInBlock();
			ix_sat += nb;
			ix_offset -= nb * HeaderStreamsInBlock();
		}
		sat_element* ps = (sat_element*)BIoGetCache(ix_sat);
		ps->data_block_index = ix_dat;
		BIoWriteToCache(ix_sat, (DATA_PTR)ps,(U_INT)(ix_offset*sizeof(ps[0])), sizeof(ps[0]));
	}


private:


private:

};

#endif //___SAT_H___
