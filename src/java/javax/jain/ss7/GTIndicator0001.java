/*
 @(#) $RCSfile: GTIndicator0001.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:45 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:45 $ by $Author: brian $
 */

package javax.jain.ss7;

import javax.jain.*;

/**
  * This class represents a Global Title Indicator-0001, it can be used
  * to store, create and retrieve GlobalTitle-0001 entries.
  *
  * It contains the accessor methods for the following parameters in
  * order to accommodate the difference in the protocol variants
  * supported, namely ANSI SCCP T1.112 (1992), ANSI SCCP T1.112 (1996),
  * ITU SCCP Q711-716 (03/1993) and ITU SCCP Q711-716 (07/1996): <ul>
  * <li>Address Information for ITU and ANSI
  * <li>Translation Type for ANSI
  * <li>Numbering Plan for ANSI
  * <li>Encoding Scheme for ANSI
  * <li>Nature of Address Indicator for ITU
  * <li>Odd Indicator (bit 8 of Address Indicator) for ITU </ul>
  *
  * For GT0001 address information it is expected that for non BCD
  * numbers to be raw data and when BCD encoding, to be 2 BCD numbers
  * per byte (i.e 2 address_signals/digits/numbers per byte when BCD
  * encoding). The nature of address indicator for GT0001, only uses
  * bits 1 to 7 for its coding, therefore bit 8 of the octet contains
  * the even/odd indicator which is coded as 0 for even or 1 for odd
  * number of address_signals/digits/numbers. <br/><br/>
  *
  * <table align="center" border=1 cellpadding=1>
  * <tr>
  * <th>Bit 8</th>
  * <th>Odd/Even</th>
  * </tr>
  * <tr>
  * <td align="center">0</td>
  * <td>Event number of address signals.</td>
  * </tr>
  * <tr>
  * <td align="center">1</td>
  * <td>Odd number of address signals.</td>
  * </tr>
  * </table> <p>
  *
  * GTIndicator0001 has a boolean oddIndicator, which can be used to
  * determine the contents of the even/odd Indicator field. The
  * isOddIndicator will return true when there is an odd number of
  * address_signals/digits/numbers and false when there is an even
  * number of address_signals/digits/numbers.  <p>
  *
  * <center> <img src=/doc-files/gt0001.jpg> </center><br>
  * <center> Global title format for Indicator 0001 </center><br>
  * @serial
  *
  * @author Monavacon Limited
  * @version 1.2.2
  * @see        GlobalTitle
  */
public final class GTIndicator0001 extends GlobalTitle {
    /**
      * Constructs a new Global Title Indicator-0001 for an ITU
      * implementation of the JAIN SS7 API with the relevent mandatory
      * parameters.
      * @param natureAddrIndicator
      * The new nature of address supplied to the constructor.
      * @param isOddIndicator
      * The new Odd Indicator supplied to the constructor.
      * @param addressInformation
      * The new address Information supplied to the constructor.
      */
    public GTIndicator0001(NAddressIndicator natureAddrIndicator,
            boolean isOddIndicator, byte[] addressInformation) {
        super();
        setNatureOfAddrInd(natureAddrIndicator);
        setOddIndicator(isOddIndicator);
        setAddressInformation(addressInformation);
    }
    /**
      * Constructs a new Global Title Indicator-0001 for an ANSI
      * implementation of the JAIN SS7 API with the relevent mandatory
      * parameters.
      * @param translationType
      * The new translation type supplied to the constructor.
      * @param numberingPlan
      * The new numbering Plan supplied to the constructor.
      * @param encodingScheme
      * The new encoding scheme supplied to the constructor.
      * @param addressInformation
      * The new address Information supplied to the constructor.
      */
    public GTIndicator0001(byte translationType, NumberingPlan numberingPlan,
            EncodingScheme encodingScheme, byte[] addressInformation) {
        super();
        setTranslationType(translationType);
        setNumberingPlan(numberingPlan);
        setEncodingScheme(encodingScheme);
        setAddressInformation(addressInformation);
    }
    /**
      * Sets the Translation Type of this Global Title Indicator-0001,
      * which directs the message to the appropriate global title
      * translator.
      * The translation type is only specific to an ANSI implementation
      * of the JAIN API for GTIndicator-0001.
      * @param translationType
      * The new Translation Type value.
      */
    public void setTranslationType(byte translationType) {
        m_translationType = translationType;
        m_translationTypeIsSet = true;
    }
    /**
      * Sets the Numbering Plan of this Global Title Indicator-0001.
      * The numbering plan is only specific to an ANSI implementation of
      * the JAIN SS7 API for GTIndicator-0001.
      * @param numberingPlan
      * One of the following:- <ul>
      * <li>{@link NumberingPlan#NP_UNKNOWN NP_UNKNOWN} - Indicates an unknown Numbering Plan
      * <li>{@link NumberingPlan#NP_ISDN_TEL NP_ISDN_TEL} - Indicates ISDN/Telephony Numbering Plan
      * <li>{@link NumberingPlan#NP_GENERIC NP_GENERIC} - Indicates a generic Numbering Plan.
      * <li>{@link NumberingPlan#NP_DATA NP_DATA} - Indicates Data Numbering Plan
      * <li>{@link NumberingPlan#NP_TELEX NP_TELEX} - Indicates a Telex Numbering Plan
      * <li>{@link NumberingPlan#NP_MARITIME_MOBILE NP_MARITIME_MOBILE} - Indicates a Maritime Mobile Numbering Plan
      * <li>{@link NumberingPlan#NP_LAND_MOBILE NP_LAND_MOBILE} - Indicates a Land Mobile Numbering Plan
      * <li>{@link NumberingPlan#NP_ISDN_MOBILE NP_ISDN_MOBILE} - Indicates an ISDN Mobile Numbering Plan
      * <li>{@link NumberingPlan#NP_NETWORK NP_NETWORK} - Indicates Network Specific Numbering Plan </ul>
      * @see NumberingPlan
      */
    public void setNumberingPlan(NumberingPlan numberingPlan) {
        m_numberingPlan = numberingPlan;
        m_numberingPlanIsSet = true;
    }
    /**
      * Sets the Encoding Scheme of this Global Title Indicator-0001.
      * The encoding scheme is only specific to an ANSI implementation
      * of the JAIN SS7 API for GTIndicator-0001.
      * @param  encodingScheme
      * One of the following:- <ul>
      * <li>{@link EncodingScheme#ES_UNKNOWN ES_UNKNOWN} - Indicates an unknown encoding scheme.
      * <li>{@link EncodingScheme#ES_ODD ES_ODD} - Indicates BCD, odd number of digits
      * <li>{@link EncodingScheme#ES_EVEN ES_EVEN} - Indicates - BCD, even number of digits
      * <li>{@link EncodingScheme#ES_NATIONAL_SPECIFIC ES_NATIONAL_SPECIFIC} - Indicates a national specific </ul>
      * @see EncodingScheme
      */
    public void setEncodingScheme(EncodingScheme encodingScheme) {
        m_encodingScheme = encodingScheme;
        m_encodingSchemeIsSet = true;
    }
    /**
      * Sets the Nature of Address Indicator of this Global Title
      * Indicator-0001.
      * The Nature of address indicator is only specific to the ITU
      * implementation of the JAIN SS7 API.
      * @param  natureOfAddrInd
      * The new Nature Of Address Indicator value.
      */
    public void setNatureOfAddrInd(NAddressIndicator natureOfAddrInd) {
        m_natureOfAddrInd = natureOfAddrInd;
        m_natureOfAddrIndIsSet = true;
    }
    /**
      * Sets the Odd Indicator, bit 8 of the Nature of Address Indicator.
      * The Odd Indicator is only specific to the ITU version of
      * GTIndicator-0001. This field is defaulted to false therefore
      * "Odd BCD Encoding".
      * @param  oddIndicator
      * One of the following:- <ul>
      * <li>true - if Even BCD Encoding.
      * <li>false - if Odd BCD Encoding.  </ul>
      */
    public void setOddIndicator(boolean oddIndicator) {
        m_oddIndicator = oddIndicator;
        m_oddIndicatorIsSet = true;
    }
    /**
      * Gets the Translation Type of this Global Title Indicator-0001,
      * which directs the message to the appropriate global title
      * translator.
      * The translation type is only specific to an ANSI implementation
      * of the JAIN INAP API for GTIndicator-0001.
      * @return The Translation Type value
      * @exception ParameterNotSetException
      * Thrown if an instantiation of this GTIndicator does not support this parameter.
      */
    public byte getTranslationType()
        throws ParameterNotSetException {
        if (m_translationTypeIsSet)
            return m_translationType;
        throw new ParameterNotSetException("Translation Type: not set.");
    }
    /**
      * Gets the Numbering Plan of this Global Title Indicator-0001.
      * The numbering plan is only specific to an ANSI implementation of
      * the JAIN INAP API for GTIndicator-0001.
      * @return The Numbering Plan
      * One of the following:- <ul>
      * <li>{@link NumberingPlan#NP_UNKNOWN NP_UNKNOWN} - Indicates an unknown Numbering Plan
      * <li>{@link NumberingPlan#NP_ISDN_TEL NP_ISDN_TEL} - Indicates ISDN/Telephony Numbering Plan
      * <li>{@link NumberingPlan#NP_GENERIC NP_GENERIC} - Indicates a generic Numbering plan in ANSI.
      * <li>{@link NumberingPlan#NP_DATA NP_DATA} - Indicates Data Numbering Plan
      * <li>{@link NumberingPlan#NP_TELEX NP_TELEX} - Indicates a Telex Numbering Plan
      * <li>{@link NumberingPlan#NP_MARITIME_MOBILE NP_MARITIME_MOBILE} - Indicates a Maritime Mobile Numbering Plan
      * <li>{@link NumberingPlan#NP_LAND_MOBILE NP_LAND_MOBILE} - Indicates a Land Mobile Numbering Plan
      * <li>{@link NumberingPlan#NP_ISDN_MOBILE NP_ISDN_MOBILE} - Indicates an ISDN Mobile Numbering Plan
      * <li>{@link NumberingPlan#NP_NETWORK NP_NETWORK} - Indicates Network Specific Numbering Plan </ul>
      * @exception ParameterNotSetException
      * Thrown if an instantiation of this GTIndicator does not support this parameter.
      * @see NumberingPlan
      */
    public NumberingPlan getNumberingPlan()
        throws ParameterNotSetException {
        if (m_numberingPlanIsSet)
            return m_numberingPlan;
        throw new ParameterNotSetException("Numbering Plan: not set.");
    }
    /**
      * Gets the Encoding Scheme of this Global Title Indicator-0001.
      * The encoding scheme is only specific to an ANSI implementation
      * of the JAIN INAP API for GTIndicator-0001.
      * @return
      * The Encoding Scheme - one of the following:- <ul>
      * <li>{@link EncodingScheme#ES_UNKNOWN ES_UNKNOWN} - Indicates an unknown encoding scheme.
      * <li>{@link EncodingScheme#ES_ODD ES_ODD} - Indicates BCD, odd number of digits
      * <li>{@link EncodingScheme#ES_EVEN ES_EVEN} - Indicates BCD, even number of digits
      * <li>{@link EncodingScheme#ES_NATIONAL_SPECIFIC ES_NATIONAL_SPECIFIC} - Indicates
      * a national specific encoding scheme.</ul>
      * @exception ParameterNotSetException
      * Thrown if an instantiation of this GTIndicator does not support this parameter.
      * @see EncodingScheme
      */
    public EncodingScheme getEncodingScheme()
        throws ParameterNotSetException {
        if (m_encodingSchemeIsSet)
            return m_encodingScheme;
        throw new ParameterNotSetException("Encoding Scheme: not set.");
    }
    /**
      * Gets the Nature of Address Indicator of this Global Title Indicator-0001.
      * The nature of address indicator is only specific to the ITU
      * implementation of the JAIN INAP API.
      * @return
      * One of the following: <ul>
      * <li>{@link NAddressIndicator#NA_UNKNOWN NA_UNKNOWN} - Indicates a unknown Nature of Address
      * <li>{@link NAddressIndicator#NA_SUBSCRIBER NA_SUBSCRIBER} - Indicates a subscriber number
      * <li>{@link NAddressIndicator#NA_RESERVED NA_RESERVED} - Reserved for national use Nature of Address Indicator
      * <li>{@link NAddressIndicator#NA_NATIONAL_SIGNIFICANT NA_NATIONAL_SIGNIFICANT} - Indicates a National Significant Number
      * <li>{@link NAddressIndicator#NA_INTERNATIONAL NA_INTERNATIONAL} - Indicates an international number </ul>
      * @exception ParameterNotSetException
      * Thrown if an instantiation of this GTIndicator does not support this parameter.
      * @see NAddressIndicator
      */
    public NAddressIndicator getNatureOfAddrInd()
        throws ParameterNotSetException {
        if (m_natureOfAddrIndIsSet)
            return m_natureOfAddrInd;
        throw new ParameterNotSetException("Nature of Address Indicator: not set.");
    }
    /**
      * Gets the Odd Indicator, bit 8 of the Nature of Address Indicator.
      * The Odd Indicator is only specific to the ITU version of the
      * GTIndicator-0001. This field is defaulted to false therefore
      * "Odd BCD Encoding".
      * @return
      * One of the following:- <ul>
      * <li>true  - if Even BCD Encoding.
      * <li>false - if Odd BCD Encoding.  </ul>
      * @exception ParameterNotSetException
      * Thrown if an instantiation of this GTIndicator does not support this parameter.
      */
    public boolean isOddIndicator()
        throws ParameterNotSetException{
        if (m_oddIndicatorIsSet)
            return m_oddIndicator;
        throw new ParameterNotSetException("Odd Indicator: not set.");
    }
    /**
      * This method returns the format of this Global Title -
      * GTINDICATOR-0001.
      * @return
      * The Global Title Indicator value.
      */
    public GTIndicator getGTIndicator() {
        return GTIndicator.GTINDICATOR_0001;
    }
    /**
      * java.lang.String representation of class GTIndicator0001.
      * @return
      * java.lang.String provides description of class GTIndicator0001.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.GtIndicator0001");
        b.append("\n\tGlobal Title Indicator: = GTINDICATOR_0001");
        b.append("\n\tm_translationType = " + m_translationType);
        b.append("\n\tm_translationTypeIsSet = " + m_translationTypeIsSet);
        b.append("\n\tm_numberingPlan = ");
        if (m_numberingPlan != null) b.append(m_numberingPlan.toString());
        b.append("\n\tm_numberingPlanIsSet = " + m_numberingPlanIsSet);
        b.append("\n\tm_encodingScheme = ");
        if (m_encodingScheme != null) b.append(m_encodingScheme.toString());
        b.append("\n\tm_encodingSchemeIsSet = " + m_encodingSchemeIsSet);
        b.append("\n\tm_natureOfAddrInd = ");
        if (m_natureOfAddrInd != null) b.append(m_natureOfAddrInd.toString());
        b.append("\n\tm_natureOfAddrIndIsSet = " + m_natureOfAddrIndIsSet);
        b.append("\n\tm_oddIndicator = " + m_oddIndicator);
        b.append("\n\tm_oddIndicatorIsSet = " + m_oddIndicatorIsSet);
        return b.toString();
    }
    /** The Translation Type of this Global Title Indicator.
      * @serial m_translationType
      * A default serializable field.  */
    protected byte m_translationType = -1;
    /** Whether the Translation Type is set in this Global Title
      * Indicator.
      * @serial m_translationTypeIsSet
      * A default serializable field.  */
    protected boolean m_translationTypeIsSet = false;
    /** The Numbering Plan of this Global Title Indicator
      * @serial m_numberingPlan
      * A default serializable field.  */
    protected NumberingPlan m_numberingPlan = null;
    /** Whether the Numbering Plan is set in this Global Title
      * Indicator.
      * @serial m_numberingPlanIsSet
      * A default serializable field.  */
    protected boolean m_numberingPlanIsSet = false;
    /** The Encoding Scheme of this Global Title Indicator
      * @serial m_encodingScheme
      * A default serializable field.  */
    protected EncodingScheme m_encodingScheme = null;
    /** Whether the Encoding Scheme is set in this Global Title
      * Indicator.
      * @serial m_encodingSchemeIsSet
      * A default serializable field.  */
    protected boolean m_encodingSchemeIsSet = false;
    /** The Nature of Address Indicator of this Global Title Indicator
      * @serial m_natureOfAddrInd
      * A default serializable field.  */
    protected NAddressIndicator m_natureOfAddrInd = null;
    /** Whether the Nature of Address Indicator is set in this Global Title
      * Indicator.
      * @serial m_natureOfAddrIndIsSet
      * A default serializable field.  */
    protected boolean m_natureOfAddrIndIsSet = false;
    /** Indicates if the Odd Indicator parameter has been set - defaulte
      * to Odd BCD encoding.
      * @serial m_oddIndicator
      * A default serializable field.  */
    protected boolean m_oddIndicator = false;
    /** Whether the Odd/Event Indicator is set in this Global Title
      * Indicator.
      * @serial m_oddIndicatorIsSet
      * A default serializable field.  */
    protected boolean m_oddIndicatorIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
