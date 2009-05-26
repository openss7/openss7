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
* DATE         : $Date: 2008/05/16 12:23:55 $
* 
* MODULE NAME  : $RCSfile: FacilityRejectItuEvent.java,v $
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

public class FacilityRejectItuEvent extends IsupEvent{

	public FacilityRejectItuEvent(java.lang.Object source,
                              SignalingPointCode dpc,
                              SignalingPointCode opc,
                              byte sls,
                              int cic,
                              byte congestionPriority,
                              short facilityInd,
                              CauseInd causeIndicator)
                       throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);
		facInd   = facilityInd;
		causeInd = causeIndicator;
		m_causeIndPresent = true;
	}

	public void checkMandatoryParameters()
                      throws MandatoryParameterNotSetException{
		if(m_causeIndPresent == true)
			return;
		else throw new MandatoryParameterNotSetException();

	}

	public int getIsupPrimitive(){

		return IsupConstants.ISUP_PRIMITIVE_FACILITY_REJECT; 

	}

	public CauseInd getCauseInd()
                     throws MandatoryParameterNotSetException{
		if(m_causeIndPresent == true)
			return causeInd;
		else throw new MandatoryParameterNotSetException();

	}

	public void setCauseInd(CauseInd causeIndicator){		
		causeInd          = causeIndicator;
		m_causeIndPresent = true;
	}

	public short getFacilityInd(){
		return facInd;
	}

	public void setFacilityInd(short in_facInd)
                    throws ParameterRangeInvalidException{
		if((in_facInd >=0) && (in_facInd<=255))
			facInd = in_facInd;		
		else throw new ParameterRangeInvalidException();
	}
	
	public UserToUserIndicatorsItu getUserToUserIndicators()
                                                throws ParameterNotSetException{
		if(isUserToUserIndicatorsItu == true)
			return userToUserIndicatorsItu;
		else throw new ParameterNotSetException();
	}
		
	public void setUserToUserIndicators(UserToUserIndicatorsItu uui){

		userToUserIndicatorsItu   = uui; 
		isUserToUserIndicatorsItu = true;
	}

	public boolean isUserToUserIndicatorsPresent(){
		return  isUserToUserIndicatorsItu;
	}

	/**
    * String representation of class FacilityRejectItuEvent
    *
    * @return    String provides description of class FacilityRejectItuEvent
    */
        public java.lang.String toString(){
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\ncauseInd = ");
				buffer.append(causeInd);
				buffer.append("\n\nfacInd  = ");
				buffer.append(facInd);	
				buffer.append("\n\nisUserToUserIndicatorsItu  = ");
				buffer.append(isUserToUserIndicatorsItu);		
				buffer.append("\nuserToUserIndicatorsItu  = ");
				buffer.append(userToUserIndicatorsItu);			
				return buffer.toString();
		
		}

	



	CauseInd                causeInd;
	short                   facInd;
	boolean                 isUserToUserIndicatorsItu;
	UserToUserIndicatorsItu userToUserIndicatorsItu;

	private boolean m_causeIndPresent = false;
    
}


