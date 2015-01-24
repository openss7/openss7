// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/javax/megaco/message/Termination.java <p>
 
 Copyright &copy; 2008-2015  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
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
 <a href="http://www.openss7.com/">http://www.openss7.com/</a>
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.message
Class Termination

java.lang.Object
  |
  +--javax.megaco.message.Termination

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class Termination
   extends java.lang.Object

   The Jain MEGACO termination class is used to get the detailed
   information about the termination and the wildcard data for the
   termination.

   See Also:
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   Termination()
             Constructs a Jain Megaco termination class.

   Method Summary
    void setTermName(java.lang.String termName)
       throws javax.megaco.InvalidArgumentException

             Sets the termination name as a string value for text
   encoding.
    void setTermName(byte[] termName)
       throws javax.megaco.InvalidArgumentException

             Sets the termination name as a octet value for binary
   encoding.
    void setTermType(TermType termType)
       throws javax.megaco.InvalidArgumentException

              Sets the termination type specified in the termination
   name. This is valid for both binary encoding and text encoding.
    void setTermWildCard(byte[] wildCard)
       throws javax.megaco.InvalidArgumentException

             Sets the wildcarded octets for the termination name for
   binary encoding of termination name.
    java.lang.String getTextTermName()
             Gets the termination name assuming the termination name is
   received in text format and returns a string value.
    byte[]  getBinTermName()
             Gets the termination name assuming the termination name is
   received in binary format and returns a termination name in octet
   format.
    int  getTermType()
              Gets the termination type for the termination name in the
   command. Gets the integer corresponding to the termination type.
    byte[]  getTermWildCard()
             Gets the wild card bytes for termination name assuming the
   termination name is received in binary format and returns a wildcard
   octet.
    java.lang.String toString()
             Overrides java.lang.Object.toString().

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  Termination

public Termination()

          Constructs a Jain Megaco Termination Object.

   Method Detail

  setTermName

   public void setTermName(java.lang.String termName)
               throws javax.megaco.InvalidArgumentException

          Sets the termination name as a string value for text encoding.


        Parameters:
                termName - An string format of the termination name.

        Throws:
                InvalidArgumentException If the termination name is set
                such that it violates the protocol.
     _________________________________________________________________

  setTermName

   public void setTermName(byte[] termName)
               throws javax.megaco.InvalidArgumentException

          Sets the termination name as a byte stream for binary encoding.


        Parameters:
                termName - A byte stream of the termination name.

        Throws:
                InvalidArgumentException If the termination name is set
                such that it violates the syntax defined by the protocol.
     _________________________________________________________________

  setTermType

   public void setTermType(TermType termType)
       throws javax.megaco.InvalidArgumentException

          Sets a reference object corresponding to the termination type
          for the termination name set.


        Parameters:
                termType - A reference to an object specifying the type
                of the termination name. These could be one of normal,
                choose, root or wildcarded.

             
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Termination Type passed to this method
                is NULL.
     _________________________________________________________________

  setTermWildCard

   public void setTermWildCard(byte[] termWC)
           throws javax.megaco.InvalidArgumentException

          Sets the termination name wild card as a byte stream for binary
          encoding.


        Parameters:
                termName - A byte stream of the termination name
                wildarding information.

        Throws:
                InvalidArgumentException If the termination name is set
                such that it violates the protocol.
     _________________________________________________________________

  getTextTermName

   public java.lang.String  getTextTermName()

          Gets the termination name assuming the termination name is
          string encoded.


        Returns:
                Returns a string for the text termination identifier.
     _________________________________________________________________

  getBinTermName

public byte[]  getBinTermName()

          Gets the termination name assuming the termination name is
          octect byte encoded for binary encoding.


        Returns:
                Returns a binary encoded termination name in byte format.
     _________________________________________________________________

  getTermType

public int  getTermType()

          Gets the termination type as an integer value.
          See javax.megaco.message.TermType for the definition of the
          constants for the termination type.


        Returns:
                Returns a termination type as an integer value.
     _________________________________________________________________

  getTermWildCard

public byte[]  getTermWildCard()

          Gets the wild card bytes for termination name assuming the
          termination name is received in binary format and returns a
          wildcard octet.


        Returns:
                Returns a binary encoded termination name wild card in
                byte format.
     _________________________________________________________________

  toString

public java.lang.String toString()

          Overrides java.lang.Object.toString().


        Overrides:
                toString in class java.lang.Object
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

                  Copyright (C) 2001 Hughes Software Systems
       HUGHES SOFTWARE SYSTEMS and JAIN JSPA SIGNATORIES PROPRIETARY
        This document contains proprietary information that shall be
     distributed, routed or made available only within Hughes Software
       Systems and JAIN JSPA Signatory Companies, except with written
                   permission of Hughes Software Systems
             _________________________________________________

   22 December 2003


    If you have any comments or queries, please mail them to
    Jmegaco_hss@hss.hns.com
*/
