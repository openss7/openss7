/*
 @(#) src/java/javax/jain/protocol/ss7/map/position/etsipar/PosRequestSpecifier.java <p>
 
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

package javax.jain.protocol.ss7.map.position.etsipar;

import javax.jain.protocol.ss7.map.position.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/** This parameter specifies different aspects of the location request, like: who is
  * asking, type of location, accuracy, priority, privacy, etc. <p>
  *
  * The following rules applies for the setting of parameter components:- <ul>
  * <li>Mandatory component with no default value must be provided to the constructor.
  * <li>Mandatory component with default value need not be set.
  * <li>Optional / conditional component is by default not present. Such a component
  * becomes present by setting it. </ul> <p>
  *
  * <h4>Parameter components:-</h4> <ul>
  *
  * <li>locationType, mandatory component with default
  * <li>lcsClientId, mandatory component with no default
  * <li>privacyOverride, optional component
  * <li>lcsPriority, conditional component
  * <li>lcsQualityOfService, conditional component </ul> <p>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class PosRequestSpecifier extends MapParameter {
    /** Location Type requested: current location.  */
    public static final int CURRENT_LOCATION = 0;
    /** Location Type requested: current or last location.  */
    public static final int CURRENT_OR_LAST_LOCATION = 1;
    /** Location Type requested: initial location.  */
    public static final int INITIAL_LOCATION = 2;
    /** Priority of location request: normal priority.  */
    public static final int NORMAL_PRIORITY = 3;
    /** Priority of location request: highest priority.  */
    public static final int HIGHEST_PRIORITY = 4;
    /** The constructor.
      * @param lcsClientId  Information identifying the recipient client.
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public PosRequestSpecifier(LcsClientId lcsClientId)
        throws SS7InvalidParamException {
        setLcsClientId(lcsClientId);
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public PosRequestSpecifier()
        throws SS7InvalidParamException {
    }
    /** Change type of location that is requested.
      * @param locationType  Integer indicating type of location.  Possible values: <ul>
      * <li>CURRENT_LOCATION,
      * <li>CURRENT_OR_LAST_LOCATION (default) and
      * <li>INITIAL_LOCATION. </ul>
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public void setLocationType(int locationType)
        throws SS7InvalidParamException {
        m_locationType = locationType;
    }
    /** Get type of location that is requested.
      * @return Integer indicating type of location. (See setLocationType for possible
      * values.)
      */
    public int getLocationType() {
        return m_locationType;
    }
    /** Set information identifying the recipient client.
      * @param lcsClientId  Client Id.
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public void setLcsClientId(LcsClientId lcsClientId)
        throws SS7InvalidParamException {
        m_lcsClientId = lcsClientId;
    }
    /** Get information identifying the recipient client.
      * @return Client Id.
      */
    public LcsClientId getLcsClientId() {
        return m_lcsClientId;
    }
    /** Set the parameter that indicates whether MS privacy is overridden by the LCS
      * client.
      * @param privacyOverride  True or false.
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public void setPrivacyOverride(boolean privacyOverride)
        throws SS7InvalidParamException {
        m_privacyOverride = privacyOverride;
        m_privacyOverridePresent = true;
    }
    /** Get the parameter that indicates whether MS privacy is overridden by the LCS
      * client. It shall be checked if this parameter is present before getting it.
      * @return True or false.
      */
    public boolean getPrivacyOverride() {
        return m_privacyOverride;
    }
    /** Check if the Privacy Override parameter is present.
      * @return True or false.
      */
    public boolean isPrivacyOverridePresent() {
        return m_privacyOverridePresent;
    }
    /** Set the priority of the location request.
      * @param lcsPriority  Integer indicating priority. Possible values: <ul>
      * <li>NORMAL_PRIORITY and
      * <li>HIGHEST_PRIORITY. </ul>
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public void setLcsPriority(int lcsPriority)
        throws SS7InvalidParamException {
        m_lcsPriority = lcsPriority;
        m_lcsPriorityPresent = true;
    }
    /** Get the priority of the location request. It shall be checked if this parameter is
      * present before getting it.
      * @return Integer indicating priority. (See setLcsPriority for possible values.)
      */
    public int getLcsPriority() {
        return m_lcsPriority;
    }
    /** Check if the LCS Priority parameter is present.
      * @return True or false.
      */
    public boolean isLcsPriorityPresent() {
        return m_lcsPriorityPresent;
    }
    /** Set the Quality Of Service parameter for the location request.
      * @param lcsQos  QoS specification.
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public void setLcsQos(LcsQos lcsQos)
        throws SS7InvalidParamException {
        m_lcsQos = lcsQos;
        m_lcsQosPresent = true;
    }
    /** Get the Quality Of Service parameter for the location request.  It shall be
      * checked if this parameter is present before getting it.
      * @return QoS specification.
      */
    public LcsQos getLcsQos() {
        return m_lcsQos;
    }
    /** Check if the Quality Of Service parameter is present.
      * @return True or false.
      */
    public boolean isLcsQosPresent() {
        return m_lcsQosPresent;
    }
    protected int m_locationType;
    protected LcsClientId m_lcsClientId = null;
    protected boolean m_privacyOverride;
    protected boolean m_privacyOverridePresent = false;
    protected int m_lcsPriority;
    protected boolean m_lcsPriorityPresent = false;
    protected LcsQos m_lcsQos = null;
    protected boolean m_lcsQosPresent = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
