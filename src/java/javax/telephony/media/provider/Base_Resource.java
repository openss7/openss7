/*
 * Base_Resource.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

package javax.telephony.media.provider;

import javax.telephony.media.*;
import javax.telephony.media.async.*;

/** a Generic Resource.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */

public class Base_Resource implements MPI.Resource {
    /** the owning MediaService object; where to send nonTransEvents */
    protected MPI.Owner owner;

    /** This is set when a Resource is allocated and bound. 
     * The Owner is used to direct Events to the application.
     */
    public void setOwner(MPI.Owner owner) { this.owner = owner; } 
}
