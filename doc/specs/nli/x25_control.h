/* linkid:              the number of the link.
 *
 * network_state:       a code fining the network state.  The codes are as
 *                      follows:
 *
 *                      1 - connecting to DXE
 *                      2 - connecte resolving DXE
 *                      3 - random wait started
 *                      4 - connected and resolved DXE
 *                      5 - DTE restart request
 *                      6 - waiting link disconnect reply
 *                      7 - buffer to enter WtgRES
 *                      8 - buffer to enter L3 restarting
 *                      9 - buffer to enter L_disconnect
 *                      10 - registration request
 *
 * mon_array:           the array containing the statistics mon_array is
 *                      defined in the file x25_control.h
 */
struct perlinkstats {
    uint32_t linkid;                    /* link id (ppa) */
    int network_state;                  /* network state */
    uint32_t mon_array[link_mon_size];  /* L3 per link monitor array */
};

/* version:             the version of NLI supported by the X.25 multiplexor.
 */
struct nliformat {
    unsigned char version;              /* NLI version number */
};

enum {
    cll_in_v = 1,
#define cll_in_v        cll_in_v
    cll_out_v,
#define cll_out_v       cll_out_v
    caa_in_v,
#define caa_in_v        caa_in_v
    caa_out_v,
#define caa_out_v       caa_out_v
    dt_in_v,
#define dt_in_v         dt_in_v
    dt_out_v,
#define dt_out_v        dt_out_v
    ed_in_v,
#define ed_in_v         ed_in_v
    ed_out_v,
#define ed_out_v        ed_out_v
    rnr_in_v,
#define rnr_in_v        rnr_in_v
    rnr_out_v,
#define rnr_out_v       rnr_out_v
    rr_in_v,
#define rr_in_v         rr_in_v
    rr_out_v,
#define rr_out_v        rr_out_v
    rst_in_v,
#define rst_in_v        rst_in_v
    rst_out_v,
#define rst_out_v       rst_out_v
    rsc_in_v,
#define rsc_in_v        rsc_in_v
    rsc_out_v,
#define rsc_out_v       rsc_out_v
    clr_in_v,
#define clr_in_v        clr_in_v
    clr_out_v,
#define clr_out_v       clr_out_v
    clc_in_v,
#define clc_in_v        clc_in_v
    clc_out_v,
#define clc_out_v       clc_out_v
    octets_in_v,
#define octetst_in_v    octetst_in_v
    octets_out_v,
#define octets_out_v    octets_out_v
    rst_timeouts_v,
#define rst_timeouts_v  rst_timeouts_v
    ed_timeouts_v,
#define ed_timeouts_v   ed_timeouts_v
    prov_rst_in_v,
#define prov_rst_in_v   prov_rst_in_v
    rem_rst_in_v,
#define rem_rst_in_v    rem_rst_in_v
    perVCmon_size
#define perVCmon_size   perVCmon_size
};

/* rem_addr:            the called address if its an outgoing call, or the
 *                      calling address for incoming calls.
 *
 * xu_ident:            the link identifier
 *
 * process_id:          the relevant user id
 *
 * lci:                 the logical channel identifier
 *
 * xstate:              the VC state
 *
 * xtag:                the VC check record
 *
 * ampvc:               set to 1 if this is a PVC
 *
 * call_direction:      0 indicates incoming call, 1 outgoing call
 *
 * perVC_stats:         an array containing the per-virtual channel circuit
 *                      statistics.  The array is defined in the x25_control.h
 *                      file.
 */
struct vcinfo {
    struct xaddrf rem_addr;
    /* struct xaddrf loc_addr; */
    uint32_t xu_ident;                  /* link identifier */
    uint32_t process_id;                /* relevant user id */
    unsigned short lci;                 /* logical channel id */
    unsigned char xstate;               /* VC state */
    unsigned char xtag;                 /* VC check record */
    unsigned char ampvc;                /* true if is a PVC */
    unsigned char call_direction;       /* 0, incoming; 1, outgoing */
    unsigned char domain;               /* vctype */
    int perVC_stats[perVCmon_size];     /* per-VC statistics */
};

/* entries:             contains the structure for the returned mapping
 *                      entries.
 *
 * first_ent:           informs the X.25 multiplexor where to start or
 *                      restart the table read.  It should intially be set to
 *                      zero 0, to indicate starting at the beginning of the
 *                      table.  On return, it points to the next entry.
 *
 * num_ent:             indicate the number of mapping entries returned in the
 *                      entries member.  It should be set to 0 before maing
 *                      the ioctl.
 */
struct pvcmapf {
    struct pvcconff entries[MAX_PVC_ENTS];      /* data buffer */
    int first_ent;                      /* where to start search */
    unsigned char num_ent;              /* number entries returned */
};

// int N_getstats[mon_size];
enum {
    cll_in_g = 0,
    caa_in_g,
    caa_out_g,
    ed_in_g,
    ed_out_g,
    rnr_in_g,
    rnr_out_g,
    rr_in_g,
    rr_out_g,
    rst_in_g,
    rst_out_g,
    rsc_in_g,
    rsc_out_g,
    clr_in_g,
    clr_out_g,
    clc_in_g,
    clc_out_g,
    cll_coll_g,
    cll_uabort_g,
    rjc_buflow_g,
    rjc_coll_g,
    rjc_failNRS_g,
    rjc_lstate_g,
    rjc_nochnl_g,
    rjc_nouser_g,
    rjc_remote_g,
    rjc_u_g,
    dg_in_g,
    dg_out_g,
    p4_ferr_g,
    rem_perr_g,
    rem_ferr_g,
    res_in_g,
    res_out_g,
    vcs_labort_g,
    r23exp_g,
    l2conin_g,
    l2conok_g,
    l2conrej_g,
    l2refusal_g,
    l2lzap_g,
    l2r20exp_g,
    l2dxeexp_g,
    l2dxebuf_g,
    l2noconfig_g,
    xiffnerror_g,
    xintdisc_g,
    xifaborts_g,
    PVCusergone_g,
    max_opens_g,
    vcs_est_g,
    bytes_in_g,
    bytes_out_g,
    dt_in_g,
    dt_out_g,
    res_conf_in_g,
    res_conf_out_g,
    reg_in_g,
    reg_out_g,
    reg_conf_in_g,
    reg_conf_out_g,
    l2r28exp_g,
    mon_size
};

struct N_getstats {
    int cll_in;                         /* calls received and indicated */
    int caa_in;                         /* call established outgoing */
    int caa_out;                        /* call established incoming */
    int ed_in;                          /* interrupts recv */
    int ed_out;                         /* interrupts sent */
    int rnr_in;                         /* receiver not ready recv */
    int rnr_out;                        /* receiver not ready sent */
    int rr_in;                          /* receiver ready recv */
    int rr_out;                         /* receiver ready sent */
    int rst_in;                         /* resets recv */
    int rst_out;                        /* resets sent */
    int rsc_in;                         /* restart confirms recv */
    int rsc_out;                        /* restart confirms sent */
    int clr_in;                         /* clears recv */
    int clr_out;                        /* clears sent */
    int clc_in;                         /* clear confirms recv */
    int clc_out;                        /* clear confirms sent */
    int cll_coll;                       /* call collision count (not rjc) */
    int cll_uabort;                     /* calls aborted by user b4 sent */
    int rjc_buflow;                     /* calls rejected no buffs b4 sent */
    int rjc_coll;                       /* calls rejected collision DCE mode */
    int rjc_failNRS;                    /* calls rejected netative NRS response */
    int rjc_lstate;                     /* calls rejected link disconnecting */
    int rjc_nochnl;                     /* calls rejected no lcns left */
    int rjc_nouser;                     /* in call but no user on NSAP */
    int rjc_remote;                     /* call rejected by remote responder */
    int rjc_u;                          /* call rejected by NS user */
    int dg_in;                          /* DIAG packets recv */
    int dg_out;                         /* DIAG packets sent */
    int p4_ferr;                        /* format errors in P4 */
    int rem_perr;                       /* remote protocol errors */
    int rem_ferr;                       /* restart format errors */
    int res_in;                         /* restarts recv (inc DTE/DXE) */
    int res_out;                        /* restarts sent (inc DTE/DXE) */
    int vcs_labort;                     /* circtuis aborted via link event */
    int r23exp;                         /* circuits hung by r23 expiry */
    int l2conin;                        /* Link level connect established */
    int l2conok;                        /* LLC connections accepted */
    int l2conrej;                       /* LLC connections rejected */
    int l2refusal;                      /* LLC connect requests refused */
    int l2lzap;                         /* operator requests to kill link */
    int l2r20exp;                       /* R20 transmission expiry */
    int l2dxeexp;                       /* DXE connect expiry */
    int l2dxebuf;                       /* DXE resolv abort - no buffers */
    int l2noconfig;                     /* no config base - error */
    int xiffnerror;                     /* upper interface bad M_PROTO type */
    int xintdisc;                       /* internal disconnect events */
    int xifaborts;                      /* interface abort_vc called */
    int PVCusergone;                    /* count of non-user interactions */
    int max_opens;                      /* highest number of simultaneous opens */
    int vcs_est;                        /* VCs established since reset */
    int bytes_in;                       /* data bytes recv */
    int bytes_out;                      /* data bytes sent */
    int dt_in;                          /* data packets recv */
    int dt_out;                         /* data packets sent */
    int res_conf_in;                    /* restart confirms recv */
    int res_conf_out;                   /* restart confirms sent */
    int reg_in;                         /* registration requests recv */
    int reg_out;                        /* registration requests sent */
    int reg_conf_in;                    /* registration confirms recv */
    int reg_conf_out;                   /* registration confirms sent */
    int l2r28exp;                       /* R28 transmission expiries */
};

struct pervcinfo {
    struct xaddrf rem_addr;
    uint32_t xu_ident;
    uint32_t process_id;
    unsigned short lci;
    unsigned char xstate;
    unsigned char xtag;
    unsigned char ampvc;
    unsigned char call_direction;
    unsigned char domain;
    uint32_t perVC_stats[perVCstat_size];
    /* compatibility */
    unsigned char vctype;
    struct xaddrf loc_addr;
    uint32_t start_time;
};

struct vcstatsf {
    int first_ent;                      /* where to start search */
    unsigned char num_ent;              /* number entries returned */
    struct pervcinfo vc;                /* data buffer, extendable by malloc overlay */
};

struct vcstatusf {
    struct vcinfo vcs[MAX_VC_ENTS];
    int first_ent;
    unsigned char num_ent;
};

#define X25_LLC         1       /* X.25(84/88)/LLC2 */
#define X25_88          2       /* X.25(88) */
#define X25_84          3       /* X.25(84) */
#define X25_80          4       /* X.25(80) */
#define GNS             5       /* UK */
#define AUSTPAC         6       /* Austrailia */
#define DATAPAC         7       /* Canada */
#define DDN             8       /* USA */
#define TELKNET         9       /* USA */
#define TRANSPAC        10      /* France */
#define TYMNET          11      /* USA */
#define DATAEX_P        12      /* Germany */
#define DDX_P           13      /* Japan */
#define VENUS_P         14      /* Japan */
#define ACCUNET         15      /* USA */
#define ITAPAC          16      /* Italy */
#define DATAPAK         17      /* Sweden */
#define DATANET         18      /* Holland */
#define DCS             19      /* Belgium */
#define TELEPAC         20      /* Switzerland */
#define F_DATAPAC       21      /* Finland */
#define FINPAC          22      /* Finland */
#define PACNET          23      /* New Zeland */
#define LUXPAC          24      /* Luxembourg */
#define x25_Circuit     25      /* dialup call */

/* SUB_MODES */
#define SUB_EXTENDED
#define BAR_EXTENDED
#define SUB_FSELECT
#define SUB_FSRRESP
#define SUB_REVCHARGE
#define SUB_LOC_CHG_PREV
#define SUB_TOA_NPI_FMT
#define BAR_TOA_NPI_FMT
#define BAR_CALL_X32_REG
#define SUB_NUI_OVERRIDE
#define BAR_INCALL
#define BAR_OUTCALL

/* PSDN Modes */
#define ACC_NODIAG
#define USE_DIAG
#define CCITT_CLEAR_LEN
#define BAR_DIAG
#define DISC_NZ_DIAG
#define ACC_HEX_ADD
#define BAR_NONPRIV_LISTEN
#define INTL_PRIO
#define DATAPACK_PRIORITY
#define ISO_8882_MODE
#define X121_MAC_OUT
#define X121_MAC_IN

struct linkoptformat {
    uint32_t U_LINK_ID;
    unsigned short newSUB_MODES;
    unsigned char rd_wr;
};

#define NUIMAXSIZE 64
#define NUIFACMAXSIZE 32

struct nuiformat {
    unsigned char nui_len;
    unsigned char nui_string[NUIMAXSIZE];       /* Network User Identifier */
};

struct facformat {
    unsigned short SUB_MODES;           /* Mode tuning bits for net */
    unsigned char LOCDEFPKTSIZE;        /* loc default packet size */
    unsigned char REMDEFPKTSIZE;        /* rem default packet size */
    unsigned char LOCDEFWSIZE;          /* loc default window size */
    unsigned char REMDEFWSIZE;          /* rem default window size */
    unsigned char locadefthclass;       /* loc default class */
    unsigned char remdeflthclass;       /* rem default class */
    unsigned char CUG_CONTROL;          /* CUG facilities */
};

struct nui_del {
    char prim_class;                    /* always NUI_MSG */
    char op;                            /* always NUI_DEL */
    struct nuiformat nuid;              /* NUI to delete */
};

struct nui_get {
    char prim_class;                    /* always NUI_MSG */
    char op;                            /* always NUI_GET */
    struct nuiformat nuid;              /* NUI to get */
    struct facformat nuifacility;       /* NUI facilities */
};

struct nui_mget {
    unsigned int first_ent;
    unsigned int last_ent;
    unsigned int num_ent;
    char buf[MGET_NBUFSIZE];
};

struct nui_put {
    char prim_class;                    /* always NUI_MSG */
    char op;                            /* always NUI_ENT */
    struct nuiformat nuid;              /* NUI to put */
    struct facformat nuifacility;       /* NUI facilities */
};

struct nui_reset {
    char prim_class;                    /* always NUI_MSG */
    char op;                            /* always NUI_RESET */
};

struct pvcconff {
    uint32_t link_id;                   /* link id */
    unsigned short lci;                 /* logical channel identifier */
    unsigned char locpacket;            /* local packet size */
    unsigned char rempacket;            /* remote packet size */
    unsigned char locwsize;             /* local window size */
    unsigned char remwsize;             /* remote window size */
};

struct trc_regioc {
    uint8_t all_links;                  /* trace on all links */
    uint8_t spare[3];                   /* for alignment */
    uint32_t linkid;                    /* link id */
    uint8_t level;                      /* level for tracing required */
    uint8_t space2[3];                  /* for alignment */
    uint32_t active[MAX_LINES + 1];     /* tracing actively on */
};

#define TR_CTL          100     /* basic */
#define TR_LCLC2_DAT    101     /* basic + LLC2 parameters */
#define TR_LAPB_DAT     TR_CTL  /* basic for now */
#define TR_MLP_DAT      TR_CTL  /* basic for now */
#define TR_X25_DAT      TR_CTL  /* basic for now */
#define TR_DLPI         102     /* type use for tracing DLPI primitives */

/* Format for control part of trace messages */
struct trc_ctl {
    uint8_t trc_prim;                   /* trace message identifier */
    uint8_t trc_mid;                    /* id of protocol module */
    uint16_t trc_space;                 /* for alignment */
    uint32_t trc_linkid;                /* link id */
    uint8_t trc_rcv;                    /* message tx or rx */
    uint8_t trc_space2[3];              /* for alignment */
    uint32_t trc_time;                  /* time stamp */
    uint16_t trc_seq;                   /* message sequence number */
};

#define R_NONE          0
#define R_X121_HOST     1
#define R_X121_PREFIX   2
#define R_AEF_HOST      3
#define R_AEF_PREFIX    4
#define R_AEF_SOURCE    5

#define MAX_PID_LEN     4

typedef struct x25_route_s {
    uint32_t index;                     /* used for reading next route */
    u_char r_type;
    CONN_ADR x121;
    u_char pid_len;
    u_char pid[MAX_PID_LEN];
    AEF aef;
    int linkid;
    X25_MAXADDR mac;
    int use_count;
    char pstn_number[16];
} X25_ROUTE;

#define SNIOC ('N'<<8)

#if 0
#define N_snident               (SNIOC|0x01)
#define N_snmode                (SNIOC|0x02)
#define N_snconfig              (SNIOC|0x03)
#define N_snread                (SNIOC|0x04)
#define N_getstats              (SNIOC|0x05)
#define N_zerostats             (SNIOC|0x06)
#define N_putpvcmap             (SNIOC|0x07)
#define N_getpvcmap             (SNIOC|0x08)
#define N_getVCstatus           (SNIOC|0x09)
#define N_getnliversion         (SNIOC|0x0a)
#define N_traceon               (SNIOC|0x0b)
#define N_traceoff              (SNIOC|0x0c)
#define N_nuimsg                (SNIOC|0x0d)
#define N_nuiput                (SNIOC|0x0e)
#define N_nuidel                (SNIOC|0x0f)
#define N_nuiget                (SNIOC|0x10)
#define N_nuimget               (SNIOC|0x11)
#define N_nuireset              (SNIOC|0x12)
#define N_zeroVCstats           (SNIOC|0x13)
#define N_putx32map             (SNIOC|0x14)
#define N_getx32map             (SNIOC|0x15)
#define N_getNSNIDstats         (SNIOC|0x16)
#define N_zeroSNIDstats         (SNIOC|0x17)
#define N_setQOSDATPRI          (SNIOC|0x18)
#define N_resetQOSDATPRI        (SNIOC|0x19)
#endif

#define N_getnliversion         (SNIOC|0x00)    /* read NLI version */

#define N_nuidel                (SNIOC|0x01)    /* delete specified NUI mapping (root) */
#define N_nuiget                (SNIOC|0x02)    /* read specified NUI mapping */
#define N_nuimget               (SNIOC|0x03)    /* read all NUI mappings */
#define N_nuiput                (SNIOC|0x04)    /* store a set of NUI mappings (root) */
#define N_nuireset              (SNIOC|0x05)    /* delete all NUI mappings (root) */

#define N_getstats              (SNIOC|0x06)    /* read X.25 multiplexor statistics */
#define N_zerostats             (SNIOC|0x07)    /* reset X.25 multiplexor statistics to
                                                   zero (root) */

#define N_getoneVCstats         (SNIOC|0x08)    /* get status and statistics for VC
                                                   associated with current stream */
#define N_getpvcmap             (SNIOC|0x09)    /* get default packet and window sizes */
#define N_getVCstats            (SNIOC|0x0a)    /* get per VC statistics */
#define N_getVCstatus           (SNIOC|0x0b)    /* get per VC state and statistics */
#define N_putpvcmap             (SNIOC|0x0c)    /* change per VC packet and window sizes */

#define N_traceon               (SNIOC|0x0d)    /* start packet level tracing */
#define N_traceoff              (SNIOC|0x0e)    /* stop packet level tracing */

#define N_X25_ADD_ROUTE         (SNIOC|0x0f)    /* add a new route or update an existing
                                                   route (root) */
#define N_X25_FLUSH_ROUTE       (SNIOC|0x10)    /* clear all etnries from the routing
                                                   table (root) */
#define N_X25_GET_ROUTE         (SNIOC|0x11)    /* obtain routing information for
                                                   specified address */
#define N_X25_NEXT_ROUTE        (SNIOC|0x12)    /* obtain routine information for the
                                                   next route in the routeing table */
#define N_X25_RM_ROUTE          (SNIOC|0x13)    /* remove the specified route (root) */

#define N_linkconfig            (SNIOC|0x14)    /* configure the wlcfg database */
#define N_linkent               (SNIOC|0x15)    /* configure newly linked driver */
#define N_linkmode              (SNIOC|0x16)    /* alter link characetistics */
#define N_linkread              (SNIOC|0x17)    /* read the wlcfg database */
