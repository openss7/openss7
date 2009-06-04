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
  * This interface hence reports all state changes occurring in the JcatTerminalConnection
  * object.
  *
  * @since 1.0
  * @author Monavacon Limited
  * @version 1.2.2
  */
public interface JcatTerminalConnectionListener {
    /**
      * Indicates that the JcatTerminalConnection has just been placed in the
      * JcatTerminalConnection.IDLE state
      *
      * @param termconnectionevent
      * Event resulting from state change.
      *
      * @since 1.0
      */
    public void terminalConnectionCreated(JcatTerminalConnectionEvent termconnectionevent);
    /**
      * Indicates that the JcatTerminalConnection has just been placed in the
      * JcatTerminalConnection.RINGING state
      *
      * @param termconnectionevent
      * Event resulting from state change.
      *
      * @since 1.0
      */
    public void terminalConnectionRinging(JcatTerminalConnectionEvent termconnectionevent);
    /**
      * Indicates that the JcatTerminalConnection has just been placed in the
      * JcatTerminalConnection.DROPPED state
      *
      * @param termconnectionevent
      * Event resulting from state change.
      *
      * @since 1.0
      */
    public void terminalConnectionDropped(JcatTerminalConnectionEvent termconnectionevent);
    /**
      * Indicates that the JcatTerminalConnection has just been placed in the
      * JcatTerminalConnection.BRIDGED state
      *
      * @param termconnectionevent
      * Event resulting from state change.
      *
      * @since 1.0
      */
    public void terminalConnectionBridged(JcatTerminalConnectionEvent termconnectionevent);
    /**
      * Indicates that the JcatTerminalConnection has just been placed in the
      * JcatTerminalConnection.TALKING state
      *
      * @param termconnectionevent
      * Event resulting from state change.
      *
      * @since 1.0
      */
    public void terminalConnectionTalking(JcatTerminalConnectionEvent termconnectionevent);
    /**
      * Indicates that the JcatTerminalConnection has just been placed in the
      * JcatTerminalConnection.INUSE state
      *
      * @param termconnectionevent
      * Event resulting from state change.
      *
      * @since 1.0
      */
    public void terminalConnectionInuse(JcatTerminalConnectionEvent termconnectionevent);
    /**
      * Indicates that the JcatTerminalConnection has just been placed in the
      * JcatTerminalConnection.HELD state
      *
      * @param termconnectionevent
      * Event resulting from state change.
      *
      * @since 1.0
      */
    public void terminalConnectionHeld(JcatTerminalConnectionEvent termconnectionevent);
    /**
      * Indicates that event transmission has ended.
      *
      * @param termconnectionevent
      * Event indicating for which terminal connection event transmission has ended.
      *
      * @since 1.0
      */
    public void terminalConnectionEventTransmissionEnded(JcatTerminalConnectionEvent termconnectionevent);
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-

