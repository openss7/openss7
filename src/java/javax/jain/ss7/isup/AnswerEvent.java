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
* DATE         : $Date: 2008/05/16 12:23:50 $
* 
* MODULE NAME  : $RCSfile: AnswerEvent.java,v $
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

public abstract class AnswerEvent extends IsupEvent{

	protected AnswerEvent(java.lang.Object source,
                      SignalingPointCode dpc,
                      SignalingPointCode opc,
                      byte sls,
                      int cic,
                      byte congestionPriority)
               throws ParameterRangeInvalidException{

		super(source,dpc,opc,sls,cic,congestionPriority);
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
		
	public BwdCallInd getBwdCallInd()
                         throws ParameterNotSetException{
		if(isBwdCallInd == true)
				return bwdCallInd;
		else throw new ParameterNotSetException();
	}

	public boolean isBwdCallIndPresent(){
		return isBwdCallInd;
	}

	public void setBwdCallInd(BwdCallInd bci){
		bwdCallInd = bci;
		isBwdCallInd = true;
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

	public int getIsupPrimitive(){
                return IsupConstants.ISUP_PRIMITIVE_ANSWER;
    }

	public NotificationInd getNotificationInd()
                                   throws ParameterNotSetException{
                if(isNotificationInd == true)
                                return notificationInd;
                else throw new ParameterNotSetException();
    }

    public boolean isNotificationIndPresent(){
                return isNotificationInd;
    }

    public void setNotificationInd(NotificationInd noi){
                notificationInd = noi;
                isNotificationInd = true;
    }

	public OptionalBwdCallInd getOptionalBwdCallInd()
                                 throws ParameterNotSetException{
                if(isOptionalBwdCallInd == true)
                                return optionalBwdCallInd;
                else throw new ParameterNotSetException();
    }

    public boolean isOptionalBwdCallIndPresent(){
                return isOptionalBwdCallInd;
    }

    public void setOptionalBwdCallInd(OptionalBwdCallInd obci){
                optionalBwdCallInd = obci;
                isOptionalBwdCallInd = true;
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

	public byte getTransmissionMediumUsed()
                               throws ParameterNotSetException{
                if(isTransmissionMediumUsed == true)
                                return transmissionMediumUsed;
                else throw new ParameterNotSetException();
    }

    public boolean isTransmissionMediumUsedPresent(){
                return isTransmissionMediumUsed;
    }

    public void setTransmissionMediumUsed(byte tmu)
                               throws ParameterRangeInvalidException{
		if ((tmu == TMR_SPEECH) || ((tmu >= TMR_64_KBPS_UNRESTRICTED) && (tmu <= TMR_1920_KBPS_UNRESTRICTED))) {
			transmissionMediumUsed = tmu;
			isTransmissionMediumUsed = true;
		}
		else 
			throw new ParameterRangeInvalidException();
    }

	public UserToUserIndicators getUserToUserIndicators()
                                             throws ParameterNotSetException{
                if(isUserToUserIndicators == true)
                                return  userToUserIndicators;
                else throw new ParameterNotSetException();
	}

    public boolean isUserToUserIndicatorsPresent(){
                return isUserToUserIndicators;
    }

    public void setUserToUserIndicators(UserToUserIndicators uui){
                userToUserIndicators = uui;
                isUserToUserIndicators = true;
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
    * String representation of class AnswerEvent
    *
    * @return    String provides description of class AnswerEvent
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
				buffer.append("\n\nisBwdCallInd = ");
				buffer.append(isBwdCallInd);
				buffer.append("\nbwdCallInd  = ");
				buffer.append(bwdCallInd);
				buffer.append("\n\nisCallReference = ");
				buffer.append(isCallReference);
				buffer.append("\ncallReference = ");
				buffer.append(callReference);
				buffer.append("\n\nisNotificationInd = ");
				buffer.append(isNotificationInd);
				buffer.append("\nnotificationInd = ");
				buffer.append(notificationInd);
				buffer.append("\n\nisOptionalBwdCallInd = ");
				buffer.append(isOptionalBwdCallInd);
				buffer.append("\noptionalBwdCallInd = ");
				buffer.append(optionalBwdCallInd);
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
				buffer.append("\n\nisUserToUserIndicators = ");
				buffer.append(isUserToUserIndicators);
				buffer.append("\nuserToUserIndicators = ");
				buffer.append(userToUserIndicators);
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
	BwdCallInd           bwdCallInd;	
	CallReference        callReference;
	boolean              isAccessTransport;
	boolean              isBwdCallInd;
	boolean              isCallReference;
	boolean              isNotificationInd;
	boolean              isOptionalBwdCallInd;
	boolean              isRemoteOperations;
	boolean              isServiceActivation;
	boolean              isTransmissionMediumUsed;
	boolean              isUserToUserIndicators;
	boolean              isUserToUserInformation;
	NotificationInd      notificationInd;
	OptionalBwdCallInd   optionalBwdCallInd;
	RemoteOperations     remoteOperations;
	byte[]               serviceActivation;
	byte                 transmissionMediumUsed;
	UserToUserIndicators userToUserIndicators;
	byte[]               userToUserInformation;
    
	public static final byte TMR_SPEECH = 0x00; 
	public static final byte TMR_64_KBPS_UNRESTRICTED = 0x02; 
	public static final byte TMR_1920_KBPS_UNRESTRICTED = 0x0A;
}


