/*
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Copyrights:
 *
 * Copyright - 1999 Sun Microsystems, Inc. All rights reserved.
 * 901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 * This product and related documentation are protected by copyright and
 * distributed under licenses restricting its use, copying, distribution, and
 * decompilation. No part of this product or related documentation may be
 * reproduced in any form by any means without prior written authorization of
 * Sun and its licensors, if any.
 *
 * RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 * States Government is subject to the restrictions set forth in DFARS
 * 252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 * The product described in this manual may be protected by one or more U.S.
 * patents, foreign patents, or pending applications.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Author:
 *
 * AePONA Limited, Interpoint Building
 * 20-24 York Street, Belfast BT15 1AQ
 * N. Ireland.
 *
 * Module Name   : JAIN TCAP API
 * File Name     : DialoguePortion.java
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     16/10/2000  Phelim O'Doherty    Deprecated invalid protocol version
 *                                         constant and added pAbort, application
 *                                         DP protocol version and security id
 *                                         constants. Made constructor private.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

package jain.protocol.ss7.tcap.dialogue;

// import jain.protocol.ss7.tcap.*;

/**
 *  This class contains the common Constants used throughout the package
 *  jain.protocol.ss7.tcap.dialogue
 *
 *@author     Sun Microsystems Inc.
 *@version    1.1
 */

public class DialogueConstants {

	/**
	 *  Constructor for the DialogueConstants object
	 */
	private DialogueConstants() {
	}


        private final static int TERM = 0;

	/**
	 *  Termination constant: Indicates a basic dialogue termination ending, this
	 *  constant has an integer value of 1.
	 */

	public final static int TC_BASIC_END = TERM | 1;

	/**
	 *  Trmination Constant: Indicates a pre-arranged dialogue termination ending,
	 *  this constant has an integer value of 2.
	 */

	public final static int TC_PRE_ARRANGED_END = TERM | 2;


	private final static int ABORT_REASON = 0;

	/**
	 *  Abort Reason Constant: Indicates that the application context is not
	 *  supported, this constant has an integer value of 1.
	 */

	public final static int ABORT_REASON_ACN_NOT_SUPPORTED = ABORT_REASON | 1;

	/**
	 *  Abort Reason Constant: Indicates that the abort reason is user specific,
	 *  this constant has an integer value of 2.
	 */

	public final static int ABORT_REASON_USER_SPECIFIC = ABORT_REASON | 2;



	private final static int P_ABORT = 0;

	/**
	 *  Provider Abort Constant: Unrecognised Message Type(ITU)/Package(ANSI), this
	 *  constant has an integer value of 0.
	 */

	public final static int P_ABORT_UNRCGNZ_MSG_TYPE = P_ABORT;

	/**
	 *  Provider Abort Constant: Unrecognized(ITU)/Unassigned Responding(ANSI)
	 *  Transaction ID, this constant has an integer value of 1.
	 */

	public final static int P_ABORT_UNRECOGNIZED_TRANSACTION_ID = P_ABORT | 1;

	/**
	 *  Provider Abort Constant: Badly Formatted(ITU)/Badly Structured(ANSI)
	 *  Transaction Portion, this constant has an integer value of 2.
	 */

	public final static int P_ABORT_BADLY_FORMATTED_TRANSACTION_PORTION = P_ABORT | 2;

	/**
	 *  Provider Abort Constant: Incorrect Transaction Portion, this constant has
	 *  an integer value of 3.
	 */

	public final static int P_ABORT_INCORRECT_TRANSACTION_PORTION = P_ABORT | 3;

	/**
	 *  Provider Abort Constant: Resource Limitation(ITU)/Unavailable(ANSI), this
	 *  constant has an integer value of 4.
	 */

	public final static int P_ABORT_RESOURCE_LIMIT = P_ABORT | 4;

	/**
	 *  Provider Abort Constant: Abnormal dialogue, this constant has an integer
	 *  value of 5.
	 */

	public final static int P_ABORT_ABNORMAL_DIALOGUE = P_ABORT | 5;

	/**
	 *  Provider Abort Constant: Unrecognized Dialogue Portion ID(ANSI), this
	 *  constant has an integer value of 6.
	 */

	public final static int P_ABORT_UNRECOG_DIALOGUE_PORTION_ID = P_ABORT | 6;

	/**
	 *  Provider Abort Constant: Badly Structured Dialogue Portion(ANSI), this
	 *  constant has an integer value of 7.
	 */

	public final static int P_ABORT_BADLY_STRUCTURED_DIALOGUE_PORTION = P_ABORT | 7;

	/**
	 *  Provider Abort Constant: Missing Dialogue Portion(ANSI), this constant has
	 *  an integer value of 8.
	 */

	public final static int P_ABORT_MISSING_DIALOGUE_PORTION = P_ABORT | 8;

	/**
	 *  Provider Abort Constant: Inconsistent Dialogue Portion(ANSI), this constant
	 *  has an integer value of 9.
	 */

	public final static int P_ABORT_INCONSISTENT_DIALOGUE_PORTION = P_ABORT | 9;

	/**
	 *  Provider Abort Constant: Permission to Release Problem(ANSI), this constant
	 *  has an integer value of 10.
	 */

	public final static int P_ABORT_PERMISSION_TO_RELEASE_PROBLEM = P_ABORT | 10;



// POD - Update v1.1 - deprecated field
	private final static int PROTOCOL_VERSION = 0;

	/**
	 *@deprecated    - As of JAIN TCAP v1.1, use the <a href =
         *          "jain/protocol/ss7/tcap/TcapConstants.html#STACK_SPECIFICATION_ANSI_92"> STACK_SPECIFICATION_ANSI_92</a> constant.
	 */
	public final static int PROTOCOL_VERSION_ANSI_92 = PROTOCOL_VERSION | 1;

	/**
	 *@deprecated    - As of JAIN TCAP v1.1, use the <a href =
         *              "jain/protocol/ss7/tcap/TcapConstants.html#STACK_SPECIFICATION_ANSI_96"> STACK_SPECIFICATION_ANSI_96</a> constant.
	 */
	public final static int PROTOCOL_VERSION_ANSI_96 = PROTOCOL_VERSION | 2;

	/**
	 *@deprecated    - As of JAIN TCAP v1.1, use the <a href =
         *                 "jain/protocol/ss7/tcap/TcapConstants.html#STACK_SPECIFICATION_ITU_93"> STACK_SPECIFICATION_ITU_93</a> constant.
	 */
	public final static int PROTOCOL_VERSION_ITU_93 = PROTOCOL_VERSION | 3;

	/**
	 *@deprecated    - As of JAIN TCAP v1.1, use the <a href =
         *                "jain/protocol/ss7/tcap/TcapConstants.html#STACK_SPECIFICATION_ITU_97"> STACK_SPECIFICATION_ITU_97</a> constant.
	 */
	public final static int PROTOCOL_VERSION_ITU_97 = PROTOCOL_VERSION | 4;



// POD - Update v1.1 - Added new constant for Dialogue Portion Protocol version
	private final static int DP_PROTOCOL_VERSION = 0;

	/**
	 *  Dialogue Portion Protocol Version Constant: ANSI 1996 Dialogue Portion,
	 *  specific to an ANSI 1996 implementation of the JAIN TCAP API. This constant
	 *  has an integer value of 1.
	 *
	 *@since    JAIN TCAP v1.1
	 */
	public final static int DP_PROTOCOL_VERSION_ANSI_96 = DP_PROTOCOL_VERSION | 1;



// POD - Update v1.1 - Added new constant for Dialogue Portion Application
// Context Identifier
	private final static int APPLICATION_CONTEXT_IDENTIFIER = 0;


	/**
	 *  Application Context Identifier Constant: Integer, this constant has an
	 *  integer value of 1
	 */

	public final static int APPLICATION_CONTEXT_INTEGER = APPLICATION_CONTEXT_IDENTIFIER | 1;

	/**
	 *  Application Context Identifier Constant: Object, this constant has an
	 *  integer value of 2
	 */
	public final static int APPLICATION_CONTEXT_OBJECT = APPLICATION_CONTEXT_IDENTIFIER | 2;


// POD - Update v1.1 - Added new constant for Dialogue Portion Security Context
// Identifier
	private final static int SECURITY_CONTEXT_IDENTIFIER = 0;

	/**
	 *  Security Context Identifier Constant: Integer, this constant has an integer
	 *  value of 1
	 */

	public final static int SECURITY_CONTEXT_INTEGER = SECURITY_CONTEXT_IDENTIFIER | 1;

	/**
	 *  Security Context Identifier Constant: Object, this constant has an integer
	 *  value of 2
	 */
	public final static int SECURITY_CONTEXT_OBJECT = SECURITY_CONTEXT_IDENTIFIER | 2;


}


