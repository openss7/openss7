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
 *  File Name     : ReturnErrorProblem.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.constants;


import java.io.*;
import java.util.*;

/**
This class indicates the Return Error Problem.*/

public class ReturnErrorProblem extends Hashtable {

    public static final int UNRECOGNIZED_INVOKE_ID=0;
    public static final int RETURN_ERROR_UNEXPECTED=1;
    public static final int UNRECOGNIZED_ERROR=2;
    public static final int UNEXPECTED_ERROR=3;
    public static final int MISTYPED_PARAMETER=4;
    

/**
Constructor for ReturnErrorProblem datatype
*/
    
    public ReturnErrorProblem() {
    	clear();
    	put("UNRECOGNIZED_INVOKE_ID", new Integer(0));
    	put("RETURN_ERROR_UNEXPECTED", new Integer(1));
	   put("UNRECOGNIZED_ERROR", new Integer(2));
    	put("UNEXPECTED_ERROR", new Integer(3));
    	put("MISTYPED_PARAMETER", new Integer(4));
	       
    }
    
    
}
