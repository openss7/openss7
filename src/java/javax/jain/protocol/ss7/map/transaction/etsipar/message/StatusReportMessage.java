/*
 @(#) $RCSfile: StatusReportMessage.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:44 $ <p>
 
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

import java.util.*;
import javax.jain.protocol.ss7.map.transaction.etsipar.*;
import javax.jain.protocol.ss7.map.transaction.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/**
  * This class represents an ETSI SMS Status Report Message.
  * It contains information to be used by the API implementation to
  * build the SMS-STATUS-REPORT PDU defined in the standard GSM 03.40.
  * <p>
  *
  * The following rules applies for the setting of parameter components:
  * <ul>
  *
  * <li>Mandatory component with no default value must be provided to
  * the constructor.
  *
  * <li>Mandatory component with default value need not be set, but can
  * be changed.
  *
  * <li>Optional / conditional component is by default not present. Such
  * a component becomes present by setting it. </ul>
  *
  * <h5>Parameter components:</h5><ul>
  *
  * <li>More Messages to Send (MMS) <ul>
  *     <li>Description: indicates more messages waiting for the Mobile
  *     Station (MS) in the Service Centre (SC).
  *     <li>Presence: mandatory with default value.
  *     <li>Default value: false. </ul>
  *
  * <li>Status Report Qualifier (SRQ) <ul>
  *     <li>Description: flag to indicate if this is a status report
  *     triggered by a SUBMIT or a COMMAND.
  *     <li>Presence: mandatory with no default value. </ul>
  *
  * <li>Message Identification (MId) <ul>
  *     <li>Description: the identifier for the previously submitted
  *     message to which this status report refers (range 0-255).
  *     [Local between API User and API implementation; the MId may be
  *     different from the Message Reference in the SMS protocol if the
  *     API implementation performs reassembly of concatenated
  *     messages.]
  *     <li>Presence: mandatory with no default value. </ul>
  *
  * <li>Recipient Address (RA) <ul>
  *     <li>Description: the address of the recipient of the message to
  *     which this status report refers.
  *     <li>Presence: mandatory with no default value. </ul>
  *
  * <li>Service Centre Time Stamp (SCTS) <ul>
  *     <li>Description: time when the Service Center received the
  *     submitted message to which this status report refers.
  *     <li>Presence: mandatory with no default value. </ul>
  *
  * <li>Discharge Time (DT) <ul>
  *     <li>Description: time when the Service Center delivered/tried to
  *     deliver/deleted the message to which this status report refers.
  *     <li>Presence: mandatory with no default value. </ul>
  *
  * <li>Status (ST) <ul>
  *     <li>Description: status of the message to which this status
  *     report refers.
  *     <li>Presence: mandatory with no default value. </ul>
  *
  * <li>Protocol Identifier (PID) <ul>
  *     <li>Description: indicates what type of protocol (if any) that
  *     was used in the message to which this status report refers.
  *     <li>Presence: optional. </ul>
  *
  * <li>Data Coding Scheme (DCS) <ul>
  *     <li>Description: coding scheme to be used for the message.
  *     <li>Presence: optional. </ul>
  *
  * <li>User Text <ul>
  *     <li>Description: data from the receiver of the originally
  *     submitted message and/or data inserted by the Service Centre.
  *     <li>Presence: optional. </ul> </ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class StatusReportMessage extends EtsiMessage {
    /**
      * The constructor.
      *
      * @param srq
      * Status Report Qualifier, indicating if this status report was
      * triggered by a SUBMIT or a COMMAND. Possible values: <ul>
      * <li>SRQ_RESULT_OF_SUBMIT
      * <li>SRQ_RESULT_OF_COMMAND </ul>
      *
      * @param mId
      * Message Identification, the id for the message to which this
      * status report refers (range 0-255).
      *
      * @param ra
      * Recipient Address, the address of the recipient of the message
      * to which this status report refers.
      *
      * @param scts
      * Time when the SC received the message to which this status
      * report refers.
      *
      * @param dt
      * Discharge Time, the latest time when the SC acted on the
      * referred message (delivered/tried to deliver/deleted).
      *
      * @param st
      * Status of the the message to which this status report refers.
      * Possible values: <ul>
      * <li>ST_COMPLETED_MESSAGE_DELIVERED
      * <li>ST_COMPLETED_DELIVERY_NOT_CONFIRMED
      * <li>ST_COMPLETED_MESSAGE_REPLACED
      * <li>ST_TEMP_ERROR_CONGESTION
      * <li>ST_TEMP_ERROR_RECEIVER_BUSY
      * <li>ST_TEMP_ERROR_RECEIVER_NOT_RESPONDING
      * <li>ST_ERROR_REMOTE_PROCEDURE_ERROR
      * <li>ST_ERROR_INCOMPATIBLE_DESTINATION
      * <li>... </ul>
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public StatusReportMessage(boolean srq, int mId, SS7Number ra,
            Date scts, Date dt, int st)
        throws SS7InvalidParamException {
        super(MESSAGE_STATUS_REPORT);
        setSrq(srq);
        setMId(mId);
        setRa(ra);
        setScts(scts);
        setDt(dt);
        setSt(st);
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public StatusReportMessage()
        throws SS7InvalidParamException {
        super(MESSAGE_STATUS_REPORT);
    }
    /**
      * Change the More Messages to Send flag.
      *
      * @param mms
      * True of false.
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setMms(boolean mms)
        throws SS7InvalidParamException {
        m_mms = mms;
        m_mmsIsSet = true;
    }
    /**
      * Get the More Messages to Send flag.
      *
      * @return
      * True of false.
      */
    public boolean getMms() {
        return m_mms;
    }
    /**
      * Change the Status Report Qualifier flag.
      *
      * @param srq
      * Status Report Qualifier. (See the constructor for possible
      * values.)
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setSrq(boolean srq)
        throws SS7InvalidParamException {
        m_srq = srq;
        m_srqIsSet = true;
    }
    /**
      * Get the Status Report Qualifier flag.
      *
      * @return
      * Status Report Qualifier. (See the constructor for possible
      * values.)
      */
    public boolean getSrq() {
        return m_srq;
    }
    /**
      * Change the Message Identification.
      * 
      * @param mId
      * Message Identification, the id for the message to which this
      * status report refers (range 0-255).
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setMId(int mId)
        throws SS7InvalidParamException {
        if (0 <= mId && mId <= 255) {
            m_mId = mId;
            m_mIdIsSet = true;
            return;
        }
        throw new SS7InvalidParamException("Message Identification value " + mId + " out of range.");
    }
    /**
      * Get the Message Identification.
      *
      * @return
      * Message Identification, the id for the message to which this
      * status report refers (range 0-255).
      */
    public int getMId() {
        return m_mId;
    }
    /**
      * Change the Recipient Address.
      *
      * @param ra
      * Recipient Address, the address of the recipient of the message
      * to which this status report refers.
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setRa(SS7Number ra)
        throws SS7InvalidParamException {
        if (ra != null) {
            m_ra = ra;
            m_raIsSet = true;
        } else {
            m_ra = null;
            m_raIsSet = false;
        }
    }
    /**
      * Get the Recipient Address.
      *
      * @return
      * Recipient Address, the address of the recipient of the message
      * to which this status report refers.
      */
    public SS7Number getRa() {
        return m_ra;
    }
    /**
      * Change the Service Centre Time Stamp.
      *
      * @param scts
      * Time when the SC received the message to which this status
      * report refers.
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setScts(Date scts)
        throws SS7InvalidParamException {
        if (scts != null) {
            m_scts = scts;
            m_sctsIsSet = true;
        } else {
            m_scts = null;
            m_sctsIsSet = false;
        }
    }
    /**
      * Get the Service Centre Time Stamp.
      *
      * @return
      * Time when the SC received the message to which this status
      * report refers.
      */
    public Date getScts() {
        return m_scts;
    }
    /**
      * Change the Discharge Time.
      *
      * @param dt
      * Discharge Time, the latest time when the SC acted on the
      * referred message (delivered/tried to deliver/deleted).
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setDt(Date dt)
        throws SS7InvalidParamException {
        if (dt != null) {
            m_dt = dt;
            m_dtIsSet = true;
        } else {
            m_dt = null;
            m_dtIsSet = false;
        }
    }
    /**
      * Get the Discharge Time.
      *
      * @return
      * Discharge Time, the latest time when the SC acted on the
      * referred message (delivered/tried to deliver/deleted).
      */
    public Date getDt() {
        return m_dt;
    }
    /**
      * Change the Status parameter.
      *
      * @param st
      * Status of the the message to which this status report refers.
      * (See the constructor for possible values.)
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setSt(int st)
        throws SS7InvalidParamException {
        m_st = st;
        m_stIsSet = true;
    }
    /**
      * Get the Status parameter.
      *
      * @return
      * Status of the the message to which this status report refers.
      * (See the constructor for possible values.)
      */
    public int getSt() {
        return m_st;
    }
    /**
      * Set the Protocol Identifier.
      *
      * @param pid
      * Protocol Identifier that indicates what type of protocol (if
      * any) that was used in the message to which this status report
      * refers. Possible values: <ul>
      * <li>PID_SME_TO_SME_IMPLICIT
      * <li>PID_INTERWORKING_INTERNET_MAIL
      * <li>... </ul>
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setPid(int pid)
        throws SS7InvalidParamException {
        switch (pid) {
            case PID_SME_TO_SME_IMPLICIT:
            case PID_INTERWORKING_INTERNET_MAIL:
                m_pid = pid;
                m_pidIsSet = true;
                break;
            default:
                throw new SS7InvalidParamException("Protocol Identifier " + pid + " out of range.");
        }
    }
    /**
      * Get the Protocol Identifier.
      * It shall be checked if this parameter is present before getting
      * it.
      *
      * @return
      * Protocol Identifier that indicates what type of protocol (if
      * any) that was used in the message to which this status report
      * refers. Possible values: (See the setPid for possible values.)
      *
      * @exception SS7ParameterNotSetException
      * Thrown if the parameter is not present.
      */
    public int getPid()
        throws SS7ParameterNotSetException {
        return m_pid;
    }
    /**
      * Check if the Protocol Identifier is present.
      *
      * @return
      * True or false.
      */
    public boolean isPidPresent() {
        return m_pidIsSet;
    }
    /**
      * Set the Data Coding Scheme.
      *
      * @param dcs
      * Data Coding Scheme to be used for the user data in this message.
      * Possible values: <ul>
      * <li>DCS_7_BIT_DEFAULT_ALPHABET
      * <li>... </ul>
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setDcs(int dcs)
        throws SS7InvalidParamException {
        switch (dcs) {
            case DCS_7_BIT_DEFAULT_ALPHABET:
                m_dcs = dcs;
                m_dcsIsSet = true;
                break;
            default:
                throw new SS7InvalidParamException("Data Coding Schemem " + dcs + " out of range.");
        }
    }
    /**
      * Get the Data Coding Scheme.
      * It shall be checked if this parameter is present before getting
      * it.
      *
      * @return
      * Data Coding Scheme to be used for the user data in this message.
      * (See the setDcs for possible values.)
      *
      * @exception SS7ParameterNotSetException
      * Thrown if the parameter is not present.
      */
    public int getDcs()
        throws SS7ParameterNotSetException {
        return m_dcs;
    }
    /**
      * Check if the Data Coding Scheme parameter is present.
      *
      * @return
      * True or false.
      */
    public boolean isDcsPresent() {
        return m_dcsIsSet;
    }
    /**
      * Set the users text message.
      *
      * @param userText
      * Message containing data from the receiver of the originally
      * submitted message and/or data inserted by the Service Centre.
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setUserText(java.lang.String userText)
        throws SS7InvalidParamException {
        if (userText != null) {
            m_userText = userText;
            m_userTextIsSet = true;
        } else {
            m_userText = null;
            m_userTextIsSet = false;
        }
    }
    /**
      * Get the users text message.
      *
      * @return
      * Message containing data from the receiver of the originally
      * submitted message and/or data inserted by the Service Centre. It
      * shall be checked if this parameter is present before getting it.
      *
      * @exception SS7ParameterNotSetException
      * Thrown if the parameter is not present.
      */
    public java.lang.String getUserText()
        throws SS7ParameterNotSetException {
        return m_userText;
    }
    /**
      * Check if a text message is present.
      *
      * @return
      * True or false.
      */
    public boolean isUserTextPresent() {
        return m_userTextIsSet;
    }
    protected boolean m_mms;
    protected boolean m_mmsIsSet = false;
    protected boolean m_srq;
    protected boolean m_srqIsSet = false;
    protected int m_mId;
    protected boolean m_mIdIsSet = false;
    protected SS7Number m_ra = null;
    protected boolean m_raIsSet = false;
    protected Date m_scts = null;
    protected boolean m_sctsIsSet = false;
    protected Date m_dt = null;
    protected boolean m_dtIsSet = false;
    protected int m_st;
    protected boolean m_stIsSet = false;
    protected int m_pid;
    protected boolean m_pidIsSet = false;
    protected int m_dcs;
    protected boolean m_dcsIsSet = false;
    protected java.lang.String m_userText;
    protected boolean m_userTextIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
