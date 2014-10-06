/*
 @(#) $RCSfile: MediaGroupConfig.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:10 $ <p>
 
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
 
 Last Modified $Date: 2011-02-07 07:27:10 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

     _________________________________________________________________

   Overview  Package   Class  Use  Tree  Deprecated  Index  Help
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES     All Classes
   SUMMARY: NESTED | FIELD | CONSTR | METHOD
   DETAIL: FIELD | CONSTR | METHOD
     _________________________________________________________________

javax.media.mscontrol.mediagroup
Interface MediaGroupConfig

   All Superinterfaces:
          MediaConfig
     _________________________________________________________________

public interface MediaGroupConfig

   extends MediaConfig

   Stores the information necessary to configure (or re-configure) the
   collection of resources of an MediaGroup.
   A MediaGroupConfig is a non-mutable (read-only) structure. It
   describes the Resources and how they are connected together, as well
   as the initial values of the parameters.

    Underlying MediaConfig

   A MediaGroupConfig has its internal structure of Resources described
   by the inherited MediaConfig.

    Pre-defined MediaGroupConfigs:

   Several MediaGroupConfig objects are pre-defined, for the most common
   application types. Please see below the Symbols of the form
   c_SomeFeature.

   A pre-defined MediaGroupConfig can be customized with
   MediaConfig.createCustomizedClone(javax.media.mscontrol.resource.Param
   eters).
     _________________________________________________________________

   Field Summary
   static ConfigSymbol<MediaGroupConfig> c_Player
             Designates a MediaGroupConfig containing only a Player
   interface (supporting all protocols)
   Suitable for ring back tone applications, for example.
   static ConfigSymbol<MediaGroupConfig> c_PlayerRecorderSignalDetector
             A MediaGroupConfig containing Player, Recorder,
   SignalDetector, and SignalGenerator.
   static ConfigSymbol<MediaGroupConfig> c_PlayerSignalDetector
             Designates a MediaGroupConfig containing a Player and a
   SignalDetector
   Suitable for interactive applications that do not need recording.
   static ConfigSymbol<MediaGroupConfig> c_SignalDetector
             Contains only a signaldetector.



   Method Summary



   Methods inherited from interface javax.media.mscontrol.MediaConfig
   createCustomizedClone, getSupportedSymbols, hasStream, marshall



   Field Detail

  c_Player

static final ConfigSymbol<MediaGroupConfig> c_Player

          Designates a MediaGroupConfig containing only a Player
          interface (supporting all protocols)
          Suitable for ring back tone applications, for example.
     _________________________________________________________________

  c_PlayerSignalDetector

static final ConfigSymbol<MediaGroupConfig> c_PlayerSignalDetector

          Designates a MediaGroupConfig containing a Player and a
          SignalDetector
          Suitable for interactive applications that do not need
          recording.
     _________________________________________________________________

  c_PlayerRecorderSignalDetector

static final ConfigSymbol<MediaGroupConfig> c_PlayerRecorderSignalDetector

          A MediaGroupConfig containing Player, Recorder, SignalDetector,
          and SignalGenerator.
          Suitable for Interactive Voice Response applications.
          Contains also a speechdetector, to control begin and end of
          recording (see SpeechDetectorConstants).
     _________________________________________________________________

  c_SignalDetector

static final ConfigSymbol<MediaGroupConfig> c_SignalDetector

          Contains only a signaldetector.
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
package javax.jain.media.mscontrol.mediagroup;
