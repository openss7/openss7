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
* DATE         : $Date: 2008/05/16 12:24:12 $
* 
* MODULE NAME  : $RCSfile: SetupEvent.java,v $
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

public abstract class SetupEvent extends IsupEvent{

	protected SetupEvent(java.lang.Object source,
                     SignalingPointCode dpc,
                     SignalingPointCode opc,
                     byte sls,
                     int cic,
                     byte congestionPriority,
                     boolean glareControl,
                     NatureConnInd natureConInd,
                     FwdCallInd forwardCallInd,
                     CalledPartyNumber calledPartyNum,
                     byte callingCategory)
              throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);
		glareCntrl = glareControl;
		natureConnInd = natureConInd;
		fwdCallInd = forwardCallInd;
		calledPartyNumber = calledPartyNum;
		m_natureConnIndPresent     = true;
		m_fwdCallIndPresent        = true;
		m_calledPartyNumberPresent = true;

		 if ((callingCategory >= CPC_UNKNOWN) &&
		  (callingCategory <= CPC_NATIONAL_SECURITY_EMERGENCY_PREPAREDNESS_CALL)) {
			callingPartyCat = callingCategory;
			m_callingPartyCatPresent = true;
		 }
		 else 
			throw new ParameterRangeInvalidException();
			
	}

	public void checkMandatoryParameters()
                      throws MandatoryParameterNotSetException{
		if((m_natureConnIndPresent == true) &&
			(m_fwdCallIndPresent == true) &&
			(m_calledPartyNumberPresent == true) &&
			(m_callingPartyCatPresent == true))
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

	public CalledPartyNumber getCalledPartyNumber()
                            throws MandatoryParameterNotSetException{
		if(m_calledPartyNumberPresent == true)
			return calledPartyNumber;		
		else throw new MandatoryParameterNotSetException();
	}

	public void setCalledPartyNumber(CalledPartyNumber cpn){
		calledPartyNumber = cpn;
		m_calledPartyNumberPresent  = true;
	}


	public byte getCallingPartyCat(){
		return callingPartyCat;
	}

	public void setCallingPartyCat(byte cat)
                        throws ParameterRangeInvalidException{
		 if ((cat >= CPC_UNKNOWN) &&
		  (cat <= CPC_NATIONAL_SECURITY_EMERGENCY_PREPAREDNESS_CALL)) {
			callingPartyCat          = cat;
			m_callingPartyCatPresent = true;
		 }
		 else 
			throw new ParameterRangeInvalidException();
	}

	public CallingPartyNumber getCallingPartyNumber()
                            throws ParameterNotSetException{
		if(isCallingPartyNumber == true)
				return callingPartyNumber;
		else throw new ParameterNotSetException();
	}

	public boolean isCallingPartyNumberPresent(){
		return isCallingPartyNumber;
	}

	public void setCallingPartyNumber(CallingPartyNumber callingPartyNum){
		callingPartyNumber = callingPartyNum;
		isCallingPartyNumber = true;
	}

	public CallReference getCallReference()
                               throws ParameterNotSetException{
		if(isCallReference == true)
				return callReference;
		else throw new ParameterNotSetException();
	}

	public boolean isCallReferencePresent(){
		return isCallReference;
	}

	public void setCallReference(CallReference callRef){
		callReference = callRef;
		isCallReference = true;
	}
	
	public ConReq getConReq()
                 throws ParameterNotSetException{
		if(isConReq == true)
				return conReq;
		else throw new ParameterNotSetException();		 
	}

	public boolean isConReqPresent(){
		return isConReq; 
	}	

	public void setConReq(ConReq connectionReq){
		conReq = connectionReq;
		isConReq = true;
	}

	public FwdCallInd getFwdCallInd()
                         throws MandatoryParameterNotSetException{
		if(m_fwdCallIndPresent == true)
			return fwdCallInd;
		else throw new MandatoryParameterNotSetException();
	}

	public void setFwdCallInd(FwdCallInd fci){
		fwdCallInd = fci;
		m_fwdCallIndPresent = true;
	}

	public GenericDigits getGenericDigits()
                               throws ParameterNotSetException{
		if(isGenericDigits == true)
				return genericDigits;
		else throw new ParameterNotSetException();
	}

	public boolean isGenericDigitsPresent(){
		return isGenericDigits;
	}

	public void setGenericDigits(GenericDigits gd){
		genericDigits = gd;
		isGenericDigits = true;
	}

	public GenericNumber getGenericNumber()
                               throws ParameterNotSetException{
		if(isGenericNumber == true)
				return genericNumber;
		else throw new ParameterNotSetException();
	}

	public boolean isGenericNumberPresent(){
		return isGenericNumber;
	}

	public void setGenericNumber(GenericNumber cid){
		genericNumber = cid;
		isGenericNumber = true;
	}

	public boolean getGlareControl(){
		return glareCntrl;
	}

	public void setGlareControl(boolean glareControl){
		glareCntrl = glareControl;
	}

	public int getIsupPrimitive(){
                return IsupConstants.ISUP_PRIMITIVE_SETUP;
    }

	public Precedence getMLPPprecedence()
                             throws ParameterNotSetException{
		if(isMLPPprecedence == true)
				return mlppPrecedence;
		else throw new ParameterNotSetException();
	}

	public boolean isMLPPprecedencePresent(){
		return isMLPPprecedence;
	}

	public void setMLPPprecedence(Precedence precedence){
		mlppPrecedence = precedence;
		isMLPPprecedence = true;
	}

	public NatureConnInd getNatureConnInd()
                               throws MandatoryParameterNotSetException{
		if(m_natureConnIndPresent == true)
			return natureConnInd;
		else throw new MandatoryParameterNotSetException();
	}

	public void setNatureConnInd(NatureConnInd nci){
		natureConnInd = nci;
		m_natureConnIndPresent = true;
	}

	public OrigCalledNumber getOrigCalledNumber()
                                     throws ParameterNotSetException{
		if(isOrigCalledNumber == true)
				return origCalledNumber;
		else throw new ParameterNotSetException();
	}

	public boolean isOrigCalledNumberPresent(){
		return isOrigCalledNumber;
	}
		
	public void setOrigCalledNumber(OrigCalledNumber ocn){
		origCalledNumber = ocn;
		isOrigCalledNumber = true;
	}
	
	public RedirectingNumber getRedirectingNumber()
                                 throws ParameterNotSetException{
		if(isRedirectingNumber == true)
				return redirectingNumber;
		else throw new ParameterNotSetException();
	}
		
	public boolean isRedirectingNumberPresent(){
		return isRedirectingNumber; 
	}

	public void setRedirectingNumber(RedirectingNumber rn){
		redirectingNumber = rn;
		isRedirectingNumber = true;
	}

	public RedirectionInfo getRedirectionInfo()
                                   throws ParameterNotSetException{
		if(isRedirectionInfo == true)
				return redirectionInfo;  
		else throw new ParameterNotSetException();
	}	

	public boolean isRedirectionInfoPresent(){
		return isRedirectionInfo;
	}

	public void setRedirectionInfo(RedirectionInfo ri){
		redirectionInfo = ri;
		isRedirectionInfo = true;
	}
	
	public RemoteOperations getRemoteOperations()
                                     throws ParameterNotSetException{
		if(isRemoteOperations == true)
				return remoteOperations;
		else throw new ParameterNotSetException();
	}

	public boolean isRemoteOperationsPresent(){
		return isRemoteOperations;
	}

	public void setRemoteOperations(RemoteOperations remoteOp){
		remoteOperations = remoteOp;
		isRemoteOperations = true;
	}

	public byte[] getServiceActivation()
                            throws ParameterNotSetException{
		if(isServiceActivation == true)
				return serviceActivation;
		else throw new ParameterNotSetException();
	}

	public boolean isServiceActivationPresent(){
		return isServiceActivation;
	}

	public void setServiceActivation(byte[] servAct){
		serviceActivation = servAct;
		isServiceActivation = true;
	}

	public TransitNwSel getTransitNwSel()
                             throws ParameterNotSetException{
		if(isTransitNwSel == true)
				return transitNwSel;
		else throw new ParameterNotSetException();
	}

	public boolean isTransitNwSelPresent(){
		return isTransitNwSel;
	}

	public void setTransitNwSel(TransitNwSel tns){
		transitNwSel = tns;
		isTransitNwSel = true;
	}

	public UserServiceInfoPrime getUserServiceInfoPrime()
                                  throws ParameterNotSetException{
		if(isUserServiceInfoPrime == true)
				return userServiceInfoPrime;
		else throw new ParameterNotSetException();
	}

	public boolean isUserServiceInfoPrimePresent(){
		return isUserServiceInfoPrime;
	}

	public void setUserServiceInfoPrime(UserServiceInfoPrime usip){
		userServiceInfoPrime = usip;
		isUserServiceInfoPrime = true;
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


	/**
    * String representation of class SetupEvent
    *
    * @return    String provides description of class SetupEvent
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
				buffer.append("\n\ncalledPartyNumber  = ");
				buffer.append(calledPartyNumber);
				buffer.append("\n\ncallingPartyCat = ");
				buffer.append(callingPartyCat);
				buffer.append("\n\nisCallingPartyNumber= ");
				buffer.append(isCallingPartyNumber);
				buffer.append("\ncallingPartyNumber = ");
				buffer.append(callingPartyNumber);
				buffer.append("\n\nisCallReference = ");
				buffer.append(isCallReference);
				buffer.append("\ncallReference = ");
				buffer.append(callReference);
				buffer.append("\n\nisConReq = ");
				buffer.append(isConReq);
				buffer.append("\nconReq = ");
				buffer.append(conReq);
				buffer.append("\n\nfwdCallInd = ");
				buffer.append(fwdCallInd);
				buffer.append("\n\nisGenericDigits = ");
				buffer.append(isGenericDigits);
				buffer.append("\ngenericDigits = ");
				buffer.append(genericDigits);
				buffer.append("\n\nisGenericNumber = ");
				buffer.append(isGenericNumber);
				buffer.append("\ngenericNumber = ");
				buffer.append(genericNumber);
				buffer.append("\n\nglareCntrl = ");
				buffer.append(glareCntrl);
				buffer.append("\n\nisMLPPprecedence = ");
				buffer.append(isMLPPprecedence);
				buffer.append("\nmlppPrecedence = ");
				buffer.append(mlppPrecedence);
				buffer.append("\n\nnatureConnInd = ");
				buffer.append(natureConnInd);
				buffer.append("\n\nisOrigCalledNumber = ");
				buffer.append(isOrigCalledNumber);
				buffer.append("\norigCalledNumber = ");
				buffer.append(origCalledNumber);
				buffer.append("\n\nisRedirectingNumber = ");
				buffer.append(isRedirectingNumber);
				buffer.append("\nredirectingNumber = ");
				buffer.append(redirectingNumber);
				buffer.append("\n\nisRedirectionInfo = ");
				buffer.append(isRedirectionInfo);
				buffer.append("\nredirectionInfo = ");
				buffer.append(redirectionInfo);
				buffer.append("\n\nisRemoteOperations = ");
				buffer.append(isRemoteOperations);
				buffer.append("\nremoteOperations = ");
				buffer.append(remoteOperations);
				buffer.append("\n\nisServiceActivation = ");
				buffer.append(isServiceActivation);
				if(isServiceActivation == true){
					buffer.append("\nserviceActivation = ");
					for(i=0;i<serviceActivation.length;i++)
						buffer.append(" "+Integer.toHexString((int)(serviceActivation[i] & 0xFF)));
				}
				buffer.append("\n\nisTransitNwSel = ");
				buffer.append(isTransitNwSel);
				buffer.append("\ntransitNwSel = ");
				buffer.append(transitNwSel);
				buffer.append("\n\nisUserServiceInfoPrime = ");
				buffer.append(isUserServiceInfoPrime);
				buffer.append("\nuserServiceInfoPrime = ");
				buffer.append(userServiceInfoPrime);
				buffer.append("\n\nisUserToUserInformation = ");
				buffer.append(isUserToUserInformation);
				if(isUserToUserInformation == true){
					buffer.append("\nuserToUserInformation = ");
					for(i=0;i<userToUserInformation.length;i++)
						buffer.append(" "+Integer.toHexString((int)(userToUserInformation[i] & 0xFF)));
				}
				return buffer.toString();
		
		}

	byte[]               accessTransport;
	CalledPartyNumber    calledPartyNumber;
	byte                 callingPartyCat;
	CallingPartyNumber   callingPartyNumber;
	CallReference        callReference;
	ConReq               conReq;
	FwdCallInd           fwdCallInd;
	GenericDigits        genericDigits;
	GenericNumber        genericNumber;
	boolean              glareCntrl;
	boolean              isAccessTransport;
	boolean              isCallingPartyNumber;
	boolean              isCallReference;
	boolean              isConReq;
	boolean              isGenericDigits;
	boolean              isGenericNumber;
	boolean              isMLPPprecedence;
	boolean              isOrigCalledNumber;
	boolean              isRedirectingNumber;
	boolean              isRedirectionInfo;
	boolean              isRemoteOperations;
	boolean              isServiceActivation;
	boolean              isTransitNwSel;
	boolean              isUserServiceInfoPrime;
	boolean              isUserToUserInformation;	
	Precedence           mlppPrecedence;
	NatureConnInd        natureConnInd;
	OrigCalledNumber     origCalledNumber;
	RedirectingNumber    redirectingNumber;
	RedirectionInfo      redirectionInfo;
	RemoteOperations     remoteOperations;
	byte[]               serviceActivation;
	TransitNwSel         transitNwSel;
	UserServiceInfoPrime userServiceInfoPrime;
	byte[]               userToUserInformation;

	protected boolean m_natureConnIndPresent     = false;
	protected boolean m_fwdCallIndPresent        = false;
	protected boolean m_calledPartyNumberPresent = false;
	protected boolean m_callingPartyCatPresent   = false;

	public static final int CPC_UNKNOWN = 0; 
	public static final int CPC_NATIONAL_SECURITY_EMERGENCY_PREPAREDNESS_CALL = 0xE2; 
	
}


