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

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing the ISUP ANSI Precedence parameter and ITU MLPP Precedence
  * parameter.
  * This class is common to ITU and the ANSI variants of the parameter.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class Precedence implements java.io.Serializable {
    public static final byte LFB_ALLOWED = 0;
    public static final byte LFB_PATH_RESERVED = 2;
    public static final byte LFB_NOT_ALLOWED = 4;
    public static final byte PL_FLASH_OVERIDE = 0;
    public static final byte PL_FLASH = 1;
    public static final byte PL_IMMEDIATE = 2;
    public static final byte PL_PRIORITY = 3;
    public static final byte PL_ROUTINE = 4;
    /** Constructs a new Precedence class, parameters with default values.  */
    public Precedence() {
    }
    /** Constructs a new Precedence class from the input parameters specified.
      * @param in_precLevel  The precedence level, range 0 to 5 <ul> <li>PL_FLASH_OVERIDE,
      * <li>PL_FLASH, <li>PL_IMMEDIATE, <li>PL_PRIORITY and <li>PL_ROUTINE. </ul>
      * @param in_lfb  The look ahead for busy, range 0 to 3 <ul> <li>LFB_ALLOWED,
      * <li>LFB_PATH_RESERVED and <li>LFB_NOT_ALLOWED. </ul>
      * @param in_nwId  The network identity, maximum array size is 4.
      * @param in_mlppServDomain  The MLPP service domain, range 0 to 0xffffff.
      * @exception ParameterRangeInvalidException  thrown when value is out of range.
      */
    public Precedence(byte in_precLevel, byte in_lfb, byte[] in_nwId,
            int in_mlppServDomain)
        throws ParameterRangeInvalidException {
        this.setPrecedenceLevel(in_precLevel);
        this.setLookAheadForBusyInd(in_lfb);
        this.setNetworkId(in_nwId);
        this.setMLPPServiceDomain(in_mlppServDomain);
    }
    /** Gets the Precedence Level field of the parameter.
      * @return Byte the PrecedenceLevel value, range 0 to 15, see Precedence().
      */
    public byte getPrecedenceLevel() {
        return m_precedenceLevel;
    }
    /** Sets the Precedence Level field of the parameter.
      * @param aPrecedenceLevel  The PrecedenceLevel value, range 0 to 15, see
      * Precedence().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setPrecedenceLevel(byte aPrecedenceLevel)
        throws ParameterRangeInvalidException {
        if (0 <= aPrecedenceLevel && aPrecedenceLevel <= 15) {
            m_precedenceLevel = aPrecedenceLevel;
            return;
        }
        throw new ParameterRangeInvalidException("PrecedenceLevel value " + aPrecedenceLevel + " out of range.");
    }
    /** Gets the Look Ahead for Busy field of the parameter.
      * @return Byte the Look Ahead for Busy value, range 0 to 3, see Precedence().
      */
    public byte getLookAheadForBusyInd() {
        return m_lookAheadForBusy;
    }
    /** Sets the Look Ahead for Busy field of the parameter.
      * @param aLookAheadForBusy  The Look Ahead For Busy value, range 0 to 3, see
      * Precedence().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setLookAheadForBusyInd(byte aLookAheadForBusy)
        throws ParameterRangeInvalidException {
        if (0 <= aLookAheadForBusy && aLookAheadForBusy <= 3) {
            m_lookAheadForBusy = aLookAheadForBusy;
            return;
        }
        throw new ParameterRangeInvalidException("LookAheadForBusyInd value " + aLookAheadForBusy + " out of range.");
    }
    /** Gets the Network Identity field of the parameter.
      * @return The Network Identity Digit array. The digits are coded in binary format.
      * Array size is 4.
      */
    public byte[] getNetworkId() {
        return m_networkId;
    }
    /** Sets the NI Digit field of the parameter.
      * @param networkId  Network identity digits. The digits array is coded in binary
      * format and the maximum size of the array is 4.
      */
    public void setNetworkId(byte[] networkId) {
        m_networkId = networkId;
    }
    /** Gets the MLPP Service Domain field of the parameter.
      * @return The MLPP Service Domain, range 0 to 0xffffff.
      */
    public int getMLPPServiceDomain() {
        return m_MLPPServiceDomain;
    }
    /** Sets the MLPP Service Domain field of the parameter.
      * @param aMLPPServiceDomain  The MLPP Service Domain, range 0 to 0xffffff.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setMLPPServiceDomain(int aMLPPServiceDomain)
        throws ParameterRangeInvalidException {
        if (0 <= aMLPPServiceDomain && aMLPPServiceDomain <= 0xffffff) {
            m_MLPPServiceDomain = aMLPPServiceDomain;
            return;
        }
        throw new ParameterRangeInvalidException("MLPPServiceDomain value " + aMLPPServiceDomain + " out of range.");
    }
    /** The toString method retrieves a string containing the values of the members of the
      * Precedence class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.Precedence");
        b.append("\n\tm_precedenceLevel: " + m_precedenceLevel);
        b.append("\n\tm_lookAheadForBusy: " + m_lookAheadForBusy);
        b.append("\n\tm_networkId: " + JainSS7Utility.bytesToHex(m_networkId, 0, m_networkId.length));
        b.append("\n\tm_MLPPServiceDomain: " + m_MLPPServiceDomain);
        return b.toString();
    }
    protected byte m_precedenceLevel;
    protected byte m_lookAheadForBusy;
    protected byte[] m_networkId;
    protected int m_MLPPServiceDomain;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
