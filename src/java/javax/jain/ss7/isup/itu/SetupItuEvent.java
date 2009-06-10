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

package javax.jain.ss7.isup.itu;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** An ISUP Event: The SetupItuEvent class is a sub class of ISUP Core SetupEvent class
  * and is to handle the ITU variant of IAM message.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class SetupItuEvent extends SetupEvent {
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
    public SetupItuEvent(Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
            boolean glareControl, NatureConnInd nci, FwdCallIndItu fci,
            CalledPartyNumberItu cpn, byte cat, byte tmr)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority, glareControl, nci, fci,
                cpn, cat);
        this.setTransmissionMediumReq(tmr);
    }
    /** Checks if the mandatory parameters have been set.
      * @exception MandatoryParameterNotSetException  Thrown when the mandatory parameter
      * is not set.
      */
    public void checkMandatoryParameters()
        throws MandatoryParameterNotSetException {
        super.checkMandatoryParameters();
    }
    /** Gets the Transmission Medium Requirement parameter of the message.
      * This has a range 0 - 10.
      * @return The Transmission Medium Requirement parameter of the event.
      */
    public byte getTransmissionMediumReq() {
        return m_tmr;
    }
    /** Sets the transmission medium requirement parameter in SetupEvent.
      * @param tmrthe  Transmission medium requirement parameter of the event.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setTransmissionMediumReq(byte tmr)
        throws ParameterRangeInvalidException {
        switch (tmr) {
            case IsupConstants.TMR_SPEECH:
            case IsupConstants.TMR_64_KBPS_UNRESTRICTED:
            case IsupConstants.TMR_3DOT1_KHZ_AUDIO:
            case IsupConstants.TMR_64_KBPS_PREFERRED:
            case IsupConstants.TMR_2_by_64_KBPS_UNRESTRICTED:
            case IsupConstants.TMR_384_KBPS_UNRESTRICTED:
            case IsupConstants.TMR_1536_KBPS_UNRESTRICTED:
            case IsupConstants.TMR_1920_KBPS_UNRESTRICTED:
                m_tmr = tmr;
                return;
        }
        throw new ParameterRangeInvalidException("TrnansmissionMediumReq value " + tmr + " out of range.");
    }
    /** Gets the Closed User Group (CUG) Interlock Code parameter of the message.
      * Refer to CUGInterlockCode parameter for more details.
      * @return The CUGInterlockCodeItu parameter of the event.
      * @exception ParameterNotSetException  Thrown when the parameter is not set in the
      * event.
      */
    public CUGInterlockCodeItu getCUGInterlockCode()
        throws ParameterNotSetException {
        if (m_cugic != null)
            return m_cugic;
        throw new ParameterNotSetException("CUGInterlockCode not set.");
    }
    /** Sets the Closed User Group (CUG) Interlock Code of the message.
      * Refer to CUGInterlockCodeItu parameter for more details.
      * @param cugic  The CUG Interlock Code parameter of the event.
      */
    public void setCUGInterlockCode(CUGInterlockCodeItu cugic) {
        m_cugic = cugic;
    }
    /** Indicates if the CUGInterlockCodeItu parameter is present in this Event.
      * @return True if the parameter is set.
      */
    public boolean isCUGInterlockCodePresent() {
        return (m_cugic != null);
    }
    /** Gets the Generic NotificationInd parameter of the message.
      * @return The NotificationInd parameter of the event.
      * @exception ParameterNotSetException  Thrown when the parameter is not set in the
      * event.
      */
    public NotificationInd getNotificationInd()
        throws ParameterNotSetException {
        if (m_gni != null)
            return m_gni;
        throw new ParameterNotSetException("NotificationInd not set.");
    }
    /** Sets the Generic NotificationInd parameter of the message.
      * @param gni  The NotificationInd parameter of the event.
      */
    public void setNotificationInd(NotificationInd gni) {
        m_gni = gni;
    }
    /** Indicates if the NotificationInd parameter is present in this Event.
      * @return True if the parameter is set.
      */
    public boolean isNotificationIndPresent() {
        return (m_gni != null);
    }
    /** Gets the LocationNumberItu parameter of the message.
      * Refer to LocationNumber parameter for more details.
      * @return The LocationNumberItu parameter of the event.
      * @exception ParameterNotSetException   Thrown when the parameter is not set in the
      * event.
      */
    public LocationNumberItu getLocationNumber()
        throws ParameterNotSetException {
        if (m_ln != null)
            return m_ln;
        throw new ParameterNotSetException("LocationNumberItu not set.");
    }
    /** Sets the LocationNumberItu parameter of the message.
      * Refer to LocationNumberItu parameter for more details.
      * @param ln  The LocationNumber parameter of the event.
      */
    public void setLocationNumber(LocationNumberItu ln) {
        m_ln = ln;
    }
    /** Indicates if the LocationNumberItu parameter is present in this Event.
      * @return True if the parameter is set.
      */
    public boolean isLocationNumberPresent() {
        return (m_ln != null);
    }
    /** Gets the NwSpecificFacItu parameter of the message.
      * @return The NwSpecificFacItu parameter of the event.
      * @exception ParameterNotSetException  Thrown when the parameter is not set in the
      * event.
      */
    public NwSpecificFacItu getNwSpecificFac()
        throws ParameterNotSetException {
        if (m_nsf != null)
            return m_nsf;
        throw new ParameterNotSetException("NwSpecificFacItu not set.");
    }
    /** Sets the NwSpecificFacItu parameter of the message.
      * @param nsf  The NwSpecificFacItu parameter of the event.
      */
    public void setNwSpecificFac(NwSpecificFacItu nsf) {
        m_nsf = nsf;
    }
    /** Indicates if the NwSpecificFacItu parameter is present in this Event.
      * @return True if the parameter is set.
      */
    public boolean isNwSpecificFacPresent() {
        return (m_nsf != null);
    }
    /** Gets the Originating ISC Point Code parameter of the message.
      * The byte array returned is not interpreted by Provider/Stack object.
      * @return The OrigISCPointCode parameter of the event.
      * @exception this  Dxception is thrown if ITU.
      */
    public byte[] getOrigISCPointCode()
        throws ParameterNotSetException {
        if (m_origISCpointcode != null)
            return m_origISCpointcode;
        throw new ParameterNotSetException("OrigISCPointCode not set.");
    }
    /** Sets the Originating ISC Point Code parameter of the message.
      * @param origISCpointcode  The Originating ISC point code parameter of the event.
      */
    public void setOrigISCPointCode(byte[] origISCpointcode) {
        m_origISCpointcode = origISCpointcode;
    }
    /** Indicates if the OrigISCPointCode parameter is present in this Event.
      * @return True if the parameter is set.
      */
    public boolean isOrigISCPointCodePresent() {
        return (m_origISCpointcode != null);
    }
    /** Gets the Optional Forward Call Indicator parameter of the message.
      * Refer to OptionalFwdCallInd parameter for more details.
      * @return The OptionalFwdCallIndItu parameter of the event.
      * @exception ParameterNotSetException  Thrown when the parameter is not set in the
      * event.
      */
    public OptionalFwdCallIndItu getOptionalFwdCallInd()
        throws ParameterNotSetException {
        if (m_ofci != null)
            return m_ofci;
        throw new ParameterNotSetException("OptionalFwdCallIndItu not set.");
    }
    /** Sets the Optional Forward Call Indicator parameter of the message.
      * @param ofci  The Optional Fwd Call Indicator parameter of the event.
      */
    public void setOptionalFwdCallInd(OptionalFwdCallIndItu ofci) {
        m_ofci = ofci;
    }
    /** Indicates if the OptionalFwdCallIndItu parameter is present in this Event.
      * @return True if the parameter is set.
      */
    public boolean isOptionalFwdCallIndPresent() {
        return (m_ofci != null);
    }
    /** Gets the ParamCompatibilityInfoItu parameter of the message.
      * @return The ParamCompatibilityInfoItu parameter of the event.
      * @exception ParameterNotSetException  Thrown when the parameter is not set in the
      * event.
      */
    public ParamCompatibilityInfoItu getParamCompatibilityInfo()
        throws ParameterNotSetException {
        if (m_pci != null)
            return m_pci;
        throw new ParameterNotSetException("ParamCompatibilityInfoItu not set.");
    }
    /** Sets the ParamCompatibilityInfoItu parameter of the message.
      * @param pci  The ParamCompatibilityInfoItu parameter of the event.
      */
    public void setParamCompatibilityInfo(ParamCompatibilityInfoItu pci) {
        m_pci = pci;
    }
    /** Indicates if the ParamCompatibilityInfoItu parameter is present in this Event.
      * @return True if the parameter is set.
      */
    public boolean isParamCompatibilityInfoPresent() {
        return (m_pci != null);
    }
    /** Gets the PropagationDelayCounter parameter of the message.Its a value in the range
      * 0 to 0xffff.
      * @return The PropagationDelayCounter parameter of the event.
      * @exception ParameterNotSetException  Thrown when the parameter is not set in the
      * event.
      */
    public int getPropagationDelayCounter()
        throws ParameterNotSetException {
        if (m_pdcIsSet)
            return m_pdc;
        throw new ParameterNotSetException("PropagationDelayCounter not set.");
    }
    /** Sets the PropagationDelayCounter parameter of the message.
      * @param propDelayCounter  The Propagation delay counter parameter of the event.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setPropagationDelayCounter(int propDelayCounter)
        throws ParameterRangeInvalidException {
        if (0 <= propDelayCounter && propDelayCounter <= 0xffff) {
            m_pdc = propDelayCounter;
            m_pdcIsSet = true;
            return;
        }
        throw new ParameterRangeInvalidException("PropagationDelayCounter value " + propDelayCounter + " out of range.");
    }
    /** Indicates if the PropagationDelayCounter parameter is present in this Event.
      * @return True if the parameter is set.
      */
    public boolean isPropagationDelayCounterPresent() {
        return m_pdcIsSet;
    }
    /** Gets the Transmission Medium Requirement Prime parameter of the message, value
      * lies in the range of 0 and 10.
      * @return The Transmission Medium Requirement Prime parameter of the event, range 0
      * - 10.
      * @exception ParameterNotSetException  Thrown when the parameter is not set in the
      * event ITU.
      */
    public byte getTransmissionMediumReqPrime()
        throws ParameterNotSetException {
        if (m_tmrpIsSet)
            return m_tmrp;
        throw new ParameterNotSetException("TransmissionMediumReqPrime not set.");
    }
    /** Sets the transmission medium Requirement Prime parameter in SetupEvent, range
      * between 0 - 10.
      * @param tmrpthe  Transmission Medium Requirement Prime parameter of the event.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setTransmissionMediumReqPrime(byte tmrp)
        throws ParameterRangeInvalidException {
        switch (tmrp) {
            case IsupConstants.TMRP_SPEECH:
            case IsupConstants.TMRP_64_KBPS_UNRESTRICTED:
            case IsupConstants.TMRP_3DOT1_KHZ_AUDIO:
                m_tmrp = tmrp;
                m_tmrpIsSet = true;
                return;
        }
        throw new ParameterRangeInvalidException("TrnasmissionMediumReqPrime value " + tmrp + " out of range.");
    }
    /** Indicates if the TransmissionMediumReqPrime parameter is present in this Event.
      * @return True if the parameter is set.
      */
    public boolean isTransmissionMediumReqPrimePresent() {
        return m_tmrpIsSet;
    }
    /** Gets the User Service Information parameter of the message.
      * Refer to UserServiceInfo parameter for greater details.
      * @return The UserServiceInfo parameter of the event.
      * @exception ParameterNotSetException  Thrown when the ISUP parameter is not set in
      * the event.
      */
    public UserServiceInfo getUserServiceInfo()
        throws ParameterNotSetException {
        if (m_usi != null)
            return m_usi;
        throw new ParameterNotSetException("UserServiceInfo not set.");
    }
    /** Sets the User Service Information parameter of the message.
      * Refer to UserServiceInfo parameter for greater details.
      * @param usi  User Service Information parameter.
      */
    public void setUserServiceInfo(UserServiceInfo usi) {
        m_usi = usi;
    }
    /** Indicates if the UserServiceInfo parameter is present in this Event.
      * @return True if the parameter is set.
      */
    public boolean isUserServiceInfoPresent() {
        return (m_usi != null);
    }
    /** Gets the UserToUserIndicatorsItu parameter of the message.
      * Refer to UserToUserIndicatorsItu parameter for greater details.
      * @return The UserToUserIndicatorsItu parameter of the event.
      * @exception ParameterNotSetException  Thrown when the ISUP parameter is not set in
      * the event.
      */
    public UserToUserIndicatorsItu getUserToUserIndicators()
        throws ParameterNotSetException {
        if (m_uui != null)
            return m_uui;
        throw new ParameterNotSetException("UserToUserIndicatorsItu not set.");
    }
    /** Sets the UserToUserIndicatorsItu parameter of the message.
      * Refer to ToUserIndicators parameter for greater details.
      * @param uui  The User To User Indicators parameter of the event.
      */
    public void setUserToUserIndicators(UserToUserIndicatorsItu uui) {
        m_uui = uui;
    }
    /** Indicates if the UserToUserIndicatorsItu parameter is present in this Event.
      * @return True if the parameter is set.
      */
    public boolean isUserToUserIndicatorsPresent() {
        return (m_uui != null);
    }
    /** The toString method retrieves a string containing the values of the members of the
      * SetupItuEvent class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.itu.SetupItuEvent");
        b.append("\n\tm_tmr:" + m_tmr);
        b.append("\n\tm_cugic:" + m_cugic);
        b.append("\n\tm_gni:" + m_gni);
        b.append("\n\tm_ln:" + m_ln);
        b.append("\n\tm_nsf:" + m_nsf);
        b.append("\n\tm_origISCpointcode:");
        if (m_origISCpointcode!= null)
            b.append(JainSS7Utility.bytesToHex(m_origISCpointcode, 0, m_origISCpointcode.length));
        b.append("\n\tm_ofci:" + m_ofci);
        b.append("\n\tm_pci:" + m_pci);
        if (m_pdcIsSet)
            b.append("\n\tm_pdc:" + m_pdc);
        if (m_tmrpIsSet)
            b.append("\n\tm_tmrp:" + m_tmrp);
        b.append("\n\tm_usi:" + m_usi);
        b.append("\n\tm_uui:" + m_uui);
        return b.toString();
    }
    protected byte m_tmr;
    protected CUGInterlockCodeItu m_cugic = null;
    protected NotificationInd m_gni = null;
    protected LocationNumberItu m_ln = null;
    protected NwSpecificFacItu m_nsf = null;
    protected byte[] m_origISCpointcode = null;
    protected OptionalFwdCallIndItu m_ofci = null;
    protected ParamCompatibilityInfoItu m_pci = null;
    protected int m_pdc;
    protected boolean m_pdcIsSet = false;
    protected byte m_tmrp;
    protected boolean m_tmrpIsSet = false;
    protected UserServiceInfo m_usi = null;
    protected UserToUserIndicatorsItu m_uui = null;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
