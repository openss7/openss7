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

import jain.protocol.ss7.tcap.component.*;
import jain.protocol.ss7.tcap.dialogue.*;
import jain.protocol.ss7.tcap.*;
import jain.protocol.ss7.*;
import jain.*;

public class JainTcapProviderImpl implements java.rmi.Remote, Runnable, JainTcapProvider {
    protected JainTcapProviderImpl() {
    }
    public JainTcapProviderImpl(JainTcapStackImpl stack) {
        m_stack = stack;
    }
    public native void finalize();
    public native int getNewDialogueId()
        throws IdNotAvailableException;
    public native void releaseDialogueId(int dialogueId)
        throws IdNotAvailableException;
    public native int getNewInvokeId(int dialogueId)
        throws IdNotAvailableException;
    public native void releaseInvokeId(int invokeId, int dialogueId)
        throws IdNotAvailableException;
    public native void sendComponentReqEvent(ComponentReqEvent event)
        throws MandatoryParameterNotSetException;
    public native void sendDialogueReqEvent(DialogueReqEvent event)
        throws MandatoryParameterNotSetException;
    public native void addJainTcapListener(JainTcapListener listener, TcapUserAddress userAddress)
        throws TooManyListenersException, ListenerAlreadyRegisteredException, InvalidUserAddressException; 
    public native void addJainTcapListener(JainTcapListener listener, SccpUserAddress userAddress)
        throws TooManyListenersException, ListenerAlreadyRegisteredException, InvalidAddressException;
    public native void removeJainTcapListener(JainTcapListener listener)
        throws ListenerNotRegisteredException;
    public JainTcapStack getAttachedStack() {
        return getStack();
    }
    public JainTcapStack getStack() {
        return m_stack;
    }
    public native boolean isAttached();

    public void run() {
        while (running) {
            try {
                EventObject rawevent;
                rawevent = recvTcapEvent();
                if (rawevent != null) {
                    Object source;

                    /* The source of the event is really the destination
                     * JainTcapListener object registered for this event.
                     * So we cast the object and pass it to the listener,
                     * resetting the source to us.  When the event is
                     * destined to more than one listener, it is the
                     * responsibility of the native method to duplicate
                     * it. */
                    source = rawevent.getSource();
                    if (source instanceof JainTcapListener) {
                        JainTcapListener listener = (JainTcapListener) source;
                        /* TODO: we might want to spawn a thread from a
                         * thread pool to process the event. */
                        if (rawevent instanceof DialogueIndEvent) {
                            DialogueIndEvent event = (DialogueIndEvent) rawevent;
                            event.setSource((Object) this);
                            listener.processDialogueIndEvent(event);
                        } else if (rawevent instanceof ComponentIndEvent) {
                            ComponentIndEvent event = (ComponentIndEvent) rawevent;
                            event.setSource((Object) this);
                            listener.processComponentIndEvent(event);
                        } else if (rawevent instanceof TcapErrorEvent) {
                            TcapErrorEvent event = new TcapErrorEvent(this, ((TcapErrorEvent)rawevent).getError());
                            listener.processTcapError(event);
                        }
                    }
                }
            } catch (Exception e) {
                /* If there is an exception, discard the event. */
            }
        }
    }
    public void shutdown() {
        running = false;
    }

    protected native EventObject recvTcapEvent();

    private JainTcapStackImpl m_stack;
    private boolean running = true;
}

// vim: ft=java tw=72 sw=4 et com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
