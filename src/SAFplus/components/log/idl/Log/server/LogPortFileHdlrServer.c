/*********************************************************************
* ModuleName  : idl
*********************************************************************/
/*********************************************************************
* Description :Server Stub routines
*     
* THIS FILE IS AUTO-GENERATED BY OPENCLOVIS IDE. EDIT THIS FILE AT
* YOUR OWN RISK. ANY CHANGE TO THIS FILE WILL BE OVERWRITTEN ON
* RE-GENERATION.
*     
*********************************************************************/
#include <netinet/in.h>
#include <clBufferApi.h>
#include <clRmdApi.h>
#include <clEoApi.h>
#include <ipi/clRmdIpi.h>
#include <string.h>
#include "LogPortFileHdlrServer.h"
#include "LogServer.h"

extern ClUint32T  LogidlSyncKey;
extern ClHandleDatabaseHandleT  LogidlDatabaseHdl;



ClRcT clLogFileHdlrFileOpenServer_4_0_0(ClEoDataT eoData, ClBufferHandleT inMsgHdl, ClBufferHandleT outMsgHdl)
{
    ClIdlContextInfoT *pIdlCtxInfo = NULL;
    ClRcT rc = CL_OK;
    ClStringT  fileName;
    ClStringT  fileLocation;
    ClUint32T  operatingLvl;

    memset(&(fileName), 0, sizeof(ClStringT));
    memset(&(fileLocation), 0, sizeof(ClStringT));
    memset(&(operatingLvl), 0, sizeof(ClUint32T));


    rc = clXdrUnmarshallClStringT( inMsgHdl,&(fileName));
    if (CL_OK != rc)
    {
        goto LL0;
    }

    rc = clXdrUnmarshallClStringT( inMsgHdl,&(fileLocation));
    if (CL_OK != rc)
    {
        goto LL1;
    }

    pIdlCtxInfo = (ClIdlContextInfoT *)clHeapAllocate(sizeof(ClIdlContextInfoT));
    if(pIdlCtxInfo == NULL)
    {
       return CL_IDL_RC(CL_ERR_NO_MEMORY);
    }
    memset(pIdlCtxInfo, 0, sizeof(ClIdlContextInfoT));
    pIdlCtxInfo->idlDeferMsg = outMsgHdl; 
    pIdlCtxInfo->inProgress  = CL_FALSE;
    rc = clIdlSyncPrivateInfoSet(LogidlSyncKey, (void *)pIdlCtxInfo);
    if (CL_OK != rc)
    {
        clHeapFree(pIdlCtxInfo);
        goto L0;
    }
    rc = clLogFileHdlrFileOpen_4_0_0(&(fileName), &(fileLocation), &(operatingLvl));
    if(pIdlCtxInfo->inProgress == CL_FALSE)
    {
      clHeapFree(pIdlCtxInfo);
      pIdlCtxInfo = NULL;
    }
    if (CL_OK != rc)
    {
       goto L0;
    }
    
    rc = clXdrMarshallClStringT(&(fileName), 0, 1);
    if (CL_OK != rc)
    {
        goto L1;
    }

    rc = clXdrMarshallClStringT(&(fileLocation), 0, 1);
    if (CL_OK != rc)
    {
        goto L2;
    }

    if(pIdlCtxInfo != NULL)
    {
      clHeapFree(pIdlCtxInfo);
      return rc;
    }
    
    rc = clXdrMarshallClUint32T(&(operatingLvl), outMsgHdl, 1);
    if (CL_OK != rc)
    {
        goto L3;
    }

L3:    return rc;

LL1:  clXdrMarshallClStringT(&(fileLocation), 0, 1);
LL0:  clXdrMarshallClStringT(&(fileName), 0, 1);

    return rc;

L0:  clXdrMarshallClStringT(&(fileName), 0, 1);
L1:  clXdrMarshallClStringT(&(fileLocation), 0, 1);

L2:  clXdrMarshallClUint32T(&(operatingLvl), 0, 1);

    return rc;
}

ClRcT clLogFileHdlrFileOpenResponseSend_4_0_0(ClIdlHandleT idlHdl,ClRcT retCode,CL_OUT  ClUint32T  operatingLvl)
{
    ClIdlSyncInfoT    *pIdlSyncDeferInfo = NULL;
    ClRcT              rc                = CL_OK;
    ClBufferHandleT outMsgHdl     = 0;
    
    rc = clHandleCheckout(LogidlDatabaseHdl,idlHdl,(void **)&pIdlSyncDeferInfo);
    if( rc != CL_OK)
    {
      goto Label0; 
    }
    outMsgHdl = pIdlSyncDeferInfo->idlRmdDeferMsg;
    
    rc = clXdrMarshallClUint32T(&(operatingLvl), outMsgHdl, 1);
    if (CL_OK != rc)
    {
        goto L3;
    }

    rc = clIdlSyncResponseSend(pIdlSyncDeferInfo->idlRmdDeferHdl,outMsgHdl,
                                retCode);
    goto Label1; 
L3:  clXdrMarshallClUint32T(&(operatingLvl), 0, 1);

    clHandleCheckin(LogidlDatabaseHdl, idlHdl);
    clHandleDestroy(LogidlDatabaseHdl, idlHdl);
    return rc;
Label1:
    clHandleCheckin(LogidlDatabaseHdl, idlHdl);
    clHandleDestroy(LogidlDatabaseHdl, idlHdl);
Label0:
    return rc;
}

ClRcT clLogFileHdlrFileRecordsGetServer_4_0_0(ClEoDataT eoData, ClBufferHandleT inMsgHdl, ClBufferHandleT outMsgHdl)
{
    ClIdlContextInfoT *pIdlCtxInfo = NULL;
    ClRcT rc = CL_OK;
    ClStringT  fileName;
    ClStringT  fileLocation;
    ClUint16T  isDelete;
    ClUint64T  pStartRec;
    ClUint32T  version;
    ClUint32T  pNumRecords;
    ClUint32T  pBuffeLen;
    ClUint8T*  pLogRecords;

    memset(&(fileName), 0, sizeof(ClStringT));
    memset(&(fileLocation), 0, sizeof(ClStringT));
    memset(&(isDelete), 0, sizeof(ClUint16T));
    memset(&(pStartRec), 0, sizeof(ClUint64T));
    memset(&(version), 0, sizeof(ClUint32T));
    memset(&(pNumRecords), 0, sizeof(ClUint32T));
    memset(&(pBuffeLen), 0, sizeof(ClUint32T));
    memset(&(pLogRecords), 0, sizeof(ClUint8T*));


    rc = clXdrUnmarshallClStringT( inMsgHdl,&(fileName));
    if (CL_OK != rc)
    {
        goto LL0;
    }

    rc = clXdrUnmarshallClStringT( inMsgHdl,&(fileLocation));
    if (CL_OK != rc)
    {
        goto LL1;
    }

    rc = clXdrUnmarshallClUint16T( inMsgHdl,&(isDelete));
    if (CL_OK != rc)
    {
        goto LL2;
    }

    rc = clXdrUnmarshallClUint64T( inMsgHdl,&(pStartRec));
    if (CL_OK != rc)
    {
        goto LL3;
    }

    rc = clXdrUnmarshallClUint32T( inMsgHdl,&(version));
    if (CL_OK != rc)
    {
        goto LL4;
    }

    rc = clXdrUnmarshallClUint32T( inMsgHdl,&(pNumRecords));
    if (CL_OK != rc)
    {
        goto LL5;
    }


    pIdlCtxInfo = (ClIdlContextInfoT *)clHeapAllocate(sizeof(ClIdlContextInfoT));
    if(pIdlCtxInfo == NULL)
    {
       return CL_IDL_RC(CL_ERR_NO_MEMORY);
    }
    memset(pIdlCtxInfo, 0, sizeof(ClIdlContextInfoT));
    pIdlCtxInfo->idlDeferMsg = outMsgHdl; 
    pIdlCtxInfo->inProgress  = CL_FALSE;
    rc = clIdlSyncPrivateInfoSet(LogidlSyncKey, (void *)pIdlCtxInfo);
    if (CL_OK != rc)
    {
        clHeapFree(pIdlCtxInfo);
        goto L0;
    }
    rc = clLogFileHdlrFileRecordsGet_4_0_0(&(fileName), &(fileLocation), isDelete, &(pStartRec), &(version), &(pNumRecords), &(pBuffeLen), &pLogRecords);
    if(pIdlCtxInfo->inProgress == CL_FALSE)
    {
      clHeapFree(pIdlCtxInfo);
      pIdlCtxInfo = NULL;
    }
    if (CL_OK != rc)
    {
       goto L0;
    }
    
    rc = clXdrMarshallClStringT(&(fileName), 0, 1);
    if (CL_OK != rc)
    {
        goto L1;
    }

    rc = clXdrMarshallClStringT(&(fileLocation), 0, 1);
    if (CL_OK != rc)
    {
        goto L2;
    }

    rc = clXdrMarshallClUint16T(&(isDelete), 0, 1);
    if (CL_OK != rc)
    {
        goto L3;
    }

    if(pIdlCtxInfo != NULL)
    {
      clHeapFree(pIdlCtxInfo);
      return rc;
    }
    
    rc = clXdrMarshallClUint64T(&(pStartRec), outMsgHdl, 1);
    if (CL_OK != rc)
    {
        goto L4;
    }

    rc = clXdrMarshallClUint32T(&(version), outMsgHdl, 1);
    if (CL_OK != rc)
    {
        goto L5;
    }

    rc = clXdrMarshallClUint32T(&(pNumRecords), outMsgHdl, 1);
    if (CL_OK != rc)
    {
        goto L6;
    }

    rc = clXdrMarshallClUint32T(&(pBuffeLen), outMsgHdl, 1);
    if (CL_OK != rc)
    {
        goto L7;
    }

    rc = clXdrMarshallPtrClUint8T(pLogRecords, pBuffeLen, outMsgHdl, 1);
    if (CL_OK != rc)
    {
        goto L8;
    }

L8:    return rc;

LL5:  clXdrMarshallClUint32T(&(pNumRecords), 0, 1);
LL4:  clXdrMarshallClUint32T(&(version), 0, 1);
LL3:  clXdrMarshallClUint64T(&(pStartRec), 0, 1);
LL2:  clXdrMarshallClUint16T(&(isDelete), 0, 1);
LL1:  clXdrMarshallClStringT(&(fileLocation), 0, 1);
LL0:  clXdrMarshallClStringT(&(fileName), 0, 1);

    return rc;

L0:  clXdrMarshallClStringT(&(fileName), 0, 1);
L1:  clXdrMarshallClStringT(&(fileLocation), 0, 1);
L2:  clXdrMarshallClUint16T(&(isDelete), 0, 1);

L3:  clXdrMarshallClUint64T(&(pStartRec), 0, 1);
L4:  clXdrMarshallClUint32T(&(version), 0, 1);
L5:  clXdrMarshallClUint32T(&(pNumRecords), 0, 1);
L6:  clXdrMarshallClUint32T(&(pBuffeLen), 0, 1);
L7:  clXdrMarshallPtrClUint8T(pLogRecords, pBuffeLen, 0, 1);

    return rc;
}

ClRcT clLogFileHdlrFileRecordsGetResponseSend_4_0_0(ClIdlHandleT idlHdl,ClRcT retCode,CL_INOUT  ClUint64T  pStartRec,CL_INOUT  ClUint32T  version,CL_INOUT  ClUint32T  pNumRecords,CL_OUT  ClUint32T  pBuffeLen,ClUint8T*  pLogRecords)
{
    ClIdlSyncInfoT    *pIdlSyncDeferInfo = NULL;
    ClRcT              rc                = CL_OK;
    ClBufferHandleT outMsgHdl     = 0;
    
    rc = clHandleCheckout(LogidlDatabaseHdl,idlHdl,(void **)&pIdlSyncDeferInfo);
    if( rc != CL_OK)
    {
      goto Label0; 
    }
    outMsgHdl = pIdlSyncDeferInfo->idlRmdDeferMsg;
    
    rc = clXdrMarshallClUint64T(&(pStartRec), outMsgHdl, 1);
    if (CL_OK != rc)
    {
        goto L4;
    }

    rc = clXdrMarshallClUint32T(&(version), outMsgHdl, 1);
    if (CL_OK != rc)
    {
        goto L5;
    }

    rc = clXdrMarshallClUint32T(&(pNumRecords), outMsgHdl, 1);
    if (CL_OK != rc)
    {
        goto L6;
    }

    rc = clXdrMarshallClUint32T(&(pBuffeLen), outMsgHdl, 1);
    if (CL_OK != rc)
    {
        goto L7;
    }

    rc = clXdrMarshallPtrClUint8T(pLogRecords, pBuffeLen, outMsgHdl, 1);
    if (CL_OK != rc)
    {
        goto L8;
    }

    rc = clIdlSyncResponseSend(pIdlSyncDeferInfo->idlRmdDeferHdl,outMsgHdl,
                                retCode);
    goto Label1; 
L4:  clXdrMarshallClUint64T(&(pStartRec), 0, 1);
L5:  clXdrMarshallClUint32T(&(version), 0, 1);
L6:  clXdrMarshallClUint32T(&(pNumRecords), 0, 1);
L7:  clXdrMarshallClUint32T(&(pBuffeLen), 0, 1);
L8:  clXdrMarshallPtrClUint8T(pLogRecords, pBuffeLen, 0, 1);

    clHandleCheckin(LogidlDatabaseHdl, idlHdl);
    clHandleDestroy(LogidlDatabaseHdl, idlHdl);
    return rc;
Label1:
    clHandleCheckin(LogidlDatabaseHdl, idlHdl);
    clHandleDestroy(LogidlDatabaseHdl, idlHdl);
Label0:
    return rc;
}

ClRcT clLogFileHdlrFileMetaDataGetServer_4_0_0(ClEoDataT eoData, ClBufferHandleT inMsgHdl, ClBufferHandleT outMsgHdl)
{
    ClIdlContextInfoT *pIdlCtxInfo = NULL;
    ClRcT rc = CL_OK;
    ClStringT  fileName;
    ClStringT  fileLocation;
    ClUint32T  version;
    ClLogStreamAttrIDLT_4_0_0  pStreamAttr;
    ClUint32T  pNumStreams;
    ClLogStreamMapT_4_0_0*  pLogStreams;

    memset(&(fileName), 0, sizeof(ClStringT));
    memset(&(fileLocation), 0, sizeof(ClStringT));
    memset(&(version), 0, sizeof(ClUint32T));
    memset(&(pStreamAttr), 0, sizeof(ClLogStreamAttrIDLT_4_0_0));
    memset(&(pNumStreams), 0, sizeof(ClUint32T));
    memset(&(pLogStreams), 0, sizeof(ClLogStreamMapT_4_0_0*));


    rc = clXdrUnmarshallClStringT( inMsgHdl,&(fileName));
    if (CL_OK != rc)
    {
        goto LL0;
    }

    rc = clXdrUnmarshallClStringT( inMsgHdl,&(fileLocation));
    if (CL_OK != rc)
    {
        goto LL1;
    }

    rc = clXdrUnmarshallClUint32T( inMsgHdl,&(version));
    if (CL_OK != rc)
    {
        goto LL2;
    }


    pIdlCtxInfo = (ClIdlContextInfoT *)clHeapAllocate(sizeof(ClIdlContextInfoT));
    if(pIdlCtxInfo == NULL)
    {
       return CL_IDL_RC(CL_ERR_NO_MEMORY);
    }
    memset(pIdlCtxInfo, 0, sizeof(ClIdlContextInfoT));
    pIdlCtxInfo->idlDeferMsg = outMsgHdl; 
    pIdlCtxInfo->inProgress  = CL_FALSE;
    rc = clIdlSyncPrivateInfoSet(LogidlSyncKey, (void *)pIdlCtxInfo);
    if (CL_OK != rc)
    {
        clHeapFree(pIdlCtxInfo);
        goto L0;
    }
    rc = clLogFileHdlrFileMetaDataGet_4_0_0(&(fileName), &(fileLocation), &(version), &(pStreamAttr), &(pNumStreams), &pLogStreams);
    if(pIdlCtxInfo->inProgress == CL_FALSE)
    {
      clHeapFree(pIdlCtxInfo);
      pIdlCtxInfo = NULL;
    }
    if (CL_OK != rc)
    {
       goto L0;
    }
    
    rc = clXdrMarshallClStringT(&(fileName), 0, 1);
    if (CL_OK != rc)
    {
        goto L1;
    }

    rc = clXdrMarshallClStringT(&(fileLocation), 0, 1);
    if (CL_OK != rc)
    {
        goto L2;
    }

    if(pIdlCtxInfo != NULL)
    {
      clHeapFree(pIdlCtxInfo);
      return rc;
    }
    
    rc = clXdrMarshallClUint32T(&(version), outMsgHdl, 1);
    if (CL_OK != rc)
    {
        goto L3;
    }

    rc = clXdrMarshallClLogStreamAttrIDLT_4_0_0(&(pStreamAttr), outMsgHdl, 1);
    if (CL_OK != rc)
    {
        goto L4;
    }

    rc = clXdrMarshallClUint32T(&(pNumStreams), outMsgHdl, 1);
    if (CL_OK != rc)
    {
        goto L5;
    }

    rc = clXdrMarshallPtrClLogStreamMapT_4_0_0(pLogStreams, pNumStreams, outMsgHdl, 1);
    if (CL_OK != rc)
    {
        goto L6;
    }

L6:    return rc;

LL2:  clXdrMarshallClUint32T(&(version), 0, 1);
LL1:  clXdrMarshallClStringT(&(fileLocation), 0, 1);
LL0:  clXdrMarshallClStringT(&(fileName), 0, 1);

    return rc;

L0:  clXdrMarshallClStringT(&(fileName), 0, 1);
L1:  clXdrMarshallClStringT(&(fileLocation), 0, 1);

L2:  clXdrMarshallClUint32T(&(version), 0, 1);
L3:  clXdrMarshallClLogStreamAttrIDLT_4_0_0(&(pStreamAttr), 0, 1);
L4:  clXdrMarshallClUint32T(&(pNumStreams), 0, 1);
L5:  clXdrMarshallPtrClLogStreamMapT_4_0_0(pLogStreams, pNumStreams, 0, 1);

    return rc;
}

ClRcT clLogFileHdlrFileMetaDataGetResponseSend_4_0_0(ClIdlHandleT idlHdl,ClRcT retCode,CL_INOUT  ClUint32T  version,CL_OUT  ClLogStreamAttrIDLT_4_0_0  pStreamAttr,CL_OUT  ClUint32T  pNumStreams,ClLogStreamMapT_4_0_0*  pLogStreams)
{
    ClIdlSyncInfoT    *pIdlSyncDeferInfo = NULL;
    ClRcT              rc                = CL_OK;
    ClBufferHandleT outMsgHdl     = 0;
    
    rc = clHandleCheckout(LogidlDatabaseHdl,idlHdl,(void **)&pIdlSyncDeferInfo);
    if( rc != CL_OK)
    {
      goto Label0; 
    }
    outMsgHdl = pIdlSyncDeferInfo->idlRmdDeferMsg;
    
    rc = clXdrMarshallClUint32T(&(version), outMsgHdl, 1);
    if (CL_OK != rc)
    {
        goto L3;
    }

    rc = clXdrMarshallClLogStreamAttrIDLT_4_0_0(&(pStreamAttr), outMsgHdl, 1);
    if (CL_OK != rc)
    {
        goto L4;
    }

    rc = clXdrMarshallClUint32T(&(pNumStreams), outMsgHdl, 1);
    if (CL_OK != rc)
    {
        goto L5;
    }

    rc = clXdrMarshallPtrClLogStreamMapT_4_0_0(pLogStreams, pNumStreams, outMsgHdl, 1);
    if (CL_OK != rc)
    {
        goto L6;
    }

    rc = clIdlSyncResponseSend(pIdlSyncDeferInfo->idlRmdDeferHdl,outMsgHdl,
                                retCode);
    goto Label1; 
L3:  clXdrMarshallClUint32T(&(version), 0, 1);
L4:  clXdrMarshallClLogStreamAttrIDLT_4_0_0(&(pStreamAttr), 0, 1);
L5:  clXdrMarshallClUint32T(&(pNumStreams), 0, 1);
L6:  clXdrMarshallPtrClLogStreamMapT_4_0_0(pLogStreams, pNumStreams, 0, 1);

    clHandleCheckin(LogidlDatabaseHdl, idlHdl);
    clHandleDestroy(LogidlDatabaseHdl, idlHdl);
    return rc;
Label1:
    clHandleCheckin(LogidlDatabaseHdl, idlHdl);
    clHandleDestroy(LogidlDatabaseHdl, idlHdl);
Label0:
    return rc;
}

