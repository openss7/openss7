/*****************************************************************************

 @(#) $RCSfile: perftest.c,v $ $Name:  $($Revision: 0.9.2.16 $) $Date: 2007/05/07 18:51:43 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 Unauthorized distribution or duplication is prohibited.

 This software and related documentation is protected by copyright and
 distributed under licenses restricting its use, copying, distribution and
 decompilation.  No part of this software or related documentation may be
 reproduced in any form by any means without the prior written authorization
 of the copyright holder, and licensors, if any.

 The recipient of this document, by its retention and use, warrants that the
 recipient will protect this information and keep it confidential, and will
 not disclose the information contained in this document without the written
 permission of its owner.

 The author reserves the right to revise this software and documentation for
 any reason, including but not limited to, conformity with standards
 promulgated by various agencies, utilization of advances in the state of the
 technical arts, or the reflection of changes in the design of any techniques,
 or procedures embodied, described, or referred to herein.  The author is
 under no obligation to provide any feature listed herein.

 -----------------------------------------------------------------------------

 As an exception to the above, this software may be distributed under the GNU
 General Public License (GPL) Version 2, so long as the software is distributed
 with, and only used for the testing of, OpenSS7 modules, drivers, and libraries.

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

 Last Modified $Date: 2007/05/07 18:51:43 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: perftest.c,v $
 Revision 0.9.2.16  2007/05/07 18:51:43  brian
 - changes from release testing

 Revision 0.9.2.15  2007/05/03 22:18:35  brian
 - updated perftest program and docuemntation

 Revision 0.9.2.14  2007/04/12 20:06:17  brian
 - changes from performance testing and misc bug fixes

 Revision 0.9.2.13  2007/04/04 01:15:25  brian
 - performance improvements (speeds up put and srv procedures)

 Revision 0.9.2.12  2007/04/01 12:29:30  brian
 - performance tuning (QWANTR/QWANTW corrections)

 Revision 0.9.2.11  2006/12/18 07:32:42  brian
 - lfs device names, autoload clone minors, device numbering, missing manpages

 Revision 0.9.2.10  2006/07/24 09:01:23  brian
 - results of udp2 optimizations

 Revision 0.9.2.9  2006/02/25 01:30:46  brian
 - more roughing in of 32bit compatibilty support
 - updated perftest program to be able to use FIFOs as well as pipes
 - added README-LiS file to capture LiS binary compatibility issues

 Revision 0.9.2.8  2005/12/07 11:14:12  brian
 - fixed bug keeping getmsg/putmsg from working.

 Revision 0.9.2.7  2005/11/01 11:21:11  brian
 - updates for testing and documentation

 Revision 0.9.2.6  2005/10/21 03:54:27  brian
 - modifications for queueing testing

 Revision 0.9.2.5  2005/10/20 08:18:58  brian
 - modifications for queuing and scheduling testing

 Revision 0.9.2.4  2005/10/19 11:08:32  brian
 - performance tweaks

 Revision 0.9.2.3  2005/10/19 01:53:28  brian
 - some alterations to perf tests

 Revision 0.9.2.2  2005/10/18 08:55:27  brian
 - added Linux native pipe test

 Revision 0.9.2.1  2005/10/18 03:10:12  brian
 - pipe performance tests

 Revision 0.9.2.3  2005/05/14 08:31:24  brian
 - copyright header correction

 *****************************************************************************/

#ident "@(#) $RCSfile: perftest.c,v $ $Name:  $($Revision: 0.9.2.16 $) $Date: 2007/05/07 18:51:43 $"

static char const ident[] =
    "$RCSfile: perftest.c,v $ $Name:  $($Revision: 0.9.2.16 $) $Date: 2007/05/07 18:51:43 $";

/*
 *  These are benchmark performance tests on a pipe for testing LiS
 *  performance patches.
 */

#include <stropts.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/resource.h>

#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif

#ifdef _GNU_SOURCE
#include <getopt.h>
#endif

#define MAXMSGSIZE 32768
int verbose = 1;
int msgsize = 64;
int report = 1;
int iterations = -1;

#ifdef NATIVE_PIPES
int native = 1;
int readwrite = 1;
#else
int native = 0;
int readwrite = 0;
#endif
int sethiwat = 0;
int setlowat = 0;
size_t hiwat = 0;
size_t lowat = 0;
int sndmread = 0;
int readfill = 0;
int fullreads = 0;
int niceread = 0;
int nicerval = 19;
int nicesend = 0;
int nicesval = 19;
int fifo = 0;
int push = 0;
int blocking = 0;
int strhold = 0;
int asynchronous = 0;
char my_msg[MAXMSGSIZE] = { 0, };
char modname[256] = "pipemod";

int dummy = 0;

volatile int timer_timeout = 0;
volatile int alarm_signal = 0;

void
timer_handler(int signum)
{
	if (signum == SIGALRM)
		timer_timeout = 1;
	if (signum == SIGPIPE || signum == SIGHUP) {
		alarm_signal = 1;
		timer_timeout = 1;
	}
	return;
}

int
timer_sethandler(void)
{
	sigset_t mask;
	struct sigaction act;

	act.sa_handler = timer_handler;
	act.sa_flags = SA_RESTART | SA_ONESHOT;
	act.sa_restorer = NULL;
	sigemptyset(&act.sa_mask);
	if (sigaction(SIGALRM, &act, NULL))
		return -1;
	if (sigaction(SIGPOLL, &act, NULL))
		return -1;
	if (sigaction(SIGURG, &act, NULL))
		return -1;
	if (sigaction(SIGPIPE, &act, NULL))
		return -1;
	if (sigaction(SIGHUP, &act, NULL))
		return -1;
	sigemptyset(&mask);
	sigaddset(&mask, SIGALRM);
	sigaddset(&mask, SIGPOLL);
	sigaddset(&mask, SIGURG);
	sigaddset(&mask, SIGPIPE);
	sigaddset(&mask, SIGHUP);
	sigprocmask(SIG_UNBLOCK, &mask, NULL);
	siginterrupt(SIGALRM, 1);
	siginterrupt(SIGPOLL, 1);
	siginterrupt(SIGURG, 1);
	siginterrupt(SIGPIPE, 1);
	siginterrupt(SIGHUP, 1);
	return (0);
}

int
start_timer(void)
{
	struct itimerval setting = { {0, 0}, {report, 0} };

	if (timer_sethandler())
		return (-1);
	timer_timeout = 0;
	alarm_signal = 0;
	if (setitimer(ITIMER_REAL, &setting, NULL))
		return (-1);
	return 0;
}

#ifndef PIPE_BUF
#define PIPE_BUF 8192
#endif

#define RECENT_WEIGHT 2

int
test_sync(int fds[])
{
	long long tbytcnt = 0, tmsgcnt = 0, tavg_msgs = 0, tavg_tput = 0, tbytmin = PIPE_BUF, tbytmax = 0, tbyttot = 0;
	long long rbytcnt = 0, rmsgcnt = 0, ravg_msgs = 0, ravg_tput = 0, rbytmin = PIPE_BUF, rbytmax = 0, rbyttot = 0;
	long long tmsize = msgsize;
	long long rmsize = msgsize;
	long long report_count = 0;

	if (fullreads)
		rmsize = PIPE_BUF;
	if (verbose > 1)
		fprintf(stderr, "Starting timer\n");
	if (start_timer()) {
		if (verbose)
			perror("start_timer()");
		goto dead;
	}
	if (verbose > 1)
		fprintf(stderr, "--> Timer started\n");
	for (;;) {
		if (alarm_signal)
			goto dead;
		if (timer_timeout) {
			{
				long long thrput = tbytcnt / report;
				long long msgcnt = tmsgcnt / report;
				long long avgsiz = tbytcnt / tmsgcnt;

#if 0
				tavg_msgs = (3 * tavg_msgs + msgcnt) / 4;
				tavg_tput = (3 * tavg_tput + thrput) / 4;
#else
				tavg_msgs = (tavg_msgs * report_count + msgcnt * RECENT_WEIGHT) / (report_count + RECENT_WEIGHT);
				tavg_tput = (tavg_tput * report_count + thrput * RECENT_WEIGHT) / (report_count + RECENT_WEIGHT);
#endif
				fprintf(stdout,
					"%d Msgs sent: %10lld (%10lld), throughput: %10lld (%10lld), size (%4lld) %4lld-%4lld\n",
					fds[1], msgcnt, tavg_msgs, thrput, tavg_tput, avgsiz, tbytmin, tbytmax);
				fflush(stdout);
			}
			{
				long long thrput = rbytcnt / report;
				long long msgcnt = rmsgcnt / report;
				long long avgsiz = rbytcnt / rmsgcnt;

#if 0
				ravg_msgs = (3 * ravg_msgs + msgcnt) / 4;
				ravg_tput = (3 * ravg_tput + thrput) / 4;
#else
				ravg_msgs = (ravg_msgs * report_count + msgcnt * RECENT_WEIGHT) / (report_count + RECENT_WEIGHT);
				ravg_tput = (ravg_tput * report_count + thrput * RECENT_WEIGHT) / (report_count + RECENT_WEIGHT);
#endif
				fprintf(stdout,
					"%d Msgs read: %10lld (%10lld), throughput: %10lld (%10lld), size (%4lld) %4lld-%4lld\n",
					fds[0], msgcnt, ravg_msgs, thrput, ravg_tput, avgsiz, rbytmin, rbytmax);
				fflush(stdout);
			}
			tbyttot -= rbyttot;
			rbyttot = 0;
			tbytcnt = 0;
			tmsgcnt = 0;
			tbytmin = PIPE_BUF;
			tbytmax = 0;
			rbytcnt = 0;
			rmsgcnt = 0;
			rbytmin = PIPE_BUF;
			rbytmax = 0;
			report_count++;
			if (iterations > 0 && report_count >= iterations) {
				close(fds[0]);
				close(fds[1]);
				return (0);
			}
			if (start_timer()) {
				if (verbose)
					perror("start_timer()");
				goto dead;
			}
		}
		if (tbyttot >= rbyttot + rmsize) {
			int ret = 0;

			if (readwrite) {
				while (!timer_timeout && (ret = read(fds[0], my_msg, rmsize)) > 0) {
					rbytcnt += ret;
					rbyttot += ret;
					if (rbytcnt < 0)
						goto dead;
					rmsgcnt += 1;
					if (ret < rbytmin)
						rbytmin = ret;
					if (ret > rbytmax)
						rbytmax = ret;
					if (blocking)
						break;
				}
			} else {
				int flags = 0;
				struct strbuf cbuf = { -1, 0, my_msg };
				struct strbuf dbuf = { rmsize, 0, my_msg };

				while (!timer_timeout
				       && (ret = getmsg(fds[0], &cbuf, &dbuf, &flags)) != -1) {
					rbytcnt += dbuf.len;
					rbyttot += dbuf.len;
					if (rbytcnt < 0)
						goto dead;
					rmsgcnt += 1;
					if (ret < rbytmin)
						rbytmin = ret;
					if (ret > rbytmax)
						rbytmax = ret;
					if (blocking)
						break;
				}
			}
			if (ret < 0) {
				switch (errno) {
				case EAGAIN:
				case EINTR:
					break;
				default:
					if (verbose)
						perror("read()");
					goto dead;
				}
			}
		}
		if (tbyttot < rbyttot + rmsize) {
			int ret = 0;

			if (readwrite) {
				while (!timer_timeout && (ret = write(fds[1], my_msg, tmsize)) > 0) {
					tbytcnt += ret;
					tbyttot += ret;
					if (tbytcnt < 0)
						goto dead;
					tmsgcnt += 1;
					if (ret < tbytmin)
						tbytmin = ret;
					if (ret > tbytmax)
						tbytmax = ret;
					if (blocking)
						break;
				}
			} else {
				struct strbuf dbuf = { 0, tmsize, my_msg };

				while (!timer_timeout
				       && (ret = putmsg(fds[1], NULL, &dbuf, 0)) != -1) {
					tbytcnt += tmsize;
					tbyttot += tmsize;
					if (tbytcnt < 0)
						goto dead;
					tmsgcnt += 1;
					if (ret < tbytmin)
						tbytmin = ret;
					if (ret > tbytmax)
						tbytmax = ret;
					if (blocking)
						break;
				}
			}
			if (ret < 0) {
				switch (errno) {
				case EAGAIN:
				case EINTR:
					break;
				default:
					if (verbose)
						perror("write()");
					goto dead;
				}
			}
		}
	}
      dead:
	close(fds[0]);
	close(fds[1]);
	return (1);
}

/*
   first child process sits and reads 
 */
int
read_child(int fd)
{
	long long rbytcnt = 0, rmsgcnt = 0, ravg_msgs = 0, ravg_tput = 0, rbytmin = PIPE_BUF, rbytmax = 0;
	long long reintr = 0, reagain = 0, rerestart = 0;
	long long rmsize = msgsize;
	struct pollfd pfd = { fd, (POLLIN | POLLRDNORM), 0 };
	int rtn, report_count = 0;

	if (niceread)
		if (setpriority(PRIO_PROCESS, 0, nicerval) != 0) {
			perror("setpriority()");
			goto dead;
		}
	if (fullreads)
		rmsize = PIPE_BUF;
	if (verbose > 1)
		fprintf(stderr, "Starting timer\n");
	if (start_timer()) {
		if (verbose)
			perror("start_timer()");
		goto dead;
	}
	if (verbose > 1)
		fprintf(stderr, "--> Timer started\n");
	for (;;) {
		if (timer_timeout) {
			long long thrput = rbytcnt / report;
			long long msgcnt = rmsgcnt / report;
			long long errcnt = reagain / report;
			long long avgsiz = rbytcnt / rmsgcnt;

#if 0
			ravg_msgs = (3 * ravg_msgs + msgcnt) / 4;
			ravg_tput = (3 * ravg_tput + thrput) / 4;
#else
			ravg_msgs = (ravg_msgs * report_count + msgcnt * RECENT_WEIGHT) / (report_count + RECENT_WEIGHT);
			ravg_tput = (ravg_tput * report_count + thrput * RECENT_WEIGHT) / (report_count + RECENT_WEIGHT);
#endif
			fprintf(stdout,
				"%d Msgs read: %10lld (%10lld), throughput: %10lld (%10lld), size (%4lld) %4lld-%4lld %6lld %6lld %6lld\n",
				fd, msgcnt, ravg_msgs, thrput, ravg_tput, avgsiz, rbytmin, rbytmax, errcnt, reintr, rerestart);
			fflush(stdout);
			rbytcnt = 0;
			rmsgcnt = 0;
			rbytmin = PIPE_BUF;
			rbytmax = 0;
			reintr = 0;
			reagain = 0;
			rerestart = 0;
			report_count++;
			if (iterations > 0 && report_count >= iterations) {
				close(fd);
				return (0);
			}
			if (start_timer()) {
				if (verbose)
					perror("start_timer()");
				goto dead;
			}
		}
		if ((rtn = poll(&pfd, 1, -1)) < 0) {
			switch (errno) {
			case EINTR:
				reintr++;
				continue;
			case ERESTART:
				rerestart++;
				continue;
			}
			if (verbose)
				perror("poll()");
			goto dead;
		}
		if (rtn == 0) {
			rerestart++;
			continue;
		}
		if (pfd.revents & (POLLIN|POLLRDNORM)) {
			int ret = 0;

			if (readwrite) {
				while (!timer_timeout && (ret = read(fd, my_msg, rmsize)) > 0) {
					rbytcnt += ret;
					if (rbytcnt < 0)
						goto dead;
					rmsgcnt += 1;
					if (ret < rbytmin)
						rbytmin = ret;
					if (ret > rbytmax)
						rbytmax = ret;
				}
			} else {
				int flags = 0;
				struct strbuf cbuf = { -1, 0, my_msg };
				struct strbuf dbuf = { rmsize, 0, my_msg };

				while (!timer_timeout
				       && (ret = getmsg(fd, &cbuf, &dbuf, &flags)) != -1) {
					rbytcnt += dbuf.len;
					if (rbytcnt < 0)
						goto dead;
					rmsgcnt += 1;
					if (ret < rbytmin)
						rbytmin = ret;
					if (ret > rbytmax)
						rbytmax = ret;
				}
			}
			if (ret < 0) {
				switch (errno) {
				case EAGAIN:
					reagain++;
					break;
				case EINTR:
					reintr++;
					break;
				default:
					if (verbose)
						perror("read()");
					goto dead;
				}
			}
			pfd.revents &= ~(POLLIN|POLLRDNORM);
		}
		if (pfd.revents & POLLHUP)
			goto done;
		if (pfd.revents & (POLLERR|POLLNVAL|POLLMSG))
			goto dead;
	}
      dead:
	close(fd);
	return (1);
      done:
	close(fd);
	return (0);
}

/*
   second child process sits and writes 
 */
int
write_child(int fd)
{
	long long tbytcnt = 0, tmsgcnt = 0, tavg_msgs = 0, tavg_tput = 0, tbytmin = PIPE_BUF, tbytmax = 0;
	long long teintr = 0, teagain = 0, terestart = 0;
	long long tmsize = msgsize;
	struct pollfd pfd = { fd, (POLLOUT | POLLWRNORM), 0 };
	int rtn, report_count = 0;

	if (nicesend)
		if (setpriority(PRIO_PROCESS, 0, nicesval) != 0) {
			perror("setpriority()");
			goto dead;
		}
	if (verbose > 1)
		fprintf(stderr, "Starting timer\n");
	if (start_timer()) {
		if (verbose)
			perror("start_timer()");
		goto dead;
	}
	if (verbose > 1)
		fprintf(stderr, "--> Timer started\n");
	for (;;) {
		if (timer_timeout) {
			long long thrput = tbytcnt / report;
			long long msgcnt = tmsgcnt / report;
			long long errcnt = teagain / report;
			long long avgsiz = tbytcnt / tmsgcnt;

#if 0
			tavg_msgs = (3 * tavg_msgs + msgcnt) / 4;
			tavg_tput = (3 * tavg_tput + thrput) / 4;
#else
			tavg_msgs = (tavg_msgs * report_count + msgcnt * RECENT_WEIGHT) / (report_count + RECENT_WEIGHT);
			tavg_tput = (tavg_tput * report_count + thrput * RECENT_WEIGHT) / (report_count + RECENT_WEIGHT);
#endif
			fprintf(stdout,
				"%d Msgs sent: %10lld (%10lld), throughput: %10lld (%10lld), size (%4lld) %4lld-%4lld %6lld %6lld %6lld\n",
				fd, msgcnt, tavg_msgs, thrput, tavg_tput, avgsiz, tbytmin, tbytmax, errcnt, teintr, terestart);
			fflush(stdout);
			tbytcnt = 0;
			tmsgcnt = 0;
			tbytmin = PIPE_BUF;
			tbytmax = 0;
			teintr = 0;
			teagain = 0;
			terestart = 0;
			report_count++;
			if (iterations > 0 && report_count >= iterations) {
				close(fd);
				return (0);
			}
			if (start_timer()) {
				if (verbose)
					perror("start_timer()");
				goto dead;
			}
		}
		if ((rtn = poll(&pfd, 1, -1)) < 0) {
			switch (errno) {
			case EINTR:
				teintr++;
				continue;
			case ERESTART:
				terestart++;
				continue;
			}
			if (verbose)
				perror("poll()");
			goto dead;
		}
		if (rtn == 0) {
			terestart++;
			continue;
		}
		if (pfd.revents & (POLLOUT|POLLWRNORM)) {
			int ret = 0;

			if (readwrite) {
				while (!timer_timeout && (ret = write(fd, my_msg, tmsize)) > 0) {
					tbytcnt += ret;
					if (tbytcnt < 0)
						goto dead;
					tmsgcnt += 1;
					if (ret < tbytmin)
						tbytmin = ret;
					if (ret > tbytmax)
						tbytmax = ret;
				}
			} else {
				struct strbuf dbuf = { 0, tmsize, my_msg };

				while (!timer_timeout && (ret = putmsg(fd, NULL, &dbuf, 0)) != -1) {
					tbytcnt += tmsize;
					if (tbytcnt < 0)
						goto dead;
					tmsgcnt += 1;
					if (ret < tbytmin)
						tbytmin = ret;
					if (ret > tbytmax)
						tbytmax = ret;
				}
			}
			if (ret < 0) {
				switch (errno) {
				case EAGAIN:
					teagain++;
					break;
				case EINTR:
					teintr++;
					break;
				default:
					if (verbose)
						perror("write()");
					goto dead;
				}
			}
			pfd.revents &= ~(POLLOUT|POLLWRNORM);
		}
		if (pfd.revents & POLLHUP)
			goto done;
		if (pfd.revents & (POLLERR|POLLNVAL|POLLMSG))
			goto dead;
	}
      dead:
	close(fd);
	return (1);
      done:
	close(fd);
	return (0);
}

int
test_async(int fds[])
{
	int child[2] = { 0, 0 };
	int children = 2;

	if (verbose > 1) {
		fprintf(stderr, "Starting read child on fd %d\n", fds[0]);
	}
	switch ((child[0] = fork())) {
	case 0:		/* child */
		exit(read_child(fds[0]));
	case -1:
		if (verbose)
			perror("fork()");
		goto dead;
	}
	if (verbose > 1) {
		fprintf(stderr, "--> Read child started.\n");
		fprintf(stderr, "Starting write child on fd %d\n", fds[1]);
	}
	switch ((child[1] = fork())) {
	case 0:		/* child */
		exit(write_child(fds[1]));
	case -1:
		if (verbose)
			perror("fork()");
		goto dead;
	}
	if (verbose > 1)
		fprintf(stderr, "--> Write child started.\n");
	close(fds[0]);
	close(fds[1]);
	for (; children; children--) {
		int this_child;
		int this_status;

		if ((this_child = wait(&this_status)) > 0) {
			if (WIFEXITED(this_status)) {
				if (child[0] == this_child)
					child[0] = 0;
				if (child[1] == this_child)
					child[1] = 0;
				if (WEXITSTATUS(this_status) != 0)
					goto dead;
			} else if (WIFSIGNALED(this_status)) {
				if (child[0] == this_child)
					child[0] = 0;
				if (child[1] == this_child)
					child[1] = 0;
				switch (WTERMSIG(this_status)) {
				default:
					goto dead;
				}
			} else if (WIFSTOPPED(this_status)) {
				if (child[0] == this_child)
					child[0] = 0;
				if (child[1] == this_child)
					child[1] = 0;
				switch (WSTOPSIG(this_status)) {
				default:
					goto dead;
				}
			} else
				goto dead;
		}
	}
      dead:
	if (child[0] > 0)
		kill(child[0], SIGKILL);
	if (child[1] > 0)
		kill(child[1], SIGKILL);
	close(fds[0]);
	close(fds[1]);
	return (0);
}

#ifdef LFS
static const char fifoname[] = "/dev/streams/fifo/0";
#else
static const char fifoname[] = "/dev/fifo";
#endif

int
do_tests(void)
{
	int fds[2] = { 0, 0 };

	if (!fifo) {
		if (verbose > 1) {
			fprintf(stderr, "Opening pipe\n");
			fflush(stderr);
		}
		if (pipe(fds) != 0) {
			if (verbose)
				perror("pipe()");
			goto dead;
		}
		if (verbose > 1) {
			fprintf(stderr, "--> Pipe opened.\n");
		}
	} else {
		if (verbose > 1) {
			fprintf(stderr, "Opening fifo\n");
			fflush(stderr);
		}
		if ((fds[0] = open(fifoname, O_RDONLY | (blocking ? 0 : O_NONBLOCK))) < 0) {
			if (verbose)
				perror("open()");
			goto dead;
		}
		if ((fds[1] = open(fifoname, O_WRONLY | (blocking ? 0 : O_NONBLOCK))) < 0) {
			if (verbose)
				perror("open()");
			goto dead;
		}
		if (verbose > 1) {
			fprintf(stderr, "--> FIFO opened.\n");
		}
	}
#if 0
	if (!native) {
		if (verbose > 1) {
			fprintf(stderr, "Setting options on fd %d\n", fds[0]);
		}
		if (ioctl(fds[0], I_SRDOPT, RMSGD) < 0) {
			if (verbose)
				perror("ioctl(I_SRDOPT)");
			goto dead;
		}
		if (ioctl(fds[0], I_SWROPT, (SNDPIPE | SNDHOLD)) < 0) {
			if (verbose)
				perror("ioctl(I_SWROPT)");
			goto dead;
		}
	}
#endif
	if (!native) {
#ifdef I_SET_HIWAT
		if (sethiwat) {
			if (verbose > 1) {
				fprintf(stderr, "Setting hiwat on fd %d\n", fds[0]);
			}
			if (ioctl(fds[0], I_SET_HIWAT, hiwat) < 0) {
				if (verbose)
					perror("ioctl(I_SET_HIWAT)");
			}
		}
#endif
#ifdef I_SET_LOWAT
		if (setlowat) {
			if (verbose > 1) {
				fprintf(stderr, "Setting lowat on fd %d\n", fds[0]);
			}
			if (ioctl(fds[0], I_SET_LOWAT, lowat) < 0) {
				if (verbose)
					perror("ioctl(I_SET_LOWAT)");
				goto dead;
			}
		}
#endif
#ifdef I_SET_HIWAT
		if (sethiwat) {
			if (verbose > 1) {
				fprintf(stderr, "Setting hiwat on fd %d\n", fds[0]);
			}
			if (ioctl(fds[0], I_SET_HIWAT, hiwat) < 0) {
				if (verbose)
					perror("ioctl(I_SET_HIWAT)");
				goto dead;
			}
		}
#endif
#ifdef RFILL
		if (readfill) {
			if (ioctl(fds[0], I_SRDOPT, (RFILL | RPROTNORM)) < 0) {
				if (verbose)
					perror("ioctl(I_SRDOPT)");
				goto dead;
			}
		}
#endif
	}
	if (fcntl(fds[0], F_SETFL, blocking ? 0 : O_NONBLOCK) < 0) {
		if (verbose)
			perror("fcntl");
		goto dead;
	}
	if (verbose > 1) {
		fprintf(stderr, "--> Options set.\n");
	}
#if 0
	if (!native) {
		if (verbose > 1) {
			fprintf(stderr, "Setting options on fd %d\n", fds[1]);
		}
		if (ioctl(fds[1], I_SRDOPT, RMSGD) < 0) {
			if (verbose)
				perror("ioctl(I_SRDOPT)");
			goto dead;
		}
	}
#endif
	if (!native) {
#ifdef I_SET_HIWAT
		if (sethiwat) {
			if (verbose > 1) {
				fprintf(stderr, "Setting hiwat on fd %d\n", fds[0]);
			}
			if (ioctl(fds[1], I_SET_HIWAT, hiwat) < 0) {
				if (verbose)
					perror("ioctl(I_SET_HIWAT)");
			}
		}
#endif
#ifdef I_SET_LOWAT
		if (setlowat) {
			if (verbose > 1) {
				fprintf(stderr, "Setting lowat on fd %d\n", fds[0]);
			}
			if (ioctl(fds[1], I_SET_LOWAT, lowat) < 0) {
				if (verbose)
					perror("ioctl(I_SET_LOWAT)");
				goto dead;
			}
		}
#endif
#ifdef I_SET_HIWAT
		if (sethiwat) {
			if (verbose > 1) {
				fprintf(stderr, "Setting hiwat on fd %d\n", fds[0]);
			}
			if (ioctl(fds[1], I_SET_HIWAT, hiwat) < 0) {
				if (verbose)
					perror("ioctl(I_SET_HIWAT)");
				goto dead;
			}
		}
#endif
#ifdef SNDHOLD
		if (strhold) {
			int wropts = SNDPIPE | SNDHOLD;

#ifdef SNDMREAD
			if (sndmread)
				wropts |= SNDMREAD;
#endif
			if (ioctl(fds[1], I_SWROPT, wropts) < 0) {
				if (verbose)
					perror("ioctl(I_SWROPT)");
				goto dead;
			}
		}
#endif
#ifdef SNDMREAD
		if (sndmread) {
			int wropts = SNDPIPE | SNDMREAD;

#ifdef SNDHOLD
			if (strhold)
				wropts |= SNDHOLD;
#endif
			if (ioctl(fds[1], I_SWROPT, wropts) < 0) {
				if (verbose)
					perror("ioctl(I_SWROPT)");
				goto dead;
			}
		}
#endif
	}
	if (fcntl(fds[1], F_SETFL, blocking ? 0 : O_NONBLOCK) < 0) {
		if (verbose)
			perror("fcntl");
		goto dead;
	}
	if (verbose > 1) {
		fprintf(stderr, "--> Options set.\n");
	}
	if (!native) {
		int i;

		if (verbose > 1) {
			fprintf(stderr, "Pushing %d instances of %s on %d\n", push, modname,
				fds[0]);
		}
		for (i = 0; i < push; i++) {
			if (ioctl(fds[1], I_PUSH, modname) < 0) {
				if (verbose)
					perror("ioctl(I_PUSH)");
				goto dead;
			}
		}
		if (verbose > 1) {
			fprintf(stderr, "--> %s pushed.\n", modname);
		}
	}
	if (asynchronous)
		test_async(fds);
	else
		test_sync(fds);
	return (0);
      dead:
	close(fds[0]);
	close(fds[1]);
	return (1);
}

void
print_header(void)
{
	if (!verbose)
		return;
	dummy = lockf(fileno(stdout), F_LOCK, 0);
	fprintf(stdout, "\nSTREAMS Benchmark Performance Tests on a Pipe\n");
	fflush(stdout);
	dummy = lockf(fileno(stdout), F_ULOCK, 0);
}

void
copying(int argc, char *argv[])
{
	if (!verbose)
		return;
	print_header();
	fprintf(stdout, "\
\n\
Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>\n\
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>\n\
\n\
All Rights Reserved.\n\
\n\
Unauthorized distribution or duplication is prohibited.\n\
\n\
This software and related documentation is protected by copyright and distribut-\n\
ed under licenses restricting its use,  copying, distribution and decompilation.\n\
No part of this software or related documentation may  be reproduced in any form\n\
by any means without the prior  written  authorization of the  copyright holder,\n\
and licensors, if any.\n\
\n\
The recipient of this document,  by its retention and use, warrants that the re-\n\
cipient  will protect this  information and  keep it confidential,  and will not\n\
disclose the information contained  in this document without the written permis-\n\
sion of its owner.\n\
\n\
The author reserves the right to revise  this software and documentation for any\n\
reason,  including but not limited to, conformity with standards  promulgated by\n\
various agencies, utilization of advances in the state of the technical arts, or\n\
the reflection of changes  in the design of any techniques, or procedures embod-\n\
ied, described, or  referred to herein.   The author  is under no  obligation to\n\
provide any feature listed herein.\n\
\n\
As an exception to the above,  this software may be  distributed  under the  GNU\n\
General Public License (GPL) Version 2,  so long as the  software is distributed\n\
with, and only used for the testing of, OpenSS7 modules, drivers, and libraries.\n\
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
");
	fflush(stdout);
}

void
version(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
\n\
%1$s:\n\
    %2$s\n\
    Copyright (c) 1997-2007  OpenSS7 Corporation.  All Rights Reserved.\n\
\n\
    Distributed by OpenSS7 Corporation under GPL Version 2,\n\
    incorporated here by reference.\n\
\n\
    See `%1$s --copying' for copying permission.\n\
\n\
", argv[0], ident);
}

void
usage(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stderr, "\
Usage:\n\
    %1$s [options]\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
", argv[0]);
}

void
help(int argc, char *argv[])
{
	if (!verbose)
		return;
	fprintf(stdout, "\
Usage:\n\
    %1$s [options]\n\
    %1$s {-h, --help}\n\
    %1$s {-V, --version}\n\
    %1$s {-C, --copying}\n\
Arguments:\n\
    (none)\n\
Options:\n\
    --hiwat=HIWAT\n\
        Set high water mark on stream head.\n\
    --lowat=LOWAT\n\
        Set low water mark on stream head.\n\
    -M, --mread\n\
        Issue M_READ messages.\n\
    -w, --readfill\n\
        Read fill mode.\n\
    -R, --niceread [NICE]\n\
        Run read child nice 19.\n\
    -S, --nicesend [NICE]\n\
        Run write child nice 19.\n\
    -F, --full\n\
        Perform full size reads.\n\
    -H, --hold\n\
        Use SNDHOLD message coallescing\n\
    -a, --async\n\
        Perform asynchronous testing\n\
    -f, --fifo\n\
        Use a STREAMS FIFO instead of a STREAMS-based pipe\n\
    -m, --module=MODNAME\n\
        Module name to push [default: %6$s]\n\
    -p, --push=[COUNT]\n\
        Push COUNT instances of module [default: %5$d]\n\
    -b, --blocking\n\
        Use blocking operation on read and write\n\
    -s, --size=[MSGSIZE]\n\
        Packet size to be used for testing [default: %2$d]\n\
    -r, --readwrite\n\
        Use read/write instead of putmsg/getmsg\n\
    -t, --time=[REPORT]\n\
        Report time in seconds [default: %3$d]\n\
    -i, --iterations=ITERATIONS\n\
        Stop after INTERATIONS number of reports [default: none]\n\
    -q, --quiet\n\
        Suppress normal output (equivalent to --verbose=0)\n\
    -v, --verbose=[LEVEL]\n\
        Increase verbosity or set to LEVEL [default: %4$d]\n\
        This option may be repeated.\n\
    -h, --help, -?, --?\n\
        Print this usage message and exit\n\
    -V, --version\n\
        Print version and exit\n\
    -C, --copying\n\
        Print copying permission and exit\n\
", argv[0], msgsize, report, verbose, push, modname);
}

int
main(int argc, char *argv[])
{
	for (;;) {
		int c, val;

#if defined _GNU_SOURCE
		int option_index = 0;
		/* *INDENT-OFF* */
		static struct option long_options[] = {
			{"hiwat",	required_argument,	NULL, '\1'},
			{"lowat",	required_argument,	NULL, '\2'},
			{"mread",	no_argument,		NULL, 'M'},
			{"readfill",	no_argument,		NULL, 'w'},
			{"niceread",	optional_argument,	NULL, 'R'},
			{"nicesend",	optional_argument,	NULL, 'S'},
			{"full",	no_argument,		NULL, 'F'},
			{"module",	required_argument,	NULL, 'm'},
			{"hold",	no_argument,		NULL, 'H'},
			{"async",	no_argument,		NULL, 'a'},
			{"fifo",	no_argument,		NULL, 'f'},
			{"push",	required_argument,	NULL, 'p'},
			{"blocking",	no_argument,		NULL, 'b'},
			{"size",	required_argument,	NULL, 's'},
			{"readwrite",	no_argument,		NULL, 'r'},
			{"time",	required_argument,	NULL, 't'},
			{"iterations",	required_argument,	NULL, 'i'},
			{"quiet",	no_argument,		NULL, 'q'},
			{"verbose",	optional_argument,	NULL, 'v'},
			{"help",	no_argument,		NULL, 'h'},
			{"version",	no_argument,		NULL, 'V'},
			{"copying",	no_argument,		NULL, 'C'},
			{"?",		no_argument,		NULL, 'h'},
			{NULL,		0,			NULL,  0 }
		};
		/* *INDENT-ON* */

		c = getopt_long(argc, argv, "MwR::S::Fm:Hafp:bs:rt:i:qvhV?W:", long_options,
				&option_index);
#else				/* defined _GNU_SOURCE */
		c = getopt(argc, argv, "MwR::S::Fm:Hafp:bs:rt:i:qvhV?");
#endif				/* defined _GNU_SOURCE */
		if (c == -1)
			break;
		switch (c) {
		case '\1':	/* --hiwat=HIWAT */
			sethiwat = 1;
			hiwat = strtoul(optarg, NULL, 0);
			if (setlowat && hiwat < lowat)
				goto bad_option;
			break;
		case '\2':	/* --lowat=LOWAT */
			setlowat = 1;
			lowat = strtoul(optarg, NULL, 0);
			if (sethiwat && lowat > hiwat)
				goto bad_option;
			break;
		case 'M':
			sndmread = 1;
			break;
		case 'w':
			readfill = 1;
			break;
		case 'R':
			niceread = 1;
			if (optarg != NULL)
				nicerval = strtoul(optarg, NULL, 0);
			else
				nicerval = 19;
			break;
		case 'S':
			nicesend = 1;
			if (optarg != NULL)
				nicesval = strtoul(optarg, NULL, 0);
			else
				nicesval = 19;
			break;
		case 'F':
			fullreads = 1;
			break;
		case 'm':
			if (verbose > 1)
				fprintf(stderr, "Processing -m, --module=%s option\n", optarg);
			if (optarg == NULL)
				goto bad_option;
			if (strnlen(optarg, FMNAMESZ + 1) > FMNAMESZ)
				goto bad_option;
			strncpy(modname, optarg, FMNAMESZ + 1);
			break;
		case 'H':
			strhold = 1;
			break;
		case 'a':
			asynchronous = 1;
			break;
		case 'f':
			if (!native)
				fifo = 1;
			break;
		case 'p':
			if (optarg == NULL) {
				push++;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			push = val;
			break;
		case 'b':
			blocking = 1;
			break;
		case 's':
			msgsize = strtol(optarg, NULL, 0);
			if (1 > msgsize || msgsize > MAXMSGSIZE)
				goto bad_option;
			break;
		case 'r':
			readwrite = 1;
			break;
		case 't':
			report = strtol(optarg, NULL, 0);
			if (1 > report)
				goto bad_option;
			break;
		case 'i':
			iterations = strtol(optarg, NULL, 0);
			if (1 > iterations)
				goto bad_option;
			break;
		case 'q':
			verbose = 0;
			break;
		case 'v':
			if (optarg == NULL) {
				verbose++;
				break;
			}
			if ((val = strtol(optarg, NULL, 0)) < 0)
				goto bad_option;
			verbose = val;
			break;
		case 'h':	/* -h, --help */
			help(argc, argv);
			exit(0);
		case 'V':
			version(argc, argv);
			exit(0);
		case 'C':
			copying(argc, argv);
			exit(0);
		case '?':
		default:
		      bad_option:
			optind--;
		      bad_nonopt:
			if (optind < argc && verbose) {
				fprintf(stderr, "%s: illegal syntax -- ", argv[0]);
				while (optind < argc)
					fprintf(stderr, "%s ", argv[optind++]);
				fprintf(stderr, "\n");
				fflush(stderr);
			}
			goto bad_usage;
		      bad_usage:
			usage(argc, argv);
			exit(2);
		}
	}
	/* 
	 * dont' ignore non-option arguments
	 */
	if (optind < argc)
		goto bad_nonopt;
	copying(argc, argv);
	do_tests();
	exit(0);
}
