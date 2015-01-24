/*
 @(#) src/java/javax/jain/ss7/inap/datatype/CollectedDigits.java <p>
 
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

/** This class defines the CollectedDigits Datatype.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class CollectedDigits implements java.io.Serializable {
    /** Constructor For CollectedDigits.  */
    public CollectedDigits(int minimumNbOfDigits,int maximumNbOfDigits)
        throws IllegalArgumentException {
        setMinimumNbOfDigits(minimumNbOfDigits);
        setMaximumNbOfDigits(maximumNbOfDigits);
    }
    /** Gets Minimum Number Of Digits.  */
    public int getMinimumNbOfDigits() {
        return minimumNbOfDigits;
    }
    /** Sets Minimum Number Of Digits.  */
    public void setMinimumNbOfDigits(int minimumNbOfDigits)
        throws IllegalArgumentException {
        if( 1 <= minimumNbOfDigits && minimumNbOfDigits <= 127) {
            this.minimumNbOfDigits = minimumNbOfDigits;
            return;
        }
        throw new IllegalArgumentException("Minimum Number of Digits: " + minimumNbOfDigits + ", out of range.");
    }
    /** Gets Maximum Number Of Digits.  */
    public int getMaximumNbOfDigits() {
        return maximumNbOfDigits;
    }
    /** Sets Maximum Number Of Digits.  */
    public void setMaximumNbOfDigits(int maximumNbOfDigits)
        throws IllegalArgumentException { 
        if (1 <= maximumNbOfDigits && maximumNbOfDigits <= 127) {
            this.maximumNbOfDigits = maximumNbOfDigits;
            return;
        }
        throw new IllegalArgumentException("Maximum Number of Digits: " + maximumNbOfDigits + ", out of range");
    }
    /** Gets End Of Reply Digit.  */
    public java.lang.String getEndOfReplyDigit() throws ParameterNotSetException {
        if (isEndOfReplyDigit)
            return endOfReplyDigit;
        throw new ParameterNotSetException("End of Reply Digit: not set.");
    }
    /** Sets End Of Reply Digit.  */
    public void setEndOfReplyDigit(java.lang.String endOfReplyDigit) {
        this.endOfReplyDigit = endOfReplyDigit;
        this.isEndOfReplyDigit = true;
    }
    /** Indicates if the End Of Reply Digit optional parameter is present.
      * @return
      * True if present, false otherwise.  */
    public boolean isEndOfReplyDigitPresent() {
        return isEndOfReplyDigit;
    }
    /** Gets Cancel Digit.  */
    public java.lang.String getCancelDigit()
        throws ParameterNotSetException {
        if (isCancelDigit)
            return cancelDigit;
        throw new ParameterNotSetException("Cancel Digit: not set.");
    }
    /** Sets Cancel Digit.  */
    public void setCancelDigit(java.lang.String cancelDigit) {
        this.cancelDigit = cancelDigit;
        this.isCancelDigit = true;
    }
    /** Indicates if the Cancel Digit optional parameter is present.
      * @return
      * True if present, false otherwise.  */
    public boolean isCancelDigitPresent() {
        return isCancelDigit;
    }
    /** Gets Start Digit.  */
    public java.lang.String getStartDigit()
        throws ParameterNotSetException {
        if (isStartDigit)
            return startDigit;
        throw new ParameterNotSetException("Start Digit: not set.");
    }
    /** Sets Start Digit.  */
    public void setStartDigit(java.lang.String startDigit) {
        this.startDigit = startDigit;
        this.isStartDigit = true;      
    }
    /** Indicates if the Start Digit optional parameter is present.
      * @return
      * True if present, false otherwise.  */
    public boolean isStartDigitPresent() {
        return isStartDigit;
    }
    /** Gets First Digit Time Out.  */
    public int getFirstDigitTimeOut()
        throws ParameterNotSetException {
        if (isFirstDigitTimeOut)
            return firstDigitTimeOut;
        throw new ParameterNotSetException("First Digit: not set.");
    }
    /** Sets First Digit Time Out.  */
    public void setFirstDigitTimeOut(int firstDigitTimeOut)
        throws IllegalArgumentException {
        if(1 <= firstDigitTimeOut && firstDigitTimeOut <= 127) {
            this.firstDigitTimeOut = firstDigitTimeOut;
            this.isFirstDigitTimeOut = true;
            return;
        }
        throw new IllegalArgumentException("First Digit Timeout: " + firstDigitTimeOut + ", out of range.");
    }
    /** Indicates if the First Digit Time Out optional parameter is present.
      * @return
      * True if present, false otherwise.  */
    public boolean isFirstDigitTimeOutPresent() {
        return isFirstDigitTimeOut;
    }
    /** Gets Inter Digit Time Out.  */
    public int getInterDigitTimeOut() throws ParameterNotSetException {
        if (isInterDigitTimeOut)
            return interDigitTimeOut;
        throw new ParameterNotSetException("Inter Digit Timeout: not set.");
    }
    /** Sets Inter Digit Time Out.  */
    public void setInterDigitTimeOut(int interDigitTimeOut)
        throws IllegalArgumentException {
        if (1 <= interDigitTimeOut && interDigitTimeOut <= 127) {
            this.interDigitTimeOut = interDigitTimeOut;
            this.isInterDigitTimeOut = true;
            return;
        }
        throw new IllegalArgumentException("Inter Digit Timeout: " + interDigitTimeOut + ", out of range");
    }
    /** Indicates if the Inter  Digit Time Out optional parameter is present.
      * @return
      * True if present, false otherwise.  */
    public boolean isInterDigitTimeOutPresent() {
        return isInterDigitTimeOut;
    }
    /** Gets Error Treatment.
      * This parameter gets what specific action has been taken by the SRF
      * in the event of error conditions occurring.  <ul>
      * <li>REPORT_ERROR_TO_SCF
      * <li>HELP        
      * <li>REPEAT_PROMPT </ul> */
    public ErrorTreatment getErrorTreatment()
        throws ParameterNotSetException {
        if (isErrorTreatment)
            return errorTreatment;
        throw new ParameterNotSetException("Error Treatment: not set.");
    }
    /** Sets Error Treatment.  */
    public void setErrorTreatment(ErrorTreatment errorTreatment) {
        this.errorTreatment = errorTreatment;
        this.isErrorTreatment = true;
    }
    /** Indicates if the Error Treatment optional parameter is present.
      * @return
      * True if present, false otherwise.  */
    public boolean isErrorTreatmentPresent() {
        return isErrorTreatment;
    }
    /** Gets Interruptable Ann Ind.  */
    public boolean getInterruptableAnnInd()
        throws ParameterNotSetException {
        if (isInterruptableAnnInd)
            return interruptableAnnInd;
        throw new ParameterNotSetException("Interruptable Announcment Indicator: not set.");
    }
    /** Sets Interruptable Ann Ind.  */
    public void setInterruptableAnnInd(boolean interruptableAnnInd) {
        this.interruptableAnnInd = interruptableAnnInd;
        this.isInterruptableAnnInd = true;
    }
    /** Indicates if the Interruptable Ann Ind  optional parameter is present.
      * @return
      * True if present, false otherwise.  */
    public boolean isInterruptableAnnIndPresent() {
        return isInterruptableAnnInd;
    }
    /** Gets Voice Information.  */
    public boolean getVoiceInformation()
        throws ParameterNotSetException {
        if (isVoiceInformation)
            return voiceInformation;
        throw new ParameterNotSetException("Voice Information: not set.");
    }
    /** Sets Voice Information.  */
    public void setVoiceInformation(boolean voiceInformation) {
        this.voiceInformation = voiceInformation;
        this.isVoiceInformation = true;
    }
    /** Indicates if the Voice Information optional parameter is present.
      * @return
      * True if present, false otherwise.  */
    public boolean isVoiceInformationPresent() {
        return isVoiceInformation;
    }
    /** Gets Voice Back.  */
    public boolean getVoiceBack()
        throws ParameterNotSetException {
        if (isVoiceBack)
            return voiceBack;
        throw new ParameterNotSetException("Voice Back: not set.");
    }
    /** Sets Voice Back.  */
    public void setVoiceBack(boolean voiceBack) {
        this.voiceBack = voiceBack;
        this.isVoiceBack = true; 
    }
    /** Indicates if the Voice Back optional parameter is present.
      * @return
      * True if present, false otherwise.  */
    public boolean isVoiceBackPresent() {
        return isVoiceBack;
    }
    private int minimumNbOfDigits = 1;
    private int maximumNbOfDigits;
    private java.lang.String endOfReplyDigit;
    private boolean isEndOfReplyDigit =false ;
    private java.lang.String cancelDigit;
    private boolean isCancelDigit =false ;
    private java.lang.String startDigit;
    private boolean isStartDigit =false ;
    private int firstDigitTimeOut;
    private boolean isFirstDigitTimeOut =false ;
    private int interDigitTimeOut;
    private boolean isInterDigitTimeOut =false;
    private ErrorTreatment errorTreatment = ErrorTreatment.REPORT_ERROR_TO_SCF;
    private boolean isErrorTreatment =false ;
    private boolean interruptableAnnInd = true;
    private boolean isInterruptableAnnInd =false ;
    private boolean voiceInformation = false;
    private boolean isVoiceInformation =false ;
    private boolean voiceBack = false;
    private boolean isVoiceBack =false ;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
