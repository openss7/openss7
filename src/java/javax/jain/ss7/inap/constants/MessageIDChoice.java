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
 *  File Name     : MessageIDChoice.java
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
This class indicates the MessageID choice
*/


public class MessageIDChoice {


	/*
	* internal Constant available for switch statements 
	*/
	
    public static final int M_ELEMENTARY_MESSAGE_ID =0;
    public static final int M_TEXT =1;
    public static final int M_ELEMENTARY_MESSAGE_IDS =29;
    public static final int M_VARIABLE_MESSAGE=30;
	
	
	// internal variable to store constant value 
	
	private int messageIDChoice;
	
   
/**
Constructor for MessageID choice
*/

 
    private MessageIDChoice(int messageIDChoice) 
	{
	
		this.messageIDChoice= messageIDChoice;
	
	}
	
/**
* MessageIDChoice :ELEMENTARY_MESSAGE_ID
*/

public static final MessageIDChoice  ELEMENTARY_MESSAGE_ID =new MessageIDChoice (M_ELEMENTARY_MESSAGE_ID);

/**
* MessageIDChoice :ATTRIBUTES
*/

public static final MessageIDChoice TEXT=new MessageIDChoice (M_TEXT);


/**
* MessageIDChoice :ELEMENTARY_MESSAGE_IDS
*/

public static final MessageIDChoice  ELEMENTARY_MESSAGE_IDS =new MessageIDChoice (M_ELEMENTARY_MESSAGE_IDS);

/**
* MessageIDChoice :VARIABLE_MESSAGE
*/

public static final MessageIDChoice  VARIABLE_MESSAGE =new MessageIDChoice (M_VARIABLE_MESSAGE);


	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/

	public int getMessageIDChoice ()
	{
			return messageIDChoice;
	}
	
}
// end class
