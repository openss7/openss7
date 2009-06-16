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

package javax.telephony.callcontrol;
import  javax.telephony.AddressObserver;

/**
 * The <CODE>CallControlAddressObserver</CODE> interface reports all events
 * for the <CODE>CallControlAddress</CODE> interface. Applications implement
 * this interface to receive <CODE>CallControlAddress</CODE>-related events.
 * All events are reported via the
 * <CODE>AddressObserver.addressChangedEvent()</CODE> method. This interface,
 * therefore, allows applications to signal to the implementation that they
 * are interested in call control package events. This interface defines no
 * additional methods.
 * <p>
 * All events must extend the <CODE>CallCtlAddrEv</CODE> event interface,
 * which in turn, extends the core <CODE>AddrEv</CODE> interface.
 * <p>
 * The following are those events which are associated with this interface:
 * <p>
 * <TABLE CELLPADDING=2>
 * <TR>
 * <TD WIDTH="20%"><CODE>CallCtlAddrDoNotDisturbEv</CODE></TD>
 * <TD WIDTH="80%">
 * Indicates the do not disturb characteristics of this Address has changed.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>CallCtlAddrForwardEv</CODE></TD>
 * <TD WIDTH="80%">
 * Indicates the forwarding characteristics of this Address has changed.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%"><CODE>CallCtlAddrMessageWaitingEv</CODE></TD>
 * <TD WIDTH="80%">
 * Indicates the message waiting characteristics of this Address has changed.
 * </TD>
 * </TR>
 * </TABLE>
 * <p>
 * @see javax.telephony.AddressObserver
 * @see javax.telephony.events.AddrEv
 * @see javax.telephony.callcontrol.CallControlAddress
 * @see javax.telephony.callcontrol.events.CallCtlAddrEv
 * @see javax.telephony.callcontrol.events.CallCtlAddrDoNotDisturbEv
 * @see javax.telephony.callcontrol.events.CallCtlAddrForwardEv
 * @see javax.telephony.callcontrol.events.CallCtlAddrMessageWaitingEv
 * @deprecated As of JTAPI 1.4, replaced by {@link javax.telephony.callcontrol.CallControlAddressListener}
 */

public interface CallControlAddressObserver extends AddressObserver {
}
