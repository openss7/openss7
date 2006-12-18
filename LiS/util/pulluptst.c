/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

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

 Last Modified $Date$ by $Author$

 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

/************************************************************************
*                   Test Program for Pullupmsg                          *
************************************************************************/

#include <sys/stream.h>

/************************************************************************
*                             xit                                       *
*************************************************************************
*									*
* This routine is called to exit the program when a test fails.		*
*									*
************************************************************************/
void
xit(void)
{
	printf("\n\n\n");
	printf("****************************************************\n");
	printf("*                  Test Failed                     *\n");
	printf("****************************************************\n\n");

	printf("Dump of memory areas in use:\n\n");

	port_print_mem();
	exit(1);

}				/* xit */

/************************************************************************
*                          set_debug_mask                               *
*************************************************************************
*									*
* Use stream ioctl to set the debug mask for streams.			*
*									*
************************************************************************/
void
set_debug_mask(long msk)
{
	int fd;
	int rslt;

	fd = user_open("loop.1", 0, 0);
	if (fd < 0) {
		printf("loop.1: %s\n", strerror(-fd));
		xit();
	}

	rslt = user_ioctl(fd, I_LIS_SDBGMSK, msk);
	if (rslt < 0) {
		printf("loop.1: I_LIS_SDBGMSK: %s\n", strerror(-rslt));
		xit();
	}

	printf("\nSTREAMS debug mask set to 0x%08lx\n", msk);

	user_close(fd);

}				/* set_debug_mask */

void
pullupmsg_test(void)
{
	mblk_t *mp;
	mblk_t *mp2;
	int i;

	printf("\nTwo blocks, each with 10 bytes, pullupmsg(15)\n");
	mp = allocb(10, BPRI_MED);
	if (mp == (mblk_t *) NULL) {
		printf("Failed to allocate message\n");
		xit();
	}

	for (i = 0; i < 10; i++)
		*mp->b_wptr++ = (unsigned char) i;

	mp2 = allocb(10, BPRI_MED);
	if (mp2 == (mblk_t *) NULL) {
		printf("Failed to allocate message\n");
		xit();
	}

	for (i = 0; i < 10; i++)
		*mp2->b_wptr++ = (unsigned char) i;

	mp->b_cont = mp2;

	lis_print_msg(mp, "before", PRINT_DATA_ENTIRE);
	pullupmsg(mp, 15);
	lis_print_msg(mp, "after", PRINT_DATA_ENTIRE);
	freemsg(mp);
	printf("\nMemory at end of test\n");
	port_print_mem();

	printf("\n\nTwo blocks, each with 80 bytes, pullupmsg(100)\n");
	mp = allocb(80, BPRI_MED);
	if (mp == (mblk_t *) NULL) {
		printf("Failed to allocate message\n");
		xit();
	}

	for (i = 0; i < 80; i++)
		*mp->b_wptr++ = (unsigned char) i;

	mp2 = allocb(80, BPRI_MED);
	if (mp2 == (mblk_t *) NULL) {
		printf("Failed to allocate message\n");
		xit();
	}

	for (i = 0; i < 80; i++)
		*mp2->b_wptr++ = (unsigned char) i;

	mp->b_cont = mp2;

	lis_print_msg(mp, "before", PRINT_DATA_ENTIRE);
	pullupmsg(mp, 100);
	lis_print_msg(mp, "after", PRINT_DATA_ENTIRE);
	freemsg(mp);
	printf("\nMemory at end of test\n");
	port_print_mem();

	printf("\n\nTwo blocks, each with 80 bytes, pullupmsg(-1)\n");
	mp = allocb(80, BPRI_MED);
	if (mp == (mblk_t *) NULL) {
		printf("Failed to allocate message\n");
		xit();
	}

	for (i = 0; i < 80; i++)
		*mp->b_wptr++ = (unsigned char) i;

	mp2 = allocb(80, BPRI_MED);
	if (mp2 == (mblk_t *) NULL) {
		printf("Failed to allocate message\n");
		xit();
	}

	for (i = 0; i < 80; i++)
		*mp2->b_wptr++ = (unsigned char) i;

	mp->b_cont = mp2;

	lis_print_msg(mp, "before", PRINT_DATA_ENTIRE);
	pullupmsg(mp, -1);
	lis_print_msg(mp, "after", PRINT_DATA_ENTIRE);
	freemsg(mp);
	printf("\nMemory at end of test\n");
	port_print_mem();

}				/* pullupmsg_test */

void
msgpullup_test(void)
{
	mblk_t *mp;
	mblk_t *mp2;
	mblk_t *mpnew;
	int i;

	printf("\nTwo blocks, each with 10 bytes, msgpullup(30) should fail\n");
	mp = allocb(10, BPRI_MED);
	if (mp == (mblk_t *) NULL) {
		printf("Failed to allocate message\n");
		xit();
	}

	for (i = 0; i < 10; i++)
		*mp->b_wptr++ = (unsigned char) i;

	mp2 = allocb(10, BPRI_MED);
	if (mp2 == (mblk_t *) NULL) {
		printf("Failed to allocate message\n");
		xit();
	}

	for (i = 0; i < 10; i++)
		*mp2->b_wptr++ = (unsigned char) i;

	mp->b_cont = mp2;

	lis_print_msg(mp, "before", PRINT_DATA_ENTIRE);
	mpnew = msgpullup(mp, 30);
	if (mpnew != NULL) {
		printf("\nERROR: msgpullup should have returned NULL\n");
		lis_print_msg(mp, "after", PRINT_DATA_ENTIRE);
		lis_print_msg(mpnew, "new", PRINT_DATA_ENTIRE);
		freemsg(mpnew);
	} else
		printf("msgpullup return NULL pointer correctly\n");

	freemsg(mp);
	printf("\nMemory at end of test\n");
	port_print_mem();

	printf("\nTwo blocks, each with 10 bytes, msgpullup(15)\n");
	mp = allocb(10, BPRI_MED);
	if (mp == (mblk_t *) NULL) {
		printf("Failed to allocate message\n");
		xit();
	}

	for (i = 0; i < 10; i++)
		*mp->b_wptr++ = (unsigned char) i;

	mp2 = allocb(10, BPRI_MED);
	if (mp2 == (mblk_t *) NULL) {
		printf("Failed to allocate message\n");
		xit();
	}

	for (i = 0; i < 10; i++)
		*mp2->b_wptr++ = (unsigned char) i;

	mp->b_cont = mp2;

	lis_print_msg(mp, "before", PRINT_DATA_ENTIRE);
	mpnew = msgpullup(mp, 15);
	lis_print_msg(mp, "after", PRINT_DATA_ENTIRE);
	lis_print_msg(mpnew, "new", PRINT_DATA_ENTIRE);
	freemsg(mp);
	freemsg(mpnew);
	printf("\nMemory at end of test\n");
	port_print_mem();

	printf("\n\nTwo blocks, each with 80 bytes, msgpullup(100)\n");
	mp = allocb(80, BPRI_MED);
	if (mp == (mblk_t *) NULL) {
		printf("Failed to allocate message\n");
		xit();
	}

	for (i = 0; i < 80; i++)
		*mp->b_wptr++ = (unsigned char) i;

	mp2 = allocb(80, BPRI_MED);
	if (mp2 == (mblk_t *) NULL) {
		printf("Failed to allocate message\n");
		xit();
	}

	for (i = 0; i < 80; i++)
		*mp2->b_wptr++ = (unsigned char) i;

	mp->b_cont = mp2;

	lis_print_msg(mp, "before", PRINT_DATA_ENTIRE);
	mpnew = msgpullup(mp, 100);
	lis_print_msg(mp, "after", PRINT_DATA_ENTIRE);
	lis_print_msg(mpnew, "new", PRINT_DATA_ENTIRE);
	freemsg(mp);
	freemsg(mpnew);
	printf("\nMemory at end of test\n");
	port_print_mem();

	printf("\n\nTwo blocks, each with 80 bytes, msgpullup(-1)\n");
	mp = allocb(80, BPRI_MED);
	if (mp == (mblk_t *) NULL) {
		printf("Failed to allocate message\n");
		xit();
	}

	for (i = 0; i < 80; i++)
		*mp->b_wptr++ = (unsigned char) i;

	mp2 = allocb(80, BPRI_MED);
	if (mp2 == (mblk_t *) NULL) {
		printf("Failed to allocate message\n");
		xit();
	}

	for (i = 0; i < 80; i++)
		*mp2->b_wptr++ = (unsigned char) i;

	mp->b_cont = mp2;

	lis_print_msg(mp, "before", PRINT_DATA_ENTIRE);
	mpnew = msgpullup(mp, -1);
	lis_print_msg(mp, "after", PRINT_DATA_ENTIRE);
	lis_print_msg(mpnew, "new", PRINT_DATA_ENTIRE);
	freemsg(mp);
	freemsg(mpnew);

	printf("\n\nThree blocks, each with 80 bytes, msgpullup(100)\n");
	mp = allocb(80, BPRI_MED);
	if (mp == (mblk_t *) NULL) {
		printf("Failed to allocate message\n");
		xit();
	}

	for (i = 0; i < 80; i++)
		*mp->b_wptr++ = (unsigned char) i;

	mp2 = allocb(80, BPRI_MED);
	if (mp2 == (mblk_t *) NULL) {
		printf("Failed to allocate message\n");
		xit();
	}

	for (i = 0; i < 80; i++)
		*mp2->b_wptr++ = (unsigned char) i;

	mp->b_cont = mp2;

	mp2 = allocb(80, BPRI_MED);
	if (mp2 == (mblk_t *) NULL) {
		printf("Failed to allocate message\n");
		xit();
	}

	for (i = 0; i < 80; i++)
		*mp2->b_wptr++ = (unsigned char) i;

	mp->b_cont->b_cont = mp2;

	lis_print_msg(mp, "before", PRINT_DATA_ENTIRE);
	mpnew = msgpullup(mp, 100);
	lis_print_msg(mp, "after", PRINT_DATA_ENTIRE);
	lis_print_msg(mpnew, "new", PRINT_DATA_ENTIRE);
	freemsg(mp);
	freemsg(mpnew);
	printf("\nMemory at end of test\n");
	port_print_mem();

	printf("\nMemory at end of test\n");
	port_print_mem();

}				/* msgpullup_test */

void
main(void)
{
	port_init();		/* stream head init routine */
	make_nodes();
	set_debug_mask(0xFFFFFFFF);

	pullupmsg_test();
	msgpullup_test();

}
