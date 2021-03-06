/*********************************************************************
* ModuleName  : idl
*********************************************************************/
/*********************************************************************
* Description :Client Stub routines
*     
* THIS FILE IS AUTO-GENERATED BY OPENCLOVIS IDE. EDIT THIS FILE AT
* YOUR OWN RISK. ANY CHANGE TO THIS FILE WILL BE OVERWRITTEN ON
* RE-GENERATION.
*     
*********************************************************************/
#undef __SERVER__
#define __CLIENT__

#include "alarm_clock_EOClient.h"
#include "alarm_clock_EOFuncTable.h"

ClRcT clAlarm_clock_EOClientTableRegister(ClIocPortT clientPort)
{
    ClRcT rc = CL_OK;

    rc = clEoClientTableRegister(CL_EO_CLIENT_SYM_MOD(gAspFuncTable, Alarm_clock_EO),
                                 clientPort);
    if (CL_OK != rc)
    {
        return rc;
    }

    return rc;
}

ClRcT clAlarm_clock_EOClientTableDeregister(void)
{
    return CL_OK;
}

