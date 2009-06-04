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

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/** This class is the base class of all event classes (representing the primitives) of the
  * MS Information Capability.
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract class InformationEvent extends JainEvent {
    /** Event ID for MS Information Request.  */
    public static final int MS_INFO_REQ_ID = 0;
    /** Event ID for MS Information Confirm.  */
    public static final int MS_INFO_CONF_ID = 1;
    /** The constructor.
      * @param source  The source of this event.
      * @param eventId  EventId identifies the type of event/primitive.  It is provided in
      * order to get faster execution, i.e.  there is no need to use the instanceof
      * operator to retrieve the type.
      * @exception SS7InvalidParamException  Thrown if the eventId is out of range.
      */
    public InformationEvent(Object source, int eventId)
            throws SS7InvalidParamException {
            super(source);
            setEventId(eventId);
    }
    /** Empty constructor needed for serializable beans.  */
    public InformationEvent() {
    }
    /** Set the eventId that identifies the type of event/primitive.
      * @param eventId  The eventId.
      * @exception SS7InvalidParamException  Thrown if the eventId is out of range.
      */
    public void setEventId(int eventId)
            throws SS7InvalidParamException {
            switch (eventId) {
                case MS_INFO_REQ_ID:
                case MS_INFO_CONF_ID:
                    m_eventId = eventId;
                    return;
            }
            throw new SS7InvalidParamException("EventId value " + eventId + " out of range.");
    }
    /** Get the eventId that identifies the type of event/primitive. It is provided in
      * order to get faster execution, i.e. there is no need to use the instanceof
      * operator to retrieve the type of the event.
      * @return The eventId.
      */
    public int getEventId() {
        return m_eventId;
    }
    protected int m_eventId;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
