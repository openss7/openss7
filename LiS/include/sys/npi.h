
#ident "@(#) npi.h 1.12 7/24/01"


/*
 * npi.h header for the Network Provider Interface (OSI Conforming)
 */

#ifndef SYS_NPI_H
#define SYS_NPI_H				    /* mark file as included */

#define N_CURRENT_VERSION   0x02    /* current version of NPI */
#define N_VERSION_2	    0x02    /* version of npi, December 16, 1991 */
				    /* NPI Spec 2.0.0, August 17, 1992   */


/*
 * Primitives that are initiated by the network user.
 */

#define N_CONN_REQ	0	/* NC request */
#define N_CONN_RES	1	/* Accept previous connection indication */
#define N_DISCON_REQ	2	/* NC disconnection request */
#define N_DATA_REQ	3	/* Connection-Mode data transfer request */
#define N_EXDATA_REQ	4	/* Expedited data request */
#define N_INFO_REQ	5	/* Information Request */
#define N_BIND_REQ	6	/* Bind a NS user to network address */
#define N_UNBIND_REQ	7	/* Unbind NS user from network address */
#define N_UNITDATA_REQ	8	/* Connection-less data send request */
#define N_OPTMGMT_REQ	9	/* Options Management request */

/*
 * Primitives that are initiated by the network provider.
 */

#define N_CONN_IND	11	/* Incoming connection indication */
#define N_CONN_CON	12	/* Connection established */
#define N_DISCON_IND	13	/* NC disconnected */
#define N_DATA_IND	14	/* Incoming connection-mode data indication */
#define N_EXDATA_IND	15	/* Incoming expedited data indication */
#define N_INFO_ACK	16	/* Information Acknowledgement */
#define N_BIND_ACK	17	/* NS User bound to network address */
#define N_ERROR_ACK	18	/* Error Acknowledgement */
#define N_OK_ACK	19	/* Success Acknowledgement */
#define N_UNITDATA_IND	20	/* Connection-less data receive indication */
#define N_UDERROR_IND	21	/* UNITDATA Error Indication */

/*
 * Additional NPI Primitivies
 */

#define N_DATACK_REQ	23	/* Data acknowledgement request */
#define N_DATACK_IND	24	/* Data acknowledgement indication */
#define N_RESET_REQ	25	/* NC reset request */
#define N_RESET_IND	26	/* Incoming NC reset request indication */
#define N_RESET_RES	27	/* Reset processing accepted */
#define N_RESET_CON	28	/* Reset processing complete */

/*
 * The following are the events that drive the state machine
 */

/*
 * Initialization events
 */

#define NE_BIND_REQ	0	/* bind request */
#define NE_UNBIND_REQ	1	/* unbind request */
#define NE_OPTMGMT_REQ	2	/* manage options request */
#define NE_BIND_ACK	3	/* bind acknowledgement */
#define NE_ERROR_ACK	5	/* error acknowledgement */
#define NE_OK_ACK1	6	/* ok ack, outcnt == 0 */
#define NE_OK_ACK2	7	/* ok ack, outcnt == 1, q == rq */
#define NE_OK_ACK3	8	/* ok ack, outcnt == 1, q! == rq */
#define NE_OK_ACK4	9	/* ok ack, outcnt > 1 */

/*
 * Connection-Mode events
 */
#define NE_CONN_REQ	10	/* connect request */
#define NE_CONN_RES	11	/* connect response */
#define NE_DISCON_REQ	12	/* disconnect request */
#define NE_DATA_REQ	13	/* data request */
#define NE_EXDATA_REQ	14	/* expedited data request */
#define NE_CONN_IND	16	/* connect indication */
#define NE_CONN_CON	17	/* connect confirm */
#define NE_DATA_IND	18	/* data indication */
#define NE_EXDATA_IND	19	/* expedited data indication */
#define NE_DISCON_IND1	21	/* disconnect indication, outcnt == 0 */
#define NE_DISCON_IND2	22	/* disconnect indication, outcnt == 1 */
#define NE_DISCON_IND3	23	/* disconnect indication, outcnt > 1 */
#define NE_PASS_CON	24	/* pass connection */
#define NE_RESET_REQ	28	/* reset request */
#define NE_RESET_RES	29	/* reset response */
#define NE_DATACK_REQ	30	/* data acknowledgement request */
#define NE_DATACK_IND	31	/* data acknowledgement indication */
#define NE_RESET_IND	32	/* reset indication */
#define NE_RESET_CON	33	/* reset confirm */

/*
 * Connection-less events
 */
#define NE_UNITDATA_REQ 25	/* unitdata request */
#define NE_UNITDATA_IND 26	/* unitdata indication */
#define NE_UDERROR_IND	27	/* unitdata error indication */

#define NE_NOEVENTS	36	/* no events */

/*
 * NPI interface states
 */
#define NS_UNBND	0	/* NS user not bound to network address */
#define NS_WACK_BREQ	1	/* Awaiting acknowledgement of N_BIND_REQ */
#define NS_WACK_UREQ	2	/* Pending acknowledgement for N_UNBIND_REQ */
#define NS_IDLE		3	/* Idle, no connection */
#define NS_WACK_OPTREQ	4	/* Pending acknowledgement of N_OPTMGMT_REQ */
#define NS_WACK_RRES	5	/* Pending acknowledgement of N_RESET_RES */
#define NS_WCON_CREQ	6	/* Pending confirmation of N_CONN_REQ */
#define NS_WRES_CIND	7	/* Pending response of N_CONN_REQ */
#define NS_WACK_CRES	8	/* Pending acknowledgement of N_CONN_RES */
#define NS_DATA_XFER	9	/* Connection-mode data transfer */
#define NS_WCON_RREQ	10	/* Pending confirmation of N_RESET_REQ */
#define NS_WRES_RIND	11	/* Pending response of N_RESET_IND */
#define NS_WACK_DREQ6	12	/* Waiting ack of N_DISCON_REQ */
#define NS_WACK_DREQ7	13	/* Waiting ack of N_DISCON_REQ */
#define NS_WACK_DREQ9	14	/* Waiting ack of N_DISCON_REQ */
#define NS_WACK_DREQ10	15	/* Waiting ack of N_DISCON_REQ */
#define NS_WACK_DREQ11	16	/* Waiting ack of N_DISCON_REQ */

#define NS_NOSTATES	18	/* No states */

/*
 * N_ERROR_ACK error return code values
 */
#define NBADADDR	1  /* Incorrect address format/illegal address information */
#define NBADOPT		2  /* Options in incorrect format or contain illegal information */
#define NACCESS		3  /* User did not have proper permissions */
#define NNOADDR		5  /* NS Provider could not allocate address */
#define NOUTSTATE	6  /* Primitive was issues in wrong sequence */
#define NBADSEQ		7  /* Sequence number in primitive was incorrect/illegal */
#define NSYSERR		8  /* UNIX system error occurred */
#define NBADDATA	10 /* User data spec. outside range supported by NS provider */
#define NBADFLAG	16 /* Flags specified in primitive were */
			   /* illegal/incorrect */
#define NNOTSUPPORT	18 /* Primitive type not supported by the NS provider */
#define NBOUND		19 /* Illegal second attempt to bind listener or */
			   /* default listener */
#define NBADQOSPARAM	20 /* QOS values specified are outside the range */
			   /* supported by the NS provider */
#define NBADQOSTYPE	21 /* QOS structure type specified is not supported */
			   /* by the NS provider */
#define NBADTOKEN	22 /* Token used is not associated with an open stream */
#define NNOPROTOID	23 /* Protocol id could not be allocated */


/*
 * N_UDERROR_IND reason codes
 */
#define N_UD_UNDEFINED		10	/* no reason specified */
#define N_UD_TD_EXCEEDED	11	/* Transit delay exceeded */
#define N_UD_CONGESTION		12	/* NS Provider congestion */
#define N_UD_QOS_UNAVAIL	13	/* Requested QOS/service characteristic unavailable */
#define N_UD_LIFE_EXCEEDED	14	/* NSDU Lifetime exceeded */
#define N_UD_ROUTE_UNAVAIL	15	/* Suitable route unavailable */
#define N_UD_SEG_REQUIRED	16	/* Segmentation reqd where none permitted */

/*
 * NPI Originator for Resets and Disconnects
 */
#define N_PROVIDER	0x0100	/* provider originated reset/disconnect */
#define N_USER		0x0101	/* user originated reset/disconnect */
#define N_UNDEFINED	0x0102	/* reset/disconnect originator undefined */

/*
 * NPI Disconnect & Reset reasons when the originator is the N_UNDEFINED
 */
#define N_REASON_UNDEFINED	0x0200

/*
 * NPI Disconnect reasons when the originator is the N_PROVIDER
 */
#define N_DISC_P		0x0300	/* Disconnection-permanent condition */
#define N_DISC_T		0x0301	/* Disconnection-transient condition */
#define N_REJ_NSAP_UNKNOWN	0x0302	/* Connection rejection-NSAP address */
					/* unknown (permanent condition) */
#define N_REJ_NSAP_UNREACH_P	0x0303	/* Connection rejection-NSAP */
					/* unreachable (permanent condition) */
#define N_REJ_NSAP_UNREACH_T	0x0304	/* Connection rejection-NSAP */
					/* unreachable (transient condition) */

/*
 * NPI Disconnect reasons when the originator is the N_USER
 */
#define N_DISC_NORMAL		0x0400	/* Disconnection-normal condition */
#define N_DISC_ABNORMAL		0x0401	/* Disconnection-abnormal condition */
#define N_REJ_P			0x0402	/* Connection rejection-permanent */
					/* condition */
#define N_REJ_T			0x0403	/* Connection rejection-transient */
					/* condition */
#define N_REJ_INCOMPAT_INFO	0x0406	/* Connection rejection-incompatible */
					/* information in NS-user-data */

/*
 * NPI Disconnect reasons when the originator is the N_USER or N_PROVIDER
 */
#define N_REJ_QOS_UNAVAIL_P	0x0305	/* Connection rejection-QOS */
					/* unavailable (permanent condition) */
#define N_REJ_QOS_UNAVAIL_T	0x0306	/* Connection rejection-QOS */
					/* unavailable (transient condition) */
#define N_REJ_UNSPECIFIED	0x0307	/* Connection rejection-reason */
					/* unspecified */

/*
 * NPI Reset reasons when originator is N_PROVIDER
 */
#define N_CONGESTION		0x0500	/* Reset due to congestion */
#define N_RESET_UNSPECIFIED	0x0501	/* Reset-reason "unspecified" */

/*
 * NPI Reset reasons when originator is N_USER
 */
#define N_USER_RESYNC		0x0600	/* Reset due to user resynchronization */

/*
 * CONN_flags definition; (used in N_conn_req, N_conn_ind, N_conn_res,
 * and N_conn_con primitives)
 *
 * Flags to indicate support of network provider options;
 * (used with the OPTIONS_flags field of N_info_ack primitive)
 */
#define REC_CONF_OPT	0x00000001L	/* Receipt Confirmation Selection and Support */
#define EX_DATA_OPT	0x00000002L	/* Expedited Data Selection and Support */

/* This flag is used with the OPTIONS_flags field of N_info_ack as well as */
/* the OPTMGMT_flags field of the N_optmgmt_req primitive */
#define DEFAULT_RC_SEL	0x00000004L	/* Indicates if default receipt confirmation is selected */
					/* npi.h in 2.0.0 spec has this as 3,
					 * but the text says 4 and an npi.h
					 * from AT&T (earlier version) also
					 * had the value at 4.
					 */

/*
 * BIND_flags; (used with N_bind_req primitive)
 */

#define DEFAULT_LISTENER    0x00000001L /* indicates if this stream is the default listener */
#define TOKEN_REQUEST	    0x00000002L /* indicates if "token" should be assigned to the stream */
#define DEFAULT_DEST	    0x00000004L /* indicates if default dest. stream */

/*
 * QOS Parameter Definitions
 */

/*
 * Throughput
 *
 * This parameter is specified for both directions.
 */
typedef struct
{
    long    thru_targ_value;	    /* target throughput values */
    long    thru_min_value;	    /* minimum acceptable throughput value */
} thru_values_t;

/*
 * Transit Delay
 */
typedef struct
{
    long    td_targ_value;	    /* target transit delay */
    long    td_max_value;	    /* maximum acceptable transit delay */
} td_values_t;

/*
 * Protection Values
 */
typedef struct
{
    long    protect_targ_value;	    /* target protection value */
    long    protect_min_value;	    /* minimum or available protection */
} protection_values_t;

/*
 * Priority Values
 */
typedef struct
{
    long    priority_targ_value;    /* target priority */
    long    priority_min_value;	    /* minimum acceptable priority */
} priority_values_t;

/*
 * Types of protection specifications
 */
#define N_NO_PROT		0x00000000L	/* no protection */
#define N_PASSIVE_PROT		0x00000001L	/* protection against passive monitoring */
#define N_ACTIVE_PROT		0x00000002L	/* protection against active monitoring */
#define N_ACTIVE_PASSIVE_PROT	0x00000003L	/* protection against active and passive monitoring */

/*
 * Cost Selection
 */
#define N_LEAST_EXPENSIVE	0x00000000L	/* choose least expensive means */

/*
 * QOS STRUCTURE TYPES AND DEFINED VALUES
 */
#define N_QOS_CO_RANGE1		0x0101
#define N_QOS_CO_SEL1		0x0102
#define N_QOS_CL_RANGE1		0x0103
#define N_QOS_CL_SEL1		0x0104
#define N_QOS_CO_OPT_RANGE1	0x0105
#define N_QOS_CO_OPT_SEL1	0x0106

/*
 * When a NS user/provider cannot determine the value of a QOS field,
 * it should return a value of QOS_UNKNOWN.
 */
#define QOS_UNKNOWN			-1

#if !defined(_LINUX_TYPES_H) && !defined(_INTTYPES_H)
# if defined(__sparcv9)			/* 64-bit SPARC compile */
    typedef unsigned int	np_ulong ;
    typedef unsigned short	np_ushort ;
# else
    typedef unsigned long	np_ulong ;
    typedef unsigned short	np_ushort ;
# endif
#else
    typedef uint32_t		np_ulong ;
    typedef uint16_t		np_ushort ;
#endif

/*
 * QOS range for CONS. (Used with N_CONN_REQ and N_CONN_IND.)
 */
typedef struct
{
    np_ulong		n_qos_type;		/* always N_QOS_CO_RANGE */
    thru_values_t	src_throughput_range;	/* source throughput range */
    thru_values_t	dest_throughput_range;	/* destination throughput range */
    td_values_t		transit_delay_range;	/* transit delay range */
    protection_values_t protection_range;	/* protection range */
    priority_values_t	priority_range;		/* priority range */
} N_qos_co_range_t;

/*
 * QOS selected for CONS. (Used with N_CONN_RES and N_CONN_CON.)
 */
typedef struct
{
    np_ulong	n_qos_type;		/* always N_QOS_CO_SEL */
    long	src_throughput_sel;	/* source throughput selected */
    long	dest_throughput_sel;	/* destination throughput selected */
    long	transit_delay_sel;	/* transit delay selected */
    long	protection_sel;		/* NC protection selected */
    long	priority_sel;		/* NC priority selected */
} N_qos_co_sel_t;

/*
 * QOS range for CLNS options management. (Used with N_INFO_ACK.)
 */
typedef struct
{
    np_ulong		n_qos_type;	     /* always N_QOS_CL_RANGE */
    td_values_t		transit_delay_max;   /* maximum transit delay */
    np_ulong		residual_error_rate; /* residual error rate */
    protection_values_t protection_range;    /* protection range */
    priority_values_t	priority_range;	     /* priority range */
    long		max_accept_cost;     /* maximum acceptable cost */
} N_qos_cl_range_t;

/*
 * QOS selection for CLNS options management.
 * (Used with N_OPTMGMT_REQ and N_INFO_ACK.)
 */
typedef struct
{
    np_ulong	n_qos_type;		/* always N_QOS_CL_sel */
    long	transit_delay_max;	/* maximum transit delay */
    np_ulong	residual_error_rate;	/* residual error rate */
    long	protection_sel;		/* protection selected */
    long	priority_sel;		/* priority selected */
    long	max_accept_cost;	/* maximum acceptable cost */
} N_qos_cl_sel_t;

/*
 * QOS range for CONS options management. (Used with N_OPTMGMT_REQ.)
 */
typedef struct
{
    np_ulong		n_qos_type;		/* always N_QOS_CO_OPT_RANGE */
    thru_values_t	src_throughput;		/* source throughput values */
    thru_values_t	dest_throughput;	/* dest throughput values */
    td_values_t		transit_delay_t;	/* transit delay values */
    long		nc_estab_delay;		/* NC establishment delay */
    np_ulong		nc_estab_fail_prob;	/* NC estab failure probability */
    np_ulong		residual_error_rate;	/* residual error rate */
    np_ulong		xfer_fail_prob;		/* transfer failure probability */
    np_ulong		nc_resilience;		/* NC resilience */
    long		nc_rel_delay;		/* NC release delay */
    np_ulong		nc_rel_fail_prob;	/* NC release failure probability */
    protection_values_t protection_range;	/* protection range */
    priority_values_t	priority_range;		/* priority range */
    long		max_accept_cost;	/* maximum acceptable cost */
} N_qos_co_opt_range_t;

/*
 * QOS values selected for CONS options management.
 * (Used with N_OPTMGMT_REQ and N_INFO_ACK.)
 */
typedef struct
{
    np_ulong	    n_qos_type;		 /* always N_QOS_CO_OPT_SEL */
    thru_values_t   src_throughput;	 /* source throughput values */
    thru_values_t   dest_throughput;	 /* dest throughput values */
    td_values_t	    transit_delay_t;	 /* transit delay values */
    long	    nc_estab_delay;	 /* NC establishment delay */
    np_ulong	    nc_estab_fail_prob;	 /* NC estab failure probability */
    np_ulong	    residual_error_rate; /* residual error rate */
    np_ulong	    xfer_fail_prob;	 /* transfer failure probability */
    np_ulong	    nc_resilience;	 /* NC resilience */
    long	    nc_rel_delay;	 /* NC release delay */
    np_ulong	    nc_rel_fail_prob;	 /* NC release failure probability */
    long	    protection_sel;	 /* protection selected */
    long	    priority_sel;	 /* priority selected */
    long	    max_accept_cost;	 /* maximum acceptable cost */
} N_qos_co_opt_sel_t;

/*
 * NPI Primitive Definitions
 */

/*
 * Local management service primitives
 */

/*
 * Information request
 */
typedef struct
{
    np_ulong	PRIM_type;  /* always N_INFO_REQ */
} N_info_req_t;

/*
 * Information acknowledgement
 */
typedef struct
{
    np_ulong	PRIM_type;	  /* always N_INFO_ACK */
    np_ulong	NSDU_size;	  /* maximum NSDU size */
    np_ulong	ENSDU_size;	  /* maximum ENSDU size */
    np_ulong	CDATA_size;	  /* connect data size */
    np_ulong	DDATA_size;	  /* discon data size */
    np_ulong	ADDR_size;	  /* address size */
    np_ulong	ADDR_length;	  /* address length */
    np_ulong	ADDR_offset;	  /* address offset */
    np_ulong	QOS_length;	  /* QOS values length */
    np_ulong	QOS_offset;	  /* QOS values offset */
    np_ulong	QOS_range_length; /* length of QOS values' range */
    np_ulong	QOS_range_offset; /* offset of QOS values' range */
    np_ulong	OPTIONS_flags;	  /* bit masking for options supported */
    np_ulong	NIDU_size;	  /* network i/f data unit size */
    long	SERV_type;	  /* service type */
    np_ulong	CURRENT_state;	  /* current state */
    np_ulong	PROVIDER_type;	  /* type of NS provider */
    np_ulong	NODU_size;	  /* optimal NSDU size */
    np_ulong	PROTOID_length;	  /* length of bound protocol ids */
    np_ulong	PROTOID_offset;	  /* offset of bound protocol ids */
    np_ulong	NPI_version;	  /* version # of npi that is supported */
} N_info_ack_t;

/*
 * Service types supported by NS provider
 */
#define N_CONS 1 /* Connection-mode network service supported */
#define N_CLNS 2 /* Connection-less network service supported */

/*
 * Valid provider types
 */
#define N_SNICFP 1
#define N_SUBNET 2

/*
 * Bind request
 */
typedef struct
{
    np_ulong   PRIM_type;      /* always N_BIND_REQ */
    np_ulong   ADDR_length;    /* length of address */
    np_ulong   ADDR_offset;    /* offset of address */
    np_ulong   CONIND_number;  /* requested # of connect- */
			       /* indications to be queued */
    np_ulong   BIND_flags;     /* bind flags */
    np_ulong   PROTOID_length; /* length of bound protocol ids */
    np_ulong   PROTOID_offset; /* offset of bound protocol ids */
} N_bind_req_t;

/*
 * Bind acknowledgement
 */
typedef struct
{
    np_ulong   PRIM_type;      /* always N_BIND_ACK */
    np_ulong   ADDR_length;    /* address length */
    np_ulong   ADDR_offset;    /* offset of address */
    np_ulong   CONIND_number;  /* connection indications */
    np_ulong   TOKEN_value;    /* value of "token" assigned to stream */
    np_ulong   PROTOID_length; /* length of bound protocol ids */
    np_ulong   PROTOID_offset; /* offset of bound protocol ids */
} N_bind_ack_t;

/*
 * Unbind request
 */
typedef struct
{
    np_ulong PRIM_type;	    /* always N_UNBIND_REQ */
} N_unbind_req_t;

/*
 * Options management request
 */
typedef struct
{
    np_ulong   PRIM_type;      /* always N_OPTMGMT_REQ */
    np_ulong   QOS_length;     /* length of QOS parameter values */
    np_ulong   QOS_offset;     /* offset of QOS parameter values */
    np_ulong   OPTMGMT_flags;  /* options management flags */
} N_optmgmt_req_t;

/*
 * Error acknowledgement for CONS services
 */
typedef struct
{
    np_ulong   PRIM_type;      /* always N_ERROR_ACK */
    np_ulong   ERROR_prim;     /* primitive in error */
    np_ulong   NPI_error;      /* NPI error code */
    np_ulong   UNIX_error;     /* UNIX error code */
} N_error_ack_t;

/*
 * Successful completion acknowledgement
 */
typedef struct
{
    np_ulong   PRIM_type;      /* always N_OK_ACK */
    np_ulong   CORRECT_prim;   /* primitive being acknowledged */
} N_ok_ack_t;

/*
 * CONS PRIMITIVES
 */

/*
 * Network connection request
 */
typedef struct
{
    np_ulong   PRIM_type;      /* always N_CONN_REQ */
    np_ulong   DEST_length;    /* destination address length */
    np_ulong   DEST_offset;    /* destination address offset */
    np_ulong   CONN_flags;     /* bit masking for options flags */
    np_ulong   QOS_length;     /* length of QOS parameter values */
    np_ulong   QOS_offset;     /* offset of QOS parameter values */
} N_conn_req_t;

/*
 * Connection indication
 */
typedef struct
{
    np_ulong   PRIM_type;      /* always N_CONN_IND */
    np_ulong   DEST_length;    /* destination address length */
    np_ulong   DEST_offset;    /* destination address offset */
    np_ulong   SRC_length;     /* source address length */
    np_ulong   SRC_offset;     /* source address offset */
    np_ulong   SEQ_number;     /* sequence number */
    np_ulong   CONN_flags;     /* bit masking for options flags */
    np_ulong   QOS_length;     /* length of QOS parameter values */
    np_ulong   QOS_offset;     /* offset of QOS parameter values */
} N_conn_ind_t;

/*
 * Connection response
 */
typedef struct
{
    np_ulong   PRIM_type;      /* always N_CONN_RES */
    np_ulong   TOKEN_value;    /* NC response token value */
    np_ulong   RES_length;     /* responding address length */
    np_ulong   RES_offset;     /* responding address offset */
    np_ulong   SEQ_number;     /* sequence number */
    np_ulong   CONN_flags;     /* bit masking for options flags */
    np_ulong   QOS_length;     /* length of QOS parameter values */
    np_ulong   QOS_offset;     /* offset of QOS parameter values */
} N_conn_res_t;

/*
 * Connection confirmation
 */
typedef struct
{
    np_ulong   PRIM_type;      /* always N_CONN_CON */
    np_ulong   RES_length;     /* responding address length */
    np_ulong   RES_offset;     /* responding address offset */
    np_ulong   CONN_flags;     /* bit masking for options flags */
    np_ulong   QOS_length;     /* length of QOS parameter values */
    np_ulong   QOS_offset;     /* offset of QOS parameter values */
} N_conn_con_t;

/*
 * Connection mode data transfer request
 */
typedef struct
{
    np_ulong   PRIM_type;	   /* always N_DATA_REQ */
    np_ulong   DATA_xfer_flags;	   /* data transfer flags */
} N_data_req_t;

/*
 * NPI MORE_DATA_FLAG for segmenting NSDU into more than 1 NIDUs
 */
#define N_MORE_DATA_FLAG    0x00000001L /* Indicates that the next NIDU is */
					/* part of this NSDU */

/*
 * NPI Receipt confirmation request set flag
 */
#define N_RC_FLAG	    0x00000002L /* Indicates if receipt */
					/* confirmation is required */

/*
 * Incoming data indication for an NC
 */
typedef struct
{
    np_ulong   PRIM_type;	   /* always N_DATA_IND */
    np_ulong   DATA_xfer_flags;	   /* data transfer flags */
} N_data_ind_t;

/*
 * Data acknowledgement request
 */
typedef struct
{
    np_ulong   PRIM_type;  /* always N_DATACK_REQ */
} N_datack_req_t;

/*
 * Data acknowledgement indication
 */
typedef struct
{
    np_ulong   PRIM_type;  /* always N_DATACK_IND */
} N_datack_ind_t;

/*
 * Expedited data transfer request
 */
typedef struct
{
    np_ulong   PRIM_type;  /* always N_EXDATA_REQ */
} N_exdata_req_t;

/*
 * Expedited data transfer indication
 */
typedef struct
{
    np_ulong   PRIM_type;  /* always N_EXDATA_IND */
} N_exdata_ind_t;

/*
 * NC reset request
 */
typedef struct
{
    np_ulong   PRIM_type;      /* always N_RESET_REQ */
    np_ulong   RESET_reason;   /* reason for reset */
} N_reset_req_t;

/*
 * NC reset indication
 */
typedef struct
{
    np_ulong   PRIM_type;      /* always N_RESET_IND */
    np_ulong   RESET_orig;     /* reset originator */
    np_ulong   RESET_reason;   /* reason for reset */
} N_reset_ind_t;

/*
 * NC reset response
 */
typedef struct
{
    np_ulong   PRIM_type;  /* always N_RESET_RES */
} N_reset_res_t;

/*
 * NC reset confirmed
 */
typedef struct
{
    np_ulong   PRIM_type;  /* always N_RESET_CON */
} N_reset_con_t;

/*
 * NC disconnection request
 */
typedef struct
{
    np_ulong   PRIM_type;      /* always N_DISCON_REQ */
    np_ulong   DISCON_reason;  /* reason */
    np_ulong   RES_length;     /* responding address length */
    np_ulong   RES_offset;     /* responding address offset */
    np_ulong   SEQ_number;     /* sequence number */
} N_discon_req_t;

/*
 * NC disconnection indication
 */
typedef struct
{
    np_ulong   PRIM_type;      /* always N_DISCON_IND */
    np_ulong   DISCON_orig;    /* originator */
    np_ulong   DISCON_reason;  /* reason */
    np_ulong   RES_length;     /* address length */
    np_ulong   RES_offset;     /* address offset */
    np_ulong   SEQ_number;     /* sequence number */
} N_discon_ind_t;

/*
 * CLNS PRIMITIVES
 */

/*
 * Unitdata transfer request
 */
typedef struct
{
    np_ulong   PRIM_type;	   /* always N_UNITDATA_REQ */
    np_ulong   DEST_length;	   /* destination address length */
    np_ulong   DEST_offset;	   /* destination address offset */
    np_ulong   RESERVED_field[2];  /* reserved field for DLPI compatibility */
} N_unitdata_req_t;

/*
 * Unitdata transfer indication
 *
 * Note: The body of the specification for NPI 2.0.0 has the addresses
 * in the order DEST-SRC.  The npi.h printed at the end of the spec has
 * SRC-DEST.  It is not clear which is correct.  We will go with the npi.h
 * order with an easy option to get the other order if need be.
 */
typedef struct
{
    np_ulong   PRIM_type;      /* always N_UNITDATA_IND */
#ifdef NPI_UNITDATA_IND_SPEC_ORDER
    np_ulong   DEST_length;    /* source address length */
    np_ulong   DEST_offset;    /* source address offset */
    np_ulong   SRC_length;     /* source address length */
    np_ulong   SRC_offset;     /* source address offset */
#else
    np_ulong   SRC_length;     /* source address length */
    np_ulong   SRC_offset;     /* source address offset */
    np_ulong   DEST_length;    /* source address length */
    np_ulong   DEST_offset;    /* source address offset */
#endif
    np_ulong   ERROR_type;     /* reserved field for DLPI compatibility */
} N_unitdata_ind_t;

/*
 * Unitdata error indication for CLNS services
 */
typedef struct
{
    np_ulong   PRIM_type;      /* always N_UDERROR_IND */
    np_ulong   DEST_length;    /* destination address length */
    np_ulong   DEST_offset;    /* destination address offset */
    np_ulong   RESERVED_field; /* reserved field for DLPI compatibility */
    np_ulong   ERROR_type;     /* error type */
} N_uderror_ind_t;

/*
 * The following represents a union of all the NPI primitives
 */
union N_primitives
{
    np_ulong		type;
    N_info_req_t	info_req;	/* information request */
    N_info_ack_t	info_ack;	/* information acknowledgement */
    N_bind_req_t	bind_req;	/* bind request */
    N_bind_ack_t	bind_ack;	/* bind acknowledgement */
    N_unbind_req_t	unbind_req;	/* unbind acknowledgement */
    N_optmgmt_req_t	optmgmt_req;	/* options management request */
    N_error_ack_t	error_ack;	/* error acknowledgement */
    N_uderror_ind_t 	uderror_ind;	/* unitdata error indication */
    N_ok_ack_t		ok_ack;		/* ok acknowledgement */
    N_conn_req_t	conn_req;	/* connect request */
    N_conn_ind_t	conn_ind;	/* connect indication */
    N_conn_res_t	conn_res;	/* connect response */
    N_conn_con_t	conn_con;	/* connect confirm */
    N_data_req_t	data_req;	/* data request */
    N_data_ind_t	data_ind;	/* data indictation */
    N_datack_req_t	datack_req;	/* data acknowledgement request */
    N_datack_ind_t	datack_ind;	/* data acknowledgement indication */
    N_exdata_req_t	exdata_req;	/* expedited data request */
    N_exdata_ind_t	exdata_ind;	/* expedited data indication */
    N_reset_req_t	reset_req;	/* reset request */
    N_reset_ind_t	reset_ind;	/* reset indication */
    N_reset_res_t	reset_res;	/* reset response */
    N_reset_con_t	reset_con;	/* reset confirm */
    N_discon_req_t	discon_req;	/* disconnect request */
    N_discon_ind_t	discon_ind;	/* disconnect indication */
    N_unitdata_req_t	unitdata_req;	/* unitdata request */
    N_unitdata_ind_t	unitdata_ind;	/* unitdata indication */
};


#endif					/* from top of file */
