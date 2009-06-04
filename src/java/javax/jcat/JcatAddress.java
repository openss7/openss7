/* ***************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

package javax.jcat;

import javax.csapi.cc.jcc.*;

/**
    Like a JccAddress, a JcatAddress represents a user's directory
    number.  This directory number may be based on an E.164 addressing
    scheme or some other addressing scheme. For an IP-based network, the
    address might represent an IP address or a specific string (e.g.,
    "758-1000@telcordia.com"), as indicated by JccAddress.getType(). At
    a minimum an address is unique within a Call Agent's local domain.
    <p>

    A JcatAddress object has a string name which corresponds to the
    directory number. This name is first assigned when the Address
    object is created and does not change throughout the lifetime of the
    object.  The operation JccAddress.getName() returns the name of the
    JcatAddress object. <p>

    <h5>JcatAddress and JcatTerminal Objects</h5>
    A JcatAddress is also associated with a JcatTerminal. JcatAddress
    and JcatTerminal objects exist in a many-to-many relationship. A
    JcatAddress object may have zero or more JcatTerminals associated
    with it. Each JcatTerminal associated with a JcatAddress must
    reflect its association with the JcatAddress. Since the
    implementation creates JcatAddress (and JcatTerminal) objects, it is
    responsible for ensuring the correctness of these relationships. The
    JcatTerminals associated with a JcatAddress are given by the
    getTerminals() operation. <p>

    An association between a JcatAddress and a JcatTerminal indicates
    that the JcatTerminal is addressable using that JcatAddress. In many
    instances, a telephone set (represented by a JcatTerminal object)
    has only one directory number (represented by a JcatAddress object)
    associated with it. In more complex configurations, telephone sets
    may have several directory numbers associated with them. Likewise, a
    directory number may appear on more than one telephone set. <p>

    <h5>JcatAddress and JcatCall Objects</h5>
    JcatAddress objects represent the logical endpoints of a call. A
    logical view of a call views the call as originating from one
    JcatAddress endpoint and terminating at another JcatAddress
    endpoint. <p>

    JcatAddress objects are related to JcatCall objects via the
    JcatConnection object; this is the same as in case of the
    JccConnection objects. The JcatConnection object has a state which
    describes the current relationship between the JcatCall and the
    JcatAddress. Each JcatAddress object may be part of more than one
    call, and in each case, is represented by a separate JcatConnection
    object. The getConnections() operation returns all connection
    objects currently associated with the address. <p>

    A JcatAddress is associated with a JcatCall until the JcatConnection
    moves into the JccConnection.DISCONNECTED state. At that time, the
    JcatConnection is no longer reported via the getConnections()
    operation. This behavior is similar to that for JccConnection
    objects. <p>

    <h5>Address Listeners and Events</h5>
    All changes in a JcatAddress object are reported via the
    JcatAddressListener interface. Applications instantiate an object
    which implements this interface and begins this delivery of events
    to this object using the
    addAddressListener(JcatAddressListener,EventFilter) operation.
    Applications receive events on a listener until the listener is
    removed via the removeAddressListener(JcatAddressListener) operation
    or until the JcatAddress is not longer observable. In these
    instances, each JcatAddressListener instance receives a
    JcatAddressEvent.ADDRESS_EVENT_TRANSMISSION_ENDED as its final
    event. <p>

    @since 1.0
    @see JcatTerminal
    @see JcatCall
    @see JcatAddressListener
    @author Monavacon Limited
    @version 1.2.2
  */
public interface JcatAddress extends JccAddress {
    /**
      * Adds a listener to the address. The address Listener can be used
      * to listen to events such as registration of terminals associated
      * with this JcatAddress.  The address object will report events to
      * this address listener object for the lifetime of the Address
      * object, until the listener is removed with the
      * removeAddressListener(JcatAddressListener) operation or until
      * the address is no longer observable. In these instances, a
      * JcatAddressEvent.ADDRESS_EVENT_TRANSMISSION_ENDED is delivered
      * to the listener as its final event. The listener will receive no
      * events after ADDRESS_EVENT_TRANSMISSION_ENDED unless the
      * listener is explicitly re-added via this operation. <p>
      *
      * If an application attempts to add an instance of a listener
      * already present on this address, this attempt will silently
      * fail.
      *
      * @param addrlistener  JcatAddressListener which will receive
      * state changes in this JcatAddress.
      *
      * @param eventFilter  EventFilter determines if the
      * JcatAddressEvent.TERMINAL_REGISTERED is to be delivered to the
      * specified listener.
      */
    public void addAddressListener(JcatAddressListener addrlistener, EventFilter eventFilter);
    /**
      * This method is used to register a JcatTerminal with a
      * JcatAddress
      *
      * @param term  This parameter contains the terminal which is to be
      * registered with this address.
      *
      * @exception InvalidPartyException  The specified terminal
      * provided is not valid.
      *
      * @exception MethodNotSupportedException  This operation is not
      * supported by the implementation.
      *
      * @exception PrivilegeViolationException  The application does not
      * have the proper authority to invoke this operation.
      */
    public void registerTerminal(JcatTerminal term)
        throws InvalidPartyException, MethodNotSupportedException, PrivilegeViolationException;
    /**
      * This method is used to deregister a JcatTerminal with a
      * JcatAddress
      *
      * @param term  This parameter contains the terminal which is to be
      * deregistered with this address.
      *
      * @exception InvalidPartyException  The specified terminal
      * provided is not valid.
      *
      * @exception MethodNotSupportedException  This operation is not
      * supported by the implementation.
      *
      * @exception PrivilegeViolationException  The application does not
      * have the proper authority to invoke this operation.
      */
    public void deregisterTerminal(JcatTerminal term)
        throws InvalidPartyException, MethodNotSupportedException, PrivilegeViolationException;
    /**
      * Returns a set of JcatTerminals associated with this JcatAddress
      * object.
      *
      * @return set of JcatTerminals associated with this JcatAddress
      * object.
      */
    public java.util.Set getTerminals();
    /**
      * Removes the given listener from the JcatAddress. If successful,
      * the listener will no longer receive events generated by this
      * Address object. As its final event, the Address Listener
      * receives is an JcatAddressEvent.ADDRESS_EVENT_TRANSMISSION_ENDED
      * event. If a listener is not part of the address, then this
      * operation fails silently.
      *
      * @param addrlistener  JcatAddressListener which was registered to
      * receive state changes in this JcatAddress.
      */
    public void removeAddressListener(JcatAddressListener addrlistener);
    /**
      * Returns all the connection objects associated with this address.
      *
      * @return a set of connections associated with this address.
      */
    public java.util.Set getConnections();
    /**
      * Displays the specified string on the terminal.
      *
      * @param text  the string to be displayed
      *
      * @param displayAllowed  is displaying the text is allowed
      *
      * @exception InvalidArgumentException  This exception is thrown if
      * one of the parameter is invalid, e.g. typically if the text to
      * be displayed cannot be displayed due to its length or because of
      * invalid characters.
      */
    public void setDisplayText(java.lang.String text, boolean displayAllowed)
        throws InvalidArgumentException;
    /**
      * Gets the text that can be displayed on the terminal.
      *
      * @return the string to be displayed if getDisplayAllowed()
      * returns true
      */
    public java.lang.String getDisplayText();
    /**
      * Indication whether getDisplayText() is suppossed to be
      * displayed.
      *
      * @return indication whether getDisplayText() is suppossed to be
      * displayed
      */
    public boolean getDisplayAllowed();
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
