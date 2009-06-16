/*
 * S200_PlayerEvent.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

import javax.telephony.media.*;
import javax.telephony.media.async.*;
import java.util.EventListener;

/**
 * Base_PlayerEvent defines the  methods used by Player Events.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public
class S200_PlayerEvent extends Base_PlayerEvent
    implements Async_PlayerEvent {
    /**
     * Create specific completion or non-transaction event from Player.
     *
     * @param source the Object that created this EventObject.
     * @param eventID a Symbol that identifies this event: 
     * ev_Play
     */
    public S200_PlayerEvent(Object source, Symbol eventID) {
	super(source, eventID);
	// set flag values:
	index = -2;
	offset = -2;
    }

    /** Constructor for non-trans vesion */
    public S200_PlayerEvent(Object source, Symbol eventID, boolean isNonTrans)
    {
	this(source, eventID);
	this.isNonTrans = isNonTrans;
    }

    /**
     * Return index (zero based) into TVMList of last TVM played.
     * @return the index of the TVM that stopped.
     */
    public int getIndex() {
	waitForEventDone(); 
	if (index != -2) return index;
	Integer tvmInt = (Integer) payload.get(ESymbol.Player_TVM);
	index = (tvmInt != null) ? tvmInt.intValue() : -1;
	return index;
    }
    
    /**
     * Return index into a TVM phrase where play stopped.
     * @return the index into a TVM phrase where play stopped, paused, etc.
     */
    public int getOffset() {
	waitForEventDone(); 
	if (offset != -2) return offset;
	Integer offInt = (Integer) payload.get(ESymbol.Player_Offset);
	offset = (offInt != null) ? offInt.intValue() : -1;
	return offset;
    }

    /**
     * Return the RTC action Symbol that caused this change.
     * Valid in non-trans Speed/Volume/Jump events.
     * @return one of the rtca_ Symbols.
     */
    public Symbol getChangeType() {
	waitForEventDone(); 
	if (changeType != null) return changeType;
	changeType = (Symbol)payload.get(ESymbol.Player_ChangeType);
	if (eventID.equals(ev_Jump)) {
	    // Need to derive the correct changeType from type & unit
	    Symbol type = (Symbol)payload.get( ESymbol.Player_Type );
	    Symbol unit = (Symbol)payload.get( ESymbol.Player_Unit );
	    changeType = getJumpRTC( type, unit );
	}
	return changeType;
    }

    /**
     * Return the RTC Action Symbol corresponding to the given
     * type/unit values. 
     * Returns null for invalid type/unit combinations.
     * <p>
     * Background: S.200 wants to split the rtca_Jump commands
     * into two Symbols: JumpType (for,back,start,end) 
     * and JumpUnit (Time, MDO, MDOList, Sentence)
     * This routine to reconstruct the single Symbol.
     */
    static public Symbol getJumpRTC( Symbol type, Symbol unit ) {
        for (int i=0; i < jumpSymbols.length; i++) {
            if (jumpSymbols[i][0].equals(type) && jumpSymbols[i][1].equals(unit))
                return jumpSymbols[i][2];
        }
        return null;
    }

    static public Symbol getJumpType( Symbol rtca ) {
        for (int i=0; i < jumpSymbols.length; i++) {
            if (jumpSymbols[i][2].equals(rtca))
                return jumpSymbols[i][0];
        }
        return null;
    }

    static public Symbol getJumpUnit( Symbol rtca ) {
        for (int i=0; i < jumpSymbols.length; i++) {
            if (jumpSymbols[i][2].equals(rtca))
                return jumpSymbols[i][1];
        }
        return null;
    }
    
    /**
     * Table to translate between Jump rtca Symbols and Jump (Type,Unit) symbols
     * <ul> The columns in the table are:
     * <li> the  Type value </li>
     * <li> the  Unit value </li>
     * <li> the  RTC Action Symbol </li>
     * </ul>
     * Type/Unit pairs not in the table are invalid.
     */
    private static final Symbol[][] jumpSymbols = {
        { ESymbol.Player_JumpStart,    ESymbol.Player_JumpTVMList,  rtca_JumpStartMDOList },
        { ESymbol.Player_JumpEnd,      ESymbol.Player_JumpTVMList,  rtca_JumpEndMDOList   },

        { ESymbol.Player_JumpForward,  ESymbol.Player_JumpTVM,      rtca_JumpForwardMDOs  },
        { ESymbol.Player_JumpBackward, ESymbol.Player_JumpTVM,      rtca_JumpBackwardMDOs },
        { ESymbol.Player_JumpStart,    ESymbol.Player_JumpTVM,      rtca_JumpStartMDO     },
        { ESymbol.Player_JumpEnd,      ESymbol.Player_JumpTVM,      rtca_JumpEndMDO       },

        { ESymbol.Player_JumpForward,  ESymbol.Player_JumpTime,     rtca_JumpForwardTime  },
        { ESymbol.Player_JumpBackward, ESymbol.Player_JumpTime,     rtca_JumpBackwardTime },

	{ ESymbol.Player_JumpForward,  ESymbol.Player_JumpWord,     rtca_JumpForwardWords },
	{ ESymbol.Player_JumpBackward, ESymbol.Player_JumpWord,     rtca_JumpBackwardWords},

	{ ESymbol.Player_JumpStart,    ESymbol.Player_JumpSentence, rtca_JumpStartSentence},
	{ ESymbol.Player_JumpEnd,      ESymbol.Player_JumpSentence, rtca_JumpEndSentence  },
	{ ESymbol.Player_JumpForward,  ESymbol.Player_JumpSentence, rtca_JumpForwardSentences},
	{ ESymbol.Player_JumpBackward, ESymbol.Player_JumpSentence, rtca_JumpBackwardSentences},
    };

}

