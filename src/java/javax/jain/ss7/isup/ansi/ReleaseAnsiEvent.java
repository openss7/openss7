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

/** An ISUP Event: The ReleaseAnsiEvent class is a sub class of the Core ReleaseEvent
    class and handles parameters specific to ANSI variant of REL message.
    @author Monavacon Limited
    @version 1.2.2
  */
public class ReleaseAnsiEvent extends ReleaseEvent {
    /** Constructs a new ReleaseAnsiEvent object for ANSI variant, with only the JAIN ISUP
        Mandatory parameters using the super class constructor.

        @param source  The source of this event.
        @param primitive  Primitive value for StartResetInd is ISUP_PRIMITIVE_RELEASE.
        Refer to IsupEvent class for more details.
        @param dpc  The destination point code.
        @param opc  The origination point code.
        @param sls  The signaling link selection.
        @param cic  The CIC on which the call has been established.
        @param congestionPriority  Priority of the ISUP message which may be used in the
        optional national congestion control procedures at MTP3. Refer to
        getCongestionPriority method in IsupEvent class for more details.
        @param causeIndicator  Cause indicators.
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public ReleaseAnsiEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
            CauseInd cai)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority, cai);
    }
    /** Gets the CallReference parameter of the message.
        Refer to CallReference parameter for greater details.
        @return The CallReference parameter of the event.
        @exception ParameterNotSetException  Thrown when the optional parameter is not
        set.
      */
    public CallReference getCallReference()
        throws ParameterNotSetException {
        if (m_callRef != null)
            return m_callRef;
        throw new ParameterNotSetException("CallReference not set.");
    }
    /** Sets the CallReference parameter of the message.
        Refer to CallReference parameter for greater details.
        @param callRef  The CallReference parameter of the event.
      */
    public void setCallReference(CallReference callRef) {
        m_callRef = callRef;
    }
    /** Indicates if the CallReference parameter is present in this Event.
        @return True if the parameter has been set.
      */
    public boolean isCallReferencePresent() {
        return (m_callRef != null);
    }
    /** Gets the ChargeNumberAnsi parameter of the message.
        Refer to ChargeNumberAnsi parameter for greater details.
        @return The ChargeNumberAnsi parameter of the event.
        @exception ParameterNotSetException  Thrown when the optional parameter is not
        set.
      */
    public ChargeNumberAnsi getChargeNumber()
        throws ParameterNotSetException {
        if (m_chargeNumAnsi != null)
            return m_chargeNumAnsi;
        throw new ParameterNotSetException("ChargeNumberAnsi not set.");
    }
    /** Sets the ChargeNumberAnsi parameter of the message.
        Refer to ChargeNumberAnsi parameter for greater details.
        @param chargeNumAnsi  The ChargeNumberAnsi parameter of the event.
      */
    public void setChargeNumber(ChargeNumberAnsi chargeNumAnsi) {
        m_chargeNumAnsi = chargeNumAnsi;
    }
    /** Indicates if the ChargeNumberAnsi parameter is present in this Event.
        @return True if the parameter has been set.
      */
    public boolean isChargeNumberPresent() {
        return (m_chargeNumAnsi != null);
    }
    /** Gets the Generic Number (referred in ANSI T1.113 as Generic Address) parameter of
        the message.
        Refer to GenericNumber parameter for greater details.
        @return The GenericNumber parameter of the event.
        @exception ParameterNotSetException  Thrown when the optional parameter is not
        set.
      */
    public GenericNumber getGenericNumber()
        throws ParameterNotSetException {
        if (m_genericNum != null)
            return m_genericNum;
        throw new ParameterNotSetException("GenericNumber not set.");
    }
    /** Sets the Generic Number (referred in ANSI T1.113 as Generic Address) parameter of
        the message.
        Refer to GenericNumber parameter for greater details.
        @param genericNum  The GenericNumber parameter of the event.
      */
    public void setGenericNumber(GenericNumber genericNum) {
        m_genericNum = genericNum;
    }
    /** Indicates if the Generic Number (referred in ANSI T1.113 as Generic Address)
        parameter is present in this Event.
        @return True if the parameter has been set.
      */
    public boolean isGenericNumberPresent() {
        return (m_genericNum != null);
    }
    /** Gets the ServiceActivation parameter of the message.
        @return The ServiceActivation parameter of the event.
        @exception ParameterNotSetException  Thrown when the optional parameter is not
        set.
      */
    public byte[] getServiceActivation()
        throws ParameterNotSetException {
        if (m_servAct != null)
            return m_servAct;
        throw new ParameterNotSetException("ServiceActivation not set.");
    }
    /** Sets the ServiceActivation parameter of the message.
        @param servAct  The ServiceActivation parameter of the event.
      */
    public void setServiceActivation(byte[] servAct) {
        m_servAct = servAct;
    }
    /** Indicates if the ServiceActivation parameter is present in this Event.
        @return True if the parameter has been set.
      */
    public boolean isServiceActivationPresent() {
        return (m_servAct != null);
    }
    /** The toString method retrieves a string containing the values of the members of the
        ReleaseAnsiEvent class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.ansi.ReleaseAnsiEvent");
        b.append("\n\tm_callRef: " + m_callRef);
        b.append("\n\tm_chargeNumAnsi: " + m_chargeNumAnsi);
        b.append("\n\tm_genericNum: " + m_genericNum);
        b.append("\n\t: ");
        if (m_servAct != null)
            b.append(JainSS7Utility.bytesToHex(m_servAct, 0, m_servAct.length));
        return b.toString();
    }
    protected CallReference m_callRef = null;
    protected ChargeNumberAnsi m_chargeNumAnsi = null;
    protected GenericNumber m_genericNum = null;
    protected byte[] m_servAct = null;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
