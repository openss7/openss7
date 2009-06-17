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

/** This class represents the EstablishTemporaryConnection Operation.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class EstablishTemporaryConnection extends Operation implements java.io.Serializable {
    /** Constructor For EstablishTemporaryConnection.  */
    public EstablishTemporaryConnection(DigitsGenericNumber assistingSSPIPRoutingAddress) {
        operationCode = OperationCode.ESTABLISH_TEMPORARY_CONNECTION;
        setAssistingSSPIPRoutingAddress(assistingSSPIPRoutingAddress);
    }
    /** Gets Operation Code.  */
    public OperationCode getOperationCode() {
        return operationCode;
    }
    /** Gets Assisting SSP IP Routing Address.  */
    public DigitsGenericNumber getAssistingSSPIPRoutingAddress() {
        return assistingSSPIPRoutingAddress;
    }
    /** Sets Assisting SSP IP Routing Address.  */
    public void setAssistingSSPIPRoutingAddress(DigitsGenericNumber assistingSSPIPRoutingAddress) {
        this.assistingSSPIPRoutingAddress = assistingSSPIPRoutingAddress;
    }
    /** Gets Correlation ID.  */
    public DigitsGenericNumber getCorrelationID() throws ParameterNotSetException {
        if (isCorrelationID)
             return correlationID;
        throw new ParameterNotSetException("Correlation Id: not set.");
    }
    /** Sets Correlation ID.  */
    public void setCorrelationID(DigitsGenericNumber correlationID) {
        this.correlationID = correlationID;
        this.isCorrelationID = true;
    }
    /** Indicates if the Correlation ID optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isCorrelationIDPresent() {
        return isCorrelationID;
    }
    /** Gets Party To Connect.  */
    public PartyToConnect getPartyToConnect() throws ParameterNotSetException {
        if (isPartyToConnect)
            return partyToConnect;
        throw new ParameterNotSetException("Party To Connect: not set.");
    }
    /** Sets Party To Connect.  */
    public void setPartyToConnect(PartyToConnect partyToConnect) {
        this.partyToConnect = partyToConnect;
        this.isPartyToConnect = true;
    }
    /** Indicates if the Party To Connect optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isPartyToConnectPresent() {
        return isPartyToConnect;
    }
    /** Gets Scf ID.  */
    public ScfID getScfID() throws ParameterNotSetException {
        if (isScfID)
            return scfID;
        throw new ParameterNotSetException("SCF Id: not set.");
    }
    /** Sets Scf ID.  */
    public void setScfID(ScfID scfID) {
        this.scfID = scfID;
        this.isScfID = true;
    }
    /** Indicates if the Scf ID optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isScfIDPresent() {
        return isScfID;
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
        this.isCarrier = true;
    }
    /** Indicates if the Carrier optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isCarrierPresent() {
        return isCarrier;
    }
    /** Gets Service Interaction Indicators (CS-1).  */
    public java.lang.String getServiceInteractionIndicators() throws ParameterNotSetException {
        if (isServiceInteractionIndicators)
            return serviceInteractionIndicators;
        throw new ParameterNotSetException("Service Interaction Indicators: not set.");
    }
    /** Sets Service Interaction Indicators (CS-1).  */
    public void setServiceInteractionIndicators(java.lang.String serviceInteractionIndicators) {
        this.serviceInteractionIndicators = serviceInteractionIndicators;
        this.isServiceInteractionIndicators = true;
    }
    /** Indicates if the Service Interaction Indicators (CS-1) optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isServiceInteractionIndicatorsPresent() {
        return isServiceInteractionIndicators;
    }
    /** Gets Service Interaction Indicators Two (CS-2).  */
    public ServiceInteractionIndicatorsTwo getServiceInteractionIndicatorsTwo() throws ParameterNotSetException {
        if (isServiceInteractionIndicatorsTwo)
            return serviceInteractionIndicatorsTwo;
        throw new ParameterNotSetException("Service Interaction Indicators 2: not set.");
    }
    /** Sets Service Interaction Indicators (CS-2).  */
    public void setServiceInteractionIndicators(ServiceInteractionIndicatorsTwo serviceInteractionIndicatorsTwo) {
        this.serviceInteractionIndicatorsTwo = serviceInteractionIndicatorsTwo;
        this.isServiceInteractionIndicatorsTwo = true;
    }
    /** Indicates if the Service Interaction Indicators Two (CS-2) optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isServiceInteractionIndicatorsTwoPresent() {
        return isServiceInteractionIndicatorsTwo;
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
    private DigitsGenericNumber assistingSSPIPRoutingAddress;
    private DigitsGenericNumber correlationID;
    private boolean isCorrelationID = false;
    private PartyToConnect partyToConnect;
    private boolean isPartyToConnect = false;
    private ScfID scfID;
    private boolean isScfID = false;
    private Carrier carrier;
    private boolean isCarrier = false;
    private java.lang.String serviceInteractionIndicators;
    private boolean isServiceInteractionIndicators = false;
    private ServiceInteractionIndicatorsTwo serviceInteractionIndicatorsTwo;
    private boolean isServiceInteractionIndicatorsTwo = false;
    private ExtensionField extensions[];
    private boolean isExtensions = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
