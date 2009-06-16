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

package javax.telephony.media.capabilities;
import  javax.telephony.capabilities.*;

/**
 * Old MediaTerminalConnectionCapabilities from JTAPI-1.2.
 *
 * The MediaTerminalConnectionCapabilities interface extends the
 * TerminalConnectionCapabilities interface. This interface provides
 * capabilities methods for the MediaTerminalConnection object. The methods in
 * this interface provides applications the ability to query for those actions
 * where are possible on the MediaTerminalConnection interface as part of
 * the capabilities package.
 *
 * @deprecated   As of JTAPI 1.3, replaced by {@link javax.telephony.media.ConfigSpec}
 */

public interface MediaTerminalConnectionCapabilities extends TerminalConnectionCapabilities {

  /**
   * This method returns true if the application can invoke the
   * useDefaultSpeaker() method and route the media from the telephone line
   * to the default speaker. Returns false otherwise.
   * <p>
   * @return True if the application can route voice media to the default
   * speaker, false otherwise.
   */
  public boolean canUseDefaultSpeaker();


  /**
   * This method returns true if the application can invoke the
   * useDefaultMicrophone() method and route the media from the default
   * microphone. Returns false otherwise.
   * <p>
   * @return True if the application can route voice media from the default
   * microphone, false otherwise.
   */
  public boolean canUseDefaultMicrophone();


  /**
   * This method returns true if the application can invoke the
   * useRecordURL() method and route voice media to URL's. Returns false
   * otherwise.
   * <p>
   * @return True if the application can route voice media to URL's, false
   * otherwise.
   */
  public boolean canUseRecordURL();


  /**
   * This method returns true if the application can invoke the
   * usePlayURL() method and route voice media from URL's. Returns false
   * otherwise.
   * <p>
   * @return True if the application can route voice media from URL's, false
   * otherwise.
   */
  public boolean canUsePlayURL();


  /**
   * This method returns true if the application is able to start playing to
   * the telephone line. Returns false otherwise.
   * <p>
   * @return True if the application can begin playing to the telephone line,
   * false otherwise.
   */
  public boolean canStartPlaying();


  /**
   * This method returns true if the application is able to stop playing to
   * the telephone line. Returns false otherwise.
   * <p>
   * @return True if the application can stop playing to the telephone line,
   * false otherwise.
   */
  public boolean canStopPlaying();


  /**
   * This method returns true if the application is able to start recording
   * from the telephone line. Returns false otherwise.
   * <p>
   * @return True if the application can start recording from the telephone
   * line, false otherwise.
   */
  public boolean canStartRecording();


  /**
   * This method returns true if the application is able to stop recording from
   * the telephone line. Returns false otherwise.
   * <p>
   * @return True if the application can stop recording from the telephone
   * line, false otherwise.
   */
  public boolean canStopRecording();


  /**
   * This method returns true if the application is able to detect DTMF-tones
   * on the telephone line. Returns false otherwise. This method indicates
   * whether the application is able to invoke the setDtmfDetection(true)
   * method.
   * <p>
   * @return True if the application can detect DTMF-tones from the telephone
   * line, false otherwise.
   */
  public boolean canDetectDtmf();


  /**
   * This method returns true if the application is able to generate DTMF-
   * tones the telephone line. Returns false otherwise.
   * <p>
   * @return True if the application can generate DTMF-tones to the telephone
   * line, false otherwise.
   */
  public boolean canGenerateDtmf();
}
