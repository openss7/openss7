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
  * This class indicates the type of trigger which caused call suspension.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class TriggerType  {
    /** Internal integer value: FEATURE_ACTIVATION.  */
    public static final int M_FEATURE_ACTIVATION = 0;
    /** Internal integer value: VERTICAL_SERVICE_CODE.  */
    public static final int M_VERTICAL_SERVICE_CODE = 1;
    /** Internal integer value: CUSTOMIZED_ACCESS.  */
    public static final int M_CUSTOMIZED_ACCESS = 2;
    /** Internal integer value: CUSTOMIZED_INTERCOM.  */
    public static final int M_CUSTOMIZED_INTERCOM = 3;
    /** Internal integer value: EMERGENCY_SERVICE.  */
    public static final int M_EMERGENCY_SERVICE = 12;
    /** Internal integer value: AFR.  */
    public static final int M_AFR = 13;
    /** Internal integer value: SHARED_IO_TRUNK.  */
    public static final int M_SHARED_IO_TRUNK = 14;
    /** Internal integer value: OFF_HOOK_DELAY.  */
    public static final int M_OFF_HOOK_DELAY = 17;
    /** Internal integer value: CHANNEL_SETUP_PRI.  */
    public static final int M_CHANNEL_SETUP_PRI = 18;
    /** Internal integer value: T_NO_ANSWER.  */
    public static final int M_T_NO_ANSWER = 25;
    /** Internal integer value: T_BUSY.  */
    public static final int M_T_BUSY = 26;
    /** Internal integer value: O_CALLED_PARTY_BUSY.  */
    public static final int M_O_CALLED_PARTY_BUSY = 27;
    /** Internal integer value: O_NO_ANSWER.  */
    public static final int M_O_NO_ANSWER = 29;
    /** Internal integer value: ORIGINATION_ATTEMPT_AUTHORIZED.  */
    public static final int M_ORIGINATION_ATTEMPT_AUTHORIZED = 30;
    /** Internal integer value: O_ANSWER.  */
    public static final int M_O_ANSWER = 31;
    /** Internal integer value: O_DISCONNECT.  */
    public static final int M_O_DISCONNECT = 32;
    /** Internal integer value: TERM_ATTEMPT_AUTHORIZED.  */
    public static final int M_TERM_ATTEMPT_AUTHORIZED = 33;
    /** Internal integer value: T_ANSWER.  */
    public static final int M_T_ANSWER = 34;
    /** Internal integer value: T_DISCONNECT.  */
    public static final int M_T_DISCONNECT = 35;
    /** Private internal integer value of constant class.  */
    private int triggerType;
    /** Private constructor for constant class.
      * @param triggerType
      * Internal integer value of constant class.  */
    private TriggerType(int triggerType) {
        this.triggerType = triggerType;
    }
    /** TriggerType  :FEATURE_ACTIVATION */
    public static final TriggerType FEATURE_ACTIVATION
        = new TriggerType(M_FEATURE_ACTIVATION);
    /** TriggerType  :VERTICAL_SERVICE_CODE */
    public static final TriggerType VERTICAL_SERVICE_CODE
        = new TriggerType(M_VERTICAL_SERVICE_CODE);
    /** TriggerType  :CUSTOMIZED_ACCESS */
    public static final TriggerType CUSTOMIZED_ACCESS
        = new TriggerType(M_CUSTOMIZED_ACCESS);
    /** TriggerType  :CUSTOMIZED_INTERCOM */
    public static final TriggerType CUSTOMIZED_INTERCOM
        = new TriggerType(M_CUSTOMIZED_INTERCOM);
    /** TriggerType  :EMERGENCY_SERVICE */
    public static final TriggerType EMERGENCY_SERVICE
        = new TriggerType(M_EMERGENCY_SERVICE);
    /** TriggerType  :AFR */
    public static final TriggerType AFR
        = new TriggerType(M_AFR);
    /** TriggerType  :SHARED_IO_TRUNK */
    public static final TriggerType SHARED_IO_TRUNK
        = new TriggerType(M_SHARED_IO_TRUNK);
    /** TriggerType  :OFF_HOOK_DELAY */
    public static final TriggerType OFF_HOOK_DELAY
        = new TriggerType(M_OFF_HOOK_DELAY);
    /** TriggerType  :CHANNEL_SETUP_PRI */
    public static final TriggerType CHANNEL_SETUP_PRI
        = new TriggerType(M_CHANNEL_SETUP_PRI);
    /** TriggerType  :T_NO_ANSWER */
    public static final TriggerType T_NO_ANSWER
        = new TriggerType(M_T_NO_ANSWER);
    /** TriggerType  :T_BUSY */
    public static final TriggerType T_BUSY
        = new TriggerType(M_T_BUSY);
    /** TriggerType  :O_CALLED_PARTY_BUSY */
    public static final TriggerType O_CALLED_PARTY_BUSY
        = new TriggerType(M_O_CALLED_PARTY_BUSY);
    /** TriggerType  :O_NO_ANSWER */
    public static final TriggerType O_NO_ANSWER
        = new TriggerType(M_O_NO_ANSWER);
    /** TriggerType  :ORIGINATION_ATTEMPT_AUTHORIZED */
    public static final TriggerType ORIGINATION_ATTEMPT_AUTHORIZED
        = new TriggerType(M_ORIGINATION_ATTEMPT_AUTHORIZED);
    /** TriggerType  :O_ANSWER */
    public static final TriggerType O_ANSWER
        = new TriggerType(M_O_ANSWER);
    /** TriggerType  :O_DISCONNECT */
    public static final TriggerType O_DISCONNECT
        = new TriggerType(M_O_DISCONNECT);
    /** TriggerType  :TERM_ATTEMPT_AUTHORIZED */
    public static final TriggerType TERM_ATTEMPT_AUTHORIZED
        = new TriggerType(M_TERM_ATTEMPT_AUTHORIZED);
    /** TriggerType  :T_ANSWER */
    public static final TriggerType T_ANSWER
        = new TriggerType(M_T_ANSWER);
    /** TriggerType  :T_DISCONNECT */
    public static final TriggerType T_DISCONNECT
        = new TriggerType(M_T_DISCONNECT);
    /** Gets the integer value representation of the Constant class.
      * @return
      * Internal integer value of constant class.  */
    public int getTriggerType() {
        return triggerType;
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
