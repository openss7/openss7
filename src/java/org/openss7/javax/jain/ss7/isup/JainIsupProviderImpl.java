/*
 @(#) $RCSfile: JainIsupProviderImpl.java,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2009-07-21 11:06:16 $ <p>
 
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
 
 Last Modified $Date: 2009-07-21 11:06:16 $ by $Author: brian $
 */

package org.openss7.javax.jain.ss7.isup;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** This interface represents the JAIN ISUP layer that interfaces directly with a
  * proprietary (vendor-specific) implementation of the ISUP stack.
  * Any object that interfaces between a JAIN ISUP application and an ISUP stack must
  * implement this interface.  This interface defines the methods that will be used by any
  * registered ISUP User application implementing the JainIsupListener interface to send
  * and receive ISUP Events.  An IsupEvent is a java event version of ISUP protocol
  * messages and other interface related information (service primitives) exchanged
  * between the Provider and the Listener. <p>
  *
  * Henceforth, the object implementing the JainIsupListener and JainIsupProvider
  * interface will be referred to as JainIsupListener object and JainIsupProvider object.
  * JainIsupListener object registers with a JainIsupProvider object for receiving ISUP
  * Events.  At the time of registration , the Listener object supplies an ISUP User
  * Address to the Provider object.  An ISUP User Address consists of a destination point
  * code, along with a network indicator for indicating whether the point code belongs to
  * the national or international network, and a set of CICs.  The JainIsupListener object
  * can add more ISUP User Addresses by invoking the addIsupListener method.  A
  * JainIsupListener calls the sendIsupEvent method on the JainIsupProvider to send an
  * ISUP message to the network. <p>
  *
  * A JainIsupProvider object encapsulates the received protocol messages from the ISUP
  * stack into Java events and passes the information to the Listener.
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class JainIsupProviderImpl implements Runnable, JainIsupProvider {
    /** Protected default constructor. */
    protected JainIsupProviderImpl() {
    }
    /** A constructor for this class providing the attached stack.
      * @param stack
      * The JainIsupStackImpl instance creating this JainIsupProviderImpl
      * object.
      */
    public JainIsupProviderImpl(JainIsupStackImpl stack) {
        m_stack = stack;
    }
    /** Provides a finzalization method for this class. */
    public native void finalize();
    /** Adds a JainIsupListener object to the list of registered Event
      * Listeners being serviced by this JainIsupProvider object.
      * This is also used to add new ISUP User Addresses being handled by an
      * ISUP Listener so that a Listener can register for more than one ISUP
      * User Address by repeatedly calling this method, provided that the
      * implementation maximum number of listeners would not be exceeded.
      * @param isupListener
      * The JainIsupListener object to be added.
      * @param isupAddress
      * The ISUP User Address handled by the JainIsupListener object.
      * @exception java.util.TooManyListenersException
      * Thrown if a limit is placed on the allowable number of registered
      * JainIsupListener obects, and this limit would be exceeded.
      * @exception ListenerAlreadyRegisteredException
      * Thrown if the Listener is already in the list of registered ones
      * handled by the Provider.
      * @exception InvalidListenerException
      * Thrown if the Listener is null.
      * @exception InvalidAddressException
      * Thrown if the IsupAddress has an invalid format or is null.
      * @exception SendStackException
      * Thrown if the registration could not be sent to the underlying stack.  */
    public native void addIsupListener(JainIsupListener isupListener, IsupUserAddress isupAddress)
        throws java.util.TooManyListenersException, ListenerAlreadyRegisteredException,
                          InvalidListenerException, InvalidAddressException, SendStackException;
    /** This method is used to remove the ISUP User Address being handled by a
      * JainIsupListener.
      * This removes a JainIsupListener from the list of registered Listeners
      * being services by this Provider, if it is the last User Address being
      * handled by the ISUP Listener.
      * @param isupListener
      * The JainIsupListener object to be removed from this JainIsupProvider
      * object.
      * @param isupAddress
      * The ISUP User Adress handled by the JainIsupListener object.
      * @exception ListenerNotRegisteredException
      * Thrown when the JainIsupListener object to be removed is not registered
      * as an Event Listener of this JainIsupProvider object.
      * @exception InvalidListenerException
      * Thrown if the isupListener is null.
      * @exception InvalidAddressException
      * Thrown if the isupAddress has an invalid format.  */
    public native void removeIsupListener(JainIsupListener isupListener, IsupUserAddress isupAddress)
        throws ListenerNotRegisteredException, InvalidListenerException, InvalidAddressException;
    /** This method is for sending an IsupEvent from a JainIsupListener object to the ISUP
      * layer of the SS7 stack.
      * This method is invoked by the JAIN ISUP Listener on the Provider for delivering an
      * event to the Stack.
      * @param isupEvent
      * The IsupEvent to be sent to the stack.
      * @exception MandatoryParameterNotSetException
      * Thrown if all of the mandatory parameters required by this JainIsupProvider object
      * to send the ISUP message out to the network are not set.  Note that different
      * implementations of this JainIsupProvider interface will mandate that a different
      * set of parameters be set for each of the IsupEvents.  It is recommended that the
      * detail message returned in the MandatoryParameterNotSetException should be a
      * java.lang.String of the form, <p> <center><code>"Parameter: not set"</code></center><p>
      * @exception SendStackException
      * Thrown if the event could not be sent to the underlying stack.
      * @exception ParameterRangeInvalidException
      * Thrown if the event being sent has a Circuit Identification Code or a Destination
      * Point Code that has not been registered for by the Listener application.
      * @exception VersionNotSupportedException
      * Thrown if the event is a variant specific event that is different from the
      * underlying stack variant.  */
    public native void sendIsupEvent(IsupEvent isupEvent)
        throws MandatoryParameterNotSetException, SendStackException,
                          ParameterRangeInvalidException, VersionNotSupportedException;
    /** Returns the JainIsupStackImpl to which this JainIsupProvider object is attached.
      * @return The attached JainIsupStack.  */
    public JainIsupStack getAttachedStack() {
        return m_stack;
    }
    /** Provides a runable method for this class.
      * The runable method is responsible for polling the underlying provider
      * and marshalling received events to registered listeners. <p>
      *
      * This method calls the {@link JainIsupProviderImpl#recvIsupEvent} method
      * to obtain an {@link IsupEvent} to be delivered to a registered {@link
      * JainIsupListener}.  The source of the event is reslly the destination
      * JainIsupListener object registered for this event.  The method recasts
      * the object and passes it to the listener, resetting the source to
      * reflect this JainIsupProviderImpl class as the source.
      */
    public void run() {
        while (running) {
            try {
                IsupEvent event;
                event = recvIsupEvent();
                if (event != null) {
                    java.lang.Object source;

                    /* The source of the event is really the destination
                     * JainIsupListener object registered for this event.
                     * So we cast the object and pass it to the listener,
                     * resetting the source to us. */
                    source = event.getSource();
                    if (source instanceof JainIsupListener) {
                        JainIsupListener listener;
                        listener = (JainIsupListener) source;
                        event.setSource(this);
                        /* TODO: we might want to spawn a thread from a
                         * thread pool to process the event. */
                        listener.processIsupEvent(event);
                    }
                }
            } catch (Exception e) {
                /* If there is an exception, discard the event. */
            }
        }
    }
    /** Provides a shutdown method for this class.
      * The shutdown method is responsible for stopping the runnable method and
      * ceasing polling off the underlying provider for this class.
      */
    public void shutdown() {
        running = false;
    }

    /** Provides a receive method for this class.
      * The receive method is called by the {@link JainIsupProviderImpl#run}
      * method to obtain the next event to be marshalled to registered
      * listeners.  The received IsupEvent returned by this method has a source
      * member that specifies the JainIsupListener rather than the
      * JainIsupProvider class for the purposes of distribution of the message.
      * @return
      * An IsupEvent to be delivered to a JainIsupListener.  */
    protected native IsupEvent recvIsupEvent();

    private JainIsupStack m_stack;
    private boolean running = true;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
