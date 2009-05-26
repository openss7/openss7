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
 *  File Name     : GlobalTitle0011.java
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
This Class defines the Global Title 0011 datatype (Encoded as in Q.713)
*/

public class GlobalTitle0011 extends GlobalTitle implements Serializable 
{

    private int translationType;
    private int numberingPlan;
    private int encodingScheme;

/**
Constructor For GlobalTitle0011
*/
	public GlobalTitle0011(int translationType, int numberingPlan,  int encodingScheme,
				int globalTitleIndicator, String globalTitleAddress)
	{
		super(globalTitleIndicator, globalTitleAddress);
		setTranslationType(translationType);
		setNumberingPlan(numberingPlan);
		setEncodingScheme(encodingScheme);
	}
	
//------------------------------------------	

/**
Gets the Translation Type
*/
    public int getTranslationType()
    {
        return translationType ;
    }

/**
Sets the Translation Type
*/

    public void  setTranslationType(int translationType)
    {
        this.translationType = translationType ;
    }

//------------------------------------------	

/**
Gets the Numbering Plan
*/
    public int getNumberingPlan()
    {
        return numberingPlan ;
    }

/**
Sets the Numbering Plan
*/

    public void  setNumberingPlan(int numberingPlan)
    {
        this.numberingPlan = numberingPlan ;
    }

//------------------------------------------	

/**
Gets the Encoding Scheme
*/
    public int getEncodingScheme()
    {
        return encodingScheme ;
    }

/**
Sets the Encoding Scheme
*/

    public void  setEncodingScheme(int encodingScheme)
    {
        this.encodingScheme = encodingScheme ;
    }
//------------------------------------------	

}
