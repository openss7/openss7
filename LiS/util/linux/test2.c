/*
 *  Test program for clarification of bug with inode locking.
 *
 *  This forks two processes:
 *  1) writer1. This does blocking putmsg() on /dev/loop.1.
 *  2) writer2. This opens another file on /dev/loop.1 and does
 *     non-blocking putmsg(). Whenever the putmsg() call takes
 *     too long, a warning is printed.
 *  The main process does getmsg() from /dev/loop.2 (connected
 *  to /dev/loop.1), but slowly.
 *
 */
#include <sys/stropts.h>
#include <sys/LiS/loop.h>

#include <sys/time.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int fd1, fd2;

void writer1(void)
{
	struct strbuf datbuf;
	char buf[1024];

	buf[0] = 'A' ;
	while (1) {
		usleep(1000);

		datbuf.maxlen = sizeof(buf);
		datbuf.len = sizeof(buf);
		datbuf.buf = (char *)&buf;

		printf("a"); fflush(stdout);
		if (putmsg(fd1, NULL, &datbuf, 0) < 0)
			perror("writer1: putmsg()");
	}
}

void writer2(void)
{
	int fd;
	struct strbuf datbuf;
	char buf[1024];
	int ret;
	struct timeval tv1, tv2;
	int t;

	if ((fd = open("/dev/loop.1", O_RDWR|O_NONBLOCK)) < 0) {
		perror("writer2: open(fd)");
		exit(1);
	}

	buf[0] = 'B' ;
	while (1) {
		datbuf.maxlen = sizeof(buf);
		datbuf.len = sizeof(buf);
		datbuf.buf = (char *)&buf;

		gettimeofday(&tv1, NULL);
		ret = putmsg(fd, NULL, &datbuf, 0);
		gettimeofday(&tv2, NULL);

		t = tv2.tv_usec - tv1.tv_usec + 1000000 * (tv2.tv_sec - tv1.tv_sec);
		if (t > 500000) {
			printf("\n*** Nonblocking putmsg() duration: %d microsecs.\n", t);
			fflush(stdout);
		}

		if (ret < 0) {
			if (errno != EWOULDBLOCK)
				perror("writer2: putmsg()");
			else {
				/*
				printf("\n*** Nonblocking putmsg(): Would block.\n");
				fflush(stdout);
				 */
				usleep(1);
			}
		} else {
			printf("b");
			fflush(stdout);
		}
	}
}

void reader(void)
{
	struct strbuf datbuf;
	char buf[1024];
	int flags;

	while (1) {
		usleep(200000); /* Slow reader.. */

		datbuf.maxlen = sizeof(buf);
		datbuf.len = 0;
		datbuf.buf = (char *)&buf;
		flags = 0;

		if (getmsg(fd2, NULL, &datbuf, &flags) < 0)
			perror("reader: getmsg()");
		else
			printf("%c", buf[0]); fflush(stdout);
	}
}


int main( int argc, char *argv[] )
{
	struct strioctl ioc;
	int arg;

	/*
	 *  Make this a process group leader
         *  so that signals hit all children.
	 */
        if (setpgrp() < 0) {
                perror("setpgrp()");
                exit(1);
        }

	/*
	 *  Open two loop endpoints and connect them.
	 */
	if ((fd1 = open("/dev/loop.1", O_RDWR)) < 0) {
		perror("open(fd1)");
		exit(1);
	}

	if ((fd2 = open("/dev/loop.2", O_RDWR)) < 0) {
		perror("open(fd2)");
		exit(1);
	}

	arg = 2;
	ioc.ic_cmd = LOOP_SET;
	ioc.ic_timout = 10;
	ioc.ic_len = sizeof(int);
	ioc.ic_dp = (char *) &arg;

	if (ioctl(fd1, I_STR, &ioc) < 0) {
		perror("ioctl()");
		exit(1);
	}

	/*
	 *  Start the writer processes
	 */
	if (!fork())
		writer1();

	if (!fork())
		writer2();

	/*
	 *  Now run reader
	 */
	reader();

	exit(0);
}
