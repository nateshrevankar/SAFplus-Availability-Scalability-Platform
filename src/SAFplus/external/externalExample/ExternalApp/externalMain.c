#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <clCommon.h>
#include <clOsalApi.h>
#include <clOsalErrors.h>
#include <clBufferApi.h>
#include <clCntApi.h>
#include <clHeapApi.h>
#include <clEoApi.h>
#include <clIocApi.h>
#include <clIocApiExt.h>
#include <clIocErrors.h>
#include <clIocParseConfig.h>
#include <clIocLogicalAddresses.h>
#include "rmdExternalDefs.h"
#include "clRmdIpi.h"
#include <saEvt.h>
#include <clEventApi.h>
#include <clEventExtApi.h>
#include <clLogApi.h>


// Logging utility routines are located in a separate file.
#include "log.h"
#define LOOP_COUNT 150


/* This local address must match the TIPC node address */
#define LOCAL_ADDRESS 5

//***********External***********************************
#define __LOGICAL_ADDRESS(a) CL_IOC_LOGICAL_ADDRESS_FORM(CL_IOC_STATIC_LOGICAL_ADDRESS_START + (a))
#define __RPC_SERVER_ADDRESS __LOGICAL_ADDRESS(LOCAL_ADDRESS)
extern ClRcT clRmdLibInitialize(ClPtrT pConfig);


//********************publisher********************
#define EVENT_CHANNEL_NAME_1 "TestEventChannel1"
#define PUBLISHER_NAME_1 "TestEventPublisher1"
typedef struct
{
    ClEoExecutionObjT             *tstRegEoObj;
    SaEvtHandleT		  evtInitHandle;
    SaEvtEventHandleT		  eventHandle;
    ClVersionT                    version;
    SaVersionT                    evtVersion;
    SaNameT                       evtChannelName;
    SaNameT                       publisherName;
    int                           running;
    int                           exiting;
} gTestInfoT;
SaAisErrorT openPublisherChannel();
static void testEvtMainLoop();
gTestInfoT gTestInfo;

//*************************************************


//********************subscribe *******************
#define EVENT_CHANNEL_NAME "TestEventChannel"
SaNameT                 evtChannelName;
//handle for subscribe event
SaEvtChannelHandleT   evtChannelHandle = 0;
SaEvtHandleT      evtHandle;
//handle for public event
SaEvtChannelHandleT evtChannelHandlePublic      = 0;
//************************************************/

static void appEventCallback( SaEvtSubscriptionIdT	subscriptionId, SaEvtEventHandleT     eventHandle, SaSizeT eventDataSize);


#include <clNodeCache.h>

const char* Cap2Str(ClUint32T cap)
{
    if(CL_NODE_CACHE_LEADER_CAPABILITY(cap)) return "leader";
    if (CL_NODE_CACHE_SC_CAPABILITY(cap)) return "controller";
    if (CL_NODE_CACHE_PL_CAPABILITY(cap)) return "payload";
    return "unknown";
}
void nodeCacheWait(void)
{
    unsigned int done = CL_FALSE;
    ClNodeCacheMemberT nodes[64];
    ClUint32T numNodes;
    ClIocNodeAddressT leader;
    ClTimerTimeOutT delay;
    delay.tsSec = 0;
    delay.tsMilliSec = 700;  

    // I need to wait for one SC to tell us about itself for registration and one payload to come up to receive my logs
    // This is specific to this demo.  Your application may need to wait for different nodes...
    while(done != 3)  
    {
        numNodes = 64;

        printf("Nodes in the cluster:\n");
        clNodeCacheViewGet(nodes,&numNodes);
        for (int i = 0; i<numNodes;i++)
        {
            printf("%30s: Address: %d,  Version: %x  Capability: %s\n",nodes[i].name,nodes[i].address,nodes[i].version,Cap2Str(nodes[i].capability));
            if (CL_NODE_CACHE_LEADER_CAPABILITY(nodes[i].capability)) done |=1;
            if ((nodes[i].address != LOCAL_ADDRESS) && (CL_NODE_CACHE_PL_CAPABILITY(nodes[i].capability))) done |=2;
        }
        
#if 0  // GAS why is the node cache leader not being updated?
        ClRcT rc = clNodeCacheLeaderGet(&leader);
        if (rc == CL_OK)
        {
            done = CL_TRUE;            
        }
#endif        
        
        if (done != 3) clOsalTaskDelay(delay);        
    }
    
    
}


int main(int argc, char **argv)
{

    int ioc_address_local = LOCAL_ADDRESS;
    ClRcT rc = CL_OK;
    rc = clExtInitialize(ioc_address_local);
    if (rc != CL_OK)
    {
        printf("Error: failed to Initialize SAFplus libraries\n");
        exit(1);
    }

    nodeCacheWait();

    // Demonstrate logging */
    printf("\n\nOpen a global log stream and write several records to active PY node\n");        
    if ((rc=logInitialize()) == CL_OK)
    {
        printf("log Initialized\n"); 
        logWrite(CL_LOG_SEV_NOTICE,"This is a test of an external app doing logging");
        for(int i=0;i<100;i++)
        {
            ClTimerTimeOutT delay;
            delay.tsSec = 0;
            delay.tsMilliSec = 10; 
            logWrite(CL_LOG_SEV_NOTICE,"external app log %d", i);
            clOsalTaskDelay(delay);            
        }            
    }
    else
    {
        printf("Unable to open log.  Error 0x%x\n",rc);
        return rc;
    }



    //  Open a subscribe event channel and start receiving events.
    printf("\n\nOpen an Event subscription\n");        
    const SaEvtCallbacksT evtCallbacks =
    {
        NULL,
        appEventCallback
    };
    SaVersionT  evtVersion = CL_EVENT_VERSION;
    rc = saEvtInitialize(&evtHandle, &evtCallbacks, &evtVersion);
    if (rc != SA_AIS_OK)
    {
        printf("Failed to init event mechanism [0x%x]\n",rc);
        return rc;
    }

    saNameSet(&evtChannelName,EVENT_CHANNEL_NAME);
    rc = saEvtChannelOpen(evtHandle,&evtChannelName, (SA_EVT_CHANNEL_SUBSCRIBER | SA_EVT_CHANNEL_CREATE), (SaTimeT)SA_TIME_END, &evtChannelHandle);
    if (rc != SA_AIS_OK)
    {
        printf("Failure opening event channel[0x%x] at %ld\n", rc, time(0L));
        goto errorexit;
    }
    rc = saEvtEventSubscribe(evtChannelHandle, NULL, 1);
    if (rc != SA_AIS_OK)
    {
        printf("Failed to subscribe to event channel [0x%x]\n", rc);
        goto errorexit;
    }
    //open a global log stream and write several records
        
    printf("Open a publisher event channel\n");        
    // open a publisher event channel and 
    openPublisherChannel();
    printf("Start publishing events.\nThe PY component on active PY node subscribes to these events and logs them so you can verify receipt by looking in the clock.log.latest and app.lates  on node active PY node .\n");        
    testEvtMainLoop();
    printf("Unsubscribe event chanel.............................\n");        
    rc = saEvtEventUnsubscribe(evtChannelHandle,1);
    if (rc != SA_AIS_OK) 
        printf("Channel unsubscribe result: %d\n", rc);
    printf("Close subscribe event chanel.............................\n");        
    rc = saEvtChannelClose(evtChannelHandle);
    if (rc != SA_AIS_OK) 
        printf("Channel close result: %d\n", rc);
    printf("Close publish event channel.............................\n");        
    rc = saEvtChannelClose(evtChannelHandlePublic);
    if (rc != SA_AIS_OK) 
        printf("Channel close result: %d\n", rc);
    // never inited: printf("Finalize publish event handle.............................\n");        
    //    saEvtFinalize(gTestInfo.evtInitHandle);
    printf("Finalize xubscribe event handle.............................\n");        
    saEvtFinalize(evtHandle);
    openPublisherChannel();
    testEvtMainLoop();
    printf("Close publish event channel.............................\n");        
    rc = saEvtChannelClose(evtChannelHandlePublic);
    if (rc != SA_AIS_OK) 
        printf("Channel close result: %d\n", rc);
    // never inited printf("Finalize publish event handle.............................\n");        
    //   saEvtFinalize(gTestInfo.evtInitHandle);
    
    return 0;
errorexit:
    printf ("Initialization error [0x%x]\n",rc);
}


static void
generate_time_of_day(char **data, ClSizeT *data_len)
{
    time_t t;

    // minimal error checking
    if (data == 0 || data_len == 0)
    {
        printf("generate_time_of_day passed null pointer\n");
        return;
    }

    // magic number, but well, that's what ctime_r needs
    *data_len = 26;
    *data = (char*)clHeapAllocate(*data_len);
    if (*data == 0)
    {
        *data_len = 0;
        return;
    }
    time(&t);
    ctime_r(&t, *data);
    *(*data + 24) = 0;
    (*data_len) -= 1;
    return;
}

static void
generate_load_average(char **data, ClSizeT *data_len)
{
    int fd;
    char *tmp_ptr;
    char buf[500];                  //insane over doing it
    ssize_t num_read;

    // minimal error checking
    if (data == 0 || data_len == 0)
    {
        printf("generate_load_average passed null pointer\n ");
        return;
    }
    if ((fd = open("/proc/loadavg", O_RDONLY, 0)) == -1)
    {
        printf( "failed to open /proc/loadavg\n");
        return;
    }
    num_read = read(fd, buf, sizeof buf);
    if (num_read == 0 || num_read == -1)
    {
        printf( "bogus result from read of loadavg\n");
        return;
    }
    close(fd);
    *data_len = num_read + 1;
    *data = (char*)clHeapAllocate(*data_len);
    if (data == 0)
    {
        printf(
                "failed to allocate memory for loadavg contents\n");
        *data_len = 0;
        close(fd);
        return;
    }
    *(*data + (*data_len) - 1) = 0;    
    strncpy(*data, buf, *data_len);
    tmp_ptr = strchr(*data, ' ');
    if (tmp_ptr == 0)
    {
        return;
    }
    tmp_ptr = strchr(tmp_ptr + 1, ' ');
    if (tmp_ptr == 0)
    {
        return;
    }
    tmp_ptr = strchr(tmp_ptr + 1, ' ');
    if (tmp_ptr == 0)
    {
        return;
    }
    *tmp_ptr = 0;
    return;
}
static ClRcT
appPublishEvent()
{
    ClEventIdT      eventId         = 0;
    static int      index           = 0;
    SaSizeT         data_len        = 0;
    SaAisErrorT	    saRc = SA_AIS_OK;
    char            *data           = 0;
    typedef void (*Generator)(char **, ClSizeT*);

    //
    // Note: to add a new generator, just define it above and then include
    // the new functions name in the generators list.
    // Next, maybe something that gets disk free info by way of getfsent
    // and statfs?
    static Generator generators[]   =
    {
        generate_time_of_day,
        generate_load_average
    };

    //
    // every time through increment index and then set index to
    // it's value modulo the number of entries in the generators
    // array.  This will cause us to cycle through the list of
    // generators as we're called to publish events.
    (*generators[index++])(&data, &data_len);
    index %= (int)(sizeof generators / sizeof generators[0]);
    if (data == 0 || data_len == 0)
    {
        printf("no event data generated\n");
        return CL_ERR_NO_MEMORY;
    }
    printf("Publishing Event: %.*s\n", (int)data_len, data);
    saRc = saEvtEventPublish(gTestInfo.eventHandle, (void *)data, data_len, &eventId);
    clHeapFree(data);

    return CL_OK;
}

static void testEvtMainLoop()
{
    /* Main loop: Keep printing and publishing unless we are suspended */
    int i=0;
    SaAisErrorT  saRc = SA_AIS_OK;
    for(int i=0; i< LOOP_COUNT; i++)
    {
        //appPublishEvent();   
        ClEventIdT      eventId         = 0;
        printf("Publishing Event\n");  
        ClCharT buff[40] = {0};
        memset(buff,0,sizeof(buff));
        sprintf(buff,"Event from external application_%d",i);
        saRc = saEvtEventPublish(gTestInfo.eventHandle, (const void*)buff, strlen(buff), &eventId);   
        ClTimerTimeOutT delay;
        delay.tsSec = 0;
        delay.tsMilliSec = 200;
        clOsalTaskDelay(delay);
    }
}    

static void appEventCallback( SaEvtSubscriptionIdT	subscriptionId, SaEvtEventHandleT     eventHandle, SaSizeT eventDataSize)
{
    SaAisErrorT  saRc = SA_AIS_OK;
    static ClPtrT   resTest = 0;
    static ClSizeT  resSize = 0;
    if (resTest != 0)
    {
        // Maybe try to save the previously allocated buffer if it's big
        // enough to hold the new event message.
        clHeapFree((char *)resTest);
        resTest = 0;
        resSize = 0;
    }
    resTest = clHeapAllocate(eventDataSize + 1);
    if (resTest == 0)
    {
        printf("Failed to allocate space for event\n");
        return;
    }
    *(((char *)resTest) + eventDataSize) = 0;
    resSize = eventDataSize;
    saRc = saEvtEventDataGet(eventHandle, resTest, &resSize);
    if (saRc!= SA_AIS_OK)
    {
        printf("Failed to get event data [0x%x]\n",saRc);
    }
    printf ("Received event from internal node: %s\n", (char *)resTest);
    return;
}

SaAisErrorT openPublisherChannel()
{
    SaAisErrorT  rc = SA_AIS_OK;
    gTestInfo.tstRegEoObj      = 0;
  	gTestInfo.evtInitHandle    = 0;
  	gTestInfo.eventHandle      = 0;
    gTestInfo.version.releaseCode                    = 'B';
    gTestInfo.version.majorVersion                   = 01;
    gTestInfo.version.minorVersion                   = 01;
    gTestInfo.evtVersion.releaseCode                    = 'B';
    gTestInfo.evtVersion.majorVersion                   = 01;
    gTestInfo.evtVersion.minorVersion                   = 01;
    saNameSet(&gTestInfo.evtChannelName,EVENT_CHANNEL_NAME_1);
    saNameSet(&gTestInfo.publisherName,PUBLISHER_NAME_1);
    gTestInfo.running          = 1;
    gTestInfo.exiting          = 0;
    SaEvtCallbacksT     evtCallbacks          = {NULL, NULL};
    printf("Initial event.................... \n");
    rc = saEvtInitialize(&gTestInfo.evtInitHandle,
                         &evtCallbacks,
                         &gTestInfo.evtVersion);
    if (rc != SA_AIS_OK)
    {
        printf( "Failed to init event system[0x%x]\n",rc);
        return rc;
    }

    printf("Opening event publisher to channel [%s]\n",gTestInfo.evtChannelName.value);
    rc = saEvtChannelOpen (gTestInfo.evtInitHandle,
                           &gTestInfo.evtChannelName,
                           (SA_EVT_CHANNEL_PUBLISHER | SA_EVT_CHANNEL_CREATE),
                           (ClTimeT)SA_TIME_END,
                           &evtChannelHandlePublic);
    if (rc != SA_AIS_OK)
    {
        printf( "Failed to open event channel [0x%x]\n",rc);
        return rc;
    }

    rc = saEvtEventAllocate(evtChannelHandlePublic, &gTestInfo.eventHandle);
    if (rc != SA_AIS_OK)
    {
        printf( "Failed to cllocate event [0x%x]\n",rc);
        return rc;
    }

    rc = saEvtEventAttributesSet(gTestInfo.eventHandle,
                                 NULL,
                                 1,
                                 0,
                                 &gTestInfo.publisherName);
    if (rc != SA_AIS_OK)
    {
        printf( "Failed to set event attributes [0x%x]\n",rc);
        return rc;
    }
    return rc;
}

