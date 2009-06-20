package javax.telephony.phone;

import javax.telephony.Event;
import javax.telephony.TerminalEvent;

/**
 * The <CODE>PhoneTerminalEvent</CODE> is the base event for all events in
 * the <CODE>phone</CODE> package. Each event in this package must extend this
 * interface. Events from the <CODE>phone</CODE> package reported via the 
 * PhoneTerminalListener interface.
 * <p>
 * This interface contains the <CODE>getPhoneCause()</CODE> method which
 * returns a phone package specific cause for the event. Cause codes
 * pertaining to this package are defined in this interface as well.
 * 
 * @see javax.telephony.Terminal
 * @see javax.telephony.TerminalEvent
 * @see javax.telephony.Event
 * @see javax.telephony.phone.PhoneTerminalListener
 * @since 1.4
 */
public interface PhoneTerminalEvent extends Event, TerminalEvent 
{

    
    
    /**
     * Cause code indicating normal operation
     */
    public static final int CAUSE_PHONE_TERMINAL_NORMAL = 500;

    /**
     * Cause code indicating the cause was unknown
     */
    public static final int CAUSE_PHONE_TERMINAL_UNKNOWN = 501;

  
    /**
     * Returns the phone causes associated with this event.
     *
     * @return The cause of the event.
     */
    public int getPhoneCause();
    
    /**
     * Returns the ComponentGroup object associated with this event.
     * <p>
     * @return The ComponentGroup object associated with this event.
     */
    public ComponentGroup getComponentGroup();


    /**
     * Returns the Component object responsible for this event.
     * <p>
     * @return The Component object responsible for this event.
     */
    public Component getComponent();


        
    /**
     * The <CODE>PHONE_TERMINAL_BUTTON_INFO_EVENT</CODE> event indicates
     * that the information associated with a button component has changed.
     * <p>
     * Applications may obtain the new information associated with this button
     * via the {@link PhoneButton#getInfo()} method.
     *
     * @see PhoneButton#setInfo
     */
    public static final int PHONE_TERMINAL_BUTTON_INFO_EVENT      = 500;
    
    /**
     * The <CODE>PHONE_TERMINAL_BUTTON_PRESS_EVENT</CODE> event indicates
     * that a button component has been pressed.
     * <p>
     * Applications may obtain the identifying information associated with this
     * button via the {@link PhoneButton#getInfo()} method.
     * 
     * @see PhoneButton#buttonPress
     */
    public static final int PHONE_TERMINAL_BUTTON_PRESS_EVENT     = 501;
    
    /**
     * The <CODE>PHONE_TERMINAL_DISPLAY_UPDATE_EVENT</CODE> event indicates 
     * that the contents of the display component has changed.
     * <p>
     * Applications may obtain the new contents of the display component via
     * the {@link PhoneDisplay#getDisplay(int x, int y)} method.
     *
     * @see PhoneDisplay#setDisplay(java.lang.String string, int x, int y)
     */
    public static final int PHONE_TERMINAL_DISPLAY_UPDATE_EVENT   = 502;
    
    /**
     * The <CODE>PHONE_TERMINAL_HOOKSWITCH_STATE_EVENT</CODE> event indicates 
     * that the state of the hookswitch component has changed.
     * <p>
     * Applications may obtain the new contents of the state of the 
     * hookswitch (either on-hook or off-hook) via the {@link 
     * PhoneHookswitch#getHookSwitchState()} method.
     *
     * @see PhoneHookswitch#ON_HOOK
     * @see PhoneHookswitch#OFF_HOOK
     * @see PhoneHookswitch#setHookSwitch(int hookSwitchState)
     */
    public static final int PHONE_TERMINAL_HOOKSWITCH_STATE_EVENT = 503;
    
    /**
     * The <CODE>PHONE_TERMINAL_LAMP_MODE_EVENT</CODE> event indicates 
     * that the mode of the <CODE>PhoneLamp</CODE> has changed.
     * <p>
     * Applications may obtain the new mode of the <CODE>PhoneLamp</CODE>
     * via the {@link PhoneLamp#getMode()} method.
     *
     * @see PhoneLamp#LAMPMODE_OFF
     * @see PhoneLamp#LAMPMODE_FLASH
     * @see PhoneLamp#LAMPMODE_STEADY
     * @see PhoneLamp#LAMPMODE_FLUTTER
     * @see PhoneLamp#LAMPMODE_BROKENFLUTTER
     * @see PhoneLamp#LAMPMODE_WINK
     * @see PhoneLamp#setMode(int mode)
     */
    public static final int PHONE_TERMINAL_LAMP_MODE_EVENT        = 504;
    
    /**
     * The <CODE>PHONE_TERMINAL_LAMP_MODE_EVENT</CODE> event indicates 
     * that the gain of a <CODE>PhoneMicrophone</CODE> component has changed.
     * <p>
     * Applications may obtain the new gain of the <CODE>PhoneMicrophone</CODE>
     * via the {@link PhoneMicrophone#getGain()} method.
     *
     * @see PhoneMicrophone#MUTE
     * @see PhoneMicrophone#MID
     * @see PhoneMicrophone#FULL
     * @see PhoneMicrophone#setGain(int gain)
     */
    public static final int PHONE_TERMINAL_MICROPHONE_GAIN_EVENT  = 505;
    
    /**
     * The <CODE>PHONE_TERMINAL_RINGER_PATTERN_EVENT</CODE> event indicates 
     * that the pattern of a ringer component has changed.
     * <p>
     * Applications may obtain the new pattern of the of the ringer component
     * via the {@link PhoneRinger#getRingerPattern()} method.
     *
     * @see PhoneRinger#setRingerPattern(int ringerPattern)
     */
    public static final int PHONE_TERMINAL_RINGER_PATTERN_EVENT   = 506;
    
    /**
     * The <CODE>PHONE_TERMINAL_RINGER_VOLUME_EVENT</CODE> event indicates 
     * that the volume of a ringer component has changed.
     * <p>
     * Applications may obtain the new volume of the ringer component
     * via the {@link PhoneRinger#getRingerVolume()} method.
     * 
     * @see PhoneRinger#FULL
     * @see PhoneRinger#MIDDLE
     * @see PhoneRinger#OFF
     * @see PhoneRinger#setRingerVolume(int volume)
     */
    public static final int PHONE_TERMINAL_RINGER_VOLUME_EVENT    = 507;
    
    /**
     * The <CODE>PHONE_TERMINAL_SPEAKER_VOLUME_EVENT</CODE> event indicates 
     * that the volume of a speaker component has changed.
     * <p>
     * Applications may obtain the new volume of the speaker component
     * via the {@link PhoneSpeaker#getVolume()} method.
     *
     * @see PhoneSpeaker#MUTE
     * @see PhoneSpeaker#MID
     * @see PhoneSpeaker#FULL
     * @see PhoneSpeaker#setVolume(int volume)
     */
    public static final int PHONE_TERMINAL_SPEAKER_VOLUME_EVENT   = 508;
    
    
    /**
     * The <CODE>PHONE_TERMINAL_BUTTON_PRESS_THRESHOLD_EXCEEDED_EVENT</CODE> event indicates 
     * that the indicated button has been pressed for longer than the
     * requested time threshold.
     * <p>
     * First this event is sent (if the threshold is exceeded), then
     * the PHONE_TERMINAL_BUTTON_PRESSED_EVENT is sent (since the second event
     * represents a complete button pressed in, button released again action).
     * <p>
     * The PHONE_TERMINAL_BUTTON_PRESSED_EVENT will now provide the time duration
     * that the button was actually pressed and held before the user released it.
     * Applications may obtain the actual duration that the button was held
     * via the {@link PhoneTerminalEvent#getPressedDuration()} method (which has
     * a meaningful value for this event).     
     * <p>
     * An application that is not interested in button press durations may simply
     * implement the PHONE_TERMINAL_BUTTON_PRESSED_EVENT method in their listener object
     * and ignore the PHONE_TERMINAL_BUTTON_PRESS_THRESHOLD_EXCEEDED_EVENT event
     * (returning a corresponding canSetButtonPressThreshold capability value).
     */
    public static final int PHONE_TERMINAL_BUTTON_PRESS_THRESHOLD_EXCEEDED_EVENT   = 509;
    

// ButtonInfoEv, ButtonPressEv, threshold_exceeded

  /**
   * Returns the button information.
   * <p>
   * @return The string button information.
   */
  public java.lang.String getInfo();


// ButtonInfoEv

  /**
   * Returns the information previously associated with this button.
   * <p>
   * @return The old button information.
   */
  public java.lang.String getOldInfo();
  

// DisplayUpdateEv

  /**
   * Returns the displayed string starting at coordinates (x, y).
   * <p>
   * @param x The x-coordinate.
   * @param y The y-coordinate.
   * @return The string displayed starting at coordinates (x, y).
   */
  public java.lang.String getDisplay(int x, int y);
  
  
  // HookswitchStateEv
  
  /**
   * Returns the current state of the hookswitch.
   * <p>
   * @return The current state of the hookswitch.
   */
  public int getHookSwitchState();
  
  
  // LampModeEv
  

  /**
   * Returns the current lamp mode.
   * <p>
   * @return The current lamp mode.
   */
  public int getMode();
  
  
  // MicrophoneGainEv
  
  /**
   * Returns the gain of the microphone.
   * <p>
   * @return The gain of the microphone.
   */
  public int getGain();
  
  
  // RingerPatternEv

  /**
   * Returns the pattern of the ringer.
   * <p>
   * @return The pattern of the ringer.
   */
  public int getRingerPattern();
  
  
  // RingerVolumeEv, SpeakerVolumeEv
  
  /**
   * Returns the volume of the ringer or speaker (depending on event).
   * <p>
   * @return The volume of the ringer or speaker (depending on event).
   */
  public int getVolume();
  
  
  // ButtonPressedEv (new)
 
  /**
   * Returns the duration of time starting when a user first pressed and held a button,
   * and ending when the user releases the button.
   * The value is in milliseconds.
   * <p>
   * @since 1.4
   */
  public int getPressedDuration();
  
}
