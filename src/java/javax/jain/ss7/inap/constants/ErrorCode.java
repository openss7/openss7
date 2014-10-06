/*
 @(#) $RCSfile: ErrorCode.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:47 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:47 $ by $Author: brian $
 */

package javax.jain.ss7.inap.constants;

import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

/**
  * This class specifies the Return Error Code. 
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class ErrorCode   {
    /** Internal integer value: CANCELLED.  */
    public static final int M_CANCELLED = 0;
    /** Internal integer value: CANCEL_FAILED.  */
    public static final int M_CANCEL_FAILED = 1;
    /** Internal integer value: ETC_FAILED.  */
    public static final int M_ETC_FAILED = 3;
    /** Internal integer value: IMPROPER_CALLER_RESPONSE.  */
    public static final int M_IMPROPER_CALLER_RESPONSE = 4;
    /** Internal integer value: MISSING_CUSTOMER_RECORD.  */
    public static final int M_MISSING_CUSTOMER_RECORD = 6;
    /** Internal integer value: MISSING_PARAMETER.  */
    public static final int M_MISSING_PARAMETER = 7;
    /** Internal integer value: PARAMETER_OUT_OF_RANGE.  */
    public static final int M_PARAMETER_OUT_OF_RANGE = 8;
    /** Internal integer value: REQUESTED_INFO_ERROR.  */
    public static final int M_REQUESTED_INFO_ERROR = 10;
    /** Internal integer value: SYSTEM_FAILURE.  */
    public static final int M_SYSTEM_FAILURE = 11;
    /** Internal integer value: TASK_REFUSED.  */
    public static final int M_TASK_REFUSED = 12;
    /** Internal integer value: UNAVAILABLE_RESOURCE.  */
    public static final int M_UNAVAILABLE_RESOURCE = 13;
    /** Internal integer value: UNEXPECTED_COMPONENT_SEQUENCE.  */
    public static final int M_UNEXPECTED_COMPONENT_SEQUENCE = 14;
    /** Internal integer value: UNEXPECTED_DATA_VALUE.  */
    public static final int M_UNEXPECTED_DATA_VALUE = 15;
    /** Internal integer value: UNEXPECTED_PARAMETER.  */
    public static final int M_UNEXPECTED_PARAMETER = 16;
    /** Internal integer value: UNKNOWN_LEGID.  */
    public static final int M_UNKNOWN_LEGID = 17;
    /** Internal integer value: UNKNOWN_RESOURCE.  */
    public static final int M_UNKNOWN_RESOURCE = 18;
    /** Internal integer value: SCF_REFERRAL.  */
    public static final int M_SCF_REFERRAL = 21;
    /** Internal integer value: SCF_TASK_REFUSED.  */
    public static final int M_SCF_TASK_REFUSED = 22;
    /** Internal integer value: CHAINING_REFUSED.  */
    public static final int M_CHAINING_REFUSED = 23;
    /** Private internal integer value of constant class. */
    private int errorCode;
    /** Private constructor for constant class.
      * @param errorCode
      * Internal integer value of constant class.  */
    private ErrorCode(int errorCode) {
        this.errorCode = errorCode;
    }
    /** ErrorCode :CANCELLED */
    public static final ErrorCode CANCELLED
        = new ErrorCode(M_CANCELLED);
    /** ErrorCode :CANCEL_FAILED */
    public static final ErrorCode CANCEL_FAILED
        = new ErrorCode(M_CANCEL_FAILED);
    /** ErrorCode :ETC_FAILED */
    public static final ErrorCode ETC_FAILED
        = new ErrorCode(M_ETC_FAILED);
    /** ErrorCode :IMPROPER_CALLER_RESPONSE */
    public static final ErrorCode IMPROPER_CALLER_RESPONSE
        = new ErrorCode(M_IMPROPER_CALLER_RESPONSE);
    /** ErrorCode :MISSING_CUSTOMER_RECORD */
    public static final ErrorCode MISSING_CUSTOMER_RECORD
        = new ErrorCode(M_MISSING_CUSTOMER_RECORD);
    /** ErrorCode :MISSING_PARAMETER  */
    public static final ErrorCode MISSING_PARAMETER
        = new ErrorCode(M_MISSING_PARAMETER);
    /** ErrorCode :PARAMETER_OUT_OF_RANGE */
    public static final ErrorCode PARAMETER_OUT_OF_RANGE
        = new ErrorCode(M_PARAMETER_OUT_OF_RANGE);
    /** ErrorCode :REQUESTED_INFO_ERROR */
    public static final ErrorCode REQUESTED_INFO_ERROR
        = new ErrorCode(M_REQUESTED_INFO_ERROR);
    /** ErrorCode :SYSTEM_FAILURE */
    public static final ErrorCode SYSTEM_FAILURE
        = new ErrorCode(M_SYSTEM_FAILURE);
    /** ErrorCode :TASK_REFUSED */
    public static final ErrorCode TASK_REFUSED
        = new ErrorCode(M_TASK_REFUSED);
    /** ErrorCode :UNAVAILABLE_RESOURCE */
    public static final ErrorCode UNAVAILABLE_RESOURCE
        = new ErrorCode(M_UNAVAILABLE_RESOURCE);
    /** ErrorCode :UNEXPECTED_COMPONENT_SEQUENCE */
    public static final ErrorCode UNEXPECTED_COMPONENT_SEQUENCE
        = new ErrorCode(M_UNEXPECTED_COMPONENT_SEQUENCE);
    /** ErrorCode :UNEXPECTED_DATA_VALUE */
    public static final ErrorCode UNEXPECTED_DATA_VALUE
        = new ErrorCode(M_UNEXPECTED_DATA_VALUE);
    /** ErrorCode :UNEXPECTED_PARAMETER */
    public static final ErrorCode UNEXPECTED_PARAMETER
        = new ErrorCode(M_UNEXPECTED_PARAMETER);
    /** ErrorCode :UNKNOWN_LEGID */
    public static final ErrorCode UNKNOWN_LEGID
        = new ErrorCode(M_UNKNOWN_LEGID);
    /** ErrorCode :UNKNOWN_RESOURCE */
    public static final ErrorCode UNKNOWN_RESOURCE
        = new ErrorCode(M_UNKNOWN_RESOURCE);
    /** ErrorCode :SCF_REFERRAL */
    public static final ErrorCode SCF_REFERRAL
        = new ErrorCode(M_SCF_REFERRAL);
    /** ErrorCode :SCF_TASK_REFUSED */
    public static final ErrorCode SCF_TASK_REFUSED
        = new ErrorCode(M_SCF_TASK_REFUSED);
    /** ErrorCode :CHAINING_REFUSED */
    public static final ErrorCode CHAINING_REFUSED
        = new ErrorCode(M_CHAINING_REFUSED);
    /** Gets the integer value representation of the Constant class.
      * @return
      * Internal integer value of constant class.  */
    public int getErrorCode() {
        return errorCode;
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
