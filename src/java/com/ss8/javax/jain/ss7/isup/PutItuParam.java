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
* DATE         : $Date: 2008/05/16 12:23:20 $
* 
* MODULE NAME  : $RCSfile: PutItuParam.java,v $
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
package com.ss8.javax.jain.ss7.isup;

import java.util.*;

import javax.jain.*;
import javax.jain.ss7.*;
import javax.jain.ss7.isup.*;
import javax.jain.ss7.isup.itu.*;

public class  PutItuParam{

	static final int NONE               = IsupConstants.NOT_SET;
	static final int FIXED_PARAMETER    = 1;
	static final int VARIABLE_PARAMETER = 2;
	static final int OPTIONAL_PARAMETER = 3;

	public static byte[] putParam(IsupEvent event) 
						throws ParameterNotSetException{

		int index = 1;
		int                          PropagationDelayCounter	= NONE;
		long                         chi						= NONE;
		byte[]                       accessTransport			= null;
		CUGInterlockCodeItu          cug						= null; 
		LocationNumberItu            locno						= null;		
		NotificationInd              nind						= null;
		NwSpecificFacItu             nwcfac						= null;
		OptionalFwdCallIndItu        optfwdcallind				= null;
		byte[]                       OrigISCPointCode			= null;
		byte[]                       ServiceActivation			= null;
		byte[]                       uuin						= null;
		ParamCompatibilityInfoItu    pci						= null;	
		UserServiceInfo              usi						= null;
		UserToUserIndicatorsItu      uuiitu						= null;
		UserToUserIndicators         uui						= null;
		CallReference                cref						= null;
		CallingPartyNumber           cgpn						= null;
		ConReq                       conr						= null;
		GenericDigits                gnd						= null;
		GenericNumber                gad						= null;
		GenericNumberItu             gaditu						= null;
		Precedence                   mlpp						= null;
		OrigCalledNumber             ocdn						= null;
		RedirectingNumber            rdtg						= null;
		RedirectionInfo              redi						= null;
		RemoteOperations             rop						= null;
		TransitNwSel                 xns						= null;
		UserServiceInfoPrime         usip						= null;
		BwdCallInd                   bci						= null;
		CallDiversionInfoItu         cdvi						= null;
		CauseInd                     csi						= null;
		ConnectedNumberItu           conn						= null;
		EchoControlInfoItu           eci						= null;
		MCIDReqIndItu                mrq						= null;
		MCIDRespIndItu               mrs						= null;
		MessageCompatibilityInfoItu  mcoi						= null;
		OptionalBwdCallInd           obi						= null;
		OptionalBwdCallIndItu        obiitu						= null;
		RedirectionNumberItu         redn						= null;
		SignalingPointCode           spc						= null;

		byte cug_len, locno_len, nind_len,
nwcfac_len, pci_len, usi_len,
cref_len, cgpn_len, conr_len, gnd_len, gad_len, gaditu_len, mlpp_len,
ocdn_len, rdtg_len, redi_len, rop_len, xns_len, usip_len, bci_len,
csi_len, conn_len, mcoi_len, obiitu_len, redn_len, spc_len; 

		byte[] par_array = ByteArray.getByteArray(256);	
		
		try{
			/* SETUP EVENT */
			if(event instanceof SetupEvent){
				if(((SetupEvent)event).isAccessTransportPresent()){					
						accessTransport    = ((SetupEvent)event).getAccessTransport();
						par_array[index++] = IsupParameterValues.ISUP_PMT_AXP;
						par_array[index++] = (byte)accessTransport.length;
						par_array[index++] = OPTIONAL_PARAMETER;
						System.arraycopy(accessTransport,0,
								par_array,index,accessTransport.length);
						index+= accessTransport.length;								
				}	
				if (((SetupEvent)event).isCallReferencePresent()) {
					cref               = ((SetupEvent)event).getCallReference();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CRF;
					par_array[index++] = cref_len = (byte)cref.flatCallReference().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cref.flatCallReference(),0,
						par_array,index,cref_len);
					index+=	cref_len;						
				}		
				if (((SetupEvent)event).isCallingPartyNumberPresent()) {
					cgpn               = ((SetupEvent)event).getCallingPartyNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CGPN;
					par_array[index++] = cgpn_len = (byte)cgpn.flatCallingPartyNumber().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cgpn.flatCallingPartyNumber(),0,
						par_array,index,cgpn_len);
					index+=	cgpn_len;		
				}
				if (((SetupEvent)event).isConReqPresent()) {
					conr               = ((SetupEvent)event).getConReq();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CONR;
					par_array[index++] = conr_len = (byte)conr.flatConReq().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(conr.flatConReq(),0,
						par_array,index,conr_len);
					index+=	conr_len;		
				}
				if (((SetupEvent)event).isGenericDigitsPresent()) {
					gnd               = ((SetupEvent)event).getGenericDigits();
					par_array[index++] = IsupParameterValues.ISUP_PMT_GND;
					par_array[index++] = gnd_len = (byte)gnd.flatGenericDigits().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(gnd.flatGenericDigits(),0,
								par_array,index,gnd_len);
					index += gnd_len;				
				}
				if (((SetupEvent)event).isGenericNumberPresent()) {				
					gad               = ((SetupEvent)event).getGenericNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_GAD;
					par_array[index++] = gad_len = (byte)gad.flatGenericNumber().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(gad.flatGenericNumber(),0,
								par_array,index,gad_len);
				
					index += gad_len;				
				}		
				if (((SetupEvent)event).isMLPPprecedencePresent()) {
					mlpp               = ((SetupEvent)event).getMLPPprecedence();
					par_array[index++] = IsupParameterValues.ISUP_PMT_MLPP;
					par_array[index++] = mlpp_len = (byte)mlpp.flatPrecedence().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(mlpp.flatPrecedence(),0,
								par_array,index,mlpp_len);
				
					index += mlpp_len;				
				}
				if (((SetupEvent)event).isOrigCalledNumberPresent()) {
					ocdn               = ((SetupEvent)event).getOrigCalledNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_OCDN;
					par_array[index++] = ocdn_len = (byte)ocdn.flatOrigCalledNumber().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(ocdn.flatOrigCalledNumber(),0,
								par_array,index,ocdn_len);
				
					index += ocdn_len;				
				}
				if (((SetupEvent)event).isRedirectingNumberPresent()){
					rdtg               = ((SetupEvent)event).getRedirectingNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_RDTG;
					par_array[index++] = rdtg_len = (byte)rdtg.flatRedirectingNumber().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(rdtg.flatRedirectingNumber(),0,
								par_array,index,rdtg_len);
				
					index += rdtg_len;			
				}			
				if (((SetupEvent)event).isTransitNwSelPresent()) {				
					xns                = ((SetupEvent)event).getTransitNwSel();
					par_array[index++] = IsupParameterValues.ISUP_PMT_XNS;
					par_array[index++] = xns_len = (byte)xns.flatTransitNwSel().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(xns.flatTransitNwSel(),0,
								par_array,index,xns_len);
				
					index += xns_len;		
				}				
				if (((SetupEvent)event).isUserServiceInfoPrimePresent()) {
					usip               = ((SetupEvent)event).getUserServiceInfoPrime();
					par_array[index++] = IsupParameterValues.ISUP_PMT_USIP;
					par_array[index++] = usip_len = (byte)usip.flatUserServiceInfoPrime().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(usip.flatUserServiceInfoPrime(),0,
								par_array,index,usip_len);
					index+= usip_len;				
				}	
				if (((SetupEvent)event).isRedirectionInfoPresent()) {
					redi                  = ((SetupEvent)event).getRedirectionInfo();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_REDI;
					par_array[index++]    = redi_len = (byte)redi.flatRedirectionInfo().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(redi.flatRedirectionInfo(),0,
						par_array,index,redi_len);
					index+=	redi_len;						
				}	
				if (((SetupEvent)event).isRemoteOperationsPresent()) {
					rop                   = ((SetupEvent)event).getRemoteOperations();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_ROP;
					par_array[index++]    = rop_len = (byte)rop.flatRemoteOperations().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(rop.flatRemoteOperations(),0,
						par_array,index,rop_len);
					index+=	rop_len;						
				}	
				if (((SetupEvent)event).isServiceActivationPresent()) {
					ServiceActivation     = ((SetupEvent)event).getServiceActivation();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_SAC; 
					par_array[index++]    = (byte)ServiceActivation.length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(ServiceActivation,0,
								par_array,index,ServiceActivation.length);
					index+= ServiceActivation.length;	
				}	
				if (((SetupEvent)event).isUserToUserInformationPresent()) {	
					uuin                  = ((SetupEvent)event).getUserToUserInformation();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_UUIN; 
					par_array[index++]    = (byte)uuin.length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(uuin,0,
								par_array,index,uuin.length);
					index+= uuin.length;			
				}
				if (((SetupItuEvent)event).isCUGInterlockCodePresent()) {		
					cug                = ((SetupItuEvent)event).getCUGInterlockCode();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CUGC;
					par_array[index++] = cug_len = (byte)cug.flatCUGInterlockCodeItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cug.flatCUGInterlockCodeItu(),0,
								par_array,index,cug_len);	
					index += cug_len;
				}			
				if (((SetupItuEvent)event).isLocationNumberPresent()) {
					locno              = ((SetupItuEvent)event).getLocationNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_LON;
					par_array[index++] = locno_len = (byte)locno.flatLocationNumberItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(locno.flatLocationNumberItu(),0,
								par_array,index,locno_len);
				
					index += locno_len;				
				}			
				if (((SetupItuEvent)event).isOptionalFwdCallIndPresent()) {
					optfwdcallind         = ((SetupItuEvent)event).getOptionalFwdCallInd();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_OFI;
					par_array[index++]    = 1;
					par_array[index++]    = OPTIONAL_PARAMETER;
					par_array[index++]    = optfwdcallind.flatOptionalFwdCallIndItu();
				}			
				if (((SetupItuEvent)event).isOrigISCPointCodePresent()) {
					OrigISCPointCode      = ((SetupItuEvent)event).getOrigISCPointCode();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_OISC; 
					par_array[index++]    = (byte)OrigISCPointCode.length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(OrigISCPointCode,0,
								par_array,index,OrigISCPointCode.length);
					index+= OrigISCPointCode.length;	
				}			
				if (((SetupItuEvent)event).isPropagationDelayCounterPresent()) {
					PropagationDelayCounter  = ((SetupItuEvent)event).getPropagationDelayCounter();
					par_array[index++]       = IsupParameterValues.ISUP_PMT_PDC;
					par_array[index++]       = 4;
					par_array[index++]		 = OPTIONAL_PARAMETER;
					for(int j=0;j<4;j++)
					{
						par_array[index++] = (byte)(PropagationDelayCounter >> (8*j) & 0x000000FF);
					}						
				}			
				if(((SetupItuEvent)event).isTransmissionMediumReqPrimePresent()){				
					par_array[index++]         = IsupParameterValues.ISUP_PMT_XMRP;
					par_array[index++]         = 1;
					par_array[index++]         = OPTIONAL_PARAMETER;
					par_array[index++]         = ((SetupItuEvent)event).getTransmissionMediumReqPrime();
				}			
				if (((SetupItuEvent)event).isUserServiceInfoPresent()) {
					usi                = ((SetupItuEvent)event).getUserServiceInfo();
					par_array[index++] = IsupParameterValues.ISUP_PMT_USI;
					par_array[index++] = usi_len = (byte)usi.flatUserServiceInfo().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(usi.flatUserServiceInfo(),0,
								par_array,index,usi_len);
					index+= usi_len;				
				}
				if (((SetupItuEvent)event).isNotificationIndPresent()) {
					nind               = ((SetupItuEvent)event).getNotificationInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_GNNO;
					par_array[index++] = nind_len = (byte)nind.flatNotificationInd().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(nind.flatNotificationInd(),0,
								par_array,index,nind_len);
				
					index += nind_len;				
				}
				if (((SetupItuEvent)event).isNwSpecificFacPresent()) {
					nwcfac             = ((SetupItuEvent)event).getNwSpecificFac();
					par_array[index++] = IsupParameterValues.ISUP_PMT_NSF;					
					par_array[index++] = nwcfac_len = (byte)nwcfac.flatNwSpecificFacItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;					
					System.arraycopy(nwcfac.flatNwSpecificFacItu(),0,
								par_array,index,nwcfac_len);
					index+= nwcfac_len;
					
				}
				if (((SetupItuEvent)event).isParamCompatibilityInfoPresent()) {
					pci                   = ((SetupItuEvent)event).getParamCompatibilityInfo();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_PCI;
					par_array[index++]    = pci_len = (byte)pci.flatParamCompatibilityInfoItu().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(pci.flatParamCompatibilityInfoItu(),0,
						par_array,index,pci_len);
					index+=	pci_len;						
				}	
				if (((SetupItuEvent)event).isUserToUserIndicatorsPresent()) {
					uuiitu             = ((SetupItuEvent)event).getUserToUserIndicators();
					par_array[index++] = IsupParameterValues.ISUP_PMT_UUIC;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = uuiitu.flatUserToUserIndicatorsItu();	
				}				
				
			}
			/* ALERT EVENT */
			else if(event instanceof AlertEvent) {
				if(((AlertEvent)event).isAccessTransportPresent()){					
						accessTransport    = ((AlertEvent)event).getAccessTransport();
						par_array[index++] = IsupParameterValues.ISUP_PMT_AXP;
						par_array[index++] = (byte)accessTransport.length;
						par_array[index++] = OPTIONAL_PARAMETER;
						System.arraycopy(accessTransport,0,
								par_array,index,accessTransport.length);
						index+= accessTransport.length;						
				}			
				if (((AlertEvent)event).isCallReferencePresent()) {
					cref               = ((AlertEvent)event).getCallReference();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CRF;
					par_array[index++] = cref_len = (byte)cref.flatCallReference().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cref.flatCallReference(),0,
						par_array,index,cref_len);
					index+=	cref_len;		
				}		
				if (((AlertEvent)event).isCauseIndPresent()) {
					csi                = ((AlertEvent)event).getCauseInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CSI;
					par_array[index++] = csi_len = (byte)csi.flatCauseInd().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(csi.flatCauseInd(),0,
						par_array,index,csi_len);
					index+=	csi_len;		
				}
				if (((AlertEvent)event).isNotificationIndPresent()) {
					nind               = ((AlertEvent)event).getNotificationInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_GNNO;
					par_array[index++] = nind_len = (byte)nind.flatNotificationInd().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(nind.flatNotificationInd(),0,
								par_array,index,nind_len);
				
					index += nind_len;				
				}
				if (((AlertEvent)event).isOptionalBwdCallIndPresent()) {
					obiitu             = (OptionalBwdCallIndItu)(((AlertEvent)event).getOptionalBwdCallInd());
					par_array[index++] = IsupParameterValues.ISUP_PMT_OBI;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = obiitu.flatOptionalBwdCallIndItu();		
				}
				if (((AlertEvent)event).isRemoteOperationsPresent()) {
					rop                   = ((AlertEvent)event).getRemoteOperations();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_ROP;
					par_array[index++]    = rop_len = (byte)rop.flatRemoteOperations().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(rop.flatRemoteOperations(),0,
						par_array,index,rop_len);
					index+=	rop_len;						
				}	
				if (((AlertEvent)event).isServiceActivationPresent()) {
					ServiceActivation     = ((AlertEvent)event).getServiceActivation();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_SAC; 
					par_array[index++]    = (byte)ServiceActivation.length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(ServiceActivation,0,
								par_array,index,ServiceActivation.length);
					index+= ServiceActivation.length;			
				}	
				if(((AlertEvent)event).isTransmissionMediumUsedPresent()){
					par_array[index++]         = IsupParameterValues.ISUP_PMT_XMU;
					par_array[index++]         = 1;
					par_array[index++]         = OPTIONAL_PARAMETER;
					par_array[index++]         = ((AlertEvent)event).getTransmissionMediumUsed();
				}
				if (((AlertEvent)event).isUserToUserInformationPresent()) {	
					uuin                  = ((AlertEvent)event).getUserToUserInformation();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_UUIN; 
					par_array[index++]    = (byte)uuin.length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(uuin,0,
								par_array,index,uuin.length);
					index+= uuin.length;			
				}
				if (((AlertEvent)event).isUserToUserIndicatorsPresent()) {				
					uui                = ((AlertEvent)event).getUserToUserIndicators();
					par_array[index++] = IsupParameterValues.ISUP_PMT_UUIC;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = uui.flatUserToUserIndicators();	
				}			
				if(((AlertItuEvent)event).isAccessDeliveryInfoPresent()){
					par_array[index++] = IsupParameterValues.ISUP_PMT_ADI;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = ((AlertItuEvent)event).getAccessDeliveryInfo();
				}
				if(((AlertItuEvent)event).isRedirectionNumberPresent()){
						redn               = ((AlertItuEvent)event).getRedirectionNumber();
						par_array[index++] = IsupParameterValues.ISUP_PMT_REDN;
						par_array[index++] = redn_len = (byte)redn.flatRedirectionNumberItu().length;
						par_array[index++] = OPTIONAL_PARAMETER;
						System.arraycopy(redn.flatRedirectionNumberItu(),0,
							par_array,index,redn_len);
						index+= redn_len;							
				}			
				if(((AlertItuEvent)event).isRedirectionNumberRestPresent()){
						par_array[index++] = IsupParameterValues.ISUP_PMT_RNR;
						par_array[index++] = 1;
						par_array[index++] = OPTIONAL_PARAMETER;
						par_array[index++] = ((AlertItuEvent)event).getRedirectionNumberRest();				
				}
				if(((AlertItuEvent)event).isCallDiversionInfoPresent()){
					cdvi               = ((AlertItuEvent)event).getCallDiversionInfo();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CDVI;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = cdvi.flatCallDiversionInfoItu();					
				}		
				if (((AlertItuEvent)event).isEchoControlInfoPresent()) {
					eci                = ((AlertItuEvent)event).getEchoControlInfo();
					par_array[index++] = IsupParameterValues.ISUP_PMT_ECI;					
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = eci.flatEchoControlInfoItu();			
				}
				if (((AlertItuEvent)event).isNwSpecificFacPresent()) {
					nwcfac             = ((AlertItuEvent)event).getNwSpecificFac();
					par_array[index++] = IsupParameterValues.ISUP_PMT_NSF;
					par_array[index++] = nwcfac_len = (byte)nwcfac.flatNwSpecificFacItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(nwcfac.flatNwSpecificFacItu(),0,
								par_array,index,nwcfac_len);
					index+= nwcfac_len;
				}
				if (((AlertItuEvent)event).isParamCompatibilityInfoPresent()) {
					pci                   = ((AlertItuEvent)event).getParamCompatibilityInfo();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_PCI;
					par_array[index++]    = pci_len = (byte)pci.flatParamCompatibilityInfoItu().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(pci.flatParamCompatibilityInfoItu(),0,
						par_array,index,pci_len);
					index+=	pci_len;						
				}					
			}
			/* ANSWER EVENT */
			else if(event instanceof AnswerEvent){
				if(((AnswerEvent)event).isAccessTransportPresent()){					
					accessTransport    = ((AnswerEvent)event).getAccessTransport();
					par_array[index++] = IsupParameterValues.ISUP_PMT_AXP;
					par_array[index++] = (byte)accessTransport.length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(accessTransport,0,
								par_array,index,accessTransport.length);
					index+= accessTransport.length;						
				}	
				if(((AnswerEvent)event).isBwdCallIndPresent()){
					bci                = ((AnswerEvent)event).getBwdCallInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_BACI;
					par_array[index++] = bci_len = (byte)bci.flatBwdCallInd().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(bci.flatBwdCallInd(),0,
							par_array,index,bci_len);
					index+= bci_len;	
				}	
				if (((AnswerEvent)event).isCallReferencePresent()) {
					cref               = ((AnswerEvent)event).getCallReference();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CRF;
					par_array[index++] = cref_len = (byte)cref.flatCallReference().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cref.flatCallReference(),0,
						par_array,index,cref_len);
					index+=	cref_len;						
				}		
				if (((AnswerEvent)event).isNotificationIndPresent()) {
					nind               = ((AnswerEvent)event).getNotificationInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_GNNO;
					par_array[index++] = nind_len = (byte)nind.flatNotificationInd().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(nind.flatNotificationInd(),0,
								par_array,index,nind_len);
				
					index += nind_len;				
				}
				if (((AnswerEvent)event).isOptionalBwdCallIndPresent()) {
					obi                = ((AnswerEvent)event).getOptionalBwdCallInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_OBI;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = obi.flatOptionalBwdCallInd();											
				}
				if (((AnswerEvent)event).isRemoteOperationsPresent()) {
					rop                   = ((AnswerEvent)event).getRemoteOperations();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_ROP;
					par_array[index++]    = rop_len = (byte)rop.flatRemoteOperations().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(rop.flatRemoteOperations(),0,
						par_array,index,rop_len);
					index+=	rop_len;						
				}	
				if (((AnswerEvent)event).isServiceActivationPresent()) {
					ServiceActivation     = ((AnswerEvent)event).getServiceActivation();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_SAC; 
					par_array[index++]    = (byte)ServiceActivation.length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(ServiceActivation,0,
								par_array,index,ServiceActivation.length);
					index+= ServiceActivation.length;			
				}	
				if(((AnswerEvent)event).isTransmissionMediumUsedPresent()){
					par_array[index++]         = IsupParameterValues.ISUP_PMT_XMU;
					par_array[index++]         = 1;
					par_array[index++]         = OPTIONAL_PARAMETER;
					par_array[index++]         = ((AnswerEvent)event).getTransmissionMediumUsed();
				}
				if (((AnswerEvent)event).isUserToUserIndicatorsPresent()) {
					uui                = ((AnswerEvent)event).getUserToUserIndicators();
					par_array[index++] = IsupParameterValues.ISUP_PMT_UUIC;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = uui.flatUserToUserIndicators();	
				}	
				if (((AnswerEvent)event).isUserToUserInformationPresent()) {	
					uuin                  = ((AnswerEvent)event).getUserToUserInformation();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_UUIN; 
					par_array[index++]    = (byte)uuin.length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(uuin,0,
								par_array,index,uuin.length);
					index+= uuin.length;			
				}
				if(((AnswerItuEvent)event).isAccessDeliveryInfoPresent()){
						par_array[index++] = IsupParameterValues.ISUP_PMT_ADI;
						par_array[index++] = 1;
						par_array[index++] = OPTIONAL_PARAMETER;
						par_array[index++] = ((AnswerItuEvent)event).getAccessDeliveryInfo();
				}
				if(((AnswerItuEvent)event).isCallHistoryInfoPresent()){
					chi                = ((AnswerItuEvent)event).getCallHistoryInfo();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CHI;
					par_array[index++] = 8;
					par_array[index++] = OPTIONAL_PARAMETER;
					for(int j=0;j<8;j++)
					{
						par_array[index++] = (byte)(chi >> (8*j) & 0x000000FF);
					}								
				}		
				if (((AnswerItuEvent)event).isConnectedNumberPresent()) {
					conn               = ((AnswerItuEvent)event).getConnectedNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CONN;
					par_array[index++] = conn_len = (byte)conn.flatConnectedNumberItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(conn.flatConnectedNumberItu(),0,
						par_array,index,conn_len);
					index+=	conn_len;		
				}
				if (((AnswerItuEvent)event).isEchoControlInfoPresent()) {
					eci                = ((AnswerItuEvent)event).getEchoControlInfo();
					par_array[index++] = IsupParameterValues.ISUP_PMT_ECI;					
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = eci.flatEchoControlInfoItu();			
				}
				if (((AnswerItuEvent)event).isGenericNumberPresent()) {
					gaditu             = ((AnswerItuEvent)event).getGenericNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_GAD;
					par_array[index++] = gaditu_len = (byte)gaditu.flatGenericNumberItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(gaditu.flatGenericNumberItu(),0,
								par_array,index,gaditu_len);
				
					index += gaditu_len;				
				}
				if (((AnswerItuEvent)event).isNwSpecificFacPresent()) {
					nwcfac             = ((AnswerItuEvent)event).getNwSpecificFac();
					par_array[index++] = IsupParameterValues.ISUP_PMT_NSF;
					par_array[index++] = nwcfac_len = (byte)nwcfac.flatNwSpecificFacItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(nwcfac.flatNwSpecificFacItu(),0,
								par_array,index,nwcfac_len);
					index+= nwcfac_len;
				}
				if (((AnswerItuEvent)event).isParamCompatibilityInfoPresent()) {
					pci                   = ((AnswerItuEvent)event).getParamCompatibilityInfo();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_PCI;
					par_array[index++]    = pci_len = (byte)pci.flatParamCompatibilityInfoItu().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(pci.flatParamCompatibilityInfoItu(),0,
						par_array,index,pci_len);
					index+=	pci_len;						
				}	
				if(((AnswerItuEvent)event).isRedirectionNumberPresent()){
						redn               = ((AnswerItuEvent)event).getRedirectionNumber();
						par_array[index++] = IsupParameterValues.ISUP_PMT_REDN;
						par_array[index++] = redn_len = (byte)redn.flatRedirectionNumberItu().length;
						par_array[index++] = OPTIONAL_PARAMETER;
						System.arraycopy(redn.flatRedirectionNumberItu(),0,
							par_array,index,redn_len);
						index+= redn_len;							
				}
				if(((AnswerItuEvent)event).isRedirectionNumberRestPresent()){
						par_array[index++] = IsupParameterValues.ISUP_PMT_RNR;
						par_array[index++] = 1;
						par_array[index++] = OPTIONAL_PARAMETER;
						par_array[index++] = ((AnswerItuEvent)event).getRedirectionNumberRest();
				}						

			}
			/* CALL PROGRESS EVENT */
			else if(event instanceof CallProgressEvent){
				if(((CallProgressEvent)event).isAccessTransportPresent()){					
					accessTransport    = ((CallProgressEvent)event).getAccessTransport();
					par_array[index++] = IsupParameterValues.ISUP_PMT_AXP;
					par_array[index++] = (byte)accessTransport.length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(accessTransport,0,
								par_array,index,accessTransport.length);
					index+= accessTransport.length;						
				}					
				if(((CallProgressEvent)event).isBwdCallIndPresent()){
					bci                = ((CallProgressEvent)event).getBwdCallInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_BACI;
					par_array[index++] = bci_len = (byte)bci.flatBwdCallInd().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(bci.flatBwdCallInd(),0,
							par_array,index,bci_len);
					index+= bci_len;	
				}	
				if (((CallProgressEvent)event).isCallReferencePresent()) {
					cref               = ((CallProgressEvent)event).getCallReference();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CRF;
					par_array[index++] = cref_len = (byte)cref.flatCallReference().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cref.flatCallReference(),0,
						par_array,index,cref_len);
					index+=	cref_len;		
				}			
				if (((CallProgressEvent)event).isCauseIndPresent()) {
					csi                = ((CallProgressEvent)event).getCauseInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CSI;
					par_array[index++] = csi_len = (byte)csi.flatCauseInd().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(csi.flatCauseInd(),0,
						par_array,index,csi_len);
					index+=	csi_len;		
				}
				if (((CallProgressEvent)event).isNotificationIndPresent()) {
					nind               = ((CallProgressEvent)event).getNotificationInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_GNNO;
					par_array[index++] = nind_len = (byte)nind.flatNotificationInd().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(nind.flatNotificationInd(),0,
								par_array,index,nind_len);
				
					index += nind_len;				
				}			
				if (((CallProgressEvent)event).isOptionalBwdCallIndPresent()) {
					obi                = ((CallProgressEvent)event).getOptionalBwdCallInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_OBI;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = obi.flatOptionalBwdCallInd();											
				}
				if (((CallProgressEvent)event).isRemoteOperationsPresent()) {
					rop                   = ((CallProgressEvent)event).getRemoteOperations();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_ROP;
					par_array[index++]    = rop_len = (byte)rop.flatRemoteOperations().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(rop.flatRemoteOperations(),0,
						par_array,index,rop_len);
					index+=	rop_len;						
				}
				if (((CallProgressEvent)event).isServiceActivationPresent()) {
					ServiceActivation     = ((CallProgressEvent)event).getServiceActivation();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_SAC; 
					par_array[index++]    = (byte)ServiceActivation.length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(ServiceActivation,0,
								par_array,index,ServiceActivation.length);
					index+= ServiceActivation.length;	
				}	
				if(((CallProgressEvent)event).isTransmissionMediumUsedPresent()){
					par_array[index++]         = IsupParameterValues.ISUP_PMT_XMU;
					par_array[index++]         = 1;
					par_array[index++]         = OPTIONAL_PARAMETER;
					par_array[index++]         = ((CallProgressEvent)event).getTransmissionMediumUsed();
				}
				if (((CallProgressEvent)event).isUserToUserIndicatorsPresent()) {
					uui                = ((CallProgressEvent)event).getUserToUserIndicators();
					par_array[index++] = IsupParameterValues.ISUP_PMT_UUIC;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = uui.flatUserToUserIndicators();	
				}			
				if (((CallProgressEvent)event).isUserToUserInformationPresent()) {	
					uuin                  = ((CallProgressEvent)event).getUserToUserInformation();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_UUIN; 
					par_array[index++]    = (byte)uuin.length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(uuin,0,
								par_array,index,uuin.length);
					index+= uuin.length;	
				}
				if(((CallProgressItuEvent)event).isAccessDeliveryInfoPresent()){
					par_array[index++] = IsupParameterValues.ISUP_PMT_ADI;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = ((CallProgressItuEvent)event).getAccessDeliveryInfo();
				}
				if(((CallProgressItuEvent)event).isCallDiversionInfoPresent()){
					cdvi               = ((CallProgressItuEvent)event).getCallDiversionInfo();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CDVI;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = cdvi.flatCallDiversionInfoItu();					
				}	
				if (((CallProgressItuEvent)event).isNwSpecificFacPresent()) {
				
					nwcfac             = ((CallProgressItuEvent)event).getNwSpecificFac();
					par_array[index++] = IsupParameterValues.ISUP_PMT_NSF;
					par_array[index++] = nwcfac_len = (byte)nwcfac.flatNwSpecificFacItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(nwcfac.flatNwSpecificFacItu(),0,
								par_array,index,nwcfac_len);
					index+= nwcfac_len;
				}
				if (((CallProgressItuEvent)event).isParamCompatibilityInfoPresent()) {
					pci                   = ((CallProgressItuEvent)event).getParamCompatibilityInfo();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_PCI;
					par_array[index++]    = pci_len = (byte)pci.flatParamCompatibilityInfoItu().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(pci.flatParamCompatibilityInfoItu(),0,
						par_array,index,pci_len);
					index+=	pci_len;						
				}	
				if(((CallProgressItuEvent)event).isRedirectionNumberPresent()){
					redn               = ((CallProgressItuEvent)event).getRedirectionNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_REDN;
					par_array[index++] = redn_len = (byte)redn.flatRedirectionNumberItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(redn.flatRedirectionNumberItu(),0,
							par_array,index,redn_len);
					index+= redn_len;											
				}
				if(((CallProgressItuEvent)event).isRedirectionNumberRestPresent()){
					par_array[index++] = IsupParameterValues.ISUP_PMT_RNR;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = ((CallProgressItuEvent)event).getRedirectionNumberRest();
				}
			}
			/* CONNECT ITU EVENT */
			else if(event instanceof ConnectItuEvent){
				if(((ConnectItuEvent)event).isAccessDeliveryInfoPresent()){
					par_array[index++] = IsupParameterValues.ISUP_PMT_ADI;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = ((ConnectItuEvent)event).getAccessDeliveryInfo();				
				}
				if(((ConnectItuEvent)event).isAccessTransportPresent()){
					accessTransport    = ((ConnectItuEvent)event).getAccessTransport();
					par_array[index++] = IsupParameterValues.ISUP_PMT_AXP;
					par_array[index++] = (byte)accessTransport.length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(accessTransport,0,
								par_array,index,accessTransport.length);
					index+= accessTransport.length;	
				}			
				if(((ConnectItuEvent)event).isCallHistoryInfoPresent()){
					chi                = ((ConnectItuEvent)event).getCallHistoryInfo();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CHI;
					par_array[index++] = 8;
					par_array[index++] = OPTIONAL_PARAMETER;
					for(int j=0;j<8;j++)
					{
						par_array[index++] = (byte)(chi >> (8*j) & 0x000000FF);
					}								
				}		
				if (((ConnectItuEvent)event).isCallReferencePresent()) {
					cref               = ((ConnectItuEvent)event).getCallReference();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CRF;
					par_array[index++] = cref_len = (byte)cref.flatCallReference().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cref.flatCallReference(),0,
						par_array,index,cref_len);
					index+=	cref_len;		
				}			
				if (((ConnectItuEvent)event).isConnectedNumberPresent()) {
					conn               = ((ConnectItuEvent)event).getConnectedNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CONN;
					par_array[index++] = conn_len = (byte)conn.flatConnectedNumberItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(conn.flatConnectedNumberItu(),0,
						par_array,index,conn_len);
					index+=	conn_len;		
				}
				if (((ConnectItuEvent)event).isEchoControlInfoPresent()) {
					eci                = ((ConnectItuEvent)event).getEchoControlInfo();
					par_array[index++] = IsupParameterValues.ISUP_PMT_ECI;					
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = eci.flatEchoControlInfoItu();			
				}
				if (((ConnectItuEvent)event).isNotificationIndPresent()) {
					nind               = ((ConnectItuEvent)event).getNotificationInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_GNNO;
					par_array[index++] = nind_len = (byte)nind.flatNotificationInd().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(nind.flatNotificationInd(),0,
								par_array,index,nind_len);
				
					index += nind_len;				
				}
				if (((ConnectItuEvent)event).isGenericNumberPresent()) {
					gaditu             = ((ConnectItuEvent)event).getGenericNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_GAD;
					par_array[index++] = gaditu_len = (byte)gaditu.flatGenericNumberItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(gaditu.flatGenericNumberItu(),0,
								par_array,index,gaditu_len);
				
					index += gaditu_len;				
				}
				if (((ConnectItuEvent)event).isNwSpecificFacPresent()) {
					nwcfac             = ((ConnectItuEvent)event).getNwSpecificFac();
					par_array[index++] = IsupParameterValues.ISUP_PMT_NSF;
					par_array[index++] = nwcfac_len = (byte)nwcfac.flatNwSpecificFacItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(nwcfac.flatNwSpecificFacItu(),0,
								par_array,index,nwcfac_len);
					index+= nwcfac_len;
				}
				if (((ConnectItuEvent)event).isOptionalBwdCallIndPresent()) {
					obiitu             = ((ConnectItuEvent)event).getOptionalBwdCallInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_OBI;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = obiitu.flatOptionalBwdCallIndItu();											
				}
				if (((ConnectItuEvent)event).isParamCompatibilityInfoPresent()) {
					pci                   = ((ConnectItuEvent)event).getParamCompatibilityInfo();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_PCI;
					par_array[index++]    = pci_len = (byte)pci.flatParamCompatibilityInfoItu().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(pci.flatParamCompatibilityInfoItu(),0,
						par_array,index,pci_len);
					index+=	pci_len;						
				}	
				if(((ConnectItuEvent)event).isRedirectionNumberPresent()){
					redn               = ((ConnectItuEvent)event).getRedirectionNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_REDN;
					par_array[index++] = redn_len = (byte)redn.flatRedirectionNumberItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(redn.flatRedirectionNumberItu(),0,
							par_array,index,redn_len);
					index+= redn_len;							
				}
				if(((ConnectItuEvent)event).isRedirectionNumberRestPresent()){
					par_array[index++] = IsupParameterValues.ISUP_PMT_RNR;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = ((ConnectItuEvent)event).getRedirectionNumberRest();
				}
				if (((ConnectItuEvent)event).isRemoteOperationsPresent()) {
					rop                   = ((ConnectItuEvent)event).getRemoteOperations();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_ROP;
					par_array[index++]    = rop_len = (byte)rop.flatRemoteOperations().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(rop.flatRemoteOperations(),0,
						par_array,index,rop_len);
					index+=	rop_len;						
				}	
				if (((ConnectItuEvent)event).isServiceActivationPresent()) {
					ServiceActivation     = ((ConnectItuEvent)event).getServiceActivation();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_SAC; 
					par_array[index++]    = (byte)ServiceActivation.length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(ServiceActivation,0,
								par_array,index,ServiceActivation.length);
					index+= ServiceActivation.length;			
				}	
				if(((ConnectItuEvent)event).isTransmissionMediumUsedPresent()){
					par_array[index++]         = IsupParameterValues.ISUP_PMT_XMU;
					par_array[index++]         = 1;
					par_array[index++]         = OPTIONAL_PARAMETER;
					par_array[index++]         = ((ConnectItuEvent)event).getTransmissionMediumUsed();
				}
				if (((ConnectItuEvent)event).isUserToUserIndicatorsPresent()) {
					uuiitu             = ((ConnectItuEvent)event).getUserToUserIndicators();
					par_array[index++] = IsupParameterValues.ISUP_PMT_UUIC;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = uuiitu.flatUserToUserIndicatorsItu();	
				}		
				if (((ConnectItuEvent)event).isUserToUserInformationPresent()) {	
					uuin                  = ((ConnectItuEvent)event).getUserToUserInformation();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_UUIN; 
					par_array[index++]    = (byte)uuin.length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(uuin,0,
								par_array,index,uuin.length);
					index+= uuin.length;	
				}	
			}
			/* INFORMATION EVENT */
			else if (event instanceof InformationEvent){
				if (((InformationEvent)event).isCallReferencePresent()) {
					cref               = ((InformationEvent)event).getCallReference();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CRF;
					par_array[index++] = cref_len = (byte)cref.flatCallReference().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cref.flatCallReference(),0,
						par_array,index,cref_len);
					index+=	cref_len;		
				}	
				if (((InformationEvent)event).isCallingPartyNumberPresent()) {
					cgpn               = ((InformationEvent)event).getCallingPartyNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CGPN;
					par_array[index++] = cgpn_len = (byte)cgpn.flatCallingPartyNumber().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cgpn.flatCallingPartyNumber(),0,
						par_array,index,cgpn_len);
					index+=	cgpn_len;		
				}
				if (((InformationEvent)event).isCallingPartyCatPresent()) {
					par_array[index++] = IsupParameterValues.ISUP_PMT_CGPC;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = ((InformationEvent)event).getCallingPartyCat();						
				}
				if (((InformationEvent)event).isConReqPresent()) {
					conr               = ((InformationEvent)event).getConReq();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CONR;
					par_array[index++] = conr_len = (byte)conr.flatConReq().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(conr.flatConReq(),0,
						par_array,index,conr_len);
					index+=	conr_len;		
				}
				if (((InformationItuEvent)event).isNwSpecificFacPresent()) {
					nwcfac             = ((InformationItuEvent)event).getNwSpecificFac();
					par_array[index++] = IsupParameterValues.ISUP_PMT_NSF;
					par_array[index++] = nwcfac_len = (byte)nwcfac.flatNwSpecificFacItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(nwcfac.flatNwSpecificFacItu(),0,
								par_array,index,nwcfac_len);
					index+= nwcfac_len;
				}
				if (((InformationItuEvent)event).isParamCompatibilityInfoPresent()) {
					pci                   = ((InformationItuEvent)event).getParamCompatibilityInfo();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_PCI;
					par_array[index++]    = pci_len = (byte)pci.flatParamCompatibilityInfoItu().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(pci.flatParamCompatibilityInfoItu(),0,
						par_array,index,pci_len);
					index+=	pci_len;						
				}	
			}
			/* INFORMATION REQUEST EVENT */
			else if(event instanceof InformationReqEvent){
				if (((InformationReqEvent)event).isCallReferencePresent()) {
					cref               = ((InformationReqEvent)event).getCallReference();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CRF;
					par_array[index++] = cref_len = (byte)cref.flatCallReference().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cref.flatCallReference(),0,
						par_array,index,cref_len);
					index+=	cref_len;		
				}				
				if (((InformationReqItuEvent)event).isNwSpecificFacPresent()) {
					nwcfac             = ((InformationReqItuEvent)event).getNwSpecificFac();
					par_array[index++] = IsupParameterValues.ISUP_PMT_NSF;
					par_array[index++] = nwcfac_len = (byte)nwcfac.flatNwSpecificFacItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(nwcfac.flatNwSpecificFacItu(),0,
								par_array,index,nwcfac_len);
					index+= nwcfac_len;
				}
				if (((InformationReqItuEvent)event).isParamCompatibilityInfoPresent()) {
					pci                   = ((InformationReqItuEvent)event).getParamCompatibilityInfo();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_PCI;
					par_array[index++]    = pci_len = (byte)pci.flatParamCompatibilityInfoItu().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(pci.flatParamCompatibilityInfoItu(),0,
						par_array,index,pci_len);
					index+=	pci_len;						
				}	
			}
			/* FACILITY EVENT */
			else if(event instanceof FacilityEvent){		
				if (((FacilityEvent)event).isRemoteOperationsPresent()) {
					rop                   = ((FacilityEvent)event).getRemoteOperations();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_ROP;
					par_array[index++]    = rop_len = (byte)rop.flatRemoteOperations().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(rop.flatRemoteOperations(),0,
						par_array,index,rop_len);
					index+=	rop_len;						
				}
				if (((FacilityEvent)event).isServiceActivationPresent()) {
					ServiceActivation     = ((FacilityEvent)event).getServiceActivation();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_SAC; 
					par_array[index++]    = (byte)ServiceActivation.length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(ServiceActivation,0,
								par_array,index,ServiceActivation.length);
					index+= ServiceActivation.length;			
				}	
				if (((FacilityItuEvent)event).isMessageCompatibilityInfoPresent()) {
					mcoi               = ((FacilityItuEvent)event).getMessageCompatibilityInfo();
					par_array[index++] = IsupParameterValues.ISUP_PMT_MCOI;
					par_array[index++] = mcoi_len = (byte)mcoi.flatMessageCompatibilityInfoItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(mcoi.flatMessageCompatibilityInfoItu(),0,
								par_array,index,mcoi_len);
					index+= mcoi_len;
				}	
				if (((FacilityItuEvent)event).isParamCompatibilityInfoPresent()) {
					pci                   = ((FacilityItuEvent)event).getParamCompatibilityInfo();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_PCI;
					par_array[index++]    = pci_len = (byte)pci.flatParamCompatibilityInfoItu().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(pci.flatParamCompatibilityInfoItu(),0,
						par_array,index,pci_len);
					index+=	pci_len;						
				}	
			}
			/* FACILITY ACCEPT EVENT */
			else if(event instanceof FacilityAcceptItuEvent){
				if (((FacilityAcceptItuEvent)event).isCallReferencePresent()) {
					cref               = ((FacilityAcceptItuEvent)event).getCallReference();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CRF;
					par_array[index++] = cref_len = (byte)cref.flatCallReference().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cref.flatCallReference(),0,
						par_array,index,cref_len);
					index+=	cref_len;		
				}				
				if (((FacilityAcceptItuEvent)event).isConReqPresent()) {
					conr               = ((FacilityAcceptItuEvent)event).getConReq();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CONR;
					par_array[index++] = conr_len = (byte)conr.flatConReq().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(conr.flatConReq(),0,
						par_array,index,conr_len);
					index+=	conr_len;		
				}
				if (((FacilityAcceptItuEvent)event).isParamCompatibilityInfoPresent()) {
					pci                   = ((FacilityAcceptItuEvent)event).getParamCompatibilityInfo();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_PCI;
					par_array[index++]    = pci_len = (byte)pci.flatParamCompatibilityInfoItu().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(pci.flatParamCompatibilityInfoItu(),0,
						par_array,index,pci_len);
					index+=	pci_len;						
				}	
				if (((FacilityAcceptItuEvent)event).isUserToUserIndicatorsPresent()) {
					uuiitu             = ((FacilityAcceptItuEvent)event).getUserToUserIndicators();
					par_array[index++] = IsupParameterValues.ISUP_PMT_UUIC;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = uuiitu.flatUserToUserIndicatorsItu();	
				}			
			}
			/* FACILITY REJECT EVENT */
			else if(event instanceof FacilityRejectItuEvent){
				if (((FacilityRejectItuEvent)event).isUserToUserIndicatorsPresent()) {
					uuiitu             = ((FacilityRejectItuEvent)event).getUserToUserIndicators();
					par_array[index++] = IsupParameterValues.ISUP_PMT_UUIC;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = uuiitu.flatUserToUserIndicatorsItu();	
				}		
			}
			/* FACILITY REQUEST EVENT */
			else if(event instanceof FacilityRequestItuEvent){		
				if (((FacilityRequestItuEvent)event).isCallReferencePresent()) {
					cref               = ((FacilityRequestItuEvent)event).getCallReference();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CRF;
					par_array[index++] = cref_len = (byte)cref.flatCallReference().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cref.flatCallReference(),0,
						par_array,index,cref_len);
					index+=	cref_len;		
				}					
				if (((FacilityRequestItuEvent)event).isConReqPresent()) {
					conr               = ((FacilityRequestItuEvent)event).getConReq();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CONR;
					par_array[index++] = conr_len = (byte)conr.flatConReq().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(conr.flatConReq(),0,
						par_array,index,conr_len);
					index+=	conr_len;		
				}
				if (((FacilityRequestItuEvent)event).isParamCompatibilityInfoPresent()) {
					pci                   = ((FacilityRequestItuEvent)event).getParamCompatibilityInfo();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_PCI;
					par_array[index++]    = pci_len = (byte)pci.flatParamCompatibilityInfoItu().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(pci.flatParamCompatibilityInfoItu(),0,
						par_array,index,pci_len);
					index+=	pci_len;						
				}	
				if (((FacilityRequestItuEvent)event).isUserToUserIndicatorsPresent()) {
					uuiitu             = ((FacilityRequestItuEvent)event).getUserToUserIndicators();
					par_array[index++] = IsupParameterValues.ISUP_PMT_UUIC;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = uuiitu.flatUserToUserIndicatorsItu();	
				}				
			}
			/* FORWARD TRANSFER EVENT */
			else if(event instanceof ForwardTransferEvent){
				if (((ForwardTransferEvent)event).isCallReferencePresent()) {
					cref               = ((ForwardTransferEvent)event).getCallReference();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CRF;
					par_array[index++] = cref_len = (byte)cref.flatCallReference().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cref.flatCallReference(),0,
						par_array,index,cref_len);
					index+=	cref_len;		
				}			
			}		
			/* IDENTIFICATION REQUEST EVENT */
			else if(event instanceof IdentificationReqItuEvent){
				if (((IdentificationReqItuEvent)event).isMCIDReqIndPresent()) {
					mrq                = ((IdentificationReqItuEvent)event).getMCIDReqInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_MRQ;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = mrq.flatMCIDReqIndItu();								
				}			
				if (((IdentificationReqItuEvent)event).isMessageCompatibilityInfoPresent()) {
					mcoi               = ((IdentificationReqItuEvent)event).getMessageCompatibilityInfo();
					par_array[index++] = IsupParameterValues.ISUP_PMT_MCOI;
					par_array[index++] = mcoi_len = (byte)mcoi.flatMessageCompatibilityInfoItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(mcoi.flatMessageCompatibilityInfoItu(),0,
								par_array,index,mcoi_len);
					index+= mcoi_len;
				}		
				if (((IdentificationReqItuEvent)event).isParamCompatibilityInfoPresent()) {
					pci                   = ((IdentificationReqItuEvent)event).getParamCompatibilityInfo();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_PCI;
					par_array[index++]    = pci_len = (byte)pci.flatParamCompatibilityInfoItu().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(pci.flatParamCompatibilityInfoItu(),0,
						par_array,index,pci_len);
					index+=	pci_len;						
				}	
			}
			/* IDENTIFICATION RESPONSE EVENT */
			else if (event instanceof IdentificationRespItuEvent){
				if(((IdentificationRespItuEvent)event).isAccessTransportPresent()){					
					accessTransport    = ((IdentificationRespItuEvent)event).getAccessTransport();
					par_array[index++] = IsupParameterValues.ISUP_PMT_AXP;
					par_array[index++] = (byte)accessTransport.length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(accessTransport,0,
								par_array,index,accessTransport.length);
					index+= accessTransport.length;				
				}	
				if (((IdentificationRespItuEvent)event).isCallingPartyNumberPresent()) {
					cgpn               = ((IdentificationRespItuEvent)event).getCallingPartyNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CGPN;
					par_array[index++] = cgpn_len = (byte)cgpn.flatCallingPartyNumber().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cgpn.flatCallingPartyNumber(),0,
						par_array,index,cgpn_len);
					index+=	cgpn_len;		
				}
				if (((IdentificationRespItuEvent)event).isGenericNumberPresent()) {
					gaditu             = ((IdentificationRespItuEvent)event).getGenericNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_GAD;
					par_array[index++] = gaditu_len = (byte)gaditu.flatGenericNumberItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(gaditu.flatGenericNumberItu(),0,
								par_array,index,gaditu_len);
				
					index += gaditu_len;				
				}					
				if (((IdentificationRespItuEvent)event).isMCIDRespIndPresent()) {
					mrs                = ((IdentificationRespItuEvent)event).getMCIDRespInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_MRS;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = mrs.flatMCIDRespIndItu();								
				}			
				if (((IdentificationRespItuEvent)event).isMessageCompatibilityInfoPresent()) {
					mcoi               = ((IdentificationRespItuEvent)event).getMessageCompatibilityInfo();
					par_array[index++] = IsupParameterValues.ISUP_PMT_MCOI;
					par_array[index++] = mcoi_len = (byte)mcoi.flatMessageCompatibilityInfoItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(mcoi.flatMessageCompatibilityInfoItu(),0,
								par_array,index,mcoi_len);
					index+= mcoi_len;
				}
				if (((IdentificationRespItuEvent)event).isParamCompatibilityInfoPresent()) {
					pci                   = ((IdentificationRespItuEvent)event).getParamCompatibilityInfo();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_PCI;
					par_array[index++]    = pci_len = (byte)pci.flatParamCompatibilityInfoItu().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(pci.flatParamCompatibilityInfoItu(),0,
						par_array,index,pci_len);
					index+=	pci_len;						
				}			
			}
			/* SEGMENTATION EVENT */
			else if (event instanceof SegmentationItuEvent){
				if(((SegmentationItuEvent)event).isAccessTransportPresent()){		
					accessTransport    = ((SegmentationItuEvent)event).getAccessTransport();
					par_array[index++] = IsupParameterValues.ISUP_PMT_AXP;
					par_array[index++] = (byte)accessTransport.length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(accessTransport,0,
								par_array,index,accessTransport.length);
					index+= accessTransport.length;					
				}	
				if (((SegmentationItuEvent)event).isGenericDigitsPresent()) {
					gnd               = ((SegmentationItuEvent)event).getGenericDigits();
					par_array[index++] = IsupParameterValues.ISUP_PMT_GND;
					par_array[index++] = gnd_len = (byte)gnd.flatGenericDigits().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(gnd.flatGenericDigits(),0,
								par_array,index,gnd_len);
				
					index += gnd_len;				
				}
				if (((SegmentationItuEvent)event).isNotificationIndPresent()) {
					nind               = ((SegmentationItuEvent)event).getNotificationInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_GNNO;
					par_array[index++] = nind_len = (byte)nind.flatNotificationInd().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(nind.flatNotificationInd(),0,
								par_array,index,nind_len);
				
					index += nind_len;				
				}
				if (((SegmentationItuEvent)event).isGenericNumberPresent()) {
					gaditu             = ((SegmentationItuEvent)event).getGenericNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_GAD;
					par_array[index++] = gaditu_len = (byte)gaditu.flatGenericNumberItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(gaditu.flatGenericNumberItu(),0,
								par_array,index,gaditu_len);
				
					index += gaditu_len;				
				}
				if (((SegmentationItuEvent)event).isMessageCompatibilityInfoPresent()) {
					mcoi               = ((SegmentationItuEvent)event).getMessageCompatibilityInfo();
					par_array[index++] = IsupParameterValues.ISUP_PMT_MCOI;
					par_array[index++] = mcoi_len = (byte)mcoi.flatMessageCompatibilityInfoItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(mcoi.flatMessageCompatibilityInfoItu(),0,
								par_array,index,mcoi_len);
					index+= mcoi_len;
				}		
				if (((SegmentationItuEvent)event).isUserToUserInformationPresent()) {	
					uuin                  = ((SegmentationItuEvent)event).getUserToUserInformation();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_UUIN; 
					par_array[index++]    = (byte)uuin.length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(uuin,0,
								par_array,index,uuin.length);
					index+= uuin.length;	
				}
			}
			/* RELEASE EVENT */
			else if(event instanceof ReleaseEvent){
				if(((ReleaseEvent)event).isAccessTransportPresent()){					
					accessTransport    = ((ReleaseEvent)event).getAccessTransport();
					par_array[index++] = IsupParameterValues.ISUP_PMT_AXP;
					par_array[index++] = (byte)accessTransport.length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(accessTransport,0,
								par_array,index,accessTransport.length);
					index+= accessTransport.length;				
				}	
				if(((ReleaseEvent)event).isAutoCongLevelPresent()){
						par_array[index++] = IsupParameterValues.ISUP_PMT_ACL;
						par_array[index++] = 1;
						par_array[index++] = OPTIONAL_PARAMETER;
						par_array[index++] = ((ReleaseEvent)event).getAutoCongLevel();
				}
				if (((ReleaseEvent)event).isUserToUserInformationPresent()) {	
					uuin                  = ((ReleaseEvent)event).getUserToUserInformation();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_UUIN; 
					par_array[index++]    = (byte)uuin.length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(uuin,0,
								par_array,index,uuin.length);
					index+= uuin.length;	
				}
				if(((ReleaseItuEvent)event).isAccessDeliveryInfoPresent()){
					par_array[index++] = IsupParameterValues.ISUP_PMT_ADI;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = ((ReleaseItuEvent)event).getAccessDeliveryInfo();
				}
				if (((ReleaseItuEvent)event).isNwSpecificFacPresent()) {
					nwcfac             = ((ReleaseItuEvent)event).getNwSpecificFac();
					par_array[index++] = IsupParameterValues.ISUP_PMT_NSF;
					par_array[index++] = nwcfac_len = (byte)nwcfac.flatNwSpecificFacItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(nwcfac.flatNwSpecificFacItu(),0,
								par_array,index,nwcfac_len);
					index+= nwcfac_len;
				}
				if (((ReleaseItuEvent)event).isParamCompatibilityInfoPresent()) {
					pci                   = ((ReleaseItuEvent)event).getParamCompatibilityInfo();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_PCI;
					par_array[index++]    = pci_len = (byte)pci.flatParamCompatibilityInfoItu().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(pci.flatParamCompatibilityInfoItu(),0,
						par_array,index,pci_len);
					index+=	pci_len;						
				}				
				if (((ReleaseItuEvent)event).isRedirectionInfoPresent()) {
					redi                  = ((ReleaseItuEvent)event).getRedirectionInfo();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_REDI;
					par_array[index++]    = redi_len = (byte)redi.flatRedirectionInfo().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(redi.flatRedirectionInfo(),0,
						par_array,index,redi_len);
					index+=	redi_len;						
				}	
				if(((ReleaseItuEvent)event).isSignalingPointCodePresent()){
					spc					  = ((ReleaseItuEvent)event).getSignalingPointCode();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_SPC;
					par_array[index++]    = spc_len = (byte)spc.flatSignalingPointCode().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(spc.flatSignalingPointCode(),0,
						par_array,index,spc_len);
					index+=	spc_len;						
				}
				if(((ReleaseItuEvent)event).isRedirectionNumberPresent()){
					redn               = ((ReleaseItuEvent)event).getRedirectionNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_REDN;
					par_array[index++] = redn_len = (byte)redn.flatRedirectionNumberItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(redn.flatRedirectionNumberItu(),0,
							par_array,index,redn_len);
					index+= redn_len;							
				}			
				if(((ReleaseItuEvent)event).isRedirectionNumberRestPresent()){
					par_array[index++] = IsupParameterValues.ISUP_PMT_RNR;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = ((ReleaseItuEvent)event).getRedirectionNumberRest();
				}	
				if (((ReleaseItuEvent)event).isUserToUserIndicatorsPresent()) {
					uuiitu             = ((ReleaseItuEvent)event).getUserToUserIndicators();
					par_array[index++] = IsupParameterValues.ISUP_PMT_UUIC;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = uuiitu.flatUserToUserIndicatorsItu();	
				}									
			}
			/* RELEASE RESPONSE EVENT */
			else if(event instanceof ReleaseResponseItuEvent){
				if (((ReleaseResponseItuEvent)event).isCauseIndPresent()) {
					csi                = ((ReleaseResponseItuEvent)event).getCauseInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CSI;
					par_array[index++] = csi_len = (byte)csi.flatCauseInd().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(csi.flatCauseInd(),0,
						par_array,index,csi_len);
					index+=	csi_len;		
				}
			}					
			/* NETWORK RESOURCE MANAGAMENT EVENT */
			else if(event instanceof NetworkResourceMgmtItuEvent){
				if (((NetworkResourceMgmtItuEvent)event).isEchoControlInfoPresent()) {
					eci                = ((NetworkResourceMgmtItuEvent)event).getEchoControlInfo();
					par_array[index++] = IsupParameterValues.ISUP_PMT_ECI;					
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = eci.flatEchoControlInfoItu();			
				}
				if (((NetworkResourceMgmtItuEvent)event).isMessageCompatibilityInfoPresent()) {
					mcoi               = ((NetworkResourceMgmtItuEvent)event).getMessageCompatibilityInfo();
					par_array[index++] = IsupParameterValues.ISUP_PMT_MCOI;
					par_array[index++] = mcoi_len = (byte)mcoi.flatMessageCompatibilityInfoItu().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(mcoi.flatMessageCompatibilityInfoItu(),0,
								par_array,index,mcoi_len);
					index+= mcoi_len;
				}		
				if (((NetworkResourceMgmtItuEvent)event).isParamCompatibilityInfoPresent()) {
					pci                   = ((NetworkResourceMgmtItuEvent)event).getParamCompatibilityInfo();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_PCI;
					par_array[index++]    = pci_len = (byte)pci.flatParamCompatibilityInfoItu().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(pci.flatParamCompatibilityInfoItu(),0,
						par_array,index,pci_len);
					index+=	pci_len;						
				}	
				if(((NetworkResourceMgmtItuEvent)event).isTransmissionMediumUsedPresent()){
					par_array[index++]         = IsupParameterValues.ISUP_PMT_XMU;
					par_array[index++]         = 1;
					par_array[index++]         = OPTIONAL_PARAMETER;
					par_array[index++]         = ((NetworkResourceMgmtItuEvent)event).getTransmissionMediumUsed();
				}
			}
			/* USER TO USER INFO EVENT */
			else if(event instanceof UserToUserInfoItuEvent){
				if(((UserToUserInfoItuEvent)event).isAccessTransportPresent()){					
					accessTransport    = ((UserToUserInfoItuEvent)event).getAccessTransport();
					par_array[index++] = IsupParameterValues.ISUP_PMT_AXP;
					par_array[index++] = (byte)accessTransport.length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(accessTransport,0,
								par_array,index,accessTransport.length);
					index+= accessTransport.length;	
				}			
			}
			/* SUSPEND RESUME EVENT */
			else if(event instanceof SuspendResumeEvent){
				if (((SuspendResumeEvent)event).isCallReferencePresent()) {
					cref               = ((SuspendResumeEvent)event).getCallReference();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CRF;
					par_array[index++] = cref_len = (byte)cref.flatCallReference().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cref.flatCallReference(),0,
						par_array,index,cref_len);
					index+=	cref_len;		
				}	
			}
			/* USER PART TEST INFO EVENT */
			else if(event instanceof UserPartTestInfoItuEvent){
				if (((UserPartTestInfoItuEvent)event).isParamCompatibilityInfoPresent()) {
					pci                   = ((UserPartTestInfoItuEvent)event).getParamCompatibilityInfo();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_PCI;
					par_array[index++]    = pci_len = (byte)pci.flatParamCompatibilityInfoItu().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(pci.flatParamCompatibilityInfoItu(),0,
						par_array,index,pci_len);
					index+=	pci_len;										
				}	
			}	
		} catch (ParameterNotSetException exc) {};
		
		par_array[0] = (byte)(index-1);

		return par_array;
	}


}
