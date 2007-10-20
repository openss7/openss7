#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/ntp/RCS/ntp.c,v 9.0 1992/06/16 12:42:48 isode Rel $";
#endif	lint

/*
 *  $Header: /xtel/isode/isode/others/ntp/RCS/ntp.c,v 9.0 1992/06/16 12:42:48 isode Rel $
 *
 *
 *  $Log: ntp.c,v $
 * Revision 9.0  1992/06/16  12:42:48  isode
 * Release 8.0
 *
 */

/*
 * This program expects a list of host names.  It will send off a
 * network time protocol packet and print out the replies on the
 * terminal.
 * 
 * Example:
 *
 *  % ntp umd1.umd.edu
 *  Packet from: [128.8.10.1]
 *  Leap 0, version 1, mode Server, poll 6, precision -10 stratum 1 (WWVB)
 *  Synch Distance is 0000.1999  0.099991
 *  Synch Dispersion is 0000.0000  0.000000
 *  Reference Timestamp is a7bea6c3.88b40000 Tue Mar  7 14:06:43 1989
 *  Originate Timestamp is a7bea6d7.d7e6e652 Tue Mar  7 14:07:03 1989
 *  Receive Timestamp is   a7bea6d7.cf1a0000 Tue Mar  7 14:07:03 1989
 *  Transmit Timestamp is  a7bea6d8.0ccc0000 Tue Mar  7 14:07:04 1989
 *  Input Timestamp is     a7bea6d8.1a77e5ea Tue Mar  7 14:07:04 1989
 *  umd1: delay:0.019028 offset:-0.043890
 *  Tue Mar  7 14:07:04 1989
 *
 */

#include "ntp.h"

char *modename[8] = {
	"Unspecified",
	"Symmetric Active",
	"Symmetric Passive",
	"Client",
	"Server",
	"Broadcast",
	"Reserved-1",
	"Reserved-2"
	};

#define RETRY_COUNT	2	/* number of times we want to retry */
#define TIME_OUT        10	/* time to wait for reply, in secs */

extern void tstamp();

struct sockaddr_in isock = {AF_INET};
struct sockaddr_in dst = {AF_INET};
struct servent *sp;
extern double ul_fixed_to_double(), s_fixed_to_double();
extern int errno;
int set, verbose, force;
int debug;
extern int optind;
char	*myname;

main(argc, argv)
	int argc;
	char *argv[];
{
	struct hostent *hp;
	struct in_addr clock_host;
	struct l_fixedpt in_timestamp;
	static struct ntpdata ntp_data;
	struct ntpdata *pkt = &ntp_data;
	struct timeval tp, timeout;
	int host, n, retry, s;
	fd_set readfds;
	int dstlen = sizeof(dst);
	double t1, t2, t3, t4, offset, delay;
	char ref_clock[5];
	time_t net_time;
	ref_clock[4] = '\0';

	myname = argv[0];
	timeout.tv_sec = TIME_OUT;
	timeout.tv_usec = 0;
	retry = RETRY_COUNT;

	sp = getservbyname("ntp", "udp");
	if (sp == NULL) {
		(void) fprintf(stderr, "udp/ntp: service unknown; using default %d\n",
			NTP_PORT);
		dst.sin_port = htons(NTP_PORT);
	} else
		dst.sin_port = sp->s_port;

	dst.sin_family = AF_INET;
	while ((n = getopt(argc, argv, "vsf")) != EOF) {
		switch (n) {
		case 'v':
			verbose = 1;
			break;
		case 's':
			set = 1;
			break;
		case 'f':
			force = 1;
			break;
		}
	}
	for (host = optind; host < argc; ++host) {
		long	HostAddr;

		if (argv[host] == NULL)
			continue;

		hp = NULL;
		HostAddr = inet_addr(argv[host]);
		dst.sin_addr.s_addr = (u_long) HostAddr;
		if (HostAddr == -1) {
			hp = gethostbyname(argv[host]);
			if (hp == NULL) {
				(void) fprintf(stderr, "\nNo such host: %s\n",
					argv[host]);
				continue;
			}
			bcopy(hp->h_addr, (char *) &dst.sin_addr,hp->h_length);
		}

		bzero((char *)pkt, sizeof(ntp_data));

		pkt->status = NTPVERSION_1 | NO_WARNING | MODE_CLIENT;
		pkt->stratum = UNSPECIFIED;
		pkt->ppoll = 0;

		if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
			perror("ntp socket");
			exit(1);
		}

		FD_ZERO(&readfds);
		FD_SET(s, &readfds);	/* since it's always modified on ret */

		if (connect(s, (struct sockaddr *)&dst, dstlen)) {
			perror("connect");
			exit(1);
		}

		/*
		 * Needed to fill in the time stamp fields
		 */
		(void) gettimeofday(&tp, (struct timezone *) 0);
		tstamp(&pkt->xmt, &tp);

		if (send(s, (char *) pkt, sizeof(ntp_data), 0) < 0) {
			perror("send");
			exit(1);
		}

		/*
		 * Wait for the reply by watching the file descriptor 
		 */
		if ((n = select(FD_SETSIZE, & readfds, (fd_set *) 0,
				(fd_set *) 0, &timeout)) < 0) {
			perror("ntp select");
			exit(1);
		}

		if (n == 0) {
			(void) fprintf(stderr,"*Timeout*\n");
			if (--retry)
				--host;
			else {
				(void) fprintf(stderr,"Host %s is not responding\n",
				       argv[host]);
				retry = RETRY_COUNT;
			}
			continue;
		}
		if ((recvfrom(s, (char *) pkt, sizeof(ntp_data), 0,
			      (struct sockaddr *) &isock, &dstlen)) < 0) {
			perror("recvfrom");
			exit(1);
		}
		(void) gettimeofday(&tp, (struct timezone *) 0);
		tstamp(&in_timestamp, &tp);

		(void) close(s);
		if (verbose) {
			(void) printf("Packet from: [%s]\n", inet_ntoa(isock.sin_addr));
			(void) printf("Leap %d, version %d, mode %s, poll %d, precision %d stratum %d",
			       (pkt->status & LEAPMASK) >> 6, 
			       (pkt->status & VERSIONMASK) >> 3,
			       modename[pkt->status & MODEMASK],
			       pkt->ppoll, pkt->precision, pkt->stratum);
			switch (pkt->stratum) {
			case 0:
			case 1:
				(void) strncpy(ref_clock, (char *) &pkt->refid, 4);
				ref_clock[4] = '\0';
				(void) printf(" (%s)\n", ref_clock);
				break;
			default:
				clock_host.s_addr = (u_long) pkt->refid;
				(void) printf(" [%s]\n", inet_ntoa(clock_host));
				break;
			}
			(void) printf("Synch Distance is %04X.%04x  %f\n",
			       ntohs(pkt->distance.int_part),
			       ntohs(pkt->distance.fraction),
			       s_fixed_to_double(&pkt->distance));

			(void) printf("Synch Dispersion is %04X.%04x  %f\n",
			       ntohs(pkt->dispersion.int_part),
			       ntohs(pkt->dispersion.fraction),
			       s_fixed_to_double(&pkt->dispersion));

			net_time = ntohl(pkt->reftime.int_part) - JAN_1970;
			(void) printf("Reference Timestamp is %08lx.%08lx %s",
			       ntohl(pkt->reftime.int_part),
			       ntohl(pkt->reftime.fraction),
			       ctime(&net_time));

			net_time = ntohl(pkt->org.int_part) - JAN_1970;
			(void) printf("Originate Timestamp is %08lx.%08lx %s",
			       ntohl(pkt->org.int_part),
			       ntohl(pkt->org.fraction),
			       ctime(&net_time));

			net_time = ntohl(pkt->rec.int_part) - JAN_1970;
			(void) printf("Receive Timestamp is   %08lx.%08lx %s",
			       ntohl(pkt->rec.int_part),
			       ntohl(pkt->rec.fraction),
			       ctime(&net_time));

			net_time = ntohl(pkt->xmt.int_part) - JAN_1970;
			(void) printf("Transmit Timestamp is  %08lx.%08lx %s",
			       ntohl(pkt->xmt.int_part),
			       ntohl(pkt->xmt.fraction),
			       ctime(&net_time));
		}
		t1 = ul_fixed_to_double(&pkt->org);
		t2 = ul_fixed_to_double(&pkt->rec);
		t3 = ul_fixed_to_double(&pkt->xmt);
		t4 = ul_fixed_to_double(&in_timestamp);

		net_time = ntohl(in_timestamp.int_part) - JAN_1970;
		if (verbose) 
			(void) printf("Input Timestamp is     %08lx.%08lx %s",
			       ntohl(in_timestamp.int_part),
			       ntohl(in_timestamp.fraction), ctime(&net_time));

		delay = (t4 - t1) - (t3 - t2);
		offset = (t2 - t1) + (t3 - t4);
		offset = offset / 2.0;
		(void) printf("%.20s: delay:%f offset:%f  ",
		       hp ? hp->h_name : argv[host],
		       delay, offset);
		net_time = ntohl(pkt->xmt.int_part) - JAN_1970 + delay;
		(void) fputs(ctime(&net_time), stdout);
		(void)fflush(stdout);

		if (!set)
			continue;

		if ((offset < 0 ? -offset  : offset) > WAYTOOBIG && !force) {
			(void) fprintf(stderr, "Offset too large - use -f option to force clock set.\n");
			continue;
		}

		if (pkt->status & LEAPMASK == ALARM) {
			(void) fprintf(stderr, "Can't set time from %s - unsynchronized\n",
				argv[host]);
			continue;
		}

		/* set the clock */
		(void) gettimeofday(&tp, (struct timezone *) 0);
		offset += tp.tv_sec;
		offset += tp.tv_usec / 1000000.0;
		tp.tv_sec = offset;
		tp.tv_usec = (offset - tp.tv_sec) * 1000000.0;

		if (settimeofday(&tp, (struct timezone *) 0)) {
			perror("Can't set time (settimeofday)");
		} else
			set = 0;
	}			/* end of for each host */
       return 0;
}				/* end of main */
