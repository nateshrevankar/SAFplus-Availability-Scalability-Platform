/*
 * Copyright (C) 2002-2012 OpenClovis Solutions Inc.  All Rights Reserved.
 *
 * This file is available  under  a  commercial  license  from  the
 * copyright  holder or the GNU General Public License Version 2.0.
 * 
 * The source code for  this program is not published  or otherwise 
 * divested of  its trade secrets, irrespective  of  what  has been 
 * deposited with the U.S. Copyright office.
 * 
 * This program is distributed in the  hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied  warranty  of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 * 
 * For more  information, see  the file  COPYING provided with this
 * material.
 */
/*******************************************************************************
 * ModuleName  : event
 * File        : clEvtCkpt.c
 *******************************************************************************/

/*******************************************************************************
 * Description :
 *          This module contains the Check Pointing functionality
 *          that EM employs.
 *****************************************************************************/

#include "clEventCkptIpi.h"
#include <ipi/clHandleIpi.h>
#include <clLogUtilApi.h>
#include "xdrClEvtInitRequestT.h"
#include "xdrClEvtChannelOpenRequestT.h"
#include "xdrClEvtSubscribeEventRequestT.h"
#include "xdrClEvtUnsubscribeEventRequestT.h"
#include <clCkptErrors.h>
#include <clCkptApi.h>
#include <clCkptExtApi.h>
#include <ipi/clCkptIpi.h>
#include <stdint.h>
#include <inttypes.h>
#include <clGmsCommon.h>


#ifdef CKPT_ENABLED

extern ClHandleDatabaseHandleT gEvtHandleDatabaseHdl;
static ClCkptSvcHdlT gClEvtCkptHandle;

static SaNameT gClEvtCkptName = {
    sizeof(CL_EVT_CKPT_NAME) - 1,
    CL_EVT_CKPT_NAME
};


static ClBufferHandleT gEvtCkptUserInfoMsg;
static ClBufferHandleT gEvtCkptECHMsg[2];    /* For Local/Global */

/*** Check Pointing API related counters */
static ClUint16T gEvtCkptFinalizeCount;
static ClUint16T gEvtCkptChannelCloseCount[2];  /* For Local/Global */
static ClUint16T gEvtCkptUnsubscribeCount[2];   /* For Local/Global */


/*
 ** Extern Declarations
 */

extern ClRcT clEvtInitializeViaRequest(ClEvtInitRequestT *pEvtInitReq, ClUint32T type);
extern ClRcT clEvtChannelOpenViaRequest(ClEvtChannelOpenRequestT *pEvtChannelOpenRequest, ClUint32T type);
extern ClRcT clEvtEventSubscribeViaRequest(ClEvtSubscribeEventRequestT *pEvtSubsReq, ClUint32T type);
extern ClRcT clEvtEventCleanupViaRequest(ClEvtUnsubscribeEventRequestT *pEvtUnsubsReq, ClUint32T type);

/*
 ** Local Functions
 */
static ClRcT clEvtCkptInitialize(ClBoolT * pIsRecovery);
static ClRcT clEvtBufferMessageInit(void);
static ClRcT clEvtCkptFinalize(void);
static ClRcT clEvtCkptMessageBufferRelease(void);
static ClRcT clEvtCkptUserInfoWalk(ClHandleDatabaseHandleT databaseHandle,
                                   ClHandleT handle,
                                   void * pCookie);
static ClRcT clEvtCkptUserInfoSecPackComplete(ClUint32T dataSetId,
                                              ClAddrT *ppData,
                                              ClUint32T *pDataLen,
                                              ClPtrT pCookie);
static ClRcT clEvtCkptUserInfoSecPackInc(ClUint32T dataSetId, ClAddrT *ppData,
                                         ClUint32T *pDataLen,
                                         ClPtrT pCookie);
static ClRcT clEvtCkptUserInfoSerializer(ClUint32T dataSetId, ClAddrT *ppData,
                                         ClUint32T *pDataLen,
                                         ClPtrT pCookie);
static ClRcT clEvtCkptUserInfoWrite(ClEvtCkptUserInfoWithMutexT
                                    *pEvtUserInfoWithMutex);
static ClRcT clEvtCkptUserInfoDeserializer(ClUint32T dataSetId, ClAddrT pData,
                                           ClUint32T dataLen,
                                           ClPtrT pCookie);
static ClRcT clEvtCkptInitializeSimulator(ClEvtCkptUserInfoWithLenT
                                          *pUserInfoWithLen);
static ClRcT clEvtCkptFinalizeSimulator(ClEvtCkptUserInfoWithLenT
                                        *pUserInfoWithLen);
static ClRcT clEvtCkptUserofECHInfoWalk(ClCntKeyHandleT userKey,
                                        ClCntDataHandleT userData,
                                        ClCntArgHandleT userArg,
                                        ClUint32T dataLength);
static ClRcT clEvtCkptECHWalk(ClCntKeyHandleT userKey,
                              ClCntDataHandleT userData,
                              ClCntArgHandleT userArg, ClUint32T dataLength);
static ClRcT clEvtCkptECHSecPackComplete(ClUint32T dataSetId, ClAddrT *ppData,
                                         ClUint32T *pDataLen,
                                         ClPtrT pCookie);
static ClRcT clEvtCkptECHSecPackInc(ClUint32T dataSetId, ClAddrT *ppData,
                                    ClUint32T *pDataLen, ClPtrT pCookie);
static ClRcT clEvtCkptECHSerializer(ClUint32T dataSetId, ClAddrT *ppData,
                                    ClUint32T *pDataLen, ClPtrT pCookie);
static ClRcT clEvtCkptECHInfoWrite(ClEvtCkptECHInfoWithMutexT
                                   *pEvtECHInfoWithMutex);
static ClRcT clEvtCkptECHDeserializer(ClUint32T dataSetId, ClAddrT pData,
                                      ClUint32T dataLen, ClPtrT pCookie);
static ClRcT clEvtCkptChannelOpenSimulator(ClEvtCkptECHInfoWithLenT
                                           *pECHInfoWithLen);
static ClRcT clEvtCkptChannelCloseSimulator(ClEvtCkptECHInfoWithLenT
                                            *pECHInfoWithLen);
static ClRcT clEvtCkptSubsInfoWalk(ClCntKeyHandleT userKey,
                                   ClCntDataHandleT userData,
                                   ClCntArgHandleT userArg,
                                   ClUint32T dataLength);
static ClRcT clEvtCkptEventInfoWalk(ClCntKeyHandleT userKey,
                                    ClCntDataHandleT userData,
                                    ClCntArgHandleT userArg,
                                    ClUint32T dataLength);
static ClRcT clEvtCkptSubsInfoSecPackComplete(ClUint32T dataSetId,
                                              ClAddrT *ppData,
                                              ClUint32T *pDataLen,
                                              ClPtrT pCookie);
static ClRcT clEvtCkptSubsInfoSecPackInc(ClUint32T dataSetId, ClAddrT *ppData,
                                         ClUint32T *pDataLen,
                                         ClPtrT pCookie);
static ClRcT clEvtCkptSubsSerializer(ClUint32T dataSetId, ClAddrT *ppData,
                                     ClUint32T *pDataLen, ClPtrT pCookie);
static ClRcT clEvtCkptSubsInfoWrite(ClEvtCkptSubsInfoWithMutexT
                                    *pEvtSubsInfoWithMutex);
static ClRcT clEvtCkptSubsDeserializer(ClUint32T dataSetId, ClAddrT pData,
                                       ClUint32T dataLen, ClPtrT pCookie);
static ClRcT clEvtCkptSubsUnsubsSimulator(ClEvtCkptSubsInfoWithLenT
                                          *pSubsInfoWithLen);

static ClRcT clEvtCkptSubsInfoCheckPointWalk(ClCntKeyHandleT userKey,
                                             ClCntDataHandleT userData,
                                             ClCntArgHandleT userArg,
                                             ClUint32T dataLength);
static ClRcT clEvtCkptSubsInfoReconstructWalk(ClCntKeyHandleT userKey,
                                              ClCntDataHandleT userData,
                                              ClCntArgHandleT userArg,
                                              ClUint32T dataLength);
static ClRcT clEvtCkptSubsInfoReconstruct(ClEvtCkptSubsInfoWithLenT
                                          *pSubsInfoWithLen);


static ClRcT clEventUserInfoCheckpointCreate(SaNameT         *pCkptName);
static ClRcT clEventChannelOpenCheckpointCreate(SaNameT         *pCkptName);
static ClRcT clEventChannelSubCheckpointCreate(SaNameT         *pCkptName);
static 
void clEventTrackCallback(ClGmsClusterNotificationBufferT *notificationBuffer,
                        ClUint32T                       numberOfMembers,
                        ClRcT                           rc);

#define EVENT_LOG_AREA_CKPT	"CKP"
#define EVENT_LOG_AREA_EVENT	"EVT"
#define EVENT_LOG_AREA_MSG	"MSG"
#define EVENT_LOG_AREA_SEC	"SEC"
#define EVENT_LOG_AREA_USER	"USR"
#define EVENT_LOG_CTX_INI	"INI"
#define EVENT_LOG_GLOBAL	"GLOBAL"
#define EVENT_LOG_CTX_READ	"READ"
#define EVENT_LOG_CTX_OPEN	"OPE"
#define EVENT_LOG_CTX_DELETE	"DEL"
#define EVENT_LOG_CTX_FINALISE	"FIN"
#define EVENT_LOG_CTX_PACK	"PACK"
#define EVENT_LOG_CTX_INFO	"INFO"
#define EVENT_LOG_CTX_RECONSTRUCT "CONSTRUCT"
#define EVENT_LOG_CTX_WALK	"WALK"
#define EVENT_LOG_CTX_WRITE	"WRITE"
#define EVENT_LOG_CTX_SUBSCRIBE	"SUB"


#define CL_EVENT_CLEANUP(fn, expect)                          \
    do                                                      \
    {                                                       \
        ClRcT ret = (fn);                                   \
        if( ret != (expect) )                               \
        {                                                   \
        	 clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_INI," rc[0x %x]", ret); \
        }                                                   \
    }while( 0 )

/*
 ** Initialize the CKPT Library and Attempt recovery -
 **
 ** The Check Points and Datasets are created here alongwith the
 ** message buffers to hold the data to be check pointed temporarily.
 ** An attempt to do a recovery is done - if the check pointed information
 ** exists then the reconstruction is done else we return successfully to
 ** continue a normal startup.
 */

ClRcT clEvtCkptInit(void)
{
    ClRcT rc = CL_OK;

    ClBoolT isRecovery = CL_FALSE;

    CL_FUNC_ENTER();

    rc = clEvtCkptInitialize(&isRecovery);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_INI,
                   "\n CKPT Library Initialize Failed [%x]\n", rc);
        CL_FUNC_EXIT();
        return rc;
    }

    /*
     ** If this is a Recovery an attempt to reconstruct the state at the
     ** the time of failure.
     */
    if (CL_TRUE == isRecovery)
    {
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_INFO, NULL,
                   CL_EVENT_LOG_MSG_0_ATTEMPTING_RECOVERY);
        rc = clEvtCkptReconstruct();
        if (rc != CL_OK)
        {
            clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_INI,
                       "Reconstruction failed [0x%x], proceeding with normal recovery", rc);
            clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_ERROR, NULL,
                       CL_EVENT_LOG_MSG_1_RECOVERY_FAILED, rc);
            /* Andrew Stone -- if the recovery fails, why give up and take this blade out of service?
               What is the customer going to do to make it not fail?
               Instead do normal initialization.
#if 0            
            clCkptLibraryFinalize(gClEvtCkptHandle);
            CL_FUNC_EXIT();
            return rc;
#endif
            */
            isRecovery = CL_FALSE; /* continue without recovery */
        }
#if 0
        else clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_INFO, NULL,
                   CL_EVENT_LOG_MSG_0_RECOVERY_SUCCESSFUL);
#endif
    }

    rc = clEvtBufferMessageInit();
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_MSG,EVENT_LOG_CTX_INI,
                   "\nBuffer Allocation Failed [%x]\n", rc);
        clEvtCkptFinalize();
        CL_FUNC_EXIT();
        return rc;
    }

    if (CL_TRUE == isRecovery)
    {
        rc = clEvtCkptCheckPointAll();
        if (rc != CL_OK)
        {
            clLogCritical(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_INI,
                         "Complete Checkpointing after Reconstruction failed [%#x]\n\r", rc);
            clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_ERROR, NULL,
                       CL_EVENT_LOG_MSG_1_RECOVERY_FAILED, rc);
            clCkptLibraryFinalize(gClEvtCkptHandle);
            CL_FUNC_EXIT();
            return rc;
        }
        
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_INFO, NULL,
                   CL_EVENT_LOG_MSG_0_RECOVERY_SUCCESSFUL);
    }

    CL_FUNC_EXIT();
    return CL_OK;
}


/*
 ** The Subscription check points are generated dynamically
 ** each time a channel is opened. The check point name is the
 ** same as the channel name obviating the need to preserve the
 ** channel name. Two data sets one for local and other for global
 ** are created depending on the scope. The CKPT library doesn't
 ** crib if we attempt to create the check point again.
 */

ClRcT clEvtCkptSubsDSCreate(SaNameT *pChannelName, ClUint32T channelScope)
{
    ClRcT rc = CL_OK;

    CL_FUNC_ENTER();

    clLogDebug("CKPT", "READ", "Creating subscriber dataset for channel [%s], scope [%d]",
               pChannelName->value, channelScope);
    /*
     ** Create a single check point per channel name and have 2 sections
     ** for each scope with scope as the section Id. Note if the user 
     ** tries to create the channel with same name in global and local
     ** scope the Check Point is created only once. This is the way CKPT
     ** API - clCkptLibraryCkptCreate() behaves. If this behavior changes
     ** one needs to invoke clCkptLibraryDoesCkptExist() & check before
     ** invoking the create call.
     */
    rc = clCkptLibraryCkptCreate(gClEvtCkptHandle, pChannelName);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_READ,
                   "\n CKPT Check Point Create Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_CKPT_CREATION_FAILED, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    /*
     ** Create the data set depending on the scope.
     */
    rc = clCkptLibraryCkptDataSetCreate(gClEvtCkptHandle, pChannelName,
                                        CL_EVT_CKPT_SUBS_DSID(channelScope),
                                        CL_EVT_CKPT_GROUP_ID, CL_EVT_CKPT_ORDER,
                                        clEvtCkptSubsSerializer,
                                        clEvtCkptSubsDeserializer);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_READ,
                   "\n CKPT Data Set Create Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_CKPT_DATASET_CREATION_FAILED, rc);
        rc = clCkptLibraryCkptDataSetDelete(gClEvtCkptHandle, pChannelName,
                                            CL_EVT_CKPT_SUBS_DSID
                                            (channelScope));
        CL_FUNC_EXIT();
        return rc;
    }


    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** This call counters the dynamic creation of check points for subscription
 ** info. It is invoked each time a channel is deleted. The check point is
 ** deleted only when there is no data set under it.
 */
ClRcT clEvtCkptSubsDSDelete(SaNameT *pChannelName, ClUint32T channelScope)
{
    ClRcT rc = CL_OK;

    ClBoolT isExist = CL_FALSE;

    CL_FUNC_ENTER();

    rc = clCkptLibraryCkptDataSetDelete(gClEvtCkptHandle, pChannelName,
                                        CL_EVT_CKPT_SUBS_DSID(channelScope));
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_DELETE,
                   "\n CKPT Dataset Delete Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    /*
     ** Toggle the scope to test if there is a channel opened with
     ** the same name in the other scope. This happens when user 
     ** creates a global and local channel with the same name. The
     ** check point not deleted if there still exists a data set in
     ** it.
     */
    channelScope = CL_EVT_CKPT_TOGGLE_SCOPE(channelScope);

    rc = clCkptLibraryDoesDatasetExist(gClEvtCkptHandle, pChannelName,
                                       CL_EVT_CKPT_SUBS_DSID(channelScope),
                                       &isExist);
    if (CL_FALSE == isExist)
    {
        /*
         ** This error implies that the Data Set doesn't exist on
         ** the other scope and we can go ahead and delete the 
         ** checkpoint.
         */
        rc = clCkptLibraryCkptDelete(gClEvtCkptHandle, pChannelName);
        if (CL_OK != rc)
        {
            clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_DELETE,
                       "\n CKPT Check Point [%*s] Delete Failed [%x]\n",
                       pChannelName->length, pChannelName->value, rc);
            clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                       CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
            CL_FUNC_EXIT();
            return rc;
        }

        CL_FUNC_EXIT();
        return CL_OK;
    }

    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** The CKPT library is initialized here and the check points for User Info and
 ** ECH Info are created.
 */

ClRcT clEvtCkptInitialize(ClBoolT * pIsRecovery)
{
    ClRcT rc = CL_OK;

    CL_FUNC_ENTER();

    /*
     * Initialize the Check Point Service Library 
     */
    rc = clCkptLibraryInitialize(&gClEvtCkptHandle);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_INI,
                   "\n CKPT Library Initialize Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_CRITICAL, NULL,
                   CL_LOG_MESSAGE_2_LIBRARY_INIT_FAILED, "Checkpoint Library",
                   rc);
        CL_FUNC_EXIT();
        return rc;
    }

    /*
     ** Test if this is Normal startup or a Recovery by checking for the
     ** existence of the default checkpoint. If the check point already
     ** exists it implies this is a recovery else it's normal startup.
     */
    rc = clCkptLibraryDoesCkptExist(gClEvtCkptHandle, &gClEvtCkptName,
                                    pIsRecovery);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_INI,
                   "\nChecking if Recovery Failed."
                   "Assuming Normal Startup... [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        *pIsRecovery = CL_FALSE;    /* Marking Normal Recovery */
    }

    /*
     * Create the EM Check Point 
     */
    rc = clCkptLibraryCkptCreate(gClEvtCkptHandle, &gClEvtCkptName);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_INI,
                   "\n CKPT Check Point Create Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_CKPT_CREATION_FAILED, rc);
        clCkptLibraryFinalize(gClEvtCkptHandle);
        CL_FUNC_EXIT();
        return rc;
    }

    rc = clCkptLibraryCkptDataSetCreate(gClEvtCkptHandle, &gClEvtCkptName,
                                        CL_EVT_CKPT_USER_DSID,
                                        CL_EVT_CKPT_GROUP_ID, CL_EVT_CKPT_ORDER,
                                        clEvtCkptUserInfoSerializer,
                                        clEvtCkptUserInfoDeserializer);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_INI,
                   "\n CKPT Data Set Create Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_CKPT_DATASET_CREATION_FAILED, rc);

        clCkptLibraryCkptDelete(gClEvtCkptHandle, &gClEvtCkptName);
        clCkptLibraryFinalize(gClEvtCkptHandle);
        CL_FUNC_EXIT();
        return rc;
    }

    rc = clCkptLibraryCkptDataSetCreate(gClEvtCkptHandle, &gClEvtCkptName,
                                        CL_EVT_CKPT_ECH_DSID
                                        (CL_EVENT_LOCAL_CHANNEL),
                                        CL_EVT_CKPT_GROUP_ID, CL_EVT_CKPT_ORDER,
                                        clEvtCkptECHSerializer,
                                        clEvtCkptECHDeserializer);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_INI,
                   "\n CKPT Data Set Create Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_CKPT_DATASET_CREATION_FAILED, rc);

        clCkptLibraryCkptDataSetDelete(gClEvtCkptHandle, &gClEvtCkptName,
                                       CL_EVT_CKPT_USER_DSID);
        clCkptLibraryCkptDelete(gClEvtCkptHandle, &gClEvtCkptName);
        clCkptLibraryFinalize(gClEvtCkptHandle);
        CL_FUNC_EXIT();
        return rc;
    }

    rc = clCkptLibraryCkptDataSetCreate(gClEvtCkptHandle, &gClEvtCkptName,
                                        CL_EVT_CKPT_ECH_DSID
                                        (CL_EVENT_GLOBAL_CHANNEL),
                                        CL_EVT_CKPT_GROUP_ID, CL_EVT_CKPT_ORDER,
                                        clEvtCkptECHSerializer,
                                        clEvtCkptECHDeserializer);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_INI,
                   "\n CKPT Data Set Create Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_CKPT_DATASET_CREATION_FAILED, rc);

        clCkptLibraryCkptDataSetDelete(gClEvtCkptHandle, &gClEvtCkptName,
                                       CL_EVT_CKPT_USER_DSID);
        clCkptLibraryCkptDataSetDelete(gClEvtCkptHandle, &gClEvtCkptName,
                                       CL_EVT_CKPT_ECH_DSID
                                       (CL_EVENT_LOCAL_CHANNEL));
        clCkptLibraryCkptDelete(gClEvtCkptHandle, &gClEvtCkptName);
        clCkptLibraryFinalize(gClEvtCkptHandle);
        CL_FUNC_EXIT();
        return rc;
    }

    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** Create the Message Buffer for User and ECH Information.
 */

ClRcT clEvtBufferMessageInit(void)
{
    ClRcT rc = CL_OK;

    CL_FUNC_ENTER();

    /*
     * Creating buffers for incremental/complete check pointing 
     */

    rc = clBufferCreate(&gEvtCkptUserInfoMsg);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_MSG,EVENT_LOG_CTX_INI,
                   "\n Buffer message create Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_BUFFER_MSG_CREATION_FAILED, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    rc = clBufferCreate(&gEvtCkptECHMsg[CL_EVENT_LOCAL_CHANNEL]);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_MSG,EVENT_LOG_CTX_INI,
                   "\n Buffer message create Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_BUFFER_MSG_CREATION_FAILED, rc);
        clBufferDelete(&gEvtCkptUserInfoMsg);
        CL_FUNC_EXIT();
        return rc;
    }

    rc = clBufferCreate(&gEvtCkptECHMsg[CL_EVENT_GLOBAL_CHANNEL]);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_MSG,EVENT_LOG_CTX_INI,
                   "\n Buffer message create Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_BUFFER_MSG_CREATION_FAILED, rc);
        clBufferDelete(&gEvtCkptECHMsg[CL_EVENT_LOCAL_CHANNEL]);
        clBufferDelete(&gEvtCkptUserInfoMsg);
        CL_FUNC_EXIT();
        return rc;
    }

    CL_FUNC_EXIT();
    return CL_OK;
}


/*
 ** Yields all the CKPT related resources for User & ECH Info.
 */
ClRcT clEvtCkptExit(void)
{
    /*
     * Disabling deleting the event checkpoint. so it could be reused on 
     * event restart (graceful/abnormal)
     */
    return CL_OK;

    ClRcT rc = CL_OK;

    CL_FUNC_ENTER();

    rc = clEvtCkptFinalize();
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_DELETE,
                   "\n Checkpoint Delete Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        /*
         ** If Error just log and continue with rest of the cleanup.
         */
    }

    rc = clEvtCkptMessageBufferRelease();
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_DELETE,
                   "\n CKPT Library Finalize Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    CL_FUNC_EXIT();
    return CL_OK;
}


/*
 ** Finalizes the CKPT Library.
 */
ClRcT clEvtCkptFinalize(void)
{
    ClRcT rc = CL_OK;

    CL_FUNC_ENTER();

    /*
     ** Delete the Data Sets for each Section Id. 
     **
     ** If not successful continue the cleanup instead of simply
     ** returnining. Just log a message at debug level.
     */

    rc = clCkptLibraryCkptDataSetDelete(gClEvtCkptHandle, &gClEvtCkptName,
                                        CL_EVT_CKPT_USER_DSID);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_FINALISE,
                   "\n Checkpoint Delete Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
    }


    rc = clCkptLibraryCkptDataSetDelete(gClEvtCkptHandle, &gClEvtCkptName,
                                        CL_EVT_CKPT_ECH_DSID
                                        (CL_EVENT_LOCAL_CHANNEL));
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_FINALISE,
                   "\n Checkpoint Delete Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
    }

    rc = clCkptLibraryCkptDataSetDelete(gClEvtCkptHandle, &gClEvtCkptName,
                                        CL_EVT_CKPT_ECH_DSID
                                        (CL_EVENT_GLOBAL_CHANNEL));
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_FINALISE,
                   "\n Checkpoint Delete Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
    }


    /*
     * Delete the EM Check Point 
     */
    rc = clCkptLibraryCkptDelete(gClEvtCkptHandle, &gClEvtCkptName);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_FINALISE,
                   "\n CKPT Checkpoint Delete Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
    }

    /*
     * Finalize the Check Point Service Library 
     */
    rc = clCkptLibraryFinalize(gClEvtCkptHandle);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_FINALISE,
                   "\n CKPT Library Finalize Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
    }

    CL_FUNC_EXIT();
    return CL_OK;
}


/*
 ** Free all the buffers allocated for Check Pointing User & ECH Info. 
 */
ClRcT clEvtCkptMessageBufferRelease(void)
{
    ClRcT rc = CL_OK;

    CL_FUNC_ENTER();

    /*
     ** Deleting buffers for incremental/complete check pointing.
     **
     ** If not successful continue the cleanup instead of simply
     ** returnining. Just log a message at debug level.
     */

    rc = clBufferDelete(&gEvtCkptUserInfoMsg);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_MSG,EVENT_LOG_CTX_DELETE,"\n Msg delete Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_BUFFER_MSG_DELETE_FAILED, rc);
    }

    rc = clBufferDelete(&gEvtCkptECHMsg[CL_EVENT_LOCAL_CHANNEL]);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_MSG,EVENT_LOG_CTX_DELETE,"\n Msg delete Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_BUFFER_MSG_DELETE_FAILED, rc);
    }

    rc = clBufferDelete(&gEvtCkptECHMsg[CL_EVENT_GLOBAL_CHANNEL]);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_MSG,EVENT_LOG_CTX_DELETE,"\n Msg delete Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_BUFFER_MSG_DELETE_FAILED, rc);
    }


    CL_FUNC_EXIT();
    return CL_OK;
}



/*
 ** Fucntions For the Check Pointing User Info -
 */

/*
 ** The walk to check point User Info completely.
 */
ClRcT clEvtCkptUserInfoWalk(ClHandleDatabaseHandleT databaseHandle, ClHandleT handle, void * pCookie)
{
    ClRcT rc = CL_OK;

    ClEvtCkptUserInfoT *pEvtCkptUserInfo = (ClEvtCkptUserInfoT *)pCookie;
    ClEvtUserIdT *pInitKey = NULL;

    CL_FUNC_ENTER();
    rc = clHandleCheckout(databaseHandle, handle, (void **)&pInitKey);
    if (0 == pInitKey->evtHandle) // NTC
    {
        rc = clHandleCheckin(databaseHandle, handle);           
        return CL_OK;
    }

    pEvtCkptUserInfo->userId = *pInitKey;

    rc = clBufferNBytesWrite(gEvtCkptUserInfoMsg,
                                    (ClUint8T *) pEvtCkptUserInfo,
                                    sizeof(ClEvtCkptUserInfoT));
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_MSG,CL_LOG_CONTEXT_UNSPECIFIED,
                   "\nBuffer Message Write Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_BUFFER_MSG_WRITE_FAILED, rc);
        rc = clHandleCheckin(databaseHandle, handle);           
        CL_FUNC_EXIT();
        return rc;
    }
    rc = clHandleCheckin(databaseHandle, handle);           
    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** Complete check pointing for User Information done using the message buffer
 ** to hold the data to be check pointed temporarily. This is read from start
 ** and copied into a local buffer which is then passed to the CKPT library.
 */
ClRcT clEvtCkptUserInfoSecPackComplete(ClUint32T dataSetID, ClAddrT *ppData,
                                       ClUint32T *pDataLen, ClPtrT pCookie)
{
    ClRcT rc = CL_OK;

    ClEvtCkptUserInfoWithMutexT *pUserInfoWithMutex =
        (ClEvtCkptUserInfoWithMutexT *) pCookie;

    ClEvtCkptUserInfoT evtCkptUserInfo = { 0 };


    CL_FUNC_ENTER();

    evtCkptUserInfo.operation = CL_EVT_CKPT_OP_INITIALIZE;

    /*
     ** Since it's a complete checkpoint we need to discard all the stale
     ** checkpointing info and begin anew.
     */
    rc = clBufferClear(gEvtCkptUserInfoMsg);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,CL_LOG_CONTEXT_UNSPECIFIED,
                   "\nSet Buffer Message Write Offset Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    rc = clHandleWalk(pUserInfoWithMutex->databaseHandle, clEvtCkptUserInfoWalk,
                   (void *) &evtCkptUserInfo);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,CL_LOG_CONTEXT_UNSPECIFIED,
                   "\nclEvtCkptUserInfoWalk Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    /*
     ** Create the buffer to be passed to CKPT since flatten would make the buf
     ** unusable.
     */
    rc = clBufferLengthGet(gEvtCkptUserInfoMsg, pDataLen);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_MSG,CL_LOG_CONTEXT_UNSPECIFIED,
                    "\nGet Buffer Message Length Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    *ppData = (ClAddrT) clHeapAllocate(*pDataLen);
    if (NULL == *ppData)
    {
        clLogError(EVENT_LOG_AREA_CKPT,CL_LOG_CONTEXT_UNSPECIFIED,"\nMemory Allocation Failed\n\r\n");
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_0_MEMORY_ALLOCATION_FAILED);
        CL_FUNC_EXIT();
        return CL_EVENT_ERR_NO_MEM;
    }

    rc = clBufferNBytesRead(gEvtCkptUserInfoMsg, (ClUint8T *) *ppData,
                                   pDataLen);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,CL_LOG_CONTEXT_UNSPECIFIED,"\nMessage Read Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_BUFFER_MSG_READ_FAILED, rc);
        clHeapFree(*ppData);
        CL_FUNC_EXIT();
        return rc;
    }

# ifdef FLAT_FILE
    {
        FILE *gEvtFp = fopen("userInfo.dat", "w+");

        if (NULL == gEvtFp)
        {
            clLogError(EVENT_LOG_AREA_CKPT,CL_LOG_CONTEXT_UNSPECIFIED,"Failed to open the Flat file\n");
            clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                       CL_LOG_MESSAGE_0_MEMORY_ALLOCATION_FAILED);
        }

        fwrite(*ppData, 1, *pDataLen, gEvtFp);
        fclose(gEvtFp);
    }
# endif

    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** Incremental check pointing for User Info done using message buffer. Unlike
 ** complete check point the data is written from current write offset. When 
 ** reading the message buffer it is read from beginning and passed to CKPT 
 ** library. This shall change if the library supports incremental check point.
 */

ClRcT clEvtCkptUserInfoSecPackInc(ClUint32T dataSetId, ClAddrT *ppData,
                                  ClUint32T *pDataLen, ClPtrT pCookie)
{
    ClRcT rc = CL_OK;

    ClEvtCkptUserInfoWithMutexT *pUserInfoWithMutex =
        (ClEvtCkptUserInfoWithMutexT *) pCookie;

    ClEvtCkptUserInfoT evtCkptUserInfo = { 0 };

    ClEvtInitRequestT *pInitReq = pUserInfoWithMutex->pInitReq;


    CL_FUNC_ENTER();

    evtCkptUserInfo.operation = pUserInfoWithMutex->operation;
    evtCkptUserInfo.userId = pInitReq->userId;

    /*
     ** For Incremental Check Pointing we needn't update the write pointer as
     ** we wish to append the data. All else is as in Complete Check Pointing.
     */
    rc = clBufferNBytesWrite(gEvtCkptUserInfoMsg,
                                    (ClUint8T *) &evtCkptUserInfo,
                                    sizeof(ClEvtCkptUserInfoT));
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,
                   "\nBuffer Message Write Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_BUFFER_MSG_WRITE_FAILED, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    /*
     ** Create the buffer to be passed to CKPT since flatten would make the buf
     ** unusable. The size of the same can be obtained as below.
     */
    rc = clBufferLengthGet(gEvtCkptUserInfoMsg, pDataLen);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,
                   "\nGet Buffer Message Length Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    *ppData = (ClAddrT) clHeapAllocate(*pDataLen);
    if (NULL == *ppData)
    {
        clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,"\nMemory Allocation Failed\n\r\n");
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_0_MEMORY_ALLOCATION_FAILED);
        CL_FUNC_EXIT();
        return CL_EVENT_ERR_NO_MEM;
    }

    /*
     ** Update the read offset before any read operation as you know where you
     ** wish to read from.
     */
    rc = clBufferReadOffsetSet(gEvtCkptUserInfoMsg, 0,
                                      CL_BUFFER_SEEK_SET);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,
                   "\nSet Message Buffer Read Offset Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        clHeapFree(*ppData);
        CL_FUNC_EXIT();
        return rc;
    }

    rc = clBufferNBytesRead(gEvtCkptUserInfoMsg, (ClUint8T *) *ppData,
                                   pDataLen);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,"\nMessage Read Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_BUFFER_MSG_READ_FAILED, rc);
        clHeapFree(*ppData);
        CL_FUNC_EXIT();
        return rc;
    }

# ifdef FLAT_FILE
    {
        FILE *gEvtFp = fopen("userInfo.dat", "a+");

        if (NULL == gEvtFp)
        {
            clLogTrace(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,"Failed to open the Flat file\n");
            clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                       CL_LOG_MESSAGE_0_MEMORY_ALLOCATION_FAILED);
        }

        fwrite(*ppData, 1, *pDataLen, gEvtFp);
        fclose(gEvtFp);
    }
# endif

    CL_FUNC_EXIT();
    return rc;
}

/*
 ** The serializer for User Info. It invokes the complete check pointing or
 ** incremental check pointing depending on the request.
 */

ClRcT clEvtCkptUserInfoSerializer(ClUint32T dataSetId, ClAddrT *ppData,
                                  ClUint32T *pDataLen, ClPtrT pCookie)
{
    ClRcT rc = CL_OK;

    CL_FUNC_ENTER();

    if (CL_EVT_CKPT_COMPLETE == ((ClEvtCkptUserInfoWithMutexT *) pCookie)->type)
    {
        rc = clEvtCkptUserInfoSecPackComplete(dataSetId, ppData, pDataLen,
                                              pCookie);
        if (CL_OK != rc)
        {
            clLogError(EVENT_LOG_AREA_USER,EVENT_LOG_CTX_INFO,
                       "\nEvent CKPT UserInfo Complete Pack Failed [%x]\n",
                       rc);
            CL_FUNC_EXIT();
            return rc;
        }
    }
    else
    {
        rc = clEvtCkptUserInfoSecPackInc(dataSetId, ppData, pDataLen, pCookie);
        if (CL_OK != rc)
        {
            clLogError(EVENT_LOG_AREA_USER,EVENT_LOG_CTX_INFO,
                       "\nEvent CKPT UserInfo Incremental Pack Failed [%x]\n",
                       rc);
            CL_FUNC_EXIT();
            return rc;
        }
    }

    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** The wrapper to CKPT write function.
 */
ClRcT clEvtCkptUserInfoWrite(ClEvtCkptUserInfoWithMutexT *pEvtUserInfoWithMutex)
{
    ClRcT rc = CL_OK;

    CL_FUNC_ENTER();

    rc = clCkptLibraryCkptDataSetWrite(gClEvtCkptHandle, &gClEvtCkptName,
                                       CL_EVT_CKPT_USER_DSID,
                                        pEvtUserInfoWithMutex);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_USER,EVENT_LOG_CTX_INFO,
                     "\nEvent CKPT DataSet Write Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** The deserializer simply reads the data from the CKPT library and passes it
 ** back to the caller.
 */
ClRcT clEvtCkptUserInfoDeserializer(ClUint32T dataSetId, ClAddrT pData,
                                    ClUint32T dataLen, ClPtrT pCookie)
{
    ClEvtCkptUserInfoWithLenT *pUserInfoWithLen =
        (ClEvtCkptUserInfoWithLenT *) pCookie;

    CL_FUNC_ENTER();

    pUserInfoWithLen->userInfoLen = dataLen;
    pUserInfoWithLen->pUserInfo = (ClEvtCkptUserInfoT *) pData;

# ifdef FLAT_FILE
    {
        char *pInfo = clHeapAllocate(dataLen);

        if (NULL == pInfo)
        {
            clLogError(EVENT_LOG_AREA_USER,EVENT_LOG_CTX_INFO,
                       "\nMemory Allocation Failed\n\r\n");
            clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                       CL_LOG_MESSAGE_0_MEMORY_ALLOCATION_FAILED);
            CL_FUNC_EXIT();
            return CL_EVENT_ERR_NO_MEM;
        }

        FILE *gEvtFp = fopen("userInfo.dat", "r+");

        if (NULL == gEvtFp)
        {
            clLogtrace(EVENT_LOG_AREA_USER,EVENT_LOG_CTX_INFO,"Failed to open the Flat file\n");
            clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                       CL_LOG_MESSAGE_0_MEMORY_ALLOCATION_FAILED);
        }

        fread(pInfo, 1, dataLen, gEvtFp);

        fclose(gEvtFp);
        // NTC clHeapFree(pInfo);

        pUserInfoWithLen->pUserInfo = (ClEvtCkptUserInfoT *) pInfo;
    }
# endif

    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** Simulates the initialize API at the time of recovery. It is invoked
 ** during the first pass of the the reconstruction.
 */
ClRcT clEvtCkptInitializeSimulator(ClEvtCkptUserInfoWithLenT *pUserInfoWithLen)
{
    ClRcT rc = CL_OK;

    ClEvtCkptUserInfoT *pUserInfo = pUserInfoWithLen->pUserInfo;
    ClUint32T bytesReceived = pUserInfoWithLen->userInfoLen;
    ClUint32T bytesRead = 0;

    ClEvtInitRequestT evtInitRequest = { 0 };

    CL_FUNC_ENTER();

    for (bytesRead = 0; bytesRead < bytesReceived;
         bytesRead += sizeof(ClEvtCkptUserInfoT), pUserInfo++)
    {
        if (CL_EVT_CKPT_OP_INITIALIZE == pUserInfo->operation)
        {
            evtInitRequest.userId = pUserInfo->userId;

            rc = clEvtInitializeViaRequest(&evtInitRequest,
                                           CL_EVT_CKPT_REQUEST);
            if (CL_OK != rc)
            {
                clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_INI,
                           "\nInitialize via Request Failed [%x]\n", rc);
                CL_FUNC_EXIT();
                return rc;
            }
        }
        else
        {
            pUserInfoWithLen->finalizeCount++;
        }
    }

    CL_FUNC_EXIT();
    return CL_OK;
}


/*
 ** Simulates the Finalize API. Invoked during the second pass of the recovery.
 */
ClRcT clEvtCkptFinalizeSimulator(ClEvtCkptUserInfoWithLenT *pUserInfoWithLen)
{
    ClRcT rc = CL_OK;

    ClEvtCkptUserInfoT *pUserInfo = pUserInfoWithLen->pUserInfo;
    ClUint32T bytesReceived = pUserInfoWithLen->userInfoLen;
    ClUint32T bytesRead = 0;

    ClEvtUnsubscribeEventRequestT unsubscribeRequest = { 0 };


    CL_FUNC_ENTER();

    for (bytesRead = 0; bytesRead < bytesReceived;
         bytesRead += sizeof(ClEvtCkptUserInfoT), pUserInfo++)
    {
        if (CL_EVT_CKPT_OP_FINALIZE == pUserInfo->operation)
        {
            unsubscribeRequest.evtChannelHandle = 0;
            unsubscribeRequest.subscriptionId = 0;
            unsubscribeRequest.userId = pUserInfo->userId;
            unsubscribeRequest.reqFlag = CL_EVT_FINALIZE;

            rc = clEvtEventCleanupViaRequest(&unsubscribeRequest,
                                             CL_EVT_CKPT_REQUEST);
            if (CL_OK != rc)
            {
                clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_FINALISE,
                           "\nFinalize via Request Failed [%x]\n", rc);
                CL_FUNC_EXIT();
                return rc;
            }
        }
    }

    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** The wrapper to the CKPT read function.
 */
ClRcT clEvtCkptUserInfoRead(ClEvtCkptUserInfoWithLenT *pUserInfoWithLen)
{
    ClRcT rc = CL_OK;

    CL_FUNC_ENTER();

    rc = clCkptLibraryCkptDataSetRead(gClEvtCkptHandle, &gClEvtCkptName,
                                      CL_EVT_CKPT_USER_DSID,
                                       pUserInfoWithLen);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_USER,EVENT_LOG_CTX_READ,
                   "\nEvent CKPT DataSet Read Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    CL_FUNC_EXIT();

    return CL_OK;
}


/*
 ** Checkpointing for ECH Info -
 */

/*
 ** The walk that check points ECH User Info.
 */
ClRcT clEvtCkptUserofECHInfoWalk(ClCntKeyHandleT userKey,
                                 ClCntDataHandleT userData,
                                 ClCntArgHandleT userArg, ClUint32T dataLength)
{
    ClRcT rc = CL_OK;

    ClEvtCkptECHInfoT *pECHInfo = (ClEvtCkptECHInfoT *) userArg;

    ClUint32T channelScope;


    CL_FUNC_ENTER();

    pECHInfo->userId = *(ClEvtUserIdT *) userKey;

    CL_EVT_CHANNEL_SCOPE_GET(pECHInfo->chanHandle, channelScope);

    rc = clBufferNBytesWrite(gEvtCkptECHMsg[channelScope],
                                    (ClUint8T *) pECHInfo,
                                    sizeof(ClEvtCkptECHInfoT));
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_USER,EVENT_LOG_CTX_INFO,
                   "\nBuffer Message Write Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_BUFFER_MSG_WRITE_FAILED, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** The walk that obtains the channel handle for each channel.
 */
ClRcT clEvtCkptECHWalk(ClCntKeyHandleT userKey, ClCntDataHandleT userData,
                       ClCntArgHandleT userArg, ClUint32T dataLength)
{
    ClRcT rc = CL_OK;

    ClEvtChannelKeyT *pECHKey = (ClEvtChannelKeyT *) userKey;

    ClEvtChannelDBT *pChannelDB = (ClEvtChannelDBT *) userData;

    ClEvtCkptECHInfoWithMutexT *pEvtECHInfoWithMutex =
        (ClEvtCkptECHInfoWithMutexT *) userArg;

    ClEvtCkptECHInfoT evtCkptUserofECHInfo = { 0 };


    CL_FUNC_ENTER();

    /*
     * Fillin the channel related stuf here 
     */
    evtCkptUserofECHInfo.operation = CL_EVT_CKPT_OP_ECH_OPEN;
    evtCkptUserofECHInfo.chanHandle =
        CL_EVT_CHANNEL_HANDLE_FORM(pEvtECHInfoWithMutex->scope,
                                   pECHKey->channelId);
    clEvtUtilsNameCpy(&evtCkptUserofECHInfo.chanName,
                      &((ClEvtChannelKeyT *) userKey)->channelName);

    rc = clCntWalk((ClCntHandleT) pChannelDB->evtChannelUserInfo,
                   clEvtCkptUserofECHInfoWalk,
                   (ClCntArgHandleT) &evtCkptUserofECHInfo, 0);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_USER,EVENT_LOG_CTX_WALK,
                   "\nclEvtCkptUserofECHInfoWalk Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        /*
         * clOsalMutexUnlock(pChannelDB->channelLevelMutex); 
         */
        CL_FUNC_EXIT();
        return rc;
    }

    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** Complete check pointing for ECH Information done using the message buffer
 ** to hold the data to be check pointed temporarily. This is read from start
 ** and copied into a local buffer which is then passed to the CKPT library.
 */
ClRcT clEvtCkptECHSecPackComplete(ClUint32T dataSetId, ClAddrT *ppData,
                                  ClUint32T *pDataLen, ClPtrT pCookie)
{
    ClRcT rc = CL_OK;

    ClEvtCkptECHInfoWithMutexT *pEvtECHInfoWithMutex =
        (ClEvtCkptECHInfoWithMutexT *) pCookie;

    CL_FUNC_ENTER();

    /*
     ** Since it's a complete checkpoint we need to discard all the stale
     ** checkpointing info and begin anew.
     */
    rc = clBufferClear(gEvtCkptECHMsg[pEvtECHInfoWithMutex->scope]);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,
                   "\nSet Buffer Message Write Offset Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    rc = clOsalMutexLock(pEvtECHInfoWithMutex->mutexId);
    rc = clCntWalk(pEvtECHInfoWithMutex->channelContainer, clEvtCkptECHWalk,
                   (ClCntArgHandleT) pCookie, 0);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,"\nclEvtCkpECHWalk Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        clOsalMutexUnlock(pEvtECHInfoWithMutex->mutexId);
        CL_FUNC_EXIT();
        return rc;
    }
    rc = clOsalMutexUnlock(pEvtECHInfoWithMutex->mutexId);

    /*
     ** Create the buffer to be passed to CKPT since flatten would make the buf
     ** unusable. The size of the same can be obtained as below.
     */
    rc = clBufferLengthGet(gEvtCkptECHMsg[pEvtECHInfoWithMutex->scope],
                                  pDataLen);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,
                   "\nSet Message Buffer Read Offset Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    *ppData = (ClAddrT) clHeapAllocate(*pDataLen);
    if (NULL == *ppData)
    {
        clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,"\nMemory Allocation Failed\n\r\n");
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_0_MEMORY_ALLOCATION_FAILED);
        CL_FUNC_EXIT();
        return CL_EVENT_ERR_NO_MEM;
    }

    rc = clBufferNBytesRead(gEvtCkptECHMsg[pEvtECHInfoWithMutex->scope],
                                   (ClUint8T *) *ppData, pDataLen);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,"\nMessage Read Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_BUFFER_MSG_READ_FAILED, rc);
        clHeapFree(*ppData);
        CL_FUNC_EXIT();
        return rc;
    }

# ifdef FLAT_FILE
    {
        FILE *gEvtFp = fopen("echInfo.dat", "w+");

        if (NULL == gEvtFp)
        {
            clLogTrace(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,"Failed to open the Flat file\n");
            clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                       CL_LOG_MESSAGE_0_MEMORY_ALLOCATION_FAILED);
        }

        fwrite(*ppData, 1, *pDataLen, gEvtFp);
        fclose(gEvtFp);
    }
# endif

    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** Incremental check pointing for ECH Info done using message buffer. Unlike
 ** complete check point the data is written from current write offset. When 
 ** reading the message buffer it is read from beginning and passed to CKPT 
 ** library. This shall change if the library supports incremental check point.
 */
ClRcT clEvtCkptECHSecPackInc(ClUint32T dataSetId, ClAddrT *ppData,
                             ClUint32T *pDataLen, ClPtrT pCookie)
{
    ClRcT rc = CL_OK;

    ClEvtCkptECHInfoWithMutexT *pEvtECHInfoWithMutex =
        (ClEvtCkptECHInfoWithMutexT *) pCookie;

    ClEvtCkptECHInfoT evtCkptECHInfo = { 0 };


    CL_FUNC_ENTER();

    evtCkptECHInfo.operation = pEvtECHInfoWithMutex->operation;

    if (CL_EVT_CKPT_OP_ECH_OPEN == pEvtECHInfoWithMutex->operation)
    {
        ClEvtChannelOpenRequestT *pOpenReq = pEvtECHInfoWithMutex->pOpenReq;

        evtCkptECHInfo.chanHandle = pOpenReq->evtChannelHandle;

        clEvtUtilsNameCpy(&evtCkptECHInfo.chanName, &pOpenReq->evtChannelName);

        evtCkptECHInfo.userId = pOpenReq->userId;
        evtCkptECHInfo.isExternal = pOpenReq->isExternal;
    }
    else
    {
        ClEvtUnsubscribeEventRequestT *pUnsubsReq =
            pEvtECHInfoWithMutex->pUnsubsReq;

        evtCkptECHInfo.chanHandle = pUnsubsReq->evtChannelHandle;

        clEvtUtilsNameCpy(&evtCkptECHInfo.chanName,
                          &pUnsubsReq->evtChannelName);

        evtCkptECHInfo.userId = pUnsubsReq->userId;
    }

    /*
     ** For Incremental Check Pointing we needn't update the write pointer as
     ** we wish to append the data. All else is as in Complete Check Pointing.
     */
    rc = clBufferNBytesWrite(gEvtCkptECHMsg[pEvtECHInfoWithMutex->scope],
                                    (ClUint8T *) &evtCkptECHInfo,
                                    sizeof(ClEvtCkptECHInfoT));
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,
                   "\nBuffer Message Write Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_BUFFER_MSG_WRITE_FAILED, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    /*
     ** Create the buffer to be passed to CKPT since flatten would make the buf
     ** unusable. The size of the same can be obtained as below.
     */
    rc = clBufferLengthGet(gEvtCkptECHMsg[pEvtECHInfoWithMutex->scope],
                                  pDataLen);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,
                   "\nGet Buffer Message Length Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    *ppData = (ClAddrT) clHeapAllocate(*pDataLen);
    if (NULL == *ppData)
    {
        clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,"\nMemory Allocation Failed\n\r\n");
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_0_MEMORY_ALLOCATION_FAILED);
        CL_FUNC_EXIT();
        return CL_EVENT_ERR_NO_MEM;
    }

    /*
     ** Update the read offset before any read operation as you know where you
     ** wish to read from.
     */
    rc = clBufferReadOffsetSet(gEvtCkptECHMsg
                                      [pEvtECHInfoWithMutex->scope], 0,
                                      CL_BUFFER_SEEK_SET);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,
                   "\nSet Message Buffer Read Offset Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        clHeapFree(*ppData);
        CL_FUNC_EXIT();
        return rc;
    }

    rc = clBufferNBytesRead(gEvtCkptECHMsg[pEvtECHInfoWithMutex->scope],
                                   (ClUint8T *) *ppData, pDataLen);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,"\nMessage Read Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_BUFFER_MSG_READ_FAILED, rc);
        clHeapFree(*ppData);
        CL_FUNC_EXIT();
        return rc;
    }
# ifdef FLAT_FILE
    {
        FILE *gEvtFp = fopen("echInfo.dat", "a+");

        if (NULL == gEvtFp)
        {
            clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,"Failed to open the Flat file\n");
            clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                       CL_LOG_MESSAGE_0_MEMORY_ALLOCATION_FAILED);
        }

        fwrite(*ppData, 1, *pDataLen, gEvtFp);
        fclose(gEvtFp);
    }
# endif

    CL_FUNC_EXIT();
    return rc;
}

/*
 ** The serializer for ECH Info. It invokes the complete check pointing or
 ** incremental check pointing depending on the request.
 */
ClRcT clEvtCkptECHSerializer(ClUint32T dataSetId, ClAddrT *ppData,
                             ClUint32T *pDataLen, ClPtrT pCookie)
{
    ClRcT rc = CL_OK;

    CL_FUNC_ENTER();

    if (CL_EVT_CKPT_COMPLETE == ((ClEvtCkptECHInfoWithMutexT *) pCookie)->type)
    {
        rc = clEvtCkptECHSecPackComplete(dataSetId, ppData, pDataLen, pCookie);
        if (CL_OK != rc)
        {
            clLogError(EVENT_LOG_AREA_USER,EVENT_LOG_CTX_INFO,
                       "\nEvent CKPT UserInfo Complete Pack Failed [%x]\n",
                       rc);
            CL_FUNC_EXIT();
            return rc;
        }
    }
    else
    {
        rc = clEvtCkptECHSecPackInc(dataSetId, ppData, pDataLen, pCookie);
        if (CL_OK != rc)
        {
            clLogError(EVENT_LOG_AREA_USER,EVENT_LOG_CTX_INFO,
                       "\nEvent CKPT UserInfo Incremental Pack Failed [%x]\n",
                       rc);
            CL_FUNC_EXIT();
            return rc;
        }
    }

    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** The wrapper to CKPT write function.
 */
ClRcT clEvtCkptECHInfoWrite(ClEvtCkptECHInfoWithMutexT *pEvtECHInfoWithMutex)
{
    ClRcT rc = CL_OK;

    CL_FUNC_ENTER();

    rc = clCkptLibraryCkptDataSetWrite(gClEvtCkptHandle, &gClEvtCkptName,
                                       CL_EVT_CKPT_ECH_DSID
                                       (pEvtECHInfoWithMutex->scope),
                                        pEvtECHInfoWithMutex);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_INFO,
                   "\nEvent CKPT Dataset Write Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** The deserializer simply reads the data from the CKPT library and passes it
 ** back to the caller.
 */
ClRcT clEvtCkptECHDeserializer(ClUint32T dataSetId, ClAddrT pData,
                               ClUint32T dataLen, ClPtrT pCookie)
{
    ClEvtCkptECHInfoWithLenT *pECHInfoWithLen =
        (ClEvtCkptECHInfoWithLenT *) pCookie;

    CL_FUNC_ENTER();

    pECHInfoWithLen->pECHInfo = (ClEvtCkptECHInfoT *) pData;
    pECHInfoWithLen->echInfoLen = dataLen;

# ifdef FLAT_FILE
    {
        char *pInfo = clHeapAllocate(dataLen);

        if (NULL == pInfo)
        {
            clLogError(EVENT_LOG_AREA_CKPT,CL_LOG_CONTEXT_UNSPECIFIED,
                       "\nMemory Allocation Failed\n\r\n");
            clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                       CL_LOG_MESSAGE_0_MEMORY_ALLOCATION_FAILED);
            CL_FUNC_EXIT();
            return CL_EVENT_ERR_NO_MEM;
        }

        FILE *gEvtFp = fopen("echInfo.dat", "r+");

        if (NULL == gEvtFp)
        {
            clLogTrace(EVENT_LOG_AREA_CKPT,CL_LOG_CONTEXT_UNSPECIFIED,"Failed to open the Flat file\n");
            clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                       CL_LOG_MESSAGE_0_MEMORY_ALLOCATION_FAILED);
        }

        fread(pInfo, 1, dataLen, gEvtFp);

        fclose(gEvtFp);
        // NTC clHeapFree(pInfo);

        pECHInfoWithLen->pECHInfo = (ClEvtCkptECHInfoT *) pInfo;
    }
# endif

    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** Simulates the Channel Open API at the time of recovery. It is invoked
 ** during the first pass of the the reconstruction.
 */
ClRcT clEvtCkptChannelOpenSimulator(ClEvtCkptECHInfoWithLenT *pECHInfoWithLen)
{
    ClRcT rc = CL_OK;

    ClEvtCkptECHInfoT *pECHInfo = pECHInfoWithLen->pECHInfo;
    ClUint32T bytesReceived = pECHInfoWithLen->echInfoLen;
    ClUint32T bytesRead = 0;

    ClEvtChannelOpenRequestT echOpenRequest = { 0 };


    CL_FUNC_ENTER();

    for (bytesRead = 0; bytesRead < bytesReceived;
         bytesRead += sizeof(ClEvtCkptECHInfoT), pECHInfo++)
    {
        if (CL_EVT_CKPT_OP_ECH_OPEN == pECHInfo->operation)
        {
            echOpenRequest.evtChannelHandle = pECHInfo->chanHandle;
            echOpenRequest.userId = pECHInfo->userId;

            clEvtUtilsNameCpy(&echOpenRequest.evtChannelName,
                              &pECHInfo->chanName);

            rc = clEvtChannelOpenViaRequest(&echOpenRequest,
                                            CL_EVT_CKPT_REQUEST);
            if (CL_OK != rc)
            {
                clLogError(CL_LOG_AREA_UNSPECIFIED,CL_LOG_CONTEXT_UNSPECIFIED,
                           "\nChannel Open via Request Failed [%x]\n",
                           rc);
                CL_FUNC_EXIT();
                return rc;
            }
        }
        else
        {
            pECHInfoWithLen->echCloseCount++;
        }
    }

    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** Simulates the Channel Close API. Invoked during the second pass of the recovery.
 */
ClRcT clEvtCkptChannelCloseSimulator(ClEvtCkptECHInfoWithLenT *pECHInfoWithLen)
{
    ClRcT rc = CL_OK;

    ClEvtCkptECHInfoT *pECHInfo = pECHInfoWithLen->pECHInfo;

    ClUint32T bytesReceived = pECHInfoWithLen->echInfoLen;
    ClUint32T bytesRead = 0;

    ClEvtUnsubscribeEventRequestT unsubscribeRequest = { 0 };

    CL_FUNC_ENTER();

    for (bytesRead = 0; bytesRead < bytesReceived;
         bytesRead += sizeof(ClEvtCkptECHInfoT), pECHInfo++)
    {
        if (CL_EVT_CKPT_OP_ECH_CLOSE == pECHInfo->operation)
        {
            unsubscribeRequest.evtChannelHandle = pECHInfo->chanHandle;

            clEvtUtilsNameCpy(&unsubscribeRequest.evtChannelName,
                              &pECHInfo->chanName);

            unsubscribeRequest.userId = pECHInfo->userId;

            unsubscribeRequest.subscriptionId = 0;
            unsubscribeRequest.reqFlag = CL_EVT_CHANNEL_CLOSE;

            rc = clEvtEventCleanupViaRequest(&unsubscribeRequest,
                                             CL_EVT_CKPT_REQUEST);
            if (CL_OK != rc)
            {
                clLogError(CL_LOG_AREA_UNSPECIFIED,CL_LOG_CONTEXT_UNSPECIFIED,
                           "\nChannel Close via Request Failed [%x]\n",
                           rc);
                CL_FUNC_EXIT();
                return rc;
            }
        }
    }

    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** The wrapper to the CKPT read function.
 */
ClRcT clEvtCkptECHInfoRead(ClEvtCkptECHInfoWithLenT *pECHInfoWithLen)
{
    ClRcT rc = CL_OK;

    CL_FUNC_ENTER();

    rc = clCkptLibraryCkptDataSetRead(gClEvtCkptHandle, &gClEvtCkptName,
                                      CL_EVT_CKPT_ECH_DSID(pECHInfoWithLen->
                                                           scope),
                                       pECHInfoWithLen);
    if (CL_ERR_NOT_EXIST == CL_GET_ERROR_CODE(rc))
    {
        /*
         ** When Re-constructing if for global or local there is 
         ** not check pointed data we must suppress this errror.
         ** This check remains since a user may open a channel but
         ** not write anything into the channel.
         */
        return CL_OK;
    }
    else if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_READ,
                    "\nEvent CKPT ECHInfoRead Failed for %s channel %*s [%x]\n",
                        (pECHInfoWithLen->scope ==
                         CL_EVENT_GLOBAL_CHANNEL) ? "global" : "local",
                        pECHInfoWithLen->pECHInfo->chanName.length,
                        pECHInfoWithLen->pECHInfo->chanName.value, rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    CL_FUNC_EXIT();
    return CL_OK;
}

ClRcT clEvtCkptChannelOpenSimulate(ClEvtCkptECHInfoWithLenT *pECHInfoWithLen)
{
    ClRcT rc = CL_OK;

    CL_FUNC_ENTER();

    rc = clEvtCkptECHInfoRead(pECHInfoWithLen);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_READ,"\nECH Read Failed [%x]\n", rc);
        CL_FUNC_EXIT();
        return rc;
    }
    else if (0 != pECHInfoWithLen->echInfoLen)
    {
        rc = clEvtCkptChannelOpenSimulator(pECHInfoWithLen);
        if (CL_OK != rc)
        {
            clLogError(CL_LOG_AREA_UNSPECIFIED,CL_LOG_CONTEXT_UNSPECIFIED,
                       "\nChannel Open Simulation Failed [%x]\n", rc);
            CL_FUNC_EXIT();
            return rc;
        }
    }

    CL_FUNC_EXIT();
    return CL_OK;
}

ClRcT clEvtCkptChannelCloseSimulate(ClEvtCkptECHInfoWithLenT *pECHInfoWithLen)
{
    ClRcT rc = CL_OK;

    CL_FUNC_ENTER();

    if (0 != pECHInfoWithLen->echCloseCount)
    {
        rc = clEvtCkptChannelCloseSimulator(pECHInfoWithLen);
        if (CL_OK != rc)
        {
            clLogError(CL_LOG_AREA_UNSPECIFIED,CL_LOG_CONTEXT_UNSPECIFIED,
                       "\nChannel Close Simulation Failed [%x]\n", rc);
            CL_FUNC_EXIT();
            return rc;
        }
    }

    CL_FUNC_EXIT();
    return CL_OK;
}


/*
 ** Check Pointing for Subscription Information -
 */

/*
 ** Walk to check point subscription Info.
 */
ClRcT clEvtCkptSubsInfoWalk(ClCntKeyHandleT userKey, ClCntDataHandleT userData,
                            ClCntArgHandleT userArg, ClUint32T dataLength)
{
    ClRcT rc = CL_OK;

    ClEvtCkptSubsInfoWithMsgHdlT *pEvtSubsInfoWithMsgHdl =
        (ClEvtCkptSubsInfoWithMsgHdlT *) userArg;

    ClEvtCkptSubsInfoT *pSubsInfo = pEvtSubsInfoWithMsgHdl->pSubsInfo;

    ClEvtSubsKeyT *pSubsKey = (ClEvtSubsKeyT *) userKey;


    CL_FUNC_ENTER();

    pSubsInfo->userId = pSubsKey->userId;
    pSubsInfo->subsId = pSubsKey->subscriptionId;
    pSubsInfo->cookie = pSubsKey->pCookie;

    pSubsInfo->commPort = (ClIocPortT)(ClWordT) userData;

    rc = clBufferNBytesWrite(pEvtSubsInfoWithMsgHdl->msgHdl, 
                             (ClUint8T *) pSubsInfo,
                             sizeof(ClEvtCkptSubsInfoT));
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_WALK,
                   "\nBuffer Message Write Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_BUFFER_MSG_WRITE_FAILED, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    if (pSubsInfo->packedRbe)
    {
        rc = clBufferNBytesWrite(pEvtSubsInfoWithMsgHdl->msgHdl,
                                 pSubsInfo->packedRbe,
                                 pSubsInfo->packedRbeLen);
        if (CL_OK != rc) return rc;
    }

    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** Walk to obtain the Event Type Information.
 */
ClRcT clEvtCkptEventInfoWalk(ClCntKeyHandleT userKey, ClCntDataHandleT userData,
                             ClCntArgHandleT userArg, ClUint32T dataLength)
{
    ClRcT rc = CL_OK;

    ClEvtEventTypeInfoT *pEventTypeInfo = (ClEvtEventTypeInfoT *) userData;

    ClEvtEventTypeKeyT *pEvtEventTypeKey = (ClEvtEventTypeKeyT *)userKey;

    ClRuleExprT *pRbeExpr = pEvtEventTypeKey->key.pRbeExpr;

    ClEvtCkptSubsInfoT subsInfo = {0};

    ClEvtCkptSubsInfoWithMutexT *pSubsInfoWithMutex =
        (ClEvtCkptSubsInfoWithMutexT *) userArg;

    ClEvtCkptSubsInfoWithMsgHdlT evtSubsInfoWithMsgHdl = {0}; //NTC


    CL_FUNC_ENTER();

    evtSubsInfoWithMsgHdl.pSubsInfo = &subsInfo;
    evtSubsInfoWithMsgHdl.msgHdl = pSubsInfoWithMutex->msgHdl;
    subsInfo.operation = CL_EVT_CKPT_OP_SUBSCRIBE;

    subsInfo.chanHandle = pSubsInfoWithMutex->chanHandle;

    if (pRbeExpr)
    {
        rc = clRuleExprPack(pRbeExpr,
                            &subsInfo.packedRbe,
                            &subsInfo.packedRbeLen);
        if (CL_OK != rc)
        {
            CL_FUNC_EXIT();
            return rc;
        }
    }

    rc = clCntWalk((ClCntHandleT) pEventTypeInfo->subscriberInfo,
                   clEvtCkptSubsInfoWalk,
                   (ClCntArgHandleT) &evtSubsInfoWithMsgHdl, 0);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_WALK,"\nclEvtCkpECHWalk Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    clHeapFree(subsInfo.packedRbe);

    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** Complete check pointing for Subscripton Info done using the message buffer
 ** to hold the data to be check pointed temporarily. This is read from start
 ** and copied into a local buffer which is then passed to the CKPT library.
 */
ClRcT clEvtCkptSubsInfoSecPackComplete(ClUint32T dataSetId, ClAddrT *ppData,
                                       ClUint32T *pDataLen, ClPtrT pCookie)
{
    ClRcT rc = CL_OK;

    ClEvtCkptSubsInfoWithMutexT *pSubsInfoWithMutex =
        (ClEvtCkptSubsInfoWithMutexT *) pCookie;

    ClBufferHandleT subsMsgHandle = pSubsInfoWithMutex->msgHdl;


    CL_FUNC_ENTER();

    /*
     ** Since it's a complete checkpoint we need to discard all the stale
     ** checkpointing info and begin anew.
     */
    rc = clBufferClear(subsMsgHandle);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,
                   "\nSet Buffer Message Write Offset Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    rc = clCntWalk(pSubsInfoWithMutex->eventTypeContainer,
                   clEvtCkptEventInfoWalk, (ClCntArgHandleT) pCookie, 0);
    if (CL_OK != rc)
    {
         clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_PACK,
                    "\nclEvtCkptEventInfoWalk Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    /*
     ** Create the buffer to be passed to CKPT since flatten would make the buf
     ** unusable.
     */
    rc = clBufferLengthGet(subsMsgHandle, pDataLen);
    if (CL_OK != rc)
    {
         clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,
                    "\nGet Buffer Message Length Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    *ppData = (ClAddrT) clHeapAllocate(*pDataLen);
    if (NULL == *ppData)
    {
         clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,"\nMemory Allocation Failed\n\r\n");
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_0_MEMORY_ALLOCATION_FAILED);
        CL_FUNC_EXIT();
        return CL_EVENT_ERR_NO_MEM;
    }

    rc = clBufferNBytesRead(subsMsgHandle, (ClUint8T *) *ppData,
                                   pDataLen);
    if (CL_OK != rc)
    {
         clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,"\nMessage Read Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_BUFFER_MSG_READ_FAILED, rc);
        clHeapFree(*ppData);
        CL_FUNC_EXIT();
        return rc;
    }

# ifdef FLAT_FILE
    {
        FILE *gEvtFp = fopen("subsInfo.dat", "w+");

        if (NULL == gEvtFp)
        {
            clLogTrace(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,"Failed to open the Flat file\n");
            clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                       CL_LOG_MESSAGE_0_MEMORY_ALLOCATION_FAILED);
        }

        fwrite(*ppData, 1, *pDataLen, gEvtFp);
        fclose(gEvtFp);
    }
# endif

    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** Incremental check pointing for User Info done using message buffer. Unlike
 ** complete check point the data is written from current write offset. When 
 ** reading the message buffer it is read from beginning and passed to CKPT 
 ** library. This shall change if the library supports incremental check point.
 */
ClRcT clEvtCkptSubsInfoSecPackInc(ClUint32T dataSetId, ClAddrT *ppData,
                                  ClUint32T *pDataLen, ClPtrT pCookie)
{
    ClRcT rc = CL_OK;

    ClEvtCkptSubsInfoWithMutexT *pSubsInfoWithMutex =
        (ClEvtCkptSubsInfoWithMutexT *) pCookie;

    ClEvtCkptSubsInfoT subsInfo = {0};

    ClBufferHandleT subsMsgHandle = pSubsInfoWithMutex->msgHdl;

    CL_FUNC_ENTER();

    if (CL_EVT_CKPT_OP_SUBSCRIBE == pSubsInfoWithMutex->operation)
    {
        ClEvtSubscribeEventRequestT *pSubsReq = pSubsInfoWithMutex->pSubsReq;

        ClRuleExprT *pRbeExpr = NULL;

        if (pSubsReq->packedRbeLen)
        {
            clRuleExprUnpack(pSubsReq->packedRbe,
                             pSubsReq->packedRbeLen,
                             &pRbeExpr);
        }

        if (pRbeExpr)
        {
            rc = clRuleExprPack(pRbeExpr,
                                &subsInfo.packedRbe,
                                &subsInfo.packedRbeLen);
            if (CL_OK != rc)
            {
                CL_FUNC_EXIT();
                return rc;
            }
            clRuleExprDeallocate(pRbeExpr);
        }

        subsInfo.operation = CL_EVT_CKPT_OP_SUBSCRIBE;

        subsInfo.chanHandle = pSubsReq->evtChannelHandle;

        subsInfo.userId = pSubsReq->userId;

        subsInfo.cookie = pSubsReq->pCookie;

        subsInfo.subsId = pSubsReq->subscriptionId;

        subsInfo.commPort = pSubsReq->subscriberCommPort;
    }
    else
    {
        ClEvtUnsubscribeEventRequestT *pUnsubsReq =
            pSubsInfoWithMutex->pUnsubsReq;

        subsInfo.operation = CL_EVT_CKPT_OP_UNSUBSCRIBE;

        subsInfo.chanHandle = pUnsubsReq->evtChannelHandle;

        subsInfo.userId = pUnsubsReq->userId;

        subsInfo.subsId = pUnsubsReq->subscriptionId;

        subsInfo.packedRbe = NULL;
        subsInfo.packedRbeLen = 0;
    }

    /*
     ** For Incremental Check Pointing we needn't update the write pointer as
     ** we wish to append the data. All else is as in Complete Check Pointing.
     */
    rc = clBufferNBytesWrite(subsMsgHandle,
                             (ClUint8T *) &subsInfo,
                             sizeof(ClEvtCkptSubsInfoT));
    if (CL_OK != rc)
    {
         clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,
                    "\nBuffer Message Write Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_BUFFER_MSG_WRITE_FAILED, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    if (subsInfo.packedRbe)
    {
        rc = clBufferNBytesWrite(subsMsgHandle,
                                 (ClUint8T *)subsInfo.packedRbe,
                                 subsInfo.packedRbeLen);
    }

    /*
     ** Create the buffer to be passed to CKPT since flatten would make the buf
     ** unusable.
     */
    rc = clBufferLengthGet(subsMsgHandle, pDataLen);
    if (CL_OK != rc)
    {
         clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,
                    "\nGet Buffer Message Length Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    *ppData = (ClAddrT) clHeapAllocate(*pDataLen);
    if (NULL == *ppData)
    {
         clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,"\nMemory Allocation Failed\n\r\n");
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_0_MEMORY_ALLOCATION_FAILED);
        CL_FUNC_EXIT();
        return CL_EVENT_ERR_NO_MEM;
    }

    /*
     ** Update the read offset before any read operation as you know where you
     ** wish to read from
     */
    rc = clBufferReadOffsetSet(subsMsgHandle, 0, CL_BUFFER_SEEK_SET);
    if (CL_OK != rc)
    {
         clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,
                    "\nSet Message Buffer Read Offset Failed [%x]\n", rc);
        clHeapFree(*ppData);
        CL_FUNC_EXIT();
        return rc;
    }


    rc = clBufferNBytesRead(subsMsgHandle, (ClUint8T *) *ppData,
                                   pDataLen);
    if (CL_OK != rc)
    {
         clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,"\nMessage Read Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_LOG_MESSAGE_1_BUFFER_MSG_READ_FAILED, rc);
        clHeapFree(*ppData);
        CL_FUNC_EXIT();
        return rc;
    }

    clHeapFree(subsInfo.packedRbe);

# ifdef FLAT_FILE
    {
        FILE *gEvtFp = fopen("subsInfo.dat", "a+");

        if (NULL == gEvtFp)
        {
            clLogTrace(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,"Failed to open the Flat file\n");
            clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                       CL_LOG_MESSAGE_0_MEMORY_ALLOCATION_FAILED);
        }

        fwrite(*ppData, 1, *pDataLen, gEvtFp);
        fclose(gEvtFp);
    }
# endif

    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** The serializer for User Info. It invokes the complete check pointing or
 ** incremental check pointing depending on the request.
 */
ClRcT clEvtCkptSubsSerializer(ClUint32T dataSetId, ClAddrT *ppData,
                              ClUint32T *pDataLen, ClPtrT pCookie)
{
    ClRcT rc = CL_OK;

    CL_FUNC_ENTER();

    if (CL_EVT_CKPT_COMPLETE == ((ClEvtCkptSubsInfoWithMutexT *) pCookie)->type)
    {
        rc = clEvtCkptSubsInfoSecPackComplete(dataSetId, ppData, pDataLen,
                                              pCookie);
        if (CL_OK != rc)
        {
             clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_PACK,
                        "\nEvent CKPT Complete SubsInfo Pack Failed [%x]\n",
                        rc);
            CL_FUNC_EXIT();
            return rc;
        }
    }
    else
    {
        rc = clEvtCkptSubsInfoSecPackInc(dataSetId, ppData, pDataLen, pCookie);
        if (CL_OK != rc)
        {
             clLogError(EVENT_LOG_AREA_SEC,EVENT_LOG_CTX_PACK,
                        "\nEvent CKPT Incremental SubsInfo Pack Failed [%x]\n",
                        rc);
            CL_FUNC_EXIT();
            return rc;
        }
    }

    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** The wrapper to CKPT write function.
 */
ClRcT clEvtCkptSubsInfoWrite(ClEvtCkptSubsInfoWithMutexT *pEvtSubsInfoWithMutex)
{
    ClRcT rc = CL_OK;

    CL_FUNC_ENTER();

# ifdef EVT_DEBUG
    if (NULL == pEvtSubsInfoWithMutex ||
        NULL == pEvtSubsInfoWithMutex->pChanName)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_WRITE,
                    "\nSubscription Request or Channel Name is NULL\n");
        CL_FUNC_EXIT();
        return CL_EVENT_ERR_NULL_PTR;
    }
# endif

    rc = clCkptLibraryCkptDataSetWrite(gClEvtCkptHandle,
                                       pEvtSubsInfoWithMutex->pChanName,
                                       CL_EVT_CKPT_SUBS_DSID
                                       (pEvtSubsInfoWithMutex->scope),
                                        pEvtSubsInfoWithMutex);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_WRITE, 
                   "\nEvent CKPT Dataset Write Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** The deserializer simply reads the data from the CKPT library and passes it
 ** back to the caller.
 */
ClRcT clEvtCkptSubsDeserializer(ClUint32T dataSetId, ClAddrT pData,
                                ClUint32T dataLen, ClPtrT pCookie)
{

    ClEvtCkptSubsInfoWithLenT *pSubsInfoWithLen =
        (ClEvtCkptSubsInfoWithLenT *) pCookie;

    CL_FUNC_ENTER();

    pSubsInfoWithLen->pSubsInfo = (ClEvtCkptSubsInfoT *) pData;
    pSubsInfoWithLen->subsInfoLen = dataLen;

# ifdef FLAT_FILE
    {
        char *pInfo = clHeapAllocate(dataLen);

        if (NULL == pInfo)
        {
            clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_READ,
                       "\nMemory Allocation Failed\n\r\n");
            clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                       CL_LOG_MESSAGE_0_MEMORY_ALLOCATION_FAILED);
            CL_FUNC_EXIT();
            return CL_EVENT_ERR_NO_MEM;
        }

        FILE *gEvtFp = fopen("subsInfo.dat", "r+");

        if (NULL == gEvtFp)
        {
            clLogTrace(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_WRITE,"Failed to open the Flat file\n");
            clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                       CL_LOG_MESSAGE_0_MEMORY_ALLOCATION_FAILED);
        }

        fread(pInfo, 1, dataLen, gEvtFp);

        fclose(gEvtFp);
        // NTC clHeapFree(pInfo);

        pSubsInfoWithLen->pSubsInfo = (ClEvtCkptSubsInfoT *) pInfo;
    }
# endif

    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** Simulates the Subscription and Unsubscription. It does so in a single pass.
 */
ClRcT clEvtCkptSubsUnsubsSimulator(ClEvtCkptSubsInfoWithLenT *pSubsInfoWithLen)
{
    ClRcT rc = CL_OK;

    ClEvtCkptSubsInfoT *pSubsInfo = pSubsInfoWithLen->pSubsInfo;

    ClUint32T bytesReceived = pSubsInfoWithLen->subsInfoLen;
    ClUint32T bytesRead = 0;

    CL_FUNC_ENTER();

    while (bytesRead < bytesReceived)
    {
        if (CL_EVT_CKPT_OP_SUBSCRIBE == pSubsInfo->operation)
        {
            ClEvtSubscribeEventRequestT subsReq;

            memset(&subsReq, 0, sizeof(ClEvtSubscribeEventRequestT));

            subsReq.evtChannelHandle = pSubsInfo->chanHandle;
            subsReq.subscriberCommPort = pSubsInfo->commPort;
            subsReq.userId = pSubsInfo->userId;
            subsReq.subscriptionId = pSubsInfo->subsId;
            subsReq.pCookie = pSubsInfo->cookie;

            clEvtUtilsNameCpy(&subsReq.evtChannelName,
                              &pSubsInfoWithLen->chanName);

            if (pSubsInfo->packedRbeLen)
            {
                subsReq.packedRbeLen = pSubsInfo->packedRbeLen;

                subsReq.packedRbe = (ClUint8T*) clHeapAllocate(pSubsInfo->packedRbeLen * sizeof(ClUint8T));
                if (!subsReq.packedRbe) return CL_EVENTS_RC(CL_ERR_NO_MEMORY);

                pSubsInfo->packedRbe = (ClUint8T *)((ClCharT *)pSubsInfo+sizeof(ClEvtCkptSubsInfoT));
                memcpy(subsReq.packedRbe, pSubsInfo->packedRbe, pSubsInfo->packedRbeLen);
            }

            rc = clEvtEventSubscribeViaRequest(&subsReq, CL_EVT_CKPT_REQUEST);
            if (CL_OK != rc)
            {
                clLogError(EVENT_LOG_AREA_EVENT,EVENT_LOG_CTX_SUBSCRIBE,
                           "\nSubscribe via Request for Channel[%*s]"
                           "Failed [%x]\n",
                           subsReq.evtChannelName.length,
                           subsReq.evtChannelName.value, rc);
                CL_FUNC_EXIT();
                return rc;
            }

            clHeapFree(subsReq.packedRbe);
        }
        else                    /* Unsubscribe Operation - *
                                 * CL_EVT_CKPT_OP_UNSUBSCRIBE */
        {
            ClEvtUnsubscribeEventRequestT unsubsReq;

            memset(&unsubsReq, 0, sizeof(ClEvtUnsubscribeEventRequestT));

            unsubsReq.evtChannelHandle = pSubsInfo->chanHandle;
            unsubsReq.userId = pSubsInfo->userId;
            unsubsReq.subscriptionId = pSubsInfo->subsId;
            unsubsReq.reqFlag = CL_EVT_UNSUBSCRIBE;

            clEvtUtilsNameCpy(&unsubsReq.evtChannelName,
                              &pSubsInfoWithLen->chanName);

            rc = clEvtEventCleanupViaRequest(&unsubsReq, CL_EVT_CKPT_REQUEST);
            if (CL_OK != rc)
            {
                clLogError(EVENT_LOG_AREA_EVENT,CL_LOG_CONTEXT_UNSPECIFIED,
                           "\nUnsubscribe via Request for Channel[%*s]"
                           "Failed [%x]\n",
                           unsubsReq.evtChannelName.length,
                           unsubsReq.evtChannelName.value, rc);
                CL_FUNC_EXIT();
                return rc;
            }
        }
        /*** Update the length and the data pointer ***/

        bytesRead += (sizeof(ClEvtCkptSubsInfoT)+pSubsInfo->packedRbeLen);

        if (bytesRead < bytesReceived)  /* To Avoid accessing data beyond
                                         * boundary */
        {
            pSubsInfo = (ClEvtCkptSubsInfoT *) ((ClCharT *)pSubsInfo +
                                                sizeof(ClEvtCkptSubsInfoT) +
                                                pSubsInfo->packedRbeLen);
        }
    }

    CL_FUNC_EXIT();
    return CL_OK;
}

/*
 ** The wrapper to the CKPT read function.
 */
ClRcT clEvtCkptSubsInfoRead(ClEvtCkptSubsInfoWithLenT *pSubsInfoWithLen)
{
    ClRcT rc = CL_OK;

    CL_FUNC_ENTER();

    /*
     ** Reset length each time to verify if the anything was read.
     */
    pSubsInfoWithLen->subsInfoLen = 0;

    rc = clCkptLibraryCkptDataSetRead(gClEvtCkptHandle,
                                      &pSubsInfoWithLen->chanName,
                                      CL_EVT_CKPT_SUBS_DSID(pSubsInfoWithLen->
                                                            scope),
                                       pSubsInfoWithLen);
    if (CL_ERR_NOT_EXIST == CL_GET_ERROR_CODE(rc))
    {
        /*
         ** When Re-constructing if no subscription info read for publishers
         ** we must suppress this errror. Better fix on flag.
         */
        return CL_OK;
    }
    else if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_READ,
                   "\nEvent CKPT SubsInfoRead Failed for %s channel %*s [%x]\n",
                   (pSubsInfoWithLen->scope ==
                   CL_EVENT_GLOBAL_CHANNEL) ? "global" : "local",
                   pSubsInfoWithLen->chanName.length,
                   pSubsInfoWithLen->chanName.value, rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    CL_FUNC_EXIT();
    return CL_OK;
}


/*
 ** Check Pointing API for the various API in EM -
 */

ClRcT clEvtCkptCheckPointInitialize(ClEvtInitRequestT *pEvtInitReq)
{
    ClRcT rc = CL_OK;

    ClEvtCkptUserInfoWithMutexT userInfoWithMutex = { 0 };


    CL_FUNC_ENTER();

    /*
     ** This checkpoint is always incremental as it's better.
     ** The counter API could be incremental/complete depending 
     ** on threshold.
     */
    userInfoWithMutex.type = CL_EVT_CKPT_INCREMENTAL;
    userInfoWithMutex.operation = CL_EVT_CKPT_OP_INITIALIZE;
    userInfoWithMutex.pInitReq = pEvtInitReq;

    rc = clEvtCkptUserInfoWrite(&userInfoWithMutex);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_INI,
                   "Check Pointing Initialize Failed [%x]", rc);
        CL_FUNC_EXIT();
        return rc;
    }

    CL_FUNC_EXIT();
    return CL_OK;
}

ClRcT clEvtCkptCheckPointFinalize(ClEvtUnsubscribeEventRequestT *pEvtUnsubsReq)
{
    ClRcT rc = CL_OK;

    ClEvtCkptUserInfoWithMutexT userInfoWithMutex = { 0 };
    ClEvtInitRequestT evtInitReq = { 0 };


    CL_FUNC_ENTER();

    userInfoWithMutex.operation = CL_EVT_CKPT_OP_FINALIZE;

    if (++gEvtCkptFinalizeCount > CL_EVT_CKPT_THRESHOLD)
    {
        /*
         * Complete Check Pointing 
         */

        gEvtCkptFinalizeCount = 0;  /* Reset the Finalize Count since complete */

        userInfoWithMutex.databaseHandle = gEvtHandleDatabaseHdl; /* Ignore Request field 
                                                         */

        userInfoWithMutex.type = CL_EVT_CKPT_COMPLETE;
    }
    else
    {
        /*
         ** Incremental Check Pointing 
         ** The Init request is used as there is not Finalize Request
         */

        evtInitReq.userId = pEvtUnsubsReq->userId;

        userInfoWithMutex.pInitReq = &evtInitReq;
        userInfoWithMutex.type = CL_EVT_CKPT_INCREMENTAL;
    }

    rc = clEvtCkptUserInfoWrite(&userInfoWithMutex);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_FINALISE,
                   "Check Pointing Finalize Failed [%x]", rc);
        CL_FUNC_EXIT();
        return rc;
    }

    CL_FUNC_EXIT();
    return CL_OK;
}


ClRcT clEvtCkptCheckPointChannelOpen(ClEvtChannelOpenRequestT
                                     *pEvtChannelOpenRequest)
{
    ClRcT rc = CL_OK;

    ClEvtCkptECHInfoWithMutexT echWithMutex = { 0 };

    CL_FUNC_ENTER();

    CL_EVT_CHANNEL_SCOPE_GET(pEvtChannelOpenRequest->evtChannelHandle,
                             echWithMutex.scope);

    echWithMutex.type = CL_EVT_CKPT_INCREMENTAL;
    echWithMutex.operation = CL_EVT_CKPT_OP_ECH_OPEN;

    if (echWithMutex.scope == CL_EVENT_LOCAL_CHANNEL)
    {
        echWithMutex.mutexId = gEvtLocalECHMutex;
    }
    else
    {
        echWithMutex.mutexId = gEvtGlobalECHMutex;
    }

    echWithMutex.pOpenReq = pEvtChannelOpenRequest;

    rc = clEvtCkptECHInfoWrite(&echWithMutex);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_WRITE,
                   "\nEvent CKPT ECHWrite Failed [%x]\n", rc);
        CL_FUNC_EXIT();
        return rc;
    }

    CL_FUNC_EXIT();
    return CL_OK;
}


ClRcT clEvtCkptCheckPointChannelClose(ClEvtUnsubscribeEventRequestT
                                      *pEvtUnsubsReq)
{
    ClRcT rc = CL_OK;

    ClEvtCkptECHInfoWithMutexT echWithMutex = { 0 };

    ClUint32T channelScope;


    CL_FUNC_ENTER();

    CL_EVT_CHANNEL_SCOPE_GET(pEvtUnsubsReq->evtChannelHandle, channelScope);

    echWithMutex.scope = channelScope;

    echWithMutex.operation = CL_EVT_CKPT_OP_ECH_CLOSE;

    echWithMutex.mutexId =
        (channelScope ==
         CL_EVENT_LOCAL_CHANNEL) ? gEvtLocalECHMutex : gEvtGlobalECHMutex;

    if (++gEvtCkptChannelCloseCount[channelScope] > CL_EVT_CKPT_THRESHOLD)
    {
        /*
         ** Reset the counter on complete check pointing 
         */
        echWithMutex.type = CL_EVT_CKPT_COMPLETE;

        gEvtCkptChannelCloseCount[channelScope] = 0;

        echWithMutex.channelContainer =
            (channelScope ==
             CL_EVENT_LOCAL_CHANNEL) ? gpEvtLocalECHDb->
            evtChannelContainer : gpEvtGlobalECHDb->evtChannelContainer;
    }
    else
    {
        echWithMutex.type = CL_EVT_CKPT_INCREMENTAL;
        echWithMutex.pUnsubsReq = pEvtUnsubsReq;
    }

    rc = clEvtCkptECHInfoWrite(&echWithMutex);
    if (CL_OK != rc)
    {
       clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_WRITE,
                  "Check Pointing Channel Close Failed [%x]", rc);
        CL_FUNC_EXIT();
        return rc;
    }

    CL_FUNC_EXIT();
    return CL_OK;
}


ClRcT clEvtCkptCheckPointSubscribe(ClEvtSubscribeEventRequestT *pEvtSubsReq,
                                   ClEvtChannelDBT *pEvtChannelDB)
{
    ClRcT rc = CL_OK;

    ClEvtCkptSubsInfoWithMutexT subsInfoWithMutex = { 0 };

    CL_FUNC_ENTER();


    subsInfoWithMutex.type = CL_EVT_CKPT_INCREMENTAL;
    subsInfoWithMutex.operation = CL_EVT_CKPT_OP_SUBSCRIBE;

    subsInfoWithMutex.pSubsReq = pEvtSubsReq;
    subsInfoWithMutex.pChanName = &pEvtSubsReq->evtChannelName;
    subsInfoWithMutex.chanHandle = pEvtSubsReq->evtChannelHandle;
    subsInfoWithMutex.msgHdl = pEvtChannelDB->subsCkptMsgHdl;
    CL_EVT_CHANNEL_SCOPE_GET(pEvtSubsReq->evtChannelHandle,
                             subsInfoWithMutex.scope);

    rc = clEvtCkptSubsInfoWrite(&subsInfoWithMutex);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_SUBSCRIBE,
                   "Check Pointing Subscription Failed [%x]", rc);
        CL_FUNC_EXIT();
        return rc;
    }

    CL_FUNC_EXIT();

    return CL_OK;
}


ClRcT clEvtCkptCheckPointUnsubscribe(ClEvtUnsubscribeEventRequestT
                                     *pEvtUnsubsReq,
                                     ClEvtChannelDBT *pEvtChannelDB)
{
    ClRcT rc = CL_OK;

    ClEvtCkptSubsInfoWithMutexT subsInfoWithMutex = { 0 };

    ClUint32T channelScope;


    CL_FUNC_ENTER();

    subsInfoWithMutex.operation = CL_EVT_CKPT_OP_UNSUBSCRIBE;

    subsInfoWithMutex.pChanName = &pEvtUnsubsReq->evtChannelName;
    subsInfoWithMutex.chanHandle = pEvtUnsubsReq->evtChannelHandle;
    subsInfoWithMutex.msgHdl = pEvtChannelDB->subsCkptMsgHdl;

    CL_EVT_CHANNEL_SCOPE_GET(pEvtUnsubsReq->evtChannelHandle, channelScope);

    if (++gEvtCkptUnsubscribeCount[channelScope] > CL_EVT_CKPT_THRESHOLD)
    {
        /*
         * Complete Check Pointing 
         */

        ClEvtSubscribeEventRequestT evtSubsReq = { 0 };

        evtSubsReq.evtChannelHandle = pEvtUnsubsReq->evtChannelHandle;

        subsInfoWithMutex.scope = channelScope;

        clEvtUtilsNameCpy(&evtSubsReq.evtChannelName,
                          &pEvtUnsubsReq->evtChannelName);

        subsInfoWithMutex.pSubsReq = &evtSubsReq;

        subsInfoWithMutex.eventTypeContainer = pEvtChannelDB->eventTypeInfo;

        subsInfoWithMutex.mutexId = pEvtChannelDB->channelLevelMutex;

        subsInfoWithMutex.type = CL_EVT_CKPT_COMPLETE;

        /*
         ** Reset the counter on complete check pointing 
         */
        gEvtCkptUnsubscribeCount[channelScope] = 0;
    }
    else
    {
        subsInfoWithMutex.pUnsubsReq = pEvtUnsubsReq;

        subsInfoWithMutex.type = CL_EVT_CKPT_INCREMENTAL;

        CL_EVT_CHANNEL_SCOPE_GET(pEvtUnsubsReq->evtChannelHandle,
                                 subsInfoWithMutex.scope);
    }

    rc = clEvtCkptSubsInfoWrite(&subsInfoWithMutex);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,CL_LOG_CONTEXT_UNSPECIFIED,
                   "Check Pointing Unsubscription Failed [%x]", rc);
        CL_FUNC_EXIT();
        return rc;
    }


    CL_FUNC_EXIT();
    return CL_OK;
}


/*
 ** Data Structure Check Pointing (complete) API -
 */

ClRcT clEvtCkptUserInfoCheckPoint(void)
{
    ClRcT rc = CL_OK;

    ClEvtCkptUserInfoWithMutexT userInfo = { 0 };


    CL_FUNC_ENTER();

    userInfo.type = CL_EVT_CKPT_COMPLETE;
    userInfo.operation = CL_EVT_CKPT_OP_INITIALIZE;
    userInfo.databaseHandle = gEvtHandleDatabaseHdl; // NTC

    rc = clEvtCkptUserInfoWrite(&userInfo);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_INFO,
                   "\nCheck Pointing User Info Failed [%x]\n", rc);
        CL_FUNC_EXIT();
        return rc;
    }


    CL_FUNC_EXIT();
    return CL_OK;
}

ClRcT clEvtCkptECHInfoCheckPoint(void)
{
    ClRcT rc = CL_OK;

    ClEvtCkptECHInfoWithMutexT echInfoWithMutex = { 0 };


    CL_FUNC_ENTER();

    echInfoWithMutex.type = CL_EVT_CKPT_COMPLETE;
    echInfoWithMutex.operation = CL_EVT_CKPT_OP_ECH_OPEN;

    /*
     * Check Point Local Channel Information 
     */
    echInfoWithMutex.scope = CL_EVENT_LOCAL_CHANNEL;
    echInfoWithMutex.mutexId = gEvtLocalECHMutex;

    echInfoWithMutex.channelContainer = gpEvtLocalECHDb->evtChannelContainer;

    rc = clEvtCkptECHInfoWrite(&echInfoWithMutex);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_INFO,
                   "\nCheck Pointing Local ECH Info Failed [%x]\n", rc);
        CL_FUNC_EXIT();
        return rc;
    }

    /*
     * Check Point Global Channel Information 
     */
    echInfoWithMutex.scope = CL_EVENT_GLOBAL_CHANNEL;
    echInfoWithMutex.mutexId = gEvtGlobalECHMutex;

    echInfoWithMutex.channelContainer = gpEvtGlobalECHDb->evtChannelContainer;

    rc = clEvtCkptECHInfoWrite(&echInfoWithMutex);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_INFO,
                   "\nCheck Pointing Global ECH Info Failed [%x]\n", rc);
        CL_FUNC_EXIT();
        return rc;
    }

    CL_FUNC_EXIT();
    return CL_OK;
}


ClRcT clEvtCkptSubsInfoCheckPointWalk(ClCntKeyHandleT userKey,
                                      ClCntDataHandleT userData,
                                      ClCntArgHandleT userArg,
                                      ClUint32T dataLength)
{
    ClRcT rc = CL_OK;

    ClEvtChannelDBT *pChannelDB = (ClEvtChannelDBT *) userData;

    ClEvtChannelKeyT *pECHKey = (ClEvtChannelKeyT *) userKey;

    ClEvtCkptSubsInfoWithMutexT *pSubsInfoWithMutex =
        (ClEvtCkptSubsInfoWithMutexT *) userArg;

    ClEvtSubscribeEventRequestT *pSubsReq = pSubsInfoWithMutex->pSubsReq;


    CL_FUNC_ENTER();

    pSubsReq->evtChannelHandle =
        CL_EVT_CHANNEL_HANDLE_FORM(pSubsInfoWithMutex->scope,
                                   pECHKey->channelId);

    clEvtUtilsNameCpy(&pSubsReq->evtChannelName, &pECHKey->channelName);
    pSubsInfoWithMutex->pChanName = &pSubsReq->evtChannelName;
    pSubsInfoWithMutex->chanHandle = pSubsReq->evtChannelHandle; // NTC

    pSubsInfoWithMutex->mutexId = pChannelDB->channelLevelMutex;
    pSubsInfoWithMutex->eventTypeContainer = pChannelDB->eventTypeInfo;
    pSubsInfoWithMutex->msgHdl = pChannelDB->subsCkptMsgHdl;

    rc = clEvtCkptSubsInfoWrite(pSubsInfoWithMutex);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_WALK,
                   "\nEvent CKPT Subcription Info Write Failed [%x]\n",
                   rc);
        CL_FUNC_EXIT();
        return rc;
    }


    CL_FUNC_EXIT();

    return CL_OK;
}


ClRcT clEvtCkptSubsInfoCheckPoint(void)
{
    ClRcT rc = CL_OK;

    ClEvtSubscribeEventRequestT subsReq = { 0 };

    ClEvtCkptSubsInfoWithMutexT subsInfoWithMutex = { 0 };


    CL_FUNC_ENTER();

    subsInfoWithMutex.type = CL_EVT_CKPT_COMPLETE;
    subsInfoWithMutex.operation = CL_EVT_CKPT_OP_SUBSCRIBE;

    subsInfoWithMutex.pSubsReq = &subsReq;

    /*
     ** Check Point all the Local Info
     */
    subsInfoWithMutex.scope = CL_EVENT_LOCAL_CHANNEL;

    rc = clCntWalk(gpEvtLocalECHDb->evtChannelContainer,
                   clEvtCkptSubsInfoCheckPointWalk,
                   (ClCntArgHandleT) &subsInfoWithMutex, 0);

    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_WALK,
                   "\nLocal clEvtCkptSubsInfoCheckPointWalk Failed [%x]\n",
                   rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    /*
     ** Check Point all the Global Info
     */
    subsInfoWithMutex.scope = CL_EVENT_GLOBAL_CHANNEL;

    rc = clCntWalk(gpEvtGlobalECHDb->evtChannelContainer,
                   clEvtCkptSubsInfoCheckPointWalk,
                   (ClCntArgHandleT) &subsInfoWithMutex, 0);

    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_WALK,
                   "\nGlobal clEvtCkptSubsInfoCheckPointWalk Failed [%x]\n",
                   rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    CL_FUNC_EXIT();
    return CL_OK;
}

ClRcT clEvtCkptCheckPointAll(void)
{
    ClRcT rc = CL_OK;

    CL_FUNC_ENTER();

    rc = clEvtCkptUserInfoCheckPoint();
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_USER,EVENT_LOG_CTX_INFO,
                   "\nCheck Pointing User Info Write Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    rc = clEvtCkptECHInfoCheckPoint();
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,CL_LOG_CONTEXT_UNSPECIFIED,
                   "\nCheck Pointing ECH Info Write Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    rc = clEvtCkptSubsInfoCheckPoint();
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_WRITE,
                   "\nCheck Pointing Subscription Info Write Failed [%x]\n",
                    rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    CL_FUNC_EXIT();
    return CL_OK;
}

ClRcT clEvtCkptSubsInfoReconstructWalk(ClCntKeyHandleT userKey,
                                       ClCntDataHandleT userData,
                                       ClCntArgHandleT userArg,
                                       ClUint32T dataLength)
{
    ClRcT rc = CL_OK;

    ClEvtCkptSubsInfoWithLenT *pSubsInfoWithLen =
        (ClEvtCkptSubsInfoWithLenT *) userArg;

    CL_FUNC_ENTER();

    if( ((ClEvtChannelKeyT*)userKey)->channelName.length > CL_MAX_NAME_LENGTH)
    {
        clLogWarning("SUBS", "CONSTRUCT", "Ignoring subscriber entry with large channel length [%d]",
                     ((ClEvtChannelKeyT*)userKey)->channelName.length);
        return CL_OK;
    }

    clEvtUtilsNameCpy(&pSubsInfoWithLen->chanName,
                      &((ClEvtChannelKeyT *) userKey)->channelName);

    rc = clEvtCkptSubsInfoRead(pSubsInfoWithLen);
    if (0 != pSubsInfoWithLen->subsInfoLen)
    {
        rc = clEvtCkptSubsUnsubsSimulator(pSubsInfoWithLen);
    }

    CL_FUNC_EXIT();
    return CL_OK;
}


ClRcT clEvtCkptSubsInfoReconstruct(ClEvtCkptSubsInfoWithLenT *pSubsInfoWithLen)
{
    ClRcT rc = CL_OK;

    CL_FUNC_ENTER();

    /*
     * Check point Local Channel Information 
     */

    pSubsInfoWithLen->scope = CL_EVENT_LOCAL_CHANNEL;

    rc = clCntWalk(gpEvtLocalECHDb->evtChannelContainer,
                   clEvtCkptSubsInfoReconstructWalk,
                   (ClCntArgHandleT) pSubsInfoWithLen, 0);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_CTX_SUBSCRIBE,EVENT_LOG_CTX_RECONSTRUCT,
                   "\nLocal clEvtCkptSubsInfoReconstructWalk Failed [%x]\n",
                   rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }


    /*
     * Check point Global Channel Information 
     */

    pSubsInfoWithLen->scope = CL_EVENT_GLOBAL_CHANNEL;

    rc = clCntWalk(gpEvtGlobalECHDb->evtChannelContainer,
                   clEvtCkptSubsInfoReconstructWalk,
                   (ClCntArgHandleT) pSubsInfoWithLen, 0);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_CTX_SUBSCRIBE,EVENT_LOG_CTX_RECONSTRUCT,
                   "\nGlobal clEvtCkptSubsInfoReconstructWalk Failed [%x]\n",
                   rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    CL_FUNC_EXIT();
    return CL_OK;
}


/*
 ** This channel Re-constructs the necessary structures for EM.
 ** First Initialize is simulated then the Open and Subscribe.
 ** All these calls should be successful. If there are any
 ** counter calls they are then executed to trim the structures
 ** to result in the failure time state.
 */

ClRcT clEvtCkptReconstruct(void)
{
    ClRcT rc = CL_OK;

    ClEvtCkptUserInfoWithLenT userInfoWithLen = { NULL };

    ClEvtCkptECHInfoWithLenT echInfoWithLen[2] = { {NULL} };

    ClEvtCkptSubsInfoWithLenT subsInfoWithLen = { NULL };


    CL_FUNC_ENTER();

    rc = clEvtCkptUserInfoRead(&userInfoWithLen);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_USER,EVENT_LOG_CTX_INFO,"\nUser Info Read Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }
    else if (0 == userInfoWithLen.userInfoLen)
    {
      clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_RECONSTRUCT,"Checkpoint reconstruction failed, user info length is 0");
    }
    else if (userInfoWithLen.pUserInfo->userId.evtHandle == CL_HANDLE_INVALID_VALUE) 
    {
      clLogError(EVENT_LOG_AREA_EVENT,EVENT_LOG_CTX_RECONSTRUCT,"Checkpoint reconstruction failed, handle is invalid");
    }
    else
      {
        rc = clEvtCkptInitializeSimulator(&userInfoWithLen);
        if (CL_OK != rc)
        {
            clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_INI,
                       "\nInitialize Simulation Failed [%x]\n", rc);
            clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                       CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
            CL_FUNC_EXIT();
            return rc;
        }
    }

    /*
     * Simulate the Channel Open for Local and Global Channels 
     */

    echInfoWithLen[CL_EVENT_LOCAL_CHANNEL].scope = CL_EVENT_LOCAL_CHANNEL;
    rc = clEvtCkptChannelOpenSimulate(&echInfoWithLen[CL_EVENT_LOCAL_CHANNEL]);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_EVENT,EVENT_LOG_CTX_OPEN,
                   "\n Local Channel Open Simulate Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    echInfoWithLen[CL_EVENT_GLOBAL_CHANNEL].scope = CL_EVENT_GLOBAL_CHANNEL;
    rc = clEvtCkptChannelOpenSimulate(&echInfoWithLen[CL_EVENT_GLOBAL_CHANNEL]);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_EVENT,EVENT_LOG_CTX_OPEN,
                   "\n Global Channel Open Simulate Failed [%x]\n", rc);
        clLogWrite(CL_LOG_HANDLE_APP, CL_LOG_SEV_DEBUG, NULL,
                   CL_EVENT_LOG_MSG_1_INTERNAL_ERROR, rc);
        CL_FUNC_EXIT();
        return rc;
    }

    /*
     * Reconstruct the Subscription Info 
     */
    rc = clEvtCkptSubsInfoReconstruct(&subsInfoWithLen);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_CTX_SUBSCRIBE,EVENT_LOG_CTX_RECONSTRUCT,
                   "\nReconstructing Subcription Info Write Failed [%x]\n",
                   rc);
        CL_FUNC_EXIT();
        return rc;
    }

    /*
     * Simulate the Channel Close for Local and Global Channels 
     */
    rc = clEvtCkptChannelCloseSimulate(&echInfoWithLen[CL_EVENT_LOCAL_CHANNEL]);
    if (CL_OK != rc)
    {
        clLogError(CL_LOG_AREA_UNSPECIFIED,CL_LOG_CONTEXT_UNSPECIFIED,
                   "\n Local Channel Close Simulate Failed [%x]\n", rc);
        CL_FUNC_EXIT();
        return rc;
    }

    rc = clEvtCkptChannelCloseSimulate(&echInfoWithLen
                                       [CL_EVENT_GLOBAL_CHANNEL]);
    if (CL_OK != rc)
    {
        clLogError(CL_LOG_AREA_UNSPECIFIED,CL_LOG_CONTEXT_UNSPECIFIED,
                   "\n Global Channel Close Simulate Failed [%x]\n", rc);
        CL_FUNC_EXIT();
        return rc;
    }

    /*
     ** Check if there were any counter calls. If yes Simulate
     ** them now.
     */

    if (0 != userInfoWithLen.finalizeCount)
    {
        rc = clEvtCkptFinalizeSimulator(&userInfoWithLen);
        if (CL_OK != rc)
        {
            clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_CTX_FINALISE,
                       "\nFinalize Simulation Failed [%x]\n", rc);
            CL_FUNC_EXIT();
            return rc;
        }
    }

    CL_FUNC_EXIT();
    return CL_OK;
}



#define MAX_USER 100
#define MAX_CHANNEL 100
#define MAX_SUB 100
#define  CL_EVENT_CHANNEL_OPEN_SIZE  sizeof(ClEvtChannelOpenRequestT)
#define  CL_CHANNEL_OPEN_SEC_ID_SIZE       sizeof(SaNameT)
#define  CL_EVENT_CHANNEL_SUB_SIZE    sizeof(ClEvtChannelOpenRequestT) 
#define  CL_CHANNEL_SUB_SEC_ID_SIZE       sizeof(SaNameT)
#define  CL_EVENT_USER_INFO_SIZE       sizeof(ClEvtInitRequestT) + 2*sizeof(ClUint64T)
#define  CL_USER_INFO_SEC_ID_SIZE sizeof(SaNameT)
#define CL_EVENT_CKPT_RETENTION_DURATION (CL_TIME_END)
#define CL_EVENT_MAX_RETRIES 3

ClIocNodeAddressT  currentNodeLeader;

const SaNameT 
gUserInfoCkptName     = {
                      sizeof("clEventUserInfoGlobalCkpt") - 1,
                      "clEventUserInfoGlobalCkpt"
                     };
const SaNameT 
gChannelOpenCkptName     = {
                      sizeof("clEventChannelOpenGlobalCkpt") - 1,
                      "clEventChannelOpenGlobalCkpt"
                     };
const SaNameT 
gChannelSubCkptName     = {
                      sizeof("clEventChannelSubGlobalCkpt") - 1,
                      "clEventChannelSubGlobalCkpt"
                     };

ClCkptSvcHdlT  ckpt_svc_handle;   // handle for checkpoint service

ClCkptHdlT     ckpt_channel_open_handle;  // handle for our checkpoint
ClCkptHdlT     ckpt_channel_sub_handle;  // handle for our checkpoint
ClCkptHdlT     ckpt_user_info_handle;  // handle for our checkpoint

ClVersionT           ckpt_version = {'B', 1, 1};


ClRcT clEventGlobalCkptInitial()
{
        ClRcT           rc  = CL_OK;
	    rc = clCkptInitialize(&ckpt_svc_handle, /* Checkpoint service handle */
	                                NULL,           /* Optional callbacks table */
	                                &ckpt_version); /* Required verison number */
    	clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clCkptInitialize begin");
	    if (rc != CL_OK)
	    {
	    	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clCkptInitialize failed");
	        return rc;
	    }
	    sleep(2);
	    rc=clEventUserInfoCheckpointCreate((SaNameT *) &gUserInfoCkptName);
	    if (rc != CL_OK)
	    {
	    	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"create userInfo global checkpoint failed");
	    	return rc;
	    }
	    if(clCpmIsMaster())
	    {
	    	rc = clCkptActiveReplicaSet(ckpt_user_info_handle);	
	    }
	    
	    rc=clEventChannelOpenCheckpointCreate((SaNameT *) &gChannelOpenCkptName);
	    if (rc != CL_OK)
	    {
	    	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"create User channelOpen global checkpoint failed()");
	      	return rc;
	    }
	    if(clCpmIsMaster())
	    {
	    	rc = clCkptActiveReplicaSet(ckpt_channel_open_handle);	
	    }
	    
	    rc=clEventChannelSubCheckpointCreate((SaNameT *) &gChannelSubCkptName);
	    if (rc != CL_OK)
	    {
	    	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"create channelSub global checkpoint failed()");
	      	return rc;
	    }
	    if(clCpmIsMaster())
	    {
	    	rc = clCkptActiveReplicaSet(ckpt_channel_sub_handle);	
	    }
	    clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clCkptInitialize end");
	    return rc;
	    
}

ClRcT clEventGlobalCkptFinalize()
{
	clCkptCheckpointDelete(ckpt_svc_handle, &gChannelOpenCkptName);
	clCkptCheckpointDelete(ckpt_svc_handle, &gUserInfoCkptName);
	clCkptCheckpointDelete(ckpt_svc_handle, &gChannelSubCkptName);
	return CL_OK;
}

ClRcT clEventSerialiser(ClUint32T  dsId,
                      ClAddrT    *pBuffer,
                      ClUint32T  *pSize,
                      ClPtrT     cookie)
{
    ClRcT           rc  = CL_OK;
    ClBufferHandleT msg = (ClBufferHandleT)cookie;
    
    rc = clBufferLengthGet(msg, pSize);
    if( CL_OK != rc )
    {
    	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clEventSerialiser:  clBufferLengthGet(): rc[0x %x]", rc);        
        return rc;
    }
    *pBuffer = (ClAddrT) clHeapCalloc(*pSize, sizeof(ClUint8T));
    if( NULL == *pBuffer )
    {
    	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clEventSerialiser: clHeapCalloc()");
        return rc;
    }
    rc = clBufferNBytesRead(msg, (ClUint8T *)*pBuffer, pSize);
    if( CL_OK != rc )
    {
    	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clEventSerialiser : clBufferNBytesRead(): rc[0x %x]", rc);
        clHeapFree(*pBuffer);
        return rc;
    }

    return rc;
}
//***************************** create channel open checkpoint *********************************
ClRcT clEventChannelOpenCheckpointCreate(SaNameT         *pCkptName)
{
    ClRcT         rc     = CL_OK;
    ClCkptCheckpointCreationAttributesT  creationAtt    = {0};
    ClCkptOpenFlagsT                     openFlags      = 0;
    ClInt32T                             tries = 0;
    static ClTimerTimeOutT delay = { 0, 100};  
    creationAtt.creationFlags     = CL_CKPT_CHECKPOINT_COLLOCATED | CL_CKPT_ALL_OPEN_ARE_REPLICAS;
    creationAtt.checkpointSize    = MAX_CHANNEL * CL_EVENT_CHANNEL_OPEN_SIZE;
    creationAtt.retentionDuration = CL_EVENT_CKPT_RETENTION_DURATION;
    creationAtt.maxSections       = MAX_CHANNEL;
    creationAtt.maxSectionSize    = CL_EVENT_CHANNEL_OPEN_SIZE;
    creationAtt.maxSectionIdSize  = CL_CHANNEL_OPEN_SEC_ID_SIZE;

    openFlags = CL_CKPT_CHECKPOINT_CREATE | CL_CKPT_CHECKPOINT_WRITE | CL_CKPT_CHECKPOINT_READ;

    reopen:
    rc = clCkptCheckpointOpen(ckpt_svc_handle, pCkptName, &creationAtt, openFlags, 5000L, &ckpt_channel_open_handle);
    if( rc != CL_OK )
    {
        /*
         * No replica found and we are the only master.
         * Delete and try re-opening the checkpoint
         */
        if(CL_GET_ERROR_CODE(rc) == CL_ERR_NO_RESOURCE)
        {
            if(tries++ < 1)
            {
                clLogNotice("CKP", "GET", "No replica for log checkpoint."
                            "Deleting ckpt [%.*s] and retrying the ckpt open",
                            pCkptName->length, pCkptName->value);
                clCkptCheckpointDelete(ckpt_svc_handle, pCkptName);
                clOsalTaskDelay(delay);
                goto reopen;
            }
        }
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clCkptCheckpointOpen(): rc[0x %x]", rc);
    }
    
    return rc;
}
//***************************** create channel sub checkpoint *********************************
ClRcT clEventChannelSubCheckpointCreate(SaNameT         *pCkptName)
{
    ClRcT         rc     = CL_OK;
    ClCkptCheckpointCreationAttributesT  creationAtt    = {0};
    ClCkptOpenFlagsT                     openFlags      = 0;
    ClInt32T                             tries = 0;
    static ClTimerTimeOutT delay = { 0, 100};


    creationAtt.creationFlags     = CL_CKPT_CHECKPOINT_COLLOCATED | CL_CKPT_ALL_OPEN_ARE_REPLICAS;
    creationAtt.checkpointSize    = MAX_CHANNEL * CL_EVENT_CHANNEL_SUB_SIZE;
    creationAtt.retentionDuration = CL_EVENT_CKPT_RETENTION_DURATION;
    creationAtt.maxSections       = MAX_CHANNEL;// max user
    creationAtt.maxSectionSize    = CL_EVENT_CHANNEL_SUB_SIZE;
    creationAtt.maxSectionIdSize  = CL_CHANNEL_SUB_SEC_ID_SIZE;

    openFlags = CL_CKPT_CHECKPOINT_CREATE | CL_CKPT_CHECKPOINT_WRITE |
        CL_CKPT_CHECKPOINT_READ;

    reopen:
    rc = clCkptCheckpointOpen(ckpt_svc_handle,
                              pCkptName, &creationAtt, openFlags, 5000L,
                              &ckpt_channel_sub_handle);
    if( rc != CL_OK )
    {
        /*
         * No replica found and we are the only master.
         * Delete and try re-opening the checkpoint
         */
        if(CL_GET_ERROR_CODE(rc) == CL_ERR_NO_RESOURCE)
        {
            if(tries++ < 1)
            {
                clLogNotice("CKP", "GET", "No replica for log checkpoint."
                            "Deleting ckpt [%.*s] and retrying the ckpt open",
                            pCkptName->length, pCkptName->value);
                clCkptCheckpointDelete(ckpt_svc_handle, pCkptName);
                clOsalTaskDelay(delay);
                goto reopen;
            }
        }
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clCkptCheckpointOpen(): rc[0x %x]", rc);
    }

    return rc;
}

//***************************** create user checkpoint *********************************
ClRcT clEventUserInfoCheckpointCreate(SaNameT         *pCkptName)
{
    ClRcT         rc     = CL_OK;
    ClCkptCheckpointCreationAttributesT  creationAtt    = {0};
    ClCkptOpenFlagsT                     openFlags      = 0;
    ClInt32T                             tries = 0;
    static ClTimerTimeOutT delay = { 0, 100};
    creationAtt.creationFlags     = CL_CKPT_CHECKPOINT_COLLOCATED | CL_CKPT_ALL_OPEN_ARE_REPLICAS;
    creationAtt.checkpointSize    = MAX_USER * CL_EVENT_USER_INFO_SIZE;
    creationAtt.retentionDuration = CL_EVENT_CKPT_RETENTION_DURATION;
    creationAtt.maxSections       = MAX_USER;// max user
    creationAtt.maxSectionSize    = CL_EVENT_USER_INFO_SIZE;
    creationAtt.maxSectionIdSize  = CL_USER_INFO_SEC_ID_SIZE;

    openFlags = CL_CKPT_CHECKPOINT_CREATE | CL_CKPT_CHECKPOINT_WRITE |
        CL_CKPT_CHECKPOINT_READ;

    reopen:
    rc = clCkptCheckpointOpen(ckpt_svc_handle,
                              pCkptName, &creationAtt, openFlags, 5000L,
                              &ckpt_user_info_handle);
    if( rc != CL_OK )
    {
        /*
         * No replica found and we are the only master.
         * Delete and try re-opening the checkpoint
         */
        if(CL_GET_ERROR_CODE(rc) == CL_ERR_NO_RESOURCE)
        {
            if(tries++ < 1)
            {
                clLogNotice("CKP", "GET", "No replica for log checkpoint."
                            "Deleting ckpt [%.*s] and retrying the ckpt open",
                            pCkptName->length, pCkptName->value);
                clCkptCheckpointDelete(ckpt_svc_handle, pCkptName);
                clOsalTaskDelay(delay);
                goto reopen;
            }
        }
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clCkptCheckpointOpen(): rc[0x %x]", rc);
    }

    return rc;
}

//***************************** create and write UserInfo *********************************
ClRcT clEvtCkptGlobalCheckPointUserInfo(ClEvtInitRequestT *pEvtInitReq)
{
    ClRcT rc = CL_OK;
    ClBufferHandleT                   msg                     =
        CL_HANDLE_INVALID_VALUE;
    ClUint32T                         size                    = 0;
    ClAddrT                           pBuffer                 = NULL;
    ClCkptSectionIdT                  secId                   = {0};
    ClCkptSectionCreationAttributesT  secAttr                 = {0};
    ClBoolT                           createdSec              = CL_FALSE;

    secId.idLen = 16;
    secId.id    = (ClUint8T*)clHeapCalloc(secId.idLen, sizeof(ClCharT));
    if( NULL == secId.id )
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clHeapCalloc()");
        return CL_EVENTS_RC(CL_ERR_NO_MEMORY);
    }
    ClUint64T section_index = pEvtInitReq->userId.evtHandle; /*our section name apparently*/    
    char buff[21];
    sprintf(buff, "%llu", section_index);
    memcpy(secId.id,buff,16);
    clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clEvtCkptGlobalCheckPointUserInfo : userId.evtHandle: [%s]", buff);
    clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clEvtCkptGlobalCheckPointUserInfo : userId.evtHandle len: [%d]", (&secId)->idLen);
    secAttr.sectionId      = &secId;
    secAttr.expirationTime = CL_TIME_END;    
    
    rc = clCkptSectionCreate(ckpt_user_info_handle, &secAttr, NULL, 0);
    if( CL_OK != rc )
    {
        clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clEvtCkptGlobalCheckPointUserInfo : clCkptSectionCreate(): rc[0x %x]", rc);
        if(CL_GET_ERROR_CODE(rc) == CL_ERR_ALREADY_EXIST) 
        {
            clLogNotice("INFO", "SYNCUP","Section already exists.");
            rc = CL_OK;     
        }
        else
        {
        	clHeapFree(secId.id);
            return rc;
        }
    }
    
    rc = clBufferCreate(&msg);
    if( CL_OK != rc )
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clEvtCkptGlobalCheckPointUserInfo : clBufferCreate(): rc[0x %x]", rc);
        if( CL_TRUE == createdSec )
        {
            CL_EVENT_CLEANUP(clCkptSectionDelete(ckpt_user_info_handle, &secId),
                           CL_OK);
        }
        clHeapFree(secId.id);
        return rc;
    }
    
    rc = VDECL_VER(clXdrMarshallClEvtInitRequestT, 4, 0, 0)(pEvtInitReq, msg, 0);
    if( CL_OK != rc )
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clEvtCkptGlobalCheckPointUserInfo : clXdrMarshallSaNameT(): rc[0x %x]", rc);
        if(CL_TRUE == createdSec)
        {
            CL_EVENT_CLEANUP(clCkptSectionDelete(ckpt_user_info_handle, &secId), CL_OK);
        }
        clHeapFree(secId.id);
        return rc;
    }  

    rc = clEventSerialiser(0, &pBuffer, &size, msg);
    if( CL_OK != rc )
    {
        CL_EVENT_CLEANUP(clBufferDelete(&msg), CL_OK);
        if( CL_TRUE == createdSec )
        {
            CL_EVENT_CLEANUP(clCkptSectionDelete(ckpt_user_info_handle, &secId),
                           CL_OK);
        }
        clHeapFree(secId.id);
        return rc;
    }    
    rc = clCkptSectionOverwrite(ckpt_user_info_handle,
                                &secId,
                                pBuffer, size);
    if( CL_OK != rc )
    {     
        if( CL_TRUE == createdSec )
        {
            CL_EVENT_CLEANUP(clCkptSectionDelete(ckpt_user_info_handle, &secId),
                           CL_OK);
        }
    }    
    CL_EVENT_CLEANUP(clBufferDelete(&msg), CL_OK);
    clHeapFree(pBuffer);
    clHeapFree(secId.id);
    CL_FUNC_EXIT();
    return CL_OK;
}

//***************************** create and write channel open checkpoint *********************************
ClRcT clEvtCkptGlobalCheckPointChannelOpen(ClEvtChannelOpenRequestT
                                     *pEvtChannelOpenRequest)
{
    ClRcT rc = CL_OK;
    ClBufferHandleT                   msg                     =
        CL_HANDLE_INVALID_VALUE;
    ClUint32T                         size                    = 0;
    ClAddrT                           pBuffer                 = NULL;
    ClCkptSectionIdT                  secId                   = {0};
    ClCkptSectionCreationAttributesT  secAttr                 = {0};
    ClBoolT                           createdSec              = CL_FALSE;
    secId.idLen = pEvtChannelOpenRequest->evtChannelName.length;
    secId.id    = (ClUint8T*)clHeapCalloc(secId.idLen, sizeof(ClCharT));
    if( NULL == secId.id )
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clHeapCalloc()");
        return CL_EVENTS_RC(CL_ERR_NO_MEMORY);
    }
    memcpy(secId.id , pEvtChannelOpenRequest->evtChannelName.value,pEvtChannelOpenRequest->evtChannelName.length);
    secAttr.sectionId      = &secId;
    secAttr.expirationTime = CL_TIME_END;
    rc = clCkptSectionCreate(ckpt_channel_open_handle, &secAttr,
                                 NULL, 0);
    if( CL_OK != rc )
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clEvtCkptGlobalCheckPointChannelOpen: clCkptSectionCreate(): rc[0x %x]", rc);
        if(CL_GET_ERROR_CODE(rc) == CL_ERR_ALREADY_EXIST) 
        {
            clLogNotice("INFO", "SYNCUP","Section already exists.");
            rc = CL_OK;     
        }
        else
        {
        	clHeapFree(secId.id);
            return rc;
        }
    }

    rc = clBufferCreate(&msg);
    if( CL_OK != rc )
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clEvtCkptGlobalCheckPointChannelOpen: clBufferCreate(): rc[0x %x]", rc);
        if( CL_TRUE == createdSec )
        {
            CL_EVENT_CLEANUP(clCkptSectionDelete(ckpt_channel_open_handle, &secId),
                           CL_OK);
        }
        clHeapFree(secId.id);
        return rc;
    }

    rc = VDECL_VER(clXdrMarshallClEvtChannelOpenRequestT, 4, 0, 0)(pEvtChannelOpenRequest, msg, 0);
    if( CL_OK != rc )
    {
        if(CL_TRUE == createdSec)
        {
            CL_EVENT_CLEANUP(clCkptSectionDelete(ckpt_channel_open_handle, &secId), CL_OK);
        }
        clHeapFree(secId.id);
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clEvtCkptGlobalCheckPointChannelOpen: clXdrMarshallSaNameT(): rc[0x %x]", rc);
        return rc;
    }

    rc = clEventSerialiser(0, &pBuffer, &size, msg);
    if( CL_OK != rc )
    {
        CL_EVENT_CLEANUP(clBufferDelete(&msg), CL_OK);
        if( CL_TRUE == createdSec )
        {
            CL_EVENT_CLEANUP(clCkptSectionDelete(ckpt_channel_open_handle, &secId),
                           CL_OK);
        }
        clHeapFree(secId.id);
        return rc;
    }

    rc = clCkptSectionOverwrite(ckpt_channel_open_handle,
                                &secId,
                                pBuffer, size);
    if( CL_OK != rc )
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clEvtCkptGlobalCheckPointChannelOpen: clCkptSectionOverwrite(): rc[0x %x]", rc);
        if( CL_TRUE == createdSec )
        {
            CL_EVENT_CLEANUP(clCkptSectionDelete(ckpt_channel_open_handle, &secId),
                           CL_OK);
        }
        clHeapFree(secId.id);
    }
    CL_EVENT_CLEANUP(clBufferDelete(&msg), CL_OK);
    clHeapFree(pBuffer);
    clHeapFree(secId.id);
    CL_FUNC_EXIT();
    return CL_OK;
}

//***************************** create and write channel sub checkpoint *********************************
ClRcT clEvtCkptGlobalCheckPointChannelSub(ClEvtSubscribeEventRequestT *pEvtSubsReq)
{
    ClRcT rc = CL_OK;
    ClBufferHandleT                   msg                     =
        CL_HANDLE_INVALID_VALUE;
    ClUint32T                         size                    = 0;
    ClAddrT                           pBuffer                 = NULL;
    ClCkptSectionIdT                  secId                   = {0};
    ClCkptSectionCreationAttributesT  secAttr                 = {0};
    ClBoolT                           createdSec              = CL_FALSE;
    secId.idLen = pEvtSubsReq->evtChannelName.length;
    secId.id    = (ClUint8T*)clHeapCalloc(secId.idLen, sizeof(ClCharT));
    if( NULL == secId.id )
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clHeapCalloc()");
        return CL_EVENTS_RC(CL_ERR_NO_MEMORY);
    }
    memcpy(secId.id , pEvtSubsReq->evtChannelName.value,pEvtSubsReq->evtChannelName.length);
    secAttr.sectionId      = &secId;
    secAttr.expirationTime = CL_TIME_END;
    rc = clCkptSectionCreate(ckpt_channel_sub_handle, &secAttr, NULL, 0);
    if( CL_OK != rc )
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clEvtCkptGlobalCheckPointChannelSub: clCkptSectionCreate(): rc[0x %x]", rc);
        if(CL_GET_ERROR_CODE(rc) == CL_ERR_ALREADY_EXIST) 
        {
            clLogNotice("INFO", "SYNCUP","Section already exists.");
            rc = CL_OK;     
        }
        else
        {
        	clHeapFree(secId.id);
            return rc;
        }
    }

    rc = clBufferCreate(&msg);
    if( CL_OK != rc )
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clEvtCkptGlobalCheckPointChannelSub: clBufferCreate(): rc[0x %x]", rc);
        if( CL_TRUE == createdSec )
        {
            CL_EVENT_CLEANUP(clCkptSectionDelete(ckpt_channel_sub_handle, &secId),
                           CL_OK);
        }
        clHeapFree(secId.id);
        return rc;
    }

    rc = VDECL_VER(clXdrMarshallClEvtSubscribeEventRequestT, 4, 0, 0)(pEvtSubsReq, msg, 0);
    if( CL_OK != rc )
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clEvtCkptGlobalCheckPointChannelSub: clXdrMarshallSaNameT(): rc[0x %x]", rc);
        if(CL_TRUE == createdSec)
        {
            CL_EVENT_CLEANUP(clCkptSectionDelete(ckpt_channel_sub_handle, &secId), CL_OK);
        }
        clHeapFree(secId.id);
        return rc;
    }

    rc = clEventSerialiser(0, &pBuffer, &size, msg);
    if( CL_OK != rc )
    {
        CL_EVENT_CLEANUP(clBufferDelete(&msg), CL_OK);
        if( CL_TRUE == createdSec )
        {
            CL_EVENT_CLEANUP(clCkptSectionDelete(ckpt_channel_sub_handle, &secId),
                           CL_OK);
        }
        clHeapFree(secId.id);
        return rc;
    }

    rc = clCkptSectionOverwrite(ckpt_channel_sub_handle,
                                &secId,
                                pBuffer, size);
    if( CL_OK != rc )
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clEvtCkptGlobalCheckPointChannelSub: clCkptSectionOverwrite(): rc[0x %x]", rc);
        if( CL_TRUE == createdSec )
        {
            CL_EVENT_CLEANUP(clCkptSectionDelete(ckpt_channel_sub_handle, &secId),
                           CL_OK);
        }
    }
    CL_EVENT_CLEANUP(clBufferDelete(&msg), CL_OK);
    clHeapFree(pBuffer);
    clHeapFree(secId.id);
    CL_FUNC_EXIT();
    return CL_OK;
}
//***************************** UnMarshall channel open checkpoint msg*********************************
ClRcT
clEventUserInfoEntryUnpackNAdd(ClBufferHandleT        msg)
{
    ClRcT             rc         = CL_OK;
    ClEvtInitRequestT *pEvtInitReq = NULL;
    pEvtInitReq = (ClEvtInitRequestT*)clHeapCalloc(1, sizeof(ClEvtInitRequestT));
    if( NULL == pEvtInitReq )
    {
    	clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clHeapCalloc()");
        return CL_EVENTS_RC(CL_ERR_NO_MEMORY);
    }    

    rc = VDECL_VER(clXdrUnmarshallClEvtInitRequestT, 4, 0, 0)(msg,pEvtInitReq);
    if( CL_OK != rc )
    {        
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clXdrUnMarshallSaNameT(): rc[0x %x]", rc);
        return rc;
    }   
    ClUint64T section_index = pEvtInitReq->userId.evtHandle; /*our section name apparently*/
    clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"Simulator Initial via Request for Channel[%#llX]", section_index);  
    rc = clEvtInitializeViaRequest(pEvtInitReq,CL_EVT_CKPT_REQUEST);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,
                   "\nInitialize via Request Failed [%x]\n", rc);
                    CL_FUNC_EXIT();
                    return rc;
                }
    return rc;
}    


//***************************** UnMarshall channel open checkpoint msg*********************************
ClRcT
clEventChannelOpenEntryUnpackNAdd(ClBufferHandleT        msg)
{
    ClRcT             rc         = CL_OK;
    ClEvtChannelOpenRequestT *pEvtChannelOpenRequest = NULL;
    pEvtChannelOpenRequest = (ClEvtChannelOpenRequestT*)clHeapCalloc(1, sizeof(ClEvtChannelOpenRequestT));
    if( NULL == pEvtChannelOpenRequest )
    {
    	clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clHeapCalloc()");
        return CL_EVENTS_RC(CL_ERR_NO_MEMORY);
    }    

    rc = VDECL_VER(clXdrUnmarshallClEvtChannelOpenRequestT, 4, 0, 0)(msg,pEvtChannelOpenRequest);
    if( CL_OK != rc )
    {        
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"ChannelOpenRequestT(): rc[0x %x]", rc);
        return rc;
    }   
    clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"\n Simulator Open via Request for Channel[%*s] \n",pEvtChannelOpenRequest->evtChannelName.length,pEvtChannelOpenRequest->evtChannelName.value);
    rc = clEvtChannelOpenViaRequest(pEvtChannelOpenRequest,CL_EVT_CKPT_REQUEST);
    if (CL_OK != rc)
    {
         clLogError(CL_LOG_AREA_UNSPECIFIED,CL_LOG_CONTEXT_UNSPECIFIED,
                    "\nChannel Open via Request Failed [%x]\n",
                     rc);
         CL_FUNC_EXIT();
         return rc;
    }
    return rc;
}    

//***************************** UnMarshall channel sub checkpoint msg*********************************
ClRcT
clEventChannelSubEntryUnpackNAdd(ClBufferHandleT        msg)
{
    ClRcT             rc         = CL_OK;
    ClEvtSubscribeEventRequestT *pEvtSubsReq = NULL;
    pEvtSubsReq = (ClEvtSubscribeEventRequestT*)clHeapCalloc(1, sizeof(ClEvtSubscribeEventRequestT));
    if( NULL == pEvtSubsReq )
    {
    	clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clHeapCalloc()");
        return CL_EVENTS_RC(CL_ERR_NO_MEMORY);
    }    

    rc = VDECL_VER(clXdrUnmarshallClEvtSubscribeEventRequestT, 4, 0, 0)(msg,pEvtSubsReq);
    if( CL_OK != rc )
    {        
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"SubscribeEventRequestT: rc[0x %x]", rc);
        return rc;
    }   
	clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"\n Simulator Subscribe via Request for Channel[%*s] \n",
			pEvtSubsReq->evtChannelName.length,pEvtSubsReq->evtChannelName.value);
    rc = clEvtEventSubscribeViaRequest(pEvtSubsReq, CL_EVT_CKPT_REQUEST);
    if (CL_OK != rc)
    {
        clLogError(EVENT_LOG_AREA_EVENT,EVENT_LOG_CTX_SUBSCRIBE,
                   "\nSubscribe via Request for Channel Failed [%x]\n",rc);
        CL_FUNC_EXIT();
        return rc;
    }
    
    return rc;
}    

ClRcT
clEventChannelSubEntryClean(ClBufferHandleT        msg,ClIocNodeAddressT  nodeLeave)
{
    ClRcT             rc         = CL_OK;
    ClEvtSubscribeEventRequestT *pEvtSubsReq = NULL;
    pEvtSubsReq = (ClEvtSubscribeEventRequestT*)clHeapCalloc(1, sizeof(ClEvtSubscribeEventRequestT));
    if( NULL == pEvtSubsReq )
    {
        clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clHeapCalloc()");
        return CL_EVENTS_RC(CL_ERR_NO_MEMORY);
    }    
    rc = VDECL_VER(clXdrUnmarshallClEvtSubscribeEventRequestT, 4, 0, 0)(msg,pEvtSubsReq);
    if( CL_OK != rc )
    {        
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"SubscribeEventRequestT: rc[0x %x]", rc);
        return rc;
    }   
    clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"\n Simulator UnSubscribe via Request for Channel[%*s] \n",
			pEvtSubsReq->evtChannelName.length,pEvtSubsReq->evtChannelName.value);
    if(pEvtSubsReq->externalAddress==nodeLeave)
    {
        clLogDebug("CKPT", "CLEAN", "unsubscriber all channel of node [%d]",pEvtSubsReq->externalAddress);
        ClEvtUnsubscribeEventRequestT unsubsReq;
        memset(&unsubsReq, 0, sizeof(ClEvtUnsubscribeEventRequestT));
        unsubsReq.evtChannelHandle = pEvtSubsReq->evtChannelHandle;
        unsubsReq.userId = pEvtSubsReq->userId;
        unsubsReq.subscriptionId = pEvtSubsReq->subscriptionId;
        unsubsReq.reqFlag = CL_EVT_UNSUBSCRIBE;
        clEvtUtilsNameCpy(&unsubsReq.evtChannelName,
                      &pEvtSubsReq->evtChannelName);

        rc = clEvtEventCleanupViaRequest(&unsubsReq, CL_EVT_NORMAL_REQUEST);
        if (CL_OK != rc)
        {
            clLogError(EVENT_LOG_AREA_EVENT,CL_LOG_CONTEXT_UNSPECIFIED,
                   "\nUnsubscribe via Request for Channel[%*s]"
                   "Failed [%x]\n",
                   unsubsReq.evtChannelName.length,
                   unsubsReq.evtChannelName.value, rc);
            CL_FUNC_EXIT();
            return rc;
        }
    }
    
    return rc;
}    

//***************************** Delete channel open checkpoint *********************************
// Remove channel open checpoint section when channel close
//**********************************************************************************************
ClRcT clEvtChannelOpenCkptDelete(ClEvtUnsubscribeEventRequestT  *pEvtUnsubsReq)
{
	ClRcT  rc = CL_OK;
    ClCkptSectionIdT                  secId                   = {0};
    secId.idLen = pEvtUnsubsReq->evtChannelName.length;
    secId.id    = (ClUint8T*)clHeapCalloc(secId.idLen, sizeof(ClCharT));
    if( NULL == secId.id )
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clHeapCalloc()");
        return CL_EVENTS_RC(CL_ERR_NO_MEMORY);
    }    
    memcpy(secId.id , pEvtUnsubsReq->evtChannelName.value,pEvtUnsubsReq->evtChannelName.length);
    
    if( (0 != (&secId)->idLen) && (NULL != (&secId)->id) )
    {
        rc = clCkptSectionDelete(ckpt_channel_open_handle,&secId); 
        if( CL_OK != rc )
        {
            if(CL_GET_ERROR_CODE(rc) == CL_ERR_NOT_EXIST) 
            {
                clLogDebug("INFO", "SYNCUP","Section already deleted.");
                rc = CL_OK;     
            }
            else
            {
            	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clCkptSectionDelete(): rc[0x %x]", rc);  
                return rc;
            }
    
        }
    }
    return rc;
}

//***************************** Delete channel open checkpoint *********************************
// Remove channel sub checpoint section when channel un subscribe
//**********************************************************************************************
ClRcT clEvtChannelSubCkptDelete(ClEvtUnsubscribeEventRequestT  *pEvtUnsubsReq)
{
	ClRcT  rc = CL_OK;
    ClCkptSectionIdT                  secId                   = {0};
    secId.idLen = pEvtUnsubsReq->evtChannelName.length;
    secId.id    = (ClUint8T*)clHeapCalloc(secId.idLen, sizeof(ClCharT));
    if( NULL == secId.id )
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clHeapCalloc()");
        return CL_EVENTS_RC(CL_ERR_NO_MEMORY);
    }    
    memcpy(secId.id , pEvtUnsubsReq->evtChannelName.value,pEvtUnsubsReq->evtChannelName.length);
    
    if( (0 != (&secId)->idLen) && (NULL != (&secId)->id) )
    {
       rc = clCkptSectionDelete(ckpt_channel_sub_handle,&secId); 
       if( CL_OK != rc )
       {
           if(CL_GET_ERROR_CODE(rc) == CL_ERR_NOT_EXIST) 
           {
               clLogDebug("INFO", "SYNCUP","Section already deleted.");
               rc = CL_OK;     
           }
           else
           {
        	   clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clCkptSectionDelete(): rc[0x %x]", rc);
               return rc;
           }
       }
    }
    return rc;
}

//***************************** Delete channel open checkpoint *********************************
// Remove user info checpoint section when finalize event
//**********************************************************************************************
ClRcT clEvtUserInfoCkptDelete(ClEvtUnsubscribeEventRequestT  *pEvtUnsubsReq)
{
	ClRcT  rc = CL_OK;
    ClCkptSectionIdT                  secId                   = {0};    
    secId.idLen = 16;
    secId.id    = (ClUint8T*)clHeapCalloc(secId.idLen, sizeof(ClCharT));
    if( NULL == secId.id )
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clHeapCalloc()");
        return CL_EVENTS_RC(CL_ERR_NO_MEMORY);
    }
    ClUint64T section_index = pEvtUnsubsReq->userId.evtHandle; /*our section name apparently*/    
    char buff[21];
    sprintf(buff, "%llu", section_index);
    memcpy(secId.id,buff,16);  
    clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clEvtCkptGlobalCheckPointUserInfo : userId.evtHandle: [%s]", buff);    
    if( (0 != (&secId)->idLen) && (NULL != (&secId)->id) )
    {
       rc = clCkptSectionDelete(ckpt_user_info_handle,&secId); 
       if( CL_OK != rc )
       {
           if(CL_GET_ERROR_CODE(rc) == CL_ERR_NOT_EXIST) 
           {
               clLogDebug("INFO", "SYNCUP","Section already deleted.");
               rc = CL_OK;     
           }
           else
           {
        	   clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clCkptSectionDelete(): rc[0x %x]", rc);
               return rc;
           }
       }
    }
    return rc;
}

//***************************** Recovery channel open Entry *********************************

ClRcT
clChannelOpenEntryRecover(ClCkptSectionDescriptorT  *pSecDescriptor)
{
    ClRcT                   rc     = CL_OK;
    ClCkptIOVectorElementT  ioVector       = {{0}};
    ClUint32T               errIndex       = 0;
    ClBufferHandleT        msg            = CL_HANDLE_INVALID_VALUE;
    
    ioVector.sectionId  = pSecDescriptor->sectionId; 
    ioVector.dataBuffer = NULL;
    ioVector.dataSize   = 0;
    ioVector.readSize   = 0;
    ioVector.dataOffset = 0;
	clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clChannelOpenEntryRecover(): Enter");
    rc = clCkptCheckpointRead(ckpt_channel_open_handle, &ioVector, 1, &errIndex);
    if( CL_OK != rc )
    {
    	 clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clCkptCheckpointRead(): rc[0x %x]", rc);
        return rc;
    }

    if(ioVector.readSize <= 0)
    {
    	 clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"Global stream  ChannelOpen recovery failed because of inconsistent ckpt "
                            "data of size [%lld]",
                            ioVector.readSize);
        clHeapFree(ioVector.dataBuffer);
        return CL_EVENTS_RC(CL_ERR_INVALID_STATE);
    }
    rc = clBufferCreate(&msg);
    if( CL_OK != rc )
    {
    	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clBufferCreate()");
        return rc;
    }    
    rc = clBufferNBytesWrite(msg, (ClUint8T *)ioVector.dataBuffer, ioVector.readSize);
    if( CL_OK != rc )
    {
    	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clBufferNBytesWrite(): rc[0x %x]", rc);
        clBufferDelete(&msg);
        return rc;
    }    
    //simulate re initital
    clEventChannelOpenEntryUnpackNAdd(msg);
    clHeapFree(ioVector.dataBuffer);
	clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clChannelOpenEntryRecover(): End");
    return rc;
} 

//***************************** Recovery channel sub Entry *********************************
ClRcT
clChannelSubEntryRecover(ClCkptSectionDescriptorT  *pSecDescriptor)
{
    ClRcT                   rc     = CL_OK;
    ClCkptIOVectorElementT  ioVector       = {{0}};
    ClUint32T               errIndex       = 0;
    ClBufferHandleT        msg            = CL_HANDLE_INVALID_VALUE;
    
    ioVector.sectionId  = pSecDescriptor->sectionId; 
    ioVector.dataBuffer = NULL;
    ioVector.dataSize   = 0;
    ioVector.readSize   = 0;
    ioVector.dataOffset = 0;
	clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clChannelSubEntryRecover(): Enter");

    rc = clCkptCheckpointRead(ckpt_channel_sub_handle, &ioVector, 1, &errIndex);
    if( CL_OK != rc )
    {
    	 clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clCkptCheckpointRead(): rc[0x %x]", rc);
        return rc;
    }
    if(ioVector.readSize <= 0)
    {
    	 clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"Global stream ChannelSub recovery failed because of inconsistent ckpt "
                            "data of size [%lld]",
                            ioVector.readSize);
        clHeapFree(ioVector.dataBuffer);
        return CL_EVENTS_RC(CL_ERR_INVALID_STATE);
    }
    rc = clBufferCreate(&msg);
    if( CL_OK != rc )
    {
    	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clBufferCreate()");
        return rc;
    }    
    rc = clBufferNBytesWrite(msg, (ClUint8T *)ioVector.dataBuffer, ioVector.readSize);
    if( CL_OK != rc )
    {
    	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clBufferNBytesWrite(): rc[0x %x]", rc);
        clBufferDelete(&msg);
        return rc;
    }  
    //simulate re sub
    clEventChannelSubEntryUnpackNAdd(msg);
    clHeapFree(ioVector.dataBuffer);
	clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clChannelSubEntryRecover(): End");

    return rc;
} 
ClRcT
clChannelSubEntryClean(ClCkptSectionDescriptorT  *pSecDescriptor,ClIocNodeAddressT  nodeLeave)
{
    ClRcT                   rc     = CL_OK;
    ClCkptIOVectorElementT  ioVector       = {{0}};
    ClUint32T               errIndex       = 0;
    ClBufferHandleT        msg            = CL_HANDLE_INVALID_VALUE;
    
    ioVector.sectionId  = pSecDescriptor->sectionId; 
    ioVector.dataBuffer = NULL;
    ioVector.dataSize   = 0;
    ioVector.readSize   = 0;
    ioVector.dataOffset = 0;
    rc = clCkptCheckpointRead(ckpt_channel_sub_handle, &ioVector, 1, &errIndex);
    if( CL_OK != rc )
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clCkptCheckpointRead(): rc[0x %x]", rc);
        return rc;
    }
    if(ioVector.readSize <= 0)
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"Global stream ChannelSub Clean failed because of inconsistent ckpt "
                            "data of size [%lld]",
                            ioVector.readSize);
        clHeapFree(ioVector.dataBuffer);
        return CL_EVENTS_RC(CL_ERR_INVALID_STATE);
    }
    rc = clBufferCreate(&msg);
    if( CL_OK != rc )
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clBufferCreate()");
        return rc;
    }    
    rc = clBufferNBytesWrite(msg, (ClUint8T *)ioVector.dataBuffer, ioVector.readSize);
    if( CL_OK != rc )
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clBufferNBytesWrite(): rc[0x %x]", rc);
        clBufferDelete(&msg);
        return rc;
    }  
    //simulate re sub
    clEventChannelSubEntryClean(msg,nodeLeave);
    clHeapFree(ioVector.dataBuffer);
    return rc;
} 

//***************************** Recovery User Info Entry *********************************
ClRcT
clUserInfoEntryRecover(ClCkptSectionDescriptorT  *pSecDescriptor)
{
    ClRcT                   rc     = CL_OK;
    ClCkptIOVectorElementT  ioVector       = {{0}};
    ClUint32T               errIndex       = 0;
    ClBufferHandleT        msg            = CL_HANDLE_INVALID_VALUE;
	clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clUserInfoEntryRecover(): Enter for [%s] ",pSecDescriptor->sectionId.id);
    ioVector.sectionId  = pSecDescriptor->sectionId; 
    ioVector.dataBuffer = NULL;
    ioVector.dataSize   = 0;
    ioVector.readSize   = 0;
    ioVector.dataOffset = 0;
    rc = clCkptCheckpointRead(ckpt_user_info_handle, &ioVector, 1, &errIndex);
    if( CL_OK != rc )
    {
    	 clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clCkptCheckpointRead(): rc[0x %x]", rc);
        return rc;
    }

    if(ioVector.readSize <= 0)
    {
    	 clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"Global stream User Info recovery failed because of inconsistent ckpt "
                            "data of size [%lld]",
                            ioVector.readSize);
        clHeapFree(ioVector.dataBuffer);
        return CL_EVENTS_RC(CL_ERR_INVALID_STATE);
    }
    rc = clBufferCreate(&msg);
    if( CL_OK != rc )
    {
    	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clBufferCreate()");
        return rc;
    }    
    rc = clBufferNBytesWrite(msg, (ClUint8T *)ioVector.dataBuffer, ioVector.readSize);
    if( CL_OK != rc )
    {
    	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clBufferNBytesWrite(): rc[0x %x]", rc);
        clBufferDelete(&msg);
        return rc;
    }  
    //simulate re initital
    clEventUserInfoEntryUnpackNAdd(msg);
    clHeapFree(ioVector.dataBuffer);
	clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clUserInfoEntryRecover(): End");
    return rc;
} 



//***************************** Recovery channel Open state *********************************

ClRcT
clEventChannelOpenRecover(ClBoolT switchover)
{
    ClRcT                     rc            = CL_OK;
    ClHandleT                 hSecIter      = CL_HANDLE_INVALID_VALUE;
    ClCkptSectionDescriptorT  secDescriptor = {{0}};
//    ClIocNodeAddressT         localAddr     = 0;
//
//
//    localAddr = clIocLocalAddressGet();
//    if(switchover)
//    {
//        rc = clCkptActiveReplicaSetSwitchOver(ckpt_channel_open_handle);
//    }
//    else
//    {
//        rc = clCkptActiveReplicaSet(ckpt_channel_open_handle);
//    }
//
//    if (CL_OK != rc)
//    {
//    	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clCkptActiveReplicaSet(): rc[%#x], switchover flag [%d]", rc, switchover);
//        return rc;
//    }
    rc = clCkptSectionIterationInitialize(ckpt_channel_open_handle,
                                          CL_CKPT_SECTIONS_ANY, 
                                          CL_TIME_END, &hSecIter);
    if( CL_OK != rc )
    {
    	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clCkptSectionIterationInitialize(): rc[0x %x]",
                            rc);
        return rc;
    }    
    do
    {    
        rc = clCkptSectionIterationNext(hSecIter, &secDescriptor);
        if( (rc != CL_OK))
        {
            if( CL_ERR_NOT_EXIST == CL_GET_ERROR_CODE(rc)
                ||
                CL_CKPT_ERR_NO_SECTIONS == CL_GET_ERROR_CODE(rc))
            {
                rc = CL_OK;
            }
        	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clCkptSectionIterationNext(): rc[0x %x]",
                                rc);
            break;
        }
        /* 
         * Create the entries as many as we can, so explicitly
         * not checking the rc.
         */
        clChannelOpenEntryRecover(&secDescriptor);
        clHeapFree(secDescriptor.sectionId.id);
    }while((rc == CL_OK));

    CL_EVENT_CLEANUP(clCkptSectionIterationFinalize(hSecIter), CL_OK);

    return rc;
}

//***************************** Recovery Channel Subscribe Entry *********************************

ClRcT
clEventChannelSubRecover(ClBoolT switchover)
{
    ClRcT                     rc            = CL_OK;
    ClHandleT                 hSecIter      = CL_HANDLE_INVALID_VALUE;
    ClCkptSectionDescriptorT  secDescriptor = {{0}};    
//    ClIocNodeAddressT         localAddr     = 0;
//    localAddr = clIocLocalAddressGet();
//    if(switchover)
//    {
//        rc = clCkptActiveReplicaSetSwitchOver(ckpt_channel_sub_handle);
//    }
//    else
//    {
//        rc = clCkptActiveReplicaSet(ckpt_channel_sub_handle);
//    }
//
//    if (CL_OK != rc)
//    {
//    	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clCkptActiveReplicaSet(): rc[%#x], switchover flag [%d]", rc, switchover);
//        return rc;
//    }
    rc = clCkptSectionIterationInitialize(ckpt_channel_sub_handle,
                                          CL_CKPT_SECTIONS_ANY, 
                                          CL_TIME_END, &hSecIter);
    if( CL_OK != rc )
    {
    	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clCkptSectionIterationInitialize(): rc[0x %x]",
                            rc);
        return rc;
    }    
    do
    {    
        rc = clCkptSectionIterationNext(hSecIter, &secDescriptor);
        if( (rc != CL_OK))
        {
            if( CL_ERR_NOT_EXIST == CL_GET_ERROR_CODE(rc)
                ||
                CL_CKPT_ERR_NO_SECTIONS == CL_GET_ERROR_CODE(rc))
            {
            	
                rc = CL_OK;
            }
        	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clCkptSectionIterationNext(): rc[0x %x]",
                                rc);
            break;
        }
        /* 
         * Create the entries as many as we can, so explicitly
         * not checking the rc.
         */
        clChannelSubEntryRecover(&secDescriptor);        
        clHeapFree(secDescriptor.sectionId.id);
    }while((rc == CL_OK));
    CL_EVENT_CLEANUP(clCkptSectionIterationFinalize(hSecIter), CL_OK);

    return rc;
}

//***************************** Recovery User Info Entry *********************************

ClRcT
clEventUserInfoRecover(ClBoolT switchover)
{
    ClRcT                     rc            = CL_OK;
    ClHandleT                 hSecIter      = CL_HANDLE_INVALID_VALUE;
    ClCkptSectionDescriptorT  secDescriptor = {{0}};    
    rc = clCkptSectionIterationInitialize(ckpt_user_info_handle,
                                          CL_CKPT_SECTIONS_ANY, 
                                          CL_TIME_END, &hSecIter);
    if( CL_OK != rc )
    {
    	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clCkptSectionIterationInitialize(): rc[0x %x]",
                            rc);
        return rc;
    }    
    do
    {    
        rc = clCkptSectionIterationNext(hSecIter, &secDescriptor);
        if( (rc != CL_OK))
        {
            if( CL_ERR_NOT_EXIST == CL_GET_ERROR_CODE(rc)
                ||
                CL_CKPT_ERR_NO_SECTIONS == CL_GET_ERROR_CODE(rc))
            {

                rc = CL_OK;
            }
        	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clCkptSectionIterationNext(): rc[0x %x]",
                                rc);
            break;
        }
        /* 
         * Create the entries as many as we can, so explicitly
         * not checking the rc.
         */
        clUserInfoEntryRecover(&secDescriptor);        
        clHeapFree(secDescriptor.sectionId.id);
    }while((rc == CL_OK));

    CL_EVENT_CLEANUP(clCkptSectionIterationFinalize(hSecIter), CL_OK);
    return rc;
}

ClRcT
clEventExternalRecoverState(ClBoolT switchover)
{
	clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clEventExternalRecoverState(): UserInfo Recover");
	clEventUserInfoRecover(CL_TRUE);
	clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clEventExternalRecoverState(): UserInfo Recover");
	clEventChannelOpenRecover(CL_TRUE);
	clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clEventExternalRecoverState(): UserInfo Recover");
	clEventChannelSubRecover(CL_TRUE);
	return CL_TRUE;
}

ClGmsHandleT  hGms = CL_HANDLE_INVALID_VALUE;

ClGmsCallbacksT eventGmsCallbacks = {
                               NULL,
                               (ClGmsClusterTrackCallbackT) clEventTrackCallback,
                               NULL,
                               NULL,
                               };




static ClRcT
clEventAddrUpdate(ClIocNodeAddressT  leader, 
                ClIocNodeAddressT  deputy)
{
    ClRcT                  rc                = CL_OK;
    ClIocNodeAddressT      localAddr         = CL_IOC_RESERVED_ADDRESS;


    localAddr = clIocLocalAddressGet();
    if( CL_IOC_RESERVED_ADDRESS == localAddr )
    {
    	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clIocLocalAddressGet() failed");
        return CL_EVENTS_RC(CL_ERR_TRY_AGAIN);
    }
    
    if(( currentNodeLeader != leader) && (localAddr == leader) )
	{
		currentNodeLeader = leader;
		clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"...................clEventAddrUpdate() : leader change to me \n");
    	rc = clCkptActiveReplicaSet(ckpt_user_info_handle);	
    	rc = clCkptActiveReplicaSet(ckpt_channel_open_handle);	
    	rc = clCkptActiveReplicaSet(ckpt_channel_sub_handle);
	    rc = clEventExternalRecoverState(CL_TRUE);
		return rc;
	}
	clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"...................clEventAddrUpdate(): update leader \n");            
	currentNodeLeader = leader;
    return rc;
}

ClRcT
clEventCleanAllChannel(ClIocNodeAddressT  nodeLeave)
{
    ClRcT                  rc                = CL_OK;
    ClHandleT                 hSecIter      = CL_HANDLE_INVALID_VALUE;
    ClCkptSectionDescriptorT  secDescriptor[10];    
    ClUint16T count;
    count=0;
    rc = clCkptSectionIterationInitialize(ckpt_channel_sub_handle,
                                          CL_CKPT_SECTIONS_ANY, 
                                          CL_TIME_END, &hSecIter);
    if( CL_OK != rc )
    {
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clCkptSectionIterationInitialize(): rc[0x %x]",
                            rc);
        return rc;
    }       
    do
    {    
        rc = clCkptSectionIterationNext(hSecIter, &secDescriptor[count]);
        if( (rc != CL_OK))
        {
            if( CL_ERR_NOT_EXIST == CL_GET_ERROR_CODE(rc)
                ||
                CL_CKPT_ERR_NO_SECTIONS == CL_GET_ERROR_CODE(rc))
            {
            	
                rc = CL_OK;
            }
            clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clCkptSectionIterationNext(): rc[0x %x]",rc);
            break;
        }
        count++;
        /* 
         * Create the entries as many as we can, so explicitly
         * not checking the rc.
         */
    }while((rc == CL_OK));    
    CL_EVENT_CLEANUP(clCkptSectionIterationFinalize(hSecIter), CL_OK);
    clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"Clean [%d] Channel Subscriber", count);            
    for(int i=0 ; i<count; i++)
    {
        clChannelSubEntryClean(&secDescriptor[i],nodeLeave);        
        clHeapFree(secDescriptor[i].sectionId.id);
    }
    return rc;
}

static 
void clEventTrackCallback(ClGmsClusterNotificationBufferT *notificationBuffer,
                        ClUint32T                       numberOfMembers,
                        ClRcT                           rc)
{
    clLogDebug("CKPT", "CLEAN", "clEventTrackCallback event ");
    clEventAddrUpdate(notificationBuffer->leader, notificationBuffer->deputy);
    if(clCpmIsMaster())
    {    	
    	int i;
        for (i = 0; i < numberOfMembers; i++) 
        {
            if(notificationBuffer->notification[i].clusterChange==(ClInt32T)CL_GMS_MEMBER_LEFT)
            {
                //unsubscriber all external channel of this node
                clLogDebug("CKPT", "CLEAN", "unsubscriber all channel of node [%d]",notificationBuffer->notification[i].clusterNode.nodeAddress.iocPhyAddress.nodeAddress);
                clEventCleanAllChannel((ClIocNodeAddressT)notificationBuffer->notification[i].clusterNode.nodeAddress.iocPhyAddress.nodeAddress);
            }          
        }
    }
}

ClRcT
clEventGmsInit(void)
{   
    ClRcT                            rc         = CL_OK;
    ClGmsClusterNotificationBufferT  notBuffer  = {0};
    ClVersionT                       version    = {'B', 0x1, 0x1};
    ClUint32T                        numRetries = 0;

   
    do
    {
        rc = clGmsInitialize(&hGms, &eventGmsCallbacks,
                             &version);
        if( CL_OK != rc )
        {
            usleep(100);
        }
        numRetries++;
    }while((rc != CL_OK) && 
           (CL_GET_ERROR_CODE(rc) == CL_ERR_TRY_AGAIN) && 
           (numRetries < CL_EVENT_MAX_RETRIES));

    if( rc != CL_OK )
    {
    	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clGmsInitialize(): rc[0x %x]\n", rc); 
        return rc;
    }
    rc = clGmsClusterTrack( hGms,
                            CL_GMS_TRACK_CHANGES | CL_GMS_TRACK_CURRENT, 
                            &notBuffer);
    if (CL_OK != rc)
    {
        clGmsFinalize(hGms);
        clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clGmsClusterTrack(): rc[0x %x]\n", rc);
        return rc;
    }

    if(clCpmIsMaster())
    {
    	currentNodeLeader = clIocLocalAddressGet();
        if( CL_IOC_RESERVED_ADDRESS == currentNodeLeader )
        {
         	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clIocLocalAddressGet() failed");    
        }
    	clLogDebug(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"...................clEventGmsInit(): update leader \n");            
    }
    
    return rc;

}

ClRcT
clEventGmsFinalize(void)
{
    ClRcT  rc = CL_OK;
    if( CL_OK != (rc = clGmsFinalize(hGms)) )
    {
    	clLogError(EVENT_LOG_AREA_CKPT,EVENT_LOG_GLOBAL,"clGmsFinalize(): rc[0x %x]", rc);
    }
    return rc;
}        

static ClHandleT  gIocCallbackHandle = CL_HANDLE_INVALID_VALUE;

void
clEventIocNodedownCallback(ClIocNotificationIdT eventId, 
                           ClPtrT               pArg,
                           ClIocAddressT        *pAddress)
{
    /*
     * If master node is down, ask deputy to change the master address 
     */    
    
    clLogDebug("ACT", "IOC", "Received ioc notification [%d] for node [%d], port [%d]. ",
               eventId, pAddress->iocPhyAddress.nodeAddress, pAddress->iocPhyAddress.portId);
    if( (eventId == CL_IOC_NODE_LEAVE_NOTIFICATION ||
         eventId == CL_IOC_NODE_LINK_DOWN_NOTIFICATION))
    {
        /*
         * this particular event callback will be called when the current
         * master is shutting down or killed. 
         */
        clLogNotice("EVT", "IOC"," clean all subscriber ");
        clEventCleanAllChannel(pAddress->iocPhyAddress.nodeAddress);
        /* The current deputy node will be selected as master
         * while waiting for the GMS elects new master/deputy.
         */
         return;
    }
}
ClRcT clEventIocCallbackUpdate(void)
{
    ClRcT                 rc       = CL_OK;
    ClIocPhysicalAddressT compAddr = {0};

    /* deregister the old adress */
    if(CL_HANDLE_INVALID_VALUE != gIocCallbackHandle)
    {
        return rc;
    }
    compAddr.nodeAddress = CL_IOC_BROADCAST_ADDRESS;
    compAddr.portId      = CL_IOC_CKPT_PORT;
    /* register for new address */
    clCpmNotificationCallbackInstall(compAddr, clEventIocNodedownCallback, 
                                       NULL, &gIocCallbackHandle);
    return rc;
}

#endif
