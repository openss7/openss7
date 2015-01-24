/******************************************************************************
*                                                                             *
*                                                                             *
* Copyright (c) SS8 Networks, Inc.                                            *
* All rights reserved.                                                        *
*                                                                             *
* This document contains confidential and proprietary information in which    *
* any reproduction, disclosure, or use in whole or in part is expressly       *
* prohibited, except as may be specifically authorized by prior written       *
* agreement or permission of SS8 Networks, Inc.                               *
*                                                                             *
*******************************************************************************
* VERSION      : 1.1
* DATE         : 2008/05/16 12:23:18
* 
* MODULE NAME  : JainIsupStackImpl.java,v
* AUTHOR       : Nilgun Baykal [SS8]
* DESCRIPTION  : 
* DATE 1st REL : 
* REV.HIST.    : 
* 
* Date      Owner  Description
* ========  =====  ===========================================================
* 
* 
*******************************************************************************
*                                                                             *
*                     RESTRICTED RIGHTS LEGEND                                *
* Use, duplication, or disclosure by Government Is Subject to restrictions as *
* set forth in subparagraph (c)(1)(ii) of the Rights in Technical Data and    *
* Computer Software clause at DFARS 252.227-7013                              *
*                                                                             *
******************************************************************************/
package com.ss8.javax.jain.ss7.isup;

import java.util.*;

import javax.jain.*;
import javax.jain.ss7.*;
import javax.jain.ss7.isup.*;

////////////////////////////////////////////////////////////////////////////////////
//  CLASS JainIsupStackImpl
////////////////////////////////////////////////////////////////////////////////////
public class JainIsupStackImpl implements JainIsupStack {

	private static native int initIsupStack ();
	private native int[] getPCInfo (int spNo);
	private native int getNI (int spNo);

	////////////////////////////////////////////////////////////////////////////////
	// static()
	////////////////////////////////////////////////////////////////////////////////

	static {
	
		System.loadLibrary("JainIsup");		
		initIsupStack();

	}


	////////////////////////////////////////////////////////////////////////////////
    // createProvider() 
    ////////////////////////////////////////////////////////////////////////////////

	public JainIsupProvider createProvider()
                                throws ParameterNotSetException,
								PeerUnavailableException{


		/* PeerUnavailableException is added for the sake of
the code */
		
		if((isvendorName == false) || 
								(isstackSpecification == false)){
				throw new ParameterNotSetException("vendorName or stackSpecification is not set");
		}
		
		else{

				JainIsupProviderImpl D7Provider;

				try {
						D7Provider = new JainIsupProviderImpl(this); 
	    
						providers.add(D7Provider);

				} catch(Exception err) {
	    
						throw new PeerUnavailableException("Couldn't find peer"); 
				}
	
				D7Provider.ownStack = this;
	
				return D7Provider;     
		}

	}

	////////////////////////////////////////////////////////////////////////////////
    // deleteProvider() 
    ////////////////////////////////////////////////////////////////////////////////

	public void deleteProvider(JainIsupProvider providerToBeDeleted)
                    throws DeleteProviderException{

		
		JainIsupProviderImpl d7Provider = (JainIsupProviderImpl) providerToBeDeleted;

		providers.remove(d7Provider);
	
		// do not leave cleanup to finalization 
		d7Provider.cleanup(); 

	}
	
	////////////////////////////////////////////////////////////////////////////////
    //  getProviderList()
    ////////////////////////////////////////////////////////////////////////////////

	public JainIsupProvider[] getProviderList(){
		
		

		JainIsupProvider[] providerArray = new
								JainIsupProvider[providers.size()];
		
		Iterator e = providers.iterator();
		for(int i=0; e.hasNext(); i++) {
				providerArray[i] = (JainIsupProvider)e.next();
		}

		return providerArray;
	}

	////////////////////////////////////////////////////////////////////////////////
    //  get/setNetworkIndicator()
    ////////////////////////////////////////////////////////////////////////////////

	public NetworkIndicator getNetworkIndicator(){
		
		NetworkIndicator nw;
		nw = new NetworkIndicator(getNI(spNo));
		return nw;
	}

	
	public void setNetworkIndicator(NetworkIndicator nwInd)
                         throws ParameterRangeInvalidException,
                                ParameterAlreadySetException{
		/* This function is not supported */
		throw new ParameterAlreadySetException("Network Indicator is already set");

	}

	////////////////////////////////////////////////////////////////////////////////
    //  get/setSignalingPointCode()
    ////////////////////////////////////////////////////////////////////////////////

	public SignalingPointCode getSignalingPointCode()
                                 throws ParameterNotSetException{

		SignalingPointCode	spc;		
	
		int[] arr = getPCInfo(spNo);

		spc = new SignalingPointCode(arr[0],arr[1],arr[2]);

		return spc; 

	}

	public void setSignalingPointCode(SignalingPointCode spc)
                           throws ParameterRangeInvalidException,
                                  ParameterAlreadySetException{

		int member, cluster,zone,i;
		int[] spci;
		boolean found=false;

		try{
		member = spc.getMember();
		cluster = spc.getCluster();
		zone = spc.getZone();
	

		for(i=0;i<8;i++){
			if(	(spci = getPCInfo(i)) != null){ 
				if ((spci[2] == member)&&(spci[1] == cluster)&&(spci[0]==zone)){
						found = true;
						break;
				}
		  }
		}		
	 	
		if(found == true){
			this.spNo = i;		
		}
		else
			throw new ParameterRangeInvalidException();
		}catch (Exception e) {};
	}

	////////////////////////////////////////////////////////////////////////////////
    //  get/setStackName()
    ////////////////////////////////////////////////////////////////////////////////
		
	public String getStackName(){
		return stackName;
	}
		

	public void setStackName(String stackName){
		this.stackName = stackName;
	}

	////////////////////////////////////////////////////////////////////////////////
    //  get/setStackSpecification()
    ////////////////////////////////////////////////////////////////////////////////
	public int getStackSpecification(){
		return stackSpecification;
	}

	public void setStackSpecification(int stackSpec)
                           throws VersionNotSupportedException,
                                  ParameterAlreadySetException{
		switch (stackSpec){
			case IsupConstants.ISUP_PV_ANSI_1992:
			case IsupConstants.ISUP_PV_ANSI_1995: 
			case IsupConstants.ISUP_PV_ITU_1993: 
				stackSpecification = stackSpec;
				isstackSpecification = true;
				break;
			default:
				throw new VersionNotSupportedException ("stack specification " + 
						    stackSpec + " not supported \n");
		}		
	}

	////////////////////////////////////////////////////////////////////////////////
    //  get/setVendorName()
    ////////////////////////////////////////////////////////////////////////////////
	public String getVendorName(){
		return vendorName;
	}

	public void setVendorName(String vendorName)
                   throws ParameterAlreadySetException{
		// customer can change the default vendor name
		this.vendorName =  vendorName;
		isvendorName = true;

	}

	
	////////////////////////////////////////////////////////////////////////////////
    //  get/setSPNo()
    ////////////////////////////////////////////////////////////////////////////////
	public int getSPNo() {
		return spNo;
    }

		
	public void setSPNo(int spNo) throws IsupException {

		if (providers.size() != 0 && spNo != this.spNo) {
			// already has a provider can not change spNo
			throw new IsupException("no change when stack has providers");
		} 
		else 
			this.spNo = spNo;
	}


	boolean isstackSpecification = false;
	boolean isvendorName = false;	

	String vendorName = "com.ss8";
    String stackName  = "d7-isup";

	int stackSpecification = IsupConstants.ISUP_PV_ITU_1993;

	int spNo = 0;

	private Vector providers = new Vector();
}






