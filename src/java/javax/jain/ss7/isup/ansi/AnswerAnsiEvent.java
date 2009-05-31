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

/** An ISUP Event : The AnswerAnsiEvent class is a sub class of Core AnswerEvent class
  * and is used to handle the ANSI variant of ANM message.
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class AnswerAnsiEvent extends AnswerEvent {
    /** Constructs a new AnswerAnsiEvent, with only the JAIN ISUP Mandatory parameters,
      * being supplied to the constructor.
      * @param source   The source of this event
      * @param dpc   The destination point code
      * @param opc   The origination point code
      * @param sls   The signaling link selection
      * @param cic   The CIC on which the call has been established
      * @param congestionPriority   Priority of the ISUP message which may be used in
      * the optional national congestion control procedures at MTP3. Refer to
      * getCongestionPriority method in IsupEvent class for more details.
      * @exception ParameterRangeInvalidException   Thrown when value is out of range */
    public AnswerAnsiEvent(java.lang.Object source, SignalingPointCode dpc, SignalingPointCode opc, byte sls, int cic, byte congestionPriority)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
    }
    /** Gets the BusinessGroupAnsi parameter of the message.
      * @return The BusinessGroupAnsi parameter of the event.
      * @exception ParameterNotSetException Thrown when the parameter is not set in the
      * event.  */
    public BusinessGroupAnsi getBusinessGroup() throws ParameterNotSetException {
        if (isBusinessGroupPresent())
            return m_bgp;
        throw new ParameterNotSetException("Business Group not set.");
    }
    /** Sets the BusinessGroupAnsi parameter of the message.
      * @param bgp   business group parameter */
    public void setBusinessGroup(BusinessGroupAnsi bgp) {
        m_bgp = bgp;
    }
    /** Indicates if the BusinessGroupAnsi parameter is present in this Event.
      * @return True if the parameter is set.  */
    public boolean isBusinessGroupPresent() {
        return (m_bgp != null);
    }
    /** Gets the InformationIndicators parameter of the message.
      * @return The InformationIndicators parameter of the event.
      * @exception ParameterNotSetException Thrown when the parameter is not set in the
      * event.  */
    public InfoIndAnsi getInfoInd() throws ParameterNotSetException {
        if (isInfoIndPresent())
            return m_iip;
        throw new ParameterNotSetException("Information Indicators not set.");
    }
    /** Sets the InformationIndicators parameter of the message.
      * @param ini   Information indicator parameter.  */
    public void setInfoInd(InfoIndAnsi ini) {
        m_iip = ini;
    }
    /** Indicates if the InformationIndicators parameter is present in this Event.
      * @return True if the parameter is set.  */
    public boolean isInfoIndPresent() {
        return (m_iip != null);
    }
    /** Gets the NetworkTransport parameter of the message.
      * @return The NetworkTransport parameter of the event. This is a byte array, the
      * way in which the byte array is filled is; Fill the Name of the parameter, the
      * length of the parameter and the parameter contents.  This is repeated if
      * multiple parameters are to be sent.
      * @exception ParameterNotSetException Thrown when the parameter is not set in the
      * event.  */
    public byte[] getNetworkTransport() throws ParameterNotSetException {
        if (isNetworkTransportPresent())
            return m_ntp;
        throw new ParameterNotSetException("Network Transport not set.");
    }
    /** Sets the NetworkTransport parameter of the message.
      * @param ntp   Network Transport parameter. This is a byte array, the way in
      * which the byte array is filled is; Fill the Name of the parameter, the length
      * of the parameter and the parameter contents. This is repeated if multiple
      * parameters are to be sent.  */
    public void setNetworkTransport(byte[] ntp) {
        m_ntp = ntp;
    }
    /** Indicates if the NetworkTransport parameter is present in this Event.
      * @return True if the parameter is set.  */
    public boolean isNetworkTransportPresent() {
        return (m_ntp != null);
    }
    /** Gets the ConReq parameter of the message.
      * @return The ConReq parameter of the event.
      * @exception ParameterNotSetException Thrown when the parameter is not set in the
      * event.  */
    public ConReq getConReq() throws ParameterNotSetException {
        if (isConReqPresent())
            return  m_crp;
        throw new ParameterNotSetException("Connection Request is not set.");
    }
    /** Sets the connection request parameter.  */
    public void setConReq(ConReq connectionReq) {
        m_crp = connectionReq;
    }
    /** Indicates if the ConReq parameter is present in this Event.
      * @return True if the parameter is set.  */
    public boolean isConReqPresent() {
        return (m_crp != null);
    }
    /** The toString method retrieves a string containing the values of the members of
      * the AnswerAnsiEvent class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer buffer = new StringBuffer(500);
        buffer.append(super.toString());
        buffer.append("\nAnswerAnsiEvent:");
        if (isBusinessGroupPresent())
            buffer.append("\n\tBusiness Group: " + m_bgp);
        if (isInfoIndPresent())
            buffer.append("\n\tInformation Indicators: " + m_iip);
        if (isNetworkTransportPresent()) {
            buffer.append("\n\tNetwork Transport: ");
            buffer.append(JainSS7Utility.bytesToHex(m_ntp, 0, m_ntp.length));
        }
        if (isConReqPresent())
            buffer.append("\n\tConnection Request: " + m_crp);
        return buffer.toString();
    }

    private BusinessGroupAnsi m_bgp = null;
    private InfoIndAnsi m_iip = null;
    private byte[] m_ntp = null;
    private ConReq m_crp = null;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
