// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: DigitStringPosition.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:17 $ <p>
 
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
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.message.descriptor
Class DigitStringPosition

java.lang.Object
  |
  +--javax.megaco.message.descriptor.DigitStringPosition

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class DigitStringPosition
   extends java.lang.Object

   The DigitStringPosition object is a class that shall be used to set
   the Digit Position in a digit map string in the Digit Map descriptor.
   This is an independent class derived from java.util.Object and shall
   not have any derived classes. Each digit position object shall specify
   the list of digits that can be valid at that digit position.

   See Also:
          Serialized Form
     _________________________________________________________________

   Field Summary
   static int DIGIT0
              Identifies a DIGIT 0.
   Its value shall be set to 0.
   static int DIGIT1
              Identifies a DIGIT 1.
   Its value shall be set to 1.
   static int DIGIT2
              Identifies a DIGIT 2.
   Its value shall be set to 2.
   static int DIGIT3
              Identifies a DIGIT 3.
   Its value shall be set to 3.
   static int DIGIT4
              Identifies a DIGIT 4.
   Its value shall be set to 4.
   static int DIGIT5
              Identifies a DIGIT 5.
   Its value shall be set to 5.
   static int DIGIT6
              Identifies a DIGIT 6.
   Its value shall be set to 6.
   static int DIGIT7
              Identifies a DIGIT 7.
   Its value shall be set to 7.
   static int DIGIT8
              Identifies a DIGIT 8.
   Its value shall be set to 8.
   static int DIGIT9
              Identifies a DIGIT 9.
   Its value shall be set to 9.
   static int DIGITA
              Identifies a DIGIT A.
   Its value shall be set to 10.
   static int DIGITB
              Identifies a DIGIT B.
   Its value shall be set to 11.
   static int DIGITC
              Identifies a DIGIT C.
   Its value shall be set to 12.
   static int DIGITD
              Identifies a DIGIT D.
   Its value shall be set to 13.
   static int DIGIT_STAR
              Identifies a DIGIT STAR.
   Its value shall be set to 14.
   static int DIGIT_HASH
              Identifies a DIGIT HASH.
   Its value shall be set to 15.
   static int DIGITG
              Identifies a DIGIT G.
   Its value shall be set to 16.
   static int DIGITH
              Identifies a DIGIT H.
   Its value shall be set to 17.
   static int DIGITI
              Identifies a DIGIT I.
   Its value shall be set to 18.
   static int DIGITJ
              Identifies a DIGIT J.
   Its value shall be set to 19.
   static int DIGITK
              Identifies a DIGIT K.
   Its value shall be set to 20.
   static int DIGIT_DOT
              Identifies a DIGIT DOT.
              Its value shall be set to 21.
   static int DIGITL
              Identifies a DIGIT L.
              Its value shall be set to 22.
   static int DIGITS
              Identifies a DIGIT S.
              Its value shall be set to 23.
   static int DIGITZ
              Identifies a DIGIT Z.
              Its value shall be set to 24.
   static int DIGITX
              Identifies a DIGIT X.
              Its value shall be set to 25.

   Constructor Summary
   DigitStringPosition()
              Constructs an object of type digit position that specifies
   the digits that are valid for each digit position.

   Method Summary
     int[] getDigits()
           throws javax.megaco.ParameterNotSetException

              The method retrives the vector of digits that are valid for
   a digit position. The values of digits are as specified in the fields.
     void  setDigitStringPosition(int[] digits)
           throws javax.megaco.InvalidArgumentException

              The method sets the vector of digits that are valid for the
   digit position.
    java.lang.String toString()
             Overrides java.lang.Object.toString().

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Field Detail

  DIGIT0

public static final int DIGIT0

          Identifies THE DIGIT 0.
          Its value shall be set to 0.
     _________________________________________________________________

  DIGIT1

public static final int DIGIT1

          Identifies THE DIGIT 1.
          Its value shall be set to 1.
     _________________________________________________________________

  DIGIT2

public static final int DIGIT2

          Identifies THE DIGIT 2.
          Its value shall be set to 2.
     _________________________________________________________________

  DIGIT3

public static final int DIGIT3

          Identifies THE DIGIT 3.
          Its value shall be set to 3.
     _________________________________________________________________

  DIGIT4

public static final int DIGIT4

          Identifies THE DIGIT 4.
          Its value shall be set to 4.
     _________________________________________________________________

  DIGIT5

public static final int DIGIT5

          Identifies THE DIGIT 5.
          Its value shall be set to 5.
     _________________________________________________________________

  DIGIT6

public static final int DIGIT6

          Identifies THE DIGIT 6.
          Its value shall be set to 6.
     _________________________________________________________________

  DIGIT7

public static final int DIGIT7

          Identifies THE DIGIT 7.
          Its value shall be set to 7.
     _________________________________________________________________

  DIGIT8

public static final int DIGIT8

          Identifies THE DIGIT 8.
          Its value shall be set to 8.
     _________________________________________________________________

  DIGIT9

public static final int DIGIT9

          Identifies THE DIGIT 9.
          Its value shall be set to 9.
     _________________________________________________________________

  DIGITA

public static final int DIGITA

          Identifies THE DIGIT A.
          Its value shall be set to 10.
     _________________________________________________________________

  DIGITB

public static final int DIGITB

          Identifies THE DIGIT B.
          Its value shall be set to 11.
     _________________________________________________________________

  DIGITC

public static final int DIGITC

          Identifies THE DIGIT C.
          Its value shall be set to 12.
     _________________________________________________________________

  DIGITD

public static final int DIGITD

          Identifies THE DIGIT D.
          Its value shall be set to 13.
     _________________________________________________________________

  DIGIT_STAR

public static final int DIGIT_STAR

          Identifies THE DIGIT STAR.
          Its value shall be set to 14.
     _________________________________________________________________

  DIGIT_HASH

public static final int DIGIT_HASH

          Identifies THE DIGIT HASH.
          Its value shall be set to 15.
     _________________________________________________________________

  DIGITG

public static final int DIGITG

          Identifies THE DIGIT G.
          Its value shall be set to 16.
     _________________________________________________________________

  DIGITH

public static final int DIGITH

          Identifies THE DIGIT H.
          Its value shall be set to 17.
     _________________________________________________________________

  DIGITI

public static final int DIGITI

          Identifies THE DIGIT I.
          Its value shall be set to 18.
     _________________________________________________________________

  DIGITJ

public static final int DIGITJ

          Identifies THE DIGIT J.
          Its value shall be set to 19.
     _________________________________________________________________

  DIGITK

public static final int DIGITK

          Identifies THE DIGIT K.
          Its value shall be set to 20.
     _________________________________________________________________

  DIGIT_DOT

public static final int DIGIT_DOT

          Identifies THE DIGIT DOT.
          Its value shall be set to 21.
     _________________________________________________________________

  DIGITL

public static final int DIGITL

          Identifies THE DIGIT L. This refers to the inter-event long
          duration timer.
          Its value shall be set to 22.
     _________________________________________________________________

  DIGITS

public static final int DIGITS

          Identifies THE DIGIT S. This refers to the inter-event short
          duration timer.
          Its value shall be set to 23.
     _________________________________________________________________

  DIGITZ

public static final int DIGITZ

          Identifies THE DIGIT Z. This refers to the Long duration timer.
          Its value shall be set to 24.
     _________________________________________________________________

  DIGITX

public static final int DIGITX

          Identifies THE DIGIT X.
          Its value shall be set to 25.
     _________________________________________________________________

   Constructor Detail

  DigitStringPosition

public DigitStringPosition()

          Constructs a Digit Map Position Object. This shall contain
          information about the Digit Position in the digit string.

   Method Detail

  getDigitStringPosition

public int[] getDigitStringPosition()
            throws javax.megaco.ParameterNotSetException

          The method is used to get the vector of the digits that can
          occur at the digit position.


        Returns:
                 The vector specifying the digits at the digit position.

        Throws:
                javax.megaco.ParameterNotSetException : This exception is
                raised if single digit position has not been set.
     _________________________________________________________________

  setDigitStringPosition

public void setDigitStringPosition(int[] digits)
                        throws javax.megaco.InvalidArgumentException

          The method sets the vector of integers specifying digits that
          are valid for the digit position.


        Parameter:
                 The vector of the integer for digits.

        Throws:
                javax.megaco.InvalidArgumentException - if the vector of
                digits contains values other than the static constants as
                defined for this class.
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
