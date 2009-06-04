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

/**
  * This parameter class represents a Service Area Id (SAI) value as
  * defined in the specification 3G TS 23.003.
  * The following relationsship exists between SAI and Location Area Id
  * (LAI). <ul>
  * <li>SAI = LAI + SAC </ul>
  *
  * Therefore this class extends LocationAreaId class adding SAC
  * (Service Area Code). <p>
  *
  * The following rules applies for the setting of parameter components:
  * <ul>
  *
  * <li>Mandatory component with no default value must be provided to
  * the constructor.
  *
  * <li>Mandatory component with default value need not be set.
  *
  * <li>Optional / conditional component is by default not present. Such
  * a component becomes present by setting it. </ul>
  *
  * <h5>Parameter components:</h5><ul>
  * <li>sac - Service Area Code, mandatory component with no default
  * </ul>
  */
public class ServiceAreaId extends LocationAreaId {
    /**
      * The constructor.
      *
      * @param mcc
      * Mobile Country Code (3 digits)
      *
      * @param mnc
      * Mobile Network Code (2-3 digits)
      *
      * @param lac
      * Location Area Code (2 bytes)
      *
      * @param sac
      * Service Area Code (2 bytes)
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public ServiceAreaId(String mcc, String mnc, byte[] lac, byte[] sac)
        throws SS7InvalidParamException {
        super(mcc, mnc, lac);
        setServiceAreaCode(sac);
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public ServiceAreaId()
        throws SS7InvalidParamException {
    }
    /**
      * Change the value of the Service Area Code (SAC).
      *
      * @param sac
      * Byte array with 2 byte SAC.
      */
    public void setServiceAreaCode(byte[] sac) {
        m_sac = sac;
        m_sacIsSet = (sac != null);
    }
    /**
      * Get the value of the Service Area Code (SAC).
      *
      * @return
      * Byte array with 2 byte SAC.
      */
    public byte[] getServiceAreaCode() {
        return m_sac;
    }
    /**
      * Get the whole Service Area Code (MCC+MNC+LAC+SAC) as a String.
      *
      * @return
      * String containing SAC.
      */
    public String toString() {
        return new String("");
    }
    protected byte[] m_sac = null;
    protected boolean m_sacIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-

