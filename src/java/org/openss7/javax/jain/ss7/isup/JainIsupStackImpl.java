
package org.openss7.javax.jain.ss7.isup;

import javax.jain.*;
import javax.jain.ss7.*;
import javax.jain.ss7.isup.*;

public class JainIsupStackImpl implements JainIsupStack {
    public native JainIsupProvider createProvider() throws ParameterNotSetException;
    public native void deleteProvider(JainIsupProvider providerToBeDeleted) throws DeleteProviderException;
    public native JainIsupProvider[] getProviderList();
    public native void setSignalingPointCode(SignalingPointCode spc)
        throws ParameterRangeInvalidException, ParameterAlreadySetException;
    public native SignalingPointCode getSignalingPointCode() throws ParameterNotSetException;
    public native void setStackName(java.lang.String stackName);
    public native java.lang.String getStackName();
    public native void setVendorName(java.lang.String vendorName) throws ParameterAlreadySetException;
    public native java.lang.String getVendorName();
    public native int getStackSpecification();
    public native void setStackSpecification(int stackSpec)
        throws VersionNotSupportedException, ParameterAlreadySetException;
    public native void setNetworkIndicator(NetworkIndicator nwInd)
        throws ParameterRangeInvalidException, ParameterAlreadySetException;
    public native NetworkIndicator getNetworkIndicator();
}


// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
