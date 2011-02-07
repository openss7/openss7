/*
 @(#) $RCSfile: CoderConstants.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:10 $ <p>
 
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
Interface CoderConstants

   All Known Subinterfaces:
          Player, PlayerConstants, Recorder, RecorderConstants,
          RecorderEvent
     _________________________________________________________________

public interface CoderConstants

   Define the Symbol constants for the Coders.

   These are used as values for PlayerConstants.p_AudioCoder and
   RecorderConstants.p_AudioCoder correlated to
   PlayerConstants.p_FileFormat and RecorderConstants.p_FileFormat.

Audio Codecs

   The audio CODECs available to media operations are the following when
   setting p_FileFormat to v_RawFormat, v_WavFormat or v_GSMFormat :

   p_AudioCoder p_FileFormat
   v_RawFormat v_WavFormat v_GSMFormat
   v_Linear16Bit_128k default, internal, plain format supported supported
   N/A
   v_Linear16Bit_256k high quality format supported supported N/A
   v_ALawPCM_64k common european format supported supported N/A
   v_MuLawPCM_64k common american format supported supported N/A
   v_ADPCM_32k IMA's Adapatative Differential Pulse Coded Modulation
   supported (no chunking) supported (no chunking) N/A
   v_ADPCM_32kOKI OKI's ADPCM supported (no chunking) supported (no
   chunking) N/A
   v_G723_1b G723.1 N/A supported (proprietary payload type) N/A
   v_AMR AMR format at Adaptating Rate, used with additional parameters.
   N/A supported (proprietary payload type) N/A
   v_AMR_WB AMR wide band format N/A supported (proprietary payload type)
   N/A
   v_G729a G729 format at 8kb/s N/A supported (proprietary payload type)
   N/A
   v_EVRC EVRC format N/A supported (proprietary payload type) N/A
   v_GSM GSM format N/A N/A supported

   The same CODECs are available to media operations when setting
   p_FileFormat to v_RawFormat, v_WavFormat or v_GSMFormat

   NOTE: when playing a file with p_FileFormat=v_WavFormat, the
   p_AudioCoder value is not used as it will be retrieved from the wav
   file header

      Code snippet for playing Wave files (Wave file made of any of the
      supported encoding type)

   If the file has the traditional extension ".wav", no additional
   information is required:
 mediagroup.play("/home/IVR/message.wav", myRtcs, null);


   If the file has a non-standard extension, or no extension:
 params.put(PlayerConstants.p_FileFormat, PlayerConstants.v_WavFormat);
 mediagroup.play("/home/IVR/message", myRtcs, params);


      Code snippet for playing raw format OKI encoded files (4 bits per sample,
      8000 samples per second)

 params.put(PlayerConstants.p_FileFormat, PlayerConstants.v_RawFormat);
 params.put(PlayerConstants.p_Coder, PlayerConstants.v_ADPCM_32kOKI);
 mediagroup.play("/home/IVR/myokifile", myRtcs, params);


      Code snippet for recording a WAV file (16 bits per sample)

   If the file has the traditional extension ".wav", only the coder type
   is required:
 params.put(RecorderConstants.p_Coder, RecorderConstants.v_Linear16Bit_128k);
 mediagroup.record("/tmp/recorded_message.wav", myRtcs, params);


   If the file has a non-standard extension, or no extension, specifiy
   also the file format:
 params.put(RecorderConstants.p_Coder, RecorderConstants.v_Linear16Bit_128k);
 params.put(RecorderConstants.p_FileFormat, RecorderConstants.v_WavFormat);
 mediagroup.record("/tmp/recorded_message", myRtcs, params);


   Note: Using non-standard file extensions is discouraged.

Video Codecs

   3gpp and 3gpp2 files are supported. Files with the extension ".3gp"
   are automatically recognized as 3gpp files, and files with ".3g2" or
   ".3gpp2" extensions are recognized as 3gpp2 files. The Symbol
   v_3GPFormat can be used to indicate a 3gpp file format, if the file
   extension is different (similarly to v_WavFormat).

   Video codecs supported in 3gp files are indicated below.

      Code snippet for playing a video file

   Video files are played or recorded just like audio files:
 mediagroup.play("/home/IVR/message.3gp", myRtcs, null);

     _________________________________________________________________

   Field Summary
   static Symbol v_3G2Format
             3gpp2 files format.
   static Symbol v_3GPFormat
             3gp files format.
   static Symbol v_ADPCM_16kG726
             G726 16k
   static Symbol v_ADPCM_32k
             ADPCM
   static Symbol v_ADPCM_32kG726
             G726 32k
   static Symbol v_ADPCM_32kOKI
             ADPCM Oki, a.k.a. vox files
   static Symbol v_ALawPCM_48k

   static Symbol v_ALawPCM_64k
             Alaw PCM (mimetype: pcma)
   static Symbol v_AMR
             AMR (narrow band)
   static Symbol v_AMR_WB
             AMR wideband
   static Symbol v_EVRC
             EVRC coder
   static Symbol v_G723_1b
             G.723 codec
   Valid fmtp values, per rfc 3555, paragraph 4.1.3: "bitrate=6.3" or
   "bitrate="5.3" "annexa=yes" (default) or "annexa=no" Those values may
   be assigned to RecorderConstants.p_AudioFmtp.
   static Symbol v_G729a
             G.729 codec
   Valid fmtp values, per rfc 3555, paragraph 4.1.9: "annexb=yes"
   (default) or "annexb=no" Those values may be assigned to
   RecorderConstants.p_AudioFmtp.
   static Symbol v_GSM
             GSM coder
   static Symbol v_GSMFormat
             value for Player/RecorderConstants.p_Fileformat: GSM file
   format The only supported p_Coder value is v_GSM
   static Symbol v_H263
             Value for p_VideoCoder.
   static Symbol v_H263_1998
             Value for p_VideoCoder.
   static Symbol v_H264
             Value for p_VideoCoder.
   static Symbol v_Inferred
             If Player/RecorderConstants.p_Fileformat is set to
   v_Inferred, it indicates that the MSC container type must be inferred
   from the file name extension (.wav -> v_WavFormat, etc..).
   static Symbol v_Linear16Bit_128k
             16bits, linear, 8000 samples/second, 128 kbits/s
   static Symbol v_Linear16Bit_256k
             16bits, linear, 16000 samples/second, 256kbits/s
   static Symbol v_Linear8Bit_64k
             8bits, linear, 8000 samples/second, 64kbits/s
   static Symbol v_MP4V_ES
             Value for p_VideoCoder.
   static Symbol v_MuLawPCM_64k
             Mulaw PCM (mimetype: pcmu)
   static Symbol v_RawFormat
             value for Player/RecorderConstants.p_FileFormat: basic file
   format without any header.
   static Symbol v_WavFormat
             value for Player/RecorderConstants.p_FileFormat: WAV file
   format.



   Field Detail

  v_ADPCM_32k

static final Symbol v_ADPCM_32k

          ADPCM
     _________________________________________________________________

  v_ADPCM_32kOKI

static final Symbol v_ADPCM_32kOKI

          ADPCM Oki, a.k.a. vox files
     _________________________________________________________________

  v_ADPCM_16kG726

static final Symbol v_ADPCM_16kG726

          G726 16k
     _________________________________________________________________

  v_ADPCM_32kG726

static final Symbol v_ADPCM_32kG726

          G726 32k
     _________________________________________________________________

  v_EVRC

static final Symbol v_EVRC

          EVRC coder
     _________________________________________________________________

  v_G729a

static final Symbol v_G729a

          G.729 codec
          Valid fmtp values, per rfc 3555, paragraph 4.1.9:

          + "annexb=yes" (default) or "annexb=no"

          Those values may be assigned to RecorderConstants.p_AudioFmtp.
     _________________________________________________________________

  v_G723_1b

static final Symbol v_G723_1b

          G.723 codec
          Valid fmtp values, per rfc 3555, paragraph 4.1.3:

          + "bitrate=6.3" or "bitrate="5.3"
          + "annexa=yes" (default) or "annexa=no"

          Those values may be assigned to RecorderConstants.p_AudioFmtp.
          (use a semicolon as separator)
     _________________________________________________________________

  v_GSM

static final Symbol v_GSM

          GSM coder
     _________________________________________________________________

  v_AMR_WB

static final Symbol v_AMR_WB

          AMR wideband
     _________________________________________________________________

  v_AMR

static final Symbol v_AMR

          AMR (narrow band)
     _________________________________________________________________

  v_ALawPCM_48k

static final Symbol v_ALawPCM_48k
     _________________________________________________________________

  v_ALawPCM_64k

static final Symbol v_ALawPCM_64k

          Alaw PCM (mimetype: pcma)
     _________________________________________________________________

  v_MuLawPCM_64k

static final Symbol v_MuLawPCM_64k

          Mulaw PCM (mimetype: pcmu)
     _________________________________________________________________

  v_Linear8Bit_64k

static final Symbol v_Linear8Bit_64k

          8bits, linear, 8000 samples/second, 64kbits/s
     _________________________________________________________________

  v_Linear16Bit_128k

static final Symbol v_Linear16Bit_128k

          16bits, linear, 8000 samples/second, 128 kbits/s
     _________________________________________________________________

  v_Linear16Bit_256k

static final Symbol v_Linear16Bit_256k

          16bits, linear, 16000 samples/second, 256kbits/s
     _________________________________________________________________

  v_RawFormat

static final Symbol v_RawFormat

          value for Player/RecorderConstants.p_FileFormat: basic file
          format without any header.

          When the v_RawFormat is required, the supported p_Coder values
          are v_ALawPCM_64k, v_MuLawPCM_64k, v_Linear16Bit_64k,
          v_ADPCM_32k, v_ADPCM_32kOKI, v_ADPCM_16kG726, v_ADPCM_32kG726
     _________________________________________________________________

  v_WavFormat

static final Symbol v_WavFormat

          value for Player/RecorderConstants.p_FileFormat: WAV file
          format.

          When the v_WavFormat is required, the supported p_Coder values
          are v_ALawPCM_64k, v_MuLawPCM_64k, v_Linear16Bit_64k,
          v_ADPCM_32k, v_ADPCM_32kOKI, v_G723_1b, v_G729a
     _________________________________________________________________

  v_GSMFormat

static final Symbol v_GSMFormat

          value for Player/RecorderConstants.p_Fileformat: GSM file
          format

          The only supported p_Coder value is v_GSM
     _________________________________________________________________

  v_3GPFormat

static final Symbol v_3GPFormat

          3gp files format. Default value inferred if the file extension
          is ".3gp"
     _________________________________________________________________

  v_3G2Format

static final Symbol v_3G2Format

          3gpp2 files format. Default value inferred if the file
          extension is ".3g2" or ".3gpp2"
     _________________________________________________________________

  v_Inferred

static final Symbol v_Inferred

          If Player/RecorderConstants.p_Fileformat is set to v_Inferred,
          it indicates that the MSC container type must be inferred from
          the file name extension (.wav -> v_WavFormat, etc..).

          If PlayerConstants.p_Coder is set to v_Inferred, it indicates
          that the encoding must be read from the file header.

          If RecorderConstants.p_Coder is set to v_Inferred, it indicates
          to use the encoding of the incoming stream.

          Note: In all 3 cases it is the default action anyway; this
          Symbol is mostly useful to cancel a previous explicit request
          for a specific format/codec.
     _________________________________________________________________

  v_H263

static final Symbol v_H263

          Value for p_VideoCoder. Indicates a H.263 (or H.263-1996) video
          encoding.
     _________________________________________________________________

  v_H263_1998

static final Symbol v_H263_1998

          Value for p_VideoCoder. Indicates a H.263-1998 video encoding.
     _________________________________________________________________

  v_MP4V_ES

static final Symbol v_MP4V_ES

          Value for p_VideoCoder. Indicates a MP4V-ES video encoding.
     _________________________________________________________________

  v_H264

static final Symbol v_H264

          Value for p_VideoCoder. Indicates a H.264 video encoding.
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
