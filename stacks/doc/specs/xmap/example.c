#include <xom.h>          /* XOM object functions and macros */
#include <xmap.h>         /* XMAP interface */
#include <xmap_gsm.h>     /* GSM MAP */
#include <xmap_gsm_sm.h>  /* short message services */
#include <xmap_gsm_ls.h>  /* location services for common service error definitions */
#include <xmap_gsm_in.h>  /* identification and numbering services for common service error definitions */
#include <xmap_gsm_ch.h>  /* call handling services for common service error definitions */

OM_EXPORT(MAP_COMMON_PKG);
OM_EXPORT(MAP_GSM_PKG);
OM_EXPORT(MAP_GSM_SM_PKG);
OM_EXPORT(MAP_GSM_LS_PKG);
OM_EXPORT(MAP_GSM_IN_PKG);
OM_EXPORT(MAP_GSM_CH_PKG);

MAP_feature feature_list[] = {
    {MAP_COMMON_PKG, MAP_ACTIVATE, 0}
    , {MAP_GSM_PKG, MAP_ACTIVATE, 0}
    , {MAP_GSM_SM_PKG, MAP_ACTIVATE, 0}
    , {MAP_GSM_LS_PKG, MAP_ACTIVATE, 0}
    , {MAP_GSM_IN_PKG, MAP_ACTIVATE, 0}
    , {MAP_GSM_CH_PKG, MAP_ACTIVATE, 0}
    , {NULL, 0, 0}
};

OM_workspace workspace;
OM_private_object session;

MAP_status status;

workspace = map_initialize();

if (workspace == NULL) {
    perror();
    exit(1);
}

status = map_negotiate(feature_list, workspace);

if (status != MAP_SUCCESS)
    exit(1);

OM_object requestorAddress = {
    OM_OID_DESC(OM_CLASS, MAP_C_ADDRESS)
    , {MAP_SCCP_ADDRESS, MAP_S_SCCP_ADDRESS_STRING, OM_STRING("\x03\x00\x00\x05")}      /* 0-0-0 [5] no GTT */
    , OM_NULL_DESCRIPTOR
};

OM_object my_session = {
    OM_OID_DESC(OM_CLASS, MAP_C_SESSION)
    , {MAP_REQUESTOR_ADDRESS, OM_S_OBJECT, requestorAddress}
    , {MAP_ROLE, OM_S_INTEGER, (OM_integer) (MAP_T_INVOKER_ROLE)}
    , OM_NULL_DESCRIPTOR
};

status = map_bind(my_session, workspace, &session);

if (status != MAP_SUCCESS)
    exit(1);

OM_return_code rc;

OM_private_object context;

rc = om_create(MAP_C_GSM_CONTEXT, OM_FALSE, workspace, &context);

if (rc != OM_SUCCESS)
    exit(1);

OM_object responderAddress = {
    OM_OID_DESC(OM_CLASS, MAP_C_ADDRESS)
    , {MAP_SCCP_ADDRESS, MAP_S_SCCP_ADDRESS_STRING, OM_STRING("\x03\x01\x00\x06")}      /* 0-0-1 [6] no GTT */
    , OM_NULL_DESCRIPTOR
};

OM_object my_context = {
    OM_OID_DESC(OM_CLASS, MAP_C_GSM_CONTEXT)
    , {MAP_RESPONDER_ADDRESS, OM_S_OBJECT, responderAddress}
    , {MAP_ASYNCHRONOUS, OM_S_BOOLEAN, OM_TRUE}
    , OM_NULL_DESCRIPTOR
};

rc = om_put(context, OM_REPLACE_ALL, my_context, NULL, OM_ALL_VALUES, OM_ALL_VALUES);

if (rc != OM_SUCCESS)
    exit(1);

OM_object argument = {
    OM_OID_DESC(OM_CLASS, MAP_C_MO_FORWARD_SM_ARG)
    , {MAP_SM_RP_DA, OM_S_OBJECT,
       {OM_OID_DESC(OM_CLASS, MAP_C_SM_RP_DA)
        , {MAP_IMSI, MAP_S_TBCD_STRING, OM_STRING("\x21\x43\x65\x87")}
        , OM_NULL_DESCRIPTOR}
       }
    , {MAP_SM_RP_OA, OM_S_OBJECT,
       {OM_OID_DESC(OM_CLASS, MAP_C_SM_RP_OA)
        , {MAP_NO_SM_RP_OA, OM_S_NULL, NULL}
        , OM_NULL_DESCRIPTOR}
       }
    , {MAP_SM_RP_UI, OM_S_OCTET_STRING, (OM_string) {data_length, data_pointer}
       }
    , {MAP_IMSI, MAP_S_TBCD_STRING, OM_STRING("\x21\x43\x65\x87")}
    , OM_NULL_DESCRIPTOR
};

OM_private_object result;
OM_sint32 invoke_id = 0;

status = map_service_req(session, context, argument, &result, &invoke_id);

if (status != MAP_SUCCESS)
    exit(1);

MAP_waiting_sessions bound_session_list[] = { {session, OM_FALSE}
, {NULL,}
};

for (;;) {
    status = map_wait(bound_session_list, workspace, 0);

    if (status != MAP_SUCCESS)
        exit(1);

    if (bound_session_list[0].activated) {
        OM_sint mode;
        OM_sint primitive;
        OM_sint completion_flag;
        MAP_status operation_status;
        OM_private_object result;
        OM_sint32 received_invoke_id;

        status = map_receive(session, &mode, &primitive, &operation_status, &result, &received_invoke_id);
        if (status != MAP_SUCCESS)
            exit(1);

        if (received_invoke_id == invoke_id) {
            OM_boolean instance;

            instance = OM_FALSE;
            om_instance(result, MAP_C_SERVICE_RESULT, &instance);
            if (instance) {
                instance = OM_FALSE;
                om_instance(result, MAP_C_MO_FORWARD_SM_RES, &instance);
                if (instance) {
                }
            }
            om_instance(result, MAP_C_SERVICE_ERROR, &instance);
            if (instance) {
                OM_object error;
                OM_value_position count;

                rc = om_get(result, OM_NO_EXCLUSIONS, NULL, OM_FALSE, OM_ALL_VALUES, OM_ALL_VALUES, &error, &count);
                if (rc != OM_SUCCESS)
                    break;
                switch (error[1].value.integer) {
                case MAP_E_SYSTEM_FAILURE:
                    /* MAP_C_SYSTEM_FAILURE_PARAM in error[2].value.object */
                    break;
                case MAP_E_UNEXPECTED_DATA_VALUE:
                    /* MAP_C_UNEXPECTED_DATA_PARAM in error[2].value.object */
                    break;
                case MAP_E_FACILITY_NOT_SUPPORTED:
                    /* MAP_C_FACILITY_NOT_SUP_PARAM in error[2].value.object */
                    break;
                case MAP_E_SM_DELIVERY_FAILURE:
                    /* MAP_C_SM_DELIVERY_FAILURE in error[2].value.object */
                    if (count > 1) {
                        switch (error[2].value.object[1].value.enumeration) {
                        case MAP_T_MEMORY_CAPACITY_EXCEEDED:
                        case MAP_T_EQUIPMENT_PROTOCOL_ERROR:
                        case MAP_T_EQUIPMENT_NOT_SM_EQUIPPED:
                        case MAP_T_UNKNOWN_SERVICE_CENTRE:
                        case MAP_T_SC_CONGESTION:
                        case MAP_T_INVALID_SME_ADDRESS:
                        case MAP_T_SUBSCRIBER_NOT_SC_SUBSCRIBER:
                        default:
                            break;
                        }
                    }
                    break;
                default:
                    break;
                }
                om_delete(error);

            }
            om_instance(result, MAP_C_SERVICE_REJECT, &instance);
            if (instance) {
                OM_object reject;
                OM_value_position count;

                rc = om_get(result, OM_NO_EXCLUSIONS, NULL, OM_FALSE, OM_ALL_VALUES, OM_ALL_VALUES, &reject, &count);
                if (rc != OM_SUCCESS)
                    break;
                switch (reject[1].value.integer) {
                case MAP_E_DUPLICATED_INVOKE_ID:
                case MAP_E_NOT_SUPPORTED_SERVICE:
                case MAP_E_MISTYPED_PARAMETER:
                case MAP_E_RESOURCE_LIMITATION:
                case MAP_E_INITIATING_RELEASE:
                case MAP_E_UNEXPECTED_RESPONSE_FROM_PEER:
                case MAP_E_SERVICE_COMPLETION_FAILURE:
                case MAP_E_NO_RESPONSE_FROM_THE_PEER:
                case MAP_E_INVALID_RESPONSE_FROM_THE_PEER:
                default:
                    break;
                }
                om_delete(reject);
            }
        }
    }
}
