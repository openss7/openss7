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
 *  File Name     : ProviderNotAttachedException.java
 *  Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 *  Approver      : Jain Tcap Edit Group
 *
 *  HISTORY
 *  Version   Date      Author              Comments
 *  1.1     16/10/2000  Phelim O'Doherty    Class Depreceted because of remaoval
 *                                          of attach and detach method in JainTcapStack.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7.tcap;

   /**
    * This Exception is thrown when an attempt is made to detach
    * a Provider Peer from the underlying stack, or retrieve a
    * Stack attached to a Provider Peer when the Provider
    * Peer is not already attached to a Stack.
    *
    * @version 1.1
    * @author Sun Microsystems Inc.
    *
    * @deprecated As of JAIN TCAP v1.1. This exception is no longer needed as a result
    * of the removal of the explicit attach and detach methods in the JainTcapStack Interface
    */
public class ProviderNotAttachedException extends TcapException {

    /**
    * Constructs a new <code>ProviderNotAttachedException</code> with
    * the specified detail message.
    * @param <var>msg</var> the detail message
    *
    */
    public ProviderNotAttachedException(String msg) {
        super(msg);
    }

    /**
    * Constructs a new <code>ProviderNotAttachedException</code>
    */
    public ProviderNotAttachedException() {
        super();
    }
}

