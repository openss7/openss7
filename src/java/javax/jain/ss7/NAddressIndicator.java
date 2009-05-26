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
 *  File Name     : NAddressIndicator.java
 *  Originator    : Phelim O'Doherty
 *  Approver      : Jain Community
 *
 *  HISTORY
 *  Version   Date      Author              Comments
 *  1.0     28/09/2001  Phelim O'Doherty    Initial version
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7;

/**
 * Constants used by the JAIN SS7 GlobalTitle Indicator classes. Integer values
 * 0 to 4 are defined below, from 5 to 127 are spare, were the eighth binary bit
 * is used as an even/odd indicator.
 *
 * @version 1.0
 * @author Sun Microsystems Inc.
 *
 */
public class NAddressIndicator {

    /**
     * Internal Constant available for switch statements
     */
    public final static int M_NA_UNKNOWN = 0;

    /**
     * Internal Constant available for switch statements
     */
    public final static int M_NA_SUBSCRIBER = 1;

    /**
     * Internal Constant available for switch statements
     */
    public final static int M_NA_RESERVED = 2;

    /**
     * Internal Constant available for switch statements
     */
    public final static int M_NA_NATIONAL_SIGNIFICANT = 3;

    /**
     * Internal Constant available for switch statements
     */
    public final static int M_NA_INTERNATIONAL = 4;


   private NAddressIndicator(int value) {
      m_Value = value;
   }

   /**
   * String representation of the Constant class
   *
   * @return    String provides description of Constant
   */
   public String toString(){
      switch(m_Value){
        case M_NA_INTERNATIONAL:
            return "NA_INTERNATIONAL";
        case M_NA_NATIONAL_SIGNIFICANT:
            return "NA_NATIONAL_SIGNIFICANT";
        case M_NA_RESERVED:
            return "NA_RESERVED";
        case M_NA_SUBSCRIBER:
            return "NA_SUBSCRIBER";
        case M_NA_UNKNOWN:
            return "NA_UNKNOWN";
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
    * Indicates a unknown Nature of Address Indicator.
    */
    public final static NAddressIndicator NA_UNKNOWN = new NAddressIndicator(M_NA_UNKNOWN);

    /**
    * Indicates a subscriber number.
    */
    public final static NAddressIndicator NA_SUBSCRIBER = new NAddressIndicator(M_NA_SUBSCRIBER);

    /**
    * Reserved for national use Nature of Address Indicator.
    */
    public final static NAddressIndicator NA_RESERVED = new NAddressIndicator(M_NA_RESERVED);

    /**
    * Indicates a National Significant Number.
    */
    public final static NAddressIndicator NA_NATIONAL_SIGNIFICANT = new NAddressIndicator(M_NA_NATIONAL_SIGNIFICANT);

    /**
    * Indicates an international number.
    */
    public final static NAddressIndicator NA_INTERNATIONAL = new NAddressIndicator(M_NA_INTERNATIONAL);


}
