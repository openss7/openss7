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
 *  File Name     : GTIndicator.java
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
 * Constants used by the JAIN SS7 GlobalTitle class.
 *
 * @version 1.0
 * @author Sun Microsystems Inc.
 *
 */
public class GTIndicator {

   private GTIndicator(String value) {
      m_Value = value;
   }

   /**
   * String representation of the Constant class
   *
   * @return    String provides description of Constant
   */
   public String toString(){
      return m_Value;
   }

   // Internal variable to store the Conatant Value in a useful format
   private final String m_Value;

   /**
   * Indicates a that the GlobalTitle is not included.
   */
   public static final GTIndicator GTINDICATOR_0000 = new GTIndicator("GTINDICATOR_0000");

   /**
   * Indicates a GTIndicator0001 primitive.
   */
   public static final GTIndicator GTINDICATOR_0001 = new GTIndicator("GTINDICATOR_0001");

   /**
   * Indicates a GTIndicator0010 primitive.
   */
   public static final GTIndicator GTINDICATOR_0010 = new GTIndicator("GTINDICATOR_0010");

   /**
   * Indicates a GTIndicator0011 primitive.
   */
   public static final GTIndicator GTINDICATOR_0011 = new GTIndicator("GTINDICATOR_0011");

   /**
   * Indicates a GTIndicator0100 primitive.
   */
   public static final GTIndicator GTINDICATOR_0100 = new GTIndicator("GTINDICATOR_0100");


}
