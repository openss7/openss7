/*
 * S200_SignalDetectorEvent.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

package javax.telephony.media.provider;

import javax.telephony.media.*;
import javax.telephony.media.async.*;
import java.util.EventListener;

/**
 * S200_SignalDetectorEvent extends Base_SignalDetectorEvent to
 * extract info from an S.200 Payload.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public
class S200_SignalDetectorEvent extends Base_SignalDetectorEvent {
    private static Object[] faxSignalNames1 = {v_CNG, "CNG", v_CED, "CED",};
    private static Object[] faxSignalNames2 = {v_CNG, ">", v_CED, "<",};

    static {
	// setSignalNames does *both* mappings:
	Signals.setSignalNames(faxSignalNames1);// "CNG" -> v_CNG <-> "CNG"
	// so we re-assert the Symbol-to-String mapping:
	Signals.setSignalNames(faxSignalNames2);// "CNG" -> v_CNG <-> ">"
    }
    /**
     * Create specific completion or non-tranaction event from SignalDetector.
     *
     * @param source the Object that created this EventObject.
     * @param eventID a Symbol that identifies this event:
     * ev_RetrieveSignals or ev_FlushBuffer
     */
    public S200_SignalDetectorEvent(Object source, Symbol eventID) 
    {super(source, eventID);}

    /** Constructor for non-trans version of ev_FlushBuffer. */
    public S200_SignalDetectorEvent(Object source, Symbol eventID, boolean isNonTrans) {
	this(source, eventID);
	this.isNonTrans = isNonTrans;
    }

    /** the String[] retrieved from the SD Resource. */
    transient protected String[] stringArray = null;

    /** Extract and return the SD_OutputBuffer from this event. */
    public String[] getStringArray() {
	if (stringArray != null) return stringArray;
	Object outputBuffer = payload.get(ESymbol.SD_OutputBuffer);
	if (outputBuffer == null) return null;
	// For a single Signal, we might have a single String
	// instead of the usual String[]
	// Not sure if that is at variance to the S.200 spec...
	if (outputBuffer instanceof String) {
	    stringArray = new String[] { (String)outputBuffer };
	} else {
	    stringArray = (String[])outputBuffer;
	}
	return stringArray;
    }

    /** 
     * Symbol[] SignalBuffer cached here.
     * @serial 
     */
    protected Symbol[] signalBuffer = null;

    /** 
     * Get signals as a Symbol array.
     */
    public Symbol[] getSignalBuffer() {
	waitForEventDone(); 
	if ( signalBuffer != null ) return signalBuffer;
	if ( getStringArray() == null ) return null;
	int len = stringArray.length;
	signalBuffer = new Symbol[len];
	for(int i = 0; i<len; i++) {
	    Symbol signal = Signals.getSymbolFromName(stringArray[i]);
	    signalBuffer[i] = (signal != null) ? signal : v_Unknown;
	}
	return signalBuffer;
    }

    /** Stringified version of signalBuffer.
     * Possibly contains less information than signalBuffer. 
     */
    transient protected String signalString = null;

    /** 
     * Return SignalBuffer as a String of concatenated signal names.
     * Non-standard signals that have not been defined 
     * by setSignalChar() appear in the String as '?'.
     * <p>
     * As a convenience, "CNG" and "CED" are conpressed to ">" and "<"
     * 
     * @return a String representing SignalBuffer.
     */
    public String getSignalString() {
	waitForEventDone(); 
	if ( signalString != null ) return signalString;
	if ( getSignalBuffer() == null ) return null;

	int len = signalBuffer.length;
	StringBuffer sb = new StringBuffer(len);
	String str;		// generally a single char
	for (int i = 0; i<len; i++) {
	    // get the char[] for this Symbol
	    str = Signals.getSignalName(signalBuffer[i]);
	    sb.append((str != null) ? str : "?" );
	}
	signalString = sb.toString();
	return signalString;
    }

    /**
     * Stringified version of signals, with spaces between signal names. 
     */
    transient protected String spacedString = null;


    /** 
     * Return SignalBuffer as a String.
     * Non-standard signals that have not been defined 
     * by setSignalChar() appear in the String as '?'.
     * 
     * @return a String representing SignalBuffer.
     */
    public String getSpacedString() {
	waitForEventDone();
	if ( spacedString != null ) return spacedString;
	if ( getStringArray() == null) return null;

	int len = stringArray.length;
	StringBuffer sb = new StringBuffer(len+len);
	String str;		// generally a single char
	for (int i = 0; i<len; i++ ) {
	    if ( i > 0 ) sb.append( " " ); // space before other signals
	    sb.append( stringArray[i] );
	}
	spacedString = sb.toString();
	return spacedString;
    }
}
