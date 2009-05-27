
package javax.jcat;

import javax.csapi.cc.jcc.*;

public interface JcatTerminalEvent extends JccEvent {
    public static final int ADDRESS_REGISTERED = 0;
    public static final int ADDRESS_DEREGISTERED = 1;
    public static final int TERMINAL_EVENT_TRANSMISSION_ENDED = 0;
    public JcatAddress getAddress();
    public JcatTerminal getTerminal();
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
