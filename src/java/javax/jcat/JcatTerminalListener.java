

package javax.jcat;

import javax.csapi.cc.jcc.*;

public interface JcatTerminalListener {
    public void addressRegistered(JcatTerminalEvent termconnectionevent);
    public void addressDeregistered(JcatTerminalEvent termconnectionevent);
    public void eventTransmissionEnded(JcatTerminalEvent termconnectionevent);
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
