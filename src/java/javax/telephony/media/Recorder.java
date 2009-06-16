/*
 * Recorder.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

import java.util.Dictionary;

/**
 * Defines a method for recording a media stream into a data file.
 * A Recorder transcodes the media stream data from the telephony network,
 * and stores the result in a Media Data Object (MDO).
 * <p>
 * A variety a coder types are defined, depending on the particular Recorder.
 * <p>
 * Run Time Controls can be used to stop recording.
 * <p>
 * A Recorder may optionally support additional processing features.
 * These features define parameters and runtime controls that affect
 * operation of an ongoing play operation.
 * The presence of these features is indicated by a <code>true</code>
 * value for the following attributes:
 * <table border="1" cellpadding="3">
 * <tr><td>Attribute:</td><td>indicates the ability to:</td></tr>
 * <tr><td><code>a_Beep</code></td>
 * <td>play a beep before recording.<br> 
 * parameters <code>p_StartBeep, p_BeepLength, p_BeepFrequency</code>
 * are supported</td></tr>
 * <tr><td><code>a_FixedBeep</code></td>
 * <td>do limited beep. <br> 
 * parameter <code>p_StartBeep</code> is supported</td></tr>
 * <tr><td><code>a_CoderTypes</code></td>
 * <td>use the listed coders. <br>
 * see {@link CoderConstants} for defined coder types</td></tr>
 * <tr><td><code>a_Pause</code></td>
 * <td>pause and resume while recording.<br>
 * RTC actions <code>rtca_Pause</code> and <code>rtca_Resume</code>
 * are supported</td></tr>
 * </table>
 * <p>
 * <h4>States:</h4>
 * The recorder is either Idle, Active, or Paused.
 * <p>
 * <code>record()</code> fails unless the state is Idle.
 * and then the state becomes Active or Paused.
 * The state of the Recorder after play() starts is determined
 * by the Boolean value of the parameter <code>p_StartPaused</code>.
 * The state transitions to Active if <code>p_StartPaused</code> is false.
 * The state transitions to Paused if <code>p_StartPaused</code> is true.
 * <p>
 * When recording stops, the state becomes Idle.
 * Recording stops for a variety of reasons.
 * <p>
 * <code>rtca_Pause</code> has no effect unless state is Recording,
 * and then the state becomes Paused, and <code>ev_Paused</code>
 * is sent to onPaused().
 * <p>
 * <code>rtca_Resume</code> has no effect unless state is Paused,
 * and then the state becomes Recording, and <code>ev_Resumed</code>
 * is sent to onResumed().
 * <p>
 * <b>Note:</b>
 * The format of the String that identifies 
 * an MDO is not defined by this release of specification.  
 * [although it will be compatible with that used in the ContainerService].
 * The vendor-specific implementation of the MediaService 
 * (for example, the vendor of the server or resource)
 * shall define the supported String formats.
 * <p>
 * For example, an implementation may support URLs or File pathnames;
 * in which case an application could use Strings of the form: 
 * <code>"file://node/dir/name"</code> or <code>"dir/dir/name"</code>
 * <p>
 * If the MDO identifier is a file pathname, it is interpreted in the
 * logical filename-space of the <b>server</b>.  
 * Applications and their prompts need to
 * be installed with correct pathnames.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public 
interface Recorder extends Resource, RecorderConstants {
    /**
     * Record data from the Terminal into the named
     * Media Data Object (MDO).
     * Encode and store data from the telephony network.
     * <code>record()</code> encodes the media stream
     * from the bound Terminal, and stores the result in 
     * the Media Stream Container identified by 
     * the given <code>streamID</code>.
     * <p>
     * If the MDO does not exist, it is created and associated
     * with the coder type specified by parameter <code>p_Coder</code>. 
     * If the MDO <i>does</i> exist, the recording is made using
     * the coder type associated with that DSD.
     * If the Recorder does not support the indicated Coder, 
     * <code>record()</code> throws a <code>MediaResourceException</code>, 
     * indicating <code>e_Coder</code>.
     * <p>
     * This method returns when the recording stops
     * because of timeout or an RTC action. 
     * (Pause/Resume does not cause this method to return.)
     * The condition that caused recording to stop is available from the
     * completion event using <code>getQualifier</code>. 
     * <p>
     * The <code>optargs</code> argument specifies values for Recorder
     * parameters for the duration of this record operation.
     * For example, <code>p_Coder</code> or <code>p_BeepStart</code>
     * can be overridden for a single method invocation.
     * <p>
     * Exceptions are thrown if pre- or post- conditions are not satisfied
     * <br><b>Pre-conditions:</b><br>
     * <ol>
     * <li>Recorder is configured into this MediaService</li>
     * <li>The SDR exists or can be created</li>
     * <li>The SDR is not locked (that is, is not in use)</li>
     * <li>Recorder is Idle</li>
     * <li>Recorder has the required/requested coder</li>
     * </ol>
     * <br><b>Post-conditions:</b>
     * Record completes normally due to one of:
     * <ol>
     * <li>Duration time reached</li>
     * <li>a silence exceeding <code>p_SilenceThreshold</code> detected</li>
     * <li>stop() method invoked</li>
     * <li><code>rtca_Stop</code> triggered</li>
     * </ol>
     * <p>
     * @param streamID the String name of the MDO where data is to be stored.
     * @param rtc an Array of RTC (Run-Time-Control) objects
     * @param optargs a Dictionary of optional arguments 
     *
     * @return a RecorderEvent with additional information.
     * @throws MediaResourceException if requested operation fails. 
     * @throws DisconnectedException if Terminal is disconnected.
     */
    RecorderEvent record(String streamID, RTC[] rtc, Dictionary optargs)
	throws MediaResourceException;

    /**
     * Pause the current record.
     * <p>
     * This is functionally equivalent to rtca_Pause,
     * except that it generates a Completion Event 
     * instead of a non-transactional Event.
     * @return a RecorderEvent object
     * @throws MediaResourceException 
     */
    RecorderEvent pauseRecorder() throws MediaResourceException;
    /**
     * Resume the current record.
     * <p>
     * This is functionally equivalent to rtca_Resume,
     * except that it generates a Completion Event 
     * instead of a non-transactional Event.
     * @return a RecorderEvent object
     * @throws MediaResourceException 
     */
    RecorderEvent resumeRecorder() throws MediaResourceException;
    /**
     * Stop the current record.
     * <p>
     * This is functionally equivalent to rtca_Stop,
     * except that it generates a Completion Event 
     * instead of a non-transactional Event.
     * @return a RecorderEvent object
     * @throws MediaResourceException 
     */
    RecorderEvent stopRecorder() throws MediaResourceException;
}
