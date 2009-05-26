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
* DATE         : $Date: 2008/05/16 12:24:07 $
* 
* MODULE NAME  : $RCSfile: ReleaseEvent.java,v $
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

public abstract class ReleaseEvent extends IsupEvent{
	
	protected ReleaseEvent(java.lang.Object source,
                       SignalingPointCode dpc,
                       SignalingPointCode opc,
                       byte sls,
                       int cic,
                       byte congestionPriority,
                       CauseInd causeIndicator)
                throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);		
		causeInd = causeIndicator;
		m_causeIndPresent = true;
	}

	
	public void checkMandatoryParameters()
                      throws MandatoryParameterNotSetException{
		if(m_causeIndPresent == true)
			return;
		else throw new MandatoryParameterNotSetException();

	}

	public byte[] getAccessTransport()
                          throws ParameterNotSetException{
		if(isAccessTransport == true)
				return accessTransport;
		else throw new ParameterNotSetException();
	}

	public boolean isAccessTransportPresent(){
		return isAccessTransport;
	}

	public void setAccessTransport(byte[] accessTrans){
		accessTransport = accessTrans;
		isAccessTransport = true;
	}

	public byte getAutoCongLevel()
                      throws ParameterNotSetException{
		if(isAutoCongLevel == true)
				return autoCongLevel;
		else throw new ParameterNotSetException();
	}

	public boolean isAutoCongLevelPresent(){
		return isAutoCongLevel;
	}

	public void setAutoCongLevel(byte autoCongestionLevel)
                      throws ParameterRangeInvalidException{
		 if ((autoCongestionLevel >= ACL_LEVEL1_EXCEEDED) &&
		  (autoCongestionLevel <= ACL_LEVEL3_EXCEEDED)) {
			autoCongLevel = autoCongestionLevel;
			isAutoCongLevel = true;	
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public CauseInd getCauseInd()
                     throws MandatoryParameterNotSetException{
		if(m_causeIndPresent == true)
			return causeInd;
		else throw new MandatoryParameterNotSetException();
	}
		
	public int getIsupPrimitive(){
		 return IsupConstants.ISUP_PRIMITIVE_RELEASE;
	}			

	public byte[] getUserToUserInformation()
                                throws ParameterNotSetException{
		if(isUserToUserInformation == true)
				return userToUserInformation;
		else throw new ParameterNotSetException();
	}

	public boolean isUserToUserInformationPresent(){
		return isUserToUserInformation;
	}

	public void setUserToUserInformation(byte[] userToUserInfo){
		userToUserInformation = userToUserInfo;
		isUserToUserInformation = true;
	}

	public void setCauseInd(CauseInd causeIndicator){
		causeInd = causeIndicator;
		m_causeIndPresent = true;
	}
	
	/**
    * String representation of class ReleaseEvent
    *
    * @return    String provides description of class ReleaseEvent
    */
        public java.lang.String toString(){
		int i;
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\nisAccessTransport = ");
				buffer.append(isAccessTransport);
				if(isAccessTransport == true){
					buffer.append("\naccessTransport = ");
					for(i=0;i<accessTransport.length;i++)
						buffer.append(" "+Integer.toHexString((int)(accessTransport[i] & 0xFF)));
				}
				buffer.append("\n\nisAutoCongLevel = ");
				buffer.append(isAutoCongLevel);
				buffer.append("\nautoCongLevel = ");
				buffer.append(autoCongLevel);
				buffer.append("\n\ncauseInd = ");
				buffer.append(causeInd);
				buffer.append("\n\nisUserToUserInformation = ");
				buffer.append(isUserToUserInformation);
				if(isUserToUserInformation == true){
					buffer.append("\nuserToUserInformation = ");
					for(i=0;i<userToUserInformation.length;i++)
						buffer.append(" "+Integer.toHexString((int)(userToUserInformation[i] & 0xFF)));
				}
				return buffer.toString();
		
		}	
	
	byte[]   accessTransport;
	byte     autoCongLevel;
	CauseInd causeInd;
	boolean  isAccessTransport;
	boolean  isAutoCongLevel;
	boolean  isUserToUserInformation;
	byte[]   userToUserInformation;

	private boolean m_causeIndPresent = false;

	public static final byte ACL_LEVEL1_EXCEEDED = 1; 
	public static final byte ACL_LEVEL2_EXCEEDED = 2; 
	public static final byte ACL_LEVEL3_EXCEEDED = 3; 
}


