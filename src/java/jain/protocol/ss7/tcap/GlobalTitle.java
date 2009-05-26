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
 * File Name     : GlobalTitle.java
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     01/11/2000  Phelim O'Doherty    Deprecated isAddressInfo present method
 *                                         and inserted replacement method for
 *                                         getAddressInformation method. Added
 *                                         new constructor.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7.tcap;

import jain.*;
/**
 * When instantiated this class represents a Global Title format which informs
 * the user that the GlobalTitle is not included, its Global Title Indicator is
 * a value '0000'.
 *
 * @author     Sun Microsystems Inc.
 * @version    1.1
 *
 * @deprecated As of JAIN TCAP v1.1. This class is no longer needed as a result
 * of the addition of the {@link jain.protocol.ss7.GlobalTitle} class.
 */
public class GlobalTitle implements java.io.Serializable {

    /**
    * Constructs a new Global Title of format GTINDICATOR_0000, which indicates that
    * the GlobalTitle is not included.
    */
    public GlobalTitle (){
    }

    /**
    * Gets the format of the Global Title. The Global Title format of this class is GTINDICATOR_0000.
    * The other Global Title formats that extend this class include:
    * <UL>
    *   <LI><B>GTINDICATOR_0001</B>
    *   <LI><B>GTINDICATOR_0010</B>
    *   <LI><B>GTINDICATOR_0011</B>
    *   <LI><B>GTINDICATOR_0100</B>
    * </UL>
    */
    public int getGTIndicator() {
        return jain.protocol.ss7.tcap.TcapConstants.GTINDICATOR_0000;
    }

    /**
    * Sets the Address Information of this Global Title.
    * @param <var>addressInformation</var> - the GlobalTitle Address
    * Information subparameter is composed of digits in the form of Binary
    * Coded Decimal(BCD).
    */
    public void setAddressInformation (byte[] addressInformation) {
        this.addressInformation=addressInformation;
        addressInformationPresent=true;
    }

    /**
    * Indicates if the Address Information is present.
    * @return  true if the Address Information has been set.
    */
    public boolean isAddressInformationPresent() {
        return addressInformationPresent;
    }

    /**
    * Gets the Address Information of this Global Title.
    * @return - the GlobalTitle Address Information subparameter is
    * composed of digits in the form of Binary
    * Coded Decimal(BCD).
    * @exception ParameterNotSetException this exception is thrown if this
    * parameter has not yet been set
    */
    public byte[] getAddressInformation () throws ParameterNotSetException {
        if (addressInformationPresent == true) {
            return addressInformation;
        } else {
            throw new ParameterNotSetException();
        }
    }

    /**
    * Clears all previously set parameters .
    */
    public void clearAllParameters(){
        addressInformationPresent = false;
    }

    /**
    * The Address Information of this Global Title Indicator
    * @serial addressInformation - a default serializable field
    */
    private byte[] addressInformation = null;

    /**
    * Indicates if the address Information parameter has been set
    * @serial addressInformationPresent - a default serializable field
    */
    private boolean addressInformationPresent = false;
}