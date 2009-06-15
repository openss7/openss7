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

package jain.protocol.ss7.tcap.dialogue;

import jain.protocol.ss7.tcap.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * This class contains the common Constants used throughout the package
  * jain.protocol.ss7.tcap.dialogue
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class DialogueConstants extends java.lang.Object {
    /** Termination constant: Indicates a basic dialogue termination
      * ending, this constant has an integer value of 1.  */
    public static final int TC_BASIC_END = 1;
    /** Trmination Constant: Indicates a pre-arranged dialogue
      * termination ending, this constant has an integer value of 2.  */
    public static final int TC_PRE_ARRANGED_END = 2;
    /** Abort Reason Constant: Indicates that the application context is
      * not supported, this constant has an integer value of 1.  */
    public static final int ABORT_REASON_ACN_NOT_SUPPORTED = 1;
    /** Abort Reason Constant: Indicates that the abort reason is user
      * specific, this constant has an integer value of 2.  */
    public static final int ABORT_REASON_USER_SPECIFIC = 2;
    /** Provider Abort Constant: Unrecognised Message
      * Type(ITU)/Package(ANSI), this constant has an integer value of
      * 0.  */
    public static final int P_ABORT_UNRCGNZ_MSG_TYPE = 0;
    /** Provider Abort Constant: Unrecognized(ITU)/Unassigned
      * Responding(ANSI) Transaction ID, this constant has an integer
      * value of 1.  */
    public static final int P_ABORT_UNRECOGNIZED_TRANSACTION_ID = 1;
    /** Provider Abort Constant: Badly Formatted(ITU)/Badly
      * Structured(ANSI) Transaction Portion, this constant has an
      * integer value of 2.  */
    public static final int P_ABORT_BADLY_FORMATTED_TRANSACTION_PORTION = 2;
    /** Provider Abort Constant: Incorrect Transaction Portion, this
      * constant has an integer value of 3.  */
    public static final int P_ABORT_INCORRECT_TRANSACTION_PORTION = 3;
    /** Provider Abort Constant: Resource
      * Limitation(ITU)/Unavailable(ANSI), this constant has an integer
      * value of 4.  */
    public static final int P_ABORT_RESOURCE_LIMIT = 4;
    /** Provider Abort Constant: Abnormal dialogue, this constant has an
      * integer value of 5.  */
    public static final int P_ABORT_ABNORMAL_DIALOGUE = 5;
    /** Provider Abort Constant: Unrecognized Dialogue Portion ID(ANSI),
      * this constant has an integer value of 6.  */
    public static final int P_ABORT_UNRECOG_DIALOGUE_PORTION_ID = 6;
    /** Provider Abort Constant: Badly Structured Dialogue
      * Portion(ANSI), this constant has an integer value of 7.  */
    public static final int P_ABORT_BADLY_STRUCTURED_DIALOGUE_PORTION = 7;
    /** Provider Abort Constant: Missing Dialogue Portion(ANSI), this
      * constant has an integer value of 8.  */
    public static final int P_ABORT_MISSING_DIALOGUE_PORTION = 8;
    /** Provider Abort Constant: Inconsistent Dialogue Portion(ANSI),
      * this constant has an integer value of 9.  */
    public static final int P_ABORT_INCONSISTENT_DIALOGUE_PORTION = 9;
    /** Provider Abort Constant: Permission to Release Problem(ANSI),
      * this constant has an integer value of 10.  */
    public static final int P_ABORT_PERMISSION_TO_RELEASE_PROBLEM = 10;
    /** @deprecated
      * As of JAIN TCAP v1.1, use the STACK_SPECIFICATION_ANSI_92 constant.  */
    public static final int PROTOCOL_VERSION_ANSI_92 = 1;
    /** @deprecated
      * As of JAIN TCAP v1.1, use the STACK_SPECIFICATION_ANSI_96 constant.  */
    public static final int PROTOCOL_VERSION_ANSI_96 = 2;
    /** @deprecated
      * As of JAIN TCAP v1.1, use the STACK_SPECIFICATION_ITU_93 constant.  */
    public static final int PROTOCOL_VERSION_ITU_93 = 3;
    /** @deprecated
      * As of JAIN TCAP v1.1, use the STACK_SPECIFICATION_ITU_97 constant.  */
    public static final int PROTOCOL_VERSION_ITU_97 = 4;
    /** Dialogue Portion Protocol Version Constant: ANSI 1996 Dialogue
      * Portion, specific to an ANSI 1996 implementation of the JAIN
      * TCAP API. This constant has an integer value of 1.
      * @since JAIN TCAP v1.1 */
    public static final int DP_PROTOCOL_VERSION_ANSI_96 = 1;
    /** Application Context Identifier Constant: Integer, this constant
      * has an integer value of 1 */
    public static final int APPLICATION_CONTEXT_INTEGER = 1;
    /** Application Context Identifier Constant: Object, this constant
      * has an integer value of 2 */
    public static final int APPLICATION_CONTEXT_OBJECT = 2;
    /** Security Context Identifier Constant: Integer, this constant has
      * an integer value of 1 */
    public static final int SECURITY_CONTEXT_INTEGER = 1;
    /** Security Context Identifier Constant: Object, this constant has
      * an integer value of 2 */
    public static final int SECURITY_CONTEXT_OBJECT = 2;
    /** Private onstructor for the DialogueConstants object.  */
    private DialogueConstants() {
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
