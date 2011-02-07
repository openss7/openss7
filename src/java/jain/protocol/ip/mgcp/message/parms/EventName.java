// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: EventName.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:08 $ <p>
 
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
 
 Last Modified $Date: 2011-02-07 07:27:08 $ by $Author: brian $
 */

package jain.protocol.ip.mgcp.message.parms;

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview  Package   Class  Tree  Index  Help
   JAIN^TM MGCP API (Final Release) - Version 1.0
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:
   FIELD | CONSTR | METHOD
     _________________________________________________________________

jain.protocol.ip.mgcp.message.parms
Class EventName

java.lang.Object
  |
  +--jain.protocol.ip.mgcp.message.parms.EventName

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public final class EventName
   extends java.lang.Object
   implements java.io.Serializable

   Specifies an event name, as defined in RFC 2705.

   See Also: 
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   EventName(PackageName packageName, MgcpEvent event)
             Constructs a new EventName in which the package name and the
   event Identifier are specified.
   EventName(PackageName packageName, MgcpEvent event,
   ConnectionIdentifier connectionIdentifier)
             Constructs a new EventName in which the package name, the
   event Identifier, and the connection Identifier are all specified.



   Method Summary
    ConnectionIdentifier getConnectionIdentifier()
             Gets the connection Identifier.
    MgcpEvent getEventIdentifier()
             Gets the event Identifier.
    PackageName getPackageName()
             Gets the package name.
    java.lang.String toString()
             Gets the entire event name as a java.lang.String.



   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait,
   wait, wait



   Constructor Detail

  EventName

public EventName(PackageName packageName,
                 MgcpEvent event)

          Constructs a new EventName in which the package name and the
          event Identifier are specified. Note that each of these
          parameters can be "wildcarded" using the appropriate wildcard
          object reference (e.g., use
          jain.protocol.ip.mgcp.pkg.AllPackages as the package name to
          signify that the event is applicable to all packages.

        Parameters:
                packageName - A reference to an object of class
                jain.protocol.ip.mgcp.pkg.PackageName.
                event - A reference to an object that implements the
                jain.protocol.ip.mgcp.pkg.MgcpEvent interface.

        Throws:
                java.lang.IllegalArgumentException - Thrown if the
                package name or the event parameter is null-valued.
     _________________________________________________________________

  EventName

public EventName(PackageName packageName,
                 MgcpEvent event,
                 ConnectionIdentifier connectionIdentifier)
          throws java.lang.IllegalArgumentException

          Constructs a new EventName in which the package name, the event
          Identifier, and the connection Identifier are all specified.
          Note that each of these parameters can be "wildcarded" using
          the appropriate wildcard object reference (e.g., use
          jain.protocol.ip.mgcp.message.parms.AllConnections as the
          package name to signify that the event is applicable to all
          connections.

        Parameters:
                packageName - A reference to an object of class
                jain.protocol.ip.mgcp.pkg.PackageName.
                event - A reference to an object that implements the
                jain.protocol.ip.mgcp.pkg.MgcpEvent interface.
                connectionIdentifier - A reference to an object of class
                jain.protocol.ip.mgcp.message.parms.ConnectionIdentifier.

        Throws:
                java.lang.IllegalArgumentException - Thrown if the
                package name or the event parameter is null-valued.

   Method Detail

  getConnectionIdentifier

public ConnectionIdentifier getConnectionIdentifier()

          Gets the connection Identifier.

        Returns:
                The connection Identifier.
     _________________________________________________________________

  getEventIdentifier

public MgcpEvent getEventIdentifier()

          Gets the event Identifier.

        Returns:
                The event Identifier.
     _________________________________________________________________

  getPackageName

public PackageName getPackageName()

          Gets the package name.

        Returns:
                The package name.
     _________________________________________________________________

  toString

public java.lang.String toString()

          Gets the entire event name as a java.lang.String.

        Overrides:
                toString in class java.lang.Object

        Returns:
                packageName/event@connectionIdentifier
     _________________________________________________________________

   Overview  Package   Class  Tree  Index  Help
   JAIN^TM MGCP API (Final Release) - Version 1.0
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:
   FIELD | CONSTR | METHOD
     _________________________________________________________________

                  Copyright (C) 2000 Sun Microsystems, Inc.
*/
