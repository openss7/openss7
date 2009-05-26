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
 * File Name     : GTIndicator0100.java
 * Originator    : Phelim O'Doherty [AePONA]
 * Approver      : Jain Community
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     20/11/2000  Phelim O'Doherty   Initial Version
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7;

import jain.protocol.ss7.*;
import jain.protocol.ss7.AddressConstants;
import jain.MandatoryParameterNotSetException;

/**
 * This class represents a Global Title Indicator-0100, it can be used to
 * store, create and retrieve GlobalTitle-0100 entries. It is specific only to
 * ITU, as it not assigned in US Networks. It supports the following protocol
 * standards:
 *    <UL>
 *        <LI>ITU SCCP Q711-716 (03/1993)
 *        <LI>ITU SCCP Q711-716 (07/1996)
 *    </UL>
 *
 * @author     Sun Microsystems Inc.
 * @version    1.1
 */

public final class GTIndicator0100 extends GlobalTitle {

    /**
    * Constructs a new Global Title Indicator-0100 with the relevent mandatory
    * parameters.
    *
    * @param  translationType     the new Translation Type supplied to the constructor
    * @param  numberingPlan       the new Numbering Plan supplied to the constructor
    * @param  encodingScheme      the new Encoding Scheme supplied to the constructor
    * @param  natureOfAddrInd     the new Nature Of Address Indicator supplied to the constructor
    * @param  addressInformation  the new Address Information supplied to the constructor
    */
    public GTIndicator0100(byte translationType, int numberingPlan,
                            int encodingScheme, int natureOfAddrInd,
                            byte[] addressInformation) {
        setAddressInformation(addressInformation);
        setTranslationType(translationType);
        setNumberingPlan(numberingPlan);
        setEncodingScheme(encodingScheme);
        setNatureOfAddrInd(natureOfAddrInd);
    }

    /**
    * Sets the Translation Type of this Global Title Indicator-0001, which
    * directs the message to the appropriate global title translator. The coding
    * and definition of the translation type for GTIndicator-0011 is for further
    * study.
    *
    * @param  translationType The new Translation Type value
    */
    public void setTranslationType(byte translationType) {
        m_TranslationType = translationType;
    }

    /**
    * Sets the Numbering Plan of this Global Title Indicator-0100.
    *
    * @param  numberingPlan one of the following:-
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
    * @see AddressConstants
    */
    public void setNumberingPlan(int numberingPlan) {
        m_NumberingPlan = numberingPlan;
    }

    /**
    *   Sets the Encoding Scheme of this Global Title Indicator-0100.
    *
    * @param  encodingScheme - one of the following:-
    *      <UL>
    *        <LI> ES_UNKNOWN - Indicates an unknown encoding scheme.
    *        <LI> ES_ODD - Indicates BCD, odd number of digits
    *        <LI> ES_EVEN - Indicates BCD, even number of digits
    *        <LI> ES_NATIONAL_SPECIFIC - Indicates a national specific
    *      </UL>
    * @see AddressConstants
    */
    public void setEncodingScheme(int encodingScheme) {
        m_EncodingScheme = encodingScheme;
    }

    /**
    * Sets the Nature of Address Indicator of this Global Title Indicator-0100.
    *
    * @param  natureOfAddrInd one of the following:
    *      <UL>
    *        <LI> NA_UNKNOWN - Indicates a unknown Nature of Address
    *        <LI> NA_SUBSCRIBER - Indicates a subscriber number
    *        <LI> NA_RESERVED - Reserved for national use Nature of Address Indicator
    *        <LI> NA_NATIONAL_SIGNIFICANT - Indicates a National Significant Number
    *        <LI> NA_INTERNATIONAL - Indicates an international number
    *      </UL>
    * @see AddressConstants
    */
    public void setNatureOfAddrInd(int natureOfAddrInd) {
        m_NatureOfAddrInd = natureOfAddrInd;
    }

    /**
    * Gets the Translation Type of this Global Title Indicator-0100, which
    * directs the message to the appropriate global title translator. The coding
    * and definition of the translation type for GTIndicator-0100 is for further
    * study.
    *
    * @return    The Translation Type value
    * @exception MandatoryParameterNotSetException thrown if this mandatory
    *           parameter has not been set
    */
    public byte getTranslationType() throws MandatoryParameterNotSetException {
        if (AddressConstants.NOT_SET != m_TranslationType) {
            return (m_TranslationType);
        } else {
            throw new MandatoryParameterNotSetException();
        }
    }

    /**
    * Gets the Numbering Plan of this Global Title Indicator-0100.
    *
    * @return the Numbering Plan - one of the following:-
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
    * @exception MandatoryParameterNotSetException thrown if this mandatory
    *           parameter has not been set
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
    * Gets the Encoding Scheme of this Global Title Indicator-0100.
    *
    * @return the Encoding Scheme - one of the following:-
    *      <UL>
    *        <LI> ES_UNKNOWN - Indicates an unknown encoding scheme.
    *        <LI> ES_ODD - Indicates BCD, odd number of digits
    *        <LI> ES_EVEN - Indicates BCD, even number of digits
    *        <LI> ES_NATIONAL_SPECIFIC - Indicates a national specific
    *      </UL>
    *
    * @exception MandatoryParameterNotSetException thrown if this mandatory
    *           parameter has not been set
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
    * Gets the Nature of Address Indicator of this Global Title Indicator-0100.
    *
    * @return The NatureOfAddrInd value - one of the following:
    *      <UL>
    *        <LI> NA_UNKNOWN - Indicates a unknown Nature of Address
    *        <LI> NA_SUBSCRIBER - Indicates a subscriber number
    *        <LI> NA_RESERVED - Reserved for national use Nature of Address Indicator
    *        <LI> NA_NATIONAL_SIGNIFICANT - Indicates a National Significant Number
    *        <LI> NA_INTERNATIONAL - Indicates an international number
    *      </UL>
    *
    * @exception MandatoryParameterNotSetException thrown if this mandatory
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
    * This method returns the format of this Global Title - GTINDICATOR_0100
    *
    * @return The Global Title Indicator value
    */
    public int getGTIndicator() {
        return (jain.protocol.ss7.AddressConstants.GTINDICATOR_0100);
    }

    /**
    * String representation of class GTIndicator0100
    *
    * @return    String provides description of class GTIndicator0100
    */
    public String toString() {
        StringBuffer buffer = new StringBuffer(500);
        buffer.append("\n\nGlobal Title Indicator = GTINDICATOR_0100");
        buffer.append(super.toString());
        buffer.append("\n\ntranslationType = ");
        buffer.append(this.m_TranslationType);
        buffer.append("\n\nnumberingPlan = ");
        buffer.append(this.m_NumberingPlan);
        buffer.append("\n\nnatureOfAddrInd = ");
        buffer.append(this.m_NatureOfAddrInd);
        buffer.append("\n\nencodingScheme = ");
        buffer.append(this.m_EncodingScheme);

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
    * The Nature of Address Indicator of this Global Title Indicator
    *
    * @serial    m_NatureOfAddrInd - a default serializable field
    */
    private int m_NatureOfAddrInd = AddressConstants.NOT_SET;

    /**
    * The Encoding Scheme of this Global Title Indicator
    *
    * @serial    m_EncodingScheme - a default serializable field
    */
    private int m_EncodingScheme = AddressConstants.NOT_SET;
}
