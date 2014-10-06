/*
 @(#) $RCSfile: InfoReqInd.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-09-02 08:46:47 $ <p>
 
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
 
 Last Modified $Date: 2011-09-02 08:46:47 $ by $Author: brian $
 */

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing an ISUP Information Request Indicators.
  * This core class provides access methods for all the common sub-fields in ITU and
  * ANSI variant. ITU variant has no additional sub-fields. The ANSI class inherits
  * from this core class to add the ANSI specific sub-fields.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class InfoReqInd implements java.io.Serializable {
    public static final boolean CPARI_CALLING_PARTY_ADDRESS_NOT_REQUESTED = false;
    public static final boolean CPARI_CALLING_PARTY_ADDRESS_REQUESTED = true;
    public static final boolean HPI_HOLD_NOT_REQUESTED = false;
    public static final boolean HPI_HOLD_REQUESTED = true;
    public static final boolean CPCRI_CALLING_PARTY_CATEGORY_NOT_REQUESTED = false;
    public static final boolean CPCRI_CALLING_PARTY_CATEGORY_REQUESTED = true;
    public static final boolean CIRI_CHARGE_INFORMATION_NOT_REQUESTED = false;
    public static final boolean CIRI_CHARGE_INFORMATION_REQUESTED = true;
    public static final boolean MCIRI_MALICIOUS_CALL_IDENTIFICATION_NOT_REQUESTED = false;
    public static final boolean MCIRI_RESERVED_FOR_MCIRI_MALICIOUS_CALL_IDENTIFICATION_REQUESTED = true;
    /** Constructs a new InfoReqInd class, parameters with default values.  */
    public InfoReqInd() {
    }
    /** Constructs a InfoReqInd class from the input parameters specified.
      * @param in_callingAddrReqInd  The calling party address request indicator; <ul>
      * <li>CPARI_CALLING_PARTY_ADDRESS_NOT_REQUESTED and
      * <li>CPARI_CALLING_PARTY_ADDRESS_REQUESTED. </ul>
      * @param in_holdingInd  The holding indicator; <ul> <li>HPI_HOLD_NOT_REQUESTED and
      * <li>HPI_HOLD_REQUESTED. </ul>
      * @param in_callingCatReqInd  The calling party's category request indicator;
      * <ul> <li>CPCRI_CALLING_PARTY_CATEGORY_NOT_REQUESTED and
      * <li>CPCRI_CALLING_PARTY_CATEGORY_REQUESTED. </ul>
      * @param in_chargeInfoReqInd  The charge information request indicator; <ul>
      * <li>CIRI_CHARGE_INFORMATION_NOT_REQUESTED and
      * <li>CIRI_CHARGE_INFORMATION_REQUESTED. </ul>
      * @param in_mciReqInd  The malicious call identification request indicator; <ul>
      * <li>MCIRI_MALICIOUS_CALL_IDENTIFICATION_NOT_REQUESTED and
      * <li>MCIRI_RESERVED_FOR_MCIRI_MALICIOUS_CALL_IDENTIFICATION_REQUESTED. </ul> */
    public InfoReqInd(boolean in_callingAddrReqInd, boolean in_holdingInd,
            boolean in_callingCatReqInd, boolean in_chargeInfoReqInd,
            boolean in_mciReqInd) {
        this();
        this.setCallingPartyAddressReqInd(in_callingAddrReqInd);
        this.setHoldingInd(in_holdingInd);
        this.setCallingPartyCatReqInd(in_callingCatReqInd);
        this.setChargeInfoReqInd(in_chargeInfoReqInd);
        this.setMCIReqInd(in_mciReqInd);
    }
    /** Gets the Calling Party Address Request Ind field of the parameter.
      * @return boolean  The InfoReqInd value, see InfoReqInd().  */
    public boolean getCallingPartyAddressReqInd() {
        return m_callingPartyAddressReqInd;
    }
    /** Sets the Calling Party Address Request Ind field of the parameter.  */
    public void setCallingPartyAddressReqInd(boolean callingPartyAddressReqInd) {
        m_callingPartyAddressReqInd = callingPartyAddressReqInd;
    }
    /** Gets the Holding Indicator field of the parameter.
      * @return boolean  The Holding Indicator value, see InfoReqInd().  */
    public boolean getHoldingInd() {
        return m_holdingInd;
    }
    /** Sets the Holding Indicator field of the parameter.
      * @param holdingInd  The Holding Indicator value, see InfoReqInd().  */
    public void setHoldingInd(boolean holdingInd) {
        m_holdingInd = holdingInd;
    }
    /** Gets the Calling Party's Category Request Ind field of the parameter.
      * @return boolean  The Calling Party Category Request Indicator value, see
      * InfoReqInd().  */
    public boolean getCallingPartyCatReqInd() {
        return m_callingPartyCatReqInd;
    }
    /** Sets the Calling Party's Category Request Ind field of the parameter.
      * @param callingPartyCatReqInd  CallingPartyCatReqInd value, see InfoReqInd().  */
    public void setCallingPartyCatReqInd(boolean callingPartyCatReqInd) {
        m_callingPartyCatReqInd = callingPartyCatReqInd;
    }
    /** Gets the Charge Information Request Ind field of the parameter.
      * @return boolean  The ChargeInformation Request Indicator value, see
      * InfoReqInd().  */
    public boolean getChargeInfoReqInd() {
        return m_chargeInfoReqInd;
    }
    /** Sets the Charge Information Request Ind field of the parameter.
      * @param aChargeInfoReqInd  The ChargeInfoReqInd value, see InfoReqInd().  */
    public void setChargeInfoReqInd(boolean aChargeInfoReqInd) {
        m_chargeInfoReqInd = aChargeInfoReqInd;
    }
    /** Gets the Malicious Call Identification Request Ind field of the parameter.
      * @return boolean  The Malicious Call Identification Request Ind value, see
      * InfoReqInd().  */
    public boolean getMCIReqInd() {
        return m_MCIReqInd;
    }
    /** Sets the Malicious Call Identification Request Ind field of the parameter.
      * @param MCIReqInd  The MCIReqInd value, see InfoReqInd().  */
    public void setMCIReqInd(boolean MCIReqInd) {
        m_MCIReqInd = MCIReqInd;
    }
    /** The toString method retrieves a string containing the values of the members of
      * the InfoReqInd class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.InfoReqInd");
        b.append("\n\tm_callingPartyAddressReqInd: " + m_callingPartyAddressReqInd);
        b.append("\n\tm_holdingInd: " + m_holdingInd);
        b.append("\n\tm_callingPartyCatReqId: " + m_callingPartyCatReqInd);
        b.append("\n\tm_chargeInfoReqInd: " + m_chargeInfoReqInd);
        b.append("\n\tm_MCIReqInd: " + m_MCIReqInd);
        return b.toString();
    }
    protected boolean m_callingPartyAddressReqInd;
    protected boolean m_holdingInd;
    protected boolean m_callingPartyCatReqInd;
    protected boolean m_chargeInfoReqInd;
    protected boolean m_MCIReqInd;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
