/*****************************************************************************

 @(#) $RCSfile: ldterm.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/10/12 09:37:42 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
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

 Last Modified $Date: 2006/10/12 09:37:42 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ldterm.c,v $
 Revision 0.9.2.2  2006/10/12 09:37:42  brian
 - completed much of the strtty package

 Revision 0.9.2.1  2006/09/29 11:40:07  brian
 - new files for strtty package and manual pages

 *****************************************************************************/

#ident "@(#) $RCSfile: ldterm.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/10/12 09:37:42 $"

static char const ident[] =
    "$RCSfile: ldterm.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/10/12 09:37:42 $";

/*
 * This is ldterm (line discipline terminal) module.
 *
 * This is just a skeleton, for now.
 */
/*
 * LDTERM Module
 * 
 * The ldterm module is a key part of the STREAMS-based TTY subsystem, supplying the line discipline
 * for terminal devices. This line discipline is POSIX-compliant and also supports the System V and
 * BSD interfaces. The ldterm module provides the terminal interface functions specified in the
 * termios.h header file. The ldterm module also handles EUC and multibyte characters.
 * 
 * The ldterm module processes various types of STREAMS messages, as listed in the [9]Messages
 * Summary. Any other message received by ldterm is passed unchanged, either downstream or upstream.
 */

/*
 * Multibyte Processing
 * 
 * The ldterm module handles the extended UNIX code (EUC) character-encoding scheme. This encoding
 * scheme enables the module to process multibyte characters as well as single-byte characters. It
 * correctly handles backspacing and tabulation expansion for multibyte characters.
 * 
 * By default, multibyte processing by the ldterm module is turned off.  When ldterm receives an
 * EUC_.... IOCTL call that sets character width on screen to a value greater than one, it enables
 * multibyte processing. When multibyte processing is turned on, the ldterm module automatically
 * calls EUC routines as necessary.
 * 
 * Messages Summary for the LDTERM Module
 * 
 * Messages include read-side messages and write-side messages and are listed as follows:
 * 
 * Read-Side Messages 
 * Messages processed by the ldterm module:
 * 
 * + M_BREAK
 * + M_CTL
 * + M_DATA
 * + M_FLUSH
 * + M_HANGUP
 * + M_IOCACK
 * + M_LETSPLAY
 * 
 * Messages sent by the ldterm module upstream:
 * 
 * + M_CTL
 * + M_DATA
 * + M_ERROR
 * + M_FLUSH
 * + M_HANGUP
 * + M_IOCACK
 * + M_IOCNAK
 * + M_PCSIG
 * + M_SETOPTS
 * 
 * Write-Side Messages 
 * Messages processed by the ldterm module:
 * 
 * + M_CTL
 * + M_DATA
 * + M_FLUSH
 * + M_IOCTL
 * + M_READ
 * + M_START
 * + M_STARTI
 * + M_STOP
 * + M_STOPI
 * + M_NOTIFY
 * 
 * Messages sent by ldterm downstream:
 * 
 * + M_BREAK
 * + M_CTL
 * + M_DATA
 * + M_DELAY
 * + M_FLUSH
 * + M_IOCTL
 * + M_STOP
 * + M_START
 * + M_STOPI
 * + M_STARTI
 * _________________________________________________________________
 */

#include <sys/os7/compat.h>

#include <linux/termios.h>
#include <termios.h>
#include <sys/strtty.h>

#define LDTERM_DESCRIP		"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define LDTERM_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define LDTERM_REVISION		"OpenSS7 $RCSfile: ldterm.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/10/12 09:37:42 $"
#define LDTERM_DEVICE		"SVR 4.2 STREAMS Line Discipline Module (LDTERM)"
#define LDTERM_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define LDTERM_LICENSE		"GPL"
#define LDTERM_BANNER		LDTERM_DESCRIP		"\n" \
				LDTERM_COPYRIGHT	"\n" \
				LDTERM_REVISION		"\n" \
				LDTERM_DEVICE		"\n" \
				LDTERM_CONTACT
#define LDTERM_SPLASH		LDTERM_DEVICE		" - " \
				LDTERM_REVISION

#ifdef LINUX
MODULE_AUTHOR(LDTERM_CONTACT);
MODULE_DESCRIPTION(LDTERM_DESCRIP);
MODULE_SUPPORTED_DEVICE(LDTERM_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(LDTERM_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-ldterm");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifndef CONFIG_STREAMS_LDTERM_NAME
#error CONFIG_STREAMS_LDTERM_NAME must be defined.
#endif
#ifndef CONFIG_STREAMS_LDTERM_MODID
#error CONFIG_STREAMS_LDTERM_MODID must be defined.
#endif

modID_t modid = CONFIG_STREAMS_LDTERM_MODID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, short, 0);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module Id for LDTERM (0 for allocation).");

#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_LDTERM_MODID));
MODULE_ALIAS("streams-module-ldterm");
#endif				/* LFS */
#endif				/* MODULE_ALIAS */

static struct module_info ldterm_minfo = {
	.mi_idnum = CONFIG_STREAMS_LDTERM_MODID,
	.mi_idname = CONFIG_STREAMS_PCKT_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat ldterm_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat ldterm_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

#if defined __LP64__ && defined LFS
#   undef WITH_32BIT_CONVERSION
#   define WITH_32BIT_CONVERSION 1
#endif

/*
 *  Private Structure.
 */

struct ldterm {
};

#define LDTERM_PRIV(__q)	((struct ldterm *)((__q)->q_ptr))

/**
 * ldterm_rput - STREAMS read-side put procedure.
 * @q: read queue
 * @mp: message to put
 */
/*
 * Read-Side Put Routine
 * 
 * The ldterm read-side put routine processes the following STREAMS messages coming from downstream
 * modules or driver:
 * 
 * M_BREAK 
 *	The ldterm module checks if there are any messages left on its read queue.  If there are
 *	messages, the M_BREAK message gets queued. The M_BREAK message provides the following status
 *	information:
 * 
 *	    break_interrupt parity_error framing_error or overrun
 * 
 * M_CTL 
 *	If the M_CTL message cannot be handled due to memory constraints, this message is queued for
 *	later processing.
 * 
 *	The M_CTL messages received on the read-side and processed by the ldterm module are sent by
 *	the driver for different reasons:
 * 
 *	+   The M_CTL message can be sent to communicate changes in the driver's state. If the
 *	    CLOCAL flag of ldterm is not set, and the carrier state has just made a transition from
 *	    on to off, the read-side put routine sends an M_HANGUP message upstream to inform the
 *	    stream head that the terminal connection was broken.
 *
 *	+   The M_CTL message can be sent to answer to a previous request or command from the ldterm
 *	    module. The following command contained in an M_CTL message is processed:
 * 
 *	    TIOCGETA 
 *		The driver sends this command either as a response to an inquiry for current
 *		settings or to reflect an asynchronous change in the flags of its termios structure.
 * 
 *		The read-side put routine copies the termios structure from the attached M_DATA
 *		message block into its internal termios structure. It then frees the M_CTL message.
 * 
 * 
 * 
 * M_DATA 
 *	All M_DATA messages are queued for deferred processing by the service routine.
 * 
 * M_FLUSH 
 *	This is a request to flush the read-side or the write-side queue of all its data messages
 *	and all data being accumulated.  The read-side put routine processes the request and
 *	forwards the message upstream.
 * 
 *	The queue to be flushed (read-side or write-side) is determined by the M_FLUSH parameter
 *	(FLUSHR or FLUSHW ).
 * 
 * M_IOCACK 
 *	If the M_IOCACK messages cannot be handled due to memory constraints, this message is queued
 *	for later processing.
 * 
 *	The M_IOCACK message signals a positive acknowledgment of a previous M_IOCTL message.
 * 
 *	+   If the M_IOCACK message acknowledges the TIOCGETA, TIOCSETA, TIOCSETAW, or TIOCSETAF
 *	    commands,the termios structure is updated as specified in the commands.
 *
 *	+   If the message acknowledges a TIOCOUTQ command, the required number of bytes are added
 *	    to the reply value in the M_IOCACK message.
 *
 *	+   In all other cases, the message is sent upstream.
 * 
 * MC_DO_CANON 
 *	All input processing performed on M_DATA messages is enabled.
 * 
 * MC_NO_CANON 
 *	The input canonical processing normally performed on M_DATA messages is disabled and those
 *	messages are passed upstream unmodified. This is
 *	used by modules or drivers that perform their own input processing (for example, a pseudo
 *	terminal in TIOCREMOTE mode connected to a program that performs the input processing).
 * 
 * MC_PART_CANON 
 *	The driver sends this message to notify the ldterm module that it handles some part of the
 *	input processing itself (for example, flow control). An M_DATA message containing a termios
 *	structure is expected to be attached to the original M_CTL message. The ldterm module will
 *	examine the c_iflag, c_oflag and c_lflag fields of the termios structure and processes only
 *	those flags that have not been turned On.
 * 
 * TCGETX 
 *	The driver sends this message to communicate its termiox structure.
 * 
 * TIOCGETMODEM 
 *	The driver sends this message to communicate the state of its flag modem carrier. The
 *	associated M_DATA message contains a value of 1 (one) to indicate the carrier is on or a
 *	value of 0 (zero) to indicate the carrier is off. This information is used to update the
 *	ldterm module state. If the CLOCAL flag of the ldterm module is not set, and the carrier
 *	state has just made a transition from on to off, the read-side put routine sends an M_HANGUP
 *	message upstream to inform the stream head that the terminal connection was broken.
 * 
 * TIOCSETA 
 *	The driver sends this message to communicate its termios structure.
 * 
 * TIOCSWINSZ 
 *	The driver sends this message to communicate its window size.
 * 
 * TXSETIHOG 
 *	The driver generates these messages to change the IHOG settings.
 * 
 * TXSETOHOG 
 *	The driver generates these messages to change the OHOG settings.
 * 
 * TXTTYNAME 
 *	The driver sends this message to communicate its TTY name.
 * 
 *	When the command is processed, the M_CTL message is freed.
 * 
 * The M_CTL message can be sent to answer to the TIOC_REQUEST coming from the tioc module with a
 * TIOC_REPLY. This M_CTL message is forwarded upstream.
 * 
 * All other messages are forwarded upstream. These include the following:
 * 
 * M_LETSPLAY 
 *	This is a high priority message sent by a lower module or driver to register direct I/O
 *	entry points with the stream head. An integer pointer is included in this message. The
 *	ldterm module increments this integer if it permits direct I/O processing. This message
 *	should be passed upstream to the stream head.
 * 
 * M_HANGUP 
 *	A driver can put the stream in hangup mode by sending this message.  Upon receiving this
 *	message, the ldterm module sets the stream to return errors on writes, putmsg, and ioctls,
 *	with errno set to EIO or ENXIO.
 */
static streamscall int
ldterm_rput(queue_t *q, mblk_t *mp)
{
	return (0);
}

/**
 * ldterm_rsrv - STREAMS read-side service procedure.
 * @q: read queue to service
 */
/*
 * Read-Side Service Routing: Delayed Processing
 * 
 * The read-side service routine processes messages that can be delayed due to STREAMS flow control
 * or to ioctls requiring sequential processing. The read-side service routing is called by the
 * scheduler and processes the following commands:
 * 
 * M_BREAK 
 *	See [10]M_BREAK message in Read-Side Put Routine processing.
 * 
 * M_DATA 
 *	The read-side service routine processes the M_DATA messages and performs various actions
 *	according to the character found in the data and the setting of the termios flags:
 * 
 *	+   The read-side put routine generates echo characters, which are sent downstream in M_DATA
 *	    messages.
 *
 *	+   The ldterm module can control the output flow of data. If the IXON flag is set and if
 *	    the ldterm module deals with the flow control, the read-side put routine processes START
 *	    (VSTART) and STOP (V_STOP) characters and sends M_START and M_STOP messages downstream.
 *
 *	+   The ldterm module can control the input flow of data. If the IXOFF flag is set and input
 *	    is to be stopped or started, the read-side put routine generates M_STOPI and M_STARTI
 *	    messages downstream.
 *
 *	+   If the ISIG flag is active, the read-side put routine manages signal characters. It
 *	    sends MPCSIG messages upstream when signal characters are encountered and then discards
 *	    these characters.
 *
 *	+   At the logical termination of input, and if a read request is outstanding, the read-side
 *	    put routine sends the currently buffered characters upstream to the stream head. The
 *	    logical termination characters are NEWLINE, EOF, EOL, and EOL2. If the ICANON flag is
 *	    not set, the ldterm module is in noncanonical or raw input mode. In this case, the input
 *	    terminates when at least VMIN bytes are present in the input message buffer or when the
 *	    timer specified by VTIME expires.  However, the M_DATA messages are forwarded upstream
 *	    only if a read is outstanding, meaning that an M_READ message has been received from the
 *	    stream head. The M_DATA is sent if the read can be satisfied.
 * 
 * M_IOCACK 
 *	See [11]M_IOCACK message in Read-Side Put Routine processing.
 * 
 * M_CTL 
 *	See [12]M_CTL message in Read-Side Put Routine processing.
 */
static streamscall int
ldterm_rsrv(queue_t *q)
{
	return (0);
}

/**
 * ldterm_wput - STREAMS write-side put procedure.
 * @q: write queue
 * @mp: message to put
 */
/*
 * Write-Side Put Routine: Immediate Processing
 * 
 * The ldterm write-side put routine immediately processes the following STREAMS messages:
 * 
 * M_FLUSH 
 *	The write-side put routine flushes the read-side or the write-side queue and discards any
 *	buffered output data. It then forwards the message downstream. The queue to be flushed
 *	(read-side or write-side) is determined by the M_FLUSH parameter (FLUSHR or FLUSHW).
 * 
 * M_DATA 
 *	If the write-side queue is empty, the write-side put routine processes the M_DATA message.
 *	Else, it queues the M_DATA message to the write-side queue for later processing by the
 *	write-side service routine.
 * 
 * M_IOCTL 
 *	The write-side put routine validates the format of the M_IOCTL message and checks for known
 *	IOCTL command:
 * 
 *	+   If the message format is not valid, it converts the M_IOCTL message into an MIOCNAK
 *	    message,and returns it upstream.
 *
 *	+   If the IOCTL command is not recognized, it forwards the M_IOCTL message downstream for
 *	    processing by other modules.
 *
 *	+   If the IOCTL is recognized, the write-side put routine determines if the command must be
 *	    processed in the correct sequence relative to M_DATA messages. If so, it queues the
 *	    M_IOCTL message to the write-side queue for later processing.The commands that require
 *	    processing in sequence are as follows:
 *
 *               1. TIOCSETAW
 *               2. TIOCSETAF
 *               3. TCSETAW
 *               4. TCSETAF
 *               5. TCSBRK
 *               6. TCSETXW
 *               7. TCSETXF
 *               8. TCSBREAK
 * 
 *	Otherwise, the write-side put routine processes the command immediately.
 * 
 * M_READ 
 *	The M_READ message is sent by the stream head to notify downstream modules when an
 *	application has issued a read request and not enough data is queued at the stream head to
 *	satisfy the request. The message contains the number of characters requested by the
 *	application. If the request can be satisfied, the M_READ message block is transformed into
 *	an M_DATA block with the requested data appended. This message is then forwarded upstream.
 * 
 * M_START, M_STOP, M_STARTI, M_STOPI 
 *	Some IOCTLs commands (TCXONC with one parameter among TCOON, TCOOF, TCION, TCIOF) are issued
 *	by the application to control the flow of data. The blocked data is stored in the module's
 *	queues. To process these IOCTLs, the stream head generates and sends downstream the
 *	following high-priority messages:
 * 
 *           + M_START to restart output of data
 *           + M_STOP to stop output of data
 *           + M_STARTI to restart input of data
 *           + M_STOPI to stop input of data.
 * 
 *	The ldterm write-side put routine updates internal state fields and forwards these messages
 *	downstream.
 * 
 * M_BACKDONE 
 *	This message is one of many messages used when a module or driver tries to register direct
 *	I/O entry points with the stream head. Upon receiving this message, the ldterm module knows
 *	all messages from upstream have been cleared. The ldterm module should send down all queued
 *	data downstream as M_BACKWASH messages, and finally forwarding the M_BACKDONE message
 *	downstream.
 * 
 * M_PCTTY 
 *	High priority message type used for communication between the ldterm module and stream head.
 *	Used by the stream head to inform the ldterm module when an event, such as read or ioctl has
 *	been aborted by user signal (timer or ctrl/c). The ldterm module must remove message block
 *	representing aborted read or ioctl from queue and free it. A pointer to the original message
 *	block that was aborted is included in this message.
 * 
 * M_CTL 
 *	This type of message is for internal communication among the modules and driver in the
 *	stream. The ldterm module does not understand a command; it simply forwards the message
 *	downstream. The following command is processed:
 * 
 *	TIOCSTI 
 *	    Sent from an upperstream character conversion module to simulate a typein command.
 */

static streamscall int
ldterm_wput(queue_t *q, mblk_t *mp)
{
	return (0);
}

/**
 * ldterm_wsrv - STREAMS write-side service procedure.
 * @q: write queue to service
 */
/*
 * Write-Side Service Routine: Delayed Processing
 * 
 * The write-side service routine processes messages that can be delayed due to STREAMS flow control
 * or to ioctls requiring sequential processing. The write-side service routine is called by the
 * scheduler and processes the following commands:
 * 
 * M_DATA 
 *	The write-side service routine processes the data according to the flags in the termios
 *	structure.
 * 
 * M_IOCTL 
 *	Some ioctl commands must wait until output is empty before they are processed. M_IOCTL
 *	messages containing these commands are queued on the write-side queue so that the write
 *	service routine processes them in the correct sequence relative to preceding data. The
 *	commands that require processing in sequence are as follows:
 * 
 *          1. TIOCSETAW
 *          2. TIOCSETAF
 *          3. TCSETAW
 *          4. TCSETAF
 *          5. TCSBRK
 *          6. TCSETXW
 *          7. TCSETXF
 *          8. TCSBREAK
 */
static streamscall int
ldterm_wsrv(queue_t *q)
{
	return (0);
}

/*
 * ---------------------------------------------------------------------------
 *
 * OPEN and CLOSE Routines.
 *
 * ---------------------------------------------------------------------------
 */

/**
 * ldterm_qopen - ldterm(4) STREAMS open routine
 * @q: read queue of opened Stream
 * @devp: pointer to device number opened
 * @oflag: flags to the open call
 * @sflag: STREAMS flag: DRVOPEN, MODOPEN or CLONEOPEN
 * @crp: pointer to opener's credentials
 *
 * Open Routine
 * 
 * When first called, the open routine allocates space for the ldterm internal structure,and also
 * sends an M_SETOPTS message upstream.This message includes a stroptions structure defined in the
 * stream.h file, which contains options that inform the stream head how touse this stream.
 * 
 * The M_SETOPTS message allows a driver or module to exercise control over certain stream head
 * processing. An M_SETOPTS message can be sent upstream at any time. The stream head responds to
 * the message by altering the processing associated with certain system calls. The options to be
 * modified are specified by the contents of the stroptions structure contained in the message.
 * 
 * The open routine allocates space for the termios structure,which contains the flags used to
 * control the terminal.The flags are defined in the termios.h file, as follows:
 *
 *    *  c_iflag defines input modes.
 *    *  c_oflag defines output modes.
 *    *  c_cflag defines hardware control modes.
 *    *  c_lflag defines terminal functions handled by the ldterm module.
 *    *  c_cc defines the control characters.
 * 
 * The open routine initializes the flags with default values.
 * 
 * When the ldterm module is pushed during stream initialization, it sends the following M_CTL
 * messages downstream:
 * 
 * MC_CANONQUERY 
 *	Queries the driver for which termios flags the driver will manage.  This M_CTL message has
 *	an M_DATA message block linked to it containing a termios structure. Upon receiving this
 *	message, the driver should turn on the bits in this structure for each flag it plans to
 *	manage.
 * 
 * TCGETX 
 *	Queries the driver for its default termios flags. The driver responds by passing back its
 *	termios settings in the attached M_DATA message block.
 * 
 * TIOCGETA 
 *	Queries the driver for its default termios flags. The driver should respond by passing back
 *	its termios settings in the attached M_DATA message block.
 * 
 * TIOCGETMODEM 
 *	Queries the driver for the current status of the Data Carrier Detect signal. The driver
 *	responds by putting the current state in the attached M_DATA message block.
 * 
 * TXTTYNAME 
 *	Queries the driver for its TTY name, the driver responds by passing back the TTY name in the
 *	attached M_DATA message block.
 * 
 * After receiving all needed information from the driver, the ldterm module sends an M_IOCTL
 * message, with the command TIOCSETA, along with a termios in the attached M_DATA message block.
 * The driver sets its attributes according to this structure.
 * 
 * Finally, the ldterm module registers direct entry points to the stream head by issuing the wantio
 * utility.
 *  */
static streamscall int
ldterm_qopen(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	return (ENXIO);
}

/**
 * ldterm_qclose - ldterm(4) STREAMS close routine
 * @q: read queue of closing Stream
 * @oflag: flags to open call
 * @crp: pointer to closer's credentials
 */
/*
 * Close Routine
 * 
 * The close routine sends an M_SETOPTS message upstream to undo stream head changes done on the
 * first open. The ldterm module also sends M_START messages downstream to undo the effects of any
 * previous M_STOP messages.
 * 
 * The close routine frees all the outstanding buffers allocated by the ldterm module.
 * 
 * The ldterm module is inactive while waiting for output to be drained.
 */
static streamscall int
ldterm_qclose(queue_t *q, int flags, cred_t *crp)
{
	return (ENXIO);
}

/*
 * ---------------------------------------------------------------------------
 *
 * Registration and Initialization.
 *
 * ---------------------------------------------------------------------------
 */

static struct qinit ldterm_rinit = {
	.qi_putp = ldterm_rput,
	.qi_srvp = ldterm_rsrv,
	.qi_qopen = ldterm_qopen,
	.qi_qclose = ldterm_qclose,
	.qi_minfo = &ldterm_minfo,
	.qi_mstat = &ldterm_rstat,
};

static struct qinit ldterm_winit = {
	.qi_putp = ldterm_wput,
	.qi_srvp = ldterm_wsrv,
	.qi_minfo = &ldterm_minfo,
	.qi_mstat = &ldterm_wstat,
};

static struct streamtab ldterm_info = {
	.st_rdinit = &ldterm_rinit,
	.st_wrinit = &ldterm_winit,
};

#ifdef LIS
#define fmodsw _fmodsw
#endif
static struct fmodsw ldterm_fmod = {
	.f_name = CONFIG_STREAMS_LDTERM_NAME,
	.f_str = &ldterm_info,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

#ifdef CONFIG_STREAMS_LDTERM_MODULE
static
#endif
int __init
ldterm_init(void)
{
	int err;

#ifdef CONFIG_STREAMS_LDTERM_MODULE
	cmn_err(CE_NOTE, LDTERM_BANNER);
#else
	cmn_err(CE_NOTE, LDTERM_SPLASH);
#endif
	ldterm_minfo.mi_idnum = modid;
	if ((err = register_strmod(&ldterm_fmod)) < 0)
		return (err);
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
};

#ifdef CONFIG_STREAMS_LDTERM_MODULE
static
#endif
void __exit
ldterm_exit(void)
{
	unregister_strmod(&ldterm_fmod);
};

/*
 * ---------------------------------------------------------------------------
 *
 * Linux Registration.
 *
 * ---------------------------------------------------------------------------
 */

#ifdef CONFIG_STREAMS_LDTERM_MODULE
module_init(ldterm_init);
module_exit(ldterm_exit);
#endif
