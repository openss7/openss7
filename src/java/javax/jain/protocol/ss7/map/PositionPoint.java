/*
 @(#) src/java/javax/jain/protocol/ss7/map/PositionPoint.java <p>
 
 Copyright &copy; 2008-2015  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
 Copyright &copy; 2001-2008  OpenSS7 Corporation <a href="http://www.openss7.com/">&lt;http://www.openss7.com/&gt;</a>. <br>
 Copyright &copy; 1997-2001  Brian F. G. Bidulock <a href="mailto:bidulock@openss7.org">&lt;bidulock@openss7.org&gt;</a>. <p>
 
 All Rights Reserved. <p>
 
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license. <p>
 
 This program is distributed in the hope that it will be useful, but <b>WITHOUT
 ANY WARRANTY</b>; without even the implied warranty of <b>MERCHANTABILITY</b>
 or <b>FITNESS FOR A PARTICULAR PURPOSE</b>.  See the GNU Affero General Public
 License for more details. <p>
 
 You should have received a copy of the GNU Affero General Public License along
 with this program.  If not, see
 <a href="http://www.gnu.org/licenses/">&lt;http://www.gnu.org/licenses/&gt</a>,
 or write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA. <p>
 
 <em>U.S. GOVERNMENT RESTRICTED RIGHTS</em>.  If you are licensing this
 Software on behalf of the U.S. Government ("Government"), the following
 provisions apply to you.  If the Software is supplied by the Department of
 Defense ("DoD"), it is classified as "Commercial Computer Software" under
 paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
 Regulations ("DFARS") (or any successor regulations) and the Government is
 acquiring only the license rights granted herein (the license rights
 customarily provided to non-Government users).  If the Software is supplied to
 any unit or agency of the Government other than DoD, it is classified as
 "Restricted Computer Software" and the Government's rights in the Software are
 defined in paragraph 52.227-19 of the Federal Acquisition Regulations ("FAR")
 (or any successor regulations) or, in the cases of NASA, in paragraph
 18.52.227-86 of the NASA Supplement to the FAR (or any successor regulations). <p>
 
 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See
 <a href="http://www.openss7.com/">http://www.openss7.com/</a>
 */

package javax.jain.protocol.ss7.map;

import javax.jain.protocol.ss7.*;
import javax.jain.*;

/**
  * This parameter represents an Ellipsoid Point (a point on the earth)
  * as defined in the specification 3G TS 23.032 (Universal Geographical
  * Area Description, GAD). <p>
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
  * <h5>Parameter components:</h5> <ul>
  *
  * <li>typeOfShape, read-only
  *
  * <li>latitude (-90 &le; degrees &le; 90), mandatory component with
  * no default,
  *
  * <li>longitude (-180 &le; degrees &le; 180), mandatory component
  * with no default, </ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class PositionPoint extends Position {
    /**
      * The constructor.
      *
      * @param latitude
      * Latitude in degrees (-90 &le; latitude &le; 90).
      *
      * @param longitude
      * Longitude in degrees (-180 &le; longitude &le; 180).
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public PositionPoint(float latitude, float longitude)
        throws SS7InvalidParamException {
        setLatitude(latitude);
        setLongitude(longitude);
        m_tos = TOS_POINT;
        m_tos_is_set = true;
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public PositionPoint()
        throws SS7InvalidParamException {
        m_tos = TOS_POINT;
        m_tos_is_set = true;
    }
    /**
      * Get the Type Of Shape identifier for position information.
      * Testing on this parameter is an alternative to testing the class
      * using the instanceof operator.
      *
      * @return
      * Integer indicating Type Of Shape. Possible values: <ul>
      * <li>TOS_POINT
      * <li>TOS_POINT_UNCERT_CIRCLE
      * <li>TOS_POINT_UNCERT_ELLIPSE
      * <li>TOS_POLYGON (not supported by this version of the API)
      * <li>TOS_POINT_ALTITUDE
      * <li>TOS_POINT_ALTITUDE_UNCERT_ELLIPSOID
      * <li>TOS_ARC </ul>
      */
    public int getTypeOfShape() {
        return m_tos;
    }
    /**
      * Change the latitude parameter.
      *
      * @param latitude
      * Latitude in degrees (-90 &le; latitude &le; 90).
      * 
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setLatitude(float latitude)
        throws SS7InvalidParamException {
        if (-90 <= latitude && latitude <= 90) {
            m_latitude = latitude;
            m_latitudeIsSet = true;
            return;
        }
        throw new SS7InvalidParamException("Latitude " + latitude + " out of range.");
    }
    /**
      * Get the latitude parameter.
      *
      * @return
      * Latitude in degrees (-90 &le; latitude &le; 90).
      */
    public float getLatitude() {
        return m_latitude;
    }
    /**
      * Change the longitude parameter.
      *
      * @param longitude
      * Longitude in degrees (-180 &le; longitude &le; 180).
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public void setLongitude(float longitude)
        throws SS7InvalidParamException {
        if (-180 <= longitude && longitude <= 180) {
            m_longitude = longitude;
            m_longitudeIsSet = true;
            return;
        }
        throw new SS7InvalidParamException("Longitude " + longitude + " out of range.");
    }
    /**
      * Get the longitude parameter.
      *
      * @return
      * Longitude in degrees (-180 &le; longitude &le; 180).
      */
    public float getLongitude() {
        return m_longitude;
    }
    protected float m_latitude = 0;
    protected boolean m_latitudeIsSet = false;
    protected float m_longitude = 0;
    protected boolean m_longitudeIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
