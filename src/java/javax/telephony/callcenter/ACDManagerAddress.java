/*
#pragma ident "@(#)ACDManagerAddress.java	1.8      99/02/23     SMI"

 * Copyright (c) 1996,1997 Sun Microsystems, Inc. All Rights Reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for NON-COMMERCIAL purposes and without
 * fee is hereby granted provided that this copyright notice
 * appears in all copies. Please refer to the file "copyright.html"
 * for further important copyright and licensing information.
 *
 * SUN MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF
 * THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, OR NON-INFRINGEMENT. SUN SHALL NOT BE LIABLE FOR
 * ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR
 * DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.
 */

package javax.telephony.callcenter;
import  javax.telephony.*;

/**
 * <H4>Introduction</H4>
 *
 * The <CODE>ACDManagerAddress</CODE> interface models an ACD management 
 * control point that manages one or more Agent Groups. Calls 
 * are presented to an <CODE>ACDManagerAddress</CODE> so that it can
 * distribute those calls to agents logged into the Agent Groups managed by 
 * this special address.
 * 
 * <H4>ACDManagerAddresses and other ACD Objects </H4>
 *
 * When a call is placed to an <CODE>ACDManagerAddress</CODE>, its implementation 
 * routes that call to one or more Agent Groups, which are modeled by 
 * <CODE>ACDAddresses</CODE>. When an <CODE>ACDAddress</CODE> receives the call
 * it may present that call to an available Agent in its Agent Group, or
 * it may queue the call until an Agent is available. 
 * <p>
 * The ultimate destination of a call is a human at a phone terminal.  This person 
 * may have "sessions" active at different Agent Groups, each of which presents 
 * her or him as an Agent in that Agent Group. An Agent is the recipient of a
 * call in an Agent Group and Agents are modeled as <CODE>Agent</CODE> objects. 
 * When an Agent is selected, the call is completed by a <CODE>Connection</CODE>
 * between the <CODE>Call</CODE> object that originally involved the
 * <CODE>ACDManagerAddress</CODE> and the <CODE>Address</CODE> associated with 
 * that <CODE>Agent</CODE>. The <CODE>ACDManagerConnection</CODE> and 
 * <CODE>ACDConnections</CODE> that modeled ACD call routing and queuing to that
 *  point are then placed in a DISCONNECTED state. 
 * 
 * <H4>ACDManagerAddresses differ from Addresses</H4>
 *
 * Some important differences between <CODE>ACDManagerAddress</CODE> and 
 * <CODE>Address</CODE> are: 
 * <OL>
 * <LI>An <CODE>ACDManagerAddress</CODE> cannot have any <CODE>Terminal</CODE> 
 * objects associated with it.
 * <LI>An <CODE>ACDManagerAddress</CODE> is not a logical endpoint of a call in 
 * the same sense as an <CODE>Address</CODE>, rather it models a distribution 
 * process whereby the selection of a logical endpoint is deferred.
 * <LI><CODE>ACDManagerConnections</CODE> associated with an 
 * <CODE>ACDAddress</CODE> do not enter into the CONNECTED state.
 * <LI>An <CODE>ACDManagerAddress</CODE> is not returned on 
 * <CODE>Provider.getAddresses()</CODE>, but is available through 
 * <CODE>CallCenterProvider.getACDManagerAddresses()</CODE>
 * </OL>
 * <p>
 * @see javax.telephony.callcenter.ACDManagerConnection
 * @see javax.telephony.callcenter.ACDAddress
 * @see javax.telephony.callcenter.ACDManagerConnection
 */
public interface ACDManagerAddress extends CallCenterAddress {

  /**
   * Returns the <CODE>ACDAddess(es)</CODE> associated at system administration
   * time  with this <CODE>ACDManagerAddress</CODE>.
   * This method returns a null if no <CODE>ACDAddress</CODE> is associated with 
   * this <CODE>ACDManagerAddress</CODE>.
   * <p>
   * It does not return the <CODE>ACDAddress(es)</CODE> connected 
   * to this <CODE>ACDManagerAddress</CODE> in a <CODE>Call</CODE>. That 
   * information can be obtained through
   * the <CODE>getACDConnection()</CODE> method on 
   * <CODE>ACDManagerConnection</CODE>.
   * <p>
   * @return The ACDAddresses associated with this ACDManagerAddress.
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   */
  public ACDAddress[] getACDAddresses()
    throws MethodNotSupportedException;
}
