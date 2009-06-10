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
 *  File Name     : RequestedUTSI.java
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
This class defines the RequestedUTSI Datatype
*/

public class RequestedUTSI implements java.io.Serializable
{
	 
	 private String uSIServiceIndicator;
	
	 private USIMonitorMode uSIMonitorMode = null;	

    private LegID legID ;//= javax.jain.ss7.inap.datatype.LegID.sendingSideID;

/**
Constructor For RequestedUTSI
*/
	public RequestedUTSI(java.lang.String usiServiceIndicator, USIMonitorMode usiMonitorMode) 
	{
		setUSIServiceIndicator(usiServiceIndicator);
		setUSIMonitorMode(usiMonitorMode);
		
	}

//--------------------------------------	


/**
Gets USI Service Indicator
*/
    public java.lang.String  getUSIServiceIndicator()
    {
        return uSIServiceIndicator;
    }
    
/**
Sets USI Service Indicator
*/
    public void setUSIServiceIndicator(java.lang.String usiServiceIndicator)
    {
        this.uSIServiceIndicator = usiServiceIndicator;
    }

//--------------------------------------------    

/**
Gets USI Monitor Mode.
<DT> This parameter gets the monitoring relationship for teh specified UTSI IE.
<LI>MONITORING_ACTIVE
<LI>MONITORING_INACTIVE	
<P>
*/
    public USIMonitorMode  getUSIMonitorMode()
    {
        return uSIMonitorMode;
    }
    
/**
Sets USI Monitor Mode
*/
    public void setUSIMonitorMode(USIMonitorMode usiMonitorMode) 
    {
    
		this.uSIMonitorMode	= usiMonitorMode;

    }


//--------------------------------------------    
	 

/**
Gets Leg ID
*/
    public LegID getLegID()
    {
        return legID;
    }
    
/**
Sets Leg ID
*/
    public void setLegID(LegID legID)
    {
        this.legID = legID;
    }
//--------------------------------------------    

}
