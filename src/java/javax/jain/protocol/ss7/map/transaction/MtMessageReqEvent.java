/*
 @(#) src/java/javax/jain/protocol/ss7/map/transaction/MtMessageReqEvent.java <p>
 
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

package javax.jain.protocol.ss7.map.transaction;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/**
  * This primitive (event) is used by the MAP API User to send a text
  * message that shall be delivered to the Mobile Station (a Mobile
  * Terminated, MT, message).
  * The underlying protocol service is Short Message Service (SMS). The
  * API implementation is responsible for segmentation if the message is
  * longer than the protocol can handle. <p>
  *
  * The following rules applies for the setting of primitive parameters:
  * <ul>
  *
  * <li>Mandatory parameters with no default value must be provided to
  * the constructor.
  *
  * <li>Mandatory parameters with default value need not be set.
  *
  * <li>Optional / conditional parameter is by default not present. Such
  * a parameter becomes present by setting it. </ul>
  *
  * <h4>Primitive parameters:</h4><ul>
  *
  * <li>msId (Mobile Station Id), mandatory parameter with no default
  *
  * <li>message, mandatory parameter with no default
  *
  * <li>availabilityNotif, mandatory parameter with no default
  *
  * <li>originatingAddress, mandatory parameter with no default </ul>
  *
  * @serial
  * @author Monavacon Limited
  * @version 1.2.2
  */
public final class MtMessageReqEvent extends TransactionEvent {
    /**
      * The constructor.
      *
      * @param source
      * The source of this event. Should be set to the Listner object
      * for the transaction.
      * @param msId
      * Identifier (MSISDN or MIN) of the Mobile Station to which the
      * message is sent.
      * @param message
      * The message to be sent.
      * @param availabilityNotif
      * A flag indicating if a notification is desired when an
      * unavailable MS becomes available.
      * @param originatingAddress
      * The "phone number" of the Service Centre sending this message.
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public MtMessageReqEvent(java.lang.Object source, MsNumber msId, Message message,
            boolean availabilityNotif, SS7Number originatingAddress)
        throws SS7InvalidParamException {
        super(source, MT_MESSAGE_REQ_ID);
        setMsId(msId);
        setMessage(message);
        setAvailabilityNotif(availabilityNotif);
        setOriginatingAddress(originatingAddress);
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public MtMessageReqEvent()
        throws SS7InvalidParamException {
        super(null, MT_MESSAGE_REQ_ID);
    }
    /**
      * Change the Id of the Mobile Station to which the message is
      * sent.
      * @param msId
      * Identifier (MSISDN or MIN) of the Mobile Station to which the
      * message is sent.
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setMsId(MsNumber msId)
        throws SS7InvalidParamException {
        m_msId = msId;
        m_msIdIsSet = (msId != null);
    }
    /**
      * Get the Id of the Mobile Station to which the message is sent.
      * @return
      * Identifier (MSISDN or MIN) of the Mobile Station to which the
      * message is sent.
      */
    public MsNumber getMsId() {
        return m_msId;
    }
    /**
      * Change the message.
      * Different "families" of messages are used depending on what
      * protocol that is used (ETSI MAP or ANSI MAP), and in the case of
      * ETSI MAP also depending on if the API is implemented above or
      * below the SMS protocol (GSM 03.40).
      * @param message
      * The message to be sent.
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setMessage(Message message)
        throws SS7InvalidParamException {
        m_message = message;
        m_messageIsSet = (message != null);
    }
    /**
      * Get the message.
      * @return
      * The message to be sent.
      */
    public Message getMessage() {
        return m_message;
    }
    /**
      * Change the Availability Notification flag.
      * In the case that the MS to which the message is sent is not
      * available immediately to receive the message, then if this flag
      * is set the API implementation shall perform necessary protocol
      * actions so that the network sends a notification when the MS
      * becomes available.
      * @param availabilityNotif
      * A flag indicating if a notification is desired when an
      * unavailable MS becomes available.
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setAvailabilityNotif(boolean availabilityNotif)
        throws SS7InvalidParamException {
        m_availabilityNotif = availabilityNotif;
        m_availabilityNotifIsSet = true;
    }
    /**
      * Get the Availability Notification flag.
      * @return
      * A flag indicating if a notification is desired when an
      * unavailable MS becomes available.
      */
    public boolean getAvailabilityNotif() {
        return m_availabilityNotifIsSet;
    }
    /**
      * Change the originating address, which is the address of the
      * Service Centre sending this message.
      * The Jain MAP API User itself may be acting in the role of a
      * "Service Centre" (e.g. if it is a Service Application using
      * messaging capabilities).  Alternatively the Jain MAP API User
      * may relay messages to/from a remote Service Centre.
      * @param originatingAddress
      * The "phone number" of the Service Centre sending this message.
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setOriginatingAddress(SS7Number originatingAddress)
        throws SS7InvalidParamException {
        m_originatingAddress = originatingAddress;
        m_originatingAddressIsSet = true;
    }
    /**
      * Get the originating address, which is the address of the Service
      * Centre sending this message.
      * @return
      * The "phone number" of the Service Centre sending this message.
      */
    public SS7Number getOriginatingAddress() {
        return m_originatingAddress;
    }
    protected MsNumber m_msId = null;
    protected boolean m_msIdIsSet = false;
    protected Message m_message = null;
    protected boolean m_messageIsSet = false;
    protected boolean m_availabilityNotif;
    protected boolean m_availabilityNotifIsSet = false;
    protected SS7Number m_originatingAddress = null;
    protected boolean m_originatingAddressIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
