
package org.openss7.javax.jain.protocol.ss7.map.session;

import javax.jain.protocol.ss7.map.session.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public class MessageSessionProviderImpl implements MessageSessionProvider {
    public native long processMessageSessionOperation(SessionOpenReqEvent event, MessageSessionListener listener)
        throws SS7MandatoryParameterNotSetException;
    public native void processMessageSessionOperation(SessionDataReqEvent event, long sessionId)
        throws SS7MandatoryParameterNotSetException;
    public native void processMessageSessionOperation(SessionCloseReqEvent event, long sessionId)
        throws SS7MandatoryParameterNotSetException;
    public native void addMessageSessionListener(MessageSessionListener listener, SS7Address userAddress)
        throws java.util.TooManyListenersException;
    public native void removeMessageSessionListener(MessageSessionListener listener)
        throws SS7ListenerNotRegisteredException;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
