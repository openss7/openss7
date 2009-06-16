/*
 * ASR.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Automatic Speech Recognition API.
 * <p>
 * The Automatic Speech Recognition (ASR) resource performs recognition
 * and/or training. An ASR resource that performs recognition has associated
 * with it a recognition algorithm which uses a context to recognize words 
 * from utterances presented in an input media stream, returning recognition
 * results to the application.
 * <p>
 * An ASR resource that performs training creates
 * or updates words in a context for use by the recognition algorithm.
 * <p>
 * An ASR resource configured in a group contains a default context,
 * containing a set of words that the ASR's recognition algorithm can
 * potentially match in an input media stream. The context may contain a
 * grammar which constrains the search space searched by the recognition
 * algorithm. Contexts have a large number of associated parameters,
 * including the language and language variant of its words, the list of
 * words in the context (if the context contains words), and other information
 * required for ASR. The recognition algorithm returns recognition results.
 * The recognition results are a sequence of strings associated with the
 * recognized words, along with some details describing the results.
 * <p>
 * A client may train a word within a context, by collecting utterances and
 * presenting them to the resource's training algorithm. The ASR resource
 * interacts with the application to collect a sufficient number of utterances
 * to train the word correctly.
 * <p>
 * A client may load a context from a container into an ASR resource, and may
 * store a context from an ASR resource into a container.
 * <p>
 * When supported, the ASR resource may be used for speaker identification
 * and speaker verification, e.g., by training a context with utterances from
 * a particular speaker.
 * <p>
 * When supported, the ASR resource may be used for language identification.
 * <p>
 * ASR resources have a large set of attributes, which describe the various
 * types of ASR capabilities which may be supported by the resource; and by
 * a large set of parameters which control the operation of the ASR resource.
 * <p>
 * Automatic Speech Recognition is a rapidly evolving technology, with new
 * advances coming at a rapid pace. The JTAPI framework is general enough to
 * accommodate a wide range of technologies expected to enter into common
 * usage over the next several years. Most vendors' ASR resources will
 * provide a subset of the features available in the framework.
 *
 * <h2>Operation</h2>
 *
 * This section describes the operation of the ASR resource for recognition
 * and training.
 *
 * <h3>Recognition State Machine</h3>
 * The following figure is the ASR resource state diagram for recognition.
 * <P ALIGN="CENTER"><IMG SRC="doc-files/AsrStates.gif" ></P>
 * The defined states are <a href="#IdleState"><tt>v_Idle</tt></a>,
 * <a href="#RecognitionPaused"><tt>v_RecognitionPaused</tt></a>,
 * <a href="#Recognizing"><tt>v_Recognizing</tt></a>, and
 * <a href="#ResultsAvailable"><tt>v_ResultsAvailable</tt></a>.
 *
 * <a name="IdleState"><!-- --></a>
 * <h4>Idle State: <tt>v_Idle</tt></h4>
 * In the <tt>v_Idle</tt> state, the ASR resource is quiescent, performing no
 * recognition operations. It is in this state after a group configuration,
 * a handoff, or a {@link #idleASR idleASR(}) command.
 * <p>
 * This state transits to the <tt>v_Recognizing</tt> or
 * <tt>v_RecognitionPaused</tt> states when a client application requests a
 * recognition operation. Upon entering this state, all recognition results
 * stored in the resource are lost.
 * <a name="RecognitionPaused"><!-- --></a>
 *
 * <h4>Recognition Paused State: <tt>v_RecognitionPaused</tt></h4>
 * In the <tt>v_RecognitionPaused</tt> state, the ASR Resource has been
 * prepared with all parameter settings, but has not yet started to execute
 * the recognition algorithm on its input media stream. This state is entered
 * from the <tt>v_Idle</tt> state by issuing the command
 * {@link #recognize recognize()}
 * with the optional parameter <tt>p_StartPaused</tt> set to <tt>TRUE</tt>.
 * <p>
 * Recognition begins when the RTC action <tt>ASR.rtca_Start</tt> is received
 * or the command {@link #startASR startASR()} is executed
 * (the Recognizer transits to the <tt>v_Recognizing</tt> state).
 *
 * <a name="Recognizing"><!-- --></a>
 * <h4>Recognizing State: <tt>v_Recognizing</tt></h4>
 * In the <tt>v_Recognizing</tt> state, the resource executes its recognition
 * algorithm on the input media stream, using the current set of active
 * contexts and active words.
 * <p>
 * While in this state, the Recognizer may create intermediate results,
 * which can be reported to the application via the unsolicited event
 * <tt>ev_IntermediateResultsReady</tt>. The application can retrieve
 * intermediate results via the command
 * {@link #getResults getResults()}.
 * Intermediate recognition results are cumulative: since prior results of
 * recognition can change as more of the utterance is processed, the complete
 * result string since beginning of recognition is returned each time.
 * (Final results, as described in the state <tt>v_ResultsAvailable</tt>,
 * are not affected by the retrieval of intermediate results.)
 * <p>
 * This Recognizer transits to <tt>v_ResultsAvailable</tt> when the ASR
 * resource's current recognition task is completed, or if the command
 * {@link #stopASR stopASR()} is issued by the application, 
 * or if the RTC <tt>ASR.rtca_Stop</tt> is received.
 *
 * <a name="ResultsAvailable"><!-- --></a>
 * <h4>Results Available State: <tt>v_ResultsAvailable</tt></h4>
 * The <tt>v_ResultsAvailable</tt> state is reached when a recognition
 * operation is completed, because the application has issued the command
 * {@link #stopASR stopASR()}, or received the RTC <tt>ASR.rtca_Stop</tt>. 
 * In the <tt>v_ResultsAvailable</tt> state, final ASR results are available
 * and can be retrieved with the
 * {@link #getFinalResults getFinalResults()} command.
 * Retrieving the recognition results clears the results from the resource
 * and causes the Recognizer to transit to the <tt>v_Idle</tt> state.
 * 
 * <h4>Using the Recognition State Machine</h4>
 * Recognition is a process which transforms the incoming media stream,
 * assumed to contain utterances of a speaker, with text that can be
 * processed by a client.
 * <p>
 * This section discusses how to use a Recognizer which obeys the
 * recognition state machine to perform common ASR tasks.
 * <h5>Typical Recognition Operation</h5>
 * A typical recognition scenario is when a prompt is played, followed by
 * recognition. To accomplish this, the application uses delayed start of
 * recognition (see section
 * <a href="#xStarting">Immediate Start and Delayed Start of Recognition</a>),
 * and plays a prompt using a Player resource.
 * The end of the prompt triggers a run-time control to the Recognizer
 * (see section <a href="#xRTC">Runtime Control</a>),
 * which then starts recognition. After recognition is over, 
 * the application retrieves the recognition results, 
 * interprets them, and starts a new round of recognition.
 *
 * <a name="xStarting"><!-- --></a>
 * <h5> Immediate Start and Delayed Start of Recognition</h5>
 *
 * Recognition begins immediately if the Recognizer transits from
 * <tt>v_Idle</tt> to <tt>v_Recognizing</tt>. Delayed start occurs after a
 * transition from the <tt>v_Idle</tt> to the <tt>v_RecognitionPaused</tt>
 * state: recognition starts after transition to the <tt>v_Recognizing</tt>
 * state. Delayed start is used for synchronization --for example, to
 * synchronize the beginning of recognition with the end of a prompt.
 * <h5> Stopping Recognition: Resource Control and Application Control</h5>
 * Once recognition has started, there are two main programming styles
 * used to stop recognition.  Resource-controlled stop is obtained by
 * pre-defining, to the Recognizer, the stop conditions to be used by the
 * Recognizer. These conditions can be internal to the Recognizer and/or
 * based on RTC Conditions from other resources. For example, a typical
 * application will define a maximum recognition time window and some RTC
 * conditions from the Signal Detector resource. If the utterance exceeds
 * the maximum time window, or if the Signal Detector detects a DTMF
 * tone, recognition stops.
 * <p>
 * Application-controlled stop, the other main
 * programming style used to stop recognition, means that the application
 * program stops recognition directly; to make this style workable, the
 * application program must have access to intermediate results. For
 * example, the application program starts recognition without imposing
 * time limits on the recognition and then monitors intermediate
 * results. The application stops the recognizer after the
 * application receives a specific keyword. Another possible use of this
 * application programming style is when the Recognizer can send only
 * <i>raw</i> results to a <i>smart</i> application.
 * <p>
 * Note that these two programming styles may be mixed. 
 * For example, even if the stop is application-controlled, 
 * the application may set up a resource-controlled stop 
 * to occur if the telephone call is disconnected.
 * (although disconnect->stop is enabled by default for all S.410 resources)
 * <h5>Intermediate Results and Final Results</h5>
 * If reporting of Intermediate results is supported by a particular
 * resource, then intermediate results can be read -- after the
 * unsolicited event <tt>ev_IntermediateResultsReady</tt> -- by using the
 * <tt>getResults()</tt> function.  The function may be
 * used both in the <tt>v_Recognizing</tt> and in the
 * <tt>v_ResultsAvailable</tt> states without altering the state of the
 * Recognizer. Final results are read by using the
 * <tt>getFinalResults()</tt> function; this function may only be used in
 * the <tt>v_ResultsAvailable</tt> state and causes a transition to the
 * <tt>v_Idle</tt> state.
 * <p>
 * If the Recognizer supports recognition of sequences of words, the
 * Recognizer is also likely to support the retrieval of intermediate
 * results while recognition is active. (As noted above, the Recognizer
 * must support the retrieval of intermediate results if the application
 * uses the programming style of application-controlled stop.) Please
 * note that not all Recognizers support intermediate results, and
 * therefore some of the scenarios described below will not be available
 * with all Recognizers.
 * <p>
 * When a Recognizer stops, it changes state from the
 * <tt>v_Recognizing</tt> state to the <tt>v_ResultsAvailable</tt> state. The
 * Recognizer provides a summary result along with the event associated
 * with the end of recognition; more detailed information about the
 * results may be obtained by subsequently using <tt>getFinalResults()</tt>. 
 * <tt>getFinalResults()</tt> causes the Recognizer to transit from 
 * the <tt>v_ResultsAvailable</tt> state to the <tt>v_Idle</tt> state. 
 * The difference between summary results obtained
 * automatically at the end of recognition and detailed results obtained
 * via the <tt>getFinalResults()</tt> function are detailed below in
 * section <a href="#57">Recognition Results</a>.
 * <p>
 * It is possible to transit from the <tt>v_ResultsAvailable</tt> state to
 * the <tt>v_Recognizing</tt> state by using the <tt>recognize()</tt> function
 * (e.g., to speed up the recognition of a sequence of isolated
 * words). In this case, at the end of the recognition of each utterance,
 * the summary result is in the corresponding event, while detailed
 * results can be read with <tt>getResults()</tt>. After the
 * final utterance of the sequence is recognized, <tt>getFinalResults()</tt>
 *  may be used, detailed results read, 
 * and the Recognizer will transit from the
 * <tt>v_ResultsAvailable</tt> state to the <tt>v_Idle</tt> state.
 * <p>
 * A transition to <tt>v_Idle</tt> state resets the Recognizer and deletes
 * all recognition results.  The function <tt>idleASR()</tt> (and the
 * corresponding RTC Action <tt>rtca_Idle</tt>) may be used to force the end
 * of a recognition, but will also result in a loss of any recognition
 * results. The function <tt>stopASR()</tt> (and the corresponding RTC Action
 * <tt>rtca_Stop</tt>) forces the termination of recognition of an utterance,
 * but the Recognizer enters the state <tt>v_ResultsAvailable</tt>, and the
 * recognition results are still accessible via <tt>getFinalResults()</tt>.
 * <p>
 * Parameters may only be set while the Recognizer is in the <tt>v_Idle</tt> or
 * <tt>v_RecognitionPaused</tt> state, Functions that read recognition
 * parameters are legal during any Recognizer state.
 *
 * <h2>Training State Machine</h2>
 *
 * The following figure is the ASR resource state diagram when
 * performing training operations. The defined states are <tt>v_Idle</tt>,
 * <tt>v_TrainingPaused</tt>, <tt>v_Training</tt>, and <tt>v_WordTrained</tt>.
 * <P ALIGN="CENTER"><IMG SRC="doc-files/AsrTrainStates.gif" ></P>
 *
 * <h3>Idle State, <tt>v_Idle</tt></h3>
 * In the <tt>v_Idle</tt> state, the ASR resource is quiescent, 
 * performing no training operations. It is in this state after 
 * a group configuration, a handoff, a idleASR() command, 
 * or a wordCommit() command.
 * <p>
 * The ASR resource transits to the <tt>v_Training</tt> 
 * or <tt>v_StartPaused</tt> states
 * when the application requests a training operation via wordTrain().
 * <p>
 * When the ASR resources enters <tt>v_Idle</tt>, all training information
 * that has not been committed to the Context is lost.
 *
 * <h3>Training Paused State, <tt>v_TrainingPaused</tt></h3>
 * In the <tt>v_TrainingPaused</tt> state, the ASR resource has been prepared
 * with all parameter settings, but has not yet started to execute the
 * training algorithm on its input media stream.
 * <p>
 * This state is entered from the <tt>v_Idle</tt> state by issuing the command
 * wordTrain() with the optional argument <tt>v_StartPaused</tt> set to true.
 * Training begins when the RTC action <tt>rtca_Start</tt> or the command 
 * startASR() is received. 
 * (The resource transits to the <tt>v_Training</tt> state.)
 *
 * <h3>Training, <tt>v_Training</tt></h3>
 *
 * In the <tt>v_Training</tt> state, the resource executes its training algorithm
 * for a particular word and a particular context on the input media stream.
 * The resource transits to the <tt>v_WordTrained</tt> state when the 
 * training operation is completed. 
 * If the command stopASR() or RTC <tt>rtca_Stop</tt> is received,
 * the resource transits to the <tt>v_WordTrained</tt> state.
 *
 * <h3>Word Trained State, <tt>v_WordTrained</tt></h3>
 *
 * In the <tt>v_WordTrained</tt> state, a training result has been reached and a
 * temporary word model of the trained word has been updated. The results of
 * the training operation can be discarded via the command
 * {@link #wordDeleteLastUtterance wordDeleteLastUtterance()},
 * further updated via the command
 * {@link #wordTrain wordTrain()}, or committed to the context's model
 * for the word via the command <tt>wordCommit()</tt>. 
 * The command <tt>wordDeleteLastUtterance()</tt>
 * causes the state to transit to <tt>v_WordTrained</tt> again. 
 * <tt>wordTrain()</tt> causes the resource to transit
 * to the <tt>v_Training</tt> state for further training. 
 * <tt>wordCommit()</tt> commits the training to the word, 
 * deletes the temporary word model, and the ASR resource
 * transits to <tt>v_Idle</tt>.
 *
 * <h3>Using the Training State Machine</h3>
 *
 * Training is the process by which the ASR resource collects audio information
 * (or other information), and associates this information with Words in an
 * active Context on the ASR resource, so that the Word may be used by a
 * Recognizer to perform recognition. When training is
 * over, the ASR resource issues a single completion event. The parameter
 * <tt>p_NumberRepetitions</tt>, available in some Recognizers, may be set to
 * dictate how many training utterances the Recognizer should use in
 * AutomaticTraining mode.
 * <p>
 * Applications will often use {@link #wordTrain wordTrain()} and
 * {@link #wordCommit wordCommit()}
 * in a pair of loops. The outer loop will prompt the user to determine if any
 * training is required. The inner loop will prompt the user for suitable
 * utterances and use wordTrain() to perform training. The outer loop will
 * then use wordCommit() to modify the Context permanently.
 *
 * <h4>Immediate Start and Delayed Start of Training</h4>
 * Training begins immediately when the Recognizer transits 
 * from <tt>v_Idle</tt> to <tt>v_Training</tt>. 
 * Delayed start occurs with a transition from the <tt>v_Idle</tt> to the
 * <tt>v_TrainingPaused</tt> state: actual training starts after transition
 * to the <tt>v_Training</tt> state. Delayed start is used for synchronization
 * --for example, 
 * to synchronize the beginning of training with the end of a prompt.
 *
 * <h4>Internal States of the Word Models</h4>
 * Training states of a word model are not defined by this specification.
 * Instead, the ASR resource returns as part of its training results a Symbol
 * whose value indicates the readiness, with the possible 
 * values of <tt>v_Ready</tt>, <tt>v_NotReady</tt>, or <tt>v_Complete</tt>.
 * <p>
 * The criteria by which the ASR resource determines the readiness is
 * controlled by a set of parameters defined in Table 5-12. The determination
 * can be made autonomously by the ASR resource -- <i>automatically</i>, on
 * the basis of decision-making on the part of the ASR resource's training
 * algorithm, or can be set to be a fixed number of training iterations on
 * a single word.
 *
 * <h4>Automatic Training</h4>
 * Some Recognizers may perform training automatically, issuing prompts and
 * collecting utterances until a sufficient number of good utterances have
 * been collected. Such Recognizers can run in automatic mode when the
 * parameter <tt>p_AutomaticTraining</tt> is set to true. 
 * These ASR resources remain
 * in the state <tt>v_Training</tt> until training is complete.
 *
 * <a name="xContexts"><!-- --></a>
 * <h2>Contexts</h2>
 *
 * A Context is an opaque object that an ASR resource's recognition
 * algorithm uses to recognize utterances. A Context may contain a set of
 * algorithm-dependent models representing words, or a grammar used to
 * constrain
 * the algorithm's search space, or other information needed by the ASR
 * resource. Contexts have a large set of associated parameters used by the
 * application to control the operation of the ASR resource. When stored as a
 * data object in a container, some of these parameters are accessible as
 * ContainerInfo information elements; when loaded into an ASR
 * resource, they are accessible as resource parameters.
 * <p>
 * The Java Speech Grammar Format (JSGF) is supported by this specification,
 * along with commands and parameters to control the grammars. In addition,
 * this specification supports commands to facilitate speaker-dependent
 * systems: to create word models (i.e., "train" the words), remove
 * word models from contexts, and load/store contexts.
 * <p>
 * A Context is stored as a container data object with media type "file".
 * When stored as a data object, some Context parameters may be read as
 * ContainerInfo information elements of the data object.
 * <p>
 * A recommended convention is to use a context for domain-specific
 * (e.g., banking, directions) or speaker-specific information, and to store
 * multiple contexts representing information in the same language or for the
 * same speaker in a common container. This permits the use of the term
 * "context name" to refer to the language/domain pair and to refer
 * to the
 * container and data object in which the context is stored. For example,
 * a context for the banking domain in Spanish would have the data object
 * name "Spanish/Bank" (Container/Object); the context's context
 * name would be Spanish/Bank.
 *
 * <h3>Context Configuration</h3>
 *
 * An ASR resource performs its recognition and training operations only with
 * respect to contexts that are loaded into the resource; a Context that is
 * loaded onto the resource is referred to as a loaded context. When configured
 * into a group, an ASR resource has a default context established
 * by the administration function. A client may load an additional context via
 * the command {@link #contextCopy contextCopy()}, and remove a context
 * via the command {@link #contextRemove contextRemove()}.
 * <p>
 * An ASR resource performs its recognition operations with respect to active
 * contexts and active words. The ASR parameter <tt>p_ActiveContexts</tt> 
 * is a list of active contexts. 
 * The ASR parameter <tt>p_ActiveWords</tt> is a list of active words.
 * Only word models from contexts in the ActiveContexts list whose names are
 * in the ActiveWords list are used by the recognition algorithm.
 * <p>
 * These parameters may be set non-persistently as an argument to the command
 * recognize(), or persistently via the command setParameters(),
 * in those Recognizers that have settable context lists and/or word lists.
 *
 * <h2>Grammars</h2>
 *
 * The ECTF supports the Java Speech Grammar Format (JSGF), a grammar
 * specification language described by the Java consortium 
 * (http://java.sun.com/products/java-media/speech). The JSGF
 * should be consulted for a complete description of the grammar
 * specification language.
 * <p>
 * Grammars are contained within Contexts. A Context can contain at most one
 * grammar; Contexts which contain grammars are referred to as Grammar
 * Contexts.
 * <p>
 * Grammars contain public rules and private rules. Public rules may
 * be imported by other grammars and can be made active or inactive.
 * <p>
 * When a Grammar Context is loaded using the contextCopy() function,
 * the grammar is inactive, and all its rules are inactive. 
 * To activate a Grammar Context, 
 * set the parameter <tt>p_ActiveContexts</tt> to include the Grammar Context.
 * When the Grammar Context becomes active, all the rules described in the
 * Grammar Context attribute <tt>a_DefaultRules</tt> become active. 
 * The active rules
 * can be modified by setting the parameter <tt>p_ActiveRules</tt>.
 *
 * <h3>Grammar Context Attributes and Parameters</h3>
 * This section lists attributes and parameters specific to Grammar Contexts.
 * The following attributes of Grammar Contexts may be queried by the
 * application using standard commands.
 * <p>
 * <table border="1" cellpadding="3">
 * <tr><td>Attribute</td><td>Description</td></tr>
 * <tr><td><tt>{@link ASRConstants#a_DefaultRules a_DefaultRules}</tt></td>
 *     <td>public rule names that become active when the
 *        Grammar Context becomes active</td></tr>
 * <tr><td><tt>{@link ASRConstants#a_Rules a_Rules}</tt></td><td>rules (public and private) in the grammar</td></tr>
 * <tr><td><tt>{@link ASRConstants#a_PrivateRules a_PrivateRules}</tt></td><td>private rules in the grammar</td></tr>
 * <tr><td><tt>{@link ASRConstants#a_PublicRules a_PublicRules}</tt></td><td>public rules in the grammar</td></tr>
 * </table>
 * <p>
 * The parameters of a Grammar Context may be examined and set (if appropriate)
 * using the contextGetParameters() and contextSetParameters() functions.
 * The parameter <tt>p_ActiveRules</tt> contains the current complete 
 * list of active public rules and may be used to control which public rules
 * are active. When the Grammar Context first becomes active, 
 * the contents <tt>p_ActiveRules</tt>
 * will be identical to that of <tt>a_DefaultRules</tt>.
 * <p>
 * <table border="1" cellpadding="3">
 * <tr><td>Parameter</td><td>Description</td></tr>
 * <tr><td>{@link ASRConstants#p_Rules p_Rules}</td>
 *     <td>grammar rules within a loaded grammar context</td></tr>
 * <tr><td>{@link ASRConstants#p_PrivateRules p_PrivateRules}</td>
 *     <td>private grammar rules within a loaded grammar context</td></tr>
 * <tr><td>{@link ASRConstants#p_PublicRules p_PublicRules}</td>
 *     <td>public grammar rules within a loaded grammar context</td></tr>
 * <tr><td>{@link ASRConstants#p_ActiveRules p_ActiveRules}</td>
 *     <td>active grammar rules within a loaded grammar context</td></tr>
 * </table>
 *
 * <h3>Grammar Context Functions</h3>
 * A public or private rule has a name and a rule expansion. The rule expansion
 * may contain a series of words to be recognized as part of the rule, the names
 * of other rules, and other information as specified in the JSGF.
 * <p>
 * Applications may modify rule expansions. For example, if a rule expansion
 * contains a list of street names, the application may change the list of
 * street names during the course of the application to reflect the choice of
 * city name made by the user.
 * <p>
 * The application may retrieve a rule expansion by using the function
 * <tt>getRuleExpansion()</tt>, and set a rule expansion by using
 * the function <tt>setRuleExpansion()</tt>.
 *
 * <h3>Grammars and Barge-In</h3>
 * Grammars may be used to support barge-in. See the discussion of barge-in
 * in section <a href="#xBargeIn">Barge-In</a>.
 *
 * <h3>Grammars and Results</h3>
 * The JSGF grammar specification language supports grammar tags to attach
 * application-specific information to parts of rule definitions. The grammar
 * tags do not affect recognition; instead, they are a mechanism to provide
 * specific information to the application about which part of the
 * grammar was traversed.
 * <p>
 * The grammar tags are reported as part of the results of recognition.
 * See section <a href="#57">Recognition Results</a> for a description 
 * of how grammar tags are reported in results.
 *
 * <h2>Language and Variant Labels</h2>
 * For the purposes of ASR and TTS, languages are identified by a language
 * identifier (e.g., English, German) and a variant identifier
 * (e.g., US English).
 * These identifiers are used as ContainerInfo information elements on TVMs,
 * grammars, and contexts, to identify the represented, encoded, or recognized
 * language(s) and language variant(s). They are used as attributes for
 * ASR and
 * TTS-Player resources to identify the languages that can be recognized or
 * decoded. In language identification applications within ASR, they are also
 * used as parameters to indicate the language that a recognizer attributes
 * to a recognized utterance.
 * <p>
 * Language and variant identifiers are denoted by symbols. The item field of
 * the symbol is a token adapted from the descriptions of ISO 639-2, with
 * punctuation and whitespace removed and individual words in the description
 * capitalized, conformant with the symbol nomenclature followed by s.410.
 * For example, "Italian" in ISO 639-2 becomes the Symbol <tt>v_Italian</tt>.
 * <p>
 * ECTF S.100-R2 Volume 4 contains the currently-defined language
 * and variant identifiers.
 * Not all languages defined in ISO639-2 are represented in this table,
 * since Recognizers do not exist in many of these languages, and indeed may
 * never exist in such languages. An ASR vendor who wishes to support a
 * language or variant not in this table may use a vendor-defined symbol
 * for the language or variant identifier, and may contribute the identifiers
 * so defined to the ECTF for incorporation within the ECTF symbol name space
 * (the identifiers should follow the conventions of ISO639-2, as modified).
 *
 * <a name="xRTC"><!-- --></a>
 * <h2>Runtime Control</h2>
 *
 * <h3>Actions</h3>
 *
 * The following table lists the actions supported by the Recognizer, and the
 * events that the Recognizer generates after performing the actions.
 * <p>
 * <table border="1" cellpadding="3">
 * <tr><td>Action</td><td>Definition</td><td>Commands Affected</td>
 *     <td>Next State</td><td>Event Generated</td></tr>
 * <tr><td>{@link ASRConstants#rtca_Stop <tt>rtca_Stop</tt>}</td>
 *     <td>stop recognizing</td><td>recognize(), wordTrain()</td>
 *     <td><tt>v_ResultsAvailable</tt>, <tt>v_WordTrained</tt></td>
 *     <td><tt>ev_Recognize</tt> or <tt>ev_WordTrained</tt></td></tr>
 * <tr><td>{@link ASRConstants#rtca_Idle <tt>rtca_Idle</tt>}</td>
 *     <td>stop recognizing and return to <tt>v_idle</tt> state</td><td>all</td>
 *     <td><tt>v_Idle</tt></td><td><tt>ev_Idle</tt></td></tr>
 * <tr><td>{@link ASRConstants#rtca_UpdateParameters <tt>rtca_UpdateParameters</tt>}
 *         </td>
 *     <td>update parameters given in <tt>p_UpdateParametersList</tt></td>
 *     <td>recognize(), wordTrain()</td>
 *     <td><tt>v_Recognizing</tt>, <tt>v_Training</tt></td>
 *     <td>&nbsp</td></tr>
 * <tr><td>{@link ASRConstants#rtca_Start <tt>rtca_Start</tt>}</td>
 *     <td>start recognizing</td><td>recognize(), wordTrain()</td>
 *     <td><tt>v_Recognizing</tt>, <tt>v_Training</tt></td>
 *     <td><tt>ev_Start</tt></td></tr>
 * </table>
 *
 * <h3>Conditions</h3>
 * This table lists the internal conditions identified by an ASR and usable for
 * triggering run time controls of another resource, e.g., for starting or
 * stopping a Player resource.
 * <p>
 * <table border="1" cellpadding="3">
 * <tr><td>Condition</td><td>Definition</td></tr>
 * <tr><td><tt>rtcc_RecognizeStarted</tt></td>
 *     <td>Recognizer has started operating</td></tr>
 * <tr><td><tt>rtcc_TrainStarted</tt></td><td>Recognizer has started training.</td></tr>
 * <tr><td><tt>rtcc_SpeechDetected</tt></td><td>Speech has been detected</td></tr>
 * <tr><td><tt>rtcc_SpecificUtterance</tt></td>
 *     <td>A specifically defined utterance was detected. The parameter
 *         <tt>p_SpecificUtterance</tt> (see Table 5-12) is used to set the
 *         utterances that trigger this condition.</td></tr>
 * <tr><td><tt>rtcc_Recognize</tt></td><td>Recognition has terminated normally
 *         (this does not signify that a valid utterance was found).</td></tr>
 * <tr><td><tt>rtcc_WordTrain</tt></td><td>Word training has terminated normally
 *         (this does not signify that training is complete)</td></tr>
 * <tr><td><tt>rtcc_WordTrained</tt></td><td>Word training is complete</td></tr>
 * <tr><td><tt>rtcc_ValidUtteranceFound</tt></td><td>A valid utterance has been
 *         detected, even though recognition may not be over</td></tr>
 * <tr><td><tt>rtcc_ValidUtteranceFinal</tt></td><td>A valid utterance has been
 *         detected and recognition has completed</td></tr>
 * <tr><td><tt>rtcc_Silence</tt></td><td>Silence has been detected</td></tr>
 * <tr><td><tt>rtcc_InvalidUtterance</tt></td><td>An invalid utterance has been
 *         detected</td></tr>
 * <tr><td><tt>rtcc_GrammarBargeIn</tt></td><td>The grammar has detected barge-in
 *         (it has found the label <tt>v_BargeInHere</tt>).
 *         See section <a href="#5621">Grammar-Based Barge-In</a></td></tr>
 * </table>
 *
 *
 * <h2>Parameters</h2>
 *
 * <h3>Introduction</h3>
 * Parameters control various aspects of the Recognizer. This section
 * describes the officially-supported parameters.
 * <p>
 * The descriptions are phrased to describe the true case. For example,
 * if the result for <tt>p_SpeechRecognition</tt> is false, 
 * then speech recognition is not supported.
 *
 * <h3>ASR Resource Parameter Categories</h3>
 * The Context control parameters of Table 5-5 indicate the loaded
 * words and contexts; and indicate and control the active words and
 * contexts that may be used in a recognition or training
 * operation (see section <a href="#xContext">Contexts</a>).
 * <p>
 * <table border="1" cellpadding="3">
 * <tr><td>Name</td><td>Data Type</td><td>State when settable</td>
       <td>Default Value</td><td>Possible Values</td><td>Definition</td></tr>
 * <tr><td><tt>p_ActiveWords</tt></td><td>String[]</td><td><tt>v_Idle</tt></td>
 *     <td>All Loaded Words</td><td>&nbsp;</td>
 *     <td>List of Active Words</td></tr>
 * <tr><td><tt>p_LoadedWords</tt></td><td>String[]</td><td>Read-Only</td><td>&nbsp;</td>
 *     <td>&nbsp;</td><td>List of Loaded Words</td></tr>
 * <tr><td><tt>p_ActiveContexts</tt></td><td>String[]</td><td><tt>v_Idle</tt></td>
 *     <td>All Loaded Contexts</td><td>&nbsp;</td>
 *     <td>List of active Contexts</td></tr>
 * <tr><td><tt>p_LoadedContexts</tt></td><td>String[]</td><td>Read-Only</td>
 *     <td>&nbsp;</td><td>&nbsp;</td><td>List of loaded Contexts</td></tr>
 * <tr><td></td><td></td><td></td><td></td><td></td><td></td></tr>
 * </table>
 *
 * The Context Information Parameters of the Table 5-6 specify information
 * about a Context. If the Context is loaded on a Resource, this
 * information may be accessed via the contextGetParameters() and
 * contextSetParameters() functions. Several Contexts may be loaded on
 * the resource, each with its own set of parameters.
 * <p>
 * Note that any attempt to set a parameter of the Context will be
 * checked against the Resource's capabilities. If an attempt to made to
 * set a parameter to indicate support for a capability that the
 * Resource does not actually support, the attempt will fail.
 * <p>
 * If the Context is not loaded on a resource, but is in a container,
 * then these context parameters are accessible via the Context
 * Parameter's ContainerInfo information element.
 * <p>
 * <TABLE BORDER="1" CELLPADDING=3>
 * <TR><td><B><P>Context Parameter</B></TD>
 *    <td><B>Data Type</B></TD>
 *    <td><B>State when Settable</B></TD>
 *    <td><B>Possible Values</B></TD>
 *    <td><B>Description</B></TD></TR>
 * <TR><td><tt>{@link ASRConstants#p_Trainable p_Trainable}</tt></TD>
 *     <td>CTbool</TD>
 *     <td>WordTrained</TD>
 *     <td>true, false</TD>
 *     <td>Whether this context can be trained</TD></TR>
 * <TR><td><tt>{@link ASRConstants#p_SpeakerType p_SpeakerType}</tt></TD>
 *     <td>Symbol</TD>
 *     <td>WordTrained</TD>
 *     <td><tt>v_Independent</tt>, <tt>v_Dependent</tt>, 
 *         <tt>v_Verification</tt>, <tt>v_Identification</tt></TD>
 *     <td>The type of recognition possible</TD></TR>
 * <TR><td><tt>{@link ASRConstants#p_Label p_Label}</tt></TD>
 *     <td>String</TD>
 *     <td>WordTrained</TD>
 *     <td>&nbsp</td>
 *     <td>An abstract string identifying a set of words and/or phrases.
 *         This may be preloaded</TD></TR>
 * <TR><td><tt>{@link ASRConstants#p_Language p_Language}</tt></TD>
 *     <td>Symbol[]</TD>
 *     <td>Read only</TD>
 *     <td>See Language list of Volume 4</TD>
 *     <td>The language(s)recognized</TD></TR>
 * <TR><td><tt>{@link ASRConstants#p_Variant p_Variant}</tt></TD>
 *     <td>Symbol[]</TD>
 *     <td>Read only</TD>
 *     <td>See Variant list of Volume 4</TD>
 *     <td>The variant(s) recognized</TD></TR>
 * <TR><td><tt>{@link ASRConstants#p_DetectionType p_DetectionType}</tt></TD>
 *     <td>Symbol</TD>
 *     <td>WordTrained</TD>
 *     <td>Continuous, Discrete</TD>
 *     <td>&nbsp</td></TR>
 * <TR><td><tt>{@link ASRConstants#p_Spotting p_Spotting}</tt></TD>
 *     <td>Symbol</TD>
 *     <td>WordTrained</TD>
 *     <td>Available, NotAvailable</TD>
 *     <td>&nbsp</td></TR>
 * <TR><td><tt>{@link ASRConstants#p_UtteranceType p_UtteranceType}</tt></TD>
 *     <td>Symbol</TD>
 *     <td>WordTrained</TD>
 *     <td>Phonetic, Speech, Text</TD>
 *     <td>The manner in which the utterance data is described to the
 *         recognizer for training</TD></TR>
 * <TR><td><tt>{@link ASRConstants#p_Size p_Size}</tt></TD>
 *     <td>&nbsp</td>
 *     <td>Read only</TD>
 *     <td>&nbsp</td>
 *     <td>A vendor-defined heuristic describing the space requirements
 *         of the context when resident on the resource. Size is implicitly
 *         adjusted when training is performed on the resource</TD></TR>
 * <TR><td><tt>{@link ASRConstants#p_ContextType p_ContextType}</tt></TD>
 *     <td>Symbol</TD>
 *     <td>WordTrained</TD>
 *     <td><tt>v_JSGF</tt></TD>
 *     <td>The actual type of the context to be loaded.  The only
 *         ECTF-defined symbol is one for Context Grammars that support
 *         the Java Speech Grammar Format. Otherwise, this is a
 *         vendor-defined symbol that is used by the vendor to
 *         distinguish between specific data formats used internally
 *         by the vendor's Resource.</TD></TR>
 * </TABLE>
 * <p>
 * The Speech input control parameters of the Table 5-7 specify
 * parameters for determining speech boundaries (e.g., timeout
 * thresholds for end of speech). They also enable or disable echo
 * cancellation and audio prompt generation and cancellation.
 * <p>
 * <TABLE BORDER=1 CELLPADDING=3>
 * <TR><td><B>Name</B></TD>
 *     <td><B>Data Type</B></TD>
 *     <td><B>State when settable</B></TD>
 *     <td><B>Default Value</B></TD>
 *     <td><B>Possible Values</B></TD>
 *     <td><B>Definition</B></TD></TR>
 * <TR><td><tt>{@link ASRConstants#p_Duration p_Duration}</tt></TD>
 *     <td>Int</TD>
 *     <td><tt>v_Idle</tt></TD>
 *     <td><tt>v_Forever</tt></TD>
 *     <td>0 to ? and <tt>v_Forever</tt></TD>
 *     <td>maximum time window (in ms).  At the end of that time
 *         the recognition terminates</TD></TR>
 * <TR><td><tt>{@link ASRConstants#p_InitialTimeout p_InitialTimeout}</tt></TD>
 *     <td>Int</TD>
 *     <td><tt>v_Idle</tt></TD>
 *     <td><tt>v_Forever</tt></TD>
 *     <td>0 to 1000000 or <tt>v_Forever</tt></TD>
 *     <td>Initial silence timeout in ms.  If no utterance is detected
 *         in this period, recognition is terminated and the Recognizer will
 *         notify the application that silence has been detected.</TD></TR>
 * <TR><td><tt>{@link ASRConstants#p_FinalTimeout p_FinalTimeout}</tt></TD>
 *     <td>Int</TD>
 *     <td><tt>v_Idle</tt></TD>
 *     <td><tt>v_Forever</tt></TD>
 *     <td>0 to 1000000 or <tt>v_Forever</tt></TD>
 *     <td>Silence time in ms after utterance to indicate completion of
 *         the recognition</TD></TR>
 * <TR><td><tt>{@link ASRConstants#p_EchoCancellation p_EchoCancellation}</tt></TD>
 *     <td>Boolean</TD>
 *     <td><tt>v_Idle</tt></TD>
 *     <td>false</TD>
 *     <td>true, false</TD>
 *     <td>Echo cancellation active</TD></TR>
 * <TR><td><tt>{@link ASRConstants#p_GrammarBargeInThreshold p_GrammarBargeInThreshold}</tt></TD>
 *     <td>Int</TD>
 *     <td><tt>v_Idle</tt></TD>
 *     <td>500</TD>
 *     <td>0-1000</TD>
 *     <td>Minimum score at a barge-in node required to raise the
 *     <tt>rtcc_GrammarBargeIn</tt> RTC condition. The score is the same "Score"
 *     described in section <a href="#5734">ASR_ECTF_TokenScores</a>.</TD></TR>
 * </TABLE>
 * <P>
 * The state of the Recognizer can be determined by reading the following
 * parameter.
 * <p>
 * <TABLE BORDER=1 CELLPADDING=3>
 * <TR><td><B>Name</B></TD>
 *     <td><B>Data Type</B></TD>
 *     <td><B>State when settable</B></TD>
 *     <td><B>Default Value</B></TD>
 *     <td><B>Possible Values</B></TD>
 *     <td><B>Definition</B></TD></TR>
 * <TR><td><tt>{@link ASRConstants#p_State p_State}</tt></TD>
 *     <td>Symbol</TD>
 *     <td>Read Only</TD>
 *     <td><tt>v_Idle</tt></TD>
 *     <td><tt>v_Idle</tt>, <tt>v_RecognitionPaused</tt>, <tt>v_Recognizing</tt>, <tt>v_ResultsAvailalable</tt>,
 *    <tt>v_TrainingPaused</tt>, <tt>v_Training</tt>, <tt>v_WordTrained</tt></TD>
 *     <td>State of the Recognizer</TD></TR>
 * </TABLE>
 * <p>
 * The Output control parameters of the Table 5-9 specify the format in
 * which recognition results are provided. The number of alternate
 * recognition hypotheses, the presentation in text or in phonetic spelling,
 * and the inclusion of warning tokens are selected by these parameters.
 * <p>
 * <TABLE BORDER=1 CELLPADDING=3>
 * <TR><td>Name</TD>
 *     <td>Data Type</TD>
 *     <td>State when settable</TD>
 *     <td>Default Value</TD>
 *     <td>Possible Values</TD>
 *     <td>Definition</TD></TR>
 * <TR><td><tt>{@link ASRConstants#p_NumberWordsRequested p_NumberWordsRequested}</tt></TD>
 *     <td>Int</TD>
 *     <td><tt>v_Idle</tt></TD>
 *     <td>1</TD>
 *     <td>&nbsp;</TD>
 *     <td>Number of words to return (e.g., in a connected digit ASR,
 *         the number of digits to return)</TD></TR>
 * <TR><td><tt>{@link ASRConstants#p_OutputType p_OutputType}</tt></TD>
 *     <td>Symbol</TD>
 *     <td><tt>v_Idle</tt></TD>
 *     <td><tt>v_Text</tt></TD>
 *     <td><tt>v_Text</tt>, <tt>v_Phonetic</tt></TD>
 *     <td>The type of output result provided</TD></TR>
 * <TR><td><tt>{@link ASRConstants#p_TopChoices p_TopChoices}</tt></TD>
 *     <td>Int</TD>
 *     <td><tt>v_Idle</tt></TD>
 *     <td>1</TD>
 *     <td>Any positive number</TD>
 *     <td>Number of alternative results returned</TD></TR>
 * <TR><td><tt>{@link ASRConstants#p_ResultType p_ResultType}</tt></TD>
 *     <td>Symbol</TD>
 *     <td><tt>v_Idle</tt></TD>
 *     <td><tt>v_Final</tt></TD>
 *     <td><tt>v_Final</tt>, <tt>v_Intermediate</tt></TD>
 *     <td>Whether results returned by getResults() are final
 *         summaries or intermediate results</TD></TR>
 * <TR><td><tt>{@link ASRConstants#p_ResultPresentation p_ResultPresentation}</tt></TD>
 *     <td>Symbol</TD>
 *     <td><tt>v_Idle</tt></TD>
 *     <td><tt>v_TypeII</tt></TD>
 *     <td><tt>v_TypeI</tt>, <tt>v_TypeII</tt></TD>
 *     <td>Method of presentation of ASR results</TD></TR>
 * </TABLE>
 * <p>
 * The Training control parameters of Table 5-10 are used to control the
 * means by which the end of training for a word is reported. A client
 * may select automatic training (in which the resource decides when
 * enough training has been done), may set the number of repetitions,
 * and may enable or disable unsolicited events related to training.
 * <p>
 * <TABLE BORDER=1 CELLPADDING=3>
 * <TR><td><B>Name</B></TD>
 *     <td><B>Data Type</B></TD>
 *     <td><B>State when settable</B></TD>
 *     <td><B>Default Value</B></TD>
 *     <td><B>Possible Values</B></TD>
 *     <td><B>Definition</B></TD></TR>
 * <TR><td><tt>{@link ASRConstants#p_AutomaticTraining p_AutomaticTraining}</tt></TD>
 *     <td>Boolean</TD>
 *     <td><tt>v_Idle</tt></TD>
 *     <td>false</TD>
 *     <td>true, false</TD>
 *     <td>Automatic training active</TD></TR>
 * <TR><td><tt>{@link ASRConstants#p_NumberRepetitions p_NumberRepetitions}</tt></TD>
 *     <td>Int</TD>
 *     <td><tt>v_Idle</tt></TD>
 *     <td>1</TD>
 *     <td>Vendor specific</TD>
 *     <td>Number of repetitions  to perform in a training loop when
 *         automatic training is enabled</TD></TR>
 * <TR><td><tt>{@link ASRConstants#p_EnabledEvents p_EnabledEvents}</tt></TD>
 *     <td>Dictionary</TD>
 *     <td><tt>v_Idle</tt></TD>
 *     <td>Empty</TD>
 *     <td>Any unsolicited event</TD>
 *     <td>The elements in the KVSet form the list of  which unsolicited
 *         events are enabled; only events on the list are reported by
 *         the Resource. (Only the Keys in this KVset are used;
 *         the Values are ignored)</TD></TR>
 * <TR><td><tt>{@link ASRConstants#p_TrainingType p_TrainingType}</tt></TD>
 *     <td>Symbol</TD>
 *     <td><tt>v_Idle</tt></TD>
 *     <td>vendor specific</TD>
 *     <td><tt>v_Speech</tt>, <tt>v_Phonetic</tt>, <tt>v_Text</tt></TD>
 *     <td>The type of input used to perform training</TD></TR>
 * </TABLE>
 * <p>
 * The Speech buffer control parameters of Table 5-11 enable a mode in
 * which a previously-detected utterance is used as input.
 * <p>
 * <TABLE BORDER=1 CELLPADDING=3>
 * <TR><td><B>Name</B></TD>
 *     <td><B>Data Type</B></TD>
 *     <td><B>State when settable</B></TD>
 *     <td><B>Default Value</B></TD>
 *     <td><B>Possible Values</B></TD>
 *     <td><B>Definition</B></TD></TR>
 * <TR><td><tt>p_StoreInput</tt></TD>
 *     <td>Boolean</TD>
 *     <td><tt>v_Idle</tt></TD>
 *     <td>false</TD>
 *     <td>true, false</TD>
 *     <td>Acquired utterance is saved in a resource internal buffer
 *         for reuse</TD></TR>
 * <TR><td><tt>p_PlayInput</tt></TD>
 *     <td>Boolean</TD>
 *     <td><tt>v_Idle</tt></TD>
 *     <td>false</TD>
 *     <td>true, false</TD>
 *     <td>ASR internal buffer is reused as a new input</TD></TR>
 * </TABLE>
 * <p>
 * The RTC control parameters in Table 5-12 refine the behavior of the
 * RTC conditions defined forthe ASR resource.
 * <p>
 * <TABLE BORDER=1 CELLPADDING=3>
 * <tr><td><B>Name</B></TD>
 *     <td><B>Data Type</B></TD>
 *     <td><B>State when settable</B></TD>
 *     <td><B>Default Value</B></TD>
 *     <td><B>Possible Values</B></TD>
 *     <td><B>Definition</B></TD>
 * </TR>
 * <tr><td><tt>p_StartPaused</tt></TD>
 *     <td>Boolean</TD>
 *     <td><tt>v_Idle</tt></TD>
 *     <td>false</TD>
 *     <td>true, false</TD>
 *     <td>The Recognizer starts in Paused mode, transiting to the
 *         RecognitionPaused or TrainingPaused state. The Recognizer
 *         remains in the Paused state until it receives a Start RTC
 *         (or function). This parameter is valid for the recognize()
 * 	   and wordTrain() functions only.</TD></TR>
 * <tr><td><tt>p_SpecificUtterance</tt></TD>
 *     <td>String[]</TD>
 *     <td><tt>v_Idle</tt></TD>
 *     <td>&nbsp</TD>
 *     <td>&nbsp</TD>
 *     <td>Each value in the array represents an utterance.  When any
 *         of these utterances are recognized the SpecificUtterance
 * 	   condition will be triggered.</TD></TR>
 * </TABLE>
 *
 * <h2> Additional Features</h2>
 *
 * <h3> Echo Cancellation</h3>
 *
 * When a prompt is played, an echo is often returned on the input media
 * stream, an echo that can be detected by a Recognizer; the echo can
 * cause errors in recognition. Echo cancellation removes the echo from
 * the input media stream. Echo cancellation provided by a Recognizer is
 * controlled by the parameter <tt>p_EchoCancellation</tt> which when set
 * to true causes echo cancellation to run continuously on the
 * input media stream. Echo cancellation is stopped by setting
 * <tt>p_EchoCancellation</tt> to false. The attribute
 * <tt>a_EchoCancellation</tt> indicates whether the Recognizer can provide
 * echo cancellation.
 *
 * <a name="xBargeIn"><!-- --></a>
 * <h3> Barge-In</h3>
 *
 * Barge-in (or "cut-through") allows a user to speak while a
 * prompt is playing, have the prompt stop playing, and be
 * recognized. Barge-in generally requires echo cancellation (so that the
 * echo of the prompt is not confused with the utterance); and
 * cooperation with the Player resource, so that the prompt terminates
 * when an utterance is either detected or recognized.
 * <p>
 * By way of illustration, here are two possible RTCs, 
 * each of which would accomplish barge-in:
 * <ul><li>
 * new RTC(ASR.rtcc_SpeechDetected, Player.rtca_Stop);
 * </li><li>
 * new RTC(ASR.rtcc_ValidUtteranceFound, Player.rtca_Stop);
 * </li></ul>
 * <p>
 * The first of these two RTCs causes the player to stop when a speaker
 * begins to speak, i.e., when the Recognizer detects that some speech
 * has occurred. The second RTC causes the player to stop
 * only when a meaningful utterance has been received, to guard against
 * stopping a prompt because of extraneous noise (e.g., traffic,
 * throat-clearing).
 *
 * <a name="5621"><!-- --></a>
 * <h4>Grammar-Based Barge-In</h4>
 *
 * The JSGF grammar specifiation language supports grammar tags to attach
 * application-specific information to parts of rule definitions. The
 * grammar tags do not affect recognition; instead, they are a mechanism
 * to provide specific information to the application about which part of
 * the grammar was traversed.
 * <p>
 * The ECTF has extended the the JSGF grammar specification language to
 * include a special tag to support barge-in,
 * <tt>v_BargeInHere</tt>. Unlike other grammar tags, which are merely
 * reported to the application as part of the result, this grammar tag
 * affects the operation of the ASR resource. When the ASR resource
 * encounters <tt>v_BargeInHere</tt> in the grammar, the ASR resource
 * raises the RTC condition <tt>rtcc_GrammarBargeIn</tt>. In turn, this RTC
 * condition may be used (as illustrated above) to control the operation
 * of a Player resource to effect barge-in.
 * <p>
 * The parameter <tt>p_GrammarBargeInThreshold</tt> is used to determine
 * the confidence level at which the ASR resource raises the RTC condition
 * <tt>rtcc_GrammarBargeIn</tt>.
 *
 * <h4>Parameter Updates During Barge-In</h4>
 *
 * Often a different set of speech recognition parameters (e.g., timeout
 * windows) must be used when a prompt is playing. The Dictionary
 * <tt>p_UpdateParametersList</tt> may be used to update parameters while
 * speech recognition is active. <tt>p_UpdateParametersList</tt> consists
 * of keys that are the names of ASR parameters, and values appropriate
 * for those keys.  When the function updateParameters() is called,
 * or the RTC action <tt>rtca_UpdateParameters</tt> is received, the
 * Recognizer will update each parameter in <tt>p_UpdateParametersList</tt>
 * with its corresponding value. Not all parameters may be included on
 * this list: parameters that control timeouts can be altered, but
 * parameters that control the nature of recognition results, the way
 * recognition starts and stops, etc., cannot.
 * <p>
 * For example, when recognition with barge-in first begins, the
 * parameter <tt>p_InitialTimeout</tt> will be set to the value
 * <tt>v_Forever</tt> to signify that there is no initial timeout while the
 * prompt is playing. However, after play has completed, the
 * UpdateParameters RTC action can be used to set <tt>p_InitialTimeout</tt>
 * to some reasonable value.
 *
 * <a name="57"><!-- --></a>
 * <h2>Recognition Results: ASREvent.TokenSequence</h2>
 *
 * <h3>Overview</h3>
 *
 * The <i>token</i> is the fundamental unit of recognition:
 * it may be a phrase, a word or a sub-word unit (such as a phoneme),
 * depending on the Recognizer. Tokens appear in a <i>sequence</i>;
 * a sequence represents a hypothesis for a transcription of the
 * utterance. The <i>result</i> of a recognition operation consists of
 * one or more <i>sequences</i> of recognition <i>tokens</i>.
 * Additional information elements may be associated with individual
 * tokens of a sequence, with a sequence itself, or with the
 * recognition results as a whole.
 * <p>
 * Recognition results are delivered in an ASREvent; 
 * the return value of recognize(), getResults() or getFinalResults().
 * Results are accessed using getTokenSequence(int row);
 * <p>
 * <B>Note:</b> <i>Still need to re-write the results presentation
 * document to reflect the S.410 convention for returning 
 * a <tt>TokenSequnece</tt>.
 * Result accessors are defined in {@link ASREvent}.</i>
 * <p>
 * In short:
 * <ul><li> 
 * An <tt>ASREvent</tt> may contain one or more <i>TokenSequences</i>.
 * </li><li>
 * Matrix rows are obtained from an ASREvent using <tt>getTokenSequence(int r)</tt>.
 * </li><li>
 * Individual Tokens are examined using <tt>TokenSequence.getToken(int o)</tt>.
 * </li><li>
 * Individual Scores are examined using <tt>TokenSequence.getScores(int o)</tt>.
 * </li><li>
 * Each TokenSequence has accessors for other attributes, for example:
 * <tt>TokenSequence.getSequenceScore()</tt>
 * </li></ul>
 * <p>
 * 
 * <b>The following is a literal copy from ECTF S.100-R2, 
 * included here for reference:</b>
 *
 * <p>
 * Recognition results are returned as a collection of one-dimensional and
 * two-dimensional matrices. Among the two-dimensional matrices, there
 * are: a token matrix T, a qualifier matrix Q, a grammar tag matrix G,
 * and a token score matrix S. The one-dimensional matrices will be
 * discussed below. In this chapter, these matrices are referred to as
 * the recognition results matrices.
 * <p>
 * Recognition results are described as a collection of one-dimensional and
 * two-dimensional matrices. Among the two-dimensional matrices, there are: 
 * <ul><li>
 * a token matrix <b>T</b> with elements t[r][o], 
 * </li><li>
 * a qualifier matrix <b>Q</b> with elements q[r][o], 
 * </li><li>
 * a grammar tag matrix <b>G</b> with elements g[r][o], and
 * </li><li>
 * a token score matrix <b>S</b> with elements s[r][o].
 * </li></ul>
 * <p>
 * Each matrix element t[r][o] of T is a token. 
 * The row index r of token t[r][o] indicates its ranking, that is,
 * its likelihood of being a correct recognition result. 
 * The column index o of t[r][o] indicates its order within a sequence 
 * with respect to other tokens; [o=0] being first element of the sequence.
 * <p>
 * A row of matrix T is referred to as a sequence; in a Type II 
 * recognizer (see below), a sequence represents a hypothesis for
 * a transcription of the utterance. For any matrix M with elements
 * m[r][o], we represent row i by the notation M[i].
 * <p>
 * The most likely transcription of an utterance is the top row 
 * of matrix T, T[0], {t[0][0], t[0][1], ... , t[0][N-1]}, 
 * where N is the maximum number of tokens in the sequence.
 * <p>
 * The other matrices Q, G, and S have the same dimensions as T; 
 * their matrix elements contain information about the
 * corresponding matrix elements of T. 
 * For example, each element s[i][j] of S indicates the likelihood score
 * (see section <a href="#5711">Scores</a>) of t[i][j].
 * <p>
 * One-dimensonal matrices m[r] provide information about the the
 * individual sequences T[r] of matrix T. For example, each member s'[r]
 * of the matrix SequenceScore (see Table 5-15) represents the overall
 * confidence of the Recognizer in the sequence T[r].
 * <p>
 * The recognition results matrices are returned as 
 * an ordered set of TokenSequence objects.
 * Each TokenSequence[r] contains all of the information
 * pertaining to row T[r], plus the parallel rows from the 
 * other matrices: Q[r], S[r], G[r], S'[r], etc.
 * That is, for a two dimensional matrix M[i],
 * TokenSequence[i] contains { M[i][0],Mt[i][1], ..., Mt[i][N-1] }.
 * <p>
 * TokenSequence[i] contains:
 * <pre>
 * int SequenceLength;
 * String[]           Token[i][j] j=0..SequenceLength
 * Symbol[]  TokenQualifier[i][j] j=0..SequenceLength
 * int[]         TokenScore[i][j] j=0..SequenceLength
 * String[]      GrammarTag[i][j] j=0..SequenceLength
 * String       ContextName[i]
 * Symbol          Language[i]
 * Symbol   LanguageVariant[i]
 * String SequenceQualifier[i]
 * int        SequenceScore[i]
 * </pre>
 * <p>
 * ASR resource output control parameters affect the sequence(s) of
 * tokens available to the application. For example, OutputType
 * determines whether the tokens represent phonetic symbols or text.
 * <p>
 * Recognition results are returned in the completion event of various
 * functions as well as in some unsolicited events, and may represent
 * either intermediate or complete results, depending on the event in
 * question.
 * <a name="5711"><!-- --></a>
 *
 * <h4>Scores</h4>
 *
 * Scores indicating a quantitative level of confidence in any token or
 * sequence may be optionally provided by the Recognizer. They may be
 * associated either with a recognized sequence, or with the individual
 * tokens of the sequence. Scores cover the numerical range 0 to 1000,
 * according to the descriptive quality bands given in Table 5-13.
 * <p>
 * <TABLE BORDER=1 CELLPADDING=3>
 * <tr><td><B>Score</B></TD>
 *     <td><B>Description</B></TD></TR>
 * <tr><td>801-1000</TD>
 *     <td>Excellent - there is no reason to reject this transcription</TD></TR>
 * <tr><td>601-800</TD>
 *     <td>Good - this transcription should be accepted unless the action which
 *         results is deemed to have dangerous or irreversible
 *         consequences</TD></TR>
 * <tr><td>401-600</TD>
 *     <td>Acceptable - this transcription is probably correct, but other
 *         alternatives may be worth considering if they have a similar
 *         score</TD></TR>
 * <tr><td>201-400</TD>
 *     <td>Questionable - this transcription should be verified by further
 *         prompting, or discarded</TD></TR>
 * <tr><td>0-200</TD>
 *     <td>Unacceptable</TD></TR>
 * </TABLE>
 *
 * <a name="ResultPresentation"><!-- --></a>
 * <h4>Type I and Type II Recognition Results</h4>
 *
 * Many recognizers are capable of returning a result which includes
 * multiple ("N-best") alternative sequences. These alternatives can be
 * presented in one of two forms, depending on whether the tokens
 * returned for each individual hypothesis are independent of each other
 * or dependent on the other tokens of the sequence.
 * <p>
 * Some recognizers (isolated word recognizers, for example) return a set
 * of sequences in which each token in a sequence is independent of the
 * other tokens in the sequence. Such recognizers are referred to as 
 * Type I recognizers. Their token matrix t[r][o], therefore, represents a
 * large number of possible individual sequences; for each value o, the
 * index r can be selected independently from the respective alternative
 * choices. For a matrix T with N rows and M columns, each combination of
 * the form
 * t[i0][0], t[i1][1], ..., t[iN-1][M-1]
 * is a valid sequence (where {i0, i1, ..., iN-1} are integers in the
 * range of {0, ..., N-1}).
 * <p>
 * Most connected word and continuous speech systems return recognition
 * results t[r][o] in which an individual token t[r][o] depends on the
 * values of the other tokens within the sequence t[r]. 
 * Such recognizers are referred to as Type II recognizers. 
 * For a Type II recognizer, only token sequences
 * t[j][0], t[j][1], ... , t[j][N-1] are valid recognition sequences.
 *
 * <a name="572"><!-- --></a>
 * <h3>Recognition Results and ASREvent methods</h3>
 *
 * Recognition results returned from retrieve(), getResults(), or getFinalResults
 * are obtained from an ASREvent Iwith eventID equal to, respectively, 
 * <tt>ev_Recognize</tt>, <tt>ev_GetResults</tt>, or <tt>ev_GetFinalResults</tt>)
 * using the following accessors:
 * <ul><li>
 * <tt>ASREvent.getResultPresentation()</tt>
 * indicates whether the results are of Type I or Type II.
 * [returns one of the Symbols <tt>v_TypeI</tt> or <tt>v_TypeII</tt>]
 * See <a href=ResultPresentation>Type I and Type II recognizers</a>
 * </li><li>
 * <tt>ASREvent.getTokenSequence(int row)</tt>
 * returns a TokenSequence object for one row of results.
 * </li></ul>
 *
 * <h3>ASREvent.TokenSequence and its accessors</h3>
 *
 * <tt>ASREvent.getTokenSequence(int r)</tt> returns 
 * a <tt>TokenSequence</tt> object for row <tt>r</tt>.
 * An <tt>ASREvent.TokenSequence</tt> object contains information associated 
 * with a single row of the recognition result matrices. 
 * <p>
 * The accessor methods for TokenSequence are defined in
 * {@link ASREvent.TokenSequence}
 * <P>
 * <b>Note:</b> the ASREvent returned from <tt>recognize()</tt>
 * contains only one TokenSequence, representing the most likely result.
 * <p>
 * Certain of the TokenSequence accessors are described
 * briefly in the following subsections.  
 *
 * <h4>TokenSequence.getToken(int o)</h4>
 *
 * <tt>getTokenSequence(r).getToken(o)</tt> returns the String that identifies 
 * the <i>token</i> that corresponds to the matrix element t[r][o].
 *
 * <h4>TokenSequence.getTokenQualifier(int o)</h4>
 * 
 * <tt>getTokenSequence(r).getTokenQualifier(o)</tt> returns the <tt>int</tt>
 * that corresponds to the matrix element q[r][o].
 * <p>
 * A token qualifier reports whether the corresponding token 
 * is "normal" (i.e., represents a token string), "grammar tag" 
 * (i.e., the token value is NULL, and a grammar tag is provided instead),
 * "garbage," or "silence."
 * <p>
 * This value is optional, if not supplied by the ASR Resource,
 * the returned value is null for all values of {j, o}
 * 
 * <a name="5733"><!-- --></a>
 * <h4>getGrammarTag(int o)</h4>
 *
 * <tt>getTokenSequence(r).getGrammarTag(o)</tt> returns the <tt>String</tt>
 * that corresponds to the matrix element q[r][o].
 * If <tt>getTokenSequence(r).getTokenQualifier(o)</tt> is <tt>v_GrammarTag</tt>,
 * then g[r][o] provides the grammar tag for the token t[r][o]. 
 * The grammar tag will include the braces "{}" as defined 
 * in the JSGF grammar specification.
 * <p>
 * This is an optional KVpair, defined for Type II ASR resources.
 * If not supplied, the return value is <tt>null</tt>.
 * <p>
 *
 * <a name="5734"><!-- --></a>
 * <h4>getTokenScore(int o)</h4>
 *
 * <tt>getTokenSequence(r).getTokenScore(o)</tt> returns the <tt>int</tt>
 * that corresponds to the matrix element s[r][o].
 * Matrix element s[r][o] provides the score for the token t[r][o].
 * <p>
 * This value is optional, if not supplied by the ASR Resource,
 * the returned value is -1 for all values of {j, o}
 *
 * <h2>Training Results</h2>
 *
 * <h3>Overview</h3>
 *
 * The Word is the fundamental unit of training. Words are trained
 * through several possible methods, including utterances, transcriptions
 * of phonemes, or through ordinary text. The Word is contained in a
 * Context. In most cases, the goal of training is to create a Context
 * which may be used in later acts of recognition.
 * <p>
 * The result of training is a Word contained in a Context. Depending
 * on the ASR resource, training can take place on Words that have been
 * freshly created; added to Words that already exist (to update their
 * training); or replace previous training.
 * <p>
 * Training a word using speech is a complex interaction. If utterances
 * (either of a particular speaker or a group of speakers) are used to
 * train, several utterances may be required. The application, through
 * training results, is informed of whether the training session was
 * successful, whether more utterances will be required, or whether
 * training is complete.
 * <h3>Training Results</h3>
 * Several different data items the training results. They
 * are included in the completion event of the
 * function wordTrain(). Readiness, available from the method
 * TrainEvent.getReadiness(), is a description of
 * the Word being trained. It is used to determine whether more training
 * is required before the Word can be committed; this value describes the
 * overall state of the Word being trained. For example, a training
 * utterance may fail, but the Word might already have sufficient
 * training to be committed. The possible values of Readiness are:
 * <p>
 * <TABLE BORDER=1 CELLPADDING=3>
 * <tr><td><tt>v_Ready</tt></TD>
 *     <td>The Word has sufficient training available.</TD></TR>
 * <tr><td><tt>v_NotReady</tt></TD>
 *     <td>The Word does not have sufficient training available</TD></TR>
 * <tr><td><tt>v_Complete</tt></TD>
 *     <td>The Word has sufficient training available, and additional
 *         training will be ignored.</TD></TR>
 * </TABLE>
 * <p>
 * Another value of interest to training results is the Qualifier
 * of the event, available using
 * {@link ResourceEvent#getQualifier getQualifier()}.
 * The Qualifier reveals the success or failure of the
 * latest utterance used to train the Word. Note the difference between
 * the "latest utterance" and the Word itself: the Word might have
 * sufficient utterances to be committed, even though the latest
 * utterance was problematical; a wise developer will check the latest
 * utterance, and if it is problematical, delete it with the command
 * wordDeleteLastUtterance(). (If wordDeleteLastUtterance()
 * is not supported by the resource, a problematical utterance will fail
 * instead of producing a warning.) The values for the Qualifier are:
 * <p>
 * <TABLE BORDER=1 CELLPADDING=3>
 * <tr><td><tt>q_Success</tt></TD>
 *     <td>The latest utterance was successfully added to the training
 *         set.</TD></TR>
 * <tr><td><tt>q_RTC</tt></TD>
 *     <td>The training was stopped by RTC</TD></TR>
 * <tr><td><tt>q_Stop</tt></TD>
 *     <td>The training was stopped by a stopASR or other Stop command.</TD></TR>
 * <tr><td><tt>q_Duration</tt></TD>
 *     <td>The training failed because the maximum duration
 *         (<tt>p_DurationMaximumTimeWindow</tt>) of the training window
 *          expired.</TD></TR>
 * <tr><td><tt>q_InitialTimeout</tt></TD>
 *     <td>The training failed because the initial timeout timer
 *         expired.</TD></TR>
 * <tr><td><tt>q_Failure</tt></TD>
 *     <td>The latest utterance was not successful, and was not added
 *         to the training set.</TD></TR>
 * <tr><td><tt>q_Warning</tt></TD>
 *     <td>Although the latest utterance was added to the training set,
 *         the Recognizer has detected potential problems. Check
 *         the TrainingResult for details.</TD></TR>
 * </TABLE>
 * <p>
 * For example, if the training was unsuccessful because the user was
 * silent, the value of the Qualifier will be either
 * <tt>q_Duration</tt> (if <tt>p_Duration</tt> expires) or
 * <tt>p_InitialTimeout</tt> (if that timer expires first).
 * <p>
 * Warnings are included as Values of TrainingResult and are accessed with
 * {@link ASREvent#getTrainingResult getTrainingResult()}.
 * A warning means either:
 * <ul>
 * <li>The utterance has been added to the word's training.
 * The ASR resource supports wordDeleteTraining(); the application can
 * determine if the training should be retained or deleted.
 * </li><li>
 * The utterance has not been added to the word's training,
 * since the ASR resource does not support wordDeleteTraining().
 * </li></ul>
 * <p>
 * Values of TrainingResult are:
 * <TABLE BORDER=1 CELLPADDING=3>
 * <tr><td><tt>v_Success</tt></TD>
 *     <td>The training succeeded.</TD></TR>
 * <tr><td><tt>v_Collision</tt></TD>
 *     <td>The training is too close to the training of another
 *         Word in an active Context on the Recognizer.</TD></TR>
 * <tr><td><tt>v_Inconsistent</tt></TD>
 *     <td>The training is inconsistent with previous training for
 *         this Word.</TD></TR>
 * <tr><td><tt>v_Noisy</tt></TD>
 *     <td>The background for this training was too noisy.</TD></TR>
 * </TABLE>
 * <p>
 * For example, if the training is suspect because the resource cannot
 * see any similarities between two training passes, and the resource
 * supports wordDeleteLastUtterance(), then it will return the
 * qualifier <tt>q_Warning</tt> and the TrainingResult
 * <tt>v_Inconsistent</tt>. The training will become part of the Word until
 * wordDeleteLastUtterance() is called. If the resource does not
 * support wordDeleteLastUtterance(), however, then the qualifier
 * <tt>q_Failure</tt> with the TrainingResult set to
 * <tt>v_Inconsistent</tt> is returned and the utterance does not become
 * part of the word.
 * <p>
 * To summarize, training can imply two loops:
 * <ul>
 * <li> an external loop for updating models on a word base</li>
 * <li> an internal loop for acquiring a number of repetitions of the
 * same word before activating the modeling phase.  </li>
 * </ul>
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */

public interface ASR extends Resource, ASRConstants 
{
    /**
     * Thrown when an operation is attempted using a Context
     * that has not been created on the ASR Resource.
     * The method <tt>contextCreate</tt> must be used to 
     * create the Context on the resource before the
     * Context can be used or trained.
     * <p>
     * This is a RuntimeException and may be thrown by any method 
     * that uses a Context, even though it is not declared.
     */
    public class NoContextException extends MediaRuntimeException {
	/**
	 * Constructs a <code>NoContextException</code>. 
	 */
	public NoContextException() {super();}
	
	/**
	 * Constructs a <code>NoContextException</code> 
	 * with the specified detail message. 
	 *
	 * @param   s   the detail message.
	 */
	public NoContextException(String s) {super(s);}
    }

    /** Initiate speech recognition on the ASR resource.
     * The returned event contains the highest-probablility result.
     * That is, it contains only one <tt>TokenSequence</tt>.
     * To get the full set of results, use <tt>getFinalResults()</tt>.
     *
     * @param rtcs a RTC[] containing RTCs to be in effect during this call.
     * @param optargs a Dictionary containing optional arguments.
     * @return an ASREvent, when the operation is complete.
     * @throws MediaResourceException if requested operation fails.
     * @throws DisconnectedException if Terminal is disconnected.
     * 
     * @see ASREvent
     */
    public ASREvent recognize(RTC[] rtcs, Dictionary optargs)
	throws MediaResourceException;

    /** Retrieve intermediate recognition results 
     * and do <b>not</b> reset the recognizer.
     * This method is valid whenever partial results are ready.
     * The recognizer can be in either of state
     * <tt>Recognizing</tt> or <tt>ResultsReady</tt>.
     *
     * @return an ASREvent containing one or more TokenSequences.
     * @throws MediaResourceException if requested operation fails.
     */
    public
    ASREvent getResults()
	throws MediaResourceException;


    /** Retrieve recognition results and reset the recognizer.
     * <h4>Pre-conditions</h4>
     * Recognizer must be in the <tt>ResultsAvailable</tt> state.
     * <h4>Post-conditions</h4>
     * Recognizer is in the <tt>Idle</tt> state.
     * <p>
     * This is roughly equivalent to <tt>getResults()</tt>
     * followed by <tt>idleASR()</tt>.
     * 
     * @return an ASREvent containing one or more TokenSequences.
     * @throws MediaResourceException if requested operation fails.
     */
    public
    ASREvent getFinalResults()
	throws MediaResourceException;

    
    /* RTC Action Methods */
    /**
     * Starts a recognizer that is in a paused state.
     * The Paused states are <tt>v_RecognitionPaused</tt> and
     * <tt>v_TrainingPaused</tt>; the Recognizer moves to the states
     * <tt>v_Recognizing</tt> and <tt>v_Training</tt>, respectively.
     * <p>
     * Corresponds to the <tt>ASR.rtca_Start</tt> action.
     *
     * @return an ASREvent when the operation is complete.
     * @throws MediaResourceException if requested operation fails.  
     */
    ASREvent startASR()
	throws MediaResourceException;

    /**
     * Stops the Recognizer when it is in the
     * "Recognizing" or "Training" states.
     * The recognizer transitions to the next state 
     * as if though they had completed execution. 
     * <p>
     * For example, the ASR resource would move from <tt>v_Recognizing</tt>
     * to <tt>v_ResultsAvailable</tt>.
     * <p>
     * Corresponds to the <tt>ASR.rtca_Stop</tt> action.
     *
     * @return an ASREvent when the operation is complete.
     * @throws MediaResourceException if requested operation fails.
     */
    ASREvent stopASR()
	throws MediaResourceException;

    /**
     * Forces a Recognizer into the Idle state. 
     * <p>
     * The function is valid at any time, and is commonly used to move
     * a Recognizer from the <tt>v_ResultsAvailable</tt> state into the
     * <tt>v_Idle</tt> state without retrieving results from the Recognizer.
     * <p>
     * <b>Note:</b> on entry to the Idle state, any recognition results or
     * uncommitted training data is lost.
     * <p>
     * Corresponds to the <tt>ASR.rtca_Idle</tt> action.
     *
     * @return an ASREvent when the operation is complete.
     * @throws MediaResourceException if requested operation fails.
     */
    ASREvent idleASR()
	throws MediaResourceException;

    /**
     * Set resource parameters according to <tt>p_UpdateParametersList</tt>.
     * <p>
     * This function (also available as <tt>ASR.rtca_updateParameters</tt>)
     * is useful for <i>barge-in</i> scenarios, where the <tt>timeout</tt>
     * values change after the prompt has completed playing.
     * <p>
     * If <tt>p_UpdateParametersList</tt> is not set or empty,
     * then nothing is done and this method returns normally.
     * 
     * @return an ASREvent when the operation is complete.
     * @throws MediaResourceException if requested operation fails.
     */
    ASREvent updateParameters()
	throws MediaResourceException;

    
    /* Rule Expansion Mehtods */
    /**
     * Retrieve the expansion of a rule from a Grammar Context.
     * <p>
     * For a Grammar Context loaded on a resource, this method
     * retrieves the current expansion of the rule RuleName 
     * in the Grammar Context <tt>grammarContext</tt>. 
     * The rule expansion is returned as part of
     * the completion event for this function and is available using
     * {@link ASREvent#getRuleExpansion ASREvent.getRuleExpansion()}.
     *
     * @param grammarContext the name of a loaded grammar Context.
     * @param ruleName a String that names the rule to be expanded.
     * @return a String containing the rule expansion.
     * @throws MediaResourceException if requested operation fails.
     * @see #setRuleExpansion
     */
    String getRuleExpansion(String grammarContext, String ruleName)
	throws MediaResourceException;

    /**
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
     * @return an ASREvent when the operation is complete.
     * @throws MediaResourceException if requested operation fails.
     * @see #getRuleExpansion
     */
    ASREvent setRuleExpansion(String grammarContext, 
			      String ruleName,
			      String ruleExpansion)
	throws MediaResourceException;
 
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
     * The <tt>direction</tt> argument Symbol specifies whether the copy is
     * <br>from a resource to a container (<tt>v_FromResource</tt>)
     * or
     * <br>from a container to a resource (<tt>v_ToResource</tt>). 
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
     * application must take corrective action; for example, free up room
     * on the ASR Resource by using <tt>contextRemove()</tt>.
     * <p>
     * A Context copied into the Resource is inactive,
     * it joins the set of inactive contexts.
     * <p>
     *
     * @param resourceContext a Context within the ASR Resource.
     * @param containerContext a String containing the name of the container
     * context.
     * @param direction a Symbol indicating the direction and type of copy.
     * Must be one of {@link ASRConstants#v_ToResource},
     *     {@link ASRConstants#v_FromResource}, or
     *     {@link ASRConstants#v_ToResourceTraining}.
     * @throws MediaResourceException if requested operation fails.  
     */
    void contextCopy(String resourceContext,
		     String containerContext, 
		     Symbol direction)
	throws MediaResourceException;

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
     * @throws MediaResourceException if requested operation fails.
     */
    void contextCreate(String resourceContext,
		       Symbol trainingType,
		       Dictionary contextParams)
	throws MediaResourceException;

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
     * @return a Dictionary of parameter Symbols and their values.
     *
     * @throws MediaResourceException if requested operation fails.
     */
    Dictionary contextGetParameters(String resourceContext, Symbol[] keys)
	throws MediaResourceException;

    /**
     * Remove an existing Context from an ASR Resource.
     * The Context which is removed is lost.
     * <p>
     * To preserve a Context, copy it to a Container using the
     * contextCopy() method.
     *
     * @param resourceContext the Context to be removed.
     * @throws MediaResourceException if requested operation fails.
     */
    void contextRemove(String resourceContext)
	throws MediaResourceException;

    /**
     * This function sets the parameters associated with a particular
     * Context that is loaded on a Resource.
     *
     * @param resourceContext the Context on which parameters are to be set.
     * @param contextParams a Dictionary of parameters to set.
     *
     * @throws MediaResourceException if requested operation fails.
     */
    void contextSetParameters(String resourceContext, Dictionary contextParams)
	throws MediaResourceException;
    
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
     * @return an ASREvent when the operation is complete.
     * @throws MediaResourceException if requested operation fails.  */
    ASREvent wordCommit(String wordContext, String wordTemp, String wordString)
	throws MediaResourceException;

    /**
     * Create a new <b>Word</b> within a loaded Context.
     * The <b>Word</b> is accessed by using <tt>wordTemp</tt>,
     * a temporary identifier. This method can only be used with
     * Recognizers that support Context modification. Creating a
     * <b>Word</b> adds it to the Context's <tt>p_WordList</tt> parameter.
     * 
     * @param wordContext the Context in which Word is created.
     * @param wordTemp a String containing the temporary name of the word.
     * @return an ASREvent when the operation is complete.
     * @throws MediaResourceException if requested operation fails.
     */
    ASREvent wordCreate(String wordContext, String wordTemp)
	throws MediaResourceException;

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
     * @return an ASREvent when the operation is complete.
     * @throws MediaResourceException if requested operation fails.
     */
    ASREvent wordDeleteLastUtterance(String wordContext, String wordTemp)
	throws MediaResourceException;

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
     * @return an ASREvent when then operation is complete.
     * @throws MediaResourceException if requested operation fails.
     */
    ASREvent wordDeleteTraining(String wordContext, String wordString)
	throws MediaResourceException;

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
     * @return an ASREvent when the operation is complete.
     * @throws MediaResourceException if requested operation fails.
     */
    ASREvent wordDestroy(String wordContext, String wordString)
	throws MediaResourceException;

    /**
     * Train a Word in a Context. The Recognizer
     * collects audio information (or other information; 
     * see {@link ASRConstants#p_TrainingType <tt>p_TrainingType</tt>}), 
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
     * @return an ASREvent when the operation is complete.
     * @throws MediaResourceException if requested operation fails.  
     */
    ASREvent wordTrain(String wordContext, String wordString)
	throws MediaResourceException;
}

