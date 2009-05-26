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
 *  File Name     : HighLayerCompatibility.java
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
This Class defines the High layer Compatibitilty Format (Encoded as in Q.931)
*/

public class HighLayerCompatibility implements Serializable 
{


    private int codingStandard;
    
    private int interpretation;
    
    private int presentationMethodOfProtocolProfile;
    
    private String highLayerCharacteristicsIdentification;
    
    private String extendedHighLayerCharacteristicsIdentification;
    private boolean isExtendedHighLayerCharacteristicsIdentification = false ;


/**
Constructor For HighLayerCompatibility
*/
	public HighLayerCompatibility(int codingStandard, int interpretation, 
									int presentationMethodOfProtocolProfile, 
									String highLayerCharacteristicsIdentification)
	{
		setCodingStandard (codingStandard);
		setInterpretation (interpretation);
		setPresentationMethodOfProtocolProfile (presentationMethodOfProtocolProfile);
		setHighLayerCharacteristicsIdentification (highLayerCharacteristicsIdentification);
	}

//-----------------------------------

/**
Gets Coding Standard
*/

    public int getCodingStandard()
    {
        return codingStandard ;
    }

/**
Sets Coding Standard
*/

    public void  setCodingStandard (int codingStandard)
    {
        this.codingStandard = codingStandard ;
    }

//-----------------------------------

/**
Gets Interpretation
*/

    public int getInterpretation()
    {
        return interpretation;
    }

/**
Sets Interpretation
*/

    public void  setInterpretation (int interpretation)
    {
        this.interpretation = interpretation ;
    }

//-----------------------------------

/**
Gets Presentation Method Of Protocol Profile
*/

    public int getPresentationMethodOfProtocolProfile()
    {
        return presentationMethodOfProtocolProfile ;
    }

/**
Sets Presentation Method Of Protocol Profile
*/

    public void  setPresentationMethodOfProtocolProfile (int presentationMethodOfProtocolProfile)
    {
        this.presentationMethodOfProtocolProfile = presentationMethodOfProtocolProfile ;
    }


//-----------------------------------

/**
Gets High Layer Characteristics Identification
*/

    public String getHighLayerCharacteristicsIdentification()
    {
        return highLayerCharacteristicsIdentification ;
    }

/**
Sets High Layer Characteristics Identification
*/

    public void  setHighLayerCharacteristicsIdentification (String highLayerCharacteristicsIdentification)
    {
        this.highLayerCharacteristicsIdentification= highLayerCharacteristicsIdentification ;
    }

//-----------------------------------

/**
Gets Extended High Layer Characteristics Identification
*/

    public String getExtendedHighLayerCharacteristicsIdentification() throws ParameterNotSetException
    {
        if(isExtendedHighLayerCharacteristicsIdentificationPresent())
		{
        	return extendedHighLayerCharacteristicsIdentification ;
        }
		else
		{
            throw new ParameterNotSetException();
        }   
 
    }

/**
Sets Extended High Layer Characteristics Identification
*/

    public void  setExtendedHighLayerCharacteristicsIdentification (String extendedHighLayerCharacteristicsIdentification)
    {
        this.extendedHighLayerCharacteristicsIdentification= extendedHighLayerCharacteristicsIdentification ;
        isExtendedHighLayerCharacteristicsIdentification=true;
    }

/**
Indicates if the Extended High Layer Characteristics Identification optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean isExtendedHighLayerCharacteristicsIdentificationPresent()
    {
        return isExtendedHighLayerCharacteristicsIdentification;

    }


//-----------------------------------

}
