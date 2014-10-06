/*
 @(#) $RCSfile: NotificationInd.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:05 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:05 $ by $Author: brian $
 */

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing the ANSI Notification Indication parameter and the ITU
  * Generic Notification Indicator parameter.
  * This class is common to ITU and ANSI variants. The values returned by the get
  * method for the two variants may be different.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class NotificationInd implements java.io.Serializable {
    public static final byte NI_USER_SUSPENDED = 0;
    public static final byte NI_USER_RESUMED = 1;
    public static final byte NI_BEARER_SERVICE_CHANGE = 2;
    public static final byte NI_DISCRIMINATOR_FOR_EXTENSION_TO_ASN1_ENCODED_COMPONENT = 3;
    public static final byte NI_CALL_COMPLETION_DELAY = 4;
    public static final byte NI_CONFERENCE_ESTABLISHED = 66;
    public static final byte NI_CONFERENCE_DISCONNECTED = 67;
    public static final byte NI_OTHER_PARTY_ADDED = 68;
    public static final byte NI_ISOLATED = 69;
    public static final byte NI_REATTACHED = 70;
    public static final byte NI_OTHER_PARTY_ISOLATED = 71;
    public static final byte NI_OTHER_PARTY_ATTACHED = 72;
    public static final byte NI_OTHER_PARTY_SPLIT = 73;
    public static final byte NI_OTHER_PARTY_DISCONNECTED = 74;
    public static final byte NI_CONFERENCE_FLOATING = 75;
    public static final byte NI_CALL_IS_WAITING_CALL = 96;
    public static final byte NI_DIVERSION_ACTIVATED = 104;
    public static final byte NI_CALL_TRANSFER_ALERTING = 105;
    public static final byte NI_CALL_TRANSFER_ACTIVE = 106;
    public static final byte NI_REMOTE_HOLD = 113;
    public static final byte NI_REMOTE_RETRIEVAL = 122;
    public static final byte NI_CALL_IS_DIVERTING = 123;
    public static final boolean EI_NEXT_OCTET_EXIST = false;
    public static final boolean EI_LAST_OCTET = true;
    /** Constructs a new NotificationInd class, parameters with default values.  */
    public NotificationInd() {
    }
    /** Constructs a NotificationInd class from the input parameters specified.
      * @param in_notificationInd  The notification indicator, range 0 to 127; <ul>
      * <li>NI_USER_SUSPENDED (ITU only),
      * <li>NI_USER_RESUMED (ITU only),
      * <li>NI_BEARER_SERVICE_CHANGE (ITU only),
      * <li>NI_DISCRIMINATOR_FOR_EXTENSION_TO_ASN1_ENCODED_COMPONENT (ITU only),
      * <li>NI_CALL_COMPLETION_DELAY (ITU only),
      * <li>NI_CONFERENCE_ESTABLISHED (ITU only),
      * <li>NI_CONFERENCE_DISCONNECTED (ITU only),
      * <li>NI_OTHER_PARTY_ADDED (ITU only),
      * <li>NI_ISOLATED (ITU only),
      * <li>NI_REATTACHED (ITU only),
      * <li>NI_OTHER_PARTY_ISOLATED (ITU only),
      * <li>NI_OTHER_PARTY_ATTACHED (ITU only),
      * <li>NI_OTHER_PARTY_SPLIT (ITU only),
      * <li>NI_OTHER_PARTY_DISCONNECTED (ITU only),
      * <li>NI_CONFERENCE_FLOATING (ITU only),
      * <li>NI_CALL_IS_WAITING_CALL,
      * <li>NI_DIVERSION_ACTIVATED (ITU only),
      * <li>NI_CALL_TRANSFER_ALERTING,
      * <li>NI_CALL_TRANSFER_ALERTING,
      * <li>NI_CALL_TRANSFER_ACTIVE,
      * <li>NI_REMOTE_HOLD,
      * <li>NI_REMOTE_RETRIEVAL and
      * <li>NI_CALL_IS_DIVERTING. </ul>
      * @param in_extInd  The extensition indicator; <ul>
      * <li>EI_NEXT_OCTET_EXIST and
      * <li>EI_LAST_OCTET. </ul>
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public NotificationInd(byte in_notificationInd, boolean in_extInd)
        throws ParameterRangeInvalidException {
        this.setNotificationInd(in_notificationInd);
        this.setExtensionIndicator(in_extInd);
    }
    /** Gets the Notification Indicator field of the parameter.
      * @return The NotificationIndicator value, range 0 to 127, see
      * NotificationInd().  */
    public byte getNotificationInd() {
        return m_notificationIndicator;
    }
    /** Sets the Notification Indicator field of the parameter.
      * @param aNotificationIndicator  Notification indicator, range 0 to 127, see
      * NotificationInd().
      * @exception ParameterRangeInvalidException  Thrown when the sub-field is out of
      * the parameter range specified.  */
    public void setNotificationInd(byte aNotificationIndicator)
        throws ParameterRangeInvalidException {
        if (0 > aNotificationIndicator || aNotificationIndicator > 127)
            throw new ParameterRangeInvalidException("NotificationInd value " + aNotificationIndicator + " out of range.");
        m_notificationIndicator = aNotificationIndicator;
    }
    /** Gets the Extension indicator field of the parameter.
      * @return boolean  The extension indicator, see NotificationInd().  */
    public boolean getExtensionIndicator() {
        return m_extensionIndicator;
    }
    /** Sets the Extension indicator field of the parameter.
      * @param aExtensionIndicator  The ExtensionIndicator value, see
      * NotificationInd().  */
    public void setExtensionIndicator(boolean aExtensionIndicator) {
        m_extensionIndicator = aExtensionIndicator;
    }
    /** The toString method retrieves a string containing the values of the members of
      * the NotificationInd class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.siup.NotificaitonInd");
        b.append("\n\tm_notificationIndicator: " + m_notificationIndicator);
        b.append("\n\tm_extensionIndicator: " + m_extensionIndicator);
        return b.toString();
    }
    protected byte m_notificationIndicator;
    protected boolean m_extensionIndicator;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
