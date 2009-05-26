/*
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Module Name   : JAIN Common API
 * File Name     : GTIndicator0010.java
 * Originator    : Phelim O'Doherty
 * Approver      : Jain Community
 *
 *  HISTORY
 *  Version   Date      Author              Comments
 *  1.0     28/09/2001  Phelim O'Doherty    Initial version
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7;


import javax.jain.*;

/**
 * This class represents a Global Title Indicator-0010, it can be used to
 * store, create and retrieve GlobalTitle-0010 entries. It contains the
 * accessor methods for the following parameters in order to accommodate the
 * difference in the protocol variants supported, namely ANSI SCCP T1.112 (1992),
 * ANSI SCCP T1.112 (1996), ITU SCCP Q711-716 (03/1993) and ITU SCCP Q711-716
 * (07/1996):
 * <UL>
 *    <LI> Address Information for ITU and ANSI
 *    <LI> Translation Type for ITU and ANSI
 *  </UL>
 *
 * @author     Sun Microsystems Inc.
 * @version    1.0
 * @see        GlobalTitle
 */
public final class GTIndicator0010 extends GlobalTitle {

    /**
    * Constructs a new Global Title Indicator-0010 with the mandatory fields
    * required for an ANSI and ITU implementation of the GTIndicator-0010.
    *
    * @param  translationType     the new translation type supplied to the constructor
    * @param  addressInformation  the new address Information supplied to the constructor
    *
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
	m_TranslationType = translationType;
    }

    /**
    * Gets the Translation Type of this Global Title Indicator-0010, which
    * directs the message to the appropriate global title translator. In the case
    * of GTIndicator-0010 the translation type may also imply the encoding
    * scheme, used to encode the address information and the numbering plan.
    *
    * @return     The Translation Type value
    */
    public byte getTranslationType(){
        return (m_TranslationType);
    }

    /**
    * This method returns the format of this Global Title - GTINDICATOR_0010.
    *
    * @return  The Global Title Indicator value
    */
    public GTIndicator getGTIndicator() {
        return (javax.jain.ss7.GTIndicator.GTINDICATOR_0010);
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
	buffer.append(this.m_TranslationType);

	return buffer.toString();
    }

    /**
    * The Translation Type of this Global Title Indicator
    *
    * @serial    m_TranslationType - a default serializable field
    */
    private byte m_TranslationType = -1;
}
