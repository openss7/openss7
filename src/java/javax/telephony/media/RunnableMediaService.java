/*
 * RunnableMediaService.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

/** 
 * Interface for application classes that can be instantiated 
 * "on-demand" by a call management
 * <a href="MediaService.html#framework">framework</a>.
 * <p>
 * Some large server installations use statistical overloading techniques;
 * installing and enabling access to many applications with the constaint 
 * that they cannot and will not all run simultaneously. 
 * Such installations cannot 
 * in general allocate a thread for every <em>potential</em> call-service.
 * <p>
 * Therefore, applications in these installations should not take the
 * initiative to instantiate a MediaService object and should not
 * invoke {@link MediaService#bindToServiceName bindToServiceName}
 * until the framework needs their service.
 * <p>
 * Frameworks that use this technique typically perform
 * call-type or service-request discrimination,
 * instantiate the implementation object for the requested service,
 * and then create a thread to run that service for this call.
 * <p>
 * Applications can prepare for such environments by following these
 * conventions: <ul>
 * <li> put the interesting execution code in a run() method </li>
 * <li> do not hard-code the service name used in bindToServiceName() </li>
 * <li> the MediaService should invoke bindToServiceName()
 * using a service name assigned for each Call </li>
 * <li> return from the run method after handling one Call </li>
 * </ul>
 * Application classes can declare that they follow these conventions
 * by implementing the <b>RunnableMediaService</b> interface.
 * <p>
 * <h4>Usage scenario:</h4>
 * Based on the call-type or other information, 
 * the framework assigns a (possibly unique) serviceName for the Call,
 * and identifies the application class to provide that service for the Call.
 * Because the application promises to be a RunnableMediaService, the
 * framework can tell the application which <code>serviceName</code> to use.
 * The following code sample demonstrates how the framework can
 * instantitate and run the application class: 
 *
 * <pre> // when framework wants <i>appClassName</i> to handle a Call
 * // and that Call will be given to this <i>serviceName</i>
 * runService(String appClassName, String serviceName) {
 *     RunnableMediaService rms;
 *     Class appClass = classForName(appClassName); // the application Class
 *     rms = (RunnableMediaService)(appClass.newInstance());
 *     rms.setServiceName(serviceName);  // serviceName determined by framework
 *     Thread thr = new Thread(rms).start();
 * } </pre>
 * <p>
 * The application must create a MediaService object (within the
 * appClass constructor or the run method), and must eventually
 * respond to the start/run method with: <br><pre>
 *     bindToServiceName(initialConfigSpec, serviceName);</pre>
 * The framework can rely on the specific <code>serviceName</code> to recognize
 * that the bind request is from the <code>appClass</code> application.
 * Thereafter, the focus is on the MediaService, and in normal usage,
 * the framework does not directly manipulate the Runnable nor the Thread.
 * <p>
 * The well behaved RunnableMediaService should process the call, 
 * including release() and any cleanup or record keeping,
 * and then return from run(). The whole purpose of this scheme 
 * is to limit/control the existance of outstanding threads;
 * applications that subvert the framework by holding the thread or
 * other resources beyond the expected lifetime for processing 
 * a call are considered non-conformant to this interface.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public 
interface RunnableMediaService extends Runnable {
     /** 
      * Tell the RunnableMediaService application 
      * which <code>serviceName</code> it should use.
      * A dynamic instantiation framework can use this to 
      * identify specific application instances.
      * <p>
      * A RunnableMediaService promises to:<pre>
      * 	bindToServiceName(ConfigSpec, serviceName);
      * </pre>
      * <p>
      * @param serviceName the String to be used
      * in this application's subsequent bindToServiceName()
      * @see MediaService#bindToServiceName bindToServiceName
      */
     void setServiceName(String serviceName);
 }
