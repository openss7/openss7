/*
 @(#) $RCSfile: EventFilter.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:32 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:32 $ by $Author: brian $
 */

package javax.csapi.cc.jcc;


/** An instance of this EventFilter is supplied to the event source in
  * the addxxxListener() method by the EventListener to indicate what
  * Events are required by the EventListener.
  * When a {@link JccEvent} occurs, the event source will call the
  * predicate {@link #getEventDisposition(JccEvent)} to determine if the
  * Event should be fired to the EventListener.  Given an event,
  * <code>getEventDisposition()</code> returns <ol>
  *
  * <li> {@link #EVENT_DISCARD} if the listener is not interested in
  * receiving the event.
  *
  * <li> {@link #EVENT_NOTIFY} if the listener should be sent a
  * non-blocking notification.
  *
  * <li> {@link #EVENT_BLOCK} if the listener should be sent a blocking
  * event (trigger).  This return value applies to
  * {@link JccConnectionEvent}s only.  </ol>
  *
  * The EventFilter while providing flexibility will impact the
  * performance of the platform. Hence, the JCC implementation is
  * expected to provide for some standard EventFilters as explained in
  * the {@link JccProvider} interface. 
  * @version 1.2.2
  * @author Monavacon Limited
  * @since 1.0 */
public interface EventFilter {
   /** This predicate indicates whether the specified Event is required
     * by an EventListener. This method will be called by the Event
     * source prior to firing the event. The return type can be either
     * of <ol>
     *
     * <li> {@link #EVENT_DISCARD} if the listener is not interested in
     * receiving the event.
     *
     * <li> {@link #EVENT_NOTIFY} if the listener should be sent a
     * non-blocking notification.
     *
     * <li> {@link #EVENT_BLOCK} if the listener should be sent a
     * blocking event (trigger).  This return value applies to
     * JccConnectionEvents only since the other type of events are
     * non-blocking.  </ol>
     *
     * @return
     * An int representing the disposition for the event concerned. 
     * @param event
     * Specifies the event.  */
   public int getEventDisposition(JccEvent event);
   /** Predicate return constant: Indicates that the specified event is
     * not required.  This is one of the possible return values of
     * <code>getEventDisposition()</code> */
   public static final int EVENT_DISCARD = 0;
   /** Predicate return constant: Indicates that the specified event is
     * required and is a non-blocking Event (notification only), that
     * is, call processing will not be suspended.  This is one of the
     * possible return values of <code>getEventDisposition()</code> */
   public static final int EVENT_NOTIFY = 1;
   /** Predicate return constant: Indicates that the specified event is
     * required and is a blocking Event, that is, call processing will
     * be suspended until the {@link JccConnection#continueProcessing()}
     * or any other valid method is called.  This is one of the possible
     * return values of <code>getEventDisposition()</code> */
   public static final int EVENT_BLOCK =2;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
