/*****************************************************************************

 @(#) $RCSfile: ldltest.c,v $ $Name:  $($Revision: 1.1.1.2.4.2 $) $Date: 2003/12/15 23:35:06 $

 -----------------------------------------------------------------------------

 Copyright (c) 2003-2004  OpenSS7 Corporation <http://www.openss7.com>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2003/12/15 23:35:06 $ by $Author: brian $

 -----------------------------------------------------------------------------
 $Log: ldltest.c,v $
 Revision 1.1.1.2.4.2  2003/12/15 23:35:06  brian
 - Tried to reduce number of patch lines.

 Revision 1.1.1.2.4.1  2003/12/10 11:09:29  brian
 Start of autoconf changes.

 *****************************************************************************/

#ident "@(#) $RCSfile: ldltest.c,v $ $Name:  $($Revision: 1.1.1.2.4.2 $) $Date: 2003/12/15 23:35:06 $"

static char const ident[] =
    "$RCSfile: ldltest.c,v $ $Name:  $($Revision: 1.1.1.2.4.2 $) $Date: 2003/12/15 23:35:06 $";

/*
 *  ldltest: Test program for dlpi driver
 *
 *  Copyright (C) 1998 Ole Husgaard (sparre@login.dknet.dk)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *
 *  Notes:
 *    1) The PING will only work if the target is local or if an
 *       intermediary router does proxy ARP for our target.
 *    2) Under Linux 2.2, the native IP stack will NOT respond to a ping
 *       on interface loopback from this utility. This is due to some
 *       optimizations in the native IP implementation. Use another instance
 *       of this utility to do replies.
 *    3) This source is ugly.
 */

#ident "@(#) LiS ldltest.c 2.13 9/9/03"

#ifndef LINUX
#define LINUX 2
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#else
#include <linux/unistd.h>	/* To get around a glibc problem with old kernels. */
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#include <arpa/inet.h>

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#include <sys/stropts.h>
#include <sys/dlpi.h>
#include <sys/ldl.h>

#if !defined(_I386_TYPES_H) && !defined(_ASM_IA64_TYPES_H)
typedef unsigned char __u8;
typedef unsigned short __u16;
typedef unsigned int __u32;
#endif

#define ARPHRD_ETHER	1
#define ARPHRD_IEEE802	6

#define ARPOP_REQUEST	1
#define ARPOP_REPLY	2

#define ETH_P_IP	0x0800
#define ETH_P_ARP	0x0806
#define	ETH_P_RAW	0x00FF

#define __LITTLE_ENDIAN_BITFIELD

/*
 *  Ethernet header structure, snipped from <linux/if_ether.h>
 */
#define ETH_ALEN 6		/* Octets in one ethernet addr */

struct ethhdr {
      unsigned char   h_dest[ETH_ALEN];	/* destination eth addr */
      unsigned char   h_source[ETH_ALEN];	/* source ether addr */
      unsigned short  h_proto;	/* packet type ID field */
};

struct eth_llc_hdr {		/* 802.2 header */
      unsigned char   h_dest[ETH_ALEN];	/* destination eth addr */
      unsigned char   h_source[ETH_ALEN];	/* source ether addr */
      unsigned char   h_len_hi;	/* length indicator */
      unsigned char   h_len_lo;	/* length indicator */
      unsigned char   llc_dsap;	/* LLC header */
      unsigned char   llc_ssap;
      unsigned char   llc_cmnd;
};

/*
 * Token ring header, snipped from ldl.c
 *
 * Note that there is no length field as there is in an Ethernet
 * header.  IP packet types have to be encoded inside a SNAP header
 * beyond the LLC header.
 */
#define	TR_ALEN		6
typedef struct tr_hdr {
      unsigned char   acf;
      unsigned char   fcf;
      unsigned char   dst_addr[TR_ALEN];
      unsigned char   src_addr[TR_ALEN];
      /*
       * Routing control bytes: Only present if the routing bit
       * is set in the first byte of the src_addr.
       */
      unsigned char   bl;	/* brdcst & lgth */
      unsigned char   df;	/* direction & lrgst frm bits */

} tr_hdr_t;

/*
 * After a variable amount of routing data you come to the
 * LLC header.
 */
typedef struct tr_llc_frm_hdr {
      unsigned char   llc_dsap;	/* destination SAP address */
      unsigned char   llc_ssap;	/* source SAP address */
      unsigned char   llc_ctl[1];	/* control field */

} tr_llc_frm_hdr_t;

/*
 * A few bits from the token ring header.
 */
#define SADDR_0_RTE_PRES        0x80	/* 1=rte info present, 0=none */
#define RCF_0_LLLLL             0x1F	/* length bits */
#define FCF_FF          	0xC0	/* frame type bits */
#define     FCF_MAC     	0x00	/* MAC frame */
#define     FCF_LLC     	0x40	/* LLC frame */

/*
 *  IP header structure, snipped from <linux/ip.h>
 */
struct iphdr {
#if defined(__LITTLE_ENDIAN_BITFIELD)
      __u8            ihl:4,
                      version:4;
#elif defined (__BIG_ENDIAN_BITFIELD)
      __u8            version:4,
                      ihl:4;
#else
#error  "Please fix byteorder"
#endif
      __u8            tos;
      __u16           tot_len;
      __u16           id;
      __u16           frag_off;
      __u8            ttl;
      __u8            protocol;
      __u16           check;
      __u32           saddr;
      __u32           daddr;
      /*
       * The options start here. 
       */
};

/*
 *  ICMP header structure, snipped from <linux/icmp.h>
 */
struct icmphdr {
      __u8            type;
      __u8            code;
      __u16           checksum;
      union {
	    struct {
		  __u16           id;
		  __u16           sequence;
	    } echo;
	    __u32           gateway;
      } un;
};

#define ICMP_ECHOREPLY          0	/* Echo Reply */
#define ICMP_ECHO               8	/* Echo Request */

/*
 *  Ethernet ARP header structure, snipped from <linux/if_arp.h>
 */
struct arphdr {
      unsigned short  ar_hrd;	/* format of hardware address */
      unsigned short  ar_pro;	/* format of protocol address */
      unsigned char   ar_hln;	/* length of hardware address */
      unsigned char   ar_pln;	/* length of protocol address */
      unsigned short  ar_op;	/* ARP opcode (command) */
#if 0
      /*
       *  Ethernet looks like this : This bit is variable sized however...
       */
      unsigned char   ar_sha[ETH_ALEN];	/* sender hardware address */
      unsigned char   ar_sip[4];	/* sender IP address */
      unsigned char   ar_tha[ETH_ALEN];	/* target hardware address */
      unsigned char   ar_tip[4];	/* target IP address */
#endif
};

/*
 *  Our ICMP PING packet structure
 */
struct ping_pkt {
      struct iphdr    ip_hdr;
      struct icmphdr  icmp_hdr;
      struct timeval  tv;
      char            filler[48];
};

/*
 *  Variables specified at the command line
 */
int             verbose;	/* verbose level */
int             arp_respond;	/* respond to ARP queries */
int             echo_respond;	/* respond to ICMP PING packets */
unsigned long   pkt_count;	/* Number of packets to send */
struct in_addr  my_ip_addr,
                target_ip_addr;	/* Local and remote IP number */
char           *interface;	/* Interface name */
unsigned long   framing;	/* Link encapsulation type */
int             test_ping;	/* send an LLC TEST cmnd instead of IP pkt */
int             xid_ping;	/* send an LLC XID cmnd instead of IP pkt */
int             test_xid;	/* test_ping || xid_ping */
int             saps_set;	/* -S present */

#define MAX_DL_ADDR_LEN 128
#define MAX_DL_SAP_LEN 64

/*
 *  Our MAC addresses
 */
int             addr_len;	/* Data Link address length */
int             sap_len;	/* Data Link SAP length */
int             sap_first;	/* Flag: SAP first in DLSAP */
int             hw_type;	/* ARP hardware type */
int             mac_type;	/* DLPI MAC type from info ack */

unsigned char   my_addr[MAX_DL_ADDR_LEN];
unsigned char   my_brd_addr[MAX_DL_ADDR_LEN];
unsigned char   my_dlsap[MAX_DL_ADDR_LEN + MAX_DL_SAP_LEN];
unsigned char   my_sap[MAX_DL_SAP_LEN];
unsigned char   my_llc_sap[MAX_DL_SAP_LEN];
volatile int    got_target_addr;
unsigned char   target_addr[MAX_DL_ADDR_LEN];
unsigned char   target_dlsap[MAX_DL_ADDR_LEN + MAX_DL_SAP_LEN];
unsigned char   target_sap[MAX_DL_SAP_LEN];

/*
 * Procedure forward declarations
 */
extern int      do_promiscon(int fd, unsigned long level);
extern int      send_test_xid(int fd, int cmnd);

/*
 *  ldl flags
 */
unsigned long   ldl_flags = LDLFLAG_DEFAULT;

/**************************************************************/
/*
 */
/*
 * Utilities for dumping data 
 */
/*
 */
/**************************************************************/

char           *hex(unsigned char c)
{
      static char     s[3];
      static char     h[16] = "0123456789abcdef";

      s[0] = h[c >> 4];
      s[1] = h[c & 15];
      s[2] = '\0';

      return s;
}

void dumpbytes(char *prompt, unsigned char *data, int len)
{
      if (!verbose)
	    return;
      printf("%s: ", prompt);
      while (len--) {
	    printf("%s%s", hex(*data), (len) ? ":" : "");
	    ++data;
      }
      printf("\n");
}

void dumpbuf(struct strbuf buf)
{
      int             i,
                      j;
      int             lines = (buf.len == 0) ? 0 : ((buf.len - 1) >> 4) + 1;
      char            s1[128],
                      s2[128];

      if (!verbose)
	    return;
      printf("  Len = %d  MaxLen = %d\n", buf.len, buf.maxlen);
      for (i = 0; i < lines; ++i) {
	    strcpy(s1, "  ");
	    strcpy(s2, "  ");
	    for (j = 0; j < 16; ++j) {
		  if (16 * i + j < buf.len) {
			unsigned char   c = buf.buf[16 * i + j];

			strcat(s1, hex(c));
			strcat(s1, " ");
			if (isprint(c)) {
			      char            s3[2];

			      s3[0] = c;
			      s3[1] = '\0';
			      strcat(s2, s3);
			}
			else
			      strcat(s2, ".");

		  }
		  else {
			strcat(s1, "   ");
			strcat(s2, " ");
		  }
		  if (j == 7) {
			strcat(s1, " ");
			strcat(s2, " ");
		  }
	    }
	    printf("  %s  %s\n", s1, s2);
      }
}

void dumpaddr(char *prompt, unsigned char *addr, int addrlen)
{
      if (!verbose)
	    return;
      printf("%s: ", prompt);
      if (addrlen) {
	    printf("%02x", (int) *addr++);
	    --addrlen;
      }
      else {
	    printf("(none)\n");
	    return;
      }
      while (addrlen--)
	    printf(":%02x", (int) *addr++);
      printf("\n");
}

/**************************************************************/
/*
 */
/*
 * Utilities for dlsap sap/addr ordering abstraction 
 */
/*
 */
/**************************************************************/

/*
 *  Return pointer to address part of dlsap
 */
char           *dlsap_addr(char *dlsap)
{
      return (sap_first) ? dlsap + sap_len : dlsap;
}

/*
 *  Return pointer to sap part of dlsap
 */
char           *dlsap_sap(char *dlsap)
{
      return (sap_first) ? dlsap : dlsap + addr_len;
}

/*
 *  Build dlsap from addr and sap
 */
void build_dlsap(char *dlsap, char *addr, char *sap)
{
      memcpy(dlsap_addr(dlsap), addr, addr_len);
      memcpy(dlsap_sap(dlsap), sap, sap_len);
}

/**************************************************************/
/*
 */
/*
 * IP checksum calculation 
 */
/*
 */
/*
 * From: W.R.Stewens: UNIX network programming, 
 */
/*
 * Volume 1, 2nd. edition 
 */
/*
 */
/**************************************************************/

unsigned short in_cksum(void *addr, int len)
{
      int             nleft = len;
      int             sum = 0;
      unsigned short *w = addr;
      unsigned short  answer = 0;

      while (nleft > 1) {
	    sum += *w++;
	    nleft -= 2;
      }

      if (nleft == 1) {
	    *(unsigned char *) &answer = *(unsigned char *) w;
	    sum += answer;
      }

      sum = (sum >> 16) + (sum & 0xffff);
      sum += (sum >> 16);
      answer = ~sum;
      return answer;
}

/**************************************************************/
/*
 */
/*
 * The actual dlpi test routines 
 */
/*
 */
/**************************************************************/

unsigned long do_findppa(int fd, char *interface)
{
      union {
	    char            interface[256];
	    unsigned long   ppa;
      } data;
      struct strioctl strioctl;

      strioctl.ic_cmd = LDL_FINDPPA;
      strioctl.ic_timout = 3;
      strioctl.ic_len = sizeof(data);
      strioctl.ic_dp = (char *) &data;
      strcpy(data.interface, interface);

      if (ioctl(fd, I_STR, &strioctl) < 0) {
	    perror("do_findppa: ioctl()");
	    return (unsigned long) -1;
      }
      if (verbose > 2)
	    printf("do_findppa: Interface \"%s\" has PPA %ld\n", interface, data.ppa);

      return data.ppa;
}

char           *do_getname(int fd)
{
      static char     name[256];
      struct strioctl strioctl;

      strioctl.ic_cmd = LDL_GETNAME;
      strioctl.ic_timout = 3;
      strioctl.ic_len = sizeof(name);
      strioctl.ic_dp = (char *) &name;

      if (ioctl(fd, I_STR, &strioctl) < 0) {
	    perror("do_getname: ioctl()");
	    return NULL;
      }
      if (verbose > 2)
	    printf("do_getname: Interface name is \"%s\"\n", name);

      return name;
}

void do_global_stats(void)
{
      int             fd;
      struct strioctl strioctl;
      ldl_gstats_ioctl_t st;

      if ((fd = open("/dev/ldl", O_RDWR)) < 0) {
	    perror("open(\"/dev/ldl\")");
	    exit(1);
      }

      strioctl.ic_cmd = LDL_GETGSTATS;
      strioctl.ic_timout = 3;
      strioctl.ic_len = sizeof(st);
      strioctl.ic_dp = (char *) &st;

      if (ioctl(fd, I_STR, &strioctl) < 0) {
	    perror("do_global_stats: ioctl()");
	    exit(1);
      }

      if (!verbose)
	    return;
      printf("attach_req_cnt      = %lu\n", st.attach_req_cnt);
      printf("detach_req_cnt      = %lu\n", st.detach_req_cnt);
      printf("bind_req_cnt        = %lu\n", st.bind_req_cnt);
      printf("unbind_req_cnt      = %lu\n", st.unbind_req_cnt);
      printf("subs_bind_req_cnt   = %lu\n", st.subs_bind_req_cnt);
      printf("subs_unbind_req_cnt = %lu\n", st.subs_unbind_req_cnt);
      printf("udqos_req_cnt       = %lu\n", st.udqos_req_cnt);
      printf("ok_ack_cnt          = %lu\n", st.ok_ack_cnt);
      printf("error_ack_cnt       = %lu\n", st.error_ack_cnt);
      printf("unitdata_req_cnt    = %lu\n", st.unitdata_req_cnt);
      printf("unitdata_req_q_cnt  = %lu\n", st.unitdata_req_q_cnt);
      printf("unitdata_ind_cnt    = %lu\n", st.unitdata_ind_cnt);
      printf("unitdata_q_cnt      = %lu\n", st.unitdata_q_cnt);
      printf("unitdata_drp_cnt    = %lu\n", st.unitdata_drp_cnt);
      printf("uderror_ind_cnt     = %lu\n", st.uderror_ind_cnt);
      printf("ioctl_cnt           = %lu\n", st.ioctl_cnt);
      printf("net_rx_cnt          = %lu\n", st.net_rx_cnt);
      printf("net_rx_drp_cnt      = %lu\n", st.net_rx_drp_cnt);
      printf("net_tx_cnt          = %lu\n", st.net_tx_cnt);
      printf("net_tx_fail_cnt     = %lu\n", st.net_tx_fail_cnt);
}

void do_set_debug_mask(unsigned long msk)
{
      int             fd;
      struct strioctl strioctl;

      if ((fd = open("/dev/ldl", O_RDWR)) < 0) {
	    perror("open(\"/dev/ldl\")");
	    exit(1);
      }

      strioctl.ic_cmd = LDL_SETDEBUG;
      strioctl.ic_timout = 3;
      strioctl.ic_len = sizeof(msk);
      strioctl.ic_dp = (char *) &msk;

      if (ioctl(fd, I_STR, &strioctl) < 0) {
	    perror("do_set_debug_mask: ioctl()");
	    exit(1);
      }

}

int do_info(int fd)
{
      dl_info_req_t   request;
      struct {
	    dl_info_ack_t   ack;
	    unsigned char   extra[1024];
      } reply;
      int             flags;
      struct strbuf   ctlbuf;
      dl_error_ack_t *err_ack;

      if (verbose > 2)
	    printf("do_info: Sending DL_INFO_REQ\n");

      request.dl_primitive = DL_INFO_REQ;
      ctlbuf.maxlen = ctlbuf.len = sizeof(request);
      ctlbuf.buf = (char *) &request;
      if (putmsg(fd, &ctlbuf, NULL, 0) < 0) {
	    perror("do_info: putmsg()");
	    return -1;
      }

      ctlbuf.maxlen = sizeof(reply);
      ctlbuf.len = 0;
      ctlbuf.buf = (char *) &reply;
      flags = RS_HIPRI;
      flags = 0;
      if (getmsg(fd, &ctlbuf, NULL, &flags) < 0) {
	    perror("do_info: getmsg()");
	    return -1;
      }

      if (verbose > 2) {
	    printf("do_info: Buffer dump:\n");
	    dumpbuf(ctlbuf);
      }

      if (ctlbuf.len < sizeof(dl_ulong)) {
	    if (verbose)
		  fprintf(stderr, "do_info: Bad reply length %d\n", ctlbuf.len);
	    return -1;
      }

      switch (reply.ack.dl_primitive) {
      case DL_ERROR_ACK:
	    if (verbose)
		  printf("do_info: Got DL_ERROR_ACK\n");
	    if (ctlbuf.len < sizeof(dl_error_ack_t)) {
		  if (verbose)
			fprintf(stderr, "do_info: Bad DL_ERROR_ACK length %d\n",
				ctlbuf.len);
		  return -1;
	    }
	    err_ack = (dl_error_ack_t *) & reply;
	    if (verbose) {
		  printf("do_info: error ack:\n");
		  printf("\tprimitive=%lu, errno=%lu, unix_errno=%lu\n",
			 err_ack->dl_error_primitive, err_ack->dl_errno,
			 err_ack->dl_unix_errno);
	    }
	    return -1;

      case DL_INFO_ACK:
	    if (verbose > 2)
		  printf("do_info: Got DL_INFO_ACK\n");
	    if (ctlbuf.len < sizeof(dl_info_ack_t)) {
		  if (verbose)
			fprintf(stderr, "do_info: Bad DL_INFO_ACK length %d\n",
				ctlbuf.len);
		  return -1;
	    }
	    if (verbose > 1) {
		  printf("do_info: info ack:\n");
		  printf("\tprimitive=%lu\n", reply.ack.dl_primitive);
		  printf("\tmin_sdu=%lu\n", reply.ack.dl_min_sdu);
		  printf("\tmax_sdu=%lu\n", reply.ack.dl_max_sdu);
		  printf("\taddr_length=%lu\n", reply.ack.dl_addr_length);
		  printf("\tmac_type=%lu\n", reply.ack.dl_mac_type);
		  printf("\treserved=%lu\n", reply.ack.dl_reserved);
		  printf("\tcurrent_state=%lu\n", reply.ack.dl_current_state);
		  printf("\tsap_length=%ld\n", reply.ack.dl_sap_length);
		  printf("\tservice_mode=%lu\n", reply.ack.dl_service_mode);
		  printf("\tqos_length=%lu\n", reply.ack.dl_qos_length);
		  printf("\tqos_offset=%lu\n", reply.ack.dl_qos_offset);
		  printf("\tqos_range_length=%lu\n", reply.ack.dl_qos_range_length);
		  printf("\tqos_range_offset=%lu\n", reply.ack.dl_qos_range_offset);
		  printf("\tprovider_style=%ld\n", reply.ack.dl_provider_style);
		  printf("\taddr_offset=%lu\n", reply.ack.dl_addr_offset);
		  printf("\tversion=%lu\n", reply.ack.dl_version);
		  printf("\tbrdcst_addr_length=%lu\n", reply.ack.dl_brdcst_addr_length);
		  printf("\tbrdcst_addr_offset=%lu\n", reply.ack.dl_brdcst_addr_offset);
		  printf("\tgrowth=%lu\n", reply.ack.dl_growth);
		  if (reply.ack.dl_addr_length && reply.ack.dl_addr_offset &&
		      reply.ack.dl_addr_offset + reply.ack.dl_addr_length <= ctlbuf.len)
			dumpaddr("\tAddress", &ctlbuf.buf[reply.ack.dl_addr_offset],
				 reply.ack.dl_addr_length);
		  if (reply.ack.dl_brdcst_addr_length && reply.ack.dl_brdcst_addr_offset
		      && reply.ack.dl_brdcst_addr_offset +
		      reply.ack.dl_brdcst_addr_length <= ctlbuf.len)
			dumpaddr("\tBroadcast address",
				 &ctlbuf.buf[reply.ack.dl_brdcst_addr_offset],
				 reply.ack.dl_brdcst_addr_length);
		  if (reply.ack.dl_qos_length >= sizeof(unsigned long)
		      && reply.ack.dl_qos_offset) {
			dl_qos_cl_sel1_t *sel =
			    (dl_qos_cl_sel1_t *) ((char *) &reply.ack +
						  reply.ack.dl_qos_offset);

			printf("\tQOS selection:\n");
			if (sel->dl_qos_type != DL_QOS_CL_SEL1)
			      printf("\t\tUnknown type %lu, expected %lu\n",
				     sel->dl_qos_type, (dl_ulong) DL_QOS_CL_SEL1);
			else {
			      printf("\t\ttrans_delay=%ld\n", sel->dl_trans_delay);
			      printf("\t\tpriority=%ld\n", sel->dl_priority);
			      printf("\t\tprotection=%ld\n", sel->dl_protection);
			      printf("\t\tresidual_error=%ld\n", sel->dl_residual_error);
			}
		  }
		  if (reply.ack.dl_qos_range_length >= sizeof(unsigned long)
		      && reply.ack.dl_qos_range_offset) {
			dl_qos_cl_range1_t *range =
			    (dl_qos_cl_range1_t *) ((char *) &reply.ack +
						    reply.ack.dl_qos_range_offset);

			printf("\tQOS range:\n");
			if (range->dl_qos_type != DL_QOS_CL_RANGE1)
			      printf("\t\tUnknown type %lu, expected %lu\n",
				     range->dl_qos_type, (dl_ulong) DL_QOS_CL_RANGE1);
			else {
			      printf("\t\ttrans_delay(target, accept)=(%ld, %ld)\n",
				     range->dl_trans_delay.dl_target_value,
				     range->dl_trans_delay.dl_accept_value);
			      printf("\t\tpriority(min, max)=(%ld, %ld)\n",
				     range->dl_priority.dl_min,
				     range->dl_priority.dl_max);
			      printf("\t\tprotection(min, max)=(%ld, %ld)\n",
				     range->dl_protection.dl_min,
				     range->dl_protection.dl_max);
			      printf("\t\tresidual_error=%ld\n",
				     range->dl_residual_error);
			}
		  }
	    }
	    mac_type = reply.ack.dl_mac_type;
	    if (reply.ack.dl_sap_length < 0) {
		  sap_len = -reply.ack.dl_sap_length;
		  sap_first = 0;
	    }
	    else {
		  sap_len = reply.ack.dl_sap_length;
		  sap_first = 1;
	    }
	    addr_len = reply.ack.dl_addr_length - sap_len;
	    if (reply.ack.dl_addr_length != 0 && reply.ack.dl_addr_offset != 0) {
		  memcpy(my_dlsap, &ctlbuf.buf[reply.ack.dl_addr_offset],
			 addr_len + sap_len);
		  if (sap_first) {
			memcpy(my_sap, &ctlbuf.buf[reply.ack.dl_addr_offset], sap_len);
			memcpy(my_addr, &ctlbuf.buf[reply.ack.dl_addr_offset + sap_len],
			       addr_len);
		  }
		  else {
			memcpy(my_addr, &ctlbuf.buf[reply.ack.dl_addr_offset], addr_len);
			memcpy(my_sap, &ctlbuf.buf[reply.ack.dl_addr_offset + addr_len],
			       sap_len);
		  }
	    }
	    if (reply.ack.dl_brdcst_addr_length != 0
		&& reply.ack.dl_brdcst_addr_offset != 0) {
		  assert(reply.ack.dl_addr_length == 0
			 || reply.ack.dl_brdcst_addr_length == addr_len);
		  memcpy(my_brd_addr, &ctlbuf.buf[reply.ack.dl_brdcst_addr_offset],
			 reply.ack.dl_brdcst_addr_length);
	    }
	    break;

      default:
	    if (verbose)
		  fprintf(stderr, "do_info: Unknown reply primitive=%lu\n",
			  reply.ack.dl_primitive);
	    return -1;
      }
      return 0;
}

int do_curr_phys_addr(int fd)
{
      dl_phys_addr_req_t request;
      struct {
	    dl_phys_addr_ack_t ack;
	    unsigned char   extra[1024];
      } reply;
      int             flags;
      struct strbuf   ctlbuf;
      dl_error_ack_t *err_ack;

      if (verbose > 2)
	    printf("do_curr_phys_addr: Sending DL_CURR_PHYS_ADDR\n");

      request.dl_primitive = DL_PHYS_ADDR_REQ;
      request.dl_addr_type = DL_CURR_PHYS_ADDR;
      ctlbuf.maxlen = ctlbuf.len = sizeof(request);
      ctlbuf.buf = (char *) &request;
      if (putmsg(fd, &ctlbuf, NULL, 0) < 0) {
	    perror("do_curr_phys_addr: putmsg()");
	    return -1;
      }

      ctlbuf.maxlen = sizeof(reply);
      ctlbuf.len = 0;
      ctlbuf.buf = (char *) &reply;
      flags = RS_HIPRI;
      if (getmsg(fd, &ctlbuf, NULL, &flags) < 0) {
	    perror("do_curr_phys_addr: getmsg()");
	    return -1;
      }

      if (verbose > 2) {
	    printf("do_curr_phys_addr: Buffer dump:\n");
	    dumpbuf(ctlbuf);
      }

      if (ctlbuf.len < sizeof(dl_ulong)) {
	    if (verbose)
		  fprintf(stderr, "do_curr_phys_addr: Bad reply length %d\n", ctlbuf.len);
	    return -1;
      }

      switch (reply.ack.dl_primitive) {
      case DL_ERROR_ACK:
	    if (verbose)
		  printf("do_curr_phys_addr: Got DL_ERROR_ACK\n");
	    if (ctlbuf.len < sizeof(dl_error_ack_t)) {
		  if (verbose)
			fprintf(stderr, "do_curr_phys_addr: Bad DL_ERROR_ACK length %d\n",
				ctlbuf.len);
		  return -1;
	    }
	    err_ack = (dl_error_ack_t *) & reply;
	    if (verbose) {
		  printf("do_curr_phys_addr: error ack:\n");
		  printf("\tprimitive=%lu, errno=%lu, unix_errno=%lu\n",
			 err_ack->dl_error_primitive, err_ack->dl_errno,
			 err_ack->dl_unix_errno);
	    }
	    return -1;

      case DL_PHYS_ADDR_ACK:
	    if (verbose > 2)
		  printf("do_curr_phys_addr: Got DL_PHYS_ADDR_ACK\n");
	    if (ctlbuf.len < sizeof(dl_phys_addr_ack_t)) {
		  if (verbose)
			fprintf(stderr,
				"do_curr_phys_addr: Bad DL_PHYS_ADDR_ACK length %d\n",
				ctlbuf.len);
		  return -1;
	    }
	    if (verbose > 1) {
		  printf("do_curr_phys_addr: phys_addr ack:\n");
		  printf("\tprimitive=%lu\n", reply.ack.dl_primitive);
		  printf("\taddr_length=%lu\n", reply.ack.dl_addr_length);
		  printf("\taddr_offset=%lu\n", reply.ack.dl_addr_offset);
		  if (reply.ack.dl_addr_length && reply.ack.dl_addr_offset &&
		      reply.ack.dl_addr_offset + reply.ack.dl_addr_length <= ctlbuf.len)
			dumpaddr("\tAddress", &ctlbuf.buf[reply.ack.dl_addr_offset],
				 reply.ack.dl_addr_length);
	    }
	    memcpy(my_addr, &ctlbuf.buf[reply.ack.dl_addr_offset],
		   reply.ack.dl_addr_length);
	    break;

      default:
	    if (verbose)
		  fprintf(stderr, "do_curr_phys_addr: Unknown reply primitive=%lu\n",
			  reply.ack.dl_primitive);
	    return -1;
      }
      return 0;
}

int do_attach(int fd, dl_ulong ppa)
{
      dl_attach_req_t request;
      struct {
	    dl_ok_ack_t     ack;
	    unsigned char   extra[1024];
      } reply;
      int             flags;
      struct strbuf   ctlbuf;
      dl_error_ack_t *err_ack;

      if (verbose > 2)
	    printf("do_attach: Sending DL_ATTACH_REQ\n");

      request.dl_primitive = DL_ATTACH_REQ;
      request.dl_ppa = ppa;
      ctlbuf.maxlen = ctlbuf.len = sizeof(request);
      ctlbuf.buf = (char *) &request;
      if (verbose > 2) {
	    printf("do_attach: Attach_req buffer dump:\n");
	    dumpbuf(ctlbuf);
      }
      if (putmsg(fd, &ctlbuf, NULL, 0) < 0) {
	    perror("do_attach: putmsg()");
	    return -1;
      }

      ctlbuf.maxlen = sizeof(reply);
      ctlbuf.len = 0;
      ctlbuf.buf = (char *) &reply;
      flags = RS_HIPRI;
      if (getmsg(fd, &ctlbuf, NULL, &flags) < 0) {
	    perror("do_attach: getmsg()");
	    return -1;
      }

      if (verbose > 2) {
	    printf("do_attach: Buffer dump:\n");
	    dumpbuf(ctlbuf);
      }

      if (ctlbuf.len < sizeof(dl_ulong)) {
	    if (verbose)
		  fprintf(stderr, "do_attach: Bad reply length %d\n", ctlbuf.len);
	    return -1;
      }

      switch (reply.ack.dl_primitive) {
      case DL_ERROR_ACK:
	    if (verbose)
		  printf("do_attach: Got DL_ERROR_ACK\n");
	    if (ctlbuf.len < sizeof(dl_error_ack_t)) {
		  if (verbose)
			fprintf(stderr, "do_attach: Bad DL_ERROR_ACK length %d\n",
				ctlbuf.len);
		  return -1;
	    }
	    err_ack = (dl_error_ack_t *) & reply;
	    if (verbose) {
		  printf("do_attach: error ack:\n");
		  printf("\tprimitive=%lu, errno=%lu, unix_errno=%lu\n",
			 err_ack->dl_error_primitive, err_ack->dl_errno,
			 err_ack->dl_unix_errno);
	    }
	    return -1;

      case DL_OK_ACK:
	    if (verbose > 2)
		  printf("do_attach: Got DL_OK_ACK\n");
	    if (ctlbuf.len < sizeof(dl_ok_ack_t)) {
		  if (verbose)
			fprintf(stderr, "do_attach: Bad DL_OK_ACK length %d\n",
				ctlbuf.len);
		  return -1;
	    }
	    if (verbose > 1) {
		  printf("do_attach: ok ack:\n");
		  printf("\tprimitive=%lu\n", reply.ack.dl_primitive);
		  printf("\tcorrect_primitive=%lu\n", reply.ack.dl_correct_primitive);
	    }
	    break;

      default:
	    if (verbose)
		  fprintf(stderr, "do_attach: Unknown reply primitive=%lu\n",
			  reply.ack.dl_primitive);
	    return -1;
      }
      return 0;
}

int do_promiscon(int fd, unsigned long level)
{
      dl_promiscon_req_t request;
      struct {
	    dl_ok_ack_t     ack;
	    unsigned char   extra[1024];
      } reply;
      int             flags;
      struct strbuf   ctlbuf;
      dl_error_ack_t *err_ack;

      if (verbose > 2)
	    printf("do_promiscon: Sending DL_PROMISCON_REQ\n");

      request.dl_primitive = DL_PROMISCON_REQ;
      request.dl_level = level;
      ctlbuf.maxlen = ctlbuf.len = sizeof(request);
      ctlbuf.buf = (char *) &request;
      if (putmsg(fd, &ctlbuf, NULL, 0) < 0) {
	    perror("do_promiscon: putmsg()");
	    return -1;
      }

      ctlbuf.maxlen = sizeof(reply);
      ctlbuf.len = 0;
      ctlbuf.buf = (char *) &reply;
      flags = RS_HIPRI;
      if (getmsg(fd, &ctlbuf, NULL, &flags) < 0) {
	    perror("do_promiscon: getmsg()");
	    return -1;
      }

      if (verbose > 2) {
	    printf("do_promiscon: Buffer dump:\n");
	    dumpbuf(ctlbuf);
      }

      if (ctlbuf.len < sizeof(dl_ulong)) {
	    if (verbose)
		  fprintf(stderr, "do_promiscon: Bad reply length %d\n", ctlbuf.len);
	    return -1;
      }

      switch (reply.ack.dl_primitive) {
      case DL_ERROR_ACK:
	    if (verbose)
		  printf("do_promiscon: Got DL_ERROR_ACK\n");
	    if (ctlbuf.len < sizeof(dl_error_ack_t)) {
		  if (verbose)
			fprintf(stderr, "do_promiscon: Bad DL_ERROR_ACK length %d\n",
				ctlbuf.len);
		  return -1;
	    }
	    err_ack = (dl_error_ack_t *) & reply;
	    if (verbose) {
		  printf("do_promiscon: error ack:\n");
		  printf("\tprimitive=%lu, errno=%lu, unix_errno=%lu\n",
			 err_ack->dl_error_primitive, err_ack->dl_errno,
			 err_ack->dl_unix_errno);
	    }
	    return -1;

      case DL_OK_ACK:
	    if (verbose > 2)
		  printf("do_promiscon: Got DL_OK_ACK\n");
	    if (ctlbuf.len < sizeof(dl_ok_ack_t)) {
		  if (verbose)
			fprintf(stderr, "do_promiscon: Bad DL_OK_ACK length %d\n",
				ctlbuf.len);
		  return -1;
	    }
	    if (verbose > 1) {
		  printf("do_promiscon: ok ack:\n");
		  printf("\tprimitive=%lu\n", reply.ack.dl_primitive);
		  printf("\tcorrect_primitive=%lu\n", reply.ack.dl_correct_primitive);
	    }
	    if (reply.ack.dl_correct_primitive != request.dl_primitive) {
		  if (verbose)
			fprintf(stderr,
				"do_promiscon: " "DL_OK_ACK acks wrong primitive\n");
		  return -1;
	    }
	    break;

      default:
	    if (verbose)
		  fprintf(stderr, "do_promiscon: Unknown reply primitive=%lu\n",
			  reply.ack.dl_primitive);
	    return -1;
      }
      return 0;
}

int do_bind(int fd, dl_ulong sap)
{
      dl_bind_req_t   request;
      struct {
	    dl_bind_ack_t   ack;
	    unsigned char   extra[1024];
      } reply;
      int             flags;
      struct strbuf   ctlbuf;
      dl_error_ack_t *err_ack;

      if (verbose > 2)
	    printf("do_bind(sap=0x%lx): Sending DL_BIND_REQ\n", sap);

      request.dl_primitive = DL_BIND_REQ;
      request.dl_sap = sap;
      request.dl_max_conind = 0;
      request.dl_service_mode = DL_CLDLS;
      request.dl_conn_mgmt = 0;
      request.dl_xidtest_flg = 0;
      ctlbuf.maxlen = ctlbuf.len = sizeof(request);
      ctlbuf.buf = (char *) &request;
      if (putmsg(fd, &ctlbuf, NULL, 0) < 0) {
	    perror("do_bind: putmsg()");
	    return -1;
      }

      ctlbuf.maxlen = sizeof(reply);
      ctlbuf.len = 0;
      ctlbuf.buf = (char *) &reply;
      flags = RS_HIPRI;
      if (getmsg(fd, &ctlbuf, NULL, &flags) < 0) {
	    perror("do_bind: getmsg()");
	    return -1;
      }

      if (verbose > 2) {
	    printf("do_bind: Buffer dump:\n");
	    dumpbuf(ctlbuf);
      }

      if (ctlbuf.len < sizeof(dl_ulong)) {
	    if (verbose)
		  fprintf(stderr, "do_bind: Bad reply length %d\n", ctlbuf.len);
	    return -1;
      }

      switch (reply.ack.dl_primitive) {
      case DL_ERROR_ACK:
	    if (verbose)
		  printf("do_bind: Got DL_ERROR_ACK\n");
	    if (ctlbuf.len < sizeof(dl_error_ack_t)) {
		  if (verbose)
			fprintf(stderr, "do_bind: Bad DL_ERROR_ACK length %d\n",
				ctlbuf.len);
		  return -1;
	    }
	    err_ack = (dl_error_ack_t *) & reply;
	    if (verbose) {
		  printf("do_bind: error ack:\n");
		  printf("\tprimitive=%lu, errno=%lu, unix_errno=%lu\n",
			 err_ack->dl_error_primitive, err_ack->dl_errno,
			 err_ack->dl_unix_errno);
	    }
	    return -1;

      case DL_BIND_ACK:
	    if (verbose > 2)
		  printf("do_bind: Got DL_BIND_ACK\n");
	    if (ctlbuf.len < sizeof(dl_bind_ack_t)) {
		  if (verbose)
			fprintf(stderr, "do_bind: Bad DL_BIND_ACK length %d\n",
				ctlbuf.len);
		  return -1;
	    }
	    if (verbose > 1) {
		  printf("do_bind: bind ack:\n");
		  printf("\tprimitive=%lu\n", reply.ack.dl_primitive);
		  printf("\tsap=%lu\n", reply.ack.dl_sap);
		  printf("\taddr_length=%lu\n", reply.ack.dl_addr_length);
		  printf("\taddr_offset=%lu\n", reply.ack.dl_addr_offset);
		  printf("\tmax_conind=%lu\n", reply.ack.dl_max_conind);
		  printf("\txidtest_flg=%lu\n", reply.ack.dl_xidtest_flg);
		  dumpbytes("\tSAP", (char *) &reply.ack + reply.ack.dl_addr_offset,
			    reply.ack.dl_addr_length);
	    }
	    break;

      default:
	    if (verbose)
		  fprintf(stderr, "do_bind: Unknown reply primitive=%lu\n",
			  reply.ack.dl_primitive);
	    return -1;
      }
      return 0;
}

int do_bind_peer(int fd, char *sap, int saplen)
{
      struct {
	    dl_subs_bind_req_t req;
	    unsigned char   extra[1024];
      } request;
      struct {
	    dl_subs_bind_ack_t ack;
	    unsigned char   extra[1024];
      } reply;
      int             flags;
      struct strbuf   ctlbuf;
      dl_error_ack_t *err_ack;

      assert(saplen < 1024);

      if (verbose > 2)
	    printf("do_bind_peer: Sending DL_SUBS_BIND_REQ\n");

      request.req.dl_primitive = DL_SUBS_BIND_REQ;
      request.req.dl_subs_sap_length = saplen;
      request.req.dl_subs_sap_offset = sizeof(request.req);
      request.req.dl_subs_bind_class = DL_PEER_BIND;
      memcpy(request.extra, sap, saplen);

      ctlbuf.maxlen = sizeof(request);
      ctlbuf.len = sizeof(request.req) + saplen;
      ctlbuf.buf = (char *) &request;
      if (verbose > 2) {
	    printf("do_bind_peer: Buffer dump:\n");
	    dumpbuf(ctlbuf);
      }

      if (putmsg(fd, &ctlbuf, NULL, 0) < 0) {
	    perror("do_bind_peer: putmsg()");
	    return -1;
      }

      ctlbuf.maxlen = sizeof(reply);
      ctlbuf.len = 0;
      ctlbuf.buf = (char *) &reply;
      flags = RS_HIPRI;
      if (getmsg(fd, &ctlbuf, NULL, &flags) < 0) {
	    perror("do_bind_peer: getmsg()");
	    return -1;
      }

      if (verbose > 2) {
	    printf("do_bind_peer: Buffer dump:\n");
	    dumpbuf(ctlbuf);
      }

      if (ctlbuf.len < sizeof(dl_ulong)) {
	    if (verbose)
		  fprintf(stderr, "do_bind_peer: Bad reply length %d\n", ctlbuf.len);
	    return -1;
      }

      switch (reply.ack.dl_primitive) {
      case DL_ERROR_ACK:
	    if (verbose)
		  printf("do_bind_peer: Got DL_ERROR_ACK\n");
	    if (ctlbuf.len < sizeof(dl_error_ack_t)) {
		  if (verbose)
			fprintf(stderr, "do_bind_peer: Bad DL_ERROR_ACK length %d\n",
				ctlbuf.len);
		  return -1;
	    }
	    err_ack = (dl_error_ack_t *) & reply;
	    if (verbose) {
		  printf("do_bind_peer: error ack:\n");
		  printf("\tprimitive=%lu, errno=%lu, unix_errno=%lu\n",
			 err_ack->dl_error_primitive, err_ack->dl_errno,
			 err_ack->dl_unix_errno);
	    }
	    return -1;

      case DL_SUBS_BIND_ACK:
	    if (verbose > 2)
		  printf("do_bind_peer: Got DL_SUBS_BIND_ACK\n");
	    if (ctlbuf.len < sizeof(dl_subs_bind_ack_t)) {
		  if (verbose)
			fprintf(stderr, "do_bind_peer: Bad DL_BIND_ACK length %d\n",
				ctlbuf.len);
		  return -1;
	    }
	    if (verbose > 1) {
		  printf("do_bind_peer: subs_bind ack:\n");
		  printf("\tprimitive=%lu\n", reply.ack.dl_primitive);
		  printf("\tsap_length=%lu\n", reply.ack.dl_subs_sap_length);
		  printf("\tsap_offset=%lu\n", reply.ack.dl_subs_sap_offset);
		  dumpbytes("\tSAP", (char *) &reply.ack + reply.ack.dl_subs_sap_offset,
			    reply.ack.dl_subs_sap_length);
	    }
	    break;

      default:
	    if (verbose)
		  fprintf(stderr, "do_bind_peer: Unknown reply primitive=%lu\n",
			  reply.ack.dl_primitive);
	    return -1;
      }
      return 0;
}

int do_send_unitdata(int fd, unsigned char *data, int datalen, unsigned char *dlsap)
{
      struct {
	    struct ethhdr hdr __attribute__ ((packed));
	    char            data[4096] __attribute__ ((packed));
      } pkt;
      struct strbuf   ctlbuf,
                      databuf;
      struct {
	    dl_unitdata_req_t req;
	    unsigned char   extra[1024];
      } request;

      assert(datalen < 4096);

      if (verbose > 1)
	    printf("do_send_unitdata: sending unitdata.\n");

      memcpy(pkt.data, data, datalen);

      if (ldl_flags & LDLFLAG_RAW) {
	    databuf.maxlen = sizeof(pkt);
	    databuf.len = sizeof(pkt.hdr) + datalen;
	    databuf.buf = (char *) &pkt;

	    /*
	     * Fill in header 
	     */
	    assert(addr_len == ETH_ALEN);
	    assert(sap_len == 2);
	    memcpy(pkt.hdr.h_source, my_addr, ETH_ALEN);

	    memcpy(&pkt.hdr.h_proto, dlsap_sap(dlsap), sap_len);
	    memcpy(pkt.hdr.h_dest, dlsap_addr(dlsap), addr_len);

	    if (verbose > 2) {
		  printf("do_send_unitdata: Buffer dump RAW data:\n");
		  dumpbuf(databuf);
	    }

	    if (putmsg(fd, NULL, &databuf, 0) < 0) {
		  perror("do_send_unitdata: putmsg()");
		  return -1;
	    }
      }
      else {
	    databuf.maxlen = sizeof(pkt.data);
	    databuf.len = datalen;
	    databuf.buf = pkt.data;

	    ctlbuf.maxlen = sizeof(request);
	    ctlbuf.len = sizeof(request.req) + addr_len + sap_len;
	    ctlbuf.buf = (char *) &request;

	    request.req.dl_primitive = DL_UNITDATA_REQ;
	    request.req.dl_dest_addr_length = addr_len + sap_len;
	    request.req.dl_dest_addr_offset = sizeof(dl_unitdata_req_t);
	    request.req.dl_priority.dl_min = DL_QOS_DONT_CARE;
	    request.req.dl_priority.dl_max = DL_QOS_DONT_CARE;
	    request.req.dl_priority.dl_min = 50;
	    request.req.dl_priority.dl_max = 50;
	    memcpy(request.extra, dlsap, addr_len + sap_len);

	    if (verbose > 2) {
		  printf("do_send_unitdata: Buffer ctl data:\n");
		  dumpbuf(ctlbuf);
		  printf("do_send_unitdata: Buffer dump data:\n");
		  dumpbuf(databuf);
	    }

	    if (putmsg(fd, &ctlbuf, &databuf, 0) < 0) {
		  perror("do_send_unitdata: putmsg()");
		  return -1;
	    }
      }
      if (verbose > 1)
	    printf("do_send_unitdata: sent unitdata.\n");
      return 0;
}

/**************************************************************/
/*
 */
/*
 * IP address resolution 
 */
/*
 */
/**************************************************************/

/*
 *  Send an ARP request
 */
int send_arp_request(int fd)
{
      struct arphdr  *arp;
      unsigned char  *p,
                      databuf[1024];
      char            sap[2],
                      dlsap[11];

      assert(addr_len == ETH_ALEN);
      assert(sap_len == 2);

      arp = (struct arphdr *) databuf;
      arp->ar_hrd = htons(hw_type);
      arp->ar_pro = htons(ETH_P_IP);
      arp->ar_hln = addr_len;
      arp->ar_pln = 4;
      arp->ar_op = htons(ARPOP_REQUEST);
      p = (char *) (arp + 1);
      memcpy(p, my_addr, addr_len);
      p += addr_len;
      memcpy(p, &my_ip_addr, sizeof(struct in_addr));
      p += sizeof(struct in_addr);
      memset(p, 0, addr_len);
      p += addr_len;
      memcpy(p, &target_ip_addr, sizeof(struct in_addr));
      p += sizeof(struct in_addr);

      *(unsigned short *) sap = ETH_P_ARP;
      build_dlsap(dlsap, my_brd_addr, sap);

      return do_send_unitdata(fd, databuf, p - databuf, dlsap);
}

int process_arp_reply(int fd, char *databuf, int datalen, char *dlsap)
{
      struct arphdr  *arp;
      unsigned char  *pr;

      if (verbose > 2)
	    printf("process_arp_reply() entered\n");

      if (target_ip_addr.s_addr == 0) {
	    if (verbose > 2)
		  printf("process_arp_reply(): Ignoring reply.\n");
	    return 0;
      }

      arp = (struct arphdr *) databuf;
      if (arp->ar_hrd != htons(hw_type) ||
	  arp->ar_pro != htons(ETH_P_IP) ||
	  arp->ar_hln != addr_len || arp->ar_pln != 4
	  || arp->ar_op != htons(ARPOP_REPLY)) {
	    if (verbose > 2)
		  printf("process_arp_reply(): Not for me\n");
	    return 0;		/* Not an IP request */
      }

      pr = (char *) (arp + 1);
      if (memcmp(pr + addr_len, &target_ip_addr, 4)) {
	    if (verbose > 2) {
		  struct in_addr  ip_addr;

		  printf
		      ("process_arp_reply(): ARP reply is not for target IP address (%s)",
		       inet_ntoa(target_ip_addr));
		  memcpy(&ip_addr, pr + addr_len, sizeof(ip_addr));
		  printf(", but for %s\n", inet_ntoa(ip_addr));
	    }
	    return 0;		/* Not for my address */
      }

      memcpy(target_addr, pr, addr_len);

      if (verbose > 2)
	    dumpbytes("Got ARP reply: Target hardware address", target_addr, addr_len);

      got_target_addr = 1;
      kill(getpid(), SIGALRM);

      return 0;
}

int process_arp_request(int fd, char *databuf, int datalen, char *dlsap)
{
      struct arphdr  *arp;
      unsigned char  *pr;

      /*
       * if (verbose > 2) 
       */
      if (verbose)
	    printf("process_arp_request() entered\n");

      if (!arp_respond)
	    return 0;

      arp = (struct arphdr *) databuf;
      if (arp->ar_hrd != htons(hw_type) ||
	  arp->ar_pro != htons(ETH_P_IP) ||
	  arp->ar_hln != addr_len || arp->ar_pln != 4
	  || arp->ar_op != htons(ARPOP_REQUEST)) {
	    if (verbose > 2)
		  printf("process_arp_request(): Not for me\n");
	    return 0;
      }

      pr = (char *) (arp + 1);
      if (memcmp(pr + 2 * addr_len + 4, &my_ip_addr, 4)) {
	    if (verbose > 2) {
		  struct in_addr  ip_addr;

		  printf("process_arp_request(): Not for my address (%s)",
			 inet_ntoa(my_ip_addr));
		  memcpy(&ip_addr, pr + 2 * addr_len + 4, sizeof(ip_addr));
		  printf(", but for %s\n", inet_ntoa(ip_addr));
	    }
	    return 0;		/* Not for my address */
      }

      /*
       * Create ARP reply 
       */
      memcpy(pr + addr_len + 4, my_addr, addr_len);
      arp->ar_op = htons(ARPOP_REPLY);
      {
	    int             i;
	    char           *a = pr,
	                   *b = pr + addr_len + 4;

	    for (i = 0; i < addr_len + 4; ++i) {
		  char            c = a[i];

		  a[i] = b[i];
		  b[i] = c;
	    }
      }

      if (verbose)
	    printf("process_arp_request() replying\n");
      return do_send_unitdata(fd, databuf, datalen, dlsap);
}

int process_incoming_arp(int fd, char *databuf, int datalen, char *dlsap)
{
      struct arphdr  *arp;

      if (verbose > 2)
	    printf("process_incoming_arp() entered\n");

      if (datalen < sizeof(struct arphdr)) {
	    if (verbose)
		  fprintf(stderr, "process_incoming_arp(): Received short ARP frame\n");
	    return -1;
      }

      arp = (struct arphdr *) databuf;
      if (arp->ar_hrd != htons(hw_type) || arp->ar_pro != htons(ETH_P_IP)
	  || arp->ar_hln != addr_len || arp->ar_pln != 4) {
	    if (verbose > 2)
		  printf("process_incoming_arp(): Not for me.\n");
	    return 0;		/* Not an IP request */
      }

      if (datalen < sizeof(struct arphdr) + 2 * (addr_len + 4)) {
	    if (verbose)
		  fprintf(stderr, "process_incoming_arp(): Received short ARP frame\n");
	    return -1;
      }

      switch (ntohs(arp->ar_op)) {
      case ARPOP_REPLY:
	    return process_arp_reply(fd, databuf, datalen, dlsap);
      case ARPOP_REQUEST:
	    return process_arp_request(fd, databuf, datalen, dlsap);
      default:
	    if (verbose)
		  printf
		      ("process_incoming_arp(): Ignoring unknown ARP operation code 0x%04x\n",
		       ntohs(arp->ar_op));
	    return -1;
      }
}

int process_incoming_echo_request(int fd, char *databuf, int datalen, char *dlsap)
{
      struct ping_pkt *pkt = (struct ping_pkt *) databuf;
      __u32           addr;

      if (!echo_respond)
	    return 0;

      /*
       *  Sanity checks
       */
      if (in_cksum(&pkt->icmp_hdr, ntohs(pkt->ip_hdr.tot_len) - sizeof(pkt->ip_hdr)) != 0) {
	    if (verbose)
		  fprintf(stderr,
			  "process_incoming_echo_request: Bad ICMP packet checksum\n");
	    dumpbytes("process_incoming_echo_request: packet dump:", databuf, datalen);
	    return -1;
      }
      assert(pkt->icmp_hdr.type == ICMP_ECHO);
      if (pkt->icmp_hdr.code != 0) {
	    if (verbose)
		  fprintf(stderr,
			  "process_incoming_echo_request: ICMP ECHOREQUEST packet is not code 0\n.");
	    return -1;
      }

      /*
       *  Build reply
       */
      pkt->ip_hdr.ttl = 4;
      pkt->ip_hdr.check = 0;
      addr = pkt->ip_hdr.daddr;
      pkt->ip_hdr.daddr = pkt->ip_hdr.saddr;
      pkt->ip_hdr.saddr = addr;
      pkt->ip_hdr.check = in_cksum(&pkt->ip_hdr, sizeof(struct iphdr));

      pkt->icmp_hdr.type = ICMP_ECHOREPLY;
      pkt->icmp_hdr.code = 0;
      pkt->icmp_hdr.checksum = 0;
      pkt->icmp_hdr.checksum =
	  in_cksum(&pkt->icmp_hdr, ntohs(pkt->ip_hdr.tot_len) - sizeof(pkt->ip_hdr));

      if (verbose > 2)
	    printf("process_incoming_echo_request: Sending ICMP ECHO reply.\n");

      /*
       *  Send reply
       */
      return do_send_unitdata(fd, databuf, datalen, dlsap);
}

int process_incoming_llc_request(int fd, char *databuf, int datalen,
				 unsigned char *src_dlsap, unsigned char *dst_dlsap)
{
      unsigned        cmnd;

      if (verbose > 2) {
	    char           *p;
	    int             n;

	    printf("process_incoming_llc_request: length=%d\n", datalen);
	    for (p = databuf, n = datalen; n > 0; p++, n--)
		  printf("%02x ", *p & 0xff);
	    printf("\n");
      }
      else if (verbose > 1)
	    printf("process_incoming_llc_request: "
		   "%d bytes: %02x %02x %02x\n", datalen, databuf[0] & 0xFF,
		   databuf[1] & 0xFF, databuf[2] & 0xFF);

      if (databuf[1] & 0x01)	/* it's a response */
	    return (1);		/* only respond to commands */

      switch ((databuf[2] & ~0x10) & 0xFF) {
      case 0xE3:		/* TEST */
	    cmnd = databuf[2];
	    break;
      case 0xAF:		/* XID */
	    cmnd = databuf[2];
	    break;
      case 0x43:		/* DISC */
      case 0x6F:		/* SABME */
	    cmnd = 0x0F | (databuf[2] & 0x10);	/* DM, possibly w/F-bit */
	    break;
      default:
	    return (0);		/* no response */
      }

      memcpy(my_addr, dst_dlsap, ETH_ALEN);	/* dst addr is my addr */
      memcpy(target_dlsap, src_dlsap, ETH_ALEN);	/* src addr is remote addr */
      target_sap[0] = databuf[1];	/* trgt sap is ssap */
      my_llc_sap[0] = databuf[0] | 0x01;	/* my sap is dsap */
      send_test_xid(fd, cmnd);	/* return response */
      return (-1);		/* responded to */
}

int _do_rcv_unitdata(int fd, unsigned char *data, int *datalen, unsigned char *src_dlsap,
		     unsigned char *dst_dlsap)
{
      struct strbuf   ctlbuf,
                      databuf;
      char            buf[1024];
      dl_unitdata_ind_t *reply;
      int             flags,
                      ret;
      unsigned short  pkt_type = 0;

      ctlbuf.maxlen = sizeof(buf);
      ctlbuf.len = 0;
      ctlbuf.buf = buf;
      databuf.maxlen = *datalen;
      databuf.len = 0;
      databuf.buf = data;

      flags = 0;
      if (verbose > 1)
	    printf("_do_rcv_unitdata(): Waiting for data...\n");
      ret = getmsg(fd, &ctlbuf, &databuf, &flags);
      if (ret < 0) {
	    perror("_do_rcv_unitdata(): getmsg()");
	    return -1;
      }
      if (ret & MORECTL) {
	    struct strbuf   ctlbuf2;
	    char            buf2[1024];
	    int             flags2,
	                    ret2;

	    if (verbose > 2)
		  printf("_do_rcv_unitdata(): More control data: ignored\n");
	    do {
		  ctlbuf2.maxlen = sizeof(buf2);
		  ctlbuf2.len = 0;
		  ctlbuf2.buf = buf2;
		  flags2 = 0;
		  ret2 = getmsg(fd, &ctlbuf2, NULL, &flags2);
		  if (ret2 < 0) {
			perror("_do_rcv_unitdata(): getmsg()");
			return -1;
		  }
	    }
	    while (ret2 & MORECTL);
      }
      if (ret & MOREDATA) {
	    struct strbuf   databuf2;
	    char            buf2[1024];
	    int             flags2,
	                    ret2;

	    if (verbose > 2)
		  printf("_do_rcv_unitdata(): More data: ignored\n");
	    do {
		  databuf2.maxlen = sizeof(buf2);
		  databuf2.len = 0;
		  databuf2.buf = buf2;
		  flags2 = 0;
		  ret2 = getmsg(fd, NULL, &databuf2, &flags2);
		  if (ret2 < 0) {
			perror("_do_rcv_unitdata(): getmsg()");
			return -1;
		  }
	    }
	    while (ret2 & MOREDATA);
      }

      if (verbose > 2) {
	    printf("_do_rcv_unitdata(): Buffer dump ctl:\n");
	    dumpbuf(ctlbuf);
	    printf("\n_do_rcv_unitdata(): Buffer dump data:\n");
	    dumpbuf(databuf);
      }

      if (ctlbuf.len <= 0 && framing == LDL_FRAME_RAW_LLC) {
	    switch (mac_type) {	/* DLPI MAC type */
	    case DL_ETHER:	/* Ethernet */
		  /*
		   * Extract dst and src addrs.  Tack saps onto the
		   * end of the respective addresses.  Trim addresses
		   * and saps from data buffer.
		   */
		  memcpy(dst_dlsap, &databuf.buf[0], ETH_ALEN);
		  dst_dlsap[ETH_ALEN] = databuf.buf[2 * ETH_ALEN];
		  dst_dlsap[ETH_ALEN + 1] = databuf.buf[2 * ETH_ALEN + 1];
		  dst_dlsap[ETH_ALEN + 2] = databuf.buf[2 * ETH_ALEN + 2];
		  memcpy(src_dlsap, &databuf.buf[ETH_ALEN], ETH_ALEN);
		  src_dlsap[ETH_ALEN] = databuf.buf[2 * ETH_ALEN + 2 + 1];
		  pkt_type = ntohs(*((unsigned short *) &databuf.buf[2 * ETH_ALEN]));
		  if (pkt_type < 1536)	/* llc type */
			databuf.len = pkt_type;
		  else		/* IP or other */
			databuf.len -= (2 * ETH_ALEN + 2);

		  memcpy(&databuf.buf[0], &databuf.buf[2 * ETH_ALEN + 2], databuf.len);
		  addr_len = ETH_ALEN;
		  sap_len = 1;
		  break;
	    case DL_TPR:	/* Token Ring */
		  {
			unsigned        rtelgth = 0;
			tr_hdr_t       *tp = (tr_hdr_t *) databuf.buf;
			tr_llc_frm_hdr_t *llcp;

			/*
			 * THIS CODE IS UNTESTED 
			 */
			memcpy(dst_dlsap, tp->dst_addr, TR_ALEN);
			memcpy(src_dlsap, tp->src_addr, TR_ALEN);
			if ((databuf.buf[1] & 0xC0) != 0x40)	/* not LLC */
			      break;

			if (tp->src_addr[0] & SADDR_0_RTE_PRES) {	/* has source
									 * route field */
			      rtelgth = tp->bl & RCF_0_LLLLL;
			      if ((rtelgth < 2)	/* count includes rte ctl fld */
				  ||(rtelgth & 0x01)	/* must be pairs of bytes */
				  )
				    break;	/* ill-formed */
			}

			llcp =
			    (tr_llc_frm_hdr_t *) (databuf.buf + sizeof(*tp) - 2 +
						  rtelgth);
			dst_dlsap[TR_ALEN] = llcp->llc_dsap;
			src_dlsap[TR_ALEN] = llcp->llc_ssap;
			databuf.len -= (sizeof(*tp) - 2 + rtelgth);
			memcpy(&databuf.buf[0], llcp, databuf.len);
			addr_len = TR_ALEN;
			sap_len = 1;
		  }
		  break;
	    }
      }
      else {
	    if (ctlbuf.len < sizeof(dl_unitdata_ind_t) + 2 * (addr_len + sap_len)) {
		  if (ctlbuf.len == 0) {
			if (verbose)
			      printf("_do_rcv_unitdata(): "
				     "No ctlbuf data, probably leftover data\n");
		  }
		  else {
			if (verbose)
			      fprintf(stderr, "_do_rcv_unitdata(): "
				      "Bad ctlbuf.len=%d returned, "
				      "should be at least %d\n",
				      ctlbuf.len,
				      sizeof(dl_unitdata_ind_t) + 2 * (addr_len +
								       sap_len));
		  }
		  return -1;
	    }

	    reply = (dl_unitdata_ind_t *) buf;

	    memcpy(dst_dlsap, ctlbuf.buf + reply->dl_dest_addr_offset,
		   addr_len + sap_len);
	    memcpy(src_dlsap, ctlbuf.buf + reply->dl_src_addr_offset, addr_len + sap_len);

      }
      *datalen = databuf.len;
      if (verbose > 2) {
	    int             i;

	    printf("    dsap: ");
	    for (i = 0; i < addr_len + sap_len; i++)
		  printf("%02x ", dst_dlsap[i] & 0xff);

	    printf("  ssap: ");
	    for (i = 0; i < addr_len + sap_len; i++)
		  printf("%02x ", src_dlsap[i] & 0xff);
	    printf("\n");
      }

      return 0;
}

int check_ip_packet(unsigned char *data, int datalen)
{

      struct iphdr   *hdr = (struct iphdr *) data;

      if (datalen < sizeof(*hdr)) {
	    if (verbose)
		  fprintf(stderr, "check_ip_packet: Too short for IP header.\n");
	    return -1;
      }
      if (hdr->version != 4) {
	    if (verbose)
		  fprintf(stderr, "check_ip_packet: Not IP version 4.\n");
	    return -1;
      }
      if (hdr->ihl != 5) {
	    if (verbose)
		  fprintf(stderr, "check_ip_packet: Cannot handle IP options.\n");
	    return -1;
      }
      if ((hdr->frag_off & ~0x40) != 0) {
	    if (verbose)
		  fprintf(stderr, "check_ip_packet: Cannot handle fragmented packets.\n");
	    return -1;
      }
      if (in_cksum(hdr, sizeof(*hdr)) != 0) {
	    if (verbose)
		  fprintf(stderr, "check_ip_packet: Bad IP header checksum\n");
	    return -1;
      }
      if (ntohs(hdr->tot_len) > datalen) {
	    if (verbose)
		  fprintf(stderr, "check_ip_packet: Shorter than IP header says.\n");
	    return -1;
      }
      if (hdr->daddr != my_ip_addr.s_addr) {
	    if (verbose > 3) {
		  struct in_addr  ip_addr;

		  printf("check_ip_packet: Not for my address (%s)",
			 inet_ntoa(my_ip_addr));
		  memcpy(&ip_addr, &hdr->daddr, sizeof(ip_addr));
		  printf(", but for %s\n", inet_ntoa(ip_addr));
	    }
	    return -1;
      }

      return 0;
}

int do_rcv_unitdata(int fd, unsigned char *data, int *datalen, unsigned char *src_dlsap,
		    unsigned char *dst_dlsap)
{
      int             dlen;
      unsigned short  pkt_type;

      for (;;) {
	    dlen = *datalen;
	    if (_do_rcv_unitdata(fd, data, &dlen, src_dlsap, dst_dlsap) < 0)
		  return -1;

	    if (mac_type == DL_ETHER)
		  pkt_type = *(unsigned short *) (dst_dlsap + addr_len);
	    else
		  pkt_type = 0x0000;

	    if (pkt_type == ETH_P_ARP) {
		  if (verbose > 2)
			printf("do_rcv_unitdata(): Got ARP packet.\n");
		  process_incoming_arp(fd, data, dlen, src_dlsap);

		  continue;
	    }

	    if (pkt_type == ETH_P_IP) {
		  if (verbose > 2)
			printf("do_rcv_unitdata(): Got IP packet.\n");
		  if (memcmp(my_addr, dlsap_addr(dst_dlsap), addr_len)) {
			if (verbose > 2)
			      printf
				  ("do_rcv_unitdata(): IP packet not for my MAC address.\n");
			continue;
		  }
		  if (check_ip_packet(data, dlen) < 0)
			continue;
		  if (((struct iphdr *) data)->protocol == 1 /* ICMP */  &&
		      ((struct ping_pkt *) data)->icmp_hdr.type == ICMP_ECHO) {
			process_incoming_echo_request(fd, data, dlen, src_dlsap);
			continue;
		  }
	    }
	    else if (ntohs(pkt_type) < 1536) {
		  if (verbose > 2)
			printf("do_rcv_unitdata(): Got LLC frame.\n");
		  if (memcmp(my_addr, dlsap_addr(dst_dlsap), addr_len)) {
			if (verbose > 2)
			      printf
				  ("do_rcv_unitdata(): LLC frame not for my MAC address.\n");
			continue;
		  }
		  if (process_incoming_llc_request(fd, data, dlen, src_dlsap, dst_dlsap)
		      <= 0)
			continue;	/* keep going if cmnd rcvd */
		  /*
		   * fall thru and return if resp received 
		   */
	    }
	    *datalen = dlen;
	    return 0;
      }
}

jmp_buf         timeout_jmp;

static void arp_timeout(int arg)
{
      longjmp(timeout_jmp, 1);
}

int do_arp(int fd, struct in_addr ip, unsigned char *target_addr)
{
      /*
       *  Sending out an ICMP PING packet using the DLPI directly
       *  is a bit hard as we have to know the datalink address of
       *  the node (host or router) that we should use. This address
       *  is normally found using the Address Resolution Protocol,
       *  but DLPI gives us no ARP service.
       *
       *  This means that we have to do the ARP protocol manually.
       *
       *  This will not work across routers (unless the router does
       *  proxy ARP for our target).
       *  Anybody that wants to fix this: Feel free to add /proc/net/route
       *  lookup. (Or better: Do an IP Network Provider Interface).
       *
       */
      char            buf[1024];

      if (verbose > 2)
	    printf("do_arp: Trying to do ARP lookup.\n");

#if 0
      /*
       *  We have no arp on the loopback device
       */
      if (!strcmp((char *) inet_ntoa(ip), "127.0.0.1")) {
#if 0
	    my_ip_addr = ip;
#endif
	    memset(target_addr, 0, ETH_ALEN);
	    build_dlsap(target_dlsap, target_addr, dlsap_sap(my_dlsap));
	    if (verbose > 2)
		  printf("do_arp: ARP lookup on loopback.\n");
	    return 0;
      }
#endif

      if (send_arp_request(fd) < 0) {
	    if (verbose)
		  fprintf(stderr, "do_arp: send_arp_request() failed\n");
	    return -1;
      }

      got_target_addr = 0;
      if (setjmp(timeout_jmp) != 0) {
	    alarm(0);
	    signal(SIGALRM, SIG_IGN);
	    if (!got_target_addr) {
		  if (verbose)
			fprintf(stderr, "Timed out while waiting for ARP reply\n");
		  return -1;
	    }
	    build_dlsap(target_dlsap, target_addr, dlsap_sap(my_dlsap));
	    return 0;
      }
      signal(SIGALRM, arp_timeout);
      alarm(2000);
      for (;;) {		/* Loop broken by signal */
	    int             buflen;
	    unsigned char   src_dlsap[MAX_DL_ADDR_LEN + MAX_DL_SAP_LEN];
	    unsigned char   dst_dlsap[MAX_DL_ADDR_LEN + MAX_DL_SAP_LEN];

	    buflen = 1000;
	    if (do_rcv_unitdata(fd, buf, &buflen, src_dlsap, dst_dlsap) < 0)
		  return -1;
      }
}

/**************************************************************/
/*
 */
/*
 * The ping test routines 
 */
/*
 */
/**************************************************************/

int send_ping(int fd, __u16 id, __u16 sequence)
{
      struct ping_pkt pkt;

      if (verbose > 1) {
	    printf("send_ping: sending ping.\n");
	    printf("send_ping: Target: IP=%s\n", inet_ntoa(target_ip_addr));
      }

      pkt.ip_hdr.version = 4;
      pkt.ip_hdr.ihl = 5;
      pkt.ip_hdr.tos = 0;
      pkt.ip_hdr.tot_len = htons(sizeof(pkt));
      pkt.ip_hdr.id = 1;
      pkt.ip_hdr.frag_off = 0;
      pkt.ip_hdr.ttl = 4;	/* We don't want too much network trouble. ;-) */
      pkt.ip_hdr.protocol = 1;	/* ICMP */
      pkt.ip_hdr.check = 0;
      pkt.ip_hdr.daddr = (__u32) target_ip_addr.s_addr;
      pkt.ip_hdr.saddr = (__u32) my_ip_addr.s_addr;
      pkt.ip_hdr.check = in_cksum(&pkt.ip_hdr, sizeof(struct iphdr));

      pkt.icmp_hdr.type = ICMP_ECHO;
      pkt.icmp_hdr.code = 0;
      pkt.icmp_hdr.checksum = 0;
      pkt.icmp_hdr.un.echo.id = id;
      pkt.icmp_hdr.un.echo.sequence = sequence;

      if (gettimeofday(&pkt.tv, NULL) < 0) {
	    perror("send_ping: gettimeofday()");
	    return -1;
      }
      memset(pkt.filler, 0, sizeof(pkt.filler));
      pkt.icmp_hdr.checksum = in_cksum(&pkt.icmp_hdr, sizeof(pkt) - sizeof(pkt.ip_hdr));

      if (do_send_unitdata(fd, (char *) &pkt, sizeof(pkt), target_dlsap) < 0) {
	    if (verbose)
		  fprintf(stderr, "send_ping: do_send_unitdata() failed.\n");
	    return -1;
      }

      if (verbose > 1)
	    printf("send_ping: sent ping.\n");
      return 0;
}

int rcv_ping(int fd, __u16 id, __u16 * sequence)
{
      int             buflen;
      struct {
	    struct ping_pkt ip_pkt __attribute__ ((packed));
	    unsigned char   extra[1024];
      } pkt;
      unsigned char  *saddr,
                     *ssap;
      unsigned char  *daddr,
                     *dsap;
      char            strs[64],
                      strd[64];
      unsigned char   s_dlsap[MAX_DL_ADDR_LEN + MAX_DL_SAP_LEN];
      unsigned char   d_dlsap[MAX_DL_ADDR_LEN + MAX_DL_SAP_LEN];
      struct timeval  tv;
      double          rtt;

      if (verbose > 3)
	    printf("--\nrcv_ping: Waiting for data...\n");

      buflen = sizeof(pkt);
      if (do_rcv_unitdata(fd, (char *) &pkt, &buflen, s_dlsap, d_dlsap) < 0) {
	    if (verbose)
		  printf("rcv_ping: do_rcv_unitdata() failed\n");
	    return -1;
      }

      if (gettimeofday(&tv, NULL) < 0) {
	    perror("rcv_ping: gettimeofday()");
	    return -1;
      }

      dsap = dlsap_sap(d_dlsap);
      ssap = dlsap_sap(s_dlsap);
      daddr = dlsap_addr(d_dlsap);
      saddr = dlsap_addr(s_dlsap);

      if (memcmp(daddr, my_addr, addr_len)) {
	    if (verbose > 2)
		  printf("rcv_ping: Reply is not for our address\n");
	    return -1;
      }
      if (memcmp(dsap, my_sap, sap_len)) {
	    if (verbose > 2)
		  printf("rcv_ping: Reply is not for our SAP\n");
	    return -1;
      }
      if (memcmp(saddr, target_addr, addr_len)) {
	    if (verbose > 2)
		  printf("rcv_ping: Packet is not from our target MAC addr.\n");
	    return -1;
      }
      if (in_cksum(&pkt.ip_pkt.ip_hdr, sizeof(struct iphdr)) != 0) {
	    if (verbose)
		  fprintf(stderr, "rcv_ping: Bad IP header checksum\n");
	    return -1;
      }
      if (verbose > 3) {
	    struct in_addr  ss;

	    ss.s_addr = pkt.ip_pkt.ip_hdr.saddr;
	    strcpy(strs, (char *) inet_ntoa(ss));
	    ss.s_addr = pkt.ip_pkt.ip_hdr.daddr;
	    strcpy(strd, (char *) inet_ntoa(ss));
	    printf("rcv_ping: Packet from %s to %s\n", strs, strd);
      }
      if (pkt.ip_pkt.ip_hdr.daddr != my_ip_addr.s_addr) {
	    if (verbose > 2)
		  printf("rcv_ping: Packet is not for my IP addr.\n");
	    return -1;
      }
      if (pkt.ip_pkt.ip_hdr.saddr != target_ip_addr.s_addr) {
	    if (verbose > 2)
		  printf("rcv_ping: Packet is not from our target IP addr.\n");
	    return -1;
      }
      if (pkt.ip_pkt.ip_hdr.protocol != 1 /* ICMP */ ) {
	    if (verbose > 2)
		  printf("rcv_ping: Packet from our target is not type ICMP but %d\n.",
			 (int) pkt.ip_pkt.ip_hdr.protocol);
	    return -1;
      }

      if (in_cksum(&pkt.ip_pkt, ntohs(pkt.ip_pkt.ip_hdr.tot_len)) != 0) {
	    if (verbose)
		  fprintf(stderr, "rcv_ping: Bad ICMP packet checksum\n");
	    dumpbytes("rcv_ping: packet dump:", (char *) &pkt, buflen);
	    return -1;
      }
      if (pkt.ip_pkt.icmp_hdr.type != ICMP_ECHOREPLY) {
	    if (verbose > 2)
		  printf
		      ("rcv_ping: Packet from our target is not ICMP type ECHOREPLY.\n");
	    return -1;
      }
      if (pkt.ip_pkt.icmp_hdr.code != 0) {
	    if (verbose)
		  fprintf(stderr,
			  "rcv_ping: ICMP ECHOREPLY packet from our target is not code 0\n.");
	    return -1;
      }
      if (pkt.ip_pkt.icmp_hdr.un.echo.id != id) {
	    if (verbose > 2)
		  printf
		      ("rcv_ping: ICMP ECHOREPLY packet from our target is not for us\n.");
	    return -1;
      }

      *sequence = pkt.ip_pkt.icmp_hdr.un.echo.sequence;

      if (verbose > 3) {
	    printf("\nIt worked !!!!!\n\n");
	    printf("Received ECHO sequence %d from our target\n", *sequence);
	    rtt = (1000.0 * tv.tv_sec + tv.tv_usec / 1000.0) -
		(1000.0 * pkt.ip_pkt.tv.tv_sec + pkt.ip_pkt.tv.tv_usec / 1000.0);
	    printf("RTT = %6.2f ms\n", rtt);
      }

      return 0;
}

int send_test_xid(int fd, int cmnd)
{
      struct strbuf   databuf;

      if (mac_type == DL_ETHER) {
	    struct eth_llc_hdr frm;	/* 802.2 header */

	    databuf.maxlen = sizeof(frm);
	    databuf.len = sizeof(frm);
	    databuf.buf = (char *) &frm;

	    /*
	     * Fill in header 
	     */
	    memcpy(frm.h_source, my_addr, ETH_ALEN);
	    memcpy(frm.h_dest, dlsap_addr(target_dlsap), addr_len);
	    frm.h_len_hi = 0;
	    frm.h_len_lo = 3;	/* 3 bytes of LLC hdr */
	    frm.llc_dsap = target_sap[0];
	    frm.llc_ssap = my_llc_sap[0];
	    frm.llc_cmnd = cmnd;
      }
      else {
	    unsigned char   trbuf[100];	/* token ring */
	    tr_hdr_t       *tp = (tr_hdr_t *) trbuf;
	    tr_llc_frm_hdr_t *llcp;

	    /*
	     * THIS CODE IS UNTESTED 
	     */
	    databuf.maxlen = sizeof(*tp) - 2 + sizeof(*llcp);
	    databuf.len = sizeof(*tp) - 2 + sizeof(*llcp);
	    databuf.buf = (char *) trbuf;

	    tp->acf = 0x10;	/* canned */
	    tp->fcf = 0x40;	/* canned */
	    memcpy(tp->dst_addr, my_addr, TR_ALEN);
	    memcpy(tp->src_addr, dlsap_addr(target_dlsap), TR_ALEN);
	    tp->src_addr[0] &= ~SADDR_0_RTE_PRES;	/* no route */

	    llcp = (tr_llc_frm_hdr_t *) (((char *) (tp + 1)) - 2);
	    llcp->llc_dsap = target_sap[0];
	    llcp->llc_ssap = my_llc_sap[0];
	    llcp->llc_ctl[0] = cmnd;
      }

      if (verbose > 2) {
	    printf("send_test_xid: Buffer dump RAW data:\n");
	    dumpbuf(databuf);
      }

      if (putmsg(fd, NULL, &databuf, 0) < 0) {
	    perror("send_test_xid: putmsg()");
	    return -1;
      }

      if (verbose > 1)
	    printf("send_test_xid: sent LLC command.\n");
      return 0;
}

int rcv_test_xid(int fd)
{
      int             buflen;
      int             i;
      struct {
	    struct eth_llc_hdr frm __attribute__ ((packed));
	    unsigned char   extra[1024];
      } pkt;
      unsigned char  *saddr,
                     *ssap;
      unsigned char  *daddr,
                     *dsap;
      unsigned char   s_dlsap[MAX_DL_ADDR_LEN + MAX_DL_SAP_LEN];
      unsigned char   d_dlsap[MAX_DL_ADDR_LEN + MAX_DL_SAP_LEN];

      if (verbose > 3)
	    printf("--\nrcv_test_xid: Waiting for data...\n");

      buflen = sizeof(pkt);
      if (do_rcv_unitdata(fd, (char *) &pkt, &buflen, s_dlsap, d_dlsap) < 0) {
	    if (verbose)
		  printf("rcv_test_xid: do_rcv_unitdata() failed\n");
	    return -1;
      }

      dsap = dlsap_sap(d_dlsap) + 2;
      ssap = dlsap_sap(s_dlsap);
      daddr = dlsap_addr(d_dlsap);
      saddr = dlsap_addr(s_dlsap);

      if (verbose > 2) {
	    printf("rcv_test_xid: daddr=");
	    for (i = 0; i < addr_len; i++)
		  printf("%02x%c", daddr[i], i == addr_len - 1 ? ' ' : ':');
	    printf("saddr=");
	    for (i = 0; i < addr_len; i++)
		  printf("%02x%c", saddr[i], i == addr_len - 1 ? ' ' : ':');
	    printf("dsap=%02x ssap=%02x\n", *dsap, *ssap);
      }

      if (memcmp(daddr, my_addr, addr_len)) {
	    if (verbose > 2)
		  printf("rcv_test_xid: Reply is not for our address\n");
	    return -1;
      }
      if (memcmp(dsap, my_llc_sap, sap_len)) {
	    if (verbose > 2)
		  printf("rcv_test_xid: Reply is not for our SAP\n");
	    return -1;
      }
      if (memcmp(saddr, dlsap_addr(target_dlsap), addr_len)) {
	    if (verbose > 2)
		  printf("rcv_test_xid: Packet is not from our target MAC addr.\n");
	    return -1;
      }

      if (verbose > 3) {
	    printf("\nIt worked !!!!!\n\n");
      }

      return 0;
}

int get_mac_addr(char *optarg, unsigned char *dlsap)
{
      int             byte;
      int             dlsap_len = 0;
      char           *p = optarg;
      char           *endp;

      while (p && *p) {
	    byte = strtoul(p, &endp, 16);
	    if (*endp && *endp != ':') {
		  if (verbose)
			fprintf(stderr, "Invalid MAC address: %s\n", optarg);
		  return (-1);
	    }
	    *dlsap++ = byte;
	    dlsap_len++;
	    p = endp + (*endp == ':');
      }

      if (dlsap_len != ETH_ALEN) {
	    if (verbose)
		  fprintf(stderr, "MAC address must be %d bytes\n", ETH_ALEN);
	    return (-1);
      }

      return (dlsap_len);
}

int initialize(void)
{
      int             fd;
      unsigned long   ppa;
      char           *name;
      char            arp_sap[2];

      if (framing == LDL_FRAME_EII || framing == LDL_FRAME_SNAP)
	    hw_type = ARPHRD_ETHER;
      else
	    hw_type = ARPHRD_IEEE802;

      if ((fd = open("/dev/ldl", O_RDWR)) < 0) {
	    perror("open(\"/dev/ldl\")");
	    exit(1);
      }
      ppa = do_findppa(fd, interface);
      if (ppa == (unsigned long) -1) {
	    if (verbose)
		  fprintf(stderr, "Unable to find PPA for interface \"%s\".\n",
			  interface);
	    exit(1);
      }
      if (verbose > 2) {
	    if (do_info(fd) < 0)
		  exit(1);
      }
      ppa |= framing;
      if (do_attach(fd, ppa) < 0)
	    exit(1);
      if ((name = do_getname(fd)) == NULL)
	    exit(1);
      if (strcmp(name, interface)) {
	    if (verbose)
		  fprintf(stderr, "Wanted interface \"%s\", but got \"%s\"\n", interface,
			  name);
	    exit(1);
      }
      if (verbose > 2) {
	    printf("INFO after ATTACH:\n");
	    if (do_info(fd) < 0)
		  exit(1);
      }

      if (do_curr_phys_addr(fd) < 0)
	    exit(1);
      if (framing == LDL_FRAME_RAW_LLC && do_promiscon(fd, DL_PROMISC_SAP) < 0)
	    exit(1);
      if (do_bind(fd, framing == LDL_FRAME_RAW_LLC ? ETH_P_RAW : ETH_P_IP) < 0)
	    exit(1);
      if (verbose > 2)
	    printf("INFO after ATTACH and BIND:\n");
      if (do_info(fd) < 0)
	    exit(1);

      if (target_ip_addr.s_addr != 0 || arp_respond != 0) {
	    /*
	     * Do a peer binding for ARP 
	     */
	    assert(sap_len == 2);
	    *(unsigned short *) arp_sap = ETH_P_ARP;
	    if (do_bind_peer(fd, arp_sap, sap_len) < 0)
		  exit(1);
	    if (target_ip_addr.s_addr != 0 && do_arp(fd, target_ip_addr, target_addr) < 0)
		  exit(1);
      }

      return fd;
}

void version(int argc, char *argv[])
{
      if (!verbose)
	    return;
      fprintf(stdout, "\
%1$s %2$s:\n\
    Copyright (c) 2003-2004  OpenSS7 Corporation.  All Rights Reserved.\n\
    Copyright (c) 1998       Ole Husgaard (sparre@login.dknet.dk)\n\
\n\
    Distributed by OpenSS7 Corporation under GPL Version 2,\n\
    included here by reference.\n\
", argv[0], ident);
}

void usage(int argc, char *argv[])
{
      if (!verbose)
	    return;
      fprintf(stderr, "\
Usage:\n\
    %1$s [options] INTERFACE LOCALIP\n\
    %1$s {-h,--help}\n\
    %1$s {-V,--version}\n\
", argv[0]);
}

void help(int argc, char *argv[])
{
      if (!verbose)
	    return;
      fprintf(stdout, "\
Usage:\n\
    %1$s [options] INTERFACE LOCALIP\n\
    %1$s {-h,--help}\n\
    %1$s {-V,--version}\n\
Arguments:\n\
    INTERFACE\n\
        local interface\n\
    LOCALIP\n\
        local ip address\n\
Options:\n\
    -a, --arp\n\
        Respond to arp queries\n\
    -s, --ping\n\
        Respond to ping packets\n\
    -S, --sap DSAP:SSAP\n\
        Set DSAP and SSAP for TEST/XID\n\
    -r, --remoteip REMOTE-IP\n\
        Set remote IP number to ping\n\
    -R, --remotemac REMOTE-MAC\n\
        Set remote adapter address\n\
    -c, --count PACKET-COUNT\n\
        Number of ping packets to send\n\
    -f, --framing FRAMING\n\
        Link framing: RAW or EII or SNAP\n\
    -d, --debug MASK\n\
        Set LDL debug mask.  MASK is a bitwise OR of zero or more of the\n\
        following:\n\
	    0x01 dump raw frame from net driver\n\
	    0x02 dump UNITDATA_IND upstream\n\
	    0x04 dump UNITDATA_REQ downstream\n\
	    0x08 dump Tx to net driver\n\
	    0x10 dump full buffer, not just header\n\
	    0x20 debug info for DL_ATTACH\n\
	    0x40 debug info for DL_BIND\n\
    -H\n\
        List debug flags (obsolete, see above, use -h)\n\
    -g, --global\n\
        Print global LDL statistics\n\
    -T, --test\n\
        Send LLC TEST command\n\
    -X, --xid\n\
        Send LLC XID command\n\
    -v, --verbose [LEVEL]\n\
        Increase verbosity or set to LEVEL [default: 1]\n\
        This option may be repeated.\n\
    -q, --quiet\n\
        Suppress normal output (equivalent to --verbose=0)\n\
    -h, --help, -?, --?\n\
        Print this usage message and exits\n\
    -V, --version\n\
        Print the version and exits\n\
", argv[0]);
}

int main(int argc, char *argv[])
{
      char           *endp;
      long            l;

      int             fd;
      int             i;
      struct timeval  tv1,
                      tv2;
      int             pid = getpid();
      unsigned short  seq = 0,
                      rseq;

      /*
       * Set options defaults 
       */
      verbose = 1;
      arp_respond = 0;
      echo_respond = 0;
      pkt_count = 1;
      target_ip_addr.s_addr = 0;
      framing = LDL_FRAME_EII;

      /*
       * Get command line options 
       */
      for (;;) {
	    int             c,
	                    val;
#ifdef _GNU_SOURCE
	    int             option_index = 0;
	    static struct option long_options[] = {
		  {"arp", 0, 0, 'a'},
		  {"ping", 0, 0, 's'},
		  {"sap", 1, 0, 'S'},
		  {"remoteip", 1, 0, 'r'},
		  {"remotemac", 1, 0, 'R'},
		  {"count", 1, 0, 'c'},
		  {"framing", 1, 0, 'f'},
		  {"debug", 1, 0, 'd'},
		  {"global", 0, 0, 'g'},
		  {"test", 0, 0, 'T'},
		  {"xid", 0, 0, 'X'},
		  {"quiet", 0, 0, 'q'},
		  {"verbose", 2, 0, 'v'},
		  {"help", 0, 0, 'h'},
		  {"version", 0, 0, 'V'},
		  {"?", 0, 0, 'h'},
	    };
	    c = getopt_long_only(argc, argv, "asgHTXr:R:c:v:f:d:S:hqV?", long_options,
				 &option_index);
#else				/* _GNU_SOURCE */
	    c = getopt(argc, argv, "asgHTXr:R:c:v:f:d:S:hqV?");
#endif				/* _GNU_SOURCE */
	    if (c == -1)
		  break;
	    switch (c) {
	    case 'a':		/* -a, --arp */
		  arp_respond = 1;
		  break;
	    case 's':		/* -s, --ping */
		  echo_respond = 1;
		  break;
	    case 'r':		/* -r, --remoteip IPADDR */
		  if (inet_aton(optarg, &target_ip_addr) == 0)
			goto bad_option;
		  break;
	    case 'R':		/* -R, --remotemac MAC */
		  if (get_mac_addr(optarg, target_dlsap) <= 0)
			goto bad_option;
		  break;
	    case 'c':		/* -c, --count PACKET-COUNT */
		  if ((val = strtoul(optarg, &endp, 0)) <= 0)
			goto bad_option;
		  if (endp == NULL || *endp != '\0')
			goto bad_option;
		  pkt_count = val;
		  break;
	    case 'f':		/* -f, --framing FRAMING */
		  if (!strcmp(optarg, "EII")) {
			framing = LDL_FRAME_EII;
			break;
		  }
		  if (!strcmp(optarg, "SNAP")) {
			framing = LDL_FRAME_SNAP;
			break;
		  }
		  if (!strcmp(optarg, "RAW")) {
			framing = LDL_FRAME_RAW_LLC;
			break;
		  }
		  goto bad_option;
	    case 'd':		/* -d, --debug MASK */
		  val = strtol(optarg, &endp, 0);
		  if (endp == NULL || *endp != '\0')
			goto bad_option;
		  do_set_debug_mask(val);
		  exit(0);
	    case 'g':		/* -g, --global */
		  do_global_stats();
		  exit(0);
	    case 'S':		/* -S, --sap SAP */
		  target_sap[0] = (unsigned char) strtol(optarg, &endp, 0);
		  if (*endp == ':')
			my_llc_sap[0] = (unsigned char) strtol(endp + 1, &endp, 0);
		  saps_set = 1;
		  break;
	    case 'T':		/* -T, --test */
		  test_ping = 0xF3;	/* LLC TEST cmnd */
		  break;
	    case 'X':		/* -X, --xid */
		  xid_ping = 0xBF;	/* LLC XID cmnd */
		  break;
	    case 'q':		/* -q, --quiet */
		  verbose = 0;
		  break;
	    case 'v':		/* -v, --verbose [LEVEL] */
		  if (optarg == NULL) {
			verbose++;
			break;
		  }
		  if ((val = strtol(optarg, NULL, 0)) < 0)
			goto bad_option;
		  verbose = val;
		  break;
	    case 'H':		/* -H */
	    case 'h':		/* -h, --help */
		  help(argc, argv);
		  exit(0);
	    case 'V':		/* -V, --version */
		  version(argc, argv);
		  exit(0);
	    case '?':
	    default:
		bad_option:
		  optind--;
		bad_nonopt:
		  if (optind < argc && verbose) {
			fprintf(stderr, "%s: illegal syntax -- ", argv[0]);
			for (; optind < argc; optind++)
			      fprintf(stderr, "%s ", argv[optind]);
			fprintf(stderr, "\n");
		  }
		bad_usage:
		  usage(argc, argv);
		  exit(2);
	    }
      }
      /*
       * don't ignore non-option arguments 
       */
      if (optind < argc)
	    goto bad_nonopt;

      /*
       * Get two mandatory arguments 
       */
      if (test_ping && xid_ping) {
	    if (verbose)
		  fprintf(stderr, "%s: -T and -X are exclusive options\n", argv[0]);
	    goto bad_usage;
      }
      if ((test_xid = (test_ping | xid_ping)) != 0) {
	    if (optind != argc - 1) {
		  if (verbose)
			fprintf(stderr, "%s: Missing 'interface' name\n", argv[0]);
		  goto bad_usage;
	    }
	    framing = LDL_FRAME_RAW_LLC;
	    if (!saps_set) {	/* provide defaults */
		  my_llc_sap[0] = 0x04;
		  target_sap[0] = 0x00;
	    }
      }
      else if (optind != argc - 2) {
	    if (verbose)
		  fprintf(stderr, "%s: Wrong number of arguments after options\n",
			  argv[0]);
	    goto bad_usage;
      }
      interface = argv[optind++];
      if (!test_xid) {
	    if (inet_aton(argv[optind], &my_ip_addr) == 0) {
		  if (verbose)
			fprintf(stderr, "%s: Bad local IP number\n", argv[0]);
		  goto bad_usage;
	    }

	    /*
	     * An extra sanity check 
	     */
	    if (target_ip_addr.s_addr == 0 && arp_respond == 0 && echo_respond == 0) {
		  if (verbose)
			fprintf(stderr,
				"%1$s: No remote to ping, and not responding to ARP or ping:\n"
				"%1$s: This means: nothing to do.\n", argv[9]);
		  goto bad_usage;
	    }

      }

      setlinebuf(stdout);
      setlinebuf(stderr);
      signal(SIGALRM, SIG_IGN);

      fd = initialize();

      if (!test_xid && target_ip_addr.s_addr == 0) {
	    char            buf[4096];
	    unsigned char   s_dlsap[MAX_DL_ADDR_LEN + MAX_DL_SAP_LEN];
	    unsigned char   d_dlsap[MAX_DL_ADDR_LEN + MAX_DL_SAP_LEN];
	    int             buflen;

	    if (verbose)
		  printf("Running in echo mode\n");
	    for (;;) {
		  buflen = 4096;
		  do_rcv_unitdata(fd, buf, &buflen, s_dlsap, d_dlsap);
	    }
      }

      /*
       *  We actually managed to BIND our data link provider.
       *  Now to something interesting...
       */
      gettimeofday(&tv2, NULL);
      for (i = 0; i < pkt_count; ++i) {
	    if (test_xid) {
		  if (send_test_xid(fd, test_xid) < 0)
			exit(1);
		  while (rcv_test_xid(fd) < 0);
	    }
	    else {
		  if (send_ping(fd, pid, seq++) < 0)
			exit(1);
		  while (rcv_ping(fd, pid, &rseq) < 0);
	    }

	    if (i % 1000 == 0) {
		  double          rtt;

		  /*
		   * if (verbose) printf("%d\n", i); 
		   */
		  gettimeofday(&tv1, NULL);
		  rtt =
		      (1000.0 * tv1.tv_sec + tv1.tv_usec / 1000.0) -
		      (1000.0 * tv2.tv_sec + tv2.tv_usec / 1000.0);
		  if (verbose)
			printf("%10d  RTT =%10.4f ms\n", i, rtt / 1000);
		  tv2 = tv1;
	    }
      }

      close(fd);

      exit(0);
}
