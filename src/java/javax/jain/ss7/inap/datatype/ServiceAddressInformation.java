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
 *  File Name     : ServiceAddressInformation.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

package javax.jain.ss7.inap.datatype;

import java.io.*;
import java.util.*;
import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.exception.*;
import javax.jain.*;
/**
This class defines the Service Address Information DataType
*/

public class ServiceAddressInformation  implements java.io.Serializable{

    private int serviceKey;
    private boolean isServiceKey = false ; 

    private MiscCallInfo miscCallInfo;

    private TriggerType triggerType;
    private boolean isTriggerType = false ;


/**
Constructor For ServiceAddressInformation
*/
	public ServiceAddressInformation(MiscCallInfo miscCallInfo)
	{
		setMiscCallInfo( miscCallInfo);
	}
	
//---------------------------------------------	

/**
Gets Service Key
*/

    public int  getServiceKey () throws ParameterNotSetException
    {
         if(isServiceKeyPresent ()){
         return serviceKey ;
         }else{
               throw new ParameterNotSetException();
              } 
    }

/**
Sets Service Key
*/


    public void setServiceKey ( int serviceKey ) throws IllegalArgumentException
    { 
      if((serviceKey>=0)&&(serviceKey<=2147483647))
       {
        this.serviceKey = serviceKey ;
        isServiceKey=true;
        }
        else
        {  
           throw new IllegalArgumentException("ParameterOutOfRange");
        }
    }


/**
Indicates if the Service Key optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean  isServiceKeyPresent ()
    {
        return isServiceKey ;
    }


//---------------------------------------------	

/**
Gets Misc Call Info
*/


    public MiscCallInfo  getMiscCallInfo()
    {
        return miscCallInfo;
    }

/**
Sets Misc Call Info
*/


    public void setMiscCallInfo( MiscCallInfo miscCallInfo )
    {
        this.miscCallInfo=miscCallInfo;
    }

//---------------------------------------------	

/**
Gets Trigger Type
*/


    public TriggerType  getTriggerType() throws ParameterNotSetException
    {
       if(isTriggerTypePresent ()){ 
       return triggerType;
        }else{
               throw new ParameterNotSetException();
              } 
    }

/**
Sets Trigger Type
*/


    public void setTriggerType ( TriggerType triggerType ) 

       {
        	this.triggerType  = triggerType;
			isTriggerType = true;

    }

/**
Indicates if the Trigger Type optional parameter is present .
Returns: TRUE if present , FALSE otherwise.
*/
    public boolean  isTriggerTypePresent ()
    {
        return isTriggerType ;
    }


}
