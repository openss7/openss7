// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: MegacoListener.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:34 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:34 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS NEXT CLASS FRAMES  NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco
Interface MegacoListener

   All Superinterfaces:
          java.util.EventListener
     _________________________________________________________________

   public interface MegacoListener
   extends java.util.EventListener

   This interface defines the methods that must be implemented by any
   MEGACO user application to receive and process JAIN MEGACO Events
   emitted by an object that implements the MegacoProvider interface.
   Separate methods for handling JAIN MEGACO Command Event Objects and
   JAIN MEGACO Association Event Objects are specified. An instance of a
   class that implements this interface must register with an instance of
   a class that implements the MegacoProvider interface to receive these
   Event Objects.
     _________________________________________________________________

   Method Summary
    void processMegacoEvent(CommandEvent cmdEvent)
             Processes a Command event received from a MegacoProvider.

                                                                     void

   processMegacoEvent(AssociationEvent assocEvent)
             Processes a Association event received from a
   MegacoProvider.

   Method Detail

  processMegacoEvent

public void processMegacoEvent(CommandEvent cmdEvent)

          Processes a MEGACO Command event received from a
          MegacoProvider. Command level event occurs when the MEGACO
          stack receives a command from the peer. The stack after parsing
          the command parameters passes the command to the application
          through this listener interface.



        Parameters:
                cmdEvent - The JAIN MEGACO Command Event Object that is
                to be processed.
     _________________________________________________________________

  processMegacoEvent

public void processMegacoEvent(AssociationEvent assocEvent)

          Processes a MEGACO Association Event received from a
          MegacoProvider. Association level event occurs whenever stack
          wants to send the response of a request from the listener
          application or when the stack wants to give indication of the
          change in the state of the association at the stack.



        Parameters:
                assocEvent - The JAIN MEGACO Association Event Object
                that is to be processed.
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS NEXT CLASS FRAMES  NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD

   <
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
