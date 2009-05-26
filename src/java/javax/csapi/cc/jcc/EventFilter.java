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
 *  Version       : 1.0b
 *  Notes         :
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */


   /**
    * An instance of this EventFilter is supplied to the event source
    * in the addxxxListener() method by the EventListener to indicate what Events are
    * required by the EventListener. When a {@link JccEvent} occurs, the event source
    * will call the predicate {@link #getEventDisposition(JccEvent)}
    * to determine if the Event should be fired to the EventListener.
    * Given an event, <CODE>getEventDisposition()</CODE> returns
    * <OL>
    * <LI>{@link #EVENT_DISCARD} if the listener is not interested in receiving the event.
    * <LI>{@link #EVENT_NOTIFY} if the listener should be sent a non-blocking notification.
    * <LI>{@link #EVENT_BLOCK} if the listener should be sent a blocking event (trigger).
    * This return value applies to {@link JccConnectionEvent}s only.
    * </OL>
    * The EventFilter while providing flexibility will impact the performance of the 
    platform. Hence,  the JCC implementation is expected to provide for some standard
    EventFilters as explained in the {@link JccProvider} interface. 
    @since 1.0
    */
public interface EventFilter {

   /**
    * This predicate indicates whether the specified Event is required
    * by an EventListener. This method will be called by the Event source
    * prior to firing the event. The return type can be either of 
     <OL>
     <LI>{@link #EVENT_DISCARD} if the listener is not interested in receiving the event.
     <LI>{@link #EVENT_NOTIFY} if the listener should be sent a non-blocking notification.
     <LI>{@link #EVENT_BLOCK} if the listener should be sent a blocking event (trigger).
     This return value applies to JccConnectionEvents only since the other type of events 
    are non-blocking.
     </OL>
    @return an int representing the disposition for the event concerned. 
    @param event specifies the event. 
    */
   public int getEventDisposition(JccEvent event);


   /**
    * Predicate return constant: Indicates that the specified event is not required.
    * This is one of the possible return values of <CODE>getEventDisposition()</CODE>
    */

   public static final int EVENT_DISCARD = 0;


   /**
    * Predicate return constant: Indicates that the specified event is required and
    * is a non-blocking Event (notification only), that is, call processing will not be suspended.
    * This is one of the possible return values of <CODE>getEventDisposition()</CODE>
    */
   public static final int EVENT_NOTIFY = 1;


   /**
    * Predicate return constant: Indicates that the specified event is required and
    * is a blocking Event, that is, call processing will be suspended until the
    * {@link JccConnection#continueProcessing()}
    * or any other valid method is called.
    * This is one of the possible return values of <CODE>getEventDisposition()</CODE>
    */
   public static final int EVENT_BLOCK =2;
}
