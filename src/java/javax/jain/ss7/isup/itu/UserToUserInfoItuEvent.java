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

package javax.jain.ss7.isup.itu;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** An ISUP EVENT: The UserToUserInfoItuEvent class is a sub class of the IsupEvent class
  * and is exchanged between an ISUP Provider and an ISUP Listener for sending and
  * receiving the ISUP User to User Information (USR) Message.
  * Listener would send an EventUserToUserInfo object to the provider for sending an USR
  * message to the ISUP stack. ISUP Provider would send an EventUserToUserInfo object to
  * the listener on the reception of an USR message from the stack for the user address
  * handled by that listener. The mandatory parameters are supplied to the constructor.
  * Optional parameters may then be set using the set methods The primitive field is
  * filled as ISUP_PRIMITIVE_USER_TO_USER_INFO.  
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class UserToUserInfoItuEvent extends IsupEvent {
    /** Constructs a new UserToUserInfoItuEvent, with only the JAIN ISUP Mandatory
      * parameters being supplied to the constructor.
      * @param source  The source of this event.
      * @param dpc  The destination point code.
      * @param opc  The origination point code.
      * @param sls  The signaling link selection.
      * @param cic  The CIC on which the call has been established.
      * @param congestionPriority  Priority of the ISUP message which may be used in the
      * optional national congestion control procedures at MTP3. Refer to
      * getCongestionPriority method in IsupEvent class for more details.
      * @param userToUserInfo  User to User information.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public UserToUserInfoItuEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority,
            byte[] in_userToUserInfo)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
        this.setUserToUserInformation(in_userToUserInfo);
    }
    /** Gets the ISUP USER TO USER INFORMATION primtive value.
      * @return The ISUP USER TO USER INFORMATION primitive value.
      */
    public int getIsupPrimitive() {
        return IsupConstants.ISUP_PRIMITIVE_USER_TO_USER_INFO;
    }
    /** Checks if the mandatory parameters have been set.
      * @exception MandatoryParameterNotSetException  Thrown when the mandatory parameter
      * is not set.
      */
    public void checkMandatoryParameters()
        throws MandatoryParameterNotSetException {
        super.checkMandatoryParameters();
        getUserToUserInformation();
    }
    /** Gets the UserToUserInformation parameter of the message.
      * @return The byte array for UserToUserInformation parameter of the event.
      * @exception MandatoryParameterNotSetException  Thrown when the Mandatory parameter
      * is not set.
      */
    public byte[] getUserToUserInformation()
        throws MandatoryParameterNotSetException {
        if (m_uui != null)
            return m_uui;
        throw new MandatoryParameterNotSetException("UserToUserInformation not set.");
    }
    /** Sets the UserToUserInformation parameter of the message.
      * @param userToUserInfo  The UserToUserInformation parameter of the event.
      */
    public void setUserToUserInformation(byte[] userToUserInfo) {
        m_uui = userToUserInfo;
    }
    /** Gets the AccessTransport parameter of the message.
      * @return The AccessTransport parameter of the event.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.
      */
    public byte[] getAccessTransport()
        throws ParameterNotSetException {
        if (m_at != null)
            return m_at;
        throw new ParameterNotSetException("AccessTransport not set.");
    }
    /** Sets the AccessTransport parameter of the message.
      * @param accessTrans  The AccessTransport parameter of the event.
      */
    public void setAccessTransport(byte[] accessTrans) {
        m_at = accessTrans;
    }
    /** Indicates if the AccessTransport parameter is present in this Event.
      * @return True if the parameter has been set.
      */
    public boolean isAccessTransportPresent() {
        return (m_at != null);
    }
    /** The toString method retrieves a string containing the values of the members of the
      * UserToUserInfoItuEvent class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.itu.UserToUserInfoItuEvent");
        b.append("\n\tm_uui: ");
        if (m_uui != null)
            b.append(JainSS7Utility.bytesToHex(m_uui, 0, m_uui.length));
        b.append("\n\tm_at: ");
        if (m_at != null)
            b.append(JainSS7Utility.bytesToHex(m_at, 0, m_at.length));
        return b.toString();
    }
    protected byte[] m_uui = null;
    protected byte[] m_at = null;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
