/*
 * PlayerConstants.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Defines the Symbols used by the Player Resource.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public 
interface PlayerConstants extends ResourceConstants, CoderConstants {
    /** 
     * Denotes the standard Player Resource Class in a ResourceSpec.
     */
    Symbol v_Class		= ESymbol.Player_ResourceClass;

    /**
     * Player selection attributes in ConfigSpec.
     */

    /**
     * Attribute to request support for various coders.
     * Value is a Symbol or Symbol[] that identifies the required coder(s).
     * <p>
     * The Symbols that identify coders are in 
     * {@link CoderConstants}
     */
    Symbol a_Coder		= ESymbol.Player_Coder;

    /**
     * Attribute to request capability to Jump forward and backward in a MDO.
     * <p>
     * Value is a Boolean.
     */
    Symbol a_Jump		= ESymbol.Player_Jump;

    /**
     * Attribute to request capability to Pause and Resume Play of MDO. 
     * Value is a Boolean.
     */
    Symbol a_Pause		= ESymbol.Player_Pause;

    /**
     * Attribute to request capability to adjust Speed during Play of a MDO. 
     * <p>
     * Value is an IntRange or a boolean.
     */
    Symbol a_Speed		= ESymbol.Player_Speed;

    /**
     * Attribute to request capability to adjust Volume during Play of a MDO. 
     * <p>
     * Value is an IntRange or a boolean.
     */
    Symbol a_Volume		= ESymbol.Player_Volume;

    /**
     * Attribute to indicate which TTS dictionaries are supported.
     * <p>
     * Value is an Symbol[] of TTS Dictionary; or String[] ??
     * <p>
     * Used for TTS Player.
     */
    Symbol a_LoadedDictionary     = ESymbol.Player_LoadedDictionary;

    /** 
     * Attribute to indicate have many TTS dictionaries may be loaded.
     * <p>
     * Value is an Integer.
     * <p>
     * Used for TTS Player.
     */
    Symbol a_DictionaryList	  = ESymbol.Player_DictionaryList;

    /**
     * Attribute used to indicate which SAPI optional tags are supported.
     * <p>
     * Value is a Symbol[].
     * <p>
     * Used for TTS Player.
     * <p>
     * <b>Note:</b> the tags themselves are strings that are embedded
     * in the input text stream and in TTS dictionaries. Each tag has a
     * corresponding Symbol, whose presence in the value of this attribute
     * indicates that the tag is supported.
     */
    Symbol a_SapiOptions	  = ESymbol.Player_SapiOptions;

    /**
     * Returned when <code>(p_IfBusy == v_Fail)</code>.  */
    Symbol e_Busy 		= ESymbol.Error_Busy;

    /**
     * Parameters of a Player, read/write unless indicated as Read-Only.
     */

    /**
     * Value is an array of Symbols
     * identifying the coders actually supported by this Player.
     * <p> Valid values: {@link CoderConstants}
     * <br> Default value is implementation dependent.
     * <br> Read-Only.
     *
     * @see #a_Coder
     */
    Symbol p_CoderTypes		= ESymbol.Player_CoderTypes;

    /**
     * Indicates the action to take if Player is busy
     * when <code>play()</code> is invoked.
     * <p>Valid values: <code>v_Queue, v_Stop, v_Fail</code>. 
     * <br>Default value is {@link #v_Queue}.
     */
    Symbol p_IfBusy 		= ESymbol.Player_IfBusy;

    /** value for p_IfBusy: wait for previous requests to complete. 
     * @see #p_IfBusy
     */
    Symbol v_Queue 		= ESymbol.Player_Queue;

    /** value for p_IfBusy: stop any previous play. 
     * @see #p_IfBusy
     */
    Symbol v_Stop 		= ESymbol.Player_Stop;

    /** value for p_IfBusy: signal an error, throw a MediaResourceException.
     * @see #p_IfBusy
     */
    Symbol v_Fail 		= ESymbol.Player_Fail;

    /** 
     * Integer indicating maximum duration of this or subsequent 
     * <code>play()</code> in milleseconds. 
     * <p>
     * Value is an Integer (0 - 1,000,000) milliseconds, or v_Forever. 
     */
    Symbol p_MaxDuration	= ESymbol.Player_MaxDuration;

    /**
     * Boolean indicating that this or subsequent <code>play()</code>
     * should be started in the Paused state.
     */
    Symbol p_StartPaused	= ESymbol.Player_StartPaused;

    /**
     * A Dictionary of Player event Symbols, indicating which events 
     * are generated and delivered to the application.
     * If a given eventID is not enabled, then no processing
     * is expended to generate or distribute that kind of event.
     * <p>
     * Value is a Dictionary of non-transactional event Symbols.
     * <ol>
     * <br>Valid keys: <code>ev_Pause, ev_Resume, ev_Speed, ev_Volume, ev_Marker</code>
     * <br>Valid values: <code>Boolean.TRUE, Boolean.FALSE</code>
     * <br>Default value = <i>no events</i>.
     * </ol>
     * <p>
     * For a PlayerListener to receive events, those events must
     * be enabled by setting this parameter.  
     * This parameter can be set in a {@link ConfigSpec} or by
     * {@link MediaService#setParameters setParameters}.
     * <p>
     * <b>Note:</b>
     * This parameter controls the generation of events.
     * The events cannot be delivered unless a {@link PlayerListener}
     * is added using 
     * {@link MediaService#addMediaListener addMediaListener}. 
     *
     * @see #ev_Pause
     * @see #ev_Resume
     * @see #ev_Speed
     * @see #ev_Volume
     * @see #ev_Marker
     */

    Symbol p_EnabledEvents	= ESymbol.Player_EnabledEvents;


    /** p_JumpMDO. @deprecated use p_JumpMDO */
    Symbol p_JumpMDOIncrement	= ESymbol.Player_JumpTVM;
    /** p_JumpMDO. @deprecated use p_JumpMDO */
    Symbol p_JumpMSCIncrement	= ESymbol.Player_JumpTVM;

    /**
     * Integer number of MDOs to jump forward or backward in the MDO list,
     * for either a <tt>rtca_JumpForwardMDO</tt> or <tt>rtca_JumpBackwardMDO</tt>.
     * <p> Valid values: any positive Integer.
     * <br>Default value = 1.
     * 
     * @see #rtca_JumpForwardMDOs
     * @see #rtca_JumpBackwardMDOs
     */
    Symbol p_JumpMDO		= ESymbol.Player_JumpTVM;

    /**
     * Integer number of milliseconds by which the current
     * MDO offset is changed by <tt>rtca_JumpForwardTime</tt> 
     * or <tt>rtca_JumpBackwardTime</tt>
     * <p>Valid values: any positive Integer. 
     * <br>Default value = 1.
     * 
     * @see #rtca_JumpForwardTime
     * @see #rtca_JumpBackwardTime
     */
    Symbol p_JumpTime		= ESymbol.Player_JumpTime;
    
    /**
     * Integer number of Words to jump forward or backward,
     * for either a <tt>rtca_JumpForwardWords</tt> or <tt>rtca_JumpBackwardWords</tt>.
     * <p> Valid values: any positive Integer.
     * <br>Default value = 1.
     * 
     * @see #rtca_JumpForwardWords
     * @see #rtca_JumpBackwardWords
     */
    Symbol p_JumpWord = ESymbol.Player_JumpWord;

    /**
     * Integer number of Sentences to jump forward or backward,
     * for either a <tt>rtca_JumpForwardSentences</tt> or <tt>rtca_JumpBackwardSentences</tt>.
     * <p> Valid values: any positive Integer.
     * <br>Default value = 1.
     * 
     * @see #rtca_JumpForwardSentences
     * @see #rtca_JumpBackwardSentences
     */
    Symbol p_JumpSentence = ESymbol.Player_JumpSentence;

    /**
     * Determines the amount the playback speed is changed
     * by RTC actions <code>rtca_SpeedUp</code> or <code>rtca_SpeedDown</code>.
     * Value is an Integer in the range [0..100] expressing
     * percent from normal.
     * <p>Valid values: Integer in the range [0..100] 
     * <br>Default value = 0.
     * <p>
     * <b>Note:</b>
     * Whenever p_SpeedChange is altered, the player speed is reset to normal.
     *
     * @see #rtca_SpeedUp
     * @see #rtca_SpeedDown
     */
    Symbol p_SpeedChange	= ESymbol.Player_SpeedChange;

    /**
     * Determines the amount the volume parameter is changed
     * by the RTC actions rtca_VolumeUp and rtca_VolumeDown.
     * The value is an Integer in the range [0..30] expressing dB of change.
     * <p> Valid values: an Integer in the range [0..30].
     * <br>Default value is 0.
     * <p>
     * <b>Note:</b>
     * Whenever p_VolumeChange is altered, the player volume is reset to normal.
     *
     * @see #rtca_VolumeUp
     * @see #rtca_VolumeDown
     */
    Symbol p_VolumeChange	= ESymbol.Player_VolumeChange;

    /**
     * These symbols are recognized RTC Actions for Player.
     * They can be used when creating RTC objects.
     */

    /**
     * RTC Action to stop the current Play operation. 
     */
    Symbol rtca_Stop		= ESymbol.Player_Stop;
    
    /**
     * RTC Action to resume the current Play operation if paused.
     */
    Symbol rtca_Resume		= ESymbol.Player_Resume;
    
    /**
     * RTC Action to pause the current Play operation.
     * Pause maintains the current position in the MDO List being played.
     */
    Symbol rtca_Pause		= ESymbol.Player_Pause;
    
    /**
     * RTC Action to jump forward the amount specified by <tt>p_JumpTime</tt>.
     */
    Symbol rtca_JumpForwardTime	= ESymbol.Player_JumpForwardTime;
    
    /**
     * RTC Action to jump backward the amount specified by p_JumpTime.
     */
    Symbol rtca_JumpBackwardTime = ESymbol.Player_JumpBackwardTime;

    /**
     * RTC Action to jump to the start of the curent MDO.
     */
    Symbol rtca_JumpStartMDO	= ESymbol.Player_JumpStartTVM;

    /** rtca_JumpStartMDO. @deprecated use rtca_JumpStartMDO. */
    Symbol rtca_JumpStartMSC	= ESymbol.Player_JumpStartTVM;
    
    /**
     * RTC Action to jump to the end of the curent MDO.
     */
    Symbol rtca_JumpEndMDO	= ESymbol.Player_JumpEndTVM;

    /** rtca_JumpEndMDO. @deprecated use rtca_JumpEndMDO. */
    Symbol rtca_JumpEndMSC	= ESymbol.Player_JumpEndTVM;

    /**
     * RTC Action to jump forward the number of MDOs indicated by <tt>p_JumpMDO</tt>.
     */
    Symbol rtca_JumpForwardMDOs = ESymbol.Player_JumpForwardTVMs;

    /** rtca_JumpForwardMDOs @deprecated use rtca_JumpForwardMDOs. */
    Symbol rtca_JumpForwardMSCs = ESymbol.Player_JumpForwardTVMs;
    
    /**
     * RTC Action to jump backward the number of MDOs indicated by <tt>p_JumpMDO</tt>.
     */
    Symbol rtca_JumpBackwardMDOs = ESymbol.Player_JumpBackwardTVMs;

    /** rtca_JumpBackwardMDOs. @deprecated use rtca_JumpBackwardMDOs. */
    Symbol rtca_JumpBackwardMSCs = ESymbol.Player_JumpForwardTVMs;


    /**
     * RTC Action to jump to the first MDO in the MDO List.
     */
    Symbol rtca_JumpStartMDOList	= ESymbol.Player_JumpStartTVMList;

    /** rtca_JumpStartMDOList @deprecated use rtca_JumpStartMDOList. */
    Symbol rtca_JumpStartMSCList	= ESymbol.Player_JumpStartTVMList;
    
    /**
     * RTC Action to jump to the last MDO in the MDOList.
     */
    Symbol rtca_JumpEndMDOList	= ESymbol.Player_JumpEndTVMList;

    /** rtca_JumpEndMDOList. @deprecated use rtca_JumpEndMDOList. */
    Symbol rtca_JumpEndMSCList	= ESymbol.Player_JumpEndTVMList;


    /**
     * RTC Action to jump forward the number of Words indicated by <tt>p_JumpWord</tt>.
     */
    Symbol rtca_JumpForwardWords 	= ESymbol.Player_JumpForwardWords;

    /**
     * RTC Action to jump backward the number of Words indicated by <tt>p_JumpWord</tt>.
     */
    Symbol rtca_JumpBackwardWords 	= ESymbol.Player_JumpBackwardWords;

    /**
     * RTC Action to jump to the first Sentence in the MDO.
     */
    Symbol rtca_JumpStartSentence 	= ESymbol.Player_JumpStartSentence;

    /**
     * RTC Action to jump to the last Sentence in the MDO.
     */
    Symbol rtca_JumpEndSentence 	= ESymbol.Player_JumpEndSentence;

    /**
     * RTC Action to jump forward the number of Sentences indicated by <tt>p_JumpSentence</tt>.
     */
    Symbol rtca_JumpForwardSentences 	= ESymbol.Player_JumpForwardSentences;

    /**
     * RTC Action to jump backward the number of Sentences indicated by <tt>p_JumpSentence</tt>.
     */
    Symbol rtca_JumpBackwardSentences 	= ESymbol.Player_JumpBackwardSentences;


    /**
     * RTC Action to increase speed by <tt>p_SpeedChange</tt>.
     */
    Symbol rtca_SpeedUp		= ESymbol.Player_SpeedUp;
    
    /**
     * RTC Action to decrease speed by <tt>p_SpeedChange</tt>.
     */
    Symbol rtca_SpeedDown	= ESymbol.Player_SpeedDown;
    
    /**
     * RTC Action to set speed to normal.
     */
    Symbol rtca_NormalSpeed	= ESymbol.Player_NormalSpeed;
    
    /**
     * RTC Action to toggle speed between normal and previous adjusted value.
     */
    Symbol rtca_ToggleSpeed	= ESymbol.Player_ToggleSpeed;
    
    /**
     * RTC Action to increase volume by <tt>p_VolumeChange</tt>.
     */
    Symbol rtca_VolumeUp	= ESymbol.Player_VolumeUp;
    
    /**
     * RTC Action to decrease volume by <tt>p_VolumeChange</tt>.
     */
    Symbol rtca_VolumeDown	= ESymbol.Player_VolumeDown;
    
    /**
     * RTC Action to set volume to normal.
     */
    Symbol rtca_NormalVolume	= ESymbol.Player_NormalVolume;
    
    /**
     * RTC Action to toggle volume between normal and previous adjusted value.
     */
    Symbol rtca_ToggleVolume	= ESymbol.Player_ToggleVolume;
    
    
    /**
     * These symbols are recognized RTC Conditions for Player.
     * They can be used when creating RTC objects.
     * (They may also show up as events.)
     */

    /**
     * RTC trigger when a Play is started.
     */
    Symbol rtcc_PlayStarted	= ESymbol.Player_PlayStarted;

    /**
     * RTC trigger when a Play is completed.
     */
    Symbol rtcc_PlayComplete	= ESymbol.Player_Play;



    /*************************************************************/

    /**
     * Player events Constants.
     * These are the EventID Symbols.
     */
    
    /* Completion events */
    /**
     * Completion event for the <code>play</code> method.
     * <p>
     * Possible qualifiers are:<br><code>
     * q_RTC, q_Stop, q_Duration, q_Standard, q_EndOfData
     * </code>
     * @see PlayerEvent#getIndex()
     * @see PlayerEvent#getOffset() 
     * @see PlayerEvent#getRTCTrigger()
     */
    Symbol ev_Play			= ESymbol.Player_Play;
    
    /**
     * Completion event for the <code>adjustSpeed</code> method.
     */
    Symbol ev_AdjustSpeed		= ESymbol.Player_AdjustSpeed;

    /**
     * Completion event for the <code>adjustVolume</code> method.
     */
    Symbol ev_AdjustVolume		= ESymbol.Player_AdjustVolume;

    /* RTC Events *AND* transactional EventIDs */
    /**
     * Play has been stopped by RTC or stopPlayer.
     * @see PlayerEvent#getIndex()
     * @see PlayerEvent#getOffset() 
     * @see PlayerEvent#getRTCTrigger()
     */
    Symbol ev_Stop			= ESymbol.Player_Stop;

    /**
     * Play has been paused by RTC or pausePlayer.
     * @see PlayerEvent#getIndex()
     * @see PlayerEvent#getOffset() 
     * @see PlayerEvent#getRTCTrigger()
     */
    Symbol ev_Pause			= ESymbol.Player_Pause;

    /**
     * Play has been resumed by RTC or resumePlayer.
     * @see PlayerEvent#getIndex()
     * @see PlayerEvent#getOffset() 
     * @see PlayerEvent#getRTCTrigger()
     */
    Symbol ev_Resume			= ESymbol.Player_Resume;

    /**
     * A jump has been caused by RTC or jumpPlayer.
     * @see PlayerEvent#getChangeType()
     * @see PlayerEvent#getRTCTrigger()
     */
    Symbol ev_Jump			= ESymbol.Player_Jump;

    /**
     * Playback speed has been changed due to RTC.
     * @see PlayerEvent#getChangeType()
     * @see PlayerEvent#getRTCTrigger()
     */
    Symbol ev_Speed			= ESymbol.Player_Speed;

    /**
     * Playback volume has been changed due to RTC.
     * @see PlayerEvent#getChangeType()
     * @see PlayerEvent#getRTCTrigger()
     */
    Symbol ev_Volume			= ESymbol.Player_Volume;

    /**
     * Indicates a TTS Marker has been reached. (TTS)
     */
    Symbol ev_Marker			= ESymbol.Player_Marker;
    
    /* Completion Events (TTS) */

    /**
     * Completion event for LoadDictionary (TTS).
     */
    Symbol ev_LoadDictionary 		= ESymbol.Player_LoadDictionary;

    /**
     * Completion event for UnLoadDictionary (TTS).
     */
    Symbol ev_UnloadDictionary		 = ESymbol.Player_UnloadDictionary;

    /**
     * Completion event for ActivateDictionary (TTS).
     */
    Symbol ev_ActivateDictionary	= ESymbol.Player_ActivateDictionary;
    
    /**
     * Indicate Play complete because all the streams played to completion.
     */
    Symbol q_EndOfData			= ESymbol.Player_EOD;
    
    /**
     * Play ended because the maximum play duration time has been reached.
     */
    Symbol q_Duration 			= ESymbol.Any_Duration;
}
