//  ==========================================================================
//  
//  @(#) $Id$
//  
//  --------------------------------------------------------------------------
//  
//  Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
//  Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
//  Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
//  
//  All Rights Reserved.
//  
//  This program is free software; you can redistribute it and/or modify it
//  under the terms of the GNU Affero General Public License as published by
//  the Free Software Foundation; version 3 of the License.
//  
//  This program is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
//  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public
//  License for more details.
//  
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>, or
//  write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
//  02139, USA.
//  
//  --------------------------------------------------------------------------
//  
//  U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
//  behalf of the U.S. Government ("Government"), the following provisions
//  apply to you.  If the Software is supplied by the Department of Defense
//  ("DoD"), it is classified as "Commercial Computer Software" under
//  paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
//  Regulations ("DFARS") (or any successor regulations) and the Government is
//  acquiring only the license rights granted herein (the license rights
//  customarily provided to non-Government users).  If the Software is
//  supplied to any unit or agency of the Government other than DoD, it is
//  classified as "Restricted Computer Software" and the Government's rights
//  in the Software are defined in paragraph 52.227-19 of the Federal
//  Acquisition Regulations ("FAR") (or any successor regulations) or, in the
//  cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
//  (or any successor regulations).
//  
//  --------------------------------------------------------------------------
//  
//  Commercial licensing and support of this software is available from
//  OpenSS7 Corporation at a fee.  See http://www.openss7.com/
//  
//  --------------------------------------------------------------------------
//  
//  Last Modified $Date$ by $Author$
//  
//  --------------------------------------------------------------------------
//  
//  $Log$
//  ==========================================================================

package javax.jain.protocol.ss7.map.information.etsipar;

import javax.jain.protocol.ss7.map.information.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/** This parameter class represents a Localised Service Area Id (LSA ID) value as defined
  * in the specification 3G TS 23.003. <p>
  *
  * The following rules applies for the setting of parameter components:- <ul>
  * <li>Mandatory component with no default value must be provided to the constructor.
  * <li>Mandatory component with default value need not be set.
  * <li>Optional / conditional component is by default not present. Such a component
  * becomes present by setting it. </ul> <p>
  *
  * Parameter components:- <ul>
  * <li>lsaId - Localised Service Area Id, mandatory component with no default </ul>
  */
public class LsaId extends MapParameter {
    /** The constructor.
      * @param lsaId  Localised Service Area Id (3 bytes)
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of range.
      */
    public LsaId(byte[] lsaId)
        throws SS7InvalidParamException {
        setLsaId(lsaId);
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public LsaId()
        throws SS7InvalidParamException {
    }
    /** Change the value of the Localised Service Area Id (LSA ID).
      * @param lsaId  Byte array with 3 byte LSA ID.
      */
    public void setLsaId(byte[] lsaId) {
        m_lsaId = lsaId;
    }
    /** Get the value of the Localised Service Area Id (LSA ID).
      * @return Byte array with 3 byte LSA ID.
      */
    public byte[] getLsaId() {
        return m_lsaId;
    }
    /** Check if the Localised Service Area Id (LSA ID) is universal or PLMN significant.
      * This is indicated by least significant bit in the LSA ID value: 1 = universal, 0 =
      * PLMN significant
      * @return True if LSA ID is universal, otherwise false.
      */
    public boolean isLsaIdUniversal() {
        if (m_lsaId != null) {
            return ((m_lsaId[0] & 0x01) != 0);
        }
        return false;
    }
    protected byte[] m_lsaId = null;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
