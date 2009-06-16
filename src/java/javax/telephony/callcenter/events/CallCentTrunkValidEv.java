/*
#pragma ident "@(#)CallCentTrunkValidEv.java	1.4      99/02/09     SMI"

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

package javax.telephony.callcenter.events;

/**
 * The <CODE>CallCentTrunkValidEv</CODE> interface indicates that a Trunk has
 * moved into the <CODE>CallCenterTrunk.VALID_TRUNK</CODE> state. This
 * interface extends the <CODE>CallCentTrunkEv</CODE> interface and is reported
 * via the <CODE>CallCenterCallObserver</CODE> interface.
 * <p>
 * This interface defines no additional methods.
 * <p>
 * @see javax.telephony.Call
 * @see javax.telephony.CallObserver
 * @see javax.telephony.callcenter.CallCenterTrunk
 * @see javax.telephony.callcenter.CallCenterCallObserver
 * @see javax.telephony.callcenter.events.CallCentTrunkEv
 * @deprecated as of JTAPI 1.4, replaced by {@link javax.telephony.callcenter.CallCenterCallEvent}
 */
public interface CallCentTrunkValidEv extends CallCentTrunkEv {

  /**
   * Event id
   */
  public static final int ID = 317;
}
