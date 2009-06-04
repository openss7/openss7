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

package javax.jain.protocol.ss7.map;

import java.lang.Object;
import java.lang.Cloneable;
import java.io.Serializable;

/**
  * This class is the base class for all error events returned to the listener from the provider.
  */
public class MapErrorEvent extends java.util.EventObject implements Serializable, Cloneable {
    /**
      * The constructor.
      *
      * @param source The source of this event.
      * @param error A vendor-defined object containing the error.
      */
    public MapErrorEvent(Object source, Object error) {
        super(source);
        setError(error);
    }
    /**
      * Empty constructure needed for serializable objects and beans.
      */
    public MapErrorEvent() {
        super(null);
    }
    /**
      * Change the object contianing the error.
      * @param error The Object containing the error.
      */
    public void setError(Object error) {
        m_error = error;
        m_error_is_set = true;
    }
    /**
      * Get the object containing the error.
      * @return The Object containing the error.
      */
    public Object getError() {
        Object error = null;
        if (m_error_is_set)
            error = m_error;
        return error;
    }
    public Object getSource() {
        Object source = null;
        if (m_source_is_set)
            source = m_source;
        return source;
    }
    private Object m_source = null;
    private boolean m_source_is_set = false;
    private Object m_error = null;
    private boolean m_error_is_set = false;
}


// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-

