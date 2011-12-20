/*
 * Copyright (c) 1990, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from the Stanford/CMU enet packet filter,
 * (net/enet.c) distributed as part of 4.3BSD, and code contributed
 * to Berkeley by Steven McCanne and Van Jacobson both of Lawrence
 * Berkeley Laboratory.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *      @(#)bpf.h	8.2 (Berkeley) 1/9/95
 *
 * @(#) Header: bpf.h,v 1.36 97/06/12 14:29:53 leres Exp  (LBL)
 */

#ifndef _NET_BPF_H_
#define _NET_BPF_H_

//#include <sys/time.h>

/* BSD style release date */
#define BPF_RELEASE 199606

typedef int32_t bpf_int32;
typedef u_int32_t bpf_u_int32;
typedef int64_t bpf_int64;
typedef u_int64_t bpf_u_int64;

/*
 * Alignment macros.  BPF_WORDALIGN rounds up to the next 
 * even multiple of BPF_ALIGNMENT. 
 */
#define BPF_ALIGNMENT sizeof(long)
#define BPF_WORDALIGN(x) (((x)+(BPF_ALIGNMENT-1))&~(BPF_ALIGNMENT-1))

#define BPF_MAXINSNS 512
#define BPF_DFLTBUFSIZE (1024*1024)	/* default static upper limit */
#define BPF_MAXBUFSIZE (1024*1024*16)	/* hard limit on sysctl'able value */
//#define BPF_MAXBUFSIZE 0x8000
#define BPF_MINBUFSIZE 32

/*
 *  Structure for BIOCSETF.
 */
struct bpf_program {
	u_int bf_len;
	struct bpf_insn *bf_insns;
};

#ifdef __LP64__
#ifdef __KERNEL__
/*
 * This structure is only useful within the kernel for performing conversion
 * from 32-bit users to the 64-bit kernel.
 */
struct bpf_program32 {
	u_int32_t bf_len;
	u_int32_t bf_insns;
};
#endif				/* __KERNEL__ */
#endif				/* __LP64__ */

/*
 * Struct returned by BIOCGSTATS.
 */
struct bpf_stat {
	u_int64_t bs_recv;		/* number of packets received */
	u_int64_t bs_drop;		/* number of packets dropped */
	u_int64_t bs_capt;		/* number of packets captured */
	u_int64_t bs_padding[13];
};

/*
 * Struct returned by BIOCGSTATSOLD.
 */
struct bpf_stat_old {
	u_int bs_recv;			/* number of packets received */
	u_int bs_drop;			/* number of packets dropped */
};

/*
 * Struct return by BIOCVERSION.  This represents the version number of 
 * the filter language described by the instruction encodings below.
 * bpf understands a program iff kernel_major == filter_major &&
 * kernel_minor >= filter_minor, that is, if the value returned by the
 * running kernel has the same major number and a minor number equal
 * equal to or less than the filter being downloaded.  Otherwise, the
 * results are undefined, meaning an error may be returned or packets
 * may be accepted haphazardly.
 * It has nothing to do with the source code version.
 */
struct bpf_version {
	u_short bv_major;
	u_short bv_minor;
};

/* Current version number. */
#define BPF_MAJOR_VERSION 1
#define BPF_MINOR_VERSION 1

/*
 * Historically, BPF has supported a single buffering model, first using mbuf
 * clusters in kernel, and later using malloc(9) buffer in kernel.  We now
 * support multiple buffering modes, which may be queried and set using
 * BIOCGETBUFMODE and BIOCSETBUFMODE.  So as to avoid handling the complexity of
 * changing modes while sniffing packets, the mode becomes fixed once an
 * interface has been attached to the BPF descriptor.
 */
#define BPF_BUFMODE_BUFFER	1	/* kernel buffers with read(). */
#define BPF_BUFMODE_ZBUF	2	/* zero-copy buffers */

/*
 * Struct used by BIOCSETZBUF, BIOCROTZBUF: describes up to tow zero-copy
 * buffers used by BPF.
 */
struct bpf_zbuf {
	void *bz_bufa;			/* location of 'a' zero-copy buffer */
	void *bz_bufb;			/* location of 'b' zero-copy buffer */
	size_t bz_buflen;		/* sizeof of zero-copy buffers */
};

/*
 * There are some wild differences in timeval.  On BSD, time_t and useconds_t
 * are integer values.  On POSIX systems, they are long.  Some POSIX systems
 * (like Solaris) make them integer to match BSD, other (like Linux) don't.
 */

struct bpf_timeval32 {
	int tv_sec;
	int tv_usec;
};

#ifdef _SUN_SOURCE
struct bpf_timeval {
	int32_t tv_sec;
	int32_t tv_usec;
};
#else				/* _SUN_SOURCE */
struct bpf_timeval {
	long tv_sec;
	long tv_usec;
};
#endif				/* _SUN_SOURCE */

/*
 * BPF ioctls
 *
 * The first set is for compatibility with Sun's pcc style
 * header files.  If your using gcc, we assume that you
 * have run fixincludes so the latter set should work.
 */
#define BIOC ('B'<<8)

#if (defined(sun) || defined(ibm032)) && !defined(__GNUC__)
#define BIOCGBLEN	_IOR(B,102, u_int)
#define BIOCSBLEN	_IOWR(B,102, u_int)
#define BIOCSETF	_IOW(B,103, struct bpf_program)
#define BIOCFLUSH	_IO(B,104)
#define BIOCPROMISC	_IO(B,105)
#define BIOCGDLT	_IOR(B,106, u_int)
#define BIOCGETIF	_IOR(B,107, struct ifreq)
#define BIOCGETLIF      _IOR(B,107, struct lifreq)	/* Solaris */
#define BIOCSETIF	_IOW(B,108, struct ifreq)
#define BIOCSETLIF      _IOW(B,108, struct lifreq)	/* Solaris */
#define BIOCOSRTIMEOUT	_IOW(B,109, struct bpf_timeval32)
#define BIOCOGRTIMEOUT	_IOR(B,110, struct bpf_timeval32)
#define BIOCGSTATS	_IOR(B,111, struct bpf_stat)
#define BIOCGSTATSOLD	_IOR(B,111, struct bpf_stat_old)
#define BIOCIMMEDIATE	_IOW(B,112, u_int)
#define BIOCVERSION	_IOR(B,113, struct bpf_version)
#define BIOCSTCPF	_IOW(B,114, struct bpf_program)	/* Solaris */
#define BIOCSUDPF	_IOW(B,115, struct bpf_program)	/* Solaris */
#define BIOCGRSIG	_IOR(B,114, u_int)
#define BIOCSRSIG	_IOW(B,115, u_int)
#define BIOCGHDRCMPLT	_IOR(B,116, u_int)
#define BIOCSHDRCMPLT	_IOW(B,117, u_int)
#define BIOCSDLT	_IOW(B,118, u_int)
#define BIOCGDLTLIST	_IOR(B,119, struct bpf_dltlist)
#define BIOCGSEESENT	_IOR(B,120, u_int)
#define BIOCSSEESENT	_IOW(B,121, u_int)
#define BIOCSRTIMEOUT	_IOW(B,122, struct bpf_timeval)
#define BIOCGRTIMEOUT	_IOR(B,123, struct bpf_timeval)
#define BIOCGFEEDBACK	_IOR(B,124, u_int)
#define BIOCSFEEDBACK	_IOW(B,125, u_int)
#define BIOCFEEDBACK	BIOCSFEEDBACK
#else
#define BIOCGBLEN	_IOR('B',102, u_int)
#define BIOCSBLEN	_IOWR('B',102, u_int)
#define BIOCSETF	_IOW('B',103, struct bpf_program)
#define BIOCFLUSH	_IO('B',104)
#define BIOCPROMISC	_IO('B',105)
#define BIOCGDLT	_IOR('B',106, u_int)
#define BIOCGETIF	_IOR('B',107, struct ifreq)
#define BIOCGETLIF      _IOR('B',107, struct lifreq)	/* Solaris */
#define BIOCSETIF	_IOW('B',108, struct ifreq)
#define BIOCSETLIF      _IOW('B',108, struct lifreq)	/* Solaris */
#define BIOCOSRTIMEOUT	_IOW('B',109, struct bpf_timeval32)	/* OpenBSD */
#define BIOCOGRTIMEOUT	_IOR('B',110, struct bpf_timeval32)	/* OpenBSD */
#define BIOCGSTATS	_IOR('B',111, struct bpf_stat)
#define BIOCGSTATSOLD	_IOR('B',111, struct bpf_stat_old)	/* Solaris too */
#define BIOCIMMEDIATE	_IOW('B',112, u_int)
#define BIOCVERSION	_IOR('B',113, struct bpf_version)
#define BIOCSTCPF	_IOW('B',114, struct bpf_program)	/* Solaris too */
#define BIOCSUDPF	_IOW('B',115, struct bpf_program)	/* Solaris too */
#define BIOCGRSIG	_IOR('B',114, u_int)
#define BIOCSRSIG	_IOW('B',115, u_int)
#define BIOCGHDRCMPLT	_IOR('B',116, u_int)
#define BIOCSHDRCMPLT	_IOW('B',117, u_int)
#if 0
#define BIOCGDIRECTION	_IOR('B',118, u_int)	/* FreeBSD */
#define BIOCSDIRECTION	_IOW('B',119, u_int)	/* FreeBSD */
#endif
#define BIOCSDLT	_IOW('B',118, u_int)
#define BIOCGDLTLIST	_IOR('B',119, struct bpf_dltlist)
//efine BIOCSDLT	_IOW('B',120, u_int)	/* FreeBSD */
//efine BIOCGDLTLIST	_IOR('B',121, struct bpf_dltlist)	/* FreeBSD */
#define BIOCGSEESENT	_IOR('B',120, u_int)	/* FreeBSD obsolete, set to BIOCGDIRECTION */
#define BIOCSSEESENT	_IOW('B',121, u_int)	/* FreeBSD obsolete, set to BIOCSDIRECTION */
#define BIOCSRTIMEOUT	_IOW('B',122, struct bpf_timeval)	/* OpenBSD */
#define BIOCGRTIMEOUT	_IOR('B',123, struct bpf_timeval)	/* OpenBSD */
#define BIOCLOCK	_IO('B', 122)	/* FreeBSD */
//efine BIOCSETWF	_IOW('B', 123, struct bpf_program)	/* FreeBSD */
//efine BIOCFEEDBACK	_IOW('B', 124, u_int)	/* FreeBSD */
#define BIOCGFEEDBACK	_IOR('B',124, u_int)
#define BIOCSFEEDBACK	_IOW('B',125, u_int)
//efine BIOCFEEDBACK	BIOCSFEEDBACK	/* OpenBSD */
#define BIOCGETBUFMODE	_IOR('B',125, u_int)	/* FreeBSD */
#define BIOCSETBUFMODE	_IOW('B',126, u_int)	/* FreeBSD */
#define BIOCGETZMAX	_IOR('B',127, size_t)	/* FreeBSD */
#define BIOCROTZBUF	_IOR('B',128, struct bpf_zbuf)	/* FreeBSD */
#define BIOCSETZBUF	_IOW('B',129, struct bpf_zbuf)	/* FreeBSD */
#define BIOCSETFNR	_IOW('B',130, struct bpf_program)	/* FreeBSD */
#define BIOCGTSTAMP	_IOR('B',131, u_int)	/* FreeBSD */
#define BIOCSTSTAMP	_IOR('B',132, u_int)	/* FreeBSD */
#endif

/* packet direction for use with BIOC[GS]DIRECTION */
enum bpf_direction {
	BPF_D_IN,			/* see incoming packets */
#define BPF_D_IN	BPF_D_IN
	BPF_D_INOUT,			/* see incoming and outgoing packets */
#define BPF_D_INOUT	BPF_D_INOUT
	BPF_D_OUT			/* see outgoing packets */
#define BPF_D_OUT	BPF_D_OUT
};

enum bpf_dirfilt {
	BPF_DIRECTION_NONE, /* do not filter */
#define BPF_DIRECTION_NONE	BPF_DIRECTION_NONE
	BPF_DIRECTION_IN, /* filter incoming packets */
#define BPF_DIRECTION_IN	BPF_DIRECTION_IN
	BPF_DIRECTION_OUT /* filter outgoing packets */
#define BPF_DIRECTION_OUT	BPR_DIRECTION_OUT
};

/* Time stamping functions for use with BIOC[GS]STAMP */
#define BPF_T_MICROTIME		0x0000
#define BPF_T_NANOTIME		0x0001
#define BPF_T_BINTIME		0x0002
#define BPF_T_NONE		0x0003
#define BPF_T_FORMAT_MASK	0x0003
#define BPF_T_FORMAT(t)		((t) & BPF_T_FORMAT_MASK)

#define BPF_T_NORMAL		0x0000
#define BPF_T_FAST		0x0100
#define BPF_T_MONOTONIC		0x0200
#define BPF_T_MONOTONIC_FAST	(BPF_T_FAST|BPF_T_MONOTONIC)
#define BPF_T_FLAG_MASK		0x0300
#define BPF_T_FLAG(t)		((t) & BPF_T_FLAG_MASK)

#define BPF_T_VALID(t) \
	((t) == BPF_T_NONE || (BPF_T_FORMAT(t) != BPF_T_NONE && \
	((t) & ~(BPF_T_FORMAT_MASK | BPF_T_FLAG_MASK)) == 0))

#define BPF_T_MICROTIME_FAST		(BPF_T_MICROTIME|BPF_T_FAST)
#define BPF_T_NANOTIME_FAST		(BPF_T_NANOTIME|BPF_T_FAST)
#define BPF_T_BINTIME_FAST		(BPF_T_BINTIME|BPF_T_FAST)
#define BPF_T_MICROTIME_MONOTONIC	(BPF_T_MICROTIME|BPF_T_MONOTONIC)
#define BPF_T_NANOTIME_MONOTONIC	(BPF_T_NANOTIME|BPF_T_MONOTONIC)
#define BPF_T_BINTIME_MONOTONIC		(BPF_T_BINTIME|BPF_T_MONOTONIC)
#define BPF_T_MICROTIME_MONOTONIC_FAST	(BPF_T_MICROTIME|BPF_T_MONOTONIC_FAST)
#define BPF_T_NANOTIME_MONOTONIC_FAST	(BPF_T_NANOTIME|BPF_T_MONOTONIC_FAST)
#define BPF_T_BINTIME_MONOTONIC_FAST	(BPF_T_BINTIME|BPF_T_MONOTONIC_FAST)

struct bpf_ts {
	int64_t bt_sec;			/* seconds */
	u_int64_t bt_frac;		/* fraction */
};
struct bpf_xhdr {
	struct bpf_ts bh_tstamp;	/* time stamp */
	u_int32_t bh_caplen;		/* length of captured portion */
	u_int32_t bh_datalen;		/* original length of packet */
	u_int16_t bh_hdrlen;		/* length of bpf header plus alignment padding */
} __attribute__ ((packed, aligned(2)));

/*
 * Structure prepended to each packet.  This is "wire" format, so we cannot
 * change it unfortunately to 64 bit times on 32 bit systems [yet].
 */
struct bpf_hdr {
	struct bpf_timeval bh_tstamp;	/* time stamp */
	u_int32_t bh_caplen;		/* length of captured portion */
	u_int32_t bh_datalen;		/* original length of packet */
	u_int16_t bh_hdrlen;		/* length of bpf header (this struct plus alignment
					   padding) */
} __attribute__ ((packed, aligned(2)));

/*
 * When using zero-copy BPF buffers, a shared memory header is present allowing
 * the kernel BPF implementation and user process to syncrhonize without using
 * system calls.  This structure defines that header.  WHen accessing these
 * fields, appropriate atomic operation and memory barriers are required in
 * order not to see stale or out-of-order data; see bpf(4) for reference code to
 * access these fields from userspace.
 *
 * The layout of this structure is critical, and must not be changed; it must
 * fit in a single page on all architectures.
 */
struct bpf_zbuf_header {
	volatile u_int bzh_kernel_gen;	/* kernel generation number */
	volatile u_int bzh_kerhel_len;	/* length of data in the buffer */
	volatile u_int bzh_user_gen;	/* user generation number */
	u_int _bzh_pad[5];
};

/*
 * Data-link level type codes.
 * Currently, only DLT_EN10MB and DLT_SLIP are supported.
 */
#define DLT_NULL			  0	/* no link-layer encapsulation */
#define DLT_EN10MB			  1	/* Ethernet (10Mb) */
#define DLT_EN3MB			  2	/* Experimental Ethernet (3Mb) */
#define DLT_AX25			  3	/* Amateur Radio AX.25 */
#define DLT_PRONET			  4	/* Proteon ProNET Token Ring */
#define DLT_CHAOS			  5	/* Chaos */
#define DLT_IEEE802			  6	/* IEEE 802 Networks */
#define DLT_ARCNET			  7	/* ARCNET */
#define DLT_SLIP			  8	/* Serial Line IP */
#define DLT_PPP				  9	/* Point-to-point Protocol */
#define DLT_FDDI			 10	/* FDDI */
#define DLT_ATM_RFC1483			 11	/* LLC/SNAP encapsulated atm */
#define DLT_RAW				 12	/* raw IP */

#define DLT_HIPPI			 15	/* HIPPI */
#define DLT_HDLC			 16	/* HDLC framing */
#define DLT_SLIP_BSDOS			 15	/* BSD/OS Serial Line IP */
#define DLT_PPP_BSDOS			 16	/* BSD/OS PPP */

#define DLT_ATM_CLIP			 19
#define DLT_REDBACK_SMARTEDGE		 32
#define DLT_PPP_SERIAL			 50
#define DLT_PPP_ETHER			 51
#define DLT_SYMANTEC_FIREWALL		 99
#define DLT_C_HDLC			104
#define DLT_CHDLC			DLT_C_HDLC
#define DLT_IEEE802_11			105
#define DLT_FRELAY			107
#define DLT_LOOP			108
#define DLT_ENC				109
#define DLT_LINUX_SLL			113
#define DLT_LTALK			114
#define DLT_ECONET			115
#define DLT_IPFILTER			116
#define DLT_PFLOG			118
#define DLT_PRISM_HEADER		119
#define DLT_AIRONET_HEADER		120
#define DLT_PFSYNC			121
#define DLT_HHDLC			121
#define DLT_IP_OVER_FC			122
#define DLT_SUNATM			123
#define DLT_RIO				124
#define DLT_PCI_EXP			125
#define DLT_AURORA			126
#define DLT_IEEE802_11_RADIO		127
#define DLT_TZSP			128
#define DLT_ARCNET_LINUX		129
#define DLT_JUNIPER_MLPPP		130
#define DLT_JUNIPER_MLFR		131
#define DLT_JUNIPER_ES			132
#define DLT_JUNIPER_GGSN		133
#define DLT_JUNIPER_MFR			134
#define DLT_JUNIPER_ATM2		135
#define DLT_JUNIPER_SERVICES		136
#define DLT_JUNIPER_ATM1		137
#define DLT_APPLE_IP_OVER_IEEE1394	138
#define DLT_MTP2_WITH_PHDR		139	/* pseudo-header with various info, followed by
						   MTP2 */
#define DLT_MTP2			140	/* MTP2, without pseudo-header */
#define DLT_MTP3			141	/* MTP3, without pseudo-header or MTP2 */
#define DLT_SCCP			142	/* SCCP, without pseudo-header or MTP2 or MTP3 */
#define DLT_DOCSIS			143
#define DLT_LINUX_IRDA			144
#define DLT_IBM_SP			145
#define DLT_IBM_SN			146
#define DLT_USER0			147
#define DLT_USER1			148
#define DLT_USER2			149
#define DLT_USER3			150
#define DLT_USER4			151
#define DLT_USER5			152
#define DLT_USER6			153
#define DLT_USER7			154
#define DLT_USER8			155
#define DLT_USER9			156
#define DLT_USER10			157
#define DLT_USER11			158
#define DLT_USER12			159
#define DLT_USER13			160
#define DLT_USER14			161
#define DLT_USER15			162
#define DLT_IEEE802_11_RADIO_AVS	163	/* 802.11 plus AVS radio header */
#define DLT_JUNIPER_MONITOR		164
#define DLT_BACNET_MS_TP		165
#define DLT_PPP_PPPD			166
#define DLT_PPP_WITH_DIRECTION		DLT_PPP_PPPD
#define DLT_LINUX_PPP_WITHDIRECTION	DLT_PPP_PPPD
#define DLT_JUNIPER_PPPOE		167
#define DLT_JUNIPER_PPPOE_ATM		168
#define DLT_GPRS_LLC			169	/* GPRS LLC */
#define DLT_GPF_T			170	/* GPF-T (ITU-T G.7041/Y.1303) */
#define DLT_GPF_F			171	/* GPF-F (ITU-T G.7041/Y.1303) */
#define DLT_GCOM_T1E1			172
#define DLT_GCOM_SERIAL			173
#define DLT_JUNIPER_PIC_PEER		174
#define DLT_ERF_ETH			175	/* Ethernet */
#define DLT_ERF_POS			176	/* Packet-over-SONET */
#define DLT_LINUX_LAPD			177
#define DLT_JUNIPER_ETHER		178
#define DLT_JUNIPER_PPP			179
#define DLT_JUNIPER_FRELAY		180
#define DLT_JUNIPER_CHDLC		181
#define DLT_MFR				182
#define DLT_JUNIPER_VP			183
#define DLT_A429			184
#define DLT_A653_ICM			185
#define DLT_USB				186
#define DLT_BLUETOOTH_HCI_H4		187
#define DLT_IEEE802_16_MAC_CPS		188
#define DLT_USB_LINUX			189
#define DLT_CAN20B			190
#define DLT_IEEE802_15_4_LINUX		191
#define DLT_PPI				192
#define DLT_IEEE802_16_MAC_CPS_RADIO	193
#define DLT_JUNIPER_ISM			194
#define DLT_IEEE802_15_4		195
#define DLT_SITA			196
#define DLT_ERF				197
#define DLT_RAIF1			198
#define DLT_IPMB			199
#define DLT_JUNIPER_ST			200
#define DLT_BLUETOOTH_HCI_H4_WITH_PHDR	201
#define DLT_AX25_KISS			202
#define DLT_LAPD			203
#define DLT_PPP_WITH_DIR		204	/* PPP - don't confuse with DLT_PPP_WITH_DIRECTION */
#define DLT_C_HDLC_WITH_DIR		205	/* Cisco HDLC */
#define DLT_FRELAY_WITH_DIR		206	/* Frame Relay */
#define DLT_LAPB_WITH_DIR		207	/* LAPB */
#define DLT_IPMB_LINUX			209
#define DLT_FLEXRAY			210
#define DLT_MOST			211
#define DLT_LIN				212
#define DLT_X2E_SERIAL			213
#define DLT_X2E_XORAYA			214
#define DLT_IEEE802_15_4_NONASK_PHY	215
#define DLT_LINUX_EVDEV			216
#define DLT_GSMTAP_UM			217
#define DLT_GSMTAP_ABIS			218
#define DLT_MPLS			219
#define DLT_USB_LINUX_MMAPPED		220
#define DLT_DECT			221
#define DLT_AOS				222
#define DLT_WIHART			223
#define DLT_FC_2			224
#define DLT_FC_2_WITH_FRAME_DELIMS	225
#define DLT_IPNET			226
#define DLT_CAN_SOCKETCAN		227
#define DLT_IPV4			228
#define DLT_IPV6			229
#define DLT_IEEE802_15_4_NOFCS		230
#define DLT_DBUS			231
#define DLT_JUNIPER_VS			232
#define DLT_JUNIPER_SRX_E2E		233
#define DLT_JUNIPER_FIBRECHANNEL	234
#define DLT_DVB_CI			235
#define DLT_MUX27010			236
#define DLT_STANAG_5066_D_PDU		237
#define DLT_JUNIPER_ATM_CEMIC		238

/* PUll in data-link level type codes. */
//#include <net/dlt.h>

/*
 * The instruction encodings.
 */
/* instruction classes */
#define BPF_CLASS(code) ((code) & 0x07)
#define		BPF_LD		0x00
#define		BPF_LDX		0x01
#define		BPF_ST		0x02
#define		BPF_STX		0x03
#define		BPF_ALU		0x04
#define		BPF_JMP		0x05
#define		BPF_RET		0x06
#define		BPF_MISC	0x07

/* ld/ldx fields */
#define BPF_SIZE(code)	((code) & 0x18)
#define		BPF_W		0x00
#define		BPF_H		0x08
#define		BPF_B		0x10
#define BPF_MODE(code)	((code) & 0xe0)
#define		BPF_IMM 	0x00
#define		BPF_ABS		0x20
#define		BPF_IND		0x40
#define		BPF_MEM		0x60
#define		BPF_LEN		0x80
#define		BPF_MSH		0xa0

/* alu/jmp fields */
#define BPF_OP(code)	((code) & 0xf0)
#define		BPF_ADD		0x00
#define		BPF_SUB		0x10
#define		BPF_MUL		0x20
#define		BPF_DIV		0x30
#define		BPF_OR		0x40
#define		BPF_AND		0x50
#define		BPF_LSH		0x60
#define		BPF_RSH		0x70
#define		BPF_NEG		0x80
#define		BPF_JA		0x00
#define		BPF_JEQ		0x10
#define		BPF_JGT		0x20
#define		BPF_JGE		0x30
#define		BPF_JSET	0x40
#define BPF_SRC(code)	((code) & 0x08)
#define		BPF_K		0x00
#define		BPF_X		0x08

/* ret - BPF_K and BPF_X also apply */
#define BPF_RVAL(code)	((code) & 0x18)
#define		BPF_A		0x10

/* misc */
#define BPF_MISCOP(code) ((code) & 0xf8)
#define		BPF_TAX		0x00
#define		BPF_TXA		0x80

/*
 * The instruction data structure.
 */
struct bpf_insn {
	u_short code;
	u_char jt;
	u_char jf;
	u_int k;
};

/*
 * Macros for insn array initializers.
 */
#define BPF_STMT(code, k) (struct bpf_insn){ (u_short)(code), 0, 0, k }
#define BPF_JUMP(code, k, jt, jf) (struct bpf_insn){ (u_short)(code), jt, jf, k }

/*
 * Structure to retrieve available DLTs for the interface.
 */
struct bpf_dltlist {
	u_int bfl_len;			/* number of bfl_list array elements */
	u_int *bfl_list;		/* array of DLTs */
};

#ifdef __LP64__
#ifdef __KERNEL__
/*
 * This structure is only useful within the kernel for performing conversion
 * from 32-bit users to the 64-bit kernel.
 */
struct bpf_dltlist32 {
	u_int32_t bfl_len;
	u_int32_t bfl_list;
};
#endif				/* __KERNEL__ */
#endif				/* __LP64__ */

#ifdef KERNEL
int bpf_validate __P((struct bpf_insn *, int));
int bpfopen __P((dev_t, int));
int bpfclose __P((dev_t, int));
int bpfread __P((dev_t, struct uio *));
int bpfwrite __P((dev_t, struct uio *));
int bpfioctl __P((dev_t, int, caddr_t, int));
int bpf_select __P((dev_t, int, struct proc *));
void bpf_tap __P((caddr_t, u_char *, u_int));
void bpf_mtap __P((caddr_t, struct mbuf *));
void bpfattach __P((caddr_t *, struct ifnet *, u_int, u_int));
void bpfilterattach __P((int));
u_int bpf_filter __P((struct bpf_insn *, u_char *, u_int, u_int));
#endif

#ifdef __KERNEL__

#ifndef __EXTERN
#define __EXTERN extern
#endif

#ifndef __STREAMS_EXTERN
#define __STREAMS_EXTERN __EXTERN streams_fastcall
#endif

__STREAMS_EXTERN int register_bpf(struct cdevsw *cdev);
__STREAMS_EXTERN int unregister_bpf(struct cdevsw *cdev);

#define BPF_NODE    2

#endif				/* __KERNEL__ */

/*
 * Number of scratch memory words (for BPF_LD|BPF_MEM and BPF_ST).
 */
#define BPF_MEMWORDS 16

#endif				/* _NET_BPF_H_ */
