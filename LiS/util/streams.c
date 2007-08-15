/*****************************************************************************

 @(#) $RCSfile: streams.c,v $ $Name:  $($Revision: 1.1.1.6.4.8 $) $Date: 2007/08/15 04:58:12 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

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

 Last Modified $Date: 2007/08/15 04:58:12 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: streams.c,v $
 Revision 1.1.1.6.4.8  2007/08/15 04:58:12  brian
 - GPLv3 updates

 Revision 1.1.1.6.4.5  2005/07/13 12:01:22  brian
 - working up compat and check pass (finally lindented LiS)

 Revision 1.1.1.6.4.3  2005/05/14 08:35:12  brian
 - copyright header correction

 *****************************************************************************/

#ident "@(#) $RCSfile: streams.c,v $ $Name:  $($Revision: 1.1.1.6.4.8 $) $Date: 2007/08/15 04:58:12 $"

static char const ident[] =
    "$RCSfile: streams.c,v $ $Name:  $($Revision: 1.1.1.6.4.8 $) $Date: 2007/08/15 04:58:12 $";

#include <sys/types.h>
#undef GCOM_OPEN
#include <sys/stropts.h>
#include <sys/LiS/stats.h>
#ifdef QNX
#include <unix.h>
#define	LOOP_CLONE	"/dev/gcom/loop_clone"
#else
#define	LOOP_CLONE	"/dev/loop_clone"
#endif
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

/*
 * Storage Declarations
 */
unsigned long strstats[STRMAXSTAT][4];	/* the stats */
char buf[100000];			/* large buffer */

int output = 1;

unsigned long strstats[STRMAXSTAT][4];	/* the stats */

int cflag = 0;
int Cflag = 0;
int mflag = 0;
int dflag = 0;
int Dflag = 0;
int Lflag = 0;
int sflag = 0;
int Sflag = 0;
int tflag = 0;
int Tflag = 0;
int pflag = 0;
int qflag = 0;
int Hflag = 0;

char *prog_name;

long memK = 0;
long msgmemK = 0;

lis_qrun_stats_t qrun_stats;

typedef struct {
	int micro_secs;
	unsigned counter;

} histogram_bucket_t;

/*
 * Names of stat slots
 */
itemname_t lis_itemnames[] = {
	{MODUSE, MODUSESTR},
	{MEMALLOCS, MEMALLOCSSTR},
	{MEMFREES, MEMFREESSSTR},
	{MEMALLOCD, MEMALLOCDSTR},
	{MEMLIM, MEMLIMSTR},
	{HEADERS, HEADERSSTR},
	{FREEHDRS, FREEHDRSSTR},
	{DATABS, DATABSSTR},
	{DBLKMEM, DBLKMEMSTR},
	{MSGMEMLIM, MSGMEMLIMSTR},
	{BUFCALLS, BUFCALLSSTR},
	{MSGSQD, MSGSQDSTR},
	{MSGQDSTRHD, MSGQDSTRHDSTR},
	{CANPUTS, CANPUTSSTR},
	{QUEUES, QUEUESSTR},
	{QSCHEDS, QSCHEDSSTR},
	{WRITECNT, WRITESTR},
	{READCNT, READSTR},
	{OPENTIME, OPENTIMESTR},
	{CLOSETIME, CLOSETIMESTR},
	{READTIME, READTIMESTR},
	{WRITETIME, WRITETIMESTR},
	{IOCTLTIME, IOCTLTIMESTR},
	{GETMSGTIME, GETMSGTIMESTR},
	{PUTMSGTIME, PUTMSGTIMESTR},
	{POLLTIME, POLLTIMESTR},
	{LOCKCNTS, LOCKCNTSSTR}
};

char *lis_countnames[] = {
	CURRENTSTR, TOTALSTR, MAXIMUMSTR, FAILURESSTR
};

void
LisShowStrStats(void)
{
	int i;
	int inx;
	int found_time = 0;
	int have_hdr = 0;

	if (!output)
		return;
	if (!sflag && !tflag)
		return;		/* no stats desired */

	/* 
	 * Print statistics 
	 */
	for (i = 0; i < STRMAXSTAT; i++) {
		int j;

		if (lis_itemnames[i].name == NULL)
			continue;

		inx = lis_itemnames[i].stats_inx;
		if (inx == OPENTIME) {	/* time for a new header */
			found_time = 1;
			have_hdr = 0;
		}

		if (found_time) {	/* got to time stats */
			if (!tflag)
				break;	/* not supposed to print them */
		} else /* still in regular stats */ if (!sflag)	/* not supposed to * * print them */
			continue;

		if (!have_hdr) {
			int k;

			have_hdr = 1;
			printf("\n%-28s  ", "");
			for (k = 0; k < 4; k++)
				printf("%12s", lis_countnames[k]);
			printf("\n");
		}

		printf("%-28s: ", lis_itemnames[i].name);
		for (j = 0; j < 4; j++)
#if (defined(_S390X_LIS_) || defined(_PPC64_LIS_))
			printf("%12lu", strstats[inx][j] / 4294967296);
#else				/* (defined(_S390X_LIS_) || defined(_PPC64_LIS_)) */
			printf("%12lu", strstats[inx][j]);
#endif				/* (defined(_S390X_LIS_) || defined(_PPC64_LIS_)) */
		printf("\n");
	}

}				/* LisShowStrStats */

void
print_qrun_stats(void)
{
	int cpu;

	if (!output)
		return;
	printf("N-CPUs N-Qrunners N-Running N-Requested\n" "%6u %10u %9u %10u\n",
	       qrun_stats.num_cpus, qrun_stats.num_qrunners, qrun_stats.queues_running,
	       qrun_stats.runq_req_cnt);

	printf("\n" "CPU   Qsched-Cnts Qsched-ISR Svc-Q-Cnts Qrun-Wkups Qrun-Cnts Active Thread-PID"
	       "\n");
	for (cpu = 0; cpu < qrun_stats.num_cpus; cpu++) {
		printf("%3u   %11lu %10lu %10lu %10lu %9lu %6u %10u\n", cpu,
		       qrun_stats.setqsched_cnts[cpu], qrun_stats.setqsched_isr_cnts[cpu],
		       qrun_stats.runq_cnts[cpu], qrun_stats.runq_wakeups[cpu],
		       qrun_stats.queuerun_cnts[cpu], qrun_stats.active_flags[cpu],
		       qrun_stats.runq_pids[cpu]);
	}
}

void
print_sem_time_hist(char *buf, int nbytes)
{
	histogram_bucket_t *p = (histogram_bucket_t *) buf;

	for (; p->micro_secs != 0; p++) {
		if (p->counter != 0)
			printf("%8d %12u\n", p->micro_secs, p->counter);
	}

	if (p->counter != 0)
		printf("%8s %12u\n", "Larger", p->counter);
}

void
copying(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stdout, "\
\n\
%1$s %2$s:\n\
\n\
Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
Copyright (c) 1997       David Grothe, Gcom, Inc <dave@gcom.com>\n\
\n\
All Rights Reserved.\n\
\n\
This program is free software;  you can  redistribute  it and/or modify it under\n\
the terms of the GNU General  Public License as  published by the  Free Software\n\
Foundation; version 3 of  the  License.\n\
\n\
This program is distributed in the hope that it will be  useful, but WITHOUT ANY\n\
WARRANTY;  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A\n\
PARTICULAR PURPOSE.  See the GNU General Public License for more details.\n\
\n\
You should  have received  a copy of the GNU  General  Public License along with\n\
this program.  If not, see <http://www.gnu.org/licenses/>,  or write to the Free\n\
Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.\n\
\n\
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf\n\
of the  U.S. Government  (\"Government\"),  the following provisions apply to you.\n\
If the Software is  supplied by the Department of Defense (\"DoD\"), it is classi-\n\
fied as  \"Commercial Computer Software\"  under paragraph 252.227-7014 of the DoD\n\
Supplement  to the  Federal Acquisition Regulations  (\"DFARS\") (or any successor\n\
regulations) and the  Government  is acquiring  only the license rights  granted\n\
herein (the license  rights customarily  provided to non-Government  users).  If\n\
the Software is supplied to any unit or agency of the Government other than DoD,\n\
it is classified as  \"Restricted Computer Software\" and the  Government's rights\n\
in the  Software are defined in  paragraph 52.227-19 of the Federal  Acquisition\n\
Regulations  (\"FAR\") (or any successor regulations) or, in the cases of NASA, in\n\
paragraph  18.52.227-86 of the  NASA Supplement  to the  FAR (or  any  successor\n\
regulations).\n\
\n\
", argv[0], ident);
}

void
version(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stdout, "\
\n\
%1$s %2$s:\n\
    Copyright (c) 2003-2007  OpenSS7 Corporation.  All Rights Reserved.\n\
    Copyright (c) 1997       David Grothe, Gcom, Inc <dave@gcom.com>\n\
\n\
    Distributed by OpenSS7 Corporation under GPL Version 3,\n\
    included here by reference.\n\
\n\
    See `%1$s --copying' for copying permissions.\n\
\n\
", argv[0], ident);
}

void
usage(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [command]\n\
    %1$s [options]\n\
    %1$s {-h,--help}\n\
    %1$s {-V,--version}\n\
    %1$s --copying\n\
", argv[0]);
}

void
help(int argc, char *argv[])
{
	if (!output)
		return;
	fprintf(stdout, "\
\n\
Usage:\n\
    %5$s [command]\n\
    %5$s [options]\n\
    %5$s {-h,--help}\n\
    %5$s {-V,--version}\n\
    %5$s --copying\n\
Commands:\n\
    start\n\
        starts the STREAMS subsystem\n\
    stop\n\
        stops the STREAMS subsystem\n\
    status\n\
        reports the status of the STREAMS subsystem\n\
Options:\n\
    -c, --maxmsg [KBYTES]\n\
        print/set maximum message memory usage for LIS [default: %1$ld]\n\
    -C, --maxmem KBYTES\n\
        print/set maximum memory usage for LIS [default: %2$ld]\n\
    -s, --memstats\n\
        prints STREAMS memory statistics\n\
    -S, --qrunstats\n\
        prints STREAMS queue run statistics\n\
    -t, --timestats\n\
        prints STREAMS timing statistics\n\
    -m, --dumpmem\n\
        dump STREAMS memory allocation to syslog file\n\
    -p, --dumpspl\n\
        dump SPL trace buffer to syslog file\n\
    -q, --dumpqueues\n\
        dump all queues to syslog file\n\
    -d, --debug1 MASK\n\
        sets the first debug mask [default: 0x%3$08x]\n\
        MASK can be a bitwise OR of any of the following flags:\n\
          0x01 OPEN    0x0100 PUTMSG    0x010000 SAFE        0x01000000 DMP_QUEUE\n\
          0x02 CLOSE   0x0200 GETMSG    0x020000 TRCE_MSG    0x02000000 DMP_MBLK\n\
          0x04 READ    0x0400 POLL      0x040000 CLEAN_MSG   0x04000000 DMP_DBLK\n\
          0x08 WRITE   0x0800 LINK      0x080000 SPL_TRACE   0x08000000 DMP_STRHD\n\
          0x10 IOCTL   0x1000 MEAS_TIME 0x100000 MP_ALLOC    0x10000000 VOPEN\n\
          0x20 PUTNEXT 0x2000 MEM_LEAK  0x200000 MP_FREEMSG  0x20000000 VCLOSE\n\
          0x40 STRRPUT 0x4000 FLUSH     0x400000 MALLOC      0x40000000 -\n\
          0x80 SIG     0x8000 FATTACH   0x800000 MONITOR_MEM 0x80000000 ADDRS\n\
    -D, --debug2 MASK2\n\
        sets the second debug mask [default: 0x%4$08x]\n\
        MASK2 can be a bitwise OR of any of the following flags:\n\
          0x01 SNDFD   0x02 CP   0x04 CACHE\n\
    -<FLAG>, --<FLAG>\n\
        where FLAG is one of the symbolic --debug1 or --debug2 flag names\n\
        above.  Multiple flags options are OR'ed together.\n\
    -Q, --quiet\n\
        suppress normal output\n\
    -h, --help, -H (obsolete)\n\
        print this usage information and exit\n\
    -V, --version\n\
        print version and exit\n\
    --copying\n\
        print copying permissions and exit\n\
\n\
", msgmemK, memK, 0, 0, argv[0]);
}

int
main(int argc, char *argv[])
{
	int fd;
	int rslt;
	unsigned long debug_mask = 0;
	unsigned long debug_mask2 = 0;

	prog_name = argv[0];

	/* 
	 * Check for mnemonic options.  These must appear alone
	 * after the command name.
	 */
	if (argc >= 2 && argv[1] != NULL) {
		if (strcmp(argv[1], "start") == 0) {
			exit(system("/usr/sbin/strms_up"));
		}

		if (strcmp(argv[1], "stop") == 0) {
			exit(system("/usr/sbin/strms_down"));
		}

		if (strcmp(argv[1], "status") == 0) {
			exit(system("/usr/sbin/strms_status"));
		}
	}

	while (1) {
		int c;

#ifdef _GNU_SOURCE
		int option_index = 0;
	/* *INDENT-OFF* */
	static struct option long_options[] = {
	    { "help",		no_argument,		NULL, 'h' },
	    { "version",	no_argument,		NULL, 'V' },
	    { "copying",	no_argument,		NULL, 55  },
	    { "maxmsg",		optional_argument,	NULL, 'c' },
	    { "maxmem",		optional_argument,	NULL, 'C' },
	    { "debug1",		required_argument,	NULL, 'd' },
	    { "debug2",		required_argument,	NULL, 'D' },
	    { "memstats",	no_argument,		NULL, 's' },
	    { "qrunstats",	no_argument,		NULL, 'S' },
	    { "timestats",	no_argument,		NULL, 't' },
	    { "dumpmem",	no_argument,		NULL, 'm' },
	    { "dumpspl",	no_argument,		NULL, 'p' },
	    { "dumpqueues",	no_argument,		NULL, 'q' },
	    { "quiet",		no_argument,		NULL, 'Q' },
	    { "help",		no_argument,		NULL, 'h' },
	    { "version",	no_argument,		NULL, 'V' },
	    { "OPEN",		no_argument,		NULL, 1	  },
	    { "CLOSE",		no_argument,		NULL, 2	  },
	    { "READ",		no_argument,		NULL, 3	  },
	    { "WRITE",		no_argument,		NULL, 4	  },
	    { "IOCTL",		no_argument,		NULL, 5	  },
	    { "PUTNEXT",	no_argument,		NULL, 6	  },
	    { "STRRPUT",	no_argument,		NULL, 7	  },
	    { "SIG",		no_argument,		NULL, 8	  },
	    { "PUTMSG",		no_argument,		NULL, 9	  },
	    { "GETMSG",		no_argument,		NULL, 10  },
	    { "POLL",		no_argument,		NULL, 11  },
	    { "LINK",		no_argument,		NULL, 12  },
	    { "MEAS_TIME",	no_argument,		NULL, 13  },
	    { "MEM_LEAK",	no_argument,		NULL, 14  },
	    { "FLUSH",		no_argument,		NULL, 15  },
	    { "FATTACH",	no_argument,		NULL, 16  },
	    { "SAFE",		no_argument,		NULL, 17  },
	    { "TRCE_MSG",	no_argument,		NULL, 18  },
	    { "CLEAN_MSG",	no_argument,		NULL, 19  },
	    { "SPL_TRACE",	no_argument,		NULL, 20  },
	    { "MP_ALLOC",	no_argument,		NULL, 21  },
	    { "MP_FREEMSG",	no_argument,		NULL, 22  },
	    { "MALLOC",		no_argument,		NULL, 23  },
	    { "MONITOR_MEM",	no_argument,		NULL, 24  },
	    { "DMP_QUEUE",	no_argument,		NULL, 25  },
	    { "DMP_MBLK",	no_argument,		NULL, 26  },
	    { "DMP_DBLK",	no_argument,		NULL, 27  },
	    { "DMP_STRHD",	no_argument,		NULL, 28  },
	    { "VOPEN",		no_argument,		NULL, 29  },
	    { "VCLOSE",		no_argument,		NULL, 30  },
	    { "ADDRS",		no_argument,		NULL, 32  },
	    { "SNDFD",		no_argument,		NULL, 33  },
	    { "CP",		no_argument,		NULL, 34  },
	    { "CACHE",		no_argument,		NULL, 35  },
	    { 0, }
	};
	/* *INDENT-ON* */

		c = getopt_long_only(argc, argv, "pqsStmhHVQd:D:c::C::", long_options,
				     &option_index);
#else				/* _GNU_SOURCE */
		c = getopt(argc, argv, "pqsStmhHVQd:D:c::C::");
#endif				/* _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case 0:
			usage(argc, argv);
			exit(2);
		case 'c':	/* -c, --maxmsg KBYTES */
			cflag = 1;
			if (optarg != NULL && sscanf(optarg, "%li", &msgmemK) != 1)
				goto bad_option;
			break;
		case 'C':	/* -C, --maxmem KBYTES */
			Cflag = 1;
			if (optarg != NULL && sscanf(optarg, "%li", &memK) != 1)
				goto bad_option;
			break;
		case 'd':	/* -d, --debug1 MASK */
			dflag = 1;
			if (sscanf(optarg, "%lx", &debug_mask) != 1)
				goto bad_option;
			break;
		case 'D':	/* -D, --debug2 MASK */
			Dflag = 1;
			if (sscanf(optarg, "%lx", &debug_mask2) != 1)
				goto bad_option;
			break;
		case 's':	/* -s, --memstats */
			sflag = 1;
			break;
		case 'S':	/* -S, --qrunstats */
			Sflag = 1;
			break;
		case 't':	/* -t, --timestats */
			tflag = 1;
			break;
		case 'm':	/* -m, --dumpmem */
			mflag = 1;
			break;
		case 'p':	/* -p, --dumpspl */
			pflag = 1;
			break;
		case 'q':	/* -q, --dumpqueues */
			qflag = 1;
			break;
		case 'Q':	/* -Q, --quiet */
			output = 0;
			break;
		case 'H':	/* -H (obsolete) */
			Hflag = 1;
		case 'h':	/* -h, --help */
			help(argc, argv);
			exit(0);
		case 'V':	/* -V, --version */
			version(argc, argv);
			exit(0);
		case 55:	/* --copying */
			copying(argc, argv);
			exit(0);
		default:
			if (0 < c && c <= 32) {
				dflag = 1;	/* debug 1 flag */
				debug_mask |= (1 << (c - 1));
				break;
			}
			if (32 < c && c <= 35) {
				Dflag = 1;	/* debug 2 flag */
				debug_mask2 |= (1 << (c - 32));
				break;
			}
		case '?':
		      bad_option:
			optind--;
		      bad_nonoption:
			if (optind < argc && output) {
				fprintf(stderr, "%s: illegal syntax -- ", argv[0]);
				for (; optind < argc; optind++)
					fprintf(stderr, "%s ", argv[optind]);
				fprintf(stderr, "\n");
			}
			usage(argc, argv);
			exit(2);
		}
	}
	/* 
	 * don't ignore (permuted) non-option arguments 
	 */
	if (optind < argc)
		goto bad_nonoption;

	if (!mflag && !dflag && !sflag && !tflag && !pflag && !qflag && !cflag && !Cflag && !Sflag
	    && !Dflag && !Lflag && !Tflag) {
		usage(argc, argv);
		exit(1);
	}

	if ((fd = open(LOOP_CLONE, 0, 0)) < 0) {
		if (output)
			fprintf(stderr, LOOP_CLONE ": %s\n", strerror(-fd));
		exit(1);
	}
	if (dflag && (rslt = ioctl(fd, I_LIS_SDBGMSK, debug_mask)) < 0) {
		if (output)
			fprintf(stderr, LOOP_CLONE ": I_LIS_SDBGMSK: %s\n", strerror(-rslt));
		exit(1);
	}
	if (Dflag && (rslt = ioctl(fd, I_LIS_SDBGMSK2, debug_mask2)) < 0) {
		if (output)
			fprintf(stderr, LOOP_CLONE ": I_LIS_SDBGMSK2: %s\n", strerror(-rslt));
		exit(1);
	}
	if (sflag || tflag) {
		/* 
		 * printf("sizeof strstats: %d\n", sizeof(strstats)); 
		 */
		if ((rslt = ioctl(fd, I_LIS_GETSTATS, strstats)) < 0) {
			if (output)
				fprintf(stderr, LOOP_CLONE ": I_LIS_GETSTATS: %s\n",
					strerror(-rslt));
			exit(1);
		}
		if (output)
			LisShowStrStats();
	}
	if (Sflag) {
		if ((rslt = ioctl(fd, I_LIS_QRUN_STATS, &qrun_stats)) < 0) {
			if (output)
				fprintf(stderr, LOOP_CLONE ": I_LIS_QRUN_STATS: %s\n",
					strerror(-rslt));
			exit(1);
		}
		print_qrun_stats();
	}
	if (mflag) {
		if ((rslt = ioctl(fd, I_LIS_PRNTMEM, 0)) < 0) {
			if (output)
				fprintf(stderr, LOOP_CLONE ": I_LIS_PRNTMEM: %s\n",
					strerror(-rslt));
			exit(1);
		}
#ifdef QNX
		if (output)
			printf("The memory dump is in the /usr/lib/gcom/streams.log file\n");
#endif
	}
	if (pflag && (rslt = ioctl(fd, I_LIS_PRNTSPL, 0)) < 0) {
		if (output)
			fprintf(stderr, LOOP_CLONE ": I_LIS_PRNTSPL: %s\n", strerror(-rslt));
		exit(1);
	}
	if (qflag && (rslt = ioctl(fd, I_LIS_PRNTQUEUES, 0)) < 0) {
		if (output)
			fprintf(stderr, LOOP_CLONE ": I_LIS_PRNTQUEUES: %s\n", strerror(-rslt));
		exit(1);
	}
	if (cflag) {
		if (msgmemK && (rslt = ioctl(fd, I_LIS_SET_MAXMSGMEM, msgmemK * 1024)) < 0) {
			if (output)
				perror("ioctl I_LIS_SET_MAXMSGMEM");
			exit(1);
		}
		if ((rslt = ioctl(fd, I_LIS_GET_MAXMSGMEM, &msgmemK)) < 0) {
			if (output)
				perror("ioctl I_LIS_GET_MAXMSGMEM");
			exit(1);
		}
		if (output) {
			if ((msgmemK /= 1024))
				printf("Maximum memory to use for messages: %ldK\n", msgmemK);
			else
				printf("Maximum memory to use for messages: No limit\n");
		}
	}
	if (Cflag) {
		if (memK && (rslt = ioctl(fd, I_LIS_SET_MAXMEM, memK * 1024)) < 0) {
			if (output)
				perror("ioctl I_LIS_SET_MAXMEM");
			exit(1);
		}
		if ((rslt = ioctl(fd, I_LIS_GET_MAXMEM, &memK)) < 0) {
			if (output)
				perror("ioctl I_LIS_GET_MAXMEM");
			exit(1);
		}
		if (output) {
			if ((memK /= 1024))
				printf("Maximum total memory to use: %ldK\n", memK);
			else
				printf("Maximum total memory to use: No limit\n");
		}
	}

	if (Lflag) {
		rslt = ioctl(fd, I_LIS_LOCKS, &buf);
		if (rslt < 0) {
			if (output)
				perror("ioctl I_LIS_LOCKS");
			exit(1);
		}
		printf("%s", buf);
	}

	if (Tflag) {
		rslt = ioctl(fd, I_LIS_SEMTIME, &buf);
		if (rslt < 0) {
			if (output)
				perror("ioctl I_LIS_SEMTIME");
			exit(1);
		}
		printf("Semaphore wakeup latency histogram:\n");
		print_sem_time_hist(buf, rslt);
	}

	close(fd);

	return 0;
}				/* main */
