/******************************************************************************
*                                                                             *
*                                                                             *
* Copyright (c) SS8 Networks, Inc.                                            *
* All rights reserved.                                                        *
*                                                                             *
* This document contains confidential and proprietary information in which    *
* any reproduction, disclosure, or use in whole or in part is expressly       *
* prohibited, except as may be specifically authorized by prior written       *
* agreement or permission of SS8 Networks, Inc.                               *
*                                                                             *
*******************************************************************************
* VERSION      : $Revision: 1.1 $
* DATE         : $Date: 2008/05/16 12:24:04 $
* 
* MODULE NAME  : $RCSfile: MaintenanceTimeoutIndEvent.java,v $
* AUTHOR       : Nilgun Baykal [SS8]
* DESCRIPTION  : 
* DATE 1st REL : 
* REV.HIST.    : 
* 
* Date      Owner  Description
* ========  =====  ===========================================================
* 
* 
*******************************************************************************
*                                                                             *
*                     RESTRICTED RIGHTS LEGEND                                *
* Use, duplication, or disclosure by Government Is Subject to restrictions as *
* set forth in subparagraph (c)(1)(ii) of the Rights in Technical Data and    *
* Computer Software clause at DFARS 252.227-7013                              *
*                                                                             *
******************************************************************************/


package javax.jain.ss7.isup;

import javax.jain.*;
import javax.jain.ss7.*;

public class MaintenanceTimeoutIndEvent extends IsupEvent{

	public MaintenanceTimeoutIndEvent(java.lang.Object source,
                                  SignalingPointCode dpc,
                                  SignalingPointCode opc,
                                  byte sls,
                                  int cic,
                                  byte congestionPriority,
                                  int maintTimeoutReason)
                           throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);
		maintTmoReason = maintTimeoutReason;
	}

	public int getIsupPrimitive(){
		return IsupConstants.ISUP_PRIMITIVE_MAINT_TMO_IND;
	}

	public int getMaintenanceTimeoutReason(){
		return maintTmoReason;
	}

	public void setMaintenanceTimeoutReason(int maintTimeoutReason){
		maintTmoReason = maintTimeoutReason;
	}

	/**
    * String representation of class MaintenanceTimeoutIndEvent
    *
    * @return    String provides description of class MaintenanceTimeoutIndEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\nmaintTmoReason = ");
				buffer.append(maintTmoReason);
				return buffer.toString();
		
		}

	
	int maintTmoReason;

	public static final int ISUP_MTR_BLOCKING_TIMEOUT = 13; /*BLA Timer - T13*/
	public static final int ISUP_MTR_UNBLOCKING_TIMEOUT = 15; /*UBA timer - T15 */
	public static final int ISUP_MTR_RELEASE_TIMEOUT = 1; /* RLC timer - T1 */
	public static final int ISUP_MTR_RESET_CIRCUIT_TIMEOUT = 17; /* RSC timer - T17*/
	public static final int ISUP_MTR_GROUP_BLOCKING_TIMEOUT = 19; /* CGBA timer - T19*/
	public static final int ISUP_MTR_GROUP_UNBLOCKING_TIMEOUT = 21; /* CGUA timer - T21 */
	public static final int ISUP_MTR_GROUP_RESET_CIRCUIT_TIMEOUT = 23; /* GRA timer - T23 */
	public static final int ISUP_MTR_CIRCUIT_QUERY_TIMEOUT = 28; /* CRR timer - T28 */
	public static final int ISUP_MTR_CIRCUIT_VALIDATION_TIMEOUT = 44; /* CVT timer - T44 */
	public static final int ISUP_MTR_CONTINUITY_CHECK_REPEAT_TIMEOUT = 40; /* CCRr timer - T40 */
	public static final int ISUP_MTR_CONTINUITY_CHECK_RESPONSE_TIMEOUT = 26; /*CCR responsw timer - T26*/
	public static final int ISUP_MTR_LOOPBACK_ACK_TIMEOUT = 34; /* LBA timer - T34 */		
	public static final int ISUP_MTR_HGA_TIMEOUT = 47; /* HGA timer - T47 */
	public static final int ISUP_MTR_SCGA_TIMEOUT = 48; /* SCGA timer - T48 */

	
}


