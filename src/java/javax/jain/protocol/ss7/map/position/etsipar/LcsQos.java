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

package javax.jain.protocol.ss7.map.position.etsipar;

import javax.jain.protocol.ss7.map.position.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/** This parameter specifies the the desired response time for a position request, if
  * vertical cordinate is requested and what the desired accuracy is in meters. <p>
  *
  * The following rules applies for the setting of parameter components:- <ul>
  * <li>Mandatory component with no default value must be provided to the constructor.
  * <li>Mandatory component with default value need not be set.
  * <li>Optional / conditional component is by default not present. Such a component
  * becomes present by setting it. </ul> <p>
  *
  * Parameter components:- <ul>
  * <li>horizontalAccuracy, optional component
  * <li>verticalCoordinateRequest, optional component
  * <li>verticalAccuracy, optional component
  * <li>responseTime, optional component </ul> <p>
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class LcsQos extends MapParameter {
    /** LCS Response Time: low delay response requested.  */
    public static final int LOW_DELAY_RESPONSE = 0;
    /** LCS Response Time: delay is tolerated.  */
    public static final int DELAYTOLERANT = 1;
    /** The constructor.
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public LcsQos()
        throws SS7InvalidParamException {
    }
    /** Set the desired Horizontal Accuracy.
      * @param horizontalAccuracy  Horizontal Accuracy in meters.
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public void setHorizontalAccuracy(float horizontalAccuracy)
        throws SS7InvalidParamException {
        m_horizontalAccuracy = horizontalAccuracy;
        m_horizontalAccuracyPresent = true;
    }
    /** Get the desired Horizontal Accuracy. It shall be checked if this parameter is
      * present before getting it.
      * @return Horizontal Accuracy in meters.
      */
    public float getHorizontalAccuracy() {
        return m_horizontalAccuracy;
    }
    /** Check if the Horizontal Accuracy parameter is present.
      * @return True or false.
      */
    public boolean isHorizontalAccuracyPresent() {
        return m_horizontalAccuracyPresent;
    }
    /** Set the Vertical Coordinate Request parameter to indicate whether vertical
      * position is requested.
      * @param verticalCoordinateRequest  True or false.
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public void setVerticalCoordinateRequest(boolean verticalCoordinateRequest)
        throws SS7InvalidParamException {
        m_verticalCoordinateRequest = verticalCoordinateRequest;
        m_verticalCoordinateRequestPresent = true;
    }
    /** Get the Vertical Coordinate Request parameter to indicate whether vertical
      * position is requested.
      * @return True or false.
      */
    public boolean getVerticalCoordinateRequest() {
        return m_verticalCoordinateRequest;
    }
    /** Check if the Vertical Coordinate Request parameter is present.
      * @return True or false.
      */
    public boolean isVerticalCoordinateRequestPresent() {
        return m_verticalCoordinateRequestPresent;
    }
    /** Set the desired Vertical Accuracy.
      * @param verticalAccuracy  Vertical Accuracy in meters.
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public void setVerticalAccuracy(float verticalAccuracy)
        throws SS7InvalidParamException {
        m_verticalAccuracy = verticalAccuracy;
        m_verticalAccuracyPresent = true;
    }
    /** Get the desired Vertical Accuracy. It shall be checked if this parameter is
      * present before getting it.
      * @return Vertical Accuracy in meters.
      */
    public float getVerticalAccuracy() {
        return m_verticalAccuracy;
    }
    /** Check if the Vertical Accuracy parameter is present.
      * @return True or false.
      */
    public boolean isVerticalAccuracyPresent() {
        return m_verticalAccuracyPresent;
    }
    /** Set the desired Response Time Category.
      * @param responseTime  Integer indicating response time category.  Possible values:
      * <ul> <li>LOW_DELAY_RESPONSE and <li>DELAYTOLERANT. </ul>
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public void setResponseTime(int responseTime)
        throws SS7InvalidParamException {
        m_responseTime = responseTime;
        m_responseTimePresent = true;
    }
    /** Get the desired Response Time Category. It shall be checked if this parameter is
      * present before getting it.
      * @return Integer indicating response time category. (See setResponseTime for
      * possible values.)
      */
    public int getResponseTime() {
        return m_responseTime;
    }
    /** Check if the Response Time Category parameter is present.
      * @return True or false.
      */
    public boolean isResponseTimePresent() {
        return m_responseTimePresent;
    }
    protected float m_horizontalAccuracy;
    protected boolean m_horizontalAccuracyPresent = false;
    protected boolean m_verticalCoordinateRequest;
    protected boolean m_verticalCoordinateRequestPresent = false;
    protected float m_verticalAccuracy;
    protected boolean m_verticalAccuracyPresent = false;
    protected int m_responseTime;
    protected boolean m_responseTimePresent = false;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
