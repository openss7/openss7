/* ***************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

package javax.jain.ss7.isup.itu;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing a Calling Party Number for ITU variant.
    It inherits from the core class for calling party number and the adds the ITU specific
    sub-fields in the parameter.
    @author Monavacon Limited
    @version 1.2.2
  */
public class CallingPartyNumberItu extends CallingPartyNumber {
    public static final boolean CPNII_COMPLETE = false;
    public static final boolean CPNII_INCOMPLETE = true;
    /** Constructs a new CallingPartyNumberItu taking no parameters.
        This constructor sets all the sub-field values to their spare value (which is
        zero).
      */
    public CallingPartyNumberItu() {
    }
    /** Constructs a new CallingPartyNumberItu.
        @param natureOfAddressIndicator  The nature of address indicator, range 0-127, see
        NumberParameter().
        @param numberingPlanIndicator  The numbering plan indicator, range 0-7, see
        NumberParameter().
        @param addressPresentationRestrictedIndicator  The address presentation restricted
        indicator, range 0-3, see CallingPartyNumber().
        @param screeningIndicator  The screening indicator, range 0-3, see
        CallingPartyNumber().
        @param addressSignal  The address signal.
        @param numberIncompleteIndicator  The number Incomplete Indicator. <ul>
        <li>CPNII_COMPLETE and <li>CPNII_INCOMPLETE. </ul>
        @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public CallingPartyNumberItu(byte natureOfAddressIndicator,
            byte numberingPlanIndicator, byte addressPresentationRestrictedIndicator,
            byte screeningIndicator, byte[] addressSignal,
            boolean numberIncompleteIndicator)
        throws ParameterRangeInvalidException {
        super(natureOfAddressIndicator, numberingPlanIndicator,
                addressPresentationRestrictedIndicator, screeningIndicator,
                addressSignal);
        this.setNumberIncompleteInd(numberIncompleteIndicator);
    }
    /** Gets the Calling Party Number Incomplete Indicator field of the parameter.
        @return The CallingParty Number Incomplete Indicator value, see
        CallingPartyNumberItu()
      */
    public boolean getNumberIncompleteInd() {
        return m_numberIncompleteIndicator;
    }
    /** Sets the Calling Party Number Incomplete Indicator field of the parameter.
        @param numberIncompleteInd  The CallingParty Number Incomplete Indicator value,
        see CallingPartyNumberItu().
      */
    public void setNumberIncompleteInd(boolean numberIncompleteIndicator) {
        m_numberIncompleteIndicator = numberIncompleteIndicator;
    }
    /** The toString method retrieves a string containing the values of the members of the
        CallingPartyNumberItu class.
        @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.itu.CallingPartyNumberItu");
        b.append("\n\tm_numberIncompleteIndicator: " + m_numberIncompleteIndicator);
        return b.toString();
    }
    protected boolean m_numberIncompleteIndicator;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
