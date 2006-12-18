/*****************************************************************************

 @(#) $RCSfile: wrapper.c,v $ $Name:  $($Revision: 1.1.6.5 $) $Date: 2006/12/18 09:51:12 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
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

 Last Modified $Date: 2006/12/18 09:51:12 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: wrapper.c,v $ $Name:  $($Revision: 1.1.6.5 $) $Date: 2006/12/18 09:51:12 $"

static char const ident[] =
    "$RCSfile: wrapper.c,v $ $Name:  $($Revision: 1.1.6.5 $) $Date: 2006/12/18 09:51:12 $";

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#ifdef HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif

#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <strings.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_TEST_INSTANCES 256
pid_t test_pid[MAX_TEST_INSTANCES];
int num_instances = 1;
long max_latency = 0;
double max_load = 0.0;

void
sig_chld(int sig)
{
	/* empty handler required otherwise SIGCHLD is SIG_IGN and select() isn't interrupted */
}

void
kill_test(void)
{
	int i;

	for (i = 0; i < MAX_TEST_INSTANCES; i++)
		if (test_pid[i] != 0)
			kill(test_pid[i], SIGTERM);
	for (i = 0; i < MAX_TEST_INSTANCES; i++)
		if (test_pid[i] != 0)
			waitpid(test_pid[i], NULL, 0);
}

void
sig_term(int signum)
{
	printf("\nwrapper: exiting on signal %d\n", signum);
	exit(1);
}

void
mangle_argv(int i, char **argv)
{
	char *s;

	while (*argv != NULL) {
		if (strchr(*argv, '%') == NULL) {
			argv++;
			continue;
		}
		s = malloc(strlen(*argv) + 20);
		if (!s) {
			perror("malloc");
			exit(1);
		}
		sprintf(s, *argv, i);
/* 		printf("%s => %s\n", *argv, s); */
		*argv = s;
		argv++;
	}
}

int
spawn_test(char *progname, char **argv)
{
	int i;

	fflush(stdout);
	atexit(kill_test);
	for (i = 0; i < num_instances; i++) {
		switch (test_pid[i] = fork()) {
		case -1:
			perror("fork");
			test_pid[i] = 0;
			exit(1);
		case 0:	/* in child */
			mangle_argv(i, argv);
			execvp(progname, argv);
			perror("execvp");
			memset(test_pid, 0, sizeof(test_pid));
			exit(1);
		default:	/* in parent */
			break;
		}
	}
	return 0;
}

void
sample_latency(void)
{
	static struct timeval prev_time, curr_time;
	long latency;

	if (curr_time.tv_sec == 0) {	/* initial call */
		if (gettimeofday(&curr_time, NULL)) {
			perror("gettimeofday");
			exit(1);
		}
		return;
	}

	/* measure responsiveness (roughly) */
	prev_time = curr_time;
	if (gettimeofday(&curr_time, NULL)) {
		perror("gettimeofday");
		exit(1);
	}
	latency = curr_time.tv_sec - prev_time.tv_sec;
	if (latency > max_latency)
		max_latency = latency;
}

void
sample_load(void)
{
	static int fd = -1;
	static char buf[256];
	double load;
	char *endptr;

	if (fd < 0) {
		fd = open("/proc/loadavg", O_RDONLY);
		if (fd < 0) {
			perror("open");
			exit(1);
		}
	}
	/* measure system load (first field in /proc/loadavg) */
	if (lseek(fd, 0, SEEK_SET) < 0) {
		perror("lseek");
		exit(1);
	}
	if (read(fd, buf, sizeof(buf)) < 0) {
		perror("read");
		exit(1);
	}
	*index(buf, ' ') = '\0';
	load = strtod(buf, &endptr);
	if (*endptr != '\0') {
		fprintf(stderr, "strtod: error in conversion of '%s'\n", buf);
		exit(1);
	}
	if (load > max_load)
		max_load = load;
}

void
wait4test(void)
{
	int status;
	pid_t child;
	struct timeval tv;
	int i;

	sample_latency();	/* initial call */
	sample_load();

	for (;;) {
		/* sample system status: system load, LiS mem, responsiveness */
		/* TBD */

		sample_latency();
		sample_load();

		/* check if we have any zombie pending */
		for (;;) {
			if ((child = waitpid(-1, &status, WNOHANG)) < 0) {
				perror("waitpid");
				exit(1);
			}
			if (child == 0)
				break;
			if (!WIFEXITED(status) || (WEXITSTATUS(status) != 0)) {
				printf("\nwrapper: child process %i exited abnormally\n", child);
				exit(1);
			}
			/* child exited with 0 */
			for (i = 0; i < MAX_TEST_INSTANCES; i++) {
				if (test_pid[i] == child) {
					test_pid[i] = 0;
					if (--num_instances == 0)
						return;
					break;
				}
			}
		}

		/* still some children running, wait for any signal (including SIGCHLD) */
		tv.tv_sec = 1;
		tv.tv_usec = 0;
		select(0, NULL, NULL, NULL, &tv);

	}
}

void
report(void)
{
	printf("\nwrapper: test finished successfully; max latency %ld max load %.2f\n",
	       max_latency, max_load);
}

void
usage(char *progname)
{
	fprintf(stderr, "Usage: %s [-n <num instances>] /path/to/testprog [test args...]\n",
		progname);
	exit(1);
}

int
main(int argc, char **argv)
{
	int c;
	char *ptr;

	if (argc < 2)
		usage(argv[0]);
	while ((c = getopt(argc, argv, "+n:")) != -1) {
		switch ((char) c) {
		case 'n':
			/* number of processes */
			num_instances = strtoul(optarg, &ptr, 0);
			if (*ptr != '\0')
				usage(argv[0]);
			if (num_instances > MAX_TEST_INSTANCES) {
				fprintf(stderr, "%s: max number of instances exceeded (%d)\n",
					argv[0], MAX_TEST_INSTANCES);
				exit(1);
			}
			break;
		default:
			usage(argv[0]);
		}
	}

	/* 
	 * signal handler for SIGCHLD required
	 * by default SIGCHLD is SIG_IGN and select() is not interrupted
	 * signal handlers for SIGHUP, SIGTERM and SIGINT
	 */
	if ((signal(SIGCHLD, sig_chld) == SIG_ERR)
	    || (signal(SIGHUP, sig_term) == SIG_ERR)
	    || (signal(SIGTERM, sig_term) == SIG_ERR)
	    || (signal(SIGINT, sig_term) == SIG_ERR)) {
		perror("signal");
		exit(1);
	}

	/* start test program */
	spawn_test(argv[optind], argv + optind);

	/* gather stats until test program exits */
	wait4test();

	/* report stats */
	report();

	return 0;
}
