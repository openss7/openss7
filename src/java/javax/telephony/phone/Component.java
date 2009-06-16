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

package javax.telephony.phone;
import  javax.telephony.*;
import  javax.telephony.phone.capabilities.*;

/**
 * The Component interface is the base interface for all individual
 * components used to model telephone hardware. Each individual component
 * extends this interface.
 * <p>
 * Each component is identified not only by its type, but also by an
 * identifying name, which may be obtained via the <EM>getName()</EM> method
 * on this interface.
 */

public interface Component {

  /**
   * Returns the name of the Component.
   * <p>
   * @return The name of this component.
   */
  public String getName();


  /**
   * Returns the dynamic capabilities for this <CODE>Component</CODE> instance.
   * Static capabilities are not available for components.
   * <p>
   * @return The dynamic component capabilities.
   */
  public ComponentCapabilities getCapabilities();
}
