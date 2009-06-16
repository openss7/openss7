/*
 * MediaProvider.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
 * ECTF S.410-R2 Source code distribution.
 *
 * Copyright (c) 2002, Enterprise Computer Telephony Forum (ECTF),
 * All Rights Reserved.
 *
 * Use and redistribution of this file is subject to a License.
 * For terms and conditions see: javax/telephony/media/LICENSE.HTML
 * 
 * In short, you can use this source code if you keep and display
 * the ECTF Copyright and the License conditions. The code is supplied
 * "AS IS" and ECTF disclaims all warranties and liability.
 */

package javax.telephony.media;

import javax.telephony.media.container.ContainerService;
import javax.telephony.media.messaging.MessagingService;

/**
 * A marker interface indicating that this object (possibly a JTAPI Provider)
 * can be used directly as an argument to construct a MediaService object.
 * <p>
 * A MediaProvider implementation will provide the media services
 * (for Resource allocation and configuration), call processing services
 * for bind and release, and may provide other services such as OA&M.
 * <p>
 * A MediaService must be associated with some implementation
 * that provides the requisite call and media services.
 * The simplest way to find the media services implementation
 * is to use the default obtained by using the no argument constructor.
 * <p>
 * The most general way for an application to specify the 
 * media service implementation to be used is by supplying a 
 * JtapiPeer class name and a peer-specific <i>providerString</i> 
 * to a MediaService constructor.
 * <p>
 * Sometimes, the media services implementation is obtained by
 * an alternative, vendor specific technique. For example, a vendor
 * may specify that its JTAPI Provider object supports media services.
 * In these cases, it should be declared that the object
 * <code>implements MediaProvider</code>, so the object 
 * may be cast to a MediaProvider and used in the constructor
 * for a MediaService.
 * <p>
 * @see BasicMediaService#BasicMediaService(MediaProvider)
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public 
interface MediaProvider {
    /**
     * Return an unbound instance of this MediaProvider's preferred 
     * implementation of MediaService.
     * <p>
     * This precludes the application from extending the MediaService
     * class with their own methods, but guarantees getting the
     * most capable extension above BasicMediaService, without
     * using the class name in the application.
     * <p>
     * This method allows a <i>portable</i> application to get a MediaService
     * that has been extended by the MediaPeer vendor, without knowing
     * the actual class name or the actual extensions. The application
     * can then use the <tt>instanceof</tt> operator to see which of
     * the standard optional interfaces has been implemented.
     * Then the application can then upgrade or degrade its capabilities
     * based on that information.
     * <p>
     * For example:
     * <pre>
     * MediaProvider mp = ...;	// get a MediaProvider
     * MediaService ms = mp.getMediaService(); // get preferred MediaService
     * if( ms instanceof ASR ) { ... } // use ASR capabilities
     * if( ms instanceof Delegation ) { ... } // use Delegation capabilites
     * if( ms instanceof Async.MediaService ) { ... } // use Async capabilities
     * if( ms instanceof MediaConnection ) { ... } // use MediaConnection capabilites
     * </pre>
     *
     * @return an unbound MediaService object.
     */
    MediaService getMediaService();

    /** 
     * Return an empty ConfigSpec extended for this MediaProvider.
     * Maybe get the ConfigSpec for BasicMediaService?
     * <p>
     * Application alters using ConfigSpec <tt>set</tt> methods.
     */
    // ConfigSpec getConfigSpec();

    /** Add Listener to this MediaProvider.
     * Events from a MediaProvider are dispatched to MediaProviderListener:
     * <br><tt>MediaProviderListener.onProviderEvent(MediaProviderEvent event)</tt>
     * @param listener a MediaListener for this MediaProvider
     */
    // public void addListener(java.util.EventListener listener);

    /** Remove Listener from this MediaProvider.
     * Events from a MediaProvider are dispatched to MediaProviderListener.
     * @param listener a MediaListener for this MediaProvider
     */
    // public void removeListener(java.util.EventListener listener);
}
