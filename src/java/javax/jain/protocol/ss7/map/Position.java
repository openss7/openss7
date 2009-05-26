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

package javax.jain.protocol.ss7.map;

import javax.jain.protocol.ss7.SS7InvalidParamException;

/**
  * This class is a base class of different classes with position information.
  * <p>
  *
  * The following rules apply when setting parameter components: <p>
  *
  * <li><b>Mandatory component</b> with <b>no default value</b> must be provided to the constructor.
  * <li><b>Mandatory component</b> with <b>default value</b> need not be set.
  * <li><b>Optional/conditional component</b> is by default not present.  Such a component becomes present by setting it.
  * <p>
  *
  * <b>Parameter components:</b>
  * <li><var>ageOfPositionInfo</var>, conditional component, present if available.
  */
public abstract class Position extends MapParameter {
    /** Type of Shape (TOS) of location information: point without uncertainty information. */
    public static final int TOS_POINT = 1;
    /** Type of Shape (TOS) of location information: point with uncertainty circle. */
    public static final int TOS_POINT_UNCERT_CIRCLE = 2;
    /** Type of Shape (TOS) of location information: point with uncertainty ellipse. */
    public static final int TOS_POINT_UNCERT_ELLIPSE = 3;
    /** Type of Shape (TOS) of location information: polygon. */
    public static final int TOS_POLYGON = 4;
    /** Type of Shape (TOS) of location information: point with altitude and no uncertainty information. */
    public static final int TOS_POINT_ALTITUDE = 5;
    /** Type of Shape (TOS) of location information: point with altitude and uncertainty ellipsoid. */
    public static final int TOS_POINT_ALTITUDE_UNCERT_ELLIPSOID = 6;
    /** Type of Shape (TOS) of location information: Arc */
    public static final int TOS_ARC = 7;

    /** The constructor.
      * @exception SS7InvalidParamException Thrown if parameters are invalid or out of range.  */
    public Position() throws SS7InvalidParamException {
        this();
        this.setAgeOfPositionInfo();
    }
    /** Set the Age of Position information.
      * @param ageOfPositionInfo Age of position information.  */
    public void setAgeOfPositionInfo(Data ageOfPositionInfo) {
        Date date = Date.now();
        if (ageOfPositionInfo != null)
            m_age = ageOfPositionInfo;
        m_age_is_set = true;
    }
    /** Get the Age of Position information.
      * @return Age of position information.  */
    public Date getAgeOfPositionInfo() {
        Date date = Date.now();
        if (m_age_is_set == true)
            date = m_age;
        return date;
    }
    /** Check if the Age of Position Information is present.
      * @return True or false.  */
    public boolean isAgeOfPositionInfoPresent() {
        return m_age_is_set;
    }

    private int m_tos = TOS_POINT;
    private boolean m_tos_is_set = false;
    private Date m_age;
    private boolean m_age_is_set = false;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
