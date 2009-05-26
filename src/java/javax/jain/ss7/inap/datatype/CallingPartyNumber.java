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
 *  File Name     : CallingPartyNumber.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.datatype;


import java.io.*;
import java.util.*;
import java.lang.*;
import javax.jain.ss7.inap.constants.*;

/**
This Class defines the Calling Party Number Format (Encoded as in Q.763)
*/

public class CallingPartyNumber implements Serializable 
{


    private int natureOfAddressIndicator;
    
    private int numberIncompleteIndicator;
    
    private int numberingPlanIndicator;
    
    private int addressPresentationRestrictedIndicator;
    
    private int screeningIndicator;
    
    private String addressSignal;

/**
Constructor For CallingPartyNumber
*/
	public CallingPartyNumber(int natureOfAddressIndicator, 
								int numberIncompleteIndicator, 
								int numberingPlanIndicator,
								int addressPresentationRestrictedIndicator,
								int screeningIndicator,
								String addressSignal)
	{
		setNatureOfAddressIndicator (natureOfAddressIndicator);
		setNumberIncompleteIndicator (numberIncompleteIndicator);
		setNumberingPlanIndicator (numberingPlanIndicator);
		setAddressPresentationRestrictedIndicator (addressPresentationRestrictedIndicator);
		setScreeningIndicator (screeningIndicator);
		setAddressSignal (addressSignal);
	}

//---------------------------------------

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

//---------------------------------------

/**
Gets Number Incomplete Indicator
*/

    public int getNumberIncompleteIndicator()
    {
        return numberIncompleteIndicator ;
    }

/**
Sets Number Incomplete Indicator
*/

    public void  setNumberIncompleteIndicator (int numberIncompleteIndicator)
    {
        this.numberIncompleteIndicator = numberIncompleteIndicator ;
    }

//---------------------------------------

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

//---------------------------------------

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

//---------------------------------------

/**
Gets Screening Indicator
*/

    public int getScreeningIndicator()
    {
        return screeningIndicator ;
    }

/**
Sets Screening Indicator
*/

    public void  setScreeningIndicator (int screeningIndicator)
    {
        this.screeningIndicator = screeningIndicator ;
    }

//---------------------------------------

/**
Gets Address Signal
*/

    public String getAddressSignal()
    {
        return addressSignal ;
    }

/**
Sets Address Signal
*/

    public void  setAddressSignal (String addressSignal)
    {
        this.addressSignal = addressSignal ;
    }

//---------------------------------------

}
