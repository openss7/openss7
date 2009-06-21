/*
 @(#) $RCSfile: OMidCall.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:00 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:00 $ by $Author: brian $
 */

package javax.jain.ss7.inap.operation;

import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.datatype.*;
import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** This class represents OMidCall Operation.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class OMidCall  extends Operation   implements java.io.Serializable
{
    /** Constructor For OMidCall.  */
    public OMidCall(DPSpecificCommonParameters dPSpecificCommonParameters) {
        operationCode = OperationCode.O_MID_CALL;
        setDPSpecificCommonParameters(dPSpecificCommonParameters);
    }
    /** Gets Operation Code.  */
    public OperationCode getOperationCode() {
        return operationCode;
    }
    /** Gets DP Specific Common Parameter.  */
    public DPSpecificCommonParameters getDPSpecificCommonParameters() {
        return dPSpecificCommonParameters;
    }
    /** Sets DP Specific Common Parameter.  */
    public void setDPSpecificCommonParameters(DPSpecificCommonParameters dPSpecificCommonParameters) {
        this.dPSpecificCommonParameters = dPSpecificCommonParameters;
    }
    /** Gets Called Party Business Group ID.  */
    public java.lang.String getCalledPartyBusinessGroupID() throws ParameterNotSetException {
        if (isCalledPartyBusinessGroupID)
            return calledPartyBusinessGroupID;
        throw new ParameterNotSetException("Called Party Business Group Id: not set.");
    }
    /** Sets Called Party Business Group ID.  */
    public void setCalledPartyBusinessGroupID(java.lang.String calledPartyBusinessGroupID) {
        this.calledPartyBusinessGroupID = calledPartyBusinessGroupID;
        this.isCalledPartyBusinessGroupID = true;
    }
    /** Indicates if the Called Party Business Group ID optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isCalledPartyBusinessGroupIDPresent() {
        return isCalledPartyBusinessGroupID;
    }
    /** Gets Called Party Sub-Address.  */
    public CalledPartySubaddress getCalledPartySubaddress() throws ParameterNotSetException {
        if (isCalledPartySubaddress)
            return calledPartySubaddress;
        throw new ParameterNotSetException("Called Party Subaddress: not set.");
    }
    /** Sets Called Party Sub-Address.  */
    public void setCalledPartySubaddress(CalledPartySubaddress calledPartySubaddress) {
        this.calledPartySubaddress = calledPartySubaddress;
        this.isCalledPartySubaddress = true;
    }
    /** Indicates if the Called Party Sub-Address optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isCalledPartySubaddressPresent() {
        return isCalledPartySubaddress;
    }
    /** Gets Calling Party Business Group ID.  */
    public java.lang.String getCallingPartyBusinessGroupID() throws ParameterNotSetException {
        if (isCallingPartyBusinessGroupID)
            return callingPartyBusinessGroupID;
        throw new ParameterNotSetException("Calling Party Business Group ID: not set.");
    }
    /** Sets Calling Party Business Group ID.  */
    public void setCallingPartyBusinessGroupID(java.lang.String callingPartyBusinessGroupID) {
        this.callingPartyBusinessGroupID = callingPartyBusinessGroupID;
        this.isCallingPartyBusinessGroupID = true;
    }
    /** Indicates if the Calling Party Business Group ID optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isCallingPartyBusinessGroupIDPresent() {
        return isCallingPartyBusinessGroupID;
    }
    /** Gets Calling Party Sub-Address.  */
    public CallingPartySubaddress getCallingPartySubaddress() throws ParameterNotSetException {
        if (isCallingPartySubaddress)
            return callingPartySubaddress;
        throw new ParameterNotSetException("Calling Party Sub-Address: not set.");
    }
    /** Sets Calling Party Sub-Address.  */
    public void setCallingPartySubaddress(CallingPartySubaddress callingPartySubaddress) {
        this.callingPartySubaddress = callingPartySubaddress;
        this.isCallingPartySubaddress = true;
    }
    /** Indicates if the Calling Party Sub-Address optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isCallingPartySubaddressPresent() {
        return isCallingPartySubaddress;
    }
    /** Gets Feature Request Indicator.  This parameter gets the feature
      * activated (e.g. a switch-hook flash, feature activation).  Spare
      * values are reserved for future use. <ul>
      * <li>HOLD
      * <li>RETRIEVE
      * <li>FEATURE_ACTIVATION
      * <li>SPARE_1
      * <li>SPARE_N </ul> */
    public FeatureRequestIndicator getFeatureRequestIndicator() throws ParameterNotSetException {
        if (isFeatureRequestIndicator)
            return featureRequestIndicator;
        throw new ParameterNotSetException("Feature Request Indicator: not set.");
    }
    /** Sets Feature Request Indicator.  */
    public void setFeatureRequestIndicator(FeatureRequestIndicator featureRequestIndicator) {
        this.featureRequestIndicator = featureRequestIndicator;
        this.isFeatureRequestIndicator = true;
    }
    /** Indicates if the Feature Request Indicator optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isFeatureRequestIndicatorPresent() {
        return isFeatureRequestIndicator;
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
    /** Indicates if the Component Type optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isComponentTypePresent() {
        return isComponentType;
    }
    /** Gets Component Correlation ID.  */
    public int getComponentCorrelationID() throws ParameterNotSetException {
        if (isComponentCorrelationID)
            return componentCorrelationID;
        throw new ParameterNotSetException("Component Correlation Id: not set.");
    }
    /** Sets Component Correlation ID.  */
    public void setComponentCorrelationID(int componentCorrelationID) {
        this.componentCorrelationID = componentCorrelationID;
        this.isComponentCorrelationID = true;
    }
    /** Indicates if the Component Correlation ID optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isComponentCorrelationIDPresent() {
        return isComponentCorrelationID;
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
    /** Gets Component.  */
    public Component getComponent() throws ParameterNotSetException {
        if (isComponent)
            return component;
        throw new ParameterNotSetException("Component: not set.");
    }
    /** Sets Component.  */
    public void setComponent(Component component) {
        this.component = component;
        this.isComponent = true;
    }
    /** Indicates if the Component optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isComponentPresent() {
        return isComponent;
    }
    private DPSpecificCommonParameters dPSpecificCommonParameters;
    private java.lang.String calledPartyBusinessGroupID;
    private boolean isCalledPartyBusinessGroupID = false;
    private CalledPartySubaddress calledPartySubaddress;
    private boolean isCalledPartySubaddress = false;
    private java.lang.String callingPartyBusinessGroupID;
    private boolean isCallingPartyBusinessGroupID =false;
    private CallingPartySubaddress callingPartySubaddress;
    private boolean isCallingPartySubaddress = false;
    private FeatureRequestIndicator featureRequestIndicator;
    private boolean isFeatureRequestIndicator = false;
    private Carrier carrier;
    private boolean isCarrier =false;
    private ComponentType componentType;
    private boolean isComponentType = false;
    private int componentCorrelationID;
    private boolean isComponentCorrelationID = false;
    private ExtensionField extensions[];
    private boolean isExtensions = false;
    private Component component;
    private boolean isComponent= false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
