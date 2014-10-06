// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: PkgConsts.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:19 $ <p>
 
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
 
 Last Modified $Date: 2011-02-07 07:27:19 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
   SUMMARY:  INNER | FIELD |  CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.pkg
Class PkgConsts

java.lang.Object
  |
  +--javax.megaco.pkg.PkgConsts

   All Implemented Interfaces:
          None
     _________________________________________________________________

   public final class PkgConsts
   extends java.lang.Object

   Constants used in package javax.megaco.pkg to define the megaco
   packages.
     _________________________________________________________________

   Field Summary
   static int GENERIC_PACKAGE
              Identifies generic package.
   static int BASE_ROOT_PACKAGE
              Identifies base root package.
   static int TONE_GEN_PACKAGE
              Identifies Tone generation package.
   static int TONE_DET_PACKAGE
              Identifies tone detection package.
   static int BASIC_DTMF_GEN_PACKAGE
              Identifies basic dtmf generator package.
   static int DTMF_DET_PACKAGE
              Identifies DTMF detection package.
   static int CALL_PROG_TONE_GEN_PACKAGE
              Identifies call progress tone generation package.
   static int CALL_PROG_TONE_DET_PACKAGE
              Identifies call progress tone detection package.
   static int ANALOG_LINE_PACKAGE
              Identifies analog line supervision package.
   static int CONTINUITY_PACKAGE
              Identifies basic continuity package.
   static int NETWORK_PACKAGE
              Identifies network package.
   static int RTP_PACKAGE
              Identifies RTP package.
   static int TDM_CKT_PACKAGE
              Identifies TDM Circuit package.

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll,
   toString, wait

   Field Detail

  GENERIC_PACKAGE

public static final int GENERIC_PACKAGE

          Identifies generic MEGACO Package. Its value shall be set equal
          to 1.
     _________________________________________________________________

  BASE_ROOT_PACKAGE

public static final int BASE_ROOT_PACKAGE

          Identifies base root MEGACO Package. Its value shall be set
          equal to 2.
     _________________________________________________________________

  TONE_GEN_PACKAGE

public static final int TONE_GEN_PACKAGE

          Identifies tone generation MEGACO Package. Its value shall be
          set equal to 3.
     _________________________________________________________________

  TONE_DET_PACKAGE

public static final int TONE_DET_PACKAGE

          Identifies tone detection MEGACO Package. Its value shall be
          set equal to 4.
     _________________________________________________________________

  BASIC_DTMF_GEN_PACKAGE

public static final int BASIC_DTMF_GEN_PACKAGE

          Identifies basic DTMF generation MEGACO Package. Its value
          shall be set equal to 5.
     _________________________________________________________________

  DTMF_DET_PACKAGE

public static final int DTMF_DET_PACKAGE

          Identifies DTMF detection MEGACO Package. Its value shall be
          set equal to 6.
     _________________________________________________________________

  CALL_PROG_TONE_GEN_PACKAGE

public static final int CALL_PROG_TONE_GEN_PACKAGE

          Identifies call progress tone generation MEGACO Package. Its
          value shall be set equal to 7.
     _________________________________________________________________

  CALL_PROG_TONE_DET_PACKAGE

public static final int CALL_PROG_TONE_DET_PACKAGE

          Identifies call progress tone detection MEGACO Package. Its
          value shall be set equal to 8.
     _________________________________________________________________

  ANALOG_LINE_PACKAGE

public static final int ANALOG_LINE_PACKAGE

          Identifies analog line supervision MEGACO Package. Its value
          shall be set equal to 9.
     _________________________________________________________________

  CONTINUITY_PACKAGE

public static final int CONTINUITY_PACKAGE

          Identifies basic continuity MEGACO Package. Its value shall be
          set equal to 10.
     _________________________________________________________________

  NETWORK_PACKAGE

public static final int NETWORK_PACKAGE

          Identifies network MEGACO Package. Its value shall be set equal
          to 11.
     _________________________________________________________________

  RTP_PACKAGE

public static final int RTP_PACKAGE

          Identifies RTP MEGACO Package. Its value shall be set equal to
          12.
     _________________________________________________________________

  TDM_CKT_PACKAGE

public static final int TDM_CKT_PACKAGE

          Identifies TDM Circuit MEGACO Package. Its value shall be set
          equal to 13.
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
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
