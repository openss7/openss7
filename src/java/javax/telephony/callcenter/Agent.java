/*
#pragma ident "%W%      %E%     SMI"

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
 * An <CODE>Agent</CODE> represents an individual capable of handling telephone
 * calls for a particular Address. For example, an agent may be a customer
 * service representative in a call center environment.
 *
 * An Agent is associated with a particular <CODE>AgentTerminal</CODE>, which
 * represents the particular Terminal endpoint associated with the Agent. Each
 * Agent is also <EM>logged into</EM> a particular <CODE>ACDAddress</CODE>.
 * The act of logging into an address announces the availability of the Agent
 * to handle Calls which come into that <CODE>ACDAddress</CODE>. Distinct
 * Agents are used to represent the same individual who is logged into multiple
 * <CODE>ACDAddresses</CODE> from the same <CODE>AgentTerminal</CODE>
 *
 * <H4>Adding and Removing Agents</H5>
 *
 * Agents are created and added to a particular <CODE>ACDAddress</CODE> via the
 * <CODE>AgentTerminal.addAgent()</CODE> method. This method creates a new
 * <CODE>Agent</CODE> associated with the <CODE>AgentTerminal</CODE> and the
 * <CODE>ACDAddress</CODE> given as an argument.
 * <p>
 * Agents are removed from an <CODE>AgentTerminal</CODE> via the
 * <CODE>AgentTerminal.removeAgent()</CODE> method. This method also removes
 * the Agent from its <CODE>ACDAddress</CODE>. The Agent is no longer available
 * to receive telephone calls coming into its <CODE>ACDAddress</CODE>.
 *
 * <H4>The Agent's State</H4>
 *
 * The state of the Agent describes whether it is currently logged into an
 * <CODE>ACDAddress</CODE> or its current ability to handle telephone calls.
 * Applications obtain the state of the Agent via the
 * <CODE>Agent.getState()</CODE> method. Applications may also directly change
 * the state of the Agent via the <CODE>Agent.setState()</CODE> method. Each
 * state is an integer constant defined in this interface and summarized below.
 * <p>
 * <TABLE>
 * <TR><TD WIDTH="20%"><CODE>Agent.LOG_IN</CODE></TD>
 * <TD WIDTH="80%">
 * This state indicates the Agent is logged into an <CODE>ACDAddress</CODE>.
 * </TD></TR>
 *
 * <TR><TD WIDTH="20%"><CODE>Agent.LOG_OUT</CODE></TD>
 * <TD WIDTH="80%">
 * This state indicates the Agent has logged out of an <CODE>ACDAddress</CODE>.
 * </TD></TR>
 *
 * <TR><TD WIDTH="20%"><CODE>Agent.NOT_READY</CODE></TD>
 * <TD WIDTH="80%">
 * This state indicates the Agent is not available to handle Calls because it
 * is busy with other non-call servicing related tasks.
 * </TD></TR>
 *
 * <TR><TD WIDTH="20%"><CODE>Agent.READY</CODE></TD>
 * <TD WIDTH="80%">
 * This state indicates the Agent is available to service Calls.
 * </TD></TR>
 *
 * <TR><TD WIDTH="20%"><CODE>Agent.WORK_NOT_READY</CODE></TD>
 * <TD WIDTH="80%">
 * This state indicates the Agent is not available to service Calls because it
 * is busy with other call-servicing related tasks.
 * </TD></TR>
 *
 * <TR><TD WIDTH="20%"><CODE>Agent.WORK_READY</CODE></TD>
 * <TD WIDTH="80%">
 * This state indicates the Agent is available to service Calls and is also
 * performing other call-servicing related tasks.
 * </TD></TR>
 *
 * <TR><TD WIDTH="20%"><CODE>Agent.BUSY</CODE></TD>
 * <TD WIDTH="80%">
 * This state indicates the Agent is not available to service Calls because it
 * is busy with another Call.
 * </TD></TR>
 *
 * <TR><TD WIDTH="20%"><CODE>Agent.UNKNOWN</CODE></TD>
 * <TD WIDTH="80%">
 * This state indicates the state of the Agent is currently not known.
 * </TD></TR>
 * </TABLE>
 * <p>
 * The following diagram illustrates the valid state transitions for the
 * <CODE>Agent</CODE>. The implementation must guarantee the Agent state
 * adheres to these state transitions. If an applications requests an Agent
 * state change which violates the transitions, the <CODE>setState()</CODE>
 * method on this interface will throw <CODE>InvalidStateException</CODE>.
 * The state of this object can be altered by invoking the the setState method.
 * <P>
 * <IMG SRC="doc-files/callcent-agentstate.gif">
 * </P>
 *
 * <H4>Observers and Events</H4>
 *
 * Application are notified when an Agent changes state via an event. Each
 * Agent state has a corresponding event. Agent state changes are reported via
 * two interfaces: <CODE>ACDAddressObserver</CODE> for the
 * <CODE>ACDAddress</CODE> associated with this Agent, and
 * <CODE>AgentTerminalObserver</CODE> for the <CODE>AgentTerminal</CODE>
 * associated with this Agent. Both of these interfaces receive Agent state
 * events.
 * <p>
 * @see javax.telephony.callcenter.AgentTerminal
 * @see javax.telephony.callcenter.ACDAddress
 * @see javax.telephony.callcenter.AgentTerminalObserver
 * @see javax.telephony.callcenter.ACDAddressObserver
 */

public interface Agent {

  /**
   * The <CODE>Agent.UNKNOWN</CODE> state indicates the state of the Agent is
   * currently not known.
   */
  public static final int UNKNOWN = 0;


  /**
   * The <CODE>Agent.LOG_IN</CODE> state indicates that an Agent, which is
   * associated with an <CODE>AgentTerminal</CODE> is logged into an
   * <CODE>ACDAddress</CODE>.
   */
  public static final int LOG_IN = 1;


  /**
   * The <CODE>Agent.LOG_OUT</CODE> state indicates the Agent has logged out of
   * an <CODE>ACDAddress</CODE>.
   */
  public static final int LOG_OUT = 2;


  /**
   * The <CODE>Agent.NOT_READY</CODE> state indicates the Agent is not
   * available to handle Calls because it is busy with other non-call servicing
   * related tasks.
   */
  public static final int NOT_READY = 3;


  /**
   * The <CODE>Agent.READY</CODE> state indicates the Agent is available to
   * service Calls.
   */
  public static final int READY = 4;


  /**
   * The <CODE>Agent.WORK_NOT_READY</CODE> state indicates the Agent is not
   * available to service Calls because it is busy with other call-servicing
   * related tasks.
   */
  public static final int WORK_NOT_READY = 5;


  /**
   * The <CODE>Agent.WORK_READY</CODE> state indicates the Agent is available
   * to service Calls and is also performing other call-servicing related
   * tasks.
   */
  public static final int WORK_READY = 6;


  /**
   * The <CODE>Agent.BUSY</CODE> state indicates the Agent is not available
   * to service Calls because it is busy with another Call.
   */
  public static final int BUSY = 7;


  /**
   * Changes the state of the Agent. The new desired state is given as a
   * parameter to this method. The Agent's state must adhere to the state
   * transition diagram given in this interface definition. If the given, new
   * Agent state violates the transition diagram, this method throws
   * <CODE>InvalidStateException</CODE>.
   * <p>
   * <B>Pre-Conditions</B>
   * <OL>
   * <LI>this.getAgentTerminal().getProvider().getState()==Provider.IN_SERVICE
   * <LI>this.getState() == Agent.READY, Agent.NOT_READ, Agent.WORK_READY, or
   * Agent.WORK_NOT_READY.
   * </OL>
   * <B>Post-Conditions</B>
   * <OL>
   * <LI>this.getAgentTerminal().getProvider().getState()==Provider.IN_SERVICE
   * <LI>this.getState() == state
   * <LI>The proper Agent state event is delivered to the application
   * </OL>
   * <p>
   * @param state The new, desired state of the Agent.
   * @exception InvalidArgumentException The state given as the argument is
   * not a valid Agent state.
   * @exception InvalidStateException Either the provider is not in service
   * or the Agent is not in a state in which the requested state change can
   * be honored.
   */
  public void setState(int state)
    throws InvalidArgumentException, InvalidStateException;


  /**
   * Returns the current Agent state. This method returns one of the integer
   * constants defined by this interface.
   * <p>
   * @return The current Agent state.
   */
  public int getState();


  /**
   * Returns the Agent's string identification. This identification is passed
   * as an argument to the <CODE>AgentTerminal.addAgent()</CODE> method.
   * <p>
   * @return the Agent's ID.
   */
  public String getAgentID();


  /**
   * Returns the <CODE>ACDAddress</CODE> which this Agent is logged into.
   * <p>
   * @return The ACDAddress this Agent is logged into.
   */
  public ACDAddress getACDAddress();


  /**
   * Returns the Address associated with the <CODE>AgentTerminal</CODE> from
   * which this Agent is logged in.
   * <p>
   * @return The Agent's Address.
   */
  public Address getAgentAddress();


  /**
   * Returns the <CODE>AgentTerminal</CODE> associated with this Agent and
   * which this Agent is logged into. If the state of the Agent is
   * <CODE>Agent.LOG_OUT</CODE>, this method returns null.
   * <p>
   * @return The AgentTerminal associated with this Agent.
   */
  public AgentTerminal getAgentTerminal();
}
