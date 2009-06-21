/*
 @(#) $RCSfile: Constants.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:20 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:20 $ by $Author: brian $
 */

package jain.protocol.ip.mgcp.message;

/**
    Constants used in package jain.protocol.ip.mgcp.message.
    @version 1.2.2
    @author Monavacon Limited
  */
public final class Constants implements java.io.Serializable {
    /** Identifies an AuditConnection command.  */
    public static final int CMD_AUDIT_CONNECTION = 0;
    /** Identifies an AuditEndpoint command.  */
    public static final int CMD_AUDIT_ENDPOINT = 1;
    /** Identifies a CreateConnection command.  */
    public static final int CMD_CREATE_CONNECTION = 2;
    /** Identifies a DeleteConnection command.  */
    public static final int CMD_DELETE_CONNECTION = 3;
    /** Identifies an EndpointConfiguration command.  */
    public static final int CMD_ENDPOINT_CONFIGURATION = 4;
    /** Identifies a ModifyConnection command.  */
    public static final int CMD_MODIFY_CONNECTION = 5;
    /** Identifies a NotificationRequest command.  */
    public static final int CMD_NOTIFICATION_REQUEST = 6;
    /** Identifies a Notify command.  */
    public static final int CMD_NOTIFY = 7;
    /**
        Identifies an unknown command or response. When this value is returned by getObjectID, the
        implementation should ascertain the class of the object employing methods of class
        java.lang.Class and take implementation-specific action based on the class of the object. A
        possible use of JAIN MGCP command or response objects that return this object identifier is
        in experimental commands and responses.
      */
    public static final int CMD_RESP_UNKNOWN = 8;
    /** Identifies a RestartInProgress command.  */
    public static final int CMD_RESTART_IN_PROGRESS = 9;
    /** Identifies a response to an AuditConnection command.  */
    public static final int RESP_AUDIT_CONNECTION = 10;
    /** Identifies a response to an AuditEndpoint command.  */
    public static final int RESP_AUDIT_ENDPOINT = 11;
    /** Identifies a response to a CreateConnection command.  */
    public static final int RESP_CREATE_CONNECTION = 12;
    /** Identifies a response to a DeleteConnection command.  */
    public static final int RESP_DELETE_CONNECTION = 13;
    /** Identifies a response to an EndpointConfiguration command.  */
    public static final int RESP_ENDPOINT_CONFIGURATION = 14;
    /** Identifies a response to a ModifyConnection command.  */
    public static final int RESP_MODIFY_CONNECTION = 15;
    /** Identifies a response to a NotificationRequest command.  */
    public static final int RESP_NOTIFICATION_REQUEST = 16;
    /** Identifies a response to a Notify command.  */
    public static final int RESP_NOTIFY = 17;
    /** Identifies a response to a RestartInProgress command.  */
    public static final int RESP_RESTART_IN_PROGRESS = 18;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
