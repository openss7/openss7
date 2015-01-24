/*
 @(#) src/java/javax/jain/protocol/ss7/map/transaction/etsipar/message/DeliverMessage.java <p>
 
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

package javax.jain.protocol.ss7.map.transaction.etsipar.message;

import java.util.*;
import javax.jain.protocol.ss7.map.transaction.etsipar.*;
import javax.jain.protocol.ss7.map.transaction.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/**
  * This class represents an ETSI SMS Deliver Message.
  * It contains information to be used by the API implementation to
  * build the SMS-DELVIER PDU defined in the standard GSM 03.40. <p>
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
  * <li>More Messages to Send (MMS) <ul>
  *
  *     <li>Description: indicates more messages waiting for the Mobile
  *     Station (MS) in the Service Centre (SC).
  *     <li>Presence: mandatory with default value.
  *     <li>Default value: false. </ul>
  *
  * <li>Reply Path (RP) <ul>
  *
  *     <li>Description: flag to indicate if a Reply Path has been set
  *     up.
  *     <li>Presence: mandatory with default value.
  *     <li>Default value: false. </ul>
  *
  * <li>Status Report Indication (SRI) <ul>
  *
  *     <li>Description: flag to indicate if a status report shall be
  *     returned to the originating Short Message Entity (SME).
  *     <li>Presence: optional. </ul>
  *
  * <li>Originating Address (OA) <ul>
  *
  *     <li>Description: address of the originating SME.
  *     <li>Presence: mandatory with no default value. </ul>
  *
  * <li>Protocol Identifier (PID) <ul>
  *
  *     <li>Description: indicates what type of protocol (if any) that
  *     is used for the message.
  *     <li>Presence: mandatory with default value.
  *     <li>Default value: PID_SME_TO_SME_IMPLICIT. </ul>
  *
  * <li>Data Coding Scheme (DCS) <ul>
  *
  *     <li>Description: coding scheme to be used for the message.
  *     <li>Presence: mandatory with default value.
  *     <li>Default value: DCS_7_BIT_DEFAULT_ALPHABET. </ul>
  *
  * <li>Service Centre Time Stamp (SCTS) <ul>
  *
  *     <li>Description: time when the Service Center received the
  *     submitted message.
  *     <li>Presence: mandatory with default no value. </ul>
  *
  * <li>User Text <ul>
  *
  *     <li>Description: the real "message".
  *     <li>Presence: optional.</ul> </ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class DeliverMessage extends EtsiMessage {
    /**
      * The constructor.
      *
      * @param oa
      * Originating Address, the address of the originating Short
      * Message Entity (SME).
      *
      * @param scts
      * The time when the Service Centre received the submitted message.
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public DeliverMessage(SS7Number oa, Date scts)
        throws SS7InvalidParamException {
        super(MESSAGE_DELIVER);
        setOa(oa);
        setScts(scts);
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public DeliverMessage()
        throws SS7InvalidParamException {
        super(MESSAGE_DELIVER);
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
      * Change the Reply Path flag.
      *
      * @param rp
      * True of false.
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setRp(boolean rp)
        throws SS7InvalidParamException {
        m_rp = rp;
        m_rpIsSet = true;
    }
    /**
      * Get the Reply Path flag.
      *
      * @return
      * True of false.
      */
    public boolean getRp() {
        return m_rp;
    }
    /**
      * Set the Status Report Indication flag.
      *
      * @param sri
      * True of false.
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setSri(boolean sri)
        throws SS7InvalidParamException {
        m_sri = sri;
        m_sriIsSet = true;
    }
    /**
      * Get the Status Report Indication flag.
      * It shall be checked if this parameter is present before getting
      * it.
      *
      * @return
      * True of false.
      *
      * @exception SS7ParameterNotSetException
      * Thrown if the parameter is not present.
      */
    public boolean getSri()
        throws SS7ParameterNotSetException {
        return m_sri;
    }
    /**
      * Check if the Status Report Indication flag is present.
      *
      * @return
      * True or false.
      */
    public boolean isSriPresent() {
        return m_sriIsSet;
    }
    /**
      * Change the Originating Address.
      *
      * @param oa
      * Originating Address, the address of the originating Short
      * Message Entity (SME).
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setOa(SS7Number oa)
        throws SS7InvalidParamException {
        if (oa != null) {
            m_oa = oa;
            m_oaIsSet = true;
        } else {
            m_oa = null;
            m_oaIsSet = false;
        }
    }
    /**
      * Get the Originating Address.
      *
      * @return
      * Originating Address, the address of the originating Short
      * Message Entity (SME).
      */
    public SS7Number getOa() {
        return m_oa;
    }
    /**
      * Change the Protocol Identifier.
      *
      * @param pid
      * Protocol Identifier that indicates what type of protocol (if
      * any) that is used for the message. Possible values: <ul>
      * <li>PID_SME_TO_SME_IMPLICIT
      * <li>PID_INTERWORKING_INTERNET_MAIL
      * <li>... </ul>
      *
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
      *
      * @return
      * Protocol Identifier that indicates what type of protocol (if
      * any) that is used for the message. (See the setPid for possible
      * values.)
      */
    public int getPid() {
        return m_pid;
    }
    /**
      * Change the Data Coding Scheme.
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
      *
      * @return
      * Data Coding Scheme to be used for the user data in this message.
      * (See the setDcs for possible values.)
      */
    public int getDcs() {
        return m_dcs;
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
        if (userText != null) {
            m_userText = userText;
            m_userTextIsSet = true;
        } else {
            m_userText = null;
            m_userTextIsSet = false;
        }
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
    protected boolean m_mms;
    protected boolean m_mmsIsSet = false;
    protected boolean m_rp;
    protected boolean m_rpIsSet = false;
    protected boolean m_sri;
    protected boolean m_sriIsSet = false;
    protected SS7Number m_oa = null;
    protected boolean m_oaIsSet = false;
    protected int m_pid;
    protected boolean m_pidIsSet = false;
    protected int m_dcs;
    protected boolean m_dcsIsSet = false;
    protected Date m_scts;
    protected boolean m_sctsIsSet = false;
    protected java.lang.String m_userText;
    protected boolean m_userTextIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
