/*
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 * MA 02139, USA.
 * 
 */
#ident "@(#) LiS streams.c 2.18 12/27/03 15:13:10 "
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
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * Storage Declarations
 */
unsigned long strstats[STRMAXSTAT][4]; /* the stats */

char		*prog_name ;

int		cflag = 0;
int		Cflag = 0;
int		mflag = 0;
int		dflag = 0;
int		Dflag = 0;
int		sflag = 0;
int		Sflag = 0;
int		tflag = 0;
int		pflag = 0;
int		qflag = 0;
int		Hflag = 0;

long		memK = 0 ;
long		msgmemK = 0 ;

lis_qrun_stats_t	qrun_stats ;

/*
 * Names of stat slots
 */
itemname_t lis_itemnames[] =
{
    {MODUSE,		MODUSESTR},
    {MEMALLOCS,		MEMALLOCSSTR},
    {MEMFREES,		MEMFREESSSTR},
    {MEMALLOCD,		MEMALLOCDSTR},
    {MEMLIM,		MEMLIMSTR},
    {HEADERS,		HEADERSSTR},
    {FREEHDRS,		FREEHDRSSTR},
    {DATABS,		DATABSSTR},
    {DBLKMEM,		DBLKMEMSTR},
    {MSGMEMLIM,		MSGMEMLIMSTR},
    {BUFCALLS,		BUFCALLSSTR},
    {MSGSQD,		MSGSQDSTR},
    {MSGQDSTRHD,	MSGQDSTRHDSTR},
    {CANPUTS,		CANPUTSSTR},
    {QUEUES,		QUEUESSTR},
    {QSCHEDS,		QSCHEDSSTR},
    {OPENTIME,		OPENTIMESTR},
    {CLOSETIME,		CLOSETIMESTR},
    {READTIME,		READTIMESTR},
    {WRITETIME,		WRITETIMESTR},
    {IOCTLTIME,		IOCTLTIMESTR},
    {GETMSGTIME,	GETMSGTIMESTR},
    {PUTMSGTIME,	PUTMSGTIMESTR},
    {POLLTIME,		POLLTIMESTR},
    {LOCKCNTS,		LOCKCNTSSTR}
};

char *lis_countnames[] =
{
    CURRENTSTR, TOTALSTR, MAXIMUMSTR, FAILURESSTR
};

void
LisShowStrStats(void)
{
    int i;
    int inx;
    int found_time = 0 ;
    int have_hdr = 0 ;

    if (!sflag && !tflag) return ;	/* no stats desired */

    /* Print statistics */
    for (i = 0; i < STRMAXSTAT; i++)
    {
	int j;

	if (lis_itemnames[i].name == NULL) continue ;

	inx = lis_itemnames[i].stats_inx ;
	if (inx == OPENTIME)
	{				/* time for a new header */
	    found_time = 1 ;
	    have_hdr = 0 ;
	}

	if (found_time)			/* got to time stats */
	{
	    if (!tflag) break ;		/* not supposed to print them */
	}
	else				/* still in regular stats */
	if (!sflag)			/* not supposed to print them */
	    continue ;

	if (!have_hdr)
	{
	    int	k ;
	    have_hdr = 1 ;
	    printf("\n%-28s  ", "");
	    for (k = 0; k < 4; k++)
		printf("%12s", lis_countnames[k]) ;
	    printf("\n");
	}

	printf("%-28s: ", lis_itemnames[i].name);
	for (j = 0; j < 4; j++)
	    printf("%12lu", strstats[inx][j]);
	printf("\n");
    }


} /*LisShowStrStats*/

void print_qrun_stats(void)
{
    int		cpu ;

    printf("N-CPUs N-Qrunners N-Running N-Requested\n"
	   "%6u %10u %9u %10u\n",
	    qrun_stats.num_cpus, qrun_stats.num_qrunners,
	    qrun_stats.queues_running, qrun_stats.runq_req_cnt) ;

    printf(
"\n"
"CPU   Qsched-Cnts Qsched-ISR Svc-Q-Cnts Qrun-Wkups Qrun-Cnts Active Thread-PID"
"\n"
          ) ;
    for (cpu = 0; cpu < qrun_stats.num_cpus; cpu++)
    {
	printf("%3u   %11lu %10lu %10lu %10lu %9lu %6u %10u\n",
		cpu,
		qrun_stats.setqsched_cnts[cpu],
		qrun_stats.setqsched_isr_cnts[cpu],
		qrun_stats.runq_cnts[cpu],
		qrun_stats.runq_wakeups[cpu],
		qrun_stats.queuerun_cnts[cpu],
		qrun_stats.active_flags[cpu], qrun_stats.runq_pids[cpu]) ;
    }
}


void print_debug_bits(void)
{
    printf("-d<debug-bits -- \n") ;
    printf("DEBUG_OPEN          0x00000001\n");
    printf("DEBUG_CLOSE         0x00000002\n");
    printf("DEBUG_READ          0x00000004\n");
    printf("DEBUG_WRITE         0x00000008\n");
    printf("DEBUG_IOCTL         0x00000010\n");
    printf("DEBUG_PUTNEXT       0x00000020\n");
    printf("DEBUG_STRRPUT       0x00000040\n");
    printf("DEBUG_SIG           0x00000080\n");
    printf("DEBUG_PUTMSG        0x00000100\n");
    printf("DEBUG_GETMSG        0x00000200\n");
    printf("DEBUG_POLL          0x00000400\n");
    printf("DEBUG_LINK          0x00000800\n");
    printf("DEBUG_MEAS_TIME     0x00001000\n");
    printf("DEBUG_MEM_LEAK      0x00002000\n");
    printf("DEBUG_FLUSH         0x00004000\n");
    printf("DEBUG_FATTACH       0x00008000\n");
    printf("DEBUG_SAFE          0x00010000\n");
    printf("DEBUG_TRCE_MSG      0x00020000\n");
    printf("DEBUG_CLEAN_MSG     0x00040000\n");
    printf("DEBUG_SPL_TRACE     0x00080000\n");
    printf("DEBUG_MP_ALLOC      0x00100000\n");
    printf("DEBUG_MP_FREEMSG    0x00200000\n");
    printf("DEBUG_MALLOC        0x00400000\n");
    printf("DEBUG_MONITOR_MEM   0x00800000\n");
    printf("DEBUG_DMP_QUEUE     0x01000000\n");
    printf("DEBUG_DMP_MBLK      0x02000000\n");
    printf("DEBUG_DMP_DBLK      0x04000000\n");
    printf("DEBUG_DMP_STRHD     0x08000000\n");
    printf("DEBUG_VOPEN         0x10000000\n");
    printf("DEBUG_VCLOSE        0x20000000\n");
    printf("DEBUG_ADDRS         0x80000000\n");
    printf("-D<debug-bits -- \n") ;
    printf("DEBUG_SNDFD         0x00000001\n");
    printf("DEBUG_CP (code path)0x00000002\n");
    printf("DEBUG_CACHE         0x00000004\n");

} /* print_debug_bits */

void print_options(void)
{
    printf("%s <options> [start] [stop]\n", prog_name) ;
    printf("  -c[<kb>] print/set max message memory usage for LiS\n");
    printf("  -C[<kb>] print/set max memory usage for LiS\n");
    printf("  -d<msk>  debug mask (in hex)\n");
    printf("  -D<msk>  second debug mask (in hex)\n");
    printf("  -s       print streams memory stats\n");
    printf("  -S       print streams queue run stats\n");
    printf("  -m       print memory allocation to log file (from kernel)\n");
    printf("  -p       print SPL trace buffer to log file (from kernel)\n");
    printf("  -q       print all queues to log file (from kernel)\n");
    printf("  -t       print streams timing stats\n");
    printf("  start    starts the streams subsystem\n");
    printf("  stop     stops the streams subsystem\n");
    printf("  status   reports status of streams subsystem\n");

    if (Hflag)
	print_debug_bits() ;

} /* print_options */

int main( int argc, char *argv[])
{
    int		fd ;
    int		rslt ;
    int		c;
    unsigned long debug_mask;
    unsigned long debug_mask2;
    extern char *optarg;

    prog_name = argv[0] ;

    /* 
     * Check for mnemonic options.  These must appear alone
     * after the command name.
     */
    if (argc >= 2 && argv[1] != NULL)
    {
	if (strcmp(argv[1], "start") == 0)
	{
	    exit( system("/usr/sbin/strms_up") );
	}

	if (strcmp(argv[1], "stop") == 0)
	{
	    exit( system("/usr/sbin/strms_down") );
	}

	if (strcmp(argv[1], "status") == 0)
	{
	    exit( system("/usr/sbin/strms_status") );
	}
    }
    
    while(( c = getopt(argc, argv, "pqsStmhHd:D:c::C::")) != -1)
    {
	switch (c)
	{
	    case 'c':
	        cflag = 1;
		if (optarg != NULL)
		{
		    if ( sscanf(optarg, "%li", &msgmemK) != 1 )
		    {
			printf("invalid argument for -c: \"%s\"\n", optarg);
			exit(1);
		    }
		}
		break ;

	    case 'C':
	        Cflag = 1;
		if (optarg != NULL)
		{
		    if ( sscanf(optarg, "%li", &memK) != 1 )
		    {
			printf("invalid argument for -C: \"%s\"\n", optarg);
			exit(1);
		    }
		}
		break ;

	    case 'd':
	        dflag = 1;
	        if ( sscanf(optarg, "%lx", &debug_mask) != 1 )
	        {
	            printf("need hex argument for -d\n");
	            exit(1);
	        }
		break;

	    case 'D':
	        Dflag = 1;
	        if ( sscanf(optarg, "%lx", &debug_mask2) != 1 )
	        {
	            printf("need hex argument for -D\n");
	            exit(1);
	        }
		break;

	    case 's':
	        sflag = 1;
	        break;

	    case 'S':
	        Sflag = 1;
	        break;

	    case 't':
	        tflag = 1;
	        break;

	    case 'm':
	        mflag = 1;
	        break;

	    case 'p':
	    	pflag = 1 ;
		break ;

	    case 'q':
	    	qflag = 1 ;
		break ;

	    case 'H':
		Hflag = 1 ;
	    case 'h':
	    case '?':
	    default:
		print_options() ;
		exit(1);
	}
    }

    if (   !mflag && !dflag && !sflag && !tflag && !pflag && !qflag
	&& !cflag && !Cflag && !Sflag && !Dflag)
    {
	print_options() ;
	exit(1);
    }

    fd = open(LOOP_CLONE, 0, 0) ;
    if (fd < 0)
    {
	printf(LOOP_CLONE ": %s\n", strerror(-fd)) ;
	exit(1) ;
    }

    if ( dflag )
    {
	rslt = ioctl(fd, I_LIS_SDBGMSK, debug_mask) ;
	if (rslt < 0)
	{
	    printf(LOOP_CLONE ": I_LIS_SDBGMSK: %s\n", strerror(-rslt)) ;
	    exit(1) ;
	}
    }

    if ( Dflag )
    {
	rslt = ioctl(fd, I_LIS_SDBGMSK2, debug_mask2) ;
	if (rslt < 0)
	{
	    printf(LOOP_CLONE ": I_LIS_SDBGMSK2: %s\n", strerror(-rslt)) ;
	    exit(1) ;
	}
    }

    if ( sflag || tflag )
    {
	/* printf("sizeof strstats: %d\n", sizeof(strstats)); */
	rslt = ioctl(fd, I_LIS_GETSTATS, strstats) ;
	if (rslt < 0)
	{
	    printf(LOOP_CLONE ": I_LIS_GETSTATS: %s\n", strerror(-rslt)) ;
	    exit(1) ;
	}
	LisShowStrStats();
    }

    if ( Sflag )
    {
	rslt = ioctl(fd, I_LIS_QRUN_STATS, &qrun_stats) ;
	if (rslt < 0)
	{
	    printf(LOOP_CLONE ": I_LIS_QRUN_STATS: %s\n", strerror(-rslt)) ;
	    exit(1) ;
	}
	print_qrun_stats() ;
    }

    if ( mflag )
    {
	rslt = ioctl(fd, I_LIS_PRNTMEM, 0) ;
	if (rslt < 0)
	{
	    printf(LOOP_CLONE ": I_LIS_PRNTMEM: %s\n", strerror(-rslt)) ;
	    exit(1) ;
	}
#ifdef QNX
	printf("The memory dump is in the /usr/lib/gcom/streams.log file\n");
#endif
    }

    if ( pflag )
    {
	rslt = ioctl(fd, I_LIS_PRNTSPL, 0) ;
	if (rslt < 0)
	{
	    printf(LOOP_CLONE ": I_LIS_PRNTSPL: %s\n", strerror(-rslt)) ;
	    exit(1) ;
	}
    }

    if ( qflag )
    {
	rslt = ioctl(fd, I_LIS_PRNTQUEUES, 0) ;
	if (rslt < 0)
	{
	    printf(LOOP_CLONE ": I_LIS_PRNTQUEUES: %s\n", strerror(-rslt)) ;
	    exit(1) ;
	}
    }

    if ( cflag )
    {
	if (msgmemK)
	{
	    rslt = ioctl(fd, I_LIS_SET_MAXMSGMEM, msgmemK * 1024) ;
	    if (rslt < 0)
	    {
		perror("ioctl I_LIS_SET_MAXMSGMEM") ;
		exit(1) ;
	    }

	}

	rslt = ioctl(fd, I_LIS_GET_MAXMSGMEM, &msgmemK) ;
	if (rslt < 0)
	{
	    perror("ioctl I_LIS_GET_MAXMSGMEM") ;
	    exit(1) ;
	}

	if (msgmemK == 0)
	    printf("Maximum memory to use for messages: No limit\n") ;
	else
	{
	    msgmemK /= 1024 ;
	    printf("Maximum memory to use for messages: %ldK\n", msgmemK) ;
	}
    }

    if ( Cflag )
    {
	if (memK)
	{
	    rslt = ioctl(fd, I_LIS_SET_MAXMEM, memK * 1024) ;
	    if (rslt < 0)
	    {
		perror("ioctl I_LIS_SET_MAXMEM") ;
		exit(1) ;
	    }

	}

	rslt = ioctl(fd, I_LIS_GET_MAXMEM, &memK) ;
	if (rslt < 0)
	{
	    perror("ioctl I_LIS_GET_MAXMEM") ;
	    exit(1) ;
	}

	if (memK == 0)
	    printf("Maximum total memory to use: No limit\n") ;
	else
	{
	    memK /= 1024 ;
	    printf("Maximum total memory to use: %ldK\n", memK) ;
	}
    }

    close(fd) ;

    return 0;

} /* main */
