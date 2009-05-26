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
 *  File Name     : SPCFormat.java
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
 * Constants used by the JAIN SS7 SignalingPointCode class.
 *
 * @version 1.0
 * @author Sun Microsystems Inc.
 *
 */
public class SPCFormat {

    /**
     * Internal Constant available for switch statements
     */
    public final static int M_ANSI_SPC_FORMAT =0;

    /**
     * Internal Constant available for switch statements
     */
    public final static int M_ITU_SPC_FORMAT =1;


   /**
   * String representation of the Constant class
   *
   * @return    String provides description of Constant
   */
   public String toString(){
      switch(m_Value){
        case M_ITU_SPC_FORMAT:
            return "ITU_SPC_FORMAT";
			case M_ANSI_SPC_FORMAT:
            return "ANSI_SPC_FORMAT";
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
   private int m_Value;

   private SPCFormat(int value) {
      m_Value = value;
   }

   /**
   * ANSI SPC Format Constant
   */
   public static final SPCFormat ANSI_SPC_FORMAT   = new SPCFormat(M_ANSI_SPC_FORMAT);

   /**
   * ITU SPC Format Constant
   */
	public static final SPCFormat ITU_SPC_FORMAT = new SPCFormat(M_ITU_SPC_FORMAT);
	

		
}
