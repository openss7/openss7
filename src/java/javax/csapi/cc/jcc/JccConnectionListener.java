package javax.csapi.cc.jcc;

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
    This interface is an extension of the JccCallListener  
    interface and reports
    state changes both of the {@link JccCall} and its {@link JccConnection}s. 
    @since 1.0
*/
public interface JccConnectionListener extends JccCallListener {

    /**
        Indicates that the JccConnection has just been placed in the {@link JccConnection#AUTHORIZE_CALL_ATTEMPT} state

        @param connectionevent JccConnectionEvent.
    */
    public void connectionAuthorizeCallAttempt(JccConnectionEvent connectionevent );

    /**
       Indicates that the JccConnection has just been placed in the {@link JccConnection#ADDRESS_COLLECT} state

        @param connectionevent JccConnectionEvent.
    */
    public void connectionAddressCollect(JccConnectionEvent connectionevent );

    /**
        Indicates that the JccConnection has just been placed in the {@link JccConnection#ADDRESS_ANALYZE} state

        @param connectionevent JccConnectionEvent.
    */
    public void connectionAddressAnalyze( JccConnectionEvent connectionevent );

    /**
       Indicates that the JccConnection has just been placed in the {@link JccConnection#CALL_DELIVERY} state

        @param connectionevent JccConnectionEvent.
    */
    public void connectionCallDelivery( JccConnectionEvent connectionevent );
    
    /**
       Indicates that the JccConnection has just experienced a mid call event.  To learn more about the specifics of the 
       mid call event, {@link JccConnection#getMidCallData()} can be queried.

        @param connectionevent JccConnectionEvent.
		@since 1.0b
    */
    public void connectionMidCall( JccConnectionEvent connectionevent );
    //--------------------------------------------------------------------------
    //FROM JCP
    
    /**
        Indicates that the {@link JccConnection} object has just been created.

        @param connectionevent event resulting from state change.
		@since 1.0b
    */
    public void connectionCreated( JccConnectionEvent connectionevent );

    /**
       Indicates that the {@link JccConnection} has just been placed in the {@link JccConnection#ALERTING} state

        @param connectionevent event resulting from state change.
		@since 1.0b
    */
    public void connectionAlerting( JccConnectionEvent connectionevent );

    /**
       Indicates that the {@link JccConnection} has just been placed in the {@link JccConnection#CONNECTED}  state

        @param connectionevent event resulting from state change.
		@since 1.0b
    */
    public void connectionConnected( JccConnectionEvent connectionevent );

//    /**
//       Indicates that the {@link JccConnection} has just been placed in the {@link JccConnection#INPROGRESS} state
//
//        @param connectionevent event resulting from state change.
//       
//    */
//    public void connectionInProgress( JccConnectionEvent connectionevent );

    /**
       Indicates that the {@link JccConnection} has just been placed in the {@link JccConnection#FAILED}  state

        @param connectionevent event resulting from state change.
		@since 1.0b
    */
    public void connectionFailed( JccConnectionEvent connectionevent );

    /**
       Indicates that the {@link JccConnection} has just been placed in the {@link JccConnection#DISCONNECTED}  state

        @param connectionevent event resulting from state change.
		@since 1.0b
    */
    public void connectionDisconnected( JccConnectionEvent connectionevent );
//    /**
//       Indicates that the {@link JccConnection} has just been placed in the {@link JccConnection#UNKNOWN}  state
//
//        @param connectionevent event resulting from state change.
//		@since 1.0b
//    */
//    public void connectionUnknown( JccConnectionEvent connectionevent );


}

