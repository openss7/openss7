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
Interface ResourceConstants

   All Known Subinterfaces:
          Player, Recorder, Resource<T>, SignalDetector, SignalGenerator,
          VideoRenderer
     _________________________________________________________________

public interface ResourceConstants

   Defines the constants used by Resource and ResourceEvent. As these are
   inherited by every Resource and ResourceEvent class, they are
   accessible from each of those classes or instances.
     _________________________________________________________________

   Field Summary
   static Symbol e_OK
             Symbol returned from MediaEvent.getError() if there is no
   error.
   static int FOREVER
             Special int value (-1) used to indicate infinite timeout.
   static Symbol q_Duration
             Symbol returned from ResourceEvent.getQualifier() if an
   operation completed because the requested duration was reached.
   static Symbol q_RTC
             Qualifier: Completion caused by a Run-Time Control.
   static Symbol q_Standard
             Qualifier: normal, default completion.
   static Symbol q_Stop
             Qualifier: Completion caused by a Stop.
   static Symbol rtcc_TriggerRTC
             The Symbol returned from ResourceEvent.getRTCTrigger()
   representing the RTC Condition for ResourceEvents
   static java.lang.Integer v_Forever
             Integer value for FOREVER, when used in a value in a
   Parameters



   Field Detail

  FOREVER

static final int FOREVER

          Special int value (-1) used to indicate infinite timeout.

        See Also:
                Constant Field Values
     _________________________________________________________________

  v_Forever

static final java.lang.Integer v_Forever

          Integer value for FOREVER, when used in a value in a Parameters
     _________________________________________________________________

  e_OK

static final Symbol e_OK

          Symbol returned from MediaEvent.getError() if there is no
          error.
     _________________________________________________________________

  q_Duration

static final Symbol q_Duration

          Symbol returned from ResourceEvent.getQualifier() if an
          operation completed because the requested duration was reached.
     _________________________________________________________________

  q_Standard

static final Symbol q_Standard

          Qualifier: normal, default completion.
     _________________________________________________________________

  q_Stop

static final Symbol q_Stop

          Qualifier: Completion caused by a Stop. Normal response from
          operations stopped by MediaGroup.stop() method.
     _________________________________________________________________

  q_RTC

static final Symbol q_RTC

          Qualifier: Completion caused by a Run-Time Control.
     _________________________________________________________________

  rtcc_TriggerRTC

static final Symbol rtcc_TriggerRTC

          The Symbol returned from ResourceEvent.getRTCTrigger()
          representing the RTC Condition for ResourceEvents
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
