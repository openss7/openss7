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

package javax.csapi.cc.jcc;

/** The JccPeer interface represents a vendor's particular
  * implementation of the JCC API. <p>
  *
  * Other interfaces derived from JCC are expected to implement this
  * interface. The JccPeer object, returned by the
  * JccPeerFactory.getJccPeer(java.lang.String) method, determines which
  * JccProviders are made available to the application. <p>
  *
  * Applications use the getProvider(java.lang.String) method on this interface to
  * obtain new JccProvider objects. Each implementation may support one
  * or more different "services". A list of available services can be
  * obtained via the getServices() method. <p>
  *
  * <h5>Obtaining a JccProvider</h5>
  * Applications use the getProvider(java.lang.String) method on this interface to
  * obtain new JccProvider objects. Each implementation may support one
  * or more different "services" (e.g. for different types of underlying
  * network substrate). A list of available services can be obtained via
  * the getServices() method. <p>
  *
  * Applications may also supply optional arguments to the JccProvider
  * through the getProvider(java.lang.String) method. These arguments are appended
  * to the providerString argument passed to the getProvider(java.lang.String)
  * method. The providerString argument has the following format: <p>
  *
  * <code>&lt; service name &gt; ; arg1 = val1; arg2 = val2; ...</code>
  * <p>
  *
  * Where <code>&lt; service name &gt;</code> is not optional, and each
  * optional argument pair which follows is separated by a semi-colon.
  * The keys for these arguments is implementation specific, except for
  * two standard-defined keys: <ol>
  * <li>login: provides the login user name to the Provider.
  * <li>passwd: provides a password to the Provider.</ol>
  * @since 1.0b
  * @version 1.2.2
  * @author Monavacon Limited
  */
public interface JccPeer {
    /** Returns the name of this JccPeer object instance. The name is
      * the same name used as an argument to
      * JccPeerFactory.getJccPeer(java.lang.String) method.
      * @return
      * The name of this JccPeer object instance.  */
    public java.lang.String getName();
    /** Returns the services that this implementation supports. This
      * method returns null if no services are supported.
      * @return
      * Services that this implementation supports.  */
    public java.lang.String[] getServices();
    /** Returns an instance of a Provider object given a string argument
      * which contains the desired service name. Optional arguments may
      * also be provided in this string, with the following format:<p>
      *
      * <code>&lt; service name &gt; ; arg1 = val1; arg2 = val2; ...</code> <p>
      *
      * Where <code>&lt; service name &gt; is not optional, and each
      * optional argument pair which follows is separated by a
      * semi-colon. The keys for these arguments is implementation
      * specific, except for two standard-defined keys: <ol>
      * <li>login: provides the login user name to the Provider.
      * <li>passwd: provides a password to the Provider.</ol>
      *
      * If the argument is null, this method returns some default
      * Provider as determined by the JccPeer object. The returned
      * Provider is not in the JccProvider.SHUTDOWN state.Note that this
      * may also result in the application obtaining a reference to a
      * Provider which has already been created. <p>
      *
      * Post-conditions:<ol>
      * <li>this.getProvider().getState() != SHUTDOWN </ol>
      * @param providerString
      * The name of the desired service.
      * @return
      * An instance of the JccProvider object.
      * @exception ProviderUnavailableException
      * Indicates a Provider corresponding to the given string is
      * unavailable.  */
    public JccProvider getProvider(java.lang.String providerString)
        throws ProviderUnavailableException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
