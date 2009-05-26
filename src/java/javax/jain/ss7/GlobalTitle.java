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
 * Module Name   : JAIN Common API
 * File Name     : GlobalTitle.java
 * Originator    : Phelim O'Doherty
 * Approver      : Jain Community
 *
 *  HISTORY
 *  Version   Date      Author              Comments
 *  1.0     28/09/2001  Phelim O'Doherty    Initial version
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7;

import java.io.Serializable;
import javax.jain.*;

/**
 * When instantiated this class represents a Global Title format which informs
 * the user that the GlobalTitle is not included, its Global Title Indicator is
 * of value '0000'. The various Global Title Indicators that extend this class
 * are:
 *  <UL>
 *      <LI>GTINDICATOR_0000
 *      <LI>GTINDICATOR_0001
 *      <LI>GTINDICATOR_0010
 *      <LI>GTINDICATOR_0011
 *      <LI>GTINDICATOR_0100
 *  </UL>
 *
 * The complete combination of GlobalTitles included support the following
 * standards:
 *  <UL>
 *      <LI>ANSI SCCP T1.112 (1992)
 *      <LI>ANSI SCCP T1.112 (1996)
 *      <LI>ITU SCCP Q711-716 (03/1993)
 *      <LI>ITU SCCP Q711-716 (07/1996)
 *  </UL>
 *
 *
 * @author     Sun Microsystems Inc.
 * @version    1.0
 */
public class GlobalTitle implements Serializable {

    /**
    * Constructs a new Global Title of format GTINDICATOR_0000, which indicates
    * that the GlobalTitle is not included. That is no Address Information
    * included.
    */

    public GlobalTitle() {
    }

    /**
    * Sets the Address Information of this Global Title of the subclassed
    * GTIndicators.
    * <p>
    * <b>Note to developers</b> :- An Address Information is forbidden an
    * instantiation of this class, that is GTindicator-0000.
    *
    * @param  addressInformation  The new Address Information
    */
    public void setAddressInformation(byte[] addressInformation) {
	m_AddressInformation = addressInformation;
    }

    /**
    *  Gets the format of the Global Title. The Global Title format of this class
    *  is GTINDICATOR_0000.
    *
    * @return The Global Title Indicator value
    */

    public GTIndicator getGTIndicator() {
        return javax.jain.ss7.GTIndicator.GTINDICATOR_0000;
    }

    /**
    * Gets the Address Information of the sub-classed Global Title Indicators.
    * <p>
    *
    * <b>Note to developers</b> :- An Address Information is forbidden an
    * instantiation of this class, that is GTindicator-0000.
    *
    * @return - the GlobalTitle Address Information subparameter.
    * @exception  ParameterNotSetException  this exception is thrown if
    *      this method is invoked when the GlobalTitle is of type
    *      GTIndicator0000, that is an instantiation of the GlobalTitle class.
    */
    public byte[] getAddressInformation() throws ParameterNotSetException {
        if (javax.jain.ss7.GTIndicator.GTINDICATOR_0000 == getGTIndicator()) {
            throw new ParameterNotSetException();
        } else {
            return m_AddressInformation;
        }
    }

    /**
    * String representation of class GlobalTitle
    *
    * @return    String provides description of class GlobalTitle
    */
    public String toString() {
	StringBuffer buffer = new StringBuffer(500);
        buffer.append("\n\nGlobal Title Indicator: = GTINDICATOR_0000");
	buffer.append("\n\naddressInformation = ");
	if (this.m_AddressInformation != null) {
            buffer.append(this.m_AddressInformation.toString());
	}
	else {
	    buffer.append("value is null");
        }

	return buffer.toString();
    }

    /**
    * The Address Information of this Global Title Indicator
    *
    * @serial    m_AddressInformation - a default serializable field
    */
    private byte[] m_AddressInformation = null;


}
