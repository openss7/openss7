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

package javax.jain.ss7.isup.itu;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing the ITU ISUP Call Diversion Information parameter.
    This class provides methods for accessing the sub-fields of this parameter.

    @author Monavacon Limited
    @version 1.2.2
  */
public class CallDiversionInfoItu implements java.io.Serializable {
    public static final byte NSO_UNKNOWN = 0;
    public static final byte NSO_PRESENTATION_NOT_ALLOWED = 1;
    public static final byte NSO_PRESENTATION_ALLOWED_WITH_REDIRECTION_NUMBER = 2;
    public static final byte NSO_PRESENTATION_ALLOWED_WITHOUT_REDIRECTION_NUMBER = 3;
    public static final byte RR_UNKNOWN = 0;
    public static final byte RR_USER_BUSY = 1;
    public static final byte RR_NO_REPLY = 2;
    public static final byte RR_UNCONDITIONAL = 3;
    public static final byte RR_REFLECTION_DURING_ALERTING = 4;
    public static final byte RR_REFLECTION_IMMEDIATE_RESPONSE = 5;
    public static final byte RR_MOBILE_SUBSCRIBER_NOT_REACHABLE = 6;
    /** Constructs a CallDiversionInfoItu class, parameters with default values. */
    public CallDiversionInfoItu() {
    }
    /** Constructs a CallDiversionInfoItu class from the input parameters specified.
        @param in_notificationSubsOpt  The notification subscription option, range 0 to 7.
        <ul> <li>NSO_UNKNOWN, <li>NSO_PRESENTATION_NOT_ALLOWED,
        <li>NSO_PRESENTATION_ALLOWED_WITH_REDIRECTION_NUMBER and
        <li>NSO_PRESENTATION_ALLOWED_WITHOUT_REDIRECTION_NUMBER. </ul>
        @param in_redReason - The redirection reason, range 0 to 15. <ul> <li>RR_UNKNOWN,
        <li>RR_USER_BUSY, <li>RR_NO_REPLY, <li>RR_UNCONDITIONAL,
        <li>RR_REFLECTION_DURING_ALERTING, <li>RR_REFLECTION_IMMEDIATE_RESPONSE and
        <li>RR_MOBILE_SUBSCRIBER_NOT_REACHABLE. </ul>
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public CallDiversionInfoItu(byte in_notificationSubsOpt, byte in_redReason)
        throws ParameterRangeInvalidException {
        this.setNotificationSubsOpt(in_notificationSubsOpt);
        this.setRedirectionReason(in_redReason);
    }
    /** Gets the notification subscription options field of the parameter.
        @return The NotificationSubscriptionOptions value, range 0 to 7, see
        CallDiversionInfoItu().
      */
    public byte getNotificationSubsOpt() {
        return m_notificationSubscriptionOptions;
    }
    /** Sets the notification subscription options field of the parameter.
        @param aNotificationSubscriptionOptions  The Notification Subscription Options
        value, see CallDiversionInfoItu().
        @exception ParameterRangeInvalidException  Thrown when the sub-field is out of the
        specified range.
      */
    public void setNotificationSubsOpt(byte aNotificationSubscriptionOptions)
        throws ParameterRangeInvalidException {
        if (0 <= aNotificationSubscriptionOptions && aNotificationSubscriptionOptions <= 7) {
            m_notificationSubscriptionOptions = aNotificationSubscriptionOptions;
            return;
        }
        throw new ParameterRangeInvalidException("NotificationSubsOpt valud " + aNotificationSubscriptionOptions + " out of range.");
    }
    /** Gets the redirection reason field of the parameter.
        @return The RedirectionReason value, range 0 to 15, see CallDiversionInfoItu().
      */
    public byte getRedirectionReason() {
        return m_redirectionReason;
    }
    /** Sets the redirection reason field of the parameter.
        @param aRedirectionReason  The Redirection Reason value, see
        CallDiversionInfoItu().
        @exception ParameterRangeInvalidException  Thrown when the sub-field is out of the
        specified range.
      */
    public void setRedirectionReason(byte aRedirectionReason)
        throws ParameterRangeInvalidException {
        if (0 <= aRedirectionReason && aRedirectionReason <= 15) {
            m_redirectionReason = aRedirectionReason;
        }
        throw new ParameterRangeInvalidException("RedirectionReason value " + aRedirectionReason + " out of range.");
    }
    /** The toString method retrieves a string containing the values of the members of the
        CallDiversionInfoItu class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.itu.CallDiversionInfoItu");
        b.append("\n\tm_notificationSubscriptionOptions: " + m_notificationSubscriptionOptions);
        b.append("\n\tm_redirectionReason: " + m_redirectionReason);
        return b.toString();
    }
    protected byte m_notificationSubscriptionOptions;
    protected byte m_redirectionReason;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
