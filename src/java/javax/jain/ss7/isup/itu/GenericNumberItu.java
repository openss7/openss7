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

/** A class representing a Generic Number parameter.
  * It inherits from the core GenericNumber class to add the ITU only sub-fields. This
  * class provides the methods to access the ITU only fields.
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class GenericNumberItu extends GenericNumber {
    public static final boolean NII_NUMBER_COMPLETE = false;
    public static final boolean NII_NUMBER_INCOMPLETE = true;
    public static final byte SI_USER_PROVIDED_NOT_VERIFIED = 0;
    public static final byte SI_USER_PROVIDED_VERIFIED_PASSED = 1;
    public static final byte SI_USER_PROVIDED_VERIFIED_FAILED = 2;
    public static final byte SI_NETWORK_PROVIDED = 3;
    /** Constructs a new GenericNumberItu taking no parameters.
      * This constructor sets all the sub-field values to their spare value (which is
      * zero).
      */
    public GenericNumberItu() {
    }
    /** Constructor for GenericNumberItu.
      * @param numberQualifierIndicator  The number qualifier indicator, range 0-255, see
      * GenericNumber().
      * @param natureOfAddressIndicator  The nature of address indicator, see
      * NumberParameter().
      * @param numberingPlanIndicator  The numbering plan indicator, see
      * NumberParameter().
      * @param addressPresentationRestrictedIndicator  The address presentation restricted
      * indicator, see GenericNumber().
      * @param addressSignal  The address signal.
      * @param numberIncompleteIndicator  The number incomplete indicator. <ul>
      * <li>NII_NUMBER_COMPLETE and <li>NII_NUMBER_INCOMPLETE. </ul>
      * @param screeningIndicator  The screening indicator. <ul>
      * <li>SI_USER_PROVIDED_NOT_VERIFIED, <li>SI_USER_PROVIDED_VERIFIED_PASSED,
      * <li>SI_USER_PROVIDED_VERIFIED_FAILED and <li>SI_NETWORK_PROVIDED. </ul>
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public GenericNumberItu(short numberQualifierIndicator, byte natureOfAddressIndicator,
            byte numberingPlanIndicator, byte addressPresentationRestrictedIndicator,
            byte[] addressSignal, boolean numberIncompleteIndicator,
            byte screeningIndicator)
        throws ParameterRangeInvalidException {
        super(numberQualifierIndicator, natureOfAddressIndicator, numberingPlanIndicator,
                addressPresentationRestrictedIndicator, addressSignal);
        this.setNumberIncompleteInd(numberIncompleteIndicator);
        this.setScreeningInd(screeningIndicator);
    }
    /** Gets the Calling Party Number Incomplete Indicator field of the parameter.
      * @return The CallingParty Number Incomplete Indicator value, see GenericNumber().
      */
    public boolean getNumberIncompleteInd() {
        return m_numberIncompleteIndicator;
    }
    /** Sets the Calling Party Number Incomplete Indicator field of the parameter.
      * @param numberIncompleteInd  The CallingParty Number Incomplete Indicator value,
      * see GenericNumber().
      */
    public void setNumberIncompleteInd(boolean numberIncompleteIndicator) {
        m_numberIncompleteIndicator = numberIncompleteIndicator;
    }
    /** Gets the Screening Ind field of the parameter.
      * @return Screening indicator, range 0 to 3, see GenericNumber().
      */
    public byte getScreeningInd() {
        return m_screeningIndicator;
    }
    /** Sets the Screening Ind field of the parameter.
      * @param screeningIndicator  Screening indicator, range 0 to 3, see GenericNumber().
      * @return ParameterRangeInvalidException  Thrown when the value is out of range.
      */
    public void setScreeningInd(byte screeningIndicator)
        throws ParameterRangeInvalidException {
        if (0 <= screeningIndicator && screeningIndicator <= 3)
            m_screeningIndicator = screeningIndicator;
        throw new ParameterRangeInvalidException("ScreeningIndicator value " + screeningIndicator + " out of range.");
    }
    /** The toString method retrieves a string containing the values of the members of the
      * GenericNumberItu class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.itu.GenericNumberItu");
        b.append("\n\tm_numberIncompleteIndicator: " + m_numberIncompleteIndicator);
        b.append("\n\tm_screeningIndicator: " + m_screeningIndicator);
        return b.toString();
    }
    protected boolean m_numberIncompleteIndicator;
    protected byte m_screeningIndicator;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
