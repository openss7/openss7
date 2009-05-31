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

/** An ISUP Event : The CircuitGrpResetAnsiEvent class is a sub class of ISUP Core
  * CircuitGrpResetEvent class and is to handle the ANSI variant of GRS message.
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class CircuitGrpResetAnsiEvent extends CircuitGrpResetEvent {
    /** Constructs a new CircuitGrpResetEvent, with only the JAIN ISUP Mandatory
      * parameters being supplied to the constructor, using the super class's
      * constructor.
      * @param source  The source of this event.
      * @param primitive  Primitive value is ISUP_PRIMITIVE_CIRCUIT_GRP_RESET. Refer to
      * IsupEvent class for more details.
      * @param dpc  The destination point code.
      * @param opc  The origination point code.
      * @param sls  The signaling link selection.
      * @param cic  The CIC on which the call has been established.
      * @param congestionPriority  Priority of the ISUP message which may be used in
      * the optional national congestion control procedures at MTP3. Refer to
      * getCongestionPriority method in IsupEvent class for more details.
      * @param in_rangeAndStatus  Gives the range of the affected circuits. IN THIS
      * EVENT, THE STATUS FIELD IS NOT PRESENT.  Refer to RangeAndStatus class for more
      * details.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public CircuitGrpResetAnsiEvent(java.lang.Object source, SignalingPointCode dpc, SignalingPointCode opc, byte sls,
            int cic, byte congestionPriority, RangeAndStatus in_rangeAndStatus)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority, in_rangeAndStatus);
    }
    /** Gets the CircuitAssignMapAnsi parameter of the message.
      * @return The CircuitAssignMapAnsi parameter of the event.
      * @exception ParameterNotSetException  Thrown when the parameter is not set in
      * the event.  */
    public CircuitAssignMapAnsi getCircuitAssignMap() throws ParameterNotSetException {
        if (isCircuitAssignMapPresent())
            return m_cam;
        throw new ParameterNotSetException("CircuitAssignMapAnsi is not set.");
    }
    /** Sets the CircuitAssignMapAnsi Group parameter of the message.  
      * @param circuitAssignMapAnsi  Business group parameter.  */
    public void setCircuitAssignMap(CircuitAssignMapAnsi circuitAssignMapAnsi) {
        m_cam = circuitAssignMapAnsi;
    }
    /** Indicates if the CircuitAssignMapAnsi parameter is present in this Event.
      * @return True if the parameter is set.  */
    public boolean isCircuitAssignMapPresent() {
        return (m_cam != null);
    }
    /** The toString method retrieves a string containing the values of the members of
      * the CircuitGrpResetAnsiEvent class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\nCircuitGrpResetAnsiEvent:");
        if (isCircuitAssignMapPresent())
            b.append("\n\tCircuitAssignMap: " + m_cam);
        return b.toString();
    }
    private CircuitAssignMapAnsi m_cam = null;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
