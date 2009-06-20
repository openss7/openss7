/*
 @(#) $RCSfile$ $Name$($Revision$) $Date$ <p>
 
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
 
 Last Modified $Date$ by $Author$
 */

package javax.media.mscontrol;

/** A base class for objects that have modifiable parameters.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public interface MediaObject {
    /** A non-mutable ID string, replacing the system-provided ID.  The
      * value must be a java.lang.String starting with a letter or a number.  This
      * attribute can be used in the Parameters argument of the
      * createContainer method, or the createVxmlDialog method.  */
    static final Symbol a_MediaObjectId;
    /** Set the value of various parameters for the media object. <p>
      * For each key in the supplied Parameters map, the corresponding
      * parameter of the media object is set to the value of that key in
      * the map. For sanity, the keys in the Parameters map are
      * restricted to type Symbol. The compiler will not accept keys of
      * another type. <p>
      * If a parameter Symbol refers to a resource that is not present,
      * or the media object has no meaning associated with a particular
      * Symbol, the setting for that parameter is quietly ignored.
      * @param params
      * A Parameters map in which the keys are Symbols and the values
      * are assigned to the parameter with that Symbol name.  */
    void setParameters(Parameters params);
    /** Gets the value of various parameters from the media object. <p>
      * Each entry in the returned Parameters map holds the value of the
      * corresponding parameter. If the argument is null, then get all
      * parameters suported by this object. For example: <p>
      * <blockquote><code>
      * Parameters parms = getParameters(null);
      * </code></blockquote> <p>
      * If a key refers to a Resource that is not present, or the
      * resource has no meaning associated with a particular Symbol, the
      * key is ignored, no error is generated, and that key does not
      * appear in the returned Parameters map. <p>
      * Note: This can be used to know the list of parameters supported
      * by a given object.
      * @param params
      * An array of Symbols naming parameters to be retrieved.
      * @return
      * A Parameters map of parameter Symbols and their values. */
    Parameters getParameters(Symbol[] params);
    /** Return a URI that uniquely identifies this object. <p>
      * Example: <br>
      * <blockquote>
      * <code>mscontrol:////MediaGroup.mg7348</code>
      * </blockquote>
      * <h5>Generic URI structure:</h5>
      * <code>mscontrol://&lt;host&gt;/&lt;mediaSessionID&gt;/{MediaGroup|NetworkConnection}.&lt;containerID&gt;/{in|out}/{audio|video|message}</code><br>
      * <code>mscontrol://&lt;host&gt;/&lt;mediaSessionID&gt;/Mixer.&lt;containerID&gt;/MixerAdapter.&lt;containerID&gt;/{in|out}/{audio|video|message}</code><br>
      * <code>mscontrol://&lt;host&gt;/&lt;mediaSessionID&gt;/Mixer.&lt;containerID&gt;/StreamGroup.&lt;streamGroupID&gt;/{in|out}/{audio|video|message}</code><br>
      * <h5>Notes:</h5> <ul>
      * <li>When upper parts are omitted, as in
      * <code>mscontrol:////MediaGroup.mg7348</code>, the mediaserver
      * and MediaSession are determined (implicitly) by the context.
      * <li>Trailing parts are optional, and indicate a direction and a
      * stream name (they are not part of the URI returned by this
      * method). </ul> */
    java.net.URI getURI();
    /** Unjoin from all other media objects, and release the resources
      * associated to this media object.  The call is cascaded to the
      * children of this object (the objects created by it).
      */
    void release();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
