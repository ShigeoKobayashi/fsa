//
//****************************************************************************
//*                                                                          *
//* Å@Copyright (C) Shigeo Kobayashi, 2022.									 *
//*                 All rights reserved.									 *
//*                                                                          *
//****************************************************************************
//

#ifndef ___HEADER_H___
#define ___HEADER_H___

#include "fileio.h"

#define DEF_STREAMS_IN_BLOCK 4

static const char gfile_id[8] = {'F','S','A',' ','1','0','0','\0'};

//
// header structure
struct HEADER_CONSTANT {
	// ID and block_size etc, are unchanged once after set!
	char   ID[8];            // ID of FSA == gfile_id[] (the firsrt 4 element must be "FSA ")
	U_INT  block_size;       // the size of one block (==sizeof(directory_element)*streams_in_block,unchanged once defined!). 
	U_INT  streams_in_block; // stream count in one block(currently unchanged==DEF_STREAMS_IN_BLOCK)
} ;

struct HEADER_VARIABLE {
	// member's value bellow change during the processing!
	U_LONG tag;           // For user
	U_INT  total_blocks;     // total count of blocks,total_blocks*block_size == File size!
	U_INT  max_streams;      // maximum number of streams (== directory_block_count*block_size/sizeof(directory_element))
	                         // directory size in blocks can be computed by this value.
	U_INT  directory_index;  // the position of the stream directory.
	U_INT  deleted_chain;    // deleted block chain
} ;

struct Header {
	HEADER_CONSTANT unchangeable;
	HEADER_VARIABLE changeable;
};

//
// element of stream directory 
struct directory_element {
	U_LONG tag;             // For user
	U_LONG stream_size;     // the size of the stream(in bytes).
	U_INT  sat_block_count; // block count of the SAT (stream_size<=sat_block_count*block_size)
	U_INT  sat_index;       // the position of the stream allocation table(SAT) of the stream.
};

//
// element of SAT (stream allocation table)
struct sat_element {
	U_INT data_block_index;        // the position of a data block.
};

#define HeaderAddTotalBlocks(bc)  {m_header.changeable.total_blocks += bc;	m_fHeaderModified = true;}
#define HeaderSetDirIndex(ix)     {m_header.changeable.directory_index = ix;m_fHeaderModified = true;}
#define HeaderSetDelChain(id)     {m_header.changeable.deleted_chain = id;m_fHeaderModified = true;}
#define HeaderGetDelChain()       (m_header.changeable.deleted_chain)
#define HeaderGetDirIndex()       (m_header.changeable.directory_index)
#define HeaderSetTotalBlocks(bc)  {ASSERT(m_header.changeable.total_blocks < bc);m_header.changeable.total_blocks = bc;	m_fHeaderModified = true;}
#define HeaderStreamsInBlock()    (m_header.unchangeable.streams_in_block)


//
// Header I/O class.
//
class CHeader : public CBlockIo
{
public:
	CHeader()
	{
		m_fHeaderModified = false;
		memset(&m_header, 0, sizeof(m_header));
		strcpy(m_header.unchangeable.ID, gfile_id);
		m_header.unchangeable.streams_in_block = DEF_STREAMS_IN_BLOCK;
		m_header.unchangeable.block_size = m_header.unchangeable.streams_in_block * sizeof(struct directory_element);
		ASSERT(m_header.unchangeable.block_size>=sizeof(m_header));
	};

	~CHeader()
	{
#ifdef _DEBUG
		HeaderPrint();
#endif
	};
	
	// Initializer
	void HeaderCreate()
	{
		strcpy(m_header.unchangeable.ID, gfile_id); // FSA ID
		m_header.unchangeable.block_size    = m_header.unchangeable.streams_in_block*sizeof(directory_element);
		m_header.changeable.total_blocks    = 1;
		m_header.changeable.deleted_chain   = 0;
		m_header.changeable.max_streams     = m_header.unchangeable.streams_in_block;
		m_header.changeable.directory_index = 1;
	}

	U_INT HeaderMaxStreams()     {
		return m_header.changeable.max_streams;
	};

	U_INT HeaderBlockSize()
	{
		return m_header.unchangeable.block_size;
	};

	U_INT HeaderGetTotalBlocks() { 
		return m_header.changeable.total_blocks; 
	};  // total number of blocks in the file

	U_LONG  HeaderGetTag() {
		return m_header.changeable.tag;
	}

	void   HeaderSetTag(U_LONG tag) {
		m_header.changeable.tag = tag;
		m_fHeaderModified = true;
	};

	U_INT HeaderAddBlocks(U_INT cb)
	{
		ASSERT(cb > 0);
		memset(BIoBuffer(), 0, (U_INT)BIoBlockSize());
		FIoSeek(((U_LONG)HeaderGetTotalBlocks()) * (U_LONG)BIoBlockSize());
		for (U_INT i = 0; i < cb; ++i) {
			FIoWrite(BIoBuffer(), (U_INT)BIoBlockSize());
		}
		HeaderSetTotalBlocks(HeaderGetTotalBlocks() + cb);
		return HeaderGetTotalBlocks() - cb;
	}

	U_INT HeaderGetFreeBlock()
	{
		U_INT ix;
		U_INT ib = HeaderGetDelChain();
		if (ib == 0) return HeaderAddBlocks(1);
		// deleted block exists => use it!
		FIoSeek(((U_LONG)ib) * (U_LONG)BIoBlockSize());
		FIoRead(&ix, sizeof(ix));
		HeaderSetDelChain(ix);
		return ib;
	};

	void   HeaderDelBlock(U_INT ib)
	{
		ASSERT(ib != 0);
		U_INT ix = HeaderGetDelChain();
		HeaderSetDelChain(ib);
		BIoFlushCache();
		FIoSeek(((U_LONG)ib) * (U_LONG)BIoBlockSize());
		FIoWrite(&ix, sizeof(ix));
	};

	void   HeaderExtendMaxStreams(U_INT cs)
	{
		U_INT dir_index       = m_header.changeable.directory_index;
		U_INT cur_block_count = m_header.changeable.max_streams / m_header.unchangeable.streams_in_block;
		U_INT added_blocks    = (cs + m_header.unchangeable.streams_in_block - 1) / m_header.unchangeable.streams_in_block;
		U_INT added_streams   = added_blocks * m_header.unchangeable.streams_in_block;
		U_INT ix_new = HeaderAddBlocks(cur_block_count + added_blocks);
		// Copy current directory to new AREA
		BIoCopyBlocks(dir_index, ix_new, cur_block_count);
		for (U_INT i = 0; i < cur_block_count; ++i) {
			HeaderDelBlock(dir_index + i);
		}
		m_header.changeable.max_streams += added_streams;
		m_header.changeable.directory_index = ix_new;
		m_fHeaderModified = true;
		HeaderFlush();
	}

	void HeaderRead()
	{
		FIoSeek(0);
		FIoRead(&m_header, sizeof(m_header));
		m_fHeaderModified = false;
	}

	void HeaderWrite()
	{
		FIoSeek(0);
		FIoWrite(&m_header, sizeof(m_header));
		m_fHeaderModified = false;
	}

	void HeaderFlush()
	{
		if (m_fHeaderModified) {
			FIoSeek(sizeof(m_header.unchangeable));
			FIoWrite(&(m_header.changeable), sizeof(m_header.changeable));
			m_fHeaderModified = false;
		}
	};


#ifdef _DEBUG
	void HeaderPrint()
	{

		struct HEADER_VARIABLE {
			// member's value bellow change during the processing!
			U_LONG directory_index;  // the position of the stream directory.
			U_LONG deleted_chain;    // deleted block chain
		};
		TRACE(("            ID            =%s",  m_header.unchangeable.ID));
		TRACE(("            block_size    =%d", m_header.unchangeable.block_size));
		TRACE(("            streams/block =%d", m_header.unchangeable.streams_in_block));
		TRACE(("            total block   =%d", m_header.changeable.total_blocks));
		TRACE(("            max_streams   =%d", m_header.changeable.max_streams));
		TRACE(("            dir index     =%d", m_header.changeable.directory_index));
		TRACE(("            deleted chain =%d", m_header.changeable.deleted_chain));
		(void)getchar();
	}
#endif

	void HeaderCheck()
	{
		TRACE(("HeaderCheck:%s", m_header.unchangeable.ID));
		for (int i = 0; i < 4; ++i) {
			CHECK(m_header.unchangeable.ID[i] == gfile_id[i], ERROR_FSA_ILLEGAL, "Illegal file opened (ID,not a FSA file).");
		}
		CHECK(m_header.unchangeable.block_size == m_header.unchangeable.streams_in_block * sizeof(directory_element), ERROR_FSA_ILLEGAL, "Illegal file opened (block size,not a FSA file).");
	}

protected:
	// Header cache.
	struct Header  m_header;
	bool           m_fHeaderModified;

private:
};

#endif //___HEADER_H___