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

/** This parameter class represents a Location Area Id (LAI) value as defined in the specification 3G TS 23.003. <p>
  *
  * The following rules applies for the setting of parameter components:- <ul>
  * <li>Mandatory component with no default value must be provided to the constructor.
  * <li>Mandatory component with default value need not be set.
  * <li>Optional / conditional component is by default not present. Such a component
  * becomes present by setting it. </ul> <p>
  *
  * Parameter components:- <ul>
  * <li>mcc - Mobile Country Code, mandatory component with no default
  * <li>mnc - Mobile Network Code, mandatory component with no default
  * <li>lac - Location Area Code, mandatory component with no default </ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class LocationAreaId extends MapParameter {
    /** The constructor.
      * @param mcc  Mobile Country Code (3 digits)
      * @param mnc  Mobile Network Code (2-3 digits)
      * @param lac  Location Area Code (2 bytes)
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public LocationAreaId(String mcc, String mnc, byte[] lac)
        throws SS7InvalidParamException {
        setMobileCountryCode(mcc);
        setMobileNetworkCode(mnc);
        setLocationAreaCode(lac);
    }
    /** Empty constructor; needed for serializable objects and beans.
      */
    public LocationAreaId()
        throws SS7InvalidParamException {
    }
    /** Change the value of the Mobile Country Code (MCC).
      * @param mcc - String with 3 digit MCC.
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public void setMobileCountryCode(String mcc)
        throws SS7InvalidParamException {
        if (mcc.length() == 3) {
            m_mcc = mcc;
            return;
        }
        throw new SS7InvalidParamException("Mobile Country Code length " + mcc.length() + " invalid.");
    }
    /** Get the value of the Mobile Country Code (MCC).
      * @return String with 3 digit MCC.
      */
    public String getMobileCountryCode() {
        return m_mcc;
    }
    /** Change the value of the Mobile Network Code (MNC).
      * @param mnc  String with 2-3 digit MNC.
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public void setMobileNetworkCode(String mnc)
        throws SS7InvalidParamException {
        if (2 <= mnc.length() && mnc.length() <= 3) {
            m_mnc = mnc;
            return;
        }
        throw new SS7InvalidParamException("Mobile Network Code length " + mnc.length() + " is invalid.");
    }
    /** Get the value of the Mobile Network Code (MNC).
      * @return String with 2-3 digit MNC.
      */
    public String getMobileNetworkCode() {
        return m_mnc;
    }
    /** Change the value of the Location Area Code (LAC).
      * @param lac  Byte array with 2 byte LAC.
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public void setLocationAreaCode(byte[] lac)
        throws SS7InvalidParamException {
        if (lac.length == 2) {
            m_lac = lac;
            return;
        }
        throw new SS7InvalidParamException("Location Area Code length "  + lac.length + " is invalid.");
    }
    /** Get the value of the Location Area Code (LAC).
      * @return Byte array with 2 byte LAC.
      */
    public byte[] getLocationAreaCode() {
        return m_lac;
    }
    /** Get the whole Location Area Id (MCC+MNC+LAC) as a String.
      * @return String containing LAI.
      */
    public String toString() {
        StringBuffer b = new StringBuffer(512);
        b.append(super.toString());
        b.append("\njavax.jain.protocol.ss7.map.information.etsipar.LocationAreaId");
        b.append("\n\tm_mcc: " + m_mcc);
        b.append("\n\tm_mnc: " + m_mnc);
        b.append("\n\tm_lac: " + m_lac);
        return b.toString();
    }
    /** Compares this Location Area Id with another LAI.
      * @param lai  Location Area Id (LAI) to be compared with this LAI.
      * @return True if equal, otherwise false.
      */
    public boolean equals(Object lai) {
        if (lai instanceof LocationAreaId) {
            LocationAreaId other = (LocationAreaId) lai;
            if (other.m_mcc == m_mcc && other.m_mnc == m_mnc && other.m_lac == m_lac)
                return true;
        }
        return false;
    }
    protected String m_mcc = "";
    protected String m_mnc = "";
    protected byte[] m_lac;
}


// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
