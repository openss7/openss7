/*
 @(#) $RCSfile: InfoInd.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:04 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:04 $ by $Author: brian $
 */

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing the ISUP Information Indicator parameter.
    This core class provides access methods for all the common sub-fields in ITU
    and ANSI variant. ITU variant has no additional sub-fields. The ANSI class
    inherits from this core class to add the ANSI specific sub-fields.

    @author Monavacon Limited
    @version 1.2.2
  */
public class InfoInd implements java.io.Serializable {
    public static final byte CPARI_CALLING_PARTY_ADDRESS_NOT_INCLUDED = 0;
    public static final byte CPARI_CALLING_PARTY_ADDRESS_NOT_AVAILABLE = 1;
    public static final byte CPARI_CALLING_PARTY_ADDRESS_INCLUDED = 3;
    public static final boolean HPI_HOLD_NOT_PROVIDED = false;
    public static final boolean HPI_HOLD_PROVIDED = true;
    public static final boolean CPCRI_CALLING_PARTY_CATEGORY_NOT_INCLUDED = false;
    public static final boolean CPCRI_CALLING_PARTY_CATEGORY_INCLUDED = true;
    public static final boolean CIRI_CHARGE_INFORMATION_NOT_INCLUDED = false;
    public static final boolean CIRI_CHARGE_INFORMATION_INCLUDED = true;
    public static final boolean SII_SOLICITED = false;
    public static final boolean SII_UNSOLICITED = true;

    /** Constructs a new InfoInd class, parameters with default values.  */
    public InfoInd() {
    }
    /** Constructs an InfoInd class from the input parameters specified.
        @param in_callingAddrRespInd The calling party address response
        indicator, range 0 to 3. <ul> <li>CIRI_CHARGE_INFORMATION_NOT_INCLUDED
        and <li>CIRI_CHARGE_INFORMATION_INCLUDED. </ul>
        @param in_holdInd  The hold provided indicator. <ul>
        <li>HPI_HOLD_NOT_PROVIDED and <li>HPI_HOLD_PROVIDED. </ul>
        @param in_callingCatResInd  The calling party's category response
        indicator. <ul> <li>CPCRI_CALLING_PARTY_CATEGORY_NOT_INCLUDED and
        <li>CPCRI_CALLING_PARTY_CATEGORY_INCLUDED. </ul>
        @param in_chargeInfoResInd  The charge information response indicator.
        <ul> <li>CIRI_CHARGE_INFORMATION_NOT_INCLUDED and
        <li>CIRI_CHARGE_INFORMATION_INCLUDED. </ul>
        @param in_solInfoInd  The solicited information indicator. <ul>
        <li>SII_SOLICITED and <li>SII_UNSOLICITED. </ul>
      */
    public InfoInd(byte in_callingAddrRespInd, boolean in_holdInd,
            boolean in_callingCatResInd, boolean in_chargeInfoResInd,
            boolean in_solInfoInd)
        throws ParameterRangeInvalidException {
        this.setCallingPartyAddressRespInd(in_callingAddrRespInd);
        this.setHoldProvidedInd(in_holdInd);
        this.setCallingPartyCatRespInd(in_callingCatResInd);
        this.setChargeInfoRespInd(in_chargeInfoResInd);
        this.setSolicitedInfoInd(in_solInfoInd);
    }
    /** Gets the Calling Party Address Response Indicator field of the
        parameter.
        @return The CallingPartyAddressResponseIndicator value, range 0 to 3,
        see InfoInd().
      */
    public byte getCallingPartyAddressRespInd() {
        return m_callingPartyAddressResponseInd;
    }
    /** Sets the Calling Party Address Response Indicator field of the
        parameter.
        @param aCallingPartyAddressResponseIndicator  The CallingParty Address
        ResponseIndicator value, range 0 to 3, see InfoInd().
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setCallingPartyAddressRespInd(byte aCallingPartyAddressResponseIndicator)
        throws ParameterRangeInvalidException {
        if (0 <= aCallingPartyAddressResponseIndicator && aCallingPartyAddressResponseIndicator <= 3) {
            m_callingPartyAddressResponseInd = aCallingPartyAddressResponseIndicator;
            return;
        }
        throw new ParameterRangeInvalidException("CallingPartyAddressRespInd value " + aCallingPartyAddressResponseIndicator + " out of range.");
    }
    /** Gets the Hold Provided Indicator field of the parameter.
        @return Boolean, the HoldProvidedIndicator value, see InfoInd().
      */
    public boolean getHoldProvidedInd() {
        return m_holdProvidedIndicator;
    }
    /** Sets the Hold Provided Indicator field of the parameter.
        @param aHoldProvidedIndicator  The HoldProvidedIndicator value, see InfoInd().
      */
    public void setHoldProvidedInd(boolean aHoldProvidedIndicator) {
        m_holdProvidedIndicator = aHoldProvidedIndicator;
    }
    /** Gets the Calling Party Category Response Indicator field of the
        parameter.
        @return Boolean, the Calling Party Category ResponseIndicator value, see
        InfoInd().
      */
    public boolean getCallingPartyCatRespInd() {
        return m_callingPartyCategoryResponseInd;
    }
    /** Sets the Calling Party Category Response Indicator field of the parameter.
        @param aCallingPartyCategoryResponseIndicator  The Calling Party
        Category ResponseIndicator value, see InfoInd().
      */
    public void setCallingPartyCatRespInd(boolean aCallingPartyCategoryResponseIndicator) {
        m_callingPartyCategoryResponseInd = aCallingPartyCategoryResponseIndicator;
    }
    /** Gets the Charge Information Response Indicator field of the parameter.
        @return Boolean, the ChargeInformationResponseIndicator value, see InfoInd().
      */
    public boolean getChargeInfoRespInd() {
        return m_chargeInformationResponseIndicator;
    }
    /** Sets the Charge Information Response Indicator field of the parameter.
        @param aChargeInformationResponseIndicator  The
        ChargeInformationResponseIndicator value, see InfoInd().
      */
    public void setChargeInfoRespInd(boolean aChargeInformationResponseIndicator) {
        m_chargeInformationResponseIndicator = aChargeInformationResponseIndicator;
    }
    /** Gets the Solicited Information Indicator field of the parameter.
        @return Boolean, the SolicitedInformationIndicator value, see InfoInd().
      */
    public boolean getSolicitedInfoInd() {
        return m_solicitedInformationIndicator;
    }
    /** Sets the Solicited Information Indicator field of the parameter.
        @param aSolicitedInformationIndicator  The SolicitedInformationIndicator
        value, see InfoInd().
      */
    public void setSolicitedInfoInd(boolean aSolicitedInformationIndicator) {
        m_solicitedInformationIndicator = aSolicitedInformationIndicator;
    }
    /** The toString method retrieves a string containing the values of the
        members of the InfoInd class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.InfoInd");
        b.append("\n\tm_callingPartyAddressResponseInd: " + m_callingPartyAddressResponseInd);
        b.append("\n\tm_holdProvidedIndicator: " + m_holdProvidedIndicator);
        b.append("\n\tm_callingPartyCategoryResponseInd: " + m_callingPartyCategoryResponseInd);
        b.append("\n\tm_chargeInformationResponseIndicator: " + m_chargeInformationResponseIndicator);
        b.append("\n\tm_solicitedInformationIndicator: " + m_solicitedInformationIndicator);
        return b.toString();
    }
    protected byte m_callingPartyAddressResponseInd;
    protected boolean m_holdProvidedIndicator;
    protected boolean m_callingPartyCategoryResponseInd;
    protected boolean m_chargeInformationResponseIndicator;
    protected boolean m_solicitedInformationIndicator;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
