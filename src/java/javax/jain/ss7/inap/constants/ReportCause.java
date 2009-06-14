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

/** This class defines the ReportCause Class: (encoded as in Q.713).
  *
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class ReportCause {
    /** Internal integer value: NO_TRANSLATION_FOR_AN_ADDRESS_OF_SUCH_NATURE.  */
    public static final int M_NO_TRANSLATION_FOR_AN_ADDRESS_OF_SUCH_NATURE = 0;
    /** Internal integer value: NO_TRANSLATION_FOR_THIS_SPECIFIC_ADDRESS.  */
    public static final int M_NO_TRANSLATION_FOR_THIS_SPECIFIC_ADDRESS = 1;
    /** Internal integer value: SUBSYSTEM_CONGESTION.  */
    public static final int M_SUBSYSTEM_CONGESTION = 2;
    /** Internal integer value: SUBSYSTEM_FAILURE.  */
    public static final int M_SUBSYSTEM_FAILURE = 3;
    /** Internal integer value: UNEQUIPPED_USER.  */
    public static final int M_UNEQUIPPED_USER = 4;
    /** Internal integer value: MTP_FAILURE.  */
    public static final int M_MTP_FAILURE = 5;
    /** Internal integer value: NETWORK_CONGESTION.  */
    public static final int M_NETWORK_CONGESTION = 6;
    /** Internal integer value: SCCP_UNQUALIFIED.  */
    public static final int M_SCCP_UNQUALIFIED = 7;
    /** Internal integer value: ERROR_IN_MESSAGE_TRANSPORT.  */
    public static final int M_ERROR_IN_MESSAGE_TRANSPORT = 8;
    /** Internal integer value: ERROR_IN_LOCAL_PROCESSING.  */
    public static final int M_ERROR_IN_LOCAL_PROCESSING = 9;
    /** Internal integer value: DESTINATION_CAN_NOT_PERFORM_REASSEMBLY.  */
    public static final int M_DESTINATION_CAN_NOT_PERFORM_REASSEMBLY = 10;
    /** Internal integer value: SCCP_FAILURE.  */
    public static final int M_SCCP_FAILURE = 11;
    /** Internal integer value: HOPCOUNTER_VIOLATION.  */
    public static final int M_HOPCOUNTER_VIOLATION = 12;
    /** Internal integer value: SEGMENTATION_NOT_SUPPORTED.  */
    public static final int M_SEGMENTATION_NOT_SUPPORTED = 13;
    /** Internal integer value: SEGMENTATION_FAILED.  */
    public static final int M_SEGMENTATION_FAILED = 14;
    /** Private internal integer value of constant class.  */
    private int reportCause;
    /** Private constructor for constant class.
      * @param reportCause
      * Internal integer value of constant class.  */
    private ReportCause(int reportCause) {
        this.reportCause = reportCause;
    }
    /** TCNotice :NO_TRANSLATION_FOR_AN_ADDRESS_OF_SUCH_NATURE */
    public static final ReportCause NO_TRANSLATION_FOR_AN_ADDRESS_OF_SUCH_NATURE
        = new ReportCause(M_NO_TRANSLATION_FOR_AN_ADDRESS_OF_SUCH_NATURE);
    /** TCNotice :NO_TRANSLATION_FOR_THIS_SPECIFIC_ADDRESS */
    public static final ReportCause NO_TRANSLATION_FOR_THIS_SPECIFIC_ADDRESS
        = new ReportCause(M_NO_TRANSLATION_FOR_THIS_SPECIFIC_ADDRESS);
    /** TCNotice :SUBSYSTEM_CONGESTION */
    public static final ReportCause SUBSYSTEM_CONGESTION
        = new ReportCause(M_SUBSYSTEM_CONGESTION);
    /** TCNotice :SUBSYSTEM_FAILURE */
    public static final ReportCause SUBSYSTEM_FAILURE
        = new ReportCause(M_SUBSYSTEM_FAILURE);
    /** TCNotice :UNEQUIPPED_USER */
    public static final ReportCause UNEQUIPPED_USER
        = new ReportCause(M_UNEQUIPPED_USER);
    /** TCNotice :MTP_FAILURE */
    public static final ReportCause MTP_FAILURE
        = new ReportCause(M_MTP_FAILURE);
    /** TCNotice :NETWORK_CONGESTION */
    public static final ReportCause NETWORK_CONGESTION
        = new ReportCause(M_NETWORK_CONGESTION);
    /** TCNotice :SCCP_UNQUALIFIED */
    public static final ReportCause SCCP_UNQUALIFIED
        = new ReportCause(M_SCCP_UNQUALIFIED);
    /** TCNotice :ERROR_IN_MESSAGE_TRANSPORT */
    public static final ReportCause ERROR_IN_MESSAGE_TRANSPORT
        = new ReportCause(M_ERROR_IN_MESSAGE_TRANSPORT);
    /** TCNotice :ERROR_IN_LOCAL_PROCESSING */
    public static final ReportCause ERROR_IN_LOCAL_PROCESSING
        = new ReportCause(M_ERROR_IN_LOCAL_PROCESSING);
    /** TCNotice :DESTINATION_CAN_NOT_PERFORM_REASSEMBLY */
    public static final ReportCause DESTINATION_CAN_NOT_PERFORM_REASSEMBLY
        = new ReportCause(M_DESTINATION_CAN_NOT_PERFORM_REASSEMBLY);
    /** TCNotice :SCCP_FAILURE */
    public static final ReportCause SCCP_FAILURE
        = new ReportCause(M_SCCP_FAILURE);
    /** TCNotice :HOPCOUNTER_VIOLATION */
    public static final ReportCause HOPCOUNTER_VIOLATION
        = new ReportCause(M_HOPCOUNTER_VIOLATION);
    /** TCNotice :SEGMENTATION_NOT_SUPPORTED */
    public static final ReportCause SEGMENTATION_NOT_SUPPORTED
        = new ReportCause(M_SEGMENTATION_NOT_SUPPORTED);
    /** TCNotice :SEGMENTATION_FAILED */
    public static final ReportCause SEGMENTATION_FAILED
        = new ReportCause(M_SEGMENTATION_FAILED);
    /** Gets the integer String representation of the Constant class.
      * @return
      * Internal integer value of constant class.  */
    public int getReportCause () {
        return reportCause;
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
