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
 *  Module Name   : JAIN Common API
 *  File Name     : NumberingPlan.java
 *  Originator    : Phelim O'Doherty
 *  Approver      : JAIN Community
 *
 *  HISTORY
 *  Version   Date      Author              Comments
 *  1.0     28/09/2001  Phelim O'Doherty    Initial version
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7;

/**
 * Constants used by the JAIN SS7 GlobalTitle Indicators classes. The Numbering
 * Plans with integer value of 8 to 13 are spare, the rest are defined below.
 *
 * @version 1.0
 * @author Sun Microsystems Inc.
 *
 */
public class NumberingPlan {

    /**
     * Internal Constant available for switch statements
     */
    public final static int M_NP_UNKNOWN = 0;

    /**
     * Internal Constant available for switch statements
     */
    public final static int M_NP_ISDN_TEL = 1;

    /**
     * Internal Constant available for switch statements
     */
    public final static int M_NP_GENERIC = 2;

    /**
     * Internal Constant available for switch statements
     */
    public final static int M_NP_DATA = 3;

    /**
     * Internal Constant available for switch statements
     */
    public final static int M_NP_TELEX = 4;

    /**
     * Internal Constant available for switch statements
     */
    public final static int M_NP_MARITIME_MOBILE = 5;

    /**
     * Internal Constant available for switch statements
     */
    public final static int M_NP_LAND_MOBILE = 6;

    /**
     * Internal Constant available for switch statements
     */
    public final static int M_NP_ISDN_MOBILE = 7;

    /**
     * Internal Constant available for switch statements
     */
    public final static int M_NP_NETWORK = 8;

   private NumberingPlan(int value) {
      m_Value = value;
   }

   /**
   * String representation of the Constant class
   *
   * @return    String provides description of Constant
   */
   public String toString(){
      switch(m_Value){
        case M_NP_DATA:
            return "NP_DATA";
        case M_NP_ISDN_MOBILE:
            return "NP_ISDN_MOBILE";
        case M_NP_ISDN_TEL:
            return "NP_ISDN_TEL";
        case M_NP_LAND_MOBILE:
            return "NP_LAND_MOBILE";
        case M_NP_MARITIME_MOBILE:
            return "NP_MARITIME_MOBILE";
        case M_NP_NETWORK:
            return "NP_NETWORK";
        case M_NP_TELEX:
            return "NP_TELEX";
        case M_NP_UNKNOWN:
            return "NP_UNKNOWN";
        default:
            return "ERROR";
      }
   }

   /**
    * Gets the integer representation of the Constant class
    *
    * @return    integer provides value of Constant
    */
   public int intValue() {
      return m_Value;
   }


   // Internal variable to store the Constant Value in a useful format
   private final int m_Value;


    /**
    * Unknown Numbering Plan.
    */
    public final static NumberingPlan NP_UNKNOWN = new NumberingPlan(M_NP_UNKNOWN);

    /**
    * ISDN/Telephony Numbering Plan.
    */
    public final static NumberingPlan NP_ISDN_TEL = new NumberingPlan(M_NP_ISDN_TEL);

    /**
    * Generic Numbering Plan.
    */
    public final static NumberingPlan NP_GENERIC = new NumberingPlan(M_NP_GENERIC);

    /**
    * Data Numbering Plan.
    */
    public final static NumberingPlan NP_DATA = new NumberingPlan(M_NP_DATA);

    /**
    * Telex Numbering Plan.
    */
    public final static NumberingPlan NP_TELEX = new NumberingPlan(M_NP_TELEX);

    /**
    * Maritime Mobile Numbering Plan.
    */
    public final static NumberingPlan NP_MARITIME_MOBILE = new NumberingPlan(M_NP_MARITIME_MOBILE);

    /**
    * Land/Mobile Numbering Plan.
    */
    public final static NumberingPlan NP_LAND_MOBILE = new NumberingPlan(M_NP_LAND_MOBILE);

    /**
    * ISDN/Mobile Numbering Plan.
    */
    public final static NumberingPlan NP_ISDN_MOBILE = new NumberingPlan(M_NP_ISDN_MOBILE);

    /**
    * Private Network or Network Specific Numbering Plan.
    */
    public final static NumberingPlan NP_NETWORK = new NumberingPlan(M_NP_NETWORK);

}
