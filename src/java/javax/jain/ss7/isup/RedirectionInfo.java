/* ***************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing a Redirection Information.
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class RedirectionInfo extends java.lang.Object implements java.io.Serializable {
    public static final byte RI_NO_REDIRECTION = 0;
    public static final byte RI_CALL_ROUTED = 1;
    public static final byte RI_CALL_REROUTED = 2;
    public static final byte RI_CALL_DIVERSION = 3;
    public static final byte RI_CALL_DIVERSION_ALL_REDIRECTION_INFORMATION_PRESENTATION_RESTRICTED = 4;
    public static final byte RI_CALL_REROUTED_REDIRECTION_NUMBER_PRESENTATION_RESTRICTED = 5;
    public static final byte RI_CALL_DIVERSION_REDIRECTION_NUMBER_PRESENTATION_RESTRICTED = 6;
    public static final byte ORR_UNKNOWN_UNAVAILABLE = 0;
    public static final byte ORR_USER_BUSY = 1;
    public static final byte ORR_NO_REPLY = 2;
    public static final byte ORR_UNCONDITIONAL = 3;
    public static final byte RR_UNKNOWN_UNAVAILABLE = 0;
    public static final byte RR_USER_BUSY = 1;
    public static final byte RR_NO_REPLY = 2;
    public static final byte RR_UNCONDITIONAL = 3;
    public static final byte RR_DEFLECTION_DURING_ALERTING = 4;
    public static final byte RR_DEFLECTION_IMMEDIATE_RESPONSE = 5;
    public static final byte RR_MOBILE_SUBSCRIBER_NOT_REACHABLE = 6;
    /** Constructs a new RedirectionInfo taking no parameters.
      * This constructor sets all the sub-field values to their spare value (which is
      * zero).
      */
    public RedirectionInfo() {
    }
    /** Constructor For RedirectionInfo.
      * @param redirectingIndicator  The redirecting indicator <ul> <li>RI_NO_REDIRECTION
      * (ITU only), <li>RI_CALL_ROUTED (ITU only), <li>RI_CALL_REROUTED (ITU only),
      * <li>RI_CALL_DIVERSION (ITU only),
      * <li>RI_CALL_DIVERSION_ALL_REDIRECTION_INFORMATION_PRESENTATION_RESTRICTED (ITU
      * only), <li>RI_CALL_REROUTED_REDIRECTION_NUMBER_PRESENTATION_RESTRICTED (ITU only)
      * and <li>RI_CALL_DIVERSION_REDIRECTION_NUMBER_PRESENTATION_RESTRICTED (ITU only).
      * </ul>
      * @param originalRedirectingReason  The original redirecting reason <ul>
      * <li>ORR_UNKNOWN_UNAVAILABLE, <li>ORR_USER_BUSY, <li>ORR_NO_REPLY and
      * <li>ORR_UNCONDITIONAL. </ul>
      * @param redirectionCounter  The redirection counter.
      * @param redirectingReason  The redirecting reason <ul> <li>RR_UNKNOWN_UNAVAILABLE,
      * <li>RR_USER_BUSY, <li>RR_NO_REPLY, <li>RR_UNCONDITIONAL,
      * <li>RR_DEFLECTION_DURING_ALERTING (ITU only), <li>RR_DEFLECTION_IMMEDIATE_RESPONSE
      * (ITU only) and <li>RR_MOBILE_SUBSCRIBER_NOT_REACHABLE (ITU only). </ul>
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public RedirectionInfo(byte redirectingIndicator, byte originalRedirectingReason,
            byte redirectionCounter, byte redirectingReason)
        throws ParameterRangeInvalidException {
        this.setRedirectingInd(redirectingIndicator);
        this.setOriginalRedirectingReason(originalRedirectingReason);
        this.setRedirectionCounter(redirectionCounter);
        this.setRedirectingReason(redirectingReason);
    }
    /** Gets the Original Redirecting Reason field of the parameter.
      * @return Byte containing the OrigRedirectingReason, range 0 to 15, see
      * RedirectionInfo().
      */
    public byte getOriginalRedirectingReason() {
        return m_originalRedirectingReason;
    }
    /** Sets the Original Redirecting Reason field of the parameter.
      * @param originalRedirectingReason  The Original Redirecting Reason, range 0 to 15,
      * see RedirectionInfo().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setOriginalRedirectingReason(byte originalRedirectingReason)
        throws ParameterRangeInvalidException {
        if (0 <= originalRedirectingReason && originalRedirectingReason <= 15) {
            m_originalRedirectingReason = originalRedirectingReason;
            return;
        }
        throw new ParameterRangeInvalidException("OriginalRedirectingReason value " + originalRedirectingReason + " out of range.");
    }
    /** Gets the Redirecting Indicator field of the parameter.
      * @return byte containing the Redirecting Indicator, range 0 to 7, see
      * RedirectionInfo().
      */
    public byte getRedirectingInd() {
        return m_redirectingIndicator;
    }
    /** Sets the Redirecting Indicator field of the parameter.
      * @param redirectingIndicator  Byte containing the Redirecting Indicator value,
      * range 0 to 7, see RedirectionInfo().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setRedirectingInd(byte redirectingIndicator)
        throws ParameterRangeInvalidException {
        if (0 <= redirectingIndicator && redirectingIndicator <= 7) {
            m_redirectingIndicator = redirectingIndicator;
            return;
        }
        throw new ParameterRangeInvalidException("RedirectingIndicator value " + redirectingIndicator + " out of range.");
    }
    /** Gets the Redirecting Reason field of the parameter.
      * @return The Redirecting Reason, see RedirectionInfo().
      */
    public byte getRedirectingReason() {
        return m_redirectingReason;
    }
    /** Sets the Redirecting Reason field of the parameter.
      * @param redirectingReason  Byte containing the Redirecting Reason value, range 0 to
      * 15, see RedirectionInfo.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setRedirectingReason(byte redirectingReason)
        throws ParameterRangeInvalidException {
        if (0 <= redirectingReason && redirectingReason <= 15) {
            m_redirectingReason = redirectingReason;
            return;
        }
        throw new ParameterRangeInvalidException("RedirectingReason value " + redirectingReason + " out of range.");
    }
    /** Gets the Redirection Counter field of the parameter.
      * Gives a count of the number of times a call has been redirected.
      * @return byte containing the Redirection Counter, range 0 to 7, see
      * RedirectionInfo().
      */
    public byte getRedirectionCounter() {
        return m_redirectionCounter;
    }
    /** Sets the Redirection Counter field of the parameter.
      * This gives the number of times the call has been redirected.
      * @param redirectionCounter  The Redirection Counter value, range 0 to 7, see
      * RedirectionInfo().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setRedirectionCounter(byte redirectionCounter)
        throws ParameterRangeInvalidException {
        if (0 <= redirectionCounter && redirectionCounter <= 7) {
            m_redirectionCounter = redirectionCounter;
        }
        throw new ParameterRangeInvalidException("RedirectionCounter value " + redirectionCounter + " out of range.");
    }
    /** The toString method retrieves a string containing the values of the members of the
      * RedirectionInfo class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.RedirectionInfo");
        b.append("\n\tm_originalRedirectingReason: " + m_originalRedirectingReason);
        b.append("\n\tm_redirectingIndicator: " + m_redirectingIndicator);
        b.append("\n\tm_redirectingReason: " + m_redirectingReason);
        b.append("\n\tm_redirectionCounter: " + m_redirectionCounter);
        return b.toString();
    }
    protected byte m_originalRedirectingReason;
    protected byte m_redirectingIndicator;
    protected byte m_redirectingReason;
    protected byte m_redirectionCounter;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
