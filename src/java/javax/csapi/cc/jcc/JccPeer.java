package javax.csapi.cc.jcc;

/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Copyrights:
 *
 *  Copyright - 2001, 2002 Sun Microsystems, Inc. All rights reserved.
 *  901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 *  This product and related documentation are protected by copyright and
 *  distributed under licenses restricting its use, copying, distribution, and
 *  decompilation. No part of this product or related documentation may be
 *  reproduced in any form by any means without prior written authorization of
 *  Sun and its licensors, if any.
 *
 *  RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 *  States Government is subject to the restrictions set forth in DFARS
 *  252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 *  The product described in this manual may be protected by one or more U.S.
 *  patents, foreign patents, or pending applications.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Author:
 *
 *  Telcordia Technologies, Inc.
 *  Farooq Anjum, John-Luc Bakker, Ravi Jain
 *  445 South Street
 *  Morristown, NJ 07960
 *
 *  Version       : 1.1
 *  Notes         :
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/**
	The JccPeer interface represents a vendor's particular implementation of the JCC API.
	<p>
	Other interfaces
	derived from JCC are expected to implement this interface. 
	The JccPeer object, returned
	by the {@link JccPeerFactory#getJccPeer(String)} method, determines which JccProviders are made 
	available to 
	the application. 
	<p>
	Applications use the {@link #getProvider(String)} method on this interface to obtain new JccProvider
	objects. Each implementation may support one or more different "services". A list of available
	services can be obtained via the {@link #getServices()} method. 

	<H4>Obtaining a JccProvider</H4> 
	Applications use the {@link #getProvider(String)} method on this 
	interface to obtain new JccProvider objects. Each implementation may support 
	one or more different "services" (e.g. for different types of underlying 
	network substrate). A list of available services can be obtained via the 
	{@link #getServices()} method. 
	
	<p> Applications may also 
	supply optional arguments to the JccProvider through the 
	{@link #getProvider(String)} method. These arguments are appended to
	the <CODE>providerString</CODE> argument passed to the 
	{@link #getProvider(String)} method. The <CODE>providerString</CODE>
	argument has the following format: 
	<p> &lt service name &gt ; arg1 = val1; arg2 = val2; ... <p> 
	Where &lt service name &gt is not optional, and each optional argument pair
	which follows is separated by a semi-colon. The keys for these arguments is 
	implementation specific, except for two standard-defined keys: <OL> 
	<LI>login: provides the login user name to the Provider. 
	<LI>passwd: provides a password to the Provider. </OL>
	@since 1.0b
 */
public interface JccPeer {
    /**
        Returns the name of this JccPeer object instance. The name is the same name used as an
        argument to {@link JccPeerFactory#getJccPeer(String)} method.
       <br>
       <br>
       @return The name of this JccPeer object instance.
     */
    public String getName();
    
    /**
    Returns the services that this implementation supports. This method returns null if no 
    services are supported.
    
    @return services that this implementation supports.
    
    */    
    public String[] getServices();
    /**
    Returns an instance of a <CODE>Provider</CODE> object given a string 
    argument which contains the desired service name. Optional arguments may also
    be provided in this string, with the following format: <p> 
    &lt service name &gt ; arg1 = val1; arg2 = val2; ... <p> 
    Where &lt service name &gt is not optional, and each optional argument pair 
    which follows is separated by a semi-colon. The keys for these arguments is 
    implementation specific, except for two standard-defined keys: 
    <OL> 
    <LI>login: provides the login user name to the Provider. 
    <LI>passwd: provides a password to the Provider. </OL> 

    <p> If the argument is null, this method returns some default Provider as 
    determined by the JccPeer object. The returned Provider is not in the 
    {@link JccProvider#SHUTDOWN} state.Note that this may also result in 
    the application obtaining a reference to a Provider which has already been 
    created. <p> 
    
    <B>Post-conditions:</B> 
    <OL>
     <LI>this.getProvider().getState() != SHUTDOWN
    </OL>
    @param providerString is the name of the desired service.
    @return An instance of the JccProvider object.
    @throws ProviderUnavailableException indicates a Provider corresponding to the given string
    is unavailable. 
    */
    
    public JccProvider getProvider(String providerString) throws ProviderUnavailableException;
    

}

