// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: DigitMapDescriptor.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:17 $ <p>
 
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
 
 Last Modified $Date: 2011-02-07 07:27:17 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES NO FRAMES
     _________________________________________________________________

javax.megaco.message.descriptor
Class DigitMapDescriptor

java.lang.Object
    |
    +--javax.megaco.message.Descriptor
            |
            +--javax.megaco.message.descriptor.DigitMapDescriptor

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class DigitMapDescriptor
   extends Descriptor

   The class extends JAIN MEGACO Descriptor. This class describes the
   DigitMap descriptor.

   See Also:
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   DigitMapDescriptor()
              Constructs a DigitMap Descriptor object. This extends the
   Descriptor class. It defines the Digit Map descriptor of the Megaco.

   Method Summary
    java.lang.String getDigitMapName()
              Returns a string value corresponding to the digit map name.
   If the digit map name is not set, then this shall return a null value.
    void setDigitMapName(java.lang.String digitName)
           throws javax.megaco.InvalidArgumentException

              Sets the digit map name in the DigitMap descriptor.
    DigitMapValue getDigitMapValue()
              Returns a DigitMapValue object reference from the DigitMap
   descriptor.
   void setDigitMapValue(DigitMapValue digitValue)
           throws javax.megaco.InvalidArgumentException

              Sets the DigitMapValue object reference in the DigitMap
   descriptor.

                                                         java.lang.String

   getDigitMapValueStr()
              Returns a DigitMapValue java.lang.String object reference from the
   DigitMap descriptor.

                                                                     void

   setDigitMapValueStr(java.lang.String dmapValStr)
       throwsjavax.megaco.InvalidArgumentException

              Sets the DigitMapValue java.lang.String object reference in the
   DigitMap descriptor.

   Methods inherited from class javax.megaco.message.Descriptor
   getDescriptorId

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  DigitMapDescriptor

public DigitMapDescriptor()

          Constructs a DigitMap Descriptor.

   Method Detail

  getDigitMapName

public final java.lang.String getDigitMapName()

          This method returns the digit map name if set for the digit map
          descriptor.


        Returns:
                Returns a string value that identifies the digit map name
                for the DigitMap descriptor. If the digit map name is not
                set, then this shall return a null value.
     _________________________________________________________________

  setDigitMapName

public final void setDigitMapName(java.lang.String digitName)
            throws javax.megaco.InvalidArgumentException

          This method sets the digit map name if set for the digit map
          descriptor.


        Parameter:
                digitName - Sets a string value that identifies the digit
                map name for the DigitMap descriptor. If the digit map
                name is not set, then this shall return a null value.

        Throws:
                javax.megaco.InvalidArgumentException - Thrown if the
                string value does not satisfy the grammar for the digit
                map name.
     _________________________________________________________________

  getDescriptorId

public final int getDescriptorId()

          This method cannot be overridden by the derived class. This
          method returns that the descriptor identifier is of type
          descriptor DigitMap. This method overrides the corresponding
          method of the base class Descriptor.


        Returns:
                Returns an integer value that identifies this DigitMap
                object as the type of DigitMap descriptor. It returns
                the  value M_DIGIT_MAP_DESC for a DigitMap Descriptor.
     _________________________________________________________________

  getDigitMapValue

public final DigitMapValue getDigitMapValue()

          This method gets the DigitMap Value. The DigitMap value class
          contains information about the dial plan. This may also
          contains special symbols like wildcards and timer values to be
          used on application of the dial plan.


        Return:
                 Returns a DigitMapValue object.
     _________________________________________________________________

  setDigitMapValue

public final void setDigitMapValue(DigitMapValue digitValue)
            throws javax.megaco.InvalidArgumentException

          This method sets the DigitMap Value.


        Parameter:
                digitValue - The DigitMap value object refernce.

        Throws:
                javax.megaco.InvalidArgumentException - Thrown if the
                digit map value passed in the arguments contains invalid
                parameters.
     _________________________________________________________________

  getDigitMapValueStr

public final java.lang.String getDigitMapValueStr()

          This method gets the DigitMap Value in the string format. The
          DigitMap value class contains information about the dial plan.
          This may also contains special symbols like wildcards and timer
          values to be used on application of the dial plan.


        Return:
                 Returns a DigitMapValue java.lang.String object.
     _________________________________________________________________

  setDigitMapValueStr

   public final void setDigitMapValueStr(java.lang.String digitValueStr)
       throws javax.megaco.InvalidArgumentException

          This method sets the DigitMap Value in the string format. The
          applications may use this method in case it has pre-encoded
          dial plan and wants to use the same for subsequent calls.
          The digitValueStr string set in this method must be in the same
          format as defined by the MEGACO protocol.


        Parameter:
                digitValue - The DigitMap value object refernce.

        Throws:
                javax.megaco.InvalidArgumentException - Thrown if the
                digit map value passed in the arguments contains invalid
                parameters.
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR |  METHOD DETAIL:  FIELD | CONSTR |
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
