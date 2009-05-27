
package com.ss8.jain.protocol.ss7.tcap;

import java.util.*;
import java.lang.*;
import java.io.*;

import jain.*;
import jain.protocol.ss7.*;
import jain.protocol.ss7.tcap.*;
import jain.protocol.ss7.tcap.component.*;
import jain.protocol.ss7.tcap.dialogue.*;

import com.ss8.jain.protocol.ss7.tcap.*;


////////////////////////////////////////////////////////////////////////////////////
//  CLASS JainSampleTcapListener
////////////////////////////////////////////////////////////////////////////////////
public class JainSampleTcapListener implements JainTcapListener {

    private com.ss8.jain.protocol.ss7.tcap.JainTcapStackImpl    d7TcapStack    = null;
    private com.ss8.jain.protocol.ss7.tcap.JainTcapProviderImpl d7TcapProvider = null;
    private SccpUserAddress  userAddress          = null;
    private SccpUserAddress  originationAddress  = null;
    private SccpUserAddress  destinationAddress   = null;
    private short defaultSSN     = 0;
    private int[] defaultPC      = {0, 0, 0};
    private short defaultDestSSN = 0;
    private int[] defaultDestPC  = {0, 0, 0};
    private static final int DEFAULT_BUF_SIZE = 100;
    private static final int DEFAULT_NUM_BUFS = 3000;
    private static final int DEFAULT_NUM_DLGS = 16384;
    private static final int CONN_ID_RM       = 8;
    private static Hashtable ListenerTbl = new Hashtable();
    private static int TblCnt      = 0;
    private static boolean prntOpt = false;

    public static int burst       = 0;
    public static int msgcntresp  = 0;
    public static long START_TIME = 0;
    public static long STOP_TIME  = 0;

    public static int statUnidirectRcvd = 0;
    public static int statUnidirectSent = 0;
    public static int statDialoguesSent = 0;
    public static int statDialoguesRcvd = 0;
    public static int statResponsesSent = 0;
    public static int statResponsesRcvd = 0;
    public static int statTimeoutsRcvd  = 0;

    public static int responseType             = 1;  // 0 : none 1:end 2:continue 3:continue with cgpa
    public static final int INVOKE_TIMER_1     = 10000;
    public static final byte QOS_SCCP_CLTS_NS_NR = (byte) 0;
    public static final byte QOS_SCCP_CLTS_NS_RE = (byte) 128;
    public static final byte QOS_SCCP_CLTS_S_NR_NL = (byte) 1;
    public static final byte QOS_SCCP_CLTS_S_RE_NL = (byte) 129;
    public static JainSS7Factory d7TcapFactory = JainSS7Factory.getInstance();

    BufferedReader bufr = new BufferedReader(new InputStreamReader(System.in));

    ////////////////////////////////////////////////////////////////////////////////
    // JainSampleTcapListener()
    ////////////////////////////////////////////////////////////////////////////////
    public JainSampleTcapListener() {

	try {
		d7TcapFactory.setPathName("com.ss8");
	} catch (Exception exception) {
		exception.printStackTrace(System.out);
	}

    }


    ////////////////////////////////////////////////////////////////////////////////
    // JainSampleTcapListener()
    ////////////////////////////////////////////////////////////////////////////////
    public JainTcapProviderImpl getD7TcapProvider() {
	return d7TcapProvider;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // register()
    ////////////////////////////////////////////////////////////////////////////////
    public void register() {
	Vector vect;
	int[] spcToMatch;
	boolean spcFound = false;

	initializeSettings();

	vect = d7TcapFactory.getJainObjectList();
	if (!vect.isEmpty()) {  // if the JainObject List is not empty
		// search the stack on SPC, if SPC is already created, use it
		// else, create a new d7TcapStack
		for (Enumeration e = vect.elements(); e.hasMoreElements(); ) {
			d7TcapStack = (com.ss8.jain.protocol.ss7.tcap.JainTcapStackImpl)e.nextElement();
			spcToMatch = d7TcapStack.getSignalingPointCode();
			if(compareSPCs(spcToMatch, defaultPC)) {
				spcFound = true;
				break;
			}
		}
	}
		
	if (!spcFound) {
		System.out.println("\nPathName  : " + d7TcapFactory.getPathName());

		try {
			d7TcapStack = (com.ss8.jain.protocol.ss7.tcap.JainTcapStackImpl)
				d7TcapFactory.createSS7Object ("jain.protocol.ss7.tcap.JainTcapStackImpl");
			d7TcapStack.setSignalingPointCode(defaultPC);
			initializeStack(d7TcapStack);

		} catch (Exception exception) {
			exception.printStackTrace(System.out);
		}
	}

	try {
		int[] spc = d7TcapStack.getSignalingPointCode();

		SignalingPointCode signalingPointCode = new SignalingPointCode(spc[0], spc[1], spc[2]);

		SubSystemAddress    subSystemAddress   = new SubSystemAddress(signalingPointCode,
										getDefaultSSN());
		SccpUserAddress     reqUserAddress     = new SccpUserAddress(subSystemAddress);

		System.out.println("\nADDR : " + reqUserAddress.toString());

		d7TcapProvider = (com.ss8.jain.protocol.ss7.tcap.JainTcapProviderImpl) d7TcapStack.createProvider();

		// Allow Listener to custom set the Buffer and Dialogue values
		d7TcapProvider.setSizeOfBuffers(DEFAULT_BUF_SIZE);
		d7TcapProvider.setNumberOfBuffers(DEFAULT_NUM_BUFS);
		d7TcapProvider.setNumberOfDialogues(DEFAULT_NUM_DLGS);

		d7TcapProvider.addJainTcapListener(this, reqUserAddress);

		originationAddress = userAddress = reqUserAddress;

	} catch (Exception exception) {
		exception.printStackTrace(System.out);
	}
    }


    ////////////////////////////////////////////////////////////////////////////////
    // processComponentIndEvent()
    ////////////////////////////////////////////////////////////////////////////////
    public void processComponentIndEvent(ComponentIndEvent event) {
	int dialogueId;
	int invokeId;

	try {
		switch (event.getPrimitiveType()) {
		case TcapConstants.PRIMITIVE_INVOKE :
			if (prntOpt) {
				System.out.println(event.toString());
				System.out.println("##################################\n");
			}

			InvokeIndEvent receivedInvoke = (InvokeIndEvent)event;
			// now we can access the methods within the Invoke Indication Primitive.
			dialogueId = receivedInvoke.getDialogueId();
			invokeId = receivedInvoke.getInvokeId();
			Operation op = receivedInvoke.getOperation(); 
			if (op.getOperationType() == Operation.OPERATIONTYPE_LOCAL) { 
				// this is a local/private operation 
			} else {
				// this is a global/national operation
			}
			// Execute the Invoke primitive
			try {
			    if (responseType != 0) { 
				sendResponse(receivedInvoke.getDialogueId(), receivedInvoke.getInvokeId());
			    }
			} catch (Exception exception) {
			    exception.printStackTrace(System.out);
			}
			statResponsesSent++;

			break;
	    
		case TcapConstants.PRIMITIVE_ERROR  :
		case TcapConstants.PRIMITIVE_REJECT :
		case TcapConstants.PRIMITIVE_RESULT :
			if (prntOpt) {
				System.out.println(event.toString());
				System.out.println("##################################\n");
			}
                        msgcntresp++;
                        if (msgcntresp == burst) {
                                STOP_TIME = System.currentTimeMillis();
                                System.out.println("\n\nTIME for " + burst + " MESGS IN MILLISECS = " + (STOP_TIME - START_TIME));
				burst = 0;
                        }

			break;
		case TcapConstants.PRIMITIVE_LOCAL_CANCEL :
			statTimeoutsRcvd++;
//			if (prntOpt) {
				System.out.print("##################################");
				System.out.println(event.toString());
				System.out.println("##################################\n");
//			}

			LocalCancelIndEvent receivedLocCancel = (LocalCancelIndEvent)event;
			dialogueId = receivedLocCancel.getDialogueId();

			// release the Dialogue ID back to the system
			d7TcapProvider.releaseDialogueId(dialogueId);

			break;

		default :
			System.out.println("Unknown Primitive Type : " + event.getPrimitiveType());
			break;
		}
	} catch (Exception exception) { 
		// Access the error Message 
		exception.printStackTrace(System.out);
	}

    } // end of processComponentIndEvent() method


    ////////////////////////////////////////////////////////////////////////////////
    // processDialogueIndEvent()
    ////////////////////////////////////////////////////////////////////////////////
    public void processDialogueIndEvent(DialogueIndEvent event){

	if (prntOpt) {
		System.out.println("\n\n##################################");
		System.out.println("Received on SSN   : " + getDefaultSSN());
		System.out.println(event.toString());
	}

	switch(event.getPrimitiveType()) {
	case TcapConstants.PRIMITIVE_UNIDIRECTIONAL :
		statUnidirectRcvd++;
		UnidirectionalIndEvent uni = (UnidirectionalIndEvent)event;
		break;
	case TcapConstants.PRIMITIVE_BEGIN :
		statDialoguesRcvd++;
		BeginIndEvent begin = (BeginIndEvent)event;
		break;
	case TcapConstants.PRIMITIVE_NOTICE :
		NoticeIndEvent notice = (NoticeIndEvent)event;
		break;
	case TcapConstants.PRIMITIVE_CONTINUE :
		break;
	case TcapConstants.PRIMITIVE_END :
		statResponsesRcvd++;
		break;
	case TcapConstants.PRIMITIVE_PROVIDER_ABORT :
	case TcapConstants.PRIMITIVE_USER_ABORT :
		if (prntOpt)
			System.out.println("\n##################################");
		break;
	default:
		System.out.println("Unknown Primitive Type : " + event.getPrimitiveType());
		break;
	}
    }


    ////////////////////////////////////////////////////////////////////////////////
    // addUserAddress()
    ////////////////////////////////////////////////////////////////////////////////
    public void addUserAddress(SccpUserAddress reqUserAddress) throws UserAddressLimitException {
	if (destinationAddress != null) 
	    throw new UserAddressLimitException("One address per listener");
	
	try {
	    d7TcapProvider.addJainTcapListener(this, reqUserAddress);
	    
	    originationAddress = userAddress = reqUserAddress;

	} catch (Exception exception) {

	    exception.printStackTrace(System.out);

	}
    }

    public void processTcapError(TcapErrorEvent error) {
    }


    ////////////////////////////////////////////////////////////////////////////////
    // removeUserAddress()
    ////////////////////////////////////////////////////////////////////////////////
    public void removeUserAddress(SccpUserAddress userAddress) throws InvalidAddressException {
	// Modify the equals method of SccpUserAddress to check if address is correct

	try {
	    if (d7TcapProvider != null) {

		System.out.println("\n** Removing User Address **");
		System.out.println(userAddress);
		
		d7TcapProvider.removeJainTcapListener(this);
		
		d7TcapProvider = null;
	    }
	} catch (Exception exception) {

	    exception.printStackTrace(System.out);

	}
    }


    ////////////////////////////////////////////////////////////////////////////////
    // addUserAddress() DEPRECATED
    ////////////////////////////////////////////////////////////////////////////////
    public void addUserAddress(TcapUserAddress userAddress) {
    }


    ////////////////////////////////////////////////////////////////////////////////
    // removeUserAddress() DEPRECATED
    ////////////////////////////////////////////////////////////////////////////////
    public void removeUserAddress(TcapUserAddress userAddress) {
    }


    ////////////////////////////////////////////////////////////////////////////////
    // getUserAddressList()
    ////////////////////////////////////////////////////////////////////////////////
    public SccpUserAddress[] getUserAddressList() throws UserAddressEmptyException {
	if (destinationAddress == null) 
	    throw new UserAddressEmptyException("No address");
	
	SccpUserAddress[] addressArray = new SccpUserAddress [1];

	addressArray[0] = destinationAddress;

	return addressArray;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // sendResponse()
    ////////////////////////////////////////////////////////////////////////////////
    public void sendResponse(int dlgid, int invkid) {

        try {
                byte[]    operationCode = {1, 1, 2};
		byte[]    parameter     = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
		byte[]    acn           = {1, 2, 3};
		byte[]    user_info     = {40, 4, 3, 2, 1, 0, 9, 8};

                Operation operation           = new Operation(Operation.OPERATIONTYPE_LOCAL,
                                                                        operationCode);

                Parameters parameters         = new Parameters(Parameters.PARAMETERTYPE_SINGLE,
                                                                        parameter);

                // set the dialogue portion of the message
                DialoguePortion dialoguePor = new DialoguePortion();
                dialoguePor.setAppContextIdentifier(DialogueConstants.APPLICATION_CONTEXT_OBJECT);
                dialoguePor.setAppContextName(acn);
                dialoguePor.setUserInformation(user_info);
		
		DialogueReqEvent dialogueRequestEvent;
		if (responseType != 4) {
		    ResultReqEvent resultReqEvent = new ResultReqEvent(this,
								       dlgid,
								       true);
		    
		    // set the parameters of the Invoke Event
		    
		    resultReqEvent.setInvokeId(invkid);
		    resultReqEvent.setLastResultEvent(true);
		    resultReqEvent.setOperation(operation);
		    resultReqEvent.setParameters(parameters);
		    resultReqEvent.setDialogueId(dlgid);

		    d7TcapProvider.sendComponentReqEvent(resultReqEvent);
		    
		    if (responseType == 1) {
			// end
			EndReqEvent endRequestEvent = new EndReqEvent(this, dlgid);
			
			endRequestEvent.setDialoguePortion(dialoguePor);
			//              endRequestEvent.setQualityOfService(QOS_SCCP_CLTS_NS_RE);
			dialogueRequestEvent = endRequestEvent;
			
		    } else {
			
			// continue 
			ContinueReqEvent continueRequestEvent = new ContinueReqEvent(this, dlgid);
			
			if (responseType == 3) {
			    // continue with cgpa
			    
			    continueRequestEvent.setOriginatingAddress(originationAddress);
			}
			
			dialogueRequestEvent = continueRequestEvent;
		    }
		} else {
 
		    UserAbortReqEvent userAbortReqEvent = new UserAbortReqEvent(this,dlgid);
		    
		    userAbortReqEvent.setAbortReason(DialogueConstants.ABORT_REASON_ACN_NOT_SUPPORTED);

		    userAbortReqEvent.setDialoguePortion(dialoguePor);

		    dialogueRequestEvent = userAbortReqEvent;

		}

                d7TcapProvider.sendDialogueReqEvent(dialogueRequestEvent);

        } catch (Exception exception) {
                exception.printStackTrace(System.out);
        }
    }


    ////////////////////////////////////////////////////////////////////////////////
    // setDestinationAddress()
    ////////////////////////////////////////////////////////////////////////////////
    public void setDestinationAddress(int[] destPC, short destSubsysNum) {
	SignalingPointCode signalingPointCode = new SignalingPointCode(destPC[0],
									destPC[1],
									destPC[2]);

	SubSystemAddress   destSubSystemAddress   = new SubSystemAddress(signalingPointCode,
									 destSubsysNum);

	destinationAddress     = new SccpUserAddress(destSubSystemAddress);
    }

    public void setDestinationAddress( SccpUserAddress address)  {
	destinationAddress = address;
    }

    public void setOriginationAddress( SccpUserAddress address)  {
	originationAddress = address;
    }

    public void setDefaultSSN(short ssn) {
	defaultSSN = ssn;
    }
    public short getDefaultSSN() {
	return defaultSSN;
    }
    public void setDefaultDestSSN(short ssn) {
	defaultDestSSN = ssn;
    }
    public short getDefaultDestSSN() {
	return defaultDestSSN;
    }
    public void setDefaultPC(int[] pc) {
	defaultPC[0] = pc[0];
	defaultPC[1] = pc[1];
	defaultPC[2] = pc[2];
    }
    public int[] getDefaultPC() {
	return defaultPC;
    }
    public void setDefaultDestPC(int[] pc) {
	defaultDestPC[0] = pc[0];
	defaultDestPC[1] = pc[1];
	defaultDestPC[2] = pc[2];
    }
    public int[] getDefaultDestPC() {
	return defaultDestPC;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // main()
    ////////////////////////////////////////////////////////////////////////////////
    public static void main(String args[]) {
	JainSampleTcapListener tcapListener = null;
	int choice = 0;
	String InputStr;
	boolean CHOICE_SET = false;
	SccpUserAddress[] addressList;
	BufferedReader bufr = new BufferedReader(new InputStreamReader(System.in));

	try {
		for(;;) {

		        while(!CHOICE_SET) {
				System.out.print("\n\nJAIN TCAP LISTENER IMPLEMENTATION\n" +
				"                                      \n" +
				"   1) Establish a Connection          \n" +
				"   2) Send a Transaction              \n" +
				"   3) Set the Response Type           \n" +
				"   4) Close Connection                \n" +
				"   5) Toggle Print Option ");
				if (prntOpt)
					System.out.print("(enabled)\n");
				else
					System.out.print("(disabled)\n");
				System.out.print(
				"   6) Time a Burst of Messages        \n" +
				"   7) Display Statistics              \n" +
				"   8) Set Destination Address         \n" +
				"   9) Set Origination Address         \n" +
				"  10) Set listener status             \n" + 
				"  11) Quit                          \n\n" +
				"> ");
				try {
					choice = Integer.parseInt(bufr.readLine());
					switch (choice) {
					case 1: case 3: case 5: case 11:
						CHOICE_SET = true;
						break;
					case 2: case 4: case 6: case 7: case 8: case 9: case 10:
						CHOICE_SET = true;
						if (ListenerTbl.size() == 0) {
							System.out.println("\nA Connection must be established first.");
							CHOICE_SET = false;
						}
						break;
					default:
						break;
					}
				} catch (Exception exception) {
					continue;
				}
			} //while
			CHOICE_SET = false;


			switch (choice) {
			case 1:
				ListenerTbl.put(Integer.toString(TblCnt), new JainSampleTcapListener());
				tcapListener = (JainSampleTcapListener) ListenerTbl.get(Integer.toString(TblCnt));
				tcapListener.register();
				tcapListener.setDestinationAddress(tcapListener.getDefaultDestPC(),
								   tcapListener.getDefaultDestSSN());
				System.out.println("\nCONNECTION ID : " + TblCnt++);
				break;

			case 2:
				if ((tcapListener = getTcapListener(0)) == null)
					break;
				for(;;) {
					System.out.print("\n   1) Repeatedly (auto   msg creation)\n" +
							   "   2) Single Msg (manual msg creation)\n" +
							   "\n> ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1: tcapListener.repeatMsgSend();
							break;
						case 2: tcapListener.singleMsgSend();
							break;
						default: continue;
						}
					} catch (Exception exception) {
						System.out.println("Invalid Input.");
					}
					break;
				}
				break;

			case 3:
				if ((tcapListener = getTcapListener(0)) == null)
					break;
				tcapListener.setRespType();
				break;	

			case 4:
				if ((tcapListener = getTcapListener(CONN_ID_RM)) == null)
					break;
				addressList = tcapListener.getUserAddressList();

				// This is a mandatory operation for tcap deregistration
				tcapListener.removeUserAddress(addressList[0]);
				break;

			case 5:
				prntOpt = !prntOpt;
				break;

			case 6:
				if ((tcapListener = getTcapListener(0)) == null)
					break;
				for(;;) {
					System.out.print("\nNumber of Transactions to time? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						burst = choice;
					} catch (Exception exception) {
						continue;
					}
					break;
				} //for

                                START_TIME = System.currentTimeMillis();
                                msgcntresp = 0;
				for (int j = 0; j < burst; j++ ) {
					tcapListener.sendBeginInvoke();
					statDialoguesSent++;
				}
				break;

			case 7:
				if ((tcapListener = getTcapListener(0)) == null)
					break;
				System.out.print("\n\n******************* USER STATISTICS *******************\n\n" +
						"SUBSYSTEM NUMBER           : " + tcapListener.getDefaultSSN() + "\n" +
						"TOTAL # UNIDIRECT SENT     : " + statUnidirectSent            + "\n" +
						"TOTAL # UNIDIRECT RECEIVED : " + statUnidirectRcvd            + "\n" +
						"TOTAL # DIALOGUES SENT     : " + statDialoguesSent            + "\n" +
						"TOTAL # DIALOGUES RECEIVED : " + statDialoguesRcvd            + "\n" +
						"TOTAL # RESPONSES SENT     : " + statResponsesSent            + "\n" +
						"TOTAL # RESPONSES RECEIVED : " + statResponsesRcvd            + "\n" +
						"TOTAL # TIMEOUTS           : " + statTimeoutsRcvd             + "\n" +
						"\n*******************************************************\n" +
						"\nreset user statistics [y/n]? ");
				InputStr = bufr.readLine();
				switch (InputStr.length()) {
				case 1:
					if ((InputStr.compareTo("y") == 0) || (InputStr.compareTo("Y") == 0)) {
						statUnidirectSent = 0;
						statUnidirectRcvd = 0;
						statDialoguesSent = 0;
						statDialoguesRcvd = 0;
						statResponsesSent = 0;
						statResponsesRcvd = 0;
						statTimeoutsRcvd  = 0;
					}
					break;
				default:
					break;
				}
				break;

			case 8:
			    if ((tcapListener = getTcapListener(0)) == null)
				break;

			    tcapListener.setDestinationAddress (tcapListener.getAddress());
			    break;
			case 9:
			    if ((tcapListener = getTcapListener(0)) == null)
				break;

			    tcapListener.setOriginationAddress (tcapListener.getAddress());
			    break;
			case 10:
			    if ((tcapListener = getTcapListener(0)) == null)
				break;

			    int  status;

			    System.out.print("Status (1-Busy/2-Available) ? ");
			    for(;;) {
				try {
				    status = Integer.parseInt(bufr.readLine());
				    
				    if (status != 1 && status != 2) 
					
					throw new Exception ("Invalid Status");
				    
				} catch (Exception exception) {
				    continue;
				}

				break;
			    }

			    tcapListener.getD7TcapProvider().setListenerBusy((status == 1) ? true : false);

			    break;
			case 11:
				if (!ListenerTbl.isEmpty()) {
					for (Enumeration e = ListenerTbl.elements(); e.hasMoreElements(); ) {
						tcapListener = (com.ss8.jain.protocol.ss7.tcap.JainSampleTcapListener)e.nextElement();
						addressList = tcapListener.getUserAddressList();
						// This is a mandatory operation for tcap deregistration
						tcapListener.removeUserAddress(addressList[0]);
					}
				}
				System.out.println("Exiting...");
				return;
			
			default:
				break;
			}
		} //for

	} catch (Exception exception) {
		System.out.println("Exception: " + exception);
		exception.printStackTrace(System.out);
	}
    } //main

     SccpUserAddress getAddress() {
	jain.protocol.ss7.GlobalTitle globalTitle = null;
	String             inputStr;
	SignalingPointCode pointCode   = null;
	SubSystemAddress   subSystemAddress = null;
	SccpUserAddress    sccpUserAddress;
	int                routing;
	boolean            nationalUse;

	System.out.print("\nGT Included (y/n) ? ");
	for(;;) {
	    try {
		inputStr = bufr.readLine();
		
		if ((inputStr.compareTo("y") == 0) || (inputStr.compareTo("Y") == 0)) {
		    globalTitle = getGT();
		}
	    } catch (Exception exception) {
		
	    }
	    break;
	}

	System.out.print("PC/SSN Included (y/n) ? ");
	for(;;) {
	    try {
		inputStr = bufr.readLine();
		if ((inputStr.compareTo("y") == 0) || (inputStr.compareTo("Y") == 0)) {
		     subSystemAddress = getSubSysAddr();
		}
	    }  catch (Exception exception) {
		continue;
	    }
	    break;
	}

	
	System.out.print("Routing (1-Subys/2-GT) ? ");
	for(;;) {
	    try {
		routing = Integer.parseInt(bufr.readLine());

		if (routing != AddressConstants.ROUTING_GLOBALTITLE &&
		    routing != AddressConstants.ROUTING_SUBSYSTEM) 
		    
		    throw new Exception ("Invalid Routing");
		
	    } catch (Exception exception) {
		continue;
	    }
	    break;
	}

	System.out.print("National (y/n) ? ");
	for(;;) {
	    try {
		inputStr = bufr.readLine();
		if ((inputStr.compareTo("y") == 0) || (inputStr.compareTo("Y") == 0)) 
		    nationalUse = true;
		else
		    nationalUse = false;
	    } catch (Exception exception) {
		continue;
	    }

	    break;
	}

	sccpUserAddress = new SccpUserAddress();
	
	if (globalTitle != null)
	    sccpUserAddress.setGlobalTitle(globalTitle);

	if (subSystemAddress != null)
	    sccpUserAddress.setSubSystemAddress(subSystemAddress);
	
	sccpUserAddress.setRoutingIndicator(routing);
	sccpUserAddress.setNationalUse(nationalUse);
	
	System.out.println(sccpUserAddress);

	return sccpUserAddress;

    }

    SubSystemAddress getSubSysAddr() {
	SubSystemAddress    subSystemAddress = null;
	SignalingPointCode  pointCode        = null;
	int                 ssn = AddressConstants.NOT_SET;
	int[] pc  = {0, 0, 0};
	String inputStr;

	subSystemAddress = new SubSystemAddress();
	
	System.out.print("PC Included (y/n) ? ");
	for(;;) {
	    try {
		inputStr = bufr.readLine();
	    } catch (Exception exception) {
		continue;
	    }
	    break;
	}
		
	if ((inputStr.compareTo("y") == 0) || (inputStr.compareTo("Y") == 0)) {
	    for(;;) {
		System.out.print("SPC (x-y-z) ? ");
		try {
		    String entry;
		    entry = new String(bufr.readLine());
		    pc[0] = Integer.parseInt(entry.substring(entry.lastIndexOf("-") + 1));
		    pc[1] = Integer.parseInt(entry.substring(entry.indexOf("-") + 1, entry.lastIndexOf("-")));
		    pc[2] = Integer.parseInt(entry.substring(0, entry.indexOf("-")));

		    pointCode = new SignalingPointCode(pc[2], pc[1], pc[0]);

		} catch (Exception exception) {
		    continue;
		}
		break;
	    } 
	    
	    subSystemAddress.setSignalingPointCode(pointCode);
	}
	
	System.out.print("SSN Included (y/n) ? ");

	for(;;) {
	    try {
		inputStr = bufr.readLine();
	    } catch (Exception exception) {
		continue;
	    }
	    break;
	}
		
	try {
	    if ((inputStr.compareTo("y") == 0) || (inputStr.compareTo("Y") == 0)) {
		for(;;) {
		    System.out.print("SSN? ");
		    try {
			ssn = (short) Integer.parseInt(bufr.readLine());
		    } catch (Exception exception) {
			continue;
		    }
		    break;
		} //for

		subSystemAddress.setSubSystemNumber(ssn);
	    }
	} catch (Exception exception) {
	    exception.printStackTrace(System.out);
	}

	return subSystemAddress;
    }


    jain.protocol.ss7.GlobalTitle getGT() {
	jain.protocol.ss7.GlobalTitle gt = null;
	int         gtie = 0, xlateType = 0, numPlan = 0, encScheme = 0, natOfAddr = 0;
	boolean     oddInd = false;;
	byte[]      addrInfo;
	
	for(;;) {
	    System.out.print("GT Indicator ? ");
	    try {
		gtie =  Integer.parseInt(bufr.readLine());
	    } catch (Exception exception) {
		continue;
	    }
	    break;
	} //for
	
	if (gtie == AddressConstants.GTINDICATOR_0001 ||
	    gtie == AddressConstants.GTINDICATOR_0010 ||
	    gtie == AddressConstants.GTINDICATOR_0011 ||
	    gtie == AddressConstants.GTINDICATOR_0100) {

	    for(;;) {
		System.out.print("TR Type ? ");
		try {
		    xlateType =  Integer.parseInt(bufr.readLine());
		} catch (Exception exception) {
		    continue;
		}
		break;
	    } 
	

	    if (gtie == AddressConstants.GTINDICATOR_0001 ||
		gtie == AddressConstants.GTINDICATOR_0011 ||
		gtie == AddressConstants.GTINDICATOR_0100)
		for(;;) {
		    System.out.print("Number Plan ? ");
		    try {
			numPlan =  Integer.parseInt(bufr.readLine());
		    } catch (Exception exception) {
			continue;
		    }
		    break;
		} //for
	

	    if (gtie == AddressConstants.GTINDICATOR_0001 ||
		gtie == AddressConstants.GTINDICATOR_0011 ||
		gtie == AddressConstants.GTINDICATOR_0100)
		for(;;) {
		    System.out.print("Encoding Scheme ? ");
		    try {
			encScheme =  Integer.parseInt(bufr.readLine());
		    } catch (Exception exception) {
			continue;
		    }
		    break;
		} //for
	
	
	    if (gtie == AddressConstants.GTINDICATOR_0001 ||
		gtie == AddressConstants.GTINDICATOR_0100)
		for(;;) {
		    System.out.print("Nature of Addr. ? ");
		    try {
			natOfAddr =  Integer.parseInt(bufr.readLine());
		    } catch (Exception exception) {
			continue;
		    }
		    break;
		} //for
	

	    if (gtie == AddressConstants.GTINDICATOR_0001)
		for(;;) {
		    System.out.print("Odd Ind.(0/1) ? ");
		    try {
			if (Integer.parseInt(bufr.readLine()) !=0) 
			    oddInd = true;
			else 
			    oddInd = false;

		    } catch (Exception exception) {
			continue;
		    }
		    break;
		} //for
	
	    System.out.println("Enter Address Info");
	    addrInfo = get_input_bytes();
	
	    switch (gtie) {
	    case AddressConstants.GTINDICATOR_0001:
		// fill the superset of ANSI/ITU gt information
	    
		gt = new jain.protocol.ss7.GTIndicator0001((byte) xlateType,
							   numPlan,
							   encScheme,
							   addrInfo);
	    
		((jain.protocol.ss7.GTIndicator0001) gt).setOddIndicator(oddInd);
		((jain.protocol.ss7.GTIndicator0001) gt).setNatureOfAddrInd(natOfAddr);
		break;
	    
	    case AddressConstants.GTINDICATOR_0010:
		gt = new jain.protocol.ss7.GTIndicator0010((byte) xlateType,
							   addrInfo);
	    
		break;
	    case AddressConstants.GTINDICATOR_0011:
		gt = new jain.protocol.ss7.GTIndicator0011((byte) xlateType,
							   numPlan,
							   encScheme,
							   addrInfo);
		
		break;
	    case AddressConstants.GTINDICATOR_0100:
		gt = new jain.protocol.ss7.GTIndicator0100((byte) xlateType,
							   numPlan,
							   encScheme,
							   natOfAddr,
							   addrInfo);
		break;
	    }
	}
	return gt;
    }
    

    
    ////////////////////////////////////////////////////////////////////////////////
    // getTcapListener()
    ////////////////////////////////////////////////////////////////////////////////
    public static JainSampleTcapListener getTcapListener(int flags) {
	BufferedReader bufr = new BufferedReader(new InputStreamReader(System.in));
	JainSampleTcapListener tcapListener = null;
	int choice;
	String str;

	for(;;) {
		if (ListenerTbl.size() == 1) {
			Enumeration e = ListenerTbl.keys();
			str = (String)e.nextElement();
		} else {
			System.out.print("\nConnection ID? ");
			try {
				choice = Integer.parseInt(bufr.readLine());
			} catch (Exception exception) {
				System.out.println("Invalid Input.");
				continue;
			}
			str = Integer.toString(choice);
		}
		tcapListener = (JainSampleTcapListener) ListenerTbl.get(str);
		if (tcapListener == null)
			System.out.print("\nInvalid Connection ID!\n");
		break;
	} //for

	if ((flags & CONN_ID_RM) == CONN_ID_RM)
		ListenerTbl.remove(str);

	return tcapListener;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // initializeStack()
    ////////////////////////////////////////////////////////////////////////////////
    public void initializeStack(com.ss8.jain.protocol.ss7.tcap.JainTcapStackImpl d7TcapStack) {
        int choice;

	System.out.print("\nSetting Stack Vendor Name as:  ");
	d7TcapStack.setVendorName("com.ss8");
	System.out.println(d7TcapStack.getVendorName());

	for(;;) {
		System.out.print("\n\nprotocol versions available: \n\n" +
				"     1) ANSI 1992\n" +
				"     2) ANSI 1996\n" +
				"     3) ITU  1993\n" +
				"     4) ITU  1997\n" +
				"\nprotocol version [1-4]? ");
		try {
			choice = Integer.parseInt(bufr.readLine());
			switch (choice) {
			case 1:
				d7TcapStack.setStackSpecification(TcapConstants.STACK_SPECIFICATION_ANSI_92);
				break;
			case 2:
				d7TcapStack.setStackSpecification(TcapConstants.STACK_SPECIFICATION_ANSI_96);
				break;
			case 3:
				d7TcapStack.setStackSpecification(TcapConstants.STACK_SPECIFICATION_ITU_93);
				break;
			case 4:
				d7TcapStack.setStackSpecification(TcapConstants.STACK_SPECIFICATION_ITU_97);
				break;
			default:
				continue;
			}
		} catch (Exception exception) {
			continue;
		}
		break;
	} //for

	System.out.print("\nSetting Stack Name as:  ");
	d7TcapStack.setStackName("D7Tcap-" + d7TcapStack.getSPNo() + "-" + getDefaultSSN());
	System.out.println(d7TcapStack.getStackName());

    } //initializeStack


    ////////////////////////////////////////////////////////////////////////////////
    // initializeSettings()
    ////////////////////////////////////////////////////////////////////////////////
    public void initializeSettings() {
	int choice;
	String entry;
	int[] pc = {0, 0, 0};

	for(;;) {
		System.out.print("\nOriginating SPC (x-y-z)? ");
		try {
			entry = new String(bufr.readLine());
			pc[0] = Integer.parseInt(entry.substring(entry.lastIndexOf("-") + 1));
			pc[1] = Integer.parseInt(entry.substring(entry.indexOf("-") + 1, entry.lastIndexOf("-")));
			pc[2] = Integer.parseInt(entry.substring(0, entry.indexOf("-")));
			setDefaultPC(pc);
		} catch (Exception exception) {
			continue;
		}
		break;
	} //for

	for(;;) {
		System.out.print("\nOriginating SSN? ");
		try {
			choice = Integer.parseInt(bufr.readLine());
			setDefaultSSN((short)choice);
		} catch (Exception exception) {
			continue;
		}
		break;
	} //for

	for(;;) {
		System.out.print("\nDestination SPC (x-y-z)? ");
		try {
			entry = new String(bufr.readLine());
			pc[0] = Integer.parseInt(entry.substring(entry.lastIndexOf("-") + 1));
			pc[1] = Integer.parseInt(entry.substring(entry.indexOf("-") + 1, entry.lastIndexOf("-")));
			pc[2] = Integer.parseInt(entry.substring(0, entry.indexOf("-")));
			setDefaultDestPC(pc);
		} catch (Exception exception) {
			continue;
		}
		break;
	} //for

	for(;;) {
		System.out.print("\nDestination SSN? ");
		try {
			choice = Integer.parseInt(bufr.readLine());
			setDefaultDestSSN((short)choice);
		} catch (Exception exception) {
			continue;
		}
		break;
	} //for
    }


    ////////////////////////////////////////////////////////////////////////////////
    // compareSPCs()
    ////////////////////////////////////////////////////////////////////////////////
    public boolean compareSPCs(int[] a, int[] b) {

	if (( a[0] == b[0] ) &&
	    ( a[1] == b[1] ) &&
	    ( a[2] == b[2] )) {
		return true;
	}
	return false;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // setRespType()
    ////////////////////////////////////////////////////////////////////////////////
    public void setRespType() {
	int choice;

	for(;;) {
		System.out.print("\n\nSet Response Message Type: \n\n" +
				 "     0) No response \n" +
				 "     1) END / RESULT \n" +
				 "     2) CONTINUE / RESULT  \n" +
				 "     3) CONTINUE with CGPA / RESULT\n" +
				 "     4) ABORT \n" +
				"\nresponse[0-4]? ");
		try {
			choice = Integer.parseInt(bufr.readLine());
			switch (choice) {
			case 0: case 1: case 2: case 3:case 4:
				responseType = choice;
				break;
			default:
				continue;
			}
		} catch (Exception exception) {
			continue;
		}
		break;
	} //for
    }


    ////////////////////////////////////////////////////////////////////////////////
    // repeatMsgSend()
    ////////////////////////////////////////////////////////////////////////////////
    public void repeatMsgSend() {
	int choice;
	int msg_cnt = 0;
	int secs = 0;
	int ips = 0;
	int tpi = 0;


	for(;;) {
		System.out.print("\nNumber of Transactions/per interrupt? ");
		try {
			choice = Integer.parseInt(bufr.readLine());
			tpi = choice;
		} catch (Exception exception) {
			continue;
		}
		break;
	} //for

	for(;;) {
		System.out.print("Number of Interrupts/per second     ? ");
		try {
			choice = Integer.parseInt(bufr.readLine());
			ips = choice;
		} catch (Exception exception) {
			continue;
		}
		break;
	} //for

	for(;;) {
		System.out.print("Number of seconds                   ? ");
		try {
			choice = Integer.parseInt(bufr.readLine());
			secs = choice;
		} catch (Exception exception) {
			continue;
		}
		break;
	} //for

	msg_cnt = 0;
	try {
		while (secs -- > 0) {
			for (int k = 0; k < ips; k++ ) {
				for (int j = 0; j < tpi; j++ ) {
					sendBeginInvoke();
					statDialoguesSent++;
					msg_cnt++;
					if (!prntOpt) {
						if ( (msg_cnt % 100) == 0 ) {
							for(int i = 0; i < Integer.toString(msg_cnt).length(); i++) {
								System.out.print("\b");
							}
							System.out.print("\b\b\b\b\b\b\bSent: " + msg_cnt + " ");
						}
					}
				}
				Thread.currentThread().sleep(1000 / ips);
			}
		}
	} catch (Exception exception) {
		System.out.println("Exception : " + exception);
	}
	System.out.println("\nMessages Sent: " + msg_cnt);
    }


    ////////////////////////////////////////////////////////////////////////////////
    // sendBeginInvoke()
    ////////////////////////////////////////////////////////////////////////////////
    public void sendBeginInvoke() {

	try {
		// get a new Dialogue Id for use in this Dialogue
		int newDialogueId = d7TcapProvider.getNewDialogueId();

		// get a new Invoke Id for use within the Dialogue
		int newInvokeId = d7TcapProvider.getNewInvokeId(newDialogueId);

		byte[]    operationCode = {1, 1, 2};
		byte[]    parameter     = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
		byte[]    acn           = {1, 2, 3};
		byte[]    user_info     = {40, 4, 3, 2, 1, 0, 9, 8};

		Operation operation           = new Operation(Operation.OPERATIONTYPE_LOCAL,
								operationCode);

		Parameters parameters         = new Parameters(Parameters.PARAMETERTYPE_SINGLE,
								parameter);

		InvokeReqEvent invokeReqEvent = new InvokeReqEvent(this,newDialogueId,
								operation);

		// set the parameters of the Invoke Event

		invokeReqEvent.setInvokeId(newInvokeId);
		invokeReqEvent.setClassType(ComponentConstants.CLASS_1);
		invokeReqEvent.setTimeOut(INVOKE_TIMER_1);
		invokeReqEvent.setLastInvokeEvent(true);
		invokeReqEvent.setParameters(parameters);

		d7TcapProvider.sendComponentReqEvent(invokeReqEvent);

		// set the dialogue portion of the message
		DialoguePortion dialoguePor = new DialoguePortion();
		dialoguePor.setAppContextIdentifier(DialogueConstants.APPLICATION_CONTEXT_OBJECT);
		dialoguePor.setAppContextName(acn);
		dialoguePor.setUserInformation(user_info);

		BeginReqEvent beginRequestEvent = new BeginReqEvent(this,newDialogueId,
								originationAddress,destinationAddress);

		beginRequestEvent.setDialoguePortion(dialoguePor);
		beginRequestEvent.setQualityOfService(QOS_SCCP_CLTS_NS_RE);

		d7TcapProvider.sendDialogueReqEvent(beginRequestEvent);

	} catch (Exception exception) {
		exception.printStackTrace(System.out);
	}
    }


    ////////////////////////////////////////////////////////////////////////////////
    // singleMsgSend()
    ////////////////////////////////////////////////////////////////////////////////
    public void singleMsgSend() {
	int choice;
	int dialogueId = 0;
	int invokeId = 0;
	int msg_cnt = 0;
	int secs = 0;
	int ips = 0;
	int tpi = 0;
	int count = 0;
	int operType = 0;
	int paramType = 0;
	int errType = 0;
	int probType = 0;
	int probCode = 0;
	byte qualityOfServ = 0;
	boolean lastResult = true;
	byte[] operationCode = null;
	byte[] parameterCode = null;
	byte[] errorCode = null;
	byte[] appContextName = null;
	byte[] secContextInfo = null;
	byte[] confidentInfo = null;
	byte[] userInfo = null;
	Operation operation = null;
	Parameters parameters = null;
	DialoguePortion dialoguePor = null;
	String inputStr;


	for(;;) {
		System.out.print("\n\nDialogue ID Info:        \n\n" +
				"     1) Get A New Dialogue ID   \n" +
				"     2) Use Existing Dialogue ID\n" +
				"\nresponse[1-2]? ");
		try {
			choice = Integer.parseInt(bufr.readLine());
			switch (choice) {
			case 1:
				// get a new Dialogue Id for use in this Dialogue
				dialogueId = d7TcapProvider.getNewDialogueId();
				break;
			case 2:
				System.out.print("Enter Dialogue ID: ");
				try {
					choice = Integer.parseInt(bufr.readLine());
					dialogueId = choice;
				} catch (Exception exception) {
					System.out.println("Exception : " + exception);
					continue;
				}
				break;
			default:
				continue;
			}
		} catch (Exception exception) {
			System.out.println("Exception : " + exception);
			continue;
		}
		break;
	} //for

	// Enter Component Information
	for(;;) {
		System.out.print("\n\nEnter Component Type: \n\n" +
				"     1) Invoke               \n" +
				"     2) Result               \n" +
				"     3) Error                \n" +
				"     4) Reject               \n" +
				"     5) User Cancel          \n" +
				"     6) Timer Reset          \n" +
				"     7) None                 \n" +
				"\nresponse[1-7]? ");
		try {
			choice = Integer.parseInt(bufr.readLine());
			switch (choice) {
			case 1:
				for(;;) {
					System.out.print("\n\nEnter Invoke Operation Type: \n\n" +
							"     1) Local Operation (4 byte max)\n" +
							"     2) Global Operation            \n" +
							"\nresponse[1-2]? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1:
							operType = Operation.OPERATIONTYPE_LOCAL;
							break;
						case 2:
							operType = Operation.OPERATIONTYPE_GLOBAL;
							break;
						default:
							continue;
						}
						System.out.println("\nEnter Operation Code");
						operationCode = get_input_bytes();

					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					operation = new Operation(operType, operationCode);
					break;
				} //for
				InvokeReqEvent invokeReqEvent = new InvokeReqEvent(this,dialogueId,operation);

				for(;;) {
					System.out.print("\n\nClass Type: \n\n" +
							"     1) Class 1    \n" +
							"     2) Class 2    \n" +
							"     3) Class 3    \n" +
							"     4) Class 4    \n" +
							"     5) None       \n" +
							"\nresponse[1-5]? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1:
							invokeReqEvent.setClassType(ComponentConstants.CLASS_1);
							break;
						case 2:
							invokeReqEvent.setClassType(ComponentConstants.CLASS_2);
							break;
						case 3:
							invokeReqEvent.setClassType(ComponentConstants.CLASS_3);
							break;
						case 4:
							invokeReqEvent.setClassType(ComponentConstants.CLASS_4);
							break;
						case 5:
							break;
						default:
							continue;
						}
					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					break;
				} //for

				for(;;) {
					System.out.print("\n\nInvoke ID Info:        \n\n" +
							"     1) Get A New Invoke ID   \n" +
							"     2) Use Existing Invoke ID\n" +
							"     3) None                  \n" +
							"\nresponse[1-3]? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1:
							// get a new Invoke Id for use in this Dialogue
							invokeReqEvent.setInvokeId(d7TcapProvider.getNewInvokeId(dialogueId));
							break;
						case 2:
							System.out.print("Enter Invoke ID: ");
							try {
								choice = Integer.parseInt(bufr.readLine());
								invokeReqEvent.setInvokeId(choice);
							} catch (Exception exception) {
								System.out.println("Exception : " + exception);
								continue;
							}
							break;
						case 3:
							break;
						default:
							continue;
						}
					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					break;
				} //for

				for(;;) {
					System.out.print("\n\nLinked ID Info:             \n\n" +
							"     1) Enter a Link/Correlation ID\n" +
							"     2) None                       \n" +
							"\nresponse[1-2]? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1:
							System.out.print("Enter Link/Correlation ID: ");
							try {
								choice = Integer.parseInt(bufr.readLine());
								invokeReqEvent.setLinkedId(choice);
							} catch (Exception exception) {
								System.out.println("Exception : " + exception);
								continue;
							}
							break;
						case 2:
							break;
						default:
							continue;
						}
					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					break;
				} //for

				for(;;) {
					System.out.print("\n\nEnter Parameter Type: \n\n" +
							"     1) SINGLE               \n" +
							"     2) SEQUENCE             \n" +
							"     3) SET                  \n" +
							"     4) None                 \n" +
							"\nresponse[1-4]? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1:
							paramType = Parameters.PARAMETERTYPE_SINGLE;
							System.out.println("\nEnter Parameter Information");
							parameterCode = get_input_bytes();
							parameters = new Parameters(paramType, parameterCode);
							invokeReqEvent.setParameters(parameters);
							break;
						case 2:
							paramType = Parameters.PARAMETERTYPE_SEQUENCE;
							System.out.println("\nEnter Parameter Information");
							parameterCode = get_input_bytes();
							parameters = new Parameters(paramType, parameterCode);
							invokeReqEvent.setParameters(parameters);
							break;
						case 3:
							paramType = Parameters.PARAMETERTYPE_SET;
							System.out.println("\nEnter Parameter Information");
							parameterCode = get_input_bytes();
							parameters = new Parameters(paramType, parameterCode);
							invokeReqEvent.setParameters(parameters);
							break;
						case 4:
							break;
						default:
							continue;
						}
					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					break;
				} //for

				for(;;) {
					System.out.print("\n\nEnter Timeout Value in Milliseconds, \n" +
							"ie. 1000 = 1sec, 0 = no timeout    : ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						invokeReqEvent.setTimeOut(choice);
					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					break;
				} //for

				for(;;) {
					System.out.print("\n\nIs Last Invoke Event?\n\n" +
							"     1) True                \n"  +
							"     2) False               \n" +
							"\nresponse[1-2]? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1:
							invokeReqEvent.setLastInvokeEvent(true);
							break;
						case 2:
							invokeReqEvent.setLastInvokeEvent(false);
							break;
						default:
							continue;
						}
					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					break;
				} //for
				d7TcapProvider.sendComponentReqEvent(invokeReqEvent);
				break; // case 1 invoke
			case 2:
				for(;;) {
					System.out.print("\n\nIs Last Result Event?\n\n" +
							"     1) True                \n"  +
							"     2) False               \n" +
							"\nresponse[1-2]? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1:
							lastResult = true;
							break;
						case 2:
							lastResult = false;
							break;
						default:
							continue;
						}
					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					break;
				} //for
				ResultReqEvent resultReqEvent = new ResultReqEvent(this,dialogueId,lastResult);

				for(;;) {
					System.out.print("\n\nInvoke ID Info:        \n\n" +
							"     1) Get A New Invoke ID   \n" +
							"     2) Use Existing Invoke ID\n" +
							"     3) None                  \n" +
							"\nresponse[1-3]? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1:
							// get a new Invoke Id for use in this Dialogue
							resultReqEvent.setInvokeId(d7TcapProvider.getNewInvokeId(dialogueId));
							break;
						case 2:
							System.out.print("Enter Invoke ID: ");
							try {
								choice = Integer.parseInt(bufr.readLine());
								resultReqEvent.setInvokeId(choice);
							} catch (Exception exception) {
								System.out.println("Exception : " + exception);
								continue;
							}
							break;
						case 3:
							break;
						default:
							continue;
						}
					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					break;
				} //for

				for(;;) {
					System.out.print("\n\nEnter Invoke Operation Type: \n\n" +
							"     1) Local Operation (4 byte max)\n" +
							"     2) Global Operation            \n" +
							"     3) None                        \n" +
							"\nresponse[1-3]? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1:
							operType = Operation.OPERATIONTYPE_LOCAL;
							System.out.println("\nEnter Operation Code");
							operationCode = get_input_bytes();
							operation = new Operation(operType, operationCode);
							resultReqEvent.setOperation(operation);
							break;
						case 2:
							operType = Operation.OPERATIONTYPE_GLOBAL;
							System.out.println("\nEnter Operation Code");
							operationCode = get_input_bytes();
							operation = new Operation(operType, operationCode);
							resultReqEvent.setOperation(operation);
							break;
						case 3:
							break;
						default:
							continue;
						}

					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					break;
				} //for

				for(;;) {
					System.out.print("\n\nEnter Parameter Type: \n\n" +
							"     1) SINGLE               \n" +
							"     2) SEQUENCE             \n" +
							"     3) SET                  \n" +
							"     4) None                 \n" +
							"\nresponse[1-4]? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1:
							paramType = Parameters.PARAMETERTYPE_SINGLE;
							System.out.println("\nEnter Parameter Information");
							parameterCode = get_input_bytes();
							parameters = new Parameters(paramType, parameterCode);
							resultReqEvent.setParameters(parameters);
							break;
						case 2:
							paramType = Parameters.PARAMETERTYPE_SEQUENCE;
							System.out.println("\nEnter Parameter Information");
							parameterCode = get_input_bytes();
							parameters = new Parameters(paramType, parameterCode);
							resultReqEvent.setParameters(parameters);
							break;
						case 3:
							paramType = Parameters.PARAMETERTYPE_SET;
							System.out.println("\nEnter Parameter Information");
							parameterCode = get_input_bytes();
							parameters = new Parameters(paramType, parameterCode);
							resultReqEvent.setParameters(parameters);
							break;
						default:
							continue;
						}

					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					break;
				} //for
				d7TcapProvider.sendComponentReqEvent(resultReqEvent);
				break; // case 2 result
			case 3:
				for(;;) {
					System.out.print("\n\nEnter Error Type:        \n\n" +
							"     1) LOCAL ERROR (4 byte max)\n" +
							"     2) GLOBAL ERROR            \n" +
							"\nresponse[1-2]? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1:
							errType = ComponentConstants.ERROR_LOCAL;
							break;
						case 2:
							errType = ComponentConstants.ERROR_GLOBAL;
							break;
						default:
							continue;
						}
						System.out.println("\nEnter Error Code Information");
						errorCode = get_input_bytes();

					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					break;
				} //for
				ErrorReqEvent errorReqEvent = new ErrorReqEvent(this,dialogueId,errType,errorCode);

				for(;;) {
					System.out.print("\n\nInvoke ID Info:        \n\n" +
							"     1) Get A New Invoke ID   \n" +
							"     2) Use Existing Invoke ID\n" +
							"     3) None                  \n" +
							"\nresponse[1-3]? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1:
							// get a new Invoke Id for use in this Dialogue
							errorReqEvent.setInvokeId(d7TcapProvider.getNewInvokeId(dialogueId));
							break;
						case 2:
							System.out.print("Enter Invoke ID: ");
							try {
								choice = Integer.parseInt(bufr.readLine());
								errorReqEvent.setInvokeId(choice);
							} catch (Exception exception) {
								System.out.println("Exception : " + exception);
								continue;
							}
							break;
						case 3:
							break;
						default:
							continue;
						}
					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					break;
				} //for

				for(;;) {
					System.out.print("\n\nEnter Parameter Type: \n\n" +
							"     1) SINGLE               \n" +
							"     2) SEQUENCE             \n" +
							"     3) SET                  \n" +
							"     4) None                 \n" +
							"\nresponse[1-3]? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1:
							paramType = Parameters.PARAMETERTYPE_SINGLE;
							System.out.println("\nEnter Parameter Information");
							parameterCode = get_input_bytes();
							parameters = new Parameters(paramType, parameterCode);
							errorReqEvent.setParameters(parameters);
							break;
						case 2:
							paramType = Parameters.PARAMETERTYPE_SEQUENCE;
							System.out.println("\nEnter Parameter Information");
							parameterCode = get_input_bytes();
							parameters = new Parameters(paramType, parameterCode);
							errorReqEvent.setParameters(parameters);
							break;
						case 3:
							paramType = Parameters.PARAMETERTYPE_SET;
							System.out.println("\nEnter Parameter Information");
							parameterCode = get_input_bytes();
							parameters = new Parameters(paramType, parameterCode);
							errorReqEvent.setParameters(parameters);
							break;
						case 4:
							break;
						default:
							continue;
						}

					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					break;
				} //for
				d7TcapProvider.sendComponentReqEvent(errorReqEvent);
				break; // case 3 error
			case 4:
				for(;;) {
					System.out.print("\n\nProblem Type:         \n\n" +
							"     1) General Problem      \n" +
							"     2) Invoke Problem       \n" +
							"     3) Return Result Problem\n" +
							"     4) Return Error Problem \n" +
							"     5) Transaction Problem  \n" +
							"\nresponse[1-5]? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1:
							probType = ComponentConstants.PROBLEM_TYPE_GENERAL;
							System.out.print("\n\nGeneral Problem Code: \n\n" +
									"     1) Unrecognized Comp    \n" +
									"     2) Mistyped Comp        \n" +
									"     3) Bad Structured Comp  \n" +
									"     4) Incorrect Comp Coding\n" +
									"\nresponse[1-4]? ");
							try {
								choice = Integer.parseInt(bufr.readLine());
								switch (choice) {
								case 1:
									probCode = ComponentConstants.PROBLEM_CODE_UNRECOGNISED_COMPONENT;
									break;
								case 2:
									probCode = ComponentConstants.PROBLEM_CODE_MISTYPED_COMPONENT;
									break;
								case 3:
									probCode = ComponentConstants.PROBLEM_CODE_BADLY_STRUCTURED_COMPONENT;
									break;
								case 4:
									probCode = ComponentConstants.PROBLEM_CODE_INCORRECT_COMPONENT_CODING;
									break;
								default:
									continue;
								}
							} catch (Exception exception) {
								System.out.println("Exception : " + exception);
								continue;
							}
							break;
						case 2:
							probType = ComponentConstants.PROBLEM_TYPE_INVOKE;
							System.out.print("\n\nInvoke Problem Code:        \n\n" +
									"     1) Duplicate Invoke ID        \n" +
									"     2) Unrecogized Operation      \n" +
									"     3) Mistyped Parameter         \n" +
									"     4) Resource Limitation        \n" +
									"     5) Initiating Release         \n" +
									"     6) Unrecognized Linked ID     \n" +
									"     7) Linked Response Unexpected \n" +
									"     8) Unexpected Linked Operation\n" +
									"\nresponse[1-8]? ");
							try {
								choice = Integer.parseInt(bufr.readLine());
								switch (choice) {
								case 1:
									probCode = ComponentConstants.PROBLEM_CODE_DUPLICATE_INVOKE_ID;
									break;
								case 2:
									probCode = ComponentConstants.PROBLEM_CODE_UNRECOGNIZED_OPERATION;
									break;
								case 3:
									probCode = ComponentConstants.PROBLEM_CODE_MISTYPED_PARAMETER;
									break;
								case 4:
									probCode = ComponentConstants.PROBLEM_CODE_RESOURCE_LIMITATION;
									break;
								case 5:
									probCode = ComponentConstants.PROBLEM_CODE_INITIATING_RELEASE;
									break;
								case 6:
									probCode = ComponentConstants.PROBLEM_CODE_UNRECOGNIZED_LINKED_ID;
									break;
								case 7:
									probCode = ComponentConstants.PROBLEM_CODE_LINKED_RESPONSE_UNEXPECTED;
									break;
								case 8:
									probCode = ComponentConstants.PROBLEM_CODE_UNEXPECTED_LINKED_OPERATION;
									break;
								default:
									continue;
								}
							} catch (Exception exception) {
								System.out.println("Exception : " + exception);
								continue;
							}
							break;
						case 3:
							probType = ComponentConstants.PROBLEM_TYPE_RETURN_RESULT;
							System.out.print("\n\nReturn Result Problem Code: \n\n" +
									"     1) Unrecognized Invoke ID     \n" +
									"     2) Return Result Unexpected   \n" +
									"\nresponse[1-2]? ");
							try {
								choice = Integer.parseInt(bufr.readLine());
								switch (choice) {
								case 1:
									probCode = ComponentConstants.PROBLEM_CODE_UNRECOGNIZED_INVOKE_ID;
									break;
								case 2:
									probCode = ComponentConstants.PROBLEM_CODE_RETURN_RESULT_UNEXPECTED;
									break;
								default:
									continue;
								}
							} catch (Exception exception) {
								System.out.println("Exception : " + exception);
								continue;
							}
							break;
						case 4:
							probType = ComponentConstants.PROBLEM_TYPE_RETURN_ERROR;
							System.out.print("\n\nReturn Error Problem Code: \n\n" +
									"     1) Return Error Unexpected   \n" +
									"     2) Unrecognized Error        \n" +
									"     3) Unexpected Error          \n" +
									"\nresponse[1-3]? ");
							try {
								choice = Integer.parseInt(bufr.readLine());
								switch (choice) {
								case 1:
									probCode = ComponentConstants.PROBLEM_CODE_RETURN_ERROR_UNEXPECTED;
									break;
								case 2:
									probCode = ComponentConstants.PROBLEM_CODE_UNRECOGNIZED_ERROR;
									break;
								case 3:
									probCode = ComponentConstants.PROBLEM_CODE_UNEXPECTED_ERROR;
									break;
								default:
									continue;
								}
							} catch (Exception exception) {
								System.out.println("Exception : " + exception);
								continue;
							}
							break;
						case 5:
							probType = ComponentConstants.PROBLEM_TYPE_TRANSACTION;
							System.out.print("\n\nTransaction Problem Code:    \n\n" +
									"     1) Unrecognized Package Type   \n" +
									"     2) Incorrect Transaction       \n" +
									"     3) Badly Structured Transaction\n" +
									"     4) Unassigned Responding ID    \n" +
									"     5) Permission to Release       \n" +
									"     6) Resource Unavailable        \n" +
									"\nresponse[1-6]? ");
							try {
								choice = Integer.parseInt(bufr.readLine());
								switch (choice) {
								case 1:
									probCode = ComponentConstants.PROBLEM_CODE_UNRECOGNIZED_PACKAGE_TYPE;
									break;
								case 2:
									probCode = ComponentConstants.PROBLEM_CODE_INCORRECT_TRANSACTION;
									break;
								case 3:
									probCode = ComponentConstants.PROBLEM_CODE_BADLY_STRUCTURED_TRANSACTION;
									break;
								case 4:
									probCode = ComponentConstants.PROBLEM_CODE_UNASSIGNED_RESPONDING_ID;
									break;
								case 5:
									probCode = ComponentConstants.PROBLEM_CODE_PERMISSION_TO_RELEASE;
									break;
								case 6:
									probCode = ComponentConstants.PROBLEM_CODE_RESOURCE_UNAVAILABLE;
									break;
								default:
									continue;
								}
							} catch (Exception exception) {
								System.out.println("Exception : " + exception);
								continue;
							}
							break;
						default:
							continue;
						}
					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					break;
				} //for
				RejectReqEvent rejectReqEvent = new RejectReqEvent(this,dialogueId,probType,probCode);

				for(;;) {
					System.out.print("\n\nInvoke ID Info:        \n\n" +
							"     1) Get A New Invoke ID   \n" +
							"     2) Use Existing Invoke ID\n" +
							"     3) None                  \n" +
							"\nresponse[1-3]? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1:
							// get a new Invoke Id for use in this Dialogue
							rejectReqEvent.setInvokeId(d7TcapProvider.getNewInvokeId(dialogueId));
							break;
						case 2:
							System.out.print("Enter Invoke ID: ");
							try {
								choice = Integer.parseInt(bufr.readLine());
								rejectReqEvent.setInvokeId(choice);
							} catch (Exception exception) {
								System.out.println("Exception : " + exception);
								continue;
							}
							break;
						case 3:
							break;
						default:
							continue;
						}
					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					break;
				} //for

				for(;;) {
					System.out.print("\n\nEnter Parameter Type: \n\n" +
							"     1) SINGLE               \n" +
							"     2) SEQUENCE             \n" +
							"     3) SET                  \n" +
							"     4) None                 \n" +
							"\nresponse[1-4]? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1:
							paramType = Parameters.PARAMETERTYPE_SINGLE;
							System.out.println("\nEnter Parameter Information");
							parameterCode = get_input_bytes();
							parameters = new Parameters(paramType, parameterCode);
							rejectReqEvent.setParameters(parameters);
							break;
						case 2:
							paramType = Parameters.PARAMETERTYPE_SEQUENCE;
							System.out.println("\nEnter Parameter Information");
							parameterCode = get_input_bytes();
							parameters = new Parameters(paramType, parameterCode);
							rejectReqEvent.setParameters(parameters);
							break;
						case 3:
							paramType = Parameters.PARAMETERTYPE_SET;
							System.out.println("\nEnter Parameter Information");
							parameterCode = get_input_bytes();
							parameters = new Parameters(paramType, parameterCode);
							rejectReqEvent.setParameters(parameters);
							break;
						case 4:
							break;
						default:
							continue;
						}

					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					break;
				} //for
				d7TcapProvider.sendComponentReqEvent(rejectReqEvent);
				break; // case 4 reject
			case 5:
				for(;;) {
					System.out.print("\n\nInvoke ID Info:        \n\n" +
							"     1) Get A New Invoke ID   \n" +
							"     2) Use Existing Invoke ID\n" +
							"\nresponse[1-2]? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1:
							// get a new Invoke Id for use in this Dialogue
							invokeId = d7TcapProvider.getNewInvokeId(dialogueId);
							break;
						case 2:
							System.out.print("Enter Invoke ID: ");
							try {
								choice = Integer.parseInt(bufr.readLine());
								invokeId = choice;
							} catch (Exception exception) {
								System.out.println("Exception : " + exception);
								continue;
							}
							break;
						default:
							continue;
						}
					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					break;
				} //for
				UserCancelReqEvent userCancelReqEvent = new UserCancelReqEvent(this,dialogueId,invokeId);
				d7TcapProvider.sendComponentReqEvent(userCancelReqEvent);
				break; // case 5 user cancel
			case 6:
				for(;;) {
					System.out.print("\n\nInvoke ID Info:        \n\n" +
							"     1) Get A New Invoke ID   \n" +
							"     2) Use Existing Invoke ID\n" +
							"\nresponse[1-2]? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1:
							// get a new Invoke Id for use in this Dialogue
							invokeId = d7TcapProvider.getNewInvokeId(dialogueId);
							break;
						case 2:
							System.out.print("Enter Invoke ID: ");
							try {
								choice = Integer.parseInt(bufr.readLine());
								invokeId = choice;
							} catch (Exception exception) {
								System.out.println("Exception : " + exception);
								continue;
							}
							break;
						default:
							continue;
						}
					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					break;
				} //for
				TimerResetReqEvent timerResetReqEvent = new TimerResetReqEvent(this,dialogueId,invokeId);
				d7TcapProvider.sendComponentReqEvent(timerResetReqEvent);
				break; // case 6 timer reset
			case 7: //no component
				break;
			default:
				continue;
			}
		} catch (Exception exception) {
			System.out.println("Exception: " + exception);
			continue;
		}
		break;
	} //for


	// Enter Dialogue Portion Information
	for(;;) {
		System.out.print("\n\nEnter Dialogue Portion Information: \n\n" +
				"     1) Dialogue Portion Present           \n" +
				"     2) None                               \n" +
				"\nresponse[1-2]? ");
		try {
			choice = Integer.parseInt(bufr.readLine());
			switch (choice) {
			case 1:
				dialoguePor = new DialoguePortion();
				for(;;) {
					System.out.print("\n\nProtocol Version Information: \n\n" +
							"     1) PROTOCOL VERSION ANSI 96     \n" +
							"     2) None                         \n" +
							"\nresponse[1-2]? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1:
							dialoguePor.setProtocolVersion(DialogueConstants.DP_PROTOCOL_VERSION_ANSI_96);
							break;
						case 2:
							break;
						default:
							continue;
						}
					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					break;
				} //for

				for(;;) {
					System.out.print("\n\nEnter Application Context Name Type: \n\n" +
							"     1) INTEGER                             \n" +
							"     2) OBJECT                              \n" +
							"     3) None                                \n" +
							"\nresponse[1-3]? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1:
							dialoguePor.setAppContextIdentifier(DialogueConstants.APPLICATION_CONTEXT_INTEGER);
							System.out.println("\nEnter Application Context Name");
							appContextName = get_input_bytes();
							dialoguePor.setAppContextName(appContextName);
							break;
						case 2:
							dialoguePor.setAppContextIdentifier(DialogueConstants.APPLICATION_CONTEXT_OBJECT);
							System.out.println("\nEnter Application Context Name");
							appContextName = get_input_bytes();
							dialoguePor.setAppContextName(appContextName);
							break;
						case 3:
							break;
						default:
							continue;
						}

					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					break;
				} //for

				for(;;) {
					System.out.print("\n\nEnter Security Context Type: \n\n" +
							"     1) INTEGER                     \n" +
							"     2) OBJECT                      \n" +
							"     3) None                        \n" +
							"\nresponse[1-3]? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1:
							dialoguePor.setSecurityContextIdentifier(DialogueConstants.SECURITY_CONTEXT_INTEGER);
							System.out.println("\nEnter Security Context Information");
							secContextInfo = get_input_bytes();
							dialoguePor.setSecurityContextInfo(secContextInfo);
							break;
						case 2:
							dialoguePor.setSecurityContextIdentifier(DialogueConstants.SECURITY_CONTEXT_OBJECT);
							System.out.println("\nEnter Security Context Information");
							secContextInfo = get_input_bytes();
							dialoguePor.setSecurityContextInfo(secContextInfo);
							break;
						case 3:
							break;
						default:
							continue;
						}
					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					break;
				} //for

				for(;;) {
					System.out.print("\n\nEnter Confidentiality Information: \n\n" +
							"     1) Confidentiality Info Present      \n" +
							"     2) None                              \n" +
							"\nresponse[1-2]? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1:
							System.out.println("\nEnter Confidentiality Information");
							confidentInfo = get_input_bytes();
							dialoguePor.setConfidentialityInformation(confidentInfo);
							break;
						case 2:
							break;
						default:
							continue;
						}
					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					break;
				} //for

				for(;;) {
					System.out.print("\n\nEnter User Information:  \n\n" +
							"     1) User Information Present\n" +
							"     2) None                    \n" +
							"\nresponse[1-2]? ");
					try {
						choice = Integer.parseInt(bufr.readLine());
						switch (choice) {
						case 1:
							System.out.println("\nEnter User Information");
							userInfo = get_input_bytes();
							dialoguePor.setUserInformation(userInfo);
							break;
						case 2:
							break;
						default:
							continue;
						}
					} catch (Exception exception) {
						System.out.println("Exception : " + exception);
						continue;
					}
					break;
				} //for
				break;
			case 2:
				break;
			default:
				continue;
			}
		} catch (Exception exception) {
			System.out.println("Exception : " + exception);
			continue;
		}
		break;
	} //for

	for(;;) {
		System.out.print("\n\nEnter Quality of Service:        \n\n" +
				"     1) DISCARD MSG ON ERROR            \n" +
				"     2) RETURN MSG ON ERROR             \n" +
				"     3) DISCARD MSG ON ERROR IN SEQUENCE\n" +
				"     4) RETURN MSG ON ERROR IN SEQUENCE \n" +
				"     5) None                            \n" +
				"\nresponse[1-5]? ");
		try {
			choice = Integer.parseInt(bufr.readLine());
			switch (choice) {
			case 1:
				qualityOfServ = QOS_SCCP_CLTS_NS_NR;
				break;
			case 2:
				qualityOfServ = QOS_SCCP_CLTS_NS_RE;
				break;
			case 3:
				qualityOfServ = QOS_SCCP_CLTS_S_NR_NL;
				break;
			case 4:
				qualityOfServ = QOS_SCCP_CLTS_S_RE_NL;
				break;
			case 5:
				break;
			default:
				continue;
			}
		} catch (Exception exception) {
			System.out.println("Exception : " + exception);
			continue;
		}
		break;
	} //for

	for(;;) {
		System.out.print("\n\nEnter Transaction Information: \n\n" +
				"     1) Unidirectional                \n" +
				"     2) Begin                         \n" +
				"     3) Continue                      \n" +
				"     4) End                           \n" +
				"     5) User Abort                    \n" +
				"     6) None                          \n" +
				"\nresponse[1-6]? ");
		try {
			choice = Integer.parseInt(bufr.readLine());
			switch (choice) {
			case 1:
				UnidirectionalReqEvent unidirectionalReqEvent = new UnidirectionalReqEvent(this,
										originationAddress,destinationAddress);
//				unidirectionalReqEvent.setDialogueId(dialogueId);
				if (dialoguePor != null) {
					unidirectionalReqEvent.setDialoguePortion(dialoguePor);
				}
				unidirectionalReqEvent.setQualityOfService(qualityOfServ);
				d7TcapProvider.sendDialogueReqEvent(unidirectionalReqEvent);
				statUnidirectSent++;
				break;
			case 2:
				BeginReqEvent beginRequestEvent = new BeginReqEvent(this,dialogueId,
										originationAddress,destinationAddress);
				if (dialoguePor != null) {
					beginRequestEvent.setDialoguePortion(dialoguePor);
				}
				beginRequestEvent.setQualityOfService(qualityOfServ);
				d7TcapProvider.sendDialogueReqEvent(beginRequestEvent);
				statDialoguesSent++;
				break;
			case 3:
				ContinueReqEvent continueReqEvent = new ContinueReqEvent(this,dialogueId);
				if (dialoguePor != null) {
					continueReqEvent.setDialoguePortion(dialoguePor);
				}
				continueReqEvent.setQualityOfService(qualityOfServ);
				d7TcapProvider.sendDialogueReqEvent(continueReqEvent);
				break;
			case 4:
				EndReqEvent endReqEvent = new EndReqEvent(this,dialogueId);
				if (dialoguePor != null) {
					endReqEvent.setDialoguePortion(dialoguePor);
				}
				endReqEvent.setQualityOfService(qualityOfServ);
				d7TcapProvider.sendDialogueReqEvent(endReqEvent);
				statResponsesSent++;
				break;
			case 5:
				UserAbortReqEvent userAbortReqEvent = new UserAbortReqEvent(this,dialogueId);
				
				for(;;) {
				    byte abortReason;

				    System.out.print("\n\nEnter Abort Reason:        \n\n" +
						     "     1) ACTION NOT SUPPORTED            \n" +
						     "     2) USER SPECIFIC                   \n" +
						     "\nresponse[1-2]? ");
				    try {
					choice = Integer.parseInt(bufr.readLine());
					switch (choice) {
					case 1:
					    abortReason = (byte)DialogueConstants.ABORT_REASON_ACN_NOT_SUPPORTED;
					    break;
					case 2:
					    abortReason = (byte)DialogueConstants.ABORT_REASON_USER_SPECIFIC;
					    break;
					default:
					    continue;
					}
				    } catch (Exception exception) {
					System.out.println("Exception : " + exception);
					continue;
				    }
				    break;
				} //for

				/* user specific abort cannot send a dialogue portion */
				userAbortReqEvent.setAbortReason(DialogueConstants.ABORT_REASON_ACN_NOT_SUPPORTED);

				if (dialoguePor != null) {
					userAbortReqEvent.setDialoguePortion(dialoguePor);
				}
				userAbortReqEvent.setQualityOfService(qualityOfServ);
				d7TcapProvider.sendDialogueReqEvent(userAbortReqEvent);
				break;
			case 6:
				break;
			default:
				continue;
			}
		} catch (Exception exception) {
			System.out.println("Exception : " + exception);
		}
		break;
	} //for



	msg_cnt = 0;
	msg_cnt++;
	System.out.println("Messages Sent: " + msg_cnt);
    }



    ////////////////////////////////////////////////////////////////////////////////
    // get_input_bytes()
    ////////////////////////////////////////////////////////////////////////////////
    public byte[] get_input_bytes() {
        int choice;
        int dialogueId = 0;
        int invokeId = 0;
        int msg_cnt = 0;
        int secs = 0;
        int ips = 0;
        int tpi = 0;
        int count = 0;
	int cnt = 0;
        int operType = 0;
	byte inputByte = 0;
	byte[] copyArray = new byte[100];
	byte[] inputBytes;
        String inputStr;
	char tmpChar;

	System.out.println("Enter Data as single byte hex values, end with a single period (.) " +
			"\nEx. > 08\nEx. > a1\nEx. > .\n");

	for(;;) {
		try {
			System.out.print("> ");
			inputStr = bufr.readLine();
			switch (inputStr.length()) {
			case 0:
				continue;
			case 1:
				// check for .
				if (inputStr.compareTo(".") == 0) {
					break;
				} else {
					System.out.println("String Invalid: " + inputStr);
					continue;
				}
			case 2:
				// check for digits
				tmpChar = inputStr.charAt(0);
				inputByte = 0;
				switch (tmpChar) {
				case '0': break;
				case '1': inputByte += 16; break;
				case '2': inputByte += 32; break;
				case '3': inputByte += 48; break;
				case '4': inputByte += 64; break;
				case '5': inputByte += 80; break;
				case '6': inputByte += 96; break;
				case '7': inputByte += 112; break;
				case '8': inputByte += (byte)128; break;
				case '9': inputByte += (byte)144; break;
				case 'a': case 'A': inputByte += (byte)160; break;
				case 'b': case 'B': inputByte += (byte)176; break;
				case 'c': case 'C': inputByte += (byte)192; break;
				case 'd': case 'D': inputByte += (byte)208; break;
				case 'e': case 'E': inputByte += (byte)224; break;
				case 'f': case 'F': inputByte += (byte)240; break;
				default:
					System.out.println("String Invalid: " + inputStr);
					continue;
				}
				tmpChar = inputStr.charAt(1);
				switch (tmpChar) {
				case '0': break;
				case '1': inputByte += 1; break;
				case '2': inputByte += 2; break;
				case '3': inputByte += 3; break;
				case '4': inputByte += 4; break;
				case '5': inputByte += 5; break;
				case '6': inputByte += 6; break;
				case '7': inputByte += 7; break;
				case '8': inputByte += 8; break;
				case '9': inputByte += 9; break;
				case 'a': case 'A': inputByte += 10; break;
				case 'b': case 'B': inputByte += 11; break;
				case 'c': case 'C': inputByte += 12; break;
				case 'd': case 'D': inputByte += 13; break;
				case 'e': case 'E': inputByte += 14; break;
				case 'f': case 'F': inputByte += 15; break;
				default:
					System.out.println("String Invalid: " + inputStr);
					continue;
				}
				copyArray[count] = inputByte;
				count++;
				continue;
			default:
				// bad value
				System.out.println("String Invalid: " + inputStr);
				continue;
			}
		} catch (Exception exception) {
			System.out.println("Exception : " + exception);
		}
		break;
	} // for

	inputBytes = new byte[count];
	System.arraycopy(copyArray, 0, inputBytes, 0, count);

	return inputBytes;
	} // method end

}
