/*
 @(#) src/java/javax/jain/ss7/inap/operation/AuthorizeTermination.java <p>
 
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

/** This class represents the AuthorizeTermination Operation.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class AuthorizeTermination extends Operation implements java.io.Serializable {
    /** Constructor For AuthorizeTermination.  */
    public AuthorizeTermination() {
        operationCode = OperationCode.AUTHORIZE_TERMINATION;
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
    /** Indicates if the Alerting Pattern optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isAlertingPatternPresent() {
        return isAlertingPattern;
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
    /** Gets Destination Number Routing Address.  */
    public CalledPartyNumber getDestinationNumberRoutingAddress() throws ParameterNotSetException {
        if (isDestinationNumberRoutingAddress)
            return destinationNumberRoutingAddress;
        throw new ParameterNotSetException("Destination Number Routing Address: not set.");
    }
    /** Sets Destination Number Routing Address.  */
    public void setDestinationNumberRoutingAddress(CalledPartyNumber destinationNumberRoutingAddress) {
        this.destinationNumberRoutingAddress = destinationNumberRoutingAddress;
        this.isDestinationNumberRoutingAddress = true;
    }
    /** Indicates if the Destination Number Routing Address optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isDestinationNumberRoutingAddressPresent() {
        return isDestinationNumberRoutingAddress;
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
    /** Gets Travelling Class Mark.  */
    public LocationNumber getTravellingClassMark() throws ParameterNotSetException {
        if (isTravellingClassMark)
            return travellingClassMark;
        throw new ParameterNotSetException("Travelling Class Mark: not set.");
    }
    /** Sets Travelling Class Mark.  */
    public void setTravellingClassMark(LocationNumber travellingClassMark) {
        this.travellingClassMark = travellingClassMark;
        this.isTravellingClassMark = true;
    }
    /** Indicates if the Travelling Class Mark optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isTravellingClassMarkPresent() {
        return isTravellingClassMark;
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
    /** Gets a particular  Extension Parameter.  */
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
    private CallingPartyNumber callingPartyNumber;
    private boolean isCallingPartyNumber = false;
    private CalledPartyNumber destinationNumberRoutingAddress;
    private boolean isDestinationNumberRoutingAddress = false;
    private java.lang.String displayInformation;
    private boolean isDisplayInformation = false;
    private java.lang.String iSDNAccessRelatedInformation;
    private boolean isISDNAccessRelatedInformation = false;
    private OriginalCalledPartyID originalCalledPartyID;
    private boolean isOriginalCalledPartyID = false;
    private LocationNumber travellingClassMark;
    private boolean isTravellingClassMark = false;
    private Entry iNServiceCompatibilityResponse;
    private boolean isINServiceCompatibilityResponse = false;
    private ForwardGVNS forwardGVNS;
    private boolean isForwardGVNS = false;
    private BackwardGVNS backwardGVNS;
    private boolean isBackwardGVNS = false;
    private LegID legToBeCreated;
    private boolean isLegToBeCreated = false;
    private ExtensionField extensions[];
    private boolean isExtensions = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
