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
 *  File Name     : LegIDChoice.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.constants;


import java.io.*;
import java.util.*;
import java.lang.*;
/**
This class indicates the LegID choice
*/


public class LegIDChoice {

/**
*  Internal constant available for switch statements
*/


    public static final int M_SENDING_SIDE_ID =0;
    public static final int M_RECEIVING_SIDE_ID =1;
	
	// internal variable to store the constant value
	
	private int legIDChoice;
	
	       
/**
Constructor for LegID choice
*
*/

  
    private LegIDChoice(int legIDChoice)
	{
	
		this.legIDChoice=legIDChoice;
	
	}
	
/**
*  LegIDChoice  :SENDING_SIDE_ID
*/

public static final LegIDChoice SENDING_SIDE_ID  = new LegIDChoice (M_SENDING_SIDE_ID);


/**
*  LegIDChoice  :RECEIVING_SIDE_ID
*/

public static final LegIDChoice  RECEIVING_SIDE_ID = new LegIDChoice (M_RECEIVING_SIDE_ID);

	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	public int getLegIDChoice ()
	{
		return legIDChoice;
	}
		
}

