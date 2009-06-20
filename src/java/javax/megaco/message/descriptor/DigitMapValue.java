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
Class DigitMapValue

java.lang.Object
  |
  +--javax.megaco.message.descriptor.DigitMapValue

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class DigitMapValue
   extends java.lang.Object

   The DigitMapValue object is a class that shall be used to set the
   digit map within the digit map descriptor. The DigitMap value class
   contains information about the dial plan. This may also contains
   special symbols like wildcards and timer values to be used on
   application of the dial plan. This is an independent class derived
   from java.util.Object and shall not have any derived classes.

   See Also:
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   DigitMapValue()
              Constructs an object of type digit map, which is used as a
   parameter within digit map descriptor, and specifies the dial plan.

   Method Summary
     boolean  isTimerTPresent()
              This method specifies whether the timer T is present or
   not.
     int  getTimerTValue()
           throws javax.megaco.ParameterNotSetException

              The method gets the timer T value.
     void  setTimerTValue(int timerT)
           throws javax.megaco.InvalidArgumentException

              The method sets the timer T value.
     boolean  isTimerSPresent()
              This method specifies whether the timer S is present or
   not.
     int  getTimerSValue()
           throws javax.megaco.ParameterNotSetException

              The method gets the timer S value.
     void  setTimerSValue(int timerS)
           throws javax.megaco.InvalidArgumentException

              The method sets the timer S value.
     boolean  isTimerLPresent()
              This method specifies whether the timer L is present or
   not.
     int  getTimerLValue()
           throws javax.megaco.ParameterNotSetException

              The method gets the timer L value.
     void  setTimerLValue(int timerL)
           throws javax.megaco.InvalidArgumentException

              The method sets the timer L value.
     DigitMapString[] getDigitMapStrings()
              The method retrives the vector of the object identifier for
   the digit string. Each of the vector element specifies one digit
   string.
     void  setDigitMapStrings (DigitMapString[] digitStrings)
           throws javax.megaco.InvalidArgumentException

              The method sets the vector of the object identifier for the
   digit string. Each of the vector element specifies one digit string.
    java.lang.String toString()
             Overrides java.lang.Object.toString().

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  DigitMapValue

public DigitMapValue()

          Constructs a Digit Map Value Object. This shall contain
          information about the digit string in the dial plan.

   Method Detail

  isTimerTPresent

public boolean isTimerTPresent()

          This method specifies whether the timer T value is present or
          not.


        Returns:
                 TRUE if the timer T value is present.
     _________________________________________________________________

  getTimerTValue

public int getTimerTValue()
         throws javax.megaco.ParameterNotSetException

          The method gets the timer T value.This method should be invoked
          after verifying using method isTimerTPresent() that the timer T
          value has been set.


        Returns:
                 The integer value for the timer value if it is set.

        Throws:
                javax.megaco.ParameterNotSetException - if
                isTimerTPresent() returns FALSE.
     _________________________________________________________________

  setTimerTValue

public void setTimerTValue(int timerT)
           throws javax.megaco.InvalidArgumentException

          The method sets the timer T value.


        Parameter:
                 The integer value for the timer value. This
                automatically sets the isTimerTPresent() to TRUE.

        Throws:
                javax.megaco.InvalidArgumentException - if timer value is
                not correctly set.
     _________________________________________________________________

  isTimerSPresent

public boolean isTimerSPresent()

          This method specifies whether the timer S value is present or
          not.


        Returns:
                 TRUE if the timer S value is present.
     _________________________________________________________________

  getTimerSValue

public int getTimerSValue()
     throws javax.megaco.ParameterNotSetException

          The method gets the timer S value. This method should be
          invoked after verifying using method isTimerSPresent() that the
          timer S value has been set.


        Returns:
                 The integer value for the timer value if it is set.

        Throws:
                javax.megaco.ParameterNotSetException - if
                isTimerSPresent() returns FALSE.
     _________________________________________________________________

  setTimerSValue

public void setTimerSValue(int timerS)
          throws javax.megaco.InvalidArgumentException

          The method sets the timer S value.


        Parameter:
                 The integer value for the timer value. This
                automatically sets the isTimerSPresent() to TRUE.

        Throws:
                javax.megaco.InvalidArgumentException - if timer value is
                not correctly set.
     _________________________________________________________________

  isTimerLPresent

public boolean isTimerLPresent()

          This method specifies whether the timer L value is present or
          not.


        Returns:
                 TRUE if the timer L value is present.
     _________________________________________________________________

  getTimerLValue

public int getTimerLValue()
          throws javax.megaco.ParameterNotSetException

          The method gets the timer L value. This method should be
          invoked after verifying using method isTimerLPresent() that the
          timer L value has been set.


        Returns:
                 The integer value for the timer value if it is set.

        Throws:
                javax.megaco.ParameterNotSetException - if
                isTimerLPresent() returns FALSE.
     _________________________________________________________________

  setTimerLValue

public void setTimerLValue(int timerL)
                        throws javax.megaco.InvalidArgumentException

          The method sets the timer L value.


        Parameter:
                 The integer value for the timer value. This
                automatically sets the isTimerLPresent() to TRUE.

        Throws:
                javax.megaco.InvalidArgumentException - if timer value is
                not correctly set.
     _________________________________________________________________

  getDigitMapStrings

public DigitMapString[] getDigitMapStrings()

          The method is used to get the vector of all dial plans in the
          digit map.


        Returns:
                 The vector of all digit strings in a dial plan. If the
                vector of digit map strings is not set then this method
                will return NULL.
     _________________________________________________________________

  setDigitMapStrings

public void setDigitMapStrings(DigitMapString[] digitStrings)
                        throws javax.megaco.InvalidArgumentException

          The method sets the vector of the object identifier for the
          digit string. Each of the vector element specifies one digit
          string.


        Parameter:
                 The vector of the object identifer of the digit strings.

        Throws:
                javax.megaco.InvalidArgumentException - if the digit
                string is not set properly.
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
