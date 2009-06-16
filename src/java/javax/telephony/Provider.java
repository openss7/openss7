/*
 * Copyright (c) 1996 Sun Microsystems, Inc. All Rights Reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for NON-COMMERCIAL purposes and without
 * fee is hereby granted provided that this copyright notice
 * appears in all copies. Please refer to the file "copyright.html"
 * for further important copyright and licensing information.
 *
 * SUN MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF
 * THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, OR NON-INFRINGEMENT. SUN SHALL NOT BE LIABLE FOR
 * ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR
 * DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.
 */

package javax.telephony;
import  javax.telephony.capabilities.*;

/**
 * A <CODE>Provider</CODE> represents the telephony software-entity that interfaces with a 
 * telephony subsystem.
 *
 * <H4>Introduction</H4>
 *
 * The telephony subsystem could be a PBX connected to 
 * a server machine, a telephony/fax card in a desktop machine or a networking
 * technology such as IP or ATM.
 *
 * <H4>Provider States</H4>
 * 
 * The Provider may either be in one of the following states:
 * <CODE>Provider.IN_SERVICE</CODE>, <CODE>Provider.OUT_OF_SERVICE</CODE>, or
 * <CODE>Provider.SHUTDOWN</CODE>. The Provider state represents whether any
 * action on that Provider may be valid. The following tables describes each
 * state:
 * <p>
 * <TABLE>
 * <TR>
 * <TD WIDTH="20%">
 * <CODE>Provider.IN_SERVICE</CODE>
 * </TD>
 * <TD WIDTH="80%">
 * This state indicates that the Provider is currently alive and available for
 * use.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%">
 * <CODE>Provider.OUT_OF_SERVICE</CODE>
 * </TD>
 * <TD WIDTH="80%">
 * This state indicates that a Provider is temporarily not available for use.
 * Many methods in the Java Telephony API are invalid when the Provider is in
 * this state. Providers may come back in service at any time, however, the
 * application can take no direct action to cause this change.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%">
 * <CODE>Provider.SHUTDOWN</EM></CODE>:
 * </TD>
 * <TD WIDTH="80%">
 * This state indicates that a Provider is permanently no longer available for
 * use. Most methods in the Java Telephony API are invalid when the Provider
 * is in this state. Applications may use the <CODE>Provider.shutdown()</CODE>
 * method on this interface to cause a Provider to move into the
 * <CODE>Provider.SHUTDOWN</CODE> state.
 * </TD>
 * </TR>
 * </TABLE>
 * <p>
 * The following diagram shows the allowable state transitions for the Provider
 * as defined by the core package.
 * <p>
 * <IMG SRC="doc-files/providerstates.gif" ALIGN="center">
 * <p>
 * <H4>Obtaining a Provider</H4>
 *
 * A Provider is created and returned by the
 * <CODE>JtapiPeer.getProvider()</CODE> method which is given a string to
 * describe the desired Provider. This method sets up any needed communication
 * paths between the application and the Provider. The string given is one of
 * the services listed in the <CODE>JtapiPeer.getServices()</CODE>. JtapiPeers
 * particular implementation on a system and may be obtained via the
 * <CODE>JtapiPeerFactory</CODE> class.
 * 
 * <H4>Listeners and Events</H4>
 *
 * Each time a state changes occurs on a Provider, the application is notified
 * via an <EM>event</EM>. This event is reported via the
 * <CODE>ProviderListener</CODE> interface. Applications instantiate objects
 * which implement this interface and use the
 * <CODE>Provider.addProviderListener()</CODE> method to begin the delivery of events.
 * All Provider events reported via this interface extend the
 * <CODE>ProviderEvent</CODE> interface.
 * Therefore, applications may then query the event object
 * returned for the specific state change, via the <CODE>Event.getID()</CODE> method.
 * In the core package API,
 * when the Provider changes state,
 * a <CODE>ProviderEvent</CODE>
 * is sent to the ProviderListener, having one of the following event ids:
 * <CODE>PROVIDER_IN_SERVICE</CODE>, <CODE>PROVIDER_OUT_OF_SERVICE</CODE>,
 * and <CODE>PROVIDER_SHUTDOWN</CODE>.
 * A <CODE>ProviderEvent</CODE> with event id 
 * The <CODE>PROVIDER_EVENT_TRANSMISSION_ENDED</CODE> is delivered to all ProviderListeners
 * when the Provider
 * becomes unobservable and is the final event delivered to the listener.
 *
 * <H4>Call Objects and Providers</H4>
 *
 * The Provider maintains knowledge of the calls currently associated with it.
 * Applications may obtain an array of these Calls via the
 * <CODE>Provider.getCalls()</CODE> method. A Provider may have Calls
 * associated with it which were created before it came into existence. It is
 * the responsibility of the implementation of the Provider to model and
 * report all existing telephone calls which were created prior to the
 * Provider's lifetime. The Provider maintains references to all calls until
 * they move into the <CODE>Call.INVALID</CODE> state.
 * <p>
 * Applications may create new Calls using the
 * <CODE>Provider.createCall()</CODE> method. A new Call is returned in the
 * <CODE>Call.IDLE</CODE> state. Applications may then use this idle Call to
 * place new telephone calls. Once created, this new Call object is returned
 * via the <CODE>Provider.getCalls()</CODE> method.
 *
 * <H4>The Provider's domain</H4>
 *
 * The term <EM>Provider's domain</EM> refers to the collection of Address
 * and Terminal objects which are local to the Provider, and typically, can be
 * controlled by the Provider. For example, the domain of a Provider of a
 * desktop system with an ISDN card are the Address(es) and Terminal(s) which
 * represent that ISDN endpoint. The domain of a Provider for a PBX may be
 * the Addresses and Terminals in that PBX. The Provider implementation
 * controls access to Addresses and Terminals by limiting the domain it
 * presents to the application.
 *
 * <H4>Address and Terminal Objects</H4>
 *
 * An Address object represents what we commonly think of as a "telephone
 * number." In more rare implementations where the underlying network is not
 * a telephony network, this address may represent something else, such as
 * an IP address. Regardless, it represents a <EM>logical</EM> endpoint of
 * a telephone call. A Terminal represents a physical hardware endpoint
 * connected to the telephone network. An example of a Terminal is a telephone
 * set, but a Terminal does not have to take the form of this limited and
 * traditional example. Addresses and Terminals are in a many-to-many
 * relationship. An Address may contain multiple Terminals, and Terminals
 * may contain multiple Addresses. See the specifications for the Address
 * and Terminal objects for more information.
 * <p>
 * Unlike Call objects, applications may not create Terminal or Address
 * objects. The Provider begins with knowledge of certain Terminal and Address
 * objects defined as its local domain. This list is static once the Provider
 * is created. The Addresses and Terminals in the Provider's domain are
 * reported via the <CODE>Provider.getAddresses()</CODE> and
 * <CODE>Provider.getTerminals()</CODE> methods, respectively.
 * <p>
 * Other Addresses and Terminals may be created sometime during the operation
 * of the Provider when the Provider learns of new instances of these objects.
 * These new objects, however, represent Addresses and Terminals outside the
 * Provider's domain. For example, if the Provider's domain is a PBX, the
 * Provider will know about all Addresses and Terminals in this PBX when the
 * Provider first starts. Any Addresses and Terminals it subsequently learns
 * about are outside this PBX. These Address and Terminal objects outside this
 * PBX are not reported via the <CODE>Provider.getTerminals()</CODE> and
 * <CODE>Provider.getAddresses()</CODE> methods. Address and Terminal objects
 * outside of the Provider's domain are referred to as <EM>remote</EM>.
 *
 * <H4>Capabilities: Static and Dynamic</H4>
 *
 * The Provider interface supports methods to return <EM>static</EM>
 * capabilities for each of the Java Telephony call model objects. Static
 * capabilities provide applications with information concerning the ability
 * of the implementation to perform certain methods. These static
 * capabilities indicate whether a method is implemented for a particular type
 * of object and does not depend upon the particular instance of the object
 * nor the current state of the call model. Those methods for which the static
 * capability returns false will throw a MethodNotSupportedException when
 * invoked. The static capability methods supported on this interface are:
 * <CODE>Provider.getProviderCapabilities()</CODE>,
 * <CODE>Provider.getCallCapabilities()</CODE>,
 * <CODE>Provider.getAddressCapabilities()</CODE>,
 * <CODE>Provider.getTerminalCapabilities()</CODE>,
 * <CODE>Provider.getConnectionCapabilities()</CODE>, and
 * <CODE>Provider.getTerminalConnectionCapabilities()</CODE>.
 * <p>
 * Dynamic capabilities tell the application which actions are possible at the
 * time this method is invoked based upon the implementations knowledge of its
 * ability to successfully perform the action. This determination may be based
 * upon arguments passed to this method, the current state of the call model,
 * or some implementation-specific knowledge. These indications do not
 * guarantee that a particular method can be successfully invoked, however.
 * This interface returns the dynamic capabilities for a Provider object
 * via the <CODE>Provider.getCapabilities()</CODE> method. Note that this
 * method is distinct from the static capability method
 * <CODE>Provider.getProviderCapabilities()</CODE>.
 *
 * <H4>Multiple Providers and Multiple Applications</H4>
 *
 * It is not guaranteed or expected that objects (Call, Terminal, etc.)
 * instantiated through one Provider will be usable with another Provider.
 * Therefore, an application that uses two providers must keep all the objects
 * relating to these providers separate. In the future, there may be a
 * mechanism whereby a Provider may share objects with another Provider if they
 * are speaking to the same telephony hardware, however, such capabilities are
 * not available in this release.
 * <p>
 * Also, multiple applications may request and communicate with the same
 * Provider implementation. Typically, since each application executes in its
 * own object space, each will have its own instance of the Provider object.
 * These two different Provider objects may, in fact, be proxies for a
 * centralized Provider instance. Certain methods in JTAPI are specified to
 * affect only the invoking applications and have no affect on others. The
 * only example in the core package is the <CODE>Provider.shutdown()</CODE>
 * method.
 * <p>
 * @see javax.telephony.JtapiPeer
 * @see javax.telephony.JtapiPeerFactory
 * @see javax.telephony.ProviderListener
 * @version 04/05/99 1.53
 */

public interface Provider {

  /**
   * This state indicates that a Provider is currently available for use.
   */
  public static final int IN_SERVICE = 0x10;


  /** 
   * This state indicates that a Provider is currently not available for use. 
   * Many methods in the Java Telephony
   * API are invalid when the Provider is in this state. Providers may come
   * back in service at any time, however, the application can take no direct
   * action to cause this change.
   */
  public static final int OUT_OF_SERVICE = 0x11;


  /**
   * This state indicates that a Provider is
   * permanently no longer available for use. Most methods in the Java
   * Telephony API are invalid when the Provider is in this state.
   */
  public static final int SHUTDOWN = 0x12; 


  /**
   * Returns the current state of this Provider. The value is one of
   * <CODE>Provider.IN_SERVICE</CODE>, <CODE>Provider.OUT_OF_SERVICE</CODE>,
   * or <CODE>Provider.SHUTDOWN</CODE>.
   * <p>
   * @return The current state of this Provider.
   */
  public int getState();


  /**
   * Returns the unique string name of this Provider. Each different Provider
   * must have a unique string associated with it. This is the same string
   * which the application passed to the <CODE>JtapiPeer.getProvider()</CODE>
   * method to create this Provider instance.
   * <p>
   * @see javax.telephony.JtapiPeer
   * @return The unique String name of this Provider.
   */
  public String getName();
 

  /**
   * Returns an array of Call objects currently associated with the Provider.
   * When a Call moves into the <CODE>Call.INVALID</CODE> state, the Provider
   * loses its reference to this Call. Therefore, all Calls returned by this
   * method must either be in the <CODE>Call.IDLE</CODE> or
   * <CODE>Call.ACTIVE</CODE> state. This method returns null if the Provider
   * has zero calls associated with it.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let Calls calls[] = Provider.getCalls()
   * <LI>calls == null or calls.length >= 1
   * <LI>For all i, calls[i].getState() == Call.IDLE or Call.ACTIVE
   * </OL>
   * @return An array of Call objects currently associated with this Provider.
   * @exception ResourceUnavailableException Indicates the number of calls
   * present in the Provider is too great to return as a static array.
   */
  public Call[] getCalls()
    throws ResourceUnavailableException;


  /**
   * Returns an Address object which corresponds to the telephone number string
   * provided. If the provided name does not correspond to an Address known by
   * the Provider and within the Provider's domain, InvalidArgumentException is
   * thrown. In other words, the Address must appear in the list generated by
   * <CODE>Provider.getAddresses()</CODE>.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>Let Address address = this.getAddress(number);
   * <LI>address is an element of this.getAddresses();
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let Address address = this.getAddress(number);
   * <LI>address is an element of this.getAddresses();
   * </OL>
   * @param number The telephone address string.
   * @return The Address object corresponding to the given telephone
   * number.
   * @exception InvalidArgumentException The name, e.g. telephone number 
   * of the Address does not
   * correspond to the name of any Address object known to the Provider or
   * within the Provider's domain.
   */
  public Address getAddress(String number)
    throws InvalidArgumentException;


  /**
   * Returns an array of Addresses associated with the Provider and within the
   * Provider's domain. This list is static (i.e. is does not change) after
   * the Provider is first created. If no Address objects are associated with
   * this Provider, then this method returns null.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let Address[] addresses = this.getAddresses()
   * <LI>addresses == null or addresses.length >= 1
   * </OL>
   * @return An array of Addresses known by this provider.
   * @exception ResourceUnavailableException Indicates the number of addresses
   * present in the Provider is too great to return as a static array.
   */
  public Address[] getAddresses()
    throws ResourceUnavailableException;


  /**
   * Returns an array of Terminals associated with the Provider and within the
   * Provider's domain.  Each Terminal possesses a unique name, which is
   * assigned to it by the JTAPI implementation. If there are no Terminals
   * associated with this Provider, then this method returns null.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let Terminal[] terminals = this.getTerminals()
   * <LI>terminals == null or terminals.length >= 1
   * </OL>
   * @return An array of Terminals in the Provider's local domain.
   * @exception ResourceUnavailableException Indicates the number of terminals
   * present in the Provider is too great to return as a static array.
   */
  public Terminal[] getTerminals()
    throws ResourceUnavailableException;


  /**
   * Returns an instance of the Terminal class which corresponds to the
   * given name. Each Terminal has a unique name associated with it, which
   * is assigned to it by the JTAPI implementation.  If no Terminal is
   * available for the given name within the Provider's domain, this method
   * throws the InvalidArgumentException. This Terminal must be in the array
   * generated by <CODE>Provider.getTerminals().</CODE>
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>Let Terminal terminal = this.getTerminal(name);
   * <LI>terminals is an element of this.getTerminals();
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let Terminal terminal = this.getTerminal(name);
   * <LI>terminal is an element of this.getTerminals();
   * </OL>
   * @param name The name of desired Terminal object.
   * @return The Terminal object associated with the given name.
   * @exception InvalidArgumentException The name provided does not correspond
   * to a name of any Terminal known to the Provider or within the Provider's
   * domain.
   */
  public Terminal getTerminal(String name)
    throws InvalidArgumentException;


  /**
   * Instructs the Provider to shut itself down and perform all necessary
   * cleanup. Applications invoke this method when they no longer intend to
   * use the Provider, most often right before they exit. This
   * method is intended to allow the Provider to perform any necessary
   * cleanup which would not be taken care of when the Java objects are
   * garbage collected. This method causes the Provider to move into the
   * <CODE>Provider.SHUTDOWN</CODE> state, in which it will stay indefinitely.
   * <p>
   * If the Provider is already in the <CODE>Provider.SHUTDOWN</CODE> state,
   * this method does nothing. The invocation of this method should not affect
   * other applications which are using the same implementation of the Provider
   * object.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>this.getState() == Provider.SHUTDOWN
   * </OL>
   */
  public void shutdown();


  /**
   * Creates and returns a new instance of the Call object. This new call
   * object is in the <CODE>Call.IDLE state</CODE> and has no Connections. An
   * exception is generated if a new call cannot be created for various
   * reasons. This Provider must be in the <CODE>Provider.IN_SERVICE</CODE>
   * state, otherwise an InvalidStateException is thrown.
   * <p>
   * <B>Pre-conditions:</B>
   * <OL>
   * <LI>this.getState() == Provider.IN_SERVICE
   * </OL>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>this.getState() == Provider.IN_SERVICE
   * <LI>Let Call call = this.createCall();
   * <LI>call.getState() == Call.IDLE.
   * <LI>call.getConnections() == null
   * <LI>call is an element of this.getCalls()
   * </OL>
   * @return The new Call object.
   * @exception ResourceUnavailableException An internal resource necessary
   * to create a new Call object is unavailable.
   * @exception InvalidStateException The Provider is not in the
   * <CODE>Provider.IN_SERVICE</CODE> state.
   * @exception PrivilegeViolationException The application does not have
   * the proper authority to create a new telephone call object.
   * @exception MethodNotSupportedException The implementation does not support
   * creating new Call objects.
   */
  public Call createCall()
    throws ResourceUnavailableException, InvalidStateException,
      PrivilegeViolationException, MethodNotSupportedException;

  
  /**
   * Adds an observer to the Provider. Provider-related events are reported
   * via the <CODE>ProviderObserver</CODE> interface. The Provider
   * object will report events to this interface for the lifetime of the
   * Provider object or until the observer is removed with the
   * <CODE>Provider.removeObserver()</CODE> method or until the Provider
   * is no longer observable.
   * <p>
   * If the Provider becomes unobservable, a ProvObservationEndedEv is
   * delivered to the application as is final event. No further events are
   * delivered to the observer unless it is explicitly re-added by the
   * application. When an observer receives a ProvObservationEndedEv it is
   * no longer reported via the <CODE>Provider.getObservers()</CODE> method.
   * <p>
   * This method is valid anytime and has no pre-conditions. Application must
   * have the ability to add observers to Providers so they can monitor
   * the changes in state in the Provider.
   * <p>
   * If an application attempts to add an instance of an observer already
   * present on this Provider, then repeated attempts to add the instance
   * of the observer will silently fail, i.e. multiple instances of an
   * observer are not added and no exception will be thrown.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>observer is an element of this.getObservers()
   * </OL>
   * @param observer The observer being added.
   * @exception MethodNotSupportedException The observer cannot be added at this time.
   * @exception ResourceUnavailableException The resource limit for the
   * numbers of observers has been exceeded.
   */ 
  public void addObserver(ProviderObserver observer)
    throws ResourceUnavailableException, MethodNotSupportedException;


  /**
   * Returns a list of all ProviderObservers associated with this Provider
   * object. If no observers exist on this Provider, then this method returns
   * null.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let ProviderObserver[] observers = this.getObservers()
   * <LI>observers == null or observers.length >= 1
   * </OL>
   * @return An array of ProviderObserver objects associated with this
   * Provider.
   */
  public ProviderObserver[] getObservers();


  /**
   * Removes the given observer from the Provider. The given observer will no
   * longer receive events generated by this Provider object. The final event
   * will be the ProvObservationEndedEv event and will no longer be listed by
   * the <CODE>Provider.getObservers()</CODE> method.
   * <p>
   * Also, if an observer is not part of the Provider, then this method
   * fails silently, i.e. no observer is removed an no exception is thrown.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>observer is not an element of this.getObservers()
   * <LI>ProvObservationEndedEv is delivered to observer
   * </OL>
   * @param observer The observer which is being removed.
   */ 
  public void removeObserver(ProviderObserver observer);


  /**
   * Returns the static capabilities of the Provider object. The value of
   * these capabilities will not change over the lifetime of the Provider.
   * They represent the static abilities of the implementation to perform
   * certain methods on the Provider object. For all capability methods which
   * return false, the invocation of that method will always throw
   * MethodNotSupportedException.
   * <p>
   * NOTE: This method is different from the
   * <CODE>Provider.getCapabilities()</CODE>, method which returns the dynamic
   * capabilities of a Provider object instance.
   * <p>
   * @return The static capabilities of the Provider object.
   */
  public ProviderCapabilities getProviderCapabilities();


  /**
   * Returns the static capabilities of the Call object. The value of these
   * capabilities will not change over the lifetime of the Provider. They
   * represent the static abilities of the implementation to perform certain
   * methods on the Call object. For all capability methods which return false,
   * the invocation of that method will always throw
   * MethodNotSupportedException.
   * <p>
   * @return The static capabilities of the Call object.
   */
  public CallCapabilities getCallCapabilities();


  /**
   * Returns the static capabilities of the Address object. The value of these
   * capabilities will not change over the lifetime of the Provider. They
   * represent the static abilities of the implementation to perform certain 
   * methods on the Address object. For all capability methods which return
   * false, the invocation of that method will always throw
   * MethodNotSupportedException.
   * <p>
   * @return The static capabilities of the Address object.
   */
  public AddressCapabilities getAddressCapabilities();


  /**
   * Returns the static capabilities of the Terminal object. The value of these
   * capabilities will not change over the lifetime of the Provider. They
   * represent the static abilities of the implementation to perform certain 
   * methods on the Terminal object. For all capability methods which return
   * false, the invocation of that method will always throw
   * MethodNotSupportedException.
   * <p>
   * @return The static capabilities of the Address object.
   */
  public TerminalCapabilities getTerminalCapabilities();


  /**
   * Returns the static capabilities of the Connection object. The value of
   * these capabilities will not change over the lifetime of the Provider.
   * They represent the static abilities of the implementation to perform
   * certain methods on the Connection object. For all capability methods
   * which return false, the invocation of that method will always throw
   * MethodNotSupportedException.
   * <p>
   * @return The static capabilities of the Connection object.
   */
  public ConnectionCapabilities getConnectionCapabilities();


  /**
   * Returns the static capabilities of the TerminalConnection object. The
   * value of these capabilities will not change over the lifetime of the
   * Provider. They represent the static abilities of the implementation to
   * perform certain methods on the TerminalConnection object. For all
   * capability methods which return false, the invocation of that method will
   * always throw MethodNotSupportedException.
   * <p>
   * @return The static capabilities of the TerminalConnection object.
   */
  public TerminalConnectionCapabilities getTerminalConnectionCapabilities();


  /**
   * Returns the dynamic capabilities for the instance of the Provider object.
   * Dynamic capabilities tell the application which actions are possible at
   * the time this method is invoked based upon the implementations knowledge
   * of its ability to successfully perform the action. This determination may
   * be based upon argument passed to this method, the current state of the
   * call model, or some implementation-specific knowledge. These indications
   * do not guarantee that a particular method can be successfully invoked,
   * however.
   * <p>
   * There are no arguments passed into this method for dynamic Provider
   * capabilities
   * <p>
   * NOTE: This method is different from the
   * <CODE>Provider.getProviderCapabilities()</CODE> method which returns the
   * static capabilities for the Provider object.
   * <p>
   * @return The dynamic Provider capabilities.
   */
  public ProviderCapabilities getCapabilities();


  /**
   * Returns the ProviderCapabilities object with respect to a Terminal.
   * If null is passed as a Terminal parameter, the general/provider-wide 
   * Provider capabilities are returned.
   * <p>
   * <STRONG>Note:</STRONG> This method has been replaced in JTAPI v1.2. The
   * <CODE>Provider.getProviderCapabilities()</CODE> method returns the static
   * Provider capabilities. This method now should simply invoke the
   * <CODE>Provider.getProviderCapabilities(void)</CODE> method.
   * <p>
   * @deprecated Since JTAPI v1.2. This method has been replaced by the
   * Provider.getProviderCapabilities() method.
   * @param terminal This parameter is ignored in JTAPI v1.2 and later.
   * @exception InvalidArgumentException This exception is never thrown
   * in JTAPI v1.2 and later.
   * @exception PlatformException A platform-specific exception occurred.
   * @return The dynamic ProviderCapabilities capabilities.
   */
  public ProviderCapabilities getProviderCapabilities(Terminal terminal)
    throws InvalidArgumentException, PlatformException;


  /**
   * Gets the CallCapabilities object with respect to a Terminal and an
   * Address. If null is passed as a Terminal/Address parameter, the
   * general/provider-wide Call capabilities are returned.
   * <p>
   * <STRONG>Note:</STRONG> This method has been replaced in JTAPI v1.2. The
   * <CODE>Provider.getCallCapabilities()</CODE> method returns the static
   * Call capabilities. This method now should simply invoke the
   * <CODE>Provider.getCallCapabilities(void)</CODE> method.
   * <p>
   * @deprecated Since JTAPI v1.2. This method has been replaced by the
   * Provider.getCallCapabilities() method.
   * @param terminal This argument is ignored in JTAPI v1.2 and later.
   * @param address This argument is ignored in JTAPI v1.2 and later.
   * @exception InvalidArgumentException This exception is never thrown in
   * JTAPI v1.2 and later.
   * @exception PlatformException A platform-specific exception occurred.
   * @return The static CallCapabilities cpabilities.
   */
  public CallCapabilities getCallCapabilities(Terminal terminal,
					      Address address)
    throws InvalidArgumentException, PlatformException;


  /**
   * Gets the ConnectionCapabilities object with respect to a Terminal and an
   * Address. If null is passed as a Terminal/Address parameter, the general/
   * provider-wide Connection capabilities are returned.
   * <p>
   * <STRONG>Note:</STRONG> This method has been replaced in JTAPI v1.2. The
   * <CODE>Provider.getConnectionCapabilities()</CODE> method returns the
   * static Connection capabilities. This method now should simply invoke the
   * <CODE>Provider.getConnectionCapabilities(void)</CODE> method.
   * <p>
   * @deprecated Since JTAPI v1.2. This method has been replaced by the
   * Provider.getConnectionCapabilities() method.
   * @param terminal This argument is ignored in JTAPI v1.2 and later.
   * @param address This argument is ignored in JTAPI v1.2 and later.
   * @exception InvalidArgumentException This exception is never thrown in
   * JTAPI v1.2 and later.
   * @exception PlatformException A platform-specific exception occurred.
   * @return The static ConnectionCapabilities capabilities
   */
  public ConnectionCapabilities getConnectionCapabilities(Terminal terminal,
							  Address address)
    throws InvalidArgumentException, PlatformException;


  /**
   * Gets the AddressCapabilities object with respect to a Terminal. If null
   * is passed as a Terminal parameter, the general/provider-wide Address
   * capabilities are returned.
   * <p>
   * <STRONG>Note:</STRONG> This method has been replaced in JTAPI v1.2. The
   * <CODE>Provider.getAddressCapabilities()</CODE> method returns the static
   * Address capabilities. This method now should simply invoke the
   * <CODE>Provider.getAddressCapabilities(void)</CODE> method.
   * <p>
   * @deprecated Since JTAPI v1.2. This method has been replaced by the
   * Provider.getAddressCapabilities() method.
   * @param terminal This argument is ignored in JTAPI v1.2 and later.
   * @exception InvalidArgumentException This exception is never thrown in
   * JTAPI v1.2 and later.
   * @exception PlatformException A platform-specific exception occurred.
   */
  public AddressCapabilities getAddressCapabilities(Terminal terminal)
    throws InvalidArgumentException, PlatformException;


  /**
   * Gets the TerminalConnectionCapabilities of a Terminal. If null is passed
   * as a Terminal parameter, the general/provider-wide TerminalConnection
   * capabilities are returned.
   * <p>
   * <STRONG>Note:</STRONG> This method has been replaced in JTAPI v1.2. The
   * <CODE>Provider.getTerminalConnectionCapabilities()</CODE> method returns
   * the static TerminalConnection capabilities. This method now should simply
   * invoke the <CODE>Provider.getTerminalConnectionCapabilities(void)</CODE>
   * method.
   * <p>
   * @deprecated Since JTAPI v1.2. This method has been replaced by the
   * Provider.getTerminalConnectionCapabilities() method.
   * @param terminal This argument is ignored in JTAPI v1.2 and later.
   * are being queried
   * @exception InvalidArgumentException This exception is never thrown in
   * JTAPI v1.2 and later.
   * @exception PlatformException A platform-specific exception occurred.
   */
  public TerminalConnectionCapabilities
    getTerminalConnectionCapabilities(Terminal terminal)
      throws InvalidArgumentException, PlatformException;


  /**
   * Gets the TerminalCapabilities object with respect to a Terminal. If null
   * is passed as a Terminal parameter, the general/provider-wide Terminal
   * capabilities are returned.
   * <p>
   * <STRONG>Note:</STRONG> This method has been replaced in JTAPI v1.2. The
   * <CODE>Provider.getTerminalCapabilities()</CODE> method returns the static
   * Terminal capabilities. This method now should simply invoke the
   * <CODE>Provider.getTerminalCapabilities(void)</CODE> method.
   * <p>
   * @deprecated Since JTAPI v1.2. This method has been replaced by the
   * Provider.getTerminalCapabilities() method.
   * @param terminal This argument is ignored in JTAPI v1.2 and later.
   * @exception InvalidArgumentException This exception is never thrown in
   * JTAPI v1.2 and later.
   * @exception PlatformException A platform-specific exception occurred.
   */
  public TerminalCapabilities getTerminalCapabilities(Terminal terminal)
    throws InvalidArgumentException, PlatformException;

 
  /**
   * Adds an listener to the Provider. Provider-related events are reported
   * via the <CODE>ProviderListener</CODE> interface. The Provider
   * object will report events to this interface for the lifetime of the
   * Provider object or until the listener is removed with the
   * <CODE>Provider.removeProviderListener()</CODE> method or until the Provider
   * is no longer observable.
   * <p>
   * If the Provider becomes unobservable, a <CODE>ProviderEvent</CODE> with
   * id <CODE>PROVIDER_EVENT_TRANSMISSION_ENDED</CODE>
   * is delivered to the application as is final event. No further events are
   * delivered to the listener unless it is explicitly re-added by the
   * application. When an listener receives <CODE>PROVIDER_EVENT_TRANSMISSION_ENDED</CODE> 
   * it is
   * no longer reported via the <CODE>Provider.getProviderListeners()</CODE> method.
   * <p>
   * This method is valid anytime and has no pre-conditions. Application must
   * have the ability to add listeners to Providers so they can monitor
   * the changes in state in the Provider.
   * <p>
   * If an application attempts to add an instance of an listener already
   * present on this Provider, then repeated attempts to add the instance
   * of the listener will silently fail, i.e. multiple instances of an
   * listener are not added and no exception will be thrown.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>listener is an element of this.getProviderListeners()
   * </OL>
   * @param listener The listener being added.
   * @exception MethodNotSupportedException The listener cannot be added at this time.
   * @exception ResourceUnavailableException The resource limit for the
   * numbers of listeners has been exceeded.
   */ 
  public void addProviderListener(ProviderListener listener)
    throws ResourceUnavailableException,MethodNotSupportedException;


  /**
   * Returns a list of all <CODE>ProviderListeners</CODE> associated with this Provider
   * object. If no listeners exist on this Provider, then this method returns
   * null.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let ProviderListener[] listeners = this.getProviderListeners()
   * <LI>listeners == null or listeners.length >= 1
   * </OL>
   * @return An array of ProviderListener objects associated with this
   * Provider.
   */
  public ProviderListener[] getProviderListeners();


  /**
   * Removes the given listener from the Provider. The given listener will no
   * longer receive events generated by this Provider object. The final event
   * will have id <CODE>PROVIDER_EVENT_TRANSMISSION_ENDED</CODE>;
   * subsequently, the listener will no longer be listed
   * by the <CODE>Provider.getProviderListeners()</CODE> method.
   * <p>
   * Also, if the listener is not currently registered with the Provider, then this method
   * fails silently, i.e. no listener is removed an no exception is thrown.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>listener is not an element of this.getProviderListeners()
   * <LI>ProviderEvent with id <CODE>PROVIDER_EVENT_TRANSMISSION_ENDED</CODE>
   * is delivered to listener
   * </OL>
   * @param listener The listener which is being removed.
   */ 
  public void removeProviderListener(ProviderListener listener);


}
