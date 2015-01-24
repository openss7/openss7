/*
 @(#) src/java/javax/jain/protocol/ss7/map/information/InformationEvent.java <p>
 
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

package javax.jain.protocol.ss7.map.information;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/** This class is the base class of all event classes (representing the primitives) of the
  * MS Information Capability.
  *
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
    public InformationEvent(java.lang.Object source, int eventId)
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

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
