/*****************************************************************************

 @(#) $RCSfile: ttcompat.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/10/02 12:07:23 $

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

 Last Modified $Date: 2006/10/02 12:07:23 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ttcompat.c,v $
 Revision 0.9.2.2  2006/10/02 12:07:23  brian
 - working up compatibility module

 Revision 0.9.2.1  2006/09/29 11:40:07  brian
 - new files for strtty package and manual pages

 *****************************************************************************/

#ident "@(#) $RCSfile: ttcompat.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/10/02 12:07:23 $"

static char const ident[] =
    "$RCSfile: ttcompat.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/10/02 12:07:23 $";

/*
 * Terminal compatibility module.  Provides ioctl inteface to user.  Provides
 * Version 7 and 4BSD compatibility with SVR4 termio(7).
 *
 * OK, working on this module was a big waste of time.  Linux does not
 * currently support BSD or Version 7 compatibility anyway.  It uses only
 * POSIX compatible terminals, so ldterm should do.
 */

/*

   [1]DOC HOME [2]SITE MAP [3]MAN PAGES [4]GNU INFO [5]SEARCH 

                                  ttcompat(7)
     _________________________________________________________________

   ttcompat -- V7 and 4BSD STREAMS compatibility module 

Synopsis

   #include <sys/stream.h>
   #include <sys/stropts.h>
   #include <sys/ttold.h>

   ioctl(fd, I_PUSH, "ttcompat");

Description

   ttcompat is a STREAMS module that translates the ioctl calls supported
   by  the older Version 7 and 4BSD terminal drivers into the ioctl calls
   supported  by  the  termio  interface  (see  [6]termio(7)).  All other
   messages  pass through this module unchanged; the behavior of read and
   write calls is unchanged, as is the behavior of ioctl calls other than
   the ones supported by ttcompat.

   This  module  can  be  automatically  pushed  onto  a  stream with the
   [7]autopush(1M)  mechanism  when  a terminal device is opened; it does
   not  have  to be explicitly pushed onto a stream. This module requires
   that  the  termios  interface  be  supported  by  the  modules and the
   application  can  push  the driver downstream. The TCGETS, TCSETS, and
   TCSETSF  ioctl  calls  must  be  supported;  if any information set or
   fetched  by  those  ioctl  calls  is  not supported by the modules and
   driver downstream, some of the V7/4BSD functions may not be supported.
   For example, if the CBAUD bits in the c_cflag field are not supported,
   the  functions  provided  by the sg_ispeed and sg_ospeed fields of the
   sgttyb  structure  (see  below)  will  not be supported. If the TCFLSH
   ioctl  is  not supported, the function provided by the TIOCFLUSH ioctl
   will  not  be  supported.  If  the  TCXONC ioctl is not supported, the
   functions  provided by the TIOCSTOP and TIOCSTART ioctl calls will not
   be  supported.  If  the  TIOCMBIS  and  TIOCMBIC  ioctl  calls are not
   supported,  the  functions provided by the TIOCSDTR and TIOCCDTR ioctl
   calls will not be supported.

   The basic ioctl calls use the sgttyb structure defined by sys/ioctl.h:
   struct sgttyb {
        char    sg_ispeed;
        char    sg_ospeed;
        char    sg_erase;
        char    sg_kill;
        int     sg_flags;
   };

   The  sg_ispeed  and  sg_ospeed  fields  describe  the input and output
   speeds  of  the device, and reflect the values in the c_cflag field of
   the termios structure. The sg_erase and sg_kill fields of the argument
   structure  specify  the  erase  and  kill characters respectively, and
   reflect  the  values in the VERASE and VKILL members of the c_cc field
   of the termios structure.

   The  sg_flags  field  of the argument structure contains several flags
   that determine the system's treatment of the terminal. They are mapped
   into flags in fields of the terminal state, represented by the termios
   structure.

   Delay  type 0 is always mapped into the equivalent delay type 0 in the
   c_oflag  field  of  the  termios  structure.  Other delay mappings are
   performed as follows:
   [If this table is unreadable, a preformatted table follows]

   sg_flags c_oflag
   BS1      BS1
   FF1      VT1
   CR1      CR2
   CR2      CR3
   CR3      Not supported
   TAB1     TAB1
   TAB2     TAB2
   XTABS    TAB3
   NL1      ONLRET|CR1
   NL2      NL1

 sg_flags   c_oflag
 BS1        BS1
 FF1        VT1
 CR1        CR2
 CR2        CR3
 CR3        Not supported
 TAB1       TAB1
 TAB2       TAB2
 XTABS      TAB3
 NL1        ONLRET|CR1
 NL2        NL1

   If  previous TIOCLSET or TIOCLBIS ioctl calls have not selected LITOUT
   or PASS8 mode, and if RAW mode is not selected, the ISTRIP flag is set
   in  the c_iflag field of the termios structure, and the EVENP and ODDP
   flags  control  the  parity  of  characters  sent  to the terminal and
   accepted from the terminal:
     * Parity is not to be generated on output or checked on input:
       The  character  size  is set to CS8 and the flag is cleared in the
       c_cflag field of the termios structure.
     * Even  parity characters are to be generated on output and accepted
       on input:
       The flag is set in the c_iflag field of the termios structure, the
       character  size  is  set to CS7 and the flag is set in the c_cflag
       field of the termios structure.
     * Odd  parity  characters are to be generated on output and accepted
       on input:
       The flag is set in the c_iflag field, the character size is set to
       CS7  and the and flags are set in the c_cflag field of the termios
       structure.
     * Even   parity  characters  are  to  be  generated  on  output  and
       characters of either parity are to be accepted on input:
       The  flag  is  cleared in the c_iflag field, the character size is
       set to CS7 and the flag is set in the c_cflag field of the termios
       structure.

   The  RAW  flag  disables  all output processing (the OPOST flag in the
   c_oflag field, and the XCASE flag in the c_lflag field, are cleared in
   the  termios structure) and input processing (all flags in the c_iflag
   field  other than the IXOFF and IXANY flags are cleared in the termios
   structure).  8 bits of data, with no parity bit, are accepted on input
   and  generated  on  output;  the  character size is set to CS8 and the
   PARENB  and  PARODD  flags  are  cleared  in  the c_cflag field of the
   termios  structure.  The  signal-generating  and  line-editing control
   characters  are  disabled by clearing the ISIG and ICANON flags in the
   c_lflag field of the termios structure.

   The   CRMOD   flag   turns  input  <Return>  characters  into  newline
   characters, and output and echoed newline characters to be output as a
   <Return>  followed by a linefeed. The ICRNL flag in the c_iflag field,
   and  the  OPOST  and  ONLCR flags in the c_oflag field, are set in the
   termios structure.

   The  LCASE  flag maps upper-case letters in the ASCII character set to
   their  lower-case  equivalents  on input (the IUCLC flag is set in the
   c_iflag field), and maps lower-case letters in the ASCII character set
   to  their  upper-case  equivalents on output (the OLCUC flag is set in
   the  c_oflag  field).  Escape  sequences  are  accepted  on input, and
   generated  on output, to handle certain ASCII characters not supported
   by older terminals (the XCASE flag is set in the c_lflag field).

   Other flags are directly mapped to flags in the termios structure:
   [If this table is unreadable, a preformatted table follows]

   sg_flags Flags in termios structure
   CBREAK   Complement of ICANON in c_lflag field
   ECHO     ECHO in c_lflag field
   TANDEM   IXOFF in c_iflag field

 sg_flags   Flags in termios structure
 CBREAK     Complement of ICANON in c_lflag field
 ECHO       ECHO in c_lflag field
 TANDEM     IXOFF in c_iflag field

   Another  structure  associated with each terminal specifies characters
   that are special in both the old Version 7 and the newer 4BSD terminal
   interfaces. The following structure is defined by sys/ioctl.h:
*/

#if 0
struct tchars {
	char t_intrc;			/* interrupt */
	char t_quitc;			/* quit */
	char t_startc;			/* start output */
	char t_stopc;			/* stop output */
	char t_eofc;			/* end-of-file */
	char t_brkc;			/* input delimiter (like nl) */
};
#endif

/*

   Also  associated  with  each terminal is a local flag word, specifying
   flags  supported  by  the  new  4BSD terminal interface. Most of these
   flags are directly mapped to flags in the termios structure:
   [If this table is unreadable, a preformatted table follows]

   Local flags Flags in termios structure
   LCRTBS      Not supported
   LPRTERA     ECHOPRT in the c_lflag field
   LCRTERA     ECHOE in the c_lflag field
   LTILDE      Not supported
   LTOSTOP     TOSTOP in the c_lflag field
   LFLUSHO     FLUSHO in the c_lflag field
   LNOHANG     CLOCAL in the c_cflag field
   LCRTKIL     ECHOKE in the c_lflag field
   LCTLECH     CTLECH in the c_lflag field
   LPENDIN     PENDIN in the c_lflag field
   LDECCTQ     Complement of IXANY in the c_iflag field
   LNOFLSH     NOFLSH in the c_lflag field

   Local flags   Flags in termios structure
   LCRTBS        Not supported
   LPRTERA       ECHOPRT in the c_lflag field
   LCRTERA       ECHOE in the c_lflag field
   LTILDE        Not supported
   LTOSTOP       TOSTOP in the c_lflag field
   LFLUSHO       FLUSHO in the c_lflag field
   LNOHANG       CLOCAL in the c_cflag field
   LCRTKIL       ECHOKE in the c_lflag field
   LCTLECH       CTLECH in the c_lflag field
   LPENDIN       PENDIN in the c_lflag field
   LDECCTQ       Complement of IXANY in the
   c_iflag field
   LNOFLSH       NOFLSH in the c_lflag field

   Another  structure  associated  with  each  terminal  is  the  ltchars
   structure  which  defines control characters for the new 4BSD terminal
   interface. Its structure is:
*/

#if 0
struct ltchars {
	char t_suspc;			/* stop process signal */
	char t_dsuspc;			/* delayed stop process signal */
	char t_rprntc;			/* reprint line */
	char t_flushc;			/* flush output (toggles) */
	char t_werasc;			/* word erase */
	char t_lnextc;			/* literal next character */
};
#endif

/*
   The  characters are mapped to members of the c_cc field of the termios
   structure as follows:
   [If this table is unreadable, a preformatted table follows]

   ltchars  c_cc index
   t_suspc  VSUSP
   t_dsuspc VDSUSP
   t_rprntc VREPRINT
   t_flushc VDISCARD
   t_werasc VWERASE
   t_lnextc VLNEXT

   ltchars    c_cc index
   t_suspc    VSUSP
   t_dsuspc   VDSUSP
   t_rprntc   VREPRINT
   t_flushc   VDISCARD
   t_werasc   VWERASE
   t_lnextc   VLNEXT

   ioctl calls

   ttcompat  responds to the following ioctl calls. All others are passed
   to the module below.
   TIOCGETP
   The  argument  is a pointer to an sgttyb structure. The current
   terminal  state  is  fetched; the appropriate characters in the
   terminal  state  are stored in that structure, as are the input
   and  output  speeds.  The  values  of the flags in the sg_flags
   field  are  derived  from  the  flags in the terminal state and
   stored in the structure.
   TIOCEXCL
   Set  ``exclusive-use''  mode;  no  further  opens  (except by a
   privileged user) are permitted until the file has been closed.
   TIOCNXCL
   Turn off ``exclusive-use'' mode.
   TIOCSETP
   The   argument  is  a  pointer  to  an  sgttyb  structure.  The
   appropriate  characters  and  input  and  output  speeds in the
   terminal  state  are set from the values in that structure, and
   the  flags in the terminal state are set to match the values of
   the flags in the sg_flags field of that structure. The state is
   changed with a TCSETSF ioctl so that the interface delays until
   output  is  quiescent,  then throws away any unread characters,
   before changing the modes.
   TIOCSETN
   The  argument is a pointer to an sgttyb structure. The terminal
   state  is  changed  as  TIOCSETP  would change it, but a TCSETS
   ioctl  is  used,  so  that  the  interface  neither  delays nor
   discards input.
   TIOCHPCL
   The  argument  is ignored. The HUPCL flag is set in the c_cflag
   word of the terminal state.
   TIOCFLUSH
   The  argument  is a pointer to an int variable. If its value is
   zero,  all  characters  waiting  in  input or output queues are
   flushed.  Otherwise,  the  value  of  the int is treated as the
   logical OR of the FREAD and FWRITE flags defined by sys/file.h;
   if the FREAD bit is set, all characters waiting in input queues
   are  flushed,  and  if  the  FWRITE  bit is set, all characters
   waiting in output queues are flushed.
   TIOCBRK
   The argument is ignored. The break bit is set for the device.
   TIOCCBRK
   The  argument  is  ignored.  The  break  bit is cleared for the
   device.
   TIOCSDTR
   The argument is ignored. The Data Terminal Ready bit is set for
   the device.
   TIOCCDTR
   The argument is ignored. The Data Terminal Ready bit is cleared
   for the device.
   TIOCSTOP
   The  argument  is  ignored.  Output  is  stopped as if the STOP
   character had been typed.
   TIOCSTART
   The  argument  is  ignored. Output is restarted as if the START
   character had been typed.
   TIOCGETC
   The  argument  is  a pointer to a tchars structure. The current
   terminal  state  is  fetched, and the appropriate characters in
   the terminal state are stored in that structure.
   TIOCSETC
   The  argument is a pointer to a tchars structure. The values of
   the  appropriate  characters in the terminal state are set from
   the characters in that structure.
   TIOCLGET
   The argument is a pointer to an int. The current terminal state
   is  fetched, and the values of the local flags are derived from
   the  flags  in the terminal state and stored in the int pointed
   to by the argument.
   TIOCLBIS
   The  argument  is  a  pointer  to  an int whose value is a mask
   containing flags to be set in the local flags word. The current
   terminal  state  is  fetched, and the values of the local flags
   are derived from the flags in the terminal state; the specified
   flags are set, and the flags in
   the  terminal state are set to match the new value of the local
   flags word.
   TIOCLBIC
   The  argument  is  a  pointer  to  an int whose value is a mask
   containing  flags  to  be  cleared in the local flags word. The
   current  terminal state is fetched, and the values of the local
   flags  are  derived  from  the flags in the terminal state; the
   specified  flags  are  cleared,  and  the flags in the terminal
   state are set to match the new value of the local flags word.
   TIOCLSET
   The  argument  is  a  pointer to an int containing a new set of
   local  flags.  The flags in the terminal state are set to match
   the new value of the local flags word.
   TIOCGLTC
   The  argument  is a pointer to an ltchars structure. The values
   of  the appropriate characters in the terminal state are stored
   in that structure.
   TIOCSLTC
   The  argument  is a pointer to an ltchars structure. The values
   of  the  appropriate  characters  in the terminal state are set
   from the characters in that structure.
   FIORDCHK
   FIORDCHK returns the number of immediately readable characters.
   The argument is ignored.
   FIONREAD
   FIONREAD  returns the number of immediately readable characters
   in the int pointed to by the argument.
   LDSMAP
   Calls   the   function   emsetmap(tp,mp)  if  the  function  is
   configured in the kernel.
   LDGMAP
   Calls   the   function   emgetmap(tp,mp)  if  the  function  is
   configured in the kernel.
   LDNMAP
   Calls the function emunmap(tp,mp) if the function is configured
   in the kernel.

   The  following  ioctls  are  returned  as  successful  for the sake of
   compatibility.  However,  nothing  significant  is  done (that is, the
   state of the terminal is not changed in any way).
   [If this table is unreadable, a preformatted table follows]

   TIOCSETD LDOPEN
   TIOCGETD LDCLOSE
   DIOCSETP LDCHG
   DIOCSETP LDSETT
   DIIOGETP LDGETT

   TIOCSETD   LDOPEN
   TIOCGETD   LDCLOSE
   DIOCSETP   LDCHG
   DIOCSETP   LDSETT
   DIIOGETP   LDGETT

   References

   [8]ioctl(2), [9]ldterm(7), [10]termio(7), [11]termios(3C)

   Notices

   TIOCBRK  and  TIOCCBRK  should  be handled by the driver. FIONREAD and
   FIORDCHK are handled in the stream head.
   _________________________________________________________________

   [12]© 2002 Caldera International, Inc. All rights reserved.
   UnixWare 7 Release 7.1.3 - 30 October 2002

   References

   1. file://localhost/en/index.html
   2. file://localhost/en/Navpages/sitemap.html
   3. file://localhost/cgi-bin/manform?lang=en
   4. file://localhost/cgi-bin/infocat?lang=en
   5. file://localhost/cgi-bin/search?lang=en
   6. file://localhost/home/html.7/termio.7.html
   7. file://localhost/home/html.1M/autopush.1M.html
   8. file://localhost/home/html.2/ioctl.2.html
   9. file://localhost/home/html.7/ldterm.7.html
   10. file://localhost/home/html.7/termio.7.html
   11. file://localhost/home/html.3C/termios.3C.html
   12. file://localhost/home/brian/COPYRIGHT.html

*/

#include <sys/os7/compat.h>

#define TTCOMPAT_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define TTCOMPAT_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define TTCOMPAT_REVISION	"OpenSS7 $RCSfile: ttcompat.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/10/02 12:07:23 $"
#define TTCOMPAT_DEVICE		"SVR 4.2 STREAMS Packet Mode Module (TTCOMPAT)"
#define TTCOMPAT_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define TTCOMPAT_LICENSE	"GPL"
#define TTCOMPAT_BANNER		TTCOMPAT_DESCRIP	"\n" \
				TTCOMPAT_COPYRIGHT	"\n" \
				TTCOMPAT_REVISION	"\n" \
				TTCOMPAT_DEVICE	"\n" \
				TTCOMPAT_CONTACT
#define TTCOMPAT_SPLASH		TTCOMPAT_DEVICE	" - " \
				TTCOMPAT_REVISION
#ifdef LINUX
MODULE_AUTHOR(TTCOMPAT_CONTACT);
MODULE_DESCRIPTION(TTCOMPAT_DESCRIP);
MODULE_SUPPORTED_DEVICE(TTCOMPAT_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(TTCOMPAT_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-ttcompat");
#endif
#endif				/* LINUX */

#ifndef CONFIG_STREAMS_TTCOMPAT_NAME
#error CONFIG_STREAMS_TTCOMPAT_NAME must be defined.
#endif
#ifndef CONFIG_STREAMS_TTCOMPAT_MODID
#error CONFIG_STREAMS_TTCOMPAT_MODID must be defined.
#endif

modID_t modid = CONFIG_STREAMS_TTCOMPAT_MODID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else
module_param(modid, ushort, 0);
#endif
MODULE_PARM_DESC(modid, "Module ID for SC.");

#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_TTCOMPAT_MODID));
MODULE_ALIAS("streams-module-ttcompat");
#endif
#endif

static struct module_info ttcompat_minfo = {
	.mi_idnum = CONFIG_STREAMS_TTCOMPAT_MODID,
	.mi_idname = CONFIG_STREAMS_TTCOMPAT_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat ttcompat_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat ttcompat_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

#if defined __LP64__ && defined LFS
#  undef WITH_32BIT_CONVERSION
#  define WITH_32BIT_CONVERSION 1
#endif

/*
 *  Private structure.
 */
struct ttcompat {
	int flags;
	bcid_t bufcall;
};

#define TTCOMPAT_PRIV(__q) ((struct ttcompat *)((__q)->q_ptr))

/* 
 *  -------------------------------------------------------------------------
 *
 *  PUTP and SRVP routines
 *
 *  -------------------------------------------------------------------------
 */
static streamscall int
ttcompat_r_iocack(queue_t *q, mblk_t *mp)
{
	struct ttcompat *tt = TTCOMPAT_PRIV(q);
	struct iocblk *ioc, *ioc2;
	mblk_t **bpp, *bp, *b;
	struct termios *c;

	ioc = (struct iocblk *) mp->b_rptr;

	for (bpp = &tt->ioctls;
	     (b = *bpp) && ((struct iocblk *) b->b_rptr)->ioc_id != ioc->ioc_id; bpp = &b->b_next) ;

	if (!b) {
		/* not for us */
		putnext(q, mp);
		return (0);
	}

	/* remove it */
	*bpp = b->b_next;
	b->b_next = NULL;
	bp = b->b_cont;

	ioc2 = (typeof(ioc2)) b->b_rptr;
	c = (struct termios *) mp->b_cont->b_rptr;

	switch (ioc2->ioc_cmd) {
	case TIOCLGET:		/* int * (output) */
	{
		int *arg, flags = 0;

		arg = (typeof(arg)) bp->b_rptr;
		bp->b_wptr = bp->b_rptr + sizeof(*arg);

		/* The argument is a pointer to an int. The current terminal state is fetched, and
		   the values of the local flags are derived from the flags in the terminal state
		   and stored in the int pointed to by the argument. */

		if (c->c_lflag & ECHOPRT)
			flags |= LPRTERA;
		if (c->c_lflag & ECHOE)
			flags |= LCRTERA;
		if (c->c_lflag & TOSTOP)
			flags |= LTOSTOP;
		if (c->c_lflag & FLUSHO)
			flags |= LFLUSHO;
		if (c->c_cflag & CLOCAL)
			flags |= LHOHANG;
		if (c->c_lflag & ECHOKE)
			flags |= LCRTKIL;
		if (c->c_lflag & CTLECH)
			flags |= LCTLECH;
		if (c->c_lflag & PENDIN)
			flags |= LPENDIN;
		if (c->c_iflag & IXANY)
			flags &= ~LDECCTQ;
		else
			flags |= LDECCTQ;
		if (c->c_lflag & NOFLSH)
			flags |= LNOFLSH;

		*arg = flags;

		b->b_datap->db_type = M_IOCNAK;
		ioc2->ioc_error = 0;
		ioc2->ioc_rval = 0;
		ioc2->ioc_count = sizeof(*arg);
		putnext(q, b);
		freemsg(mp);
		return (0);
	}
	case TIOCGETP:		/* struct sgttyb * (output) */
	{
		struct sgttyb *sg;

		sg = (typeof(sg)) bp->b_rptr;
		bp->b_wptr = bp->b_rptr + sizeof(*sg);

		/* The argument is a pointer to an sgttyb structure. The current terminal state is
		   fetched; the appropriate characters in the terminal state are stored in that
		   structure, as are the input and output speeds. The values of the flags in the
		   sg_flags field are derived from the flags in the terminal state and stored in
		   the structure. */

		/* The sg_ispeed and sg_ospeed fields describe the input and output speeds of the
		   device, and reflect the values in the c_cflag field of the termios structure. */

		sg->sg_ispeed = (c->c_cflag & CIBAUD) >> IBSHIFT;
		if (c->c_flag & CIBAUDEXT)
			sg->sg_ispeed += (CIBAUD >> IBSHIFT) + 1;

		sg->sg_ospeed = (c->c_cflag & CBAUD);
		if (c->c_flag & CBAUDEXT)
			sg->sg_ospeed += CBAUD + 1;

		if (sg->sg_ispeed == 0)
			sg->sg_ispeed = sg->sg_ospeed;

		/* The sg_erase and sg_kill fields of the argument structure specify the erase and
		   kill characters respectively, and reflect the values in the VERASE and VKILL
		   members of the c_cc field of the termios structure. */

		if (!(sg->sg_erase = c->c_cc[VERASE]))
			sg->sg_erase = 0xff;
		if (!(sg->sg_kill = c->c_cc[VKILL]))
			sg->sg_kill = 0xff;

		/* The sg_flags field of the argument structure contains several flags that
		   determine the system's treatment of the terminal. They are mapped into flags in
		   fields of the terminal state, represented by the termios structure.  Delay type
		   0 is always mapped into the equivalent delay type 0 in the c_oflag field of the
		   termios structure. Other delay mappings are performed as follows: */

		sg->sg_flags = 0;

		if ((c->c_oflag & BSDLY) == BS1)
			flags |= BS1;
		if ((c->c_oflag & VTDLY) == VT1)
			flags |= FF1;

		if (c->c_oflag & ONLCR) {
			switch (c->c_oflag & CRDLY) {
			case CR2:
				sg->sg_flags |= CR1;
				break;
			case CR3:
				sg->sg_flags |= CR2;
				break;
			}
		} else {
			if ((c->c_oflag & (ONLRET|CR1)) == (ONLRET|CR1))
				sg->sg_flags |= NL1;
		}
		if ((c->c_oflag & (ONLRET|NL1)) == (ONLRET|NL1))
			sg-sg_flags |= NL2;

		switch (c->c_oflag & TABDLY) {
		case TAB1:
			sg->sg_flags |= TAB1;
			break;
		case TAB2:
			sg->sg_flags |= TAB2;
			break;
		case TAB3:
			sg->sg_flags |= TAB3;
			break;
		}
	}
	case TIOCGETC:		/* struct tchars * (output) */
	{
		struct tchars *t;

		t = (typeof(t)) bp->b_rptr;
		bp->b_wptr = bp->b_rptr + sizeof(*t);

		/* The argument is a pointer to a tchars structure. The current terminal state is
		   fetched, and the appropriate characters in the terminal state are stored in that
		   structure. */

		/* never set zero in these characters */
		if (!(t->t_intrc = c->c_cc[VINTR]))
			t->t_intrc = 0xff;
		if (!(t->t_quitc = c->c_cc[VQUIT]))
			t->t_quitc = 0xff;
		if (!(t->t_startc = c->c_cc[VSTART]))
			t->t_startc = 0xff;
		if (!(t->t_stopc = c->c_cc[VSTOP]))
			t->t_stopc = 0xff;
		if (!(t->t_eofc = c->c_cc[VEOF]))
			t->t_eofc = 0xff;
		if (!(t->t_brkc = c->c_cc[VEOL]))
			t->t_brkc = 0xff;

		b->b_datap->db_type = M_IOCACK;
		ioc2->ioc_error = 0;
		ioc2->ioc_rval = 0;
		ioc2->ioc_count = sizeof(*t);
		putnext(q, b);
		freemsg(mp);
		return (0);
	}
	case TIOCGLTC:		/* struct ltchars * (output) */
	{
		struct ltchars *t;

		t = (typeof(t)) bp->b_rptr;
		bp->b_wptr = bp->b_rptr + sizeof(*t);

		/* The argument is a pointer to an ltchars structure. The values of the appropriate 
		   characters in the terminal state are stored in that structure. */

		/* never set zero in these characters */
		if (!(t->t_suspc = c->c_cc[VSUSP]))
			t->t_suspc = 0xff;
		if (!(t->t_dsuspc = c->c_cc[VDSUSP]))
			t->t_dsuspc = 0xff;
		if (!(t->t_rprntc = c->c_cc[VREPRINT]))
			t->t_rprntc = 0xff;
		if (!(t->t_flushc = c->c_cc[VDISCARD]))
			t->t_flushc = 0xff;
		if (!(t->t_werasc = c->c_cc[VWERASE]))
			t->t_werasc = 0xff;
		if (!(t->t_lnextc = c->c_cc[VLNEXT]))
			t->t_lnextc = 0xff;

		b->b_datap->db_type = M_IOCACK;
		ioc2->ioc_error = 0;
		ioc2->ioc_rval = 0;
		ioc2->ioc_count = sizeof(*t);
		putnext(q, b);
		freemsg(mp);
		return (0);
	}

	case TIOCSETP:		/* struct sgttyb * (input) */
	case TIOCSETN:		/* struct sgttyb * (input) */
	case TIOCSETC:		/* struct tchars * (input) */
	case TIOCSLTC:		/* struct ltchars * (input) */
	case TIOCLBIS:		/* int * (input) */
	case TIOCLBIC:		/* int * (input) */
	case TIOCLSET:		/* int * (input) */
	case TIOCHPCL:		/* void */
	default:
		__swerr();
		freemsg(b);
		putnext(q, mp);
		return (0);
	}
}

static streamscall int
ttcompat_r_iocnak(queue_t *q, mblk_t *mp)
{
	struct ttcompat *tt = TTCOMPAT_PRIV(q);
	struct iocblk *ioc, *ioc2;
	mblk_t **bp, *b;

	ioc = (struct iocblk *) mp->b_rptr;

	for (bp = &tt->ioctls;
	     (b = *bp) && ((struct iocblk *) b->b_rptr)->ioc_id != ioc->ioc_id; bp = &b->b_next) ;

	if (!b) {
		/* not for us */
		putnext(q, mp);
		return (0);
	}

	/* remove it */
	*bp = b->b_next;
	b->b_next = NULL;

	ioc2 = (typeof(ioc2)) b->b_rptr;

	/* nak original with same erroro we got back */
	b->b_datap->db_type = M_IOCNAK;
	ioc2->ioc_error = ioc->ioc_error;
	ioc2->ioc_rval = -1;
	ioc2->ioc_count = 0;

	freemsg(mp);

	putnext(q, b);
	return (0);
}

static streamscall __hot_get int
ttcompat_rput(queue_t *q, mblk_t *mp)
{
	int type = mp->b_datap->db_type;

	/* Fast Path. */
	if (likely(type < QPCTL)
	    || (type != M_IOCACK && type != M_IOCNAK)) {
	      pass:
		putnext(q, mp);
		return (0);
	}
	switch (type) {
	default:
		goto pass;
	case M_IOCACK:
		return ttcompat_r_iocack(q, mp);
	case M_IOCNAK:
		return ttcompat_r_iocnak(q, mp);
	}
}

static streamscall void
ttcompat_w_ioctl(queue_t *q, mblk_t *mp)
{
	struct iocblk *ioc;
	struct ttcompat *tt;
	int error = EINVAL;
	int count = 0;
	int rval = 0;
	mblk_t *bp;

	if ((bp = mp->b_cont) == NULL)
		goto nak;

	ioc = (typeof(ioc)) mp->b_rptr;

	/* The Stream head is set to make all TRANPARENT ioctls into I_STR ioctls. */
	if (ioc->ioc_count == TRANSPARENT)
		goto nak;

	switch (ioc->ioc_cmd) {
	case TIOCLGET:		/* int * (output) */
		break;
	case TIOCGETC:		/* struct tchars * (output) */
		if (FASTBUF < sizeof(struct tchars)) {
			count = sizeof(struct tchars);
			if (!pullupmsg(bp, count)) {
				freemsg(bp);
				if (!(bp = mp->b_cont = allocb(count, BPRI_MED))) {
					error = EAGAIN;
					goto nak;
				}
			}
		}
		break;
	case TIOCGLTC:		/* struct ltchars * (output) */
		if (FASTBUF < sizeof(struct ltchars)) {
			count = sizeof(struct ltchars);
			if (!pullupmsg(bp, count)) {
				freemsg(bp);
				if (!(bp = mp->b_cont = allocb(count, BPRI_MED))) {
					error = EAGAIN;
					goto nak;
				}
			}
		}
		break;
	case TIOCGETP:		/* struct sgttyb * (output) */
		if (FASTBUF < sizeof(struct sgttyb)) {
			count = sizeof(struct sgttyb);
			if (!pullupmsg(bp, count)) {
				freemsg(bp);
				if (!(bp = mp->b_cont = allocb(count, BPRI_MED))) {
					error = EAGAIN;
					goto nak;
				}
			}
		}
		break;

	case TIOCSETP:		/* struct sgttyb * (input) */
	case TIOCSETN:		/* struct sgttyb * (input) */
		if (!pullupmsg(bp, sizeof(struct sgttyb)))
			goto nak;
		break;

	case TIOCSETC:		/* struct tchars * (input) */
		if (!pullupmsg(bp, sizeof(struct tchars)))
			goto nak;
		break;

	case TIOCSLTC:		/* struct ltchars * (input) */
		if (!pullupmsg(bp, sizeof(struct ltchars)))
			goto nak;
		break;

	case TIOCLBIS:		/* int * (input) */
	case TIOCLBIC:		/* int * (input) */
	case TIOCLSET:		/* int * (input) */
		if (!pullupmsg(bp, sizeof(int)))
			goto nak;
		break;

	case TIOCHPCL:		/* void */
		break;

	case TIOCSDTR:
	case TIOCCDTR:
		ioc->ioc_cmd = (ioc->ioc_cmd == TIOCSDTR) ? TIOCMBIS : TIOCMBIC;
		ioc->ioc_count = sizeof(int);
		bp->b_rptr = bp->b_datap->db_base;
		bp->b_wptr = bp->b_rptr + sizeof(int);
		*(int *) bp->b_rptr = TIOCM_DTR;
		putnext(q, mp);
		return;

	case TIOCFLUSH:
		if (!pullupmsg(bp, sizeof(int)))
			goto nak;
		ioc->ioc_cmd = TCFLSH;
		ioc->ioc_count = sizeof(int);
		switch (*(int *) bp->b_rptr & (FREAD | FWRITE)) {
		case FREAD:
			*(int *) bp->b_rptr = 0;
			break;
		case FWRITE:
			*(int *) bp->b_rptr = 1;
			break;
		case FREAD | FWRITE:
			*(int *) bp->b_rptr = 2;
			break;
		}
		putnext(q, mp);
		return;

	case TIOCSTOP:
	case TIOCSTART:
		*(int *) bp->b_rptr = (ioc->ioc_cmd == TIOCSTOP) ? 0 : 1;
		ioc->ioc_cmd = TCXONC;
		ioc->ioc_count = sizeof(int);
		putnext(q, mp);
		return;

	case TIOCSETD:
		/* The following ioctls are returned as successful for the sake of compatibility.
		   However, nothing significant is done (that is, the state of the terminal is not
		   changed in any way): TIOCSETD, TIOCGETD, DIOCSETP, DIIOGETP, LDOPEN, LDCLOSE,
		   LDCHG, LDSETT and LDGETT. */

		if (!pullupmsg(bp, sizeof(uchar)))
			goto nak;
		if (*bp->b_rptr != 0)
			goto nak;
		/* fall through */
	case TIOCGETD:
	case DIOCSETP:
	case DIOCGETP:
	case LDOPEN:
	case LDCLOSE:
	case LDCHG:
	case LDSETT:
	case LDGETT:
		goto ack;

	case IOCTYPE:
		rval = TIOC;
		goto ack;

	case TIOCEXCL:
		tt = TTCOMPAT_PRIV(q);
		tt->flags |= TTCOMPAT_EXCLUDE;
		goto ack;

	case TIOCNXCL:
		tt = TT_COMPAT_PRIV(q);
		tt->flags &= TTCOMPAT_EXCLUDE;
		goto ack;

	default:
		putnext(q, mp);
		return;
	}
	/* 
	 * Process complex ioctls.  Complex ioctls require an exchange with
	 * the tty below.  First we want to obtain the information from the
	 * tty as though we were the application and then we want to sent the
	 * appropriate set command afterward.  To do this, we need to maintain
	 * the state information of the original ioctl in the meantime.  The
	 * most direct way to do this is to save the original M_IOCTL message
	 * and allocate a new one for the call.
	 */
	{
		mblk_t *mb;
		struct iocblk *ioc2;

		if (!(mb = allocb(sizeof(strioctl), BPRI_MED))) {
			error = EAGAIN;
			goto nak;
		}
		if (!(mb->b_cont = allocb(sizeof(struct termios), BPRI_MED))) {
			error = EAGAIN;
			freeb(mb);
			goto nak;
		}

		mb->b_datap->db_type = M_IOCTL;
		ioc2 = (typeof(ioc2)) mb->b_rptr;
		mb->b_wptr += sizeof(*ioc2);

		bzero(ioc2, sizeof(*ioc2));	/* security */
		ioc2->ioc_cmd = TGETS;
		ioc2->ioc_cr = ioc->ioc_cr;	/* hmmm.. */
		ioc2->ioc_id = ioc->ioc_id;
		ioc2->ioc_count = sizeof(struct termios);
		ioc2->ioc_error = 0;
		ioc2->ioc_rval = 0;
		ioc2->ioc_flag = 0;	/* this is a native call */

		tt = TTCOMPAT_PRIV(q);

		/* save original */
		mp->b_next = tt->ioctls;
		tt->ioctls = mp;

		putnext(q, mb);
		return;
	}
      nak:
	mp->b_datap->db_type = M_IOCACK;
	ioc->ioc_error = 0;
	ioc->ioc_rval = rval;
	ioc->ioc_count = count;
	goto reply;
      nak:
	mp->b_datap->db_type = M_IOCNAK;
	ioc->ioc_error = error;
	ioc->ioc_rval = -1;
	ioc->ioc_count = 0;
      reply:
	qreply(q, mp);
	return (0);

}

static streamscall __hot_put int
ttcompat_wput(queue_t *q, mblk_t *mp)
{
	/* Fast path. */
	if (likely(mp->b_datap->db_type != M_IOCTL)) {
	      non_ioctl:
		putnext(q, mp);
		return (0);
	}
	switch (mp->b_datap->db_type) {
	default:
		goto non_ioctl;
	case M_IOCTL:
		ttcompat_w_ioctl(q, mp);
	}
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  OPEN and CLOSE
 *
 *  -------------------------------------------------------------------------
 */
static streamscall int
ttcompat_qopen(queue_t *q, dev_t *devp, int oflag, int sflag, cred_t *crp)
{
	if (q->q_ptr)
		return (0);	/* already open */
	if (sflag == MODOPEN && WR(q)->q_next != NULL) {
		struct ttcompat *tt;

		if ((tt = kmem_alloc(sizeof(*tt)))) {
			bzero(tt, sizeof(*tt));
			tt->flags = oflag;	/* to test later for data model */
			q->q_ptr = WR(q)->q_ptr = (void *) tt;
			qprocson(q);
			return (0);
		}
		return (OPENFAIL);
	}
	return (OPENFAIL);	/* can't be opened as driver */
}

static streamscall int
ttcompat_qclose(queue_t *q, int oflag, cred_t *crp)
{
	struct ttcompat *tt;

	qprocsoff(q);
	if ((tt = (struct ttcompat *) q->q_ptr)) {
		bcid_t bc;

		/* atomic exchange for LiS's stupid sake */
		if ((bc = xchg(&tt->bufcall, 0)))
			unbufcall(bc);
		kmem_free(tt, sizeof(*tt));
	}
	q->q_ptr = WR(q)->q_ptr = NULL;
	return (0);
}

/* 
 *  -------------------------------------------------------------------------
 *
 *  Registration and initialization
 *
 *  -------------------------------------------------------------------------
 */

static struct qinit ttcompat_rqinit = {
	.qi_putp = ttcompat_rput,
	.qi_qopen = ttcompat_qopen,
	.qi_qclose = ttcompat_qclose,
	.qi_minfo = &ttcompat_minfo,
	.qi_mstat = &ttcompat_rstat,
};

static struct qinit ttcompat_wqinit = {
	.qi_putp = ttcompat_wput,
	.qi_minfo = &ttcompat_minfo,
	.qi_mstat = &ttcompat_wstat,
};

static struct streamtab ttcompat_info = {
	.st_rdinit = &ttcompat_rqinit,
	.st_wrinit = &ttcompat_wqinit,
};

#ifdef LIS
#define fmodsw _fmodsw
#endif
static struct fmodsw ttcompat_fmod = {
	.f_name = CONFIG_STREAMS_TTCOMPAT_NAME,
	.f_str = &ttcompat_info,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

#ifdef CONFIG_STREAMS_TTCOMPAT_MODULE
static
#endif
int __init
ttcompat_init(void)
{
	int err;

#ifdef CONFIG_STREAMS_TTCOMPAT_MODULE
	cmn_err(CE_NOTE, TTCOMPAT_BANNER);
#else
	cmn_err(CE_NOTE, TTCOMPAT_SPLASH);
#endif
	ttcompat_minfo.mi_idnum = modid;
	if ((err = register_strmod(&ttcompat_fmod)) < 0)
		return (err);
	if (modid == 0 && err > 0)
		modid = err;
	return (0);
}

#ifdef CONFIG_STREAMS_TTCOMPAT_MODULE
static
#endif
void __exit
ttcompat_exit(void)
{
	unregister_strmod(&ttcompatfmod);
}

#ifdef CONFIG_STREAMS_TTCOMPAT_MODULE
module_init(ttcompat_init);
module_exit(ttcompat_exit);
#endif
