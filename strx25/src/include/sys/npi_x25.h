/*****************************************************************************

 @(#) $Id: npi_x25.h,v 0.9.2.1 2008-05-07 16:01:40 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2008-05-07 16:01:40 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: npi_x25.h,v $
 Revision 0.9.2.1  2008-05-07 16:01:40  brian
 - added NLI X.25-PLP CONS and XX25 implemetnation

 *****************************************************************************/

#ifndef __SYS_NPI_X25_H__
#define __SYS_NPI_X25_H__

#ident "@(#) $RCSfile: npi_x25.h,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/* Additional definitions for NPI for use with X.25. */

/*
   NPI Q-bit flag for use with DATA_xfer_flags field of N_DATA_REQ and N_DATA_IND.
 */
#define N_Q_FLAG		0x00010000L	/* Indicates qualified data. */

/*
   Additional N_ERROR_ACK error return code values.
 */
#define NNOINTCF		32	/* Previous expedited data transfer not yet complete. */
					/* Indicates that the NS user sent the N_EXDATA_REQ
					   primitive to the NS provider before the user's previous
					   N_EXDATA_REQ primitive completed. Each expedited data
					   sequence must complete before another one may be
					   requested by the same user. */
#define NODDCUD			33	/* Indicates an odd-length call user data string. */

/*
   X.25 Facilities
 */
typedef struct {
	np_long loc_fac_value;		/* local facility value */
	np_long rem_fac_value;		/* remote facility value */
} fac_values_t;

/*
   X.25 Facilities strings
 */

/*
   Network User Identification
 */
#define NP_MAX_NUI	64
typedef struct {
	np_ulong nui_len;		/* octet length of the nui_field */
	uint8_t nui_field[64];
} nui_string_t;

/*
   Registered Private Operator Administration
 */
#define NP_MAX_RPOA	8
typedef struct {
	np_ulong rpoa_len;		/* octet length of the rpoa_field */
	uint8_t rpoa_field[NP_MAX_RPOA];
} rpoa_string_t;

/*
   Closed User Group
 */
#define NP_MAX_CUG	2
typedef struct {
	np_ulong cug_len;		/* octet length of the cug_field */
	uint8_t cug_field[NP_MAX_CUG];
} cug_string_t;

/*
   Charging call duration.
 */
#define NP_MAX_TARRIFS	4
#define NP_MAX_CD	NP_MAX_TARRIFS * 4
typedef struct {
	np_ulong cd_len;		/* octet lengthof the cd_field */
	uint8_t cd_field[NP_MAX_CD];
} chg_cd_string_t;

/*
 * Charging 
 */
#define NP_MAX_SC	NP_MAX_TARRIFS * 8
typedef struct {
	np_ulong sc_len;		/* octet length of the sc_field */
	uint8_t sc_field[NP_MAX_SC];
} chg_sc_string_t;

/*
   Charging monetary unit.
 */
#define NP_MAX_MU	16
typedef struct {
	np_ulong mu_len;		/* octet length of the mu_field */
	uint8_t mu_field[NP_MAX_MU];
} chg_mu_string_t;

/*
   Non-X25 Facilities
 */
#define NP_MAX_FAC	32
typedef struct {
	np_ulong x_fac_len;		/* X.25 facilities octet length */
	np_ulong cq_fac_len;		/* calling non-X.25 facilities octet length */
	np_ulong cd_fac_len;		/* called non-X.25 facilities octet length */
	uint8_t fac_field[NP_MAX_FAC];	/* raw facilities */
} fac_string_t;

#define N_QOS_X25_RANGE		0x0100
#define N_QOS_X25_SEL		0x0101
#define N_QOS_X25_OPT_RANGE	0x0102
#define N_QOS_X25_OPT_SEL	0x0103

/*
 * QOS range for X.25.  (Used with N_CONN_REQ and N_CONN_IND.)
 */
typedef struct {
	np_ulong n_qos_type;		/* always N_QOS_X25_RANGE */
	thru_values_t src_throughput_range;	/* source throughput range */
	thru_values_t dest_throughput_range;	/* destination throughput range */
	td_values_t transit_delay_range;	/* transit delay range */
	protection_values_t protection_range;	/* protection range */
	priority_values_t priority_range;	/* priority range */
	/* */
	fac_values_t packet_size;	/* packet size range */
	fac_values_t window_size;	/* window size range */
	np_long nsdulimit;
} N_qos_x25_range_t;

/*
 * QOS selected for X.25.  (Used with N_CONN_RES and N_CONN_CON.)
 */
typedef struct {
	np_ulong n_qos_type;		/* always N_QOS_X25_SEL */
	np_long src_throughput_sel;	/* source throughput selected */
	np_long dest_throughput_sel;	/* destination throughput selected */
	np_long transit_delay_sel;	/* transit delay selected */
	np_long protection_sel;		/* NC protection selected */
	np_long priority_sel;		/* NC priority selected */
	/* */
	np_long packet_size;		/* packet size */
	np_long window_size;		/* window size */
	np_long nsdulimit;
} N_qos_x25_sel_t;

/*
 * QOS range for X.25 options management.  (Used with N_OPTMGMT_REQ and N_INFO_ACK.)
 */
typedef struct {
	np_ulong n_qos_type;		/* always N_QOS_X25_OPT_RANGE */
	thru_values_t src_throughput;	/* source throughput values */
	thru_values_t dest_throughput;	/* destination throughput values */
	td_values_t transit_delay_t;	/* transit delay values */
	np_long nc_estab_delay;		/* NC establishment delay */
	np_ulong nc_estab_fail_prob;	/* NC establishment failure probability */
	np_ulong residual_error_rate;	/* residual error rate */
	np_ulong xfer_fail_prob;	/* transfer failure probability */
	np_ulong nc_resilience;		/* NC resilience */
	np_long nc_rel_delay;		/* NC release delay */
	np_ulong nc_rel_fail_prob;	/* NC release failure probability */
	protection_values_t protection_range;	/* protection range */
	priority_values_t priority_range;	/* priority range */
	np_long max_accept_cost;	/* maximum acceptable cost */
	/* */
	np_long packet_size;		/* packet size range */
	np_long window_size;		/* window size range */
	np_long nsdulimit;
	np_ulong nui_range;
	np_ulong rpoa_range;
	np_ulong cug_range;
	np_ulong chg_cd_range;
	np_ulong chg_sc_range;
	np_ulong chg_mu_range;
} N_qos_x25_opt_range_t;

/*
 * QOS selection for X.25 options management.  (Used with N_OPTMGMT_REQ and N_INFO_ACK.)
 */
typedef struct {
	np_ulong n_qos_type;		/* always N_QOS_X25_OPT_SEL */
	thru_values_t src_throughput;	/* source throughput values */
	thru_values_t dest_throughput;	/* destination throughput values */
	td_values_t transit_delay_t;	/* transit delay values */
	np_long nc_estab_delay;		/* NC establishment delay */
	np_ulong nc_estab_fail_prob;	/* NC establishment failure probability */
	np_ulong residual_error_rate;	/* residual error rate */
	np_ulong xfer_fail_prob;	/* trhansfer failure probability */
	np_ulong nc_resilience;		/* NC resilience */
	np_long nc_rel_delay;		/* NC release delay */
	np_ulong nc_rel_fail_prob;	/* NC release failure probability */
	np_long protection_sel;		/* protection selected */
	np_long priority_sel;		/* priority selected */
	np_long max_accept_cost;	/* maximum acceptable cost */
	/* */
	fac_values_t packet_size;	/* packet size */
	fac_values_t window_size;	/* window size */
	np_long nsdulimit;
	nui_string_t nui;
	rpoa_string_t rpoa;
	cug_string_t cug;
	chg_cd_string_t chd_cd;
	chg_sc_string_t chg_sc;
	chg_mu_string_t chg_mu;
	fac_string_t facilites;		/* raw non-X.25 facilities */
} N_qos_x25_opt_sel_t;

#endif				/* __SYS_NPI_X25_H__ */
