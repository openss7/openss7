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

package javax.jain.ss7.inap.operation;

import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.datatype.*;
import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** This class represents the InitialDP Operation.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class InitialDP  extends Operation   implements java.io.Serializable
{
    /** Constructor For InitialDP.  */
    public InitialDP(int serviceKey,TriggerType triggerType) {
        operationCode = OperationCode.INITIAL_DP;
        try {
            setServiceKey(serviceKey);
            setTriggerType(triggerType);
        } catch (IllegalArgumentException ex) {
            System.err.println(ex.getMessage());
        }
    }
    /** Gets Operation Code.  */
    public OperationCode getOperationCode() {
        return operationCode;
    }
    /** Gets Service Key.  */
    public int  getServiceKey() {
        return serviceKey;
    }
    /** Sets Service Key.  */
    public void setServiceKey(int serviceKey) throws IllegalArgumentException {
        if (0 <= serviceKey && serviceKey <= 2147483647) {
            this.serviceKey = serviceKey;
            return;
        }
        throw new IllegalArgumentException("Service Key: " + serviceKey + ", out of range.");
    }
    /** Gets Dialled Digits.  */
    public CalledPartyNumber getDialledDigits() throws ParameterNotSetException {
        if (isDialledDigits)
            return dialledDigits;
        throw new ParameterNotSetException();
    }
    /** Sets Dialled Digits.  */
    public void setDialledDigits(CalledPartyNumber dialledDigits) {
        this.dialledDigits = dialledDigits;
        this.isDialledDigits = true;
    }
    /** Indicates if the Dialled Digits optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isDialledDigitsPresent() {
        return isDialledDigits;
    }
    /** Gets Called Party Number.  */
    public CalledPartyNumber getCalledPartyNumber() throws ParameterNotSetException {
        if (isCalledPartyNumber)
            return calledPartyNumber;
        throw new ParameterNotSetException("Called Party Number: not set.");
    }
    /** Sets Called Party Number.  */
    public void setCalledPartyNumber(CalledPartyNumber calledPartyNumber) {
        this.calledPartyNumber = calledPartyNumber;
        this.isCalledPartyNumber = true;
    }
    /** Indicates if the Called Party Number optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isCalledPartyNumberPresent() {
        return isCalledPartyNumber;
    }
    /** Gets Calling Party Number.  */
    public CallingPartyNumber getCallingPartyNumber() throws ParameterNotSetException {
        if (isCallingPartyNumber)
            return callingPartyNumber;
        throw new ParameterNotSetException("Calling Party Number: not set.");
    }
    /** Sets Calling Party Number.  */
    public void setCallingPartyNumber(CallingPartyNumber callingPartyNumber) {
        this.callingPartyNumber = callingPartyNumber;
        this.isCallingPartyNumber = true;
    }
    /** Indicates if the Calling Party Number optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isCallingPartyNumberPresent() {
        return isCallingPartyNumber;
    }
    /** Gets Calling Party Business Group ID.  */
    public java.lang.String getCallingPartyBusinessGroupID() throws ParameterNotSetException {
        if (isCallingPartyBusinessGroupID)
            return callingPartyBusinessGroupID;
        throw new ParameterNotSetException("Calling Party Business Group Id: not set.");
    }
    /** Sets  Calling Party Business Group ID.  */
    public void setCallingPartyBusinessGroupID(java.lang.String callingPartyBusinessGroupID) {
        this.callingPartyBusinessGroupID = callingPartyBusinessGroupID;
        this.isCallingPartyBusinessGroupID = true;
    }
    /** Indicates if the Calling Party Business Group ID optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isCallingPartyBusinessGroupIDPresent() {
        return isCallingPartyBusinessGroupID;
    }
    /** Gets Calling Party's Category.  */
    public java.lang.String getCallingPartysCategory() throws ParameterNotSetException {
        if (isCallingPartysCategory)
            return callingPartysCategory;
        throw new ParameterNotSetException("Calling Party Category: not set.");
    }
    /** Sets Calling Party's Category.  */
    public void setCallingPartysCategory(java.lang.String callingPartysCategory) {
        this.callingPartysCategory = callingPartysCategory;
        this.isCallingPartysCategory = true;
    }
    /** Indicates if the Calling Party's Category optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isCallingPartysCategoryPresent() {
        return isCallingPartysCategory;
    }
    /** Gets Calling Party Subaddress.  */
    public CallingPartySubaddress getCallingPartySubaddress() throws ParameterNotSetException {
        if (isCallingPartySubaddress)
            return callingPartySubaddress;
        throw new ParameterNotSetException("Calling Party Subaddress: not set.");
    }
    /** Sets Calling Party Subaddress.  */
    public void setCallingPartySubaddress(CallingPartySubaddress callingPartySubaddress) {
        this.callingPartySubaddress = callingPartySubaddress;
        this.isCallingPartySubaddress = true;
    }
    /** Indicates if the Calling Party Subaddress optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isCallingPartySubaddressPresent() {
        return isCallingPartySubaddress;
    }
    /** Gets Call Gap Encountered.  Indicates the type of automatic Call
      * Gapping encountered, if any: <ul>
      * <li>NO_CG_ENCOUNTERED
      * <li>MANUAL_CG_ENCOUNTERED
      * <li>SCP_OVERLOAD </ul> */
    public CGEncountered getCGEncountered() throws ParameterNotSetException {
        if (isCGEncountered)
            return cGEncountered;
        throw new ParameterNotSetException("Call Gap Encountered: not set.");
    }
    /** Sets Call Gap Encountered.  */
    public void setCGEncountered(CGEncountered cGEncountered) {
        this.cGEncountered = cGEncountered;
        this.isCGEncountered = true;
    }
    /** Indicates if the Call Gap Encountered optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isCGEncounteredPresent() {
        return isCGEncountered;
    }
    /** Gets IP SSP Capabilities.  */
    public java.lang.String getIPSSPCapabilities() throws ParameterNotSetException {
        if (isIPSSPCapabilities)
            return iPSSPCapabilities;
        throw new ParameterNotSetException("IP SSP Capabilities: not set.");
    }
    /** Sets IP SSP Capabilities.  */
    public void setIPSSPCapabilities(java.lang.String iPSSPCapabilities) {
        this.iPSSPCapabilities = iPSSPCapabilities;
        this.isIPSSPCapabilities = true;
    }
    /** Indicates if the IP SSP Capabilities optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isIPSSPCapabilitiesPresent() {
        return isIPSSPCapabilities;
    }
    /** Gets IPAvailable.  */
    public java.lang.String getIPAvailable() throws ParameterNotSetException {
        if (isIPAvailable)
            return iPAvailable;
        throw new ParameterNotSetException("IP Available: not set.");
    }
    /** Sets IPAvailable.  */
    public void setIPAvailable(java.lang.String iPAvailable) {
        this.iPAvailable = iPAvailable;
        this.isIPAvailable = true;
    }
    /** Indicates if the IPAvailable optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isIPAvailablePresent() {
        return isIPAvailable;
    }
    /** Gets Location Number.  */
    public LocationNumber getLocationNumber() throws ParameterNotSetException {
        if (isLocationNumber)
            return locationNumber;
        throw new ParameterNotSetException("Location Number: not set.");
    }
    /** Sets Location Number.  */
    public void setLocationNumber(LocationNumber locationNumber) {
        this.locationNumber = locationNumber;
        this.isLocationNumber = true;
    }
    /** Indicates if the Location Number optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isLocationNumberPresent() {
        return isLocationNumber;
    }
    /** Gets Misc Call Info.  */
    public MiscCallInfo getMiscCallInfo() throws ParameterNotSetException {
        if (isMiscCallInfo)
            return miscCallInfo;
        throw new ParameterNotSetException("Misc Call Info: not set.");
    }
    /** Sets Misc Call Info.  */
    public void setMiscCallInfo(MiscCallInfo miscCallInfo) {
        this.miscCallInfo = miscCallInfo;
        this.isMiscCallInfo = true;
    }
    /** Indicates if the Misc Call Info optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isMiscCallInfoPresent() {
        return isMiscCallInfo;
    }
    /** Gets Original Called Party ID.  */
    public OriginalCalledPartyID getOriginalCalledPartyID() throws ParameterNotSetException {
        if (isOriginalCalledPartyID)
            return originalCalledPartyID;
        throw new ParameterNotSetException("Original Called Party Id: not set.");
    }
    /** Sets Original Called Party ID.  */
    public void setOriginalCalledPartyID(OriginalCalledPartyID originalCalledPartyID) {
        this.originalCalledPartyID = originalCalledPartyID;
        this.isOriginalCalledPartyID = true;
    }
    /** Indicates if the Original Called Party ID optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isOriginalCalledPartyIDPresent() {
        return isOriginalCalledPartyID;
    }
    /** Gets Service Profile Identifier.  */
    public java.lang.String getServiceProfileIdentifier() throws ParameterNotSetException {
        if (isServiceProfileIdentifier)
            return serviceProfileIdentifier;
        throw new ParameterNotSetException("Service Profile Identifier: not set.");
    }
    /** Sets Service Profile Identifier.  */
    public void setServiceProfileIdentifier(java.lang.String serviceProfileIdentifier) {
        this.serviceProfileIdentifier = serviceProfileIdentifier;
        this.isServiceProfileIdentifier = true;
    }
    /** Indicates if the Service Profile Identifier optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isServiceProfileIdentifierPresent() {
        return isServiceProfileIdentifier;
    }
    /** Gets Terminal Type.  This parameter gets the Terminal Type so that
      * the SCF can specify, to the SRF, appropriate type of capability
      * (voice recognition, DTMF, display capability etc.). <ul>
      * <li>UNKNOWN
      * <li>DIAL_PULSE
      * <li>DTMF
      * <li>ISDN
      * <li>ISDN_NO_DTMF
      * <li>SPARE </ul> */
    public TerminalType getTerminalType() throws ParameterNotSetException {
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
      * @return True when present, false otherwise.  */
    public boolean isTerminalTypePresent() {
        return isTerminalType;
    }
    /** Gets Trigger Type.  */
    public TriggerType getTriggerType() {
        return triggerType;
    }
    /** Sets Trigger Type.  */
    public void setTriggerType(TriggerType triggerType) {
        this.triggerType = triggerType;
    }
    /** Gets High Layer Compatibility*/
    public HighLayerCompatibility getHighLayerCompatibility() throws ParameterNotSetException {
        if (isHighLayerCompatibility)
            return highLayerCompatibility;
        throw new ParameterNotSetException("High Layer Compatibility: not set.");
    }
    /** Sets High Layer Compatibility.  */
    public void setHighLayerCompatibility(HighLayerCompatibility highLayerCompatibility) {
        this.highLayerCompatibility = highLayerCompatibility;
        this.isHighLayerCompatibility = true;
    }
    /** Indicates if the High Layer Compatibility optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isHighLayerCompatibilityPresent() {
        return isHighLayerCompatibility;
    }
    /** Gets Service Interaction Indicators.  */
    public java.lang.String getServiceInteractionIndicators() throws ParameterNotSetException {
        if (isServiceInteractionIndicators)
            return serviceInteractionIndicators;
        throw new ParameterNotSetException("Service Interaction Indicators: not set.");
    }
    /** Sets Service Interaction Indicators.  */
    public void setServiceInteractionIndicators(java.lang.String serviceInteractionIndicators) {
        this.serviceInteractionIndicators = serviceInteractionIndicators;
        this.isServiceInteractionIndicators = true;
    }
    /** Indicates if the Service Interaction Indicators optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isServiceInteractionIndicatorsPresent() {
        return isServiceInteractionIndicators;
    }
    /** Gets Additional Calling Party Number.  */
    public DigitsGenericNumber getAdditionalCallingPartyNumber() throws ParameterNotSetException {
        if (isAdditionalCallingPartyNumber)
            return additionalCallingPartyNumber;
        throw new ParameterNotSetException("Additional Calling Party Number: not set.");
    }
    /** Sets Additional Calling Party Number.  */
    public void setAdditionalCallingPartyNumber(DigitsGenericNumber additionalCallingPartyNumber) {
        this.additionalCallingPartyNumber = additionalCallingPartyNumber;
        this.isAdditionalCallingPartyNumber = true;
    }
    /** Indicates if the Additional Calling Party Number optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isAdditionalCallingPartyNumberPresent() {
        return isAdditionalCallingPartyNumber;
    }
    /** Gets Forward Call Indicators.  */
    public ForwardCallIndicators getForwardCallIndicators() throws ParameterNotSetException {
        if (isForwardCallIndicators)
            return forwardCallIndicators;
        throw new ParameterNotSetException("Forward Call Indicators: not set.");
    }
    /** Sets Forward Call Indicators.  */
    public void setForwardCallIndicators(ForwardCallIndicators forwardCallIndicators) {
        this.forwardCallIndicators = forwardCallIndicators;
        this.isForwardCallIndicators = true;
    }
    /** Indicates if the Forward Call Indicators optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isForwardCallIndicatorsPresent() {
        return isForwardCallIndicators;
    }
    /** Gets Bearer Capability.  */
    public BearerCapability getBearerCapability() throws ParameterNotSetException {
        if (isBearerCapability)
            return bearerCapability;
        throw new ParameterNotSetException("Bearer Capability: not set.");
    }
    /** Sets Bearer Capability.  */
    public void setBearerCapability(BearerCapability bearerCapability) {
        this.bearerCapability = bearerCapability;
        this.isBearerCapability = true;
    }
    /** Indicates if the Bearer Capability optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isBearerCapabilityPresent() {
        return isBearerCapability;
    }
    /** Gets Event Type BCSM.  This parameter gets the BCSM detection point
      * event. <ul>
      * <LI>ORIG_ATTEMPT_AUTHORIZED
      * <LI>COLLECTED_INFO
      * <LI>ANALYSED_INFORMATION
      * <LI>ROUTE_SELECT_FAILURE
      * <LI>O_CALLED_PARTY_BUSY
      * <LI>O_NO_ANSWER
      * <LI>O_ANSWER
      * <LI>O_MID_CALL
      * <LI>O_DISCONNECT
      * <LI>O_ABANDON
      * <LI>TERM_ATTEMPT_AUTHORIZED
      * <LI>T_BUSY
      * <LI>T_NO_ANSWER
      * <LI>T_ANSWER
      * <LI>T_MID_CALL
      * <LI>T_DISCONNECT
      * <LI>T_ABANDON
      * <LI>O_TERM_SEIZED
      * <LI>O_SUSPENDED
      * <LI>T_SUSPENDED
      * <LI>ORIG_ATTEMPT
      * <LI>TERM_ATTEMPT
      * <LI>O_RE_ANSWER
      * <LI>T_RE_ANSWER
      * <LI>FACILITY_SELECTED_AND_AVAILABLE
      * <LI>CALL_ACCPETED </ul> */
    public EventTypeBCSM getEventTypeBCSM() throws ParameterNotSetException {
        if (isEventTypeBCSM)
            return eventTypeBCSM;
        throw new ParameterNotSetException("Event Type BCSM: not set.");
    }
    /** Sets Event Type BCSM.  */
    public void setEventTypeBCSM(EventTypeBCSM eventTypeBCSM) {
        this.eventTypeBCSM = eventTypeBCSM;
        this.isEventTypeBCSM = true;
    }
    /** Indicates if the Event Type BCSM optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isEventTypeBCSMPresent() {
        return isEventTypeBCSM;
    }
    /** Gets Redirecting Party ID.  */
    public RedirectingPartyID getRedirectingPartyID() throws ParameterNotSetException {
        if (isRedirectingPartyID)
            return redirectingPartyID;
        throw new ParameterNotSetException("Redirecting Party Id: not set.");
    }
    /** Sets Redirecting Party ID.  */
    public void setRedirectingPartyID(RedirectingPartyID redirectingPartyID) {
        this.redirectingPartyID = redirectingPartyID;
        this.isRedirectingPartyID = true;
    }
    /** Indicates if the Redirecting Party ID optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isRedirectingPartyIDPresent() {
        return isRedirectingPartyID;
    }
    /** Gets Redirection Information.  */
    public RedirectionInformation getRedirectionInformation() throws ParameterNotSetException {
        if (isRedirectionInformation)
            return redirectionInformation;
        throw new ParameterNotSetException("Redirection Information: not set.");
    }
    /** Sets Redirection Information.  */
    public void setRedirectionInformation(RedirectionInformation redirectionInformation) {
        this.redirectionInformation = redirectionInformation;
        this.isRedirectionInformation = true;
    }
    /** Indicates if the Redirection Information optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isRedirectionInformationPresent() {
        return isRedirectionInformation;
    }
    /** Gets Cause.  */
    public Cause getCause() throws ParameterNotSetException {
        if (isCause)
            return cause;
        throw new ParameterNotSetException("Cause: not set.");
    }
    /** Sets Cause.  */
    public void setCause(Cause cause) {
        this.cause = cause;
        this.isCause = true;
    }
    /** Indicates if the Cause optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isCausePresent() {
        return isCause;
    }
    /** Gets ComponentType.  This parameter indicates the type of event that
      * is reported to the SSF: <ul>
      * <li>ANY
      * <li>INVOKE
      * <li>R_RESULT
      * <li>R_ERROR
      * <li>R_REJECT
      * <li>R_RESULT_NOT_LAST </ul> */
    public ComponentType getComponentType() throws ParameterNotSetException {
        if (isComponentType)
            return componentType;
        throw new ParameterNotSetException("Component Type: not set.");
    }
    /** Sets Component Type.  */
    public void setComponentType(ComponentType componentType) {
        this.componentType = componentType;
        this.isComponentType = true;
    }
    /** Indicates if the Component Type  optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isComponentTypePresent() {
        return isComponentType;
    }
    /** Gets Component CorrelationID.  */
    public int getComponentCorrelationID() throws ParameterNotSetException {
        if (isComponentCorrelationID)
            return componentCorrelationID;
        throw new ParameterNotSetException("Component Correlation Id: not set.");
    }
    /** Sets Component CorrelationID.  */
    public void setComponentCorrelationID(int componentCorrelationID) {
        this.componentCorrelationID = componentCorrelationID;
        this.isComponentCorrelationID = true;
    }
    /** Indicates if the Component CorrelationID optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isComponentCorrelationIDPresent() {
        return isComponentCorrelationID;
    }
    /** Gets ISDN Access Related Information.  */
    public java.lang.String getISDNAccessRelatedInformation() throws ParameterNotSetException {
        if (isISDNAccessRelatedInformation)
            return iSDNAccessRelatedInformation;
        throw new ParameterNotSetException("ISDN Access Related Information: not set.");
    }
    /** Sets ISDN Access Related Information.  */
    public void setISDNAccessRelatedInformation(java.lang.String iSDNAccessRelatedInformation) {
        this.iSDNAccessRelatedInformation = iSDNAccessRelatedInformation;
        this.isISDNAccessRelatedInformation = true;
    }
    /** Indicates if the ISDN Access Related Information optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isISDNAccessRelatedInformationPresent() {
        return isISDNAccessRelatedInformation;
    }
    /** Gets IN Service Compatibility Indication.  */
    public INServiceCompatibilityIndication getINServiceCompatibilityIndication() throws ParameterNotSetException {
        if (isINServiceCompatibilityIndication)
            return iNServiceCompatibilityIndication;
        throw new ParameterNotSetException("IN Service Compatibility Indication: not set.");
    }
    /** Sets IN Service Compatibility Indication.  */
    public void setINServiceCompatibilityIndication(INServiceCompatibilityIndication iNServiceCompatibilityIndication) {
        this.iNServiceCompatibilityIndication = iNServiceCompatibilityIndication;
        this.isINServiceCompatibilityIndication = true;
    }
    /** Indicates if the IN Service Compatibility Indication optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isINServiceCompatibilityIndicationPresent() {
        return isINServiceCompatibilityIndication;
    }
    /** Gets Generic Numbers.  */
    public GenericNumbers getGenericNumbers() throws ParameterNotSetException {
        if (isGenericNumbers)
            return genericNumbers;
        throw new ParameterNotSetException("Generic Numbers: not set.");
    }
    /** Sets Generic Numbers.  */
    public void setGenericNumbers(GenericNumbers genericNumbers) {
        this.genericNumbers = genericNumbers;
        this.isGenericNumbers = true;
    }
    /** Indicates if the Generic Numbers optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isGenericNumbersPresent() {
        return isGenericNumbers;
    }
    /** Gets Service Interaction Indicators Two (CS-2).  */
    public ServiceInteractionIndicatorsTwo getServiceInteractionIndicatorsTwo() throws ParameterNotSetException {
        if (isServiceInteractionIndicatorsTwo)
            return serviceInteractionIndicatorsTwo;
        throw new ParameterNotSetException("Service Interaction Indicators 2: not set.");
    }
    /** Sets Service Interaction Indicators Two (CS-2).  */
    public void setServiceInteractionIndicatorsTwo(ServiceInteractionIndicatorsTwo serviceInteractionIndicatorsTwo) {
        this.serviceInteractionIndicatorsTwo = serviceInteractionIndicatorsTwo;
        this.isServiceInteractionIndicatorsTwo = true;
    }
    /** Indicates if the Service Interaction Indicators Two optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isServiceInteractionIndicatorsTwoPresent() {
        return isServiceInteractionIndicatorsTwo;
    }
    /** Gets Forward GVNS.  */
    public ForwardGVNS getForwardGVNS() throws ParameterNotSetException {
        if (isForwardGVNS)
            return forwardGVNS;
        throw new ParameterNotSetException("Forward GVNS: not set.");
    }
    /** Sets Forward GVNS.  */
    public void setForwardGVNS(ForwardGVNS forwardGVNS) {
        this.forwardGVNS = forwardGVNS;
        this.isForwardGVNS=true;
    }
    /** Indicates if the Forward GVNS optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isForwardGVNSPresent() {
        return isForwardGVNS;
    }
    /** Gets Created Call Segment Association.  */
    public int getCreatedCallSegmentAssociation() throws ParameterNotSetException {
        if (isCreatedCallSegmentAssociation)
            return createdCallSegmentAssociation;
        throw new ParameterNotSetException("Created Call Segment Association: not set.");
    }
    /** Sets Created Call Segment Association.  */
    public void setCreatedCallSegmentAssociation(int createdCallSegmentAssociation) {
        this.createdCallSegmentAssociation = createdCallSegmentAssociation;
        this.isCreatedCallSegmentAssociation=true;
    }
    /** Indicates if the Created Call Segment Association optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isCreatedCallSegmentAssociationPresent() {
        return isCreatedCallSegmentAssociation;
    }
    /**Gets USI Service Indicator.  */
    public java.lang.String getUSIServiceIndicator() throws ParameterNotSetException {
        if (isUSIServiceIndicator)
            return uSIServiceIndicator;
        throw new ParameterNotSetException("USI Service Indicator: not set.");
    }
    /**Sets USI Service Indicator.  */
    public void setUSIServiceIndicator(java.lang.String uSIServiceIndicator) {
        this.uSIServiceIndicator = uSIServiceIndicator;
        this.isUSIServiceIndicator=true;
    }
    /** Indicates if the Sets USI Service Indicator optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isUSIServiceIndicatorPresent() {
        return isUSIServiceIndicator;
    }
    /** Gets USI Information.  */
    public java.lang.String getUSIInformation() throws ParameterNotSetException {
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
      * @return True when present, false otherwise.  */
    public boolean isUSIInformationPresent() {
        return isUSIInformation;
    }
    /** Gets Extensions Parameter.  */
    public ExtensionField[] getExtensions()throws ParameterNotSetException {
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
    public void setExtension(int index, ExtensionField extension) {
        this.extensions[index] = extension;
    }
    /** Indicates if the Extensions optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isExtensionsPresent() {
        return isExtensions;
    }
    /** Gets Component.  */
    public Component getComponent() throws ParameterNotSetException {
        if (isComponent)
            return component;
        throw new ParameterNotSetException("Component: not set.");
    }
    /** Sets Component.  */
    public void  setComponent(Component component) {
        this.component = component;
        this.isComponent = true;
    }
    /** Indicates if the Component optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isComponentPresent() {
        return isComponent;
    }
    private int serviceKey;
    private CalledPartyNumber dialledDigits;
    private boolean isDialledDigits = false;
    private CalledPartyNumber calledPartyNumber;
    private boolean isCalledPartyNumber = false;
    private CallingPartyNumber callingPartyNumber;
    private boolean isCallingPartyNumber = false;
    private java.lang.String callingPartyBusinessGroupID;
    private boolean isCallingPartyBusinessGroupID = false;
    private java.lang.String callingPartysCategory;
    private boolean isCallingPartysCategory = false;
    private CallingPartySubaddress callingPartySubaddress;
    private boolean isCallingPartySubaddress =false;
    private CGEncountered cGEncountered;
    private boolean isCGEncountered = false;
    private java.lang.String iPSSPCapabilities;
    private boolean isIPSSPCapabilities = false;
    private java.lang.String iPAvailable;
    private boolean isIPAvailable = false;
    private LocationNumber locationNumber;
    private boolean isLocationNumber = false;
    private MiscCallInfo miscCallInfo;
    private boolean isMiscCallInfo = false;
    private OriginalCalledPartyID originalCalledPartyID;
    private boolean isOriginalCalledPartyID = false;
    private java.lang.String serviceProfileIdentifier;
    private boolean isServiceProfileIdentifier = false;
    private TerminalType terminalType;
    private boolean isTerminalType = false;
    private TriggerType triggerType;
    private HighLayerCompatibility highLayerCompatibility;
    private boolean isHighLayerCompatibility = false;
    private java.lang.String serviceInteractionIndicators;
    private boolean isServiceInteractionIndicators = false;
    private DigitsGenericNumber additionalCallingPartyNumber;
    private boolean isAdditionalCallingPartyNumber = false;
    private ForwardCallIndicators forwardCallIndicators;
    private boolean isForwardCallIndicators = false;
    private BearerCapability bearerCapability;
    private boolean isBearerCapability = false;
    private EventTypeBCSM eventTypeBCSM;
    private boolean isEventTypeBCSM = false;
    private RedirectingPartyID redirectingPartyID;
    private boolean isRedirectingPartyID = false;
    private RedirectionInformation redirectionInformation;
    private boolean isRedirectionInformation = false;
    private Cause cause;
    private boolean isCause = false;
    private ComponentType componentType;
    private boolean isComponentType = false;
    private int componentCorrelationID;
    private boolean isComponentCorrelationID = false;
    private java.lang.String iSDNAccessRelatedInformation;
    private boolean isISDNAccessRelatedInformation = false;
    private INServiceCompatibilityIndication iNServiceCompatibilityIndication;
    private boolean isINServiceCompatibilityIndication = false;
    private GenericNumbers genericNumbers;
    private boolean isGenericNumbers = false;
    private ServiceInteractionIndicatorsTwo serviceInteractionIndicatorsTwo;
    private boolean isServiceInteractionIndicatorsTwo = false;
    private ForwardGVNS forwardGVNS;
    private boolean isForwardGVNS = false;
    private int createdCallSegmentAssociation;
    private boolean isCreatedCallSegmentAssociation = false;
    private java.lang.String uSIServiceIndicator;
    private boolean isUSIServiceIndicator = false;
    private java.lang.String uSIInformation;
    private boolean isUSIInformation = false;
    private ExtensionField extensions[];
    private boolean isExtensions = false;
    private Component component;
    private boolean isComponent = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
