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
 *  File Name     : IdNotAvailableException.java
 *  Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 *  Approver      : Jain Tcap Edit Group
 *
 *  HISTORY
 *  Version   Date      Author              Comments
 *  1.1     16/10/2000  Phelim O'Doherty    Updated after public release and
 *                                          certification process comments.
 * 1.1      11/07/01      Eugene Bell       Updated for JAIN Exception strategy
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7;

// import jain.JainException;
/**
* This Exception is thrown to indicate that there
* are no more free Dialogue Ids or InvokeId
*
* @version 1.1
* @author Sun Microsystems Inc.
*
*/
public class IdNotAvailableException extends SS7Exception {

    /**
    * Constructs a new <code>IdNotAvailableException</code> with
    * the specified detail message.
    * @param <var>msg</var> the detail message
    */
    public IdNotAvailableException(String msg) {
        super(msg);
    }

    /**
    * Constructs a new <code>IdNotAvailableException</code>
    */
    public IdNotAvailableException() {
        super();
    }
}

