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

import java.util.Date;
import javax.jain.protocol.ss7.map.information.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/** This parameter specifies location information requested for a Mobile Station. The
  * content included in this parameter is based on the specification 3G TS 23.078 (CAMEL
  * phase 3, stage 2). <p>
  *
  * The following rules applies for the setting of parameter components:- <ul>
  * <li>Mandatory component with no default value must be provided to the constructor.
  * <li>Mandatory component with default value need not be set.
  * <li>Optional /conditional component is by default not present. Such a component
  * becomes present by setting it. </ul> <p>
  *
  * Parameter components:- <ul>
  * <li>locationNumber, conditional component (specified in 3G TS 23.003)
  * <li>locationAreaId or serviceAreaId or cellGlobalId, conditional component (specified
  * in 3G TS 23.003)
  * <li>position ("Geographical Information"), conditional component (specified in 3G TS
  * 23.032)
  * <li>vlrNumber, conditional component (specified in 3G TS 23.003)
  * <li>localisedServiceAreaId, conditional component (specified in 3G TS 23.003)
  * <li>ageOfLocationInfo, conditional component
  * <li>(geodeticInfo, conditional component, not included in this API version; no
  * specification found) </ul> <p>
  *
  * The conditions for presence of these parameter components are not defined in this
  * specification.
  */
public class Location extends MapParameter {
    /** The constructor.
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public Location()
        throws SS7InvalidParamException {
    }
    /** Set the Location Number.
      * @param locationNumber  Location Number.
      */
    public void setLocationNumber(LocationNumber locationNumber) {
        m_locationNumber = locationNumber;
    }
    /** Get the Location Number. It shall be checked if this parameter component is
      * present before getting it.
      * @return Location Number.
      */
    public LocationNumber getLocationNumber() {
        return m_locationNumber;
    }
    /** Check if the Location Number is present.
      * @return True or false.
      */
    public boolean isLocationNumberPresent() {
        return (m_locationNumber != null);
    }
    /** Set the Location Area Id (LAI) or Service Area Id (SAI) or Cell Global Id (CGI).
      * SAI and CGI are represented by subclasses of LAI.
      * @param locationAreaId  LAI or SAI or CGI.
      */
    public void setLocationAreaId(LocationAreaId locationAreaId) {
        m_locationAreaId = locationAreaId;
    }
    /** Get the Locaion Area Id (LAI) or Service Area Id (SAI) or Cell Global Id (CGI).
      * SAI and CGI are represented by subclasses of LAI.Whether it is LAI, SAI or CGI has
      * to be tested using the instanceof() operator. It shall be checked if this
      * parameter component is present before getting it.
      * @return LAI or SAI or CGI.
      */
    public LocationAreaId getLocationAreaId() {
        return m_locationAreaId;
    }
    /** Check if the Locaion Area Id (LAI) or Service Area Id (SAI) or Cell Global Id
      * (CGI) is present. SAI and CGI are represented by subclasses of LAI.
      * @return True or false.
      */
    public boolean isLocationAreaIdPresent() {
        return (m_locationAreaId != null);
    }
    /** Set the Position (called "Geographical Information" in the specifications).
      * @param position  Position.
      */
    public void setPosition(Position position) {
        m_position = position;
    }
    /** Get the Position (called "Geographical Information" in the specifications). It
      * shall be checked if this parameter component is present before getting it.
      * @return Position.
      */
    public Position getPosition() {
        return m_position;
    }
    /** Check if the Position (called "Geographical Information" in the specifications) is
      * present.
      * @return True or false.
      */
    public boolean isPositionPresent() {
        return (m_position != null);
    }
    /** Set the VLR Number.
      * @param vlrNumber  VLR Number.
      */
    public void setVlrNumber(VlrNumber vlrNumber) {
        m_vlrNumber = vlrNumber;
    }
    /** Get the VLR Number. It shall be checked if this parameter component is present
      * before getting it.
      * @return VLR Number.
      */
    public VlrNumber getVlrNumber() {
        return m_vlrNumber;
    }
    /** Check if the VLR Number is present.
      * @return True or false.
      */
    public boolean isVlrNumberPresent() {
        return (m_vlrNumber != null);
    }
    /** Set the Localised Service Area Id (LSA ID).
      * @param lsaId  LSA ID.
      */
    public void setLsaId(LsaId lsaId) {
        m_lsaId = lsaId;
    }
    /** Get the Localised Service Area Id (LSA ID). It shall be checked if this parameter
      * component is present before getting it.
      * @return LSA ID.
      */
    public LsaId getLsaId() {
        return m_lsaId;
    }
    /** Check if the Localised Service Area Id (LSA ID) is present.
      * @return True or false.
      */
    public boolean isLsaIdPresent() {
        return (m_lsaId != null);
    }
    /** Set the Age of Location Information.
      * @param ageOfLocationInfo  Age of Location Information.
      */
    public void setAgeOfLocationInfo(Date ageOfLocationInfo) {
        m_ageOfLocationInfo = ageOfLocationInfo;
    }
    /** Get the Age of Location Information.
      * @return Age of Location Information.
      */
    public Date getAgeOfLocationInfo() {
        return m_ageOfLocationInfo;
    }
    /** Check if the Age of Location Information is present.  
      * @return True or false.
      */
    public boolean isAgeOfLocationInfoPresent() {
        return (m_ageOfLocationInfo != null);
    }
    protected LocationNumber m_locationNumber = null;
    protected LocationAreaId m_locationAreaId = null;
    protected Position m_position = null;
    protected VlrNumber m_vlrNumber = null;
    protected LsaId m_lsaId = null;
    protected Date m_ageOfLocationInfo = null;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
