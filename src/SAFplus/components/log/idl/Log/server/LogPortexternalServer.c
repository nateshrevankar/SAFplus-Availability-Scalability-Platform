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
#include "LogPortexternalServer.h"
#include "LogServer.h"

extern ClUint32T  LogidlSyncKey;
extern ClHandleDatabaseHandleT  LogidlDatabaseHdl;



ClRcT clLogExternalSendServer_4_0_0(ClEoDataT eoData, ClBufferHandleT inMsgHdl, ClBufferHandleT outMsgHdl)
{
    ClIdlContextInfoT *pIdlCtxInfo = NULL;
    ClRcT rc = CL_OK;
    ClUint16T  recsize;
    ClUint8T*  pRecord;
    SaNameT  pStreamName;
    SaNameT  pStreamScopeNode;

    memset(&(recsize), 0, sizeof(ClUint16T));
    memset(&(pRecord), 0, sizeof(ClUint8T*));
    memset(&(pStreamName), 0, sizeof(SaNameT));
    memset(&(pStreamScopeNode), 0, sizeof(SaNameT));


    rc = clXdrUnmarshallClUint16T( inMsgHdl,&(recsize));
    if (CL_OK != rc)
    {
        goto LL0;
    }

    rc = clXdrUnmarshallSaNameT( inMsgHdl,&(pStreamName));
    if (CL_OK != rc)
    {
        goto LL1;
    }

    rc = clXdrUnmarshallSaNameT( inMsgHdl,&(pStreamScopeNode));
    if (CL_OK != rc)
    {
        goto LL2;
    }

    rc = clXdrUnmarshallPtrClUint8T( inMsgHdl,(void**)&(pRecord), recsize);
    if (CL_OK != rc)
    {
        goto LL3;
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
    rc = clLogExternalSend_4_0_0(recsize, pRecord, &(pStreamName), &(pStreamScopeNode));
    if(pIdlCtxInfo->inProgress == CL_FALSE)
    {
      clHeapFree(pIdlCtxInfo);
      pIdlCtxInfo = NULL;
    }
    if (CL_OK != rc)
    {
       goto L0;
    }
    
    rc = clXdrMarshallClUint16T(&(recsize), 0, 1);
    if (CL_OK != rc)
    {
        goto L1;
    }

    rc = clXdrMarshallSaNameT(&(pStreamName), 0, 1);
    if (CL_OK != rc)
    {
        goto L2;
    }

    rc = clXdrMarshallSaNameT(&(pStreamScopeNode), 0, 1);
    if (CL_OK != rc)
    {
        goto L3;
    }

    rc = clXdrMarshallPtrClUint8T(pRecord, recsize, 0, 1);
    if (CL_OK != rc)
    {
        goto L4;
    }

    if(pIdlCtxInfo != NULL)
    {
      clHeapFree(pIdlCtxInfo);
      return rc;
    }
    
L4:    return rc;

LL3:  clXdrMarshallPtrClUint8T(pRecord, recsize, 0, 1);
LL2:  clXdrMarshallSaNameT(&(pStreamScopeNode), 0, 1);
LL1:  clXdrMarshallSaNameT(&(pStreamName), 0, 1);
LL0:  clXdrMarshallClUint16T(&(recsize), 0, 1);

    return rc;

L0:  clXdrMarshallClUint16T(&(recsize), 0, 1);
L1:  clXdrMarshallSaNameT(&(pStreamName), 0, 1);
L2:  clXdrMarshallSaNameT(&(pStreamScopeNode), 0, 1);
L3:  clXdrMarshallPtrClUint8T(pRecord, recsize, 0, 1);


    return rc;
}

ClRcT clLogExternalSendResponseSend_4_0_0(ClIdlHandleT idlHdl,ClRcT retCode)
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
    
    rc = clIdlSyncResponseSend(pIdlSyncDeferInfo->idlRmdDeferHdl,outMsgHdl,
                                retCode);
    

    

    clHandleCheckin(LogidlDatabaseHdl, idlHdl);
    clHandleDestroy(LogidlDatabaseHdl, idlHdl);
Label0:
    return rc;
}

