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

/** An ISUP EVENT: The CircuitGrpQueryEvent class is a sub class of the
  * CircuitGrpCommonEvent class.
  * It is exchanged between an ISUP Provider and an ISUP Listener for getting either
  * local status or the status of the circuits at the remote node using Circuit Group
  * Query Message (CQM). The local status or remote status is determined based on the
  * primitive field in the event. This event is common to both ITU and ANSI.
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class CircuitGrpQueryEvent extends CircuitGrpCommonEvent {
    /** Constructs a new CircuitGrpQueryEvent, with the JAIN ISUP Mandatory parameters
      * being supplied to the constructor.
      * @param source  Source of the object.
      * @param dpc  The destination point code.
      * @param opc  The origination point code.
      * @param sls  Signaling link selection for load sharing.
      * @param cic  CIC of the voice circuit on which the message is intended.
      * @param congestionPriority  Priority of the ISUP message which may be used in
      * the optional national congestion control procedures at MTP3. Refer to
      * getCongestionPriority method in IsupEvent class for more details.
      * @param primitive  Primitive value is one of the following; <ul>
      * <li>ISUP_PRIMITIVE_CIRCUIT_GRP_QUERY for sending CQM message, and
      * <li>ISUP_PRIMITIVE_CIRCUIT_GRP_LOCAL_QUERY for getting the local status of the
      * circuits. </ul>
      * @param in_rangeAndStatus  Gives the range of the affected circuits. IN THIS
      * EVENT, THE STATUS FIELD IS NOT PRESENT.  Refer to RangeAndStatus class for
      * more details.
      * @exception PrimitiveInvalidException  Thrown when the primitive value is not
      * as specified.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public CircuitGrpQueryEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority, int
            primitive, RangeAndStatus in_rangeAndStatus)
        throws PrimitiveInvalidException, ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority, in_rangeAndStatus);
        this.setIsupPrimitive(primitive);
    }
    /** Gets the ISUP primtive value.
      * @return One of the following ISUP primitive values. If ISUP_PRIMITIVE_UNSET is
      * returned, it indicates the primitive value is not set, hence this is an
      * invalid event. <ul> <li>ISUP_PRIMITIVE_UNSET,
      * <li>ISUP_PRIMITIVE_CIRCUIT_GRP_QUERY and
      * <li>ISUP_PRIMITIVE_CIRCUIT_GRP_LOCAL_QUERY. </ul> */
    public int getIsupPrimitive() {
        return m_primitive;
    }
    /** Sets the ISUP primtive value.
      * @param i_isupPrimitive  The value of isup primitive chosen from; <ul>
      * <li>ISUP_PRIMITIVE_CIRCUIT_GRP_QUERY and
      * <li>ISUP_PRIMITIVE_CIRCUIT_GRP_LOCAL_QUERY. </ul>
      * @exception PrimitiveInvalidException  Thrown when the primitive value is not
      * set correctly.  */
    public void setIsupPrimitive(int i_isupPrimitive)
        throws PrimitiveInvalidException {
        switch (i_isupPrimitive) {
            case IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_QUERY:
            case IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_LOCAL_QUERY:
                m_primitive = i_isupPrimitive;
                m_primitiveIsSet = true;
                return;
        }
        throw new PrimitiveInvalidException("IsupPrimitive value " + i_isupPrimitive + " out of range.");
    }
    /** Checks if the mandatory parameters have been set.
      * @exception MandatoryParameterNotSetException  Thrown when the mandatory
      * parameter is not set.  */
    public void checkMandatoryParameters()
        throws MandatoryParameterNotSetException {
        if (m_primitiveIsSet)
            throw new MandatoryParameterNotSetException("IsupPrimitive is not set.");
    }
    /** The toString method retrieves a string containing the values of the members of
      * the CircuitGrpQueryEvent class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.CircuitGrpQueryEvent");
        if (m_primitiveIsSet)
            b.append("\n\tm_primitive: " + m_primitive);
        return b.toString();
    }
    protected int m_primitive;
    protected boolean m_primitiveIsSet = false;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
