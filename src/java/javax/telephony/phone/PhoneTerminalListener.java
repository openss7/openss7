package javax.telephony.phone;

import javax.telephony.TerminalListener;

/**
 * The <CODE>PhoneTerminalListener</CODE> interface reports all changes 
 * that happen to the phone components. This interface extends the 
 * <CODE>TerminalListener</CODE> interfaces.
 *
 * @see javax.telephony.TerminalListener
 * @see javax.telephony.phone.PhoneTerminalEvent
 * @since 1.4
 */
public interface PhoneTerminalListener extends TerminalListener {
    
    /**
     * The <CODE>terminalButtonInfoChanged</CODE> method is called to report 
     * that the information associated with a button component has changed.
     * <p>
     * @param event A PhoneTerminalEvent with eventID 
     * <CODE>PhoneTerminalEvent.PHONE_TERMINAL_BUTTON_INFO_EVENT</CODE>.
     *
     * @see PhoneTerminalEvent#PHONE_TERMINAL_BUTTON_INFO_EVENT
     * @see PhoneButton
     * @see PhoneButton#getInfo
     */
    public void terminalButtonInfoChanged( PhoneTerminalEvent event);
    
    /**
     * The <CODE>terminalButtonPressed</CODE> method is called to report 
     * that a button component has been pressed.
     * <p>
     * @param event A PhoneTerminalEvent with eventID 
     * <CODE>PhoneTerminalEvent.PHONE_TERMINAL_BUTTON_PRESS_EVENT</CODE>.
     *
     * @see PhoneTerminalEvent#PHONE_TERMINAL_BUTTON_PRESS_EVENT
     * @see PhoneButton#getInfo
     * @see PhoneButton#buttonPress
     */
    public void terminalButtonPressed( PhoneTerminalEvent event);
    
    /**
     * The <CODE>terminalDispalyUpdated</CODE> method is called to report 
     * that the contents of the display component has changed.
     * <p>
     * @param event A PhoneTerminalEvent with eventID 
     * <CODE>PhoneTerminalEvent.PHONE_TERMINAL_DISPLAY_UPDATE_EVENT</CODE>.
     *
     * @see PhoneTerminalEvent#PHONE_TERMINAL_DISPLAY_UPDATE_EVENT
     * @see PhoneDisplay#getDisplay(int x, int y)
     * @see PhoneDisplay#setDisplay(java.lang.String string, int x, int y)
     */
    public void terminalDisplayUpdated( PhoneTerminalEvent event);
    
    /**
     * The <CODE>terminalHookswitchStateChanged</CODE> method is called to report 
     * that the state of the hookswitch component has changed.
     * <p>
     * @param event A PhoneTerminalEvent with eventID 
     * <CODE>PhoneTerminalEvent.PHONE_TERMINAL_HOOKSWITCH_STATE_EVENT</CODE>.
     *
     * @see PhoneTerminalEvent#PHONE_TERMINAL_HOOKSWITCH_STATE_EVENT
     * @see PhoneHookswitch#getHookSwitchState()
     * @see PhoneHookswitch#setHookSwitch(int hookSwitchState)
     */
    public void terminalHookswitchStateChanged( PhoneTerminalEvent event);
    
    /**
     * The <CODE>terminalLampModeChanged</CODE> method is called to report 
     * that the mode of the <CODE>PhoneLamp</CODE> has changed.
     * <p>
     * @param event A PhoneTerminalEvent with eventID 
     * <CODE>PhoneTerminalEvent.PHONE_TERMINAL_LAMP_MODE_EVENT</CODE>.
     *
     * @see PhoneTerminalEvent#PHONE_TERMINAL_LAMP_MODE_EVENT
     * @see PhoneLamp#getMode()
     * @see PhoneLamp#setMode(int mode)
     * @see PhoneLamp#getSupportedModes()
     */
    public void terminalLampModeChanged( PhoneTerminalEvent event);
    
    /**
     * The <CODE>terminalMicrophoneGaineChanged</CODE> method is called to report 
     * that the gain of a <CODE>PhoneMicrophone</CODE> component has changed.
     * <p>
     * @param event A PhoneTerminalEvent with eventID 
     * <CODE>PhoneTerminalEvent.PHONE_TERMINAL_MICROPHONE_GAIN_EVENT</CODE>.
     *
     * @see PhoneTerminalEvent#PHONE_TERMINAL_MICROPHONE_GAIN_EVENT
     * @see PhoneMicrophone
     * @see PhoneMicrophone#getGain()
     * @see PhoneMicrophone#setGain(int gain)
     */
    public void terminalMicrophoneGainChanged( PhoneTerminalEvent event);
    
    /**
     * The <CODE>terminalRingerPatternChanged</CODE> method is called to report 
     * that the pattern of a <CODE>PhoneRinger</CODE> component has changed.
     * <p>
     * @param event A PhoneTerminalEvent with eventID 
     * <CODE>PhoneTerminalEvent.PHONE_TERMINAL_RINGER_PATTERN_EVENT</CODE>.
     *
     * @see PhoneTerminalEvent#PHONE_TERMINAL_RINGER_PATTERN_EVENT
     * @see PhoneRinger#getRingerPattern()
     * @see PhoneRinger#setRingerPattern(int ringerPattern)
     */
    public void terminalRingerPatternChanged( PhoneTerminalEvent event);
    
    /**
     * The <CODE>terminalRingerVolumeChanged</CODE> method is called to report 
     * that the volume of a <CODE>PhoneRinger</CODE> component has changed.
     * <p>
     * @param event A PhoneTerminalEvent with eventID 
     * <CODE>PhoneTerminalEvent.PHONE_TERMINAL_RINGER_VOLUME_EVENT</CODE>.
     *
     * @see PhoneTerminalEvent#PHONE_TERMINAL_RINGER_VOLUME_EVENT
     * @see PhoneRinger#getRingerVolume
     * @see PhoneRinger#setRingerVolume(int volume)
     */
    public void terminalRingerVolumeChanged( PhoneTerminalEvent event);
    
    /**
     * The <CODE>terminalSpeakerVolumeChanged</CODE> method is called to report 
     * that the volume of a <CODE>PhoneSpeaker</CODE> component has changed.
     * <p>
     * @param event A PhoneTerminalEvent with eventID 
     * <CODE>PhoneTerminalEvent.PHONE_TERMINAL_SPEAKER_VOLUME_EVENT</CODE>.
     *
     * @see PhoneTerminalEvent#PHONE_TERMINAL_SPEAKER_VOLUME_EVENT
     * @see PhoneSpeaker#getVolume()
     * @see PhoneSpeaker#setVolume(int volume)
     */
    public void terminalSpeakerVolumeChanged( PhoneTerminalEvent event);
    
    
    /**
     * The <CODE>terminalButtonPressThresholdExceeded</CODE> method is called to report 
     * that a button was held longer than the threshold set with method
     * {@link PhoneButton#setButtonPressThreshold(int threshold)}.
     * <p>
     * @param event A PhoneTerminalEvent with eventID 
     * <CODE>PhoneTerminalEvent.PHONE_TERMINAL_BUTTON_PRESS_THRESHOLD_EXCEEDED_EVENT</CODE>.
     * <p>
     * @see PhoneTerminalEvent#PHONE_TERMINAL_BUTTON_PRESS_THRESHOLD_EXCEEDED_EVENT
     * @see PhoneButton#setButtonPressThreshold(int threshold)
     */
    public void terminalButtonPressThresholdExceeded( PhoneTerminalEvent event);
    
}
