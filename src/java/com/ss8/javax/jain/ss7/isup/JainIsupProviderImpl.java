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
* VERSION      : 1.1 
* DATE         : 2008/05/16 12:23:18
* 
* MODULE NAME  : JainIsupProviderImpl.java,v
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
import javax.jain.ss7.isup.ansi.*;


////////////////////////////////////////////////////////////////////////////////////
//  CLASS JainIsupProviderImpl
////////////////////////////////////////////////////////////////////////////////////
public class JainIsupProviderImpl implements JainIsupProvider {

	static final int NONE               = IsupConstants.NOT_SET;

    static final int SUCCESS            = 0;
    static final int FAIL               = -1;
    static final int FAIL_SYSAPI        = -1;
    static final int FAIL_INVINP        = -2;
    static final int FAIL_NOTREG        = -3;
    static final int FAIL_MAXCON        = -4;
    static final int FAIL_ILL_IDX       = -5;
    static final int FAIL_WRONG_SIZE    = -6;
    static final int FAIL_WRONG_PROTO   = -7;
    static final int FAIL_SYSTEM        = -8;
    static final int FAIL_MTP           = -9;
    static final int FAIL_NO_MEM        = -10;
    static final int FAIL_WRONG_INFO    = -11;
	static final int FIXED_PARAMETER    = 1;
	static final int VARIABLE_PARAMETER = 2;
	static final int OPTIONAL_PARAMETER = 3;

	////////////////////////////////////////////////////////////////////////////////
    // static()
    ////////////////////////////////////////////////////////////////////////////////

	static {

	initIsupProvider();

    }

	////////////////////////////////////////////////////////////////////////////////
    // JainIsupProviderImpl()
    ////////////////////////////////////////////////////////////////////////////////

		
	JainIsupProviderImpl(JainIsupStackImpl d7Stack) throws ClassNotFoundException {
        ownStack = d7Stack;
    }

	////////////////////////////////////////////////////////////////////////////////
    // cleanup()
    ////////////////////////////////////////////////////////////////////////////////

	public void cleanup() {
		
		JainIsupListener isupListener = null;
        IsupUserAddress  isupAddress = null;
		int rx;
		
		if(!listeners.isEmpty()){
			Enumeration e = listeners.keys();
			while(e.hasMoreElements() ){
				isupAddress  = (IsupUserAddress)e.nextElement();
				isupListener = (JainIsupListener)listeners.get(isupAddress);
				
				try{
					removeIsupListener(isupListener,isupAddress);
			
				} catch (JainException exc) {};

			}
		}			

        // remove stack reference
		ownStack = null;
 
	}

	////////////////////////////////////////////////////////////////////////////////
    // sendDeregistrationEvent()
    ////////////////////////////////////////////////////////////////////////////////
	
    private void sendDeregistrationEvent() {
    
        cleanup();
    }

	////////////////////////////////////////////////////////////////////////////////
    // addIsupListener()
    ////////////////////////////////////////////////////////////////////////////////

	public void addIsupListener(JainIsupListener isupListener,
                            IsupUserAddress isupAddress)
                     throws java.util.TooManyListenersException,
                            ListenerAlreadyRegisteredException,
                            InvalidListenerException,
                            InvalidAddressException,
                            SendStackException{

			int regIdx = 0;
			int pc =0, i;
			
			int spNo = ((JainIsupStackImpl) getStack()).getSPNo();

			if (isupListener == null) {
				throw new InvalidListenerException("Listener cannot be null");

			}					

			try {

				int endCIC                 = isupAddress.getEndCIC();
				int startCIC               = isupAddress.getStartCIC();
				int stackSpec              = getStack().getStackSpecification();

				SignalingPointCode userDPC  = 
										isupAddress.getSignalingPointCode();

				
				SignalingPointCode ownPC =
										ownStack.getSignalingPointCode();  
			
				

				if(stackSpec == IsupConstants.ISUP_PV_ITU_1993)				
						pc = ((userDPC.getMember() &0x07)<<11)|
								((userDPC.getCluster()&0x00FF) << 3)| 
								(userDPC.getZone()&0x07);
				else 
						pc = ((userDPC.getMember() & 0x00FF)<<16)|
							((userDPC.getCluster() & 0x00FF) << 8)|
							(userDPC.getZone() & 0x00FF);
				
									
						// register to ISUP
						if((regIdx = isupReg(spNo, 
										 getStack().getStackSpecification(), 
										 startCIC,
										 endCIC,
										 pc,
										 isupAddress)) < 0){  
								throw new InvalidAddressException("isup registration failed ");
						}
												
						VectorAddress addr = new VectorAddress(userDPC,startCIC, endCIC, regIdx);
					
						usrAddrLookup.add(addr);
											
						listeners.put(isupAddress, isupListener);
						idxLookup.put(isupAddress,new Integer(regIdx));												
					
	
			} catch (JainException exc) {
				throw new InvalidAddressException(exc.toString());
			}
	}
	
	////////////////////////////////////////////////////////////////////////////////
    // getAttachedStack()
    ////////////////////////////////////////////////////////////////////////////////
			
	public JainIsupStack getAttachedStack(){

		 return ownStack;		

	}

	////////////////////////////////////////////////////////////////////////////////
    // removeIsupListener()
    ////////////////////////////////////////////////////////////////////////////////

	public void removeIsupListener(JainIsupListener isupListener,
                               IsupUserAddress isupAddress)
                        throws ListenerNotRegisteredException,
                               InvalidListenerException,
                               InvalidAddressException{

		int rx;
		Integer idx=null;
		VectorAddress vaddr;		

		JainIsupListener listener = (JainIsupListener)listeners.get(isupAddress);
		if(listener == null){
				throw new ListenerNotRegisteredException("Listener is not registered");
		}
		Integer regIdx = (Integer)idxLookup.get(isupAddress);
		listeners.remove(isupAddress);

		rx = regIdx.intValue();
		
		if(isupDereg(rx) != SUCCESS){
			 throw new ListenerNotRegisteredException ("Native call tcapDereg failed");
		}
		else{
			idxLookup.remove(isupAddress);
			Iterator i = usrAddrLookup.iterator();

            while(i.hasNext()){
				vaddr    = (VectorAddress)i.next();
				idx      = new Integer(vaddr.getidx());
				if(regIdx == idx){
					usrAddrLookup.remove(vaddr);
					break;
                }
			}		
		}

	}
		
	////////////////////////////////////////////////////////////////////////////////
    // getStack()
    ////////////////////////////////////////////////////////////////////////////////			
		
	public JainIsupStack getStack() {
        return ownStack;
    }	
	
	////////////////////////////////////////////////////////////////////////////////
    // sendIsupEvent()
    ////////////////////////////////////////////////////////////////////////////////		

	public void sendIsupEvent(IsupEvent isupEvent)
                   throws MandatoryParameterNotSetException,
                          SendStackException,
                          ParameterRangeInvalidException,
                          VersionNotSupportedException{

		int cic=NONE, primitive=NONE, startCIC, endCIC, idpc,iopc;
		Integer idx=null;
		VectorAddress vaddr;
		SignalingPointCode vdpc;		

		if(listeners.isEmpty()){
				 throw new MandatoryParameterNotSetException("No listener registered yet");
		}
		
		cic = isupEvent.getCic();

		SignalingPointCode dpc = isupEvent.getDpc();

		Iterator e = usrAddrLookup.iterator();

		while(e.hasNext()){
			vaddr    = (VectorAddress)e.next();
			vdpc     = vaddr.getDPC();
			startCIC = vaddr.getstartCIC();
			endCIC   = vaddr.getendCIC();
						
			if(dpc.equals(vdpc) &&
				(cic <= endCIC) &&
				(cic >= startCIC)){
				
				idx = new Integer(vaddr.getidx());		
				break;
			}
		}
				
		int stackSpec              = getStack().getStackSpecification();
		SignalingPointCode opc     = isupEvent.getOpc();
		
		if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){				
			idpc = ((dpc.getMember() &0x07)<<11)|
					((dpc.getCluster()&0x00FF) << 3)| 
					(dpc.getZone()&0x07);
			iopc = ((opc.getMember() &0x07)<<11)|
					((opc.getCluster()&0x00FF) << 3)| 
					(opc.getZone()&0x07);
		}
		else{
			idpc = ((dpc.getMember() & 0x00FF)<<16)|
				((dpc.getCluster() & 0x00FF) << 8)|
				(dpc.getZone() & 0x00FF);
			
			iopc = ((opc.getMember() & 0x00FF)<<16)|
				((opc.getCluster() & 0x00FF) << 8)|
				(opc.getZone() & 0x00FF);
		}
		
		byte sls               = isupEvent.getSls();
		byte congpri           = isupEvent.getCongestionPriority();


		if(idx != null)	
		{
			try{
				if (isupEvent instanceof CircuitMaintenanceEvent)			
					primitive =  ((CircuitMaintenanceEvent)isupEvent).getIsupPrimitive();
			
				else if (isupEvent instanceof CircuitGrpMaintenanceEvent)
					primitive =  ((CircuitGrpMaintenanceEvent)isupEvent).getIsupPrimitive();
			
				else if (isupEvent instanceof SetupItuEvent)
					primitive =  ((SetupItuEvent)isupEvent).getIsupPrimitive();

				else if (isupEvent instanceof SetupAnsiEvent)
					primitive =  ((SetupAnsiEvent)isupEvent).getIsupPrimitive();

				else if (isupEvent instanceof AlertItuEvent)
					primitive = ((AlertItuEvent)isupEvent).getIsupPrimitive();

				else if (isupEvent instanceof AlertAnsiEvent)
					primitive = ((AlertAnsiEvent)isupEvent).getIsupPrimitive();

				else if(isupEvent instanceof AnswerItuEvent)
					primitive = ((AnswerItuEvent)isupEvent).getIsupPrimitive();

				else if(isupEvent instanceof AnswerAnsiEvent)
					primitive = ((AnswerAnsiEvent)isupEvent).getIsupPrimitive();

				else if(isupEvent instanceof CallProgressItuEvent )
					primitive = ((CallProgressItuEvent)isupEvent).getIsupPrimitive();

				else if(isupEvent instanceof CircuitReservationAnsiEvent)
					primitive = ((CircuitReservationAnsiEvent)isupEvent).getIsupPrimitive();

				else if(isupEvent instanceof CallProgressAnsiEvent )
					primitive = ((CallProgressAnsiEvent)isupEvent).getIsupPrimitive();

				else if(isupEvent instanceof CircuitValidationRespAnsiEvent )
					primitive = ((CircuitValidationRespAnsiEvent)isupEvent).getIsupPrimitive();
				
				else if(isupEvent instanceof  ConnectItuEvent)
					primitive = ((ConnectItuEvent)isupEvent).getIsupPrimitive();
				
				else if(isupEvent instanceof ExitAnsiEvent)
					primitive = ((ExitAnsiEvent)isupEvent).getIsupPrimitive();

				else if(isupEvent instanceof FacilityAcceptItuEvent)
					primitive = ((FacilityAcceptItuEvent)isupEvent).getIsupPrimitive();
				
				else if(isupEvent instanceof FacilityEvent)
					primitive = ((FacilityEvent)isupEvent).getIsupPrimitive();	

				else if(isupEvent instanceof FacilityItuEvent)
					primitive = ((FacilityItuEvent)isupEvent).getIsupPrimitive();	
		
				else if(isupEvent instanceof FacilityRejectItuEvent)
					primitive = ((FacilityRejectItuEvent)isupEvent).getIsupPrimitive();	
		
				else if(isupEvent instanceof FacilityRequestItuEvent)
					primitive = ((FacilityRequestItuEvent)isupEvent).getIsupPrimitive();	

				else if(isupEvent instanceof IdentificationReqItuEvent)
					primitive = ((IdentificationReqItuEvent)isupEvent).getIsupPrimitive();

				else if(isupEvent instanceof IdentificationRespItuEvent)
					primitive = ((IdentificationRespItuEvent)isupEvent).getIsupPrimitive();

				else if(isupEvent instanceof InformationItuEvent)
					primitive = ((InformationItuEvent)isupEvent).getIsupPrimitive();

				else if(isupEvent instanceof InformationAnsiEvent)
					primitive = ((InformationAnsiEvent)isupEvent).getIsupPrimitive();			

				else if(isupEvent instanceof InformationReqItuEvent)
					primitive = ((InformationReqItuEvent)isupEvent).getIsupPrimitive();

				else if(isupEvent instanceof InformationReqAnsiEvent)
					primitive = ((InformationReqAnsiEvent)isupEvent).getIsupPrimitive();

				else if(isupEvent instanceof NetworkResourceMgmtItuEvent)
					primitive = ((NetworkResourceMgmtItuEvent)isupEvent).getIsupPrimitive();
		
				else if(isupEvent instanceof ReleaseItuEvent)
					primitive = ((ReleaseItuEvent)isupEvent).getIsupPrimitive();

				else if(isupEvent instanceof ReleaseAnsiEvent)
					primitive = ((ReleaseAnsiEvent)isupEvent).getIsupPrimitive();
				
				else if(isupEvent instanceof ReleaseResponseItuEvent)
					primitive = ((ReleaseResponseItuEvent)isupEvent).getIsupPrimitive();

				else if(isupEvent instanceof ReleaseResponseEvent)
					primitive = ((ReleaseResponseEvent)isupEvent).getIsupPrimitive();

				else if(isupEvent instanceof SegmentationItuEvent)
					primitive = ((SegmentationItuEvent)isupEvent).getIsupPrimitive();

				else if(isupEvent instanceof SubsequentAddressItuEvent)
					primitive = ((SubsequentAddressItuEvent)isupEvent).getIsupPrimitive();

				else if(isupEvent instanceof UserPartTestInfoItuEvent)
					primitive = ((UserPartTestInfoItuEvent)isupEvent).getIsupPrimitive();

				else if(isupEvent instanceof UserToUserInfoItuEvent)
					primitive = ((UserToUserInfoItuEvent)isupEvent).getIsupPrimitive();

				else  if(isupEvent instanceof CircuitGrpQueryEvent)
					primitive = ((CircuitGrpQueryEvent)isupEvent).getIsupPrimitive();
			
				else  if(isupEvent instanceof CircuitGrpQueryRespEvent)
					primitive = ((CircuitGrpQueryRespEvent)isupEvent).getIsupPrimitive();

				else  if(isupEvent instanceof CircuitGrpResetEvent)
					primitive = ((CircuitGrpResetEvent)isupEvent).getIsupPrimitive();
			
				else  if(isupEvent instanceof CircuitGrpResetAnsiEvent)
					primitive =((CircuitGrpResetAnsiEvent)isupEvent).getIsupPrimitive();

				else  if(isupEvent instanceof CircuitGrpResetAckEvent)
					primitive = ((CircuitGrpResetAckEvent)isupEvent).getIsupPrimitive();

				else  if(isupEvent instanceof CircuitGrpResetAckAnsiEvent)
					primitive = ((CircuitGrpResetAckAnsiEvent)isupEvent).getIsupPrimitive();
				
				else  if(isupEvent instanceof ConfusionEvent)
					primitive = ((ConfusionEvent)isupEvent).getIsupPrimitive();

				else  if(isupEvent instanceof ContinuityEvent)
					primitive = ((ContinuityEvent)isupEvent).getIsupPrimitive();

				else  if(isupEvent instanceof ForwardTransferEvent)
					primitive = ((ForwardTransferEvent)isupEvent).getIsupPrimitive();

				else  if(isupEvent instanceof PassAlongEvent)
					primitive = ((PassAlongEvent)isupEvent).getIsupPrimitive();		

				else  if(isupEvent instanceof SuspendResumeEvent)
					primitive = ((SuspendResumeEvent)isupEvent).getIsupPrimitive();		
		
				else  if(isupEvent instanceof StopMaintReqEvent){
					primitive = ((StopMaintReqEvent)isupEvent).getIsupPrimitive();		
				}

				if(stackSpec == IsupConstants.ISUP_PV_ITU_1993)	
					sendItuIsupEvent (isupEvent, cic, iopc, idpc, sls, congpri,idx, primitive,stackSpec);	
				else	
					sendAnsiIsupEvent (isupEvent, cic, iopc, idpc,sls, congpri,idx, primitive,stackSpec);
			
			}catch (Exception exc) {};
		}
	}

	////////////////////////////////////////////////////////////////////////////////
    // sendItuIsupEvent()
    ////////////////////////////////////////////////////////////////////////////////	
	
	private void sendItuIsupEvent(IsupEvent event, int cic, int opc,
											int dpc, byte sls, byte congpri,
											Integer idx, int primitive,int stackSpec) 
												throws MandatoryParameterNotSetException,
													   ParameterNotSetException {

		
		NatureConnInd               nci           = null;
		FwdCallIndItu               fci           = null;
        CalledPartyNumberItu        cpn           = null;
        BwdCallIndItu               bci           = null;
		EventInformation            eventInf      = null;
		CauseInd                    csi           = null;
		InfoInd                     iind          = null;
		InfoReqInd                  irind         = null;
		RangeAndStatus              ras           = null;  
		byte[]                      subsequentNum = null;      
		byte[]                      uuin          = null;
		byte[]                      cti           = null;
		byte[]                      psa           = null;
		short                       facind        = NONE;
		int index = 1;
		int rc=NONE, i;
						
		byte fci_len, cpn_len, bci_len, csi_len,iind_len, irind_len, ras_len;

		byte[] par_array = ByteArray.getByteArray(256);	
		byte[] opt_par_array = null;
		
		try{	
				
			if(primitive == IsupConstants.ISUP_PRIMITIVE_SETUP ){
				
				event.checkMandatoryParameters();

				nci = ((SetupEvent)event).getNatureConnInd();
			
				par_array[index++] = IsupParameterValues.ISUP_PMT_NTCI;
				par_array[index++] = 1;
				par_array[index++] = FIXED_PARAMETER;
				par_array[index++] = nci.flatNatureConnInd();				
	
				fci = (FwdCallIndItu)((SetupEvent)event).getFwdCallInd();
				par_array[index++] = IsupParameterValues.ISUP_PMT_FOCI;		
				par_array[index++] = fci_len = (byte)fci.flatFwdCallIndItu().length;
				par_array[index++] = FIXED_PARAMETER;
				System.arraycopy(fci.flatFwdCallIndItu(),0,
							par_array,index,fci_len);
				index+= fci_len;						

				par_array[index++] = IsupParameterValues.ISUP_PMT_CGPC;
				par_array[index++] = 1;
				par_array[index++] = FIXED_PARAMETER;
				par_array[index++] = ((SetupEvent)event).getCallingPartyCat();

				par_array[index++] = IsupParameterValues.ISUP_PMT_XMR;
				par_array[index++] = 1;
				par_array[index++] = FIXED_PARAMETER;
				par_array[index++] = ((SetupItuEvent)event).getTransmissionMediumReq();
								
				cpn =(CalledPartyNumberItu)((SetupEvent)event).getCalledPartyNumber();	
				par_array[index++] = IsupParameterValues.ISUP_PMT_CDPN;
				par_array[index++] = cpn_len = (byte)cpn.flatCalledPartyNumberItu().length;
				par_array[index++] = VARIABLE_PARAMETER;
				System.arraycopy(cpn.flatCalledPartyNumberItu(),0,
							par_array,index,cpn_len);
				index+= cpn_len;

			}
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_ALERT){
				event.checkMandatoryParameters();

				bci                = (BwdCallIndItu)(((AlertEvent)event).getBwdCallInd());

				par_array[index++] = IsupParameterValues.ISUP_PMT_BACI;
				par_array[index++] = bci_len = (byte)bci.flatBwdCallIndItu().length;
				par_array[index++] = FIXED_PARAMETER;
				System.arraycopy(bci.flatBwdCallIndItu(),0,
							par_array,index,bci_len);
				index+= bci_len;			
			}
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_CALL_PROGRESS){
				event.checkMandatoryParameters();

				eventInf           = ((CallProgressEvent)event).getEventInformation();

				par_array[index++] = IsupParameterValues.ISUP_PMT_EVI;
				par_array[index++] = 1;
				par_array[index++] = FIXED_PARAMETER;
				par_array[index++] = eventInf.flatEventInformation();											
			}
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_CONNECT){
				event.checkMandatoryParameters();

				bci                = (BwdCallIndItu)(((ConnectItuEvent)event).getBwdCallInd());

				par_array[index++] = IsupParameterValues.ISUP_PMT_BACI;
				par_array[index++] = bci_len = (byte)bci.flatBwdCallIndItu().length;
				par_array[index++] = FIXED_PARAMETER;
				System.arraycopy(bci.flatBwdCallIndItu(),0,
							par_array,index,bci_len);
				index+= bci_len;			
			}
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_FACILITY_ACCEPT){
				
				facind                   = ((FacilityAcceptItuEvent)event).getFacilityInd();
				par_array[index++]       = IsupParameterValues.ISUP_PMT_FCI;
				par_array[index++]       = 2;
				par_array[index++]		 = FIXED_PARAMETER;
				for(i=0;i<2;i++)
				{
					par_array[index++] = (byte)(facind >> (8*i) & 0x00FF);
				}								

			}	
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_FACILITY_REJECT){
				
				facind                   = ((FacilityRejectItuEvent)event).getFacilityInd();
				par_array[index++]       = IsupParameterValues.ISUP_PMT_FCI;
				par_array[index++]       = 2;
				par_array[index++]		 = FIXED_PARAMETER;
				for(i=0;i<2;i++)
				{
					par_array[index++] = (byte)(facind >> (8*i) & 0x00FF);
				}
				csi                = ((FacilityRejectItuEvent)event).getCauseInd();
				par_array[index++] = IsupParameterValues.ISUP_PMT_CSI;
				par_array[index++] = csi_len = (byte)csi.flatCauseInd().length;
				par_array[index++] = VARIABLE_PARAMETER;
				System.arraycopy(csi.flatCauseInd(),0,
						par_array,index,csi_len);
				index+=	csi_len;										

			}	
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_FACILITY_REQUEST){
				
				facind                   = ((FacilityRequestItuEvent)event).getFacilityInd();
				par_array[index++]       = IsupParameterValues.ISUP_PMT_FCI;
				par_array[index++]       = 2;
				par_array[index++]		 = FIXED_PARAMETER;
				for(i=0;i<2;i++)
				{
					par_array[index++] = (byte)(facind >> (8*i) & 0x00FF);
				}								

			}	
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_INFORMATION){

				iind               = ((InformationEvent)event).getInfoInd();

				par_array[index++] = IsupParameterValues.ISUP_PMT_INI;
				par_array[index++] = iind_len = (byte)iind.flatInfoInd().length;
				par_array[index++] = FIXED_PARAMETER;
				System.arraycopy(iind.flatInfoInd(),0,
							par_array,index,iind_len);
				index+= iind_len;			

			}

			else if(primitive == IsupConstants.ISUP_PRIMITIVE_INFORMATION_REQ){

				irind              = ((InformationReqEvent)event).getInfoReqInd();

				par_array[index++] = IsupParameterValues.ISUP_PMT_INRI;
				par_array[index++] = irind_len = (byte)irind.flatInfoReqInd().length;
				par_array[index++] = FIXED_PARAMETER;
				System.arraycopy(irind.flatInfoReqInd(),0,
							par_array,index,irind_len);
				index+= irind_len;			

			}
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_RELEASE){
				
				csi                = ((ReleaseEvent)event).getCauseInd();
				par_array[index++] = IsupParameterValues.ISUP_PMT_CSI;
				par_array[index++] = csi_len = (byte)csi.flatCauseInd().length;
				par_array[index++] = VARIABLE_PARAMETER;
				System.arraycopy(csi.flatCauseInd(),0,
						par_array,index,csi_len);
				index+=	csi_len;										

			}	
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_SUBSEQUENT_ADDRESS){
			
				subsequentNum      = ((SubsequentAddressItuEvent)event).getSubsequentNumber();
				par_array[index++] = IsupParameterValues.ISUP_PMT_SAN;
				par_array[index++] = (byte)subsequentNum.length;
				par_array[index++] = VARIABLE_PARAMETER;
				System.arraycopy(subsequentNum,0,
								par_array,index,subsequentNum.length);
				index+= subsequentNum.length;							

			}
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_USER_TO_USER_INFO){
				uuin                  = ((UserToUserInfoItuEvent)event).getUserToUserInformation();
				par_array[index++]    = IsupParameterValues.ISUP_PMT_UUIN; 
				par_array[index++]    = (byte)uuin.length;
				par_array[index++]	  = VARIABLE_PARAMETER;
				System.arraycopy(uuin,0,
								par_array,index,uuin.length);
				index+= uuin.length;			
			}
			else if((primitive == IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_QUERY) ||
					(primitive == IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_LOCAL_QUERY)){

				ras                   = ((CircuitGrpQueryEvent)event).getRangeAndStatus();
				par_array[index++]    = IsupParameterValues.ISUP_PMT_RAS; 
				par_array[index++]    = ras_len = (byte)ras.flatRangeAndStatus().length;
				par_array[index++]	  = VARIABLE_PARAMETER;
				System.arraycopy(ras.flatRangeAndStatus(),0,
						par_array,index,ras_len);
				index+=	ras_len;	

			}

			else if((primitive == IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_QUERY_RESP) ||
					(primitive == IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_LOCAL_QUERY_RESP)){

				ras                   = ((CircuitGrpQueryRespEvent)event).getRangeAndStatus();
				par_array[index++]    = IsupParameterValues.ISUP_PMT_RAS; 
				par_array[index++]    = ras_len = (byte)ras.flatRangeAndStatus().length;
				par_array[index++]	  = VARIABLE_PARAMETER;
				System.arraycopy(ras.flatRangeAndStatus(),0,
						par_array,index,ras_len);
				index+=	ras_len;	

				cti                   = ((CircuitGrpQueryRespEvent)event).getCircuitStateInd();
				par_array[index++]    = IsupParameterValues.ISUP_PMT_CTI; 
				par_array[index++]    = (byte)cti.length;
				par_array[index++]	  = VARIABLE_PARAMETER;
				System.arraycopy(cti,0,
						par_array,index,cti.length);
				index+=	cti.length;	
				

			}
			
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_RESET){

				ras                   = ((CircuitGrpResetEvent)event).getRangeAndStatus();
				par_array[index++]    = IsupParameterValues.ISUP_PMT_RAS; 
				par_array[index++]    = ras_len = (byte)ras.flatRangeAndStatus().length;
				par_array[index++]	  = VARIABLE_PARAMETER;
				System.arraycopy(ras.flatRangeAndStatus(),0,
						par_array,index,ras_len);
				index+=	ras_len;	

			}
		
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_RESET_ACK){

				ras                   = ((CircuitGrpResetAckEvent)event).getRangeAndStatus();
				par_array[index++]    = IsupParameterValues.ISUP_PMT_RAS; 
				par_array[index++]    = ras_len = (byte)ras.flatRangeAndStatus().length;
				par_array[index++]	  = VARIABLE_PARAMETER;
				System.arraycopy(ras.flatRangeAndStatus(),0,
						par_array,index,ras_len);
				index+=	ras_len;	

			}
		
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_CONFUSION){
				
				csi                = ((ConfusionEvent)event).getCauseInd();
				par_array[index++] = IsupParameterValues.ISUP_PMT_CSI;
				par_array[index++] = csi_len = (byte)csi.flatCauseInd().length;
				par_array[index++] = VARIABLE_PARAMETER;
				System.arraycopy(csi.flatCauseInd(),0,
						par_array,index,csi_len);
				index+=	csi_len;										

			}	

			else if(primitive == IsupConstants.ISUP_PRIMITIVE_CONTINUITY){							
				par_array[index++] = IsupParameterValues.ISUP_PMT_COTI;
				par_array[index++] = 1;
				par_array[index++] = FIXED_PARAMETER;
				par_array[index++] = ((ContinuityEvent)event).getContinuityInd();						

			}	

			else if(primitive == IsupConstants.ISUP_PRIMITIVE_PASS_ALONG){

				psa                = ((PassAlongEvent)event).getEmbeddedMessage();
				par_array[index++] = IsupParameterValues.ISUP_PMT_MSGINPAM;
				par_array[index++] = (byte)psa.length;
				par_array[index++] = FIXED_PARAMETER;
				System.arraycopy(psa,0,
						par_array,index,psa.length);	
				
			}
			else if((primitive == IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_BLOCKING) ||
					(primitive == IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_BLOCKING_ACK) ||
					(primitive == IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_UNBLOCKING) ||
					(primitive == IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_UNBLOCKING_ACK)){

				par_array[index++] = IsupParameterValues.ISUP_PMT_CGMI;
				par_array[index++] = 1;
				par_array[index++] = FIXED_PARAMETER;
				par_array[index++] = ((CircuitGrpMaintenanceEvent)event).getCircuitGrpSupervisionMsgTypeInd();	

				ras                   = ((CircuitGrpMaintenanceEvent)event).getRangeAndStatus();
				
				par_array[index++]    = IsupParameterValues.ISUP_PMT_RAS; 
				par_array[index++]    = ras_len = (byte)ras.flatRangeAndStatus().length;
				par_array[index++]	  = VARIABLE_PARAMETER;
				System.arraycopy(ras.flatRangeAndStatus(),0,
						par_array,index,ras_len);
				index+=	ras_len;	

			}
			else if((primitive == IsupConstants.ISUP_PRIMITIVE_SUSPEND) ||
					(primitive == IsupConstants.ISUP_PRIMITIVE_RESUME)){

				par_array[index++] = IsupParameterValues.ISUP_PMT_SRI;
				par_array[index++] = 1;
				par_array[index++] = FIXED_PARAMETER;
				par_array[index++] = ((SuspendResumeEvent)event).getSuspendResumeInd();	


			} 
			
			opt_par_array = PutItuParam.putParam(event);	

			int ind = opt_par_array[0] & 0xFF;

			System.arraycopy(opt_par_array,1,par_array,index,ind);
			
			index+= ind;

			par_array[0] = (byte)(index - 1); 

		    rc = putMsg(idx.intValue(), 
								cic,
								opc,
								dpc,
								sls,
								congpri, 
								par_array,
								primitive,
								stackSpec);

		}catch (Exception exc) {};
		
		ByteArray.returnByteArray(par_array);
		ByteArray.returnByteArray(opt_par_array);
		
		if (rc != SUCCESS) throw new MandatoryParameterNotSetException("native call fail: " + rc);

	}

	////////////////////////////////////////////////////////////////////////////////
    // sendAnsiIsupEvent()
    ////////////////////////////////////////////////////////////////////////////////

	private void sendAnsiIsupEvent(IsupEvent event, int cic, int opc,
											int dpc, byte sls, byte congpri,
											Integer idx, int primitive,int stackSpec) 
												throws MandatoryParameterNotSetException,
														ParameterNotSetException {

		
		BwdCallIndAnsi                   bci           = null;
		NatureConnInd                    nci           = null;
		FwdCallIndAnsi                   fci           = null;		
		CalledPartyNumber                cpn           = null;	
		CircuitGrpCharacteristicsIndAnsi cgci          = null;
		UserServiceInfo                  usi           = null;
		EventInformation                 eventInf      = null;
		CauseInd                         csi           = null;
		InfoIndAnsi                      iind          = null;
		InfoReqIndAnsi                   irind         = null;
		RangeAndStatus                   ras           = null;  
		byte[]                           subsequentNum = null;      
		byte[]                           uuin          = null;
		byte[]                           cti           = null;
		byte[]                           psa           = null;
		short                            facind        = NONE;
		short                            cvr           = NONE;
		int index = 1;
		int rc=NONE, i;
						
		byte[] par_array = ByteArray.getByteArray(256);	
		byte[] opt_par_array = null;

		byte bci_len,fci_len, cpn_len, usi_len, csi_len, iind_len, irind_len, ras_len;

		try{	
				
			if(primitive == IsupConstants.ISUP_PRIMITIVE_SETUP ){
				event.checkMandatoryParameters();

				nci = ((SetupEvent)event).getNatureConnInd();
			
				par_array[index++] = IsupParameterValues.ISUP_PMT_NTCI;
				par_array[index++] = 1;
				par_array[index++] = FIXED_PARAMETER;
				par_array[index++] = nci.flatNatureConnInd();

				fci = (FwdCallIndAnsi)((SetupEvent)event).getFwdCallInd();
				par_array[index++] = IsupParameterValues.ISUP_PMT_FOCI;
				par_array[index++] = fci_len = (byte)fci.flatFwdCallIndAnsi().length;
				par_array[index++] = FIXED_PARAMETER;
				System.arraycopy(fci.flatFwdCallIndAnsi(),0,
							par_array,index,fci_len);
				index+= fci_len;	

				par_array[index++] = IsupParameterValues.ISUP_PMT_CGPC;
				par_array[index++] = 1;
				par_array[index++] = FIXED_PARAMETER;
				par_array[index++] = ((SetupEvent)event).getCallingPartyCat();
				
				usi                = ((SetupAnsiEvent)event).getUserServiceInfo();
				par_array[index++] = IsupParameterValues.ISUP_PMT_USI;
				par_array[index++] = usi_len = (byte)usi.flatUserServiceInfo().length;
				par_array[index++] = VARIABLE_PARAMETER;
				System.arraycopy(usi.flatUserServiceInfo(),0,
								par_array,index,usi_len);
				index+= usi_len;								
		
				cpn =(CalledPartyNumber)((SetupEvent)event).getCalledPartyNumber();	
				par_array[index++] = IsupParameterValues.ISUP_PMT_CDPN;
				par_array[index++] = cpn_len = (byte)cpn.flatCalledPartyNumber().length;
				par_array[index++] = VARIABLE_PARAMETER;
				System.arraycopy(cpn.flatCalledPartyNumber(),0,
							par_array,index,cpn_len);
				index+= cpn_len; 

			}
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_ALERT){
				event.checkMandatoryParameters();

				bci                = (BwdCallIndAnsi)(((AlertEvent)event).getBwdCallInd());
				par_array[index++] = IsupParameterValues.ISUP_PMT_BACI;
				par_array[index++] = bci_len = (byte)bci.flatBwdCallIndAnsi().length;
				par_array[index++] = FIXED_PARAMETER;
				System.arraycopy(bci.flatBwdCallIndAnsi(),0,
							par_array,index,bci_len);
				index+= bci_len;		 	
		
			}
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_CALL_PROGRESS){
				event.checkMandatoryParameters();

				eventInf           = ((CallProgressEvent)event).getEventInformation();

				par_array[index++] = IsupParameterValues.ISUP_PMT_EVI;
				par_array[index++] = 1;
				par_array[index++] = FIXED_PARAMETER;
				par_array[index++] = eventInf.flatEventInformation();										
			}
		
			else if((primitive == IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_QUERY_RESP) ||
					(primitive == IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_LOCAL_QUERY_RESP)){

				ras                   = ((CircuitGrpQueryRespEvent)event).getRangeAndStatus();
				par_array[index++]    = IsupParameterValues.ISUP_PMT_RAS; 
				par_array[index++]    = (byte)ras.flatRangeAndStatus().length;
				par_array[index++]	  = VARIABLE_PARAMETER;
				System.arraycopy(ras.flatRangeAndStatus(),0,
						par_array,index,ras.flatRangeAndStatus().length);
				index+=	ras.flatRangeAndStatus().length;	

				cti                   = ((CircuitGrpQueryRespEvent)event).getCircuitStateInd();
				par_array[index++]    = IsupParameterValues.ISUP_PMT_CTI; 
				par_array[index++]    = (byte)cti.length;
				par_array[index++]	  = VARIABLE_PARAMETER;
				System.arraycopy(cti,0,
						par_array,index,cti.length);
				index+=	cti.length;	
				

			}
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_CIRCUIT_RESERVATION){

				event.checkMandatoryParameters();
				nci = ((CircuitReservationAnsiEvent)event).getNatureConnInd();
			
				par_array[index++] = IsupParameterValues.ISUP_PMT_NTCI;
				par_array[index++] = 1;
				par_array[index++] = FIXED_PARAMETER;
				par_array[index++] = nci.flatNatureConnInd();

			}
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_CIRCUIT_VALIDATION_RESP){
				event.checkMandatoryParameters();

				cvr                      = ((CircuitValidationRespAnsiEvent)event).getCircuitValidationResp();
				par_array[index++]       = IsupParameterValues.ISUP_PMT_CVR;
				par_array[index++]       = 2;
				par_array[index++]		 = FIXED_PARAMETER;
				for(i=0;i<2;i++)
				{
					par_array[index++] = (byte)(cvr >> (8*i) & 0x00FF);
				} 							
				
				cgci                     = (((CircuitValidationRespAnsiEvent)event).getCircuitGrpCharacteristicsInd());

				par_array[index++] = IsupParameterValues.ISUP_PMT_CGCI;
				par_array[index++] = 1;
				par_array[index++] = FIXED_PARAMETER;
				par_array[index++] = cgci.flatCircuitGrpCharacteristicsIndAnsi();
				               
			
			}		
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_CONFUSION){
				
				csi                = ((ConfusionEvent)event).getCauseInd();
				par_array[index++] = IsupParameterValues.ISUP_PMT_CSI;
				par_array[index++] = (byte)csi.flatCauseInd().length;
				par_array[index++] = VARIABLE_PARAMETER;
				System.arraycopy(csi.flatCauseInd(),0,
						par_array,index,csi.flatCauseInd().length);
				index+=	csi.flatCauseInd().length;										

			}	
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_CONTINUITY){							
				par_array[index++] = IsupParameterValues.ISUP_PMT_COTI;
				par_array[index++] = 1;
				par_array[index++] = FIXED_PARAMETER;
				par_array[index++] = ((ContinuityEvent)event).getContinuityInd();						

			}	
				
				
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_INFORMATION){

				iind               = (InfoIndAnsi)((InformationEvent)event).getInfoInd();

				par_array[index++] = IsupParameterValues.ISUP_PMT_INI;
				par_array[index++] = iind_len = (byte)iind.flatInfoIndAnsi().length;
				par_array[index++] = FIXED_PARAMETER;
				System.arraycopy(iind.flatInfoIndAnsi(),0,
							par_array,index,iind_len);
				index+= iind_len;

			}

			else if(primitive == IsupConstants.ISUP_PRIMITIVE_INFORMATION_REQ){

				irind              = (InfoReqIndAnsi)((InformationReqEvent)event).getInfoReqInd();

				par_array[index++] = IsupParameterValues.ISUP_PMT_INRI;
				par_array[index++] = irind_len=(byte)irind.flatInfoReqIndAnsi().length;
				par_array[index++] = FIXED_PARAMETER;
				System.arraycopy(irind.flatInfoReqIndAnsi(),0,
							par_array,index,irind_len);
				index+= irind_len;			

			}
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_RELEASE){
				
				csi                = ((ReleaseEvent)event).getCauseInd();
				par_array[index++] = IsupParameterValues.ISUP_PMT_CSI;
				par_array[index++] = csi_len = (byte)csi.flatCauseInd().length;
				par_array[index++] = VARIABLE_PARAMETER;
				System.arraycopy(csi.flatCauseInd(),0,
						par_array,index,csi_len);
				index+=	csi_len;									

			}	
			else if((primitive == IsupConstants.ISUP_PRIMITIVE_SUSPEND) ||
					(primitive == IsupConstants.ISUP_PRIMITIVE_RESUME)){

				par_array[index++] = IsupParameterValues.ISUP_PMT_SRI;
				par_array[index++] = 1;
				par_array[index++] = FIXED_PARAMETER;
				par_array[index++] = ((SuspendResumeEvent)event).getSuspendResumeInd();	


			} 
			else if((primitive == IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_BLOCKING) ||
					(primitive == IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_BLOCKING_ACK) ||
					(primitive == IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_UNBLOCKING) ||
					(primitive == IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_UNBLOCKING_ACK)){

				par_array[index++] = IsupParameterValues.ISUP_PMT_CGMI;
				par_array[index++] = 1;
				par_array[index++] = FIXED_PARAMETER;
				par_array[index++] = ((CircuitGrpMaintenanceEvent)event).getCircuitGrpSupervisionMsgTypeInd();	

				ras                   = ((CircuitGrpMaintenanceEvent)event).getRangeAndStatus();
				
				par_array[index++]    = IsupParameterValues.ISUP_PMT_RAS; 
				par_array[index++]    = (byte)ras.flatRangeAndStatus().length;
				par_array[index++]	  = VARIABLE_PARAMETER;
				System.arraycopy(ras.flatRangeAndStatus(),0,
						par_array,index,ras.flatRangeAndStatus().length);
				index+=	ras.flatRangeAndStatus().length;

			}
			
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_RESET){

				ras                   = ((CircuitGrpResetEvent)event).getRangeAndStatus();
				par_array[index++]    = IsupParameterValues.ISUP_PMT_RAS; 
				par_array[index++]    = (byte)ras.flatRangeAndStatus().length;
				par_array[index++]	  = VARIABLE_PARAMETER;
				System.arraycopy(ras.flatRangeAndStatus(),0,
						par_array,index,ras.flatRangeAndStatus().length);
				index+=	ras.flatRangeAndStatus().length;	

			}
				
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_RESET_ACK){

				ras                   = ((CircuitGrpResetAckEvent)event).getRangeAndStatus();
				par_array[index++]    = IsupParameterValues.ISUP_PMT_RAS; 
				par_array[index++]    = (byte)ras.flatRangeAndStatus().length;
				par_array[index++]	  = VARIABLE_PARAMETER;
				System.arraycopy(ras.flatRangeAndStatus(),0,
						par_array,index,ras.flatRangeAndStatus().length);
				index+=	ras.flatRangeAndStatus().length;	

			}	
			else if((primitive == IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_QUERY) ||
					(primitive == IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_LOCAL_QUERY)){

				ras                   = ((CircuitGrpQueryEvent)event).getRangeAndStatus();
				par_array[index++]    = IsupParameterValues.ISUP_PMT_RAS; 
				par_array[index++]    = (byte)ras.flatRangeAndStatus().length;
				par_array[index++]	  = VARIABLE_PARAMETER;
				System.arraycopy(ras.flatRangeAndStatus(),0,
						par_array,index,ras.flatRangeAndStatus().length);
				index+=	ras.flatRangeAndStatus().length;	

			}	
										
			else if(primitive == IsupConstants.ISUP_PRIMITIVE_PASS_ALONG){

				psa                = ((PassAlongEvent)event).getEmbeddedMessage();
				par_array[index++] = IsupParameterValues.ISUP_PMT_MSGINPAM;
				par_array[index++] = (byte)psa.length;
				par_array[index++] = FIXED_PARAMETER;
				System.arraycopy(psa,0,
						par_array,index,psa.length);	
					
			}
							
			opt_par_array = PutAnsiParam.putParam(event);

			int ind = opt_par_array[0] & 0xFF;
	
			System.arraycopy(opt_par_array,1,par_array,index,ind);

			index+= ind;

			par_array[0] = (byte)(index - 1); 
		
		    rc = putMsg(idx.intValue(), 
								cic,
								opc,
								dpc,
								sls,
								congpri, 
								par_array,
								primitive,
								stackSpec);

		}catch (Exception exc) {};
		
		ByteArray.returnByteArray(par_array);
		ByteArray.returnByteArray(opt_par_array);
		
		if (rc != SUCCESS) throw new MandatoryParameterNotSetException("native call fail: " + rc);

	}

	////////////////////////////////////////////////////////////////////////////////
    // getIsupEvent()
    ////////////////////////////////////////////////////////////////////////////////

	private int getIsupEvent(byte[] par_array,int CIC,int iopc, int idpc, byte sls, byte congprio,IsupUserAddress listAddress) {

		int i,rc = FAIL, omember, ocluster, ozone, dmember, dcluster, dzone;
		int msg_type, prim_type;
		int index = 1;
		byte par_code, par_len;
		IsupEvent event                       = null;
		byte[] tmp_array                      = null;
		CircuitGrpCharacteristicsIndAnsi cgci = new CircuitGrpCharacteristicsIndAnsi();
		RangeAndStatus rns                    = new RangeAndStatus();
		BwdCallIndItu  bci                    = new BwdCallIndItu();
		BwdCallIndAnsi bcia                   = new BwdCallIndAnsi();
		CauseInd csi                          = new CauseInd();	
		NatureConnInd ncind                   = new NatureConnInd();
		short fac                             = 0;		
		short cvri                            = 0;

		int stackSpec				          = getStack().getStackSpecification();

				
		if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){				
			ozone    = iopc & 0x07;
			ocluster = (iopc >> 3) & 0xFF;
			omember  = (iopc >> 11) & 0x07;				
			dzone    = idpc  & 0x07;
			dcluster = (idpc >> 3) & 0xFF;
			dmember  = (idpc >> 11) & 0x07;	

		}
		else{ 
			ozone    = iopc & 0xFF;
			ocluster = (iopc >> 8) & 0xFF;
			omember  = (iopc >> 16) & 0xFF;				
			dzone    = idpc  & 0xFF;
			dcluster = (idpc >> 8) & 0xFF;
			dmember  = (idpc >> 16) & 0xFF;						
						
		}
						
		SignalingPointCode opc = new SignalingPointCode(omember,ocluster,ozone);
		SignalingPointCode dpc = new SignalingPointCode(dmember,dcluster,dzone);

		try{
						
			JainIsupListener listener = (JainIsupListener)listeners.get(listAddress);
		
			if(listener != null){
							
				msg_type  = par_array[index++];
				prim_type =  par_array[index++];			
		
				switch(msg_type){

					case IsupMsgValues.ISUP_MSG_ACM:
					
						par_code = par_array[index++];
						par_len  = par_array[index++];											
					
						if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){
							bci.putBwdCallIndItu(par_array,index,par_len);									
							event = new AlertItuEvent(this,dpc,opc,sls,CIC,congprio,bci);
						}
						else{
							bcia.putBwdCallIndAnsi(par_array,index,par_len);								
							event = new AlertAnsiEvent(this,dpc,opc,sls,CIC,congprio,bcia);
						}
						index+=par_len;

						break;


					case IsupMsgValues.ISUP_MSG_ANM:
						
						if(stackSpec == IsupConstants.ISUP_PV_ITU_1993)					
							event = new AnswerItuEvent(this,dpc,opc,sls,CIC,congprio);
						else
							event = new AnswerAnsiEvent(this,dpc,opc,sls,CIC,congprio);
						break;
				
					case IsupMsgValues.ISUP_MSG_BLO:
					case IsupMsgValues.ISUP_MSG_BLA:
					case IsupMsgValues.ISUP_MSG_CCR:
					case IsupMsgValues.ISUP_MSG_CRA:
					case IsupMsgValues.ISUP_MSG_CVT:
					case IsupMsgValues.ISUP_MSG_LBA:
					case IsupMsgValues.ISUP_MSG_OVL:
					case IsupMsgValues.ISUP_MSG_RSC:
					case IsupMsgValues.ISUP_MSG_UBA:						
					case IsupMsgValues.ISUP_MSG_UBL:							
					case IsupMsgValues.ISUP_MSG_UCIC:
						
						if(prim_type == IsupConstants.ISUP_PRIMITIVE_START_RESET_IND)
							event = new StartResetIndEvent(this,dpc,opc,sls,CIC,congprio);						
						else if(prim_type == IsupConstants.ISUP_PRIMITIVE_CALL_FAILURE_IND)
							event = new CallFailureIndEvent(this,dpc,opc,sls,CIC,congprio,0);
	
						else 
							event = new CircuitMaintenanceEvent(this,dpc,opc,sls,CIC,congprio,prim_type);
						break;

					case IsupMsgValues.	ISUP_MSG_CPG:
												
						par_code = par_array[index++];
						par_len  = par_array[index++];											

						EventInformation  eventinfo = new EventInformation();
					
						eventinfo.putEventInformation(par_array,index,par_len);
				
						index+=par_len;
					
						if(stackSpec == IsupConstants.ISUP_PV_ITU_1993)										
							event = new CallProgressItuEvent(this,dpc,opc,sls,CIC,congprio,eventinfo);
						else 						
							event = new CallProgressAnsiEvent(this,dpc,opc,sls,CIC,congprio,eventinfo);
						break;

					case IsupMsgValues.ISUP_MSG_CGB:
					case IsupMsgValues.ISUP_MSG_CGU:
					case IsupMsgValues.ISUP_MSG_CGBA:
					case IsupMsgValues.ISUP_MSG_CGUA:
												
						par_code = par_array[index++]; /*Circuit GroupSupervision msg type Ind*/
						par_len  = par_array[index++];		
						
						byte cgsm = par_array[index];
						index+=par_len;		
		
						par_code = par_array[index++]; /* Range and status */
						par_len  = par_array[index++];

						rns.putRangeAndStatus(par_array,index,par_len);										
						index+=par_len;										
						
						if(prim_type == IsupConstants.ISUP_PRIMITIVE_MTP_PAUSE){
							event = new NwStatusIndEvent(this,dpc,opc, sls,CIC,congprio,
										prim_type,opc); /*last dpc needs to be spc - 
										affected point code - D7 does not pass it */
						}
						else if(prim_type == IsupConstants.ISUP_PRIMITIVE_MTP_RESUME){
							event = new NwStatusIndEvent(this,dpc,opc,sls,CIC,congprio,
										prim_type,opc); /*last dpc needs to be spc - 
										affected point code - D7 does not pass it */
						}	
						else if(prim_type == IsupConstants.ISUP_PRIMITIVE_MTP_STATUS_USER_PART){
							event = new NwStatusIndEvent(this,dpc,opc, sls,CIC,congprio,
										prim_type,opc); /*last dpc needs to be spc - 
										affected point code - D7 doesnot pass it */

							if(msg_type == IsupMsgValues.ISUP_MSG_CGB){
								
								((NwStatusIndEvent)event).setUserPartStatus(
												NwStatusIndEvent.ISUP_UP_USER_INACCESSIBLE);
								/* UNEQUIPPED also can be set here-
								don't know how to distinguish */
							}
								
						}					
						else{
							event = new CircuitGrpMaintenanceEvent(this,dpc,opc,sls,CIC,congprio,
										prim_type,rns,cgsm);
						}
						break;					
					
					case IsupMsgValues.ISUP_MSG_CON:
					
						par_code = par_array[index++]; /*BwdCallInd*/
						par_len  = par_array[index++];											
											
						bci.putBwdCallIndItu(par_array,index,par_len);
				
						index+=par_len;
																					
						event = new ConnectItuEvent(this,dpc,opc,sls,CIC,congprio,bci);
						break;

					case IsupMsgValues.ISUP_MSG_CRM: //ANSI only
												
						par_code = par_array[index++]; /*Nature of Connection ind*/
						par_len  = par_array[index++];		
						
						ncind.putNatureConnInd(par_array,index,par_len);
				
						index+=par_len;

						event = new CircuitReservationAnsiEvent(this,dpc,opc,sls,CIC,congprio,ncind);
						break;

					case IsupMsgValues.ISUP_MSG_CQM:
					
						par_code = par_array[index++]; /* Range and status */
						par_len  = par_array[index++];											
												
						rns.putRangeAndStatus(par_array,index,par_len);										
						index+=par_len;	
						
						event = new CircuitGrpQueryEvent(this,dpc,opc,sls,CIC,congprio,prim_type,rns); 
						break;

					case IsupMsgValues.ISUP_MSG_CQR:
						
						par_code = par_array[index++]; /* Range and status */
						par_len  = par_array[index++];											
						
						
						rns.putRangeAndStatus(par_array,index,par_len);										
						index+=par_len;										
						
						par_code = par_array[index++]; /*Circuit StateInd*/
						par_len  = par_array[index++];			

						tmp_array = ByteArray.getByteArray(par_len);
						System.arraycopy(par_array,index, tmp_array,0,par_len);
						index+=par_len;			
								
						event = new CircuitGrpQueryRespEvent(this,dpc,opc,sls,CIC,congprio,prim_type, 
																rns,tmp_array);		
						ByteArray.returnByteArray(tmp_array);	
						break;

					case IsupMsgValues.ISUP_MSG_CVR:
						
						par_code = par_array[index++]; /* Circuit Validation Response Ind */
						par_len  = par_array[index++];											
						
						cvri = 0;
						for(i=0;i<2;i++)
							cvri = (short)(cvri | ((par_array[index+i] >> (8*i))& (0xFF >> (i*8))));		

						index+=par_len;						

						par_code = par_array[index++]; /* CircuitGroup Characteristics Ind */
						par_len  = par_array[index++];											
						
						
						cgci.putCircuitGrpCharacteristicsIndAnsi(par_array,index,par_len);							
						index+=par_len;					

						event = new CircuitValidationRespAnsiEvent(this,dpc,opc,sls,CIC,congprio,cvri,cgci); 		
						break;

					case IsupMsgValues.ISUP_MSG_GRA:
						
						par_code = par_array[index++]; /* Range and status */
						par_len  = par_array[index++];											
												
						rns.putRangeAndStatus(par_array,index,par_len);										
						index+=par_len;	
						
						if(stackSpec == IsupConstants.ISUP_PV_ITU_1993)
							event = new CircuitGrpResetAckEvent(this,dpc,opc, sls,CIC,congprio, rns);
						else
							event = new CircuitGrpResetAckAnsiEvent(this,dpc,opc, sls,CIC,congprio, rns);	
						break;					

					case IsupMsgValues.ISUP_MSG_GRS:
						
						par_code = par_array[index++]; /* Range and status */
						par_len  = par_array[index++];											
												
						rns.putRangeAndStatus(par_array,index,par_len);										
						index+=par_len;	
						
						if(stackSpec == IsupConstants.ISUP_PV_ITU_1993)
							event = new CircuitGrpResetEvent(this,dpc,opc,sls,CIC,congprio,rns);
						else
							event = new CircuitGrpResetAnsiEvent(this,dpc,opc,sls,CIC,congprio,rns);	
						break;

					case IsupMsgValues.ISUP_MSG_CFN:
						
						par_code = par_array[index++]; /* Cause indicators */
						par_len  = par_array[index++];				

								
						csi.putCauseInd(par_array,index,par_len);
						index+=par_len;			

						event = new ConfusionEvent(this,dpc,opc,sls,CIC,congprio, csi); 

						break;

					case IsupMsgValues.ISUP_MSG_COT:
						
						par_code = par_array[index++]; /* Continuity indicators */
						par_len  = par_array[index++];	

						byte cot = par_array[index];
						index+=par_len;		
						
						event = new ContinuityEvent(this,dpc,opc,sls,CIC,congprio,cot); 

						break;


					case IsupMsgValues.ISUP_MSG_EXM:
						
						event = new ExitAnsiEvent(this,dpc,opc,sls,CIC,congprio);
						break;


					case IsupMsgValues.ISUP_MSG_FAC:
						
						if(stackSpec == IsupConstants.ISUP_PV_ITU_1993)		
							event = new FacilityItuEvent(this,dpc,opc,sls,CIC,congprio);

						else 
							event = new FacilityEvent(this,dpc,opc,sls,CIC,congprio);
						break;

					case IsupMsgValues.ISUP_MSG_FCA:
						
						par_code = par_array[index++]; /* Facility  indicators */
						par_len  = par_array[index++];		

						fac = 0;
						for(i=0;i<2;i++)
							fac = (short)(fac | ((par_array[index+i] >> (8*i))& (0xFF >> (i*8))));		

						index+=par_len;
				
						event = new FacilityAcceptItuEvent(this,dpc,opc,(byte) 0,CIC,(byte) 0,fac);
						break;

					case IsupMsgValues.ISUP_MSG_FCJ:
						
						par_code = par_array[index++]; /* Facility  indicators */
						par_len  = par_array[index++];		

						fac = 0;
						for(i=0;i<2;i++)
							fac = (short)(fac | ((par_array[index+i] >> (8*i))& (0xFF >> (i*8))));		

						index+=par_len;		

						par_code = par_array[index++]; /* Cause indicators */
						par_len  = par_array[index++];				
	
						csi.putCauseInd(par_array,index,par_len);
						index+=par_len;			

						event = new FacilityRejectItuEvent(this,dpc,opc,(byte) 0,CIC,(byte) 0,fac,csi);
						break;

					case IsupMsgValues.ISUP_MSG_FCR:
						
						par_code = par_array[index++]; /* Facility  indicators */
						par_len  = par_array[index++];		

						fac = 0;
						for(i=0;i<2;i++)
							fac = (short)(fac | ((par_array[index+i] >> (8*i))& (0xFF >> (i*8))));		

						index+=par_len;
				
						event = new FacilityRequestItuEvent(this,dpc,opc,(byte) 0,CIC,(byte) 0,fac);
						break;

					case IsupMsgValues.ISUP_MSG_FOT:
						
						event = new ForwardTransferEvent(this,dpc,opc,sls,CIC,congprio);
						break;
				
					case IsupMsgValues.ISUP_MSG_IAM:

						par_code = par_array[index++]; /*Nature of Connection */
						par_len  = par_array[index++];			
												
						NatureConnInd nci = new NatureConnInd();
						nci.putNatureConnInd(par_array,index,par_len);				
						index+=par_len;									
						
						par_code = par_array[index++]; /*Forward Call Ind */
						par_len  = par_array[index++];			
						
						if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){	
							FwdCallIndItu fci = new FwdCallIndItu();
							fci.putFwdCallIndItu(par_array,index,par_len);
						
							index+=par_len;		

							par_code = par_array[index++]; /*Calling Party Cat */
							par_len  = par_array[index++];			
							byte cat = par_array[index];
							index+=par_len;		

							par_code = par_array[index++]; /*Transmission Medium Req */
							par_len  = par_array[index++];	
							byte tmr = par_array[index];
							index+=par_len;

							par_code = par_array[index++]; /*Called Party Number */
							par_len  = par_array[index++];	
							CalledPartyNumberItu cpn = new CalledPartyNumberItu();
							cpn.putCalledPartyNumberItu(par_array,index,par_len);	
							index+=par_len;

							event = new SetupItuEvent(this,dpc,opc,sls,CIC,congprio,false,nci,fci,cpn,cat,tmr);
		
						}
						else{
							FwdCallIndAnsi fci = new FwdCallIndAnsi();
							fci.putFwdCallIndAnsi(par_array,index,par_len);
						
							index+=par_len;

							par_code = par_array[index++]; /*Calling Party Cat */
							par_len  = par_array[index++];			
							byte cat = par_array[index];
							index+=par_len;

							par_code = par_array[index++]; /*User Service Information */
							par_len  = par_array[index++];	
							UserServiceInfo  usi= new UserServiceInfo();
							usi.putUserServiceInfo(par_array,index,par_len);	
							index+=par_len;
						
							par_code = par_array[index++]; /*Called Party Number */
							par_len  = par_array[index++];	
							CalledPartyNumber cpn = new CalledPartyNumber();
							cpn.putCalledPartyNumber(par_array,index,par_len);	
							index+=par_len;

							event = new SetupAnsiEvent(this,dpc,opc,sls,CIC,congprio,false,nci,fci,cpn,cat,usi);

						}
						break;
						
					case IsupMsgValues.ISUP_MSG_IDR:
						
						event = new IdentificationReqItuEvent(this,dpc,opc,sls,CIC,congprio);		
						break;

					case IsupMsgValues.ISUP_MSG_INF:
						
						par_code = par_array[index++]; /* Information Indicators */
						par_len  = par_array[index++];											
							
						if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){
							InfoInd inf = new InfoInd();					
							inf.putInfoInd(par_array,index,par_len);										
							index+=par_len;	
						
							event = new InformationItuEvent(this,dpc,opc, sls,CIC,congprio, inf);
						}
						else{
							InfoIndAnsi inf = new InfoIndAnsi();					
							inf.putInfoIndAnsi(par_array,index,par_len);										
							index+=par_len;	
						
							event = new InformationAnsiEvent(this,dpc,opc, sls,CIC,congprio, inf);
						}
						break;

					case IsupMsgValues.ISUP_MSG_INR:
						
						par_code = par_array[index++]; /* Information Req Indicators */
						par_len  = par_array[index++];											
							
						if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){
							InfoReqInd inr = new InfoReqInd();					
							inr.putInfoReqInd(par_array,index,par_len);										
							index+=par_len;	
						
							event = new InformationReqItuEvent(this,dpc,opc,sls,CIC,congprio, inr);
						}
						else{
							InfoReqIndAnsi inr = new InfoReqIndAnsi();					
							inr.putInfoReqIndAnsi(par_array,index,par_len);										
							index+=par_len;	
						
							event = new InformationReqAnsiEvent(this,dpc,opc,sls,CIC,congprio, inr);
						}
						break;
				
					case IsupMsgValues.ISUP_MSG_IRS:
						
						event = new IdentificationRespItuEvent(this,dpc,opc,sls,CIC,congprio);
						break;					

					case IsupMsgValues.ISUP_MSG_NRM:
						
						event = new NetworkResourceMgmtItuEvent(this,dpc,opc,sls,CIC,congprio);

						break;					
					
					case IsupMsgValues.ISUP_MSG_PSA:
						
						par_code = par_array[index++]; 
						par_len  = par_array[index++];			
				
						tmp_array = ByteArray.getByteArray(par_array.length-index);
						System.arraycopy(par_array,index, tmp_array,0,(par_array.length-index));
						index = par_array.length;			
								
						event = new PassAlongEvent(this,dpc,opc,sls,CIC,congprio);
		
						if(tmp_array.length != 0)
								((PassAlongEvent)event).setEmbeddedMessage(tmp_array);		
		
						ByteArray.returnByteArray(tmp_array);						

						break;

					case IsupMsgValues.ISUP_MSG_REL:
						
						par_code = par_array[index++]; /* Cause indicators */
						par_len  = par_array[index++];				
	
						csi.putCauseInd(par_array,index,par_len);
						index+=par_len;
						
						if(prim_type == IsupConstants.ISUP_PRIMITIVE_CALL_FAILURE_IND){
							
							event = new CallFailureIndEvent(this,dpc,opc,sls,CIC,congprio,(int)csi.getCauseValue());
						}
						else if(prim_type ==IsupConstants.ISUP_PRIMITIVE_REATTEMPT_IND){
							
							event = new ReattemptIndEvent(this,dpc,opc,sls,CIC,congprio,(int)csi.getCauseValue());
						}
						else{
							if(stackSpec == IsupConstants.ISUP_PV_ITU_1993){	
								event = new ReleaseItuEvent(this,dpc,opc,sls,CIC,congprio, csi);
							}
							else{
								event = new ReleaseAnsiEvent(this,dpc,opc,sls,CIC,congprio, csi);
							}
						}			
						break;

					case IsupMsgValues.ISUP_MSG_RLC:
						
						if(stackSpec == IsupConstants.ISUP_PV_ITU_1993)
							event = new ReleaseResponseItuEvent(this,dpc,opc,sls,CIC,congprio);
						else
							event = new ReleaseResponseEvent(this,dpc,opc,sls,CIC,congprio);
						break;

					

					case IsupMsgValues.ISUP_MSG_SAM:
						
						par_code = par_array[index++]; /*Subsequent Number*/
						par_len  = par_array[index++];			

						tmp_array = ByteArray.getByteArray(par_len);
						System.arraycopy(par_array,index, tmp_array,0,par_len);
						index+=par_len;			
								
						event = new SubsequentAddressItuEvent(this,dpc,opc,sls,CIC,congprio,tmp_array);
		
						ByteArray.returnByteArray(tmp_array);
						break;

					case IsupMsgValues.ISUP_MSG_SGM:
						
						event = new SegmentationItuEvent(this,dpc,opc,sls,CIC,congprio);					
						break;

					case IsupMsgValues.ISUP_MSG_SUS:
					case IsupMsgValues.ISUP_MSG_RES:
						
						par_code = par_array[index++];/*Suspend/Resume Ind*/
						par_len  = par_array[index++];		
						
						byte sus = par_array[index];
						index+=par_len;		
		
						event = new SuspendResumeEvent(this,dpc,opc,sls,CIC,congprio,prim_type,sus);
						break;

					

					case IsupMsgValues.ISUP_MSG_UPT:
					case IsupMsgValues.ISUP_MSG_UPA:
						
						
						event = new UserPartTestInfoItuEvent(this,dpc,opc,sls,CIC,congprio,prim_type);
						break;	

					
						
					case IsupMsgValues.ISUP_MSG_UUI:
						
						par_code = par_array[index++]; /*User to user info*/
						par_len  = par_array[index++];			

						tmp_array = ByteArray.getByteArray(par_len);
						System.arraycopy(par_array,index, tmp_array,0,par_len);
						index+=par_len;			
								
						event = new UserToUserInfoItuEvent(this,dpc,opc,sls,CIC,congprio,tmp_array);
		
						ByteArray.returnByteArray(tmp_array);
						break;
									
					case IsupMsgValues.ISUP_MSG_NULL:
						if(prim_type == IsupConstants.ISUP_PRIMITIVE_ISUP_ERROR_IND){
							

							event = new IsupErrorIndEvent(this,dpc,opc,sls,CIC,congprio,0,
										"Error-Invalid Message");
						}
						else if(prim_type == IsupConstants.ISUP_PRIMITIVE_MTP_RESUME){
							// SP_ACCESS denied is received 
							
						}
						else if(prim_type == IsupConstants.ISUP_PRIMITIVE_MTP_STATUS_CONGESTION){

							int conglvl = (par_array[index++] & 0x000000FF)|
								((par_array[index++]<<8)& 0x0000FF00)|
								((par_array[index++]<<16)& 0x00FF0000)|
								((par_array[index++]<<24)& 0xFF000000); 							
						    event = new NwStatusIndEvent(this,dpc,opc, sls,CIC,congprio,
										prim_type,opc); 
							((NwStatusIndEvent)event).setCongLevel(conglvl);				
							index += 4;					
						}
						else if(prim_type == IsupConstants.ISUP_PRIMITIVE_MAINT_TMO_IND){
							
							short timerid = (short)((par_array[index++] & 0x00FF)|
								((par_array[index++]<<8)& 0xFF00));

							event = new MaintenanceTimeoutIndEvent(this,dpc,opc,sls,CIC,congprio,(int)timerid);
							
						}
						else 
							System.out.println("Unknown message is received");
						break;						

					default:				
						System.out.println("Unknown message is received"); 	
						// Unknown Msg is received;
						break;
				}
				/* Process the rest of the parameters */
			
				if(	msg_type !=  IsupMsgValues.ISUP_MSG_PSA){	
					while(index < par_array.length){		
						par_code = 	par_array[index++];							
						par_len  = par_array[index++];	
						if(stackSpec == IsupConstants.ISUP_PV_ITU_1993)												
								processItuIsupParam(event,par_code,par_len,par_array,index);
						else
								processAnsiIsupParam(event,par_code,par_len,par_array,index);
						index+=par_len;								
					}
				}	
				
				listener.processIsupEvent(event);
		
			}
		
		
		}catch (Exception exc) {};	
		rc = SUCCESS;
		return rc;
	}

	////////////////////////////////////////////////////////////////////////////////
    // processItuIsupParam()
    ////////////////////////////////////////////////////////////////////////////////			
	private void processItuIsupParam(IsupEvent event, byte par_code,byte par_len, byte[]par_array, int index) 
		throws ParameterRangeInvalidException{
		
		int i = 0;
		byte[] tmp_array = null;

		try{
		switch(par_code){

			case IsupParameterValues.ISUP_PMT_BACI:
				BwdCallIndItu baci = new BwdCallIndItu();
				baci.putBwdCallIndItu(par_array,index,par_len);
				if(event instanceof AnswerEvent)
					((AnswerEvent)event).setBwdCallInd(baci);	
				else if(event instanceof CallProgressEvent)
					((CallProgressEvent)event).setBwdCallInd(baci);	
				break;

			case IsupParameterValues.ISUP_PMT_CSI:				
				CauseInd csi =  new CauseInd();					
				csi.putCauseInd(par_array,index,par_len);
				if(event instanceof AlertItuEvent)
					((AlertEvent)event).setCauseInd(csi);	
				else if(event instanceof CallProgressItuEvent)
					((CallProgressEvent)event).setCauseInd(csi);	
				else if(event instanceof ReleaseResponseItuEvent)
					((ReleaseResponseItuEvent)event).setCauseInd(csi);												
				break;

			case IsupParameterValues.ISUP_PMT_GNNO:
				NotificationInd nind = new NotificationInd();
				nind.putNotificationInd(par_array,index,par_len);
				if(event instanceof AlertItuEvent)
					((AlertEvent)event).setNotificationInd(nind);
				else if(event instanceof AnswerItuEvent)
					((AnswerEvent)event).setNotificationInd(nind);
				else if(event instanceof SetupItuEvent)
					((SetupItuEvent)event).setNotificationInd(nind);
				else if(event instanceof CallProgressItuEvent)
					((CallProgressItuEvent)event).setNotificationInd(nind);
				else if(event instanceof ConnectItuEvent)
					((ConnectItuEvent)event).setNotificationInd(nind);
				else if(event instanceof SegmentationItuEvent)
					((SegmentationItuEvent)event).setNotificationInd(nind);
				break;		
				
			case IsupParameterValues.ISUP_PMT_REDN:
				RedirectionNumberItu rdn = new RedirectionNumberItu();
				rdn.putRedirectionNumberItu(par_array,index,par_len);
				if(event instanceof AlertItuEvent)
					((AlertItuEvent)event).setRedirectionNumber(rdn);
				else if(event instanceof AnswerItuEvent)
					((AnswerItuEvent)event).setRedirectionNumber(rdn);
				else if(event instanceof CallProgressItuEvent)
					((CallProgressItuEvent)event).setRedirectionNumber(rdn);
				else if(event instanceof ConnectItuEvent)
					((ConnectItuEvent)event).setRedirectionNumber(rdn);
				else if(event instanceof ReleaseItuEvent)
					((ReleaseItuEvent)event).setRedirectionNumber(rdn);
				break;
		
			case IsupParameterValues.ISUP_PMT_RNR:
				if(event instanceof AlertItuEvent)
					((AlertItuEvent)event).setRedirectionNumberRest(par_array[index]);
				else if(event instanceof AnswerItuEvent)
					((AnswerItuEvent)event).setRedirectionNumberRest(par_array[index]);
				else if(event instanceof CallProgressItuEvent)
					((CallProgressItuEvent)event).setRedirectionNumberRest(par_array[index]);
				else if(event instanceof ConnectItuEvent)
					((ConnectItuEvent)event).setRedirectionNumberRest(par_array[index]);
				else if(event instanceof ReleaseItuEvent)
					((ReleaseItuEvent)event).setRedirectionNumberRest(par_array[index]);
				break;

			case IsupParameterValues.ISUP_PMT_CDVI:
				CallDiversionInfoItu cdv = new CallDiversionInfoItu();
				cdv.putCallDiversionInfoItu(par_array,index,par_len);
				if(event instanceof AlertItuEvent)
					((AlertItuEvent)event).setCallDiversionInfo(cdv);
				else if(event instanceof CallProgressItuEvent)
					((CallProgressItuEvent)event).setCallDiversionInfo(cdv);
				break;

			case IsupParameterValues.ISUP_PMT_OBI:
				OptionalBwdCallIndItu obc = new OptionalBwdCallIndItu();
				obc.putOptionalBwdCallIndItu(par_array, index, par_len);
				if(event instanceof AlertEvent)
					((AlertEvent)event).setOptionalBwdCallInd(obc);
				else if(event instanceof AnswerEvent)
					((AnswerEvent)event).setOptionalBwdCallInd(obc);
				else if(event instanceof CallProgressEvent)
					((CallProgressEvent)event).setOptionalBwdCallInd(obc);		
				else if(event instanceof ConnectItuEvent)
					((ConnectItuEvent)event).setOptionalBwdCallInd(obc);				
				break;
		
			case IsupParameterValues.ISUP_PMT_CRF:
				CallReference crf = new CallReference();
				crf.putCallReference(par_array,index,par_len,CallReference.PC_SIZE_14);
				if(event instanceof AlertEvent)
					((AlertEvent)event).setCallReference(crf);
				else if(event instanceof AnswerEvent)
					((AnswerEvent)event).setCallReference(crf);
				else if(event instanceof SetupEvent)
					((SetupEvent)event).setCallReference(crf);
				else if(event instanceof CallProgressEvent)
					((CallProgressEvent)event).setCallReference(crf);
				else if(event instanceof ConnectItuEvent)
					((ConnectItuEvent)event).setCallReference(crf);
				else if(event instanceof FacilityAcceptItuEvent)
					((FacilityAcceptItuEvent)event).setCallReference(crf);
				else if(event instanceof FacilityRequestItuEvent)
					((FacilityRequestItuEvent)event).setCallReference(crf);
				else if(event instanceof InformationEvent)
					((InformationEvent)event).setCallReference(crf); 
				else if(event instanceof InformationReqEvent)
					((InformationReqEvent)event).setCallReference(crf); 
				else if(event instanceof ForwardTransferEvent)
					((ForwardTransferEvent)event).setCallReference(crf);
				else if(event instanceof SuspendResumeEvent)
					((SuspendResumeEvent)event).setCallReference(crf); 
				break;

			case IsupParameterValues.ISUP_PMT_UUIC:
				UserToUserIndicatorsItu uuic = new UserToUserIndicatorsItu();
				uuic.putUserToUserIndicatorsItu(par_array,index,par_len);
				if(event instanceof AlertEvent)
					((AlertEvent)event).setUserToUserIndicators(uuic);
				else if(event instanceof AnswerEvent)
					((AnswerEvent)event).setUserToUserIndicators(uuic);
				else if(event instanceof SetupItuEvent)
					((SetupItuEvent)event).setUserToUserIndicators(uuic);
				else if(event instanceof CallProgressItuEvent)
					((CallProgressItuEvent)event).setUserToUserIndicators(uuic);
				else if(event instanceof ConnectItuEvent)
					((ConnectItuEvent)event).setUserToUserIndicators(uuic);
				else if(event instanceof FacilityRejectItuEvent)
					((FacilityRejectItuEvent)event).setUserToUserIndicators(uuic);
				else if(event instanceof FacilityAcceptItuEvent)
					((FacilityAcceptItuEvent)event).setUserToUserIndicators(uuic);
				else if(event instanceof FacilityRequestItuEvent)
					((FacilityRequestItuEvent)event).setUserToUserIndicators(uuic);
				else if(event instanceof ReleaseItuEvent)
					((ReleaseItuEvent)event).setUserToUserIndicators(uuic);
				break;

			case IsupParameterValues.ISUP_PMT_UUIN:
				
				tmp_array = ByteArray.getByteArray(par_len);
				System.arraycopy(par_array,index, tmp_array,0,par_len);
								
				if(event instanceof AlertEvent)
					((AlertEvent)event).setUserToUserInformation(tmp_array);
				else if(event instanceof AnswerEvent)
					((AnswerEvent)event).setUserToUserInformation(tmp_array);
				else if(event instanceof SetupEvent)
					((SetupEvent)event).setUserToUserInformation(tmp_array);
				else if(event instanceof CallProgressEvent)
					((CallProgressEvent)event).setUserToUserInformation(tmp_array);
				else if(event instanceof ConnectItuEvent)
					((ConnectItuEvent)event).setUserToUserInformation(tmp_array);
				else if(event instanceof ReleaseItuEvent)
					((ReleaseItuEvent)event).setUserToUserInformation(tmp_array);
				else if(event instanceof SegmentationItuEvent)
					((SegmentationItuEvent)event).setUserToUserInformation(tmp_array);

				ByteArray.returnByteArray(tmp_array);	
				break;

			case IsupParameterValues.ISUP_PMT_AXP:
								
				tmp_array = ByteArray.getByteArray(par_len);
				System.arraycopy(par_array,index, tmp_array,0,par_len);		
				
				if(event instanceof AlertEvent)
					((AlertEvent)event).setAccessTransport(tmp_array);
				else if(event instanceof AnswerEvent)
					((AnswerEvent)event).setAccessTransport(tmp_array);
				else if(event instanceof SetupEvent)
					((SetupEvent)event).setAccessTransport(tmp_array);
				else if(event instanceof CallProgressEvent)
					((CallProgressEvent)event).setAccessTransport(tmp_array);
				else if(event instanceof ConnectItuEvent)
					((ConnectItuEvent)event).setAccessTransport(tmp_array);
				else if(event instanceof ReleaseItuEvent)
					((ReleaseItuEvent)event).setAccessTransport(tmp_array);
				else if(event instanceof UserToUserInfoItuEvent)
					((UserToUserInfoItuEvent)event).setAccessTransport(tmp_array);
				else if(event instanceof IdentificationRespItuEvent)
					((IdentificationRespItuEvent)event).setAccessTransport(tmp_array);
				else if(event instanceof SegmentationItuEvent)
					((SegmentationItuEvent)event).setAccessTransport(tmp_array);

				ByteArray.returnByteArray(tmp_array);	
				break;

			case IsupParameterValues.ISUP_PMT_XMU:
				if(event instanceof AlertEvent)
					((AlertEvent)event).setTransmissionMediumUsed(par_array[index]);
				else if(event instanceof AnswerEvent)
					((AnswerEvent)event).setTransmissionMediumUsed(par_array[index]);
				else if(event instanceof CallProgressEvent)
					((CallProgressEvent)event).setTransmissionMediumUsed(par_array[index]);
				else if(event instanceof ConnectItuEvent)
					((ConnectItuEvent)event).setTransmissionMediumUsed(par_array[index]);
				else if(event instanceof NetworkResourceMgmtItuEvent)
					((NetworkResourceMgmtItuEvent)event).setTransmissionMediumUsed(par_array[index]);
				break;

			case IsupParameterValues.ISUP_PMT_ECI:
				EchoControlInfoItu eci = new EchoControlInfoItu();
				eci.putEchoControlInfoItu(par_array,index,par_len);
				if(event instanceof AlertItuEvent)
					((AlertItuEvent)event).setEchoControlInfo(eci);
				else if(event instanceof AnswerItuEvent)
					((AnswerItuEvent)event).setEchoControlInfo(eci);
				else if(event instanceof ConnectItuEvent)
					((ConnectItuEvent)event).setEchoControlInfo(eci);
				else if(event instanceof NetworkResourceMgmtItuEvent)
					((NetworkResourceMgmtItuEvent)event).setEchoControlInfo(eci);
				break;

			case IsupParameterValues.ISUP_PMT_ADI:
				if(event instanceof AlertItuEvent)
					((AlertItuEvent)event).setAccessDeliveryInfo(par_array[index]);
				else if(event instanceof AnswerItuEvent)
					((AnswerItuEvent)event).setAccessDeliveryInfo(par_array[index]);
				else if(event instanceof CallProgressItuEvent)
					((CallProgressItuEvent)event).setAccessDeliveryInfo(par_array[index]);
				else if(event instanceof ConnectItuEvent)
					((ConnectItuEvent)event).setAccessDeliveryInfo(par_array[index]);
				else if(event instanceof ReleaseItuEvent)
					((ReleaseItuEvent)event).setAccessDeliveryInfo(par_array[index]);
				break;

			case IsupParameterValues.ISUP_PMT_CGPC:
				if(event instanceof InformationEvent)
					((InformationEvent)event).setCallingPartyCat(par_array[index]);
				break;

			case IsupParameterValues.ISUP_PMT_PCI:
				ParamCompatibilityInfoItu pci = new ParamCompatibilityInfoItu();
				pci.putParamCompatibilityInfoItu(par_array, index, par_len);
				if(event instanceof AlertItuEvent)
					((AlertItuEvent)event).setParamCompatibilityInfo(pci);
				else if(event instanceof AnswerItuEvent)
					((AnswerItuEvent)event).setParamCompatibilityInfo(pci);
				else if(event instanceof SetupItuEvent)
					((SetupItuEvent)event).setParamCompatibilityInfo(pci);
				else if(event instanceof CallProgressItuEvent)
					((CallProgressItuEvent)event).setParamCompatibilityInfo(pci);
				else if(event instanceof ConnectItuEvent)
					((ConnectItuEvent)event).setParamCompatibilityInfo(pci);
				else if(event instanceof FacilityItuEvent)
					((FacilityItuEvent)event).setParamCompatibilityInfo(pci);
				else if(event instanceof FacilityAcceptItuEvent)
					((FacilityAcceptItuEvent)event).setParamCompatibilityInfo(pci);
				else if(event instanceof FacilityRequestItuEvent)
					((FacilityRequestItuEvent)event).setParamCompatibilityInfo(pci);
				else if(event instanceof InformationItuEvent)
					((InformationItuEvent)event).setParamCompatibilityInfo(pci);
				else if(event instanceof InformationReqItuEvent)
					((InformationReqItuEvent)event).setParamCompatibilityInfo(pci);
				else if(event instanceof ReleaseItuEvent)
					((ReleaseItuEvent)event).setParamCompatibilityInfo(pci);
				else if(event instanceof UserPartTestInfoItuEvent)
					((UserPartTestInfoItuEvent)event).setParamCompatibilityInfo(pci);
				else if(event instanceof NetworkResourceMgmtItuEvent)
					((NetworkResourceMgmtItuEvent)event).setParamCompatibilityInfo(pci);
				else if(event instanceof IdentificationReqItuEvent)
					((IdentificationReqItuEvent)event).setParamCompatibilityInfo(pci);
				else if(event instanceof IdentificationRespItuEvent)
					((IdentificationRespItuEvent)event).setParamCompatibilityInfo(pci);
				break;

			case IsupParameterValues.ISUP_PMT_NSF:
				NwSpecificFacItu nsf = new NwSpecificFacItu();
				nsf.putNwSpecificFacItu(par_array,index,par_len);
				if(event instanceof AlertItuEvent)
					((AlertItuEvent)event).setNwSpecificFac(nsf);
				else if(event instanceof AnswerItuEvent)
					((AnswerItuEvent)event).setNwSpecificFac(nsf);
				else if(event instanceof SetupItuEvent)
					((SetupItuEvent)event).setNwSpecificFac(nsf);
				else if(event instanceof CallProgressItuEvent)
					((CallProgressItuEvent)event).setNwSpecificFac(nsf);
				else if(event instanceof ConnectItuEvent)
					((ConnectItuEvent)event).setNwSpecificFac(nsf);
				else if(event instanceof InformationItuEvent)
					((InformationItuEvent)event).setNwSpecificFac(nsf);
				else if(event instanceof InformationReqItuEvent)
					((InformationReqItuEvent)event).setNwSpecificFac(nsf);
				else if(event instanceof ReleaseItuEvent)
					((ReleaseItuEvent)event).setNwSpecificFac(nsf);
				break;

			case IsupParameterValues.ISUP_PMT_ROP:
				RemoteOperations rop = new RemoteOperations();
				rop.putRemoteOperations(par_array,index,par_len);
				if(event instanceof AlertEvent)
						((AlertEvent)event).setRemoteOperations(rop);
				else if(event instanceof AnswerEvent)
						((AnswerEvent)event).setRemoteOperations(rop);
				else if(event instanceof SetupEvent)
						((SetupEvent)event).setRemoteOperations(rop);
				else if(event instanceof CallProgressEvent)
						((CallProgressEvent)event).setRemoteOperations(rop);
				else if(event instanceof ConnectItuEvent)
						((ConnectItuEvent)event).setRemoteOperations(rop);
				else if(event instanceof FacilityItuEvent)
						((FacilityItuEvent)event).setRemoteOperations(rop);
				break;

			case IsupParameterValues.ISUP_PMT_SAC:				
				tmp_array = ByteArray.getByteArray(par_len);
				System.arraycopy(par_array,index, tmp_array,0,par_len);		
				
				if(event instanceof AlertEvent)
					((AlertEvent)event).setServiceActivation(tmp_array);
				else if(event instanceof AnswerEvent)
					((AnswerEvent)event).setServiceActivation(tmp_array);
				else if(event instanceof SetupEvent)
					((SetupEvent)event).setServiceActivation(tmp_array);
				else if(event instanceof CallProgressEvent)
					((CallProgressEvent)event).setServiceActivation(tmp_array);
				else if(event instanceof ConnectItuEvent)
					((ConnectItuEvent)event).setServiceActivation(tmp_array);
				else if(event instanceof FacilityItuEvent)
					((FacilityItuEvent)event).setServiceActivation(tmp_array);
				
				ByteArray.returnByteArray(tmp_array);	
				break;

			case IsupParameterValues.ISUP_PMT_CONN:
				ConnectedNumberItu conn = new ConnectedNumberItu();
				conn.putConnectedNumberItu(par_array,index,par_len);
				if(event instanceof AnswerItuEvent)
						((AnswerItuEvent)event).setConnectedNumber(conn);
				else if(event instanceof ConnectItuEvent)
						((ConnectItuEvent)event).setConnectedNumber(conn);
				break;

			case IsupParameterValues.ISUP_PMT_CHI:
				long chi=0;	
				chi = (par_array[index]<<8)|(par_array[index+1]);
				if(event instanceof AnswerItuEvent)
						((AnswerItuEvent)event).setCallHistoryInfo(chi);
				else if(event instanceof ConnectItuEvent)
						((ConnectItuEvent)event).setCallHistoryInfo(chi);
				break;
				
			case IsupParameterValues.ISUP_PMT_GAD:
				GenericNumberItu gad = new GenericNumberItu();
				gad.putGenericNumberItu(par_array,index,par_len);
				if(event instanceof AnswerItuEvent)
						((AnswerItuEvent)event).setGenericNumber(gad);
				else if(event instanceof SetupItuEvent)
						((SetupItuEvent)event).setGenericNumber(gad);
				else if(event instanceof ConnectItuEvent)
						((ConnectItuEvent)event).setGenericNumber(gad);
				else if(event instanceof IdentificationRespItuEvent)
						((IdentificationRespItuEvent)event).setGenericNumber(gad);
				else if(event instanceof SegmentationItuEvent)
						((SegmentationItuEvent)event).setGenericNumber(gad);
				break;
		
			case IsupParameterValues.ISUP_PMT_XNS:
				TransitNwSel xns = new TransitNwSel();
				xns.putTransitNwSel(par_array,index,par_len);
				if(event instanceof SetupEvent)
						((SetupEvent)event).setTransitNwSel(xns);
				break;

			case IsupParameterValues.ISUP_PMT_USIP:
				UserServiceInfoPrime usip = new UserServiceInfoPrime();
				usip.putUserServiceInfoPrime(par_array,index,par_len);
				if(event instanceof SetupEvent)
						((SetupEvent)event).setUserServiceInfoPrime(usip);

				break;

			case IsupParameterValues.ISUP_PMT_CGPN:
				CallingPartyNumber cgpn = new CallingPartyNumber();
				cgpn.putCallingPartyNumber(par_array,index,par_len);
				if(event instanceof SetupEvent)
						((SetupEvent)event).setCallingPartyNumber(cgpn);
				else if(event instanceof InformationEvent)
						((InformationEvent)event).setCallingPartyNumber(cgpn);
				else if(event instanceof IdentificationRespItuEvent)
						((IdentificationRespItuEvent)event).setCallingPartyNumber((CallingPartyNumberItu)cgpn);
				break;

			case IsupParameterValues.ISUP_PMT_OFI:
				OptionalFwdCallIndItu ofi = new OptionalFwdCallIndItu();
				ofi.putOptionalFwdCallIndItu(par_array,index,par_len);
				if(event instanceof SetupItuEvent)
						((SetupItuEvent)event).setOptionalFwdCallInd(ofi);
				break;

			case IsupParameterValues.ISUP_PMT_RDTG:
				RedirectingNumber rdtg = new RedirectingNumber();
				rdtg.putRedirectingNumber(par_array,index,par_len);
				if(event instanceof SetupEvent)
						((SetupEvent)event).setRedirectingNumber(rdtg);
				break;

			case IsupParameterValues.ISUP_PMT_REDI:
				RedirectionInfo redi = new RedirectionInfo();
				redi.putRedirectionInfo(par_array, index, par_len);
				if(event instanceof SetupEvent)
						((SetupEvent)event).setRedirectionInfo(redi);
				else if(event instanceof ReleaseItuEvent)
						((ReleaseItuEvent)event).setRedirectionInfo(redi);
				break;

			case IsupParameterValues.ISUP_PMT_CUGC:
				CUGInterlockCodeItu cugc = new CUGInterlockCodeItu();
				cugc.putCUGInterlockCodeItu(par_array,index,par_len);
				if(event instanceof SetupItuEvent)
						((SetupItuEvent)event).setCUGInterlockCode(cugc);
				break;

			case IsupParameterValues.ISUP_PMT_CONR:
				ConReq conr = new ConReq();
				conr.putConReq(par_array,index,par_len,ConReq.PC_SIZE_14);
				if(event instanceof SetupEvent)
						((SetupEvent)event).setConReq(conr);
				else if(event instanceof FacilityAcceptItuEvent)
						((FacilityAcceptItuEvent)event).setConReq(conr);
				else if(event instanceof FacilityRequestItuEvent)
						((FacilityRequestItuEvent)event).setConReq(conr);		
				else if(event instanceof InformationEvent)
						((InformationEvent)event).setConReq(conr);
				break;				

			case IsupParameterValues.ISUP_PMT_OCDN:
				OrigCalledNumber ocdn = new OrigCalledNumber();
				ocdn.putOrigCalledNumber(par_array,index,par_len);
				if(event instanceof SetupEvent)
						((SetupEvent)event).setOrigCalledNumber(ocdn);
				break;		

			case IsupParameterValues.ISUP_PMT_USI:
				UserServiceInfo usi = new UserServiceInfo();
				usi.putUserServiceInfo(par_array,index,par_len);
				if(event instanceof SetupItuEvent)
						((SetupItuEvent)event).setUserServiceInfo(usi);				
				break;		

			case IsupParameterValues.ISUP_PMT_PDC:
				int pdc=0;
				pdc = ((par_array[index] << 8)&0xFF00)|(par_array[index+1]&0x00FF);
				if(event instanceof SetupItuEvent)
						((SetupItuEvent)event).setPropagationDelayCounter(pdc);
				break;

			case IsupParameterValues.ISUP_PMT_GND:
				GenericDigits gnd = new GenericDigits();
				gnd.putGenericDigits(par_array,index,par_len);
				if(event instanceof SetupEvent)
						((SetupEvent)event).setGenericDigits(gnd);
				else if(event instanceof SegmentationItuEvent)
						((SegmentationItuEvent)event).setGenericDigits(gnd);
				break;

			case IsupParameterValues.ISUP_PMT_OISC:
				tmp_array = ByteArray.getByteArray(par_len);
				System.arraycopy(par_array,index, tmp_array,0,par_len);
				
				if(event instanceof SetupItuEvent)
					((SetupItuEvent)event).setOrigISCPointCode(tmp_array);

				ByteArray.returnByteArray(tmp_array);
				break;

			case IsupParameterValues.ISUP_PMT_MLPP:
				Precedence mlpp = new Precedence();
				mlpp.putPrecedence(par_array,index,par_len);
				if(event instanceof SetupEvent)
						((SetupEvent)event).setMLPPprecedence(mlpp);
				break;

			case IsupParameterValues.ISUP_PMT_XMRP:
				if(event instanceof SetupItuEvent)
					((SetupItuEvent)event).setTransmissionMediumReqPrime(par_array[index]);

				break;

			case IsupParameterValues.ISUP_PMT_LON:
				LocationNumberItu lon = new LocationNumberItu();
				lon.putLocationNumberItu(par_array,index,par_len);
				if(event instanceof SetupItuEvent)
						((SetupItuEvent)event).setLocationNumber(lon);
				break;

			case IsupParameterValues.ISUP_PMT_MCOI:
				MessageCompatibilityInfoItu mcoi = new MessageCompatibilityInfoItu();
				mcoi.putMessageCompatibilityInfoItu(par_array,index,par_len);
				if(event instanceof FacilityItuEvent)
						((FacilityItuEvent)event).setMessageCompatibilityInfo(mcoi);
				else if(event instanceof NetworkResourceMgmtItuEvent)
						((NetworkResourceMgmtItuEvent)event).setMessageCompatibilityInfo(mcoi);
				else if(event instanceof IdentificationReqItuEvent)
						((IdentificationReqItuEvent)event).setMessageCompatibilityInfo(mcoi);
				else if(event instanceof IdentificationRespItuEvent)
						((IdentificationRespItuEvent)event).setMessageCompatibilityInfo(mcoi);
				else if(event instanceof SegmentationItuEvent)
						((SegmentationItuEvent)event).setMessageCompatibilityInfo(mcoi);

				break;

			case IsupParameterValues.ISUP_PMT_SPC:
				SignalingPointCode spc = new SignalingPointCode(-1,-1,-1);
				spc.putSignalingPointCode(par_array,index,par_len);
				if(event instanceof ReleaseItuEvent)
						((ReleaseItuEvent)event).setSignalingPointCode(spc);

				break;

			case IsupParameterValues.ISUP_PMT_ACL:
				if(event instanceof ReleaseEvent)
					((ReleaseEvent)event).setAutoCongLevel(par_array[index]);
				break;

			case IsupParameterValues.ISUP_PMT_MRQ:
				MCIDReqIndItu mcreq = new  MCIDReqIndItu();
				mcreq.putMCIDReqIndItu(par_array,index,par_len);
				if(event instanceof IdentificationReqItuEvent)
					((IdentificationReqItuEvent)event).setMCIDReqInd(mcreq);

				break;

			case IsupParameterValues.ISUP_PMT_MRS:
				MCIDRespIndItu mcresp = new MCIDRespIndItu();
				mcresp.putMCIDRespIndItu(par_array,index,par_len);
				if(event instanceof IdentificationRespItuEvent)
					((IdentificationRespItuEvent)event).setMCIDRespInd(mcresp);

				break;

			default:
				// Unknown Parameter
				break;
		}
		}catch (Exception exception) {};
	}

	////////////////////////////////////////////////////////////////////////////////
    // processAnsiIsupParam()
    ////////////////////////////////////////////////////////////////////////////////	

	private void processAnsiIsupParam(IsupEvent event, byte par_code,byte par_len, byte[]par_array, int index) 
		throws ParameterRangeInvalidException{
		
		int i = 0;
		byte[] tmp_array = null;

		try{
		switch(par_code){

			case IsupParameterValues.ISUP_PMT_ACL:
				if(event instanceof ReleaseEvent)
					((ReleaseEvent)event).setAutoCongLevel(par_array[index]);
				break;

			case IsupParameterValues.ISUP_PMT_AXP:
				tmp_array = ByteArray.getByteArray(par_len);
				System.arraycopy(par_array,index, tmp_array,0,par_len);		
				
				if(event instanceof AlertEvent)
					((AlertEvent)event).setAccessTransport(tmp_array);				
				else if(event instanceof AnswerEvent)
					((AnswerEvent)event).setAccessTransport(tmp_array);
				else if(event instanceof CallProgressEvent)
					((CallProgressEvent)event).setAccessTransport(tmp_array);
				else if(event instanceof InformationAnsiEvent)
					((InformationAnsiEvent)event).setAccessTransport(tmp_array);
				else if(event instanceof SetupEvent)
					((SetupEvent)event).setAccessTransport(tmp_array);
				else if(event instanceof ReleaseEvent)
					((ReleaseEvent)event).setAccessTransport(tmp_array);
				ByteArray.returnByteArray(tmp_array);	
				break;
		
			case IsupParameterValues.ISUP_PMT_BACI:
				BwdCallIndAnsi baci = new BwdCallIndAnsi();
				baci.putBwdCallIndAnsi(par_array,index,par_len);
				if(event instanceof AnswerEvent)
					((AnswerEvent)event).setBwdCallInd(baci);	
				else if(event instanceof CallProgressEvent)
					((CallProgressEvent)event).setBwdCallInd(baci);	
				break;

			case IsupParameterValues.ISUP_PMT_BUG:
				BusinessGroupAnsi bug = new BusinessGroupAnsi();
				bug.putBusinessGroupAnsi(par_array,index,par_len);
				if(event instanceof AlertAnsiEvent)
					((AlertAnsiEvent)event).setBusinessGroup(bug);	
				else if(event instanceof AnswerAnsiEvent)
					((AnswerAnsiEvent)event).setBusinessGroup(bug);	
				else if(event instanceof CallProgressAnsiEvent)
					((CallProgressAnsiEvent)event).setBusinessGroup(bug);	
				else if(event instanceof InformationAnsiEvent)
					((InformationAnsiEvent)event).setBusinessGroup(bug);	
				else if(event instanceof SetupAnsiEvent)
					((SetupAnsiEvent)event).setBusinessGroup(bug);	
				break;

			case IsupParameterValues.ISUP_PMT_CAM:
				CircuitAssignMapAnsi cam = new CircuitAssignMapAnsi();
				cam.putCircuitAssignMapAnsi(par_array,index,par_len);
				if(event instanceof SetupAnsiEvent)
					((SetupAnsiEvent)event).setCircuitAssignMap(cam);
				else if(event instanceof CircuitGrpResetAnsiEvent)
					((CircuitGrpResetAnsiEvent)event).setCircuitAssignMap(cam);	
				else if(event instanceof CircuitGrpResetAckAnsiEvent)
					((CircuitGrpResetAckAnsiEvent)event).setCircuitAssignMap(cam);	
				break;

			case IsupParameterValues.ISUP_PMT_CGPC:
				if(event instanceof InformationEvent)
					((InformationEvent)event).setCallingPartyCat(par_array[index]);
				break;

			case IsupParameterValues.ISUP_PMT_CGPN:
				CallingPartyNumber cgpn = new CallingPartyNumber();
				cgpn.putCallingPartyNumber(par_array,index,par_len);
				if(event instanceof InformationEvent)
						((InformationEvent)event).setCallingPartyNumber(cgpn);
				else if(event instanceof SetupEvent)
						((SetupEvent)event).setCallingPartyNumber(cgpn);
				break;

			case IsupParameterValues.ISUP_PMT_CIC:
				CarrierIdAnsi cic = new CarrierIdAnsi();
				cic.putCarrierIdAnsi(par_array,index,par_len);
				if(event instanceof SetupAnsiEvent)
						((SetupAnsiEvent)event).setCarrierId(cic);
				break;

			case IsupParameterValues.ISUP_PMT_CINM:
				CircuitIdNameAnsi cinm = new CircuitIdNameAnsi();
				cinm.putCircuitIdNameAnsi(par_array,index,par_len);
				if(event instanceof CircuitValidationRespAnsiEvent)
					((CircuitValidationRespAnsiEvent)event).setCircuitIdName(cinm);
				break;

			case IsupParameterValues.ISUP_PMT_CLLI:
				CommonLangLocationIdAnsi clli = new CommonLangLocationIdAnsi();
				clli.putCommonLangLocationIdAnsi(par_array,index,par_len);
				if(event instanceof CircuitValidationRespAnsiEvent)
					((CircuitValidationRespAnsiEvent)event).setCommonLangLocationId(clli);
				break;
			
			case IsupParameterValues.ISUP_PMT_CONR:
				ConReq conr = new ConReq();
				conr.putConReq(par_array,index,par_len,ConReq.PC_SIZE_24);
				if(event instanceof AlertAnsiEvent)						
					((AlertAnsiEvent)event).setConReq(conr);
				else if(event instanceof AnswerAnsiEvent)						
					((AnswerAnsiEvent)event).setConReq(conr);
				else if(event instanceof InformationEvent)						
					((InformationEvent)event).setConReq(conr);
				else if(event instanceof InformationReqAnsiEvent)						
					((InformationReqAnsiEvent)event).setConReq(conr);
				else if(event instanceof SetupEvent)						
					((SetupEvent)event).setConReq(conr);
				break;

			case IsupParameterValues.ISUP_PMT_CRF:
				CallReference crf = new CallReference();
				crf.putCallReference(par_array,index,par_len,CallReference.PC_SIZE_24);
				if(event instanceof AlertEvent)
					((AlertEvent)event).setCallReference(crf);
				else if(event instanceof AnswerEvent)
					((AnswerEvent)event).setCallReference(crf);
				else if(event instanceof CallProgressEvent)
					((CallProgressEvent)event).setCallReference(crf);
				else if(event instanceof InformationEvent)
					((InformationEvent)event).setCallReference(crf);
				else if(event instanceof InformationReqEvent)
					((InformationReqEvent)event).setCallReference(crf);
				else if(event instanceof SetupEvent)
					((SetupEvent)event).setCallReference(crf);
				else if(event instanceof ReleaseAnsiEvent)
					((ReleaseAnsiEvent)event).setCallReference(crf);
				else if(event instanceof ForwardTransferEvent)
					((ForwardTransferEvent)event).setCallReference(crf);
				else if(event instanceof SuspendResumeEvent)
					((SuspendResumeEvent)event).setCallReference(crf); 
				break;

			case IsupParameterValues.ISUP_PMT_CRGN:
				ChargeNumberAnsi crgn = new ChargeNumberAnsi();
				crgn.putChargeNumberAnsi(par_array,index,par_len);
				if(event instanceof InformationAnsiEvent)
					((InformationAnsiEvent)event).setChargeNumber(crgn);
				else if(event instanceof SetupAnsiEvent)
					((SetupAnsiEvent)event).setChargeNumber(crgn);
				else if(event instanceof ReleaseAnsiEvent)
					((ReleaseAnsiEvent)event).setChargeNumber(crgn);
				break;
			
			case IsupParameterValues.ISUP_PMT_CSI:				
				CauseInd csi =  new CauseInd();					
				csi.putCauseInd(par_array,index,par_len);
				if(event instanceof AlertEvent)
					((AlertEvent)event).setCauseInd(csi);	
				else if(event instanceof CallProgressEvent)
					((CallProgressEvent)event).setCauseInd(csi);			
				break;			
				
			case IsupParameterValues.ISUP_PMT_CSIN:
				short csin=0;
				csin = (short)par_array[index];
				if(event instanceof SetupAnsiEvent)
					((SetupAnsiEvent)event).setCarrierSelInfo(csin);	
				break;			
			
			case IsupParameterValues.ISUP_PMT_EGR:
				tmp_array = ByteArray.getByteArray(par_len);
				System.arraycopy(par_array,index, tmp_array,0,par_len);
				
				if(event instanceof SetupAnsiEvent)
					((SetupAnsiEvent)event).setEgressService(tmp_array);
				ByteArray.returnByteArray(tmp_array);
				break;

			case IsupParameterValues.ISUP_PMT_GAD:
				GenericNumber gad = new GenericNumber();
				gad.putGenericNumber(par_array,index,par_len);	
				if(event instanceof SetupEvent)						
					((SetupEvent)event).setGenericNumber(gad);
				else if(event instanceof ReleaseAnsiEvent)						
					((ReleaseAnsiEvent)event).setGenericNumber(gad);
				break;

			case IsupParameterValues.ISUP_PMT_GND:
				GenericDigits gnd = new GenericDigits();
				gnd.putGenericDigits(par_array,index,par_len);	
				if(event instanceof SetupEvent)						
					((SetupEvent)event).setGenericDigits(gnd);
				break;

			case IsupParameterValues.ISUP_PMT_GNM:
				GenericNameAnsi gnm = new GenericNameAnsi();
				gnm.putGenericNameAnsi(par_array,index,par_len);	
				if(event instanceof SetupAnsiEvent)						
					((SetupAnsiEvent)event).setGenericName(gnm);
				break;
			
			case IsupParameterValues.ISUP_PMT_HOP:
				if(event instanceof SetupAnsiEvent)
					((SetupAnsiEvent)event).setHopCounter(par_array[index]);

				break;

			case IsupParameterValues.ISUP_PMT_INI:
				InfoIndAnsi inf = new InfoIndAnsi();					
				inf.putInfoIndAnsi(par_array,index,par_len);	
				if(event instanceof AlertAnsiEvent)						
					((AlertAnsiEvent)event).setInfoInd(inf);
				else if(event instanceof AnswerAnsiEvent)						
					((AnswerAnsiEvent)event).setInfoInd(inf);
				else if(event instanceof CallProgressAnsiEvent)						
					((CallProgressAnsiEvent)event).setInfoInd(inf);
				break;

			case IsupParameterValues.ISUP_PMT_INRI:
				InfoReqIndAnsi inri = new InfoReqIndAnsi();
				inri.putInfoReqIndAnsi(par_array,index,par_len);	
				if(event instanceof SetupAnsiEvent)						
					((SetupAnsiEvent)event).setInfoReqInd(inri);

				break;

			case IsupParameterValues.ISUP_PMT_JUR:
				tmp_array = ByteArray.getByteArray(par_len);
				System.arraycopy(par_array,index, tmp_array,0,par_len);
				
				if(event instanceof SetupAnsiEvent)
					((SetupAnsiEvent)event).setJurisdiction(tmp_array);
				ByteArray.returnByteArray(tmp_array);
				break;
				
			case IsupParameterValues.ISUP_PMT_MLPP:
				Precedence mlpp = new Precedence();
				mlpp.putPrecedence(par_array,index,par_len);
				if(event instanceof SetupEvent)
						((SetupEvent)event).setMLPPprecedence(mlpp);
				break;


			case IsupParameterValues.ISUP_PMT_NTP:
				tmp_array = ByteArray.getByteArray(par_len);
				System.arraycopy(par_array,index, tmp_array,0,par_len);
				
				if(event instanceof AlertAnsiEvent)
					((AlertAnsiEvent)event).setNetworkTransport(tmp_array);
				else if(event instanceof AnswerAnsiEvent)
					((AnswerAnsiEvent)event).setNetworkTransport(tmp_array);
				else if(event instanceof CallProgressAnsiEvent)
					((CallProgressAnsiEvent)event).setNetworkTransport(tmp_array);
				else if(event instanceof InformationReqAnsiEvent)
					((InformationReqAnsiEvent)event).setNetworkTransport(tmp_array);
				else if(event instanceof SetupAnsiEvent)
					((SetupAnsiEvent)event).setNetworkTransport(tmp_array);
				ByteArray.returnByteArray(tmp_array);
				break;
		
			case IsupParameterValues.ISUP_PMT_NTI:
				NotificationInd nti = new NotificationInd();
				nti.putNotificationInd(par_array,index,par_len);	
				if(event instanceof AlertEvent)						
					((AlertEvent)event).setNotificationInd(nti);
				else if(event instanceof AnswerEvent)						
					((AnswerEvent)event).setNotificationInd(nti);
				else if(event instanceof CallProgressEvent)						
					((CallProgressEvent)event).setNotificationInd(nti);
				break;

			case IsupParameterValues.ISUP_PMT_OBI:
				OptionalBwdCallIndAnsi obi = new OptionalBwdCallIndAnsi();
				obi.putOptionalBwdCallIndAnsi(par_array,index,par_len);	
				if(event instanceof AlertEvent)						
					((AlertEvent)event).setOptionalBwdCallInd(obi);
				else if(event instanceof AnswerEvent)						
					((AnswerEvent)event).setOptionalBwdCallInd(obi);
				else if(event instanceof CallProgressEvent)						
					((CallProgressEvent)event).setOptionalBwdCallInd(obi);
				break;

			case IsupParameterValues.ISUP_PMT_OCDN:
				OrigCalledNumber ocdn = new OrigCalledNumber();
				ocdn.putOrigCalledNumber(par_array,index,par_len);	
				if(event instanceof SetupEvent)						
					((SetupEvent)event).setOrigCalledNumber(ocdn);
				break;

			case IsupParameterValues.ISUP_PMT_OLI:
				if(event instanceof InformationAnsiEvent)
					((InformationAnsiEvent)event).setOrigLineInfo(par_array[index]);
				else if(event instanceof SetupAnsiEvent)
					((SetupAnsiEvent)event).setOrigLineInfo(par_array[index]);
				break;

			case IsupParameterValues.ISUP_PMT_OPSI:
				OperatorServicesInfoAnsi opsi = new OperatorServicesInfoAnsi();
				opsi.putOperatorServicesInfoAnsi(par_array,index,par_len);	
				if(event instanceof SetupAnsiEvent)						
					((SetupAnsiEvent)event).setOperatorServicesInfo(opsi);
				break;

			case IsupParameterValues.ISUP_PMT_OTGN:
				tmp_array = ByteArray.getByteArray(par_len);
				System.arraycopy(par_array,index, tmp_array,0,par_len);		
				String otgn = new String(tmp_array);
				if(event instanceof ExitAnsiEvent)						
					((ExitAnsiEvent)event).setOutgoingTrunkGrpNumber(otgn);
				ByteArray.returnByteArray(tmp_array);	
				break;
		
			case IsupParameterValues.ISUP_PMT_RDTG:
				RedirectingNumber rdtg = new RedirectingNumber();
				rdtg.putRedirectingNumber(par_array,index,par_len);	
				if(event instanceof InformationAnsiEvent)						
					((InformationAnsiEvent)event).setRedirectingNumber(rdtg);
				else if(event instanceof SetupEvent)						
					((SetupEvent)event).setRedirectingNumber(rdtg); 
				break;

			case IsupParameterValues.ISUP_PMT_REDI:
				RedirectionInfo redi = new RedirectionInfo();
				redi.putRedirectionInfo(par_array,index,par_len);	
				if(event instanceof AlertAnsiEvent)						
					((AlertAnsiEvent)event).setRedirectionInfo(redi);
				else if(event instanceof InformationAnsiEvent)						
					((InformationAnsiEvent)event).setRedirectionInfo(redi);
				else if(event instanceof SetupEvent)						
					((SetupEvent)event).setRedirectionInfo(redi);
				break;

			case IsupParameterValues.ISUP_PMT_ROP:
				RemoteOperations rop = new RemoteOperations(); 
				rop.putRemoteOperations(par_array,index,par_len);	
				if(event instanceof AlertEvent)						
					((AlertEvent)event).setRemoteOperations(rop);
				else if(event instanceof AnswerEvent)						
					((AnswerEvent)event).setRemoteOperations(rop);
				else if(event instanceof CallProgressEvent)						
					((CallProgressEvent)event).setRemoteOperations(rop);
				else if(event instanceof FacilityEvent)						
					((FacilityEvent)event).setRemoteOperations(rop);
				else if(event instanceof SetupEvent)						
					((SetupEvent)event).setRemoteOperations(rop);
				break;

			case IsupParameterValues.ISUP_PMT_SAC:
				tmp_array = ByteArray.getByteArray(par_len);
				System.arraycopy(par_array,index, tmp_array,0,par_len);
				
				if(event instanceof AlertEvent)
					((AlertEvent)event).setServiceActivation(tmp_array);
				else if(event instanceof AnswerEvent)
					((AnswerEvent)event).setServiceActivation(tmp_array);
				else if(event instanceof CallProgressEvent)
					((CallProgressEvent)event).setServiceActivation(tmp_array);
				else if(event instanceof FacilityEvent)
					((FacilityEvent)event).setServiceActivation(tmp_array);
				else if(event instanceof SetupEvent)
					((SetupEvent)event).setServiceActivation(tmp_array);
				else if(event instanceof ReleaseAnsiEvent)
					((ReleaseAnsiEvent)event).setServiceActivation(tmp_array);

				ByteArray.returnByteArray(tmp_array);
				break;
		
			case IsupParameterValues.ISUP_PMT_SEC:
				short sec=0;
				sec = (short)par_array[index];
				if(event instanceof SetupAnsiEvent)
					((SetupAnsiEvent)event).setServiceCode(sec);	

				break;

			case IsupParameterValues.ISUP_PMT_SPR:
				short spr=0;
				spr = (short)par_array[index]; 
				if(event instanceof SetupAnsiEvent)
					((SetupAnsiEvent)event).setSpecialProcReq(spr);	

				break;

			case IsupParameterValues.ISUP_PMT_TRR:
				TransactionReqAnsi trr = new TransactionReqAnsi();
				trr.putTransactionReqAnsi(par_array,index,par_len);	
				if(event instanceof SetupAnsiEvent)						
					((SetupAnsiEvent)event).setTransactionReq(trr);
				
				break;

			case IsupParameterValues.ISUP_PMT_XMU:
				if(event instanceof AlertEvent)
					((AlertEvent)event).setTransmissionMediumUsed(par_array[index]); 
				else if(event instanceof AnswerEvent)
					((AnswerEvent)event).setTransmissionMediumUsed(par_array[index]);
				else if(event instanceof CallProgressEvent)
					((CallProgressEvent)event).setTransmissionMediumUsed(par_array[index]); 
				break;

			case IsupParameterValues.ISUP_PMT_XNS:
				TransitNwSel xns = new TransitNwSel();
				xns.putTransitNwSel(par_array,index,par_len);
				if(event instanceof SetupEvent)
						((SetupEvent)event).setTransitNwSel(xns);
				break;

			case IsupParameterValues.ISUP_PMT_USIP:
				UserServiceInfoPrime usip = new UserServiceInfoPrime();
				usip.putUserServiceInfoPrime(par_array,index,par_len);
				if(event instanceof SetupEvent)
						((SetupEvent)event).setUserServiceInfoPrime(usip);

				break;

			case IsupParameterValues.ISUP_PMT_UUIC:
				UserToUserIndicators uuic = new UserToUserIndicators();
				uuic.putUserToUserIndicators(par_array,index,par_len);
				if(event instanceof AlertEvent)
					((AlertEvent)event).setUserToUserIndicators(uuic);
				else if(event instanceof AnswerEvent)
					((AnswerEvent)event).setUserToUserIndicators(uuic);
				else if(event instanceof CallProgressEvent)
					((CallProgressEvent)event).setUserToUserIndicators(uuic);
				break;

			case IsupParameterValues.ISUP_PMT_UUIN:				
				tmp_array = ByteArray.getByteArray(par_len);
				System.arraycopy(par_array,index, tmp_array,0,par_len);
								
				if(event instanceof AlertEvent)
					((AlertEvent)event).setUserToUserInformation(tmp_array);
				else if(event instanceof AnswerEvent)
					((AnswerEvent)event).setUserToUserInformation(tmp_array);
				else if(event instanceof CallProgressEvent)
					((CallProgressEvent)event).setUserToUserInformation(tmp_array);
				else if(event instanceof InformationAnsiEvent)
					((InformationAnsiEvent)event).setUserToUserInformation(tmp_array);
				else if(event instanceof SetupEvent)
					((SetupEvent)event).setUserToUserInformation(tmp_array);
				else if(event instanceof ReleaseEvent)
					((ReleaseEvent)event).setUserToUserInformation(tmp_array);
				ByteArray.returnByteArray(tmp_array);	
				break;

			default:
				// Unknown Parameter
				break;
		}
		}catch (Exception exception) {};
	}
	
	
	////////////////////////////////////////////////////////////////////////////////
    // 
    ////////////////////////////////////////////////////////////////////////////////	

	private synchronized native int isupReg(int sp, 
						int stackSpec, 
						int startCIC,
						int endCIC,
						int pc,
						IsupUserAddress addr); 

	private synchronized native int isupDereg(int regIdx);

	private static native int initIsupProvider ();
	
	private native int putMsg	(int regIdx, 
										int cic,
										int opc,
										int dpc,
										byte sls,
										byte congpri,
										byte[] par_array,
										int primitive,
										int stackSpec);

			
	JainIsupStackImpl  ownStack;
	
	private Hashtable listeners = new Hashtable();

	private Hashtable idxLookup = new Hashtable();	

	private Vector usrAddrLookup = new Vector();	

}


