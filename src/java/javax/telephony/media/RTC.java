/*
 * RTC.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
 * ECTF S.410-R2 Source code distribution.
 *
 * Copyright (c) 2002, Enterprise Computer Telephony Forum (ECTF),
 * All Rights Reserved.
 *
 * Use and redistribution of this file is subject to a License.
 * For terms and conditions see: javax/telephony/media/LICENSE.HTML
 * 
 * In short, you can use this source code if you keep and display
 * the ECTF Copyright and the License conditions. The code is supplied
 * "AS IS" and ECTF disclaims all warranties and liability.
 */

package javax.telephony.media;

import java.io.Serializable;

/**
 * An RTC (Run Time Control) object associates a trigger condition
 * with some action.
 * When the Resource identified by the trigger condition emits an event
 * that corresponds to that condition, the action command is sent
 * to the Resource whose operation is being controlled by the RTC.
 * <p>
 * The RTC trigger Symbols are a subset of the event Symbols.  
 * The events available as RTC triggers are defined 
 * by the resource that generates them.
 * <ul>
 * <li>The name of an RTC <i>trigger</i> has the form:
 * <code>rtcc_&ltEventName&gt</code>.
 * </li>
 * <li>
 * The name of an RTC <i>action</i> has the form:
 * <code>rtca_&ltCommandName&gt</code>.
 * </li></ul>
 * <p>
 * For example:<pre>
 * static RTC speedUp = new RTC(SignalDetector.rtcc_Pattern[2],
 *                                      Player.rtca_SpeedUp);
 *        RTC[] rtcs = {speedUp};
 *        playEvent = play(..., rtcs, ...);</pre>
 * In this case, when the signal detector resource 
 * matches Pattern 2, the Player resource recieves the 
 * <code>Player.rtca_speedUp</code> command.
 * <p>
 * RTC objects typically appear in media transaction commands
 * as elements of the <code>RTC[] rtc</code> argument.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public class RTC implements java.io.Serializable {
    // This is basically a read-only struct of two fields:
    /** the RTC Trigger or Condition Symbol,
     * typically named with <tt>rtcc_</tt>. */
    protected Symbol trigger;

    /** the RTC Action Symbol, typically named with <tt>rtca_</tt>. */
    protected Symbol action;

    /**
     * Create an RTC object linking the <code>trigger</code>
     * condition to the <code>action</code> command.
     * <p>
     * The Symbols used to define an RTC <i>trigger</i> are a subset
     * of the EventID Symbols.  The applicable RTC triggers are defined 
     * by the resource that generates the event.
     * <ul>
     * <li>The name of an RTC <i>trigger</i> has the form:
     * <code>rtcc_&ltEventName&gt</code>.
     * </li>
     * <li> The name of an RTC <i>action</i> has the form:
     * <code>rtca_&ltCommandName&gt</code>.
     * </li></ul>
     * 
     * @param trigger  a Symbol, typically named with <tt>rtcc_</tt>
     * @param action   a Symbol, typically named with <tt>rtca_</tt>
     */
    public RTC(Symbol trigger, Symbol action) {
	this.trigger = trigger;
	this.action = action;
    }
    public String toString() {
	String fullName = super.toString();
	String shortName = fullName.substring( fullName.lastIndexOf('.') + 1 );
	String x = "(" + trigger + "->" + action + ")";
	return shortName+x;
    }

    public int hashCode() {
	return trigger.hashCode()+action.hashCode();
    }

    /** Two RTCs are equal if both the trigger and action Symbols
     * (respectively) are equal.
     * @param other the Object to be compared to this RTC Object
     * @return true iff both RTCs have the same trigger and action Symbols.
     */
    public boolean equals(Object other) {
	return ((other instanceof RTC) && 
		((trigger == ((RTC)other).trigger) ||
		 ((trigger != null) && trigger.equals(((RTC)other).trigger))) &&
		((action == ((RTC)other).action) ||
		 ((action != null) && action.equals(((RTC)other).action))));
    }
    
    /** Return the Symbol that defines the trigger condition for this RTC.
     * @return the Symbol that defines the trigger condition for this RTC.
     */
    public Symbol getTrigger() {return trigger;}

    /** Return the Symbol that defines action for this RTC.
     * @return the Symbol that defines action for this RTC.
     */
    public Symbol getAction() {return action;}

    /**
     * The common RTC to stop a prompt when a DTMF is detected.
     */
    public final static RTC 
	SigDet_StopPlay = new RTC(SignalDetector.rtcc_SignalDetected,
				  Player.rtca_Stop);
    
    /**
     * The common RTC to stop a recording when a DTMF is detected.
     */
    public final static RTC 
	SigDet_StopRecord = new RTC(SignalDetector.rtcc_SignalDetected,
				    Recorder.rtca_Stop);

    /**
     * The RTC that implements <code>a_StopOnDisconnect</code>.
     * This RTC is enabled by default.
     * It is disabled by requesting 
     * <code>({@link ConfigSpecConstants#a_StopOnDisconnect} == FALSE)</code>.
     */
    //public final static RTC
    //Disconnect_Stop = new RTC(ESymbol.CCR_Idle, ESymbol.Group_Stop);

    /**
     * The common RTC to stop a prompt when ASR recognises voice energy.
     */
    public final static RTC 
    ASR_StopPlay = new RTC(ESymbol.ASR_SpeechDetected, ESymbol.Player_Stop);

}
