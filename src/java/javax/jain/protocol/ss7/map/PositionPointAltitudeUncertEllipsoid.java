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

import javax.jain.protocol.ss7.*;
import javax.jain.*;

/**
  * This parameter represents an Ellipsoid Point (a point on the earth)
  * with altitude and uncertainty ellipsoid as defined in the
  * specification 3G TS 23.032 (Universal Geographical Area Description,
  * GAD). <p>
  *
  * This class represents the position information in a straight forward
  * manner (metres, degrees), and it is left to JAIN MAP API
  * implementations to translate between these values and the protocol
  * encoding. <p>
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
  *
  * <li>altitude (-32767 &lt;= metres &lt;= 32767), mandatory component
  * with no default
  *
  * <li>uncertaintyAltitude (0 &lt;= metres &lt;= 990,5; up or down),
  * mandatory component with no default </ul>
  */
public class PositionPointAltitudeUncertEllipsoid extends PositionPointUncertEllipse {
    /**
      * The constructor.
      *
      * @param latitude
      * Latitude in degrees (-90 &lt;= latitude &lt;= 90).
      *
      * @param longitude
      * Longitude in degrees (-180 &lt;= longitude &lt;= 180).
      *
      * @param uncertaintyMajorAxis
      * Length of major semi-axis of uncertainty ellipse in metres (0
      * &lt;= major semi-axis &lt;= 1800000).
      *
      * @param uncertaintyMinorAxis
      * Length of minor semi-axis of uncertainty ellipse in metres (0
      * &lt;= minor semi-axis &lt;= 1800000).
      *
      * @param orientationOfMajorAxis
      * Orientation of major axis; degrees clockwise from north (0 &lt;=
      * orientation &lt;= 179).
      *
      * @param altitude
      * Altitude in metres; up or down from the surface of the earth
      * (-32767 &lt;= altitude &lt;= 32767).
      *
      * @param uncertaintyAltitude
      * Uncertainty in the altitude, up or down from the altitude value
      * (0 &lt;= altitude uncertainty &lt;= 990,5).
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public PositionPointAltitudeUncertEllipsoid(float latitude,
            float longitude, float uncertaintyMajorAxis,
            float uncertaintyMinorAxis, float orientationOfMajorAxis,
            float altitude, float uncertaintyAltitude)
        throws SS7InvalidParamException {
        super(latitude,longitude,uncertaintyMajorAxis,uncertaintyMinorAxis,orientationOfMajorAxis);
        m_tos = TOS_POINT_ALTITUDE_UNCERT_ELLIPSOID;
        setAltitude(altitude);
        setUncertaintyAltitude(uncertaintyAltitude);
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public PositionPointAltitudeUncertEllipsoid()
        throws SS7InvalidParamException {
        super();
        m_tos = TOS_POINT_ALTITUDE_UNCERT_ELLIPSOID;
    }
    /**
      * Change the altitude parameter.
      *
      * @param altitude
      * Altitude in metres; up or down from the surface of the earth
      * (-32767 &lt;= altitude &lt;= 32767).
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setAltitude(float altitude)
        throws SS7InvalidParamException {
        if (-32767 <= altitude && altitude <= 32767) {
            m_altitude = altitude;
            m_altitudeIsSet = true;
            return;
        }
        throw new SS7InvalidParamException("Altitude " + altitude + " out of range.");
    }
    /**
      * Get the altitude parameter.
      *
      * @return
      * Altitude in metres; up or down from the surface of the earth
      * (-32767 &lt;= altitude &lt;= 32767).
      */
    public float getAltitude() {
        return m_altitude;
    }
    /**
      * Change the parameter specifying the uncertainty of the altitude.
      *
      * @param uncertaintyAltitude
      * Uncertainty in the altitude, up or down from the altitude value
      * (0 &lt;= altitude uncertainty &lt;= 990,5).
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setUncertaintyAltitude(float uncertaintyAltitude)
        throws SS7InvalidParamException {
        if (0 <= uncertaintyAltitude && uncertaintyAltitude <= 990.5) {
            m_uncertaintyAltitude = uncertaintyAltitude;
            m_uncertaintyAltitudeIsSet = true;
            return;
        }
        throw new SS7InvalidParamException("Uncertainty Altitude " + uncertaintyAltitude + " out of range.");
    }
    /**
      * Get the parameter specifying the uncertainty of the altitude.
      *
      * @return
      * Uncertainty in the altitude, up or down from the altitude value
      * (0 &lt;= altitude uncertainty &lt;= 990,5).
      */
    public float getUncertaintyAltitude() {
        return m_uncertaintyAltitude;
    }
    protected float m_altitude = 0;
    protected boolean m_altitudeIsSet = false;
    protected float m_uncertaintyAltitude = 0;
    protected boolean m_uncertaintyAltitudeIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-

