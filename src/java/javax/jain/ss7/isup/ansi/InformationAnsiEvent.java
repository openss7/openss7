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

package javax.jain.ss7.isup.ansi;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** An ISUP Event: The InformationAnsiEvent class is a sub class of ISUP Core
    InformationEvent class and is to handle the ANSI variant of INF message.

    @author Monavacon Limited
    @version 1.2.2
  */
public class InformationAnsiEvent extends InformationEvent {
    /** Constructs a new InformationAnsiEvent, with only the JAIN ISUP Mandatory
        parameters being supplied to the constructor, using the super class's constructor.
        @param source  The source of this event.
        @param primitive  Primitive value is ISUP_PRIMITIVE_INFORMATION. Refer to
        IsupEvent class for more details.
        @param dpc  The destination point code.
        @param opc  The origination point code.
        @param sls  The signaling link selection.
        @param cic  The CIC on which the call has been established.
        @param congestionPriority  Priority of the ISUP message which may be used in the
        optional national congestion control procedures at MTP3. Refer to
        getCongestionPriority method in IsupEvent class for more details.
        @param in_infoInd  Information Indicator parameter.
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public InformationAnsiEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
            InfoIndAnsi in_infoInd)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority, in_infoInd);
    }
    /** Gets the AccessTransport parameter of the message.
        @return The AccessTransport parameter of the event.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public byte[] getAccessTransport()
        throws ParameterNotSetException {
        if (m_accessTransport != null)
            return m_accessTransport;
        throw new ParameterNotSetException("AccessTransport not set.");
    }
    /** Sets the AccessTransport parameter of the message.
        @param in_accessTransport  AccessTransport parameter.
      */
    public void setAccessTransport(byte[] in_accessTransport) {
        m_accessTransport = in_accessTransport;
    }
    /** Indicates if the AccessTransport parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isAccessTransportPresent() {
        return (m_accessTransport != null);
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
    /** Sets the Business Group parameter of the message.
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
    /** Gets the ChargeNumberAnsi parameter of the message.
        Refer to ChargeNumberAnsi parameter for greater details.
        @return The ChargeNumber parameter of the event.
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
    /** Gets the RedirectionInfo parameter of the message.
        @return The RedirectionInfo parameter of the event.
        @exception ParameterNotSetException  Thrown when the parameter is not set in the
        event.
      */
    public RedirectionInfo getRedirectionInfo()
        throws ParameterNotSetException {
        if (m_ri != null)
            return m_ri;
        throw new ParameterNotSetException("RedirectionInfo not set.");
    }
    /** Sets the RedirectionInfo parameter of the message.
        @param ri  Redirection Information parameter.
      */
    public void setRedirectionInfo(RedirectionInfo ri) {
        m_ri = ri;
    }
    /** Indicates if the RedirectionInfo parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isRedirectionInfoPresent() {
        return (m_ri != null);
    }
    /** Gets the Redirecting Number parameter of the message.
        Refer to RedirectingNumber parameter class for more information.
        @return The Redirecting Number parameter of the event.
        @exception ParameterNotSetException  Thrown when mandatory ISUP parameter is not
        set.
      */
    public RedirectingNumber getRedirectingNumber()
        throws ParameterNotSetException {
        if (m_rn != null)
            return m_rn;
        throw new ParameterNotSetException("RedirectingNumber not set.");
    }
    /** Sets the Redirecting Number parameter of the message.
        Refer to RedirectingNumber parameter class for more information.
        @param ncithe  Redirecting Number parameter of the event.
      */
    public void setRedirectingNumber(RedirectingNumber rn) {
        m_rn = rn;
    }
    /** Indicates if the RedirectingNumber parameter is present in this Event.
        @return True if the parameter has been set.
      */
    public boolean isRedirectingNumberPresent() {
        return (m_rn != null);
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
        InformationAnsiEvent class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.ansi.InformationAnsiEvent");
        b.append("\n\tm_accessTransport: ");
        if (m_accessTransport != null)
            b.append(JainSS7Utility.bytesToHex(m_accessTransport, 0, m_accessTransport.length));
        b.append("\n\tm_bgp: " + m_bgp );
        b.append("\n\tm_chargeNumAnsi: " + m_chargeNumAnsi );
        if (m_oliIsSet)
            b.append("\n\tm_oli: " + m_oli );
        b.append("\n\tm_ri: " + m_ri );
        b.append("\n\tm_rn: " + m_rn );
        b.append("\n\tm_userToUserInfo: ");
        if (m_userToUserInfo != null)
            b.append(JainSS7Utility.bytesToHex(m_userToUserInfo , 0, m_userToUserInfo.length));
        return b.toString();
    }
    protected byte[] m_accessTransport = null;
    protected BusinessGroupAnsi m_bgp = null;
    protected ChargeNumberAnsi m_chargeNumAnsi = null;
    protected byte m_oli;
    protected boolean m_oliIsSet = false;
    protected RedirectionInfo m_ri = null;
    protected RedirectingNumber m_rn = null;
    protected byte[] m_userToUserInfo = null;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
