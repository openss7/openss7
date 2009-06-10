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
 *  File Name     : MiscCallInfo.java
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
This Class defines the MiscCallInfo DataType
*/

public class MiscCallInfo implements java.io.Serializable 
{

    private MessageType messageType;

    private DPAssignment dPAssignment;    
    private boolean isDpAssignment = false ;

/**
Constructor For MiscCallInfo
*/
	public MiscCallInfo(MessageType messageType) 
	{
		setMessageType (messageType);
	}
	
//---------------------------------------------	

/**<P>
Gets Message Type
<DT> This parameter gets the category of instruction issued to the SCF
<LI>REQUEST
<LI>NOTIFICATION	
<P>
*/

    public MessageType getMessageType ()
    {
        return messageType;
    }

/**
Sets Message Type
*/

    public void setMessageType (MessageType messageType) 
       {
       		this.messageType = messageType;

    }

//---------------------------------------------	

/**<P>
Gets Detection Point Assignment.
<DT> Request for instructions has been issued to the SCF.This gets the category for the trigger (line, group, office).
<LI>INDIVIDUAL_LINE
<LI>GROUP_BASED	
<LI>OFFICE_BASED
<P>
*/

    public DPAssignment getDpAssignment () throws ParameterNotSetException
    {
        if(isDpAssignmentPresent ())
		{
        	return dPAssignment;
        }
		else
		{
            throw new ParameterNotSetException();
        }
    }

/**
Sets Detection Point Assignment
*/

    public void setDpAssignment (DPAssignment dpAssignment)
            {
            	this.dPAssignment = dPAssignment;
				isDpAssignment = true;

    }
/**
Indicates if the DP Assignment optional parameter is present .
Returns: TRUE if present , FALSE otherwise.
*/
    public boolean  isDpAssignmentPresent ()
    {
        return isDpAssignment;
    }


//---------------------------------------------	

}

