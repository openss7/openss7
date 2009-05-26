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
 *  File Name     : RedirectionInformation.java
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

/**
This Class defines the RedirectionInformation datatype (Encoded as in Q.763)
*/

public class RedirectionInformation implements Serializable {


    private int redirectingIndicator;
    private int originalRedirectionReason;
    private int redirectionCounter;
    private int redirectingReason;

/**
Constructor For RedirectionInformation
*/
	public RedirectionInformation(int redirectingIndicator, 
								int originalRedirectionReason, 
								int redirectionCounter, 
								int redirectingReason)
	{
		setRedirectingIndicator (redirectingIndicator);
		setOriginalRedirectionReason (originalRedirectionReason);
		setRedirectionCounter (redirectionCounter);
		setRedirectingReason (redirectingReason);
	}
	
//---------------------------------------------	

/**
Gets  Redirecting Indicator
*/

    public int getRedirectingIndicator()
    {
        return redirectingIndicator ;
    }

/**
Sets  Redirecting Indicator
*/

    public void  setRedirectingIndicator (int redirectingIndicator)
    {
        this.redirectingIndicator = redirectingIndicator ;
    }

//---------------------------------------------	

/**
Gets  Original Redirection Reason
*/

    public int getOriginalRedirectionReason()
    {
        return originalRedirectionReason ;
    }

/**
Sets  Original Redirection Reason
*/

    public void  setOriginalRedirectionReason (int originalRedirectionReason)
    {
        this.originalRedirectionReason = originalRedirectionReason ;
    }

//---------------------------------------------	

/**
Gets  Redirection Counter
*/

    public int getRedirectionCounter()
    {
        return redirectionCounter ;
    }

/**
Sets  Redirection Counter
*/

    public void  setRedirectionCounter (int redirectionCounter)
    {
       this.redirectionCounter = redirectionCounter ;
    }

//---------------------------------------------	

/**
Gets  Redirecting Reason
*/

    public int getRedirectingReason()
    {
        return redirectingReason ;
    }

/**
Sets  Redirecting Reason
*/

    public void  setRedirectingReason (int redirectingReason)
    {
        this.redirectingReason = redirectingReason;
    }

//---------------------------------------------	

}
