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

/** A class representing the itu ISUP MCIDReqIndItu parameter.
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class MCIDReqIndItu extends java.lang.Object implements java.io.Serializable {
    public static final boolean MRI_MCID_NOT_REQUESTED = false;
    public static final boolean MRI_MCID_REQUESTED = true;
    public static final boolean HI_HOLDING_NOT_REQUESTED = false;
    public static final boolean HI_HOLDING_REQUESTED = true;
    /** Constructs a new MCIDReqIndItu class, parameters with default values.  */
    public MCIDReqIndItu() {
    }
    /** Constructs a MCIDReqindItu class from the input parameters specified.
      * @param in_mcidReqInd  The MCID request indicator. <ul> <li>MRI_MCID_NOT_REQUESTED
      * and <li>MRI_MCID_REQUESTED. </ul>
      * @param in_holdingInd  The holding indicator. <ul> <li>HI_HOLDING_NOT_REQUESTED and
      * <li>HI_HOLDING_REQUESTED. </ul>
      */
    public MCIDReqIndItu(boolean in_mcidReqInd, boolean in_holdingInd) {
        this.setMCIDReqInd(in_mcidReqInd);
        this.setHoldingInd(in_holdingInd);
    }
    /** Gets the MCIDReqIndItu field of MCIDReqInd parameter
      * @return MCIDReqInd, see MCIDReqIndItu().
      */
    public boolean getMCIDReqInd() {
        return m_mcidReqInd;
    }
    /** Sets the MCIDReqInd field of MCIDReqInd parameter.
      * @param in_mcidReqInd  The MCIDReqInd value, see MCIDReqIndItu().
      */
    public void setMCIDReqInd(boolean in_mcidReqInd) {
        m_mcidReqInd = in_mcidReqInd;
    }
    /** Gets the HoldingInd field of MCIDReqInd parameter.
      * @return HoldingInd, see MCIDReqIndItu().
      */
    public boolean getHoldingInd() {
        return m_holdingInd;
    }
    /** Sets the HoldingInd field of MCIDReqInd parameter.
      * @param in_holdingInd  The HoldingInd value, see MCIDReqIndItu().
      */
    public void setHoldingInd(boolean in_holdingInd) {
        m_holdingInd = in_holdingInd;
    }
    /** The toString method retrieves a string containing the values of the members of the
      * MCIDReqIndItu class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.itu.MCIDReqIndItu");
        b.append("\n\tm_mcidReqInd: " + m_mcidReqInd);
        b.append("\n\tm_holdingInd: " + m_holdingInd);
        return b.toString();
    }
    protected boolean m_mcidReqInd;
    protected boolean m_holdingInd;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
