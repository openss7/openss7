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
 *  File Name     : ForwardGVNS.java
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
This Class defines the Forward Global Virtual Network Service Format (Encoded as in Q.763)
*/

public class ForwardGVNS implements java.io.Serializable 
{

    private int oPSPLengthIndicator;
    private String oPSPDigit;
    private int gUGLengthIndicator;
    private String gUGDigit;
    private int numberingPlanIndicator;
    private int tNRNLengthIndicator;
    private int natureOfAddressIndicator;
    private String tNRNDigit;

/**
Constructor For ForwardGVNS
*/
	public ForwardGVNS(int oPSPLengthIndicator, java.lang.String oPSPDigit, 
  		  				int gUGLengthIndicator, java.lang.String gUGDigit, 
    					int numberingPlanIndicator, int tNRNLengthIndicator, 
    					int natureOfAddressIndicator, java.lang.String tNRNDigit)
	{
		setOPSPLengthIndicator (oPSPLengthIndicator);
    	setOPSPDigit (oPSPDigit);
    	setGUGLengthIndicator (gUGLengthIndicator);
    	setGUGDigit (gUGDigit);
    	setNumberingPlanIndicator (numberingPlanIndicator);
    	setTNRNLengthIndicator (tNRNLengthIndicator);
    	setNatureOfAddressIndicator (natureOfAddressIndicator);
    	setTNRNDigit (tNRNDigit);

	}

//-----------------------------------

/**
Gets Originating Participation Service Provider Indicator
*/
	public int getOPSPLengthIndicator()
    	{
        		return oPSPLengthIndicator;
    	}

/**
Sets Originating Participation Service Provider (OPSP) Indicator
*/

    public void  setOPSPLengthIndicator (int oPSPLengthIndicator)
    {
        this.oPSPLengthIndicator = oPSPLengthIndicator ;
    }


//-----------------------------------

/**
Gets OPSP Digits
*/

    public java.lang.String getOPSPDigit()
    {
        return oPSPDigit ;
    }

/**
Sets OPSP Digits
*/

    public void  setOPSPDigit (java.lang.String oPSPDigit)
    {
        this.oPSPDigit = oPSPDigit ;
    }

//-----------------------------------

/**
Gets GVNS User Group (GUG) Indicator
*/

    public int getGUGLengthIndicator()
    {
        return gUGLengthIndicator ;
    }

/**
Sets GVNS User Group Indicator
*/

    public void  setGUGLengthIndicator (int gUGLengthIndicator)
    {
        this.gUGLengthIndicator = gUGLengthIndicator ;
    }

//-----------------------------------

/**
Gets GUG Digits
*/

    public java.lang.String getGUGDigit()
    {
        return gUGDigit ;
    }

/**
Sets GUG Digits
*/

    public void  setGUGDigit (java.lang.String gUGDigit)
    {
        this.gUGDigit = gUGDigit ;
    }

//-----------------------------------

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

//-----------------------------------

/**
Gets Terminating Network Routing Number (TNRN) Length Indicator
*/

    public int getTNRNLengthIndicator()
    {
        return tNRNLengthIndicator ;
    }

/**
Sets Terminating Network Routing Number (TNRN) Length Indicator
*/

    public void  setTNRNLengthIndicator (int tNRNLengthIndicator)
    {
        this.tNRNLengthIndicator = tNRNLengthIndicator ;
    }


//-----------------------------------

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

//-----------------------------------

/**
Gets TNRN Digits
*/

    public java.lang.String getTNRNDigit()
    {
        return tNRNDigit ;
    }

/**
Sets TNRN Digits
*/

    public void  setTNRNDigit (java.lang.String tNRNDigit)
    {
        this.tNRNDigit = tNRNDigit ;
    }

//-----------------------------------

}
