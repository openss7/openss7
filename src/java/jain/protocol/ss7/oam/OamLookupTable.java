/* ***************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

package jain.protocol.ss7.oam;

import javax.management.*;

import jain.protocol.ss7.*;
import jain.*;

/**
  * This interface provides a mechanism for an application to map between a MBean's
  * ObjectName and a reference. <p>
  *
  * An application may operate with or without a JMX agent and the need for this
  * LookupTable arises when an application is running without JMX. Without JMX the
  * application needs a means to map from an ObjectName to an Object Reference. The
  * LookupTable provides this mapping. <p>
  *
  * The implementation should have a class that will implement the OamLookupTable and
  * everything can then be accessed through this class.  The OamLayerManagerMBean is
  * responsible for registering the MBean's ObjectName and reference with this class. And
  * also when the MBean is being deleted the corresponding entry in this class must be
  * removed.  The same OamLookupTable will be shared between all layer managers. <p>
  *
  * Note: An implementation may use any means to maintain these mappings.  This interface
  * just provides a convenient way to perform this.
  *
  * @version 1.1
  * @author Colm Hayden and Phelim O'Doherty
  */
public abstract interface OamLookupTable {
    /** Adds an ObjectName/object reference pair to the table.  */
    public void put(ObjectName name, java.lang.Object reference);
    /** Removes an ObjectName/object reference pair from the table.  */
    public void remove(ObjectName name);
    /** Returns a reference to the object with the supplied ObjectName.  */
    public java.lang.Object get(ObjectName name);
}


// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-

