/*
 @(#) src/java/javax/jain/protocol/ss7/map/position/etsipar/LcsClientId.java <p>
 
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

package javax.jain.protocol.ss7.map.position.etsipar;

import javax.jain.protocol.ss7.map.position.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/** This parameter identifies the client or type of client positioning application. (See
  * specification GSM 03.71 version 8.0.0 Release 1999, section 7.6.1.1.  for guidance.)
  * <p>
  *
  * The following rules applies for the setting of parameter components:- <ul>
  * <li>Mandatory component with no default value must be provided to the constructor.
  * <li>Mandatory component with default value need not be set.
  * <li>Optional / conditional component is by default not present. Such a component
  * becomes present by setting it. </ul> <p>
  *
  * <h4>Parameter components:-</h4> <ul>
  *
  * <li>lcsClientType, mandatory component with no default
  * <li>lcsClientExternalId, optional component
  * <li>lcsClientDialedByMS, optional component
  * <li>lcsClientInternalId, optional component
  * <li>lcsClientName, optional component </ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class LcsClientId extends MapParameter {
    /** An undefined service is requesting position information.  */
    public static final int UNDEFINED_SERVICES = 0;
    /** An emergency service is requesting position information.  */
    public static final int EMERGENCY_SERVICES = 1;
    /** A value added service is requesting position information.  */
    public static final int VALUE_ADDED_SERVICES = 2;
    /** A PLMN operator service is requesting position information.  */
    public static final int PLMN_OPERATOR_SERVICES = 3;
    /** A lawful intercept service is requesting position information.  */
    public static final int LAWFUL_INTERCEPT_SERVICES = 4;
    /** Client Internal ID in the visited network: broadcast service.  */
    public static final int CLIENT_BROADCAST_SERVICE = 5;
    /** Client Internal ID in the visited network: OAM in home-PLMN.  */
    public static final int CLIENT_OAM_HPLMN = 6;
    /** Client Internal ID in the visited network: OAM in visited-PLMN.  */
    public static final int CLIENT_OAM_VPLMN = 7;
    /** Client Internal ID in the visited network: anonymous location.  */
    public static final int CLIENT_ANONYMOUS_LOCATION = 8;
    /** Client Internal ID in the visited network: subscribed service.  */
    public static final int CLIENT_MS_SUBSCRIBED_SERVICE = 9;
    /** The constructor.
      * @param lcsClientType  Indicates what kind of service that is requesting position.
      * Possible values: <ul>
      * <li>EMERGENCY_SERVICES,
      * <li>VALUE_ADDED_SERVICES,
      * <li>PLMN_OPERATOR_SERVICES and
      * <li>LAWFUL_INTERCEPT_SERVICES. </ul>
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public LcsClientId(int lcsClientType)
        throws SS7InvalidParamException {
        setLcsClientType(lcsClientType);
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public LcsClientId()
        throws SS7InvalidParamException {
    }
    /** Change the Client Type parameter whcih indicates what kind of service that is
      * requesting position.
      * @param lcsClientType  Integer indicating type of client. (See constructor for
      * possible values.)
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public void setLcsClientType(int lcsClientType)
        throws SS7InvalidParamException {
        switch (lcsClientType) {
            case UNDEFINED_SERVICES:
            case EMERGENCY_SERVICES:
            case VALUE_ADDED_SERVICES:
            case PLMN_OPERATOR_SERVICES:
            case LAWFUL_INTERCEPT_SERVICES:
                m_lcsClientType = lcsClientType;
                return;
        }
        throw new SS7InvalidParamException("LCS Client Type value " + lcsClientType + " out of range.");
    }
    /** Get the Client Type parameter whcih indicates what kind of service that is
      * requesting position.
      * @return Integer indicating type of client. (See constructor for possible values.)
      */
    public int getLcsClientType() {
        return m_lcsClientType;
    }
    /** Set the Client External Id parameter whcih is an address ("telephone number") that
      * identifies the LCS Client.
      * @param lcsClientExternalId  Number that identifies the client.
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public void setLcsClientExternalId(SS7Number lcsClientExternalId)
        throws SS7InvalidParamException {
        m_lcsClientExternalId = lcsClientExternalId;
    }
    /** Get the Client External Id parameter whcih is an address ("telephone number") that
      * identifies the LCS Client. It shall be checked if this parameter is present before
      * getting it.
      * @return Number that identifies the client.
      */
    public SS7Number getLcsClientExternalId() {
        return m_lcsClientExternalId;
    }
    /** Check if the Client External Id parameter is present.
      * @return True or false.
      */
    public boolean isLcsClientExternalIdPresent() {
        return (m_lcsClientExternalId != null);
    }
    /** Set the LCS Client Dialed by MS parameter whcih is the number that a Mobile
      * Station dialed in order to get access to a specific positioning service in the
      * client. (See specification GSM 03.71 version 8.0.0 Release 1999, annex A3 for
      * description.)
      * @param lcsClientDialedByMS  Number that was dialed to the client.
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public void setLcsClientDialedByMS(SS7Number lcsClientDialedByMS)
        throws SS7InvalidParamException {
        m_lcsClientDialedByMS = lcsClientDialedByMS;
    }
    /** Get the LCS Client Dialed by MS parameter whcih is the number that a Mobile
      * Station dialed in order to get access to a specific positioning service in the
      * client. It shall be checked if this parameter is present before getting it.
      * @return Number that was dialed to the client.
      */
    public SS7Number getLcsClientDialedByMS() {
        return m_lcsClientDialedByMS;
    }
    /** Check if the LCS Client Dialed by MS parameter is present.
      * @return True or false.
      */
    public boolean isLcsClientDialedByMSPresent() {
        return (m_lcsClientDialedByMS != null);
    }
    /** Set the Client Internal Id parameter whcih identifies the type of client in the
      * case that the client is within or associated with the visited network. (See
      * specification GSM 03.71 version 8.0.0 Release 1999, section 7.12.3.)
      * @param lcsClientInternalId  Integer indicating type of client.  Possible values:
      * <ul>
      * <li>CLIENT_BROADCAST_SERVICE,
      * <li>CLIENT_OAM_HPLMN,
      * <li>CLIENT_OAM_VPLMN,
      * <li>CLIENT_ANONYMOUS_LOCATION and
      * <li>CLIENT_MS_SUBSCRIBED_SERVICE. </ul>
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public void setLcsClientInternalId(int lcsClientInternalId)
        throws SS7InvalidParamException {
        switch (lcsClientInternalId) {
            case CLIENT_BROADCAST_SERVICE:
            case CLIENT_OAM_HPLMN:
            case CLIENT_OAM_VPLMN:
            case CLIENT_ANONYMOUS_LOCATION:
            case CLIENT_MS_SUBSCRIBED_SERVICE:
                m_lcsClientInternalId = lcsClientInternalId;
                return;
        }
        throw new SS7InvalidParamException("LCS Client value " + lcsClientInternalId + " out of range.");
    }
    /** Get the Client Internal Id parameter whcih identifies the type of client in the
      * case that the client is within or associated with the visited network. It shall be
      * checked if this parameter is present before getting it.
      * @return Integer indicating type of client. (See setLcsClientInternalId for
      * possible values.)
      */
    public int getLcsClientInternalId() {
        return m_lcsClientInternalId;
    }
    /** Check if the Client Internal Id parameter is present.
      * @return True or false.
      */
    public boolean isLcsClientInternalIdPresent() {
        return m_lcsClientInternalIdPresent;
    }
    /** Set the Client Name parameter that names the client requesting position. Only
      * characters from the "7 bit default alphabet" specified in 3G TS 23.038 V4.0.0
      * should be used.
      * @param lcsClientName  A java.lang.String with the client name; maximum 63 characters.
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public void setLcsClientName(java.lang.String lcsClientName)
        throws SS7InvalidParamException {
        m_lcsClientName = lcsClientName;
    }
    /** Get the Client Name parameter that names the client requesting position. It shall
      * be checked if this parameter is present before getting it.
      * @return A java.lang.String with the client name.
      */
    public java.lang.String getLcsClientName() {
        return m_lcsClientName;
    }
    /** Check if the Client Name parameter is present.
      * @return True or false.
      */
    public boolean isLcsClientNamePresent() {
        return (m_lcsClientName != null);
    }
    protected int m_lcsClientType;
    protected SS7Number m_lcsClientExternalId = null;
    protected SS7Number m_lcsClientDialedByMS = null;
    protected int m_lcsClientInternalId;
    protected boolean m_lcsClientInternalIdPresent = false;
    protected java.lang.String m_lcsClientName = null;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
