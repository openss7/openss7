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

/** An ISUP Event: The CircuitReservationAnsiEvent class is a subclass of the IsupEvent
  * class and is used for exchanging between an ISUP Provider and an ISUP Listener for
  * sending the ANSI ISUP Circuit Reservation message (CRM).
  * @author Monavacon
  * @version 1.2.2
  */
public class CircuitReservationAnsiEvent extends IsupEvent {
    /** Constructs a new EventCircuitReservation for ANSI variant, with only the JAIN ISUP
      * Mandatory parameters using the super class constructor.
      * @param source  The source of this event.
      * @param dpc  The destination point code.
      * @param opc  The origination point code.
      * @param sls  The signaling link selection.
      * @param cic  The CIC on which the call has been established.
      * @param congestionPriority  Priority of the ISUP message which may be used in the
      * optional national congestion control procedures at MTP3. Refer to
      * getCongestionPriority method in IsupEvent class for more details.
      * @param in_natureConnInd  Nature of Connection Indicators. Refer to for more
      * details.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public CircuitReservationAnsiEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
            NatureConnInd in_natureConnInd)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
        this.setNatureConnInd(in_natureConnInd);
    }
    /** Gets the ISUP CIRCUIT RESERVATION primtive value.
      * @return The ISUP CIRCUIT RESERVATION primitive value.  */
    public int getIsupPrimitive() {
        return IsupConstants.ISUP_PRIMITIVE_CIRCUIT_RESERVATION;
    }
    /** Checks if the mandatory parameters have been set.
      * @exception MandatoryParameterNotSetException  Thrown when the mandatory parameter
      * is not set.  */
    public void checkMandatoryParameters()
        throws MandatoryParameterNotSetException {
        super.checkMandatoryParameters();
        if (m_natureConnInd == null)
            throw new MandatoryParameterNotSetException("NatureConnInd is not set.");
    }
    /** Gets the Nature of connection indicators parameter of the message.
      * Refer to NatureConnInd parameter class for more information.
      * @return The Nature of Connection Indicator parameter of the event.
      * @exception MandatoryParameterNotSetException  Thrown when mandatory ISUP parameter
      * is not set.  */
    public NatureConnInd getNatureConnInd()
        throws MandatoryParameterNotSetException {
        if (m_natureConnInd != null)
            return m_natureConnInd;
        throw new MandatoryParameterNotSetException("NatureConnInd is not set.");
    }
    /** Sets the Event Information parameter of the message.
      * Refer to NatureConnInd parameter class for more information.
      * @param in_natureConnIndthe  Nature of Connection Indicator parameter of the event.  */
    public void setNatureConnInd(NatureConnInd in_natureConnInd) {
        m_natureConnInd = in_natureConnInd;
    }
    /** The toString method retrieves a string containing the values of the members of the
      * CircuitReservationAnsiEvent class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.CircuitReservationAnsiEvent");
        b.append("\n\tm_natureConnInd: " + m_natureConnInd);
        return b.toString();
    }
    protected NatureConnInd m_natureConnInd = null;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
