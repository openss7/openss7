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

package javax.jain.protocol.ss7.map.information;

import javax.jain.protocol.ss7.map.information.etsipar.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/** This primitive (event) is used by the MAP API Provider to deliver information (status
  * and/or location) for a Mobile Station. <p>
  *
  * The following rules applies for the setting of primitive parameters:- <ul>
  * <li>Mandatory parameters with no default value must be provided to the constructor.
  * <li>Mandatory parameters with default value need not be set.
  * <li>Optional / conditional parameter is by default not present. Such a parameter
  * becomes present by setting it. </ul> <p>
  *
  * Primitive parameters:- <ul>
  * <li>location, conditional parameter, present if location information was requested and
  * available
  * <li>state, conditional parameter, present if state information was requested and
  * available
  * <li>result, conditional parameter, present if information request failed </ul>
  */
public final class MsInfoConfEvent extends InformationEvent {
    /** MS state is idle (reachable an not busy).  */
    public static final int MS_STATE_IDLE = 0;
    /** MS state is busy.  */
    public static final int MS_STATE_BUSY = 1;
    /** MS state is not reachable (i.e. switched off or outside network coverage).  */
    public static final int MS_STATE_NOT_REACHABLE = 2;
    /** MS state is not provided.  */
    public static final int MS_STATE_NOT_PROVIDED = 3;
    /** The constructor.
      * @param source  The source of this event. Should be set to the Provider object for
      * the transaction.
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public MsInfoConfEvent(Object source)
        throws SS7InvalidParamException {
        super(source, InformationEvent.MS_INFO_CONF_ID);
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public MsInfoConfEvent()
        throws SS7InvalidParamException {
        setEventId(InformationEvent.MS_INFO_CONF_ID);
    }
    /** Set location information about the Mobile Station. This parametr is present if
      * location information is requested and available.
      * @param location  Location information.
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public void setLocation(Location location)
        throws SS7InvalidParamException {
        m_location = location;
    }
    /** Get location information about the Mobile Station. It shall be checked if this
      * parameter is present before getting it.
      * @return Location information.
      */
    public Location getLocation() {
        return m_location;
    }
    /** Check if location information is present.
      * @return True or false.
      */
    public boolean isLocationPresent() {
        return (m_location != null);
    }
    /** Set state information about the Mobile Station. This parametr is present if state
      * information is requested and available.
      * @param state  State information. Possible values:- <ul>
      * <li>MS_STATE_IDLE,
      * <li>MS_STATE_BUSY,
      * <li>MS_STATE_NOT_REACHABLE and
      * <li>MS_STATE_NOT_PROVIDED. </ul>
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of range.
      */
    public void setState(int state)
        throws SS7InvalidParamException {
        m_state = state;
        m_statePresent = true;
    }
    /** Get state information about the Mobile Station. It shall be checked if this
      * parameter is present before getting it.
      * @return State information. (See setState for possible values.)
      */
    public int getState() {
        return m_state;
    }
    /** Check if state information is present.
      * @return True or false.
      */
    public boolean isStatePresent() {
        return m_statePresent;
    }
    /** Set the result parameter. This parameter is present if the operation failed.
      * @param result  Result information.
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public void setResult(Result result)
        throws SS7InvalidParamException {
        m_result = result;
    }
    /** Get the result parameter. It shall be checked if this parameter is present before
      * getting it.
      * @return Result information.
      */
    public Result getResult() {
        return m_result;
    }
    /** Check if result parameter is present.
      * @return True or false.
      */
    public boolean isResultPresent() {
        return (m_result != null);
    }
    protected Location m_location = null;
    protected int m_state;
    protected boolean m_statePresent = false;
    protected Result m_result = null;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
