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
* MODULE NAME  : $RCSfile: SetupAnsiEvent.java,v $
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

public class SetupAnsiEvent extends SetupEvent{

	public SetupAnsiEvent(java.lang.Object source,
                      SignalingPointCode dpc,
                      SignalingPointCode opc,
                      byte sls,
                      int cic,
                      byte congestionPriority,
                      boolean glareControl,
                      NatureConnInd nci,
                      FwdCallIndAnsi fci,
                      CalledPartyNumber cpn,
                      byte cat,
                      UserServiceInfo usi)
               throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority,glareControl,nci,fci,cpn,cat);
		userServiceInfo = usi;
		m_userServiceInfoPresent = true;
	}

	public void checkMandatoryParameters()
                              throws MandatoryParameterNotSetException{
		if(m_userServiceInfoPresent == true)
			super.checkMandatoryParameters();
		else throw new MandatoryParameterNotSetException();
	}
	
	public BusinessGroupAnsi getBusinessGroup()
                                   throws ParameterNotSetException{
		if(isBusinessGroupAnsi == true)
			return businessGroupAnsi;
		else throw new ParameterNotSetException();
	}

	public void setBusinessGroup(BusinessGroupAnsi bgp){
		businessGroupAnsi   = bgp;
		isBusinessGroupAnsi = true;
	}

	public boolean isBusinessGroupPresent(){
		return isBusinessGroupAnsi;
	}
	
	public CarrierIdAnsi getCarrierId()
                           throws ParameterNotSetException{
		if(isCarrierIdAnsi == true)
			return carrierIdAnsi;
		else throw new ParameterNotSetException();
	}

	public void setCarrierId(CarrierIdAnsi cid){
		carrierIdAnsi   = cid;
		isCarrierIdAnsi = true;
	}

	public boolean isCarrierIdPresent(){
		return isCarrierIdAnsi;
	}

	public short getCarrierSelInfo()
                        throws ParameterNotSetException{

		if(isCarrierSelInfo == true)
			return carrierSelInfo;
		else throw new ParameterNotSetException();
	}

	public void setCarrierSelInfo(short csi)
                       throws ParameterRangeInvalidException{
		if((csi >= CSI_NO_INDICATION) && ( csi <= CSI_IDENTIFICATION_NOT_PRESUBSCRIBED_INPUT_BY_CALLING_PARTY)) {
			carrierSelInfo   = csi;
			isCarrierSelInfo = true;
		}
		else throw new ParameterRangeInvalidException();

	}

	public boolean isCarrierSelInfoPresent(){
		return isCarrierSelInfo;
	}

	public ChargeNumberAnsi getChargeNumber()
                                 throws ParameterNotSetException{
		if(isChargeNumberAnsi == true)
			return chargeNumberAnsi;
		else throw new ParameterNotSetException();
		
	}

	public void setChargeNumber(ChargeNumberAnsi chargeNumAnsi){
		chargeNumberAnsi   = chargeNumAnsi;
		isChargeNumberAnsi = true;  
	}

	public boolean isChargeNumberPresent(){
		return isChargeNumberAnsi;
	}

	public CircuitAssignMapAnsi getCircuitAssignMap()
                                         throws ParameterNotSetException{
		if(isCircuitAssignMapAnsi == true)
			return circuitAssignMapAnsi;
		else throw new ParameterNotSetException();
	}

	public void setCircuitAssignMap(CircuitAssignMapAnsi cam){
		circuitAssignMapAnsi   = cam;
		isCircuitAssignMapAnsi = true;
	}
	
	public boolean isCircuitAssignMapPresent(){
		return isCircuitAssignMapAnsi;
	}
	
	public byte[] getEgressService()
                        throws ParameterNotSetException{
		if(isEgressService == true)
			return egressService;
		else throw new ParameterNotSetException();
	}

	public void setEgressService(byte[] es){
		egressService   = es;
		isEgressService = true; 

	}

	public boolean isEgressServicePresent(){
		return isEgressService;
	}
	
	public GenericNameAnsi getGenericName()
                               throws ParameterNotSetException{
		if(isGenericNameAnsi == true)
			return genericNameAnsi;
		else throw new ParameterNotSetException();
	}

	public void setGenericName(GenericNameAnsi genNameAnsi){
		genericNameAnsi   = genNameAnsi;
		isGenericNameAnsi = true;
	}

	public boolean isGenericNamePresent(){
		return isGenericNameAnsi;
	}

	public byte getHopCounter()
                   throws ParameterNotSetException{
		if(isHopCounter == true)
			return hopCounter;
		else throw new ParameterNotSetException();
	}

	public void setHopCounter(byte hc)
                   throws ParameterRangeInvalidException{

		if((hc >=0) && (hc <= 31)){
			hopCounter = hc;
			isHopCounter = true;
		}
		else throw new ParameterRangeInvalidException();
		
	}

	public boolean isHopCounterPresent(){
		return isHopCounter;
	}

	public InfoReqIndAnsi getInfoReqInd()
                             throws ParameterNotSetException{
		if(isInfoReqIndAnsi == true)
			return infoReqIndAnsi;
		else throw new ParameterNotSetException();
	
	}

	public void setInfoReqInd(InfoReqIndAnsi infomationReqIndAnsi){
		infoReqIndAnsi   = infomationReqIndAnsi;
		isInfoReqIndAnsi = true;
	}

	public boolean isInfoReqIndPresent(){
		return isInfoReqIndAnsi;
	}
	
	public byte[] getJurisdiction()
                       throws ParameterNotSetException{
		if(isJurisdiction == true)
			return jurisdiction;
		else throw new ParameterNotSetException();
	}
	public void setJurisdiction(byte[] jurisdict){
		jurisdiction   = jurisdict;	
		isJurisdiction = true;
	}
	public boolean isJurisdictionPresent(){
		return isJurisdiction;
	}
	
	public byte[] getNetworkTransport()
                           throws ParameterNotSetException{
		if(isNetworkTransport == true)
			return networkTransport;
		else throw new ParameterNotSetException();
	}

	public void setNetworkTransport(byte[] ntp){
		networkTransport   = ntp;
		isNetworkTransport = true;
	}

	public boolean isNetworkTransportPresent(){
		return isNetworkTransport;
	}
	
	public OperatorServicesInfoAnsi getOperatorServicesInfo()
                                                 throws ParameterNotSetException{
		if(isOperatorServicesInfoAnsi == true)
			return operatorServicesInfoAnsi;
		else throw new ParameterNotSetException();
	}
	
	public void setOperatorServicesInfo(OperatorServicesInfoAnsi osi){
		operatorServicesInfoAnsi   = osi;
		isOperatorServicesInfoAnsi = true;
	}

	public boolean isOperatorServicesInfoPresent(){
		return isOperatorServicesInfoAnsi;
	}
	public byte getOrigLineInfo()
                     throws ParameterNotSetException{
		if(isOrigLineInfo == true)
			return origLineInfo;
		else throw new ParameterNotSetException();
	}

	public void setOrigLineInfo(byte oli)
                     throws ParameterRangeInvalidException{
		/* no valid range specified in specs */
		origLineInfo   = oli;
		isOrigLineInfo = true;
	}

	public boolean isOrigLineInfoPresent(){
		return isOrigLineInfo;
	}
	
	public short getServiceCode()
                     throws ParameterNotSetException{
		if(isServiceCode == true)
			return serviceCode;
		else throw new ParameterNotSetException();
	}

	public void setServiceCode(short sc)
                    throws ParameterRangeInvalidException{
		/* no valid range specified in specs */
		serviceCode   = sc;
		isServiceCode = true;
	}

	public boolean isServiceCodePresent(){
		return isServiceCode;
	}

	public short getSpecialProcReq()
                        throws ParameterNotSetException{
		if(isSpecialProcReq == true)
			return specialProcReq;
		else throw new ParameterNotSetException();
	}
		
	public void setSpecialProcReq(short spr)
                       throws ParameterRangeInvalidException{
		/* no valid range specified in specs */
		specialProcReq   = spr;
		isSpecialProcReq = true;
	}

	public boolean isSpecialProcReqPresent(){
		return isSpecialProcReq;
	}
	
	public TransactionReqAnsi getTransactionReq()
                                     throws ParameterNotSetException{
		if(isTransactionReqAnsi == true)
			return transactionReqAnsi;
		else throw new ParameterNotSetException();
	}

	public void setTransactionReq(TransactionReqAnsi tr){
		transactionReqAnsi   = tr;
		isTransactionReqAnsi = true;
	}

	public boolean isTransactionReqPresent(){
		return isTransactionReqAnsi;
	}

	public UserServiceInfo getUserServiceInfo()
                                   throws MandatoryParameterNotSetException{
		if(m_userServiceInfoPresent == true)
			return userServiceInfo;
		else throw new MandatoryParameterNotSetException();
	}

	public void setUserServiceInfo(UserServiceInfo usi){
		userServiceInfo          = usi;
		m_userServiceInfoPresent = true;
	}


	/**
    * String representation of class SetupAnsiEvent
    *
    * @return    String provides description of class SetupAnsiEvent
    */
        public java.lang.String toString(){
		int i;
        StringBuffer buffer = new StringBuffer(500);
		        buffer.append(super.toString());
				buffer.append("\n\nisBusinessGroupAnsi = ");
				buffer.append(isBusinessGroupAnsi);
				buffer.append("\nbusinessGroupAnsi = ");
				buffer.append(businessGroupAnsi);
				buffer.append("\n\nisCarrierIdAnsi = ");
				buffer.append(isCarrierIdAnsi);
				buffer.append("\ncarrierIdAnsi = ");
				buffer.append(carrierIdAnsi);
				buffer.append("\n\nisCarrierSelInfo = ");
				buffer.append(isCarrierSelInfo);
				buffer.append("\ncarrierSelInfo = ");
				buffer.append(carrierSelInfo);
				buffer.append("\n\nisChargeNumberAnsi = ");
				buffer.append(isChargeNumberAnsi);
				buffer.append("\nchargeNumberAnsi = ");
				buffer.append(chargeNumberAnsi);
				buffer.append("\n\nisCircuitAssignMapAnsi = ");
				buffer.append(isCircuitAssignMapAnsi);
				buffer.append("\ncircuitAssignMapAnsi = ");
				buffer.append(circuitAssignMapAnsi);
				buffer.append("\n\nisEgressService = ");
				buffer.append(isEgressService);
				if(isEgressService == true){
					buffer.append("\negressService = ");
					for(i=0;i<egressService.length;i++)
						buffer.append(" "+Integer.toHexString((int)(egressService[i] & 0xFF)));
				}
				buffer.append("\n\nisGenericNameAnsi = ");
				buffer.append(isGenericNameAnsi);
				buffer.append("\ngenericNameAnsi = ");
				buffer.append(genericNameAnsi);
				buffer.append("\n\nisHopCounter = ");
				buffer.append(isHopCounter);
				buffer.append("\nhopCounter = ");
				buffer.append(hopCounter);
				buffer.append("\n\nisInfoReqIndAnsi = ");
				buffer.append(isInfoReqIndAnsi);
				buffer.append("\ninfoReqIndAnsi = ");
				buffer.append(infoReqIndAnsi);
				buffer.append("\n\nisJurisdiction = ");
				buffer.append(isJurisdiction);
				if(isJurisdiction == true){
					buffer.append("\njurisdiction = ");
					for(i=0;i<jurisdiction.length;i++)
						buffer.append(" "+Integer.toHexString((int)(jurisdiction[i] & 0xFF)));
				}
				buffer.append("\n\nisNetworkTransport = ");
				buffer.append(isNetworkTransport);
				if(isNetworkTransport == true){
					buffer.append("\nnetworkTransport = ");
					for(i=0;i<networkTransport.length;i++)
						buffer.append(" "+Integer.toHexString((int)(networkTransport[i] & 0xFF)));		
				}
				buffer.append("\n\nisOperatorServicesInfoAnsi = ");
				buffer.append(isOperatorServicesInfoAnsi);
				buffer.append("\noperatorServicesInfoAnsi = ");
				buffer.append(operatorServicesInfoAnsi);
				buffer.append("\n\nisOrigLineInfo = ");
				buffer.append(isOrigLineInfo);
				buffer.append("\norigLineInfo = ");
				buffer.append(origLineInfo);
				buffer.append("\n\nisServiceCode = ");
				buffer.append(isServiceCode);
				buffer.append("\nserviceCode = ");
				buffer.append(serviceCode);
				buffer.append("\n\nisSpecialProcReq = ");
				buffer.append(isSpecialProcReq);
				buffer.append("\nspecialProcReq = ");
				buffer.append(specialProcReq);
				buffer.append("\n\nisTransactionReqAnsi = ");
				buffer.append(isTransactionReqAnsi);
				buffer.append("\ntransactionReqAnsi = ");
				buffer.append(transactionReqAnsi);
				buffer.append("\n\nuserServiceInfo = ");
				buffer.append(userServiceInfo);
				return buffer.toString();
		
		}



	
	BusinessGroupAnsi        businessGroupAnsi;
	CarrierIdAnsi            carrierIdAnsi;
	short                    carrierSelInfo;
	ChargeNumberAnsi         chargeNumberAnsi;
	CircuitAssignMapAnsi     circuitAssignMapAnsi;
	byte[]                   egressService;
	GenericNameAnsi          genericNameAnsi;
	byte                     hopCounter;
	InfoReqIndAnsi           infoReqIndAnsi;
	boolean                  isBusinessGroupAnsi;
	boolean                  isCarrierIdAnsi;
	boolean                  isCarrierSelInfo;
	boolean                  isChargeNumberAnsi;
	boolean                  isCircuitAssignMapAnsi;
	boolean                  isEgressService;
	boolean                  isGenericNameAnsi;
	boolean                  isHopCounter;
	boolean                  isInfoReqIndAnsi;
	boolean                  isJurisdiction;
	boolean                  isNetworkTransport;
	boolean                  isOperatorServicesInfoAnsi;
	boolean                  isOrigLineInfo;
	boolean                  isServiceCode;
	boolean                  isSpecialProcReq;
	boolean                  isTransactionReqAnsi;
	byte[]                   jurisdiction;
	byte[]                   networkTransport;
	OperatorServicesInfoAnsi operatorServicesInfoAnsi;
	byte                     origLineInfo;
	short                    serviceCode;
	short                    specialProcReq;
	TransactionReqAnsi       transactionReqAnsi;
	UserServiceInfo          userServiceInfo;

	protected boolean m_userServiceInfoPresent     = false;

	public static final byte CSI_NO_INDICATION = 0x00; 
	public static final byte CSI_IDENTIFICATION_NOT_PRESUBSCRIBED_INPUT_BY_CALLING_PARTY = 0x04; 

	
}	


