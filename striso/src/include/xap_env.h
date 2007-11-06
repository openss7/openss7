/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ifndef __XAP_ENV_H__
#define __XAP_ENV_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

enum {
	AP_UNBOUND = 0,
	/**< Unbound state. */
#define AP_UNBOUND		AP_UNBOUND
	AP_IDLE,
	/**< Idle state (bound to address). */
#define AP_IDLE			AP_IDLE
	AP_DATA_XFER,
	/**< Data transfer phase. */
#define AP_DATA_XFER		AP_DATA_XFER
	AP_WASSOCrsp_ASSOCind,
	/**< Wait for A-ASSOCIATE response having issued A-ASSOCIATE indication. */
#define AP_WASSOCrsp_ASSOCind	AP_WASSOCrsp_ASSOCind
	AP_WASSOCcnf_ASSOCreq,
	/**< Wait for A-ASSOCIATE confirmation having issued A-ASSOCIATE request. */
#define AP_WASSOCcnf_ASSOCreq	AP_WASSOCcnf_ASSOCreq
	AP_WRELrsp_RELind,
	/**< Wait for A-RELEASE response having issued A-RELEASE indication. */
#define AP_WRELrsp_RELind	AP_WRELrsp_RELind
	AP_WRELcnf_RELreq,
	/**< Wait for A-RELEASE configuration having issued A-RELEASE indication. */
#define AP_WRELcnf_RELreq	AP_WRELcnf_RELreq
	AP_WRESYNrsp_RESYNind,
	/**< Wait for P-RESYNCHRONIZATION response having issued P-RESYNCHRONIZATION indication. */
#define AP_WRESYNrsp_RESYNind	AP_WRESYNrsp_RESYNind
	AP_WRESYNcnf_RESYNreq,
	/**< Wait for P-RESYNCHRONIZATION confirmation having issued P-RESYNCHRONIZATION request. */
#define AP_WRESYNcnf_RESYNreq	AP_WRESYNcnf_RESYNreq
	AP_WRELrsp_RELind_init,
	/**< Wait for A-RELEASE response having issued A-RELEASE indication from initial state. */
#define AP_WRELrsp_RELind_init	AP_WRELrsp_RELind_init
	AP_WRELcnf_RELres_rsp,
	/**< Wait for A-RELEASE confirm having issued A-RELEASE request from the response state. */
#define AP_WRELcnf_RELreq_rsp	AP_WRELcnf_RELres_rsp
	AP_WACTDrsp_ACTDind,
	/**< Wait for P-ACTIVITY-DISCARD response having issued P-ACTIVITY-DISCARD indication */
#define AP_WACTDrsp_ACTDind	AP_WACTDrsp_ACTDind
	AP_WACTDcnf_ACTDreq,
	/**< Wait for P-ACTIVITY-DISCARD confirm having issued P-ACTIVITY-DISCARD request */
#define AP_WACTDcnf_ACTDreq	AP_WACTDcnf_ACTDreq
	AP_WACTErsp_ACTEind,
	/**< Wait for P-ACTIVITY-END response having issued P-ACTIVITY-END indication */
#define AP_WACTErsp_ACTEind	AP_WACTErsp_ACTEind
	AP_WACTEcnf_ACTEreq,
	/**< Wait for P-ACTIVITY-END confirm having issued P-ACTIVITY-END request */
#define AP_WACTEcnf_ACTEreq	AP_WACTEcnf_ACTEreq
	AP_WACTIrsp_ACTIind,
	/**< Wait for P-ACTIVITY-INTERRUPT response having issued P-ACTIVITY-INTERRUPT indication */
#define AP_WACTIrsp_ACTIind	AP_WACTIrsp_ACTIind
	AP_WACTIcnf_ACTIreq,
	/**< Wait for P-ACTIVITY-INTERRUPT confirm having issued P-ACTIVITY-INTERRUPT request */
#define AP_WACTIcnf_ACTIreq	AP_WACTIcnf_ACTIreq
	AP_WSYNCMArsp_SYNCMAind,
	/**< Wait for P-SYNC-MAJOR response having issued P-SYNC-MAJOR indication */
#define AP_WSYNCMArsp_SYNCMAind	AP_WSYNCMArsp_SYNCMAind
	AP_WSYNCMAcnf_SYNCMAreq,
	/**< Wait for P-SYNC-MAJOR confirm having issued P-SYNC-MAJOR request */
#define AP_WSYNCMAcnf_SYNCMAreq	AP_WSYNCMAcnf_SYNCMAreq
	AP_WCDATArsp_CDATAind,
	/**< Wait for P-CAPABILITY-DATA response having issued P-CAPABILITY-DATA indication */
#define AP_WCDATArsp_CDATAind	AP_WCDATArsp_CDATAind
	AP_WCDATAcnf_CDATAreq,
	/**< Wait for P-CAPABILITY-DATA confirm having issued P-CAPABILITY-DATA request */
#define AP_WCDATAcnf_CDATAreq	AP_WCDATAcnf_CDATAreq
	AP_WRECOVERYind,
#define AP_WRECOVERYind		AP_WRECOVERYind
	AP_WRECOVERYreq
#define AP_WRECOVERYreq		AP_WRECOVERYreq
};

enum {
	AP_ACSE_AVAIL,
	/**< Indicates which version of the ACSE protocol are currently available.  Not used in
	   "X.410-1984" mode, see AP_MODE_SEL. */
#define AP_ACSE_AVAIL		AP_ACSE_AVAIL
#	define	AP_ACSEVER1		(1<< 0)	/**< ACSE protocol version 1 */
	AP_ACSE_SEL,
	/**< Indicates which version of the ACSE protocol has been selected for use with the
	   current association.  Not use in "X.410-1984" mode, see AP_MODE_SEL.  */
#define AP_ACSE_SEL		AP_ACSE_SEL
	AP_AFU_AVAIL,
	/**< Indicates which optional ACSE functional units are currently available.  Not used in
	   "X.410-1984" mode. */
#define AP_AFU_AVAIL		AP_AFU_AVAIL
#	define	AP_ACSE_AUTH		(1<< 0) /**< Authentication FU */
#	define	AP_ACSE_CNTXNEG		(1<< 1) /**< ASO Context Negotiation FU */
#	define	AP_ACSE_HLASSOC		(1<< 2) /**< High Level Association FU */
#	define	AP_ACSE_NESTASSOC	(1<< 3) /**< Nested Association FU */
	AP_AFU_SEL,
	/**< Indicates which optional ACSE functional units have been requested for use over the
	   current association. Not used in "X.410-1984" mode. */
#define AP_AFU_SEL		AP_AFU_SEL
	AP_BIND_PADDR,
	/**< The AP_BIND_PADDR attribute is used to indicate the address to which this isntance of 
	   XAP will be bound when ap_bind(3) is called.  Setting this attribute will result in the
	   attribute AP_LCL_PADDR being set to the same value.  Nevertheless, the attributes
	   AP_BIND_PADDR and AP_LCL_PADDR do not always have the same value.  For example, a
	   listener application might set thte AP_BIND_PADDR to a wildcard address and use the
	   called address (conveyed by the AP_LCL_PADDR attribute) specified in each indication
	   primtive to dispatach associations. */
#define AP_BIND_PADDR		AP_BIND_PADDR
	AP_CLD_AEID,
	/**< The AP_CLD_AEID is the called AE invocation identifier parameter of the AARQ APDU. This
	   attribute is not used in the X.410-1984 mode. Refer to the description of the AP_MODE_SEL 
	   attribute. */
#define AP_CLD_AEID		AP_CLD_AEID
	AP_CLD_AEQ,
	/**< The AP_CLD_AEQ is the called AE-qualifier parameter of the AARQ APDU. This attribute is
	   not used in the X.410-1984if mode. Refer to the description of the AP_MODE_SEL
	   attribute. */
#define AP_CLD_AEQ		AP_CLD_AEQ
	AP_CLD_APID,
	/**< The AP_CLD_APID is the called AP invocation identifier parameter of the AARQ APDU. This
	   attribute is not used in the X.410-1984 mode. Refer to the description of the
	   AP_MODE_SEL attribute. */
#define AP_CLD_APID		AP_CLD_APID
	AP_CLD_APT,
	/**< The AP_CLD_APT is the called AP-title parameter of the AARQ APDU. This attribute is not
	   used in the X.410-1984te mode. Refer to the description of the AP_MODE_SEL attribute. */
#define AP_CLD_APT		AP_CLD_APT
	AP_CLD_CONN_ID,
	/**< The AP_CLD_CONN_ID attribute conveys the value of the session connection identifier that 
	   was proposed by the association-responder. */
#define AP_CLD_CONN_ID		AP_CLD_CONN_ID
	AP_CLG_AEID,
	/**< The AP_CLG_AEID is the calling AE invocation identifier parameter of the AARQ APDU. This 
	   attribute is not used in the X.410-1984 mode. Refer to the description of the
	   AP_MODE_SEL attribute. */
#define AP_CLG_AEID		AP_CLG_AEID
	AP_CLG_AEQ,
	/**< The AP_CLG_AEQ is the calling AE-qualifier parameter of the AARQ APDU. See the
	   description of the ap_aeq_t data type for information about how its absence is
	   indicated. This attribute is not used in the X.410-1984 mode. Refer to the description
	   of the AP_MODE_SEL attribute. */
#define AP_CLG_AEQ		AP_CLG_AEQ
	AP_CLG_APID,
	/**< The AP_CLG_APID is the calling AP invocation identifier parameter of the AARQ APDU. This 
	   attribute is not used in the X.410-1984 mode. Refer to the description of the
	   AP_MODE_SEL attribute. */
#define AP_CLG_APID		AP_CLG_APID
	AP_CLG_APT,
	/**< The AP_CLG_APT is the calling AP-title parameter of the AARQ APDU. See the description
	   of the ap_apt_t data type for information about how its absence is indicated. This
	   attribute is not used in the X.410-1984 mode. */
#define AP_CLG_APT		AP_CLG_APT
	AP_CLG_CONN_ID,
	/**< The AP_CLG_CONN_ID attribute conveys the value of the session connection identifier that 
	   was proposed by the association-initiator. */
#define AP_CLG_CONN_ID		AP_CLG_CONN_ID
	AP_COPYENV,
	/**< The AP_COPYENV attribute is used to indicate whether certain environment attributes that 
	   correspond to parameters on the A-ASSOCIATE indication and confirmation services should
	   be returned to the user in the cdata argument of the ap_rcv( ) function. When the value
	   of this attribute is TRUE, these attributes are returned via the cdata argument. */
#define AP_COPYENV		AP_COPYENV
	AP_CNTX_NAME,
	/**< The AP_CNTX_NAME attribute is the application-context-name parameter of the AARQ and
	   AARE APDUs. See the discussion of the ap_objid_t data type for information about how the 
	   absence of this parameter is indicated. This attribute is not used in the X.410-1984
	   mode. Refer to the description of the AP_MODE_SEL attribute. */
#define AP_CNTX_NAME		AP_CNTX_NAME
	AP_DCS,
	/**< The AP_DCS attribute is the defined context set. The defined context set contains the
	   abstract syntax/transfer syntax pairs that were negotiated when the association was
	   established, together with the presentation context identifier that identifies the pair.
	   The value of AP_DCS shall be empty (size field of 0) in the states AP_IDLE and
	   AP_WASSOCcnf_ASSOCreq, it shall contain the ACSE context negotiated by XAP in the state
	   AP_WASSOCrsp_ASSOCind, and it shall contain the fully negotiated defined context set in
	   all other states. Note that the defined context set contains both the presentation
	   contexts that were negotiated by the user (using AP_PCDL and AP_PCDRL) and any additional 
	   presentation contexts that were negotiated by XAP or provider. For example, the ACSE
	   context (i.e., {joint-iso-ccitt 2 1 0 1}) will be negotiated automatically and will
	   appear in the defined context set after negotiation is complete. This attribute is not
	   used in the X.410-1984 mode. Refer to the description of the AP_MODE_SEL attribute. */
#define AP_DCS			AP_DCS
	AP_DIAGNOSTIC,
	/**< The AP_DIAGNOSTIC attribute is used to pass diagnostic details on error conditions
	   reported by ACSE, Presentation, Session and Transport providers, in addition to that
	   conveyed with the A_PABORT_IND primitive. This mechanism is used to provide descriptive
	   text for all errors reported by XAP, and may be used to provide additional diagnostic
	   information (for example, errors in incoming APDUs reported by the local ACPM, or the
	   reason for a transport layer disconnect). It should be noted that the information
	   provided by this attribute is implementationdependent - that is, an implementation may
	   not report all of the source/reason/event combinations defined for this attribute.
	   However, if an implementation does use the attribute to report diagnostic information,
	   it must use the classifications defined in this specification. */
#define AP_DIAGNOSTIC		AP_DIAGNOSTIC
	AP_DPCN,
	/**< The AP_DPCN attribute is the default-context-name parameter of the CP PPDU. This
	   attribute is not used in the X.410-1984t- mode. Refer to the description of the
	   AP_MODE_SEL attribute. */
#define AP_DPCN			AP_DPCN
	AP_DPCR,
	/**< The AP_DPCR attribute is the default-context-result parameter of the CPR PPDU. Since the 
	   user is responsible for encoding and decoding of user-data, it is the user s
	   responsibility to accept or reject any proposed default-presentation-context. The value, 
	   AP_DPCR_NOVAL, indicates that it is not present. This attribute is not used in the
	   X.410-1984se mode. Refer to the description of the AP_MODE_SEL attribute. */
#define AP_DPCR			AP_DPCR
	AP_FLAGS,
	/**< The AP_FLAGS attribute is used to control the characteristics of this instance of XAP.
	   The currently defined flags are described below. If the AP_NDELAY bit is set, XAP will
	   be set to operate in non-blocking execution mode (that is, when the XAP instance is
	   opened with the AP_NDELAY flag set, the AP_FLAG attribute is set to AP_NDELAY.) */
#define AP_FLAGS		AP_FLAGS
	AP_INIT_SYNC_PT,
	/**< The AP_INIT_SYNC_PT attribute conveys the desired initial session synchronisation point
	   serial number. */
#define AP_INIT_SYNC_PT		AP_INIT_SYNC_PT
	AP_INIT_TOKENS,
	/**< The AP_INIT_TOKENS attribute conveys the specified initial assignment of tokens. The
	   values which may be set by the initialising user, and by the responding user, are
	   specified in Section 4.1.4 on page 60. */
#define AP_INIT_TOKENS		AP_INIT_TOKENS
	AP_LCL_PADDR,
	/**< The AP_LCL_PADDR attribute holds the value used as the presentation address of the local 
	   entity. If the local entity is the initiator of an association, this value will be used
	   as the calling presentation address. If the local entity is the association-responder,
	   this value will be used as the called presentation address when the A_ASSOC_IND
	   primitive is received and as the responding presentation address when the A_ASSOC_RSP
	   primitive is issued. This attribute is not directly settable by the user. However,
	   setting the AP_BIND_PADDR attribute will result in the AP_LCL_PADDR attribute being set
	   to the same value. Nevertheless, the attributes AP_BIND_PADDR and AP_LCL_PADDR do not
	   always have the same value. For example, a listener application might set the
	   AP_BIND_PADDR to a wildcard address and use the called address (conveyed by the
	   AP_LCL_PADDR attribute) specified in each indication primitive to dispatch associations. 
	 */
#define AP_LCL_PADDR		AP_LCL_PADDR
	AP_LIB_AVAIL,
	/**< The AP_LIB_AVAIL attribute indicates which versions of XAP are available to the user. */
#define AP_LIB_AVAIL		AP_LIB_AVAIL
	AP_LIB_SEL,
	/**< The AP_LIB_SEL attribute is used to indicate which version of XAP is used. This
	   attribute must be set before any other attributes are set or any primitives are issued. */
#define AP_LIB_SEL		AP_LIB_SEL
	AP_MODE_AVAIL,
	/**< The AP_MODE_AVAIL attribute indicates the available modes of operation for XAP and
	   Provider. The modes that may be supported are normal (AP_NORMAL_MODE) and X.410-1984
	   (AP_X410_MODE). */
#define AP_MODE_AVAIL		AP_MODE_AVAIL
	AP_MODE_SEL,
	/**< The AP_MODE_SEL attribute indicates the mode (normal or X.410-1984) in which XAP and
	   Provider are to be used. This value is used as the mode parameter of the CP PPDU. When
	   this attribute is set to AP_X410_MODE (indicating the X.410-1984 mode is in effect),
	   certain other attributes are not used. The readability and writability of these
	   attributes is not affected; but their values will not be used or updated by XAP. A list
	   of the attributes that are not used in the X.410-1984il mode is provided below.

	   Attributes Not Used in X.410-1984 Mode: AP_ACSE_AVAIL AP_CLD_APT AP_DPCN AP_PRES_SEL
	   AP_ACSE_SEL AP_CLG_AEID AP_DPCR AP_RSP_AEID AP_AFU_AVAIL AP_CLG_AEQ AP_PCDL AP_RSP_AEQ
	   AP_AFU_SEL AP_CLG_APID AP_PCDRL AP_RSP_APID AP_CLD_AEID AP_CLG_APT AP_PFU_AVAIL
	   AP_RSP_APT AP_CLD_AEQ AP_CNTX_NAME AP_PFU_SEL AP_CLD_APID AP_DCS AP_PRES_AVAIL */
#define AP_MODE_SEL		AP_MODE_SEL
	AP_MSTATE,
	/**< the XAP instance is awaiting additional data from the user (the last ap_snd( ) call had
	   the AP_MORE bit set), the AP_SNDMORE bit in AP_MSTATE is set. If there is more user data
	   for the current service (the last call to ap_rcv( ) returned with the AP_MORE bit set)
	   then AP_RCVMORE bit is set. (Note that it is possible for both bits to be set.) */
#define AP_MSTATE		AP_MSTATE
	AP_OLD_CONN_ID,
	/**< The AP_OLD_CONN_ID attribute conveys the values of the session connection identifiers
	   from a previous session. */
#define AP_OLD_CONN_ID		AP_OLD_CONN_ID
	AP_OPT_AVAIL,
	/**< The AP_OPT_AVAIL attribute is used to indicate what optional functionality is supported
	   in the underlying protocol implementation. AP_XXXX_WILD indicates whether address
	   wildcarding is supported at the specified level. (XXXX can be NSAP, TSEL, SSEL or PSEL.) 
	 */
#define AP_OPT_AVAIL		AP_OPT_AVAIL
#	define	AP_NSAP_WILD	(1<< 0)	/**< NSAP can be wildcarded. */
#	define	AP_TSEL_WILD	(1<< 1)	/**< TSEL can be wildcarded. */
#	define	AP_SSEL_WILD	(1<< 2)	/**< SSEL can be wildcarded. */
#	define	AP_PSEL_WILD	(1<< 3)	/**< PSEL can be wildcarded. */
	AP_PCDL,
	/**< The AP_PCDL attribute is used to propose the list of presentation contexts to be used on
	   the connection. The association-initiator sets this list to indicate the contexts it
	   wishes to use for encoding user data on the association. Since the user is responsible
	   for all encoding and decoding of user data, the proposed transfer syntaxes for each
	   proposed abstract syntax must be included in the list proposed by the
	   association-initiator user. For the association-responder, the list indicates which
	   presentation contexts are being requested by the association-initiator for use on the
	   association. The association-initiator may specify the ACSE context in AP_PCDL, although
	   it is not required to do so. If the association-initiator does not supply the ACSE
	   context in AP_PCDL, XAP supplies it automatically for the association. Regardless of
	   whether or not the association-initiator supplies the ACSE context, it is not provided to 
	   the associationresponder in AP_PCDL (the responding user may obtain it from AP_DCS if
	   required). This attribute is not used in the X.410-1984t mode. Refer to the description
	   of the AP_MODE_SEL attribute. */
#define AP_PCDL			AP_PCDL
	AP_PCDRL,
	/**< The AP_PCDRL attribute is used to indicate whether the presentation contexts proposed in
	   the presentation context definition list have been accepted or rejected. The
	   associationresponder uses this attribute to indicate which of the proposed presentation
	   contexts are acceptable before issuing the A_ASSOC_RSP primitive. For the
	   association-initiator, this attribute indicates the remote userOs response to the
	   proposed presentation contexts, received in the A_ASSOC_CNF primitive. Each entry in
	   AP_PCDRL corresponds one-to-one with an entry in AP_PCDL which, in the case of the
	   responder, never contains the proposed ACSE context. The associationresponder has an
	   opportunity to accept or reject each of the proposed contexts that are indicated in
	   AP_PCDL. The ACSE context is automatically accepted by XAP, and appears in the value of
	   AP_DCS. This attribute is not used in the X.410-1984 mode. Refer to the description of
	   the AP_MODE_SEL attribute. */
#define AP_PCDRL		AP_PCDRL
	AP_PFU_AVAIL,
	/**< The AP_PFU_AVAIL attribute indicates which optional Presentation functional units are
	   currently available. This attribute is not used in the X.410-1984 mode. Refer to the
	   description of the AP_MODE_SEL attribute. */
#define AP_PFU_AVAIL		AP_PFU_AVAIL
	AP_PFU_SEL,
	/**< The AP_PFU_SEL attribute indicates which optional Presentation Layer functional units
	   have been requested for use over the current association. This attribute is not used in
	   the X.410-1984 a mode. Refer to the description of the AP_MODE_SEL attribute. */
#define AP_PFU_SEL		AP_PFU_SEL
	AP_PRES_AVAIL,
	/**< The AP_PRES_AVAIL attribute indicates which versions of the Presentation Layer protocol
	   are currently available. This attribute is not used in the X.410-1984ti mode. */
#define AP_PRES_AVAIL		AP_PRES_AVAIL
	AP_PRES_SEL,
	/**< The AP_PRES_SEL attribute indicates which version of the Presentation Layer protocol has
	   been selected for use with the current association. This attribute is not used in the
	   X.410-1984ia mode. Refer to the description of the AP_MODE_SEL attribute. */
#define AP_PRES_SEL		AP_PRES_SEL
	AP_QLEN,
	/**< The AP_QLEN attribute is used to indicate the number of connection indications that will
	   be held by the provider when all of the connection endpoints bound for listening at
	   AP_BIND_PADDR are in use. The user can set this attribute to request a specific queue
	   size. The provider may change the value if it cannot support the requested queue size or
	   if another process bound to the same address has already requested a larger queue. The
	   user can determine the current queue size by examining the value of AP_QLEN using
	   ap_get_env( ). */
#define AP_QLEN			AP_QLEN
	AP_QOS,
	/**< The AP_QOS attribute is used to specify the quality of service requirements for the
	   association. It holds the ranges of QOS values that the XAP user is willing to accept. */
#define AP_QOS			AP_QOS
	AP_REM_PADDR,
	/**< The AP_REM_PADDR attribute holds the value used as the presentation address of the
	   remote entity. If the local entity is the initiator of an association, this value is the 
	   called presentation address. If the local entity is the association-responder, this
	   value is the calling presentation address. */
#define AP_REM_PADDR		AP_REM_PADDR
	AP_ROLE_ALLOWED,
	/**< The AP_ROLE_ALLOWED parameter is used to specify how an instance of XAP may be used.
	   Specifically, the value of this attribute indicates whether the XAP instance may be used 
	   to send an A_ASSOC_REQ primitive, receive an A_ASSOC_IND primitive, or both. Note that
	   this attribute only affects the roles that an instance of XAP may play with respect to
	   association establishment. Changing the value of this attribute will not affect the way
	   in which XAP participates in an association that has already been established. It should 
	   be noted that an A_ASSOC_IND primitive may be received even after this attribute has
	   been set to prohibit receipt of association indications if AP_ROLE_ALLOWED is set to
	   AP_INITIATOR while the XAP instance is in the AP_IDLE state. This situation occurs when
	   the A_ASSOC_IND primitive has been queued prior to setting AP_ROLE_ALLOWED to
	   AP_INITIATOR. To avoid this situation, the AP_ROLE_ALLOWED attribute should be set
	   before the XAP instance is bound to a presentation address if it will only be used to
	   initiate associations. */
#define AP_ROLE_ALLOWED		AP_ROLE_ALLOWED
#	define	AP_INITIATOR	(1<< 0)	/**< XAP instance in association-initiator role. */
#	define	AP_RESPONDER	(1<< 1)	/**< XAP instance in association-responder role. */
	AP_ROLE_CURRENT,
	/**< The AP_ROLE_CURRENT attribute indicates the role of the local user in the current
	   association. The attribute is set to AP_INITIATOR as soon as an A_ASSOC_REQ primitive is 
	   sent and remains unchanged until the association is rejected or subsequently terminated.
	   Similarly, the attribute is set to AP_RESPONDER upon receipt of an A_ASSOC_IND and left
	   unchanged until the association is terminated. */
#define AP_ROLE_CURRENT		AP_ROLE_CURRENT
	AP_RSP_AEID,
	/**< The AP_RSP_AEID is the responding AE invocation identifier parameter of the AARE APDU.
	   This attribute is not used in the X.410-1984 mode. Refer to the description of the
	   AP_MODE_SEL attribute. */
#define AP_RSP_AEID		AP_RSP_AEID
	AP_RSP_AEQ,
	/**< The AP_RSP_AEQ is the responding AE-qualifier parameter of the AARE APDU. See the
	   description of the ap_aeq_t data type for information about how its absence is
	   indicated. This attribute is not used in the X.410-1984 mode. Refer to the description
	   of the AP_MODE_SEL attribute. */
#define AP_RSP_AEQ		AP_RSP_AEQ
	AP_RSP_APID,
	/**< The AP_RSP_APID is the responding AP invocation identifier parameter of the AARE APDU.
	   This attribute is not used in the X.410-1984 mode. Refer to the description of the
	   AP_MODE_SEL attribute. */
#define AP_RSP_APID		AP_RSP_APID
	AP_RSP_APT,
	/* <**< The AP_RSP_APT is the responding AP-title parameter of the AARE APDU. This
	   attribute is not used in the X.410-1984ar mode. Refer to the description of the
	   AP_MODE_SEL attribute. */
#define AP_RSP_APT		AP_RSP_APT
	AP_SESS_AVAIL,
	/**< The AP_SESS_AVAIL attribute indicates which versions of the Session Layer protocol are
	   currently available. */
#define AP_SESS_AVAIL		AP_SESS_AVAIL
#	define	AP_SESSVER1	(1<<0)		/**< Session Protocol Version 1 */
#	define	AP_SESSVER2	(1<<1)		/**< Session Protocol Version 2 */
	AP_SESS_SEL,
	/**< The AP_SESS_SEL attribute indicates which version of the Session Layer protocol has been
	   selected for use with the current association. */
#define AP_SESS_SEL		AP_SESS_SEL
	AP_SESS_OPT_AVAIL,
	/**< The AP_SESS_OPT_AVAIL attribute is used to indicate which optional session capabilities
	   are available through the XAP interface. If the AP_UCBC flag is set, the underlying
	   provider supports user control of basic concatenation. If the AP_UCEC flag is set, the
	   underlying provider supports user control of extended concatenation. For further
	   information, refer to ap_snd( ) and the Session Layer protocol specification. */
#define AP_SESS_OPT_AVAIL	AP_SESS_OPT_AVAIL
#	define	AP_UCBC		(1<<0)		/**< User control of basic concatentation. */
#	define	AP_UCEC		(1<<0)		/**< User control of extended concatentation. */
	AP_SFU_AVAIL,
	/**< The AP_SFU_AVAIL attribute indicates which Session Layer functional units are currently
	   available. The value of this attribute may affect the value of AP_TOKENS_AVAIL. */
#define AP_SFU_AVAIL		AP_SFU_AVAIL
	AP_SFU_SEL,
	/**< The AP_SFU_SEL attribute indicates which Session Layer functional units have been
	   requested for use over the current association. */
#define AP_SFU_SEL		AP_SFU_SEL
#	define	AP_SESS_HALFDUPLEX	(1<< 0)	/**< Session Half Duplex FU */
#	define	AP_SESS_DUPLEX		(1<< 1)	/**< Session Duplex FU */
#	define	AP_SESS_XDATA		(1<< 2)	/**< Session Expedited Data FU */
#	define	AP_SESS_MINORSYNC	(1<< 3)	/**< Session Minor Syncrhonization FU */
#	define	AP_SESS_MAJORSYNC	(1<< 4)	/**< Session Major Syncrhonization FU */
#	define	AP_SESS_RESYNC		(1<< 5)	/**< Session Symmetric Resynchronization FU */
#	define	AP_SESS_ACTMGMT		(1<< 6)	/**< Session Actvitiy Management FU */
#	define	AP_SESS_NEGREL		(1<< 7)	/**< Session Negotiated Release FU */
#	define	AP_SESS_CDATA		(1<< 8)	/**< Session Capability Data FU */
#	define	AP_SESS_EXCEPT		(1<< 9)	/**< Session Exceptions FU */
#	define	AP_SESS_TDATA		(1<<10)	/**< Session Typed Data FU */
#	define	AP_SESS_DATASEP		(1<<11)	/**< Session Data Separation FU */
#	define	AP_SESS_NOORDREL	(1<<12)	/**< Session No Orderly Release FU */
#	define	AP_SESS_NESTCON		(1<<13)	/**< Session Nested Connections FU */
	AP_STATE,
	/**< The AP_STATE attribute is used to convey state information about the XAP interface. It
	   is used to determine which primitives are legal, which attributes can be read/written,
	   etc. */
#define AP_STATE		AP_STATE
	AP_TOKENS_AVAIL,
	/**< A token is an attribute of an association which is dynamically assigned to one user at a
	   time. The user that possesses a token has exclusive rights to initiate the service which
	   that token represents. The AP_TOKENS_AVAIL attribute indicates which tokens are available
	   for assignment on this association. The value of this attribute is dependent on
	   AP_SFU_SEL. */
#define AP_TOKENS_AVAIL		AP_TOKENS_AVAIL
#	define	AP_DATA_TOK		(1<< 0)	/**< Data Token */
#	define	AP_SYNCMINOR_TOK	(1<< 1)	/**< Minor Synchronization Token */
#	define	AP_MAJACT_TOK		(1<< 2)	/**< Major Activity Token */
#	define	AP_RELEASE_TOK		(1<< 3)	/**< Release Token */
	AP_TOKENS_OWNED,
	/**< The AP_TOKENS_OWNED attribute indicates which available tokens (see AP_TOKENS_AVAIL) are 
	   currently assigned to the user. The user has exclusive rights to initiate the service
	   represented by each of the tokens owned. The value of this attribute is affected by
	   AP_INIT_TOKENS. */
#define AP_TOKENS_OWNED		AP_TOKENS_OWNED
};

#endif				/* __XAP_ENV_H__ */
