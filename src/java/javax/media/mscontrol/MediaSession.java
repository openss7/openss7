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

/** A MediaSession is a container and factory for media objects. It
  * handles the cleanup on release().
  * @version 1.2.2
  * @author Monavacon Limited
  */
public interface MediaSession extends MediaObject {
    /** Integer value, in milliseconds, after which an API call (on the
      * objects created by this MediaSession) should timeout.  This can
      * be caused in particular by lengthy/failing call to a remote
      * media server.  The API call will throw TimeoutException in this
      * case.  */
    static final Symbol p_Timeout;
    /** Allows the application to pass to the implementation a hint
      * regarding the best suited Media Server based on capabilities,
      * resource brokering or any other mechanism.  The associated value
      * is a java.lang.String.  This parameter should be added to the Parameters
      * argument of MediaSessionFactory.createMediaSession(Parameters).
      */
    static final Symbol p_MediaServerHint;
    /** Create an instance of one of the ResourceContainer derived
      * objects. <p>
      * The containers life cycle is under control of the application.
      * It means that the application has to release the containers,
      * when no longer used. However, containers are attached to a
      * MediaSession, consequently they are automatically released when
      * the MediaSession is released. <p>
      * Examples: <br>
      * <blockquote><code>
      * NetworkConnection myNC = myMediaSession.createContainer(NetworkConnectionConfig.c_Basic);
      * MediaGroup myMediaGoup = myMediaSession.createContainer(MediaGroupConfig.c_IVR);
      * </code></blockquote> <p>
      * <h5>Type Parameters:</h5> <dl>
      * <dt>C</dt><dd>a config class, that determines the type of the
      * returned container (T)</dd>
      * <dt>T</dt><dd>one of the ResourceContainer types, among
      * MediaGroup, NetworkConnection, or MediaMixer (To create a
      * MixerAdapter, see BasicMediaMixer#createMixerAdapter).</dd></dl>
      * @param aPredefinedConfig
      * A Symbol identifying a predefined container configuration.
      * @return
      * A MediaGroup, a NetworkConnection, a MediaMixer or a
      * MixerAdapter.
      * @exception MscontrolException
      * @see createContainer(Class, Parameters)
      * @see createContainer(MediaConfig, java.lang.String) */
    <C extends MediaConfig,T extends ResourceContainer<? extends C>> T createContainer(ConfigSymbol<C> aPredefinedConfig)
        throws MscontrolException;
    /** Create a ResourceContainer, providing a set of discrete attributes. <p>
      * Example: <br>
      * Create a Mixer without video capabilities. The other Mixer
      * characteristics are unspecified and will take the implementation
      * default values. <br>
      * <blockquote><code>
      * Parameters attribs = myMsFactory.createParameters();
      * attribs.put(MixerConstants.a_VideoMixer, false);
      * MediaMixer myMixer = myMs.createContainer(MediaMixer.class, attribs);
      * </code></blockquote>
      * <h5>Type Parameters:</h5> <dl>
      * <dt>T</dt><dd> one of the ResourceContainer types.</dd>
      * @param aClass
      * A Class object, defining the exact type of the container.
      * @param params
      * A set of parameters defining the resources required in the
      * container, and possibly default values for some parameters of
      * these resources.  A containerId may be specified using
      * a_MediaObjectId
      * @return
      * A MediaGroup, a NetworkConnection, a MediaMixer or a
      * MixerAdapter.
      * @exception MscontrolException
      * @see createContainer(MediaConfig, java.lang.String)
      * @see createContainer(ConfigSymbol) */
    <T extends ResourceContainer<? extends MediaConfig>> T createContainer(java.lang.Class<T> aClass, Parameters params)
        throws MscontrolException;
    /** <h5>Type Parameters:</h5> <dl>
      * <dt> C </dt><dd> a config class, that determines the type of the
      * returned container (T) </dd>
      * <dt> T </dt><dd> one of the ResourceContainer types </dd> </dl>
      * @param aConfig
      * An instance of MediaConfig. See
      * MediaSessionFactory.getMediaConfig(ConfigSymbol) or
      * MediaSessionFactory.getMediaConfig(Class, java.io.Reader) to
      * create one.
      * @param aContainerId
      * An application-provided Id, to replace the system-generated one.
      * @return
      * A MediaGroup, a NetworkConnection, a MediaMixer or a
      * MixerAdapter.
      * @exception MscontrolException
      * @see createContainer(Class, Parameters)
      * @see createContainer(ConfigSymbol)
      */
    <C extends MediaConfig,T extends ResourceContainer<? extends C>> T createContainer(C aConfig, java.lang.String aContainerId)
        throws MscontrolException;
    /** A VxmlDialog is a Joinable object that can interpret a vxml script.
      * @param parameters
      * @exception MscontrolException */
    VxmlDialog createVxmlDialog(Parameters parameters)
        throws MscontrolException;
    /** Return the object associated to a name from this session.
      * @param name of the object */
    java.lang.Object getAttribute(java.lang.String name);
    /** Removes the object associated to a name from this session.
      * @param name name of the associated object */
    void removeAttribute(java.lang.String name);
    /** Set an object to this session for a given name.
      * @param name bound name
      * @param attribute bound attribute */
    void setAttribute(java.lang.String name, java.lang.Object attribute);
    /** Returns an Enumeration containing the names of all the objects
      * associated to this session.
      * @return
      * Enumeration of name.  */
    java.util.Enumeration<java.lang.String> getAttributeNames();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
