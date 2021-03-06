#include <clAppSubAgentConfig.h>
#include <clSnmpocTrainInstXlation.h>
#include <clSnmpocTrainUtil.h>
#include <clSnmpocTrainNotifications.h>
                                                                                                                  
                                                                                                                             
/*
 * This structure contains information regarding oid of the table and the 
 * type of index. The index  can  either be  a string value or a non-string value.
 * Any SNMP type apart from string is treated as non-string.
 */

ClSnmpOidInfoT appOidInfo[] =
{
	{    CL_OCTRAIN_SCALARS, "1.3.6.1.4.1.103.1.1",
		1 /* no. of indexes */,
        {
	        { 0, 0 },
            
        },
        NULL, clSnmpocTrainDefaultInstXlator, clSnmpocTrainDefaultInstCompare
    },
	{    CL_OCTRAIN_SCALARS, "1.3.6.1.4.1.103.1.2",
		1 /* no. of indexes */,
        {
	        { 0, 0 },
            
        },
        NULL, clSnmpocTrainDefaultInstXlator, clSnmpocTrainDefaultInstCompare
    },
	{    CL_CLOCKTABLE, "1.3.6.1.4.1.103.2.1",
		1 /* no. of indexes */,
        {
	        { CL_SNMP_NON_STRING_ATTR, 4 },
            
        },
        NULL, clSnmpclockTableInstXlator, clSnmpclockTableInstCompare
    },
	{    CL_TIMESETTABLE, "1.3.6.1.4.1.103.3.1",
		1 /* no. of indexes */,
        {
	        { CL_SNMP_NON_STRING_ATTR, 4 },
            
        },
        NULL, clSnmptimeSetTableInstXlator, clSnmptimeSetTableInstCompare
    },
	{    CL_NAMETABLE, "1.3.6.1.4.1.103.4.1",
		1 /* no. of indexes */,
        {
	        { CL_SNMP_NON_STRING_ATTR, 4 },
            
        },
        clSnmpnameTableIndexCorAttrGet, clSnmpnameTableInstXlator, clSnmpnameTableInstCompare
    },
    
    {0, NULL, 0, {{0,0}}, NULL, NULL}
};

/*
 * This provides mapping between trap oid and the notification call back function 
 * generated by mib2c. When an alarm is raised this mapping is used to call the
 * corresponding notification function.
 */
ClSnmpNtfyCallbackTableT clSnmpAppCallback[]= {
    {".1.3.6.1.4.1.103.5.1", clSnmpalarmTrapIndexGet, clSnmpSendalarmTrapTrap},

    {NULL, NULL, NULL}
};

void init_appMIB()
{
    init_ocTrainMIB();

}
