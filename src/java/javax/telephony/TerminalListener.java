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

package javax.telephony;
import  javax.telephony.TerminalEvent;

/**
 * The <CODE>TerminalListener</CODE> interface reports all changes which happen
 * to the Terminal object.
 *
 * <H4>Introduction</H4>
 *
 * These changes are reported as events to the
 * <CODE>TerminalListener()</CODE>. Applications
 * must instantiate an object which implements this interface and then use
 * the <CODE>Terminal.addListener()</CODE>) method to register the object to
 * receive all future events associated with the Terminal object.
 *
 * <H4>Terminal Listener Ending</H4>
 *
 * At various times, the underlying implementation may not be able to observe
 * the Terminal. In these instances, the TerminalListener is sent an
 * terminalListenerEnded event. This indicates that the application will not 
 * receive further events associated with the Terminal object. This listener
 * is no longer reported via the <CODE>Terminal.getListeners()</CODE> method.
 * <p>
 * @see javax.telephony.TerminalEvent
 * @version 10/22/99 1.5
 */

public interface TerminalListener extends java.util.EventListener {


/**
 * The <CODE>terminalListenerEnded</CODE> event indicates that the application
 * will no longer receive Terminal events on the instance of the
 * <CODE>TerminalListener</CODE>.
 *<p>
 * This constant indicates a specific event passed via a <CODE>TerminalEvent</CODE>
 * event, and is reported on the <CODE>TerminalListener</CODE> interface.
 *<p>
 * @see javax.telephony.TerminalEvent
 */

public void terminalListenerEnded(TerminalEvent event);

}

