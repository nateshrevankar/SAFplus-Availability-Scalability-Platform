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


/*********************************************************************
* ModuleName  : idl
*********************************************************************/
/*********************************************************************
* Description : This file contains the declarations for marshall 
*               and unmarshall funtions of ClCorObjAttrWalkFilterT 
*     
* THIS FILE IS AUTO-GENERATED BY OPENCLOVIS IDE. EDIT THIS FILE AT
* YOUR OWN RISK. ANY CHANGE TO THIS FILE WILL BE OVERWRITTEN ON
* RE-GENERATION.
*     
*********************************************************************/

#ifndef _XDR_CL_COR_OBJ_ATTR_WALK_FILTER_T_H_
#define _XDR_CL_COR_OBJ_ATTR_WALK_FILTER_T_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "clXdrApi.h"

#include "clCorTxnJobStream.h"
#include "clTimerApi.h"
#include "clCorNotifyApi.h"
#include "clCorClient.h"
#include "clIocApi.h"
#include "clCorDmData.h"
#include "clCorMetaData.h"
#include "clCorSync.h"
#include "xdrClCorAttrPathT.h"
#include "xdrClCorAttrWalkOpT.h"
#include "xdrClCorAttrCmpFlagT.h"

struct _ClCorObjAttrWalkFilterT_4_0_0;


ClRcT  clXdrMarshallClCorObjAttrWalkFilterT_4_0_0(void *,ClBufferHandleT , ClUint32T);

ClRcT  clXdrUnmarshallClCorObjAttrWalkFilterT_4_0_0(ClBufferHandleT, void *);

#define clXdrMarshallArrayClCorObjAttrWalkFilterT_4_0_0(pointer, multiplicity, msg, isDelete) \
clXdrMarshallArray((pointer), sizeof(ClCorObjAttrWalkFilterT_4_0_0), (multiplicity), clXdrMarshallClCorObjAttrWalkFilterT_4_0_0, (msg), (isDelete))

#define clXdrUnmarshallArrayClCorObjAttrWalkFilterT_4_0_0(msg,pointer, multiplicity) \
clXdrUnmarshallArray((msg),(pointer), sizeof(ClCorObjAttrWalkFilterT_4_0_0), (multiplicity), clXdrUnmarshallClCorObjAttrWalkFilterT_4_0_0)

#define clXdrMarshallPointerClCorObjAttrWalkFilterT_4_0_0(pointer, multiplicity, msg, isDelete) \
clXdrMarshallPointer((pointer), sizeof(ClCorObjAttrWalkFilterT_4_0_0), (multiplicity), clXdrMarshallClCorObjAttrWalkFilterT_4_0_0, (msg), (isDelete))

#define clXdrUnmarshallPointerClCorObjAttrWalkFilterT_4_0_0(msg,pointer) \
clXdrUnmarshallPointer((msg),(pointer), sizeof(ClCorObjAttrWalkFilterT_4_0_0), clXdrUnmarshallClCorObjAttrWalkFilterT_4_0_0)

#define clXdrMarshallPtrClCorObjAttrWalkFilterT_4_0_0(pointer, multiplicity, msg, isDelete) \
clXdrMarshallPtr((pointer), sizeof(ClCorObjAttrWalkFilterT_4_0_0), (multiplicity), clXdrMarshallClCorObjAttrWalkFilterT_4_0_0, (msg), (isDelete))

#define clXdrUnmarshallPtrClCorObjAttrWalkFilterT_4_0_0(msg,pointer,multiplicity) \
clXdrUnmarshallPtr((msg),(pointer), sizeof(ClCorObjAttrWalkFilterT_4_0_0),multiplicity, clXdrUnmarshallClCorObjAttrWalkFilterT_4_0_0)


typedef ClCorObjAttrWalkFilterT ClCorObjAttrWalkFilterT_4_0_0;



#ifdef __cplusplus
}
#endif

#endif /*_XDR_CL_COR_OBJ_ATTR_WALK_FILTER_T_H_*/