// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
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
Class LocalOptVal

java.lang.Object
  |
  +--jain.protocol.ip.mgcp.message.parms.CapabilityValue
        |
        +--jain.protocol.ip.mgcp.message.parms.LocalOptVal

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class LocalOptVal
   extends CapabilityValue
   implements java.io.Serializable

   This class represents an endpoint capability expressed as a Local
   Option Value.

   See Also: 
          Serialized Form
     _________________________________________________________________

   Fields inherited from class
   jain.protocol.ip.mgcp.message.parms.CapabilityValue
   LOCAL_OPTION_VALUE, SUPPORTED_MODES, SUPPORTED_PACKAGES



   Constructor Summary
   LocalOptVal(LocalOptionValue localOptionValue)
             Constructs a new LocalOptVal object that encapsulates a
   specific Local Option Value object.



   Method Summary
    LocalOptionValue getLocalOptionValue()
             Returns the LocalOptionValue that is encapsulated by this
   CapabilityValue object.



   Methods inherited from class
   jain.protocol.ip.mgcp.message.parms.CapabilityValue
   getCapabilityValueType, getSupportedModes, getSupportedPackageNames



   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll,
   toString, wait, wait, wait



   Constructor Detail

  LocalOptVal

public LocalOptVal(LocalOptionValue localOptionValue)

          Constructs a new LocalOptVal object that encapsulates a
          specific Local Option Value object.

   Method Detail

  getLocalOptionValue

public LocalOptionValue getLocalOptionValue()

          Returns the LocalOptionValue that is encapsulated by this
          CapabilityValue object.

        Overrides:
                getLocalOptionValue in class CapabilityValue

          Following copied from class:
          jain.protocol.ip.mgcp.message.parms.CapabilityValue

        Throws:
                java.lang.NoSuchFieldException - Thrown if this
                CapabilityValue object does not represent a Local Option
                Value.
     _________________________________________________________________

   Overview  Package   Class  Tree  Index  Help
   JAIN^TM MGCP API (Final Release) - Version 1.0
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:
   FIELD | CONSTR | METHOD
     _________________________________________________________________

                  Copyright © 2000 Sun Microsystems, Inc.
*/
