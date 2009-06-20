/*
 @(#) $RCSfile$ $Name$($Revision$) $Date$ <p>
 
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
 
 Last Modified $Date$ by $Author$
 */

package javax.jain.protocol.ss7.map.transaction.etsipar.message;

import javax.jain.protocol.ss7.map.transaction.etsipar.*;
import javax.jain.protocol.ss7.map.transaction.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/**
  * This class represents an ETSI SMS Command Message.
  * It contains information to be used by the API implementation to
  * build the SMS-COMMAND PDU defined in the standard GSM 03.40. <p>
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
  * <li>Status Report Request (SRR) <ul>
  *
  * <li>Description: flag to indicate if a status report is requested by
  * the submitting Short Message Entity (SME).
  *
  * <li>Presence: optional (shall be present if the Command Type is
  * ENQUIRY). </ul>
  *
  * <li>sage Identification (MId) <ul>
  * <li>Description: an identifier for this message (range 0-255).  [Local between API User and API implementation; the MId may be different from the Message Reference in the SMS protocol if the API implementation performs reassembly of concatenated messages.]
  * <li>Presence: mandatory with no default value. </ul>
  *
  * <li>Protocol Identifier (PID) <ul>
  * <li>Description: indicates what type of protocol (if any) that is used for the message.
  * <li>Presence: mandatory with default value.
  * <li>Default value: PID_SME_TO_SME_IMPLICIT. </ul>
  *
  * <li>Command Type (CT) <ul>
  * <li>Description: specifies which operation that is to be performed on a Short Message (SM).
  * <li>Presence: mandatory with no default value. </ul>
  * <li>Target Message Identification (TMId) <ul>
  * <li>Description: an identifier for the message that the command shall operate on (if applicable; in case the command is NOT for a specific message a dummy value must be provided).  [Local between API User and API implementation; the TMId may be different from the Message Number in the SMS protocol if the API implementation performs reassembly of concatenated messages.]
  * <li>Presence: mandatory with no default value. </ul>
  *
  * <li>Destination Address (DA) <ul>
  * <li>Description: the destination SME of the message that the command shall operate on (if applicable; in case the command is NOT for a specific message null must be provided).
  * <li>Presence: mandatory with no default value. </ul>
  *
  * <li>Command Data (CD) <ul>
  * <li>Description: consists of command data relating to the command type.
  * <li>Presence: optional. </ul></ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class CommandMessage extends EtsiMessage {
    /**
      * The constructor.
      *
      * @param mId
      * Message Identification, an identifier for this message (range
      * 0-255).
      *
      * @param ct
      * Command Type, specifies the command (operation) that is to be
      * performed. Possible values: <ul>
      * <li>CT_ENQUIRY CT_ENQUIRY CT_ENQUIRY (setSrr(true) is called in
      * this case)
      * <li>CT_CANCEL_STATUS_REPORT
      * <li>CT_ENABLE_STATUS_REPORT
      * <li>CT_DELETE_PREVIOUS_MESSAGE </ul>
      *
      * @param tMId
      * Target Message Identification, identifies the message that the
      * command shall operate on.
      *
      * @param da
      * Destination Address, of the message that the command shall
      * operate on.
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public CommandMessage(int mId, int ct, int tMId, SS7Number da)
        throws SS7InvalidParamException {
        super(MESSAGE_COMMAND);
        setMId(mId);
        setCt(ct);
        setTMId(tMId);
        setDa(da);
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public CommandMessage()
        throws SS7InvalidParamException {
        super(MESSAGE_COMMAND);
    }
    /**
      * Set the Status Report Request flag.
      *
      * @param srr
      * True of false.
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setSrr(boolean srr)
        throws SS7InvalidParamException {
        m_srr = srr;
        m_srrIsSet = true;
    }
    /**
      * Get the Status Report Request flag.
      * It shall be checked if this parameter is present before getting
      * it.
      *
      * @return
      * True of false.
      *
      * @exception SS7ParameterNotSetException
      * Thrown if the parameter is not present.
      */
    public boolean getSrr()
        throws SS7ParameterNotSetException {
        return m_srr;
    }
    /**
      * Check if the Status Report Request flag is present.
      *
      * @return
      * True or false.
      */
    public boolean isSrrPresent() {
        return m_srrIsSet;
    }
    /**
      * Change the Message Identification for this message.
      *
      * @param mId
      * Message Identification (range 0-255).
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setMId(int mId)
        throws SS7InvalidParamException {
        m_mId = mId;
        m_mIdIsSet = true;
    }
    /**
      * Get the Message Identification for this message.
      *
      * @return
      * Message Identification (range 0-255).
      */
    public int getMId() {
        return m_mId;
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
      * Change the Command Type parameter.
      * The operation requested by the Mobile Station which is to be
      * performed at the Service Centre.
      *
      * @param ct
      * Command Type, specifies the command (operation) that is to be
      * performed. Possible values: <ul>
      * <li>CT_ENQUIRY (setSrr(true) is called in this case)
      * <li>CT_CANCEL_STATUS_REPORT
      * <li>CT_ENABLE_STATUS_REPORT
      * <li>CT_DELETE_PREVIOUS_MESSAGE </ul>
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setCt(int ct)
        throws SS7InvalidParamException {
        m_ct = ct;
        m_ctIsSet = true;
    }
    /**
      * Get the Command Type parameter.
      * The operation requested by the Mobile Station which is to be
      * performed at the Service Centre.
      *
      * @return
      * Command Type, specifies the command (operation) that is to be
      * performed. (See the setCt for possible values.)
      */
    public int getCt() {
        return m_ct;
    }
    /**
      * Change the Target Message Identification of command.
      *
      * @param tMId
      * Target Message Identification, identifies the message that the
      * command shall operate on.
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setTMId(int tMId)
        throws SS7InvalidParamException {
        m_tMId = tMId;
        m_tMIdIsSet = true;
    }
    /**
      * Get the Target Message Identification for the command.
      *
      * @return
      * Target Message Identification, identifies the message that the
      * command shall operate on.
      */
    public int getTMId() {
        return m_tMId;
    }
    /**
      * Change the Destination Address for the target message of the
      * command.
      *
      * @param da
      * Destination Address, of the message that the command shall
      * operate on.
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setDa(SS7Number da)
        throws SS7InvalidParamException {
        if (da != null) {
            m_da = da;
            m_daIsSet = true;
        } else {
            m_da = null;
            m_daIsSet = false;
        }
    }
    /**
      * Get the Destination Address for the target message of the
      * command.
      *
      * @return
      * Destination Address, of the message that the command shall
      * operate on.
      */
    public SS7Number getDa() {
        return m_da;
    }
    /**
      * Set the Command Data relating to the operation.
      *
      * @param cd
      * Command Data.
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setCd(java.lang.String cd)
        throws SS7InvalidParamException {
        if (cd != null) {
            m_cd = cd;
            m_cdIsSet = true;
        } else {
            m_cd = null;
            m_cdIsSet = false;
        }
    }
    /**
      * Get the Command Data relating to the operation.
      *
      * @return
      * Command Data. It shall be checked if this parameter is present
      * before getting it.
      *
      * @exception SS7ParameterNotSetException
      * Thrown if the parameter is not present.
      */
    public java.lang.String getCd()
        throws SS7ParameterNotSetException {
        return m_cd;
    }
    /**
      * Check if Command Data is present.
      *
      * @return
      * True or false.
      */
    public boolean isCdPresent() {
        return m_cdIsSet;
    }
    protected boolean m_srr;
    protected boolean m_srrIsSet = false;
    protected int m_mId;
    protected boolean m_mIdIsSet = false;
    protected int m_pid;
    protected boolean m_pidIsSet = false;
    protected int m_ct;
    protected boolean m_ctIsSet = false;
    protected int m_tMId;
    protected boolean m_tMIdIsSet = false;
    protected SS7Number m_da = null;
    protected boolean m_daIsSet = false;
    protected java.lang.String m_cd = null;
    protected boolean m_cdIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
