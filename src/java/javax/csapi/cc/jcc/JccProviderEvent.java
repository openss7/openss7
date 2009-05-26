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
    This is the base interface for all {@link JccProvider} related events. All events 
    which pertain to the JccProvider object must extend this interface. Events
    which extend this interface are reported via the {@link JccProviderListener} interface. 
    @since 1.0b
*/

public interface JccProviderEvent extends JccEvent {
    /**
    returns the JccProvider associated with this JccProvider Event. The result of this call might be a cast of the result of {@link JccEvent#getSource()}.
    @return The JccProvider associated with this event. 
    */
    public JccProvider getProvider();
    /**
         This indicates that the
        state of the JccProvider object has changed to {@link JccProvider#IN_SERVICE}. 
        This constant 
        indicates a specific event passed via a JccProviderEvent event and is reported on the
        JccProviderListener interface.
    */
    public static final int PROVIDER_IN_SERVICE     = 119;

    /**
         This also indicates that the
        state of the JccProvider object has changed to {@link JccProvider#OUT_OF_SERVICE}.
        This constant 
        indicates a specific event passed via a JccProviderEvent event and is reported on the
        JccProviderListener interface.
    */
    public static final int PROVIDER_OUT_OF_SERVICE = 120;

    /**
        This also indicates that the
        state of the JccProvider object has changed to {@link JccProvider#SHUTDOWN}.
        This constant 
        indicates a specific event passed via a JccProviderEvent event and is reported on the
        JccProviderListener interface.
    */
    public static final int PROVIDER_SHUTDOWN = 121;
    /**
        indicates that the listener will no longer receive 
        JccProvider Events from the sending JccProvider instance.
        This constant indicates a specific event passed via a JccProviderEvent 
        event and is reported on the JccProviderListener interface.
    */
    public static final int PROVIDER_EVENT_TRANSMISSION_ENDED = 122;
}
