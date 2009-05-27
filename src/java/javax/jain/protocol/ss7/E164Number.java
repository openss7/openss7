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

import javax.jain.protocol.ss7.SS7InvalidParamException;

/**
  * This parameter class represents an E.164 number as defined in the
  * specifications 3G TS 23.003 and ITU-T Recommendation E.164. <p>
  *
  * The following rules applies for the setting of parameter components:
  * <li><i>Mandatory component</i> with <i>no default value</i> must be provided
  * to the constructor.
  * <li><i>Mandatory component</i> with <i>default value</i> need not be set.
  * <li><i>Optional/conditional component</i> is by default not present. Such a
  * component becomes present by setting it.
  * <p>
  *
  * <i>Parameter components:</i>
  * <li>cc - Country Code, mandatory component with no default
  * <li>ndc - National Destination Code, optional component
  * <li>sn - Subscriber Number, mandatory component with no default
  * <p>
  *
  * Format rule: the length of the whole E.164 Number must not be more
  * than 15 digits.
  */
public class E164Number extends SS7Parameter {
    /**
      * Empty constructor; needed for serializable objects and beans.
      */
    public E164Number() throws SS7InvalidParamException {
    }
    /**
      * The constructor.
      *
      * @param cc Country Code (1-3 digits)
      * @param sn Subscriber Number (limited by format rule)
      * @throws SS7InvalidParamException Thrown if parameter(s) are
      *	    invalid / out of range.
      */
    public E164Number(String cc, String sn) throws SS7InvalidParamException {
        this.setCountryCode(cc);
        this.setSubscriberNumber(sn);
    }
    /**
      * Change the value of the Country Code (CC).
      *
      * @param cc String with 1-3 digits CC.
      */
    public void setCountryCode(String cc) throws SS7InvalidParamException {
        if (cc.length() > 3)
            throw SS7InvalidParamException("Country Code \"" + cc + "\" is too long.");
        for (int i = 0; i < cc.length(); i++) {
            switch (cc.charAt(i)) {
                case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                    break;
                default:
                    throw SS7InvalidParamException("Country Code invalid digit '" + cc.charAt(i) + "' at position " + i + ".");
            }
        }
        m_cc = cc;
        m_cc_is_set = true;
    }
    /**
      * Get the value of the Country Code (CC).
      *
      * @return String with 1-3 digits CC.
      */
    public String getCountryCode() {
        String cc = "";
        if (m_cc_is_set)
            cc = m_cc;
        return cc;
    }
    /**
      * Get the value of the National Destination Code (NDC).
      *
      * Get the value of the National Destination Code (NDC). It shall
      * be checked if this parameter is present before getting it.
      *
      * @return String with NDC.
      */
    public String getNatDestCode() {
        String ndc = "";
        if (m_ndc_is_set)
            ndc = m_ndc;
        return ndc;
    }
    /**
      * Check if the National Destination Code (NDC) is present.
      *
      * @return True or false.
      */
    public boolean isNatDestCodePresent() {
        return m_ndc_is_present;
    }
    /**
      * Change the value of the Subscriber Number (SN).
      *
      * @param sn String with SN.
      */
    public void setSubscriberNumber(String sn) throws SS7InvalidParamException {
        if (sn == null)
            throw SS7InvalidParamException("Subscriber Number must be specified.");
        int len = sn.length();
        int max = 15 - getCountryCode().length() - getNatDestCode().length();
        if (len > max)
            throw SS7InvalidParamException("Subscriber Number invalid, " + (len - max) + " digits too long.");
        for (int i=0; i < len; i++) {
            switch (ndc.charAt(i)) {
                case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                    break;
                default:
                    throw SS7InvalidParamException("Subscriber Number invalid digit '" + cc.charAt(i) + "' at position " + i + ".");
            }
        }
        m_sn = sn;
        m_sn_is_set = true;
    }
    /**
      * Get the value of the Subscriber Number (SN).
      *
      * @return String with SN.
      */
    public String getSubscriberNumber() {
        String sn = "";
        if (m_sn_is_set)
            sn = m_sn;
        return sn;
    }
    /**
      * Set the value of the National Destination Code (NDC).
      *
      * Set the value of the National Destination Code (NDC). The
      * length of this optional parameter component is limited by the
      * fact that the total length of an E.164 Number can be maximum 15
      * digits.
      *
      * @param ndc String with NDC.
      */
    public void setNatDestCode(String ndc) throws SS7InvalidParamException {
        int len = ndc.length();
        int max = 15 - getCountryCode().length() - getSubscriberNumber().length();
        if (len > max)
            throw SS7InvalidParamException("Nat Dest Code invalid, " + (len - max) + " digits too long.");
        for (int i=0; i < len; i++) {
            switch (ndc.charAt(i)) {
                case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
                    break;
                default:
                    throw SS7InvalidParamException("Nat Dest Code invalid digit '" + cc.charAt(i) + "' at position " + i + ".");
            }
        }
        m_ndc = ndc;
        m_ndc_is_set = true;
    }
    /**
      * Get the whole E.164 Number (CC+NDC+SN) as a String.
      *
      * @overrides toString in class Object
      *
      * @return String containing the E.164 Number.
      */
    public String toString() {
        String text = "E164Number\n";
        text += "\t\t\tCountry Code: " + getCountryCode() + "\n";
        text += "\t\t\tNational Destination Code: " + getNatDestCode() + "\n";
        text += "\t\t\tSubscriber Number: " + getSubscriberNumber() + "\n";
        return text;
    }
    /**
      * Compares this E.164 Number with another.
      *
      * @override equals in class Object
      * @param e164 E.164 Number to be compared with this E.164
      *	    Number.
      * @return True if equal, otherwise false.
      */
    public boolean equals(Object e164) {
        try {
            if (e164 instanceof E164Number) {
                if (m_cc_is_set != e164.m_cc_is_set)
                    return false;
                if (m_ndc_is_set != e164.m_ndc_is_set)
                    return false;
                if (m_sn_is_set != e164.m_sn_is_set)
                    return false;
                if (!m_cc.equals(e164.m_cc))
                    return false;
                if (!m_ndc.equals(e164.m_ndc))
                    return false;
                if (!m_sn.equals(e164.m_sn))
                    return false;
                return true;
            }
            return false;
        } catch (Exception e) {
            return false;
        }
    }
    /**
      * Get the length of the E.164 Number; should be 15 digits or less.
      *
      * @return Number of digits.
      */
    public int length() {
        int len = 0;
        len += getCountryCode().length();
        len += getNatDestCode().length();
        len += getSubscriberNumber().length();
        return len;
    }

    private byte[] m_cc;
    private boolean m_cc_is_set = false;
    private String m_ndc;
    private boolean m_ndc_is_set = false;
    private String m_sn;
    private boolean m_sn_is_set = false;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
