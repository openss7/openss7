// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile: ExceptionInfoCode.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:34 $ <p>
 
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
    PREV CLASS   NEXT CLASS FRAMESNO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:  FIELD | CONSTR |
   METHOD
     _________________________________________________________________

javax.megaco
Class ExceptionInfoCode

java.lang.Object
  |
  +--javax.megaco.ExceptionInfoCode

   All Implemented Interfaces:
          java.io.Serializable

   Direct Known Subclasses:
             None
     _________________________________________________________________

   public class ExceptionInfoCode
   extends java.lang.Object

   The ExceptionInfoCode class defines the contants for the associated
   info code whenever a exception is raised. The info code would assist
   the application in debugging the cause of exception, whenever it is
   raised. Most the the info codes specified in this class are for the
   IllegalArgumentException raised when a invalid argument is passed to a
   method.

   See Also:
          Serialized Form
     _________________________________________________________________

   Field Summary
   static int M_INV_IP_ADDR_FORMAT
              Identifies that the IP Address string passed to the method
   is invalid as per the protocol.
   static int M_INV_MTP3B_ADDR_FORMAT
              Identifies that the MTP3b Address string passed to the
   method is invalid as per what has been defined by the class.
   static int M_INV_AAL5_ADDR_FORMAT
              Identifies that the AAL5 Address string passed to the
   method is invalid as per what has been defined by the class.
   static int M_INV_LOCAL_ADDR
              Identifies that the Local Address object reference contains
   incorrect or incomplete information.
   static int M_INV_REMOTE_ADDR
              Identifies that the Remote Address object reference
   contains incorrect or incomplete information.
   static int M_INV_ERROR_CODE
              Identifies that the error code object reference is not
   valid for the class for which it is being set. For example
   CreateAssocResp class should return error code corresponding to
   CreateAssocReq only. If any othen error code is set, then exception is
   raised with this info code.
   static int M_INV_EVT_STATUS_VAL
              Identifies that the event status valus specified for this
   class is invalid. This will also be returned in case error code is
   being set for a class, but the event status is set to M_SUCCESS.
   static int M_INV_REQ_ID
              Identifies that the request Id passed to the method is
   invalid. This will be returned if only positive values are allowed and
   request Id passed is negative.
   static int M_MISSING_EVT_PARAM
              Identifies that the event parameters are missing in the
   class RequestedEventParam object.
   static int M_MISSING_PKG_NAME
              Identifies that the package name is missing in the object
   reference.
   static int M_MISSING_ITEM_NAME
              Identifies that the item name is missing in the object
   reference.
   static int M_MISSING_PKG_PARAM
              Identifies that the package parameter is missing in the
   object reference.
   static int M_MISSING_INFO_IN_OBJ
              Identifies that the class object contains no information.
   For example class LocalAddr object can be created with empty
   constructor. If further no parameters are set in it and the reference
   of the same is passed to a method, then this exception is raised.
   static int M_BOTH_KA_EMBEDSIG_PRESENT
              Identifies that both Keep Active and Embed with Signal
   Descriptor are present in the event params.
   static int M_INV_STREAM_ID
              Identifies that the stream Id passed to the method is
   invalid. This will be returned if only positive values are allowed and
   stream Id passed is negative.
   static int M_INV_TIMER_VAL
              Identifies that the timer value as passed in the method is
   not correct as per the syntax defined by the protocol.
   static int M_INV_DIGIT_VAL
              Identifies that the digit value specified for a
   digitpoisition in the digitmap is invalid. The digits specified must
   be in the range DIGIT0 to DIGITX.
   static int M_NULL_OBJ_REF_PASSED
              Identifies that the NULL object reference has been passed
   to the method. This would be raised if NULL reference is passed to a
   method which is expecting a valid reference.
   static int M_INV_SIGNAL_LIST_ID
              Identifies that the signal list Id specified is not correct
   as per the syntax of the protocol.
   static int M_MISSING_SIGNAL_LIST_ID
              Identifies that the signal param type has been set to
   M_SIGNAL_PARAM_LIST but the signal param list Id is missing in the
   class SignalParam.
   static int M_INV_OPR_ON_SIGNAL_PARAM
              Identifies that the signal param type is set to
   M_SIGNAL_PARAM_REQUEST. This would occur if the a method is invoked
   which is invalid for this signal param type. For example if signal
   param list Id is being set for the signal type M_SIGNAL_PARAM_REQUEST.
   static int M_INV_SIGNAL_DURATION
              Identifies that the signal duration specified is invalid as
   per the syntax defined by the protocol.
   static int M_BOTH_STREAM_PARAM_DESC_PRESENT
              Identifies that the both stream param and stream descriptor
   are present in the media descriptor, which is not valid as per the
   protocol.
   static int M_INV_SDP_PARAM
              Identifies that the parameter specified in the SDP is
   invalid.
   static int M_MISSING_MEDIA_STREAM_PARAM
              Identifies that the the media stream parameters are missing
   in the stream descriptor.
   static int M_MISSING_STREAM_ID
              Identifies that the stream Id is missing in the stream
   descriptor.
   static int M_MISSING_EXTENSION_PARAM
              Identifies that the extension parameter is missing in the
   class object. This will be returned if in case the get method for
   extension parameter is invoked without checking for its presence.
   static int M_INV_PROTOCOL_VERSION
              Identifies that the protocol version is incorrect as per
   the range defined by the protocol.
   static int M_INV_DATE_STRING
              Identifies that the date value is not in the format defined
   by the protocol.
   static int M_INV_TIME_STRING
              Identifies that the time value is not in the format defined
   by the protocol.
   static int M_INV_MEDIA_DESC
              Identifies that the media descriptor is invalid. This would
   be returned if it contains no information.
   static int M_INV_DIGITMAP_DESC
              Identifies that the digitmap descriptor is invalid. This
   would be returned if it contains no information.
   static int M_INV_PKG_DESC
              Identifies that the package descriptor is invalid. This
   would be returned if it contains no information.
   static int M_INV_CNTX_TERM_AUDIT_DESC
              Identifies that the context termination audit descriptor is
   invalid. This would be returned if it contains no information.
   static int M_ERROR_DESC_ALRDY_PRESENT
              Identifies that the error descriptor is already present for
   the class object. So in this case no other parameters can be set. For
   example in class ContextTermAuditDescriptor both termIdList and error
   descriptor cannot co-exist.
   static int M_TERMID_LIST_ALRDY_PRESENT
              Identifies that the terminationId list is already present
   for the class object. So in this case if set error descriptor is
   invoked, then exception is raised. For example in class
   ContextTermAuditDescriptor both termIdList and error descriptor cannot
   co-exist.
   static int M_INV_YEAR_VAL
              Identifies that the value of year specified is invalid. If
   the value of year in not in the valid range, then the exception is
   raised.
   static int M_INV_MONTH_VAL
              Identifies that the value of month specified is invalid. If
   the value of month in not in the valid range, then the exception is
   raised.
   static int M_INV_DAY_VAL
              Identifies that the value of day specified is invalid. If
   the value of day in not in the valid range, then the exception is
   raised.
   static int M_INV_HOUR_VAL
              Identifies that the value of hour specified is invalid. If
   the value of hour in not in the valid range, then the exception is
   raised.
   static int M_INV_MINUTES_VAL
              Identifies that the value of minutes specified is invalid.
   If the value of minutes in not in the valid range, then the exception
   is raised.
   static int M_INV_SECONDS_VAL
              Identifies that the value of seconds specified is invalid.
   If the value of seconds in not in the valid range, then the exception
   is raised.
   static int M_INV_ASSOCIATED_PKD_ID
              Identifies that the value of associated package Id is not
   valid. The associated package Id passed in the method is not one
   amongst the package Ids that this package extends for the specified
   item.
   static int M_INV_PKG_ITEM_PARAM
              Identifies that the package item param passed for the item
   are invalid. The package item param contains information of the
   associated item and the associated package. If these values donot
   match with the item for which these parameters are specified, then the
   exception with this info code is raised.
   static int M_INV_RETURN_STATUS_VALUE
              Identifies that the value of the return status as passed to
   the method is invalid.
   static int M_INV_ASSOC_EVENT_TYPE
              Identifies that the value of the association event type as
   passed to the method is invalid.
   static int M_INV_ASSOC_IND_REASON
              Identifies that the value of the association indication
   reason as passed to the method is invalid.
   static int M_INV_ASSOC_STATE
              Identifies that the value of the association state as
   passed to the method is invalid.
   static int M_INV_ENC_FORMAT
              Identifies that the value of the encoding format as passed
   to the method is invalid.
   static int M_INV_SVC_CHNG_METHOD
              Identifies that the value of the service change method as
   passed to the method is invalid.
   static int M_INV_SVC_CHNG_REASON
              Identifies that the value of the service change reason as
   passed to the method is invalid.
   static int M_INV_TPT_TYPE
              Identifies that the value of the transport type as passed
   to the method is invalid.
   static int M_INV_CMD_REQ_TYPE
              Identifies that the value of the command request type as
   passed to the method is invalid.
   static int M_INV_CMD_RESP_TYPE
              Identifies that the value of the command response type as
   passed to the method is invalid.
   static int M_INV_CMD_TYPE
              Identifies that the value of the command type as passed to
   the method is invalid.
   static int M_INV_DESC_TYPE
              Identifies that the value of the descriptor type as passed
   to the method is invalid.
   static int M_INV_TERM_TYPE
              Identifies that the value of the termination type as passed
   to the method is invalid.
   static int M_INV_EVT_BUFFER_CTRL
              Identifies that the value of the event buffer control as
   passed to the method is invalid.
   static int M_INV_MODEM_TYPE
              Identifies that the value of the modem type as passed to
   the method is invalid.
   static int M_INV_MUX_TYPE
              Identifies that the value of the mux type as passed to the
   method is invalid.
   static int M_INV_SERVICE_STATE
              Identifies that the value of the service state as passed to
   the method is invalid.
   static int M_INV_SIGNAL_PARAM_TYPE
              Identifies that the value of the signal param type as
   passed to the method is invalid.
   static int M_INV_SIGNAL_TYPE
              Identifies that the value of the signal type as passed to
   the method is invalid.
   static int M_INV_STREAM_MODE
              Identifies that the value of the stream mode as passed to
   the method is invalid.
   static int M_INV_TOPOLOGY_DIR
              Identifies that the value of the topology direction as
   passed to the method is invalid.
   static int M_INV_PARAM_VAL_TYPE
              Identifies that the value of the param value type as passed
   to the method is invalid.
   static int M_INV_PARAM_RELATION
              Identifies that the value of the parameter relation as
   passed to the method is invalid.
   static int M_INV_PKG_ITEM_TYPE
              Identifies that the value of the package item type as
   passed to the method is invalid.
   static int M_INV_PRIORITY_VAL
              Identifies that the value of priority as passed to the
   method is invalid. The valid valued for Priority as defined by the
   protocol is between the range 0 to 15. For any value other than this
   range, exception is raised with this info code.
   static int M_DUPLICATE_DESC_IN_CMD
              Identifies that same descriptor has been set twice in the
   command. This will be retuned if for example Media Descriptor is set
   twice in the command request.
   static int M_MISSING_DIGIT_WITH_QUALIFIER_DOT
              According to the protocol, DOT is a digit qualifier. At a
   digit position, DOT qualiifies the associated digit. In case if only
   DOT is specified for the digit position, then exception is raised with
   info code set to this.
   static int M_SYNTAX_ERR_IN_TERM_NAME
              Identifies that the termination name as passed in the
   method is not correct as per the syntax defined by the protocol.
   static int M_SYNTAX_ERR_IN_DMAP_NAME
              Identifies that the digitmap name as passed in the method
   is not correct as per the syntax defined by the protocol.
   static int M_SYNTAX_ERR_IN_SVC_CHNG_PROFILE
              Identifies that the service change profile is incorrect as
   per the syntax defined by the protocol.

                                                                   static
                                                        ExceptionInfoCode

   INV_IP_ADDR_FORMAT
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_IP_ADDR_FORMAT.

                                                                   static
                                                        ExceptionInfoCode

   INV_MTP3B_ADDR_FORMAT
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_MTP3B_ADDR_FORMAT.

                                                                   static
                                                        ExceptionInfoCode

   INV_AAL5_ADDR_FORMAT
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_AAL5_ADDR_FORMAT.

                                                                   static
                                                        ExceptionInfoCode

   INV_LOCAL_ADDR
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_LOCAL_ADDR.

                                                                   static
                                                        ExceptionInfoCode

   INV_REMOTE_ADDR
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_REMOTE_ADDR.

                                                                   static
                                                        ExceptionInfoCode

   INV_EVT_STATUS_VAL
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_EVT_STATUS_VAL.

                                                                   static
                                                        ExceptionInfoCode

   INV_REQ_ID
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_REQ_ID.

                                                                   static
                                                        ExceptionInfoCode

   MISSING_EVT_PARAM
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_MISSING_EVT_PARAM.

                                                                   static
                                                        ExceptionInfoCode

   MISSING_PKG_NAME
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_MISSING_PKG_NAME.

                                                                   static
                                                        ExceptionInfoCode

   MISSING_ITEM_NAME
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_MISSING_ITEM_NAME.

                                                                   static
                                                        ExceptionInfoCode

   MISSING_PKG_PARAM
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_MISSING_PKG_PARAM.

                                                                   static
                                                        ExceptionInfoCode

   MISSING_INFO_IN_OBJ
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_MISSING_INFO_IN_OBJ.

                                                                   static
                                                        ExceptionInfoCode

   BOTH_KA_EMBEDSIG_PRESENT
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_BOTH_KA_EMBEDSIG_PRESENT.

                                                                   static
                                                        ExceptionInfoCode

   INV_STREAM_ID
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_STREAM_ID.

                                                                   static
                                                        ExceptionInfoCode

   INV_TIMER_VAL
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_TIMER_VAL.

                                                                   static
                                                        ExceptionInfoCode

   INV_DIGIT_VAL
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_DIGIT_VAL.

                                                                   static
                                                        ExceptionInfoCode

   NULL_OBJ_REF_PASSED
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_NULL_OBJ_REF_PASSED.

                                                                   static
                                                        ExceptionInfoCode

   INV_SIGNAL_LIST_ID
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_SIGNAL_LIST_ID.

                                                                   static
                                                        ExceptionInfoCode

   MISSING_SIGNAL_LIST_ID
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_MISSING_SIGNAL_LIST_ID.

                                                                   static
                                                        ExceptionInfoCode

   INV_OPR_ON_SIGNAL_PARAM
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_OPR_ON_SIGNAL_PARAM.

                                                                   static
                                                        ExceptionInfoCode

   INV_SIGNAL_DURATION
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_SIGNAL_DURATION.

                                                                   static
                                                        ExceptionInfoCode

   BOTH_STREAM_PARAM_DESC_PRESENT
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_BOTH_STREAM_PARAM_DESC_PRESENT.

                                                                   static
                                                        ExceptionInfoCode

   INV_SDP_PARAM
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_SDP_PARAM.

                                                                   static
                                                        ExceptionInfoCode

   MISSING_MEDIA_STREAM_PARAM
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_MISSING_MEDIA_STREAM_PARAM.

                                                                   static
                                                        ExceptionInfoCode

   MISSING_STREAM_ID
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_MISSING_STREAM_ID.

                                                                   static
                                                        ExceptionInfoCode

   MISSING_EXTENSION_PARAM
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_MISSING_EXTENSION_PARAM.

                                                                   static
                                                        ExceptionInfoCode

   INV_PROTOCOL_VERSION
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_PROTOCOL_VERSION.

                                                                   static
                                                        ExceptionInfoCode

   INV_DATE_STRING
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_DATE_STRING.

                                                                   static
                                                        ExceptionInfoCode

   INV_TIME_STRING
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_TIME_STRING.

                                                                   static
                                                        ExceptionInfoCode

   INV_MEDIA_DESC
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_MEDIA_DESC.

                                                                   static
                                                        ExceptionInfoCode

   INV_DIGITMAP_DESC
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_DIGITMAP_DESC.

                                                                   static
                                                        ExceptionInfoCode

   INV_PKG_DESC
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_PKG_DESC.

                                                                   static
                                                        ExceptionInfoCode

   INV_ERROR_CODE
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_ERROR_CODE.

                                                                   static
                                                        ExceptionInfoCode

   INV_CNTX_TERM_AUDIT_DESC
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_CNTX_TERM_AUDIT_DESC.

                                                                   static
                                                        ExceptionInfoCode

   ERROR_DESC_ALRDY_PRESENT
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_ERROR_DESC_ALRDY_PRESENT.

                                                                   static
                                                        ExceptionInfoCode

   TERMID_LIST_ALRDY_PRESENT
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_TERMID_LIST_ALRDY_PRESENT.

                                                                   static
                                                        ExceptionInfoCode

   INV_YEAR_VAL
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_YEAR_VAL.

                                                                   static
                                                        ExceptionInfoCode

   INV_MONTH_VAL
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_MONTH_VAL.

                                                                   static
                                                        ExceptionInfoCode

   INV_DAY_VAL
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_DAY_VAL.

                                                                   static
                                                        ExceptionInfoCode

   INV_HOUR_VAL
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_HOUR_VAL.

                                                                   static
                                                        ExceptionInfoCode

   INV_MINUTES_VAL
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_MINUTES_VAL.

                                                                   static
                                                        ExceptionInfoCode

   INV_SECONDS_VAL
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_SECONDS_VAL.

                                                                   static
                                                        ExceptionInfoCode

   INV_ASSOCIATED_PKD_ID
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_ASSOCIATED_PKD_ID.

                                                                   static
                                                        ExceptionInfoCode

   INV_PKG_ITEM_PARAM
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_PKG_ITEM_PARAM.

                                                                   static
                                                        ExceptionInfoCode

   INV_RETURN_STATUS_VALUE
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_RETURN_STATUS_VALUE.

                                                                   static
                                                        ExceptionInfoCode

   INV_ASSOC_EVENT_TYPE
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_ASSOC_EVENT_TYPE.

                                                                   static
                                                        ExceptionInfoCode

   INV_ASSOC_IND_REASON
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_ASSOC_IND_REASON.

                                                                   static
                                                        ExceptionInfoCode

   INV_ASSOC_STATE
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_ASSOC_STATE.

                                                                   static
                                                        ExceptionInfoCode

   INV_ENC_FORMAT
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_ENC_FORMAT.

                                                                   static
                                                        ExceptionInfoCode

   INV_SVC_CHNG_METHOD
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_SVC_CHNG_METHOD.

                                                                   static
                                                        ExceptionInfoCode

   INV_SVC_CHNG_REASON
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_SVC_CHNG_REASON.

                                                                   static
                                                        ExceptionInfoCode

   INV_TPT_TYPE
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_TPT_TYPE.

                                                                   static
                                                        ExceptionInfoCode

   INV_CMD_REQ_TYPE
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_CMD_REQ_TYPE.

                                                                   static
                                                        ExceptionInfoCode

   INV_CMD_RESP_TYPE
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_CMD_RESP_TYPE.

                                                                   static
                                                        ExceptionInfoCode

   INV_CMD_TYPE
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_CMD_TYPE.

                                                                   static
                                                        ExceptionInfoCode

   INV_DESC_TYPE
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_DESC_TYPE.

                                                                   static
                                                        ExceptionInfoCode

   INV_TERM_TYPE
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_TERM_TYPE.

                                                                   static
                                                        ExceptionInfoCode

   INV_EVT_BUFFER_CTRL
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_EVT_BUFFER_CTRL.

                                                                   static
                                                        ExceptionInfoCode

   INV_MODEM_TYPE
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_MODEM_TYPE.

                                                                   static
                                                        ExceptionInfoCode

   INV_MUX_TYPE
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_MUX_TYPE.

                                                                   static
                                                        ExceptionInfoCode

   INV_SERVICE_STATE
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_SERVICE_STATE.

                                                                   static
                                                        ExceptionInfoCode

   INV_SIGNAL_PARAM_TYPE
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_SIGNAL_PARAM_TYPE.

                                                                   static
                                                        ExceptionInfoCode

   INV_SIGNAL_TYPE
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_SIGNAL_TYPE.

                                                                   static
                                                        ExceptionInfoCode

   INV_STREAM_MODE
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_STREAM_MODE.

                                                                   static
                                                        ExceptionInfoCode

   INV_TOPOLOGY_DIR
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_TOPOLOGY_DIR.

                                                                   static
                                                        ExceptionInfoCode

   INV_PARAM_VAL_TYPE
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_PARAM_VAL_TYPE.

                                                                   static
                                                        ExceptionInfoCode

   INV_PARAM_RELATION
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_PARAM_RELATION.

                                                                   static
                                                        ExceptionInfoCode

   INV_PKG_ITEM_TYPE
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_PKG_ITEM_TYPE.

                                                                   static
                                                        ExceptionInfoCode

   INV_PRIORITY_VAL
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_INV_PRIORITY_VAL.

                                                                   static
                                                        ExceptionInfoCode

   DUPLICATE_DESC_IN_CMD
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_DUPLICATE_DESC_IN_CMD.

                                                                   static
                                                        ExceptionInfoCode

   MISSING_DIGIT_WITH_QUALIFIER_DOT
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_MISSING_DIGIT_WITH_QUALIFIER_DOT.

                                                                   static
                                                        ExceptionInfoCode

   SYNTAX_ERR_IN_TERM_NAME
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_SYNTAX_ERR_IN_TERM_NAME.

                                                                   static
                                                        ExceptionInfoCode

   SYNTAX_ERR_IN_DMAP_NAME
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_SYNTAX_ERR_IN_DMAP_NAME.

                                                                   static
                                                        ExceptionInfoCode

   SYNTAX_ERR_IN_SVC_CHNG_PROFILE
              Identifies a ExceptionInfoCode object that constructs the
   class with the constant M_SYNTAX_ERR_IN_SVC_CHNG_PROFILE.

   Constructor Summary
   ExceptionInfoCode(int info_code)
              Constructs a class initialised with value exception info
   code as passed to it in the constructor.

   Method Summary
   int getExceptionInfoCode()
             Returns one of the allowed integer values defined as static
   fields in this class which shall identify one of the ExceptionInfoCode
   codes.
   int readResolve()
              throws ObjectStreamException()

              This method is required to perform instance substitution
   during serialization.

   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll,
   toString, wait

   Field Detail

  M_INV_IP_ADDR_FORMAT

public static final int M_INV_IP_ADDR_FORMAT

          Identifies that the IP Address string passed to the method is
          invalid as per the protocol.
          Its value shall be set to 1.
     _________________________________________________________________

  M_INV_MTP3B_ADDR_FORMAT

public static final int M_INV_MTP3B_ADDR_FORMAT

          Identifies that the MTP3b Address string passed to the method
          is invalid as per what has been defined by the class.
          Its value shall be set to 2.
     _________________________________________________________________

  M_INV_AAL5_ADDR_FORMAT

public static final int M_INV_AAL5_ADDR_FORMAT

          Identifies that the AAL5 Address string passed to the method is
          invalid as per what has been defined by the class.
          Its value shall be set to 3.
     _________________________________________________________________

  M_INV_LOCAL_ADDR

public static final int M_INV_LOCAL_ADDR

          Identifies that the Local Address object reference contains
          incorrect or incomplete information.
          Its value shall be set to 4.
     _________________________________________________________________

  M_INV_REMOTE_ADDR

public static final int M_INV_REMOTE_ADDR

          Identifies that the Remote Address object reference contains
          incorrect or incomplete information.
          Its value shall be set to 5.
     _________________________________________________________________

  M_INV_ERROR_CODE

public static final int M_INV_ERROR_CODE

          Identifies that the error code object reference is not valid
          for the class for which it is being set. For example
          CreateAssocResp class should return error code corresponding to
          CreateAssocReq only. If any othen error code is set, then
          exception is raised with this info code.
          Its value shall be set to 6.
     _________________________________________________________________

  M_INV_EVT_STATUS_VAL

public static final int M_INV_EVT_STATUS_VAL

          Identifies that the event status valus specified for this class
          is invalid. This will also be returned in case error code is
          being set for a class, but the event status is set to
          M_SUCCESS.
          Its value shall be set to 7.
     _________________________________________________________________

  M_INV_REQ_ID

public static final int M_INV_REQ_ID

          Identifies that the request Id passed to the method is invalid.
          This will be returned if only positive values are allowed and
          request Id passed is negative.
          Its value shall be set to 8.
     _________________________________________________________________

  M_MISSING_EVT_PARAM

public static final int M_MISSING_EVT_PARAM

          Identifies that the event parameters are missing in the class
          RequestedEventParam object.
          Its value shall be set to 9.
     _________________________________________________________________

  M_SYNTAX_ERR_IN_SVC_CHNG_PROFILE

public static final int M_SYNTAX_ERR_IN_SVC_CHNG_PROFILE

          Identifies that the service change profile is incorrect as per
          the syntax defined by the protocol. The service change profile
          should not begin with the numeric character.
          Its value shall be set to 10.
     _________________________________________________________________

  M_MISSING_PKG_NAME

public static final int M_MISSING_PKG_NAME

          Identifies that the package name is missing in the object
          reference.
          Its value shall be set to 11.
     _________________________________________________________________

  M_MISSING_ITEM_NAME

public static final int M_MISSING_ITEM_NAME

          Identifies that the item name is missing in the object
          reference.
          Its value shall be set to 12.
     _________________________________________________________________

  M_MISSING_PKG_PARAM

public static final int M_MISSING_PKG_PARAM

          Identifies that the package parameter is missing in the object
          reference.
          Its value shall be set to 13.
     _________________________________________________________________

  M_MISSING_INFO_IN_OBJ

public static final int M_MISSING_INFO_IN_OBJ

          Identifies that the class object contains no information. For
          example class LocalAddr object can be created with empty
          constructor. If further no parameters are set in it and the
          reference of the same is passed to a method, then this
          exception is raised.
          Its value shall be set to 14.
     _________________________________________________________________

  M_BOTH_KA_EMBEDSIG_PRESENT

public static final int M_BOTH_KA_EMBEDSIG_PRESENT

          Identifies that both Keep Active and Embed with Signal
          Descriptor are present in the event params.
          Its value shall be set to 15.
     _________________________________________________________________

  M_INV_STREAM_ID

public static final int M_INV_STREAM_ID

          Identifies that the stream Id passed to the method is invalid.
          This will be returned if only positive values are allowed and
          stream Id passed is negative.
          Its value shall be set to 16.
     _________________________________________________________________

  M_SYNTAX_ERR_IN_DMAP_NAME

public static final int M_SYNTAX_ERR_IN_DMAP_NAME

          Identifies that the digitmap name as passed in the method is
          not correct as per the syntax defined by the protocol. The
          digitmap name must not begin with the numeric character.
          Its value shall be set to 17.
     _________________________________________________________________

  M_INV_TIMER_VAL

public static final int M_INV_TIMER_VAL

          Identifies that the timer value as passed in the method is not
          correct as per the syntax defined by the protocol. As per
          protocol it shouls have maximum of 2 digits.
          Its value shall be set to 18.
     _________________________________________________________________

  M_INV_DIGIT_VAL

public static final int M_INV_DIGIT_VAL

          Identifies that the digit value specified for a digitpoisition
          in the digitmap is invalid. The digits specified must be in the
          range DIGIT0 to DIGITX.
          Its value shall be set to 19.
     _________________________________________________________________

  M_NULL_OBJ_REF_PASSED

public static final int M_NULL_OBJ_REF_PASSED

          Identifies that the NULL object reference has been passed to
          the method. This would be raised if NULL reference is passed to
          a method which is expecting a valid reference.
          Its value shall be set to 20.
     _________________________________________________________________

  M_INV_SIGNAL_LIST_ID

public static final int M_INV_SIGNAL_LIST_ID

          Identifies that the signal list Id specified is not correct as
          per the syntax of the protocol.
          Its value shall be set to 21.
     _________________________________________________________________

  M_MISSING_SIGNAL_LIST_ID

public static final int M_MISSING_SIGNAL_LIST_ID

          Identifies that the signal param type has been set to
          M_SIGNAL_PARAM_LIST but the signal param list Id is missing in
          the class SignalParam.
          Its value shall be set to 22.
     _________________________________________________________________

  M_INV_OPR_ON_SIGNAL_PARAM

public static final int M_INV_OPR_ON_SIGNAL_PARAM

          Identifies that the signal param type is set to
          M_SIGNAL_PARAM_REQUEST. This would occur if the a method is
          invoked which is invalid for this signal param type. For
          example if setSignalParamListId method is invoked for the
          signal type M_SIGNAL_PARAM_REQUEST, then the exception is
          raised with this info code.
          Its value shall be set to 23.
     _________________________________________________________________

  M_INV_SIGNAL_DURATION

public static final int M_INV_SIGNAL_DURATION

          Identifies that the signal duration specified is invalid as per
          the syntax defined by the protocol.
          Its value shall be set to 24.
     _________________________________________________________________

  M_BOTH_STREAM_PARAM_DESC_PRESENT

public static final int M_BOTH_STREAM_PARAM_DESC_PRESENT

          Identifies that the both stream param and stream descriptor are
          present in the media descriptor, which is not valid as per the
          protocol.
          Its value shall be set to 25.
     _________________________________________________________________

  M_INV_SDP_PARAM

public static final int M_INV_SDP_PARAM

          Identifies that the parameter specified in the SDP is invalid.
          Its value shall be set to 26.
     _________________________________________________________________

  M_MISSING_MEDIA_STREAM_PARAM

public static final int M_MISSING_MEDIA_STREAM_PARAM

          Identifies that the the media stream parameters are missing in
          the stream descriptor.
          Its value shall be set to 27.
     _________________________________________________________________

  M_MISSING_STREAM_ID

public static final int M_MISSING_STREAM_ID

          Identifies that the stream Id is missing in the stream
          descriptor.
          Its value shall be set to 28.
     _________________________________________________________________

  M_MISSING_EXTENSION_PARAM

public static final int M_MISSING_EXTENSION_PARAM

          Identifies that the extension parameter is missing in the class
          object. This will be returned if in case the get method for
          extension parameter is invoked without checking for its
          presence.
          Its value shall be set to 29.
     _________________________________________________________________

  M_SYNTAX_ERR_IN_TERM_NAME

public static final int M_SYNTAX_ERR_IN_TERM_NAME

          Identifies that the termination name as passed in the method is
          not correct as per the syntax defined by the protocol. The
          termination name must not begin with a numeric character.
          Its value shall be set to 30.
     _________________________________________________________________

  M_INV_PROTOCOL_VERSION

public static final int M_INV_PROTOCOL_VERSION

          Identifies that the protocol version is incorrect as per the
          range defined by the protocol.
          Its value shall be set to 31.
     _________________________________________________________________

  M_INV_DATE_STRING

public static final int M_INV_DATE_STRING

          Identifies that the date value is not in the format defined by
          the protocol. As per protocol, the date must be specified in
          the format "yyyymmdd".
          Its value shall be set to 32.
     _________________________________________________________________

  M_INV_TIME_STRING

public static final int M_INV_TIME_STRING

          Identifies that the time value is not in the format defined by
          the protocol. As per protocol, the date must be specified in
          the format "hhmmssss".
          Its value shall be set to 33.
     _________________________________________________________________

  M_INV_MEDIA_DESC

public static final int M_INV_MEDIA_DESC

          Identifies that the media descriptor is invalid. This would be
          returned if it contains no information.
          Its value shall be set to 34.
     _________________________________________________________________

  M_INV_DIGITMAP_DESC

public static final int M_INV_DIGITMAP_DESC

          Identifies that the digitmap descriptor is invalid. This would
          be returned if it contains no information.
          Its value shall be set to 35.
     _________________________________________________________________

  M_INV_PKG_DESC

public static final int M_INV_PKG_DESC

          Identifies that the package descriptor is invalid. This would
          be returned if it contains no information.
          Its value shall be set to 36.
     _________________________________________________________________

  M_INV_CNTX_TERM_AUDIT_DESC

public static final int M_INV_CNTX_TERM_AUDIT_DESC

          Identifies that the context termination audit descriptor is
          invalid. This would be returned if it contains no information.
          Its value shall be set to 37.
     _________________________________________________________________

  M_ERROR_DESC_ALRDY_PRESENT

public static final int M_ERROR_DESC_ALRDY_PRESENT

          Identifies that the error descriptor is already present for the
          class object. So in this case no other parameters can be set.
          For example in class ContextTermAuditDescriptor both termIdList
          and error descriptor cannot co-exist.
          Its value shall be set to 38.
     _________________________________________________________________

  M_TERMID_LIST_ALRDY_PRESENT

public static final int M_TERMID_LIST_ALRDY_PRESENT

          Identifies that the terminationId list is already present for
          the class object. So in this case if set error descriptor is
          invoked, then exception is raised. For example in class
          ContextTermAuditDescriptor both termIdList and error descriptor
          cannot co-exist.
          Its value shall be set to 39.
     _________________________________________________________________

  M_INV_YEAR_VAL

public static final int M_INV_YEAR_VAL

          Identifies that the value of year specified is invalid. If the
          value of year in not in the valid range, then the exception is
          raised.
          Its value shall be set to 40.
     _________________________________________________________________

  M_INV_MONTH_VAL

public static final int M_INV_MONTH_VAL

          Identifies that the value of month specified is invalid. If the
          value of month in not in the valid range, then the exception is
          raised.
          Its value shall be set to 41.
     _________________________________________________________________

  M_INV_DAY_VAL

public static final int M_INV_DAY_VAL

          Identifies that the value of day specified is invalid. If the
          value of day in not in the valid range, then the exception is
          raised.
          Its value shall be set to 42.
     _________________________________________________________________

  M_INV_HOUR_VAL

public static final int M_INV_HOUR_VAL

          Identifies that the value of hour specified is invalid. If the
          value of hour in not in the valid range, then the exception is
          raised.
          Its value shall be set to 43.
     _________________________________________________________________

  M_INV_MINUTES_VAL

public static final int M_INV_MINUTES_VAL

          Identifies that the value of minutes specified is invalid. If
          the value of minutes in not in the valid range, then the
          exception is raised.
          Its value shall be set to 44.
     _________________________________________________________________

  M_INV_SECONDS_VAL

public static final int M_INV_SECONDS_VAL

          Identifies that the value of seconds specified is invalid. If
          the value of seconds in not in the valid range, then the
          exception is raised.
          Its value shall be set to 45.
     _________________________________________________________________

  M_INV_ASSOCIATED_PKD_ID

public static final int M_INV_ASSOCIATED_PKD_ID

          Identifies that the value of associated package Id is not
          valid. The associated package Id passed in the method is not
          one amongst the package Ids that this package extends for the
          specified item.
          Its value shall be set to 46.
     _________________________________________________________________

  M_INV_PKG_ITEM_PARAM

public static final int M_INV_PKG_ITEM_PARAM

          Identifies that the package item param passed for the item are
          invalid. The package item param contains information of the
          associated item and the associated package. If these values
          donot match with the item for which these parameters are
          specified, then the exception with this info code is raised.
          Its value shall be set to 47.
     _________________________________________________________________

  M_INV_RETURN_STATUS_VALUE

public static final int M_INV_RETURN_STATUS_VALUE

          Identifies that the value of the return status as passed to the
          method is invalid.
          Its value shall be set to 48.
     _________________________________________________________________

  M_INV_ASSOC_EVENT_TYPE

public static final int M_INV_ASSOC_EVENT_TYPE

          Identifies that the value of the association event type as
          passed to the method is invalid.
          Its value shall be set to 49.
     _________________________________________________________________

  M_INV_ASSOC_IND_REASON

public static final int M_INV_ASSOC_IND_REASON

          Identifies that the value of the association indication reason
          as passed to the method is invalid.
          Its value shall be set to 50.
     _________________________________________________________________

  M_INV_ASSOC_STATE

public static final int M_INV_ASSOC_STATE

          Identifies that the value of the association state as passed to
          the method is invalid.
          Its value shall be set to 51.
     _________________________________________________________________

  M_INV_ENC_FORMAT

public static final int M_INV_ENC_FORMAT

          Identifies that the value of the encoding format as passed to
          the method is invalid.
          Its value shall be set to 52.
     _________________________________________________________________

  M_INV_SVC_CHNG_METHOD

public static final int M_INV_SVC_CHNG_METHOD

          Identifies that the value of the service change method as
          passed to the method is invalid.
          Its value shall be set to 53.
     _________________________________________________________________

  M_INV_SVC_CHNG_REASON

public static final int M_INV_SVC_CHNG_REASON

          Identifies that the value of the service change reason as
          passed to the method is invalid.
          Its value shall be set to 54.
     _________________________________________________________________

  M_INV_TPT_TYPE

public static final int M_INV_TPT_TYPE

          Identifies that the value of the transport type as passed to
          the method is invalid.
          Its value shall be set to 55.
     _________________________________________________________________

  M_INV_CMD_REQ_TYPE

public static final int M_INV_CMD_REQ_TYPE

          Identifies that the value of the command request type as passed
          to the method is invalid.
          Its value shall be set to 56.
     _________________________________________________________________

  M_INV_CMD_RESP_TYPE

public static final int M_INV_CMD_RESP_TYPE

          Identifies that the value of the command response type as
          passed to the method is invalid.
          Its value shall be set to 57.
     _________________________________________________________________

  M_INV_CMD_TYPE

public static final int M_INV_CMD_TYPE

          Identifies that the value of the command type as passed to the
          method is invalid.
          Its value shall be set to 58.
     _________________________________________________________________

  M_INV_DESC_TYPE

public static final int M_INV_DESC_TYPE

          Identifies that the value of the descriptor type as passed to
          the method is invalid.
          Its value shall be set to 59.
     _________________________________________________________________

  M_INV_TERM_TYPE

public static final int M_INV_TERM_TYPE

          Identifies that the value of the termination type as passed to
          the method is invalid.
          Its value shall be set to 60.
     _________________________________________________________________

  M_INV_EVT_BUFFER_CTRL

public static final int M_INV_EVT_BUFFER_CTRL

          Identifies that the value of the event buffer control as passed
          to the method is invalid.
          Its value shall be set to 61.
     _________________________________________________________________

  M_INV_MODEM_TYPE

public static final int M_INV_MODEM_TYPE

          Identifies that the value of the modem type as passed to the
          method is invalid.
          Its value shall be set to 62.
     _________________________________________________________________

  M_INV_MUX_TYPE

public static final int M_INV_MUX_TYPE

          Identifies that the value of the mux type as passed to the
          method is invalid.
          Its value shall be set to 63.
     _________________________________________________________________

  M_INV_SERVICE_STATE

public static final int M_INV_SERVICE_STATE

          Identifies that the value of the service state as passed to the
          method is invalid.
          Its value shall be set to 64.
     _________________________________________________________________

  M_INV_SIGNAL_PARAM_TYPE

public static final int M_INV_SIGNAL_PARAM_TYPE

          Identifies that the value of the signal param type as passed to
          the method is invalid.
          Its value shall be set to 65.
     _________________________________________________________________

  M_INV_SIGNAL_TYPE

public static final int M_INV_SIGNAL_TYPE

          Identifies that the value of the signal type as passed to the
          method is invalid.
          Its value shall be set to 66.
     _________________________________________________________________

  M_INV_STREAM_MODE

public static final int M_INV_STREAM_MODE

          Identifies that the value of the stream mode as passed to the
          method is invalid.
          Its value shall be set to 67.
     _________________________________________________________________

  M_INV_TOPOLOGY_DIR

public static final int M_INV_TOPOLOGY_DIR

          Identifies that the value of the topology direction as passed
          to the method is invalid.
          Its value shall be set to 68.
     _________________________________________________________________

  M_INV_PARAM_VAL_TYPE

public static final int M_INV_PARAM_VAL_TYPE

          Identifies that the value of the param value type as passed to
          the method is invalid.
          Its value shall be set to 69.
     _________________________________________________________________

  M_INV_PARAM_RELATION

public static final int M_INV_PARAM_RELATION

          Identifies that the value of the parameter relation as passed
          to the method is invalid.
          Its value shall be set to 70.
     _________________________________________________________________

  M_INV_PKG_ITEM_TYPE

public static final int M_INV_PKG_ITEM_TYPE

          Identifies that the value of the package item type as passed to
          the method is invalid.
          Its value shall be set to 71.
     _________________________________________________________________

  M_INV_PRIORITY_VAL

public static final int M_INV_PRIORITY_VAL

          Identifies that the value of priority as passed to the method
          is invalid. The valid valued for Priority as defined by the
          protocol is between the range 0 to 15. For any value other than
          this range, exception is raised with this info code.
          Its value shall be set to 72.
     _________________________________________________________________

  M_DUPLICATE_DESC_IN_CMD

public static final int M_DUPLICATE_DESC_IN_CMD

          Identifies that same descriptor has been set twice in the
          command. This will be retuned if for example Media Descriptor
          is set twice in the command request.
          Its value shall be set to 73.
     _________________________________________________________________

  M_MISSING_DIGIT_WITH_QUALIFIER_DOT

public static final int M_MISSING_DIGIT_WITH_QUALIFIER_DOT

          According to the protocol, DOT is a digit qualifier. At a digit
          position, DOT qualiifies the associated digit. In case if only
          DOT is specified for the digit position, then exception is
          raised with info code set to this.
          Its value shall be set to 74.
     _________________________________________________________________

  INV_IP_ADDR_FORMAT

public static final ExceptionInfoCode INV_IP_ADDR_FORMAT
        = new ExceptionInfoCode(M_INV_IP_ADDR_FORMAT);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_IP_ADDR_FORMAT. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  INV_MTP3B_ADDR_FORMAT

public static final ExceptionInfoCode INV_MTP3B_ADDR_FORMAT
        = new ExceptionInfoCode(M_INV_MTP3B_ADDR_FORMAT);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_MTP3B_ADDR_FORMAT. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  INV_AAL5_ADDR_FORMAT

public static final ExceptionInfoCode INV_AAL5_ADDR_FORMAT
        = new ExceptionInfoCode(M_INV_AAL5_ADDR_FORMAT);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_AAL5_ADDR_FORMAT. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  INV_LOCAL_ADDR

public static final ExceptionInfoCode INV_LOCAL_ADDR
        = new ExceptionInfoCode(M_INV_LOCAL_ADDR);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_LOCAL_ADDR. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_REMOTE_ADDR

public static final ExceptionInfoCode INV_REMOTE_ADDR
        = new ExceptionInfoCode(M_INV_REMOTE_ADDR);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_REMOTE_ADDR. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_ERROR_CODE

public static final ExceptionInfoCode INV_ERROR_CODE
        = new ExceptionInfoCode(M_INV_ERROR_CODE);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_ERROR_CODE. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_EVT_STATUS_VAL

public static final ExceptionInfoCode INV_EVT_STATUS_VAL
        = new ExceptionInfoCode(M_INV_EVT_STATUS_VAL);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_EVT_STATUS_VAL. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  INV_REQ_ID

public static final ExceptionInfoCode INV_REQ_ID
        = new ExceptionInfoCode(M_INV_REQ_ID);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_REQ_ID. Since it is reference to static
          final object, it prevents further instantiation of the same
          object in the system.
     _________________________________________________________________

  MISSING_EVT_PARAM

public static final ExceptionInfoCode MISSING_EVT_PARAM
        = new ExceptionInfoCode(M_MISSING_EVT_PARAM);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_MISSING_EVT_PARAM. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  MISSING_PKG_NAME

public static final ExceptionInfoCode MISSING_PKG_NAME
        = new ExceptionInfoCode(M_MISSING_PKG_NAME);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_MISSING_PKG_NAME. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  MISSING_ITEM_NAME

public static final ExceptionInfoCode MISSING_ITEM_NAME
        = new ExceptionInfoCode(M_MISSING_ITEM_NAME);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_MISSING_ITEM_NAME. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  MISSING_PKG_PARAM

public static final ExceptionInfoCode MISSING_PKG_PARAM
        = new ExceptionInfoCode(M_MISSING_PKG_PARAM);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_MISSING_PKG_PARAM. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  MISSING_INFO_IN_OBJ

public static final ExceptionInfoCode MISSING_INFO_IN_OBJ
        = new ExceptionInfoCode(M_MISSING_INFO_IN_OBJ);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_MISSING_INFO_IN_OBJ. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  BOTH_KA_EMBEDSIG_PRESENT

public static final ExceptionInfoCode BOTH_KA_EMBEDSIG_PRESENT
        = new ExceptionInfoCode(M_BOTH_KA_EMBEDSIG_PRESENT);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_BOTH_KA_EMBEDSIG_PRESENT. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system. descriptor.
     _________________________________________________________________

  INV_STREAM_ID

public static final ExceptionInfoCode INV_STREAM_ID
        = new ExceptionInfoCode(M_INV_STREAM_ID);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_STREAM_ID. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_TIMER_VAL

public static final ExceptionInfoCode INV_TIMER_VAL
        = new ExceptionInfoCode(M_INV_TIMER_VAL);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_TIMER_VAL. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_DIGIT_VAL

public static final ExceptionInfoCode INV_DIGIT_VAL
        = new ExceptionInfoCode(M_INV_DIGIT_VAL);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_DIGIT_VAL. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  NULL_OBJ_REF_PASSED

public static final ExceptionInfoCode NULL_OBJ_REF_PASSED
        = new ExceptionInfoCode(M_NULL_OBJ_REF_PASSED);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_NULL_OBJ_REF_PASSED. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  INV_SIGNAL_LIST_ID

public static final ExceptionInfoCode INV_SIGNAL_LIST_ID
        = new ExceptionInfoCode(M_INV_SIGNAL_LIST_ID);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_SIGNAL_LIST_ID. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  MISSING_SIGNAL_LIST_ID

public static final ExceptionInfoCode MISSING_SIGNAL_LIST_ID
        = new ExceptionInfoCode(M_MISSING_SIGNAL_LIST_ID);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_MISSING_SIGNAL_LIST_ID. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  INV_OPR_ON_SIGNAL_PARAM

public static final ExceptionInfoCode INV_OPR_ON_SIGNAL_PARAM
        = new ExceptionInfoCode(M_INV_OPR_ON_SIGNAL_PARAM);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_OPR_ON_SIGNAL_PARAM. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  INV_SIGNAL_DURATION

public static final ExceptionInfoCode INV_SIGNAL_DURATION
        = new ExceptionInfoCode(M_INV_SIGNAL_DURATION);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_SIGNAL_DURATION. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  BOTH_STREAM_PARAM_DESC_PRESENT

public static final ExceptionInfoCode BOTH_STREAM_PARAM_DESC_PRESENT
        = new ExceptionInfoCode(M_BOTH_STREAM_PARAM_DESC_PRESENT);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_BOTH_STREAM_PARAM_DESC_PRESENT. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  INV_SDP_PARAM

public static final ExceptionInfoCode INV_SDP_PARAM
        = new ExceptionInfoCode(M_INV_SDP_PARAM);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_SDP_PARAM. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  MISSING_MEDIA_STREAM_PARAM

public static final ExceptionInfoCode MISSING_MEDIA_STREAM_PARAM
        = new ExceptionInfoCode(M_MISSING_MEDIA_STREAM_PARAM);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_MISSING_MEDIA_STREAM_PARAM. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  MISSING_STREAM_ID

public static final ExceptionInfoCode MISSING_STREAM_ID
        = new ExceptionInfoCode(M_MISSING_STREAM_ID);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_MISSING_STREAM_ID. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  MISSING_EXTENSION_PARAM

public static final ExceptionInfoCode MISSING_EXTENSION_PARAM
        = new ExceptionInfoCode(M_MISSING_EXTENSION_PARAM);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_MISSING_EXTENSION_PARAM. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  INV_PROTOCOL_VERSION

public static final ExceptionInfoCode INV_PROTOCOL_VERSION
        = new ExceptionInfoCode(M_INV_PROTOCOL_VERSION);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_PROTOCOL_VERSION. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  INV_DATE_STRING

public static final ExceptionInfoCode INV_DATE_STRING
        = new ExceptionInfoCode(M_INV_DATE_STRING);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_DATE_STRING. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_TIME_STRING

public static final ExceptionInfoCode INV_TIME_STRING
        = new ExceptionInfoCode(M_INV_TIME_STRING);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_TIME_STRING. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_MEDIA_DESC

public static final ExceptionInfoCode INV_MEDIA_DESC
        = new ExceptionInfoCode(M_INV_MEDIA_DESC);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_MEDIA_DESC. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_DIGITMAP_DESC

public static final ExceptionInfoCode INV_DIGITMAP_DESC
        = new ExceptionInfoCode(M_INV_DIGITMAP_DESC);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_DIGITMAP_DESC. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_PKG_DESC

public static final ExceptionInfoCode INV_PKG_DESC
        = new ExceptionInfoCode(M_INV_PKG_DESC);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_PKG_DESC. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_CNTX_TERM_AUDIT_DESC

public static final ExceptionInfoCode INV_CNTX_TERM_AUDIT_DESC
        = new ExceptionInfoCode(M_INV_CNTX_TERM_AUDIT_DESC);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_CNTX_TERM_AUDIT_DESC. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  ERROR_DESC_ALRDY_PRESENT

public static final ExceptionInfoCode ERROR_DESC_ALRDY_PRESENT
        = new ExceptionInfoCode(M_ERROR_DESC_ALRDY_PRESENT);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_ERROR_DESC_ALRDY_PRESENT. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  TERMID_LIST_ALRDY_PRESENT

public static final ExceptionInfoCode TERMID_LIST_ALRDY_PRESENT
        = new ExceptionInfoCode(M_TERMID_LIST_ALRDY_PRESENT);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_TERMID_LIST_ALRDY_PRESENT. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  INV_YEAR_VAL

public static final ExceptionInfoCode INV_YEAR_VAL
        = new ExceptionInfoCode(M_INV_YEAR_VAL);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_YEAR_VAL. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_MONTH_VAL

public static final ExceptionInfoCode INV_MONTH_VAL
        = new ExceptionInfoCode(M_INV_MONTH_VAL);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_MONTH_VAL. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_DAY_VAL

public static final ExceptionInfoCode INV_DAY_VAL
        = new ExceptionInfoCode(M_INV_DAY_VAL);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_DAY_VAL. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_HOUR_VAL

public static final ExceptionInfoCode INV_HOUR_VAL
        = new ExceptionInfoCode(M_INV_HOUR_VAL);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_HOUR_VAL. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_MINUTES_VAL

public static final ExceptionInfoCode INV_MINUTES_VAL
        = new ExceptionInfoCode(M_INV_MINUTES_VAL);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_MINUTES_VAL. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_SECONDS_VAL

public static final ExceptionInfoCode INV_SECONDS_VAL
        = new ExceptionInfoCode(M_INV_SECONDS_VAL);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_SECONDS_VAL. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_ASSOCIATED_PKD_ID

public static final ExceptionInfoCode INV_ASSOCIATED_PKD_ID
        = new ExceptionInfoCode(M_INV_ASSOCIATED_PKD_ID);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_ASSOCIATED_PKD_ID. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  INV_PKG_ITEM_PARAM

public static final ExceptionInfoCode INV_PKG_ITEM_PARAM
        = new ExceptionInfoCode(M_INV_PKG_ITEM_PARAM);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_PKG_ITEM_PARAM. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  INV_RETURN_STATUS_VALUE

public static final ExceptionInfoCode INV_RETURN_STATUS_VALUE
        = new ExceptionInfoCode(M_INV_RETURN_STATUS_VALUE);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_RETURN_STATUS_VALUE. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  INV_ASSOC_EVENT_TYPE

public static final ExceptionInfoCode INV_ASSOC_EVENT_TYPE
        = new ExceptionInfoCode(M_INV_ASSOC_EVENT_TYPE);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_ASSOC_EVENT_TYPE. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  INV_ASSOC_IND_REASON

public static final ExceptionInfoCode INV_ASSOC_IND_REASON
        = new ExceptionInfoCode(M_INV_ASSOC_IND_REASON);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_ASSOC_IND_REASON. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  INV_ASSOC_STATE

public static final ExceptionInfoCode INV_ASSOC_STATE
        = new ExceptionInfoCode(M_INV_ASSOC_STATE);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_ASSOC_STATE. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_ENC_FORMAT

public static final ExceptionInfoCode INV_ENC_FORMAT
        = new ExceptionInfoCode(M_INV_ENC_FORMAT);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_ENC_FORMAT. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_SVC_CHNG_METHOD

public static final ExceptionInfoCode INV_SVC_CHNG_METHOD
        = new ExceptionInfoCode(M_INV_SVC_CHNG_METHOD);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_SVC_CHNG_METHOD. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  INV_SVC_CHNG_REASON

public static final ExceptionInfoCode INV_SVC_CHNG_REASON
        = new ExceptionInfoCode(M_INV_SVC_CHNG_REASON);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_SVC_CHNG_REASON. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  INV_TPT_TYPE

public static final ExceptionInfoCode INV_TPT_TYPE
        = new ExceptionInfoCode(M_INV_TPT_TYPE);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_TPT_TYPE. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_CMD_REQ_TYPE

public static final ExceptionInfoCode INV_CMD_REQ_TYPE
        = new ExceptionInfoCode(M_INV_CMD_REQ_TYPE);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_CMD_REQ_TYPE. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_CMD_RESP_TYPE

public static final ExceptionInfoCode INV_CMD_RESP_TYPE
        = new ExceptionInfoCode(M_INV_CMD_RESP_TYPE);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_CMD_RESP_TYPE. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_CMD_TYPE

public static final ExceptionInfoCode INV_CMD_TYPE
        = new ExceptionInfoCode(M_INV_CMD_TYPE);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_CMD_TYPE. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_DESC_TYPE

public static final ExceptionInfoCode INV_DESC_TYPE
        = new ExceptionInfoCode(M_INV_DESC_TYPE);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_DESC_TYPE. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_TERM_TYPE

public static final ExceptionInfoCode INV_TERM_TYPE
        = new ExceptionInfoCode(M_INV_TERM_TYPE);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_TERM_TYPE. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_EVT_BUFFER_CTRL

public static final ExceptionInfoCode INV_EVT_BUFFER_CTRL
        = new ExceptionInfoCode(M_INV_EVT_BUFFER_CTRL);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_EVT_BUFFER_CTRL. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  INV_MODEM_TYPE

public static final ExceptionInfoCode INV_MODEM_TYPE
        = new ExceptionInfoCode(M_INV_MODEM_TYPE);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_MODEM_TYPE. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_MUX_TYPE

public static final ExceptionInfoCode INV_MUX_TYPE
        = new ExceptionInfoCode(M_INV_MUX_TYPE);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_MUX_TYPE. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_SERVICE_STATE

public static final ExceptionInfoCode INV_SERVICE_STATE
        = new ExceptionInfoCode(M_INV_SERVICE_STATE);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_SERVICE_STATE. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_SIGNAL_PARAM_TYPE

public static final ExceptionInfoCode INV_SIGNAL_PARAM_TYPE
        = new ExceptionInfoCode(M_INV_SIGNAL_PARAM_TYPE);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_SIGNAL_PARAM_TYPE. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  INV_SIGNAL_TYPE

public static final ExceptionInfoCode INV_SIGNAL_TYPE
        = new ExceptionInfoCode(M_INV_SIGNAL_TYPE);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_SIGNAL_TYPE. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_STREAM_MODE

public static final ExceptionInfoCode INV_STREAM_MODE
        = new ExceptionInfoCode(M_INV_STREAM_MODE);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_STREAM_MODE. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_TOPOLOGY_DIR

public static final ExceptionInfoCode INV_TOPOLOGY_DIR
        = new ExceptionInfoCode(M_INV_TOPOLOGY_DIR);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_TOPOLOGY_DIR. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_PARAM_VAL_TYPE

public static final ExceptionInfoCode INV_PARAM_VAL_TYPE
        = new ExceptionInfoCode(M_INV_PARAM_VAL_TYPE);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_PARAM_VAL_TYPE. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  INV_PARAM_RELATION

public static final ExceptionInfoCode INV_PARAM_RELATION
        = new ExceptionInfoCode(M_INV_PARAM_RELATION);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_PARAM_RELATION. Since it is reference
          to static final object, it prevents further instantiation of
          the same object in the system.
     _________________________________________________________________

  INV_PKG_ITEM_TYPE

public static final ExceptionInfoCode INV_PKG_ITEM_TYPE
        = new ExceptionInfoCode(M_INV_PKG_ITEM_TYPE);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_PKG_ITEM_TYPE. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  INV_PRIORITY_VAL

public static final ExceptionInfoCode INV_PRIORITY_VAL
        = new ExceptionInfoCode(M_INV_PRIORITY_VAL);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_INV_PRIORITY_VAL. Since it is reference to
          static final object, it prevents further instantiation of the
          same object in the system.
     _________________________________________________________________

  DUPLICATE_DESC_IN_CMD

public static final ExceptionInfoCode DUPLICATE_DESC_IN_CMD
        = new ExceptionInfoCode(M_DUPLICATE_DESC_IN_CMD);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_DUPLICATE_DESC_IN_CMD. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  MISSING_DIGIT_WITH_QUALIFIER_DOT

public static final ExceptionInfoCode MISSING_DIGIT_WITH_QUALIFIER_DOT
        = new ExceptionInfoCode(M_MISSING_DIGIT_WITH_QUALIFIER_DOT);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_MISSING_DIGIT_WITH_QUALIFIER_DOT. Since it
          is reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  SYNTAX_ERR_IN_TERM_NAME

public static final ExceptionInfoCode SYNTAX_ERR_IN_TERM_NAME
        = new ExceptionInfoCode(M_SYNTAX_ERR_IN_TERM_NAME);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_SYNTAX_ERR_IN_TERM_NAME. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  SYNTAX_ERR_IN_DMAP_NAME

public static final ExceptionInfoCode SYNTAX_ERR_IN_DMAP_NAME
        = new ExceptionInfoCode(M_SYNTAX_ERR_IN_DMAP_NAME);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_SYNTAX_ERR_IN_DMAP_NAME. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

  SYNTAX_ERR_IN_SVC_CHNG_PROFILE

public static final ExceptionInfoCode SYNTAX_ERR_IN_SVC_CHNG_PROFILE
        = new ExceptionInfoCode(M_SYNTAX_ERR_IN_SVC_CHNG_PROFILE);

          Identifies a ExceptionInfoCode object that constructs the class
          with the constant M_SYNTAX_ERR_IN_SVC_CHNG_PROFILE. Since it is
          reference to static final object, it prevents further
          instantiation of the same object in the system.
     _________________________________________________________________

   Constructor Detail

  ExceptionInfoCode

private ExceptionInfoCode(int info_code)

          Constructs an abstract class that specifies the exception info
          code in the command.
     _________________________________________________________________

   Method Detail

  getExceptionInfoCode

public int getExceptionInfoCode()

          This method returns one of the static field constants defined
          in this class. This method is overriden by the derived class,
          which shall return an hard coded value depending on the class.


        Returns:
                Returns an integer value that identifies the exception
                info code to be one of one of the constant values defined
                for this class.
     _________________________________________________________________

  getObject

public static final ExceptionInfoCode getObject(int value)
                throws IllegalArgumentException()

          Returns reference of the ExceptionInfoCode object that
          identifies the termination type as value passed to this method.


                Parameters:
                          value - It is one of the possible values of the
                static constant that this class provides.

        Returns:
                Returns reference of the ExceptionInfoCode object.

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
