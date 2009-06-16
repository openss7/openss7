/*
 * SignalDetectorConstants.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Defines the Symbols used by the SignalDetector Resource.
 * <p>
 * The Symbols that name DTMF, CNG and CED signals are inherited from
 * SignalConstants.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public 
interface SignalDetectorConstants extends ResourceConstants, SignalConstants {
    /** 
     * Denotes the standard SignalDetector Resource Class in a ResourceSpec.
     */
    Symbol v_Class		= ESymbol.SD_ResourceClass;

    /**
     * Attributes when selecting or specifying a SignalDetector resource.
     */
    
    /**
     * Attribute to request support for buffering, 
     * required to support pattern matching.
     * <p>
     * Value is a Boolean indicating if Buffering is required.
     * <br>Default value: false
     */
    Symbol a_Buffering		= ESymbol.SD_Buffering;

    /**
     * Requests capability for specific size of Signal Buffer.
     * <p>
     * Value is the Integer number of signals to buffer.
     * <br>Default value: is implementation dependent, but is at least 35. 
     */
    Symbol a_BufferSize		= ESymbol.SD_BufferSize;

    /**
     * Requests capability to automatically discard oldest
     * signals when buffer fills.
     * <p>
     * <br>Default value: vendor specific.
     */
    Symbol a_DiscardOldest	= ESymbol.SD_DiscardOldest;

    /**
     * Requests support for the 
     * Pattern detection features. Used in ConfigSpec.
     * <p>
     * Value is the Integer number of patterns supported. 
     * <br>Default value: implementation dependant, but at least 32.
     *
     * @see SignalDetectorConstants#p_Pattern
     * @see SignalDetectorConstants#p_PatternCount
     */
    Symbol a_PatternCount		= ESymbol.SD_PatternCount;

    /*
     * EventIDs returned from <code>getEventID()</code>
     */

    /**
     * FlushBuffer completion event.
     * {@link SignalDetectorEvent#getSignalBuffer getSignalBuffer} returns
     * the signals that were discarded or <code>null</code>
     * if that information is not available.
     */
    Symbol ev_FlushBuffer	= ESymbol.SD_FlushBuffer;

    /**
     * The SignalDetector signal buffer has overflowed.
     * A non-transactional event is delivered to
     * {@link SignalDetectorListener#onOverflow onOverflow}.
     * <p>
     * {@link SignalDetectorEvent#getSignalBuffer getSignalBuffer} returns
     * the signal(s) that has been discarded or <code>null</code>
     * if that information is not available.
     * @see SignalDetectorConstants#p_DiscardOldest
     */
    Symbol ev_Overflow = ESymbol.SD_Overflow;

    /**
     * Array of pattern Symbols, indicating a pattern matched event.
     * A non-transactional event with one of these eventIDs 
     * is delivered to 
     * {@link SignalDetectorListener#onPatternMatched onPatternMatched}.
     * <p>
     * The EventID indicates which of the patterns was matched.
     * {@link SignalDetectorEvent#getSignalBuffer getSignalBuffer} returns
     * the signals that matched the pattern.
     *
     * @see SignalDetectorEvent#getPatternIndex
     */
    Symbol[] ev_Pattern = {
	ESymbol.SD_Pattern0,
	ESymbol.SD_Pattern1,
	ESymbol.SD_Pattern2,
	ESymbol.SD_Pattern3,
	ESymbol.SD_Pattern4,
	ESymbol.SD_Pattern5,
	ESymbol.SD_Pattern6,
	ESymbol.SD_Pattern7,
	ESymbol.SD_Pattern8,
	ESymbol.SD_Pattern9,
	ESymbol.SD_Pattern10,
	ESymbol.SD_Pattern11,
	ESymbol.SD_Pattern12,
	ESymbol.SD_Pattern13,
	ESymbol.SD_Pattern14,
	ESymbol.SD_Pattern15,
	ESymbol.SD_Pattern16,
	ESymbol.SD_Pattern17,
	ESymbol.SD_Pattern18,
	ESymbol.SD_Pattern19,
	ESymbol.SD_Pattern20,
	ESymbol.SD_Pattern21,
	ESymbol.SD_Pattern22,
	ESymbol.SD_Pattern23,
	ESymbol.SD_Pattern24,
	ESymbol.SD_Pattern25,
	ESymbol.SD_Pattern26,
	ESymbol.SD_Pattern27,
	ESymbol.SD_Pattern28,
	ESymbol.SD_Pattern29,
	ESymbol.SD_Pattern30,
	ESymbol.SD_Pattern31
    };

    /**
     * Completion event for retrieveSignals. 
     * {@link SignalDetectorEvent#getSignalBuffer getSignalBuffer} returns
     * the retrieved signals. 
     */
    Symbol ev_RetrieveSignals	= ESymbol.SD_RetrieveSignals;
     
    /**
     * A single Signal has been detected.
     * {@link SignalDetectorEvent#getSignalBuffer getSignalBuffer()}[0] 
     * contains the signal.
     * <p>
     * <b>Note:</b> 
     * To enable this event, one should set <tt>p_EnabledEvents</tt>
     * to contain <tt>ev_SignalDetected</tt>.
     * However, some SignalDetector resources implement the S.100-R2 
     * specification, which indicates that <tt>p_EnabledEvents</tt>
     * should be set to contain <tt>Esymbol.SD_SingleSignal</tt>.
     */
    Symbol ev_SignalDetected 	= ESymbol.SD_SignalDetected;

    /**
     * Parameter names: used as keys in <code>MediaService.getParamters</code>.
     * Some are also valid as Attibute keys in a ResourceSpec.
     * <p>
     * <code>p_Buffering, p_BufferSize, p_DiscardOldest, p_Duration, 
     * p_EnabledEvents, p_Filter, p_InitialTimeout, p_InterSigTimeout,
     * p_Mode, p_Pattern[<b>i</b>], p_PatternCount</code>
     */ 


    /**
     * Enable/disable buffering of signals.
     * <p> valid values: <code>true</code> and <code>false</code>. 
     * <code>true</code> enables buffering, 
     * <code>false</code> disables buffering.
     * <br> Default value: <code>true</code>.
     * <p>
     * Pattern matching requires that this parameter be true.
     */

    Symbol p_Buffering			= ESymbol.SD_Buffering;

    /**
     * Size of the signal buffer.
     * Read-only parameter containing the number of signals that
     * can be stored in the signal buffer.
     * @see SignalDetectorConstants#a_BufferSize
     */

    Symbol p_BufferSize			= ESymbol.SD_BufferSize;

    /**
     * Boolean that indicates whether the oldest signals are
     * discarded when the signal buffer overflows.
     * <p>Valid values: <code>true</code> and <code>false</code>.
     * <br> Default value: <code>false</code>.
     */
    Symbol p_DiscardOldest		= ESymbol.SD_DiscardOldest;

    /**
     * A Dictionary of SignalGenerator event Symbols, indicating 
     * which events are generated and delivered to the application.
     * If a given eventID is not enabled, then no processing
     * is expended to generate or distribute that kind of event.
     * <p>
     * Value is a Dictionary of non-transactional event Symbols.
     * <br>Valid keys are the event Symbols:<ul>
     * <li><code>ev_Overflow</code></li>.
     * <li><code>ev_Pattern[<b>i</b>]</code></li>
     * <li><code>ev_SignalDetected</code></li>.
     * </ul>
     * <br>Valid values: <code>Boolean.TRUE, Boolean.FALSE</code>
     * <br>Default value = <i>no events</i>.
     * <p>
     * For a SignalDetectorListener to receive events, 
     * the events must be enabled by setting this parameter.  
     * This parameter can be set in a {@link ConfigSpec} or by
     * {@link MediaService#setParameters setParameters}.
     * <p>
     * <b>Note:</b>
     * This parameter controls the generation of events.
     * The events cannot be delivered unless a 
     * {@link SignalDetectorListener} is added using 
     * {@link MediaService#addMediaListener addMediaListener}. 
     */
    Symbol p_EnabledEvents		= ESymbol.SD_EnabledEvents;
    
    /**
     * Enable/disable pattern filtering.
     * <p>
     * Value of this parameter is an Array of ev_Pattern[<b>i</b>] Symbols.
     * When a Pattern in the Array is matched, the matched signals
     * are removed from the signal buffer.
     */
    Symbol p_Filter			= ESymbol.SD_Filter;

    /**
     * Maximum time duration for retrieving signals in milliseconds.
     * Stop {@link SignalDetector#retrieveSignals retrieveSignals}
     * after this time limit.
     * <p>
     * Valid values: an int [0, 100000] milliseconds or FOREVER.
     * <br>Default value: FOREVER
     */
    Symbol p_Duration			= ESymbol.SD_Duration;

    /**
     * Maximum time limit for first signal.
     * Stop {@link SignalDetector#retrieveSignals retrieveSignals}
     * if the first signal is not detected within this many milliseconds.
     * <p>
     * Valid values: an Integer [0, 100000] or FOREVER which indicates
     * no limit.
     * <br>Default value: FOREVER.
     */
    Symbol p_InitialTimeout		= ESymbol.SD_InitialTimeout;

    /**
     * Maximum time limit between consecutive signals.
     * Stop {@link SignalDetector#retrieveSignals retrieveSignals} if 
     * another signal is not detected within this many milliseconds
     * after detecting the previous signal.
     * <p>
     * Valid values: an Integer [0, 100000] or FOREVER which indicates
     * no limit.
     * <br>Default value: FOREVER.
     */
    Symbol p_InterSigTimeout		= ESymbol.SD_InterSigTimeout;

    /**
     * Enable/disable the signal detector.
     * Setting mode to <code>v_Idle</code> disables the signal detector
     * while keeping it allocated.
     * Setting mode to <code>v_Detecting</code> enable the signal detector.
     * <p>
     * <br>Caution:</br> setting this value may reset p_Pattern
     * <P>
     * Valid values: <code>v_Idle</code> or <code>v_Detecting</code>.
     * <br>Default value: <code>v_Detecting</code>
     */
    Symbol p_Mode			= ESymbol.SD_Mode;

    /** value for p_Mode. */
    Symbol v_Idle			= ESymbol.SD_Idle;
    /** value for p_Mode. */
    Symbol v_Detecting			= ESymbol.SD_Detecting;

    /**
     * Each p_Pattern[<b>i</b>] defines a pattern to be matched.
     * An array of Pattern Symbols to be used as keys in a Dictionary.
     * The associated value in the Dictionary should be a String
     * that defines the series of Signals that matches the pattern.
     * <p>
     * For each p_Pattern[<b>i</b>], the valid value is a 
     * <a href="SignalDetector.html#pattern_Strings">pattern String</a>
     * <p>
     * These parameters may be set in setParameters, 
     * or the optargs argument of 
     * {@link SignalDetector#retrieveSignals retrieveSignals}.
     */
    Symbol[] p_Pattern = ev_Pattern;

    /**
     * Number of pattern definitions supported by this SignalDetector 
     * resource.
     * For p_Pattern[<b>i</b>], <b>i</b> ranges from [0, p_PatternCount].
     * <p>
     * This is a read-only parameter.
     * The value is implementation dependent.
     * @see SignalDetectorConstants#a_PatternCount
     */
    Symbol p_PatternCount		= ESymbol.SD_PatternCount;

    /**
     * SignalDetector Symbols returned by <code>getQualifier()</code>.
     */

    /**
     * The array of Pattern Symbols which appear as qualifiers.
     * <p>
     * q_Pattern[<b>i</b>] is the qualifier in ev_RetrieveSignals when
     * retrieveSignals terminates because that pattern was matched. 
     * The contents of the buffer after the patterns were matched
     * are available
     * using {@link SignalDetectorEvent#getSignalBuffer getSignalBuffer}.
     * <p>
     * <b>Note:</b>
     * The actual signals that matched the pattern may not be in the buffer
     * due to filtering, overflow (p_DiacardOldest==FALSE), (p_Buffering==FALSE)
     */
    Symbol[] q_Pattern = p_Pattern;

    /** 
     * Complete because total duration timeout (p_Duration) exceeded.
     */
    Symbol q_Duration		= ESymbol.SD_Duration;

    /** 
     * Complete because initial signal timeout (p_InitialTimeout) exceeded.
     */
    Symbol q_InitialTimeout	= ESymbol.SD_InitialTimeout;

    /** 
     * Complete because inter signal timeout (p_InterSigTimeout) exceeded.
     */
    Symbol q_InterSigTimeout	= ESymbol.SD_InterSigTimeout;

    /** 
     * Complete because requested number of signals detected.
     */
    Symbol q_NumSignals		= ESymbol.SD_NumSignals;

    /* available RTC Actions */

    /**
     * RTC Action to cause the buffer to be flushed.
     */
    Symbol rtca_FlushBuffer = ESymbol.SD_FlushBuffer;
    
    /**
     * RTC Action to cause the current operation to stop.
     */
    Symbol rtca_Stop = ESymbol.SD_Stop;

    /**
     * SignalDetector EventIDs which are recognized as RTC conditions.
     */

    /**
     * RTC trigger when some signal has been detected.
     */
    Symbol rtcc_SignalDetected 	= ESymbol.SD_SignalDetected;
    
    /**
     * RTC trigger when buffer has been flushed.
     */
    Symbol rtcc_FlushBuffer = ESymbol.SD_FlushBuffer;
    
    /**
     * RTC trigger when completion event ev_RetrieveSignals was issued.
     */
    Symbol rtcc_RetrieveSignals = ESymbol.SD_RetrieveSignals;

    /**
     * RTC trigger when the corresponding pattern event was issued.
     * The <code>rtcc_Pattern[<b>i</b>]</code> is triggered when 
     * the corresponding <code>ev_Pattern[<b>i</b>]</code> event was issued. 
     */
    Symbol[] rtcc_Pattern = p_Pattern;
}
