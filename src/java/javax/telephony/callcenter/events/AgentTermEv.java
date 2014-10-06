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

package javax.telephony.callcenter.events;
import  javax.telephony.Address;
import  javax.telephony.events.TermEv;
import  javax.telephony.callcenter.*;

/**
 * The <CODE>AgentTermEv</CODE> interfaces is the base event for all events
 * pertaining to the <CODE>AgentTerminal</CODE> interface. This interface
 * extends the <CODE>CallCentEv</CODE> interface and the core
 * <CODE>TermEv</CODE> interface. All event interfaces which extend this
 * interface are reported via the <CODE>AgentTerminalObserver</CODE> interface.
 * <p>
 * The call center package defines events interfaces which extend this
 * interface to report state changes in the <CODE>Agent</CODE>. These events
 * are: <CODE>AgentTermBusyEv</CODE>, <CODE>AgentTermLoggedOffEv</CODE>,
 * <CODE>AgentTermLoggedOnEv</CODE>, <CODE>AgentTermNotReadyEv</CODE>,
 * <CODE>AgentTermUnknownEv</CODE>, <CODE>AgentTermWorkNotReadyEv</CODE>,
 * and <CODE>AgentTermWorkReadyEv</CODE>.
 * <p>
 * @see javax.telephony.Terminal
 * @see javax.telephony.events.TermEv
 * @see javax.telephony.callcenter.Agent
 * @see javax.telephony.callcenter.AgentTerminal
 * @see javax.telephony.callcenter.events.CallCentEv
 * @see javax.telephony.callcenter.events.AgentTermBusyEv
 * @see javax.telephony.callcenter.events.AgentTermLoggedOnEv
 * @see javax.telephony.callcenter.events.AgentTermLoggedOffEv
 * @see javax.telephony.callcenter.events.AgentTermNotReadyEv
 * @see javax.telephony.callcenter.events.AgentTermReadyEv
 * @see javax.telephony.callcenter.events.AgentTermUnknownEv
 * @see javax.telephony.callcenter.events.AgentTermWorkNotReadyEv
 * @see javax.telephony.callcenter.events.AgentTermWorkReadyEv
 * @deprecated as of JTAPI 1.4, replaced by {@link javax.telephony.callcenter.AgentTerminalEvent}
 */
public interface AgentTermEv extends CallCentEv, TermEv {

  /**
   * Returns the <CODE>Agent</CODE> associated with the
   * <CODE>AgentTerminal</CODE>.
   * <p>
   * @since JTAPI v1.2
   * @return The associated Agent.
   */
  public Agent getAgent();


  /**
   * Returns the <CODE>ACDAddress</CODE> the agent currently is or was logged
   * into.
   * <p>
   * <STRONG>Note:</STRONG> This method has been replaced in JTAPI v1.2 and
   * later. Applications should use the <CODE>AgentTermEv.getAgent()</CODE>
   * method to obtain the <CODE>Agent</CODE> and then use the
   * <CODE>Agent.getACDAddress()</CODE> method.
   * <p>
   * @deprecated JTAPI v1.2 This method has been replaced by the getAgent()
   * method.
   * @return The ACDAddress currently or formerly associated with the Agent.
   */
  public ACDAddress getACDAddress();


  /**
   * Returns the ID of the <CODE>Agent</CODE>.
   * <p>
   * <STRONG>Note:</STRONG> This method has been replaced in JTAPI v1.2 and
   * later. Applications should use the <CODE>AgentTermEv.getAgent()</CODE>
   * method to obtain the <CODE>Agent</CODE> and then use the
   * <CODE>Agent.getAgentID()</CODE> method.
   * <p>
   * @deprecated JTAPI v1.2 This method has been replaced by the getAgent()
   * method.
   * @return The Agent ID.
   */
  public java.lang.String getAgentID();


  /**
   * Returns the state of the <CODE>Agent</CODE>.
   * <p>
   * <STRONG>Note:</STRONG> This method has been replaced in JTAPI v1.2 and
   * later. Applications should use the <CODE>AgentTermEv.getAgent()</CODE>
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
   * later. Applications should use the <CODE>AgentTermEv.getAgent()</CODE>
   * method to obtain the <CODE>Agent</CODE> and then use the
   * <CODE>Agent.getAgentAddress()</CODE> method.
   * <p>
   * @deprecated JTAPI v1.2 This method has been replaced by the getAgent()
   * method.
   * @return The Address associated with the Agent's Terminal.
   */
  public Address getAgentAddress();
}
