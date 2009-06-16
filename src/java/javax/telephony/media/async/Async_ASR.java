/*
 * Async_ASR.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

package javax.telephony.media.async;

import javax.telephony.media.*;
import java.util.Dictionary;

/**
 * Asynchronous version of ASR Resource methods.
 * <p>
 * The Automatic Speech Recognition (ASR) resource performs recognition
 * and/or training. An ASR resource that performs recognition has associated
 * with it a recognition algorithm which uses a context to recognize words 
 * from utterances presented in an input media stream, returning recognition
 * results to the application.
 * <p>
 *
 * <p>
 * @see javax.telephony.media.ASR ASR Interface
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */


public interface Async_ASR extends ASRConstants 
{
    /**
     * Non-blocking version of recognize().
     * <p>
     * Initiate speech recognition on the ASR resource.
     * The returned event contains only the highest-probablility result.
     * That is, it contains at most one <tt>TokenSequence</tt>.
     *
     * @param rtcs a RTC[] containing RTCs to be in effect during this call.
     * @param optargs a Dictionary containing optional arguments.
     * @return an Async_ASREvent
     * 
     * @see ASREvent
     */
    Async_ASREvent async_recognize(RTC[] rtcs, Dictionary optargs);

    /**
     * Non-blocking version of getResults().
     * <p>
     * Retrieve intermediate recognition results 
     * and do <b>not</b> reset the recognizer.
     * This method is valid whenever partial results are ready.
     * The recognizer can be in either of state
     * <tt>Recognizing</tt> or <tt>ResultsReady</tt>.
     *
     * @return an async_ASREvent
     */
    Async_ASREvent async_getResults(); 

    /**
     * Non-blocking version of getFinalResults().
     * <p>
     * Retrieve recognition results and reset the recognizer.
     * <h4>Pre-conditions</h4>
     * Recognizer must be in the ResultsAvailable state.
     * <h4>Post-conditions</h4>
     * Recognizer is in the Idle state.
     * <p>
     * This is roughly equivalent to <tt>getResults()</tt>
     * followed by <tt>triggerRTC(ASR.rtca_Idle)</tt>.
     *
     * @return an async_ASREvent
     */
    Async_ASREvent async_getFinalResults();

    /**
     * Non-blocking version of startASR().
     * <p>
     * Starts a recognizer that is in a paused state.
     * The Paused states are <tt>v_RecognitionPaused</tt> and
     * <tt>v_TrainingPaused</tt>; the Recognizer moves to the states
     * <tt>v_Recognizing</tt> and <tt>v_Training</tt>, respectively.
     *
     * @return an async_ASREvent
     */
    Async_ASREvent async_startASR();

    /**
     * Non-blocking version of stopASR().
     * <p>
     * Stops the Recognizer when it is in the
     * "Recognizing" or "Training" states.
     * The recognizer transitions to the next state 
     * as if though they had completed execution. 
     * For example, the ASR resource would move from <tt>v_Recognizing</tt>
     * to <tt>v_ResultsAvailable</tt>.
     *
     * @return an async_ASREvent
     */
    Async_ASREvent async_stopASR();

    /**
     * Non-blocking version of idleASR().
     * <p>
     * Forces a Recognizer into the Idle state. 
     * <br><i>Is this a transaction? or a void return?</i><br>
     * <p>
     * The function is valid at any time, and is commonly used to move
     * a Recognizer from the <tt>v_ResultsAvailable</tt> state into the
     * <tt>v_Idle</tt> state without retrieving results from the Recognizer.
     * Note that on entry to the Idle state, any recognition results or
     * uncommitted training data is lost.
     * <p>
     * This function is also available as <tt>ASR.rtca_idle</tt>.
     *
     * @return an async_ASREvent
     */
    Async_ASREvent async_idleASR();

    /**
     * Non-blocking verions of updateParameters().
     * <p>
     * Set resource parameters according to <tt>p_UpdateParametersList</tt>.
     * <p>
     * This function (also available as <tt>ASR.rtca_updateParameters</tt>)
     * is useful for <i>barge-in</i> scenarios, where the <tt>timeout</tt>
     * values change after the prompt has completed playing.
     * <p>
     * If <tt>p_UpdateParametersList</tt> is not set or empty,
     * then nothing is done and this method returns normally.
     *
     * @return an async_ASREvent
     */
    Async_ASREvent async_updateParameters();

    /**
     * Non-blocking version of getRuleExpansion().
     * Retrieve the expansion of a rule from a Grammar Context.
     * <p>
     * For a Grammar Context loaded on a resource, this method
     * retrieves the current expansion of the rule RuleName 
     * in the Grammar Context <tt>GrammarContextName</tt>. 
     * The rule expansion is returned as part of
     * the completion event for this function and is available using
     * {@link ASREvent#getRuleExpansion ASREvent.getRuleExpansion()}.
     *
     * @param grammarContext the name of a loaded grammar Context.
     * @param ruleName a String that names the rule to be expanded.
     *
     * @return an async_ASREvent
     */
    Async_ASREvent async_getRuleExpansion(String grammarContext, String ruleName);

    /**
     * Non-blocking version of setRuleExpansion().
     * Set the expansion of a rule in a Grammar Context.
     * <p>
     * For a loaded Grammar Context, this function sets the rule expansion
     * of the rule <tt>ruleName</tt> to the rule expansion given in 
     * the <tt>ruleExpansion</tt> argument. 
     * The syntax of RuleExpansion is defined by the JSGF grammar
     * specification language.
     * 
     * @param grammarContext a loaded Context.
     * @param ruleName a String that names the rule being defined.
     * @param ruleExpansion a String containing the rule expansion.
     *
     * @return an async_ASREvent
     */
    Async_ASREvent async_setRuleExpansion(String grammarContext, 
					  String ruleName,
					  String ruleExpansion);
 
    /** Training methods */
    /**
     * Commits a <b>Word</b>, as trained, into a Context.  
     * Invoked when sufficient information has been collected 
     * to indicate that the Context may be permanently modified 
     * based on the updated training information. 
     * <p>
     * The argument <tt>tempWord</tt> identifies the <b>Word</b>
     * to be committed. 
     * When an utterance corresponding to that <b>Word</b> is detected, 
     * the recognizer will return <tt>wordString</tt> as the result.
     * <tt>wordString</tt> is not the return value of the function --
     * <tt>wordString</tt> is the value the Recognizer will return 
     * as an answer in the <tt>Token</tt> field in the result object. 
     * <tt>wordString</tt> is <i>permanent</i> -- once set, 
     * the association between the <b>Word</b> and <tt>wordString</tt>
     * cannot be changed.
     * <tt>wordString</tt> is not necessarily an exact transcription 
     * of the utterance; in most cases, an arbitrary String is used.
     * <p>
     * For example, in a scenario where arbitrary voice
     * labels are associated with a telephone number, the actual
     * transcription is unknown. <tt>wordString</tt> can be an 
     * arbitrary index, and that index can used by the application 
     * to find the correct telephone number. 
     * <p>
     * If this function is used to commit additional training to an
     * already-existing <b>Word</b>, then <tt>wordTemp</tt> should be
     * the permanent name for the <b>Word</b>; 
     * in that case, the value of <tt>wordString</tt> is ignored.
     * 
     * @param wordContext the Context to which this word is committed.
     * @param wordTemp a String that identifes the word to commit.
     * @param wordString the permanent String to identify this word.
     *
     * @return an async_ASREvent
     */
    Async_ASREvent async_wordCommit(String wordContext, 
				    String wordTemp, 
				    String wordString);

    /**
     * Create a new <b>Word</b> within a loaded Context.
     * The <b>Word</b> is accessed by using <tt>wordTemp</tt>,
     * a temporary identifier. This method can only be used with
     * Recognizers that support Context modification. Creating a
     * <b>Word</b> adds it to the Context's <tt>p_WordList</tt> parameter.
     * 
     * @param wordContext the Context in which Word is created.
     * @param wordTemp a String containing the temporary name of the word.
     * @return an async_ASREvent
     */
    Async_ASREvent async_wordCreate(String wordContext, String wordTemp);

    /**
     * Prevent the previous utterance from contributing to training.
     * The most recent utterance associated with <tt>train()</tt> 
     * is not used as the training of a <b>Word</b>.
     * <p>
     * This method must be issued before any new training is made
     * or before the training is committed to the Context. 
     * That is, this method must be issued before any other train(),
     * and commit() method; either of those methods makes the utterance
     * part of the permanent training of the <b>Word</b>.
     * 
     * @param wordContext the Context in which the work is being trained.
     * @param wordTemp the String that identifies the word being trained.
     *
     * @return an async_ASREvent
     */
    Async_ASREvent async_wordDeleteLastUtterance(String wordContext, String wordTemp);

    /**
     * Delete all training associated with a Word in the
     * specified Context. 
     * <p>
     * If the Word has been committed, then <tt>wordString</tt> 
     * must be the permanent name of the Word.
     * If the Word has not been committed, then <tt>wordString</tt> 
     * must be the temporary name.
     *
     * @param wordContext the Context from which the training is deleted.
     * @param wordString the String that idenifies word to be deleted.
     *
     * @return an async_ASREvent
     */
    Async_ASREvent async_wordDeleteTraining(String wordContext, String wordString);

    /**
     * Remove a word from a loaded Context.
     * This ASR Resource must support Context modification.
     * Destroying a word will remove it from the Context's
     * <tt>p_WordList</tt> parameter.
     * <p>
     * If the Word has been committed, then <tt>wordString</tt> 
     * must be the permanent name of the Word.
     * If the Word has not been committed, then <tt>wordString</tt> 
     * must be the temporary name.
     *
     * @param wordContext the Context from which the Word is removed.
     * @param wordString the String that identifies the word.
     *
     * @return an async_ASREvent
     */
    Async_ASREvent async_wordDestroy(String wordContext, String wordString);

    /**
     * Train a Word in a Context. The Recognizer
     * collects audio information (or other information; 
     * {@link ASRConstants#p_TrainingType <tt> p_TrainingType</tt>}), 
     * and associates this information with the Word. 
     * <P>
     * The Word might be one that has been trained before,
     * or one that has just been created. 
     * If the Word has been committed, then <tt>wordString</tt> 
     * must be the permanent name of the Word.
     * If the Word has not been committed, then <tt>wordString</tt> 
     * must be the temporary name.
     * <p>
     * Not all Recognizers can add additional training to a Word
     * that has already been committed to a
     * Context. The attribute <tt>a_TrainingModifiable</tt> may be queried
     * to determine the Recognizer's abilities:
     * <br><tt>v_NotModifiable</tt> means that training cannot be modified. 
     * <br><tt>v_Retrain</tt> means that retraining the Word is possible, 
     * but only by performing a asrTrain.deleteTraining() and 
     * retraining the Word entirely. 
     * <br><tt>v_AddTraining</tt> means that additional training
     * may be added to a Word by using the asrTrain.train() command.
     * <p>
     * Some Recognizers may perform training automatically,
     * issuing prompts and collecting utterances until a sufficient
     * number of good utterances have been collected. Such Recognizers,
     * which have the attribute <tt>a_AutomaticTraining</tt> set to true,
     * will run in automatic mode when the parameter
     * <tt>p_AutomaticTraining</tt> is set to true. When training is over,
     * they will issue a single completion event. The parameter
     * <tt>p_NumberRepetitions</tt>, available in some Recognizers, may be
     * set to dictate how many training utterances the Recognizer
     * should use in AutomaticTraining mode.
     * <p>
     * Recognizers have different requirements for the minimum number of
     * utterances necessary to provide training for a new Word; if the
     * application is collecting utterances, the application will
     * need to provide at least that many utterances to the Recognizer.
     * The range of the <tt>p_NumberRepetitions</tt> parameter may be
     * queried to determine this number, which is indicated by the
     * minimum number of repetitions. The Recognizer may also have an
     * upper limit on the number of utterances it can use for training;
     * the <tt>p_NumberRepetitions</tt> parameter may also be queried to
     * determine this number, which is the maximum number of
     * repetitions.  
     * <p>
     * Different types of training may be available on Recognizers; the
     * <tt>a_TrainingType</tt> attribute indicates which types are
     * available.  The parameter <tt>p_TrainingType</tt> is used to set
     * the type when more than one type is supported:
     * <tt>p_TrainingType</tt> takes on the values <tt>v_Speech</tt>,
     * <tt>v_Text</tt>, and <tt>v_Phonetic</tt> to describe the training
     * input.
     * Recognizers that accept training of type <tt>v_Phonetic</tt> use
     * IPA as their text input for training. If <tt>p_TrainingType</tt>
     * is set to <tt>v_Text</tt> or <tt>v_Phonetic</tt>, the dictionary
     * entry <tt>p_TrainingInfo</tt> must also be present. This array of
     * String will contain either the text or phonetic representation of
     * the training material, as appropriate.
     * 
     * @param wordContext the Context in which the word is trained.
     * @param wordString a String that identifies the word to be trained.
     *
     * @return an async_ASREvent
     */
    Async_ASREvent async_wordTrain(String wordContext, String wordString);

    /** Context methods */
    /**
     * Copy a Context from a Resource to a Container, 
     * or from a Container to a Resource.
     * This method can either 
     * store a loaded Context into a Container
     * or 
     * load a context into the Resource. 
     * <p>
     * The method <tt>contextCreate</tt> must be used to 
     * create the Context on the resource before using
     * this method.
     * <i>If not, should we auto-define that Context?
     *  or should we define the Exception that is thrown?</i>
     * <p>
     * The <tt>direction</tt> Symbol specifies whether the copy is
     * from a resource to a container (<tt>v_FromResource</tt>)
     * or
     * from a container to a resource (<tt>v_ToResource</tt>). 
     * <p>
     * To copy a Context from a Container to the Resource 
     * for the purpose of training, then the symbol 
     * <tt>v_ToResourceTraining</tt> should be used.
     * <p>
     * When copying a Context <i>to</i> the Resource, the argument
     * <tt>resourceContext</tt> assigns a name to the Context. 
     * This name is used to identify the Context as loaded in the Resource.  
     * It does not need to be the same name used to identify 
     * the Context in a Container.
     * <p>
     * This is a non-destructive copy; the source copy of the Context is
     * unaffected. If a Context of the same name already exists at the
     * destination, it will be overwritten and lost. If the destination
     * is a container, a new Context Object will be created to
     * accommodate the Context if necessary. If the destination is an
     * ASR Resource, and the Resource currently has a Context, and the
     * Resource does not support multiple Contexts or the Resource has
     * no room for this Context the copy will fail. In such cases the
     * application must take corrective action, e.g., free up room on
     * the ASR Resource by performing a asrContext.remove().
     * <p>
     * A Context copied into the Resource is inactive,
     * it joins the set of inactive contexts.
     * <p>
     *
     * @param contextResource a Context within the ASR Resource.
     * @param containerContext a String containing the name of the container
     * context.
     * @param direction a Symbol indicating the direction and type of copy.
     * Must be one of {@link ASRConstants#v_ToResource},
     * {@link ASRConstants#v_FromResource}, or
     * {@link ASRConstants#v_ToResourceTraining}.
     *
     * @return an async_ASREvent
     */
    Async_ASREvent async_contextCopy(String contextResource,
				     String containerContext, 
				     Symbol direction);

    /**
     * Creates a new context on the ASR Resource. 
     * A new context is created on the ASR Resource and is associated
     * with the name of the supplied Context.  The Context object
     * should be used for all further operations on that context.
     * <p>
     * The ASR Resource implementation must support Context modification.
     * <p>
     * The <tt>trainingType</tt> argment indicates the type of 
     * input used to perform training. 
     * The possible values are:
     * <tt>v_Speech</tt>, <tt>v_Phonetic</tt>, <tt>v_Text</tt>
     * <p>
     * Any context parameter -- including those that are normally 
     * read-only and cannot be written by the application - may be set
     * during the time of creation by sending the appropriate Dictionary
     * entry in <tt>contextParms</tt>. 
     * <p>
     * @param resourceContext a Context to be created in the Resource.
     * @param trainingType a Symbol that identifies the type of input 
     * to be used for training this Context.
     * @param contextParams a Dictionary containing the context parameters.
     *
     * @return an async_ASREvent
     */
    Async_ASREvent async_contextCreate(String resourceContext,
				       Symbol trainingType,
				       Dictionary contextParams);

    /**
     * Retrieve context parameter values for a Context
     * loaded in this resource. 
     * <p>
     * If a key refers to a Parameter that is not present,
     * or the Context has no meaning associated with a particular Symbol,
     * or that parameter cannot be returned, 
     * then the key is ignored, no error is generated, 
     * and that key does not appear returned Dictionary.
     * <p>
     * If there is not ASR Resource configured in this MediaService,
     * or if the ASR Resource cannot return parameters for the
     * given Context, or if the Context is not loaded in the
     * ASR Resource, then <tt>null</tt> is returned.
     * <p>
     * 
     * @param resourceContext the Context from which parameters are retrieved
     * @param keys an array of Symbols identifying the requested parameters.
     *
     * @return an async_ASREvent
     */
    Async_ASREvent async_contextGetParameters(String resourceContext, Symbol[] keys);

    /**
     * Remove an existing Context from an ASR Resource.
     * The Context which is removed is lost.
     * <p>
     * To preserve a Context, copy it to a Container using the
     * contextCopy() method.
     *
     * @param resourceContext the Context to be removed.
     *
     * @return an async_ASREvent
     */
    Async_ASREvent async_contextRemove(String resourceContext);

    /**
     * This function sets the parameters associated with a particular
     * Context that is loaded on a Resource.
     *
     * @param resourceContext the Context on which parameters are to be set.
     * @param contextParams a Dictionary of parameters to set.
     *
     * @return an async_ASREvent
     */
    Async_ASREvent async_contextSetParameters(String resourceContext, 
					      Dictionary contextParams);

}

