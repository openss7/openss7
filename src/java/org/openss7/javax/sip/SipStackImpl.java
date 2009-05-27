
package org.openss7.javax.sip;

import javax.sip.address.*;
import javax.sip.message.*;
import javax.sip.header.*;
import javax.sip.*;

public class SipStackImpl implements SipStack {
    public native SipProvider createSipProvider(ListeningPoint listeningPoint)
        throws ObjectInUseException;
    public native void deleteSipProvider(SipProvider sipProvider)
        throws ObjectInUseException;
    public native java.util.Iterator getSipProviders();
    public native ListeningPoint createListeningPoint(int port, java.lang.String transport)
        throws TransportNotSupportedException, InvalidArgumentException;
    public native ListeningPoint createListeningPoint(java.lang.String ipAddress, int port, java.lang.String transport)
        throws TransportNotSupportedException, InvalidArgumentException;
    public native void deleteListeningPoint(ListeningPoint listeningPoint)
        throws ObjectInUseException;
    public native java.util.Iterator getListeningPoints();
    public native java.lang.String getStackName();
    public native java.lang.String getIPAddress();
    public native Router getRouter();
    public native void stop();
    public native void start() throws SipException;
    public native boolean isRetransmissionFilterActive();
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
