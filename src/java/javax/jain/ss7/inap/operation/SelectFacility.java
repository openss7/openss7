/*
 @(#) src/java/javax/jain/ss7/inap/operation/SelectFacility.java <p>
 
 Copyright &copy; 2008-2015  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
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
 <a href="http://www.openss7.com/">http://www.openss7.com/</a>
 */

package javax.jain.ss7.inap.operation;

import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.datatype.*;
import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** This class represents the Select Facility Operation.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class SelectFacility extends Operation implements java.io.Serializable {
    /** Constructor For SelectFacility.  */
    public SelectFacility() {
        operationCode = OperationCode.SELECT_FACILITY;
    }
    /** Gets Operation Code.  */
    public OperationCode getOperationCode() {
        return operationCode;
    }
    /** Gets Alerting Pattern.  */
    public java.lang.String getAlertingPattern() throws ParameterNotSetException {
        if (isAlertingPattern)
            return alertingPattern;
        throw new ParameterNotSetException("Alerting Pattern: not set.");
    }
    /** Sets Alerting Pattern.  */
    public void setAlertingPattern(java.lang.String alertingPattern) {
        this.alertingPattern = alertingPattern;
        this.isAlertingPattern = true;
    }
    /** Indicates if the AlertingPattern optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isAlertingPattern() {
        return isAlertingPattern;
    }
    /** Gets DestinationNumberRoutingAddress.  */
    public CalledPartyNumber getDestinationNumberRoutingAddress() throws ParameterNotSetException {
        if (isDestinationNumberRoutingAddress)
            return destinationNumberRoutingAddress;
        throw new ParameterNotSetException("Destination Routing Address: not set.");
    }
    /** Sets Destination Number Routing Address.  */
    public void setDestinationNumberRoutingAddress(CalledPartyNumber destinationNumberRoutingAddress) {
        this.destinationNumberRoutingAddress = destinationNumberRoutingAddress;
        this.isDestinationNumberRoutingAddress = true;
    }
    /** Indicates if the DestinationNumberRoutingAddress optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isDestinationNumberRoutingAddressPresent() {
        return isDestinationNumberRoutingAddress;
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
    /** Gets Called Facility Group.  */
    public FacilityGroup getCalledFacilityGroup() throws ParameterNotSetException {
        if (isCalledFacilityGroup)
            return calledFacilityGroup;
        throw new ParameterNotSetException("Called Facility Group: not set.");
    }
    /** Sets Called Facility Group.  */
    public void setCalledFacilityGroup(FacilityGroup calledFacilityGroup) {
        this.calledFacilityGroup = calledFacilityGroup;
        this.isCalledFacilityGroup = true;
    }
    /** Indicates if the Called Facility Group optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isCalledFacilityGroupPresent() {
        return isCalledFacilityGroup;
    }
    /** Gets Called Facility Group Member.  */
    public int getCalledFacilityGroupMember() throws ParameterNotSetException {
        if (isCalledFacilityGroupMember)
            return calledFacilityGroupMember;
        throw new ParameterNotSetException("Called Facility Group Member: not set.");
    }
    /** Sets Called Facility Group Member.  */
    public void setCalledFacilityGroupMember(int calledFacilityGroupMember) {
        this.calledFacilityGroupMember = calledFacilityGroupMember;
        this.isCalledFacilityGroupMember = true;
    }
    /** Indicates if the Called Facility Group Member optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isCalledFacilityGroupMemberPresent() {
        return isCalledFacilityGroupMember;
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
    /** Gets Display Information.  */
    public java.lang.String getDisplayInformation() throws ParameterNotSetException {
        if (isDisplayInformation)
            return displayInformation;
        throw new ParameterNotSetException("Display Information: not set.");
    }
    /** Sets Display Information.  */
    public void setDisplayInformation(java.lang.String displayInformation) {
        this.displayInformation = displayInformation;
        this.isDisplayInformation = true;
    }
    /** Indicates if the Display Information optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isDisplayInformationPresent() {
        return isDisplayInformation;
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
    /** Gets IN Service Compatibility Response.  */
    public Entry getINServiceCompatibilityResponse() throws ParameterNotSetException {
        if (isINServiceCompatibilityResponse)
            return iNServiceCompatibilityResponse;
        throw new ParameterNotSetException("IN Service Compatibility Response: not set.");
    }
    /** Sets IN Service Compatibility Response.  */
    public void setINServiceCompatibilityResponse(Entry iNServiceCompatibilityResponse) {
        this.iNServiceCompatibilityResponse = iNServiceCompatibilityResponse;
        this.isINServiceCompatibilityResponse = true;
    }
    /** Indicates if the IN Service Compatibility Response optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isINServiceCompatibilityResponsePresent() {
        return isINServiceCompatibilityResponse;
    }
    /** Gets Forward Global Virtual Network Services.  */
    public ForwardGVNS getForwardGVNS() throws ParameterNotSetException {
        if (isForwardGVNS)
            return forwardGVNS;
        throw new ParameterNotSetException("Forward Global Virtual Network Services: not set.");
    }
    /** Sets Forward Global Virtual Network Services.  */
    public void setForwardGVNS(ForwardGVNS forwardGVNS) {
        this.forwardGVNS = forwardGVNS;
        this.isForwardGVNS = true;
    }
    /** Indicates if the Forward Global Virtual Network Services optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isForwardGVNSPresent() {
        return isForwardGVNS;
    }
    /** Gets Backward Global Virtual Network Services.  */
    public BackwardGVNS getBackwardGVNS() throws ParameterNotSetException {
        if (isBackwardGVNS)
            return backwardGVNS;
        throw new ParameterNotSetException("Backward GVNS: not set.");
    }
    /** Sets Backward Global Virtual Network Services.  */
    public void setBackwardGVNS(BackwardGVNS backwardGVNS) {
        this.backwardGVNS = backwardGVNS;
        this.isBackwardGVNS = true;
    }
    /** Indicates if the Backward Global Virtual Network Services optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isBackwardGVNSPresent() {
        return isBackwardGVNS;
    }
    /** Gets Service Interaction Indicators Two.  */
    public ServiceInteractionIndicatorsTwo getServiceInteractionIndicatorsTwo() throws ParameterNotSetException {
        if (isServiceInteractionIndicatorsTwo)
            return serviceInteractionIndicatorsTwo;
        throw new ParameterNotSetException("Service Interaction Indicators Two: not set.");
    }
    /** Sets Service Interaction Indicators Two.  */
    public void setServiceInteractionIndicatorsTwo(ServiceInteractionIndicatorsTwo serviceInteractionIndicatorsTwo) {
        this.serviceInteractionIndicatorsTwo = serviceInteractionIndicatorsTwo;
        this.isServiceInteractionIndicatorsTwo = true;
    }
    /** Indicates if the Service Interaction Indicators Two optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isServiceInteractionIndicatorsTwoPresent() {
        return isServiceInteractionIndicatorsTwo;
    }
    /** Gets Correlation ID.  */
    public DigitsGenericDigits getCorrelationID() throws ParameterNotSetException {
        if (isCorrelationID)
            return correlationID;
        throw new ParameterNotSetException("Correlation Id: not set.");
    }
    /** Sets Correlation ID.  */
    public void setCorrelationID(DigitsGenericDigits correlationID) {
        this.correlationID = correlationID;
        this.isCorrelationID = true;
    }
    /** Indicates if the Correlation ID optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isCorrelationIDPresent() {
        return isCorrelationID;
    }
    /** Gets SCF ID.  */
    public ScfID getScfID() throws ParameterNotSetException {
        if (isScfID)
            return scfID;
        throw new ParameterNotSetException("SCF Id: not set.");
    }
    /** Sets SCF ID.  */
    public void setScfID(ScfID scfID) {
        this.scfID = scfID;
        this.isScfID = true;
    }
    /** Indicates if the SCF ID optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isScfIDPresent() {
        return isScfID;
    }
    /** Gets Call Segment ID.  */
    public int getCallSegmentID() throws ParameterNotSetException {
        if (isCallSegmentID)
            return callSegmentID;
        throw new ParameterNotSetException("Call Segment Id: not set.");
    }
    /** Sets Call Segment ID.  */
    public void setCallSegmentID(int callSegmentID) {
        this.callSegmentID = callSegmentID;
        this.isCallSegmentID = true;
    }
    /** Indicates if the Call Segment ID optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isCallSegmentIDPresent() {
        return isCallSegmentID;
    }
    /** Gets Leg To Be Created.  */
    public LegID getLegToBeCreated() throws ParameterNotSetException {
        if (isLegToBeCreated)
            return legToBeCreated;
        throw new ParameterNotSetException("Leg To Be Created: not set.");
    }
    /** Sets Leg To Be Created.  */
    public void setLegToBeCreated(LegID legToBeCreated) {
        this.legToBeCreated = legToBeCreated;
        this.isLegToBeCreated = true;
    }
    /** Indicates if the Leg To Be Created optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isLegToBeCreatedPresent() {
        return isLegToBeCreated;
    }
    /** Gets Extensions Parameter.  */
    public ExtensionField[] getExtensions() throws ParameterNotSetException {
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
    private java.lang.String alertingPattern;
    private boolean isAlertingPattern = false;
    private CalledPartyNumber destinationNumberRoutingAddress;
    private boolean isDestinationNumberRoutingAddress = false;
    private java.lang.String iSDNAccessRelatedInformation;
    private boolean isISDNAccessRelatedInformation = false;
    private FacilityGroup calledFacilityGroup;
    private boolean isCalledFacilityGroup= false;
    private int calledFacilityGroupMember;
    private boolean isCalledFacilityGroupMember = false;
    private OriginalCalledPartyID originalCalledPartyID;
    private boolean isOriginalCalledPartyID = false;
    private java.lang.String displayInformation;
    private boolean isDisplayInformation = false;
    private java.lang.String serviceInteractionIndicators;
    private boolean isServiceInteractionIndicators = false;
    private Entry iNServiceCompatibilityResponse;
    private boolean isINServiceCompatibilityResponse = false;
    private ForwardGVNS forwardGVNS;
    private boolean isForwardGVNS = false;
    private BackwardGVNS backwardGVNS;
    private boolean isBackwardGVNS = false;
    private ServiceInteractionIndicatorsTwo serviceInteractionIndicatorsTwo;
    private boolean isServiceInteractionIndicatorsTwo  = false;
    private DigitsGenericDigits correlationID;
    private boolean isCorrelationID = false;
    private ScfID scfID;
    private boolean isScfID = false;
    private int callSegmentID;
    private boolean isCallSegmentID = false;
    private LegID legToBeCreated;
    private boolean isLegToBeCreated = false;
    private ExtensionField extensions[];
    private boolean isExtensions = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
