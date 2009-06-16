/*
 @(#) $RCSfile$ $Name$($Revision$) $Date$ <p>
 
 Copyright &copy; 2008-2009  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
 Copyright &copy; 2001-2008  OpenSS7 Corporation <a href="http://www.openss7.com/">&lt;http://www.openss7.com/&gt;</a>. <br>
 Copyright &copy; 1997-2001  Brian F. G. Bidulock <a href="mailto:bidulock@openss7.org">&lt;bidulock@openss7.org&gt;</a>. <p>
 
 All Rights Reserved. <p>
 
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license. <p>
 
 This program is distributed in the hope that it will be useful, but <b>WITHOUT
 ANY WARRANTY</b>; without even the implied warranty of <b>MERCHANTABILITY</b>
 or <b>FITNESS FOR A PARTICULAR PURPOSE</b>.  See the GNU Affero General Public
 License for more details. <p>
 
 You should have received a copy of the GNU Affero General Public License along
 with this program.  If not, see
 <a href="http://www.gnu.org/licenses/">&lt;http://www.gnu.org/licenses/&gt</a>,
 or write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA. <p>
 
 <em>U.S. GOVERNMENT RESTRICTED RIGHTS</em>.  If you are licensing this
 Software on behalf of the U.S. Government ("Government"), the following
 provisions apply to you.  If the Software is supplied by the Department of
 Defense ("DoD"), it is classified as "Commercial Computer Software" under
 paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
 Regulations ("DFARS") (or any successor regulations) and the Government is
 acquiring only the license rights granted herein (the license rights
 customarily provided to non-Government users).  If the Software is supplied to
 any unit or agency of the Government other than DoD, it is classified as
 "Restricted Computer Software" and the Government's rights in the Software are
 defined in paragraph 52.227-19 of the Federal Acquisition Regulations ("FAR")
 (or any successor regulations) or, in the cases of NASA, in paragraph
 18.52.227-86 of the NASA Supplement to the FAR (or any successor regulations). <p>
 
 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See
 <a href="http://www.openss7.com/">http://www.openss7.com/</a> <p>
 
 Last Modified $Date$ by $Author$
 */

package javax.jain.ss7.inap.datatype;

import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** This class defines DPSpecificCommonParameters Datatype.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class DPSpecificCommonParameters implements java.io.Serializable {
    /** Constructor For DPSpecificCommonParameters.  */
    public DPSpecificCommonParameters(ServiceAddressInformation serviceAddressInformation) {
        setServiceAddressInformation(serviceAddressInformation);
    }
    /** Gets Service Address Information.  */
    public ServiceAddressInformation  getServiceAddressInformation() {
        return serviceAddressInformation;
    }
    /** Sets Service Address Information.  */
    public void setServiceAddressInformation( ServiceAddressInformation serviceAddressInformation) {
        this.serviceAddressInformation=serviceAddressInformation;
    }
    /** Gets Bearer Capability.  */
    public BearerCapability getBearerCapability()
        throws ParameterNotSetException {
        if (isBearerCapability)
            return bearerCapability;
        throw new ParameterNotSetException("Bearer Capability: not set.");
    }
    /** Sets Bearer Capability.  */
    public void setBearerCapability( BearerCapability bearerCapability) {
        this.bearerCapability = bearerCapability;
        this.isBearerCapability = true;
    }
    /** Indicates if the optional parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isBearerCapabilityPresent() {
        return isBearerCapability;
    }
    /** Gets Called Party Nuber.  */
    public CalledPartyNumber getCalledPartyNumber()
        throws ParameterNotSetException {
        if (isCalledPartyNumber)
            return calledPartyNumber;
        throw new ParameterNotSetException("Called Party Number: not set.");
    }
    /** Sets Called Party Number.  */
    public void setCalledPartyNumber (CalledPartyNumber calledPartyNumber) {
        this.calledPartyNumber = calledPartyNumber;
        this.isCalledPartyNumber = true;
    }
    /** Indicates if the Called Party Nuber optional parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isCalledPartyNumberPresent() {
        return isCalledPartyNumber;
    }
    /** Gets Calling Party Number.  */
    public CallingPartyNumber getCallingPartyNumber()
        throws ParameterNotSetException {
        if (isCallingPartyNumber)
            return callingPartyNumber;
        throw new ParameterNotSetException("Calling Party Number: not set.");
    }
    /** Sets Calling Party Number.  */
    public void setCallingPartyNumber (CallingPartyNumber  callingPartyNumber) {
        this.callingPartyNumber = callingPartyNumber;
        this.isCallingPartyNumber = true;
    }
    /** Indicates if the Calling Party Number optional parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isCallingPartyNumberPresent() {
        return isCallingPartyNumber;
    }
    /** Gets Calling Partys Category.  */
    public java.lang.String getCallingPartysCategory()
        throws ParameterNotSetException {
        if (isCallingPartysCategory)
            return callingPartysCategory;
        throw new ParameterNotSetException("Calling Party Category: not set.");
    }
    /** Sets Calling Partys Category.  */
    public void setCallingPartysCategory (java.lang.String callingPartysCategory) {
        this.callingPartysCategory = callingPartysCategory;
        this.isCallingPartysCategory = true;
    }
    /** Indicates if the Calling Partys Category optional parameter is
      * present.
      * @return
      * True when present, false otherwise.  */
    public boolean isCallingPartysCategoryPresent() {
        return isCallingPartysCategory;
    }
    /** Gets IP SSP Capabilities parameter.  */
    public java.lang.String getIPSSPCapabilities()
        throws ParameterNotSetException {
        if (isIPSSPCapabilities)
            return iPSSPCapabilities;
        throw new ParameterNotSetException("IP SSP Capabilities: not set");
    }
    /** Sets IP SSP Capabilities parameter.  */
    public void setIPSSPCapabilities (java.lang.String iPSSPCapabilities) {
        this.iPSSPCapabilities = iPSSPCapabilities;
        this.isIPSSPCapabilities = true;
    }
    /** Indicates if the IP SSP Capabilities optional parameter is
      * present.
      * @return
      * True when present, false otherwise.  */
    public boolean isIPSSPCapabilitiesPresent() {
        return isIPSSPCapabilities;
    }
    /** Gets IP Available.  */
    public java.lang.String getIPAvailable()
        throws ParameterNotSetException {
        if (isIPAvailable)
            return iPAvailable;
        throw new ParameterNotSetException();
    }
    /** Sets IP Available.  */
    public void setIPAvailable (java.lang.String iPAvailable) {
        this.iPAvailable = iPAvailable;
        this.isIPAvailable = true;
    }
    /** Indicates if the IP Available optional parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isIPAvailablePresent() {
        return isIPAvailable;
    }
    /** Gets ISDN Access Related Information.  */
    public java.lang.String getISDNAccessRelatedInformation()
        throws ParameterNotSetException {
        if (isISDNAccessRelatedInformation)
            return iSDNAccessRelatedInformation;
        throw new ParameterNotSetException();
    }
    /** Sets ISDN Access Related Information.  */
    public void setISDNAccessRelatedInformation(java.lang.String iSDNAccessRelatedInformation) {
        this.iSDNAccessRelatedInformation = iSDNAccessRelatedInformation;
        this.isISDNAccessRelatedInformation = true;
    }
    /** Indicates if the ISDN Access Related Information optional
      * parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isISDNAccessRelatedInformationPresent() {
        return isISDNAccessRelatedInformation;
    }
    /** Gets Call Gap Encountered.
      * Indicates the type of automatic Call Gapping encountered, if any: <ul>
      * <li>NO_CG_ENCOUNTERED
      * <li>MANUAL_CG_ENCOUNTERED
      * <li>SCP_OVERLOAD </ul>
      */
    public CGEncountered getCGEncountered()
        throws ParameterNotSetException {
        if (isCGEncountered)
            return cGEncountered;
        throw new ParameterNotSetException();
    }
    /** Sets Call Gap Encountered.  */
    public void setCGEncountered( CGEncountered cGEncountered) {
        this.cGEncountered = cGEncountered;
        this.isCGEncountered = true;
    }
    /** Indicates if the Call Gap Encountered optional parameter is
      * present.
      * @return
      * True when present, false otherwise.  */
    public boolean isCGEncounteredPresent() {
        return isCGEncountered;
    }
    /** Gets Location Number.  */
    public LocationNumber getLocationNumber()
        throws ParameterNotSetException {
        if (isLocationNumber)
            return locationNumber;
        throw new ParameterNotSetException("Location Number: not set.");
    }
    /** Sets Location Number.  */
    public void setLocationNumber (LocationNumber locationNumber) {
        this.locationNumber = locationNumber;
        this.isLocationNumber = true;
    }
    /** Indicates if the Location Number optional parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isLocationNumberPresent() {
        return isLocationNumber;
    }
    /** Gets Service Profile Identifier.  */
    public java.lang.String getServiceProfileIdentifier()
        throws ParameterNotSetException {
        if (isServiceProfileIdentifier)
            return serviceProfileIdentifier;
        throw new ParameterNotSetException("Service Profile Identifier: not set.");
    }
    /** Sets Service Profile Identifier.  */
    public void setServiceProfileIdentifier(java.lang.String serviceProfileIdentifier) {
        this.serviceProfileIdentifier = serviceProfileIdentifier;
        this.isServiceProfileIdentifier = true;
    }
    /** Indicates if the Service Profile Identifier optional parameter
      * is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isServiceProfileIdentifierPresent() {
        return isServiceProfileIdentifier;
    }
    /** Gets Terminal Type.  This parameter gets the Terminal Type so
      * that the SCF can specify, to the SRF, appropriate type of
      * capability (voice recognition, DTMF, display capability etc.).
      * <ul>
      * <li>UNKNOWN
      * <li>DIAL_PULSE  
      * <li>DTMF
      * <li>ISDN
      * <li>ISDN_NO_DTMF        
      * <li>SPARE </ul> */
    public TerminalType getTerminalType()
        throws ParameterNotSetException {
        if (isTerminalType)
            return terminalType;
        throw new ParameterNotSetException("Terminal Type: not set.");
    }
    /** Sets Terminal Type.  */
    public void setTerminalType(TerminalType terminalType) {
        this.terminalType = terminalType;
        this.isTerminalType = true;
    }
    /** Indicates if the Terminal Type optional parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isTerminalTypePresent() {
        return isTerminalType;
    }
    /** Gets Charge Number.  */
    public LocationNumber getChargeNumber()
        throws ParameterNotSetException {
        if (isChargeNumber)
            return chargeNumber;
        throw new ParameterNotSetException();
    }
    /** Sets Charge Number.  */
    public void setChargeNumber(LocationNumber chargeNumber) {
        this.chargeNumber = chargeNumber;
        isChargeNumber = true;
    }
    /** Indicates if the ChargeNumber optional parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isChargeNumberPresent() {
        return isChargeNumber;
    }
    /** Gets Serving Area ID.  */
    public LocationNumber getServingAreaID()
        throws ParameterNotSetException {
        if (isServingAreaID)
            return servingAreaID;
        throw new ParameterNotSetException("Serving Area ID: not set.");
    }
    /** Sets Serving AreaID.  */
    public void setServingAreaID( LocationNumber servingAreaID) {
        this.servingAreaID = servingAreaID;
        this.isServingAreaID = true;
    }
    /** Indicates if the Serving AreaID  optional parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isServingAreaIDPresent() {
        return isServingAreaID;
    }
    /**Gets Service Interaction Indicators.  */
    public java.lang.String getServiceInteractionIndicators()
        throws ParameterNotSetException {
        if (isServiceInteractionIndicators)
            return serviceInteractionIndicators;
        throw new ParameterNotSetException("Service Interaction Indicators: not set.");
    }
    /**Sets Service Interaction Indicators.  */
    public void setServiceInteractionIndicators(java.lang.String serviceInteractionIndicators) {
        this.serviceInteractionIndicators = serviceInteractionIndicators;
        this.isServiceInteractionIndicators = true; 
    }
    /** Indicates if the Service Interaction Indicators optional
      * parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isServiceInteractionIndicatorsPresent() {
        return isServiceInteractionIndicators;
    }
    /** Gets IN Service Compatibility Indication.  */
    public INServiceCompatibilityIndication getINServiceCompatibilityIndication()
        throws ParameterNotSetException {
        if (isINServiceCompatibilityIndication)
            return iNServiceCompatibilityIndication;
        throw new ParameterNotSetException("IN Service Compatibility Interaction: not set.");
    }
    /** Sets IN Service Compatibility Indication.  */
    public void setINServiceCompatibilityIndication(INServiceCompatibilityIndication iNServiceCompatibilityIndication) {
        this.iNServiceCompatibilityIndication = iNServiceCompatibilityIndication;
        isINServiceCompatibilityIndication = true;
    }
    /** Indicates if the IN Service Compatibility Indication optional
      * parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isINServiceCompatibilityIndicationPresent() {
        return isINServiceCompatibilityIndication;
    }
    /** Gets Service Interaction IndicatorsTwo (CS-2).  */
    public ServiceInteractionIndicatorsTwo getServiceInteractionIndicatorsTwo()
        throws ParameterNotSetException {
        if (isServiceInteractionIndicatorsTwo)
            return serviceInteractionIndicatorsTwo;
        throw new ParameterNotSetException("Service Interaction Indicators: not set.");
    }
    /** Sets Service Interaction IndicatorsTwo (CS-2).  */
    public void setServiceInteractionIndicatorsTwo(ServiceInteractionIndicatorsTwo serviceInteractionIndicatorsTwo) {
        this.serviceInteractionIndicatorsTwo = serviceInteractionIndicatorsTwo;
        this.isServiceInteractionIndicatorsTwo = true;
    }
    /** Indicates if the Service Interaction IndicatorsTwo optional
      * parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isServiceInteractionIndicatorsTwoPresent() {
        return isServiceInteractionIndicatorsTwo;
    }
    /** Gets USI Service Indicator.  */
    public java.lang.String getUSIServiceIndicator()
        throws ParameterNotSetException {
        if (isUSIServiceIndicator)
            return uSIServiceIndicator;
        throw new ParameterNotSetException("USI Service Indicator: not set.");
    }
    /** Sets USI Service Indicator.  */
    public void setUSIServiceIndicator(java.lang.String uSIServiceIndicator) {
        this.uSIServiceIndicator = uSIServiceIndicator;
        this.isUSIServiceIndicator = true;
    }
    /** Indicates if the USI Service Indicator optional parameter is
      * present.
      * @return
      * True when present, false otherwise.  */
    public boolean isUSIServiceIndicatorPresent() {
        return isUSIServiceIndicator;
    }
    /** Gets USI Information.  */
    public java.lang.String getUSIInformation()
        throws ParameterNotSetException {
        if (isUSIInformation)
            return uSIInformation;
        throw new ParameterNotSetException("USI Information: not set.");
    }
    /** Sets USI Information.  */
    public void setUSIInformation(java.lang.String uSIInformation) {
        this.uSIInformation = uSIInformation;
        this.isUSIInformation = true;
    }
    /** Indicates if the USI Information optional parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isUSIInformationPresent() {
        return isUSIInformation;
    }
    /** Gets Forward GVNS.  */
    public ForwardGVNS getForwardGVNS()
        throws ParameterNotSetException {
        if (isForwardGVNS)
            return forwardGVNS;
        throw new ParameterNotSetException("Forward GVNS: not set.");
    }
    /** Sets Forward GVNS.  */
    public void setForwardGVNS( ForwardGVNS forwardGVNS) {
        this.forwardGVNS = forwardGVNS;
        this.isForwardGVNS = true;
    }
    /** Indicates if the Forward GVNS optional parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isForwardGVNSPresent() {
        return isForwardGVNS;
    }
    /** Gets Created Call Segment Association.  */
    public int getCreatedCallSegmentAssociation()
        throws ParameterNotSetException {
        if (isCreatedCallSegmentAssociation)
            return createdCallSegmentAssociation;
        throw new ParameterNotSetException("Created Call Segment Association: not set.");
    }
    /** Sets Created Call Segment Association.  */
    public void setCreatedCallSegmentAssociation(int createdCallSegmentAssociation) {
        this.createdCallSegmentAssociation = createdCallSegmentAssociation;
        this.isCreatedCallSegmentAssociation = true;
    }
    /** Indicates if the Created Call Segment Association optional
      * parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isCreatedCallSegmentAssociationPresent() {
        return isCreatedCallSegmentAssociation;
    }
    /** Gets Extensions Parameter.  */
    public ExtensionField[] getExtensions()
        throws ParameterNotSetException {
        if (isExtensions)
            return extensions;
        throw new ParameterNotSetException("Extensions: not set.");
    }
    /** Gets a particular Extension Parameter.  */
    public ExtensionField getExtension(int index) {
        return extensions[index];
    }
    /** Sets Extensions Parameter.  */
    public void setExtensions(ExtensionField extensions[]) {
        this.extensions = extensions;
        this.isExtensions = true; 
    }
    /** Sets a particular Extensions Parameter.  */
    public void setExtension (int index, ExtensionField extension) {
        this.extensions[index] = extension ;
    }
    /** Indicates if the Extensions optional parameter is present.
      * @return
      * True when present, false otherwise.  */
    public boolean isExtensionsPresent() {
        return isExtensions;
    }
    private ServiceAddressInformation serviceAddressInformation;
    private BearerCapability bearerCapability;
    private CalledPartyNumber calledPartyNumber;
    private CallingPartyNumber callingPartyNumber;
    private java.lang.String callingPartysCategory;
    private java.lang.String iPSSPCapabilities;
    private java.lang.String iPAvailable;
    private java.lang.String iSDNAccessRelatedInformation;
    private CGEncountered cGEncountered;
    private LocationNumber locationNumber;
    private java.lang.String serviceProfileIdentifier;
    private TerminalType terminalType;
    private LocationNumber chargeNumber;
    private LocationNumber servingAreaID;
    private java.lang.String serviceInteractionIndicators;
    private INServiceCompatibilityIndication iNServiceCompatibilityIndication;
    private ServiceInteractionIndicatorsTwo serviceInteractionIndicatorsTwo;
    private java.lang.String uSIServiceIndicator;
    private java.lang.String uSIInformation;
    private ForwardGVNS forwardGVNS;
    private int createdCallSegmentAssociation;
    private ExtensionField extensions[];
    private boolean isBearerCapability = false;
    private boolean isCalledPartyNumber = false;
    private boolean isCallingPartyNumber = false;
    private boolean isCallingPartysCategory = false;
    private boolean isIPSSPCapabilities = false;
    private boolean isIPAvailable = false;
    private boolean isISDNAccessRelatedInformation = false;
    private boolean isCGEncountered =false;
    private boolean isLocationNumber = false;
    private boolean isServiceProfileIdentifier = false;
    private boolean isTerminalType = false;
    private boolean isChargeNumber = false;
    private boolean isServingAreaID = false;
    private boolean isServiceInteractionIndicators =false;
    private boolean isINServiceCompatibilityIndication = false;
    private boolean isServiceInteractionIndicatorsTwo = false;
    private boolean isUSIServiceIndicator = false;
    private boolean isUSIInformation = false;
    private boolean isForwardGVNS = false;
    private boolean isCreatedCallSegmentAssociation = false;
    private boolean isExtensions = false; 
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
