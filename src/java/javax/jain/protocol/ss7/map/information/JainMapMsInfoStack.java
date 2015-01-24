/*
 @(#) src/java/javax/jain/protocol/ss7/map/information/JainMapMsInfoStack.java <p>
 
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

package javax.jain.protocol.ss7.map.information;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/** This interface defines the methods required to represent a proprietary JAIN MAP
  * protocol stack for the MS Information Capability, the implementation of which will be
  * vendor specific. The object that implements the JainMapMsInfoStack interface is
  * referred to as JainMapMsInfoStackImpl. <p>
  *
  * Each vendor's protocol stack will have an object that implements this interface to
  * control the creation/deletion of proprietary implementations of the interface
  * MsInfoProvider. The objects that implement the MsInfoProvider interface are referred
  * to as MsInfoProviderImpl. <p>
  *
  * An application may create a JainMapMsInfoStackImpl by invoking the method
  * createSS7Object on a JainSS7Factory object. The path name of the vendor specific
  * implementation which you want to instantiate needs to be set on the JainSS7Factory
  * object using the setPathName method before calling the createSS7Object method. The
  * parameter objectClassName of the createSS7Object method shall specify the package name
  * and class name within the scope of the path defined by means of the setPathName
  * method.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public interface JainMapMsInfoStack extends JainMapStack {
    /** Creates a new Peer (vendor specific) instance of the class MsInfoProviderImpl that
      * is attached to this instance of JainMapMsInfoStackImpl.
      * @return A reference to a new instance of MsInfoProviderImpl.
      * @exception SS7PeerUnavailableException  Thrown if the provider instance cannot be
      * created.
      */
    public MsInfoProvider createMsInfoProvider()
        throws SS7PeerUnavailableException;
    /** Deletes a specified MsInfoProviderImpl attached to this instance of
      * JainMapMsInfoStackImpl.
      * @param provider  The provider instance to be deleted.
      */
    public void deleteMsInfoProvider(MsInfoProvider provider);
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
