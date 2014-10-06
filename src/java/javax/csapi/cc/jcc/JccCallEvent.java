/*
 @(#) $RCSfile: JccCallEvent.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:33 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:33 $ by $Author: brian $
 */

package javax.csapi.cc.jcc;

/** This is the base interface for all {@link JccCall}-related events.
  * @version 1.2.2
  * @author Monavacon Limited
  * @since 1.0 */
public interface JccCallEvent extends JccEvent {
    /** The CALL_SUPERVISE_START event indicates that the supervision of
      * the call has started.  The policy followed is that given in
      * {@link JccCall#superviseCall(JccCallListener,double,int)}.  */
    public static final int CALL_SUPERVISE_START = 111;
    /** The CALL_SUPERVISE_END event indicates that the supervision of
      * the call has ended.  The policy followed is that given in {@link
      * JccCall#superviseCall(JccCallListener,double,int)}.  */        
    public static final int CALL_SUPERVISE_END = 112;
    /** Returns the JccCall object associated with this event.
      * @return JccCall
      * Represents the JccCall object associated with this JccCall
      * event. The result of this call might be a cast of the result of
      * {@link JccEvent#getSource()}.
      * @since 1.0b */
    public JccCall getCall();
    /** The CALL_ACTIVE event indicates that the state of the Call
      * object has changed to {@link JccCall#ACTIVE}.  This constant
      * corresponds to a specific call state change, it is passed via a
      * JccCallEvent event and is reported to the
      * {@link JccCallListener#callActive(JccCallEvent)} method.
      * @since 1.0b */
    public static int CALL_ACTIVE = 101;

    /** The CALL_INVALID event indicates that the state of the JccCall
      * object has changed to {@link JccCall#INVALID}.  This constant
      * corresponds to a specific call state change, it is passed via a
      * JccCallEvent event and is reported to the {@link
      * JccCallListener#callInvalid(JccCallEvent)} method.
      * @since 1.0b */
    public static int CALL_INVALID = 102;
    /** The CALL_EVENT_TRANSMISSION_ENDED event indicates that the
      * application will no longer receive JccCall events from this call
      * on the instance of the JccCallListener.  This constant is passed
      * via a JccCallEvent event and is reported to the {@link
      * JccCallListener#callEventTransmissionEnded(JccCallEvent)}
      * method.
      * @since 1.0b */
    public static int CALL_EVENT_TRANSMISSION_ENDED = 103;
    /** The CALL_CREATED event indicates that the JccCall object has
      * been created and is in the {@link JccCall#IDLE} state.  This
      * constant corresponds to a specific call state change, it is
      * passed via a JccCallEvent event and is reported to the {@link
      * JccCallListener#callCreated(JccCallEvent)} method.
      * @since 1.0b */
    public static int CALL_CREATED = 118;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
