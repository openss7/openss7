/*
 * MessageConstants.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
 * ECTF S.410-R2 Source code distribution.
 *
 * Copyright (c) 2002, Enterprise Computer Telephony Forum (ECTF),
 * All Rights Reserved.
 *
 * Use and redistribution of this file is subject to a License.
 * For terms and conditions see: javax/telephony/media/LICENSE.HTML
 * 
 * In short, you can use this source code if you keep and display
 * the ECTF Copyright and the License conditions. The code is supplied
 * "AS IS" and ECTF disclaims all warranties and liability.
 */

package javax.telephony.media.messaging;

import javax.telephony.media.*;
import java.util.Dictionary;

/**
 * Symbols used in various MessageService methods.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public interface MessageConstants extends MediaConstants {
    Symbol e_NotSupported	= ESymbol.Error_NotSupported;

    /** If openNamedService could not find a registered service supplier.*/
    Symbol e_NotFound		= ESymbol.Error_NotFound;

    Symbol ev_ServiceClosed	= ESymbol.Service_Closed;
    Symbol ev_ServiceOpened	= ESymbol.Service_Opened;
    Symbol ev_RegisterService	= ESymbol.Session_RegisterService;
    Symbol ev_UnregisterService = ESymbol.Session_UnregisterService;
    Symbol ev_OpenNamedService	= ESymbol.Session_FindService;
    Symbol ev_Close	 	= ESymbol.Session_ReleaseService;

    Symbol k_ServiceName	= ESymbol.Session_ServiceName;


    Symbol k_Qualifier		= ESymbol.Message_Qualifier;
    Symbol k_Error		= ESymbol.Message_Error;
    Symbol k_SubError		= ESymbol.Message_SubError;
    /** Integer: statusOK, statusFail, statusWarn; or Boolean: isExc. */
    Symbol k_Status		= ESymbol.Message_Status;
    Symbol k_EventData		= ESymbol.Message_EventData;
    Symbol k_Payload		= ESymbol.Message_EventData;

    Symbol v_Null		= ESymbol.Any_Null;
}
