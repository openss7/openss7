/*
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

package javax.telephony.media;
import  javax.telephony.*;

/**
 * Old MediaCallObserver class.
 * This interface replaced by MediaServiceListener.
 * <p>
 * The MediaCallObserver extends the CallObserver interface and
 * reports all events pertaining to the MediaTerminalConnection object.
 * Events for this object are reported on this observer because, in the
 * core, TerminalConnection events are reported on the CallObserver object.
 * <p>
 * This interface does not have any methods. All events for the
 * MediaTerminalConnection object are reported via the callChangedEvent()
 * method on the CallObserver interface. All MediaTerminalConnection events,
 * therefore, extend the core TermConnEv interface (which extends the core
 * CallEv interface.
 * <p>
 * Applications which desire MediaTerminalConnection events implement this
 * interface as a "signal" to the implementation that is wants to be sent
 * events for the MediaTerminalConnection object.
 *
 * @deprecated   As of JTAPI 1.3, replaced by {@link javax.telephony.media.MediaServiceListener}
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */

public interface MediaCallObserver extends CallObserver {
}
