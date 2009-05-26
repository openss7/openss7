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
* MODULE NAME  : $RCSfile: SetupItuEvent.java,v $
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

public class SetupItuEvent extends SetupEvent{

	public SetupItuEvent(java.lang.Object source,
                     SignalingPointCode dpc,
                     SignalingPointCode opc,
                     byte sls,
                     int cic,
                     byte congestionPriority,
                     boolean glareControl,
                     NatureConnInd nci,
                     FwdCallIndItu fci,
                     CalledPartyNumberItu cpn,
                     byte cat,
                     byte tmr)
              throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority,glareControl,nci,fci,cpn,cat);
		
		transmissionMediumReq = tmr;
		m_transmissionMediumReqPresent = true;
	}

	public void checkMandatoryParameters()
                      throws MandatoryParameterNotSetException{
		super.checkMandatoryParameters();
		if(m_transmissionMediumReqPresent == true)
			return;
		else throw new MandatoryParameterNotSetException();
	}


	public CUGInterlockCodeItu getCUGInterlockCode()
                                        throws ParameterNotSetException{
		if(isCUGInterlockCodeItu == true)
			return cugInterlockCodeItu;
		else throw new ParameterNotSetException();
	}

	public void setCUGInterlockCode(CUGInterlockCodeItu cugic){
		cugInterlockCodeItu = cugic;
		isCUGInterlockCodeItu = true;
	}

	public boolean isCUGInterlockCodePresent(){
		return isCUGInterlockCodeItu;
	}

	public LocationNumberItu getLocationNumber()
                                    throws ParameterNotSetException{
		if(isLocationNumberItu == true)
			return  locationNumberItu;
		else throw new ParameterNotSetException();
	}

	public void setLocationNumber(LocationNumberItu ln){
		locationNumberItu = ln;	
		isLocationNumberItu = true;
	}

	public boolean isLocationNumberPresent(){
		return isLocationNumberItu;
	}

	public NotificationInd getNotificationInd()
                                   throws ParameterNotSetException{
		if(isNotificationInd == true)
			return notificationInd;
		else throw new ParameterNotSetException();
	}

	public void setNotificationInd(NotificationInd gni){
		notificationInd = gni;
		isNotificationInd = true;
	}

	public boolean isNotificationIndPresent(){
		return isNotificationInd;
	}

	public NwSpecificFacItu getNwSpecificFac()
                                  throws ParameterNotSetException{
		if(isNwSpecificFacItu == true)
			return nwSpecificFacItu;
		else throw new ParameterNotSetException();
	}

	public void setNwSpecificFac(NwSpecificFacItu nsf){
		nwSpecificFacItu = nsf;
		isNwSpecificFacItu = true;
	}

	public boolean isNwSpecificFacPresent(){
		return isNwSpecificFacItu;
	}

	public OptionalFwdCallIndItu getOptionalFwdCallInd()
                                            throws ParameterNotSetException{
		if(isOptionalFwdCallIndItu == true)
			return optionalFwdCallIndItu;
		else throw new ParameterNotSetException();
	}
	
	public void setOptionalFwdCallInd(OptionalFwdCallIndItu ofci){
		optionalFwdCallIndItu = ofci;
		isOptionalFwdCallIndItu = true;
	}

	public boolean isOptionalFwdCallIndPresent(){
		return isOptionalFwdCallIndItu;
	}

	public byte[] getOrigISCPointCode()
                           throws ParameterNotSetException{
		if(isOrigISCPointCode == true)
			return origISCPointCode;
		else throw new ParameterNotSetException();
	}

	public void setOrigISCPointCode(byte[] origISCpointcode){
		origISCPointCode = origISCpointcode;
		isOrigISCPointCode = true;
	}

	public boolean isOrigISCPointCodePresent(){
		return isOrigISCPointCode;
	}
	
	public ParamCompatibilityInfoItu getParamCompatibilityInfo()
                                                    throws ParameterNotSetException{
		if(isParamCompatibilityInfoItu == true)
			return paramCompatibilityInfoItu;
		else throw new ParameterNotSetException();
	}

	public void setParamCompatibilityInfo(ParamCompatibilityInfoItu pci){
		paramCompatibilityInfoItu = pci;
		isParamCompatibilityInfoItu = true;
	}

	public boolean isParamCompatibilityInfoPresent(){
		return isParamCompatibilityInfoItu;
	}

	public int getPropagationDelayCounter()
                               throws ParameterNotSetException{
		if(isPropagationDelayCounter == true)
			return propagationDelayCounter;
		else throw new ParameterNotSetException();
	}

	public void setPropagationDelayCounter(int propDelayCounter)
                                throws ParameterRangeInvalidException{
		propagationDelayCounter = propDelayCounter;
		isPropagationDelayCounter = true;
	}

	public boolean isPropagationDelayCounterPresent(){
		return isPropagationDelayCounter;
	}

	public byte getTransmissionMediumReq() throws MandatoryParameterNotSetException{
		if(m_transmissionMediumReqPresent = true)
				return transmissionMediumReq;	
		else throw new MandatoryParameterNotSetException();
	}

	public void setTransmissionMediumReq(byte tmr)
                              throws ParameterRangeInvalidException{
		if ((tmr == TMR_SPEECH) || (tmr >= TMR_64_KBPS_UNRESTRICTED) && (tmr <= TMR_1920_KBPS_UNRESTRICTED)) {
			transmissionMediumReq          = tmr;
			m_transmissionMediumReqPresent = true;
		}
		else 
			throw new ParameterRangeInvalidException();

	}

	public byte getTransmissionMediumReqPrime()
                                   throws ParameterNotSetException{
		if(isTransmissionMediumReqPrime == true)
			return transmissionMediumReqPrime;
		else throw new ParameterNotSetException();
	}

	public void setTransmissionMediumReqPrime(byte tmrp)
                                   throws ParameterRangeInvalidException{
		if ((tmrp == TMR_SPEECH) || (tmrp >= TMR_64_KBPS_UNRESTRICTED) && (tmrp <= TMR_1920_KBPS_UNRESTRICTED)) {
			transmissionMediumReqPrime = tmrp;
			isTransmissionMediumReqPrime = true;
		}
		else 
			throw new ParameterRangeInvalidException();

	}
	
	public boolean isTransmissionMediumReqPrimePresent(){
		return isTransmissionMediumReqPrime;
	}

	public UserServiceInfo getUserServiceInfo()
                                   throws ParameterNotSetException{
		if(isUserServiceInfo == true)
			return userServiceInfo;
		else throw new ParameterNotSetException();
	}

	public void setUserServiceInfo(UserServiceInfo usi){
		userServiceInfo = usi;
		isUserServiceInfo = true;
	}

	public boolean isUserServiceInfoPresent(){
		return isUserServiceInfo;
	}
	
	public UserToUserIndicatorsItu getUserToUserIndicators()
                                                throws ParameterNotSetException{
		if(isUserToUserIndicatorsItu == true)
			return userToUserIndicatorsItu;
		else throw new ParameterNotSetException();
	}
	public void setUserToUserIndicators(UserToUserIndicatorsItu uui){
		userToUserIndicatorsItu = uui;
		isUserToUserIndicatorsItu = true;
	}
	
	public boolean isUserToUserIndicatorsPresent(){
		return isUserToUserIndicatorsItu;
	}
		
	/**
    * String representation of class SetupEvent
    *
    * @return    String provides description of class SetupEvent
    */
        public java.lang.String toString(){
		int i;
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\nisCUGInterlockCodeItu = ");
				buffer.append(isCUGInterlockCodeItu);
				buffer.append("\ncugInterlockCodeItu = ");
				buffer.append(cugInterlockCodeItu);
				buffer.append("\n\nisLocationNumberItu = ");
				buffer.append(isLocationNumberItu);
				buffer.append("\nlocationNumberItu  = ");
				buffer.append(locationNumberItu);
				buffer.append("\n\nisNotificationInd = ");
				buffer.append(isNotificationInd);
				buffer.append("\nnotificationInd = ");
				buffer.append(notificationInd);
				buffer.append("\n\nisNwSpecificFacItu = ");
				buffer.append(isNwSpecificFacItu);
				buffer.append("\nnwSpecificFacItu = ");
				buffer.append(nwSpecificFacItu);
				buffer.append("\n\nisOptionalFwdCallIndItu = ");
				buffer.append(isOptionalFwdCallIndItu);
				buffer.append("\noptionalFwdCallIndItu = ");
				buffer.append(optionalFwdCallIndItu);
				buffer.append("\n\nisOrigISCPointCode = ");
				buffer.append(isOrigISCPointCode);				
				if(isOrigISCPointCode == true){
					buffer.append("\norigISCPointCode = ");
					for(i=0;i<origISCPointCode.length;i++)
						buffer.append(" "+Integer.toHexString((int)(origISCPointCode[i] & 0xFF)));		
				}
				buffer.append("\n\nisParamCompatibilityInfoItu = ");
				buffer.append(isParamCompatibilityInfoItu);
				buffer.append("\nparamCompatibilityInfoItu = ");
				buffer.append(paramCompatibilityInfoItu);
				buffer.append("\n\nisPropagationDelayCounter = ");
				buffer.append(isPropagationDelayCounter);
				buffer.append("\npropagationDelayCounter = ");
				buffer.append(propagationDelayCounter);
				buffer.append("\n\ntransmissionMediumReq = ");
				buffer.append(transmissionMediumReq);
				buffer.append("\n\nisTransmissionMediumReqPrime = ");
				buffer.append(isTransmissionMediumReqPrime);
				buffer.append("\ntransmissionMediumReqPrime = ");
				buffer.append(transmissionMediumReqPrime);
				buffer.append("\n\nisUserServiceInfo = ");
				buffer.append(isUserServiceInfo);
				buffer.append("\nuserServiceInfo = ");
				buffer.append(userServiceInfo);
				buffer.append("\n\nisUserToUserIndicatorsItu = ");
				buffer.append(isUserToUserIndicatorsItu);
				buffer.append("\n userToUserIndicatorsItu= ");
				buffer.append(userToUserIndicatorsItu);				
				return buffer.toString();
		
		}


	CUGInterlockCodeItu       cugInterlockCodeItu;
	boolean                   isCUGInterlockCodeItu;
	boolean                   isLocationNumberItu;
	boolean                   isNotificationInd;
	boolean                   isNwSpecificFacItu;
	boolean                   isOptionalFwdCallIndItu;
	boolean                   isOrigISCPointCode;
	boolean                   isParamCompatibilityInfoItu;
	boolean                   isPropagationDelayCounter;
	boolean                   isTransmissionMediumReqPrime;
	boolean                   isUserServiceInfo;
	boolean                   isUserToUserIndicatorsItu;
	LocationNumberItu         locationNumberItu;
	NotificationInd           notificationInd;
	NwSpecificFacItu          nwSpecificFacItu;
	OptionalFwdCallIndItu     optionalFwdCallIndItu;
	byte[]                    origISCPointCode;
	ParamCompatibilityInfoItu paramCompatibilityInfoItu;
	int                       propagationDelayCounter;
	byte                      transmissionMediumReq;
	byte                      transmissionMediumReqPrime;
	UserServiceInfo           userServiceInfo;
	UserToUserIndicatorsItu   userToUserIndicatorsItu;


	protected boolean m_transmissionMediumReqPresent     = false;

	public static final byte TMR_SPEECH = 0x00; 
	public static final byte TMR_64_KBPS_UNRESTRICTED = 0x02; 
	public static final byte TMR_1920_KBPS_UNRESTRICTED = 0x0A;    
    
}


