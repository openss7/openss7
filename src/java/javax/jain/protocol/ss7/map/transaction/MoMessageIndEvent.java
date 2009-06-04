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

package javax.jain.protocol.ss7.map.transaction;

import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

public final class MoMessageIndEvent extends TransactionEvent {
    public MoMessageIndEvent(Object source, MsNumber msId,
            Message message, SS7Number destinationAddress)
        throws SS7InvalidParamException {
        super(source, MO_MESSAGE_IND_ID);
        setMsId(msId);
        setMessage(message);
        setDestinationAddress(destinationAddress);
    }
    public MoMessageIndEvent()
        throws SS7InvalidParamException {
        super(null, MO_MESSAGE_IND_ID);
    }
    public void setMsId(MsNumber msId)
        throws SS7InvalidParamException {
        // FIXME: check validity of msId
        if (msId != null) {
            m_msId = msId;
            m_msIdIsSet = true;
        } else {
            m_msId = null;
            m_msIdIsSet = false;
        }
    }
    public MsNumber getMsId() {
        return m_msId;
    }
    public void setMessage(Message message)
        throws SS7InvalidParamException {
        if (message != null) {
            m_message = message;
            m_messageIsSet = true;
        } else {
            m_message = null;
            m_messageIsSet = false;
        }
    }
    public Message getMessage() {
        return m_message;
    }
    public void setDestinationAddress(SS7Number destinationAddress)
        throws SS7InvalidParamException {
        if (destinationAddress != null) {
            m_destinationAddress = destinationAddress;
            m_destinationAddressIsSet = true;
        } else {
            m_destinationAddress = null;
            m_destinationAddressIsSet = false;
        }
    }
    public SS7Number getDestinationAddress() {
        return m_destinationAddress;
    }
    protected MsNumber m_msId = null;
    protected boolean m_msIdIsSet = false;
    protected Message m_message = null;
    protected boolean m_messageIsSet = false;
    protected SS7Number m_destinationAddress = null;
    protected boolean m_destinationAddressIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
