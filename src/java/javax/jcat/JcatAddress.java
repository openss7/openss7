
package javax.jcat;

import javax.csapi.cc.jcc.*;

public interface JcatAddress extends JccAddress {
    public void addAddressListener(JcatAddressListener addrlistener, EventFilter eventFilter);
    public void registerTerminal(JcatTerminal term)
        throws InvalidPartyException, MethodNotSupportedException, PrivilegeViolationException;
    public void deregisterTerminal(JcatTerminal term)
        throws InvalidPartyException, MethodNotSupportedException, PrivilegeViolationException;
    public java.util.Set getTerminals();
    public void removeAddressListener(JcatAddressListener addrlistener);
    public java.util.Set getConnections();
    public void setDisplayText(java.lang.String text, boolean displayAllowed)
        throws InvalidArgumentException;
    public java.lang.String getDisplayText();
    public boolean getDisplayAllowed();
}


// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-

