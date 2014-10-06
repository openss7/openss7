/*
 @(#) $RCSfile: EtsiMessage.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:44 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:44 $ by $Author: brian $
 */

package javax.jain.protocol.ss7.map.transaction.etsipar.message;

import javax.jain.protocol.ss7.map.transaction.etsipar.*;
import javax.jain.protocol.ss7.map.transaction.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/**
  * This class is a superclass for the ETSI message family containing
  * classes representing the SMS Messages defined in the ETSI standard
  * GSM 03.40 standard. <p>
  *
  * The ETSI message family is to be used when when the JAIN MAP API is
  * implemented on top of a layer handling the ETSI SMS protocol
  * according to GSM 03.40. <p>
  *
  * This superclass contains data common for all messages in the family,
  * such as Message Type Indicator (MTI) and User Data Header
  * information. <p>
  *
  * Note: The information in the User Data Header for message
  * concatenation, shall be handled by the MAP API implementation which
  * should do message segmentation and reassembly using the User Data
  * Header fields. <p>
  *
  * The following rules applies for the setting of parameter components:
  * <ul>
  *
  * <li>Mandatory component with no default value must be provided to
  * the constructor.
  *
  * <li>Mandatory component with default value need not be set.
  *
  * <li>Optional / conditional component is by default not present. Such
  * a component becomes present by setting it. </ul>
  *
  * <h5>Parameter components:</h5><ul>
  *
  * <li>mti - Message Type Indicator, mandatory component with no
  * default, read-only
  *
  * <li>applicationPort, optional parameter </ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract class EtsiMessage extends Message {
    /** Identifier for the ETSI DELIVER message.  */
    public static final int MESSAGE_DELIVER = 1;
    /** Identifier for the ETSI DELIVER REPORT message.  */
    public static final int MESSAGE_DELIVER_REPORT = 2;
    /** Identifier for the ETSI SUBMIT message.  */
    public static final int MESSAGE_SUBMIT = 3;
    /** Identifier for the ETSI SUBMIT REPORT message.  */
    public static final int MESSAGE_SUBMIT_REPORT = 4;
    /** Identifier for the ETSI COMMAND message.  */
    public static final int MESSAGE_COMMAND = 5;
    /** Identifier for the ETSI STATUS message.  */
    public static final int MESSAGE_STATUS_REPORT = 6;
    /** Protocol Identifier for simple MS to SC communication.  */
    public static final int PID_SME_TO_SME_IMPLICIT = 1;
    /** Protocol Identifier for interworking with Internet e-mail.  */
    public static final int PID_INTERWORKING_INTERNET_MAIL = 2;
    /** Default Data Coding Scheme (DCS).  */
    public static final int DCS_7_BIT_DEFAULT_ALPHABET = 1;
    /** Failure Cause (FCS) Telematic interworking not supported.  */
    public static final int FCS_INTERWORKING_NOT_SUPPORTED = 1;
    /** Failure Cause (FCS) Service Centre (SC) busy.  */
    public static final int FCS_SC_BUSY = 2;
    /** Failure Cause (FCS) no Service Centre subscription.  */
    public static final int FCS_NO_SC_SUBSCRIPTION = 3;
    /** Failure Cause (FCS) invalid Short Message Entity (SME) address.  */
    public static final int FCS_INVALID_SME_ADDRESS = 4;
    /** Failure Cause (FCS) error in Mobile Station (MS).  */
    public static final int FCS_ERROR_IN_MS = 5;
    /** Command Type (CT) enquiry (requesting a Status Report Message)
      * for a previously submitted message.  */
    public static final int CT_ENQUIRY = 1;
    /** Command Type (CT) cancel Status Report Request for a previously
      * submitted message.  */
    public static final int CT_CANCEL_STATUS_REPORT = 2;
    /** Command Type (CT) enable Status Report Request for a previously
      * submitted message.  */
    public static final int CT_ENABLE_STATUS_REPORT = 3;
    /** Command Type (CT) delete a previously submitted message.  */
    public static final int CT_DELETE_PREVIOUS_MESSAGE = 4;
    /** Status (ST) message transaction completed, message received by
      * the SME.  */
    public static final int ST_COMPLETED_MESSAGE_DELIVERED = 1;
    /** Status (ST) message transaction completed, message forwarded by
      * SC, unable to confirm delivery.  */
    public static final int ST_COMPLETED_DELIVERY_NOT_CONFIRMED = 2;
    /** Status (ST) message transaction completed, message replaced by
      * the SC.  */
    public static final int ST_COMPLETED_MESSAGE_REPLACED = 3;
    /** Status (ST) temporary error, SC still trying to transfer
      * message, congestion.  */
    public static final int ST_TEMP_ERROR_CONGESTION = 4;
    /** Status (ST) temporary error, SC still trying to transfer
      * message, receiver busy.  */
    public static final int ST_TEMP_ERROR_RECEIVER_BUSY = 5;
    /** Status (ST) temporary error, SC still trying to transfer
      * message, no response from receiver.  */
    public static final int ST_TEMP_ERROR_RECEIVER_NOT_RESPONDING = 6;
    /** Status (ST) permanent error, SC is not making any more transfer
      * attempts, remote procedure error.  */
    public static final int ST_ERROR_REMOTE_PROCEDURE_ERROR = 7;
    /** Status (ST) permanent error, SC is not making any more transfer
      * attempts, incompatible destination.  */
    public static final int ST_ERROR_INCOMPATIBLE_DESTINATION = 8;
    /** Status Report Qualifier (SRQ), Status Report triggered by a
      * SUBMIT message.  */
    public static final boolean SRQ_RESULT_OF_SUBMIT = false;
    /** Status Report Qualifier (SRQ), Status Report triggered by a
      * COMMAND message.  */
    public static final boolean SRQ_RESULT_OF_COMMAND = true;
    /**
      * The constructor.
      *
      * @param mti
      * Message Type Indicator, that identifies the message. Possible
      * values: <ul>
      * <li>MESSAGE_DELIVER
      * <li>MESSAGE_DELIVER_REPORT
      * <li>MESSAGE_SUBMIT
      * <li>MESSAGE_SUBMIT_REPORT
      * <li>MESSAGE_COMMAND
      * <li>MESSAGE_STATUS_REPORT </ul>
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public EtsiMessage(int mti)
        throws SS7InvalidParamException {
        setMti(mti);
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public EtsiMessage()
        throws SS7InvalidParamException {
    }
    /**
      * Get the Message Type Indicator parameter.
      *
      * @return
      * Message Type Indicator, that identifies the message. (See the
      * constructor for possible values.)
      */
    public int getMti() {
        return m_mti;
    }
    /**
      * Set the Application Port parameter.
      * This parameter makes it possible to address different
      * sub-applications using this messaging service.
      *
      * @param applicationPort
      * Port number.
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setApplicationPort(int applicationPort)
        throws SS7InvalidParamException {
        m_applicationPort = applicationPort;
        m_applicationPortIsSet = true;
    }
    /**
      * Get the Application Port parameter.
      * It shall be checked if this parameter is present before getting
      * it.
      *
      * @return
      * Port number.
      *
      * @exception SS7ParameterNotSetException
      * Thrown if the parameter is not present.
      */
    public int getApplicationPort()
        throws SS7ParameterNotSetException {
        return m_applicationPort;
    }
    /**
      * Check if the Application Port parameter is present.
      *
      * @return
      * True or false.
      */
    public boolean isApplicationPortPresent() {
        return m_applicationPortIsSet;
    }
    protected void setMti(int mti)
        throws SS7InvalidParamException {
        switch (mti) {
            case MESSAGE_DELIVER:
            case MESSAGE_DELIVER_REPORT:
            case MESSAGE_SUBMIT:
            case MESSAGE_SUBMIT_REPORT:
            case MESSAGE_COMMAND:
            case MESSAGE_STATUS_REPORT:
                m_mti = mti;
                m_mtiIsSet = true;
                break;
            default:
                throw new SS7InvalidParamException("Message Type Indicator " + mti + " out of range.");
        }
    }
    protected int m_mti = 0;
    protected boolean m_mtiIsSet = false;
    protected int m_applicationPort = 0;
    protected boolean m_applicationPortIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
