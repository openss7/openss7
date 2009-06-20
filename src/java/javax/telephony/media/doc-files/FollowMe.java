/**
 * Example code from the ECTF working group.
 */

import java.util.*;

import javax.telephony.*;
import javax.telephony.media.*;
import javax.telephony.callcontrol.*;


/**
 * FollowMe application.
 * <p>
 * This example application implements a FollowMe service, 
 * using the JTAPI Media API package in conjunction with 
 * the JTAPI core and CallControl packages.
 * <p>
 * The user interface is as follows: <br>
 * If a subscriber does not answer an incoming call, 
 * the caller is prompted to either leave a message 
 * or wait for the subscriber to be contacted.
 * If the caller chooses to wait, 
 * they must enter their telephone number for identification, 
 * and they get music while the subscriber is paged,
 * with the caller's ID included in the page.
 * <p>
 * If the subscriber chooses to answer, 
 * he/she calls a standard number for the FollowMe service, 
 * is prompted for a password, and then is joined to the waiting call.  
 * <p>
 * If the caller has dropped by this time, 
 * a message is left in the subscriber's mailbox.
 * <p>
 * The service is used in conjunction with a messaging application,
 * which is assumed to be independently developed, but  with the following
 * external interface:
 * <ul>
 * <li>service name: LeaveMessage prompts caller to leave a message, 
 * 			and saves it subscribers mailbox</li>
 * <li>service name: Login - allows subscriber to retrieve messages, etc.</li>
 * </ul>
 * <p>
 * The System is configured as follows: <br>
 * All subscribers' lines are administered to forward on "ring-no answer"
 * to DN XXXX, which is administratively assigned to route to the FollowMe
 * service name. 
 * FollowMe.main() starts up an arbitrary number of threads (5),
 * each waiting for an incoming call (forwarded from the subscriber address)
 * or for a page answer from the subscriber.
 * If a redirecting number exists, then FollowMe knows the call 
 * was forwarded, otherwise it must be from a subscriber.
 * <p>
 * One thread is created for each channel or port that
 * can be playing simultaneously.  FollowMe is started
 * on each thread and runs forever, doing a new bindToServiceName()
 * immediately after each call is completed.
 * <p>
 */
public class FollowMe extends BasicMediaService implements Runnable {
    Hashtable waitingCalls = new Hashtable(5);
    /**
     * An RTC array for playing the prompt.
     *  stop if a DTMF is detected.
     */
    static RTC[] promptRTC = { RTC.SigDet_StopPlay };
    
    /* Jtapi Provider */
    static Provider myProvider;
    
    /** Standard constructor */
    public FollowMe(MediaProvider prov) {super(prov);}
    
    /**
     * The method which is invoked when this aplication is started.
     * <p>
     * @usage java FollowMe [peerName] [provString]
     */
    public static void main(java.lang.String[] argv) throws Exception {
	// create a jtapi peer and jtapi media provider.
	java.lang.String peerName = "DefaultJtapiPeer";
	if (argv.length > 0) peerName = argv[0];
	java.lang.String provString = null;
	if (argv.length > 1) provString = argv[1];
	
	JtapiPeer myPeer = JtapiPeerFactory.getJtapiPeer(peerName);
	myProvider = myPeer.getProvider(provString);
	
	/** now kick off 5 threads to provide the service...
	 * Each service is an instance of FollowMe to which
	 * we pass the provider object in the constructor.
	 * The object supports the Runnable interface and
	 * the <code>run()</code> method is invoked when
	 * the thread is started.
	 */
	for (int i = 0; i < 5; i++) {
	    //assuming that the requested Provider is a MediaProvider
	    new Thread(new FollowMe((MediaProvider)myProvider)).start();
	}
    }
    
    /* The TerminalConnection array associated with mediaTerminal,
     * there should be only one terminal  connection returned.
     */
    TerminalConnection myTermConn[];
    /* The CallControlCall object for ithe incoming call... */
    CallControlCall myCall;

     /** The real work starts here...
      * This method is called when the thread is started.
      */
     public void run() {
	 while (true) {
	     try {
		 /* bind to an answered FollowMe call */
		 bindToServiceName(ConfigSpec.basicConfig, "FollowMe");
		 Terminal myTerm = getTerminal();

		 myTermConn = myTerm.getTerminalConnections();
		 Connection myConn = myTermConn[0].getConnection();
		 myCall = (CallControlCall)myConn.getCall();
		 Address redirectingNr = myCall.getLastRedirectedAddress();
		 
		 if (redirectingNr == null) 
		     callFromSubscriber();
		 else 
		     callForSubscriber();
	     } catch (Exception ex) { // something is wrong...
		 System.err.println(ex); 
	     }
	     if(isBound()) try {
		 releaseToService("Operator", 5000); // give to operator
	     } catch (Exception ex2) {
		 if (isBound()) try {
		     release(); 	    // ok, let framework handle it...
		 } catch (Exception ex3) {} // should not happen.
	     }
	 }
     }

     /** we have a direct call from a presumed subscriber */
     void callFromSubscriber() throws Exception {
	 java.lang.String subscriberID = validateSubscriber();
	 if (subscriberID == null) {
	     release();	/* drop call - not from a valid subscriber */
	     return;
	 }
	 
	 /* merge the Calls: otherTermConn and myTermConn
	  * otherTermConn: IVR Terminal-1 connected to caller
	  *    myTermConn: IVR Terminal-2 connected to subscriber.
	  * If caller is no longer present, 
	  * route subscriber to the message application.
	  */
	 TerminalConnection otherTermConn;
	 otherTermConn = (TerminalConnection)waitingCalls.get(subscriberID);
	 if (otherTermConn!=null) {
	     transferDisjointCalls(otherTermConn, myTermConn[0]);
	     release();
	 } else {
	     releaseToService("Login", 5000);
	 }
     }

    /**
     * A caller has been forwarded to the followMe service.
     * Ask whether the caller desires to leave a message or wait.
     * If wait, then collect the callers ID and page the subscriber,
     * (The page message will identify FollowMe call with "**"
     * and include subscribers ID).
     */
    void callForSubscriber() throws Exception {
	try {
	    if (!wantToWait()) {
		releaseToService("LeaveMessage", 0);
		return;
	    }  
	} catch (DisconnectedException ex) {
	    release();
	    return;
	}
	    
	// subscriberID is the called number
	java.lang.String subscriberID = myCall.getCalledAddress().getName();
	java.lang.String callerID = getCallerID(); // let caller express themselves
	try {
	    /* now save the subscriberID in "waitingCall" Hashtable.
	     * mapping subscriberID -> this.myTermConn
	     * We will connect subscriber to myTermConn when he calls in.
	     * [bug: if a someone else is already waiting for subscriber!]
	     */
	    waitingCalls.put(subscriberID, myTermConn);
	    sendpage(subscriberID, "**" + callerID);
	    music();		// play music until connected to subscriber.
	} catch (DisconnectedException ex) {
	    // OK: either caller gave up 
	    // or caller was connected to the subscriber.
	    release();
	} finally {
	    waitingCalls.remove(subscriberID);
	}
    }

    /** 
     * Merge two calls that have no terminal in common.
     * <p>
     * This requires placing a new call between the IVR terminals,
     * then transferring Caller and subscriber to the new call. 
     * since each call has a terminal in common with the new call, 
     * it is legal to do these transfers.
     * <p>
     * Note: Both TermConns must be CallControlTerminalConnections.
     *
     * @param TC1 the TermConn to caller's Call 
     * @param TC2 the TermConn to subscriber's Call 
     */
    void transferDisjointCalls(TerminalConnection TC1,
			       TerminalConnection TC2)
	throws Exception {
	    CallControlTerminalConnection callerTC = 
		(CallControlTerminalConnection) TC1;
	    CallControlTerminalConnection subscriberTC =
		(CallControlTerminalConnection) TC2;
	    
	    /* create new call */
	    CallControlCall newCall = (CallControlCall)myProvider.createCall();

	    /* put the caller IVR TerminalConnection on Hold */
	    callerTC.hold();
	    /* place call from caller IVR Terminal to subscriber IVR Terminal */
	    Connection IVRConns[] = ((Call)newCall).connect(
			callerTC.getTerminal(),
			callerTC.getConnection().getAddress(),
			subscriberTC.getConnection().getAddress().getName());
	    
	    /* put the subscriber IVR TerminalConnection on Hold */
	    subscriberTC.hold();

	    /* answer the new call at the subscribers IVR term conn. 
	     * Note IVRConns[1] is the destination connection
	     * Note IVR Terminals have only one connection, connection[0]
	     */
	    IVRConns[1].getTerminalConnections()[0].answer();
	    
	    /* transfer caller to the new call 
	     * (original caller call and its IVR Terminal are dropped)
	     */
	    newCall.transfer(callerTC.getConnection().getCall());
	    
	    /* transfer subscriber to the new call 
	     * (original subscriber call and its IVR Terminal are dropped). 
	     * The newcall has Connections to the caller and the subscriber.
	     */
	    newCall.transfer(subscriberTC.getConnection().getCall());
	}
    
    boolean wantToWait() throws Exception {
	java.lang.String input = promptForInput("Press 1 to Leave Message, 2 to Wait",1);
	if (input.equals("2"))
	    return true;
	else
	    return false;
    }

    /**
     * Prompt for user input, reprompt if necessary, return input.
     * This could be better...
     */
    java.lang.String promptForInput(java.lang.String prompt, int inputLength) throws Exception {
	while (true) {
	    // play the prompt
	    PlayerEvent pev = play(prompt, 0, promptRTC, null);
	    // stop when DTMF or play done.
	    
	    // retrieve a DTMF (assumes reasonable timeout defaults)
	    SignalDetectorEvent sdev;
	    sdev = retrieveSignals(inputLength, null, null,null);
	    
	    if (sdev.getQualifier().equals(sdev.q_NumSignals)) {
		return (sdev.getSignalString());
	    } else {
		// Play an error message:  [interesting file name, eh?]
		pev = play("Invalid Input, try again", 0, null, null); 
		// loop to try again.
	    }
	}
    }



    /**
     * Send page to a subscriber identified by their regular telephone number. 
     * Requires looking up the pager number, and sending a page to that number. 
     * This could be done by originating a call and sending DTMF digits,
     * or by a digital message to the paging service. 
     * Details left to the reader.
     *
     * @param subscriberID a java.lang.String representation of the subscribers Address
     * @param message a java.lang.String to be displayed by the page.
     */
    void sendpage(java.lang.String subscriberID, java.lang.String message) {
	// TBD
    }
    
    /**
     * prompt subscriber for subscriber ID and password, check for validity,
     * and return subscriberID if the ID and password are valid.
     */
    java.lang.String validateSubscriber() throws Exception {
	java.lang.String subscriberID = promptForInput("Enter subscriberID", 10);
	java.lang.String passwd = promptForInput("Enter your password", 4);
	// check your database, validate password.
	// if (!valid) return null;
	return subscriberID;
    }
    
    /** Prompt caller for their ID (telephone Nr), and return it. */
    java.lang.String getCallerID() throws Exception {
	return promptForInput("Enter your 10 digit telephone number", 10);
    }
    
    /**
      * Play music indefinitely.
      * Better implementations would listen for DTMF input while playing...
      * DisconnectedException (a RuntimeException) *will* be thrown.
      */
    void music() {
	while (true) try {
	    play("muzak", 0, null, null);
	} catch (MediaResourceException mre) {
	    System.err.println(mre.toString());
	}
    }
}
