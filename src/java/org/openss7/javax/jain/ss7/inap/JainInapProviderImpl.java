
package org.openss7.javax.jain.ss7.inap;

import java.util.*;
import java.lang.*;
import javax.jain.ss7.inap.exception.*;
import javax.jain.ss7.inap.event.*;
import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

public class JainInapProviderImpl implements java.rmi.Remote, JainInapProvider {
    protected JainInapProviderImpl() { }
    public JainInapProviderImpl(JainInapStackImpl stack) { m_stack = stack; }
    public native void finalize();
    public native void addJainInapListener(JainInapListener listener, byte[] applicationContext)
        throws ListenerAlreadyRegisteredException, TooManyListenersRegisteredException;
    public native void removeJainInapListener(JainInapListener listener, byte[] applicationContext)
        throws ListenerNotRegisteredException;
    public native JainInapStack getStack();
    public native int getCallID()
        throws IDNotAvailableException;
    public native int[] sendInapReqEvent(DialogueReqEvent event)
        throws InvalidAddressException, InvalidCallIDException,InvalidApplicationContextException;
    private JainInapStackImpl m_stack;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
