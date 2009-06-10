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
 *  File Name     : Entry.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.datatype;


import java.io.*;
import java.util.*;
import javax.jain.ss7.inap.constants.*;
import javax.jain.*;

/**
This Class defines the Entry Datatype
*/ 


public class Entry implements java.io.Serializable {

    private int agreements[];

    
    private int networkSpecific;


	 private EntryChoice entryChoice;

/**
Constructor For Entry
*/
	public Entry(int agreements[])
	{
		setAgreements (agreements);
	}

   public Entry(int networkSpecific)
	{
		setNetworkSpecific (networkSpecific);
	}
	
//---------------------------------------------	

/**
Gets the Entry Choice
*/
	public EntryChoice getEntryChoice()
	{ 
		return entryChoice;
	}

/**
Gets the Agreements Parameter
*/

    public int[] getAgreements() throws ParameterNotSetException
    {
       if(entryChoice.getEntryChoice() == EntryChoice.AGREEMENTS.getEntryChoice())
	   { 
       		return agreements;
       }
	   else
	   {
           	throw new ParameterNotSetException();
       }
    }

/**
Gets a particular  Agreements Parameter
*/

    public int getAgreements(int index)
    {
        return agreements[index];
    }

/**
Sets the Agreements Parameter
*/

    public void  setAgreements (int agreements[])
    {
        this.agreements = agreements ;
        entryChoice = EntryChoice.AGREEMENTS; 
    }

/**
Sets a particular Agreements Parameter
*/

    public void  setAgreements (int index , int agreements)
    {
        this.agreements[index] = agreements ;
    }


//---------------------------------------------	
/**
Gets the Network Specific Parameter
*/

    public int getNetworkSpecific() throws ParameterNotSetException
    {
        if(entryChoice.getEntryChoice() == EntryChoice.NETWORK_SPECIFIC.getEntryChoice())
		{
        	return networkSpecific ;
        }
		else
		{
            throw new ParameterNotSetException();
        }
    }

/**
Sets the Network Specific Parameter
*/

    public void  setNetworkSpecific (int networkSpecific)
    {
        this.networkSpecific= networkSpecific;
        entryChoice = EntryChoice.NETWORK_SPECIFIC;
    }




//---------------------------------------------	
}
