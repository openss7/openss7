/*
 * Copyright (c) 1996 Sun Microsystems, Inc. All Rights Reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for NON-COMMERCIAL purposes and without
 * fee is hereby granted provided that this copyright notice
 * appears in all copies. Please refer to the file "copyright.html"
 * for further important copyright and licensing information.
 *
 * SUN MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF
 * THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, OR NON-INFRINGEMENT. SUN SHALL NOT BE LIABLE FOR
 * ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR
 * DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.
 */

package javax.telephony.media;
import  javax.telephony.*;
import  java.net.URL;

/**
 * Old MediaTerminalConnection class.
 * Replaced in JTAPI-1.3 by the MediaService interface,
 * which delegates resource management and media stream state control
 * from the telephony application to the provider implementation.
 * This makes applications simpler and allows multiple applications
 * to easily share access to the provider's plaform; because the
 * provider software controls access, and applications don't build
 * competing frameworks for each platform.
 *
 * <H4>Introduction</H4>
 *
 * The MediaTerminalConnection interface extends the TerminalConnection
 * interface to add media capabilities. Media streams are associated with
 * the TerminalConnection object in the call model. Therefore, different
 * Terminals which are part of the same call at the same Address may have
 * their own media streams. Additionality, Terminals which are part of more
 * than one call have separate media streams for each of its calls.
 * <p>
 * The media interface consists of a base media API which supports all of
 * the various types of media-based telephony applications. A simplier,
 * voice-based API exist for applications which desire only the most
 * simply voice-based media features. The base media API is still under
 * development. This specification only represent the voice API.
 * <p>
 * The voice API supports the following applications: routing voice data
 * to/from the telephone line to/from a workstation's speaker of microphone;
 * routing voice data to/from the telephone line to/from audio files; starting
 * and stoping of playing and recording; and DTMF tone detection.
 * <p>
 * In this specification, "playing" is defined as sending information to the 
 * telephone line. For example, an application would "play" an audio file to
 * the telephone line for the opposite parties to hear. The term "recording"
 * is defines as receiving information from the telephone line. For example,
 * an application may "record" data from the telephone line into a file on
 * disk.
 *
 * <H4>Playing</H4>
 *
 * For playing, applications may either route data from a URL with the
 * usePlayURL() method or from the workstatation's default microphone using the
 * useDefaultMicrophone() method. Note that if there is more than one
 * microphone on the workstation, then the default microphone may be set using
 * the javax.telephony.phone package. Applications begin playing using the
 * startPlaying() method and stop playing using the stopPlaying() method.
 * If an application issues a startPlaying() after a stopPlaying(), the
 * implementation attempts to read from the media where it last left off,
 * if possible. If the application wishes to "rewind" the media to the
 * beginning, it should re-issue the usePlayURL() method.
 *
 * <H4>Recording</H4>
 *
 * For recording, applications may either route data to a URL with the
 * useRecordURL() method or to the workstation's default speaker using the
 * useDefaultSpeaker() method. Note that if there is more than one speaker on
 * the workstation, then the default speaker may be set using the
 * javax.telephony.phone package. Applications begin recording using the
 * startRecording() method and stop recording using the stopRecording() method.
 * If an application issues a startRecording() after a stopRecording(), the
 * implementation attempts to write to the media where it last left off, if
 * possible. If the application wishes to "overwrite" the media from the
 * beginning, it should re-issue the useRecordURL() method.
 *
 * @deprecated   As of JTAPI 1.3, replaced by {@link javax.telephony.media.MediaService}
 *
 */

public interface MediaTerminalConnection extends TerminalConnection {

  /**
   * Media is currently available on this terminal connection.
   */
  public static final int AVAILABLE = 0x80;


  /**
   * Media is currently not available on this terminal connection.
   */
  public static final int UNAVAILABLE = 0x81;


  /**
   * There is currently playing on this terminal connection.
   */
  public static final int PLAYING = 0x01;


  /**
   * There is currently recording on this terminal connection.
   */
  public static final int RECORDING = 0x02;


  /**
   * There is currently no activity on this TerminalConnection.
   */
  public static final int NOACTIVITY = 0x0;


  /**
   * Returns the current media availability state, either AVAILABLE or
   * UNAVAILABLE.
   * <p>
   * @return The current availability of the media channel.
   */
  public int getMediaAvailability();


  /**
   * Returns the current state of the terminal connection as a bit mask
   * of PLAYING and RECORDING. If there is not activity, then this method
   * returns <CODE>MediaTerminalConnection.NOACTIVITY</CODE>.
   * <p>
   * @return The current state of playing or recording.
   */
  public int getMediaState();


  /**
   * Instructs the terminal connection to use the default speaker for
   * recording from the telephone line.
   * <p>
   * @exception PrivilegeViolationException Indicates the application is
   * not permitted to direct voice media to the default speaker.
   * @exception ResourceUnavailableException Indicates that the speaker is
   * not currently available for use.
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   */
  public void useDefaultSpeaker()
    throws PrivilegeViolationException, ResourceUnavailableException,
      MethodNotSupportedException;


  /**
   * Instructs the terminal connection to use a file for recording from
   * the telephone line.
   * <p>
   * @param url The URL-destination for the voice data for recording.
   * @exception PrivilegeViolationException Indicates the application is not
   * permitted to use the give URL for recording.
   * @exception ResourceUnavailableException Indicates the URL given is not
   * available, either because the URL was invalid or a network problem
   * occurred.
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.

   */
  public void useRecordURL(URL url)
    throws PrivilegeViolationException, ResourceUnavailableException,
      MethodNotSupportedException;


  /**
   * Instructs the terminal connection to use the default microphone for
   * playing to the telephone line.
   * <p>
   * @exception PrivilegeViolationException Indicates the application is
   * not permitted to direct voice media from the default microphone.
   * @exception ResourceUnavailableException Indicates that the microphone is
   * not currently available for use.
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.

   */
  public void useDefaultMicrophone()
    throws PrivilegeViolationException, ResourceUnavailableException,
      MethodNotSupportedException;


  /**
   * Instructs the terminal connection to use a file for playing to
   * the telephone line.
   * <p>
   * @param url The URL-source of the voice data to play.
   * valid or available source of voice data.
   * @exception PrivilegeViolationException Indicates the application is not
   * permitted to use the give URL for playing.
   * @exception ResourceUnavailableException Indicates the URL given is not
   * available, either because the URL was invalid or a network problem
   * occurred.
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.

   */
  public void usePlayURL(URL url)
    throws PrivilegeViolationException, ResourceUnavailableException,
      MethodNotSupportedException;


  /**
   * Start the playing. This method returns once playing has begun, that is,
   * when getMediaState() & PLAYING == PLAYING.
   * <p>
   * @exception MethodNotSupportedException The implementation does not
   * support playing to the telephone line.
   * @exception ResourceUnavailableException Indicates playing is not able
   * to be started because some resource is unavailable.
   * @exception InvalidStateException Indicates the TerminalConnection is not
   * in the media channel available state.
   */
  public void startPlaying()
    throws MethodNotSupportedException, ResourceUnavailableException,
      InvalidStateException;


  /**
   * Stop the playing. This method returns once the playing has stopped, that
   * is, when getMediaState() & PLAYING == 0. If playing is not currently
   * taking place, this method has no effect.
   */
  public void stopPlaying();


  /**
   * Start the recording. This method returns once the recording has started,
   * that is, when getMediaState() & RECORDING == RECORDING.
   * <p>
   * @exception MethodNotSupportedException The implementation does not
   * support recording from the telephone line.
   * @exception ResourceUnavailableException Indicates recording is not able
   * to be started because some resource is unavailable.
   * @exception InvalidStateException Indicates the TerminalConnection is not
   * in the media channel available state.
   */
  public void startRecording()
    throws MethodNotSupportedException, ResourceUnavailableException,
      InvalidStateException;


  /**
   * Stop the recording. This method returns once the recording has stopped,
   * that is, when getMediaState() & RECORDING == 0. If recording is not
   * currently taking place, this method has no effect.
   */
  public void stopRecording();


  /**
   * Sets the DTMF tone detection either on or off. If the boolean flag
   * argument is true, then DTMF detection is turned on, otherwise, it is
   * turned off.
   * <p>
   * @param enable If true, turns DTMF-tone detection on, if false, turns
   * DTMF-tone detection off.
   * @exception MethodNotSupportedException The implementation does not
   * support the detection of DTMF-tones.
   * @exception ResourceUnavailableException Indicates DTMF-detection cannot
   * be started because some resource is unavailable.
   * @exception InvalidStateException Indicates the TerminalConnection is not
   * in the media channel available state.
   */
  public void setDtmfDetection(boolean enable)
    throws MethodNotSupportedException, ResourceUnavailableException,
      InvalidStateException;


  /**
   * Generates one or more DTMF tones. The string parameters must consist of
   * one or more of the following characters: the numbers zero through nine
   * (0 - 9), the letters A through D, asterisk (*), or pound(#). Any other
   * characters in the string parameter are ignored.
   * <p>
   * @param digits The string of DTMF-tone digits to generate on the telephone
   * line.
   * @exception MethodNotSupportedException The implementation does not
   * support generating DTMF tones.
   * @exception ResourceUnavailableException Indicates the generating of DTMF
   * tones cannot be done because some resource is unavailable.
   * @exception InvalidStateException Indicates the TerminalConnection is not
   * in the media channel available state.
   */
  public void generateDtmf(String digits)
    throws MethodNotSupportedException, ResourceUnavailableException,
      InvalidStateException;
}
