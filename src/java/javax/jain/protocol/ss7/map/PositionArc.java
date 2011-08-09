/*
 @(#) $RCSfile: PositionArc.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:38 $ <p>
 
 Copyright &copy; 2008-2009  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
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
 <a href="http://www.openss7.com/">http://www.openss7.com/</a> <p>
 
 Last Modified $Date: 2009-06-21 11:34:38 $ by $Author: brian $
 */

package javax.jain.protocol.ss7.map;

import javax.jain.protocol.ss7.SS7InvalidParamException;

/**
  * This parameter represents an Ellispoid Arc (an arc shaped area on the earth) as defined in the specification 3G TS
  * 23.032 (Universal Geographical Area Description, GAD). (Special cases of an arc is a sector and a circle.) <p>
  *
  * This class represents the position information in a straight-forward manner (meters, degrees) and is left to JAIN
  * MAP API implementations to translate between these value and the protocol encoding. <p>
  *
  * The following rules apply when setiing parameter components: <ul>
  *
  * <li><b>Mandatory component</b> with <b>no default value</b> must be
  * provided to the constructor.
  *
  * <li><b>Mandatory component</b> with <b>default value</b> need not be
  * set.
  *
  * <li><b>Optional/conditional component</b> is by default not present.
  * Such a component becomes present by setting it.  </ul>
  *
  * <h4>Parameter components:</h4><ul>
  *
  * <li><var>innerRadius</var> (0 &le; meters &le;  327675), mandatory
  * componet with no default;
  *
  * <li><var>uncertaintyRadius</var>, (0 &le; meters &le; 18000000)
  * mandatory component with no default;
  *
  * <li><var>offsetAngle</var>, (0 &le; degrees &le; 359) mandatory
  * component with no default;
  *
  * <li><var>includeAngle</var>, (1 &le; degrees &le; 360) mandatory
  * component with no default;
  *
  * <li><var>confidence</var>, (1 &le; percent &le; 100), optional
  * component. </ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class PositionArc extends PositionPoint {
    /**
      * The constructor.
      *
      * @param latitude Latitude in degrees (-90 &le; latitude &le; 90).
      * @param longitude Longitude in degrees (-180 &le; longitude &le; 180).
      * @param innerRadius Inner radius of the arg in meters (0 &le; radius &le; 327675).
      * @param uncertaintyRadius Arc depth from the inner radius to the outer radius in meters (0 &le; arc depth &le; 1800000).
      * @param offsetAngle Angle clockwise from north to the left edge of the arc in degrees (0 &le; offset &le; 359).
      * @param includedAngle Angle width from left edge to the right edge of the arc (1 &le; width &le; 360).
      * @exception SS7InvalidParamException Thrown if parameters are invalid or out of range.
      */
    public PositionArc(float latitude, float longitude, float innerRadius, float uncertaintyRadius, float offsetAngle, float includedAngle)
        throws SS7InvalidParamException {
        super(latitude, longitude);
        setInnerRadius(innerRadius);
        setUncertaintyRadius(uncertaintyRadius);
        setOffsetAngle(offsetAngle);
        setIncludedAngle(includedAngle);
    }
    /**
      * Empty constructor; needed for serializable objects and beans.
      */
    public PositionArc()
        throws SS7InvalidParamException {
    }
    /**
      * Change the parameter specifying the inner radius of the arc.
      * @param innerRadius Inner radius of the arg in meters (0 &le; radius &le; 327675).
      * @exception SS7InvalidParamException Thrown if parameters are invalid or out of range.
      */
    public void setInnerRadius(float innerRadius)
        throws SS7InvalidParamException {
        if (innerRadius < 0 || 327675 < innerRadius)
            throw new SS7InvalidParamException("Inner Radius " + innerRadius + " out of range.");
        m_radius = innerRadius;
        m_radius_is_set = true;
    }
    /**
      * Get the parameter specifying the inner radius of the arc.
      * @return float inner radius.
      */
    public float getInnerRadius() {
        float radius = 0;
        if (m_radius_is_set)
            radius = m_radius;
        return radius;
    }
    /**
      * Change the parameter specifying the uncertainty radius of the arc.
      * @param uncertaintyRadius Arc depth from the inner radius to the outer radius in meters (0 &le; arc depth &le; 1800000).
      * @exception SS7InvalidParamException Thrown if parameters are invalid or out of range.
      */
    public void setUncertaintyRadius(float uncertaintyRadius)
        throws SS7InvalidParamException {
        if (uncertaintyRadius < 0 || 1800000 < uncertaintyRadius)
            throw new SS7InvalidParamException("Uncertainty Radius " + uncertaintyRadius + " is out of range.");
        m_uradius = uncertaintyRadius;
        m_uradius_is_set = true;
    }
    /**
      * Get the parameter specifying the uncertainty radius of the arc.
      * @return Arc depth from the inner radius to the outer radius in meters (0 &le; arc depth &le; 1800000).
      */
    public float getUncertaintyRadius() {
        float uradius = 0;
        if (m_uradius_is_set)
            uradius = m_uradius;
        return uradius;
    }
    /**
      * Change the parameter specifying the offset angle of the arc.
      * @param offsetAngle Angle clockwise from north to the left edge of the arc in degrees (0 &le; offset &le; 359).
      * @exception SS7InvalidParamException Thrown if parameters are invalid or out of range.
      */
    public void setOffsetAngle(float offsetAngle)
        throws SS7InvalidParamException {
        if (offsetAngle < 0 || 359 < offsetAngle)
            throw new SS7InvalidParamException("Offset Angle " + offsetAngle + " is out of range.");
        m_offset = offsetAngle;
        m_offset_is_set = true;
    }
    /**
      * Get the parameter specifying the offset angle of the arc.
      * @return Angle clockwise from north to the left edge of the arc in degrees (0 &le; offset &le; 359).
      */
    public float getOffsetAngle() {
        float offset = 0;
        if (m_offset_is_set)
            offset = m_offset;
        return offset;
    }
    /**
      * Change the parameter specifying the included angle of the arc.
      * @param includedAngle Angle width from left edge to the right edge of the arc (1 &le; width &le; 360).
      * @exception SS7InvalidParamException Thrown if parameters are invalid or out of range.
      */
    public void setIncludedAngle(float includedAngle)
        throws SS7InvalidParamException {
        if (includedAngle < 1 || 360 < includedAngle)
            throw new SS7InvalidParamException("Included Angle " + includedAngle + " is out of range.");
        m_angle = includedAngle;
        m_angle_is_set = true;
    }
    /**
      * Get the parameter specifying the included angle of the arc.
      * @return Angle width from left edge to the right edge of the arc (1 &le; width &le; 360).
      */
    public float getIncludedAngle() {
        float angle = 360;
        if (m_angle_is_set)
            angle = m_angle;
        return angle;
    }
    /**
      * Set the parameter specifying the confidence by which the target is within the unrectainty area.
      * @param confidence The probability in percent that the target is within the uncertainty area (0 &le; confidence &le; 100).
      * @exception SS7InvalidParamException Thrown if parameters are invalid or out of range.
      */
    public void setConfidence(float confidence)
        throws SS7InvalidParamException {
        if (confidence < 0 || 100 < confidence)
            throw new SS7InvalidParamException("Confidence " + confidence + " is out of range.");
        m_confidence = confidence;
        m_confidence_is_set = true;
    }
    /**
      * Get the parameter specifying the confidence by which a target is within the uncertaintly area.
      * @return The probability in percent that the target is within the uncertainty area (0 &le; confidence &le; 100).
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

    /*
     * Private fields;
     */
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

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
