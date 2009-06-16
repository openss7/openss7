/*
#pragma ident "@(#)PhoneTermEv.java	1.8      97/01/20     SMI"

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

package javax.telephony.phone.events;
import  javax.telephony.events.TermEv;
import  javax.telephony.phone.*;

/**
 * The PhoneTermEv interface extends the TermEv interface and is the base
 * event interface for all phone-components related events. All component
 * events must extends this interface. These events are reported through the
 * TerminalObserver interface.
 * @deprecated as of JTAPI 1.4, replaced by {@link javax.telephony.phone.PhoneTerminalEvent}
 */
public interface PhoneTermEv extends PhoneEv, TermEv {

  /**
   * Returns the ComponentGroup object associated with this event.
   * <p>
   * @return The ComponentGroup object associated with this event.
   */
  public ComponentGroup getComponentGroup();


  /**
   * Returns the Component object responsible for this event.
   * <p>
   * @return The Component object responsible for this event.
   */
  public Component getComponent();
}
