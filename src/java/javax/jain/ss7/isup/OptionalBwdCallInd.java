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

/** A class representing the ISUP Optional Backward Call Indicators parameter.
  * This class provides methods to access the sub-fields which are common to ITU and ANSI.
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract class OptionalBwdCallInd extends java.lang.Object implements java.io.Serializable {
    public static final boolean III_NO_INDICATION = false;
    public static final boolean III_INBAND_INFORMATION_OR_PATTERN_AVAILABLE = true;
    /** Constructs a new OptionalBwdCallInd class, parameters with default values.  */
    protected OptionalBwdCallInd() {
    }
    /** Constructs a OptionalBwdInd class from the input parameters specified.
      * @param in_inbanInfoInd  The in-band information indicator. <ul>
      * <li>III_NO_INDICATION and <li>III_INBAND_INFORMATION_OR_PATTERN_AVAILABLE. </ul>
      */
    protected OptionalBwdCallInd(boolean in_inbandInfoInd) {
        this.setInBandInfoInd(in_inbandInfoInd);
    }
    /** Gets the in-band Information Indicator field of the parameter.
      * @return Boolean, the InBandInformation Indicator value, see OptionalBwdCallInd().
      */
    public boolean getInBandInfoInd() {
        return m_ibi;
    }
    /** Sets the In Band Information Indicator field of the parameter.
      * @param aInBandInformationIndicator  Boolean, the InBand Information Indicator, see
      * OptionalBwdCallInd().
      */
    public void setInBandInfoInd(boolean aInBandInformationIndicator) {
        m_ibi = aInBandInformationIndicator;
    }
    /** The toString method retrieves a string containing the values of the members of the
      * OptionalBwdCallInd class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.OptionalBwdCallInd");
        b.append("\n\tm_ibi: " + m_ibi);
        return b.toString();
    }
    protected boolean m_ibi;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
