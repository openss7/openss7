/*
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Copyrights:
 *
 * Copyright - 1999 Sun Microsystems, Inc. All rights reserved.
 * 901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 * This product and related documentation are protected by copyright and
 * distributed under licenses restricting its use, copying, distribution, and
 * decompilation. No part of this product or related documentation may be
 * reproduced in any form by any means without prior written authorization of
 * Sun and its licensors, if any.
 *
 * RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 * States Government is subject to the restrictions set forth in DFARS
 * 252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 * The product described in this manual may be protected by one or more U.S.
 * patents, foreign patents, or pending applications.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Author:
 *
 * AePONA Limited, Interpoint Building
 * 20-24 York Street, Belfast BT15 1AQ
 * N. Ireland.
 *
 *
 * Module Name   : JAIN TCAP API
 * File Name     : GTIndicator0001.java
 * Originator    : Phelim O'Doherty [AePONA]
 * Approver      : Jain Community
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     20/11/2000  Phelim O'Doherty    Initial Version. Added isOddIndicator
 *                                         and setOddIndicator methods for
 *                                         Nature of Address Indicator.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7;

import jain.protocol.ss7.*;
import jain.protocol.ss7.AddressConstants;
import jain.MandatoryParameterNotSetException;

/**
 * This class represents a Global Title Indicator-0001, it can be used to
 * store, create and retrieve GlobalTitle-0001 entries. It contains the
 * accessor methods for the following parameters in order to accommodate the
 * difference in the protocol variants supported, namely ANSI SCCP T1.112 (1992),
 * ANSI SCCP T1.112 (1996), ITU SCCP Q711-716 (03/1993) and ITU SCCP Q711-716
 * (07/1996):
 *  <UL>
 *    <LI> Address Information for ITU and ANSI
 *    <LI> Translation Type for ANSI
 *    <LI> Numbering Plan for ANSI
 *    <LI> Encoding Scheme for ANSI
 *    <LI> Nature of Address Indicator for ITU
 *    <LI> Odd Indicator (bit 8 of Address Indicator) for ITU
 *  </UL>
 *  <BR>
 * For GT0001 address information it is expected that for non BCD numbers to be
 * raw data and when BCD encoding, to be 2 BCD numbers per byte (i.e 2
 * address_signals/digits/numbers per byte when BCD encoding). <BR>
 * The nature of address indicator for GT0001, only uses bits 1 to 7 for its
 * coding, therefore bit 8 of the octet contains the even/odd indicator which
 * is coded as 0 for even or 1 for odd number of
 * address_signals/digits/numbers. <BR>
 *
 *  <TABLE BORDER CELLSPACING=2 BORDERCOLOR="#000000" CELLPADDING=7 WIDTH=500>
 *    <CENTER>
 *    <TR>
 *      <TD WIDTH="48%" VALIGN="TOP" BGCOLOR="#ffffff">
 *        <B><P ALIGN="CENTER">
 *        Bit 8</B>
 *      </TD>
 *      <TD WIDTH="52%" VALIGN="TOP" BGCOLOR="#ffffff">
 *        <B><P ALIGN="CENTER">
 *        Odd/Even</B>
 *      </TD>
 *    </TR>
 *    <TR>
 *      <TD WIDTH="48%" VALIGN="TOP" BGCOLOR="#ffffff">
 *        <B><P ALIGN="CENTER">
 *
 *        0</B>
 *      </TD>
 *      <TD WIDTH="52%" VALIGN="TOP" BGCOLOR="#ffffff">
 *        <B><P ALIGN="CENTER">
 *
 *        Even number of address signals</B>
 *      </TD>
 *    </TR>
 *    <TR>
 *      <TD WIDTH="48%" VALIGN="TOP" BGCOLOR="#ffffff">
 *        <B><P ALIGN="CENTER">
 *        1</B>
 *      </TD>
 *      <TD WIDTH="52%" VALIGN="TOP" BGCOLOR="#ffffff">
 *        <B><P ALIGN="CENTER">
 *
 *        Odd number of address signals</B>
 *      </TD>
 *    </TR>
 *
 *  </TABLE>
 *  </CENTER><br>
 * GTIndicator0001 has a boolean oddIndicator, which can be used to
 * determine the contents of the even/odd Indicator field. The <code>
 * isOddIndicator</code> will return <var>true</var> when there is an odd
 * number of address_signals/digits/numbers and <var>false</var> when there is
 * an even number of address_signals/digits/numbers.
 *  <br>
 *  <B><FONT FACE="Courier New"><P><center>
 *  <IMG SRC=/doc-files/gt0001.jpg></P></FONT></B> <br>
 *  Global title format for Indicator 0001</center>
 *
 * @author     Sun Microsystems Inc.
 * @version    1.1
 * @see        GlobalTitle
 */
public final class GTIndicator0001 extends GlobalTitle {

    /**
    * Constructs a new Global Title Indicator-0001 for an <b>ITU implementation
    * </b>of the JAIN TCAP API with the relevent mandatory parameters.
    *
    * @param  natureAddrIndicator  the new nature of address supplied to the constructor
    * @param  isOddIndicator       the new Odd Indicator supplied to the constructor
    * @param  addressInformation   the new address Information supplied to the constructor
    */
    public GTIndicator0001(int natureAddrIndicator, boolean isOddIndicator,
                            byte[] addressInformation) {
        setAddressInformation(addressInformation);
        setNatureOfAddrInd(natureAddrIndicator);
        setOddIndicator(isOddIndicator);
    }

    /**
    * Constructs a new Global Title Indicator-0001 for an <b>ANSI implementation
    * </b>of the JAIN TCAP API with the relevent mandatory parameters.
    *
    * @param  translationType     the new translation type supplied to the constructor
    * @param  numberingPlam       the new numbering Plan supplied to the constructor
    * @param  encodingScheme      the new encoding scheme supplied to the constructor
    * @param  addressInformation  the new address Information supplied to the constructor
    */
    public GTIndicator0001(byte translationType, int numberingPlan,
                            int encodingScheme, byte[] addressInformation) {
        setAddressInformation(addressInformation);
        setTranslationType(translationType);
        setNumberingPlan(numberingPlan);
        setEncodingScheme(encodingScheme);
    }

    /**
    * Sets the Translation Type of this Global Title Indicator-0001, which
    * directs the message to the appropriate global title translator. The
    * translation type is only specific to an ANSI implementation of the JAIN
    * TCAP API for GTIndicator-0001.
    *
    * @param  translationType The new Translation Type value
    */
    public void setTranslationType(byte translationType) {
        m_TranslationType = translationType;
    }

    /**
    * Sets the Numbering Plan of this Global Title Indicator-0001. The numbering
    * plan is only specific to an ANSI implementation of the JAIN TCAP API for
    * GTIndicator-0001.
    *
    * @param  numberingPlan  one of the following:-
    *      <UL>
    *        <LI> NP_UNKOWN - Indicates an unknown Numbering Plan
    *        <LI> NP_ISDN_TEL - Indicates ISDN/Telephony Numbering Plan
    *        <LI> NP_RESERVED - Indicates a reserved Numbering Plan.
    *        <LI> NP_DATA - Indicates Data Numbering Plan
    *        <LI> NP_TELEX - Indicates a Telex Numbering Plan
    *        <LI> NP_MARITIME_MOBILE - Indicates a Maritime Mobile Numbering Plan
    *        <LI> NP_LAND_MOBILE - Indicates a Land Mobile Numbering Plan
    *        <LI> NP_ISDN_MOBILE - Indicates an ISDN Mobile Numbering Plan
    *      </UL>
    * @see AddressConstants
    */
    public void setNumberingPlan(int numberingPlan) {
        m_NumberingPlan = numberingPlan;
    }

    /**
    * Sets the Encoding Scheme of this Global Title Indicator-0001. The encoding
    * scheme is only specific to an ANSI implementation of the JAIN TCAP API for
    * GTIndicator-0001.
    *
    * @param  encodingScheme one of the following:-
    *      <UL>
    *        <LI> ES_UNKNOWN - Indicates an unknown encoding scheme.
    *        <LI> ES_ODD - Indicates BCD, odd number of digits
    *        <LI> ES_EVEN - Indicates - BCD, even number of digits
    *        <LI> ES_NATIONAL_SPECIFIC - Indicates a national specific
    *      </UL>
    * @see AddressConstants
    */
    public void setEncodingScheme(int encodingScheme) {
        m_EncodingScheme = encodingScheme;
    }

    /**
    * Sets the Nature of Address Indicator of this Global Title Indicator-0001.
    * The Nature of address indicator is only specific to the ITU implementation
    * of the JAIN TCAP API.
    *
    * @param  natureOfAddrInd The new Nature Of Address Indicator value
    */
    public void setNatureOfAddrInd(int natureOfAddrInd) {
        m_NatureOfAddrInd = natureOfAddrInd;
    }

    /**
    * Sets the Odd Indicator, bit 8 of the Nature of Address Indicator. The Odd
    * Indicator is only specific to the ITU version of GTIndicator-0001. This
    * field is defaulted to false therefore "Odd BCD Encoding".
    *
    * @param  oddIndicator  one of the following:-
    *      <UL>
    *        <LI> <var>true</var> - if Even BCD Encoding.
    *        <LI> <var>false</var> - if Odd BCD Encoding.
    *      </UL>
    */
    public void setOddIndicator(boolean oddIndicator) {
        m_OddIndicator = oddIndicator;
    }

    /**
    * Gets the Translation Type of this Global Title Indicator-0001, which
    * directs the message to the appropriate global title translator. The
    * translation type is only specific to an ANSI implementation of the JAIN
    * TCAP API for GTIndicator-0001.
    *
    * @return The Translation Type value
    * @exception  MandatoryParameterNotSetException thrown when this parameter
    *        has not been set.
    */
    public byte getTranslationType() throws MandatoryParameterNotSetException {
        if (AddressConstants.NOT_SET != m_TranslationType) {
            return (m_TranslationType);
        } else {
            throw new MandatoryParameterNotSetException();
        }
    }

    /**
    * Gets the Numbering Plan of this Global Title Indicator-0001. The numbering
    * plan is only specific to an ANSI implementation of the JAIN TCAP API for
    * GTIndicator-0001.
    *
    * @return The Numbering Plan - one of the following:-
    *      <UL>
    *        <LI> NP_UNKOWN - Indicates an unknown Numbering Plan
    *        <LI> NP_ISDN_TEL - Indicates ISDN/Telephony Numbering Plan
    *        <LI> NP_RESERVED - Indicates a reserved Numbering plan in ANSI.
    *        <LI> NP_DATA - Indicates Data Numbering Plan
    *        <LI> NP_TELEX - Indicates a Telex Numbering Plan
    *        <LI> NP_MARITIME_MOBILE - Indicates a Maritime Mobile Numbering Plan
    *        <LI> NP_LAND_MOBILE - Indicates a Land Mobile Numbering Plan
    *        <LI> NP_ISDN_MOBILE - Indicates an ISDN Mobile Numbering Plan
    *      </UL>
    *
    * @exception MandatoryParameterNotSetException thrown when this mandatory parameter
    *           has not been set.
    * @see AddressConstants
    */
    public int getNumberingPlan() throws MandatoryParameterNotSetException {
        if (AddressConstants.NOT_SET != m_NumberingPlan) {
            return (m_NumberingPlan);
        } else {
            throw new MandatoryParameterNotSetException();
        }
    }

    /**
    * Gets the Encoding Scheme of this Global Title Indicator-0001. The encoding
    * scheme is only specific to an ANSI implementation of the JAIN TCAP API for
    * GTIndicator-0001.
    *
    * @return the Encoding Scheme - one of the following:-
    *      <UL>
    *        <LI> ES_UNKNOWN - Indicates an unknown encoding scheme.
    *        <LI> ES_ODD - Indicates BCD, odd number of digits
    *        <LI> ES_EVEN - Indicates BCD, even number of digits
    *        <LI> <B>ES_NATIONAL_SPECIFIC - Indicates a national specific
    *      </UL>
    *
    * @exception MandatoryParameterNotSetException thrown if this mandatory
    *           parameter has not been set.
    * @see AddressConstants
    */
    public int getEncodingScheme() throws MandatoryParameterNotSetException {
        if (AddressConstants.NOT_SET != m_EncodingScheme) {
            return (m_EncodingScheme);
        } else {
            throw new MandatoryParameterNotSetException();
        }
    }

    /**
    * Gets the Nature of Address Indicator of this Global Title Indicator-0001.
    * The nature of address indicator is only specific to the ITU implementation
    * of the JAIN TCAP API.
    *
    * @return - one of the following:
    *      <UL>
    *        <LI> NA_UNKNOWN - Indicates a unknown Nature of Address
    *        <LI> NA_SUBSCRIBER - Indicates a subscriber number
    *        <LI> NA_RESERVED - Reserved for national use Nature of Address Indicator
    *        <LI> NA_NATIONAL_SIGNIFICANT - Indicates a National Significant Number
    *        <LI> NA_INTERNATIONAL - Indicates an international number
    *      </UL>
    *
    * @exception  MandatoryParameterNotSetException thrown if this mandatory
    *           parameter has not been set
    * @see AddressConstants
    */
    public int getNatureOfAddrInd() throws MandatoryParameterNotSetException {
        if (AddressConstants.NOT_SET != m_NatureOfAddrInd) {
            return (m_NatureOfAddrInd);
        } else {
            throw new MandatoryParameterNotSetException();
        }
    }

    /**
    * Gets the Odd Indicator, bit 8 of the Nature of Address Indicator. The Odd
    * Indicator is only specific to the ITU version of the GTIndicator-0001. This
    * field is defaulted to false therefore "Odd BCD Encoding".
    *
    * @return    one of the following:-
    *      <UL>
    *        <LI> true  - if Even BCD Encoding.
    *        <LI> false - if Odd BCD Encoding.
    *      </UL>
    */
    public boolean isOddIndicator() {
        return m_OddIndicator;
    }

    /**
    * This method returns the format of this Global Title - GTINDICATOR-0001
    *
    * @return    The Global Title Indicator value
    */
    public int getGTIndicator() {
        return (jain.protocol.ss7.AddressConstants.GTINDICATOR_0001);
    }

    /**
    * String representation of class GTIndicator0001
    *
    * @return    String provides description of class GTIndicator0001
    */
    public String toString() {
        StringBuffer buffer = new StringBuffer(500);
        buffer.append("\n\nGlobal Title Indicator = GTINDICATOR_0001");
        buffer.append(super.toString());
        buffer.append("\n\ntranslationType = ");
        buffer.append(this.m_TranslationType);
        buffer.append("\n\nnumberingPlan = ");
        buffer.append(this.m_NumberingPlan);
        buffer.append("\n\nencodingScheme = ");
        buffer.append(this.m_EncodingScheme);
        buffer.append("\n\nnatureOfAddrInd = ");
        buffer.append(this.m_NatureOfAddrInd);
        buffer.append("\n\noddIndicator = ");
        buffer.append(this.m_OddIndicator);

        return buffer.toString();
    }

    /**
    * The Translation Type of this Global Title Indicator
    *
    * @serial    m_TranslationType - a default serializable field
    */
    private byte m_TranslationType = AddressConstants.NOT_SET;

    /**
    * The Numbering Plan of this Global Title Indicator
    *
    * @serial    m_NumberingPlan - a default serializable field
    */
    private int m_NumberingPlan = AddressConstants.NOT_SET;

    /**
    * The Encoding Scheme of this Global Title Indicator
    *
    * @serial    m_EncodingScheme - a default serializable field
    */
    private int m_EncodingScheme = AddressConstants.NOT_SET;

    /**
    * The Nature of Address Indicator of this Global Title Indicator
    *
    * @serial    m_NatureOfAddrInd - a default serializable field
    */
    private int m_NatureOfAddrInd = AddressConstants.NOT_SET;

    /**
    * Indicates if the Odd Indicator parameter has been set - defaulte to Odd BCD
    * encoding.
    *
    * @serial    m_OddIndicator - a default serializable field
    */
    private boolean m_OddIndicator = false;
}

