/*
 @(#) src/java/javax/jain/ss7/inap/datatype/ServiceInteractionIndicatorsTwo.java <p>
 
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

package javax.jain.ss7.inap.datatype;

import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.exception.*;
import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** This Class defines the ServiceInteractionIndicatorsTwo Datatype.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class ServiceInteractionIndicatorsTwo implements java.io.Serializable {
    /** Gets Forward Service Interaction Indicator.  */
    public ForwardServiceInteractionInd getForwardServiceInteractionInd()
        throws ParameterNotSetException {
        if (isForwardServiceInteractionInd)
            return forwardServiceInteractionInd;
        throw new ParameterNotSetException("Forward Service Interaction Ind: not set.");
    }
    /** Sets Forward Service Interaction Indicator.  */
    public void setForwardServiceInteractionInd(ForwardServiceInteractionInd forwardServiceInteractionInd) {
        this.forwardServiceInteractionInd = forwardServiceInteractionInd;
        isForwardServiceInteractionInd = true;
    }
    /** Indicates if the Forward Service Interaction Indicator optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isForwardServiceInteractionIndPresent() {
        return isForwardServiceInteractionInd;
    }
    /** Gets Backward Service Interaction Indicator.  */
    public BackwardServiceInteractionInd getBackwardServiceInteractionInd()
        throws ParameterNotSetException {
        if (isBackwardServiceInteractionInd)
            return backwardServiceInteractionInd;
        throw new ParameterNotSetException("Backward Service Interaction Ind: not set.");
    }
    /** Sets Backward Service Interaction Indicator.  */
    public void setBackwardServiceInteractionInd(BackwardServiceInteractionInd backwardServiceInteractionInd) {
        this.backwardServiceInteractionInd = backwardServiceInteractionInd;
        isBackwardServiceInteractionInd = true;
    }
    /** Indicates if the Backward Service Interaction Indicator optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isBackwardServiceInteractionIndPresent() {
        return isBackwardServiceInteractionInd;
    }
    /** Gets Bothway Through Connection Indicator.  */
    public int getBothwayThroughConnectionInd()
        throws ParameterNotSetException {
        if (isBothwayThroughConnectionInd)
            return bothwayThroughConnectionInd;
        throw new ParameterNotSetException("Bothway Through Connection Ind: not set.");
    }
    /** Sets Bothway Through Connection Indicator.  */
    public void setBothwayThroughConnectionInd(int bothwayThroughConnectionInd) {
        this.bothwayThroughConnectionInd = bothwayThroughConnectionInd;
        isBothwayThroughConnectionInd = true;
    }
    /** Indicates if the Bothway Through Connection Indicator optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isBothwayThroughConnectionIndPresent() {
        return isBothwayThroughConnectionInd;
    }
    /** Gets Suspend Timer.  */
    public int getSuspendTimer()
        throws ParameterNotSetException {
        if (isSuspendTimer)
            return suspendTimer;
        throw new ParameterNotSetException("Suspend Timer: not set.");
    }
    /** Sets Suspend Timer.  */
    public void setSuspendTimer(int suspendTimer)
        throws IllegalArgumentException {
        if (0 <= suspendTimer && suspendTimer <= 120) {
            this.suspendTimer = suspendTimer;
            isSuspendTimer = true;
            return;
         }
        throw new IllegalArgumentException("Suspend Timer: " + suspendTimer + ", out of range.");
    }
    /** Indicates if the Suspend Timer optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isSuspendTimerPresent() {
        return isSuspendTimer;
    }
    /** Gets Connected Number Treatment Indicator.  */
    public int getConnectedNumberTreatmentInd()
        throws ParameterNotSetException {
        if (isConnectedNumberTreatmentInd)
            return connectedNumberTreatmentInd;
        throw new ParameterNotSetException("Connected Number Treatment Ind: not set.");
    }
    /** Sets Connected Number Treatment Indicator.  */
    public void setConnectedNumberTreatmentInd(int connectedNumberTreatmentInd) {
        this.connectedNumberTreatmentInd = connectedNumberTreatmentInd;
        isConnectedNumberTreatmentInd = true;
    }
    /** Indicates if the Connected Number Treatment Indicator optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isConnectedNumberTreatmentIndPresent() {
        return isConnectedNumberTreatmentInd;
    }
    /** Gets Suppress Call Diversion Notification.  */
    public boolean getSuppressCallDiversionNotification()
        throws ParameterNotSetException {
        if (isSuppressCallDiversionNotification)
            return suppressCallDiversionNotification;
        throw new ParameterNotSetException("Suppress Call Diversion Notification: not set.");
    }
    /** Sets Suppress Call Diversion Notification.  */
    public void setSuppressCallDiversionNotification(boolean suppressCallDiversionNotification) {
        this.suppressCallDiversionNotification = suppressCallDiversionNotification;
        isSuppressCallDiversionNotification = true;
    }
    /** Indicates if the Suppress Call Diversion Notification optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isSuppressCallDiversionNotificationPresent() {
        return isSuppressCallDiversionNotification;
    }
    /** Gets Suppress Call Transfer Notification.  */
    public boolean getSuppressCallTransferNotification()
        throws ParameterNotSetException {
        if (isSuppressCallTransferNotification)
            return suppressCallTransferNotification;
        throw new ParameterNotSetException("Suppress Call Transfer Notification: not set.");
    }
    /** Sets Suppress Call Transfer Notification.  */
    public void setSuppressCallTransferNotification(boolean suppressCallTransferNotification) {
        this.suppressCallTransferNotification = suppressCallTransferNotification;
        isSuppressCallTransferNotification = true;
    }
    /** Indicates if the Suppress Call Transfer Notification optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isSuppressCallTransferNotificationPresent() {
        return isSuppressCallTransferNotification;
    }
    /** Gets Allowed Called IN Number Presentation Indicator.  */
    public boolean getAllowCdINNoPresentationInd()
        throws ParameterNotSetException {
        if (isAllowCdINNoPresentationInd)
            return allowCdINNoPresentationInd;
        throw new ParameterNotSetException("Allow Cd IN No Presentation Ind: not set.");
    }
    /** Sets Allowed Called IN Number Presentation Indicator.  */
    public void setAllowCdINNoPresentationInd(boolean allowCdINNoPresentationInd) {
        this.allowCdINNoPresentationInd = allowCdINNoPresentationInd;
        isAllowCdINNoPresentationInd = true;
    }
    /** Indicates if the Allowed Called IN Number Presentation Indicator optional parameter is present.
      * @return True when present, false otherwise.  */
    public boolean isAllowCdINNoPresentationIndPresent() {
        return isAllowCdINNoPresentationInd;
    }
    /** Gets User Dialogue Duration Indicator.  */
    public boolean getUserDialogueDurationInd() {
        return userDialogueDurationInd;
     }
    /** Sets User Dialogue Duration Indicator.  */
    public void setUserDialogueDurationInd(boolean userDialogueDurationInd) {
        this.userDialogueDurationInd = userDialogueDurationInd;
    }
    private ForwardServiceInteractionInd forwardServiceInteractionInd;
    private boolean isForwardServiceInteractionInd = false;
    private BackwardServiceInteractionInd backwardServiceInteractionInd;
    private boolean isBackwardServiceInteractionInd = false;
    private int bothwayThroughConnectionInd;
    private boolean isBothwayThroughConnectionInd = false;
    private int suspendTimer;
    private boolean isSuspendTimer = false;
    private int connectedNumberTreatmentInd;
    private boolean isConnectedNumberTreatmentInd = false;
    private boolean suppressCallDiversionNotification;
    private boolean isSuppressCallDiversionNotification = false;
    private boolean suppressCallTransferNotification;
    private boolean isSuppressCallTransferNotification = false;
    private boolean allowCdINNoPresentationInd;
    private boolean isAllowCdINNoPresentationInd = false;
    private boolean userDialogueDurationInd = true;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
