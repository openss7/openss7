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
* DATE         : $Date: 2008/05/16 12:24:09 $
* 
* MODULE NAME  : $RCSfile: ExitAnsiEvent.java,v $
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


package javax.jain.ss7.isup.ansi;

import javax.jain.*;
import javax.jain.ss7.*;
import javax.jain.ss7.isup.*;

public class ExitAnsiEvent extends IsupEvent{

	public ExitAnsiEvent(java.lang.Object source,
                     SignalingPointCode dpc,
                     SignalingPointCode opc,
                     byte sls,
                     int cic,
                     byte congestionPriority)
              throws ParameterRangeInvalidException{
	


		super(source,dpc,opc,sls,cic,congestionPriority);		

	}

	public int getIsupPrimitive(){
		return IsupConstants.ISUP_PRIMITIVE_EXIT;
	}
		
	public java.lang.String getOutgoingTrunkGrpNumber()
                                           throws ParameterNotSetException{
		if(isOgTgpNum == true)
			return ogTgpNum;
		else throw new ParameterNotSetException();
	}
	
	public void setOutgoingTrunkGrpNumber(java.lang.String in_ogTgpNum){
		ogTgpNum   = in_ogTgpNum;
		isOgTgpNum = true;
	}

	public boolean isOutgoingTrunkGrpNumberPresent(){
		return  isOgTgpNum;
	}
	

	/**
    * String representation of class ExitAnsiEvent
    *
    * @return    String provides description of class ExitAnsiEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\nisOgTgpNum = ");
				buffer.append(isOgTgpNum);
				buffer.append("\nogTgpNum = ");
				buffer.append(ogTgpNum);
				return buffer.toString();
		
		}


	boolean          isOgTgpNum;
	java.lang.String ogTgpNum;

	
}	


