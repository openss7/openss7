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

package javax.jain.protocol.ss7.map.position.etsipar;

import javax.jain.protocol.ss7.map.position.*;
import javax.jain.protocol.ss7.map.*;
import javax.jain.protocol.ss7.*;
import javax.jain.*;

/**
  * This parameter class represents an International Mobile Station
  * Equipment Identity (IMEI) as defined in the specifications 3G TS
  * 23.003 and 3G TS 22.016. <p>
  *
  * The following rules applies for the setting of parameter components:
  * <ul>
  *
  * <li>Mandatory component with no default value must be provided to
  * the constructor.
  *
  * <li>Mandatory component with default value need not be set.
  *
  * <li>Optional / conditional component is by default not present. Such
  * a component becomes present by setting it. </ul>
  *
  * <h5>Parameter components:</h5><ul>
  * <li>tac - Type Approval Code, mandatory component with no default
  * <li>fac - Final Assembly Code, mandatory component with no default
  * <li>snr - Serial Number, mandatory component with no default
  * <li>svn - Software Version Number, optional component </ul>
  */
public class Imei extends EquipmentId {
    /**
      * The constructor.
      *
      * @param tac
      * Type Approval Code (6 digits)
      *
      * @param fac
      * Final Assembly Code (2 digits)
      *
      * @param snr
      * Serial Number (6 digits)
      *
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public Imei(String tac, String fac, String snr)
        throws SS7InvalidParamException {
        setTypeApprovalCode(tac);
        setFinalAssemblyCode(fac);
        setSerialNumber(snr);
    }
    /** Empty constructor; needed for serializable objects and beans.  */
    public Imei()
        throws SS7InvalidParamException {
    }
    /**
      * Change the value of the Type Approval Code (TAC).
      *
      * @param tac
      * String with 6 digits TAC.
      */
    public void setTypeApprovalCode(String tac)
        throws SS7InvalidParamException {
        m_tac = tac;
        m_tacIsSet = (tac != null);
    }
    /**
      * Get the value of the Type Approval Code (TAC).
      *
      * @return
      * String with 6 digits TAC.
      */
    public String getTypeApprovalCode() {
        return m_tac;
    }
    /**
      * Change the value of the Final Assembly Code( FAC).
      *
      * @param fac
      * String with 2 digits FAC.
      */
    public void setFinalAssemblyCode(String fac)
        throws SS7InvalidParamException {
        m_fac = fac;
        m_facIsSet = (fac != null);
    }
    /**
      * Get the value of the Final Assembly Code( FAC).
      *
      * @return
      * String with 2 digits FAC.
      */
    public String getFinalAssemblyCode() {
        return m_fac;
    }
    /**
      * Change the value of the Serial Number (SNR).
      *
      * @param snr
      * String with 6 digits SNR.
      */
    public void setSerialNumber(String snr)
        throws SS7InvalidParamException {
        m_snr = snr;
        m_snrIsSet = (snr != null);
    }
    /**
      * Get the value of the Serial Number (SNR).
      *
      * @return
      * String with 6 digits SNR.
      */
    public String getSerialNumber() {
        return m_snr;
    }
    /**
      * Set the value of the Software Version Number (SVN).
      *
      * @param svn
      * String with 1 digit SVN.
      */
    public void setSoftwareVersionNumber(String svn)
        throws SS7InvalidParamException {
        m_svn = svn;
        m_svnIsSet = (svn != null);
    }
    /**
      * Get the value of the Software Version Number (SVN).
      * It shall be checked if this parameter is present before getting
      * it.
      *
      * @return
      * String with 1 digit SVN.
      */
    public String getSoftwareVersionNumber() {
        return m_svn;
    }
    /**
      * Check if the Software Version Number (SVN) is present.
      *
      * @return
      * True or false.
      */
    public boolean isSoftwareVersionNumberPresent() {
        return m_svnIsSet;
    }
    /**
      * Get the whole IMEI (TAC+FAC+SNR+SVN) as a String.
      *
      * @return
      * String containing the IMEI.
      */
    public String toString() {
        return new String("");
    }
    /**
      * Compares this IMEI with another.
      *
      * @param imei
      * IMEI to be compared with this IMEI.
      *
      * @return
      * True if equal, otherwise false.
      */
    public boolean equals(Object imei) {
        if (imei instanceof Imei) {
            Imei other = (Imei) imei;
            if (m_tac != other.m_tac)
                return false;
            if (m_tacIsSet != other.m_tacIsSet)
                return false;
            if (m_fac != other.m_fac)
                return false;
            if (m_facIsSet != other.m_facIsSet)
                return false;
            if (m_snr != other.m_snr)
                return false;
            if (m_snrIsSet != other.m_snrIsSet)
                return false;
            if (m_svn != other.m_svn)
                return false;
            if (m_svnIsSet != other.m_svnIsSet)
                return false;
        }
        return false;
    }
    protected String m_tac;
    protected boolean m_tacIsSet = false;
    protected String m_fac;
    protected boolean m_facIsSet = false;
    protected String m_snr;
    protected boolean m_snrIsSet = false;
    protected String m_svn;
    protected boolean m_svnIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-

