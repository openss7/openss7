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
 *  File Name     : InvokeProblem.java
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
This class indicates the Invoke Problem.*/

public class InvokeProblem extends Hashtable {

    public static final int DUPLICATE_INVOKE_ID=0;
    public static final int UNRECOGNIZED_OPERATION=1;
    public static final int MISTYPED_PARAMETER=2;
    public static final int RESOURCE_LIMITATION=3;
	 public static final int INITIATING_RELEASE=4;
    public static final int UNRECOGNIZED_LINKED_ID=5;
    public static final int LINKED_RESPONSE_UNEXPECTED=6;
    public static final int UNEXPECTED_LINKED_OPERATION=7;


/**
Constructor for InvokeProblem datatype
*/
    
    public InvokeProblem() {
    	clear();
    	put("DUPLICATE_INVOKE_ID", new Integer(0));
    	put("UNRECOGNIZED_OPERATION", new Integer(1));
    	put("MISTYPED_PARAMETER", new Integer(2));
	   put("RESOURCE_LIMITATION", new Integer(3));
    	put("INITIATING_RELEASE", new Integer(4));
    	put("UNRECOGNIZED_LINKED_ID", new Integer(5));
      put("LINKED_RESPONSE_UNEXPECTED", new Integer(6));
    	put("UNEXPECTED_LINKED_OPERATION", new Integer(7));
    
    }
    
    
}
