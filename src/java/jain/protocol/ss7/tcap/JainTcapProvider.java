/*
 @(#) $RCSfile: JainTcapProvider.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:29 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:29 $ by $Author: brian $
 */

package jain.protocol.ss7.tcap;

import jain.protocol.ss7.*;
import jain.*;

/**
  * This interface must be implemented by any Object representing the
  * JAIN TCAP layer of any SS7 stack that interacts directly with a
  * proprietary (stack vendor specific) implementation of the TCAP
  * layer. This interface defines the methods that will be used by any
  * registered TCAP User application implementing the JainTcapListener
  * interface to send component primitives and dialogue handling
  * primitives. It must be noted that any object that implements the:
  * <ul>
  * <li>JainTcapListener Interface is referred to as
  * JainTcapListenerImpl.
  * <li>JainTcapProvider Interface is referred to as
  * JainTcapProviderImpl.
  * <li>JainTcapStack Interface is referred to as JainTcapStackImpl.
  * </ul>
  * A TCAP User application may send a number of Component Request
  * primitives as ComponentReqEvents to a Provider object. When the TCAP
  * User application (JainTcapListenerImpl) sends a Dialogue handling
  * request primitive as a DialogueReqEvent this will trigger the
  * JainTcapProviderImpl object to send of all of the associated
  * Component requests that have been previously passed to the
  * JainTcapProviderImpl.  Since the same JainTcapProviderImpl will be
  * used to handle a particular transaction, Dialogue Request Events
  * with the same Originating Transaction Id must be sent to the same
  * JainTcapProviderImpl. <p>
  * This interface defines methods for: <ul>
  * <li>Sending ComponentReqEvents . Numerous Component requests may be
  * sent to a JainTcapProviderImpl before they are transmitted to the
  * remote JainTcapProviderImpl.
  * <li>Sending DialogueReqEvents . This will trigger the transmission
  * of the Dialogue request primitive along with any associated
  * Component request primitives to the destination node
  * <li>Adding a JainTcapListener to its list of registered Event
  * Listeners. The JainTcapListenerImpl will register with the
  * JainTcapProviderImpl with an User Address and become an Event
  * Listener of that JainTcapProviderImpl object. Once registered as an
  * Event Listener, a JainTcapListenerImpl will receive any Dialogue
  * Indication primitives, and Component Indication primitives addressed
  * to its User Address as Events.
  * <li>Removing a JainTcapListener from the JainTcapProviderImpl's list
  * of Event Listeners. Once a JainTcapListenerImpl is de-registered, it
  * will no longer receive any Events from that JainTcapProviderImpl.
  * <li>Retrieving a new Dialogue Id from TCAP.
  * <li>Releasing a Dialogue Id back into TCAP. </ul>
  * @version 1.2.2
  * @author Monavacon Limited
  * @see JainTcapListener
  * @see JainTcapStack
  */
public interface JainTcapProvider extends java.util.EventListener {
    /** Returns a unique Dialogue Id to initiate a dialogue with another
      * TCAP user.
      * @return
      * The new Dialogue Id returned by the underlying TCAP layer.
      * @exception IdNotAvailableException
      * Thrown when a new Dialogue Id is not available.  */
    public int getNewDialogueId() throws IdNotAvailableException;
    /** Release the dialogue Id back to the system.  Note: In some SS7
      * stacks the TCAP Dialogue Id is automatically released following
      * the end of a TCAP transaction. In such instances the
      * implementation of this method may be left null.
      * @param dialogueId
      * The dialogue Id supplied to the method.
      * @exception IdNotAvailableException
      * Thrown when a new Dialogue Id is not available.  */
    public void releaseDialogueId(int dialogueId) throws IdNotAvailableException;
    /** Returns a unique Invoke Id for identifying invoke requests
      * within the dialogue identified by the supplied Dialogue Id.
      * Each dialogue between two JainTcapListeners is identified by a
      * unique Dialogue Id. Note that the returned Invoke Id will be
      * unique for a particular Dialogue Id.
      * @param dialogueId
      * The dialogue Id supplied to the method.
      * @return
      * An unused unique Invoke Id.
      * @exception IdNotAvailableException
      * Thrown when an invoke Id is not available to the specified
      * dialogue Id.  */
    public int getNewInvokeId(int dialogueId) throws IdNotAvailableException;
    /** Releases the unique Invoke Id, allowing it to be reused within
      * the dialogue identified by the supplied Dialogue Id.
      * @param invokeId
      * The invoke Id to be released.
      * @param dialogueId
      * The dialogue Id from which to release the dialogue Id.
      * @exception IdNotAvailableException
      * Thrown when an invoke Id is not available to the specified
      * dialogue Id.  */
    public void releaseInvokeId(int invokeId, int dialogueId) throws IdNotAvailableException;
    /** Sends a Component Request primitive into the TCAP layer of the
      * SS7 protocol stack.
      * @param event
      * The new component event supplied to the method.
      * @exception MandatoryParameterNotSetException
      * Thrown if all of the mandatory parameters required by this
      * JainTcapProviderImpl, to send the Component Request are not set.
      * <p>
      * Note that different implementations of this JainTcapProvider
      * interface will mandate that different parameters must be set for
      * each ComponentReqEvent  It is recommended that the message
      * detail returned in the MandatoryParameterNotSetException should
      * be a java.lang.String of the form: <br>
      * <center> "Parameter: not set" </center><br> */
    public void sendComponentReqEvent(ComponentReqEvent event) throws MandatoryParameterNotSetException;
    /** Sends a Dialogue Request primitive into the TCAP layer of the
      * SS7 protocol stack. This will trigger the transmission to the
      * destination node of the Dialogue request primitive along with
      * any associated Component request primitives that have previously
      * been passed to this JainTcapProviderImpl. Since the same
      * JainTcapProviderImpl will be used to handle a particular
      * transaction, Dialogue Request Events with the same Originating
      * Transaction Id must be sent to the same JainTcapProviderImpl.
      * @param event
      * The new dialogue event supplied to the method.
      * @exception MandatoryParameterNotSetException
      * Thrown when all of the mandatory parameters required by this
      * JainTcapProviderImpl to send the Dialogue Request are not set.
      * <p>
      * <em>Note to developers:-</em> Different implementations of this
      * JainTcapProvider interface will mandate that different
      * parameters must be set for each DialogueReqEvent . It is
      * recommended that the detail message returned in the
      * MandatoryParameterNotSetException should be a java.lang.String of the
      * form: <br>
      * <center> "Parameter: not set" </center><br> */
    public void sendDialogueReqEvent(DialogueReqEvent event) throws MandatoryParameterNotSetException;
    /** @deprecated As of JAIN TCAP version 1.1. This method is replaced
      * by the addJainTcapListener(JainTcapListener, SccpUserAddress)
      * method.
      *
      * Adds a JainTcapListener to the list of registered Event
      * Listeners of this JainTcapProviderImpl.
      * @param listener
      * The feature to be added to the JainTcapListener attribute.
      * @param userAddress
      * The feature to be added to the JainTcapListener attribute.
      * @exception java.util.TooManyListenersException
      * Thrown if a limit is placed on the allowable number of
      * registered JainTcapListeners, and this limit is exceeded.
      * @exception ListenerAlreadyRegisteredException
      * Thrown if the listener listener supplied is already registered.
      * @exception InvalidUserAddressException
      * Thrown if the user address supplied is not a valid address.  */
    public void addJainTcapListener(JainTcapListener listener, TcapUserAddress userAddress) throws java.util.TooManyListenersException, ListenerAlreadyRegisteredException, InvalidUserAddressException;
    /** Adds a JainTcapListener to the list of registered Event
      * Listeners of this JainTcapProviderImpl.
      * @param listener
      * The feature to be added to the JainTcapListener attribute.
      * @param userAddress
      * The feature to be added to the JainTcapListener attribute.
      * @exception java.util.TooManyListenersException
      * Thrown if a limit is placed on the allowable number of
      * registered JainTcapListeners, and this limit is exceeded.
      * @exception ListenerAlreadyRegisteredException
      * Thrown if the listener listener supplied is already registered.
      * @exception InvalidAddressException
      * Thrown if the user address supplied is not a valid address.  */
    public void addJainTcapListener(JainTcapListener listener, SccpUserAddress userAddress) throws java.util.TooManyListenersException, ListenerAlreadyRegisteredException, InvalidAddressException;
    /** Removes a JainTcapListener from the list of registered
      * JainTcapListeners of this JainTcapProviderImpl.
      * @param listener
      * The listener to be removed from this provider.
      * @exception ListenerNotRegisteredException
      * Thrown if there is no such listener registered with this provider.  */
    public void removeJainTcapListener(JainTcapListener listener) throws ListenerNotRegisteredException;
    /** @deprecated As of JAIN TCAP v1.1. This class is no longer needed
      * as a result of the addition of the getStack() class. The reason
      * for deprecating this method is that the provider is attached
      * implicitly.
      *
      * Returns the JainTcapStackImpl that this JainTcapProviderImpl is
      * attached to.
      * @return
      * The attached JainTcapStack.  */
    public JainTcapStack getAttachedStack();
    /** Returns the JainTcapStackImpl that this JainTcapProviderImpl is
      * attached to.
      * @return
      * The attached JainTcapStack.
      * @since version 1.1 */
    public JainTcapStack getStack();
    /** @deprecated As of JAIN TCAP v1.1. No replacement, the
      * JainTcapProvider is attached implicitly within the
      * createProvider method call in the JainTcapStack Interface.  */
    public boolean isAttached();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
