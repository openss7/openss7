
package org.openss7.javax.jain.protocol.ss7.map.position;

import javax.jain.protocol.ss7.map.position.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public class MsPosProviderImpl implements java.rmi.Remote, MsPosProvider {
    public native long processMsPosOperation(MsPosReqEvent event, MsPosListener listener)
        throws SS7MandatoryParameterNotSetException;
    public native void processMsPosOperation(MsPosRespEvent event, long transactionId)
        throws SS7MandatoryParameterNotSetException;
    public native void processMsPosOperation(MsPosReportRespEvent event, long transactionId)
        throws SS7MandatoryParameterNotSetException;
    public native void addMsPosListener(MsPosListener listener, SS7Address userAddress)
        throws java.util.TooManyListenersException;
    public native void removeMsPosListener(MsPosListener listener)
        throws SS7ListenerNotRegisteredException;
}


// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
