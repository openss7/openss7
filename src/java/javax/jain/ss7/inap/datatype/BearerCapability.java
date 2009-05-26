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
 *  File Name     : BearerCapability.java
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
This Class defines the BearerCapability Datatype. 
*/

public class BearerCapability implements Serializable 
{
    private BearerCap bearerCap;
    
    private byte tmr;
    
    private BearerCapabilityChoice bearerCapabilityChoice;


/**
Constructors For BearerCapability
*/
	public BearerCapability(BearerCap bearerCap)  
	{
		setBearerCap(bearerCap);
	}

	public BearerCapability(byte tmr) 
	{
		setTmr(tmr);
	}


//--------------------------------------------------------------------------

/**
Gets BearerCapability Choice
*/

	public BearerCapabilityChoice getBearerCapabilityChoice()
		{ 
			return bearerCapabilityChoice;
		}

//--------------------------------------------------------------------------


/**
Gets Bearer Cap
*/

    public BearerCap getBearerCap() throws ParameterNotSetException
    {
        if(bearerCapabilityChoice.getBearerCapabilityChoice() == BearerCapabilityChoice.BEARER_CAP.getBearerCapabilityChoice())
		{
        	return  bearerCap;
        }
		else
		{
            throw new ParameterNotSetException();
        } 
    }

/**
Sets Bearer Cap
*/

    public void  setBearerCap(BearerCap bearercap) 
    {
        	this.bearerCap= bearercap;
        	bearerCapabilityChoice = BearerCapabilityChoice.BEARER_CAP;
    }

//--------------------------------------------------------------------------


/**
Gets Transmission Medium Requirement
*/

    public byte getTmr() throws ParameterNotSetException
    {
         if(bearerCapabilityChoice.getBearerCapabilityChoice() == BearerCapabilityChoice.TMR.getBearerCapabilityChoice())
		{
        	return  tmr;
        }
		else
		{
            throw new ParameterNotSetException();
        } 
    }

/**
Sets Transmission Medium Requirement
*/

    public void  setTmr(byte tmr) 
    {
        	this.tmr= tmr;
        	bearerCapabilityChoice = BearerCapabilityChoice.TMR;
    }


}
