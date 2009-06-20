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

/** An ISUP Event: The InformationReqAnsiEvent class is a sub class of Core
    InformationReqEvent class and is used to handle the ANSI variant of INR message.

    @author Monavacon Limited
    @version 1.2.2
  */
public class InformationReqAnsiEvent extends InformationReqEvent {
    /** Constructs a new InformationReqAnsiEvent, with only the JAIN ISUP Mandatory
        parameters, being supplied to the constructor.
        @param source - The source of this event.
        @param primitive - Primitive value for StartResetInd is
        ISUP_PRIMITIVE_INFORMATION_REQ. Refer to IsupEvent class for more details.
        @param dpc - The destination point code.
        @param opc - The origination point code.
        @param sls - The signaling link selection.
        @param cic - The CIC on which the call has been established.
        @param congestionPriority - Priority of the ISUP message which may be used in the
        optional national congestion control procedures at MTP3. Refer to
        getCongestionPriority method in IsupEvent class for more details.
        @param informationReqInd - The Information Request Indicator parameter.
        @exception ParameterRangeInvalidException - Thrown when value is out of range.
     */
    public InformationReqAnsiEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
            InfoReqIndAnsi informationReqInd)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority, informationReqInd);
    }
    /** Gets the NetworkTransport parameter of the message.
        @return The NetworkTransport parameter of the event. This is a byte array, the way
        in which the byte array is filled is :- Fill the Name of the parameter, the length
        of the parameter and the parameter contents. This is repeated if multiple
        parameters are to be sent.
        @exception ParameterNotSetException - Thrown when the parameter is not set in the
        event.
      */
    public byte[] getNetworkTransport()
        throws ParameterNotSetException {
        if (m_ntp != null)
            return m_ntp;
        throw new ParameterNotSetException("NetworkTransport not set.");
    }
    /** Sets the NetworkTransport parameter of the message.
        @param ntp - Network Transport parameter. This is a byte array, the way in which
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
    /** Gets the ConReq parameter of the message.
        @return The ConReq parameter of the event.
        @exception ParameterNotSetException - Thrown when the parameter is not set in the
        event ITU.
      */
    public ConReq getConReq()
        throws ParameterNotSetException {
        if (m_connectionReq != null)
            return m_connectionReq;
        throw new ParameterNotSetException("ConRq not set.");
    }
    /** Sets the connection request parameter.
        @param connectionReq - Connection Request parameter.
      */
    public void setConReq(ConReq connectionReq) {
        m_connectionReq = connectionReq;
    }
    /** Indicates if the ConReq parameter is present in this Event.
        @return True if the parameter is set.
      */
    public boolean isConReqPresent() {
        return (m_connectionReq != null);
    }
    /** The toString method retrieves a string containing the values of the members of the
        InformationReqAnsiEvent class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.ansi.InformationReqAnsiEvent");
        b.append("\n\tm_ntp: ");
        if (m_ntp != null)
            b.append(JainSS7Utility.bytesToHex(m_ntp, 0, m_ntp.length));
        b.append("\n\tm_connectionReq: " + m_connectionReq);
        return b.toString();
    }
    protected byte[] m_ntp = null;
    protected ConReq m_connectionReq = null;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
