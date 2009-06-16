/*
 * Signals.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

import java.util.Hashtable;
import javax.telephony.media.*;

/** some static stuff for handling SignalGenerator and SignalDetector.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */

public
class Signals implements SignalConstants {

    /**
     * Define the String representation for Signal Symbols.
     * Used to generate getSignalString();
     * <p>
     * Generally, we expect the String name to be a single character;
     * but this routine normalizes them all to Strings.
     * <p>
     * It is possible that a Symbol could be assigned
     * a multi-char String. That could confuse someone who 
     * expected <tt>getSignalString.charAt(n)</tt> to correspond
     * to <tt>getSignalBuffer()[n]</tt>.
     * <p>
     * Note: the published API for invoking this (p_SymbolChar)a
     * suggests that Character constants should be used; 
     * so apps should not easily trip on this.
     * <p>
     * @param sc is array of Objects of the form: {Symbol, String, ...}
     * @see SignalConstants#p_SymbolChar 
     */
    public static void setSignalNames(Object[] sc) {
	for (int i = 0; i<sc.length; i+=2) {
	    // Object[] is {Symbol, String, ... }
	    // symToSignalName is {Symbol -> String}
	    Symbol symbol = (Symbol)sc[i];
	    String string = ""+sc[i+1];	// invoke .toString() as necessary.
	    symToSignalName.put( symbol, string );
	    signalNameToSym.put( string, symbol );
	}
    }

    private static Hashtable symToSignalName = new Hashtable();
    private static Hashtable signalNameToSym = new Hashtable();

    /**
     * Retrieve the name for a signal Symbol.
     * These Strings are concatenated by getSignalString().
     * 
     * @return a String, generally a single character.
     * @see SignalDetectorEvent#getSignalString()
     */
    public static String getSignalName(Symbol sym) {
	return (String)symToSignalName.get(sym);
    }
    
    /**
     * Convert from String form to Symbol form. 
     */
    static Symbol getSymbolFromName(String str) {
	return (Symbol)signalNameToSym.get(str);
    }

    private static Object[] standardSignals = {
	v_DTMF0, "0",
	v_DTMF1, "1",
	v_DTMF2, "2",
	v_DTMF3, "3",
	v_DTMF4, "4",
	v_DTMF5, "5",
	v_DTMF6, "6",
	v_DTMF7, "7",
	v_DTMF8, "8",
	v_DTMF9, "9",
	v_DTMFA, "A",
	v_DTMFB, "B",
	v_DTMFC, "C",
	v_DTMFD, "D",
	v_DTMFStar, "*",
	v_DTMFHash, "#",
	v_CNG, ">",
	v_CED, "<",
	v_Unknown, "?",
    };
    static {
	setSignalNames(standardSignals);
	// S.200_SignalDetectorEvent adds these mappings:
	// signalNameToSym.put("CNG", SignalConstants.v_CNG);
	// signalNameToSym.put("CED", SignalConstants.v_CED);
    }
}
