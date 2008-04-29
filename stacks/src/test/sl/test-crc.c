/*****************************************************************************

 @(#) $RCSfile: test-crc.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-04-29 07:11:30 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
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
 Affero General Public License (GPL) Version 3, so long as the software is
 distributed with, and only used for the testing of, OpenSS7 modules, drivers,
 and libraries.

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

 Last Modified $Date: 2008-04-29 07:11:30 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: test-crc.c,v $
 Revision 0.9.2.2  2008-04-29 07:11:30  brian
 - updating headers for release

 *****************************************************************************/

#ident "@(#) $RCSfile: test-crc.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-04-29 07:11:30 $"

static char const ident[] = "$RCSfile: test-crc.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-04-29 07:11:30 $";

#include <stropts.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include <string.h>
#include <time.h>

#if 1
unsigned char data[] = {
	0xff, 0xff, 0x01, 0x00, 0x27, 0xe6
};
#else
unsigned char data[] = {
	0xff, 0xff, 0x80, 0x00, 0xe4, 0x67
};
#endif

int dlen = 4;

// 01111110 11111111 11111111 10000000 00000000 11100100 01100111
//
// 1 0001 0000 0010 0001

typedef ushort bc_entry_t;

bc_entry_t bc_table[1024];

#if 1
ulong init = 0x0000ffff;
ulong poly = 0x00008408;
#else
ulong init = 0x0000ff00;
ulong poly = 0x00001201;
#endif

bc_entry_t bc_table_value(uint bit_string, uint bit_length)
{
	int pos;
	for (pos = 0; pos < bit_length; pos++) {
		if (bit_string & 0x1)
			bit_string = (bit_string >> 1) ^ poly;
		else
			bit_string >>= 1;
	}
	return (bit_string);
}

void bc_table_generate(void)
{
	int pos = 0, bit_string, bit_length = 8, bit_mask = 0x100;
	do {
		for (bit_string = 0; bit_string < bit_mask; bit_string++, pos++)
			bc_table[pos] = bc_table_value(bit_string, bit_length);
		bit_length--;
	} while ((bit_mask >>= 1));
}

int main(int argc, char **argv)
{
	int i;
	ulong bcc = init, mask = 0x0001;
	bc_table_generate();
	for (i = 0; i < dlen; i++)
		bcc = (bcc >> 8) ^ bc_table[(bcc ^ data[i]) & 0x00ff];
	bcc = ~bcc;
	printf("BCC = ");
	for (i = 0; i < 16; i++) {
		if (bcc & mask)
			printf("1");
		else
			printf("0");
		if (!((i + 1) & 0x7))
			printf(" ");
		mask <<= 1;
	}
	printf("\n");
	{
		unsigned char *ptr = data;
		unsigned char src = 0;
		bcc = init;
		while (dlen--) {
			src = *ptr++;
			for (i = 0; i < 8; i++) {
				if ((src ^ bcc) & 0x1) {
					src >>= 1;
					bcc >>= 1;
					bcc ^= poly;
				} else {
					src >>= 1;
					bcc >>= 1;
				}
			}
		}
	}
	bcc = ~bcc;
	printf("BCC = 11100100 01100111 <- should be\n");
	printf("BCC = ");
	for (i = 0; i < 16; i++) {
		if (bcc & 0x1)
			printf("1");
		else
			printf("0");
		if (!((i + 1) & 0x7))
			printf(" ");
		bcc >>= 1;
	}
	printf("\n");
	return (0);
}
