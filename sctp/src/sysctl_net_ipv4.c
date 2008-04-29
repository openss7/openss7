/*****************************************************************************

 @(#) $RCSfile: sysctl_net_ipv4.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-04-29 08:49:53 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

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

 Last Modified $Date: 2008-04-29 08:49:53 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sysctl_net_ipv4.c,v $
 Revision 0.9.2.2  2008-04-29 08:49:53  brian
 - updated headers for Affero release

 *****************************************************************************/

#ident "@(#) $RCSfile: sysctl_net_ipv4.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-04-29 08:49:53 $"

static char const ident[] = "$RCSfile: sysctl_net_ipv4.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-04-29 08:49:53 $";

	{NET_SCTP_RTO_INITIAL, "sctp_rto_initial",
	 &sysctl_sctp_rto_initial, sizeof(int), 0644, NULL,
	 &proc_doulongvec_ms_jiffies_minmax, &sysctl_intvec, NULL,
	 &sysctl_sctp_rto_min, &sysctl_sctp_rto_max},
	{NET_SCTP_RTO_MIN, "sctp_rto_min",
	 &sysctl_sctp_rto_min, sizeof(int), 0644, NULL,
	 &proc_doulongvec_ms_jiffies_minmax, &sysctl_intvec, NULL,
	 NULL, &sysctl_sctp_rto_max},
	{NET_SCTP_RTO_MAX, "sctp_rto_max",
	 &sysctl_sctp_rto_max, sizeof(int), 0644, NULL,
	 &proc_doulongvec_ms_jiffies_minmax, &sysctl_intvec, NULL,
	 &sysctl_sctp_rto_min, NULL},
	{NET_SCTP_HEARTBEAT_ITVL, "sctp_heartbeat_itvl",
	 &sysctl_sctp_heartbeat_itvl, sizeof(int), 0644, NULL,
	 &proc_dointvec_jiffies, &sysctl_jiffies},
	{NET_SCTP_INIT_RETRIES, "sctp_init_retries",
	 &sysctl_sctp_max_init_retries, sizeof(int), 0644, NULL, &proc_dointvec},
	{NET_SCTP_VALID_COOKIE_LIFE, "sctp_valid_cookie_life",
	 &sysctl_sctp_valid_cookie_life, sizeof(int), 0644, NULL,
	 &proc_doulongvec_ms_jiffies_minmax, &sysctl_intvec, NULL,
	 NULL, NULL},
	{NET_SCTP_MAX_SACK_DELAY, "sctp_max_sack_delay",
	 &sysctl_sctp_max_sack_delay, sizeof(int), 0644, NULL,
	 &proc_doulongvec_ms_jiffies_minmax, &sysctl_intvec, NULL,
	 NULL, NULL},
	{NET_SCTP_PATH_MAX_RETRANS, "sctp_path_max_retrans",
	 &sysctl_sctp_path_max_retrans, sizeof(int), 0644, NULL, &proc_dointvec},
	{NET_SCTP_ASSOC_MAX_RETRANS, "sctp_assoc_max_retrans",
	 &sysctl_sctp_assoc_max_retrans, sizeof(int), 0644, NULL, &proc_dointvec},
	{NET_SCTP_MAC_TYPE, "sctp_mac_type",
	 &sysctl_sctp_mac_type, sizeof(int), 0644, NULL, &proc_dointvec},
	{NET_SCTP_CSUM_TYPE, "sctp_csum_type",
	 &sysctl_sctp_csum_type, sizeof(int), 0644, NULL, &proc_dointvec},
	{NET_SCTP_COOKIE_INC, "sctp_cookie_inc",
	 &sysctl_sctp_cookie_inc, sizeof(int), 0644, NULL,
	 &proc_doulongvec_ms_jiffies_minmax, &sysctl_intvec, NULL,
	 NULL, NULL},
	{NET_SCTP_THROTTLE_ITVL, "sctp_throttle_itvl",
	 &sysctl_sctp_throttle_itvl, sizeof(int), 0644, NULL,
	 &proc_doulongvec_ms_jiffies_minmax},
	{NET_SCTP_MEM, "sctp_mem",
	 &sysctl_sctp_mem, sizeof(sysctl_sctp_mem), 0644, NULL, &proc_dointvec},
	{NET_SCTP_WMEM, "sctp_wmem",
	 &sysctl_sctp_wmem, sizeof(sysctl_sctp_wmem), 0644, NULL, &proc_dointvec},
	{NET_SCTP_RMEM, "sctp_rmem",
	 &sysctl_sctp_rmem, sizeof(sysctl_sctp_rmem), 0644, NULL, &proc_dointvec},
	{NET_SCTP_MAX_ISTREAMS, "sctp_max_istreams",
	 &sysctl_sctp_max_istreams, sizeof(int), 0644, NULL, &proc_dointvec_minmax,
	 &sysctl_intvec, NULL, &min_sctp_max_istreams, NULL },
	{NET_SCTP_REQ_OSTREAMS, "sctp_req_ostreams",
	 &sysctl_sctp_req_ostreams, sizeof(int), 0644, NULL, &proc_dointvec_minmax,
	 &sysctl_intvec, NULL, &min_sctp_req_ostreams, NULL },
	{NET_SCTP_ECN, "sctp_ecn",
	 &sysctl_sctp_ecn, sizeof(int), 0644, NULL, &proc_dointvec},
	{NET_SCTP_ADAPTATION_LAYER_INFO, "sctp_adaptation_layer_info",
	 &sysctl_sctp_adaptation_layer_info, sizeof(int), 0644, NULL, &proc_dointvec},
	{NET_SCTP_PARTIAL_RELIABILITY, "sctp_partial_reliability",
	 &sysctl_sctp_partial_reliability, sizeof(int), 0644, NULL, &proc_dointvec},
	{NET_SCTP_MAX_BURST, "sctp_max_burst",
	 &sysctl_sctp_max_burst, sizeof(int), 0644, NULL, &proc_dointvec_minmax,
	 &sysctl_intvec, NULL, &min_sctp_max_burst, NULL},
