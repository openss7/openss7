/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Copyrights:
 *
 *  Copyright - 1999 Sun Microsystems, Inc. All rights reserved.
 *  901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 *  This product and related documentation are protected by copyright and
 *  distributed under licenses restricting its use, copying, distribution, and
 *  decompilation. No part of this product or related documentation may be
 *  reproduced in any form by any means without prior written authorization of
 *  Sun and its licensors, if any.
 *
 *  RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 *  States Government is subject to the restrictions set forth in DFARS
 *  252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 *  The product described in this manual may be protected by one or more U.S.
 *  patents, foreign patents, or pending applications.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Author:
 *
 *  Mahindra British Telecom
 *  155 , Bombay - Pune Road 
 *  Pimpri ,
 *  Pune - 411 018.
 *
 *  Module Name   : JAIN INAP API
 *  File Name     : OriginalCalledPartyID.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.datatype;


import java.io.*;
import java.util.*;
import javax.jain.ss7.inap.constants.*;


/**
This Class defines the Original Called Party ID Datatype (Encoded  as Original Called Number in Q.763)
*/

public class OriginalCalledPartyID implements java.io.Serializable 
{


    private int natureOfAddressIndicator;
    private int numberingPlanIndicator;
    private int addressPresentationRestrictedIndicator;
    private String addressSignal;

/**
Constructor For OriginalCalledPartyID
*/
	public OriginalCalledPartyID(int natureOfAddressIndicator,
								int numberingPlanIndicator, 
								int addressPresentationRestrictedIndicator, 
								String addressSignal)
	{
		setNatureOfAddressIndicator (natureOfAddressIndicator);
		setNumberingPlanIndicator (numberingPlanIndicator);
		setAddressPresentationRestrictedIndicator (addressPresentationRestrictedIndicator);
		setAddressSignal (addressSignal);
	}
	
//---------------------------------------------	

/**
Gets Nature Of Address Indicator
*/

    public int getNatureOfAddressIndicator()
    {
        return natureOfAddressIndicator ;
    }

/**
Sets Nature Of Address Indicator
*/

    public void  setNatureOfAddressIndicator (int natureOfAddressIndicator)
    {
        this.natureOfAddressIndicator = natureOfAddressIndicator ;
    }

//---------------------------------------------	

/**
Gets Numbering Plan Indicator
*/

    public int getNumberingPlanIndicator()
    {
        return numberingPlanIndicator ;
    }

/**
Sets Numbering Plan Indicator
*/

    public void  setNumberingPlanIndicator (int numberingPlanIndicator)
    {
        this.numberingPlanIndicator = numberingPlanIndicator ;
    }

//---------------------------------------------	
/**
Gets Address Presentation Restricted Indicator
*/

    public int getAddressPresentationRestrictedIndicator()
    {
        return addressPresentationRestrictedIndicator ;
    }

/**
Sets Address Presentation Restricted Indicator
*/

    public void  setAddressPresentationRestrictedIndicator (int addressPresentationRestrictedIndicator)
    {
        this.addressPresentationRestrictedIndicator = addressPresentationRestrictedIndicator ;
    }

//---------------------------------------------	

/**
Gets Address Signal
*/

    public java.lang.String getAddressSignal()
    {
        return addressSignal ;
    }

/**
Sets Address Signal
*/

    public void  setAddressSignal (java.lang.String addressSignal)
    {
        this.addressSignal = addressSignal ;
    }

//---------------------------------------------	

}
