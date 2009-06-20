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

package jain.protocol.ss7.oam.sccp;

import javax.management.*;

import jain.protocol.ss7.oam.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * This Notification may be emitted by any Sccp Managed MBean if a user defined error is
  * encountered. <p>
  *
  * This SccpErrorNotification should be sent to all applications that have registered
  * with the Sccp MBean as a javax.management.NotificationListener using a filter 'f',
  * where f.isNotificationEnabled(this.getType()) == true. <p>
  *
  * The Notification class extends the java.util.EventObject base class and defines the
  * minimal information contained in a notification. It contains the following fields:
  * <ul>
  *
  * <li> the <em>notification type</em>, which is a string expressed in a dot notation
  * similar to Java properties.
  *
  * <li> a <em>sequence number</em>, which is a serial number identifying a particular
  * instance of notification in the context of the notification source
  *
  * <li> a <em>time stamp</em>, indicating when the notification was generated
  *
  * <li> a <em>message</em> contained in a string, which could be the explanation of the
  * notification for displaying to a user
  *
  * <li> <em>userData</em> is used for whatever other data the notification source wishes
  * to communicate to its consumers </ul>
  *
  * Notification sources should use the notification type to indicate the nature of the
  * event to their consumers. When additional information needs to be transmitted to
  * consumers, the source may place it in the message or user data fields. <p>
  *
  * <center> [NotificationHierarchy.jpg] </center><br>
  * <center> Inheritance hierarchy for JAIN OAM Notification </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class SccpErrorNotification extends OamErrorNotification {
    /**
      * Syntax error in message received from the network. <p>
      * <br> Emitted By: SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.error.msg_syntax".
      */
    public static final int SCCP_ERROR_MSG_SYNTAX = 1;
    /**
      * Invalid network message. <p>
      * <br> Emitted By: SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.error.msg_invalid".
      */
    public static final int SCCP_ERROR_MSG_INVALID = 2;
    /**
      * Parameter in a message is out of range. <p>
      * <br> Emitted By: SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.error.msg_param_out_of_range".
      */
    public static final int SCCP_ERROR_MSG_PARAM_OUT_OF_RANGE = 3;
    /**
      * Error encountered while decoding SCCP message. <p>
      * <br> Emitted By: SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.error.msg_decoding".
      */
    public static final int SCCP_ERROR_MSG_DECODING = 4;
    /**
      * Invalid message type. <p>
      * <br> Emitted By: SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.error.msg_invalid_type".
      */
    public static final int SCCP_ERROR_MSG_INVALID_TYPE = 5;
    /**
      * Routing error - Invalid route or route not available. <p>
      * <br> Emitted By: SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.error.routing_error".
      */
    public static final int SCCP_ERROR_ROUTING_ERROR = 6;
    /**
      * Illegal timer expiry in the SCCP software. This may happen if a timer expires in a
      * state where this timer was not supposed to be active. <p>
      * <br> Emitted By: SccpTimerProfileMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.error.illegal_timer_expiry".
      */
    public static final int SCCP_ERROR_ILLEGAL_TIMER_EXPIRY = 7;
    /**
      * Constructs a new SccpErrorNotification of the specified Error Type.
      *
      * @param source
      * the source of the error
      *
      * @param sequenceNumberThe
      * notification sequence number within the source object.
      *
      * @param errorType
      * one of the defined Error Types.
      *
      * @exception IllegalArgumentException
      * If any of the supplied parameters are invalid
      */
    public SccpErrorNotification(OamManagedObjectMBean source, long sequenceNumber, int errorType)
        throws IllegalArgumentException {
    }
    /**
      * Returns a string representation (with details) of classes which extend this class.
      * Over rides standard JAVA toString method.
      */
    public java.lang.String toString() {
        return new java.lang.String("");
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
