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

package javax.jain.ss7.inap.datatype;

import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** This class defines the ControlDigits Datatype.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class ControlDigits implements java.io.Serializable {
    /** Gets End Of Recording Digit.  */
    public java.lang.String getEndOfRecordingDigit()
        throws ParameterNotSetException {
        if (isEndOfRecordingDigit)
            return endOfRecordingDigit;
        throw new ParameterNotSetException("Control Digits: not set.");
    }
    /** Sets End Of Recording Digit.  */
    public void setEndOfRecordingDigit(java.lang.String endOfRecordingDigit) {
        this.endOfRecordingDigit = endOfRecordingDigit;
        this.isEndOfRecordingDigit = true;         
    }
    /** Indicates if the Recording  Digit optional parameter is present.
      * @return
      * True if present, false otherwise.  */
    public boolean isEndOfRecordingDigitPresent() {
        return isEndOfRecordingDigit;
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
    /** Gets Replay Digit.  */
    public java.lang.String getReplayDigit()
        throws ParameterNotSetException {
        if (isReplayDigit)
            return replayDigit;
        throw new ParameterNotSetException("Replay Digit: not set.");
    }
    /** Sets Replay Digit.  */
    public void setReplayDigit(java.lang.String replayDigit) {
        this.replayDigit = replayDigit;
        isReplayDigit = true; 
    }
    /** Indicates if the Replay  Digit optional parameter is present.
      * @return
      * True if present, false otherwise.  */
    public boolean isReplayDigitPresent() {
        return isReplayDigit;
    }
    /** Gets Restart Recording Digit.  */
    public java.lang.String getRestartRecordingDigit()
        throws ParameterNotSetException {
        if (isRestartRecordingDigit)
            return restartRecordingDigit;
        throw new ParameterNotSetException("Restart Recording: not set.");
    }
    /** Sets Restart  Recording Digit.  */
    public void setRestartRecordingDigit(java.lang.String restartRecordingDigit) {
        this.restartRecordingDigit = restartRecordingDigit;
        this.isRestartRecordingDigit = true;
    }
    /** Indicates if the Restart Recording  Digit optional parameter is present.
      * @return
      * True if present, false otherwise.  */
    public boolean isRestartRecordingDigitPresent() {
        return isRestartRecordingDigit;
    }
    /** Gets Restart Allowed.  */
    public boolean getRestartAllowed() {
        return restartAllowed;
    }
    /** Sets Restart Allowed .  */
    public void setRestartAllowed(boolean restartAllowed) {
        this.restartAllowed = restartAllowed;
    }
    /** Gets Replay Allowed.  */
    public boolean getReplayAllowed() {
        return replayAllowed;
    }
    /** Sets Replay Allowed .  */
    public void setReplayAllowed(boolean replayAllowed) {
        this.replayAllowed = replayAllowed;
    }
    private java.lang.String endOfRecordingDigit;
    private boolean isEndOfRecordingDigit = false;
    private java.lang.String cancelDigit;
    private boolean isCancelDigit = false;
    private java.lang.String replayDigit;
    private boolean isReplayDigit = false;
    private java.lang.String restartRecordingDigit;
    private boolean isRestartRecordingDigit = false;
    private boolean restartAllowed = false;
    private boolean replayAllowed = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
