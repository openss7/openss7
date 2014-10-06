/*
#pragma ident "@(#)ACDManagerConnection.java	1.10      99/02/23     SMI"

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
 * The <CODE>ACDManagerConnection</CODE> models a call that is being offered for 
 * routing by an ACD. The ACD mechanism selects among Agent Groups as recipients 
 * of the call.  A call can be offered to one or more Agent Groups.  If none of 
 * those groups contain an agent that is able to service the call, the call will 
 * be queued at each of the groups.  As soon as an agent from one of those groups 
 * can be given the call, the call is connected to that agent's Terminal and
 * all the queued instances of that call are removed.
 *
 * <H4> ACDManagerConnections and other ACD Objects</H4>
 *
 * The <CODE>ACDManagerConnection</CODE> interface models the relationship between
 * a <CODE>Call</CODE> and an <CODE>ACDManagerAddress</CODE>. A call placed to an
 * <CODE>ACDManagerAddress</CODE> is one that is being offered for routing to 
 * Agent Groups. Agent Groups are modeled by <CODE>ACDAddresses</CODE>.  When an 
 * implementation of an <CODE>ACDManagerAddress</CODE> routes a call to an 
 * <CODE>ACDAddress</CODE>, an <CODE>ACDConnection</CODE> is added to the routing
 * ACDManagerConnection to model the offering or queuing nature of the call being
 * extended to the <CODE>ACDAddress</CODE>
 *
 * <H4> ACDManagerConnections and TerminalConnections </H4>
 * 
 * The <CODE>getTerminalConnection()</CODE> method on the <CODE>Connection</CODE>
 * interface, that <CODE>ACDManagerConnection</CODE> extends, will always return 
 * null because <CODE>ACDManagerAddresses</CODE> do not have <CODE>Terminals</CODE>
 * associated with them.
 * 
 * <H4> ACDManagerConnection States </H4>
 *
 * The following are the possible core <CODE>Connection</CODE> states presented by this
 * interface: IDLE, ALERTING, FAILED, DISCONNECTED.
 * <p>
 * The state of the <CODE>ACDManagerConnection</CODE> is availble through the
 * <CODE>ACDManagerConnection.getState()</CODE> inherited from the core 
 * <CODE>Connection</CODE> interface.  Each state is an integer constant
 * defined in the core <CODE>Connection</CODE> interface.  Their meaning
 * in this interface are summarized below:
 * <p>
 * <TABLE>
 * <TR><TD WIDTH="20%"><CODE>ACDManagerConnection.IDLE</CODE></TD>
 * <TD WIDTH="80%">
 * As in the core, this is the initial and transitory state for new 
 * <CODE>ACDConnection</CODE> objects.
 * </TD></TR>
 
 * <TR><TD WIDTH="20%"><CODE>ACDManagerConnection.ALERTING</CODE></TD>
 * <TD WIDTH="80%">
 * This state indicates that the <CODE>ACDManagerConnection</CODE> has been 
 * made to a particular <CODE>ACDManagerAddress</CODE>. 
 * </TD></TR>
 * 
 * <TR><TD WIDTH="20%"><CODE>ACDManagerConnection.DISCONNECTED</CODE></TD>
 * <TD WIDTH="80%">
 * This state has the same definition as in the core.
 * </TD></TR>
 * 
 * <TR><TD WIDTH="20%"><CODE>ACDManagerConnection.FAILED</CODE></TD>
 * <TD WIDTH="80%">
 * This state has the same definition as in the core.
 * </TD></TR>
 * 
 * <TR><TD WIDTH="20%"><CODE>ACDManagerConnection.UNKNOWN</CODE></TD>
 * <TD WIDTH="80%">
 * This state has the same definition as in the core.
 * </TD></TR>
 * </TABLE>
 * <p>
 * <H4>ACDManagerConnection State Transitions</H4>
 *
 * The <CODE>ACDManagerConnection</CODE> class defines the allowable 
 * <CODE>ACDManagerConnection</CODE> state transitions. These finite-state 
 * transitions  must be guaranteed by the implementation. Each method that 
 * causes a change in an <CODE>ACDManagerConnection</CODE> state must be 
 * consistent with this state diagram. 
 * <P>
 * Note there is a general left-to-right progression of the state transitions.
 * A Connection object may transition into and out of the
 * <CODE>ACDManagerConnection.UNKNOWN</CODE> state at any time with the annotated
 * exceptions (hence, the asterisk qualifier next to its bidirectional transition 
 * arrow).
 * <p>
 * <IMG SRC="doc-files/acdconnection-states.gif" ALIGN="center">
 *
 * @see javax.telephony.callcenter.ACDAddress
 * @see javax.telephony.callcenter.ACDManagerAddress
 * @see javax.telephony.callcenter.ACDConnection
 */

public interface ACDManagerConnection extends Connection {

  /**
   * Returns the <CODE>ACDConnection</CODE> objects associated with this
   * <CODE>ACDManagerConnection</CODE>. A null will be returned if this 
   * <CODE>ACDManagerConnection</CODE> has no associated <CODE>ACDConnections</CODE>.
   * <p>
   * @return The list of ACDConnection associated with this
   * ACDManagerConnection.
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   */
  public ACDConnection[] getACDConnections()
    throws MethodNotSupportedException;
}
