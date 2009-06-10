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
 * Author:
 *
 * AePONA Limited, Interpoint Building
 * 20-24 York Street, Belfast BT15 1AQ
 * N. Ireland.
 *
 *
 * Module Name   : JAIN TCAP API
 * File Name     : TcapErrorEvent.java
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     13/11/2000  Phelim O'Doherty    Added toString() method
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7.tcap;

import jain.protocol.ss7.*;
import jain.*;

/**
 * This Event may be emitted by a <a href="JainTcapProvider.html">
 * JainTcapProvider</a> if the Provider encounters an Error that cannot be
 * passed to all registered <a href="JainTcapListener.html">JainTcapListeners
 * </a>through normal TCAP error handling mechanisms.
 *
 * @author     Sun Microsystems Inc.
 * @version    1.1
 */
public class TcapErrorEvent extends java.util.EventObject implements java.io.Serializable, java.lang.Cloneable {

    /**
    * Constructs a new TcapErrorEvent containing the user defined error object.
    *
    * @param  source             the new Object source supplied to the constructor
    * @param  error              the new error supplied to the constructor
    */
    public TcapErrorEvent(java.lang.Object source, java.lang.Object error) {
        super(source);
        this.setError(error);
    }

    /**
    * Sets the Error attribute of the TcapErrorEvent object
    *
    * @param  error  The new Error value
    */
    public void setError(java.lang.Object error) {
        m_error = error;
    }

    /**
    * Gets the Error attribute of the TcapErrorEvent object
    *
    * @return    The Error value
    */
    public java.lang.Object getError() {
        return m_error;
    }

    /**
    * String representation of class TcapErrorEvent
    *
    * @return    String provides description of class TcapErrorEvent
    */
    public java.lang.String toString() {
        StringBuffer buffer = new StringBuffer(500);
        buffer.append(super.toString());
        buffer.append("\n\nerror = ");
        if (m_error != null) {
            buffer.append(m_error.toString());
        } else {
            buffer.append("value is null");
        }
            return buffer.toString();
    }

    /**
    * @serial    m_error - a default serializable field
    */
    private java.lang.Object m_error;
}

