/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Copyrights:
 *
 *  Copyright - 1999 Sun Microsystems, Inc. All rights reserved.
 *  901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 *  This product and related documentation are protected by copyright and
 *  distributed under licenses restricting its use, copying, distribution, and
 *  decompilation. No part of this product or related documentation may be
 *  reproduced in any form by any means without prior written authorization of
 *  Sun and its licensors, if any.
 *
 *  RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 *  States Government is subject to the restrictions set forth in DFARS
 *  252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 *  The product described in this manual may be protected by one or more U.S.
 *  patents, foreign patents, or pending applications.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Author:
 *
 *  AePONA Limited, Interpoint Building
 *  20-24 York Street, Belfast BT15 1AQ
 *  N. Ireland.
 *
 *
 *  Module Name   : JAIN TCAP API
 *  File Name     : TcapConstants.java
 *  Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 *  Approver      : Jain Tcap Edit Group
 *
 *  HISTORY
 *  Version   Date      Author              Comments
 *  1.1     16/10/2000  Phelim O'Doherty    Added stack specification constants.
 *                                          Added routing Indicator constants.
 *                                          Made constructor private.
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7.tcap;

/**
 * Constants used in the "jain.protocol.ss7.tcap" package
 *
 * @version 1.1
 * @author Sun Microsystems Inc.
 *
 */
public class TcapConstants {
    private TcapConstants() {
    }

    private static final int PRIMITIVE = 0 ;

    /**
    * Indicates an ERROR component primitive. It has an integer value of 1.
    */
    public static final int PRIMITIVE_ERROR = PRIMITIVE|1;

    /**
    * Indicates an INVOKE component primitive. It has an integer value of 2.
    */
    public static final int PRIMITIVE_INVOKE = PRIMITIVE|2;

    /**
    * Indicates a LOCAL_CANCEL component primitive. It has an integer value of 3.
    */
    public static final int PRIMITIVE_LOCAL_CANCEL = PRIMITIVE|3;

    /**
    * Indicates a REJECT component primitive. It has an integer value of 5.
    */
    public static final int PRIMITIVE_REJECT = PRIMITIVE|5;

    /**
    * Indicates a RESULT component primitive.
    * It has an integer value of 7
    */
    public static final int PRIMITIVE_RESULT = PRIMITIVE|7;

    /**
    * Indicates a TIMER RESET component primitive. It has an integer value of 9.
    */
    public static final int PRIMITIVE_TIMER_RESET = PRIMITIVE|9;

    /**
    * Indicates a USER CANCEL component primitive. It has an integer value of 10.
    */
    public static final int PRIMITIVE_USER_CANCEL = PRIMITIVE|10;

    /**
    * Primitive Type constants:  The following constants represent the allowable
    * values for the GetPrimitiveType() method in each of the Component
    * and Dialogue handling primitives.<BR><BR>
    * Indicates a BEGIN dialogue primitive. It has an integer value of 11.
    */
    public static final int PRIMITIVE_BEGIN = PRIMITIVE|11;

    /**
    * Indicates a CONTINUE dialogue primitive. It has an integer value of 12.
    */
    public static final int PRIMITIVE_CONTINUE = PRIMITIVE|12;

    /**
    * Indicates an END dialogue primitive. It has an integer value of 13.
    */
    public static final int PRIMITIVE_END = PRIMITIVE|13;

    /**
    * Indicates a NOTICE dialogue primitive. It has an integer value of 14.
    */
    public static final int PRIMITIVE_NOTICE = PRIMITIVE|14;

    /**
    * Indicates a PROVIDER ABORT dialogue primitive. It has an integer value of 15
    */
    public static final int PRIMITIVE_PROVIDER_ABORT = PRIMITIVE|15;

    /**
    * Indicates a UNIDIRECTIONAL dialogue primitive. It has an integer value of 16.
    */
    public static final int PRIMITIVE_UNIDIRECTIONAL = PRIMITIVE|16;

    /**
    * Indicates a USER ABORT dialogue primitive. It has an integer value of 17.
    */
    public static final int PRIMITIVE_USER_ABORT = PRIMITIVE|17;



    private static final int GTINDICATOR = 0 ;

    /**
    * Global TitleIndicator constants: The following constants represent the
    * allowable values for the getGTIndicator method.<BR><BR>
    * Indicates a that the GlobalTitle is not included, this constant has an integer value of 0.
    */
    public static final int GTINDICATOR_0000 = GTINDICATOR|0;

    /**
    * Indicates a GTIndicator0001 primitive, this constant has an integer value of 1.
    */
    public static final int GTINDICATOR_0001 = GTINDICATOR|1;

    /**
    * Indicates a GTIndicator0010 primitive, this constant has an integer value of 2.
    */
    public static final int GTINDICATOR_0010 = GTINDICATOR|2;

    /**
    * Indicates a GTIndicator0011 primitive, this constant has an integer value of 3.
    */
    public static final int GTINDICATOR_0011 = GTINDICATOR|3;

    /**
    * Indicates a GTIndicator0100 primitive, this constant has an integer value of 4.
    */
    public static final int GTINDICATOR_0100 = GTINDICATOR|4;

    /**
    * Update v1.1: Added new Routing Indicator constants to determine
    * elements of TcapUserAddress used for routing
    * @since JAIN TCAP v1.1
    */
    private static final int ROUTING = 0;

    /**
    * Routing Indicator Constant:- Subsystem and Signaling Pointcode routing, this has an integer value of 1.
    * @since JAIN TCAP v1.1
    */
    public static final int ROUTING_SUBSYSTEM   = ROUTING|1;

    /**
    * Routing Indicator Constant:- GlobalTitle routing, this has an integer value of 2.
    * @since JAIN TCAP v1.1
    */
    public static final int ROUTING_GLOBALTITLE    = ROUTING|2;

    private final static int NP = 0;

    /**
    * Unknown Numbering Plan, this constant has an integer value of 0.
    */
    public final static int NP_UNKNOWN = NP|0;

    /**
    * ISDN/Telephony Numbering Plan, this constant has an integer value of 1.
    */
    public final static int NP_ISDN_TEL = NP|1;

    /**
    * Generic Numbering Plan, this constant has an integer value of 2.
    */
    public final static int NP_GENERIC = NP|2;

    /**
    * Numbering Plan constants: The following values represent
    * the allowable values for the numbering plan;
    * They are used by the GlobalTitle Indicator classes.<BR><BR>
    * Data Numbering Plan, this constant has an integer value of 3.
    */
    public final static int NP_DATA = NP|3;

    /**
    * Telex Numbering Plan, this constant has an integer value of 4.
    */
    public final static int NP_TELEX = NP|4;

    /**
    * Maritime Mobile Numbering Plan, this constant has an integer value of 5.
    */
    public final static int NP_MARITIME_MOBILE = NP|5;

    /**
    * Land/Mobile Numbering Plan, this constant has an integer value of 6.
    */
    public final static int NP_LAND_MOBILE = NP|6;

    /**
    * ISDN/Mobile Numbering Plan, this constant has an integer value of 7.
    * The Numbering Plans with integer value of 8 to 13 are spare.
    */
    public final static int NP_ISDN_MOBILE = NP|7;

    /**
    * Private Network or Network Specific Numbering Plan, this constant has an integer value of 14.
    */
    public final static int NP_NETWORK = NP|14;

    private final static int NA = 0;

    /**
    * Indicates a unknown Nature of Address Indicator, this constant has an integer value of 0.
    */
    public final static int NA_UNKNOWN = NA|0;

    /**
    * Indicates a subscriber number, this constant has an integer value of 1.
    */
    public final static int NA_SUBSCRIBER = NA|1;

    /**
    * Reserved for national use Nature of Address Indicator, this constant has an integer value of 2.
    */
    public final static int NA_RESERVED = NA|2;

    /**
    * Indicates a National Significant Number, this constant has an integer value of 3.
    */
    public final static int NA_NATIONAL_SIGNIFICANT = NA|3;

    /**
    * Nature of Address Indicator constants: The following values represent
    * the allowable values for the nature of address indicator. The constants
    * are used by the GlobalTitle Indicator classes.<BR><BR>
    * Indicates an international number, this constant has an integer value of 4.
    * Integer values from 5 to 127 are spare, were the eighth binary bit is used as an even/odd indicator.
    */
    public final static int NA_INTERNATIONAL = NA|4;

    private final static int ES = 0;

    /**
    * Indicates an unknown encoding scheme, this constant has an integer value of 0.
    */
    public final static int ES_UNKNOWN = ES|0;

    /**
    * Indicates BCD, odd number of digits, this constant has an integer value of 1.
    */
    public final static int ES_ODD = ES|1;

    /**
    * Encoding Scheme constants: The following values represent
    * the allowable values for the Encoding scheme, used by the GlobalTitle Indicator classes.<BR><BR>
    * Indicates a BCD, with an even number of digits, this constant has an integer value of 2.
    */
    public final static int ES_EVEN = ES|2;

    /**
    * Indicates National Specific encoding scheme for ITU, this constant has an integer value of 3.
    * This encoding scheme is spare for ANSI, and integer values of 4 to 13 are spare for both variants.
    */
    public final static int ES_NATIONAL_SPECIFIC = ES|3;

        /**
    * v1.1: Added new Stack specification constants to avoid confusion
    * between stack specification and the protocol version supported by an ANSI 1996
    * TCAP protocol
    * @since JAIN TCAP v1.1
    */
    private static final int STACK_SPECIFICATION = 0;

    /**
    * Stack Specification Constant:- ANSI T1.114 - 1992 Stack Specification, this has an integer value of 1.
    * @since JAIN TCAP v1.1
    */
    public static final int STACK_SPECIFICATION_ANSI_92   = STACK_SPECIFICATION|1;

    /**
    * Stack Specification Constant:- ANSI T1.114 - 1996 Stack Specification, this has an integer value of 2.
    * @since JAIN TCAP v1.1
    */
    public static final int STACK_SPECIFICATION_ANSI_96    = STACK_SPECIFICATION|2;

    /**
    * Stack Specification Constant:- ITU Q.771 - Q.775 - 1993 Stack Specification, this has an integer value of 3.
    * @since JAIN TCAP v1.1
    */
    public static final int STACK_SPECIFICATION_ITU_93    = STACK_SPECIFICATION|3;

    /**
    * Stack Specification Constant:- ITU Q.771 - Q.775 - 1997 Stack Specification, this has an integer value of 4.
    * @since JAIN TCAP v1.1
    */
    public static final int STACK_SPECIFICATION_ITU_97    = STACK_SPECIFICATION|4;
}




