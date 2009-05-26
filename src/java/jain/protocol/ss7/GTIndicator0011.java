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
 * File Name     : GTIndicator0011.java
 * Originator    : Phelim O'Doherty [AePONA]
 * Approver      : Jain Community
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     20/11/2000  Phelim O'Doherty    Initial Version
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7;

import jain.protocol.ss7.*;
import jain.protocol.ss7.AddressConstants;
import jain.MandatoryParameterNotSetException;

/**
 * This class represents a Global Title Indicator-0011, it can be used to
 * store, create and retrieve GlobalTitle-0011 entries. It is specific only to
 * ITU, as it not assigned in US Networks. It supports the following protocol
 * standards:
 *    <UL>
 *        <LI>ITU SCCP Q711-716 (03/1993)
 *        <LI>ITU SCCP Q711-716 (07/1996)
 *    </UL>
 *
 * @author     Sun Microsystems Inc.
 * @version    1.1
 * @see        GlobalTitle
 */
public final class GTIndicator0011 extends GlobalTitle {

    /**
    * Constructs a new ITU Global Title Indicator-0011 with the relevent mandatory
    * parameters.
    *
    * @param  translationType     the new Translation Type supplied to the constructor
    * @param  numberingPlan       the new Numbering Plan supplied to the constructor
    * @param  encodingScheme      the new Encoding Scheme supplied to the constructor
    * @param  addressInformation  the new Address Information supplied to the constructor
    */
    public GTIndicator0011(byte translationType, int numberingPlan,
                                int encodingScheme, byte[] addressInformation) {
        setAddressInformation(addressInformation);
	setTranslationType(translationType);
	setNumberingPlan(numberingPlan);
	setEncodingScheme(encodingScheme);
    }

    /**
    * Sets the Translation Type of this Global Title Indicator-0011, which
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
    * Sets the Numbering Plan of this Global Title Indicator-0011.
    *
    * @param  numberingPlan one of the following:-
    *      <UL>
    *        <LI> NP_UNKNOWN - Indicates an unknown Numbering Plan
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
    * Sets the Encoding Scheme of this Global Title Indicator-0011.
    *
    * @param  encodingScheme one of the following:-
    *      <UL>
    *        <LI> <B>ES_UNKNOWN</B> Indicates an unknown encoding scheme.
    *        <LI> <B>ES_ODD</B> Indicates BCD, odd number of digits
    *        <LI> <B>ES_EVEN</B> Indicates BCD, even number of digits
    *        <LI> <B>ES_NATIONAL_SPECIFIC</B> Indicates a national specific
    *      </UL>
    * @see AddressConstants
    */
    public void setEncodingScheme(int encodingScheme) {
        m_EncodingScheme = encodingScheme;
    }

    /**
    * Gets the Translation Type of this Global Title Indicator-0001, which
    * directs the message to the appropriate global title translator. The coding
    * and definition of the translation type for GTIndicator-0011 is for further
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
    * Gets the Numbering Plan of this Global Title Indicator-0011.
    *
    * @return the Numbering Plan - one of the following:-
    *      <UL>
    *        <LI> NP_UNKNOWN - Indicates an unknown Numbering Plan
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
    * Gets the Encoding Scheme of this Global Title Indicator-0011.
    *
    * @return the Encoding Scheme - one of the following:-
    *      <UL>
    *        <LI> <B>ES_UNKNOWN</B> Indicates an unknown encoding scheme.
    *        <LI> <B>ES_ODD</B> Indicates BCD, odd number of digits
    *        <LI> <B>ES_EVEN</B> Indicates BCD, even number of digits
    *        <LI> <B>ES_NATIONAL_SPECIFIC</B> Indicates a national specific
    *      </UL>
    *
    * @exception MandatoryParameterNotSetException thrown if this mandatory
    *           parameter has not been set
    */
    public int getEncodingScheme() throws MandatoryParameterNotSetException {
        if (AddressConstants.NOT_SET != m_EncodingScheme) {
            return (m_EncodingScheme);
        } else {
            throw new MandatoryParameterNotSetException();
        }
    }


    /**
    * This method returns the format of this Global Title - GTINDICATOR_0011
    *
    * @return The Global Title Indicator value
    */
    public int getGTIndicator() {
        return (jain.protocol.ss7.AddressConstants.GTINDICATOR_0011);
    }

    /**
    * String representation of class GTIndicator0011
    *
    * @return    String provides description of class GTIndicator0011
    */
    public String toString() {
        StringBuffer buffer = new StringBuffer(500);
        buffer.append("\n\nGlobal Title Indicator = GTINDICATOR_0011");
        buffer.append(super.toString());
        buffer.append("\n\ntranslationType = ");
        buffer.append(m_TranslationType);
        buffer.append("\n\nnumberingPlan = ");
        buffer.append(m_NumberingPlan);
        buffer.append("\n\nencodingScheme = ");
        buffer.append(m_EncodingScheme);

        return buffer.toString();
    }

    /**
    * The Translation Type of this Global Title Indicator
    *
    * @serial    m-TranslationPresent - a default serializable field
    */
    private byte m_TranslationType = AddressConstants.NOT_SET;

    /**
    * The Numbering Plan of this Global Title Indicator
    *
    * @serial    m_NumberingPlan is a serialazable field
    */
    private int m_NumberingPlan = AddressConstants.NOT_SET;

    /**
    * The Encoding Scheme of this Global Title Indicator
    *
    * @serial    m_EncodingScheme - a default serialazable field
    */
    private int m_EncodingScheme = AddressConstants.NOT_SET;
}
