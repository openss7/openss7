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

/**
 * The <CODE>CallCentCallAppDataEv</CODE> event interfaces indicates that the
 * application data associated with the Call has changed. This interface
 * extends the <CODE>CallCentCallEv</CODE> interface and is reported via the
 * <CODE>CallCenterCallObserver</CODE> interface.
 * <p>
 * The <CODE>CallCentCallAppDataEv.getApplicationData()</CODE> method returns
 * the new application data.
 * <p>
 * @see javax.telephony.Call
 * @see javax.telephony.callcenter.CallCenterCall
 * @see javax.telephony.callcenter.CallCenterCallObserver
 * @see javax.telephony.callcenter.events.CallCentCallEv
 * @deprecated as of JTAPI 1.4, replaced by {@link javax.telephony.callcenter.CallCenterCallEvent}
 */
public interface CallCentCallAppDataEv extends CallCentCallEv {

  /**
   * Event id.
   */
  public static final int ID = 316;


  /**
   * Returns the new application data for this call. This method returns null
   * if the application data has been cleared from the call.
   * <p>
   * @return The data object, null if it has been cleared.
   */
  public Object getApplicationData();
}

