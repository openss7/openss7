/*
 * ASREvent.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Defines the methods for ASR Events.
 * <p>
 * Recognition Results are returned in an ASREvent.
 * Results consist of one or more TokenSequences.
 * <ul><li> 
 * An <tt>ASREvent</tt> may contain one or more <i>TokenSequences</i>.
 * </li><li>
 * Matrix rows are extracted from the ASREvent using getTokenSequence(int r).
 * </li><li>
 * </li><li>
 * Individual Tokens are examined using TokenSequence.getToken(int o).
 * </li><li>
 * Individual Scores are examined using TokenSequence.getScores(int o).
 * </li><li>
 * Each TokenSequence has accessor for other attributes, for example:
 * <tt>TokenSequence.getSequenceScore()</tt>
 * </li></ul>
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public 
interface ASREvent extends ResourceEvent, ASRConstants {
    /** methods for Recognize and RetrieveRecognition */

    /**
     * Return a Symbol indicating the type of results.
     * @return one of the Symbols {@link ASRConstants#v_TypeI} or
     * {@link ASRConstants#v_TypeII}.
     */
    Symbol getResultPresentation();
    
    /**
     * The number of TokenSequences available.
     * For <tt>eventID</tt> equal <tt>ASR.ev_recognize</tt>
     * this will be at most 1.
     * @return the int number of TokenSequences available for this event.
     */
    int getNumberOfSequences();
    
    /**
     * Return one recognition sequence.
     * Sequences are ranked in order of likelihood of being correct.
     * Sequence number zero is the most likely.
     * <p>
     * <i>This presentation may be sub-optimal for Type-I results.</i>
     * <p>
     * If <tt>sequenceNumber</tt> is out of range <tt>null</tt> is returned.
     *
     * @param sequenceNumber an int specifying the sequence to be returned;
     * valid values are non-negative and less than <tt>getNumberOfSequences()</tt>.
     * @return a TokenSequence containing the specified row of results.
     * 
     * @see ASR#getResults ASR.getResults()
     * @see ASR#getFinalResults ASR.getFinalResults()
     * @see ASR#recognize ASR.recognize()
     */
    TokenSequence getTokenSequence(int sequenceNumber);

    /* Implementor's note: the object implementing TokenSequence
     * can be as simple as a struct of two Fields: 
     * ASREvent ev = this; and int row = row;
     * The implementation of accessors in TokenSequence
     * can use those two items to index into the 2D arrays
     * delivered from S.200
     */

    /**
     * Defines the methods for retrieving a single sequence of Tokens.
     * <p>
     * For TypeI results,
     * each TokenSequence holds one row of result Tokens.
     * For TypeII results,
     * each TokenSequence is one potential interpretation of the utterance.
     * <p>
     * Each TokenSequence consists of several attributes, 
     * plus a sequence of Token Strings, 
     * and parallel sequences of TokenQualifiers and TokenScores.
     * <p>
     * <b>Note:</b> Some of these values are 'optional', and may not
     * be supplied in the ASREvent.  If not supplied, the return value
     * is <tt>null</tt>, or for return values of type <tt>int</tt>
     * the value will be <tt>-1</tt>.
     * 
     * @see ASREvent#getTokenSequence ASREvent.getTokenSequence
     */
    public
    interface TokenSequence {
	/**
	 * Get the number of tokens in this result sequence.
	 * 
	 * @return the int number of tokens in this sequence.
	 */
	int getSequenceLength();
	
	/**
	 * Get the name of the <tt>jth</tt> token in this sequence.
	 * @param j index of the token to be returned. 
	 * @return the name String that identifies the <tt>jth</tt> token.
	 * @see ASR#wordCommit
	 */
	String getToken(int j);

	/**
	 * Get the qualifier for the <tt>jth</tt> token in this sequence.
	 * The returned Symbol indicates whether the <tt>jth</tt> token
	 * is to be interpreted as garbage, silence, an actual token, or
	 * a grammar tag.
	 * <p>
	 * Possible values are 
	 * {@link ASRConstants#v_Normal},
	 * {@link ASRConstants#v_Garbage}, and 
	 * {@link ASRConstants#v_Silence}.
	 * For Type II results, the value may also be 
	 * {@link ASRConstants#v_GrammarTag}.
	 *
	 * @param j index of the token to be returned. 
	 * @return the Qualifier Symbol for the jth token.
	 */
	Symbol getTokenQualifier(int j);
	
	/**
	 * Get the GrammarTag for the <tt>jth</tt> token in this sequence.
	 * [optional]
	 * <p>
	 * When getToken(j) is null, one should check getTokenQualifier(j)
	 * to see if there is a valid grammerTag in getGrammerTag(j).
	 *
	 * <b>Note:</b> 
	 * <i>This can probably be folded into <tt>getToken</tt>
	 * because the TokenQualifier indicates whether the <tt>jth</tt>
	 * element is a GrammarTag
	 * </I>.
	 * <p>
	 * @param j index of the token for which the GrammarTag is returned. 
	 * @return the GrammarTag String for the <tt>jth</tt> token.
	 */
	String getGrammarTag(int j);

	/**
	 * Get the the recognizer's confidence for the <tt>jth</tt> token.
	 * [optional]
	 * <p>
	 * <b>Note:</b> getTokenScore(0) should return -1 iff
	 * getTokenScore(i: i &lt; sequenceLength) returns -1.
	 * An application should not need to check each value
	 * for validity.
	 * 
	 * @param j index of the token for which the confidence is returned. 
	 * @return an int in the range 0 to 1000,
	 * or -1 if TokenScores are not provided.
	 */
	int getTokenScore(int j);

	/**
	 * Get the context from which the sequence is taken.
	 * @return String containing the name of the context.
	 */
	String getContextName();

	/**
	 * Get the language of the sequence.
	 * [optional]
	 * @return Symbol containing the language of the sequence.
	 */
	Symbol getLanguage();

	/**
	 * Get the language variant of the sequence.
	 * [optional]
	 * @return Symbol containing the language variant.
	 */
	Symbol getLanguageVariant();

	/**
	 * Get the name of the speaker of the sequence.
	 * @return String containing the name of the speaker.
	 */
	String getSequenceSpeaker();

	/**
	 * Get an overall qualifier of the sequence.
	 * @return Symbol containing one of
	 * {@link ASRConstants#v_Normal},
	 * {@link ASRConstants#v_Garbage}, and
	 * {@link ASRConstants#v_Silence}.
	 */
	Symbol getSequenceQualifier();

	/**
	 * Get the recognizer's confidence in the sequence of tokens.
	 * [optional]
	 * @return int value in the range 0 to 1000,
	 * or -1 if SequenceScore is not provided.
	 */
	int getSequenceScore();

    }

    /* special fields for context, training, and rule expansion. */

    /**
     * Return a Dictionary containing the context parameters.
     * <p>
     *
     * @return context parameter Dictionary.
     * @see ASR#contextGetParameters(String, Symbol[]) ASR.contextGetParameters
     */
    Dictionary getContextDictionary();

    /**
     * Return the rule expansion from getRuleExpansion.
     * <p>
     * @return a String which is the rule expansion.
     * @see ASR#getRuleExpansion ASR.getRuleExpansion()
     */
    String getRuleExpansion();

    /**
     * Return the name of the affected word.
     * <p>
     *
     * @return a String which is the name of the word.
     * @see ASR#wordCommit ASR.wordCommit()
     * @see ASR#wordCreate ASR.wordCreate()
     * @see ASR#wordDeleteTraining ASR.wordDeleteTraining()
     * @see ASR#wordDestroy ASR.wordDestroy()
     * @see ASR#wordTrain ASR.wordTrain()
     */
    String getWordName();

    /**
     * Return the name of the affected context.
     * @return a String which is the context name.
     * @see ASR#contextRemove ASR.contextRemove()
     * @see ASR#wordCommit ASR.wordCommit()
     * @see ASR#wordCreate ASR.wordCreate()
     * @see ASR#wordDeleteLastUtterance ASR.wordDeleteLastUtterance()
     * @see ASR#wordDeleteTraining ASR.wordDeleteTraining()
     * @see ASR#wordDestroy ASR.wordDestroy()
     * @see ASR#wordTrain ASR.wordTrain()
     */
    String getContextName();

    /**
     * Return a Symbol telling if the word has been sufficiently trained.
     * It is used to determine whether more training is required before
     * the Word can be committed; this value describes the overall
     * state of the Word being trained.
     *
     * @return one of 
     * {@link ASRConstants#v_Ready},
     * {@link ASRConstants#v_NotReady}, 
     * {@link ASRConstants#v_Complete}.
     *
     * @see ASR#wordTrain ASR.wordTrain()
     */
    Symbol getReadiness();
    
    /**
     * Return a Symbol describing the results of training.
     * @return one of 
     * {@link ASRConstants#v_Success},
     * {@link ASRConstants#v_Collision},
     * {@link ASRConstants#v_Inconsistent},
     * {@link ASRConstants#v_Noisy}.
     * @see ASR#wordTrain ASR.wordTrain()
     */
    Symbol getTrainingResult();

}
