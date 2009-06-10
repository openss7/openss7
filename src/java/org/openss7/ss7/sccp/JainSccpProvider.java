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

package org.openss7.ss7.sccp;

import org.openss7.ss7.*;

/**
  * The JainSccpProvider interface models the services available from an
  * JainSccpProvider implementation.  Each vendor-sepcific
  * JainSccpProviderImpl class will implement the JainSccpProvider
  * interface. <p>
  *
  * This interface provides the implementation object with the ability
  * to offer services to {@link JainSccpListener} implementation objects
  * (application). <p>
  *
  * This interface must be implemented by any Object representing the
  * SCCP layer of any SS7 stack that interacts directly with a
  * proprietary (stack vendor specific) implementation of the SCCP
  * layer.  This interface defines the methods that will be used by any
  * registered SCCP User application implementing the
  * {@link JainSccpListener} interface to send connection-oriented or
  * connectionless primitives. <p>
  *
  * It must be noted that any object that implements the: <ul>
  *
  * <li>JainSccpListener interface is referred to as JainSccpListenerImpl;
  *
  * <li>JainSccpProvider Interface is referred to as JainSccpProviderImpl;
  *
  * <li>JainSccpStack Interace is referred to as JainSccpStackImpl. </ul>
  *
  * A SCCP User application may send a number of Connection Request
  * primitives as SccpEvent events to a provider object.
  *
  * Since the same JainSccpProviderImpl will be used to handle a
  * particular connection, Connection-Oriented Events with the same
  * Local Reference Id must be sent to the same
  * JainSccpProviderImpl. <p>
  *
  * This interface defines methods for: <ul>
  *
  * <li>Sending {@link SccpEvent SccpEvents}.
  * This will trigger the sending of SCCP connnectionless or
  * connection-oriented service
  * primitives to the destination node.
  *
  * <li>Adding a {@link JainSccpListener JainSccpListener} to its list
  * of registered Event Listeners.  The JainSccpListenerImpl will
  * register with the JainSccpProviderImpl with a User Address and
  * become an Event Listener of that JainSccpProviderImpl object.  Once
  * registered as an Event Listener, a JainSccpListenerImpl will receive
  * any connectionless or connection-oriented or management indication
  * primitives addressed to its User Address as Events.
  *
  * <li>Removing a {@link JainSccpListener JainSccpListener} from the
  * JainSccpProviderImpl's list of Event Listeners.  Once a
  * JainSccpListenerImpl is de-registered, it will no longer receive any
  * Events from that JainSccpProviderImpl.
  *
  * <li>Retrieving a new Session Id from SCCP.
  *
  * <li>Releasing a Session Id back into SCCP. </ul>
  *
  * @author     Monavacon Limited
  * @version    1.2.2
  * @see        JainSccpListener
  * @see        JainSccpStack
  */
public interface JainSccpProvider extends java.util.EventListener {

    public JainSccpListeners[] getSccpListeners();

    /**
      * Returns a unique Session Id to initiate a session with another
      * SCCP user.
      *
      * @return
      * The new Session Id returned by the underlying SCCP layer.
      *
      * @exception IdNotAvailableException
      * If a new Session Id is not available.
      */
    public int getNewSessionId()
        throws IdNotAvailableException;

    /**
      * Release the session Id back to the system. <p>
      * <b>Note:</b> In some SS7 stacks the SCCP Session Id is
      * automatically released following the end of a SCCP connection.
      * In such instances the implementation of this method may be left
      * null.
      *
      * @param  sessionId
      * The session Id supplied to the method.
      *
      * @exception IdNotAvailableException
      * If a new Session Id is not available.
      */
    public void releaseSessionId(int sessionId)
        throws IdNotAvailableException;

    /**
      * Returns a unique Reference Id for identifying connection requests
      * within the session identified by the supplied Session Id.
      * Each session between two <code>JainSccpListeners</code> is
      * identified by a unique Session Id. Note that the returned
      * Reference Id will be unique for a particular Session Id.
      *
      * @param sessionId
      * The session Id supplied to the method.
      *
      * @return
      * An unused unique Reference Id.
      *
      * @exception IdNotAvailableException
      * If the reference Id is not avialable to the specified session Id.
      */
    public int getNewReferenceId(int sessionId)
        throws IdNotAvailableException;

    /**
      * Releases the unique Reference Id, allows it to be reused within
      * the session identified by the supplied session Id.
      *
      * @param referenceId
      * The reference Id to be released.
      *
      * @param sessionId
      * The session Id from which to release the reference Id.
      *
      * @exception IdNotAvailableException
      * If the reference Id is not available to the specified session Id.
      */
    public void releaseReferenceId(int referenceId, int sessionId)
        throws IdNotAvailableException;

    /**
      * Sends a Connection-Oriented primitive into the TCAP layer of the
      * SS7 protocol stack.
      *
      * @param event
      * The new connection-oriented event supplied to the method.
      *
      * @exception MandatoryParameterNotSetException
      * Thrown if all of the mandatory parameters required by this
      * JainSccpProviderImpl, to send the Connection-Oriented Event are not set.
      * <p>
      *
      * Note that different implementations of this JainSccpProvider
      * interface will mandate that different parameters must be set for
      * each <code>ConnectionOrientedEvent</code> . It is recommended that the
      * message detail returned in the
      * <code>MandatoryParameterNotSetException</code> should be a
      * <code>String</code> of the form: <p>
      * <center><B>"Parameter: <parameterName> not set"</B></center>
      */
    public void sendConnectOrientedEvent(ConnectionOrientedEvent event)
        throws MandatoryParameterNotSetException;

    /**
      * Sends a Connectionless primitive to the SCCP layer of the
      * SS7 protocol stack.  This will trigger the transmission to the
      * destination node of the connectionless primtive along with
      * any associated data.
      *
      * Since the same JainSccpProviderImpl will be used to handle a
      * particular transaction, Connectionless Events with the same
      * Sequence Control muse be sent to the same
      * JainSccpProviderImpl.
      *
      * @param event
      * The new connectionless event supplied to the method.
      *
      * @exception MandatoryParameterNotSetException
      * Thrown if all of the mandatory parameters required by this
      * JainSccpProviderImpl to send the Connectionless event are not set.
      * <p>
      *
      * <b>Note to developers</b> :- different implementations of this
      * JainSccpProvider interface will mandate that different
      * parameters must be set for each <code>ConnectionlessEvent</code>.
      * It is recommended that the detail message returned in the
      * <code>MandatoryParameterNotSetException</code> should be a
      * <code>String</code> of the form: <p>
      * <center><B>"Parameter: <parameterName> not set"</B></center>
      */
    public void sendConnectionlessEvent(ConnectionlessEvent event)
        throws MandatoryParameterNotSetException;

    /**
      */

    /**
      * Adds a {@link JainSccpListener} to the list of registered Event
      * Listeners of this JainSccpProviderImpl.
      *
      * @param listener
      * The feature to be added to the JainSccpListener attribute.
      *
      * @param userAddress
      * The feature to be added to the JainSccpListener attribute.
      *
      * @exception TooManyListenersException
      * Thrown if a limit is placed on the allowable number of
      * registered JainSccpListeners, and this limit has been reached.
      *
      * @exception ListenerAlreadyRegisteredException
      * Thrown if the listener supplied is already registered for the
      * specified userAddress with this JainSccpProviderImpl.
      *
      * @exception InvalidAddressException
      * Thrown if the userAddress supplied is not a valid address.
      */
    public void addJainSccpListener(JainSccpListener listener, SccpUserAddress userAddress)
        throws TooManyListenersException, ListenerAlreadyRegisteredException, InvalidAddressException;

    /**
      * Removes a {@link JainSccpListener} from the list of registered
      * JainSccpListeners of thie JainSccpProviderImpl.
      *
      * @param listener
      * The listener to be removed from this provider.
      *
      * @exception ListenerNotRegisteredException
      * Thrown if there is no such listener registered with this
      * provider.
      */
    public void removeJainSccpListener(JainSccpListener listener)
        throws ListenerNotRegisteredException;

    /**
      * Returns the JainSccpStackImpl to which this JainSccpProviderImpl
      * is attached.
      *
      * @return
      * The attached JainSccpStack.
      *
      * @deprecated
      * As of JAIN SCCP v1.1. This class is no longer needed as a result
      * of the addition of the
      * {@link org.openss7.ss7.sccp.JainSccpProvider#getStack()}
      * method.  The reason for deprecating this method is that the
      * provider is attached implicitly.
      */
    public JainSccpStack getAttachedStack();

    /**
      * Returns the JainSccpStackImpl to which this JainSccpProviderImpl
      * is attached.
      *
      * @return
      * The attached JainSccpStack.
      * @since 1.1
      */
    public JainSccpStack getStack();

    /**
      * @deprecated
      * As of JAIN SCCP v1.1. No replacement, the JainSccpProvider is
      * attached implicitly within the
      * {@link JainSccpStack#createProvider() createProvider} method
      * call in the <code>JainSccpStack</code> Interface.
      */
    public boolean isAttached();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
