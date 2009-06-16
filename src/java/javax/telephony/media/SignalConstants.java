/*
 * SignalConstants.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

/**
 * SignalConstants supplies the names of the DTMF signals.
 * <p>
 * These Symbols are used by SignalGenerator and SignalDetector.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public 
interface SignalConstants {

    /** Symbol for DTMF 0. */
    Symbol v_DTMF0		= ESymbol.SG_DTMF0;
    /** Symbol for DTMF 1. */
    Symbol v_DTMF1		= ESymbol.SG_DTMF1;
    /** Symbol for DTMF 2. */
    Symbol v_DTMF2		= ESymbol.SG_DTMF2;
    /** Symbol for DTMF 3. */
    Symbol v_DTMF3		= ESymbol.SG_DTMF3;
    /** Symbol for DTMF 4. */
    Symbol v_DTMF4		= ESymbol.SG_DTMF4;
    /** Symbol for DTMF 5. */
    Symbol v_DTMF5		= ESymbol.SG_DTMF5;
    /** Symbol for DTMF 6. */
    Symbol v_DTMF6		= ESymbol.SG_DTMF6;
    /** Symbol for DTMF 7. */
    Symbol v_DTMF7		= ESymbol.SG_DTMF7;
    /** Symbol for DTMF 8. */
    Symbol v_DTMF8		= ESymbol.SG_DTMF8;
    /** Symbol for DTMF 9. */
    Symbol v_DTMF9		= ESymbol.SG_DTMF9;
    /** Symbol for DTMF A. */
    Symbol v_DTMFA		= ESymbol.SG_DTMFA;
    /** Symbol for DTMF B. */
    Symbol v_DTMFB		= ESymbol.SG_DTMFB;
    /** Symbol for DTMF C. */
    Symbol v_DTMFC		= ESymbol.SG_DTMFC;
    /** Symbol for DTMF D. */
    Symbol v_DTMFD		= ESymbol.SG_DTMFD;
    
    /** Symbol for DTMF *. */
    Symbol v_DTMFStar		= ESymbol.SG_DTMFStar;
    /** Symbol for DTMF #. */
    Symbol v_DTMFHash		= ESymbol.SG_DTMFHash;
    
    /** Symbol for Fax-calling tone CNG. */
    Symbol v_CNG		= ESymbol.SG_CNG;
    /** Symbol for Fax-called tone CED. */
    Symbol v_CED		= ESymbol.SG_CED;

    /** Symbol for unknown/unregistered Signals. 
     * Use <tt>getSignalStrings()</tt> to retrieve the SignalName.
     * <p>
     * Use <tt>p_SymbolChar</tt> to define a Signal Symbol.
     */
    Symbol v_Unknown		= ESymbol.Error_BadSignalID;

    /**
     * Specifies the chars to use when non standard Signals
     * are converted to a String or vice versa.
     * The associated value is a Object[]. 
     * Even numbered elements are a Symbol, 
     * odd numbered elements are the associated Character.
     * <p>
     * For example, The standard values could be represented like:
     * <pre>
     * Object[] v_StandardDTMFChars = { 
     *     v_DTMF0, new Character('0'),
     *     v_DTMF1, new Character('1'),
     *     v_DTMF2, new Character('2'),
     *     v_DTMF3, new Character('3'),
     *     // ...
     *     };
     * </pre>
     */
    Symbol p_SymbolChar		= ESymbol.SG_SymbolChar;

}
