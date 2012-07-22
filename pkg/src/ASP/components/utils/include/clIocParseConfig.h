/*
 * Copyright (C) 2002-2009 by OpenClovis Inc. All  Rights Reserved.
 * 
 * The source code for  this program is not published  or otherwise 
 * divested of  its trade secrets, irrespective  of  what  has been 
 * deposited with the U.S. Copyright office.
 * 
 * This program is  free software; you can redistribute it and / or
 * modify  it under  the  terms  of  the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 * 
 * This program is distributed in the  hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied  warranty  of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 * 
 * You  should  have  received  a  copy of  the  GNU General Public
 * License along  with  this program. If  not,  write  to  the 
 * Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */
/*
 * Build: 4.2.0
 */
#ifndef _CL_IOC_PARSE_CONFIG_H_
#define _CL_IOC_PARSE_CONFIG_H_

#ifdef __cplusplus

extern "C" { 

#endif

#include <clCommon.h>
#include <clIocApi.h>
#include <clIocUdpTransportApi.h>

    typedef struct ClIocConfig
    {
        ClIocLibConfigT iocConfigInfo;
        /*Transport info*/
        ClUint32T numOfXports;
        ClIocUserTransportConfigT *transport;
        ClEoActionInfoT wmActions[CL_IOC_QUEUE_MAX];
    } ClIocConfigT;

    extern ClRcT clIocParseConfig(const ClCharT *pNodeName,ClIocConfigT **ppConfig);

#ifdef __cplusplus
}
#endif

#endif