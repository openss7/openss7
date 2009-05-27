
package org.openss7.javax.jain.ss7.inap;

import java.util.*;
import java.lang.*;
import javax.jain.ss7.inap.exception.*;
import javax.jain.ss7.inap.event.*;
import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

public class JainInapProviderImpl implements JainInapProvider {
    public native void addJainInapListener(JainInapListener listener, byte[] applicationContext)
        throws ListenerAlreadyRegisteredException, TooManyListenersRegisteredException;
    public native void removeJainInapListener(JainInapListener listener, byte[] applicationContext)
        throws ListenerNotRegisteredException;
    public native JainInapStack getStack();
    public native int getCallID()
        throws IDNotAvailableException;
    public native int[] sendInapReqEvent(DialogueReqEvent event)
        throws InvalidAddressException, InvalidCallIDException,InvalidApplicationContextException;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
