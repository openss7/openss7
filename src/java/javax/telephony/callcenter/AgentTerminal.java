/*
#pragma ident "@(#)AgentTerminal.java	1.13      99/03/02     SMI"

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
 * The <CODE>AgentTerminal</CODE> interface extends the core
 * <CODE>Terminal</CODE> interface. This interface add methods to support ACD
 * and Agent features.
 *
 * <H4>Adding and Removing Agents</H4>
 *
 * Applications may create and add new <CODE>Agents</CODE> associated with this
 * <CODE>AgentTerminal</CODE> via the <CODE>AgentTerminal.addAgent()</CODE>
 * method. This method creates and returns a new <CODE>Agent</CODE> which is
 * associated with this <CODE>AgentTerminal</CODE> and an
 * <CODE>ACDAddress</CODE> given as an argument. Agents model human individuals
 * who are able to service telephone calls coming into an Address.
 * <p>
 * Agents may be removed from this <CODE>AgentTerminal</CODE> via the
 * <CODE>AgentTerminal.removeAgent()</CODE> method.
 * <p>
 * @see javax.telephony.Terminal
 * @see javax.telephony.callcenter.Agent
 * @see javax.telephony.callcenter.ACDAddress
 * @see javax.telephony.callcenter.AgentTerminalObserver
 */

public interface AgentTerminal extends Terminal {

  /**
   * Creates a new <CODE>Agent</CODE> associated with this Terminal and is
   * logged into the <CODE>ACDAddress</CODE> given as an argument. This method
   * returns the new <CODE>Agent</CODE> when it has been successfully created
   * and logged into the <CODE>ACDAddress</CODE>.
   * <p>
   * Applications remove the new Agent via the <CODE>removeAgent()</CODE>
   * method defined by this interface. Applications obtain all Agents
   * associated with this Terminal via the <CODE>getAgents()</CODE> method
   * defined on this interface.
   * <p>
   * Subsequent invocations of this methods with the same agentAddress and
   * acdAddress parameters will simply return the Agent originally created.
   * <p>
   * <B>Pre-Conditions</B>
   * <OL>
   * <LI>this.getProvider().getState() == Provider.IN_SERVICE
   * <LI>initialState == Agent.LOGIN, Agent.READY, or Agent.NOT_READY
   * </OL>
   * <B>Post-Conditions</B>
   * <OL>
   * <LI>Let agent be the Agent created an returned
   * <LI>this.getProvider().getState() == Provider.IN_SERVICE
   * <LI>agent is an element of this.getAgents()
   * <LI>agent.getState() == initialState
   * <LI>Either an AgentTermLoggedOnEv, AgentTermReadyEv, or
   * AgentTermNotReadyEv is delivered for this Agent
   * <LI>Either an ACDAddrLoggedOnEv, ACDAddrReadyEv, or ACDAddrNotReadyEv is
   * delivered for this Agent
   * </OL>
   * @since JTAPI v1.2
   * @param agentAddress The Address associated with this Terminal. Terminals
   * may support more than one Address on which an Agent may be associated.
   * @param acdAddress The Address which the Agent logs into.
   * @param initialState The initial state of the Agent.
   * @param agentID The Agent's string identification.
   * @param password The string password which authorizes the application to
   * log in as an Agent.
   * @return An Agent representing the association between this AgentTerminal
   * and the ACDAddress.
   * @exception ResourceUnavailableException An internal resource necessary
   * for adding the Agent to this Terminal and ACDAddress is unavailable.
   * @exception InvalidArgumentException An argument provided is not valid
   * either by not providing enough information for addAgent() or is
   * inconsistent with another argument.
   * @exception InvalidStateException Either the provider is not in service
   * or the AgentTerminal is not in a state in which it can be logged into
   * the ACDAddress.
   */
  public Agent addAgent(Address agentAddress, ACDAddress acdAddress,
			int initialState, String agentID, String password)
    throws InvalidArgumentException, InvalidStateException,
      ResourceUnavailableException;


  /**
   * Removes a previously added <CODE>Agent</CODE> from this
   * <CODE>AgentTerminal</CODE>. This method returns when the Agent is logged
   * out of the <CODE>ACDAddress</CODE> and the state of the Agent moves to
   * <CODE>Agent.LOG_OUT</CODE>.
   * <p>
   * <B>Pre-Conditions</B>
   * <OL>
   * <LI>this.getProvider().getState() == Provider.IN_SERVICE
   * <LI>agent is an element of this.getAgents()
   * </OL>
   * <B>Post-Conditions</B>
   * <OL>
   * <LI>this.getProvider().getState() == Provider.IN_SERVICE
   * <LI>agent is not an element of this.getAgents()
   * <LI>agent.getState() == Agent.LOG_OUT
   * <LI>AgentTermLoggedOffEv and ACDAddrLoggedOffEv is delivered for the
   * Agent.
   * </OL>
   * @since JTAPI v1.2
   * @param agent The Agent to be removed and logged out.
   * @exception InvalidArgumentException The Agent given is not valid.
   * @exception InvalidStateException Either the provider is not in service
   * or the AgentTerminal is not in a state in which it can be logged out of
   * the ACDAddress.
   * @see javax.telephony.callcenter.events.AgentTermLoggedOffEv
   * @see javax.telephony.callcenter.events.AgentTermLoggedOnEv
   */
  public void removeAgent(Agent agent)
    throws InvalidArgumentException, InvalidStateException;


  /**
   * Returns an array of Agents current associated with this Terminal. This
   * method returns <CODE>null</CODE> is no Agents exist. Agents are reported
   * via this method once they are added via the <CODE>addAgent()</CODE> method
   * until they are removed via the <CODE>removeAgent()</CODE> method defined
   * on this interface.
   * <p>
   * @return A list of Agents associated with this Terminal.
   */
  public Agent[] getAgents();


  /**
   * Sets the current list of Agents on this Terminal. This method may remove
   * Agents previously on this Terminal. When this method is finished, the set
   * of Agents on this Terminal will be the given array.
   * <p>
   * <STRONG>Note:</STRONG> This method has been removed for JTAPI v1.2 and
   * later. It has been replaced with the <CODE>AgentTerminal.addAgent()</CODE>
   * and <CODE>AgentTerminal.removeAgent()</CODE> methods. This method should
   * now always throw <CODE>MethodNotSupportedException</CODE>.
   * <p>
   * @deprecated Since JTAPI v1.2. Has been replaced with addAgent() and
   * removeAgent()
   * @param agents The array of Agents to be either added, removed, or changed.
   * @exception MethodNotSupportedException This exception should always be
   * thrown for JTAPI v1.2 and later.
   */
  public void setAgents(Agent[] agents)
    throws MethodNotSupportedException;
}
