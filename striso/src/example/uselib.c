/*
#ident "@(#) uselib.c 1.2 24 Jan 1994 SMI"
 *
 * This program show how to use the OSI library
 * Usage:
 *  -n <NSAP>
 *  Gives the host name associated to this NSAP
 *  -h <HOSTNAME>
 *  Gives the NSAP associated to this HOSTNAME
 *  -s <SERVICE NAME>
 *  Gives the TSEL associated to this SERVICE NAME
 *  -l
 *  Gives the CLNS NSAP of my OSI stack
 *  -o
 *  Gives the CONS NSAP of my OSI stack
 */
#include <stdio.h>
#include <stdlib.h>

#include <osi.h>

int
main(int argc, char *argv[])
{
	char *s_nsap, *s_serv, *s_host;
	char nsap[BUFLEN], host[BUFLEN], tsel[BUFLEN];
	int len, get_clns_nsap = 0, get_cons_nsap = 0, nsap_len = BUFLEN, host_len =
	    BUFLEN, tsel_len = BUFLEN;
	int c;
	extern char *optarg;
	extern int optbind;

	s_nsap = (char *) NULL;
	s_serv = (char *) NULL;
	s_host = (char *) NULL;

	while ((c = getopt(argc, argv, "n:h:s:lo")) != EOF) {
		switch (c) {
		case 'n':
			s_nsap = optarg;
			break;
		case 'h':
			s_host = optarg;
			break;
		case 's':
			s_serv = optarg;
			break;
		case 'l':
			get_clns_nsap = 1;
			break;
		case 'o':
			get_cons_nsap = 1;
			break;
		default:
			printf("Usage: -n <nsap> -h <hostname> -s <tsel> -l -o\n", argv[0]);
			break;
		}
	}
	if (s_nsap != (char *) NULL) {
		printf("Looking for hostname with NSAP = %s\n", s_nsap);
		if ((len = getnamebynsap(s_nsap, host, host_len)) < 0) {
			perror("getnamebynsap");
		} else {
			printf("\tFound HOSTNAME = %s with len = %d\n", host, len);
		}
	}

	if (s_host != (char *) NULL) {
		printf("Looking for NSAP with hostname = %s\n", s_host);
		if ((len = getnsapbyname(s_host, nsap, nsap_len)) < 0) {
			perror("getnsapbyname");
		} else {
			printf("\tFound NSAP = %s with len = %d\n", nsap, len);
		}
	}
	if (s_serv != (char *) NULL) {
		printf("Looking for TSEL with service = %s\n", s_serv);
		if ((len = gettselbyname(s_serv, tsel, tsel_len)) < 0) {
			perror("gettselbyname");
		} else {
			printf("\tFound TSEL = %s with len = %d\n", tsel, len);
		}
	}
	nsap[0] = '\0';
	if (get_clns_nsap == 1) {
		printf("Looking for the CLNS NSAP...\n");
		if ((len = getmyclnsnsap(nsap, nsap_len)) < 0) {
			perror("getmyclnsnsap");
		} else {
			printf("CLNS NSAP = %s with len = %d\n", nsap, len);
		}
	}
	nsap[0] = '\0';
	if (get_cons_nsap == 1) {
		printf("Looking for the CONS NSAP...\n");
		if ((len = getmyconsnsap(nsap, nsap_len)) < 0) {
			perror("getmyconsnsap");
		} else {
			printf("CONS NSAP = %s with len = %d\n", nsap, len);
		}
	}
	exit (0);
}

