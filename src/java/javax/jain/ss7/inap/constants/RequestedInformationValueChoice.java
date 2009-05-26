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
 *  File Name     : RequestedInformationValueChoice.java
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
This class indicates the RequestedInformationValue choice
*/


public class RequestedInformationValueChoice {

	/*
	* internal constant available for switch statements
	*/
	
	public static final int M_CALL_ATTEMPT_ELAPSED_TIME_VALUE =0;
    public static final int M_CALL_STOP_TIME_VALUE =1;
    public static final int M_CALL_CONNECTED_ELAPSED_TIME_VALUE=2;
    public static final int M_CALLED_ADDRESS_VALUE =3;
	public static final int M_RELEASE_CAUSE_VALUE =30;

   // internal variable to store the constant value 
   
   
   private int requestedInformationValueChoice;
   
   
   /**
   * Constructor for RequestedInformationValueChoice  date Type 
   */
   
   private RequestedInformationValueChoice  (int requestedInformationValueChoice )
   {
   	
		this.requestedInformationValueChoice = requestedInformationValueChoice;
		
   }
   
  /**
  * RequestedInformationValueChoice  :CALL_ATTEMPT_ELAPSED_TIME_VALUE
  *
  */
  
  public static final RequestedInformationValueChoice CALL_ATTEMPT_ELAPSED_TIME_VALUE= new RequestedInformationValueChoice (M_CALL_ATTEMPT_ELAPSED_TIME_VALUE);
  
  
   /**
  * RequestedInformationValueChoice  :CALL_STOP_TIME_VALUE
  *
  */
  
  public static final RequestedInformationValueChoice CALL_STOP_TIME_VALUE= new RequestedInformationValueChoice (M_CALL_STOP_TIME_VALUE);
 
  /**
  * RequestedInformationValueChoice  :CALL_CONNECTED_ELAPSED_TIME_VALUE
  *
  */
  
  public static final RequestedInformationValueChoice CALL_CONNECTED_ELAPSED_TIME_VALUE= new RequestedInformationValueChoice (M_CALL_CONNECTED_ELAPSED_TIME_VALUE);
 
  /**
  * RequestedInformationValueChoice  :CALLED_ADDRESS_VALUE
  *
  */
  
  public static final RequestedInformationValueChoice CALLED_ADDRESS_VALUE= new RequestedInformationValueChoice (M_CALLED_ADDRESS_VALUE);
 
  /**
  * RequestedInformationValueChoice  :RELEASE_CAUSE_VALUE
  *
  */
  
  public static final RequestedInformationValueChoice RELEASE_CAUSE_VALUE= new RequestedInformationValueChoice (M_RELEASE_CAUSE_VALUE);
 
 
 
 	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	
	public int getRequestedInformationValueChoice ()
	{
	
		return requestedInformationValueChoice;
	
	}
	 
}// end class 