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

/** An ISUP EVENT: The SegmentationItuEvent class is a sub class of the IsupEvent class
  * and is exchanged between an ISUP Provider and an ISUP Listener for sending and
  * receiving the ISUP Segmentation Message.
  * Listener would send an EventSegmentation object to the provider for sending a SGM
  * message to the ISUP stack. ISUP Provider would send an EventSegmentation object to the
  * listener on the reception of an SGM message from the stack for the user address
  * handled by that listener.  The mandatory parameters for generating an SGM ISUP message
  * are set using the constructor itself.  The optional parameters may be set using get
  * and set methods described below.
  * @author Monavacon
  * @version 1.2.2
  */
public class SegmentationItuEvent extends IsupEvent {
    /** Constructs a new SegmentationItuEvent, with only the JAIN ISUP Mandatory
      * parameters being supplied to the constructor.
      * @param source  The source of this event.
      * @param dpc  The destination point code.
      * @param opc  The origination point code.
      * @param sls  The signaling link selection.
      * @param cic  The CIC on which the call has been established.
      * @param congestionPriority  Priority of the ISUP message which may be used in the
      * optional national congestion control procedures at MTP3. Refer to
      * getCongestionPriority method in IsupEvent class for more details.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public SegmentationItuEvent(java.lang.Object source, SignalingPointCode dpc,
            SignalingPointCode opc, byte sls, int cic, byte congestionPriority)
        throws ParameterRangeInvalidException {
        super(source, dpc, opc, sls, cic, congestionPriority);
    }
    /** Gets the ISUP SEGMENTATION primtive value.
      * @return The ISUP SEGMENTATION primitive value.
      */
    public int getIsupPrimitive() {
        return IsupConstants.ISUP_PRIMITIVE_SEGMENTATION;
    }
    /** Gets the AccessTransport parameter of the message.
      * @return The AccessTransport parameter of the event.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not set
      */
    public byte[] getAccessTransport()
        throws ParameterNotSetException {
        if (m_accessTrans != null)
            return m_accessTrans;
        throw new ParameterNotSetException("AccessTransport not set.");
    }
    /** Sets the AccessTransport parameter of the message.
      * @param accessTrans  The AccessTransport parameter of the event.
      */
    public void setAccessTransport(byte[] accessTrans) {
        m_accessTrans = accessTrans;
    }
    /** Indicates if the AccessTransport parameter is present in this Event.
      * @return True if the parameter has been set.
      */
    public boolean isAccessTransportPresent() {
        return (m_accessTrans != null);
    }
    /** Gets the GenericDigits parameter of the message.
      * Refer to GenericDigits parameter class for more information.
      * @return The GenericDigits parameter of the event.
      * @exception ParameterNotSetException  Thrown when mandatory ISUP parameter is not
      * set.
      */
    public GenericDigits getGenericDigits()
        throws ParameterNotSetException {
        if (m_gd != null)
            return m_gd;
        throw new ParameterNotSetException("GenericDigits not set.");
    }
    /** sets the Generic Digits parameter of the message.
      * Refer to GenericDigits parameter class for more information.
      * @param nci  The Generic Digits parameter of the event.
      */
    public void setGenericDigits(GenericDigits gd) {
        m_gd = gd;
    }
    /** Indicates if the GenericDigits parameter is present in this Event.
      * @return True if the parameter has been set.
      */
    public boolean isGenericDigitsPresent() {
        return (m_gd != null);
    }
    /** Gets the GenericNumberItu parameter of the message.
      * @return The GenericNumberItu parameter of the event.
      * @exception ParameterNotSetException  Thrown when the parameter is not set in the
      * event.
      */
    public GenericNumberItu getGenericNumber()
        throws ParameterNotSetException {
        if (m_genNum != null)
            return m_genNum;
        throw new ParameterNotSetException("GenericNumberItu not set.");
    }
    /** Sets the GenericNumberItu parameter of the message.
      * @param genNum  Generic Number parameter of the event.
      */
    public void setGenericNumber(GenericNumberItu genNum) {
        m_genNum = genNum;
    }
    /** Indicates if the GenericNumberItu parameter is present in this Event.
      * @return True if the parameter is set.
      */
    public boolean isGenericNumberPresent() {
        return (m_genNum != null);
    }
    /** Gets the Message Compatibility Information parameter of the message.
      * Refer to Message Compatibility Information parameter for greater details.
      * @return Message Compatibility Information parameter.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.
      */
    public MessageCompatibilityInfoItu getMessageCompatibilityInfo()
        throws ParameterNotSetException {
        if (m_messageCompInfo != null)
            return m_messageCompInfo;
        throw new ParameterNotSetException("MessageCompatibilityInfoItu not set.");
    }
    /** Sets the Message Compatibility Information parameter of the message.
      * Refer to Message Compatibility Information parameter for greater details.
      * @param in_MessageCompInfo  The Message Compatibility Information parameter of the
      * event.
      */
    public void setMessageCompatibilityInfo(MessageCompatibilityInfoItu in_MessageCompInfo) {
        m_messageCompInfo = in_MessageCompInfo;
    }
    /** Indicates if the Message Compatibility Information parameter is present in this
      * Event.
      * @return True if the parameter has been set.
      */
    public boolean isMessageCompatibilityInfoPresent() {
        return (m_messageCompInfo != null);
    }
    /** Gets the NotificationInd parameter of the message.
      * @return The NotificationInd parameter of the event.
      * @exception ParameterNotSetException  Thrown when the parameter is not set in the
      * event.
      */
    public NotificationInd getNotificationInd()
        throws ParameterNotSetException {
        if (m_noi != null)
            return m_noi;
        throw new ParameterNotSetException("NotificationInd not set.");
    }
    /** Sets the NotificationInd parameter of the message.
      * @param noi  Notification Indicator parameter.
      */
    public void setNotificationInd(NotificationInd noi) {
        m_noi = noi;
    }
    /** Indicates if the NotificationInd parameter is present in this Event.
      * @return True if the parameter is set.
      */
    public boolean isNotificationIndPresent() {
        return (m_noi != null);
    }
    /** Gets the UserToUserInformation parameter of the message.
      * @return The byte array for UserToUserInformation parameter of the event.
      * @exception ParameterNotSetException  Thrown when the optional parameter is not
      * set.
      */
    public byte[] getUserToUserInformation()
        throws ParameterNotSetException {
        if (m_userToUserInfo != null)
            return m_userToUserInfo;
        throw new ParameterNotSetException("UserToUserInformation not set.");
    }
    /** Sets the UserToUserInformation parameter of the message.
      * @param userToUserInfo  The UserToUserInformation parameter of the event.
      */
    public void setUserToUserInformation(byte[] userToUserInfo) {
        m_userToUserInfo = userToUserInfo;
    }
    /** Indicates if the UserToUserInformation parameter is present in this Event.
      * @return True if the parameter has been set.
      */
    public boolean isUserToUserInformationPresent() {
        return (m_userToUserInfo != null);
    }
    /** The toString method retrieves a string containing the values of the members of the
      * SegmentationItuEvent class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.itu.SegmentationItuEvent");
        b.append("\n\tm_accessTrans: ");
        if (m_accessTrans!= null)
            b.append(JainSS7Utility.bytesToHex(m_accessTrans, 0, m_accessTrans.length));
        b.append("\n\tm_gd: " + m_gd);
        b.append("\n\tm_genNum: " + m_genNum);
        b.append("\n\tm_messageCompInfo: " + m_messageCompInfo);
        b.append("\n\tm_noi: " + m_noi);
        b.append("\n\tm_userToUserInfo: ");
        if (m_userToUserInfo!= null)
            b.append(JainSS7Utility.bytesToHex(m_userToUserInfo, 0, m_userToUserInfo.length));
        return b.toString();
    }
    protected byte[] m_accessTrans = null;
    protected GenericDigits m_gd = null;
    protected GenericNumberItu m_genNum = null;
    protected MessageCompatibilityInfoItu m_messageCompInfo = null;
    protected NotificationInd m_noi = null;
    protected byte[] m_userToUserInfo = null;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
