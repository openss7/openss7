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
 *  File Name     : TransportAddress.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.datatype;


import java.io.*;
import java.util.*;
import java.net.*;
import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.exception.*;
import javax.jain.ss7.*;
import javax.jain.*;

/**
This class represents the TransportAddress DataType
*/

public class TransportAddress implements java.io.Serializable{

	 private InetAddress ipAddress;
	
 	 private SccpUserAddress sccpUserAddress;

	 private TransportAddressChoice transportAddressChoice;
	 
/**
Constructor For TransportAddress
*/
	public TransportAddress(SccpUserAddress sccpUserAddress)
	{
		transportAddressChoice = TransportAddressChoice.SCCP_USER_ADDRESS;
		setSccpUserAddress(sccpUserAddress);
							
	}
	
/**
Constructor For TransportAddress
*/
	public TransportAddress(InetAddress ipAddress)
	{
		transportAddressChoice = TransportAddressChoice.IP_ADDRESS;
		setIPAddress(ipAddress);
							
	}

//---------------------------------------------	

/**
  Gets Address  Choice
 */
 
 	public TransportAddressChoice getTransportAddressChoice()
	 	{ 
			return transportAddressChoice;
		}


//---------------------------------------------	

/**
  Gets Sccp User Address
*/

    public SccpUserAddress getSccpUserAddress() throws ParameterNotSetException
    {
        if(transportAddressChoice.getTransportAddressChoice() == TransportAddressChoice.SCCP_USER_ADDRESS.getTransportAddressChoice())
		{
        	return sccpUserAddress;
        }
		else
		{
            throw new ParameterNotSetException();
        }
    }

/**
  Sets Sccp User Address

*/

    public void setSccpUserAddress( SccpUserAddress sccpUserAddress) 
    {  
        	this.sccpUserAddress = sccpUserAddress;
        	transportAddressChoice =TransportAddressChoice.SCCP_USER_ADDRESS;
    }
    
//-----------------------

/**
  Gets IP Address
*/

    public InetAddress getIPAddress() throws ParameterNotSetException
    {
        if(transportAddressChoice.getTransportAddressChoice() == TransportAddressChoice.IP_ADDRESS.getTransportAddressChoice())
		{
        	return ipAddress;
        }
		else
		{
            throw new ParameterNotSetException();
        }
    }

/**
  Sets IP Address

*/

    public void setIPAddress( InetAddress ipAddress) 
    {  
        	this.ipAddress = ipAddress;
        	transportAddressChoice =TransportAddressChoice.IP_ADDRESS;
    }
    
//-----------------------
    
}
