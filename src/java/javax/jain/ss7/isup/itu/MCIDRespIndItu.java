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

/** A class representing the ISUP MCIDRespIndItu parameter.
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class MCIDRespIndItu extends java.lang.Object implements java.io.Serializable {
    public static final boolean MRI_MCID_NOT_INCLUDED = false;
    public static final boolean MRI_MCID_INCLUDED = true;
    public static final boolean HPI_HOLDING_NOT_RROVIDED = false;
    public static final boolean HPI_HOLDING_PROVIDED = true;
    /** Constructs a new MCIDRespIndItu class, parameters with default values.  */
    public MCIDRespIndItu() {
    }
    /** Constructs a MCIDRespindItu class from the input parameters specified.
      * @param in_mcidRespInd  The MCID response indicator. <ul> <li>MRI_MCID_NOT_INCLUDED
      * and <li>MRI_MCID_INCLUDED. </ul>
      * @param in_holdProvidedInd  The holding indicator. <ul>
      * <li>HPI_HOLDING_NOT_RROVIDED and <li>HPI_HOLDING_PROVIDED. </ul>
      */
    public MCIDRespIndItu(boolean in_mcidRespInd, boolean in_holdProvidedInd) {
        this.setMCIDRespInd(in_mcidRespInd);
        this.setHoldProvidedInd(in_holdProvidedInd);
    }
    /** Gets the MCIDRespInd field of MCIDRespInd parameter.
      * @return MCIDRespInd value, see MCIDRespIndItu().
      */
    public boolean getMCIDRespInd() {
        return m_mcidRespInd;
    }
    /** Sets the MCIDRespInd field of MCIDRespInd parameter.
      * @param in_mcidRespInd  The MCIDRespInd value, see MCIDRespIndItu().
      */
    public void setMCIDRespInd(boolean in_mcidRespInd) {
        m_mcidRespInd = in_mcidRespInd;
    }
    /** Gets the HoldProvidedInd field of MCIDRespInd parameter.
      * @return HoldProvidedInd value, see MCIDRespIndItu().
      */
    public boolean getHoldProvidedInd() {
        return m_holdProvidedInd;
    }
    /** Sets the HoldProvidedInd field of MCIDRespInd parameter.
      * @param in_holdProvidedInd  The HoldProvidedInd value, see MCIDRespIndItu().
      */
    public void setHoldProvidedInd(boolean in_holdProvidedInd) {
        m_holdProvidedInd = in_holdProvidedInd;
    }
    /** The toString method retrieves a string containing the values of the members of the
      * MCIDRespIndItu class.
      * @return A string representation of the member variables.
      */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.itu.MCIDRespIndItu");
        b.append ("\n\tm_mcidRespInd: " + m_mcidRespInd);
        b.append ("\n\tm_holdProvidedInd: " + m_holdProvidedInd);
        return b.toString();
    }
    protected boolean m_mcidRespInd;
    protected boolean m_holdProvidedInd;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
