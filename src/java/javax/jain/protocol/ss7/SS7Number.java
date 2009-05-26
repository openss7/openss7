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

/**
  * This class can represent any "telephone number"/"address" including
  * information about Type of Number (TON) and Numbering Plan Indicator (NPI).
  * This class can be instantiated as is and will then allow any defined value
  * for TON and NPI as well as any object representing the number itself.  It
  * is the responsibility of the user of this class to ensure that TON, NPI and
  * the type of the number value are consistent with each other. <p>
  *
  * The number value can be just a String or an array of bytes, the encoding of
  * which should be possible to determine based on TON and NPI.  The number
  * value can alos be represented by a specific class, e.g. the class
  * E164Number, that preserves the internal structure of the number value.
  * Ideally, the TON an NPI shall uniquely identify the class for the number
  * value, so that no testing is needed. <p>
  *
  * Subclasses of this class can be declared to get a more relevant name for
  * the contextual usage, e.g. what tyoe of thing the number identifies.  A
  * subclass can also introduce consistency checks and narrow the scope of
  * allowed values for TON and NPI and type of number value. <p>
  *
  * The following rules apply when setting parameter components: <p>
  *
  * <li><i>Mandatory component</i> with <i>no default value</i> must be
  * provided to the constructor.
  * <li><i>Mandatory component</i> with <i>default value</i> neet not be set.
  * <li><i>Optional/conditional component</i> is by default not present.  Such
  * a component becomes present by setting it.
  * <p>
  *
  * <i>Parameter components:</i>
  * <li>ton - Type of Number, mandatory component with no default.
  * <li>npi - Numbering plan indicator, mandatory component with no default.
  * <li>number - Number, mandatory component with no default.
  */
public SS7Number extends SS7Parameter {
    /** Type of number: minimum value used for range check. */
    public static final int TON_MIN = 0;
    /** Type of number: unspecified or unknown type. */
    public static final int TON_UNKNOWN = 0;
    /** Type of number: international significant number. */
    public static final int TON_INTERNATIONAL_NO = 1;
    /** Type of number: national significant number. */
    public static final int TON_NATIONAL_NO = 2;
    /** Type of number: network specific number. */
    public static final int TON_NETW_SPECIFIC_NO = 3;
    /** Type of number: subscriber number. */
    public static final int TON_SUBSCRIBER_NO = 4;
    /** Type of number: alphanumeric identifier. */
    public static final int TON_ALPHANUMERIC = 5;
    /** Type of number: abbreviated number. */
    public static final int TON_ABBREVIATED_NO = 6;
    /** Type of number: reserved value. */
    public static final int TON_RESERVED = 7;
    /** Type of number: maximum value used for range check. */
    public static final int TON_MAX = 7;

    /** Numbering plan identifier: minimum value used for range check. */
    public static final int NPI_MIN = 0;
    /** Numbering plan identifier: unknown or unspecified numbering plan. */
    public static final int NPI_UNKNOWN = 0;
    /** Numbering plan identifier: numbering plan according to ITU-T
      * Recommendation E.164 (including E.163). */
    public static final int NPI_ISDN_E163_164 = 1;
    /** Numbering plan identifier: generic numbering plan. */
    public static final int NPI_GENERIC = 2;
    /** Numbering plan identifier: numbering plan according to ITU-T
      * Recommendation X.121. */
    public static final int NPI_DATA_X121 = 3;
    /** Numbering plan identifier: numbering plan according to ITU-T
      * Recommendation F.69. */
    public static final int NPI_TELEX_F69 = 4;
    /** Numbering plan identifier: numbering plan according to ITU-T
      * Recommendation E.210 and E.211. */
    public static final int NPI_MARI_MOBILE_E210_211 = 5;
    /** Numbering plan identifier: numbering plan according to ITU-T
      * Recommendation E.212. */
    public static final int NPI_LAND_MOBILE_E212 = 6;
    /** Numbering plan identifier: numbering plan according to ITU-T
      * Recommdnation E.214.. */
    public static final int NPI_ISDN_MOBILE_E214 = 7;
    /** Numbering plan identifier: national numbering plan. */
    public static final int NPI_NATIONAL = 8;
    /** Numbering plan identifier: private numbering plan. */
    public static final int NPI_PRIVATE = 9;
    /** Numbering plan identifier: ERMES. */
    public static final int NPI_ERMES = 10;
    /** Numbering plan identifier: maximum value used for range check. */
    public static final int NPI_MAX = 10;

    /**
      * The constructor.
      *
      * @param ton Type of number.
      * @param npi Numbering plan identifier.
      * @param number Number value.
      *
      * @exception SS7InvalidParamException Thrown if paraemeter(s) are invalid
      * or out of range.
      */
    public SS7Number(int ton, int npi, Object number) throws SS7InvalidParameterException {
        if (ton != null) {
            m_ton = ton;
            m_ton_is_set = true;
        }
        if (npi != null) {
            m_npi = npi;
            m_npi_is_set = true;
        }
        if (number != null) {
            m_number = number;
            m_number_is_set = true;
        }
    }
    /**
      * Empty constructor needed for serializable objects and beans.
      */
    public SS7Number() throws SS7InvalidParmException {
    }
    /**
      * Change the Type of Number (TON) parameter.
      *
      * @param ton Type of number.
      * @exception SS7InvalidParamException Thrown if parameter(s) are invalid
      * or out of range.
      */
    public void setTypeOfNumber(int ton) throws SS7InvalidParamException {
        m_ton = ton;
        m_ton_is_set = true;
    }
    /**
      * Get the Type of Number (TON) parameter.
      *
      * @return Type of number.
      */
    public int getTypeOfNumber() {
        int r_ton = null;

        if (m_ton_is_set)
            r_ton = m_ton;
        return r_ton;
    }
    /**
      * Change the Numbering Plan Indicator (NPI) parameter.
      *
      * @param npi Numbering plan indicator.
      * @exception SS7InvalidParamException Thrown if parameters are invalid or
      * out of range.
      */
    public void setNumPlanId(int npi) throws SS7InvalidParamException {
        m_npi = npi;
        m_npi_is_set = true;
    }
    /**
      * Get the Numbering Plan Indicator (NPI) parameter.
      * @return Numbering plan indicator.
      */
    public int getNumPlanId() {
        int r_npi = null;

        if (m_npi_is_set)
            r_npi = m_npi;
        return r_npi;
    }
    /**
      * Change the number value.
      *
      * @param number Number value.
      * @exception SS7InvalidParamException Thrown if parameters are invalid or
      * out of range.
      */
    public void setNumber(Object number) throws SS7InvalidParamException {
        m_number = number;
        m_number_is_set = true;
    }
    /**
      * Get the number value.
      * @return Number value.
      */
    public Object getNumber() {
        Object r_number = null;

        if (m_number_is_set)
            r_number = m_number;
        return r_number;
    }
    /**
      * Get the whole SS7 Number (TON+NPI+Number) as a String.
      *
      * @return String containing the Number.
      */
    public String toString() {
        String text = "SS7Number\n";
        text += "\t\t\tType Of Number: " + getTypeOfNumber() + "\n";
        text += "\t\t\tNumbering Plan Indicator: " + getNumPlanId() + "\n";
        try {
            text += "\t\t\tNumber: " + getNumber().toString() + "\n";
        } catch (Exception e) { }
        return text;
    }
    /**
      * Compares this SS7 Number with another.
      *
      * @param e164 E.164 Number to be compared with this E.164 Number.
      * @return True if equal, otherwise false.
      */
    public boolean equals(Object nr) {
        if (m_ton_is_set != nr.m_ton_is_set)
            return false;
        if (m_npi_is_set != nr.m_npi_is_set)
            return false;
        if (m_number_is_set != nr.m_number_is_set)
            return false;
        if (m_ton != nr.m_ton)
            return false;
        if (m_npi != nr.m_npi)
            return false;
        if (m_number.equals(nr.m_number) != true)
            return false;
        return true;
    }

    private int m_ton = TON_UNKNOWN;
    private boolean m_ton_is_set = false;
    private int m_npi = NPI_UNKNOWN;
    private boolean m_npi_is_set = false;
    private Object m_number;
    private boolean m_number_is_set = false;
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
