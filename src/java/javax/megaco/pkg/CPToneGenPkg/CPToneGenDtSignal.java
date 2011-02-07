// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: CPToneGenDtSignal.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:21 $ <p>
 
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
 
 Last Modified $Date: 2011-02-07 07:27:21 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.pkg.CPToneGenPkg
Class CPToneGenDtSignal

java.lang.Object
    |
    +--javax.megaco.pkg.PkgItem
        |
        +--javax.megaco.pkg.PkgSignalItem
               |
               +--javax.megaco.pkg.CPToneGenPkg.CPToneGenDtSignal

   All Implemented Interfaces:
          None
     _________________________________________________________________

   public class CPToneGenDtSignal
   extends PkgSignalItem

   The MEGACO Dial Tone signal class extends the PkgSignalItem class.
   This is a final class. This  class defines Play Tone signal of MEGACO
   Call Progress Tone Generator package. The methods shall define that
   this signal item belongs to the Call Progress Tone Generator package.
     _________________________________________________________________

   Constructor Summary
   CPToneGenDtSignal()
              Constructs a MEGACO Signal Item with signal id as Dial
   Tone.

   Method Summary
    java.lang.String toString()
             Overrides java.lang.Object.toString().

   Field Summary
    public static final int CP_TONE_GEN_DT_SIGNAL
               Identifies Dial tone signal of the MEGACO Call Progress
   Tone Generator Package.

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Methods inherited from class javax.megaco.pkg.PkgItem
   getItemName, getItemId, getItemsPkgId

   Methods inherited from class javax.megaco.pkg.PkgSignalItem
   getSignalId

   Constructor Detail

  CPToneGenDtSignal

public CPToneGenDtSignal()

          Constructs a Jain MEGACO Object representing signal Item of the
          MEGACO Package for signal Dial Tone and Package as Call
          Progress Tone Generator.

   Method Detail

  getSignalId

public int getSignalId()

          This method is used to get the signal identifier from an Signal
          Item object. The implementations of this method in this class
          returns the id of the Dial Tone signal of Call Progress Tone
          Generator Package.

        Returns:
                It shall return CP_TONE_GEN_DT_SIGNAL.
     _________________________________________________________________

  getItemId

public int getItemId()

          This method is used to get the item identifier from an Item
          object. The implementations of this method in this class
          returns the id of the Dial Tone signal of Call Progress Tone
          Generator Package.

        Returns:
                It shall return CP_TONE_GEN_DT_SIGNAL.
     _________________________________________________________________

  getItemsPkgId

public MegacoPkg getItemsPkgId()

          This method gets the package id to which the item belongs.
          Since the Dial Tone signal is defined in the Call Progress Tone
          Generator Package of MEGACO protocol, this method returns the
          CPToneGenPkg class object.

        Returns:
                The package is CPToneGenPkg.
     _________________________________________________________________

  toString

public java.lang.String toString()

          Overrides java.lang.Object.toString().


        Overrides:
                toString in class java.lang.Object
     _________________________________________________________________

   Field Detail

  CP_TONE_GEN_DT_SIGNAL

public static final int CP_TONE_GEN_DT_SIGNAL

          Identifies Dial tone signal of the MEGACO Call Progress Tone
          Generator Package. Its value shall be set equal to 0x0030.
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
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
