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

/** This class represents the CallGap Operation.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class CallGap  extends Operation implements java.io.Serializable
{
    /** Constructor For CallGap.  */
    public CallGap(GapCriteria gapCriteria,GapIndicators gapIndicators) {
        operationCode = OperationCode.CALL_GAP;
        setGapCriteria(gapCriteria);
        setGapIndicators(gapIndicators);
    }
    /** Gets Operation Code.  */
    public OperationCode getOperationCode()
    {
        return operationCode;
    }
    /** Gets Gap Criteria.  */
    public GapCriteria getGapCriteria() {
        return gapCriteria;
    }
    /** Sets Gap Criteria.  */
    public void setGapCriteria(GapCriteria gapCriteria) {
        this.gapCriteria = gapCriteria;
    }
    /** Gets Gap Indicators.  */
    public GapIndicators getGapIndicators() {
        return gapIndicators;
    }
    /** Sets Gap Indicators.  */
    public void setGapIndicators(GapIndicators gapIndicators) {
        this.gapIndicators = gapIndicators;
    }
    /** Gets Control Type.  This parameter indicates indicates the reason
      * for activating call gapping. <ul>
      * <li>SCP_OVERLOAD - indicates that a control mechanism in the SCP has
      * detected a congestion situation.
      * <li>MANUALLY_INITIATED - indicates that the service and or
      * network/service management centre has detected a congestion
      * situation.
      * <li>DESTINATION_OVERLOAD - indicates that a control mechanism  has
      * detected a congestion situation at the destination. </ul> */
    public ControlType getControlType() throws ParameterNotSetException {
        if (isControlType)
            return controlType;
        throw new ParameterNotSetException("Control Type: not set.");
    }
    /** Sets Control Type.  */
    public void setControlType(ControlType controlType) {
        this.controlType =  controlType;
        this.isControlType = true;
    }
    /** Indicates if the Control Type optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isControlTypePresent() {
        return isControlType;
    }
    /** Gets Gap Treatment.  */
    public GapTreatment getTreatment() throws ParameterNotSetException {
        if (isGapTreatment)
            return gapTreatment;
        throw new ParameterNotSetException("Treatment: not set.");
    }
    /** Sets Gap Treatment.  */
    public void setGapTreatment(GapTreatment gapTreatment) {
        this.gapTreatment = gapTreatment;
        this.isGapTreatment = true;
    }
    /** Indicates if the Gap Treatment optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isGapTreatmentPresent() {
        return isGapTreatment;
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
    public void  setExtensions(ExtensionField extensions[]) {
        this.extensions = extensions;
        this.isExtensions = true;
    }
    /** Sets a particular Extensions Parameter.  */
    public void  setExtension(int index, ExtensionField extension) {
        this.extensions[index] = extension;
    }
    /** Indicates if the Extensions optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isExtensionsPresent() {
        return isExtensions;
    }
    private GapCriteria gapCriteria;
    private GapIndicators gapIndicators;
    private ControlType controlType;
    private boolean isControlType = false;
    private GapTreatment gapTreatment;
    private boolean isGapTreatment = false;
    private ExtensionField extensions[];
    private boolean isExtensions = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
