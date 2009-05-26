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
 *  File Name     : TCNotice.java
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
This class defines the ReportCause Class . (Encoded as in Q.713)
*/

public class ReportCause  {


	// Internal Constant available for switch statements
	
    public static final int M_NO_TRANSLATION_FOR_AN_ADDRESS_OF_SUCH_NATURE=0;
    public static final int M_NO_TRANSLATION_FOR_THIS_SPECIFIC_ADDRESS=1;
    public static final int M_SUBSYSTEM_CONGESTION=2;
    public static final int M_SUBSYSTEM_FAILURE=3;
    public static final int M_UNEQUIPPED_USER=4;
    public static final int M_MTP_FAILURE=5;
    public static final int M_NETWORK_CONGESTION=6;
    public static final int M_SCCP_UNQUALIFIED=7;
    public static final int M_ERROR_IN_MESSAGE_TRANSPORT=8;
    public static final int M_ERROR_IN_LOCAL_PROCESSING=9;
    public static final int M_DESTINATION_CAN_NOT_PERFORM_REASSEMBLY=10;
    public static final int M_SCCP_FAILURE=11;
    public static final int M_HOPCOUNTER_VIOLATION=12;
    public static final int M_SEGMENTATION_NOT_SUPPORTED=13;
    public static final int M_SEGMENTATION_FAILED=14;
    
	
	// internal variable to store the constant value 
	
	private int reportCause;
	
	
/**
Constructor for TCNotice datatype
*/
    
    private ReportCause(int reportCause) 
	{
	
		this.reportCause =reportCause ;
	
	}
	
	/**
	* TCNotice :NO_TRANSLATION_FOR_AN_ADDRESS_OF_SUCH_NATURE
	*
	*/
	
	public static final ReportCause NO_TRANSLATION_FOR_AN_ADDRESS_OF_SUCH_NATURE = new ReportCause (M_NO_TRANSLATION_FOR_AN_ADDRESS_OF_SUCH_NATURE) ;
	
	
	
	/**
	* TCNotice :NO_TRANSLATION_FOR_THIS_SPECIFIC_ADDRESS
	*
	*/
	
	public static final ReportCause NO_TRANSLATION_FOR_THIS_SPECIFIC_ADDRESS = new ReportCause (M_NO_TRANSLATION_FOR_THIS_SPECIFIC_ADDRESS) ;
	
	
	/**
	* TCNotice :SUBSYSTEM_CONGESTION
	*
	*/
	
	public static final ReportCause SUBSYSTEM_CONGESTION = new ReportCause (M_SUBSYSTEM_CONGESTION) ;
	
	
	/**
	* TCNotice :SUBSYSTEM_FAILURE
	*
	*/
	
	public static final ReportCause SUBSYSTEM_FAILURE = new ReportCause (M_SUBSYSTEM_FAILURE) ;
	


	/**
	* TCNotice :UNEQUIPPED_USER
	*
	*/
	
	public static final ReportCause UNEQUIPPED_USER = new ReportCause (M_UNEQUIPPED_USER) ;
	
	
	/**
	* TCNotice :MTP_FAILURE
	*
	*/
	
	public static final ReportCause  MTP_FAILURE= new ReportCause (M_MTP_FAILURE) ;
	
	
	/**
	* TCNotice :NETWORK_CONGESTION
	*
	*/
	
	public static final ReportCause NETWORK_CONGESTION = new ReportCause (M_NETWORK_CONGESTION) ;
	
	
	/**
	* TCNotice :SCCP_UNQUALIFIED
	*
	*/
	

	public static final ReportCause SCCP_UNQUALIFIED = new ReportCause (M_SCCP_UNQUALIFIED) ;
	
	
	/**
	* TCNotice :ERROR_IN_MESSAGE_TRANSPORT
	*
	*/
	
	public static final ReportCause ERROR_IN_MESSAGE_TRANSPORT = new ReportCause (M_ERROR_IN_MESSAGE_TRANSPORT) ;
	
	
	/**
	* TCNotice :ERROR_IN_LOCAL_PROCESSING
	*
	*/
	
	public static final ReportCause ERROR_IN_LOCAL_PROCESSING = new ReportCause (M_ERROR_IN_LOCAL_PROCESSING) ;
	
	
	/**
	* TCNotice :DESTINATION_CAN_NOT_PERFORM_REASSEMBLY
	*
	*/
	
	public static final ReportCause DESTINATION_CAN_NOT_PERFORM_REASSEMBLY = new ReportCause (M_DESTINATION_CAN_NOT_PERFORM_REASSEMBLY) ;
	
	
	/**
	* TCNotice :SCCP_FAILURE
	*
	*/
	
	public static final ReportCause SCCP_FAILURE = new ReportCause (M_SCCP_FAILURE) ;
	
	
	/**
	* TCNotice :HOPCOUNTER_VIOLATION
	*
	*/
	
	public static final ReportCause HOPCOUNTER_VIOLATION = new ReportCause (M_HOPCOUNTER_VIOLATION) ;
	
	
	/**
	* TCNotice :SEGMENTATION_NOT_SUPPORTED
	*
	*/
	
	public static final ReportCause SEGMENTATION_NOT_SUPPORTED = new ReportCause (M_SEGMENTATION_NOT_SUPPORTED) ;
	
	
	/**
	* TCNotice :SEGMENTATION_FAILED
	*
	*/
	
	public static final ReportCause  SEGMENTATION_FAILED= new ReportCause (M_SEGMENTATION_FAILED) ;
	
	
	
	
	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	public int getReportCause ()
	{
	
		return reportCause;
	
	}
	
	
    
}// end class 
