/*
 @(#) $RCSfile: NumberingPlan.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:45 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:45 $ by $Author: brian $
 */

package javax.jain.ss7;

import javax.jain.*;

/**
  * Constants used by the JAIN SS7 GlobalTitle Indicators classes.
  * The Numbering Plans with integer value of 8 to 13 are spare, the
  * rest are defined below.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class NumberingPlan {
    /** Internal Constant available for switch statements.  */
    public static final int M_NP_UNKNOWN = 0;
    /** Internal Constant available for switch statements.  */
    public static final int M_NP_ISDN_TEL = 1;
    /** Internal Constant available for switch statements.  */
    public static final int M_NP_GENERIC = 2;
    /** Internal Constant available for switch statements.  */
    public static final int M_NP_DATA = 3;
    /** Internal Constant available for switch statements.  */
    public static final int M_NP_TELEX = 4;
    /** Internal Constant available for switch statements.  */
    public static final int M_NP_MARITIME_MOBILE = 5;
    /** Internal Constant available for switch statements.  */
    public static final int M_NP_LAND_MOBILE = 6;
    /** Internal Constant available for switch statements.  */
    public static final int M_NP_ISDN_MOBILE = 7;
    /** Internal Constant available for switch statements.  */
    public static final int M_NP_NETWORK = 8;
    /** Protected constructure with value.
      * @param value
      * The value of the constant class. */
    protected NumberingPlan(int value) {
        m_value = value;
        m_valueIsSet = true;
    }
    /**
      * java.lang.String representation of the Constant class.
      * @return
      * java.lang.String provides description of Constant.
      */
    public java.lang.String toString() {
        if (m_valueIsSet) {
            switch (m_value) {
                case M_NP_UNKNOWN:
                    return "NP_UNKNOWN";
                case M_NP_ISDN_TEL:
                    return "NP_ISDN_TEL";
                case M_NP_GENERIC:
                    return "NP_GENERIC";
                case M_NP_DATA:
                    return "NP_DATA";
                case M_NP_TELEX:
                    return "NP_TELEX";
                case M_NP_MARITIME_MOBILE:
                    return "NP_MARITIME_MOBILE";
                case M_NP_LAND_MOBILE:
                    return "NP_LAND_MOBILE";
                case M_NP_ISDN_MOBILE:
                    return "NP_ISDN_MOBILE";
                case M_NP_NETWORK:
                    return "NP_NETWORK";
            }
        }
        return "(error)";
    }
    /**
      * Gets the integer representation of the Constant class.
      * @return
      * Integer provides value of Constant.
      */
    public int intValue() {
        return m_value;
    }
    protected int m_value = M_NP_UNKNOWN;
    protected boolean m_valueIsSet = false;
    /** Unknown Numbering Plan.  */
    public static final NumberingPlan NP_UNKNOWN
         = new NumberingPlan(M_NP_UNKNOWN);
    /** ISDN/Telephony Numbering Plan.  */
    public static final NumberingPlan NP_ISDN_TEL
         = new NumberingPlan(M_NP_ISDN_TEL);
    /** Generic Numbering Plan.  */
    public static final NumberingPlan NP_GENERIC
         = new NumberingPlan(M_NP_GENERIC);
    /** Data Numbering Plan.  */
    public static final NumberingPlan NP_DATA
         = new NumberingPlan(M_NP_DATA);
    /** Telex Numbering Plan.  */
    public static final NumberingPlan NP_TELEX
         = new NumberingPlan(M_NP_TELEX);
    /** Maritime Mobile Numbering Plan.  */
    public static final NumberingPlan NP_MARITIME_MOBILE
         = new NumberingPlan(M_NP_MARITIME_MOBILE);
    /** Land/Mobile Numbering Plan.  */
    public static final NumberingPlan NP_LAND_MOBILE
         = new NumberingPlan(M_NP_LAND_MOBILE);
    /** ISDN/Mobile Numbering Plan.  */
    public static final NumberingPlan NP_ISDN_MOBILE
         = new NumberingPlan(M_NP_ISDN_MOBILE);
    /** Private Network or Network Specific Numbering Plan.  */
    public static final NumberingPlan NP_NETWORK
         = new NumberingPlan(M_NP_NETWORK);
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
