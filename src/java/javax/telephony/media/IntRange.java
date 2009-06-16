/*
 * IntRange.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Represents a closed interval delimited by upper and lower ints.
 * <p>
 * Some Resources have attributes and/or parameters that use
 * this range type.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public class IntRange implements java.io.Serializable {
    /** Field containing the lower bound. */
    public int lower; 
    /** Field containing the upper bound. */
    public int upper; 
    /**
     * Construct an IntRange, a closed interval
     * delimited by the [lower, upper] bounds.
     * @param lower the int at the lower bound of the range.
     * @param upper the int at the upper bound of the range.
     */
    public IntRange(int lower, int upper) {
	this.lower=lower;
	this.upper=upper;
    }
    public String toString() {
	return "IntRange["+lower+","+upper+"]";
    }
    
    public boolean equals( Object other ) {
        return ((other instanceof IntRange) &&
                (this.lower == ((IntRange)other).lower) &&
		(this.upper == ((IntRange)other).upper));
    }
}
