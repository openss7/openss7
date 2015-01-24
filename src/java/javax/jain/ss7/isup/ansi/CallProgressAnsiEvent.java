/*
 @(#) src/java/javax/jain/ss7/isup/ansi/CallProgressAnsiEvent.java <p>
 
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

package javax.jain.ss7.isup.ansi;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** An ISUP Event: The CallProgressAnsiEvent class is a sub class of ISUP Core
  * CallProgressEvent class and is to handle the ANSI variant of CPG message.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class CallProgressAnsiEvent extends CallProgressEvent {
    /** Constructs a new CallProgressAnsiEvent, with only the JAIN ISUP Mandatory
      * parameters being supplied to the constructor.
      * @param source  The source of this event.
      * @param dpc  The destination point code.
      * @param opc  The origination point code.
      * @param sls  The signaling link selection.
      * @param cic  The CIC on which the call has been established.
      * @param congestionPriority  Priority of the ISUP message which may be used in
      * the optional national congestion control procedures at MTP3. Refer to
      * getCongestionPriority method in IsupEvent class for more details.
      * @param eventInformation  Event Information parameter.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public CallProgressAnsiEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
            EventInformation eventInformation)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority, eventInformation);
    }
    /** Gets the BusinessGroup parameter of the message.
      * @return The BusinessGroup parameter of the event.
      * @exception ParameterNotSetException  Thrown when the parameter is not set in
      * the event.  */
    public BusinessGroupAnsi getBusinessGroup() throws ParameterNotSetException {
        if (isBusinessGroupPresent())
            return m_bgp;
        throw new ParameterNotSetException("Business Group parameter not set.");
    }
    /** Sets the Business Group parameter of the message.
      * @param bgp  Business group parameter.  */
    public void setBusinessGroup(BusinessGroupAnsi bgp) {
        m_bgp = bgp;
    }
    /** Indicates if the BusinessGroupAnsi parameter is present in this Event.
      * @return True if the parameter is set.  */
    public boolean isBusinessGroupPresent() {
        return (m_bgp != null);
    }
    /** Gets the Information Indicators parameter of the message. Refer to InfoIndAnsi
      * parameter for greater details.
      * @return The Information Indicators parameter of the event.
      * @exception ParameterNotSetException  Exception is thrown if ITU.  */
    public InfoIndAnsi getInfoInd() throws ParameterNotSetException {
        if (isInfoIndPresent())
            return m_iip;
        throw new ParameterNotSetException("Information Indicators parameter not set.");
    }
    /** Sets the Information Indicators parameter of the message. Refer to InfoIndAnsi
      * parameter for greater details.
      * @param infomationIndAnsi  Information Indicators parameter.  */
    public void setInfoInd(InfoIndAnsi infomationIndAnsi) {
        m_iip = infomationIndAnsi;
    }
    /** Indicates if the InfoIndAnsi parameter is present in this Event.
      * @return True if the parameter is set else false.  */
    public boolean isInfoIndPresent() {
        return (m_iip != null);
    }
    /** Gets the NetworkTransport parameter of the message.  
      * @return The NetworkTransport parameter of the event. This is a byte array, the
      * way in which the byte array is filled is; Fill the Name of the parameter, the
      * length of the parameter and the parameter contents.  This is repeated if
      * multiple parameters are to be sent.
      * @exception ParameterNotSetException  Thrown when the parameter is not set in
      * the event.  */
    public byte[] getNetworkTransport() throws ParameterNotSetException {
        if (isNetworkTransportPresent())
            return m_ntp;
        throw new ParameterNotSetException("Network Transport parameter not set.");
    }
    /** Sets the NetworkTransport parameter of the message.
      * @param ntp  Network Transport parameter. This is a byte array, the way in which
      * the byte array is filled is; Fill the Name of the parameter, the length of the
      * parameter and the parameter contents. This is repeated if multiple parameters
      * are to be sent.  */
    public void setNetworkTransport(byte[] ntp) {
        m_ntp = ntp;
    }
    /** Indicates if the NetworkTransport parameter is present in this Event.
      * @return True if the parameter is set.  */
    public boolean isNetworkTransportPresent() {
        return (m_ntp != null);
    }
    /** The toString method retrieves a string containing the values of the members of the CallProgressAnsiEvent class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer buffer = new StringBuffer(500);
        buffer.append(super.toString());
        buffer.append("\nCallProgressAnsiEvent:");
        if (isBusinessGroupPresent())
            buffer.append("\n\tBusiness Group: " + m_bgp);
        if (isInfoIndPresent())
            buffer.append("\n\tInformation Indicators: " + m_iip);
        if (isNetworkTransportPresent()) {
            buffer.append("\n\tNetwork Transport: ");
            buffer.append(JainSS7Utility.bytesToHex(m_ntp, 0, m_ntp.length));
        }
        return buffer.toString();
    }
    private BusinessGroupAnsi m_bgp = null;
    private InfoIndAnsi m_iip = null;
    private byte[] m_ntp = null;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
