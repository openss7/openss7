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
 *  File Name     : ScfID.java
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
This Class defines the SCF ID (Encoded as in Q.713)
*/

 public class ScfID  implements Serializable {

    private int subSystemNumber;
    private int translationType;
    private int globalTranslationIndicator;
	private int numberinigPlan;
	private int encodingScheme;
	private int natureOfAddressIndicator;
	
	private boolean pointCodeIndicator;
	private boolean subSystemNumberIndicator;
	private boolean routeIndicator;
	
	private String countryCode;
	private String nsnDigit;

/**
Constructor For ScfID
*/
	public ScfID(boolean pointCodeIndicator, boolean subSystemNumberIndicator, int globalTranslationIndicator, boolean routeIndicator, int subSystemNumber, int translationType, int encodingScheme, int numberinigPlan, int natureOfAddressIndicator, String countryCode, String nsnDigit)
	{
		setPointCodeIndicator(pointCodeIndicator);
		setSubSystemNumberIndicator(subSystemNumberIndicator);
		setGlobalTranslationIndicator(globalTranslationIndicator);
		setRouteIndicator(routeIndicator);
		setSubSystemNumber(subSystemNumber);
		setTranslationType(translationType);
		setEncodingScheme(encodingScheme);
		setNumberinigPlan(numberinigPlan);
		setNatureOfAddressIndicator(natureOfAddressIndicator);
		setCountryCode(countryCode);
		setNsnDigit(nsnDigit);
	}
	
//---------------------------------------------	

/**
Gets  Point Code Indicator
*/
    public boolean getPointCodeIndicator()
    {
        return pointCodeIndicator ;
    }

/**
Sets  Point Code Indicator
*/

    public void  setPointCodeIndicator (boolean pointCodeIndicator)
    {
        this.pointCodeIndicator = pointCodeIndicator ;
    }
//---------------------------------------------	

/**
Gets  Sub System Number Indicator
*/
    public boolean getSubSystemNumberIndicator()
    {
        return subSystemNumberIndicator ;
    }

/**
Sets  Sub System Indicator
*/

    public void  setSubSystemNumberIndicator (boolean subSystemNumberIndicator)
    {
        this.subSystemNumberIndicator = subSystemNumberIndicator ;
    }
//---------------------------------------------	

/**
Gets  Global Translation Indicator
*/
    public int getGlobalTranslationIndicator()
    {
        return globalTranslationIndicator ;
    }

/**
Sets  Global Translation Indicator
*/

    public void  setGlobalTranslationIndicator (int globalTranslationIndicator)
    {
        this.globalTranslationIndicator = globalTranslationIndicator ;
    }
//---------------------------------------------	

/**
Gets  Sub Route Indicator
*/
    public boolean getRouteIndicator()
    {
        return routeIndicator ;
    }

/**
Sets  Sub Route Indicator
*/

    public void  setRouteIndicator (boolean routeIndicator)
    {
        this.routeIndicator = routeIndicator ;
    }
//---------------------------------------------	

/**
Gets  Sub System Number
*/

    public int getSubSystemNumber()
    {
        return subSystemNumber ;
    }

/**
Sets  Sub System Number
*/

    public void  setSubSystemNumber (int subSystemNumber)
    {
        this.subSystemNumber = subSystemNumber ;
    }
//---------------------------------------------	

/**
Gets  Translation Type
*/

    public int getTranslationType()
    {
        return translationType ;
    }

/**
Sets  Translation Type
*/

    public void  setTranslationType (int translationType)
    {
        this.translationType = translationType ;
    }
//---------------------------------------------	

/**
Gets  Encoding Scheme
*/

    public int getEncodingScheme()
    {
        return encodingScheme ;
    }

/**
Sets  Encoding Scheme
*/

    public void  setEncodingScheme (int encodingScheme)
    {
        this.encodingScheme = encodingScheme ;
    }
//---------------------------------------------	

/**
Gets  Numbering Plan
*/

    public int getNumberinigPlan()
    {
        return numberinigPlan ;
    }

/**
Sets  Numbering Plan
*/

    public void  setNumberinigPlan (int numberinigPlan)
    {
        this.numberinigPlan = numberinigPlan ;
    }
//---------------------------------------------	

/**
Gets  Nature Of Address Indicator
*/

    public int getNatureOfAddressIndicator()
    {
        return natureOfAddressIndicator ;
    }

/**
Sets  Nature Of Address Indicator
*/

    public void  setNatureOfAddressIndicator (int natureOfAddressIndicator)
    {
        this.natureOfAddressIndicator = natureOfAddressIndicator ;
    }
//---------------------------------------------	

/**
Gets  Country Code
*/

    public String getCountryCode()
    {
        return countryCode ;
    }

/**
Sets  Country Code
*/

    public void  setCountryCode (String countryCode)
    {
        this.countryCode = countryCode ;
    }
//---------------------------------------------	

/**
Gets  NSN Digit
*/

    public String getNsnDigit()
    {
        return nsnDigit ;
    }

/**
Sets  NSN Digit
*/

    public void  setNsnDigit (String nsnDigit)
    {
        this.nsnDigit = nsnDigit ;
    }
//---------------------------------------------	



}
