/*****************************************************************************

 @(#) src/include/pcapng.h

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This library is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Lesser Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License
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

 *****************************************************************************/

#ifndef _PCAPNG_H
#define _PCAPNG_H

/* This file can be processed with doxygen(1). */

#include <pcap.h>

/**
  * @weakgroup pcapng OpenSS7 PCAP-NG Library
  * @{ */

/** @file
  * PCAP-NG (User) Header File. */

#include <sys/pcapng.h>

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

#ifndef __KERNEL__
/**
  * @name PCAP-NG Library Functions
  * These are the formal version of the PCAP-NG library functions.  They are
  * strong aliased to the thread-safe "__pcapng_*_r()" versions below.  Where a
  * "_r" version does not exist the "__pcapng_*()" version below is used in its
  * stead.
  * @{ */
/* *INDENT-OFF* */
/** PCAP-NG Library Function: pcapng_add_if - add an interface to a PCAP-NG
  * descriptor. */
/* *INDENT-ON* */
extern int pcapng_add_if(pcapng_t * p, pcap_t * i);

int pcap_activate(pcap_t *);
void pcap_breakloop(pcap_t *);
int pcap_can_set_rfmon(pcap_t *);
void pcap_close(pcap_t *);
int pcap_compile(pcap_t *, struct bpf_program *, const char *, int, bpf_u_int32);
int pcap_compile_nopcap(int, int, struct bpf_program *, const char *, int, bpf_u_int32);
pcap_t *pcap_create(const char *, char *);
int pcap_datalink(pcap_t *);
int pcap_datalink_ext(pcap_t *);
int pcap_datalink_name_to_val(const char *);
const char *pcap_datalink_val_to_description(int);
const char *pcap_datalink_val_to_name(int);
int pcap_dispatch(pcap_t *, int, pcap_handler, u_char *);
void pcap_dump(u_char *, const struct pcap_pkthdr *, const u_char *);
void pcap_dump_close(pcap_dumper_t *);
FILE *pcap_dump_file(pcap_dumper_t *);
int pcap_dump_flush(pcap_dumper_t *);
pcap_dumper_t *pcap_dump_fopen(pcap_t *, FILE *fp);
long pcap_dump_ftell(pcap_dumper_t *);
pcap_dumper_t *pcap_dump_open(pcap_t *, const char *);
FILE *pcap_file(pcap_t *);
int pcap_fileno(pcap_t *);
int pcap_findalldevs(pcap_if_t **, char *);
pcap_t *pcap_fopen_offline(FILE *, char *);
void pcap_free_datalinks(int *);
void pcap_free_tstamp_types(int *);
void pcap_freealldevs(pcap_if_t *);
void pcap_freecode(struct bpf_program *);
int pcap_get_selectable_fd(pcap_t *);
char *pcap_geterr(pcap_t *);
int pcap_getnonblock(pcap_t *, char *);
int pcap_inject(pcap_t *, const void *, size_t);
int pcap_is_swapped(pcap_t *);
const char *pcap_lib_version(void);
int pcap_list_datalinks(pcap_t *, int **);
int pcap_list_tstamp_types(pcap_t *, int **);
char *pcap_lookupdev(char *);
int pcap_lookupnet(const char *, bpf_u_int32 *, bpf_u_int32 *, char *);
int pcap_loop(pcap_t *, int, pcap_handler, u_char *);
int pcap_major_version(pcap_t *);
int pcap_minor_version(pcap_t *);
const u_char *pcap_next(pcap_t *, struct pcap_pkthdr *);
int pcap_next_ex(pcap_t *, struct pcap_pkthdr **, const u_char **);
int pcap_offline_filter(struct bpf_program *, const struct pcap_pkthdr *, const u_char *);
pcap_t *pcap_open_dead(int, int);
pcap_t *pcap_open_live(const char *, int, int, int, char *);
pcap_t *pcap_open_offline(const char *, char *);
void pcap_perror(pcap_t *, char *);
int pcap_sendpacket(pcap_t *, const u_char *, int);
int pcap_set_buffer_size(pcap_t *, int);
int pcap_set_datalink(pcap_t *, int);
int pcap_set_promisc(pcap_t *, int);
int pcap_set_rfmon(pcap_t *, int);
int pcap_set_snaplen(pcap_t *, int);
int pcap_set_timeout(pcap_t *, int);
int pcap_set_tstamp_type(pcap_t *, int);
int pcap_setdirection(pcap_t *, pcap_direction_t);
int pcap_setfilter(pcap_t *, struct bpf_program *);
int pcap_setnonblock(pcap_t *, int, char *);
int pcap_snapshot(pcap_t *);
int pcap_stats(pcap_t *, struct pcap_stat *);
const char *pcap_statustostr(int);
const char *pcap_strerror(int);
int pcap_tstamp_type_name_to_val(const char *);
const char *pcap_tstamp_type_val_to_name(int);
const char *pcap_tstamp_type_val_to_description(int);

struct pcapng_pkthdr {
	uint32_t iid; /* interface id */
	uint64_t ts; /* timestamp */
	uint32_t caplen;
	uint32_t len;
	char *comment;
	uint32_t flags;
	uint8_t *hash; /* hash */
	uint16_t hashlen;
	uint64_t drops;	/* packet drops since last packet */
	uint64_t dups; /* identical packets before this packet */
};

struct pcapng {
	int if_count;  /* number of interfaces */
	pcap_t **interfaces; /* interface structure */
	char errbuf[PCAP_ERRBUF_SIZE+1];
	int break_loop;
};

int pcapng_interfaces(pcapng_t *p);
int pcapng_activate(pcapng_t *p, int iid);
void pcapng_breakloop(pcapng_t *p);
int pcapng_can_set_rfmon(pcapng_t *p, int iid);
void pcapng_close(pcapng_t *p);
int pcapng_compile(pcapng_t *p, int iid, struct bpf_program *fp, const char *str, int optimize, bpf_u_int32 netmask);
pcapng_t *pcapng_create(const char *source, char *ebuf);
int pcapng_datalink(pcapng_t *p, int iid);
int pcapng_datalink_ext(pcapng_t *p, int iid);
typedef void (*pcapng_handler)(u_char *user, const struct pcapng_pkthdr *h, const u_char *bytes);
int pcapng_dispatch(pcapng_t *p, int cnt, pcapng_handler callback, u_char *user);
void pcapng_dump(u_char *user, const struct pcapng_pkthdr *h, const u_char *sp);
void pcapng_dump_close(pcapng_dumper_t *p);
FILE *pcapng_dump_file(pcapng_dumper_t *p);
int pcapng_dump_flush(pcapng_dumper_t *p);
pcapng_dumper_t *pcapng_dump_fopen(pcapng_t *p, FILE *fp);
long pcap_dump_ftell(pcapng_dumper_t *p);
pcapng_dumper_t *pcapng_dump_open(pcapng_t *p, const char *fname);
FILE *pcapng_file(pcapng_t *p);
int pcapng_fileno(pcapng_t *p);
pcapng_t *pcapng_fopen_offline(FILE *fp, char *errbuf);
void pcapng_free_datalinks(int *dlt_list);
void pcapng_free_tstamp_types(int *tstamp_types);
int pcapng_get_selectable_fd(pcapng_t *p, int iid);
char *pcapng_geterr(pcapng_t *p);
int pcapng_getnonblock(pcapng_t *p, char *errbuf);
int pcapng_inject(pcapng_t *p, int iid, const void *buf, size_t size);
int pcapng_is_swapped(pcapng_t *p);
const char *pcapng_lib_version(void);
int pcapng_list_datalinks(pcapng_t *p, int iid, int **dlt_buf);
int pcapng_list_tstamp_types(pcapng_t *p, int iid, int **tstamp_types);
int pcapng_loop(pcapng_t *p, int iid, pcapng_handler callback, u_char *user);
int pcapng_major_version(pcapng_t *p);
int pcapng_minor_version(pcapng_t *p);
const u_char *pcapng_next(pcapng_t *p, struct pcapng_pkthdr *h);
int pcapng_next_ex(pcapng_t *p, struct pcapng_pkthdr **pkt_header, const u_char **pkt_data);
int pcapng_offline_filter(struct bpf_program *fp, const struct pcapng_pkthdr *h, const u_char *pkt);
pcapng_t *pcapng_open_dead(int linktype, int snaplen);
pcapng_t *pcapng_open_live(const char *device, int snaplen, int promisc, int to_ms, char *errbuf);
pcapng_t *pcapng_open_offline(const char *fname, char *errbuf);
void pcapng_perror(pcapng_t *p, char *prefix);
int pcapng_sendpacket(pcapng_t *p, int iid, const u_char *buf, int size);
int pcapng_set_buffer_size(pcapng_t *p, int iid, int buffer_size);
int pcapng_set_datalink(pcapng_t *p, int iid, int dlt);
int pcapng_set_promisc(pcapng_t *p, int iid, int promisc);
int pcapng_set_rfmon(pcapng_t *p, int iid, int rfmon);
int pcapng_set_snaplen(pcapng_t *p, int iid, int snaplen);
int pcapng_set_timeout(pcapng_t *p, int to_ms);
int pcapng_set_tstamp_type(pcapng_t *p, int iid, int tstamp_type);
int pcapng_setdirection(pcapng_t *p, int iid, pcap_direction_t d);
int pcapng_setfilter(pcapng_t *p, int iid, struct bpf_program *fp);
int pcapng_setnonblock(pcapng_t *p, int nonblock, char *errbuf);
int pcapng_snapshot(pcapng_t *p, int iid);
int pcapng_stats(pcapng_t *p, int iid, struct pcap_stat *ps);

/** @} */

#endif				/* __KERNEL__ */

/** @} */

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif

#endif				/* _PCAPNG_H */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
