package com.ss8.jain.protocol.ss7.tcap;

import java.util.*;

import jain.*;
import jain.protocol.ss7.*;

class AddressConverter {

    static int NONE = JainTcapProviderImpl.NONE;

    static int getRouteIndicator(SccpUserAddress address) {
	return address.getRoutingIndicator();
    }

    static boolean getNationalUse(SccpUserAddress address) {
	return address.getNationalUse();
    }

    static int getZone(SccpUserAddress address) {
	int zone = NONE;
	
	try {
	    if (address.isSubSystemAddressSet()) 
		if (address.getSubSystemAddress().isSignalingPointCodeSet()) {
		    zone = address.getSubSystemAddress().getSignalingPointCode().getZone();
		}
	} catch (Exception exc) {};
	
	return zone;
    }

    static int getMember(SccpUserAddress address) {
	int member = NONE;
	
	try {
	    if (address.isSubSystemAddressSet()) 
		if (address.getSubSystemAddress().isSignalingPointCodeSet()) {
		    member = address.getSubSystemAddress().getSignalingPointCode().getMember();
		}
	} catch (Exception exc) {};
	
	return member;
    }
    
    static int getCluster(SccpUserAddress address) {
	int cluster = NONE;


	try {
	    if (address.isSubSystemAddressSet()) 
		if (address.getSubSystemAddress().isSignalingPointCodeSet()) {
		    cluster = address.getSubSystemAddress().getSignalingPointCode().getCluster();
		}
	} catch (Exception exc) {};
	    
	return cluster;
    }

    static int getSSN(SccpUserAddress address) {
	int subsystemNumber = NONE;
	
	try {
	    if (address.isSubSystemAddressSet()) 
		if (address.getSubSystemAddress().isSubSystemNumberSet()) {
		    subsystemNumber = address.getSubSystemAddress().getSubSystemNumber();
		}
	} catch (Exception exc) {};
	
	return subsystemNumber;
    }
    
    /* Get global title only if routing indicator is route on gt */
    /* these methods should never generate exceptions */
    /* fill in the bodies */
    static int getGTIndicator(SccpUserAddress address) {
	int GTIndicator = NONE;
	
	if (address.isGlobalTitleSet()) {
	    try {
		GTIndicator = address.getGlobalTitle().getGTIndicator();
	    } catch (Exception exc) {};
	}
	
	return GTIndicator;
    }

    static byte[] getAddressInfo(SccpUserAddress address) {
	byte[] addressInformation =  null;

	if (address.isGlobalTitleSet()) {
	    try {
		addressInformation = address.getGlobalTitle().getAddressInformation();
	    } catch (Exception exc) {};
	}
	
	return addressInformation;
    }

    static int getEncodingScheme(SccpUserAddress address) {
	int encodingScheme = NONE;

	if (address.isGlobalTitleSet()) {
	    try {
		GlobalTitle globalTitle = address.getGlobalTitle();
		
		switch (globalTitle.getGTIndicator()) {
		case AddressConstants.GTINDICATOR_0001: 
		    encodingScheme = ((GTIndicator0001) globalTitle).getEncodingScheme();
		    break;
		case AddressConstants.GTINDICATOR_0011: 
		    encodingScheme = ((GTIndicator0011) globalTitle).getEncodingScheme();
		    break;
		case AddressConstants.GTINDICATOR_0100: 
		    encodingScheme = ((GTIndicator0100) globalTitle).getEncodingScheme();
		    break;
		}
	    } catch (Exception exc) {};
	}
	
	return encodingScheme;
	
    }

    static int getNatureOfAddr(SccpUserAddress address) {
	int natureOfAddrInd = NONE;

	if (address.isGlobalTitleSet()) {
	    try {
		GlobalTitle globalTitle = address.getGlobalTitle();
 
		switch (globalTitle.getGTIndicator()) {
		case AddressConstants.GTINDICATOR_0001: 
		    natureOfAddrInd =((GTIndicator0001) globalTitle).getNatureOfAddrInd();
		    break;
		case AddressConstants.GTINDICATOR_0100: 
		    natureOfAddrInd =((GTIndicator0100) globalTitle).getNatureOfAddrInd();
		    break;
		}
	    } catch (Exception exc) {};
	}
	
	return natureOfAddrInd;
	
    }

    static int getNumPlan(SccpUserAddress address) {
	int numberingPlan = NONE;

	if (address.isGlobalTitleSet()) {
	    try {
		GlobalTitle globalTitle = address.getGlobalTitle();
 
		switch (globalTitle.getGTIndicator()) {
		case AddressConstants.GTINDICATOR_0001: 
		    numberingPlan =  ((GTIndicator0001) globalTitle).getNumberingPlan();
		    break;
		case AddressConstants.GTINDICATOR_0011: 
		    numberingPlan =  ((GTIndicator0011) globalTitle).getNumberingPlan();
		    break;
		case AddressConstants.GTINDICATOR_0100: 
		    numberingPlan =  ((GTIndicator0100) globalTitle).getNumberingPlan();
		    break;
		}

	    } catch (Exception exc) {};
	}
	
	return numberingPlan;
    }

    static int getTranslationType(SccpUserAddress address) {
	int translationType = NONE;

	if (address.isGlobalTitleSet()) {

	    try {
		GlobalTitle globalTitle = address.getGlobalTitle();
 
		switch (globalTitle.getGTIndicator()) {
		case AddressConstants.GTINDICATOR_0001: 
		    translationType =((GTIndicator0001) globalTitle).getTranslationType();
		    break;
		case AddressConstants.GTINDICATOR_0010: 
		    translationType =((GTIndicator0010) globalTitle).getTranslationType();
		    break;
		case AddressConstants.GTINDICATOR_0011: 
		    translationType =((GTIndicator0011) globalTitle).getTranslationType();
		    break;
		case AddressConstants.GTINDICATOR_0100: 
		    translationType =((GTIndicator0100) globalTitle).getTranslationType();
		    break;
		}

	    } catch (Exception exc) {};
	}
	
	return translationType;
    }

    static boolean getOddIndicator(SccpUserAddress address) {
	boolean oddIndicator = false;

	if (address.getRoutingIndicator() == AddressConstants.ROUTING_GLOBALTITLE) {
 
	    try {
		GlobalTitle globalTitle = address.getGlobalTitle();

		switch (globalTitle.getGTIndicator()) {
		case AddressConstants.GTINDICATOR_0001: 
		    oddIndicator  =  ((GTIndicator0001) globalTitle).isOddIndicator();
		    break;
		}

	    } catch (Exception exc) {};
	}
	
	return oddIndicator;
    }

    static SccpUserAddress constructAdress(int     routeIndicator,
					   boolean nationalUse,
					   int     zone,
					   int     cluster,
					   int     member,
					   int     subsystemNumber,
					   int     GTIndicator,
					   byte[]  addressInfo,
					   int     encodingScheme,
					   int     natureOfAddr,
					   int     numberingPlan,
					   int     translationType,
					   boolean oddIndicator
					   ) 
    {
	SccpUserAddress     address;
	GlobalTitle         globalTitle      = null;
	SubSystemAddress    subSystemAddress = null;
	SignalingPointCode  pointCode        = null;
	
	if (GTIndicator != NONE && GTIndicator != AddressConstants.GTINDICATOR_0000) {
	    
	    switch (GTIndicator) {
	    case AddressConstants.GTINDICATOR_0001:
		// fill the superset of ANSI/ITU gt information

		globalTitle = new GTIndicator0001((byte) translationType,
						  numberingPlan,
						  encodingScheme,
						  addressInfo);
		
		((GTIndicator0001) globalTitle).setOddIndicator(oddIndicator);
		((GTIndicator0001) globalTitle).setNatureOfAddrInd(natureOfAddr);
		break;
		
	    case AddressConstants.GTINDICATOR_0010:
		globalTitle = new GTIndicator0010((byte) translationType,
						  addressInfo);
		    
		break;
	    case AddressConstants.GTINDICATOR_0011:
		globalTitle = new GTIndicator0011((byte) translationType,
						  numberingPlan,
						  encodingScheme,
						  addressInfo);
		
		break;
	    case AddressConstants.GTINDICATOR_0100:
		globalTitle = new GTIndicator0100((byte) translationType,
						  numberingPlan,
						  encodingScheme,
						  natureOfAddr,
						  addressInfo);
		break;
	    }
	}

	if (member != NONE || zone != NONE || cluster != NONE)
	    pointCode = new SignalingPointCode(member, cluster, zone);

	if (pointCode != null || subsystemNumber != NONE) {
	    subSystemAddress = new SubSystemAddress(pointCode, (short) subsystemNumber);
	}

	address  = new SccpUserAddress();

	if (globalTitle != null)
	    address.setGlobalTitle(globalTitle);

	if (subSystemAddress != null)
	    address.setSubSystemAddress(subSystemAddress);
	
	address.setRoutingIndicator(routeIndicator);

	address.setNationalUse(nationalUse);

	return address;
    }

};
