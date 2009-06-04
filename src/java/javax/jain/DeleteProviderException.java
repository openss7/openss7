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
 * File Name     : DeleteProviderException.java
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
 *
 * The DeleteProviderException is thrown when a problem occurs while trying
 * to delete an object that implements any JAIN Provider interface.
 *
 * @author     Sun Microsystems
 * @version    1.0
 */
public class DeleteProviderException extends JainException {

    /**
    * Constructs a new <code>DeleteProviderException</code>
    */
    public DeleteProviderException() {
    }

    /**
    * Constructs a new <code>DeleteProviderException</code> with a specified
    * error message.
    *
    * @param  error the detail of the error message
    */
    public DeleteProviderException(String error) {
        super(error);
    }

    /**
    * Constructs a new <code>DeleteProviderException</code> with the specified
    * error message and specialized exception that triggered this error condition.
    *
    * @param  error the detail of the message
    * @param  excp  the specialized exception that triggered this exception
    */
    public DeleteProviderException(String error, Exception excp) {
        super(error, excp);
    }

}

