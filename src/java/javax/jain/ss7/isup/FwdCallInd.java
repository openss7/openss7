/*
 @(#) $RCSfile: FwdCallInd.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:04 $ <p>
 
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

/** A class representing a Forward Call Indicators parameter. This class provides
  * access to the common sub-fields in ITU and ANSI variants in the Forward Call
  * Indicators parameter.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract class FwdCallInd implements java.io.Serializable {
    public static final byte EMI_NO_ETE_METHOD_IND = 0;
    public static final byte EMI_SCCP_METHOD_AVAILABLE = 1;
    public static final byte EMI_PASS_ALONG_AND_SCCP_METHOD_AVAILABLE = 2;
    public static final byte EMI_PASS_ALONG_METHOD_AVAILABLE = 3;

    public static final boolean II_NO_INTERWORKING_IND = false;
    public static final boolean II_INTERWORKING_ENCOUNTERED = true;

    public static final boolean IAI_ORIGINATING_ACCESS_NON_ISDN = false;
    public static final boolean IAI_ORIGINATING_ACCESS_ISDN = true;

    public static final boolean ISUPI_ISUP_NOT_USED = false;
    public static final boolean ISUPI_ISUP_USED = true;

    public static final boolean NICI_NATIONAL_CALL = false;
    public static final boolean NICI_INTERNATIONAL_CALL = true;

    public static final byte ISUPPI_PREFERRED = 0;
    public static final byte ISUPPI_NOT_REQUIRED = 1;
    public static final byte ISUPPI_REQUIRED = 2;

    public static final byte SMI_SCCP_METHOD_NO_IND = 0;
    public static final byte SMI_CONNECTIONLESS_METHOD_AVAILABLE = 1;
    public static final byte SMI_CONNECTION_METHOD_AVAILABLE = 2;
    public static final byte SMI_CONNECTIONLESS_AND_CONNECTION_METHOD_AVAILABLE = 3;

    /** Constructs a new FwdCallInd taking no parameters. This constructor sets all
      * the sub-field values to their spare value (which is zero).  */
    public FwdCallInd() {
    }
    /** Constructs a new Forward Call Indicator.
      * @param endToEndMethodIndicator  The end to end method indicator.<ul>
      * <li>EMI_NO_ETE_METHOD_IND
      * <li>EMI_PASS_ALONG_METHOD_AVAILABLE
      * <li>EMI_SCCP_METHOD_AVAILABLE
      * <li>EMI_PASS_ALONG_AND_SCCP_METHOD_AVAILABLE</ul>
      * @param interworkingIndicator  The interworking indicator.<ul>
      * <li>II_NO_INTERWORKING_IND
      * <li>II_INTERWORKING_ENCOUNTERED</ul>
      * @param isdnUserPartIndicator  The ISDN User part indicator.<ul>
      * <li>ISUPI_ISUP_NOT_USED
      * <li>ISUPI_ISUP_USED</ul>
      * @param isdnUserPartPreferenceIndicator  The ISDN User part preference
      * indicator.<ul>
      * <li>ISUPPI_PREFERRED
      * <li>ISUPPI_NOT_REQUIRED
      * <li>ISUPPI_REQUIRED</ul>
      * @param isdnAccessIndicator  The ISDN access indicator.<ul>
      * <li>IAI_ORIGINATING_ACCESS_NON_ISDN
      * <li>IAI_ORIGINATING_ACCESS_ISDN</ul>
      * @param sccpMethodIndicator  The SCCP method indicator.<ul>
      * <li>SMI_SCCP_METHOD_NO_IND
      * <li>SMI_CONNECTIONLESS_METHOD_AVAILABLE
      * <li>SMI_CONNECTION_METHOD_AVAILABLE
      * <li>SMI_CONNECTIONLESS_AND_CONNECTION_METHOD_AVAILABLE</ul>
      * @param typeOfCall  The national/international call indicator.<ul>
      * <li>NICI_NATIONAL_CALL
      * <li>NICI_INTERNATIONAL_CALL</ul>
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public FwdCallInd(byte endToEndMethodIndicator, boolean interworkingIndicator,
            boolean isdnUserPartIndicator, byte isdnUserPartPreferenceIndicator,
            boolean isdnAccessIndicator, byte sccpMethodIndicator,
            boolean typeOfCallInd)
        throws ParameterRangeInvalidException {
        this();
        this.setETEMethodInd(endToEndMethodIndicator);
        this.setInterworkingInd(interworkingIndicator);
        this.setISDNAccessInd(isdnAccessIndicator);
        this.setISDNUserPartInd(isdnUserPartIndicator);
        this.setISUPPreferenceInd(isdnUserPartPreferenceIndicator);
        this.setSCCPMethodInd(sccpMethodIndicator);
        this.setTypeOfCallInd(typeOfCallInd);
    }
    /** Gets the End-to-End method Indicator field of the parameter.
      * @return The End-to-End method Indicator value, range 0 to 3, see FwdCallInd().
      */
    public byte getETEMethodInd() {
        return m_endToEndMethodIndicator;
    }
    /** Sets the End-to-End method Indicator field of the parameter.
      * @param endToEndMethodIndicator - The End-to-End method Indicator, range 0 to
      * 3, see FwdCallInd().
      * @exception ParameterRangeInvalidException  Thrown when the sub-field is out of
      * the specified range.  */
    public void setETEMethodInd(byte endToEndMethodIndicator)
        throws ParameterRangeInvalidException {
        if (EMI_NO_ETE_METHOD_IND > endToEndMethodIndicator
                || endToEndMethodIndicator > EMI_PASS_ALONG_METHOD_AVAILABLE)
            throw new ParameterRangeInvalidException("endToEndMethodIndicator value " + endToEndMethodIndicator + " is out of range.");
        m_endToEndMethodIndicator = endToEndMethodIndicator;
    }
    /** Gets the Interworking Indicator field of the parameter.
      * @return Boolean the Interworking Indicator value, see FwdCallInd().  */
    public boolean getInterworkingInd() {
        return m_interworkingIndicator;
    }
    /** Sets the Interworking Indicator field of the parameter.
      * @param interworkingIndicator  The interworking indicator, see FwdCallInd().
      */
    public void setInterworkingInd(boolean interworkingIndicator) {
        m_interworkingIndicator = interworkingIndicator;
    }
    /** Gets the ISDN Access Indicator field of the parameter.
      * @return Boolean the ISDN Access Indicator value, see FwdCallInd().  */
    public boolean getISDNAccessInd() {
        return m_isdnAccessIndicator;
    }
    /** Sets the ISDN Access Indicator field of the parameter.
      * @param isdnAccessIndicator  The ISDN Access Indicator value, see FwdCallInd().
      */
    public void setISDNAccessInd(boolean isdnAccessIndicator) {
        m_isdnAccessIndicator = isdnAccessIndicator;
    }
    /** Gets the ISDN User Part Indicator field of the parameter.
      * @return Boolean the ISDN User Part Indicator value, see FwdCallInd().
      */
    public boolean getISDNUserPartInd() {
        return m_isdnUserPartIndicator;
    }
    /** Sets the ISDN User Part Indicator field of the parameter.
      * @param isdnUserPartIndicator  ISUP indicator, see FwdCallInd().  */
    public void setISDNUserPartInd(boolean isdnUserPartIndicator) {
        m_isdnUserPartIndicator = isdnUserPartIndicator;
    }
    /** Gets the ISDN User Part Preference Indicator field of the parameter.
      * @return ISDN User Part Preference Indicator value, range 0 to 3, see
      * FwdCallInd().  */
    public byte getISUPPreferenceInd() {
        return m_isdnUserPartPreferenceIndicator;
    }
    /** Sets the ISDN User Part Preference Indicator field of the parameter.
      * @param isdnUserPartPreferenceIndicator  The ISDN User Part Preference
      * Indicator, range 0 to 3, see FwdCallInd().
      * @exception ParameterRangeInvalidException  Thrown when the sub-field is out of
      * the specified range.  */
    public void setISUPPreferenceInd(byte isdnUserPartPreferenceIndicator)
        throws ParameterRangeInvalidException {
        if (ISUPPI_PREFERRED > isdnUserPartPreferenceIndicator
                || isdnUserPartPreferenceIndicator > ISUPPI_REQUIRED)
        m_isdnUserPartPreferenceIndicator = isdnUserPartPreferenceIndicator;
    }
    /** Gets the SCCP Method Indicator field of the parameter.
      * @return SCCP Method Indicator value, range 0 to 3, see FwdCallInd().  */
    public byte getSCCPMethodInd() {
        return m_sccpMethodIndicator;
    }
    /** Sets the SCCP Method Indicator field of the parameter.
      * @param sccpMethodIndicator  The SCCP Method Indicator, range 0 to 3, see
      * FwdCallInd().
      * @exception ParameterRangeInvalidException  Thrown when the sub-field is out of
      * the specified range.  */
    public void setSCCPMethodInd(byte sccpMethodIndicator)
        throws ParameterRangeInvalidException {
        if (sccpMethodIndicator > SMI_SCCP_METHOD_NO_IND ||
            SMI_CONNECTIONLESS_AND_CONNECTION_METHOD_AVAILABLE > sccpMethodIndicator)
            throw new ParameterRangeInvalidException("javax.jain.FwdCallInd");
        m_sccpMethodIndicator = sccpMethodIndicator;
    }
    /** Gets the type of call Indicator.
      * @return Boolean the type of call indicator, see FwdCallInd().  */
    public boolean getTypeOfCallInd() {
        return m_typeOfCallInd;
    }
    /** Sets the type of call Indicator
      * @param typeOfCallInd  The type of call indicator, see FwdCallInd().  */
    public void setTypeOfCallInd(boolean typeOfCallInd) {
        m_typeOfCallInd = typeOfCallInd;
    }
    /** The toString method retrieves a string containing the values of the members of
      * the FwdCallInd class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.FwdCallInd:");
        b.append("\n\tm_endToEndMethodIndicator: " + m_endToEndMethodIndicator);
        b.append("\n\tm_interworkingIndicator: " + m_interworkingIndicator);
        b.append("\n\tm_isdnAccessIndicator: " + m_isdnAccessIndicator);
        b.append("\n\tm_isdnUserPartIndicator: " + m_isdnUserPartIndicator);
        b.append("\n\tm_isdnUserPartPreferenceIndicator: " + m_isdnUserPartPreferenceIndicator);
        b.append("\n\tm_sccpMethodIndicator: " + m_sccpMethodIndicator);
        b.append("\n\tm_typeOfCallInd: " + m_typeOfCallInd);
        return b.toString();
    }
    protected byte m_endToEndMethodIndicator;
    protected boolean m_interworkingIndicator;
    protected boolean m_isdnAccessIndicator;
    protected boolean m_isdnUserPartIndicator;
    protected byte m_isdnUserPartPreferenceIndicator;
    protected byte m_sccpMethodIndicator;
    protected boolean m_typeOfCallInd;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
