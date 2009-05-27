
package javax.jcat;

import javax.csapi.cc.jcc.*;

public interface JcatAddressEvent extends JccEvent {
    public static final int TERMINAL_REGISTERED = 1;
    public static final int TERMINAL_DEREGISTERED = 0;
    public static final int ADDRESS_EVENT_TRANSMISSION_ENDED = 0;
    public JcatTerminal getTerminal();
    public JcatAddress getAddress();
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
