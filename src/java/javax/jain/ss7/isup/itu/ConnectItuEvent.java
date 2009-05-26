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
* MODULE NAME  : $RCSfile: ConnectItuEvent.java,v $
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

public class ConnectItuEvent extends IsupEvent{


	public ConnectItuEvent(java.lang.Object source,
                       SignalingPointCode dpc,
                       SignalingPointCode opc,
                       byte sls,
                       int cic,
                       byte congestionPriority,
                       BwdCallIndItu bci)
                throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);
		bwdCallIndItu = bci;
		m_bwdCallIndItuPresent = true;
		
	}

	public void checkMandatoryParameters()
                      throws MandatoryParameterNotSetException{
		if(m_bwdCallIndItuPresent == true)
			return;
		else throw new MandatoryParameterNotSetException();

	}

	public byte getAccessDeliveryInfo()
                           throws ParameterNotSetException{
		if(isAccessDeliveryInfo == true)
			return accessDeliveryInfo;
		else throw new ParameterNotSetException();
	}
	
	public void setAccessDeliveryInfo(byte adi){
		accessDeliveryInfo   = adi; 
		isAccessDeliveryInfo = true;
	}

	public boolean isAccessDeliveryInfoPresent(){
		return isAccessDeliveryInfo;
	}
	
	public byte[] getAccessTransport()
                          throws ParameterNotSetException{
		if(isAccessTransport == true)
			return accessTransport;
		else throw new ParameterNotSetException();

	}

	public void setAccessTransport(byte[] accessTrans){
		accessTransport   = accessTrans;
		isAccessTransport = true;
	}

	public boolean isAccessTransportPresent(){
		return isAccessTransport;
	}

	public BwdCallIndItu getBwdCallInd()
                            throws MandatoryParameterNotSetException{
		if(m_bwdCallIndItuPresent == true)
			return bwdCallIndItu;
		else throw new MandatoryParameterNotSetException();
	}

	public void setBwdCallInd(BwdCallIndItu bci){
		bwdCallIndItu = bci;
		m_bwdCallIndItuPresent = true;
	}
	
	public long getCallHistoryInfo()
                        throws ParameterNotSetException{
		if(isCallHistoryInfo == true)
			return callHistoryInfo;
		else throw new ParameterNotSetException();
	}

	public void setCallHistoryInfo(long callHistInfo){
		callHistoryInfo   = callHistInfo;
		isCallHistoryInfo = true;
	}

	public boolean isCallHistoryInfoPresent(){
		return isCallHistoryInfo;
	}

	public CallReference getCallReference()
                               throws ParameterNotSetException{
		if(isCallReference == true)
			return callReference;
		else throw new ParameterNotSetException();
	}

	public void setCallReference(CallReference callRef){
		callReference   = callRef;
		isCallReference = true;
	}

	public boolean isCallReferencePresent(){
		return isCallReference;
	}

	public ConnectedNumberItu getConnectedNumber()
                                      throws ParameterNotSetException{
		if(isConnectedNumberItu == true)
			return connectedNumberItu;
		else throw new ParameterNotSetException();
	}

	public void setConnectedNumber(ConnectedNumberItu connNumItu){
		connectedNumberItu   = connNumItu;
		isConnectedNumberItu = true;
	}

	public boolean isConnectedNumberPresent(){
		return isConnectedNumberItu;
	}
	
	public EchoControlInfoItu getEchoControlInfo()
                                      throws ParameterNotSetException{
		if(isEchoControlInfoItu == true)
			return echoControlInfoItu;
		else throw new ParameterNotSetException();
	}

	public void setEchoControlInfo(EchoControlInfoItu eci){
		echoControlInfoItu   = eci; 
		isEchoControlInfoItu = true;
	}

	public boolean isEchoControlInfoPresent(){
		return isEchoControlInfoItu;
	}

	public GenericNumberItu getGenericNumber()
                                  throws ParameterNotSetException{
		if(isGenericNumberItu == true)
			return 	genericNumberItu;
		else throw new ParameterNotSetException();
	}

	public void setGenericNumber(GenericNumberItu genNumItu){
		genericNumberItu   = genNumItu;
		isGenericNumberItu = true;
	}

	public boolean isGenericNumberPresent(){
		return isGenericNumberItu;
	}

	public int getIsupPrimitive(){

		return IsupConstants.ISUP_PRIMITIVE_CONNECT; 

	}

	public NotificationInd getNotificationInd()
                                   throws ParameterNotSetException{
		if(isNotificationInd == true)
			return notificationInd;
		else throw new ParameterNotSetException();
	}

	public void setNotificationInd(NotificationInd noi){
		notificationInd   = noi;
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
		nwSpecificFacItu   = nsf;
		isNwSpecificFacItu = true;
	}

	public boolean isNwSpecificFacPresent(){
		return isNwSpecificFacItu;
	}

	public OptionalBwdCallIndItu getOptionalBwdCallInd()
                                            throws ParameterNotSetException{
		if(isOptionalBwdCallIndItu == true)
			return optionalBwdCallIndItu;
		else throw new ParameterNotSetException();
		
	}

	public void setOptionalBwdCallInd(OptionalBwdCallIndItu obci){
		optionalBwdCallIndItu   = obci;
		isOptionalBwdCallIndItu = true;
	}

	public boolean isOptionalBwdCallIndPresent(){
		return isOptionalBwdCallIndItu;
	}

	public ParamCompatibilityInfoItu getParamCompatibilityInfo()
                                        throws ParameterNotSetException{
		if(isParamCompatibilityInfoItu == true)
			return paramCompatibilityInfoItu;
		else throw new ParameterNotSetException();
	}

	public void setParamCompatibilityInfo(ParamCompatibilityInfoItu pci){
		paramCompatibilityInfoItu   = pci;
		isParamCompatibilityInfoItu = true;
	}

	public boolean isParamCompatibilityInfoPresent(){
		return isParamCompatibilityInfoItu;
	}

	public RedirectionNumberItu getRedirectionNumber()
                                          throws ParameterNotSetException{
		if(isRedirectionNumberItu == true)
			return redirectionNumberItu;
		else throw new ParameterNotSetException();
	}

	public void setRedirectionNumber(RedirectionNumberItu rn){
		redirectionNumberItu   = rn;
		isRedirectionNumberItu = true;
	}

	public boolean isRedirectionNumberPresent(){
		return isRedirectionNumberItu;
	}
		
	public byte getRedirectionNumberRest()
                              throws ParameterNotSetException{
		if(isRedirectionNumberRest == true)
			return redirectionNumberRest;
		else throw new ParameterNotSetException();
		
	}

	public void setRedirectionNumberRest(byte rnr){
		redirectionNumberRest   = rnr;
		isRedirectionNumberRest =  true;
	}

	public boolean isRedirectionNumberRestPresent(){
		return isRedirectionNumberRest;
	}
	
	public RemoteOperations getRemoteOperations()
                                     throws ParameterNotSetException{

		if(isRemoteOperations == true)
			return remoteOperations;
		else throw new ParameterNotSetException();
	}

	public void setRemoteOperations(RemoteOperations remoteOp){
		remoteOperations   = remoteOp;
		isRemoteOperations = true; 
	}

	public boolean isRemoteOperationsPresent(){
		return isRemoteOperations;
	}

	public byte[] getServiceActivation()
                            throws ParameterNotSetException{
		if(isServiceActivation == true)
			return serviceActivation;
		else throw new ParameterNotSetException();
		
	}

	public void setServiceActivation(byte[] servAct){

		serviceActivation   = servAct;
		isServiceActivation = true;
	}

	public boolean isServiceActivationPresent(){
		return isServiceActivation;
	}

	public byte getTransmissionMediumUsed()
                               throws ParameterNotSetException{
		if(isTransmissionMediumUsed == true)
			return transmissionMediumUsed;
		else throw new ParameterNotSetException();
	}

	public void setTransmissionMediumUsed(byte tmu)
                               throws ParameterRangeInvalidException{
		if ((tmu == TMR_SPEECH) || (tmu >= TMR_64_KBPS_UNRESTRICTED) && (tmu <= TMR_1920_KBPS_UNRESTRICTED)) {
			transmissionMediumUsed   = tmu;
			isTransmissionMediumUsed = true;
		}
		else 
			throw new ParameterRangeInvalidException();

	}

	public boolean isTransmissionMediumUsedPresent(){
		return isTransmissionMediumUsed;
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
		return isUserToUserIndicatorsItu;
	}

	public byte[] getUserToUserInformation()
                                throws ParameterNotSetException{

		if(isUserToUserInformation == true)
			return userToUserInformation;
		else throw new ParameterNotSetException();
	}

	public void setUserToUserInformation(byte[] userToUserInfo){

		userToUserInformation   = userToUserInfo;
		isUserToUserInformation = true;
	}

	public boolean isUserToUserInformationPresent(){
		return isUserToUserInformation;
	}

	/**
    * String representation of class ConnectItuEvent
    *
    * @return    String provides description of class ConnectItuEvent
    */
        public java.lang.String toString(){
		int i;
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\nisAccessDeliveryInfo = ");
				buffer.append(isAccessDeliveryInfo);	
				buffer.append("\naccessDeliveryInfo = ");
				buffer.append(accessDeliveryInfo);	
				buffer.append("\n\nisAccessTransport = ");
				buffer.append(isAccessTransport);	
				if(isAccessTransport == true){
					buffer.append("\naccessTransport = ");
					for(i=0;i<accessTransport.length;i++)
						buffer.append(" "+Integer.toHexString((int)(accessTransport[i] & 0xFF)));
				}
				buffer.append("\n\nbwdCallIndItu = ");
				buffer.append(bwdCallIndItu);	
				buffer.append("\n\nisCallHistoryInfo = ");
				buffer.append(isCallHistoryInfo);	
				buffer.append("\ncallHistoryInfo  = ");
				buffer.append(callHistoryInfo);
				buffer.append("\n\nisCallReference = ");
				buffer.append(isCallReference);	
				buffer.append("\ncallReference = ");
				buffer.append(callReference);
				buffer.append("\n\nisConnectedNumberItu = ");
				buffer.append(isConnectedNumberItu);	
				buffer.append("\nconnectedNumberItu = ");
				buffer.append(connectedNumberItu);
				buffer.append("\n\nisEchoControlInfoItu = ");
				buffer.append(isEchoControlInfoItu);	
				buffer.append("\nechoControlInfoItu = ");
				buffer.append(echoControlInfoItu);
				buffer.append("\n\nisGenericNumberItu = ");
				buffer.append(isGenericNumberItu);	
				buffer.append("\ngenericNumberItu = ");
				buffer.append(genericNumberItu);
				buffer.append("\n\nisNotificationInd = ");
				buffer.append(isNotificationInd);	
				buffer.append("\nnotificationInd = ");
				buffer.append(notificationInd);
				buffer.append("\n\nisNwSpecificFacItu = ");
				buffer.append(isNwSpecificFacItu);	
				buffer.append("\nnwSpecificFacItu = ");
				buffer.append(nwSpecificFacItu);
				buffer.append("\n\nisOptionalBwdCallIndItu = ");
				buffer.append(isOptionalBwdCallIndItu);	
				buffer.append("\noptionalBwdCallIndItu = ");
				buffer.append(optionalBwdCallIndItu);
				buffer.append("\n\nisParamCompatibilityInfoItu = ");
				buffer.append(isParamCompatibilityInfoItu);	
				buffer.append("\nparamCompatibilityInfoItu = ");
				buffer.append(paramCompatibilityInfoItu);
				buffer.append("\n\nisRedirectionNumberItu = ");
				buffer.append(isRedirectionNumberItu);		
				buffer.append("\nredirectionNumberItu = ");
				buffer.append(redirectionNumberItu);
				buffer.append("\n\nisRedirectionNumberRest = ");
				buffer.append(isRedirectionNumberRest);	
				buffer.append("\nredirectionNumberRest = ");
				buffer.append(redirectionNumberRest);
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
				buffer.append("\n\nisTransmissionMediumUsed = ");
				buffer.append(isTransmissionMediumUsed);	
				buffer.append("\ntransmissionMediumUsed = ");
				buffer.append(transmissionMediumUsed);
				buffer.append("\n\nisUserToUserIndicatorsItu = ");
				buffer.append(isUserToUserIndicatorsItu);	
				buffer.append("\nuserToUserIndicatorsItu; = ");
				buffer.append(userToUserIndicatorsItu);
				buffer.append("\n\nisUserToUserInformation = ");
				buffer.append(isUserToUserInformation);
				if(isUserToUserInformation == true){
					buffer.append("\nuserToUserInformation = ");
					for(i=0;i<userToUserInformation.length;i++)
						buffer.append(" "+Integer.toHexString((int)(userToUserInformation[i] & 0xFF)));					
				}
				return buffer.toString();
		
		}

	
	byte                      accessDeliveryInfo;
	byte[]                    accessTransport;
	BwdCallIndItu             bwdCallIndItu;
	long                      callHistoryInfo;
	CallReference             callReference;
	ConnectedNumberItu        connectedNumberItu;
	EchoControlInfoItu        echoControlInfoItu;
	GenericNumberItu          genericNumberItu;
	boolean                   isAccessDeliveryInfo;
	boolean                   isAccessTransport;
	boolean                   isCallHistoryInfo;
	boolean                   isCallReference;
	boolean                   isConnectedNumberItu;
	boolean                   isEchoControlInfoItu;
	boolean                   isGenericNumberItu;
	boolean                   isNotificationInd;
	boolean                   isNwSpecificFacItu;
	boolean                   isOptionalBwdCallIndItu;
	boolean                   isParamCompatibilityInfoItu;
	boolean                   isRedirectionNumberItu;
	boolean                   isRedirectionNumberRest;
	boolean                   isRemoteOperations;
	boolean                   isServiceActivation;
	boolean                   isTransmissionMediumUsed;
	boolean                   isUserToUserIndicatorsItu;
	boolean                   isUserToUserInformation;
	NotificationInd           notificationInd;
	NwSpecificFacItu          nwSpecificFacItu;
	OptionalBwdCallIndItu     optionalBwdCallIndItu;
	ParamCompatibilityInfoItu paramCompatibilityInfoItu;
	RedirectionNumberItu      redirectionNumberItu;
	byte                      redirectionNumberRest;
	RemoteOperations          remoteOperations;
	byte[]                    serviceActivation;
	byte                      transmissionMediumUsed;
	UserToUserIndicatorsItu   userToUserIndicatorsItu;
	byte[]                    userToUserInformation;


	protected boolean m_bwdCallIndItuPresent = false;
	public static final byte TMR_SPEECH = 0x00; 
	public static final byte TMR_64_KBPS_UNRESTRICTED = 0x02; 
	public static final byte TMR_1920_KBPS_UNRESTRICTED = 0x0A;        
}


