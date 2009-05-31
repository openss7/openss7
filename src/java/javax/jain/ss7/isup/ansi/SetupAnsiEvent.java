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

/** An ISUP Event: The SetupAnsiEvent class is a sub class of ISUP Core SetupEvent class
    and is to handle the ANSI variant of IAM message.
    @author Monavacon Limited
    @version 1.2.2
  */
public class SetupAnsiEvent extends SetupEvent {
    /** Constructs a new SetupAnsiEvent, with only the JAIN ISUP Mandatory parameters
        being supplied to the constructor, using the super class's constructor.
        @param source  The source of this event.
        @param primitive  Primitive value for StartResetInd is ISUP_PRIMITIVE_SETUP. Refer
        to IsupEvent class for more details.
        @param dpc  The destination point code.
        @param opc  The origination point code.
        @param sls  The signaling link selection.
        @param cic  The CIC on which the call has been established.
        @param congestionPriority  Priority of the ISUP message which may be used in the
        optional national congestion control procedures at MTP3. Refer to
        getCongestionPriority method in IsupEvent class for more details.
        @param glareControl  Glare control is used to indicate whether a voice circuit
        (CIC) is controlled by a node or not.  This field is used in dual seizure
        procedures. If an IAM is sent out by a node for a CIC and then receives an IAM for
        the same CIC from the remote node, then if glareControl flag is set for that CIC
        in the Setup event, the outgoing call is successful while the incoming call will
        be backed off by the ISUP stack. And in the reverse case, when the glare control
        flag is not set, then the outgoing call is released and a Reattempt indication
        event is sent to the application for retrying the call on another CIC.
        @param nci  Nature of connection indicators.
        @param fci  Forward call indicators.
        @param cpn  Called party number.
        @param cat  Calling party category.
        @param usi  User service information.
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public SetupAnsiEvent(java.lang.Object source, SignalingPointCode dpc, SignalingPointCode
            opc, byte sls, int cic, byte congestionPriority, boolean glareControl,
            NatureConnInd nci, FwdCallIndAnsi fci, CalledPartyNumber cpn, byte cat,
            UserServiceInfo usi)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority, glareControl, nci, fci, cpn, cat);
        this.setUserServiceInfo(usi);
    }
    /** Checks if the mandatory parameters have been set.
        @exception MandatoryParameterNotSetException  Thrown when the mandatory parameter
        is not set.
      */
    public void checkMandatoryParameters()
        throws MandatoryParameterNotSetException {
        super.checkMandatoryParameters();
        getUserServiceInfo();
    }
    /** Gets the User Service Information parameter of the message.
        Refer to UserServiceInfo parameter for greater details.
        @return The UserServiceInfo parameter of the event.
        @exception MandatoryParameterNotSetException  Thrown when the mandatory ISUP
        parameter is not set in the event.
      */
    public UserServiceInfo getUserServiceInfo()
        throws MandatoryParameterNotSetException {
        if (m_usi != null)
            return m_usi;
        throw new MandatoryParameterNotSetException("UserServiceInfo not set.");
    }
    /** Sets the User Service Information parameter of the message.
        Refer to UserServiceInfo parameter for greater details.
        @param usi  User Service Information parameter.
      */
    public void setUserServiceInfo(UserServiceInfo usi) {
        m_usi = usi;
    }
    /** Gets the BusinessGroupAnsi parameter of the message.
        @return The BusinessGroupAnsi parameter of the event.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public BusinessGroupAnsi getBusinessGroup()
        throws ParameterNotSetException {
        if (m_bgp != null)
            return m_bgp;
        throw new ParameterNotSetException("BusinessGroupAnsi not set.");
    }
    /** Sets the BusinessAnsi Group parameter of the message.
        @param bgp  Business group parameter.
      */
    public void setBusinessGroup(BusinessGroupAnsi bgp) {
        m_bgp = bgp;
    }
    /** Indicates if the BusinessGroupAnsi parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isBusinessGroupPresent() {
        return (m_bgp != null);
    }
    /** Gets the CarrierIdAnsi parameter of the message.
        Refer to CarrierIdAnsi parameter for greater details.
        @return The CarrierIdAnsi parameter of the event.
        @exception ParameterNotSetException  Thrown when an ISUP parameter is not set in
        the event.
      */
    public CarrierIdAnsi getCarrierId()
        throws ParameterNotSetException {
        if (m_cid != null)
            return m_cid;
        throw new ParameterNotSetException("CarrierIdAnsi not set.");
    }
    /** Sets the Carrier Identity parameter of the message.
        Refer to CarrierIdAnsi parameter for greater details.
        @param cid  Carrier Identity parameter.
      */
    public void setCarrierId(CarrierIdAnsi cid) {
        m_cid = cid;
    }
    /** Indicates if the CarrierIdAnsi parameter is present in this Event.
        @return True if the parameter is set else false.
      */
    public boolean isCarrierIdPresent() {
        return (m_cid != null);
    }
    /** Gets the Carrier Selection Information parameter of the message.
        @return The CarrierSelInfoAnsi parameter of the event, range 0 to 255.
        @exception ParameterNotSetException  Thrown when an ISUP parameter is not set in the event.
      */
    public short getCarrierSelInfo()
        throws ParameterNotSetException {
        if (m_csiIsSet)
            return m_csi;
        throw new ParameterNotSetException("CarrierSelInfo not set.");
    }
    /** Sets the Carrier Selection Information parameter of the message.
        @param csi  Carrier Selection Information parameter, range 0 to 255.
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setCarrierSelInfo(short csi)
        throws ParameterRangeInvalidException {
        if (0 <= csi && csi <= 255) {
            m_csi = csi;
            m_csiIsSet = true;
            return;
        }
        throw new ParameterRangeInvalidException("CarrierSelInfo value " + csi + " out of range.");
    }
    /** Indicates if the CarrierSelInfo parameter is present in this Event.
        @return True if the parameter is set else false.
      */
    public boolean isCarrierSelInfoPresent() {
        return m_csiIsSet;
    }
    /** Gets the ChargeNumberAnsi parameter of the message.
        Refer to ChargeNumberAnsi parameter for greater details.
        @return The ChargeNumber parameter of the event.
        @exception ParameterNotSetException  Thrown when the optional parameter is not set.
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
    /** Gets the Circuit Assign Map parameter of the message.
        Refer to CircuitAssignMapAnsi parameter for greater details.
        @return The CircuitAssignMapAnsi parameter of the event.
        @exception ParameterNotSetException  Thrown when an ISUP parameter is not set in the event.
      */
    public CircuitAssignMapAnsi getCircuitAssignMap()
        throws ParameterNotSetException {
        if (m_cam != null)
            return m_cam;
        throw new ParameterNotSetException("CircuitAssignMapAnsi not set.");
    }
    /** Sets the Circuit Assign Map parameter of the message.
        Refer to CircuitAssignMapAnsi parameter for greater details.
        @param usi  Circuit Assign Map parameter.
      */
    public void setCircuitAssignMap(CircuitAssignMapAnsi cam) {
        m_cam = cam;
    }
    /** Indicates if the CircuitAssignMapAnsi parameter is present in this Event.
        @return True if the parameter is set else false.
      */
    public boolean isCircuitAssignMapPresent() {
        return (m_cam != null);
    }
    /** Gets the EgressService parameter of the message which is an array of bytes not
        interpreted by the Provider/Stack Object.
        @return The EgressService parameter of the event.
        @exception ParameterNotSetException  Thrown when an ISUP parameter is not set in
        the event.
      */
    public byte[] getEgressService()
        throws ParameterNotSetException {
        if (m_es != null)
            return m_es;
        throw new ParameterNotSetException("EgressService not set.");
    }
    /** Sets the Egress Service parameter to the value provided by the Listener
        implementation object.
        @param es  Egress Service parameter.
      */
    public void setEgressService(byte[] es) {
        m_es = es;
    }
    /** Indicates if the EgressService parameter is present in this Event.
        @return True if the parameter is set else false.
      */
    public boolean isEgressServicePresent() {
        return (m_es != null);
    }
    /** Gets the GenericNameAnsi parameter of the message.
        Refer to GenericNameAnsi parameter for greater details.
        @return The GenericNameAnsi parameter of the event.
        @exception ParameterNotSetException  Thrown when an ISUP parameter is not set in
        the event.
      */
    public GenericNameAnsi getGenericName()
        throws ParameterNotSetException {
        if (m_genNameAnsi != null)
            return m_genNameAnsi;
        throw new ParameterNotSetException("GenericNameAnsi not set.");
    }
    /** Sets the Generic Name parameter of the message.
        Refer to GenericName parameter for more details.
        @param genNameAnsi  Generic Name parameter.
      */
    public void setGenericName(GenericNameAnsi genNameAnsi) {
        m_genNameAnsi = genNameAnsi;
    }
    /** Indicates if the GenericNameAnsi parameter is present in this Event.
        @return True if the parameter is set else false.
      */
    public boolean isGenericNamePresent() {
        return (m_genNameAnsi != null);
    }
    /** Gets the HopCounter parameter of the message which has a range of 0-31.
        @return The HopCounter parameter, range 0 to 31..
      */
    public byte getHopCounter()
        throws ParameterNotSetException {
        if (m_hcIsSet)
            return m_hc;
        throw new ParameterNotSetException("HopCounter not set.");
    }
    /** Sets the Hop Counter parameter of the message.
        @param hc  Hop Counter parameter.
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setHopCounter(byte hc)
        throws ParameterRangeInvalidException {
        if (0 <= hc && hc <= 31) {
            m_hc = hc;
            m_hcIsSet = true;
            return;
        }
        throw new ParameterRangeInvalidException("HopCounter value " + hc + " out of range.");
    }
    /** Indicates if the HopCounter parameter is present in this Event.
        @return True if the parameter is set else false.
      */
    public boolean isHopCounterPresent() {
        return m_hcIsSet;
    }
    /** Gets the Information Request Indicators parameter of the message.
        Refer to InfoReqIndAnsi parameter for greater details.
        @return The Information Request Indicators parameter of the event.
        @exception this  Exception is thrown if ITU.
      */
    public InfoReqIndAnsi getInfoReqInd()
        throws ParameterNotSetException {
        if (m_informationReqIndAnsi != null)
            return m_informationReqIndAnsi;
        throw new ParameterNotSetException("InfoReqIndAnsi not set.");
    }
    /** Sets the Information Request Indicators parameter of the message.
        Refer to InfoReqIndAnsi parameter for greater details.
        @param infomationReqIndAnsi  Information Request Indicators parameter.
      */
    public void setInfoReqInd(InfoReqIndAnsi informationReqIndAnsi) {
            m_informationReqIndAnsi = informationReqIndAnsi;
    }
    /** Indicates if the InfoReqIndAnsi parameter is present in this Event.
        @return True if the parameter is set else false.
      */
    public boolean isInfoReqIndPresent() {
        return (m_informationReqIndAnsi != null);
    }
    /** Gets the Jurisdiction parameter of the message which is an array of bytes not
        interpreted by the JainIsupProviderImpl object.
        Each digit is represented as one byte, e.g. number 9876543210 can be represented
        as jurisdiction[0]=9, jurisdiction[1]=8, jurisdiction[2]=7, and so on.
        @return The Jurisdiction parameter of the event.
        @exception ParameterNotSetException  Thrown when an ISUP parameter is not set in
        the event.
      */
    public byte[] getJurisdiction()
        throws ParameterNotSetException {
        if (m_jurisdict != null)
            return m_jurisdict;
        throw new ParameterNotSetException("Jurisdiction not set.");
    }
    /** Sets the Jurisdiction parameter of the message.
        Each digit is represented as one byte, e.g. number 9876543210 can be represented
        as jurisdiction[0]=9, jurisdiction[1]=8, jurisdiction[2]=7, and so on.
        @param jurisdict  Jurisdiction parameter.
      */
    public void setJurisdiction(byte[] jurisdict) {
            m_jurisdict = jurisdict;
    }
    /** Indicates if the Jurisdiction parameter is present in this Event.
        @return True if the parameter is set else false.
      */
    public boolean isJurisdictionPresent() {
        return (m_jurisdict != null);
    }
    /** Gets the NetworkTransport parameter of the message.
        @return The NetworkTransport parameter of the event. This is a byte array, the way
        in which the byte array is filled is :- Fill the Name of the parameter, the length
        of the parameter and the parameter contents. This is repeated if multiple
        parameters are to be sent.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public byte[] getNetworkTransport()
        throws ParameterNotSetException {
        if (m_ntp != null)
            return m_ntp;
        throw new ParameterNotSetException("NetworkTransport not set.");
    }
    /** Sets the NetworkTransport parameter of the message.
        @param ntp  Network Transport parameter. This is a byte array, the way in which
        the byte array is filled is :- Fill the Name of the parameter, the length of the
        parameter and the parameter contents. This is repeated if multiple parameters are
        to be sent.
      */
    public void setNetworkTransport(byte[] ntp) {
            m_ntp = ntp;
    }
    /** Indicates if the NetworkTransport parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isNetworkTransportPresent() {
        return (m_ntp != null);
    }
    /** Gets the OperatorServicesInfoAnsi parameter of the message.
        Refer to OperatorServicesInfoAnsi parameter for greater details.
        @return The OperatorServicesInfoAnsi parameter of the event.
        @exception ParameterNotSetException  Thrown when an ISUP parameter is not set in
        the event.
      */
    public OperatorServicesInfoAnsi getOperatorServicesInfo()
        throws ParameterNotSetException {
        if (m_osi != null)
            return m_osi;
        throw new ParameterNotSetException("OperatorServicesInfoAnsi not set.");
    }
    /** Sets the Operator Services Information parameter of the message.
        Refer to OperatorServicesInfoAnsi parameter for greater details.
        @param osi  Operator Services Information parameter.
      */
    public void setOperatorServicesInfo(OperatorServicesInfoAnsi osi) {
        m_osi = osi;
    }
    /** Indicates if the OperatorServicesInfoAnsi parameter is present in this Event.
        @return True if the parameter is set else false.
      */
    public boolean isOperatorServicesInfoPresent() {
        return (m_osi != null);
    }
    /** Gets the Originating Line Information parameter of the message.
        @return The OrigLineInfo parameter of the event.
        @exception this  Exception is thrown if ITU.
      */
    public byte getOrigLineInfo()
        throws ParameterNotSetException {
        if (m_oliIsSet)
            return m_oli;
        throw new ParameterNotSetException("OrigLineInfo not set.");
    }
    /** Sets the Originating Line Information parameter of the message.
        @param oli  Originating Line Information parameter.
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setOrigLineInfo(byte oli)
        throws ParameterRangeInvalidException {
        m_oli = oli;
        m_oliIsSet = true;
    }
    /** Indicates if the OrigLineInfo parameter is present in this Event.
        @return True if the parameter is set else false.
      */
    public boolean isOrigLineInfoPresent() {
        return m_oliIsSet;
    }
    /** Gets the ServiceCode parameter of the message in the range 0 to 255.
        @return The ServiceCode parameter of the event, range 0 to 255.
        @exception ParameterNotSetException  Thrown when an ISUP parameter is not set in
        the event.
      */
    public short getServiceCode()
        throws ParameterNotSetException {
        if (m_scIsSet)
            return m_sc;
        throw new ParameterNotSetException("ServiceCode not set.");
    }
    /** Sets the Service Code parameter of the message.
        @param sc  Service Code parameter, range 0 to 255.
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setServiceCode(short sc)
        throws ParameterRangeInvalidException {
        if (0 <= sc && sc <= 255) {
            m_sc = sc;
            m_scIsSet = true;
            return;
        }
        throw new ParameterRangeInvalidException("ServiceCode value " + sc + " out of range.");
    }
    /** Indicates if the ServiceCode parameter is present in this Event.
        @return True if the parameter is set else false.
      */
    public boolean isServiceCodePresent() {
        return m_scIsSet;
    }
    /** Gets the SpecialProcessingReq parameter of the message.
        @return The SpecialProcessingReq parameter of the event.
        @exception ParameterNotSetException  Thrown when an ISUP parameter is not set in
        the event.
      */
    public short getSpecialProcReq()
        throws ParameterNotSetException {
        if (m_sprIsSet)
            return m_spr;
        throw new ParameterNotSetException("SpecialProcReq not set.");
    }
    /** Gets the Special Processing Request parameter of the message.
        @param spr  Special Processing Request parameter.
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setSpecialProcReq(short spr)
        throws ParameterRangeInvalidException {
        m_spr = spr;
        m_sprIsSet = true;
    }
    /** Indicates if the SpecialProcessingReq parameter is present in this Event.
        @return True if the parameter is set else false.
      */
    public boolean isSpecialProcReqPresent() {
        return m_sprIsSet;
    }
    /** Gets the Transaction Request parameter of the message.
        Refer to TransactionReqAnsi parameter for greater details.
        @return The TransactionReqAnsi parameter of the event.
        @exception ParameterNotSetException  Thrown when an ISUP parameter is not set in
        the event.
      */
    public TransactionReqAnsi getTransactionReq()
        throws ParameterNotSetException {
        if (m_tr != null)
            return m_tr;
        throw new ParameterNotSetException("TrnansactionReqAnsi not set.");
    }
    /** Sets the Transaction Request parameter of the message.
        Refer to TransactionReqAnsi parameter for greater details.
        @param tr  Transaction Request parameter.
      */
    public void setTransactionReq(TransactionReqAnsi tr) {
        m_tr = tr;
    }
    /** Indicates if the TransactionReqAnsi parameter is present in this Event.
        @return True if the parameter is set else false.
      */
    public boolean isTransactionReqPresent() {
        return (m_tr != null);
    }
    /** The toString method retrieves a string containing the values of the members of the
        SetupAnsiEvent class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.ansi.SetupAnsiEvent");
        b.append("\n\tm_usi: " + m_usi);
        b.append("\n\tm_bgp: " + m_bgp);
        b.append("\n\tm_cid: " + m_cid);
        if (m_csiIsSet)
            b.append("\n\tm_csi: " + m_csi);
        b.append("\n\tm_chargeNumAnsi: " + m_chargeNumAnsi);
        b.append("\n\tm_cam: " + m_cam);
        b.append("\n\tm_es: ");
        if (m_es!= null)
            b.append(JainSS7Utility.bytesToHex(m_es, 0, m_es.length));
        b.append("\n\tm_genNameAnsi: " + m_genNameAnsi);
        if (m_hcIsSet);
            b.append("\n\tm_hc: " + m_hc);
        b.append("\n\tm_informationReqIndAnsi: " + m_informationReqIndAnsi);
        b.append("\n\tm_jurisdic: ");
        if (m_jurisdict!= null)
            b.append(JainSS7Utility.bytesToHex(m_jurisdict, 0, m_jurisdict.length));
        b.append("\n\tm_ntp: ");
        if (m_ntp!= null)
            b.append(JainSS7Utility.bytesToHex(m_ntp, 0, m_ntp.length));
        b.append("\n\tm_osi: " + m_osi);
        if (m_oliIsSet)
            b.append("\n\tm_oli: " + m_oli);
        if (m_scIsSet)
            b.append("\n\tm_sc: " + m_sc);
        if (m_sprIsSet)
            b.append("\n\tm_spr: " + m_spr);
        b.append("\n\tm_tr: " + m_tr);
        return b.toString();
    }
    protected UserServiceInfo m_usi = null;
    protected BusinessGroupAnsi m_bgp = null;
    protected CarrierIdAnsi m_cid = null;
    protected short m_csi;
    protected boolean m_csiIsSet = false;
    protected ChargeNumberAnsi m_chargeNumAnsi = null;
    protected CircuitAssignMapAnsi m_cam = null;
    protected byte[] m_es = null;
    protected GenericNameAnsi m_genNameAnsi = null;
    protected byte m_hc;
    protected boolean m_hcIsSet = false;
    protected InfoReqIndAnsi m_informationReqIndAnsi = null;
    protected byte[] m_jurisdict = null;
    protected byte[] m_ntp = null;
    protected OperatorServicesInfoAnsi m_osi = null;
    protected byte m_oli;
    protected boolean m_oliIsSet = false;
    protected short m_sc;
    protected boolean m_scIsSet = false;
    protected short m_spr;
    protected boolean m_sprIsSet = false;
    protected TransactionReqAnsi m_tr = null;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
