/**
 * Example code from the ECTF working group.
 */

import java.util.*;

import javax.telephony.*;
import javax.telephony.media.*;
import javax.telephony.media.container.*;

/**
 * Example answerPhone Java Telephony Media API usage
 * This example shows an application that implements 
 * an answering machine/message service.
 * <p>
 * The user interface is as follows: <br>
 * Each Address that the app runs on has a dedicated answering machine.
 * A caller is prompted to leave a message.
 * During the initial prompt the caller may enter <ul>
 * <li> a 4 digit password = "1234" : the caller enters playback mode</li>
 * <li> "*", or no input : a message is recorded.</li>
 * <li> "#" : request that answering machine hangup. </li>
 * <li> "0" : release call, with request for a live operator</li>
 * <li> other input causes the prompt to be replayed.</li>
 * </ul>
 * <b>Note:</b> a simple "operator" service can use Jtapi to transfer the call,
 * or play a message indicating no operator is available and disconnect.
 * For example, rename the @link{HelloWorld} media service to "operator".
 * <p>
 * During the Playback of messages, the following DTMF input is recognized:
 * <ul>
 * <li> "2" : pause</li>
 * <li> "3" : resume</li>
 * <li> "4" : back up, replay a short segment</li>
 * <li> "6" : skip forward</li>
 * <li> "7" : slow down</li>
 * <li> "9" : speed up</li>
 * <li> "*" : stop playback, goto next message</li>
 * <li> "#" : stop playback, skip remaining messages, goto initial prompt</li>
 * <li> "0 *" : stop playback, goto previous undeleted message</li>
 * <li> "0 0" : stop playback, repeat current message</li>
 * <li> "0 #" : stop playback, goto next message</li>
 * <li> "0 1" : stop playback, delete message, goto next message</li>
 * </ul>
 * Other DTMF input is ignored during playback.
 * <p>
 * When recording, the message is recorded until terminated
 * by max duration (default), a DTMF, or hangup by caller. 
 * The message is inserted in the msgBase Container, 
 * and caller is returned to the initial prompt.
 * <p>
 * The app uses bindToServiceName() to be notified when a call comes in.
 * This method uses the MediaProvider's implementation that detects
 * an incoming call, selects the application name to be run,
 * and delivers the call to that application. 
 * <p>
 * One thread is created for each channel or port that
 * can be playing simultaneously. The AnswerPhone is started
 * on each thread and runs forever (doing a new bindToServiceName()
 * immediately after each call is completed.)  
 * <p>
 */
public class 
    AnswerPhone extends BasicMediaService implements Runnable {

    /* set up semantically meaningful local names for the Pattern Symbols*/
    static final Symbol prev   = SignalDetector.p_Pattern[1];
    static final Symbol replay = SignalDetector.p_Pattern[2];
    static final Symbol next   = SignalDetector.p_Pattern[3];
    static final Symbol delm   = SignalDetector.p_Pattern[4];

    static final Symbol passwd = SignalDetector.p_Pattern[6];
    static final Symbol oper   = SignalDetector.p_Pattern[7];

    static final Symbol stop   = SignalDetector.p_Pattern[10];
    static final Symbol drop   = SignalDetector.p_Pattern[11];

    static final Symbol slow   = SignalDetector.p_Pattern[13];
    static final Symbol resume = SignalDetector.p_Pattern[14];
    static final Symbol fast   = SignalDetector.p_Pattern[15];
    static final Symbol back   = SignalDetector.p_Pattern[16];
    static final Symbol pause  = SignalDetector.p_Pattern[17];
    static final Symbol forw   = SignalDetector.p_Pattern[18];
    
    /**
     * define patterns for signal detector
     */
    static Hashtable patterns = new Hashtable();
    static {
	patterns.put(passwd, "{4}?");	// greeting: passwd implies get messages
	patterns.put(oper,   "0"); 	// greeting: release to "operator"
	patterns.put(stop,   "*");	// greeting: stop, goto record message
			    	  	// playback: stop, get next message
				  	// keep bashing # to get up, out, off...
	patterns.put(drop,   "#");	// greeting: stop, hangup
					// playback: stop, skip rest of messages

	patterns.put(prev,  "0 *");	// playback stop, back to previous message
	patterns.put(replay,"0 0");	// playback stop, replay current message
	patterns.put(next,  "0 #");	// playback stop, skip to next message
	patterns.put(delm,  "0 1");	// playback stop, delete current message

	// these are filtered:
	patterns.put(pause,  "2");	// RTC: pause
	patterns.put(resume, "3");	// RTC: resume
	patterns.put(back,   "4");	// RTC: jumpbackward
	patterns.put(forw,   "6");	// RTC: jumpforward
	patterns.put(slow,   "7");	// RTC: speeddown
	patterns.put(fast,   "9");	// RTC: speedup
    }

    // remove these control signals from signal detector buffer: 
    static Symbol[] filtersOn = { pause, resume, back, forw, slow, fast };

    static Hashtable filterOn = new Hashtable();
    static {
	filterOn.put(SignalDetector.p_Filter, filtersOn);
    }

    static Symbol[] filtersOff = new Symbol[0]; 
    static Hashtable filterOff = new Hashtable();
    static {
	filterOff.put(SignalDetector.p_Filter, filtersOff);
    }
    
    /** get initial user service request */
    static Symbol[] DTMFRequest = { stop, drop, passwd, oper };

    /** An RTC array for playing the prompt. stop if a DTMF is detected. */
    static RTC[] promptRTC = { RTC.SigDet_StopPlay };
    
    /** An RTC array for controlling the playing of messages. */
    static RTC[] messageRTC = {new RTC(stop, Player.rtca_Stop),
			       new RTC(replay, Player.rtca_Stop),
			       new RTC(drop, Player.rtca_Stop),
			       new RTC(delm, Player.rtca_Stop),
			       new RTC(prev, Player.rtca_Stop),
			       new RTC(next, Player.rtca_Stop),

			       new RTC(forw, Player.rtca_JumpForwardTime),
			       new RTC(back, Player.rtca_JumpBackwardTime),
			       new RTC(fast, Player.rtca_SpeedUp),
			       new RTC(slow, Player.rtca_SpeedDown),
			       new RTC(pause, Player.rtca_Pause),
			       new RTC(resume, Player.rtca_Resume)};

    // could setParameters(params) if the defaults are not sufficient:
    static Hashtable params = new Hashtable();
    static {
	params.put(Player.p_JumpTime, new Integer(5000));
	params.put(SignalDetector.p_InitialTimeout, new Integer(5000));
    }
    
    /** initial prompt as an array of MDOs. {Welcome, LeaveAMessage}*/
    static java.lang.String[] mainWelcome = {"Widgitek/Welcome",
				   "Widgitek/LeaveAMessage"};

    /** Tell user they botched the password. */
    static java.lang.String errMsg = "Widgitek/InvalidCode";

    /** Play this between each message during playback. */
    static java.lang.String msgSeparator = "Widgitek/Beep";

    /** Play this before recording. */
    static java.lang.String msgRecord = "Widgitek/Beep";

    /** Play this to confirm message delete. */
    static java.lang.String msgDeleted = "Widgitek/Deleted";
    
    /** Record Messages to this Container. */
    static java.lang.String msgBase = "Widgitek/Messages/";
    
    /**
     * Run four threads of AnswerPhone service.
     * <p>
     * Start 4 threads to provide the service...
     * Each service is an instance of AnswerPhone.
     * AnswerPhone implements the Runnable interface and 
     * the <code>run()</code> method is invoked when
     * the thread is started.
     * <p>
     * @usage java AnswerPhone
     */
    public static void main(java.lang.String[] argv) throws Exception {
	Symbol.loadSymbolNames(null);
	// new Thread(new AnswerPhone()).start();
	for (int i = 0; i < 4; i++) {
	    // assume default Provider of default Peer is a MediaProvider
	    new Thread(new AnswerPhone()).start();
	}
    }
    
    /** A ContainerService so we can list/delete messages. */
    ContainerService cs;

    /** Default constructor, advised to get ContainerService. */
    public AnswerPhone() {
	super();
	// throw if MediaProvider is not a ContainerProvider:
	ContainerProvider cp = (ContainerProvider)getMediaProvider();
	cs = cp.getContainerService();
    }

    /** 
     * The real work starts here...
     * This method is called when the thread is started.
     */
    public void run() {
	// this is NOT a RunnableMediaService
	// a RunnableMediaService does one call and returns.
	while (true)
	    try {
		bindToServiceName(ConfigSpec.basicConfig, "AnswerPhone");
		userRequest("1234"); // userRequest will release()
	    } catch (Exception ex) {
		System.err.println(ex);
	    }
    }
    
    /* various events used in userRequest */
    SignalDetectorEvent sdev;
    RecorderEvent rev;
    PlayerEvent pev;
    Symbol qual;
    Symbol rtcc;

    /** Run the toplevel user-interaction loop, then release. */
    void userRequest(java.lang.String password) {
	setParameters(patterns); // a Dictionary of Pattern parameters
	try {
	    // collect input until 'break', return, Disconnect, Exception
	    while (true) {	
		flushBuffer();
		// play the welcome...
		pev = play(mainWelcome, 0, promptRTC, null);
		// stop when DTMF or play done.
		qual = pev.getQualifier();
		if (!qual.equals(pev.q_EndOfData)) {
		    // retrieve a DTMF (assumes reasonable timeout defaults)
		    sdev = retrieveSignals(-1, DTMFRequest, null, null);
		    qual = sdev.getQualifier();
		}
		if (qual.equals(stop) || (qual.equals(pev.q_EndOfData))) {
		    recordNewMessage();
		} else if ((qual.equals(passwd)) ||
			   (qual.equals(SignalDetector.q_NumSignals))) {
		    if (sdev.getSignalString().equals(password)) {
			playbackMessages(); // password OK
		    } else {
			play(errMsg, 0, null, null); // Play an error message
		    }
		} else if (qual.equals(oper)) {
		    System.out.println("Release to oper");
		    releaseToService("operator", 9000); // release to "operator"
		    return;
		} else if (qual.equals(drop)) {
		    // User asked for drop:
		    // releaseToDestroy();
		    releaseToDestroy();
		    break;
		}
	    }
	}
	catch (DisconnectedException ex) {
	    // Caller gone, nothing else to do.
	}
	catch (Exception ex) {
	    // un-expected Exception
	    System.err.println(ex);
	    ex.printStackTrace(System.err);
	    System.err.println("releasing call now:"); // Sorry...
	}
	try {
	    if (isBound()) release();
	} catch (MediaBindException ex) {
	    System.err.println("User Request: " + ex);
	    // stack trace is not interesting at this point...
	}
	return;
    }

    /** Play all the user's message MDOs sequentially...
     * <p>
     * Delete if interrupted with 'delete' RTC.
     */
    private void playbackMessages() throws Exception  {
	java.lang.String[] message = {msgSeparator, msgSeparator}; // beep + msg
	// setParameters(patterns); // a Dictionary of Pattern parameters
	setParameters(filterOn);

	java.lang.Object[] msgs = cs.listObjects(msgBase, null);
	if (msgs.length == 0) return; // no messages
	// else sort by message number:
	Arrays.sort(msgs, mcomp);

	for (int i = 0; i<msgs.length; i++) {
	    message[1] = msgBase+(java.lang.String)msgs[i];
	    // if this is a real message, play it until we 'break'
	    while (msgs[i] != null) { // play this message, until user is done
		flushBuffer();
		System.out.println("Playing "+message[1]);
		pev = play(message, 0, messageRTC, null);
		qual = pev.getQualifier();
		rtcc = pev.getRTCTrigger();

		// why did we complete?
		if (qual.equals(pev.q_EndOfData)) break; // do next message
		else if (qual.equals(pev.q_RTC)) {
		    // which RTC was it?
		    if (rtcc.equals(replay)) continue; // play again
		    else if (rtcc.equals(stop)) break; // next message
		    else if (rtcc.equals(next)) break; // next message
		    else if (rtcc.equals(drop)) return; // to main menu
		    else if (rtcc.equals(delm)) { // delete message
			System.out.println("Delete "+message[1]);
			cs.destroy(message[1], null); // remove MDO
			play(msgDeleted, 0, null, null); // confirm to user
			msgs[i] = null;	// mark as deleted
			break;		// next message
		    }
		    else if (rtcc.equals(prev)) {
			// backup to undeleted msg
			for ( ; ((--i > 0) && (msgs[i] == null)) ; );
			i -= 1;	// outer 'for' will re-increment
			break;	// exit while; continue with for(i++)
		    }
		} 
	    }
	}
	setParameters(filterOff);
	return;
    }
    
    /**
     *  Record a message into the msgBase Container.
     */
    private void recordNewMessage() throws Exception {
	// create a new unique MDO identifier, in this Container:
	java.lang.String mdo = newMsgName(msgBase);
	System.out.println("recordNewMessage: "+mdo);
	
	RTC[] recordRTC = {RTC.SigDet_StopRecord};
	
	try {
	    play(msgRecord, 0, null, null); // a beep
	    rev = record(mdo, recordRTC, null);
	} catch (DisconnectedException ex) {
	    // disconnect is expected, message is saved.
	}
    }

    /** Create a MDO name of the form: "base/msg-n".
     * where "n" is the next number not already used.
     * @param base Container name where messages are stored.
     * @throws ContainerException if <tt>listObjects(base)</tt> fails
     * @return a java.lang.String MDO name in the base container.
     */
    java.lang.String newMsgName(java.lang.String base) throws ContainerException {
	java.lang.String msgid = "msg-1";
	java.lang.Object[] msgs = cs.listObjects(base, null);
	if (msgs.length > 0) {
	    // find largest message number and increment:
	    Arrays.sort(msgs, mcomp);
	    java.lang.String mx = (java.lang.String)msgs[msgs.length-1];
	    int x = mx.indexOf("-")+1;
	    int last = Integer.parseInt(mx.substring(x));
	    msgid = mx.substring(0, x) + (last+1);
	}
	return base+msgid;
    }

    /** a comparator that works with messages named by newMsgName */
    static final Comparator mcomp = new Comparator() {
	    public int compare(java.lang.Object m1, java.lang.Object m2) {
		int x1 = ((java.lang.String)m1).indexOf("-")+1;
		int x2 = ((java.lang.String)m2).indexOf("-")+1;
		// Integer.compareTo(...)
		return (Integer.parseInt(((java.lang.String)m1).substring(x1))
			-
			Integer.parseInt(((java.lang.String)m2).substring(x2))
			);
	    }
	};
}
