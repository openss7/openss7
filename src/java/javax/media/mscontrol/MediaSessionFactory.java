/*
 @(#) src/java/javax/media/mscontrol/MediaSessionFactory.java <p>
 
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

package javax.media.mscontrol;

/** This factory creates MediaSessions.  It creates also the objects
  * that are not tied to a particular MediaSession.  (The objects that
  * have a relationship to MediaSession are created by the MediaSession
  * itself.) Note: The internals of this factory are
  * implementation-dependant, and for this reason it is provided as an
  * interface. As a result, another class is required to build it:
  * MscontrolFactory (similarly to the JAIN family fo interfaces).
  * @version 1.2.2
  * @author Monavacon Limited
  */
public interface MediaSessionFactory {
    /** @exception MscontrolException */
    MediaSession createMediaSession(Parameters params)
        throws MscontrolException;
    /** @exception MscontrolException */
    MediaSession createMediaSession()
        throws MscontrolException;
    /** Return an instance of predefined MediaConfig, as described by
      * the given Symbol.
      * Type Parameters: C - a typed derived from MediaConfig
      * @param aConfigSymbol
      * A Symbol identifying a predefined config, for example
      * MediaGroupConfig#c_IVR.
      * @return
      * An instance of a class derived from MediaConfig, with the
      * expected type.
      * @exception MediaConfigException
      * @see getMediaConfig(Class, Reader) */
    <C extends MediaConfig> C getMediaConfig(ConfigSymbol<C> aConfigSymbol)
        throws MediaConfigException;
    /** Create an instance of MediaConfig, from an xml byte stream (for
      * example a file, or a java.lang.String). <p>
      * Example: <br>
      * <blockquote><code>
      * Reader xmlDoc = new StringReader("&lt;?xml .... &gt;");
      * NetworkConnectionConfig myNCConfig = myMediaSessionFactory.getMediaConfig(NetworkConnectionConfig.class, xmlDoc);
      * </code></blockquote> <p>
      * The schema for the xml document is provided by the media-server
      * vendor.
      * Type Parameters: C - a typed derived from MediaConfig
      * @param aConfigClass
      * A Class object, defining the exact type of the returned object.
      * @param xmlDef
      * A Reader able to provide an xml byte stream.
      * @return
      * An instance of a class derived from MediaConfig, with the
      * expected type.
      * @exception MediaConfigException
      * @see getMediaConfig(ConfigSymbol) */
    <C extends MediaConfig> C getMediaConfig(java.lang.Class<C> aConfigClass, java.io.Reader xmlDef)
        throws MediaConfigException;
    /** Create an empty Parameters map.  */
    Parameters createParameters();
    /** Factory method for creating a instance of VideoLayout.
      * @param mimeType
      * For example, "application/smil+xml".
      * @param xmlDef
      * A byte stream providing an xml definition for the layout
      * @exception MediaConfigException */
    VideoLayout createVideoLayout(java.lang.String mimeType, java.io.Reader xmlDef)
        throws MediaConfigException;
    /** Returns an array of mixer VideoLayouts that are supported by the
      * mediaserver. <br>
      * Each entry in the array may differ from the previous by the size
      * of the regions, decoration, etc.  Simple layouts should have low
      * indexes.  Some media servers may support only those layouts.
      * Some media servers may support only minor modifications to those
      * layouts: for example, structural modifications disallowed (only
      * attributes values can be changed).  The array may have a null
      * size.  Note: This is a helper method for implementations or
      * media servers that cannot provide full support of an xml format.
      * @param numberOfLiveRegions
      * The number of regions, in the rendered image, that can display a
      * live stream (for instance, a NetworkConnection, or a MediaGroup
      * playing a clip).
      * @exception MediaConfigException */
    VideoLayout[] getPresetLayouts(int numberOfLiveRegions)
        throws MediaConfigException;
    /** Returns a VideoLayout (supported by the mediaserver), with the
      * characteristics as defined by type.
      * @param type
      * A string to further describe the desired layout, see
      * VideoLayout#getType().
      * @exception MediaConfigException
      * @see getPresetLayouts(int) */
    VideoLayout getPresetLayout(java.lang.String type)
        throws MediaConfigException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
