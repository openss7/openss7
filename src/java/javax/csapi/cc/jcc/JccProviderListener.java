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
    Interface for notifying changes happening in a {@link JccProvider}. These changes are
    reported as events to the JccProviderListener method corresponding to the type of event.
    Applications must instantiate an object which implements this interface and then use
    the {@link JccProvider#addProviderListener(JccProviderListener) addProviderListener(JccProviderListener)} 
    method to register the object to receive all future events associated with the JccProvider object.
    @since 1.0b
*/
public interface JccProviderListener extends EventListener {
    /**
        Indicates that the state of the JccProvider has changed to {@link JccProvider#IN_SERVICE}.

        @param providerevent JccProviderEvent with event ID PROVIDER_IN_SERVICE.
    */
    public void providerInService( JccProviderEvent providerevent );

    /**
        Indicates that the state of the JccProvider has changed to {@link JccProvider#OUT_OF_SERVICE}.

        @param providerevent JccProviderEvent with event ID PROVIDER_OUT_OF_SERVICE.
    */
    public void providerOutOfService( JccProviderEvent providerevent );

    /**
        Indicates that the state of the JccProvider has changed to {@link JccProvider#SHUTDOWN}.

        @param providerevent JccProviderEvent with event ID PROVIDER_SHUTDOWN.
    */
    public void providerShutdown( JccProviderEvent providerevent );
    /**
        Indicates that this instance of JccProviderListener 
        will no longer receive JccProvider events from the 
        instance of JccProvider.

        @param providerevent JccProviderEvent with event ID PROVIDER_EVENT_TRANSMISSION_ENDED.
    */
    public void providerEventTransmissionEnded( JccProviderEvent providerevent );
}
