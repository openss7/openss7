/*******************************************************************************/
/*        This code is provided for example purposes only and does not         */
/*                 comprise part of the supported product.                     */
/*******************************************************************************/

/*******************************************************************************/
/* rfclient                                                                    */
/*                                                                             */
/* This is a client program provided to demonstrate the operation of the TLI   */
/* interface when used over RFC1006.  It can use used in conjunction with the  */
/* rfcserver program also supplied.                                            */
/*                                                                             */
/* This program opens a TLI connection in non-blocking mode, waits for a set   */
/* amount of data to be transferred, and then disconnects the connection.      */
/*                                                                             */
/* The user invokes the program using the command                              */
/*                                                                             */
/*   hostname% rfcclient remotehostname <tsel>                                 */
/*                                                                             */
/* where tsel is the Transport Selector on which the server process is         */
/* listening.  In this example it is assumend to be a character string.        */
/*                                                                             */
/*******************************************************************************/

#include <sys/tihdr.h>
#include <stropts.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define MAXDATA	    1000000
#define BUFSIZE	    1024
#define MAXADDRLEN  42

/*
 * The TSEL values in this example are hardcoded.  In a normal application
 * they would be configurable.
 */
#define TSEL	    "trs"

extern int t_errno;

main(int argc, char **argv)
{
	struct hostent *hostinfo;
	struct t_bind *sndbind, *rcvbind;
	struct t_call *call, *rcall;
	struct t_discon *discon;
	int fd, tslen, nslen, flags, nbytes, cc, i;
	char buf[BUFSIZE], remote_tsel[32];
	unsigned char taddr[MAXADDRLEN], ip_addr[4];

	/* 
	 * Get the IP address of the remote host from the supplied hostname.
	 */
	if (argc < 2 || argc > 3) {
		printf("Usage: tli/rfcclient remotehost <remote_tsel>\n");
		exit(1);
	}

	if ((hostinfo = gethostbyname(argv[1])) == NULL) {
		perror("gethostbyname failed");
		exit(2);
	}
	memcpy(ip_addr, *hostinfo->h_addr_list, 4);

	/* Read the TSEL - if non present this will default to "trs" */
	if (argc == 3) {
		strcpy(remote_tsel, argv[2]);
	} else {
		strcpy(remote_tsel, TSEL);
	}

	/* 
	 * Print out remote host information.
	 */
	printf("tli/rfcclient - remote host %s: address %d.%d.%d.%d, TSEL \"%s\"\n",
	       argv[1], ip_addr[0], ip_addr[1], ip_addr[2], ip_addr[3], remote_tsel);

	/* 
	 * Open the /dev/otk6 device.  Open it non-blocking in this example.
	 */

	if ((fd = t_open("/dev/otk6", O_RDWR | O_NONBLOCK, (struct t_info *) NULL)) < 0) {
		t_error("t_open /dev/otk6 failed");
		exit(3);
	}

	/* 
	 * Allocate t_bind structures for the address to be bound to and the
	 * address actually bound.
	 */
	if ((sndbind = (struct t_bind *) t_alloc(fd, T_BIND, T_ALL)) == NULL) {
		t_error("t_alloc sndbind");
		t_close(fd);
		exit(4);
	}

	if ((rcvbind = (struct t_bind *) t_alloc(fd, T_BIND, T_ALL)) == NULL) {
		t_error("t_alloc rcvbind");
		t_close(fd);
		exit(5);
	}

	/* 
	 * Now bind to the address.
	 *
	 * Addresses are in nebuf format.  This is an array as follows:
	 *
	 * [tsaplen][tsap..............][nsaplen][nsap.................]
	 *
	 * where tsaple and nsaplen are one byte each
	 */

	/* 
	 * In this case we assume the TSEL is a string of characters - it
	 * could be numeric.
	 */
	tslen = sizeof(TSEL) - 1;	/* Don't include \0 as part of TSEL */
	taddr[0] = (unsigned char) tslen;
	memcpy(&taddr[1], TSEL, tslen);

	/* 
	 * We do not have to supply an NSAP since we will bind to the default.
	 */
	nslen = 0;
	taddr[tslen + 1] = (unsigned char) nslen;

	sndbind->addr.len = tslen + nslen + 2;
	sndbind->addr.buf = (char *) taddr;
	sndbind->qlen = 0;	/* As a client we will not listen. */

	/* 
	 * Initialize the t_bind for returning the bound address.
	 */
	rcvbind->addr.maxlen = MAXADDRLEN;
	rcvbind->addr.buf = (char *) taddr;

	/* 
	 * Now do the bind.
	 */

	if (t_bind(fd, sndbind, rcvbind) < 0) {
		t_error("t_bind failed");
		t_close(fd);
		exit(6);
	}
	printf("tli/rfcclient - bind completed successfully\n");

	/* 
	 * Allocate a t_call structure to host the call information.
	 */

	if ((call = (struct t_call *) t_alloc(fd, T_CALL, T_ALL)) == NULL) {
		t_error("t_alloc failed call");
		t_close(fd);
		exit(7);
	}

	/* 
	 * Allocate a t_call structure to hold the returned call information.
	 */

	if ((rcall = (struct t_call *) t_alloc(fd, T_CALL, T_ALL)) == NULL) {
		t_error("t_alloc failed rcall");
		t_close(fd);
		exit(7);
	}

	/* 
	 * Called address is the same format as the bound address.  However,
	 * we need to supply both an NSAP and a TSEL.  The NSAP will be the IP
	 * address of the remote system.
	 */

	/* 
	 * As before assume that the TSEL is a character string.
	 */

	tslen = strlen(remote_tsel);
	taddr[0] = (unsigned char) tslen;
	memcpy(&taddr[1], remote_tsel, tslen);

	/* 
	 * The IP address (ie. the NSAP) is alwasy stored in 4 bytes.
	 */

	nslen = 4;
	taddr[tslen + 1] = (unsigned char) nslen;
	memcpy(&taddr[tslen + 2], ip_addr, 4);

	call->addr.len = tslen + nslen + 2;
	call->addr.buf = (char *) taddr;
	call->opt.buf = NULL;	/* No options are supported by RFC1006 */
	call->opt.len = 0;
	call->udata.len = 0;
	call->sequence = 0;

	/* 
	 * Now attempt a connection to the remote system.  If t_connect
	 * fails with t_errno = TNODATA then it means that the connection
	 * has not completed yet.
	 */
	printf("tli/rfcclient - doing connect.....\n");
	if (t_connect(fd, call, rcall) < 0) {
		if (t_errno == TNODATA) {
			printf("tli/rfcclient - waiting on connection\n");
		} else {
			t_error("t_connect failed");
			t_close(fd);
			exit(8);
		}
	}

	/* 
	 * Wait in a loop for the connection to complete.  We could also
	 * do other useful things here in a real application.
	 */
	while (t_rcvconnect(fd, rcall) < 0) {
		/* 
		 * Need to check to see if this is a "real" error condition
		 * or not.
		 */
		switch (t_errno) {
		case TNODATA:	/* Still no connection */
			printf("tli/rfcclient - waiting on connection\n");
			sleep(1);
			continue;
		case TLOOK:	/* This is most likely a disconnect */
			switch (t_look(fd)) {
			case T_DATA:
				continue;
			case T_DISCONNECT:
				printf("tli/rfcclient - disconnect received\n");
				if ((discon =
				     (struct t_discon *) t_alloc(fd, T_DIS, T_ALL)) == NULL) {
					t_error("t_alloc failed discon");
					exit(9);
				}
				if (t_rcvdis(fd, discon) < 0) {
					t_error("t_rcvdis failed");
					exit(10);
				}
				t_close(fd);
				exit(0);
			case T_EXDATA:
			case T_LISTEN:
			case T_CONNECT:
			case T_UDERR:
			case T_ORDREL:
			default:
				fprintf(stderr, "tli/rfcclient - protocol error\n");
				t_close(fd);
				exit(11);
			}
		default:
			t_error("t_rcvconnect failed");
			t_close(fd);
			exit(12);
		}
	}

	/* 
	 * As an example receive some data from the remote system.  Wait
	 * until MAXDATA bytes of data have been received and then disconnect.
	 */

	cc = 0;
	flags = 0;
	while (cc < MAXDATA) {
		if ((nbytes = t_rcv(fd, buf, BUFSIZE, &flags)) < 0) {

			/* 
			 * Need to check if this is a real error condition
			 * or not.
			 */

			switch (t_errno) {
			case TNODATA:
				printf("tli/rfcclient - no data to be read...\n");
				sleep(1);
				continue;
			case TLOOK:
				switch (t_look(fd)) {
				case T_DATA:
					printf("tli/rfcclient - T_DATA...\n");
					sleep(1);
					continue;
				case T_DISCONNECT:
					printf
					    ("tli/rfcclient - disconnect received during data reception\n");
					if ((discon =
					     (struct t_discon *) t_alloc(fd, T_DIS,
									 T_ALL)) == NULL) {
						t_error("t_alloc failed discon");
						t_close(fd);
						exit(13);
					}
					if (t_rcvdis(fd, discon) < 0) {
						t_error("t_rcvdis failed");
						t_close(fd);
						exit(14);
					}
					exit(0);
				case T_EXDATA:
				case T_LISTEN:
				case T_CONNECT:
				case T_UDERR:
				case T_ORDREL:
				default:
					fprintf(stderr, "tli/rfcclient - protocol error\n");
					t_close(fd);
					exit(15);
				}
			default:
				t_error("t_rcv failed");
				t_close(fd);
				exit(16);
			}
		} else {
			/* Data has been received */
			printf("tli/rfcclient - %d bytes of data received\n", nbytes);
			cc += nbytes;
		}
	}

	/* 
	 * All data received now disconnect the connection.
	 */

	printf("tli/rfcclient - %d bytes received - disconnecting\n", cc);
	if (t_snddis(fd, NULL) < 0) {
		t_error("t_disconnect failed");
		exit(17);
	}
	t_close(fd);
	exit(0);
}
