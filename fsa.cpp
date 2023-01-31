#include "stdafx.h"
#include "fsa.h"

#define ENTER(s)   int e=0;CStream *p=nullptr;const char *function=s;errno=0;try {// TRACE(("Enter:%s",s));
#define LEAVE()    }  catch (CErrException er) {\
    if(p!=nullptr && p->GetErrHandler()!=nullptr) {\
        (*(p->GetErrHandler()))((FSA_HANDLE)p,function,er.fsa_code,er.err_no,er.msg,er.source_file,er.source_line);\
    };\
    e = er.fsa_code;\
  } catch(...) {\
   TRACE(("Undefined error(...) errno=%d!",errno));\
    e = ERROR_FSA_UNDEFINED;\
    if(p!=nullptr && p->GetErrHandler()!=nullptr) {\
        (*(p->GetErrHandler()))((FSA_HANDLE)p,function,e,errno,"Undefined error(not catched by FSA)","",0);\
    };\
  };\
  return e;


EXPORT(int) FsaOpen(FSA_HANDLE* h, const char* file, const char mode)
{
    ENTER("FsaOpen");
    *h = nullptr;
    CStream *p = new CStream();
    p->Open(file,mode);
    *h = p;
    LEAVE();
}

EXPORT(int) FsaClose(FSA_HANDLE *h)
{
    ENTER("FsaClose");
    if (h != nullptr) {
        if (*h != nullptr) {
            CStream* p = (CStream*)(*h);
            p->Close();
            delete p;
            *h = nullptr;
        }
    }
    LEAVE();
}

EXPORT(int) FsaWriteStream(FSA_HANDLE h, U_INT i, void* buff, U_LONG from, S_INT bytes)
{
    ENTER("FsaWriteStream");
    if (bytes == 0) return 0;
    CStream* p = (CStream*)(h);
    p->Write(i,buff,from,bytes);
    p->HeaderFlush();
    LEAVE();
}

EXPORT(int) FsaReadStream(FSA_HANDLE h, U_INT i, void* buff, U_LONG from,S_INT bytes)
{
    ENTER("FsaReadStream");
    if (bytes == 0) return 0;
    CStream* p = (CStream*)(h);
    p->Read(i, buff, from,bytes);
    LEAVE();
}

EXPORT(int) FsaGetMaxStreamCount(FSA_HANDLE h, U_INT* cs)
{
    ENTER("FsaGetMaxStreamCount");
    CStream* p = (CStream*)(h);
    *cs = p->HeaderMaxStreams();
    LEAVE();
}

EXPORT(int) FsaExtendMaxStreamCount(FSA_HANDLE h, U_INT cs)
{
    ENTER("FsaExtendMaxStreamCount");
    CStream* p = (CStream*)(h);
    p->HeaderExtendMaxStreams(cs);
    LEAVE();
}

EXPORT(int) FsaGetStreamSize(FSA_HANDLE h, U_INT i, U_LONG* size)
{
    ENTER("FsaGetStreamSize");
    CStream* p = (CStream*)(h);
    *size = p->DirGetStreamSize(i);
    LEAVE();
}

EXPORT(int) FsaGetHeaderTag(FSA_HANDLE h, U_LONG* tag)
{
    ENTER("FsaGetHeaderTag");
    CStream* p = (CStream*)(h);
    *tag = p->HeaderGetTag();
    LEAVE();
}

EXPORT(int) FsaSetHeaderTag(FSA_HANDLE h, U_LONG  tag)
{
    ENTER("FsaSetHeaderTag");
    CStream* p = (CStream*)(h);
    p->HeaderSetTag(tag);
    p->HeaderFlush();
    LEAVE();
}
EXPORT(int) FsaGetStreamTag(FSA_HANDLE h, U_INT i, U_LONG* tag)
{
    ENTER("FsaGetStreamTag");
    CStream* p = (CStream*)(h);
    *tag = p->DirGetStreamSize(i);
    LEAVE();
}
EXPORT(int) FsaSetStreamTag(FSA_HANDLE h, U_INT i, U_LONG  tag)
{
    ENTER("FsaSetStreamTag");
    CStream* p = (CStream*)(h);
    p->DirSetTag(i,tag);
    p->HeaderFlush();
    LEAVE();
}

EXPORT(int) FsaSetErrHandler(FSA_HANDLE h,ErrHandler f)
{
    ENTER("FsaSetErrHandler");
    CStream* p = (CStream*)(h);
    p->SetErrHandler(f);
    LEAVE();
}