/*
 *  Test program for clarification of Berges bug.
 *
 *  This forks three processes:
 *  1) The two sleepers. They ends up sleeping in getpmsg().
 *  2) The hanger. This tries to do a putpmsg() on the
 *     same file where the sleepers are sleeping. It will
 *     hang because the sleeper has downed the inode
 *     semaphore.
 *
 */
#include <sys/stropts.h>
#include <sys/LiS/loop.h>

#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int fd1, fd2;

void sleeper1(void)
{
	struct strbuf datbuf;
	char buf[256];
	int flags;

	datbuf.maxlen = sizeof(buf);
	datbuf.len = 0;
	datbuf.buf = (char *)&buf;
	flags = 0;

	printf("sleeper1: calling getmsg()\n");
	if (getmsg(fd1, NULL, &datbuf, &flags) < 0)
		perror("sleeper1: getmsg()");
	else
		printf("sleeper1: returned from getmsg()\n");
}

void sleeper2(void)
{
	struct strbuf datbuf;
	char buf[256];
	int flags;

	datbuf.maxlen = sizeof(buf);
	datbuf.len = 0;
	datbuf.buf = (char *)&buf;
	flags = 0;

	printf("sleeper2: calling getmsg()\n");
	if (getmsg(fd1, NULL, &datbuf, &flags) < 0)
		perror("sleeper2: getmsg()");
	else
		printf("sleeper2: returned from getmsg()\n");
}

void hanger(void)
{
	struct strbuf datbuf;
	char buf[256];

	sleep(10); /* Sleeper goes to sleep first */

	datbuf.maxlen = sizeof(buf);
	datbuf.len = sizeof(buf);
	datbuf.buf = (char *)&buf;

	printf("hanger: calling putmsg()\n");
	if (putmsg(fd1, NULL, &datbuf, 0) < 0)
		perror("hanger: putmsg()");
	else
		printf("hanger: returned from putmsg()\n");
}

int main(void)
{
	struct strioctl ioc;
	int arg, i;

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
	 *  Start the slepper and hanger processes.
	 */

	if (!fork()) {
		sleeper1();
		exit(0);
	}

	if (!fork()) {
		sleeper2();
		exit(0);
	}

	if (!fork()) {
		hanger();
		exit(0);
	}

	/*
	 *  Wait until things get hung up.
	 */

	sleep(30);

	/*
	 *  Now wakeup sleepers. That will make hanger continue.
	 */

	for (i=1; i < 3; ++i) {
		struct strbuf datbuf;
		char buf[256];

		datbuf.maxlen = sizeof(buf);
		datbuf.len = sizeof(buf);
		datbuf.buf = (char *)&buf;

		printf("main: calling putmsg() to wakeup sleeper%d\n", i);
		if (putmsg(fd2, NULL, &datbuf, 0) < 0)
			perror("main: putmsg()");
		else
			printf("main: returned from putmsg()\n");
	}

	exit(0);
}
