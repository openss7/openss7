/*
 @(#) $RCSfile$ $Name$($Revision$) $Date$ <p>
 
 Copyright &copy; 2008-2009  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
 Copyright &copy; 2001-2008  OpenSS7 Corporation <a href="http://www.openss7.com/">&lt;http://www.openss7.com/&gt;</a>. <br>
 Copyright &copy; 1997-2001  Brian F. G. Bidulock <a href="mailto:bidulock@openss7.org">&lt;bidulock@openss7.org&gt;</a>. <p>
 
 All Rights Reserved. <p>
 
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license. <p>
 
 This program is distributed in the hope that it will be useful, but <b>WITHOUT
 ANY WARRANTY</b>; without even the implied warranty of <b>MERCHANTABILITY</b>
 or <b>FITNESS FOR A PARTICULAR PURPOSE</b>.  See the GNU Affero General Public
 License for more details. <p>
 
 You should have received a copy of the GNU Affero General Public License along
 with this program.  If not, see
 <a href="http://www.gnu.org/licenses/">&lt;http://www.gnu.org/licenses/&gt</a>,
 or write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA. <p>
 
 <em>U.S. GOVERNMENT RESTRICTED RIGHTS</em>.  If you are licensing this
 Software on behalf of the U.S. Government ("Government"), the following
 provisions apply to you.  If the Software is supplied by the Department of
 Defense ("DoD"), it is classified as "Commercial Computer Software" under
 paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
 Regulations ("DFARS") (or any successor regulations) and the Government is
 acquiring only the license rights granted herein (the license rights
 customarily provided to non-Government users).  If the Software is supplied to
 any unit or agency of the Government other than DoD, it is classified as
 "Restricted Computer Software" and the Government's rights in the Software are
 defined in paragraph 52.227-19 of the Federal Acquisition Regulations ("FAR")
 (or any successor regulations) or, in the cases of NASA, in paragraph
 18.52.227-86 of the NASA Supplement to the FAR (or any successor regulations). <p>
 
 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See
 <a href="http://www.openss7.com/">http://www.openss7.com/</a> <p>
 
 Last Modified $Date$ by $Author$
 */

package javax.jain.ss7.inap.constants;

import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

/**
  * This class indicates the EventSpecificInformationBCSM choice.
  *
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class EventSpecificInformationBCSMChoice {
    /** Internal integer value: COLLECTED_INFO_SPECIFIC_INFO.  */
    public static final int M_COLLECTED_INFO_SPECIFIC_INFO = 0;
    /** Internal integer value: ANALYSED_INFO_SPECIFIC_INFO.  */
    public static final int M_ANALYSED_INFO_SPECIFIC_INFO = 1;
    /** Internal integer value: ROUTE_SELECT_FAILURE_SPECIFIC_INFO.  */
    public static final int M_ROUTE_SELECT_FAILURE_SPECIFIC_INFO = 2;
    /** Internal integer value: O_CALLED_PARTY_BUSY_SPECIFIC_INFO.  */
    public static final int M_O_CALLED_PARTY_BUSY_SPECIFIC_INFO = 3;   
    /** Internal integer value: O_NO_ANSWER_SPECIFIC_INFO.  */
    public static final int M_O_NO_ANSWER_SPECIFIC_INFO = 4;
    /** Internal integer value: O_ANSWER_SPECIFIC_INFO.  */
    public static final int M_O_ANSWER_SPECIFIC_INFO = 5;
    /** Internal integer value: O_MID_CALL_SPECIFIC_INFO.  */
    public static final int M_O_MID_CALL_SPECIFIC_INFO = 6;
    /** Internal integer value: O_DISCONNECT_SPECIFIC_INFO.  */
    public static final int M_O_DISCONNECT_SPECIFIC_INFO = 7;
    /** Internal integer value: T_BUSY_SPECIFIC_INFO.  */
    public static final int M_T_BUSY_SPECIFIC_INFO = 8;
    /** Internal integer value: T_NO_ANSWER_SPECIFIC_INFO.  */
    public static final int M_T_NO_ANSWER_SPECIFIC_INFO = 9;
    /** Internal integer value: T_ANSWER_SPECIFIC_INFO.  */
    public static final int M_T_ANSWER_SPECIFIC_INFO= 10;
    /** Internal integer value: T_MID_CALL_SPECIFIC_INFO.  */
    public static final int M_T_MID_CALL_SPECIFIC_INFO = 11;   
    /** Internal integer value: T_DISCONNECT_SPECIFIC_INFO.  */
    public static final int M_T_DISCONNECT_SPECIFIC_INFO = 12;
    /** Internal integer value: O_TERM_SEIZED_SPECIFIC_INFO.  */
    public static final int M_O_TERM_SEIZED_SPECIFIC_INFO = 13;
    /** Internal integer value: O_SUSPENDED.  */
    public static final int M_O_SUSPENDED = 14;
    /** Internal integer value: T_SUSPENDED.  */
    public static final int M_T_SUSPENDED = 15;
    /** Internal integer value: ORIG_ATTEMPT_AUTHORIZED.  */
    public static final int M_ORIG_ATTEMPT_AUTHORIZED = 16;
    /** Internal integer value: O_RE_ANSWER.  */
    public static final int M_O_RE_ANSWER = 17;
    /** Internal integer value: T_RE_ANSWER.  */
    public static final int M_T_RE_ANSWER = 18;
    /** Internal integer value: FACILITY_SELECTED_AND_AVAILABLE.  */
    public static final int M_FACILITY_SELECTED_AND_AVAILABLE = 19;   
    /** Internal integer value: CALL_ACCEPTED.  */
    public static final int M_CALL_ACCEPTED = 20;
    /** Internal integer value: O_ABANDON.  */
    public static final int M_O_ABANDON = 21;
    /** Internal integer value: T_ABANDON.  */
    public static final int M_T_ABANDON = 22;
    /** Private internal integer value of constant class. */
    private int eventSpecificInformationBCSMChoice;
    /** Private constructor for constant class.
      * @param eventSpecificInformationBCSMChoice
      * Internal integer value of the constant class. */
    private EventSpecificInformationBCSMChoice(int eventSpecificInformationBCSMChoice) {
        this.eventSpecificInformationBCSMChoice = eventSpecificInformationBCSMChoice;
    }
    /** EventSpecificInformationBCSMChoice  :COLLECTED_INFO_SPECIFIC_INFO */
    public static final EventSpecificInformationBCSMChoice COLLECTED_INFO_SPECIFIC_INFO
        = new EventSpecificInformationBCSMChoice(M_COLLECTED_INFO_SPECIFIC_INFO);
    /** EventSpecificInformationBCSMChoice  :ANALYSED_INFO_SPECIFIC_INFO */
    public static final EventSpecificInformationBCSMChoice ANALYSED_INFO_SPECIFIC_INFO
        = new EventSpecificInformationBCSMChoice(M_ANALYSED_INFO_SPECIFIC_INFO);
    /** EventSpecificInformationBCSMChoice  :ROUTE_SELECT_FAILURE_SPECIFIC_INFO */
    public static final EventSpecificInformationBCSMChoice ROUTE_SELECT_FAILURE_SPECIFIC_INFO
        = new EventSpecificInformationBCSMChoice(M_ROUTE_SELECT_FAILURE_SPECIFIC_INFO);
    /** EventSpecificInformationBCSMChoice  :O_CALLED_PARTY_BUSY_SPECIFIC_INFO */
    public static final EventSpecificInformationBCSMChoice O_CALLED_PARTY_BUSY_SPECIFIC_INFO
        = new EventSpecificInformationBCSMChoice(M_O_CALLED_PARTY_BUSY_SPECIFIC_INFO);
    /** EventSpecificInformationBCSMChoice  :O_NO_ANSWER_SPECIFIC_INFO */
    public static final EventSpecificInformationBCSMChoice O_NO_ANSWER_SPECIFIC_INFO
        = new EventSpecificInformationBCSMChoice(M_O_NO_ANSWER_SPECIFIC_INFO);
    /** EventSpecificInformationBCSMChoice  :O_ANSWER_SPECIFIC_INFO */
    public static final EventSpecificInformationBCSMChoice O_ANSWER_SPECIFIC_INFO
        = new EventSpecificInformationBCSMChoice(M_O_ANSWER_SPECIFIC_INFO);
    /** EventSpecificInformationBCSMChoice  :O_MID_CALL_SPECIFIC_INFO */
    public static final EventSpecificInformationBCSMChoice O_MID_CALL_SPECIFIC_INFO
        = new EventSpecificInformationBCSMChoice(M_O_MID_CALL_SPECIFIC_INFO);
    /** EventSpecificInformationBCSMChoice  :O_DISCONNECT_SPECIFIC_INFO */
    public static final EventSpecificInformationBCSMChoice O_DISCONNECT_SPECIFIC_INFO
        = new EventSpecificInformationBCSMChoice(M_O_DISCONNECT_SPECIFIC_INFO);
    /** EventSpecificInformationBCSMChoice  :T_BUSY_SPECIFIC_INFO */
    public static final EventSpecificInformationBCSMChoice T_BUSY_SPECIFIC_INFO
        = new EventSpecificInformationBCSMChoice(M_T_BUSY_SPECIFIC_INFO);
    /** EventSpecificInformationBCSMChoice  :T_NO_ANSWER_SPECIFIC_INFO */
    public static final EventSpecificInformationBCSMChoice T_NO_ANSWER_SPECIFIC_INFO
        = new EventSpecificInformationBCSMChoice(M_T_NO_ANSWER_SPECIFIC_INFO);
    /** EventSpecificInformationBCSMChoice  :T_ANSWER_SPECIFIC_INFO */
    public static final EventSpecificInformationBCSMChoice T_ANSWER_SPECIFIC_INFO
        = new EventSpecificInformationBCSMChoice(M_T_ANSWER_SPECIFIC_INFO);
    /** EventSpecificInformationBCSMChoice  :T_MID_CALL_SPECIFIC_INFO */
    public static final EventSpecificInformationBCSMChoice T_MID_CALL_SPECIFIC_INFO
        = new EventSpecificInformationBCSMChoice(M_T_MID_CALL_SPECIFIC_INFO);
    /** EventSpecificInformationBCSMChoice  :T_DISCONNECT_SPECIFIC_INFO */
    public static final EventSpecificInformationBCSMChoice T_DISCONNECT_SPECIFIC_INFO
        = new EventSpecificInformationBCSMChoice(M_T_DISCONNECT_SPECIFIC_INFO);
    /** EventSpecificInformationBCSMChoice  :O_TERM_SEIZED_SPECIFIC_INFO */
    public static final EventSpecificInformationBCSMChoice O_TERM_SEIZED_SPECIFIC_INFO
        = new EventSpecificInformationBCSMChoice(M_O_TERM_SEIZED_SPECIFIC_INFO);
    /** EventSpecificInformationBCSMChoice  :O_SUSPENDED */
    public static final EventSpecificInformationBCSMChoice O_SUSPENDED
        = new EventSpecificInformationBCSMChoice(M_O_SUSPENDED);
    /** EventSpecificInformationBCSMChoice  :T_SUSPENDED */
    public static final EventSpecificInformationBCSMChoice T_SUSPENDED
        = new EventSpecificInformationBCSMChoice(M_T_SUSPENDED);
    /** EventSpecificInformationBCSMChoice  :ORIG_ATTEMPT_AUTHORIZED */
    public static final EventSpecificInformationBCSMChoice ORIG_ATTEMPT_AUTHORIZED
        = new EventSpecificInformationBCSMChoice(M_ORIG_ATTEMPT_AUTHORIZED);
    /** EventSpecificInformationBCSMChoice  :O_RE_ANSWER */
    public static final EventSpecificInformationBCSMChoice O_RE_ANSWER
        = new EventSpecificInformationBCSMChoice(M_O_RE_ANSWER);
    /** EventSpecificInformationBCSMChoice  :T_RE_ANSWER */
    public static final EventSpecificInformationBCSMChoice T_RE_ANSWER
        = new EventSpecificInformationBCSMChoice(M_T_RE_ANSWER );
    /** EventSpecificInformationBCSMChoice  :FACILITY_SELECTED_AND_AVAILABLE */
    public static final EventSpecificInformationBCSMChoice FACILITY_SELECTED_AND_AVAILABLE
        = new EventSpecificInformationBCSMChoice(M_FACILITY_SELECTED_AND_AVAILABLE);
    /** EventSpecificInformationBCSMChoice  :CALL_ACCEPTED */
    public static final EventSpecificInformationBCSMChoice CALL_ACCEPTED
        = new EventSpecificInformationBCSMChoice(M_CALL_ACCEPTED);
    /** EventSpecificInformationBCSMChoice  :O_ABANDON */
    public static final EventSpecificInformationBCSMChoice O_ABANDON
        = new EventSpecificInformationBCSMChoice(M_O_ABANDON);
    /** EventSpecificInformationBCSMChoice  :T_ABANDON */
    public static final EventSpecificInformationBCSMChoice T_ABANDON
        = new EventSpecificInformationBCSMChoice(M_T_ABANDON);
    /**
      * Gets the integer String representation of the Constant class .
      * @return
      * Internal integer value of the constant class. */
    public int getEventSpecificInformationBCSMChoice() {
        return eventSpecificInformationBCSMChoice;
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
