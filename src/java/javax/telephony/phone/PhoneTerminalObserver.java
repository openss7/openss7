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
import  javax.telephony.TerminalObserver;

/**
 * The PhoneTerminalObserver interface is used to report all Phone-related
 * events. Note that this observer does not have any method associated with
 * it. Applications which implement a TerminalObserver class should also
 * implement this interface to indicate to the implementation that it wants
 * Phone-related events sent to it. If an application's observer does not
 * implement this interface, phone-related events will not be sent to the
 * application.
 * <p>
 * @deprecated As of JTAPI 1.4, replaced by {@link javax.telephony.phone.PhoneTerminalListener}
 */

public interface PhoneTerminalObserver extends TerminalObserver {
}
