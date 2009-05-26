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
 * File Name     : GTIndicator0010.java
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     02/11/2000  Phelim O'Doherty    Deprecated getTranslationType for new
 *                                         get with MandantoryParameterNotSetException.
 *                                         Added new mandatory constructor.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7.tcap;

import jain.*;
/**
 * This class represents a Global Title Indicator-0010, it can be used to
 * store, create and retrieve GlobalTitle-0010 entries.
 *
 * @author     Sun Microsystems Inc.
 * @version    1.1
 * @see        GlobalTitle
 *
 * @deprecated As of JAIN TCAP v1.1. This class is no longer needed as a result
 * of the addition of the {@link jain.protocol.ss7.GTIndicator0010} class.
 */
public final class GTIndicator0010 extends GlobalTitle {

    /**
    * @deprecated As of JAIN TCAP v1.1, this empty constructor should no
    *      longer be used as all paramters are mandatory in the GT Indicators.
    */
    public GTIndicator0010() {
    }

    /**
    * Constructs a new Global Title Indicator-0010 with the mandatory fields
    * required for an ANSI and ITU implementation of the GTIndicator-0010.
    *
    * @param  translationType     the new translation type supplied to the constructor
    * @param  addressInformation  the new address Information supplied to the constructor
    * @since  JAIN TCAP v1.1
    */
    public GTIndicator0010(byte translationType, byte[] addressInformation) {
        setAddressInformation(addressInformation);
        setTranslationType(translationType);
    }

    /**
    * Sets the Translation Type of this Global Title Indicator-0010, which
    * directs the message to the appropriate global title translator. In the case
    * of GTIndicator-0010 the translation type may also imply the encoding
    * scheme, used to encode the address information and the numbering plan.
    *
    * @param  translationType The new Translation Type value
    */
    public void setTranslationType(byte translationType) {
        m_translationType = translationType;
        m_translationTypePresent = true;
    }

    /**
    * @deprecated  As of JAIN TCAP API v1.1, the translation type is mandatory
    *      therefore no longer needs an <code>isPresent method</code> .
    */
    public boolean isTranslationTypePresent() {
        return m_translationTypePresent;
    }

    /**
    * Gets the Translation Type of this Global Title Indicator-0010, which
    * directs the message to the appropriate global title translator. In the case
    * of GTIndicator-0010 the translation type may also imply the encoding
    * scheme, used to encode the address information and the numbering plan.
    *
    * @return     The Translation Type value
    * @exception  MandatoryParameterNotSetException  thrown when this
    *            mandatory parameter has not been set.
    * @since      JAIN TCAP v1.1
    */
    public byte getTranslationType() throws MandatoryParameterNotSetException {
        if (m_translationTypePresent == true) {
            return (m_translationType);
        } else {
            throw new MandatoryParameterNotSetException();
        }
    }

    /**
    * This method returns the format of this Global Title.
    *
    * @return  The Global Title Indicator value
    */
    public int getGTIndicator() {
        return (jain.protocol.ss7.tcap.TcapConstants.GTINDICATOR_0010);
    }

    /**
    * Clears all previously set parameters.
    */
    public void clearAllParameters() {
        super.clearAllParameters();
        m_translationTypePresent = false;
    }

    /**
    * String representation of class GTIndicator0010
    *
    * @return    String provides description of class GTIndicator0010
    */
    public String toString() {
        StringBuffer buffer = new StringBuffer(500);
        buffer.append("\n\nGlobal Title Indicator = GTINDICATOR_0010");
        buffer.append(super.toString());
        buffer.append("\n\ntranslationType = ");
        buffer.append(m_translationType);
        buffer.append("\n\ntranslationTypePresent = ");
        buffer.append(m_translationTypePresent);

        return buffer.toString();
    }

    /**
    * The Translation Type of this Global Title Indicator
    *
    * @serial    m_translationType - a default serializable field
    */
    private byte m_translationType = 0;

    /**
    * Indcates if the translation type parameter has been set
    *
    * @serial    m_translationTypePresent - a default serializable field
    */
    private boolean m_translationTypePresent = false;
}

