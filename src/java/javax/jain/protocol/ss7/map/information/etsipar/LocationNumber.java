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

package javax.jain.protocol.ss7.map.information.etsipar;

import javax.jain.protocol.ss7.map.information.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/** This parameter class represents a Location Number which has the format of an E.164
  * Number. Therefore this class extends E164Number class. <p>
  *
  * Parameter components:
  * <li>(No parameters are added in this class. Subscriber Number (SN) is renamed to
  * Locally Significant Part (LSP).)
  */
public class LocationNumber extends E164Number {
    /** The constructor.
      * @param cc  Country Code
      * @param lsp  Locally Significant Part (mapped to Subscriber Number)
      * @exception SS7InvalidParamException  Thrown if parameter(s) are invalid / out of
      * range.
      */
    public LocationNumber(String cc, String lsp)
        throws SS7InvalidParamException {
        if (cc == null)
            throw new SS7InvalidParamException("Country Code null.");
        m_cc = cc;
        setLocallySignificantPart(lsp);
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public LocationNumber()
        throws SS7InvalidParamException {
    }
    /** Change the value of the Locally Significant Part (LSP). (This method is identical
      * to setSubscriberNumber, just with a more appropriate name in the context of
      * LocationNumber.)
      * @param lsp  String with LSP.
      */
    public void setLocallySignificantPart(String lsp)
        throws SS7InvalidParamException {
        m_lsp = lsp;
    }
    /** Get the value of the Locally Significant Part (LSP). (This method is identical to
      * getSubscriberNumber, just with a more appropriate name in the context of
      * LocationNumber.)
      * @return String with LSP.
      */
    public String getLocallySignificantPart() {
        return m_lsp;
    }
    protected String m_cc;
    protected String m_lsp;
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
