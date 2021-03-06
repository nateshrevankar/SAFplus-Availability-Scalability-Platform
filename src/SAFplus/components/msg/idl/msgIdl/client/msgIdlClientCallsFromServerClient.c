
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
#include "msgIdlClientCallsFromServerClient.h"
extern ClIdlClntT gIdlClnt;


ClRcT clMsgQDatabaseUpdateClientSync_4_0_0(CL_IN ClIdlHandleT handle, CL_IN ClMsgSyncActionT_4_0_0 syncupType, CL_IN ClMsgQueueCkptDataT_4_0_0* queueData, CL_IN ClUint16T updateCkpt)
{
    ClRcT rc = CL_OK;
    ClVersionT funcVer = {4, 0, 0};
    ClUint32T funcNo = CL_EO_GET_FULL_FN_NUM(CL_EO_NATIVE_COMPONENT_TABLE_ID, 12);
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

    rc = clXdrMarshallClMsgSyncActionT_4_0_0(&(syncupType), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        return rc;
    }

    rc = clXdrMarshallClMsgQueueCkptDataT_4_0_0(queueData, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        return rc;
    }

    rc = clXdrMarshallClUint16T(&(updateCkpt), inMsgHdl, 0);
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


static void clMsgQDatabaseUpdateAsyncCallback_4_0_0(ClRcT rc, void *pIdlCookie, ClBufferHandleT inMsgHdl, ClBufferHandleT outMsgHdl)
{
    ClIdlCookieT* pCookie = (ClIdlCookieT*)pIdlCookie;
    ClRcT retVal = CL_OK;
    ClMsgSyncActionT_4_0_0  syncupType;
    ClMsgQueueCkptDataT_4_0_0  queueData;
    ClUint16T  updateCkpt;

    memset(&(syncupType), 0, sizeof(ClMsgSyncActionT_4_0_0));
    memset(&(queueData), 0, sizeof(ClMsgQueueCkptDataT_4_0_0));
    memset(&(updateCkpt), 0, sizeof(ClUint16T));


    retVal = clXdrUnmarshallClMsgSyncActionT_4_0_0(inMsgHdl, &(syncupType));
    if (CL_OK != retVal)
    {
        goto L0;
    }

    retVal = clXdrUnmarshallClMsgQueueCkptDataT_4_0_0(inMsgHdl, &(queueData));
    if (CL_OK != retVal)
    {
        goto L1;
    }

    retVal = clXdrUnmarshallClUint16T(inMsgHdl, &(updateCkpt));
    if (CL_OK != retVal)
    {
        goto L2;
    }

    if (rc != CL_OK)
    {
        retVal = rc;
    }

    ((MsgIdlClMsgQDatabaseUpdateAsyncCallbackT_4_0_0)(pCookie->actualCallback))(pCookie->handle, syncupType, &(queueData), updateCkpt, retVal, pCookie->pCookie);
    goto L3;

L3: 
L2: 
L1: 

L0:  clHeapFree(pCookie);
     clBufferDelete(&outMsgHdl);
     return;
}


ClRcT clMsgQDatabaseUpdateClientAsync_4_0_0(CL_IN ClIdlHandleT handle, CL_IN ClMsgSyncActionT_4_0_0  syncupType, CL_IN ClMsgQueueCkptDataT_4_0_0* queueData, CL_IN ClUint16T  updateCkpt,CL_IN MsgIdlClMsgQDatabaseUpdateAsyncCallbackT_4_0_0 fpAsyncCallback, CL_IN void *cookie)
{
    ClRcT rc = CL_OK;
    ClVersionT funcVer = {4, 0, 0};
    ClUint32T funcNo = CL_EO_GET_FULL_FN_NUM(CL_EO_NATIVE_COMPONENT_TABLE_ID, 12);
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

    rc = clXdrMarshallClMsgSyncActionT_4_0_0(&(syncupType), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClMsgQueueCkptDataT_4_0_0(queueData, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClUint16T(&(updateCkpt), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    if(fpAsyncCallback != NULL)
    {
        

        pCookie = (ClIdlCookieT*) clHeapAllocate(sizeof(ClIdlCookieT));
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
        asyncOptions.fpCallback = clMsgQDatabaseUpdateAsyncCallback_4_0_0;

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


ClRcT clMsgGroupDatabaseUpdateClientSync_4_0_0(CL_IN ClIdlHandleT handle, CL_IN ClMsgSyncActionT_4_0_0 syncupType, CL_IN SaNameT* pGroupName, CL_IN SaMsgQueueGroupPolicyT_4_0_0 policy, CL_IN ClIocPhysicalAddressT_4_0_0 qGroupAddress, CL_IN ClUint16T updateCkpt)
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

    rc = clXdrMarshallClMsgSyncActionT_4_0_0(&(syncupType), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        return rc;
    }

    rc = clXdrMarshallSaNameT(pGroupName, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        return rc;
    }

    rc = clXdrMarshallSaMsgQueueGroupPolicyT_4_0_0(&(policy), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        return rc;
    }

    rc = clXdrMarshallClIocPhysicalAddressT_4_0_0(&(qGroupAddress), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        return rc;
    }

    rc = clXdrMarshallClUint16T(&(updateCkpt), inMsgHdl, 0);
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


static void clMsgGroupDatabaseUpdateAsyncCallback_4_0_0(ClRcT rc, void *pIdlCookie, ClBufferHandleT inMsgHdl, ClBufferHandleT outMsgHdl)
{
    ClIdlCookieT* pCookie = (ClIdlCookieT*)pIdlCookie;
    ClRcT retVal = CL_OK;
    ClMsgSyncActionT_4_0_0  syncupType;
    SaNameT  pGroupName;
    SaMsgQueueGroupPolicyT_4_0_0  policy;
    ClIocPhysicalAddressT_4_0_0  qGroupAddress;
    ClUint16T  updateCkpt;

    memset(&(syncupType), 0, sizeof(ClMsgSyncActionT_4_0_0));
    memset(&(pGroupName), 0, sizeof(SaNameT));
    memset(&(policy), 0, sizeof(SaMsgQueueGroupPolicyT_4_0_0));
    memset(&(qGroupAddress), 0, sizeof(ClIocPhysicalAddressT_4_0_0));
    memset(&(updateCkpt), 0, sizeof(ClUint16T));


    retVal = clXdrUnmarshallClMsgSyncActionT_4_0_0(inMsgHdl, &(syncupType));
    if (CL_OK != retVal)
    {
        goto L0;
    }

    retVal = clXdrUnmarshallSaNameT(inMsgHdl, &(pGroupName));
    if (CL_OK != retVal)
    {
        goto L1;
    }

    retVal = clXdrUnmarshallSaMsgQueueGroupPolicyT_4_0_0(inMsgHdl, &(policy));
    if (CL_OK != retVal)
    {
        goto L2;
    }

    retVal = clXdrUnmarshallClIocPhysicalAddressT_4_0_0(inMsgHdl, &(qGroupAddress));
    if (CL_OK != retVal)
    {
        goto L3;
    }

    retVal = clXdrUnmarshallClUint16T(inMsgHdl, &(updateCkpt));
    if (CL_OK != retVal)
    {
        goto L4;
    }

    if (rc != CL_OK)
    {
        retVal = rc;
    }

    ((MsgIdlClMsgGroupDatabaseUpdateAsyncCallbackT_4_0_0)(pCookie->actualCallback))(pCookie->handle, syncupType, &(pGroupName), policy, qGroupAddress, updateCkpt, retVal, pCookie->pCookie);
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


ClRcT clMsgGroupDatabaseUpdateClientAsync_4_0_0(CL_IN ClIdlHandleT handle, CL_IN ClMsgSyncActionT_4_0_0  syncupType, CL_IN SaNameT* pGroupName, CL_IN SaMsgQueueGroupPolicyT_4_0_0  policy, CL_IN ClIocPhysicalAddressT_4_0_0  qGroupAddress, CL_IN ClUint16T  updateCkpt,CL_IN MsgIdlClMsgGroupDatabaseUpdateAsyncCallbackT_4_0_0 fpAsyncCallback, CL_IN void *cookie)
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

    rc = clXdrMarshallClMsgSyncActionT_4_0_0(&(syncupType), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallSaNameT(pGroupName, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallSaMsgQueueGroupPolicyT_4_0_0(&(policy), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClIocPhysicalAddressT_4_0_0(&(qGroupAddress), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClUint16T(&(updateCkpt), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    if(fpAsyncCallback != NULL)
    {
        

        pCookie = (ClIdlCookieT*) clHeapAllocate(sizeof(ClIdlCookieT));
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
        asyncOptions.fpCallback = clMsgGroupDatabaseUpdateAsyncCallback_4_0_0;

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



static void clMsgGroupMembershipUpdateAsyncCallback_4_0_0(ClRcT rc, void *pIdlCookie, ClBufferHandleT inMsgHdl, ClBufferHandleT outMsgHdl)
{
    ClIdlCookieT* pCookie = (ClIdlCookieT*)pIdlCookie;
    ClRcT retVal = CL_OK;
    ClMsgSyncActionT_4_0_0  syncAct;
    SaNameT  pGroupName;
    SaNameT  pQueueName;
    ClUint16T  updateCkpt;

    memset(&(syncAct), 0, sizeof(ClMsgSyncActionT_4_0_0));
    memset(&(pGroupName), 0, sizeof(SaNameT));
    memset(&(pQueueName), 0, sizeof(SaNameT));
    memset(&(updateCkpt), 0, sizeof(ClUint16T));


    retVal = clXdrUnmarshallClMsgSyncActionT_4_0_0(inMsgHdl, &(syncAct));
    if (CL_OK != retVal)
    {
        goto L0;
    }

    retVal = clXdrUnmarshallSaNameT(inMsgHdl, &(pGroupName));
    if (CL_OK != retVal)
    {
        goto L1;
    }

    retVal = clXdrUnmarshallSaNameT(inMsgHdl, &(pQueueName));
    if (CL_OK != retVal)
    {
        goto L2;
    }

    retVal = clXdrUnmarshallClUint16T(inMsgHdl, &(updateCkpt));
    if (CL_OK != retVal)
    {
        goto L3;
    }

    if (rc != CL_OK)
    {
        retVal = rc;
    }

    ((MsgIdlClMsgGroupMembershipUpdateAsyncCallbackT_4_0_0)(pCookie->actualCallback))(pCookie->handle, syncAct, &(pGroupName), &(pQueueName), updateCkpt, retVal, pCookie->pCookie);
    goto L4;

L4: 
L3: 
L2: 
L1: 

L0:  clHeapFree(pCookie);
     clBufferDelete(&outMsgHdl);
     return;
}


ClRcT clMsgGroupMembershipUpdateClientAsync_4_0_0(CL_IN ClIdlHandleT handle, CL_IN ClMsgSyncActionT_4_0_0  syncAct, CL_IN SaNameT* pGroupName, CL_IN SaNameT* pQueueName, CL_IN ClUint16T  updateCkpt,CL_IN MsgIdlClMsgGroupMembershipUpdateAsyncCallbackT_4_0_0 fpAsyncCallback, CL_IN void *cookie)
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

    rc = clXdrMarshallClMsgSyncActionT_4_0_0(&(syncAct), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallSaNameT(pGroupName, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallSaNameT(pQueueName, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    rc = clXdrMarshallClUint16T(&(updateCkpt), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        goto L;
    }

    if(fpAsyncCallback != NULL)
    {
        

        pCookie = (ClIdlCookieT*) clHeapAllocate(sizeof(ClIdlCookieT));
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
        asyncOptions.fpCallback = clMsgGroupMembershipUpdateAsyncCallback_4_0_0;

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


ClRcT clMsgQueueAllocateClientSync_4_0_0(CL_IN ClIdlHandleT handle, CL_IN SaNameT* pQName, CL_IN SaMsgQueueOpenFlagsT_4_0_0 openFlags, CL_IN SaMsgQueueCreationAttributesT_4_0_0* pCreationAttributes, CL_OUT ClHandleT* pQueueHandle)
{
    ClRcT rc = CL_OK;
    ClVersionT funcVer = {4, 0, 0};
    ClUint32T funcNo = CL_EO_GET_FULL_FN_NUM(CL_EO_NATIVE_COMPONENT_TABLE_ID, 15);
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

    rc = clXdrMarshallSaNameT(pQName, inMsgHdl, 0);
    if (CL_OK != rc)
    {
        return rc;
    }

    rc = clXdrMarshallSaMsgQueueOpenFlagsT_4_0_0(&(openFlags), inMsgHdl, 0);
    if (CL_OK != rc)
    {
        return rc;
    }

    rc = clXdrMarshallSaMsgQueueCreationAttributesT_4_0_0(pCreationAttributes, inMsgHdl, 0);
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


    rc = clXdrUnmarshallClHandleT( outMsgHdl, pQueueHandle);
    if (CL_OK != rc)
    {
        return rc;
    }

    clBufferDelete(&outMsgHdl);
    
    rc = clHandleCheckin(gIdlClnt.idlDbHdl,handle);
    return rc;
}

