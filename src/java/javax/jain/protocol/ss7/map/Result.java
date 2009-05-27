
package javax.jain.protocol.ss7.map;

import javax.jain.protocol.ss7.*;
import javax.jain.*;

public class Result extends SS7Parameter {
    public static final int STATUS_UNDEFINED = 0;
    public static final int STATUS_OK = 1;
    public static final int STATUS_USER_ERROR = 2;
    public static final int STATUS_PROVIDER_ERROR = 3;
    public static final int ERROR_CODE_UNDEFINED = 4;
    public static final int DUPLICATED_INVOKE_ID = 5;
    public static final int NOT_SUPPORTED_SERVICE = 6;
    public static final int MISTYPED_PARAMETER = 7;
    public static final int PE_RESOURCE_LIMITATION = 8;
    public static final int PE_INITIATING_RELEASE = 9;
    public static final int UNEXPECTED_RESPONSE_FROM_PEER = 10;
    public static final int SERVICE_COMPLETION_FAILURE = 11;
    public static final int NO_RESPONSE_FROM_PEER = 12;
    public static final int INVALID_RESPONSE_RECEIVED = 13;
    public static final int SYSTEM_FAILURE = 14;
    public static final int DATA_MISSING = 15;
    public static final int UNEXPECTED_DATA_VALUE = 16;
    public static final int UE_RESOURCE_LIMITATION = 17;
    public static final int UE_INITIATING_RELEASE = 18;
    public static final int FACILITY_NOT_SUPPORTED = 19;
    public static final int INCOMPATIBLE_TERMINAL = 20;
    public static final int UNKNOWN_SUBSCRIBER = 21;
    public static final int NUMBER_CHANGED = 22;
    public static final int UNKNOWN_MSC = 23;
    public static final int UNIDENTIFIED_SUBSCRIBER = 24;
    public static final int UNALLOCATED_ROAMING_NUMBER = 25;
    public static final int UNKNOWN_EQUIPMENT = 26;
    public static final int UNKNOWN_LOCATION_AREA = 27;
    public static final int ROAMING_NOT_ALLOWED = 28;
    public static final int ILLEGAL_SUBSCRIBER = 29;
    public static final int BEARER_SERVICE_NOT_PROVISIONED = 30;
    public static final int TELESERVICE_NOT_PROVISIONED = 31;
    public static final int ILLEGAL_EQUIPMENT = 32;
    public static final int ILLEGAL_SS_OPERATION = 33;
    public static final int SS_ERROR_STATUS = 34;
    public static final int SS_NOT_AVAILABLE = 35;
    public static final int SS_SUBSCRIPTION_VIOLATION = 36;
    public static final int SS_INCOMPATIBILITY = 37;
    public static final int NEGATIVE_PASSWORD_CHECK = 38;
    public static final int PASSWORD_REGISTRATION_FAILURE = 39;
    public static final int NUMBER_OF_PASSWORD_ATTEMPTS = 40;
    public static final int USSD_BUSY = 41;
    public static final int UNKNOWN_ALPHABET = 42;
    public static final int SHORT_TERM_DENIAL = 43;
    public static final int LONG_TERM_DENIAL = 44;
    public static final int MEMORY_CAPACITY_EXCEEDED = 45;
    public static final int MS_PROTOCOL_ERROR = 46;
    public static final int MS_NOT_EQUIPPED = 47;
    public static final int UNKNOWN_SERVICE_CENTRE = 48;
    public static final int SC_CONGESTION = 49;
    public static final int INVALID_SME_ADDRESS = 50;
    public static final int SUBSCRIBER_IS_NOT_AN_SC_SUBSCRIBER = 51;
    public static final int MESSAGE_WAITING_LIST_FULL = 52;
    public static final int SUBSCRIBER_BUSY_FOR_MT_SMS = 53;
    public static final int ABSENT_SUBSCRIBER_SM = 54;
    public static final int UNAUTHORIZED_REQUESTING_NETWORK = 55;
    public static final int UNAUTHORIZED_LCS_CLIENT = 56;
    public static final int UNAUTHORZIED_PRIVACY_CLASS = 57;
    public static final int UNAUTHORIZED_CALL_UNRELATED_EXTERNAL_CLIENT = 58;
    public static final int UNAUTHORIZED_CALL_RELATED_EXTERNAL_CLIENT = 59;
    public static final int PRIVACY_OVERRIDE_NOT_APPLICABLE = 60;
    public static final int CONGESTION = 61;
    public static final int INSUFFICIENT_RESOURCES = 62;
    public static final int INSUFFICIENT_MEASUREMENT_DATA = 63;
    public static final int INCONSISTENT_MEASUREMENT_DATA = 64;
    public static final int LOCATION_PROCEDURE_NOT_COMPLETED = 65;
    public static final int LOCATION_PROCEDURE_NOT_SUPPORTED_BY_TARGET_MS = 66;
    public static final int QOS_NOT_ATTAINABLE = 67;
    public static final int UNKNOWN_OR_UNREACHABLE_LCS_CLIENT = 68;
    public static final int ATI_SYSTEM_FAILURE = 69;
    public static final int ATI_NOT_ALLOWED = 70;
    public static final int ATI_DATA_MISSING = 71;
    public static final int ATI_UNEXPECTED_DATA_VALUE = 72;
    public static final int ATI_UNKNOWN_SUBSCRIBER = 73;
    public Result(int status)
        throws SS7InvalidParamException {
    }
    public Result(int status, int errorCode)
        throws SS7InvalidParamException {
    }
    public Result()
        throws SS7InvalidParamException {
    }
    public void setStatus(int status)
        throws SS7InvalidParamException {
    }
    public int getStatus() {
        return 0;
    }
    public void setErrorCode(int errorCode)
        throws SS7InvalidParamException {
    }
    public int getErrorCode() {
        return 0;
    }
}


// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
