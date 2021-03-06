
/*********************************************************************
* ModuleName  : idl
*********************************************************************/
/*********************************************************************
* Description : This file contains the declartions for client stub
*               routines
*     
* THIS FILE IS AUTO-GENERATED BY OPENCLOVIS IDE. EDIT THIS FILE AT
* YOUR OWN RISK. ANY CHANGE TO THIS FILE WILL BE OVERWRITTEN ON
* RE-GENERATION.
*     
*********************************************************************/
#ifndef _MSG_CLT_CLIENT_CALLS_FROM_SERVER_TO_CLIENT_CLIENT_H_
#define _MSG_CLT_CLIENT_CALLS_FROM_SERVER_TO_CLIENT_CLIENT_H_
#ifdef __cplusplus
extern "C" {
#endif
#include <clXdrApi.h>
#include <clIdlApi.h>
#include "../clientIDDefinitions.h"
#include "xdrSaMsgQueueGroupNotificationBufferT.h"



typedef void (*MsgCltClMsgClientsTrackCallbackAsyncCallbackT_4_0_0) (CL_IN ClIdlHandleT handle, CL_IN ClHandleT  clientHandle, CL_IN SaNameT* pGroupName, CL_IN SaMsgQueueGroupNotificationBufferT_4_0_0* pNotification, CL_IN ClRcT rc, CL_IN void* pCookie);

typedef MsgCltClMsgClientsTrackCallbackAsyncCallbackT_4_0_0 MsgCltClMsgClientsTrackCallbackAsyncCallbackT;

ClRcT clMsgClientsTrackCallbackClientAsync_4_0_0(CL_IN ClIdlHandleT handle, CL_IN ClHandleT  clientHandle, CL_IN SaNameT* pGroupName, CL_IN SaMsgQueueGroupNotificationBufferT_4_0_0* pNotification,CL_IN MsgCltClMsgClientsTrackCallbackAsyncCallbackT_4_0_0 fpAsyncCallback, CL_IN void *cookie);


#ifdef __cplusplus
}
#endif
#endif /*_MSG_CLT_CLIENT_CALLS_FROM_SERVER_TO_CLIENT_CLIENT_H_*/
