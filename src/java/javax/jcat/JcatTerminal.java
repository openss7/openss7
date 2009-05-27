
package javax.jcat;

import javax.csapi.cc.jcc.*; 


public interface JcatTerminal {
    public void addCallListener(JccCallListener listener)
        throws MethodNotSupportedException, ResourceUnavailableException;
    public void addTerminalListener(JcatTerminalListener listener);
    public java.util.Set getAddresses();
    public java.lang.String getName();
    public JcatProvider getProvider();
    public java.util.Set getTerminalConnections();
    public void removeCallListener(JccCallListener calllistener);
    public void removeTerminalListener(JcatTerminalListener termlistener);
    public JcatTerminalCapabilities getTerminalCapabilities();
    public void registerAddress(JcatAddress addr)
        throws InvalidPartyException, MethodNotSupportedException, PrivilegeViolationException;
    public void deregisterAddress(JcatAddress addr)
        throws InvalidPartyException, MethodNotSupportedException, PrivilegeViolationException;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
