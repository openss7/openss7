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
 *  File Name     : JainSS7Factory.java
 *  Author        : Colm Hayden & Phelim O'Doherty [AePONA]
 *  Approver      : Jain Tcap Edit Group
 *  Version       : 1.1
 *  Notes         :
 *
 * HISTORY
 * Version   Date      Author              Comments
 *  1.1     16/07/01    Eugene Bell         Updated for JAIN Exception strategy
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

 package javax.jain.protocol.ss7;

   /**
    * The SS7 Peer Unavailable Exception indicates that the JAIN SS7 Peer (a particular implementation
    * of JAIN SS7) is unavailable on the current system.
    * This Exception is thrown when the method in the class specified is invoked and the class
    * specified by <b>className</b> in not available.
    * <P>
    * This Exception may be thrown for any one of the following reasons:
    * <UL>
    * <LI>The supplied className refers to a class that is <i>not</i> located within class
    * searchpath.
    * <LI>The peer JAIN SS7 class <i>is</i> located within the
    * <a href="JainSS7Factory.html#setPathName(java.lang.String)">pathName</a> but the
    * supplied classname is incorrect. Note that a fully qualified classname
    * must be supplied. ie. - "jain.protocol.ss7.tcap.JainTcapStackImpl"
    * </UL>
    *
    * @version 1.1
    * @author Sun Microsystems Inc.
    *
    * @deprecated As of JAIN TCAP v1.1. This class is no longer needed as a result
    * of the addition of the {@link jain.PeerUnavailableException} class.
    */

public class SS7PeerUnavailableException extends SS7Exception {

   /**
    * Constructs a new SS7 Peer Unavailable Exception with
    * the specified message detail.
    * @param <var>msg</var> the message detail of this Exception.
    */

   public SS7PeerUnavailableException(String msg) {
      super(msg);
   }

   /**
    * Constructs a new SS7 Peer Unavailable Exception.
    */

   public SS7PeerUnavailableException() {
      super();
   }
}


