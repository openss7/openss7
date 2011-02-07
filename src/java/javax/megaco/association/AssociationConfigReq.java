// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: AssociationConfigReq.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:15 $ <p>
 
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
 
 Last Modified $Date: 2011-02-07 07:27:15 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco.association
Class AssociationConfigReq

java.lang.Object
  |
  +--java.util.EventObject
        |
        +--javax.megaco.association.AssociationEvent
                 |
                 +--javax.megaco.AssociationConfigReq

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public class AssociationConfigReq
   extends AssociationEvent

   The class extends JAIN MEGACO Association Events. This method is used
   for configuring the association related information. If this API is
   not invoked by application, stack would assume the default values for
   the configuration parameters.

   See Also:
          Serialized Form
     _________________________________________________________________

   Fields inherited from class java.util.EventObject
   source

   Constructor Summary
   AssociationConfigReq (java.lang.Object source, int assocHandle)
             Constructs a Association configuration Association Event
   object.

   Method Summary
   int getMaxRegstRetries()
       throws javax.megaco.ParameterNotSetException
              Returns the value of the maximum registration retries that
   is to be configured.
    void  setMaxRegstRetries(int regstRetries)
       throwsjavax.megaco.InvalidArgumentException

              Sets the valid value of the maximum registration retries.
   This method automatically sets the maximum registration retries to
   true.
    boolean isMaxRegstRetriesPresent()
             Returns true if the maximum registration retries is
   configured.
    int  getRestartAvalanceMWD()
       throws javax.megaco.ParameterNotSetException
              Returns the value of the MWD used during restart avalance
   prevention that is to be configured.
    void  setRestartAvalanceMWD(int mwd)
       throwsjavax.megaco.InvalidArgumentException

              Sets the valid value of the MWD used during restart
   avalance prevention. This method automatically sets the MWD used
   during restart avalance prevention to true.
    boolean isRestartAvalanceMWDPresent()
             Returns true if the MWD used during restart avalance
   prevention is configured.
    int  getNormalMGExecTime()
       throws javax.megaco.ParameterNotSetException
              Returns the value of the normal MG execution time that is
   to be configured. This is used for calculating the retransmission
   timer.
    void  setNormalMGExecTime(int time)
       throwsjavax.megaco.InvalidArgumentException

              Sets the valid value of the normal MG execution time. This
   method automatically sets the normal MG execution time to true.
    boolean isNormalMGExecTimePresent()
             Returns true if the normal MG execution time is configured.
    int  getNormalMGCExecTime()
       throws javax.megaco.ParameterNotSetException
              Returns the value of the normal MGC execution time that is
   to be configured. This is used for calculating the retransmission
   timer.
    void  setNormalMGCExecTime(int time)
       throwsjavax.megaco.InvalidArgumentException

              Sets the valid value of the normal MGC execution time. This
   method automatically sets the normal MGC execution time to true.
    boolean isNormalMGCExecTimePresent()
             Returns true if the normal MGC execution time is
   configured.
    int  getProvisionalRespTime()
       throws javax.megaco.ParameterNotSetException
              Returns the value of the provisional response time that is
   to be configured. This is used for calculating the pending generation
   timer.
    void
   setProvisionalRespTime(int time)
       throwsjavax.megaco.InvalidArgumentException
              Sets the valid value of the normal MGC execution time. This
   method automatically sets the provisional response time to true.
    boolean isProvisionalRespTimePresent()
             Returns true if the provisional response time is
   configured.
    int  getResponseRetentionTime()
       throws javax.megaco.ParameterNotSetException
              Returns the value of the time during which the transaction
   is to be retained that is to be configured. This is used for
   calculating the time after receipt of response during which the recipt
   of same transaction request would imply a retransmission of the
   previous request.
    void  setResponseRetentionTime(int time)
       throwsjavax.megaco.InvalidArgumentException

              Sets the valid value of the response retention time. This
   method automatically sets response retention time to true.
    boolean isResponseRetentionTimePresent()
             Returns true if the response retention timer is configured.
    int  getMaxPndngRespCount()
       throws javax.megaco.ParameterNotSetException
              Returns the value of the maximum pending response count
   that is to be configured. This is used for find the number of pending
   message that can be sent before stack assumes that the application
   cannot get back a response for the same.
    void  setMaxPndngRespCount(int time)
       throwsjavax.megaco.InvalidArgumentException

              Sets the valid value of the maximum response pending count,
   beyond which the MG/MGC stack assume that the application cannot send
   back the response. This method automatically sets the maximum pending
   response count to true.
    boolean isMaxPndngRespCountPresent()
             Returns true if the maximum pending response count is
   configured.
    int  getMaxRetransmissionCount()
       throws javax.megaco.ParameterNotSetException
              Returns the value of the maximum retransmission count that
   is to be configured. This is used to find the number of times a
   message can be retransmitted before assuming that there is a network
   related problem due to which the message could not be forwarded to the
   peer.
    void  setMaxRetransmissionCount(int time)
       throwsjavax.megaco.InvalidArgumentException

              Sets the valid value of the maximum retransmission count.
   This method automatically sets the maximum retransmission count to
   true.
    boolean isMaxRetransmissionCountPresent()
             Returns true if the maximum retransmission count is
   configured.
    int  getInitialRTT()
       throws javax.megaco.ParameterNotSetException
              Returns the value of the initial value of RTT that is to be
   configured. This is used to calculate the retransmission timer.
    void  setInitialRTT(int time)
       throwsjavax.megaco.InvalidArgumentException

              Sets the valid value of the initial RTT. This method
   automatically sets the initial value of RTT to true.
    boolean isInitialRTTPresent()
             Returns true if the initial value of RTT is configured.
    int  getProtocolVersion()
       throws javax.megaco.ParameterNotSetException
              Returns the protocol version configured for use in
   association creation to peer.
    void  setProtocolVersion(int version)
       throwsjavax.megaco.InvalidArgumentException

              Sets the valid value of the protocol version. This method
   automatically sets the protocol version Present to true.
    boolean isProtocolVersionPresent()
             Returns true if the protocol version is set.
    int getEncodingFormat()
              Returns the encoding format configured for use encoding
   towards peer. If not encoding format is set, it shall return value set
   to 0.
    void  setEncodingFormat(EncodingFormat format)
       throwsjavax.megaco.InvalidArgumentException

              Sets the valid value of the encoding format.
    int getTransportType()
              Returns the transport type configured for use od sending
   message towards peer. If transport type is not set, it shall be
   returned as null value.
    void  setTransport Type(TransportType transportType)
       throwsjavax.megaco.InvalidArgumentException

              Sets the valid value of the transport type.
    java.lang.String  getSrvChngProfile()
              Returns the service change profile to be exchanged in the
   service change to peer. This is to be used only at MG side
   configuration.
    void  setSrvChngProfile(java.lang.String profile)
       throwsjavax.megaco.InvalidArgumentException

              Sets the valid value of the service change profile. This
   method returns a NULL value when service change profile is not set.

   Methods inherited from class javax.megaco. AssociationEvent
   getAssocHandle, getAssocOperIdentifier

   Methods inherited from class java.util.EventObject
   getSource, toString

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait

   Constructor Detail

  AssociationConfigReq

public AssociationConfigReq(java.lang.Object source,
                     int assocHandle)

          Constructs a Config Association Association Event object.


        Parameters:
                source - A reference to the object, the "source", that is
                logically deemed to be the object upon which the Event in
                question initially occurred.
                assocHandle - The association handle to uniquely identify
                the MG-MGC pair. This is allocated by the stack when the
                Listener registers with the provider with a unique MG-MGC
                identity.

   Method Detail

  getAssocOperIdentifier

public final int getAssocOperIdentifier()

          This method returns that the event object as association
          configuration event This method overrides the corresponding
          method of the base class AssociationEvent.


        Returns:
                Returns an integer value that identifies this event
                object as the type of association configuration event.
                This is defined as M_ASSOC_CONFIG_REQ in AssocEventType.
     _________________________________________________________________

  getMaxRegstRetries

   public int getMaxRegstRetries()
       throws javax.megaco.ParameterNotSetException

                Gets the integer value of maximum registration retries.


              Returns:
                      Returns the integer value of maximum registration
                      retries.

              Throws:
                      ParameterNotSetException : This exception is raised
                      if the max registration retires has not been
                      specified.
            __________________________________________________________

  setMaxRegstRetries

public void setMaxRegstRetries(int regstRetries)

              throwsjavax.megaco.InvalidArgumentException

                This method sets the maximum registration retries which
                is to be configured. This is the maximum number of times
                the MG needs to try to register with one MGC before
                moving on to the next.


              Parameter:
                      The integer value of maximum registration retries.


              Throws:
                      InvalidArgumentException : This exception is raised
                      if the value of registration retires passed to this
                      method is less than 0.


            __________________________________________________________

  isMaxRegstRetriesPresent

public boolean isMaxRegstRetriesPresent()

                Identifies whether the maximum registration retries is
                present.


              Returns:
                      Returns true if the maximum registration retries is
                      present.
            __________________________________________________________

  getRestartAvalanceMWD

          public int getRestartAvalanceMWD()
              throws javax.megaco.ParameterNotSetException

                  Gets the integer value of MWD used during restart
          avalance prevention.



              Returns:
                      Returns the integer value of MWD used during
                      restart avalance prevention.

              Throws:
                      ParameterNotSetException : This exception is raised
                      if the restart avalanche MWD has not been
                      specified.
            __________________________________________________________

  setRestartAvalanceMWD

          public void setRestartAvalanceMWD(int restartAvalancheTime)

          throws javax.megaco.InvalidArgumentException


          This method sets the MWD used during restart avalance
          prevention which is to be configured. This sets the Maximum
          Wait Delay parameter required to prevent the restart avalanche.


        Parameter:
                The integer value of MWD used during restart avalance
                prevention .

        Throws:
                InvalidArgumentException : This exception is raised if
                the value of restart avalanche time passed to this method
                is less than 0.
     _________________________________________________________________

  isRestartAvalanceMWDPresent

public boolean isRestartAvalanceMWDPresent()

          Identifies whether the MWD used during restart avalance
          prevention is present.


        Returns:
                Returns true if the MWD used during restart avalanche
                prevention is present.
   ______________________________________________________________________

  getNormalMGExecTime

          public int getNormalMGExecTime()
              throws javax.megaco.ParameterNotSetException

          Gets the integer value of normal MG execution time.


        Returns:
                Returns the integer value of normal MG execution time.

        Throws:
                ParameterNotSetException : This exception is raised if
                the MG Normal execution value has not been specified.
     _________________________________________________________________

  setNormalMGExecTime

public void setNormalMGExecTime(int mgExecTime)

              throwsjavax.megaco.InvalidArgumentException


          This method sets the normal MG execution time which is to be
          configured. This sets execution time beyond whih MG retransmits
          the message.


        Parameter:
                The integer value of normal MG execution time.

        Throws:
                InvalidArgumentException : This exception is raised if
                the value of normal MG execution time passed to this
                method is less than 0.
     _________________________________________________________________

  isNormalMGExecTimePresent

public boolean isNormalMGExecTimePresent()

          Identifies whether the normal MG execution time is present.


        Returns:
                Returns true if the normal MG execution time is present
                is present.
     _________________________________________________________________

  getNormalMGCExecTime

          public int getNormalMGCExecTime()
              throws javax.megaco.ParameterNotSetException

          Gets the integer value of normal MGC execution time.


        Returns:
                Returns the integer value of normal MGC execution time.

        Throws:
                ParameterNotSetException : This exception is raised if
                the MGC Normal execution value has not been specified.
     _________________________________________________________________

  setNormalMGCExecTime

          public void setNormalMGCExecTime(int mgcExecTime)

              throws javax.megaco.InvalidArgumentException


          This method sets the normal MGC execution time which is to be
          configured. This sets execution time beyond whih MGC
          retransmits the message.


        Parameter:
                The integer value of normal MGC execution time.

        Throws:
                InvalidArgumentException : This exception is raised if
                the value of normal MGC execution time passed to this
                method is less than 0.
     _________________________________________________________________

  isNormalMGCExecTimePresent

public boolean isNormalMGCExecTimePresent()

          Identifies whether the normal MGC execution time is present.


        Returns:
                Returns true if the normal MGC execution time is present
                is present.
     _________________________________________________________________

  getProvisionalRespTime

          public int getProvisionalRespTime()
              throws javax.megaco.ParameterNotSetException

          Gets the integer value of provisional response timer.


        Returns:
                Returns the integer value of provisional response timer.

        Throws:
                ParameterNotSetException : This exception is raised if
                the provisional response time has not been specified.
     _________________________________________________________________

  setProvisionalRespTime

public void setProvisionalRespTime(int provRespTime)

              throwsjavax.megaco.InvalidArgumentException


          This method sets the provisional response timer which is to be
          configured. This sets the time when the provisional response
          has to be sent by the stack, if the application does not send
          back a response by that time.


        Parameter:
                The integer value of provisional response timer.

        Throws:
                InvalidArgumentException : This exception is raised if
                the value of provisional response time passed to this
                method is less than 0.
     _________________________________________________________________

  isProvisionalRespTimePresent

public boolean isProvisionalRespTimePresent()

          Identifies whether the provisional response timer is present.


        Returns:
                Returns true if the provisional response timer is present
                is present.
     _________________________________________________________________

  getResponseRetentionTime

          public int getResponseRetentionTime()
              throws javax.megaco.ParameterNotSetException




          Gets the integer value of response Retention time.



        Returns:
                Returns the integer value of response Retention time.

        Throws:
                ParameterNotSetException : This exception is raised if
                the response retention time has not been specified.
     _________________________________________________________________

  setResponseRetentionTime

public void setResponseRetentionTime(int responseTime)

              throwsjavax.megaco.InvalidArgumentException


          This method sets the response retention time which is to be
          configured. This sets the Maximum time during which the
          transaction has to be retained after the receipt of the
          response.


        Parameter:
                The integer value of response retention time.

        Throws:
                InvalidArgumentException : This exception is raised if
                the value of response retention time passed to this
                method is less than 0.
     _________________________________________________________________

  isResponseRetentionTimePresent

public boolean isResponseRetentionTimePresent()

          Identifies whether the response retention time is present.


        Returns:
                Returns true if the response retention time is present is
                present.
     _________________________________________________________________

  getMaxPndngRespCount

          public int getMaxPndngRespCount()
              throws javax.megaco.ParameterNotSetException


          Gets the integer value of maximum pending response count.


        Returns:
                Returns the integer value of maximum pending response
                count.

        Throws:
                ParameterNotSetException : This exception is raised if
                the value of maximum pending response count has not been
                specified.
     _________________________________________________________________

  setMaxPndngRespCount

public void setMaxPndngRespCount(int pndngCount)

              throwsjavax.megaco.InvalidArgumentException


          This method sets the maximum pending response count which is to
          be configured. This sets the count, which gives the maximum
          provisional responses beyond which the stack shall assume that
          the application could not send the message back. The stack in
          this case will generate a error response towards the peer.


        Parameter:
                The integer value of maximum pending response count.

        Throws:
                InvalidArgumentException : This exception is raised if
                the value of pending response count passed to this method
                is less than 0.
     _________________________________________________________________

  isMaxPndngRespCountPresent

public boolean isMaxPndngRespCountPresent()

          Identifies whether the maximum pending response count is
          present.


        Returns:
                Returns true if the maximum pending response count is
                present is present.
     _________________________________________________________________

  getMaxRetransmissionCount

          public int getMaxRetransmissionCount()
              throws javax.megaco.ParameterNotSetException

          Gets the integer value of maximum retransmission count.


        Returns:
                Returns the integer value of maximum retransmission
                count.

        Throws:
                ParameterNotSetException : This exception is raised if
                the value of maximum retransmission counthas not been
                specified.
     _________________________________________________________________

  setMaxRetransmissionCount

public void setMaxRetransmissionCount(int retransmissionCount)

              throwsjavax.megaco.InvalidArgumentException


          This method sets the maximum retransmission count which is to
          be configured. This sets the count, which gives the maximum
          retransmission count, beyond which if stack still does not get
          back a response from peer, the stack shall assume a network
          problem with the peer.


        Parameter:
                The integer value of maximum retransmission count.

        Throws:
                InvalidArgumentException : This exception is raised if
                the value of retransmission count passed to this method
                is less than 0.
     _________________________________________________________________

  isMaxRetransmissionCountPresent

public boolean isMaxRetransmissionCountPresent()

          Identifies whether the maximum retransmission count is present.


        Returns:
                Returns true if the maximum retransmission count is
                present.
     _________________________________________________________________

  getInitialRTT

          public int getInitialRTT()
              throws javax.megaco.ParameterNotSetException

          Gets the integer value of initial rtt.


        Returns:
                Returns the integer value of initial rtt.

        Throws:
                ParameterNotSetException : This exception is raised if
                the value of initial rtt has not been specified.
     _________________________________________________________________

  setInitialRTT

public void setInitialRTT(int rtt)

              throwsjavax.megaco.InvalidArgumentException


          This method sets the initial rtt which is to be configured.
          This is used for calculating the retransmission timer.


        Parameter:
                The integer value of initial rtt.

        Throws:
                InvalidArgumentException : This exception is raised if
                the value of initial RTT passed to this method is less
                than 0.
     _________________________________________________________________

  isInitialRTTPresent

public boolean isInitialRTTPresent()

          Identifies whether the initial rtt is present.


        Returns:
                Returns true if the initial rtt is present.
     _________________________________________________________________

  getEncodingFormat

public int getEncodingFormat()

          Gets the object reference to the encoding format. The possible
          values are field constants defined for the class
          EncodingFormat.


        Returns:
                Returns the a derived object of the EncodingFormat class
                to represent the value of encoding format. If the
                encoding format has not been specified, then this method
                would return value 0.
     _________________________________________________________________

  setEncodingFormat

public void setEncodingFormat(EncodingFormat format)

              throwsjavax.megaco.InvalidArgumentException


          This method sets the encoding format which is to be configured.


        Parameter:
                The object reference to derived class of EncodingFormat
                class which gives value of encoding format .

        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of encoding format passed to this method is
                NULL.
     _________________________________________________________________

  getTransportType

public int getTransportType()

          Gets the integer value which identifies the transport type. The
          possible values are the field constants defined for the class
          TransportType.


        Returns:
                Returns the object reference to transport type. If no
                transport type has been specified, then this method would
                return value 0.
     _________________________________________________________________

  setTransportType

public void setTransportType(TransportType transport)

              throwsjavax.megaco.InvalidArgumentException


          This method sets the transport type which is to be configured.


        Parameter:
                The object refenrece to transport type.


        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of Transport type passed to this method is
                NULL.
     _________________________________________________________________

  getProtocolVersion

          public int getProtocolVersion()
              throws javax.megaco.ParameterNotSetException

          Gets the protocol version value received from peer in the
          service change. This is ther protocol version after
          negotiation.


        Returns:
                Returns the protocol version parameter as an integer
                value.

        Throws:
                ParameterNotSetException : This exception is raised if
                the service change delay has not been specified.
     _________________________________________________________________

  setProtocolVersion

public void setProtocolVersion(int version)

              throwsjavax.megaco.InvalidArgumentException


          This method sets the protocol version value that is to be used
          for the specified association.


        Parameter:
                The protocol version as an integer value.

        Throws:
                InvalidArgumentException : This exception is raised if
                the value of protocol version passed to this method is
                less than 0.
     _________________________________________________________________

  isProtocolVersionPresent

public boolean isProtocolVersionPresent()

          Identifies whether the protocol version is present.


        Returns:
                Returns true if the protocol version is present.
     _________________________________________________________________

  getSrvChngProfile

public java.lang.String getSrvChngProfile()

          Gets the service change profile value to be sent to peer in the
          service change.


        Returns:
                Returns the service change profile parameter as a string
                value. If service change profile is not set then a NULL
                value is returned.
     _________________________________________________________________

  setSrvChngProfile

public void setSrvChngProfile(java.lang.String profile)

              throwsjavax.megaco.InvalidArgumentException


          This method sets the service change profile value.


        Parameter:
                The service change profile as a string value.


        Throws:
                InvalidArgumentException : This exception is raised if
                the reference of service change profile passed to this
                method is NULL.
     _________________________________________________________________
     _________________________________________________________________





   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR |  METHOD DETAIL:  FIELD | CONSTR |
   METHOD
            __________________________________________________________

                  Copyright (C) 2001 Hughes Software Systems
       HUGHES SOFTWARE SYSTEMS and JAIN JSPA SIGNATORIES PROPRIETARY
        This document contains proprietary information that shall be
     distributed, routed or made available only within Hughes Software
       Systems and JAIN JSPA Signatory Companies, except with written
                   permission of Hughes Software Systems
                   ____________________________________________

          22 December 2003


    If you have any comments or queries, please mail them to
    Jmegaco_hss@hss.hns.com
*/
