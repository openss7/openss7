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

/**
 * The PhoneTerminal interface extends the Terminal interface to provide
 * functionality for the Phone package. It allows applications to obtain
 * arrays of telephony Components (each group is called a ComponentGroup)
 * which represents the physical components of telephones.
 */

public interface PhoneTerminal extends Terminal {

  /**
   * Returns an array of ComponentGroup objects available on the Terminal. A
   * ComponentGroup object is composed of a number of Components. Examples of
   * Component objects include headsets, handsets, speakerphones, and
   * buttons. ComponentGroup objects group Components together.
   * <p>
   * @return An array of ComponetGroup objects on this Terminal.
   */
  public ComponentGroup[] getComponentGroups();
}
