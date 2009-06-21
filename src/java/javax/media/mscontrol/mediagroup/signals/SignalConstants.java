/*
 @(#) $RCSfile: SignalConstants.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:25 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:25 $ by $Author: brian $
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
Interface SignalConstants

   All Known Subinterfaces:
          SignalDetector, SignalDetectorConstants, SignalGenerator,
          SignalGeneratorConstants, SignalGeneratorEvent
     _________________________________________________________________

public interface SignalConstants

   SignalConstants supplies the names of the DTMF signals.

   These Symbols are used by SignalGenerator and SignalDetector.
     _________________________________________________________________

   Field Summary
   static Symbol p_SymbolChar
             Specifies the chars to use when non standard Signals are
   converted to a java.lang.String or vice versa.
   static Symbol v_CED
             Symbol for CED. 2100Hz tone answer to v_CNG.
   static Symbol v_CNG
             Symbol for CNG. 1100Hz tone answered by v_CED.
   static Symbol v_DTMF0
             Symbol for DTMF 0
   static Symbol v_DTMF1
             Symbol for DTMF 1
   static Symbol v_DTMF2
             Symbol for DTMF 2
   static Symbol v_DTMF3
             Symbol for DTMF 3
   static Symbol v_DTMF4
             Symbol for DTMF 4
   static Symbol v_DTMF5
             Symbol for DTMF 5
   static Symbol v_DTMF6
             Symbol for DTMF 6
   static Symbol v_DTMF7
             Symbol for DTMF 7
   static Symbol v_DTMF8
             Symbol for DTMF 8
   static Symbol v_DTMF9
             Symbol for DTMF 9
   static Symbol v_DTMFA
             Symbol for DTMF A
   static Symbol v_DtmfAuto
             Value for p_DtmfMode: DTMFs are handled either in or
   out-of-band, depending on the context.
   static Symbol v_DTMFB
             Symbol for DTMF B
   static Symbol v_DTMFC
             Symbol for DTMF C
   static Symbol v_DTMFD
             Symbol for DTMF D
   static Symbol v_DTMFHash
             Symbol for DTMF #
   static Symbol v_DtmfInBand
             Value for p_DtmfMode: DTMFs are handled in-band (signal
   modulation)
   static Symbol v_DtmfInBandOutBand

   static Symbol v_DtmfNone

   static Symbol v_DtmfOutBand
             Value for p_DtmfMode: DTMFs are handled out-of-band (rfc2833
   packets)
   static Symbol v_DTMFStar
             Symbol for DTMF *
   static Symbol v_VFURequest
             Symbol representing a request for a VideoFastUpdate.



   Field Detail

  v_DTMF0

static final Symbol v_DTMF0

          Symbol for DTMF 0
     _________________________________________________________________

  v_DTMF1

static final Symbol v_DTMF1

          Symbol for DTMF 1
     _________________________________________________________________

  v_DTMF2

static final Symbol v_DTMF2

          Symbol for DTMF 2
     _________________________________________________________________

  v_DTMF3

static final Symbol v_DTMF3

          Symbol for DTMF 3
     _________________________________________________________________

  v_DTMF4

static final Symbol v_DTMF4

          Symbol for DTMF 4
     _________________________________________________________________

  v_DTMF5

static final Symbol v_DTMF5

          Symbol for DTMF 5
     _________________________________________________________________

  v_DTMF6

static final Symbol v_DTMF6

          Symbol for DTMF 6
     _________________________________________________________________

  v_DTMF7

static final Symbol v_DTMF7

          Symbol for DTMF 7
     _________________________________________________________________

  v_DTMF8

static final Symbol v_DTMF8

          Symbol for DTMF 8
     _________________________________________________________________

  v_DTMF9

static final Symbol v_DTMF9

          Symbol for DTMF 9
     _________________________________________________________________

  v_DTMFA

static final Symbol v_DTMFA

          Symbol for DTMF A

        Since:
                Not supported
     _________________________________________________________________

  v_DTMFB

static final Symbol v_DTMFB

          Symbol for DTMF B

        Since:
                Not supported
     _________________________________________________________________

  v_DTMFC

static final Symbol v_DTMFC

          Symbol for DTMF C

        Since:
                Not supported
     _________________________________________________________________

  v_DTMFD

static final Symbol v_DTMFD

          Symbol for DTMF D

        Since:
                Not supported
     _________________________________________________________________

  v_DTMFStar

static final Symbol v_DTMFStar

          Symbol for DTMF *
     _________________________________________________________________

  v_DTMFHash

static final Symbol v_DTMFHash

          Symbol for DTMF #
     _________________________________________________________________

  v_CNG

static final Symbol v_CNG

          Symbol for CNG. 1100Hz tone answered by v_CED. Alternating
          500ms of 1100Hz and 3 seconds of silence for more than one
          minute, until the remote fax answers with 2100Hz.
     _________________________________________________________________

  v_CED

static final Symbol v_CED

          Symbol for CED. 2100Hz tone answer to v_CNG.
     _________________________________________________________________

  v_VFURequest

static final Symbol v_VFURequest

          Symbol representing a request for a VideoFastUpdate.
     _________________________________________________________________

  p_SymbolChar

static final Symbol p_SymbolChar

          Specifies the chars to use when non standard Signals are
          converted to a java.lang.String or vice versa. The associated value is a
          java.lang.Object[] even numbered elements are a Symbol, odd numbered
          elements are the associated Character.

          For example, The standard values could be represented like:

 java.lang.Object[] v_StandardDTMFChars = {
     v_DTMF0, new Character('0'),
     v_DTMF1, new Character('1'),
     v_DTMF2, new Character('2'),
     v_DTMF3, new Character('3'),
     // ...
     };

     _________________________________________________________________

  v_DtmfInBand

static final Symbol v_DtmfInBand

          Value for p_DtmfMode: DTMFs are handled in-band (signal
          modulation)
     _________________________________________________________________

  v_DtmfOutBand

static final Symbol v_DtmfOutBand

          Value for p_DtmfMode: DTMFs are handled out-of-band (rfc2833
          packets)
     _________________________________________________________________

  v_DtmfInBandOutBand

static final Symbol v_DtmfInBandOutBand
     _________________________________________________________________

  v_DtmfNone

static final Symbol v_DtmfNone
     _________________________________________________________________

  v_DtmfAuto

static final Symbol v_DtmfAuto

          Value for p_DtmfMode: DTMFs are handled either in or
          out-of-band, depending on the context. This is the default
          value.
          A full understanding of the DTMF modes requires background
          information outside of the scope of this document. Application
          developpers that are unsure about this should just rely on the
          implementation default behaviour.
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
package javax.jain.media.mscontrol.mediagroup.signals;
