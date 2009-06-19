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

package javax.csapi.cc.jcc;

/** The Event interface is the parent of all JCC Event interfaces. Event
  * interfaces within each package are organized in a hierarchical
  * fashion. <p>
  *
  * Event objects correspond to the object which is undergoing a state
  * change; the specific state change is conveyed to the application in
  * two ways. <p>
  *
  * First, the implementation reports the event to a particular method
  * in a particular Listener interface to a listening object; generally
  * the method corresponds to a particular state change. <p>
  *
  * Second, the event that is presented to the method has an
  * identification integer which indicates the specific state change.
  * The getID() method returns this identification number for each
  * event. The actual event identification integer values that may be
  * conveyed by the individual event object are defined in each of the
  * specific event interfaces. <p>
  *
  * Each event caries a cause or a reason why the event happened. The
  * getCause() method returns this cause value.
  *
  * @since 1.0b
  * @version 1.2.2
  * @author Monavacon Limited
  */
public interface JccEvent {
    /** Cause code indicating a normal operation.  */
    public static final int CAUSE_NORMAL = 100;
    /** Cause code indicating the cause was unknown.  */
    public static final int CAUSE_UNKNOWN = 101;
    /** Cause code indicating the user has terminated call. This can be
      * either because of the user going on-hook or because the call has
      * been terminated by the service logic.  */
    public static final int CAUSE_CALL_CANCELLED = 102;
    /** Cause code indicating the destination is not available.  */
    public static final int CAUSE_DEST_NOT_OBTAINABLE = 103;
    /** Cause code indicating that a call has encountered an
      * incompatible destination.  */
    public static final int CAUSE_INCOMPATIBLE_DESTINATION = 104;
    /** Cause code indicating that a call has encountered an inter-digit
      * timeout while dialing.  */
    public static final int CAUSE_LOCKOUT = 105;
    /** Cause code indicating a new call.  */
    public static final int CAUSE_NEW_CALL = 106;
    /** Cause code indicating that resources were not available.  */
    public static final int CAUSE_RESOURCES_NOT_AVAILABLE = 107;
    /** Cause code indicating that a call has encountered network
      * congestion.  */
    public static final int CAUSE_NETWORK_CONGESTION = 108;
    /** Cause code indicating that a call could not reach a destination
      * network.  */
    public static final int CAUSE_NETWORK_NOT_OBTAINABLE = 109;
    /** Cause code indicating that the event is part of a snapshot of
      * the current state of the call.  */
    public static final int CAUSE_SNAPSHOT = 110;
    /** Cause code indicating the cause was because of call being
      * redirected. If this cause code occurs, the event id. is always
      * JccConnectionEvent.CONNECTION_DISCONNECTED and the connection
      * returned by JccConnectionEvent.getConnection() is a terminating
      * connection.  */
    public static final int CAUSE_REDIRECTED = 111;
    /** Cause code indicating that the network needs more addressing
      * information to complete the call.  */
    public static final int CAUSE_MORE_DIGITS_NEEDED = 112;
    /** The user is busy. This value may be returned by getCause().  */
    public static final int CAUSE_BUSY = 113;
    /** No answer was received. This value may be returned by
      * getCause().  */
    public static final int CAUSE_NO_ANSWER = 114;
    /** A disconnect was received. This value may be returned by
      * getCause().  */
    public static final int CAUSE_CALL_RESTRICTED = 115;
    /** A general network failure occurred. This value may be returned
      * by getCause().  */
    public static final int CAUSE_GENERAL_FAILURE = 116;
    /** The connection was released because an activity timer expired.
      * This value may be returned by getCause().  */
    public static final int CAUSE_TIMER_EXPIRY = 117;
    /** The user isn't available in the network. This means that the
      * number isn't allocated or that the user isn't registered. This
      * value may be returned by getCause().  */
    public static final int CAUSE_USER_NOT_AVAILABLE = 118;
    /** Returns the cause associated with this event. Every event has a
      * cause. The various cause values are defined as public static
      * final variables in this interface.
      * @return
      * The cause of the event.  */
    public int getCause();
    /** Returns the id of event. Every event has an id.
      * @return
      * The id of the event.  */
    public int getID();
    /** Returns the event source of the event.
      * @return
      * The object sending the event.  */
    public java.lang.Object getSource();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
