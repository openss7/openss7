/*
 @(#) $RCSfile: AddressConstants.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:23 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:23 $ by $Author: brian $
 */

package jain.protocol.ss7;

import jain.*;

/**
  * Constants used by the JAIN SS7 Address classes
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class AddressConstants {
    /** SPC Format Constant:- this has an integer value of 1.
      * @since JAIN TCAP v1.1 */
    public static final int ANSI_SPC_FORMAT = 1;
    /** SPC Format Constant:- this has an integer value of 2.
      * @since JAIN TCAP v1.1 */
    public static final int ITU_SPC_FORMAT = 2;
    /** Default Constant for the initializaltion of all integer values.  */
    public static final int NOT_SET = 0;
    /** Global TitleIndicator constants: The following constants
      * represent the allowable values for the getGTIndicator method.
      * Indicates a that the GlobalTitle is not included, this constant
      * has an integer value of 0.  */
    public static final int GTINDICATOR_0000 = 0;
    /** Indicates a GTIndicator0001 primitive, this constant has an
      * integer value of 1.  */
    public static final int GTINDICATOR_0001 = 1;
    /** Indicates a GTIndicator0010 primitive, this constant has an
      * integer value of 2.  */
    public static final int GTINDICATOR_0010 = 2;
    /** Indicates a GTIndicator0011 primitive, this constant has an
      * integer value of 3.  */
    public static final int GTINDICATOR_0011 = 3;
    /** Indicates a GTIndicator0100 primitive, this constant has an
      * integer value of 4.  */
    public static final int GTINDICATOR_0100 = 4;
    /** Routing Indicator Constant:- Subsystem and Signaling Pointcode
      * routing, this has an integer value of 1.  */
    public static final int ROUTING_SUBSYSTEM = 1;
    /** Routing Indicator Constant:- GlobalTitle routing, this has an
      * integer value of 2.  */
    public static final int ROUTING_GLOBALTITLE = 2;
    /** Unknown Numbering Plan, this constant has an integer value of 0.
      * */
    public static final int NP_UNKNOWN = 0;
    /** ISDN/Telephony Numbering Plan, this constant has an integer
      * value of 1.  */
    public static final int NP_ISDN_TEL = 1;
    /** Generic Numbering Plan, this constant has an integer value of 2.
      * */
    public static final int NP_GENERIC = 2;
    /** Numbering Plan constants: The following values represent the
      * allowable values for the numbering plan; They are used by the
      * GlobalTitle Indicator classes. 
      * Data Numbering Plan, this constant has an integer value of 3.
      * */
    public static final int NP_DATA = 3;
    /** Telex Numbering Plan, this constant has an integer value of 4.
      * */
    public static final int NP_TELEX = 4;
    /** Maritime Mobile Numbering Plan, this constant has an integer
      * value of 5.  */
    public static final int NP_MARITIME_MOBILE = 5;
    /** Land/Mobile Numbering Plan, this constant has an integer value
      * of 6.  */
    public static final int NP_LAND_MOBILE = 6;
    /** ISDN/Mobile Numbering Plan, this constant has an integer value
      * of 7.
      * The Numbering Plans with integer value of 8 to 13 are spare.  */
    public static final int NP_ISDN_MOBILE = 7;
    /** Private Network or Network Specific Numbering Plan, this
      * constant has an integer value of 14.  */
    public static final int NP_NETWORK = 14;
    /** Indicates a unknown Nature of Address Indicator, this constant
      * has an integer value of 0.  */
    public static final int NA_UNKNOWN = 0;
    /** Indicates a subscriber number, this constant has an integer
      * value of 1.  */
    public static final int NA_SUBSCRIBER = 1;
    /** Reserved for national use Nature of Address Indicator, this
      * constant has an integer value of 2.  */
    public static final int NA_RESERVED = 2;
    /** Indicates a National Significant Number, this constant has an
      * integer value of 3.  */
    public static final int NA_NATIONAL_SIGNIFICANT = 3;
    /** Nature of Address Indicator constants: The following values
      * represent the allowable values for the nature of address
      * indicator. The constants are used by the GlobalTitle Indicator
      * classes. <p> Indicates an international number, this constant
      * has an integer value of 4. Integer values from 5 to 127 are
      * spare, were the eighth binary bit is used as an even/odd
      * indicator.  */
    public static final int NA_INTERNATIONAL = 4;
    /** Indicates an unknown encoding scheme, this constant has an
      * integer value of 0.  */
    public static final int ES_UNKNOWN = 0;
    /** Indicates BCD, odd number of digits, this constant has an
      * integer value of 1.  */
    public static final int ES_ODD = 1;
    /** Encoding Scheme constants: The following values represent the
      * allowable values for the Encoding scheme, used by the
      * GlobalTitle Indicator classes.
      * Indicates a BCD, with an even number of digits, this constant
      * has an integer value of 2.  */
    public static final int ES_EVEN = 2;
    /** Indicates National Specific encoding scheme for ITU, this
      * constant has an integer value of 3.
      * This encoding scheme is spare for ANSI, and integer values of 4
      * to 13 are spare for both variants.  */
    public static final int ES_NATIONAL_SPECIFIC = 3;
    /** A Network Indicator constant: Indicates the International
      * Network 00, which is International this has an integer value of
      * 0.  */
    public static final int NI_INTERNATIONAL_00 = 0;
    /** A Network Indicator constant: Indicates the International
      * Network 01, which is International spare this has an integer
      * value of 1.  */
    public static final int NI_INTERNATIONAL_01 = 1;
    /** A Network Indicator constant: Indicates the National Network 10,
      * which is national this has an integer value of 2.  */
    public static final int NI_NATIONAL_02 = 2;
    /** A Network Indicator constant: Indicates the National Network 11,
      * which is national reserved this has an integer value of 3.  */
    public static final int NI_NATIONAL_03 = 3;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
