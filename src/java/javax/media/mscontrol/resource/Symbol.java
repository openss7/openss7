/*
 @(#) $RCSfile$ $Name$($Revision$) $Date$ <p>
 
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
 
 Last Modified $Date$ by $Author$
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

     _________________________________________________________________

   Overview  Package   Class  Use  Tree  Deprecated  Index  Help
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES     All Classes
   SUMMARY: NESTED | FIELD | CONSTR | METHOD
   DETAIL: FIELD | CONSTR | METHOD
     _________________________________________________________________

javax.media.mscontrol.resource
Interface Symbol

   All Known Subinterfaces:
          ConfigSymbol<C>
     _________________________________________________________________

public interface Symbol

   Symbols are opaque, immutable, nominal tokens. They are used in
   applications only for assignment and equality/identity testing.

   Symbols allow extensibilty by vendors, interoperability across
   languages, and other benefits.

   Symbols are defined as "public final static" members of various
   interfaces. Symbol constants are used to identify Events, Event
   Qualifiers, Resource Parameters, RTC triggers Conditions, RTC Actions,
   and Keys or Values in Parameters maps.

    Symbol name convention:

   A particular Symbol (and its associated value) may be used in multiple
   contexts. Each expected usage of the Symbol is given a distinct name,
   composed by a diffentiating prefix that indicates the usage, and an
   item name that identifies the actual value.

   The convention for Symbol name prefixes is as follows:

   With prefix: the Symbol is used as: for example:
             a_ Attribute                 a_LocalState
             e_ Error                     e_Disconnected
            ev_ Event                    ev_SignalDetected
             p_ Parameter                 p_EnabledEvents
             q_ Qualifier                 q_Disconnected
          rtca_ RTC Action             rtca_Stop
          rtcc_ RTC Condition          rtcc_SignalDetected
             v_ Parameter value           v_Stop
             c_ MediaConfig               c_IVR

   Each (non-Error) Symbol typically appears as a final static field in
   each interface or class to which it is relevant. This is often
   accomplished by defining the Symbol in a "Constants" interface that is
   inherited by the classes or interfaces in which the Symbol is
   relevant. For example, each Resource class has a ResourceConstants
   interface that is inherited by the Resource interface and the
   associated ResourceEvent interface.

   Note: Symbols used to identify errors, (the return value from
   ResourceEvent.getError()) are used solely for errors and do not appear
   in other contexts. Because error Symbols are numerous and may appear
   in many interfaces, the error Symbols are defined in interface Error
   and typically are not renamed or inherited in other classes.
   Therefore, Error Symbols are accessed as Error.e_ItemName.

   Note: Pre-defined Symbol constants with the same internal value
   generally refer to the same Object, but this cannot be relied upon. To
   compare two Symbols for numerical/logical equivalence, the equals
   method must be used.

   Note: The other methods on Symbol are useful when debugging, and may
   be used by implementors, but are not usually needed (or useful) during
   production. Symbols are defined by name in final static fields, by the
   classes that assign some meaning to a Symbol. Applications do not need
   to create Symbols at runtime.
     _________________________________________________________________

   Method Summary
    boolean equals(java.lang.Object other)
             Compares the internal value of two Symbols.
    int hashCode()
             Returns a hashcode for this Symbol.
    java.lang.String toString()
             Decode and print as many of the Symbol components as are
   registered.



   Method Detail

  equals

boolean equals(java.lang.Object other)

          Compares the internal value of two Symbols.

          The result is TRUE if and only if the argument is an instance
          of Symbol and contains the same internal value as this Symbol.

          Note: The "==" operator just compares the identity (object
          handle) of two Symbols.

        Overrides:
                equals in class java.lang.Object

        Parameters:
                other - a Symbol to compare to this Symbol.

        Returns:
                TRUE iff the Symbols have the same value.
     _________________________________________________________________

  hashCode

int hashCode()

          Returns a hashcode for this Symbol.

        Overrides:
                hashCode in class java.lang.Object

        Returns:
                a hashcode value for this Symbol.
     _________________________________________________________________

  toString

java.lang.String toString()

          Decode and print as many of the Symbol components as are
          registered. Unregistered segments are printed in hex.

        Overrides:
                toString in class java.lang.Object
     _________________________________________________________________

   Overview  Package   Class  Use  Tree  Deprecated  Index  Help
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES     All Classes
   SUMMARY: NESTED | FIELD | CONSTR | METHOD
   DETAIL: FIELD | CONSTR | METHOD
     _________________________________________________________________

   Copyright © 2007-2008 Oracle and/or its affiliates. © Copyright
   2007-2008 Hewlett-Packard Development Company, L.P. All rights
   reserved. Use is subject to license terms.
*/
package javax.jain.media.mscontrol.resource;
