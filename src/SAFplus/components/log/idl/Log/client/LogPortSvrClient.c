
/*********************************************************************
* ModuleName  : idl
*********************************************************************/
/*********************************************************************
* Description :ClientSide Stub routines
*
* THIS FILE IS AUTO-GENERATED BY OPENCLOVIS IDE. EDIT THIS FILE AT
* YOUR OWN RISK. ANY CHANGE TO THIS FILE WILL BE OVERWRITTEN ON
* RE-GENERATION.
*
*********************************************************************/

#include <netinet/in.h>
#include <string.h>
#include <clBufferApi.h>
#include <clRmdApi.h>
#include <clIdlApi.h>
#include <clEoApi.h>
#include <clXdrApi.h>
#include <clHandleApi.h>
#include "LogPortSvrClient.h"
extern ClIdlClntT gIdlClnt;


ClRcT clLogSvrStreamOpenClientSync_4_0_0(CL_IN ClIdlHandleT handle, CL_IN ClNameT* streamName, CL_IN ClUint32T streamScope, CL_IN ClNameT* streamScopeNode, CL_IN ClLogStreamAttrIDLT_4_0_0* pStreamAttr, CL_IN ClUint8T streamOpenFlags, CL_IN ClUint32T compId, CL_IN ClUint32T portId, CL_OUT ClStringT* pShmName, CL_OUT ClUint32T* pShmSize)
{
    ClRcT rc = CL_OK;
    ClVersionT funcVer = {4, 0, 0};
    ClUint32T funcNo = CL_EO_GET_FULL_FN_NUM(CL_EO_NATIVE_COMPONENT_TABLE_ID, 13);
    ClBufferHandleT inMsgHdl = 0;
    ClBufferHandleT outMsgHdl = 0;
    ClIocAddressT address = {{0}};
    ClIdlHandleObjT* pHandleObj = NULL;
    ClUint32T tempFlags = 0;

    rc = clHandleCheckout(gIdlClnt.idlDbHdl,handle,(void **)&pHandleObj);
    if( rc != CL_OK )
    {
        return rc ;
    }
    if (CL_IDL_ADDRESSTYPE_IOC == pHandleObj->address.addressType)
    {
        address = pHandleObj->address.address.iocAddress;
    }
    else if (CL_IDL_ADDRESSTYPE_NAME == pHandleObj->address.addressType)
    {
        rc = clNameToObjectReferenceGet(&(pHandleObj->address.address.nameAddress.name),
                                        pHandleObj->address.address.nameAddress.attrCount,
                                        pHandleObj->address.address.nameAddress.attr,
                                        pHandleObj->address.address.nameAddress.contextCookie,
                                        (ClUint64T*)&address);
        if (CL_OK != rc)
        {
            return rc;
        }
    }
    else
    {
        return CL_IDL_RC(CL_ERR_INVALID_PARAMETER);
    }

    rc = clBufferCreate(&inMsgHdl);
    if (CL_OK != rc)
    {
        return rc;
    }

    rc = clXdrMarshallClNameT(streamName, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        return rc;
    }

    rc = clXdrMarshallClUint32T(&(streamScope), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        return rc;
    }

    rc = clXdrMarshallClNameT(streamScopeNode, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        return rc;
    }

    rc = clXdrMarshallClLogStreamAttrIDLT_4_0_0(pStreamAttr, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        return rc;
    }

    rc = clXdrMarshallClUint8T(&(streamOpenFlags), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        return rc;
    }

    rc = clXdrMarshallClUint32T(&(compId), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        return rc;
    }

    rc = clXdrMarshallClUint32T(&(portId), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        return rc;
    }


    rc = clBufferCreate(&outMsgHdl);
    if (CL_OK != rc)
    {
        return rc;
    }

    tempFlags |= pHandleObj->flags |
                 (CL_RMD_CALL_NON_PERSISTENT | CL_RMD_CALL_NEED_REPLY);
    tempFlags &= ~CL_RMD_CALL_ASYNC;

    rc = clRmdWithMsgVer(address, &funcVer, funcNo, inMsgHdl, outMsgHdl, tempFlags, &(pHandleObj->options), NULL);
    if(CL_OK != rc)
    {
        clBufferDelete(&outMsgHdl);
    return rc;
    }


    rc = clXdrUnmarshallClStringT( outMsgHdl, pShmName);
    if (CL_OK != rc)
    {
        return rc;
    }

    rc = clXdrUnmarshallClUint32T( outMsgHdl, pShmSize);
    if (CL_OK != rc)
    {
        return rc;
    }

    clBufferDelete(&outMsgHdl);
    
    rc = clHandleCheckin(gIdlClnt.idlDbHdl,handle);
    return rc;
}


static void clLogSvrStreamOpenAsyncCallback_4_0_0(ClRcT rc, void *pIdlCookie, ClBufferHandleT inMsgHdl, ClBufferHandleT outMsgHdl)
{
    ClIdlCookieT* pCookie = (ClIdlCookieT*)pIdlCookie;
    ClRcT retVal = CL_OK;
    ClNameT  streamName;
    ClUint32T  streamScope;
    ClNameT  streamScopeNode;
    ClLogStreamAttrIDLT_4_0_0  pStreamAttr;
    ClUint8T  streamOpenFlags;
    ClUint32T  compId;
    ClUint32T  portId;
    ClStringT  pShmName;
    ClUint32T  pShmSize;

    memset(&(streamName), 0, sizeof(ClNameT));
    memset(&(streamScope), 0, sizeof(ClUint32T));
    memset(&(streamScopeNode), 0, sizeof(ClNameT));
    memset(&(pStreamAttr), 0, sizeof(ClLogStreamAttrIDLT_4_0_0));
    memset(&(streamOpenFlags), 0, sizeof(ClUint8T));
    memset(&(compId), 0, sizeof(ClUint32T));
    memset(&(portId), 0, sizeof(ClUint32T));
    memset(&(pShmName), 0, sizeof(ClStringT));
    memset(&(pShmSize), 0, sizeof(ClUint32T));


    retVal = clXdrUnmarshallClNameT(inMsgHdl, &(streamName));
    if (CL_OK != retVal)
    {
        goto L0;
    }

    retVal = clXdrUnmarshallClUint32T(inMsgHdl, &(streamScope));
    if (CL_OK != retVal)
    {
        goto L1;
    }

    retVal = clXdrUnmarshallClNameT(inMsgHdl, &(streamScopeNode));
    if (CL_OK != retVal)
    {
        goto L2;
    }

    retVal = clXdrUnmarshallClLogStreamAttrIDLT_4_0_0(inMsgHdl, &(pStreamAttr));
    if (CL_OK != retVal)
    {
        goto L3;
    }

    retVal = clXdrUnmarshallClUint8T(inMsgHdl, &(streamOpenFlags));
    if (CL_OK != retVal)
    {
        goto L4;
    }

    retVal = clXdrUnmarshallClUint32T(inMsgHdl, &(compId));
    if (CL_OK != retVal)
    {
        goto L5;
    }

    retVal = clXdrUnmarshallClUint32T(inMsgHdl, &(portId));
    if (CL_OK != retVal)
    {
        goto L6;
    }

    if (CL_OK == rc)
    {
        retVal = clXdrUnmarshallClStringT(outMsgHdl, &(pShmName));
        if (CL_OK != retVal)
        {
            goto L7;
        }
    }

    if (CL_OK == rc)
    {
        retVal = clXdrUnmarshallClUint32T(outMsgHdl, &(pShmSize));
        if (CL_OK != retVal)
        {
            goto L8;
        }
    }

    if (rc != CL_OK)
    {
        retVal = rc;
    }

    ((LogClLogSvrStreamOpenAsyncCallbackT_4_0_0)(pCookie->actualCallback))(pCookie->handle, &(streamName), streamScope, &(streamScopeNode), &(pStreamAttr), streamOpenFlags, compId, portId, &(pShmName), &(pShmSize), retVal, pCookie->pCookie);
    goto L9;

L9: 
L8: 
L7: 
L6: 
L5: 
L4: 
L3: 
L2: 
L1: 

L0:  clHeapFree(pCookie);
     clBufferDelete(&outMsgHdl);
     return;
}


ClRcT clLogSvrStreamOpenClientAsync_4_0_0(CL_IN ClIdlHandleT handle, CL_IN ClNameT* streamName, CL_IN ClUint32T  streamScope, CL_IN ClNameT* streamScopeNode, CL_IN ClLogStreamAttrIDLT_4_0_0* pStreamAttr, CL_IN ClUint8T  streamOpenFlags, CL_IN ClUint32T  compId, CL_IN ClUint32T  portId, CL_OUT ClStringT* pShmName, CL_OUT ClUint32T* pShmSize,CL_IN LogClLogSvrStreamOpenAsyncCallbackT_4_0_0 fpAsyncCallback, CL_IN void *cookie)
{
    ClRcT rc = CL_OK;
    ClVersionT funcVer = {4, 0, 0};
    ClUint32T funcNo = CL_EO_GET_FULL_FN_NUM(CL_EO_NATIVE_COMPONENT_TABLE_ID, 13);
    ClBufferHandleT inMsgHdl = 0;
    ClBufferHandleT outMsgHdl = 0;
    ClIocAddressT address = {{0}};
    ClIdlHandleObjT* pHandleObj = NULL;
    ClRmdAsyncOptionsT asyncOptions;
    ClUint32T tempFlags = 0;
    ClIdlCookieT* pCookie = NULL;

    rc = clHandleCheckout(gIdlClnt.idlDbHdl,handle,(void **)&pHandleObj);
    if(rc != CL_OK)
    {
        return rc;
    }
    if (CL_IDL_ADDRESSTYPE_IOC == pHandleObj->address.addressType)
    {
        address = pHandleObj->address.address.iocAddress;
    }
    else if (CL_IDL_ADDRESSTYPE_NAME == pHandleObj->address.addressType)
    {
        rc = clNameToObjectReferenceGet(&(pHandleObj->address.address.nameAddress.name),
                                        pHandleObj->address.address.nameAddress.attrCount,
                                        pHandleObj->address.address.nameAddress.attr,
                                        pHandleObj->address.address.nameAddress.contextCookie,
                                        (ClUint64T*)&address);
        if (CL_OK != rc)
        {
            goto L;
        }
    }
    else
    {
        rc = CL_IDL_RC(CL_ERR_INVALID_PARAMETER);
        goto L;
    }

    rc = clBufferCreate(&inMsgHdl);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClNameT(streamName, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClUint32T(&(streamScope), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClNameT(streamScopeNode, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClLogStreamAttrIDLT_4_0_0(pStreamAttr, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClUint8T(&(streamOpenFlags), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClUint32T(&(compId), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClUint32T(&(portId), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    if(fpAsyncCallback != NULL)
    {
        

        pCookie = clHeapAllocate(sizeof(ClIdlCookieT));
        if (NULL == pCookie)
        {
            return CL_IDL_RC(CL_ERR_NO_MEMORY);
        }
        
        asyncOptions.pCookie = NULL;
        asyncOptions.fpCallback = NULL;
        
        rc = clBufferCreate(&outMsgHdl);
        if (CL_OK != rc)
        {
            goto L2;
        }

        tempFlags |= pHandleObj->flags |
                     (CL_RMD_CALL_ASYNC | CL_RMD_CALL_NON_PERSISTENT | CL_RMD_CALL_NEED_REPLY);
        
        pCookie->pCookie = cookie;
        pCookie->actualCallback = (void(*)())fpAsyncCallback;
        pCookie->handle = handle;
        asyncOptions.pCookie = pCookie;
        asyncOptions.fpCallback = clLogSvrStreamOpenAsyncCallback_4_0_0;

        rc = clRmdWithMsgVer(address, &funcVer, funcNo, inMsgHdl, outMsgHdl, tempFlags, &(pHandleObj->options), &asyncOptions);
        if (CL_OK != rc)
        {
            goto LL;
         }
    }
    else
    {
        tempFlags |= pHandleObj->flags |
                         (CL_RMD_CALL_ASYNC | CL_RMD_CALL_NON_PERSISTENT);
        rc = clRmdWithMsgVer(address, &funcVer, funcNo, inMsgHdl, 0, tempFlags, &(pHandleObj->options),NULL);
        if(CL_OK != rc)
        {
               goto L;
        }
    }
    
    
    clHandleCheckin(gIdlClnt.idlDbHdl,handle);
    return rc;

LL: clBufferDelete(&outMsgHdl);
L2:  clHeapFree(pCookie);
L:
     clHandleCheckin(gIdlClnt.idlDbHdl,handle);
    return rc;
}


ClRcT clLogSvrStreamCloseClientSync_4_0_0(CL_IN ClIdlHandleT handle, CL_IN ClNameT* streamName, CL_IN ClUint32T streamScope, CL_IN ClNameT* streamScopeNode, CL_IN ClUint32T compId)
{
    ClRcT rc = CL_OK;
    ClVersionT funcVer = {4, 0, 0};
    ClUint32T funcNo = CL_EO_GET_FULL_FN_NUM(CL_EO_NATIVE_COMPONENT_TABLE_ID, 14);
    ClBufferHandleT inMsgHdl = 0;
    ClBufferHandleT outMsgHdl = 0;
    ClIocAddressT address = {{0}};
    ClIdlHandleObjT* pHandleObj = NULL;
    ClUint32T tempFlags = 0;

    rc = clHandleCheckout(gIdlClnt.idlDbHdl,handle,(void **)&pHandleObj);
    if( rc != CL_OK )
    {
        return rc ;
    }
    if (CL_IDL_ADDRESSTYPE_IOC == pHandleObj->address.addressType)
    {
        address = pHandleObj->address.address.iocAddress;
    }
    else if (CL_IDL_ADDRESSTYPE_NAME == pHandleObj->address.addressType)
    {
        rc = clNameToObjectReferenceGet(&(pHandleObj->address.address.nameAddress.name),
                                        pHandleObj->address.address.nameAddress.attrCount,
                                        pHandleObj->address.address.nameAddress.attr,
                                        pHandleObj->address.address.nameAddress.contextCookie,
                                        (ClUint64T*)&address);
        if (CL_OK != rc)
        {
            return rc;
        }
    }
    else
    {
        return CL_IDL_RC(CL_ERR_INVALID_PARAMETER);
    }

    rc = clBufferCreate(&inMsgHdl);
    if (CL_OK != rc)
    {
        return rc;
    }

    rc = clXdrMarshallClNameT(streamName, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        return rc;
    }

    rc = clXdrMarshallClUint32T(&(streamScope), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        return rc;
    }

    rc = clXdrMarshallClNameT(streamScopeNode, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        return rc;
    }

    rc = clXdrMarshallClUint32T(&(compId), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        return rc;
    }


    tempFlags |= pHandleObj->flags |
                 (CL_RMD_CALL_NON_PERSISTENT);
    tempFlags &= ~CL_RMD_CALL_ASYNC;

    rc = clRmdWithMsgVer(address, &funcVer, funcNo, inMsgHdl, outMsgHdl, tempFlags, &(pHandleObj->options), NULL);
    if(CL_OK != rc)
    {
        return rc;
    }


    
    rc = clHandleCheckin(gIdlClnt.idlDbHdl,handle);
    return rc;
}


static void clLogSvrStreamCloseAsyncCallback_4_0_0(ClRcT rc, void *pIdlCookie, ClBufferHandleT inMsgHdl, ClBufferHandleT outMsgHdl)
{
    ClIdlCookieT* pCookie = (ClIdlCookieT*)pIdlCookie;
    ClRcT retVal = CL_OK;
    ClNameT  streamName;
    ClUint32T  streamScope;
    ClNameT  streamScopeNode;
    ClUint32T  compId;

    memset(&(streamName), 0, sizeof(ClNameT));
    memset(&(streamScope), 0, sizeof(ClUint32T));
    memset(&(streamScopeNode), 0, sizeof(ClNameT));
    memset(&(compId), 0, sizeof(ClUint32T));


    retVal = clXdrUnmarshallClNameT(inMsgHdl, &(streamName));
    if (CL_OK != retVal)
    {
        goto L0;
    }

    retVal = clXdrUnmarshallClUint32T(inMsgHdl, &(streamScope));
    if (CL_OK != retVal)
    {
        goto L1;
    }

    retVal = clXdrUnmarshallClNameT(inMsgHdl, &(streamScopeNode));
    if (CL_OK != retVal)
    {
        goto L2;
    }

    retVal = clXdrUnmarshallClUint32T(inMsgHdl, &(compId));
    if (CL_OK != retVal)
    {
        goto L3;
    }

    if (rc != CL_OK)
    {
        retVal = rc;
    }

    ((LogClLogSvrStreamCloseAsyncCallbackT_4_0_0)(pCookie->actualCallback))(pCookie->handle, &(streamName), streamScope, &(streamScopeNode), compId, retVal, pCookie->pCookie);
    goto L4;

L4: 
L3: 
L2: 
L1: 

L0:  clHeapFree(pCookie);
     clBufferDelete(&outMsgHdl);
     return;
}


ClRcT clLogSvrStreamCloseClientAsync_4_0_0(CL_IN ClIdlHandleT handle, CL_IN ClNameT* streamName, CL_IN ClUint32T  streamScope, CL_IN ClNameT* streamScopeNode, CL_IN ClUint32T  compId,CL_IN LogClLogSvrStreamCloseAsyncCallbackT_4_0_0 fpAsyncCallback, CL_IN void *cookie)
{
    ClRcT rc = CL_OK;
    ClVersionT funcVer = {4, 0, 0};
    ClUint32T funcNo = CL_EO_GET_FULL_FN_NUM(CL_EO_NATIVE_COMPONENT_TABLE_ID, 14);
    ClBufferHandleT inMsgHdl = 0;
    ClBufferHandleT outMsgHdl = 0;
    ClIocAddressT address = {{0}};
    ClIdlHandleObjT* pHandleObj = NULL;
    ClRmdAsyncOptionsT asyncOptions;
    ClUint32T tempFlags = 0;
    ClIdlCookieT* pCookie = NULL;

    rc = clHandleCheckout(gIdlClnt.idlDbHdl,handle,(void **)&pHandleObj);
    if(rc != CL_OK)
    {
        return rc;
    }
    if (CL_IDL_ADDRESSTYPE_IOC == pHandleObj->address.addressType)
    {
        address = pHandleObj->address.address.iocAddress;
    }
    else if (CL_IDL_ADDRESSTYPE_NAME == pHandleObj->address.addressType)
    {
        rc = clNameToObjectReferenceGet(&(pHandleObj->address.address.nameAddress.name),
                                        pHandleObj->address.address.nameAddress.attrCount,
                                        pHandleObj->address.address.nameAddress.attr,
                                        pHandleObj->address.address.nameAddress.contextCookie,
                                        (ClUint64T*)&address);
        if (CL_OK != rc)
        {
            goto L;
        }
    }
    else
    {
        rc = CL_IDL_RC(CL_ERR_INVALID_PARAMETER);
        goto L;
    }

    rc = clBufferCreate(&inMsgHdl);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClNameT(streamName, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClUint32T(&(streamScope), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClNameT(streamScopeNode, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClUint32T(&(compId), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    if(fpAsyncCallback != NULL)
    {
        

        pCookie = clHeapAllocate(sizeof(ClIdlCookieT));
        if (NULL == pCookie)
        {
            return CL_IDL_RC(CL_ERR_NO_MEMORY);
        }
        
        asyncOptions.pCookie = NULL;
        asyncOptions.fpCallback = NULL;
        
        rc = clBufferCreate(&outMsgHdl);
        if (CL_OK != rc)
        {
            goto L2;
        }

        tempFlags |= pHandleObj->flags |
                     (CL_RMD_CALL_ASYNC | CL_RMD_CALL_NON_PERSISTENT | CL_RMD_CALL_NEED_REPLY);
        
        pCookie->pCookie = cookie;
        pCookie->actualCallback = (void(*)())fpAsyncCallback;
        pCookie->handle = handle;
        asyncOptions.pCookie = pCookie;
        asyncOptions.fpCallback = clLogSvrStreamCloseAsyncCallback_4_0_0;

        rc = clRmdWithMsgVer(address, &funcVer, funcNo, inMsgHdl, outMsgHdl, tempFlags, &(pHandleObj->options), &asyncOptions);
        if (CL_OK != rc)
        {
            goto LL;
         }
    }
    else
    {
        tempFlags |= pHandleObj->flags |
                         (CL_RMD_CALL_ASYNC | CL_RMD_CALL_NON_PERSISTENT);
        rc = clRmdWithMsgVer(address, &funcVer, funcNo, inMsgHdl, 0, tempFlags, &(pHandleObj->options),NULL);
        if(CL_OK != rc)
        {
               goto L;
        }
    }
    
    
    clHandleCheckin(gIdlClnt.idlDbHdl,handle);
    return rc;

LL: clBufferDelete(&outMsgHdl);
L2:  clHeapFree(pCookie);
L:
     clHandleCheckin(gIdlClnt.idlDbHdl,handle);
    return rc;
}



static void clLogSvrFilterSetAsyncCallback_4_0_0(ClRcT rc, void *pIdlCookie, ClBufferHandleT inMsgHdl, ClBufferHandleT outMsgHdl)
{
    ClIdlCookieT* pCookie = (ClIdlCookieT*)pIdlCookie;
    ClRcT retVal = CL_OK;
    ClNameT  pStreamName;
    ClUint32T  streamScope;
    ClNameT  pStreamScopeNode;
    ClLogFilterT_4_0_0  pFilter;

    memset(&(pStreamName), 0, sizeof(ClNameT));
    memset(&(streamScope), 0, sizeof(ClUint32T));
    memset(&(pStreamScopeNode), 0, sizeof(ClNameT));
    memset(&(pFilter), 0, sizeof(ClLogFilterT_4_0_0));


    retVal = clXdrUnmarshallClNameT(inMsgHdl, &(pStreamName));
    if (CL_OK != retVal)
    {
        goto L0;
    }

    retVal = clXdrUnmarshallClUint32T(inMsgHdl, &(streamScope));
    if (CL_OK != retVal)
    {
        goto L1;
    }

    retVal = clXdrUnmarshallClNameT(inMsgHdl, &(pStreamScopeNode));
    if (CL_OK != retVal)
    {
        goto L2;
    }

    retVal = clXdrUnmarshallClLogFilterT_4_0_0(inMsgHdl, &(pFilter));
    if (CL_OK != retVal)
    {
        goto L3;
    }

    if (rc != CL_OK)
    {
        retVal = rc;
    }

    ((LogClLogSvrFilterSetAsyncCallbackT_4_0_0)(pCookie->actualCallback))(pCookie->handle, &(pStreamName), streamScope, &(pStreamScopeNode), &(pFilter), retVal, pCookie->pCookie);
    goto L4;

L4: 
L3: 
L2: 
L1: 

L0:  clHeapFree(pCookie);
     clBufferDelete(&outMsgHdl);
     return;
}


ClRcT clLogSvrFilterSetClientAsync_4_0_0(CL_IN ClIdlHandleT handle, CL_IN ClNameT* pStreamName, CL_IN ClUint32T  streamScope, CL_IN ClNameT* pStreamScopeNode, CL_IN ClLogFilterT_4_0_0* pFilter,CL_IN LogClLogSvrFilterSetAsyncCallbackT_4_0_0 fpAsyncCallback, CL_IN void *cookie)
{
    ClRcT rc = CL_OK;
    ClVersionT funcVer = {4, 0, 0};
    ClUint32T funcNo = CL_EO_GET_FULL_FN_NUM(CL_EO_NATIVE_COMPONENT_TABLE_ID, 15);
    ClBufferHandleT inMsgHdl = 0;
    ClBufferHandleT outMsgHdl = 0;
    ClIocAddressT address = {{0}};
    ClIdlHandleObjT* pHandleObj = NULL;
    ClRmdAsyncOptionsT asyncOptions;
    ClUint32T tempFlags = 0;
    ClIdlCookieT* pCookie = NULL;

    rc = clHandleCheckout(gIdlClnt.idlDbHdl,handle,(void **)&pHandleObj);
    if(rc != CL_OK)
    {
        return rc;
    }
    if (CL_IDL_ADDRESSTYPE_IOC == pHandleObj->address.addressType)
    {
        address = pHandleObj->address.address.iocAddress;
    }
    else if (CL_IDL_ADDRESSTYPE_NAME == pHandleObj->address.addressType)
    {
        rc = clNameToObjectReferenceGet(&(pHandleObj->address.address.nameAddress.name),
                                        pHandleObj->address.address.nameAddress.attrCount,
                                        pHandleObj->address.address.nameAddress.attr,
                                        pHandleObj->address.address.nameAddress.contextCookie,
                                        (ClUint64T*)&address);
        if (CL_OK != rc)
        {
            goto L;
        }
    }
    else
    {
        rc = CL_IDL_RC(CL_ERR_INVALID_PARAMETER);
        goto L;
    }

    rc = clBufferCreate(&inMsgHdl);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClNameT(pStreamName, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClUint32T(&(streamScope), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClNameT(pStreamScopeNode, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClLogFilterT_4_0_0(pFilter, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    if(fpAsyncCallback != NULL)
    {
        

        pCookie = clHeapAllocate(sizeof(ClIdlCookieT));
        if (NULL == pCookie)
        {
            return CL_IDL_RC(CL_ERR_NO_MEMORY);
        }
        
        asyncOptions.pCookie = NULL;
        asyncOptions.fpCallback = NULL;
        
        rc = clBufferCreate(&outMsgHdl);
        if (CL_OK != rc)
        {
            goto L2;
        }

        tempFlags |= pHandleObj->flags |
                     (CL_RMD_CALL_ASYNC | CL_RMD_CALL_NON_PERSISTENT | CL_RMD_CALL_NEED_REPLY);
        
        pCookie->pCookie = cookie;
        pCookie->actualCallback = (void(*)())fpAsyncCallback;
        pCookie->handle = handle;
        asyncOptions.pCookie = pCookie;
        asyncOptions.fpCallback = clLogSvrFilterSetAsyncCallback_4_0_0;

        rc = clRmdWithMsgVer(address, &funcVer, funcNo, inMsgHdl, outMsgHdl, tempFlags, &(pHandleObj->options), &asyncOptions);
        if (CL_OK != rc)
        {
            goto LL;
         }
    }
    else
    {
        tempFlags |= pHandleObj->flags |
                         (CL_RMD_CALL_ASYNC | CL_RMD_CALL_NON_PERSISTENT);
        rc = clRmdWithMsgVer(address, &funcVer, funcNo, inMsgHdl, 0, tempFlags, &(pHandleObj->options),NULL);
        if(CL_OK != rc)
        {
               goto L;
        }
    }
    
    
    clHandleCheckin(gIdlClnt.idlDbHdl,handle);
    return rc;

LL: clBufferDelete(&outMsgHdl);
L2:  clHeapFree(pCookie);
L:
     clHandleCheckin(gIdlClnt.idlDbHdl,handle);
    return rc;
}



static void clLogSvrStreamHandleFlagsUpdateAsyncCallback_4_0_0(ClRcT rc, void *pIdlCookie, ClBufferHandleT inMsgHdl, ClBufferHandleT outMsgHdl)
{
    ClIdlCookieT* pCookie = (ClIdlCookieT*)pIdlCookie;
    ClRcT retVal = CL_OK;
    ClNameT  pStreamName;
    ClUint32T  streamScope;
    ClNameT  pStreamScopeNode;
    ClUint8T  handleFlags;
    ClUint16T  setFlags;

    memset(&(pStreamName), 0, sizeof(ClNameT));
    memset(&(streamScope), 0, sizeof(ClUint32T));
    memset(&(pStreamScopeNode), 0, sizeof(ClNameT));
    memset(&(handleFlags), 0, sizeof(ClUint8T));
    memset(&(setFlags), 0, sizeof(ClUint16T));


    retVal = clXdrUnmarshallClNameT(inMsgHdl, &(pStreamName));
    if (CL_OK != retVal)
    {
        goto L0;
    }

    retVal = clXdrUnmarshallClUint32T(inMsgHdl, &(streamScope));
    if (CL_OK != retVal)
    {
        goto L1;
    }

    retVal = clXdrUnmarshallClNameT(inMsgHdl, &(pStreamScopeNode));
    if (CL_OK != retVal)
    {
        goto L2;
    }

    retVal = clXdrUnmarshallClUint8T(inMsgHdl, &(handleFlags));
    if (CL_OK != retVal)
    {
        goto L3;
    }

    retVal = clXdrUnmarshallClUint16T(inMsgHdl, &(setFlags));
    if (CL_OK != retVal)
    {
        goto L4;
    }

    if (rc != CL_OK)
    {
        retVal = rc;
    }

    ((LogClLogSvrStreamHandleFlagsUpdateAsyncCallbackT_4_0_0)(pCookie->actualCallback))(pCookie->handle, &(pStreamName), streamScope, &(pStreamScopeNode), handleFlags, setFlags, retVal, pCookie->pCookie);
    goto L5;

L5: 
L4: 
L3: 
L2: 
L1: 

L0:  clHeapFree(pCookie);
     clBufferDelete(&outMsgHdl);
     return;
}


ClRcT clLogSvrStreamHandleFlagsUpdateClientAsync_4_0_0(CL_IN ClIdlHandleT handle, CL_IN ClNameT* pStreamName, CL_IN ClUint32T  streamScope, CL_IN ClNameT* pStreamScopeNode, CL_IN ClUint8T  handleFlags, CL_IN ClUint16T  setFlags,CL_IN LogClLogSvrStreamHandleFlagsUpdateAsyncCallbackT_4_0_0 fpAsyncCallback, CL_IN void *cookie)
{
    ClRcT rc = CL_OK;
    ClVersionT funcVer = {4, 0, 0};
    ClUint32T funcNo = CL_EO_GET_FULL_FN_NUM(CL_EO_NATIVE_COMPONENT_TABLE_ID, 16);
    ClBufferHandleT inMsgHdl = 0;
    ClBufferHandleT outMsgHdl = 0;
    ClIocAddressT address = {{0}};
    ClIdlHandleObjT* pHandleObj = NULL;
    ClRmdAsyncOptionsT asyncOptions;
    ClUint32T tempFlags = 0;
    ClIdlCookieT* pCookie = NULL;

    rc = clHandleCheckout(gIdlClnt.idlDbHdl,handle,(void **)&pHandleObj);
    if(rc != CL_OK)
    {
        return rc;
    }
    if (CL_IDL_ADDRESSTYPE_IOC == pHandleObj->address.addressType)
    {
        address = pHandleObj->address.address.iocAddress;
    }
    else if (CL_IDL_ADDRESSTYPE_NAME == pHandleObj->address.addressType)
    {
        rc = clNameToObjectReferenceGet(&(pHandleObj->address.address.nameAddress.name),
                                        pHandleObj->address.address.nameAddress.attrCount,
                                        pHandleObj->address.address.nameAddress.attr,
                                        pHandleObj->address.address.nameAddress.contextCookie,
                                        (ClUint64T*)&address);
        if (CL_OK != rc)
        {
            goto L;
        }
    }
    else
    {
        rc = CL_IDL_RC(CL_ERR_INVALID_PARAMETER);
        goto L;
    }

    rc = clBufferCreate(&inMsgHdl);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClNameT(pStreamName, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClUint32T(&(streamScope), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClNameT(pStreamScopeNode, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClUint8T(&(handleFlags), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClUint16T(&(setFlags), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    if(fpAsyncCallback != NULL)
    {
        

        pCookie = clHeapAllocate(sizeof(ClIdlCookieT));
        if (NULL == pCookie)
        {
            return CL_IDL_RC(CL_ERR_NO_MEMORY);
        }
        
        asyncOptions.pCookie = NULL;
        asyncOptions.fpCallback = NULL;
        
        rc = clBufferCreate(&outMsgHdl);
        if (CL_OK != rc)
        {
            goto L2;
        }

        tempFlags |= pHandleObj->flags |
                     (CL_RMD_CALL_ASYNC | CL_RMD_CALL_NON_PERSISTENT | CL_RMD_CALL_NEED_REPLY);
        
        pCookie->pCookie = cookie;
        pCookie->actualCallback = (void(*)())fpAsyncCallback;
        pCookie->handle = handle;
        asyncOptions.pCookie = pCookie;
        asyncOptions.fpCallback = clLogSvrStreamHandleFlagsUpdateAsyncCallback_4_0_0;

        rc = clRmdWithMsgVer(address, &funcVer, funcNo, inMsgHdl, outMsgHdl, tempFlags, &(pHandleObj->options), &asyncOptions);
        if (CL_OK != rc)
        {
            goto LL;
         }
    }
    else
    {
        tempFlags |= pHandleObj->flags |
                         (CL_RMD_CALL_ASYNC | CL_RMD_CALL_NON_PERSISTENT);
        rc = clRmdWithMsgVer(address, &funcVer, funcNo, inMsgHdl, 0, tempFlags, &(pHandleObj->options),NULL);
        if(CL_OK != rc)
        {
               goto L;
        }
    }
    
    
    clHandleCheckin(gIdlClnt.idlDbHdl,handle);
    return rc;

LL: clBufferDelete(&outMsgHdl);
L2:  clHeapFree(pCookie);
L:
     clHandleCheckin(gIdlClnt.idlDbHdl,handle);
    return rc;
}



static void clLogHandlerSvrAckSendAsyncCallback_4_0_0(ClRcT rc, void *pIdlCookie, ClBufferHandleT inMsgHdl, ClBufferHandleT outMsgHdl)
{
    ClIdlCookieT* pCookie = (ClIdlCookieT*)pIdlCookie;
    ClRcT retVal = CL_OK;
    ClNameT  pStreamName;
    ClNameT  pNodeName;
    ClUint32T  streamScope;
    ClUint32T  sequenceNumber;
    ClUint32T  numRecords;
    ClHandleT  flushCookie;

    memset(&(pStreamName), 0, sizeof(ClNameT));
    memset(&(pNodeName), 0, sizeof(ClNameT));
    memset(&(streamScope), 0, sizeof(ClUint32T));
    memset(&(sequenceNumber), 0, sizeof(ClUint32T));
    memset(&(numRecords), 0, sizeof(ClUint32T));
    memset(&(flushCookie), 0, sizeof(ClHandleT));


    retVal = clXdrUnmarshallClNameT(inMsgHdl, &(pStreamName));
    if (CL_OK != retVal)
    {
        goto L0;
    }

    retVal = clXdrUnmarshallClNameT(inMsgHdl, &(pNodeName));
    if (CL_OK != retVal)
    {
        goto L1;
    }

    retVal = clXdrUnmarshallClUint32T(inMsgHdl, &(streamScope));
    if (CL_OK != retVal)
    {
        goto L2;
    }

    retVal = clXdrUnmarshallClUint32T(inMsgHdl, &(sequenceNumber));
    if (CL_OK != retVal)
    {
        goto L3;
    }

    retVal = clXdrUnmarshallClUint32T(inMsgHdl, &(numRecords));
    if (CL_OK != retVal)
    {
        goto L4;
    }

    retVal = clXdrUnmarshallClHandleT(inMsgHdl, &(flushCookie));
    if (CL_OK != retVal)
    {
        goto L5;
    }

    if (rc != CL_OK)
    {
        retVal = rc;
    }

    ((LogClLogHandlerSvrAckSendAsyncCallbackT_4_0_0)(pCookie->actualCallback))(pCookie->handle, &(pStreamName), &(pNodeName), streamScope, sequenceNumber, numRecords, flushCookie, retVal, pCookie->pCookie);
    goto L6;

L6: 
L5: 
L4: 
L3: 
L2: 
L1: 

L0:  clHeapFree(pCookie);
     clBufferDelete(&outMsgHdl);
     return;
}


ClRcT clLogHandlerSvrAckSendClientAsync_4_0_0(CL_IN ClIdlHandleT handle, CL_IN ClNameT* pStreamName, CL_IN ClNameT* pNodeName, CL_IN ClUint32T  streamScope, CL_IN ClUint32T  sequenceNumber, CL_IN ClUint32T  numRecords, CL_IN ClHandleT  flushCookie,CL_IN LogClLogHandlerSvrAckSendAsyncCallbackT_4_0_0 fpAsyncCallback, CL_IN void *cookie)
{
    ClRcT rc = CL_OK;
    ClVersionT funcVer = {4, 0, 0};
    ClUint32T funcNo = CL_EO_GET_FULL_FN_NUM(CL_EO_NATIVE_COMPONENT_TABLE_ID, 17);
    ClBufferHandleT inMsgHdl = 0;
    ClBufferHandleT outMsgHdl = 0;
    ClIocAddressT address = {{0}};
    ClIdlHandleObjT* pHandleObj = NULL;
    ClRmdAsyncOptionsT asyncOptions;
    ClUint32T tempFlags = 0;
    ClIdlCookieT* pCookie = NULL;

    rc = clHandleCheckout(gIdlClnt.idlDbHdl,handle,(void **)&pHandleObj);
    if(rc != CL_OK)
    {
        return rc;
    }
    if (CL_IDL_ADDRESSTYPE_IOC == pHandleObj->address.addressType)
    {
        address = pHandleObj->address.address.iocAddress;
    }
    else if (CL_IDL_ADDRESSTYPE_NAME == pHandleObj->address.addressType)
    {
        rc = clNameToObjectReferenceGet(&(pHandleObj->address.address.nameAddress.name),
                                        pHandleObj->address.address.nameAddress.attrCount,
                                        pHandleObj->address.address.nameAddress.attr,
                                        pHandleObj->address.address.nameAddress.contextCookie,
                                        (ClUint64T*)&address);
        if (CL_OK != rc)
        {
            goto L;
        }
    }
    else
    {
        rc = CL_IDL_RC(CL_ERR_INVALID_PARAMETER);
        goto L;
    }

    rc = clBufferCreate(&inMsgHdl);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClNameT(pStreamName, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClNameT(pNodeName, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClUint32T(&(streamScope), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClUint32T(&(sequenceNumber), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClUint32T(&(numRecords), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClHandleT(&(flushCookie), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    if(fpAsyncCallback != NULL)
    {
        

        pCookie = clHeapAllocate(sizeof(ClIdlCookieT));
        if (NULL == pCookie)
        {
            return CL_IDL_RC(CL_ERR_NO_MEMORY);
        }
        
        asyncOptions.pCookie = NULL;
        asyncOptions.fpCallback = NULL;
        
        rc = clBufferCreate(&outMsgHdl);
        if (CL_OK != rc)
        {
            goto L2;
        }

        tempFlags |= pHandleObj->flags |
                     (CL_RMD_CALL_ASYNC | CL_RMD_CALL_NON_PERSISTENT | CL_RMD_CALL_NEED_REPLY);
        
        pCookie->pCookie = cookie;
        pCookie->actualCallback = (void(*)())fpAsyncCallback;
        pCookie->handle = handle;
        asyncOptions.pCookie = pCookie;
        asyncOptions.fpCallback = clLogHandlerSvrAckSendAsyncCallback_4_0_0;

        rc = clRmdWithMsgVer(address, &funcVer, funcNo, inMsgHdl, outMsgHdl, tempFlags, &(pHandleObj->options), &asyncOptions);
        if (CL_OK != rc)
        {
            goto LL;
         }
    }
    else
    {
        tempFlags |= pHandleObj->flags |
                         (CL_RMD_CALL_ASYNC | CL_RMD_CALL_NON_PERSISTENT);
        rc = clRmdWithMsgVer(address, &funcVer, funcNo, inMsgHdl, 0, tempFlags, &(pHandleObj->options),NULL);
        if(CL_OK != rc)
        {
               goto L;
        }
    }
    
    
    clHandleCheckin(gIdlClnt.idlDbHdl,handle);
    return rc;

LL: clBufferDelete(&outMsgHdl);
L2:  clHeapFree(pCookie);
L:
     clHandleCheckin(gIdlClnt.idlDbHdl,handle);
    return rc;
}

