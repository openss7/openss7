
package javax.jcat;

public interface JcatAddressListener {
    public boolean terminalRegistered(JcatAddressEvent addressevent);
    public void terminalDeregistered(JcatAddressEvent addressevent);
    public void addressEventTransmissionEnded(JcatAddressEvent addressevent);
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
