package com.ss8.jain.protocol.ss7.tcap;

import java.util.*;

import jain.*;
import jain.protocol.ss7.*;
import jain.protocol.ss7.tcap.*;
import jain.protocol.ss7.tcap.component.*;
import jain.protocol.ss7.tcap.dialogue.*;


////////////////////////////////////////////////////////////////////////////////////
//  CLASS JainTcapProviderImpl
////////////////////////////////////////////////////////////////////////////////////
public class JainTcapProviderImpl implements JainTcapProvider {

    public static final int NONE             = AddressConstants.NOT_SET;
    public static final int DEFAULT_BUF_SIZE = 100;
    public static final int DEFAULT_NUM_BUFS = 10000;
    public static final int DEFAULT_NUM_DLGS = 16000;
    public static final int SUCCESS          =  0;
    public static final int FAIL             = -1;
    public static final int FAIL_SYSAPI      = -1;
    public static final int FAIL_INVINP      = -2;
    public static final int FAIL_NOTREG      = -3;
    public static final int FAIL_MAXCON      = -4;
    public static final int FAIL_ILL_IDX     = -5;
    public static final int FAIL_WRONG_SIZE  = -6;
    public static final int FAIL_WRONG_PROTO = -7;
    public static final int FAIL_SYSTEM      = -8;
    public static final int FAIL_MTP         = -9;
    public static final int FAIL_NO_MEM      = -10;
    public static final int FAIL_WRONG_INFO  = -11;
    private static final int BYTE2INT_MASK   = 0x00FF;
    private int bufSize                      = DEFAULT_BUF_SIZE;
    private int numBufs                      = DEFAULT_NUM_BUFS;
    private int numDlgs                      = DEFAULT_NUM_DLGS;
    private int regIdx                       = NONE; // tcap registration index
    private int ssn                          = NONE;
    private int m_invokeId                   = 0;
    private JainTcapListener listener        = null;
    private JainTcapStackImpl  ownStack;


    ////////////////////////////////////////////////////////////////////////////////
    // static()
    ////////////////////////////////////////////////////////////////////////////////
    static {
	initTcapProvider();
    }


    ////////////////////////////////////////////////////////////////////////////////
    // JainTcapProviderImpl()
    ////////////////////////////////////////////////////////////////////////////////
    JainTcapProviderImpl(JainTcapStackImpl d7Stack) throws ClassNotFoundException {
	ownStack = d7Stack;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // addJainTcapListener( SccpUserAddress )
    ////////////////////////////////////////////////////////////////////////////////
    public void addJainTcapListener(JainTcapListener listener, SccpUserAddress userAddress)
	throws java.util.TooManyListenersException, ListenerAlreadyRegisteredException, InvalidAddressException {


	int spNo = ((JainTcapStackImpl) getStack()).getSPNo();

	if (this.listener != null) {

		// We support one listener per provider
		throw new java.util.TooManyListenersException("Only one listener per provider");
	}

	try {

		SubSystemAddress   listenerAddress = userAddress.getSubSystemAddress();
		int                ssn             = listenerAddress.getSubSystemNumber ();
		SignalingPointCode userPC          = listenerAddress.getSignalingPointCode();


		int[] pcInfoArray = ownStack.getSignalingPointCode();

		SignalingPointCode  ownPC = new SignalingPointCode(pcInfoArray[0],
								pcInfoArray[1],
								pcInfoArray[2]);


		int                regIdx;

		if (userPC.equals(ownPC)) {

			if ((regIdx = tcapSccpReg(((JainTcapStackImpl) getStack()).getSPNo(),
						ssn,
						getStack().getStackSpecification(),
						bufSize,
						numBufs,
						numDlgs)) < 0) {
				throw new InvalidAddressException("tcap registration failed ");
			}

			this.regIdx   = regIdx;
			this.ssn      = ssn;
			this.listener = listener;
		} else {
			throw new InvalidAddressException("Invalid point code");
		}

	} catch (JainException exc) {
		throw new InvalidAddressException(exc.toString());
	}
    }


    ////////////////////////////////////////////////////////////////////////////////
    // removeJainTcapListener()
    ////////////////////////////////////////////////////////////////////////////////
    public void removeJainTcapListener(JainTcapListener listener) throws ListenerNotRegisteredException {

	if (listener != null) {
		if (!this.listener.equals(listener)) {
			// We support one listener per provider
			throw new ListenerNotRegisteredException();
		}
	}

	this.listener = null;

	if (tcapDereg(regIdx) != SUCCESS) {
		throw new ListenerNotRegisteredException ("Native call tcapDereg failed");
	}
    }


    ////////////////////////////////////////////////////////////////////////////////
    // cleanup()
    ////////////////////////////////////////////////////////////////////////////////
    public void cleanup() {

	
	if (listener != null) {
		int[] pcInfoArray = ownStack.getSignalingPointCode();

		SignalingPointCode  ownPC = new SignalingPointCode(pcInfoArray[0],
								pcInfoArray[1],
								pcInfoArray[2]);

		try {
			SubSystemAddress userAddress = new SubSystemAddress(ownPC,(short) ssn);

			listener.removeUserAddress(new SccpUserAddress(userAddress));

			removeJainTcapListener(listener);
		} catch (JainException exc) {};
	} 

	// remove stack reference
	ownStack = null;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // getNewDialogueId() / releaseDialogueId()
    ////////////////////////////////////////////////////////////////////////////////
    public int getNewDialogueId() throws IdNotAvailableException {

	int dlgId;

	if (listener == null) { // This provider not registered yet, throw the only available exception
		throw new IdNotAvailableException ("No listener registered yet");
	}

	dlgId = getDlgId(regIdx);

	if (dlgId == NONE) {
		throw new IdNotAvailableException("");
	}

	return dlgId;
    }

    public void releaseDialogueId(int dialogueId) throws IdNotAvailableException{

	if (listener == null) { // This provider not registered yet, throw the only available exception
		throw new IdNotAvailableException ("No listener registered yet");
	}

	if (rlsDlgId(regIdx, dialogueId) != SUCCESS) {
		throw new IdNotAvailableException("");
	}

	return;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // getNewInvokeId() / releaseInvokeId()
    ////////////////////////////////////////////////////////////////////////////////
    public int getNewInvokeId(int dialogueId) throws IdNotAvailableException{

	if (m_invokeId > 254)  // 255 reserved as NULL_INVOKE_ID in tcap.h
		m_invokeId = 0;
	return m_invokeId++;
    }

    public void releaseInvokeId(int invokeId, int dialogueId) throws IdNotAvailableException {

    }



    ////////////////////////////////////////////////////////////////////////////////
    // sendComponentReqEvent()
    ////////////////////////////////////////////////////////////////////////////////
    public void sendComponentReqEvent(ComponentReqEvent event) throws MandatoryParameterNotSetException {

	if (listener == null) { // This provider not registered yet, throw the only available exception
		throw new MandatoryParameterNotSetException("No listener registered yet");
	}

	if (event instanceof InvokeReqEvent) {
		sendInvokeReqEvent((InvokeReqEvent) event);

	} else if (event instanceof ResultReqEvent) {
		sendResultReqEvent((ResultReqEvent) event);

	} else if (event instanceof ErrorReqEvent) {
		sendErrorReqEvent((ErrorReqEvent) event);

	} else if (event instanceof RejectReqEvent) {
		sendRejectReqEvent((RejectReqEvent) event);

	} else if (event instanceof UserCancelReqEvent) {
		sendUserCancelReqEvent((UserCancelReqEvent) event);

	} else if (event instanceof TimerResetReqEvent) {
		sendTimerResetReqEvent((TimerResetReqEvent) event);
	}
    }


    ////////////////////////////////////////////////////////////////////////////////
    // sendDialogueReqEvent()
    ////////////////////////////////////////////////////////////////////////////////
    public void sendDialogueReqEvent(DialogueReqEvent event) throws MandatoryParameterNotSetException {
	int userAbortReason = NONE;


	if (listener == null) { // This provider not registered yet, throw the only available exception
		throw new MandatoryParameterNotSetException("No listener registered yet");
	}

	if (event.isDialoguePortionPresent()) {
		// handle dialogue portion
		DialoguePortion dialogPortion = null;

		try {
		        if (event instanceof UnidirectionalReqEvent) {
		        } else if (event instanceof BeginReqEvent) {
		        } else if (event instanceof ContinueReqEvent) {
		        } else if (event instanceof EndReqEvent) {
		        } else if (event instanceof UserAbortReqEvent) {
				try {
					userAbortReason = ((UserAbortReqEvent) event).getAbortReason();
				} catch (ParameterNotSetException exc) {
					throw new MandatoryParameterNotSetException("abort reason not set");
				}
		        }


			sendDialoguePortion (event.getDialogueId(), 
					     event.getDialoguePortion(),
					     event.getPrimitiveType(),
					     userAbortReason);

		} catch (ParameterNotSetException exc) {
			throw new MandatoryParameterNotSetException(exc.toString());
		};
	}

	if (event instanceof UnidirectionalReqEvent) {
		sendUnidirectionalReqEvent((UnidirectionalReqEvent) event);

	} else if (event instanceof BeginReqEvent) {
		sendBeginReqEvent((BeginReqEvent) event);

	} else if (event instanceof ContinueReqEvent) {
		sendContinueReqEvent((ContinueReqEvent) event);

	} else if (event instanceof EndReqEvent) {
		sendEndReqEvent((EndReqEvent) event);

	} else if (event instanceof UserAbortReqEvent) {
		sendUserAbortReqEvent((UserAbortReqEvent) event);
	}

    }


    ////////////////////////////////////////////////////////////////////////////////
    // sendInvokeReqEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private void sendInvokeReqEvent(InvokeReqEvent event) throws MandatoryParameterNotSetException {
	int  linkedId   = NONE, invokeId = NONE, dialogueId, classType = NONE, 
		parameterId = NONE, operationType, rc;
	byte [] parameterVal = null, operationCode;
	long timeOut = NONE;


	boolean isLastInvokeEvent = event.isLastInvokeEvent();

	dialogueId     = event.getDialogueId();
	operationCode  = event.getOperation().getOperationCode();
	operationType  = event.getOperation().getOperationType();

	if (event.isTimeOutPresent()) {
		try {
			timeOut        = event.getTimeOut();
		} catch (ParameterNotSetException exc) {};
	}

	if (event.isInvokeIdPresent()) {
		try {
			invokeId   = event.getInvokeId();
		} catch (ParameterNotSetException exc) {};
	}

	if (event.isLinkedIdPresent()) {
		try {
			linkedId   = event.getLinkedId();
		} catch (ParameterNotSetException exc) {};
	}

	if (event.isClassTypePresent()) {
		try {
			classType = event.getClassType();
		} catch (ParameterNotSetException exc) {};
	}

	if (event.isParametersPresent()) {
		try {
			Parameters parameters = event.getParameters();
			parameterId  = parameters.getParameterIdentifier();
			parameterVal = parameters.getParameter();
		} catch (ParameterNotSetException exc) {};
	}

	rc = putInvokeComp(regIdx,
			dialogueId,
			invokeId,
			linkedId,
			classType,
			operationType,
			operationCode,
			timeOut,
			parameterId,
			parameterVal,
			isLastInvokeEvent);

	if (rc != SUCCESS) throw new MandatoryParameterNotSetException("native call fail: " + rc);
    }


    ////////////////////////////////////////////////////////////////////////////////
    // sendResultReqEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private void sendResultReqEvent(ResultReqEvent event) throws MandatoryParameterNotSetException {
	int  linkedId   = NONE, invokeId = NONE, dialogueId, classType = NONE, 
		parameterId = NONE, operationType = NONE, rc;
	byte [] parameterVal = null, operationCode = null;
	long timeOut = NONE;


	boolean isLastResultEvent = event.isLastResultEvent();

	dialogueId     = event.getDialogueId();

	if (event.isOperationPresent()) {
		try {
			operationCode  = event.getOperation().getOperationCode();
			operationType  = event.getOperation().getOperationType();
		} catch (ParameterNotSetException exc) {};
	}

	if (event.isInvokeIdPresent()) {
		try {
			invokeId = event.getInvokeId();
		} catch (ParameterNotSetException exc) {};
	}

	if (event.isLinkIdPresent()) {
		try {
			// LinkID has been deprecated in JainTcapv1.1, InvokeID used for ANSI/ITU in Result
			invokeId = event.getLinkId();
		} catch (ParameterNotSetException exc) {};
	}

	if (event.isParametersPresent()) {
		try {
			Parameters parameters = event.getParameters();
			parameterId  = parameters.getParameterIdentifier();
			parameterVal = parameters.getParameter();
		} catch (ParameterNotSetException exc) {};
	}

	rc = putResultComp(regIdx,
			dialogueId,
			invokeId,
			linkedId,
			operationType,
			operationCode,
			parameterId,
			parameterVal,
			isLastResultEvent);

	if (rc != SUCCESS) throw new MandatoryParameterNotSetException("native call fail: " + rc);
    }


    ////////////////////////////////////////////////////////////////////////////////
    // sendErrorReqEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private void sendErrorReqEvent(ErrorReqEvent event) throws MandatoryParameterNotSetException {
	int  linkedId   = NONE, invokeId = NONE, dialogueId, classType = NONE, 
		parameterId = NONE, errorType = NONE, rc;
	byte [] parameterVal = null, errorCode = null;
	long timeOut = NONE;


	dialogueId = event.getDialogueId();

	errorCode  = event.getErrorCode();
	errorType  = event.getErrorType();

	if (event.isInvokeIdPresent()) {
		try {
			invokeId = event.getInvokeId();
		} catch (ParameterNotSetException exc) {};
	}

	if (event.isLinkIdPresent()) {
		try {
			// LinkID has been deprecated in JainTcapv1.1, InvokeID used for ANSI/ITU in Error
			invokeId = event.getLinkId();
		} catch (ParameterNotSetException exc) {};
	}

	if (event.isParametersPresent()) {
		try {
			Parameters parameters = event.getParameters();
			parameterId  = parameters.getParameterIdentifier();
			parameterVal = parameters.getParameter();
		} catch (ParameterNotSetException exc) {};
	}

	rc = putErrorComp(regIdx, 
			dialogueId, 
			invokeId,
			linkedId, 
			errorType,
			errorCode,
			parameterId, 
			parameterVal);

	if (rc != SUCCESS) throw new MandatoryParameterNotSetException("native call fail: " + rc);
    }


    ////////////////////////////////////////////////////////////////////////////////
    // sendRejectReqEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private void sendRejectReqEvent(RejectReqEvent event) throws MandatoryParameterNotSetException {
	int  invokeId = NONE, dialogueId, problemType = NONE, problem = NONE,
		parameterId = NONE, linkId = NONE, rc;
	byte [] parameterVal = null;


	dialogueId = event.getDialogueId();

	if (event.isInvokeIdPresent()) {
		try {
			invokeId = event.getInvokeId();
		} catch (ParameterNotSetException exc) {};
	}

	if (event.isLinkIdPresent()) {
		try {
			// LinkID has been deprecated in JainTcapv1.1, InvokeID used for ANSI/ITU in Reject
			invokeId = event.getLinkId();
		} catch (ParameterNotSetException exc) {};
	} 

	problemType = event.getProblemType();

	try {
		problem  = event.getProblem();
	} catch (ParameterNotSetException exc) {};

	if (event.isParametersPresent()) {
		try {
			Parameters parameters = event.getParameters();
			parameterId  = parameters.getParameterIdentifier();
			parameterVal = parameters.getParameter();
		} catch (ParameterNotSetException exc) {};
	}

	rc = putRejectComp(regIdx, 
			dialogueId,
			invokeId,
			linkId,
			problemType,
			problem,
			parameterId, 
			parameterVal);

	if (rc != SUCCESS) throw new MandatoryParameterNotSetException("native call fail: " + rc);
    }


    ////////////////////////////////////////////////////////////////////////////////
    // sendUserCancelReqEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private void sendUserCancelReqEvent(UserCancelReqEvent event) throws MandatoryParameterNotSetException {
	int  invokeId = NONE, dialogueId, rc;


	dialogueId = event.getDialogueId();

	try {
		invokeId   = event.getInvokeId();
	} catch (ParameterNotSetException exc) {};

	rc = putUserCancelComp(regIdx, 
			dialogueId,
			invokeId);

	if (rc != SUCCESS) throw new MandatoryParameterNotSetException("native call fail: " + rc);
    }


    ////////////////////////////////////////////////////////////////////////////////
    // sendTimerResetReqEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private void sendTimerResetReqEvent(TimerResetReqEvent event) throws MandatoryParameterNotSetException {
	int  invokeId = NONE, dialogueId, rc;


	dialogueId = event.getDialogueId();

	try {
		invokeId   = event.getInvokeId();
	} catch (ParameterNotSetException exc) {};

	rc = putTimerResetComp(regIdx, 
			dialogueId,
			invokeId);

	if (rc != SUCCESS) throw new MandatoryParameterNotSetException("native call fail: " + rc);
    }


    ////////////////////////////////////////////////////////////////////////////////
    // sendDialoguePortion()
    ////////////////////////////////////////////////////////////////////////////////
    private void sendDialoguePortion(int dialogueId,
				     DialoguePortion dialoguePortion, 
				     int             primitive,
				     int             abortReason) 
	throws MandatoryParameterNotSetException {

	int    rc, protocolVersion = NONE, appContextIdentifier = NONE, securityContextIdentifier = NONE;
	byte[] appContextName  = null, securityContextInfo = null, confidentialityInfo = null,
	    userInformation = null;
	
	if (dialoguePortion.isAppContextNamePresent()) {
	    try {
		appContextName= dialoguePortion.getAppContextName();
	    } catch (ParameterNotSetException exc) {};
	}

	appContextIdentifier= dialoguePortion.getAppContextIdentifier();

	try {
	    securityContextInfo = dialoguePortion.getSecurityContextInfo();
	} catch (ParameterNotSetException exc) {};

	securityContextIdentifier= dialoguePortion.getSecurityContextIdentifier();

	if (dialoguePortion.isConfidentialityInformationPresent()) {
	    try {
		confidentialityInfo = dialoguePortion.getConfidentialityInformation();
	    } catch (ParameterNotSetException exc) {};
	}
	if (dialoguePortion.isUserInformationPresent()) {
	    try {
		userInformation = dialoguePortion.getUserInformation();
	    } catch (ParameterNotSetException exc) {};
	}
	if (dialoguePortion.isProtocolVersionPresent()) {
	    try {
		protocolVersion = dialoguePortion.getProtocolVersion();
	    } catch (ParameterNotSetException exc) {};
	}

	
	if (abortReason != DialogueConstants.ABORT_REASON_ACN_NOT_SUPPORTED && 
	    abortReason != DialogueConstants.ABORT_REASON_USER_SPECIFIC)
		
		abortReason = DialogueConstants.ABORT_REASON_USER_SPECIFIC;

	rc = putDialoguePortion(regIdx,
				dialogueId,
				userInformation,
				confidentialityInfo,
				appContextIdentifier,
				appContextName,
				securityContextIdentifier,
				securityContextInfo,
				protocolVersion,
				primitive,
				abortReason);


	if (rc != SUCCESS) throw new MandatoryParameterNotSetException("native call fail: " + rc);
    }


    ////////////////////////////////////////////////////////////////////////////////
    // sendBeginReqEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private void sendBeginReqEvent(BeginReqEvent event) throws MandatoryParameterNotSetException {
	int  qualityOfService  = 0, dialogueId, rc;

	
	boolean allowedPermission = true;
	SccpUserAddress destinationAddress, originatingAddress;

	dialogueId = event.getDialogueId();
	
	if (event.isQualityOfServicePresent()) {
	    try {
		qualityOfService = event.getQualityOfService();
	    } catch (ParameterNotSetException exc) {};
	}

	if (event.isAllowedPermissionPresent()) {
	    try {
		allowedPermission = event.isAllowedPermission();
	    } catch (ParameterNotSetException exc) {};
	};

	destinationAddress = event.getDestinationAddress();
	originatingAddress = event.getOriginatingAddress();

	rc = putBeginReqEvent(regIdx,
			      dialogueId,
			      qualityOfService & BYTE2INT_MASK,
			      event.isDialoguePortionPresent(),
			      allowedPermission,
			      AddressConverter.getRouteIndicator(destinationAddress),
			      AddressConverter.getNationalUse(destinationAddress),
			      AddressConverter.getZone(destinationAddress),
			      AddressConverter.getCluster(destinationAddress),
			      AddressConverter.getMember(destinationAddress),
			      AddressConverter.getSSN(destinationAddress),
			      AddressConverter.getGTIndicator(destinationAddress),
			      AddressConverter.getAddressInfo(destinationAddress),
			      AddressConverter.getEncodingScheme(destinationAddress),
			      AddressConverter.getNatureOfAddr(destinationAddress),
			      AddressConverter.getNumPlan(destinationAddress),
			      AddressConverter.getTranslationType(destinationAddress),
			      AddressConverter.getOddIndicator(destinationAddress),

			      AddressConverter.getRouteIndicator(originatingAddress),
			      AddressConverter.getNationalUse(originatingAddress),
			      AddressConverter.getZone(originatingAddress),
			      AddressConverter.getCluster(originatingAddress),
			      AddressConverter.getMember(originatingAddress),
			      AddressConverter.getSSN(originatingAddress),
			      AddressConverter.getGTIndicator(originatingAddress),
			      AddressConverter.getAddressInfo(originatingAddress),
			      AddressConverter.getEncodingScheme(originatingAddress),
			      AddressConverter.getNatureOfAddr(originatingAddress),
			      AddressConverter.getNumPlan(originatingAddress),
			      AddressConverter.getTranslationType(originatingAddress),
			      AddressConverter.getOddIndicator(originatingAddress)
			      );
	if (rc != SUCCESS) throw new MandatoryParameterNotSetException("native call fail: " + rc);
    }


    ////////////////////////////////////////////////////////////////////////////////
    // sendEndReqEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private void sendEndReqEvent(EndReqEvent event) throws MandatoryParameterNotSetException {
	int  qualityOfService  = 0, dialogueId, termination = NONE, rc;


	SccpUserAddress destinationAddress, originatingAddress;
	
	dialogueId = event.getDialogueId();
	
	if (event.isQualityOfServicePresent()) {
	    try {
		qualityOfService = event.getQualityOfService();
	    } catch (ParameterNotSetException exc) {};
	}
	
	if (event.isTerminationPresent()) {
	    try {
		termination = event.getTermination();
	    } catch (ParameterNotSetException exc) {};
	}
	
	rc = putEndReqEvent(regIdx,
			    dialogueId,
			    qualityOfService & BYTE2INT_MASK,
			    event.isDialoguePortionPresent(),
			    termination);

	if (rc != SUCCESS) throw new MandatoryParameterNotSetException("native call fail: " + rc);
    }


    ////////////////////////////////////////////////////////////////////////////////
    // sendContinueReqEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private void sendContinueReqEvent(ContinueReqEvent event) throws MandatoryParameterNotSetException {
	int  qualityOfService  = 0, dialogueId, rc;


	boolean allowedPermission = true;
	SccpUserAddress originatingAddress = null;

	dialogueId = event.getDialogueId();
	
	if (event.isQualityOfServicePresent()) {
	    try {
		qualityOfService = event.getQualityOfService();
	    } catch (ParameterNotSetException exc) {};
	}

	if (event.isAllowedPermissionPresent()) {
	    try {
		allowedPermission = event.isAllowedPermission();

	    } catch (ParameterNotSetException exc) {};
	};
	
	if (!event.isOriginatingAddressPresent()) {
	    rc = putContinueReqEvent(regIdx,
				     dialogueId,
				     qualityOfService & BYTE2INT_MASK,
				     event.isDialoguePortionPresent(),
				     allowedPermission
				     );
	} else {
	    /* request to change originating address, send continue confirm */
	    try {
		originatingAddress = event.getOriginatingAddress();
	    } catch (ParameterNotSetException exc) {};

	    rc = putContinueConfirmReqEvent(regIdx,
					    dialogueId,
					    qualityOfService & BYTE2INT_MASK,
					    event.isDialoguePortionPresent(),
					    allowedPermission,
					    AddressConverter.getRouteIndicator(originatingAddress),
					    AddressConverter.getNationalUse(originatingAddress),
					    AddressConverter.getZone(originatingAddress),
					    AddressConverter.getCluster(originatingAddress),
					    AddressConverter.getMember(originatingAddress),
					    AddressConverter.getSSN(originatingAddress),
					    AddressConverter.getGTIndicator(originatingAddress),
					    AddressConverter.getAddressInfo(originatingAddress),
					    AddressConverter.getEncodingScheme(originatingAddress),
					    AddressConverter.getNatureOfAddr(originatingAddress),
					    AddressConverter.getNumPlan(originatingAddress),
					    AddressConverter.getTranslationType(originatingAddress),
					    AddressConverter.getOddIndicator(originatingAddress)
					    );
	    
	}

	if (rc != SUCCESS) throw new MandatoryParameterNotSetException("native call fail: " + rc);
    }


    ////////////////////////////////////////////////////////////////////////////////
    // sendUnidirectionalReqEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private void sendUnidirectionalReqEvent(UnidirectionalReqEvent event) throws MandatoryParameterNotSetException {
	int  qualityOfService  = 0, dialogueId, rc;


	boolean allowedPermission = true;
	SccpUserAddress destinationAddress, originatingAddress;

	// DialogueId is really not allowed in TCAP UnidirectReq,
	// but the JAIN version inherits it from DialogReqEvent
	dialogueId = event.isDialogueIdPresent() ? event.getDialogueId() : 0;

	if (event.isQualityOfServicePresent()) {
		try {
			qualityOfService = event.getQualityOfService();
		} catch (ParameterNotSetException exc) {};
	}

	destinationAddress = event.getDestinationAddress();
	originatingAddress = event.getOriginatingAddress();

	rc = putUnidirectionalReqEvent(regIdx,
					dialogueId,
					qualityOfService & BYTE2INT_MASK,
					event.isDialoguePortionPresent(),

					AddressConverter.getRouteIndicator(destinationAddress),
					AddressConverter.getNationalUse(destinationAddress),
					AddressConverter.getZone(destinationAddress),
					AddressConverter.getCluster(destinationAddress),
					AddressConverter.getMember(destinationAddress),
					AddressConverter.getSSN(destinationAddress),
					AddressConverter.getGTIndicator(destinationAddress),
					AddressConverter.getAddressInfo(destinationAddress),
					AddressConverter.getEncodingScheme(destinationAddress),
					AddressConverter.getNatureOfAddr(destinationAddress),
					AddressConverter.getNumPlan(destinationAddress),
					AddressConverter.getTranslationType(destinationAddress),
					AddressConverter.getOddIndicator(destinationAddress),

					AddressConverter.getRouteIndicator(originatingAddress),
					AddressConverter.getNationalUse(originatingAddress),
					AddressConverter.getZone(originatingAddress),
					AddressConverter.getCluster(originatingAddress),
					AddressConverter.getMember(originatingAddress),
					AddressConverter.getSSN(originatingAddress),
					AddressConverter.getGTIndicator(originatingAddress),
					AddressConverter.getAddressInfo(originatingAddress),
					AddressConverter.getEncodingScheme(originatingAddress),
					AddressConverter.getNatureOfAddr(originatingAddress),
					AddressConverter.getNumPlan(originatingAddress),
					AddressConverter.getTranslationType(originatingAddress),
					AddressConverter.getOddIndicator(originatingAddress)
					);

	if (rc != SUCCESS) throw new MandatoryParameterNotSetException("native call fail: " + rc);
    }


    ////////////////////////////////////////////////////////////////////////////////
    // sendUserAbortReqEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private void sendUserAbortReqEvent(UserAbortReqEvent event) throws MandatoryParameterNotSetException {
	int  qualityOfService  = 0, dialogueId, abortReason = NONE, rc;
	byte[] userAbortInformation = null;



	SccpUserAddress destinationAddress, originatingAddress;

	dialogueId = event.getDialogueId();

	if (event.isQualityOfServicePresent()) {
		try {
			qualityOfService = event.getQualityOfService();
		} catch (ParameterNotSetException exc) {};
	}

	if (event.isAbortReasonPresent()) {
		try {
			abortReason = event.getAbortReason();
		} catch (ParameterNotSetException exc) {};
	}

	if (event.isUserAbortInformationPresent()) {
		try {
			userAbortInformation = event.getUserAbortInformation();
		} catch (ParameterNotSetException exc) {};
	}

	rc = putUserAbortReqEvent(regIdx,
				dialogueId,
				qualityOfService & BYTE2INT_MASK,
				event.isDialoguePortionPresent(),
				abortReason,
				userAbortInformation);

	if (rc != SUCCESS) throw new MandatoryParameterNotSetException("native call fail: " + rc);
    }


    ////////////////////////////////////////////////////////////////////////////////
    // constructDialoguePortion()
    ////////////////////////////////////////////////////////////////////////////////
    private DialoguePortion constructDialoguePortion(byte[]  userInformation,
						byte[]  confidentialityInfo,
						int     appContextIdentifier,
						byte[]  appContextName,
						int     securityContextIdentifier,
						byte[]  securityContextInfo,
						int     protocolVersion,
						int     primitive) {
	// Only static utility method, keep non-static for simplified jni method loading

	DialoguePortion dialoguePortion = new DialoguePortion();

	if (userInformation != null)
		dialoguePortion.setUserInformation(userInformation);

	if (confidentialityInfo != null)
		dialoguePortion.setConfidentialityInformation(confidentialityInfo);

	if (appContextName != null)
		dialoguePortion.setAppContextName(appContextName);

	if (appContextIdentifier != NONE)
		dialoguePortion.setAppContextIdentifier(appContextIdentifier);

	if (securityContextIdentifier != NONE)
		dialoguePortion.setSecurityContextIdentifier(securityContextIdentifier);

	if (securityContextInfo != null) 
		dialoguePortion.setSecurityContextInfo(securityContextInfo);

	if (protocolVersion != NONE)
		dialoguePortion.setProtocolVersion(protocolVersion);

	return dialoguePortion;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // getUnidirectionalIndEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private int getUnidirectionalIndEvent(int           dialogueId,
					  int           qualityOfService,
					  DialoguePortion dialoguePortion,
					  boolean       componentsPresent,
					  int           destRouteIndicator, // xlate
					  boolean       destNationalUse,    // xlate
					  int    	destZone,
					  int    	destCluster,
					  int    	destMember,
					  int    	destSSN,
					  int    	destGTIndicator,    // xlate ?
					  byte[] 	destAddressInfo,
					  int    	destEncodingScheme, // xlate
					  int    	destNatureOfAddr,
					  int    	destNumPlan,
					  int    	destTranslationType,
					  boolean       destOddIndicator,
					  
					  int           origRouteIndicator, // xlate
					  boolean       origNationalUse,    // xlate
					  int    	origZone,
					  int    	origCluster,
					  int    	origMember,
					  int    	origSSN,
					  int    	origGTIndicator,    // xlate ?
					  byte[] 	origAddressInfo,
					  int    	origEncodingScheme, // xlate
					  int    	origNatureOfAddr,
					  int    	origNumPlan,
					  int    	origTranslationType,
					  boolean       origOddIndicator
	) {
	int rc = FAIL;
	
	if (listener != null) {
	    SccpUserAddress destinationAddress, originatingAddress;
	    
	    destinationAddress = AddressConverter.constructAdress(destRouteIndicator,
								  destNationalUse,
								  destZone,
								  destCluster,
								  destMember,
								  destSSN,
								  destGTIndicator,
								  destAddressInfo,
								  destEncodingScheme,
								  destNatureOfAddr,
								  destNumPlan,
								  destTranslationType,
								  destOddIndicator
								  );

	    originatingAddress = AddressConverter.constructAdress(origRouteIndicator,
								  origNationalUse,
								  origZone,
								  origCluster,
								  origMember,
								  origSSN,
								  origGTIndicator,
								  origAddressInfo,
								  origEncodingScheme,
								  origNatureOfAddr,
								  origNumPlan,
								  origTranslationType,
								  origOddIndicator
								  );

	    UnidirectionalIndEvent event = new UnidirectionalIndEvent(this,
								      originatingAddress,
								      destinationAddress,
								      componentsPresent);

	    event.setQualityOfService((byte) qualityOfService);

	    if (dialoguePortion != null) {
		event.setDialoguePortion(dialoguePortion);
	    }
	
	    event.setDialogueId(dialogueId);

	    /* if listener is null an exception is automatically thrown */

	    listener.processDialogueIndEvent(event);


	    rc = SUCCESS;
	}

	return rc;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // getBeginIndEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private  int getBeginIndEvent(int          dialogueId,
				  int          qualityOfService,
				  DialoguePortion dialoguePortion,
				  boolean      allowedPermission,
				  boolean      componentsPresent,
				  
				  int           destRouteIndicator, // xlate
				  boolean       destNationalUse,    // xlate
				  int    	destZone,
				  int    	destCluster,
				  int    	destMember,
				  int    	destSSN,
				  int    	destGTIndicator,    // xlate ?
				  byte[] 	destAddressInfo,
				  int    	destEncodingScheme, // xlate
				  int    	destNatureOfAddr,
				  int    	destNumPlan,
				  int    	destTranslationType,
				  boolean       destOddIndicator,
				  
				  int           origRouteIndicator, // xlate
				  boolean       origNationalUse,    // xlate
				  int    	origZone,
				  int    	origCluster,
				  int    	origMember,
				  int    	origSSN,
				  int    	origGTIndicator,    // xlate ?
				  byte[] 	origAddressInfo,
				  int    	origEncodingScheme, // xlate
				  int    	origNatureOfAddr,
				  int    	origNumPlan,
				  int    	origTranslationType,
				  boolean       origOddIndicator
				  ) {

	int rc = FAIL;
	
	if (listener != null) {
	    SccpUserAddress destinationAddress, originatingAddress;
	    
	    destinationAddress = AddressConverter.constructAdress(destRouteIndicator,
								  destNationalUse,
								  destZone,
								  destCluster,
								  destMember,
								  destSSN,
								  destGTIndicator,
								  destAddressInfo,
								  destEncodingScheme,
								  destNatureOfAddr,
								  destNumPlan,
								  destTranslationType,
								  destOddIndicator
								  );

	    originatingAddress = AddressConverter.constructAdress(origRouteIndicator,
								  origNationalUse,
								  origZone,
								  origCluster,
								  origMember,
								  origSSN,
								  origGTIndicator,
								  origAddressInfo,
								  origEncodingScheme,
								  origNatureOfAddr,
								  origNumPlan,
								  origTranslationType,
								  origOddIndicator
								  );

	    BeginIndEvent event = new BeginIndEvent(this,
						    dialogueId,
						    originatingAddress,
						    destinationAddress,
						    componentsPresent);
	    
	    event.setQualityOfService((byte) qualityOfService);

	    event.setAllowedPermission(allowedPermission);

	    if (dialoguePortion != null) {
		event.setDialoguePortion(dialoguePortion);
	    }

	    /* if listener is null an exception is automatically thrown */

	    listener.processDialogueIndEvent(event);


	    rc = SUCCESS;
	}

	return rc;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // getEndIndEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private  int getEndIndEvent(int          dialogueId,
				int          qualityOfService,
				DialoguePortion dialoguePortion,
				boolean      componentsPresent) {
	
	int rc = FAIL;
	
	if (listener != null) {

	    EndIndEvent event = new EndIndEvent(this,
						dialogueId,
						componentsPresent);
	    
	    event.setQualityOfService((byte) qualityOfService);

	    if (dialoguePortion != null) {
		event.setDialoguePortion(dialoguePortion);
	    }
	
	    /* if listener is null an exception is automatically thrown */

	    listener.processDialogueIndEvent(event);


	    rc = SUCCESS;
	}

	return rc;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // getContinueIndEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private  int getContinueIndEvent(int     dialogueId,
				     int     qualityOfService,
				     DialoguePortion dialoguePortion,
				     boolean allowedPermission,  
				     boolean componentsPresent) {
	
	int rc = FAIL;
	
	if (listener != null) {

	    ContinueIndEvent event = new ContinueIndEvent(this,
							  dialogueId,
							  componentsPresent);
	    
	    event.setQualityOfService((byte) qualityOfService);
	    event.setAllowedPermission(allowedPermission);

	    if (dialoguePortion != null) {
		event.setDialoguePortion(dialoguePortion);
	    }
	    
	    /* if listener is null an exception is automatically thrown */
	    
	    listener.processDialogueIndEvent(event);
	   
 
	    rc = SUCCESS;
	}

	return rc;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // getNoticeIndEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private  int getNoticeIndEvent(int     dialogueId,
				   byte    returnReason,
				   				  
				   int           destRouteIndicator, // xlate
				   boolean       destNationalUse,    // xlate
				   int    	destZone,
				   int    	destCluster,
				   int    	destMember,
				   int    	destSSN,
				   int    	destGTIndicator,    // xlate ?
				   byte[] 	destAddressInfo,
				   int    	destEncodingScheme, // xlate
				   int    	destNatureOfAddr,
				   int    	destNumPlan,
				   int    	destTranslationType,
				   boolean       destOddIndicator,
				   
				   int           origRouteIndicator, // xlate
				   boolean       origNationalUse,    // xlate
				   int    	origZone,
				   int    	origCluster,
				   int    	origMember,
				   int    	origSSN,
				   int    	origGTIndicator,    // xlate ?
				   byte[] 	origAddressInfo,
				   int    	origEncodingScheme, // xlate
				   int    	origNatureOfAddr,
				   int    	origNumPlan,
				   int    	origTranslationType,
				   boolean       origOddIndicator
				   ) {
	
	int rc = FAIL;
	
	if (listener != null) {
	    SccpUserAddress destinationAddress, originatingAddress;
	    
	    destinationAddress = AddressConverter.constructAdress(destRouteIndicator,
								  destNationalUse,
								  destZone,
								  destCluster,
								  destMember,
								  destSSN,
								  destGTIndicator,
								  destAddressInfo,
								  destEncodingScheme,
								  destNatureOfAddr,
								  destNumPlan,
								  destTranslationType,
								  destOddIndicator
								  );

	    originatingAddress = AddressConverter.constructAdress(origRouteIndicator,
								  origNationalUse,
								  origZone,
								  origCluster,
								  origMember,
								  origSSN,
								  origGTIndicator,
								  origAddressInfo,
								  origEncodingScheme,
								  origNatureOfAddr,
								  origNumPlan,
								  origTranslationType,
								  origOddIndicator
								  );

	    byte[] returnCause = {returnReason};

	    NoticeIndEvent event = new NoticeIndEvent(this,
						      dialogueId,
						      returnCause);
	    
	    event.setDestinationAddress(destinationAddress);
	    event.setOriginatingAddress(originatingAddress);

	    /* if listener is null an exception is automatically thrown */
	    
	    listener.processDialogueIndEvent(event);
	   
 
	    rc = SUCCESS;
	}

	return rc;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // getUserAbortIndEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private  int getUserAbortIndEvent(int     dialogueId,
				      int     qualityOfService,
				      DialoguePortion dialoguePortion,
				      int     abortReason,  
				      byte[]  userAbortInformation) {
	
	int rc = FAIL;
	
	if (listener != null) {

	    UserAbortIndEvent event = new UserAbortIndEvent(this,
							    dialogueId);
	    
	    event.setQualityOfService((byte) qualityOfService);

	    if (dialoguePortion != null) {
		event.setDialoguePortion(dialoguePortion);
	    }
	    
	    if (abortReason != NONE) {
		event.setAbortReason(abortReason);
	    } 
	    
	    if (userAbortInformation != null) {
		event.setUserAbortInformation(userAbortInformation);
	    } 
	    /* if listener is null an exception is automatically thrown */
	    
	    listener.processDialogueIndEvent(event);
	   
 
	    rc = SUCCESS;
	}

	return rc;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // getProviderAbortIndEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private  int getProviderAbortIndEvent(int     dialogueId,
					  int     qualityOfService,
					  DialoguePortion dialoguePortion,
					  int     PAbort) {
	
	int rc = FAIL;
	
	if (listener != null) {
	    
	    ProviderAbortIndEvent event = new ProviderAbortIndEvent(this,
								    dialogueId, PAbort);
	    
	    event.setQualityOfService((byte) qualityOfService);
	    
	    if (dialoguePortion != null) {
		event.setDialoguePortion(dialoguePortion);
	    }
	    
	    /* if listener is null an exception is automatically thrown */
	    
	    listener.processDialogueIndEvent(event);
	   
 
	    rc = SUCCESS;
	}

	return rc;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // getInvokeIndEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private  int getInvokeIndEvent(int     dialogueId, 
				   int     invokeId, 
				   int     linkedId, 
				   int     operationType,
				   byte[]  operationCode,
				   int     parameterId, 
				   byte[]  parameterVal, 
				   boolean isLastInvokeEvent,
				   boolean isLastComponent) {
	int rc = FAIL;
	
	if (listener != null) {
	    Operation operation = new Operation(operationType, operationCode);

	    InvokeIndEvent event = new InvokeIndEvent(this, 
						      operation, 
						      isLastComponent);


	    event.setDialogueId(dialogueId);

	    if (invokeId != NONE) 
		event.setInvokeId(invokeId);
	    
	    if (linkedId != NONE)
		event.setLinkedId(linkedId);

	    if (parameterId != NONE && parameterVal != null) 
		event.setParameters(new Parameters(parameterId, parameterVal));

	    event.setLastInvokeEvent(isLastInvokeEvent);
	    
	    listener.processComponentIndEvent(event);


	    rc = SUCCESS;
	}

	return rc;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // getResultIndEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private  int getResultIndEvent(int     dialogueId, 
				   int     invokeId, 
				   int     linkedId, 
				   int     operationType,
				   byte[]  operationCode,
				   int     parameterId, 
				   byte[]  parameterVal, 
				   boolean isLastResultEvent,
				   boolean isLastComponent) {
	int rc = FAIL;
	
	if (listener != null) {
	    ResultIndEvent event = new ResultIndEvent(this, 
						      dialogueId, 
						      isLastComponent,
						      isLastResultEvent);

	    if (operationType != NONE) {
		Operation operation = new Operation(operationType, operationCode);

		event.setOperation(operation);
	    }

	    if (invokeId != NONE) 
		event.setInvokeId(invokeId);

	    if (linkedId != NONE)
		event.setLinkId(linkedId);
	    
	    if (parameterId != NONE && parameterVal != null) 
		event.setParameters(new Parameters(parameterId, parameterVal));

	    event.setLastResultEvent(isLastResultEvent);

	    listener.processComponentIndEvent(event);


	    rc = SUCCESS;
	}

	return rc;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // getErrorIndEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private int getErrorIndEvent(int       dialogueId, 
				 int       invokeId, 
				 int       linkedId, 
				 int       errorType,
				 byte[]    errorCode,
				 int       parameterId, 
				 byte[]    parameterVal,
				 boolean   isLastComponent) {
	
	int rc = FAIL;
	
	if (listener != null) {
	    ErrorIndEvent event = new ErrorIndEvent(this, 
						    dialogueId, 
						    errorType,
						    errorCode,
						    isLastComponent);
	    if (invokeId != NONE) 
		event.setInvokeId(invokeId);
	    
	    if (linkedId != NONE)
		event.setLinkId(linkedId);

	    if (parameterId != NONE && parameterVal != null) 
		event.setParameters(new Parameters(parameterId, parameterVal));

	    listener.processComponentIndEvent(event);


	    rc = SUCCESS;
	}

	return rc;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // getRejectIndEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private  int getRejectIndEvent(int     dialogueId, 
				   int     invokeId, 
				   int     linkedId, 
				   int     problemType,
				   int     problem,
				   int     rejectType,
				   int     parameterId, 
				   byte[]  parameterVal,
				   boolean isLastComponent) {
	int rc = FAIL;
	
	if (listener != null) {
	    RejectIndEvent event = new RejectIndEvent(this, 
						      dialogueId,
						      problemType,
						      problem,
						      isLastComponent);

	    event.setRejectType(rejectType);

	    if (invokeId != NONE) 
		event.setInvokeId(invokeId);
	    
	    if (linkedId != NONE)
		event.setLinkId(linkedId);

	    if (parameterId != NONE && parameterVal != null) {
		event.setParameters(new Parameters(parameterId, parameterVal));
	    }

	    listener.processComponentIndEvent(event);


	    rc = SUCCESS;
	}

	return rc;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // getLocalCancelIndEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private  int getLocalCancelIndEvent(int   dialogueId, 
					int        invokeId, 
					boolean    isLastComponent) {
	int rc = FAIL;
	
	if (listener != null) {
	    LocalCancelIndEvent event = new LocalCancelIndEvent(this, 
								dialogueId,
								invokeId);
	    
	    event.setLastComponent(isLastComponent);
	
	
	    listener.processComponentIndEvent(event);


	    rc = SUCCESS;
	}

	return rc;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // sendDeregistrationEvent()
    ////////////////////////////////////////////////////////////////////////////////
    private void sendDeregistrationEvent(int ssn) {
	// ssn not used since there is only one registration per provider

	cleanup();
    }


    ////////////////////////////////////////////////////////////////////////////////
    // finalize()
    ////////////////////////////////////////////////////////////////////////////////
    protected void finalize() throws Throwable {
    }


    ////////////////////////////////////////////////////////////////////////////////
    // OTHER METHODS
    ////////////////////////////////////////////////////////////////////////////////
    public JainTcapStack getStack() {

        return ownStack;
    }
    public void setSizeOfBuffers(int newBufSize) {
        bufSize = newBufSize;
    }
    public int getSizeOfBuffers() {
        return bufSize;
    }
    public void setNumberOfBuffers(int newNumBufs) {
        numBufs = newNumBufs;
    }
    public int getNumberOfBuffers() {
        return numBufs;
    }
    public void setNumberOfDialogues(int newNumDlgs) {
        numDlgs = newNumDlgs;
    }
    public int getNumberOfDialogues() {
        return numDlgs;
    }

    public void setListenerBusy(boolean busy) throws ListenerNotRegisteredException {
	
	int dlgId;
	
	if (listener == null) { // This provider not registered yet, throw the only available exception
		throw new ListenerNotRegisteredException ("No listener registered yet");
	}

	setListenerBusy(regIdx, busy);

	return;
    }

    // Unconditionally dump from the TCAP queues all messages older than
    // the timestamp as specified in seconds and microseconds
    public void auditQueues(int seconds){
       	tcapPurge(this.regIdx, seconds);
    }

    protected void setStack(JainTcapStackImpl tcapStack) {
        ownStack = tcapStack;
    }
    private void dummy() {
    }


    ////////////////////////////////////////////////////////////////////////////////
    // NATIVE INTERFACES
    ////////////////////////////////////////////////////////////////////////////////

    private static native int initTcapProvider ();

    private synchronized native int tcapSccpReg(int sp, 
						int ssn, 
						int stackSpec, 
						int bufSize, 
						int numBufs,
						int numDlgs);
    
    private synchronized native int tcapDereg(int regIdx);
    
    private native int getDlgId (int regIdx);
    
    private native int rlsDlgId (int regIdx, int dialogueId);
    
    private native int putInvokeComp(int     regIdx, 
				     int     dialogueId, 
				     int     invokeId, 
				     int     linkedId, 
				     int     classType, 
				     int     operationType,
				     byte[]  operationCode,
				     long    timeOut, 
				     int     paramaterId, 
				     byte[]  parameterVal, 
				     boolean isLastInvokeEvent);

    private native int putResultComp(int     regIdx,
				     int     dialogueId,
				     int     invokeId,
				     int     linkedId,
				     int     operationType,
				     byte[]  operationCode,
				     int     paramaterId,
				     byte[]  parameterVal,
				     boolean isLastResultEvent);

    private native int putErrorComp(int     regIdx, 
				    int     dialogueId, 
				    int     invokeId, 
				    int     linkedId, 
				    int     errorType,
				    byte[]  errorCode,
				    int     paramaterId, 
				    byte[]  parameterVal);

    private native int putRejectComp(int     regIdx, 
				     int     dialogueId, 
				     int     invokeId, 
				     int     linkId, 
				     int     problemType,
				     int     problem,
				     int     paramaterId, 
				     byte[]  parameterVal);

    private native int putUserCancelComp(int     regIdx, 
					 int     dialogueId, 
					 int     invokeId);
    
    private native int putTimerResetComp(int     regIdx, 
					 int     dialogueId, 
					 int     invokeId);
    

    private native int putDialoguePortion(int     regIdx, 
					  int     dialogueId,
					  byte[]  userInformation,
					  byte[]  confidentialityInfo,
					  int     appContextIdentifier,
					  byte[]  appContextName,
					  int     securityContextIdentifier,
					  byte[]  securityContextInfo,
					  int     protocolVersion,
					  int     primitive,
					  int     abortReason);
			   
    private native int putBeginReqEvent(int     regIdx,
					int     dialogueId,
					int     qualityOfService,
					boolean dialoguePortionPresent,
					boolean allowedPermission,

					int     destRouteIndicator, // xlate
					boolean destNationalUse,    // xlate
					int    	destZone,
					int    	destCluster,
					int    	destMember,
					int    	destSSN,
					int    	destGTIndicator,    // xlate ?
					byte[] 	destAddressInfo,
					int    	destEncodingScheme, // xlate
					int    	destNatureOfAddr,
					int    	destNumPlan,
					int    	destTranslationType,
					boolean destOddIndicator,
					
					int     origRouteIndicator, // xlate
					boolean origNationalUse,    // xlate
					int    	origZone,
					int    	origCluster,
					int    	origMember,
					int    	origSSN,
					int    	origGTIndicator,    // xlate ?
					byte[] 	origAddressInfo,
					int    	origEncodingScheme, // xlate
					int    	origNatureOfAddr,
					int    	origNumPlan,
					int    	origTranslationType,
					boolean origOddIndicator
					);

    private native int putEndReqEvent(int     regIdx,
				      int     dialogueId,
				      int     qualityOfService,
				      boolean dialoguePortionPresent,
				      int     termination
				      );

    private native int putContinueReqEvent(int     regIdx,
					   int     dialogueId,
					   int     qualityOfService,
					   boolean dialoguePortionPresent,
					   boolean allowedPermission
					   );
					   

    private native int putContinueConfirmReqEvent(int     regIdx,
						  int     dialogueId,
						  int     qualityOfService,
						  boolean dialoguePortionPresent,
						  boolean allowedPermission,
						  int     origRouteIndicator, // xlate
						  boolean origNationalUse,    // xlate
						  int    	origZone,
						  int    	origCluster,
						  int    	origMember,
						  int    	origSSN,
						  int    	origGTIndicator,    // xlate ?
						  byte[] 	origAddressInfo,
						  int    	origEncodingScheme, // xlate
						  int    	origNatureOfAddr,
						  int    	origNumPlan,
						  int    	origTranslationType,
						  boolean origOddIndicator
						  );
    

    private native int putUnidirectionalReqEvent(int     regIdx,
						 int     dialogueId,
						 int     qualityOfService,
						 boolean dialoguePortionPresent,

						 int     destRouteIndicator, // xlate
						 boolean destNationalUse,    // xlate
						 int    	destZone,
						 int    	destCluster,
						 int    	destMember,
						 int    	destSSN,
						 int    	destGTIndicator,    // xlate ?
						 byte[] 	destAddressInfo,
						 int    	destEncodingScheme, // xlate
						 int    	destNatureOfAddr,
						 int    	destNumPlan,
						 int    	destTranslationType,
						 boolean destOddIndicator,
						 
						 int     origRouteIndicator, // xlate
						 boolean origNationalUse,    // xlate
						 int    	origZone,
						 int    	origCluster,
						 int    	origMember,
						 int    	origSSN,
						 int    	origGTIndicator,    // xlate ?
						 byte[] 	origAddressInfo,
						 int    	origEncodingScheme, // xlate
						 int    	origNatureOfAddr,
						 int    	origNumPlan,
						 int    	origTranslationType,
						 boolean origOddIndicator
						 );

    private native int putUserAbortReqEvent (int     regIdx,
					     int     dialogueId,
					     int     qualityOfService,
					     boolean dialoguePortionPresent,
					     int     abortReason,
					     byte[]  userAbortInformation
					     );

    

    private native void setListenerBusy (int regIdx, boolean busy);

    private native void tcapPurge(int regIdx, int seconds);

    ////////////////////////////////////////////////////////////////////////////////
    // addJainTcapListener( TcapUserAddress ) DEPRECATED
    ////////////////////////////////////////////////////////////////////////////////
    public void addJainTcapListener(JainTcapListener listener, TcapUserAddress userAddress)
        throws java.util.TooManyListenersException, ListenerAlreadyRegisteredException,
                InvalidUserAddressException {

        throw new InvalidUserAddressException("use class SccpUserAddress not TcapUserAddress\n");
    }


    ////////////////////////////////////////////////////////////////////////////////
    // getAttachedStack() DEPRECATED
    ////////////////////////////////////////////////////////////////////////////////
    public JainTcapStack getAttachedStack() {

        return ownStack;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // isAttached() DEPRECATED
    ////////////////////////////////////////////////////////////////////////////////
    public boolean isAttached() {

        return true;
    }
}
