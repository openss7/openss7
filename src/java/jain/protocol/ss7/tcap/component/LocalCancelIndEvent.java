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
 * File Name     : LocalCancelIndEvent.java
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     14/11/2000  Phelim O'Doherty    Updated after public release and
 *                                         certification process comments.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7.tcap.component;

import jain.*;
import jain.protocol.ss7.tcap.*;

/**
 * An event representing a TCAP Local Cancel indication component primitive.
 * The mandatory parameters of this primitive are supplied to the constructor.
 * Optional parameters may then be set using the set methods.
 *
 * @author     Sun Microsystems Inc.
 * @version    1.1
 * @see        ComponentIndEvent
 */
public final class LocalCancelIndEvent extends ComponentIndEvent {

    /**
    * Constructs a new Local Cancel indication Event, with only the Event Source
    * and the <a href="package-summary.html">JAIN TCAP Mandatory</a> parameters
    * being supplied to the constructor.
    *
    * @param  source      the Event Source supplied to the constructor
    * @param  dialogueId  the Dialogue Identifier supplied to the constructor
    * @param  invokeId    the Invoke Identifier supplied to the constructor
    */
    public LocalCancelIndEvent(Object source, int dialogueId, int invokeId) {

        super(source);
        setDialogueId(dialogueId);
        setInvokeId(invokeId);
    }

    /**
    * Gets the Dialogue Id parameter of this Local Cancel Indication, which is a
    * reference identifier that is used to associate all components within a
    * transaction.
    *
    * @return the Dialogue Id of this Local Cancel indication
    * @exception  MandatoryParameterNotSetException  if this JAIN Mandatory
    * parameter has not been set.
    */
    public int getDialogueId() throws MandatoryParameterNotSetException {
        if (m_dialogueIdPresent == true) {
            return (m_dialogueId);
        } else {
            throw new MandatoryParameterNotSetException();
        }
    }

    /**
    * Gets the Invoke Id parameter of this Local Cancel indication. Invoke Id
    * identifies the operation invocation and its result.
    *
    * @return the Invoke Id of the indication event
    * @exception  MandatoryParameterNotSetException  this exception is thrown if
    * this is a JAIN Mandatory parameter has not been set
    */
    public int getInvokeId() throws MandatoryParameterNotSetException {
        if (m_invokeIdPresent == true) {
            return (m_invokeId);
        } else {
            throw new MandatoryParameterNotSetException();
        }
    }

    /**
    * This method returns the type of this primitive.
    *
    * @return  The Primitive Type of this event
    */
    public int getPrimitiveType() {
        return (jain.protocol.ss7.tcap.TcapConstants.PRIMITIVE_LOCAL_CANCEL);
    }

    /**
    * Clears all previously set parameters .
    */
    public void clearAllParameters() {
        m_dialogueIdPresent = false;
        m_invokeIdPresent = false;
    }

    /**
    * String representation of class LocalCancelIndEvent
    *
    * @return    String provides description of class LocalCancelIndEvent
    */
    public String toString() {
        StringBuffer buffer = new StringBuffer(500);
        buffer.append("\n\nLocalCancelIndEvent");
        buffer.append(super.toString());
        return buffer.toString();
    }
}

