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
 *  File Name     : ProfileIdentifier.java
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
This class defines the ProfileIdentifier Datatype
*/

public class ProfileIdentifier implements Serializable
{

    private CalledPartyNumber access;


    private FacilityGroup group;


	private ProfileIdentifierChoice profileIdentifierChoice;


/**
Constructor For ProfileIdentifier
*/
	public ProfileIdentifier(CalledPartyNumber access)
	{
		setAccess(access);
	}

	public ProfileIdentifier(FacilityGroup group)
	{
		setGroup(group);
	}

//-------------------------------
/**
Gets Profile Identifier Choice
*/

	public ProfileIdentifierChoice getProfileIdentifierChoice()
		{ 
			return profileIdentifierChoice;
		}

/**
Gets Access
*/
    public CalledPartyNumber getAccess() throws ParameterNotSetException
    {
        if(profileIdentifierChoice.getProfileIdentifierChoice()==ProfileIdentifierChoice.ACCESS.getProfileIdentifierChoice())
		{
        	return access;
        }
		else
		{
            throw new ParameterNotSetException();
        }
    }
    
/**
Sets Access
*/
    public void setAccess(CalledPartyNumber access)
    {
        this.access = access;
        profileIdentifierChoice=ProfileIdentifierChoice.ACCESS;
    }

//----------------------------------------------

/**
Gets Group
*/
    public FacilityGroup getGroup() throws ParameterNotSetException
    {
       if(profileIdentifierChoice.getProfileIdentifierChoice()==ProfileIdentifierChoice.GROUP.getProfileIdentifierChoice())
	   { 
       		return group;
       }
	   else
	   {
            throw new ParameterNotSetException();
       }
    }
    
/**
Sets Group
*/
    public void setGroup(FacilityGroup group)
    {
        this.group = group;
        profileIdentifierChoice=ProfileIdentifierChoice.GROUP;
    }

//----------------------------------------------

}
