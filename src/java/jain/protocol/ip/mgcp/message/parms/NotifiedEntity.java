// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: NotifiedEntity.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:08 $ <p>
 
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
Class NotifiedEntity

java.lang.Object
  |
  +--jain.protocol.ip.mgcp.message.parms.NotifiedEntity

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public final class NotifiedEntity
   extends java.lang.Object
   implements java.io.Serializable

   Specifies where notifications should be sent.

   See Also: 
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   NotifiedEntity(java.lang.String domainName)
             Constructs a new Notified Entity object.
   NotifiedEntity(java.lang.String localName,
   java.lang.String domainName)
             Constructs a new Notified Entity object.
   NotifiedEntity(java.lang.String localName,
   java.lang.String domainName, int portNumber)
             Constructs a new Notified Entity object.



   Method Summary
    java.lang.String getDomainName()
             Gets the Domain Name of the Notified Entity.
    java.lang.String getLocalName()
             Gets the Local Name of the Notified Entity.
    int getPortNumber()
             Gets the Port Number of the Notified Entity.
    java.lang.String toString()
             Gets the complete Notified Entity string.



   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait,
   wait, wait



   Constructor Detail

  NotifiedEntity

public NotifiedEntity(java.lang.String domainName)
               throws java.lang.IllegalArgumentException

          Constructs a new Notified Entity object. Local Name is assumed
          to be null and port number is implicitly set to the default
          value of 2427.

        Parameters:
                domainName - A string that designates the Domain Name of
                the notified entity. Must be non-null and must conform to
                RFC 821 syntax.

        Throws:
                java.lang.IllegalArgumentException - Thrown if the Domain
                Name is null or does not conform to RFC 821 syntax.
     _________________________________________________________________

  NotifiedEntity

public NotifiedEntity(java.lang.String localName,
                      java.lang.String domainName)
               throws java.lang.IllegalArgumentException

          Constructs a new Notified Entity object.

        Parameters:
                localName - A string that designates the Local Name of
                the notified entity. Must be non-null and must conform to
                RFC 2705 syntax for a Local Name.
                domainName - A string that designates the Domain Name of
                the notified entity. Must be non-null and must conform to
                RFC 821 syntax.

        Throws:
                java.lang.IllegalArgumentException - Thrown if either
                parameter is null or if either parameter does not conform
                to the syntax as defined in the appropriate RFC.
     _________________________________________________________________

  NotifiedEntity

public NotifiedEntity(java.lang.String localName,
                      java.lang.String domainName,
                      int portNumber)
               throws java.lang.IllegalArgumentException

          Constructs a new Notified Entity object.

        Parameters:
                localName - A string that designates the Local Name of
                the notified entity. Must be non-null and must conform to
                RFC 2705 syntax for a Local Name.
                domainName - A string that designates the Domain Name of
                the notified entity. Must be non-null and must conform to
                RFC 821 syntax.
                portNumber - The port number on the notified entity. Must
                be a positive integer from 1 through 5 digits in length.

        Throws:
                java.lang.IllegalArgumentException - Thrown if either
                parameter is null or if either parameter does not conform
                to the syntax as defined in the appropriate RFC or if the
                port number is not a positive integer from 1 through 5
                digits in length.

   Method Detail

  getDomainName

public java.lang.String getDomainName()

          Gets the Domain Name of the Notified Entity.

        Returns:
                A string that designates the Domain Name of the notified
                entity.
     _________________________________________________________________

  getLocalName

public java.lang.String getLocalName()

          Gets the Local Name of the Notified Entity.

        Returns:
                A string that designates the Local Name of the notified
                entity. If null, none was set.
     _________________________________________________________________

  getPortNumber

public int getPortNumber()

          Gets the Port Number of the Notified Entity.

        Returns:
                The Port Number of the Notified Entity.
     _________________________________________________________________

  toString

public java.lang.String toString()

          Gets the complete Notified Entity string.

        Overrides:
                toString in class java.lang.Object

        Returns:
                LocalName@DomainName:PortNumber
     _________________________________________________________________

   Overview  Package   Class  Tree  Index  Help
   JAIN^TM MGCP API (Final Release) - Version 1.0
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:
   FIELD | CONSTR | METHOD
     _________________________________________________________________

                  Copyright (C) 2000 Sun Microsystems, Inc.
*/
