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

/** An ISUP EVENT: The SetupEvent class is a sub class of the IsupEvent class and is
  * exchanged between an ISUP Provider and an ISUP Listener for sending and receiving the
  * ISUP Initial Address Message.
  * Listener would send a SetupEvent object to the provider for sending a IAM message to
  * the ISUP stack. ISUP Provider would send a SetupEvent object to the listener on the
  * reception of a IAM message from the stack for the user address handled by that
  * listener The primitive field is filled as ISUP_PRIMITIVE_SETUP.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract class SetupEvent extends IsupEvent {
    /** Constructs a new SetupEvent, with only the JAIN ISUP Mandatory parameters being
      * supplied to the constructor.
      * @param source  The source of this event.
      * @param dpc  The destination point code.
      * @param opc  The origination point code.
      * @param sls  The signaling link selection.
      * @param cic  The CIC on which the call has been established.
      * @param congestionPriority  Priority of the ISUP message which may be used in the
      * optional national congestion control procedures at MTP3. Refer to
      * getCongestionPriority method in IsupEvent class for more details.
      * @param glareControl  Glare control is used to indicate whether a voice circuit
      * (CIC) is controlled by a node or not.  This field is used in dual seizure
      * procedures. If an IAM is sent out by a node for a CIC and then receives an IAM for
      * the same CIC from the remote node, then if glareControl flag is set for that CIC
      * in the Setup event, the outgoing call is successful while the incoming call will
      * be backed off by the ISUP stack. And in the reverse case, when the glare control
      * flag is not set, then the outgoing call is released and a Reattempt indication
      * event is sent to the application for retrying the call on another CIC.
      * @param natureConInd  Nature of connection indicators.
      * @param forwardCallInd  Forward call indicators.
      * @param calledPartyNum  Called party number.
      * @param callingCategory  Calling party category.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    protected SetupEvent(Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
            boolean glareControl, NatureConnInd natureConInd, FwdCallInd forwardCallInd,
            CalledPartyNumber calledPartyNum, byte callingCategory)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
        this.setGlareControl(glareControl);
        this.setNatureConnInd(natureConInd);
        this.setFwdCallInd(forwardCallInd);
        this.setCalledPartyNumber(calledPartyNum);
        this.setCallingPartyCat(callingCategory);
    }
    /** Gets the ISUP SETUP primtive value.
      * @return The ISUP SETUP primitive value.
      */
    public int getIsupPrimitive() {
        return IsupConstants.ISUP_PRIMITIVE_SETUP;
    }
    /** Checks if the mandatory parameters have been set.
      * @exception MandatoryParameterNotSetException  Thrown when the mandatory parameter
      * is not set.
      */
    public void checkMandatoryParameters()
        throws MandatoryParameterNotSetException {
        super.checkMandatoryParameters();
        getNatureConnInd();
        getFwdCallInd();
        getCalledPartyNumber();
    }
    /** Gets the glare control flag value for this event.
      * Glare control is used to indicate whether a voice circuit (CIC) is controlled by a
      * node or not. This field is used in dual seizure procedures. If an IAM is sent out
      * by a node for a CIC and then receives an IAM for the same CIC from the remote
      * node, then if glareControl flag is set for that CIC in the Setup event, the
      * outgoing call is successful while the incoming call will be backed off by the ISUP
      * stack. And in the reverse case, when the glare control flag is not set, then the
      * outgoing call is released and a Reattempt indication event is sent to the
      * application for retrying the call on another CIC.
      * @return The glare control flag for this Setup Event. The value is true, if this
      * node is controlling the CIC in this Setup Event and it is false if the node is not
      * the controlling the CIC in this Setup event object.
      */
    public boolean getGlareControl() {
        return m_glareControl;
    }
    /** Sets the glare control flag value for this event.
      * @param glareControl  Glare control field in the Setup Event.
      */
    public void setGlareControl(boolean glareControl) {
        m_glareControl = glareControl;
    }
    /** Gets the Nature of Connection Indicators parameter of the message.
      * Refer to NatureConnInd parameter class for more information.
      * @return The Nature of Connection Indicators parameter of the event.
      * @exception MandatoryParameterNotSetException  Thrown when mandatory ISUP parameter
      * is not set.
      */
    public NatureConnInd getNatureConnInd()
        throws MandatoryParameterNotSetException {
        if (m_nci != null)
            return m_nci;
        throw new MandatoryParameterNotSetException("NatureConnInd not set.");
    }
    /** Sets the Nature of Connection Indicators parameter of the message.
      * Refer to NatureConnInd parameter class for more information.
      * @param nci  The  Nature of Connection Indicators parameter of the event.
      */
    public void setNatureConnInd(NatureConnInd nci) {
        m_nci =  nci;
    }
    /** Gets the Forward Call Indicators parameter of the message.
      * Refer to NatureConnInd parameter class for more information.
      * @return The Forward Call Indicators parameter of the event.
      * @exception MandatoryParameterNotSetException  Thrown when mandatory ISUP parameter
      * is not set.
      */
    public FwdCallInd getFwdCallInd()
        throws MandatoryParameterNotSetException {
        if (m_fci != null)
            return m_fci;
        throw new MandatoryParameterNotSetException("FwdCallInd not set.");
    }
    /** Sets the Forward Call Indicators parameter of the message.
      * Refer to FwdCallInd parameter class for more information.
      * @param fci  The Forward Call Indicators parameter of the event.
      */
    public void setFwdCallInd(FwdCallInd fci) {
        m_fci = fci;
    }
    /** Gets the Calling party category parameter of the message.
      * @return The Calling party category parameter of the event.
      */
    public byte getCallingPartyCat() {
        return m_cat;
    }
    /** Sets the Calling Party Category parameter of the message.
      * @param cat  The Calling Party Category parameter of the event.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setCallingPartyCat(byte cat)
        throws ParameterRangeInvalidException {
        m_cat = cat;
    }
    /** Gets the Called Party Number parameter of the message.
      * Refer to CalledPartyNumber parameter class for more information.
      * @return The Called Party Number parameter of the event.
      * @exception MandatoryParameterNotSetException  Thrown when mandatory ISUP parameter
      * is not set.
      */
    public CalledPartyNumber getCalledPartyNumber()
        throws MandatoryParameterNotSetException {
        if (m_cpn != null)
            return m_cpn;
        throw new MandatoryParameterNotSetException("CalledPartyNumber not set.");
    }
    /** Sets the Called Party Number parameter of the message.
      * Refer to CalledPartyNumber parameter class for more information.
      * @param cpn  The Called Party Number parameter of the event.
      */
    public void setCalledPartyNumber(CalledPartyNumber cpn) {
        m_cpn = cpn;
    }
    /** Gets the AccessTransport parameter of the message.
      * @return The AccessTransport parameter of the event.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.
      */
    public byte[] getAccessTransport()
        throws ParameterNotSetException {
        if (m_accessTrans != null)
            return m_accessTrans;
        throw new ParameterNotSetException("AccessTransport is not set.");
    }
    /** Sets the AccessTransport parameter of the message.
      * @param accessTrans  The AccessTransport parameter of the event.
      */
    public void setAccessTransport(byte[] accessTrans) {
        m_accessTrans = accessTrans;
    }
    /** Indicates if the AccessTransport parameter is present in this Event.
      * @return True if the parameter has been set.
      */
    public boolean isAccessTransportPresent() {
        return (m_accessTrans != null);
    }
    /** Gets the CallReference parameter of the message.
      * Refer to CallReference parameter for greater details.
      * @return The CallReference parameter of the event.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.
      */
    public CallReference getCallReference()
        throws ParameterNotSetException {
        if (m_callRef != null)
            return m_callRef;
        throw new ParameterNotSetException("CallReference is not set.");
    }
    /** Sets the CallReference parameter of the message.
      * Refer to CallReference parameter for greater details.
      * @param callRef  The CallReference parameter of the event.
      */
    public void setCallReference(CallReference callRef) {
        m_callRef = callRef;
    }
    /** Indicates if the CallReference parameter is present in this Event.
      * @return True if the parameter has been set.
      */
    public boolean isCallReferencePresent() {
        return (m_callRef != null);
    }
    /** Gets the Calling party Number parameter of the message.
      * @return The Calling party Number parameter of the event.
      * @exception ParameterNotSetException  Thrown when ISUP parameter is not set.
      */
    public CallingPartyNumber getCallingPartyNumber()
        throws ParameterNotSetException {
        if (m_callingPartyNum != null)
            return m_callingPartyNum;
        throw new ParameterNotSetException("CallingPartyNumber not set");
    }
    /** Sets the Calling Party Number parameter of the message.
      * @param callingPartyNumthe  Calling Party Number parameter of the event.
      */
    public void setCallingPartyNumber(CallingPartyNumber callingPartyNum) {
        m_callingPartyNum = callingPartyNum;
    }
    /** Indicates if the Calling Party Number parameter is present in this Event.
      * @return True if the parameter has been set.
      */
    public boolean isCallingPartyNumberPresent() {
        return (m_callingPartyNum != null);
    }
    /** Gets the ConReq parameter of the message.
      * @return The ConReq parameter of the event.
      * @exception ParameterNotSetException  Thrown when the parameter is not set in the
      * event ITU.
      */
    public ConReq getConReq()
        throws ParameterNotSetException {
        if (m_connectionReq != null)
            return m_connectionReq;
        throw new ParameterNotSetException("ConnectionRequest not set.");
    }
    /** Sets the connection request parameter.
      * @param connectionReq  Connection Request parameter.
      */
    public void setConReq(ConReq connectionReq) {
        m_connectionReq = connectionReq;
    }
    /** Indicates if the ConReq parameter is present in this Event.
      * @return True if the parameter is set.
      */
    public boolean isConReqPresent() {
        return (m_connectionReq != null);
    }
    /** Gets the GenericDigits parameter of the message.
      * Refer to GenericDigits parameter class for more information.
      * @return The GenericDigits parameter of the event.
      * @exception ParameterNotSetException  Thrown when mandatory ISUP parameter is not
      * set.
      */
    public GenericDigits getGenericDigits()
        throws ParameterNotSetException {
        if (m_gd != null)
            return m_gd;
        throw new ParameterNotSetException("GenericDigits not set.");
    }
    /** Sets the Generic Digits parameter of the message.
      * Refer to GenericDigits parameter class for more information.
      * @param nci  The Generic Digits parameter of the event.
      */
    public void setGenericDigits(GenericDigits gd) {
        m_gd = gd;
    }
    /** Indicates if the GenericDigits parameter is present in this Event.
      * @return True if the parameter has been set.
      */
    public boolean isGenericDigitsPresent() {
        return (m_gd != null);
    }
    /** Gets the GenericNumber parameter of the message.
      * Generic Number parameter is referred as Generic Address parameter in ANSI T1.113.
      * @return The GenericNumber parameter of the event.
      */
    public GenericNumber getGenericNumber()
        throws ParameterNotSetException {
        if (m_cid != null)
            return m_cid;
        throw new ParameterNotSetException("GenericNumber not set.");
    }
    /**
      */
    public void setGenericNumber(GenericNumber cid) {
        m_cid = cid;
    }
    /** Indicates if the GenericNumber parameter is present in this Event.
      * @return True if the parameter is set.
      */
    public boolean isGenericNumberPresent() {
        return (m_cid != null);
    }
    /** Gets the OrigCalledNumber parameter of the message.
      * Refer to OrigCalledNumber parameter class for more information.
      * @return The OrigCalledNumber parameter of the event.
      * @exception ParameterNotSetException  Thrown when ISUP parameter is not set.
      */
    public OrigCalledNumber getOrigCalledNumber()
        throws ParameterNotSetException {
        if (m_ocn != null)
            return m_ocn;
        throw new ParameterNotSetException("OrigCalledNumber not set.");
    }
    /** Sets the Original Called Number parameter of the message.
      * Refer to OrigCalledNumber parameter class for more information.
      * @param ocnthe  Original Called Number parameter of the event.
      */
    public void setOrigCalledNumber(OrigCalledNumber ocn) {
        m_ocn = ocn;
    }
    /** Indicates if the OrigCalledNumber parameter is present in this Event.
      * @return True if the parameter has been set.
      */
    public boolean isOrigCalledNumberPresent() {
        return (m_ocn != null);
    }
    /** Gets the MLPP precedence parameter of the message.
      * Refer to MLPPprecedence parameter for more details.
      * @return The MLPPprecedence parameter of the event.
      * @exception ParameterNotSetException  Thrown when the parameter is not set in the
      * event.
      */
    public Precedence getMLPPprecedence()
        throws ParameterNotSetException {
        if (m_precedence != null)
            return m_precedence;
        throw new ParameterNotSetException("Precedence not set.");
    }
    /** Sets the MLPPprecedence parameter of the message.
      * Refer to MLPPprecedence parameter for more details.
      * @param precedence  The MLPP Precedence parameter of the event.
      */
    public void setMLPPprecedence(Precedence precedence) {
        m_precedence = precedence;
    }
    /** Indicates if the MLPPprecedence parameter is present in this Event.
      * @return True if the parameter is set.
      */
    public boolean isMLPPprecedencePresent() {
        return (m_precedence != null);
    }
    /** Gets the Redirecting Number parameter of the message.
      * Refer to RedirectingNumber parameter class for more information.
      * @return The Redirecting Number parameter of the event.
      * @exception ParameterNotSetException  Thrown when mandatory ISUP parameter is not
      * set.
      */
    public RedirectingNumber getRedirectingNumber()
        throws ParameterNotSetException {
        if (m_rn != null)
            return m_rn;
        throw new ParameterNotSetException("RedirectingNumber not set.");
    }
    /** Sets the Redirecting Number parameter of the message.
      * Refer to RedirectingNumber parameter class for more information.
      * @param nci  The Redirecting Number parameter of the event.
      */
    public void setRedirectingNumber(RedirectingNumber rn) {
        m_rn = rn;
    }
    /** Indicates if the RedirectingNumber parameter is present in this Event.
      * @return True if the parameter has been set.
      */
    public boolean isRedirectingNumberPresent() {
        return (m_rn != null);
    }
    /** Gets the RedirectionInfo parameter of the message.
      * @return The RedirectionInfo parameter of the event.
      * @exception ParameterNotSetException  Thrown when the parameter is not set in the
      * event.
      */
    public RedirectionInfo getRedirectionInfo()
        throws ParameterNotSetException {
        if (m_ri != null)
            return m_ri;
        throw new ParameterNotSetException("RedirectionInfo not set.");
    }
    /** Sets the RedirectionInfo parameter of the message.
      * @param ri  Redirection Information parameter.
      */
    public void setRedirectionInfo(RedirectionInfo ri) {
        m_ri = ri;
    }
    /** Indicates if the RedirectionInfo parameter is present in this Event.
      * @return True if the parameter is set.
      */
    public boolean isRedirectionInfoPresent() {
        return (m_ri != null);
    }
    /** Gets the RemoteOperations parameter of the message.
      * Refer to RemoteOperations parameter for greater details.
      * @return Remote operation parameter.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.
      */
    public RemoteOperations getRemoteOperations()
        throws ParameterNotSetException {
        if (m_remoteOp != null)
            return m_remoteOp;
        throw new ParameterNotSetException("RemoteOperations not set.");
    }
    /** Sets the RemoteOperations parameter of the message.
      * Refer to RemoteOperations parameter for greater details.
      * @param remoteOp  The RemoteOperations parameter of the event.
      */
    public void setRemoteOperations(RemoteOperations remoteOp) {
        m_remoteOp = remoteOp;
    }
    /** Indicates if the RemoteOperations parameter is present in this Event.
      * @return True if the parameter has been set.
      */
    public boolean isRemoteOperationsPresent() {
        return (m_remoteOp != null);
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
      * @param servAct  The ServiceActivation parameter of the event.
      */
    public void setServiceActivation(byte[] servAct) {
        m_servAct = servAct;
    }
    /** Indicates if the ServiceActivation parameter is present in this Event.
      * @return True if the parameter has been set.
      */
    public boolean isServiceActivationPresent() {
        return (m_servAct != null);
    }
    /** Gets the Transit Network Selection parameter of the message.
      * Refer to TransitNwSel parameter class for more information.
      * @return The TransitNwSel parameter of the event.
      * @exception ParameterNotSetException  Thrown when mandatory ISUP parameter is not
      * set.
      */
    public TransitNwSel getTransitNwSel()
        throws ParameterNotSetException {
        if (m_tns != null)
            return m_tns;
        throw new ParameterNotSetException("TransitNwSel not set.");
    }
    /** Sets the Transit Network Selection parameter of the message.
      * Refer to TransitNwSel parameter class for more information.
      * @param tns  The Transit Network Selection parameter of the event.
      */
    public void setTransitNwSel(TransitNwSel tns) {
        m_tns = tns;
    }
    /** Indicates if the Transit Network Selection parameter is present in this Event.
      * @return True if the parameter has been set.
      */
    public boolean isTransitNwSelPresent() {
        return (m_tns != null);
    }
    /** Gets the UserToUserInformation parameter of the message.
      * @return The byte array for UserToUserInformation parameter of the event.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.
      */
    public byte[] getUserToUserInformation()
        throws ParameterNotSetException {
        if (m_userToUserInfo != null)
            return m_userToUserInfo;
        throw new ParameterNotSetException("UserToUserInformation not set.");
    }
    /** Sets the UserToUserInformation parameter of the message.
      * @param userToUserInfo  The UserToUserInformation parameter of the event.
      */
    public void setUserToUserInformation(byte[] userToUserInfo) {
        m_userToUserInfo = userToUserInfo;
    }
    /** Indicates if the UserToUserInformation parameter is present in this Event.
      * @return True if the parameter has been set.
      */
    public boolean isUserToUserInformationPresent() {
        return (m_userToUserInfo != null);
    }
    /** Gets the User Service Information Prime parameter of the message.
      * Refer to UserServiceInfoPrime parameter for greater details.
      * @return The UserServiceInfoPrime parameter of the event.
      * @exception ParameterNotSetException  Thrown when an ISUP parameter is not set in
      * the event.
      */
    public UserServiceInfoPrime getUserServiceInfoPrime()
        throws ParameterNotSetException {
        if (m_usip != null)
            return m_usip;
        throw new ParameterNotSetException("UserServiceInfoPrime not set.");
    }
    /** Sets the User Service Information Prime parameter of the message.
      * Refer to UserServiceInfoPrime parameter for greater details.
      * @param usip  User Service Information Prime parameter.
      */
    public void setUserServiceInfoPrime(UserServiceInfoPrime usip) {
        m_usip = usip;
    }
    /** Indicates if the UserServiceInfoPrime parameter is present in this Event.
      * @return True if the parameter is set else false.
      */
    public boolean isUserServiceInfoPrimePresent() {
        return (m_usip != null);
    }
    /** The toString method retrieves a string containing the values of the members of the
      * SetupEvent class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.SetupEvent");
        b.append("\n\tm_glareControl: " + m_glareControl);
        b.append("\n\tm_nci: " + m_nci);
        b.append("\n\tm_fci: " + m_fci);
        b.append("\n\tm_cat: " + m_cat);
        b.append("\n\tm_cpn: " + m_cpn);
        b.append("\n\tm_accessTrans: ");
        if (m_accessTrans!= null)
            b.append(JainSS7Utility.bytesToHex(m_accessTrans, 0, m_accessTrans.length));
        b.append("\n\tm_callRef: " + m_callRef);
        b.append("\n\tm_callingPartyNum: " + m_callingPartyNum);
        b.append("\n\tm_connectionReq: " + m_connectionReq);
        b.append("\n\tm_gd: " + m_gd);
        b.append("\n\tm_cid: " + m_cid);
        b.append("\n\tm_ocn: " + m_ocn);
        b.append("\n\tm_precedence: " + m_precedence);
        b.append("\n\tm_rn: " + m_rn);
        b.append("\n\tm_ri: " + m_ri);
        b.append("\n\tm_remoteOp: " + m_remoteOp);
        b.append("\n\tm_servAct: ");
        if (m_servAct!= null)
            b.append(JainSS7Utility.bytesToHex(m_servAct, 0, m_servAct.length));
        b.append("\n\tm_tns: " + m_tns);
        b.append("\n\tm_userToUserInfo: ");
        if (m_userToUserInfo!= null)
            b.append(JainSS7Utility.bytesToHex(m_userToUserInfo, 0, m_userToUserInfo.length));
        b.append("\n\tm_usip: " + m_usip);
        return b.toString();
    }
    protected boolean m_glareControl;
    protected NatureConnInd m_nci;
    protected FwdCallInd m_fci;
    protected byte m_cat;
    protected CalledPartyNumber m_cpn;
    protected byte[] m_accessTrans;
    protected CallReference m_callRef;
    protected CallingPartyNumber m_callingPartyNum;
    protected ConReq m_connectionReq;
    protected GenericDigits m_gd;
    protected GenericNumber m_cid;
    protected OrigCalledNumber m_ocn;
    protected Precedence m_precedence;
    protected RedirectingNumber m_rn;
    protected RedirectionInfo m_ri;
    protected RemoteOperations m_remoteOp;
    protected byte[] m_servAct;
    protected TransitNwSel m_tns;
    protected byte[] m_userToUserInfo;
    protected UserServiceInfoPrime m_usip;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
