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

package javax.jain.ss7.isup;

import javax.jain.ss7.*;
import javax.jain.*;

/** A class representing a CalledParty Number.
  * This class contains all the subfields common to ITU and ANSI variants. The ANSI
  * variant has no other sub-fields and is instantiated by this class. The ITU variant
  * inherits from this class and adds the ITU specific sub-fields to this class.
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class CalledPartyNumber extends NumberParameter {
    /** Constructs a new CalledPartyNumber taking no parameters.
      * This constructor sets all the sub-field values to their spare value (which is
      * zero). There is nothing set in this constructor as all the sub-fields are also
      * that of the parent, therefore these fields will get set when the parent's
      * constructor is automatically called.  */
    public CalledPartyNumber() {
    }
    /** Constructs a new Called Party Number.
      * @param natureOfAddressIndicator  The nature of address indicator, see
      * NumberParameter().
      * @param numberingPlanIndicator  The numbering plan indicator, see
      * NumberParameter().
      * @param addressSignal  The address signal.
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.  */
    public CalledPartyNumber(byte natureOfAddressIndicator,
            byte numberingPlanIndicator, byte[] addressSignal)
        throws ParameterRangeInvalidException {
        super(natureOfAddressIndicator, numberingPlanIndicator, addressSignal);
    }
    /** The toString method retrieves a string containing the values of the members of
      * the CalledPartyNumber class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.CalledPartyNumber:");
        return b.toString();
    }
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
