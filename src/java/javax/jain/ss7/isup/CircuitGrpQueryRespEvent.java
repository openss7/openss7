/*
 @(#) $RCSfile: CircuitGrpQueryRespEvent.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:03 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:03 $ by $Author: brian $
 */

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** An ISUP EVENT: The CircuitGrpQueryRespEvent class is a sub class of the
    CircuitGrpCommonEvent class, and is exchanged between an ISUP Provider and an ISUP
    Listener for exchanging the LOCAL or REMOTE status of the circuits.
    The primitive field indicates whether the status is LOCAL or REMOTE. For the local
    status the implementation will fetch the status of the circuits locally. Hence the
    CircuitGrpQueryRespEvent cannot be sent from the Listener to Provider for a LOCAL
    status of the circuits but can only flow from the Provider to the Listener. For the
    remote status this event from Listener to Provider will cause the sending of the
    Circuit Group Query Response (CQR) message. And when the Provider receives the CQR
    message, it will send the CircuitGrpQueryRespEvent to the Listener.

    @author Monavacon Limited
    @version 1.2.2
  */
public class CircuitGrpQueryRespEvent extends CircuitGrpCommonEvent {
    /** Constructs a new CircuitGrpQueryRespEvent, with the JAIN ISUP Mandatory parameters
        being supplied to the constructor.
        @param source - Source of the object.
        @param dpc - The destination point code.
        @param opc - The origination point code.
        @param sls - Signaling link selection for load sharing.
        @param cic - CIC of the voice circuit on which the message is intended.
        @param primitive - Primitive value is one of the following : <ul>
        <li>ISUP_PRIMITIVE_CIRCUIT_GRP_QUERY_RESP for sending CQM message; and
        <li>ISUP_PRIMITIVE_CIRCUIT_GRP_LOCAL_QUERY_RESP for getting the local status of
        the circuits. </ul>
        @param congestionPriority - Priority of the ISUP message which may be used in the
        optional national congestion control procedures at MTP3. Refer to
        getCongestionPriority method in IsupEvent class for more details.
        @param in_rangeAndStatus - Gives the range of the affected circuits. The Status
        subfield is not present in this Event. Refer to RangeAndStatus.getRange() method
        @param in_circuitStateInd - Gives the status of all the circuits given by the
        range starting from the cic in the message.  i.e. Octet n gives the status of the
        circuit given by CIC cic+(n-1).
        @exception PrimitiveInvalidException - Thrown when the primitive value is not as
        specified.
        @exception ParameterRangeInvalidException - Thrown when value is out of range.
      */
    public CircuitGrpQueryRespEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
            int primitive, RangeAndStatus in_rangeAndStatus, byte[] in_circuitStateInd)
        throws PrimitiveInvalidException, ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority, in_rangeAndStatus);
        this.setIsupPrimitive(primitive);
        this.setCircuitStateInd(in_circuitStateInd);
    }
    /** Gets the ISUP primtive value.
        @return One of the following ISUP primitive values. If ISUP_PRIMITIVE_UNSET is
        returned, it indicates the primitive value is not set, hence this is an invalid
        event. <ul> <li>ISUP_PRIMITIVE_UNSET, <li>ISUP_PRIMITIVE_CIRCUIT_GRP_QUERY_RESP
        and <li>ISUP_PRIMITIVE_CIRCUIT_GRP_LOCAL_QUERY_RESP. </ul>
      */
    public int getIsupPrimitive() {
        return m_isupPrimitive;
    }
    /** Sets the ISUP primtive value.
        @param i_isupPrimitive - The value of isup primitive chosen from: <ul>
        <li>ISUP_PRIMITIVE_CIRCUIT_GRP_QUERY_RESP and
        <li>ISUP_PRIMITIVE_CIRCUIT_GRP_LOCAL_QUERY_RESP. </ul>
        @exception PrimitiveInvalidException - Thrown when the primitive value is not set
        correctly.
      */
    public void setIsupPrimitive(int i_isupPrimitive)
        throws PrimitiveInvalidException {
        switch (i_isupPrimitive) {
            case IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_QUERY_RESP:
            case IsupConstants.ISUP_PRIMITIVE_CIRCUIT_GRP_LOCAL_QUERY_RESP:
                m_isupPrimitive = i_isupPrimitive;
                return;
        }
        throw new PrimitiveInvalidException("Primitive value " + i_isupPrimitive + " out of range.");
    }
    /** Checks if the mandatory parameters have been set.
        @exception MandatoryParameterNotSetException - Thrown when the mandatory parameter
        is not set.
      */
    public void checkMandatoryParameters()
        throws MandatoryParameterNotSetException {
        super.checkMandatoryParameters();
        getIsupPrimitive();
        getCircuitStateInd();
    }
    /** Gets the Circuit State Indicator parameter of the message.
        Each octet of the Circuit State Indicator corresponds to the status of one
        circuit. The status is as given in the respective ITU and ANSI specifications.
        @return The Circuit State Indicator parameter of the event.
        @exception MandatoryParameterNotSetException - Is thrown when mandatory ISUP
        parameter is not set.
      */
    public byte[] getCircuitStateInd()
        throws MandatoryParameterNotSetException {
        return m_circuitStateInd;
    }
    /** Sets the Circuit State Indicator parameter of the message.
        Each octet of the Circuit State Indicator corresponds to the status of one
        circuit. The status is as given in the respective ITU and ANSI specifications.
        @param circuitStateInd - The Circuit State Indicator parameter of the event.
      */
    public void setCircuitStateInd(byte[] circuitStateInd) {
        m_circuitStateInd = circuitStateInd;
    }
    /** The toString method retrieves a string containing the values of the members of the
        CircuitGrpQueryRespEvent class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.CircuitGrpQueryRespEvent");
        b.append("\n\tm_isupPrimitive: " + m_isupPrimitive);
        b.append("\n\tm_circuitStateInd: " + JainSS7Utility.bytesToHex(m_circuitStateInd, 0, m_circuitStateInd.length));
        return b.toString();
    }
    protected int m_isupPrimitive;
    protected byte[] m_circuitStateInd;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
