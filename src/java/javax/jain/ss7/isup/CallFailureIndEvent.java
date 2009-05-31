/* ***************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** An ISUP EVENT : The CallFailureIndEvent class is a sub class of the IsupEvent
  * class and is used to indicate to the Listener that a call has failed due to
  * protocol timer expiry relating to call processing.  eg. Answer Timeout, Address
  * Complete Timeout etc. This information is filled in the call failure reason.
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class CallFailureIndEvent extends IsupEvent {
    public static final int ISUP_CFR_ADDRESS_COMPLETE_TIMEOUT = 7;
    public static final int ISUP_CFR_ANSWER_TIMEOUT = 9;
    public static final int ISUP_CFR_INFORMATION_TIMEOUT = 33;
    public static final int ISUP_CFR_CIRCUIT_RESERVATION_TIMEOUT = 43;
    public static final int ISUP_CFR_CIRCUIT_RESERVATION_ACK_TIMEOUT = 42;
    public static final int ISUP_CFR_RESUME_TIMEOUT = 2;
    public static final int ISUP_CFR_CONTINUITY_CHECK_RESPONSE_TIMEOUT = 8;
    public static final int ISUP_CFR_CHARGE_INFO_TIMEOUT = 0;
    /** Constructor for initializing the CallFailureInd event.
      * @param source - The source of this event.
      * @param dpc - The destination point code.
      * @param opc - The origination point code.
      * @param sls - Not used as it is a locally generated event.
      * @param cic - CIC on which the error has been raised.
      * @param congestionPriority - Not used, fill as 0. Refer to
      * getCongestionPriority method in IsupEvent class for more details.
      * @param callFailureReason - Call failure reason refer to getCallFailureReason
      * method for values of callFailureReason.
      * @exception ParameterRangeInvalidException - Thrown when value is out of range.  */
    public CallFailureIndEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
            int callFailureReason)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
        m_callFailureReason = callFailureReason;
    }
    /** Gets the ISUP Call Failure Indication primtive value.
      * @return The ISUP Call Failure Indication primitive value.  */
    public int getIsupPrimitive() {
        return IsupConstants.ISUP_PRIMITIVE_CALL_FAILURE_IND;
    }
    /** Gets the value for cause value leading to the call failure.
      * @return The call failure reason. Call Failure reason values are as
      * follows;<ul>
      * <li>ISUP_CFR_ADDRESS_COMPLETE_TIMEOUT
      * <li>ISUP_CFR_ANSWER_TIMEOUT
      * <li>ISUP_CFR_INFORMATION_TIMEOUT
      * <li>ISUP_CFR_CIRCUIT_RESERVATION_TIMEOUT
      * <li>ISUP_CFR_CIRCUIT_RESERVATION_ACK_TIMEOUT
      * <li>ISUP_CFR_SUSPEND_TIMEOUT
      * <li>ISUP_CFR_CONTINUITY_CHECK_RESPONSE_TIMEOUT
      * <li>ISUP_CFR_CHARGE_INFO_TIMEOUT</ul> */
    public int getCallFailureReason() {
        return m_callFailureReason;
    }
    /** Sets the value for call failure reason value. refer to getCallFailureReason
      * method for values of callFailureReason.  callFailReason call failure reason.  */
    public void setCallFailureReason(int callFailureReason) {
        m_callFailureReason = callFailureReason;
    }
    /** The toString method retrieves a string containing the values of the members of
      * the CallFailureIndEvent class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.CallFailureEvent:");
        b.append("\n\tm_callFailureReason: " + m_callFailureReason);
        return b.toString();
    }
    protected int m_callFailureReason;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
