/**
#pragma ident "@(#)CallCenterTrunk.java	1.7      99/02/23     SMI"

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
import  javax.telephony.Call;

/**
 * <H4>Introduction</H4>
 *
 * The <CODE>CallCenterTrunk</CODE> interface represents a trunk on the
 * underlying telephony hardware. Each trunk has four attributes: its
 * <EM>name</EM>, its <EM>state</EM>, its <EM>type</EM>, and its <EM>associated
 * Call</EM>.
 *
 * <H4>The Trunk Name</H4>
 *
 * The first attribute of a trunk is its string name. This name is assigned to
 * the trunk by the underlying telephony hardware. Applications obtains the
 * name of the trunk via the <CODE>getName()</CODE> method on this interface.
 * A trunk's name does not change throughout its lifetime.
 *
 * <H4>The Trunk State</H4>
 *
 * The second attribute of a trunk is its state. The state indicates whether
 * the trunk is associated with a Call (i.e. valid) or not associated with a
 * Call (i.e. invalid). Applications obtain the state of a trunk via the
 * <CODE>getState()</CODE> method on this interface. The following chart
 * summarizes the two trunk states:
 * <p>
 * <TABLE>
 * <TR><TD WIDTH="20%"><CODE>CallCenterTrunk.VALID_TRUNK</CODE></TD>
 * <TD WIDTH="80%">The trunk is valid and associated with a Call.</TD></TR>
 *
 * <TR><TD WIDTH="20%"><CODE>CallCenterTrunk.INVALID_TRUNK</CODE></TD>
 * <TD WIDTH="80%">The trunk is not valid and not associated with a Call.</TD>
 * </TR>
 * </TABLE>
 *
 * <H4>The Trunk Type</H4>
 *
 * The third attribute of a trunk is its type. The type indicates whether the
 * trunk is an "incoming", "outgoing", or "unknown". Applications obtain the
 * trunk type via the <CODE>getType()</CODE> method on this interface. The
 * following chart summarizes the three trunk types:
 * <p>
 * <TABLE>
 * <TR><TD WIDTH="20%"><CODE>CallCenterTrunk.INCOMING_TRUNK</CODE></TD>
 * <TD WIDTH="80%">The trunk is an incoming trunk</TD></TR>
 *
 * <TR><TD WIDTH="20%"><CODE>CallCenterTrunk.OUTGOING_TRUNK</CODE></TD>
 * <TD WIDTH="80%">The trunk is an outgoing trunk</TD></TR>
 *
 * <TR><TD WIDTH="20%"><CODE>CallCenterTrunk.UNKNOWN_TRUNK</CODE></TD>
 * <TD WIDTH="80%">The trunk type is not known</TD></TR>
 * </TABLE>
 *
 * <H4>The Associated Call</H4>
 *
 * The fourth attribute of a trunk is the Call associated with it. The Call
 * is assigned when the trunk is first created and remains the same throughout
 * the lifetime of the trunk. Applications obtain the associated Call via the
 * <CODE>getCall()</CODE> method on this interface.
 *
 * <H4>Observers and Events</H4>
 *
 * Applications receive an event whenever the state of the trunk changes. These
 * events are reported via the <CODE>CallCenterCallObserver</CODE> interface.
 * The <CODE>CallCentTrunkValidEv</CODE> event is delivered when the trunk
 * moves to the <CODE>CallCenterTrunk.VALID_TRUNK</CODE> state and a
 * <CODE>CallCentTrunkInvalidEv</CODE> event is devliered when the trunk moves
 * to the <CODE>CallCenterTrunk.INALID_TRUNK</CODE> state.
 * <p>
 * @see javax.telephony.callcenter.CallCenterCall
 * @see javax.telephony.callcenter.CallCenterCallObserver
 * @see javax.telephony.callcenter.events.CallCentTrunkValidEv
 * @see javax.telephony.callcenter.events.CallCentTrunkInvalidEv
 */

public interface CallCenterTrunk {

  /**
   * Trunk state: The trunk is invalid.
   */
  public final static int INVALID_TRUNK = 0x1;


  /**
   * Trunk state: The trunk is valid.
   */
  public final static int VALID_TRUNK = 0x2;


  /**
   * Trunk type: The trunk is incoming.
   */
  public final static int INCOMING_TRUNK = 0x1;


  /**
   * Trunk type: The trunk is outgoing.
   */
  public final static int OUTGOING_TRUNK = 0x2;


  /**
   * Trunk type: The trunk is unknown.
   */
  public final static int UNKNOWN_TRUNK = 0x3;


  /**
   * Returns the name of the trunk. This name is assigned by the underlying
   * telephony hardware.
   * <p>
   * @return The name of the trunk.
   */
 public String getName();


  /**
   * Returns the current state of the Trunk.
   * <p>
   * @return The current state of the trunk.
   */
  public int getState();


  /**
   * Returns the type of trunk.
   * <p>
   * @return The type of trunk.
   */
  public int getType();


  /**
   * Returns the Call associated with this trunk. This Call reference remains
   * valid throughout the lifetime of the trunk, despite the current state of
   * the trunk.
   * <p>
   * @return The Call associated with this trunk.
   */
  public Call getCall();
}
