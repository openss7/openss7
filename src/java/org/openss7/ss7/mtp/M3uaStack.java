
package org.openss7.ss7.mtp;

public interface M3uaStack {
    public SignalingPointCode getSignalingPointCode();
    public M3uaProvider createProvider()
	throws PeerUnavailableException;
    public void deleteProvider(M3uaProvider providerToBeDeleted)
	throws DeletedProviderException;
    public M3uaProvider[] getProviderList();
    public String getStackName();
    public void setStackName(String stackname);
    public String getVendorName();
    public void setVendorName(String vendorname);
    public int getStackSpecification();
    public void setStackSpecificaiton(int stackSpecification)
	throws VersionNotSupportedException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
