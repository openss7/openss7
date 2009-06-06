/* ***************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

package javax.jcat;

import javax.csapi.cc.jcc.*;

/**
  * This interface is related to events on the JcatTerminalConnection.
  *
  * @since 1.0
  * @author Monavacon Limited
  * @version 1.2.2
  */
public interface JcatTerminalConnectionEvent extends JccEvent {
    /** Cause code indicating the event is related to the park feature. */
    public static final int CAUSE_PARK = 1;
    /** Cause code indicating the event is related to the unhold feature. */
    public static final int CAUSE_UNHOLD = 2;
    /** This event indicates that the state of the JcatTerminalConnection object has
      * changed to JcatTerminalConnection.IDLE.  */
    public static final int TERMINALCONNECTION_IDLE = 1;
    /** This event indicates that the state of the JcatTerminalConnection object has
      * changed to JcatTerminalConnection.RINGING.  */
    public static final int TERMINALCONNECTION_RINGING = 2;
    /** This event indicates that the state of the JcatTerminalConnection object has
      * changed to JcatTerminalConnection.DROPPED.  */
    public static final int TERMINALCONNECTION_DROPPED = 3;
    /** This event indicates that the state of the JcatTerminalConnection object has
      * changed to JcatTerminalConnection.BRIDGED.  */
    public static final int TERMINALCONNECTION_BRIDGED = 4;
    /** This event indicates that the state of the JcatTerminalConnection object has
      * changed to JcatTerminalConnection.TALKING.  */
    public static final int TERMINALCONNECTION_TALKING = 5;
    /** This event indicates that the state of the JcatTerminalConnection object has
      * changed to JcatTerminalConnection.INUSE.  */
    public static final int TERMINALCONNECTION_INUSE = 6;
    /** This event indicates that the state of the JcatTerminalConnection object has
      * changed to JcatTerminalConnection.HELD.  */
    public static final int TERMINALCONNECTION_HELD = 7;
    /** This event indicates that event transmissions associated with the associated
      * JcatTerminalConnection object have ended.  */
    public static final int TERMINALCONNECTION_EVENT_TRANSMISSION_ENDED = 8;
    /**
      * This method returns the JcatTerminalConnection associated with this
      * JcatTerminalConnectionEvent. The result of this call might be a cast of the result of
      * JccEvent.getSource().
      *
      * @return
      * The JcatTerminalConnection associated with this event.
      */
    public JcatTerminalConnection getTerminalConnection();
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-

