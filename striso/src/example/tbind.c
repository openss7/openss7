/*
#ident "@(#) tbind.c 1.2 24 Jan 1995 SMI"
 *
 * This program shows how to use the OSI library with TLI.
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <tiuser.h>

#include <osi.h>

int
main(int argc, char *argv[])
{
	char nsap[BUFSIZ], clns_nsap[BUFSIZ], tsel[BUFSIZ], clns_buf[BUFSIZ], otpi_buf[BUFSIZ];
	int nsap_len, clns_nsap_len, tsel_len;
	int fd_otpi, fd_clns;
	struct t_bind b_otpi, b_clns;

	/* Get the NSAP to bind on for otpi */
	if ((nsap_len = getmyclnsnsap(nsap, BUFSIZ)) < 0) {
		perror("getmyclnsnsap");
		exit(1);
	}
	/* Since we cannot use the NSAP of the stack because it identify the OSI transport, we
	   should take a new one.  Be sure that the entry for this NSAP exist on
	   /etc/net/oclt/hosts */
	if ((clns_nsap_len = getnsapbyname("clns", clns_nsap, BUFSIZ)) < 0) {
		perror("getnsapbyname");
		exit(1);
	}
	/* Get a TSEL for the bind on transport. A corresponding entry should exist on
	   /etc/net/oclt/services */
	if ((tsel_len = gettselbyname("test", tsel, BUFSIZ)) < 0) {
		perror("gettselbyname");
		exit(1);
	}
	/* Open the both file descriptors */
	if ((fd_clns = t_open("/dev/clns", O_RDWR, (struct t_info *) NULL)) == -1) {
		t_error("t_open /dev/clns");
		exit(1);
	}
	if ((fd_otpi = t_open("/dev/otpi", O_RDWR, (struct t_info *) NULL)) < 0) {
		t_error("t_open /dev/otpi");
		exit(1);
	}
	/* Format the netbuf structure */
	printf("Set the bind address for clns with nsap = 0x%s\n", clns_nsap);

	b_clns.addr.len = nsap2net(clns_buf, clns_nsap, BUFSIZ);

	if (b_clns.addr.len < 0) {
		perror("nsap2net");
		exit(1);
	}

	b_clns.addr.buf = clns_buf;
	b_clns.qlen = 0;

	printf("Set the bind address for otpi with nsap = 0x%s and TSEL = 0x%s\n", nsap, tsel);

	b_otpi.addr.len = tsap2net(otpi_buf, nsap, tsel, BUFSIZ);

	if (b_otpi.addr.len < 0) {
		perror("tsap2net");
		exit(1);
	}

	b_otpi.addr.buf = otpi_buf;
	b_otpi.qlen = 0;

	/* Send the t_bind request */

	if (t_bind(fd_clns, &b_clns, (struct t_bind *) NULL) < 0) {
		t_error("t_bind clns");
	}
	if (t_bind(fd_otpi, &b_otpi, (struct t_bind *) NULL) < 0) {
		t_error("t_bind otpi");
	}
	t_close(fd_clns);
	t_close(fd_opti);

	exit(0);
}
