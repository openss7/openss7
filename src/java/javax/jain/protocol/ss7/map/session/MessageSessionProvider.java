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

package javax.jain.protocol.ss7.map.session;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/**
  * This interface defines the methods required to provide the Message
  * Session Capability on a MAP protocol stack, the implementation of
  * which will be vendor specific.
  * The object that implements the MessageSessionProvider interface must
  * be named MessageSessionProviderImpl. <p>
  *
  * The methods defined in this interface will be used by the MAP API
  * User application to send the following primitives (events) to the
  * Provider object: <ul>
  *
  * <li>SessionOpenReqEvent, sent with the method
  * processMessageSessionOperation(SessionOpenReqEvent
  * event,MessageSessionListener listener)
  *
  * <li>SessionDataReqEvent, sent with the method
  * processMessageSessionOperation(SessionDataReqEvent event, long
  * sessionId)
  *
  * <li>SessionCloseReqEvent, sent with the method
  * processMessageSessionOperation(SessionCloseReqEvent event, long
  * sessionId) </ul>
  *
  * When the MAP API User is sending a SessionOpenReqEvent it
  * initiatesinitiates an "outgoing session". The MAP API User can also
  * register listeners for "incoming sessions" with the method
  * addMessageSessionListener, and de-register those listeners with the
  * method removeMessageSessionListener. "Incoming sessions" are
  * sessions initiated by a Mobile Station. <p>
  *
  * When registering a listener a userAddress (an SS7Address) is
  * provided to which the listener is listening. If the listener shall
  * listen to several addresses then several calls to
  * addMessageSessionListener have to be made for the same listener. The
  * provider may only forward events to the listener that have an
  * address that is matching one of the registered listener's addresses.
  * <p>
  *
  * If several listeners are registered with the same userAddress, then
  * the provider must send events for that address to all listeners
  * ("multicast"). The provider must however ensure that only one
  * listener continue to communicate on the session, by rejecting all
  * but the first listener that sends an event on the session. <p>
  *
  * Note: A listener registered with the addMessageSessionListener may
  * be a "default listener", that only instantiates a new listener to be
  * used for each incoming session that is opened. Therefore the
  * provider implementation always must check the source of events from
  * the MAP API User on each incoming session and subsequently use that
  * source as the listener on the session.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public interface MessageSessionProvider {
    /**
      * This method is used by the JAIN MAP API User to open a new
      * message session by sending a SessionOpenReqEvent to the JAIN MAP
      * API Provider.
      * The session is identified by a sessionId which is allocated by
      * the provider. <p>
      *
      * A session opened with this method is called an "outgoing
      * session" and the listener used to handle events from the
      * provider on such a session is provided as a parameter in this
      * method (and not by using the addMessageSessionListener method).
      *
      * @param event
      * A reference to the SessionOpenReqEvent object.
      * @param listener
      * A reference to the listner object that the JAIN MAP API User
      * will use to receive incoming events on this session.
      * @return
      * A sessionId of the new session is returned.
      * @exception SS7MandatoryParameterNotSetException
      * Thrown if a mandatory parameter of the SessionOpenReqEvent is
      * not set.
      */
    public long processMessageSessionOperation(SessionOpenReqEvent event, MessageSessionListener listener)
        throws SS7MandatoryParameterNotSetException;
    /**
      * This method is used by the JAIN MAP API User to send an event of
      * the type SessionDataReqEvent to the JAIN MAP API Provider.
      * The session is identified by a sessionId which was allocated by
      * the provider when the session was opened.
      * @param event
      * A reference to the event object (a SessionDataReqEvent object).
      * @param sessionId
      * The identifier of the session.
      * @exception SS7MandatoryParameterNotSetException
      * Thrown if a mandatory parameter of the event is not set.
      */
    public void processMessageSessionOperation(SessionDataReqEvent event, long sessionId)
        throws SS7MandatoryParameterNotSetException;
    /**
      * This method is used by the JAIN MAP API User to send an event of
      * the type SessionCloseReqEvent to the JAIN MAP API Provider.
      * The session is identified by a sessionId which was allocated by
      * the provider when the session was opened.
      * @param event
      * A reference to the event object (a SessionCloseReqEvent object).
      * @param sessionId
      * The identifier of the session.
      * @exception SS7MandatoryParameterNotSetException
      * Thrown if a mandatory parameter of the event is not set.
      */
    public void processMessageSessionOperation(SessionCloseReqEvent event, long sessionId)
        throws SS7MandatoryParameterNotSetException;
    /**
      * This method is used by the JAIN MAP API User to register a
      * listener with the provider that is used to receive events on an
      * "incoming session", i.e, a session that is opened when a
      * SessionOpenIndEvent is sent by the provider to the registered
      * listener. <p>
      *
      * Note: Listeners used to handle events on "outgoing sessions" are
      * not registered with this method, but are sent to the provider as
      * a parameter in the processMessageSessionOperation method when
      * the SessionOpenReqEvent is sent.
      *
      * @param listener
      * The listener to be registered for "incoming sessions".
      * @param userAddress
      * The SS7Address that the listener is listening to.
      * @exception java.util.TooManyListenersException
      * Thrown if the provider cannot handle any more listeners.  This
      * exception normally is used to indicate that only one listener
      * can be registered!
      */
    public void addMessageSessionListener(MessageSessionListener listener, SS7Address userAddress)
        throws java.util.TooManyListenersException;
    /**
      * This method is used to de-register a listener previously
      * registered with the provider for "incoming sessions".
      * If the listener was registered for more than one address it is
      * removed as a listener for all the addresses. <p>
      *
      * Note: The same listener object may be used to handle events on
      * an "outgoing session" as well as to listen for "incoming
      * sessions". If such a listener is removed with this method it
      * stops listening for "incoming sessions", but it's role for
      * "outgoing sessions" are unaffected, i.e. the listener is used
      * for outgoing sessions until those sessions are closed.
      *
      * @param listener
      * The reference of the listener to be removed.
      * @exception SS7ListenerNotRegisteredException
      * Thrown when trying to remove a listener that is not registered
      * with this provider.
      */
    public void removeMessageSessionListener(MessageSessionListener listener)
        throws SS7ListenerNotRegisteredException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
