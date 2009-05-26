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
 *  File Name     : MessageID.java
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
 This class defines the MessageId DataType
 */

public class MessageID implements Serializable
{

	 private int elementaryMessageID;


	 private int elementaryMessageIDs[];


     private Text text;

    
     private VariableMessage variableMessage;

  
	 private MessageIDChoice messageIDChoice;
			    

/**
Constructors For MessageId
*/
	public MessageID(int elementaryMessageID)
	{
		setElementaryMessageID(elementaryMessageID);
	}

	public MessageID(int elementaryMessageIDs[])
	{
		setElementaryMessageIDs(elementaryMessageIDs);
	}

	public MessageID(Text text)
	{
		setText(text);
	}

	public MessageID(VariableMessage variableMessage)
	{
		setVariableMessage(variableMessage);
	}

//-----------------------------------
 /**
  Gets Message ID Choice*/
	public MessageIDChoice getMessageIDChoice()
		{
			return messageIDChoice;
		}
    
/**
  Gets Elementary Message Id 
*/

    public int getElementaryMessageID() throws ParameterNotSetException
    {
        if(messageIDChoice.getMessageIDChoice()==MessageIDChoice.ELEMENTARY_MESSAGE_ID.getMessageIDChoice())
		{
        	return elementaryMessageID;
        }
		else
		{
            throw new ParameterNotSetException();
        }   
    }

/**
  Sets Elementary Message Id
*/
    public void setElementaryMessageID(int elementaryMessageID) 
    {
        this.elementaryMessageID = elementaryMessageID;
		messageIDChoice=MessageIDChoice.ELEMENTARY_MESSAGE_ID;	 
    }
    


//-----------------------

/**
  Gets Elementary Message Ids 
*/

    public int[] getElementaryMessageIDs() throws ParameterNotSetException
    {
        if(messageIDChoice.getMessageIDChoice()==MessageIDChoice.ELEMENTARY_MESSAGE_IDS.getMessageIDChoice())
		{
        	return elementaryMessageIDs;
        }
		else
		{
            throw new ParameterNotSetException();
        }   
    }

/**
Gets a particular Elementary Message Id
*/

    public int getElementaryMessageIDs(int index)
    {
        return elementaryMessageIDs[index];
    }

/**
  Sets Elementary Message Ids
*/
    public void setElementaryMessageIDs(int elementaryMessageIDs[])
    {
        this.elementaryMessageIDs = elementaryMessageIDs;
        messageIDChoice=MessageIDChoice.ELEMENTARY_MESSAGE_IDS;
    }

/**
Sets a particular Elementary Message Id
*/

    public void  setElementaryMessageIDs(int index , int elementaryMessageIDs)
    {
        this.elementaryMessageIDs[index] = elementaryMessageIDs ;
    }
    


//-----------------------
	 
/**
  Gets Text
*/

    public Text getText() throws ParameterNotSetException
    {
        if(messageIDChoice.getMessageIDChoice()==MessageIDChoice.TEXT.getMessageIDChoice())
		{
        	return text;
        }
		else
		{
            throw new ParameterNotSetException();
        }   
    }

/**
  Sets Text
*/
    public void setText(Text text)
    {
        this.text = text;
        messageIDChoice=MessageIDChoice.TEXT;
    }
    
//-----------------------

/**
  Gets Variable Message
*/

    public VariableMessage getVariableMessage() throws ParameterNotSetException
    {
        if( messageIDChoice==MessageIDChoice.VARIABLE_MESSAGE)
		{
        	return variableMessage;
        }
		else
		{
            throw new ParameterNotSetException();
        }   
    }

/**
  Sets Variable Message
*/
    public void setVariableMessage(VariableMessage variableMessage) 
    {
        this.variableMessage = variableMessage;
        messageIDChoice=MessageIDChoice.VARIABLE_MESSAGE;
    }
    

}
