/*
 @(#) src/java/org/openss7/ss7/mtp/JainMtpProviderImpl.java <p>
 
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

package org.openss7.ss7.mtp;

import org.openss7.ss7.*;
import javax.jain.ss7.*;
import javax.jain.*;

/**
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class JainMtpProviderImpl implements java.lang.Runnable, java.rmi.Remote, java.io.Serializable, JainMtpProvider {
    private JainMtpStackImpl stack;
    private boolean running = true;
    /** Protected default constructor.  */
    protected JainMtpProviderImpl() { }
    /** A constructor for this class providing the attached stack.
      * @param stack
      * The JainMtpStackImpl instance creating this JainMtpProviderImpl
      * object.
      */
    public JainMtpProviderImpl(JainMtpStackImpl stack) {
        this.stack = stack;
    }
    /** Provides a finalization method for this class.  */
    public native void finalize();
    /** Adds a JainMtpListener object to the list of registered
      * Event Listeners being serviced by this JainMtpProvider
      * object.
      * This is also used to add new MTP User Addresses being
      * handled by an MTP Listener so that a Listener can
      * register for more than one MTP User Address by
      * repeatedly calling this method, provided that the
      * implementation maximum number of listeners would not be
      * exceeded.
      * @param listener
      * The JainMtpListener object to be added.
      * @param userAddress
      * The MTP User Address handled by the JainMtpListener
      * object.
      * @exception java.util.TooManyListenersException
      * Thrown if a limit is placed on the allowable number of
      * registered JainMtpListeners, and this limit has been
      * reached.
      * @exception javax.jain.ListenerAlreadyRegisteredException
      * Thrown if the listener supplied is already registered
      * for the specified userAddress with this
      * JainMtpProviderimpl.
      * @exception javax.jain.ss7.isup.InvalidListenerException
      * Thrown if the Listener is null.
      * @exception javax.jain.InvalidAddressException
      * Thrown if the MtpAddress has an invalid format or is
      * null.
      * @exception javax.jain.ss7.isup.SendStackException
      * Thrown if the registration could not be sent to the
      * underlying stack.
      */
    public native void addMtpListener(JainMtpListener listener, MtpUserAddress userAddress)
        throws java.util.TooManyListenersException,
            javax.jain.ListenerAlreadyRegisteredException,
            javax.jain.ss7.isup.InvalidListenerException,
            javax.jain.InvalidAddressException,
            javax.jain.ss7.isup.SendStackException;
    /** This method is used to remove the MTP User Address
      * being handled by a JainMtpListener.
      * This removes a JainMtpListener from the list of
      * registered Listeners being services by this Provider,
      * if it is the last User Address being handled by the
      * MTP Listener.
      * @param listener
      * The JainMtpListener object to be removed from this
      * JainMtpProvider object.
      * @param userAddress
      * The MTP User Adress handled by the JainMtpListener
      * object.
      * @exception javax.jain.ListenerNotRegisteredException
      * Thrown when the JainMtpListener object to be removed is
      * not registered as an Event Listener of this
      * JainMtpProvider object.
      * @exception javax.jain.ss7.isup.InvalidListenerException
      * Thrown if the listener is null.
      * @exception javax.jain.InvalidAddressException
      * Thrown if the userAddress has an invalid format.  */
    public native void removeMtpListener(JainMtpListener listener, MtpUserAddress userAddress)
        throws javax.jain.ListenerNotRegisteredException,
            javax.jain.ss7.isup.InvalidListenerException,
            javax.jain.InvalidAddressException;
    /** This method is for sending an MtpEvent from a
      * JainMtpListener object to the MTP layer of the SS7
      * stack.
      * This method is invoked by the JAIN MTP Listener on the
      * Provider for delivering an event to the Stack.
      * @param event
      * The MtpEvent to be sent to the stack.
      * @exception javax.jain.MandatoryParameterNotSetException
      * Thrown if all of the mandatory parameters required by
      * this JainMtpProvider object to send the MTP message
      * out to the network are not set.  Note that different
      * implementations of this JainMtpProvider interface will
      * mandate that a different set of parameters be set for
      * each of the MtpEvents.  It is recommended that the
      * detail message returned in the
      * MandatoryParameterNotSetException should be a java.lang.String of
      * the form, <p> <center><code>"Parameter: not
      * set"</code></center><p>
      * @exception javax.jain.ss7.isup.SendStackException
      * Thrown if the event could not be sent to the underlying
      * stack.
      * @exception javax.jain.ss7.isup.ParameterRangeInvalidException
      * Thrown if the event being sent has a Circuit
      * Identification Code or a Destination Point Code that
      * has not been registered for by the Listener
      * application.
      * @exception javax.jain.VersionNotSupportedException
      * Thrown if the event is a variant specific event that is
      * different from the underlying stack variant.  */
    public native void sendMtpEvent(MtpEvent event)
        throws javax.jain.MandatoryParameterNotSetException,
            javax.jain.ss7.isup.SendStackException,
            javax.jain.ss7.isup.ParameterRangeInvalidException,
            javax.jain.VersionNotSupportedException;
    /** Gets the list of MTP listeners.
      * @return
      * Array of MTP listeners. */
    public native JainMtpListener[] getMtpListeners();
    /** Returns the JainMtpStackImpl to whic this
      * JainMtpProvider object is attached.
      * @return
      * The attached JainMtpStack.  */
    public JainMtpStack getMtpStack() {
        return (JainMtpStack) stack;
    }
    /** Provides a runable method for this class.
      * The runable method is responsible for polling the
      * underlying provider and marshalling received events to
      * registered listeners. <p>
      *
      * This method calls the
      * {@link JainMtpProviderImpl#recvMtpEvent} method to
      * obtain an {@link MtpEvent} to be delivered to a
      * registered {@link JainMtpListener}.  The source of the
      * event is reslly the destination JainMtpListener object
      * registered for this event.  The method recasts the
      * object and passes it to the listener, resetting the
      * source to reflect this JainMtpProviderImpl class as
      * the source.
      */
    public void run() {
        while (running) {
            try {
                MtpEvent event = recvMtpEvent();
                if (event != null) {
                    java.lang.Object source = event.getSource();
                    if (source instanceof JainMtpListener) {
                        JainMtpListener listener = (JainMtpListener) source;
                        event.setSource((java.lang.Object) this);
                        listener.processMtpEvent(event);
                    }
                }
            } catch (Exception e) {
                /* When there is an exception, discard the event. */
            }
        }
    }
    /** Provides a shutdown method for this class.
      * The shutdown method is responsible for stopping the
      * runnable method and ceasing polling off the underlying
      * provider for this class.
      */
    public void shutdown() {
        running = false;
    }
    /** Provides a receive method for this class.
      * The receive method is called by the
      * {@link JainMtpProviderImpl#run} method to obtain the
      * next event to be marshalled to registered listeners.
      * The received MtpEvent returned by this method has a
      * source member that specifies the JainMtpListener
      * rather than the JainMtpProvider class for the purposes
      * of distribution of the message.
      * @return
      * An MtpEvent to be delivered to a JainMtpListener.  */
    private native MtpEvent recvMtpEvent();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-

