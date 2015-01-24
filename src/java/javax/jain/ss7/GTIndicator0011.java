/*
 @(#) src/java/javax/jain/ss7/GTIndicator0011.java <p>
 
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
  * This class represents a Global Title Indicator-0011, it can be used
  * to store, create and retrieve GlobalTitle-0011 entries.
  * It is specific only to ITU, as it not assigned in US Networks. It
  * supports the following protocol standards: <ul>
  * <li>ITU SCCP Q711-716 (03/1993)
  * <li>ITU SCCP Q711-716 (07/1996) </ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  * @see GlobalTitle
  */
public final class GTIndicator0011 extends GlobalTitle {
    /**
      * Constructs a new ITU Global Title Indicator-0011 with the
      * relevent mandatory parameters.
      * @param translationType
      * The new Translation Type supplied to the constructor.
      * @param numberingPlan
      * The new Numbering Plan supplied to the constructor.
      * @param encodingScheme
      * The new Encoding Scheme supplied to the constructor.
      * @param addressInformation
      * The new Address Information supplied to the constructor.
      */
    public GTIndicator0011(byte translationType,
            NumberingPlan numberingPlan, EncodingScheme encodingScheme,
            byte[] addressInformation) {
        super(addressInformation);
        setTranslationType(translationType);
        setNumberingPlan(numberingPlan);
        setEncodingScheme(encodingScheme);
    }

    /**
      * Sets the Translation Type of this Global Title Indicator-0011,
      * which directs the message to the appropriate global title
      * translator.
      * The coding and definition of the translation type for
      * GTIndicator-0011 is for further study.
      * @param translationType
      * The new Translation Type value.
      */
    public void setTranslationType(byte translationType) {
        m_translationType = translationType;
        m_translationTypeIsSet = true;
    }
    /**
      * Sets the Numbering Plan of this Global Title Indicator-0011.
      * @param numberingPlan
      * One of the following:- <ul>
      * <li>{@link NumberingPlan#NP_UNKNOWN NP_UNKNOWN} - Indicates an unknown Numbering Plan.
      * <li>{@link NumberingPlan#NP_ISDN_TEL NP_ISDN_TEL} - Indicates ISDN/Telephony Numbering Plan.
      * <li>{@link NumberingPlan#NP_GENERIC NP_GENERIC} - Indicates a generic Numbering plan.
      * <li>{@link NumberingPlan#NP_DATA NP_DATA} - Indicates Data Numbering Plan.
      * <li>{@link NumberingPlan#NP_TELEX NP_TELEX} - Indicates a Telex Numbering Plan.
      * <li>{@link NumberingPlan#NP_MARITIME_MOBILE NP_MARITIME_MOBILE} - Indicates a Maritime Mobile Numbering Plan.
      * <li>{@link NumberingPlan#NP_LAND_MOBILE NP_LAND_MOBILE} - Indicates a Land Mobile Numbering Plan.
      * <li>{@link NumberingPlan#NP_ISDN_MOBILE NP_ISDN_MOBILE} - Indicates an ISDN Mobile Numbering Plan.
      * <li>{@link NumberingPlan#NP_NETWORK NP_NETWORK} - Indicates a Network Specific Numbering Plan. </ul>
      * @see NumberingPlan
      */
    public void setNumberingPlan(NumberingPlan numberingPlan) {
        m_numberingPlan = numberingPlan;
        m_numberingPlanIsSet = (numberingPlan != null);
    }
    /**
      * Sets the Encoding Scheme of this Global Title Indicator-0011.
      * @param encodingScheme
      * One of the following:- <ul>
      * <li>{@link EncodingScheme#ES_UNKNOWN ES_UNKNOWN} Indicates an unknown encoding scheme.
      * <li>{@link EncodingScheme#ES_ODD ES_ODD} Indicates BCD, odd number of digits.
      * <li>{@link EncodingScheme#ES_EVEN ES_EVEN} Indicates BCD, even number of digits.
      * <li>{@link EncodingScheme#ES_NATIONAL_SPECIFIC ES_NATIONAL_SPECIFIC} Indicates a national specific. </ul>
      * @see EncodingScheme
      */
    public void setEncodingScheme(EncodingScheme encodingScheme) {
        m_encodingScheme = encodingScheme;
        m_encodingSchemeIsSet = (encodingScheme != null);
    }
    /**
      * Gets the Translation Type of this Global Title Indicator-0001,
      * which directs the message to the appropriate global title
      * translator.
      * The coding and definition of the translation type for
      * GTIndicator-0011 is for further study.
      * @return
      * The Translation Type value.
      */
    public byte getTranslationType() {
        return m_translationType;
    }
    /**
      * Gets the Numbering Plan of this Global Title Indicator-0011.
      * @return
      * The Numbering Plan - one of the following:- <ul>
      * <li>{@link NumberingPlan#NP_UNKNOWN NP_UNKNOWN} - Indicates an unknown Numbering Plan.
      * <li>{@link NumberingPlan#NP_ISDN_TEL NP_ISDN_TEL} - Indicates ISDN/Telephony Numbering Plan.
      * <li>{@link NumberingPlan#NP_GENERIC NP_GENERIC} - Indicates a generic Numbering plan.
      * <li>{@link NumberingPlan#NP_DATA NP_DATA} - Indicates Data Numbering Plan.
      * <li>{@link NumberingPlan#NP_TELEX NP_TELEX} - Indicates a Telex Numbering Plan.
      * <li>{@link NumberingPlan#NP_MARITIME_MOBILE NP_MARITIME_MOBILE} - Indicates a Maritime Mobile Numbering Plan.
      * <li>{@link NumberingPlan#NP_LAND_MOBILE NP_LAND_MOBILE} - Indicates a Land Mobile Numbering Plan.
      * <li>{@link NumberingPlan#NP_ISDN_MOBILE NP_ISDN_MOBILE} - Indicates an ISDN Mobile Numbering Plan.
      * <li>{@link NumberingPlan#NP_NETWORK NP_NETWORK} - Indicates a Network Specific Numbering Plan. </ul>
      * @see NumberingPlan
      */
    public NumberingPlan getNumberingPlan() {
        return m_numberingPlan;
    }
    /**
      * Gets the Encoding Scheme of this Global Title Indicator-0011.
      * @return
      * The Encoding Scheme - one of the following:- <ul>
      * <li>{@link EncodingScheme#ES_UNKNOWN ES_UNKNOWN} Indicates an unknown encoding scheme.
      * <li>{@link EncodingScheme#ES_ODD ES_ODD} Indicates BCD, odd number of digits.
      * <li>{@link EncodingScheme#ES_EVEN ES_EVEN} Indicates BCD, even number of digits.
      * <li>{@link EncodingScheme#ES_NATIONAL_SPECIFIC ES_NATIONAL_SPECIFIC} Indicates a national specific. </ul>
      * @see EncodingScheme
      */
    public EncodingScheme getEncodingScheme() {
        return m_encodingScheme;
    }
    /**
      * This method returns the format of this Global Title -
      * GTINDICATOR_0011.
      * @return
      * The Global Title Indicator value.
      */
    public GTIndicator getGTIndicator() {
        return GTIndicator.GTINDICATOR_0011;
    }
    /**
      * java.lang.String representation of class GTIndicator0011.
      * @return
      * java.lang.String provides description of class GTIndicator0011.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.GTIndicator0011");
        b.append("\n\tGlobal Title Indicator: = GTINDICATOR_0011");
        b.append("\n\tm_translationType = " + m_translationType);
        b.append("\n\tm_translationTypeIsSet = " + m_translationTypeIsSet);
        b.append("\n\tm_numberingPlan = " + m_numberingPlan);
        b.append("\n\tm_numberingPlanIsSet = " + m_numberingPlanIsSet);
        b.append("\n\tm_encodingScheme = " + m_encodingScheme);
        b.append("\n\tm_encodingSchemeIsSet = " + m_encodingSchemeIsSet);
        return b.toString();
    }
    protected byte m_translationType = 0;
    protected boolean m_translationTypeIsSet = false;
    protected NumberingPlan m_numberingPlan = null;
    protected boolean m_numberingPlanIsSet = false;
    protected EncodingScheme m_encodingScheme = null;
    protected boolean m_encodingSchemeIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
