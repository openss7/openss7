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

package javax.jain.protocol.ss7.map;

import javax.jain.protocol.ss7.*;
import javax.jain.*;

/**
  * This parameter is used in confirmation primitives (from provider to
  * user) and in response primitives (from user to provider) to convey
  * result information. <p>
  *
  * <h4>Parameter components:</h4><ul>
  * <li>status - Status code with general result information, mandatory
  * component
  *
  * <li>errorCode - Error code is specified in case of error, mandatory
  * component <ul>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class Result extends SS7Parameter {
    /** Status code: undefined.  */
    public static final int STATUS_UNDEFINED = 0;
    /** Status code: OK.  */
    public static final int STATUS_OK = 1;
    /** Status code: user error.  */
    public static final int STATUS_USER_ERROR = 2;
    /** Status code: provider error.  */
    public static final int STATUS_PROVIDER_ERROR = 3;
    /** Error code: undefined.  */
    public static final int ERROR_CODE_UNDEFINED = 4;
    /** Error code: provider error, protocol related error, Duplicated
      * invoke ID.  */
    public static final int DUPLICATED_INVOKE_ID = 5;
    /** Error code: provider error, protocol related error, Provider
      * does not support the service.  */
    public static final int NOT_SUPPORTED_SERVICE = 6;
    /** Error code: provider error, protocol related error, Wrong
      * parameter.  */
    public static final int MISTYPED_PARAMETER = 7;
    /** Error code: provider error, protocol related error, Provider
      * resource limitation.  */
    public static final int PE_RESOURCE_LIMITATION = 8;
    /** Error code: provider error, protocol related error, Provider
      * initiating release.  */
    public static final int PE_INITIATING_RELEASE = 9;
    /** Error code: provider error, protocol related error, Unexpected
      * response from peer.  */
    public static final int UNEXPECTED_RESPONSE_FROM_PEER = 10;
    /** Error code: provider error, protocol related error, Service
      * compretion failure.  */
    public static final int SERVICE_COMPLETION_FAILURE = 11;
    /** Error code: provider error, protocol related error, No response
      * from peer.  */
    public static final int NO_RESPONSE_FROM_PEER = 12;
    /** Error code: provider error, protocol related error, Invalid
      * response received.  */
    public static final int INVALID_RESPONSE_RECEIVED = 13;
    /** Error code: user error, generic error, System failure.  */
    public static final int SYSTEM_FAILURE = 14;
    /** Error code: user error, generic error, Data missing.  */
    public static final int DATA_MISSING = 15;
    /** Error code: user error, generic error, Unexpected data value.  */
    public static final int UNEXPECTED_DATA_VALUE = 16;
    /** Error code: user error, generic error, User resource limitation.  */
    public static final int UE_RESOURCE_LIMITATION = 17;
    /** Error code: user error, generic error, User initiating release.  */
    public static final int UE_INITIATING_RELEASE = 18;
    /** Error code: user error, generic error, Facility not supported.  */
    public static final int FACILITY_NOT_SUPPORTED = 19;
    /** Error code: user error, generic error, Incompatible terminal.  */
    public static final int INCOMPATIBLE_TERMINAL = 20;
    /** Error code: user error, identification or numbering problem,
      * Unknown subscriber.  */
    public static final int UNKNOWN_SUBSCRIBER = 21;
    /** Error code: user error, identification or numbering problem,
      * Number changed.  */
    public static final int NUMBER_CHANGED = 22;
    /** Error code: user error, identification or numbering problem,
      * Unknown MSC.  */
    public static final int UNKNOWN_MSC = 23;
    /** Error code: user error, identification or numbering problem,
      * Unidentified subscriber.  */
    public static final int UNIDENTIFIED_SUBSCRIBER = 24;
    /** Error code: user error, identification or numbering problem,
      * Unallocated roaming number.  */
    public static final int UNALLOCATED_ROAMING_NUMBER = 25;
    /** Error code: user error, identification or numbering problem,
      * Unknown equipment.  */
    public static final int UNKNOWN_EQUIPMENT = 26;
    /** Error code: user error, identification or numbering problem,
      * Unknown location area.  */
    public static final int UNKNOWN_LOCATION_AREA = 27;
    /** Error code: user error, subscription problem, Roaming not
      * allowed.  */
    public static final int ROAMING_NOT_ALLOWED = 28;
    /** Error code: user error, subscription problem, Illegal
      * subscriber.  */
    public static final int ILLEGAL_SUBSCRIBER = 29;
    /** Error code: user error, subscription problem, Bearer service not
      * provisioned.  */
    public static final int BEARER_SERVICE_NOT_PROVISIONED = 30;
    /** Error code: user error, subscription problem, Teleservice not
      * provisioned.  */
    public static final int TELESERVICE_NOT_PROVISIONED = 31;
    /** Error code: user error, subscription problem, Illegal equipment.
      * */
    public static final int ILLEGAL_EQUIPMENT = 32;
    /** Error code: user error, supplementary services problem, Illegal
      * supplementary service operation.  */
    public static final int ILLEGAL_SS_OPERATION = 33;
    /** Error code: user error, supplementary services problem,
      * Supplementary service error status.  */
    public static final int SS_ERROR_STATUS = 34;
    /** Error code: user error, supplementary services problem,
      * Supplementary service not available.  */
    public static final int SS_NOT_AVAILABLE = 35;
    /** Error code: user error, supplementary services problem,
      * Supplementary service subscription violation.  */
    public static final int SS_SUBSCRIPTION_VIOLATION = 36;
    /** Error code: user error, supplementary services problem,
      * Supplementary service imcompatibility.  */
    public static final int SS_INCOMPATIBILITY = 37;
    /** Error code: user error, supplementary services problem, Negative
      * password check.  */
    public static final int NEGATIVE_PASSWORD_CHECK = 38;
    /** Error code: user error, supplementary services problem, Password
      * registration failure.  */
    public static final int PASSWORD_REGISTRATION_FAILURE = 39;
    /** Error code: user error, supplementary services problem, Number
      * of password attemts.  */
    public static final int NUMBER_OF_PASSWORD_ATTEMPTS = 40;
    /** Error code: user error, supplementary services problem, USSD
      * busy.  */
    public static final int USSD_BUSY = 41;
    /** Error code: user error, supplementary services problem, Unknown
      * alphabet.  */
    public static final int UNKNOWN_ALPHABET = 42;
    /** Error code: user error, supplementary services problem, Short
      * term denial of service.  */
    public static final int SHORT_TERM_DENIAL = 43;
    /** Error code: user error, supplementary services problem, Long
      * term denial of service.  */
    public static final int LONG_TERM_DENIAL = 44;
    /** Error code: user error, Short Message delivery failure, Memory
      * capacity exceeded.  */
    public static final int MEMORY_CAPACITY_EXCEEDED = 45;
    /** Error code: user error, Short Message delivery failure, MS
      * protocol error.  */
    public static final int MS_PROTOCOL_ERROR = 46;
    /** Error code: user error, Short Message delivery failure, MS not
      * equipped to handle Short Message.  */
    public static final int MS_NOT_EQUIPPED = 47;
    /** Error code: user error, Short Message delivery failure, Unknown
      * Service Centre.  */
    public static final int UNKNOWN_SERVICE_CENTRE = 48;
    /** Error code: user error, Short Message delivery failure, Service
      * Centre congestion.  */
    public static final int SC_CONGESTION = 49;
    /** Error code: user error, Short Message delivery failure, Invalid
      * Short Message Entity address.  */
    public static final int INVALID_SME_ADDRESS = 50;
    /** Error code: user error, Short Message delivery failure,
      * Subscriber is not a Service Center subscriber.  */
    public static final int SUBSCRIBER_IS_NOT_AN_SC_SUBSCRIBER = 51;
    /** Error code: user error, Short Message delivery failure, Message
      * waiting list in HLR is full.  */
    public static final int MESSAGE_WAITING_LIST_FULL = 52;
    /** Error code: user error, Short Message delivery failure,
      * Subscriber busy for Mobile Terminated SMS.  */
    public static final int SUBSCRIBER_BUSY_FOR_MT_SMS = 53;
    /** Error code: user error, Short Message delivery failure, Absent
      * subscriber.  */
    public static final int ABSENT_SUBSCRIBER_SM = 54;
    /** Error code: user error, location services problem, Unauthorized
      * requesting network.  */
    public static final int UNAUTHORIZED_REQUESTING_NETWORK = 55;
    /** Error code: user error, location services problem, Unauthorized
      * LCS client.  */
    public static final int UNAUTHORIZED_LCS_CLIENT = 56;
    /** Error code: user error, location services problem, Unauthorized
      * privacy class.  */
    public static final int UNAUTHORZIED_PRIVACY_CLASS = 57;
    /** Error code: user error, location services problem, Unauthorized
      * call, unrelated external client.  */
    public static final int UNAUTHORIZED_CALL_UNRELATED_EXTERNAL_CLIENT = 58;
    /** Error code: user error, location services problem, Unauthorized
      * call, related external client.  */
    public static final int UNAUTHORIZED_CALL_RELATED_EXTERNAL_CLIENT = 59;
    /** Error code: user error, location services problem, Privacy
      * override not applicable.  */
    public static final int PRIVACY_OVERRIDE_NOT_APPLICABLE = 60;
    /** Error code: user error, position method failure, Congestion.  */
    public static final int CONGESTION = 61;
    /** Error code: user error, position method failure, Insufficient
      * resources.  */
    public static final int INSUFFICIENT_RESOURCES = 62;
    /** Error code: user error, position method failure, Insufficient
      * measurement data.  */
    public static final int INSUFFICIENT_MEASUREMENT_DATA = 63;
    /** Error code: user error, position method failure, Inconsistent
      * measurement data.  */
    public static final int INCONSISTENT_MEASUREMENT_DATA = 64;
    /** Error code: user error, position method failure, Location
      * procedure not completed.  */
    public static final int LOCATION_PROCEDURE_NOT_COMPLETED = 65;
    /** Error code: user error, position method failure, Location
      * procedure not supported by target MS.  */
    public static final int LOCATION_PROCEDURE_NOT_SUPPORTED_BY_TARGET_MS = 66;
    /** Error code: user error, position method failure, Requested
      * quality of service not attainable.  */
    public static final int QOS_NOT_ATTAINABLE = 67;
    /** Error code: user error, position method failure, Unknown or
      * unreachable LCS client.  */
    public static final int UNKNOWN_OR_UNREACHABLE_LCS_CLIENT = 68;
    /** Error code: user error, errors for Any Time Interrogation,
      * System failure.  */
    public static final int ATI_SYSTEM_FAILURE = 69;
    /** Error code: user error, errors for Any Time Interrogation, ATI
      * not allowed.  */
    public static final int ATI_NOT_ALLOWED = 70;
    /** Error code: user error, errors for Any Time Interrogation, Data
      * missing.  */
    public static final int ATI_DATA_MISSING = 71;
    /** Error code: user error, errors for Any Time Interrogation,
      * Unexpected data value.  */
    public static final int ATI_UNEXPECTED_DATA_VALUE = 72;
    /** Error code: user error, errors for Any Time Interrogation,
      * Unknown subscriber.  */
    public static final int ATI_UNKNOWN_SUBSCRIBER = 73;
    /**
      * Constructor setting status.
      * @param status
      * Status code with general result information.
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public Result(int status)
        throws SS7InvalidParamException {
    }
    /**
      * Constructor setting status and errorCode.
      * @param status
      * Status code with general result information.
      * @param errorCode
      * Error code is specified in case of error.
      * @exception SS7InvalidParamException
      * Thrown if parameter(s) are invalid / out of range.
      */
    public Result(int status, int errorCode)
        throws SS7InvalidParamException {
    }
    /**
      * Empty constructor; needed for serializable objects and beans.
      */
    public Result()
        throws SS7InvalidParamException {
    }
    /**
      * Set the status.
      * @param status
      * Status code with general result information.
      */
    public void setStatus(int status)
        throws SS7InvalidParamException {
    }
    /**
      * Get the status.
      * @return
      * Status code with general result information.
      */
    public int getStatus() {
        return 0;
    }
    /**
      * Set the error code.
      * @param errorCode
      * Error code is specified in case of error.
      */
    public void setErrorCode(int errorCode)
        throws SS7InvalidParamException {
    }
    /**
      * Get the error code.
      * @return
      * Error code is specified in case of error.
      */
    public int getErrorCode() {
        return 0;
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
