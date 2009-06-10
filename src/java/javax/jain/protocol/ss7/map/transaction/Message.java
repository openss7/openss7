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

package javax.jain.protocol.ss7.map.transaction;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/**
  * This class is a base class of different message classes.
  * This class has a parameter that can be used to identify the family
  * that the message belongs to, i.e. if it is an ETSI message, an ETSI
  * PDU encoded according to GSM 03.40 or if it is an ANSI 41 message.
  * What family of messages to use depends on how the MAP API is
  * implemented: <p>
  *
  * If implemented on an ANSI MAP stack, use the ANSI_MESSAGE family <p>
  *
  * If implemented on an ETSI MAP stack, above the GSM SMS protocol (GSM
  * 03.40) use the ETSI_MESSAGE family <p>
  *
  * If implemented on an ETSI MAP stack, below the GSM SMS protocol (GSM
  * 03.40) use the ETSI_PDU family <p>
  *
  * The following rules applies for the setting of parameter components:
  * <ul>
  *
  * <li>Mandatory component with no default value must be provided to
  * the constructor.
  *
  * <li>Mandatory component with default value need not be set.
  *
  * <li>Optional / conditional component is by default not present. Such
  * a component becomes present by setting it. </ul>
  *
  * <h4>Parameter components:</h4><ul>
  *
  * <li>typeOfMessage, mandatory component with no default, read-only
  * </ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract class Message extends MapParameter {
    /** Identifies the ANSI_MESSAGE family.  */
    public static final int ANSI_MESSAGE = 0;
    /** Identifies the ETSI_MESSAGE family.  */
    public static final int ETSI_MESSAGE = 1;
    /** Identifies the ETSI_PDU family.  */
    public static final int ETSI_PDU = 2;
    /**
      * The constructor.
      * @param typeOfMessage
      * Integer that identifies what family the message belongs to.
      * Possible values: <ul>
      * <li>ANSI_MESSAGE
      * <li>ETSI_MESSAGE
      * <li>ETSI_PDU </ul>
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public Message(int typeOfMessage)
        throws SS7InvalidParamException {
        super();
        setTypeOfMessage(typeOfMessage);
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public Message()
        throws SS7InvalidParamException {
    }
    /**
      * Get the Type Of Message parameter.
      * @return
      * Integer that identifies what family the message belongs to. (See
      * the constructor for possible values.)
      */
    public int getTypeOfMessage() {
        return m_typeOfMessage;
    }
    protected void setTypeOfMessage(int typeOfMessage)
        throws SS7InvalidParamException {
        switch (typeOfMessage) {
            case ANSI_MESSAGE:
            case ETSI_MESSAGE:
            case ETSI_PDU:
                m_typeOfMessage = typeOfMessage;
                m_typeOfMessageIsSet = true;
                break;
            default:
                throw new SS7InvalidParamException("Type of Message " + typeOfMessage + " out of range.");
        }
    }
    protected int m_typeOfMessage = 0;
    protected boolean m_typeOfMessageIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
