/*
 * MediaConstants.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

package javax.telephony.media;

/**
 * Defines constants used by many components.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public interface MediaConstants {
    /**
     * Symbol returned from {@link ResourceEvent#getError()} 
     * if there is no error.
     */
    Symbol e_OK				= ESymbol.Error_OK;

    /**
     * Symbol returned from {@link ResourceEvent#getQualifier()}
     * if an operation completed because the requested duration
     * was reached.
     */
    Symbol q_Duration 			= ESymbol.Any_Duration;

    /**
     * Qualifier: normal, default completion.
     */
    Symbol q_Standard 			= ESymbol.Any_Standard;
    
    /**
     * Qualifier: Completion caused by a Stop.
     * Normal response from operations stopped by MediaService.stop() method.
     */
    Symbol q_Stop 			= ESymbol.Any_Stop;
}
