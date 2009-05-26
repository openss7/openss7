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
 *  File Name     : ForwardCallIndicators.java
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
This Class defines the Forward Call Indicators Format (Encoded as in Q.763)
*/


public class ForwardCallIndicators implements Serializable 
{
	private boolean nationalInternationalCallIndicator;
	 
	private int endToEndMethodIndicator;
	 
	private boolean interworkingIndicator;
	 
	private boolean endToEndInformationIndicator;
	 
    private boolean iSDNUserPartIndicator;
    
    private int iSDNUserPartPreferenceIndicator;
    
    private boolean iSDNAccessIndicator;
    
    private int sCCPMethodIndicator;

/**
Constructor For ForwardCallIndicators
*/
	public ForwardCallIndicators(boolean nationalInternationalCallIndicator,
									int endToEndMethodIndicator,
									boolean interworkingIndicator,
									boolean endToEndInformationIndicator,
    								boolean iSDNUserPartIndicator,
    								int iSDNUserPartPreferenceIndicator,
    								boolean iSDNAccessIndicator,
    								int sCCPMethodIndicator)
	{
		setNationalInternationalCallIndicator (nationalInternationalCallIndicator);
		setEndToEndMethodIndicator(endToEndMethodIndicator);
		setInterworkingIndicator (interworkingIndicator);
		setEndToEndInformationIndicator(endToEndInformationIndicator);
    	setISDNUserPartIndicator(iSDNUserPartIndicator);
    	setISDNUserPartPreferenceIndicator(iSDNUserPartPreferenceIndicator);
	    setISDNAccessIndicator(iSDNAccessIndicator);
    	setSCCPMethodIndicator(sCCPMethodIndicator);
	}

//-----------------------------------

/**
Gets  National International Call Indicator
*/

    public boolean getNationalInternationalCallIndicator()
    {
        return nationalInternationalCallIndicator;
    }

/**
Sets  National International Call Indicator
*/

    public void setNationalInternationalCallIndicator (boolean nationalInternationalCallIndicator)
    {
      	this.nationalInternationalCallIndicator=nationalInternationalCallIndicator;
    }

//-----------------------------------

/**
Gets  End To End Method Indicator
*/

    public int getEndToEndMethodIndicator()
    {
        return endToEndMethodIndicator;
    }

/**
Sets   End To End Method Indicator
*/

    public void setEndToEndMethodIndicator(int endToEndMethodIndicator)
    {
      	this.endToEndMethodIndicator=endToEndMethodIndicator;
    }

//-----------------------------------


/**
Gets  Interworking Indicator
*/

    public boolean getInterworkingIndicator()
    {
        return interworkingIndicator;
    }

/**
Sets  Interworking Indicator
*/

    public void setInterworkingIndicator (boolean interworkingIndicator)
    {
      	this.interworkingIndicator=interworkingIndicator;
    }

//-----------------------------------

/**
Gets  End To End Method Information Indicator
*/

    public boolean getEndToEndInformationIndicator()
    {
        return endToEndInformationIndicator;
    }

/**
Sets  End To End Method Information Indicator
*/

    public void setEndToEndInformationIndicator(boolean endToEndInformationIndicator)
    {
      	this.endToEndInformationIndicator=endToEndInformationIndicator;
    }

//-----------------------------------

/**
Gets  ISDN UserPart Indicator
*/

    public boolean getISDNUserPartIndicator()
    {
        return iSDNUserPartIndicator;
    }

/**
Sets  ISDN UserPart Indicator
*/

    public void setISDNUserPartIndicator(boolean iSDNUserPartIndicator)
    {
      	this.iSDNUserPartIndicator=iSDNUserPartIndicator;
    }

//-----------------------------------

/**
Gets  ISDN UserPart Preference Indicator
*/

    public int getISDNUserPartPreferenceIndicator()
    {
        return iSDNUserPartPreferenceIndicator;
    }

/**
Sets  ISDN UserPart Preference Indicator
*/

    public void setISDNUserPartPreferenceIndicator(int iSDNUserPartPreferenceIndicator)
    {
      	this.iSDNUserPartPreferenceIndicator=iSDNUserPartPreferenceIndicator;
    }
//-----------------------------------

/**
Gets  ISDN Access Indicator
*/

    public boolean getISDNAccessIndicator()
    {
        return iSDNAccessIndicator;
    }

/**
Sets  ISDN Access Indicator
*/

    public void setISDNAccessIndicator(boolean iSDNAccessIndicator)
    {
      	this.iSDNAccessIndicator=iSDNAccessIndicator;
    }

//-----------------------------------

/**
Gets  SCCP Method Indicator
*/

    public int getSCCPMethodIndicator()
    {
        return sCCPMethodIndicator;
    }

/**
Sets  SCCP Method Indicator
*/

    public void setSCCPMethodIndicator(int sCCPMethodIndicator)
    {
      	this.sCCPMethodIndicator=sCCPMethodIndicator;
    }

//-----------------------------------

}