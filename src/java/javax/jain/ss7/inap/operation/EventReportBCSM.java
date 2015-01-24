/*
 @(#) src/java/javax/jain/ss7/inap/operation/EventReportBCSM.java <p>
 
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
import javax.jain.ss7.inap.exception.*;
import javax.jain.ss7.inap.datatype.*;
import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** This class represents Event Report BCSM Operation.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class EventReportBCSM extends Operation implements java.io.Serializable {
    /** Constructor For EventReportBCSM.  */
    public EventReportBCSM(EventTypeBCSM eventTypeBCSM,MiscCallInfo miscCallInfo) {
        operationCode = OperationCode.EVENT_REPORT_BCSM;
        setEventTypeBCSM(eventTypeBCSM);
        setMiscCallInfo(miscCallInfo);
    }
    /** Gets Operation Code.  */
    public OperationCode getOperationCode() {
        return operationCode;
    }
    /** Gets Event Type BCSM.  This parameter gets the BCSM detection point event. <ul>
      * <li>ORIG_ATTEMPT_AUTHORIZED
      * <li>COLLECTED_INFO
      * <li>ANALYSED_INFORMATION
      * <li>ROUTE_SELECT_FAILURE
      * <li>O_CALLED_PARTY_BUSY
      * <li>O_NO_ANSWER
      * <li>O_ANSWER
      * <li>O_MID_CALL
      * <li>O_DISCONNECT
      * <li>O_ABANDON
      * <li>TERM_ATTEMPT_AUTHORIZED
      * <li>T_BUSY
      * <li>T_NO_ANSWER
      * <li>T_ANSWER
      * <li>T_MID_CALL
      * <li>T_DISCONNECT
      * <li>T_ABANDON
      * <li>O_TERM_SEIZED
      * <li>O_SUSPENDED
      * <li>T_SUSPENDED
      * <li>ORIG_ATTEMPT
      * <li>TERM_ATTEMPT
      * <li>O_RE_ANSWER
      * <li>T_RE_ANSWER
      * <li>FACILITY_SELECTED_AND_AVAILABLE
      * <li>CALL_ACCPETED </ul> */
    public EventTypeBCSM getEventTypeBCSM() {
        return eventTypeBCSM;
    }
    /** Sets Event Type BCSM.  */
    public void setEventTypeBCSM(EventTypeBCSM eventTypeBCSM) {
        this.eventTypeBCSM = eventTypeBCSM;
    }
    /** Gets BCSM Event Correlation ID.  */
    public DigitsGenericDigits getBcsmEventCorrelationID() throws ParameterNotSetException {
        if (isBcsmEventCorrelationID)
            return bcsmEventCorrelationID;
        throw new ParameterNotSetException("BCSM Event Correlation Id: not set.");
    }
    /** Sets BCSM Event Correlation ID.  */
    public void setBcsmEventCorrelationID(DigitsGenericDigits bcsmEventCorrelationID) {
        this.bcsmEventCorrelationID = bcsmEventCorrelationID;
        this.isBcsmEventCorrelationID = true;
    }
    /** Indicates if the BCSM Event Correlation ID optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isBcsmEventCorrelationIDPresent() {
        return isBcsmEventCorrelationID;
    }
    /** Gets Event Specific Information BCSM.  */
    public EventSpecificInformationBCSM getEventSpecificInformationBCSM() throws ParameterNotSetException {
        if (isEventSpecificInformationBCSM)
            return eventSpecificInformationBCSM;
        throw new ParameterNotSetException("Specific Information BCSM: not set.");
    }
    /** Sets Event Specific Information BCSM.  */
    public void setEventSpecificInformationBCSM(EventSpecificInformationBCSM eventSpecificInformationBCSM) throws IllegalArgumentException {
        this.eventSpecificInformationBCSM = eventSpecificInformationBCSM;
        this.isEventSpecificInformationBCSM = true;
    }
    /** Indicates if the Event Specific Information BCSM optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isEventSpecificInformationBCSMPresent() {
        return isEventSpecificInformationBCSM;
    }
    /** Gets Leg ID.  */
    public LegID getLegID() throws ParameterNotSetException {
        if (isLegID)
            return legID;
        throw new ParameterNotSetException("Leg Id: not set.");
    }
    /** Sets Leg ID.  */
    public void setLegID(LegID legID) {
        this.legID = legID;
        this.isLegID = true;
    }
    /** Indicates if the Leg ID optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isLegIDPresent() {
        return isLegID;
    }
    /** Gets Misc Call Info.  */
    public MiscCallInfo getMiscCallInfo() {
        return miscCallInfo;
    }
    /** Sets Misc Call Info.  */
    public void setMiscCallInfo(MiscCallInfo miscCallInfo) {
        this.miscCallInfo = miscCallInfo;
    }
    /** Gets ComponentType.  This parameter indicates the type of event that is reported to the SSF: <ul>
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
        this.componentType  = componentType;
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
    public void  setExtensions(ExtensionField extensions[]) {
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
    private EventTypeBCSM eventTypeBCSM;
    private DigitsGenericDigits bcsmEventCorrelationID;
    private boolean isBcsmEventCorrelationID;
    private EventSpecificInformationBCSM eventSpecificInformationBCSM;
    private boolean isEventSpecificInformationBCSM;
    private LegID legID;
    private boolean isLegID;
    private MiscCallInfo miscCallInfo;
    private ComponentType componentType;
    private boolean isComponentType;
    private int componentCorrelationID;
    private boolean isComponentCorrelationID;
    private ExtensionField extensions[];
    private boolean isExtensions;
    private Component component;
    private boolean isComponent;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
