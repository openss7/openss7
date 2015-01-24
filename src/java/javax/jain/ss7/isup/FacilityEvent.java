/*
 @(#) src/java/javax/jain/ss7/isup/FacilityEvent.java <p>
 
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

/** An ISUP EVENT: The FacilityEvent class is a sub class of the IsupEvent class and is
    exchanged between an ISUP Provider and an ISUP Listener for sending and receiving the
    ISUP FacilityEvent Message.
    Listener would send an FacilityEvent object to the provider for sending a FAC message
    to the ISUP stack. ISUP Provider would send an FacilityEvent object to the listener on
    the reception of an FAC message from the stack for the user address handled by that
    listener.  The ANSI variant of the event is the same as the core class. The ITU
    variant of Facility event, inherits from the core class and adds the
    MessageCompatibilityInfo and ParamCompatibilityInfo parameter classes.  The mandatory
    parameters for generating an FAC ISUP message are set using the constructor itself.
    The optional parameters may be set using get and set methods described below. The
    primitive field is filled as ISUP_PRIMITIVE_FACILITY.

    @author Monavacon Limited
    @version 1.2.2
  */
public class FacilityEvent extends IsupEvent {
    /** Constructs a new FacilityEvent, with only the JAIN ISUP Mandatory parameters being
        supplied to the constructor.
        @param source  The source of this event.
        @param dpc  The destination point code.
        @param opc  The origination point code.
        @param sls  The signaling link selection.
        @param cic  The CIC on which the call has been established.
        @param congestionPriority  Priority of the ISUP message which may be used in the
        optional national congestion control procedures at MTP3. Refer to
        getCongestionPriority method in IsupEvent class for more details.
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public FacilityEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
    }
    /** Gets the ISUP FACILITY primtive value.
        @return The ISUP FACILITY primitive value.
      */
    public int getIsupPrimitive() {
        return IsupConstants.ISUP_PRIMITIVE_FACILITY;
    }
    /** Gets the RemoteOperations parameter of the message.
        Refer to RemoteOperations parameter for greater details.
        @return Remote operation parameter.
        @exception ParameterNotSetException  Thrown when the optional parameter is not
        set.
      */
    public RemoteOperations getRemoteOperations()
        throws ParameterNotSetException {
        if (m_remoteOp != null)
            return m_remoteOp;
        throw new ParameterNotSetException("RemoteOperations not set.");
    }
    /** Sets the RemoteOperations parameter of the message.
        Refer to RemoteOperations parameter for greater details.
        @param remoteOp  The RemoteOperations parameter of the event.
      */
    public void setRemoteOperations(RemoteOperations remoteOp) {
        m_remoteOp = remoteOp;
    }
    /** Indicates if the RemoteOperations parameter is present in this Event.
        @return True if the parameter has been set.
      */
    public boolean isRemoteOperationsPresent() {
        return (m_remoteOp != null);
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
        FacilityEvent class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.FacilityEvent");
        b.append("\n\tm_remoteOp: " + m_remoteOp);
        b.append("\n\tm_servAct: ");
        if (m_servAct != null)
            b.append(JainSS7Utility.bytesToHex(m_servAct, 0, m_servAct.length));
        return b.toString();
    }
    protected RemoteOperations m_remoteOp = null;
    protected byte[] m_servAct = null;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
