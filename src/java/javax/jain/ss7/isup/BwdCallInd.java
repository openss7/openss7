/*
 @(#) $RCSfile: BwdCallInd.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-09-02 08:46:47 $ <p>
 
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

/** A class representing the ISUP Parameter Backward Call Indicators. This core class
  * provides access methods for all the common sub-fields in ITU and ANSI variant.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract class BwdCallInd implements java.io.Serializable {
    public static final byte CI_NO_CHARGE_IND = 0;
    public static final byte CI_NO_CHARGE = 1;
    public static final byte CI_CHARGE = 2;
    public static final byte CPSI_NO_CALLED_STATUS_IND = 0;
    public static final byte CPSI_SUBSCRIBER_FREE = 1;
    public static final byte CPSI_CONNECT_WHEN_FREE = 2;
    public static final byte CPSI_EXCESSIVE_DELAY = 3;
    public static final byte CPCI_NO_CALLED_CATEGORY_IND = 0;
    public static final byte CPCI_ORDINARY_SUBSCRIBER = 1;
    public static final byte CPCI_PAYPHONE = 2;
    public static final byte EMI_NO_ETE_METHOD_IND = 0;
    public static final byte EMI_PASS_ALONG_METHOD_AVAILABLE = 1;
    public static final byte EMI_SCCP_METHOD_AVAILABLE = 2;
    public static final byte EMI_PASS_ALONG_AND_SCCP_METHOD_AVAILABLE = 3;
    public static final boolean II_NO_INTERWORKING_IND = false;
    public static final boolean II_INTERWORKING_ENCOUNTERED = false;
    public static final boolean ISUPI_ISUP_NOT_USED = false;
    public static final boolean ISUPI_ISUP_USED = false;
    public static final boolean HI_HOLDING_NOT_REQUESTED = false;
    public static final boolean HI_HOLDING_REQUESTED = false;
    public static final boolean IAI_TERMINATING_ACCESS_NON_ISDN = false;
    public static final boolean IAI_TERMINATING_ACCESS_ISDN = false;
    public static final boolean ECDI_INCOMING_HALF_ECHO_CONTROL_DEVICE_NOT_INCLUDED = false;
    public static final boolean ECDI_INCOMING_HALF_ECHO_CONTROL_DEVICE_INCLUDED = false;
    public static final byte SMI_SCCP_METHOD_NO_IND = 0;
    public static final byte SMI_CONNECTIONLESS_METHOD_AVAILABLE = 1;
    public static final byte SMI_CONNECTION_METHOD_AVAILABLE = 2;
    public static final byte SMI_CONNECTIONLESS_AND_CONNECTION_METHOD_AVAILABLE = 3;
    /** Constructs a new BwdCallInd class, parameters with default values.  */
    protected BwdCallInd() {
    }
    /** Constructs a BwdCallInd class from the input parameters specified.
      * @param in_chargeInd  The charge indicator, range 0 to 3; <ul>
      * <li>CI_NO_CHARGE_IND, <li>CI_NO_CHARGE and <li>CI_CHARGE. </ul>
      * @param in_calledStatusInd  The called party's status indicator, range 0 to 3;
      * <ul> <li>CPSI_NO_CALLED_STATUS_IND, <li>CPSI_SUBSCRIBER_FREE,
      * <li>CPSI_CONNECT_WHEN_FREE and <li>CPSI_EXCESSIVE_DELAY (ANSI only). </ul>
      * @param in_calledCateInd  The called party's category indicator, range 0 to 3;
      * <ul> <li>CPCI_NO_CALLED_CATEGORY_IND, <li>CPCI_ORDINARY_SUBSCRIBER and
      * <li>CPCI_PAYPHONE. </ul>
      * @param in_eteMethodInd  The end-to-end method indicator, range 0 to 3; <ul>
      * <li>EMI_NO_ETE_METHOD_IND, <li>EMI_PASS_ALONG_METHOD_AVAILABLE,
      * <li>EMI_PASS_ALONG_AND_SCCP_METHOD_AVAILABLE and
      * <li>EMI_PASS_ALONG_AND_SCCP_METHOD_AVAILABLE. </ul>
      * @param in_iwInd  The interworking indicator; <ul> <li>II_NO_INTERWORKING_IND
      * and <li>II_INTERWORKING_ENCOUNTERED. </ul>
      * @param in_isupInd  The ISDN User Part indicator; <ul> <li>IUPI_ISUP_NOT_USED
      * and <li>IUPI_ISUP_USED. </ul>
      * @param in_holdInd  The holding indicator; <ul> <li>HI_HOLDING_NOT_REQUESTED
      * and <li>HI_HOLDING_REQUESTED. </ul>
      * @param in_isdnAccessInd  The ISDB access indicator; <ul>
      * <li>IAI_TERMINATING_ACCESS_NON_ISDN and <li>IAI_TERMINATING_ACCESS_ISDN. </ul>
      * @param in_echoDevInd  The echo control device indicator; <ul>
      * <li>ECDI_INCOMING_HALF_ECHO_CONTROL_DEVICE_NOT_INCLUDED and
      * <li>ECDI_INCOMING_HALF_ECHO_CONTROL_DEVICE_INCLUDED. </ul>
      * @param in_sccpMethodInd  The SCCP method indicator, range 0 to 3; <ul>
      * <li>SMI_SCCP_METHOD_NO_IND, <li>SMI_CONNECTIONLESS_METHOD_AVAILABLE,
      * <li>SMI_CONNECTION_METHOD_AVAILABLE and
      * <li>SMI_CONNECTIONLESS_AND_CONNECTION_METHOD_AVAILABLE. </ul>
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    protected BwdCallInd(byte in_chargeInd, byte in_calledStatusInd,
            byte in_calledCateInd, byte in_eteMethodInd, boolean in_iwInd,
            boolean in_isupInd, boolean in_holdInd, boolean in_isdnAccessInd,
            boolean in_echoDevInd, byte in_sccpMethodInd)
        throws ParameterRangeInvalidException {
        this();
        this.setChargeInd(in_chargeInd);
        this.setCalledStatusInd(in_calledStatusInd);
        this.setCalledCategoryInd(in_calledCateInd);
        this.setETEMethodInd(in_eteMethodInd);
        this.setInterworkingInd(in_iwInd);
        this.setISUPInd(in_isupInd);
        this.setHoldingInd(in_holdInd);
        this.setISDNAccessInd(in_isdnAccessInd);
        this.setEchoControlDevInd(in_echoDevInd);
        this.setSCCPMethodInd(in_sccpMethodInd);
    }
    /** Gets the charge indicator field of the parameter.
      * @return byte Value of the ChargeIndicator value, range 0 to 3, see
      * BwdCallInd().  */
    public byte getChargeInd() {
        return m_chargeInd ;
    }
    /** Sets the charge indicator field of the parameter.
      * @param aChargeInd  The ChargeIndicator value, range 0-3, see BwdCallInd().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public void setChargeInd(byte aChargeInd)
        throws ParameterRangeInvalidException {
        switch (aChargeInd) {
            case CI_NO_CHARGE_IND:
            case CI_NO_CHARGE:
            case CI_CHARGE:
                m_chargeInd = aChargeInd;
                return;
        }
        throw new ParameterRangeInvalidException("ChargeInd value " + aChargeInd + " out of range.");
    }
    /** Gets the called Partys Status Indicator field of the parameter.
      * @return byte Contains the value of the CalledPartysStatusIndicator value,
      * range 0 to 3, see BwdCallInd().  */
    public byte getCalledStatusInd() {
        return m_calledPartyStatusIndicator ;
    }
    /** Sets the called Partys Status Indicator field of the parameter.
      * @param aCalledPartysStatusIndicator  The Called Party Status Indicator value,
      * range 0 to 3, see BwdCallInd().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public void setCalledStatusInd(byte aCalledPartysStatusIndicator)
        throws ParameterRangeInvalidException {
        switch (aCalledPartysStatusIndicator) {
            case CPSI_NO_CALLED_STATUS_IND:
            case CPSI_SUBSCRIBER_FREE:
            case CPSI_CONNECT_WHEN_FREE:
            case CPSI_EXCESSIVE_DELAY:
                m_calledPartyStatusIndicator = aCalledPartysStatusIndicator;
                return;
        }
        throw new ParameterRangeInvalidException("CalledPartyStatusIndicator value " + aCalledPartysStatusIndicator + " out of range.");
    }
    /** Gets the called Party's Category Indicator field of the parameter.
      * @return byte Contains the CalledPartysCategoryIndicator value, range 0 to 3,
      * see BwdCallInd().  */
    public byte getCalledCategoryInd() {
        return m_calledPartysCategoryIndicator ;
    }
    /** Sets the called Partys Category Indicator field of the parameter.
      * @param aCalledPartysCategoryIndicator  The Called Party Category Indicator
      * value, range 0 to 3, see BwdCallInd().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public void setCalledCategoryInd(byte aCalledPartysCategoryIndicator)
        throws ParameterRangeInvalidException {
        switch (aCalledPartysCategoryIndicator) {
            case CPCI_NO_CALLED_CATEGORY_IND:
            case CPCI_ORDINARY_SUBSCRIBER:
            case CPCI_PAYPHONE:
                m_calledPartysCategoryIndicator = aCalledPartysCategoryIndicator;
                return;
        }
        throw new ParameterRangeInvalidException("CalledPartysCategoryIndicator value " + aCalledPartysCategoryIndicator + " out of range.");
    }
    /** Gets the End To End Method Indicator field of the parameter.
      * @return byte Contains the EndToEndMethodIndicator value, range 0 to 3, see
      * BwdCallInd().  */
    public byte getETEMethodInd() {
        return m_endToEndMethodIndicator ;
    }
    /** Sets the End To End Method Indicator field of the parameter.
      * @param aEndToEndMethodIndicator  The End To End Method Indicator value, range
      * 0 to 3, see BwdCallInd().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public void setETEMethodInd(byte aEndToEndMethodIndicator)
        throws ParameterRangeInvalidException {
        switch (aEndToEndMethodIndicator) {
            case EMI_NO_ETE_METHOD_IND:
            case EMI_PASS_ALONG_METHOD_AVAILABLE:
            case EMI_SCCP_METHOD_AVAILABLE:
            case EMI_PASS_ALONG_AND_SCCP_METHOD_AVAILABLE:
                m_endToEndMethodIndicator = aEndToEndMethodIndicator;
                return;
        }
        throw new ParameterRangeInvalidException("EndToEndMethodIndicator value " + aEndToEndMethodIndicator + " out of range.");
    }
    /** Gets the interworking Indicator field of the parameter.
      * @return Boolean the InterworkingIndicator value, see BwdCallInd().  */
    public boolean getInterworkingInd() {
        return m_interworkingIndicator ;
    }
    /** Sets the interworking Indicator field of the parameter.
      * @param aInterworkingIndicator  The InterworkingIndicator value, see
      * BwdCallInd().  */
    public void setInterworkingInd(boolean aInterworkingIndicator) {
        m_interworkingIndicator = aInterworkingIndicator;
    }
    /** Gets the ISDN User Part Indicator field of the parameter.
      * @return Boolean the ISDN UserPart Indicator value, see BwdCallInd().  */
    public boolean getISUPInd() {
        return m_isdnUserPartIndicator ;
    }
    /** Sets the ISDN User Part Indicator field of the parameter.
      * @param aISDNUserPartIndicator  The ISDN UserPart Indicator value, see
      * BwdCallInd().  */
    public void setISUPInd(boolean aISDNUserPartIndicator) {
        m_isdnUserPartIndicator = aISDNUserPartIndicator;
    }
    /** Gets the Holding Indicator field of the parameter.
      * @return Boolean the HoldingIndicator value, see BwdCallInd().  */
    public boolean getHoldingInd() {
        return m_holdingIndicator ;
    }
    /** Sets the Holding Indicator field of the parameter.
      * @param aHoldingIndicator  The HoldingIndicator value, see BwdCallInd(). */
    public void setHoldingInd(boolean aHoldingIndicator) {
        m_holdingIndicator = aHoldingIndicator;
    }
    /** Gets the ISDN Access Indicator field of the parameter.
      * @return Boolean the ISDNAccessIndicator value, see BwdCallInd() */
    public boolean getISDNAccessInd() {
        return m_isdnAccessIndicator ;
    }
    /** Sets the ISDN Access Indicator field of the parameter.
      * @param aISDNAccessIndicator the ISDNAccessIndicator value, see BwdCallInd().  */
    public void setISDNAccessInd(boolean aISDNAccessIndicator) {
        m_isdnAccessIndicator = aISDNAccessIndicator;
    }
    /** Gets the Echo Control Device Indicator field of the parameter.
      * @return Boolean the EchoControlDeviceIndicator value, see BwdCallInd().  */
    public boolean getEchoControlDevInd() {
        return m_echoControlDeviceIndicator ;
    }
    /** Sets the Echo Control Device Indicator field of the parameter.
      * @param aEchoControlDeviceIndicator  The EchoControlDeviceIndicator value, see
      * BwdCallInd().  */
    public void setEchoControlDevInd(boolean aEchoControlDeviceIndicator) {
        m_echoControlDeviceIndicator = aEchoControlDeviceIndicator;
    }
    /** Gets the SCCP Method Indicator field of the parameter.
      * @return byte Contains the SCCPMethodIndicator value, range 0 to 3, see
      * BwdCallInd().  */
    public byte getSCCPMethodInd() {
        return m_sccpMethodIndicator ;
    }
    /** Sets the SCCP Method Indicator field of the parameter.
      * @param aSCCPMethodIndicator  The SCCPMethodIndicator value, range 0 to 3, see
      * BwdCallInd().
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public void setSCCPMethodInd(byte aSCCPMethodIndicator)
        throws ParameterRangeInvalidException {
        switch (aSCCPMethodIndicator) {
            case SMI_SCCP_METHOD_NO_IND:
            case SMI_CONNECTIONLESS_METHOD_AVAILABLE:
            case SMI_CONNECTION_METHOD_AVAILABLE:
            case SMI_CONNECTIONLESS_AND_CONNECTION_METHOD_AVAILABLE:
                m_sccpMethodIndicator = aSCCPMethodIndicator;
                return;
        }
        throw new ParameterRangeInvalidException("SCCPMethodInd value " + aSCCPMethodIndicator + " out of range.");
    }
    /** The toString method retrieves a string containing the values of the members of
      * the BwdCallInd class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.BwdCallInd");
        b.append("\n\tm_chargeInd: " + m_chargeInd);
        b.append("\n\tm_calledPartyStatusIndicator: " + m_calledPartyStatusIndicator);
        b.append("\n\tm_calledPartysCategoryIndicator: " + m_calledPartysCategoryIndicator);
        b.append("\n\tm_endToEndMethodIndicator: " + m_endToEndMethodIndicator);
        b.append("\n\tm_interworkingIndicator: " + m_interworkingIndicator);
        b.append("\n\tm_isdnUserPartIndicator: " + m_isdnUserPartIndicator);
        b.append("\n\tm_holdingIndicator: " + m_holdingIndicator);
        b.append("\n\tm_isdnAccessIndicator: " + m_isdnAccessIndicator);
        b.append("\n\tm_echoControlDeviceIndicator: " + m_echoControlDeviceIndicator);
        b.append("\n\tm_sccpMethodIndicator: " + m_sccpMethodIndicator);
        return b.toString();
    }
    protected byte m_chargeInd;
    protected byte m_calledPartyStatusIndicator;
    protected byte m_calledPartysCategoryIndicator;
    protected byte m_endToEndMethodIndicator;
    protected boolean m_interworkingIndicator;
    protected boolean m_isdnUserPartIndicator;
    protected boolean m_holdingIndicator;
    protected boolean m_isdnAccessIndicator;
    protected boolean m_echoControlDeviceIndicator;
    protected byte m_sccpMethodIndicator;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
