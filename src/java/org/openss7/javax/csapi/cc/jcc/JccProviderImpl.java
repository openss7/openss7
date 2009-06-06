
package org.openss7.javax.csapi.cc.jcc;

import javax.csapi.cc.jcc.*;

public class JccProviderImpl implements java.rmi.Remote, JccProvider {
    public native EventFilter createEventFilterEventSet(int blockEvents[], int notifyEvents[])
        throws ResourceUnavailableException, InvalidArgumentException;
    public native EventFilter createEventFilterMidCallEvent(int midCallType, String midCallValue, int matchDisposition, int nomatchDisposition)
        throws ResourceUnavailableException, InvalidArgumentException;
    public native EventFilter createEventFilterMinimunCollectedAddressLength(int minimumAddressLength, int matchDisposition, int nomatchDisposition)
        throws ResourceUnavailableException, InvalidArgumentException;
    public native EventFilter createEventFilterCauseCode(int causeCode, int matchDisposition, int nomatchDisposition)
        throws ResourceUnavailableException, InvalidArgumentException;
    public native EventFilter createEventFilterAddressRange(String lowAddress, String highAddress, int matchDisposition, int nomatchDisposition)
        throws ResourceUnavailableException, InvalidArgumentException;
    public native EventFilter createEventFilterAddressRegEx(String addressRegex, int matchDisposition, int nomatchDisposition)
        throws ResourceUnavailableException,InvalidArgumentException ;
    public native EventFilter createEventFilterOr(EventFilter filters[], int nomatchDisposition)
        throws ResourceUnavailableException, InvalidArgumentException;
    public native EventFilter createEventFilterAnd(EventFilter filters[],int nomatchDisposition)
        throws ResourceUnavailableException, InvalidArgumentException;
    public native EventFilter createEventFilterDestAddressRange(String lowDestAddress, String highDestAddress, int matchDisposition, int nomatchDisposition)
        throws ResourceUnavailableException, InvalidArgumentException;
    public native EventFilter createEventFilterDestAddressRegEx(String destAddressRegex, int matchDisposition, int nomatchDisposition)
        throws ResourceUnavailableException, InvalidArgumentException;
    public native EventFilter createEventFilterOrigAddressRange(String lowOrigAddress, String highOrigAddress, int matchDisposition, int nomatchDisposition)
        throws ResourceUnavailableException, InvalidArgumentException;
    public native EventFilter createEventFilterOrigAddressRegEx(String origAddressRegex, int matchDisposition, int nomatchDisposition)
        throws ResourceUnavailableException, InvalidArgumentException;
    public native void addCallListener( JccCallListener calllistener )
        throws MethodNotSupportedException, ResourceUnavailableException;
    public native void removeCallListener( JccCallListener calllistener );
    public native void addConnectionListener( JccConnectionListener connectionlistener, EventFilter filter)
        throws ResourceUnavailableException, MethodNotSupportedException;
    public native void removeConnectionListener( JccConnectionListener connectionlistener);
    public native void setCallLoadControl(JccAddress[] address, double duration, double[] mechanism,int[] treatment)
        throws MethodNotSupportedException;
    public native void addCallLoadControlListener(CallLoadControlListener loadcontrollistener)
        throws MethodNotSupportedException, ResourceUnavailableException;
    public native void removeCallLoadControlListener(CallLoadControlListener loadcontrollistener);
    public native int getState();
    public native JccCall createCall()
        throws InvalidStateException, ResourceUnavailableException, PrivilegeViolationException, MethodNotSupportedException;
    public native void addProviderListener( JccProviderListener providerlistener )
        throws ResourceUnavailableException, MethodNotSupportedException;
    public native void removeProviderListener( JccProviderListener providerlistener );
    public native String getName();
    public native JccAddress getAddress(String address)
        throws InvalidPartyException;
    public native void shutdown();
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
