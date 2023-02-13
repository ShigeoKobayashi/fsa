#include "stdafx.h"
#include "fsa.h"

#define ENTER(s)   int e=0;CStream *p=nullptr;const char *function=s;errno=0;try {// TRACE(("Enter:%s",s));
#define LEAVE()    }  catch (CErrException er) {\
    e = er.fsa_code;\
    if(p!=nullptr && p->GetErrHandler()!=nullptr) {\
        e = (*(p->GetErrHandler()))((FSA_HANDLE)p,function,er.fsa_code,er.err_no,er.msg,er.source_file,er.source_line);\
    }\
  } catch(...) {\
   TRACE(("Undefined error(...) errno=%d!",errno));\
    e = ERROR_FSA_UNDEFINED;\
    if(p!=nullptr && p->GetErrHandler()!=nullptr) {\
        e = (*(p->GetErrHandler()))((FSA_HANDLE)p,function,e,errno,"Undefined error(not catched by FSA)","",0);\
    };\
  };\
  return e;

EXPORT(int) FsaOpen(FSA_HANDLE* h, const char* file, const char mode)
{
    ENTER("FsaOpen");
    *h = nullptr;
    p = new CStream();
    p->Open(file,mode);
    *h = p;
    LEAVE();
}

EXPORT(int) FsaClose(FSA_HANDLE *h)
{
    ENTER("FsaClose");
    if (h != nullptr) {
        if (*h != nullptr) {
            p = (CStream*)(*h);
            p->SetErrHandler(nullptr);
            p->Close();
            delete p;
            *h = nullptr;
        }
    }
    LEAVE();
}

EXPORT(int) FsaWriteStream(FSA_HANDLE h, U_INT i, void* buff, U_LONG from, U_INT bytes)
{
    ENTER("FsaWriteStream");
    if (bytes == 0) return 0;
    p = (CStream*)(h);
    p->Write(i,buff,from,bytes);
    LEAVE();
}

EXPORT(int) FsaReadStream(FSA_HANDLE h, U_INT i, void* buff, U_LONG from,U_INT bytes)
{
    ENTER("FsaReadStream");
    if (bytes == 0) return 0;
    p = (CStream*)(h);
    p->Read(i, buff, from,bytes);
    LEAVE();
}

EXPORT(int) FsaTrimStream(FSA_HANDLE h,U_INT i, U_LONG from)
{
    ENTER("FsaTrimStream");
    p = (CStream*)(h);
    p->Trim(i,from);
    LEAVE();
}

EXPORT(int) FsaGetMaxStreamCount(FSA_HANDLE h, U_INT* cs)
{
    ENTER("FsaGetMaxStreamCount");
    p = (CStream*)(h);
    *cs = p->HeaderMaxStreams();
    LEAVE();
}

EXPORT(int) FsaExtendMaxStreamCount(FSA_HANDLE h, U_INT cs)
{
    ENTER("FsaExtendMaxStreamCount");
    p = (CStream*)(h);
    p->HeaderExtendMaxStreams(cs);
    LEAVE();
}

EXPORT(int) FsaGetStreamSize(FSA_HANDLE h, U_INT i, U_LONG* size)
{
    ENTER("FsaGetStreamSize");
    p = (CStream*)(h);
    *size = p->DirGetStreamSize(i);
    LEAVE();
}

EXPORT(int) FsaGetFsaTag(FSA_HANDLE h, U_LONG* tag)
{
    ENTER("FsaGetFsaTag");
    p = (CStream*)(h);
    *tag = p->HeaderGetTag();
    LEAVE();
}

EXPORT(int) FsaSetFsaTag(FSA_HANDLE h, U_LONG  tag)
{
    ENTER("FsaSetFsaTag");
    p = (CStream*)(h);
    p->HeaderSetTag(tag);
    LEAVE();
}
EXPORT(int) FsaGetStreamTag(FSA_HANDLE h, U_INT i, U_LONG* tag)
{
    ENTER("FsaGetStreamTag");
    p = (CStream*)(h);
    *tag = p->DirGetStreamSize(i);
    LEAVE();
}
EXPORT(int) FsaSetStreamTag(FSA_HANDLE h, U_INT i, U_LONG  tag)
{
    ENTER("FsaSetStreamTag");
    p = (CStream*)(h);
    p->DirSetTag(i,tag);
    LEAVE();
}

EXPORT(int) FsaSetErrHandler(FSA_HANDLE h, FSA_ERROR_HANDLER f)
{
    ENTER("FsaSetErrHandler");
    p = (CStream*)(h);
    p->SetErrHandler(f);
    LEAVE();
}

EXPORT(int) FsaGetErrHandler(FSA_HANDLE h, FSA_ERROR_HANDLER* pf)
{
    ENTER("FsaGetErrHandler");
    p = (CStream*)(h);
    *pf = p->GetErrHandler();
    LEAVE();
}

EXPORT(int) FsaGetOpenMode(FSA_HANDLE h, unsigned char* pmode)
{
    ENTER("FsaGetOpenMode");
    p = (CStream*)(h);
    *pmode = p->FIoOpenedMode();
    LEAVE();
}
