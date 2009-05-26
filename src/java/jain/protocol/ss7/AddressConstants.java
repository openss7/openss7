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
 *  File Name     : SS7AddressConstants.java
 *  Originator    : Phelim O'Doherty [AePONA]
 *  Approver      : Jain Community
 *
 *  HISTORY
 *  Version   Date      Author              Comments
 *  1.0     20/11/2000  Phelim O'Doherty    Initial version
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7;

/**
 * Constants used by the JAIN SS7 Address classes
 *
 * @version 1.0
 * @author Sun Microsystems Inc.
 *
 */
public class AddressConstants {

    private AddressConstants()
    {
    }

    /**
    * v1.1: Added new SPC Format constants for working out the format of the SPC
    *
    * @since JAIN TCAP v1.1
    */
    private static final int SPC_FORMAT = 0;

    /**
    * SPC Format Constant:- this has an integer value of 1.
    * @since JAIN TCAP v1.1
    */
    public static final int ANSI_SPC_FORMAT   = SPC_FORMAT|1;

    /**
    * SPC Format Constant:- this has an integer value of 2.
    * @since JAIN TCAP v1.1
    */
    public static final int ITU_SPC_FORMAT    = SPC_FORMAT|2;

    /**
    * Default Constant for the initializaltion of all integer values.
    */
    public static final int NOT_SET = -1;

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
    * Routing Indicator constants determine
    * elements of the USER TCAP used for routing
    * Update v1.1
    */
    private static final int ROUTING = 0;

    /**
    * Routing Indicator Constant:- Subsystem and Signaling Pointcode routing, this has an integer value of 1.
    */
    public static final int ROUTING_SUBSYSTEM   = ROUTING|1;

    /**
    * Routing Indicator Constant:- GlobalTitle routing, this has an integer value of 2.
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

    private final static int NI = 0;

    /**
    * A Network Indicator constant: Indicates the International Network 00, which is International
    * this has an integer value of 0.
    */
    public final static int NI_INTERNATIONAL_00 = NI|0;

    /**
    * A Network Indicator constant: Indicates the International Network 01, which is International spare
    * this has an integer value of 1.
    */
    public final static int NI_INTERNATIONAL_01 = NI|1;

    /**
    * A Network Indicator constant: Indicates the National Network 10, which is national
    * this has an integer value of 2.
    */
    public final static int NI_NATIONAL_02 = NI|2;

    /**
    * A Network Indicator constant: Indicates the National Network 11, which is national reserved
    * this has an integer value of 3.
    */
    public final static int NI_NATIONAL_03 = NI|3;
}
