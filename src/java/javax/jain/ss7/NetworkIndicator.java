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
 *  File Name     : NetworkIndicator.java
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
 * Constants used by the JAIN SS7 Common Address Schema.
 *
 * @version 1.0
 * @author Sun Microsystems Inc.
 *
 */
public class NetworkIndicator {

    /**
     * Internal Constant available for switch statements
     */
    public final static int M_NI_INTERNATIONAL_00 = 0;

    /**
     * Internal Constant available for switch statements
     */
    public final static int M_NI_INTERNATIONAL_01 = 1;

    /**
     * Internal Constant available for switch statements
     */
    public final static int M_NI_NATIONAL_10 = 2;

    /**
     * Internal Constant available for switch statements
     */
    public final static int M_NI_NATIONAL_11 = 3;


   private NetworkIndicator(int value) {
      m_Value = value;
   }

   /**
   * String representation of the Constant class
   *
   * @return    String provides description of Constant
   */
   public String toString(){
      switch(m_Value){
        case M_NI_NATIONAL_11:
            return "NI_NATIONAL_11";
        case M_NI_NATIONAL_10:
            return "NI_NATIONAL_10";
        case M_NI_INTERNATIONAL_01:
            return "NI_INTERNATIONAL_01";
			case M_NI_INTERNATIONAL_00:
            return "NI_INTERNATIONAL_00";
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
    * Indicates the International Network 00, which is International.
    */
    public final static NetworkIndicator NI_INTERNATIONAL_00 = new NetworkIndicator(M_NI_INTERNATIONAL_00);

    /**
    * Indicates the International Network 01, which is International spare.
    */
    public final static NetworkIndicator NI_INTERNATIONAL_01 = new NetworkIndicator(M_NI_INTERNATIONAL_01);

    /**
    * Indicates the National Network 10, which is national.
    */
    public final static NetworkIndicator NI_NATIONAL_10 = new NetworkIndicator(M_NI_NATIONAL_10);

    /**
    * Indicates the National Network 11, which is national reserved.
    */
    public final static NetworkIndicator NI_NATIONAL_11 = new NetworkIndicator(M_NI_NATIONAL_11);
}
