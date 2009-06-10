/*
 @(#) $RCSfile$ $Name$($Revision$) $Date$ <p>
 
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
 
 Last Modified $Date$ by $Author$
 */

package javax.jain.protocol.ss7.map.position;

import javax.jain.protocol.ss7.map.position.etsipar.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/** This primitive (event) is used by the MAP API Provider to deliver "pushed" position
  * information for a Mobile Station. <p>
  *
  * The following rules applies for the setting of primitive parameters:- <ul>
  * <li>Mandatory parameters with no default value must be provided to the constructor.
  * <li>Mandatory parameters with default value need not be set.
  * <li>Optional / conditional parameter is by default not present. Such a parameter
  * becomes present by setting it. </ul> <p>
  *
  * Primitive parameters:- <ul>
  * <li>msId, mandatory parameter with no default.
  * <li>position, conditional parameter, present if position information is available.
  * <li>triggeringEvent, mandatory parameter with no default.
  * <li>lcsClientId, mandatory parameter with no default. </ul> <p>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public final class MsPosReportIndEvent extends PositionEvent {
    /** Event that trigged the location report: emergency call origination.  */
    public static final int EVENT_EMERGENCY_CALL_ORIGINATION = 0;
    /** Event that trigged the location report: emergency call release.  */
    public static final int EVENT_EMERGENCY_CALL_RELEASE = 1;
    /** Event that trigged the location report: mobile originated location request.  */
    public static final int EVENT_MO_LOCATION_REQUEST = 2;
    /** Event that trigged the location report: undefined event.  */
    public static final int EVENT_UNDEFINED = 3;
    /** The constructor.
      * @param source  The source of this event. Should be set to the Provider object for
      * the transaction.
      * @param msId  Id of the Mobile Station to which the position information refers:
      * MSISDN, IMSI, ... .
      * @param triggeringEvent  The event that triggered this "push" of position
      * information.
      * @param lcsClientId  Information identifying the recipient client.
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      *
      */
    public MsPosReportIndEvent(java.lang.Object source, PositionTargetId msId, int triggeringEvent, LcsClientId lcsClientId)
        throws SS7InvalidParamException {
        super(source, PositionEvent.MS_POS_IND_ID);
        setMsId(msId);
        setTriggeringEvent(triggeringEvent);
        setLcsClientId(lcsClientId);
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public MsPosReportIndEvent()
        throws SS7InvalidParamException {
    }
    /** Change the Id of the Mobile Station to which the position information refers.
      * @param msId  Identifier of the Mobile Station: MSISDN, IMSI, ... .
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public void setMsId(PositionTargetId msId)
        throws SS7InvalidParamException {
        m_msId = msId;
    }
    /** Get the Id of the Mobile Station to which the position information refers.
      * @return Identifier of the Mobile Station: MSISDN, IMSI, ... .
      */
    public PositionTargetId getMsId() {
        return m_msId;
    }
    /** Set the Position.
      * @param position  Position.
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public void setPosition(Position position)
        throws SS7InvalidParamException {
        m_position = position;
        m_positionPresent = true;
    }
    /** Get the Position. It shall be checked if this parameter is present before getting
      * it.
      * @return Position.
      */
    public Position getPosition() {
        return m_position;
    }
    /** Check if the Position is present.
      * @return True or false.
      */
    public boolean isPositionPresent() {
        return m_positionPresent;
    }
    /** Set the event that triggered this "push" of position information.
      * @param triggeringEvent  Integer indicating triggering event.  Possible values:-
      * <ul>
      * <li>EVENT_EMERGENCY_CALL_ORIGINATION,
      * <li>EVENT_EMERGENCY_CALL_RELEASE and
      * <li>EVENT_MO_LOCATION_REQUEST. </ul>
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public void setTriggeringEvent(int triggeringEvent)
        throws SS7InvalidParamException {
        m_triggeringEvent = triggeringEvent;
    }
    /** Get the event that triggered this "push" of position information.
      * @return Integer indicating triggering event. (See setTriggeringEvent for possible
      * values.)
      */
    public int getTriggeringEvent() {
        return m_triggeringEvent;
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
    protected PositionTargetId m_msId;
    protected Position m_position;
    protected boolean m_positionPresent = false;
    protected int m_triggeringEvent;
    protected LcsClientId m_lcsClientId;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
