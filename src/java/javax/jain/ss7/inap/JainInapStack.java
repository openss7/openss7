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
 *  File Name     : JainInapStack.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */


package javax.jain.ss7.inap;

import java.io.*;
import java.util.*;
import java.lang.*;
import javax.jain.ss7.*;
import javax.jain.*;

/**
 <P>
This interface defines the methods required to represent a proprietary
 JAIN INAP protocol stack, the implementation of which will be vendor specific. 
 Each vendor's protocol stack will have an object
 that implements this interface to control the creation/deletion of proprietary
 <CODE>JainInapProvider</CODE> <P>
 It must be noted that any object that implements the:
<UL>
<LI>JainInapProvider Interface is referred to as JainInapProviderImpl.
<LI>JainInapStack Interface is referred to as JainInapStackImpl.
</UL>
 An application may create a JainInapStackImpl by invoking the 
 <a href="JainFactory.html#createSS7Object()">JainFactory.createSS7Object()</a>
 method on the JainSS7Factory object. The <b>PathName</b> of the vendor specific implementation of which you want to 
 instantiate needs to be set on the JainSS7Factory object using the <a href="JainFactory.html#setPathName()">JainFactory.setPathName()</a>before calling the createSS7Object method.
<P>
*/

public interface JainInapStack 
{
 
 
/**
<DL>
<DD>Creates a new Peer (vendor specific) instance of the class JainInapProviderImpl that
<DD>is attached to this instance of JainInapStackImpl.
<DL>
*/
 
public JainInapProvider createProvider()throws PeerUnavailableException;
 
/**
<DL>
<DD>Deletes the specified JainInapProviderImpl attached to this JainInapStackImpl.
 <P><DD><DL>
<DT><B>Parameters:</B><DD><CODE><var>providerToBeDeleted</var></CODE> - the JainInapProviderImpl to be deleted from this JainInapStackImpl.<DT></DL>
</DD>
</DL>
*/
public void deleteProvider(JainInapProvider providerToBeDeleted) throws DeleteProviderException;
  

/**
<DL>
<DD>Returns the vector of JainInapProviderImpls that have been created by this JainInapStackImpl. All of the JainInapProviderImpls of this
    JainInapStackImpl will be proprietary objects belonging to the same stack vendor. Note that the JainInapProviderImpls in this vector may
     be either attached or detached.<DD><DL>
<DT><B>Returns:</B><DD>a Vector containing all the of JainInapProviderImpls created by this JainInapStackImpl.<DT></DL>
</DD>
</DL>
*/


  public java.util.Vector getProviderList();
  
  /**
  * Returns the name of the stack as a string
  *
  * @return  a string describing the Stack Name
  */

    public String getStackName();

	/**
    * Sets the name of the stack as a string
    *
    * @param  stackName  The new Stack Name value
    */
    public void setStackName(String stackName);

    /**
     * Returns the Vendor's name for this stack
     *
     *  @return  a string describing the Vendor's name
     */
    public String getVendorName();

    /**
     * Sets the Vendors name for this stack, this name may be the Vendor's domain
     * name i.e. "com.sun" 
     */
     public void setVendorName(String vendorName);

    /**
     * Returns the Signalling Point Code of the JainInapStcak
     *
	  * @return    the signalingPointCode of this JainInapStack.
     */
     public SignalingPointCode getSignalingPointCode();

    /**
     * Returns the Sub System Number of the JainInapStack
     *
	  * @return    the Sub System Number of this JainInapStack.
     */
     public int getSubSystemNumber();

}  

