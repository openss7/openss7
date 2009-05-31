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

package javax.jain.ss7.isup.ansi;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** An ISUP Event: The ExitAnsiEvent class is a subclass of the IsupEvent class and is
    used for exchanging between an ISUP Provider and an ISUP Listener for sending the ANSI
    ISUP Exit Reservation message (EXM).
    @author Monavacon Limited
    @version 1.2.2
  */
public class ExitAnsiEvent extends IsupEvent {
    /** Constructs a new ExitAnsiEvent for ANSI variant, with only the JAIN ISUP Mandatory
        parameters using the super class constructor.
        @param source - The source of this event.
        @param dpc - The destination point code.
        @param opc - The origination point code.
        @param sls - The signaling link selection.
        @param cic - The CIC on which the call has been established.
        @param congestionPriority - Priority of the ISUP message which may be used in the
        optional national congestion control procedures at MTP3. Refer to
        getCongestionPriority method in IsupEvent class for more details.
        @exception ParameterRangeInvalidException - Thrown when value is out of range.
      */
    public ExitAnsiEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
    }
    /** Gets the ISUP EXIT primtive value.
        @return The ISUP EXIT primitive value.
      */
    public int getIsupPrimitive() {
        return IsupConstants.ISUP_PRIMITIVE_EXIT;
    }
    /** Gets the Outgoing Trunk Group Number parameter of the message.
        @return The string representing the Outgoing Trunk Group Number parameter of the
        event, 0 for validation successful and 1 for validation failure.
        @exception ParameterNotSetException - Thrown when the parameter is not set in the
        event.
      */
    public java.lang.String getOutgoingTrunkGrpNumber()
        throws ParameterNotSetException {
        if (m_ogTgpNum != null)
            return m_ogTgpNum;
        throw new ParameterNotSetException("OutgoingTrunkGrpNumber is not set.");
    }
    /** Sets the Outgoing Trunk Group Number parameter of the message.
        @param in_ogTgpNum - String value representing the Outgoing Trunk Group Number
        parameter of the event.
      */
    public void setOutgoingTrunkGrpNumber(java.lang.String in_ogTgpNum) {
        m_ogTgpNum = in_ogTgpNum;
    }
    /** Indicates if the Outgoing Trunk Group Number parameter is present in this Event.
        @return True if the parameter is set else false.
      */
    public boolean isOutgoingTrunkGrpNumberPresent() {
        return (m_ogTgpNum != null);
    }
    /** The toString method retrieves a string containing the values of the members of the
        ExitAnsiEvent class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.ansi.ExitAnsiEvent");
        b.append("\n\tm_ogTgpNum: " + m_ogTgpNum);
        return b.toString();
    }
    protected String m_ogTgpNum = null;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
