// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
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

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES   NO FRAMES
     _________________________________________________________________

javax.megaco.message.descriptor
Class AuditValRespDescriptor

java.lang.Object
    |
    +--javax.megaco.message.Descriptor
            |
            +--javax.megaco.message.descriptor.AuditValRespDescriptor

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class AuditValRespDescriptor
   extends Descriptor

   The class extends JAIN MEGACO Descriptor. This class describes the
   audit value response descriptor. It specifies the tokens and
   descriptors for audit value response.

   See Also:
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   AuditValRespDescriptor()
              Constructs an Audit Value Response Descriptor object. This
   extends the Descriptor class. It defines the Audit Return Parameter of
   the Megaco Protocol for Audit Value Response.

   Method Summary
    Descriptor[] getDescriptor()
              Gets the Descriptor information for all the descriptor for
   which it has been audited. Only valid descriptors are
    1. Media Descriptor
    2. Modem Descriptor
    3. Mux Descriptor
    4. Events Descriptor
    5. Signal Descriptor
    6. Digit Map Descriptor
    7. Observed Events Descriptor
    8. Event Buffer Descriptor
    9. Statistics Descriptor
   10. Package Descriptor
   11. Error Descriptor

    void setDescriptor (Descriptor[] descriptorInfo)
             Sets the vector of Descriptor Information for which it has
   been audited. Only valid descriptors are
    1. Media Descriptor
    2. Modem Descriptor
    3. Mux Descriptor
    4. Events Descriptor
    5. Signal Descriptor
    6. Digit Map Descriptor
    7. Observed Events Descriptor
    8. Event Buffer Descriptor
    9. Statistics Descriptor
   10. Package Descriptor
   11. Error Descriptor

    boolean isMuxTokenPresent()
              Returns TRUE if the Mux token is present.
    void setMuxToken()
              Sets flag to indicate that the Mux token is present.
    boolean isModemTokenPresent()
              Returns TRUE if the Modem token is present.
    void setModemToken()
              Sets flag to indicate that the Modem token is present.
    boolean isMediaTokenPresent()
              Returns TRUE if the Media token is present.
    void setMediaToken()
              Sets flag to indicate that the Media token is present.
    boolean isSignalTokenPresent()
              Returns TRUE if the Signal token is present.
    void setSignalToken()
              Sets flag to indicate that the Signal token is present.
    boolean isEventBuffTokenPresent()
              Returns TRUE if the Event Buffer token is present.
    void setEventBuffToken()
              Sets flag to indicate that the Event Buffer token is
   present.
    boolean isDigitMapTokenPresent()
              Returns TRUE if the digit map token is present.
    void setDigitMapToken()
              Sets flag to indicate that the Digit Map token is present.
    boolean isStatsTokenPresent()
              Returns TRUE if the Statistics token is present.
    void setStatsToken()
              Sets flag to indicate that the Statistics token is present.
    boolean isEventsTokenPresent()
              Returns TRUE if the Events token is present.
    void setEventsToken()
              Sets flag to indicate that the Events token is present.
    boolean isObsEventTokenPresent()
              Returns TRUE if the Observed Event token is present.
    void setObsEventToken()
              Sets flag to indicate that the Observed Event token is
   present.
    boolean isPackagesTokenPresent()
              Returns TRUE if the Packages token is present.
    void setPackagesToken()
              Sets flag to indicate that the Packages token is present.

   Methods inherited from class javax.megaco.message.Descriptor
   getDescriptorId,

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  AuditValRespDescriptor

public AuditValRespDescriptor()

          Constructs a Audit Value Response Descriptor. It specifies the
          tokens for which the audit value is required.
     _________________________________________________________________

   Method Detail

  getDescriptorId

public final int getDescriptorId()

          This method cannot be overridden by the derived class. This
          method returns that the descriptor identifier is of type audit
          value response descriptor. This method overrides the
          corresponding method of the base class Descriptor.


        Returns:
                Returns an integer value that identifies this object of
                the type of audit value response descriptor. It returns
                that it is audit value response Descriptor i.e.,
                M_AUDIT_VAL_REPLY_DESC.
     _________________________________________________________________

  getDescriptor

public Descriptor[] getDescriptor()

          Gets the Descriptor information for all the descriptor in this
          audit response parameter.


        Returns:
                The vector of the reference to the object identifier of
                type descriptor information.
     _________________________________________________________________

  setDescriptor

public void setDescriptor(Descriptor[] descriptor)

          Sets the vector of Descriptor Information for this audit
          response parameter. Only valid descriptors are

    1. Media Descriptor
    2. Modem Descriptor
    3. Mux Descriptor
    4. Events Descriptor
    5. Signal Descriptor
    6. Digit Map Descriptor
    7. Observed Events Descriptor
    8. Event Buffer Descriptor
    9. Statistics Descriptor
   10. Package Descriptor
   11. Error Descriptor


   Parameters:
          The vector of reference to the object identifier of type
          descriptor information.
     _________________________________________________________________

  isMuxTokenPresent

public final boolean isMuxTokenPresent()

          This method cannot be overridden by the derived class. This
          method indicates if the mux token is present or not.


        Returns:
                Returns TRUE if the Mux token is present.
     _________________________________________________________________

  setMuxToken

public final void setMuxToken()

          This method cannot be overridden by the derived class. This
          method sets a flag to indicate that the mux token is present.
     _________________________________________________________________

  isModemTokenPresent

public final boolean isModemTokenPresent()

          This method cannot be overridden by the derived class. This
          method indicates if the Modem token is present or not.


        Returns:
                Returns TRUE if the Modem token is present.
     _________________________________________________________________

  setModemToken

public final void setModemToken()

          This method cannot be overridden by the derived class. This
          method sets a flag to indicate that the Modem token is present.
     _________________________________________________________________

  isMediaTokenPresent

public final boolean isMediaTokenPresent()

          This method cannot be overridden by the derived class. This
          method indicates if the Media token is present or not.


        Returns:
                Returns TRUE if the Media token is present.
     _________________________________________________________________

  setMediaToken

public final void setMediaToken()

          This method cannot be overridden by the derived class. This
          method sets a flag to indicate that the Media token is present.
     _________________________________________________________________

  isSignalTokenPresent

public final boolean isSignalTokenPresent()

          This method cannot be overridden by the derived class. This
          method indicates if the Signal token is present or not.


        Returns:
                Returns TRUE if the Signal token is present.
     _________________________________________________________________

  setSignalToken

public final void setSignalToken()

          This method cannot be overridden by the derived class. This
          method sets a flag to indicate that the Signal token is
          present.
     _________________________________________________________________

  isEventBuffTokenPresent

public final boolean isEventBuffTokenPresent()

          This method cannot be overridden by the derived class. This
          method indicates if the Event Buffer token is present or not.


        Returns:
                Returns TRUE if the Event Buffer token is present.
     _________________________________________________________________

  setEventBuffToken

public final void setEventBuffToken()

          This method cannot be overridden by the derived class. This
          method sets a flag to indicate that the Event Buffer token is
          present.
     _________________________________________________________________

  isDigitMapTokenPresent

public final boolean isDigitMapTokenPresent()

          This method cannot be overridden by the derived class. This
          method indicates if the digit map token is present or not.


        Returns:
                Returns TRUE if the digit map token is present.
     _________________________________________________________________

  setDigitMapToken

public final void setDigitMapToken()

          This method cannot be overridden by the derived class. This
          method sets a flag to indicate that the digit map token is
          present.
     _________________________________________________________________

  isStatsTokenPresent

public final boolean isStatsTokenPresent()

          This method cannot be overridden by the derived class. This
          method indicates if the Statistics token is present or not.


        Returns:
                Returns TRUE if the Statistics token is present.
     _________________________________________________________________

  setStatsToken

public final void setStatsToken()

          This method cannot be overridden by the derived class. This
          method sets a flag to indicate that the Statistics token is
          present.
     _________________________________________________________________

  isEventsTokenPresent

public final boolean isEventsTokenPresent()

          This method cannot be overridden by the derived class. This
          method indicates if the Events token is present or not.


        Returns:
                Returns TRUE if the Events token is present.
     _________________________________________________________________

  setEventsToken

public final void setEventsToken()

          This method cannot be overridden by the derived class. This
          method sets a flag to indicate that the Events token is
          present.
     _________________________________________________________________

  isObsEventTokenPresent

public final boolean isObsEventTokenPresent()

          This method cannot be overridden by the derived class. This
          method indicates if the Observed Event token is present or not.


        Returns:
                Returns TRUE if the Observed Event token is present.
     _________________________________________________________________

  setObsEventToken

public final void setObsEventToken()

          This method cannot be overridden by the derived class. This
          method sets a flag to indicate that the Observed Event token is
          present.
     _________________________________________________________________

  isPackagesTokenPresent

public final boolean isPackagesTokenPresent()

          This method cannot be overridden by the derived class. This
          method indicates if the Packages token is present or not.


        Returns:
                Returns TRUE if the Packages token is present.
     _________________________________________________________________

  setPackagesToken

public final void setPackagesToken()

          This method cannot be overridden by the derived class. This
          method sets a flag to indicate that the Packages token is
          present.
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMES   NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR |  METHOD DETAIL:  FIELD | CONSTR |
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
