/*
 * MediaService.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

import javax.telephony.*;		// get core Objects and Exceptions
import java.util.Dictionary;
import java.util.EventListener;

/**
 * Defines the methods for binding and controlling
 * the media processing resources associated with a Terminal.
 *
 * <h4>Introduction:</h4>
 *
 * A MediaService object represents the
 * "service" that an application provides to a caller on the telephone.
 * A media service encompasses the concepts of IVR and VoiceMail 
 * services and is similar to the concept of an "agent" as used in 
 * a call center environment. 
 * A MediaService plays the role of a "robotic agent" assigned to
 * the Terminal, handling the Call that is active on that Terminal.
 * <p>
 * In contrast to the usual scenario for live-agents in a call center,
 * in which the call is routed and connected to the agent's Terminal; 
 * in the JTAPI Media environment the MediaService application 
 * is <i>brought to a Terminal</i> on which the call is connected.
 * This approach allows a series of MediaService applications to 
 * interact with the remote user without recourse to switching the call.
 * Of course, using JTAPI Media does not preclude routing a call
 * to other Addresses and Terminals.
 *
 * <h4>Bind and Release:</h4>
 *
 * When a MediaService instance is "brought to a Terminal", 
 * the MediaService is said to be <b>bound</b> to
 * <i>a collection of media processing resources that are connected 
 * to the media streams</i>
 * of that Terminal.
 * A "collection of media processing resources that are connected 
 * to the media streams" [of a Terminal] is quite wordy, so we
 * abbreviate and use the term <b>MediaGroup</b>.
 * Therefore, one should properly say
 * "the MediaService is bound to a MediaGroup". However, since that 
 * MediaGroup is [generally] connected to the streams of a Terminal,
 * we sometimes say "the MediaService is bound to the Terminal".
 * <p>
 * <b>Note:</b>
 * The proper phrase is preferred because the attributes and methods
 * of the MediaGroup are accessible from the MediaService 
 * when it is bound to that MediaGroup. 
 * In contrast,
 * the methods of the Terminal are only accessible from the Terminal,
 * and that access is independent of whether the Terminal is 
 * otherwise associated with the MediaService.
 * <p>
 * When a MediaService is bound to a MediaGroup, 
 * the MediaService can control the media resources to
 * play prompts and record messages, generate and detect tones
 * or other communications signals for the Terminal.
 * <p>
 * When the MediaService is finished, it will <b>release</b> the MediaGroup.
 * At that point, another MediaService may continue to process the call
 * (in which case, that other MediaService gets bound to the MediaGroup),
 * or the Connection to the Terminal may be dropped 
 * (possibly in conjunction with routing the call to some other Address).
 * <p>
 * <a name="framework"><!-- --></a>
 * <b>Note:</b>
 * The bind and release methods presume some collection of administrative
 * and call management support outside of the application. Throughout
 * this document, this collection of service is refered to as the
 * implementation or administrative or installation <b>framework</b>. 
 * <p>
 * The framework may be as simple as routing all calls
 * to a single application, or as complex as a
 * geographically distributed call management system using SS7.
 * Whatever the framework is, the implementation of the MediaService
 * isolates an application from the details of that framework;
 * so an application can be portable across a variety of simple or
 * complex frameworks.
 * <p>
 * <a name="bindMethods"><!-- --></a>
 * <h4>Bind methods:</h4>
 *
 * There are several methods that bind a MediaService to a Terminal.
 * They are collectively refered to as the <b>bind methods</b>.
 * The methods all function similarly, and are differentiated primarily
 * in how each determines the Terminal to bind.
 * <p>
 * The Terminal to bind 
 * (that is: the Terminal whose streams are connected
 * to the MediaGroup to which the MediaService is bound)
 * is determined for each method as follows:
 * <table border="1" cellpadding="3">
 * <tr><td align=top>Method</td><td>binds to:</td></tr>
 * <tr><td align=top>
 * {@link #bindAndConnect(ConfigSpec,String,String)}</td>
 * 	<td>a Terminal connected to a new (outbound) Call</td></tr>
 * <tr><td align=top>
 * {@link #bindToServiceName(ConfigSpec,String)}</td>
 * 	<td>a Terminal connected to a Call 
 *		that is routed to the named service</td></tr>
 * <tr><td align=top>
 * {@link #bindToCall(ConfigSpec,Call)}</td>
 * 	<td>a Terminal added to a specified Call</td></tr>
 * <tr><td align=top>
 * {@link #bindToTerminalName(ConfigSpec,String)}</td>
 * 	<td>a Terminal named by the specified String</td></tr>
 * <tr><td align=top>
 * {@link #bindToTerminal(ConfigSpec,Terminal)}</td>
 * 	<td>the specified Terminal object</td></tr>
 * </table>
 *
 * <h4>Details of binding:</h4>
 *
 * All the bind methods attempt to allocate media processing resources 
 * and connect them to the media streams associated with the Terminal.
 * Again, the "media processing resources connected to the media streams" 
 * of a Terminal are called a <b>MediaGroup</b>.
 * <p>
 * The first argument to the bind methods, a {@link ConfigSpec},
 * specifies the particular media processing resources to be included 
 * in the MediaGroup and parameters that control how they are connected.
 * <p>
 * To <b>configure</b> means to find and connect the 
 * media processing resources that implement the interfaces and 
 * attributes specified in the ConfigSpec.
 * When the configure is successful, the methods of the requested
 * resource interfaces are available from the MediaService.
 * Typically, the methods are implemented by the MediaGroup 
 * to which the MediaService is bound.
 * <p>
 * All the bind methods share these common semantics:
 * <ul><b>Preconditions:</b>
 * <li>The application is ready to process a call,</li>
 * <li>the MediaService is not already bound. </li>
 * <li>The MediaService implementation can access the Terminal or Call</li>
 * </ul>
 * The bind methods block until the post-conditions are satisfied.
 * <ul><b>Postconditions:</b>
 * <li>A Connection is made to a suitable Terminal</li>
 * <li>That Terminal's media streams are connected to a MediaGroup</li>
 * <li>That MediaGroup is configured according to the given ConfigSpec</li>
 * <li>That MediaGroup is bound to this MediaService</li>
 * </ul>
 * The bind methods return the <code>Connection</code> to the Terminal,
 * or <code>null</code> if there is no Connection.
 *
 * <a name="Release"><!-- --></a>
 * <h4>Release:</h4>
 * 
 * A MediaService is generally bound to a MediaGroup only for the
 * duration of a single call. When the MediaService is done servicing
 * a call, the MediaGroup is released. 
 * When a MediaGroup is released the resources and UserDictionary 
 * are no longer available to this MediaService.
 * <p>
 * For all MediaService <i>release</i> methods:
 * <br>pre-conditions: <tt>(isBound() && {@link Delegation#isOwner()})</tt>
 * <br>post-conditions: <tt>(!isBound())</tt>
 * <p>
 * There are several forms of release:
 * The simple {@link #release()} indicates that this MediaService
 * has completed its processing of the call, and control of 
 * the call passes to the previous owner.  In the simple cases,
 * ownership returns to the implementation or installation 
 * <a href=#framework>framework</a> which is empowered and directed 
 * to disconnect the Connection and recycle any resources.
 * <p>
 * In contrast, {@link #releaseToService(String, int)} indicates that
 * further processing is expected, and the supplied disposition string
 * determines which service (or Address) should continue the processing.
 * The disposition strings are application specific, and are mapped to
 * actual services or Addresses via implementation specific 
 * administrative interfaces.
 * <p>
 * If {@link Delegation <i>delegation</i>} is being used, 
 * then there are other alternatives.
 * An application can use {@link #releaseToTag(String) releaseToTag(returnTag)}
 * to return control to a previous owner.  The <tt>returnTag</tt> is
 * primarily designed to select which previous owner, but may also
 * encode the status or cause for the return.
 * <p>
 * The method {@link #releaseToDestroy()} is a simplified form 
 * of <tt>releaseToTag(String)</tt>
 * which directs the call to the system framework with an indication
 * that the call should be disconnected.
 * <p>
 * <a name="disconnectProcessing"><!-- --></a>
 * <h4>Disconnect processing:</h4>
 *
 * When an active Connection to the bound Terminal is disconnected
 * (that is, when it achieves the <code>Connection.DISCONNECTED</code> state),
 * the method {@link MediaServiceListener#onDisconnected onDisconnected}
 * is invoked for each MediaServiceListener.
 * <p>
 * The effect of disconnect on media operations is controlled by the 
 * attribute {@link ConfigSpecConstants#a_StopOnDisconnect <code>a_StopOnDisconnect</code>}.
 * When the Connection to the Terminal is disconnected and
 * <code>(a_StopOnDisconnect == TRUE)</code>,
 * then<ul>
 * <li>any current media resource operations terminate, and throw
 * {@link DisconnectedException}.</li>
 * <li>any new media resource operations throw
 * {@link DisconnectedException}.</li>
 * </ul>
 * <p>
 * When all Connections to a Terminal are disconnected (or failed)
 * some implementations may choose to recycle the resources.
 * This feature is identified by the attribute
 * <code>{@link ConfigSpecConstants#a_DeallocateOnIdle a_DeallocateOnIdle}</code>.
 * When there are no viable Connections to the Terminal
 * and <code>(a_DeallocateOnIdle == TRUE)</code>,
 * then<ul>
 * <li>attempts to Configure may fail, and</li>
 * <li>the resource parameters may not be accessible.</li>
 * </ul>
 * Note: the MediaGroup is still bound, and
 * the UserDictionary is still accessible until
 * the MediaService is unbound by {@link #release release}.
 *
 * <a name="Associated data"><!-- --></a>
 * <h4>Associated data:</h4>
 *
 * There are several sets of information associated with a MediaService:
 * <ul>
 * <li> the UserDictionary</li>
 * <li> the Parameters</li>
 * <li> the Attributes</li>
 * <li> the Configuration</li>
 * </ul>
 *
 * <h5>UserDictionary:</h5>
 * The <i>UserDictionary</i> is a Dictionary of key-value pairs associated 
 * with the MediaGroup to which the MediaService is bound. 
 * If the MediaGroup is released to another MediaService, 
 * the UserDictionary goes with the MediaGroup.
 * This allows cooperating applications to easily share information.
 * The UserDictionary is accessed using {@link #getUserValues getUserValues},
 * {@link #setUserValues setUserValues} and 
 * {@link #setUserDictionary setUserDictionary}.
 * <p>
 * The contents of the UserDictionary are defined and controlled by
 * applications. 
 * This specification does not require the implementation/server 
 * to supply particular content for the UserDictionary. 
 * However, the framework or other applications
 * are allowed to set values in the UserDictionary.
 *
 * <h5>Parameters:</h5>
 * The <I>Parameters</I> of a MediaService are quantities that effect
 * the control or status of the MediaGroup and its Resources.
 * Resources may report various aspects of their state via parameters.
 * Applications control or modify various aspects of a Resource
 * by setting the appropriate Parameters.
 * Parameters are accessed using {@link #getParameters getParameters}
 * and {@link #setParameters setParameters}.
 *
 * <h5>Attributes:</h5>
 * The <i>Attributes</i> of a MediaService describe generally immutable
 * characteristics and capabilities of the MediaGroup.
 * Attributes may be requested in a ConfigSpec, 
 * and attributes of a MediaService/MediaGroup can be accessed using
 * {@link #getConfiguration() getConfiguration()}
 *{@link ConfigSpec#getAttributes() .getAttributes()}.
 * 
 * <h5>Configuration:</h5>
 * The <i>Configuration</i> of a MediaService is the attributes and
 * parameters of the MediaGroup plus the collection of 
 * Resources allocated to the MediaGroup including the attributes 
 * and parameters of those Resources.
 * The method {@link #getConfiguration()} returns 
 * a ConfigSpec object that represents the current configuration 
 * of the MediaService. The ConfigSpec argument to
 * {@link #configure(ConfigSpec)} 
 * or any of
 * <a href="#bindMethods">the bind methods</a>
 * determines the configuration of the MediaService.
 * <p>
 * <h4>BasicMediaService:</h4>
 * <p>
 * MediaService is an interface; an implementation and constructor are 
 * available from the class {@link BasicMediaService} which
 * implements MediaService and various Resource interfaces.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public 
interface MediaService extends MediaServiceConstants {
    
    /**
     * Return the MediaProvider used by this MediaService.
     * <p>
     * A MediaService object is associated with a MediaProvider
     * when it is constructed. 
     * See: {@link BasicMediaService#BasicMediaService(String, String)}
     * This method returns the MediaProvider instance 
     * used by this MediaService object.
     * @return the MediaProvider used by this MediaService.
     */
    public MediaProvider getMediaProvider();

    /** 
     * Bind this MediaService to a particular Terminal.
     * This MediaService is bound to a MediaGroup that is
     * configured as requested and
     * connected to the named Terminal.
     * <p>
     * It is possible to bind to a Terminal that does not have any
     * Connections. 
     * However, <a href="#disconnectProcessing">disconnect processing</a>
     * may prevent configuration of a MediaGroup unless there is
     * an active Connection to the Terminal.
     * <p>
     * Throws a MediaConfigException 
     * if the Terminal cannot be connected to a MediaGroup, or
     * if the MediaGroup cannot be configured as requested.
     * <p>
     * @param configSpec a ConfigSpec describing the requested configuration
     * @param terminalName a String that names a media capable Terminal
     * @throws MediaBindException one of 
     * AlreadyBoundException, BindInProgressException, BindCancelledException.
     * @throws MediaConfigException if a MediaGroup can not be configured as requested
     */
    public void bindToTerminalName(ConfigSpec configSpec, String terminalName)
	throws MediaBindException, MediaConfigException;
     
    /** 
     * Bind this MediaService to a particular Terminal.
     * This MediaService is bound to a MediaGroup that is
     * configured as requested and
     * connected to the given Terminal.
     * <p>
     * This is functionally equivalent to:<br>
     * {@link #bindToTerminalName bindToTerminalName}<code>(configSpec, term.getName())</code>,
     * <br>but may perform better in some implementations.
     * <p>
     * @param configSpec a ConfigSpec describing the requested configuration
     * @param terminal a media-capable Terminal object
     *
     * @throws MediaBindException one of
     * AlreadyBoundException, BindInProgressException, BindCancelledException.
     * @throws MediaConfigException if a MediaGroup can not be configured as requested.
     */
    public void bindToTerminal(ConfigSpec configSpec, Terminal terminal)
	throws MediaBindException, MediaConfigException;
     
    /**
     * Bind this MediaService to a call directed to <code>serviceName</code>.
     * Wait until this MediaService is bound to the MediaGroup of a Terminal
     * connected to a Call that is directed to the given <code>serviceName</code>.
     * <p>
     * A call is directed to this <code>serviceName</code> when:<ul>
     * <li>the characteristics of an new (inbound) call are 
     * mapped to <code>serviceName</code>.</li>
     * <li>the <code>disposition</code> String of 
     * {@link #releaseToService releaseToService}
     * maps to <code>serviceName</code></li>
     * </ul>
     * <p>
     * The mapping of disposition strings or call characteristics
     * to service names is implementation specific, and is controlled
     * by implementation specific administrative interfaces.
     * For example: <ul>
     * <li> an ACD may do inbound call routing based on
     * the dialed number or the type of call (voice, fax, or modem)</li>
     * <li> an auto-attendant application may query the
     * caller and direct the call based on user input, using 
     * <code>releaseToService</code></li>
     * <li> in a simple framework, all Calls that come to an Address may be
     * directed to a <code>serviceName</code> 
     * that matches <code>Address.toString()</code></li>
     * </ul>
     * Some frameworks may event take the initiative to assign a
     * service name for each application media service to use.
     * For those frameworks, consider using the {@link RunnableMediaService}
     * interface.
     * <p>
     * This method does not complete unless the ConfigSpec is satisfied.
     * If resources are not available when a call arrives, then the
     * underlying call discrimination or routing process may leave
     * the call unanswered until resources are available,
     * or the call may be routed to some other service.
     * <p>
     * By default, the Connection of the Call to this Terminal is brought
     * to the <code>CONNECTED</code> state before this method returns.
     * The attribute 
     * <code>{@link ConfigSpecConstants#a_LocalState a_LocalState}</code>
     * can be set in the ConfigSpec to override the default.
     * <p>
     * <b>Note:</b>
     * This function blocks, waiting for a call to be directed to
     * the given <code>serviceName</code>.
     * If {@link #cancelBindRequest()} is subsequently invoked, 
     * this method unblocks throwing a {@link BindCancelledException}.
     * <p>
     * @param configSpec a ConfigSpec describing the requested configuration
     * @param serviceName the administrative name for this MediaService
     *
     * @throws MediaBindException one of
     * AlreadyBoundException, BindInProgressException, BindCancelledException.
     * @throws MediaConfigException one of 
     * BadConfigSpecException, ResourceNotSupportedException
     *
     * @see ConfigSpecConstants#a_LocalState a_LocalState
     * @see RunnableMediaService
     */
    public void bindToServiceName(ConfigSpec configSpec, String serviceName)
	throws MediaBindException, MediaConfigException;
     
    /** 
     * Bind this MediaService to a particular Call.
     * This MediaService is bound to the MediaGroup of a Terminal
     * connected to the given Call.
     * The effect is to provide media access to an existing Call.
     * This typically creates a new Connection to the Call; which
     * Connection is to the media enabled Terminal bound to this MediaService.
     * <p>
     * The Call must be in the <code>ACTIVE</code> state (or else
     * attribute <code>a_DealloateOnIdle</code> must be <code>FALSE</code>).
     * Otherwise, <a href="#disconnectProcessing">disconnect processing</a>
     * may interfere with allocating resources.
     * <p>
     * To create Connections and bind to a <i>new</i> Call,
     * use {@link #bindAndConnect bindAndConnect}.
     * 
     * @param configSpec a ConfigSpec describing the requested configuration.
     * @param call a JTAPI Call object
     *
     * @throws MediaBindException one of
     * AlreadyBoundException, BindInProgressException, BindCancelledException.
     * @throws MediaConfigException
     *  if a MediaGroup can not be configured as requested.
     * @throws MediaCallException encapsulates any call processing exceptions
     * generated while trying to establish the new Call and Connection.
     * @return the Connection between the bound Terminal and the given Call.
     */
    public Connection bindToCall(ConfigSpec configSpec, Call call)
	throws MediaBindException, MediaConfigException, MediaCallException;
     
    /**
     * Bind this MediaService to a new (outbound) Call.
     * This MediaService is bound to the MediaGroup of a Terminal
     * connected to a new Call connected to the given Address.
     * <p>
     * This is a high-level function for making outbound calls while
     * solving the problem of going offhook and finding an inbound call.
     * (the phenomenon called <b>glare</b>).
     * <p>
     * This method (conceptually) does the following:
     * <ol>
     * <li><code>Let term =</code> an available media enabled Terminal at origAddr </li>
     * <li><code>conn = call.connect(origAddr, term, dialDigits)[0];</code></li>
     * <li>If inbound/glare then deliver that new Call as appropriate; 
     * goto 1.</li>
     * <li><code>bindToTerminal(configSpec, term)</code></li>
     * <li><code>return conn;</code></li>
     * </ol>
     * <p>
     * By default, the Connection to the remote party is brought to
     * the <code>Connection.CONNECTED</code> state before this method returns.
     * The attribute {@link ConfigSpecConstants#a_RemoteState a_RemoteState}
     * can be set in the ConfigSpec to override that default.
     * <p>
     * <b>Note:</b>
     * If <code>origAddr</code> is the empty string (<code>""</code>), 
     * then the implementation framework may choose a Terminal
     * at any available Address.
     * <p>
     * @param configSpec a ConfigSpec describing the requested configuration
     * @param origAddr a String identifying the origination Address
     * @param dialDigits a String identifying the destination Address
     * 
     * @throws MediaBindException one of 
     * AlreadyBoundException, BindInProgressException, BindCancelledException.
     * @throws MediaConfigException if a MediaGroup can not be configured as requested.
     * @throws MediaCallException if the Call or Connections cannot be established.
     *
     * @see Call#connect(Terminal, Address, String)
     */
    public void bindAndConnect(ConfigSpec configSpec, 
			       String origAddr, 
			       String dialDigits)
	throws MediaBindException, 
	       MediaConfigException, 
	       MediaCallException;

    /** 
     * Bind this MediaService to a new (outbound) Call,
     * with optargs for provider specific extensions.
     * <p>
     * @param configSpec a ConfigSpec describing the requested configuration
     * @param origAddr a String identifying the origination Address
     * @param dialDigits a String identifying the destination Address
     * 
     * @throws MediaBindException one of 
     * AlreadyBoundException, BindInProgressException, BindCancelledException.
     * @throws MediaConfigException if a MediaGroup can not be configured as requested.
     * @throws MediaCallException if the Call or Connections cannot be established.
     *
     * @see #bindAndConect(ConfigSpec, String, String)
     */
    /*
    public void bindAndConnect(ConfigSpec configSpec, 
			       String origAddr, 
			       String dialDigits
			       Dictionary optargs)
	throws MediaBindException, 
	       MediaConfigException, 
	       MediaCallException;
    */
    /* Future Note:
     * Additional attributes in the configSpec may indicate that 
     * the remote party should not be an answering machine, or fax machine.
     */
     
    /**
     * Revoke previous bind or release request from this MediaService.
     * <p>
     * This method is called from a thread that is <i>not</i> blocked 
     * in a bind method.
     * This method terminates any outstanding bind or release request 
     * on this MediaService, causing that bind request to
     * throw a BindCancelledException.
     * If the outstanding bind request coincidentally completes normally
     * then this method has no effect, and no error is signalled.
     * <p>
     * This method "succeeds" as soon as the cancel request is delivered
     * to the MediaGroup/Provider implementation. 
     * This method is conceptually "one-way"
     * and therefore safe to be invoked from within a Listener callback. 
     * The implemention of this method ensures that the threads 
     * and events do not deadlock as a result.
     * <p>
     * <b>Implementation note:</b>
     * This can be arranged because this method has no return value, 
     * does not generate any exceptions, and has no provisions for "failing" 
     * once the invocation is delivered to the implementation. 
     * Therefore, the invoking thread does not need to wait 
     * for any response from the service provider implementation. 
     * If for any reason this method "fails" to unblock
     * the outstanding request, the application may detect that
     * (perhaps using a timeout) and retry this method.
     * <p>
     * If there is no outstanding bind request for this MediaService, 
     * no action is taken and no error is signalled;
     * the assumption is that a bind just succeeded. 
     * Likewise, if the application times out and invokes a second
     * cancelBindRequest, no harm is done.
     */
    public void cancelBindRequest();
     
    /**
     * Return <code>true</code> iff this MediaService is bound to a MediaGroup.
     * @return <code>true</code> iff this MediaService is bound to a MediaGroup.
     */
    public boolean isBound();
     
    /**
     * Release the MediaGroup from this MediaService using a NormalReturn.
     * Indicates that this service has completed and has no
     * interest in the further processing of the call.
     * <p>
     * For this and other MediaService <i>release</i> methods:
     * <br>pre-conditions: <tt>(isBound() && isOwner())</tt>
     * <br>post-conditions: <tt>(!isBound())</tt>
     * <p>
     * How the call is processed after <tt>release()</tt> 
     * depends on whether some other media service has 
     * {@link Delegation <i>delegated</i>} the call.
     * If the call was delegated,
     * then ownership of the call is returned to the 
     * MediaService which delegated the call.
     * If the call was <i>not</i> delegated, then the system
     * framework gets ownership and (typically) disconnects the
     * call and releases the resources.
     * <p>
     * <b>Note:</b>To indicate that framework disconnect processing 
     * should be done even if the call is delegated, 
     * use the {@link #releaseToDestroy()} method.
     * <p>
     * <b>Note:</b>To release this MediaService, but direct the further
     * processing of the call to a specific new owner, 
     * an application may use:
     * {@link #releaseToService(String, int)}.
     * <p>
     * <b>Note:</b>To release this MediaService, but direct the further
     * processing of the call to a specific previous owner
     * (which used delegateToService()), 
     * an application may use:
     * {@link #releaseToTag(String)}.
     * <p>
     * @throws NotBoundException if not currently bound to a MediaGroup
     * @throws MediaBindException one of:
     * <tt>BindInProgressException</tt> or <tt>BindCancelledException</tt>.
     * @throws BindInProgressException
     * if a bind or release is already in progress.     
     * @throws BindCancelledException
     * if interrupted by cancelBindRequest.
     */
    public void release() throws NotBoundException, MediaBindException;
     
    /**
     * Release the MediaGroup from this MediaService with an indication
     * that the call should to be processed by some other service.
     * The next service is determined by the disposition string.
     * <p>
     * Typically, the <a href=#framework>framework</a> maps the 
     * disposition string to the serviceName of some other MediaService.
     * The framework finds another MediaService that is waiting
     * (in {@link #bindToServiceName bindToServiceName}) for that serviceName,
     * configures the MediaGroup for that MediaService, 
     * binds the MediaGroup to that MediaService,
     * and releases the MediaGroup from this MediaService.
     * <P>
     * If the given disposition string can not be mapped to a recognised
     * serviceName, NoServiceAssignedException is thrown. 
     * If the disposition code is recognised, 
     * but no service suppliers are ready,
     * this method waits for up to <I>timeout</I> milliseconds
     * for a service to become ready and then throw NoServiceReadyException.
     * <p>
     * On completion, this MediaService is unbound from the MediaGroup.
     * Further operations on this MediaService throw NotBoundException, 
     * until another MediaGroup is bound to this MediaService.
     * <p>
     * <b>Note:</b>
     * The releasing media service can store additional information
     * for the new owner by using {@link #setUserValues(Dictionary)}.
     * <p>
     * <b>Implementation note:</b>
     * The disposition strings used by an application should be defined
     * and documented as part of the external interface of the application.
     * The installation and administration processes of a framework
     * use that information to setup the mapping from disposition
     * codes to the handling services. In some cases the framework
     * may supply services to handle some disposition codes.
     * <p>
     * <b>Note:</b>
     * Disposition strings beginning with "Special:" are reserved 
     * for standardized treatment. 
     * For example:<br>
     * <table border="1" cellpadding="3">
     * <tr><td><tt>Special:Queue?address=&lt;address&gt;</tt><td> 
     * <td>queue the call at the given address</td></tr>
     * <tr><td><tt>Special:Transfer?address=&lt;address&gt;</tt><td> 
     * <td>transfer the call to the given address</td></tr>
     * </table>
     * In these cases, the framework knows that a special service 
     * exists to implement the requested disposition.
     * Additional information may be passed to the special service
     * implementation using standard URL construction conventions.
     * <p>
     * @param disposition a String that identifies the next MediaService. 
     * @param timeout milliseconds to wait for service to become ready.
     *
     * @throws NotBoundException if not currently bound to a MediaGroup
     * @throws MediaBindException one of:
     * <tt>NoServiceAssignedException</tt>, <tt>NoServiceReadyException</tt>, 
     * <tt>BindInProgressException</tt>, <tt>BindCancelledException</tt>
     * @throws NoServiceAssignedException
     * if <tt>disposition</tt> is not recognised or is not mapped to any serviceName.
     * @throws NoServiceReadyException 
     * if none of the MediaServices registered to <tt>disposition</tt> are ready
     * and none become ready within <code>timeout</code> milliseconds.
     * @throws BindInProgressException if a bind or release 
     * is already in progress.     
     * @throws BindCancelledException if interrupted by <tt>cancelBindRequest</tt>.
     * @throws MediaConfigException 
     * if the MediaGroup could not be configured for the recipient service.
     */
    public void releaseToService(String disposition, int timeout)
	throws NotBoundException, MediaBindException, MediaConfigException;

    /**
     * Release MediaGroup from this MediaService to a previous owner.
     * Return ownership to the MediaService that called
     * {@link Delegation#delegateToService delegateToService()} 
     * with <tt>catchTags</tt> that matche one of the given <tt>returnTags</tt>.
     * <p>
     * Some return tags have assigned usage:
     * <table border="1" cellpadding="3">
     * <tr><td><tt>Tag Name:</tt></td><td>Used for:</td></tr>
     * <tr><td><tt>tag_NormalReturn</tt></td><td>simple release()</td></tr>
     * <tr><td><tt>tag_DestroyRequest</tt></td><td>releaseToDestroy</td></tr>
     * <tr><td><tt>tag_RemoveSession</tt></td><td>releaseToRemove</td></tr>
     * <tr><td><tt>null</tt> or <tt>""</tt></td><td>NormalReturn</td></tr>
     * </table>
     * <p>
     * <b>Note:</b>
     * each previous owner is checked, from most recent to oldest.
     * If the previous owner <b>does not</b> catch the return tag, then
     * <tt>delegateToService</tt> completes with qualifier <tt>q_Released</tt>,
     * and that MediaService <tt>!isBound()</tt>.
     * If the previous owner <b>does</b> catch the return tag,
     * then it becomes the new owner: 
     * <tt>delegateToService</tt> completes with qualifier <tt>q_Returned</tt>
     * and that MediaService <tt>isBound()</tt> and <tt>isOwner()</tt>.
     * 
     * <p>
     * <b>Note:</b> the <tt>returnTag</tt> conceptually specifies a single tag,
     * indicating the terminatation status of this MediaService.
     * Each 'catcher' should catch all that apply, using <tt>catchTags</tt>. 
     * However, to enable interoperability with applications that may be
     * catching disjoint names, the MediaService invoking <tt>returnToTag</tt>
     * is allowed to supply a space-separated list of tags in <tt>returnTag</tt>.
     * The list of tags is consdered to be a list of synonyms,
     * each previous owner is checked to see if its list of <tt>catchTags</tt>
     * matches any one of the given <tt>returnTag</tt> tags.
     * 
     * <p>
     * @param returnTag a String of tags to match a previous <tt>catchTags</tt>
     *
     * @throws MediaBindException a <tt>BindInProgressException</tt> 
     * if a bind or release is already in process.
     *
     * @see MediaService#release()
     * @see MediaService#releaseToService(String, int)
     */
    void releaseToTag(String returnTag) throws MediaBindException;

    /**
     * Release with an indication that 
     * the call is or should be disconnected and the MediaGroup destroyed.
     * <p>
     * Equivalent to <tt>releaseToTag(tag_DestroyRequest)</tt>. 
     *
     * @throws MediaBindException a <tt>BindInProgressException</tt>
     * if a bind or release is already in process.
     * 
     * @see #releaseToTag(String)
     */
    void releaseToDestroy() throws MediaBindException;

    // Here begin the MediaGroup methods:
     
    /**
     * Configure the bound MediaGroup according to the given configSpec.
     * <p>
     * Post-Condition: 
     * <code>foreach R in configSpec: {(this instanceof R)}</code>
     * and the Resource is allocated and connected in the MediaGroup.
     * <p>
     * <b>Note:</b>
     * On some implementations this method stops all media operations.
     *
     * @param configSpec a ConfigSpec describing the requested configuration 
     * @throws NotBoundException if not currently bound to a MediaGroup
     * @throws MediaConfigException if resources can not be configured as requested
     */
    public void configure(ConfigSpec configSpec) 
	throws NotBoundException, 
	       MediaConfigException; 

    /**
     * Return a ConfigSpec that describes the current configuration of 
     * this MediaService.
     * The returned ConfigSpec contains the attributes and parameters
     * of the currently bound MediaGroup, including ResourceSpecs for all
     * of the Resources currently configured into this MediaGroup.
     * <p>
     * <b>Note:</b>.
     * This does not generally return a copy of the ConfigSpec 
     * used to create/configure this MediaGroup. 
     * The returned ConfigSpec describes the MediaService/MediaGroup 
     * as it is currently and actually configured.
     * <p>
     * The attributes of the individual resources
     * appear in the ResourceSpec corresponding to that resource.
     * The parameters and attributes of the MediaService/MediaGroup
     * as a whole are available using 
     * {@link ConfigSpec#getAttributes() ConfigSpec.getAttributes()}
     * and 
     * {@link ConfigSpec#getParameters() ConfigSpec.getParameters()}.
     *
     * @return a ConfigSpec describing the current configuration.
     * @throws NotBoundException if not currently bound to a MediaGroup
     */
    public ConfigSpec getConfiguration() throws NotBoundException;
      
    /**
     * Return the installation-specific String that identifies
     * the Terminal to which this MediaService is bound.
     * <p>
     * @return a String that uniquely identifies the bound Terminal.
     * @throws NotBoundException if not currently bound to a MediaGroup
     * @see Terminal#getName Terminal.getName()
     */
    public String getTerminalName() throws NotBoundException;
     
    /**
     * Return the Terminal associated with the MediaService.
     * The Terminal may be used to access the associated JTAPI objects.
     * <p>
     * <b>Note:</b>
     * If this MediaService is not associated with a JTAPI Provider,
     * this method may return <code>null</code>.
     * <p>
     * <b>Note:</b> 
     * This method is supplied as a potential accelerator for some 
     * implementations.  Applications that need a reference to the
     * bound Terminal without reference to other JTAPI objects should
     * restrict their usage to {@link #getTerminalName()}.
     * <p>
     *
     * @return the Terminal object associated with this MediaService.
     * @throws NotBoundException if not currently bound to a MediaGroup
     */
    public Terminal getTerminal() throws NotBoundException;
     
    /**
     * Stop media operations on this MediaService.
     * Stops all ongoing media operations, unblocking any resource methods.
     * Those operations that are stopped get a completion event with:
     * <br><code>ResourceEvent.getQualifier() == ResourceEvent.q_Stop</code>
     * <p>
     * This method may not unblock a bind or release request.
     * For that effect, use {@link #cancelBindRequest()}.
     * <p>
     * @throws NotBoundException if not currently bound to a MediaGroup
     */
    public void stop() throws NotBoundException; 
     
    /**
     * Trigger a RTC action from the application.
     * <p>
     * This is a non-blocking, one-way invocation.
     * <p>
     * This method allows the application to synthesize the triggering
     * of RTC actions. The RTC Condition associated with this action
     * is {@link ResourceConstants#rtcc_TriggerRTC rtcc_TriggerRTC}.
     * <P>
     * The <tt>rtca</tt> actions are documented by various Resources
     * in the <i>Resource</i>Constants interface. Look for Symbol fields
     * with the <tt>rtca_</tt> prefix.
     * <p>
     * <b>Note:</b> <i>this method was ill-conceived and has been recalled.
     * RTC actions internally are associated with a specific transaction.
     * Until implementations with the correct semantics are defined,
     * it is better to use the existing transactional commands.</i>
     * <p>
     * For example: use <tt>Player.pause()</tt> 
     * instead of <tt>triggerRTC(Player.rtca_Pause)</tt>.
     * <p>
     * In a future release there may be:
     * <br><tt>void triggerRTC(Symbol rtca, Async.Event event)</tt>
     * <br><i>which would effect only the transaction associated
     * with the given event.</i>
     * <p>
     * @param rtca a Symbol for a recognized RTC action: rtca_<i>Action</i>
     * @throws NotBoundException if not currently bound to a MediaGroup
     * @deprecated use transactional Resource commands.
     */
    public void triggerRTC(Symbol rtca) throws NotBoundException;
     
    /**
     * Creates and returns a new Dictionary that contains 
     * the UserDictionary values corresponding to a given set of keys.
     * <p>
     * If the keys argument is <code>null</code> then a snapshot of 
     * the entire UserDictionary is returned.
     * <p>
     * For interoperability with other languages, 
     * the keys in the Dictionary are restricted to type Symbol.
     * The result of using keys of other types is undefined,
     * but throwing a ClassCastException is considered compliant.
     * <p>
     * @param keys an array of key Symbols
     * @return a Dictionary of application-shared information.
     *
     * @throws NotBoundException if not currently bound to a MediaGroup
     * @see #setUserValues
     */
    public Dictionary getUserValues(Symbol[] keys) 
	throws NotBoundException;
     
    /**
     * Set the values of several UserDictionary keys.
     *
     * The values supplied in the given <code>dict</code>
     * are merged with the current UserDictionary.
     *
     * @param dict a Dictionary whose contents is merged into the UserDictionary.
     * @throws NotBoundException if not currently bound to a MediaGroup
     * @see #getUserValues
     */
    public void setUserValues(Dictionary dict) 
	throws NotBoundException;
     
    /**
     * Set the entire UserDictionary to a new collection of key-value pairs.
     * <p>
     * <b>Note:</b>
     * <code>setUserDictionary(null)</code> achieves the same result 
     * as <code>setUserDictionary(new Hashtable())</code>;
     * <p>
     * @param newDict A Dictionary whose contents is copied into the MediaGroup.
     * @throws NotBoundException if not currently bound to a MediaGroup
     */
    public void setUserDictionary(Dictionary newDict) 
	throws NotBoundException;
     
    /**
     * Get the value of various parameters from 
     * the MediaGroup bound to this MediaService. 
     * Each key in the returned Dictionary has the value of the 
     * corresponding parameter.  
     * <p>
     * If the params is <code>null</code>, then get <b>all</b> parameters.
     * For example:<pre>	Dictionary parms;
     *	parms = getParameters(null); // on return, parms contains all parameters.</pre>
     * Some implementations may not be able to retrieve parameter values
     * when one or more resources are busy. 
     * In that case, <code>null</code> is returned. 
     * <p>
     * If a key refers to a Resource that is not present,
     * or the resource has no meaning associated with a particular Symbol,
     * the key is ignored, no error is generated, 
     * and that key does not appear returned Dictionary.
     * <p>
     * @param params an array of Symbols naming parameters to be retrieved.
     * @return a Dictionary of parameter Symbols and their values.
     *
     * @throws NotBoundException if not currently bound to a MediaGroup
     * @see #setParameters
     */
    public Dictionary getParameters(Symbol[] params) 
	throws NotBoundException; 
    /*
     * <br><i><b>Note:</b>
     * S.100 says: if an invalid-key is found, return an error event
     * and the invalid key is available using <code>getInvalidKey()</code>.
     * However, for Java, any invalid key is not present in the Dictionary,
     * so that </i><code>parms.get(SomeInvalidKey)</code><i>
     * returns </i><code>null</code><i>.
     * </i>
     * <br><i>A vendor's implementation may define a p_Debug parameter,
     * which if set to TRUE would change this behavior; 
     * returning an error status, and a list of offending keys.</i>
     */
     
    /**
     * Set the value of various parameters 
     * for the MediaGroup bound to this MediaService.
     * For each key in the supplied Dictionary,
     * the corresponding parameter of the MediaGroup
     * is set to the value of that key in the Dictionary. 
     * <p>
     * For interoperability with other languages,
     * the keys in the Dictionary are restricted to type {@link Symbol}.
     * The result of using keys of other types is undefined,
     * but a compliant implementation may throw
     * a ClassCastException or IllegalArgumentException.
     * <p>
     * If a parameter Symbol refers to a resource that is not present,
     * or the MediaGroup or Resource has no meaning associated with
     * a particular Symbol, 
     * the setting for that parameter is quietly ignored.
     * <p>
     * @param params a Dictionary in which the keys are Symbols
     * and the values are assigned to the parameter with that Symbol name.
     *
     * @throws NotBoundException if not currently bound to a MediaGroup
     * @see #getParameters
     */
    public void setParameters(Dictionary params) 
	throws NotBoundException; 

    /*
     * <b>Note:</B>
     * S.300 should be enhanced to make setting non-existant parameters easier.
     * The current S.300 spec always fails and returns only one failed symbol.
     * An implementation must catch that failure, remove
     * the failed symbol and try them all again.
     * Or conversly, try setting only one parameter at a time.
     * <p>
     * One can imagine a debugging aid where
     * the result is determined the value of
     * <code>MediaService.p_IfBadParameters</code>. 
     * one of the following treatments if any parameter failed:
     * <table border="1" cellpadding="3">
     * <tr><td>Value:</td><td>Result:</td></tr>
     * <tr><td align=top>v_Fail</td>
     * <td>the operation fails, no parameter values are changed</td></tr>
     * <tr><td align=top>v_Warn</td>
     * <td>return an array of offending Symbols</td></tr> 
     * <tr><td align=top>v_Ignore</td>
     * <tr><td>silently accept the missing parameters</td></tr></table>
     */
     
    /**
     * Add a MediaListener to this MediaService.
     * <p>
     * Events generated by this MediaService are dispatched to the
     * given listener if it implements the appropriate Listener interface.
     *
     * @param listener an object that implements 
     * one or more of the ResourceListener interfaces and/or 
     * the MediaServiceListener interface.
     * @deprecated As of JTAPI 1.4, replaced by {@link #addListener(EventListener)}
     * @see #removeMediaListener
     */ 
    public void addMediaListener(MediaListener listener);
     
    /**
     * Remove the MediaListener from this MediaService.
     * @param listener an object that implements MediaListener
     * @deprecated As of JTAPI 1.4, replaced by {@link #removeListener(EventListener)}
     * @see #addMediaListener
     */ 
    public void removeMediaListener(MediaListener listener);
    
    /** 
     * Add a MediaListener to this MediaService.
     * <p>
     * Events generated by this MediaService are dispatched to the
     * given listener if it implements the appropriate Listener interface.
     * <p>
     * <b>Note:</b>
     * This more general form supercedes <tt>addMediaListener(MediaListener)</tt>.
     * The EventListener is added to this MediaService,
     * and events are delivered for any MediaListener interface
     * implemented by the listener.
     * @param listener a MediaListener for this MediaService
     * @see #removeListener
     */
    void addListener(EventListener listener);
    
    // non-transactional: local
    /**
     * Remove the MediaListener from this MediaService.
     * <p>
     * <b>Note:</b>
     * This more general form supercedes <tt>removeMediaListener(MediaListener)</tt>.
     * @param listener an object that implements MediaListener
     * @see #addListener
     */
    void removeListener(EventListener listener);
}

