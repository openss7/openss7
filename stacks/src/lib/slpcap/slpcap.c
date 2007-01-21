/*****************************************************************************

 @(#) $RCSfile: slpcap.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/01/21 20:20:12 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2007/01/21 20:20:12 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: slpcap.c,v $
 Revision 0.9.2.1  2007/01/21 20:20:12  brian
 - added documetation

 *****************************************************************************/

#ident "@(#) $RCSfile: slpcap.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/01/21 20:20:12 $"

static char const ident[] = "$RCSfile: slpcap.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/01/21 20:20:12 $";

/*
 *  This is a replacement libpcap to permit live capture of SS7 SDUs using ethereal or wireshark.
 *  This library is simply preloaded before loading the ethereal or tethereal and it overrides the
 *  normal pcap functions.  Another alternative is to place the library in a separate load path
 *  under the name libpcap and set the LD_LIBRARY_PATH appropriately as invoking ethereal or
 *  wireshark.
 *
 *  There are three levels at which one can capture and 4 points in the SS7 stack: the three levels
 *  are MTP2, MTP3 and SCCP.  The four points of capture are the SDTI (MTP2), SLI (MTP3), MTPI
 *  (MTP3) and SCCPI (SCCP) interfaces.  Not all signallingl links support an MTP2 capture.  For
 *  example, M2PA and M2UA do not support an MTP2 level.  (But these can be captured normally as IP
 *  packets.)  Some drivers, such as the x400p_sl driver, do not support monitoring at the SDTI
 *  (MTP2) level, and only support monitoring at the SLI (MTP3) level.  Other drivers, such as the
 *  x400P_ch driver will support both.  For now, the only supported monitoring point is the
 *  /dev/sl-mon device on the sl-mux driver.  A /dev/cd-mon device on the cd-mux driver will be
 *  available later.
 */

#include <pcap.h>

char *__sl_pcap_lookupdev(char *);
int __sl_pcap_lookupnet(char *, bpf_u_int32 *, bpf_u_int32 *, char *);
pcap_t *__sl_pcap_open_live(char *, int, int, int, char *);
pcap_t *__sl_pcap_open_dead(int, int);
pcap_t *__sl_pcap_open_offline(const char *, char *);
void __sl_pcap_close(pcap_t *);
int __sl_pcap_loop(pcap_t *, int, pcap_handler, u_char *);
int __sl_pcap_dispatch(pcap_t *, int, pcap_handler, u_char *);
const u_char *__sl_pcap_next(pcap_t *, struct pcap_pkthdr *);
int __sl_pcap_stats(pcap_t *, struct bpf_program *);
void __sl_pcap_perror(pcap_t *, char *);
char *__sl_pcap_strerror(int);
char *__sl_pcap_geterr(pcap_t *);
int __sl_pcap_compile(pcap_t *, struct bpf_program *, char *, int, bpf_u_int32);
int __sl_pcap_compile_nopcap(int, int, struct bpf_program *, char *, int, bpf_u_int32);
void __sl_pcap_freecode(struct bpf_program *);
int __sl_pcap_datalink(pcap_t *);
int __sl_pcap_snapshot(pcap_t *);
int __sl_pcap_is_swapped(pcap_t *);
int __sl_pcap_major_version(pcap_t *);
int __sl_pcap_minor_version(pcap_t *);

FILE *__sl_pcap_file(pcap_t *);
int __sl_pcap_fileno(pcap_t *);

pcap_dumper_t *__sl_pcap_dump_open(pcap_t *, const char *);
void __sl_pcap_dump_close(pcap_dumper_t *);
void __sl_pcap_dump(u_char *, const struct pcak_pkthdr *, const u_char *);
