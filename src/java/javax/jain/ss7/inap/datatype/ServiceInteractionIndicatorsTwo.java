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
 *  File Name     : ServiceInteractionIndicatorsTwo.java
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
import javax.jain.ss7.inap.exception.*;
import javax.jain.*;

/**
This Class defines the ServiceInteractionIndicatorsTwo Datatype
*/

public class ServiceInteractionIndicatorsTwo  implements Serializable {

    private ForwardServiceInteractionInd	 forwardServiceInteractionInd;
	 private boolean isForwardServiceInteractionInd = false ;
    
	 private BackwardServiceInteractionInd	 backwardServiceInteractionInd;
	 private boolean isBackwardServiceInteractionInd = false ;
	 
 	 private int bothwayThroughConnectionInd;
	 private boolean isBothwayThroughConnectionInd = false ;
	  
	 private int	suspendTimer;
	 private boolean isSuspendTimer = false ;
	 
	 private int	connectedNumberTreatmentInd;
	 private boolean isConnectedNumberTreatmentInd = false ;
	 
	 private boolean suppressCallDiversionNotification;
	 private boolean isSuppressCallDiversionNotification = false ;
	 
	 private boolean suppressCallTransferNotification;
	 private boolean isSuppressCallTransferNotification = false ;
	 
	 private boolean allowCdINNoPresentationInd;
	 private boolean isAllowCdINNoPresentationInd = false ;
	 
	 private boolean userDialogueDurationInd = true	;

/**
Gets Forward Service Interaction Indicator
*/

    public ForwardServiceInteractionInd getForwardServiceInteractionInd() throws ParameterNotSetException
    {
        if(isForwardServiceInteractionIndPresent ()){
        return forwardServiceInteractionInd;
        }else{
               throw new ParameterNotSetException();
              } 
    }

/**
Sets Forward Service Interaction Indicator
*/


    public void setForwardServiceInteractionInd (ForwardServiceInteractionInd forwardServiceInteractionInd)
    {
        this.forwardServiceInteractionInd = forwardServiceInteractionInd;
        isForwardServiceInteractionInd=true;
    }


/**
Indicates if the Forward Service Interaction Indicator optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean  isForwardServiceInteractionIndPresent ()
    {
        return isForwardServiceInteractionInd;
    }


//----------------------------------------------------

/**
Gets Backward Service Interaction Indicator
*/

   public BackwardServiceInteractionInd getBackwardServiceInteractionInd() throws ParameterNotSetException
    {
        if(isBackwardServiceInteractionIndPresent ()){
        return backwardServiceInteractionInd;
        }else{
               throw new ParameterNotSetException();
              } 
    }

/**
Sets Backward Service Interaction Indicator
*/

    public void setBackwardServiceInteractionInd (BackwardServiceInteractionInd backwardServiceInteractionInd)
    {
        this.backwardServiceInteractionInd = backwardServiceInteractionInd;
        isBackwardServiceInteractionInd=true;
    }

/**
Indicates if the Backward Service Interaction Indicator optional parameter is present .
Returns: TRUE if present , FALSE otherwise.
*/
    public boolean  isBackwardServiceInteractionIndPresent ()
    {
        return isBackwardServiceInteractionInd;
    }


//----------------------------------------------------

/**
Gets Bothway Through Connection Indicator
*/

    public int getBothwayThroughConnectionInd() throws ParameterNotSetException
    {
        if(isBothwayThroughConnectionIndPresent ()){
        return bothwayThroughConnectionInd;
        }else{
               throw new ParameterNotSetException();
              } 
    }

/**
Sets Bothway Through Connection Indicator
*/

    public void setBothwayThroughConnectionInd(int bothwayThroughConnectionInd)
    {
        this.bothwayThroughConnectionInd = bothwayThroughConnectionInd;
        isBothwayThroughConnectionInd=true;
    }

/**
Indicates if the Bothway Through Connection Indicator optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean  isBothwayThroughConnectionIndPresent ()
    {
        return isBothwayThroughConnectionInd;
    }


//----------------------------------------------------

/**
Gets Suspend Timer
*/

    public int getSuspendTimer() throws ParameterNotSetException
    {
       if(isSuspendTimerPresent ()){ 
       return suspendTimer;
        }else{
               throw new ParameterNotSetException();
              } 
    }

/**
Sets Suspend Timer
*/

    public void setSuspendTimer(int suspendTimer) throws IllegalArgumentException
    {
    	if((suspendTimer>=0)&&(suspendTimer<=120))
    	 {
    	   this.suspendTimer = suspendTimer;
           isSuspendTimer=true;
         }
         else
         {  
           throw new IllegalArgumentException("ParameterOutOfRange");
        }
    }


/**
Indicates if the Suspend Timer optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean  isSuspendTimerPresent ()
    {
        return isSuspendTimer;
    }


//----------------------------------------------------


/**
Gets Connected Number Treatment Indicator
*/

    public int getConnectedNumberTreatmentInd() throws ParameterNotSetException
    {
        if(isConnectedNumberTreatmentIndPresent ()){
        return connectedNumberTreatmentInd;
        }else{
               throw new ParameterNotSetException();
              } 
    }

/**
Sets Connected Number Treatment Indicator
*/

    public void setConnectedNumberTreatmentInd(int connectedNumberTreatmentInd)
    {
        this.connectedNumberTreatmentInd = connectedNumberTreatmentInd;
        isConnectedNumberTreatmentInd=true;
    }

/**
Indicates if the Connected Number Treatment Indicator optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean  isConnectedNumberTreatmentIndPresent ()
    {
        return isConnectedNumberTreatmentInd;
    }



//----------------------------------------------------

/**
Gets Suppress Call Diversion Notification
*/

    public boolean getSuppressCallDiversionNotification() throws ParameterNotSetException
    {
        if(isSuppressCallDiversionNotificationPresent ()){
        return suppressCallDiversionNotification;
        }else{
               throw new ParameterNotSetException();
              } 
    }

/**
Sets Suppress Call Diversion Notification
*/

    public void setSuppressCallDiversionNotification(boolean suppressCallDiversionNotification)
    {
        this.suppressCallDiversionNotification = suppressCallDiversionNotification;
        isSuppressCallDiversionNotification=true;
    }


/**
Indicates if the Suppress Call Diversion Notification optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean  isSuppressCallDiversionNotificationPresent ()
    {
        return isSuppressCallDiversionNotification;
    }


//----------------------------------------------------

/**
Gets Suppress Call Transfer Notification
*/

    public boolean getSuppressCallTransferNotification() throws ParameterNotSetException
    {
        if(isSuppressCallTransferNotificationPresent ()){
        return suppressCallTransferNotification;
        }else{
               throw new ParameterNotSetException();
              } 
    }

/**
Sets Suppress Call Transfer Notification
*/

    public void setSuppressCallTransferNotification(boolean suppressCallTransferNotification)
    {
        this.suppressCallTransferNotification = suppressCallTransferNotification;
        isSuppressCallTransferNotification=true;
    }


/**
Indicates if the Suppress Call Transfer Notification optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean  isSuppressCallTransferNotificationPresent ()
    {
        return isSuppressCallTransferNotification;
    }


//----------------------------------------------------


/**
Gets Allowed Called IN Number Presentation Indicator
*/

    public boolean getAllowCdINNoPresentationInd() throws ParameterNotSetException
    {
        if(isAllowCdINNoPresentationIndPresent ()){
        return allowCdINNoPresentationInd;
        }else{
               throw new ParameterNotSetException();
              }  
    }

/**
Sets Allowed Called IN Number Presentation Indicator
*/

    public void setAllowCdINNoPresentationInd(boolean allowCdINNoPresentationInd)
    {
        this.allowCdINNoPresentationInd = allowCdINNoPresentationInd;
        isAllowCdINNoPresentationInd=true;
    }


/**
Indicates if the Allowed Called IN Number Presentation Indicator optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean  isAllowCdINNoPresentationIndPresent ()
    {
        return isAllowCdINNoPresentationInd;
    }


//----------------------------------------------------


/**
Gets User Dialogue Duration Indicator
*/

    public boolean getUserDialogueDurationInd()
    {
        return userDialogueDurationInd;
     }

/**
Sets User Dialogue Duration Indicator
*/

    public void setUserDialogueDurationInd(boolean userDialogueDurationInd)
    {
        this.userDialogueDurationInd = userDialogueDurationInd;
    }

//----------------------------------------------------

}
