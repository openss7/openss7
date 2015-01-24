/*
 @(#) src/java/javax/jain/ss7/inap/datatype/RequestedInformationValue.java <p>
 
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
import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** This Class defines the RequestedInformationValue Datatype.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class RequestedInformationValue implements java.io.Serializable {
    /** Constructors For RequestedInformationValue.  */
    public RequestedInformationValue(int callElapsedTimeValue, ElapsedTimeValueID elapsedTimeValueID)
        throws IllegalArgumentException {
        if (elapsedTimeValueID == ElapsedTimeValueID.CALL_ATTEMPT_ELAPSED_TIME_VALUE)
            setCallAttemptElapsedTimeValue(callElapsedTimeValue);
        else if (elapsedTimeValueID == ElapsedTimeValueID.CALL_CONNECTED_ELAPSED_TIME_VALUE)
            setCallConnectedElapsedTimeValue(callElapsedTimeValue);
    }
    /** Constructors For RequestedInformationValue.  */
    public RequestedInformationValue(java.lang.String callStopTimeValue) {
        setCallStopTimeValue(callStopTimeValue);
    }
    /** Constructors For RequestedInformationValue.  */
    public RequestedInformationValue(DigitsGenericNumber calledAddressValue) {
        setCalledAddressValue(calledAddressValue);
    }
    /** Constructors For RequestedInformationValue.  */
    public RequestedInformationValue(Cause releaseCauseValue) {
        setReleaseCauseValue(releaseCauseValue);
    }
    /** Gets Call Attempt Elapsed Time Value.  */
    public int getCallAttemptElapsedTimeValue() {
        return callAttemptElapsedTimeValue;
    }
    /** Sets Call Attempt Elapsed Time Value.  */
    public void setCallAttemptElapsedTimeValue(int callAttemptElapsedTimeValue)
        throws IllegalArgumentException {
        if (0 <= callAttemptElapsedTimeValue && callAttemptElapsedTimeValue <= 255) {
            this.callAttemptElapsedTimeValue = callAttemptElapsedTimeValue;
            requestedInformationValueChoice = RequestedInformationValueChoice.CALL_ATTEMPT_ELAPSED_TIME_VALUE;
            return;
        }
        throw new IllegalArgumentException("Elapsed Time Value: " + callAttemptElapsedTimeValue + ", out of range.");
    }
    /** Gets Call Stop Time Value.  */
    public java.lang.String getCallStopTimeValue() {
        return callStopTimeValue;
    }
    /** Sets Call Stop Time Value.  */
    public void setCallStopTimeValue(java.lang.String callStopTimeValue) {
        this.callStopTimeValue = callStopTimeValue;
        requestedInformationValueChoice = RequestedInformationValueChoice.CALL_STOP_TIME_VALUE;
    }
    /** Gets Call Connected Elapsed Time Value. <ul>
      * <li>CALL_ATTEMPT_ELAPSED_TIME_VALUE - This parameter gets the
      * duration between the end of INAP processing of operations
      * initiating call setup ("Connect", "AnalyseInformation",
      * "CollectInformation", "Continue" and "SelectRoute") and the
      * received answer indication from the indicated or default called
      * party side.
      * <li>CALL_CONNECTED_ELAPSED_TIME_VALUE -This parameter gets the
      * duration between the received answer indication from the
      * indicated or default called party side and the release of that
      * connection. </ul> */
    public int getCallConnectedElapsedTimeValue() {
        return callConnectedElapsedTimeValue;
    }
    /** Sets Call Connected Elapsed Time Value.  */
    public void setCallConnectedElapsedTimeValue(int callConnectedElapsedTimeValue)
        throws IllegalArgumentException {
        if (0 <= callConnectedElapsedTimeValue && callConnectedElapsedTimeValue <= 2147483647 ) {
            this.callConnectedElapsedTimeValue = callConnectedElapsedTimeValue;
            requestedInformationValueChoice = RequestedInformationValueChoice.CALL_CONNECTED_ELAPSED_TIME_VALUE;
            return;
        }
        throw new IllegalArgumentException("Call Connected Elapsed Time Value: " + callConnectedElapsedTimeValue + ", out of range.");
    }
    /** Gets Called Address Value.  */
    public DigitsGenericNumber getCalledAddressValue() {
        return calledAddressValue;
    }
    /** Sets Called Address Value.  */
    public void setCalledAddressValue(DigitsGenericNumber calledAddressValue) {
        this.calledAddressValue = calledAddressValue;
        requestedInformationValueChoice = RequestedInformationValueChoice.CALLED_ADDRESS_VALUE;
    }
    /** Gets Release Cause Value.  */
    public Cause getReleaseCauseValue() {
        return releaseCauseValue;
    }
    /** Sets Release Cause Value.  */
    public void setReleaseCauseValue(Cause releaseCauseValue) {
        this.releaseCauseValue = releaseCauseValue;
        requestedInformationValueChoice = RequestedInformationValueChoice.RELEASE_CAUSE_VALUE;
    }
    /** Gets Requested Information Choice.  */
    public RequestedInformationValueChoice getRequestedInformationChoice() {
        return requestedInformationValueChoice;
    }
    private int callAttemptElapsedTimeValue;
    private java.lang.String callStopTimeValue;
    private int callConnectedElapsedTimeValue;
    private DigitsGenericNumber calledAddressValue;
    private Cause releaseCauseValue;
    private RequestedInformationValueChoice requestedInformationValueChoice;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
