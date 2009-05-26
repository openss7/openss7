//  ==========================================================================
//  
//  @(#) $Id$
//  
//  --------------------------------------------------------------------------
//  
//  Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
//  Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
//  Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
//  
//  All Rights Reserved.
//  
//  This program is free software; you can redistribute it and/or modify it
//  under the terms of the GNU Affero General Public License as published by
//  the Free Software Foundation; version 3 of the License.
//  
//  This program is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
//  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public
//  License for more details.
//  
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>, or
//  write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
//  02139, USA.
//  
//  --------------------------------------------------------------------------
//  
//  U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
//  behalf of the U.S. Government ("Government"), the following provisions
//  apply to you.  If the Software is supplied by the Department of Defense
//  ("DoD"), it is classified as "Commercial Computer Software" under
//  paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
//  Regulations ("DFARS") (or any successor regulations) and the Government is
//  acquiring only the license rights granted herein (the license rights
//  customarily provided to non-Government users).  If the Software is
//  supplied to any unit or agency of the Government other than DoD, it is
//  classified as "Restricted Computer Software" and the Government's rights
//  in the Software are defined in paragraph 52.227-19 of the Federal
//  Acquisition Regulations ("FAR") (or any successor regulations) or, in the
//  cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
//  (or any successor regulations).
//  
//  --------------------------------------------------------------------------
//  
//  Commercial licensing and support of this software is available from
//  OpenSS7 Corporation at a fee.  See http://www.openss7.com/
//  
//  --------------------------------------------------------------------------
//  
//  Last Modified $Date$ by $Author$
//  
//  --------------------------------------------------------------------------
//  
//  $Log$
//  ==========================================================================

package org.openss7.jain.protocol.ss7.tcap;

import java.util.*;

import jain.*;
import jain.protocol.ss7.*;
import jain.protocol.ss7.tcap.*;
// import jain.protocol.ss7.tcap.component.*;
// import jain.protocol.ss7.tcap.dialogue.*;

public class JainTcapProviderImpl implements JainTcapProvider {
    static {
        try {
            System.loadLibrary("openss7TcapJNI");
        } catch (UnsatisfiedLinkError e) {
            System.err.println("Native code library failed to load. \n" + e);
            System.exit(1);
        }
    }
    private JainTcapStackImpl stack;
    private int stackSpecification;
    private native void getProvider(int stackSpecification)
        throws PeerUnavailableException;
    private native void putProvider();

    /**
      * Create a JainTcapProviderImpl instance.
      *
      * Note that the only thing that we specify now is the stackSpecification.
      * The native method can support a wider range of values than is provided in
      * TcapConstants.
      */
    JainTcapProviderImpl(JainTcapStackImpl stack)
        throws PeerUnavailableException {

        try {
            this.stack = stack;
            this.stackSpecification = stack.getStackSpecification();
            getProvider(stackSpecification);
        } catch (Exception e) {
            throw new PeerUnavailableException("Cannot instantiate provider.");
        }
    }

    /**
      * Destructor for the JAIN TCAP provider implementation.  When a
      * provider is finalized, we need to release the underlying file
      * descriptor.  It is ok if this is not called at system exit
      * because all file descriptors will be closed at that point
      * anyway.
      */
    protected void finalize() {
        putProvider();
    }

    /**
      * Returns a unique Dialogue Id to initiate a dialog with another
      * TCAP user.
      *
      * @return the newe Dialogue Id returned by the underlying TCAP layer
      * @exception IdNotAvailableException if a new Dialogue Id is not available.
      */
    public native int getNewDialogueId()
        throws IdNotAvailableException;
    /**
      * Release the dialogue Id back to the system. <br>
      * <b>Note:</b> In some SS7 stacks the TCAP Dialogue Id is automatically
      * released following the end of a TCAP transaction. In such instances the
      * implementation of this method may be left null.
      *
      * <p><b>Implementation Notes:</b>
      * Dialogue Ids are automatically released.
      *
      * @param  dialogueId  the dialogue Id supplied to the method
      * @exception  IdNotAvailableException  if a new Dialogue Id is not available
      */
    public void releaseDialogueId(int dialogueId)
        throws IdNotAvailableException {
    }
    /**
      * Returns a unique Invoke Id for identifying invoke requests within the
      * dialogue identified by the supplied Dialogue Id. Each dialogue between two
      * <CODE>JainTcapListeners</CODE> is identified by a unique Dialogue Id. Note
      * that the returned Invoke Id will be unique for a particular Dialogue Id.
      *
      * @param  dialogueId the dialogue Id supplied to the method
      * @return an unused unique Invoke Id
      * @exception  IdNotAvailableException  if an invoke Id is not available to
      * the specified dialogue Id
      */
    public native int getNewInvokeId(int dialogueId)
        throws IdNotAvailableException;
    /**
      * Releases the unique Invoke Id, allowing it to be reused within the
      * dialogue identified by the supplied Dialogue Id.
      *
      * <p><b>Implementation Notes:</b>
      * Invoke Ids are automatically released.
      *
      * @param  invokeId the invoke Id to be released
      * @param  dialogueId the dialogue Id from which to release the dialogue Id
      * @exception  IdNotAvailableException if an invoke Id is not available to
      * the specified dialogue Id
      */
    public void releaseInvokeId(int invokeId, int dialogueId)
        throws IdNotAvailableException {
    }
    /**
      * Sends a Component Request primitive into the TCAP layer of the SS7
      * protocol stack.
      *
      * @param  event the new component event supplied to the method
      * @exception  MandatoryParameterNotSetException  thrown if all of the
      * mandatory parameters required by this JainTcapProviderImpl, to send the
      * Component Request are not set. <BR>
      * Note that different implementations of this JainTcapProvider interface
      * will mandate that different parameters must be set for each
      * <CODE>ComponentReqEvent</CODE> . It is recommended that the message detail
      * returned in the <CODE>MandatoryParameterNotSetException</CODE> should be a
      * <CODE>String</CODE> of the form: <BR>
      * <CENTER><B>"Parameter: <parameterName> not set"</B></CENTER>
      */
    public native void sendComponentReqEvent(ComponentReqEvent event)
        throws MandatoryParameterNotSetException;
    /**
      * Sends a Dialogue Request primitive into the TCAP layer of the SS7 protocol
      * stack. This will trigger the transmission to the destination node of the
      * Dialogue request primitive along with any associated Component request
      * primitives that have previously been passed to this JainTcapProviderImpl.
      * Since the same JainTcapProviderImpl will be used to handle a particular
      * transaction, Dialogue Request Events with the same Originating Transaction
      * Id must be sent to the same JainTcapProviderImpl.
      *
      * @param  event the new dialogue event supplied to the method
      * @exception  MandatoryParameterNotSetException  thrown if all of the
      * mandatory parameters required by this JainTcapProviderImpl to send the
      * Dialogue Request are not set. <p>
      *
      * <b>Note to developers</b> :- different implementations of this
      * JainTcapProvider interface will mandate that different parameters must be
      * set for each <CODE>DialogueReqEvent</CODE> . It is recommended that the
      * detail message returned in the
      * <CODE>MandatoryParameterNotSetException</CODE> should be a
      * <CODE>String</CODE>of the form: <P>
      *
      * <CENTER><B>"Parameter: <parameterName> not set"</B></CENTER>
      */
    public native void sendDialogueReqEvent(DialogueReqEvent event)
        throws MandatoryParameterNotSetException;
    /**
      * Adds a <a href="JainTcapListener.html">JainTcapListener</a> to the list of
      * registered Event Listeners of this JainTcapProviderImpl.
      *
      * <p><b>Implementation Notes:</b> This could be implemented by converting the
      * TcapUserAddress to an SccpUserAddress and calling the replacement version
      * for this deprecated method.
      *
      * @param  listener the feature to be added to the JainTcapListener attribute
      * @param  userAddress the feature to be added to the JainTcapListener
      * attribute
      * @exception  TooManyListenersException thrown if a limit is placed on the
      * allowable number of registered JainTcapListeners, and this limit is
      * exceeded.
      * @exception  ListenerAlreadyRegisteredException thrown if the listener
      * listener supplied is already registered
      * @exception  InvalidUserAddressException thrown if the user address
      * supplied is not a valid address
      *
      * @deprecated As of JAIN TCAP version 1.1. This method is replaced by the
      * {@link #addJainTcapListener(JainTcapListener, SccpUserAddress)} method.
      */
    public native void addJainTcapListener(JainTcapListener listener, TcapUserAddress userAddress)
        throws TooManyListenersException, ListenerAlreadyRegisteredException, InvalidUserAddressException; 
    /**
      * Adds a <a href="JainTcapListener.html">JainTcapListener</a> to the list of
      * registered Event Listeners of this JainTcapProviderImpl.
      *
      * <p><b>Implementation Notes:</b>
      * Nowhere does the specification say the maximum number of listeners that
      * must be accepted.  Is suppose the value could be zero (0).  It might be
      * easier for us to restrict this value to one (1).  Otherwise, we need to
      * open a new underlying TCAP Stream for each listener.  This is no problem I
      * suppose.  One non-listening TCAP Stream will be opened for each TCAP
      * provider instantiated for TCAP initators.  TCAP responders will use the
      * dialogue ID of the indication.  Because we have only on initiating Stream,
      * it is easy to obtain a dialogue Id unique to that Stream.  However, for
      * initiators, it is not necessary for the underlying stack to acquire a
      * Dialogue Id as one can be provided automatically by the provider.  But
      * this interface cannot handle that. <p>
      *
      * To allow this the native method will need to map a listenerHandle returned
      * from the native method to the specific listener.
      *
      * @param  listener the feature to be added to the JainTcapListener attribute
      * @param  userAddress the feature to be added to the JainTcapListener
      * attribute
      * @exception  TooManyListenersException thrown if a limit is placed on the
      * allowable number of registered JainTcapListeners, and this limit is
      * exceeded.
      * @exception  ListenerAlreadyRegisteredException thrown if the listener
      * listener supplied is already registered
      * @exception  InvalidAddressException thrown if the user address supplied is
      * not a valid address
      */
    public native void addJainTcapListener(JainTcapListener listener, SccpUserAddress userAddress)
        throws TooManyListenersException, ListenerAlreadyRegisteredException, InvalidAddressException;
    /**
      * Removes a <a href="JainTcapListener.html">JainTcapListener</a> from the
      * list of registered JainTcapListeners of this JainTcapProviderImpl.
      *
      * @param  listener the listener to be removed from this provider
      * @exception  ListenerNotRegisteredException thrown if there is no such
      * listener registered with this provider
      */
    public native void removeJainTcapListener(JainTcapListener listener)
        throws ListenerNotRegisteredException;
    /**
      * Returns the JainTcapStackImpl to which this JainTcapProviderImpl is
      * attached.
      *
      * @return the attached JainTcapStack.
      * @deprecated As of JAIN TCAP v1.1. This class is no longer needed as a
      * result of the addition of the {@link
      * jain.protocol.ss7.tcap.JainTcapProvider#getStack} class.
      * The reason for deprecating this method is that the provider is attached
      * implicitly.
      */
    public JainTcapStack getAttachedStack() {
        return getStack();
    }
    /**
      * Returns the JainTcapStackImpl to which this JainTcapProviderImpl is
      * attached.
      *
      * @return the attached JainTcapStack.
      * @since version 1.1
      */
    public JainTcapStack getStack() {
        return this.stack;
    }
    /**
      * @deprecated    As of JAIN TCAP v1.1. No replacement, the JainTcapProvider
      * is attached implicitly within the <a href =
      * "JainTcapStack.html#createProvider()">createProvider</a> method call in
      * the <code>JainTcapStack</code> Interface.
      */
    public boolean isAttached() {
        return true;
    }
}

// vim: ft=java tw=72 sw=4 et com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
