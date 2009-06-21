/*
 @(#) $RCSfile: SmsPdu.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:44 $ <p>
 
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

package javax.jain.protocol.ss7.map.transaction.etsipar.pdu;

import javax.jain.protocol.ss7.map.transaction.etsipar.*;
import javax.jain.protocol.ss7.map.transaction.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/**
  * This class represents an ETSI SMS Protocol Data Units (PDU) encoded
  * according to the GSM 03.40 standard. <p>
  *
  * The ETSI PDU:s shall be to be used when the JAIN MAP API is
  * implemented below a layer handling the ETSI SMS protocol according
  * to GSM 03.40.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class SmsPdu extends Message {
    /** Identifier for the ETSI DELIVER PDU.  */
    public static final int PDU_DELIVER = 1;
    /** Identifier for the ETSI DELIVER REPORT PDU.  */
    public static final int PDU_DELIVER_REPORT = 2;
    /** Identifier for the ETSI SUBMIT PDU.  */
    public static final int PDU_SUBMIT = 3;
    /** Identifier for the ETSI SUBMIT REPORT PDU.  */
    public static final int PDU_SUBMIT_REPORT = 4;
    /** Identifier for the ETSI COMMAND PDU.  */
    public static final int PDU_COMMAND = 5;
    /** Identifier for the ETSI STATUS PDU.  */
    public static final int PDU_STATUS_REPORT = 6;
    /**
      * The constructor.
      *
      * @param pti
      * PDU Type Indicator, that identifies the PDU.  Possible values: <ul>
      * <li>PDU_DELIVER
      * <li>PDU_DELIVER_REPORT
      * <li>PDU_SUBMIT
      * <li>PDU_SUBMIT_REPORT
      * <li>PDU_COMMAND
      * <li>PDU_STATUS_REPORT </ul>
      *
      * @param pdu
      * A byte array containing an encoded SMS PDU.
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public SmsPdu(int pti, byte[] pdu)
        throws SS7InvalidParamException {
        setPti(pti);
        setPdu(pdu);
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public SmsPdu()
        throws SS7InvalidParamException {
    }
    /**
      * Change the PDU Type Indicator parameter.
      *
      * @param pti
      * PDU Type Indicator. (See the constructor for possible values.)
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setPti(int pti)
        throws SS7InvalidParamException {
        switch(pti) {
            case PDU_DELIVER:
            case PDU_DELIVER_REPORT:
            case PDU_SUBMIT:
            case PDU_SUBMIT_REPORT:
            case PDU_COMMAND:
            case PDU_STATUS_REPORT:
                m_pti = pti;
                m_ptiIsSet = true;
                break;
            default:
                throw new SS7InvalidParamException("PDU Type Indicator " + pti + " out of range.");
        }
    }
    /**
      * Get the PDU Type Indicator parameter.
      *
      * @return
      * PDU Type Indicator. (See the constructor for possible values.)
      */
    public int getPti() {
        return m_pti;
    }
    /**
      * Change the PDU.
      *
      * @param pdu
      * A byte array containing an encoded SMS PDU.
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setPdu(byte[] pdu)
        throws SS7InvalidParamException {
        // FIXME: check for validity
        if (pdu != null) {
            m_pdu = pdu;
            m_pduIsSet = true;
        } else {
            m_pdu = null;
            m_pduIsSet = false;
        }
    }
    /**
      * Get the PDU.
      *
      * @return
      * A byte array containing an encoded SMS PDU.
      */
    public byte[] getPdu() {
        return m_pdu;
    }
    protected int m_pti = 0;
    protected boolean m_ptiIsSet = false;
    protected byte[] m_pdu = null;
    protected boolean m_pduIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
