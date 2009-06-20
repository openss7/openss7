/**
 * Example code from the ECTF working group.
 */

import javax.telephony.*;
import javax.telephony.media.*;
import javax.telephony.events.*;

/* Incoming Telephone Call Example:

   This example application demonstrates using JTAPI to 
   answer calls to a particular Terminal.
   It shows how to add a Recorder onto a call when it is answered.
   
   A Desktop application to allow an agent to answer a call 
   and record the conversation could use this logic.
   The recording resource could be on a PBX, 
   on an adjunct IVR connected to the PBX, 
   or on a desktop PC, depending on how the MediaProvider is implemented.
   
   This code is the same as the core InCall code example, with the addition
   of the recordCall() method which is invoked after answer.
   */

/**
 * Create a provider and monitor a particular terminal for an incoming call.
 * <p>
 * usage: java Incall "terminal_name"
 * example: java Incall "4761111"
 */
public class InCall {
  public static final void main(java.lang.String argv[]) {
      /*
       * Create a provider by first obtaining the default implementation of
       * JTAPI and then the default provider of that implementation.
       */
      Provider myprovider = null;
      try {
	  JtapiPeer peer = JtapiPeerFactory.getJtapiPeer(null);
	  myprovider = peer.getProvider(null);
      } catch (Exception excp) {
	  System.err.println("Can't get Provider. \n" + excp);
	  System.exit(0);
      }
      /*
       * Get the terminal we wish to monitor and add a call observer to that
       * Terminal. This will place a call observer on all calls which come to
       * that terminal. We are assuming that Terminals are named after some
       * primary telephone number on them.
       */
      try {
	  Terminal terminal = myprovider.getTerminal(argv[0]);
	  terminal.addCallObserver(new MyCallObserver());
      } catch (Exception excp) {
	  System.err.println("Can't get Terminal: " + argv[0]);
	  System.err.println(excp);
	  System.exit(0);
      }
  }
  
  /**
   * nested class MyCallObserver implements the CallObserver 
   * and receives all Call-related events.
   * Look for TermConnRingingEv and answer that TerminalConnection.
   */
  static class MyCallObserver implements CallObserver {
      public void callChangedEvent(CallEv[] evlist) {
	  for (int i = 0; i < evlist.length; i++) {
	      if (evlist[i] instanceof TermConnEv) {
		  if (evlist[i].getID() == TermConnRingingEv.ID) {
		      try {
			  TerminalConnection termconn;
			  TermConnEv tcev = (TermConnEv)evlist[i];
			  termconn = tcev.getTerminalConnection();
			  Call call = tcev.getCall();
			  termconn.answer();
			  recordCall(call);
		      } catch (Exception excp) {
			  // Handle Exceptions!
			  System.err.println(excp);
		      }}}}}
  }
  
  /** create a Thread to:
   * attach a BasicMediaService with a Recorder, 
   * and record the call until it is disconnected.
   */
  static void recordCall(Call call) throws Exception {
      new Thread(new CallRecorder(call)).start();	    
  }
  
  /** 
   * Attach a Recorder to the given Call and record everything.
   * CallRecorder.run() can/should be started in a Thread.
   */
  static class CallRecorder extends BasicMediaService implements Runnable {
      Call call;
      CallRecorder(Call call) {
	  super((MediaProvider)call.getProvider());
	  this.call = call;
      }
      /* a ConfigSpec that specifies a Recorder only */
      static ResourceSpec[] aRecorder = {ResourceSpec.basicRecorder};
      static ConfigSpec mycs = new ConfigSpec(aRecorder, 2000, null, null,null);
      
      /* invoked by Thread.start() */
      public void run() {
	  try {
	      bindToCall(mycs, call);
	      /* Start Recording; terminate when the maximum timelimit
	       * is reached or the conection is disconnected.
	       * [you can set the timelimit in the ConfigSpec or optargs]
	       */
	      record("recordFile", null, null);
	  } catch (Exception ex) { // bind or record has failed
	      System.err.println(ex);
	  }
	  if (isBound()) try {
	      release(); 	     // ok, let framework handle it...
	  } catch (Exception ex3) {} // should not happen.
      }
  }
}
