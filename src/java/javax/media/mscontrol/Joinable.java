/*
 @(#) $RCSfile: Joinable.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:20 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:20 $ by $Author: brian $
 */

package javax.media.mscontrol;

/** A Joinable is suitable for media composition.  A Joinable object is
  * either a NetworkConnection, a MediaGroup, a VxmlDialog, or a
  * MediaMixer (base of a Conference application).  It can also be a
  * Stream (audio, video, ...) of any of those objects.  Joinables can
  * be joined/unjoined, to make the media stream flow between them. Join
  * operations can be limited both to a stream (i.e audio,video, see
  * JoinableStream.StreamType) and direction (i.e. SEND, RECV, DUPLEX,
  * see Joinable.Direction).  The join direction can be changed by
  * calling join again, with a different Direction.
  */
public interface Joinable {
    /** @param direction
      * Filter on the connection direction.
      * @return
      * An array of other Joinables to which we are joined,
      * exclusive/restrictive list: Only the Joinables with at least the
      * same streams are listed. (e.g. if this Joinable has both audio
      * and video stream, only Joinables connected both with audio and
      * video are listed).  If direction is SEND, then all objects
      * joined in SEND or DUPLEX are included.  If direction is RECV,
      * then only the objects joined in RECV or DUPLEX are included.  If
      * direction is DUPLEX, then only the objects joined in DUPLEX are
      * included.  The result includes only the Joinables directly
      * connected to us (does not jump across MediaMixers).
      * @exception MscontrolException */
    Joinable[] getJoinees(Joinable.Direction direction)
        throws MscontrolException;
    /** @return
      * Other Joinables to which we are connected, on at least one
      * stream (possibly more), and at least in one direction (Inclusive
      * list).  The result includes only the Joinables directly
      * connected to us (does not jump across MediaMixers).
      * @exception MscontrolException */
    Joinable[] getJoinees()
        throws MscontrolException;
    /** Establish a media stream between this object and other.  The
      * Direction argument indicates a direction; The order of the
      * arguments helps to remember which is the origin and which is the
      * destination. For example: objectA.join(Direction.SEND, objectB);
      * means that objectA sends to objectB The result is strictly
      * equivalent to objectB.join(Direction.RECV, objectA).  If objectA
      * is a MediaMixer (implementing a conference) and objectB is a
      * NetworkConnection (representing a conference participant), this
      * means that the participant is "muted".
      * <h5>Joining again the same pair of objects ("re-joining")</h5> 
      * The given direction replaces a possibly existing relationship
      * between the objects.  For example: ObjectA.join(REVC, ObjectB)
      * followed by ObjectA.join(SEND, ObjectB) results in ObjectA
      * sending to ObjectB (not duplex, the SEND direction is not
      * "added" to the RECV direction).
      * <h5>Joining an object to multiple other objects</h5>
      * The application is allowed to join objectA to objectB, objectC,
      * etc. The resulting relationship is: <ul>
      * <li>objectA will send data to all others (broadcast to objectB, objectC, etc.)
      * <li>objectA will only receive from the last joined object </ul>
      * For example: <ul>
      * <li> ObjectA.join(DUPLEX, ObjectB)
      * <li> ObjectA.join(DUPLEX, ObjectC)
      * <li> ObjectA.join(DUPLEX, ObjectD)
      * <li> ObjectA sends the same stream(s) to ObjectB, ObjectC and ObjectD.
      * <li> ObjectA only receives from ObjectD. </ul>
      * <h5>joining across MediaSessions</h5>
      * It is allowed to join a MediaObject that has been created by an
      * other MediaSession.
      * @param aDirection
      * Indicates direction (DUPLEX, SEND, RECV).
      * @param other
      * Joinable object to connect.
      * @exception MscontrolException */
    void join(Joinable.Direction aDirection, Joinable other)
        throws MscontrolException;
    /** Disconnect any media streams flowing between this object and
      * other's. <p>
      * Note: Changing the direction (e.g. changing from DUPLEX to
      * RECV), is obtained by calling join again, with the desired
      * direction.
      * @param other
      * Joinable object to disconnect.
      * @exception MscontrolException */
    void unjoin(Joinable other)
        throws MscontrolException;
    /** Initiates the same action as
      * join(javax.media.mscontrol.Joinable.Direction, Joinable), but
      * returns immediately, without waiting for the join completion
      * (asynchronous operation). The application is later notified of
      * the completion by a JoinEvent sent to the JoinableContainer's
      * listener, or the JoinableDialog's listener.  In the case of a
      * JoinableStream, the listener of the parent JoinableContainer is
      * called.
      * @param aDirection
      * See the join method.
      * @param other
      * See the join method.
      * @param context
      * An opaque object, that will be provided in the completion event.
      * @exception MscontrolException */
    void joinInitiate(Joinable.Direction aDirection,
            Joinable other, java.io.Serializable context)
        throws MscontrolException;
    /** Asynchronous version of unjoin. <p>
      * See unjoin(Joinable) and
      * joinInitiate(javax.media.mscontrol.Joinable.Direction, Joinable,
      * Serializable).
      * @exception MscontrolException */
    void unjoinInitiate(Joinable other,
            java.io.Serializable context)
        throws MscontrolException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
