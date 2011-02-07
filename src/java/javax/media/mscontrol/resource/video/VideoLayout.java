/*
 @(#) $RCSfile: VideoLayout.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:13 $ <p>
 
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
 
 Last Modified $Date: 2011-02-07 07:27:13 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

     _________________________________________________________________

   Overview  Package   Class  Use  Tree  Deprecated  Index  Help
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES     All Classes
   SUMMARY: NESTED | FIELD | CONSTR | METHOD
   DETAIL: FIELD | CONSTR | METHOD
     _________________________________________________________________

javax.media.mscontrol.resource.video
Interface VideoLayout
     _________________________________________________________________

public interface VideoLayout

   All video layout features are controlled by xml documents, currently
   SMIL is a proposed format.
   A VideoLayout java object can be marshalled/unmarshalled from/to an
   xml document.
   A MediaMixer can be configured to contain a VideoRenderer resource.
   This VideoRenderer can be given a VideoLayout to control the image
   that the mixer outputs.

   The VideoLayout describes:
     * Regions definitions, with positioning (x,y,z-index)
     * Regions contents, defined by URIs pointing to the actual media:
       (see below for the definition of media types)
          + Discrete media: text, logos, etc...
            As an exception, simple text can also be inlined instead of
            referenced through a URI. Use the data: scheme, as defined in
            RFC 2397.
          + Continuous media: video clips, animated GIF
          + Live media: They are defined by the URI of a MixerAdapter's
            stream. (This where the xml document is tied with JSR 309
            objects).

   In SMIL layouts, the regions are defined in the < head > of the
   document, while the contents are defined in the < body > .
   A VideoLayout is a non-mutable object (read-only).
     _________________________________________________________________

   Field Summary
   static java.net.URI anyActiveStream
             Virtual URI, representing any stream that is active.
   static java.net.URI anyStream
             A virtual URI, representing one of the incoming streams, at
   the mixer's choice.
   static java.lang.String mixerAdapterURIBase
             The basename of a MixerAdapter, for insertion in a
   VideoLayout document.
   static java.lang.String mixerStreamGroupURIBase

   static java.lang.String mixerURIBase

   static java.net.URI mostActiveStream
             Virtual URI, representing the stream of the most active
   input.



   Method Summary
    java.lang.String getType()
             A string to describe the layout.
    java.lang.String marshall()



   Field Detail

  mixerURIBase

static final java.lang.String mixerURIBase

        See Also:
                Constant Field Values
     _________________________________________________________________

  mixerStreamGroupURIBase

static final java.lang.String mixerStreamGroupURIBase

        See Also:
                Constant Field Values
     _________________________________________________________________

  mixerAdapterURIBase

static final java.lang.String mixerAdapterURIBase

          The basename of a MixerAdapter, for insertion in a VideoLayout
          document.
          For example, the URI of a MixerAdapter, whose ID is
          "chairmanAdapter", can be computed with:

 java.lang.String chairURI = VideoLayout.mixerAdapterURIBase+"chairmanAdapter";


          This string can then be inserted in a VideoLayout document:

 stringBuffer.append("< ref region=\"chairmanWindow\" src=\"" + chairURI + "\"
/>");


        See Also:
                Constant Field Values
     _________________________________________________________________

  mostActiveStream

static final java.net.URI mostActiveStream

          Virtual URI, representing the stream of the most active input.
          If the Mixer cannot determine a most active, it should fall
          back to a anyActiveStream This URI should never cause a
          duplicate display: if the most active stream happens to be
          already explicitly referenced and displayed in another region,
          then the media server should pick an alternate stream.
     _________________________________________________________________

  anyActiveStream

static final java.net.URI anyActiveStream

          Virtual URI, representing any stream that is active. If the
          Mixer cannot determine an active stream, it should fall back to
          a anyStream. The exact selection is at the mixer's choice. This
          URI should never cause a duplicate display: the most active
          stream and the explicit URIs are valid candidates only if they
          are not already displayed.
     _________________________________________________________________

  anyStream

static final java.net.URI anyStream

          A virtual URI, representing one of the incoming streams, at the
          mixer's choice. This URI should never cause a duplicate
          display: the most active stream, the anyActiveStream's and the
          explicit URIs are valid candidates only if they are not already
          displayed.

   Method Detail

  getType

java.lang.String getType()

          A string to describe the layout. The string is in two parts,
          colon-separated. The first part identifies a particular
          standard/namespace (e.g. "xcon"), and the second part is to be
          interpreted in the context of this standard/namespace.
          Examples:
          "xcon:dual-view-crop"
          "mediaserver:2D"
          If this object does not correspond to any layout described in a
          standard/namespace , this method returns an empty string.
     _________________________________________________________________

  marshall

java.lang.String marshall()

        Returns:
                A readable java.lang.String typically representing the xml
                definition of the VideoLayout.
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
package javax.jain.media.mscontrol.resource.video;
