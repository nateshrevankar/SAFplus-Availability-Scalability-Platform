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
#undef __CLIENT__
#define __SERVER__

#include <netinet/in.h>
#include <clBufferApi.h>
#include <clRmdApi.h>
#include <clEoApi.h>
#include <ipi/clRmdIpi.h>
#include <string.h>
#include "msgCltSrvClientCallsFromClientToClientServerServer.h"
#include "msgCltSrvServer.h"
#include "msgCltSrvFuncTable.h"

ClHandleDatabaseHandleT  msgCltSrvidlDatabaseHdl = 0;
ClUint32T                msgCltSrvidlSyncKey     = 0;

ClRcT clMsgCltSrvIdlSyncDefer(ClIdlHandleT *pIdlHdl)
{
    return clIdlSyncDefer(msgCltSrvidlDatabaseHdl, msgCltSrvidlSyncKey, pIdlHdl);
}    
    
ClRcT clMsgCltSrvClientInstall(void)
{
    ClEoExecutionObjT* pEoObj = NULL;
    ClRcT rc = CL_OK;

    rc = clEoMyEoObjectGet(&pEoObj);
    if (CL_OK != rc)
    {
        return rc;
    }

    rc = clEoClientInstallTables(pEoObj,CL_EO_SERVER_SYM_MOD(gAspFuncTable,MsgCltSrv));
    if (CL_OK != rc)
    {
        return rc;
    }
    
    rc = clOsalTaskKeyCreate(&msgCltSrvidlSyncKey,NULL);
    if (CL_OK != rc)
    {
        return rc;
    }

    rc = clHandleDatabaseCreate(NULL,&msgCltSrvidlDatabaseHdl);

    return rc;
}

ClRcT clMsgCltSrvClientUninstall(void)
{
    ClEoExecutionObjT* pEoObj = NULL;
    ClRcT rc = CL_OK;

    rc = clEoMyEoObjectGet(&pEoObj);
    if (CL_OK != rc)
    {
        return rc;
    }
    rc = clOsalTaskKeyDelete(msgCltSrvidlSyncKey);
    if (CL_OK != rc)
    {
        return rc;
    }
    rc = clHandleDatabaseDestroy(msgCltSrvidlDatabaseHdl);
    if (CL_OK != rc)
    {
        return rc;
    }

    clEoClientUninstallTables(pEoObj,CL_EO_SERVER_SYM_MOD(gAspFuncTable,MsgCltSrv));

    return rc;
}

    
