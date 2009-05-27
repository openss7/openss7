
package org.openss7.ss7.sccp;

public interface SuaStack {
    public SignalingPointCode getSignalingPointCode();
    public SuaProvider createProvider()
	throws PeerUnavailableException;
    public void deleteProvider(SuaProvider providerToBeDeleted)
	throws DeletedProviderException;
    public SuaProvider[] getProviderList();
    public String getStackName();
    public void setStackName(String stackname);
    public String getVendorName();
    public void setVendorName(String vendorname);
    public int getStackSpecification();
    public void setStackSpecificaiton(int stackSpecification)
	throws VersionNotSupportedException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
