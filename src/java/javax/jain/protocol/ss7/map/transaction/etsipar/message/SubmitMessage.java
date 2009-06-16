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

import java.util.*;
import javax.jain.protocol.ss7.map.transaction.etsipar.*;
import javax.jain.protocol.ss7.map.transaction.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/**
  * This class represents an ETSI SMS Submit Message.
  * It contains information to be used by the API implementation to
  * build the SMS-SUBMIT PDU defined in the standard GSM 03.40. <p>
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
  * <li>Reject Duplicates (RD) <ul>
  *     <li>Description: flag to indicate if the Service Centre shall
  *     discard duplicates.
  *     <li>Presence: mandatory with default value.
  *     <li>Default value: true. </ul>
  *
  * <li>Reply Path (RP) <ul>
  *     <li>Description: flag to indicate if a Reply Path is requested
  *     to be set up.
  *     <li>Presence: mandatory with default value.
  *     <li>Default value: false. </ul>
  *
  * <li>Status Report Request (SRR) <ul>
  *     <li>Description: flag to indicate if a status report is
  *     requested by the submitting Short Message Entity (SME).
  *     <li>Presence: optional.
  *
  * <li>Message Identification (MId) <ul>
  *     <li>Description: an identifier for this message (range 0-255).
  *     [Local between API User and API implementation; the MId may be
  *     different from the Message Reference in the SMS protocol if the
  *     API implementation performs reassembly of concatenated
  *     messages.]
  *     <li>Presence: mandatory with no default value. </ul>
  *
  * <li>Destination Address (DA) <ul>
  *     <li>Description: address of the destination SME.
  *     <li>Presence: mandatory with no default value. </ul>
  *
  * <li>Protocol Identifier (PID) <ul>
  *     <li>Description: indicates what type of protocol (if any) that
  *     is used for the message.
  *     <li>Presence: mandatory with default value.
  *     <li>Default value: PID_SME_TO_SME_IMPLICIT. </ul>
  *
  * <li>Data Coding Scheme (DCS) <ul>
  *     <li>Description: coding scheme to be used for the message.
  *     <li>Presence: mandatory with default value.
  *     <li>Default value: DCS_7_BIT_DEFAULT_ALPHABET. </ul>
  *
  * <li>Validity Period (VP) <ul>
  *     <li>Description: validity period for the message from when the
  *     service center receives the message (including format
  *     information). When set it can be expressed in absoulte format or
  *     in relative format. It is stored in absolute format. amd when
  *     get it is expressed in absolute format.
  *     <li>Presence: optonal. </ul>
  *
  * <li>User Text <ul>
  *     <li>Description: the real "message".
  *     <li>Presence: optional. </ul></ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class SubmitMessage extends EtsiMessage {
    /**
      * The constructor.
      *
      * @param da
      * Destination Address, the address of the destination Short
      * Message Entity (SME).
      *
      * @param mId
      * Message Identification, an identifier for this message (range
      * 0-255).
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public SubmitMessage(SS7Number da, int mId)
        throws SS7InvalidParamException {
        super(MESSAGE_SUBMIT);
        setDa(da);
        setMId(mId);
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public SubmitMessage()
        throws SS7InvalidParamException {
        super(MESSAGE_SUBMIT);
    }
    /**
      * Change the Reject Duplicates flag.
      *
      * @param rd
      * True of false.
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setRd(boolean rd)
        throws SS7InvalidParamException {
        m_rd = rd;
        m_rdIsSet = true;
    }
    /**
      * Get the Reject Duplicates flag.
      *
      * @return
      * True of false.
      */
    public boolean getRd() {
        return m_rd;
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
        if (0 <= mId && mId <= 255) {
            m_mId = mId;
            m_mIdIsSet = true;
            return;
        }
        throw new SS7InvalidParamException("Message Identification " + mId + " out of range.");
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
      * Change the Destination Address.
      *
      * @param da
      * Destination Address, the address of the destination Short
      * Message Entity (SME).
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
      * Get the Destination Address.
      *
      * @return
      * Destination Address, the address of the destination Short
      * Message Entity (SME).
      */
    public SS7Number getDa() {
        return m_da;
    }
    /**
      * Change the Protocol Identifier.
      *
      * @param pid
      * Protocol Identifier that indicates what type of protocol (if
      * any) that is used for the message. Possible values:  <ul>
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
        switch (dcs) {
            case DCS_7_BIT_DEFAULT_ALPHABET:
                m_dsc = dcs;
                m_dscIsSet = true;
                break;
            default:
                throw new SS7InvalidParamException("Data Coding Scheem " + dcs + " out of range.");
        }
    }
    /**
      * Get the Data Coding Scheme.
      *
      * @return
      * Data Coding Scheme to be used for the user data in this message.
      * (See the setDcs for possible values.)
      */
    public int getDcs() {
        return m_dsc;
    }
    /**
      * Set validity period, absolute format.
      *
      * @param vp
      * An instance of Date identifying date and time when validity
      * period expires ("absolute format").
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setVp(Date vp)
        throws SS7InvalidParamException {
        if (vp != null) {
            m_vp = vp;
            m_vpIsSet = true;
        } else {
            m_vp = null;
            m_vpIsSet = false;
        }
    }
    /**
      * Set validity period, relative format.
      * Total validity period will be weeks+days+hours+minutes. Any
      * parameter takes an integer &gt; 0. Parameter values less or
      * equal to 0 will be interpreted as 0.
      *
      * @param weeks
      * Weeks of validity period.
      *
      * @param days
      * Days of validity period.
      *
      * @param hours
      * Hours of validity period.
      *
      * @param minutes
      * Minutes of validity period.
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setVp(int weeks, int days, int hours, int minutes)
        throws SS7InvalidParamException {
        // FIXME
    }
    /**
      * Get validity period, absolute format.
      * It shall be checked if this parameter is present before getting
      * it.
      *
      * @return
      * An instance of Date identifying date and time when validity
      * period expires ("absolute format").
      *
      * @exception SS7ParameterNotSetException
      * Thrown if the parameter is not present.
      */
    public Date getVp()
        throws SS7ParameterNotSetException {
        return m_vp;
    }
    /**
      * Check if the Validity Period parameter is present.
      *
      * @return
      * True or false.
      */
    public boolean isVpPresent() {
        return m_vpIsSet;
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
      * @return True or false.  
      */
    public boolean isUserTextPresent() {
        return m_userTextIsSet;
    }
        protected boolean m_rd;
        protected boolean m_rdIsSet = false;
        protected boolean m_rp;
        protected boolean m_rpIsSet = false;
        protected boolean m_srr;
        protected boolean m_srrIsSet = false;
        protected int m_mId;
        protected boolean m_mIdIsSet = false;
        protected SS7Number m_da = null;
        protected boolean m_daIsSet = false;
        protected int m_pid;
        protected boolean m_pidIsSet = false;
        protected int m_dsc;
        protected boolean m_dscIsSet = false;
        protected Date m_vp = null;
        protected boolean m_vpIsSet = false;
        protected String m_userText;
        protected boolean m_userTextIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
