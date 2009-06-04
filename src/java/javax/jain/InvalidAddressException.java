/*
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Copyrights:
 *
 * Copyright - 1999 Sun Microsystems, Inc. All rights reserved.
 * 901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 * This product and related documentation are protected by copyright and
 * distributed under licenses restricting its use, copying, distribution, and
 * decompilation. No part of this product or related documentation may be
 * reproduced in any form by any means without prior written authorization of
 * Sun and its licensors, if any.
 *
 * RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 * States Government is subject to the restrictions set forth in DFARS
 * 252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 * The product described in this manual may be protected by one or more U.S.
 * patents, foreign patents, or pending applications.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Author        : Sun Microsystems Inc.
 * Module Name   : JAIN Common API
 * File Name     : InvalidAddressException.java
 * Originator    : Phelim O'Doherty
 * Approver      : Jain Common Expert Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 *  1.0     25/09/01   Phelim O'Doherty    Created for JAIN Exception strategy
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain;

/**
* This JAIN InvalidAddressException is thrown if a listener registers with an
* address that is not handled by the domain of the the stack object that owns
* the provider object to which the listener is trying to register.
*
* @version 1.0
* @author Sun Microsystems
*/
public class InvalidAddressException extends InvalidArgumentException {

    /**
    * Constructs a new <code>InvalidAddressException</code>
    */
    public InvalidAddressException() {
        super();
    }

    /**
    * Constructs a new <code>InvalidAddressException</code> with the specified
    * error message.
    *
    * @param error the detail of the error message
    */
    public InvalidAddressException(String error) {
        super(error);
    }


    /**
    * Constructs a new <code>InvalidAddressException</code> with the specified
    * error message and specialized exception that triggered this error condition.
    *
    * @param  error the detail of the message
    * @param  excp  the specialized exception that triggered this exception
    */
    public InvalidAddressException(String error, Exception excp) {
        super(error, excp);
    }

}

