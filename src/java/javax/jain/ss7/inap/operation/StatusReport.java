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

/** This class represents the StatusReport Operation.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class StatusReport extends Operation implements java.io.Serializable {
    /** Constructor For StatusReport.  */
    public StatusReport() {
        operationCode = OperationCode.STATUS_REPORT;
    }
    /** Gets Operation Code.  */
    public OperationCode getOperationCode() {
        return operationCode;
    }
    /** Gets Resource Status.  */
    public ResourceStatus getResourceStatus() throws ParameterNotSetException {
        if (isResourceStatus)
            return resourceStatus;
        throw new ParameterNotSetException("Resource Status: not set.");
    }
    /** Sets Resource Status.  */
    public void setResourceStatus(ResourceStatus resourceStatus) {
        this.resourceStatus = resourceStatus;
        this.isResourceStatus = true;
    }
    /** Indicates if the Resource Status parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isResourceStatusPresent() {
        return isResourceStatus;
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
    /** Gets Resource ID.  */
    public ResourceID getResourceID() throws ParameterNotSetException {
        if (isResourceID)
            return resourceID;
        throw new ParameterNotSetException("Resource Id: not set.");
    }
    /** Sets Resource ID.  */
    public void setResourceID(ResourceID resourceID) {
        this.resourceID = resourceID;
        this.isResourceID = true;
    }
    /** Indicates if the Resource ID optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isResourceIDPresent() {
        return isResourceID;
    }
    /** Gets Report Condition.  This parameter gets the reason of issuing
      * the 'Status Report' operation. <ul>
      * <li>STATUS_REPORT
      * <li>TIMER_EXPIRED
      * <li>CANCELLED </ul> */
    public ReportCondition getReportCondition() throws ParameterNotSetException {
        if (isReportCondition)
            return reportCondition;
        throw new ParameterNotSetException("Report Condition: not set.");
    }
    /** Sets Report Condition.  */
    public void setReportCondition(ReportCondition reportCondition) {
        this.reportCondition = reportCondition;
        this.isReportCondition = true;
    }
    /** Indicates if the Report Condition optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isReportConditionPresent() {
        return isReportCondition;
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
    private ResourceStatus resourceStatus;
    private boolean isResourceStatus = false;
    private DigitsGenericDigits correlationID;
    private boolean isCorrelationID = false;
    private ResourceID resourceID;
    private boolean isResourceID = false;
    private ReportCondition reportCondition;
    private boolean isReportCondition = false;
    private ExtensionField extensions[];
    private boolean isExtensions = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
