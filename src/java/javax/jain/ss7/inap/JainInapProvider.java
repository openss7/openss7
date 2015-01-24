/*
 @(#) src/java/javax/jain/ss7/inap/JainInapProvider.java <p>
 
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

package javax.jain.ss7.inap;

import javax.jain.ss7.inap.exception.*;
import javax.jain.ss7.inap.event.*;
import javax.jain.ss7.*;
import javax.jain.*;

/**
  * This interface must be implemented by any Object representing the
  * JAIN INAP layer of any SS7 stack that interacts directly with a
  * proprietary (stack vendor specific) implementation of the INAP
  * layer.
  * This interface defines the methods that will be used by any
  * registered INAP User application implementing the JainInapListener
  * interface to send Operations, Return Results or Return Errors . It
  * must be noted that any object that implements the: <ul>
  *
  * <li>JainInapListener Interface is referred to as
  * JainInapListenerImpl.
  *
  * <li>JainInapProvider Interface is referred to as
  * JainInapProviderImpl.
  *
  * <li>JainInapStack Interface is referred to as JainInapStackImpl.
  * </ul>
  *
  * A INAP API User may send number of events to a Provider object. This
  * interface defines methods for: <ul>
  *
  * <li>Adding a JainInapListener to its list of registered Event
  * Listeners. The JainInapListenerImpl will register with the
  * JainInapProviderImpl with an Application Context and become an Event
  * Listener of that JainInapProviderImpl object. Once registered as an
  * Event Listener, a JainInapListenerImpl will receive any Indications
  * addressed to its User Address as Events.
  *
  * <li>Removing a JainInapListener from the JainInapProviderImpl's list
  * of Event Listeners. Once a JainInapListenerImpl is de-registered, it
  * will no longer receive any Events from that JainInapProviderImpl.
  *
  * <li>Returning a new Call Id. </ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public interface JainInapProvider extends java.util.EventListener {
    /** Adds a JainInapListener to the list of registered Event
      * Listeners of this JainInapProviderImpl.  */
    public void addJainInapListener(JainInapListener listener,
            byte[] applicationContext)
        throws ListenerAlreadyRegisteredException,
                          TooManyListenersRegisteredException;
    /** Removes a JainInapListener from the list of registered
      * JainInapListeners of this JainInapProviderImpl.  */
    public void removeJainInapListener(JainInapListener listener,
            byte[] applicationContext)
        throws ListenerNotRegisteredException;
    /** Returns the JainInapStackImpl that this JainInapProviderImpl is
      * attached to.  */
    public JainInapStack getStack();
    /** Returns the Call ID to the JAIN INAP API User.  */
    public int getCallID()
        throws IDNotAvailableException;
    /** Sends the DialogueReqEvent into the INAP layer of the SS7
      * protocol stack.
      * The Provider shall return an array of Invoke IDs.  */
    public int[] sendInapReqEvent(DialogueReqEvent event)
        throws InvalidAddressException, InvalidCallIDException,
                          InvalidApplicationContextException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
