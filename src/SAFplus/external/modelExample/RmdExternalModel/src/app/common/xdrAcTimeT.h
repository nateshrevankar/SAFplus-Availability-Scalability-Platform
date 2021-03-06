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


/*********************************************************************
* ModuleName  : idl
*********************************************************************/
/*********************************************************************
* Description : This file contains the declarations for marshall 
*               and unmarshall funtions of acTimeT 
*     
* THIS FILE IS AUTO-GENERATED BY OPENCLOVIS IDE. EDIT THIS FILE AT
* YOUR OWN RISK. ANY CHANGE TO THIS FILE WILL BE OVERWRITTEN ON
* RE-GENERATION.
*     
*********************************************************************/

#ifndef _XDR_AC_TIME_T_H_
#define _XDR_AC_TIME_T_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "clXdrApi.h"



struct _acTimeT_4_0_0;

typedef struct _acTimeT_4_0_0 {
    ClUint32T    hour;
    ClUint32T    minute;
    ClUint32T    second;

}acTimeT_4_0_0;


ClRcT  clXdrMarshallacTimeT_4_0_0(void *,ClBufferHandleT , ClUint32T);

ClRcT  clXdrUnmarshallacTimeT_4_0_0(ClBufferHandleT, void *);

#define clXdrMarshallArrayacTimeT_4_0_0(pointer, multiplicity, msg, isDelete) \
clXdrMarshallArray((pointer), sizeof(acTimeT_4_0_0), (multiplicity), clXdrMarshallacTimeT_4_0_0, (msg), (isDelete))

#define clXdrUnmarshallArrayacTimeT_4_0_0(msg,pointer, multiplicity) \
clXdrUnmarshallArray((msg),(pointer), sizeof(acTimeT_4_0_0), (multiplicity), clXdrUnmarshallacTimeT_4_0_0)

#define clXdrMarshallPointeracTimeT_4_0_0(pointer, multiplicity, msg, isDelete) \
clXdrMarshallPointer((pointer), sizeof(acTimeT_4_0_0), (multiplicity), clXdrMarshallacTimeT_4_0_0, (msg), (isDelete))

#define clXdrUnmarshallPointeracTimeT_4_0_0(msg,pointer) \
clXdrUnmarshallPointer((msg),(pointer), sizeof(acTimeT_4_0_0), clXdrUnmarshallacTimeT_4_0_0)

#define clXdrMarshallPtracTimeT_4_0_0(pointer, multiplicity, msg, isDelete) \
clXdrMarshallPtr((pointer), sizeof(acTimeT_4_0_0), (multiplicity), clXdrMarshallacTimeT_4_0_0, (msg), (isDelete))

#define clXdrUnmarshallPtracTimeT_4_0_0(msg,pointer,multiplicity) \
clXdrUnmarshallPtr((msg),(pointer), sizeof(acTimeT_4_0_0),multiplicity, clXdrUnmarshallacTimeT_4_0_0)


typedef acTimeT_4_0_0 acTimeT;



#ifdef __cplusplus
}
#endif

#endif /*_XDR_AC_TIME_T_H_*/
