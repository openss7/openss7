/*
 @(#) $RCSfile: JcatTerminal.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:10 $ <p>
 
 Copyright &copy; 2008-2009  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
 Copyright &copy; 2001-2008  OpenSS7 Corporation <a href="http://www.openss7.com/">&lt;http://www.openss7.com/&gt;</a>. <br>
 Copyright &copy; 1997-2001  Brian F. G. Bidulock <a href="mailto:bidulock@openss7.org">&lt;bidulock@openss7.org&gt;</a>. <p>
 
 All Rights Reserved. <p>
 
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license. <p>
 
 This program is distributed in the hope that it will be useful, but <b>WITHOUT
 ANY WARRANTY</b>; without even the implied warranty of <b>MERCHANTABILITY</b>
 or <b>FITNESS FOR A PARTICULAR PURPOSE</b>.  See the GNU Affero General Public
 License for more details. <p>
 
 You should have received a copy of the GNU Affero General Public License along
 with this program.  If not, see
 <a href="http://www.gnu.org/licenses/">&lt;http://www.gnu.org/licenses/&gt</a>,
 or write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA. <p>
 
 <em>U.S. GOVERNMENT RESTRICTED RIGHTS</em>.  If you are licensing this
 Software on behalf of the U.S. Government ("Government"), the following
 provisions apply to you.  If the Software is supplied by the Department of
 Defense ("DoD"), it is classified as "Commercial Computer Software" under
 paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
 Regulations ("DFARS") (or any successor regulations) and the Government is
 acquiring only the license rights granted herein (the license rights
 customarily provided to non-Government users).  If the Software is supplied to
 any unit or agency of the Government other than DoD, it is classified as
 "Restricted Computer Software" and the Government's rights in the Software are
 defined in paragraph 52.227-19 of the Federal Acquisition Regulations ("FAR")
 (or any successor regulations) or, in the cases of NASA, in paragraph
 18.52.227-86 of the NASA Supplement to the FAR (or any successor regulations). <p>
 
 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See
 <a href="http://www.openss7.com/">http://www.openss7.com/</a> <p>
 
 Last Modified $Date: 2009-06-21 11:35:10 $ by $Author: brian $
 */

package javax.jcat;

import javax.csapi.cc.jcc.*; 

/**
  * A JcatTerminal represents a physical endpoint connected to the
  * softswitch domain. For example, telephone set, computer workstations
  * and hand-held devices are modeled as JcatTerminal objects if they
  * act as physical endpoints in a communications network. A
  * JcatTerminal object has a string name that is unique for all
  * JcatTerminal objects.  The JcatTerminal does not attempt to
  * interpret this string in any way.  This name is first assigned when
  * the JcatTerminal is created and does not change throughout the
  * lifetime of the object. The method getName() returns the name of the
  * JcatTerminal object. The name of the JcatTerminal may not have any
  * real-world interpretation.  Typically, JcatTerminals are chosen from
  * a list of JcatTerminal objects obtained from an JcatAddress object.
  * <p>
  *
  * JcatTerminal objects may be classified into two categories: local
  * and remote. Local JcatTerminal objects are those terminals which are
  * part of the JcatProvider's local domain. These JcatTerminal objects
  * are created by the implementation of the JcatProvider object when it
  * is first instantiated. Remote JcatTerminal objects are those outside
  * of the JcatProvider's domain which the JcatProvider learns about
  * during its lifetime through various happenings (e.g., an incoming
  * call from a currently unknown address). Note that applications never
  * explicitly create new JcatTerminal objects through the JCAT API. <p>
  *
  * <h5>Address and Terminal Objects</h5>
  * JcatAddress and JcatTerminal objects exist in a many-to-many
  * relationship. A JcatAddress object may have zero or more
  * JcatTerminal objects associated with it. For each JcatTerminal
  * associated with a JcatAddress, that JcatTerminal must also reflect
  * its association with the JcatAddress. Since the implementation
  * creates JcatAddress (and JcatTerminal) objects, it is responsible
  * for insuring the correctness of these relationships. The
  * JcatTerminal objects associated with a JcatAddress is given by the
  * JcatAddress.getTerminals() method. An association between a
  * JcatAddress and a JcatTerminal object indicates that the
  * JcatTerminal contains the JcatAddress object as one of its telephone
  * number addresses. In many instances, a telephone set (represented by
  * a JcatTerminal object) has only one directory number (represented by
  * a JcatAddress object) associated with it. In more complex
  * configurations, telephone sets may have several directory numbers
  * associated with them. For example, some of the current PDAs may
  * exhibit this configuration. <p>
  *
  * <h5>Terminals and Call Objects</h5>
  * JcatTerminal objects represent the physical endpoints of a call.
  * With respect to a single JcatAddress endpoint on a JcatCall,
  * multiple physical JcatTerminal endpoints may exist. JcatTerminal
  * objects are related to JcatCall objects via the
  * JcatTerminalConnection object.  JcatTerminalConnection objects are
  * associated with JcatCall indirectly via JcatConnection objects. <p>
  *
  * A JcatTerminal may be associated with a JcatCall only if one of its
  * JcatAddress objects is associated with the JcatCall. The
  * JcatTerminalConnection object has a state which describes the
  * current relationship between the JcatConnection and the
  * JcatTerminal. Each JcatTerminal object may be part of more than one
  * call, and in each case, is represented by a separate
  * JcatTerminalConnection object. The getTerminalConnections() method
  * returns all JcatTerminalConnection objects currently associated with
  * the Terminal. <p>
  *
  * A JcatTerminal object is associated with a JcatConnection until the
  * JcatTerminalConnection moves into the JcatTerminalConnection.DROPPED
  * state. At that time, the JcatTerminalConnection is no longer
  * reported via the JcatTerminal.getTerminalConnections() method.
  * Therefore, the JcatTerminal.getTerminalConnections() method never
  * reports a JcatTerminalConnection in the
  * JcatTerminalConnection.DROPPED state. <p>
  *
  * <h5>Terminal Listeners and Events</h5>
  * All changes in a JcatTerminal object are reported via the
  * JcatTerminalListener interface. Applications instantiate an object
  * which implements this interface and begins this delivery of events
  * to this object using the addTerminalListener(JcatTerminalListener)
  * method. Applications receive events on a listener until the
  * listener is removed via the
  * removeTerminalListener(JcatTerminalListener) method or until the
  * JcatTerminal is no longer observable. In these instances, each
  * JcatTerminalListener instance receives a
  * JcatTerminalEvent.TERMINAL_EVENT_TRANSMISSION_ENDED as its final
  * event. At this point, we envisage use of this feature to be used
  * to report association and disassociation of JcatAddresses with a
  * JcatTerminal. <p>
  *
  * <h5>Call Listeners</h5>
  * At times, applications may want to monitor a particular JcatTerminal
  * for all JcatCalls which come to that JcatTerminal. For example, a
  * desktop telephone application is only interested in calls associated
  * with a particular agent terminal. To achieve this sort of
  * JcatTerminal-based JcatCall monitoring applications may add
  * JcatCallListeners to a JcatTerminal via the
  * addCallListener(JccCallListener) method. When a JcatCallListener is
  * added to a JcatTerminal, this listener instance is immediately added
  * to all JcatCall objects at this JcatTerminal and is added to all
  * calls which come to this JcatTerminal in the future. These listeners
  * remain on the call as long as the JcatTerminal is associated with
  * the call. <p>
  *
  * @since 1.0
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public interface JcatTerminal {
    /**
      * Adds a listener to a JcatCall when this JcatTerminal first
      * becomes part of that JcatCall. This method permits applications
      * to select a JcatTerminal object in which they are interested and
      * automatically have the implementation attach this listener to
      * all present and future calls which come to this JcatTerminal. If
      * an application attempts to add an instance of a call listener
      * already present on this JcatTerminal, it will silently fail.
      * When a call listener is added to a JcatTerminal with this
      * method, the following behavior is exhibited by the
      * implementation. <ol>
      *
      * <li>It is immediately associated with any existing calls at the
      *     JcatTerminal and a snapshot of those calls are reported to
      *     the call listener.
      *
      * <li>It is associated with all future calls which come to this
      *     JcatTerminal.  </ol>
      *
      * Note that the definition of the term "... JcatTerminal first
      * becomes part of that JcatCall ..." means that the JcatCall
      * contains a JcatConnection which contains a terminal connection
      * with this JcatTerminal as its JcatTerminal.
      *
      * <h5>Call Listener on Multiple Terminals</h5>
      * It is possible for an application to add call listeners to more
      * than one JcatTerminal (using addCallListener(JccCallListener)).
      * The rules outlined above still apply, with the following
      * additions: <ol>
      *
      * <li>A call listener is not added to a JcatCall more than once,
      *     even if it has been added to more than one JcatTerminal
      *     which are present on the JcatCall.
      *
      * <li>The call listener leaves the call only if ALL of the objects
      *     through which the call listener was added leave the call. If
      *     one of those objects becomes part of the JcatCall again, the
      *     call listener is re-applied to the JcatCall.  </ol>
      *
      * <h5>Call Listener Event level of Granularity</h5>
      * The application shall receive the event reports associated with
      * the methods which are defined in the Call Listener interface
      * that the application implements - either the JccCallListener,
      * JccConnectionListener or JcatConnectionListener interface. Note
      * that the JccCallListener, JccConnectionListener and
      * JcatConnectionListener form a hierarchy of interfaces, and so
      * provide successively increasing amounts of call information and
      * successively finer-grained event granularity.
      *
      * <h5>Call Listener Lifetime</h5>
      * For all call listeners which are present on calls because of
      * this method, the following behavior is exhibited with respect to
      * the lifetime of the call.  <ol>
      *
      * <li>The call listener receives events until the call is no
      *     longer at this JcatTerminal.
      *
      * <li>All object that have added the call listener have also
      *     (subsequently) removed the call listener.
      *
      * <li>The JcatCall can no longer be monitored by the
      *     implementation.
      *
      * <li>The JcatCall moves into the JccCall.INVALID state.  </ol>
      *
      * When the Call Listener leaves the JcatCall because of one of the
      * reasons above, it receives a
      * JccCallEvent.CALL_EVENT_TRANSMISSION_ENDED as its final event.
      *
      * @param listener  To be added to any JcatCalls on this JcatTerminal.
      * @exception MethodNotSupportedException
      * @exception ResourceUnavailableException
      */
    public void addCallListener(JccCallListener listener)
        throws MethodNotSupportedException, ResourceUnavailableException;
    /**
      * Adds a listener to the Terminal. The Terminal Listener reports
      * all JcatTerminal-related state changes as JcatTerminalEvents.
      * The terminal object will report events to this terminal listener
      * object for the lifetime of the terminal object, until the
      * listener is removed with the
      * removeTerminalListener(JcatTerminalListener) or until the
      * terminal is no longer observable. In these instances, a
      * JcatTerminalEvent.TERMINAL_EVENT_TRANSMISSION_ENDED is delivered
      * to the listener as its final event. The listener will receive no
      * events after TERMINAL_EVENT_TRANSMISSION_ENDED unless the
      * listener is explicitly re-added via the this method.  If an
      * application attempts to add an instance of a listener already
      * present on this terminal, this attempt will silently fail.
      */
    public void addTerminalListener(JcatTerminalListener listener);
    /**
      * Returns an array of JcatAddress objects associated with this
      * JcatTerminal object.
      *
      * @return a set of JcatAddresses associated with this
      * JcatTerminal.
      */
    public java.util.Set getAddresses();
    /**
      * Returns the name of the JcatTerminal. Each JcatTerminal
      * possesses a unique name.  This name is assigned by the
      * implementation and may or may not carry a real-world
      * interpretation.
      *
      * @return the name of the JcatTerminal.
      */
    public java.lang.String getName();
    /**
      * Returns the JcatProvider associated with this JcatTerminal.
      * This JcatProvider object is valid throughout the lifetime of the
      * JcatTerminal and does not change once the JcatTerminal is
      * created.
      *
      * @return JcatProvider associated with this JcatTerminal.
      */
    public JcatProvider getProvider();
    /**
      * Returns a set of JcatTerminalConnection objects associated with
      * this JcatTerminal.  Once a JcatTerminalConnection is added to a
      * JcatTerminal, the JcatTerminal maintains a reference until the
      * JcatTerminalConnection moves into the
      * JcatTerminalConnection.DROPPED state.
      *
      * @return a set of JcatTerminalConnection objects associated with
      * this JcatTerminal.
      */
    public java.util.Set getTerminalConnections();
    /**
      */
    public void removeCallListener(JccCallListener calllistener);
    /**
      * Removes the given listener from the Terminal. If successful, the
      * listener will no longer receive events generated by this
      * Terminal object. As its final event, the Terminal Listener
      * receives a JcatTerminalEvent.TERMINAL_EVENT_TRANSMISSION_ENDED.
      * If a listener is not part of the Terminal, then this method
      * fails silently.
      *
      * @param termlistener  listener to be removed from the
      * JcatTerminal.
      */
    public void removeTerminalListener(JcatTerminalListener termlistener);
    /**
      * Returns the dynamic capabilities for the instance of the
      * JcatTerminal object.  Dynamic capabilities tell the application
      * which actions are possible at the time this method is invoked
      * based upon the implementation's knowledge of its ability to
      * successfully perform the action. This determination may be based
      * upon argument passed to this method, the current state of the
      * call model, or some implementation-specific knowledge.  These
      * indications do not guarantee that a particular method will be
      * successful when invoked, however.
      */
    public JcatTerminalCapabilities getTerminalCapabilities();
    /**
      * This method is used to register a JcatAddress with the
      * JcatTerminal
      *
      * @param addr  This parameter contains the address which is to be
      * registered with this terminal. Successfully invoking this method
      * will lead to emmitting JcatAddressEvent.TERMINAL_REGISTERED and
      * JcatTerminalEvent.ADDRESS_REGISTERED.
      *
      * @exception InvalidPartyException  The specified address provided
      * is not valid.
      *
      * @exception MethodNotSupportedException  This method is not
      * supported by the implementation.
      *
      * @exception PrivilegeViolationException  The application does not
      * have the proper authority to invoke this method.
      */
    public void registerAddress(JcatAddress addr)
        throws InvalidPartyException, MethodNotSupportedException, PrivilegeViolationException;
    /**
      * This method is used to deregister a JcatAddress with the
      * JcatTerminal
      *
      * @param addr  This parameter contains the address which is to be
      * deregistered with this terminal. Successfully invoking this
      * method will lead to emmitting
      * JcatAddressEvent.TERMINAL_DEREGISTERED and
      * JcatTerminalEvent.ADDRESS_DEREGISTERED.
      *
      * @exception InvalidPartyException  The specified address provided
      * is not valid.
      *
      * @exception MethodNotSupportedException  This method is not
      * supported by the implementation.
      *
      * @exception PrivilegeViolationException  The application does not
      * have the proper authority to invoke this method.
      */
    public void deregisterAddress(JcatAddress addr)
        throws InvalidPartyException, MethodNotSupportedException, PrivilegeViolationException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
