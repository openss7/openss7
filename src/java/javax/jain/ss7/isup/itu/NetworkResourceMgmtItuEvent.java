/*
 @(#) src/java/javax/jain/ss7/isup/itu/NetworkResourceMgmtItuEvent.java <p>
 
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

/** An ISUP EVENT: The NetworkResourceMgmtItuEvent class is a sub class of the IsupEvent
  * class and is exchanged between an ISUP Provider and an ISUP Listener for sending and
  * receiving the ISUP Network Resource Management Message.
  * Listener would send an EventNetworkResourceMgmt object to the provider for sending a
  * NRM message to the ISUP stack.  ISUP Provider would send an EventNetworkResourceMgmt
  * object to the listener on the reception of an NRM message from the stack for the user
  * address handled by that listener. The mandatory parameters for generating an NRM ISUP
  * message are set using the constructor itself.  The optional parameters may be set
  * using get and set methods described below. The primitive field is filled as
  * ISUP_PRIMITIVE_NETWORK_RESOURCE_MGMT.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class NetworkResourceMgmtItuEvent extends IsupEvent {
    /** Constructs a new NetworkResourceMgmtItuEvent, with only the JAIN ISUP Mandatory
      * parameters being supplied to the constructor.
      * @param source  The source of this event.
      * @param dpc  The destination point code.
      * @param opc  The origination point code.
      * @param sls  The signaling link selection.
      * @param cic  The CIC on which the call has been established.
      * @param congestionPriority  Priority of the ISUP message which may be used in the
      * optional national congestion control procedures at MTP3. Refer to
      * getCongestionPriority method in IsupEvent class for more details.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public NetworkResourceMgmtItuEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
    }
    /** Gets the ISUP NETWORK RESOURCE MANAGEMENT primtive value.
      * @return The ISUP NETWORK RESOURCE MANAGEMENT primitive value.
      */
    public int getIsupPrimitive() {
        return IsupConstants.ISUP_PRIMITIVE_NETWORK_RESOURCE_MGMT;
    }
    /** Gets the EchoControlInfoItu parameter of the message.
      * @return The EchoControlInfoItu parameter of the event.
      * @exception ParameterNotSetException  Thrown when the parameter is not set in the
      * event.
      */
    public EchoControlInfoItu getEchoControlInfo()
        throws ParameterNotSetException {
        if (m_eci != null)
            return m_eci;
        throw new ParameterNotSetException("EchoControlInfoItu not set.");
    }
    /** Sets the EchoControlInfoItu parameter of the message.
      * @param eci  The EchoControlInfoItu parameter of the event.
      */
    public void setEchoControlInfo(EchoControlInfoItu eci) {
        m_eci = eci;
    }
    /** Indicates if the EchoControlInfoItu parameter is present in this Event.
      * @return True if the parameter is set.
      */
    public boolean isEchoControlInfoPresent() {
        return (m_eci != null);
    }
    /** Gets the Parameter Compatibility Information parameter of the message.
      * Refer to Parameter Compatibility Information parameter for greater details.
      * @return Parameter Compatibility Information parameter.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.
      */
    public ParamCompatibilityInfoItu getParamCompatibilityInfo()
        throws ParameterNotSetException {
        if (m_paramConpInf != null)
            return m_paramConpInf;
        throw new ParameterNotSetException("ParamCompatibilityInfoItu not set.");
    }
    /** Sets the Parameter Compatibility Information parameter of the message.
      * Refer to Parameter Compatibility Information parameter for greater details.
      * @param in_paramCompInfo  The Parameter Compatibility Information parameter of the
      * event.
      */
    public void setParamCompatibilityInfo(ParamCompatibilityInfoItu in_paramCompInfo) {
        m_paramConpInf = in_paramCompInfo;
    }
    /** Indicates if the Parameter Compatibility Information parameter is present in this
      * Event.
      * @return True if the parameter has been set.
      */
    public boolean isParamCompatibilityInfoPresent() {
        return (m_paramConpInf != null);
    }
    /** Gets the Message Compatibility Information parameter of the message.
      * Refer to Message Compatibility Information parameter for greater details.
      * @return Message Compatibility Information parameter.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.
      */
    public MessageCompatibilityInfoItu getMessageCompatibilityInfo()
        throws ParameterNotSetException {
        if (m_messageCompInfo != null)
            return m_messageCompInfo;
        throw new ParameterNotSetException("MessageCompatibilityInfoItu not set.");
    }
    /** Sets the Message Compatibility Information parameter of the message.
      * Refer to Message Compatibility Information parameter for greater details.
      * @param in_MessageCompInfo  The Message Compatibility Information parameter of the
      * event.
      */
    public void setMessageCompatibilityInfo(MessageCompatibilityInfoItu in_MessageCompInfo) {
        m_messageCompInfo = in_MessageCompInfo;
    }
    /** Indicates if the Message Compatibility Information parameter is present in this
      * Event.
      * @return True if the parameter has been set.
      */
    public boolean isMessageCompatibilityInfoPresent() {
        return (m_messageCompInfo != null);
    }
    /** Gets the TransmissionMediumUsed parameter of the message.
      * @return The TransmissionMediumUsed parameter of the event.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.
      */
    public byte getTransmissionMediumUsed()
        throws ParameterNotSetException {
        if (m_tmuIsSet)
            return m_tmu;
        throw new ParameterNotSetException("TransmissionMediumUsed not set.");
    }
    /** Sets the TransmissionMediumUsed parameter of the message.
      * @param tmu  The TransmissionMediumUsed parameter of the event.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
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
      * @return True if the parameter has been set.
      */
    public boolean isTransmissionMediumUsedPresent() {
        return m_tmuIsSet;
    }
    /** The toString method retrieves a string containing the values of the members of the
      * NetworkResourceMgmtItuEvent class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.itu.NetworkResourceMgmtItuEvent");
        b.append("\n\tm_eci: " + m_eci);
        b.append("\n\tm_paramConpInf: " + m_paramConpInf);
        b.append("\n\tm_messageCompInfo: " + m_messageCompInfo);
        if (m_tmuIsSet)
            b.append("\n\tm_tmu: " + m_tmu);
        return b.toString();
    }
    protected EchoControlInfoItu m_eci = null;
    protected ParamCompatibilityInfoItu m_paramConpInf = null;
    protected MessageCompatibilityInfoItu m_messageCompInfo = null;
    protected byte m_tmu;
    protected boolean m_tmuIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
