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

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.message.descriptor
Class DigitMapString

java.lang.Object
  |
  +--javax.megaco.message.descriptor.DigitMapString

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class DigitMapString
   extends java.lang.Object

   The DigitMapString object is a class that shall be used to set the
   digit string in digit map within the digit map descriptor. This is an
   independent class derived from java.util.Object and shall not have any
   derived classes. Each digit string object in turn stores a vector of
   DigitPosition object references. The DigitPosition object specifies
   the combination of digits that can be present at a position in the
   digit string.

   See Also:
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   DigitMapString()
              Constructs an object of type digit string, which is used as
   a parameter within digit map for digit map descriptor, and specifies
   the digit position values within a digit string.

   Method Summary
     DigitStringPosition[] getDigitStringPosition()

              The method retrives the vector of the object identifier for
   the digit position. Each of the vector element specifies one digit
   position within one digit string.
     void  setDigitStringPosition (DigitStringPosition[] digitPositions)
           throws javax.megaco.InvalidArgumentException

              The method sets the vector of the object identifier for the
   digit positions. Each of the vector element specifies different
   combinations of digits that can occur in one digit position within one
   digit string.
    java.lang.String toString()
             Overrides java.lang.Object.toString().

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  DigitMapString

public DigitMapString()

          Constructs a Digit Map java.lang.String Object. This shall contain
          information about the digit string in the digit plan.

   Method Detail

  getDigitStringPosition

public DigitStringPosition[] getDigitStringPosition()

          The method is used to get the vector of the digit position
          object references. Each of these digit positions represents
          digits with values between 1-9 and A-K, L, S or Z. The index of
          the vector DigitStringPosition gives the position for which the
          position object is specifying the digits.


        Returns:
                 The vector specifying the digit combinations that can
                occur at each digit position for the current digit string
                in a digit plan. If no value for digit position is
                specified, then this method will return NULL.
     _________________________________________________________________

  setDigitStringPosition

public void setDigitStringPosition(DigitStringPosition[] digitPositions)
                        throws javax.megaco.InvalidArgumentException

          The method sets the vector of the object identifier for the
          digit positions. Each of the vector element specifies digits
          that are valid for each digit position within one digit string.
          The index of the vector DigitStringPosition gives the position
          for which the position object is specifying the digits.


        Parameter:
                 The vector of the object identifer of the digit
                positions.

         
        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Digit java.lang.String Position passed to this
                method is NULL.
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

                  Copyright © 2001 Hughes Software Systems
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
