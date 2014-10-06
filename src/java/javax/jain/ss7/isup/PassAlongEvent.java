/*
 @(#) $RCSfile: PassAlongEvent.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:05 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:05 $ by $Author: brian $
 */

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/**
  * An ISUP EVENT: The Pass Along class is a sub class of the IsupEvent class and is
  * exchanged between an ISUP Provider and an ISUP Listener for sending and receiving
  * the PAM message. ISUP Provider would send a Pass Along object to the listener on
  * the reception of one of PAM message from the stack, for the user address handled
  * by that listener.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class PassAlongEvent extends IsupEvent {
    /** Constructs a new PassAlongEvent, with the JAIN ISUP Mandatory parameters being
      * supplied to the constructor.
      * @param source  Source of the object.
      * @param dpc  The destination point code.
      * @param opc  The origination point code.
      * @param sls  Signaling link selection for load sharing.
      * @param cic  CIC on which the error has been raised.
      * @param congestionPriority  Priority of the ISUP message which may be used in
      * the optional national congestion control procedures at MTP3. Refer to
      * getCongestionPriority method in IsupEvent class for more details.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public PassAlongEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
    }
    /** Gets the ISUP PASS ALONG primtive value.
      * @return The ISUP PASS ALONG primitive value.  */
    public int getIsupPrimitive() {
        return IsupConstants.ISUP_PRIMITIVE_PASS_ALONG;
    }
    /** Gets the EmbeddedMessage parameter of the message.  EmbeddedMessage can be
      * used to send and receive propreitary parameter information.
      * @return The EmbeddedMessage parameter which is an array of uninterpreted
      * bytes.
      * @exception ParameterNotSetException  Is thrown if this parameter has not been
      * set.  */
    public byte[] getEmbeddedMessage()
        throws ParameterNotSetException {
        if (m_embeddedMessage!= null)
            return m_embeddedMessage;
        throw new ParameterNotSetException("Embedded Message is not set.");
    }
    /** Sets the EmbeddedMessage parameter of the message.  EmbeddedMessage can be
      * used to send and receive propreitary parameter information
      * @param embeddedMessage  isupParam is array of bytes not interpreted by the
      * Provider/Stack.  */
    public void setEmbeddedMessage(byte[] embeddedMessage) {
        m_embeddedMessage = embeddedMessage;
    }
    /** Indicates if the EmbeddedMessage parameter is present in this Event.
      * @return True if the parameter has been set, false otherwise.  */
    public boolean isEmbeddedMessagePresent() {
        return (m_embeddedMessage != null);
    }
    /** The toString method retrieves a string containing the values of the members of
      * the PassAlongEvent class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.PassAlongEvent:");
        b.append("\n\t#m_embeddedMessage: ");
        if (m_embeddedMessage != null)
            b.append(JainSS7Utility.bytesToHex(m_embeddedMessage, 0, m_embeddedMessage.length));
        return b.toString();
    }
    protected byte[]  m_embeddedMessage = null;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
