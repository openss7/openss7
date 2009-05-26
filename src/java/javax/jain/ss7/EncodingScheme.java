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
 *  File Name     : Encoding_Scheme.java
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
 * Constants used by the JAIN SS7 GlobalTitle classes
 *
 * @version 1.0
 * @author Sun Microsystems Inc.
 *
 */
public class EncodingScheme {

    /**
     * Internal Constant available for switch statements
     */
    public static final int M_ES_UNKNOWN=0;

    /**
     * Internal Constant available for switch statements
     */
    public static final int M_ES_ODD=1;

    /**
     * Internal Constant available for switch statements
     */
    public static final int M_ES_EVEN=2;

    /**
     * Internal Constant available for switch statements
     */
    public static final int M_ES_NATIONAL_SPECIFIC = 3;


   private EncodingScheme(int value) {
      m_Value = value;
   }

   /**
   * String representation of the Constant class
   *
   * @return    String provides description of Constant
   */
   public String toString(){
      switch(m_Value){
        case M_ES_EVEN:
            return "ES_EVEN";
        case M_ES_NATIONAL_SPECIFIC:
            return "ES_NATIONAL_SPECIFIC";
        case M_ES_ODD:
            return "ES_ODD";
        case M_ES_UNKNOWN:
            return "ES_UNKNOWN";
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
    * Indicates an unknown encoding scheme.
    */
    public final static EncodingScheme ES_UNKNOWN = new EncodingScheme(M_ES_UNKNOWN);

    /**
    * Indicates BCD, odd number of digits.
    */
    public final static EncodingScheme ES_ODD = new EncodingScheme(M_ES_ODD);

    /**
    * Indicates a BCD, with an even number of digits.
    */
    public final static EncodingScheme ES_EVEN = new EncodingScheme(M_ES_EVEN);

    /**
    * Indicates National Specific encoding scheme for ITU.
    * This encoding scheme is spare for ANSI.
    */
    public final static EncodingScheme ES_NATIONAL_SPECIFIC = new EncodingScheme(M_ES_NATIONAL_SPECIFIC);



}
