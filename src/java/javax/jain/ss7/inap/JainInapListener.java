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
 *  File Name     : JainInapListener.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */


package javax.jain.ss7.inap;

import java.io.*;
import java.util.*;
import java.lang.*;
import javax.jain.ss7.inap.event.*;


/**
 <P>
This interface defines the methods required by all INAP API Users to receive and process JAIN INAP Events that
 are emitted by an object implementing the 
 <a href="JainInapProvider.html">JainInapProvider</a> interface.
 It must be noted that any object that implements the:
<UL>
<LI>JainInapListener Interface is referred to as JainInapListenerImpl.
<LI>JainInapProvider Interface is referred to as JainInapProviderImpl.
</UL>
<p>An INAP API User will receive Events once it has registered as an EventListener of a JainInapProviderImpl. The User application will
 register with the JainInapProviderImpl with an Application Context by invoking the ProviderImpl's
 <a href="JainInapProvider.html#addJainInapListener(javax.jain.ss7.inap.JainInapProvider)">addJainInapListener() method</a>.
 Any Events addressed to a User Application will be passed to it using the User Address and the Application Context Name.
 <p>The User application will use the JainInapProviderImpl to send INAP 
 messages.
<P>
*/

public interface JainInapListener extends java.util.EventListener
{

/** This method processes an INAP Indication Event emitted by the JainInapProvider.*/

 	public void  processInapIndEvent(DialogueIndEvent event);

/** This method processes an Call ID Time Out Event emitted by the JainInapProvider.*/

 	public void  processCallIDTimeOutEvent(int callID);

}  //interface

