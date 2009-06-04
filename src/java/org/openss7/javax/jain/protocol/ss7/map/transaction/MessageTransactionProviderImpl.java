
package org.openss7.javax.jain.protocol.ss7.map.transaction;

import javax.jain.protocol.ss7.map.transaction.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public class MessageTransactionProviderImpl implements java.rmi.Remote, MessageTransactionProvider {
    public native long processMessageTransactionOperation(MtMessageReqEvent event, MessageTransactionListener listener)
        throws SS7MandatoryParameterNotSetException;
    public native void processMessageTransactionOperation(MoMessageRespEvent event, long transactionId)
        throws SS7MandatoryParameterNotSetException;
    public native void processMessageTransactionOperation(MtMessageRecAvailRespEvent event, long transactionId)
        throws SS7MandatoryParameterNotSetException;
    public native void addMessageTransactionListener(MessageTransactionListener listener, SS7Address userAddress)
        throws java.util.TooManyListenersException;
    public native void removeMessageTransactionListener(MessageTransactionListener listener)
        throws SS7ListenerNotRegisteredException;
}


// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
