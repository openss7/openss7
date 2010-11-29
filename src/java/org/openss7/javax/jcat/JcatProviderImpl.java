/*
 @(#) $RCSfile: JcatProviderImpl.java,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2010-11-28 14:28:38 $ <p>
 
 Copyright &copy; 2008-2010  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
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
 
 Last Modified $Date: 2010-11-28 14:28:38 $ by $Author: brian $
 */

package org.openss7.javax.jcat;

import org.openss7.javax.csapi.cc.jcc.*;
import javax.csapi.cc.jcc.*;
import javax.jcat.*;

/**
  * JcatProvider interface extends the JccProvider interface.
  * This interface is expected to be used when advanced call control
  * features are desired. JcatProvider has the same finite state machine
  * as JccProvider. <p>
  *
  * A JcatProvider is associated with JcatCall, JcatConnection and
  * JcatAddress which are extensions of corresponding JCC entities. In
  * addition, a JcatProvider also has JcatTerminal objects and
  * JcatTerminalConnection objects associated with it during a call.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class JcatProviderImpl extends JccProviderImpl implements JcatProvider {
    /**
      * Returns a Set of JcatCall objects in which the JcatAddress participates.
      * The call are currently associated with the JcatProvider. When a
      * JcatCall moves into the JccCall.INVALID state, the JcatProvider
      * loses its reference to this JcatCall.  Therefore, all Calls
      * returned by this method must either be in the JccCall.ACTIVE
      * state. This method returns an empty Set if zero calls match the
      * request.
      * @param address
      * The address that participates in the calls
      */
    public native java.util.Set<JcatCall> getCalls(JcatAddress address);
    /**
      * Returns a Set of JcatTerminal objects administered by the
      * JcatProvider who's name satisfies the regular expression.
      * @param nameRegex
      * Denotes the regular expression (for the purpose of this
      * specification, the platform will use the Perl5 regular
      * expressions).
      */
    public native java.util.Set<JcatTerminal> getTerminals(java.lang.String nameRegex);
    /**
      * This method returns a standard EventFilter which is implemented
      * by the JCAT platform.
      * For all events that require filtering by this EventFilter, apply
      * the following: <ul>
      *
      * <li>If the terminal name is matched, the filter returns the value matchDisposition.
      * <li>If the terminal name is not matched, then return nomatchDisposition. </ul>
      *
      * @param terminalNameRegex
      * Denotes the regular expression (for the purpose of this
      * specification, the platform will use the Perl5 regular
      * expressions).
      * @param matchDisposition
      * Indicates the disposition of the
      * JcatAddressEvent.TERMINAL_REGISTERED event with equal cause
      * code. This should be one of the legal dispositions namely,
      * EventFilter.EVENT_BLOCK, EventFilter.EVENT_DISCARD or
      * EventFilter.EVENT_NOTIFY.
      * @param nomatchDisposition
      * Indicates the disposition the
      * JcatAddressEvent.TERMINAL_REGISTERED event with other cause
      * code. This should be one of the legal dispositions namely,
      * EventFilter.EVENT_BLOCK, EventFilter.EVENT_DISCARD or
      * EventFilter.EVENT_NOTIFY.
      * @return
      * EventFilter standard EventFilter provided by the JCAT platform
      * to enable filtering of events based on the application's
      * requirements.
      * @exception ResourceUnavailableException
      * An internal resource for completing this call is unavailable.
      * @exception InvalidArgumentException
      * One or more of the provided argument is not valid
      * @since 1.0
      */
    public native EventFilter createEventFilterRegistration(java.lang.String terminalNameRegex, int matchDisposition, int nomatchDisposition)
        throws ResourceUnavailableException, InvalidArgumentException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
