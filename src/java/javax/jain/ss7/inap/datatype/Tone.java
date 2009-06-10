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
 *  File Name     : Tone.java
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
 This class represents the Tone Datatype
*/

public class Tone implements java.io.Serializable{

	 private int toneID;

         private int duration;
	 private boolean isDuration = false ;

/**
Constructor For Tone
*/
	public Tone(int toneID)
	{
		setToneID(toneID);
	}
	
//---------------------------------------------	

/**
  Gets Tone Id
*/

    public int getToneID()
    {
        return toneID;
    }

/**
  Sets Tone Id
*/
    public void setToneID(int toneID)
    {
        this.toneID = toneID;
    }
    

//-----------------------
	 
    
/**
  Gets Duration*/

    public int getDuration() throws ParameterNotSetException
    {
        if(isDurationPresent()){
        return duration;
         }else{
               throw new ParameterNotSetException();
              }   
    }

/**
  Sets Duration 
*/
    public void setDuration(int duration)
    {
        this.duration = duration;
         isDuration=true;
    }
    
/**
Indicates if the Duration  optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isDurationPresent()
    {
        return isDuration;
    }

//-----------------------

    
}
