
package org.openss7.javax.sip;

import javax.sip.message.*;
import javax.sip.header.*;
import javax.sip.*;

public class SipProviderImpl implements SipProvider {
    public native void addSipListener(SipListener sipListener)
        throws java.util.TooManyListenersException;
    public native void removeSipListener(SipListener sipListener);
    public native SipStack getSipStack();
    public native ListeningPoint getListeningPoint();
    public native ListeningPoint[] getListeningPoints();
    public native void setListeningPoint(ListeningPoint listeningPoint)
        throws ObjectInUseException;
    public native void addListeningPoint(ListeningPoint listeningPoint)
        throws ObjectInUseException, TransportAlreadySupportedException;
    public native ListeningPoint getListeningPoint(java.lang.String transport);
    public native void removeListeningPoint(ListeningPoint listeningPoint)
        throws ObjectInUseException;
    public native CallIdHeader getNewCallId();
    public native ClientTransaction getNewClientTransaction(Request request)
        throws TransactionUnavailableException;
    public native ServerTransaction getNewServerTransaction(Request request)
        throws TransactionAlreadyExistsException, TransactionUnavailableException;
    public native void sendRequest(Request request)
        throws SipException;
    public native void sendResponse(Response response)
        throws SipException;
    public native Dialog getNewDialog(Transaction transaction)
        throws SipException;
    public native void setAutomaticDialogSupportEnabled(boolean flag);
}


// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
