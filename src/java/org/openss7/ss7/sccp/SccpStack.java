
package org.openss7.ss7.sccp;

public interface SccpStack {
    public SignalingPointCode getSignalingPointCode();
    public SccpProvider createProvider()
	throws PeerUnavailableException;
    public void deleteProvider(SccpProvider providerToBeDeleted)
	throws DeletedProviderException;
    public SccpProvider[] getProviderList();
    public String getStackName();
    public void setStackName(String stackname);
    public String getVendorName();
    public void setVendorName(String vendorname);
    public int getStackSpecification();
    public void setStackSpecificaiton(int stackSpecification)
	throws VersionNotSupportedException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
