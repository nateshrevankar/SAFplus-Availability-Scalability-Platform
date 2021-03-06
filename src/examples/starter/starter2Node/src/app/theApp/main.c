/*
 * Copyright (C) 2002-2013 OpenClovis Solutions Inc.  All Rights Reserved.
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
 * This file is autogenerated by OpenClovis IDE, 
 */

/******************************************************************************
 * Include files needed to compile this file
 *****************************************************************************/

/* POSIX Includes */
#include <assert.h>
#include <errno.h>

/* Basic SAFplus Includes */
#include <clCommon.h>

/* SAFplus Client Includes */
#include <clLogApi.h>
#include <clCpmApi.h>
#include <saAmf.h>

#include <sys/socket.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/socket.h>       /* for AF_INET */
#include <netdb.h>

/* Local function declarations */

/* The application should fill these functions */
void safTerminate(SaInvocationT invocation, const SaNameT *compName);
void safAssignWork(SaInvocationT       invocation,
                   const SaNameT       *compName,
                   SaAmfHAStateT       haState,
                   SaAmfCSIDescriptorT csiDescriptor);
void safRemoveWork(SaInvocationT  invocation,
                   const SaNameT  *compName,
                   const SaNameT  *csiName,
                   SaAmfCSIFlagsT csiFlags);

/* Utility functions */
void initializeAmf(void);
void dispatchLoop(void);
void printCSI(SaAmfCSIDescriptorT csiDescriptor, SaAmfHAStateT haState);
int  errorExit(SaAisErrorT rc);


/******************************************************************************
 * Optional Features
 *****************************************************************************/

/* A wrapper for our logging. You can change "MAI" and CL_LOG_CONTEXT_UNSPECIFIED
   to another 3 letter acronym meaningful more meaningful to this module.
 */

#define clprintf(severity, ...)   clAppLog(CL_LOG_HANDLE_APP, severity, 10, "MAI", CL_LOG_CONTEXT_UNSPECIFIED,__VA_ARGS__)

/******************************************************************************
 * Global Variables.
 *****************************************************************************/

/* The process ID is stored in this variable.  This is only used in our logging. */
pid_t        mypid;

/* Access to the SAF AMF framework occurs through this handle */
SaAmfHandleT amfHandle;

/* This process's SAF name */
SaNameT      appName = {0};


ClBoolT unblockNow = CL_FALSE;

/* Declare other global variables here. */

enum
  {
  VipFieldLen = 64
  };

typedef struct
{
  char ip[VipFieldLen];
  char dev[VipFieldLen];
  char netmask[VipFieldLen];
} VirtualIpAddress;

VirtualIpAddress gVirtualIp;

/* From the RFC
    Ethernet packet data:
        16.bit: (ar$hrd) Hardware address space (e.g., Ethernet,
                         Packet Radio Net.)
        16.bit: (ar$pro) Protocol address space.  For Ethernet
                         hardware, this is from the set of type
                         fields ether_typ$<protocol>.
         8.bit: (ar$hln) byte length of each hardware address
         8.bit: (ar$pln) byte length of each protocol address
        16.bit: (ar$op)  opcode (ares_op$REQUEST | ares_op$REPLY)
        nbytes: (ar$sha) Hardware address of sender of this
                         packet, n from the ar$hln field.
        mbytes: (ar$spa) Protocol address of sender of this
                         packet, m from the ar$pln field.
        nbytes: (ar$tha) Hardware address of target of this
                         packet (if known).
        mbytes: (ar$tpa) Protocol address of target.
*/

typedef struct
{
  ClUint8T  dstMac[6];
  ClUint8T  myMac[6];
  ClUint16T type;

  ClUint16T hrd;
  ClUint16T pro;
  ClUint8T  hln;
  ClUint8T  pln;
  ClUint16T op;
  ClUint8T  sha[6];
  ClUint8T  spa[4];
  ClUint8T  tha[6];
  ClUint8T  tpa[4];
} EthIpv4ArpPacket;

enum
  {
    ArpRequest = 1,
    ArpReply   = 2,
    ArpHwTypeEthernet = 1,

    IpProtoType = 0x0800,
    //ArpAddressResolutionType = 
    MacAddrLen  = 6
  };

ClRcT DevToMac(const char* dev, char mac[6])
{
  struct ifreq req;

  int sd = socket(AF_INET, SOCK_STREAM, 0);

  memset(&req, 0, sizeof(struct ifreq));
  strcpy(req.ifr_name, dev);
  req.ifr_addr.sa_family = AF_UNSPEC;
  ioctl(sd, SIOCGIFHWADDR, &req);
  memcpy(mac, &(req.ifr_addr.sa_data), 6); 

  close(sd);
  return CL_OK;
}

ClRcT HostToIp(const char* myHost, unsigned int* ip)
{
  struct hostent* host = NULL;
  struct hostent hostdata;
  char buf[128];
  int errnum = 0;
  
  if (gethostbyname_r(myHost,&hostdata,buf,128,&host,&errnum))
    {
      return CL_ERR_LIBRARY;
    }
 
  *ip = *((unsigned int*) host->h_addr_list[0]);
  return CL_OK; 
}

ClRcT SendArp(const char* host, const char* dev)
{
  int i;
  char myMac[MacAddrLen];
  unsigned int myIp = 0;
  EthIpv4ArpPacket pkt;
 
  DevToMac(dev,myMac);
  HostToIp(host,&myIp);

  for (i = ArpRequest; i<= ArpRequest; i++)
    {

      memcpy(pkt.myMac, myMac, MacAddrLen);
      memset(pkt.dstMac, 0xFF, MacAddrLen);
      pkt.type = htons(ETHERTYPE_ARP);

      pkt.hrd = htons(ArpHwTypeEthernet);
      pkt.pro = htons(IpProtoType); //htons(ETHERTYPE_IP); //htons(ETH_P_IP); //ArpAddressResolutionType;
      pkt.hln = MacAddrLen;  /* ETH_HW_ADDR_LEN; Length in bytes of ethernet address */
      pkt.pln = 4; // IP_ADDR_LEN;
      pkt.op  = htons(i);     // htons(ARPOP_REQUEST);

      memcpy(pkt.sha,myMac,MacAddrLen);
      memset(pkt.tha, 0xFF, MacAddrLen);
      memcpy(pkt.spa,&myIp,4);
      memcpy(pkt.tpa,&myIp,4);

      if (1)
        {
          int             sd;
          struct sockaddr_ll sal; 

          bzero(&sal,sizeof(sal)); 
          sal.sll_family    = AF_PACKET; 
          sal.sll_protocol  = htons(ETH_P_ARP); 
          sal.sll_ifindex   = 2; 
          sal.sll_hatype    = htons(i); 
          sal.sll_pkttype   = PACKET_BROADCAST; 
          memcpy(sal.sll_addr, myMac, MacAddrLen); 
          sal.sll_halen     = MacAddrLen;

          if ((sd = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ARP))) < 0) 
            { perror("socket"); return CL_ERR_LIBRARY; }

          if (sendto(sd,&pkt,sizeof(pkt),0,(struct sockaddr *)&sal,sizeof(sal)) < 0)
            { 
              perror("sendto"); 
              return CL_ERR_LIBRARY; 
            }
          close(sd);
        }
    }
  return CL_OK;
}

void GetVirtualAddressInfo(SaAmfCSIDescriptorT* csiDescriptor, VirtualIpAddress* vip)
{
    int i;
    vip->ip[0] = 0;
    vip->netmask[0] = 0;
    vip->dev[0] = 0;

    /* Pull the appropriate values out of the workload dictionary */
    for (i = 0; i < csiDescriptor->csiAttr.number; i++)
    {
        if (strcmp((const char*) csiDescriptor->csiAttr.attr[i].attrName,"VirtualIpAddress") == 0)
            strncpy(vip->ip, (const char*) csiDescriptor->csiAttr.attr[i].attrValue, VipFieldLen);
        if (strcmp((const char*) csiDescriptor->csiAttr.attr[i].attrName,"VirtualNetMask") == 0)
            strncpy(vip->netmask, (const char*) csiDescriptor->csiAttr.attr[i].attrValue,VipFieldLen);
        if (strcmp((const char*) csiDescriptor->csiAttr.attr[i].attrName,"VirtualDevice") == 0)
            strncpy(vip->dev, (const char*) csiDescriptor->csiAttr.attr[i].attrValue,VipFieldLen);
    }

}

void AddRemVirtualAddress(const char *cmd,const VirtualIpAddress* vip)
{
  if (vip->ip && vip->dev && vip->netmask)
    {
      char exec[301] = { 0 };
      snprintf(exec, 300,"%s/virtualIp %s %s %s %s ",CL_APP_BINDIR, cmd,vip->ip,vip->netmask,vip->dev);
      __attribute__((unused)) ClUint32T rc = system(exec);

      if (cmd[0] == 'u')  /* If we are coming up, do a gratuitous arp */
        SendArp(vip->ip,vip->dev);
    }
}


/******************************************************************************
 * Application Life Cycle Management Functions
 *****************************************************************************/


int main(int argc, char *argv[])
{
    SaAisErrorT rc = SA_AIS_OK;

    /* Connect to the SAF cluster */
    initializeAmf();

    /* Do the application specific initialization here. */
    
    /* Block on AMF dispatch file descriptor for callbacks.
       When this function returns its time to quit. */
    dispatchLoop();
    
    /* Do the application specific finalization here. */

    /* Now finalize my connection with the SAF cluster */
    if((rc = saAmfFinalize(amfHandle)) != SA_AIS_OK)
      clprintf (CL_LOG_SEV_ERROR, "AMF finalization error[0x%X]", rc);
    else
      clprintf (CL_LOG_SEV_INFO, "AMF Finalized");   

    return 0;
}


/*
 * clCompAppTerminate
 * ------------------
 * This function is invoked when the application is to be terminated.
 */

void safTerminate(SaInvocationT invocation, const SaNameT *compName)
{
    SaAisErrorT rc = SA_AIS_OK;

    clprintf (CL_LOG_SEV_INFO, "Component [%.*s] : PID [%d]. Terminating\n", compName->length, compName->value, mypid);

    
    /*
     * Unregister with AMF and respond to AMF saying whether the
     * termination was successful or not.
     */
    if ( (rc = saAmfComponentUnregister(amfHandle, compName, NULL)) != SA_AIS_OK)
    {
        clprintf (CL_LOG_SEV_ERROR, "Component [%.*s] : PID [%d]. Unregister failed with error [0x%x]\n",
                  compName->length, compName->value, mypid, rc);
        return;        
    }

    /* Ok tell SAFplus that we handled it properly */
    saAmfResponse(amfHandle, invocation, SA_AIS_OK);

    clprintf (CL_LOG_SEV_INFO, "Component [%.*s] : PID [%d]. Terminated\n",
              compName->length, compName->value, mypid);

    unblockNow = CL_TRUE;
}

/******************************************************************************
 * Application Work Assignment Functions
 *****************************************************************************/

/*
 * safAssignWork
 * ------------------
 * This function is invoked when a CSI assignment is made or the state
 * of a CSI is changed.
 */

void safAssignWork(SaInvocationT       invocation,
                   const SaNameT       *compName,
                   SaAmfHAStateT       haState,
                   SaAmfCSIDescriptorT csiDescriptor)
{
    /* Print information about the CSI Set */

    clprintf (CL_LOG_SEV_INFO, "Component [%.*s] : PID [%d]. CSI Set Received\n", 
              compName->length, compName->value, mypid);

    printCSI(csiDescriptor, haState);

    /*
     * Take appropriate action based on state
     */

    switch ( haState )
    {
        /* AMF asks this process to take the standby HA state for the work
           described in the csiDescriptor variable */
        case SA_AMF_HA_ACTIVE:
        {
            /* Typically you would spawn a thread here to initiate active 
               processing of the work. */

            if ((csiDescriptor.csiFlags == SA_AMF_CSI_TARGET_ALL )||(csiDescriptor.csiFlags == SA_AMF_CSI_TARGET_ONE))
              {
                /* Do not reload the virtual address info for the "TARGET" flag -- the CSI is not filled in.
                   We will use the values that were cached when the "standby" assignment occurred.
                 */
              }
            else /* SA_AMF_CSI_ADD_ONE  */
              {
                GetVirtualAddressInfo(&csiDescriptor, &gVirtualIp);
              }


            AddRemVirtualAddress("up",&gVirtualIp);


            /* The AMF times the interval between the assignment and acceptance
               of the work (the time interval is configurable).
               So it is important to call this saAmfResponse function ASAP.
             */
            saAmfResponse(amfHandle, invocation, SA_AIS_OK);
            break;
        }

        /* AMF asks this process to take the standby HA state for the work
           described in the csiDescriptor variable */
        case SA_AMF_HA_STANDBY:
        {
            /* If your standby has ongoing maintenance, you would spawn a thread
               here to do it. */
            GetVirtualAddressInfo(&csiDescriptor, &gVirtualIp);
            AddRemVirtualAddress("down",&gVirtualIp); 

            /* The AMF times the interval between the assignment and acceptance
               of the work (the time interval is configurable).
               So it is important to call this saAmfResponse function ASAP.
             */
            saAmfResponse(amfHandle, invocation, SA_AIS_OK);  
            break;
        }

        case SA_AMF_HA_QUIESCED:
        {
            /*
             * AMF has requested application to quiesce the CSI currently
             * assigned the active or quiescing HA state. The application 
             * must stop work associated with the CSI immediately.
             */
            if ((csiDescriptor.csiFlags == SA_AMF_CSI_TARGET_ALL)||(csiDescriptor.csiFlags == SA_AMF_CSI_TARGET_ONE))
            {
                AddRemVirtualAddress("down",&gVirtualIp);           
            }
            else /* SA_AMF_CSI_ADD_ONE */
            {
                VirtualIpAddress temp;
                GetVirtualAddressInfo(&csiDescriptor, &temp);
                AddRemVirtualAddress("down",&temp);
            }


            saAmfResponse(amfHandle, invocation, SA_AIS_OK);
            break;
        }

        case SA_AMF_HA_QUIESCING:
        {
            /*
             * AMF has requested application to quiesce the CSI currently
             * assigned the active HA state. The application must stop work
             * associated with the CSI gracefully and not accept any new
             * workloads while the work is being terminated.
             */

            /* There are two typical cases for quiescing.  Chooose one!
               CASE 1: Its possible to quiesce rapidly within this thread context */
            if (1)
              {
              /* App code here: Now finish your work and cleanly stop the work*/
            
              /* Call saAmfCSIQuiescingComplete when stopping the work is done */
              saAmfCSIQuiescingComplete(amfHandle, invocation, SA_AIS_OK);
              }
            else
              {
              /* CASE 2: You can't quiesce within this thread context or quiesce
               rapidly. */

              /* Respond immediately to the quiescing request */
              saAmfResponse(amfHandle, invocation, SA_AIS_OK);

              /* App code here: Signal or spawn a thread to cleanly stop the work*/
              /* When that thread is done, it should call:
                 saAmfCSIQuiescingComplete(amfHandle, invocation, SA_AIS_OK);
              */
              }

            break;
        }

        default:
        {
            assert(0);
            break;
        }
    }

    return;
}

/*
 * safRemoveWork
 * ---------------------
 * This function is invoked when a CSI assignment is to be removed.
 */

void safRemoveWork(SaInvocationT  invocation,
                   const SaNameT  *compName,
                   const SaNameT  *csiName,
                   SaAmfCSIFlagsT csiFlags)
{
    clprintf (CL_LOG_SEV_INFO, "Component [%.*s] : PID [%d]. CSI Remove Received\n", 
              compName->length, compName->value, mypid);

    clprintf (CL_LOG_SEV_INFO, "   CSI                     : %.*s\n", csiName->length, csiName->value);
    clprintf (CL_LOG_SEV_INFO, "   CSI Flags               : 0x%d\n", csiFlags);

    /*
     * Add application specific logic for removing the work for this CSI.
     */
    AddRemVirtualAddress("down",&gVirtualIp);

    saAmfResponse(amfHandle, invocation, SA_AIS_OK);
}

/******************************************************************************
 * Utility functions 
 *****************************************************************************/
/* This simple helper function just prints an error and quits */
int errorExit(SaAisErrorT rc)
{        
    clprintf (CL_LOG_SEV_ERROR, "Component [%.*s] : PID [%d]. Initialization error [0x%x]\n",
              appName.length, appName.value, mypid, rc);
    exit(-1);
    return -1;
}

void initializeAmf(void)
{
    SaAmfCallbacksT     callbacks;
    SaVersionT          version;
    ClIocPortT          iocPort;
    SaAisErrorT         rc = SA_AIS_OK;

    /* Get the pid for the process and store it in global variable. */
    mypid = getpid();

    /* SAFplus is fully API compatible with SA-Forum (SAF) definitions.

       This optional call customizes OpenClovis SAFplus Platform extensions
       to the basic SAF services (to use, you would define the parameters as globals).  
       
       If this call is removed, standard SAF services will work just fine. */

    /* clAppConfigure(&clEoConfig,clEoBasicLibs,clEoClientLibs); */

    
    /*
     * Initialize and register with SAFplus AMF. 'version' specifies the
     * version of AMF with which this application would like to
     * interface. 'callbacks' is used to register the callbacks this
     * component expects to receive.
     */
    version.releaseCode  = 'B';
    version.majorVersion = 01;
    version.minorVersion = 01;
    
    callbacks.saAmfHealthcheckCallback          = NULL; /* rarely necessary because SAFplus monitors the process */
    callbacks.saAmfComponentTerminateCallback   = safTerminate;
    callbacks.saAmfCSISetCallback               = safAssignWork;
    callbacks.saAmfCSIRemoveCallback            = safRemoveWork;
    callbacks.saAmfProtectionGroupTrackCallback = NULL;
        
    /* Initialize AMF client library. */
    if ( (rc = saAmfInitialize(&amfHandle, &callbacks, &version)) != SA_AIS_OK)
        errorExit(rc);

    /*
     * Now register the component with AMF. At this point it is
     * ready to provide service, i.e. take work assignments.
     */

    if ( (rc = saAmfComponentNameGet(amfHandle, &appName)) != SA_AIS_OK) 
        errorExit(rc);
    if ( (rc = saAmfComponentRegister(amfHandle, &appName, NULL)) != SA_AIS_OK) 
        errorExit(rc);

    /*
     * Print out standard information for this component.
     */

    clEoMyEoIocPortGet(&iocPort);
    
    clprintf (CL_LOG_SEV_INFO, "Component [%.*s] : PID [%d]. Initializing\n", appName.length, appName.value, mypid);
    clprintf (CL_LOG_SEV_INFO, "   IOC Address             : 0x%x\n", clIocLocalAddressGet());
    clprintf (CL_LOG_SEV_INFO, "   IOC Port                : 0x%x\n", iocPort);
}

void dispatchLoop(void)
{        
  SaAisErrorT         rc = SA_AIS_OK;
  SaSelectionObjectT amf_dispatch_fd;
  int maxFd;
  fd_set read_fds;

  /* This boilerplate code includes an example of how to simultaneously
     dispatch for 2 services (in this case AMF and CKPT).  But since checkpoint
     is not initialized or used, it is commented out */
  /* SaSelectionObjectT ckpt_dispatch_fd; */

  /*
   * Get the AMF dispatch FD for the callbacks
   */
  if ( (rc = saAmfSelectionObjectGet(amfHandle, &amf_dispatch_fd)) != SA_AIS_OK)
    errorExit(rc);
  /* if ( (rc = saCkptSelectionObjectGet(ckptLibraryHandle, &ckpt_dispatch_fd)) != SA_AIS_OK)
       errorExit(rc); */
    
  maxFd = amf_dispatch_fd;  /* maxFd = max(amf_dispatch_fd,ckpt_dispatch_fd); */
  do
    {
      FD_ZERO(&read_fds);
      FD_SET(amf_dispatch_fd, &read_fds);
      /* FD_SET(ckpt_dispatch_fd, &read_fds); */
        
      if( select(maxFd + 1, &read_fds, NULL, NULL, NULL) < 0)
        {
          char errorStr[80];
          int err = errno;
          if (EINTR == err) continue;

          errorStr[0] = 0; /* just in case strerror does not fill it in */
          strerror_r(err, errorStr, 79);
          clprintf (CL_LOG_SEV_ERROR, "Error [%d] during dispatch loop select() call: [%s]",err,errorStr);
          break;
        }
      if (FD_ISSET(amf_dispatch_fd,&read_fds)) saAmfDispatch(amfHandle, SA_DISPATCH_ALL);
      /* if (FD_ISSET(ckpt_dispatch_fd,&read_fds)) saCkptDispatch(ckptLibraryHandle, SA_DISPATCH_ALL); */
    }while(!unblockNow);      
}


#define STRING_HA_STATE(S)                                                  \
(   ((S) == SA_AMF_HA_ACTIVE)             ? "Active" :                \
    ((S) == SA_AMF_HA_STANDBY)            ? "Standby" :               \
    ((S) == SA_AMF_HA_QUIESCED)           ? "Quiesced" :              \
    ((S) == SA_AMF_HA_QUIESCING)          ? "Quiescing" :             \
                                            "Unknown" )

#define STRING_CSI_FLAGS(S)                                                 \
(   ((S) == SA_AMF_CSI_ADD_ONE)            ? "Add One" :               \
    ((S) == SA_AMF_CSI_TARGET_ONE)         ? "Target One" :            \
    ((S) == SA_AMF_CSI_TARGET_ALL)         ? "Target All" :            \
                                                  "Unknown" )

/*
 * printCSI
 * --------------------
 * Print information received in a CSI set request.
 */

void printCSI(SaAmfCSIDescriptorT csiDescriptor, SaAmfHAStateT haState)
{
    clprintf (CL_LOG_SEV_INFO, "CSI Flags : [%s]", STRING_CSI_FLAGS(csiDescriptor.csiFlags));

    if (SA_AMF_CSI_TARGET_ALL != csiDescriptor.csiFlags)
    {
        clprintf (CL_LOG_SEV_INFO, "CSI Name : [%s]", csiDescriptor.csiName.value);
    }

    if (SA_AMF_CSI_ADD_ONE == csiDescriptor.csiFlags)
    {
        ClUint32T i = 0;
        
        clprintf (CL_LOG_SEV_INFO, "Name value pairs :");
        for (i = 0; i < csiDescriptor.csiAttr.number; i++)
        {
            clprintf (CL_LOG_SEV_INFO, "Name : [%s]",
                      csiDescriptor.csiAttr.
                      attr[i].attrName);
            clprintf (CL_LOG_SEV_INFO, "Value : [%s]",
                      csiDescriptor.csiAttr.
                      attr[i].attrValue);
        }
    }
    
    clprintf (CL_LOG_SEV_INFO, "HA state : [%s]", STRING_HA_STATE(haState));

    if (SA_AMF_HA_ACTIVE == haState)
    {
        clprintf (CL_LOG_SEV_INFO, "Active Descriptor :");
        clprintf (CL_LOG_SEV_INFO,
                  "Transition Descriptor : [%d]",
                  csiDescriptor.csiStateDescriptor.
                  activeDescriptor.transitionDescriptor);
        clprintf (CL_LOG_SEV_INFO,
                  "Active Component : [%s]",
                  csiDescriptor.csiStateDescriptor.
                  activeDescriptor.activeCompName.value);
    }
    else if (SA_AMF_HA_STANDBY == haState)
    {
        clprintf (CL_LOG_SEV_INFO, "Standby Descriptor :");
        clprintf (CL_LOG_SEV_INFO,
                  "Standby Rank : [%d]",
                  csiDescriptor.csiStateDescriptor.
                  standbyDescriptor.standbyRank);
        clprintf (CL_LOG_SEV_INFO, "Active Component : [%s]",
                  csiDescriptor.csiStateDescriptor.
                  standbyDescriptor.activeCompName.value);
    }
}


