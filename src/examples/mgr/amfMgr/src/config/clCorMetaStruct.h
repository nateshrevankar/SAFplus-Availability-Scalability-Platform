/*******************************************************************************
*
* This file is auto-generated by OpenClovis IDE version 3.1
*
* /home/hoangle/git/SAFplus-Availability-Scalability-Platform/src/examples/mgr/ide_workspace/amfMgr/.ngc/config/clCorMetaStruct.h
* 
*
********************************************************************************/
#ifndef _CL_COR_META_STRUCT_H_
#define _CL_COR_META_STRUCT_H_

#ifdef __cplusplus
extern "C" {
#endif

enum ENUM_classes
{
    CLASS_RESERVED=2,
        CLASS_CHASSIS_MO ,
    CLASS_SAAMFSCALARS_MO ,
    CLASS_SAAMFSCALARS_PROV_MSO ,
    CLASS_SAAMFAPPLICATIONTABLE_MO ,
    CLASS_SAAMFAPPLICATIONTABLE_PROV_MSO ,
    CLASS_SAAMFNODETABLE_MO ,
    CLASS_SAAMFNODETABLE_PROV_MSO ,
    CLASS_SAAMFSGTABLE_MO ,
    CLASS_SAAMFSGTABLE_PROV_MSO ,
    CLASS_SAAMFSUTABLE_MO ,
    CLASS_SAAMFSUTABLE_PROV_MSO ,
    CLASS_SAAMFSITABLE_MO ,
    CLASS_SAAMFSITABLE_PROV_MSO ,
    CLASS_SAAMFSUSPERSIRANKTABLE_MO ,
    CLASS_SAAMFSUSPERSIRANKTABLE_PROV_MSO ,
    CLASS_SAAMFSGSIRANKTABLE_MO ,
    CLASS_SAAMFSGSIRANKTABLE_PROV_MSO ,
    CLASS_SAAMFSGSURANKTABLE_MO ,
    CLASS_SAAMFSGSURANKTABLE_PROV_MSO ,
    CLASS_SAAMFSISIDEPTABLE_MO ,
    CLASS_SAAMFSISIDEPTABLE_PROV_MSO ,
    CLASS_SAAMFCOMPTABLE_MO ,
    CLASS_SAAMFCOMPTABLE_PROV_MSO ,
    CLASS_SAAMFCOMPCSTYPESUPPORTEDTABLE_MO ,
    CLASS_SAAMFCOMPCSTYPESUPPORTEDTABLE_PROV_MSO ,
    CLASS_SAAMFCSITABLE_MO ,
    CLASS_SAAMFCSITABLE_PROV_MSO ,
    CLASS_SAAMFCSICSIDEPTABLE_MO ,
    CLASS_SAAMFCSICSIDEPTABLE_PROV_MSO ,
    CLASS_SAAMFCSINAMEVALUETABLE_MO ,
    CLASS_SAAMFCSINAMEVALUETABLE_PROV_MSO ,
    CLASS_SAAMFCSTYPEATTRNAMETABLE_MO ,
    CLASS_SAAMFCSTYPEATTRNAMETABLE_PROV_MSO ,
    CLASS_SAAMFSUSITABLE_MO ,
    CLASS_SAAMFSUSITABLE_PROV_MSO ,
    CLASS_SAAMFHEALTHCHECKTABLE_MO ,
    CLASS_SAAMFHEALTHCHECKTABLE_PROV_MSO ,
    CLASS_SAAMFSCOMPCSITABLE_MO ,
    CLASS_SAAMFSCOMPCSITABLE_PROV_MSO ,
    CLASS_SAAMFPROXYPROXIEDTABLE_MO ,
    CLASS_SAAMFPROXYPROXIEDTABLE_PROV_MSO ,
    CLASS_SAAMFTRAPOBJECT_MO ,
    CLASS_SAAMFTRAPOBJECT_PROV_MSO ,
    CLASS_SAAMFTRAPOBJECT_ALARM_MSO ,
    CLASS_X86_64_MO ,

    COR_CLASS_MAX
};
enum ENUM_attributes
{
    ATTRIBUTE_RESERVED = 1,
    SAAMFSCALARS_SAAMFAGENTSPECVERSION,
    SAAMFSCALARS_SAAMFAGENTVENDOR,
    SAAMFSCALARS_SAAMFAGENTVENDORPRODUCTREV,
    SAAMFSCALARS_SAAMFSERVICESTARTENABLED,
    SAAMFSCALARS_SAAMFSERVICESTATE,
    SAAMFSCALARS_SAAMFCLUSTERSTARTUPTIMEOUT,
    SAAMFSCALARS_SAAMFCLUSTERADMINSTATE,
    SAAMFSCALARS_SAAMFCLUSTERADMINSTATETRIGGER,
    SAAMFAPPLICATIONTABLE_SAAMFAPPLICATIONNAMEID,
    SAAMFAPPLICATIONTABLE_SAAMFAPPLICATIONNAME,
    SAAMFAPPLICATIONTABLE_SAAMFAPPLICATIONADMINSTATE,
    SAAMFAPPLICATIONTABLE_SAAMFAPPLICATIONCURRNUMSG,
    SAAMFAPPLICATIONTABLE_SAAMFAPPLICATIONADMINSTATETRIGGER,
    SAAMFAPPLICATIONTABLE_SAAMFAPPLICATIONRESTARTSTATE,
    SAAMFAPPLICATIONTABLE_SAAMFAPPLICATIONROWSTATUS,
    SAAMFNODETABLE_SAAMFNODENAMEID,
    SAAMFNODETABLE_SAAMFNODENAME,
    SAAMFNODETABLE_SAAMFNODECLMNODE,
    SAAMFNODETABLE_SAAMFNODESUFAILOVERPROB,
    SAAMFNODETABLE_SAAMFNODESUFAILOVERMAX,
    SAAMFNODETABLE_SAAMFNODEADMINSTATE,
    SAAMFNODETABLE_SAAMFNODEOPERATIONALSTATE,
    SAAMFNODETABLE_SAAMFNODEAUTOREPAIROPTION,
    SAAMFNODETABLE_SAAMFNODEREBOOTONTERMINATIONFAIL,
    SAAMFNODETABLE_SAAMFNODEREBOOTONINSTANTIATIONFAIL,
    SAAMFNODETABLE_SAAMFNODEADMINSTATETRIGGER,
    SAAMFNODETABLE_SAAMFNODERESTARTSTATE,
    SAAMFNODETABLE_SAAMFNODEREPAIR,
    SAAMFNODETABLE_SAAMFNODEROWSTATUS,
    SAAMFSGTABLE_SAAMFSGNAMEID,
    SAAMFSGTABLE_SAAMFSGNAME,
    SAAMFSGTABLE_SAAMFSGREDMODEL,
    SAAMFSGTABLE_SAAMFSGNUMPREFACTIVESUS,
    SAAMFSGTABLE_SAAMFSGNUMPREFSTANDBYSUS,
    SAAMFSGTABLE_SAAMFSGNUMPREFINSERVICESUS,
    SAAMFSGTABLE_SAAMFSGNUMPREFASSIGNEDSUS,
    SAAMFSGTABLE_SAAMFSGMAXACTIVESISPERSU,
    SAAMFSGTABLE_SAAMFSGMAXSTANDBYSISPERSU,
    SAAMFSGTABLE_SAAMFSGADMINSTATE,
    SAAMFSGTABLE_SAAMFSGCOMPRESTARTPROB,
    SAAMFSGTABLE_SAAMFSGCOMPRESTARTMAX,
    SAAMFSGTABLE_SAAMFSGSURESTARTPROB,
    SAAMFSGTABLE_SAAMFSGSURESTARTMAX,
    SAAMFSGTABLE_SAAMFSGAUTOREPAIROPTION,
    SAAMFSGTABLE_SAAMFSGNUMCURRASSIGNEDSU,
    SAAMFSGTABLE_SAAMFSGNUMCURRNONINSTANTIATEDSPARESU,
    SAAMFSGTABLE_SAAMFSGNUMCURRINSTANTIATEDSPARESU,
    SAAMFSGTABLE_SAAMFSGAUTOADJUSTOPTION,
    SAAMFSGTABLE_SAAMFSGAUTOADJUSTPROB,
    SAAMFSGTABLE_SAAMFSGADJUSTSTATE,
    SAAMFSGTABLE_SAAMFSGADMINSTATETRIGGER,
    SAAMFSGTABLE_SAAMFSGROWSTATUS,
    SAAMFSUTABLE_SAAMFSUNAMEID,
    SAAMFSUTABLE_SAAMFSUNAME,
    SAAMFSUTABLE_SAAMFSURANK,
    SAAMFSUTABLE_SAAMFSUNUMCOMPONENTS,
    SAAMFSUTABLE_SAAMFSUNUMCURRACTIVESIS,
    SAAMFSUTABLE_SAAMFSUNUMCURRSTANDBYSIS,
    SAAMFSUTABLE_SAAMFSUADMINSTATE,
    SAAMFSUTABLE_SAAMFSUFAILOVER,
    SAAMFSUTABLE_SAAMFSUREADINESSSTATE,
    SAAMFSUTABLE_SAAMFSUOPERSTATE,
    SAAMFSUTABLE_SAAMFSUPRESENCESTATE,
    SAAMFSUTABLE_SAAMFSUPREINSTANTIABLE,
    SAAMFSUTABLE_SAAMFSUISEXTERNAL,
    SAAMFSUTABLE_SAAMFSUHOSTEDBYNODE,
    SAAMFSUTABLE_SAAMFSUADMINSTATETRIGGER,
    SAAMFSUTABLE_SAAMFSURESTARTSTATE,
    SAAMFSUTABLE_SAAMFSUEAMTRIGGER,
    SAAMFSUTABLE_SAAMFSUREPAIR,
    SAAMFSUTABLE_SAAMFSUROWSTATUS,
    SAAMFSITABLE_SAAMFSINAMEID,
    SAAMFSITABLE_SAAMFSINAME,
    SAAMFSITABLE_SAAMFSIRANK,
    SAAMFSITABLE_SAAMFSINUMCSIS,
    SAAMFSITABLE_SAAMFSIPREFNUMASSIGNMENTS,
    SAAMFSITABLE_SAAMFSIADMINSTATE,
    SAAMFSITABLE_SAAMFSIASSIGNMENTSTATE,
    SAAMFSITABLE_SAAMFSINUMCURRACTIVEASSIGNMENTS,
    SAAMFSITABLE_SAAMFSINUMCURRSTANDBYASSIGNMENTS,
    SAAMFSITABLE_SAAMFSIPROTECTEDBYSG,
    SAAMFSITABLE_SAAMFSIADMINSTATETRIGGER,
    SAAMFSITABLE_SAAMFSISWAPSTATE,
    SAAMFSITABLE_SAAMFSIROWSTATUS,
    SAAMFSUSPERSIRANKTABLE_SAAMFSUSPERSINAMEID,
    SAAMFSUSPERSIRANKTABLE_SAAMFSUSPERSIRANK,
    SAAMFSUSPERSIRANKTABLE_SAAMFSUSPERSINAME,
    SAAMFSUSPERSIRANKTABLE_SAAMFSUSPERSISUNAME,
    SAAMFSUSPERSIRANKTABLE_SAAMFSUSPERSIROWSTATUS,
    SAAMFSGSIRANKTABLE_SAAMFSGSIRANKSGNAMEID,
    SAAMFSGSIRANKTABLE_SAAMFSGSIRANK,
    SAAMFSGSIRANKTABLE_SAAMFSGSIRANKSGNAME,
    SAAMFSGSIRANKTABLE_SAAMFSGSIRANKSINAME,
    SAAMFSGSURANKTABLE_SAAMFSGSURANKSGNAMEID,
    SAAMFSGSURANKTABLE_SAAMFSGSURANK,
    SAAMFSGSURANKTABLE_SAAMFSGSURANKSGNAME,
    SAAMFSGSURANKTABLE_SAAMFSGSURANKSUNAME,
    SAAMFSISIDEPTABLE_SAAMFSISIDEPSINAMEID,
    SAAMFSISIDEPTABLE_SAAMFSISIDEPDEPNDSINAMEID,
    SAAMFSISIDEPTABLE_SAAMFSISIDEPSINAME,
    SAAMFSISIDEPTABLE_SAAMFSISIDEPDEPNDSINAME,
    SAAMFSISIDEPTABLE_SAAMFSISIDEPTOLTIME,
    SAAMFSISIDEPTABLE_SAAMFSISIDEPROWSTATUS,
    SAAMFCOMPTABLE_SAAMFCOMPNAMEID,
    SAAMFCOMPTABLE_SAAMFCOMPNAME,
    SAAMFCOMPTABLE_SAAMFCOMPCAPABILITY,
    SAAMFCOMPTABLE_SAAMFCOMPCATEGORY,
    SAAMFCOMPTABLE_SAAMFCOMPINSTANTIATECMD,
    SAAMFCOMPTABLE_SAAMFCOMPTERMINATECMD,
    SAAMFCOMPTABLE_SAAMFCOMPCLEANUPCMD,
    SAAMFCOMPTABLE_SAAMFCOMPAMSTARTCMD,
    SAAMFCOMPTABLE_SAAMFCOMPAMSTOPCMD,
    SAAMFCOMPTABLE_SAAMFCOMPINSTANTIATIONLEVEL,
    SAAMFCOMPTABLE_SAAMFCOMPDEFAULTCLCCLITIMEOUT,
    SAAMFCOMPTABLE_SAAMFCOMPDEFAULTCALLBACKTIMEOUT,
    SAAMFCOMPTABLE_SAAMFCOMPINSTANTIATETIMEOUT,
    SAAMFCOMPTABLE_SAAMFCOMPDELAYBETWEENINSTANTIATEATTEMPTS,
    SAAMFCOMPTABLE_SAAMFCOMPTERMINATETIMEOUT,
    SAAMFCOMPTABLE_SAAMFCOMPCLEANUPTIMEOUT,
    SAAMFCOMPTABLE_SAAMFCOMPAMSTARTTIMEOUT,
    SAAMFCOMPTABLE_SAAMFCOMPAMSTOPTIMEOUT,
    SAAMFCOMPTABLE_SAAMFCOMPTERMINATECALLBACKTIMEOUT,
    SAAMFCOMPTABLE_SAAMFCOMPCSISETCALLBACKTIMEOUT,
    SAAMFCOMPTABLE_SAAMFCOMPQUIESCINGCOMPLETETIMEOUT,
    SAAMFCOMPTABLE_SAAMFCOMPCSIRMVCALLBACKTIMEOUT,
    SAAMFCOMPTABLE_SAAMFCOMPRECOVERYONERROR,
    SAAMFCOMPTABLE_SAAMFCOMPNUMMAXINSTANTIATEWITHOUTDELAY,
    SAAMFCOMPTABLE_SAAMFCOMPNUMMAXINSTANTIATEWITHDELAY,
    SAAMFCOMPTABLE_SAAMFCOMPNUMMAXAMSTARTSTOPATTEMPTS,
    SAAMFCOMPTABLE_SAAMFCOMPDISABLERESTART,
    SAAMFCOMPTABLE_SAAMFCOMPNUMMAXACTIVECSI,
    SAAMFCOMPTABLE_SAAMFCOMPNUMMAXSTANDBYCSI,
    SAAMFCOMPTABLE_SAAMFCOMPNUMCURRACTIVECSI,
    SAAMFCOMPTABLE_SAAMFCOMPNUMCURRSTANDBYCSI,
    SAAMFCOMPTABLE_SAAMFCOMPRESTARTCOUNT,
    SAAMFCOMPTABLE_SAAMFCOMPOPERSTATE,
    SAAMFCOMPTABLE_SAAMFCOMPPRESENCESTATE,
    SAAMFCOMPTABLE_SAAMFCOMPREADINESSSTATE,
    SAAMFCOMPTABLE_SAAMFCOMPCURRPROXYNAME,
    SAAMFCOMPTABLE_SAAMFCOMPPROXYCSI,
    SAAMFCOMPTABLE_SAAMFCOMPRESTARTSTATE,
    SAAMFCOMPTABLE_SAAMFCOMPEAMTRIGGER,
    SAAMFCOMPTABLE_SAAMFCOMPROWSTATUS,
    SAAMFCOMPCSTYPESUPPORTEDTABLE_SAAMFCOMPCSTYPESUPPORTEDCOMPNAMEID,
    SAAMFCOMPCSTYPESUPPORTEDTABLE_SAAMFCOMPCSTYPESUPPORTEDCSTYPENAMEID,
    SAAMFCOMPCSTYPESUPPORTEDTABLE_SAAMFCOMPCSTYPESUPPORTEDCOMPNAME,
    SAAMFCOMPCSTYPESUPPORTEDTABLE_SAAMFCOMPCSTYPESUPPORTEDCSINAME,
    SAAMFCOMPCSTYPESUPPORTEDTABLE_SAAMFCOMPCSTYPESUPPORTEDROWSTATUS,
    SAAMFCSITABLE_SAAMFCSINAMEID,
    SAAMFCSITABLE_SAAMFCSINAME,
    SAAMFCSITABLE_SAAMFCSTYPE,
    SAAMFCSITABLE_SAAMFCSIROWSTATUS,
    SAAMFCSICSIDEPTABLE_SAAMFCSICSIDEPCSINAMEID,
    SAAMFCSICSIDEPTABLE_SAAMFCSICSIDEPDEPNDCSINAMEID,
    SAAMFCSICSIDEPTABLE_SAAMFCSICSIDEPCSINAME,
    SAAMFCSICSIDEPTABLE_SAAMFCSICSIDEPDEPNDCSINAME,
    SAAMFCSICSIDEPTABLE_SAAMFCSICSIDEPROWSTATUS,
    SAAMFCSINAMEVALUETABLE_SAAMFCSINAMEVALUECSINAMEID,
    SAAMFCSINAMEVALUETABLE_SAAMFCSINAMEVALUEATTRNAMEID,
    SAAMFCSINAMEVALUETABLE_SAAMFCSINAMEVALUECSINAME,
    SAAMFCSINAMEVALUETABLE_SAAMFCSINAMEVALUEATTRNAME,
    SAAMFCSINAMEVALUETABLE_SAAMFCSINAMEVALUEATTRVALUE,
    SAAMFCSINAMEVALUETABLE_SAAMFCSINAMEVALUEROWSTATUS,
    SAAMFCSTYPEATTRNAMETABLE_SAAMFCSTYPENAMEID,
    SAAMFCSTYPEATTRNAMETABLE_SAAMFCSTYPEATTRNAMEID,
    SAAMFCSTYPEATTRNAMETABLE_SAAMFCSTYPENAME,
    SAAMFCSTYPEATTRNAMETABLE_SAAMFCSTYPEATTRNAME,
    SAAMFCSTYPEATTRNAMETABLE_SAAMFCSTYPEATTRROWSTATUS,
    SAAMFSUSITABLE_SAAMFSUSISUNAMEID,
    SAAMFSUSITABLE_SAAMFSUSISINAMEID,
    SAAMFSUSITABLE_SAAMFSUSISUNAME,
    SAAMFSUSITABLE_SAAMFSUSISINAME,
    SAAMFSUSITABLE_SAAMFSUSIHASTATE,
    SAAMFHEALTHCHECKTABLE_SAAMFHEALTHCOMPNAMEID,
    SAAMFHEALTHCHECKTABLE_SAAMFHEALTHCHECKKEY,
    SAAMFHEALTHCHECKTABLE_SAAMFHEALTHCOMPNAME,
    SAAMFHEALTHCHECKTABLE_SAAMFHEALTHCHECKPERIOD,
    SAAMFHEALTHCHECKTABLE_SAAMFHEALTHCHECKMAXDURATION,
    SAAMFHEALTHCHECKTABLE_SAAMFHEALTHCHECKROWSTATUS,
    SAAMFSCOMPCSITABLE_SAAMFSCOMPCSICOMPNAMEID,
    SAAMFSCOMPCSITABLE_SAAMFSCOMPCSICSINAMEID,
    SAAMFSCOMPCSITABLE_SAAMFSCOMPCSICOMPNAME,
    SAAMFSCOMPCSITABLE_SAAMFSCOMPCSICSINAME,
    SAAMFSCOMPCSITABLE_SAAMFSCOMPCSIHASTATE,
    SAAMFPROXYPROXIEDTABLE_SAAMFPROXYPROXIEDPROXYNAMEID,
    SAAMFPROXYPROXIEDTABLE_SAAMFPROXYPROXIEDPROXIEDNAMEID,
    SAAMFPROXYPROXIEDTABLE_SAAMFPROXYPROXIEDPROXYNAME,
    SAAMFPROXYPROXIEDTABLE_SAAMFPROXYPROXIEDPROXIEDNAME,
    SAAMFTRAPOBJECT_SAAMFSVCUSERNAME,
    SAAMFTRAPOBJECT_SAAMFPROBABLECAUSE,

    COR_ATTRIBUTES_MAX
};

#ifdef __cplusplus
}
#endif

#endif
