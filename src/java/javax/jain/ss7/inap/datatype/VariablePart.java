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
 *  File Name     : VariablePart.java
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
 This class represents the Variable Part DataType
 */

public class VariablePart implements java.io.Serializable{

	 private int integer;
          
	 
	 private DigitsGenericDigits number;

	 
	 private String date;

	 
	 private String time;

	 
	 private String price;

	 private VariablePartChoice variablePartChoice;
	 
	 

/**
Constructor For VariablePart
*/
	public VariablePart(int integer)
	{
		setInteger(integer);
	}

	public VariablePart(DigitsGenericDigits number)
	{
		setNumber(number);
	}

	public VariablePart(java.lang.String value, VariablePartID variablepartid)
	{
		if(variablepartid.getVariablePartID() == VariablePartID.DATE.getVariablePartID())
			setDate(value);
		else if(variablepartid.getVariablePartID() == VariablePartID.TIME.getVariablePartID())
			setTime(value);
		else if(variablepartid.getVariablePartID() == VariablePartID.PRICE.getVariablePartID())
			setPrice(value);
	}

//-----------------------------------

/**
  Gets Integer
*/

    public int getInteger()
    {
        return integer;
    }

/**
  Sets Integer 
*/
    public void setInteger(int integer)
    {
        this.integer = integer;
        this.variablePartChoice=VariablePartChoice.INTEGER;
    }
    
    
    
//-----------------------

/**
  Gets Number
*/

    public DigitsGenericDigits getNumber()
    {
        return number;
    }

/**
  Sets Number
*/
    public void setNumber(DigitsGenericDigits number)
    {
        this.number = number;
        this.variablePartChoice=VariablePartChoice.NUMBER;

    }

    
//-----------------------

/**
  Gets Time
*/

    public java.lang.String getTime()
    {
        return time;
    }

/**
  Sets Time
*/
    public void setTime(java.lang.String time)
    {
        this.time = time;
        this.variablePartChoice=VariablePartChoice.TIME;

    }


//-----------------------

/**
  Gets Date
*/

    public java.lang.String getDate()
    {
        return date;
    }

/**
  Sets Date
*/
    public void setDate(java.lang.String date)
    {
        this.date = date;
        this.variablePartChoice=VariablePartChoice.DATE;

    }

//-----------------------
/**
  Gets Price
*/

    public java.lang.String getPrice()
    {
        return price;
    }

/**
  Sets Price
*/
    public void setPrice(java.lang.String price)
    {
        this.price = price;
        this.variablePartChoice=VariablePartChoice.PRICE;
    }
//-----------------------    

/**
  Gets VariableChoice
*/
    public VariablePartChoice getVariablePartChoice()
    {
        return variablePartChoice;
    }


//-----------------------   


}
