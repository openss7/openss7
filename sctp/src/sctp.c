/*****************************************************************************

 @(#) $RCSfile: sctp.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2004/12/22 11:27:54 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2004/12/22 11:27:54 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: sctp.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2004/12/22 11:27:54 $"

static char const ident[] = "$RCSfile: sctp.c,v $ $Name:  $($Revision: 0.9.2.8 $) $Date: 2004/12/22 11:27:54 $";

#include <linux/config.h>
#include <linux/sysctl.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/random.h>
#include <linux/init.h>
#include <linux/socket.h>
#include <net/sock.h>
#include <linux/ipsec.h>
#include <linux/poll.h>
#include <linux/slab.h>
#include <linux/mm.h>

#ifdef CONFIG_SCTP_MODULE
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <net/protocol.h>
#endif

#include <net/inet_common.h>
#ifdef HAVE_NET_XFRM_H
#include <net/xfrm.h>
#endif
#include <net/icmp.h>
#ifdef HAVE_NET_DST_H
#include <net/dst.h>
#endif
#include <net/ip.h>
#ifdef	CONFIG_IP_MASQUERADE
#include <net/ip_masq.h>
#endif
#ifdef	CONFIG_SCTP_ECN
#include <net/inet_ecn.h>
#endif

#include <asm/uaccess.h>
#include <asm/segment.h>

#include <linux/inet.h>
#include <linux/stddef.h>
#include <linux/string.h>
#include <asm/types.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

#include "include/linux/hooks.h"
#include "include/net/net_sctp.h"
#include "include/net/net_sock.h"
#include "include/net/net_snmp.h"
#include "include/os7_namespace.h"

#define SCTP_DESCRIP	"SCTP/IP (RFC 2960) FOR LINUX NET4 $Name:  $($Revision: 0.9.2.8 $)" "\n" \
			"Part of the OpenSS7 Stack for Linux."
#define SCTP_COPYRIGHT	"Copyright (c) 1997-2002 OpenSS7 Corp.  All Rights Reserved."
#define SCTP_DEVICE	"Supports Linux NET4."
#define SCTP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SCTP_LICENSE	"GPL"
#define SCTP_BANNER	SCTP_DESCRIP	"\n" \
			SCTP_COPYRIGHT	"\n" \
			SCTP_DEVICE	"\n" \
			SCTP_CONTACT	"\n"

#ifdef CONFIG_SCTP_MODULE
MODULE_AUTHOR(SCTP_CONTACT);
MODULE_DESCRIPTION(SCTP_DESCRIP);
MODULE_SUPPORTED_DEVICE(SCTP_DEVICE);
MODULE_LICENSE(SCTP_LICENSE);
#endif

#define skb_next(__skb) \
	(((__skb)->next != (struct sk_buff *) (__skb)->list) ? (__skb)->next : NULL)
#define skb_prev(__skb) \
	(((__skb)->prev != (struct sk_buff *) (__skb)->list) ? (__skb)->prev : NULL)

/*
 *  =========================================================================
 *
 *  LOCK DEBUGGING
 *
 *  =========================================================================
 */
#define sock_locked(__sk) ((__sk)->lock.users != 0)

STATIC spinlock_t sctp_protolock = SPIN_LOCK_UNLOCKED;

STATIC char *sctp_state_name[] = {
	"(Uninitialized)",
	"ESTABLISHED",
	"COOKIE-WAIT",
	"COOKIE-ECHOED",
	"SHUTDOWN-PENDING",
	"SHUTDOWN-SENT",
	"(Unreachable)",
	"CLOSED",
	"SHUTDOWN-RECEIVED",
	"SHUTDOWN-RECVWAIT",
	"LISTEN",
	"SHUTDOWN-ACK-SENT",
	"(Max-State)"
};

/* *INDENT-OFF* */
STATIC INLINE void sctp_change_state(struct sock *sk, int newstate)
{
	char *oldname, *newname;
	int oldstate = sk->state;
	switch (oldstate) {
	case 0:				oldname = sctp_state_name[oldstate]; break;
	case SCTP_ESTABLISHED:		oldname = sctp_state_name[oldstate]; break;
	case SCTP_COOKIE_WAIT:		oldname = sctp_state_name[oldstate]; break;
	case SCTP_COOKIE_ECHOED:	oldname = sctp_state_name[oldstate]; break;
	case SCTP_SHUTDOWN_PENDING:	oldname = sctp_state_name[oldstate]; break;
	case SCTP_SHUTDOWN_SENT:	oldname = sctp_state_name[oldstate]; break;
	case SCTP_CLOSED:		oldname = sctp_state_name[oldstate]; break;
	case SCTP_SHUTDOWN_RECEIVED:	oldname = sctp_state_name[oldstate]; break;
	case SCTP_SHUTDOWN_RECVWAIT:	oldname = sctp_state_name[oldstate]; break;
	case SCTP_LISTEN:		oldname = sctp_state_name[oldstate]; break;
	case SCTP_SHUTDOWN_ACK_SENT:	oldname = sctp_state_name[oldstate]; break;
	case SCTP_UNREACHABLE:		oldname = sctp_state_name[oldstate]; break;
	case SCTP_MAX_STATES:		oldname = sctp_state_name[oldstate]; break;
	default:			oldname = "(Invalid)"; break;
	}
	switch (newstate) {
	case 0:				newname = sctp_state_name[newstate]; break;
	case SCTP_ESTABLISHED:		newname = sctp_state_name[newstate]; break;
	case SCTP_COOKIE_WAIT:		newname = sctp_state_name[newstate]; break;
	case SCTP_COOKIE_ECHOED:	newname = sctp_state_name[newstate]; break;
	case SCTP_SHUTDOWN_PENDING:	newname = sctp_state_name[newstate]; break;
	case SCTP_SHUTDOWN_SENT:	newname = sctp_state_name[newstate]; break;
	case SCTP_CLOSED:		newname = sctp_state_name[newstate]; break;
	case SCTP_SHUTDOWN_RECEIVED:	newname = sctp_state_name[newstate]; break;
	case SCTP_SHUTDOWN_RECVWAIT:	newname = sctp_state_name[newstate]; break;
	case SCTP_LISTEN:		newname = sctp_state_name[newstate]; break;
	case SCTP_SHUTDOWN_ACK_SENT:	newname = sctp_state_name[newstate]; break;
	case SCTP_UNREACHABLE:		newname = sctp_state_name[newstate]; break;
	case SCTP_MAX_STATES:		newname = sctp_state_name[newstate]; break;
	default:			newname = "(Invalid)"; break;
	}
	printd(("INFO: STATE CHANGE: sk %p, %s to %s\n", sk, oldname, newname));
	sk->state = newstate;
	return;
}
/* *INDENT-ON* */

/*
 *  =========================================================================
 *
 *  DATA STRUCTURES
 *
 *  =========================================================================
 */
struct sctp_bhash_bucket {
	rwlock_t lock;
	struct sctp_bind_bucket *list;
};

struct sctp_hash_bucket {
	rwlock_t lock;
	struct sctp_opt *list;
};

/*
 *  Queue name shortcuts
 */
#define accq accept_queue
#define rcvq receive_queue
#define expq expedited_queue
#define sndq write_queue
#define errq error_queue
#define oooq out_of_order_queue
#define urgq urgent_queue
#define priq priority_queue
#define rtxq retrans_queue
#define dupq duplicate_queue
#define ackq ack_queue

/*
 *  Timer function forward declarations
 */
STATIC void sctp_init_timeout(unsigned long data);
STATIC void sctp_cookie_timeout(unsigned long data);
STATIC void sctp_shutdown_timeout(unsigned long data);
STATIC void sctp_guard_timeout(unsigned long data);
STATIC void sctp_sack_timeout(unsigned long data);
#ifdef CONFIG_SCTP_ADD_IP
STATIC void sctp_asconf_timeout(unsigned long data);
#endif
#if defined(CONFIG_SCTP_LIFETIMES) || defined(CONFIG_SCTP_PARTIAL_RELIABILITY)
STATIC void sctp_life_timeout(unsigned long data);
#endif
STATIC void sctp_heartbeat_timeout(unsigned long data);
STATIC void sctp_retrans_timeout(unsigned long data);
STATIC void sctp_idle_timeout(unsigned long data);

#define SCTP_TIMER_BACKOFF 1
#define SCTP_TIMER_MINIMUM 1

#define SCTP_FR_COUNT 4

STATIC INLINE void sctp_mod_timer(struct timer_list *timer, unsigned long interval)
{
	mod_timer(timer,
		  jiffies + ((interval >
			     SCTP_TIMER_MINIMUM) ? interval : SCTP_TIMER_MINIMUM));
}

/*
 *  =========================================================================
 *
 *  CHECKSUMS
 *
 *  =========================================================================
 */
STATIC INLINE uint32_t nocsum_and_copy_from_user(const char *src, char *dst,
						 int len, int sum, int *errp)
{
	if (!verify_area(VERIFY_READ, src, len)) {
		memcpy(dst, src, len);
		return (sum);
	}
	if (len)
		*errp = -EFAULT;
	return sum;
}

#if defined(CONFIG_SCTP_ADLER_32)||!defined(CONFIG_SCTP_CRC_32C)
/*
 *  -------------------------------------------------------------------------
 *
 *  Adler-32 Checksum
 *
 *  -------------------------------------------------------------------------
 *  Compute the adler32 checksum.  This follows the description in RFC 2960
 *  Appendix B.  Borrowed from zlib.c.
 */
#define BASE 65521L		/* largest prime smaller than 65536 */
#define NMAX 5552		/* NMAX is the largest n such that 255n(n+1)/2 +
				   (n+1)(BASE-1) <= 2^32-1 */
#define DO1(buf,i)  {s1 += buf[i]; s2 += s1;}
#define DO2(buf,i)  DO1(buf,i); DO1(buf,i+1);
#define DO4(buf,i)  DO2(buf,i); DO2(buf,i+2);
#define DO8(buf,i)  DO4(buf,i); DO4(buf,i+4);
#define DO16(buf)   DO8(buf,0); DO8(buf,8);
STATIC INLINE uint32_t adler32(uint32_t adler, void *buf, size_t len)
{
	register uint32_t s1 = adler;
	register uint32_t s2 = (adler >> 16) & 0xffff;
	register uint8_t *ptr = buf;
	register int k;
	if (!ptr)
		return 1L;
	while (len > 0) {
		k = (len < NMAX) ? len : NMAX;
		len -= k;
		while (k >= 16) {
			DO16(ptr);
			ptr += 16;
			k -= 16;
		}
		if (k != 0)
			do {
				s1 += *ptr++;
				s2 += s1;
			}
			while (--k);
		s1 %= BASE;
		s2 %= BASE;
	}
	return (s2 << 16) | s1;
}

/*
 *  This relies on a characteristic of the adler32 checksum.  The checksum is
 *  two sums:
 *
 *      s1[i] = s1[i-1] + (s1 - 1)
 *      s2[i] = s2[i-1] + s2 + k*(s1[i-1] - 1)
 *
 *  This works as long as the two buffers have a total length < NMAX above.
 *  When the partial sums are complete, it is necessary to apply the %=BASE to
 *  the result before combining it as in the calculation above.
 */
STATIC INLINE uint32_t __add_adler32(uint32_t csum1, uint32_t csum2, uint16_t l2)
{
	uint16_t s1 = (csum1);
	uint16_t s2 = (csum1 >> 16);
	uint16_t p1 = (csum2);
	uint16_t p2 = (csum2 >> 16);
	return ((s2 + p2 + (l2 * (s1 - 1))) << 16) | (s1 + p1);
}
STATIC INLINE uint32_t __final_adler32(uint32_t csum)
{
	uint16_t s1 = (csum);
	uint16_t s2 = (csum >> 16);
	s1 %= BASE;
	s2 %= BASE;
	return (s2 << 16) | s1;
}

#define CP1(i) { s1 += (dst[i]=src[i]); s2 += s1; }
#define CP2(i)  CP1(i); CP1(i+1);
#define CP4(i)  CP2(i); CP2(i+2);
#define CP8(i)  CP4(i); CP4(i+4);
#define CP16    CP8(0); CP8(8);
STATIC INLINE uint32_t
__adler32_partial_copy_from_user(register const unsigned char *src,
				 register unsigned char *dst, int len,
				 uint32_t sum, int *errp)
{
	register uint16_t s1 = sum;
	register uint16_t s2 = sum >> 16;
	(void) errp;
	if (!src)
		return 1L;
	while (len > 0) {
		while (len >= 16) {
			CP16;
			src += 16;
			dst += 16;
			len -= 16;
		}
		if (len != 0)
			do {
				s1 += (*dst++ = *src++);
				s2 += s1;
			} while (--len);
	}
	return (s2 << 16) | s1;
}
STATIC INLINE uint32_t adler32_and_copy_from_user(const char *src, char *dst,
						  int len, int sum, int *errp)
{
	if (!verify_area(VERIFY_READ, src, len))
		return __adler32_partial_copy_from_user(src, dst, len, sum, errp);
	if (len)
		*errp = -EFAULT;
	return sum;
}
#endif
#if defined(CONFIG_SCTP_CRC_32C)
/*
 *  -------------------------------------------------------------------------
 *
 *  CRC-32C Checksum
 *
 *  -------------------------------------------------------------------------
 */
/* #define CRC32C_POLY 0x1EDC6F41 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Copyright 2001, D. Otis.  Use this program, code or tables    */
/* extracted from it, as desired without restriction.            */
/*                                                               */
/* 32 Bit Reflected CRC table generation for SCTP.               */
/* To accommodate serial byte data being shifted out least       */
/* significant bit first, the table's 32 bit words are reflected */
/* which flips both byte and bit MS and LS positions.  The CRC   */
/* is calculated MS bits first from the perspective of the serial*/
/* stream.  The x^32 term is implied and the x^0 term may also   */
/* be shown as +1.  The polynomial code used is 0x1EDC6F41.      */
/* Castagnoli93                                                  */
/* x^32+x^28+x^27+x^26+x^25+x^23+x^22+x^20+x^19+x^18+x^14+x^13+  */
/* x^11+x^10+x^9+x^8+x^6+x^0                                     */
/* Guy Castagnoli Stefan Braeuer and Martin Herrman              */
/* "Optimization of Cyclic Redundancy-Check Codes                */
/* with 24 and 32 Parity Bits",                                  */
/* IEEE Transactions on Communications, Vol.41, No.6, June 1993  */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
uint32_t crc_table[] = {
	0x00000000L, 0xF26B8303L, 0xE13B70F7L, 0x1350F3F4L,
	0xC79A971FL, 0x35F1141CL, 0x26A1E7E8L, 0xD4CA64EBL,
	0x8AD958CFL, 0x78B2DBCCL, 0x6BE22838L, 0x9989AB3BL,
	0x4D43CFD0L, 0xBF284CD3L, 0xAC78BF27L, 0x5E133C24L,
	0x105EC76FL, 0xE235446CL, 0xF165B798L, 0x030E349BL,
	0xD7C45070L, 0x25AFD373L, 0x36FF2087L, 0xC494A384L,
	0x9A879FA0L, 0x68EC1CA3L, 0x7BBCEF57L, 0x89D76C54L,
	0x5D1D08BFL, 0xAF768BBCL, 0xBC267848L, 0x4E4DFB4BL,
	0x20BD8EDEL, 0xD2D60DDDL, 0xC186FE29L, 0x33ED7D2AL,
	0xE72719C1L, 0x154C9AC2L, 0x061C6936L, 0xF477EA35L,
	0xAA64D611L, 0x580F5512L, 0x4B5FA6E6L, 0xB93425E5L,
	0x6DFE410EL, 0x9F95C20DL, 0x8CC531F9L, 0x7EAEB2FAL,
	0x30E349B1L, 0xC288CAB2L, 0xD1D83946L, 0x23B3BA45L,
	0xF779DEAEL, 0x05125DADL, 0x1642AE59L, 0xE4292D5AL,
	0xBA3A117EL, 0x4851927DL, 0x5B016189L, 0xA96AE28AL,
	0x7DA08661L, 0x8FCB0562L, 0x9C9BF696L, 0x6EF07595L,
	0x417B1DBCL, 0xB3109EBFL, 0xA0406D4BL, 0x522BEE48L,
	0x86E18AA3L, 0x748A09A0L, 0x67DAFA54L, 0x95B17957L,
	0xCBA24573L, 0x39C9C670L, 0x2A993584L, 0xD8F2B687L,
	0x0C38D26CL, 0xFE53516FL, 0xED03A29BL, 0x1F682198L,
	0x5125DAD3L, 0xA34E59D0L, 0xB01EAA24L, 0x42752927L,
	0x96BF4DCCL, 0x64D4CECFL, 0x77843D3BL, 0x85EFBE38L,
	0xDBFC821CL, 0x2997011FL, 0x3AC7F2EBL, 0xC8AC71E8L,
	0x1C661503L, 0xEE0D9600L, 0xFD5D65F4L, 0x0F36E6F7L,
	0x61C69362L, 0x93AD1061L, 0x80FDE395L, 0x72966096L,
	0xA65C047DL, 0x5437877EL, 0x4767748AL, 0xB50CF789L,
	0xEB1FCBADL, 0x197448AEL, 0x0A24BB5AL, 0xF84F3859L,
	0x2C855CB2L, 0xDEEEDFB1L, 0xCDBE2C45L, 0x3FD5AF46L,
	0x7198540DL, 0x83F3D70EL, 0x90A324FAL, 0x62C8A7F9L,
	0xB602C312L, 0x44694011L, 0x5739B3E5L, 0xA55230E6L,
	0xFB410CC2L, 0x092A8FC1L, 0x1A7A7C35L, 0xE811FF36L,
	0x3CDB9BDDL, 0xCEB018DEL, 0xDDE0EB2AL, 0x2F8B6829L,
	0x82F63B78L, 0x709DB87BL, 0x63CD4B8FL, 0x91A6C88CL,
	0x456CAC67L, 0xB7072F64L, 0xA457DC90L, 0x563C5F93L,
	0x082F63B7L, 0xFA44E0B4L, 0xE9141340L, 0x1B7F9043L,
	0xCFB5F4A8L, 0x3DDE77ABL, 0x2E8E845FL, 0xDCE5075CL,
	0x92A8FC17L, 0x60C37F14L, 0x73938CE0L, 0x81F80FE3L,
	0x55326B08L, 0xA759E80BL, 0xB4091BFFL, 0x466298FCL,
	0x1871A4D8L, 0xEA1A27DBL, 0xF94AD42FL, 0x0B21572CL,
	0xDFEB33C7L, 0x2D80B0C4L, 0x3ED04330L, 0xCCBBC033L,
	0xA24BB5A6L, 0x502036A5L, 0x4370C551L, 0xB11B4652L,
	0x65D122B9L, 0x97BAA1BAL, 0x84EA524EL, 0x7681D14DL,
	0x2892ED69L, 0xDAF96E6AL, 0xC9A99D9EL, 0x3BC21E9DL,
	0xEF087A76L, 0x1D63F975L, 0x0E330A81L, 0xFC588982L,
	0xB21572C9L, 0x407EF1CAL, 0x532E023EL, 0xA145813DL,
	0x758FE5D6L, 0x87E466D5L, 0x94B49521L, 0x66DF1622L,
	0x38CC2A06L, 0xCAA7A905L, 0xD9F75AF1L, 0x2B9CD9F2L,
	0xFF56BD19L, 0x0D3D3E1AL, 0x1E6DCDEEL, 0xEC064EEDL,
	0xC38D26C4L, 0x31E6A5C7L, 0x22B65633L, 0xD0DDD530L,
	0x0417B1DBL, 0xF67C32D8L, 0xE52CC12CL, 0x1747422FL,
	0x49547E0BL, 0xBB3FFD08L, 0xA86F0EFCL, 0x5A048DFFL,
	0x8ECEE914L, 0x7CA56A17L, 0x6FF599E3L, 0x9D9E1AE0L,
	0xD3D3E1ABL, 0x21B862A8L, 0x32E8915CL, 0xC083125FL,
	0x144976B4L, 0xE622F5B7L, 0xF5720643L, 0x07198540L,
	0x590AB964L, 0xAB613A67L, 0xB831C993L, 0x4A5A4A90L,
	0x9E902E7BL, 0x6CFBAD78L, 0x7FAB5E8CL, 0x8DC0DD8FL,
	0xE330A81AL, 0x115B2B19L, 0x020BD8EDL, 0xF0605BEEL,
	0x24AA3F05L, 0xD6C1BC06L, 0xC5914FF2L, 0x37FACCF1L,
	0x69E9F0D5L, 0x9B8273D6L, 0x88D28022L, 0x7AB90321L,
	0xAE7367CAL, 0x5C18E4C9L, 0x4F48173DL, 0xBD23943EL,
	0xF36E6F75L, 0x0105EC76L, 0x12551F82L, 0xE03E9C81L,
	0x34F4F86AL, 0xC69F7B69L, 0xD5CF889DL, 0x27A40B9EL,
	0x79B737BAL, 0x8BDCB4B9L, 0x988C474DL, 0x6AE7C44EL,
	0xBE2DA0A5L, 0x4C4623A6L, 0x5F16D052L, 0xAD7D5351L
};
#define DOCRC1(buf,i)	{crc=(crc>>8)^crc_table[(crc^(buf[i]))&0xff];}
#define DOCRC2(buf,i)	DOCRC1(buf,i); DOCRC1(buf,i+1);
#define DOCRC4(buf,i)	DOCRC2(buf,i); DOCRC2(buf,i+2);
#define DOCRC8(buf,i)	DOCRC4(buf,i); DOCRC4(buf,i+4);
#define DOCRC16(buf)	DOCRC8(buf,0); DOCRC8(buf,8)
/* Note crc should be initialized to 0xffffffff */
STATIC INLINE uint32_t crc32c(register uint32_t crc, void *buf, register int len)
{
	register uint8_t *ptr = buf;
	if (ptr) {
		while (len >= 16) {
			DOCRC16(ptr);
			ptr += 16;
			len -= 16;
		}
		if (len != 0)
			do {
				crc =
				    (crc >> 8) ^ crc_table[(crc ^ (*ptr++)) & 0xff];
			} while (--len);
	}
	crc = swab32(crc);
	return ~crc;
}

/*
 *  Partial checksumming a CCITT checksum relies on the fact that the result
 *  of the checksum is a long division.  If there are two partial checksums
 *  on *  two strings of length `l' bits, then
 *
 *  A message d can be split into two substrings d1 and d2 such that
 *  d = (d1 * 2**l2) + d2.   Dividing this by the generator polynomial p,
 *  we have:
 *
 *  d/p = (d1/p * 2**l2/p) + d2/p.
 *
 *  The term is brackets is really the long division on d1 taken to l2 more
 *  places.  By staring with the value 1 and calculating the normal crc with
 *  a subsequent message of all zeros to l2 places, a table in l2 can be built
 *  which will supply the contant term, or the constant term can be calculated
 *  by taking a CRO  on zeros (abbreviated calculation) to l2 places.
 *
 */
#define DOZ1	{sum=(sum>>8)^crc_table[sum&0xff];}
#define DOZ2	DOZ1; DOZ1;
#define DOZ4	DOZ2; DOZ2;
#define DOZ8	DOZ4; DOZ4;
#define DOZ16	DOZ8; DOZ8
STATIC INLINE uint32_t __add_crc32c(register uint32_t sum, uint32_t csum2,
				    register uint16_t len)
{
	if (len) {
		while (len >= 16) {
			DOZ16;
			len -= 16;
		}
		if (len != 0)
			do {
				DOZ1;
			} while (--len);
		sum += csum2;
	}
	return (sum);
}
STATIC INLINE uint32_t __final_crc32c(uint32_t csum)
{
	return csum;
}

#define CPCRC1(i)   {crc=(crc>>8)^crc_table[(crc^(dst[i]=src[i]))&0xff];}
#define CPCRC2(i)   CPCRC1(i); CPCRC1(i+1);
#define CPCRC4(i)   CPCRC2(i); CPCRC2(i+2);
#define CPCRC8(i)   CPCRC4(i); CPCRC4(i+4);
#define CPCRC16	    CPCRC8(0); CPCRC8(8)
/* Note crc should be initialized to 0xffffffff */
STATIC INLINE uint32_t
__crc32c_partial_copy_from_user(register const unsigned char *src,
				register unsigned char *dst, register int len,
				register uint32_t crc, int *errp)
{
	(void) errp;
	if (len) {
		while (len >= 16) {
			CPCRC16;
			src += 16;
			dst += 16;
			len -= 16;
		}
		if (len != 0)
			do {
				crc =
				    (crc >> 8) ^ crc_table[(crc ^ (*dst++ = *src++))
							   & 0xff];
			} while (--len);
	}
	return crc;
}
STATIC INLINE uint32_t crc32c_and_copy_from_user(const char *src, char *dst,
						 int len, int sum, int *errp)
{
	if (!verify_area(VERIFY_READ, src, len))
		return __crc32c_partial_copy_from_user(src, dst, len, sum, errp);
	if (len)
		*errp = -EFAULT;
	return sum;
}
#endif
/*
 *  -------------------------------------------------------------------------
 *
 *  CHECKSUM
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE uint32_t cksum(struct sock * sk, void *buf, size_t len)
{
	switch (SCTP_PROT(sk)->cksum) {
	default:
#ifdef CONFIG_SCTP_CRC_32C
	case SCTP_CSUM_CRC32C:
		return crc32c(~0UL, buf, len);
#endif
#if defined(CONFIG_SCTP_ADLER_32)||!defined(CONFIG_SCTP_CRC_32C)
	case SCTP_CSUM_ADLER32:
		return adler32(1UL, buf, len);
#endif
	}
}
STATIC INLINE uint32_t __add_cksum(struct sock * sk, uint32_t csum1,
				   uint32_t csum2, uint16_t l2)
{
	switch (SCTP_PROT(sk)->cksum) {
	default:
#ifdef CONFIG_SCTP_CRC_32C
	case SCTP_CSUM_CRC32C:
		return __add_crc32c(csum1, csum2, l2);
#endif
#if defined(CONFIG_SCTP_ADLER_32)||!defined(CONFIG_SCTP_CRC_32C)
	case SCTP_CSUM_ADLER32:
		return __add_adler32(csum1, csum2, l2);
#endif
	}
}
STATIC INLINE uint32_t __final_cksum(struct sock * sk, uint32_t csum)
{
	switch (SCTP_PROT(sk)->cksum) {
	default:
#ifdef CONFIG_SCTP_CRC_32C
	case SCTP_CSUM_CRC32C:
		return __final_crc32c(csum);
#endif
#if defined(CONFIG_SCTP_ADLER_32)||!defined(CONFIG_SCTP_CRC_32C)
	case SCTP_CSUM_ADLER32:
		return __final_adler32(csum);
#endif
	}
}
STATIC INLINE uint32_t
cksum_and_copy_from_user(struct sock * sk, const char *src, char *dst, int len,
			 int sum, int *errp)
{
	switch (SCTP_PROT(sk)->cksum) {
	default:
#ifdef CONFIG_SCTP_CRC_32C
	case SCTP_CSUM_CRC32C:
		return crc32c_and_copy_from_user(src, dst, len, sum, errp);
#endif
#if defined(CONFIG_SCTP_ADLER_32)||!defined(CONFIG_SCTP_CRC_32C)
	case SCTP_CSUM_ADLER32:
		return adler32_and_copy_from_user(src, dst, len, sum, errp);
#endif
	}
}
STATIC INLINE int cksum_verify(uint32_t csum, void *buf, size_t len)
{
#if defined(CONFIG_SCTP_CRC_32C)||!defined(CONFIG_SCTP_ADLER_32)
	if (csum != crc32c(~0UL, buf, len))
#endif
#ifdef CONFIG_SCTP_ADLER_32
		if (csum != adler32(1UL, buf, len))
#endif
			return (0);
	return (1);
}
STATIC INLINE int cksum_sk_verify(struct sock *sk, uint32_t csum, void *buf,
				  size_t len)
{
	switch (SCTP_PROT(sk)->cksum) {
	default:
#ifdef CONFIG_SCTP_CRC_32C
	case SCTP_CSUM_CRC32C:
		if (csum != crc32c(~0UL, buf, len))
			return (0);
		return (1);
#endif
#if defined(CONFIG_SCTP_ADLER_32)||!defined(CONFIG_SCTP_CRC_32C)
	case SCTP_CSUM_ADLER32:
		if (csum != adler32(1UL, buf, len))
			return (0);
		return (1);
#endif
	}
}
STATIC INLINE uint32_t cksum_generate(void *buf, size_t len)
{
	uint32_t csum = 0;
#if defined(CONFIG_SCTP_CRC_32C)||!defined(CONFIG_SCTP_ADLER_32)
	csum = crc32c(~0UL, buf, len);
#elif defined(CONFIG_SCTP_ADLER_32)
	csum = adler32(1UL, buf, len);
#endif
	return (csum);
}
STATIC INLINE int skb_add_data(struct sk_buff *skb, char *from, int copy)
{
	int err = 0;
	uint32_t csum;
	int off = skb->len;
	if (!(SCTP_SKB_CB(skb)->flags & SCTPCB_FLAG_CKSUMMED))
		csum =
		    nocsum_and_copy_from_user(from, skb_put(skb, copy), copy,
					      skb->csum, &err);
	else
		csum =
		    cksum_and_copy_from_user(skb->sk, from, skb_put(skb, copy),
					     copy, skb->csum, &err);
	if (!err) {
		skb->csum = csum;
		return (0);
	}
	__skb_trim(skb, off);
	return (-EFAULT);
}
STATIC INLINE void skb_init_cksum(struct sk_buff *skb)
{
	if ((skb->sk->route_caps & (NETIF_F_NO_CSUM | NETIF_F_HW_CSUM))) {
		skb->csum = 0;
		return;
	}
	SCTP_SKB_CB(skb)->flags |= SCTPCB_FLAG_CKSUMMED;
	switch (SCTP_PROT(skb->sk)->cksum) {
	default:
#ifdef CONFIG_SCTP_CRC_32C
	case SCTP_CSUM_CRC32C:
		skb->csum = 1UL;
		return;
#endif
#if defined(CONFIG_SCTP_ADLER_32)||!defined(CONFIG_SCTP_CRC_32C)
	case SCTP_CSUM_ADLER32:
		skb->csum = ~0UL;
		return;
#endif
	}
}

/*
 *  =========================================================================
 *
 *  CACHES
 *
 *  =========================================================================
 */
/* these are located in sctp_glue.c */
extern kmem_cache_t *sctp_bind_cachep;
extern kmem_cache_t *sctp_dest_cachep;
extern kmem_cache_t *sctp_srce_cachep;
extern kmem_cache_t *sctp_strm_cachep;
/* *INDENT-OFF* */
STATIC void sctp_init_caches(void)
{
	if (!sctp_bind_cachep)
	if (!(sctp_bind_cachep = kmem_cache_create("sctp_bind_bucket", sizeof(struct sctp_bind_bucket),
					0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
	if (!(sctp_bind_cachep = kmem_find_general_cachep(sizeof(struct sctp_bind_bucket), GFP_ATOMIC)))
		panic("%s: Cannot alloc sctp_bind cache.\n", __FUNCTION__);
	if (!sctp_dest_cachep)
	if (!(sctp_dest_cachep = kmem_cache_create("sctp_daddr", sizeof(struct sctp_daddr),
					0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
	if (!(sctp_dest_cachep = kmem_find_general_cachep(sizeof(struct sctp_daddr), GFP_ATOMIC)))
		panic("%s: Cannot alloc sctp_daddr cache.\n", __FUNCTION__);
	if (!sctp_srce_cachep)
	if (!(sctp_srce_cachep = kmem_cache_create("sctp_saddr", sizeof(struct sctp_saddr),
					0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
	if (!(sctp_srce_cachep = kmem_find_general_cachep(sizeof(struct sctp_saddr), GFP_ATOMIC)))
		panic("%s: Cannot alloc sctp_saddr cache.\n", __FUNCTION__);
	if (!sctp_strm_cachep)
	if (!(sctp_strm_cachep = kmem_cache_create("sctp_strm", sizeof(struct sctp_strm),
					0, SLAB_HWCACHE_ALIGN, NULL, NULL)))
	if (!(sctp_strm_cachep = kmem_find_general_cachep(sizeof(struct sctp_strm), GFP_ATOMIC)))
		panic("%s: Cannot alloc sctp_strm cache.\n", __FUNCTION__);
}
/* *INDENT-ON* */
#if defined CONFIG_SCTP_MODULE
STATIC void sctp_term_caches(void)
{
	/* this has cause a kernel crash in the past */
#if 0
	kmem_cache_shrink(sctp_bind_cachep);
	kmem_cache_shrink(sctp_dest_cachep);
	kmem_cache_shrink(sctp_srce_cachep);
	kmem_cache_shrink(sctp_strm_cachep);
#endif
}
#endif

/* 
 *  Find a Destination Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE struct sctp_daddr *sctp_find_daddr(struct sock *sk, uint32_t daddr)
{
	struct sctp_daddr *sd;
#ifndef CONFIG_SCTP_ADD_IP
	for (sd = SCTP_PROT(sk)->daddr; sd && sd->daddr != daddr; sd = sd->next);
#else
	for (sd = SCTP_PROT(sk)->daddr;
	     sd && !(sd->flags & SCTP_DESTF_UNUSABLE) && sd->daddr != daddr;
	     sd = sd->next);
#endif
	return (sd);
}

/* 
 *  Find a Source Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE struct sctp_saddr *sctp_find_saddr(struct sock *sk, uint32_t saddr)
{
	struct sctp_saddr *ss;
	for (ss = SCTP_PROT(sk)->saddr; ss && ss->saddr != saddr; ss = ss->next);
	return (ss);
}
STATIC struct sctp_strm *sctp_strm_alloc(struct sctp_strm **stp, uint16_t sid,
					 int *erp);
/* 
 *  Find or Add an Inbound or Outbound Stream
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE struct sctp_strm *sctp_istrm_find(struct sock *sk, uint16_t sid,
						int *errp)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_strm *st;
	if (!(st = sp->istr) || st->sid != sid)
		for (st = sp->istrm; st && st->sid != sid; st = st->next);
	if (st)
		sp->istr = st;	/* cache */
	else  
		st = sctp_strm_alloc(&sp->istrm, sid, errp);
	return (st);
}
STATIC INLINE struct sctp_strm *sctp_ostrm_find(struct sock *sk, uint16_t sid,
						int *errp)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_strm *st;
	if (!(st = sp->ostr) || st->sid != sid)
		for (st = sp->ostrm; st && st->sid != sid; st = st->next);
	if (st)
		sp->ostr = st;	/* cache */
	else
		st = sctp_strm_alloc(&sp->ostrm, sid, errp);
	return (st);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  DESTINATION ADDRESS HANDLING
 *
 *  -------------------------------------------------------------------------
 *
 *  Allocate a Destination Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC struct sctp_daddr *__sctp_daddr_alloc(struct sock *sk, uint32_t daddr,
					     int *errp)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_daddr *sd;
	int dat = inet_addr_type(daddr);
	assert(errp);
	ensure(sp, *errp = -EFAULT; return (NULL));
	if (daddr == INADDR_ANY) {
		*errp = -EDESTADDRREQ;
		ptrace(("ERROR: cannot connect to INADDR_ANY for sk = %p\n", sk));
		return (NULL);
	}
	if (sp->daddr && inet_addr_type(sp->daddr->daddr) != dat) {
		*errp = -EADDRNOTAVAIL;
		printd(("WARNING: skipping incompatible %d.%d.%d.%d for sk = %p\n",
			(daddr >> 0) & 0xff, (daddr >> 8) & 0xff,
			(daddr >> 16) & 0xff, (daddr >> 24) & 0xff, sk));
		return (NULL);
	}
	if (dat != RTN_UNICAST && dat != RTN_LOCAL && dat != RTN_ANYCAST) {
		*errp = -EADDRNOTAVAIL;
		printd(("WARNING: skipping non-unicast %d.%d.%d.%d for sk = %p\n",
			(daddr >> 0) & 0xff, (daddr >> 8) & 0xff,
			(daddr >> 16) & 0xff, (daddr >> 24) & 0xff, sk));
		return (NULL);
	}
	/* 
	 *  TODO: need to check permissions (TACCES) for broadcast or multicast addresses
	 *  and whether host addresses are valid (TBADADDR).
	 */
	if ((sd = kmem_cache_alloc(sctp_dest_cachep, SLAB_ATOMIC))) {
		printd(("INFO: Allocating destination address %d.%d.%d.%d for sk = %p\n", (daddr >> 0) & 0xff, (daddr >> 8) & 0xff, (daddr >> 16) & 0xff, (daddr >> 24) & 0xff, sk));
		memset(sd, 0, sizeof(*sd));
		if ((sd->next = sp->daddr))
			sd->next->prev = &sd->next;
		sd->prev = &sp->daddr;
		sp->daddr = sd;
		sp->danum++;
		sd->sp = sp;
		sd->daddr = daddr;
		sd->mtu = 576;	/* fix up after routing */
		sd->dmps = sd->mtu - sp->ext_header_len - sizeof(struct iphdr) - sizeof(struct sctphdr);
		sd->ssthresh = 2 * sd->mtu;	/* fix up after routing */
		sd->cwnd = sd->mtu;	/* fix up after routing */
		/* per destination defaults */
		sd->hb_itvl = sp->hb_itvl;	/* heartbeat interval */
		sd->rto_max = sp->rto_max;	/* maximum RTO */
		sd->rto_min = sp->rto_min;	/* minimum RTO */
		sd->rto = sp->rto_ini;	/* initial RTO */
		sd->max_retrans = sp->rtx_path;	/* max path retrans */
		/* init timers */
		init_timer(&sd->timer_heartbeat);
		sd->timer_heartbeat.function = &sctp_heartbeat_timeout;
		sd->timer_heartbeat.data = (unsigned long) sd;
		init_timer(&sd->timer_retrans);
		sd->timer_retrans.function = &sctp_retrans_timeout;
		sd->timer_retrans.data = (unsigned long) sd;
		init_timer(&sd->timer_idle);
		sd->timer_idle.function = &sctp_idle_timeout;
		sd->timer_idle.data = (unsigned long) sd;
		return (sd);
	}
	*errp = -ENOMEM;
	rare();
	return (NULL);
}

/*
 *  Find or Add a Destination Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC struct sctp_daddr *sctp_daddr_include(struct sock *sk, uint32_t daddr,
					     int *errp)
{
	struct sctp_daddr *sd;
	assert(errp);
	ensure(sk, *errp = -EFAULT;
	       return (NULL));
	if (!(sd = sctp_find_daddr(sk, daddr)))
		sd = __sctp_daddr_alloc(sk, daddr, errp);
	usual(sd);
	return (sd);
}

/*
 *  Free a Destination Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void __sctp_daddr_free(struct sctp_daddr *sd)
{
	ensure(sd, return);
	del_timer(&sd->timer_retrans);
	del_timer(&sd->timer_heartbeat);
	del_timer(&sd->timer_idle);
	/* 
	 *  Need to free any cached IP routes.
	 */
	if (sd->dst_cache)
		dst_release(xchg(&sd->dst_cache, NULL));
	if (sd->sp)
		sd->sp->danum--;
	else
		swerr();
	if ((*sd->prev = sd->next))
		sd->next->prev = sd->prev;
	printd(("INFO: Deallocating destination address %d.%d.%d.%d for sk = %p\n",
		(sd->daddr >> 0) & 0xff, (sd->daddr >> 8) & 0xff,
		(sd->daddr >> 16) & 0xff, (sd->daddr >> 24) & 0xff,
		sd->sp ? SCTP_SOCK(sd->sp) : NULL));
	memset(sd, 0, sizeof(*sd));	/* debug */
	kmem_cache_free(sctp_dest_cachep, sd);
}

/*
 *  Free all Destination Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void __sctp_free_daddrs(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_daddr *sd, *sd_next;
	ensure(sk, return);
	sd_next = sp->daddr;
	usual(sd_next);
	while ((sd = sd_next)) {
		sd_next = sd->next;
		__sctp_daddr_free(sd);
	}
	unless(sp->danum, sp->danum = 0);
	sp->taddr = NULL;
	sp->raddr = NULL;
	sp->caddr = NULL;
	sk->dport = 0;
	sk->daddr = 0;
}

#ifdef CONFIG_SCTP_ADD_IP
/*
 *  Add a Destination Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC struct sctp_daddr *sctp_add_daddr(struct sock *sk, uint32_t daddr)
{
	int err = 0;
	/* TODO: Check that adding the address does not cause a transport endpoint
	   conflict with an existing association. */
	return __sctp_daddr_alloc(sk, daddr, &err);
}

/*
 *  Delete a Destination Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void sctp_del_daddr(struct sctp_daddr *sd)
{
	struct sctp_opt *sp;
	struct sock *sk;
	struct sk_buff *skb;
	ensure(sd, return);
	ensure(sd->sp, return);
	sp = sd->sp;
	sk = SCTP_SOCK(sp);
	if (!(sd->flags & SCTP_DESTF_UNUSABLE)) {
		for (skb = skb_peek(&sk->rcvq); skb; skb = skb_next(skb)) {
			struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
		for (skb = skb_peek(&sp->expq); skb; skb = skb_next(skb)) {
			struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
		for (skb = skb_peek(&sk->sndq); skb; skb = skb_next(skb)) {
			struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
		for (skb = skb_peek(&sp->urgq); skb; skb = skb_next(skb)) {
			struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
		for (skb = skb_peek(&sp->oooq); skb; skb = skb_next(skb)) {
			struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
		for (skb = skb_peek(&sp->dupq); skb; skb = skb_next(skb)) {
			struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
		for (skb = skb_peek(&sp->ackq); skb; skb = skb_next(skb)) {
			struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
		del_timer(&sd->timer_heartbeat);
		del_timer(&sd->timer_idle);
		sd->flags |= SCTP_DESTF_UNUSABLE;
		if (sd->dst_cache)
			dst_release(xchg(&sd->dst_cache, NULL));
	}
	if (!timer_pending(&sd->timer_retrans) && !sd->in_flight) {
		for (skb = skb_peek(&sp->rtxq); skb; skb = skb_next(skb)) {
			struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
			if (cb->daddr == sd)
				cb->daddr = NULL;
		}
		__sctp_daddr_free(sd);
	}
}
#endif				/* CONFIG_SCTP_ADD_IP */

/*
 *  Allocate a group of Destination Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int sctp_alloc_daddrs(struct sock *sk, uint16_t dport, uint32_t * daddrs,
			     size_t dnum)
{
	int err = 0;
	ensure(sk, return (-EFAULT));
	ensure(daddrs || !dnum, return (-EFAULT));
	sk->dport = dport;
	if (dnum) {
		while (dnum--)
			if (!sctp_daddr_include(sk, daddrs[dnum], &err) && err) {
				rare();
				break;
			}
	} else
		usual(dnum);
	if (err) {
		abnormal(err);
		ptrace(("Returning error %d\n", err));
		__sctp_free_daddrs(sk);
	}
	return (err);
}

/*
 *  Allocate a group of Destination Socket Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int sctp_alloc_sock_daddrs(struct sock *sk, uint16_t dport,
				  struct sockaddr_in *daddrs, size_t dnum)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	int err = 0, allocated = 0;
	ensure(sk, return (-EFAULT));
	ensure(daddrs || !dnum, return (-EFAULT));
	if (sp->daddr || sp->danum) {
		swerr();
		__sctp_free_daddrs(sk);
	}
	if (dnum) {
		while (dnum--) {
			if (daddrs[dnum].sin_family != AF_INET ||
			    daddrs[dnum].sin_port != dport) {
				err = -EINVAL;
				break;
			}
			if (sctp_daddr_include
			    (sk, daddrs[dnum].sin_addr.s_addr, &err))
				allocated++;
			else if (err) {
				rare();
				break;
			}
		}
	} else
		usual(dnum);
	if (err) {
		abnormal(err);
		__sctp_free_daddrs(sk);
		return (err);
	}
	return (allocated);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SOURCE ADDRESS HANDLING
 *
 *  -------------------------------------------------------------------------
 *
 *  Allocate a Source Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
#ifndef sysctl_ip_nonlocal_bind
#define sysctl_ip_nonlocal_bind 0
#endif
STATIC struct sctp_saddr *__sctp_saddr_alloc(struct sock *sk, uint32_t saddr,
					     int *errp)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_saddr *ss;
	if (saddr == INADDR_ANY) {
		printd(("WARNING: skipping INADDR_ANY for sk = %p\n", sk));
		return (NULL);
	}
	if (sp->saddr && LOOPBACK(sp->saddr->saddr) != LOOPBACK(saddr)) {
		*errp = -EADDRNOTAVAIL;
		printd(("WARNING: skipping incompatible %d.%d.%d.%d for sk = %p\n",
			(saddr >> 0) & 0xff, (saddr >> 8) & 0xff,
			(saddr >> 16) & 0xff, (saddr >> 24) & 0xff, sk));
		return (NULL);
	}
	if (!sysctl_ip_nonlocal_bind && inet_addr_type(saddr) != RTN_LOCAL) {
		*errp = -EADDRNOTAVAIL;
		printd(("WARNING: skipping non-local %d.%d.%d.%d for sk = %p\n",
			(saddr >> 0) & 0xff, (saddr >> 8) & 0xff,
			(saddr >> 16) & 0xff, (saddr >> 24) & 0xff, sk));
		return (NULL);
	}
	if ((ss = kmem_cache_alloc(sctp_srce_cachep, SLAB_ATOMIC))) {
		printd(("INFO: Allocating source address %d.%d.%d.%d for sk = %p\n",
			(saddr >> 0) & 0xff, (saddr >> 8) & 0xff,
			(saddr >> 16) & 0xff, (saddr >> 24) & 0xff, sk));
		memset(ss, 0, sizeof(*ss));
		if ((ss->next = sp->saddr))
			ss->next->prev = &ss->next;
		ss->prev = &sp->saddr;
		sp->saddr = ss;
		sp->sanum++;
		ss->sp = sp;
		ss->saddr = saddr;
		return (ss);
	}
	*errp = -ENOMEM;
	return (ss);
}

/*
 *  Find or Add a Source Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC struct sctp_saddr *sctp_saddr_include(struct sock *sk, uint32_t saddr,
					     int *errp)
{
	struct sctp_saddr *ss;
	assert(errp);
	ensure(sk, *errp = -EFAULT;
	       return (NULL));
	if (!(ss = sctp_find_saddr(sk, saddr)))
		ss = __sctp_saddr_alloc(sk, saddr, errp);
	usual(ss);
	return (ss);
}

/*
 *  Free a Source Address
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void __sctp_saddr_free(struct sctp_saddr *ss)
{
	assert(ss);
	if (ss->sp)
		ss->sp->sanum--;
	else
		swerr();
	if ((*ss->prev = ss->next))
		ss->next->prev = ss->prev;
	printd(("INFO: Deallocating source address %d.%d.%d.%d for sk = %p\n",
		(ss->saddr >> 0) & 0xff, (ss->saddr >> 8) & 0xff,
		(ss->saddr >> 16) & 0xff, (ss->saddr >> 24) & 0xff,
		ss->sp ? SCTP_SOCK(ss->sp) : NULL));
	memset(ss, 0, sizeof(*ss));	/* debug */
	kmem_cache_free(sctp_srce_cachep, ss);
}

/*
 *  Free all Source Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void __sctp_free_saddrs(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_saddr *ss, *ss_next;
	assert(sk);
	ss_next = sp->saddr;
	usual(ss_next);
	while ((ss = ss_next)) {
		ss_next = ss->next;
		__sctp_saddr_free(ss);
	}
	unless(sp->sanum, sp->sanum = 0);
	sk->rcv_saddr = 0;
	sk->saddr = 0;
}

/*
 *  Allocate a group of Source Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int sctp_alloc_saddrs(struct sock *sk, uint16_t sport, uint32_t * saddrs,
			     size_t anum)
{
	int err = 0;
	ensure(sk, return (-EFAULT));
	ensure(saddrs || !anum, return (-EFAULT));
	sk->sport = sport;
	if (anum) {
		while (anum--)
			if (!sctp_saddr_include(sk, saddrs[anum], &err) && err) {
				rare();
				break;
			}
	} else
		usual(anum);
	if (err) {
		rare();
		__sctp_free_saddrs(sk);
	}
	return (err);
}

/*
 *  Allocate a group of Source Socket Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int sctp_alloc_sock_saddrs(struct sock *sk, uint16_t sport,
				  struct sockaddr_in *saddrs, size_t snum)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	int err = 0, allocated = 0;
	ensure(sk, return (-EFAULT));
	ensure(saddrs || !snum, return (-EFAULT));
	if (sp->saddr || sp->sanum) {
		swerr();
		__sctp_free_saddrs(sk);
	}
	if (snum) {
		while (snum--) {
			if (saddrs[snum].sin_family != AF_INET ||
			    saddrs[snum].sin_port != sport) {
				err = -EINVAL;
				break;
			}
			if (sctp_saddr_include
			    (sk, saddrs[snum].sin_addr.s_addr, &err))
				allocated++;
			else if (err) {
				rare();
				break;
			}
		}
	} else
		usual(snum);
	if (err) {
		abnormal(err);
		__sctp_free_saddrs(sk);
		return (err);
	}
	return (allocated);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SCTP Stream handling
 *
 *  -------------------------------------------------------------------------
 *
 *  Allocate an Inbound or Outbound Stream
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC struct sctp_strm *sctp_strm_alloc(struct sctp_strm **stp, uint16_t sid,
					 int *errp)
{
	struct sctp_strm *st;
	if ((st = kmem_cache_alloc(sctp_strm_cachep, SLAB_ATOMIC))) {
		memset(st, 0, sizeof(*st));
		if ((st->next = (*stp)))
			st->next->prev = &st->next;
		st->prev = stp;
		(*stp) = st;
		st->sid = sid;
		st->ssn = -1;
		return (st);
	}
	*errp = -ENOMEM;
	rare();
	return (NULL);
}

/*
 *  Free a Stream
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void sctp_strm_free(struct sctp_strm *st)
{
	assert(st);
	if ((*st->prev = st->next))
		st->next->prev = st->prev;
	memset(st, 0, sizeof(*st));	/* debug */
	kmem_cache_free(sctp_strm_cachep, st);
}

/*
 *  Free all Streams
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void sctp_free_strms(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_strm *st, *st_next;
	assert(sk);
	st_next = sp->ostrm;
	usual(st_next);
	while ((st = st_next)) {
		st_next = st->next;
		sctp_strm_free(st);
	}
	sp->ostr = NULL;
	st_next = sp->istrm;
	usual(st_next);
	while ((st = st_next)) {
		st_next = st->next;
		sctp_strm_free(st);
	}
	sp->istr = NULL;
}

/*
 *  =========================================================================
 *
 *  HASHES
 *
 *  =========================================================================
 */
STATIC unsigned int sctp_bhash_size = 0;
#define sctp_lhash_size (1<<6)
#define sctp_phash_size (1<<6)
#define sctp_cache_size (1<<6)
STATIC unsigned int sctp_vhash_size = 0;
#define sctp_thash_size (1<<6)

STATIC struct sctp_bhash_bucket *sctp_bhash;	/* bind */
STATIC struct sctp_hash_bucket sctp_lhash[sctp_lhash_size];	/* listen */
STATIC struct sctp_hash_bucket sctp_phash[sctp_phash_size];	/* p_tag */
STATIC struct sctp_hash_bucket sctp_cache[sctp_cache_size];	/* v_tag level 1 */
STATIC struct sctp_hash_bucket *sctp_vhash;	/* v_tag level 2 */
STATIC struct sctp_hash_bucket sctp_thash[sctp_thash_size];	/* tcb level 4 */

STATIC unsigned int sctp_bhash_order = 0;
STATIC unsigned int sctp_vhash_order = 0;

STATIC void sctp_init_hashes(void)
{
	int order;
	unsigned long goal;
	/* size and allocate vtag hash table */
	goal = num_physpages >> (20 - PAGE_SHIFT);
	for (order = 0; (1UL << order) < goal; order++);
	do {
		sctp_vhash_order = order;
		sctp_vhash_size =
		    (1UL << order) * PAGE_SIZE / sizeof(struct sctp_hash_bucket);
		sctp_vhash =
		    (struct sctp_hash_bucket *) __get_free_pages(GFP_ATOMIC, order);
	} while (sctp_vhash == NULL && --order >= 0);
	if (!sctp_vhash)
		panic("%s: Failed to allocate SCTP established hash table\n",
		      __FUNCTION__);
	/* size and allocate bind hash table */
	goal = (((1UL << order) * PAGE_SIZE) / sizeof(struct sctp_hash_bucket));
	if (goal > (64 * 1024)) {
		goal =
		    (((64 * 1024) * sizeof(struct sctp_bind_bucket *)) / PAGE_SIZE);
		for (order = 0; (1UL << order) < goal; order++);
	}
	do {
		sctp_bhash_order = order;
		sctp_bhash_size =
		    (1UL << order) * PAGE_SIZE / sizeof(struct sctp_bhash_bucket);
		sctp_bhash =
		    (struct sctp_bhash_bucket *) __get_free_pages(GFP_ATOMIC, order);
	} while (sctp_bhash == NULL && --order >= 0);
	if (!sctp_bhash)
		panic("%s: Failed to allocate SCTP bind hash table\n", __FUNCTION__);
	memset(sctp_bhash, 0, sctp_bhash_size * sizeof(struct sctp_bhash_bucket));
	memset(sctp_lhash, 0, sctp_lhash_size * sizeof(struct sctp_hash_bucket));
	memset(sctp_phash, 0, sctp_phash_size * sizeof(struct sctp_hash_bucket));
	memset(sctp_cache, 0, sctp_cache_size * sizeof(struct sctp_hash_bucket));
	memset(sctp_vhash, 0, sctp_vhash_size * sizeof(struct sctp_hash_bucket));
	memset(sctp_thash, 0, sctp_thash_size * sizeof(struct sctp_hash_bucket));
	printd(("INFO: bind hash table configured size = %d\n", sctp_bhash_size));
	printd(("INFO: list hash table configured size = %d\n", sctp_lhash_size));
	printd(("INFO: ptag hash table configured size = %d\n", sctp_phash_size));
	printd(("INFO: vtag cach table configured size = %d\n", sctp_cache_size));
	printd(("INFO: vtag hash table configured size = %d\n", sctp_vhash_size));
	printd(("INFO: tcb  hash table configured size = %d\n", sctp_thash_size));
}
#if defined CONFIG_SCTP_MODULE
STATIC void sctp_term_hashes(void)
{
	/* free hashes */
	if (sctp_vhash) {
		free_pages((unsigned long) sctp_vhash, sctp_vhash_order);
		sctp_vhash = NULL;
		sctp_vhash_order = 0;
	}
	if (sctp_bhash) {
		free_pages((unsigned long) sctp_bhash, sctp_bhash_order);
		sctp_bhash = NULL;
		sctp_bhash_order = 0;
	}
}
#endif

/*
 *  Hashing Functions
 *  -------------------------------------------------------------------------
 */
STATIC INLINE uint sctp_bhashfn(uint16_t num)
{
	return ((sctp_bhash_size - 1) & num);
}
STATIC INLINE uint sctp_lhashfn(uint16_t port)
{
	return ((sctp_lhash_size - 1) & port);
}
STATIC INLINE uint sctp_phashfn(uint32_t ptag)
{
	return ((sctp_phash_size - 1) & ptag);
}
STATIC INLINE uint sctp_cachefn(uint32_t vtag)
{
	return ((sctp_cache_size - 1) & vtag);
}
STATIC INLINE uint sctp_vhashfn(uint32_t vtag)
{
	return ((sctp_vhash_size - 1) & vtag);
}
STATIC INLINE uint sctp_thashfn(uint16_t sport, uint16_t dport)
{
	return ((sctp_thash_size - 1) & (sport + (dport << 4)));
}
STATIC INLINE uint sctp_sk_bhashfn(struct sock * sk)
{
	return sctp_bhashfn(sk->num);
}
STATIC INLINE uint sctp_sp_lhashfn(struct sctp_opt * sp)
{
	return sctp_lhashfn(SCTP_SOCK(sp)->sport);
}
STATIC INLINE uint sctp_sp_phashfn(struct sctp_opt * sp)
{
	return sctp_phashfn(sp->p_tag);
}
STATIC INLINE uint sctp_sp_cachefn(struct sctp_opt * sp)
{
	return (sp->hashent = sctp_cachefn(sp->v_tag));
}
STATIC INLINE uint sctp_sp_vhashfn(struct sctp_opt * sp)
{
	return sctp_vhashfn(sp->v_tag);
}
STATIC INLINE uint sctp_sp_thashfn(struct sctp_opt * sp)
{
	return sctp_thashfn(SCTP_SOCK(sp)->sport, SCTP_SOCK(sp)->dport);
}
STATIC void __sctp_lhash_insert(struct sctp_opt *sp)
{
	struct sctp_hash_bucket *hp = &sctp_lhash[sctp_sp_lhashfn(sp)];
	printd(("INFO: Adding socket %p to Listen hashes\n", SCTP_SOCK(sp)));
	write_lock(&hp->lock);
	if (!sp->lprev) {
		if ((sp->lnext = hp->list))
			sp->lnext->lprev = &sp->lnext;
		sp->lprev = &hp->list;
		hp->list = sp;
	} else
		swerr();
	write_unlock(&hp->lock);
}
STATIC void __sctp_phash_insert(struct sctp_opt *sp)
{
	struct sctp_hash_bucket *hp = &sctp_phash[sctp_sp_phashfn(sp)];
	printd(("INFO: Adding socket %p to Peer Tag hashes\n", SCTP_SOCK(sp)));
	write_lock(&hp->lock);
	if (!sp->pprev) {
		if ((sp->pnext = hp->list))
			sp->pnext->pprev = &sp->pnext;
		sp->pprev = &hp->list;
		hp->list = sp;
	} else
		swerr();
	write_unlock(&hp->lock);
}
STATIC void __sctp_vhash_insert(struct sctp_opt *sp)
{
	struct sctp_hash_bucket *hp = &sctp_vhash[sctp_sp_vhashfn(sp)];
	struct sctp_hash_bucket *cp = &sctp_cache[sctp_sp_cachefn(sp)];
	printd(("INFO: Adding socket %p to Verification Tag hashes\n",
		SCTP_SOCK(sp)));
	write_lock(&hp->lock);
	if (!sp->pprev) {
		if ((sp->vnext = hp->list))
			sp->vnext->vprev = &sp->vnext;
		sp->vprev = &hp->list;
		hp->list = sp;
		cp->list = sp;
	} else
		swerr();
	write_unlock(&hp->lock);
}
STATIC void ___sctp_thash_insert(struct sctp_opt *sp, struct sctp_hash_bucket *hp)
{
	printd(("INFO: Adding socket %p to TCB hashes\n", SCTP_SOCK(sp)));
	if (!sp->tprev) {
		if ((sp->tnext = hp->list))
			sp->tnext->tprev = &sp->tnext;
		sp->tprev = &hp->list;
		hp->list = sp;
	} else
		swerr();
}
STATIC void __sctp_thash_insert(struct sctp_opt *sp)
{
	struct sctp_hash_bucket *hp = &sctp_thash[sctp_sp_thashfn(sp)];
	write_lock(&hp->lock);
	___sctp_thash_insert(sp, hp);
	write_unlock(&hp->lock);
}
STATIC void __sctp_lhash_unhash(struct sctp_opt *sp)
{
	struct sctp_hash_bucket *hp = &sctp_lhash[sctp_sp_lhashfn(sp)];
	printd(("INFO: Removing socket %p from Listen hashes\n", SCTP_SOCK(sp)));
	if (sp->lprev) {
		write_lock(&hp->lock);
		if ((*(sp->lprev) = sp->lnext))
			sp->lnext->lprev = sp->lprev;
		sp->lnext = NULL;
		sp->lprev = NULL;
		write_unlock(&hp->lock);
	} else
		swerr();
}
STATIC void __sctp_phash_unhash(struct sctp_opt *sp)
{
	struct sctp_hash_bucket *hp = &sctp_phash[sctp_sp_phashfn(sp)];
	printd(("INFO: Removing socket %p from Peer Tag hashes\n", SCTP_SOCK(sp)));
	if (sp->pprev) {
		write_lock(&hp->lock);
		if ((*(sp->pprev) = sp->pnext))
			sp->pnext->pprev = sp->pprev;
		sp->pnext = NULL;
		sp->pprev = NULL;
		write_unlock(&hp->lock);
	} else
		swerr();
}
STATIC void __sctp_vhash_unhash(struct sctp_opt *sp)
{
	struct sctp_hash_bucket *hp = &sctp_vhash[sctp_sp_vhashfn(sp)];
	printd(("INFO: Removing socket %p from Verification Tag hashes\n",
		SCTP_SOCK(sp)));
	if (sp->vprev) {
		write_lock(&hp->lock);
		if ((*(sp->vprev) = sp->vnext))
			sp->vnext->vprev = sp->vprev;
		sp->vnext = NULL;
		sp->vprev = NULL;
		if (sctp_cache[sp->hashent].list == sp)
			sctp_cache[sp->hashent].list = NULL;
		sp->hashent = 0;
		write_unlock(&hp->lock);
	} else
		swerr();
}
STATIC void __sctp_thash_unhash(struct sctp_opt *sp)
{
	struct sctp_hash_bucket *hp = &sctp_thash[sctp_sp_thashfn(sp)];
	printd(("INFO: Removing socket %p from TCB hashes\n", SCTP_SOCK(sp)));
	if (sp->tprev) {
		write_lock(&hp->lock);
		if ((*(sp->tprev) = sp->tnext))
			sp->tnext->tprev = sp->tprev;
		sp->tnext = NULL;
		sp->tprev = NULL;
		write_unlock(&hp->lock);
	} else
		swerr();
}
STATIC void __sctp_bindb_get(unsigned short snum)
{
	struct sctp_bhash_bucket *hp = &sctp_bhash[sctp_bhashfn(snum)];
	(void) hp;
	write_lock(&hp->lock);
}
STATIC void __sctp_bindb_put(unsigned short snum)
{
	struct sctp_bhash_bucket *hp = &sctp_bhash[sctp_bhashfn(snum)];
	(void) hp;
	write_unlock(&hp->lock);
}
STATIC struct sctp_bind_bucket *sctp_bindb_get(unsigned short snum)
{
	struct sctp_bind_bucket *sb;
	struct sctp_bhash_bucket *hp = &sctp_bhash[sctp_bhashfn(snum)];
	write_lock_bh(&hp->lock);
	printd(("INFO: Getting bind bucket for port = %d\n", snum));
	for (sb = hp->list; sb && sb->port != snum; sb = sb->next);
	return (sb);
}
STATIC void sctp_bindb_put(unsigned short snum)
{
	struct sctp_bhash_bucket *hp = &sctp_bhash[sctp_bhashfn(snum)];
	(void) hp;
	printd(("INFO: Putting bind bucket for port = %d\n", snum));
	write_unlock_bh(&hp->lock);
}
STATIC INLINE struct sctp_bind_bucket *__sctp_bindb_create(unsigned short snum)
{
	struct sctp_bind_bucket *sb;
	if ((sb = kmem_cache_alloc(sctp_bind_cachep, SLAB_ATOMIC))) {
		struct sctp_bhash_bucket *hp = &sctp_bhash[sctp_bhashfn(snum)];
		printd(("INFO: Allocating bind bucket for port = %d\n", snum));
		memset(sb, 0, sizeof(*sb));
		sb->port = snum;
		sb->fastreuse = 1;
		if ((sb->next = hp->list))
			sb->next->prev = &sb->next;
		sb->prev = &hp->list;
		hp->list = sb;
	}
	return (sb);
}
STATIC void __sctp_bhash_insert(struct sctp_opt *sp, struct sctp_bind_bucket *sb)
{
	struct sock *sk = SCTP_SOCK(sp);
	if (!sp->bprev) {
		printd(("INFO: Adding socket %p to bind bucket at port = %d\n",
			sk, sb->port));
		if ((sp->bnext = sb->owners))
			sp->bnext->bprev = &sp->bnext;
		sp->bprev = &sb->owners;
		sb->owners = sp;
		sp->bindb = sb;
		sk->num = sb->port;
	} else if (sk->state == SCTP_LISTEN) {
		printd(("INFO: Re-adding listening socket %p to bind bucket at port = %d\n", sk, sb->port));
	} else
		swerr();
	if (!sk->reuse || sk->state == SCTP_LISTEN)
		sb->fastreuse = 0;
}

STATIC void __sctp_inherit_port(struct sock *sk, struct sock *lsk)
{
	struct sctp_bind_bucket *sb;
	sk->num = lsk->num;
	__sctp_bindb_get(sk->num);
	if ((sb = SCTP_PROT(lsk)->bindb))
		__sctp_bhash_insert(SCTP_PROT(sk), sb);
	else
		swerr();
	__sctp_bindb_put(sk->num);
}
STATIC void __sctp_bhash_unhash(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_bhash_bucket *bp = &sctp_bhash[sctp_sk_bhashfn(sk)];
	write_lock(&bp->lock);
	if (sp->bprev) {
		printd(("INFO: Removing socket %p from bind bucket at port = %d, num = %d\n", sk, sp->bindb ? sp->bindb->port : -1U, sk->num));
		if ((*(sp->bprev) = sp->bnext))
			sp->bnext->bprev = sp->bprev;
		sp->bnext = NULL;
		sp->bprev = NULL;
		if (sp->bindb) {
			if (sp->bindb->owners == NULL) {
				struct sctp_bind_bucket *sb = sp->bindb;
				printd(("INFO: Deallocating bind bucket for port = %d\n", sb->port));
				if (sb->prev) {
					if ((*(sb->prev) = sb->next))
						sb->next->prev = sb->prev;
					sb->next = NULL;
					sb->prev = NULL;
					memset(sb, 0, sizeof(*sb));	/* debug */
					kmem_cache_free(sctp_bind_cachep, sb);
				} else
					swerr();
			}
			sp->bindb = NULL;
		} else
			swerr();
	} else
		swerr();
	sk->num = 0;
	write_unlock(&bp->lock);
}

/*
 *  Add to listening Hashes
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This also checks for conflicts.  This uses the rule that no two sockets
 *  can be listening on the same primary transport address (IP/port).  It is a
 *  question whether whether a socket which is bound to multiple addresses
 *  using bind(2) and then calls listen(2) should be viewed as listening on
 *  the primary address or should be viewed as listening on all addresses.
 *  The xinetd(8) will probably want to listen on INADDR_ANY for specific port
 *  numbers.
 */

/*
 *  Get Local Addresses
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Get all the available local addresses and build the source address list
 *  from those that are available.
 */
STATIC int __sctp_get_addrs(struct sock *sk, uint32_t daddr)
{
	int allocated = 0;
	struct net_device *dev;
	read_lock(&dev_base_lock);
	read_lock(&inetdev_lock);
	for (dev = dev_base; dev; dev = dev->next) {
		struct in_device *in_dev;
		struct in_ifaddr *ifa;
		if (!(in_dev = __in_dev_get(dev)))
			continue;
		read_lock(&in_dev->lock);
		/* get primary or secondary addresses for each interface */
		for (ifa = in_dev->ifa_list; ifa; ifa = ifa->ifa_next) {
			int err = 0;
			if (LOOPBACK(ifa->ifa_local) != LOOPBACK(daddr))
				continue;
#if 0
			if (ifa->ifa_scope != RT_SCOPE_HOST)
				continue;
#endif
			if (sctp_saddr_include(sk, ifa->ifa_local, &err))
				allocated++;
		}
		read_unlock(&in_dev->lock);
	}
	read_unlock(&inetdev_lock);
	read_unlock(&dev_base_lock);
	return (allocated == 0);
}
STATIC int sctp_get_addrs(struct sock *sk, uint32_t daddr)
{
	int ret;
	local_bh_disable();
	ret = __sctp_get_addrs(sk, daddr);
	local_bh_enable();
	return (ret);
}

/*
 *  Add to Connection Hashes
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This also checks for conflicts.  This uses the rule that there cannot be
 *  more than one SCTP association between an given pair of local and remote
 *  transport addresses (IP/port).
 */
STATIC int sctp_conn_hash(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk), *sp2;
	struct sctp_hash_bucket *hp;
	struct sock *sk2;
	struct sctp_saddr *ss, *ss2, *ss_next;
	struct sctp_daddr *sd, *sd2;
	assert(sk);
	local_bh_disable();
#ifdef CONFIG_SCTP_DEBUG
	if (sp->tprev) {
		swerr();
		__sctp_thash_unhash(sp);
	}
	if (sp->vprev) {
		swerr();
		__sctp_vhash_unhash(sp);
	}
	if (sp->pprev) {
		swerr();
		__sctp_phash_unhash(sp);
	}
#endif
	/* 
	 *  Check for conflicts: we search the TCB hash list for other sockets
	 *  with the same port pair and with the same pairing of src and dst
	 *  addresses.  We do this with write locks on for the hash list so
	 *  that we can add our socket if we succeed.
	 *
	 *  This is the TCB hash, so even if this is a long search, we are
	 *  only blocking other connection hash calls and TCB lookups (for
	 *  ootb packets) for this hash bucket.
	 */
	hp = &sctp_thash[sctp_sp_thashfn(sp)];
	write_lock(&hp->lock);
	for (sp2 = hp->list; sp2; sp2 = sp2->tnext) {
		sk2 = SCTP_SOCK(sp2);
		if (sk2->sport != sk->sport || sk2->dport != sk->dport)
			continue;
		ss_next = sp->saddr;
		while ((ss = ss_next)) {
			ss_next = ss->next;
			for (ss2 = sp2->saddr; ss2; ss2 = ss2->next) {
				if (ss->saddr != ss2->saddr)
					continue;
				for (sd = sp->daddr; sd; sd = sd->next) {
					for (sd2 = sp2->daddr; sd2; sd2 = sd2->next) {
						if (sd->daddr != sd2->daddr)
							continue;
						/* matching daddr */
/* 
 *  Now, if we have a matching saddr/sport/daddr/dport n-tuple, then we should
 *  check if we are SOCK_BINDADDR_LOCK'd.  If we are not, then we were bound
 *  to INADDR_ANY and (for connect) we are permitted to kick addresses out of
 *  the source address list to resolve conflicts.
 */
						if (!(sk->userlocks &
						      SOCK_BINDADDR_LOCK)) {
							__sctp_saddr_free(ss);
							goto next_saddr;
						}
						goto eaddrinuse;
					}
				}
			}
		      next_saddr:
			continue;
		}
	}
	/* Of course, if we kick all the source addresses out of the list, then we
	   still have a conflict.  */
	if (!(sk->userlocks & SOCK_BINDADDR_LOCK) && !sp->saddr) {
		rare();
		goto eaddrinuse;
	}
	___sctp_thash_insert(sp, hp);
	write_unlock(&hp->lock);
	__sctp_vhash_insert(sp);
	__sctp_phash_insert(sp);
	local_bh_enable();
	return (0);
      eaddrinuse:
	write_unlock(&hp->lock);
	local_bh_enable();
	return (-EADDRINUSE);	/* conflict */
}

/*
 *  =========================================================================
 *
 *  LOOKUP Functions
 *
 *  =========================================================================
 *
 *  A fast caching hashing lookup function for SCTP.
 *
 *  IMPLEMENTATION NOTES:- All but a few SCTP messages carry our Verification
 *  Tag.  If the message requires our Verification Tag and we cannot lookup
 *  the stream on the Verification Tag we treat the packet similar to an OOTB
 *  packet.  The only restriction that this approach imposes is in the
 *  selection of our Verification Tag, which cannot be identical to any other
 *  Verification Tag which we have chosen so far.  We, therefore, check the
 *  Verification Tag selected at initialization against the cache for
 *  uniqueness.  This also allows us to acquire the Verification Tag to
 *  minimize collisions on the hash table.  This allows us to simplify the
 *  hashing function because we are guaranteeing equal hash coverage using
 *  selection.
 */
/*
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP LISTEN - LISTEN hash (sport)
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  Listener stream lookup with wildcards.  This will find any bound listener
 *  stream from the destination address, destination port and device index.
 *  This is only for INIT and COOKIE ECHO.
 */
STATIC struct sock *sctp_lookup_listen(uint16_t dport, uint32_t daddr)
{
	struct sctp_opt *sp, *result = NULL;
	int hiscore = 0;
	struct sctp_hash_bucket *hp = &sctp_lhash[sctp_lhashfn(dport)];
	read_lock(&hp->lock);
	for (sp = hp->list; sp; sp = sp->lnext) {
		struct sock *sk = SCTP_SOCK(sp);
		int score = 0;
		if (sk->sport) {
			if (sk->sport != dport)
				continue;
			score++;
			if (sp->saddr) {
				struct sctp_saddr *ss;
				if (!(ss = sctp_find_saddr(sk, daddr)))
					continue;
				if (ss->flags & SCTP_SRCEM_ASCONF)
					continue;
				score++;
			}
		} else {
			ptrace(("ERROR: Unassigned port number socket = %p\n", sk));
		}
		if (score == 2) {
			result = sp;
			break;
		}
		if (score > hiscore) {
			hiscore = score;
			result = sp;
		}
	}
	read_unlock(&hp->lock);
	usual(result);
	if (result) {
		sock_hold(SCTP_SOCK(result));
		return SCTP_SOCK(result);
	}
	return NULL;
}

/*
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP BIND - BIND hash (port)
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  Like the listen lookup, but done on the bind hashes.  This is to find
 *  potential conflicts for INIT.  We are just looking for a non-closed scoket
 *  bound to the port with a compatible destination address considering
 *  wildcards.  This is used for exceptional INIT cases when no listening socket
 *  is found.  Port locks must have been taken on the bind (the port must have
 *  been specified by the user and not dynamically assigned otherwise the socket
 *  will not accept INITs.
 */
STATIC struct sock *sctp_lookup_bind(uint16_t dport, uint32_t daddr)
{
	struct sctp_opt *result = NULL;
	int snum = ntohs(dport);
	struct sctp_bind_bucket *sb;
	struct sctp_bhash_bucket *hp = &sctp_bhash[sctp_bhashfn(snum)];
	read_lock(&hp->lock);
	for (sb = hp->list; sb && sb->port != snum; sb = sb->next);
	if (sb) {
		struct sctp_opt *sp;
		int hiscore = 0;
		for (sp = sb->owners; sp; sp = sp->bnext) {
			struct sock *sk = SCTP_SOCK(sp);
			int score = 0;
			if ((1 << sk->state) & (SCTPF_DISCONNECTED))
				continue;
			if (!(sk->userlocks & SOCK_BINDPORT_LOCK))
				continue;
			if (sk->sport) {
				if (sk->sport != dport)
					continue;
				score++;
				if (sp->saddr) {
					struct sctp_saddr *ss;
					if (!(ss = sctp_find_saddr(sk, daddr)))
						continue;
					if (ss->flags & SCTP_SRCEM_ASCONF)
						continue;
					score++;
				}
			} else {
				ptrace(("ERROR: Unassigned port number socket = %p\n", sk));
			}
			if (score > 0) {
				result = sp;
				break;
			}
			if (score > hiscore) {
				hiscore = score;
				result = sp;
			}
		}
	}
	read_unlock(&hp->lock);
	usual(result);
	if (result) {
		sock_hold(SCTP_SOCK(result));
		return SCTP_SOCK(result);
	}
	return NULL;
}

/*
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP - TCB hash (port pair)
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  We do the hard match here, because we don't have valid v_tags or p_tags.
 *  We don't have the information anyways.  We are just looking for an
 *  established stream which can accept the packet based on port numbers and
 *  source and destination addresses.  This is used for INIT, OOTB determination
 *  as well as ICMP lookups for failed sent INIT messages.
 *
 */
#define sctp_match_tcb(__sk, __saddr, __daddr, __sport, __dport) \
	( ((__sport) == (__sk)->sport) && \
	  ((__dport) == (__sk)->dport) && \
	  (sctp_find_saddr((__sk),(__saddr))) && \
	  (sctp_find_daddr((__sk),(__daddr))) )

STATIC struct sock *sctp_lookup_tcb(uint16_t sport, uint16_t dport,
				    uint32_t saddr, uint32_t daddr)
{
	struct sctp_opt *sp;
	struct sctp_hash_bucket *hp = &sctp_thash[sctp_thashfn(sport, dport)];
	read_lock(&hp->lock);
	for (sp = hp->list; sp; sp = sp->tnext)
		if (sctp_match_tcb(SCTP_SOCK(sp), saddr, daddr, sport, dport))
			break;
	read_unlock(&hp->lock);
	if (sp) {
		sock_hold(SCTP_SOCK(sp));
		return SCTP_SOCK(sp);
	}
	return NULL;
}

/*
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP PTAG - Peer hash (peer tag)
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  We can either match fast and loose or slow and sure.  We have a peer tag
 *  which is validation enough without walking the address lists most of the
 *  time.  The INIT and COOKIE ECHO stuff needs it for checking peer tags and
 *  peer tie tags.  ICMP needs this for looking up all packets which were
 *  returned that we sent out with the peer's tag (excepts INIT which has no
 *  tag, we use TCB ICMP lookup for that).  ICMP lookups match with reversed
 *  source and destination addresses.
 *
 *  ADD-IP must ignore addresses at times (when retransmitting an ASCONF which
 *  successfully deleted an IP address but for which the ASCONF ACK was lost).
 *  Therefore, we always use just verification tag and port for ADD-IP
 *  regardless of the slow verification setting.
 */
#if !defined(CONFIG_SCTP_SLOW_VERIFICATION) || defined(CONFIG_SCTP_ADD_IP)
#define	sctp_match_ptag(__sk, __saddr, __daddr, __p_tag, __sport, __dport) \
	( ((__p_tag) == SCTP_PROT(__sk)->p_tag) && \
	  ((__sport) == (__sk)->sport) && \
	  ((__dport) == (__sk)->dport) )
#else
#define	sctp_match_ptag(__sk, __saddr, __daddr, __p_tag, __sport, __dport) \
	( ((__p_tag) == SCTP_PROT(__sk)->p_tag) && \
	  ((__sport) == (__sk)->sport) && \
	  ((__dport) == (__sk)->dport) && \
	  (sctp_find_saddr((__sk),(__saddr))) && \
	  (sctp_find_daddr((__sk),(__daddr))) )
#endif

STATIC struct sock *sctp_lookup_ptag(uint32_t p_tag, uint16_t sport,
				     uint16_t dport, uint32_t saddr, uint16_t daddr)
{
	struct sctp_opt *sp;
	struct sctp_hash_bucket *hp = &sctp_phash[sctp_phashfn(p_tag)];
	(void) saddr;
	(void) daddr;
	read_lock(&hp->lock);
	for (sp = hp->list; sp; sp = sp->pnext)
		if (sctp_match_ptag
		    (SCTP_SOCK(sp), saddr, daddr, p_tag, sport, dport))
			break;
	read_unlock(&hp->lock);
	if (sp) {
		sock_hold(SCTP_SOCK(sp));
		return SCTP_SOCK(sp);
	}
	return NULL;
}

/*
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP VTAG - Established hash (local tag)
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  This is the main lookup for data transfer on established streams.  This
 *  should run as fast and furious as possible.  We run fast and loose and
 *  rely on the verification tag only.  We cache and hash so a stream of
 *  back-to-back packets to the same destination (bursty traffic) will whirl.
 *  Because we MD4 hash verification tags when we generate them, the hash
 *  should get good random distribution with minimum collisions.
 *
 *  ADD-IP must ignore addresses at times (when retransmitting an ASCONF which
 *  successfully deleted an IP address but for which the ASCONF ACK was lost).
 *  Therefore, we always use just verification tag for ADD-IP regardless of
 *  the slow verification setting.
 *
 */
#if !defined(CONFIG_SCTP_SLOW_VERIFICATION) || defined(CONFIG_SCTP_ADD_IP)
#define sctp_match_vtag(__sk, __saddr, __daddr, __v_tag, __sport, __dport) \
	( ((__v_tag) == SCTP_PROT(__sk)->v_tag) )
#else
#define sctp_match_vtag(__sk, __saddr, __daddr, __v_tag, __sport, __dport) \
	( ((__v_tag) == SCTP_PROT(__sk)->v_tag) && \
	  ((__sport) == (__sk)->sport) && \
	  ((__dport) == (__sk)->dport) && \
	  (sctp_find_saddr((__sk),(__daddr))) )
#endif

STATIC struct sock *sctp_lookup_vtag(uint32_t v_tag, uint16_t sport,
				     uint16_t dport, uint32_t saddr, uint16_t daddr)
{
	struct sctp_opt *sp;
	unsigned int hash = sctp_cachefn(v_tag);
	struct sctp_hash_bucket *hp = &sctp_vhash[sctp_vhashfn(v_tag)];
	(void) saddr;
	(void) daddr;
	read_lock(&hp->lock);
	if (!(sp = sctp_cache[hash].list) ||
	    !sctp_match_vtag(SCTP_SOCK(sp), saddr, daddr, v_tag, sport, dport)) {
		for (sp = hp->list; sp; sp = sp->vnext)
			if (sctp_match_vtag
			    (SCTP_SOCK(sp), saddr, daddr, v_tag, sport, dport)) {
				sctp_cache[hash].list = sp;
				break;
			}
	}
	read_unlock(&hp->lock);
	if (sp) {
		sock_hold(SCTP_SOCK(sp));
		return SCTP_SOCK(sp);
	}
	return NULL;
}

/*
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP COOKIE ECHO - Special lookup rules for cookie echo chunks
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
/* *INDENT-OFF* */
STATIC struct sock *sctp_lookup_cookie_echo(struct sctp_cookie *ck, uint32_t v_tag,
					    uint16_t sport, uint16_t dport,
					    uint32_t daddr, uint32_t saddr)
{
	struct sock *sk = NULL;
	/* quick sanity checks on cookie */
	if (ck->v_tag == v_tag && ck->sport == sport && ck->dport == dport) {
		if (		/* RFC 2960 5.2.4 (A) */
		   (ck->l_ttag && ck->p_ttag
		    && (sk = sctp_lookup_vtag(ck->l_ttag, sport, dport, saddr, daddr))
		    && sk == sctp_lookup_ptag(ck->p_ttag, sport, dport, saddr, daddr))
				/* RFC 2960 5.2.4 (B) or (D) */
		   || ((sk = sctp_lookup_vtag(v_tag, sport, dport, saddr, daddr)))
				/* RFC 2960 5.2.4 (C) or IG 2.6 replacement */
		   || (!ck->l_ttag && !ck->p_ttag
		       && ((sk = sctp_lookup_ptag(ck->p_tag, sport, dport, saddr, daddr))
			   || (sk = sctp_lookup_tcb(sport, dport, saddr, ck->daddr))))
				/* RFC 2960 5.1 */
		   || ((sk = sctp_lookup_listen(sport, daddr))))
			if (sk->state == SCTP_LISTEN || (sk->userlocks & SOCK_BINDPORT_LOCK))
				return (sk);
	} else {
		rare();
		if (ck->v_tag != v_tag)
			printd(("INFO: cookie v_tag = %08X, header v_tag = %08X\n", ck->v_tag, v_tag));
		if (ck->sport != sport)
			printd(("INFO: cookie sport = %08X, header sport = %08X\n", ck->sport, sport));
		if (ck->dport != dport)
			printd(("INFO: cookie dport = %08X, header dport = %08X\n", ck->dport, dport));
	}
	seldom();
	return (NULL);
}
/* *INDENT-ON* */

/*
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  LOOKUP - Established hash (local verification tag with fallbacks)
 *
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  Stream hash lookup with fast path for data.  This uses verification tag
 *  when it is available.  Source address and port are checked after the
 *  verification tag matches.  When called for INIT and COOKIE ECHO messages,
 *  the function returns a listening (bind) hash lookup.  For SHUTDOWN
 *  COMPLETE and ABORT messages with the T-bit set, an icmp (peer tag) lookup
 *  is performed instead.
 *
 *  Note: SCTP IG 2.18 is unnecessary.
 */
STATIC struct sock *sctp_lookup(struct sctphdr *sh, uint32_t daddr, uint32_t saddr)
{
	struct sock *sk = NULL;
	struct sctpchdr *ch = (struct sctpchdr *) (sh + 1);
	int ctype = ch->type;
	uint32_t v_tag = sh->v_tag;
	uint16_t dport = sh->dest;
	uint16_t sport = sh->srce;
	if (v_tag) {
		/* fast path */
		if (ctype == SCTP_CTYPE_SACK || ctype == SCTP_CTYPE_DATA)
			return sctp_lookup_vtag(v_tag, dport, sport, daddr, saddr);
		switch (ctype) {
			/* See RFC 2960 Section 8.5.1 */
		case SCTP_CTYPE_ABORT:
		case SCTP_CTYPE_SHUTDOWN_COMPLETE:
			/* SCTP IG 2.13 */
			if (ch->flags & 0x1)	/* T bit set */
				return sctp_lookup_ptag(v_tag, dport, sport,
							daddr, saddr);
		default:
			if ((sk =
			     sctp_lookup_vtag(v_tag, dport, sport, daddr, saddr)))
				return (sk);
			if (ctype == SCTP_CTYPE_ABORT)
				/* check abort for conn ind */
				if ((sk = sctp_lookup_listen(dport, daddr)))
					return (sk);
		case SCTP_CTYPE_INIT:
			break;
		case SCTP_CTYPE_COOKIE_ECHO:{
			struct sctp_cookie *ck = (struct sctp_cookie *)
			    ((struct sctp_cookie_echo *) ch)->cookie;
			return sctp_lookup_cookie_echo(ck, v_tag, dport, sport,
						       daddr, saddr);
		}
		}
	} else if (ctype == SCTP_CTYPE_INIT)
		if ((sk = sctp_lookup_listen(dport, daddr)) ||
		    (sk = sctp_lookup_bind(dport, daddr)))
			return (sk);
	seldom();
	return (NULL);
}

/*
 *  =========================================================================
 *
 *  COOKIE HANDLING and SECURITY
 *
 *  =========================================================================
 */
extern uint32_t secure_tcp_sequence_number(uint32_t, uint32_t, uint16_t, uint16_t);
STATIC uint32_t sctp_get_vtag(uint32_t daddr, uint32_t saddr, uint16_t dport,
			      uint16_t sport)
{
	uint32_t ret;
	while (!(ret = secure_tcp_sequence_number(daddr, saddr, dport, sport)) ||
	       sctp_lookup_vtag(ret, sport, dport, saddr, daddr));
	return (ret);
}

#define HMAC_SIZE (160/8)
#define raw_cookie_size(__ck) \
		( sizeof(struct sctp_cookie) \
		+ ((__ck)->opt_len) \
		+ ((__ck)->danum * sizeof(uint32_t)) \
		+ ((__ck)->sanum * sizeof(uint32_t)) )
/*
 *  =========================================================================
 *
 *  SHA-1
 *
 *  =========================================================================
 *  
 *  This is a GPL version fo the FIPS 180-1 Secure Hash Algorithm written by
 *  Brian Bidulock <bidulock@openss7.org>.
 *
 *  Adapted from code written November 2000 by David Ireland of DI Management
 *  Services Pty Limited <code@di-mgt.com.au>
 *  
 *  Adapted from code in the Python Cryptography Toolkit, version 1.0.0 by
 *  A.M.  Kuchling 1995.
 */
#ifdef CONFIG_SCTP_HMAC_SHA1
/*
 *  The structure for storing SHS info
 */
typedef struct {
	uint32_t dig[5];		/* Message digest */
	uint32_t lo, hi;		/* 64-bit bit count */
	uint32_t dat[16];		/* SHS data buffer */
} SHA_CTX;
/*
 *  The SHS f()-functions.  The f1 and f3 functions can be optimized to save one
 *  boolean operation each - thanks to Rich Schroeppel, rcs@cs.arizona.edu for
 *  discovering this
 */
#define f1(x,y,z)   (z^(x&(y^z)))	/* Rounds 0-19 */
#define f2(x,y,z)   (x^y^z)	/* Rounds 20-39 */
#define f3(x,y,z)   ((x&y)|(z&(x|y)))	/* Rounds 40-59 */
#define f4(x,y,z)   (x^y^z)	/* Rounds 60-79 */
/*
 *  32-bit rotate left - kludged with shifts
 */
#define ROTL(n,X)   (((X)<<n)|((X)>>(32-n)))
/*
 *  The initial expanding function.  The hash function is defined over an
 *  80-UINT2 expanded input array W, where the first 16 are copies of the input
 *  data, and the remaining 64 are defined by
 *
 *	W[i] = W[i - 16] ^ W[i - 14] ^ W[i - 8] ^ W[i - 3]
 *
 *  This implementation generates these values on the fly in a circular buffer -
 *  thanks to Colin Plumb, colin@nyx10.cs.du.edu for this optimization.
 *
 *  The updated SHS changes the expanding function by adding a rotate of 1 bit.
 *  Thanks to Jim Gillogly, jim@rand.org, and an anonymous contributor for this
 *  information
 */
#define x(W,i) \
	(W[i & 15] = \
	  ROTL(1, (W[i&15] ^ W[(i-14)&15] ^ W[(i-8)&15] ^ W[(i-3)&15])))
/*
 *  The prototype SHS sub-round.  The fundamental sub-round is:
 *
 *	a' = e + ROTL(5, a) + f(b, c, d) + k + data;
 *	b' = a;
 *	c' = ROTL(30, b);
 *	d' = c;
 *	e' = d;
 *
 *  but this is implemented by unrolling the loop 5 times and renaming the
 *  variables (e, a, b, c, d) = (a', b', c', d', e') each iteration.  This
 *  code is then replicated 20 times for each of the 4 functions, using the next
 *  20 values from the W[] array each time
 */
#define subRound(a,b,c,d,e,f,k,data) \
    (e += ROTL(5,a)+f(b,c,d)+k+data, b = ROTL(30,b))
/*
 *  Initialize the SHS values
 */
STATIC void SHAInit(SHA_CTX * sha1)
{
	/* Set the h-vars to their initial values */
	sha1->dig[0] = 0x67452301L;
	sha1->dig[1] = 0xefcdab89L;
	sha1->dig[2] = 0x98badcfeL;
	sha1->dig[3] = 0x10325476L;
	sha1->dig[4] = 0xc3d2e1f0L;
	/* Initialise bit count */
	sha1->lo = sha1->hi = 0;
}

/*
 *  Perform the SHS transformation.  Note that this code, like MD5, seems to
 *  break some optimizing compilers due to the complexity of the expressions and
 *  the size of the basic block.  It may be necessary to split it into sections,
 *  e.g. based on the four subrounds
 *
 *  Note that this corrupts the sha1->data area
 */
STATIC void SHSTransform(uint32_t * dig, uint32_t * dat)
{
	uint32_t A, B, C, D, E;		/* Local vars */
	uint32_t xd[16];		/* Expanded data */
	/* Set up first buffer and local data buffer */
	A = dig[0];
	B = dig[1];
	C = dig[2];
	D = dig[3];
	E = dig[4];
	memcpy(xd, dat, 64);
	/* Heavy mangling, in 4 sub-rounds of 20 interations each. */
	subRound(A, B, C, D, E, f1, 0x5a827999L, xd[0]);
	subRound(E, A, B, C, D, f1, 0x5a827999L, xd[1]);
	subRound(D, E, A, B, C, f1, 0x5a827999L, xd[2]);
	subRound(C, D, E, A, B, f1, 0x5a827999L, xd[3]);
	subRound(B, C, D, E, A, f1, 0x5a827999L, xd[4]);
	subRound(A, B, C, D, E, f1, 0x5a827999L, xd[5]);
	subRound(E, A, B, C, D, f1, 0x5a827999L, xd[6]);
	subRound(D, E, A, B, C, f1, 0x5a827999L, xd[7]);
	subRound(C, D, E, A, B, f1, 0x5a827999L, xd[8]);
	subRound(B, C, D, E, A, f1, 0x5a827999L, xd[9]);
	subRound(A, B, C, D, E, f1, 0x5a827999L, xd[10]);
	subRound(E, A, B, C, D, f1, 0x5a827999L, xd[11]);
	subRound(D, E, A, B, C, f1, 0x5a827999L, xd[12]);
	subRound(C, D, E, A, B, f1, 0x5a827999L, xd[13]);
	subRound(B, C, D, E, A, f1, 0x5a827999L, xd[14]);
	subRound(A, B, C, D, E, f1, 0x5a827999L, xd[15]);
	subRound(E, A, B, C, D, f1, 0x5a827999L, x(xd, 16));
	subRound(D, E, A, B, C, f1, 0x5a827999L, x(xd, 17));
	subRound(C, D, E, A, B, f1, 0x5a827999L, x(xd, 18));
	subRound(B, C, D, E, A, f1, 0x5a827999L, x(xd, 19));
	subRound(A, B, C, D, E, f2, 0x63d9eba1L, x(xd, 20));
	subRound(E, A, B, C, D, f2, 0x63d9eba1L, x(xd, 21));
	subRound(D, E, A, B, C, f2, 0x63d9eba1L, x(xd, 22));
	subRound(C, D, E, A, B, f2, 0x63d9eba1L, x(xd, 23));
	subRound(B, C, D, E, A, f2, 0x63d9eba1L, x(xd, 24));
	subRound(A, B, C, D, E, f2, 0x63d9eba1L, x(xd, 25));
	subRound(E, A, B, C, D, f2, 0x63d9eba1L, x(xd, 26));
	subRound(D, E, A, B, C, f2, 0x63d9eba1L, x(xd, 27));
	subRound(C, D, E, A, B, f2, 0x63d9eba1L, x(xd, 28));
	subRound(B, C, D, E, A, f2, 0x63d9eba1L, x(xd, 29));
	subRound(A, B, C, D, E, f2, 0x63d9eba1L, x(xd, 30));
	subRound(E, A, B, C, D, f2, 0x63d9eba1L, x(xd, 31));
	subRound(D, E, A, B, C, f2, 0x63d9eba1L, x(xd, 32));
	subRound(C, D, E, A, B, f2, 0x63d9eba1L, x(xd, 33));
	subRound(B, C, D, E, A, f2, 0x63d9eba1L, x(xd, 34));
	subRound(A, B, C, D, E, f2, 0x63d9eba1L, x(xd, 35));
	subRound(E, A, B, C, D, f2, 0x63d9eba1L, x(xd, 36));
	subRound(D, E, A, B, C, f2, 0x63d9eba1L, x(xd, 37));
	subRound(C, D, E, A, B, f2, 0x63d9eba1L, x(xd, 38));
	subRound(B, C, D, E, A, f2, 0x63d9eba1L, x(xd, 39));
	subRound(A, B, C, D, E, f3, 0x8f1bbcdcL, x(xd, 40));
	subRound(E, A, B, C, D, f3, 0x8f1bbcdcL, x(xd, 41));
	subRound(D, E, A, B, C, f3, 0x8f1bbcdcL, x(xd, 42));
	subRound(C, D, E, A, B, f3, 0x8f1bbcdcL, x(xd, 43));
	subRound(B, C, D, E, A, f3, 0x8f1bbcdcL, x(xd, 44));
	subRound(A, B, C, D, E, f3, 0x8f1bbcdcL, x(xd, 45));
	subRound(E, A, B, C, D, f3, 0x8f1bbcdcL, x(xd, 46));
	subRound(D, E, A, B, C, f3, 0x8f1bbcdcL, x(xd, 47));
	subRound(C, D, E, A, B, f3, 0x8f1bbcdcL, x(xd, 48));
	subRound(B, C, D, E, A, f3, 0x8f1bbcdcL, x(xd, 49));
	subRound(A, B, C, D, E, f3, 0x8f1bbcdcL, x(xd, 50));
	subRound(E, A, B, C, D, f3, 0x8f1bbcdcL, x(xd, 51));
	subRound(D, E, A, B, C, f3, 0x8f1bbcdcL, x(xd, 52));
	subRound(C, D, E, A, B, f3, 0x8f1bbcdcL, x(xd, 53));
	subRound(B, C, D, E, A, f3, 0x8f1bbcdcL, x(xd, 54));
	subRound(A, B, C, D, E, f3, 0x8f1bbcdcL, x(xd, 55));
	subRound(E, A, B, C, D, f3, 0x8f1bbcdcL, x(xd, 56));
	subRound(D, E, A, B, C, f3, 0x8f1bbcdcL, x(xd, 57));
	subRound(C, D, E, A, B, f3, 0x8f1bbcdcL, x(xd, 58));
	subRound(B, C, D, E, A, f3, 0x8f1bbcdcL, x(xd, 59));
	subRound(A, B, C, D, E, f4, 0xca62c1d6L, x(xd, 60));
	subRound(E, A, B, C, D, f4, 0xca62c1d6L, x(xd, 61));
	subRound(D, E, A, B, C, f4, 0xca62c1d6L, x(xd, 62));
	subRound(C, D, E, A, B, f4, 0xca62c1d6L, x(xd, 63));
	subRound(B, C, D, E, A, f4, 0xca62c1d6L, x(xd, 64));
	subRound(A, B, C, D, E, f4, 0xca62c1d6L, x(xd, 65));
	subRound(E, A, B, C, D, f4, 0xca62c1d6L, x(xd, 66));
	subRound(D, E, A, B, C, f4, 0xca62c1d6L, x(xd, 67));
	subRound(C, D, E, A, B, f4, 0xca62c1d6L, x(xd, 68));
	subRound(B, C, D, E, A, f4, 0xca62c1d6L, x(xd, 69));
	subRound(A, B, C, D, E, f4, 0xca62c1d6L, x(xd, 70));
	subRound(E, A, B, C, D, f4, 0xca62c1d6L, x(xd, 71));
	subRound(D, E, A, B, C, f4, 0xca62c1d6L, x(xd, 72));
	subRound(C, D, E, A, B, f4, 0xca62c1d6L, x(xd, 73));
	subRound(B, C, D, E, A, f4, 0xca62c1d6L, x(xd, 74));
	subRound(A, B, C, D, E, f4, 0xca62c1d6L, x(xd, 75));
	subRound(E, A, B, C, D, f4, 0xca62c1d6L, x(xd, 76));
	subRound(D, E, A, B, C, f4, 0xca62c1d6L, x(xd, 77));
	subRound(C, D, E, A, B, f4, 0xca62c1d6L, x(xd, 78));
	subRound(B, C, D, E, A, f4, 0xca62c1d6L, x(xd, 79));
	/* Build message digest */
	dig[0] += A;
	dig[1] += B;
	dig[2] += C;
	dig[3] += D;
	dig[4] += E;
}

/*
 *  When run on a little-endian CPU we need to perform byte reversal on an array
 *  of long words.
 */
#ifdef __LITTLE_ENDIAN
STATIC void longReverse(uint32_t * buf, int cnt)
{
	uint32_t val;
	cnt /= sizeof(uint32_t);
	while (cnt--) {
		val = *buf;
		val = ((val & 0xff00ff00L) >> 8) | ((val & 0x00ff00ffL) << 8);
		*buf++ = (val << 16) | (val >> 16);
	}
}
#else
#define longReverse(__buf, __cnt) do { } while(0)
#endif
/*
 *  Update SHS for a block of data
 */
STATIC void SHAUpdate(SHA_CTX * sha1, uint8_t * buf, int len)
{
	uint32_t tmp;
	int cnt;
	/* Update bitcount */
	tmp = sha1->lo;
	if ((sha1->lo = tmp + ((uint32_t) len << 3)) < tmp)
		sha1->hi++;	/* Carry from low to high */
	sha1->hi += len >> 29;
	/* Get count of bytes already in data */
	cnt = (int) (tmp >> 3) & 0x3F;
	/* Handle any leading odd-sized chunks */
	if (cnt) {
		uint8_t *p = (uint8_t *) sha1->dat + cnt;
		cnt = 64 - cnt;
		if (len < cnt) {
			memcpy(p, buf, len);
			return;
		}
		memcpy(p, buf, cnt);
		longReverse(sha1->dat, 64);
		SHSTransform(sha1->dig, sha1->dat);
		buf += cnt;
		len -= cnt;
	}
	/* Process data in 64 chunks */
	while (len >= 64) {
		memcpy(sha1->dat, buf, 64);
		longReverse(sha1->dat, 64);
		SHSTransform(sha1->dig, sha1->dat);
		buf += 64;
		len -= 64;
	}
	/* Handle any remaining bytes of data. */
	memcpy(sha1->dat, buf, len);
}

/*
 *  Final wrapup - pad to 64-byte boundary with the bit pattern 1 0*
 *  (64-bit count of bits processed, MSB-first)
 */
STATIC void SHAFinal(uint8_t * out, SHA_CTX * sha1)
{
	int len;
	unsigned int i, j;
	uint8_t *dat;
	/* Compute number of bytes mod 64 */
	len = (int) sha1->lo;
	len = (len >> 3) & 0x3F;
	/* 
	 *  Set the first char of padding to 0x80.  This is safe since there is
	 *  always at least one byte free
	 */
	dat = (uint8_t *) sha1->dat + len;
	*dat++ = 0x80;
	/* Bytes of padding needed to make 64 bytes */
	len = 64 - 1 - len;
	/* Pad out to 56 mod 64 */
	if (len < 8) {
		/* Two lots of padding: Pad the first block to 64 bytes */
		memset(dat, 0, len);
		longReverse(sha1->dat, 64);
		SHSTransform(sha1->dig, sha1->dat);
		/* Now fill the next block with 56 bytes */
		memset(sha1->dat, 0, 64 - 8);
	} else
		/* Pad block to 56 bytes */
		memset(dat, 0, len - 8);
	/* Append length in bits and transform */
	sha1->dat[14] = sha1->hi;
	sha1->dat[15] = sha1->lo;
	longReverse(sha1->dat, 64 - 8);
	SHSTransform(sha1->dig, sha1->dat);
	/* Output SHA digest in byte array */
	for (i = 0, j = 0; j < 20; i++, j += 4) {
		out[j + 3] = (uint8_t) (sha1->dig[i] & 0xff);
		out[j + 2] = (uint8_t) ((sha1->dig[i] >> 8) & 0xff);
		out[j + 1] = (uint8_t) ((sha1->dig[i] >> 16) & 0xff);
		out[j] = (uint8_t) ((sha1->dig[i] >> 24) & 0xff);
	}
	/* Zeroise sensitive stuff */
	memset(sha1, 0, sizeof(sha1));
}

/*
 *  -------------------------------------------------------------------------
 *
 *  HMAC-SHA-1
 *
 *  -------------------------------------------------------------------------
 *
 *  Code adapted directly from RFC 2401.
 */
STATIC void hmac_sha1(uint8_t * text, int tlen, uint8_t * key, int klen,
		      uint8_t * digest)
{
	SHA_CTX context;
	uint8_t k_ipad[64];
	uint8_t k_opad[64];
	uint8_t tk[16];
	int i;
	if (klen > 64) {
		SHA_CTX ctx;
		SHAInit(&ctx);
		SHAUpdate(&ctx, key, klen);
		SHAFinal(tk, &ctx);
		key = tk;
		klen = 16;
	}
	memset(k_ipad, 0, sizeof(k_ipad));
	memset(k_opad, 0, sizeof(k_opad));
	memcpy(k_ipad, key, klen);
	memcpy(k_opad, key, klen);
	for (i = 0; i < 64; i++) {
		k_ipad[i] ^= 0x36;
		k_opad[i] ^= 0x5c;
	}
	/* inner */
	SHAInit(&context);
	SHAUpdate(&context, k_ipad, 64);
	SHAUpdate(&context, text, tlen);
	SHAFinal(digest, &context);
	/* outer */
	SHAInit(&context);
	SHAUpdate(&context, k_opad, 64);
	SHAUpdate(&context, digest, 20);
	SHAFinal(digest, &context);
}
#endif				/* CONFIG_SCTP_HMAC_SHA1 */
/*
 *  =========================================================================
 *
 *  MD5
 *
 *  =========================================================================
 */
#ifdef CONFIG_SCTP_HMAC_MD5
typedef struct {
	uint32_t buf[4];
	uint32_t lo, hi;
	uint32_t dat[16];
} MD5_CTX;
/* The four core functions - F1 is optimized somewhat */
/* #define F1(x, y, z) (x & y | ~x & z) */
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))
/* This is the central step in the MD5 algorithm. */
#define MD5STEP(f,w,x,y,z,dat,s) \
	 (w += f(x,y,z) + dat, w = (w<<s | w>>(32-s)) + x)
/*
 *  The core of the MD5 algorithm, this alters an existing MD5 hash to reflect
 *  the addition of 16 longwords of new data.  MD5Update blocks the data and
 *  converts bytes into longwords for this routine.
 */
STATIC void MD5Transform(uint32_t dig[4], uint32_t const dat[16])
{
	register uint32_t a, b, c, d;
	a = dig[0];
	b = dig[1];
	c = dig[2];
	d = dig[3];
	MD5STEP(F1, a, b, c, d, dat[0] + 0xd76aa478, 7);
	MD5STEP(F1, d, a, b, c, dat[1] + 0xe8c7b756, 12);
	MD5STEP(F1, c, d, a, b, dat[2] + 0x242070db, 17);
	MD5STEP(F1, b, c, d, a, dat[3] + 0xc1bdceee, 22);
	MD5STEP(F1, a, b, c, d, dat[4] + 0xf57c0faf, 7);
	MD5STEP(F1, d, a, b, c, dat[5] + 0x4787c62a, 12);
	MD5STEP(F1, c, d, a, b, dat[6] + 0xa8304613, 17);
	MD5STEP(F1, b, c, d, a, dat[7] + 0xfd469501, 22);
	MD5STEP(F1, a, b, c, d, dat[8] + 0x698098d8, 7);
	MD5STEP(F1, d, a, b, c, dat[9] + 0x8b44f7af, 12);
	MD5STEP(F1, c, d, a, b, dat[10] + 0xffff5bb1, 17);
	MD5STEP(F1, b, c, d, a, dat[11] + 0x895cd7be, 22);
	MD5STEP(F1, a, b, c, d, dat[12] + 0x6b901122, 7);
	MD5STEP(F1, d, a, b, c, dat[13] + 0xfd987193, 12);
	MD5STEP(F1, c, d, a, b, dat[14] + 0xa679438e, 17);
	MD5STEP(F1, b, c, d, a, dat[15] + 0x49b40821, 22);
	MD5STEP(F2, a, b, c, d, dat[1] + 0xf61e2562, 5);
	MD5STEP(F2, d, a, b, c, dat[6] + 0xc040b340, 9);
	MD5STEP(F2, c, d, a, b, dat[11] + 0x265e5a51, 14);
	MD5STEP(F2, b, c, d, a, dat[0] + 0xe9b6c7aa, 20);
	MD5STEP(F2, a, b, c, d, dat[5] + 0xd62f105d, 5);
	MD5STEP(F2, d, a, b, c, dat[10] + 0x02441453, 9);
	MD5STEP(F2, c, d, a, b, dat[15] + 0xd8a1e681, 14);
	MD5STEP(F2, b, c, d, a, dat[4] + 0xe7d3fbc8, 20);
	MD5STEP(F2, a, b, c, d, dat[9] + 0x21e1cde6, 5);
	MD5STEP(F2, d, a, b, c, dat[14] + 0xc33707d6, 9);
	MD5STEP(F2, c, d, a, b, dat[3] + 0xf4d50d87, 14);
	MD5STEP(F2, b, c, d, a, dat[8] + 0x455a14ed, 20);
	MD5STEP(F2, a, b, c, d, dat[13] + 0xa9e3e905, 5);
	MD5STEP(F2, d, a, b, c, dat[2] + 0xfcefa3f8, 9);
	MD5STEP(F2, c, d, a, b, dat[7] + 0x676f02d9, 14);
	MD5STEP(F2, b, c, d, a, dat[12] + 0x8d2a4c8a, 20);
	MD5STEP(F3, a, b, c, d, dat[5] + 0xfffa3942, 4);
	MD5STEP(F3, d, a, b, c, dat[8] + 0x8771f681, 11);
	MD5STEP(F3, c, d, a, b, dat[11] + 0x6d9d6122, 16);
	MD5STEP(F3, b, c, d, a, dat[14] + 0xfde5380c, 23);
	MD5STEP(F3, a, b, c, d, dat[1] + 0xa4beea44, 4);
	MD5STEP(F3, d, a, b, c, dat[4] + 0x4bdecfa9, 11);
	MD5STEP(F3, c, d, a, b, dat[7] + 0xf6bb4b60, 16);
	MD5STEP(F3, b, c, d, a, dat[10] + 0xbebfbc70, 23);
	MD5STEP(F3, a, b, c, d, dat[13] + 0x289b7ec6, 4);
	MD5STEP(F3, d, a, b, c, dat[0] + 0xeaa127fa, 11);
	MD5STEP(F3, c, d, a, b, dat[3] + 0xd4ef3085, 16);
	MD5STEP(F3, b, c, d, a, dat[6] + 0x04881d05, 23);
	MD5STEP(F3, a, b, c, d, dat[9] + 0xd9d4d039, 4);
	MD5STEP(F3, d, a, b, c, dat[12] + 0xe6db99e5, 11);
	MD5STEP(F3, c, d, a, b, dat[15] + 0x1fa27cf8, 16);
	MD5STEP(F3, b, c, d, a, dat[2] + 0xc4ac5665, 23);
	MD5STEP(F4, a, b, c, d, dat[0] + 0xf4292244, 6);
	MD5STEP(F4, d, a, b, c, dat[7] + 0x432aff97, 10);
	MD5STEP(F4, c, d, a, b, dat[14] + 0xab9423a7, 15);
	MD5STEP(F4, b, c, d, a, dat[5] + 0xfc93a039, 21);
	MD5STEP(F4, a, b, c, d, dat[12] + 0x655b59c3, 6);
	MD5STEP(F4, d, a, b, c, dat[3] + 0x8f0ccc92, 10);
	MD5STEP(F4, c, d, a, b, dat[10] + 0xffeff47d, 15);
	MD5STEP(F4, b, c, d, a, dat[1] + 0x85845dd1, 21);
	MD5STEP(F4, a, b, c, d, dat[8] + 0x6fa87e4f, 6);
	MD5STEP(F4, d, a, b, c, dat[15] + 0xfe2ce6e0, 10);
	MD5STEP(F4, c, d, a, b, dat[6] + 0xa3014314, 15);
	MD5STEP(F4, b, c, d, a, dat[13] + 0x4e0811a1, 21);
	MD5STEP(F4, a, b, c, d, dat[4] + 0xf7537e82, 6);
	MD5STEP(F4, d, a, b, c, dat[11] + 0xbd3af235, 10);
	MD5STEP(F4, c, d, a, b, dat[2] + 0x2ad7d2bb, 15);
	MD5STEP(F4, b, c, d, a, dat[9] + 0xeb86d391, 21);
	dig[0] += a;
	dig[1] += b;
	dig[2] += c;
	dig[3] += d;
}

#ifdef __BIG_ENDIAN
STATIC void byteSwap(uint32_t * buf, unsigned cnt)
{
	uint8_t *p = (uint8_t *) buf;
	do {
		*buf++ = (uint32_t) ((unsigned) p[3] << 8 | p[2]) << 16 | ((unsigned)
									   p[1] << 8
									   | p[0]);
		p += 4;
	} while (--cnt);
}
#else
#define byteSwap(__buf,__cnt)
#endif
/*
 * Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious
 * initialization constants.
 */
STATIC void MD5Init(MD5_CTX * md5)
{
	md5->buf[0] = 0x67452301;
	md5->buf[1] = 0xefcdab89;
	md5->buf[2] = 0x98badcfe;
	md5->buf[3] = 0x10325476;
	md5->lo = 0;
	md5->hi = 0;
}

/*
 * Update context to reflect the concatenation of another buffer full
 * of bytes.
 */
STATIC void MD5Update(MD5_CTX * md5, uint8_t const *buf, unsigned len)
{
	uint32_t t;
	/* Update byte count */
	t = md5->lo;
	if ((md5->lo = t + len) < t)
		md5->hi++;	/* Carry from low to high */
	t = 64 - (t & 0x3f);	/* Space available in md5->dat (at least 1) */
	if (t > len) {
		memcpy((uint8_t *) md5->dat + 64 - t, buf, len);
		return;
	}
	/* First chunk is an odd size */
	memcpy((uint8_t *) md5->dat + 64 - t, buf, t);
	byteSwap(md5->dat, 16);
	MD5Transform(md5->buf, md5->dat);
	buf += t;
	len -= t;
	/* Process data in 64-byte chunks */
	while (len >= 64) {
		memcpy(md5->dat, buf, 64);
		byteSwap(md5->dat, 16);
		MD5Transform(md5->buf, md5->dat);
		buf += 64;
		len -= 64;
	}
	/* Handle any remaining bytes of data. */
	memcpy(md5->dat, buf, len);
}

/*
 * Final wrapup - pad to 64-byte boundary with the bit pattern 
 * 1 0* (64-bit count of bits processed, MSB-first)
 */
STATIC void MD5Final(uint8_t dig[16], MD5_CTX * md5)
{
	int count = md5->lo & 0x3f;	/* Number of bytes in md5->dat */
	uint8_t *p = (uint8_t *) md5->dat + count;
	/* Set the first char of padding to 0x80.  There is always room. */
	*p++ = 0x80;
	/* Bytes of padding needed to make 56 bytes (-8..55) */
	count = 56 - 1 - count;
	if (count < 0) {	/* Padding forces an extra block */
		memset(p, 0, count + 8);
		byteSwap(md5->dat, 16);
		MD5Transform(md5->buf, md5->dat);
		p = (uint8_t *) md5->dat;
		count = 56;
	}
	memset(p, 0, count);
	byteSwap(md5->dat, 14);
	/* Append length in bits and transform */
	md5->dat[14] = md5->lo << 3;
	md5->dat[15] = md5->hi << 3 | md5->lo >> 29;
	MD5Transform(md5->buf, md5->dat);
	byteSwap(md5->buf, 4);
	memcpy(dig, md5->buf, 16);
	memset(md5, 0, sizeof(md5));	/* In case it's sensitive */
}

/*
 *  -------------------------------------------------------------------------
 *
 *  HMAC-MD5
 *
 *  -------------------------------------------------------------------------
 *
 *  Code adapted directly from RFC 2401.
 */
STATIC void hmac_md5(uint8_t * text, int tlen, uint8_t * key, int klen,
		     uint8_t * digest)
{
	MD5_CTX context;
	uint8_t k_ipad[65];
	uint8_t k_opad[65];
	uint8_t tk[16];
	int i;
	if (klen > 64) {
		MD5_CTX ctx;
		MD5Init(&ctx);
		MD5Update(&ctx, key, klen);
		MD5Final(tk, &ctx);
		key = tk;
		klen = 16;
	}
	memset(k_ipad, 0, sizeof(k_ipad));
	memset(k_opad, 0, sizeof(k_opad));
	memcpy(k_ipad, key, klen);
	memcpy(k_opad, key, klen);
	for (i = 0; i < 64; i++) {
		k_ipad[i] ^= 0x36;
		k_opad[i] ^= 0x5c;
	}
	/* inner */
	MD5Init(&context);
	MD5Update(&context, k_ipad, 64);
	MD5Update(&context, text, tlen);
	MD5Final(digest, &context);
	/* outer */
	MD5Init(&context);
	MD5Update(&context, k_opad, 64);
	MD5Update(&context, digest, 16);
	MD5Final(digest, &context);
}
#endif				/* CONFIG_SCTP_HMAC_MD5 */
/*
 *  =========================================================================
 *
 *  SCTP Key handling
 *
 *  =========================================================================
 *
 *  This algorithm uses 2^n keys (NUM_KEYS) which are recycled quickly and
 *  evenly.  Each key has a key tag which is placed in the SCTP cookie.  Each
 *  key is valid for the cookie lifespan plus a key life.  The key life should
 *  be set to whatever cookie life increment has been permitted.  When a cookie
 *  is checked for validity, its MAC is verified using the key with the key tag
 *  in the cookie.  If the key has already been recycled, the tagged key will
 *  not fit the lock anymore.  Note that the keys are cycled only as quickly as
 *  the requests for signatures come in.  This adds another degree of
 *  variability to the key selection.
 */
struct sctp_key {
	union {
		uint32_t seq[16];
		uint8_t key[64];
	} u;
	unsigned int last;
	unsigned long created;
};
#define NUM_KEYS 4
STATIC struct sctp_key sctp_keys[NUM_KEYS];
STATIC int sctp_current_key = 0;
extern uint32_t secure_tcp_sequence_number(uint32_t, uint32_t, uint16_t, uint16_t);
/*
 *  TODO:  This rekeying is too predicatable.  There are several things bad
 *  about it: (1) the key has a historic component, which is bad; (2) initial
 *  keys have zeros in alot of places which makes it no stronger than if only
 *  32 bit keys were used.
 */
STATIC void sctp_rekey(int k)
{
	uint32_t *seq;
	int n = (sctp_keys[k].last + 1) & 0xf;
	sctp_keys[k].last = n;
	seq = &sctp_keys[k].u.seq[n];
	*seq =
	    secure_tcp_sequence_number(*(seq + 1), *(seq + 2), *(seq + 3),
				       *(seq + 4));
}
STATIC int sctp_get_key(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	int k = sctp_current_key;
	unsigned long duration =
	    ((sp->ck_life + NUM_KEYS - 1) / NUM_KEYS) + sp->ck_inc;
	unsigned long created = sctp_keys[k].created;
	if (!created) {
		sctp_keys[k].created = jiffies;
		return k;
	}
	if (created + duration < jiffies) {
		k = (k + 1) % NUM_KEYS;
		sctp_rekey(k);
		sctp_current_key = k;
	}
	return k;
}
STATIC void sctp_hmac(struct sock *sk, uint8_t * text, int tlen, uint8_t * key,
		      int klen, uint8_t * hmac)
{
	memset(hmac, 0xff, HMAC_SIZE);
	switch (SCTP_PROT(sk)->hmac) {
#ifdef CONFIG_SCTP_HMAC_SHA1
	case SCTP_HMAC_SHA_1:
		hmac_sha1(text, tlen, key, klen, hmac);
		break;
#endif
#ifdef CONFIG_SCTP_HMAC_MD5
	case SCTP_HMAC_MD5:
		hmac_md5(text, tlen, key, klen, hmac);
		break;
#endif
	default:
	case SCTP_HMAC_NONE:
		break;
	}
}
STATIC void sctp_sign_cookie(struct sock *sk, struct sctp_cookie *ck)
{
	uint8_t *text = (uint8_t *) ck;
	int tlen = raw_cookie_size(ck);
	int ktag = sctp_get_key(sk);
	uint8_t *key = sctp_keys[ktag].u.key;
	int klen = sizeof(sctp_keys[0].u.key);
	uint8_t *hmacp = ((uint8_t *) ck) + tlen;
	ck->key_tag = ktag;
	sctp_hmac(sk, text, tlen, key, klen, hmacp);
}

/* Note: caller must verify length of cookie */
STATIC int sctp_verify_cookie(struct sock *sk, struct sctp_cookie *ck)
{
	uint8_t hmac[HMAC_SIZE];
	uint8_t *text = (uint8_t *) ck;
	int tlen = raw_cookie_size(ck);
	int ktag = (ck->key_tag) % NUM_KEYS;
	uint8_t *key = sctp_keys[ktag].u.key;
	int klen = sizeof(sctp_keys[0].u.key);
	uint8_t *hmacp = ((uint8_t *) ck) + tlen;
	sctp_hmac(sk, text, tlen, key, klen, hmac);
	return memcmp(hmacp, hmac, HMAC_SIZE);
}

/* 
 *  =========================================================================
 *
 *  IP OUTPUT: ROUTING FUNCTIONS
 *
 *  =========================================================================
 *
 *  MULTI-HOMED IP ROUTING FUNCTIONS
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  BREAK TIE
 *  -------------------------------------------------------------------------
 *  Break a tie between two equally rated routes.
 */
STATIC INLINE size_t sctp_avail(struct sctp_opt *sp, struct sctp_daddr *sd);
STATIC INLINE struct sctp_daddr *sctp_break_tie(struct sctp_opt *sp,
						struct sctp_daddr *sd1,
						struct sctp_daddr *sd2)
{
#if 0
	/* choose usable routes over unusable routes */
	if ((sd1->flags & SCTP_DESTF_UNUSABLE) != (sd2->flags & SCTP_DESTF_UNUSABLE)) {
		if (sd2->flags & SCTP_DESTF_UNUSABLE)
			goto use1;
		goto use2;
	}
	/* choose routes over non-routes */
	if ((sd1->dst_cache == NULL) != (sd2->dst_cache == NULL)) {
		if (sd2->dst_cache == NULL)
			goto use1;
		goto use2;
	}
#endif
	/* choose routes with the least excessive timeouts */
	if ((sd1->retransmits > sd1->max_retrans) &&
	    (sd2->retransmits > sd2->max_retrans)) {
		if (sd1->retransmits + sd2->max_retrans !=
		    sd2->retransmits + sd1->max_retrans) {
			if (sd1->retransmits + sd2->max_retrans <
			    sd2->retransmits + sd1->max_retrans)
				goto use1;
			goto use2;
		}
	}
	/* choose routes with the least duplicates */
	if (sd1->dups && sd2->dups) {
		if (sd1->dups != sd2->dups) {
			if (sd1->dups < sd2->dups)
				goto use1;
			goto use2;
		}
	}
	/* choose routes with the lowest srtt */
	if (sd1->srtt != sd2->srtt) {
		if (sd1->srtt < sd2->srtt)
			goto use1;
		goto use2;
	}
	/* choose not to slow start */
	if ((sd1->cwnd > sd1->ssthresh) != (sd2->cwnd > sd2->ssthresh)) {
		if (sd1->cwnd > sd1->ssthresh)
			goto use1;
		goto use2;
	}
	/* choose same route */
	if (sd1 == sp->taddr || sd2 == sp->taddr) {
		if (sd1 == sp->taddr)
			goto use1;
		goto use2;
	}
#if 0
	/* choose routes with lowest congestion window */
	if (sd1->cwnd != sd2->cwnd) {
		if (sd1->cwnd < sd2->cwnd)
			goto use1;
		goto use2;
	}
#endif
	/* choose routes with best congestion window */
	if (sd1->cwnd + sd2->in_flight != sd2->cwnd + sd1->in_flight) {
		if (sd1->cwnd + sd2->in_flight > sd2->cwnd + sd1->in_flight)
			goto use1;
		goto use2;
	}
	/* choose routes with the lowest rto */
	if (sd1->rto != sd2->rto) {
		if (sd1->rto < sd2->rto)
			goto use1;
		goto use2;
	}
	/* can't break tie, just go with sd1 */
      use1:
	return (sd1);
      use2:
	return (sd2);
}

/*
 *  RATE ROUTE
 *  -------------------------------------------------------------------------
 *  Rate the provided route according to a weighting function.
 */
STATIC INLINE int sctp_rate_route(struct sctp_opt *sp, struct sctp_daddr *sd)
{
	int value = 0;
#if 0
#ifdef CONFIG_SCTP_ADD_IP
	if (sd->flags & SCTP_DESTF_UNUSABLE)
		goto done;
	value += 10;
#endif
#endif
	if (sd->dst_cache == NULL)
		goto done;
	value += 10;
	if (sd->retransmits > sd->max_retrans)
		goto done;
	value++;
	if (sctp_avail(sp, sd) == 0)
		goto done;
	value++;
	if (!sd->retransmits)
		value += 1;
	if (!sd->dups)
		value += 1;
	if (sd->cwnd >= sd->in_flight + sd->dmps)
		value += 2;
	if (sd->cwnd <= sd->ssthresh)
		value += 1;
	if (sd == sp->taddr)
		value -= 1;
      done:
	return (value);
}

/*
 *  CHOSE BEST
 *  -------------------------------------------------------------------------
 *  This function is called by sctp_update_routes to choose the best primary
 *  address.  We alway return a usable address if possible.  This is called
 *  only when a route fails or a destination goes inactive due to too many
 *  timeouts, so destinations that are timing out are not so large a factor.
 */
STATIC INLINE struct sctp_daddr *sctp_choose_best(struct sctp_opt *sp)
{
	struct sctp_daddr *best = NULL, *sd;
	int best_value = -1, value;
	for (sd = sp->daddr; sd; sd = sd->next) {
		if (best_value != (value = sctp_rate_route(sp, sd)) || !best) {
			if (best_value < value) {
				best = sd;
				best_value = value;
			}
		} else
			best = sctp_break_tie(sp, best, sd);
	}
	return (best);
}

/*
 *  UPDATE ROUTES
 *  -------------------------------------------------------------------------
 *  This function is called whenever there is a routing problem or whenever a
 *  timeout occurs on a destination.  It's purpose is to perform a complete
 *  re-analysis of the available destination addresses and IP routing and
 *  establish new routes as required and set the (primary) and (secondary)
 *  destination addresses.
 */
#ifndef sysctl_ip_dynaddr
#define sysctl_ip_dynaddr 0
#endif
#ifndef ip_rt_min_pmtu
#define ip_rt_min_pmtu 552
#endif
#if defined(CONFIG_SCTP_DEBUG) && defined(CONFIG_SCTP_ERROR_GENERATOR)
#define ERROR_GENERATOR_LEVEL  8
#define ERROR_GENERATOR_LIMIT 13
#define BREAK_GENERATOR_LEVEL 50
#define BREAK_GENERATOR_LIMIT 200
#endif
STATIC INLINE int dst_check(struct dst_entry **dstp) {
	struct dst_entry *dst;
	if (dstp) {
		dst = *dstp;
		if (dst->ops && dst->ops->check)
			return (!(*dstp = dst->ops->check(dst, 0)));
		dst_release(xchg(dstp, NULL));
		return (0);
	} else {
		swerr();
		return (0);
	}
}
/*
 *  sysctl_ip_dynaddr: this symbol is normally not exported, but we exported
 *  for the Linux Native version of SCTP, so we may have to treat it as extern
 *  here...
 */
STATIC int sctp_update_routes(struct sock *sk, int force_reselect)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	int err = -EHOSTUNREACH;
	int mtu_changed = 0;
	int viable_route = 0;
	int route_changed = 0;
	struct sctp_daddr *sd;
	struct sctp_daddr *taddr;
	int old_pmtu;
	assert(sk);
	ensure(sp->daddr, return (-EFAULT));
	old_pmtu = xchg(&sp->pmtu, INT_MAX);	/* big enough? */
	sk->route_caps = -1L;
	taddr = sp->taddr;
	/* First we check our cached routes..  */
	for (sd = sp->daddr; sd; sd = sd->next) {
		struct rtable *rt = (struct rtable *) sd->dst_cache;
#ifdef CONFIG_SCTP_ADD_IP
		if (sd->flags & SCTP_DESTF_UNUSABLE) {
			if (sd->dst_cache)
				dst_release(xchg(&sd->dst_cache, NULL));
			continue;
		}
#endif
		printd(("INFO: Checking socket %p route for %d.%d.%d.%d\n", sk,
			(sd->daddr >> 0) & 0xff, (sd->daddr >> 8) & 0xff,
			(sd->daddr >> 16) & 0xff, (sd->daddr >> 24) & 0xff));
		if (!sd->dst_cache || (sd->dst_cache->obsolete && !dst_check(&sd->dst_cache))) {
			rt = NULL;
			route_changed = 1;
			/* try wildcard saddr and dif routing */
#if defined HAVE_STRUCT_INET_PROTOCOL_PROTOCOL
			err = ip_route_connect(&rt, sd->daddr, 0, RT_CONN_FLAGS(sk), 0);
#elif defined HAVE_STRUCT_INET_PROTOCOL_NO_POLICY
			err = ip_route_connect(&rt, sd->daddr, 0, RT_CONN_FLAGS(sk), 0,
					IPPROTO_SCTP, sk->sport, sk->dport, NULL);
#else
#error One of HAVE_STRUCT_INET_PROTOCOL_PROTOCOL or HAVE_STRUCT_INET_PROTOCOL_NO_POLICY must be defined.
#endif
			if (err < 0 || !rt || rt->u.dst.obsolete) {
				rare();
				if (rt)
					ip_rt_put(rt);
				if (err == 0)
					err = -EHOSTUNREACH;
				continue;
			}
			if (rt->rt_flags & (RTCF_MULTICAST | RTCF_BROADCAST)
			    && !sk->broadcast) {
				rare();
				ip_rt_put(rt);
				err = -ENETUNREACH;
				continue;
			}
			sd->saddr = rt->rt_src;
			if (!sctp_find_saddr(sk, sd->saddr)) {
				rare();
#ifdef CONFIG_SCTP_ADD_IP
				/* Candidate for ADD-IP but we can't use it yet */
				if (sp->p_caps & SCTP_CAPS_ADD_IP
				    && !(sk->userlocks & SOCK_BINDADDR_LOCK)) {
					int err = 0;
					struct sctp_saddr *ss;
					if ((ss =
					     __sctp_saddr_alloc(sk, rt->rt_src, &err))) {
						ss->flags |= SCTP_SRCEF_ADD_REQUEST;
						sp->sackf |= SCTP_SACKF_ASC;
					}
				}
#endif
				if (!sp->saddr) {
					rare();
					ip_rt_put(rt);
					assure(sp->saddr);
					err = -EADDRNOTAVAIL;
					continue;
				}
				sd->saddr = sp->saddr->saddr;
			}
			sd->dif = rt->rt_iif;
			/* always revert to initial settings when rerouting */
			sd->rto = sp->rto_ini;
			sd->rttvar = 0;
			sd->srtt = 0;
			sd->mtu = dst_pmtu(&rt->u.dst);
			sd->dmps =
			    sd->mtu - sp->ext_header_len - sizeof(struct iphdr) -
			    sizeof(struct sctphdr);
			sd->ssthresh = 2 * dst_pmtu(&rt->u.dst);
			sd->cwnd = dst_pmtu(&rt->u.dst);
			/* SCTP IG Section 2.9 */
			sd->partial_ack = 0;
			sd->dst_cache = &rt->u.dst;
			sd->route_caps = rt->u.dst.dev->features;
		}
		/* You're welcome diald! */
		if (sysctl_ip_dynaddr && sk->state == SCTP_COOKIE_WAIT && sd == sp->daddr) {
			/* see if route changed on primary as result of INIT that was discarded */
			struct rtable *rt2 = NULL;
#if defined HAVE_STRUCT_INET_PROTOCOL_PROTOCOL
			if (!ip_route_connect
			    (&rt2, rt->rt_dst, 0, RT_CONN_FLAGS(sk), sd->dif))
#elif defined HAVE_STRUCT_INET_PROTOCOL_NO_POLICY
			if (!ip_route_connect
			    (&rt2, rt->rt_dst, 0, RT_CONN_FLAGS(sk), sd->dif, IPPROTO_SCTP,
			     sk->sport, sk->dport, NULL))
#else
#error One of HAVE_STRUCT_INET_PROTOCOL_PROTOCOL or HAVE_STRUCT_INET_PROTOCOL_NO_POLICY must be defined.
#endif
			{
				if (rt2->rt_src != rt->rt_src) {
					rare();
					rt2 = xchg(&rt, rt2);
					sd->rto = sp->rto_ini;
					sd->rttvar = 0;
					sd->srtt = 0;
					sd->mtu = dst_pmtu(&rt->u.dst);
					sd->dmps =
					    sd->mtu - sp->ext_header_len -
					    sizeof(struct iphdr) - sizeof(struct sctphdr);
					sd->ssthresh = 2 * dst_pmtu(&rt->u.dst);
					sd->cwnd = dst_pmtu(&rt->u.dst);
					/* SCTP IG Section 2.9 */
					sd->partial_ack = 0;
					sd->dst_cache = &rt->u.dst;
					sd->route_caps = rt->u.dst.dev->features;
					route_changed = 1;
				}
				ip_rt_put(rt2);
			}
		}
		viable_route = 1;
		/* always update MTU if we have a viable route */
		sk->route_caps &= rt->u.dst.dev->features;
		if (sd->mtu != dst_pmtu(&rt->u.dst)) {
			sd->mtu = dst_pmtu(&rt->u.dst);
			sd->dmps =
			    sd->mtu - sp->ext_header_len - sizeof(struct iphdr) -
			    sizeof(struct sctphdr);
			mtu_changed = 1;
			rare();
		}
		if (sp->pmtu > sd->mtu) {
			sp->pmtu = sd->mtu;
			sp->amps = sd->dmps;
		}
	}
	if (!viable_route) {
		rare();
		/* set defaults */
		sp->taddr = sp->daddr;
		sp->pmtu = ip_rt_min_pmtu;
		sp->amps =
		    sp->pmtu - sp->ext_header_len - sizeof(struct iphdr) -
		    sizeof(struct sctphdr);
		__ptrace(("ERROR: no viable route\n"));
		return (err);
	}
	/* if we have made or need changes then we want to reanalyze routes */
	if (force_reselect || route_changed || mtu_changed || sp->pmtu != old_pmtu
	    || !sp->taddr) {
#if defined(CONFIG_SCTP_DEBUG) && defined(CONFIG_SCTP_ERROR_GENERATOR)
		int bad_choice = 0;
#endif
		sp->taddr = sctp_choose_best(sp);
		usual(sp->taddr);
#if defined(CONFIG_SCTP_DEBUG) && defined(CONFIG_SCTP_ERROR_GENERATOR)
		if ((sp->options & SCTP_OPTION_BREAK)
		    && (sp->taddr == sp->daddr || sp->taddr == sp->daddr->next)
		    && sp->taddr->packets > BREAK_GENERATOR_LEVEL) {
			ptrace(("Primary sp->taddr %03d.%03d.%03d.%03d chosen poorly on %x\n", (sp->taddr->daddr >> 0) & 0xff, (sp->taddr->daddr >> 8) & 0xff, (sp->taddr->daddr >> 16) & 0xff, (sp->taddr->daddr >> 24) & 0xff, (uint) sp));
			bad_choice = 1;
		}
#endif
		if (sp->taddr)
			ptrace(("sp = %p, taddr = %p, Primary route: %d.%d.%d.%d -> %d.%d.%d.%d\n", sp, sp->taddr, (sp->taddr->saddr >> 0) & 0xff, (sp->taddr->saddr >> 8) & 0xff, (sp->taddr->saddr >> 16) & 0xff, (sp->taddr->saddr >> 24) & 0xff, (sp->taddr->daddr >> 0) & 0xff, (sp->taddr->daddr >> 8) & 0xff, (sp->taddr->daddr >> 16) & 0xff, (sp->taddr->daddr >> 24) & 0xff));
	}
	return (0);
}

/*
 *  ==========================================================================
 *
 *  MESSAGE OUTPUT
 *
 *  ==========================================================================
 */
/* 
 *  QUEUE XMIT (Queue for transmission)
 *  -------------------------------------------------------------------------
 *  We need this broken out so that we can use the netfilter hooks.
 */
STATIC INLINE int sctp_queue_xmit(struct sk_buff *skb)
{
	struct rtable *rt = (struct rtable *) skb->dst;
	struct iphdr *iph = skb->nh.iph;
	if (skb->len > dst_pmtu(&rt->u.dst)) {
		rare();
		return ip_fragment(skb, skb->dst->output);
	} else {
		iph->frag_off |= __constant_htons(IP_DF);
		ip_send_check(iph);
		return skb->dst->output(skb);
	}
}

/* 
 *  XMIT OOTB (Disconnect Send with no Listening Socket).
 *  -------------------------------------------------------------------------
 *  This sends disconnected without a socket.  All that is needed is a
 *  destination address and a socket buffer.  The only time that we use this
 *  is for responding to OOTB packets with ABORT or SHUTDOWN COMPLETE.
 */
STATIC void sctp_xmit_ootb(uint32_t daddr, uint32_t saddr, struct sk_buff *skb)
{
	struct rtable *rt = NULL;
	struct sk_buff *skb2 = skb;
	ensure(skb, return);
	if (!ip_route_output(&rt, daddr, 0, 0, 0)) {
		struct net_device *dev = rt->u.dst.dev;
		size_t hlen = (dev->hard_header_len + 15) & ~15;
		size_t plen = skb->len;
		size_t tlen = plen + sizeof(struct iphdr);
		size_t head = hlen + sizeof(struct iphdr);
		usual(hlen);
		usual(plen);
		if (head > skb_headroom(skb2))
			skb = skb_realloc_headroom(skb2, head);
		if (skb) {
			struct sctphdr *sh = (typeof(sh)) skb->data;
			struct iphdr *iph =
			    (typeof(iph)) skb_push(skb, sizeof(*iph));
			skb->dst = &rt->u.dst;
			skb->priority = 0;
			iph->version = 4;
			iph->ihl = 5;
			iph->tos = 0;
			iph->frag_off = 0;
			iph->ttl = 64;
			iph->daddr = rt->rt_dst;
			iph->saddr = saddr;
			iph->protocol = 132;
			iph->tot_len = htons(tlen);
			skb->nh.iph = iph;
			__ip_select_ident(iph, &rt->u.dst);
			sh->check = 0;
			if (!(dev->features & (NETIF_F_NO_CSUM | NETIF_F_HW_CSUM)))
				sh->check = htonl(cksum_generate(sh, plen));
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTSCTPPACKS
			SCTP_INC_STATS(SctpOutSCTPPacks);
#endif
			NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dev,
				sctp_queue_xmit);
			return;
		} else
			rare();
	} else
		rare();
	/* sending OOTB reponses are one time events, if we can't send the message
	   we just drop it, the peer will probably come back again later */
	kfree_skb(skb2);
	return;
}

/* 
 *
 *  XMIT MSG  (Disconnected Send)
 *  -------------------------------------------------------------------------
 *  This sends disconnected sends.  All that is needed is a destination
 *  address and a message block.  The only time that we use this is for
 *  sending INIT ACKs (because we have not built a complete socket yet,
 *  however we do at least have a Listening socket or possibly an established
 *  socket that is partially qualitfied and can provide some TOS and other IP
 *  option information for the packet.
 *
 *  Note that RFC 2960 request that we use the destination address to which the
 *  original INIT was sent as the source address for the INIT-ACK.  When we
 *  receive INIT-ACKs we really don't care whether they come from any of the
 *  addresses to which the association is bound.
 *
 *  This is also now used for sending ABORT or ABORT w/ERROR in response to
 *  INIT chunks.
 */
STATIC INLINE void freechunks(struct sk_buff *skb);
STATIC void sctp_xmit_msg(uint32_t saddr, uint32_t daddr, struct sk_buff *skb,
			  struct sock *sk)
{
	struct inet_opt *ip = &sk->protinfo.af_inet;
	struct rtable *rt = NULL;
	struct sk_buff *skb2 = skb;
	ensure(skb, return);
	if (!ip_route_output(&rt, daddr, saddr, RT_TOS(ip->tos) | sk->localroute, 0)) {
		struct net_device *dev = rt->u.dst.dev;
		size_t hlen = (dev->hard_header_len + 15) & ~15;
		size_t plen = skb->len;
		size_t tlen = plen + sizeof(struct iphdr);
		size_t head = hlen + sizeof(struct iphdr);
		usual(hlen);
		usual(plen);
		if (head > skb_headroom(skb2))
			skb = skb_realloc_headroom(skb2, head);
		if (skb) {
			struct sctphdr *sh = (typeof(sh)) skb->data;
			struct iphdr *iph =
			    (typeof(iph)) skb_push(skb, sizeof(*iph));
			skb->dst = &rt->u.dst;
			skb->priority = sk->priority;
			iph->version = 4;
			iph->ihl = 5;
			iph->tos = ip->tos;
			iph->frag_off = 0;
#ifdef HAVE_STRUCT_SOCK_PROTOINFO_AF_INET_TTL
			iph->ttl = ip->ttl;
#elif HAVE_STRUCT_SOCK_PROTOINFO_AF_INET_UC_TTL
			iph->ttl = ip->uc_ttl;
#endif
			iph->daddr = rt->rt_dst;
			iph->saddr = saddr;
			iph->protocol = sk->protocol;
			iph->tot_len = htons(tlen);
			skb->nh.iph = iph;
			__ip_select_ident(iph, &rt->u.dst);
			sh->check = 0;
			if (!(dev->features & (NETIF_F_NO_CSUM | NETIF_F_HW_CSUM)))
				sh->check = htonl(cksum(sk, sh, plen));
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTSCTPPACKS
			SCTP_INC_STATS(SctpOutSCTPPacks);
#endif
			NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dev,
				sctp_queue_xmit);
			return;
		} else
			rare();
	} else
		rare();
	/* sending INIT ACKs are one time events, if we can't get the response off,
	   we just drop the INIT ACK: the peer should send us another INIT * in a
	   short while... */
	freechunks(skb2);
	return;
}

/* 
 *  SEND MSG  (Connected Send)
 *  -------------------------------------------------------------------------
 *  This sends connected sends.  It requires a socket, a desination address
 *  structure and a scoket buffer.  This function does not free the socket
 *  buffer.  The caller is responsible for the socket buffer.
 */
#if defined(CONFIG_SCTP_DEBUG) && defined(CONFIG_SCTP_ERROR_GENERATOR)
STATIC int break_packets = 0;
STATIC int seed = 152;
STATIC unsigned char random(void)
{
	return (unsigned char) (seed = seed * 60691 + 1);
}
#endif
STATIC void sctp_send_msg(struct sock *sk, struct sctp_daddr *sd,
			  struct sk_buff *skc)
{
	struct inet_opt *ip = &sk->protinfo.af_inet;
	struct sk_buff *skb;
	size_t plen = SCTP_SKB_CB(skc)->dlen;
	size_t tlen = sizeof(struct iphdr) + plen;
	struct net_device *dev;
	size_t hlen;
#if defined(CONFIG_SCTP_DEBUG) && defined(CONFIG_SCTP_ERROR_GENERATOR)
	struct sctp_opt *sp = SCTP_PROT(sk);
	if ((sp->options & SCTP_OPTION_DBREAK)
	    && sd->daddr == 0x010000ff && ++break_packets > BREAK_GENERATOR_LEVEL) {
		if (break_packets > BREAK_GENERATOR_LIMIT)
			break_packets = 0;
		return;
	}
	if ((sp->options & SCTP_OPTION_BREAK)
	    && (sd == sp->daddr || sd == sp->daddr->next)
	    && ++sd->packets > BREAK_GENERATOR_LEVEL) {
		return;
	}
	if ((sp->options & SCTP_OPTION_DROPPING)
	    && ++sd->packets > ERROR_GENERATOR_LEVEL) {
		if (sd->packets > ERROR_GENERATOR_LIMIT)
			sd->packets = 0;
		return;
	}
	if ((sp->options & SCTP_OPTION_RANDOM)
	    && ++sd->packets > 2 * ERROR_GENERATOR_LEVEL) {
		if (!(random() & 0x7f)) {
			printd(("WARNING: Dropping packet\n"));
			return;
		}
	}
#endif
	if (!sd->dst_cache)
		return;
	dev = sd->dst_cache->dev;
	hlen = (dev->hard_header_len + 15) & ~15;
	printd(("INFO: Preparing message sk %p, hlen %u, plen %u, tlen %u\n", sk,
		hlen, plen, tlen));
	unusual(plen == 0 || plen > 1 << 14);
	/* 
	 *  IMPLEMENTATION NOTE:- We could clone these sk_buffs and put them
	 *  into a fraglist, however, this would require copying the sk_buff
	 *  header on each data chunk (just to get a new next pointer).
	 *  Unfortunately, for the most part in SCTP, data chunks are very
	 *  small: even smaller than the sk_buff header so it is probably not
	 *  worth cloning versus copying.  A workable sendpages might be a
	 *  better approach to larger data chunks.
	 */
	if ((skb = alloc_skb(hlen + tlen, GFP_ATOMIC))) {
		struct sk_buff *skp;
		struct sctp_skb_cb *cb;
		struct iphdr *iph;
		struct sctphdr *sh;
		unsigned char *head, *data;
		size_t alen = 0;
		printd(("INFO: Sending messsage %d.%d.%d.%d -> %d.%d.%d.%d\n",
			(sd->saddr >> 0) & 0xff, (sd->saddr >> 8) & 0xff,
			(sd->saddr >> 16) & 0xff, (sd->saddr >> 24) & 0xff,
			(sd->daddr >> 0) & 0xff, (sd->daddr >> 8) & 0xff,
			(sd->daddr >> 16) & 0xff, (sd->daddr >> 24) & 0xff));
		skb_reserve(skb, hlen);
		iph = (struct iphdr *) skb_put(skb, tlen);
		sh = (struct sctphdr *) (iph + 1);
		head = data = (unsigned char *) sh;
		skb->dst = dst_clone(sd->dst_cache);
		skb->priority = sk->priority;
		iph->version = 4;
		iph->ihl = 5;
		iph->tos = ip->tos;
		iph->frag_off = 0;
#ifdef HAVE_STRUCT_SOCK_PROTOINFO_AF_INET_TTL
		iph->ttl = ip->ttl;
#elif HAVE_STRUCT_SOCK_PROTOINFO_AF_INET_UC_TTL
		iph->ttl = ip->uc_ttl;
#endif
		iph->daddr = sd->daddr;
		iph->saddr = sd->saddr;
		iph->protocol = sk->protocol;
		iph->tot_len = htons(tlen);
		skb->nh.iph = iph;
		__ip_select_ident(iph, sd->dst_cache);
		for (cb = SCTP_SKB_CB(skc); cb; cb = cb->next) {
			size_t pad, blen, clen = 0;
			/* for destination RTT calculation */
			cb->daddr = sd;
			cb->when = jiffies;
			cb->trans++;
			skp = SCTP_CB_SKB(cb);
			if ((blen = skp->len) > 0) {
				ensure(head + plen >= data + blen, kfree_skb(skb);
				       return);
				memcpy(data, skp->data, blen);
				data += blen;
				clen += blen;
			}
			/* pad each chunk if not padded already */
			pad = PADC(clen) - clen;
			ensure(head + plen >= data + pad, kfree_skb(skb); return);
			memset(data, '0', pad);
			data += pad;
			alen += clen + pad;
		}
		if (alen != plen) {
			usual(alen == plen);
			ptrace(("alen = %u, plen = %u discarding\n", alen, plen));
			kfree_skb(skb);
			return;
		}
		/* 
		 *  IMPLEMENTATION NOTE:- Although the functional support is
		 *  included for partial checksum and copy from user, we don't
		 *  use it.  The only time that partial checksum is going to
		 *  save us is when chunks a bundled repeatedly (for
		 *  retransmissions).  But then we are network bound rather
		 *  than processor bound, so there is probably little use
		 *  there either.  It might be better, however, to combine
		 *  checksumming below with copying above for efficiency.
		 */
		sh->check = 0;
		if (!(dev->features & (NETIF_F_NO_CSUM | NETIF_F_HW_CSUM)))
			sh->check = htonl(cksum(sk, sh, plen));
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTSCTPPACKS
		SCTP_INC_STATS(SctpOutSCTPPacks);
#endif
		NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, skb, NULL, dev, sctp_queue_xmit);
		/* Whenever we transmit something, we expect a reply to our v_tag, so we put
		   ourselves in the 1st level vtag caches expecting a quick reply. */
		if (!((1 << sk->state) & (SCTPF_CLOSED | SCTPF_LISTEN)))
			sctp_cache[SCTP_PROT(sk)->hashent].list = SCTP_PROT(sk);
	} else {
		rare();
	}
}

/*
 *  ==========================================================================
 *
 *  SCTP --> SCTP Peer Messages (Send Messages)
 *
 *  ==========================================================================
 */
STATIC INLINE int sctp_rspace(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	int amt = 0;
	if (!(sk->shutdown & RCV_SHUTDOWN)) {
		amt = sk->rcvbuf - atomic_read(&sk->rmem_alloc);
		if (PADC(amt + sizeof(struct sctp_data)) < sp->amps)
			amt = 0;
	}
	return amt;
}
STATIC INLINE int sctp_min_wspace(struct sock *sk)
{
	return atomic_read(&sk->wmem_alloc) >> 1;
}
STATIC INLINE int sctp_wspace(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	int amt = 0;
	if (!(sk->shutdown & SEND_SHUTDOWN)) {
		ensure(atomic_read(&sk->wmem_alloc) >= 0, atomic_set(&sk->wmem_alloc, 0));
		amt = sk->sndbuf - atomic_read(&sk->wmem_alloc);
		if (PADC(amt + sizeof(struct sctp_data)) < sp->amps)
			amt = 0;
	}
	return amt;
}
STATIC void sctp_write_space(struct sock *sk)
{
	struct socket *sock = sk->socket;
	if (sctp_wspace(sk) >= sctp_min_wspace(sk) && sock) {
		clear_bit(SOCK_NOSPACE, &sock->flags);
		if (sk->sleep && waitqueue_active(sk->sleep))
			wake_up_interruptible(sk->sleep);
		if (sock->fasync_list && !(sk->shutdown & SEND_SHUTDOWN))
			sock_wake_async(sock, 2, POLL_OUT);
	}
}
STATIC void sctp_rfree(struct sk_buff *skb)
{
	struct sock *sk = skb->sk;
	struct sctp_opt *sp = SCTP_PROT(sk);
	atomic_sub(SCTP_SKB_CB(skb)->dlen, &sk->rmem_alloc);
	if (!(sk->shutdown & RCV_SHUTDOWN)) {
		/* SCTP IG 2.15.2, Receive SWS Avoidance RFC 1122 4.2.3.3 */
		size_t rbuf = sk->rcvbuf >> 1;
		size_t amps = sp->amps;
		size_t alloc = atomic_read(&sk->rmem_alloc);
		size_t a_rwnd = (sk->rcvbuf > alloc) ? sk->rcvbuf - alloc : 0;
		if (a_rwnd >= sp->a_rwnd + ((amps < rbuf) ? amps : rbuf)) {
			sp->a_rwnd = a_rwnd;
			sp->sackf |= SCTP_SACKF_WND;	/* RFC 2960 6.2 */
		}
	}
}
STATIC void sctp_wfree(struct sk_buff *skb)
{
	struct sock *sk = skb->sk;
	atomic_sub(skb->truesize, &sk->wmem_alloc);
	ensure(atomic_read(&sk->wmem_alloc) >= 0, atomic_set(&sk->wmem_alloc, 0));
	if (!sk->use_write_queue)
		sk->write_space(sk);
	sock_put(sk);
}
STATIC INLINE void sctp_set_owner_r(struct sk_buff *skb, struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	skb->sk = sk;
	skb->destructor = sctp_rfree;
	atomic_add(SCTP_SKB_CB(skb)->dlen, &sk->rmem_alloc);
	if (!(sk->shutdown & RCV_SHUTDOWN)) {
		/* SCTP IG 2.15.2, Receive SWS Avoidance RFC 1122 4.2.3.3 */
		size_t rbuf = sk->rcvbuf >> 1;
		size_t amps = sp->amps;
		size_t alloc = atomic_read(&sk->rmem_alloc);
		size_t a_rwnd = (sk->rcvbuf > alloc) ? sk->rcvbuf - alloc : 0;
		if (a_rwnd > amps || a_rwnd > rbuf)
			sp->a_rwnd = a_rwnd;
		else
			sp->a_rwnd = 0;
	}
}
STATIC INLINE void sctp_set_owner_w(struct sk_buff *skb, struct sock *sk)
{
	sock_hold(sk);
	skb->sk = sk;
	skb->destructor = sctp_wfree;
	atomic_add(skb->truesize, &sk->wmem_alloc);
	ensure(atomic_read(&sk->wmem_alloc) >= 0, atomic_set(&sk->wmem_alloc, 0));
}
STATIC INLINE void sctp_get_owner_r(struct sk_buff *new, struct sk_buff *old)
{
	new->sk = old->sk;
	new->destructor = old->destructor;
	old->sk = NULL;
	old->destructor = NULL;
}
STATIC INLINE void sctp_put_owner_r(struct sk_buff *skb)
{
	if (skb->destructor)
		skb->destructor(skb);
	skb->sk = NULL;
	skb->destructor = NULL;
}
STATIC struct sk_buff *sctp_wmalloc(struct sock *sk, unsigned long size,
				    int force, int priority)
{
	if (force || atomic_read(&sk->wmem_alloc) < sk->sndbuf) {
		struct sk_buff *skb;
		if ((skb = alloc_skb(size, priority))) {
			sctp_set_owner_w(skb, sk);
			return skb;
		}
	}
	return NULL;
}

/*
 *  CONGESTION/RECEIVE WINDOW AVAILABILITY
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *
 *  Calculate of the remaining space in the current packet, how much is
 *  available for use by data according to the current peer receive window, the
 *  current destination congestion window, and the current outstanding data
 *  bytes in flight.
 *
 *  This is called iteratively as each data chunk is tested for bundling into
 *  the current message.  The usable length returned does not include the data
 *  chunk header.
 */
STATIC INLINE size_t sctp_avail(struct sctp_opt *sp, struct sctp_daddr *sd)
{
	size_t cwnd, rwnd, swnd, awnd;
	cwnd = sd->cwnd + sd->mtu + 1;
	cwnd = (cwnd > sd->in_flight) ? cwnd - sd->in_flight : 0;
	rwnd = sp->p_rwnd;
	rwnd = (rwnd > sp->in_flight) ? rwnd - sp->in_flight : 0;
	swnd = (cwnd < rwnd) ? cwnd : rwnd;
	awnd = (sp->in_flight) ? swnd : cwnd;
	return awnd;
}

/*
 *  BUNDLING FUNCTIONS
 *  -------------------------------------------------------------------------
 *
 *  BUNDLE SACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int
sctp_bundle_sack(struct sock *sk, struct sctp_daddr *sd, struct sctp_skb_cb ***spp, size_t * mrem, size_t * mlen)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	struct sctp_sack *m;
	size_t ngaps = sp->ngaps;
	size_t ndups = sp->ndups;
	size_t glen = ngaps * sizeof(uint32_t);
	size_t dlen = ndups * sizeof(uint32_t);
	size_t clen = sizeof(*m) + glen + dlen;
	size_t plen = PADC(clen);
#ifdef CONFIG_SCTP_ECN
	struct sctp_ecne *e;
	size_t elen = ((sp->sackf & SCTP_SACKF_ECN) ? sizeof(*e) : 0);
#endif
#ifdef CONFIG_SCTP_ECN
	plen += PADC(elen);
#endif
	if ((1 << sk->state) & ~(SCTPF_RECEIVING))
		goto outstate;
	if (clen > sd->dmps) {
		size_t too_many_dups;
		rare();		/* trim down sack */
		too_many_dups = (clen - *mrem + 3) / sizeof(uint32_t);
		ndups = (ndups > too_many_dups) ? ndups - too_many_dups : 0;
		clen = sizeof(*m) + glen + ndups * sizeof(uint32_t);
		if (*mrem < clen) {
			size_t too_many_gaps;
			rare();	/* trim some more */
			too_many_gaps = (clen - *mrem + 3) / sizeof(uint32_t);
			ngaps = (ngaps > too_many_gaps) ? ngaps - too_many_gaps : 0;
			clen = sizeof(*m) + ngaps * sizeof(uint32_t);
		}
	}
	if (plen >= *mrem && plen <= sd->dmps)	/* wait for next packet */
		return (1);
	if (!(skb = alloc_skb(plen, GFP_ATOMIC)))
		return (0);
	skb_set_owner_w(skb, sk);
	{
		unsigned char *p;
		struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
		struct sctp_skb_cb *gap = sp->gaps;
		struct sctp_skb_cb *dup = sp->dups;
		memset(cb, 0, sizeof(*cb));
		cb->skb = skb;
		cb->dlen = clen;	/* XXX */
		memset((p = skb_put(skb, plen)) + clen, 0, plen - clen);
		m = ((struct sctp_sack *) (p))++;
		m->ch.type = SCTP_CTYPE_SACK;
		m->ch.flags = 0;
		m->ch.len = htons(clen);
		m->c_tsn = htonl(sp->r_ack);
		m->a_rwnd = htonl(sp->a_rwnd);
		m->ngaps = htons(ngaps);
		m->ndups = htons(ndups);
		for (; gap && ngaps; gap = gap->next, ngaps--) {
			*((uint16_t *) p)++ = htons(gap->tsn - sp->r_ack);
			gap = gap->tail;
			*((uint16_t *) p)++ = htons(gap->tsn - sp->r_ack);
		}
		for (; dup && ndups; dup = dup->next, ndups--)
			*((uint32_t *) p)++ = htonl(dup->tsn);
		__skb_queue_purge(&sp->dupq);
		sp->ndups = 0;
		sp->dups = NULL;
#ifdef CONFIG_SCTP_ECN
		if (sp->sackf & SCTP_SACKF_ECN) {
			p = skb_put(skb, PADC(elen));
			e = (typeof(e)) p;
			e->ch.type = SCTP_CTYPE_ECNE;
			e->ch.flags = 0;
			e->ch.len = __constant_htons(sizeof(*e));
			e->l_tsn = htonl(sp->l_lsn);
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS
			SCTP_INC_STATS(SctpOutCtrlChunks);
#endif
		}
#endif
		sp->sackf &= ~SCTP_SACKF_ANY;
		del_timer(&sp->timer_sack);
		*mrem = (*mrem > plen) ? *mrem - plen : 0;
		*mlen += plen;
		**spp = cb;
		*spp = &cb->next;
		cb->next = NULL;
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS
		SCTP_INC_STATS(SctpOutCtrlChunks);
#endif
		return (0);
	}
      outstate:
	sp->sackf &= ~SCTP_SACKF_ANY;
	return (0);
}

#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
/*
 *  BUNDLE FORWARD SEQUENCE NUMBER
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int sctp_bundle_fsn(struct sock *sk, struct sctp_daddr *sd,
				  struct sctp_skb_cb ***spp, size_t * mrem,
				  size_t * mlen)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	struct sctp_forward_tsn *m;
	size_t clen = sizeof(*m);
	size_t plen;
	size_t nstrs = 0;
	size_t mstrs = (sd->dmps - sizeof(*m)) / sizeof(uint32_t);
	uint32_t l_fsn = sp->t_ack;
	if ((1 << sk->state) & ~(SCTPF_SENDING))
		goto outstate;
	if (!after(sp->l_fsn, sp->t_ack))
		goto outstate;
	for (skb = skb_peek(&sp->rtxq); skb; skb = skb_next(skb)) {
		struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
		if (!after(cb->tsn, sp->t_ack))
			continue;
		if (after(cb->tsn, sp->l_fsn))
			break;
		l_fsn = cb->tsn;
		if (cb->st && !(cb->flags & SCTPCB_FLAG_URG) &&
		    !(cb->st->n.more & SCTP_STRMF_DROP)) {
			cb->st->n.more |= SCTP_STRMF_DROP;
			nstrs++;
			if (nstrs >= mstrs)
				break;
		}
	}
	clen += nstrs * sizeof(uint32_t);
	plen = PADC(clen);
	if (plen >= *mrem && plen <= sd->dmps)	/* wait for next packet */
		return (1);
	if (!(skb = alloc_skb(plen, GFP_ATOMIC)))
		return (0);
	skb_set_owner_w(skb, sk);
	{
		unsigned char *p;
		struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
		memset(cb, 0, sizeof(*cb));
		cb->skb = skb;
		cb->dlen = clen;	/* XXX */
		memset((p = skb_put(skb, plen)) + clen, 0, plen - clen);
		m = ((struct sctp_forward_tsn *) (p))++;
		m->ch.type = SCTP_CTYPE_FORWARD_TSN;
		m->ch.flags = 0;
		m->ch.len = htons(clen);
		m->f_tsn = htonl(l_fsn);
		/* run backwards to build stream number list */
		for (skb = skb_peek_tail(&sp->rtxq); skb; skb = skb_prev(skb)) {
			if (after(cb->tsn, l_fsn))
				continue;
			if (!after(cb->tsn, sp->t_ack))
				break;
			if (cb->st && !(cb->flags & SCTPCB_FLAG_URG) &&
			    (cb->st->n.more & SCTP_STRMF_DROP)) {
				*((uint16_t *) p)++ = cb->st->sid;
				*((uint16_t *) p)++ = cb->ssn;
				cb->st->n.more &= ~SCTP_STRMF_DROP;
				assure(nstrs);
				nstrs--;
			}
		}
		assure(!nstrs);
		sp->sackf &= ~SCTP_SACKF_FSN;
		if (!(sp->sackf & SCTP_SACKF_ANY))
			del_timer(&sp->timer_sack);
		sd->flags |= SCTP_DESTF_FORWDTSN;
		*mrem = (*mrem > plen) ? *mrem - plen : 0;
		*mlen += plen;
		**spp = cb;
		*spp = &cb->next;
		cb->next = NULL;
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS
		SCTP_INC_STATS(SctpOutCtrlChunks);
#endif
		return (0);
	}
      outstate:
	sp->sackf &= ~SCTP_SACKF_FSN;
	if (!(sp->sackf & SCTP_SACKF_ANY))
		del_timer(&sp->timer_sack);
	return (0);
}
#endif

#ifdef CONFIG_SCTP_ECN
/*
 *  BUNDLE CONGESTION WINDOW REDUCTION
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int
sctp_bundle_cwr(struct sock *sk, struct sctp_daddr *sd, struct sctp_skb_cb ***spp, size_t * mrem, size_t * mlen)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	struct sctp_cwr *m;
	size_t clen = sizeof(*m);
	size_t plen = PADC(clen);
	if ((1 << sk->state) & ~(SCTPF_RECEIVING))
		goto outstate;
	if (clen >= *mrem && plen <= sd->dmps)	/* wait for next packet */
		return (1);
	if (!(skb = alloc_skb(plen, GFP_ATOMIC)))
		return (0);
	skb_set_owner_w(skb, sk);
	{
		struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
		m = ((typeof(m)) skb_put(skb, plen));
		m->ch.type = SCTP_CTYPE_CWR;
		m->ch.flags = 0;
		m->ch.len = __constant_htons(sizeof(*m));
		m->l_tsn = htonl(sp->p_lsn);
		sp->sackf &= ~SCTP_SACKF_CWR;
		*mrem = (*mrem > plen) ? *mrem - plen : 0;
		*mlen += plen;
		**spp = cb;
		*spp = &cb->next;
		cb->next = NULL;
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS
		SCTP_INC_STATS(SctpOutCtrlChunks);
#endif
		return (0);
	}
      outstate:
	sp->sackf &= ~SCTP_SACKF_CWR;
	return (0);
}
#endif

/*
 *  BUNDLE ERROR
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int
sctp_bundle_error(struct sock *sk, struct sctp_daddr *sd, struct sctp_skb_cb ***spp, size_t * mrem, size_t * mlen)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	if ((1 << sk->state) & (SCTPF_COOKIE_ECHOED | SCTPF_DISCONNECTED))
		goto outstate;
	while (*mrem && (skb = skb_peek(&sp->errq))) {
		struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
		size_t plen = skb->len;
		if (plen > *mrem && plen <= sd->dmps)
			goto wait_for_next_packet;
		*mrem = (*mrem > plen) ? *mrem - plen : 0;
		*mlen += plen;
		skb_unlink(skb);
		**spp = cb;
		*spp = &cb->next;
		cb->next = NULL;
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS
		SCTP_INC_STATS(SctpOutCtrlChunks);
#endif
	}
	return (0);
      wait_for_next_packet:
	rare();
	return (1);
      outstate:
	swerr();
	return (0);
}

/*
 *  BUNDLE DATA for RETRANSMISSION
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int
sctp_bundle_retrans(struct sock *sk, struct sctp_daddr *sd, struct sctp_skb_cb ***spp, size_t * mrem, size_t * mlen)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	for (skb = skb_peek(&sp->rtxq); *mrem && skb; skb = skb_next(skb)) {
		struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
		if (cb->flags & SCTPCB_FLAG_RETRANS) {
			size_t dlen = cb->dlen;
			size_t plen = PADC(sizeof(struct sctp_data) + dlen);
			if (plen > *mrem && plen <= sd->dmps)
				goto wait_for_next_packet;
			cb->flags &= ~SCTPCB_FLAG_RETRANS;
			ensure(sp->nrtxs > 0, sp->nrtxs = 1);
			sp->nrtxs--;
			sk->wmem_queued -= PADC(skb->len);
			ensure(sk->wmem_queued >= 0, sk->wmem_queued = 0);
			cb->when = jiffies;
			cb->daddr = sd;
			sd->in_flight += dlen;
			sp->in_flight += dlen;
			*mrem = (*mrem > plen) ? *mrem - plen : 0;
			*mlen += plen;
			**spp = cb;
			*spp = &cb->next;
			cb->next = NULL;
			skb_get(skb);
		}
	}
	return (0);
      wait_for_next_packet:
	rare();
	return (1);
}

/*
 *  BUNDLE NEW EXPEDITED (OUT OF ORDER) DATA
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int
sctp_bundle_urgent(struct sock *sk, struct sctp_daddr *sd, struct sctp_skb_cb ***spp, size_t * mrem, size_t * mlen)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	while (*mrem && (skb = skb_peek(&sp->urgq))) {
		struct sctp_data *m = (typeof(m)) skb->data;
		struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
		size_t dlen = cb->dlen;
		size_t plen = PADC(sizeof(struct sctp_data) + dlen);
		ensure(cb->st, continue);
		if (plen > *mrem && plen <= sd->dmps)
			goto wait_for_next_packet;
		if ((skb == cb->st->x.head)) {
			rare();
			cb->st->x.head = NULL;	/* steal partial */
			sk->wmem_queued += PADC(skb->len);
		}
		cb->tsn = sp->t_tsn++;
		cb->flags |= SCTPCB_FLAG_SENT;
		cb->when = jiffies;
		cb->daddr = sd;
		m->tsn = htonl(cb->tsn);
		sd->in_flight += dlen;
		sp->in_flight += dlen;
		*mrem = (*mrem > plen) ? *mrem - plen : 0;
		*mlen += plen;
		**spp = cb;
		*spp = &cb->next;
		cb->next = NULL;
		skb_unlink(skb);
		sk->wmem_queued -= PADC(skb->len);
		ensure(sk->wmem_queued > 0, sk->wmem_queued = 0);
		skb_get(skb);
		__skb_queue_tail(&sp->rtxq, skb);
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTUNORDERCHUNKS
		SCTP_INC_STATS(SctpOutUnorderChunks);
#endif
	}
	return (0);
      wait_for_next_packet:
	rare();
	return (1);
}

/*
 *  BUNDLE NEW NORMAL (ORDERED) DATA
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE int
sctp_bundle_normal(struct sock *sk, struct sctp_daddr *sd, struct sctp_skb_cb ***spp, size_t * mrem, size_t * mlen)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	while (*mrem && (skb = skb_peek(&sk->sndq))) {
		struct sctp_data *m = (typeof(m)) skb->data;
		struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
		size_t dlen = cb->dlen;
		size_t plen = PADC(sizeof(*m) + dlen);
		ensure(cb->st, continue);
		if (plen > *mrem && plen <= sd->dmps)
			goto wait_for_next_packet;
		if ((skb == cb->st->n.head)) {
			seldom();
			cb->st->n.head = NULL;	/* steal partial */
			sk->wmem_queued += PADC(skb->len);
		}
		cb->tsn = sp->t_tsn++;
		cb->flags |= SCTPCB_FLAG_SENT;	/* this is data */
		cb->when = jiffies;
		cb->daddr = sd;
		m->tsn = htonl(cb->tsn);
		sd->in_flight += dlen;
		sp->in_flight += dlen;
		*mrem = (*mrem > plen) ? *mrem - plen : 0;
		*mlen += plen;
		**spp = cb;
		*spp = &cb->next;
		cb->next = NULL;
		skb_unlink(skb);
		sk->wmem_queued -= PADC(skb->len);
		ensure(sk->wmem_queued >= 0, sk->wmem_queued = 0);
		skb_get(skb);
		__skb_queue_tail(&sp->rtxq, skb);
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTORDERCHUNKS
		SCTP_INC_STATS(SctpOutOrderChunks);
#endif
	}
	return (0);
      wait_for_next_packet:
	seldom();
	return (1);
}

/*
 *  BUNDLE CHUNKS
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Tack on SACK, ERROR, and DATA chunks up to the destination MTU considering
 *  congestion windows and fragmentation sizes.
 */
STATIC int sctp_bundle_more(struct sock *sk, struct sctp_daddr *sd,
			    struct sk_buff *skb, int nonagle)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_skb_cb *cb = SCTP_SKB_CB(skb), **spp = &cb->next;
	size_t mlen = skb->len;
	size_t load = mlen - sizeof(struct sctphdr);
	size_t mrem = (load < sd->dmps) ? sd->dmps - load : 0;
	size_t swnd = sctp_avail(sp, sd);
	size_t pbuf = sp->p_rbuf >> 1;
	int max = 0;
	if (((sp->sackf & SCTP_SACKF_NOW) && !sk->backlog.head)
	    || (sp->nrtxs)
	    || (!skb_queue_empty(&sp->urgq))
	    || (!skb_queue_empty(&sp->errq))
	    || ((!sd->in_flight || nonagle == 1) && sk->wmem_queued)
	    /* SCTP IG 2.15.2, SWS avoidance sending RFC 1122 4.2.3.4 */
	    || (sk->wmem_queued >= mrem && swnd >= mrem)
	    || (sk->wmem_queued >= pbuf && swnd >= pbuf)
	    /* Manshall modification */
	    || !before(sp->m_tsn, sp->t_ack)
	    ) {
		*spp = NULL;
		if (mrem && (sp->sackf & SCTP_SACKF_ANY))
			sctp_bundle_sack(sk, sd, &spp, &mrem, &mlen);
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
		if (mrem && (sp->sackf & SCTP_SACKF_FSN))
			sctp_bundle_fsn(sk, sd, &spp, &mrem, &mlen);
#endif
		if (mrem && !skb_queue_empty(&sp->errq))
			sctp_bundle_error(sk, sd, &spp, &mrem, &mlen);
#ifdef CONFIG_SCTP_ECN
		/* really only supposed to be bundled with new data */
		if (mrem && (sp->sackf & SCTP_SACKF_CWR) && sk->wmem_queued)
			sctp_bundle_cwr(sk, sd, &spp, &mrem, &mlen);
#endif
		if (swnd) {	/* subject to flow control */
			uint32_t t_tsn = sp->t_tsn;
			int was_in_flight = sd->in_flight;
			if (mrem && !skb_queue_empty(&sp->urgq))
				max |= sctp_bundle_urgent
				    (sk, sd, &spp, &mrem, &mlen);
			if (mrem && sp->nrtxs)
				max |= sctp_bundle_retrans
				    (sk, sd, &spp, &mrem, &mlen);
			if (mrem && !skb_queue_empty(&sk->sndq))
				max |= sctp_bundle_normal
				    (sk, sd, &spp, &mrem, &mlen);
			cb->dlen = mlen;
			if ((sd->in_flight || (sd->flags & SCTP_DESTF_FORWDTSN)) &&
			    !timer_pending(&sd->timer_retrans)) {
				sctp_mod_timer(&sd->timer_retrans, sd->rto);
			}
			if (!max && sk->wmem_queued > 0 && after(sp->t_tsn, t_tsn))
				sp->m_tsn = t_tsn;	/* sent a short packet */
			else if (!was_in_flight)
				sp->m_tsn = sp->t_ack;
		}
	}
	return (max);
}

/*
 *  ALLOC CHK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Allocate an sk_buff for a chunk without hard-header, IP header, or SCTP
 *  header headroom.  This is used for allocating sk_buffs which are used to
 *  contain a chunk for bundling.
 */
STATIC struct sk_buff *sctp_alloc_chk(struct sock *sk, size_t clen)
{
	struct sk_buff *skb;
	size_t plen = PADC(clen);
	if ((skb = alloc_skb(plen, GFP_ATOMIC))) {
		struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
		skb_set_owner_w(skb, sk);
		memset(cb, 0, sizeof(*cb));
		cb->skb = skb;
		cb->dlen = plen;
		/* zero padding */
		memset(skb->tail + clen, 0, plen - clen);
	}
	return (skb);
}

/*
 *  ALLOC MSG
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Allocates a new message block with a control block, an SCTP message header,
 *  room for an IP header and hard header and the first chunk of a message.
 */
STATIC struct sk_buff *sctp_alloc_msg(struct sock *sk, size_t clen)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	struct sctphdr *sh;
	size_t plen = PADC(clen);
	size_t mlen = sizeof(*sh) + plen;
	if ((skb = alloc_skb(mlen, GFP_ATOMIC))) {
		struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
		skb_set_owner_w(skb, sk);	/* XXX */
		memset(cb, 0, sizeof(*cb));
		cb->skb = skb;
		cb->dlen = mlen;
		/* pull the hard and IP header headroom */
		sh = (struct sctphdr *) skb_put(skb, sizeof(*sh));
		sh->srce = sk->sport;
		sh->dest = sk->dport;
		sh->v_tag = sp->p_tag;
		sh->check = 0;
		/* pre-zero padding */
		memset(skb->tail + clen, 0, plen - clen);
	}
	return (skb);
}

/*
 *  ALLOC REPLY
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This is for allocating a message block with a transmission control block for
 *  a message with IP header room and an STCP header.  This is for out of the
 *  blue replies and all that is required is the SCTP header of the message to
 *  which this is a reply.
 */
STATIC struct sk_buff *sctp_alloc_reply(struct sctphdr *rh, size_t clen)
{
	struct sk_buff *skb;
	struct sctphdr *sh;
	size_t plen = PADC(clen);
	size_t mlen = sizeof(*sh) + plen;
	size_t hlen = MAX_HEADER + sizeof(struct iphdr);
	ensure(rh, return NULL);
	if ((skb = alloc_skb(hlen + mlen, GFP_ATOMIC))) {
		struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
		memset(cb, 0, sizeof(*cb));
		cb->skb = skb;
		cb->dlen = mlen;
		skb_reserve(skb, hlen);
		sh = (struct sctphdr *) skb_put(skb, sizeof(*sh));
		sh->srce = rh->dest;
		sh->dest = rh->srce;
		sh->v_tag = rh->v_tag;
		sh->check = 0;
		/* pre-zero padding */
		memset(skb->tail + clen, 0, plen - clen);
	}
	return (skb);
}

/*
 *  FREE CHUNKS
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This is for freeing a chain list of sk_buffs that represent a packet built
 *  of of separate chunks.
 */
STATIC INLINE void freechunks(struct sk_buff *skb)
{
	if (skb) {
		struct sctp_skb_cb *cb, *cb_next = SCTP_SKB_CB(skb);
		while ((cb = cb_next)) {
			cb_next = cb->next;
			kfree_skb(SCTP_CB_SKB(cb));
		}
	}
}

STATIC INLINE void sctp_error_report(struct sock *sk, int err)
{
	if (sock_locked(sk) || !sk->protinfo.af_inet.recverr)
		sk->err_soft = err;
	else {
		sk->err = err;
		if (!sk->dead)
			sk->error_report(sk);
	}
}

/*
 *  ROUTE SELECTION
 *  -------------------------------------------------------------------------
 *  
 *  ROUTE NORMAL
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Send to the normal transmit (primary) address.  If that has timedout, we use
 *  the retransmit (secondary) address.  We always use the secondary address if
 *  we have retransmit chunks to bundle or if we have be receiving duplicates
 *  (our SACKs are not getting through on the primary address).
 *
 */
STATIC INLINE void sctp_discon_ind(struct sock *sk, int err);
STATIC struct sctp_daddr *sctp_route_normal(struct sock *sk)
{
	int err;
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_daddr *sd;
	if ((err = sctp_update_routes(sk, 1))) {
		rare();
		/* we have no viable route */
		if ((1 << sk->state) & (SCTPF_OPENING)) {
			if (sock_locked(sk))
				sk->err_soft = err;
			else
				sctp_discon_ind(sk, err);
			return (NULL);
		}
		sctp_error_report(sk, err);
		return (NULL);
	}
	sd = sp->taddr;
	normal(sd);
	return (sd);
}

/*
 *  ROUTE RESPONSE
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  In response to control chunks we normally send back to the address that the
 *  control chunk came from.  If that address is unusable or wasn't provided
 *  or is suspect we send as normal.
 */
STATIC struct sctp_daddr *sctp_route_response(struct sock *sk)
{
	struct sctp_daddr *sd = SCTP_PROT(sk)->caddr;
	if (!sd || !sd->dst_cache || sd->dst_cache->obsolete || sd->retransmits || sd->dups)
		sd = sctp_route_normal(sk);
	normal(sd);
	return (sd);
}

/*
 *  WAKEUP (Send SACK, ERROR, DATA)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  TODO: When the user has specified a destination address to sctp_sendmsg()
 *  we want to suppress nagle and do a destination-specific bundling instead
 *  of our normal route selection.  We could do this by peeking the urgq and
 *  the sndq to check for user-specified destinations before selecting the
 *  destination and calling sctp_bundle_more.  This looks like it will have a
 *  performance impact so we will leave it for now.
 */
#ifdef CONFIG_SCTP_ADD_IP
STATIC void sctp_send_asconf(struct sock *sk);
#endif
STATIC void sctp_transmit_wakeup(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	int i, n, reroute = 0;
	struct sk_buff *skb;
	struct sctp_daddr *sd;
	int loop_max = 1000;
	printd(("INFO: Performing transmitter wakeup\n"));
	ensure(sk, return);
	if ((1 << sk->state) & ~(SCTPF_SENDING | SCTPF_RECEIVING))
		goto skip;
#ifdef CONFIG_SCTP_ADD_IP
	if (sk->state == SCTP_ESTABLISHED && sp->sackf & SCTP_SACKF_ASC)
		sctp_send_asconf(sk);
#endif
	/* SCTP IG 2.8, 2.14 */
//	for (i = 0, n = 0; i < loop_max && n <= sp->max_burst; i++) {
	for (i = 0, n = 0; i < loop_max; i++) {
		/* placed in order of probability */
		if (!sk->wmem_queued && (!(sp->sackf & SCTP_SACKF_NOW) || sk->backlog.head)
		    && skb_queue_empty(&sp->errq))
			goto done;
		if (!(sd = sctp_route_normal(sk)))
			goto noroute;
		if (!(skb = sctp_alloc_msg(sk, 0)))
			goto enobufs;
		n += sctp_bundle_more(sk, sd, skb, sp->nonagle);
		if (SCTP_SKB_CB(skb)->next) {
			reroute = 0;
			sctp_send_msg(sk, sd, skb);
			freechunks(skb);
			continue;
		}
		if (!reroute) {
			reroute = 1;
			kfree_skb(skb);
			continue;
		}
		goto discard;
	}
	assure(i < loop_max);
	return;
      discard:
	kfree_skb(skb);
	goto done;
      noroute:
	ptrace(("ERROR: no route to peer\n"));
	goto done;
      enobufs:
	ptrace(("ERROR: couldn't allocate buffer\n"));
	goto done;
      done:
	assure(i > 0 || !(sp->sackf & SCTP_SACKF_NOW) || sk->backlog.head);
      skip:
	return;
}

/*
 *  =========================================================================
 *
 *  SCTP State Machine Indications
 *
 *  =========================================================================
 */
/*
 *  -------------------------------------------------------------------------
 *
 *  (EX)DATA Indication
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE void sctp_data_ind(struct sock *sk, int len)
{
	printd(("INFO: Data indication on socket %p\n", sk));
	if (!sk->dead)
		sk->data_ready(sk, len);
	else
		swerr();
}
STATIC INLINE void sctp_exdata_ind(struct sock *sk, int len)
{
	printd(("INFO: Data indication on socket %p\n", sk));
	if (!sk->dead) {
		if (sk->proc != 0) {
			if (sk->proc > 0)
				kill_proc(sk->proc, SIGURG, 1);
			else
				kill_pg(-sk->proc, SIGURG, 1);
			sk_wake_async(sk, 3, POLL_PRI);
		}
	} else
		swerr();
}

/*
 *  -------------------------------------------------------------------------
 *
 *  CONNECTION Indication
 *
 *  -------------------------------------------------------------------------
 */
STATIC INLINE void sctp_conn_ind(struct sock *sk, struct sk_buff *skb)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	printd(("INFO: Connection indication on socket %p\n", sk));
	skb_queue_tail(&sp->accq, skb_get(skb));
	sk->ack_backlog++;
	if (!sk->dead)
		sk->data_ready(sk, 0);
	else
		swerr();
	return;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  DISCONNECT Indication
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  SCTP DESTROY OPHANED SOCKET
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This is for destroying an orphaned socket.  Orphaned sockets will never
 *  have their sk->prot->destroy functions called by socket code because they
 *  have been orphaned from the socket.  This function calls the destructor
 *  for the socket, drains socket queues, and does the final put on the
 *  socket.
 */
STATIC void sctp_destroy_orphan(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	printd(("INFO: Destroying orphan socket %p\n", sk));
#ifdef CONFIG_SCTP_DEBUG
	if (sk->zapped) {
		ptrace(("ERROR: double destroy sk = %p\n", sk));
		sock_hold(sk);
	}
	sk->zapped = 1;
#endif
	sk->prot->destroy(sk);
	__skb_queue_purge(&sk->rcvq);
	__skb_queue_purge(&sp->expq);
	sp->rmem_queued = 0;
	__skb_queue_purge(&sk->errq);
	__skb_queue_purge(&sk->sndq);
	__skb_queue_purge(&sp->urgq);
	sk->wmem_queued = 0;
#if defined HAVE_XFRM_POLICY_DELETE_EXPORT || defined HAVE_XFRM_POLICY_DELETE_ADDR
	xfrm_sk_free_policy(sk);
#endif
	atomic_dec(&sctp_orphan_count);
	printd(("INFO: There are now %d orphan sockets\n",
		atomic_read(&sctp_orphan_count)));
	if (atomic_read(&sk->refcnt) != 1)
		ptrace(("Delayed close socket %p, with refcnt = %d\n", sk,
			atomic_read(&sk->refcnt) - 1));
	sock_put(sk);
}

STATIC INLINE void sctp_discon_ind(struct sock *sk, int err)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	printd(("INFO: Disconnect indication on socket %p\n", sk));
	if (sk->state == SCTP_LISTEN)
		goto outstate;
	local_bh_disable();
	sctp_change_state(sk, SCTP_CLOSED);
	/* unhash and remove address lists */
	sk->prot->unhash(sk);
	/* stop timers */
	del_timer(&sp->timer_init);
	del_timer(&sp->timer_cookie);
	del_timer(&sp->timer_shutdown);
	del_timer(&sp->timer_guard);
	del_timer(&sp->timer_sack);
#ifdef CONFIG_SCTP_ADD_IP
	del_timer(&sp->timer_asconf);
#endif
#if defined(CONFIG_SCTP_LIFETIMES) || defined(CONFIG_SCTP_PARTIAL_RELIABILITY)
	del_timer(&sp->timer_life);
#endif
	local_bh_enable();
	if ((sk->err = err) && !sk->dead)
		sk->error_report(sk);
	sk->shutdown = SHUTDOWN_MASK;
	sp->sackf = 0;
	if (!sk->dead)
		sk->state_change(sk);
	else
		sctp_destroy_orphan(sk);
	return;
      outstate:
	swerr();
	return;
}

/*
 *  -------------------------------------------------------------------------
 *
 *  ORDERLY RELEASE Indication
 *
 *  -------------------------------------------------------------------------
 */
STATIC void sctp_ordrel_ind(struct sock *sk)
{
	printd(("INFO: Orderly release indication on socket %p\n", sk));
	sk->shutdown |= RCV_SHUTDOWN;
	sk->done = 1;
	if (!sk->dead) {
		sk->state_change(sk);
		if (sk->shutdown == SHUTDOWN_MASK && sk->state == SCTP_CLOSED) {
			if (sk->socket)
				sk_wake_async(sk, 1, POLL_HUP);
		} else {
			if (sk->socket)
				sk_wake_async(sk, 1, POLL_IN);
		}
	}
	return;
}

/*
 *  =========================================================================
 *
 *  SCTP State Machine TIMEOUTS
 *
 *  =========================================================================
 */
#ifdef CONFIG_SCTP_ECN
STATIC INLINE void sctp_send_cwr(struct sock *sk);
#endif
STATIC void sctp_send_heartbeat(struct sock *sk, struct sctp_daddr *sd);
STATIC void sctp_reset_idle(struct sctp_daddr *sd);
/*
 *  ASSOCIATION TIMEOUT FUNCTION
 *  -------------------------------------------------------------------------
 */
STATIC int sctp_assoc_timedout(struct sock *sk, struct sctp_daddr *sd, size_t rmax,
			       int stroke)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	ensure(sk, return (-EFAULT));
	ensure(sd, return (-EFAULT));
	/* RFC 2960 6.3.3 E1 and 7.2.3, E2, E3 and 8.3 */
	sd->ssthresh = ((sd->cwnd >> 1) > (sd->mtu << 1)) ? sd->cwnd >> 1 : sd->mtu << 1;
	sd->cwnd = sd->mtu;
	/* SCTP IG Section 2.9 */
	sd->partial_ack = 0;
	sd->rto = (sd->rto) ? sd->rto << 1 : 1;
	sd->rto = (sd->rto_min > sd->rto) ? sd->rto_min : sd->rto;
	sd->rto = (sd->rto_max < sd->rto) ? sd->rto_max : sd->rto;
	ptrace(("Timeout calculation:\n  sd->retransmits = %d\n  sp->retransmits = %d\n  ssthresh = %d\n  cwnd = %d\n  rto = %lu\n", sd->retransmits, sp->retransmits, sd->ssthresh, sd->cwnd, sd->rto));
#ifdef CONFIG_SCTP_ECN
	sctp_send_cwr(sk);
#endif
	/* SCTP IG 2.15: don't stroke counts on zero window probes, or ECN */
	if (!stroke && sk->state != SCTP_COOKIE_WAIT)
		return (0);
	/* See RFC 2960 Section 8.3 */
	if (++sd->retransmits > sd->max_retrans) {
		if (sd->retransmits > sd->max_retrans + 1) {
			if ((1 << sk->state) & (SCTPF_CONNECTED))
				return (0);
		} else {
			if (sd->dst_cache)
				dst_negative_advice(&sd->dst_cache);
#if 0
			if (sd->dst_cache)
				dst_release(xchg(&sd->dst_cache, NULL));
#endif
			if (!sd->dst_cache)
				sd->saddr = 0;
#if 1
			/* 
			 *  IMPLEMENTATION NOTE:-  When max_retrans and
			 *  rto_max are set to zero, we are cruel on
			 *  destinations that drop a single packet due to
			 *  noise.  We force an immediate heartbeat on the
			 *  destination so that it can be made available again
			 *  quickly if it passes the heartbeat.
			 */
			if (sd->rto_max < 2 && sd->max_retrans == 0 &&
			    (1 << sk->state) & (SCTPF_CONNECTED | SCTPF_CLOSING)) {
				del_timer(&sd->timer_idle);
				sctp_send_heartbeat(sk, sd);
			}
#endif
		}
	}
	/* See RFC 2960 Section 8.2 */
	if (rmax && sp->retransmits++ >= rmax) {
		seldom();
		sctp_discon_ind(sk, sk->err_soft ? sk->err_soft : ETIMEDOUT);
		return (-sk->err);
	}
	return (0);
}

/*
 *  INIT TIMEOUT (T1-init)
 *  -------------------------------------------------------------------------
 *  The init timer has expired indicating that we have not received an INIT ACK
 *  within timer T1-init.  This means that we should attempt to retransmit the
 *  INIT until we have attempted Max.Init.Retrans times.
 */
STATIC void sctp_init_timeout(unsigned long data)
{
	struct sock *sk = (struct sock *) data;
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_daddr *sd;
	ensure(sk, return);
	sd = sp->taddr;
	ensure(sd, return);
	bh_lock_sock(sk);
	if (sock_locked(sk))
		goto locked;
	if (sk->state != SCTP_COOKIE_WAIT)
		goto outstate;
	if (sctp_assoc_timedout(sk, sd, sp->max_inits ? sp->max_inits : 1, sp->p_rwnd))
		goto done;
	sd = sp->taddr;		/* might have new primary */
	ensure(sd, goto done);
	sctp_mod_timer(&sp->timer_init, sd->rto);
	normal(sp->retry);
	sctp_send_msg(sk, sd, sp->retry);
      done:
	bh_unlock_sock(sk);
	return;
      locked:
	ptrace(("WARNING: hit locks\n"));
	sctp_mod_timer(&sp->timer_init, SCTP_TIMER_BACKOFF);
	goto done;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
}

/*
 *  COOKIE TIMEOUT
 *  -------------------------------------------------------------------------
 *  The cookie timer has expired indicating that we have not yet received a
 *  COOKIE ACK within timer T1-cookie.  This means that we should attempt to
 *  retransmit the COOKIE ECHO until we have attempted Path.Max.Retrans times.
 */
STATIC void sctp_cookie_timeout(unsigned long data)
{
	struct sock *sk = (struct sock *) data;
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_daddr *sd;
	struct sk_buff *skb;
	ensure(sk, return);
	sd = sp->taddr;
	ensure(sd, return);
	bh_lock_sock(sk);
	if (sock_locked(sk))
		goto locked;
	if (sk->state != SCTP_COOKIE_ECHOED)
		goto outstate;
	if (sctp_assoc_timedout(sk, sd, sp->max_retrans ? sp->max_retrans : 1, sp->p_rwnd))
		goto done;
	/* See RFC 2960 6.3.3 E3 */
	for (skb = skb_peek(&sp->rtxq); skb; skb = skb_next(skb)) {
		struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
		seldom();
		if (cb->daddr == sd && (cb->flags & SCTPCB_FLAG_SENT)
		    && !(cb->flags & (SCTPCB_FLAG_RETRANS | SCTPCB_FLAG_DROPPED))) {
			cb->flags |= SCTPCB_FLAG_RETRANS;
			sp->nrtxs++;
			sk->wmem_queued += PADC(skb->len);
			cb->sacks = SCTP_FR_COUNT;	/* not eligble for FR now */
		}
	}
	sd = sp->taddr;		/* might have new primary */
	ensure(sd, goto done);
	sctp_mod_timer(&sp->timer_cookie, sd->rto);
	normal(sp->retry);
	sctp_send_msg(sk, sd, sp->retry);
      done:
	bh_unlock_sock(sk);
	return;
      locked:
	ptrace(("WARNING: hit locks\n"));
	sctp_mod_timer(&sp->timer_cookie, SCTP_TIMER_BACKOFF);
	goto done;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
}

/*
 *  RETRANS TIMEOUT (T3-rtx)
 *  -------------------------------------------------------------------------
 *  This means that we have not received an ack for a DATA chunk within timer
 *  T3-rtx.  This means that we should mark all outstanding DATA chunks for
 *  retransmission and start a retransmission cycle.
 */
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
STATIC INLINE void sctp_send_forward_tsn(struct sock *sk);
#endif
STATIC void sctp_retrans_timeout(unsigned long data)
{
	struct sctp_daddr *sd = (struct sctp_daddr *) data;
	struct sctp_opt *sp;
	struct sock *sk;
	struct sk_buff *skb;
	int retransmits = 0;
	ensure(sd, return);
	sp = sd->sp;
	ensure(sp, return);
	sk = SCTP_SOCK(sp);
	bh_lock_sock(sk);
	if (sock_locked(sk))
		goto locked;
	if (!((1 << sk->state) & (SCTPF_CONNECTED)))
		goto outstate;
	if (sctp_assoc_timedout
	    (sk, sd, sp->max_retrans ? sp->max_retrans : 1, sp->p_rwnd))
		goto done;
	/* See RFC 2960 6.3.3 E3 */
	for (skb = skb_peek(&sp->rtxq); skb; skb = skb_next(skb)) {
		struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
		size_t dlen = cb->dlen;
		if (cb->daddr == sd && (cb->flags & SCTPCB_FLAG_SENT) &&
		    !(cb->flags & (SCTPCB_FLAG_RETRANS | SCTPCB_FLAG_SACKED |
				   SCTPCB_FLAG_DROPPED))) {
			cb->flags |= SCTPCB_FLAG_RETRANS;
			sp->nrtxs++;
			sk->wmem_queued += PADC(skb->len);
			cb->sacks = SCTP_FR_COUNT;	/* not eligble for FR now */
			normal(sd->in_flight >= dlen);
			normal(sp->in_flight >= dlen);
			/* credit dest and assoc */
			sd->in_flight =
			    (sd->in_flight > dlen) ? sd->in_flight - dlen : 0;
			sp->in_flight =
			    (sp->in_flight > dlen) ? sp->in_flight - dlen : 0;
			retransmits++;
		}
	}
	normal(retransmits);
#ifdef CONFIG_SCTP_ADD_IP
	if (sd->flags & SCTP_DESTF_UNUSABLE)
		sctp_del_daddr(sd);
#endif
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
	if (after(sp->l_fsn, sp->t_ack)) {
		sctp_send_forward_tsn(sk);
		if (!timer_pending(&sp->timer_sack))
			sctp_mod_timer(&sp->timer_sack, sp->max_sack);
		sp->sackf |= SCTP_SACKF_NOD;	/* don't delay retransmitted FWD TSN */
	} else {
		sp->sackf &= ~SCTP_SACKF_FSN;
		if (!(sp->sackf & SCTP_SACKF_ANY))
			del_timer(&sp->timer_sack);
	}
#endif
	sctp_transmit_wakeup(sk);
      done:
	bh_unlock_sock(sk);
	return;
      locked:
	ptrace(("WARNING: hit locks\n"));
	sctp_mod_timer(&sd->timer_retrans, SCTP_TIMER_BACKOFF);
	goto done;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
}

/*
 *  SACK TIMEOUT
 *  -------------------------------------------------------------------------
 *  This timer is the 200ms timer which ensures that a SACK is sent within 200ms
 *  of the receipt of an unacknoweldged DATA chunk.  When an unacknowledged DATA
 *  chunks i receive and the timer is not running, the timer is set.  Whenever a
 *  DATA chunks(s) are acknowledged, the timer is stopped.
 */
STATIC void sctp_sack_timeout(unsigned long data)
{
	struct sock *sk = (struct sock *) data;
	struct sctp_opt *sp = SCTP_PROT(sk);
	ensure(sk, return);
	bh_lock_sock(sk);
	if (sock_locked(sk))
		goto locked;
	if (!((1 << sk->state) & (SCTPF_RECEIVING)))
		goto outstate;
	sp->sackf |= SCTP_SACKF_TIM;	/* RFC 2960 6.2 */
	sctp_transmit_wakeup(sk);
      done:
	bh_unlock_sock(sk);
	return;
      locked:
	ptrace(("WARNING: hit locks\n"));
	sctp_mod_timer(&sp->timer_sack, SCTP_TIMER_BACKOFF);
	goto done;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
}

/*
 *  IDLE TIMEOUT
 *  -------------------------------------------------------------------------
 *  This means that a destination has been idle for longer than the hb.itvl or
 *  the interval for which we must send heartbeats.  This timer is reset every
 *  time we do an RTT calculation for a destination.  It is stopped while
 *  sending heartbeats and started again whenever an RTT calculation is done.
 *  While this timer is stopped, heartbeats will be sent until they are
 *  acknowledged.
 */
STATIC void sctp_idle_timeout(unsigned long data)
{
	struct sctp_daddr *sd = (struct sctp_daddr *) data;
	struct sock *sk;
	ensure(sd, return);
	sk = SCTP_SOCK(sd->sp);
	bh_lock_sock(sk);
	if (sock_locked(sk))
		goto locked;
	/* SCTP IG 2.10 but we continue through closing states */
	if (!((1 << sk->state) & (SCTPF_CONNECTED | SCTPF_CLOSING)))
		goto outstate;
	sctp_send_heartbeat(sk, sd);
      done:
	bh_unlock_sock(sk);
	return;
      locked:
	ptrace(("WARNING: hit locks\n"));
	sctp_mod_timer(&sd->timer_idle, SCTP_TIMER_BACKOFF);
	goto done;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
}

/*
 *  HEARTBEAT TIMEOUT
 *  -------------------------------------------------------------------------
 *  If we get a heartbeat timeout we adjust RTO the same as we do for retransmit
 *  (and the congestion window) and resend the heartbeat.  Once we have sent
 *  Path.Max.Retrans heartbeats unsuccessfully, we mark the destination as
 *  unusable, but continue heartbeating until they get acknowledged.  (Well!
 *  That's not really true, is it?)
 */
STATIC void sctp_heartbeat_timeout(unsigned long data)
{
	struct sctp_daddr *sd = (struct sctp_daddr *) data;
	struct sock *sk;
	struct sctp_opt *sp;
	ensure(sd, return);
	sp = sd->sp;
	sk = SCTP_SOCK(sp);
	bh_lock_sock(sk);
	if (sock_locked(sk))
		goto locked;
	/* SCTP IG 2.10 but we continue through closing states */
	if (!((1 << sk->state) & (SCTPF_CONNECTED | SCTPF_CLOSING)))
		goto outstate;
	/* SCTP IG 2.10 */
	if (sctp_assoc_timedout(sk, sd, sp->max_retrans ? sp->max_retrans : 1, 1))
		goto done;
	sctp_reset_idle(sd);
//      sctp_send_heartbeat(sk, sd);
      done:
	bh_unlock_sock(sk);
	return;
      locked:
	ptrace(("WARNING: hit locks\n"));
	sctp_mod_timer(&sd->timer_heartbeat, SCTP_TIMER_BACKOFF);
	goto done;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
}

/*
 *  SHUTDOWN TIMEOUT
 *  -------------------------------------------------------------------------
 *  This means that we have timedout on sending a SHUTDOWN or a SHUTDOWN ACK
 *  message.  We simply resend the message.
 */
STATIC void sctp_shutdown_timeout(unsigned long data)
{
	struct sock *sk = (struct sock *) data;
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_daddr *sd;
	ensure(sk, return);
	sd = sp->taddr;
	ensure(sd, return);
	bh_lock_sock(sk);
	if (sock_locked(sk))
		goto locked;
	if (!((1 << sk->state) & (SCTPF_CLOSING)))
		goto outstate;
	if (sctp_assoc_timedout(sk, sd, sp->max_retrans ? sp->max_retrans : 1, sp->p_rwnd))
		goto done;
	if (!(sd = sctp_route_normal(sk)))
		goto done;
	sctp_mod_timer(&sp->timer_shutdown, sd->rto);
	normal(sp->retry);
	sctp_send_msg(sk, sd, sp->retry);
      done:
	bh_unlock_sock(sk);
	return;
      locked:
	ptrace(("WARNING: hit locks\n"));
	sctp_mod_timer(&sp->timer_shutdown, SCTP_TIMER_BACKOFF);
	goto done;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
}

/*
 *  GUARD TIMEOUT
 *  -------------------------------------------------------------------------
 *  This means that we have timed out in the over SHUTDOWN process and need to
 *  ABORT the association per SCTP IG 2.12.
 */
STATIC void sctp_send_abort(struct sock *sk);
STATIC void sctp_guard_timeout(unsigned long data)
{
	struct sock *sk = (struct sock *) data;
	struct sctp_opt *sp = SCTP_PROT(sk);
	bh_lock_sock(sk);
	if (sock_locked(sk))
		goto locked;
	if (!((1 << sk->state) & (SCTPF_CLOSING)))
		goto outstate;
	sctp_send_abort(sk);
	sctp_discon_ind(sk, sk->err_soft ? sk->err_soft : ETIMEDOUT);
      done:
	bh_unlock_sock(sk);
	return;
      locked:
	ptrace(("WARNING: hit locks\n"));
	sctp_mod_timer(&sp->timer_guard, SCTP_TIMER_BACKOFF);
	goto done;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
}

#ifdef CONFIG_SCTP_ADD_IP
/*
 *  ASCONF TIMEOUT
 *  -------------------------------------------------------------------------
 *  This means that we have timedout on sending a ASCONF message.  We simply
 *  resend the message.
 */
STATIC void sctp_asconf_timeout(unsigned long data)
{
	struct sock *sk = (struct sock *) data;
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_daddr *sd;
	ensure(sk, return);
	sd = sp->taddr;
	ensure(sd, return);
	bh_lock_sock(sk);
	if (sock_locked(sk))
		goto locked;
	if (!((1 << sk->state) & (SCTPF_ESTABLISHED)))
		goto outstate;
	/* ADD-IP (B1), (B2) and (B3) */
	if (sctp_assoc_timedout(sk, sd, sp->max_retrans ? sp->max_retrans : 1, sp->p_rwnd))
		goto done;
	if (!(sd = sctp_route_normal(sk)))
		goto done;
	/* ADD-IP (B5) */
	sctp_mod_timer(&sp->timer_asconf, sd->rto);
	/* ADD-IP (B4) */
	normal(sp->retry);
	sctp_send_msg(sk, sd, sp->retry);
      done:
	bh_unlock_sock(sk);
	return;
      locked:
	ptrace(("WARNING: hit locks\n"));
	sctp_mod_timer(&sp->timer_asconf, SCTP_TIMER_BACKOFF);
	goto done;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
}
#endif

#if defined(CONFIG_SCTP_LIFETIMES) || defined(CONFIG_SCTP_PARTIAL_RELIABILITY)
/*
 *  LIFETIME TIMEOUT
 *  -------------------------------------------------------------------------
 */
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
STATIC INLINE void sctp_send_forward_tsn(struct sock *sk);
#endif
STATIC void sctp_life_timeout(unsigned long data)
{
	struct sock *sk = (struct sock *) data;
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sk_buff *skb, *skb_next;
	unsigned long expires = -1UL;
	ensure(sk, return);
	bh_lock_sock(sk);
	if (sock_locked(sk))
		goto locked;
	if (!((1 << sk->state) & (SCTPF_SENDING)))
		goto outstate;
	skb_next = skb_peek(&sk->sndq);
	while ((skb = skb_next)) {
		struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
		skb_next = skb_next(skb);
		if (!cb->expires)
			continue;
		if (cb->expires > jiffies) {
			if (expires > cb->expires)
				expires = cb->expires;
			continue;
		}
		cb->flags |= (SCTPCB_FLAG_DROPPED | SCTPCB_FLAG_NACK);
		cb->when = jiffies;
		cb->next = NULL;
		skb_unlink(skb);
		if (cb->flags & SCTPCB_FLAG_CONF &&
		    cb->flags & SCTPCB_FLAG_LAST_FRAG)
			__skb_queue_tail(&sp->ackq, skb);
		else
			kfree_skb(skb);
	}
	skb_next = skb_peek(&sp->urgq);
	while ((skb = skb_next)) {
		struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
		skb_next = skb_next(skb);
		if (!cb->expires)
			continue;
		if (cb->expires > jiffies) {
			if (expires > cb->expires)
				expires = cb->expires;
			continue;
		}
		cb->flags |= (SCTPCB_FLAG_DROPPED | SCTPCB_FLAG_NACK);
		cb->when = jiffies;
		cb->next = NULL;
		skb_unlink(skb);
		if (cb->flags & SCTPCB_FLAG_CONF &&
		    cb->flags & SCTPCB_FLAG_LAST_FRAG)
			__skb_queue_tail(&sp->ackq, skb);
		else
			kfree_skb(skb);
	}
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
	/* need to walk through retransmit queue as well */
	if (!(sp->p_caps & SCTP_CAPS_PR))
		goto done;
	for (skb = skb_peek(&sp->rtxq); skb; skb = skb_next(skb)) {
		struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
		if ((cb->flags & SCTPCB_FLAG_DROPPED))
			goto push;
		if (!cb->expires)
			continue;
		if (cb->expires > jiffies) {
			if (expires > cb->expires)
				expires = cb->expires;
			continue;
		}
		/* PR-SCTP 3.5 A2 */
		if (cb->flags & SCTPCB_FLAG_RETRANS) {
			cb->flags &= ~SCTPCB_FLAG_RETRANS;
			ensure(sp->nrtxs > 0, sp->nrtxs = 1);
			sp->nrtxs--;
			sk->wmem_queued -= PADC(skb->len);
			ensure(sk->wmem_queued >= 0, sk->wmem_queued = 0);
		} else {
			struct sctp_daddr *sd = cb->daddr;
			size_t dlen = cb->dlen;
			if (sd) {
				/* credit destination (now) */
				sd->in_flight =
				    (sd->in_flight > dlen) ? sd->in_flight - dlen : 0;
				if (!sd->in_flight) {
					del_timer(&sd->timer_retrans);
#ifdef CONFIG_SCTP_ADD_IP
					if (sd->flags & SCTP_DESTF_UNUSABLE)
						sctp_del_daddr(sd);
#endif
				}
			}
			/* credit association (now) */
			normal(sp->in_flight >= dlen);
			sp->in_flight =
			    (sp->in_flight > dlen) ? sp->in_flight - dlen : 0;
		}
		cb->flags |= SCTPCB_FLAG_DROPPED;
	      push:
		/* push the forward ack point */
		if (cb->tsn == sp->l_fsn + 1) {
			sp->l_fsn++;
		}
	}
	if (expires != -1UL)
		sctp_mod_timer(&sp->timer_life, expires - jiffies);
	if (after(sp->l_fsn, sp->t_ack)) {
		sctp_send_forward_tsn(sk);
		if (!timer_pending(&sp->timer_sack))
			sctp_mod_timer(&sp->timer_sack, sp->max_sack);
		sctp_transmit_wakeup(sk);
	} else {
		sp->sackf &= ~SCTP_SACKF_FSN;
		if (!(sp->sackf & SCTP_SACKF_ANY))
			del_timer(&sp->timer_sack);
	}
#endif
      done:
	bh_unlock_sock(sk);
	return;
      locked:
	ptrace(("WARNING: hit locks\n"));
	sctp_mod_timer(&sp->timer_life, SCTP_TIMER_BACKOFF);
	goto done;
      outstate:
	ptrace(("WARNING: timeout in incorrect state\n"));
	goto done;
}
#endif

/*
 *  SEND DATA
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  THis function has been replaced by the sockets sendmsg function.
 */

/*
 *  SEND SACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE void sctp_send_sack(struct sock *sk)
{
	SCTP_PROT(sk)->sackf |= SCTP_SACKF_NOD;
}

/*
 *  SEND ERROR
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  We just queue the error, we don't send it out...  It gets bundled with other
 *  things.  This also provides for SCTP IG 2.27.
 */
STATIC int sctp_send_error(struct sock *sk, uint ecode, caddr_t eptr, size_t elen)
{
	struct sk_buff *skb;
	struct sctp_error *m;
	struct sctpehdr *eh;
	size_t clen = sizeof(*m) + sizeof(*eh) + elen;
	if (!(skb = sctp_alloc_chk(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_ERROR;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	eh = (typeof(eh)) skb_put(skb, sizeof(*eh));
	eh->code = htons(ecode);
	eh->len = htons(sizeof(*eh) + elen);
	memcpy(skb_put(skb, PADC(elen)), eptr, elen);
	__skb_queue_tail(&SCTP_PROT(sk)->errq, skb);
	return (0);
      enobufs:
	ptrace(("ERROR: couldn't allocate buffer\n"));
	return (-ENOBUFS);
}

/*
 *  SEND INIT
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  If we fail to launch the INIT and get timers started, we must return an
 *  error to the user interface calling this function.
 */
STATIC int sctp_send_init(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_daddr *sd = sp->daddr;
	struct sk_buff *skb;
	struct sctp_saddr *ss;
	struct sctp_init *m;
	struct sctp_addr_type *at;
	struct sctp_ipv4_addr *ap;
	struct sctp_cookie_psrv *cp;
#ifdef CONFIG_SCTP_ECN
	struct sctp_ecn_capable *ec;
#endif
#if defined(CONFIG_SCTP_ADD_IP) || defined(CONFIG_SCTP_ADAPTATION_LAYER_INFO)
	struct sctp_ali *ai;
#endif
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
	struct sctp_pr_sctp *pr;
#endif
	size_t sanum = sp->sanum;
	size_t clen = sizeof(*m)
	    + (sanum * PADC(sizeof(*ap)))
	    + (sp->ck_inc ? PADC(sizeof(*cp)) : 0)
	    + (sizeof(*at) + sizeof(at->type[0]))
#ifdef CONFIG_SCTP_ECN
	    + ((sp->l_caps & sp->p_caps & SCTP_CAPS_ECN) ? PADC(sizeof(*ec)) : 0)
#endif
#if defined(CONFIG_SCTP_ADD_IP) || defined(CONFIG_SCTP_ADAPTATION_LAYER_INFO)
	    + ((sp->l_caps & SCTP_CAPS_ALI) ? PADC(sizeof(*ai)) : 0)
#endif
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
	    + ((sp->l_caps & SCTP_CAPS_PR) ? PADC(sizeof(*pr)) : 0)
#endif
	;
	if (!sd)
		goto efault;
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_INIT;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	m->i_tag = sp->v_tag;
	m->a_rwnd = htonl(sk->rcvbuf);
	m->n_istr = htons(sp->max_istr);
	m->n_ostr = htons(sp->req_ostr);
	m->i_tsn = htonl(sp->v_tag);
	for (ss = sp->saddr; ss && sanum; ss = ss->next, sanum--) {
		ap = (typeof(ap)) skb_put(skb, PADC(sizeof(*ap)));
		ap->ph.type = SCTP_PTYPE_IPV4_ADDR;
		ap->ph.len = htons(sizeof(*ap));
		ap->addr = ss->saddr;
	}
	unusual(ss);
	unusual(sanum);
	if (sp->ck_inc) {
		cp = (typeof(cp)) skb_put(skb, PADC(sizeof(*cp)));
		cp->ph.type = SCTP_PTYPE_COOKIE_PSRV;
		cp->ph.len = __constant_htons(sizeof(*cp));
		cp->ck_inc = htonl(sp->ck_inc);
	}
	at = (typeof(at)) skb_put(skb, PADC(sizeof(*at) + sizeof(at->type[0])));
	at->ph.type = SCTP_PTYPE_ADDR_TYPE;
	at->ph.len = htons(sizeof(*at) + sizeof(at->type[0]));
	at->type[0] = SCTP_PTYPE_IPV4_ADDR;
#ifdef CONFIG_SCTP_ECN
	if (sp->l_caps & sp->p_caps & SCTP_CAPS_ECN) {
		ec = (typeof(ec)) skb_put(skb, PADC(sizeof(*ec)));
		ec->ph.type = SCTP_PTYPE_ECN_CAPABLE;
		ec->ph.len = __constant_htons(sizeof(*ec));
	}
#endif
#if defined(CONFIG_SCTP_ADD_IP) || defined(CONFIG_SCTP_ADAPTATION_LAYER_INFO)
	if (sp->l_caps & SCTP_CAPS_ALI) {
		ai = (typeof(ai)) skb_put(skb, PADC(sizeof(*ai)));
		ai->ph.type = SCTP_PTYPE_ALI;
		ai->ph.len = __constant_htons(sizeof(*ai));
		ai->ali = htonl(sp->l_ali);
	}
#endif
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
	if (sp->l_caps & SCTP_CAPS_PR) {
		pr = (typeof(pr)) skb_put(skb, PADC(sizeof(*pr)));
		pr->ph.type = SCTP_PTYPE_PR_SCTP;
		pr->ph.len = __constant_htons(sizeof(*pr));
	}
#endif
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS
	SCTP_INC_STATS(SctpOutCtrlChunks);
#endif
	sctp_mod_timer(&sp->timer_init, sd->rto);
	abnormal(sp->retry);
	sctp_change_state(sk, SCTP_COOKIE_WAIT);
	sctp_send_msg(sk, sd, skb);
	freechunks(xchg(&sp->retry, skb));
	return (0);
      enobufs:
	ptrace(("ERROR: no buffers\n"));
	return (-ENOBUFS);
      efault:
	swerr();
	return (-EFAULT);
}

/*
 *  SEND INIT ACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  No state change results from replying to an INIT.  INIT ACKs are sent
 *  without a TCB but a socket is referenced.  INIT ACK chunks cannot have any
 *  other chunks bundled with them. (RFC 2960 6.10).
 */
STATIC void
sctp_send_init_ack(struct sock *sk, uint32_t saddr, uint32_t daddr,
		   struct sctphdr *sh, struct sctp_cookie *ck, struct sk_buff *unrec)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	struct sctp_saddr *ss;
	struct sctphdr *rh;
	struct sctp_init_ack *m;
	struct sctpphdr *ph;
#ifdef CONFIG_SCTP_ECN
	struct sctp_ecn_capable *ec;
#endif
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
	struct sctp_pr_sctp *pr;
#endif
#if defined(CONFIG_SCTP_ADD_IP) || defined(CONFIG_SCTP_ADAPTATION_LAYER_INFO)
	struct sctp_ali *ai;
#endif
	struct sctp_unrec_parms *up;
	struct sctp_cookie *cp;
	struct sctp_ipv4_addr *ap;
	struct sctp_init *im = (struct sctp_init *) (sh + 1);
	unsigned char *init = (unsigned char *) im;
	int anum = ck->danum;
	int snum = ck->sanum;
	size_t klen = sizeof(*ph) + raw_cookie_size(ck) + HMAC_SIZE;
	size_t dlen = sp->sanum * PADC(sizeof(*ap));
	size_t ulen = (ulen = unrec->len) ? PADC(sizeof(*up) + ulen) : 0;
	size_t clen = sizeof(*m) + dlen + klen + ulen
#ifdef CONFIG_SCTP_ECN
	    + PADC(sizeof(*ec))
#endif
#if defined(CONFIG_SCTP_ADD_IP) || defined(CONFIG_SCTP_ADAPTATION_LAYER_INFO)
	    + ((sp->l_caps & SCTP_CAPS_ALI) ? PADC(sizeof(*ai)) : 0)
#endif
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
	    + ((sp->l_caps & SCTP_CAPS_PR) ? PADC(sizeof(*pr)) : 0)
#endif
	;
	int arem, alen;
	if (!(skb = sctp_alloc_reply(sh, clen)))
		return;
	rh = (typeof(rh)) skb->data;
	rh->v_tag = im->i_tag;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_INIT_ACK;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	m->i_tag = ck->v_tag;
	m->a_rwnd = htonl(sk->rcvbuf);
	m->n_istr = htons(ck->n_istr);
	m->n_ostr = htons(ck->n_ostr);
	m->i_tsn = htonl(ck->v_tag);
	for (ss = sp->saddr; ss; ss = ss->next) {
		ap = (typeof(ap)) skb_put(skb, PADC(sizeof(*ap)));
		ap->ph.type = SCTP_PTYPE_IPV4_ADDR;
		ap->ph.len = __constant_htons(sizeof(*ap));
		ap->addr = ss->saddr;
	}
	ph = (typeof(ph)) skb_put(skb, sizeof(*ph));
	ph->type = SCTP_PTYPE_STATE_COOKIE;
	ph->len = htons(klen);
	cp = (typeof(cp)) skb_put(skb, sizeof(*cp));
	memcpy(cp, ck, sizeof(*cp));
#if 0
	/* copy in IP reply options */
	if (ck->opt_len) {
		assure(opt);
		memcpy(skb->b_wptr, opt, optlength(opt));
		kfree_s(opt, optlength(opt));
		sp->opt = (struct ip_options *) skb->b_wptr;
		skb->b_wptr += ck->opt_len;
	}
#endif
	for (ap = (typeof(ap)) (init + sizeof(struct sctp_init)),
	     arem = PADC(htons(((struct sctpchdr *) init)->len)) -
	     sizeof(struct sctp_init); anum && arem >= sizeof(struct sctpphdr);
	     arem -= PADC(alen), ap = (typeof(ap)) (((uint8_t *) ap) + PADC(alen))) {
		if ((alen = ntohs(ap->ph.len)) > arem) {
			assure(alen <= arem);
			kfree_skb(skb);
			rare();
			return;
		}
		if (ap->ph.type == SCTP_PTYPE_IPV4_ADDR) {
			/* skip primary */
			if (ap->addr != ck->daddr) {
				*((uint32_t *) skb_put(skb, sizeof(uint32_t))) =
				    ap->addr;
				anum--;
			}
		}
	}
	for (ss = sp->saddr; ss; ss = ss->next) {
		if (ss->saddr != ck->saddr) {
			*((uint32_t *) skb_put(skb, sizeof(uint32_t))) = ss->saddr;
			snum--;
		}
	}
	assure(!anum);
	assure(!snum);
	sctp_sign_cookie(sk, cp);
	skb_put(skb, HMAC_SIZE);
#ifdef CONFIG_SCTP_ECN
	ec = (typeof(ec)) skb_put(skb, PADC(sizeof(*ec)));
	ec->ph.type = SCTP_PTYPE_ECN_CAPABLE;
	ec->ph.len = __constant_htons(sizeof(*ec));
#endif
#if defined(CONFIG_SCTP_ADD_IP) || defined(CONFIG_SCTP_ADAPTATION_LAYER_INFO)
	if (sp->l_caps & SCTP_CAPS_ALI) {
		ai = (typeof(ai)) skb_put(skb, PADC(sizeof(*ai)));
		ai->ph.type = SCTP_PTYPE_ALI;
		ai->ph.len = __constant_htons(sizeof(*ai));
		ai->ali = htonl(sp->l_ali);
	}
#endif
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
	if (sp->l_caps & SCTP_CAPS_PR) {
		pr = (typeof(pr)) skb_put(skb, PADC(sizeof(*pr)));
		pr->ph.type = SCTP_PTYPE_PR_SCTP;
		pr->ph.len = __constant_htons(sizeof(*pr));
	}
#endif
	/* SCTP IG 2.27 add unrecognized parameters parameter */
	if (ulen) {
		up = (typeof(up)) skb_put(skb, PADC(ulen));
		up->ph.type = SCTP_PTYPE_UNREC_PARMS;
		up->ph.len = __constant_htons(ulen);
		memcpy((up + 1), unrec->data, unrec->len);
	}
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS
	SCTP_INC_STATS(SctpOutCtrlChunks);
#endif
	sctp_xmit_msg(saddr, daddr, skb, sk);
	return;
}

/*
 *  SEND COOKIE ECHO
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  If we fail to launch the COOKIE ECHO and get timers started, we must return
 *  an error to the user interface calling this function.
 */
STATIC int sctp_send_cookie_echo(struct sock *sk, caddr_t kptr, size_t klen)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	struct sctp_daddr *sd;
	struct sctp_cookie_echo *m;
	size_t clen = sizeof(*m) + klen;
	if (!(sd = sctp_route_normal(sk)))
		goto efault;
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_COOKIE_ECHO;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	memcpy(skb_put(skb, PADC(klen)), kptr, klen);
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS
	SCTP_INC_STATS(SctpOutCtrlChunks);
#endif
	sctp_change_state(sk, SCTP_COOKIE_ECHOED);
	sctp_mod_timer(&sp->timer_cookie, sd->rto);
	sctp_bundle_more(sk, sd, skb, 1);	/* don't nagle */
	sctp_send_msg(sk, sd, skb);
	abnormal(sp->retry);
	freechunks(xchg(&sp->retry, skb));
	return (0);
      enobufs:
	rare();
	return (-ENOBUFS);
      efault:
	rare();
	return (-EFAULT);
}

/*
 *  SEND COOKIE ACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  SACK and DATA can be bundled with the COOKIE ACK.
 */
STATIC int sctp_return_more(struct sk_buff *skb);
STATIC int sctp_recv_msg(struct sock *sk, struct sk_buff *skb);
STATIC void sctp_send_cookie_ack(struct sock *sk, struct sk_buff *cp)
{
	struct sctp_daddr *sd;
	struct sk_buff *skb;
	struct sctp_cookie_ack *m;
	size_t clen = sizeof(*m);
	if (!(sd = sctp_route_response(sk)))
		goto noroute;
	sctp_change_state(sk, SCTP_ESTABLISHED);
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_COOKIE_ACK;
	m->ch.flags = 0;
	m->ch.len = __constant_htons(clen);
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS
	SCTP_INC_STATS(SctpOutCtrlChunks);
#endif
	/* process data bundled with cookie echo on new socket */
	if (sctp_return_more(cp) > 0) {
		sctp_recv_msg(sk, skb_get(cp));
		if (sk->state != SCTP_ESTABLISHED)
			goto done;
	}
	sctp_bundle_more(sk, sd, skb, 1);	/* don't nagle */
	sctp_send_msg(sk, sd, skb);
      done:
	freechunks(skb);
      enobufs:
	/* start idle timers */
	if (sk->state == SCTP_ESTABLISHED)
		for (sd = SCTP_PROT(sk)->daddr; sd; sd = sd->next)
			sctp_reset_idle(sd);
      noroute:
	return;
}

/*
 *  SEND HEARTBEAT
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  We don't send heartbeats when idle timers expire if we are in the wrong
 *  state, we just reset the idle timer.
 */
STATIC void sctp_send_heartbeat(struct sock *sk, struct sctp_daddr *sd)
{
	struct sk_buff *skb;
	struct sctp_heartbeat *m;
	struct sctp_heartbeat_info *h;
	size_t fill, clen, hlen;
	ensure(sd, return);
	fill = sd->hb_fill;
	hlen = sizeof(*h) + fill;
	clen = sizeof(*m) + hlen;
	sd->hb_time = jiffies;
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_HEARTBEAT;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	h = (typeof(h)) skb_put(skb, sizeof(*h));
	h->ph.type = SCTP_PTYPE_HEARTBEAT_INFO;
	h->ph.len = htons(hlen);
	h->hb_info.timestamp = sd->hb_time;
	h->hb_info.daddr = sd->daddr;
	h->hb_info.mtu = sd->mtu;
	memset(skb_put(skb, PADC(fill)), 0, fill);
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS
	SCTP_INC_STATS(SctpOutCtrlChunks);
#endif
	sctp_send_msg(sk, sd, skb);
	freechunks(skb);
      enobufs:
	sctp_mod_timer(&sd->timer_heartbeat, sd->rto);
}

/*
 *  SEND HEARTBEAT ACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Take the incoming HEARTBEAT message and turn it back around as a HEARTBEAT
 *  ACK message.  Note that if the incoming chunk parameters are invalid, so are
 *  the outgoing parameters, this is because the hb_info parameter is opaque to
 *  us.  This is consistent with draft-stewart-ong-sctpbakeoff-sigtran-01.
 */
STATIC void sctp_send_heartbeat_ack(struct sock *sk, caddr_t hptr, size_t hlen)
{
	struct sctp_daddr *sd;
	struct sk_buff *skb;
	struct sctp_heartbeat_ack *m;
	size_t clen = sizeof(*m) + hlen;
	if (!(sd = sctp_route_response(sk)))
		goto noroute;
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_HEARTBEAT_ACK;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	memcpy(skb_put(skb, PADC(hlen)), hptr, hlen);
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS
	SCTP_INC_STATS(SctpOutCtrlChunks);
#endif
	sctp_send_msg(sk, sd, skb);
	freechunks(skb);
	return;
      noroute:
      enobufs:
	return;
}

/*
 *  SEND ABORT
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  There is no point in bundling control chunks after an ABORT chunk.  Also,
 *  DATA chunks are not to be bundled with ABORT chunks.
 */
STATIC void sctp_send_abort(struct sock *sk)
{
	struct sctp_daddr *sd;
	struct sk_buff *skb;
	struct sctp_abort *m;
	size_t clen = sizeof(*m);
	if ((1 << sk->state) & ~(SCTPF_NEEDABORT))
		goto notneeded;
	if ((1 << sk->state) & SCTPF_CONNECTED)
		sd = sctp_route_normal(sk);
	else
		sd = SCTP_PROT(sk)->daddr;
	if (!sd)
		goto noroute;
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_ABORT;
	m->ch.flags = 0;
	m->ch.len = __constant_htons(clen);
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS
	SCTP_INC_STATS(SctpOutCtrlChunks);
#endif
	sctp_send_msg(sk, sd, skb);
	freechunks(skb);
      enobufs:
      noroute:
	if (sk->state != SCTP_LISTEN) {
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPABORTEDS
		if (sk->state != SCTP_CLOSED)
			SCTP_INC_STATS(SctpAborteds);
#endif
		sctp_change_state(sk, SCTP_CLOSED);
	}
      notneeded:
	return;
}

/*
 *  SEND ABORT (w/ERROR CAUSE)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Errors (beyond the error header) must be formatted by the called and
 *  indicated by are and len.  There is no point in bundling data or control
 *  chunks after and abort chunk.
 */
STATIC void sctp_send_abort_error(struct sock *sk, int errn, void *aptr, size_t alen)
{
	struct sctp_daddr *sd;
	struct sk_buff *skb;
	struct sctp_abort *m;
	struct sctpehdr *eh;
	size_t elen = sizeof(*eh) + alen;
	size_t clen = sizeof(*m) + elen;
	if ((1 << sk->state) & ~(SCTPF_NEEDABORT))
		goto notneeded;
	if (!(sd = sctp_route_normal(sk)))
		goto noroute;
	if (!errn)
		goto noerror;
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_ABORT;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	eh = (typeof(eh)) skb_put(skb, sizeof(*eh));
	eh->code = htons(errn);
	eh->len = htons(elen);
	memcpy(skb_put(skb, PADC(alen)), aptr, alen);
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS
	SCTP_INC_STATS(SctpOutCtrlChunks);
#endif
	sctp_send_msg(sk, sd, skb);
	freechunks(skb);
      enobufs:
      noroute:
	if (sk->state != SCTP_LISTEN) {
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPABORTEDS
		if (sk->state != SCTP_CLOSED)
			SCTP_INC_STATS(SctpAborteds);
#endif
		sctp_change_state(sk, SCTP_CLOSED);
	}
	return;
      noerror:
	sctp_send_abort(sk);
      notneeded:
	return;
}

/*
 *  SEND SHUTDOWN
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  RFC 2960 6.2 "... DATA chunks cannot be bundled with SHUTDOWN or SHUTDOWN
 *  ACK chunks ..."
 *
 *  If we fail to launch the SHUTDOWN and get timers started, we must inform the
 *  user interface calling this function.
 */
STATIC int sctp_send_shutdown(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	struct sctp_daddr *sd;
	struct sctp_shutdown *m;
	size_t clen = sizeof(*m);
	if (!skb_queue_empty(&sk->sndq) || !skb_queue_empty(&sp->urgq)
	    || !skb_queue_empty(&sp->rtxq))
		swerr();
	if (!(sd = sctp_route_normal(sk)))
		goto noroute;
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_SHUTDOWN;
	m->ch.flags = 0;
	m->ch.len = __constant_htons(clen);
	m->c_tsn = htonl(sp->r_ack);
	/* shutdown acks everything but dups and gaps */
	sp->sackf &= (SCTP_SACKF_DUP | SCTP_SACKF_GAP);
	if (timer_pending(&sp->timer_sack))
		del_timer(&sp->timer_sack);
#ifdef CONFIG_SCTP_ADD_IP
	if (timer_pending(&sp->timer_asconf))
		del_timer(&sp->timer_asconf);
#endif
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS
	SCTP_INC_STATS(SctpOutCtrlChunks);
#endif
	sctp_mod_timer(&sp->timer_shutdown, sd->rto);
	/* SCTP IG 2.12 says 5 * RTO.Max but we do 5 * RTO */
	if (sk->state != SCTP_SHUTDOWN_SENT)
		sctp_mod_timer(&sp->timer_guard, 5 * sd->rto);
	sctp_change_state(sk, SCTP_SHUTDOWN_SENT);
	sctp_bundle_more(sk, sd, skb, 1);	/* not DATA */
	sctp_send_msg(sk, sd, skb);
	freechunks(xchg(&sp->retry, skb));
	return (0);
      enobufs:
	rare();
	return (-ENOBUFS);
      noroute:
	rare();
	return (-EFAULT);
}

/*
 *  SEND SHUTDOWN ACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  SHUTDOWN ACK is sent in response to a SHUTDOWN message after all data has
 *  cleared or in reponse to a COOKIE ECHO during the SHUTDOWN_ACK_SENT state.
 *  If the error flag is set, we want to bundle and ERROR chunk with the
 *  SHUTDOWN ACK indicating "cookie received while shutting down."
 *
 *  RFC 2960 6.2. "...  DATA chunks cannot be bundled with SHUTDOWN or SHUTDOWN
 *  ACK chunks ..."
 *
 *  If we fail to launch the SHUTDOWN ACK and get timers started, we must return
 *  an error to the user interface calling this function.
 */
STATIC int sctp_send_shutdown_ack(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	struct sctp_daddr *sd;
	struct sctp_shutdown_ack *m;
	size_t clen = sizeof(*m);
	if (!skb_queue_empty(&sk->sndq) || !skb_queue_empty(&sp->urgq)
	    || !skb_queue_empty(&sp->rtxq))
		swerr();
	if (!(sd = sctp_route_response(sk)))
		goto noroute;
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_SHUTDOWN_ACK;
	m->ch.flags = 0;
	m->ch.len = __constant_htons(clen);
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS
	SCTP_INC_STATS(SctpOutCtrlChunks);
#endif
	if (timer_pending(&sp->timer_sack))
		del_timer(&sp->timer_sack);
#ifdef CONFIG_SCTP_ADD_IP
	if (timer_pending(&sp->timer_asconf))
		del_timer(&sp->timer_asconf);
#endif
	sctp_mod_timer(&sp->timer_shutdown, sd->rto);
	/* SCTP IG 2.12 says 5 * RTO.Max but we do 5 * RTO */
	if (sk->state != SCTP_SHUTDOWN_ACK_SENT)
		sctp_mod_timer(&sp->timer_guard, 5 * sd->rto);
	sctp_change_state(sk, SCTP_SHUTDOWN_ACK_SENT);
	sctp_bundle_more(sk, sd, skb, 1);	/* not DATA */
	sctp_send_msg(sk, sd, skb);
	abnormal(sp->retry);
	freechunks(xchg(&sp->retry, skb));
	return (0);
      enobufs:
	rare();
	return (-ENOBUFS);
      noroute:
	rare();
	return (-EFAULT);
}

/*
 *  SEND SHUTDOWN COMPLETE
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void sctp_send_shutdown_complete(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	struct sctp_daddr *sd;
	struct sctp_shutdown_comp *m;
	size_t clen = sizeof(*m);
	if (!skb_queue_empty(&sk->sndq) || !skb_queue_empty(&sp->urgq)
	    || !skb_queue_empty(&sp->rtxq))
		swerr();
	if (!(sd = sctp_route_response(sk)))
		goto noroute;
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_SHUTDOWN_COMPLETE;
	m->ch.flags = 0;
	m->ch.len = __constant_htons(clen);
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS
	SCTP_INC_STATS(SctpOutCtrlChunks);
#endif
	sctp_send_msg(sk, sd, skb);
	freechunks(skb);
	return;
      enobufs:
      noroute:
	if (sk->state != SCTP_LISTEN)
		sctp_change_state(sk, SCTP_CLOSED);
	return;
}

#ifdef CONFIG_SCTP_ECN
/*
 *  SEND ECNE (Explicit Congestion Notification Echo)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Just mark an ECNE chunk to be sent with the next SACK (and don't delay
 *  SACKs).
 */
STATIC INLINE void sctp_send_ecne(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	if (sp->l_caps & sp->p_caps & SCTP_CAPS_ECN)
		sp->sackf |= SCTP_SACKF_ECN;
}

/*
 *  SEND CWR (Congestion Window Reduction)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Just mark a CWR chunk to be bundled with the next DATA.
 */
STATIC INLINE void sctp_send_cwr(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	if (sp->l_caps & sp->p_caps & SCTP_CAPS_ECN)
		sp->sackf |= SCTP_SACKF_CWR;
}
#endif
#ifdef CONFIG_SCTP_ADD_IP

/*
 *  ABORT ASCONF
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void sctp_abort_asconf(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_saddr *ss;
	for (ss = sp->saddr; ss; ss = ss->next) {
		if (ss->flags & SCTP_SRCEF_ADD_PENDING) {
			ss->flags &= ~SCTP_SRCEF_ADD_PENDING;
			ss->flags |= SCTP_SRCEF_ADD_REQUEST;
			sp->sackf |= SCTP_SACKF_ASC;
		}
		if (ss->flags & SCTP_SRCEF_DEL_PENDING) {
			ss->flags &= ~SCTP_SRCEF_DEL_PENDING;
			ss->flags |= SCTP_SRCEF_DEL_REQUEST;
			sp->sackf |= SCTP_SACKF_ASC;
		}
		if (ss->flags & SCTP_SRCEF_SET_PENDING) {
			ss->flags &= ~SCTP_SRCEF_SET_PENDING;
			ss->flags |= SCTP_SRCEF_SET_REQUEST;
			sp->sackf |= SCTP_SACKF_ASC;
		}
	}
	if (sk->state == SCTP_ESTABLISHED) {
		del_timer(&sp->timer_asconf);
		freechunks(xchg(&sp->retry, NULL));
	}
	return;
}

/*
 *  SEND ASCONF
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void sctp_send_asconf(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	struct sctp_daddr *sd;
	struct sctp_asconf *m;
	struct sctp_add_ip *a;
	struct sctp_del_ip *d;
	struct sctp_set_ip *s;
	struct sctp_ipv4_addr *p;
	size_t clen = sizeof(*m);
	struct sctp_saddr *ss;
	int requested = 0;
	unsigned char *ptr;
	if (sk->state != SCTP_ESTABLISHED)
		goto skip;
	if (!(sp->p_caps & (SCTP_CAPS_ADD_IP | SCTP_CAPS_SET_IP)))
		goto skip;
	/* ADD-IP 4.1 (A3) */
	if (sp->reply || timer_pending(&sp->timer_asconf))
		goto skip;
	ensure(sp->sackf & SCTP_SACKF_ASC, goto skip);
	if (!(sd = sctp_route_normal(sk)))
		goto noroute;
	for (ss = sp->saddr; ss; ss = ss->next) {
		if (sp->p_caps & SCTP_CAPS_ADD_IP) {
			if (ss->flags & SCTP_SRCEF_ADD_REQUEST) {
				clen += sizeof(*a) + sizeof(*p);
				requested++;
			}
			if (ss->flags & SCTP_SRCEF_DEL_REQUEST) {
				clen += sizeof(*d) + sizeof(*p);
				requested++;
			}
		}
		if (sp->p_caps & SCTP_CAPS_SET_IP) {
			if (ss->flags & SCTP_SRCEF_SET_REQUEST) {
				clen += sizeof(*s) + sizeof(*p);
				requested++;
			}
		}
	}
	if (!requested)
		goto skip;
	/* ADD-IP 4.1 (A1) */
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_ASCONF;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	/* ADD-IP 4.1 (A2) */
	m->asn = htonl(++sp->l_asn);
	ptr = (unsigned char *) (m + 1);
	for (ss = sp->saddr; ss; ss = ss->next) {
		if (sp->p_caps & SCTP_CAPS_ADD_IP) {
			if (ss->flags & SCTP_SRCEF_ADD_REQUEST) {
				a = ((typeof(a)) ptr)++;
				a->ph.type = SCTP_PTYPE_ADD_IP;
				a->ph.len =
				    __constant_htons(sizeof(*a) + sizeof(*p));
				a->id = htonl((uint32_t) ss);
				p = ((typeof(p)) ptr)++;
				p->ph.type = SCTP_PTYPE_IPV4_ADDR;
				p->ph.len = __constant_htons(sizeof(*p));
				p->addr = htonl(ss->saddr);
				ss->flags &= ~SCTP_SRCEF_ADD_REQUEST;
				ss->flags |= SCTP_SRCEF_ADD_PENDING;
			}
			if (ss->flags & SCTP_SRCEF_DEL_REQUEST) {
				d = ((typeof(d)) ptr)++;
				d->ph.type = SCTP_PTYPE_DEL_IP;
				d->ph.len =
				    __constant_htons(sizeof(*d) + sizeof(*p));
				d->id = htonl((uint32_t) ss);
				p = ((typeof(p)) ptr)++;
				p->ph.type = SCTP_PTYPE_IPV4_ADDR;
				p->ph.len = __constant_htons(sizeof(*p));
				p->addr = htonl(ss->saddr);
				ss->flags &= ~SCTP_SRCEF_DEL_REQUEST;
				ss->flags |= SCTP_SRCEF_DEL_PENDING;
			}
		}
		if (sp->p_caps & SCTP_CAPS_SET_IP) {
			if (ss->flags & SCTP_SRCEF_SET_REQUEST) {
				s = ((typeof(s)) ptr)++;
				s->ph.type = SCTP_PTYPE_SET_IP;
				s->ph.len =
				    __constant_htons(sizeof(*s) + sizeof(*p));
				s->id = htonl((uint32_t) ss);
				p = ((typeof(p)) ptr)++;
				p->ph.type = SCTP_PTYPE_IPV4_ADDR;
				p->ph.len = __constant_htons(sizeof(*p));
				p->addr = htonl(ss->saddr);
				ss->flags &= ~SCTP_SRCEF_SET_REQUEST;
				ss->flags |= SCTP_SRCEF_SET_PENDING;
			}
		}
	}
	sp->sackf &= ~SCTP_SACKF_ASC;
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS
	SCTP_INC_STATS(SctpOutCtrlChunks);
#endif
	sctp_bundle_more(sk, sd, skb, 1);
	/* ADD-IP 4.1 (A4) */
	sctp_mod_timer(&sp->timer_asconf, sd->rto);
	sctp_send_msg(sk, sd, skb);
	freechunks(xchg(&sp->retry, skb));
      skip:
	return;
      enobufs:
	rare();
	return;
      noroute:
	rare();
	return;
}

/*
 *  SEND ASCONF ACK
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void sctp_send_asconf_ack(struct sock *sk, caddr_t rptr, size_t rlen)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	struct sctp_daddr *sd;
	struct sctp_asconf_ack *m;
	size_t clen = sizeof(*m) + rlen;
	if (!(sd = sctp_route_response(sk)))
		goto noroute;
	if (!(skb = sctp_alloc_msg(sk, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_ASCONF_ACK;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	m->asn = ntohl(sp->p_asn++);
	memcpy(m + 1, rptr, rlen);	/* copy in response TLVs */
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS
	SCTP_INC_STATS(SctpOutCtrlChunks);
#endif
	sctp_bundle_more(sk, sd, skb, 1);
	sctp_send_msg(sk, sd, skb);
	freechunks(xchg(&sp->reply, skb));
	return;
      enobufs:
	rare();
	return;
      noroute:
	rare();
	return;
}
#endif
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
/*
 *  SEND FORWARD TSN
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC INLINE void sctp_send_forward_tsn(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_daddr *sd;
	/* PR-SCTP 3.5 (F2) */
	sp->sackf |= SCTP_SACKF_FSN;
	for (sd = sp->daddr; sd; sd = sd->next) {
		if (sd->flags & SCTP_DESTF_FORWDTSN) {
			if (!sd->in_flight && timer_pending(&sd->timer_retrans))
				del_timer(&sd->timer_retrans);
			sd->flags &= ~SCTP_DESTF_FORWDTSN;
		}
	}
}
#endif

/*
 *  SENDING WITHOUT TCB  (Responding to OOTB packets)
 *  -------------------------------------------------------------------------
 *  When sending without an SCTP TCB, we only have the IP header and the SCTP
 *  header from which to work.  We have no associated socket.  These are
 *  usually used for replying to OOTB messages.
 *
 *  SEND ABORT (Without TCB)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
void sctp_send_abort_ootb(uint32_t daddr, uint32_t saddr, struct sctphdr *sh)
{
	struct sk_buff *skb;
	struct sctp_abort *m;
	size_t clen = sizeof(*m);
	if (!sh)
		goto noroute;
	if (!(skb = sctp_alloc_reply(sh, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_ABORT;
	m->ch.flags = 1;
	m->ch.len = __constant_htons(clen);
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS
	SCTP_INC_STATS(SctpOutCtrlChunks);
#endif
	sctp_xmit_ootb(daddr, saddr, skb);
	return;
      noroute:
      enobufs:
	return;
}

/*
 *  SEND ABORT (w/ERROR CAUSE) (Without TCB)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void
sctp_send_abort_error_ootb(uint32_t daddr, uint32_t saddr,
			   struct sctphdr *sh, int errn, caddr_t aptr, size_t alen)
{
	struct sk_buff *skb;
	struct sctp_abort *m;
	struct sctpehdr *eh;
	size_t elen = sizeof(*eh) + alen;
	size_t clen = sizeof(*m) + elen;
	if (!sh)
		goto noroute;
	if (!errn)
		goto noerror;
	if (!(skb = sctp_alloc_reply(sh, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_ABORT;
	m->ch.flags = 1;
	m->ch.len = htons(clen);
	eh = (typeof(eh)) skb_put(skb, sizeof(*eh));
	eh->code = htons(errn);
	eh->len = htons(elen);
	memcpy(skb_put(skb, PADC(alen)), aptr, alen);
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS
	SCTP_INC_STATS(SctpOutCtrlChunks);
#endif
	sctp_xmit_ootb(daddr, saddr, skb);
	return;
      noerror:
	sctp_send_abort_ootb(daddr, saddr, sh);
	return;
      noroute:
      enobufs:
	return;
}

/*
 *  SENDING With TCB  (Responding to INIT packets)
 *  -------------------------------------------------------------------------
 *  When sending without an SCTP TCB, we only have the IP header and the SCTP
 *  header from which to work.  We only have a listening socket.  These are
 *  usually used for replying to INIT messages.
 *
 *  SEND ABORT (With TCB) Responding to INIT
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
void sctp_send_abort_init(struct sock *sk, uint32_t daddr, uint32_t saddr,
			  struct sctphdr *sh, uint32_t i_tag)
{
	struct sk_buff *skb;
	struct sctp_abort *m;
	size_t clen = sizeof(*m);
	if (!sh)
		goto noroute;
	if (!(skb = sctp_alloc_reply(sh, clen)))
		goto enobufs;
	(((struct sctphdr *) skb->data) - 1)->v_tag = i_tag;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_ABORT;
	m->ch.flags = 0;
	m->ch.len = __constant_htons(clen);
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS
	SCTP_INC_STATS(SctpOutCtrlChunks);
#endif
	sctp_xmit_msg(saddr, daddr, skb, sk);
	return;
      noroute:
      enobufs:
	return;
}

/*
 *  SEND ABORT (w/ERROR CAUSE) (With TCB) Responding to INIT
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void sctp_send_abort_error_init(struct sock *sk, uint32_t daddr,
				       uint32_t saddr, struct sctphdr *sh,
				       uint32_t i_tag, int errn, caddr_t aptr,
				       size_t alen)
{
	struct sk_buff *skb;
	struct sctp_abort *m;
	struct sctpehdr *eh;
	size_t elen = sizeof(*eh) + alen;
	size_t clen = sizeof(*m) + elen;
	if (!sh)
		goto noroute;
	if (!errn)
		goto noerror;
	if (!(skb = sctp_alloc_reply(sh, clen)))
		goto enobufs;
	(((struct sctphdr *) skb->data) - 1)->v_tag = i_tag;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_ABORT;
	m->ch.flags = 0;
	m->ch.len = htons(clen);
	eh = (typeof(eh)) skb_put(skb, sizeof(*eh));
	eh->code = htons(errn);
	eh->len = htons(elen);
	memcpy(skb_put(skb, PADC(alen)), aptr, alen);
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS
	SCTP_INC_STATS(SctpOutCtrlChunks);
#endif
	sctp_xmit_msg(saddr, daddr, skb, sk);
	return;
      noerror:
	sctp_send_abort_init(sk, daddr, saddr, sh, i_tag);
	return;
      noroute:
      enobufs:
	return;
}

/*
 *  SEND SHUTDOWN COMPLETE (Without TCB)
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void sctp_send_shutdown_complete_ootb(uint32_t daddr, uint32_t saddr,
					     struct sctphdr *sh)
{
	struct sk_buff *skb;
	struct sctp_shutdown_comp *m;
	size_t clen = sizeof(*m);
	if (!sh)
		goto noroute;
	if (!(skb = sctp_alloc_reply(sh, clen)))
		goto enobufs;
	m = (typeof(m)) skb_put(skb, sizeof(*m));
	m->ch.type = SCTP_CTYPE_SHUTDOWN_COMPLETE;
	m->ch.flags = 1;
	m->ch.len = __constant_htons(clen);
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTCTRLCHUNKS
	SCTP_INC_STATS(SctpOutCtrlChunks);
#endif
	sctp_xmit_ootb(daddr, saddr, skb);
	return;
      enobufs:
      noroute:
	return;
}

/*
 *  =========================================================================
 *
 *  SCTP Peer --> SCTP Primitives (Receive Messages)
 *
 *  =========================================================================
 */
/*
 *  RETURN VALUE FUNCTIONS
 *  -------------------------------------------------------------------------
 *
 *  RETURN VALUE when expecting more chunks
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Pull the previous chunk and range check next chunk header.
 */
STATIC int sctp_return_more(struct sk_buff *skb)
{
	size_t dlen = skb->len;
	struct sctpchdr *ch = (typeof(ch)) skb->data;
	size_t clen = ntohs(ch->len);
	size_t plen = PADC(clen);
	if ((ch = (typeof(ch)) skb_pull(skb, plen))
	    && ((dlen = skb->len) >= sizeof(*ch))
	    && ((clen = ntohs(ch->len)) >= sizeof(*ch))
	    && ((plen = PADC(clen)) <= dlen))
		return (clen);
	if (skb->len)
		return (-EMSGSIZE);
	return (0);
}

/*
 *  RETURN VALUE when not expecting more chunks
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC int sctp_return_stop(struct sk_buff *skb)
{
	return (sctp_return_more(skb) ? -EPROTO : 0);
}

/* is s2<=s1<=s3 ? */
#define between(__s1,__s2,__s3)((uint32_t)(__s3)-(uint32_t)(__s2)>=(uint32_t)(__s1)-(uint32_t)(__s2))
#define before(__s1,__s2) (((int32_t)((uint32_t)(__s1)-(uint32_t)(__s2))<0))
#define after(__s1,__s2) (((int32_t)((uint32_t)(__s2)-(uint32_t)(__s1))<0))

#define between_s(__s1,__s2,__s3)((uint16_t)(__s3)-(uint16_t)(__s2)>=(uint16_t)(__s1)-(uint16_t)(__s2))
#define before_s(__s1,__s2) (((int16_t)((uint16_t)(__s1)-(uint16_t)(__s2))<0))
#define after_s(__s1,__s2) (((int16_t)((uint16_t)(__s2)-(uint16_t)(__s1))<0))

/*
 *  RESET IDLE
 *  -------------------------------------------------------------------------
 *  Reset the idle timer for generation of heartbeats.  Stop any heartbeating
 *  that we might be involved in at the moment.
 */
STATIC void sctp_reset_idle(struct sctp_daddr *sd)
{
	unsigned long rtt;
	ensure(sd, return);
	del_timer(&sd->timer_heartbeat);
	rtt = sd->rto + sd->hb_itvl + ((jiffies & 0x1) * (sd->rto >> 1));
	sctp_mod_timer(&sd->timer_idle, rtt);
}

/*
 *  RTT CALC
 *  -------------------------------------------------------------------------
 *  Round Trip Time calculations for messages acknowledged on the first
 *  transmission.  When a message is acknowledged, this function peforms and
 *  update of the RTT calculation if appropriate.  It is called by sctp_ack_calc
 *  for control chunks which expect acknowledgements, and by sctp_dest_calc when
 *  DATA chunks are acknolwedged on first transmission via SACK or SHUTDOWN
 *  chunks, and by sctp_recv_heartbeat_ack when calculating RTTs for HEARTBEAT
 *  chunks.
 */
STATIC void sctp_rtt_calc(struct sctp_daddr *sd, unsigned long time)
{
	unsigned long rtt;
	ensure(sd, return);
	ensure(jiffies >= time, return);
	/* RFC 2960 6.3.1 */
	rtt = jiffies - time;
	if (sd->srtt) {
		unsigned long rttvar;
		/* RFC 2960 6.3.1 (C3) */
		if (rtt > sd->srtt) {
			rttvar = rtt - sd->srtt;
			sd->srtt += (rtt - sd->srtt) >> 3;
		} else {
			rttvar = sd->srtt - rtt;
			sd->srtt -= (sd->srtt - rtt) >> 3;
		}
		if (rttvar > sd->rttvar)
			sd->rttvar += (rttvar - sd->rttvar) >> 2;
		else
			sd->rttvar -= (sd->rttvar - rttvar) >> 2;
		sd->rto = sd->srtt + (sd->rttvar << 2);
	} else {
		/* RFC 2960 6.3.1 (C2) */
		sd->rttvar = rtt >> 1;
		sd->srtt = rtt;
		sd->rto = rtt + (rtt << 1);
	}
	/* RFC 2960 6.3.1 (G1) */
	sd->rttvar = sd->rttvar ? sd->rttvar : 1;
	/* RFC 2960 6.3.1 (C6) */
	sd->rto = (sd->rto_min > sd->rto) ? sd->rto_min : sd->rto;
	/* RFC 2960 6.3.1 (C7) */
	sd->rto = (sd->rto_max < sd->rto) ? sd->rto_max : sd->rto;
#if defined(CONFIG_SCTP_DEBUG) && defined(CONFIG_SCTP_ERROR_GENERATOR)
	if (sd->retransmits && (sd->sp->options & SCTP_OPTION_BREAK)
	    && (sd->packets > BREAK_GENERATOR_LEVEL))
		ptrace(("Aaaarg! Reseting counts for address %d.%d.%d.%d\n",
			(sd->daddr >> 0) & 0xff, (sd->daddr >> 8) & 0xff,
			(sd->daddr >> 16) & 0xff, (sd->daddr >> 24) & 0xff));
#endif
	sd->dups = 0;
	/* RFC 2960 8.2 */
	sd->retransmits = 0;
	/* RFC 2960 8.1 */
	sd->sp->retransmits = 0;
	/* reset idle timer */
	sctp_reset_idle(sd);
}

/*
 *  DEST CALC
 *  -------------------------------------------------------------------------
 *  This function performs delayed processing of RTT and CWND calculations on
 *  destinations which need it and is called from sctp_recv_shutdown and
 *  sctp_recv_sack once all ack and gap ack processing is complete.  This
 *  performs the necessary calculations for each destination before closing
 *  processing of the received SHUTDOWN or SACK chunk.
 */
STATIC void sctp_dest_calc(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_daddr *sd;
	for (sd = sp->daddr; sd; sd = sd->next) {
		size_t accum;
		if (sd->when) {
			/* calculate RTT based on latest sent acked TSN */
			sctp_rtt_calc(sd, sd->when);
			sd->when = 0;
		}
		/* 
		 *  NOTE:- first we grow the congestion window according to
		 *  whatever TSNs were cummulatively acked to the destination
		 *  and then we back off if the destination is dropping (as
		 *  indiciated by gap reports).
		 */
		if ((accum = sd->ack_accum)) {
			if (sd->cwnd <= sd->ssthresh) {
				/* RFC 2960 7.2.1 */
				if (sd->in_flight > sd->cwnd)
					sd->cwnd +=
					    (accum < sd->mtu) ? accum : sd->mtu;
			} else {
				/* RFC 2960 7.2.2 */
				sd->partial_ack += accum;
				if (sd->in_flight >= sd->cwnd &&
				    sd->partial_ack >= sd->cwnd) {
					sd->cwnd += sd->mtu;
					sd->partial_ack -= sd->cwnd;
				}
			}
			/* credit of destination (accum) */
			normal(sd->in_flight >= accum);
			sd->in_flight =
			    (sd->in_flight > accum) ? sd->in_flight - accum : 0;
			/* RFC 2960 6.3.2 (R3) */
			del_timer(&sd->timer_retrans);
			sd->ack_accum = 0;
		}
		if (sd->flags & SCTP_DESTF_DROPPING) {
			/* RFC 2960 7.2.4 (2), 7.2.3 */
			sd->ssthresh =
			    ((sd->cwnd >> 1) > (sd->mtu << 1)) ? sd->cwnd >> 1 : sd->mtu << 1;
			sd->cwnd = sd->ssthresh;
			/* SCTP IG Section 2.9 */
			sd->partial_ack = 0;
			sd->flags &= ~SCTP_DESTF_DROPPING;
		}
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
		sd->flags &= ~SCTP_DESTF_FORWDTSN;
#endif
		/* RFC 2960 6.3.2 (R2) */
		if (!sd->in_flight) {
			del_timer(&sd->timer_retrans);
#ifdef CONFIG_SCTP_ADD_IP
			if (sd->flags & SCTP_DESTF_UNUSABLE)
				sctp_del_daddr(sd);
#endif
		} else if (!timer_pending(&sd->timer_retrans))
			sctp_mod_timer(&sd->timer_retrans, sd->rto);
	}
}

/*
 *  CUMM ACK
 *  -------------------------------------------------------------------------
 *  This function is responsible for moving the cummulative ack point.  The
 *  sender must check that the ack is valid (monotonically increasing), but it
 *  may be the same TSN as was previously acknowledged.  When the ack point
 *  advances, this function stikes DATA chunks from the retransmit buffer.
 *  This also indirectly updates the amount of data outstanding for
 *  retransmission.  This function is called by both sctp_recv_sack and
 *  sctp_recv_shutdown.
 *
 *  We only perform calculations on TSNs that were not previously acknowledged
 *  by a GAP Ack.  If the TSN has not been retransmitted (Karn's algorithm RFC
 *  2960 6.3.1 (C5)), the destination that it was set to is marked for RTT
 *  calculation update.  If the TSN is currently marked for retransmission
 *  awaiting available cwnd, it is unmarked and retranmission of the TSN is
 *  aborted.  If the TSN is not marked for retransmission, the destination's
 *  acked bytes accumulator is increased and the need for a CWND calculation
 *  for the destination indicated.  The association number of bytes in flight
 *  is decreased to account for the acknowledged TSN.  If any calculations are
 *  pending as a result of the ack, the function returns non-zero to
 *  indication that calculations (RTT and CWND) must be processed before
 *  message processing is complete.
 */
STATIC void sctp_cumm_ack(struct sock *sk, uint32_t ack)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	/* PR-SCTP 3.5 (A3) */
	sp->t_ack = ack;
	while ((skb = skb_peek(&sp->rtxq)) && !after(SCTP_SKB_CB(skb)->tsn, ack)) {
		struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
		if (!(cb->flags & SCTPCB_FLAG_SACKED)) {
			struct sctp_daddr *sd = cb->daddr;
			/* RFC 2960 6.3.1 (C5) */
			if (sd && cb->trans < 2) {
				/* remember latest transmitted packet acked for rtt
				   calc */
				sd->when = (sd->when > cb->when) ? sd->when : cb->when;
			}
			if (cb->flags & SCTPCB_FLAG_RETRANS) {
				cb->flags &= ~SCTPCB_FLAG_RETRANS;
				ensure(sp->nrtxs > 0, sp->nrtxs = 1);
				sp->nrtxs--;
				sk->wmem_queued -= PADC(skb->len);
				ensure(sk->wmem_queued >= 0, sk->wmem_queued = 0);
			} else if (!(cb->flags & SCTPCB_FLAG_DROPPED)) {
				size_t dlen = cb->dlen;
				if (sd) {
					/* credit destination (later) */
					normal(sd->in_flight >=
					       sd->ack_accum + dlen);
					sd->ack_accum =
					    sd->in_flight > sd->ack_accum + dlen ?
					    sd->ack_accum + dlen : sd->in_flight;
				}
				/* credit association (now) */
				normal(sp->in_flight >= dlen);
				sp->in_flight =
				    (sp->in_flight > dlen) ? sp->in_flight - dlen : 0;
			}
		} else {
			/* can't reneg cummulatively acked chunks */
			cb->flags &= ~SCTPCB_FLAG_SACKED;
			ensure(sp->nsack, sp->nsack = 1);
			sp->nsack--;
		}
		if (cb->flags & SCTPCB_FLAG_CONF &&
		    cb->flags & SCTPCB_FLAG_LAST_FRAG)
			__skb_queue_tail(&sp->ackq, __skb_dequeue(&sp->rtxq));
		else
			kfree_skb(__skb_dequeue(&sp->rtxq));
		/* 
		 *  NOTE: In STREAMS we need to back-enable the write queue
		 *  here, but in Linux socket code this is accomplised by the
		 *  skb destructor with the kfree_skb above, or when the ackq
		 *  is purged.
		 */
	}
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
	/* PR-SCTP 3.5 (A3 - C1) */
	if (before(sp->l_fsn, sp->t_ack))
		sp->l_fsn = sp->t_ack;
	/* PR-SCTP 3.5 (A3 - C2) */
	for (skb = skb_peek(&sp->rtxq); skb; skb = skb_next(skb)) {
		struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
		if (!after(cb->tsn, sp->l_fsn))
			continue;
		if (!(cb->flags & SCTPCB_FLAG_DROPPED))
			break;
		/* push the forward ack point */
		if (cb->tsn == sp->l_fsn + 1)
			sp->l_fsn++;
	}
	if (after(sp->l_fsn, sp->t_ack)) {
		sctp_send_forward_tsn(sk);
		if (!timer_pending(&sp->timer_sack))
			sctp_mod_timer(&sp->timer_sack, sp->max_sack);
	} else {
		sp->sackf &= ~SCTP_SACKF_FSN;
		if (!(sp->sackf & SCTP_SACKF_ANY))
			del_timer(&sp->timer_sack);
	}
#endif
	return;
}

/*
 *  ACK CALC
 *  -------------------------------------------------------------------------
 *  This one is for messages for which a timer is set an retransmission occurs
 *  until acknowledged.  We stop the timer, perform an RTT calculation if the
 *  message was not retransmitted, free the retry buffer, and clear the
 *  association and destination retransmission counts.
 */
STATIC void sctp_ack_calc(struct sock *sk, struct timer_list *tp)
{				/* timer to cancel */
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_skb_cb *cb = SCTP_SKB_CB(sp->retry);
	assert(sp->retry);
	del_timer(tp);
	if (cb->daddr && cb->trans < 2)
		sctp_rtt_calc(cb->daddr, cb->when);
	freechunks(xchg(&sp->retry, NULL));
}

/*
 *  DELIVER DATA
 *  -------------------------------------------------------------------------
 *  There is reason to believe that there is data waiting in gaps that can
 *  be delivered.  This function delivers whatever data is possible and pushes
 *  the necessary ack points.
 */
STATIC void sctp_deliver_data(struct sock *sk, struct sk_buff *skb)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_skb_cb *cb, *cb_next = sp->gaps;
	while ((cb = cb_next)) {
		cb_next = cb->next;
		if (!(cb->flags & SCTPCB_FLAG_DELIV)) {
			struct sk_buff *skp;
			struct sctp_strm *st = cb->st;
			uint flags = cb->flags;
			int ord = !(flags & SCTPCB_FLAG_URG);
			int more = !(flags & SCTPCB_FLAG_LAST_FRAG);
			int frag = !(flags & SCTPCB_FLAG_FIRST_FRAG);
			/* test for advanced delivery around gaps */
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
			if (after(cb->tsn, sp->p_fsn + 1)) {
#else
			if (after(cb->tsn, sp->r_ack + 1)) {
#endif
				/* after gap */
				if (frag)
					continue;
				if (ord) {
					if (st->n.more & SCTP_STRMF_MORE)
						continue;
					if (cb->ssn != ((st->ssn + 1) & 0xffff))
						continue;
				} else {
					if (st->x.more & SCTP_STRMF_MORE)
						continue;
					if (more)
						continue;
				}
			}
			if ((skp = skb_clone(cb->skb, GFP_ATOMIC))) {
				sctp_get_owner_r(skp, skb);	/* Why do I do
								   this??? */
				sp->rmem_queued += cb->dlen;
				cb->flags |= SCTPCB_FLAG_DELIV;
				if (ord) {
					__skb_queue_tail(&sk->rcvq, skp);
					sctp_data_ind(sk, cb->dlen);
					if (!more) {
						st->n.more &= ~SCTP_STRMF_MORE;
						st->ssn = cb->ssn;
					} else
						st->n.more |= SCTP_STRMF_MORE;
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPINORDERCHUNKS
					SCTP_INC_STATS(SctpInOrderChunks);
#endif
				} else {
					__skb_queue_tail(&sp->expq, skp);
					sctp_exdata_ind(sk, cb->dlen);
					if (!more)
						st->x.more &= ~SCTP_STRMF_MORE;
					else
						st->x.more |= SCTP_STRMF_MORE;
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPINUNORDERCHUNKS
					SCTP_INC_STATS(SctpInUnorderChunks);
#endif
				}
				sp->nunds--;
			} else {
				rare();
				break;
			}	/* no buffers */
		}
		if (cb->tsn == sp->r_ack + 1) {
			sp->r_ack++;
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
			if (before(sp->p_fsn, sp->r_ack))
				sp->p_fsn = sp->r_ack;
#endif
			sctp_send_sack(sk);
		}
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
		else if (after(cb->tsn, sp->r_ack)
			 && !after(cb->tsn, sp->p_fsn)) {
			/* FIXME: There is more to do here: we have to somehow
			   inform further receive functions in sctp_recv_data() that 
			   partial data chunks on the stream which were skipped
			   should be discarded. Also, we need to somehow inform
			   waiters in sctp_recvmsg() to wake up and set MSG_TRUNC
			   when waiting on subsequent data chunks for the same
			   packet.  We need to store tat in the receive stream
			   datastructure and then pass it into the message control
			   block.  For now, intermediate messages will simply
			   disappear (even if they are in the middle of fragmented
			   messages). */
			sp->r_ack = cb->tsn;
			sctp_send_sack(sk);
		}
#endif
		if ((cb->flags & SCTPCB_FLAG_DELIV)
		    && !after(cb->tsn, sp->r_ack)) {
			assure(sp->gaps == cb);
			if (!(sp->gaps = cb->next))
				sp->ngaps = 0;
			else {
				if (cb == cb->tail)
					sp->ngaps--;
				else
					cb->next->tail = cb->tail;
			}
			skb_unlink(cb->skb);
			kfree_skb(cb->skb);
		}
	}
}

/*
 *  RECV DATA
 *  -------------------------------------------------------------------------
 *  We have received a DATA chunk in a T-Provider state where is it valid to
 *  receive DATA (TS_DATA_XFER and TS_WIND_ORDREL).  We can also receive data
 *  chunks in TS_IDLE or TS_WRES_CIND on a listening socket bundled with a
 *  COOKIE ECHO.  In any other states we discard the data.  Because no other
 *  chunks can be bundled after a DATA chunk (just yet), we process all the DATA
 *  chunks in the remainder of the message in a single loop here.
 */
STATIC int sctp_recv_data(struct sock *sk, struct sk_buff *skb)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	int err;
	size_t plen;
#ifdef CONFIG_SCTP_ECN
	int is_ce = INET_ECN_is_ce(skb->nh.iph->tos);
#endif
	int newd, data;			/* number of new data chunks */
	struct sctp_skb_cb *cb;
	struct sctp_data *m;
	if ((1 << sk->state) & ~(SCTPF_RECEIVING)
	    || sk->dead || sk->shutdown & RCV_SHUTDOWN)
		goto outstate;
	for (newd = 0, data = 0, plen = 0, err = 0; err == 0;) {
		struct sk_buff *skd;
		struct sctp_strm *st;
		struct sctp_skb_cb **gap;
		uint32_t tsn, ppi;
		uint16_t sid, ssn;
		size_t clen, blen, dlen;
		uint flags;
		int ord, more;
		if ((!(m = (typeof(m)) skb_pull(skb, plen)))
		    || ((blen = skb->len) < sizeof(struct sctpchdr))
		    || ((clen = ntohs(m->ch.len)) < sizeof(*m))
		    || ((plen = PADC(clen)) > blen)) {
			if (skb->len)
				goto emsgsize;
			break;
		}
		flags = (m->ch.flags);
		ord = !(flags & SCTPCB_FLAG_URG);
		more = !(flags & SCTPCB_FLAG_LAST_FRAG) ? SCTP_STRMF_MORE : 0;
		tsn = ntohl(m->tsn);
		sid = ntohs(m->sid);
		ssn = ntohs(m->ssn);
		ppi = ntohl(m->ppi);
		dlen = clen - sizeof(*m);
		if (m->ch.type != SCTP_CTYPE_DATA)
			goto eproto;
		if (dlen == 0)
			goto baddata;
		if (sid >= sp->n_istr)
			goto badstream;
		sctp_put_owner_r(skb);
		if (atomic_read(&sk->rmem_alloc) >= sk->rcvbuf)
			goto flowcontrol;
		if (!(st = sctp_istrm_find(sk, sid, &err)))
			goto enomem;
		if (!(skd = skb_clone(skb, GFP_ATOMIC)))
			goto enobufs;
		data++;
		/* pull copy to data only */
		skb_pull(skd, sizeof(*m));
		/* trim to data only */
		skb_trim(skd, dlen);
		cb = SCTP_SKB_CB(skd);
		memset(cb, 0, sizeof(*cb));
		cb->dlen = dlen;
		cb->when = jiffies;
		cb->tsn = tsn;
		cb->sid = sid;
		cb->ssn = ssn;
		cb->ppi = ppi;
		cb->flags = flags & 0x7;
		cb->daddr = sp->caddr;
		cb->skb = skd;
		cb->st = st;
		cb->tail = cb;
		usual(sp->caddr);
		sctp_set_owner_r(skd, sk);
		/* fast path, next expected, nothing out of order */
		if (tsn == sp->r_ack + 1 && skb_queue_empty(&sp->oooq)) {
			/* we have next expected TSN, just process it */
			sp->rmem_queued += dlen;
			cb->flags |= SCTPCB_FLAG_DELIV;
			if (ord) {
				__skb_queue_tail(&sk->rcvq, skd);
				sctp_data_ind(sk, dlen);
				if (!more) {
					st->n.more &= ~SCTP_STRMF_MORE;
					st->ssn = cb->ssn;
				} else
					st->n.more |= SCTP_STRMF_MORE;
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPINORDERCHUNKS
				SCTP_INC_STATS(SctpInOrderChunks);
#endif
			} else {
				__skb_queue_tail(&sp->expq, skd);
				sctp_exdata_ind(sk, dlen);
				if (!more)
					st->x.more &= ~SCTP_STRMF_MORE;
				else
					st->x.more |= SCTP_STRMF_MORE;
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPINUNORDERCHUNKS
				SCTP_INC_STATS(SctpInUnorderChunks);
#endif
			}
			sp->r_ack++;
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
			if (before(sp->p_fsn, sp->r_ack))
				sp->p_fsn = sp->r_ack;
#endif
#ifdef CONFIG_SCTP_ECN
			if (sp->l_caps & sp->p_caps & SCTP_CAPS_ECN) {
				if (!newd && is_ce && !(sp->sackf & SCTP_SACKF_ECN)) {
					sp->l_lsn = tsn;
					sctp_send_ecne(sk);
				}
			}
#endif
			newd++;
			continue;
		}
		if (!after(tsn, sp->r_ack))
			goto sctp_recv_data_duplicate;
		for (gap = &sp->gaps; (*gap); gap = &((*gap)->tail->next)) {
			if (between(tsn, (*gap)->tsn, (*gap)->tail->tsn))
				goto sctp_recv_data_duplicate;
			if (before(tsn, (*gap)->tsn)) {
				/* insert in front of gap */
				ptrace(("INFO: Expecting tsn = %u, inserting %u\n",
					sp->r_ack, tsn));
				__skb_insert(skd, (*gap)->skb->prev, (*gap)->skb,
					     (*gap)->skb->list);
				cb->next = (*gap);
				(*gap) = cb;
				sp->ngaps++;
			} else if (tsn == (*gap)->tail->tsn + 1) {
				/* expand at end of gap */
				ptrace(("INFO: Expecting tsn = %u, expanding %u\n",
					sp->r_ack, tsn));
				__skb_queue_tail(&sp->oooq, skd);
				cb->next = (*gap)->tail->next;
				(*gap)->tail->next = cb;
				(*gap)->tail = cb;
			} else {
				/* try next gap */
				continue;
			}
			if (cb->next && cb->next->tsn == tsn + 1) {
				/* join two gaps */
				ptrace(("INFO: Joining gaps\n"));
				(*gap)->tail = cb->next->tail;
				usual(sp->ngaps);
				sp->ngaps--;
			}
			break;
		}
		if (!(*gap)) {
			/* append to list */
			printd(("INFO: Expecting tsn = %u, appending %u\n", sp->r_ack, tsn));
			__skb_queue_tail(&sp->oooq, skd);
			cb->next = (*gap);
			(*gap) = cb;
			sp->ngaps++;
		}
		sp->nunds++;	/* more undelivered data */
#ifdef CONFIG_SCTP_ECN
		if (sp->l_caps & sp->p_caps & SCTP_CAPS_ECN) {
			if (!newd && is_ce && !(sp->sackf & SCTP_SACKF_ECN)) {
				sp->l_lsn = tsn;
				sctp_send_ecne(sk);
			}
		}
#endif
		newd++;
		continue;
	      sctp_recv_data_duplicate:
		/* message is a duplicate tsn */
		ptrace(("INFO: Expecting tsn = %u, duplicate %u\n", sp->r_ack, tsn));
		__skb_queue_tail(&sp->dupq, skd);
		cb->next = sp->dups;
		sp->dups = cb;
		sp->ndups++;
		continue;
		/* We should not break with ENOBUFS or ENOMEM or EBUSY here... I'm
		   not sure that we have left the buffer in a state where it can be
		   put back on the queue and processed later. */
	      enobufs:
		err = -ENOBUFS;
		ptrace(("ERROR: couldn't allocate buffer\n"));
		break;
	      enomem:
		err = -ENOMEM;
		ptrace(("ERROR: couldn't allocate stream\n"));
		break;
	      flowcontrol:
		/* Protect from teardrop attack without renegging */
		if (skb_queue_empty(&sk->rcvq)) {
			/* 
			 *  TODO: we need to put renegging code in here:
			 *  renegging would first walk the gap queue looking
			 *  for any data that we have gap acked which has not
			 *  yet been delivered to the user.  Any such data can
			 *  be deleted.  Once deleting this data we can check
			 *  if we can process the current message anyway.  If
			 *  that is true, we can mark that a sack is required
			 *  immediately and jump back up to the top where we
			 *  entered flow control.  If we are still flow
			 *  controlled we will need to abort the association.
			 *  For now we just abort the association.
			 */
			sctp_send_abort(sk);
			sctp_discon_ind(sk, ECONNRESET);
			return (-ECONNRESET);
		}
		/* 
		 *  If we have data that the user has not read yet, then we
		 *  keep all our gaps, because the user reading data will make
		 *  some room to process things later.  We must send an
		 *  immediate SACK regardless, per the IG requirements.
		 */
		/* SCTP Implementor's Guide Section 2.15.1 */
		sctp_send_sack(sk);
		ptrace(("ERROR: flow controlled (discard)\n"));
		err = 0;
		break;
	      badstream:
		ptrace(("PROTO: invalid stream id\n"));
		sctp_send_error(sk, SCTP_CAUSE_INVALID_STR, (caddr_t) &m->sid, sizeof(m->sid));
		continue;	/* just skip that DATA chunk */
	      baddata:
		ptrace(("PROTO: no user data in data chunk\n"));
		/* RFC 2960 6.2: ...no user data... */
		sctp_send_abort_error(sk, SCTP_CAUSE_NO_DATA, &m->tsn, sizeof(m->tsn));
		sctp_discon_ind(sk, ECONNRESET);
		return (-ECONNRESET);
	      eproto:
		err = -EPROTO;
		ptrace(("PROTO: non-data chunk after data\n"));
		break;
	      emsgsize:
		err = -EMSGSIZE;
		ptrace(("PROTO: bad message or chunk sizes\n"));
		break;
	}
	if (!data)
		goto no_data;
	if (!newd)
		goto no_new_data;
	/* we have underlivered data and new data */
	sctp_deliver_data(sk, skb);
      no_new_data:
	/* RFC 2960 6.2 */
	/* 
	 *  IMPLEMENTATION NOTE:- If we are receiving duplicates the
	 *  probability is high that our SACKs aren't getting through (or have
	 *  been delayed too long).  If we do not have a sack pending (one
	 *  being delayed) then we will peg the duplicates against the
	 *  destination.  This will change where we are sending SACKs.
	 *
	 *  Because we are being cruel to this destination and we don't really
	 *  know that this is where the offending SACKs were sent, we send an
	 *  immediate heartbeat if there is no data in flight to the
	 *  destination (i.e., no retransmission timer running for the
	 *  destination).  This fixes some sticky problems when one-way data
	 *  is being sent.
	 */
	if (sp->ndups) {
		struct sctp_daddr *sd = sp->taddr;
		sp->sackf |= SCTP_SACKF_DUP;
		if (sd && !(sp->sackf & SCTP_SACKF_NEW)) {
			sd->dups += sp->ndups;
			if (!sd->in_flight && !timer_pending(&sd->timer_heartbeat)) {
				del_timer(&sd->timer_idle);
				sctp_send_heartbeat(sk, sd);
			}
		}
	}
	/* RFC 2960 7.2.4 */
	if (sp->ngaps) {
		sp->sackf |= SCTP_SACKF_GAP;
	}
	/* RFC 2960 6.2 */
	/* 
	 *  IMPLEMENTATION NOTE:-  The SACK timer is probably too slow.  For
	 *  unidirectional operation, the sender may have timed out before we
	 *  send a sack.  We should really not wait any longer than some
	 *  fraction of the RTO for the destination from which we are
	 *  receiving (or sending) data.  However, if we wait too long we will
	 *  just get a retransmission and a dup.
	 *
	 *  If the sack delay is set to zero, we do not set the timer, but
	 *  issue the sack immediately.
	 */
	if (newd) {
		if (sp->max_sack) {
			sp->sackf += ((sp->sackf & 0x3) < 3) ? SCTP_SACKF_NEW : 0;
			if (!timer_pending(&sp->timer_sack))
				sctp_mod_timer(&sp->timer_sack, sp->max_sack);
		} else
			sp->sackf |= SCTP_SACKF_TIM;
	}
	if (sk->state == SCTP_SHUTDOWN_SENT)
		sctp_send_shutdown(sk);
      no_data:
	return (err);
      outstate:
//      /* flow controlled */
//      rare();
//      return(-EBUSY);
//      }
	/* We have received DATA in the wrong state.  If so, it is probably an old
	   packet that was stuck in the network and just got delivered to us.
	   Nevertheless we should just ignore any message containing DATA when we
	   are not expecting it. The only exception to this might be receiving DATA
	   in the COOKIE-WAIT state. There should not be data hanging around in the
	   network that matches our tags. If that is the case, we should abandon the 
	   connection attempt and let the user try again with a different
	   verification tag.  In the COOKIE-ECHOED state, if we receive data, we
	   might very well assume that we have received a COOKIE-ACK and process the 
	   data anyway.  */
	rare();
	if (sk->state == SCTP_COOKIE_WAIT) {
		sctp_discon_ind(sk, ECONNRESET);
		return (-ECONNRESET);
	}
	return (0);
}

/*
 *  RECV SACK
 *  -------------------------------------------------------------------------
 */
STATIC int sctp_recv_sack(struct sock *sk, struct sk_buff *skb)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sk_buff *skd;
	struct sctp_sack *m = (struct sctp_sack *) skb->data;
	size_t clen = ntohs(m->ch.len);
	size_t ngaps, ndups;
	uint32_t ack, rwnd;
	uint16_t *gaps;
	if ((1 << sk->state) & ~(SCTPF_SENDING))
		goto outstate;
	if (clen < sizeof(*m))
		goto emsgsize;
	ack = ntohl(m->c_tsn);
	rwnd = ntohl(m->a_rwnd);
	ngaps = ntohs(m->ngaps);
	ndups = ntohs(m->ndups);
	gaps = m->gaps;
	if (clen < sizeof(*m) + (ndups + ngaps) * sizeof(uint32_t))
		goto emsgsize;
	/* RFC 2960 6.2.1 (D) i) */
	if (before(ack, sp->t_ack))
		goto discard;
	/* RFC 1122 4.2.3.4 IMPLEMENTATION (3) */
	sp->p_rbuf = (sp->p_rbuf >= rwnd) ? sp->p_rbuf : rwnd;
	/* RFC 2960 6.2.1 (D) ii) */
	sp->p_rwnd = rwnd;	/* we keep in_flight separate from a_rwnd */
	if (after(ack, sp->t_ack)) {
		/* advance the cummulative ack point and check need to perform
		   per-round-trip and cwnd calcs */
		sctp_cumm_ack(sk, ack);
		/* if we have cummulatively acked something, we will skip FR
		 * analysis for this SACK. */
		goto skip_rtx_analysis;
	}
	if (ndups) {
		/* 
		 *  TODO: we could look through the list of duplicate TSNs.
		 *  Duplicate TSNs really means that the peer's SACKs aren't
		 *  getting back to us.  But there is nothing really that we can
		 *  do about that.  The peer has already detected the problem
		 *  and should sent SACKs via an alternative route if possible.
		 *  But that's how this SACK got here...
		 */
	}
	/* process gap acks */
	if (!ngaps) {
		/* If there are no gap acks in the received SACK, the SACK might
		   still constitute a reneg on previous SACKs.  If some chunks have
		   been previously gap acked, we check for the reneg, otherwise we
		   skip the loop.  sp->nsack here contains the number of renegable
		   gap acked data chunks. */
		if (!sp->nsack)
			goto skip_rtx_analysis;
		for (skd = skb_peek(&sp->rtxq); skd; skd = skb_next(skd)) {
			struct sctp_skb_cb *cb = SCTP_SKB_CB(skd);
			/* RFC 2960 6.3.2 (R4) (reneg) */
			if (cb->flags & SCTPCB_FLAG_SACKED) {
				struct sctp_daddr *sd = cb->daddr;
				cb->flags &= ~SCTPCB_FLAG_SACKED;
				ensure(sp->nsack, sp->nsack = 1);
				sp->nsack--;
				if (sd && !timer_pending(&sd->timer_retrans))
					sctp_mod_timer(&sd->timer_retrans, sd->rto);
			}
		}
	} else {
		int growth = 0;
		{
			struct sctp_daddr *sd;
			/* We skip gap analysis if we are running ultra-fast
			   destinations with data outstanding after cummulative ack, 
			   because the gap reports are probably out of date and it
			   will just consume processing power checking them when an
			   RTO is coming fast anyway */
			for (sd = sp->daddr; sd; sd = sd->next)
				if (sd->in_flight && sd->rto > 0)
					goto do_gap_analysis;
			goto skip_rtx_analysis;
		}
	      do_gap_analysis:
		/* perform fast retransmission algorithm on gaps */
		while (ngaps--) {
			uint32_t beg = ack + ntohs(*gaps++);
			uint32_t end = ack + ntohs(*gaps++);
			if (before(end, beg)) {
				rare();
				continue;
			}
			/* move to the acks */
			skd = skb_peek(&sp->rtxq);
			for (; skd && before(SCTP_SKB_CB(skd)->tsn, beg);
			     skd = skb_next(skd))
				SCTP_SKB_CB(skd)->flags |= SCTPCB_FLAG_NACK;
			/* sack the acks */
			for (; skd && !after(SCTP_SKB_CB(skd)->tsn, end);
			     skd = skb_next(skd))
				SCTP_SKB_CB(skd)->flags |= SCTPCB_FLAG_ACK;
		}
		/* walk the whole retrans buffer looking for holes and renegs */
		for (skd = skb_peek(&sp->rtxq); skd; skd = skb_next(skd)) {
			struct sctp_skb_cb *cb = SCTP_SKB_CB(skd);
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
			if ((cb->flags & SCTPCB_FLAG_DROPPED)) {
				cb->flags &= ~SCTPCB_FLAG_ACK;
				cb->flags &= ~SCTPCB_FLAG_NACK;
				continue;
			}
#endif
			/* msg is inside gapack block */
			if (cb->flags & SCTPCB_FLAG_ACK) {
				cb->flags &= ~SCTPCB_FLAG_ACK;
				if (cb->flags & SCTPCB_FLAG_NACK) {
					cb->flags &= ~SCTPCB_FLAG_NACK;
					if (cb->sacks < SCTP_FR_COUNT)
						growth = 1;
				}
				if (!(cb->flags & SCTPCB_FLAG_SACKED)) {
					struct sctp_daddr *sd = cb->daddr;
					cb->flags |= SCTPCB_FLAG_SACKED;
					sp->nsack++;
					/* RFC 2960 6.3.1 (C5) */
					if (sd && cb->trans < 2) {
						/* remember latest transmitted
						   packet acked for rtt calc */
						sd->when =
						    (sd->when > cb->when)
						    ? sd->when : cb->when;
					}
					if (cb->flags & SCTPCB_FLAG_RETRANS) {
						cb->flags &= ~SCTPCB_FLAG_RETRANS;
						ensure(sp->nrtxs > 0, sp->nrtxs = 1);
						sp->nrtxs--;
						sk->wmem_queued -= PADC(skd->len);
						ensure(sk->wmem_queued >= 0,
						       sk->wmem_queued = 0);
					} else {
						size_t dlen = cb->dlen;
						if (sd) {
							/* credit destination */
							normal(sd->in_flight >=
							       dlen);
							sd->in_flight =
							    sd->in_flight >
							    dlen ? sd->in_flight -
							    dlen : 0;
						}
						/* credit association */
						normal(sp->in_flight >= dlen);
						sp->in_flight =
						    (sp->in_flight > dlen)
						    ? sp->in_flight - dlen : 0;
					}
				}
				continue;
			}
			/* msg is between gapack blocks */
			if (cb->flags & SCTPCB_FLAG_NACK) {
				cb->flags &= ~SCTPCB_FLAG_NACK;
				/* RFC 2960 7.2.4 */
				if (!growth && !(cb->flags & SCTPCB_FLAG_RETRANS)
				    && ++(cb->sacks) == SCTP_FR_COUNT) {
					size_t dlen = cb->dlen;
					struct sctp_daddr *sd = cb->daddr;
					/* RFC 2960 7.2.4 (1) */
					cb->flags |= SCTPCB_FLAG_RETRANS;
					sp->nrtxs++;
					sk->wmem_queued += PADC(skd->len);
					if (sd) {
						/* RFC 2960 7.2.4 (2) */
						sd->flags |= SCTP_DESTF_DROPPING;
						/* credit destination (now) */
						normal(sd->in_flight >= dlen);
						sd->in_flight =
						    (sd->in_flight > dlen)
						    ? sd->in_flight - dlen : 0;
					}
					/* credit association (now) */
					normal(sp->in_flight >= dlen);
					sp->in_flight =
					    (sp->in_flight > dlen)
					    ? sp->in_flight - dlen : 0;
				}
				/* RFC 2960 6.3.2 (R4) (reneg) */
				if (cb->flags & SCTPCB_FLAG_SACKED) {
					struct sctp_daddr *sd = cb->daddr;
					cb->flags &= ~SCTPCB_FLAG_SACKED;
					ensure(sp->nsack, sp->nsack = 1);
					sp->nsack--;
					if (sd && !timer_pending(&sd->timer_retrans)) {
						sctp_mod_timer(&sd->timer_retrans,
							       sd->rto);
					}
				}
				continue;
			}
			/* msg is after all gapack blocks */
			break;
		}
	}
      skip_rtx_analysis:
	sctp_dest_calc(sk);
	/* After receiving a cummulative ack, I want to check if the sndq, urgq and
	   rtxq is empty (and a SHUTDOWN or SHUTDOWN-ACK is pending. If so, I want
	   to issue these primitives. */
	if (((1 << sk->state) & (SCTPF_SHUTDOWN_PENDING | SCTPF_SHUTDOWN_RECEIVED |
				 SCTPF_SHUTDOWN_RECVWAIT)) &&
	    atomic_read(&sk->wmem_alloc) <= 0) {
		seldom();
		switch (sk->state) {
		case SCTP_SHUTDOWN_PENDING:
			/* Send the SHUTDOWN I didn't send before. */
			sctp_send_shutdown(sk);
			break;
		case SCTP_SHUTDOWN_RECEIVED:
			/* Send the SHUTDOWN-ACK I didn't send before */
			if (sk->dead)
				sctp_send_shutdown_ack(sk);
			break;
		case SCTP_SHUTDOWN_RECVWAIT:
			/* Send the SHUTDOWN-ACK I didn't send before */
			sctp_send_shutdown_ack(sk);
			break;
		default:
			swerr();
			return (-EFAULT);
		}
	}
      outstate:
	/* 
	 *  We may have received a SACK in the wrong state.  Because SACKs are
	 *  completely advisory, there is no reason to get too upset about this.
	 *  Simply ignore them.  No need to process them.
	 */
	{
		int ret;
		if ((ret = sctp_return_more(skb)) > 0) {
			struct sctpchdr *ch = (struct sctpchdr *) skb->data;
			switch (ch->type) {
				/* RFC 2960 6 */
			case SCTP_CTYPE_DATA:
				/* RFC 2960 6.5 */
			case SCTP_CTYPE_ERROR:
				/* RFC 2960 3.3.7 */
			case SCTP_CTYPE_ABORT:
				break;
			default:
				rare();
				return (-EPROTO);
			}
		}
		return (ret);
	}
      discard:
	return (0);
      emsgsize:
	return (-EMSGSIZE);
}

/*
 *  RECV ERROR
 *  -------------------------------------------------------------------------
 *  We have received an ERROR chunk in opening, connected or closing states.
 */
STATIC int sctp_recv_error(struct sock *sk, struct sk_buff *skb)
{
	int err;
	struct sctp_error *m = (struct sctp_error *) skb->data;
	struct sctpehdr *eh = (struct sctpehdr *) (m + 1);
	size_t clen = ntohs(m->ch.len);
	int ecode;
	size_t elen;
	if (clen < sizeof(*m) + sizeof(*eh))
		goto emsgsize;
	elen = ntohs(eh->len);
	if (elen < sizeof(*eh) || sizeof(*m) + elen > clen)
		goto emsgsize;
	ecode = ntohs(eh->code);
	switch (ecode) {
	case SCTP_CAUSE_STALE_COOKIE:
	{
		struct sctp_opt *sp = SCTP_PROT(sk);
		struct sctp_skb_cb *cb;
		struct sctp_daddr *sd;
		if (sk->state != SCTP_COOKIE_ECHOED)
			break;
		ensure(sp->retry, goto recv_error_error);
		cb = SCTP_SKB_CB(sp->retry);
		sd = cb->daddr;
		ensure(sd, goto recv_error_error);
		/* We can try again with cookie preservative, and then we can keep
		   trying until we have tried as many times as we can... */
		if (!sp->ck_inc) {
			sp->ck_inc = sp->ck_inc + (sd->rto >> 1);
			sctp_ack_calc(sk, &sp->timer_init);
			if ((err = sctp_send_init(sk)))
				return (err);
			return sctp_return_stop(skb);
		}
		/* RFC 2960 5.2.6 (1) */
		if (cb->trans < sp->max_inits) {
			del_timer(&sp->timer_init);
			/* RFC 2960 5.2.6 (3) */
			if (cb->trans < 2)
				sctp_rtt_calc(sd, cb->when);
			sctp_send_msg(sk, sd, sp->retry);
			return sctp_return_stop(skb);
		}
		/* RFC 2960 5.2.6 (2) */
		goto recv_error_error;
	}
	case SCTP_CAUSE_INVALID_PARM:
	case SCTP_CAUSE_BAD_ADDRESS:
		/* If the sender of the ERROR has already given us a valid INIT-ACK
		   then we can ignore these errors. */
		if (sk->state == SCTP_COOKIE_ECHOED)
			break;
		seldom();
	case SCTP_CAUSE_MISSING_PARM:
	case SCTP_CAUSE_NO_RESOURCE:
	case SCTP_CAUSE_INVALID_STR:
		/* These errors are bad.  If we don't get an abort with them then we 
		   must abort the association. */
	      recv_error_error:
		sctp_send_abort(sk);
		sctp_discon_ind(sk, ECONNABORTED);
		return sctp_return_stop(skb);
	case SCTP_CAUSE_BAD_CHUNK_TYPE:
#if defined(CONFIG_SCTP_ADD_IP)||defined(CONFIG_SCTP_PARTIAL_RELIABILITY)
	{
		struct sctp_opt *sp = SCTP_PROT(sk);
		struct sctpchdr *ch = (typeof(ch)) (eh + 1);
		if (elen < sizeof(*eh) + sizeof(*ch))
			goto emsgsize;
		switch (ch->type) {
#if defined(CONFIG_SCTP_ADD_IP)
		case SCTP_CTYPE_ASCONF:
		case SCTP_CTYPE_ASCONF_ACK:
			sctp_abort_asconf(sk);
			sp->p_caps &= ~SCTP_CAPS_ADD_IP;
			sp->p_caps &= ~SCTP_CAPS_SET_IP;
			break;
#endif
#if defined(CONFIG_SCTP_PARTIAL_RELIABILITY)
		case SCTP_CTYPE_FORWARD_TSN:
			/* This is actually worse than it looks: we were told that
			   the other end was PR capable. */
			sp->p_caps &= ~SCTP_CAPS_PR;
			break;
#endif
		}
		break;
	}
#endif
	case SCTP_CAUSE_BAD_PARM:
#if defined(CONFIG_SCTP_ADD_IP)||defined(CONFIG_SCTP_PARTIAL_RELIABILITY)||defined(CONFIG_SCTP_ADAPTATION_LAYER_INFO)
	{
		struct sctp_opt *sp = SCTP_PROT(sk);
		struct sctpphdr *ph = (typeof(ph)) (eh + 1);
		if (elen < sizeof(*eh) + sizeof(*ph))
			goto emsgsize;
		switch (ph->type) {
#if defined(CONFIG_SCTP_ADD_IP)
		case SCTP_PTYPE_ADD_IP:
		case SCTP_PTYPE_DEL_IP:
			/* FIXME: what about valid sets with invalid adds ? */
			sctp_abort_asconf(sk);
			sp->p_caps &= ~SCTP_CAPS_ADD_IP;
			break;
		case SCTP_PTYPE_SET_IP:
			/* FIXME: what about valid sets with invalid adds ? */
			sctp_abort_asconf(sk);
			sp->p_caps &= ~SCTP_CAPS_SET_IP;
			break;
#endif
#if defined(CONFIG_SCTP_PARTIAL_RELIABILITY)
		case SCTP_PTYPE_PR_SCTP:
			/* This is actually worse than it looks: we were told that
			   the other end was PR capable. */
			sp->p_caps &= ~SCTP_CAPS_PR;
			break;
#endif
#if defined(CONFIG_SCTP_ADD_IP) || defined(CONFIG_SCTP_ADAPTATION_LAYER_INFO)
		case SCTP_PTYPE_ALI:
			sp->p_caps &= ~SCTP_CAPS_ALI;
			break;
#endif
#ifdef CONFIG_SCTP_ECN
		case SCTP_PTYPE_ECN_CAPABLE:
			sp->p_caps &= ~SCTP_CAPS_ECN;
			INET_ECN_dontxmit(sk);
			break;
#endif
		}
		break;
	}
#endif
	default:
	case SCTP_CAUSE_NO_DATA:
	case SCTP_CAUSE_SHUTDOWN:
	case SCTP_CAUSE_USER_INITIATED:
	case SCTP_CAUSE_PROTO_VIOLATION:
		/* These should either not come here or result in an abort */
	case SCTP_CAUSE_ILLEGAL_ASCONF:	/* Illegal ASCONF-ACK */
	case SCTP_CAUSE_LAST_ADDR:	/* Request to delete last address */
	case SCTP_CAUSE_RES_SHORTAGE:	/* Operation refused resource shortage */
	case SCTP_CAUSE_SOURCE_ADDR:	/* Request to delete source address */
		rare();
		break;
	}
	return sctp_return_more(skb);	/* ignore */
      emsgsize:
	return (-EMSGSIZE);
}

/*
 *  RECV ABORT
 *  -------------------------------------------------------------------------
 *  We have received an ABORT chunk in opening, connected or closing states.  If
 *  there is a user around we want to send a disconnect indication, otherwise we
 *  want to just go away.
 */
STATIC int sctp_recv_abort_listening(struct sock *sk, struct sk_buff *skb)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sk_buff *skp;
	for (skp = skb_peek(&sp->accq); skp; skp = skb_next(skp)) {
		struct sctp_cookie_echo *m = (typeof(m)) skp->data;
		struct sctp_cookie *ck = (typeof(ck)) m->cookie;
		if (ck->v_tag == SCTP_SKB_SH(skb)->v_tag &&
		    ck->sport == SCTP_SKB_SH(skb)->dest &&
		    ck->dport == SCTP_SKB_SH(skb)->srce &&
		    ck->saddr == skb->nh.iph->daddr &&
		    ck->daddr == skb->nh.iph->saddr) {
			printd(("INFO: Removing Connection Indication on ABORT\n"));
			skb_unlink(skp);
			sk->ack_backlog--;
			break;
		}
	}
	return sctp_return_stop(skb);
}
STATIC int sctp_recv_abort(struct sock *sk, struct sk_buff *skb)
{
	int err;
	struct sctp_abort *m = (typeof(m)) skb->data;
	size_t clen = ntohs(m->ch.len);
	if (clen < sizeof(*m) || PADC(clen) < skb->len)
		goto emsgsize;
	switch (sk->state) {
	case SCTP_LISTEN:
		return sctp_recv_abort_listening(sk, skb);
	case SCTP_COOKIE_WAIT:
	case SCTP_COOKIE_ECHOED:
		err = ECONNREFUSED;
		break;
	case SCTP_SHUTDOWN_SENT:
	case SCTP_SHUTDOWN_RECEIVED:
	case SCTP_SHUTDOWN_RECVWAIT:
	case SCTP_SHUTDOWN_ACK_SENT:
		err = EPIPE;
		break;
	case SCTP_ESTABLISHED:
	case SCTP_SHUTDOWN_PENDING:
		err = ECONNRESET;
		break;
	default:
		swerr();
		return sctp_return_stop(skb);
	}
	sctp_discon_ind(sk, err);
	return sctp_return_stop(skb);
      emsgsize:
	return (-EMSGSIZE);
}

/*
 *  RECV INIT (Listener only)
 *  -------------------------------------------------------------------------
 *  We have receive an INIT in the LISTEN state.  This is the normal path for
 *  the establishment of an SCTP association.  There can be no other stream
 *  bound to this local port but we can have accepted streams which share the
 *  same local binding.
 *
 *  INIT chunks cannot have other chunks bundled with them (RFC 2960 6.10).
 */
STATIC int sctp_recv_init(struct sock *sk, struct sk_buff *skb)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_init *m = (struct sctp_init *) skb->data;
	size_t clen = ntohs(m->ch.len);
	struct sock *oldsk;
	union sctp_parm *ph;
	unsigned char *pptr, *pend;
	size_t plen;
	size_t anum = 0;
	ulong ck_inc = 0;
	int err = 0;
	size_t errl = 0;
	unsigned char *errp = NULL;
	struct sctp_cookie ck;
	struct iphdr *iph = skb->nh.iph;
	struct sk_buff *unrec = NULL;
#ifdef CONFIG_SCTP_THROTTLE_PASSIVEOPENS
	static ulong last_init = 0;
	if (last_init && jiffies < last_init + sp->throttle)
		goto ebusy;
	last_init = jiffies;
#endif
	pptr = (unsigned char *) (m + 1);
	pend = pptr + clen - sizeof(*m);
	if (clen < sizeof(*m))
		goto emsgsize;
	/* SCTP IG 2.11 not allowed to bundle */
	if (skb->len > PADC(clen))
		goto eproto;
	/* RFC 2960 p.26 initiate tag zero */
	if (!m->i_tag)
		goto invalid_parm;
	if (!(unrec = alloc_skb(skb->len, GFP_ATOMIC)))
		goto enobufs;
	sp->p_caps = 0;
	for (ph = (union sctp_parm *) pptr;
	     pptr + sizeof(ph->ph) <= pend &&
	     pptr + (plen = ntohs(ph->ph.len)) <= pend;
	     pptr += PADC(plen), ph = (union sctp_parm *) pptr) {
		uint type;
		if (plen < sizeof(ph->ph))
			goto eproto;
		switch ((type = ph->ph.type)) {
		case SCTP_PTYPE_IPV6_ADDR:
		case SCTP_PTYPE_HOST_NAME:
			goto bad_address;
		case SCTP_PTYPE_COOKIE_PSRV:
			if (plen < sizeof(ph->cookie_prsv))
				goto init_bad_parm;
			ck_inc = ntohl(ph->cookie_prsv.ck_inc);
			break;
		case SCTP_PTYPE_IPV4_ADDR:
			if (plen < sizeof(ph->ipv4_addr))
				goto init_bad_parm;
			/* skip primary */
			if (ph->ipv4_addr.addr != skb->nh.iph->saddr)
				anum++;
			break;
		case SCTP_PTYPE_ADDR_TYPE:
			if (plen < sizeof(ph->addr_type))
				goto init_bad_parm;
			/* Ensure that address types supported includes IPv4.
			   Actually address types must include IPv4 so we just
			   ignore. */
			break;
#ifdef CONFIG_SCTP_ECN
		case SCTP_PTYPE_ECN_CAPABLE:
			if (plen < sizeof(ph->ecn_capable))
				goto init_bad_parm;
			sp->p_caps |= SCTP_CAPS_ECN;
			INET_ECN_xmit(sk);
			break;
#endif
#if defined(CONFIG_SCTP_ADD_IP) || defined(CONFIG_SCTP_ADAPTATION_LAYER_INFO)
		case SCTP_PTYPE_ALI:
			if (plen < sizeof(ph->ali))
				goto init_bad_parm;
			sp->p_ali = ntohl(ph->ali.ali);
			sp->p_caps |= SCTP_CAPS_ALI;
			break;
#endif
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
		case SCTP_PTYPE_PR_SCTP:
			if (plen < sizeof(ph->pr_sctp))
				goto init_bad_parm;
			sp->p_caps |= SCTP_CAPS_PR;
			break;
#endif
		default:
		      init_bad_parm:
			/* SCTP IG 2.27 */
			if (!(type & SCTP_PTYPE_MASK_CONTINUE)) {
				if (type & SCTP_PTYPE_MASK_REPORT)
					goto bad_parm;
				goto discard;
			}
			if (type & SCTP_PTYPE_MASK_REPORT)
				memcpy(skb_put(unrec, PADC(plen)), pptr, PADC(plen));
			continue;
		}
	}
	/* put together cookie */
	{
		size_t istrs = ntohs(m->n_istr);
		size_t ostrs = ntohs(m->n_ostr);
		/* negotiate inbound and outbound streams */
		if (!istrs || !ostrs)
			goto invalid_parm;
		istrs = (sp->n_istr && sp->n_istr <= istrs) ? sp->n_istr : istrs;
		ostrs = (sp->n_ostr && sp->n_ostr <= ostrs) ? sp->n_ostr : ostrs;
		if (!istrs || !ostrs)
			goto invalid_parm;
		/* RFC 2969 5.2.6 */
		if (ck_inc) {
			seldom();
			ck_inc = (ck_inc * HZ + 999) / 1000;
			usual(ck_inc);
			if (ck_inc > sp->rto_ini + sp->ck_inc) {
				rare();
				ck_inc = sp->rto_ini + sp->ck_inc;
			}
		}
		ck.timestamp = jiffies;
		ck.lifespan = sp->ck_life + ck_inc;
		ck.daddr = skb->nh.iph->saddr;
		ck.saddr = skb->nh.iph->daddr;
		ck.dport = SCTP_SKB_SH(skb)->srce;
		ck.sport = SCTP_SKB_SH(skb)->dest;
		ck.v_tag = sctp_get_vtag(ck.daddr, ck.saddr, ck.dport, ck.sport);
		ck.p_tag = m->i_tag;
		ck.p_tsn = ntohl(m->i_tsn);
		ck.p_rwnd = ntohl(m->a_rwnd);
		ck.n_istr = istrs;
		ck.n_ostr = ostrs;
		ck.danum = anum;
	}
	/* RFC 2960 5.2.2 Note */
	ck.l_ttag = 0;
	ck.p_ttag = 0;
	/* 
	 *  SCTP IG 2.6 replacement: This IG section would have us do a hard
	 *  lookup to return an error.  We just lookup on IP/SCTP header
	 *  information which means if a hijacker sends an INIT with from an
	 *  extra address attempting to hijack a connection we will respond with
	 *  an INIT ACK but will not populate tie tags or initiate tag with
	 *  existing tags.  This way the tags of the existing association and
	 *  information about the addresses for the association are never
	 *  exposed, yet we reconcile the situation properly when we receive the
	 *  COOKIE ECHO.
	 */
	if ((oldsk = sctp_lookup_tcb(SCTP_SKB_SH(skb)->dest, SCTP_SKB_SH(skb)->srce,
				     skb->nh.iph->daddr, skb->nh.iph->saddr))) {
		struct sctp_opt *oldsp = SCTP_PROT(oldsk);
		if (oldsk->state != SCTP_COOKIE_WAIT) {
			/* RFC 2960 5.2.1 and 5.2.2 Note */
			rare();
			ck.l_ttag = oldsp->v_tag;
			ck.p_ttag = oldsp->p_tag;
		}
		sock_put(oldsk);
	}
	ck.opt_len = 0;
	if (sk->state == SCTP_LISTEN && !(sk->userlocks & SOCK_BINDADDR_LOCK)) {
		/* always include destination address */
		if (!sctp_saddr_include(sk, ck.saddr, &err) && err)
			goto no_resource;
		/* build a temporary address list */
		__sctp_get_addrs(sk, iph->saddr);
	}
	if (sp->sanum < 1)
		goto no_resource;
	ck.sanum = sp->sanum - 1;	/* skip primary */
	sctp_send_init_ack(sk, skb->nh.iph->daddr, skb->nh.iph->saddr,
			   SCTP_SKB_SH(skb), &ck, unrec);
      cleanup:
	if (sk->state == SCTP_LISTEN && !(sk->userlocks & SOCK_BINDADDR_LOCK))
		__sctp_free_saddrs(sk);	/* remove temporary address list */
      discard:
	if (unrec)
		kfree_skb(unrec);
	/* SCTP IG 2.2, 2.11 */
	return sctp_return_stop(skb);
      no_resource:
	err = -SCTP_CAUSE_NO_RESOURCE;
	errp = NULL;
	errl = 0;
	ptrace(("PROTO: couldn't allocate source addresses\n"));
	goto error;
      bad_address:
	err = -SCTP_CAUSE_BAD_ADDRESS;
	errp = pptr;
	errl = plen;
	ptrace(("PROTO: hostname or IPv6 address\n"));
	goto error;
      invalid_parm:
	err = -SCTP_CAUSE_INVALID_PARM;
	errp = NULL;
	errl = 0;
	ptrace(("PROTO: missing initiate tag\n"));
	goto error;
      bad_parm:
	err = -SCTP_CAUSE_BAD_PARM;
	errp = pptr;
	errl = plen;
	ptrace(("PROTO: unrecognized or poorly formatted optional parameter\n"));
	goto error;
      error:
	/* SCTP IG 2.23 Abort uses initiate tag as verification tag */
	sctp_send_abort_error_init(sk, skb->nh.iph->saddr, skb->nh.iph->daddr,
				   SCTP_SKB_SH(skb), m->i_tag, -err, errp, errl);
	goto cleanup;
#ifdef CONFIG_SCTP_THROTTLE_PASSIVEOPENS
      ebusy:
	err = -EBUSY;
	goto return_error;
#endif
      eproto:
	err = -EPROTO;
	goto return_error;
      emsgsize:
	err = -EMSGSIZE;
	goto return_error;
      enobufs:
	err = -ENOBUFS;
	goto return_error;
      return_error:
	if (unrec)
		kfree_skb(unrec);
	return (err);
}

/*
 *  RECV INIT ACK
 *  -------------------------------------------------------------------------
 *  We have recevied an INIT ACK in the SCTP_COOKIE_WAIT (TS_WCON_CREQ) state.
 *  (RFC 2960 5.2.3)
 */
STATIC int sctp_recv_init_ack(struct sock *sk, struct sk_buff *skb)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	int err = 0;
	int reason;
	if (sk->state == SCTP_COOKIE_WAIT) {
		struct iphdr *iph = skb->nh.iph;
		struct sctp_init_ack *m = (struct sctp_init_ack *) skb->data;
		size_t clen = ntohs(m->ch.len);
		if (clen < sizeof(*m))
			goto emsgsize;
		/* SCTP IG 2.11 not allowed to bundle */
		if (skb->len > PADC(clen))
			goto eproto;
		if (sctp_daddr_include(sk, iph->saddr, &err)) {
			unsigned char *kptr = NULL;
			size_t klen = 0;
			unsigned char *pptr = (unsigned char *) (m + 1);
			unsigned char *pend = pptr + PADC(clen) - sizeof(*m);
			size_t plen;
			struct sctpphdr *ph = (struct sctpphdr *) pptr;
			if (pend < skb->tail)
				goto emsgsize;
			{
				size_t ostr = ntohs(m->n_ostr);
				size_t istr = ntohs(m->n_istr);
				/* SCTP IG 2.5 */
				if (!ostr || !istr || ostr > sp->max_istr)
					goto einval;
				/* SCTP IG 2.22 */
				if (!m->i_tag)
					goto einval;
				sp->p_rwnd = ntohl(m->a_rwnd);
				sp->p_rbuf =
				    (sp->p_rbuf >= sp->p_rwnd)
				    ? sp->p_rbuf : sp->p_rwnd;
				/* SCTP IG 2.19 correct */
				sp->n_ostr = ostr;
				sp->n_istr = istr;
				sp->r_ack = ntohl(m->i_tsn) - 1;
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
				sp->p_fsn = ntohl(m->i_tsn) - 1;
#endif
#ifdef CONFIG_SCTP_ADD_IP
				sp->p_asn = ntohl(m->i_tsn) - 1;
#endif
			}
			sp->p_caps = 0;
			for (ph = (struct sctpphdr *) pptr;
			     pptr + sizeof(*ph) <= pend &&
			     pptr + (plen = ntohs(ph->len)) <= pend;
			     pptr += PADC(plen), ph = (struct sctpphdr *) pptr) {
				uint type;
				if (plen < sizeof(ph->type))
					goto emsgsize;
				switch ((type = ph->type)) {
				case SCTP_PTYPE_IPV4_ADDR:
					if (!sctp_daddr_include
					    (sk, *((uint32_t *) (ph + 1)), &err)) {
						rare();
						sctp_send_error(sk,
								SCTP_CAUSE_BAD_ADDRESS,
								(caddr_t) ph, plen);
					}
					continue;
				case SCTP_PTYPE_STATE_COOKIE:
					kptr = pptr + sizeof(*ph);
					klen = plen - sizeof(*ph);
					continue;
				case SCTP_PTYPE_IPV6_ADDR:
				case SCTP_PTYPE_HOST_NAME:
					rare();
					sctp_send_error(sk,
							SCTP_CAUSE_BAD_ADDRESS,
							(caddr_t) ph, plen);
					continue;
#ifdef CONFIG_SCTP_ECN
				case SCTP_PTYPE_ECN_CAPABLE:
					sp->p_caps |= SCTP_CAPS_ECN;
					INET_ECN_xmit(sk);
					break;
#endif
#if defined(CONFIG_SCTP_ADD_IP) || defined(CONFIG_SCTP_ADAPTATION_LAYER_INFO)
				case SCTP_PTYPE_ALI:
					sp->p_ali =
					    ntohl(((struct sctp_ali *) ph)->ali);
					sp->p_caps |= SCTP_CAPS_ALI;
					break;
#endif
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
				case SCTP_PTYPE_PR_SCTP:
					sp->p_caps |= SCTP_CAPS_PR;
					break;
#endif
				case SCTP_PTYPE_UNREC_PARMS:
#if defined(CONFIG_SCTP_ADD_IP) || defined(CONFIG_SCTP_ADAPTATION_LAYER_INFO) || defined(CONFIG_SCTP_PARTIAL_RELIABILITY)
					/* unrecongized parameters valid */
					break;
#endif
				default:
					rare();
					reason = SCTP_CAUSE_BAD_PARM;
					if (type & SCTP_PTYPE_MASK_REPORT)
						sctp_send_error(sk, reason,
								(caddr_t) ph, plen);
					if (type & SCTP_PTYPE_MASK_CONTINUE)
						continue;
					/* SCTP IG 2.2 */
					return sctp_return_stop(skb);
				}
			}
			if (!kptr) 	/* no cookie? */
				goto einval;
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
			if (sp->prel == SCTP_PR_REQUIRED &&
			    !(sp->p_caps & SCTP_CAPS_PR))
				/* require partial reliability support */
				/* abort the init process */
				goto einval;
#endif
			sctp_ack_calc(sk, &sp->timer_init);
			local_bh_disable();
			if (sp->pprev)
				__sctp_phash_unhash(sp);
			sp->p_tag = m->i_tag;
			__sctp_phash_insert(sp);
			local_bh_enable();
			sctp_send_cookie_echo(sk, kptr, klen);
			return sctp_return_stop(skb);
		}
		rare();
		return (err);	/* fall back on timer init */
	}
	rare();
	return (0); /* ignore */
      einval:
	err = -EINVAL;
	ptrace(("Missing or invalid mandatory parameter\n"));
	goto error;
      eproto:
	err = -EPROTO;
	ptrace(("Protocol violation\n"));
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	ptrace(("Bad chunk or parameter length\n"));
	goto error;
      error:
	return (err);
}

/*
 *  RECV COOKIE ECHO
 *  -------------------------------------------------------------------------
 *  We have received a COOKIE ECHO for a socket.  We have already determined the
 *  socket to which the COOKIE ECHO applies.  We must still verify the cookie
 *  and apply the appropriate action per RFC 2960 5.2.4.
 */
STATIC void sctp_reset(struct sock *sk);
STATIC void sctp_clear(struct sock *sk);
STATIC int sctp_update_from_cookie(struct sock *sk, struct sctp_cookie *ck)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	int err = 0;
	uint32_t *daddrs = (uint32_t *) (ck + 1);
	uint32_t *saddrs = daddrs + ck->danum;
	if (!(sk->userlocks & SOCK_BINDADDR_LOCK)) {
		if (!sctp_saddr_include(sk, ck->saddr, &err) && err)
			goto error;
		if ((err = sctp_alloc_saddrs(sk, ck->sport, saddrs, ck->sanum)))
			goto error;
	}
	if (!sctp_daddr_include(sk, ck->daddr, &err) && err)
		goto error;
	if ((err = sctp_alloc_daddrs(sk, ck->dport, daddrs, ck->danum)))
		goto error;
	if ((err = sctp_conn_hash(sk)))
		goto error;
	if ((err = sctp_update_routes(sk, 1)))
		goto error;
	sk->daddr = ck->daddr;
	sk->saddr = ck->saddr;
	sk->rcv_saddr = ck->saddr;
	sk->protinfo.af_inet.opt = NULL;
	sk->protinfo.af_inet.id = ck->v_tag ^ jiffies;
	sp->n_istr = ck->n_istr;
	sp->n_ostr = ck->n_ostr;
	sp->t_tsn = ck->v_tag;
	sp->t_ack = ck->v_tag - 1;
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
	sp->l_fsn = ck->v_tag - 1;
#endif
#ifdef CONFIG_SCTP_ADD_IP
	sp->l_asn = ck->v_tag - 1;
#endif
	sp->m_tsn = ck->v_tag - 1;
	sp->r_ack = ck->p_tsn - 1;
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
	sp->p_fsn = ck->p_tsn - 1;
#endif
#ifdef CONFIG_SCTP_ADD_IP
	sp->p_asn = ck->p_tsn - 1;
#endif
	sp->p_rwnd = ck->p_rwnd;
	sp->p_rbuf = ck->p_rwnd;
	sp->a_rwnd = sk->rcvbuf;
	return (0);
      error:
	return (err);
}

STATIC int sctp_recv_cookie_echo(struct sock *sk, struct sk_buff *skb)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	int err;
	struct iphdr *iph = (struct iphdr *) skb->nh.iph;
	struct sctphdr *sh = SCTP_SKB_SH(skb);
	struct sctp_cookie_echo *m = (typeof(m)) skb->data;
	struct sctp_cookie *ck = (typeof(ck)) m->cookie;
	size_t clen = ntohs(m->ch.len);
#ifdef CONFIG_SCTP_THROTTLE_PASSIVEOPENS
	static ulong last_cookie_echo = 0;
	if (last_cookie_echo && jiffies < last_cookie_echo + sp->throttle)
		return (-EBUSY);
	last_cookie_echo = jiffies;
#endif
	if (clen < sizeof(*m) + sizeof(*ck))
		goto emsgsize;
	if (clen < sizeof(*m) + raw_cookie_size(ck) + HMAC_SIZE)
		goto emsgsize;
	/* RFC 2960 5.2.4 (1) & (2) */
	if ((err = sctp_verify_cookie(sk, ck))) {
		rare();
		return (err);
	}
	if (sp->v_tag) {
		if (ck->v_tag != sp->v_tag) {
			if (ck->p_tag != sp->p_tag) {
				if (ck->l_ttag == sp->v_tag &&
				    ck->p_ttag == sp->p_tag)
					/* RFC 2960 5.2.4. Action (A) */
					goto recv_cookie_echo_action_a;
				if (ck->l_ttag == 0 && ck->p_ttag == 0)
					goto recv_cookie_echo_action_p;
			} else if (ck->l_ttag == 0 && ck->p_ttag == 0)
				/* RFC 2960 5.2.4. Action (C). */
				goto recv_cookie_echo_action_c;
		} else {
			if (!sp->p_tag ||
			    ((1 << sk->state) &
			     (SCTPF_COOKIE_WAIT | SCTPF_COOKIE_ECHOED)))
				/* RFC 2960 5.2.4 Action (B). */
				goto recv_cookie_echo_action_b;
			else if (ck->p_tag != sp->p_tag)
				/* RFC 2960 5.2.4 Action (B) */
				goto recv_cookie_echo_action_b;
			else
				/* RFC 2960 5.2.4 Action (D). */
				goto recv_cookie_echo_action_d;
		}
	} else
		/* RFC 2960 5.1 */
		goto recv_cookie_echo_listen;
	rare();
	/* RFC 2960 5.2.4 ...silently discarded */
	return (0);
      recv_cookie_echo_action_a:
	/* 
	 *  RFC 2960 5.2.4 Action (A)
	 *
	 *  In this case, the peer may have restarted.  When the endpoint
	 *  recognizes this potential 'restart', the existing session is treated
	 *  the same as if it received an ABORT followed by a new COOKIED ECHO
	 *  with the following exceptions:  - Any SCTP DATA Chunks MAY be
	 *  retained (this is an implementation specific option).  - A
	 *  notification of RESTART SHOULD be sent to the ULP instead of
	 *  "COMMUNICATION LOST" notification.
	 *
	 *  All the Congestion control parameters (e.g., cwnd, ssthresh) related
	 *  to this peer MUST be reset to their initial values (see Section
	 *  6.2.1).  After this the endpoint shall enter the ESTABLISHED
	 *  state.
	 *
	 *  If the endpoint is in the SHUTDOWN-ACK-SENT state and recognizes the
	 *  peer has restarted (Action A), it MUST NOT setup a new association
	 *  but instead resend the SHUTDOWN ACK and send an ERROR chunk with a
	 *  "Cookie Received while Shutting Down" error cause to its peer.
	 */
	/* RFC 2960 5.2.4 (3) */
	if (jiffies > ck->timestamp + ck->lifespan) {
		uint32_t staleness;
		rare();
		staleness =
		    htonl((jiffies - ck->timestamp - ck->lifespan) * HZ / 1000000);
		sctp_send_abort_error_ootb(iph->saddr, iph->daddr, sh,
					   SCTP_CAUSE_STALE_COOKIE,
					   (caddr_t) & staleness, sizeof(staleness));
		return (-ETIMEDOUT);
	}
	switch (sk->state) {
	case SCTP_SHUTDOWN_ACK_SENT:
		rare();
		/* RFC 2960 5.2.4 (A) */
		sctp_send_abort_error(sk, SCTP_CAUSE_SHUTDOWN, NULL, 0);
		sctp_send_shutdown_ack(sk);
		return sctp_return_stop(skb);
	case SCTP_ESTABLISHED:
	case SCTP_SHUTDOWN_PENDING:
	case SCTP_SHUTDOWN_SENT:
	case SCTP_SHUTDOWN_RECEIVED:
	case SCTP_SHUTDOWN_RECVWAIT:
		rare();
		/* RFC 2960 5.2.4 (A) */
		sctp_send_abort(sk);	/* abort old association */
		if (!sk->dead) {
			sctp_reset(sk);
			sp->v_tag = ck->v_tag;
			sp->p_tag = ck->p_tag;
			if ((err = sctp_update_from_cookie(sk, ck)))
				goto error_abort;
			sctp_clear(sk);
			sctp_change_state(sk, SCTP_ESTABLISHED);
			sctp_error_report(sk, ECONNRESET);
			sk->state_change(sk);
			sctp_send_cookie_ack(sk, skb);
			return (0);
		}
		sctp_discon_ind(sk, ECONNRESET);
		return (-ECONNRESET);
	default:
		rare();
		return (-EFAULT);
	}
	never();
      recv_cookie_echo_action_b:
	rare();
	/* 
	 *  RFC 2960 5.2.4 Action (B)
	 *
	 *  In this case, both sides may be attempting to start and association
	 *  at about the same time but the peer endpoint started its INIT after
	 *  responding to the local endpoint's INIT.  Thus it may have picked a
	 *  new Verification Tag not being aware of the previous Tag it had sent
	 *  this endpoint.  The endpoint should stay in or enter the ESTABLISHED
	 *  state but it MUST update its peer's Verification Tag from the
	 *  State Cookie, stop any init or cookie timers that may be running and
	 *  send a COOKIE ACK.
	 */
	/* RFC 2960 5.2.4 (3) */
	if (jiffies > ck->timestamp + ck->lifespan) {
		uint32_t staleness;
		rare();
		staleness =
		    htonl((jiffies - ck->timestamp - ck->lifespan) * HZ / 1000000);
		sctp_send_abort_error_ootb(iph->saddr, iph->daddr, sh,
					   SCTP_CAUSE_STALE_COOKIE,
					   (caddr_t) & staleness, sizeof(staleness));
		return (-ETIMEDOUT);
	}
	switch (sk->state) {
	case SCTP_COOKIE_WAIT:
		del_timer(&sp->timer_init);
	case SCTP_COOKIE_ECHOED:
		del_timer(&sp->timer_cookie);
		/* We haven't got an INIT ACK yet so we need some stuff from the cookie. */
		sk->prot->unhash(sk);
		if ((err = sctp_update_from_cookie(sk, ck)))
			goto error_abort;
	case SCTP_ESTABLISHED:
		break;
	default:
		rare();
		return (-EFAULT);
	}
	local_bh_disable();
	if (sp->pprev)
		__sctp_phash_unhash(sp);
	sp->p_tag = ck->p_tag;
	__sctp_phash_insert(sp);
	local_bh_enable();
	sctp_send_sack(sk);
	goto recv_cookie_echo_action_d;
      recv_cookie_echo_action_c:
	/* 
	 *  RFC 2960 5.2.4 Action (C)
	 *
	 *  In this case, the local endpoint's cookie has arrived late.  Before
	 *  it arrived, the local sendpoint sent an INIT and receive an INIT-ACK
	 *  and finally sent a COOKIE ECHO with the peer's same tag but a new
	 *  tag of its own.  The cookie should be silently discarded.  The
	 *  endpoint SHOULD NOT change states and should leave any timers
	 *  running.
	 */
	rare();
	/* RFC 2960 5.2.4 (3) */
	if (jiffies > ck->timestamp + ck->lifespan) {
		uint32_t staleness;
		rare();
		staleness =
		    htonl((jiffies - ck->timestamp - ck->lifespan) * HZ / 1000000);
		sctp_send_abort_error_ootb(iph->saddr, iph->daddr, sh,
					   SCTP_CAUSE_STALE_COOKIE,
					   (caddr_t) & staleness, sizeof(staleness));
		return (-ETIMEDOUT);
	}
	return (0);
      recv_cookie_echo_action_d:
	/* 
	 *  RFC 2960 5.2.4 Action (D)
	 *
	 *  When both local and remote tags match the endpoint should always
	 *  enter the ESTABLISHED state, it if has not already done so.  It
	 *  should stop any init or cookie timers that may be running and send a
	 *  COOKIE ACK.
	 */
	/* SCTP IG 2.29 */
	switch (sk->state) {
	case SCTP_COOKIE_WAIT:
		del_timer(&sp->timer_init);
	case SCTP_COOKIE_ECHOED:
		del_timer(&sp->timer_cookie);
		sctp_change_state(sk, SCTP_ESTABLISHED);
		if (!sk->dead)
			sk->state_change(sk);
		if (sk->socket)
			sk_wake_async(sk, 0, POLL_OUT);
	case SCTP_ESTABLISHED:
		sctp_send_cookie_ack(sk, skb);
		return (0);
	default:
		rare();
		/* RFC 2960 5.2.4 ...silently discarded */
		return (0);
	}
	never();
      recv_cookie_echo_listen:
	if (sk->max_ack_backlog) {
		struct sk_buff *skp;
		/* check for multiples */
		for (skp = skb_peek(&sp->accq); skp; skp = skb_next(skp)) {
			struct sctp_cookie_echo *m = (typeof(m)) skp->data;
			struct sctp_cookie *cp = (typeof(cp)) m->cookie;
			if (cp->v_tag == ck->v_tag &&
			    cp->dport == ck->dport && cp->sport == ck->sport &&
			    cp->daddr == ck->daddr && cp->saddr == ck->saddr) {
				printd(("INFO: Discarding multiple COOKIE-ECHO\n"));
				return (0);	/* discard multiple */
			}
		}
		/* RFC 2960 5.2.4 (4) */
		if (sk->ack_backlog >= sk->max_ack_backlog) {
			seldom();
			sctp_send_abort_error_ootb(iph->saddr, iph->daddr, sh,
						   SCTP_CAUSE_NO_RESOURCE, NULL, 0);
			NET_INC_STATS_BH(ListenOverflows);
			return (0);
		}
		sctp_conn_ind(sk, skb);
		return (0);
	}
	return (0);
      recv_cookie_echo_action_p:
	/* 
	 *  SCTP IG 2.6 replacement:
	 *
	 *  In this case we may have refused to divulge existing tags to a
	 *  potential attacker and poisoned the cookie and the peer did the same
	 *  to us or we have not received an INIT-ACK yet.  The INIT address is
	 *  now known to us, we will process the cookie as though we had handed
	 *  out the verification tag in the initiate tag.
	 */
	/* RFC 2960 5.2.4 (3) */
	if (jiffies > ck->timestamp + ck->lifespan) {
		uint32_t staleness;
		rare();
		staleness =
		    htonl((jiffies - ck->timestamp - ck->lifespan) * HZ / 1000000);
		sctp_send_abort_error_ootb(iph->saddr, iph->daddr, sh,
					   SCTP_CAUSE_STALE_COOKIE,
					   (caddr_t) & staleness, sizeof(staleness));
		return (-ETIMEDOUT);
	}
	switch (sk->state) {
	case SCTP_COOKIE_WAIT:
		local_bh_disable();
		__sctp_vhash_unhash(sp);
		sp->v_tag = ck->v_tag;
		__sctp_vhash_insert(sp);
		local_bh_enable();
		goto recv_cookie_echo_action_b;
	case SCTP_COOKIE_ECHOED:
		ck->v_tag = sp->v_tag;
		goto recv_cookie_echo_action_b;
	}
	rare();
	return (0);
      error_abort:
	/* We had a fatal error processing the COOKIE ECHO and must abort the association */
	sctp_send_abort_ootb(iph->saddr, iph->daddr, sh);
	sctp_discon_ind(sk, -err);
	return (err);
      emsgsize:
	err = -EMSGSIZE;
	return (err);
}

/*
 *  RECV COOKIE ACK
 *  -------------------------------------------------------------------------
 *  We have received a COOKIE ACK.  If we are in COOKIE ECHOED state then we
 *  inform the user interface that the previous connection request is confirmed,
 *  cancel the cookie timer while performing an RTO calculation on the message
 *  and enter the ESTABLISHED state.  Any DATA that is bundled with the COOKIE
 *  ACK will be separately indicated to the user with data indications.  In
 *  state other than COOKIE ECHOED the entire message is silently discarded.
 */
STATIC int sctp_recv_cookie_ack(struct sock *sk, struct sk_buff *skb)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_daddr *sd;
	struct sctp_cookie_ack *m = (typeof(m)) skb->data;
	size_t clen = ntohs(m->ch.len);
	if (sk->state != SCTP_COOKIE_ECHOED)
		goto outstate;
	if (clen < sizeof(*m))
		goto emsgsize;
	/* RFC 2960 5.1 (E) */
	sctp_change_state(sk, SCTP_ESTABLISHED);
	sctp_ack_calc(sk, &sp->timer_cookie);
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPACTIVEESTABS
	SCTP_INC_STATS_BH(SctpActiveEstabs);
#endif
	/* start idle timers */
	usual(sp->daddr);
	for (sd = sp->daddr; sd; sd = sd->next)
		sctp_reset_idle(sd);
	/* XXX: we can probably POLL_OUT after receving INIT-ACK */
	if (!sk->dead)
		sk->state_change(sk);
	else
		swerr();
	if (sk->socket)
		sk_wake_async(sk, 0, POLL_OUT);
	return sctp_return_more(skb);
      emsgsize:
	return (-EMSGSIZE);
      outstate:
	/* RFC 2960 5.2.5 */
	rare();
	return (0);		/* silently discard */
}

/*
 *  RECV HEARTBEAT
 *  -------------------------------------------------------------------------
 *  We have received a HEARTBEAT.  Quite frankly we don't care what state we
 *  are in, we take the heartbeat info and turn it back around as a HEARTBEAT
 *  ACK msg.  We do a little bit of error checking here to make sure that we are
 *  not wasting our time on the packet.  We should only receive HEARTBEAT
 *  messages if we are in the vtag hashes: meaning that we will accept and reply
 *  to a HEARTBEAT in any state other than CLOSED and LISTEN.
 */
STATIC int sctp_recv_heartbeat(struct sock *sk, struct sk_buff *skb)
{
	int err;
	struct sctp_heartbeat *m = (typeof(m)) skb->data;
	struct sctpphdr *ph = (typeof(ph)) (m + 1);
	size_t clen = ntohs(m->ch.len);
	size_t plen;
	if (clen < sizeof(*m) + sizeof(*ph) || PADC(clen) < skb->len)
		goto emsgsize;
#ifdef CONFIG_SCTP_THROTTLE_HEARTBEATS
	{
		struct sctp_opt *sp = SCTP_PROT(sk);
		if (jiffies < sp->hb_rcvd + sp->hb_tint)
			goto ebusy;
		sp->hb_rcvd = jiffies;
	}
#endif
	if (ph->type != SCTP_PTYPE_HEARTBEAT_INFO)
		goto eproto;
	plen = ntohs(ph->len);
	if (plen < sizeof(*ph) || plen > clen - sizeof(*m))
		goto emsgsize;
	sctp_send_heartbeat_ack(sk, (caddr_t) ph, plen);
	return sctp_return_stop(skb);
      eproto:
	err = -EPROTO;
	ptrace(("PROTO: bad message\n"));
	goto error;
#ifdef CONFIG_SCTP_THROTTLE_HEARTBEATS
      ebusy:
	err = -EBUSY;
	ptrace(("WARNING: throttling heartbeat\n"));
	goto error;
#endif
      emsgsize:
	err = -EMSGSIZE;
	ptrace(("PROTO: bad message size\n"));
	goto error;
      error:
	return (err);
}

/*
 *  RECV HEARTBEAT ACK
 *  -------------------------------------------------------------------------
 *  This is our HEARTBEAT coming back.  We check that the HEARTBEAT information
 *  matches the information of the last sent HEARTBEAT message to ensure that no
 *  fiddling with the HEARTBEAT info has occured.  The only information we trust
 *  initially is the destination address which is contained in the HEARTBEAT
 *  INFO.  This just helps us index the remainder of the hearbeat information.
 *
 *  We have received a HEARTBEAT ACK message on an ESTABLISHED socket.  Perform
 *  the procedures from RFC 2960 8.3.
 */
STATIC int sctp_recv_heartbeat_ack(struct sock *sk, struct sk_buff *skb)
{
	int err;
	struct sctp_daddr *sd;
	struct sctp_heartbeat_ack *m = (typeof(m)) skb->data;
	struct sctp_heartbeat_info *hb = (typeof(hb)) (m + 1);
	size_t clen = ntohs(m->ch.len);
	if (clen < sizeof(*m) + sizeof(*hb) || PADC(clen) < skb->len)
		goto emsgsize;
	if (!(sd = sctp_find_daddr(sk, hb->hb_info.daddr)))
		goto ignore;
	if (ntohs(hb->ph.len) != sizeof(*hb) + sd->hb_fill)
		goto emsgsize;
	if (hb->ph.type != SCTP_PTYPE_HEARTBEAT_INFO)
		goto eproto;
	if (sd->hb_time && sd->hb_time != hb->hb_info.timestamp)
		goto ignore;
	del_timer(&sd->timer_heartbeat);
	sctp_rtt_calc(sd, sd->hb_time);
      ignore:
	return sctp_return_stop(skb);
      eproto:
	err = -EPROTO;
	ptrace(("PROTO: bad heatbeat parameter type\n"));
	goto error;
      emsgsize:
	err = -EMSGSIZE;
	ptrace(("PROTO: bad parameter or chunk size\n"));
	goto error;
      error:
	return (err);
}

/*
 *  RECV SHUTDOWN
 *  -------------------------------------------------------------------------
 */
STATIC void sctp_shutdown_ack_calc(struct sock *sk, uint32_t ack)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	if (before(ack, sp->t_ack)) {
		rare();
		/* If the SHUTDOWN acknowledges our sent data chunks that have
		   already been acknowledged, then it is an old (or erroneous)
		   message and we will ignore it. */
		return;
	}
	if (after(ack, sp->t_ack))
		sctp_cumm_ack(sk, ack);
	sctp_dest_calc(sk);
}
STATIC int sctp_recv_shutdown(struct sock *sk, struct sk_buff *skb)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_shutdown *m = (typeof(m)) skb->data;
	size_t clen = ntohs(m->ch.len);
	uint32_t ack = ntohl(m->c_tsn);
	if (clen < sizeof(*m))
		goto emsgsize;
	switch (sk->state) {
	case SCTP_ESTABLISHED:
		sctp_change_state(sk, SCTP_SHUTDOWN_RECEIVED);
		/* fall thru */
	case SCTP_SHUTDOWN_RECEIVED:
		sctp_shutdown_ack_calc(sk, ack);
		if (!sk->dead)
			break;
		/* fall thru */
	case SCTP_SHUTDOWN_PENDING:	/* only when we have ordrel */
		sctp_change_state(sk, SCTP_SHUTDOWN_RECVWAIT);
		/* fall thru */
	case SCTP_SHUTDOWN_RECVWAIT:
		sctp_shutdown_ack_calc(sk, ack);
		if (atomic_read(&sk->wmem_alloc) <= 0)
			sctp_send_shutdown_ack(sk);
		break;
	case SCTP_SHUTDOWN_SENT:	/* only when we have ordrel */
		sctp_shutdown_ack_calc(sk, ack);
		/* faill thru */
	case SCTP_SHUTDOWN_ACK_SENT:
		sctp_send_shutdown_ack(sk);	/* We do this */
		break;
	default:
		/* ignore the SHUTDOWN chunk */
		rare();
		return sctp_return_more(skb);
	}
	if (sp->ngaps) {
		rare();
		/* Check sanity of sender: if we have gaps in our acks to the peer
		   and the peer sends a SHUTDOWN, then it is in error. The peer
		   cannot send SHUTDOWN when it has unacknowledged data. If this is
		   the case, we zap the connection. */
		sctp_send_abort_error(sk, SCTP_CAUSE_PROTO_VIOLATION, NULL, 0);
		sctp_discon_ind(sk, EPIPE);
		return sctp_return_stop(skb);
	}
	/* send up orderly release indication to ULP */
	sctp_ordrel_ind(sk);
	return sctp_return_more(skb);
      emsgsize:
	return (-EMSGSIZE);
}

/*
 *  RECV SHUTDOWN ACK
 *  -------------------------------------------------------------------------
 *  We have received a SHUTDOWN ACK chunk through the vtag hashes.  We are
 *  expecting the SHUTDOWN ACK because we have previously sent a SHUTDOWN or
 *  SHUTDOWN ACK or we ignore the message.
 */
STATIC int sctp_recv_shutdown_ack(struct sock *sk, struct sk_buff *skb)
{
	struct sctp_shutdown_ack *m = (typeof(m)) skb->data;
	size_t clen = ntohs(m->ch.len);
	if (clen < sizeof(*m))
		goto emsgsize;
	switch (sk->state) {
	case SCTP_SHUTDOWN_SENT:
		/* send up orderly release indication to ULP */
		sctp_ordrel_ind(sk);
		/* fall thru */
	case SCTP_SHUTDOWN_ACK_SENT:
//              sctp_ack_calc(sk, &SCTP_PROT(sk)->timer_shutdown);     /* WHY? */
		sctp_send_shutdown_complete(sk);
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPSHUTDOWNS
		SCTP_INC_STATS_BH(SctpShutdowns);
#endif
		sctp_discon_ind(sk, 0);
		break;
	default:
		/* ignore unexpected SHUTDOWN ACK */
		rare();
		break;
	}
	return sctp_return_stop(skb);
      emsgsize:
	return (-EMSGSIZE);
}

/*
 *  RECV SHUTDOWN COMPLETE
 *  -------------------------------------------------------------------------
 *  We have received a SHUTDOWN COMPLETE in the SHUTDOWN ACK SENT state.  This
 *  is the normal path for shutting down an SCTP association.  Outstanding data
 *  has already been processed.  Remove ourselves from the hashes and process
 *  any backlog.  RFC 2960 6.10: SHUTDOWN COMPLETE cannot have any other chunks
 *  bundled with them.
 *
 *  We receive SHUTDOWN COMPLETE chunks through the vtag hashes.  We are
 *  expecting SHUTDOWN COMPLETE because we sent a SHUTDOWN ACK.  We are in the
 *  TS_IDLE, NS_IDLE states but are still in the vtag hashes.
 */
STATIC int sctp_recv_shutdown_complete(struct sock *sk, struct sk_buff *skb)
{
	struct sctp_shutdown_comp *m = (typeof(m)) skb->data;
	size_t clen = ntohs(m->ch.len);
	if (clen < sizeof(*m))
		goto emsgsize;
	switch (sk->state) {
	case SCTP_SHUTDOWN_ACK_SENT:
//              sctp_ack_calc(sk, &SCTP_PROT(sk)->timer_shutdown); /* WHY? */
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPSHUTDOWNS
		SCTP_INC_STATS_BH(SctpShutdowns);
#endif
		sctp_discon_ind(sk, 0);
		break;
	default:
		/* ignore unexpected SHUTDOWN COMPLETE */
		rare();
		break;
	}
	return sctp_return_stop(skb);
      emsgsize:
	return (-EMSGSIZE);
}

/*
 *  RECV UNRECOGNIZED CHUNK TYPE
 *  -------------------------------------------------------------------------
 */
STATIC int sctp_recv_unrec_ctype(struct sock *sk, struct sk_buff *skb)
{
	struct sctpchdr *ch = (typeof(ch)) skb->data;
	size_t clen = ntohs(ch->len);
	uint8_t ctype;
	if (clen < sizeof(ch))
		goto emsgsize;
	ctype = ch->type;
	if (ctype & SCTP_CTYPE_MASK_REPORT)
		sctp_send_error(sk, SCTP_CAUSE_BAD_CHUNK_TYPE, skb->data,
				skb->len);
	if (ctype & SCTP_CTYPE_MASK_CONTINUE)
		return sctp_return_more(skb);
	return (0);		/* discard packet */
      emsgsize:
	return (-EMSGSIZE);
}

#ifdef CONFIG_SCTP_ECN
/*
 *  RECV ECNE (Explicit Contestion Notification Echo)
 *  -------------------------------------------------------------------------
 */
STATIC int sctp_recv_ecne(struct sock *sk, struct sk_buff *skb)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_ecne *m = (typeof(m)) skb->data;
	size_t clen = ntohs(m->ch.len);
	uint32_t l_tsn;
	struct sctp_daddr *sd;
	if (clen < sizeof(*m))
		goto emsgsize;
	l_tsn = ntohl(m->l_tsn);
	sp->p_caps |= SCTP_CAPS_ECN;
	INET_ECN_xmit(sk);
	if (!after(l_tsn, sp->p_lsn))
		goto done;
	sp->p_lsn = l_tsn;
	/* need to find the destination to which this TSN was transmitted */
	for (skb = skb_peek(&sp->rtxq); skb && SCTP_SKB_CB(skb)->tsn != l_tsn;
	     skb = skb_next(skb));
	if (!skb)
		goto done;
	if (!(sd = SCTP_SKB_CB(skb)->daddr))
		goto done;
	sctp_assoc_timedout(sk, sd, 0, 0);
      done:
	return sctp_return_more(skb);
      emsgsize:
	return (-EMSGSIZE);
}
#endif

#ifdef CONFIG_SCTP_ECN
/*
 *  RECV CWR (Congestion Window Reduction)
 *  -------------------------------------------------------------------------
 */
STATIC int sctp_recv_cwr(struct sock *sk, struct sk_buff *skb)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_cwr *m = (typeof(m)) skb->data;
	size_t clen = ntohs(m->ch.len);
	uint32_t l_tsn;
	if (clen < sizeof(*m))
		goto emsgsize;
	l_tsn = ntohl(m->l_tsn);
	fixme(("FIXME: Finish this function\n"));
	sp->sackf &= ~SCTP_SACKF_ECN;
	return sctp_return_more(skb);
      emsgsize:
	return (-EMSGSIZE);
}
#endif

#ifdef CONFIG_SCTP_ADD_IP

/*
 *  RECV ASCONF
 *  -------------------------------------------------------------------------
 */
STATIC int sctp_recv_asconf(struct sock *sk, struct sk_buff *skb)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_asconf *m = (typeof(m)) skb->data;
	size_t clen = ntohs(m->ch.len);
	uint32_t asn;
	struct sctp_daddr *sd;
	int reconfig = 0;
	if (clen < sizeof(*m) || PADC(clen) < skb->len)
		goto emsgsize;
	if (!(sp->l_caps & (SCTP_CAPS_ADD_IP | SCTP_CAPS_SET_IP)))
		goto refuse;
	/* ADD-IP 4.1.1 R4 */
	if (sk->state != SCTP_ESTABLISHED)
		goto discard;
	asn = ntohl(m->asn);
	if (asn == sp->p_asn + 1) {
		/* ADD-IP 4.2 Rule C1 & C2 */
		struct sctpehdr *eh;
		struct sctp_success_report *sr;
		struct sctp_error_cause *er;
		struct sctp_daddr *sd;
		union sctp_parm *ph;
		struct sctp_ipv4_addr *a;
		unsigned char *pptr = (unsigned char *) (m + 1);
		unsigned char *pend = pptr + ntohs(m->ch.len) - sizeof(*m);
		unsigned char *rptr;
		unsigned char *bptr;
		size_t rlen = 0;
		size_t plen;
		/* process the ASCONF chunk */
		for (ph = (union sctp_parm *) pptr;
		     pptr + sizeof(ph->ph) <= pend &&
		     pptr + (plen = ntohs(ph->ph.len)) <= pend;
		     pptr += PADC(plen), ph = (union sctp_parm *) pptr) {
			uint type;
			if (plen < sizeof(ph->ph))
				return -EPROTO;
			switch ((type = ph->ph.type)) {
			case SCTP_PTYPE_ADD_IP:
				if (!(sp->l_caps & SCTP_CAPS_ADD_IP))
					goto bad_parm;
				a = (typeof(a)) ((&ph->add_ip) + 1);
				if (ntohs(a->ph.len) != 8)
					goto bad_parm;
				if (a->ph.type != SCTP_PTYPE_IPV4_ADDR)
					goto bad_parm;
				if ((sd = sctp_find_daddr(sk, ntohl(a->addr)))) {
					/* fake success */
					rlen += sizeof(*sr);
					continue;
				}
				if ((sd = sctp_add_daddr(sk, ntohl(a->addr)))) {
					rlen += sizeof(*sr);
					reconfig++;
					continue;
				}
				/* resources or scope error */
				rlen += sizeof(*er) + sizeof(*eh) + plen;
				continue;
			case SCTP_PTYPE_DEL_IP:
				if (!(sp->l_caps & SCTP_CAPS_ADD_IP))
					goto bad_parm;
				a = (typeof(a)) ((&ph->add_ip) + 1);
				if (ntohs(a->ph.len) != 8)
					goto bad_parm;
				if (a->ph.type != SCTP_PTYPE_IPV4_ADDR)
					goto bad_parm;
				if (!(sd = sctp_find_daddr(sk, ntohl(a->addr)))) {
					/* fake success */
					rlen += sizeof(*sr);
					continue;
				}
				if (a->addr != skb->nh.iph->saddr && sp->danum != 1) {
					sctp_del_daddr(sd);
					rlen += sizeof(*sr);
					reconfig++;
					continue;
				}
				/* request to delete source address */
				/* last address need to send error */
				rlen += sizeof(*er) + sizeof(*eh) + plen;
				continue;
			case SCTP_PTYPE_SET_IP:
				if (!(sp->l_caps & SCTP_CAPS_SET_IP))
					goto bad_parm;
				a = (typeof(a)) ((&ph->add_ip) + 1);
				if (ntohs(a->ph.len) != 8)
					goto bad_parm;
				if (a->ph.type != SCTP_PTYPE_IPV4_ADDR)
					goto bad_parm;
				if (!(sd = sctp_find_daddr(sk, ntohl(a->addr))))
					goto bad_parm;
				/* fake success */
				rlen += sizeof(*sr);
				continue;
			default:
			      bad_parm:
				rare();
				if (type & SCTP_PTYPE_MASK_REPORT)
					sctp_send_error(sk, SCTP_CAUSE_BAD_PARM,
							(caddr_t) ph, plen);
				if (type & SCTP_PTYPE_MASK_CONTINUE)
					continue;
				/* SCTP IG 2.2 */
				goto discard;
			}
		}
		/* second pass to build response */
		if (!(bptr = rptr = kmalloc(rlen, GFP_ATOMIC)))
			return -ENOMEM;
		for (pptr = (unsigned char *) (m + 1), rlen = 0,
		     ph = (union sctp_parm *) pptr;
		     pptr + sizeof(ph->ph) <= pend &&
		     pptr + (plen = ntohs(ph->ph.len)) <= pend;
		     pptr += PADC(plen), ph = (union sctp_parm *) pptr) {
			uint type;
			switch ((type = ph->ph.type)) {
			case SCTP_PTYPE_ADD_IP:
				if (!(sp->l_caps & SCTP_CAPS_ADD_IP))
					continue;
				a = (typeof(a)) ((&ph->add_ip) + 1);
				if (ntohs(a->ph.len) != 8)
					continue;
				if (a->ph.type != SCTP_PTYPE_IPV4_ADDR)
					continue;
				if ((sd = sctp_find_daddr(sk, ntohl(a->addr)))) {
					sr = ((typeof(sr)) bptr)++;
					sr->ph.type = SCTP_PTYPE_SUCCESS_REPORT;
					sr->ph.len = __constant_htons(sizeof(*sr));
					sr->cid = ph->add_ip.id;
					rlen += sizeof(*sr);
					continue;
				} else {
					er = ((typeof(er)) bptr)++;
					er->ph.type = SCTP_PTYPE_ERROR_CAUSE;
					er->ph.len =
					    htons(sizeof(*er) + sizeof(*eh) + plen);
					er->cid = ph->del_ip.id;
					eh = ((typeof(eh)) bptr)++;
					eh->code = __constant_htons
					    (SCTP_CAUSE_RES_SHORTAGE);
					eh->len = htons(sizeof(*eh) + plen);
					memcpy(bptr, pptr, plen);
					bptr += plen;
					rlen += sizeof(*er) + sizeof(*eh) + plen;
					continue;
				}
				break;
			case SCTP_PTYPE_DEL_IP:
				if (!(sp->l_caps & SCTP_CAPS_ADD_IP))
					continue;
				a = (typeof(a)) ((&ph->add_ip) + 1);
				if (ntohs(a->ph.len) != 8)
					continue;
				if (a->ph.type != SCTP_PTYPE_IPV4_ADDR)
					continue;
				if (!(sd = sctp_find_daddr(sk, ntohl(a->addr)))) {
					sr = ((typeof(sr)) bptr)++;
					sr->ph.type = SCTP_PTYPE_SUCCESS_REPORT;
					sr->ph.len = __constant_htons(sizeof(*sr));
					sr->cid = ph->add_ip.id;
					rlen += sizeof(*sr);
					continue;
				}
				if (a->addr == skb->nh.iph->saddr) {
					/* request to delete source address */
					er = ((typeof(er)) bptr)++;
					er->ph.type = SCTP_PTYPE_ERROR_CAUSE;
					er->ph.len =
					    htons(sizeof(*er) + sizeof(*eh) + plen);
					er->cid = ph->del_ip.id;
					eh = ((typeof(eh)) bptr)++;
					eh->code =
					    __constant_htons(SCTP_CAUSE_SOURCE_ADDR);
					eh->len = htons(sizeof(*eh) + plen);
					memcpy(bptr, pptr, plen);
					bptr += plen;
					rlen += sizeof(*er) + sizeof(*eh) + plen;
					continue;
				}
				if (sp->danum == 1) {
					/* last address need to send error */
					er = ((typeof(er)) bptr)++;
					er->ph.type = SCTP_PTYPE_ERROR_CAUSE;
					er->ph.len = htons(sizeof(*er) + sizeof(*eh)
							   + plen);
					er->cid = ph->del_ip.id;
					eh = ((typeof(eh)) bptr)++;
					eh->code =
					    __constant_htons(SCTP_CAUSE_LAST_ADDR);
					eh->len = htons(sizeof(*eh) + plen);
					memcpy(bptr, pptr, plen);
					bptr += plen;
					rlen += sizeof(*er) + sizeof(*eh) + plen;
					continue;
				}
				break;
			case SCTP_PTYPE_SET_IP:
				if (!(sp->l_caps & SCTP_CAPS_SET_IP))
					continue;
				a = (typeof(a)) ((&ph->add_ip) + 1);
				if (ntohs(a->ph.len) != 8)
					continue;
				if (a->ph.type != SCTP_PTYPE_IPV4_ADDR)
					continue;
				if (!(sd = sctp_find_daddr(sk, ntohl(a->addr))))
					continue;
				/* fake success */
				sr = ((typeof(sr)) bptr)++;
				sr->ph.type = SCTP_PTYPE_SUCCESS_REPORT;
				sr->ph.len = __constant_htons(sizeof(*sr));
				sr->cid = ph->add_ip.id;
				rlen += sizeof(*sr);
				continue;
			}
		}
		sp->p_caps |= SCTP_CAPS_ADD_IP;
		sctp_send_asconf_ack(sk, rptr, rlen);
		/* update routes now if a configuration change occured */
		if (reconfig)
			sctp_update_routes(sk, 1);
	} else if (asn == sp->p_asn) {
		/* ADD-IP 4.2 Rule C1 & C3 */
		/* give the same reply as before */
		if (sp->reply) {
			if (!(sd = sctp_route_response(sk)))
				goto noroute;
			sctp_send_msg(sk, sd, sp->reply);
		}
	}
      discard:
	/* ADD-IP 4.2 Rule C1 & C4, R4 */
	return sctp_return_more(skb);
      noroute:
	ptrace(("ERROR: no route to peer\n"));
	return -EHOSTUNREACH;
      refuse:
	return sctp_recv_unrec_ctype(sk, skb);
      emsgsize:
	return (-EMSGSIZE);
}

/*
 *  RECV ASCONF ACK
 *  -------------------------------------------------------------------------
 */
STATIC int sctp_recv_asconf_ack(struct sock *sk, struct sk_buff *skb)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_asconf_ack *m = (typeof(m)) skb->data;
	size_t clen = ntohs(m->ch.len);
	uint32_t asn;
	if (clen < sizeof(*m) || PADC(clen) < skb->len)
		goto emsgsize;
	/* ADD-IP 4.1.1 R4 */
	if (sk->state != SCTP_ESTABLISHED)
		goto discard;
	asn = ntohl(m->asn);
	if (before(asn, sp->l_asn))
		goto discard;
	if (after(asn, sp->l_asn))
		goto abort;
	/* ADD-IP 4.3 D0 */
	if (!sp->retry && !timer_pending(&sp->timer_asconf))
		goto abort;
	/* ADD-IP 4.1 (A5) */
	sctp_ack_calc(sk, &sp->timer_asconf);
	/* process the ASCONF ACK */
	{
		union sctp_parm *ph;
		unsigned char *pptr = (unsigned char *) (m + 1);
		unsigned char *pend = pptr + ntohs(m->ch.len) - sizeof(*m);
		size_t plen;
		struct sctp_saddr *ss, *s2;
		/* process the ASCONF chunk */
		for (ph = (union sctp_parm *) pptr;
		     pptr + sizeof(ph->ph) <= pend &&
		     pptr + (plen = ntohs(ph->ph.len)) <= pend;
		     pptr += PADC(plen), ph = (union sctp_parm *) pptr) {
			uint type;
			if (plen < sizeof(ph->ph))
				return -EPROTO;
			switch ((type = ph->ph.type)) {
			case SCTP_PTYPE_ERROR_CAUSE:
				if (plen != sizeof(ph->error_cause))
					goto bad_parm;
				s2 = (typeof(s2)) ntohl(ph->error_cause.cid);
				for (ss = sp->saddr; ss && ss != s2; ss = ss->next);
				if (!ss)
					goto bad_parm;
				if (ss->flags & SCTP_SRCEF_ADD_PENDING) {
					ss->flags &= ~SCTP_SRCEF_ADD_PENDING;
					ss->flags |= SCTP_SRCEF_ADD_REQUEST;
				}
				if (ss->flags & SCTP_SRCEF_DEL_PENDING) {
					ss->flags &= ~SCTP_SRCEF_DEL_PENDING;
					ss->flags |= SCTP_SRCEF_DEL_REQUEST;
				}
				if (ss->flags & SCTP_SRCEF_SET_PENDING) {
					ss->flags &= ~SCTP_SRCEF_SET_PENDING;
					ss->flags |= SCTP_SRCEF_SET_REQUEST;
				}
				break;
			case SCTP_PTYPE_SUCCESS_REPORT:
				if (plen != sizeof(ph->success_report))
					goto bad_parm;
				s2 = (typeof(s2)) ntohl(ph->success_report.cid);
				for (ss = sp->saddr; ss && ss != s2; ss = ss->next);
				if (!ss)
					goto bad_parm;
				if (ss->flags & SCTP_SRCEF_ADD_PENDING) {
					ss->flags &= ~SCTP_SRCEF_ADD_PENDING;
				}
				if (ss->flags & SCTP_SRCEF_DEL_PENDING) {
					ss->flags &= ~SCTP_SRCEF_DEL_PENDING;
					__sctp_saddr_free(ss);
				}
				if (ss->flags & SCTP_SRCEF_SET_PENDING) {
					ss->flags &= ~SCTP_SRCEF_SET_PENDING;
				}
				break;
			default:
			      bad_parm:
				rare();
				if (type & SCTP_PTYPE_MASK_REPORT)
					sctp_send_error(sk, SCTP_CAUSE_BAD_PARM,
							(caddr_t) ph, plen);
				if (type & SCTP_PTYPE_MASK_CONTINUE)
					continue;
				/* SCTP IG 2.2 */
				goto discard;
			}
		}
		/* no news is good news */
		for (ss = sp->saddr; ss; ss = ss->next) {
			if (ss->flags & SCTP_SRCEF_ADD_PENDING) {
				ss->flags &= ~SCTP_SRCEF_ADD_PENDING;
			}
			if (ss->flags & SCTP_SRCEF_DEL_PENDING) {
				ss->flags &= ~SCTP_SRCEF_DEL_PENDING;
				__sctp_saddr_free(ss);
			}
			if (ss->flags & SCTP_SRCEF_SET_PENDING) {
				ss->flags &= ~SCTP_SRCEF_SET_PENDING;
			}
		}
	}
	sp->p_caps |= SCTP_CAPS_ADD_IP;
      discard:
	/* ADD-IP 4.2 Rule C1 & C4, R4 */
	return sctp_return_more(skb);
      abort:
	/* ADD-IP 4.3 D0 */
	/* abort the association with reason INVALID ASCONF ACK */
	sctp_send_abort_error(sk, SCTP_CAUSE_ILLEGAL_ASCONF, NULL, 0);
	sctp_discon_ind(sk, ECONNABORTED);
	return (-ECONNABORTED);
      emsgsize:
	return (-EMSGSIZE);
}

#endif

#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
/*
 *  RECV FORWARD TSN
 *  -------------------------------------------------------------------------
 *  Do a force cleanup of the receive gaps and move the cummulative ack point.
 */
STATIC int sctp_recv_forward_tsn(struct sock *sk, struct sk_buff *skb)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_forward_tsn *m = (typeof(m)) skb->data;
	size_t clen = htons(m->ch.len);
	uint32_t f_tsn;
	size_t nstrs;
	if ((1 << sk->state) & ~(SCTPF_RECEIVING))
		goto outstate;
	if (clen < sizeof(*m) || PADC(clen) < skb->len)
		goto emsgsize;
	f_tsn = ntohl(m->f_tsn);
	if (!(sp->p_caps & SCTP_CAPS_PR)) {
		seldom();
		/* strange, the peer didn't tell us on INIT(ACK) */
		sp->p_caps |= SCTP_CAPS_PR;
	}
	if (!after(f_tsn, sp->p_fsn))
		goto sctp_recv_fsn_duplicate;
	sp->p_fsn = f_tsn;
	if ((nstrs = (clen - sizeof(*m)) / sizeof(uint32_t))) {
		int err = 0;
		uint16_t *p = (uint16_t *) (m + 1);
		while (nstrs--) {
			uint16_t sid, ssn;
			struct sctp_strm *st;
			sid = *p++;
			ssn = *p++;
			/* push the sequence number */
			if ((st = sctp_istrm_find(sk, sid, &err)))
				if (!after_s(st->ssn, ssn))
					st->ssn = ssn;
		}
	}
	if (!sp->ngaps) {
		/* push ack point and mark sack */
		sp->r_ack = sp->p_fsn;
		sctp_send_sack(sk);
	} else {
		/* need to clean up any gaps */
		sctp_deliver_data(sk, skb);
	}
	return sctp_return_more(skb);
      sctp_recv_fsn_duplicate:
	/* FSN is a duplicate */
	/* TODO: Here we should peg the duplicate against the last destination to
	   which we sent a SACK.  Receiving duplicate FSNs is a weak indication that
	   our SACKs might not be getting through. */
      outstate:
	return sctp_return_more(skb);
      emsgsize:
	return (-EMSGSIZE);
}
#endif

/*
 *  RECV SCTP MESSAGE
 *  -------------------------------------------------------------------------
 */
STATIC int sctp_rcv_ootb(struct sk_buff *skb);
STATIC int sctp_recv_msg(struct sock *sk, struct sk_buff *skb)
{
	int err = skb->len;
	struct sctpchdr *ch;
	/* set address for reply chunks */
	if (!(SCTP_PROT(sk)->caddr = sctp_find_daddr(sk, skb->nh.iph->saddr))) {
		ch = (typeof(ch)) skb->data;
		switch (ch->type) {
		case SCTP_CTYPE_INIT:
		case SCTP_CTYPE_INIT_ACK:
		case SCTP_CTYPE_COOKIE_ECHO:
			break;
		default:
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTOFBLUES
			SCTP_INC_STATS_BH(SctpOutOfBlues);
#endif
			return sctp_rcv_ootb(skb);
		}
	}
	do {
		uint8_t type;
		ch = (typeof(ch)) skb->data;
		if ((type = ch->type) == SCTP_CTYPE_DATA) {
			err = sctp_recv_data(sk, skb);
		} else {
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPINCTRLCHUNKS
			SCTP_INC_STATS(SctpInCtrlChunks);
#endif
			switch (type) {
			case SCTP_CTYPE_INIT:
				err = sctp_recv_init(sk, skb);
				break;
			case SCTP_CTYPE_INIT_ACK:
				err = sctp_recv_init_ack(sk, skb);
				break;
			case SCTP_CTYPE_SACK:
				err = sctp_recv_sack(sk, skb);
				break;
			case SCTP_CTYPE_HEARTBEAT:
				err = sctp_recv_heartbeat(sk, skb);
				break;
			case SCTP_CTYPE_HEARTBEAT_ACK:
				err = sctp_recv_heartbeat_ack(sk, skb);
				break;
			case SCTP_CTYPE_ABORT:
				err = sctp_recv_abort(sk, skb);
				break;
			case SCTP_CTYPE_SHUTDOWN:
				err = sctp_recv_shutdown(sk, skb);
				break;
			case SCTP_CTYPE_SHUTDOWN_ACK:
				err = sctp_recv_shutdown_ack(sk, skb);
				break;
			case SCTP_CTYPE_ERROR:
				err = sctp_recv_error(sk, skb);
				break;
			case SCTP_CTYPE_COOKIE_ECHO:
				err = sctp_recv_cookie_echo(sk, skb);
				break;
			case SCTP_CTYPE_COOKIE_ACK:
				err = sctp_recv_cookie_ack(sk, skb);
				break;
#ifdef CONFIG_SCTP_ECN
			case SCTP_CTYPE_ECNE:
				err = sctp_recv_ecne(sk, skb);
				break;
			case SCTP_CTYPE_CWR:
				err = sctp_recv_cwr(sk, skb);
				break;
#endif
			case SCTP_CTYPE_SHUTDOWN_COMPLETE:
				err = sctp_recv_shutdown_complete(sk, skb);
				break;
#ifdef CONFIG_SCTP_ADD_IP
			case SCTP_CTYPE_ASCONF:
				err = sctp_recv_asconf(sk, skb);
				break;
			case SCTP_CTYPE_ASCONF_ACK:
				err = sctp_recv_asconf_ack(sk, skb);
				break;
#endif
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
			case SCTP_CTYPE_FORWARD_TSN:
				err = sctp_recv_forward_tsn(sk, skb);
				break;
#endif
			default:
				err = sctp_recv_unrec_ctype(sk, skb);
				break;
			}
		}
	} while (err >= sizeof(struct sctpchdr) && skb->len >= sizeof(struct sctpchdr));
	assure(err <= 0 || skb->len == 0);
	if (err < 0)
		goto error;
	/* 
	 *  NOTE: always free the skb.  If the receive function called wishes
	 *  to keep a copy it should either clone the skb or do an skb_get()
	 *  to increase the ref count.
	 */
      done:
	kfree_skb(skb);
	return (0);
      error:
	ptrace(("sk = %x, Error %d returned.\n", (uint) sk, err));
	/* 
	 *   NOTE: There are some errors that are returned by the receive
	 *   functions that are not handled by those function but are handled
	 *   here.  These are exceptional errro conditions.
	 */
	switch (err) {
	case -ENOMEM:
	case -ENOBUFS:
	case -EBUSY:
		/* These are resource problems */
		if ((1 << sk->state) & (SCTPF_NEEDABORT))
			sctp_send_abort_error(sk, SCTP_CAUSE_RES_SHORTAGE, NULL, 0);
		sctp_discon_ind(sk, ECONNABORTED);
		break;
	case -EPROTO:
		/* This is a protocol violation */
		if ((1 << sk->state) & (SCTPF_NEEDABORT))
			sctp_send_abort_error(sk, SCTP_CAUSE_PROTO_VIOLATION, NULL, 0);
		sctp_discon_ind(sk, -err);
		break;
	case -EINVAL:
		/* This is an invalid parameter */
		if ((1 << sk->state) & (SCTPF_NEEDABORT))
			sctp_send_abort_error(sk, SCTP_CAUSE_INVALID_PARM, NULL, 0);
		sctp_discon_ind(sk, -err);
		break;
	case -EMSGSIZE:
		/* This is a message formatting error */
		if ((1 << sk->state) & (SCTPF_NEEDABORT))
			sctp_send_abort(sk);
		sctp_discon_ind(sk, -err);
		break;
	default:
		/* ignore others handled specially inside receive functions */
		goto done;
	}
	goto done;
}

/*
 *  ==========================================================================
 *
 *  SOCKET PROTOCOL INTERFACE
 *
 *  ==========================================================================
 *
 *  MAJOR INET FUNCTIONS:
 *
 *  --------------------------------------------------------------------------
 *
 *  SCTP POLL
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Wait for an SCTP event, ala TCP.
 *
 */
unsigned int sctp_poll(struct file *file, struct socket *sock, poll_table * wait)
{
	struct sock *sk = sock->sk;
	struct sctp_opt *sp = SCTP_PROT(sk);
	unsigned int mask = 0;
	poll_wait(file, sk->sleep, wait);
	if (sk->state == SCTP_LISTEN) {
		if (!skb_queue_empty(&sp->accq))
			mask |= POLLIN | POLLRDNORM;
		return mask;
	}
	if (sk->err)
		mask = POLLERR;
	if (sk->shutdown == SHUTDOWN_MASK && sk->state == SCTP_CLOSED)
		mask |= POLLHUP;
	if (sk->shutdown & RCV_SHUTDOWN)
		mask |= POLLIN | POLLRDNORM;
	if ((1 << sk->state) & ~(SCTPF_OPENING)) {
		if (!skb_queue_empty(&sk->rcvq)
		    || (sk->urginline && !skb_queue_empty(&sp->expq)))
			mask |= POLLIN | POLLRDNORM;
		if (!(sk->shutdown & SEND_SHUTDOWN)) {
			if (sctp_wspace(sk) >= sctp_min_wspace(sk))
				mask |= POLLOUT | POLLWRNORM;
			else {
				set_bit(SOCK_ASYNC_NOSPACE, &sk->socket->flags);
				set_bit(SOCK_NOSPACE, &sk->socket->flags);
				if (sctp_wspace(sk) >= sctp_min_wspace(sk))
					mask |= POLLOUT | POLLWRNORM;
			}
		}
		if (!skb_queue_empty(&sp->expq))
			mask |= POLLPRI;
	}
	return mask;
}

/*
 *  SCTP LISTEN
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Place the socket into the listen hashes, make sure to rebind the protocol
 *  number for listening and recheck for conflicts.  If there socket is
 *  already in the listening state, just adjust the backlog.
 */
int sctp_listen(struct socket *sock, int backlog)
{
	struct sock *sk = sock->sk;
	unsigned char old_state;
	int err;
	lock_sock(sk);
	err = -EINVAL;
	if (sock->state != SS_UNCONNECTED || (
#ifdef CONFIG_SCTP_TCP_COMPATIBLE
	     sock->type != SOCK_STREAM &&
#endif
	     sock->type != SOCK_SEQPACKET))
		goto out;
	old_state = sk->state;
	if (!(1 << old_state) & (SCTPF_CLOSED | SCTPF_LISTEN))
		goto out;
	if (old_state != SCTP_LISTEN) {
		sctp_change_state(sk, SCTP_LISTEN);
		if (sk->prot->get_port(sk, sk->num))
			goto eaddrinuse;
		sk->sport = htons(sk->num);
		sk->prot->hash(sk);
		sk->ack_backlog = 0;
	}
	sk->max_ack_backlog = (backlog < SOMAXCONN) ? backlog : SOMAXCONN;
	err = 0;
      out:
	release_sock(sk);
	return (err);
      eaddrinuse:
	sctp_change_state(sk, SCTP_CLOSED);
	err = -EADDRINUSE;
	goto out;
}

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP CLOSE
 *
 *  --------------------------------------------------------------------------
 *
 *  This is the close(2) call being peformed on the socket.  This will either
 *  result in the orderly shutdown of the SCTP association (if no data is left
 *  unread), or in the abort of the SCTP association (if data has been left
 *  unread on the socket).
 *
 *  Unlike TCP, we have no partially open sockets, really.  Partially open
 *  sockets have a state cookie in flight which can be ignored upon cookie echo.
 *
 */
/*
 *  SCTP RESET
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Reset the connection.  This just stops timers, pulls the socket out of the
 *  hashes (except for perhapps bind), frees simple retransmission, and zeroes
 *  connection info.  The socket is left bound and souce addresses are left
 *  allocated if these were explicitly bound with sctp_bind.
 */
STATIC void sctp_reset(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	ptrace(("Resetting socket sk=%p, state = %d\n", sk, sk->state));
	/* unhash and delte address lists */
	sctp_change_state(sk, SCTP_CLOSED);
	sk->prot->unhash(sk);
	/* stop timers */
	del_timer(&sp->timer_init);
	del_timer(&sp->timer_cookie);
	del_timer(&sp->timer_shutdown);
	del_timer(&sp->timer_guard);
	del_timer(&sp->timer_sack);
#ifdef CONFIG_SCTP_ADD_IP
	del_timer(&sp->timer_asconf);
#endif
#if defined(CONFIG_SCTP_LIFETIMES) || defined(CONFIG_SCTP_PARTIAL_RELIABILITY)
	del_timer(&sp->timer_life);
#endif
	/* free retry buffer */
	freechunks(xchg(&sp->retry, NULL));
	/* free reply buffer */
	freechunks(xchg(&sp->reply, NULL));
	sp->sackf = 0;	/* don't delay first sack */
	sp->in_flight = 0;
	sp->retransmits = 0;
	sp->n_istr = 0;
	sp->n_ostr = 0;
	sp->v_tag = 0;
	sp->p_tag = 0;
	sp->p_rwnd = 0;
	sp->p_rbuf = 0;
	sp->a_rwnd = sk->rcvbuf;
//      /* clear routing cache */
//      __sk_dst_reset(sk);
	sk->route_caps = 0;
	sp->pmtu = ip_rt_min_pmtu;
	sp->amps = sp->pmtu - sp->ext_header_len - sizeof(struct iphdr) - sizeof(struct sctphdr);
	/* decide what to keep for retrieval */
	switch (sp->disp) {
	case SCTP_DISPOSITION_NONE:
	{
		struct sk_buff *skb;
		while ((skb = __skb_dequeue(&sp->rtxq))) {
			struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
			if (cb->flags & SCTPCB_FLAG_CONF)
				__skb_queue_tail(&sp->ackq, skb);
			else
				kfree_skb(skb);
		}
		while ((skb = __skb_dequeue(&sp->urgq))) {
			struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
			if (cb->flags & SCTPCB_FLAG_CONF)
				__skb_queue_tail(&sp->ackq, skb);
			else
				kfree_skb(skb);
		}
		while ((skb = __skb_dequeue(&sk->sndq))) {
			struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
			if (cb->flags & SCTPCB_FLAG_CONF)
				__skb_queue_tail(&sp->ackq, skb);
			else
				kfree_skb(skb);
		}
		break;
	}
	case SCTP_DISPOSITION_UNSENT:
	{
		struct sk_buff *skb;
		while ((skb = __skb_dequeue(&sp->rtxq))) {
			struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
			if (cb->flags & SCTPCB_FLAG_CONF)
				__skb_queue_tail(&sp->ackq, skb);
			else
				kfree_skb(skb);
		}
		while ((skb = __skb_dequeue(&sp->urgq)))
			__skb_queue_tail(&sp->ackq, skb);
		while ((skb = __skb_dequeue(&sk->sndq)))
			__skb_queue_tail(&sp->ackq, skb);
		break;
	}
	case SCTP_DISPOSITION_SENT:
	{
		struct sk_buff *skb;
		while ((skb = __skb_dequeue(&sp->rtxq))) {
			struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
			if (cb->flags & SCTPCB_FLAG_CONF)
				__skb_queue_tail(&sp->ackq, skb);
			else if (cb->flags & SCTPCB_FLAG_SACKED ||
				 cb->flags & SCTPCB_FLAG_DROPPED)
				kfree_skb(skb);
			else
				__skb_queue_tail(&sp->ackq, skb);
		}
		while ((skb = __skb_dequeue(&sp->urgq)))
			__skb_queue_tail(&sp->ackq, skb);
		while ((skb = __skb_dequeue(&sk->sndq)))
			__skb_queue_tail(&sp->ackq, skb);
		break;
	}
	case SCTP_DISPOSITION_GAP_ACKED:
	case SCTP_DISPOSITION_ACKED:
	default:
	{
		struct sk_buff *skb;
		while ((skb = __skb_dequeue(&sp->rtxq)))
			__skb_queue_tail(&sp->ackq, skb);
		while ((skb = __skb_dequeue(&sp->urgq)))
			__skb_queue_tail(&sp->ackq, skb);
		while ((skb = __skb_dequeue(&sk->sndq)))
			__skb_queue_tail(&sp->ackq, skb);
		break;
	}
	}
	sp->nrtxs = 0;
	sp->nsack = 0;
	return;
}

/*
 *  SCTP LISTEN STOP
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This is for stopping a listening socket.  This breaks user locks, removes
 *  the listening socket from all the hashes (including bind), and orphans,
 *  disconnects and destroys each of the child sockets in the accept queue.
 */
STATIC void sctp_listen_stop(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	/* break user locks */
	sk->userlocks = 0;
	sk->prot->unhash(sk);
	sctp_change_state(sk, SCTP_CLOSED);
	__skb_queue_purge(&sp->accq);
	sk->ack_backlog = 0;
}

/*
 *  SCTP CLOSE
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */
STATIC void sctp_close(struct sock *sk, long timeout)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	int data_was_unread = 0;
	ptrace(("Closing socket sk=%p, state = %d\n", sk, sk->state));
	lock_sock(sk);
	sk->shutdown = SHUTDOWN_MASK;
	if (sk->state == SCTP_LISTEN) {
		ptrace(("Closing listening socket sk=%p\n", sk));
		sctp_listen_stop(sk);
		goto dead;
	}
	if ((1 << sk->state) & (SCTPF_OPENING)) {
		ptrace(("Closing opening socket sk=%p\n", sk));
		sctp_reset(sk);
		goto dead;
	}
	/* why would socket be dead? when could we double close? */
	if (!sk->dead)
		sk->state_change(sk);
	data_was_unread = skb_queue_len(&sk->rcvq) + skb_queue_len(&sp->expq);
	{
		__skb_queue_purge(&sk->rcvq);
		__skb_queue_purge(&sp->expq);
		sp->rmem_queued = 0;
		__skb_queue_purge(&sp->oooq);
		sp->gaps = NULL;
		sp->ngaps = 0;
		sp->nunds = 0;
		__skb_queue_purge(&sp->dupq);
		sp->dups = NULL;
		sp->ndups = 0;
		__skb_queue_purge(&sp->ackq);
	}
	if (data_was_unread) {
		printd(("INFO: Closing with unread data, sk=%p\n", sk));
		sk->prot->disconnect(sk, 0);
	} else if (sk->linger && sk->lingertime == 0) {
		sk->prot->disconnect(sk, 0);
	} else if ((1 << sk->state) & (SCTPF_CONNECTED)) {
		/* Perform SHUTDOWN procedure if all data read.  */
		printd(("INFO: Performing orderly shutdown sk=%p\n", sk));
		sk->shutdown = SHUTDOWN_MASK;
		switch (sk->state) {
		case SCTP_ESTABLISHED:
			sctp_change_state(sk, SCTP_SHUTDOWN_PENDING);
		case SCTP_SHUTDOWN_PENDING:
			if (atomic_read(&sk->wmem_alloc) <= 0)
				sctp_send_shutdown(sk);
			else
				sk->prot->disconnect(sk, 0);
			break;
		case SCTP_SHUTDOWN_RECEIVED:
			sctp_change_state(sk, SCTP_SHUTDOWN_RECVWAIT);
		case SCTP_SHUTDOWN_RECVWAIT:
			if (atomic_read(&sk->wmem_alloc) <= 0)
				sctp_send_shutdown_ack(sk);
			else
				sk->prot->disconnect(sk, 0);
			break;
		default:
			swerr();
			break;
		}
	}
	if (timeout) {
		DECLARE_WAITQUEUE(wait, current);
		ptrace(("Waiting for socket sk=%p to close\n", sk));
		printd(("INFO: Socket %p entering Linger wait queue\n", sk));
		add_wait_queue(sk->sleep, &wait);
		do {
			set_current_state(TASK_INTERRUPTIBLE);
			if ((1 << sk->state) & ~(SCTPF_CLOSING))
				break;
			release_sock(sk);
			if ((1 << sk->state) & ~(SCTPF_CLOSING))
				timeout = schedule_timeout(timeout);
			lock_sock(sk);
		} while (!signal_pending(current) && timeout);
		set_current_state(TASK_RUNNING);
		remove_wait_queue(sk->sleep, &wait);
	}
      dead:
	release_sock(sk);
	local_bh_disable();
	bh_lock_sock(sk);
	sock_hold(sk);
	/* we're dead */
	sk->use_write_queue = 1;	/* don't callback write space */
	sock_orphan(sk);
	atomic_inc(&sctp_orphan_count);
	printd(("INFO: There are now %d orphan sockets\n",
		atomic_read(&sctp_orphan_count)));
	/* release socket now, otherwise wait for the protocol to release the socket 
	   when the last reference to it is released (the protocol will release the
	   last reference when the connection is closed with sk->dead == 1 */
	if (sk->state == SCTP_CLOSED)
		sctp_destroy_orphan(sk);
	bh_unlock_sock(sk);
	local_bh_enable();
	usual(atomic_read(&sk->refcnt) == 1);
	if (atomic_read(&sk->refcnt) == 1)
		ptrace(("Immediate close socket %p\n", sk));
	else
		ptrace(("Delayed close socket %p, with refcnt = %d\n", sk,
			atomic_read(&sk->refcnt) - 1));
	sock_put(sk);
}

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP CONNECT
 *
 *  --------------------------------------------------------------------------
 *
 *  This is a direct extension of the connect(2) call.  For SCTP in accordance
 *  with draft-ietf-tsvwg-sctpsocket-06.txt this connect will result in the
 *  (ultimate) establishment of an association.  This is only performed after
 *  a bind (otherwise and automatic binding is attempted).  Only one address
 *  is required for a connect.
 *
 *  For sockets bound to INADDR_ANY, we generate the local IP address list on
 *  demand at the time that the connect() function is called.  This address
 *  list will include every device address available on the system.
 *
 *  TODO: We should also register INADDR_ANY bound sockets with the
 *  netdevice_notifier so that whne local addresses are added or removed, we
 *  can add and remove them from the association with ADD-IP.
 */
STATIC int sctp_connect(struct sock *sk, struct sockaddr *uaddr, int addr_len)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sockaddr_in *usin = (struct sockaddr_in *) uaddr;
	int err, addr_num;
	unsigned short dport;
	ptrace(("Connecting socket sk=%p, state = %d\n", sk, sk->state));
	if (!(addr_num = addr_len / sizeof(struct sockaddr_in)))
		return (-EINVAL);
	if (usin->sin_family != AF_INET && usin->sin_family)
		return (-EAFNOSUPPORT);
	if (sk->state != SCTP_CLOSED)
		return (-EISCONN);
	if (!(dport = usin->sin_port))
		return (-EADDRNOTAVAIL);
	if ((err = sctp_alloc_sock_daddrs(sk, dport, usin, addr_num)) < 0)
		return (err);
	if (err == 0)
		return (-EADDRNOTAVAIL);
	err = -EADDRNOTAVAIL;
	if (!sp->sanum || !sp->saddr) {
		if (!(sk->userlocks & SOCK_BINDADDR_LOCK)) {
			if (sctp_get_addrs(sk, sp->daddr->daddr))
				goto error;
		} else {
			swerr();
			goto error;
		}
	}
	if (!sp->bprev) {
		if (!(sk->userlocks & SOCK_BINDPORT_LOCK)) {
			if (sk->prot->get_port(sk, 0))
				goto error;
			sk->sport = htons(sk->num);
		} else {
			swerr();
			goto error;
		}
	}
	sk->dport = dport;
	sk->daddr = sp->daddr->daddr;
	sk->saddr = sp->saddr->saddr;
	sp->v_tag = sctp_get_vtag(sk->saddr, sk->daddr, sk->sport, usin->sin_port);
	sp->p_tag = 0;
	printd(("INFO: Assigned verification tag %08X\n", sp->v_tag));
	sk->protinfo.af_inet.id = sp->v_tag ^ jiffies;
	sp->ext_header_len = 0;
	if ((err = sctp_update_routes(sk, 1)))
		goto error;
	sp->n_istr = 0;
	sp->n_ostr = 0;
	sp->t_tsn = sp->v_tag;
	sp->t_ack = sp->v_tag - 1;
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
	sp->l_fsn = sp->v_tag - 1;
#endif
#ifdef CONFIG_SCTP_ADD_IP
	sp->l_asn = sp->v_tag - 1;
#endif
	sp->m_tsn = sp->v_tag - 1;
	sp->r_ack = 0;
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
	sp->p_fsn = 0;
#endif
#ifdef CONFIG_SCTP_ADD_IP
	sp->p_asn = 0;
#endif
	sp->ck_inc = 0;
	sk->err = 0;
	sk->err_soft = 0;
	sk->done = 0;
	if ((err = sctp_conn_hash(sk)))
		goto error;
	if (!sk->dead)
		sk->state_change(sk);
	else
		swerr();
	if ((err = sctp_send_init(sk)))
		goto error;
	return (0);
      error:
	sk->prot->unhash(sk);
	return (err);
}

/*
 *  -------------------------------------------------------------------------
 *
 *  SCTP DISCONNECT
 *
 *  -------------------------------------------------------------------------
 */
/*
 *  SCTP CLEAR
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  Clear the connection information hanging around on a socket.  This
 *  includes any queue data blocks that are waiting around for retrieval.  It
 *  is OK to call this function twice in a row on the same socket.
 */
STATIC void sctp_clear(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	ptrace(("Clearing socket sk=%p, state = %d\n", sk, sk->state));
	/* clear flags */
	sk->shutdown = 0;
	sk->done = 0;
	/* purge queues */
	__skb_queue_purge(&sk->rcvq);
	__skb_queue_purge(&sp->expq);
	sp->rmem_queued = 0;
	__skb_queue_purge(&sk->sndq);
	__skb_queue_purge(&sp->urgq);
	sk->wmem_queued = 0;
	__skb_queue_purge(&sp->errq);
	__skb_queue_purge(&sp->dupq);
	sp->dups = NULL;
	sp->ndups = 0;
	__skb_queue_purge(&sp->oooq);
	sp->gaps = NULL;
	sp->ngaps = 0;
	sp->nunds = 0;
	__skb_queue_purge(&sp->rtxq);
	sp->nrtxs = 0;
	__skb_queue_purge(&sp->ackq);
	/* free input and output streams */
	if (sp->ostrm || sp->istrm) {
		sctp_free_strms(sk);
	}
}

/*
 *  Note: Although we are disconnecting there may be other processes waiting
 *  in wait queues if the socket is not orphaned.  This is because one can now
 *  disconnect in linux using the AF_UNSPEC approach, which allows the socket
 *  to be reused without closing and reopening the socket.
 */
STATIC int sctp_disconnect(struct sock *sk, int flags)
{
	(void) flags;
	if (sk->state == SCTP_LISTEN) {
		sctp_listen_stop(sk);
	} else if ((1 << sk->state) & (SCTPF_NEEDABORT)) {
		/* SCTP IG 2.21 */
		sctp_send_abort_error(sk, SCTP_CAUSE_USER_INITIATED, NULL, 0);
		sk->err = ECONNRESET;
	} else if ((1 << sk->state) & (SCTPF_OPENING)) {
		sk->err = ECONNRESET;
	}
	sctp_reset(sk);
	if (!sk->dead)
		sk->error_report(sk);
	else
		swerr();
	return (0);
}

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP ACCEPT
 *
 *  --------------------------------------------------------------------------
 *
 *  This is the accept(2) call being performed on the socket.  This will
 *  result the the waiting for acceptance of an incoming SCTP association.
 *  This is only performed after a bind and a listen.  The socket must be in
 *  the listening state.  We do not autobind any longer.
 *
 *  Unlike TCP, we don't keep half-open sockets on the accept list.  This is
 *  because we do not keep a record of half-open sockets in SCTP, all this
 *  state information is contained in the state cookie.  The only sockets on
 *  our sp->accept_next list are sockets for which a COOKIE ACK was sent and
 *  are fully formed sockets.
 */
STATIC struct sock *sctp_conn_res(struct sock *lsk, struct sk_buff *skb, int *errp)
{
	struct sctp_opt *lsp = SCTP_PROT(lsk);
	struct sctp_opt *sp;
	struct sock *sk;
	int err = 0;
	struct sctp_cookie_echo *m = (typeof(m)) skb->data;
	struct sctp_cookie *ck = (typeof(ck)) m->cookie;
	uint32_t *daddrs = (uint32_t *) (ck + 1);
	uint32_t *saddrs = daddrs + ck->danum;
	err = -ENOMEM;
	if (!(sk = sk_alloc(PF_INET, GFP_ATOMIC, 0)))
		goto error;
	/* 
	 *  Initialize socket information
	 */
	memcpy(sk, lsk, sizeof(*sk));
	sock_lock_init(sk);
	bh_lock_sock(sk);
	sk->pprev = NULL;
	sk->prev = NULL;
	sk->dst_lock = RW_LOCK_UNLOCKED;
	sk->dst_cache = NULL;
	sk->route_caps = 0;	/* this is set up in update routes */
	atomic_set(&sk->rmem_alloc, 0);
	atomic_set(&sk->wmem_alloc, 0);
	atomic_set(&sk->omem_alloc, 0);
	skb_queue_head_init(&sk->rcvq);
	skb_queue_head_init(&sk->sndq);
	sk->wmem_queued = 0;
	skb_queue_head_init(&sk->errq);
	sk->forward_alloc = 0;
	sk->done = 0;
	sk->proc = 0;
	sk->backlog.head = sk->backlog.tail = NULL;
	sk->callback_lock = RW_LOCK_UNLOCKED;
#ifdef CONFIG_FILTER
	if (sk->filter)
		sk_filter_charge(sk, sk->filter);
#endif
#if defined HAVE___XFRM_SK_CLONE_POLICY_EXPORT || defined HAVE___XFRM_SK_CLONE_POLICY_ADDR
	if (unlikely(xfrm_sk_clone_policy(sk))) {
		sk->destruct = NULL;
		sk_free(sk);
		return NULL;
	}
#endif
	sk->err = 0;
	sk->priority = 0;
	sk->socket = NULL;
	sk->sleep = NULL;
	init_timer(&sk->timer);
//      sk->timer.function = &sctp_timeout;
//      sk->timer.data = (unsigned long) sk;
//      sk->timeout = 0;
	sk->daddr = ck->daddr;
	sk->saddr = ck->saddr;
	sk->rcv_saddr = ck->saddr;
	/* TODO: handle IP options sk->protinfo.af_inet.opt */
	sk->protinfo.af_inet.opt = NULL;
	sk->protinfo.af_inet.id = ck->v_tag ^ jiffies;
	sk->shutdown = 0;
	/* 
	 *  Initialize protocol information
	 */
	sp = SCTP_PROT(sk);
	memset(sp, 0, sizeof(*sp));	/* more zeros than anything else */
	/* initialize timers */
	init_timer(&sp->timer_init);
	sp->timer_init.function = &sctp_init_timeout;
	sp->timer_init.data = (unsigned long) sk;
	init_timer(&sp->timer_cookie);
	sp->timer_cookie.function = &sctp_cookie_timeout;
	sp->timer_cookie.data = (unsigned long) sk;
	init_timer(&sp->timer_shutdown);
	sp->timer_shutdown.function = &sctp_shutdown_timeout;
	sp->timer_shutdown.data = (unsigned long) sk;
	init_timer(&sp->timer_guard);
	sp->timer_guard.function = &sctp_guard_timeout;
	sp->timer_guard.data = (unsigned long) sk;
	init_timer(&sp->timer_sack);
	sp->timer_sack.function = &sctp_sack_timeout;
	sp->timer_sack.data = (unsigned long) sk;
#ifdef CONFIG_SCTP_ADD_IP
	init_timer(&sp->timer_asconf);
	sp->timer_asconf.function = &sctp_asconf_timeout;
	sp->timer_asconf.data = (unsigned long) sk;
#endif
#if defined(CONFIG_SCTP_LIFETIMES) || defined(CONFIG_SCTP_PARTIAL_RELIABILITY)
	init_timer(&sp->timer_life);
	sp->timer_life.function = &sctp_life_timeout;
	sp->timer_life.data = (unsigned long) sk;
#endif
	/* initialize queues */
	skb_queue_head_init(&sp->accq);
	skb_queue_head_init(&sp->urgq);
	skb_queue_head_init(&sp->expq);
	skb_queue_head_init(&sp->oooq);
	skb_queue_head_init(&sp->errq);
	skb_queue_head_init(&sp->rtxq);
	skb_queue_head_init(&sp->dupq);
	skb_queue_head_init(&sp->ackq);
	/* TODO: handle IP options sk->protinfo.af_inet.opt */
	/* initialize hash linkage */
	sp->options = lsp->options;
	/* *INDENT-OFF* */
	/* association defaults */
	sp->max_istr	= lsp->max_istr;
	sp->req_ostr	= lsp->req_ostr;
	sp->max_inits	= lsp->max_inits;
	sp->max_retrans	= lsp->max_retrans;
	sp->ck_life	= lsp->ck_life;
	sp->ck_inc	= lsp->ck_inc;
	sp->hmac	= lsp->hmac;
	sp->cksum	= lsp->cksum;
	sp->throttle	= lsp->throttle;
	sp->sid		= lsp->sid;
	sp->ppi		= lsp->ppi;
	sp->max_sack	= lsp->max_sack;
	sp->max_burst	= lsp->max_burst;
	sp->pmtu	= ip_rt_min_pmtu;
	sp->amps	= sp->pmtu - sp->ext_header_len - sizeof(struct iphdr) - sizeof(struct sctphdr);
	/* destination defaults */
	sp->rto_ini	= lsp->rto_ini;
	sp->rto_min	= lsp->rto_min;
	sp->rto_max	= lsp->rto_max;
	sp->rtx_path	= lsp->rtx_path;
	sp->hb_itvl	= lsp->hb_itvl;
	/* *INDENT-ON* */
	sp->sackf = SCTP_SACKF_NEW;	/* don't delay first sack */
//      sp->in_flight = 0;
//      sp->retransmits = 0;
	/* Force user locks because both the source port and source addresses are
	   specified in the cookie.  */
	sk->userlocks |= (SOCK_BINDPORT_LOCK | SOCK_BINDADDR_LOCK);
	if (!sctp_saddr_include(sk, ck->saddr, &err) && err)
		goto error;
	if ((err = sctp_alloc_saddrs(sk, ck->sport, saddrs, ck->sanum)))
		goto error;
	__sctp_inherit_port(sk, lsk);
	if (!sctp_daddr_include(sk, ck->daddr, &err) && err)
		goto error;
	if ((err = sctp_alloc_daddrs(sk, ck->dport, daddrs, ck->danum)))
		goto error;
	/* *INDENT-OFF* */
	sp->v_tag	= ck->v_tag;
	sp->p_tag	= ck->p_tag;
	sp->n_istr	= ck->n_istr;
	sp->n_ostr	= ck->n_ostr;
	sp->t_tsn	= ck->v_tag;
	sp->t_ack	= ck->v_tag - 1;
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
	sp->l_fsn	= ck->v_tag - 1;
#endif
#ifdef CONFIG_SCTP_ADD_IP
	sp->l_asn	= ck->v_tag - 1;
#endif
	sp->m_tsn	= ck->v_tag - 1;
	sp->r_ack	= ck->p_tsn - 1;
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
	sp->p_fsn	= ck->p_tsn - 1;
#endif
#ifdef CONFIG_SCTP_ADD_IP
	sp->p_asn	= ck->p_tsn - 1;
#endif
	sp->p_rwnd	= ck->p_rwnd;
	sp->p_rbuf	= ck->p_rwnd;
	sp->a_rwnd	= sk->rcvbuf;
	/* *INDENT-ON* */
	atomic_set(&sk->refcnt, 1);
	if ((err = sctp_conn_hash(sk)))
		goto error;
	if ((err = sctp_update_routes(sk, 1)))
		goto error;
	sctp_change_state(sk, SCTP_ESTABLISHED);
#ifdef INET_REFCNT_DEBUG
	atomic_inc(&inet_sock_nr);
#endif
	/* put on master list */
	spin_lock_bh(&sctp_protolock);
	if ((sk->next = sctp_protolist))
		sk->next->pprev = &sk->next;
	sk->pprev = &sctp_protolist;
	sctp_protolist = sk;
#ifdef CONFIG_SCTP_MODULE
	MOD_INC_USE_COUNT;
#endif
	atomic_inc(&sctp_socket_count);
	spin_unlock_bh(&sctp_protolock);
	printd(("INFO: There are now %d sockets allocated\n",
		atomic_read(&sctp_socket_count)));
	sk->use_write_queue = 0;
	sctp_send_cookie_ack(sk, skb);
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPPASSIVEESTABS
	SCTP_INC_STATS_USER(SctpPassiveEstabs);
#endif
	bh_unlock_sock(sk);
	return sk;
      error:
	sk->userlocks = 0;	/* break bind locks */
	sk->prot->unhash(sk);
	sk_free(sk);
	*errp = err;
	return NULL;
}
STATIC INLINE int sctp_accept_wait(struct sock *sk, long timeo)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	DECLARE_WAITQUEUE(wait, current);
	int err;
	printd(("INFO: Socket %p entering Accept wait queue\n", sk));
	add_wait_queue_exclusive(sk->sleep, &wait);
	for (;;) {
		set_current_state(TASK_INTERRUPTIBLE);
		release_sock(sk);
		if (skb_queue_empty(&sp->accq))
			timeo = schedule_timeout(timeo);
		lock_sock(sk);
		err = 0;
		if (!skb_queue_empty(&sp->accq))
			break;
		err = -EINVAL;
		if (sk->state != SCTP_LISTEN)
			break;
		err = sock_intr_errno(timeo);
		if (signal_pending(current))
			break;
		err = -EAGAIN;
		if (!timeo)
			break;
	}
	set_current_state(TASK_RUNNING);
	remove_wait_queue(sk->sleep, &wait);
	return err;
}
STATIC struct sock *sctp_accept(struct sock *sk, int flags, int *errp)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sk_buff *skb;
	int err = 0;
	ptrace(("Accepting on listening socket %p\n", sk));
	lock_sock(sk);
	err = -EINVAL;
	if (sk->state != SCTP_LISTEN)
		goto out;
	if (skb_queue_empty(&sp->accq)) {
		long timeo = sock_rcvtimeo(sk, flags & O_NONBLOCK);
		err = -EAGAIN;
		if (!timeo)
			goto out;
		err = sctp_accept_wait(sk, timeo);
		if (err)
			goto out;
	}
	if ((skb = skb_peek(&sp->accq))) {
		struct sock *newsk;
		if (!(newsk = sctp_conn_res(sk, skb, &err)))
			goto out;
		skb_unlink(skb);
		kfree_skb(skb);
		sk->ack_backlog--;
		release_sock(sk);
		printd(("INFO: Socket %p accepted on socket %p\n", newsk, sk));
		return (newsk);
	}
      out:
	release_sock(sk);
	ptrace(("ERROR: couldn't accept on socket %p, error %d\n", sk, err));
	*errp = err;
	return NULL;
}

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP IOCTL
 *
 *  --------------------------------------------------------------------------
 */
STATIC int sctp_ioctl(struct sock *sk, int cmd, unsigned long arg)
{
	int answ = 0;
	struct sctp_opt *sp = SCTP_PROT(sk);
	switch (cmd) {
	case SIOCINQ:
		/* The amount of normal data waiting to be read in the receive
		   queue.  */
		if (sk->state == SCTP_LISTEN)
			return (-EINVAL);
		answ = sp->rmem_queued;
		break;
	case SIOCATMARK:
		/* Returns true when all urgent data has been received by the user
		   program.  */
		if (sk->state == SCTP_LISTEN)
			return (-EINVAL);
		answ = !skb_queue_empty(&sp->expq);
		break;
	case SIOCOUTQ:
		/* (Properly) returns the amount of unsent data in the send queue.  */
		if (sk->state == SCTP_LISTEN)
			return (-EINVAL);
		answ = sk->wmem_queued;
		break;
	default:
		return (-ENOIOCTLCMD);
	}
	return put_user(answ, (int *) arg);
}

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP SHUTDOWN
 *
 *  --------------------------------------------------------------------------
 *  We can shutdown the sending side of an SCTP association just by marking
 *  setting shutdown and the socket code will keep the application from
 *  sending on the socket.  The receive can only be shutdown if both sides are
 *  shut down for SCTP (unlike TCP's half-close).
 */
STATIC void sctp_shutdown(struct sock *sk, int how)
{
	sk->shutdown |= how;
	if (!(how & SEND_SHUTDOWN))
		return;
	/* Do an orderly shutdown if possible.  */
	lock_sock(sk);
	switch (sk->state) {
	case SCTP_ESTABLISHED:
		sctp_change_state(sk, SCTP_SHUTDOWN_PENDING);
	case SCTP_SHUTDOWN_PENDING:
		if (atomic_read(&sk->wmem_alloc) <= 0)
			sctp_send_shutdown(sk);
		break;
	case SCTP_SHUTDOWN_RECEIVED:
		sctp_change_state(sk, SCTP_SHUTDOWN_RECVWAIT);
	case SCTP_SHUTDOWN_RECVWAIT:
		if (atomic_read(&sk->wmem_alloc) <= 0)
			sctp_send_shutdown_ack(sk);
		break;
	default:
		rare();
		ptrace(("sk->state = %d\n", sk->state));
		break;
	}
	release_sock(sk);
	return;
}

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP SETSOCKOPT
 *
 *  --------------------------------------------------------------------------
 */
#ifdef CONFIG_SCTP_ADD_IP
STATIC int sctp_add_ip(struct sock *sk, struct sockaddr_in *addr)
{
	int err = 0;
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_saddr *ss;
	if ((1 << sk->state) & (SCTPF_DISCONNECTED)) {
		todo(("TODO: perform an addition to the bind\n"));
		return -ENOTCONN;
	} else if (sk->state == SCTP_ESTABLISHED) {
		if (sp->sanum == 0)
			return -EINVAL;
		if ((ss = sctp_find_saddr(sk, addr->sin_addr.s_addr)))
			return (0);	/* silent success */
		if (!(sp->p_caps & SCTP_CAPS_ADD_IP)
		    || !(sp->l_caps & SCTP_CAPS_ADD_IP))
			return -EOPNOTSUPP;
		if (!(ss = __sctp_saddr_alloc(sk, addr->sin_addr.s_addr, &err)))
			return err;
		ss->flags |= SCTP_SRCEF_ADD_REQUEST;
		sp->flags |= SCTP_SACKF_ASC;
		sctp_transmit_wakeup(sk);
		return (0);
	} else
		return -EPROTO;
}
STATIC int sctp_del_ip(struct sock *sk, struct sockaddr_in *addr)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_saddr *ss;
	if ((1 << sk->state) & (SCTPF_DISCONNECTED)) {
		todo(("TODO: perform an removal from the bind\n"));
		return -ENOTCONN;
	} else if (sk->state == SCTP_ESTABLISHED) {
		if (sp->sanum < 2)
			return -EINVAL;
		if (!(ss = sctp_find_saddr(sk, addr->sin_addr.s_addr)))
			return (0);	/* silent success */
		ss->flags |= SCTP_SRCEF_DEL_REQUEST;
		if (!(sp->p_caps & SCTP_CAPS_ADD_IP)
		    || !(sp->l_caps & SCTP_CAPS_ADD_IP))
			return -EOPNOTSUPP;
		sp->flags |= SCTP_SACKF_ASC;
		sctp_transmit_wakeup(sk);
		return (0);
	} else
		return -EPROTO;
}
STATIC int sctp_set_ip(struct sock *sk, struct sockaddr_in *addr)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_saddr *ss;
	if (sk->state == SCTP_ESTABLISHED) {
		if (sp->sanum == 0)
			return -EINVAL;
		if (!(ss = sctp_find_saddr(sk, addr->sin_addr.s_addr)))
			return -EINVAL;
		ss->flags |= SCTP_SRCEF_SET_REQUEST;
		if (!(sp->p_caps & SCTP_CAPS_SET_IP)
		    || !(sp->l_caps & SCTP_CAPS_SET_IP))
			return -EOPNOTSUPP;
		sp->flags |= SCTP_SACKF_ASC;
		sctp_transmit_wakeup(sk);
		return (0);
	} else
		return -EINVAL;
}
#endif

#define MAX_WINDOW (0x00010000 - sizeof(struct sctp_data))
STATIC int sctp_setsockopt(struct sock *sk, int level, int optname,
			   char *optval, int optlen)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	int val;
	if (level != SOL_SCTP)
		return ip_setsockopt(sk, level, optname, optval, optlen);
	if (optlen < sizeof(int))
		return -EINVAL;
	if (get_user(val, (int *) optval))
		return -EFAULT;
	switch (optname) {
	case SCTP_MAXSEG:
		if (val < 1 || val > MAX_WINDOW)
			return -EINVAL;
		sp->user_amps = val;
		return 0;
	case SCTP_NODELAY:
		if (sp->nonagle == 2)
			return -EINVAL;
		sp->nonagle = (val == 0) ? 0 : 1;
		return 0;
	case SCTP_CORK:
		if (sp->nonagle == 1)
			return -EINVAL;
		if (val != 0)
			sp->nonagle = 2;
		else {
			sp->nonagle = 0;
			lock_sock(sk);
			release_sock(sk);
		}
		return 0;
	case SCTP_RECVSID:
		if (val)
			sp->cmsg_flags |= SCTP_CMSGF_RECVSID;
		else
			sp->cmsg_flags &= ~SCTP_CMSGF_RECVSID;
		return 0;
	case SCTP_RECVPPI:
		if (val)
			sp->cmsg_flags |= SCTP_CMSGF_RECVPPI;
		else
			sp->cmsg_flags &= ~SCTP_CMSGF_RECVPPI;
		return 0;
	case SCTP_RECVSSN:
		if (val)
			sp->cmsg_flags |= SCTP_CMSGF_RECVSSN;
		else
			sp->cmsg_flags &= ~SCTP_CMSGF_RECVSSN;
		return 0;
	case SCTP_RECVTSN:
		if (val)
			sp->cmsg_flags |= SCTP_CMSGF_RECVTSN;
		else
			sp->cmsg_flags &= ~SCTP_CMSGF_RECVTSN;
		return 0;
	case SCTP_SID:
		if (sk->state != SCTP_ESTABLISHED)
			return -ENOTCONN;
		if (val < 0 || val >= sp->n_ostr)
			return -EINVAL;
		sp->sid = val;
		return 0;
	case SCTP_PPI:
		sp->ppi = val;
		return 0;
	case SCTP_HB:
	{
		struct sctp_hbitvl hb;
		struct sctp_daddr *sd;
		if (optlen < sizeof(hb))
			return -EINVAL;
		if (copy_from_user(&hb, optval, sizeof(hb)))
			return -EFAULT;
		if (hb.hb_itvl < 10)
			return -EINVAL;
		lock_sock(sk);
		if ((sd = sctp_find_daddr(sk, hb.dest.sin_addr.s_addr))) {
			sd->hb_itvl = hb.hb_itvl * HZ / 1000;
			sd->hb_act = hb.hb_act ? 1 : 0;
			release_sock(sk);
			return 0;
		}
		release_sock(sk);
		return -EINVAL;
	}
	case SCTP_RTO:
	{
		struct sctp_rtoval rv;
		struct sctp_daddr *sd;
		if (optlen < sizeof(rv))
			return -EINVAL;
		if (copy_from_user(&rv, optval, sizeof(rv)))
			return -EFAULT;
		if (rv.rto_initial < rv.rto_min || rv.rto_max < rv.rto_min ||
		    rv.rto_max < rv.rto_initial)
			return -EINVAL;
		lock_sock(sk);
		if ((sd = sctp_find_daddr(sk, rv.dest.sin_addr.s_addr))) {
			sd->rto_ini = rv.rto_initial * HZ / 1000;
			sd->rto_min = rv.rto_min * HZ / 1000;
			sd->rto_max = rv.rto_max * HZ / 1000;
			sd->max_retrans = rv.max_retrans;
			return 0;
		}
		release_sock(sk);
		return -EINVAL;
	}
	case SCTP_COOKIE_LIFE:
	{
		if (val < 0)
			return -EINVAL;
		sp->ck_life = val * HZ / 1000;
		return (0);
	}
	case SCTP_SACK_DELAY:
	{
		if (val < 0)
			return -EINVAL;
		sp->max_sack = val * HZ / 1000;
		return (0);
	}
	case SCTP_PATH_MAX_RETRANS:
	{
		if (val < 0)
			return -EINVAL;
		sp->rtx_path = val;
		return (0);
	}
	case SCTP_ASSOC_MAX_RETRANS:
	{
		if (val < 0)
			return -EINVAL;
		sp->max_retrans = val;
		return (0);
	}
	case SCTP_MAX_INIT_RETRIES:
	{
		if (val < 0)
			return -EINVAL;
		sp->max_inits = val;
		return (0);
	}
	case SCTP_MAX_BURST:
	{
		if (val <= 0)
			return -EINVAL;
		sp->max_burst = val;
		return (0);
	}
	case SCTP_HEARTBEAT_ITVL:
	{
		if (val < 0)
			return -EINVAL;
		sp->hb_itvl = val * HZ / 1000;
#ifdef CONFIG_SCTP_THROTTLE_HEARTBEATS
		sp->hb_tint = (val >> 1) + 1;
#endif
		return (0);
	}
	case SCTP_RTO_INITIAL:
	{
		if ((val > sp->rto_max * 1000 / HZ)
		    || (val < sp->rto_min * 1000 / HZ))
			return -EINVAL;
		sp->rto_ini = val * HZ / 1000;
		return (0);
	}
	case SCTP_RTO_MIN:
	{
		if ((val < 0) || (val > sp->rto_max * 1000 / HZ)
		    || (val > sp->rto_ini * 1000 / HZ))
			return -EINVAL;
		sp->rto_min = val * HZ / 1000;
		return (0);
	}
	case SCTP_RTO_MAX:
	{
		if ((val < 0) || (val < sp->rto_min * 1000 / HZ)
		    || (val < sp->rto_ini * 1000 / HZ))
			return -EINVAL;
		sp->rto_max = val * HZ / 1000;
		return (0);
	}
	case SCTP_OSTREAMS:
	{
		if (val < 1 || val > 0x0000ffff)
			return -EINVAL;
		sp->req_ostr = val;
		return (0);
	}
	case SCTP_ISTREAMS:
	{
		if (val < 1 || val > 0x0000ffff)
			return -EINVAL;
		sp->max_istr = val;
		return (0);
	}
	case SCTP_COOKIE_INC:
	{
		if (val < 0)
			return -EINVAL;
		sp->ck_inc = val * HZ / 1000;
		return (0);
	}
	case SCTP_THROTTLE_ITVL:
	{
		if (val < 0)
			return -EINVAL;
		sp->throttle = val * HZ / 1000;
		return (0);
	}
	case SCTP_MAC_TYPE:
	{
		switch (val) {
		case SCTP_HMAC_NONE:
#ifdef CONFIG_SCTP_HMAC_SHA1
		case SCTP_HMAC_SHA_1:
#endif
#ifdef CONFIG_SCTP_HMAC_MD5
		case SCTP_HMAC_MD5:
#endif
			sp->hmac = val;
			return (0);
		default:
			return -EINVAL;
		}
	}
	case SCTP_CKSUM_TYPE:
	{
		switch (val) {
#if defined(CONFIG_SCTP_ADLER_32)||!defined(CONFIG_SCTP_CRC_32C)
		case SCTP_CSUM_ADLER32:
#endif
#ifdef CONFIG_SCTP_CRC_32C
		case SCTP_CSUM_CRC32C:
#endif
			sp->cksum = val;
			return (0);
		default:
			return -EINVAL;
		}
	}
	case SCTP_DEBUG_OPTIONS:
	{
		sp->options = val;
		return (0);
	}
#ifdef CONFIG_SCTP_ECN
	case SCTP_ECN:
	{
		if (val) {
			if (!sysctl_sctp_ecn)
				return -EINVAL;
			sp->l_caps |= SCTP_CAPS_ECN;
		} else
			sp->l_caps &= ~SCTP_CAPS_ECN;
		return (0);
	}
#endif
#if defined(CONFIG_SCTP_ADD_IP) || defined(CONFIG_SCTP_ADAPTATION_LAYER_INFO)
	case SCTP_ALI:
	{
		sp->l_ali = val;
		if (val)
			sp->l_caps |= SCTP_CAPS_ALI;
		else
			sp->l_caps &= ~SCTP_CAPS_ALI;
		return (0);
	}
#endif
#ifdef CONFIG_SCTP_ADD_IP
	case SCTP_ADD:
	{
		if (val)
			sp->l_caps |= SCTP_CAPS_ADD_IP;
		else
			sp->l_caps &= ~SCTP_CAPS_ADD_IP;
		return (0);
	}
	case SCTP_SET:
	{
		if (val)
			sp->l_caps |= SCTP_CAPS_SET_IP;
		else
			sp->l_caps &= ~SCTP_CAPS_SET_IP;
		return (0);
	}
	case SCTP_ADD_IP:
	{
		int rtn;
		struct sockaddr_in addr;
		if (optlen < sizeof(addr))
			return -EINVAL;
		if (copy_from_user(&addr, optval, sizeof(addr)))
			return -EFAULT;
		if (addr.sin_family != AF_INET)
			return -EINVAL;
		if (!sk->sport || addr.sin_port != sk->sport)
			return -EINVAL;
		lock_sock(sk);
		rtn = sctp_add_ip(sk, &addr);
		release_sock(sk);
		return rtn;
	}
	case SCTP_DEL_IP:
	{
		int rtn;
		struct sockaddr_in addr;
		if (optlen < sizeof(addr))
			return -EINVAL;
		if (copy_from_user(&addr, optval, sizeof(addr)))
			return -EFAULT;
		if (addr.sin_family != AF_INET)
			return -EINVAL;
		if (!sk->sport || addr.sin_port != sk->sport)
			return -EINVAL;
		lock_sock(sk);
		rtn = sctp_del_ip(sk, &addr);
		release_sock(sk);
		return rtn;
	}
	case SCTP_SET_IP:
	{
		int rtn;
		struct sockaddr_in addr;
		if (optlen < sizeof(addr))
			return -EINVAL;
		if (copy_from_user(&addr, optval, sizeof(addr)))
			return -EFAULT;
		if (addr.sin_family != AF_INET)
			return -EINVAL;
		if (!sk->sport || addr.sin_port != sk->sport)
			return -EINVAL;
		lock_sock(sk);
		rtn = sctp_set_ip(sk, &addr);
		release_sock(sk);
		return rtn;
	}
#endif
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
	case SCTP_PR:
	{
		switch (val) {
		case SCTP_PR_NONE:
		case SCTP_PR_PREFERRED:
		case SCTP_PR_REQUIRED:
			sp->prel = val;
			return (0);
		default:
			return (-EINVAL);
		}
	}
#endif
#if defined(CONFIG_SCTP_LIFETIMES) || defined(CONFIG_SCTP_PARTIAL_RELIABILITY)
	case SCTP_LIFETIME:
	{
		sp->life = val * HZ / 1000;
		return (0);
	}
#endif
	case SCTP_DISPOSITION:
	{
		switch (val) {
		case SCTP_DISPOSITION_NONE:
		case SCTP_DISPOSITION_UNSENT:
		case SCTP_DISPOSITION_SENT:
		case SCTP_DISPOSITION_GAP_ACKED:
		case SCTP_DISPOSITION_ACKED:
			sp->disp = val;
			return (0);
		default:
			return (-EINVAL);
		}
	}
	default:
		return -ENOPROTOOPT;
	}
}

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP GETSOCKOPT
 *
 *  --------------------------------------------------------------------------
 */
STATIC int sctp_getsockopt(struct sock *sk, int level, int optname,
			   char *optval, int *optlen)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	int val, len;
	if (level != SOL_SCTP)
		return ip_getsockopt(sk, level, optname, optval, optlen);
	if (get_user(len, optlen))
		return -EFAULT;
	len = (len < sizeof(int)) ? len : sizeof(int);
	switch (optname) {
	case SCTP_MAXSEG:
		val = sp->user_amps;
		break;
	case SCTP_NODELAY:
		val = (sp->nonagle == 1);
		break;
	case SCTP_CORK:
		val = (sp->nonagle == 2);
		break;
	case SCTP_RECVSID:
		val = sp->cmsg_flags & SCTP_CMSGF_RECVSID;
		break;
	case SCTP_RECVPPI:
		val = sp->cmsg_flags & SCTP_CMSGF_RECVPPI;
		break;
	case SCTP_RECVSSN:
		val = sp->cmsg_flags & SCTP_CMSGF_RECVSSN;
		break;
	case SCTP_RECVTSN:
		val = sp->cmsg_flags & SCTP_CMSGF_RECVTSN;
		break;
	case SCTP_SID:
		val = sp->sid;
		break;
	case SCTP_PPI:
		val = sp->ppi;
		break;
	case SCTP_HB:
	{
		struct sctp_hbitvl hb;
		struct sctp_daddr *sd;
		if (len < sizeof(hb))
			return -EINVAL;
		if (copy_from_user(&hb, optval, sizeof(hb)))
			return -EFAULT;
		lock_sock(sk);
		if ((sd = sctp_find_daddr(sk, hb.dest.sin_addr.s_addr))) {
			hb.hb_itvl = (sd->hb_itvl * 1000) / HZ;
			hb.hb_act = sd->hb_act;
			release_sock(sk);
			if (copy_to_user((void *) optval, &hb, sizeof(hb)))
				return -EFAULT;
			return 0;
		}
		release_sock(sk);
		return -EINVAL;
	}
	case SCTP_RTO:
	{
		struct sctp_rtoval rv;
		struct sctp_daddr *sd;
		if (len < sizeof(rv))
			return -EINVAL;
		if (copy_from_user(&rv, optval, sizeof(rv)))
			return -EFAULT;
		lock_sock(sk);
		if ((sd = sctp_find_daddr(sk, rv.dest.sin_addr.s_addr))) {
			rv.rto_initial = (sd->rto_ini * 1000 / HZ);
			rv.rto_min = (sd->rto_min * 1000 / HZ);
			rv.rto_max = (sd->rto_max * 1000 / HZ);
			rv.max_retrans = sd->max_retrans;
			release_sock(sk);
			if (copy_to_user((void *) optval, &rv, sizeof(rv)))
				return -EFAULT;
			return 0;
		}
		release_sock(sk);
		return -EINVAL;
	}
	case SCTP_COOKIE_LIFE:
	{
		val = sp->ck_life * 1000 / HZ;
		break;
	}
	case SCTP_SACK_DELAY:
	{
		val = sp->max_sack * 1000 / HZ;
		break;
	}
	case SCTP_PATH_MAX_RETRANS:
	{
		val = sp->rtx_path;
		break;
	}
	case SCTP_ASSOC_MAX_RETRANS:
	{
		val = sp->max_retrans;
		break;
	}
	case SCTP_MAX_INIT_RETRIES:
	{
		val = sp->max_inits;
		break;
	}
	case SCTP_MAX_BURST:
	{
		val = sp->max_burst;
		break;
	}
	case SCTP_HEARTBEAT_ITVL:
	{
		val = sp->hb_itvl * 1000 / HZ;
		break;
	}
	case SCTP_RTO_INITIAL:
	{
		val = sp->rto_ini * 1000 / HZ;
		break;
	}
	case SCTP_RTO_MIN:
	{
		val = sp->rto_min * 1000 / HZ;
		break;
	}
	case SCTP_RTO_MAX:
	{
		val = sp->rto_max * 1000 / HZ;
		break;
	}
	case SCTP_OSTREAMS:
	{
		val = sp->n_ostr ? sp->n_ostr : sp->req_ostr;
		break;
	}
	case SCTP_ISTREAMS:
	{
		val = sp->n_istr ? sp->n_istr : sp->max_istr;
		break;
	}
	case SCTP_COOKIE_INC:
	{
		val = sp->ck_inc * 1000 / HZ;
		break;
	}
	case SCTP_THROTTLE_ITVL:
	{
		val = sp->throttle * 1000 / HZ;
		break;
	}
	case SCTP_MAC_TYPE:
	{
		val = sp->hmac;
		break;
	}
	case SCTP_CKSUM_TYPE:
	{
		val = sp->cksum;
		break;
	}
	case SCTP_DEBUG_OPTIONS:
	{
		val = sp->options;
		break;
	}
#ifdef CONFIG_SCTP_ECN
	case SCTP_ECN:
	{
		val = (sp->p_caps & SCTP_CAPS_ECN) ? 1 : 0;
		break;
	}
#endif
#if defined(CONFIG_SCTP_ADD_IP) || defined(CONFIG_SCTP_ADAPTATION_LAYER_INFO)
	case SCTP_ALI:
	{
		val = (sp->p_caps & SCTP_CAPS_ALI) ? sp->p_ali : 0;
		break;
	}
#endif
#ifdef CONFIG_SCTP_ADD_IP
	case SCTP_ADD:
	{
		val = (sp->p_caps & SCTP_CAPS_ADD_IP) ? 1 : 0;
		break;
	}
	case SCTP_SET:
	{
		val = (sp->p_caps & SCTP_CAPS_SET_IP) ? 1 : 0;
		break;
	}
#endif
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
	case SCTP_PR:
	{
		val = (sp->p_caps & SCTP_CAPS_PR) ? 1 : 0;
		break;
	}
#endif
#if defined(CONFIG_SCTP_LIFETIMES) || defined(CONFIG_SCTP_PARTIAL_RELIABILITY)
	case SCTP_LIFETIME:
	{
		val = sp->life;
		break;
	}
#endif
	case SCTP_DISPOSITION:
	{
		val = sp->disp;
		break;
	}
	default:
		return -ENOPROTOOPT;
	}
	if (put_user(len, optlen))
		return -EFAULT;
	if (copy_to_user(optval, &val, len))
		return -EFAULT;
	return 0;
}

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP SENDMSG
 *
 *  --------------------------------------------------------------------------
 *
 *  This copies from a user buffer into a socket and starts transmission.  The
 *  buffer is copied and partial checksummed at the same time for maximum speed.
 *  Only a zero-copy sk_buff system could do better.  I hear one is coming.
 *
 *  SCTP permits TCP compatible SOCK_STREAM operation with the appropriate
 *  configuration options set.  Otherwise, SCTP uses SOCK_SEQPACKET operation.
 *
 */
STATIC int sctp_cmsg_send(struct msghdr *msg, int *sid, int *ppi,
			  unsigned long *life, struct ipcm_cookie *ipc)
{
	/* 
	 *  IMPLEMENTATION NOTE:-  I have also set sctp_sendmsg to accept
	 *  ancillary data which can be used to select the stream identifier
	 *  and protocol payload identifier for this message.  Both default to
	 *  that which is set for the socket with setsockopt, or to 0 if not
	 *  set.
	 */
	if (msg->msg_controllen) {
		int ipcmsg = 0;
		int err;
		struct cmsghdr *cmsg;
		for (cmsg = CMSG_FIRSTHDR(msg); cmsg; cmsg = CMSG_NXTHDR(msg, cmsg)) {
			if (cmsg->cmsg_len < sizeof(struct cmsghdr)
			    || (unsigned
				long) (((char *) cmsg - (char *) msg->msg_control)
				       + cmsg->cmsg_len) > msg->msg_controllen)
				return -EINVAL;
			if (cmsg->cmsg_level == SOL_IP)
				ipcmsg = 1;
			if (cmsg->cmsg_level != SOL_SCTP)
				continue;
			switch (cmsg->cmsg_type) {
			case SCTP_SID:
				if (cmsg->cmsg_len != CMSG_LEN(sizeof(int)))
					return -EINVAL;
				*sid = *((int *) CMSG_DATA(cmsg));
				break;
			case SCTP_PPI:
				if (cmsg->cmsg_len != CMSG_LEN(sizeof(int)))
					return -EINVAL;
				*ppi = *((int *) CMSG_DATA(cmsg));
				break;
#if defined(CONFIG_SCTP_LIFETIMES) || defined(CONFIG_SCTP_PARTIAL_RELIABILITY)
			case SCTP_LIFETIME:
				if (cmsg->cmsg_len != CMSG_LEN(sizeof(int)))
					return -EINVAL;
				*life = *((int *) CMSG_DATA(cmsg));
				break;
#endif
			default:
				return -EINVAL;
			}
		}
		/* 
		 *  IMPLEMENTATION NOTE:-  I also support the UDP style
		 *  ancillary data here for ip cmsg, but only IP_PKTINFO and
		 *  not IP_RETOPTS.
		 *
		 *  Gee, yes! You can even specify the interface that you wish
		 *  the data to be sent on.  This allows the user to specify
		 *  at least the transmission policy for SCTP.  Note that this
		 *  SCTP implementation might not bundle DATA chunks that are
		 *  thus directed.
		 */
		if (ipcmsg) {
			if ((err = ip_cmsg_send(msg, ipc)))
				return err;
			if (ipc->opt) {
				kfree(ipc->opt);
				return -EINVAL;
			}
		}
	}
	return (0);
}

/*
 *  WAIT FOR CONNECT
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This waitq waits for a socket to become connected.  This function permits
 *  sending on a socket immediately after the connect() call but waits until
 *  the socket enters the established state before
 */
STATIC int sctp_wait_for_connect(struct sock *sk, long *timeo)
{
	DECLARE_WAITQUEUE(wait, current);
	int err = 0;
	printd(("INFO: Socket %p entering Connect wait queue\n", sk));
	add_wait_queue_exclusive(sk->sleep, &wait);
	for (;;) {
		set_current_state(TASK_INTERRUPTIBLE);
		release_sock(sk);
		if ((1 << sk->state) & ~(SCTPF_ESTABLISHED | SCTPF_COOKIE_ECHOED))
			*timeo = schedule_timeout(*timeo);
		lock_sock(sk);
		/* non-restartable states */
		if ((1 << sk->state) & (SCTPF_ESTABLISHED | SCTPF_COOKIE_ECHOED))
			break;
		if ((1 << sk->state) &
		    (SCTPF_DISCONNECTED | SCTPF_SHUTDOWN_ACK_SENT))
			goto epipe;
		if (signal_pending(current))
			goto eintr;
		if (!*timeo)
			goto eagain;
	}
      out:
	set_current_state(TASK_RUNNING);
	remove_wait_queue(sk->sleep, &wait);
	return err;
      epipe:
	err = -EPIPE;
	goto out;
      eagain:
	err = -EAGAIN;
	goto out;
      eintr:
	err = sock_intr_errno(*timeo);
	goto out;
}

/*
 *  WAIT FOR WRITE MEMORY
 *  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 *  This waitq waits for a socket to have available memory to which to write
 *  new data.  This is congestion flow control.
 */
STATIC int sctp_wait_for_wmem(struct sock *sk, long *timeo)
{
	DECLARE_WAITQUEUE(wait, current);
	int err = 0;
	printd(("INFO: Socket %p entering Write wait queue\n", sk));
	add_wait_queue(sk->sleep, &wait);
	for (;;) {
		set_bit(SOCK_ASYNC_NOSPACE, &sk->socket->flags);
		set_current_state(TASK_INTERRUPTIBLE);
		if (sk->err || (sk->shutdown & SEND_SHUTDOWN))
			goto epipe;
		if (!*timeo)
			goto eagain;
		if (signal_pending(current))
			goto eintr;
		clear_bit(SOCK_ASYNC_NOSPACE, &sk->socket->flags);
		if (atomic_read(&sk->wmem_alloc) < sk->sndbuf) {
			clear_bit(SOCK_NOSPACE, &sk->socket->flags);
			break;
		}
		set_bit(SOCK_NOSPACE, &sk->socket->flags);
		release_sock(sk);
		if (atomic_read(&sk->wmem_alloc) >= sk->sndbuf)
			*timeo = schedule_timeout(*timeo);
		lock_sock(sk);
	}
      out:
	set_current_state(TASK_RUNNING);
	remove_wait_queue(sk->sleep, &wait);
	return err;
      epipe:
	err = -EPIPE;
	goto out;
      eagain:
	err = -EAGAIN;
	goto out;
      eintr:
	err = sock_intr_errno(*timeo);
	goto out;
}

/*
 *  SCTP SENDMSG
 *  -----------------------------------
 *  Note: we could probably accept one packet in the opening state so that we
 *  can add it to the outbound COOKIE-ECHO message.  That would be cool.
 */
STATIC int sctp_sendmsg(struct sock *sk, struct msghdr *msg, int len)
{
	struct iovec *iov;
	struct ipcm_cookie ipc;
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_daddr *daddr = NULL;
	struct sctp_strm *st = NULL;
	struct sk_buff *skb = NULL, **head;
	struct sk_buff_head *sndq;
	uint32_t saddr = 0;
	int flags, oob, err = 0, bundle, iovlen, copied = 0, sid = 0, ppi = 0, oif = 0,
	    *more, *ppip, stream = 0;
	long timeo;
	unsigned long life = sp->life;
	flags = msg->msg_flags;
	if ((flags & ~(MSG_OOB | MSG_DONTROUTE | MSG_DONTWAIT | MSG_NOSIGNAL |
		       MSG_EOR | MSG_MORE | MSG_CONFIRM | MSG_EOF)))
		return -EINVAL;
	/* TODO: need to support MSG_PROBE per documentation.  MSG_PROBE should
	   place data in a heartbeat. */
#ifdef CONFIG_SCTP_UDP_COMPATIBLE
	if ((!(flags & MSG_EOR) || (flags & MSG_MORE)) && sk->type == SOCK_RDM)
		return -EINVAL;	/* more never used for SOCK_RDM */
#endif
	(void) stream;
#ifdef CONFIG_SCTP_TCP_COMPATIBLE
	stream = (sk->type == SOCK_STREAM);
	if (stream) {
		flags &= ~MSG_EOR;
		flags |= MSG_MORE;
	}
#endif
	if ((1 << sk->state) & (SCTPF_DISCONNECTED))
		return -ENOTCONN;
/*
 *  IMPLEMENTATION NOTE:- I have set sctp_sendmsg() to handle having addresses
 *  provided in the same fashion as udp_sendmsg() as long as the address is
 *  one of the destination addresses available to the association.  This
 *  permits the user to specify the destination address of a specific data
 *  message.
 */
	if (msg->msg_name) {
		struct sockaddr_in *addr = (struct sockaddr_in *) msg->msg_name;
#ifdef CONFIG_SCTP_TCP_COMPATIBLE
		if (sk->type == SOCK_STREAM)
			return -EINVAL;
#endif
		if (msg->msg_namelen < sizeof(*addr))
			return -EINVAL;
		if (addr->sin_family && addr->sin_family != AF_INET)
			return -EINVAL;
		if (addr->sin_port && addr->sin_port != sk->dport)
			return -EINVAL;
		if (addr->sin_addr.s_addr &&
		    !(daddr = sctp_find_daddr(sk, addr->sin_addr.s_addr)))
			return -EINVAL;
	}
	lock_sock(sk);
	timeo = sock_sndtimeo(sk, flags & MSG_DONTWAIT);
	/* Wait for connection (or disconnection) to finish. */
	if ((1 << sk->state) & ~(SCTPF_ESTABLISHED | SCTPF_COOKIE_ECHOED))
		if ((err = sctp_wait_for_connect(sk, &timeo)))
			goto out;
	/* This should be in poll */
	clear_bit(SOCK_ASYNC_NOSPACE, &sk->socket->flags);	/* clear SIGIO XXX */
/*
 *  IMPLEMENTATION NOTE:-  The user can specify the destination address
 *  (within those available to the association), the source address (within
 *  those available to the association), and/or the interface upon which to
 *  send the message.  However, for SOCK_STREAM sockets it will then not be
 *  possible to concatenate these bytes into other DATA chunks, and for
 *  SOCK_SEQPACKET and SOCK_RDM sockets it will not be possible to bundle
 *  these DATA chunks with DATA chunks for other destinations.
 */
	/* Handle SCTP and IP ancillary data */
	sid = sp->sid;		/* start with association default */
	ppi = sp->ppi;		/* start with association default */
	memset(&ipc, '0', sizeof(ipc));
	if ((err = sctp_cmsg_send(msg, &sid, &ppi, &life, &ipc)))
		goto out;
	err = -EINVAL;
#ifdef CONFIG_SCTP_TCP_COMPATIBLE
	if (sid != 0 && stream)
		goto out;
#endif
	if (sid >= sp->n_ostr)
		goto out;
	if (ipc.addr && sctp_find_saddr(sk, ipc.addr))
		saddr = ipc.addr;
	if (ipc.oif)
		oif = ipc.oif;
	bundle = (!daddr && !saddr);	/* && !oif); later */
#ifdef CONFIG_SCTP_TCP_COMPATIBLE
	if (!bundle && stream)
		goto out;
#endif
#ifdef CONFIG_SCTP_UDP_COMPATIBLE
	if (sk->type == SOCK_RDM)
		flags |= MSG_OOB;
#endif
	if (!(st = sctp_ostrm_find(sk, sid, &err)))
		goto out;
	if ((oob = (flags & MSG_OOB))) {
		sndq = &sp->urgq;
		more = &st->x.more;
		head = &st->x.head;
		ppip = &st->x.ppi;
	} else {
		sndq = &sk->sndq;
		more = &st->n.more;
		head = &st->n.head;
		ppip = &st->n.ppi;
	}
	if (!bundle && (*more & SCTP_STRMF_MORE))
		goto out;
	/* Commence sending */
	iovlen = msg->msg_iovlen;
	iov = msg->msg_iov;
	copied = 0;
	err = -EPIPE;
	if (sk->err || (sk->shutdown & SEND_SHUTDOWN))
		goto out;
	while (--iovlen >= 0) {
		int seglen = iov->iov_len;
		unsigned char *from = iov->iov_base;
		iov++;
		while (seglen > 0) {
			int copy;
			/* Find out how much data can fit in a DATA chunk. We never
			   fill data chunks to bigger than would fit one to a path
			   MTU. We call this the AMPS (Association Maximum Payload
			   Size). */
			int amps = sp->amps;
			int dmps = amps - sizeof(struct sctp_data);
			if (!(skb = *head) || (copy = amps - skb->len) <= 0 ||
			    !bundle || !(*more & SCTP_STRMF_MORE)) {
				struct sctp_skb_cb *cb;
				struct sctp_data *m;
			      new_chunk:
				if (!(copy = len))
					goto out;
				/* preallocate more if more data coming */
				if ((flags & MSG_MORE) && copy < dmps)
					copy = dmps;
				if (copy > dmps)
					copy = dmps;
				if (!(skb = sctp_wmalloc(sk, copy + sizeof(*m), 0,
							 sk->allocation)))
					goto wait_for_sndbuf;
				m = (struct sctp_data *) skb_put(skb, sizeof(*m));
				cb = SCTP_SKB_CB(skb);
				cb->skb = skb;
				cb->dlen = 0;
				cb->st = st;
				cb->expires = 0;
				cb->when = jiffies;	/* reset when transmitted */
				cb->daddr = daddr;	/* reset when transmitted */
//                              cb->dif = oif;          /* TODO */
				cb->st = st;
				cb->tsn = 0;	/* assigned later */
				cb->sid = sid;
				cb->ssn = oob ? 0 : (st->ssn + 1) & 0xffff;
				cb->ppi = (*more & SCTP_STRMF_MORE) ? *ppip : ppi;
				cb->flags = ((*more & SCTP_STRMF_MORE)
					     ? 0 : SCTPCB_FLAG_FIRST_FRAG)
				    | (!oob ? 0 : SCTPCB_FLAG_URG);
#ifdef CONFIG_SCTP_TCP_COMPATIBLE
				/* ease reassembly at peer */
				if (stream)
					cb->flags |=
					    SCTPCB_FLAG_FIRST_FRAG |
					    SCTPCB_FLAG_LAST_FRAG;
#endif
				if (sp->disp == SCTP_DISPOSITION_ACKED ||
				    flags & MSG_CONFIRM)
					cb->flags |= SCTPCB_FLAG_CONF;
				m->ch.type = SCTP_CTYPE_DATA;
				m->ch.flags = cb->flags & 0x7;
				m->ch.len = htons(skb->len);
				m->tsn = 0;	/* assigned before transmission */
				m->sid = htons(cb->sid);
				m->ssn = htons(cb->ssn);
				m->ppi = htonl(cb->ppi);
				skb_init_cksum(skb);
				/* don't put on queue till it has data in it */
			}
			if (copy > seglen)
				copy = seglen;
			if (skb_tailroom(skb) <= 0 || skb->len >= amps) {
#if defined(CONFIG_SCTP_LIFETIMES) || defined(CONFIG_SCTP_PARTIAL_RELIABILITY)
				struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
				if (life) {
					cb->expires = jiffies + life;
					if (!timer_pending(&sp->timer_life) ||
					    sp->timer_life.expires > cb->expires)
						sctp_mod_timer
						    (&sp->timer_life, life);
				}
#endif
				/* push the head buffer go */
				*head = NULL;
				sk->wmem_queued += PADC(skb->len);
				skb_queue_tail(sndq, skb);
				goto new_chunk;
			}
			/* Continue building packet */
			if (copy > skb_tailroom(skb))
				copy = skb_tailroom(skb);
			if (!(err = skb_add_data(skb, from, copy))) {
				/* touch up chunk header */
				struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
				struct sctp_data *m = (struct sctp_data *) skb->data;
				cb->dlen = skb->len - sizeof(*m);
				m->ch.len = htons(skb->len);
				/* if fresh buffer queue it now */
				if (!skb->list) {
					bundle = 1;
					*head = skb;
					*more |= SCTP_STRMF_MORE;
					*ppip = cb->ppi;
					skb_queue_tail(sndq, skb);
#if defined(CONFIG_SCTP_LIFETIMES) || defined(CONFIG_SCTP_PARTIAL_RELIABILITY)
					if (life) {
						cb->expires = jiffies + life;
						if (!timer_pending(&sp->timer_life)
						    || sp->timer_life.expires >
						    cb->expires)
							sctp_mod_timer
							    (&sp->timer_life, life);
					}
#endif
				}
				from += copy;
				copied += copy;
				seglen -= copy;
				len -= copy;
				if (len <= 0 &&
				    (!(flags & MSG_MORE) || (flags & MSG_EOR))) {
					*head = NULL;
					sk->wmem_queued += PADC(skb->len);
					*more &= ~SCTP_STRMF_MORE;
					*ppip = 0;
					if (!oob)
						st->ssn = cb->ssn;
					cb->flags |= SCTPCB_FLAG_LAST_FRAG;
					m->ch.flags = cb->flags & 0x7;
					if (flags & MSG_EOF &&
					    !(sk->shutdown & SEND_SHUTDOWN)) {
						sk->shutdown |= SEND_SHUTDOWN;
						switch (sk->state) {
						case SCTP_ESTABLISHED:
							sctp_change_state(sk,
									  SCTP_SHUTDOWN_PENDING);
							break;
						case SCTP_SHUTDOWN_RECEIVED:
							sctp_change_state(sk,
									  SCTP_SHUTDOWN_RECVWAIT);
							break;
						}
					}
				} else if (skb->len >= amps) {
					*head = NULL;
					sk->wmem_queued += PADC(skb->len);
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPFRAGUSRMSGS
					SCTP_INC_STATS_USER(SctpFragUsrMsgs);
#endif
				}
				continue;
			} else {
				if (!skb->list)
					kfree_skb(skb);
				goto out;
			}
		      wait_for_sndbuf:
			set_bit(SOCK_NOSPACE, &sk->socket->flags);
			sctp_transmit_wakeup(sk);
			if ((err = sctp_wait_for_wmem(sk, &timeo)))
				goto out;
		}
	}
	if (copied)
		sctp_transmit_wakeup(sk);
	release_sock(sk);
	return copied;
      out:
	if (copied || !err) {
		if (copied)
			sctp_transmit_wakeup(sk);
		release_sock(sk);
		return copied;
	}
	if (err == -EPIPE)
		err = sock_error(sk) ? : -EPIPE;
	if (err == -EPIPE && !(flags & MSG_NOSIGNAL))
		send_sig(SIGPIPE, current, 0);
	release_sock(sk);
	return err;
}

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP RECVMSG
 *
 *  --------------------------------------------------------------------------
 *
 *  Handle the recvmsg(2) call to an SCTP socket.  The special considerations
 *  here are the handling of urgent data (which we interpret as out-of-order
 *  delivery data) and the reception of stream-specific data.
 *
 */
STATIC void sctp_cmsg_recv(struct msghdr *msg, struct sk_buff *skb,
			   unsigned cmsg_flags, unsigned flags)
{
	struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
	if (cmsg_flags & SCTP_CMSGF_RECVSID) {
		int sid = cb->sid;
		put_cmsg(msg, SOL_SCTP, SCTP_SID, sizeof(sid), &sid);
	}
	if (cmsg_flags & SCTP_CMSGF_RECVPPI) {
		int ppi = cb->ppi;
		put_cmsg(msg, SOL_SCTP, SCTP_PPI, sizeof(ppi), &ppi);
	}
	if (cmsg_flags & SCTP_CMSGF_RECVSSN) {
		int ssn = cb->ssn;
		put_cmsg(msg, SOL_SCTP, SCTP_SSN, sizeof(ssn), &ssn);
	}
	if (cmsg_flags & SCTP_CMSGF_RECVTSN) {
		int tsn = cb->tsn;
		put_cmsg(msg, SOL_SCTP, SCTP_TSN, sizeof(tsn), &tsn);
	}
	if (flags & MSG_CONFIRM) {
		int dsp;
		if (!(cb->flags & SCTPCB_FLAG_SENT))
			dsp = SCTP_DISPOSITION_UNSENT;
		else if (!(cb->flags & SCTPCB_FLAG_SACKED)
			 && !(cb->flags & SCTPCB_FLAG_ACK))
			dsp = SCTP_DISPOSITION_SENT;
		else if (!(cb->flags & SCTPCB_FLAG_ACK))
			dsp = SCTP_DISPOSITION_GAP_ACKED;
		else
			dsp = SCTP_DISPOSITION_ACKED;
		put_cmsg(msg, SOL_SCTP, SCTP_DISPOSITION, sizeof(dsp), &dsp);
	}
}
STATIC void sctp_name_recv(struct sock *sk, struct msghdr *msg, struct sk_buff *skb)
{
	struct sockaddr_in *sin = (struct sockaddr_in *) msg->msg_name;
	if (sin) {
		sin->sin_family = AF_INET;
		sin->sin_port = SCTP_SKB_SH(skb)->srce;
		sin->sin_addr.s_addr = skb->nh.iph->saddr;
		memset(&sin->sin_zero, 0, sizeof(sin->sin_zero));
		/* why doesn't udp do this? : */
		msg->msg_namelen = sizeof(*sin);
	}
}

STATIC long sctp_data_wait(struct sock *sk, long timeo)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	DECLARE_WAITQUEUE(wait, current);
	printd(("INFO: Socket %p entering Read wait queue\n", sk));
	add_wait_queue(sk->sleep, &wait);
	set_current_state(TASK_INTERRUPTIBLE);
	set_bit(SOCK_ASYNC_WAITDATA, &sk->socket->flags);
	release_sock(sk);
	/* release clears backlog, so check again */
	if (!sp->rmem_queued)
		timeo = schedule_timeout(timeo);
	lock_sock(sk);
	clear_bit(SOCK_ASYNC_WAITDATA, &sk->socket->flags);
	remove_wait_queue(sk->sleep, &wait);
	set_current_state(TASK_RUNNING);
	return timeo;
}

/*
 *  SCTP RECVMSG
 *  -------------------------------------------------------------------------
 *  This function handles datagram semantics (SOCK_RDM, SOCK_SEQPACKET) as
 *  well as stream semantics (SOCK_STREAM).  SOCK_RDM and SOCK_SEQPACKET
 *  sockets will have MSG_EOR to delimit messages; MSG_TRUNC will be set if a
 *  data chunk cannot be fit into the supplied buffer, but data beyond the
 *  supplied buffer will not be discarded unless it is a SOCK_RDM socket.
 *  SOCK_RDM and SOCK_SEQPACKET can only read one data chunk at a time,
 *  whereas SOCK_STREAM sockets can read as much data as possible and can use
 *  the MSG_WAITALL flag.
 *
 *  SCTP IG 2.20 is not applicable.
 */
STATIC int sctp_recvmsg(struct sock *sk, struct msghdr *msg, int len,
			int noblock, int flags, int *addr_len)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sk_buff_head *head;
	struct sk_buff *skb = NULL;
	int err, target, sid, copied = 0, used, avail;
	long timeo;
	if ((err = sock_error(sk)))
		return (err);
	if ((1 << sk->state) & (SCTPF_DISCONNECTED) && !(flags & MSG_CONFIRM))
		return (-ENOTCONN);
	if (flags & ~(MSG_TRUNC | MSG_OOB | MSG_PEEK |
		      MSG_DONTWAIT | MSG_WAITALL | MSG_NOSIGNAL | MSG_CONFIRM))
		return (-EINVAL);
	lock_sock(sk);
	if (flags & MSG_CONFIRM) {
		timeo = 0;
		if (!(skb = skb_peek(&sp->ackq))) {
			err = -EINVAL;
			goto out;
		}
		msg->msg_flags |= MSG_CONFIRM;
		head = &sp->ackq;
		target = 0;
	} else if (flags & MSG_OOB) {
		timeo = 0;
		if (sk->urginline || !(skb = skb_peek(&sp->expq))) {
			err = -EINVAL;
			goto out;
		}
		msg->msg_flags |= MSG_OOB;
		head = &sp->expq;
		target = 0;
	} else {
		timeo = sock_rcvtimeo(sk, (flags & MSG_DONTWAIT));
		while ((!sk->urginline || !(skb = skb_peek(&sp->expq))) &&
		       !(skb = skb_peek(&sk->rcvq))) {
			if (!timeo) {
				err = -EAGAIN;
				goto out;
			}
			if ((err = sock_error(sk)))
				goto out;
			if (((1 << sk->state) & (SCTPF_DISCONNECTED))
			    || (sk->shutdown & RCV_SHUTDOWN)) {
				err = -ENOTCONN;
				goto out;
			}
			/* unreadable urgent data exists */
			if (!sk->urginline && !skb_queue_empty(&sp->expq))
				goto out;
			if (signal_pending(current)) {
				err = timeo ? sock_intr_errno(timeo) : -EAGAIN;
				goto out;
			}
			timeo = sctp_data_wait(sk, timeo);
		}
		if (SCTP_SKB_CB(skb)->flags & SCTPCB_FLAG_URG)
			msg->msg_flags |= MSG_OOB;
		head = skb->list;
		target = sock_rcvlowat(sk, flags & MSG_WAITALL, len);
	}
	sid = SCTP_SKB_CB(skb)->sid;
	do {
		struct sk_buff *next_skb;
		do {
			struct sctp_skb_cb *cb = SCTP_SKB_CB(skb);
			/* work the list on one stream id only */
			if (cb->sid != sid)
				goto out;
			used = avail = skb->len;
			if (len < used) {
				used = len;
#ifdef CONFIG_SCTP_TCP_COMPATIBLE
				if (sk->type != SOCK_STREAM)
#endif
					msg->msg_flags |= MSG_TRUNC;
				if (!(flags & MSG_TRUNC))
					avail = used;
			}
			printd(("INFO: Reading len %d, used %d, avail %d\n",
				len, used, avail));
			err = skb_copy_datagram_iovec(skb, 0, msg->msg_iov, used);
			if (err)
				goto out;
			copied += avail;
			if (cb->flags & SCTPCB_FLAG_LAST_FRAG) {
				if (used == skb->len)
#ifdef CONFIG_SCTP_TCP_COMPATIBLE
					if (sk->type != SOCK_STREAM) {
#endif
						msg->msg_flags |= MSG_EOR;
						len = used;	/* stop reading */
#ifdef CONFIG_SCTP_TCP_COMPATIBLE
					}
#endif
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPREASMUSRMSGS
				if (!(flags & MSG_CONFIRM))
					if (!(cb)->flags & SCTPCB_FLAG_FIRST_FRAG)
						SCTP_INC_STATS_USER
						    (SctpReasmUsrMsgs);
#endif
			}
#ifdef CONFIG_SCTP_TCP_COMPATIBLE
			if (sk->type != SOCK_STREAM) {
#endif
				if (sk->rcvtstamp || msg->msg_name ||
				    sk->protinfo.af_inet.cmsg_flags ||
				    sp->cmsg_flags || (flags & MSG_CONFIRM)) {
					if (sk->rcvtstamp)
						sock_recv_timestamp(msg, sk, skb);
					if (msg->msg_name)
						sctp_name_recv(sk, msg, skb);
					if (sk->protinfo.af_inet.cmsg_flags)
						ip_cmsg_recv(msg, skb);
					if (sp->cmsg_flags || (flags & MSG_CONFIRM))
						sctp_cmsg_recv(msg, skb,
							       sp->cmsg_flags,
							       flags);
					len = used;	/* stop reading */
				}
#ifdef CONFIG_SCTP_TCP_COMPATIBLE
			}
#endif
			next_skb = skb_next(skb);
			if (!(flags & MSG_PEEK)) {
				if (used < skb->len
#ifdef CONFIG_SCTP_UDP_COMPATIBLE
				    && sk->type != SOCK_RDM
#endif
				    ) {
					/* don't discard on MSG_TRUNC */
					__skb_pull(skb, used);
				} else {
					skb_unlink(skb);
					if (!(flags & MSG_CONFIRM)) {
						sp->rmem_queued -= cb->dlen;
						ensure(sp->rmem_queued >= 0,
						       sp->rmem_queued = 0);
					}
					kfree_skb(skb);
				}
			}
			len -= used;
		} while ((skb = next_skb) && len > 0);
		if (len > 0) {
			if (head == &sp->expq)
				goto out;
			while (!(skb = skb_peek(head))) {
				if (copied >= target && sk->backlog.tail == NULL)
					goto out;
				if (copied) {
					if (sk->err || sk->state == SCTP_CLOSED ||
					    (sk->shutdown & RCV_SHUTDOWN) || !timeo
					    || (flags & MSG_PEEK) ||
					    signal_pending(current) ||
					    !skb_queue_empty(&sp->expq))
						goto out;
				} else {
					if (sk->done)
						goto out;
					if (sk->err) {
						err = sock_error(sk);
						goto out;
					}
					if (((1 << sk->state) &
					     (SCTPF_DISCONNECTED)) ||
					    sk->shutdown & RCV_SHUTDOWN) {
						err = -ENOTCONN;
						goto out;
					}
					if (signal_pending(current)) {
						err = timeo ?
						    sock_intr_errno(timeo) : -EAGAIN;
						goto out;
					}
				}
				if (copied >= target) {
					/* just want to process backlog, not block */
					release_sock(sk);
					lock_sock(sk);
				} else {
					timeo = sctp_data_wait(sk, timeo);
					if (!timeo) {
						err = -EAGAIN;
						goto out;
					}
					if (!skb_queue_empty(&sp->expq))
						goto out;
				}
			}
		}
	} while (len > 0);
      out:
	if (copied) {
		/* SACK if the window opened */
		sctp_transmit_wakeup(sk);
		err = copied;
	}
	release_sock(sk);
	return (err);
}

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP BIND
 *
 *  --------------------------------------------------------------------------
 *
 *  IMPLEMENTATION NOTES:- There are some interesting differences between the
 *  SCTP bind function and the TCP bind function.  Multiple addresses can be
 *  provided (by increasing addr_len) in the (now) array pointed to by struct
 *  sockaddr.  For SCTP when we do a bind, we want to bind to a number of
 *  source addresses.  When binding to INADDR_ANY, we want to use the entire
 *  list of local addresses available on this host; however, we delay
 *  determining that list until either a listen() or connect() is performed on
 *  the socket.
 */
STATIC int sctp_bind(struct sock *sk, struct sockaddr *uaddr, int addr_len)
{
	struct sockaddr_in *usin = (struct sockaddr_in *) uaddr;
	unsigned short snum;
	int addr_num, err;
	uint32_t saddr = 0;
	if ((addr_num = addr_len / sizeof(*usin)) < 1)
		return -EINVAL;
	snum = ntohs(usin[0].sin_port);
	usual(snum);
	if (snum && snum < PROT_SOCK && !capable(CAP_NET_BIND_SERVICE))
		return -EACCES;
	lock_sock(sk);
	if (sk->state != SCTP_CLOSED || sk->num != 0) {
		err = -EINVAL;
		goto out;
	}
	if ((err = sctp_alloc_sock_saddrs(sk, usin[0].sin_port, usin, addr_num)) < 0)
		goto out;
	usual(err >= 0);
	/* set primary */
	if (err > 0) {
		struct sctp_opt *sp = SCTP_PROT(sk);
		if (sp->saddr)
			saddr = sp->saddr->saddr;
		else
			swerr();
	}
	usual(saddr);
	sk->rcv_saddr = sk->saddr = saddr;
	/* also places us in bind hashes */
	err = -EADDRINUSE;
	if (sk->prot->get_port(sk, snum)) {
		__sctp_free_saddrs(sk);
		goto out;
	}
	if (sk->rcv_saddr)
		sk->userlocks |= SOCK_BINDADDR_LOCK;
	if (snum)
		sk->userlocks |= SOCK_BINDPORT_LOCK;
	sk->sport = htons(sk->num);
	__sctp_free_daddrs(sk);
	err = 0;
      out:
	release_sock(sk);
	return err;
}

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP BACKLOG RCV
 *
 *  ---------------------------------------------------------------------------
 *
 *  This is the main receive hook for processing SCTP packets which are either
 *  processed directly or queued in a backlog queue against a socket.  All
 *  these packets have a socket associated with them.  Out of the blue packets
 *  are handled separately in the bottom half.  If the Berkeley Packet Filter
 *  is configured against the socket, we also do packet filtering.
 */
STATIC int sctp_rcv_ootb(struct sk_buff *skb);
STATIC int sctp_backlog_rcv(struct sock *sk, struct sk_buff *skb)
{
#ifdef CONFIG_FILTER
	/* FIXME need a config parameter to select between the two */
#if 0
	if (sk->filter && sk_filter(skb, sk->filter))
#else
	if (sk->filter && sk_filter(sk, skb, 1))
#endif
		goto discard;
#endif
	if (sk->state != SCTP_CLOSED) {
		sctp_recv_msg(sk, skb);
		sctp_transmit_wakeup(sk);
		return (0);
	} else {
		/* the socket may have closed with a backlog */
		struct sock *nsk;
		if ((nsk =
		     sctp_lookup(SCTP_SKB_SH(skb), skb->nh.iph->daddr,
				 skb->nh.iph->saddr))) {
			bh_lock_sock(nsk);
#if defined HAVE_IPSEC_SK_POLICY_EXPORT || defined HAVE_IPSEC_SK_POLICY_ADDR
			if (!ipsec_sk_policy(sk, skb)) {
				bh_unlock_sock(nsk);
				goto discard;
			}
#endif
#if defined HAVE___XFRM_POLICY_CHECK_EXPORT || defined HAVE___XFRM_POLICY_CHECK_ADDR
			if (!xfrm4_policy_check(sk, XFRM_POLICY_IN, skb)) {
				bh_unlock_sock(nsk);
				goto discard;
			}
#endif
			if (sock_locked(nsk))
				sk_add_backlog(nsk, skb);
			else
				sctp_backlog_rcv(nsk, skb);
			bh_unlock_sock(nsk);
			return (0);
		}
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTOFBLUES
		SCTP_INC_STATS_BH(SctpOutOfBlues);
#endif
#ifndef CONFIG_SCTP_DISCARD_OOTB
		return sctp_rcv_ootb(skb);
#endif
	}
      discard:
	kfree_skb(skb);
	return (0);
}

/*
 *  ---------------------------------------------------------------------------
 *
 *  SCTP HASH
 *
 *  ---------------------------------------------------------------------------
 *  This function is never called by the common inet socket code.  Ala TCP, we
 *  hash as required elsewhere.
 */
STATIC void sctp_hash(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	switch (sk->state) {
	case SCTP_ESTABLISHED:
		local_bh_disable();
		__sctp_phash_insert(sp);
		__sctp_thash_insert(sp);
		__sctp_vhash_insert(sp);
		local_bh_enable();
		return;
	case SCTP_LISTEN:
		/* this is the first place we see listening sockets */
		/* want just the listening hash */
		local_bh_disable();
		__sctp_lhash_insert(sp);
		local_bh_enable();
		return;
	case SCTP_COOKIE_WAIT:
		/* want established and tcb but not icmp */
		local_bh_disable();
		__sctp_thash_insert(sp);
		__sctp_vhash_insert(sp);
		local_bh_enable();
		return;
	case SCTP_CLOSED:
		/* 
		 *  If we are CLOSED, then the only hash we can live in is the
		 *  bind hash.  We are added to the bind hash with get_port
		 *  only.  This is so that noone steals our hash bind bucket
		 *  after we assign a port number.  This might be called,
		 *  however, by inet_bind (for historical reasons, I believe).
		 */
		return;
	}
	/* bad news */
	swerr();
}

/*
 *  ---------------------------------------------------------------------------
 *
 *  SCTP UNHASH
 *
 *  ---------------------------------------------------------------------------
 *  Removes the socket from all hashes.  It is unnecessary to put this in the
 *  protocol operations because this is only called from inet_sock_release()
 *  after calling sctp_destroy().  Because we have to drain the receive
 *  queues after unhashing the socket, we call this function directly from
 *  sctp_destroy() before flushing sctp private receive queues.
 */
STATIC void sctp_unhash(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	ptrace(("Unhashing socket sk=%p, state = %d\n", sk, sk->state));
	local_bh_disable();
	if (sk->pprev) {
		if (sp->vprev)
			__sctp_vhash_unhash(sp);
		sp->v_tag = 0;
		if (sp->pprev)
			__sctp_phash_unhash(sp);
		sp->p_tag = 0;
		if (sp->tprev)
			__sctp_thash_unhash(sp);
		if (sp->lprev) {
			__sctp_lhash_unhash(sp);
			sk->max_ack_backlog = 0;
		}
		if (sp->daddr)
			__sctp_free_daddrs(sk);
		if (sp->bprev && !(sk->userlocks & SOCK_BINDPORT_LOCK))
			__sctp_bhash_unhash(sk);
		if (sp->saddr && !(sk->userlocks & SOCK_BINDADDR_LOCK))
			__sctp_free_saddrs(sk);
	} else
		swerr();
	local_bh_enable();
}

/*
 *  ---------------------------------------------------------------------------
 *
 *  SCTP GET PORT
 *
 *  ---------------------------------------------------------------------------
 *
 *  Obtain a reference to a local port for the given socket.  If snum is zero it
 *  means to select any available local port.  This is used in a number of
 *  binding and autobinding operations of the socket.
 *
 *  SO_REUSEADDR
 *	Indicates that the rules used in validating addresses supplied in a
 *	bind(2) call should allow reuse of local addresses.  For PF_INET
 *	sockets this means that a socket may bind, except when there is an
 *	active listening socket bound to the address.  When there is an active
 *	listening socket bound to INADDR_ANY with a specific port then it is
 *	not possible to bind to this port for any local address.
 *
 */
STATIC int sctp_port_rover = 0;
#ifdef sysctl_local_port_range
extern int sysctl_local_port_range[2];
#else
STATIC int sysctl_local_port_range[2] = { 1024, 4999 };
#endif

STATIC INLINE int sctp_addr_match(struct sctp_opt *sp1, struct sctp_opt *sp2)
{
	struct sctp_saddr *ss1, *ss2;
	/* This loop isn't as bad as it looks, normally we only have 2 or 3 source
	   addresses on our list. */
	for (ss1 = sp1->saddr; ss1; ss1 = ss1->next)
		for (ss2 = sp2->saddr; ss2; ss2 = ss2->next)
			if (ss1->saddr == ss2->saddr)
				return 1;
	return 0;
}

STATIC INLINE int sctp_bind_conflict(struct sock *sk, struct sctp_bind_bucket *sb)
{
	struct sctp_opt *sp2, *sp = SCTP_PROT(sk);
	struct sock *sk2 = NULL;
	/* The bind bucket is marked for fastreuse when there is no listening or
	   non-reusable socket on the owners list.  */
	if (sb->owners && sb->fastreuse != 0 && sk->reuse != 0 &&
	    sk->state != SCTP_LISTEN)
		return 0;
	for (sp2 = sb->owners, sk2 = SCTP_SOCK(sp2); sp2;
	     sp2 = sp2->bnext, sk2 = SCTP_SOCK(sp2))
		if (sk != sk2 && sk->bound_dev_if == sk2->bound_dev_if &&
		    (!sk->reuse || !sk2->reuse || sk->state == SCTP_LISTEN ||
		     sk2->state == SCTP_LISTEN))
			if (!(sk->userlocks & SOCK_BINDADDR_LOCK) ||
			    !(sk2->userlocks & SOCK_BINDADDR_LOCK) ||
			    /* dig into the source address list */
			    sctp_addr_match(sp, sp2))
				break;
	if (sp2)
		ptrace(("CONFLICT: port %d conflict sk = %p with sk2 = %p\n",
			sb->port, sk, sk2));
	return (sp2 != NULL);
}

STATIC int sctp_get_port(struct sock *sk, unsigned short snum)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	struct sctp_bind_bucket *sb = NULL;
	local_bh_disable();
	if (snum == 0) {
		/* This approach to selecting an available port number is identical
		   to that used for TCP IPv4. We use the same port ranges.  */
		static spinlock_t sctp_portalloc_lock = SPIN_LOCK_UNLOCKED;
		int low = sysctl_local_port_range[0];
		int high = sysctl_local_port_range[1];
		int rem = (high - low) + 1;
		int rover;
		/* find a fresh, completely unused port number (that way we are
		   guaranteed not to have a port conflict */
		spin_lock(&sctp_portalloc_lock);
		rover = sctp_port_rover;
		for (; rem > 0; rover++, rem--) {
			if (rover > high || rover < low)
				rover = low;
			if (!(sb = sctp_bindb_get(rover)))
				break;
			sctp_bindb_put(rover);
		}
		sctp_port_rover = rover;
		spin_unlock(&sctp_portalloc_lock);
		if (rem <= 0 || sb)
			goto fail;
		if (!(snum = rover)) {
			swerr();
			goto fail_put;
		}
	} else
		sb = sctp_bindb_get(snum);
	if (!sb) {
		/* create an entry if there isn't one already */
		/* we still have a write lock on the hash slot */
		if (!(sb = __sctp_bindb_create(snum)))
			goto fail_put;
	} else {
		/* check reusability or conflict if entry exists */
		if (sctp_bind_conflict(sk, sb))
			goto fail_put;
	}
	/* put this socket in the bind hash */
	__sctp_bhash_insert(sp, sb);
	sctp_bindb_put(snum);
	local_bh_enable();
	return 0;
      fail_put:
	sctp_bindb_put(snum);
      fail:
	local_bh_enable();
	return 1;
}

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP INIT
 *
 *  --------------------------------------------------------------------------
 */
STATIC int sctp_init_sock(struct sock *sk)
{
	struct sctp_opt *sp = SCTP_PROT(sk);
	ptrace(("Initializing socket sk=%p\n", sk));
	sp->sackf = 0;	/* don't delay first sack */
#ifdef CONFIG_SCTP_TCP_COMPATIBLE
	if (sk->type != SOCK_STREAM)
#endif
		sk->keepopen = 1;
	/* initialize timers */
	init_timer(&sp->timer_init);
	sp->timer_init.function = &sctp_init_timeout;
	sp->timer_init.data = (unsigned long) sk;
	init_timer(&sp->timer_cookie);
	sp->timer_cookie.function = &sctp_cookie_timeout;
	sp->timer_cookie.data = (unsigned long) sk;
	init_timer(&sp->timer_shutdown);
	sp->timer_shutdown.function = &sctp_shutdown_timeout;
	sp->timer_shutdown.data = (unsigned long) sk;
	init_timer(&sp->timer_guard);
	sp->timer_guard.function = &sctp_guard_timeout;
	sp->timer_guard.data = (unsigned long) sk;
	init_timer(&sp->timer_sack);
	sp->timer_sack.function = &sctp_sack_timeout;
	sp->timer_sack.data = (unsigned long) sk;
#ifdef CONFIG_SCTP_ADD_IP
	init_timer(&sp->timer_asconf);
	sp->timer_asconf.function = &sctp_asconf_timeout;
	sp->timer_asconf.data = (unsigned long) sk;
#endif
#if defined(CONFIG_SCTP_LIFETIMES) || defined(CONFIG_SCTP_PARTIAL_RELIABILITY)
	init_timer(&sp->timer_life);
	sp->timer_life.function = &sctp_life_timeout;
	sp->timer_life.data = (unsigned long) sk;
#endif
	sp->ck_inc = sysctl_sctp_cookie_inc;
	init_timer(&sk->timer);
//      sk->timer.function = &sctp_timeout;
//      sk->timer.data = (unsigned long) sk;
//      sk->timeout = 0;
	/* initialize queues */
	skb_queue_head_init(&sp->accq);
	skb_queue_head_init(&sp->expq);
	skb_queue_head_init(&sp->urgq);
	skb_queue_head_init(&sp->oooq);
	skb_queue_head_init(&sp->errq);
	skb_queue_head_init(&sp->rtxq);
	skb_queue_head_init(&sp->dupq);
	skb_queue_head_init(&sp->ackq);
#ifdef CONFIG_SCTP_THROTTLE_HEARTBEATS
	sp->hb_tint = (sysctl_sctp_heartbeat_itvl >> 1) + 1;
	sp->hb_rcvd = jiffies;
#endif
	sk->max_ack_backlog = 0;
	sk->shutdown = 0;
	sctp_change_state(sk, SCTP_CLOSED);
	sp->a_rwnd = sk->rcvbuf;
	/* *INDENT-OFF* */
	/* association defaults */
	sp->max_istr	= sysctl_sctp_max_istreams;
	sp->req_ostr	= sysctl_sctp_req_ostreams;
	sp->max_inits	= sysctl_sctp_max_init_retries;
	sp->max_retrans	= sysctl_sctp_assoc_max_retrans;
	sp->ck_life	= sysctl_sctp_valid_cookie_life;
	sp->ck_inc	= sysctl_sctp_cookie_inc;
	sp->hmac	= sysctl_sctp_mac_type;
	sp->cksum	= sysctl_sctp_csum_type;
	sp->throttle	= sysctl_sctp_throttle_itvl;
	sp->sid		= 0;
	sp->ppi		= 0;
	sp->max_sack	= sysctl_sctp_max_sack_delay;
	sp->max_burst	= sysctl_sctp_max_burst;
	sp->pmtu	= ip_rt_min_pmtu;
	sp->amps	= sp->pmtu - sp->ext_header_len - sizeof(struct iphdr) - sizeof(struct sctphdr);
	/* destination defaults */
	sp->rto_ini	= sysctl_sctp_rto_initial;
	sp->rto_min	= sysctl_sctp_rto_min;
	sp->rto_max	= sysctl_sctp_rto_max;
	sp->rtx_path	= sysctl_sctp_path_max_retrans;
	sp->hb_itvl	= sysctl_sctp_heartbeat_itvl;
#ifdef CONFIG_SCTP_PARTIAL_RELIABILITY
	sp->prel	= sysctl_sctp_partial_reliability;
#endif
	sp->l_caps	= 0;
#ifdef CONFIG_SCTP_ADAPTATION_LAYER_INFO
	sp->l_ali	= sysctl_sctp_adaptation_layer_info;
	if (sp->l_ali)
		sp->l_caps |= SCTP_CAPS_ALI;
#endif
	/* *INDENT-ON* */
	/* put on master list */
	spin_lock_bh(&sctp_protolock);
	if ((sk->next = sctp_protolist))
		sk->next->pprev = &sk->next;
	sk->pprev = &sctp_protolist;
	sctp_protolist = sk;
#ifdef CONFIG_SCTP_MODULE
	MOD_INC_USE_COUNT;
#endif
	sk->write_space = sctp_write_space;
	atomic_inc(&sctp_socket_count);
	spin_unlock_bh(&sctp_protolock);
	printd(("INFO: There are now %d sockets allocated\n",
		atomic_read(&sctp_socket_count)));
	ptrace(("Socket %p creation reference count = %d\n", sk,
		atomic_read(&sk->refcnt)));
	sk->use_write_queue = 0;
	return (0);
}

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP DESTROY
 *
 *  --------------------------------------------------------------------------
 */
STATIC int sctp_destroy(struct sock *sk)
{
	ptrace(("Destroying sk = %p\n", sk));
	if (sk->pprev) {
		if (sk->userlocks & (SOCK_BINDPORT_LOCK | SOCK_BINDADDR_LOCK)) {
			ptrace(("Breaking user locks on destroy sk %p\n", sk));
			sk->userlocks = 0;
			sctp_reset(sk);
		}
		sctp_clear(sk);
		/* take off master list */
		spin_lock_bh(&sctp_protolock);
		if ((*(sk->pprev) = sk->next))
			sk->next->pprev = sk->pprev;
		sk->next = NULL;
		sk->pprev = NULL;
		atomic_dec(&sctp_socket_count);
		spin_unlock_bh(&sctp_protolock);
		printd(("INFO: There are now %d sockets allocated\n",
			atomic_read(&sctp_socket_count)));
#ifdef CONFIG_SCTP_MODULE
		MOD_DEC_USE_COUNT;
#endif
		return (0);
	}
	ptrace(("ERROR: Double destroying sk = %p\n", sk));
	swerr();
	return (-EFAULT);
}

/*
 *  ==========================================================================
 *
 *  IP PROTOCOL INTERFACE
 *
 *  ==========================================================================
 */
/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP RCV OOTB
 *
 *  --------------------------------------------------------------------------
 */
/* *INDENT-OFF* */
STATIC int sctp_rcv_ootb(struct sk_buff * skb)
{
	struct iphdr *iph = skb->nh.iph;
	struct sctphdr *sh = SCTP_SKB_SH(skb);
	struct sctpchdr *ch = (typeof(ch)) skb->data;
	int sat = inet_addr_type(iph->saddr);
	seldom();
	ensure(skb, return (-EFAULT));
	if (sat != RTN_UNICAST && sat != RTN_LOCAL) {
		/* RFC 2960 8.4(1) */
		kfree_skb(skb);
		return (0);
	}
	switch (ch->type) {
	case SCTP_CTYPE_COOKIE_ACK:		/* RFC 2960 8.4(7).	*/
	case SCTP_CTYPE_ERROR:			/* RFC 2960 8.4(7).	*/
	case SCTP_CTYPE_ABORT:			/* RFC 2960 8.4(2).	*/
	case SCTP_CTYPE_SHUTDOWN_COMPLETE:	/* RFC 2960 8.4(6).	*/
	case SCTP_CTYPE_INIT:			/* RFC 2960 8.4(3) and (8). */
	case SCTP_CTYPE_INIT_ACK:		/* RFC 2960 8.4(8).	*/
	case SCTP_CTYPE_COOKIE_ECHO:		/* RFC 2960 8.4(4) and (8). */
	default:
		seldom();
		break;
	case SCTP_CTYPE_SHUTDOWN:		/* RFC 2960 8.4(8).	*/
	case SCTP_CTYPE_SACK:			/* RFC 2960 8.4(8).	*/
	case SCTP_CTYPE_HEARTBEAT:		/* RFC 2960 8.4(8).	*/
	case SCTP_CTYPE_HEARTBEAT_ACK:		/* RFC 2960 8.4(8).	*/
	case SCTP_CTYPE_DATA:			/* RFC 2960 8.4(8).	*/
		seldom();
		if (!sctp_lookup_tcb(sh->dest, sh->srce, iph->daddr, iph->saddr))
			sctp_send_abort_ootb(iph->saddr, iph->daddr, sh);
		break;
	case SCTP_CTYPE_SHUTDOWN_ACK:
		seldom();
		if (!sctp_lookup_tcb(sh->dest, sh->srce, iph->daddr, iph->saddr))
			sctp_send_shutdown_complete_ootb(iph->saddr, iph->daddr, sh);
		break;
	}
	kfree_skb(skb);
	return (0);
}
/* *INDENT-ON* */

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP ERR
 *
 *  --------------------------------------------------------------------------
 *
 *  This is the error handler.
 *
 *  This routine is called by the ICMP module when it gets some sort of error
 *  condition.  It err < 0 then the socket should be closed and the error
 *  returned to the user.  If err > 0 it's just the icmp type << 8 | icmp
 *  code.  After adjustment header points to the first 8 bytes of the SCTP
 *  Common Header.  We need to find the appropriate port or verification tag.
 *
 *  The locking strategy used here is very "optimistic".  When someone else
 *  accesses the socket the ICMP is just dropped ad for some paths there is no
 *  check at all.  A more general error queue to queue errors for later
 *  handling is probably better.
 *
 */
#ifndef ip_rt_mtu_expires
#define ip_rt_mtu_expires (10*60*HZ)
#endif
__SCTP_STATIC void sctp_v4_err(struct sk_buff *skb, uint32_t info)
{
	struct iphdr *iph = (struct iphdr *) skb->data;
	struct sctphdr *sh = (struct sctphdr *) (skb->data + (iph->ihl << 2));
	struct sctp_daddr *sd;
	int type = skb->h.icmph->type;
	int code = skb->h.icmph->code;
	struct sock *sk;
	int err;
#define ICMP_MIN_LENGTH 8
	if (skb->len < (iph->ihl << 2) + ICMP_MIN_LENGTH)
		goto drop;
	if (sh->v_tag)
		sk = sctp_lookup_ptag(sh->v_tag, sh->srce, sh->dest, iph->saddr,
				      iph->daddr);
	else
		sk = sctp_lookup_tcb(sh->srce, sh->dest, iph->saddr, iph->daddr);
	if (!sk)
		goto no_socket;
	bh_lock_sock(sk);
	if (sk->state == SCTP_CLOSED)
		goto closed;
	sd = sctp_find_daddr(sk, iph->daddr);
	switch (type) {
	case ICMP_SOURCE_QUENCH:
		if (!sd)
			goto discard_and_putsk;
		if (sock_locked(sk))
			goto lock_dropped;
		sd->ssthresh =
		    ((sd->cwnd >> 1) > (sd->mtu << 1)) ? sd->cwnd >> 1 : sd->mtu << 1;
		sd->cwnd = sd->mtu;
		/* SCTP IG Section 2.9 */
		sd->partial_ack = 0;
		sd->rto = (sd->rto) ? sd->rto << 1 : 1;
		sd->rto = (sd->rto_min > sd->rto) ? sd->rto_min : sd->rto;
		sd->rto = (sd->rto_max < sd->rto) ? sd->rto_max : sd->rto;
		goto discard_and_putsk;
	case ICMP_PARAMETERPROB:
		err = EPROTO;
		break;
	case ICMP_DEST_UNREACH:
		if (code > NR_ICMP_UNREACH)
			goto discard_and_putsk;
		if (code == ICMP_FRAG_NEEDED) {
			if (sock_locked(sk))
				goto lock_dropped;
			if (sd && sd->dst_cache) {
				size_t mtu = htons(skb->h.icmph->un.frag.mtu);
				ip_rt_update_pmtu(sd->dst_cache, mtu);
				if (dst_pmtu(sd->dst_cache) > mtu && mtu && mtu >= 68
#ifdef HAVE_STRUCT_INET_PROTOCOL_PROTOCOL
				    && !(sd->dst_cache->mxlock & (1 << RTAX_MTU))
#endif
				    ) {
					dst_update_pmtu(sd->dst_cache, mtu);
					dst_set_expires(sd->dst_cache,
							ip_rt_mtu_expires);
				}
			}
		}
		err = icmp_err_convert[code].errno;
		break;
	case ICMP_TIME_EXCEEDED:
		err = EHOSTUNREACH;
		break;
	default:
		goto discard_and_putsk;
	}
	if (sk->state == SCTP_LISTEN)
		/* 
		 *  NOTE: Unlike TCP, we do not have partially formed
		 *  sockets in the accept queue, so this ICMP error
		 *  should have gone to the established socket in the
		 *  accept queue that sent the COOKIE-ACK that
		 *  generated the error.  Otherwise, it is for an
		 *  INIT-ACK that can't get to its destination, so we
		 *  don't care, just ignore it.
		 */
		goto listening;
	sctp_error_report(sk, err);
	if (sd && sd->dst_cache)
		dst_negative_advice(&sd->dst_cache);
      discard_and_putsk:
	bh_unlock_sock(sk);
	sock_put(sk);
	return;
      no_socket:
	ptrace(("ERROR: Couldn't find socket for ICMP message\n"));
	goto drop;
      drop:
	ICMP_INC_STATS_BH(IcmpInErrors);
	return;
      listening:
	ptrace(("ERROR: ICMP for listening socket\n"));
	goto discard_and_putsk;
      closed:
	ptrace(("ERROR: ICMP for closed socket\n"));
	goto discard_and_putsk;
      lock_dropped:
	ptrace(("ERROR: Hit locks on socket\n"));
	NET_INC_STATS_BH(LockDroppedIcmps);
	goto discard_and_putsk;
}

/*
 *  --------------------------------------------------------------------------
 *
 *  SCTP RCV
 *
 *  --------------------------------------------------------------------------
 *
 *  This is the received frame handler for SCTP IPv4.  All packets received by
 *  IPv4 with the protocol number IPPROTO_SCTP will arrive here first.  We
 *  should be performing the Adler-32 checksum on the packet.  If the Adler-32
 *  checksum fails then we should silently discard the packet per RFC 2960.
 */
__SCTP_STATIC int sctp_v4_rcv(struct sk_buff *skb)
{
	int ret = 0;
	struct sock *sk;
	struct sctphdr *sh = SCTP_SKB_SH(skb);
	struct sctpchdr *ch = (typeof(ch)) skb_pull(skb, sizeof(*sh));
	if (skb->pkt_type != PACKET_HOST)
		goto bad_pkt_type;
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPINSCTPPACKS
	SCTP_INC_STATS_BH(SctpInSCTPPacks);
#endif
	if (skb_is_nonlinear(skb) && skb_linearize(skb, GFP_ATOMIC) != 0)
		goto linear_fail;
	if (skb->len & 0x3)
		goto bad_padding;
	/* sanity check first chunk header */
	if (skb->len < sizeof(*ch))
		goto too_small;
	if (ntohs(ch->len) < sizeof(*ch))
		goto bad_chunk_len;
	if (skb->len < PADC(ntohs(ch->len)))
		goto bad_chunk_len;
	/* we do the lookup before the checksum */
	if (!(sk = sctp_lookup(sh, skb->nh.iph->daddr, skb->nh.iph->saddr)))
		goto no_sctp_socket;
	/* Perform the Adler-32 checksum per RFC 2960 Appendix B. */
	skb->csum = ntohl(sh->check);
	if (!(skb->dev->features & (NETIF_F_NO_CSUM | NETIF_F_HW_CSUM))) {
		sh->check = 0;
		if (!cksum_sk_verify(sk, skb->csum, sh, skb->len + sizeof(*sh))) {
			sh->check = htonl(skb->csum);
			goto bad_checksum;
		}
		sh->check = htonl(skb->csum);
	}
	bh_lock_sock(sk);
#if defined HAVE_IPSEC_SK_POLICY_EXPORT || defined HAVE_IPSEC_SK_POLICY_ADDR
	if (!ipsec_sk_policy(sk, skb))
		goto failed_security;
#endif
#if defined HAVE___XFRM_POLICY_CHECK_EXPORT || defined HAVE___XFRM_POLICY_CHECK_ADDR
	if (!xfrm4_policy_check(sk, XFRM_POLICY_IN, skb))
		goto failed_security;
#endif
	skb->dev = NULL;
	if (sock_locked(sk))
		sk_add_backlog(sk, skb);
	else
		ret = sctp_backlog_rcv(sk, skb);
	bh_unlock_sock(sk);
	sock_put(sk);
	return (ret);
      discard_and_putsk:
	bh_unlock_sock(sk);
	sock_put(sk);
      discard_it:
	/* Discard frame silently. */
	kfree_skb(skb);
	return (0);
      bad_pkt_type:
	ptrace(("ERROR: Packet not PACKET_HOST\n"));
	goto discard_it;
      linear_fail:
	ptrace(("ERROR: Couldn't linearize skb\n"));
	goto discard_it;
      bad_padding:
	ptrace(("ERROR: Packet not padded\n"));
	goto discard_it;
      too_small:
	ptrace(("ERROR: Packet too small\n"));
	goto discard_it;
      bad_chunk_len:
	ptrace(("ERROR: Chunk length incorrect\n"));
	goto discard_it;
      bad_checksum:
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPCHECKSUMERRORS
	SCTP_INC_STATS_BH(SctpChecksumErrors);
#endif
	ptrace(("ERROR: Bad checksum\n"));
	goto discard_it;
      no_sctp_socket:
	/* Perform the Adler-32 checksum per RFC 2960 Appendix B. */
	skb->csum = ntohl(sh->check);
	if (!(skb->dev->features & (NETIF_F_NO_CSUM | NETIF_F_HW_CSUM))) {
		sh->check = 0;
		if (!cksum_verify(skb->csum, sh, skb->len + sizeof(*sh))) {
			sh->check = htonl(skb->csum);
			goto bad_checksum;
		}
		sh->check = htonl(skb->csum);
	}
	ptrace(("ERROR: Received OOTB packet\n"));
#ifdef HAVE_STRUCT_SCTP_MIB_SCTPOUTOFBLUES
	SCTP_INC_STATS_BH(SctpOutOfBlues);
#endif
#if defined HAVE___XFRM_POLICY_CHECK_EXPORT || defined HAVE___XFRM_POLICY_CHECK_ADDR
	if (!xfrm4_policy_check(NULL, XFRM_POLICY_IN, skb))
		goto discard_it;
#endif
#ifndef CONFIG_SCTP_DISCARD_OOTB
	/* RFC 2960 Section 8.4 */
	return sctp_rcv_ootb(skb);
#endif
	goto discard_it;
	goto failed_security; /* shut up compiler */
      failed_security:
	ptrace(("ERROR: Packet failed security policy\n"));
	goto discard_and_putsk;
}

#ifdef CONFIG_SCTP_ADD_IP
/*
 *  ==========================================================================
 *
 *  Netdevice Notifier for ADD-IP
 *
 *  ==========================================================================
 *  
 *  This is called whenever something happens with an IP network device.  When
 *  interfaces go up we check to see if IP addresses can be added to any
 *  SCTP sockets.  When interfaces go down, we check to see if IP addresses
 *  need to be removed from any SCTP sockets.
 */
STATIC int sctp_notifier(struct notifier_block *self, unsigned long msg, void *data)
{
	struct net_device *dev = (struct net_device *) data;
	struct in_device *in_dev;
	if (!(in_dev = __in_dev_get(dev)))
		goto done;
	switch (msg) {
	case NETDEV_UP:
	case NETDEV_REBOOT:
	{
		struct sock *sk;
		struct in_ifaddr *ifa;
		for (sk = sctp_protolist; sk; sk = sk->next) {
			struct sctp_opt *sp = SCTP_PROT(sk);
			bh_lock_sock(sk);
			/* only do ADD-IP on established sockets */
			if (sk->state != SCTP_ESTABLISHED)
				continue;
			/* peer must be capable of ADD-IP */
			if (sp->p_caps & SCTP_CAPS_ADD_IP)
				continue;
			/* socket must be wildcard bound */
			if (sk->userlocks & SOCK_BINDADDR_LOCK)
				continue;
			read_lock(&in_dev->lock);
			for (ifa = in_dev->ifa_list; ifa; ifa = ifa->ifa_next) {
				struct sctp_saddr *ss;
				int err = 0;
				/* skip loopback on wildcard */
				if (LOOPBACK(ifa->ifa_local))
					continue;
				if (!(ss = sctp_find_saddr(sk, ifa->ifa_local))) {
					if ((ss = __sctp_saddr_alloc
					     (sk, ifa->ifa_local, &err))) {
						ss->flags |= SCTP_SRCEF_ADD_REQUEST;
						sp->sackf |= SCTP_SACKF_ASC;
					}
				} else
					ss->flags &= ~SCTP_SRCEF_DEL_REQUEST;
				/* should we clear del pending too? */
			}
			read_unlock(&in_dev->lock);
			sctp_transmit_wakeup(sk);
			bh_unlock_sock(sk);
		}
	}
	case NETDEV_DOWN:
	case NETDEV_GOING_DOWN:
	{
		struct sock *sk;
		struct in_ifaddr *ifa;
		for (sk = sctp_protolist; sk; sk = sk->next) {
			struct sctp_opt *sp = SCTP_PROT(sk);
			bh_lock_sock(sk);
			/* only do ADD-IP on established sockets */
			if (sk->state != SCTP_ESTABLISHED)
				continue;
			/* peer must be capable of ADD-IP */
			if (sp->p_caps & SCTP_CAPS_ADD_IP)
				continue;
			/* socket must be wildcard bound */
			if (sk->userlocks & SOCK_BINDADDR_LOCK)
				continue;
			read_lock(&in_dev->lock);
			for (ifa = in_dev->ifa_list; ifa; ifa = ifa->ifa_next) {
				struct sctp_saddr *ss;
				/* skip loopback on wildcard */
				if (LOOPBACK(ifa->ifa_local))
					continue;
				if ((ss = sctp_find_saddr(sk, ifa->ifa_local))) {
					ss->flags |= SCTP_SRCEF_DEL_REQUEST;
					sp->sackf |= SCTP_SACKF_ASC;
					ss->flags &= ~SCTP_SRCEF_ADD_REQUEST;
					/* should we clear add pending too? */
				}
			}
			read_unlock(&in_dev->lock);
			sctp_transmit_wakeup(sk);
			bh_unlock_sock(sk);
		}
	}
	case NETDEV_CHANGEADDR:
		/* we should probably do something for this, but I don't know
		   wheterh it is possible to change addresses on an up interface
		   anyway */
	default:
	case NETDEV_CHANGE:
	case NETDEV_REGISTER:
	case NETDEV_UNREGISTER:
	case NETDEV_CHANGEMTU:
	case NETDEV_CHANGENAME:
		break;
	}
      done:
	return NOTIFY_DONE;
}
#endif

/*
 *  ==========================================================================
 *
 *  Kernel Module Initialization and Cleanup
 *
 *  ==========================================================================
 */

/* *INDENT-OFF* */
__SCTP_STATIC struct proto sctp_prot = {
	name:		"SCTP",
	close:		sctp_close,
	connect:	sctp_connect,
	disconnect:	sctp_disconnect,
	accept:		sctp_accept,
	ioctl:		sctp_ioctl,
	init:		sctp_init_sock,
	destroy:	sctp_destroy,
	shutdown:	sctp_shutdown,
	setsockopt:	sctp_setsockopt,
	getsockopt:	sctp_getsockopt,
	sendmsg:	sctp_sendmsg,
	recvmsg:	sctp_recvmsg,
	bind:		sctp_bind,
	backlog_rcv:	sctp_backlog_rcv,
	hash:		sctp_hash,
	unhash:		sctp_unhash,
	get_port:	sctp_get_port,
};
#ifdef CONFIG_SCTP_ADD_IP
STATIC struct notifier_block sctp_netdev_notifier = {
	notifier_call:	sctp_notifier,
};
#endif
/* *INDENT-ON* */

__SCTP_STATIC void __SCTP_INIT sctp_v4_init(struct net_proto_family *ops)
{
#ifdef CONFIG_SCTP_MODULE
	sctp_prot_hook = &sctp_prot;
#endif
#ifdef CONFIG_SCTP_ADD_IP
	register_netdevice_notifier(&sctp_netdev_notifier);
#endif
}
extern void __skb_cb_too_small_for_sctp(int, int);	/* never defined */
__SCTP_STATIC void __SCTP_INIT sctp_init(void)
{
	struct sk_buff *skb = NULL;
	if (sizeof(struct sctp_skb_cb) > sizeof(skb->cb)) {
		/* 
		 *  The control block asigned to an sk_buff is too small for
		 *  SCTP, so I suppose we are doomed.  This (never defined) call
		 *  will fail to resolve when compiler detects above.  Sneaky
		 *  trick taken from NET4 TCP code.
		 */
		__skb_cb_too_small_for_sctp(sizeof(struct sctp_skb_cb),
					    sizeof(skb->cb));
	}
	sctp_init_caches();
	sctp_init_hashes();
	/* 
	 *  TODO: adjust buffer size sysctls, port ranges and other
	 *  parameters based on dynamic allocations.
	 */
}

#ifdef CONFIG_PROC_FS
STATIC void get_sctp_sock(struct sock *sk, char *tmpbuf, int i)
{
	unsigned int daddr, saddr;
	__u16 dport, sport;
	int timer_active;
	unsigned long timer_expires;
	struct sctp_opt *sp = SCTP_PROT(sk);
	daddr = sk->daddr;
	saddr = sk->rcv_saddr;
	dport = ntohs(sk->dport);
	sport = ntohs(sk->sport);
	if (timer_pending(&sp->timer_init)) {
		timer_active = 1;
		timer_expires = sp->timer_init.expires;
	} else if (timer_pending(&sp->timer_cookie)) {
		timer_active = 2;
		timer_expires = sp->timer_cookie.expires;
	} else if (timer_pending(&sp->timer_asconf)) {
		timer_active = 3;
		timer_expires = sp->timer_asconf.expires;
	} else if (timer_pending(&sp->timer_shutdown)) {
		timer_active = 4;
		timer_expires = sp->timer_shutdown.expires;
	} else if (timer_pending(&sp->timer_guard)) {
		timer_active = 5;
		timer_expires = sp->timer_guard.expires;
	} else if (timer_pending(&sp->timer_life)) {
		timer_active = 6;
		timer_expires = sp->timer_life.expires;
	} else if (timer_pending(&sp->timer_sack)) {
		timer_active = 7;
		timer_expires = sp->timer_sack.expires;
	} else {
		timer_active = 0;
		timer_expires = jiffies;
	}
	sprintf(tmpbuf, "%4d: %08X:%04X %08X:%04X "
		" %02X %08X:%08X %02X:%08lX %08X %5d %8d %lu %d %p",
		i, saddr, sport, daddr, dport, sk->state,
		atomic_read(&sk->wmem_alloc), atomic_read(&sk->rmem_alloc),
		timer_active, timer_expires - jiffies,
		sp->retransmits, sock_i_uid(sk), 0, sock_i_ino(sk),
		atomic_read(&sk->refcnt), sk);
}

#define TMPSZ 150
__SCTP_STATIC int sctp_get_info(char *buffer, char **start, off_t offset, int length)
{
	int len = 0, num = 0, i;
	off_t begin, pos = 0;
	char tmpbuf[TMPSZ + 1];
	if (offset < TMPSZ)
		len += sprintf(buffer, "%-*s\n", TMPSZ - 1,
			       "  sl  local_address rem_address   st tx_queue "
			       "rx_queue tr tm->when retrnsmt   uid  timeout inode");
	pos = TMPSZ;
	/* First, walk listening socket table */
	local_bh_disable();
	for (i = 0; i < sctp_lhash_size; i++) {
		struct sctp_opt *sp;
		struct sctp_hash_bucket *hp = &sctp_lhash[i];
		read_lock_bh(&hp->lock);
		for (sp = hp->list; sp; sp = sp->lnext) {
			struct sock *sk = SCTP_SOCK(sp);
			pos += TMPSZ;
			if (pos <= offset)
				continue;
			get_sctp_sock(sk, tmpbuf, num);
			len += sprintf(buffer + len, "%-*s\n", TMPSZ - 1, tmpbuf);
			if (pos >= offset + length) {
				read_unlock_bh(&hp->lock);
				goto out;
			}
			/* all open requests are in normal hash table */
		}
		read_unlock_bh(&hp->lock);
	}
	/* next walk established hash */
	for (i = 0; i < sctp_thash_size; i++) {
		struct sctp_opt *sp;
		struct sctp_hash_bucket *hp = &sctp_thash[i];
		read_lock_bh(&hp->lock);
		for (sp = hp->list; sp; sp = sp->tnext) {
			struct sock *sk = SCTP_SOCK(sp);
			pos += TMPSZ;
			if (pos <= offset)
				continue;
			get_sctp_sock(sk, tmpbuf, num);
			len += sprintf(buffer + len, "%-*s\n", TMPSZ - 1, tmpbuf);
			if (pos >= offset + length) {
				read_unlock_bh(&hp->lock);
				goto out;
			}
		}
		read_unlock_bh(&hp->lock);
	}
      out:
	local_bh_enable();
	begin = len - (pos - offset);
	*start = buffer + begin;
	len -= begin;
	if (len > length)
		len = length;
	if (len < 0)
		len = 0;
	return len;
}
#endif				/* CONFIG_PROC_FS */

#ifdef CONFIG_SCTP_MODULE
STATIC void sctp_v4_cleanup(void)
{
#ifdef CONFIG_SCTP_ADD_IP
	register_netdevice_notifier(&sctp_netdev_notifier);
#endif
	sctp_prot_hook = NULL;	/* what about race? */
}

STATIC void sctp_cleanup(void)
{
	struct sock *sk;
	spin_lock_bh(&sctp_protolock);
	for (sk = sctp_protolist; sk; sk = sk->next)
		sctp_discon_ind(sk, 0);
	spin_unlock_bh(&sctp_protolock);
	sctp_term_hashes();
	sctp_term_caches();
}

/* *INDENT-OFF* */
struct proto_ops inet_sctp_ops = {
	family:		PF_INET,
	release:	inet_release,
	bind:		inet_bind,
	connect:	inet_stream_connect,
	socketpair:	sock_no_socketpair,
	accept:		inet_accept,
#if defined(HAVE_INET_MULTI_GETNAME_EXPORT) || defined (HAVE_INET_MULTI_GETNAME_ADDR)
	getname:	inet_multi_getname,
#endif
#if defined(HAVE_INET_GETNAME_EXPORT) || defined (HAVE_INET_GETNAME_ADDR)
	getname:	inet_getname,
#endif
	poll:		sctp_poll,
	ioctl:		inet_ioctl,
	listen:		sctp_listen,
	shutdown:	inet_shutdown,
	setsockopt:	inet_setsockopt,
	getsockopt:	inet_getsockopt,
	sendmsg:	inet_sendmsg,
	recvmsg:	inet_recvmsg,
	mmap:		sock_no_mmap,
	sendpage:	sock_no_sendpage,   /* TODO: make an SCTP sendpage */
};

STATIC struct inet_protosw sctpsw_array[] = {
	{
		type:		SOCK_SEQPACKET,
		protocol:	IPPROTO_SCTP,
		prot:		&sctp_prot,
		ops:		&inet_sctp_ops,
		capability:	-1,
		no_check:	0,
		flags:		0,
	}
#ifdef CONFIG_SCTP_TCP_COMPATIBLE
	,{
		type:		SOCK_STREAM,
		protocol:	IPPROTO_SCTP,
		prot:		&sctp_prot,
		ops:		&inet_sctp_ops,
		capability:	-1,
		no_check:	0,
		flags:		0,
	}
#endif
#ifdef CONFIG_SCTP_UDP_COMPATIBLE
	,{
		type:		SOCK_RDM,
		protocol:	IPPROTO_SCTP,
		prot:		&sctp_prot,
		ops:		&inet_sctp_ops,
		capability:	-1,
		no_check:	0,
		flags:		0,
	}
#endif
};

#define SCTPSW_ARRAY_LEN (sizeof(sctpsw_array)/sizeof(struct inet_protosw))

#ifdef HAVE_STRUCT_INET_PROTOCOL_PROTOCOL
STATIC struct inet_protocol sctp_protocol = {
	.handler = sctp_v4_rcv,		/* SCTP data handler */
	.err_handler = sctp_v4_err,	/* SCTP error control */
	.protocol = IPPROTO_SCTP,	/* protocol ID */
	.name = "SCTP",			/* name */
};
STATIC void
sctp_init_proto(void)
{
	inet_add_protocol(&sctp_protocol);
}
STATIC void
sctp_term_proto(void)
{
	inet_del_protocol(&sctp_protocol);
}
#endif

#ifdef HAVE_STRUCT_INET_PROTOCOL_NO_POLICY
STATIC struct inet_protocol sctp_protocol = {
	.handler = sctp_v4_rcv,		/* SCTP data handler */
	.err_handler = sctp_v4_err,	/* SCTP error control */
};
STATIC void
sctp_init_proto(void)
{
	inet_add_protocol(&sctp_protocol, IPPROTO_SCTP);
}
STATIC void
sctp_term_proto(void)
{
	inet_del_protocol(&sctp_protocol, IPPROTO_SCTP);
}
#endif
/* *INDENT-ON* */

/* from af_inet.c */
extern struct net_proto_family inet_family_ops;

int init_module(void)
{
	struct inet_protosw *s;
#ifdef CONFIG_SCTP_DEBUG
	if (SCTP_SOCK(SCTP_PROT((struct sock *) NULL)) != NULL) {
		ptrace(("ERROR: macro failure\n"));
		return -EFAULT;
	}
	if (SCTP_CB_SKB(SCTP_SKB_CB((struct sk_buff *) NULL)) != NULL) {
		ptrace(("ERROR: macro failure\n"));
		return -EFAULT;
	}
#endif
	sctp_init_proto();
	for (s = sctpsw_array; s < &sctpsw_array[SCTPSW_ARRAY_LEN]; ++s)
		inet_register_protosw(s);
	sctp_v4_init(&inet_family_ops);
	sctp_init();
#ifdef CONFIG_PROC_FS
	proc_net_create("sctp", 0, sctp_get_info);
#endif
	printk(KERN_INFO SCTP_BANNER);
	return 0;
}

void cleanup_module(void)
{
	struct inet_protosw *s;
#ifdef CONFIG_PROC_FS
	proc_net_remove("sctp");
#endif
	sctp_cleanup();
	sctp_v4_cleanup();
	for (s = sctpsw_array; s < &sctpsw_array[SCTPSW_ARRAY_LEN]; ++s)
		inet_unregister_protosw(s);
	sctp_term_proto();
}
#endif				/* CONFIG_SCTP_MODULE */
