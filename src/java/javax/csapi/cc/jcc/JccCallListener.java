package javax.csapi.cc.jcc;

import java.util.EventListener;

/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Copyrights:
 *
 *  Copyright - 2001, 2002 Sun Microsystems, Inc. All rights reserved.
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
 *  Telcordia Technologies, Inc.
 *  Farooq Anjum, John-Luc Bakker, Ravi Jain
 *  445 South Street
 *  Morristown, NJ 07960
 *
 *  Version       : 1.1
 *  Notes         :
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/**
    This interface reports all changes to the Call object.
 * The {@link JccCallEvent} interface is the base interface for
 * all Call-related events.
 * All Call-related events must extend this interface.
 * Events which extend this interface are reported via the
 * <CODE>JccCallListener</CODE> interface.
 *
 * <p>
 * An individual <CODE>JccCallEvent</CODE> conveys one of a series of different
 * possible Call state changes;
 * the specific Call state change is indicated by
 * the {@link JccEvent#getID()}
 * value returned by the event.
 *
 * <p>
 * The {@link JccConnectionListener}
 * interface extends this
 * interface. This reflects the fact that all Connection 
 * events can be reported via the <CODE>JccCallListener</CODE> interface.
 *
 * <p>
 * The {@link JccCallEvent#getCall()} method on this interface returns the
 * Call
 * associated with the Call event.
 @since 1.0
 */

/**
    This interface reports all changes to the {@link JccCall} object.
 * <p>
 * The JccConnectionListener
 * interface extends this
 * interface. This reflects the fact that all {@link JccConnectionEvent}s and {@link JccCallEvent}s
 * events can be reported via the JccConnectionListener interface.
 @since 1.0
*/
public interface JccCallListener extends EventListener {
    /**
        Indicates that the supervision of the call has started.

        @param callevent JccCallevent.
    */
    public void callSuperviseStart( JccCallEvent callevent );

    /**
        Indicates that the supervision of the call has ended.

        @param callevent JccCallevent.
    */
    public void callSuperviseEnd( JccCallEvent callevent );
    
    //-------------------------------------------------------------------------
    
    /**
        Indicates that the state of the {@link JccCall} object has changed to {@link JccCall#ACTIVE}.

        @param callevent JccCallEvent with eventID {@link JccCallEvent#CALL_ACTIVE}.
		@since 1.0b
    */
    public void callActive( JccCallEvent callevent );

    /**
        Indicates that the state of the {@link JccCall} object has changed to {@link JccCall#INVALID}.

        @param callevent JccCallEvent with eventID {@link JccCallEvent#CALL_INVALID}.
		@since 1.0b
    */
    public void callInvalid( JccCallEvent callevent );


    /**
        This method is called to indicate that the application will no longer receive 
        JccCallEvent events of the call on the instance of 
        the JccCallListener.

        @param callevent JccCallEvent with eventID {@link JccCallEvent#CALL_EVENT_TRANSMISSION_ENDED}.
		@since 1.0b
    */
    public void callEventTransmissionEnded( JccCallEvent callevent );
    /**
        Indicates that the state of the JccCall object has changed to {@link JccCall#IDLE}.

        @param callevent JccCallEvent with eventID {@link JccCallEvent#CALL_CREATED}.
		@since 1.0b
    */
    public void callCreated( JccCallEvent callevent );
}

