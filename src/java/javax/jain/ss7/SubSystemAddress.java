/*
 @(#) src/java/javax/jain/ss7/SubSystemAddress.java <p>
 
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


package javax.jain.ss7;

import javax.jain.*;

/**
  * SubsystemAddress represents an address of an SS7 User application.
  * The User Address comprises of the following mandatory parameters:
  * <ul>
  * <li>Signaling Point Code
  * <li>Sub-System Number. <ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public final class SubSystemAddress implements java.io.Serializable {
    /**
      * Constructs a SubsystemAddress with the specified Signalling
      * Point Code and Sub-System Number.
      * @param signalingPointCode
      * The Signaling Point Code of the Subsystem Address.
      * @param subSystemNumber
      * The Sub-System Number of the Subsystem Address.
      */
    public SubSystemAddress(SignalingPointCode signalingPointCode,
            short subSystemNumber) {
        super();
        setSignalingPointCode(signalingPointCode);
        setSubSystemNumber(subSystemNumber);
    }
    /**
      * Sets the SubSystem number of this SubSystem Address.
      * The SubSystem number should be greater than 0 and less than 255.
      * @param subSystemNumber
      * The Subsystem Number of this SubSystem Address.
      * @exception java.lang.IllegalArgumentException
      * Thrown if an out of range SubSystem Number is passed to this
      * method.
      */
    public void setSubSystemNumber(int subSystemNumber)
        throws java.lang.IllegalArgumentException {
        if (0 <= subSystemNumber && subSystemNumber <= 255) {
            m_subSystemNumber = subSystemNumber;
            m_subSystemNumberIsSet = true;
            return;
        }
        throw new java.lang.IllegalArgumentException("subSystemNumber: invalid value.");
    }
    /**
      * Returns the Signaling Point Code of this SubSystem Number. <p>
      *
      * <code> SSSAAAZZZ </code> <p>
      *
      * where: <p>
      *
      * <code> SSS == Member </code> <br>
      * <code> AAA == Cluster </code> <br>
      * <code> ZZZ == Zone </code> <br> <p>
      *
      * The permitted ranges for each value are outlined in the table
      * below. <p>
      *
      * <table align="center" border=1 cellpadding=1>
      * <tr>
      * <th>Variant</th>
      * <th>Member</th>
      * <th>Cluster</th>
      * <th>Zone</th>
      * </tr>
      * <tr>
      * <th>ANSI</th>
      * <td align="center">000-255</td>
      * <td align="center">000-255</td>
      * <td align="center">000-255</td>
      * </tr>
      * <tr>
      * <th>ITU</th>
      * <td align="center">000-007</td>
      * <td align="center">000-255</td>
      * <td align="center">000-007</td>
      * </tr>
      * </table>
      *
      * @return
      * The signalingPointCode of this Subsystem Address.
      */
    public SignalingPointCode getSignalingPointCode() {
        return m_signalingPointCode;
    }
    /**
      * Sets the Signaling Point Code of this SubSystem Number. <p>
      *
      * <code> SSSAAAZZZ </code> <p>
      *
      * where: <p>
      *
      * <code> SSS == Member </code> <br>
      * <code> AAA == Cluster </code> <br>
      * <code> ZZZ == Zone </code> <br> <p>
      *
      * The permitted ranges for each value are outlined in the table
      * below. <p>
      *
      * <table align="center" border=1 cellpadding=1>
      * <tr>
      * <th>Variant</th>
      * <th>Member</th>
      * <th>Cluster</th>
      * <th>Zone</th>
      * </tr>
      * <tr>
      * <th>ANSI</th>
      * <td align="center">000-255</td>
      * <td align="center">000-255</td>
      * <td align="center">000-255</td>
      * </tr>
      * <tr>
      * <th>ITU</th>
      * <td align="center">000-007</td>
      * <td align="center">000-255</td>
      * <td align="center">000-007</td>
      * </tr>
      * </table>
      *
      * @param spc
      * The signalingPointCode of this Subsystem Address.
      */
    public void setSignalingPointCode(SignalingPointCode spc) {
        m_signalingPointCode = spc;
        m_signalingPointCodeIsSet = true;
    }
    /**
      * Gets the SubSystem number of this Subsystem Address.
      * @return
      * The SubSystem number of this SubSystem Address.
      */
    public int getSubSystemNumber() {
        return m_subSystemNumber;
    }
    /**
      * java.lang.String representation of class SubSystemAddress.
      * @return
      * java.lang.String provides description of class SubSystemAddress.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.SubSystemAddress");
        b.append("\n\tm_subSystemNumber = " + m_subSystemNumber);
        b.append("\n\tm_subSystemNumberIsSet = " + m_subSystemNumberIsSet);
        b.append("\n\tm_signalingPointCode = ");
        if (m_signalingPointCode != null) b.append(m_signalingPointCode.toString());
        b.append("\n\tm_signalingPointCodeIsSet = " + m_signalingPointCodeIsSet);
        return b.toString();
    }
    protected int m_subSystemNumber = 0;
    protected boolean m_subSystemNumberIsSet = false;
    protected SignalingPointCode m_signalingPointCode = null;
    protected boolean m_signalingPointCodeIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
