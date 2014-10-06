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
* MODULE NAME  : $RCSfile: PutAnsiParam.java,v $
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
import javax.jain.ss7.isup.ansi.*;

public class  PutAnsiParam{

	static final int NONE               = IsupConstants.NOT_SET;
	static final int FIXED_PARAMETER    = 1;
	static final int VARIABLE_PARAMETER = 2;
	static final int OPTIONAL_PARAMETER = 3;

	public static byte[] putParam(IsupEvent event) 
						throws ParameterNotSetException{

		int index = 1;
		int							PropagationDelayCounter			= NONE;
		long						chi								= NONE;
		short						csin							= NONE;
		short                       sec								= NONE;
		short                       spr								= NONE;
		BwdCallIndAnsi              bci								= null;
		BusinessGroupAnsi           bga								= null;
		CallingPartyNumber          cgpn							= null;
		CallReference               cref							= null;
		CarrierIdAnsi               cic								= null;
		CauseInd                    csi								= null;
		ChargeNumberAnsi            crgn							= null;
		CircuitAssignMapAnsi        cam								= null;
		CircuitIdNameAnsi           cinm							= null;
		CommonLangLocationIdAnsi    clli							= null;
		ConReq                      conr							= null;
		GenericDigits               gnd								= null;
		GenericNameAnsi             gnm								= null;
		GenericNumber               gad								= null;
		InfoInd                     iind							= null;
		InfoReqIndAnsi              irind							= null;
		NotificationInd             nind							= null;
		OrigCalledNumber            ocdn							= null;
		OperatorServicesInfoAnsi    opsi							= null;
		OptionalBwdCallIndAnsi      obi								= null;
		Precedence                  mlpp							= null;
		RemoteOperations            rop								= null;	
		TransitNwSelAnsi            xns								= null;
		TransactionReqAnsi          trr								= null;
		UserToUserIndicators        uui								= null;	
		UserServiceInfoPrime        usip							= null;
		RedirectingNumber           rdtg							= null;
		RedirectionInfo             redi							= null;
		byte[]                      egr								= null;
		byte[]                      uuin							= null;
		byte[]                      accessTransport					= null;
		byte[]                      ServiceActivation				= null;
		byte[]                      Jurisdiction					= null;
		byte[]                      ntp								= null;
		byte                        hop								= NONE;
		String            otgn							= null; 

		byte bci_len,cam_len, cinm_len,clli_len,cref_len, cgpn_len, cic_len, crgn_len, csi_len,bga_len, conr_len, gad_len,gnd_len,
gnm_len,iind_len,irind_len, nind_len,ocdn_len, rop_len, xns_len, usip_len, rdtg_len,
redi_len, mlpp_len, trr_len;

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
					par_array[index++] = cref_len=(byte)cref.flatCallReference().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cref.flatCallReference(),0,
						par_array,index,cref_len);
					index+=	cref_len;						
				}		
				if (((SetupEvent)event).isCallingPartyNumberPresent()) {
					cgpn               = ((SetupEvent)event).getCallingPartyNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CGPN;
					par_array[index++] = cgpn_len =(byte)cgpn.flatCallingPartyNumber().length;
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
				if (((SetupEvent)event).isGenericNumberPresent() ) {
					gad                = ((SetupEvent)event).getGenericNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_GAD;
					par_array[index++] = gad_len = (byte)gad.flatGenericNumber().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(gad.flatGenericNumber(),0,
							par_array,index,gad_len);
					index+= gad_len;	
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
				if (((SetupEvent)event).isOrigCalledNumberPresent()) {
					ocdn               = ((SetupEvent)event).getOrigCalledNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_OCDN;
					par_array[index++] = ocdn_len = (byte)ocdn.flatOrigCalledNumber().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(ocdn.flatOrigCalledNumber(),0,
								par_array,index,ocdn_len);
				
					index += ocdn_len;				
				}

				if (((SetupEvent)event).isRedirectingNumberPresent()) {
					rdtg               = ((SetupEvent)event).getRedirectingNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_RDTG;
					par_array[index++] = rdtg_len = (byte)rdtg.flatRedirectingNumber().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(rdtg.flatRedirectingNumber(),0,
								par_array,index,rdtg_len);
				
					index += rdtg_len;				
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
					
				if (((SetupEvent)event).isTransitNwSelPresent()) {				
					xns                = (TransitNwSelAnsi)((SetupEvent)event).getTransitNwSel();
					par_array[index++] = IsupParameterValues.ISUP_PMT_XNS;
					par_array[index++] = xns_len = (byte)xns.flatTransitNwSelAnsi().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(xns.flatTransitNwSelAnsi(),0,
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
											
				if (((SetupEvent)event).isUserToUserInformationPresent()) {	
					uuin                  = ((SetupEvent)event).getUserToUserInformation();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_UUIN; 
					par_array[index++]    = (byte)uuin.length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(uuin,0,
								par_array,index,uuin.length);
					index+= uuin.length;			
				} 

				if (((SetupAnsiEvent)event).isInfoReqIndPresent()) {
					irind              = (InfoReqIndAnsi)((SetupAnsiEvent)event).getInfoReqInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_INRI;
					par_array[index++] = irind_len = (byte)irind.flatInfoReqIndAnsi().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(irind.flatInfoReqIndAnsi(),0,
							par_array,index,irind_len);
					index+= irind_len;	
				}  
				if (((SetupAnsiEvent)event).isBusinessGroupPresent()) {
					bga                = ((SetupAnsiEvent)event).getBusinessGroup();
					par_array[index++] = IsupParameterValues.ISUP_PMT_BUG;
					par_array[index++] = bga_len = (byte)bga.flatBusinessGroupAnsi().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(bga.flatBusinessGroupAnsi(),0,
							par_array,index,bga_len);
					index+= bga_len;	
				}	
				if (((SetupAnsiEvent)event).isCarrierIdPresent()) {
					cic                = ((SetupAnsiEvent)event).getCarrierId();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CIC;
					par_array[index++] = cic_len = (byte)cic.flatCarrierIdAnsi().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cic.flatCarrierIdAnsi(),0,
							par_array,index,cic_len);
					index+= cic_len;	
				}	
				if (((SetupAnsiEvent)event).isCarrierSelInfoPresent()){

					csin               = ((SetupAnsiEvent)event).getCarrierSelInfo();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CSIN;
					par_array[index++] = 2;
					par_array[index++] = OPTIONAL_PARAMETER;
					for(int j=0;j<2;j++)
					{
						par_array[index++] = (byte)(csin >> (8*j) & 0x00FF);
					}							
				}	
				if (((SetupAnsiEvent)event).isChargeNumberPresent()) {
					crgn                = ((SetupAnsiEvent)event).getChargeNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CRGN;
					par_array[index++] = crgn_len = (byte)crgn.flatChargeNumberAnsi().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(crgn.flatChargeNumberAnsi(),0,
							par_array,index,crgn_len);
					index+= crgn_len;	
				}	
				if (((SetupAnsiEvent)event).isCircuitAssignMapPresent()) {
					cam                = ((SetupAnsiEvent)event).getCircuitAssignMap();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CAM;
					par_array[index++] = cam_len = (byte)cam.flatCircuitAssignMapAnsi().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cam.flatCircuitAssignMapAnsi(),0,
							par_array,index,cam_len);
					index+= cam_len;	
				}	
				if(((SetupAnsiEvent)event).isEgressServicePresent()){					
						egr                = ((SetupAnsiEvent)event).getEgressService();
						par_array[index++] = IsupParameterValues.ISUP_PMT_EGR;
						par_array[index++] = (byte)egr.length;
						par_array[index++] = OPTIONAL_PARAMETER;
						System.arraycopy(egr,0,
								par_array,index,egr.length);
						index+= egr.length;								
				}	
				
				if (((SetupAnsiEvent)event).isGenericNamePresent()) {
					gnm                = ((SetupAnsiEvent)event).getGenericName();
					par_array[index++] = IsupParameterValues.ISUP_PMT_GNM;
					par_array[index++] = gnm_len = (byte)gnm.flatGenericNameAnsi().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(gnm.flatGenericNameAnsi(),0,
							par_array,index,gnm_len);
					index+= gnm_len;	
				}	
				if (((SetupAnsiEvent)event).isHopCounterPresent()) {
					hop                   = ((SetupAnsiEvent)event).getHopCounter();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_HOP;
					par_array[index++]    = 1;
					par_array[index++]    = OPTIONAL_PARAMETER;
					par_array[index++]    = hop;
				}	
				if (((SetupAnsiEvent)event).isJurisdictionPresent()) {
					Jurisdiction          = ((SetupAnsiEvent)event).getJurisdiction();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_JUR; 
					par_array[index++]    = (byte)Jurisdiction.length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(Jurisdiction,0,
								par_array,index,Jurisdiction.length);
					index+= Jurisdiction.length;	
				}	
				if (((SetupAnsiEvent)event).isNetworkTransportPresent()) {
					ntp                   = ((SetupAnsiEvent)event).getNetworkTransport();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_NTP; 
					par_array[index++]    = (byte)ntp.length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(ntp,0,
								par_array,index,ntp.length);
					index+= ntp.length;	
				}
				if (((SetupAnsiEvent)event).isOperatorServicesInfoPresent()) {
					opsi                  = ((SetupAnsiEvent)event).getOperatorServicesInfo();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_OPSI;
					par_array[index++]    = 1;
					par_array[index++]    = OPTIONAL_PARAMETER;
					par_array[index++]    = opsi.flatOperatorServicesInfoAnsi();
				}
				
				if (((SetupAnsiEvent)event).isOrigLineInfoPresent()) {
					par_array[index++] = IsupParameterValues.ISUP_PMT_OLI;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = ((SetupAnsiEvent)event).getOrigLineInfo();		
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
				if (((SetupAnsiEvent)event).isServiceCodePresent()){

					sec                = ((SetupAnsiEvent)event).getServiceCode();
					par_array[index++] = IsupParameterValues.ISUP_PMT_SEC;
					par_array[index++] = 2;
					par_array[index++] = OPTIONAL_PARAMETER;
					for(int j=0;j<2;j++)
					{
						par_array[index++] = (byte)(sec >> (8*j) & 0x00FF);
					}							
				}	
				if (((SetupAnsiEvent)event).isSpecialProcReqPresent()){

					spr                = ((SetupAnsiEvent)event).getSpecialProcReq();
					par_array[index++] = IsupParameterValues.ISUP_PMT_SPR;
					par_array[index++] = 2;
					par_array[index++] = OPTIONAL_PARAMETER;
					for(int j=0;j<2;j++)
					{
						par_array[index++] = (byte)(spr >> (8*j) & 0x00FF);
					}							
				}	
				if (((SetupAnsiEvent)event).isTransactionReqPresent()) {
					trr                = ((SetupAnsiEvent)event).getTransactionReq();
					par_array[index++] = IsupParameterValues.ISUP_PMT_TRR;
					par_array[index++] = trr_len = (byte)trr.flatTransactionReqAnsi().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(trr.flatTransactionReqAnsi(),0,
							par_array,index,trr_len);
					index+= trr_len;	
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
				if (((AlertAnsiEvent)event).isBusinessGroupPresent()) {
					bga                = ((AlertAnsiEvent)event).getBusinessGroup();
					par_array[index++] = IsupParameterValues.ISUP_PMT_BUG;
					par_array[index++] = bga_len = (byte)bga.flatBusinessGroupAnsi().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(bga.flatBusinessGroupAnsi(),0,
							par_array,index,bga_len);
					index+= bga_len;	
				}	
				if (((AlertEvent)event).isCallReferencePresent()) {
					cref               = ((AlertEvent)event).getCallReference();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CRF;
					par_array[index++] = cref_len=(byte)cref.flatCallReference().length;
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
				if (((AlertAnsiEvent)event).isConReqPresent()) {
					conr               = ((AlertAnsiEvent)event).getConReq();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CONR;
					par_array[index++] = conr_len = (byte)conr.flatConReq().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(conr.flatConReq(),0,
						par_array,index,conr_len);
					index+=	conr_len;		
				}
				if (((AlertAnsiEvent)event).isInfoIndPresent()){
					iind               = ((AlertAnsiEvent)event).getInfoInd();

					par_array[index++] = IsupParameterValues.ISUP_PMT_INI;
					par_array[index++] = iind_len = (byte)iind.flatInfoInd().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(iind.flatInfoInd(),0,
							par_array,index,iind_len);
					index+= iind_len;	
				}
				if (((AlertAnsiEvent)event).isNetworkTransportPresent()) {
					ntp                   = ((AlertAnsiEvent)event).getNetworkTransport();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_NTP; 
					par_array[index++]    = (byte)ntp.length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(ntp,0,
								par_array,index,ntp.length);
					index+= ntp.length;	
				}
				if (((AlertAnsiEvent)event).isNotificationIndPresent()) {
					nind               = ((AlertAnsiEvent)event).getNotificationInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_NTI;
					par_array[index++] = nind_len = (byte)nind.flatNotificationInd().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(nind.flatNotificationInd(),0,
								par_array,index,nind_len);
				
					index += nind_len;				
				}	
				if (((AlertEvent)event).isOptionalBwdCallIndPresent()) {
					obi                = (OptionalBwdCallIndAnsi)((AlertEvent)event).getOptionalBwdCallInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_OBI;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = obi.flatOptionalBwdCallIndAnsi();											
				}	
				if (((AlertAnsiEvent)event).isRedirectionInfoPresent()) {
					redi                  = ((AlertAnsiEvent)event).getRedirectionInfo();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_REDI;
					par_array[index++]    = redi_len = (byte)redi.flatRedirectionInfo().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(redi.flatRedirectionInfo(),0,
						par_array,index,redi_len);
					index+=	redi_len;						
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
				if (((AlertEvent)event).isUserToUserIndicatorsPresent()) {				
					uui                = ((AlertEvent)event).getUserToUserIndicators();
					par_array[index++] = IsupParameterValues.ISUP_PMT_UUIC;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = uui.flatUserToUserIndicators();	
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
					bci                = (BwdCallIndAnsi)((AnswerEvent)event).getBwdCallInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_BACI;
					par_array[index++] = bci_len = (byte)bci.flatBwdCallIndAnsi().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(bci.flatBwdCallIndAnsi(),0,
							par_array,index,bci_len);
					index+= bci_len;	
				}	
				if (((AnswerAnsiEvent)event).isBusinessGroupPresent()) {
					bga                = ((AnswerAnsiEvent)event).getBusinessGroup();
					par_array[index++] = IsupParameterValues.ISUP_PMT_BUG;
					par_array[index++] = bga_len = (byte)bga.flatBusinessGroupAnsi().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(bga.flatBusinessGroupAnsi(),0,
							par_array,index,bga_len);
					index+= bga_len;	
				}
				if (((AnswerEvent)event).isCallReferencePresent()) {
					cref               = ((AnswerEvent)event).getCallReference();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CRF;
					par_array[index++] = cref_len=(byte)cref.flatCallReference().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cref.flatCallReference(),0,
						par_array,index,cref_len);
					index+=	cref_len;						
				}
				if (((AnswerAnsiEvent)event).isConReqPresent()) {
					conr               = ((AnswerAnsiEvent)event).getConReq();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CONR;
					par_array[index++] = conr_len = (byte)conr.flatConReq().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(conr.flatConReq(),0,
						par_array,index,conr_len);
					index+=	conr_len;		
				}
				if (((AnswerAnsiEvent)event).isInfoIndPresent()){
					iind               = ((AnswerAnsiEvent)event).getInfoInd();

					par_array[index++] = IsupParameterValues.ISUP_PMT_INI;
					par_array[index++] = iind_len = (byte)iind.flatInfoInd().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(iind.flatInfoInd(),0,
							par_array,index,iind_len);
					index+= iind_len;	
				}
				if (((AnswerAnsiEvent)event).isNetworkTransportPresent()) {
					ntp                   = ((AnswerAnsiEvent)event).getNetworkTransport();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_NTP; 
					par_array[index++]    = (byte)ntp.length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(ntp,0,
								par_array,index,ntp.length);
					index+= ntp.length;	
				}	
				if (((AnswerAnsiEvent)event).isNotificationIndPresent()) {
					nind               = ((AnswerAnsiEvent)event).getNotificationInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_NTI;
					par_array[index++] = nind_len = (byte)nind.flatNotificationInd().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(nind.flatNotificationInd(),0,
								par_array,index,nind_len);
				
					index += nind_len;				
				}
				if (((AnswerEvent)event).isOptionalBwdCallIndPresent()) {
					obi                = (OptionalBwdCallIndAnsi)((AnswerEvent)event).getOptionalBwdCallInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_OBI;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = obi.flatOptionalBwdCallIndAnsi();											
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
					bci                = (BwdCallIndAnsi)((CallProgressEvent)event).getBwdCallInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_BACI;
					par_array[index++] = bci_len = (byte)bci.flatBwdCallIndAnsi().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(bci.flatBwdCallIndAnsi(),0,
							par_array,index,bci_len);
					index+= bci_len;	
				}	
				if (((CallProgressAnsiEvent)event).isBusinessGroupPresent()) {
					bga                = ((CallProgressAnsiEvent)event).getBusinessGroup();
					par_array[index++] = IsupParameterValues.ISUP_PMT_BUG;
					par_array[index++] = bga_len = (byte)bga.flatBusinessGroupAnsi().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(bga.flatBusinessGroupAnsi(),0,
							par_array,index,bga_len);
					index+= bga_len;	
				}
				if (((CallProgressEvent)event).isCallReferencePresent()) {
					cref               = ((CallProgressEvent)event).getCallReference();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CRF;
					par_array[index++] = cref_len=(byte)cref.flatCallReference().length;
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
				if (((CallProgressAnsiEvent)event).isInfoIndPresent()){
					iind               = ((CallProgressAnsiEvent)event).getInfoInd();

					par_array[index++] = IsupParameterValues.ISUP_PMT_INI;
					par_array[index++] = iind_len = (byte)iind.flatInfoInd().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(iind.flatInfoInd(),0,
							par_array,index,iind_len);
					index+= iind_len;	
				}
				if (((CallProgressAnsiEvent)event).isNetworkTransportPresent()) {
					ntp                   = ((CallProgressAnsiEvent)event).getNetworkTransport();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_NTP; 
					par_array[index++]    = (byte)ntp.length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(ntp,0,
								par_array,index,ntp.length);
					index+= ntp.length;	
				}
				if (((CallProgressAnsiEvent)event).isNotificationIndPresent()) {
					nind               = ((CallProgressAnsiEvent)event).getNotificationInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_NTI;
					par_array[index++] = nind_len = (byte)nind.flatNotificationInd().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(nind.flatNotificationInd(),0,
								par_array,index,nind_len);
				
					index += nind_len;				
				}
				if (((CallProgressEvent)event).isOptionalBwdCallIndPresent()) {
					obi                = (OptionalBwdCallIndAnsi)((CallProgressEvent)event).getOptionalBwdCallInd();
					par_array[index++] = IsupParameterValues.ISUP_PMT_OBI;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = obi.flatOptionalBwdCallIndAnsi();											
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
					par_array[index++]         = ((AlertEvent)event).getTransmissionMediumUsed();
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
			}
			/* CIRCUIT VALIDATION RESPONSE EVENT */
			else if(event instanceof CircuitValidationRespAnsiEvent){
				if(((CircuitValidationRespAnsiEvent)event).isCircuitIdNamePresent()){	
					cinm               = ((CircuitValidationRespAnsiEvent)event).getCircuitIdName();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CINM;
					par_array[index++] = cinm_len = (byte)cinm.flatCircuitIdNameAnsi().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cinm.flatCircuitIdNameAnsi(),0,
								par_array,index,cinm_len);
					index += cinm_len;											
				}	
				if(((CircuitValidationRespAnsiEvent)event).isCommonLangLocationIdPresent()){	
					clli               = ((CircuitValidationRespAnsiEvent)event).getCommonLangLocationId();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CLLI;
					par_array[index++] = clli_len = (byte)clli.flatCommonLangLocationIdAnsi().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(clli.flatCommonLangLocationIdAnsi(),0,
								par_array,index,clli_len);
				
					index += clli_len;											
				}									
 
			}
			/* EXIT  EVENT */
			else if(event instanceof ExitAnsiEvent){
				if(((ExitAnsiEvent)event).isOutgoingTrunkGrpNumberPresent()){	
					otgn               = ((ExitAnsiEvent)event).getOutgoingTrunkGrpNumber();
					byte[] otgnstr     = otgn.getBytes();
					par_array[index++] = IsupParameterValues.ISUP_PMT_OTGN;
					par_array[index++] = (byte)otgnstr.length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(otgnstr,0,
								par_array,index,otgnstr.length);
				
					index += otgnstr.length;

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
			}
			/* INFORMATION EVENT */
			if(event instanceof InformationAnsiEvent){	
				if(((InformationAnsiEvent)event).isAccessTransportPresent()){					
						accessTransport    = ((InformationAnsiEvent)event).getAccessTransport();
						par_array[index++] = IsupParameterValues.ISUP_PMT_AXP;
						par_array[index++] = (byte)accessTransport.length;
						par_array[index++] = OPTIONAL_PARAMETER;
						System.arraycopy(accessTransport,0,
								par_array,index,accessTransport.length);
						index+= accessTransport.length;								
				}
				if (((InformationAnsiEvent)event).isBusinessGroupPresent()) {
					bga                = ((InformationAnsiEvent)event).getBusinessGroup();
					par_array[index++] = IsupParameterValues.ISUP_PMT_BUG;
					par_array[index++] = bga_len = (byte)bga.flatBusinessGroupAnsi().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(bga.flatBusinessGroupAnsi(),0,
							par_array,index,bga_len);
					index+= bga_len;	
				}
				if (((InformationEvent)event).isCallReferencePresent()) {
					cref               = ((InformationEvent)event).getCallReference();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CRF;
					par_array[index++] = cref_len=(byte)cref.flatCallReference().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cref.flatCallReference(),0,
						par_array,index,cref_len);
					index+=	cref_len;						
				}
				if (((InformationEvent)event).isCallingPartyNumberPresent()) {
					cgpn               = ((InformationEvent)event).getCallingPartyNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CGPN;
					par_array[index++] = cgpn_len =(byte)cgpn.flatCallingPartyNumber().length;
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
				if (((InformationAnsiEvent)event).isChargeNumberPresent()) {
					crgn                = ((InformationAnsiEvent)event).getChargeNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CRGN;
					par_array[index++] = crgn_len = (byte)crgn.flatChargeNumberAnsi().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(crgn.flatChargeNumberAnsi(),0,
							par_array,index,crgn_len);
					index+= crgn_len;	
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
				if (((InformationAnsiEvent)event).isOrigLineInfoPresent()) {
					par_array[index++] = IsupParameterValues.ISUP_PMT_OLI;
					par_array[index++] = 1;
					par_array[index++] = OPTIONAL_PARAMETER;
					par_array[index++] = ((InformationAnsiEvent)event).getOrigLineInfo();						
				}
				if (((InformationAnsiEvent)event).isRedirectingNumberPresent()) {
					rdtg               = ((InformationAnsiEvent)event).getRedirectingNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_RDTG;
					par_array[index++] = rdtg_len = (byte)rdtg.flatRedirectingNumber().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(rdtg.flatRedirectingNumber(),0,
								par_array,index,rdtg_len);
				
					index += rdtg_len;				
				}
				if (((InformationAnsiEvent)event).isRedirectionInfoPresent()) {
					redi                  = ((InformationAnsiEvent)event).getRedirectionInfo();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_REDI;
					par_array[index++]    = redi_len = (byte)redi.flatRedirectionInfo().length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(redi.flatRedirectionInfo(),0,
						par_array,index,redi_len);
					index+=	redi_len;						
				}
				if (((InformationAnsiEvent)event).isUserToUserInformationPresent()) {	
					uuin                  = ((InformationAnsiEvent)event).getUserToUserInformation();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_UUIN; 
					par_array[index++]    = (byte)uuin.length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(uuin,0,
								par_array,index,uuin.length);
					index+= uuin.length;			
				}
			}
			/* INFORMATION REQUEST EVENT */
			if(event instanceof InformationReqAnsiEvent){	
				if (((InformationReqEvent)event).isCallReferencePresent()) {
					cref               = ((InformationReqEvent)event).getCallReference();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CRF;
					par_array[index++] = cref_len=(byte)cref.flatCallReference().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cref.flatCallReference(),0,
						par_array,index,cref_len);
					index+=	cref_len;						
				}
				if (((InformationReqAnsiEvent)event).isConReqPresent()) {
					conr               = ((InformationReqAnsiEvent)event).getConReq();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CONR;
					par_array[index++] = conr_len = (byte)conr.flatConReq().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(conr.flatConReq(),0,
						par_array,index,conr_len);
					index+=	conr_len;		
				}
				if (((InformationReqAnsiEvent)event).isNetworkTransportPresent()) {
					ntp                   = ((InformationReqAnsiEvent)event).getNetworkTransport();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_NTP; 
					par_array[index++]    = (byte)ntp.length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(ntp,0,
								par_array,index,ntp.length);
					index+= ntp.length;	
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
				if (((ReleaseAnsiEvent)event).isCallReferencePresent()) {
					cref               = ((ReleaseAnsiEvent)event).getCallReference();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CRF;
					par_array[index++] = cref_len=(byte)cref.flatCallReference().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cref.flatCallReference(),0,
						par_array,index,cref_len);
					index+=	cref_len;						
				}	
				if (((ReleaseAnsiEvent)event).isChargeNumberPresent()) {
					crgn                = ((ReleaseAnsiEvent)event).getChargeNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CRGN;
					par_array[index++] = crgn_len = (byte)crgn.flatChargeNumberAnsi().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(crgn.flatChargeNumberAnsi(),0,
							par_array,index,crgn_len);
					index+= crgn_len;	
				}	
				if (((ReleaseAnsiEvent)event).isGenericNumberPresent() ) {
					gad                = ((ReleaseAnsiEvent)event).getGenericNumber();
					par_array[index++] = IsupParameterValues.ISUP_PMT_GAD;
					par_array[index++] = gad_len = (byte)gad.flatGenericNumber().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(gad.flatGenericNumber(),0,
							par_array,index,gad_len);
					index+= gad_len;	
				}
				if (((ReleaseAnsiEvent)event).isServiceActivationPresent()) {
					ServiceActivation     = ((ReleaseAnsiEvent)event).getServiceActivation();
					par_array[index++]    = IsupParameterValues.ISUP_PMT_SAC; 
					par_array[index++]    = (byte)ServiceActivation.length;
					par_array[index++]	  = OPTIONAL_PARAMETER;
					System.arraycopy(ServiceActivation,0,
								par_array,index,ServiceActivation.length);
					index+= ServiceActivation.length;	
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
			/* CIRCUIT GROUP RESET EVENT */
			else if(event instanceof CircuitGrpResetAnsiEvent ){
				if (((CircuitGrpResetAnsiEvent )event).isCircuitAssignMapPresent()) {
					cam                = ((CircuitGrpResetAnsiEvent)event).getCircuitAssignMap();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CAM;
					par_array[index++] = cam_len = (byte)cam.flatCircuitAssignMapAnsi().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cam.flatCircuitAssignMapAnsi(),0,
							par_array,index,cam_len);
					index+= cam_len;	
				}				
			}
			/* CIRCUIT GROUP RESET ACK EVENT */
			else if(event instanceof CircuitGrpResetAckAnsiEvent ){
				if (((CircuitGrpResetAckAnsiEvent )event).isCircuitAssignMapPresent()) {
					cam                = ((CircuitGrpResetAckAnsiEvent)event).getCircuitAssignMap();
					par_array[index++] = IsupParameterValues.ISUP_PMT_CAM;
					par_array[index++] = cam_len = (byte)cam.flatCircuitAssignMapAnsi().length;
					par_array[index++] = OPTIONAL_PARAMETER;
					System.arraycopy(cam.flatCircuitAssignMapAnsi(),0,
							par_array,index,cam_len);
					index+= cam_len;	
				}				
			}

		} catch (ParameterNotSetException exc) {};
		
		par_array[0] = (byte)(index-1);

		return par_array;
	}


}
