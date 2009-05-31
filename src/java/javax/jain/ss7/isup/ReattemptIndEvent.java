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

/** An ISUP EVENT: The ReattemptIndEvent class is a sub class of the IsupEvent class and
  * is used to indicate to the Listener object, that a reattempt has to be made on another
  * circuit for this call.
  * For example, during dual seizure, on a non-controlling circuit, the call is backed off
  * and a EventReattempt Indication object is sent to the application for retrying the
  * call on another circuit.
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class ReattemptIndEvent extends IsupEvent {
    public static final int ISUP_RR_DUAL_SEIZURE = 1;
    public static final int ISUP_RR_BAD_RESPONSE_FOR_IAM = 2;
    /** Constructor for initializing the ReattemptInd event.
      * @param sourcethe  Source of this event.
      * @param dpc  The destination point code.
      * @param opc  The origination point code.
      * @param sls  Not used as it is a locally generated event.
      * @param cic  CIC on which the error has been raised.
      * @param congestionPriority  Not used, filled as 0. Refer to getCongestionPriority
      * method in IsupEvent class for more details.
      * @param callReattemptReason  Call reattempt reason refer to getCallReattemptReason
      * method for values of callReattemptReason.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public ReattemptIndEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
            int callReattemptReason)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
        this.setCallReattemptReason(callReattemptReason);
    }
    /** Gets the ISUP REATTEMPT INDICATOR primtive value.
      * @return The ISUP REATTEMPT INDICATOR primitive value.
      */
    public int getIsupPrimitive() {
        return IsupConstants.ISUP_PRIMITIVE_REATTEMPT_IND;
    }
    /** Gets the value for call reattempt reason.
      * @return The call reattempt reason. Call reattempt reason values are as follows
      * <ul> <li>ISUP_RR_DUAL_SEIZURE and <li>ISUP_RR_BAD_RESPONSE_FOR_IAM. </ul>
      */
    public int getCallReattemptReason() {
        return m_callReattemptReason;
    }
    /** Sets the value for call reattempt reason value.
      * Refer to getCallReattemptReason method for values of callReattemptReason.
      * @param callReattemptReason  Call reattempt reason.
      */
    public void setCallReattemptReason(int callReattemptReason) {
        m_callReattemptReason = callReattemptReason;
    }
    /** The toString method retrieves a string containing the values of the members of the
      * ReattemptIndEvent class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.siup.ReatemptIndEvent");
        b.append("\n\tm_callReattemptReason: " + m_callReattemptReason);
        return b.toString();
    }
    protected int m_callReattemptReason;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
