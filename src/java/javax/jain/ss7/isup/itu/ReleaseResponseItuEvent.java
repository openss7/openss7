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
* DATE         : $Date: 2008/05/16 12:23:59 $
* 
* MODULE NAME  : $RCSfile: ReleaseResponseItuEvent.java,v $
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


package javax.jain.ss7.isup.itu;

import javax.jain.*;
import javax.jain.ss7.*;
import javax.jain.ss7.isup.*;

public class ReleaseResponseItuEvent extends ReleaseResponseEvent{
	
	public ReleaseResponseItuEvent(java.lang.Object source,
                               SignalingPointCode dpc,
                               SignalingPointCode opc,
                               byte sls,
                               int cic,
                               byte congestionPriority)
                        throws ParameterRangeInvalidException{
		super(source,dpc,opc,sls,cic,congestionPriority);

	}
		
	public CauseInd getCauseInd()
                     throws ParameterNotSetException{
		if(isCauseInd == true)
			return causeInd;
		else throw new ParameterNotSetException();
	}

	public void setCauseInd(CauseInd ci){
		causeInd   = ci;
		isCauseInd = true;
	}

	public boolean isCauseIndPresent(){
		return isCauseInd;
	}


	/**
    * String representation of class ReleaseResponseItuEvent
    *
    * @return    String provides description of class ReleaseResponseItuEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\nisCauseInd = ");
				buffer.append(isCauseInd);	
				buffer.append("\ncauseInd = ");
				buffer.append(causeInd);	
				return buffer.toString();
		
		}


	
	CauseInd causeInd;
	boolean  isCauseInd;
	    
}


