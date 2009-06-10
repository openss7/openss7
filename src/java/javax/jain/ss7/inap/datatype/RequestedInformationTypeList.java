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
 *  File Name     : RequestedInformationTypeList.java
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
This Class defines the RequestedInformationTypeList Datatype
*/


public class RequestedInformationTypeList implements java.io.Serializable {

	 private int requestedInformationType[];

/**
Constructor For RequestedInformationTypeList
*/
	public RequestedInformationTypeList(int requestedInformationType[])
	{
		setRequestedInformationType(requestedInformationType);
	}

//--------------------------------------

/**
  Gets Requested Information Type 
*/

    public int[] getRequestedInformationType()
    {
        return requestedInformationType;
    }

/**
Gets a particular Requested Information Type 
*/

    public int getRequestedInformationType(int index)
    {
        return requestedInformationType[index];
    }

/**
  Sets Requested Information Type 
*/
    public void setRequestedInformationType(int requestedInformationType[])
    {
        this.requestedInformationType = requestedInformationType;
    }

/**
Sets a particular Requested Information Type 
*/

    public void  setRequestedInformationType(int index , int requestedInformationType)
    {
        this.requestedInformationType[index] = requestedInformationType;
    }    
    
//----------------------- 

}

