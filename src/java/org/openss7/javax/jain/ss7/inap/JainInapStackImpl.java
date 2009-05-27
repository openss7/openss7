
package org.openss7.javax.jain.ss7.inap;

import java.io.*;
import java.util.*;
import java.lang.*;

import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

public class JainInapStackImpl implements JainInapStack {
    public native JainInapProvider createProvider()
        throws PeerUnavailableException;
    public native void deleteProvider(JainInapProvider providerToBeDeleted)
        throws DeleteProviderException;
    public native java.util.Vector getProviderList();
    public native String getStackName();
    public native void setStackName(String stackName);
    public native String getVendorName();
    public native void setVendorName(String vendorName);
    public native SignalingPointCode getSignalingPointCode();
    public native int getSubSystemNumber();
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-

