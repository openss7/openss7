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
 *  File Name     : UserAddressEmptyException.java
 *  Originator    : Eugene Bell [AePONA]
 *  Approver      : Jain Tcap Edit Group
 *
 *  HISTORY
 *  Version   Date      Author              Comments
 *  1.1     21/6/2001  Eugene Bell        Initial version
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7;

/**
* This JAIN UserAddressEmptyException is a run time exception thrown if
* a listener with a user-address whose point-code does not match that of
* the stack that owns the provider the listener is registered on (i.e. the
* point code of the Listener does not match the point code of the Stack, which
* the provider is registered on, to which the Listener is registered to).
*
* @version 1.1
* @author Sun Microsystems Inc.
*/
public class UserAddressEmptyException extends SS7Exception {

    /**
    * Constructs a new <code>UserAddressEmptyException</code> with
    * the specified detail message.
    * @param <var>msg</var> the detail message
    */
    public UserAddressEmptyException(String msg) {
        super(msg);
    }

    /**
    * Constructs a new <code>UserAddressEmptyException</code>
    */
    public UserAddressEmptyException() {
        super();
    }
}

