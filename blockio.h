//
//****************************************************************************
//*                                                                          *
//*               Copyright (C) Shigeo Kobayashi, 2022.	                     *
//*                             All rights reserved.                         *
//*                                                                          *
//****************************************************************************
//

#ifndef ___BLOCKIO_H___
#define ___BLOCKIO_H___

#include "fileio.h"

#define CACHE_INDEX  unsigned char    // index for cache array(from 0 to 255)
#define CACHE_COUNT  256              // Max. number of caches represented by  CACHE_INDEX
#define DATA_PTR     unsigned char *

#define BIoBlockSize()     (m_block_size)
#define BIoCompBlock(x)    (x/m_block_size)
#define BIoCompOffset(x)   (x%m_block_size)

//
// Basic block I/O class.
//
class CBlockIo : public CFileIo
{
public:
	CBlockIo()
	{
		m_block_size = 0;
		m_buffer = nullptr;
		for (int i = 0; i < CACHE_COUNT; ++i) {
#ifdef _DEBUG
			m_accessed[i] = 0;
			m_hit[i] = 0;
#endif
			m_block_ptr[i] = nullptr;
			m_block_cached[i]   = 0;
			m_modified_from [i] = -1;
			m_modified_bytes[i] = 0;
			m_modified_chain[i] = -1;
		}
		m_modified_top = -1;
	}
	
	void         BIoWriteToCache(U_INT blk, DATA_PTR buf, U_INT from, U_INT bytes);
	void         BIoReadFromCache  (U_INT blk, DATA_PTR buf, U_INT from, U_INT bytes);

	void         BIoFlushCache();
	void         BIoReadBlock(U_INT blk, DATA_PTR ptr);
	void         BIoWriteBlock(U_INT blk, DATA_PTR ptr);
	void         BIoCopyBlocks(U_INT blk_s, U_INT blk_t,U_INT nb);

	DATA_PTR     BIoGetCache(U_INT blk);
	DATA_PTR     BIoBuffer() { return m_buffer; };
	CACHE_INDEX  BIoHash4B(U_INT b)
	{
		return ((DATA_PTR)&b)[0] + ((DATA_PTR)&b)[1] + ((DATA_PTR)&b)[2] + ((DATA_PTR)&b)[3];
		/*
				CACHE_INDEX s = 0;
				for (int i = 0; i < sizeof(U_INT); ++i) {
					s += p[i];
				}
				return s;
		*/
	};

	~CBlockIo()
	{
#ifdef _DEBUG
		BIoPrintResult();
#endif
		DeleteCache();
	}

protected:
	void BIoInitCache(U_INT block_size);

private:
	void DeleteCache() { MemFree((void**)&m_buffer); };

#ifdef _DEBUG
	void BIoPrintResult()
	{
		printf("\n --- CBlockIo cache result (Hit/Accessed) ---\n");
		for (int i = 0; i < CACHE_COUNT; ++i) {
			printf("    %d) %d/%d\n",i, m_hit[i], m_accessed[i]);
		}
		(void)getchar();
	};
#endif

protected:
	U_INT        m_block_size;   // must be set before using!

private:
	// cache buffer
	DATA_PTR     m_buffer; // working buffer of which size is m_block_size.
	DATA_PTR     m_block_ptr   [CACHE_COUNT]; // ptr to each cache(m_block_size bytes).
	U_INT        m_block_cached[CACHE_COUNT]; // block number cached(0 means non-block number(not yet used))

	// dirty cache management
	U_INT        m_modified_from [CACHE_COUNT];
	U_INT        m_modified_bytes[CACHE_COUNT];
	S_INT        m_modified_chain[CACHE_COUNT];
	S_INT        m_modified_top;

#ifdef _DEBUG
	int m_accessed[CACHE_COUNT];
	int m_hit[CACHE_COUNT];
#endif

};

#endif //___BLOCKIO_H___
