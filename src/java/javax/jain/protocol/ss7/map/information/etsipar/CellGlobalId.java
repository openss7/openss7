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
  * This parameter class represents a Cell Global Id (CGI) value as
  * defined in the specification 3G TS 23.003.
  * The following relationsships exists between CGI, Location Area Id
  * (LAI) and Routing Area Id (RAI). <ul>
  * <li>CGI = LAI + CI or
  * <li>CGI = RAI + CI and
  * <li>RAI = LAI + RAC. </ul>
  *
  * Therefore this class extends LocationAreaId class adding RAC
  * (Routing Area Code) and CI (Cell Id). RAC is optional. <p>
  *
  * The following rules applies for the setting of parameter components:
  * <ul>
  *
  * <li>Mandatory component with no default value must be provided to
  * the constructor.
  * <li>Mandatory component with default value need not be set.
  * <li>Optional / conditional component is by default not present. Such
  * a component becomes present by setting it. </ul>
  *
  * <h5>Parameter components:</h5> <ul>
  * <li>rac - Routing Area Code, optional component
  * <li>ci - Cell ID, mandatory component with no default </ul>
  */
public class CellGlobalId extends LocationAreaId {
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
      * @param ci
      * Cell ID (2 bytes)
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public CellGlobalId(String mcc, String mnc, byte[] lac, byte[] ci)
        throws SS7InvalidParamException {
        super(mcc, mnc, lac);
        setCellId(ci);
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public CellGlobalId()
        throws SS7InvalidParamException {
    }
    /**
      * Change the value of the Cell Id (CI).
      *
      * @param ci
      * Byte array with 2 byte CI.
      */
    public void setCellId(byte[] ci) {
        m_ci = ci;
        m_ciIsSet = (ci != null);
    }
    /**
      * Get the value of the Cell Id (CI).
      *
      * @return
      * Byte array with 2 byte CI.
      */
    public byte[] getCellId() {
        return m_ci;
    }
    /**
      * Set the value of the Routing Area Code (RAC).
      *
      * @param rac
      * Byte array with 1 byte RAC.
      */
    public void setRoutingAreaCode(byte[] rac) {
        m_rac = rac;
        m_racIsSet = (rac != null);
    }
    /**
      * Get the value of the Routing Area Code (RAC).
      * It shall be checked if this parameter component is present
      * before getting it.
      *
      * @return
      * Byte array with 1 byte RAC.
      */
    public byte[] getRoutingAreaCode() {
        return m_rac;
    }
    /**
      * Check if Routing Area Code is present.
      *
      * @return
      * True or false.
      */
    public boolean isRoutingAreaCodePresent() {
        return m_racIsSet;
    }
    /**
      * Get the whole Cell Global Id (MCC+MNC+LAC+RAC+CI) as a String.
      *
      * @return
      * String containing CGI.
      */
    public String toString() {
        return new String("");
    }
    protected byte[] m_ci = null;
    protected boolean m_ciIsSet = false;
    protected byte[] m_rac = null;
    protected boolean m_racIsSet = false;
}


// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-

