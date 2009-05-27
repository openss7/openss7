
package org.openss7.javax.jain.ss7.isup;

import javax.jain.*;
import javax.jain.ss7.*;
import javax.jain.ss7.isup.*;

public class JainIsupProviderImpl implements JainIsupProvider {
    public native void addIsupListener(JainIsupListener isupListener, IsupUserAddress isupAddress)
        throws java.util.TooManyListenersException, ListenerAlreadyRegisteredException,
                          InvalidListenerException, InvalidAddressException, SendStackException;
    public native void removeIsupListener(JainIsupListener isupListener, IsupUserAddress isupAddress)
        throws ListenerNotRegisteredException, InvalidListenerException, InvalidAddressException;
    public native void sendIsupEvent(IsupEvent isupEvent)
        throws MandatoryParameterNotSetException, SendStackException,
                          ParameterRangeInvalidException, VersionNotSupportedException;
    public native JainIsupStack getAttachedStack();
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
