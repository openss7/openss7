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

/** A class representing an ISUP User To User Indicators.
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class UserToUserIndicators implements java.io.Serializable {
    public static final boolean TYPE_REQUEST = false;
    public static final boolean TYPE_RESPONSE = true;
    public static final boolean NDI_NO_INFORMATION = false;
    public static final boolean NDI_USER_TO_USER_INFORMATION_DISCARDED_BY_NETWORK = true;
    /** Constructs a new UserToUserIndicators class, parameters with default values.  */
    public UserToUserIndicators() {
    }
    /** Constructs a UserToUserIndicators class from the input parameters specified.
      * @param in_type  The type; <ul> <li>TYPE_REQUEST and <li>TYPE_RESPONSE. </ul>
      * @param in_nwDiscardInd  The network discard indicator; <ul>
      * <li>NDI_NO_INFORMATION and
      * <li>NDI_USER_TO_USER_INFORMATION_DISCARDED_BY_NETWORK. </ul> */
    public UserToUserIndicators(boolean in_type, boolean in_nwDiscardInd) {
        this();
        this.setType(in_type);
        this.setNwDiscardInd(in_nwDiscardInd);
    }
    /** Gets the Type field of the parameter.
      * Type indicates whether the User To User Indicators are carrying a request or a
      * response.
      * @return The Type value, see UserToUserIndicators(). */
    public boolean getType() {
        return m_type;
    }
    /** Sets the Type field of the parameter.
      * Type indicates whether the User To User Indicators are carrying a request or a
      * response.
      * @param aType  The Type value, see UserToUserIndicators().  */
    public void setType(boolean aType) {
        m_type = aType;
    }
    /** Gets the network discard indicator.
      * @return The network discard indicator, see UserToUserIndicators(). */
    public boolean getNwDiscardInd() {
        return m_nwDiscardInd;
    }
    /** Sets the network discard indicator.
      * @param aNwDiscardInd  The network discard indicator, see
      * UserToUserIndicators().  */
    public void setNwDiscardInd(boolean aNwDiscardInd) {
        m_nwDiscardInd = aNwDiscardInd;
    }
    /** The toString method retrieves a string containing the values of the members of
      * the UserToUserIndicators class.
      * @return A string representation of the member variables.  */
    public java.lang.String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.ss7.isup.UserToUserIndicators");
        b.append("\n\tm_type: " + m_type);
        b.append("\n\tm_nwDiscardInd: " + m_nwDiscardInd);
        return b.toString();
    }
    protected boolean m_nwDiscardInd;
    protected boolean m_type;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
