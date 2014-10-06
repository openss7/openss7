/*
 @(#) $RCSfile: SignalDetectorEvent.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:11 $ <p>
 
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
 
 Last Modified $Date: 2011-02-07 07:27:11 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

     _________________________________________________________________

   Overview  Package   Class  Use  Tree  Deprecated  Index  Help
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES     All Classes
   SUMMARY: NESTED | FIELD | CONSTR | METHOD
   DETAIL: FIELD | CONSTR | METHOD
     _________________________________________________________________

javax.media.mscontrol.mediagroup.signals
Interface SignalDetectorEvent

   All Superinterfaces:
          MediaEvent<SignalDetector>, ResourceEvent<SignalDetector>

   All Known Subinterfaces:
          SpeechRecognitionEvent
     _________________________________________________________________

public interface SignalDetectorEvent

   extends ResourceEvent<SignalDetector>

   SignalDetectorEvent defines the methods for SignalDetector Events.
     _________________________________________________________________

   Method Summary
    int getPatternIndex()
             When the event is caused by the matching of p_Pattern[i],
   returns the int i.
    Symbol[] getSignalBuffer()
             Get array of matched signal names.
    java.lang.String getSignalString()
             Get matched signals as a single java.lang.String.



   Methods inherited from interface
   javax.media.mscontrol.resource.ResourceEvent
   getQualifier, getRTCTrigger



   Methods inherited from interface
   javax.media.mscontrol.resource.MediaEvent
   getError, getErrorText, getEventID, getSource



   Method Detail

  getSignalString

java.lang.String getSignalString()

          Get matched signals as a single java.lang.String. The return value is
          constructed by concatenating a single-character for each signal
          Symbol.

          Each DTMF signal has a single-character equivalent, one of:
          0,1,2,3,4,5,6,7,8,9,*,#,A,B,C,D.
          The fax tones "CNG" and "CED" are represented by the characters
          '>' and '<' respectively.

        Returns:
                a java.lang.String of DTMF Signal chars.
     _________________________________________________________________

  getSignalBuffer

Symbol[] getSignalBuffer()

          Get array of matched signal names. Each signal has a Symbol
          "name".

          This method is useful for examining Signals that are not
          represented by single character names. In particular,
          extensions to the SignalDetector may use the Symbol namespace
          to define new signals.

        Returns:
                an Array of Symbols, one for each Signal in the event.
     _________________________________________________________________

  getPatternIndex

int getPatternIndex()

          When the event is caused by the matching of p_Pattern[i],
          returns the int i.
          Returns -1 in other cases.

        Returns:
                an int indicating which pattern was matched.
     _________________________________________________________________

   Overview  Package   Class  Use  Tree  Deprecated  Index  Help
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES     All Classes
   SUMMARY: NESTED | FIELD | CONSTR | METHOD
   DETAIL: FIELD | CONSTR | METHOD
     _________________________________________________________________

   Copyright (C) 2007-2008 Oracle and/or its affiliates. (C) Copyright
   2007-2008 Hewlett-Packard Development Company, L.P. All rights
   reserved. Use is subject to license terms.
*/
package javax.jain.media.mscontrol.mediagroup.signals;
