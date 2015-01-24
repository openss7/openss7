/*
 @(#) src/java/jain/protocol/ss7/tcap/TcapConstants.java <p>
 
 Copyright &copy; 2008-2015  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
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
 <a href="http://www.openss7.com/">http://www.openss7.com/</a>
 */

package jain.protocol.ss7.tcap;

import jain.protocol.ss7.*;
import jain.*;

/**
  * Constants used in the "jain.protocol.ss7.tcap" package.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class TcapConstants extends java.lang.Object {
    /** Indicates an ERROR component primitive. It has an integer value
      * of 1.  */
    public static final int PRIMITIVE_ERROR = 1;
    /** Indicates an INVOKE component primitive. It has an integer value
      * of 2.  */
    public static final int PRIMITIVE_INVOKE = 2;
    /** Indicates a LOCAL_CANCEL component primitive. It has an integer
      * value of 3.  */
    public static final int PRIMITIVE_LOCAL_CANCEL = 3;
    /** Indicates a REJECT component primitive. It has an integer value
      * of 5.  */
    public static final int PRIMITIVE_REJECT = 5;
    /** Indicates a RESULT component primitive. It has an integer value
      * of 7 */
    public static final int PRIMITIVE_RESULT = 7;
    /** Indicates a TIMER RESET component primitive. It has an integer
      * value of 9.  */
    public static final int PRIMITIVE_TIMER_RESET = 9;
    /** Indicates a USER CANCEL component primitive. It has an integer
      * value of 10.  */
    public static final int PRIMITIVE_USER_CANCEL = 10;
    /** Primitive Type constants: The following constants represent the
      * allowable values for the GetPrimitiveType() method in each of
      * the Component and Dialogue handling primitives.  Indicates a
      * BEGIN dialogue primitive. It has an integer value of 11.  */
    public static final int PRIMITIVE_BEGIN = 11;
    /** Indicates a CONTINUE dialogue primitive. It has an integer value
      * of 12.  */
    public static final int PRIMITIVE_CONTINUE = 12;
    /** Indicates an END dialogue primitive. It has an integer value of
      * 13.  */
    public static final int PRIMITIVE_END = 13;
    /** Indicates a NOTICE dialogue primitive. It has an integer value
      * of 14.  */
    public static final int PRIMITIVE_NOTICE = 14;
    /** Indicates a PROVIDER ABORT dialogue primitive. It has an integer
      * value of 15 */
    public static final int PRIMITIVE_PROVIDER_ABORT = 15;
    /** Indicates a UNIDIRECTIONAL dialogue primitive. It has an integer
      * value of 16.  */
    public static final int PRIMITIVE_UNIDIRECTIONAL = 16;
    /** Indicates a USER ABORT dialogue primitive. It has an integer
      * value of 17.  */
    public static final int PRIMITIVE_USER_ABORT = 17;
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
      * routing, this has an integer value of 1.
      * @since JAIN TCAP v1.1 */
    public static final int ROUTING_SUBSYSTEM = 1;
    /** Routing Indicator Constant:- GlobalTitle routing, this has an
      * integer value of 2.
      * @since JAIN TCAP v1.1 */
    public static final int ROUTING_GLOBALTITLE = 2;
    /** Unknown Numbering Plan, this constant has an integer value of 0.  */
    public static final int NP_UNKNOWN = 0;
    /** ISDN/Telephony Numbering Plan, this constant has an integer
      * value of 1.  */
    public static final int NP_ISDN_TEL = 1;
    /** Generic Numbering Plan, this constant has an integer value of 2.  */
    public static final int NP_GENERIC = 2;
    /** Numbering Plan constants: The following values represent the
      * allowable values for the numbering plan; They are used by the
      * GlobalTitle Indicator classes.  Data Numbering Plan, this
      * constant has an integer value of 3.  */
    public static final int NP_DATA = 3;
    /** Telex Numbering Plan, this constant has an integer value of 4.  */
    public static final int NP_TELEX = 4;
    /** Maritime Mobile Numbering Plan, this constant has an integer
      * value of 5.  */
    public static final int NP_MARITIME_MOBILE = 5;
    /** Land/Mobile Numbering Plan, this constant has an integer value
      * of 6.  */
    public static final int NP_LAND_MOBILE = 6;
    /** ISDN/Mobile Numbering Plan, this constant has an integer value
      * of 7. The Numbering Plans with integer value of 8 to 13 are
      * spare.  */
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
      * classes.  Indicates an international number, this constant has
      * an integer value of 4. Integer values from 5 to 127 are spare,
      * were the eighth binary bit is used as an even/odd indicator.  */
    public static final int NA_INTERNATIONAL = 4;
    /** Indicates an unknown encoding scheme, this constant has an
      * integer value of 0.  */
    public static final int ES_UNKNOWN = 0;
    /** Indicates BCD, odd number of digits, this constant has an
      * integer value of 1.  */
    public static final int ES_ODD = 1;
    /** Encoding Scheme constants: The following values represent the
      * allowable values for the Encoding scheme, used by the
      * GlobalTitle Indicator classes.  Indicates a BCD, with an even
      * number of digits, this constant has an integer value of 2.  */
    public static final int ES_EVEN = 2;
    /** Indicates National Specific encoding scheme for ITU, this
      * constant has an integer value of 3. This encoding scheme is
      * spare for ANSI, and integer values of 4 to 13 are spare for both
      * variants.  */
    public static final int ES_NATIONAL_SPECIFIC = 3;
    /** Stack Specification Constant:- ANSI T1.114 - 1992 Stack
      * Specification, this has an integer value of 1.
      * @since JAIN TCAP v1.1 */
    public static final int STACK_SPECIFICATION_ANSI_92 = 1;
    /** Stack Specification Constant:- ANSI T1.114 - 1996 Stack
      * Specification, this has an integer value of 2.
      * @since JAIN TCAP v1.1 */
    public static final int STACK_SPECIFICATION_ANSI_96 = 2;
    /** Stack Specification Constant:- ITU Q.771 - Q.775 - 1993 Stack
      * Specification, this has an integer value of 3.
      * @since JAIN TCAP v1.1 */
    public static final int STACK_SPECIFICATION_ITU_93 = 3;
    /** Stack Specification Constant:- ITU Q.771 - Q.775 - 1997 Stack
      * Specification, this has an integer value of 4.
      * @since JAIN TCAP v1.1 */
    public static final int STACK_SPECIFICATION_ITU_97 = 4;
    /** Private constructor for this class. */
    private TcapConstants() {
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
