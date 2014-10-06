/*
#pragma ident "@(#)ACDAddrEv.java	1.5      96/11/03     SMI"

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

package javax.telephony.callcenter.events;
import  javax.telephony.Address;
import  javax.telephony.events.AddrEv;
import  javax.telephony.callcenter.*;

/**
 * The <CODE>ACDAddrEv</CODE> interfaces is the base event for all events
 * pertaining to the <CODE>ACDAddress</CODE> interface. This interface extends
 * the <CODE>CallCentEv</CODE> interface and the core <CODE>AddrEv</CODE>
 * interface. All event interfaces which extend this interface are reported via
 * the <CODE>ACDAddressObserver</CODE> interface.
 * <p>
 * The call center package defines events interfaces which extend this
 * interface to report state changes in <CODE>Agent's</CODE> which are
 * associated with the <CODE>ACDAddress</CODE>. These events are:
 * <CODE>ACDAddrBusyEv</CODE>, <CODE>ACDAddrLoggedOffEv</CODE>,
 * <CODE>ACDAddrLoggedOnEv</CODE>, <CODE>ACDAddrNotReadyEv</CODE>,
 * <CODE>ACDAddrUnknownEv</CODE>, <CODE>ACDAddrWorkNotReadyEv</CODE>,
 * and <CODE>ACDAddrWorkReadyEv</CODE>.
 * <p>
 * @see javax.telephony.Terminal
 * @see javax.telephony.events.TermEv
 * @see javax.telephony.callcenter.Agent
 * @see javax.telephony.callcenter.ACDAddress
 * @see javax.telephony.callcenter.events.CallCentEv
 * @see javax.telephony.callcenter.events.ACDAddrBusyEv
 * @see javax.telephony.callcenter.events.ACDAddrLoggedOnEv
 * @see javax.telephony.callcenter.events.ACDAddrLoggedOffEv
 * @see javax.telephony.callcenter.events.ACDAddrNotReadyEv
 * @see javax.telephony.callcenter.events.ACDAddrReadyEv
 * @see javax.telephony.callcenter.events.ACDAddrUnknownEv
 * @see javax.telephony.callcenter.events.ACDAddrWorkNotReadyEv
 * @see javax.telephony.callcenter.events.ACDAddrWorkReadyEv
 * @deprecated as of JTAPI 1.4, replaced by {@link javax.telephony.callcenter.ACDAddressEvent}
 */
public interface ACDAddrEv extends CallCentEv, AddrEv {

  /**
   * Returns the <CODE>Agent</CODE> associated with this event.
   * <p>
   * @since JTAPI v1.2
   * @return The associated Agent.
   */
  public Agent getAgent();


  /**
   * Returns the Terminal associated with the <CODE>Agent</CODE>.
   * <p>
   * <STRONG>Note:</STRONG> This method has been replaced in JTAPI v1.2 and
   * later. Applications should use the <CODE>ACDAddrEv.getAgent()</CODE>
   * method to obtain the <CODE>Agent</CODE> and then use the
   * <CODE>Agent.getAgentTerminal()</CODE> method.
   * <p>
   * @deprecated JTAPI v1.2. This method has been replaced by the getAgent()
   * method.
   * @return The Terminal associated with the Agent.
   */
  public AgentTerminal getAgentTerminal();


 /**
   * Returns the state of the <CODE>Agent</CODE>.
   * <p>
   * <STRONG>Note:</STRONG> This method has been replaced in JTAPI v1.2 and
   * later. Applications should use the <CODE>ACDAddrEv.getAgent()</CODE>
   * method to obtain the <CODE>Agent</CODE> and then use the
   * <CODE>Agent.getState()</CODE> method.
   * <p>
   * @deprecated JTAPI v1.2 This method has been replaced by the getAgent()
   * method.
   * @return The Agent's state.
   */
  public int getState();


  /**
   * Returns the Address associated with the <CODE>Agent's</CODE> Terminal.
   * <p>
   * <STRONG>Note:</STRONG> This method has been replaced in JTAPI v1.2 and
   * later. Applications should use the <CODE>ACDAddrEv.getAgent()</CODE>
   * method to obtain the <CODE>Agent</CODE> and then use the
   * <CODE>Agent.getAgentAddress()</CODE> method.
   * <p>
   * @deprecated JTAPI v1.2 This method has been replaced by the getAgent()
   * method.
   * @return The Address associated with the Agent's Terminal.
   */
  public Address getAgentAddress();


  /**
   * Returns an array of all Trunks currently being used for this Call.
   * If there are no Trunks being used, this method returns null.
   * <p>
   * <STRONG>Note:</STRONG> This method has been replaced in JTAPI v1.2 and
   * later. Applications should use the <CODE>ACDAddrEv.getAgent()</CODE>
   * method to obtain the <CODE>Agent</CODE> and then get the trunks via the
   * <CODE>CallCenterCall</CODE> interface.
   * <p>
   * @deprecated JTAPI v1.2. This method has been replaced by the getAgent()
   * method.
   * @return An array of Trunks, null if there are none.
   */
  public CallCenterTrunk[] getTrunks();
}
