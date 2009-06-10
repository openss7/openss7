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

package javax.jain.ss7.isup.ansi;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing the ANSI ISUP Transaction Request parameter.
    This class provides the methods to access the sub-fields of this parameter.

    @author Monavacon Limited
    @version 1.2.2
  */
public class TransactionReqAnsi implements java.io.Serializable {
    /** Constructs a new ANSI TransactionReq class, parameters with default values.  */
    public TransactionReqAnsi() {
    }
    /** Constructs an ANSI TransactionReq class from the input parameters specified.
        @param in_transId  The transaction id coded as per T1.114.3 ANSI specifications
        for TCAP.
        @param in_sccpAddr  The SCCP Address coded as given in ANSI SCCP specifications,
        T1.112.3.
        @exception ParameterRangeInvalidException  thrown when value is out of range.
      */
    public TransactionReqAnsi(long in_transId, byte[] in_sccpAddr)
        throws ParameterRangeInvalidException {
        this.setTransactionId(in_transId);
        this.setSCCPAddress(in_sccpAddr);
    }
    /** Gets the Transaction ID of the parameter.
        @return The Transaction Id, the byte array containing the transaction id coded as
        per T1.114.3 ANSI specifications for TCAP.
      */
    public long getTransactionId() {
        return m_transactionId;
    }
    /** Sets the Transaction ID of the parameter.
        @param aTransactionId  the byte array containing the transaction id coded as per
        T1.114.3 ANSI specifications for TCAP.
      */
    public void setTransactionId(long aTransactionId)
        throws ParameterRangeInvalidException {
        m_transactionId = aTransactionId;
    }
    /** Gets the SCCP Address field of the parameter.
        @return The SCCP Address, byte array to be coded as given in ANSI SCCP
        specifications, T1.112.3.
      */
    public byte[] getSCCPAddress() {
        return m_sccpAddress;
    }
    /** Sets the SCCP Address field of the parameter.
        @param aSCCPAddress  the SCCP Address byte array to be coded as given in ANSI SCCP
        specifications, T1.112.3.
      */
    public void setSCCPAddress(byte[] aSCCPAddress) {
        m_sccpAddress = aSCCPAddress;
    }
    /** The toString method retrieves a string containing the values of the members of the
        TransactionReqAnsi class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.ansi.TransactionRequest");
        b.append("\n\tm_transactionId: " + m_transactionId);
        b.append("\n\tm_sccpAddress: " + JainSS7Utility.bytesToHex(m_sccpAddress, 0, m_sccpAddress.length));
        return b.toString();
    }
    protected long m_transactionId;
    protected byte[] m_sccpAddress;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
