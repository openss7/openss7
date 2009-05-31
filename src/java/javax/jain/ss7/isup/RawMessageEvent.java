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

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** An ISUP EVENT: The RawMessageEvent class is a sub class of the IsupEvent class and
  * is exchanged between an ISUP Provider and an ISUP Listener for sending and
  * receiving an ISUP Event that is not defined in either ANSI or ITU standards.
  * The parameters in this event are not recognized by the JAIN ISUP layer and are
  * application specific. ISUP Provider would send an RawMessageEvent object to the
  * listener on the reception of an unrecognized message from the stack for the user
  * address handled by that listener. For example, this can be used to send the charge
  * message defined for a national variant. The Provider does not validate any field
  * of the Event other than those defined in the IsupEvent (which are OPC, DPC, CIC
  * and SLS). <p>
  * Implementation Notes:
  * This specification is deficient in that it provides no real means for sending
  * national variant messages that are not part of the International sepcification.
  * The ITU-T sepcification provides the ability to specify message compatability for
  * such messages using a message compatability indicator as an optionals parameter,
  * and for parameters using the parameter compatability indicator as an optional
  * parameter.  This class has no mechanism for this.  Further, the class does not
  * provide the ability to specify mandatory nor optional parameters, and does not
  * provide the abilty to include any information with the message whatsoever.
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class RawMessageEvent extends IsupEvent {
    /** Constructs a new RawMessageEvent, with only the JAIN ISUP Mandatory parameters
      * being supplied to the constructor.
      * @param source - The source of this event.
      * @param dpc - The destination point code.
      * @param opc - The origination point code.
      * @param sls - The signaling link selection.
      * @param cic - The CIC on which the call has been established.
      * @param congestionPriority - Priority of the ISUP message which may be used in
      * the optional national congestion control procedures at MTP3. Refer to
      * getCongestionPriority method in IsupEvent class for more details.
      * @param msgType - The isupMsgType is the value that is specified in the message
      * type field of ISUP Protocol messages. The range is 0-256.
      * @exception ParameterRangeInvalidException - Thrown when value is out of range.
      */
    public RawMessageEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
            int msgType)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
        if (0 > msgType || msgType > 256)
            throw new ParameterRangeInvalidException("msgType value " + msgType + " out of range.");
        this.setIsupMessageType(msgType);
    }
    /** Gets the primtive value corresponding to Event RAW MESSAGE.
      * @return The primitive value correspodning to Event RAW MESSAGE.  */
    public int getIsupPrimitive() {
        return IsupConstants.ISUP_PRIMITIVE_RAW_MESSAGE;
    }
    /** Gets the message type received in the RawMessage Event.
      * @return The message type received in the RawMessage Event, Range 0 to 256.  */
    public int getIsupMessageType() {
        return m_msgType;
    }
    /** Sets the message type received in the RawMessage Event.  */
    public void setIsupMessageType(int msgType)
        throws ParameterRangeInvalidException {
        if (0 > msgType || msgType > 256)
            throw new ParameterRangeInvalidException("mstType valud " + msgType + " out of rnage.");
        m_msgType = msgType;
    }
    /** The toString method retrieves a string containing the values of the members of
      * the RawMessageEvent class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.RawMessageEvent:");
        b.append("\n\tm_msgType: " + m_msgType);
        return b.toString();
    }
    protected int m_msgType;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
