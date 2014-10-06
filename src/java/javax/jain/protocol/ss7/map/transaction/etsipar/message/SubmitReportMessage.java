/*
 @(#) $RCSfile: SubmitReportMessage.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:44 $ <p>
 
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
  * This class represents an ETSI SMS Submit Report Message.
  * It contains information to be used by the API implementation to
  * build the SMS-SUBMIT-REPORT PDU defined in the standard GSM 03.40.
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
  * <li>Failure Cause (FCS) <ul>
  *     <li>Description: indicates errors in protocol, data coding,
  *     service centre and mobile station.
  *     <li>Presence: conditional (present if error, otherwise not). </ul>
  *
  * <li>Service Centre Time Stamp (SCTS) <ul>
  *     <li>Description: time when Service Center received the message.
  *     <li>Presence: mandatory with default no value. </ul>
  *
  * <li>Protocol Identifier (PID) <ul>
  *     <li>Description: indicates what type of protocol (if any) that
  *     is used for the message.
  *     <li>Presence: optional. </ul>
  *
  * <li>Data Coding Scheme (DCS) <ul>
  *     <li>Description: coding scheme to be used for the message.
  *     <li>Presence: optional. </ul>
  *
  * <li>User Text <ul>
  *     <li>Description: the real "message".
  *     <li>Presence: optional. </ul> </ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class SubmitReportMessage extends EtsiMessage {
    /**
      * The constructor.
      *
      * @param scts
      * The time when the Service Centre received the submitted message.
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public SubmitReportMessage(Date scts)
        throws SS7InvalidParamException {
        super(MESSAGE_SUBMIT_REPORT);
        setScts(scts);
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public SubmitReportMessage()
        throws SS7InvalidParamException {
        super(MESSAGE_SUBMIT_REPORT);
    }
    /**
      * Set the Failure Cause (FCS).
      * Failure Cause is mandatory to set if there is an error. If there
      * is not an error it must not be present.
      *
      * @param fcs
      * Failure Cause. Possible values: <ul>
      * <li>FCS_INTERWORKING_NOT_SUPPORTED
      * <li>FCS_SC_BUSY
      * <li>FCS_NO_SC_SUBSCRIPTION
      * <li>FCS_INVALID_SME_ADDRESS
      * <li>FCS_ERROR_IN_MS
      * <li>... </ul>
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setFcs(int fcs)
        throws SS7InvalidParamException {
        switch (fcs) {
            case FCS_INTERWORKING_NOT_SUPPORTED:
            case FCS_SC_BUSY:
            case FCS_NO_SC_SUBSCRIPTION:
            case FCS_INVALID_SME_ADDRESS:
            case FCS_ERROR_IN_MS:
                m_fcs = fcs;
                m_fcsIsSet = true;
                break;
            default:
                throw new SS7InvalidParamException("Failure Cause " + fcs + " out of range.");
        }
    }
    /**
      * Get the Failure Cause (FCS).
      * It shall be checked if this parameter is present before getting
      * it.
      *
      * @return
      * Failure Cause. (See the setFcs for possible values.)
      *
      * @exception SS7ParameterNotSetException
      * Thrown if the parameter is not present.
      */
    public int getFcs()
        throws SS7ParameterNotSetException {
        return m_fcs;
    }
    /**
      * Check if the Failure Cause (FCS) parameter is present.
      *
      * @return
      * True or false.
      */
    public boolean isFcsPresent() {
        return m_fcsIsSet;
    }
    /**
      * Change the Service Centre Time Stamp.
      *
      * @param scts
      * The time when the Service Centre received the submitted message.
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
      * The time when the Service Centre received the message.
      */
    public Date getScts() {
        return m_scts;
    }
    /**
      * Set the Protocol Identifier.
      *
      * @param pid
      * Protocol Identifier that indicates what type of protocol (if
      * any) that is used for the message. Possible values: <ul>
      * <li>PID_SME_TO_SME_IMPLICIT
      * <li>PID_INTERWORKING_INTERNET_MAIL
      * <li>... </ul>

      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setPid(int pid)
        throws SS7InvalidParamException {
        m_pid = pid;
        m_pidIsSet = true;
    }
    /**
      * Get the Protocol Identifier.
      * It shall be checked if this parameter is present before getting
      * it.
      *
      * @return
      * Protocol Identifier that indicates what type of protocol (if
      * any) that is used for the message. (See the setPid for possible
      * values.)
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
        m_dcs = dcs;
        m_dcsIsSet = true;
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
      * Set the text message.
      *
      * @param userText
      * The text message that the user really wants to send.
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setUserText(java.lang.String userText)
        throws SS7InvalidParamException {
        m_userText = userText;
        m_userTextIsSet = true;
    }
    /**
      * Get the text message.
      *
      * @return
      * The text message that the user really wants to send. It shall be
      * checked if this parameter is present before getting it.
      *
      * @exception SS7ParameterNotSetException
      * Thrown if the parameter is not present.
      */
    public java.lang.String getUserText()
        throws SS7ParameterNotSetException {
        return m_userText;
    }
    /**
      * Check if a text messsage is present.
      *
      * @return
      * True or false.
      */
    public boolean isUserTextPresent() {
        return m_userTextIsSet;
    }
    protected int m_fcs;
    protected boolean m_fcsIsSet = false;
    protected Date m_scts = null;
    protected boolean m_sctsIsSet = false;
    protected int m_pid;
    protected boolean m_pidIsSet = false;
    protected int m_dcs;
    protected boolean m_dcsIsSet = false;
    protected java.lang.String m_userText;
    protected boolean m_userTextIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
