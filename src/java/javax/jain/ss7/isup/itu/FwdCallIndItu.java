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

/** A class representing an ITU Forward Call Indicator.
  * This class provides methods to access the ITU only sub-fields of Forward Call
  * Indicator parameter.
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class FwdCallIndItu extends FwdCallInd {
    public static final boolean EII_NO_END_TO_END_INFORMATION_AVAILABLE = false;
    public static final boolean EII_END_TO_END_INFORMATION_AVAILABLE = true;
    /** Constructs a new FwdCallInd taking no parameters.
      * This constructor sets all the sub-field values to their spare value (which is
      * zero).
      */
    public FwdCallIndItu() {
    }
    /** Constructs a new FwdCallIndItu with the parameters specified.
      * @param endToEndMethodIndicator  The end to end method indicator, range 0-3, see
      * FwdCallInd().
      * @param interworkingIndicator  The interworking indicator, see FwdCallInd().
      * @param isdnUserPartIndicator  The ISDN User part indicator, see FwdCallInd().
      * @param isdnUserPartPreferenceIndicator  The ISDN User part preference indicator,
      * range 0-3, see FwdCallInd().
      * @param isdnAccessIndicator  The ISDN access indicator, see FwdCallInd().
      * @param sccpMethodIndicator  The SCCP method indicator, range 0-3, see
      * FwdCallInd().
      * @param typeOfCallIndicator  The national/international call indicator, see
      * FwdCallInd().
      * @param endToEndInformationIndicator  The end to end Information indicator. <ul>
      * <li>EII_NO_END_TO_END_INFORMATION_AVAILABLE and
      * <li>EII_END_TO_END_INFORMATION_AVAILABLE. </ul>
      * @exception ParameterRangeInvalidException  Thrown when value is out of range.
      */
    public FwdCallIndItu(byte endToEndMethodIndicator, boolean interworkingIndicator,
            boolean isdnUserPartIndicator, byte isdnUserPartPreferenceIndicator,
            boolean isdnAccessIndicator, byte sccpMethodIndicator,
            boolean typeOfCallIndicator, boolean endToEndInformationIndicator)
        throws ParameterRangeInvalidException {
        super(endToEndMethodIndicator, interworkingIndicator, isdnUserPartIndicator,
                isdnUserPartPreferenceIndicator, isdnAccessIndicator, sccpMethodIndicator,
                typeOfCallIndicator);
        this.setEndToEndInfoInd(endToEndInformationIndicator);
    }
    /** Gets the End To End Information Indicator field of the parameter.
      * @return Boolean, the End To End Information value, see FwdCallIndItu().
      */
    public boolean getEndToEndInfoInd() {
        return m_endToEndInformationIndicator;
    }
    /** Sets the End To End Information Indicator field of the parameter.
      * @param endToEndInformationIndicator  The ISDN Access Indicator value, see
      * FwdCallIndItu().
      */
    public void setEndToEndInfoInd(boolean endToEndInformationIndicator) {
        m_endToEndInformationIndicator = endToEndInformationIndicator;
    }
    /** The toString method retrieves a string containing the values of the members of the
      * FwdCallIndItu class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.itu.FwdCallIndItu");
        b.append("\n\tm_endToEndInformationIndicator: " + m_endToEndInformationIndicator);
        return b.toString();
    }
    protected boolean m_endToEndInformationIndicator;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
