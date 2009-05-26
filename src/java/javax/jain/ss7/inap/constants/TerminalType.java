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
 *  File Name     : TerminalType.java
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
This class identifies the terminal type so that the SCF can specify, to the SRF,
the appropriate type of capability (voice recognition, DTMF, display capability etc.)
*/

public class TerminalType   {

	// internal constant available for switch statements 

    public static final int M_UNKNOWN=0;
    public static final int M_DIAL_PULSE=1;
    public static final int M_DTMF=2;
    public static final int M_ISDN=3;
    public static final int M_ISDN_NO_DTMF=4;
    public static final int M_SPARE=16;
	
	// internal variable to store the constant value 
	
	private int terminalType ;
	
	    
/**
Constructor for TerminalType datatype
*/
    
    private TerminalType(int terminalType)
	{
    
		this.terminalType=terminalType;
		
	}
	
    
	/**
	* TerminalType  :UNKNOWN
	*
	*/
	
	public static final TerminalType UNKNOWN = new TerminalType (M_UNKNOWN);
	

	/**
	* TerminalType  :DIAL_PULSE
	*
	*/
	
	public static final TerminalType  DIAL_PULSE= new TerminalType (M_DIAL_PULSE);
	
	
	
    	/**
    	* TerminalType  :DTMF
    	*
    	*/
    	
    	public static final TerminalType DTMF = new TerminalType (M_DTMF);

    	/**
    	* TerminalType  :ISDN
    	*
    	*/
    	
    	public static final TerminalType  ISDN= new TerminalType (M_ISDN);

    	/**
    	* TerminalType  :ISDN_NO_DTMF
    	*
    	*/
    	
    	public static final TerminalType ISDN_NO_DTMF = new TerminalType (M_ISDN_NO_DTMF);


        /**
    	* TerminalType  :SPARE
    	*
    	*/
    	
    	public static final TerminalType SPARE = new TerminalType (M_SPARE);




	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	public int getTerminalType ()
	{
			return terminalType ;
	}
} // end class 