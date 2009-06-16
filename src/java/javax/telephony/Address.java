/*
 * Copyright (c) 1997 Sun Microsystems, Inc. All Rights Reserved.
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
import  javax.telephony.capabilities.AddressCapabilities;

/**
 * An <CODE>Address</CODE> object represents what we commonly think of as a "telephone
 * number".
 *
 * <H4>Introduction</H4>
 *
 * In implementations where the underlying network is not a telephone
 * network, this address may represent something else. For example, if the
 * underlying network is IP, this address might represent an IP address
 * (e.g. 18.203.0.49). An Address object has a string <EM>name</EM> which
 * corresponds to this telephone address. The Address object does not attempt
 * to interpret this string in any way. This name is first assigned when the
 * Address object is created and does not change throughout the lifetime of
 * the object. The method <CODE>Address.getName()</CODE> returns the name of
 * the Address object.
 * <p>
 * Address objects may be classified into two categories: <EM>local</EM> and
 * <EM>remote</EM>. Local Address objects are those addresses which are part
 * of the Provider's local domain. These Address objects are created by the
 * implementation of the Provider object when it is first instantiated. All
 * of the Provider's local addresses are reported via the
 * <CODE>Provider.getAddresses()</CODE> method. Remote Address objects are
 * those outside of the Provider's domain which the Provider learns about
 * during its lifetime through various happenings (e.g. an incoming call from
 * a currently unknown address). Remote Addresses are <B>not</B> reported via
 * the <CODE>Provider.getAddresses()</CODE> method. Note that applications
 * never explicitly create new Address objects.
 *
 * <H4>Address and Terminal Objects</H4>
 *
 * Address and Terminal objects exist in a many-to-many relationship. An
 * Address object may have zero or more Terminals associated with it. Each
 * Terminal associated with an Address must reflect its association with the
 * Address. Since the implementation creates Address (and Terminal) objects,
 * it is responsible for insuring the correctness of these relationships. The 
 * Terminals associated with an Address is given by the
 * <CODE>Address.getTerminals()</CODE> method.
 * <p>
 * An association between an Address and Terminal indicates that the Terminal
 * is addressable via that Address.  In many instances, a telephone set
 * (represented by a Terminal object) has only one telephone number
 * (represented by an Address object) associated with it. In more complex
 * configurations, telephone sets may have several telephone numbers associated
 * with them. Likewise, a telephone number may appear on more than one
 * telephone set. For example, feature phones in PBX environments may exhibit
 * this configuration.
 *
 * <H4>Address and Call Objects</H4>
 *
 * Address objects represent the <EM>logical</EM> endpoints of a telephone
 * call. A logical view of a telephone call views the call as originating
 * from one Address endpoint and terminates at another Address endpoint.
 * <p>
 * Address objects are related to Call objects via the Connection object. The
 * Connection object has a <EM>state</EM> which describes the current
 * relationship between the Call and the Address. Each Address object may be
 * part of more than one telephone call, and in each case, is represented by
 * a separate Connection object. The <CODE>Address.getConnections()</CODE>
 * method returns all Connection objects currently associated with the Call.
 * <p>
 * An Address is associated with a Call until the Connection moves into the
 * <CODE>Connection.DISCONNECTED</CODE> state. At that time, the Connection is
 * no longer reported via the <CODE>Address.getConnections()</CODE> method.
 * Therefore, the <CODE>Address.getConnections()</CODE> method will never
 * report a Connection in the <CODE>Connection.DISCONNECTED</CODE> state.
 *
 * <H4>Existing Telephone Calls</H4>
 *
 * The Java Telephony API specification states that the implementation is
 * responsible for reporting all existing telephone calls when a Provider is
 * first created. This implies that an Address object must report information
 * regarding existing telephone calls to that Address. In other words, Address
 * objects must reports all Connection objects which represent existing
 * telephone calls.
 *
 * <H4>Address Listeners and Events</H4>
 *
 * All changes in an Address object are reported via the AddressListener
 * interface. Applications instantiate an object which implements this
 * interface and begins this delivery of events to this object using the
 * <CODE>Address.addAddressListener()</CODE> method. All Address-related events extend
 * the <CODE>AddressEvent</CODE> interface provided in the core package. Applications
 * receive events on an listener until the listener is removed via the
 * <CODE>Address.removeAddressListener()</CODE> method or until the Address is no
 * longer observable. In these instances, each AddressListener receives a
 * ADDRESS_EVENT_TRANSMISSION_ENDED as its final event.
 * <p>
 * Currently in the core package, the only Address-related event is
 * ADDRESS_EVENT_TRANSMISSION_ENDED.
 * 
 * <H4>Call Listeners</H4>
 *
 * At times, applications may want to monitor a particular Address for all
 * Calls which come to that Address. For example, a customer service agent
 * application is only interested in telephone calls associated with a
 * particular agent address. To achieve this sort of Address-based Call
 * monitoring applications may <EM>add</EM> CallListeners to an Address via the
 * <CODE>Address.addCallListener()</CODE> method.
 * <p>
 * When a CallListener is added to an Address, this listener instance is
 * immediately added to all Calls at this Address and is added to all Calls
 * which come to this Address in the future. These listeners remain on the
 * telephone call as long as the Address is associated with the telephone call.
 * <p>
 * The specification of <CODE>Address.addCallListener()</CODE> contains more
 * precise semantics.
 * <p>
 * @see javax.telephony.AddressListener
 * @see javax.telephony.CallListener
 * @version %G% %I%
 */

public interface Address {

  /**
   * Returns the name of the Address. Each Address possesses a unique
   * name. This name is a way of referencing an endpoint in a telephone call.
   * <p>
   * @return The name of this Address.
   */
  public String getName();


  /**
   * Returns the Provider associated with this Address. This Provider object
   * is valid throughout the lifetime of the Address and does not change
   * once the Address is created.
   * <p>
   * @return The Provider associated with this Address.
   */
  public Provider getProvider();


  /**
   * Returns an array of Terminals associated with this Address object.
   * If no Terminals are associated with this Address, this method returns
   * null. This list does not change throughout the lifetime of the Address
   * object.
   * <p>
   * @return An array of Terminal objects associated with this Address.
   */
  public Terminal[] getTerminals();


  /**
   * Returns an array of Connection objects currently associated with this
   * Address object at the instant the <CODE>getConnections()</CODE> method 
   * is called. When a Connection moves into the
   * <CODE>Connection.DISCONNECTED</CODE> state, the Address object loses the
   * reference to the Connection and the Connection no longer returned by this
   * method. Therefore, all Connections returned by this method will never be
   * in the <CODE>Connection.DISCONNECTED</CODE> state. If the Address has no
   * Connections associated with it, this method returns null.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let Connection c[] = this.getConnections()
   * <LI>c == null or c.length >= 1
   * <LI>For all i, c[i].getState() != Connection.DISCONNECTED
   * </OL>
   * @return An array of Connection objects associated with this Address.
   */
  public Connection[] getConnections();

  /**
   * Adds an observer to the Address. The AddressObserver reports all Address-
   * related state changes as events. The Address object will report events to
   * this AddressObserver object for the lifetime of the Address object or
   * until the observer is removed with the
   * <CODE>Address.removeObserver()</CODE> method or until the Address is no
   * longer observable. In these instances, a AddrObservationEndedEv is
   * delivered to the observer as its final event. The observer will receive
   * no events after AddrObservationEndedEv unless the observer is explicitly
   * re-added via the <CODE>Address.addObserver()</CODE> method. Also, once
   * an observer receives an AddrObservationEndedEv, it will no longer be
   * reported via the <CODE>Address.getObservers()</CODE>.
   * <p>
   * If an application attempts to add an instance of an observer already
   * present on this Address, this attempt will silently fail, i.e. multiple
   * instances of an observer are not added and no exception will be thrown.
   * <p>
   * Currently, only the AddrObservationEndedEv event is defined by the core
   * package and delivered to the AddressObserver.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>observer is an element of this.getObservers()
   * </OL>
   * @param observer The observer being added.
   * @exception MethodNotSupportedException The observer cannot be added at this time
   * @exception ResourceUnavailableException The resource limit for the
   * number of observers has been exceeded.
   */
  public void addObserver(AddressObserver observer)
    throws ResourceUnavailableException,MethodNotSupportedException;


  /**
   * Returns a list of all AddressObservers associated with this Address
   * object. If there are no observers associated with this Address object,
   * this method returns null.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let AddressObserver[] obs = this.getObservers()
   * <LI>obs == null or obs.length >= 1
   * </OL>
   * @return An array of AddressObserver objects associated with this
   * Address.
   */
  public AddressObserver[] getObservers();


  /**
   * Removes the given observer from the Address. If successful, the observer
   * will no longer receive events generated by this Address object. As its
   * final event, the AddressObserver receives is an AddrObservationEndedEv
   * event.
   * <p>
   * If an observer is not part of the Address, then this method fails
   * silently, i.e. no observer is removed and no exception is thrown.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>An AddrObservationEndedEv event is reported to the observer as its
   * final event.
   * <LI>observer is not an element of this.getObservers()
   * </OL>
   * @param observer The observer which is being removed.
   */
  public void removeObserver(AddressObserver observer);


  /**
   * Adds an observer to a Call object when this Address object first becomes
   * part of that Call. This method permits applications to select an Address
   * object in which they are interested and automatically have the
   * implementation attach an observer to all present and future Calls which
   * come to this Address.
   *
   * <H5>JTAPI v1.0 Semantics</H5>
   *
   * In version 1.0 of the Java Telephony API specification, the application
   * monitored the Address object for the AddrCallAtAddrEv event. This event
   * indicated that a Call has come to this Address. Then, applications would
   * manually add an observer to the Call. With this architecture, potentially
   * dangerous race conditions existed while an application was adding an
   * observer to the Call. As a result, this mechanism has been replaced in
   * version 1.1.
   *
   * <H5>JTAPI v1.1 Semantics</H5>
   *
   * In version 1.1 of the specification, the AddrCallAtAddrEv event does not
   * exist and this method replaces the functionality described above. Instead
   * of monitoring for a AddrCallAtAddrEv event, this application simply uses
   * the <CODE>Address.addCallObserver()</CODE> method, and observer will be
   * added to new telephone calls at this Address automatically.
   * <p>
   * If an application attempts to add an instance of a call observer already
   * present on this Address, these repeated attempts will silently fail, i.e.
   * multiple instances of a call observer are not added and no exception will
   * be thrown.
   * <p>
   * When a call observer is added to an Address with this method, the
   * following behavior is exhibited by the implementation.
   * <p>
   * <OL>
   * <LI>It is immediately associated with any existing calls at the Address
   * and a snapshot of those calls are reported to the call observer. For
   * each of these calls, the observer is reported via
   * <CODE>Call.getObservers()</CODE>.
   * <LI>It is associated with all future calls which comes to this Address.
   * For each new call, the observer is reported via
   * <CODE>Call.getObservers().</CODE>
   * </OL>
   * <p>
   * Note that the definition of the term ".. when a call is at an Address"
   * means that the Call contains one Connection which has this Address as
   * its Address.
   *
   * <H5>Call Observer Lifetime</H5>
   *
   * For all call observers which are present on Calls because of this method,
   * the following behavior is exhibited with respect to the lifetime of the
   * call.
   * <p>
   * <OL>
   * <LI>The call observer receives events until the Call is no longer at this
   * Address. In this case, the call observer will be re-applied to the Call if
   * the Call returns to this Address at some point in the future.
   * <LI>The call observer is removed with <CODE>Call.removeObserver()</CODE>.
   * Note that this only affects the instance of the call observer for that
   * particular call. If the Call subsequently leaves and returns to the
   * Address, the observer will be re-applied.
   * <LI>The Call can no longer be monitored by the implementation.
   * <LI>The Call moves into the <CODE>Call.INVALID</CODE> state.
   * </OL>
   * <p>
   * When the CallObserver leaves the Call because of one of the reasons above,
   * it receives a CallObservationEndedEv as its final event.
   *
   * <H5>Call Observer on Multiple Addresses and Terminals</H5>
   *
   * It is possible for an application to add CallObservers at more than one
   * Address and Terminal (using <CODE>Address.addCallObserver()</CODE> and
   * <CODE>Terminal.addCallObserver()</CODE>, respectively). The rules outlined
   * above still apply, with the following additions:
   * <p>
   * <OL>
   * <LI>A CallObserver is not added to a Call more than once, even if it
   * has been added to more than one Address/Terminal which are present on the
   * Call.
   * <LI>The CallObserver leaves the call only if <EM>all</EM> of the
   * Addresses and Terminals on which the Call Observer was added leave the
   * Call. If one of those Addresses/Terminals becomes part of the Call again,
   * the call observer is re-applied to the Call.
   * </OL>
   * <p>
   * <B>Post-Conditions:</B>
   * <OL>
   * <LI>observer is an element of this.getCallObservers()
   * <LI>observer is an element of Call.getObservers() for each Call associated
   * with the Connections from this.getConnections().
   * <LI>An array of snapshot events are reported to the observer for existing
   * calls associated with this Address.
   * </OL>
   * @see javax.telephony.Call
   * @param observer The observer being added.
   * @exception MethodNotSupportedException The observer cannot be added at this time.
   * @exception ResourceUnavailableException The resource limit for the
   * numbers of observers has been exceeded.
   */
  public void addCallObserver(CallObserver observer)
    throws ResourceUnavailableException,MethodNotSupportedException;


  /**
   * Returns a list of all CallObservers associated with this Address
   * object. That is, it returns a list of CallObserver object which have
   * been added via the <CODE>Address.addCallObserver()</CODE> method. If
   * there are no CallObservers associated with this Address object, this
   * method returns null.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let CallObserver[] obs = this.getCallObservers()
   * <LI>obs == null or obs.length >= 1
   * </OL>
   * @return An array of CallObserver objects associated with this
   * Address.
   */
  public CallObserver[] getCallObservers();


  /**
   * Removes the given CallObserver from the Address. In other words, it
   * removes a CallObserver which was added via the
   * <CODE>Address.addCallObserver()</CODE> method. If successful, the
   * observer will no longer be added to new Calls which are presented to this
   * Address, however it does not affect CallObservers which have already been
   * added at a Call.
   * <p>
   * Also, if the CallObserver is not part of the Address, then this method
   * fails silently, i.e. no observer is removed and no exception is thrown.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>observer is not an element of this.getCallObservers()
   * </OL>
   * @param observer The CallObserver which is being removed.
   */
  public void removeCallObserver(CallObserver observer);


  /**
   * Returns the dynamic capabilities for this instance of the Address object.
   * Dynamic capabilities tell the application which actions are possible at
   * the time this method is invoked based upon the implementations knowledge
   * of its ability to successfully perform the action. This determination may
   * be based upon argument passed to this method, the current state of the
   * call model, or some implementation-specific knowledge. These indications
   * do not guarantee that a particular method will succeed when invoked,
   * however.
   * <p>
   * The dynamic Address capabilities require no additional arguments.
   * <p>
   * @return The dynamic Address capabilities.
   */
  public AddressCapabilities getCapabilities();


  /**
   * Gets the AddressCapabilities object with respect to a Terminal 
   * If null is passed as a Terminal parameter, the general/provider-wide
   * Address capabilities are returned.
   * <p>
   * <STRONG>Note:</STRONG> This method has been replaced in JTAPI v1.2. The
   * <CODE>Address.getCapabilities()</CODE> method returns the dynamic Address
   * capabilities. This method now should simply invoke the
   * <CODE>Address.getCapabilities()</CODE> method.
   * <p>
   * @deprecated Since JTAPI v1.2. This method has been replaced by the
   * Address.getCapabilities() method.
   * @param terminal This argument is ignored in JTAPI v1.2 and later.
   * @exception InvalidArgumentException This exception is never thrown in
   * JTAPI v1.2 and later.
   * @exception PlatformException A platform-specific exception occurred.
   */
  public AddressCapabilities getAddressCapabilities(Terminal terminal)
    throws InvalidArgumentException, PlatformException;


  /**
   * Adds an listener to the Address. The AddressListener reports all Address-
   * related state changes as events. The Address object will report events to
   * this AddressListener object for the lifetime of the Address object or
   * until the listener is removed with the
   * <CODE>Address.removeAddressListener()</CODE> method or until the Address is no
   * longer observable. In these instances, a ADDRESS_EVENT_TRANSMISSION_ENDED is
   * delivered to the listener as its final event. The listener will receive
   * no events after ADDRESS_EVENT_TRANSMISSION_ENDED unless the listener is explicitly
   * re-added via the <CODE>Address.addAddressListener()</CODE> method. Also, once
   * an listener receives an ADDRESS_EVENT_TRANSMISSION_ENDED, it will no longer be
   * reported via the <CODE>Address.getAddressListeners()</CODE>.
   * <p>
   * If an application attempts to add an instance of an listener already
   * present on this Address, this attempt will silently fail, i.e. multiple
   * instances of an listener are not added and no exception will be thrown.
   * <p>
   * Currently, only the ADDRESS_EVENT_TRANSMISSION_ENDED event is defined by the core
   * package and delivered to the AddressListener.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>listener is an element of this.getAddressListeners()
   * </OL>
   * @param listener The listener being added.
   * @exception MethodNotSupportedException The listener cannot be added at this time
   * @exception ResourceUnavailableException The resource limit for the
   * number of listeners has been exceeded.
   */
  public void addAddressListener(AddressListener listener)
    throws ResourceUnavailableException,MethodNotSupportedException;


  /**
   * Returns a list of all JAddressListeners associated with this Address
   * object. If there are no listeners associated with this Address object,
   * this method returns null.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let AddressListener[] obs = this.getAddressListeners()
   * <LI>obs == null or obs.length >= 1
   * </OL>
   * @return An array of AddressListener objects associated with this
   * Address.
   */
  public AddressListener[] getAddressListeners();


  /**
   * Removes the given listener from the Address. If successful, the listener
   * will no longer receive events generated by this Address object. As its
   * final event, the AddressListener receives is an ADDRESS_EVENT_TRANSMISSION_ENDED
   * event.
   * <p>
   * If an listener is not part of the Address, then this method fails
   * silently, i.e. no listener is removed and no exception is thrown.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>An ADDRESS_EVENT_TRANSMISSION_ENDED event is reported to the listener as its
   * final event.
   * <LI>listener is not an element of this.getAddressListeners()
   * </OL>
   * @param listener The listener which is being removed.
   */
  public void removeAddressListener(AddressListener listener);


  /**
   * Adds an listener to a Call object when this Address object first becomes
   * part of that Call. This method permits applications to select an Address
   * object in which they are interested and automatically have the
   * implementation attach an listener to all present and future Calls which
   * come to this Address.
   *
   * <H5>JTAPI v1.0 Semantics</H5>
   *
   * In version 1.0 of the Java Telephony API specification, the application
   * monitored the Address object for the AddrCallAtAddrEv event. This event
   * indicated that a Call has come to this Address. Then, applications would
   * manually add an listener to the Call. With this architecture, potentially
   * dangerous race conditions existed while an application was adding an
   * listener to the Call. As a result, this mechanism has been replaced in
   * version 1.1.
   *
   * <H5>JTAPI v1.1 Semantics</H5>
   *
   * In version 1.1 of the specification, the AddrCallAtAddrEv event does not
   * exist and this method replaces the functionality described above. Instead
   * of monitoring for a AddrCallAtAddrEv event, this application simply uses
   * the <CODE>Address.addCallListener()</CODE> method, and listener will be
   * added to new telephone calls at this Address automatically.
   * <p>
   * If an application attempts to add an instance of a call listener already
   * present on this Address, these repeated attempts will silently fail, i.e.
   * multiple instances of a call listener are not added and no exception will
   * be thrown.
   * <p>
   * When a call listener is added to an Address with this method, the
   * following behavior is exhibited by the implementation.
   * <p>
   * <OL>
   * <LI>It is immediately associated with any existing calls at the Address
   * and a snapshot of those calls are reported to the call listener. For
   * each of these calls, the listener is reported via
   * <CODE>Call.getAddressListeners()</CODE>.
   * <LI>It is associated with all future calls which comes to this Address.
   * For each new call, the listener is reported via
   * <CODE>Call.getAddressListeners().</CODE>
   * </OL>
   * <p>
   * Note that the definition of the term ".. when a call is at an Address"
   * means that the Call contains one Connection which has this Address as
   * its Address.
   *
   * <H5>Call Listener Lifetime</H5>
   *
   * For all call listeners which are present on Calls because of this method,
   * the following behavior is exhibited with respect to the lifetime of the
   * call.
   * <p>
   * <OL>
   * <LI>The call listener receives events until the Call is no longer at this
   * Address. In this case, the call listener will be re-applied to the Call if
   * the Call returns to this Address at some point in the future.
   * <LI>The call listener is removed with <CODE>Call.removeCallListener()</CODE>.
   * Note that this only affects the instance of the call listener for that
   * particular call. If the Call subsequently leaves and returns to the
   * Address, the listener will be re-applied.
   * <LI>The Call can no longer be monitored by the implementation.
   * <LI>The Call moves into the <CODE>Call.INVALID</CODE> state.
   * </OL>
   * <p>
   * When the CallListener leaves the Call because of one of the reasons above,
   * it receives a CALL_EVENT_TRANSMISSION_ENDED as its final event.
   *
   * <H5>Call Listener on Multiple Addresses and Terminals</H5>
   *
   * It is possible for an application to add CallListeners at more than one
   * Address and Terminal (using <CODE>Address.addCallListener()</CODE> and
   * <CODE>Terminal.addCallListener()</CODE>, respectively). The rules outlined
   * above still apply, with the following additions:
   * <p>
   * <OL>
   * <LI>A CallListener is not added to a Call more than once, even if it
   * has been added to more than one Address/Terminal which are present on the
   * Call.
   * <LI>The CallListener leaves the call only if <EM>all</EM> of the
   * Addresses and Terminals on which the Call Listener was added leave the
   * Call. If one of those Addresses/Terminals becomes part of the Call again,
   * the call listener is re-applied to the Call.
   * </OL>
   * <p>
   * <B>Call Listener Event level of Granularity</B>
   * <p>
   * The application shall receive the event reports associated with the methods
   * which are defined in the CallListener interface that the application implements -
   * either the CallListener, ConnectionListener or TerminalConnectionListener interface.
   * Note that the CallListener, ConnectionListener and TerminalConnectionListener
   * form a hierarchy of interfaces, and so provide successively increasing amounts
   * of call information and successively finer-grained event granularity.
   * <p>
   * <B>Post-Conditions:</B>
   * <OL>
   * <LI>listener is an element of this.getCallListeners()
   * <LI>listener is an element of Call.getCallListeners() for each Call associated
   * with the Connections from this.getConnections().
   * <LI>An array of snapshot events are reported to the listener for existing
   * calls associated with this Address.
   * </OL>
   * @see javax.telephony.Call
   * @param listener The listener being added.
   * @exception MethodNotSupportedException The listener cannot be added at this time.
   * @exception ResourceUnavailableException The resource limit for the
   * numbers of listeners has been exceeded.
   */
  public void addCallListener(CallListener listener)
    throws ResourceUnavailableException,MethodNotSupportedException;


  /**
   * Returns a list of all CallListeners associated with this Address
   * object. That is, it returns a list of CallListener object which have
   * been added via the <CODE>Address.addCallListener()</CODE> method. If
   * there are no CallListeners associated with this Address object, this
   * method returns null.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let CallListener[] obs = this.getCallListeners()
   * <LI>obs == null or obs.length >= 1
   * </OL>
   * @return An array of CallListener objects associated with this
   * Address.
   */
  public CallListener[] getCallListeners();


  /**
   * Removes the given CallListener from the Address. In other words, it
   * removes a CallListener which was added via the
   * <CODE>Address.addCallListener()</CODE> method. If successful, the
   * listener will no longer be added to new Calls which are presented to this
   * Address, however it does not affect CallListeners which have already been
   * added at a Call.
   * <p>
   * Also, if the CallListener is not part of the Address, then this method
   * fails silently, i.e. no listener is removed and no exception is thrown.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>listener is not an element of this.getCallListeners()
   * </OL>
   * @param listener The CallListener which is being removed.
   */
  public void removeCallListener(CallListener listener);


}
