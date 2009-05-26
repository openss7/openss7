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
  * This parameter represents an Ellispoid Arc (an arc shaped area on the earth) as defined in the specification 3G TS
  * 23.032 (Universal Geographical Area Description, GAD). (Special cases of an arc is a sector and a circle.) <p>
  *
  * This class represents the position information in a straight-forward manner (meters, degrees) and is left to JAIN
  * MAP API implementations to translate between these value and the protocol encoding. <p>
  *
  * The following rules apply when setiing parameter components: <p>
  *
  * <li><b>Mandatory component</b> with <b>no default value</b> must be provided to the constructor.
  * <li><b>Mandatory component</b> with <b>default value</b> need not be set.
  * <li><b>Optional/conditional component</b> is by default not present.  Such a component becomes present by setting
  * it.
  * <p>
  *
  * <b>Parameter components:</b>
  * <li><var>innerRadius</var> (0 &lt;= meters &lt;=  327675), mandatory componet with no default;
  * <li><var>uncertaintyRadius</var>, (0 &lt;= meters &lt;= 18000000) mandatory component with no default;
  * <li><var>offsetAngle</var>, (0 &lt;= degrees &lt;= 359) mandatory component with no default;
  * <li><var>includeAngle</var>, (1 &lt;= degrees &lt;= 360) mandatory component with no default;
  * <li><var>confidence</var>, (1 &lt;= percent &lt;= 100), optional component.
  */
public class PositionArc extends PositionPoint {
    /**
      * The constructor.
      *
      * @param latitude Latitude in degrees (-90 &lt;= latitude &lt;= 90).
      * @param longitude Longitude in degrees (-180 &lt;= longitude &lt;= 180).
      * @param innerRadius Inner radius of the arg in meters (0 &lt;= radius &lt;= 327675).
      * @param uncertaintyRadius Arc depth from the inner radius to the outer radius in meters (0 &lt;= arc depth &lt;= 1800000).
      * @param offsetAngle Angle clockwise from north to the left edge of the arc in degrees (0 &lt;= offset &lt;= 359).
      * @param includedAngle Angle width from left edge to the right edge of the arc (1 &lt;= width &lt;= 360).
      * @exception SS7InvalidParamException Thrown if parameters are invalid or out of range.
      */
    public PositionArc(float latitude, float longitude, float innerRadius, float uncertaintyRadius, float offsetAngle, float includedAngle) {
        this();
        m_latitude = latitude;
        m_longitude = longitude;
        this.setInnerRadius(innerRadius);
        this.setUncertaintyRadius(uncertaintyRadius);
        this.setOffsetAngle(offsetAngle);
        this.setIncludedAngle(includedAngle);
    }
    /**
      * Empty constructor; needed for serializable objects and beans.
      */
    public PositionArg() throws SS7InvalidParamException {
    }
    /**
      * Change the parameter specifying the inner radius of the arc.
      * @param innerRadius Inner radius of the arg in meters (0 &lt;= radius &lt;= 327675).
      * @exception SS7InvalidParamException Thrown if parameters are invalid or out of range.
      */
    public void setInnerRadius() throws SS7InvalidParamException {
        if (innerRadius < 0 || 327675 < innerRadius)
            throw SS7InvalidParamException("Inner Radius " + innerRadius + " out of range.");
        m_radius = innerRadius;
        m_radius_is_set = true;
    }
    /**
      * Get the parameter specifying the inner radius of the arc.
      * @return
      */
    public float getInnerRadius() {
        float radius = 0;
        if (m_radius_is_set)
            radius = m_radius;
        return radius;
    }
    /**
      * Change the parameter specifying the uncertainty radius of the arc.
      * @param uncertaintyRadius Arc depth from the inner radius to the outer radius in meters (0 &lt;= arc depth &lt;= 1800000).
      * @exception SS7InvalidParamException Thrown if parameters are invalid or out of range.
      */
    public void setUncertaintyRadius(float uncertaintyRadius) throws SS7InvalidParamException {
        if (uncertaintyRadius < 0 || 1800000 < uncertaintyRadius)
            throw SS7InvalidParamException("Uncertainty Radius " + uncertaintyRadius + " is out of range.");
        m_uradius = uncertaintyRadius;
        m_uradius_is_set = true;
    }
    /**
      * Get the parameter specifying the uncertainty radius of the arc.
      * @return Arc depth from the inner radius to the outer radius in meters (0 &lt;= arc depth &lt;= 1800000).
      */
    public float getUncertaintyRadius(void) {
        float uradius = 0;
        if (m_uradius_is_set)
            uradius = m_uradius;
        return uradius;
    }
    /**
      * Change the parameter specifying the offset angle of the arc.
      * @param offsetAngle Angle clockwise from north to the left edge of the arc in degrees (0 &lt;= offset &lt;= 359).
      * @exception SS7InvalidParamException Thrown if parameters are invalid or out of range.
      */
    public void setOffsetAngle(float offsetAngle) throws SS7InvalidParamException {
        if (offsetAngle < 0 || 359 < offsetAngle)
            throw SS7InvalidParamException("Offset Angle " + offsetAngle + " is out of range.");
        m_offset = offsetAngle;
        m_offset_is_set = true;
    }
    /**
      * Get the parameter specifying the offset angle of the arc.
      * @return Angle clockwise from north to the left edge of the arc in degrees (0 &lt;= offset &lt;= 359).
      */
    public float getOffsetAngle() {
        float offset = 0;
        if (m_offset_is_set)
            offset = m_offset;
        return offset;
    }
    /**
      * Change the parameter specifying the included angle of the arc.
      * @param includedAngle Angle width from left edge to the right edge of the arc (1 &lt;= width &lt;= 360).
      * @exception SS7InvalidParamException Thrown if parameters are invalid or out of range.
      */
    public void setIncludedAngle(float includedAngle) throws SS7InvalidParamException {
        if (includedAngle < 1 || 360 < includedAngle)
            throw SS7InvalidParamException("Included Angle " + includedAngle + " is out of range.");
        m_angle = includedAngle;
        m_angle_is_set = true;
    }
    /**
      * Get the parameter specifying the included angle of the arc.
      * @return Angle width from left edge to the right edge of the arc (1 &lt;= width &lt;= 360).
      */
    public float getIncludedAngle() {
        float angle = 360;
        if (m_angle_is_set)
            angle = m_angle;
        return angle;
    }
    /**
      * Set the parameter specifying the confidence by which the target is within the unrectainty area.
      * @param confidence The probability in percent that the target is within the uncertainty area (0 &lt;= confidence &lt;= 100).
      * @exception SS7InvalidParamException Thrown if parameters are invalid or out of range.
      */
    public void setConfidence(float confidence) throws SS7InvalidParamException {
        if (confidence < 0 || 100 < confidence)
            throw SS7InvalidParamException("Confidence " + confidence + " is out of range.");
        m_confidence = confidence;
        m_confidence_is_set = true;
    }
    /**
      * Get the parameter specifying the confidence by which a target is within the uncertaintly area.
      * @return The probability in percent that the target is within the uncertainty area (0 &lt;= confidence &lt;= 100).
      */
    public float getConfidence() {
        float confidence = 0;
        if (m_confidence_is_set)
            confidence = m_confidence;
        return confidence;
    }
    /**
      * Check if the confidence parameter is present.
      */
    public boolean isConfidencePresent() {
        return m_confidence_is_set;
    }

    /**
      * Change the parameter specifying the latitude.
      * @param latitude Latitude in degrees (-90 &lt;= latitude &lt;= 90).
      * @exception SS7InvalidParamException Thrown if parameters are invalid or out of range.
      */
    protected void setLatitude(float latitude) throws SS7InvalidParamException {
        if (latitude < -90 || 90 < latitude)
            throw SS7InvalidParamException("Latitude " + latitude + " is out of range.");
        m_latitude = latitude;
        m_latitude_is_set = true;
    }
    /**
      * Get the parameter specifying the latitude.
      * @return Latitude in degrees (-90 &lt;= latitude &lt;= 90).
      */
    protected float getLatitude() {
        float latitude = 0;
        if (m_latitude_is_set)
            latitude = m_latitude;
        return latitude;
    }
    /**
      * Change the parameter specifying the longitude.
      * @param longitude Longitude in degrees (-180 &lt;= longitude &lt;= 180).
      * @exception SS7InvalidParamException Thrown if parameters are invalid or out of range.
      */
    protected void setLongitude(float latitude) throws SS7InvalidParamException {
        if (longitude < -180 || 180 < longitude)
            throw SS7InvalidParamException("Longitude " + longitude + " is out of range.");
        m_longitude = longitude;
        m_longitude_is_set = true;
    }
    /**
      * Get the parameter specifying the longitude.
      * @return Longitude in degrees (-180 &lt;= longitude &lt;= 180).
      */
    protected float getLongitude() {
        float longitude = 0;
        if (m_longitude_is_set)
            longitude = m_longitude;
        return longitude;
    }
    /*
     * Private fields;
     */
    private boolean m_latitude_is_set = false;
    private float m_latitude = 0;
    private boolean m_longitude_is_set = false;
    private float m_longitude = 0;
    private boolean m_radius_is_set = false;
    private float m_radius;
    private boolean m_uradius_is_set = false;
    private float m_uradius;
    private boolean m_offset_is_set = false;
    private float m_offset;
    private boolean m_angle_is_set = false;
    private float m_angle;
    private boolean m_confidence_is_set = false;
    private float m_confidence;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
