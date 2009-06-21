// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: ErrorCode.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:33 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:33 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco
Class ErrorCode

java.lang.Object
  |
  +--javax.megaco.ErrorCode

   All Implemented Interfaces:
          java.io.Serializable

   Direct Known Subclasses:
             None
     _________________________________________________________________

   public class ErrorCode
   extends java.lang.Object

   This class defines the error codes which can be returned in the
   response events from stack. These error codes would help the
   application in identifying the granularity of the error occured.

   See Also:
          Serialized Form
     _________________________________________________________________

   Field Summary
   static int M_PEER_ENTITY_UNVBLE
              Identifies that the network connection with local address
   could not be created since the peer entity is unavailable.
   static int M_INV_LOCAL_TPT_ADDRESS
              Identifies that the network connection with local address
   could not be created since the transport address specified was
   invalid.
   static int M_INV_REMOTE_TPT_ADDRESS
              Identifies that the network connection with remote address
   could not be created since the transport address specified was
   invalid.
   static int M_INV_LCL_RMT_ADDR_PAIR
              Identifies that the combination of local and remote address
   is invalid. This will come in case of difference in the transport type
   of local and remote address.
   static int M_INV_ASSOC_ID
              Identifies that no association exists with the specified
   association Id for the corresponding listener instance.
   static int M_INV_SVC_CHNG_METH_REASON_PAIR
              Identifies invalid combination of service change method and
   service change reason.
   static int M_UNSPTD_ENC_FORMAT
              Identifies that the encoding format as requested by the
   application is not supported at the stack.
   static int M_UNSPTD_TPT_TYPE
              Identifies that the Transport Type as requested by the
   application is not supported at the stack.
   static int M_INV_SVC_CHNG_ADDR
              Identifies that the service change address parameter
   specified is invalid. This would occur if the transport type specified
   in service change address field is different than specified in local
   address field.
   static int M_INV_HOFF_MGCID
              Identifies that the Handed Off MGCId parameter specified is
   invalid. This would occur if the transport type specified in MGCId
   field is different than specified in local address field.
   static int M_UNSPTD_SVC_CHNG_METH_EXTN
              Identifies that the service change extension method as
   requested by the application is not supported at the stack.
   static int M_INV_SVC_CHNG_REASON
              Identifies that the service change reason field is invalid
   for the requested event.
   static int M_INV_SVC_CHNG_METHOD
              Identifies that the service change method field is invalid
   for the requested event.
   static int M_RES_UNVBLE
              Identifies this the stack when it is unable to process an
   event requested because of unavailability of system resources such as
   memory and timer.
   static int M_MODIFY_ENC_FORMAT_FAILURE
              Identifies that modify encoding format failed since the
   association was already in use. The modify encoding format opeartion
   is permitted only when the association with the peer is not
   established.
   static int M_MODIFY_TPT_TYPE_FAILURE
              Identifies that modify Transport type failed since the
   association was already in use. The modify transport type opeartion is
   permitted only when the association with the peer is not established.
   static int M_MODIFY_SVC_PROFILE_FAILURE
              Identifies that modify Service Change profile failed since
   the association was already in use. The modify service change profile
   opeartion is permitted only when the association with the peer is not
   established.
   static int M_MODIFY_PROTOCOL_VERSION_FAILURE
              Identifies that modify protocol version failed since the
   association was already in use. The modify protocol version opeartion
   is permitted only when the association with the peer is not
   established.
   static int M_INV_ACTION_HANDLE
              Identifies that the action handle as specified in the
   command event is invalid. This wuuld occur if the second command is
   added in a transaction with action handle different than the previous
   command and isFirstCommandinAction flag is not set to TRUE or this
   would occur if the action handle specified in the command response
   event is not same as the action handle in the request event.
   static int M_WLDCRD_CMD_NOT_SUPTD
              Identifies that stack does not support wildcarded command
   request event.
   static int M_INV_CMD_REQID
              Identifies that the command request identifier is invalid.
   This would occur if for example Notify command request event is sent
   to stack.
   static int M_INV_CMD_RESPID
              Identifies that the command response identifier is invalid.
   This would occur if for example Modify command response event is sent
   to stack for ADD command request.
   static int M_TERM_TYPE_NOT_INSYNCH
              Identifies that the as specified in the command request
   event is not in synch with the term type in the command response
   event. This would occur if the term type specified in the command
   request is M_TERM_TYPE_NORMAL and term type specified in the response
   is M_TERM_TYPE_ROOT.
   static int M_INV_TERM_NAME_FORMAT
              Identifies that the Term Name format is different than the
   encoding format for this assciation. This would occur if the
   termination name specified is in the binary format whereas the
   encoding format supported by the association is TEXT.
   static int M_TOPOLOGY_DESC_NOT_SUPTD
              Identifies that the stack does not support topology
   descriptor. This may occur in customised implimentations which do not
   support this descriptor.
   static int M_MEDIA_DESC_NOT_SUPTD
              Identifies that the stack does not support media
   descriptor. This may occur in customised implimentations which do not
   support this descriptor.
   static int M_MODEM_DESC_NOT_SUPTD
              Identifies that the stack does not support modem
   descriptor. This may occur in customised implimentations which do not
   support this descriptor.
   static int M_MUX_DESC_NOT_SUPTD
              Identifies that the stack does not support mux descriptor.
   This may occur in customised implimentations which do not support this
   descriptor.
   static int M_EVENT_DESC_NOT_SUPTD
              Identifies that the stack does not support event
   descriptor. This may occur in customised implimentations which do not
   support this descriptor.
   static int M_SIGNAL_DESC_NOT_SUPTD
              Identifies that the stack does not support signal
   descriptor. This may occur in customised implimentations which do not
   support this descriptor.
   static int M_DIGITMAP_DESC_NOT_SUPTD
              Identifies that the stack does not support digit map
   descriptor. This may occur in customised implimentations which do not
   support this descriptor.
   static int M_EVENTBUFF_DESC_NOT_SUPTD
              Identifies that the stack does not support event buffer
   descriptor. This may occur in customised implimentations which do not
   support this descriptor.
   static int M_AUDIT_DESC_NOT_SUPTD
              Identifies that the stack does not support audit
   descriptor. This may occur in customised implimentations which do not
   support this descriptor.
   static int M_OBSRD_EVENT_DESC_NOT_SUPTD
              Identifies that the stack does not support observed event
   descriptor. This may occur in customised implimentations which do not
   support this descriptor.
   static int M_STATS_DESC_NOT_SUPTD
              Identifies that the stack does not support statistics
   descriptor. This may occur in customised implimentations which do not
   support this descriptor.
   static int M_PCKG_DESC_NOT_SUPTD
              Identifies that the stack does not support package
   descriptor. This may occur in customised implimentations which do not
   support this descriptor.
   static int M_SDPINFO_NOT_SUPTD
              Identifies that the stack does not support SDP info
   descriptor. This may occur in customised implimentations which do not
   support this descriptor.
   static int M_INV_PKG_ITEM_TYPE
              Identifies that the value of the package item type as
   passed to the method is invalid.
   static int M_INV_CTXID_TERMTYPE_PAIR
              Identifies that the combination of context Id and
   termination type is invalid. For example only valid contextId value
   for termination type ROOT is NULL or *. For anyother value of
   contextId, error is returned.
   static int M_MISSING_LCL_TPT_ADDR
              Identifies that the local transport address is missing in
   the association event sent from application to the stack.
   static int M_MISSING_REMOTE_TPT_ADDR
              Identifies that the remote transport address is missing in
   the association event sent from application to the stack.
   static int M_MISSING_SVC_CHNG_REASON
              Identifies that the service change reason field is missing
   in the association event sent from application to the stack.
   static int M_MISSING_SVC_CHNG_METHOD
              Identifies that the service change method field is missing
   in the association event sent from application to the stack.
   static int M_MISSING_ENC_FORMAT
              Identifies that the encoding format field is missing in the
   association event sent from application to the stack.
   static int M_TPT_ADDR_ALREADY_INUSE
              Identifies that the pair of local and remote address is
   already in use by some other association.

                                                                   static
                                                                ErrorCode

   PEER_ENTITY_UNVBLE
              Identifies a ErrorCode object that constructs the class
   with the constant M_PEER_ENTITY_UNVBLE.

                                                                   static
                                                                ErrorCode

   INV_LOCAL_TPT_ADDRESS
              Identifies a ErrorCode object that constructs the class
   with the constant M_INV_LOCAL_TPT_ADDRESS.

                                                                   static
                                                                ErrorCode

   INV_REMOTE_TPT_ADDRESS
              Identifies a ErrorCode object that constructs the class
   with the constant M_INV_REMOTE_TPT_ADDRESS.

                                                                   static
                                                                ErrorCode

   INV_LCL_RMT_ADDR_PAIR
              Identifies a ErrorCode object that constructs the class
   with the constant M_INV_LCL_RMT_ADDR_PAIR.

                                                                   static
                                                                ErrorCode

   INV_ASSOC_ID
              Identifies a ErrorCode object that constructs the class
   with the constant M_INV_ASSOC_ID.

                                                                   static
                                                                ErrorCode

   INV_SVC_CHNG_METH_REASON_PAIR
              Identifies a ErrorCode object that constructs the class
   with the constant M_INV_SVC_CHNG_METH_REASON_PAIR.

                                                                   static
                                                                ErrorCode

   UNSPTD_ENC_FORMAT
              Identifies a ErrorCode object that constructs the class
   with the constant M_UNSPTD_ENC_FORMAT.

                                                                   static
                                                                ErrorCode

   UNSPTD_TPT_TYPE
              Identifies a ErrorCode object that constructs the class
   with the constant M_UNSPTD_TPT_TYPE.

                                                                   static
                                                                ErrorCode

   INV_SVC_CHNG_ADDR
              Identifies a ErrorCode object that constructs the class
   with the constant M_INV_SVC_CHNG_ADDR.

                                                                   static
                                                                ErrorCode

   INV_HOFF_MGCID
              Identifies a ErrorCode object that constructs the class
   with the constant M_INV_HOFF_MGCID.

                                                                   static
                                                                ErrorCode

   UNSPTD_SVC_CHNG_METH_EXTN
              Identifies a ErrorCode object that constructs the class
   with the constant M_UNSPTD_SVC_CHNG_METH_EXTN.

                                                                   static
                                                                ErrorCode

   INV_SVC_CHNG_REASON
              Identifies a ErrorCode object that constructs the class
   with the constant M_INV_SVC_CHNG_REASON.

                                                                   static
                                                                ErrorCode

   INV_SVC_CHNG_METHOD
              Identifies a ErrorCode object that constructs the class
   with the constant M_INV_SVC_CHNG_METHOD.

                                                                   static
                                                                ErrorCode

   RES_UNVBLE
              Identifies a ErrorCode object that constructs the class
   with the constant M_RES_UNVBLE.

                                                                   static
                                                                ErrorCode

   MODIFY_ENC_FORMAT_FAILURE
              Identifies a ErrorCode object that constructs the class
   with the constant M_MODIFY_ENC_FORMAT_FAILURE.

                                                                   static
                                                                ErrorCode

   MODIFY_TPT_TYPE_FAILURE
              Identifies a ErrorCode object that constructs the class
   with the constant M_MODIFY_TPT_TYPE_FAILURE.

                                                                   static
                                                                ErrorCode

   MODIFY_SVC_PROFILE_FAILURE
              Identifies a ErrorCode object that constructs the class
   with the constant M_MODIFY_SVC_PROFILE_FAILURE.

                                                                   static
                                                                ErrorCode

   MODIFY_PROTOCOL_VERSION_FAILURE
              Identifies a ErrorCode object that constructs the class
   with the constant M_MODIFY_PROTOCOL_VERSION_FAILURE.

                                                                   static
                                                                ErrorCode

   INV_ACTION_HANDLE
              Identifies a ErrorCode object that constructs the class
   with the constant M_INV_ACTION_HANDLE.

                                                                   static
                                                                ErrorCode

   WLDCRD_CMD_NOT_SUPTD
              Identifies a ErrorCode object that constructs the class
   with the constant M_WLDCRD_CMD_NOT_SUPTD.

                                                                   static
                                                                ErrorCode

   INV_CMD_REQID
              Identifies a ErrorCode object that constructs the class
   with the constant M_INV_CMD_REQID.

                                                                   static
                                                                ErrorCode

   INV_CMD_RESPID
              Identifies a ErrorCode object that constructs the class
   with the constant M_INV_CMD_RESPID.

                                                                   static
                                                                ErrorCode

   TERM_TYPE_NOT_INSYNCH
              Identifies a ErrorCode object that constructs the class
   with the constant M_TERM_TYPE_NOT_INSYNCH.

                                                                   static
                                                                ErrorCode

   INV_TERM_NAME_FORMAT
              Identifies a ErrorCode object that constructs the class
   with the constant M_INV_TERM_NAME_FORMAT.

                                                                   static
                                                                ErrorCode

   TOPOLOGY_DESC_NOT_SUPTD
              Identifies a ErrorCode object that constructs the class
   with the constant M_TOPOLOGY_DESC_NOT_SUPTD.

                                                                   static
                                                                ErrorCode

   MEDIA_DESC_NOT_SUPTD
              Identifies a ErrorCode object that constructs the class
   with the constant M_MEDIA_DESC_NOT_SUPTD.

                                                                   static
                                                                ErrorCode

   MODEM_DESC_NOT_SUPTD
              Identifies a ErrorCode object that constructs the class
   with the constant M_MODEM_DESC_NOT_SUPTD.

                                                                   static
                                                                ErrorCode

   MUX_DESC_NOT_SUPTD
              Identifies a ErrorCode object that constructs the class
   with the constant M_MUX_DESC_NOT_SUPTD.

                                                                   static
                                                                ErrorCode

   EVENT_DESC_NOT_SUPTD
              Identifies a ErrorCode object that constructs the class
   with the constant M_EVENT_DESC_NOT_SUPTD.

                                                                   static
                                                                ErrorCode

   SIGNAL_DESC_NOT_SUPTD
              Identifies a ErrorCode object that constructs the class
   with the constant M_SIGNAL_DESC_NOT_SUPTD.

                                                                   static
                                                                ErrorCode

   DIGITMAP_DESC_NOT_SUPTD
              Identifies a ErrorCode object that constructs the class
   with the constant M_DIGITMAP_DESC_NOT_SUPTD.

                                                                   static
                                                                ErrorCode

   EVENTBUFF_DESC_NOT_SUPTD
              Identifies a ErrorCode object that constructs the class
   with the constant M_EVENTBUFF_DESC_NOT_SUPTD.

                                                                   static
                                                                ErrorCode

   AUDIT_DESC_NOT_SUPTD
              Identifies a ErrorCode object that constructs the class
   with the constant M_AUDIT_DESC_NOT_SUPTD.

                                                                   static
                                                                ErrorCode

   OBSRD_EVENT_DESC_NOT_SUPTD
              Identifies a ErrorCode object that constructs the class
   with the constant M_OBSRD_EVENT_DESC_NOT_SUPTD.

                                                                   static
                                                                ErrorCode

   STATS_DESC_NOT_SUPTD
              Identifies a ErrorCode object that constructs the class
   with the constant M_STATS_DESC_NOT_SUPTD.

                                                                   static
                                                                ErrorCode

   PCKG_DESC_NOT_SUPTD
              Identifies a ErrorCode object that constructs the class
   with the constant M_PCKG_DESC_NOT_SUPTD.

                                                                   static
                                                                ErrorCode

   SDPINFO_NOT_SUPTD
              Identifies a ErrorCode object that constructs the class
   with the constant M_SDPINFO_NOT_SUPTD.

                                                                   static
                                                                ErrorCode

   INV_PKG_ITEM_TYPE
              Identifies a ErrorCode object that constructs the class
   with the constant M_INV_PKG_ITEM_TYPE.

                                                                   static
                                                                ErrorCode

   INV_CTXID_TERMTYPE_PAIR
              Identifies a ErrorCode object that constructs the class
   with the constant M_INV_CTXID_TERMTYPE_PAIR.

                                                                   static
                                                                ErrorCode

   MISSING_LCL_TPT_ADDR
              Identifies a ErrorCode object that constructs the class
   with the constant M_MISSING_LCL_TPT_ADDR.

                                                                   static
                                                                ErrorCode

   MISSING_REMOTE_TPT_ADDR
              Identifies a ErrorCode object that constructs the class
   with the constant M_MISSING_REMOTE_TPT_ADDR.

                                                                   static
                                                                ErrorCode

   MISSING_SVC_CHNG_REASON
              Identifies a ErrorCode object that constructs the class
   with the constant M_MISSING_SVC_CHNG_REASON.

                                                                   static
                                                                ErrorCode

   MISSING_SVC_CHNG_METHOD
              Identifies a ErrorCode object that constructs the class
   with the constant M_MISSING_SVC_CHNG_METHOD.

                                                                   static
                                                                ErrorCode

   MISSING_ENC_FORMAT
              Identifies a ErrorCode object that constructs the class
   with the constant M_MISSING_ENC_FORMAT.

                                                                   static
                                                                ErrorCode

   TPT_ADDR_ALREADY_INUSE
              Identifies a ErrorCode object that constructs the class
   with the constant M_TPT_ADDR_ALREADY_INUSE.

   Constructor Summary
   ErrorCode(int error code)
              Constructs a class initialised with value error code as
   passed to it in the constructor.

   Method Summary
   int getErrorCode()
             Returns one of the allowed integer values defined as static
   fields in this class which shall identify one of the error codes.

                                                                   static

   ErrorCode getObject(int value)
           throw IllegalArgumentException()

             Returns reference of the ErrorCode object that identifies
   the termination type as value passed to this method.

                                                                   object

   readResolve()
              throws ObjectStreamException()

              This method is required to perform instance substitution
   during serialization.

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll,
   toString, wait

   Field Detail
     _________________________________________________________________

  M_MISSING_ENC_FORMAT

public static final int M_MISSING_ENC_FORMAT

          Identifies that the encoding format field is missing in the
          association event sent from application to the stack.
          Its value shall be set to 1.
     _________________________________________________________________

  M_TPT_ADDR_ALREADY_INUSE

public static final int M_TPT_ADDR_ALREADY_INUSE

          Identifies that the pair of local and remote address is already
          in use by some other association.
          Its value shall be set to 2.
     _________________________________________________________________

  M_PEER_ENTITY_UNVBLE

public static final int M_PEER_ENTITY_UNVBLE

          Identifies that the network connection with local address could
          not be created since the peer entity is unavailable.
          Its value shall be set to 3.
     _________________________________________________________________

  M_INV_LOCAL_TPT_ADDRESS

public static final int M_INV_LOCAL_TPT_ADDRESS

          Identifies that the network connection with local address could
          not be created since the transport address specified was
          invalid.
          Its value shall be set to 4
     _________________________________________________________________

  M_INV_REMOTE_TPT_ADDRESS

public static final int M_INV_REMOTE_TPT_ADDRESS

          Identifies that the network connection with remote address
          could not be created since the transport address specified was
          invalid.
          Its value shall be set to 5.
     _________________________________________________________________

  M_INV_LCL_RMT_ADDR_PAIR

public static final int M_INV_LCL_RMT_ADDR_PAIR

          Identifies that the combination of local and remote address is
          invalid. This will come in case of difference in the transport
          type of local and remote address.
          Its value shall be set to 6.
     _________________________________________________________________

  M_INV_ASSOC_ID

public static final int M_INV_ASSOC_ID

          Identifies that no association exists with the specified
          association Id for the corresponding listener instance.
          Its value shall be set to 7.
     _________________________________________________________________

  M_MISSING_SVC_CHNG_METHOD

public static final int M_MISSING_SVC_CHNG_METHOD

          Identifies that the service change method field is missing in
          the association event sent from application to the stack.
          Its value shall be set to 8.
     _________________________________________________________________

  M_INV_SVC_CHNG_METH_REASON_PAIR

public static final int M_INV_SVC_CHNG_METH_REASON_PAIR

          Identifies invalid combination of service change method and
          service change reason.
          Its value shall be set to 9.
     _________________________________________________________________

  M_UNSPTD_ENC_FORMAT

public static final int M_UNSPTD_ENC_FORMAT

          Identifies that the encoding format as requested by the
          application is not supported at the stack.
          Its value shall be set to 10.
     _________________________________________________________________

  M_UNSPTD_TPT_TYPE

public static final int M_UNSPTD_TPT_TYPE

          Identifies that the Transport Type as requested by the
          application is not supported at the stack.
          Its value shall be set to 11.
     _________________________________________________________________

  M_INV_SVC_CHNG_ADDR

public static final int M_INV_SVC_CHNG_ADDR

          Identifies that the service change address parameter specified
          is invalid. This would occur if the transport type specified in
          service change address field is different than specified in
          local address field.
          Its value shall be set to 12.
     _________________________________________________________________

  M_INV_HOFF_MGCID

public static final int M_INV_HOFF_MGCID

          Identifies that the Handed Off MGCId parameter specified is
          invalid. This would occur if the transport type specified in
          MGCId field is different than specified in local address field.
          Its value shall be set to 13.
     _________________________________________________________________

  M_UNSPTD_SVC_CHNG_METH_EXTN

public static final int M_UNSPTD_SVC_CHNG_METH_EXTN

          Identifies that the service change extension method as
          requested by the application is not supported at the stack.
          Its value shall be set to 14.
     _________________________________________________________________

  M_INV_SVC_CHNG_REASON

public static final int M_INV_SVC_CHNG_REASON

          Identifies that the service change reason field is invalid for
          the requested event.
          Its value shall be set to 15.
     _________________________________________________________________

  M_INV_SVC_CHNG_METHOD

public static final int M_INV_SVC_CHNG_METHOD

          Identifies that the service change method field is invalid for
          the requested event.
          Its value shall be set to 16.
     _________________________________________________________________

  M_RES_UNVBLE

public static final int M_RES_UNVBLE

          Identifies that the stack is unable to process an event
          requested because of unavailability of system resources such as
          memory and timer.
          Its value shall be set to 17.
     _________________________________________________________________

  M_MODIFY_ENC_FORMAT_FAILURE

public static final int M_MODIFY_ENC_FORMAT_FAILURE

          Identifies that modify encoding format failed since the
          association was already in use.
          Its value shall be set to 18.
     _________________________________________________________________

  M_MODIFY_TPT_TYPE_FAILURE

public static final int M_MODIFY_TPT_TYPE_FAILURE

          Identifies that modify Transport type failed since the
          association was already in use.
          Its value shall be set to 19.
     _________________________________________________________________

  M_MODIFY_SVC_PROFILE_FAILURE

public static final int M_MODIFY_SVC_PROFILE_FAILURE

          Identifies that modify Service Change profile failed since the
          association was already in use.
          Its value shall be set to 20.
     _________________________________________________________________

  M_MODIFY_PROTOCOL_VERSION_FAILURE

public static final int M_MODIFY_PROTOCOL_VERSION_FAILURE

          Identifies that modify protocol version failed since the
          association was already in use.
          Its value shall be set to 21.
     _________________________________________________________________

  M_INV_ACTION_HANDLE

public static final int M_INV_ACTION_HANDLE

          Identifies that the action handle as specified in the command
          event is invalid. This wuuld occur if the second command is
          added in a transaction with action handle different than the
          previous command and isFirstCommandinAction flag is not set to
          TRUE or this would occur if the action handle specified in the
          command response event is not same as the action handle in the
          request event.
          Its value shall be set to 22.
     _________________________________________________________________

  M_WLDCRD_CMD_NOT_SUPTD

public static final int M_WLDCRD_CMD_NOT_SUPTD

          Identifies that stack does not support wildcarded command
          request event.
          Its value shall be set to 23.
     _________________________________________________________________

  M_INV_CMD_REQID

public static final int M_INV_CMD_REQID

          Identifies that the command request identifier is invalid. This
          would occur if for example Notify command request event is sent
          to stack.
          Its value shall be set to 24.
     _________________________________________________________________

  M_INV_CMD_RESPID

public static final int M_INV_CMD_RESPID

          Identifies that the command response identifier is invalid.
          This would occur if for example Modify command response event
          is sent to stack for ADD command request.
          Its value shall be set to 25.
     _________________________________________________________________

  M_TERM_TYPE_NOT_INSYNCH

public static final int M_TERM_TYPE_NOT_INSYNCH

          Identifies that the as specified in the command request event
          is not in synch with the term type in the command response
          event. This would occur if the term type specified in the
          command request is M_TERM_TYPE_NORMAL and term type specified
          in the response is M_TERM_TYPE_ROOT.
          Its value shall be set to 26.
     _________________________________________________________________

  M_INV_TERM_NAME_FORMAT

public static final int M_INV_TERM_NAME_FORMAT

          Identifies that the Term Name format is different than the
          encoding format for this assciation. This would occur if the
          termination name specified is in the binary format whereas the
          encoding format supported by the association is TEXT.
          Its value shall be set to 27.
     _________________________________________________________________

  M_TOPOLOGY_DESC_NOT_SUPTD

public static final int M_TOPOLOGY_DESC_NOT_SUPTD

          Identifies that the stack does not support topology descriptor.
          This may occur in customised implimentations which do not
          support this descriptor.
          Its value shall be set to 28.
     _________________________________________________________________

  M_MEDIA_DESC_NOT_SUPTD

public static final int M_MEDIA_DESC_NOT_SUPTD

          Identifies that the stack does not support media descriptor.
          This may occur in customised implimentations which do not
          support this descriptor.
          Its value shall be set to 29.
     _________________________________________________________________

  M_MODEM_DESC_NOT_SUPTD

public static final int M_MODEM_DESC_NOT_SUPTD

          Identifies that the stack does not support modem descriptor.
          This may occur in customised implimentations which do not
          support this descriptor.
          Its value shall be set to 30.
     _________________________________________________________________

  M_MUX_DESC_NOT_SUPTD

public static final int M_MUX_DESC_NOT_SUPTD

          Identifies that the stack does not support mux descriptor. This
          may occur in customised implimentations which do not support
          this descriptor.
          Its value shall be set to 31.
     _________________________________________________________________

  M_EVENT_DESC_NOT_SUPTD

public static final int M_EVENT_DESC_NOT_SUPTD

          Identifies that the stack does not support event descriptor.
          This may occur in customised implimentations which do not
          support this descriptor.
          Its value shall be set to 32.
     _________________________________________________________________

  M_SIGNAL_DESC_NOT_SUPTD

public static final int M_SIGNAL_DESC_NOT_SUPTD

          Identifies that the stack does not support signal descriptor.
          This may occur in customised implimentations which do not
          support this descriptor.
          Its value shall be set to 33.
     _________________________________________________________________

  M_DIGITMAP_DESC_NOT_SUPTD

public static final int M_DIGITMAP_DESC_NOT_SUPTD

          Identifies that the stack does not support digit map
          descriptor. This may occur in customised implimentations which
          do not support this descriptor.
          Its value shall be set to 34.
     _________________________________________________________________

  M_EVENTBUFF_DESC_NOT_SUPTD

public static final int M_EVENTBUFF_DESC_NOT_SUPTD

          Identifies that the stack does not support event buffer
          descriptor. This may occur in customised implimentations which
          do not support this descriptor.
          Its value shall be set to 35.
     _________________________________________________________________

  M_AUDIT_DESC_NOT_SUPTD

public static final int M_AUDIT_DESC_NOT_SUPTD

          Identifies that the stack does not support audit descriptor.
          This may occur in customised implimentations which do not
          support this descriptor.
          Its value shall be set to 36.
     _________________________________________________________________

  M_OBSRD_EVENT_DESC_NOT_SUPTD

public static final int M_OBSRD_EVENT_DESC_NOT_SUPTD

          Identifies that the stack does not support observed event
          descriptor. This may occur in customised implimentations which
          do not support this descriptor.
          Its value shall be set to 37.
     _________________________________________________________________

  M_STATS_DESC_NOT_SUPTD

public static final int M_STATS_DESC_NOT_SUPTD

          Identifies that the stack does not support statistics
          descriptor. This may occur in customised implimentations which
          do not support this descriptor.
          Its value shall be set to 38.
     _________________________________________________________________

  M_PCKG_DESC_NOT_SUPTD

public static final int M_PCKG_DESC_NOT_SUPTD

          Identifies that the stack does not support package descriptor.
          This may occur in customised implimentations which do not
          support this descriptor.
          Its value shall be set to 39.
     _________________________________________________________________

  M_SDPINFO_NOT_SUPTD

public static final int M_SDPINFO_NOT_SUPTD

          Identifies that the stack does not support SDP info descriptor.
          This may occur in customised implimentations which do not
          support this descriptor.
          Its value shall be set to 40.
     _________________________________________________________________

  M_INV_PKG_ITEM_TYPE

public static final int M_INV_PKG_ITEM_TYPE

          Identifies that the value of the package item type as passed to
          the method is invalid.
          Its value shall be set to 41.
     _________________________________________________________________

  M_INV_CTXID_TERMTYPE_PAIR

public static final int M_INV_CTXID_TERMTYPE_PAIR

          Identifies that the combination of context Id and termination
          type is invalid. For example only valid contextId value for
          termination type ROOT is NULL or *. For anyother value of
          contextId, error is returned.
          Its value shall be set to 42.
     _________________________________________________________________

  M_MISSING_LCL_TPT_ADDR

public static final int M_MISSING_LCL_TPT_ADDR

          Identifies that the local transport address is missing in the
          association event sent from application to the stack.
          Its value shall be set to 43.
     _________________________________________________________________

  M_MISSING_REMOTE_TPT_ADDR

public static final int M_MISSING_REMOTE_TPT_ADDR

          Identifies that the remote transport address is missing in the
          association event sent from application to the stack.
          Its value shall be set to 44.
     _________________________________________________________________

  M_MISSING_SVC_CHNG_REASON

public static final int M_MISSING_SVC_CHNG_REASON

          Identifies that the service change reason field is missing in
          the association event sent from application to the stack.
          Its value shall be set to 45.
     _________________________________________________________________

  PEER_ENTITY_UNVBLE

public static final ErrorCode PEER_ENTITY_UNVBLE
        = new ErrorCode(M_PEER_ENTITY_UNVBLE);

          Identifies a ErrorCode object that constructs the class with
          the constant M_PEER_ENTITY_UNVBLE. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_LOCAL_TPT_ADDRESS

public static final ErrorCode INV_LOCAL_TPT_ADDRESS
        = new ErrorCode(M_INV_LOCAL_TPT_ADDRESS);

          Identifies a ErrorCode object that constructs the class with
          the constant M_INV_LOCAL_TPT_ADDRESS. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_REMOTE_TPT_ADDRESS

public static final ErrorCode INV_REMOTE_TPT_ADDRESS
        = new ErrorCode(M_INV_REMOTE_TPT_ADDRESS);

          Identifies a ErrorCode object that constructs the class with
          the constant M_INV_REMOTE_TPT_ADDRESS. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_LCL_RMT_ADDR_PAIR

public static final ErrorCode INV_LCL_RMT_ADDR_PAIR
        = new ErrorCode(M_INV_LCL_RMT_ADDR_PAIR);

          Identifies a ErrorCode object that constructs the class with
          the constant M_INV_LCL_RMT_ADDR_PAIR. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_ASSOC_ID

public static final ErrorCode INV_ASSOC_ID
        = new ErrorCode(M_INV_ASSOC_ID);

          Identifies a ErrorCode object that constructs the class with
          the constant M_INV_ASSOC_ID. Since it is reference to static
          final object, it prevents further instantiation of the same
          object in the system.
     _________________________________________________________________

  INV_SVC_CHNG_METH_REASON_PAIR

public static final ErrorCode INV_SVC_CHNG_METH_REASON_PAIR
        = new ErrorCode(M_INV_SVC_CHNG_METH_REASON_PAIR);

          Identifies a ErrorCode object that constructs the class with
          the constant M_INV_SVC_CHNG_METH_REASON_PAIR. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  UNSPTD_ENC_FORMAT

public static final ErrorCode UNSPTD_ENC_FORMAT
        = new ErrorCode(M_UNSPTD_ENC_FORMAT);

          Identifies a ErrorCode object that constructs the class with
          the constant M_UNSPTD_ENC_FORMAT. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  UNSPTD_TPT_TYPE

public static final ErrorCode UNSPTD_TPT_TYPE
        = new ErrorCode(M_UNSPTD_TPT_TYPE);

          Identifies a ErrorCode object that constructs the class with
          the constant M_UNSPTD_TPT_TYPE. Since it is reference to static
          final object, it prevents further instantiation of the same
          object in the system.
     _________________________________________________________________

  INV_SVC_CHNG_ADDR

public static final ErrorCode INV_SVC_CHNG_ADDR
        = new ErrorCode(M_INV_SVC_CHNG_ADDR);

          Identifies a ErrorCode object that constructs the class with
          the constant M_INV_SVC_CHNG_ADDR. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_HOFF_MGCID

public static final ErrorCode INV_HOFF_MGCID
        = new ErrorCode(M_INV_HOFF_MGCID);

          Identifies a ErrorCode object that constructs the class with
          the constant M_INV_HOFF_MGCID. Since it is reference to static
          final object, it prevents further instantiation of the same
          object in the system.
     _________________________________________________________________

  UNSPTD_SVC_CHNG_METH_EXTN

public static final ErrorCode UNSPTD_SVC_CHNG_METH_EXTN
        = new ErrorCode(M_UNSPTD_SVC_CHNG_METH_EXTN);

          Identifies a ErrorCode object that constructs the class with
          the constant M_UNSPTD_SVC_CHNG_METH_EXTN. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  INV_SVC_CHNG_REASON

public static final ErrorCode INV_SVC_CHNG_REASON
        = new ErrorCode(M_INV_SVC_CHNG_REASON);

          Identifies a ErrorCode object that constructs the class with
          the constant M_INV_SVC_CHNG_REASON. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_SVC_CHNG_METHOD

public static final ErrorCode INV_SVC_CHNG_METHOD
        = new ErrorCode(M_INV_SVC_CHNG_METHOD);

          Identifies a ErrorCode object that constructs the class with
          the constant M_INV_SVC_CHNG_METHOD. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  RES_UNVBLE

public static final ErrorCode RES_UNVBLE
        = new ErrorCode(M_RES_UNVBLE);

          Identifies a ErrorCode object that constructs the class with
          the constant M_RES_UNVBLE. Since it is reference to static
          final object, it prevents further instantiation of the same
          object in the system.
     _________________________________________________________________

  MODIFY_ENC_FORMAT_FAILURE

public static final ErrorCode MODIFY_ENC_FORMAT_FAILURE
        = new ErrorCode(M_MODIFY_ENC_FORMAT_FAILURE);

          Identifies a ErrorCode object that constructs the class with
          the constant M_MODIFY_ENC_FORMAT_FAILURE. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  MODIFY_TPT_TYPE_FAILURE

public static final ErrorCode MODIFY_TPT_TYPE_FAILURE
        = new ErrorCode(M_MODIFY_TPT_TYPE_FAILURE);

          Identifies a ErrorCode object that constructs the class with
          the constant M_MODIFY_TPT_TYPE_FAILURE. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  MODIFY_SVC_PROFILE_FAILURE

public static final ErrorCode MODIFY_SVC_PROFILE_FAILURE
        = new ErrorCode(M_MODIFY_SVC_PROFILE_FAILURE);

          Identifies a ErrorCode object that constructs the class with
          the constant M_MODIFY_SVC_PROFILE_FAILURE. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  MODIFY_PROTOCOL_VERSION_FAILURE

public static final ErrorCode MODIFY_PROTOCOL_VERSION_FAILURE
        = new ErrorCode(M_MODIFY_PROTOCOL_VERSION_FAILURE);

          Identifies a ErrorCode object that constructs the class with
          the constant M_MODIFY_PROTOCOL_VERSION_FAILURE. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  INV_ACTION_HANDLE

public static final ErrorCode INV_ACTION_HANDLE
        = new ErrorCode(M_INV_ACTION_HANDLE);

          Identifies a ErrorCode object that constructs the class with
          the constant M_INV_ACTION_HANDLE. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  WLDCRD_CMD_NOT_SUPTD

public static final ErrorCode WLDCRD_CMD_NOT_SUPTD
        = new ErrorCode(M_WLDCRD_CMD_NOT_SUPTD);

          Identifies a ErrorCode object that constructs the class with
          the constant M_WLDCRD_CMD_NOT_SUPTD. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_CMD_REQID

public static final ErrorCode INV_CMD_REQID
        = new ErrorCode(M_INV_CMD_REQID);

          Identifies a ErrorCode object that constructs the class with
          the constant M_INV_CMD_REQID. Since it is reference to static
          final object, it prevents further instantiation of the same
          object in the system.
     _________________________________________________________________

  INV_CMD_RESPID

public static final ErrorCode INV_CMD_RESPID
        = new ErrorCode(M_INV_CMD_RESPID);

          Identifies a ErrorCode object that constructs the class with
          the constant M_INV_CMD_RESPID. Since it is reference to static
          final object, it prevents further instantiation of the same
          object in the system.
     _________________________________________________________________

  TERM_TYPE_NOT_INSYNCH

public static final ErrorCode TERM_TYPE_NOT_INSYNCH
        = new ErrorCode(M_TERM_TYPE_NOT_INSYNCH);

          Identifies a ErrorCode object that constructs the class with
          the constant M_TERM_TYPE_NOT_INSYNCH. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_TERM_NAME_FORMAT

public static final ErrorCode INV_TERM_NAME_FORMAT
        = new ErrorCode(M_INV_TERM_NAME_FORMAT);

          Identifies a ErrorCode object that constructs the class with
          the constant M_INV_TERM_NAME_FORMAT. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  TOPOLOGY_DESC_NOT_SUPTD

public static final ErrorCode TOPOLOGY_DESC_NOT_SUPTD
        = new ErrorCode(M_TOPOLOGY_DESC_NOT_SUPTD);

          Identifies a ErrorCode object that constructs the class with
          the constant M_TOPOLOGY_DESC_NOT_SUPTD. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  MEDIA_DESC_NOT_SUPTD

public static final ErrorCode MEDIA_DESC_NOT_SUPTD
        = new ErrorCode(M_MEDIA_DESC_NOT_SUPTD);

          Identifies a ErrorCode object that constructs the class with
          the constant M_MEDIA_DESC_NOT_SUPTD. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  MODEM_DESC_NOT_SUPTD

public static final ErrorCode MODEM_DESC_NOT_SUPTD
        = new ErrorCode(M_MODEM_DESC_NOT_SUPTD);

          Identifies a ErrorCode object that constructs the class with
          the constant M_MODEM_DESC_NOT_SUPTD. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  MUX_DESC_NOT_SUPTD

public static final ErrorCode MUX_DESC_NOT_SUPTD
        = new ErrorCode(M_MUX_DESC_NOT_SUPTD);

          Identifies a ErrorCode object that constructs the class with
          the constant M_MUX_DESC_NOT_SUPTD. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  EVENT_DESC_NOT_SUPTD

public static final ErrorCode EVENT_DESC_NOT_SUPTD
        = new ErrorCode(M_EVENT_DESC_NOT_SUPTD);

          Identifies a ErrorCode object that constructs the class with
          the constant M_EVENT_DESC_NOT_SUPTD. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  SIGNAL_DESC_NOT_SUPTD

public static final ErrorCode SIGNAL_DESC_NOT_SUPTD
        = new ErrorCode(M_SIGNAL_DESC_NOT_SUPTD);

          Identifies a ErrorCode object that constructs the class with
          the constant M_SIGNAL_DESC_NOT_SUPTD. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  DIGITMAP_DESC_NOT_SUPTD

public static final ErrorCode DIGITMAP_DESC_NOT_SUPTD
        = new ErrorCode(M_DIGITMAP_DESC_NOT_SUPTD);

          Identifies a ErrorCode object that constructs the class with
          the constant M_DIGITMAP_DESC_NOT_SUPTD. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  EVENTBUFF_DESC_NOT_SUPTD

public static final ErrorCode EVENTBUFF_DESC_NOT_SUPTD
        = new ErrorCode(M_EVENTBUFF_DESC_NOT_SUPTD);

          Identifies a ErrorCode object that constructs the class with
          the constant M_EVENTBUFF_DESC_NOT_SUPTD. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  AUDIT_DESC_NOT_SUPTD

public static final ErrorCode AUDIT_DESC_NOT_SUPTD
        = new ErrorCode(M_AUDIT_DESC_NOT_SUPTD);

          Identifies a ErrorCode object that constructs the class with
          the constant M_AUDIT_DESC_NOT_SUPTD. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  OBSRD_EVENT_DESC_NOT_SUPTD

public static final ErrorCode OBSRD_EVENT_DESC_NOT_SUPTD
        = new ErrorCode(M_OBSRD_EVENT_DESC_NOT_SUPTD);

          Identifies a ErrorCode object that constructs the class with
          the constant M_OBSRD_EVENT_DESC_NOT_SUPTD. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  STATS_DESC_NOT_SUPTD

public static final ErrorCode STATS_DESC_NOT_SUPTD
        = new ErrorCode(M_STATS_DESC_NOT_SUPTD);

          Identifies a ErrorCode object that constructs the class with
          the constant M_STATS_DESC_NOT_SUPTD. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  PCKG_DESC_NOT_SUPTD

public static final ErrorCode PCKG_DESC_NOT_SUPTD
        = new ErrorCode(M_PCKG_DESC_NOT_SUPTD);

          Identifies a ErrorCode object that constructs the class with
          the constant M_PCKG_DESC_NOT_SUPTD. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  SDPINFO_NOT_SUPTD

public static final ErrorCode SDPINFO_NOT_SUPTD
        = new ErrorCode(M_SDPINFO_NOT_SUPTD);

          Identifies a ErrorCode object that constructs the class with
          the constant M_SDPINFO_NOT_SUPTD. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_PKG_ITEM_TYPE

public static final ErrorCode INV_PKG_ITEM_TYPE
        = new ErrorCode(M_INV_PKG_ITEM_TYPE);

          Identifies a ErrorCode object that constructs the class with
          the constant M_INV_PKG_ITEM_TYPE. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_CTXID_TERMTYPE_PAIR

public static final ErrorCode INV_CTXID_TERMTYPE_PAIR
        = new ErrorCode(M_INV_CTXID_TERMTYPE_PAIR);

          Identifies a ErrorCode object that constructs the class with
          the constant M_INV_CTXID_TERMTYPE_PAIR. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  MISSING_LCL_TPT_ADDR

public static final ErrorCode MISSING_LCL_TPT_ADDR
        = new ErrorCode(M_MISSING_LCL_TPT_ADDR);

          Identifies a ErrorCode object that constructs the class with
          the constant M_MISSING_LCL_TPT_ADDR. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  MISSING_REMOTE_TPT_ADDR

public static final ErrorCode MISSING_REMOTE_TPT_ADDR
        = new ErrorCode(M_MISSING_REMOTE_TPT_ADDR);

          Identifies a ErrorCode object that constructs the class with
          the constant M_MISSING_REMOTE_TPT_ADDR. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  MISSING_SVC_CHNG_REASON

public static final ErrorCode MISSING_SVC_CHNG_REASON
        = new ErrorCode(M_MISSING_SVC_CHNG_REASON);

          Identifies a ErrorCode object that constructs the class with
          the constant M_MISSING_SVC_CHNG_REASON. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  MISSING_SVC_CHNG_METHOD

public static final ErrorCode MISSING_SVC_CHNG_METHOD
        = new ErrorCode(M_MISSING_SVC_CHNG_METHOD);

          Identifies a ErrorCode object that constructs the class with
          the constant M_MISSING_SVC_CHNG_METHOD. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  MISSING_ENC_FORMAT

public static final ErrorCode MISSING_ENC_FORMAT
        = new ErrorCode(M_MISSING_ENC_FORMAT);

          Identifies a ErrorCode object that constructs the class with
          the constant M_MISSING_ENC_FORMAT. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  TPT_ADDR_ALREADY_INUSE

public static final ErrorCode TPT_ADDR_ALREADY_INUSE
        = new ErrorCode(M_TPT_ADDR_ALREADY_INUSE);

          Identifies a ErrorCode object that constructs the class with
          the constant M_TPT_ADDR_ALREADY_INUSE. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

   Constructor Detail

  ErrorCode

private ErrorCode(int error_code)

          Constructs an abstract class that specifies the error code in
          the command.
     _________________________________________________________________

   Method Detail

  getErrorCode

public int getErrorCode()

          This method returns one of the static field constants defined
          in this class. This method is overriden by the derived class,
          which shall return an hard coded value depending on the class.


        Returns:
                Returns an integer value that identifies the error code
                to be one of the constant values defined for this class.
     _________________________________________________________________

  getObject

public static final ErrorCode getObject(int value)
                throws IllegalArgumentException()

          Returns reference of the ErrorCode object that identifies the
          termination type as value passed to this method.


                Parameters:
                          value - It is one of the possible values of the
                static constant that this class provides.

        Returns:
                Returns reference of the ErrorCode object.

        throws:
                IllegalArgumentException() - If the value passed to this
                method is invalid, then this exception is raised.
     _________________________________________________________________

  readResolve

private java.lang.Object readResolve()

        This method must be implemented to perform instance substitution
   during serialization. This method is required for reference
   comparison. This method if not implimented will simply fail each time
   we compare an Enumeration value against a de-serialized Enumeration
   instance.
     _________________________________________________________________
     _________________________________________________________________

   Overview Package  Class Tree Index Help
   JAIN^TM MEGACO API (RELEASE) - Version 1.0 - 22 December 2003
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
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
