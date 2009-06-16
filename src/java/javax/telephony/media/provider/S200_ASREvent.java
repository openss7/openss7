/*
 * S200_ASREvent.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

package javax.telephony.media.provider;

import javax.telephony.media.async.*;
import javax.telephony.media.*;
import java.util.Dictionary;
import java.util.Hashtable;
import java.util.EventListener;

/**
 * Defines the accessor methods used by S.200 ASR Events.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public class S200_ASREvent extends Base_ASREvent {
    /* S.200 Message/Event/Payload structure:
    RetrieveRecognition Event.payload: {
	ResultType = Intermediate/Final
	RecognitionResults (results) = Dictionary: {
	    ResultPresentation = TypeII/TypeI
	    RecognitionSequences = Dictionary[n] {
		Context = String
		GrammarTags = String[]
		Language_Language = Symbol   Note: Language_ECTF_Language
		Language_Variant = Symbol    Note: Language_ECTF_Variant
		SequenceSpeaker = String
		SequenceQualifier = Symbol {Normal, Garbage, Silence}
		SequenceScore = int
		TokenRow = String[]
		TokenQualifiers = Symbol[]
		TokenScores = int[]
	    } // "n" of these Dictionary, one for each result row.
	} // end of RecognitionResults KVSet
    }
    */

    /**
     * Create specific completion or non-transaction event from ASR.
     *
     * @param source the Object that created this EventObject.
     * @param eventID a Symbol that identifies this event: 
     * ASR.ev_Recognize
     */
    public S200_ASREvent(Object source, Symbol eventID) {
	super(source, eventID);
    }

    /** Constructor for non-trans vesion */
    public S200_ASREvent(Object source, Symbol eventID, boolean isNonTrans)
    {
	this(source, eventID);
	this.isNonTrans = isNonTrans;
    }

    /** extract Result values from payload into local fields. */
    public void setFields() {
	// Adjust S.200 eventID to S.400 specs: (for non-Trans events?)
	// that is: ev_GetResults or ev_GetFinalResults
	if (eventID.equals(ev_RetrieveRecognition)) {
	    eventID = (Symbol)payload.get(p_ResultType);
	}
	Dictionary results;	// ResultPresentation, RecognitionSequences.
	results = (Dictionary)payload.get(ESymbol.ASR_RecognitionResults);
	if (results == null) return; // event has no recongition results

	resultPresentation = (Symbol)results.get(ESymbol.ASR_ResultPresentation);
	// extract RecognitionSequences,
	// convert to TokenSequence[]
	Object[] seqs = null;	     // a Dictionary[]
	seqs = (Object[])results.get(ESymbol.ASR_RecognitionSequences);
	if (seqs == null) return;    // unlikely... there should be at least 1
	// create array of TokenSequence objects:
	tokenSequences = new TokenSequence[seqs.length];
	for(int i = 0; i < seqs.length; i++) {
	    tokenSequences[i] = new TokenSequence(i, (Dictionary)seqs[i]);
	}
    }

    /** ResultPresentation (if there were results, from setFields) */
    protected Symbol resultPresentation = null;

    private final static
    TokenSequence[] emptyTokenSeqArray = new TokenSequence[0];

    /** ResultRows: tokenSequences (if there were results, from setFields).
     * Note: sub-class must not modify <tt>emptyTokenSeqArray</tt>; 
     * must set a new value.
     */
    protected TokenSequence[] tokenSequences = emptyTokenSeqArray;

    // Fields for RecognitionEvents:

    public Symbol getResultPresentation() {
	waitForEventDone(); 
	return resultPresentation;
    }
    public int getNumberOfSequences() {
	waitForEventDone(); 
	return tokenSequences.length; 
    }
    public ASREvent.TokenSequence getTokenSequence(int seqNum) {
	waitForEventDone(); 
	return tokenSequences[seqNum];
    }
    public Dictionary getContextDictionary() {
	waitForEventDone(); 
	return (Dictionary)payload.get(ESymbol.ASR_ContextParms);
    }
    public String getRuleExpansion() {
	waitForEventDone(); 
	return (String)payload.get(ESymbol.ASR_RuleExpansion);
    }
    public String getWordName() {
	waitForEventDone(); 
	return (String)payload.get(ESymbol.ASR_WordName);
    }
    public String getContextName() {
	waitForEventDone(); 
	return (String)payload.get(ESymbol.ASR_ContextName);
    }
    public Symbol getReadiness() {
	waitForEventDone(); 
	return (Symbol)payload.get(ESymbol.ASR_Readiness);
    }
    public Symbol getTrainingResult() {
	waitForEventDone(); 
	return (Symbol)payload.get(ESymbol.ASR_TrainingResult);
    }

    /** a simple implementation of ASREvent.TokenSequence. */
    static class TokenSequence implements ASREvent.TokenSequence {
	int row = 0;		// row from which this was derived
	private Dictionary sequence; // KVSet for this row
	private int sequence_length = 0;
	TokenSequence(int row, Dictionary sequence) {
	    this.row = row;
	    this.sequence = sequence;
	    tokenRow = (String[])sequence.get(ESymbol.ASR_TokenRow);
	    sequence_length = tokenRow.length;
	}
	public String toString() {
	    return "TokenSequence["+row+"] " + sequence;
	}

	/**
	 * Get the context from which the sequence is taken.
	 * @return String containing the name of the context.
	 */
	public String getContextName() {
	    return (String)sequence.get(ESymbol.ASR_Context);
	}	    

	/**
	 * Get the language of the sequence.
	 * @return Symbol containing the language of the sequence.
	 */
	public Symbol getLanguage() {
	    return (Symbol)sequence.get(ESymbol.Language_Language);
	}

	/**
	 * Get the language variant of the sequence.
	 * @return Symbol containing the language variant.
	 */
	public Symbol getLanguageVariant() {
	    return (Symbol)sequence.get(ESymbol.Language_Variant);
	}

	/**
	 * Get the name of the speaker of the sequence.
	 * @return String containing the name of the speaker.
	 */
	public String getSequenceSpeaker() {
	    return (String)sequence.get(ESymbol.ASR_SequenceSpeaker);
	}

	/**
	 * Get an overall qualifier of the sequence.
	 * @return Symbol containing one of
	 * {@link ASRConstants#v_Normal},
	 * {@link ASRConstants#v_Garbage}, and
	 * {@link ASRConstants#v_Silence}.
	 */
	public Symbol getSequenceQualifier() {
	    return (Symbol)sequence.get(ESymbol.ASR_SequenceQualifier);
	}

	/**
	 * Get the recognizer's confidence in the sequence of tokens.
	 * @return int value in the range 0 to 1000.
	 * or -1 if SequenceScore is not provided.
	 */
	public int getSequenceScore() {
	    Integer temp = ((Integer)sequence.get(ESymbol.ASR_SequenceScore));
	    return (temp == null)? -1 : temp.intValue();
	}

	/**
	 * Get the number of tokens in this result sequence.
	 * 
	 * @return the int number of tokens in this sequence.
	 */
	public int getSequenceLength() {
	    return sequence_length;
	}
	
	/** set in constructor */
	protected String[] tokenRow = null;
	/**
	 * Get the name of the <tt>jth</tt> token in this sequence.
	 * @return the name String that identifies the <tt>jth</tt> token.
	 * @see ASR#wordCommit
	 */
	public String getToken(int j) {
	    return (tokenRow == null) ? null : tokenRow[j];
	}

	private static final Symbol[] noSymbols = new Symbol[0];
	/** set when requested. */
	protected Symbol[] tokenQuals = noSymbols;

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
	 */
	public Symbol getTokenQualifier(int j) {
	    if (tokenQuals == noSymbols) 
		tokenQuals = (Symbol[])sequence.get(ESymbol.ASR_TokenQualifiers);
	    return (tokenQuals == null) ? null : tokenQuals[j];
	}
	
	
	private static final Integer[] noInts = new Integer[0];
	/** set when requested. */
	protected Integer[] tokenScores = noInts;
	/**
	 * Get the the recognizer's confidence for the <tt>jth</tt> token.
	 * @return an int in the range 0 to 1000, 
	 * or -1 if TokenScores are not provided.
	 */
	public int getTokenScore(int j) {
	    if (tokenScores == noInts) 
		tokenScores = ((Integer[])sequence.get(ESymbol.ASR_TokenScores));
	    // by contract, if the tokenScore[] is non-null,
	    // then it must contain a full set of valid Integers.
	    // If not, then we *want* to throw an Exception
	    // to indicate a non-conformant ASR Resource.
	    return (tokenScores == null) ? -1 : tokenScores[j].intValue();
	}

	private static final String[] noStrings = new String[0];
	/** set when requested. */
	String[] grammarTags = noStrings;
	/**
	 * Get the GrammarTag for the <tt>jth</tt> token in this sequence.
	 * <p>
	 * <b>Note:</b> 
	 * <i>This can probably be folded into <tt>getToken</tt>
	 * because the TokenQualifier indicates whether the <tt>jth</tt>
	 * element is a GrammarTag
	 * </I>.
	 * @return the GrammarTag String for the <tt>jth</tt> token.
	 *
	 * @deprecate use getToken() to get the jth token, 
	 * getTokenQualifier() indicates it is a GrammarTag.
	 */
	public String getGrammarTag(int j) {
	    if (grammarTags == noStrings)
		grammarTags = ((String[])sequence.get(ESymbol.ASR_GrammarTags));
	    return (grammarTags == null) ? null : grammarTags[j];
	}
    }

}
