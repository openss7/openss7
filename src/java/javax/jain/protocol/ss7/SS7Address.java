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

package javax.jain.protocol.ss7;

import java.lang.Object;
import java.lang.Cloneable;

import java.io.Serializable;

import javax.jain.protocol.ss7.JainSS7Utility;
import javax.jain.protocol.ss7.E164Number;
import javax.jain.protocol.ss7.SS7Parameter;
import javax.jain.protocol.ss7.SS7InvalidParamException;
import javax.jain.protocol.ss7.SS7ParameterNotSetException;

/**
  * This class represents an address o a Jain MAP API User.  This address
  * contains information that are similar to an SCCP Address. <p>
  *
  * The following rules applies for the setting of parameter components:
  * <li><i>Mandatory component</i> with <i>no default value</i> must be
  * provided ot the constructor.
  * <li><i>Mandatory component</i> with <i>default value</i> need not be set.
  * <li><i>Optional / conditional component</i> is by default not present.
  * Such a component becomes present by setting it.
  * <p>
  *
  * <i>Parameter components:</i>
  * <li>spc - Signalling Point Code, optional component
  * <li>ssn - Subsystem Number, optional component
  * <li>gt - Global Title, optional component
  * <p>
  *
  * (At least one of the components has to be present.)
  */
public interface SS7Address extends SS7Parameter implements Cloneable, Serializable {

    /**
      * The constructor.
      *
      * @exception SS7InvalidParamException Thrown if parameter(s) are invalid
      * or out of range.
      */
    public SS7Address() throws SS7InvalidParamException {
    }

    /**
      * Set the Signaling Point Code (SPC).
      *
      * @param spc Byte array containing SPC.
      *
      * @exception SS7InvalidParamException Thrown if parameter(s) are invalid
      * or out of range.
      */
    public void setSignalingPointCode(byte[] spc) throws SS7InvalidParamException {
        m_spc = spc;
        m_spc_is_set = true;
        
    }

    /**
      * Get the Signaling Point Code (SPC).
      * It shall be checked if this parameter is present before getting it.
      *
      * @return Byte array containing SPC.
      *
      * @exception SS7ParameterNotSetException Thrown if the parameter is not
      * present.
      */
    public byte[] getSignalingPointCode() throws SS7ParameterNotSetException {
        if (m_spc_is_set) {
            byte[] spc = m_spc;
            return spc;
        }
        throw SS7ParameterNotSetException("Signaling Point Code is not set.");
    }

    /**
      * Check if the Signalling Point Code (SPC) is present.
      *
      * @return True or false.
      */
    public boolean isSignalingPointCodePresent() {
        return m_spc_is_set;
    }

    /**
      * Set the Subsystem Number (SSN).
      *
      * @param ssn The SubSystem Number.
      *
      * @exception SS7InvalidParamException Thrown if parameter(s) are invalid
      * or out of range.
      */
    public void  setSubSystemNumber(int ssn) throws SS7InvalidParameterException {
        switch (ssn) {
            case 0:
            case 1:
            case 255:
                throw SS7InvalidParameterException("SubSystem Number value " + ssn + " is invalid.");
        }
        m_ssn = ssn;
        m_ssn_is_set = true;
    }

    /**
      * Get the Subsystem Number (SSN).
      * It shall be checked if this parameter is present before getting it.
      *
      * @return The SubSystem Number.
      *
      * @exception SS7ParameterNotSetException Thrown if the parameter is not
      * present.
      */
    public int getSubSystemNumber() throws SS7ParameterNotSetException {
        if (m_ssn_is_set) {
            return m_ssn;
        }
        throw SS7ParameterNotSetException("SubSystem Number is not set.");
    }

    /**
      * Check if the SubSystem Number (SSN) is present.
      *
      * @return True or false.
      */
    public boolean isSubSystemNumberPresent() {
        return m_ssn_is_set;
    }

    /**
      * Set the Global Title (GT).
      *
      * @param gt The Global Title (typically an E.164 number).
      *
      * @exception SS7InvalidParamException Thrown if parameter(s) are invalid
      * or out of range.
      */
    public void setGlobalTitle(E164Number gt) throws SS7InvalidParamException {
        if (gt != null) {
            m_gt = gt;
            m_gt_is_set = true;
            return;
        }
        throw SS7InvalidParamException("E164 Number is invalid.");
    }

    /**
      * Get the Global Title (GT).
      * It shall be checked if the parameter is present before getting it.
      *
      * @return The Global Title (typically an E.164 number).
      *
      * @exception SS7ParameterNotSetException Thrown if the parameter is not
      * present.
      */
    public E164Number getGlobalTitle() throws SS7ParameterNotSetException {
        if (m_gt_is_set) {
            return m_gt;
        }
        throw SS7ParameterNotSetException("Global Title (GT) not set.");
    }

    /**
      * Check if the Global Title (GT) is present.
      *
      * @return True or false.
      */
    public boolean isGlobalTitlePresent() {
        return m_gt_is_set;
    }

    /**
      * Get the whole SS7Address (SPC+SSN+GT) as a String.
      *
      * @return String.
      */
    public String toString() {
        String text = "SS7Address\n";

        try {
            byte[] pointCode = getSignalingPointCode();
            text += "\t\t\tSignaling Point Code: " +
                new String(JainSS7Utility.bytesToHex(pointCode, 0, pointCode.length)) + "\n";
        } catch (SS7ParameterNotSetException e ) { }
        try {
            text += "\t\t\tSub-System Number: " + getSubSystemNumber() + "\n";
        } catch (SS7ParameterNotSetException e ) { }
        try {
            text += "\t\t\tGlobal Title: " + getGlobalTitle().toString() + "\n";
        } catch (SS7ParameterNotSetException e ) { }
        return text;
    }

    /**
      * Compares this SS7Address with another.
      *
      * @param sa SS7Address to be compared with this SS7Address.
      *
      * @return True if equal, otherwise false.
      */
    public boolean equals(Object sa) {
        if (sa instanceof SS7Address) {
            if (m_spc_is_set != sa.m_spc_is_set)
                return false;
            if (m_ssn_is_set != sa.m_ssn_is_set)
                return false;
            if (m_gt_is_set != sa.m_gt_is_set)
                return false;
            if (m_spc_is_set) {
                if (m_spc.length != sa.m_spc.length)
                    return false;
                for (int i=0; i < m_spc.length; i++) {
                    if (m_spc[i] != sa.m_spc[i])
                        return false;
                }
            }
            if (m_ssn_is_set) {
                if (m_ssn != sa.m_ssn)
                    return false;
            }
            if (m_gt_is_set) {
                if (!m_gt.equals(sa.m_gt))
                    return false;
            }
            return true;
        }
        return false;
    }

    private byte[] m_spc;
    private boolean m_spc_is_set = false;
    private int m_ssn;
    private boolean m_ssn_is_set = false;
    private E164Number m_gt;
    private boolean m_gt_is_set = false;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
