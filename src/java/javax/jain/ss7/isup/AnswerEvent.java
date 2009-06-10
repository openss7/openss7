/*
 @(#) $RCSfile$ $Name$($Revision$) $Date$ <p>
 
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
 
 Last Modified $Date$ by $Author$
 */

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** An ISUP EVENT: The AnswerEvent class is a sub class of the IsupEvent class and is
  * exchanged between an ISUP Provider and an ISUP Listener for sending and receiving
  * the ISUP AnswerEvent Message.
  * Listener would send an AnswerEvent object to the provider for sending a ANM
  * message to the ISUP stack. ISUP Provider would send an AnswerEvent object to the
  * listener on the reception of an ANM message from the stack for the user address
  * handled by that listener. The mandatory parameters for generating an ANM ISUP
  * message are set using the constructor itself.  The optional parameters may be set
  * using get and set methods described below.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract class AnswerEvent extends IsupEvent {
    /** Constructs a new AnswerEvent, with only the JAIN ISUP Mandatory parameters
      * being supplied to the constructor.
      * @param source  The source of this event.
      * @param dpc  The destination point code.
      * @param opc  The origination point code.
      * @param sls  The signaling link selection.
      * @param cic  The CIC on which the call has been established.
      * @param congestionPriority  Priority of the ISUP message which may be used in
      * the optional national congestion control procedures at MTP3. Refer to
      * getCongestionPriority method in IsupEvent class for more details.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    protected AnswerEvent(Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
    }
    /** Gets the ISUP ANSWER primtive value.
      * @return The ISUP ANSWER primitive value.  */
    public int getIsupPrimitive() {
        return IsupConstants.ISUP_PRIMITIVE_ANSWER;
    }
    /** Gets the CallReference parameter of the message.
      * Refer to CallReference parameter for greater details.
      * @return The CallReference parameter of the event.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.  */
    public CallReference getCallReference()
        throws ParameterNotSetException {
        if (m_callRef != null)
            return m_callRef;
        throw new ParameterNotSetException("CallReference not set.");
    }
    /** Sets the CallReference parameter of the message.
      * Refer to CallReference parameter for greater details.
      * @param callRef  The CallReference parameter of the event.  */
    public void setCallReference(CallReference callRef) {
        m_callRef = callRef;
    }
    /** Indicates if the CallReference parameter is present in this Event.
      * @return True if the parameter has been set.  */
    public boolean isCallReferencePresent() {
        return (m_callRef != null);
    }
    /** Gets the BackwarCallIndicators parameter of the message.
      * Refer to BwdCallInd parameter for greater details.
      * @return The BackwarCallIndicators parameter of the event.  */
    public BwdCallInd getBwdCallInd()
        throws ParameterNotSetException {
        if (m_bci != null)
            return m_bci;
        throw new ParameterNotSetException("BwdCallInd not set.");
    }
    /** Sets the BackwarCallIndicators parameter of the message.
      * Refer to BwdCallInd parameter for greater details.
      * @param bci The BackwarCallIndicators parameter of the event.  */
    public void setBwdCallInd(BwdCallInd bci) {
        m_bci = bci;
    }
    /** Indicates if the BwdCallInd parameter is present in this Event.
      * @return True if the parameter has been set.  */
    public boolean isBwdCallIndPresent() {
        return (m_bci != null);
    }
    /** Gets the NotificationInd parameter of the message.
      * @return The NotificationInd parameter of the event.
      * @exception ParameterNotSetException  Thrown when the parameter is not set in
      * the event.  */
    public NotificationInd getNotificationInd()
        throws ParameterNotSetException {
        if (m_noi != null)
            return m_noi;
        throw new ParameterNotSetException("NotificationInd not set.");
    }
    /** Sets the NotificationInd parameter of the message.
      * @param noi  Notification Indicator parameter.  */
    public void setNotificationInd(NotificationInd noi) {
        m_noi = noi;
    }
    /** Indicates if the NotificationInd parameter is present in this Event.
      * @return True if the parameter is set.  */
    public boolean isNotificationIndPresent() {
        return (m_noi != null);
    }
    /** Gets the Optional Backward call indicator parameter of the message.
      * Refer to OptionalBwdCallInd parameter for greater details.
      * @return The OptionalBwdCallInd parameter of the event.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.  */
    public OptionalBwdCallInd getOptionalBwdCallInd()
        throws ParameterNotSetException {
        if (m_obci != null)
            return m_obci;
        throw new ParameterNotSetException("OptionalBwdCallInd not set.");
    }
    /** Sets the Optional Backward call indicator parameter of the message.
      * Refer to OptionalBwdCallInd parameter for greater details.
      * @param obci  The Optional Backward Call Indicator parameter of the event.  */
    public void setOptionalBwdCallInd(OptionalBwdCallInd obci) {
        m_obci = obci;
    }
    /** Indicates if the OptionalBwdCallInd parameter is present in this Event.
      * @return True if the parameter has been set.  */
    public boolean isOptionalBwdCallIndPresent() {
        return (m_obci != null);
    }
    /** Gets the AccessTransport parameter of the message.
      * Refer to AccessTransport parameter for greater details.
      * @return The AccessTransport parameter of the event.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.  */
    public byte[] getAccessTransport()
        throws ParameterNotSetException {
        if (m_accessTrans != null)
            return m_accessTrans;
        throw new ParameterNotSetException("AccessTransport not set.");
    }
    /** Sets the AccessTransport parameter of the message.
      * Refer to AccessTransport parameter for greater details.
      * @param accessTrans  The AccessTransport parameter of the event.  */
    public void setAccessTransport(byte[] accessTrans) {
        m_accessTrans = accessTrans;
    }
    /** Indicates if the AccessTransport parameter is present in this Event.
      * @return True if the parameter has been set.  */
    public boolean isAccessTransportPresent() {
        return (m_accessTrans != null);
    }
    /** Gets the UserToUserIndicators parameter of the message.
      * Refer to UserToUserIndicators parameter for greater details.
      * @return The UserToUserIndicators parameter of the event.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.  */
    public UserToUserIndicators getUserToUserIndicators()
        throws ParameterNotSetException {
        if (m_uui != null)
            return m_uui;
        throw new ParameterNotSetException("UserToUserIndicators not set.");
    }
    /** Sets the UserToUserIndicators parameter of the message.
      * Refer to UserToUserIndicators parameter for greater details.
      * @param uui  The UserToUserIndicators parameter of the event.  */
    public void setUserToUserIndicators(UserToUserIndicators uui) {
        m_uui = uui;
    }
    /** Indicates if the UserToUserIndicators parameter is present in this Event.
      * @return True if the parameter has been set.  */
    public boolean isUserToUserIndicatorsPresent() {
        return (m_uui != null);
    }
    /** Gets the UserToUserInformation parameter of the message.  
      * @return The byte array for UserToUserInformation parameter of the event.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.  */
    public byte[] getUserToUserInformation()
        throws ParameterNotSetException {
        if (m_userToUserInfo != null)
            return m_userToUserInfo;
        throw new ParameterNotSetException("UserToUserInformation not set.");
    }
    /** Sets the UserToUserInformation parameter of the message.
      * @param userToUserInfo  The UserToUserInformation parameter of the event.  */
    public void setUserToUserInformation(byte[] userToUserInfo) {
        m_userToUserInfo = userToUserInfo;
    }
    /** Indicates if the UserToUserInformation parameter is present in this Event.
      * @return True if the parameter has been set.  */
    public boolean isUserToUserInformationPresent() {
        return (m_userToUserInfo != null);
    }
    /** Gets the RemoteOperations parameter of the message.
      * Refer to RemoteOperations parameter for greater details.
      * @return Remote operation parameter.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.  */
    public RemoteOperations getRemoteOperations()
        throws ParameterNotSetException {
        if (m_remoteOp != null)
            return m_remoteOp;
        throw new ParameterNotSetException("RemoteOperations not set.");
    }
    /** Sets the RemoteOperations parameter of the message.
      * Refer to RemoteOperations parameter for greater details.
      * @param remoteOp  The RemoteOperations parameter of the event.  */
    public void setRemoteOperations(RemoteOperations remoteOp) {
        m_remoteOp = remoteOp;
    }
    /** Indicates if the RemoteOperations parameter is present in this Event.
      * @return True if the parameter has been set.  */
    public boolean isRemoteOperationsPresent() {
        return (m_remoteOp != null);
    }
    /** Gets the TransmissionMediumUsed parameter of the message.
      * @return The TransmissionMediumUsed parameter of the event.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.  */
    public byte getTransmissionMediumUsed()
        throws ParameterNotSetException {
        if (m_tmuIsSet)
            return m_tmu;
        throw new ParameterNotSetException("TransmissionMediumUsed not set.");
    }
    /** Sets the TransmissionMediumUsed parameter of the message.
      * @param tmu  The TransmissionMediumUsed parameter of the event.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public void setTransmissionMediumUsed(byte tmu)
        throws ParameterRangeInvalidException {
        switch (tmu) {
            case IsupConstants.TMU_SPEECH:
            case IsupConstants.TMU_64_KBPS_UNRESTRICTED:
            case IsupConstants.TMU_3DOT1_KHZ_AUDIO:
            case IsupConstants.TMU_64_KBPS_PREFERRED:
                m_tmu = tmu;
                m_tmuIsSet = true;
                return;
        }
        throw new ParameterRangeInvalidException("tmu value " + tmu + " out of range.");
    }
    /** Indicates if the TransmissionMediumUsed parameter is present in this Event.
      * @return True if the parameter has been set.  */
    public boolean isTransmissionMediumUsedPresent() {
        return m_tmuIsSet;
    }
    /** Gets the ServiceActivation parameter of the message.
      * @return The ServiceActivation parameter of the event.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.
      */
    public byte[] getServiceActivation()
        throws ParameterNotSetException {
        if (m_servAct != null)
            return m_servAct;
        throw new ParameterNotSetException("ServiceActivation not set.");
    }
    /** Sets the ServiceActivation parameter of the message.
      * @param servAct  The ServiceActivation parameter of the event.  */
    public void setServiceActivation(byte[] servAct) {
        m_servAct = servAct;
    }
    /** Indicates if the ServiceActivation parameter is present in this Event.
      * @return True if the parameter has been set.  */
    public boolean isServiceActivationPresent() {
        return (m_servAct != null);
    }
    /** The toString method retrieves a string containing the values of the members of
      * the AnswerEvent class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append("\njavax.jain.ss7.isup.AnswerEvent");
        b.append("\n\tm_callRef: " + m_callRef);
        b.append("\n\tm_bci: " + m_bci);
        b.append("\n\tm_noi: " + m_noi);
        b.append("\n\tm_obci: " + m_obci);
        b.append("\n\tm_accessTrans: ");
        if (m_accessTrans != null)
            b.append(JainSS7Utility.bytesToHex(m_accessTrans, 0, m_accessTrans.length));
        b.append("\n\tm_uui: " + m_uui);
        if (m_userToUserInfo != null)
            b.append(JainSS7Utility.bytesToHex(m_userToUserInfo, 0, m_userToUserInfo.length));
        b.append("\n\tm_remoteOp: " + m_remoteOp);
        if (m_tmuIsSet)
            b.append("\n\tm_tmu: " + m_tmu);
        if (m_servAct != null)
            b.append(JainSS7Utility.bytesToHex(m_servAct, 0, m_servAct.length));
        return b.toString();
    }
    protected CallReference m_callRef = null;
    protected BwdCallInd m_bci = null;
    protected NotificationInd m_noi = null;
    protected OptionalBwdCallInd m_obci = null;
    protected byte[] m_accessTrans = null;
    protected UserToUserIndicators m_uui = null;
    protected byte[] m_userToUserInfo = null;
    protected RemoteOperations m_remoteOp = null;
    protected byte m_tmu;
    protected boolean m_tmuIsSet = false;
    protected byte[] m_servAct = null;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
