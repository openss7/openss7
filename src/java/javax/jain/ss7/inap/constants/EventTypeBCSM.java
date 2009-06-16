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
  * This class indicates the BCSM detection point event.
  * @version 1.2.2
  * @author Monavacon Limited.
  */
public class EventTypeBCSM  {
    /** Internal integer value: ORIG_ATTEMPT_AUTHORIZED.  */
    public static final int M_ORIG_ATTEMPT_AUTHORIZED = 1;
    /** Internal integer value: COLLECTED_INFO.  */
    public static final int M_COLLECTED_INFO = 2;
    /** Internal integer value: ANALYSED_INFORMATION.  */
    public static final int M_ANALYSED_INFORMATION = 3;
    /** Internal integer value: ROUTE_SELECT_FAILURE.  */
    public static final int M_ROUTE_SELECT_FAILURE = 4;
    /** Internal integer value: O_CALLED_PARTY_BUSY.  */
    public static final int M_O_CALLED_PARTY_BUSY = 5;
    /** Internal integer value: O_NO_ANSWER.  */
    public static final int M_O_NO_ANSWER = 6;
    /** Internal integer value: O_ANSWER.  */
    public static final int M_O_ANSWER = 7;
    /** Internal integer value: O_MID_CALL.  */
    public static final int M_O_MID_CALL = 8;
    /** Internal integer value: O_DISCONNECT.  */
    public static final int M_O_DISCONNECT = 9;
    /** Internal integer value: O_ABANDON.  */
    public static final int M_O_ABANDON = 10;
    /** Internal integer value: TERM_ATTEMPT_AUTHORIZED.  */
    public static final int M_TERM_ATTEMPT_AUTHORIZED = 12;
    /** Internal integer value: T_BUSY.  */
    public static final int M_T_BUSY = 13;
    /** Internal integer value: T_NO_ANSWER.  */
    public static final int M_T_NO_ANSWER = 14;
    /** Internal integer value: T_ANSWER.  */
    public static final int M_T_ANSWER = 15;
    /** Internal integer value: T_MID_CALL.  */
    public static final int M_T_MID_CALL = 16;
    /** Internal integer value: T_DISCONNECT.  */
    public static final int M_T_DISCONNECT = 17;
    /** Internal integer value: T_ABANDON.  */
    public static final int M_T_ABANDON = 18;
    /** Internal integer value: O_TERM_SEIZED.  */
    public static final int M_O_TERM_SEIZED = 19;
    /** Internal integer value: O_SUSPENDED.  */
    public static final int M_O_SUSPENDED = 20;
    /** Internal integer value: T_SUSPENDED.  */
    public static final int M_T_SUSPENDED = 21;
    /** Internal integer value: ORIG_ATTEMPT.  */
    public static final int M_ORIG_ATTEMPT = 22;
    /** Internal integer value: TERM_ATTEMPT.  */
    public static final int M_TERM_ATTEMPT = 23;
    /** Internal integer value: O_RE_ANSWER.  */
    public static final int M_O_RE_ANSWER = 24;
    /** Internal integer value: T_RE_ANSWER.  */
    public static final int M_T_RE_ANSWER = 25;
    /** Internal integer value: FACILITY_SELECTED_AND_AVAILABLE.  */
    public static final int M_FACILITY_SELECTED_AND_AVAILABLE = 26;
    /** Internal integer value: CALL_ACCPETED.  */
    public static final int M_CALL_ACCPETED = 27;
    /** Private internal integer value of constant class.  */
    private int eventTypeBCSM;
    /** Private constructor for constant class.
      * @param eventTypeBCSM
      * Internal integer value of constant class.  */
    private EventTypeBCSM(int eventTypeBCSM) {
        this.eventTypeBCSM = eventTypeBCSM;
    }
    /** EventTypeBCSM : ORIG_ATTEMPT_AUTHORIZED */
    public static final EventTypeBCSM  ORIG_ATTEMPT_AUTHORIZED
        = new EventTypeBCSM(M_ORIG_ATTEMPT_AUTHORIZED);
    /** EventTypeBCSM :COLLECTED_INFO */
    public static final EventTypeBCSM  COLLECTED_INFO
        = new EventTypeBCSM(M_COLLECTED_INFO);
    /** EventTypeBCSM :ANALYSED_INFORMATION */
    public static final EventTypeBCSM ANALYSED_INFORMATION
        = new EventTypeBCSM(M_ANALYSED_INFORMATION);
    /** EventTypeBCSM :ROUTE_SELECT_FAILURE */
    public static final EventTypeBCSM  ROUTE_SELECT_FAILURE
        = new EventTypeBCSM(M_ROUTE_SELECT_FAILURE);
    /** EventTypeBCSM :O_CALLED_PARTY_BUSY */
    public static final EventTypeBCSM  O_CALLED_PARTY_BUSY
        = new EventTypeBCSM(M_O_CALLED_PARTY_BUSY);
    /** EventTypeBCSM :O_NO_ANSWER */
    public static final EventTypeBCSM  O_NO_ANSWER
        = new EventTypeBCSM(M_O_NO_ANSWER);
    /** EventTypeBCSM :O_ANSWER */
    public static final EventTypeBCSM  O_ANSWER
        = new EventTypeBCSM(M_O_ANSWER);
    /** EventTypeBCSM :O_MID_CALL */
    public static final EventTypeBCSM O_MID_CALL
        = new EventTypeBCSM(M_O_MID_CALL);
    /** EventTypeBCSM :O_DISCONNECT */
    public static final EventTypeBCSM O_DISCONNECT
        = new EventTypeBCSM(M_O_DISCONNECT);
    /** EventTypeBCSM :O_ABANDON */
    public static final EventTypeBCSM O_ABANDON
        = new EventTypeBCSM(M_O_ABANDON);
    /** EventTypeBCSM :TERM_ATTEMPT_AUTHORIZED */
    public static final EventTypeBCSM  TERM_ATTEMPT_AUTHORIZED
        = new EventTypeBCSM(M_TERM_ATTEMPT_AUTHORIZED);
    /** EventTypeBCSM :T_BUSY */
    public static final EventTypeBCSM  T_BUSY
        = new EventTypeBCSM(M_T_BUSY);
    /** EventTypeBCSM :T_NO_ANSWER */
    public static final EventTypeBCSM  T_NO_ANSWER
        = new EventTypeBCSM(M_T_NO_ANSWER);
    /** EventTypeBCSM :T_ANSWER */
    public static final EventTypeBCSM  T_ANSWER
        = new EventTypeBCSM(M_T_ANSWER);
    /** EventTypeBCSM :T_MID_CALL */
    public static final EventTypeBCSM T_MID_CALL
        = new EventTypeBCSM(M_T_MID_CALL);
    /** EventTypeBCSM :T_DISCONNECT */
    public static final EventTypeBCSM  T_DISCONNECT
        = new EventTypeBCSM(M_T_DISCONNECT);
    /** EventTypeBCSM :T_ABANDON */
    public static final EventTypeBCSM  T_ABANDON
        = new EventTypeBCSM(M_T_ABANDON);
    /** EventTypeBCSM :O_TERM_SEIZED */
    public static final EventTypeBCSM O_TERM_SEIZED
        = new EventTypeBCSM(M_O_TERM_SEIZED);
    /** EventTypeBCSM :O_SUSPENDED */
    public static final EventTypeBCSM  O_SUSPENDED
        = new EventTypeBCSM(M_O_SUSPENDED);
    /** EventTypeBCSM :T_SUSPENDED */
    public static final EventTypeBCSM  T_SUSPENDED
        = new EventTypeBCSM(M_T_SUSPENDED);
    /** EventTypeBCSM :ORIG_ATTEMPT */
    public static final EventTypeBCSM  ORIG_ATTEMPT
        = new EventTypeBCSM(M_ORIG_ATTEMPT);
    /** EventTypeBCSM :TERM_ATTEMPT */
    public static final EventTypeBCSM   TERM_ATTEMPT
        = new EventTypeBCSM(M_TERM_ATTEMPT);
    /** EventTypeBCSM :O_RE_ANSWER */
    public static final EventTypeBCSM  O_RE_ANSWER
        = new EventTypeBCSM(M_O_RE_ANSWER);
    /** EventTypeBCSM :T_RE_ANSWER */
    public static final EventTypeBCSM T_RE_ANSWER
        = new EventTypeBCSM(M_T_RE_ANSWER);
    /** EventTypeBCSM :FACILITY_SELECTED_AND_AVAILABLE */
    public static final EventTypeBCSM  FACILITY_SELECTED_AND_AVAILABLE
        = new EventTypeBCSM(M_FACILITY_SELECTED_AND_AVAILABLE);
    /** EventTypeBCSM :CALL_ACCPETED */
    public static final EventTypeBCSM CALL_ACCPETED
        = new EventTypeBCSM(M_CALL_ACCPETED);
    /** Gets the integer value representation of the Constant class.
      * @return
      * Internal integer value of constant class.  */
    public int getEventTypeBCSM() {
        return eventTypeBCSM;
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
