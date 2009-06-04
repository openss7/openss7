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

package jain.protocol.ss7.oam;

import javax.management.Notification;

import jain.protocol.ss7.*;
import jain.*;

/**
  * This Notification may be emitted by any OAMLayerManagerMBean object if an error is
  * encountered. This Notification may then be passed to all concerned Notification
  * Listeners. <p>
  *
  * If the error is not directly related to OAM/SS7, eg. it is a Java, etc. error then an
  * error message string can be set. <p>
  *
  * The Notification class extends the java.util.EventObject base class and defines the
  * minimal information contained in a notification. It contains the following fields:
  * <ul>
  *
  * <li> the <em>notification type</em>, which is a string expressed in a dot notation
  * similar to Java properties.
  *
  * <li> a <em>sequence number</em>, which is a serial number identifying a particular
  * instance of notification in the context of the notification source
  *
  * <li> a <em>time stamp</em>, indicating when the notification was generated
  *
  * <li> a <em>message</em> contained in a string, which could be the explanation of the
  * notification for displaying to a user
  *
  * <li> <em>userData</em> is used for whatever other data the notification source wishes
  * to communicate to its consumers </ul>
  *
  * Notification sources should use the notification type to indicate the nature of the
  * event to their consumers. When additional information needs to be transmitted to
  * consumers, the source may place it in the message or user data fields. <p>
  *
  * <center> [NotificationHierarchy.jpg] </center><br>
  * <center> Inheritance hierarchy for JAIN OAM Notification </center><br>
  *
  * @version 1.1
  * @author Colm Hayden
  */
public abstract class OamErrorNotification extends Notification implements java.io.Serializable, java.lang.Cloneable {
    protected OamErrorNotification() {
        super("",null,0);
    }
    /**
      * Error Type constant: Indicates that a non-OAM related. (eg. Java, JNI, etc.) error.
      * <br> Emitted By: Any OamLayerManagerMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.error.non_oam_related_error".
      */
    public static final int NON_OAM_RELATED_ERROR = 0;
    /**
      * Returns the Type of this Error.
      * @return
      * One of the defined Error Types.
      */
    public int getErrorType() {
        return m_errorType;
    }
    /**
      * Sets a non-OAM related error message. This String is set when a non-OAM related
      * (eg. Java, JNI, etc.) error is encountered and should contain the details of the
      * error. <p>
      *
      * NB: This message should only be set whenever IF ErrorType == NON_OAM_RELATED_ERROR
      *
      * @param errorMsg
      * the error message, eg. 'Lost connection to underlying management system'.
      * @exception java.lang.NullPointerException
      * if any of the Objects supplied as parameters are null
      */
    public void setNonOamErrorMsg(java.lang.String errorMsg)
        throws java.lang.NullPointerException {
        if (errorMsg == null)
            throw new java.lang.NullPointerException();
        m_errorMsg = errorMsg;
    }
    /**
      * Returns a non-OAM related error message. This String is set when a non-OAM related
      * (eg. Java, JNI, etc.) error is encountered and should contain the details of the
      * error. <p>
      *
      * NB: This message should only be read whenever IF ErrorType ==
      * NON_OAM_RELATED_ERROR
      *
      * @return
      * the error message, eg. 'Lost connection to underlying management system'.
      */
    public java.lang.String getNonOamErrorMsg() {
        return m_errorMsg;
    }

    protected int m_errorType = NON_OAM_RELATED_ERROR;
    private java.lang.String m_errorMsg = "";
}


// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-

