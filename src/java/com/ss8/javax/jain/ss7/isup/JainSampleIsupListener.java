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
* DATE         : $Date: 2008/05/16 12:23:19 $
*
* MODULE NAME  : $RCSfile: JainSampleIsupListener.java,v $
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
import java.io.*;
import java.lang.*;

import javax.jain.*;
import javax.jain.ss7.*;
import javax.jain.ss7.isup.*;
import javax.jain.ss7.isup.itu.*;
import javax.jain.ss7.isup.ansi.*;

import com.ss8.javax.jain.ss7.isup.*;

public class JainSampleIsupListener implements JainIsupListener {

	public void processIsupEvent(IsupEvent isupEvent){
		
		int  Cic;
		byte congprio,sls;		

		if(isupEvent != null){
			
			Cic      = isupEvent.getCic();
			congprio = isupEvent.getCongestionPriority();
			sls      = isupEvent.getSls();

			try{	
			switch (isupEvent.getIsupPrimitive()) {
				case IsupConstants.ISUP_PRIMITIVE_ALERT:
					System.out.println("ALERT is received.....\n");			
						if(prntOpt == true){
							if(isupEvent instanceof AlertItuEvent)
								System.out.println(((AlertItuEvent)isupEvent).toString());
							else
								System.out.println(((AlertAnsiEvent)isupEvent).toString());
						}												
					break;

				case IsupConstants.ISUP_PRIMITIVE_ANSWER:
					System.out.println("ANSWER is received.....\n");
						if(prntOpt == true){					
							if(isupEvent instanceof AnswerItuEvent)
								System.out.println(((AnswerItuEvent)isupEvent).toString());
							else
								System.out.println(((AnswerAnsiEvent)isupEvent).toString());						
						}
					break;

				case IsupConstants.ISUP_PRIMITIVE_SETUP:
					System.out.println("ISUP_SETUP is received.....\n");
						if(prntOpt == true){								
							if(isupEvent instanceof SetupItuEvent)
								System.out.println(((SetupItuEvent)isupEvent).toString());
							else
								System.out.println(((SetupAnsiEvent)isupEvent).toString());															
						}
					break;	

				case IsupConstants.ISUP_PRIMITIVE_CALL_PROGRESS:
					System.out.println("CALL PROGRESS is received.....\n");
						if(prntOpt == true){
							if(isupEvent instanceof CallProgressItuEvent)
								System.out.println(((CallProgressItuEvent)isupEvent).toString());		
							else
								System.out.println(((CallProgressAnsiEvent)isupEvent).toString());					
							
						}
					break;

				case IsupConstants.ISUP_PRIMITIVE_CIRCUIT_RESERVATION:
					System.out.println("CIRCUIT RESERVATION is received.....\n");
						if(prntOpt == true){		
							System.out.println(((CircuitReservationAnsiEvent)isupEvent).toString());				
						}
					break;

				case IsupConstants.ISUP_PRIMITIVE_CIRCUIT_VALIDATION_RESP:
					System.out.println("CIRCUIT VALIDATION RESPONSE is received.....\n");
						if(prntOpt == true){
							System.out.println(((CircuitValidationRespAnsiEvent)isupEvent).toString());			
						}
					break;


				case IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_QUERY_RESP:
				case IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_LOCAL_QUERY_RESP:
					System.out.println("CIRCUIT GROUP QUERY RESPONSE is received.....\n");
						if(prntOpt == true){
							System.out.println(((CircuitGrpQueryRespEvent)isupEvent).toString());	
						}
					break;		

				case IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_RESET_ACK:
					System.out.println("CIRCUIT GROUP RESET ACK  is received.....\n");
						if(prntOpt == true){	
							if(isupEvent instanceof CircuitGrpResetAckAnsiEvent)
								System.out.println(((CircuitGrpResetAckAnsiEvent)isupEvent).toString());	
							else
								System.out.println(((CircuitGrpResetAckEvent)isupEvent).toString());
						}
					break;		

				case IsupConstants.ISUP_PRIMITIVE_CONFUSION:
					System.out.println("CONFUSION is received.....\n");
						if(prntOpt == true){
							System.out.println(((ConfusionEvent)isupEvent).toString());	
						}
					break;	

				case IsupConstants.ISUP_PRIMITIVE_CONNECT:
					System.out.println("CONNECT is received.....\n");
						if(prntOpt == true){
							System.out.println(((ConnectItuEvent)isupEvent).toString());							
						}
					break;

				case IsupConstants.ISUP_PRIMITIVE_CONTINUITY:
					System.out.println("CONTINUITY is received.....\n");
						if(prntOpt == true){
							System.out.println(((ContinuityEvent)isupEvent).toString());	
						}
					break;

				case IsupConstants.ISUP_PRIMITIVE_EXIT:
					System.out.println("EXIT is received.....\n");
						if(prntOpt == true){
							System.out.println(((ExitAnsiEvent)isupEvent).toString());	
						}
					break;

				case IsupConstants.ISUP_PRIMITIVE_FACILITY_REJECT:
					System.out.println("FACILITY REJECT is received.....\n");
						if(prntOpt == true){
							System.out.println(((FacilityRejectItuEvent)isupEvent).toString());	
						}
					break;

				case IsupConstants.ISUP_PRIMITIVE_FACILITY:
					System.out.println("FACILITY  is received.....\n");
						if(prntOpt == true){
							if(isupEvent instanceof FacilityItuEvent)
								System.out.println(((FacilityItuEvent)isupEvent).toString());	
							else
								System.out.println(((FacilityEvent)isupEvent).toString());								
						}
					break;

				case IsupConstants.ISUP_PRIMITIVE_FACILITY_ACCEPT:
					System.out.println("FACILITY ACCEPT is received.....\n");
						if(prntOpt == true){
							System.out.println(((FacilityAcceptItuEvent)isupEvent).toString());		
						}
					break;

				case IsupConstants.ISUP_PRIMITIVE_FACILITY_REQUEST:
					System.out.println("FACILITY REQUEST is received.....\n");
						if(prntOpt == true){
							System.out.println(((FacilityRequestItuEvent)isupEvent).toString());		
						}
					break;

				case IsupConstants.ISUP_PRIMITIVE_INFORMATION:
					System.out.println("INFORMATION is received.....\n");
						if(prntOpt == true){
							if(isupEvent instanceof InformationItuEvent)
								System.out.println(((InformationItuEvent)isupEvent).toString());	
							else
								System.out.println(((InformationAnsiEvent)isupEvent).toString());		
						}						
					break;

				case IsupConstants.ISUP_PRIMITIVE_INFORMATION_REQ:
					System.out.println("INFORMATION is received.....\n");
						if(prntOpt == true){
							if(isupEvent instanceof InformationReqItuEvent)
								System.out.println(((InformationReqItuEvent)isupEvent).toString());	
							else
								System.out.println(((InformationReqAnsiEvent)isupEvent).toString());	
						}
					break;

				case IsupConstants.ISUP_PRIMITIVE_RELEASE:
					System.out.println("RELEASE is received.....\n");
						if(prntOpt == true){
							if(isupEvent instanceof ReleaseItuEvent)								
								System.out.println(((ReleaseItuEvent)isupEvent).toString());	
							
							else
								System.out.println(((ReleaseAnsiEvent)isupEvent).toString());			
						}
					break;

				case IsupConstants.ISUP_PRIMITIVE_RELEASE_RESPONSE:
					System.out.println("RELEASE RESPONSE is received.....\n");
						if(prntOpt == true){
							if(isupEvent instanceof ReleaseItuEvent)
								System.out.println(((ReleaseItuEvent)isupEvent).toString());		
									
						}
					break;
				
				case IsupConstants.ISUP_PRIMITIVE_SUBSEQUENT_ADDRESS:
					System.out.println("SUBSEQUENT ADDRESS  is received.....\n");
					try{
						if(prntOpt == true){
							System.out.println(((SubsequentAddressItuEvent)isupEvent).toString());	
						}
					}catch (Exception exception) {};
					break;
	
				case IsupConstants.ISUP_PRIMITIVE_USER_TO_USER_INFO:
					System.out.println("USER TO USER INFO  is received.....\n");
					try{
						if(prntOpt == true){
							System.out.println(((UserToUserInfoItuEvent)isupEvent).toString());	
						}		
					}catch (Exception exception) {};
					break;

				case IsupConstants.ISUP_PRIMITIVE_FORWARD_TRANSFER:
					System.out.println("FORWARD TRANSFER  is received.....\n");
						if(prntOpt == true){
							System.out.println(((ForwardTransferEvent)isupEvent).toString());	
						}
					break;

				case IsupConstants.ISUP_PRIMITIVE_SUSPEND:
				case IsupConstants.ISUP_PRIMITIVE_RESUME:
					System.out.println("SUSPEND/RESUME  is received.....\n");
						if(prntOpt == true){
							System.out.println(((SuspendResumeEvent)isupEvent).toString());								
						}
					break;

				case IsupConstants.ISUP_PRIMITIVE_BLOCKING:
					System.out.println("BLOCKING  is received.....\n");
					break;
				case IsupConstants.ISUP_PRIMITIVE_BLOCKING_ACK:
					System.out.println("BLOCKING ACK is received.....\n");
					break;
				case IsupConstants.ISUP_PRIMITIVE_CONTINUITY_CHECK_REQ:
					System.out.println("CONT CHECK REQ is received.....\n");
					break;
				case IsupConstants.ISUP_PRIMITIVE_LOOPBACK_ACK:
					System.out.println("LOOPBACK ACK is received.....\n");
					break;
				case IsupConstants.ISUP_PRIMITIVE_OVERLOAD:
					System.out.println("OVERLOAD is received.....\n");
					break;
				case IsupConstants.ISUP_PRIMITIVE_RESET_CIRCUIT:
					System.out.println("RESET CIRCUIT is received.....\n");
					break;
				case IsupConstants.ISUP_PRIMITIVE_UNBLOCKING:
					System.out.println("UNBLOCKING is received.....\n");
					break;
				case IsupConstants.ISUP_PRIMITIVE_UNBLOCKING_ACK:
					System.out.println("UNBLOCKING ACK is received.....\n");
					break;
				case IsupConstants.ISUP_PRIMITIVE_UNEQUIPPED_CIC:
					System.out.println("UNEQUIPPED CIC  is received.....\n");
					break;
				case IsupConstants.ISUP_PRIMITIVE_START_RESET_IND:
					System.out.println("START RESET IND  is received.....\n");
					break;
				case IsupConstants.ISUP_PRIMITIVE_CIRCUIT_RESERVATION_ACK:
					System.out.println("CIRCUIT RESERVATION ACK  is received.....\n");
					break;
				case IsupConstants.ISUP_PRIMITIVE_CIRCUIT_VALIDATION_TEST:
					System.out.println("CIRCUIT VALIDATION TEST  is received.....\n");
					break;

				case IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_BLOCKING:				
				case IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_BLOCKING_ACK:
				case IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_UNBLOCKING:
				case IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_UNBLOCKING_ACK:
					System.out.println("CGB/CGBA/CGU/CGUA  is received.....\n");
						if(prntOpt == true){
							System.out.println(((CircuitGrpMaintenanceEvent)isupEvent).toString());					
						}
					break;

				case IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_LOCAL_QUERY:
				case IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_QUERY:
					System.out.println("CIRCUIT GROUP QUERY  is received.....\n");
						if(prntOpt == true){
							System.out.println(((CircuitGrpCommonEvent)isupEvent).toString());					
						}											
					break;

				case IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_RESET:
					System.out.println("CIRCUIT GROUP RESET  is received.....\n");
						if(prntOpt == true){
							if(isupEvent instanceof	CircuitGrpResetAnsiEvent)
								System.out.println(((CircuitGrpResetAnsiEvent)isupEvent).toString());
							else
								System.out.println(((CircuitGrpResetEvent)isupEvent).toString());
						}							
					break;

				case IsupConstants.ISUP_PRIMITIVE_USER_PART_TEST:
				case IsupConstants.ISUP_PRIMITIVE_USER_PART_AVAILABLE:
					System.out.println("USER PART TEST/AVAILABLE  is received.....\n");
						if(prntOpt == true){
							System.out.println(((UserPartTestInfoItuEvent)isupEvent).toString());
						}
					break;

				case IsupConstants.ISUP_PRIMITIVE_NETWORK_RESOURCE_MGMT:
					System.out.println("NETWORK RESOURCE MANAGEMENT  is received.....\n");
						if(prntOpt == true){
							System.out.println(((NetworkResourceMgmtItuEvent)isupEvent).toString());
						}
					break;
				
			
				case IsupConstants.ISUP_PRIMITIVE_IDENTIFICATION_REQ:
					System.out.println("IDENTIFICATION REQUEST  is received.....\n");
						if(prntOpt == true){
							System.out.println(((IdentificationReqItuEvent)isupEvent).toString());
						}
					break;
				
				case IsupConstants.ISUP_PRIMITIVE_IDENTIFICATION_RESP:
					System.out.println("IDENTIFICATION RESPONSE is received.....\n");
						if(prntOpt == true){
							System.out.println(((IdentificationRespItuEvent)isupEvent).toString());
						}
					break;

				case IsupConstants.ISUP_PRIMITIVE_SEGMENTATION:
					System.out.println("SEGMENTATION is received.....\n");
						if(prntOpt == true){
							System.out.println(((SegmentationItuEvent)isupEvent).toString());
						}	
					break;

				case IsupConstants.ISUP_PRIMITIVE_PASS_ALONG:
					System.out.println("PASS ALONG is received.....\n");
						if(prntOpt == true){
							System.out.println(((PassAlongEvent)isupEvent).toString());
						}
					break;

				case IsupConstants.ISUP_PRIMITIVE_CALL_FAILURE_IND:
					System.out.println("CALL FAILURE is received.....\n");
						if(prntOpt == true){
							System.out.println(((CallFailureIndEvent)isupEvent).toString());			
						}							
					break;

				case IsupConstants.ISUP_PRIMITIVE_REATTEMPT_IND:
					System.out.println("REATTEMPT is received.....\n");
						if(prntOpt == true){					
							System.out.println(((ReattemptIndEvent)isupEvent).toString());	
						}											
					break;

				case IsupConstants.ISUP_PRIMITIVE_ISUP_ERROR_IND:
					System.out.println("ERROR IND is received.....\n");
						if(prntOpt == true){
							System.out.println(((IsupErrorIndEvent)isupEvent).toString());	
						}					
					break;
				
				case IsupConstants.ISUP_PRIMITIVE_MTP_PAUSE:
					System.out.println("MTP PAUSE  is received.....\n");
						if(prntOpt == true){
							System.out.println(((NwStatusIndEvent)isupEvent).toString());	
						}
					break;

				case IsupConstants.ISUP_PRIMITIVE_MTP_RESUME:
					System.out.println("MTP RESUME  is received.....\n");
					try{
						if(prntOpt == true){	
							System.out.println(((NwStatusIndEvent)isupEvent).toString());	
						}
					}catch (Exception exception) {};
					break;

				case IsupConstants.ISUP_PRIMITIVE_MTP_STATUS_CONGESTION:
					System.out.println("STATUS CONGESTION  is received.....\n");
						if(prntOpt == true){
							System.out.println(((NwStatusIndEvent)isupEvent).toString());	
						}
					break;	

				case IsupConstants.ISUP_PRIMITIVE_MTP_STATUS_USER_PART:
					System.out.println("MTP USER PART  is received.....\n");
						if(prntOpt == true){
							System.out.println(((NwStatusIndEvent)isupEvent).toString());	
						}
					break;
		
				case IsupConstants.ISUP_PRIMITIVE_MAINT_TMO_IND:
					System.out.println("MNTC TIMEOUT IND  is received.....\n");
						if(prntOpt == true){
							System.out.println(((MaintenanceTimeoutIndEvent)isupEvent).toString());	
						}
					break;

				case IsupConstants.ISUP_PRIMITIVE_RAW_MESSAGE:
					System.out.println("RAW Message  is received.....\n");
					break;

				default:
					System.out.println("UNKNOWN PRIMITIVE is received.....\n");

			}
			}catch (Exception exception) {};
		}
		
	}


	public static void main(String args[]) throws IOException
	{

		int stackSpec;
		
		
		try
		{

			JainSS7Factory d7IsupFactory = JainSS7Factory.getInstance();

			// obtain a reference to the singleton Factory class and set the vendor path name
			d7IsupFactory.setPathName("com.ss8");

			JainIsupStack d7isupStack            = null;

			d7isupStack = (JainIsupStack)d7IsupFactory.createSS7Object("javax.jain.ss7.isup.JainIsupStackImpl");

			IsupUserAddress      myAddress      = null;
			JainIsupProvider     d7IsupProvider = null; 
			JainSampleIsupListener isupListener   = null;
				
			d7isupStack.setVendorName("cok.ss8");
			d7isupStack.setStackName("test");
			
			InputStreamReader isr = new InputStreamReader(System.in);
			BufferedReader bfr = new BufferedReader(isr);
			 
			String stsp = null;
			String sopc = null;
					
			System.out.print("Enter Own Point Code: ");
		
			sopc = bfr.readLine();	

			int first,second;
		
			first = sopc.indexOf("-");			
			String sopcmem = sopc.substring(0,first);						
			second  = sopc.lastIndexOf("-");				
			String sopcclus = sopc.substring(first+1,second);
			String sopczone = sopc.substring(second+1);
			
			SignalingPointCode OPC = new SignalingPointCode(Integer.parseInt(sopcmem),
																Integer.parseInt(sopcclus),
																Integer.parseInt(sopczone));

			d7isupStack.setSignalingPointCode(OPC); 
		
			System.out.print("Enter Destination Point Code: ");
			sopc = null;
			
			try{
				sopc = bfr.readLine();
			}	catch (IOException exception) {};
		
			if(sopc == null)
				sopc = bfr.readLine();	

			first = sopc.indexOf("-");			
			sopcmem = sopc.substring(0,first);						
			second  = sopc.lastIndexOf("-");				
			sopcclus = sopc.substring(first+1,second);
			sopczone = sopc.substring(second+1);
											
			SignalingPointCode  DPC = new SignalingPointCode(Integer.parseInt(sopcmem),
																Integer.parseInt(sopcclus),
																Integer.parseInt(sopczone));		
			
			
			
		
			String startCIC = null;
			String endCIC   = null;	
			System.out.print("Enter start CIC: ");
			startCIC = bfr.readLine();	
			System.out.print("Enter end CIC: ");
			endCIC = bfr.readLine();

			System.out.println("Stack specification: 1>ITU_93     2>ANSI_92    3>ANSI_95");
			
			System.out.print("Enter your choice: ");
										
			stsp    = bfr.readLine();		
							
			
			switch (Integer.parseInt(stsp)){
				case 1:
					d7isupStack.setStackSpecification(IsupConstants.ISUP_PV_ITU_1993);					
					myAddress = new IsupUserAddress(DPC,Integer.parseInt(startCIC),Integer.parseInt(endCIC),
																				NetworkIndicator.NI_INTERNATIONAL_00); 
					stackSpec = IsupConstants.ISUP_PV_ITU_1993;
					System.out.println("Protocol is set to ITU_93");
					break;
				case 2:
					d7isupStack.setStackSpecification(IsupConstants.ISUP_PV_ANSI_1992);			
					myAddress = new IsupUserAddress(DPC,Integer.parseInt(startCIC),Integer.parseInt(endCIC),
														NetworkIndicator.NI_NATIONAL_10); 
					stackSpec = IsupConstants.ISUP_PV_ANSI_1992;
					System.out.println("Protocol is set to ANSI_92");
					break;

				case 3:
					d7isupStack.setStackSpecification(IsupConstants.ISUP_PV_ANSI_1995);			
					myAddress = new IsupUserAddress(DPC,Integer.parseInt(startCIC),Integer.parseInt(endCIC),
																				NetworkIndicator.NI_NATIONAL_10); 
					stackSpec = IsupConstants.ISUP_PV_ANSI_1995;
					System.out.println("Protocol is set to ANSI_95");
					break;

				default:
					System.out.println("Default is set to ITU_93");
					d7isupStack.setStackSpecification(IsupConstants.ISUP_PV_ITU_1993);
					myAddress = new IsupUserAddress(DPC,Integer.parseInt(startCIC),Integer.parseInt(endCIC),
																				NetworkIndicator.NI_INTERNATIONAL_00);
					stackSpec = IsupConstants.ISUP_PV_ITU_1993;
					System.out.println("Protocol is set to ITU_93"); 
					break;
			}
		
			d7IsupProvider = d7isupStack.createProvider();
			isupListener = new JainSampleIsupListener();
			d7IsupProvider.addIsupListener(isupListener,myAddress);		

			/* Set the paramater values */
			UserServiceInfo usi = new UserServiceInfo((byte)0,(byte)0,false, (byte)16, (byte)0, false,
								(byte)2, (byte)0, (byte)4,false, (byte)0, (byte)1, false, (byte)1,
								(byte)1, false, (byte)2,(byte) 2, false, (byte)6, (byte) 3, false, 
								(byte) 1, false, (byte)0,
								false, false, false, false, false, false, false, (byte)0, false,
								false, false, false, false, false, false, false, (byte)0, (byte)0,
								(byte)0, false, (byte)0, false, false);

			UserServiceInfoPrime  usip = new UserServiceInfoPrime((byte)0, (byte)0,false, (byte)16, (byte)0, false,
											(byte)2, (byte)0, (byte)4,false, (byte)0, (byte)1, false, (byte)1, (byte)1,
											false, (byte)2, (byte) 2,false, (byte)6, (byte) 3, false, (byte) 1, false,
											(byte)0, false, false,
											false, false, false, false, false, (byte)0, false, 
											false, false, false,
											false, false, false, false, (byte)0, (byte)0, 
											(byte)0, false, (byte)0, false, false);

			byte[] status = {(byte)0};
			byte[] status2 = {(byte)255};
			byte[] status3 = {(byte)255,(byte)255};

			RangeAndStatus rns = 	new RangeAndStatus((byte)7,status2);

			RangeAndStatus rnsres = 	new RangeAndStatus((byte)22,status);

				
			RangeAndStatus rnsa = 	new RangeAndStatus((byte)0,null);				

			RangeAndStatus rnscgb = 	new RangeAndStatus((byte)10,status3);
				
			BwdCallIndItu bci = new BwdCallIndItu((byte) 1,
(byte)2,(byte) 2, (byte)2, false, false, false,false, false,
(byte)0,true); 

			BwdCallIndAnsi bcia = new BwdCallIndAnsi((byte)1, (byte)2, (byte)2, (byte)2, false, false, false, false, false, (byte)0, true);			
				
			CallDiversionInfoItu calldiv = new CallDiversionInfoItu((byte)2,(byte)1);

			EventInformation evi = new EventInformation((byte)2, false);
			
			EchoControlInfoItu eco = new EchoControlInfoItu((byte)1,(byte)1,(byte)1,(byte)1);

			NatureConnInd nci = new NatureConnInd();
			nci.setContCheckInd((byte)0);
			nci.setEchoControlDevInd(true);
			nci.setSatelliteInd((byte)2);

			FwdCallIndAnsi fcia = new FwdCallIndAnsi((byte)2, false, false, (byte)2 , false, (byte)0, false, true);

			FwdCallIndItu fci = new FwdCallIndItu (); 
			fci.setETEMethodInd ((byte)2); 
			fci.setInterworkingInd (false); 
			fci.setISDNAccessInd (false); 
			fci.setISDNUserPartInd (true); 
			fci.setISUPPreferenceInd ((byte)2); 
			fci.setSCCPMethodInd ((byte)0); 
			fci.setTypeOfCallInd (true); 
			fci.setEndToEndInfoInd (true);

			CalledPartyNumberItu cpn = new CalledPartyNumberItu (); 
			byte[] addrSig = {(byte)1,(byte)8,(byte)0,(byte)0,(byte)4, (byte)6,(byte)8,(byte)5,(byte)2,(byte)4,(byte)6}; 
			cpn.setAddressSignal(addrSig); 
			cpn.setNatureOfAddressInd((byte)1); 
			cpn.setNumberingPlanInd((byte)2); 
			cpn.setIntNwNumInd(true); 	

			byte[] accessTrans = {(byte)1,(byte)2}; 

			BusinessGroupAnsi bga = new BusinessGroupAnsi((byte)1, true, true, true, 15, 13, (short)3);

			CallReference crf = new CallReference(10,DPC); 
			
			CircuitAssignMapAnsi cam = new CircuitAssignMapAnsi((byte)8,36);

			CallingPartyNumber cgpn = new CallingPartyNumber();
			cgpn.setAddressSignal(addrSig); 
			cgpn.setNatureOfAddressInd((byte)1); 
			cgpn.setNumberingPlanInd((byte)2);
			cgpn.setScreeningInd((byte)1);

			ConnectedNumberItu connum = new ConnectedNumberItu((byte)1,(byte)2,(byte)1,(byte)2,addrSig);

			ConReq conr = new ConReq(3,DPC,(short)1,(short)4);
			byte[] accesstrans = {(byte)0,(byte)1,(byte)2};

			byte[] psamsg =
{(byte)1,(byte)0,(byte)160,(byte)1,(byte)10,(byte)0,(byte)2,(byte)9,(byte)7,(byte)1,(byte)0,(byte)18,(byte)133,(byte)57,(byte)112,(byte)148,(byte)0};

			CircuitGrpCharacteristicsIndAnsi cgci = new CircuitGrpCharacteristicsIndAnsi((byte)0, (byte)1, (byte)0, (byte)2);

			byte[] nID = {(byte)5,(byte)4};
			String  networkIdentity = new String(nID,0,2);
				
			CUGInterlockCodeItu cug = new CUGInterlockCodeItu(772, networkIdentity);
			

			byte[] diagnostic = {(byte)0, (byte)1};	
			CauseInd csi = new CauseInd((byte)0, (byte)1, (byte)1,diagnostic,false,false);
			
			byte[] digis = {(byte)0, (byte)1,(byte)2, (byte)3};	

			CarrierIdAnsi cic = new CarrierIdAnsi((byte)6, (byte)7,digis);			

			ChargeNumberAnsi chgnumA = new ChargeNumberAnsi((byte)1, (byte)2, digis);
				

			byte[] town = {(byte)50,(byte)51,(byte)52,(byte)53};
			String stown  = new String(town);
			byte[] state = {(byte)41,(byte)42};
			byte[] building = {(byte)43,(byte)44};
			byte[] subdiv = {(byte)45,(byte)46,(byte)47};
			String sstate = new String(state);
			String sbuilding = new String(building);
			String ssubdiv = new String(subdiv);
		

			CommonLangLocationIdAnsi cll = new CommonLangLocationIdAnsi(stown,sstate, sbuilding,ssubdiv);

			CircuitIdNameAnsi cidn = new CircuitIdNameAnsi((long)1, cll, cll);

			String sstr = new String(digis);
			GenericNameAnsi gnm = new GenericNameAnsi((byte)1, false, (byte)1, sstr);

			GenericDigits gdn = new GenericDigits((byte)1,(byte)2,digis);				

			GenericNumber gnno = new GenericNumber((short)1,(byte)2, (byte)2, (byte)1,digis);

			LocationNumberItu lno = new LocationNumberItu((byte)1,false, (byte)1, (byte)0, (byte)1,digis);	

			OrigCalledNumber ocld =  new OrigCalledNumber((byte)1, (byte)0, (byte)0,digis);

			InfoInd infind = new InfoInd((byte)0,true,false,true,false); 
			InfoIndAnsi infinda = new InfoIndAnsi((byte)0,true, false, true, false,false);				

			InfoReqInd inrind = new InfoReqInd(true,false, true,false,false);
			InfoReqIndAnsi inrinda = new InfoReqIndAnsi(true,false, true, false,false,true);

			byte[] subsnum = {(byte)0,(byte)1,(byte)2};
			byte[] uuinfo  = {(byte)0,(byte)1,(byte)2};
			byte[] circstateind = {(byte)0,(byte)1,(byte)2};

			OperatorServicesInfoAnsi opsi = new OperatorServicesInfoAnsi((byte)1, (byte)3);

			OptionalBwdCallIndAnsi obi = new OptionalBwdCallIndAnsi(false,true,false,true);
				
			OptionalBwdCallIndItu obiitu = new OptionalBwdCallIndItu(false,true,false,true);	

			OptionalFwdCallIndItu ofiitu = new OptionalFwdCallIndItu((byte)1, false, false);

			byte[] origiscpc = {(byte)1,(byte)255};

			UserToUserIndicators uuind = new UserToUserIndicators(true,false); 

			UserToUserIndicatorsItu uuinditu = new UserToUserIndicatorsItu(false, false, (byte)1, (byte)1, (byte)0);

			MCIDReqIndItu  mcidreq = new MCIDReqIndItu(true,true);	
			MCIDRespIndItu mcidresp = new 	MCIDRespIndItu(true,true);	

			byte[] msgc = {(byte)1,(byte)2,(byte)131};
			MessageCompatibilityInfoItu msgcomp = new MessageCompatibilityInfoItu(msgc);

			byte[] outtrnknum = {(byte)45,(byte)46,(byte)47,(byte)48,(byte)49,(byte)50};
			String ogTgpNum = new String(outtrnknum);

			Precedence prec = new Precedence((byte)1, (byte)0, digis,12307677); 

			byte[] pmtcompat = 	{(byte)2,(byte)1,(byte)131,(byte)2,(byte)132};			
				
			ParamCompatibilityInfoItu pmtcmp = new ParamCompatibilityInfoItu(pmtcompat);

			RedirectingNumber rdn = new RedirectingNumber((byte)1, (byte)0, (byte)0, digis);

			RedirectionInfo rdi = new RedirectionInfo((byte)3, (byte) 0, (byte)3, (byte) 1);

			RedirectionNumberItu rdnoitu = new RedirectionNumberItu((byte)1, (byte)1, digis,false);

			RemoteOperations rmop = new RemoteOperations((byte)1, true, digis);
				
			byte[] nwid = {(byte)250};

			NwSpecificFacItu nwsitu = new NwSpecificFacItu((short)2, (byte)10, (byte)7, false,nwid, circstateind);

			NotificationInd nind = new NotificationInd((byte)1,true);
				
			TransactionReqAnsi trs = new TransactionReqAnsi((long)4,diagnostic);

			TransitNwSel tns = new TransitNwSel((byte)0, (byte)1,digis);

			InputStreamReader is = new InputStreamReader(System.in);
			BufferedReader br = new BufferedReader(is,1);
			 
			String choice		= null;			
			String stringCIC    = null;
			int    CIC			= 0;
					
			while(true){
				

				try{
					
						Menu();
						System.out.print("Enter your choice: ");

						while(true){
							try{
								choice    = br.readLine();		
								break;
							}catch (Exception e){};
						}
						if(	(Integer.parseInt(choice) != 100)  && (Integer.parseInt(choice) != 99)){								
								System.out.print("Enter CIC: ");	
		
								stringCIC = bfr.readLine();					
						
								CIC = Integer.parseInt(stringCIC);	
						}
									
					switch (Integer.parseInt(choice)){
				
						case 1:							
							if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){
								SetupItuEvent setup = new SetupItuEvent(isupListener, DPC, OPC, (byte)3, CIC, (byte)0, 
								false, nci, fci, cpn,(byte)1,(byte)2); 			
			
								setup.setAccessTransport (accessTrans);												
								setup.setCallReference(crf);			
								setup.setCallingPartyNumber(cgpn);			
								setup.setConReq(conr);
								setup.setCUGInterlockCode(cug);		
								setup.setGenericDigits(gdn);
								setup.setGenericNumber(gnno);					
								setup.setLocationNumber(lno);
								setup.setMLPPprecedence(prec);
								setup.setNotificationInd(nind);
								setup.setNwSpecificFac(nwsitu);
								setup.setOptionalFwdCallInd(ofiitu);
								setup.setOrigCalledNumber(ocld);							
							//	setup.setOrigISCPointCode(origiscpc);
								setup.setParamCompatibilityInfo(pmtcmp);
								setup.setPropagationDelayCounter(511);
								setup.setRedirectingNumber(rdn);
								setup.setRedirectionInfo(rdi);
								setup.setRemoteOperations(rmop);
								setup.setServiceActivation(digis);
							//	setup.setTransitNwSel(tns);
								setup.setTransmissionMediumReqPrime((byte)3);
								setup.setUserToUserIndicators(uuinditu);
								setup.setUserToUserInformation(digis);
								setup.setUserServiceInfo(usi);
								setup.setUserServiceInfoPrime(usip);
								

								d7IsupProvider.sendIsupEvent (setup);
							}
							else{
								SetupAnsiEvent setupa = new SetupAnsiEvent(isupListener, DPC, OPC, (byte)3, CIC, (byte)0, 
								false, nci, fcia, cpn,(byte)1,usi); 
								
								setupa.setAccessTransport(accessTrans);
						//		setupa.setBusinessGroup(bga);
								setupa.setCallReference(crf);			
								setupa.setCallingPartyNumber(cgpn);
								setupa.setCarrierId(cic);
								setupa.setCarrierSelInfo((short)2);
						//		setupa.setChargeNumber(chgnumA);
								setupa.setCircuitAssignMap(cam);								
								setupa.setConReq(conr);																
						//		setupa.setGenericDigits(gdn);
								setupa.setEgressService(digis);		
								setupa.setGenericName(gnm);						
								setupa.setHopCounter((byte)13);
								setupa.setInfoReqInd(inrinda);
								setupa.setJurisdiction(digis);
						 //		setupa.setMLPPprecedence(prec);
								setupa.setNetworkTransport(diagnostic);
								setupa.setOperatorServicesInfo(opsi);
						//		setupa.setOrigCalledNumber(ocld);
								setupa.setOrigLineInfo((byte)2);
						//		setupa.setRedirectingNumber(rdn);
								setupa.setRedirectionInfo(rdi);
    					//		setupa.setRemoteOperations(rmop);
						//		setupa.setServiceActivation(digis);
								setupa.setServiceCode((short) 3);
								setupa.setSpecialProcReq((short)4);
								setupa.setTransactionReq(trs);
						//		setupa.setTransitNwSel(tns);
						//		setupa.setUserServiceInfoPrime(usip);
								setupa.setUserToUserInformation(digis);
								
								d7IsupProvider.sendIsupEvent (setupa);
							}												
							break;
						case 2:			
				
							if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){	
								AlertItuEvent alert = new AlertItuEvent(isupListener, DPC, OPC, (byte)0, CIC, 
																						(byte)0,bci);
								alert.setAccessTransport(accesstrans);
								alert.setAccessDeliveryInfo((byte)4);
								alert.setCallDiversionInfo(calldiv);
								alert.setCauseInd(csi);
								alert.setEchoControlInfo(eco);	
								alert.setOptionalBwdCallInd(obiitu);
								alert.setRedirectionNumber(rdnoitu);
								alert.setRedirectionNumberRest((byte)1);
								alert.setTransmissionMediumUsed((byte)2);

								d7IsupProvider.sendIsupEvent (alert); 
							}
							else{
								AlertAnsiEvent alerta = new AlertAnsiEvent(isupListener, DPC, OPC, (byte)0, CIC, 
																								(byte)0,bcia);
								alerta.setAccessTransport(accesstrans);
						//		alerta.setBusinessGroup(bga);
								alerta.setCallReference(crf);		
								alerta.setCauseInd(csi);
								alerta.setConReq(conr);		
								alerta.setInfoInd(infinda);
								alerta.setNetworkTransport(diagnostic);
								alerta.setNotificationInd(nind);
								alerta.setOptionalBwdCallInd(obi);
								alerta.setTransmissionMediumUsed((byte)2);
								alerta.setUserToUserIndicators(uuind);

								d7IsupProvider.sendIsupEvent (alerta);
							}
							break; 
						case 3:		
							
							

							if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){
										
								AnswerItuEvent answer = new AnswerItuEvent(isupListener, DPC, OPC, (byte)0, 
																				CIC, (byte)0);
								answer.setRedirectionNumberRest((byte)3);
								answer.setCallHistoryInfo((long)2571);
								answer.setConnectedNumber(connum);								

								d7IsupProvider.sendIsupEvent (answer);
							}
							else{
								AnswerAnsiEvent answera = new AnswerAnsiEvent(isupListener, DPC, OPC, (byte)0, CIC, 
																										(byte)0);
								answera.setAccessTransport(accesstrans);
								
								d7IsupProvider.sendIsupEvent (answera);
							}
							break; 
						case 4:				 			
							CircuitMaintenanceEvent resetEvent = new
														CircuitMaintenanceEvent(isupListener,DPC,OPC,(byte)0,CIC,
														(byte)0,IsupConstants.ISUP_PRIMITIVE_RESET_CIRCUIT);
							d7IsupProvider.sendIsupEvent(resetEvent);
							break; 
						case 5:					 						
								CircuitMaintenanceEvent blockEvent = new
												CircuitMaintenanceEvent(isupListener,DPC,OPC,(byte)0,CIC,(byte)0,
												IsupConstants.ISUP_PRIMITIVE_BLOCKING);
							d7IsupProvider.sendIsupEvent (blockEvent); 
							break; 
						case 6:				 			
							CircuitMaintenanceEvent blockAckEvent = new
												CircuitMaintenanceEvent(isupListener,DPC,OPC,(byte)0,CIC,(byte)0,
												IsupConstants.ISUP_PRIMITIVE_BLOCKING_ACK);
							d7IsupProvider.sendIsupEvent (blockAckEvent); 
							break; 
						case 7:				 			
							CircuitMaintenanceEvent unblockEvent = new
												CircuitMaintenanceEvent(isupListener,DPC,OPC,(byte)0,CIC,(byte)0,
												IsupConstants.ISUP_PRIMITIVE_UNBLOCKING);
							d7IsupProvider.sendIsupEvent (unblockEvent); 
							break; 
						case 8:				 			
							CircuitMaintenanceEvent unblockAckEvent = new
													CircuitMaintenanceEvent(isupListener,DPC,OPC,(byte)0,CIC,(byte)0,
													IsupConstants.ISUP_PRIMITIVE_UNBLOCKING_ACK);
							d7IsupProvider.sendIsupEvent (unblockAckEvent); 
							break; 
						case 9:			 			
							CircuitMaintenanceEvent cotreqEvent = new 
													CircuitMaintenanceEvent(isupListener,DPC,OPC,(byte)0,CIC,(byte)0,
													IsupConstants.ISUP_PRIMITIVE_CONTINUITY_CHECK_REQ);
							d7IsupProvider.sendIsupEvent (cotreqEvent); 
							break;
						case 10:	
							if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){	
								CircuitMaintenanceEvent ovlEvent = new
													CircuitMaintenanceEvent(isupListener,DPC,OPC,(byte)0,CIC,(byte)0,
													IsupConstants.ISUP_PRIMITIVE_OVERLOAD);			 			
								d7IsupProvider.sendIsupEvent(ovlEvent); 

							}
							else
								System.out.println("Not supported for ANSI");
							break; 
						case 11:				 			
							CircuitMaintenanceEvent lbaEvent = new
													CircuitMaintenanceEvent(isupListener,DPC,OPC,(byte)0,CIC,(byte)0,
													IsupConstants.ISUP_PRIMITIVE_LOOPBACK_ACK);
							d7IsupProvider.sendIsupEvent (lbaEvent); 
							break; 
						case 12:				 			
							CircuitMaintenanceEvent ucicEvent = new
													CircuitMaintenanceEvent(isupListener,DPC,OPC,(byte)0,CIC,(byte)0,
													IsupConstants.ISUP_PRIMITIVE_UNEQUIPPED_CIC);
							d7IsupProvider.sendIsupEvent (ucicEvent); 
							break; 
						case 13:		
							if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){		 			
								CircuitGrpMaintenanceEvent cgbEvent = new
													CircuitGrpMaintenanceEvent(isupListener,DPC,OPC,(byte)0,CIC,(byte)0,
													IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_BLOCKING,rns,(byte)0);
								d7IsupProvider.sendIsupEvent(cgbEvent);
							}
							else{
								CircuitGrpMaintenanceEvent cgbEventa = new
													CircuitGrpMaintenanceEvent(isupListener,DPC,OPC,(byte)0,CIC,(byte)0,
													IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_BLOCKING,rnscgb,(byte)0);
								d7IsupProvider.sendIsupEvent(cgbEventa);

							}
				
							break; 
						case 14:	
										 			
							 CircuitGrpMaintenanceEvent cgbaEvent = new
													CircuitGrpMaintenanceEvent(isupListener,DPC,OPC,(byte)0,CIC,
											(byte)0,IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_BLOCKING_ACK,rns,(byte)0);
							d7IsupProvider.sendIsupEvent (cgbaEvent); 
							break; 
						case 15:				 			
							CircuitGrpMaintenanceEvent cguEvent = new
													CircuitGrpMaintenanceEvent(isupListener,DPC,OPC,(byte)0,CIC,(byte)0,
													IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_UNBLOCKING,rns,(byte)0);
							d7IsupProvider.sendIsupEvent (cguEvent); 
							break; 
						case 16:				 			
							CircuitGrpMaintenanceEvent cguaEvent = new
													CircuitGrpMaintenanceEvent(isupListener,DPC,OPC,(byte)0,CIC,
										(byte)0,IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_UNBLOCKING_ACK,rns,(byte)0);
							d7IsupProvider.sendIsupEvent (cguaEvent); 
							break;
						case 17:					 			
							if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){
								CallProgressItuEvent cpgEvent = new CallProgressItuEvent(isupListener,DPC,OPC,(byte)0,
																						CIC,(byte)0,	evi);
								d7IsupProvider.sendIsupEvent(cpgEvent);
							}
							else{
								CallProgressAnsiEvent cpgEventa = new CallProgressAnsiEvent(isupListener,DPC,OPC,
																		(byte)0,CIC,(byte)0,	evi);
								cpgEventa.setAccessTransport(accesstrans);
								d7IsupProvider.sendIsupEvent(cpgEventa);

							}
							break;
						case 18:					 			
							ConnectItuEvent conEvent = new ConnectItuEvent(isupListener,DPC,OPC,(byte)0,CIC,
																										(byte)0,bci);
							d7IsupProvider.sendIsupEvent (conEvent); 
							break; 
						case 19:	
							if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){
								FacilityAcceptItuEvent fcaEvent = new FacilityAcceptItuEvent(isupListener,DPC,OPC,
																		(byte)0,CIC,(byte)0,(short)1);
								d7IsupProvider.sendIsupEvent(fcaEvent); 
							}
							else
								System.out.println("Not supported for ANSI");
							break; 
						case 20:		
							if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){
								FacilityItuEvent facEvent = new FacilityItuEvent(isupListener,DPC,OPC,(byte)0,CIC,
																											(byte)0);
								d7IsupProvider.sendIsupEvent(facEvent);
							}
							else{
								FacilityEvent facEventa = new FacilityEvent(isupListener,DPC,OPC,(byte)0,CIC,(byte)0);
								d7IsupProvider.sendIsupEvent(facEventa);
							}
							break; 
						case 21:	
							if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){
								FacilityRejectItuEvent fcjEvent = new FacilityRejectItuEvent(isupListener,DPC,OPC,
																				(byte)0,CIC,(byte)0,(short)1,csi);
								d7IsupProvider.sendIsupEvent (fcjEvent); 
							}
							else
								System.out.println("Not supported for ANSI");
							break; 
						case 22:		
							if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){
								FacilityRequestItuEvent fcrEvent = new
																FacilityRequestItuEvent(isupListener,DPC,OPC,
																(byte)0,CIC,(byte)0,(short)1);
								d7IsupProvider.sendIsupEvent (fcrEvent); 
							}
							else
								System.out.println("Not supported for ANSI");
							break; 
						case 23:				 			
							PassAlongEvent pasEvent = new PassAlongEvent(isupListener, DPC,OPC, (byte)0, CIC, (byte)0);
							pasEvent.setEmbeddedMessage(psamsg);
							d7IsupProvider.sendIsupEvent (pasEvent); 
							break; 
						case 24:				 			
							IdentificationReqItuEvent idrEvent = new IdentificationReqItuEvent(isupListener, 
																DPC,OPC, (byte)0, CIC, (byte)0);
							idrEvent.setMCIDReqInd(mcidreq);
							idrEvent.setMessageCompatibilityInfo(msgcomp);

							d7IsupProvider.sendIsupEvent (idrEvent); 
							break; 
						case 25:				 			
							IdentificationRespItuEvent irsEvent = new IdentificationRespItuEvent(isupListener, DPC,OPC,
																				(byte)0, CIC, (byte)0);
							irsEvent.setMCIDRespInd(mcidresp);	

							d7IsupProvider.sendIsupEvent (irsEvent); 
							break; 
						case 26:				 			
							if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){
								InformationItuEvent infEvent = new InformationItuEvent(isupListener, 
												DPC,OPC, (byte)0, CIC, (byte)0,infind);
								d7IsupProvider.sendIsupEvent(infEvent); 
							}
							else{
								InformationAnsiEvent infEventa = new InformationAnsiEvent(isupListener, DPC,OPC, 
																		(byte)0, CIC, (byte)0,infinda);
								d7IsupProvider.sendIsupEvent(infEventa);
							} 
							break; 
						case 27:				 			
							if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){
								InformationReqItuEvent inrEvent = new InformationReqItuEvent(isupListener, DPC,OPC, 
																		(byte)0, CIC, (byte)0,inrind);
								d7IsupProvider.sendIsupEvent(inrEvent); 
							}
							else{
								InformationReqAnsiEvent inrEventa = new InformationReqAnsiEvent(isupListener, DPC,OPC,
																				(byte)0, CIC, (byte)0,inrinda);
								inrEventa.setNetworkTransport(digis);
								inrEventa.setCallReference(crf);	
								d7IsupProvider.sendIsupEvent(inrEventa); 
							}
							break; 
						case 28:				 			
							NetworkResourceMgmtItuEvent nrmEvent = new
																NetworkResourceMgmtItuEvent(isupListener, 
																				DPC,OPC, (byte)0, CIC,(byte)0);
							d7IsupProvider.sendIsupEvent (nrmEvent); 
							break; 
						case 29:				 			
							if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){
								ReleaseItuEvent relEvent = new ReleaseItuEvent(isupListener, DPC,OPC, (byte)0, CIC,
																		(byte)0, csi);
								relEvent.setAutoCongLevel((byte)2);
								relEvent.setSignalingPointCode(DPC);

								d7IsupProvider.sendIsupEvent(relEvent); 
							}
							else{
								ReleaseAnsiEvent relEventa = new ReleaseAnsiEvent(isupListener, DPC,OPC, (byte)0, CIC,
																		(byte)0, csi);
								relEventa.setAutoCongLevel((byte)2);									

								d7IsupProvider.sendIsupEvent(relEventa);
							}
							break; 
						case 30:
							if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){
								ReleaseResponseItuEvent relpEvent = new ReleaseResponseItuEvent(isupListener, DPC,OPC,
																				(byte)0, CIC,(byte)0);
					 			
								d7IsupProvider.sendIsupEvent(relpEvent); 
							}
							else{
								ReleaseResponseEvent relpEventa = new
												ReleaseResponseEvent(isupListener, DPC,OPC, (byte)0, CIC, (byte)0);		
								d7IsupProvider.sendIsupEvent(relpEventa);
							}
							break; 
						case 31:				 			
							SegmentationItuEvent sgmEvent = new SegmentationItuEvent(isupListener, DPC,OPC, (byte)0, 
																						CIC,(byte)0);
							d7IsupProvider.sendIsupEvent (sgmEvent); 
							break; 
						case 32:	
							if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){
								SubsequentAddressItuEvent samEvent = new SubsequentAddressItuEvent(isupListener, 
																		DPC,OPC, (byte)0, CIC,(byte)0,subsnum);
								
								samEvent.setSubsequentNumber(digis);
								
								d7IsupProvider.sendIsupEvent(samEvent); 
							}
							else
								System.out.println("Not supported for ANSI");
							break; 
						
						case 33:		
							if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){
		 			
								UserToUserInfoItuEvent uuiEvent = new UserToUserInfoItuEvent(isupListener, DPC,OPC, 
																		(byte)0, CIC, (byte)0,uuinfo);
								d7IsupProvider.sendIsupEvent(uuiEvent);
							}
							else
								System.out.println("Not supported for ANSI");							
							break;

						case 34:				 			
							CircuitGrpQueryEvent cqmEvent = new CircuitGrpQueryEvent(isupListener, DPC,OPC, (byte)0,CIC,
												(byte)0,IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_QUERY ,rns);
							d7IsupProvider.sendIsupEvent (cqmEvent); 
							break;
						case 35:				 			
							CircuitGrpQueryEvent cqmlocEvent = new CircuitGrpQueryEvent(isupListener, DPC,OPC, (byte)0,
												CIC, (byte)0,IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_LOCAL_QUERY ,rns);
							d7IsupProvider.sendIsupEvent (cqmlocEvent); 
							break;
						
						case 36:	
							if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){
								CircuitGrpResetEvent grsEvent = new CircuitGrpResetEvent(isupListener, DPC,OPC, 
																		(byte)0, CIC,(byte)0,rnsres);
								d7IsupProvider.sendIsupEvent (grsEvent); 
							}
							else{
								CircuitGrpResetAnsiEvent grsEventa = new CircuitGrpResetAnsiEvent(isupListener, DPC,OPC,
																		(byte)0, CIC,(byte)0,rnsa);
								d7IsupProvider.sendIsupEvent(grsEventa);
							}			 			
							break;
						case 37:				 			
							if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){
								CircuitGrpResetAckEvent graEvent = new
																CircuitGrpResetAckEvent(isupListener, DPC,OPC, 
																		(byte)0, CIC,(byte)0,rnsres);				
								d7IsupProvider.sendIsupEvent(graEvent); 
							}
							else{
								CircuitGrpResetAckAnsiEvent graEventa = new CircuitGrpResetAckAnsiEvent(isupListener, 
																				DPC,OPC, (byte)0, CIC, (byte)0,rnsa);	
								d7IsupProvider.sendIsupEvent (graEventa);
							}
							break;
						case 38:				 			
							ConfusionEvent cfnEvent = new ConfusionEvent(isupListener, DPC,OPC, (byte)0, CIC,
																				(byte)0,csi);
							d7IsupProvider.sendIsupEvent (cfnEvent); 
							break;
						case 39:				 			
							ContinuityEvent cotEvent = new ContinuityEvent(isupListener, DPC,OPC, (byte)0, CIC,
																				(byte)0,(byte)1);
							d7IsupProvider.sendIsupEvent (cotEvent); 
							break;
						case 40:				 			
							ForwardTransferEvent fotEvent = new ForwardTransferEvent(isupListener, DPC,OPC, (byte)0, 
																						CIC,(byte)0);
							d7IsupProvider.sendIsupEvent (fotEvent); 
							break;
						case 41:				 			
							SuspendResumeEvent susEvent = new SuspendResumeEvent(isupListener, DPC,OPC, (byte)0, CIC,
																(byte)0,IsupConstants.ISUP_PRIMITIVE_SUSPEND,(byte)1);
							
							susEvent.setSuspendResumeInd((byte)1);

							d7IsupProvider.sendIsupEvent (susEvent); 
							break;
						case 42:				 			
							SuspendResumeEvent resumeEvent = new SuspendResumeEvent(isupListener, DPC,OPC, (byte)0,CIC,
																(byte)0,IsupConstants.ISUP_PRIMITIVE_RESUME,(byte)1);
							d7IsupProvider.sendIsupEvent (resumeEvent); 
							break;
						case 43:				 			
							CircuitReservationAnsiEvent crmEvent = new
																CircuitReservationAnsiEvent(isupListener, DPC,
																				OPC, (byte)0,CIC, (byte)0,nci);
							d7IsupProvider.sendIsupEvent (crmEvent); 
							break;
						case 44:				 			
							CircuitValidationRespAnsiEvent cvrEvent = new CircuitValidationRespAnsiEvent(isupListener,
																		DPC,OPC, (byte)0, CIC, (byte)0, (short)2, cgci);
							cvrEvent.setCircuitIdName(cidn);
							cvrEvent.setCommonLangLocationId(cll);

							d7IsupProvider.sendIsupEvent (cvrEvent); 
							break;
						case 45:				 			
							ExitAnsiEvent exitEvent = new ExitAnsiEvent(isupListener,DPC,OPC,(byte)0,CIC,(byte)0);
															
							exitEvent.setOutgoingTrunkGrpNumber(ogTgpNum);
							d7IsupProvider.sendIsupEvent (exitEvent); 
							break;
						case 46:		
							if(stackSpec != IsupConstants.ISUP_PV_ITU_1993){
								CircuitMaintenanceEvent craEvent = new
												CircuitMaintenanceEvent(isupListener,DPC,OPC,(byte)0,CIC,(byte)0,
											IsupConstants.ISUP_PRIMITIVE_CIRCUIT_RESERVATION_ACK);			 	
								d7IsupProvider.sendIsupEvent(craEvent); 
							}
							else
								System.out.println("Not supported in ITU");
							break;
						case 47:			
							if(stackSpec != IsupConstants.ISUP_PV_ITU_1993){
								CircuitMaintenanceEvent cvtEvent = new
												CircuitMaintenanceEvent(isupListener,DPC,OPC,(byte)0,CIC,(byte)0,
															IsupConstants.ISUP_PRIMITIVE_CIRCUIT_VALIDATION_TEST);		
								d7IsupProvider.sendIsupEvent(cvtEvent); 
							}
							else
								System.out.println("Not supported in ITU");

							break;

						case 48:
							StopMaintReqEvent stmntc = new StopMaintReqEvent(isupListener,DPC,OPC,(byte)0,CIC,(byte)0);
							d7IsupProvider.sendIsupEvent(stmntc);
							break; 

						case 99:
							if(prntOpt == true)
								prntOpt = false;
							else
								prntOpt = true;
							break;

						case 100:
							d7IsupProvider.removeIsupListener(isupListener,myAddress);
							System.out.println("Exiting...");
							return;	
											
						default:	
							System.out.println(Integer.parseInt(choice));					
							break;
					} 
				
				}catch (Exception e){};
				
			}

			
		}
		catch (Exception exception) 
		{				
			exception.printStackTrace(System.out);
		}
	}	
		
	private static void Menu(){
		
		System.out.println("*************************************************\n");
		System.out.println("1 ... ISUP_SETUP            2 ... ISUP_ALERT");
		System.out.println("3 ... ISUP_ANSWER           4 ... ISUP_RESET");					
		System.out.println("5 ... ISUP_BLOCKING         6 ... ISUP_BLOCKING_ACK");
		System.out.println("7 ... ISUP_UNBLOCKING       8 ... ISUP_UNBLOCKING_ACK");
		System.out.println("9 ... ISUP_CONT_CHCK_REQ    10 ...ISUP_OVERLOAD ");
		System.out.println("11 ...ISUP_LOOPBACK_ACK     12 ...ISUP_UCIC");
		System.out.println("13 ...ISUP_CGB              14 ...ISUP_CGBA ");
		System.out.println("15 ...ISUP_CGU              16 ...ISUP_CGUA");
		System.out.println("17 ...ISUP_CALL_PROGRESS    18 ...ISUP_CONNECT");
		System.out.println("19 ...ISUP_FACILITY_ACCEPT  20 ...ISUP_FACILITY");
		System.out.println("21 ...ISUP_FACILITY_REJECT  22 ...ISUP_FACILITY_REQUEST");
		System.out.println("23 ...ISUP_PASSALONG        24 ...ISUP_IDREQ");
		System.out.println("25 ...ISUP_IDRES            26 ...ISUP_INFO");
		System.out.println("27 ...ISUP_INFO_REQ         28 ...ISUP_NETW_RES_MGMT");
		System.out.println("29 ...ISUP_RELEASE          30 ...ISUP_REL_RESP");
		System.out.println("31 ...ISUP_SEGMENTATION     32 ...ISUP_SAM");			
		System.out.println("33 ...ISUP_UUI              34 ...ISUP_CQM");	
		System.out.println("35 ...ISUP_CQM_LOCAL        36 ...ISUP_GRS");
		System.out.println("37 ...ISUP_GRA              38 ...ISUP_CONFUSION");
		System.out.println("39 ...ISUP_CONTINUITY       40 ...ISUP_FOT");
		System.out.println("41 ...ISUP_SUSPEND          42 ...ISUP_RESUME");
		System.out.println("43 ...ISUP_CIRCUIT_RESERV   44 ...ISUP_CIRCUIT_VALIDATION_RESP");
		System.out.println("45 ...ISUP_EXIT             46 ...ISUP_CIRCUIT_RSV_ACK");
		System.out.println("47 ...ISUP_CIRCUIT_VLD_TEST 48 ...ISUP_STOP_MAINTENANCE");
		System.out.println("99 ...Reset Print Option");
		System.out.println("100...QUIT");
		System.out.println("************************************************\n");
		


	}

	static boolean prntOpt = true;
	
	
}




