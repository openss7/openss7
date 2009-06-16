/*
 * ASRConstants.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * ASR Constants.
 * <p>
 * This interface defines additional Symbols used by the ASR Interface.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public
interface ASRConstants extends ResourceConstants {

    /* Table 5-22: Vendor specific attributes */
    
    /**
     * Attribute continaing the nameof the vendor.
     */
    Symbol a_ResourceVendor	= ESymbol.ASR_ResourceVendor;
    
    /**
     * Attribute contiaining the name of the program.
     */
    Symbol a_ResourceName	= ESymbol.ASR_ResourceName;
    
    /**
     * Attribute containing the version number.
     */
    Symbol a_ResourceVersion	= ESymbol.ASR_ResourceVersion;
    
    /**
     * Attribute containing the version date.
     */
    Symbol a_Date	= ESymbol.ASR_Date;
    
    /* Table 5-23: ASR Attributes */
    
    /**
     * Boolean value indicating that the resource is or is not capable of
     * echo cancellation.
     */
    Symbol a_EchoCancellation	= ESymbol.ASR_EchoCancellation;
    
    /**
     * Indicates that the resource generates the
     * {@link #ev_IntermediateResultsReady} event, and can return these
     * results through {@link ASR#getResults getResults}.
     * <p> Possible values are {@link #v_Mutable} and {@link #v_Immutable}.
     */
    Symbol a_IntermediateResults	= ESymbol.ASR_IntermediateResults;
    
    /**
     * Value is a Dictionary indicating the abilities of the resource that
     * can be enabled by copying an appropriate Context onto the resource.
     * <br> The possible keys are {@link #a_Trainable}, {@link #a_SpeakerType},
     * {@link #a_Label}, {@link #a_Language}, {@link #a_Variant},
     * {@link #a_DetectionType}, {@link #a_Spotting}, {@link #a_UtteranceType},
     * {@link #a_Size}, and {@link #a_ContextType}.
     */
    Symbol a_LoadableContext	= ESymbol.ASR_LoadableContext;
    
    /**
     * Value is a Dictionary indicating the current abilities of
     * the resource, enabled by the combination of the resource and a
     * Context that is presently loaded on the resource.
     * <br> The possible keys are {@link #a_Trainable}, {@link #a_SpeakerType},
     * {@link #a_Label}, {@link #a_Language}, {@link #a_Variant},
     * {@link #a_DetectionType}, {@link #a_Spotting},
     * and {@link #a_UtteranceType}.
     */
    Symbol a_LoadedContext = ESymbol.ASR_LoadedContext;
    
    /**
     * Value is a Symbol[] containing the languages(s) recognized.
     */
    Symbol a_Language	= ESymbol.Language_Language;

    /**
     * Value is a boolean indicating if the Resource supports the
     * NumberWordsRequested parameter.
     */
    Symbol a_NumberWordsRequested = ESymbol.ASR_NumberWordsRequested;

    /**
     * Value is a Symbol specifying the type of output the
     * Recognizer supports.
     * <br> The possible values are {@link #v_Text} and {@link #v_Phonetic}.
     */
    Symbol a_OutputType	= ESymbol.ASR_OutputType;

    /**
     * Value is the maximum length of utterance that can be stored
     * (in msec) for later use.
     */
    Symbol a_RepeatRecognition	= ESymbol.ASR_RepeatRecognition;

    /**
     * Value is the type of results (as explained in section 5.7) to
     * expect from the Recognizer.
     * <br> The possible values are {@link #v_TypeI} and {@link #v_TypeII}.
     */
    Symbol a_ResultPresentation	= ESymbol.ASR_ResultPresentation;

    /**
     * Value is a boolean indicating that the resource supports the
     * Specific Utterance RTC Conditions.
     * @see #rtcc_SpecificUtterance
     */
    Symbol a_SpecificUtterance	= ESymbol.ASR_SpecificUtterance;

    /**
     * Value is a dictionary indicating that the resource
     * supports training. The Dictionary contains the different options
     * available when training the resource.
     * 
     * <ul> The possible keys are:
     * <li>{@link #a_AutomaticTraining}</li>
     * <li>{@link #a_TrainingType}</li>
     * <li>{@link #a_TrainingModifiable}</li>
     * <li>{@link #a_UtteranceValidation}</li>
     * <li>{@link #a_NumberRepetitions}</li>
     * </ul>
     */
    Symbol a_Training	= ESymbol.ASR_Training;

    /**
     * Value is Symbol[] containing the variant(s) recognised.
     */
    Symbol a_Variant	= ESymbol.Language_Variant;

    /* Table 5-24: Attributes when a context is loaded */

    /**
     * Value is a boolean indicating that the resource supports training.
     */
    Symbol a_Trainable	= ESymbol.ASR_Trainable;

    /**
     * Value is a Symbol describing the type of recognition supported.
     * <br> Possible values are {@link #v_Dependent}, {@link #v_Independent},
     * {@link #v_Identification}, or {@link #v_Verification}.
     */
    Symbol a_SpeakerType	= ESymbol.ASR_SpeakerType;

    /**
     * Value is A vendor-defined String identifying a set of words and/or
     * phrases.
     */
    Symbol a_Label	= ESymbol.ASR_Label;

    //Symbol a_Language	= ESymbol.Language_Language;
    //Symbol a_Variant	= ESymbol.Language_Variant;

    /**
     * Value is a Symbol describing the type of detection available.
     * <br> Possible values are {@link #v_Continuous} or {@link #v_Discrete}.
     */
    Symbol a_DetectionType	= ESymbol.ASR_DetectionType;

    /**
     * Value is a boolean indicating if the resource supports spotting.
     */
    Symbol a_Spotting	= ESymbol.ASR_Spotting;

    /**
     * Value is a Symbol describing the manner in which the utterance
     * data is described to the recogniser for training and must
     * be {@link #v_Phonetic}, {@link #v_Speech}, or {@link #v_Text}.

     */
    Symbol a_UtteranceType	= ESymbol.ASR_UtteranceType;

    /* table 5-25 context attributes or a_loadableContext */

    /**
     * Value is a vendor-defined heuristic describing the space available for
     * loading a Context onto the Resource. This measure may be compared
     * with the Size parameter of a Context to determine if a Context may
     * be loaded.
     */
    Symbol a_Size	= ESymbol.ASR_Size;

    /**
     * Value indicating the type of the context to be loaded.
     * The only ECTF-defined symbol is one for Context Grammars
     * that support the Java Speech Grammar Format.
     * Otherwise, this is a vendor-defined symbol that is used by the
     * vendor to distinguish between specific data formats used
     * internally by the vendor’s Resource.
     */
    Symbol a_ContextType	= ESymbol.ASR_ContextType;

    /* Table 5-26: Attributes for a_Training */

    /**
     * Value is a boolean indicating whether the resource is
     * capable of automatic training or application-directed training.
     */
    Symbol a_AutomaticTraining	= ESymbol.ASR_AutomaticTraining;

    /**
     * Value is a symbol describing the type of utterances used for training.
     * <br> Possible values are {@link #v_Phonetic}, {@link #v_Speech},
     * or {@link #v_Text}.
     */
    Symbol a_TrainingType	= ESymbol.ASR_TrainingType;

    /**
     * Value is a Symbol indicating whether a Word, once it
     * has been trained and committed, can use additional training
     * utterances to update the training.
     * <br>  Possible values are {@link #v_NotModifiable},
     * {@link #v_Retrain}, and {@link #v_AddTraining}.
     */
    Symbol a_TrainingModifiable	= ESymbol.ASR_TrainingModifiable;

    /**
     * Value is a boolean indicating whether the resource is
     * capable of generating the {@link #rtcc_InvalidUtterance},
     * {@link #rtcc_ValidUtteranceFound}, and
     * {@link #rtcc_ValidUtteranceFinal} conditions.
     */
    Symbol a_UtteranceValidation	= ESymbol.ASR_UtteranceValidation;

    /**
     * Value is the default number of utterances used to train a word
     * (for automatic training, the default number; for
     * application-driven training, the recommended number of
     * utterances). The minimum of this attribute is the minimum number of
     * training utterances needed. The maximum is the maximum number
     * of utterances that can be collected without performing a
     * {@link ASR#wordCommit wordCommit()}.
     */
    Symbol a_NumberRepetitions	= ESymbol.ASR_NumberRepetitions;

    /* Table 31: Grammar Context Attributes */
    /**
     * Attribute containing a list of public rule names that become
     * active when the Grammar Context becomes active.
     * <p>
     * Value is a String[] containing the rule names.
     */
    Symbol a_DefaultRules	= ESymbol.ASR_DefaultRules;

    /**
     * Attribute containing a list of all rules in the grammar.
     * <p>
     * Value is a String[] containing the rule names.
     */
    Symbol a_Rules	= ESymbol.ASR_Rules;

    /**
     * Attribute containing a list of all private rules in the grammar.
     * <p>
     * Value is a String[] containing the rule names.
     */
    Symbol a_PrivateRules	= ESymbol.ASR_PrivateRules;

    /**
     * Attribute containing a list of all public rules in the grammar.
     * <p>
     * Value is a String[] containing the rule names.
     */
    Symbol a_PublicRules	= ESymbol.ASR_PublicRules;

    /* Table 5-19: Error Codes */
    
    //Symbol e_Full	 		= ErrSym.e_ASRFull;
    
    /**
     * Invalid Context name.
     * @see ResourceEvent#getError getError()
     */
    Symbol e_BadContext			= ErrSym.e_BadContext;
    
    /**
     * Context does not work with this Recognizer.
     * @see ResourceEvent#getError getError()
     */
    Symbol e_IncorrectContext	= ErrSym.e_IncorrectContext;
    
    /**
     * Invalid Context Container.
     * @see ResourceEvent#getError getError()
     */
    Symbol e_BadContainerName	= ErrSym.e_BadContainerName;
    
    /**
     * Word already exists.
     * @see ResourceEvent#getError getError()
     */
    Symbol e_Exists	= ErrSym.e_Exists;
    
    /**
     * Word does not exist.
     * @see ResourceEvent#getError getError()
     */
    Symbol e_NoExists	= ErrSym.e_NoExists;
    
    /**
     * Recognizer does not support function call.
     * @see ResourceEvent#getError getError()
     */
    Symbol e_NotSupported	= ErrSym.e_NotSupported;


    /* Completion Event Identifiers */

    /** EventID for recognize(). */
    Symbol ev_Recognize 	       = ESymbol.ASR_Recognize;
    /** EventID for getResults(). */
    Symbol ev_GetResults	       = ESymbol.ASR_GetResults;
    /** EventID for getFinalResults(). */
    Symbol ev_GetFinalResults	       = ESymbol.ASR_GetFinalResults;
    /** EventID for retrieveRecognition(). */
    Symbol ev_RetrieveRecognition      = ESymbol.ASR_RetrieveRecognition;

    /* Transactions methods for RTC actions. */

    /** EventID for start(). */
    Symbol ev_Start	 	       = ESymbol.ASR_Start;
    /** EventID for stop(). */
    Symbol ev_Stop	 	       = ESymbol.ASR_Stop;
    /** EventID for idle(). */
    Symbol ev_Idle	 	       = ESymbol.ASR_Idle;
    /** EventID for updateParameters(). */
    Symbol ev_UpdateParameters 	       = ESymbol.ASR_UpdateParameters;
    
    /* Rule Expansion Methods. */
    /** EventID for getRuleExpansion(). */
    Symbol ev_GetRuleExpansion	       = ESymbol.ASR_RuleExpansionGet;
    /** EventID for setRuleExpansion(). */
    Symbol ev_SetRuleExpansion	       = ESymbol.ASR_RuleExpansionSet;

    /* Context Methods */
    /** EventID for contextCopy(). */
    Symbol ev_ContextCopy		= ESymbol.ASR_ContextCopy;
    /** EventID for contextCreate(). */
    Symbol ev_ContextCreate		= ESymbol.ASR_ContextCreate;
    /** EventID for contextGetParameters(). */
    Symbol ev_ContextGetParameters	= ESymbol.ASR_ContextGetParameters;
    /** EventID for contextRemove(). */
    Symbol ev_ContextRemove		= ESymbol.ASR_ContextRemove;
    /** EventID for contextSetParameters(). */
    Symbol ev_ContextSetParameters	= ESymbol.ASR_ContextSetParameters;

    /* Training Methods */
    /** EventID for wordCommit(). */
    Symbol ev_WordCommit		= ESymbol.ASR_WordCommit;
    /** EventID for wordCreate(). */
    Symbol ev_WordCreate		= ESymbol.ASR_WordCreate;
    /** EventID for wordDeleteLastUtterance(). */
    Symbol ev_WordDeleteLastUtterance	= ESymbol.ASR_WordDeleteLastUtterance;
    /** EventID for wordDeleteTraining(). */
    Symbol ev_WordDeleteTraining	= ESymbol.ASR_WordDeleteTraining;
    /** EventID for wordDestroy(). */
    Symbol ev_WordDestroy		= ESymbol.ASR_WordDestroy;
    /** EventID for wordTrain(). */
    Symbol ev_WordTrain			= ESymbol.ASR_WordTrain;

    /* section 5.10.1 Unsolicited Event Identifiers */
    
    /**
     * Recognition has resumed due to RTC.
     * @see ASREvent
     */
    Symbol ev_RecognitionStarted	= ESymbol.ASR_RecognitionStarted;
    
    /**
     * A valid utterance has been detected during training.
     * @see ASREvent
     */
    Symbol ev_ValidUtterance		= ESymbol.ASR_ValidUtterance;
    
    /**
     * An invalid utterance has been detected during training.
     * @see ASREvent
     */
    Symbol ev_InvalidUtterance		= ESymbol.ASR_InvalidUtterance;
    
    /**
     * Intermediate results are available.
     * @see ASREvent
     */
    Symbol ev_IntermediateResultsReady	= ESymbol.ASR_IntermediateResultsReady;
    


    /* Table 32: Grammar Context Parameters */

    /**
     * Value is a String[] of names for all of the rules in the grammar.
     * <br> Read-Only
     */
    Symbol p_Rules	= ESymbol.ASR_Rules;

    /**
     * Value is a String[] of names for all of the private rules in the
     * grammar.
     * <br> Read-Only
     */
    Symbol p_PrivateRules	= ESymbol.ASR_PrivateRules;

    /**
     * Value is a String[] of names for all of the public rules in the
     * grammar.
     * <br> Read-Only
     */
    Symbol p_PublicRules	= ESymbol.ASR_PublicRules;

    /**
     * Value is a String[] of names of the active rules in the grammar.
     * <br> Default value is {@link #a_DefaultRules}
     * <br> Set when Idle.
     */
    Symbol p_ActiveRules	= ESymbol.ASR_ActiveRules;

    /* Table 5-5: Context Control Parameters */

    /**
     * Value is a String[] of the active words.
     * <br> Defualt Value is all loaded words.
     * <br> May be set in the Idle state.
     */
    Symbol p_ActiveWords		= ESymbol.ASR_ActiveWords;
    /**
     * Value is a String[] of the loaded words.
     * <br> Read-Only.
     */
    Symbol p_LoadedWords		= ESymbol.ASR_LoadedWords;
    /**
     * Value is a String[] of the active contexts.
     * <br> Defualt Value is all loaded contexts.
     * <br> May be set in the Idle state.
     */
    Symbol p_ActiveContexts		= ESymbol.ASR_ActiveContexts;
    /**
     * Value is a String[] of the loaded contexts.
     * <br> Read-Only.
     */
    Symbol p_LoadedContexts		= ESymbol.ASR_LoadedContexts;

    /* Table 5-6: Context Information Parameters */
    /**
     * Value is a boolean indicating whether this context can be trained.
     * <br> May be set in the WordTrained state.
     */
    Symbol p_Trainable			= ESymbol.ASR_Trainable;
    /**
     * Value is a Symbol indicating the type of recognition possible.
     * The value must be {@link #v_Dependent}, {@link #v_Independent},
     * {@link #v_Identification}, or {@link #v_Verification}.
     * <br> May be set in the WordTrained state.
     */
    Symbol p_SpeakerType		= ESymbol.ASR_SpeakerType;
    /**
     * Value is an abstract String identifying a set of words and/or
     * phrases.
     * <br> May be set in the WordTrained state.
     */
    Symbol p_Label			= ESymbol.ASR_Label;
    /**
     * Value is a String[] containing the languages recognised.
     * <br> Read-Only.
     */
    Symbol p_Language			= ESymbol.Language_Language;
    /**
     * Value is a String[] containing the variants recognised.
     * <br> Read-Only.
     */
    Symbol p_Variant			= ESymbol.Language_Variant;
    /**
     * Value is a Symbol describing the type of detection and must
     * be {@link #v_Continuous} or {@link #v_Discrete}.
     * <br> May be set in the WordTrained state.
     */
    Symbol p_DetectionType		= ESymbol.ASR_DetectionType;
    /**
     * Value is a Symbol describing the availabiility of spotting and must
     * be Available or NotAvailable.
     * <br> May be set in the WordTrained state.
     */
    Symbol p_Spotting			= ESymbol.ASR_Spotting;
    /**
     * Value is a Symbol describing the manner in which the utterance
     * data is described to the recogniser for training and must
     * be {@link #v_Phonetic}, {@link #v_Speech}, or {@link #v_Text}.
     * <br> May be set in the WordTrained state.
     */
    Symbol p_UtteranceType		= ESymbol.ASR_UtteranceType;
    /**
     * Value is a vendor-defined heuristic describing the space requirements
     * of the context when resident on the resource. Size is implicitly
     * adjusted when training is performed on the resource.
     * <br> Read-Only.
     */
    Symbol p_Size			= ESymbol.ASR_Size;
    /**
     * Value is a Symbol indicating the actual type of the context to be
     * loaded. The only ECTF-defined type is JSGF. Otherwise this is a
     * vendor-defined symbol.
     * <br> May be set in the WordTrained state.
     */
    Symbol p_ContextType		= ESymbol.ASR_ContextType;

    /* Table 5-7: Speech Input Control Parameters */
    /**
     * Value is maximum time window (in msecs). 
     * After this time the recognition terminates. 
     * <br> Possible values are 0 to 1000000 and
     * {@link ResourceConstants#v_Forever}.
     * <br> Default value is {@link ResourceConstants#v_Forever}
     */
    Symbol p_Duration			= ESymbol.ASR_Duration;
    /**
     * Value is the initial timeout in msecs. If no utterance is detected
     * in this period, recognition will be terminated and silence will
     * be reported.
     * <br> Possible values are 0 to 1000000 and
     * {@link ResourceConstants#v_Forever}.
     * <br> Default vlue is {@link ResourceConstants#v_Forever}
     */
    Symbol p_InitialTimeout		= ESymbol.ASR_InitialTimeout;
    /**
     * Value is the silence time in msecs after the utterance to indicate
     * the completion of the recognition.
     * <br> Possible values are 0 to 1000000 and
     * {@link ResourceConstants#v_Forever}.
     * <br> Default vlue is {@link ResourceConstants#v_Forever}
     */
    Symbol p_FinalTimeout		= ESymbol.ASR_FinalTimeout;
    /**
     * Value is a boolean that is true if echo cancellation is active.
     * <br> Default value is true.
     */
    Symbol p_EchoCancellation		= ESymbol.ASR_EchoCancellation;

    /**
     * Value is a minimum score at a barge-in node required to raise
     * {@link #rtcc_GrammarBargeIn}.
     * <br>Possible values are 0 to 1000.
     * <br>Default valueis 500.
     */
    Symbol p_GrammarBargeInThreshold	= ESymbol.ASR_GrammarBargeInThreshold;

    /* Table 5-8: Recognizer states */
    
    /**
     * Value is the state of the recognizer.
     * <br> Possible values are {@link #v_Idle}, {@link #v_RecognitionPaused},
     * {@link #v_Recognizing}, {@link #v_ResultsAvailable},
     * {@link #v_TrainingPaused}, {@link #v_Training}, and
     * {@link #v_WordTrained}.
     * <br> Default is  {@link #v_Idle}.
     * <p> Read-Only.
     */
    Symbol p_State			= ESymbol.ASR_State;

    /* Table 5-9: Output Control parameters */
    /**
     * Value is the number of words to return.
     * <br>Default value is 1.
     */
    Symbol p_NumberWordsRequested	= ESymbol.ASR_NumberWordsRequested;
    /**
     * Value is the type of output result provided.
     * <br> Possible values are {@link #v_Text} and {@link #v_Phonetic}.
     * <br> Default value is {@link #v_Text}.
     */
    Symbol p_OutputType	= ESymbol.ASR_OutputType;
    
    /**
     * Value is the number of alternative results returned.
     * <br> Default value is 1.
     */
    Symbol p_TopChoices	= ESymbol.ASR_TopChoices;
    
    /**
     * Value is a Symbol indicating if the result are final or intermediate.
     * <br> Possible values are {@link #v_Final} and {@link #v_Intermediate}.
     * <br> Default value is  {@link #v_Final}.
     */
    Symbol p_ResultType	= ESymbol.ASR_ResultType;
    
    /**
     * Value is the method fo result presentation.
     * <br> Possible values are {@link #v_TypeI} and {@link #v_TypeII}.
     * <br> Default value is {@link #v_TypeII}.
     */
    Symbol p_ResultPresentation	= ESymbol.ASR_ResultPresentation;

    /* Table 5-10: Training Control Parameters */

    /** AutomaticTraining. */
    Symbol p_AutomaticTraining	= ESymbol.ASR_AutomaticTraining;

    /** NumberRepetitions in Training. */
    Symbol p_NumberRepetitions	= ESymbol.ASR_NumberRepetitions;

    /**
     * A Dictionary of ASR event Symbols, indicating which events 
     * are generated and delivered to the application.
     * If a given eventID is not enabled, then no processing
     * is expended to generate or distribute that kind of event.
     * <p>
     * Value is a Dictionary of non-transactional event Symbols.
     * <br>Valid keys: <code> ev_RecognitionStarted, ev_onValidUtterance, 
     * ev_InvalidUtterance, ev_IntermediateResultsReady</code>
     * <br>Valid values: <code>Boolean.TRUE, Boolean.FALSE</code>
     * <br>Default value = <i>no events</i>.
     * <p>
     * For a ASRListener to receive events, those events must
     * be enabled by setting this parameter.  
     * This parameter can be set in a {@link ConfigSpec} or by
     * {@link MediaService#setParameters setParameters}.
     * <p>
     * <b>Note:</b>
     * This parameter controls the generation of events.
     * The events cannot be delivered unless a {@link ASRListener}
     * is added using 
     * {@link MediaService#addMediaListener addMediaListener}. 
     */
    Symbol p_EnabledEvents	= ESymbol.ASR_EnabledEvents;
    /** TrainingType. */
    Symbol p_TrainingType	= ESymbol.ASR_TrainingType;

    /* Table 5-11: Speech Buffer Control Parameters */
    /** StoreInput in Speech Buffer. */
    Symbol p_StoreInput	= ESymbol.ASR_StoreInput;
    /** PlayInput in Speech Buffer. */
    Symbol p_PlayInput	= ESymbol.ASR_PlayInput;

    /* Table 5-12: RTC Action Paramters */ 
    /** Indicates if Recognizer starts in Paused mode.
     * <p>Value is a Boolean.
     */
    Symbol p_StartPaused	= ESymbol.ASR_StartPaused;
    /**
     * Utterances which triggers rtcc_SpecficUtterance.
     * <p>Value is a String[], each String represents an utterance.
     * When any of these utterances are recognized,
     * {@link #rtcc_SpecificUtterance} is triggered.
     */
    Symbol p_SpecificUtterance	= ESymbol.ASR_SpecificUtterance;

    /**
     * These parameters are updated when {@link #rtca_UpdateParameters} fires.
     */
    Symbol p_UpdateParametersList	= ESymbol.ASR_UpdateParametersList;

    /* Table 33: Runtime Control Actions */

    /**
     * Start the paused recognition or training.
     * <br>Next state is <tt>Recognizing</tt> or <tt>Training</tt>.
     */
    Symbol rtca_Start			= ESymbol.ASR_Start;

    /**
     * Stop recognition or training.
     * <br>Next state is ResultsAvailable or WordTrained.
     */
    Symbol rtca_Stop			= ESymbol.ASR_Stop;

    /**
     * Stop recognition or training and return to Idle state.
     * <br>Next state is Idle.
     */
    Symbol rtca_Idle			= ESymbol.ASR_Idle;
    /**
     * Update parameters given in {@link #p_UpdateParametersList}.
     */
    Symbol rtca_UpdateParameters	= ESymbol.ASR_UpdateParameters;

    /* Table 34: Runtime Control Conditions */

    /**
     * Trigger when the recognizer has start operating.
     */
    Symbol rtcc_RecognizeStarted	= ESymbol.ASR_RecognizeStarted;

    /**
     * Trigger when the recognizer has start training.
     */
    Symbol rtcc_TrainStarted		= ESymbol.ASR_TrainStarted;

    /**
     * Trigger when speech has been detected.
     */
    Symbol rtcc_SpeechDetected		= ESymbol.ASR_SpeechDetected;

    /**
     * Trigger when a specifically defined utterance is detected.
     * @see #p_SpecificUtterance
     */
    Symbol rtcc_SpecificUtterance	= ESymbol.ASR_SpecificUtterance;
    /**
     * Trigger when recognition ends normally.
     */
    Symbol rtcc_Recognize		= ESymbol.ASR_Recognize;
    /**
     * Trigger when word training ends normally.
     */
    Symbol rtcc_WordTrain		= ESymbol.ASR_WordTrain;
    /**
     * Trigger when word training is complete.
     */
    Symbol rtcc_WordTrained		= ESymbol.ASR_WordTrained;
    /**
     * Trigger when a valid utterance has been detected even though
     * recognition may not be finished.
     */
    Symbol rtcc_ValidUtteranceFound	= ESymbol.ASR_ValidUtteranceFound;
    /**
     * Trigger when a valid utterance has been detected and recognition
     * has completed.
     */
    Symbol rtcc_ValidUtteranceFinal	= ESymbol.ASR_ValidUtteranceFinal;
    /**
     * Trigger when silence is detected.
     */
    Symbol rtcc_Silence			= ESymbol.ASR_Silence;
    /**
     * Trigger when an invalid utterance is detected.
     */
    Symbol rtcc_InvalidUtterance	= ESymbol.ASR_InvalidUtterance;
    /**
     * Trigger when the grammar has detected barge-in.
     */
    Symbol rtcc_GrammarBargeIn		= ESymbol.ASR_GrammarBargeIn; 

     
    /**
     * A Value indicating the results are of Type I.
     * @see #a_ResultPresentation
     * @see #p_ResultPresentation
     * @see ASREvent#getResultPresentation getResultPresentation
     */
    Symbol v_TypeI			= ESymbol.ASR_TypeI;
	 
    /**
     * A Value indicating the results are of Type II.
     * @see #a_ResultPresentation
     * @see #p_ResultPresentation
     * @see ASREvent#getResultPresentation getResultPresentation
     */
    Symbol v_TypeII 		= ESymbol.ASR_TypeII;
	 
    /* Table 45: Optional Keys - Type I Results */
    /* Optional Qualifier data */
    // Symbol key_Qualifier	= ESymbol.ASR_Qualifier;
    
    /**
     * a value indicating the token is normal, and contains a string.
     * @see ASREvent.TokenSequence#getTokenQualifier getTokenQualifier
     * @see ASREvent.TokenSequence#getSequenceQualifier getSequenceQualifier
     */
    Symbol v_Normal		= ESymbol.ASR_Normal;
    
    /**
     * a value indicating the token represents garbage.
     * @see ASREvent.TokenSequence#getTokenQualifier getTokenQualifier
     * @see ASREvent.TokenSequence#getSequenceQualifier getSequenceQualifier
     */
    Symbol v_Garbage		= ESymbol.ASR_Garbage;
    
    /**
     * a value indicating the token represents silence.
     * @see ASREvent.TokenSequence#getTokenQualifier getTokenQualifier
     * @see ASREvent.TokenSequence#getSequenceQualifier getSequenceQualifier
     */
    Symbol v_Silence		= ESymbol.ASR_Silence;
    
    /**
     * A value indicating that the token is NULL, 
     * and a grammar tag is provided instead.
     * @see ASREvent.TokenSequence#getTokenQualifier getTokenQualifier
     */
    Symbol v_GrammarTag		= ESymbol.ASR_GrammarTag;

    /* Table 5-16: Training Word Status */
    
    /**
     * The Word has sufficient training available.
     * @see ASREvent#getReadiness getReadiness
     */
    Symbol v_Ready		= ESymbol.ASR_Ready;
    
    /**
     * The Word does not have sufficient training available.
     * @see ASREvent#getReadiness getReadiness
     */
    Symbol v_NotReady		= ESymbol.ASR_NotReady;
    
    /**
     * The Word has sufficient training available,
     * and additional training will be ignored.
     * @see ASREvent#getReadiness getReadiness
     */
    Symbol v_Complete		= ESymbol.ASR_Complete;

    /* Table 5-18: Training Warning */
    
    /**
     * The training succeeded.
     * @see ASREvent#getTrainingResult getTrainingResult()
     */
    Symbol v_Success		= ESymbol.ASR_Success;
    
    /**
     * The training is too close to the training of another Word in an
     * active Context on the Recognizer.
     * @see ASREvent#getTrainingResult getTrainingResult()
     */
    Symbol v_Collision		= ESymbol.ASR_Collision;
    
    /**
     * The training is inconsistent with previous training for this Word.
     * @see ASREvent#getTrainingResult getTrainingResult()
     */
    Symbol v_Inconsistent	= ESymbol.ASR_Inconsistent;
    
    /**
     * The background for this training was too noisy.
     * @see ASREvent#getTrainingResult getTrainingResult()
     */
    Symbol v_Noisy		= ESymbol.ASR_Noisy;

    /* Table 5-20: ASR Symbols */
    
    /**
     * The recogniser is idle.
     * @see #p_State
     */
    Symbol v_Idle		= ESymbol.ASR_Idle;
    
    /**
     * The recogniser is paused.
     * @see #p_State
     */
    Symbol v_RecognitionPaused	= ESymbol.ASR_RecognitionPaused;
    
    /**
     * The recogniser is recognising.
     * @see #p_State
     */
    Symbol v_Recognizing	= ESymbol.ASR_Recognizing;
    
    /**
     * The recogniser has results available.
     * @see #p_State
     */
    Symbol v_ResultsAvailable	= ESymbol.ASR_ResultsAvailable;
    
    /**
     * The recogniser is training and is paused.
     * @see #p_State
     */
    Symbol v_TrainingPaused	= ESymbol.ASR_TrainingPaused;
    
    /**
     * The recogniser is training.
     * @see #p_State
     */
    Symbol v_Training		= ESymbol.ASR_Training;
    
    /**
     * The recogniser has trained a word.
     * @see #p_State
     */
    Symbol v_WordTrained	= ESymbol.ASR_WordTrained;
        
    //Symbol v_Ready	= ESymbol.ASR_Ready;
    //Symbol v_NotReady	= ESymbol.ASR_NotReady;
    
    /**
     * Value of a parameter indicating that results are returned as text.
     * @see #p_UtteranceType
     * @see #p_OutputType
     */
    Symbol v_Text		= ESymbol.ASR_Text;
    
    /**
     * Value of a parameter indicating that results are returned as a
     * phonetic transcription.
     * @see #p_UtteranceType
     * @see #p_OutputType
     */
    Symbol v_Phonetic		= ESymbol.ASR_Phonetic;
    
    /**
     * Value of a parameter indicating that results are returned as speech.
     * @see #p_UtteranceType
     * @see #p_OutputType
     */
    Symbol v_Speech		= ESymbol.ASR_Speech;
    
    /**
     * Value indicates speaker independent recognition is supported.
     * @see #p_SpeakerType
     * @see #a_SpeakerType
     */
    Symbol v_Independent	= ESymbol.ASR_Independent;
    
    /**
     * Value indicates speaker dependent recognition is supported.
     * @see #p_SpeakerType
     * @see #a_SpeakerType
     */
    Symbol v_Dependent	= ESymbol.ASR_Dependent;
    
    /**
     * Value indicates speaker identification is supported.
     * @see #p_SpeakerType
     * @see #a_SpeakerType
     */
    Symbol v_Identification	= ESymbol.ASR_Identification;
    
    /**
     * Value indicates speaker verification is supported.
     * @see #p_SpeakerType
     * @see #a_SpeakerType
     */
    Symbol v_Verification	= ESymbol.ASR_Verification;
    
    /**
     * Value indicates a detection type of continuous.
     * @see #p_DetectionType
     * @see #a_DetectionType
     */
    Symbol v_Continuous		= ESymbol.ASR_Continuous;
    
    /**
     * Value indicates a detection type of discrete.
     * @see #p_DetectionType
     * @see #a_DetectionType
     */
    Symbol v_Discrete		= ESymbol.ASR_Discrete;
    
    //Symbol v_FullWord		= ESymbol.ASR_FullWord;
    
    /**
     * Value indicates that intermediate results may change with time.
     * @see #a_IntermediateResults
     */
    Symbol v_Mutable		= ESymbol.ASR_Mutable;
    
    /**
     * Value indicates that intermediate results do not change with time.
     * @see #a_IntermediateResults
     */
    Symbol v_Immutable		= ESymbol.ASR_Immutable;
    
    /**
     * Value indicates that the word cannot be modified after being committed
     * to context.
     * @see #a_TrainingModifiable
     */
    Symbol v_NotModifiable	= ESymbol.ASR_NotModifiable;
    
    /**
     * Value indicates that the word can be retrained after it has been added
     * to Context.
     * @see #a_TrainingModifiable
     */
    Symbol v_Retrain		= ESymbol.ASR_Retrain;
    
    /**
     * Value indicates that the word can have additional training added after
     * is has been added to Context.
     * @see #a_TrainingModifiable
     */
    Symbol v_AddTraining	= ESymbol.ASR_AddTraining;
    
    /**
     * Value indicates the results are final.
     * @see #p_ResultType
     */
    Symbol v_Final		= ESymbol.ASR_Final;
    
    /**
     * Value indicates the results are intermediate.
     * @see #p_ResultType
     */
    Symbol v_Intermediate	= ESymbol.ASR_Intermediate;
    
    //Symbol v_TypeI		= ESymbol.ASR_TypeI;
    
    //Symbol v_TypeII		= ESymbol.ASR_TypeII;

    /* Table 5-21: ASR Event Qualifier values */
    
    /**
     * The latest utterance was successfully recognised.
     * @see ResourceEvent#getQualifier getQualifier()
     */
    Symbol q_Success		= ESymbol.ASR_Success;
    
    /**
     * The training or recognition was stopped via
     * the ASR Stop command or a RTC.
     * @see ResourceEvent#getQualifier getQualifier()
     */
    Symbol q_Stop		= ESymbol.ASR_Stop;
    
    /**
     * The training failed because the initial timeout timer expired.
     * @see ResourceEvent#getQualifier getQualifier()
     */
    Symbol q_InitialTimeout	= ESymbol.ASR_InitialTimeout;
    
    /**
     * The latest utterance was not successful.
     * @see ResourceEvent#getQualifier getQualifier()
     */
    Symbol q_Failure		= ESymbol.ASR_Failure;
    
    /**
     * Although the latest utterance was added to the training set,
     * the Recognizer has detected potential problems. 
     * Use * {@link ASREvent#getTrainingResult} 
     * to determine the type of problem.
     * @see ResourceEvent#getQualifier getQualifier()
     */
    Symbol q_Warning		= ESymbol.ASR_Warning;

    /**
     * Training is complete.
     * @see ResourceEvent#getQualifier getQualifier()
     */
    Symbol q_Complete		= ESymbol.ASR_Complete;

    /**
     * The recogniser heard no utterance.
     * @see ResourceEvent#getQualifier getQualifier()
     */
    Symbol q_Silence		= ESymbol.ASR_Silence;

    /**
     * Training was unsuccessful due to a collision.
     * @see ResourceEvent#getQualifier getQualifier()
     */
    Symbol q_Unsuccessful	= ESymbol.ASR_Unsuccessful;
    
    /**
     * The recogniser rejected the utterance as unrecognisable.
     * @see ResourceEvent#getQualifier getQualifier()
     */
    Symbol q_Rejected		= ESymbol.ASR_Rejected;

    /* other Symbols */

    /** direction argument in contextCopy(). */
    Symbol v_FromResource 		= ESymbol.ASR_FromResource;
    /** direction argument in contextCopy(). */
    Symbol v_ToResource 		= ESymbol.ASR_ToResource;
    /** direction argument in contextCopy(). */
    Symbol v_ToResourceTraining 	= ESymbol.ASR_ToResourceTraining;

    //    Symbol key_RuleExpansion	= ESymbol.ASR_RuleExpansion;

    //    Symbol ev_RuleExpansion	= ESymbol.ASR_RuleExpansion;

    /** Indicates the ASR Resource Class in a ResourceSpec. */
    Symbol v_Class		= ESymbol.ASR_ResourceClass;
}

