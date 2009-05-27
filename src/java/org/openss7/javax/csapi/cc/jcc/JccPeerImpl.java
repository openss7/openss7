
package org.openss7.javax.csapi.cc.jcc;

import javax.csapi.cc.jcc.*;

public class JccPeerImpl implements JccPeer {
    public native String getName();
    public native String[] getServices();
    public native JccProvider getProvider(String providerString)
        throws ProviderUnavailableException;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-

