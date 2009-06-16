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
import javax.telephony.capabilities.TerminalCapabilities;

/**
 * A <CODE>Terminal</CODE> represents a physical hardware endpoint connected to the
 * telephony domain.
 *
 * <H4>Introduction</H4>
 *
 * An example of a Terminal is a telephone set, but
 * a Terminal does not have to take the form of this limited and traditional
 * example. For example, computer workstations and hand-held devices are
 * modeled as Terminals if they act as physical endpoints in a telephony
 * network.
 * <p>
 * A Terminal object has a string <EM>name</EM> which is unique for all
 * Terminal objects. The Terminal does not attempt to interpret this string in
 * any way. This name is first assigned when the Terminal is created and
 * does not change throughout the lifetime of the object. The method
 * <CODE>Terminal.getName()</CODE> returns the name of the Terminal object. The
 * name of the Terminal may not have any real-world interpretation. Typically,
 * Terminals are chosen from a list of Terminals obtained from an Address
 * object.
 * <p>
 * Terminal objects may be classified into two categories: <EM>local</EM> and
 * <EM>remote</EM>. Local Terminal objects are those terminals which are part
 * of the Provider's local domain. These Terminal objects are created by the
 * implementation of the Provider object when it is first instantiated. All
 * of the Provider's local terminals are reported via the
 * <CODE>Provider.getTerminals()</CODE> method. Remote Terminal objects are
 * those outside of the Provider's domain which the Provider learns about
 * during its lifetime through various happenings (e.g. an incoming call from
 * a currently unknown address). Remote Terminal objects are <B>not</B>
 * reported via the <CODE>Provider.getTerminals()</CODE> method. Note that
 * applications never explicitly create new Terminal objects.
 *
 * <H4>Address and Terminal objects</H4>
 *
 * Address and Terminal objects exist in a many-to-many relationship. An
 * Address object may have zero or more Terminals associated with it. For
 * each Terminal associated with an Address, that Terminal must also reflect
 * its association with the Address. Since the implementation creates Address
 * (and Terminal) objects, it is responsible for insuring the correctness of
 * these relationships. The Terminals associated with an Address is given by
 * the <CODE>Address.getTerminals()</CODE> method.
 * <p>
 * An association between an Address and Terminal object indicates that the
 * Terminal contains the Address object as one of its telephone number
 * addresses. In many instances, a telephone set (represented by a Terminal
 * object) has only one telephone number (represented by an Address object)
 * associated with it. In more complex configurations, telephone sets may have
 * several telephone numbers associated with them. Likewise, a telephone number
 * may appear on more than one telephone set. For example, feature phones in
 * PBX environments may exhibit this configuration.
 *
 * <H4>Terminals and Call objects</H4>
 *
 * Terminal objects represent the <EM>physical</EM> endpoints of a telephone
 * call. With respect to a single Address endpoint on a Call, multiple physical
 * Terminal endpoints may exist. Terminal objects are related to Call objects
 * via the TerminalConnection object. TerminalConnection objects are
 * associated with Call indirectly via Connections. A Terminal may be
 * associated with a Call only if one of its Addresses is associated with the
 * Call. The TerminalConnection object has a <EM>state</EM> which describes
 * the current relationship between the Connection and the Terminal. Each
 * Terminal object may be part of more than one telephone call, and in each
 * case, is represented by a separate TerminalConnection objet. The
 * <CODE>Terminal.getTerminalConnections()</CODE> method returns all
 * TerminalConnection object currently associated with the Terminal.
 * <p>
 * A Terminal object is associated with a Connection until the
 * TerminalConnection moves into the <CODE>TerminalConnection.DROPPED</CODE>
 * state. At that time, the TerminalConnection is no longer reported via the
 * <CODE>Terminal.getTerminalConnections()</CODE> method. Therefore, the
 * <CODE>Terminal.getTerminalConnections()</CODE> method never reports a
 * TerminalConnection in the <CODE>TerminalConnection.DROPPED</CODE> state.
 *
 * <H4>Existing Telephone Calls</H4>
 *
 * The Java Telephony API specification states that the implementation is
 * responsible for reporting all existing telephone calls when a Provider is
 * first created. This implies that an Terminal object must report information
 * regarding existing telephone calls to that Terminal. In other words,
 * Terminal objects must report all TerminalConnection objects which
 * represent existing telephone calls.
 *
 * <H4>Terminal Listeners and Events</H4>
 *
 * All changes in an Terminal object are reported via the TerminalListener
 * interface. Applications instantiate an object which implements this
 * interface and begins this delivery of events to this object using the
 * <CODE>Terminal.addTerminalListener()</CODE> method. All Terminal-related events
 * extend the <CODE>TerminalEvent</CODE> interface provided in the core package.
 * Applications receive events on an listener until the listener is removed
 * via the <CODE>Terminal.removeTerminalListener()</CODE> method or until the Terminal
 * is no longer observable. In these instances, each TerminalListener receives
 * a TERMINAL_EVENT_TRANSMISSION_ENDED as its final event.
 * <p>
 * Currently in the core package, the only Terminal-related event is
 * TERMINAL_EVENT_TRANSMISSION_ENDED.
 * 
 * <H4>Call Listeners</H4>
 *
 * At times, applications may want to monitor a particular Terminal for all
 * Calls which come to that Terminal. For example, a desktop telephone
 * application is only interested in telephone calls associated with a
 * particular agent terminal. To achieve this sort of Terminal-based Call
 * monitoring applications may <EM>add</EM> CallListeners to an Terminal via
 * the <CODE>Terminal.addCallListener()</CODE> method.
 * <p>
 * When a CallListener is added to an Terminal, this listener instance is
 * immediately added to all Calls at this Terminal and is added to all Calls
 * which come to this Terminal in the future. These listeners remain on the
 * telephone call as long as the Terminal is associated with the telephone
 * call.
 * <p>
 * The specification of <CODE>Terminal.addCallListener()</CODE> contains more
 * precise semantics.
 * <p>
 * @see javax.telephony.TerminalListener
 * @see javax.telephony.CallListener
 * @version 04/06/99 1.65
 */

public interface Terminal {

  /**
   * Returns the name of the Terminal. Each Terminal possesses a unique
   * name. This name is assigned by the implementation and may or may not
   * carry a real-world interpretation.
   * <p>
   * @return The name of the Terminal.
   */
  public String getName();


  /**
   * Returns the Provider associated with this Terminal. This Provider object
   * is valid throughout the lifetime of the Terminal and does not change
   * once the Terminal is created.
   * <p>
   * @return The Provider associated with this Terminal.
   */
  public Provider getProvider();


  /**
   * Returns an array of Address objects associated with this Terminal object.
   * The Terminal object must have at least one Address object associated
   * with it. This list does not change throughout the lifetime of the
   * Terminal object.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let Address[] addrs = this.getAddresses()
   * <LI>addrs.length >= 1
   * </OL>
   * @return An array of Address objects associated with this Terminal.
   */
  public Address[] getAddresses();


  /**
   * Returns an array of TerminalConnection objects associated with this
   * Terminal. Once a TerminalConnection is added to a Terminal, the Terminal
   * maintains a reference until the TerminalConnection moves into the
   * <CODE>TerminalConnection.DROPPED</CODE> state. Therefore, all
   * TerminalConnections returned by this method will never be in the
   * <CODE>TerminalConnection.DROPPED</CODE> state. If there are no
   * TerminalConnections associated with this Terminal, this method returns
   * null.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let TerminalConnection tc[] = this.getTerminalConnections()
   * <LI>tc == null or tc.length >= 1
   * <LI>For all i, tc[i].getState() != TerminalConnection.DROPPED
   * </OL>
   * @return An array of TerminalConnection objects associated with this
   * Terminal.
   */
  public TerminalConnection[] getTerminalConnections();


  /**
   * Adds an observer to the Terminal. The TerminalObserver reports all
   * Terminal-related state changes as events. The Terminal object will report
   * events to this TerminalObserver object for the lifetime of the Terminal
   * object or until the observer is removed with the
   * <CODE>Terminal.removeObserver()</CODE> or until the Terminal is no
   * longer observable. In these instances, a TermObservationEndedEv is
   * delivered to the observer as its final event. The observer will receive
   * no events after TermObservationEndedEv unless the observer is explicitly
   * re-added via the <CODE>Terminal.addObserver()</CODE> method. Also, once
   * an observer receives an TermObservationEndedEv, it will no longer be
   * reported via the <CODE>Terminal.getObservers()</CODE>.
   * <p>
   * If an application attempts to add an instance of an observer already
   * present on this Terminal, this attempt will silently fail, i.e. multiple
   * instances of an observer are not added and no exception will be thrown.
   * <p>
   * Currently, only the TermObservationEndedEv event is defined by the core
   * package and delivered to the TerminalObserver.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>observer is an element of this.getObservers()
   * </OL>
   * @param observer The observer being added.
   * @exception MethodNotSupportedException The observer cannot be added at this time
   * @exception ResourceUnavailableException The resource limit for the
   * numbers of observers has been exceeded.
   */ 
  public void addObserver(TerminalObserver observer)
    throws ResourceUnavailableException,MethodNotSupportedException;


  /**
   * Returns a list of all TerminalObservers associated with this Terminal
   * object. If there are no observers associated with this Terminal, this
   * method returns null.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let TerminalObserver[] obs = this.getObservers()
   * <LI>obs == null or obs.length >= 1
   * </OL>
   * @return An array of TerminalObserver objects associated with this
   * Terminal.
   */
  public TerminalObserver[] getObservers();


  /**
   * Removes the given observer from the Terminal. If successful, the observer
   * will no longer receive events generated by this Terminal object. As its
   * final event, the TerminalObserver receives a TermObservationEndedEv.
   * <p>
   * If an observer is not part of the Terminal, then this method
   * fails silently, i.e. no observer is removed an no exception is thrown.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>A TermObservationEndedEv event is reported to the observer as its
   * final event.
   * <LI>observer is not an element of this.getObservers()
   * </OL>
   * @param observer The observer which is being removed.
   */ 
  public void removeObserver(TerminalObserver observer);


  /**
   * Adds an observer to a Call object when this Terminal object first becomes
   * part of that Call. This method permits applications to select a Terminal
   * object in which they are interested and automatically have the
   * implementation attach an observer to all present and future Calls which
   * come to this Terminal.
   *
   * <H5>JTAPI v1.0 Semantics</H5>
   *
   * In version 1.0 of the Java Telephony API specification, the application
   * monitored the Terminal object for the TermCallAtTermEv event. This event
   * indicated that a Call has come to this Terminal. Then, applications would
   * manually add an observer to the Call. With this architecture, potentially
   * dangerous race conditions existed while an application was adding an
   * observer to the Call. As a result, this mechanism has been replaced in
   * version 1.1.
   *
   * <H5>JTAPI v1.1 Semantics</H5>
   *
   * In version 1.1 of the specification, the TermCallAtTermEv event does not
   * exist and this method replaces the functionality described above. Instead
   * of monitoring for a TermCallAtTermEv event, the application simply uses
   * the <CODE>Terminal.addCallObserver()</CODE> method, and observer will be 
   * added to new telephone calls at this Terminal automatically.
   * <p>
   * If an application attempts to add an instance of a call observer already
   * present on this Terminal, these repeated attempts will silently fail, i.e.
   * multiple instances of a call observer are not added and no exception will
   * be thrown.
   * <p>
   * When a call observer is added to an Terminal with this method, the
   * following behavior is exhibited by the implementation.
   * <p>
   * <OL>
   * <LI>It is immediately associated with any existing calls at the Terminal
   * and a snapshot of those calls are reported to the call observer. For
   * each of these calls, the observer is reported via
   * <CODE>Call.getObservers()</CODE>.
   * <LI>It is associated with all future calls which come to this Terminal.
   * For each new call, the observer is reported via
   * <CODE>Call.getObservers().</CODE>
   * </OL>
   * <p>
   * Note that the definition of the term ".. when a call is at a Terminal"
   * means that the Call contains a Connection which contains a
   * TerminalConnection with this Terminal as its Terminal.
   *
   * <H5>Call Observer Lifetime</H5>
   *
   * For all call observers which are present on Calls because of this method,
   * the following behavior is exhibited with respect to the lifetime of the
   * call.
   * <p>
   * <OL>
   * <LI>The call observer receives events until the Call is no longer at this
   * Terminal. In this case, the call observer will be re-applied to the Call
   * if the Call returns to this Terminal at some point in the future.
   * <LI>The call observer is removed with <CODE>Call.removeObserver()</CODE>.
   * Note that this only affects the instance of the call observer for that
   * particular call. If the Call subsequently leaves and returns to the
   * Terminal, the observer will be re-applied.
   * <LI>The Call can no longer be monitored by the implementation.
   * <LI>The Call moves into the <CODE>Call.INVALID</CODE> state.
   * </OL>
   * <p>
   * When the CallObserver leaves the Call because of one of the reasons above,
   * it receives a CallObservationEndedEv as its final event.
   * <p>
   * <H5>Call Observer on Multiple Addresses and Terminals</H5>
   * It is possible for an application to add CallObservers to more than one
   * Address and Terminal (using <CODE>Address.addCallObserver()</CODE> and
   * <CODE>Terminal.addCallObserver()</CODE>, respectively). The rules outlined
   * above still apply, with the following additions:
   * <p>
   * <OL>
   * <LI>A CallObserver is not added to a Call more than once, even if it
   * has been added to more than one Address/Terminal which are present on the
   * Call.
   * <LI>The CallObserver leaves the call only if ALL of the Addresses and
   * Terminals on which the Call Observer was added leave the Call. If one
   * of those Addresses/Terminals becomes part of the Call again, the call
   * observer is re-applied to the Call.
   * </OL>
   * <p>
   * <B>Post-Conditions:</B>
   * <OL>
   * <LI>observer is an element of this.getCallObservers()
   * <LI>observer is an element of Call.getObservers() for each Call associated
   * with the Connections from this.getConnections().
   * <LI>An array of snapshot events are reported to the observer for existing
   * calls associated with this Terminal.
   * </OL>
   * @see javax.telephony.Call
   * @param observer The observer being added.
   * @exception MethodNotSupportedException The observer cannot be added at this time
   * @exception ResourceUnavailableException The resource limit for the
   * numbers of observers has been exceeded.
   */
  public void addCallObserver(CallObserver observer)
    throws ResourceUnavailableException,MethodNotSupportedException;


  /**
   * Returns a list of all CallObservers associated with this Terminal
   * object. That is, it returns a list of CallObserver objects which have
   * been added via the <CODE>Terminal.addCallObserver()</CODE> method. If
   * there are no Call observers associated with this Terminal object, this
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
   * Removes the given CallObserver from the Terminal. In other words, it
   * removes a CallObserver which was added via the
   * <CODE>Terminal.addCallObserver()</CODE> method. If successful, the
   * observer will no longer be added to new Calls which are presented to this
   * Terminal, however it does not affect CallObservers which have already been
   * added at a Call.
   * <p>
   * Also, if the CallObserver is not part of the Terminal, then this method
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
   * Returns the dynamic capabilities for the instance of the Terminal object.
   * Dynamic capabilities tell the application which actions are possible at
   * the time this method is invoked based upon the implementation's knowledge
   * of its ability to successfully perform the action. This determination may
   * be based upon argument passed to this method, the current state of the
   * call model, or some implementation-specific knowledge. These indications
   * do not guarantee that a particular method will be successful when invoked,
   * however.
   * <p>
   * The dynamic Terminal capabilities require no additional arguments.
   * <p>
   * @return The dynamic Terminal capabilities.
   */
  public TerminalCapabilities getCapabilities();


  /**
   * Gets the TerminalCapabilities object with respect to a Terminal and an
   * Address. If null is passed as a Terminal parameter, the general/provider-
   * wide Terminal capabilities are returned.
   * <p>
   * <STRONG>Note:</STRONG> This method has been replaced in JTAPI v1.2. The
   * <CODE>Terminal.getCapabilities()</CODE> method returns the dynamic
   * Terminal capabilities. This method now should simply invoke the
   * <CODE>Terminal.getCapabilities()</CODE> method.
   * <p>
   * @deprecated Since JTAPI v1.2. This method has been replaced by the
   * Terminal.getCapabilities() method.
   * @param address This argument is ignored in JTAPI v1.2 and later.
   * @param terminal This argument is ignored in JTAPI v1.2 and later.
   * @exception InvalidArgumentException This exception is never thrown in
   * JTAPI v1.2 and later.
   * @exception PlatformException A platform-specific exception occurred.
   */
  public TerminalCapabilities getTerminalCapabilities(Terminal terminal,
						      Address address)
    throws InvalidArgumentException, PlatformException;

  /**
   * Adds an listener to the Terminal. The TerminalListener reports all
   * Terminal-related state changes as events. The Terminal object will report
   * events to this TerminalListener object for the lifetime of the Terminal
   * object or until the listener is removed with the
   * <CODE>Terminal.removeTerminalListener()</CODE> or until the Terminal is no
   * longer observable. In these instances, a TERMINAL_EVENT_TRANSMISSION_ENDED is
   * delivered to the listener as its final event. The listener will receive
   * no events after TERMINAL_EVENT_TRANSMISSION_ENDED unless the listener is explicitly
   * re-added via the <CODE>Terminal.addTerminalListener()</CODE> method. Also, once
   * an listener receives an TERMINAL_EVENT_TRANSMISSION_ENDED, it will no longer be
   * reported via the <CODE>Terminal.getTerminalListeners()</CODE>.
   * <p>
   * If an application attempts to add an instance of an listener already
   * present on this Terminal, this attempt will silently fail, i.e. multiple
   * instances of an listener are not added and no exception will be thrown.
   * <p>
   * Currently, only the TERMINAL_EVENT_TRANSMISSION_ENDED event is defined by the core
   * package and delivered to the TerminalListener.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>listener is an element of this.getTerminalListeners()
   * </OL>
   * @param listener The listener being added.
   * @exception MethodNotSupportedException The listener cannot be added at this time
   * @exception ResourceUnavailableException The resource limit for the
   * numbers of listeners has been exceeded.
   */ 
  public void addTerminalListener(TerminalListener listener)
    throws ResourceUnavailableException,MethodNotSupportedException;


  /**
   * Returns a list of all TerminalListeners associated with this Terminal
   * object. If there are no listeners associated with this Terminal, this
   * method returns null.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let TerminalListener[] obs = this.getTerminalListeners()
   * <LI>obs == null or obs.length >= 1
   * </OL>
   * @return An array of TerminalListener objects associated with this
   * Terminal.
   */
  public TerminalListener[] getTerminalListeners();


  /**
   * Removes the given listener from the Terminal. If successful, the listener
   * will no longer receive events generated by this Terminal object. As its
   * final event, the TerminalListener receives a TERMINAL_EVENT_TRANSMISSION_ENDED.
   * <p>
   * If an listener is not part of the Terminal, then this method
   * fails silently, i.e. no listener is removed an no exception is thrown.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>A TERMINAL_EVENT_TRANSMISSION_ENDED event is reported to the listener as its
   * final event.
   * <LI>listener is not an element of this.getTerminalListeners()
   * </OL>
   * @param listener The listener which is being removed.
   */ 
  public void removeTerminalListener(TerminalListener listener);


  /**
   * Adds an listener to a Call object when this Terminal object first becomes
   * part of that Call. This method permits applications to select a Terminal
   * object in which they are interested and automatically have the
   * implementation attach an listener to all present and future Calls which
   * come to this Terminal.
   *
   * <H5>JTAPI v1.0 Semantics</H5>
   *
   * In version 1.0 of the Java Telephony API specification, the application
   * monitored the Terminal object for the TermCallAtTermEv event. This event
   * indicated that a Call has come to this Terminal. Then, applications would
   * manually add an listener to the Call. With this architecture, potentially
   * dangerous race conditions existed while an application was adding an
   * listener to the Call. As a result, this mechanism has been replaced in
   * version 1.1.
   *
   * <H5>JTAPI v1.1 Semantics</H5>
   *
   * In version 1.1 of the specification, the TermCallAtTermEv event does not
   * exist and this method replaces the functionality described above. Instead
   * of monitoring for a TermCallAtTermEv event, the application simply uses
   * the <CODE>Terminal.addCallListener()</CODE> method, and listener will be 
   * added to new telephone calls at this Terminal automatically.
   * <p>
   * If an application attempts to add an instance of a call listener already
   * present on this Terminal, these repeated attempts will silently fail, i.e.
   * multiple instances of a call listener are not added and no exception will
   * be thrown.
   * <p>
   * When a call listener is added to an Terminal with this method, the
   * following behavior is exhibited by the implementation.
   * <p>
   * <OL>
   * <LI>It is immediately associated with any existing calls at the Terminal
   * and a snapshot of those calls are reported to the call listener. For
   * each of these calls, the listener is reported via
   * <CODE>Call.getTerminalListeners()</CODE>.
   * <LI>It is associated with all future calls which come to this Terminal.
   * For each new call, the listener is reported via
   * <CODE>Call.getTerminalListeners().</CODE>
   * </OL>
   * <p>
   * Note that the definition of the term ".. when a call is at a Terminal"
   * means that the Call contains a Connection which contains a
   * TerminalConnection with this Terminal as its Terminal.
   *
   * <H5>Call Listener Lifetime</H5>
   *
   * For all call listeners which are present on Calls because of this method,
   * the following behavior is exhibited with respect to the lifetime of the
   * call.
   * <p>
   * <OL>
   * <LI>The call listener receives events until the Call is no longer at this
   * Terminal. In this case, the call listener will be re-applied to the Call
   * if the Call returns to this Terminal at some point in the future.
   * <LI>The call listener is removed with <CODE>Call.removeTerminalListener()</CODE>.
   * Note that this only affects the instance of the call listener for that
   * particular call. If the Call subsequently leaves and returns to the
   * Terminal, the listener will be re-applied.
   * <LI>The Call can no longer be monitored by the implementation.
   * <LI>The Call moves into the <CODE>Call.INVALID</CODE> state.
   * </OL>
   * <p>
   * When the CallListener leaves the Call because of one of the reasons above,
   * it receives a CALL_EVENT_TRANSMISSION_ENDED as its final event.
   * <p>
   * <H5>Call Listener on Multiple Addresses and Terminals</H5>
   * It is possible for an application to add CallListeners to more than one
   * Address and Terminal (using <CODE>Address.addCallListener()</CODE> and
   * <CODE>Terminal.addCallListener()</CODE>, respectively). The rules outlined
   * above still apply, with the following additions:
   * <p>
   * <OL>
   * <LI>A CallListener is not added to a Call more than once, even if it
   * has been added to more than one Address/Terminal which are present on the
   * Call.
   * <LI>The CallListener leaves the call only if ALL of the Addresses and
   * Terminals on which the Call Listener was added leave the Call. If one
   * of those Addresses/Terminals becomes part of the Call again, the call
   * listener is re-applied to the Call.
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
   * <LI>listener is an element of Call.getTerminalListeners() for each Call associated
   * with the Connections from this.getConnections().
   * <LI>An array of snapshot events are reported to the listener for existing
   * calls associated with this Terminal.
   * </OL>
   * @see javax.telephony.Call
   * @param listener The listener being added.
   * @exception MethodNotSupportedException The listener cannot be added at this time
   * @exception ResourceUnavailableException The resource limit for the
   * numbers of listeners has been exceeded.
   */
  public void addCallListener(CallListener listener)
    throws ResourceUnavailableException,MethodNotSupportedException;


  /**
   * Returns a list of all CallListeners associated with this Terminal
   * object. That is, it returns a list of CallListener objects which have
   * been added via the <CODE>Terminal.addCallListener()</CODE> method. If
   * there are no Call listeners associated with this Terminal object, this
   * method returns null.
   * <p>
   * <B>Post-conditions:</B>
   * <OL>
   * <LI>Let CallListener[] obs = this.getCallListeners()
   * <LI>obs == null or obs.length >= 1
   * </OL>
   * @return An array of CallListener objects associated with this
   * Terminal.
   */
  public CallListener[] getCallListeners();


  /**
   * Removes the given CallListener from the Terminal. In other words, it
   * removes a CallListener which was added via the
   * <CODE>Terminal.addCallListener()</CODE> method. If successful, the
   * listener will no longer be added to new Calls which are presented to this
   * Terminal, however it does not affect CallListeners which have already been
   * added at a Call.
   * <p>
   * Also, if the CallListener is not part of the Terminal, then this method
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
