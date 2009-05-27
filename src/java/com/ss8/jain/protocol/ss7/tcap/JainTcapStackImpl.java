package com.ss8.jain.protocol.ss7.tcap;

import java.util.*;

import jain.*;
import jain.protocol.ss7.tcap.*;
import jain.protocol.ss7.*;
import jain.PeerUnavailableException;
import jain.protocol.ss7.VersionNotSupportedException;


////////////////////////////////////////////////////////////////////////////////////
//  CLASS JainTcapStackImpl
////////////////////////////////////////////////////////////////////////////////////
public class JainTcapStackImpl implements JainTcapStack {

    String vendorName = "com.ss8";
    String stackName  = "d7-tcap";
    int stackSpecification = 1;    //default ANSI92
    private int spNo = 0;

    ///////////////////////////////////////////
    // Native Interfaces
    ///////////////////////////////////////////
    private static native int initTcapStack ();
    private native int[] getPCInfo (int spNo);
    private Vector providers = new Vector();


    ////////////////////////////////////////////////////////////////////////////////
    // static()
    ////////////////////////////////////////////////////////////////////////////////
    static {
	System.loadLibrary("JainTcap");
	initTcapStack();
    }


    ////////////////////////////////////////////////////////////////////////////////
    // createProvider() / deleteProvider()
    ////////////////////////////////////////////////////////////////////////////////
    public JainTcapProvider createProvider() throws PeerUnavailableException {
	JainTcapProviderImpl D7Provider; 

	try {
		D7Provider = new JainTcapProviderImpl(this); 
		providers.add(D7Provider);

	} catch(Exception err) {
		throw new PeerUnavailableException("Couldn't find peer"); 
	}

	D7Provider.setStack(this);
	return D7Provider;     
    }

    public void deleteProvider(JainTcapProvider providerToBeDeleted) throws DeleteProviderException {
	JainTcapProviderImpl d7Provider = (JainTcapProviderImpl) providerToBeDeleted;

	providers.remove(d7Provider);
	// do not leave cleanup to finalization
	d7Provider.cleanup();
    }


    ////////////////////////////////////////////////////////////////////////////////
    // getProviderList()
    ////////////////////////////////////////////////////////////////////////////////
    public JainTcapProvider[] getProviderList() { 
	JainTcapProvider[] providerArray = new JainTcapProvider[providers.size()];
	Enumeration        providerEnum  = providers.elements();

	for (int i=0; providerEnum.hasMoreElements(); i++) {
		providerArray[i] = (JainTcapProvider) providerEnum.nextElement();
	}

	return providerArray;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // set / get StackName()
    ////////////////////////////////////////////////////////////////////////////////
    public void setStackName(String stackName) {
	this.stackName = stackName;
    }

    public String getStackName() {
	return stackName;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // set / get VendorName()
    ////////////////////////////////////////////////////////////////////////////////
    public void setVendorName(String vendorName) {
	if (providers.size() == 0) {
		// already has a provider can not change vendorName, no exception thrown
		this.vendorName =  vendorName;
        }
    }

    public String getVendorName() {
	return vendorName;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // set / get StackSpecification()
    ////////////////////////////////////////////////////////////////////////////////
    public void setStackSpecification(int stackSpecification) throws VersionNotSupportedException {
	if (providers.size() != 0) {
		throw new VersionNotSupportedException("Cannot change when stack has providers.\n");
	} else {
		switch (stackSpecification) {
		case TcapConstants.STACK_SPECIFICATION_ANSI_92:
		case TcapConstants.STACK_SPECIFICATION_ANSI_96:
		case TcapConstants.STACK_SPECIFICATION_ITU_93:
		case TcapConstants.STACK_SPECIFICATION_ITU_97:
			this.stackSpecification = stackSpecification;
			break;
		default:
			throw new VersionNotSupportedException ("stack specification " + 
						stackSpecification + " not supported \n");
		}
	}
    }

    public int getStackSpecification() {
	return stackSpecification;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // set / get SPNo()
    ////////////////////////////////////////////////////////////////////////////////
    public void setSPNo(int spNo) throws TcapException {
	if (providers.size() != 0 && spNo != this.spNo) {
		// already has a provider can not change spNo
		throw new TcapException("Cannot change when stack has providers.");
	} else {
		this.spNo = spNo;
	}
    }

    public int getSPNo() {
	return spNo;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // set / get SignalingPointCode()
    ////////////////////////////////////////////////////////////////////////////////
    public void setSignalingPointCode(int[] spc) throws TcapException {

	int member, cluster,zone,i;
	int [] spc2;
	boolean found = false;

	try{
		member  = spc[2];
		cluster = spc[1];
		zone    = spc[0];

		for( i = 0; i < 8; i++ ) {
			if ((spc2 = getPCInfo(i)) != null) {
				if ( ( spc2[2] == member ) &&
				     ( spc2[1] == cluster) &&
				     ( spc2[0] == zone   ) ) {
					found = true;
					break;
				}
			}
		}

		if ( found ) {
			this.spNo = i;
		} else {
			throw new TcapException();
		}
	} catch (Exception e) {e.printStackTrace();};
    }

    public int[] getSignalingPointCode() {
	int [] spc;
	spc = getPCInfo(spNo);
	return spc;
    }


    ////////////////////////////////////////////////////////////////////////////////
    // define deprecated methods as no-operation / attach / detach DEPRECATED
    ////////////////////////////////////////////////////////////////////////////////
    public void attach(JainTcapProvider jainTcapProvider) throws ProviderNotAttachedException {}
    public void detach(JainTcapProvider jainTcapProvider) throws ProviderNotAttachedException {}


    ////////////////////////////////////////////////////////////////////////////////
    // createAttachedProvider() DEPRECATED
    ////////////////////////////////////////////////////////////////////////////////
    public JainTcapProvider createAttachedProvider() throws PeerUnavailableException, ProviderNotAttachedException {
	return createProvider();
    }


    ////////////////////////////////////////////////////////////////////////////////
    // createDetachedProvider() DEPRECATED
    ////////////////////////////////////////////////////////////////////////////////
    public JainTcapProvider createDetachedProvider() throws PeerUnavailableException {
	return createProvider();
    }


    ////////////////////////////////////////////////////////////////////////////////
    // setProtocolVersion() DEPRECATED
    ////////////////////////////////////////////////////////////////////////////////
    public void setProtocolVersion(int protocolVersion) throws TcapException {
	try {
		setStackSpecification(protocolVersion);
	} catch (VersionNotSupportedException exc) {
		throw new TcapException ("Protocol version " + protocolVersion
					+ " not supported \n");
	}
    }


    ////////////////////////////////////////////////////////////////////////////////
    // getProtocolVersion() DEPRECATED
    ////////////////////////////////////////////////////////////////////////////////
    public int getProtocolVersion() {
	return getStackSpecification();
    }


}
