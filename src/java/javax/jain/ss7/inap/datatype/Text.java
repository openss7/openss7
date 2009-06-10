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
 *  File Name     : Text.java
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
 This class defines the Text Datatype
*/

public class Text implements java.io.Serializable{

	 private String messageContent;
	 
	 private String attributes;
	 private boolean isAttributes = false ;
    
/**
Constructor For Text
*/
	public Text(java.lang.String messageContent)
	{
		setMessageContent(messageContent);
	}
	
//------------------------------------------	
    
/**
  Gets Message Content
*/

    public java.lang.String getMessageContent()
    {
        return messageContent;
    }

/**
  Sets Message Content
*/
    public void setMessageContent(java.lang.String messageContent)
    {
        this.messageContent = messageContent;
    }
    
//-----------------------

/**
  Gets Attributes
*/

    public java.lang.String getAttributes() throws ParameterNotSetException
    {
        if(isAttributesPresent()){
        return attributes;
         }else{
               throw new ParameterNotSetException();
              }  
    }

/**
  Sets Attributes
*/
    public void setAttributes(java.lang.String attributes)
    {
        this.attributes = attributes;
        isAttributes=true;
    }
    
/**
Indicates if the Attributes optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isAttributesPresent()
    {
        return isAttributes;
    }

//-----------------------
	 
    
}
