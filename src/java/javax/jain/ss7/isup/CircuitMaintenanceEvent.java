/*
 @(#) src/java/javax/jain/ss7/isup/CircuitMaintenanceEvent.java <p>
 
 Copyright &copy; 2008-2015  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
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
 <a href="http://www.openss7.com/">http://www.openss7.com/</a>
 */

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** An ISUP EVENT: The Circuit Maintenance class is a sub class of the IsupEvent class
  * and is exchanged between an ISUP Provider and an ISUP Listener for sending and
  * receiving the ISUP circuit maintenance messages BLO, BLA, UBL, UBA and RSC
  * messages.
  * It is also used for sending and and receiving CCR, UCIC, OLM and LPA messages. In
  * case of ANSI, this class is also used to send or receive the CRA and CVT messages.
  * So ISUP Provider would send a Circuit Maintenance object to the listener on the
  * reception of one of BLO, BLA, UBL, UBA, RSC, CCR, UCIC, OLM, LPA, CVT or CRA
  * messages from the stack, for the user address handled by that listener.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class CircuitMaintenanceEvent extends IsupEvent {
    /** Constructs a new CircuitMaintenanceEvent, with the JAIN ISUP Mandatory
      * parameters being supplied to the constructor.
      * @param source  Source of the object.
      * @param primitive  Primitive field indicates whether the message to be sent out
      * or being received is a BLO or a BLA or a UBL or a UBA or a RSC or a CCR or an
      * OLM or a LPA or a UCIC. In ANSI, it can be used to send a CVT or a CRA. For a
      * CircuitMaintenanceEvent object, the primitive field can one of the following
      * values; <ul>
      * <li>ISUP_PRIMITIVE_BLOCKING, for BLO;
      * <li>ISUP_PRIMITIVE_UNBLOCKING, for UBL;
      * <li>ISUP_PRIMITIVE_BLOCKING_ACK, for BLA;
      * <li>ISUP_PRIMITIVE_UNBLOCKING_ACK, for UBA;
      * <li>ISUP_PRIMITIVE_RESET_CIRCUIT, for RSC;
      * <li>ISUP_PRIMITIVE_CONTINUITY_CHECK_REQ, for CCR;
      * <li>ISUP_PRIMITIVE_OVERLOAD, for OLM;
      * <li>ISUP_PRIMITIVE_LOOPBACK_ACK, for LPA;
      * <li>ISUP_PRIMITIVE_UNEQUIPPED_CIC, for UCIC;
      * <li>ISUP_PRIMITIVE_CIRCUIT_RESERVATION_ACK, for CRA; and,
      * <li>ISUP_PRIMITIVE_CIRCUIT_VALIDATION_TEST, for CVT. </ul>
      * @param dpc  The destination point code.
      * @param opc  The origination point code.
      * @param sls  Signaling link selection for load sharing.
      * @param cic  CIC on which the error has been raised.
      * @param congestionPriority  Priority of the ISUP message which may be used in
      * the optional national congestion control procedures at MTP3. Refer to
      * getCongestionPriority method in IsupEvent class for more details.
      * @exception PrimitiveInvalidException  Thrown when the primitive value is not
      * set correctly.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
public CircuitMaintenanceEvent(java.lang.Object source, SignalingPointCode dpc,
        SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
        int primitive)
    throws PrimitiveInvalidException, ParameterRangeInvalidException {
    super(source, dpc, opc, sls, cic, congestionPriority);
    this.setIsupPrimitive(primitive);
}
    /** Gets the ISUP primtive value.
      * @return One of the following ISUP primitive values. If ISUP_PRIMITIVE_UNSET is
      * returned, it indicates the primitive value is not set, hence this is an
      * invalid event; <ul>
      * <li>ISUP_PRIMITIVE_UNSET, for primitive value unset event;
      * <li>ISUP_PRIMITIVE_BLOCKING, for BLO;
      * <li>ISUP_PRIMITIVE_UNBLOCKING, for UBL;
      * <li>ISUP_PRIMITIVE_BLOCKING_ACK, for BLA;
      * <li>ISUP_PRIMITIVE_UNBLOCKING_ACK, for UBA;
      * <li>ISUP_PRIMITIVE_RESET_CIRCUIT, for RSC;
      * <li>ISUP_PRIMITIVE_CONTINUITY_CHECK_REQ, for CCR;
      * <li>ISUP_PRIMITIVE_OVERLOAD, for OLM;
      * <li>ISUP_PRIMITIVE_LOOPBACK_ACK, for LPA;
      * <li>ISUP_PRIMITIVE_UNEQUIPPED_CIC, for UCIC;
      * <li>ISUP_PRIMITIVE_CIRCUIT_RESERVATION_ACK, for CRA; and,
      * <li>ISUP_PRIMITIVE_CIRCUIT_VALIDATION_TEST, for CVT. </ul> */
    public int getIsupPrimitive() {
        if (m_isupPrimitive != IsupConstants.ISUP_PRIMITIVE_UNSET)
            return m_isupPrimitive;
        return IsupConstants.ISUP_PRIMITIVE_UNSET;
    }
    /** Sets the ISUP primtive value.
      * @param i_isupPrimitive  The value of isup primitive from; <ul>
      * <li>ISUP_PRIMITIVE_BLOCKING, for BLO;
      * <li>ISUP_PRIMITIVE_UNBLOCKING, for UBL;
      * <li>ISUP_PRIMITIVE_BLOCKING_ACK, for BLA;
      * <li>ISUP_PRIMITIVE_UNBLOCKING_ACK, for UBA;
      * <li>ISUP_PRIMITIVE_RESET_CIRCUIT, for RSC;
      * <li>ISUP_PRIMITIVE_CONTINUITY_CHECK_REQ, for CCR;
      * <li>ISUP_PRIMITIVE_OVERLOAD, for OLM;
      * <li>ISUP_PRIMITIVE_LOOPBACK_ACK, for LPA;
      * <li>ISUP_PRIMITIVE_UNEQUIPPED_CIC, for UCIC;
      * <li>ISUP_PRIMITIVE_CIRCUIT_RESERVATION_ACK, for CRA; and,
      * <li>ISUP_PRIMITIVE_CIRCUIT_VALIDATION_TEST, for CVT. </ul>
      * @exception PrimitiveInvalidException  Thrown when the primitive value is not
      * set correctly.
      */
    public void setIsupPrimitive(int i_isupPrimitive)
        throws PrimitiveInvalidException {
        switch (i_isupPrimitive) {
            case IsupConstants.ISUP_PRIMITIVE_BLOCKING:
            case IsupConstants.ISUP_PRIMITIVE_UNBLOCKING:
            case IsupConstants.ISUP_PRIMITIVE_BLOCKING_ACK:
            case IsupConstants.ISUP_PRIMITIVE_UNBLOCKING_ACK:
            case IsupConstants.ISUP_PRIMITIVE_RESET_CIRCUIT:
            case IsupConstants.ISUP_PRIMITIVE_CONTINUITY_CHECK_REQ:
            case IsupConstants.ISUP_PRIMITIVE_OVERLOAD:
            case IsupConstants.ISUP_PRIMITIVE_LOOPBACK_ACK:
            case IsupConstants.ISUP_PRIMITIVE_UNEQUIPPED_CIC:
            case IsupConstants.ISUP_PRIMITIVE_CIRCUIT_RESERVATION_ACK:
            case IsupConstants.ISUP_PRIMITIVE_CIRCUIT_VALIDATION_TEST:
                m_isupPrimitive = i_isupPrimitive;
                return;
        }
        throw new PrimitiveInvalidException("IsupPrimitive value " + i_isupPrimitive + " out of range.");
    }
    /** Checks if the mandatory parameters have been set.
      * @exception MandatoryParameterNotSetException  Thrown when the mandatory
      * parameter is not set.
      */
    public void checkMandatoryParameters()
        throws MandatoryParameterNotSetException {
        super.checkMandatoryParameters();
        if (m_isupPrimitive == IsupConstants.ISUP_PRIMITIVE_UNSET)
            throw new MandatoryParameterNotSetException("IsupPrimitive not set.");
    }
    /** The toString method retrieves a string containing the values of the members of
      * the CircuitMaintenanceEvent class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.CircuitMaintenanceEvent");
        b.append("\n\tm_isupPrimitive: " + m_isupPrimitive);
        return b.toString();
    }
    protected int m_isupPrimitive = IsupConstants.ISUP_PRIMITIVE_UNSET;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
