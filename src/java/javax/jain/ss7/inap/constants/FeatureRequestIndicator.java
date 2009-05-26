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
 *  File Name     : FeatureRequestIndicator.java
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
This class indicates the the feature activated (e.g. switch-hook flash, feature activation).
*/

public class FeatureRequestIndicator {

    public static final int M_HOLD=0;
    public static final int M_RETRIEVE=1;
    public static final int M_FEATURE_ACTIVATION=2;
    public static final int M_SPARE_1=3;
    public static final int M_SPARE_N=127;
    
	
	//internal variable to store the constant value
	
	private int featureRequestIndicator;
	
	
/**
Constructor for FeatureRequestIndicator datatype
*/
    
    private FeatureRequestIndicator(int featureRequestIndicator) 
	{
    	
		
		this.featureRequestIndicator=featureRequestIndicator;
	}
	
	
/*
* FeatureRequestIndicator : HOLD
*
*/

public static final FeatureRequestIndicator HOLD   =new FeatureRequestIndicator (M_HOLD);




	
/*
* FeatureRequestIndicator : RETRIEVE 
*
*/

public static final FeatureRequestIndicator RETRIEVE   =new FeatureRequestIndicator (M_RETRIEVE);


	
/*
* FeatureRequestIndicator :FEATURE_ACTIVATION
*
*/

public static final FeatureRequestIndicator FEATURE_ACTIVATION  =new FeatureRequestIndicator (M_FEATURE_ACTIVATION);



	
/*
* FeatureRequestIndicator :SPARE_1
*
*/

public static final FeatureRequestIndicator SPARE_1  =new FeatureRequestIndicator (M_SPARE_1);


	
/*
* FeatureRequestIndicator :SPARE_N
*
*/

public static final FeatureRequestIndicator SPARE_N  =new FeatureRequestIndicator (M_SPARE_N);




	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	public int getFeatureRequestIndicator ()
	{
	 	return featureRequestIndicator;
	}

}
//end class