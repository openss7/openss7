....
[30] ServiceControls [[p &parm->cma_common.ca_servicecontrol]]
                     -- DEFAULT {} ,
                    OPTIONAL <D<0>>
                    <E< (
                    (parm->cma_common.ca_servicecontrol.svc_options != 0) ||
                    (parm->cma_common.ca_servicecontrol.svc_prio != SVC_PRIO_MED
) ||
                    (parm->cma_common.ca_servicecontrol.svc_timelimit != SVC_NOT
IMELIMIT) ||
                    (parm->cma_common.ca_servicecontrol.svc_scopeofreferral != S
VC_REFSCOPE_NONE)
                    ) >>,

....
