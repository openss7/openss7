/*
 @(#) src/java/javax/csapi/cc/jcc/JccCallListener.java <p>
 
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

package javax.csapi.cc.jcc;

/** This interface reports all changes to the Call object.  The
  * {@link JccCallEvent} interface is the base interface for all
  * Call-related events.  All Call-related events must extend this
  * interface.  Events which extend this interface are reported via the
  * <code>JccCallListener</code> interface.  <p>
  *
  * An individual <code>JccCallEvent</code> conveys one of a series of
  * different possible Call state changes; the specific Call state
  * change is indicated by the {@link JccEvent#getID()} value returned
  * by the event.  <p>
  *
  * The {@link JccConnectionListener} interface extends this interface.
  * This reflects the fact that all Connection events can be reported
  * via the <code>JccCallListener</code> interface. <p>
  *
  * The {@link JccCallEvent#getCall()} method on this interface returns
  * the Call associated with the Call event.
  *
  * The JccConnectionListener interface extends this interface. This
  * reflects the fact that all {@link JccConnectionEvent}s and {@link
  * JccCallEvent}s events can be reported via the JccConnectionListener
  * interface.
  * @since 1.0
  */
public interface JccCallListener extends java.util.EventListener {
    /** Indicates that the supervision of the call has started.
      * @param callevent JccCallevent.  */
    public void callSuperviseStart(JccCallEvent callevent);
    /** Indicates that the supervision of the call has ended.
      * @param callevent JccCallevent.  */
    public void callSuperviseEnd(JccCallEvent callevent);
    /** Indicates that the state of the {@link JccCall} object has
      * changed to {@link JccCall#ACTIVE}.
      * @param callevent
      * JccCallEvent with eventID {@link JccCallEvent#CALL_ACTIVE}.
      * @since 1.0b */
    public void callActive(JccCallEvent callevent);
    /** Indicates that the state of the {@link JccCall} object has
      * changed to {@link JccCall#INVALID}.
      * @param callevent
      * JccCallEvent with eventID {@link JccCallEvent#CALL_INVALID}.
      * @since 1.0b */
    public void callInvalid(JccCallEvent callevent);
    /** This method is called to indicate that the application will no
      * longer receive JccCallEvent events of the call on the instance
      * of the JccCallListener.
      * @param callevent
      * JccCallEvent with eventID
      * {@link JccCallEvent#CALL_EVENT_TRANSMISSION_ENDED}.
      * @since 1.0b */
    public void callEventTransmissionEnded(JccCallEvent callevent);
    /** Indicates that the state of the JccCall object has changed to
      * {@link JccCall#IDLE}.
      * @param callevent JccCallEvent with eventID
      * {@link JccCallEvent#CALL_CREATED}.
      * @since 1.0b */
    public void callCreated(JccCallEvent callevent);
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
