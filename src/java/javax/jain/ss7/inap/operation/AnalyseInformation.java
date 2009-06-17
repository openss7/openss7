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

/** This class represents the AnalyseInformation Operation.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class AnalyseInformation extends Operation implements java.io.Serializable {
    /** Constructor For AnalyseInformation.  */
    public AnalyseInformation(DestinationRoutingAddress destinationRoutingAddress) {
        operationCode = OperationCode.ANALYSE_INFORMATION;
        setDestinationRoutingAddress(destinationRoutingAddress);
    }
    /** Gets Operation Code.  */
    public OperationCode getOperationCode() {
        return operationCode;
    }
    /** Gets Destination Routing Address.  */
    public DestinationRoutingAddress getDestinationRoutingAddress() {
        return destinationRoutingAddress;
    }
    /** Sets Destination Routing Address.  */
    public void setDestinationRoutingAddress(DestinationRoutingAddress destinationRoutingAddress) {
        this.destinationRoutingAddress = destinationRoutingAddress;
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
        isAlertingPattern = true;
    }
    /** Indicates if the Alerting Pattern optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isAlertingPatternPresent() {
        return isAlertingPattern;
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
        isISDNAccessRelatedInformation = true;
    }
    /** Indicates if the ISDN Access Related Information optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isISDNAccessRelatedInformationPresent() {
        return isISDNAccessRelatedInformation;
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
        isOriginalCalledPartyID = true;
    }
    /** Indicates if the Original Called Party ID optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isOriginalCalledPartyIDPresent() {
        return isOriginalCalledPartyID;
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
        isCallingPartyNumber = true;
    }
    /** Indicates if the Calling Party Number optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isCallingPartyNumberPresent() {
        return isCallingPartyNumber;
    }
    /** Gets Calling Party Category.  */
    public java.lang.String getCallingPartysCategory() throws ParameterNotSetException {
        if (isCallingPartysCategory)
            return callingPartysCategory;
        throw new ParameterNotSetException("Calling Party Catergory: not set.");
    }
    /** Sets Calling Party Category.  */
    public void setCallingPartysCategory(java.lang.String callingPartysCategory) {
        this.callingPartysCategory = callingPartysCategory;
        isCallingPartysCategory = true;
    }
    /** Indicates if the Calling Party Category optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isCallingPartysCategoryPresent() {
        return isCallingPartysCategory;
    }
    /** Gets Called Party Number.  */
    public CalledPartyNumber getCalledPartyNumber() throws ParameterNotSetException {
        if (isCalledPartyNumber)
            return calledPartyNumber;
        throw new ParameterNotSetException("Called Party Number: not set.");
    }
    /** Sets Called Party Number.  */
    public void setCalledPartyNumber(CalledPartyNumber calledpartynumber) {
        this.calledPartyNumber = calledpartynumber;
        isCalledPartyNumber = true;
    }
    /** Indicates if the Called Party Number optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isCalledPartyNumberPresent() {
        return isCalledPartyNumber;
    }
    /** Gets Charge Number.  */
    public LocationNumber getChargeNumber() throws ParameterNotSetException {
        if (isChargeNumber)
            return chargeNumber;
        throw new ParameterNotSetException("Charge Number: not set.");
    }
    /** Sets Charge Number.  */
    public void setChargeNumber(LocationNumber chargeNumber) {
        this.chargeNumber = chargeNumber;
        isChargeNumber = true;
    }
    /** Indicates if the Charge Number optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isChargeNumberPresent() {
        return isChargeNumber;
    }
    /** Gets Travelling Class Mark.  */
    public LocationNumber getTravellingClassMark() throws ParameterNotSetException {
        if (isTravellingClassMark)
            return travellingClassMark;
        throw new ParameterNotSetException("Travelling Class Mark: not set.");
    }
    /** Sets Travelling Class Mark.  */
    public void setTravellingClassMark(LocationNumber travellingClassMark) {
        this.travellingClassMark = travellingClassMark;
        isTravellingClassMark = true;
    }
    /** Indicates if the Travelling Class Mark optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isTravellingClassMarkPresent() {
        return isTravellingClassMark;
    }
    /** Gets Carrier.  */
    public Carrier getCarrier() throws ParameterNotSetException {
        if (isCarrier)
            return carrier;
        throw new ParameterNotSetException("Carrier: not set.");
    }
    /** Sets Carrier.  */
    public void setCarrier(Carrier carrier) {
        this.carrier = carrier;
        isCarrier = true;
    }
    /** Indicates if the Carrier optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isCarrierPresent() {
        return isCarrier;
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
        isServiceInteractionIndicators = true;
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
        if(isForwardGVNS)
            return forwardGVNS;
        throw new ParameterNotSetException("Forward GVNS: not set.");
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
        throw new ParameterNotSetException("Service Interaction Indicators 2: not set.");
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
    /** Gets CorrelationID.  */
    public DigitsGenericDigits getCorrelationID() throws ParameterNotSetException {
        if (isCorrelationID)
            return correlationID;
        throw new ParameterNotSetException("Correlation Id: not set.");
    }
    /** Sets CorrelationID.  */
    public void setCorrelationID(DigitsGenericDigits correlationID) {
        this.correlationID = correlationID;
        this.isCorrelationID = true;
    }
    /** Indicates if the CorrelationID optional parameter is present.
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
    private DestinationRoutingAddress destinationRoutingAddress;
    private java.lang.String alertingPattern;
    private boolean isAlertingPattern = false;
    private java.lang.String iSDNAccessRelatedInformation;
    private boolean isISDNAccessRelatedInformation = false;
    private OriginalCalledPartyID originalCalledPartyID;
    private boolean isOriginalCalledPartyID = false;
    private CallingPartyNumber callingPartyNumber;
    private boolean isCallingPartyNumber = false;
    private java.lang.String callingPartysCategory;
    private boolean isCallingPartysCategory = false;
    private CalledPartyNumber calledPartyNumber;
    private boolean isCalledPartyNumber = false;
    private LocationNumber chargeNumber;
    private boolean isChargeNumber = false;
    private LocationNumber travellingClassMark;
    private boolean isTravellingClassMark = false;
    private Carrier carrier;
    private boolean isCarrier = false;
    private java.lang.String serviceInteractionIndicators;
    private boolean isServiceInteractionIndicators = false;
    private Entry iNServiceCompatibilityResponse;
    private boolean isINServiceCompatibilityResponse = false;
    private ForwardGVNS forwardGVNS;
    private boolean isForwardGVNS = false;
    private BackwardGVNS backwardGVNS;
    private boolean isBackwardGVNS = false;
    private ServiceInteractionIndicatorsTwo serviceInteractionIndicatorsTwo;
    private boolean isServiceInteractionIndicatorsTwo = false;
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
