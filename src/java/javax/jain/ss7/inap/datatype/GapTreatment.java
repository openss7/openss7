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
 *  File Name     : GapTreatment.java
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
* This class represents the GapTreatment DataType*/
public class GapTreatment implements java.io.Serializable
{

	 private InformationToSend informationToSend;


	 private Cause releaseCause;

	 
	 private BothGapTreatmentParameters bothGapTreatmentParameters;


	 private GapTreatmentChoice gapTreatmentChoice;


/**
Constructors For GapTreatment
*/
	public GapTreatment(InformationToSend informationToSend)
	{
		setInformationToSend(informationToSend);
	}

	public GapTreatment(Cause releaseCause)
	{
		setReleaseCause( releaseCause);
	}

	public GapTreatment(BothGapTreatmentParameters bothGapTreatmentParameters)
	{
		setBothGapTreatmentParameters(bothGapTreatmentParameters);
	}

//-----------------------------------
/**
Gets Gap Treatment Choice
*/

public GapTreatmentChoice getGapTreatmentChoice()
	{ 
		return gapTreatmentChoice;
	}

/**
  Gets Information To Send 
*/

    public InformationToSend getInformationToSend() throws ParameterNotSetException
    {
        if(gapTreatmentChoice.getGapTreatmentChoice()==GapTreatmentChoice.INFORMATION_TO_SEND.getGapTreatmentChoice())
		{
        	return informationToSend;
        }
		else
		{
            throw new ParameterNotSetException();
        }    
    }

/**
  Sets Information To Send 
*/


    public void setInformationToSend(InformationToSend informationToSend)
    {
        this.informationToSend = informationToSend;
        gapTreatmentChoice=GapTreatmentChoice.INFORMATION_TO_SEND;
    }
    

//-----------------------


/**
  Gets Release Cause 
/*/

    public Cause getReleaseCause() throws ParameterNotSetException
    {
        if(gapTreatmentChoice.getGapTreatmentChoice()==GapTreatmentChoice.RELEASE_CAUSE.getGapTreatmentChoice())
		{
        	return releaseCause;
        }
		else
		{
            throw new ParameterNotSetException();
        }           
    }

/**
  Sets Release Cause
*/

    public void setReleaseCause( Cause releaseCause)
    {
        this.releaseCause = releaseCause;
        gapTreatmentChoice=GapTreatmentChoice.RELEASE_CAUSE;
    }

    

//-----------------------
    
/**
  Gets Both Gap Treatment Parameters 
*/

    public BothGapTreatmentParameters getBothGapTreatmentParameters() throws ParameterNotSetException
    {
       if(gapTreatmentChoice.getGapTreatmentChoice()==GapTreatmentChoice.BOTH.getGapTreatmentChoice())
	   { 
       		return bothGapTreatmentParameters;
       }
	   else
	   {
            throw new ParameterNotSetException();
       }   
    }

/**
  Sets Both Gap Treatment Parameters 
*/


    public void setBothGapTreatmentParameters(BothGapTreatmentParameters bothGapTreatmentParameters)
    {
       	this.bothGapTreatmentParameters = bothGapTreatmentParameters;
        gapTreatmentChoice=GapTreatmentChoice.BOTH;
    }
    
//-----------------------
    
}
