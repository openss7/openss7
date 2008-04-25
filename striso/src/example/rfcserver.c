/*******************************************************************************/
/*        This code is provided for example purposes only and does not         */
/*                 comprise part of the supported product.                     */
/*******************************************************************************/

/*******************************************************************************/
/* rfcserver                                                                   */
/*                                                                             */
/* This is a server program provided to demonstrate the operation of the TLI   */
/* interface when used over RFC1006.  It can be used in conjunction with the   */
/* rfcclient program also supplied.                                            */
/*                                                                             */
/* This program listens for incoming calls.  When and incoming call is         */
/* received it is accepted on a different file descriptor and a new process    */
/* is forked to transfer data to the client.                                   */
/*                                                                             */
/* The forked process transmits a set amount of data and then waits for the    */
/* client to disconnect.                                                       */
/*                                                                             */
/* The user invokes the program using the command                              */
/*                                                                             */
/*   hostname% rfcserver <tsel>                                                */
/*                                                                             */
/* where tsel is the Transport Selector on which this server process is        */
/* bound.  In this example it is assumed to be a character string.             */
/*                                                                             */
/*******************************************************************************/

#include <sys/tiuser.h>
#include <stropts.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>

#define MAXDATA		1000000	/* This is an arbitrary value */
#define BUFSIZE		1024
#define MAXADDRLEN	42
#define TSEL		"trs"	/* This is the default TSEL to listen on */

extern intt_errno;

/*
 * accept_call() - accepts calls from clients.
 *
 * Unless an internal error occurs only a disconnect received from the remote
 * system can cause the t_accept call to fail.
 *
 * Input -
 *		listen_fd - endpoint where server is currently listening.
 *		call - t_call structure contianing details of incoming call.
 *		laddr - local address in netbuf format.
 *
 * Output -
 *		function returns value of endpoint on which the call will be
 *		handled.
 */

int
accept_call(int listen_fd, struct t_call *call, unsigned char *laddr)
{
	int call_fd, tslen, nslen;
	struct t_bind *bind, *bound;

	/* 
	 * Open new file descriptor to accept call.
	 */

	if ((call_fd = t_open("/dev/otk6", O_RDWR, (struct t_info *) NULL)) == 1) {
		t_error("t_open (call_fd) failed");
		exit(8);
	}

	/* 
	 * Allocate t_bind structures to contain the address being bound to
	 * and the address actually bound.
	 */

	if ((bind = (struct t_bind *) t_alloc(call_fd, T_BIND, T_ALL)) == NULL) {
		t_error("t_alloc bind failed");
		exit(9);
	}

	if ((bound = (struct t_bind *) t_alloc(call_fd, T_BIND, T_ALL)) == NULL) {
		t_error("t_alloc bound failed");
		exit(10);
	}

	/* 
	 * Insert local address into bind structure.  This is the same address
	 * as was used to bind the listening file descriptor.
	 */

	tslen = laddr[0];
	nslen = laddr[tslen + 1];
	bind->addr.buf = (char *) laddr;
	bind->addr.len = tslen + nslen + 2;
	bind->qlen = 0;		/* We will not listen on this descriptor */

	/* 
	 * Do the bind.
	 */

	if (t_bind(call_fd, bind, bound) < 0) {
		t_error("t_bind (call_fd) failed");
		exit(11);
	}

	/* 
	 * Attempt to accept the call on the new file descriptor.
	 */

	if (t_accept(listen_fd, call_fd, call) < 0) {

		/* 
		 * Find out why accept failed.
		 */

		if (t_errno == TLOOK) {

			/* 
			 * This is probably a disconnect, so close the
			 * file descriptor anyway.  Could find out more
			 * information using t_look.
			 */

			t_close(call_fd);
			printf("tli/rfcserver - disconnect received\n");
			return (DISCONNECT);
		}
		t_error("t_accept failed");	/* Non "protocol" problem */
		exit(12);
	}

	/* 
	 * Call has successfully been accepted so return the new file
	 * descriptor.
	 */

	printf("tli/rfcserver - call successfully accepted\n");
	return (call_fd);
}

/*
 * run_server() - fork a process to send some data.
 *
 * This function forks a process which transfers MAXDATA bytes of data to the
 * client.  In a real application this sould do something more useful.
 *
 * Input -
 *		listen_fd - endpoint for listening
 *		call_fd - endpoint for data transfer
 */

void
run_server(int listen_fd, int call_fd)
{
	int nbytes, cc, flags;
	char buf(MAXDATA);
	struct t_discon *discon;

	/* 
	 * Fork process to handle data transfer.
	 */

	switch (fork()) {
	case -1:		/* fork() failed */
		perror("fork failed");
		exit(13);
	default:		/* Parent */

		/* 
		 * Close file descriptor on which call was accepted since
		 * the parent only performs listens.
		 */

		t_close(call_fd);
		return;
	case 0:		/* Child */

		/* 
		 * Close the listening file descriptor since the child does
		 * not do any listens.
		 */

		t_close(listen_fd);

		/* 
		 * Allocate a t_discon structure for use later.
		 */

		if ((discon = (struct t_discon *) t_alloc(call_fd, T_DIS, T_ALL)) == NULL) {
			t_error("t_alloc of t_discon failed");
			exit(14);
		}

		/* 
		 * Now transfer the data.
		 */

		printf("tli/rfcserver - child transferring data\n");
		cc = 0;
		while (cc < MAXDATA) {
			if ((cc += t_snd(call_fd, buf, BUFSIZE, 0)) < 0) {
				t_error("t_snd failed");
				exit(15);
			}
		}

		/* 
		 * Wait for disconnect from the client.
		 */

		if (t_rcv(call_fd, buf, nbytes, &flags) < 0) {
			if (t_rcvdis(call_fd, discon) < 0) {
				t_error("t_rcvdis failed");
				exit(16);
			}
		} else {

			/* 
			 * What we just received was not a disconnect.  This
			 * is invalid in this example so initiate a disconnect
			 * from here.
			 */

			if (t_snddis(call_fd, NULL) < 0) {
				t_error("t_snddis failed");
				exit(17);
			}
		}

		/* 
		 * Finished!
		 */

		t_close(call_fd);
		exit(0);
	}
}

main(int argc, char **argv)
{
	struct t_bind *sndbind, *rcvbind;
	struct t_call *call;
	int fd, call_fd, tslen, nslen;
	char local_tsel[32];
	unsigned char taddr[MAXADDRLEN], baddr[MAXADDRLEN];

	/* 
	 * Get the local tsel.  Note that this is assumed to be a character
	 * string in this example.
	 */

	if (argc > 2) {
		printf("Usage: rfcserver <tsel>\n");
		exit(1);
	} else if (argc == 2) {
		strcpy(local_tsel, argv[1]);
	} else {
		strcpy(local_tsel, TSEL);
	}

	/* 
	 * Open the /dev/otk6 device.
	 */

	if ((fd = t_open("/dev/otk6", O_RDWR, (struct t_info *) NULL)) < 0) {
		t_error("t_open /dev/otk6 failed");
		exit(2);
	}

	/* 
	 * Allocate t_bind structure for the address to be bound to and the
	 * address actually bound.
	 */

	if ((sndbind = (struct t_bind *) t_alloc(fd, T_BIND, T_ALL)) == NULL) {
		t_error("t_alloc sndbind");
		t_close(fd);
		exit(3);
	}
	if ((rcvbind = (struct t_bind *) t_alloc(fd, T_BIND, T_ALL)) == NULL) {
		t_error("t_alloc rcvbind");
		t_close(fd);
		exit(4);
	}

	/* 
	 * Now bind to the address.
	 *
	 * Addresses are in netbuf format.  This is an array as follows:
	 *
	 * [tsaplen][tsap............][nsaplen][nsap....................]
	 *
	 * where tsaplen and nsaplen are one byte each.
	 */

	/* 
	 * In this case we assume the TSEL is a string of characters - it
	 * could be numeric.
	 */

	tslen = strlen(local_tsel);
	taddr[0] = (unsigned char) tslen;
	memcpy(&taddr[1], local_tsel, tslen);

	/* 
	 * We can bind with a NULL NSEL.
	 */

	nslen = 0;
	taddr[tslen + 1] = nslen;

	sndbind->addr.len = tslen + nslen + 2;
	sndbind->addr.buf = (char *) taddr;

	/* 
	 * Set the qlen to 1 to receive one call at a time.  In fact due to
	 * a bug in the TLI spec trying to accept when there is more than one
	 * call on the queue will cause problems.
	 */

	sndbind->qlen = 1;

	/* 
	 * Initialize the t_bind for returning the bound address.
	 */

	rcvbind->addr.maxlen = MAXADDRLEN;
	rcvbind->addr.buf = (char *) baddr;

	/* 
	 * Now do the bind.
	 */

	if (t_bind(fd, sndbind, rcvbind) < 0) {
		t_error("t_bind failed");
		t_close(fd);
		exit(5);
	}
	printf("tli/rfcserver - bind completed successfully\n");

	/* 
	 * Allocate a t_call structure to hold the call information whenever
	 * an incoming call is received.
	 */

	if ((call = (struct t_call *) t_alloc(fd, T_CALL, T_ALL)) == NULL) {
		t_error("t_alloc failed call");
		t_close(fd);
		exit(6);
	}

	/* 
	 * Enter infinite loop waiting for incoming connections.  Incoming
	 * calls are allocated their own file descriptor in accept_call() and
	 * a new process is forked for each accepted call in run_server().
	 */

	while (1) {

		/* 
		 * Listen for incoming calls.
		 */

		printf("tli/rfcserver - waiting for a connection....\n");
		if (t_listen(fd, call) < 0) {
			t_error("t_listen failed");
			exit(7);
		}
		printf("tli/rcvserver - incoming call received\n");

		/* 
		 * Accept call and fork process to do data transfer.
		 */

		if ((call_fd = accept_call(fd, call, taddr)) != DISCONNECT) {
			run_server(fd, call_fd);
		}
	}

	/* 
	 * Not reached.
	 */
}
