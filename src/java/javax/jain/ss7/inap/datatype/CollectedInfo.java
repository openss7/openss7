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
 *  File Name     : CollectedInfo.java
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
import javax.jain.*;

/**
This class defines the CollectedInfo Datatype
*/

public class CollectedInfo implements Serializable
{
    private CollectedDigits collectedDigits;


    private boolean isIA5Information =false ;


   private CollectedInfoChoice collectedInfoChoice;

   
/**
Constructor For CollectedInfo
*/
	public CollectedInfo(CollectedDigits collectedDigits)
	{
		setCollectedDigits(collectedDigits);
	}

	public CollectedInfo(boolean iA5Information)
	{
		setIA5Information(iA5Information);
	}

//-----------------------------------------
/**
Gets CollectedInfoChoice
*/
	public CollectedInfoChoice getCollectedInfoChoice()
		{ 
			return collectedInfoChoice;
		}


//------------------------------
/**
Gets Collected Digits
*/
    public CollectedDigits getCollectedDigits() throws ParameterNotSetException
    {
        if(collectedInfoChoice.getCollectedInfoChoice() ==CollectedInfoChoice.COLLECTED_DIGITS.getCollectedInfoChoice())
		{
         	return collectedDigits;
        }
		else
		{
                 throw new ParameterNotSetException();
		}           
    }
    
/**
Sets Collected Digits
*/
    public void setCollectedDigits(CollectedDigits collectedDigits)
    {
        
      this.collectedDigits = collectedDigits;
      collectedInfoChoice =CollectedInfoChoice.COLLECTED_DIGITS;  
    }


/**
Gets IA5Information 
*/
    public boolean getIA5Information() throws ParameterNotSetException
    {
        if(collectedInfoChoice.getCollectedInfoChoice() ==CollectedInfoChoice.IA5INFORMATION.getCollectedInfoChoice())
		{
        	return isIA5Information;
        }
		else
		{
            throw new ParameterNotSetException();
        }
    }
    
/**
Sets IA5Information 
*/
    public void setIA5Information(boolean iA5Information)
    {
        this.isIA5Information = isIA5Information;
        collectedInfoChoice =CollectedInfoChoice.IA5INFORMATION;
    }

    


}
