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
  *
  * @author Monavacon Limited
  * @version 1.2.2
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
    public Imei(java.lang.String tac, java.lang.String fac, java.lang.String snr)
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
      * java.lang.String with 6 digits TAC.
      */
    public void setTypeApprovalCode(java.lang.String tac)
        throws SS7InvalidParamException {
        m_tac = tac;
        m_tacIsSet = (tac != null);
    }
    /**
      * Get the value of the Type Approval Code (TAC).
      *
      * @return
      * java.lang.String with 6 digits TAC.
      */
    public java.lang.String getTypeApprovalCode() {
        return m_tac;
    }
    /**
      * Change the value of the Final Assembly Code( FAC).
      *
      * @param fac
      * java.lang.String with 2 digits FAC.
      */
    public void setFinalAssemblyCode(java.lang.String fac)
        throws SS7InvalidParamException {
        m_fac = fac;
        m_facIsSet = (fac != null);
    }
    /**
      * Get the value of the Final Assembly Code( FAC).
      *
      * @return
      * java.lang.String with 2 digits FAC.
      */
    public java.lang.String getFinalAssemblyCode() {
        return m_fac;
    }
    /**
      * Change the value of the Serial Number (SNR).
      *
      * @param snr
      * java.lang.String with 6 digits SNR.
      */
    public void setSerialNumber(java.lang.String snr)
        throws SS7InvalidParamException {
        m_snr = snr;
        m_snrIsSet = (snr != null);
    }
    /**
      * Get the value of the Serial Number (SNR).
      *
      * @return
      * java.lang.String with 6 digits SNR.
      */
    public java.lang.String getSerialNumber() {
        return m_snr;
    }
    /**
      * Set the value of the Software Version Number (SVN).
      *
      * @param svn
      * java.lang.String with 1 digit SVN.
      */
    public void setSoftwareVersionNumber(java.lang.String svn)
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
      * java.lang.String with 1 digit SVN.
      */
    public java.lang.String getSoftwareVersionNumber() {
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
      * Get the whole IMEI (TAC+FAC+SNR+SVN) as a java.lang.String.
      *
      * @return
      * java.lang.String containing the IMEI.
      */
    public java.lang.String toString() {
        return new java.lang.String("");
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
    public boolean equals(java.lang.Object imei) {
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
    protected java.lang.String m_tac;
    protected boolean m_tacIsSet = false;
    protected java.lang.String m_fac;
    protected boolean m_facIsSet = false;
    protected java.lang.String m_snr;
    protected boolean m_snrIsSet = false;
    protected java.lang.String m_svn;
    protected boolean m_svnIsSet = false;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
