/*
 @(#) src/java/javax/jain/ss7/isup/itu/ConnectItuEvent.java <p>
 
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

package javax.jain.ss7.isup.itu;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** An ISUP EVENT: The ConnectItuEvent class is a sub class of the IsupEvent class and is
    exchanged between an ISUP Provider and an ISUP Listener for sending and receiving the
    ISUP Connect Message.
    Listener would send an EventConnect object to the provider for sending an CON message
    to the ISUP stack. ISUP Provider would send an EventConnect object to the listener on
    the reception of an CON message from the stack for the user address handled by that
    listener. The mandatory parameters are supplied to the constructor. Optional
    parameters may then be set using the set methods The primitive field is filled as
    ISUP_PRIMITIVE_CONNECT.
    @version 1.2.2
    @author Monavacon Limited
  */
public class ConnectItuEvent extends IsupEvent {
    /** Constructs a new ConnectItuEvent, with only the JAIN ISUP Mandatory parameters
        being supplied to the constructor.
        @param source  The source of this event.
        @param dpc  The destination point code.
        @param opc  The origination point code.
        @param sls  The signaling link selection.
        @param cic  The CIC on which the call has been established.
        @param congestionPriority  Priority of the ISUP message which may be used in the
        optional national congestion control procedures at MTP3. Refer to
        getCongestionPriority method in IsupEvent class for more details.
        @param bci  Backward call indicators.
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public ConnectItuEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
            BwdCallIndItu bci)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
        this.setBwdCallInd(bci);
    }
    /** Gets the ISUP CONNECT primtive value.
        @return The ISUP CONNECT primitive value.
      */
    public int getIsupPrimitive() {
        return IsupConstants.ISUP_PRIMITIVE_CONNECT;
    }
    /** Checks if the mandatory parameters have been set.
        @exception MandatoryParameterNotSetException  Thrown when the mandatory parameter
        is not set.
      */
    public void checkMandatoryParameters()
        throws MandatoryParameterNotSetException {
        super.checkMandatoryParameters();
        getBwdCallInd();
    }
    /** Gets the BackwarCallIndicators parameter of the message.
        Refer to BwdCallIndItu parameter for greater details.
        @return The BackwarCallIndicators parameter of the event.
      */
    public BwdCallIndItu getBwdCallInd()
        throws MandatoryParameterNotSetException {
        if (m_bci != null)
            return m_bci;
        throw new MandatoryParameterNotSetException("BwdCallInd not set.");
    }
    /** Sets the BackwarCallIndicators parameter of the message.
        Refer to BwdItuCallInd parameter for greater details.
        @param bci  BackwarCallIndicators parameter of the event.
      */
    public void setBwdCallInd(BwdCallIndItu bci) {
        m_bci = bci;
    }
    /** Gets the AccessTransport parameter of the message.
        @return The AccessTransport parameter of the event.
        @exception ParameterNotSetException  Thrown when the optional parameter is not
        set.
      */
    public byte[] getAccessTransport()
        throws ParameterNotSetException {
        if (m_accessTrans != null)
            return m_accessTrans;
        throw new ParameterNotSetException("AccessTransport not set.");
    }
    /** Sets the AccessTransport parameter of the message.
        @param accessTrans  The AccessTransport parameter of the event.
      */
    public void setAccessTransport(byte[] accessTrans) {
        m_accessTrans = accessTrans;
    }
    /** Indicates if the AccessTransport parameter is present in this Event.
        @return True if the parameter has been set.
      */
    public boolean isAccessTransportPresent() {
        return (m_accessTrans != null);
    }
    /** Gets the AccessDeliveryInfo parameter of the message.
        @return The AccessDeliveryInfo parameter of the event.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public byte getAccessDeliveryInfo()
        throws ParameterNotSetException {
        if (m_adiIsSet)
            return m_adi;
        throw new ParameterNotSetException("AcessDeliveryInfo not set.");
    }
    /** Sets the Access Delivery Information parameter of the message.
        @param adi  Access delivery information parameter of the event.
      */
    public void setAccessDeliveryInfo(byte adi) {
        m_adi = adi;
        m_adiIsSet = true;
    }
    /** Indicates if the AccessDeliveryInfo parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isAccessDeliveryInfoPresent() {
        return m_adiIsSet;
    }
    /** Gets the CallHistoryInfo parameter of the message.
        @return The CallHistoryInfo parameter of the event, range 0 to 0xffffffff.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public long getCallHistoryInfo()
        throws ParameterNotSetException {
        if (m_callHistInfoIsSet)
            return m_callHistInfo;
        throw new ParameterNotSetException("CallHistoryInfo not set.");
    }
    /** Sets the Call History Information parameter in the message.
        @param callHistInfo  Call History Information parameter of the event, range 0 to
        0xffffffff.
      */
    public void setCallHistoryInfo(long callHistInfo)
        throws ParameterRangeInvalidException {
        if (0 <= callHistInfo && callHistInfo <= 0xffffff) {
            m_callHistInfo = callHistInfo;
            m_callHistInfoIsSet = true;
            return;
        }
        throw new ParameterRangeInvalidException("CallHistInfo value " + callHistInfo + " out of range.");
    }
    /** Indicates if the CallHistoryInfo parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isCallHistoryInfoPresent() {
        return m_callHistInfoIsSet;
    }
    /** Gets the ConnectedNumberItu parameter of the message.
        @return The ConnectedNumberItu parameter of the event.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public ConnectedNumberItu getConnectedNumber()
        throws ParameterNotSetException {
        if (m_connNum != null)
            return m_connNum;
        throw new ParameterNotSetException("ConnectedNumberItu not set.");
    }
    /** Sets the Call History Information parameter in the message.
        @param connNum  Connected Number parameter of the event.
      */
    public void setConnectedNumber(ConnectedNumberItu connNum) {
        m_connNum = connNum;
    }
    /** Indicates if the ConnectedNumberItu parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isConnectedNumberPresent() {
        return (m_connNum != null);
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
    /** Gets the EchoControlInfoItu parameter of the message.
        @return The EchoControlInfoItu parameter of the event.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public EchoControlInfoItu getEchoControlInfo()
        throws ParameterNotSetException {
        if (m_eci != null)
            return m_eci;
        throw new ParameterNotSetException("EchoControlInfoItu not set.");
    }
    /** Sets the EchoControlInfoItu parameter of the message.
        @param eci  The EchoControlInfoItu parameter of the event.
      */
    public void setEchoControlInfo(EchoControlInfoItu eci) {
        m_eci = eci;
    }
    /** Indicates if the EchoControlInfoItu parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isEchoControlInfoPresent() {
        return (m_eci != null);
    }
    /** Gets the GenericNumberItu parameter of the message.
        @return The GenericNumberItu parameter of the event.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public GenericNumberItu getGenericNumber()
        throws ParameterNotSetException {
        if (m_genNum != null)
            return m_genNum;
        throw new ParameterNotSetException("GenericNumberItu not set.");
    }
    /** Sets the GenericNumberItu parameter of the message.
        @param genNum  Generic Number parameter of the event.
      */
    public void setGenericNumber(GenericNumberItu genNum) {
        m_genNum = genNum;
    }
    /** Indicates if the GenericNumberItu parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isGenericNumberPresent() {
        return (m_genNum != null);
    }
    /** Gets the NotificationInd parameter of the message.
        @return The NotificationInd parameter of the event.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public NotificationInd getNotificationInd()
        throws ParameterNotSetException {
        if (m_noi != null)
            return m_noi;
        throw new ParameterNotSetException("NotificationInd not set.");
    }
    /** Sets the NotificationInd parameter of the message.
        @param noi  Notification Indicator parameter.
      */
    public void setNotificationInd(NotificationInd noi) {
        m_noi = noi;
    }
    /** Indicates if the NotificationInd parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isNotificationIndPresent() {
        return (m_noi != null);
    }
    /** Gets the NwSpecificFacItu parameter of the message.
        @return The NwSpecificFacItu parameter of the event.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public NwSpecificFacItu getNwSpecificFac()
        throws ParameterNotSetException {
        if (m_nsf != null)
            return m_nsf;
        throw new ParameterNotSetException("NwSpecificFacItu not set.");
    }
    /** Sets the NwSpecificFacItu parameter of the message.
        @param nsf  The NwSpecificFacItu parameter of the event.
      */
    public void setNwSpecificFac(NwSpecificFacItu nsf) {
        m_nsf = nsf;
    }
    /** Indicates if the NwSpecificFacItu parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isNwSpecificFacPresent() {
        return (m_nsf != null);
    }
    /** Gets the Optional Backward call indicator parameter of the message.
        Refer to OptionalBwdCallIndItu parameter for greater details.
        @return The OptionalBwdCallIndItu parameter of the event.
        @exception ParameterNotSetException  Thrown when the optional parameter is not
        set.
      */
    public OptionalBwdCallIndItu getOptionalBwdCallInd()
        throws ParameterNotSetException {
        if (m_obci != null)
            return m_obci;
        throw new ParameterNotSetException("OptionalBwdCallIndItu not set.");
    }
    /** Sets the Optional Backward call indicator parameter of the message.
        Refer to OptionalBwdCallIndItu parameter for greater details.
        @param obci  The Optional Backward Call Indicator parameter of the event.
      */
    public void setOptionalBwdCallInd(OptionalBwdCallIndItu obci) {
        m_obci = obci;
    }
    /** Indicates if the OptionalBwdCallIndItu parameter is present in this Event.
        @return True if the parameter has been set.
      */
    public boolean isOptionalBwdCallIndPresent() {
        return (m_obci != null);
    }
    /** Gets the ParamCompatibilityInfoItu parameter of the message.
        @return The ParamCompatibilityInfoItu parameter of the event.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public ParamCompatibilityInfoItu getParamCompatibilityInfo()
        throws ParameterNotSetException {
        if (m_pci != null)
            return m_pci;
        throw new ParameterNotSetException("ParamCompatibilityInfoItu not set.");
    }
    /** Sets the ParamCompatibilityInfoItu parameter of the message.
        @param pci  The ParamCompatibilityInfoItu parameter of the event.
      */
    public void setParamCompatibilityInfo(ParamCompatibilityInfoItu pci) {
        m_pci = pci;
    }
    /** Indicates if the ParamCompatibilityInfoItu parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isParamCompatibilityInfoPresent() {
        return (m_pci != null);
    }
    /** Gets the RedirectionNumberItu parameter of the message.
        @return The RedirectionNumberItu parameter of the event.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public RedirectionNumberItu getRedirectionNumber()
        throws ParameterNotSetException {
        if (m_rn != null)
            return m_rn;
        throw new ParameterNotSetException("ReidretionNumberItu not set.");
    }
    /** Sets the RedirectionNumberItu parameter of the message.
        @param rn  The RedirectionNumberItu parameter of the event.
      */
    public void setRedirectionNumber(RedirectionNumberItu rn) {
        m_rn = rn;
    }
    /** Indicates if the RedirectionNumberItu parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isRedirectionNumberPresent() {
        return (m_rn != null);
    }
    /** Gets the RedirectionNumberRest parameter of the message.
        @return The RedirectionNumberRest parameter of the event.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public byte getRedirectionNumberRest()
        throws ParameterNotSetException {
        if (m_rnrIsSet)
            return m_rnr;
        throw new ParameterNotSetException("RedirectionNumberRest not set.");
    }
    /** Sets the RedirectionNumberRest parameter of the message.
        @param rnr  The RedirectionNumberRest parameter of the event.
      */
    public void setRedirectionNumberRest(byte rnr) {
        m_rnr = rnr;
        m_rnrIsSet = true;
    }
    /** Indicates if the RedirectionNumberRest parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isRedirectionNumberRestPresent() {
        return m_rnrIsSet;
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
    }
    /** Indicates if the ServiceActivation parameter is present in this Event.
        @return True if the parameter has been set.
      */
    public boolean isServiceActivationPresent() {
        return (m_servAct != null);
    }
    /** Gets the TransmissionMediumUsed parameter of the message.
        @return The TransmissionMediumUsed parameter of the event.
        @exception ParameterNotSetException  Thrown when the optional parameter is not
        set.
      */
    public byte getTransmissionMediumUsed()
        throws ParameterNotSetException {
        if (m_tmuIsSet)
            return m_tmu;
        throw new ParameterNotSetException("TransmissionMediumUsed not set.");
    }
    /** Sets the TransmissionMediumUsed parameter of the message.
        @param tmu  The TransmissionMediumUsed parameter of the event.
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
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
        throw new ParameterRangeInvalidException("TransmissionMediumUsed value " + tmu + " out of range.");
    }
    /** Indicates if the TransmissionMediumUsed parameter is present in this Event.
        @return True if the parameter has been set.
      */
    public boolean isTransmissionMediumUsedPresent() {
        return m_tmuIsSet;
    }
    /** Gets the UserToUserIndicatorsItu parameter of the message.
        Refer to UserToUserIndicatorsItu parameter for greater details.
        @return The UserToUserIndicatorsItu parameter of the event.
        @exception ParameterNotSetException  Thrown when the optional parameter is not
        set.
      */
    public UserToUserIndicatorsItu getUserToUserIndicators()
        throws ParameterNotSetException {
        if (m_uui != null)
            return m_uui;
        throw new ParameterNotSetException("UserToUserIndicatorsItu not set.");
    }
    /** Sets the UserToUserIndicatorsItu parameter of the message.
        Refer to UserToUserIndicatorsItu parameter for greater details.
        @param uui  The UserToUserIndicatorsItu parameter of the event.
      */
    public void setUserToUserIndicators(UserToUserIndicatorsItu uui) {
        m_uui = uui;
    }
    /** Indicates if the UserToUserIndicatorsItu parameter is present in this Event.
        @return True if the parameter has been set.
      */
    public boolean isUserToUserIndicatorsPresent() {
        return (m_uui != null);
    }
    /** Gets the UserToUserInformation parameter of the message.
        @return The byte array for UserToUserInformation parameter of the event.
        @exception ParameterNotSetException  Thrown when the optional parameter is not
        set.
      */
    public byte[] getUserToUserInformation()
        throws ParameterNotSetException {
        if (m_userToUserInfo != null)
            return m_userToUserInfo;
        throw new ParameterNotSetException("UserToUserInformation not set.");
    }
    /** Sets the UserToUserInformation parameter of the message.
        @param userToUserInfo  The UserToUserInformation parameter of the event.
      */
    public void setUserToUserInformation(byte[] userToUserInfo) {
        m_userToUserInfo = userToUserInfo;
    }
    /** Indicates if the UserToUserInformation parameter is present in this Event.
        @return True if the parameter has been set.
      */
    public boolean isUserToUserInformationPresent() {
        return (m_userToUserInfo != null);
    }
    /** The toString method retrieves a string containing the values of the members of the
        ConnectItuEvent class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.itu.ConnectItuEvent");
        b.append("\n\tm_bci:" + m_bci);
        b.append("\n\tm_accessTrans:");
        if (m_accessTrans!= null)
            b.append(JainSS7Utility.bytesToHex(m_accessTrans, 0, m_accessTrans.length));
        if (m_adiIsSet)
            b.append("\n\tm_adi:" + m_adi);
        if (m_callHistInfoIsSet)
            b.append("\n\tm_callHistInfo:" + m_callHistInfo);
        b.append("\n\tm_connNum:" + m_connNum);
        b.append("\n\tm_callRef:" + m_callRef);
        b.append("\n\tm_eci:" + m_eci);
        b.append("\n\tm_genNum:" + m_genNum);
        b.append("\n\tm_noi:" + m_noi);
        b.append("\n\tm_nsf:" + m_nsf);
        b.append("\n\tm_obci:" + m_obci);
        b.append("\n\tm_pci:" + m_pci);
        b.append("\n\tm_rn:" + m_rn);
        if (m_rnrIsSet)
            b.append("\n\tm_rnr:" + m_rnr);
        b.append("\n\tm_remoteOp:" + m_remoteOp);
        b.append("\n\tm_servAct:");
        if (m_servAct!= null)
            b.append(JainSS7Utility.bytesToHex(m_servAct, 0, m_servAct.length));
        if (m_tmuIsSet)
            b.append("\n\tm_tmu:" + m_tmu);
        b.append("\n\tm_uui:" + m_uui);
        b.append("\n\tm_userToUserInfo:");
        if (m_userToUserInfo!= null)
            b.append(JainSS7Utility.bytesToHex(m_userToUserInfo, 0, m_userToUserInfo.length));
        return b.toString();
    }
    protected BwdCallIndItu m_bci = null;
    protected byte[] m_accessTrans = null;
    protected byte m_adi;
    protected boolean m_adiIsSet = false;
    protected long m_callHistInfo;
    protected boolean m_callHistInfoIsSet = false;
    protected ConnectedNumberItu m_connNum = null;
    protected CallReference m_callRef = null;
    protected EchoControlInfoItu m_eci = null;
    protected GenericNumberItu m_genNum = null;
    protected NotificationInd m_noi = null;
    protected NwSpecificFacItu m_nsf = null;
    protected OptionalBwdCallIndItu m_obci = null;
    protected ParamCompatibilityInfoItu m_pci = null;
    protected RedirectionNumberItu m_rn = null;
    protected byte m_rnr;
    protected boolean m_rnrIsSet = false;
    protected RemoteOperations m_remoteOp = null;
    protected byte[] m_servAct = null;
    protected byte m_tmu;
    protected boolean m_tmuIsSet = false;
    protected UserToUserIndicatorsItu m_uui = null;
    protected byte[] m_userToUserInfo = null;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
