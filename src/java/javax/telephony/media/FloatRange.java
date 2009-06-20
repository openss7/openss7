/*
 * FloatRange.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

import java.io.Serializable;

/**
 * Represents a closed interval delimited by upper and lower floats.
 * <p>
 * Some Resources have attributes and/or parameters that use
 * this range type.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public class FloatRange implements java.io.Serializable {
    /** Field containing the lower bound. */
    public float lower;
    /** Field containing the upper bound. */
    public float upper;

    /**
     * Constructs a FloatRange object.
     *
     * @param lower  a float representing lower limit of range
     * @param upper  a float representing upper limit of range
     */
    public FloatRange(float lower, float upper) {
	this.lower = lower;
	this.upper = upper;
    }
    public java.lang.String toString() {
	return "FloatRange["+lower+","+upper+"]";
    }

    public boolean equals( java.lang.Object other ) {
        return ((other instanceof FloatRange) &&
                (this.lower == ((FloatRange)other).lower) &&
		(this.upper == ((FloatRange)other).upper));
    }
}
