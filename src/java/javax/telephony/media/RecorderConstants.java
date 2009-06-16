/*
 * RecorderConstants.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Defines the Symbols used by the Recorder Resource.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public 
interface RecorderConstants extends ResourceConstants, CoderConstants {
    /** 
     * Denotes the standard Recorder Resource Class in a ResourceSpec.
     */
    Symbol v_Class		= ESymbol.Recorder_ResourceClass;

    /**
     * Boolean value indicates if Recorder supports the Beep features.
     * <ul>
     * <li>p_StartBeep</li>
     * <li>p_BeepLength</li>
     * <li>p_BeepFreq</li>
     * </ul> 
     */
    Symbol a_Beep		= ESymbol.Recorder_Beep;
    
    /** a_FixedBeep indicates that Recorder supports p_StartBeep. */
    Symbol a_FixedBeep		= ESymbol.Recorder_FixedBeep;

    /** Recorder supports Pause and Resume operations.
     * Including rtca_Pause, rtca_Resume, and p_StartPaused.
     */
    Symbol a_Pause		= ESymbol.Recorder_Pause;

    /**
     * Attribute to request support for various coders.
     * Value is a Symbol or Symbol[] that identifies the required coder(s).
     * <p>
     * The Symbols that identify coders are in {@link CoderConstants}.
     */
    Symbol a_CoderTypes		= ESymbol.Recorder_Coder;
    // changed this name, so attribute and parameter have same name.
    // Recorder_ECTF_Coder is an anachronism, before attribute has array value.

    /**********************************************************************/
    /* event constants */

    /**
     * If the Recorder does not support the indicated Coder, 
     * <code>record()</code> throws a <code>MediaResourceException</code>, 
     * indicating <code>e_Coder</code>.
     */
    Symbol e_Coder		= ESymbol.Error_Coder;

    /**
     * Indicates that recording has been completed.
     * Possible qualifiers are:
     * <code>q_RTC, q_Stop, q_Duration, q_Silence</code>
     */
    Symbol ev_Record		= ESymbol.Recorder_Record;

    /** Indicates that recording has been paused by RTC or pauseRecorder. */
    Symbol ev_Pause		= ESymbol.Recorder_Pause;    

    /** Indicates that recording has been resumed by RTC or resumeRecorder. */
    Symbol ev_Resume		= ESymbol.Recorder_Resume;    
    
    /** Indicates that recording has been stopped by RTC or stopRecorder. */
    Symbol ev_Stop		= ESymbol.Recorder_Stop;    

    /***********************************************************************/
    /**
     * Parameters that can be set:
     * Append,
     * BeepFrequency, BeepLength, Coder, CoderTypes,
     * EnabledEvents, MaxDuration, MinDuration,
     * SilenceTruncationOn, SilenceTruncationThreshold, 
     * SilenceTerminationThreshold, SilenceTerminationOn, 
     * StartBeep, StartPaused.
     */

    /**
     * Indicates that recording should append to the end of an existing MDO
     * rather than overwrite it.
     * <br> Default: TRUE
     */
    Symbol p_Append		= ESymbol.Recorder_Append;

    /**
     * The frequency of the start beep; iff attribute a_Beep.
     * <br>Default: 1500
     * <br>Range: 0 to 5000
     */
    Symbol p_BeepFrequency	= ESymbol.Recorder_BeepFrequency;

    /**
     * Length of Beep preceeding recording; iff attribute a_Beep.
     * <br>Default: 50
     * <br>Range: 0 to 100
     */
    Symbol p_BeepLength		= ESymbol.Recorder_BeepLength;

    /**
     * Symbol identifying the Coder used for a new recording.
     * <br>Default: implementation dependent.
     * <p>
     * For list of Coder type Symbols, 
     * @see CoderConstants
     */
    Symbol p_Coder		= ESymbol.Recorder_Coder;

    /**
     * An array of Symbols identifying the Coders supported by this Recorder.
     * <br>Default: implementation dependent.
     * <br>Read-only.
     */
    Symbol p_CoderTypes		= ESymbol.Recorder_CoderTypes;

    /**
     * A Dictionary of Recorder event Symbols, indicating which events 
     * are generated and delivered to the application.
     * If a given eventID is not enabled, then no processing
     * is expended to generate or distribute that kind of event.
     * <p>
     * Value is a Dictionary of non-transactional event Symbols.
     * <br>Valid keys: <code> ev_Pause, ev_Resume, ev_Record</code>
     * <br>Valid values: <code>Boolean.TRUE, Boolean.FALSE</code>
     * <br>Default value = <i>no events</i>.
     * <p>
     * For a RecorderListener to receive events, those events must
     * be enabled by setting this parameter.  
     * This parameter can be set in a {@link ConfigSpec} or by
     * {@link MediaService#setParameters setParameters}.
     * <p>
     * <b>Note:</b>
     * This parameter controls the generation of events.
     * The events cannot be delivered unless a {@link RecorderListener}
     * is added using 
     * {@link MediaService#addMediaListener addMediaListener}. 
     */
    Symbol p_EnabledEvents	= ESymbol.Recorder_EnabledEvents;

    /**
     * Integer indicating the maximum duration (in milliseconds) for a record.
     * <br>Default: v_forever.
     */
    Symbol p_MaxDuration	= ESymbol.Recorder_MaxDuration;

    /**
     * Integer indicating minimum duration (in milliseconds) 
     * that constitutes a valid recording.
     * <br>Default: 0
     */
    Symbol p_MinDuration	= ESymbol.Recorder_MinDuration;

    /**
     * Boolean indicating whether silence truncation is enabled.
     */
    Symbol p_SilenceTruncationOn = ESymbol.Recorder_PauseCompressionOn;

    /**
     * Integer threshold (in milliseconds) for silence truncation.
     * Recorder will truncate segments of silence to this length if
     * p_SilenceTruncationOn is True.
     */
    Symbol p_SilenceTruncationThreshold	= ESymbol.Recorder_PauseThreshold;

    /**
     * Boolean indicating whether silence termination is enabled.
     */
    Symbol p_SilenceTerminationOn = ESymbol.Recorder_SilenceTerminationOn ;

    /**
     * Integer threshold (in milliseconds) when silence termination is triggered.
     */
    Symbol p_SilenceTerminationThreshold = ESymbol.Recorder_SilenceThreshold;

    /**
     * Boolean indicating whether subsequent record will be preceded with a beep.
     */
    Symbol p_StartBeep		= ESymbol.Recorder_StartBeep;

    /**
     * Boolean indicating whether subsequent record will start in PAUSE mode.
     */
    Symbol p_StartPaused	= ESymbol.Recorder_StartPaused;
    

    /**
     * Record ended because Silence termination was enabled and a silence
     * of greater that <code>p_SilenceTerminationThreshold</code> milliseconds
     * was detected.
     */
    Symbol q_Silence		= ESymbol.Recorder_Silence;

    /**
     * RTC Actions that can be applied to a recorder.
     */
    
    /**
     * RTC Action to stop the current operation on a recorder.
     * <p>
     * Used as the <tt>action</tt> for an RTC.
     */
    Symbol rtca_Stop		= ESymbol.Recorder_Stop;
    
    /**
     * RTC Action to resume the current operation on a recorder if paused.
     * <p>
     * Used as the <tt>action</tt> for an RTC.
     */
    Symbol rtca_Resume		= ESymbol.Recorder_Resume; 
    
    /**
     * RTC Action to pause the current operation on a recorder.
     * Pause maintains the current position in the Media DataObject being recorded.
     * <p>
     * Used as the <tt>action</tt> for an RTC.
     */
    Symbol rtca_Pause		= ESymbol.Recorder_Pause; 
    
    /**
     * These symbols are recognized RTC Conditions for Recorder.
     * They can be used when creating RTC objects.
     * (They may also show up as events.)
     */

    /**
     * RTC trigger when a Record is completed.
     */
    Symbol rtcc_RecordComplete	= ESymbol.Recorder_Record;
}
