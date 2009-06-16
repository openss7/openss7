/*
#pragma ident "%W%      %E%     SMI"

 * Copyright (c) 1996 Sun Microsystems, Inc. All Rights Reserved.
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
 * Automated Call Distribution (ACD) is a Call Center feature that provides a 
 * mechanism for receiving calls, queueing them, and distributing them to
 * agent extensions within ACD Groups.  An ACD Group comprises zero or more
 * agent extensions, which are dynamically associated with ACD Groups through
 * a login/logout process.  The <CODE>ACDAddress</CODE> interface models an 
 * ACD Group for ACD systems.
 * <p>
 * A call placed to an <CODE>ACDAddress</CODE> represents a call which is 
 * being routed to an available agent logged into the ACD Group. In the
 * case where no agent is available, the <CODE>ACDAddress</CODE> is queued 
 * for a group of agents who are logged in to that ACD Group, but unavailable
 * to service that call.  Calls are distributed to the agents in that group 
 * based on their availability and other factors determined by the implementation.
 * <p>
 * Calls may be placed directly to an <CODE>ACDAddress</CODE> or
 * the ACD mechanism can be relied upon to select a destination 
 * <CODE>ACDAddress</CODE> by placing the call to an 
 * <CODE>ACDManagerAddress</CODE>.
 * <p>
 * The <CODE>ACDAddress</CODE> extends <CODE>CallCenterAddress</CODE> with the 
 * methods necessary to obtain ACD-specific information such as the Agent 
 * objects associated with the <CODE>ACDAddress</CODE> and a variety of call 
 * queue methods to get information on calls queued at this address.
 * 
 *<H4> ACDAddresses differ from Addresses </H4>
 *
 * Some important differences between <CODE>ACDAddress</CODE> and 
 * <CDOE>Address</CODE> are: 
 * <OL>
 *<LI>An <CODE>ACDAddress</CODE> cannot have <CODE>Terminal</CODE> objects 
 * associated with it.
 * <LI>An <CODE>ACDAddress</CODE> is not a logical endpoint of a call in the 
 * same sense as an <CODE>Address</CODE> Rather, it models a queuing process 
 * whereby the selection of a logical endpoint is deferred.
 * <LI><CODE>ACDConnections</CODE> to an <CODE>ACDAddress</CODE> do not enter into
 * a CONNECTED state.
 * <LI>It is not returned on  <CODE>Provider.getAddresses()</CODE>, but is 
 * available through <CODE>CallCenterProvider.getACDAddresses()</CODE>
 * </OL>
 *
 * <H4>ACDAddresses and ACDConnections</H4>
 *
 * A call presented to an <CODE>ACDAddress</CODE> is modeled by an 
 * <CODE>ACDConnection</CODE>.  That <CODE>ACDConnection</CODE> may be
 * between a <CODE>Call</CODE> object and the <CODE>ACDAddress</CODE> or
 * it may exist between a <CODE>ACDManagerConnection</CODE> and the 
 * <CODE>ACDAddress</CODE>, depending on whether the call was placed directly
 * to the <CODE>ACDAddress</CODE> or whether it arrived indirectly through
 * the distribution mechanism from an <CODE>ACDManagerAddress</CODE>.
 * 
 * <H4> Events at an ACDAddress </H4>
 *
 * With JTAPI 1.4, all events pertaining to the <CODE>ACDAddress</CODE> interface are
 * reported to <CODE>AddressListener</CODE> methods.
 * 
 * In order to observe <CODE>Agent</CODE> state changes for <CODE>Agents</CODE> associated with an
 * <CODE>ACDAddress</CODE>, an application must implement an <CODE>ACDAddressListener</CODE>
 * interface and associate it with the <CODE>ACDAddress</CODE>, using the <CODE>addListener</CODE>
 * method on an <CODE>ACDAddress</CODE> object.
 * 
 * JTAPI 1.2 applications may continue to use the deprecated
 * <CODE>ACDAddressObserver</CODE> interface with its
 * <CODE>AddressObserver.addressChangedEvent</CODE> method.
 * <p>
 * @see javax.telephony.callcenter.ACDConnection
 * @see javax.telephony.callcenter.CallCenterAddress
 * @see javax.telephony.callcenter.ACDAddressObserver
 * @see javax.telephony.Address
 */
public interface ACDAddress extends CallCenterAddress {

  /**
   * Returns the Agents logged into the ACDAddress.
   * <p>
   * @return An array of Agents associated with the ACDAddress.
   * @exception MethodNotSupportedException This method is not supported
   * by the implementation.
   */
  public Agent[] getLoggedOnAgents()
    throws MethodNotSupportedException;


  /**
   * Returns the number of Calls queued at an ACDAddress.
   * <p>
   * @return The number of calls queued.
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   */
  public int getNumberQueued()
    throws MethodNotSupportedException;


  /**
   * Returns the oldest Call queued to an ACDAddress.
   * <p>
   * @return The oldest Call queued.
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   */
  public Call getOldestCallQueued()
    throws MethodNotSupportedException;


  /**
   * Returns the relative load of an ACDAddress queue.
   * <p>
   * @return The relative load of the ACDAddress queue.
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   */
  public int getRelativeQueueLoad()
    throws MethodNotSupportedException;


  /**
   * Returns the estimated wait time for new Calls
   * queued at an ACDAddress.
   * <p>
   * @return The estimated wait time for new calls at the ACDAddress.
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   */
  public int getQueueWaitTime()
    throws MethodNotSupportedException;


  /**
   * Returns the <CODE>ACDManagerAddess</CODE> associated at system 
   * administration time with this <CODE>ACDAddress</CODE>.
   * This method returns a null if no <CODEACDManagerAddress</CODE> is 
   * associated with this <CODE>ACDAddress</CODE>.
   * <p>
   * It does not return the <CODE>ACDManagerAddress</CODE> connected to this
   * <CODE>ACDAddress</CODE> in a <CODE>Call</CODE>. That information can be 
   * obtained through the <CODE>getACDManagerConnection()</CODE> method on 
   * <CODE>ACDConnection</CODE>.
   * <p>
   * @return The ACDManagerAddress associated with this ACDAddress.
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   */
  public ACDManagerAddress getACDManagerAddress()
    throws MethodNotSupportedException;
}
