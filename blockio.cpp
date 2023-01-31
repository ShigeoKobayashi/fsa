///////////////////////////////////////////////////////////////////////
//
// Copyright(C) 2022 Shigeo Kobayashi. All Rights Reserved.
// 
///////////////////////////////////////////////////////////////////////
//
//

#include "stdafx.h"
#include "blockio.h"


void CBlockIo::BIoInitCache(U_INT block_size)
{
	ASSERT(block_size > 0);
	m_block_size = (U_INT)block_size;
	// m_buffer(size==m_block_size) is an I/O buffer which must not be used as cache!
	m_buffer = (DATA_PTR)MemAlloc((U_INT)((CACHE_COUNT+1) * m_block_size));
	DATA_PTR buf = m_buffer +  m_block_size;
	for (int i = 0; i < CACHE_COUNT; ++i) {
		m_block_ptr[i]      = buf;
		m_block_cached[i]   = 0;
		m_modified_from[i]  = m_block_size;
		m_modified_bytes[i] = 0;
		m_modified_chain[i] = -1;

		buf += m_block_size;
#ifdef _BEBUG
		m_accessed[i] = 0;
		m_hit[i]      = 0;
#endif
	}
	m_modified_top = -1;
}

void CBlockIo::BIoFlushCache()
{
	while (m_modified_top >= 0) {
		int next = m_modified_chain[m_modified_top];
		FIoSeek((U_LONG)m_block_cached[m_modified_top] * BIoBlockSize() + (U_LONG)m_modified_from[m_modified_top]);
		FIoWrite(m_block_ptr[m_modified_top]+ m_modified_from[m_modified_top], m_modified_bytes[m_modified_top]);
		m_modified_from[m_modified_top]  = BIoBlockSize();
		m_modified_bytes[m_modified_top] = 0;
		m_modified_chain[m_modified_top] = -1;
		m_modified_top = next;
	}
#ifdef _DEBUG
	for (int i = 0; i < 256; ++i) {
		ASSERT(m_modified_from [i] == BIoBlockSize());
		ASSERT(m_modified_bytes[i] == 0);
		ASSERT(m_modified_chain[i] == -1);
	}
	ASSERT(m_modified_top == -1);
#endif
}

DATA_PTR CBlockIo::BIoGetCache(U_INT blk)
{
	ASSERT(m_block_size > 0);

	CACHE_INDEX ix = BIoHash4B(blk);
#ifdef _DEBUG
	++m_accessed[ix];
#endif
	if (m_block_cached[ix] != blk) {
		// Not cached => read it!
		if (m_modified_bytes[ix] > 0) BIoFlushCache();
		BIoReadBlock(blk, m_block_ptr[ix]);
		m_block_cached[ix] = blk;
		m_modified_from[ix] = BIoBlockSize();
		m_modified_bytes[ix] = 0;
		m_modified_chain[ix] = -1;
	}
#ifdef _DEBUG
	else {
		++m_hit[ix];
	}
#endif
	return m_block_ptr[ix];
};

void CBlockIo::BIoWriteToCache(U_INT blk, DATA_PTR buf, U_INT from, U_INT bytes)
{
	CACHE_INDEX ix = BIoHash4B(blk);
	ASSERT(bytes > 0 && from + bytes <= BIoBlockSize());
	DATA_PTR p = BIoGetCache(blk);
	if(buf!=nullptr) memcpy(p+from,buf,bytes);
	if (m_modified_bytes[ix] <= 0) {
		m_modified_chain[ix] = m_modified_top;
		m_modified_top = ix;
	}

	if(m_modified_bytes[ix]<=0) {
		m_modified_from[ix]  = from;
		m_modified_bytes[ix] = bytes;
	}
	else {
		U_INT x1 = m_modified_from[ix];
		if (from < x1) x1 = from;
		U_INT x2 = m_modified_from[ix] + m_modified_bytes[ix];
		if (x2 < from + bytes) x2 = from + bytes;
		m_modified_from[ix] = x1;
		m_modified_bytes[ix] = x2 - x1;
	}
}

void CBlockIo::BIoReadFromCache(U_INT blk, DATA_PTR buf, U_INT from, U_INT bytes)
{
	ASSERT(bytes > 0 && from + bytes <= BIoBlockSize());
	memcpy(buf, BIoGetCache(blk) + from,  bytes);
}

void CBlockIo::BIoReadBlock(U_INT blk, DATA_PTR ptr)
{
	FIoSeek(((U_LONG)blk) * m_block_size);
	FIoRead(ptr, (U_INT)m_block_size);
}

void CBlockIo::BIoWriteBlock(U_INT blk, DATA_PTR ptr)
{
	FIoSeek(((U_LONG)blk) * m_block_size);
	FIoWrite(ptr, (U_INT)m_block_size);
}

void CBlockIo::BIoCopyBlocks(U_INT blk_s, U_INT blk_t, U_INT nb)
{
	for (U_INT i = 0; i < nb; ++i) {
		BIoReadBlock(blk_s++, m_buffer);
		BIoWriteBlock(blk_t++, m_buffer);
	}
}
