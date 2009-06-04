//  ==========================================================================
//  
//  @(#) $Id$
//  
//  --------------------------------------------------------------------------
//  
//  Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
//  Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
//  Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
//  
//  All Rights Reserved.
//  
//  This program is free software; you can redistribute it and/or modify it
//  under the terms of the GNU Affero General Public License as published by
//  the Free Software Foundation; version 3 of the License.
//  
//  This program is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
//  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public
//  License for more details.
//  
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>, or
//  write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
//  02139, USA.
//  
//  --------------------------------------------------------------------------
//  
//  U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
//  behalf of the U.S. Government ("Government"), the following provisions
//  apply to you.  If the Software is supplied by the Department of Defense
//  ("DoD"), it is classified as "Commercial Computer Software" under
//  paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
//  Regulations ("DFARS") (or any successor regulations) and the Government is
//  acquiring only the license rights granted herein (the license rights
//  customarily provided to non-Government users).  If the Software is
//  supplied to any unit or agency of the Government other than DoD, it is
//  classified as "Restricted Computer Software" and the Government's rights
//  in the Software are defined in paragraph 52.227-19 of the Federal
//  Acquisition Regulations ("FAR") (or any successor regulations) or, in the
//  cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
//  (or any successor regulations).
//  
//  --------------------------------------------------------------------------
//  
//  Commercial licensing and support of this software is available from
//  OpenSS7 Corporation at a fee.  See http://www.openss7.com/
//  
//  --------------------------------------------------------------------------
//  
//  Last Modified $Date$ by $Author$
//  
//  --------------------------------------------------------------------------
//  
//  $Log$
//  ==========================================================================

package javax.jain.protocol.ss7.map.position;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/** This interface defines the methods required to represent a proprietary JAIN MAP
  * protocol stack for the MS Position Capability, the implementation of which will be
  * vendor specific. The object that implements the JainMapMsPosStack interface is
  * referred to as JainMapMsPosStackImpl. <p>
  *
  * Each vendor's protocol stack will have an object that implements this interface to
  * control the creation/deletion of proprietary implementations of the interface
  * MsPosProvider. The objects that implement the MsPosProvider interface are referred to
  * as MsPosProviderImpl. <p>
  *
  * An application may create a JainMapMsPosStackImpl by invoking the method
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
public interface JainMapMsPosStack extends JainMapStack {
    /** Creates a new Peer (vendor specific) instance of the class MsPosProviderImpl that
      * is attached to this instance of JainMapMsPosStackImpl.
      * @return A reference to a new instance of MsPosProviderImpl.
      * @exception SS7PeerUnavailableException  Thrown if the provider instance cannot be
      * created.
      */
    public MsPosProvider createMsPosProvider()
        throws SS7PeerUnavailableException;
    /** Deletes a specified MsPosProviderImpl attached to this instance of
      * JainMapMsPosStackImpl.
      * @param provider  The provider instance to be deleted.
      */
    public void deleteMsPosProvider(MsPosProvider provider);
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
