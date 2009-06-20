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

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** An ISUP EVENT: The MaintenanceTimeoutIndEvent class is a sub class of the
  * IsupEvent class and is used to indicate any maintenance timeouts occuring locally
  * at the ISUP Stack causing a maintenance activity to the network.
  * The reason for maintenance timeout is included in the event.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class MaintenanceTimeoutIndEvent extends IsupEvent {
    /** maintenance timeout reason - Waiting for Blocking Acknowledgement - BLO Timer expiry (Timer T13) */
    public static final int ISUP_MTR_BLOCKING_TIMEOUT = 13;
    /** maintenance timeout reason - Waiting for Unblocking Acknowledgement - UBL Timer expiry (Timer T15) */
    public static final int ISUP_MTR_UNBLOCKING_TIMEOUT = 15;
    /** maintenance timeout reason - Waiting for Release Response - Release Timer Expiry (Timer T5) */
    public static final int ISUP_MTR_RELEASE_TIMEOUT = 1;
    /** maintenance timeout reason - Waiting for Release Response after sending Reset Circuit - RSC Timer Expiry (Timer T17) */
    public static final int ISUP_MTR_RESET_CIRCUIT_TIMEOUT = 17;
    /** maintenance timeout reason - Waiting for Group Blocking Acknowledgement - CGB Timer expiry (Timer T19) */
    public static final int ISUP_MTR_GROUP_BLOCKING_TIMEOUT = 19;
    /** maintenance timeout reason - Waiting for Group Unblocking Acknowledgement - CGU Timer expiry (Timer T21) */
    public static final int ISUP_MTR_GROUP_UNBLOCKING_TIMEOUT = 21;
    /** maintenance timeout reason - Waiting for Release Response in reply to Group Reset Circuit - GRS Timer expiry (Timer T23) */
    public static final int ISUP_MTR_GROUP_RESET_CIRCUIT_TIMEOUT = 23;
    /** maintenance timeout reason - Waiting for Circuit Query Response - CQM Timer expiry (Timer T28) */
    public static final int ISUP_MTR_CIRCUIT_QUERY_TIMEOUT = 28;
    /** maintenance timeout reason - Waiting for Circuit Validation Response - CVT Timer expiry (Timer TCVT) */
    public static final int ISUP_MTR_CIRCUIT_VALIDATION_TIMEOUT = 44;
    /** maintenance timeout reason - Waiting for Repeat Continuity Check Request - (Timer TCCRr/T27 expiry) */
    public static final int ISUP_MTR_CONTINUITY_CHECK_REPEAT_TIMEOUT = 40;
    /** maintenance timeout reason - Waiting for Continuity Check Response (COTF or REL) - (Timer ANSI T34/ITU T36 expiry) */
    public static final int ISUP_MTR_CONTINUITY_CHECK_RESPONSE_TIMEOUT = 26;
    /** maintenance timeout reason - Waiting for LoopBack Acknowledgement - (Timer ANSI TCCR expiry) */
    public static final int ISUP_MTR_LOOPBACK_ACK_TIMEOUT = 34;
    /** maintenance timeout reason - Waiting for Carrier Restoral - (Timer ANSI THGA expiry) */
    public static final int ISUP_MTR_HGA_TIMEOUT = 47;
    /** maintenance timeout reason - Demand Continuity Check in SCGA group - (Timer ANSI TSCGA expiry) */
    public static final int ISUP_MTR_SCGA_TIMEOUT = 48;
    /** Constructor for initializing the maintenance timeout Event.
      * @param source  The source of this event.
      * @param dpc  The destination point code.
      * @param opc  The origination point code.
      * @param sls  Not used as it is a locally generated event.
      * @param cic  CIC on which the error has been raised.
      * @param congestionPriority  Not used, filled as 0. Refer to
      * getCongestionPriority method in IsupEvent class for more details.
      * @param maintTimeoutReason  The timer which has expired Refer to
      * getMaintTmoReason method for more information.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public MaintenanceTimeoutIndEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
            int maintTimeoutReason)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
        this.setMaintenanceTimeoutReason(maintTimeoutReason);
    }
    /** Gets the ISUP MAINTENANCE TIMEOUT INDICATOR primtive value.
      * @return The ISUP MAINTENANCE TIMEOUT INDICATOR primitive value.  */
    public int getIsupPrimitive() {
        return IsupConstants.ISUP_PRIMITIVE_MAINT_TMO_IND;
    }
    /** Returns the maintenance timer identity that has expired.
      * The timeout causes a maintenance activity to begin at the stack end. eg. When
      * the timer awaiting RLC expires, then reset circuit is started on the CIC. At
      * this point a MaintenanceTimeoutIndEvent object is sent from Provider to the
      * ISUP Listener with the maintenance timeout reason as ISUP_MTR_REL_TIMEOUT,
      * which caused the RSC to be sent to the network. Similarly when the timer
      * awaiting BLA expires, a maintenance timeout object with cause as
      * ISUP_MTR_BLO_TIMEOUT is sent.
      * @return The maintenance timeout reason is sent which are as follows; <ul>
      * <li>ISUP_MTR_RESET_CIRCUIT_TIMEOUT, <li>ISUP_MTR_RELEASE_TIMEOUT,
      * <li>ISUP_MTR_BLOCKING_TIMEOUT, <li>ISUP_MTR_UNBLOCKING_TIMEOUT,
      * <li>ISUP_MTR_GROUP_BLOCKING_TIMEOUT, <li>ISUP_MTR_GROUP_UNBLOCKING_TIMEOUT,
      * <li>ISUP_MTR_GROUP_RESET_CIRCUIT_TIMEOUT, <li>ISUP_MTR_CIRCUIT_QUERY_TIMEOUT,
      * <li>ISUP_MTR_CIRCUIT_VALIDATION_TIMEOUT,
      * <li>ISUP_MTR_CONTINUITY_CHECK_REPEAT_TIMEOUT,
      * <li>ISUP_MTR_CONTINUITY_CHECK_RESPONSE_TIMEOUT,
      * <li>ISUP_MTR_LOOPBACK_ACK_TIMEOUT, <li>ISUP_MTR_HGA_TIMEOUT and
      * <li>ISUP_MTR_SCGA_TIMEOUT. </ul> */
    public int getMaintenanceTimeoutReason() {
        return m_maintTimeoutReason;
    }
    /** Sets the maintenance timeout reason see getMaintTmoReason for detailed
      * description on maintenance timeout reason and the values it can take.
      * @param maintTimeoutReason  Maintenance timeout reason.  */
    public void setMaintenanceTimeoutReason(int maintTimeoutReason) {
        m_maintTimeoutReason = maintTimeoutReason;
    }
    /** The toString method retrieves a string containing the values of the members of
      * the MaintenanceTimeoutIndEvent class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.MaintenanceTimeoutIndEvent");
        b.append("\n\tm_maintTimeoutReason: " + m_maintTimeoutReason);
        return b.toString();
    }
    protected int m_maintTimeoutReason;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
