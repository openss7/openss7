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
 *  File Name     : InformationToSend.java
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
 This class represents the InformationToSend DataType
 */

public class InformationToSend implements java.io.Serializable
{

	 private InbandInfo inbandInfo;


	 private Tone tone;

	 
	 private String displayInformation;


	private InformationToSendChoice informationToSendChoice;


/**
Constructors For InformationToSend
*/
	public InformationToSend(InbandInfo inbandInfo)
	{
		setInbandInfo( inbandInfo);
	}

	public InformationToSend(Tone tone)
	{
		setTone(tone);
	}

	public InformationToSend(java.lang.String displayInformation)
	{
		setDisplayInformation( displayInformation);
	}

//-----------------------------------
 /**
Gets Gap Criteria Choice
*/

public InformationToSendChoice getInformationToSendChoice()
	{ 
		return informationToSendChoice;
	}

   
/**
  Gets In-Band Information
*/

    public InbandInfo getInbandInfo() throws ParameterNotSetException
    {
        if(informationToSendChoice.getInformationToSendChoice()==InformationToSendChoice.INBAND_INFO.getInformationToSendChoice())
		{
        	return inbandInfo;
        }
		else
		{
            throw new ParameterNotSetException();
        }    
    }
/**
  Sets In-Band Information
/*/

	 public void setInbandInfo( InbandInfo inbandInfo)
    {
        this.inbandInfo = inbandInfo;
        informationToSendChoice=InformationToSendChoice.INBAND_INFO;
    }

//-----------------------

/**
  Gets Tone Details
*/

    public Tone getTone() throws ParameterNotSetException
    {
        if(informationToSendChoice.getInformationToSendChoice()==InformationToSendChoice.TONE.getInformationToSendChoice())
		{
        	return tone;
        }
		else
		{
            throw new ParameterNotSetException();
        }    
    }

/**
  Sets Tone Details
*/
    public void setTone(Tone tone) 
    {
        this.tone= tone;
        informationToSendChoice=InformationToSendChoice.TONE;
    }
    

//-----------------------

/**
  Gets Display Information */

    public java.lang.String getDisplayInformation() throws ParameterNotSetException
    {
        if(informationToSendChoice.getInformationToSendChoice()==InformationToSendChoice.DISPLAY_INFORMATION.getInformationToSendChoice())
		{
        	return displayInformation;
        }
		else
		{
             throw new ParameterNotSetException();
        }   
    }

/**
  Sets Display Information
*/

    public void setDisplayInformation( String displayInformation)
    {
        this.displayInformation = displayInformation;
        informationToSendChoice=InformationToSendChoice.DISPLAY_INFORMATION;
    }
    

//-----------------------

    
}
