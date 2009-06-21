// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: TimeStamp.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:38 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:38 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES   NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.message.descriptor
Class TimeStamp

java.lang.Object
  |
  +--javax.megaco.message.descriptor.TimeStamp

   All Implemented Interfaces:
          java.io.Serializable

   See Also:
          Serialized Form
     _________________________________________________________________

   public class TimeStamp
   extends java.lang.Object

   The TimeStamp object is a class that shall be used to set the time and
   date at which the event was detected within the observed event
   descriptor. This is an independent class derived from java.util.Object
   and shall not have any derived classes.
     _________________________________________________________________

   Constructor Summary
   TimeStamp(int year, int month, int day, int hour, int min, int sec)
           throws javax.megaco.InvalidArgumentException

              Constructs an object of type timestamp that shall have the
   date and time value when an event was detected. This is attached to an
   Observed Event descriptor.

   Method Summary
     int getYear()
              The method gives the year of the absolute date.
     int getMonth()
              The method gives the month of the absolute date.
     int getDay()
              The method gives the day of the absolute date.
     int getHour()
              The method gives the hour of the absolute time.
     int getMinutes()
              The method gives the minutes of the absolute time.
     int getSecs()
              The method gives the seconds of the absolute time.
    java.lang.String toString()
             Overrides java.lang.Object.toString().

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  TimeStamp

public TimeStamp(int year, int month, int day, int hour, int min, int sec)
                     throws javax.megaco.InvalidArgumentException

          Constructs a timestamp object that contains the date and time.
          This shall be used within the observed event to detect the time
          at which the event was detected.
          Throws:
          javax.megaco.InvalidArgumentException - Thrown if an illegal
          date or time parameter is set.
     _________________________________________________________________

   Method Detail

  getYear

public int getYear()

          The method is used the to retrieve the year from the absolute
          date set.

        Returns:
                year - The integer value of the year.
     _________________________________________________________________

  getMonth

public int getMonth()

          The method can be used the to retrieve month set in the object.

        Returns:
                month - Integer value of the month.
     _________________________________________________________________

  getDay

public int getDay()

          The method can be used the to retrieve day set in the object.

        Returns:
                day - Integer value of the day.
     _________________________________________________________________

  getHour

public int getHour()

          The method is used the to retrieve the hour from the absolute
          time set.

        Returns:
                hour - The integer value of the hour.
     _________________________________________________________________

  getMinutes

public int getMinutes()

          The method is used the to retrieve the minutes from the
          absolute time set.

        Returns:
                minutes - The integer value of the minutes.
     _________________________________________________________________

  getSecs

public int getSecs()

          The method is used the to retrieve the secs from the absolute
          time set.

        Returns:
                secs - The integer value of the secs.
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
    PREV CLASS   NEXT CLASS FRAMES   NO FRAMES
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
