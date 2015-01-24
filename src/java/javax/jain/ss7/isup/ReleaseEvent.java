/*
 @(#) src/java/javax/jain/ss7/isup/ReleaseEvent.java <p>
 
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

/** An ISUP EVENT: The ReleaseEvent class is a sub class of the IsupEvent class and is
  * exchanged between an ISUP Provider and an ISUP Listener for sending and receiving
  * the ISUP Release message.
  * Listener would send a ReleaseEvent object to the provider for sending a REL
  * message to the ISUP stack. ISUP Provider would send a ReleaseEvent object to the
  * listener on the reception of a REL message from the stack for the user address
  * handled by that listener. The mandatory parameters are supplied to the
  * constructor. Optional parameters may then be set using the set methods The
  * primitive field is filled as ISUP_PRIMITIVE_RELEASE.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract class ReleaseEvent extends IsupEvent {
    /** Constructs a new ReleaseEvent, with only the JAIN ISUP Mandatory parameters
      * being supplied to the constructor.
      * @param source  The source of this event.
      * @param dpc  The destination point code.
      * @param opc  The origination point code.
      * @param sls  The signaling link selection.
      * @param cic  The CIC on which the call has been established.
      * @param congestionPriority  Priority of the ISUP message which may be used in
      * the optional national congestion control procedures at MTP3. Refer to
      * getCongestionPriority method in IsupEvent class for more details.
      * @param causeIndicator  Cause indicators.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    protected ReleaseEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
            CauseInd causeIndicator)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
        this.setCauseInd(causeIndicator);
    }
    /** Gets the ISUP RELEASE primtive value.
      * @return The ISUP RELEASE primitive value.  */
    public int getIsupPrimitive() {
        return IsupConstants.ISUP_PRIMITIVE_RELEASE;
    }
    /** Checks if the mandatory parameters have been set.
      * @exception MandatoryParameterNotSetException  Thrown when the mandatory
      * parameter is not set.  */
    public void checkMandatoryParameters()
        throws MandatoryParameterNotSetException {
        if (m_causeIndicator == null)
            throw new MandatoryParameterNotSetException("CauseIndicator parameter not set.");
    }
    /** Gets the Cause Indicator parameter of the message.
      * Refer to CauseInd parameter for greater details.
      * @return The CauseInd parameter of the event.
      * @exception MandatoryParameterNotSetException  Thrown when the Mandatory
      * parameter is not set.  */
    public CauseInd getCauseInd()
        throws MandatoryParameterNotSetException {
        return m_causeIndicator;
    }
    /** Sets the Cause Indicator parameter of the message.
      * @param causeIndicator  The CauseInd parameter of the event.  */
    public void setCauseInd(CauseInd causeIndicator) {
        m_causeIndicator = causeIndicator;
    }
    /** Gets the AccessTransport parameter of the message.
      * @return The AccessTransport parameter of the event.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.  */
    public byte[] getAccessTransport()
        throws ParameterNotSetException {
        if (m_accessTrans != null)
            return m_accessTrans;
        throw new ParameterNotSetException("AccessTransport parameter not set.");
    }
    /** Sets the AccessTransport parameter of the message.
      * @param accessTrans  The AccessTransport parameter of the event.  */
    public void setAccessTransport(byte[] accessTrans) {
        m_accessTrans = accessTrans;
    }
    /** Indicates if the AccessTransport parameter is present in this Event.
      * @return True if the parameter has been set.  */
    public boolean isAccessTransportPresent() {
        return (m_accessTrans != null);
    }
    /** Gets the Automatic Congestion Level parameter of the message.
      * Automatic congestion level parameter has the values 1 and 2 in ITU and the
      * values 1, 2 and 3 for ANSI.
      * @return The Automatic Congestion Level parameter of the event.  For ITU the
      * values are 1 or 2 and for ANSI the value are 1,2 or 3.
      * @exception ParameterNotSetException - Thrown when the optional parameter is
      * not set.  */
    public byte getAutoCongLevel()
        throws ParameterNotSetException {
        if (m_autoCongestionLevelIsSet)
            return m_autoCongestionLevel;
        throw new ParameterNotSetException("AutoCongestionLevel not set.");
    }
    /** Sets the Automatic Congestion Level parameter of the message.
      * Automatic congestion level parameter has the values 1 and 2 in ITU and the
      * values 1, 2 and 3 for ANSI.
      * @param autoCongestionLevel  The Automatic Congestion Level parameter of the
      * event.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public void setAutoCongLevel(byte autoCongestionLevel)
        throws ParameterRangeInvalidException {
        if (1 > autoCongestionLevel || autoCongestionLevel > 3)
            throw new ParameterRangeInvalidException("autoCongestionLevel value " + autoCongestionLevel + " out of range.");
        m_autoCongestionLevel = autoCongestionLevel;
        m_autoCongestionLevelIsSet = true;
    }
    /** Indicates if the AutoCongLevel parameter is present in this Event.
      * @return True if the parameter has been set.  */
    public boolean isAutoCongLevelPresent() {
        return m_autoCongestionLevelIsSet;
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
    /** The toString method retrieves a string containing the values of the members of
      * the ReleaseEvent class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.ReleaseEvent");
        b.append("\n\tm_causeIndicator: " + m_causeIndicator);
        b.append("\n\tm_accessTrans: ");
        if (m_accessTrans != null)
            b.append(JainSS7Utility.bytesToHex(m_accessTrans, 0, m_accessTrans.length));
        if (m_autoCongestionLevelIsSet)
        b.append("\n\tm_autoCongestionLevel: " + m_autoCongestionLevel);
        b.append("\n\tm_userToUserInfo: ");
        if (m_userToUserInfo != null)
            b.append(JainSS7Utility.bytesToHex(m_userToUserInfo, 0, m_userToUserInfo.length));
        return b.toString();
    }
    protected CauseInd m_causeIndicator = null;
    protected byte[] m_accessTrans = null;
    protected byte m_autoCongestionLevel;
    protected boolean m_autoCongestionLevelIsSet = false;
    protected byte[] m_userToUserInfo = null;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
