/*
#pragma ident "@(#)ACDConnection.java	1.11      99/02/23     SMI"

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
 * An <CODE>ACDConnection</CODE> models either a direct relationship between
 * a <CODE>Call</CODE> and an <CODE>ACDAddress</CODE> or an indirect relationship 
 * between a <CODE>Call</CODE> and an <CODE>ACDAddress</CODE> through an 
 * <CODE>ACDManagerAddress</CODE>. 
 * <p>
 * The direct relationship occurs when a <CODE>Call</CODE> arrives at an
 * <CODE>ACDAddress</CODE>. The indirect relationship occurs when a 
 * <CODE>Call</CODE> arrives at an <CODE>ACDManagerAddress</CODE> and the
 * implementation of the <CODE>ACDMangerAddress</CODE> determines that it must 
 * involve an <CODE>ACDAddress</CODE> in the <CODE>Call</CODE>.
 * <p>
 * The <CODE>ACDConnection</CODE> to an <CODE>ACDAddress</CODE> in either case, 
 * direct or indirect, models a Call that is being routed to an agent logged 
 * into the <CODE>ACDAddress</CODE>, or a call that is being queued for agents 
 * logged into the <CODE>ACDAddress</CODE>
 *
 * <H4> ACDConnection as a Direct Connection</H4>
 *
 * <IMG SRC="doc-files/acdcon-direct.gif" ALIGN="center">
 *
 * <H4> ACDConnection as an Indirect Connection</H4>
 *
 * <IMG SRC="doc-files/acdcon-indirect.gif" ALIGN="center">
 *
 * <p>
 * The <CODE>ACDConnection</CODE> is not a connection in the same sense as a
 * <CODE>Connection</CODE>, because it never represents a call to an endpoint.
 * Its purpose is to model a call that is being routed or queued by an ACD
 * system.
 *
 * <H4>ACDConnections and TerminalConnections</H4>
 *
 * The <CODE>getTerminalConnection()</CODE> method on the core 
 * <CODE>Connection</CODE> interface, which <CODE>ACDConnection</CODE> extends, 
 * will always return <CODE>null</CODE> because <CODE>ACDAddresses</CODE> do 
 * not have <CODE>Terminals</CODE> associated with them.
 *
 * <H4> ACDConnection States </H4>
 *
 * The state of the <CODE>ACDConnection</CODE> is available through the
 * <CODE>ACDConnection.getState()</CODE> method inherited from the core 
 * <CODE>Connection</CODE> interface.  Each state is an integer constant
 * defined in the core <CODE>Connection</CODE> interface.  Their meaning
 * in this interface are summarized below:
 * <p>
 * <TABLE>
 * <TR><TD WIDTH="20%"><CODE>ACDConnection.IDLE</CODE></TD>
 * <TD WIDTH="80%">
 * As in the core, this is the initial and transitory state for new 
 * <CODE>ACDConnection</CODE> objects.
 * </TD></TR>
 *
 * <TR><TD WIDTH="20%"><CODE>ACDConnection.INPROGRESS</CODE></TD>
 * <TD WIDTH="80%">
 * This state indicates that an <CODE>ACDConnection</CODE> is queued at a
 * particular <CODE>ACDAddress</CODE>. This will result when there are no 
 * agents available to route the call to.
 * </TD></TR>
 * 
 * <TR><TD WIDTH="20%"><CODE>ACDConnection.ALERTING</CODE></TD>
 * <TD WIDTH="80%">
 * This state indicates that the <CODE>ACDConnection</CODE> has been made to a
 * particular <CODE>ACDAddress</CODE>. This state is only valid for 
 * <CODE>ACDConnections</CODE> that are not associated with an 
 * <CODE>ACDManagerConnection</CODE>.
 * </TD></TR>
 * 
 * <TR><TD WIDTH="20%"><CODE>ACDConnection.DISCONNECTED</CODE></TD>
 * <TD WIDTH="80%">
 * This state has the same definition as in the core.
 * </TD></TR>
 * 
 * <TR><TD WIDTH="20%"><CODE>ACDConnection.FAILED</CODE></TD>
 * <TD WIDTH="80%">
 * This state has the same definition as in the core.
 * </TD></TR>
 * 
 * <TR><TD WIDTH="20%"><CODE>ACDConnection.UNKNOWN</CODE></TD>
 * <TD WIDTH="80%">
 * This state has the same definition as in the core.
 * </TD></TR>
 * </TABLE>
 * <p>
 *
 * <H4>ACDConnection State Transitions</H4>
 *
 * The <CODE>ACDConnection</CODE> class defines the allowable 
 * <CODE>ACDConnection</CODE> state transitions. These finite-state transitions 
 * must be guaranteed by the implementation. Each method that causes a change in
 * an <CODE>ACDConnection</CODE> state must be consistent with this state diagram. 
 * <P>
 * Note there is a general left-to-right progression of the state transitions.
 * A Connection object may transition into and out of the
 * <CODE>ACDConnection.UNKNOWN</CODE> state at any time with the annotated
 * exceptions (hence, the asterisk qualifier next to its bidirectional transition 
 * arrow).
 * <p>
 * <IMG SRC="doc-files/acdconnection-states.gif" ALIGN="center">
 *
 * @see javax.telephony.callcenter.ACDAddress
 * @see javax.telephony.callcenter.ACDManagerAddress
 * @see javax.telephony.callcenter.ACDManagerConnection
 */
public interface ACDConnection extends Connection {

  /**
   * Returns the <CODE>ACDManagerConnection</CODE> associated with this 
   * <CODE>ACDConnection</CODE>.
   * A null is returned if this <CODE>ACDConnection</CODE> is not in an 
   * indirect relationship between a Call, an <CODE>ACDAddress</CODE> and an 
   * <CODE>ACDManagerAddress</CODE>.
   * <p>
   * @return The ACDManagerConnection associated with this ACDConnection.
   * @exception MethodNotSupportedException This method is not supported by
   * the implementation.
   */
  public ACDManagerConnection getACDManagerConnection()
    throws MethodNotSupportedException;
}
