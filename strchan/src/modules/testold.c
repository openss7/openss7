/*****************************************************************************

 @(#) $RCSfile: testold.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-04-28 23:39:58 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2008-04-28 23:39:58 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: testold.c,v $
 Revision 0.9.2.2  2008-04-28 23:39:58  brian
 - updated headers for release

 *****************************************************************************/

#ident "@(#) $RCSfile: testold.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-04-28 23:39:58 $"

static char const ident[] = "$RCSfile: testold.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-04-28 23:39:58 $";

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdlib.h>

#include "tabold.h"

struct cd_path {
	uint residue;			/* residue bits */
	uint rbits;			/* number of residue bits */
	ushort bcc;			/* crc for message */
	uint state;			/* current state machine state */
	uint mode;			/* path mode */
	uint type;			/* path frame type */
	uint bytes;			/* number of whole bytes */
	unsigned char *msg;			/* message pointer */
	unsigned char *buf;			/* bit buffer pointer */
	unsigned char *cmp;			/* compression repeat message */
	uint cmp_size;			/* compression repeat message size */
	uint repeat;			/* compression repeat count */
	uint octets;			/* octets counted */
} cd_tx = { 0, }, cd_rx = { 0, };

#define TX_MODE_IDLE 0
#define TX_MODE_FLAG 1
#define TX_MODE_BOF  2
#define TX_MODE_MOF  3
#define TX_MODE_BCC  4

#define BUF_SIZE 4096

unsigned char tx_msg[BUF_SIZE];
unsigned char rx_msg[BUF_SIZE];
uint msg_size = 1024;

unsigned char buf[BUF_SIZE];
uint buf_size = 8;

unsigned long tx_state_count[16] = { 0, };
unsigned long rx_state_count[16] = { 0, };

unsigned long cycles = 0;

#define TABLE 1
//#undef TABLE

#ifdef TABLE
static inline const struct tx_old_entry *tx_lookup(unsigned short i, unsigned char byte)
{
	return &tx_old_table[(i << 8) | byte];
}
#else
/* tests show that the tabular approach is more than two times faster than calculation */
static inline const struct tx_old_entry *tx_lookup(unsigned short state, unsigned char byte)
{
	static struct tx_old_entry result;
	int bit_mask = 0x80;
	int len = 8;

	result.state = state;
	result.bit_length = 0;
	result.bit_string = 0;
	while (len--) {
		if (byte & 0x01) {
			result.bit_string |= bit_mask;
			if (result.state++ == 4) {
				result.state = 0;
				result.bit_length++;
				result.bit_string <<= 1;
			}
		} else
			result.state = 0;
		bit_mask >>= 1;
		byte >>= 1;
	}
	return &result;
}
#endif

static inline const struct rx_old_entry *rx_lookup(unsigned short i, unsigned char byte)
{
	return &rx_old_table8[(i << 8) | byte];
}

static inline uint8_t
cd_rev(uint8_t byte)
{
	int i;
	uint8_t output = 0;

	for (i = 0; i < 8; i++) {
		output <<= 1;
		if (byte & 0x01)
			output |= 1;
		byte >>= 1;
	}
	return (output);
}

int
cd_tx_block(void)
{
	struct cd_path *tx = &cd_tx;
	const struct tx_old_entry *t;

	tx->buf = buf;
	if (tx->mode == TX_MODE_IDLE || tx->mode == TX_MODE_FLAG) {
		if (tx->msg == tx_msg)
			tx->mode = TX_MODE_BOF;
	}
	while (tx->buf < buf + buf_size) {
	      check_rbits:
		/* drain residue bits, if necessary */
		if (tx->rbits >= 8) {
		      drain_rbits:
			//fprintf(stdout, "Draining %d bits\n", tx->rbits);
			*tx->buf++ = cd_rev(tx->residue);
			tx->residue >>= 8;
			tx->rbits -= 8;
			continue;
		}
		switch (tx->mode) {
		case TX_MODE_IDLE:
			/* mark idle */
			//fprintf(stdout, "Marking idle\n");
			tx->residue |= 0xff << tx->rbits;
			tx->rbits += 8;
			goto drain_rbits;
		case TX_MODE_FLAG:
			/* idle flags */
			//fprintf(stdout, "Idling flag\n");
			tx->residue |= 0x7e << tx->rbits;
			tx->rbits += 8;
			goto drain_rbits;
		case TX_MODE_BOF:
			//fprintf(stdout, "Starting frame\n");
			tx->residue |= 0x7e << tx->rbits;
			tx->rbits += 8;
			tx->state = 0;
			tx_state_count[tx->state]++;
			tx->bcc = 0xffff;
			tx->mode = TX_MODE_MOF;
			goto drain_rbits;
		case TX_MODE_MOF:
			if (tx->msg < tx_msg + msg_size) {
				uint byte;

				//fprintf(stdout, "Transmitting frame byte %d\n", tx->msg - tx_msg);
				byte = *tx->msg++;
				tx->bcc = (tx->bcc >> 8) ^ bc_old_table[(tx->bcc ^ byte) & 0x00ff];
				t = tx_lookup(tx->state, byte);
				//fprintf(stdout, "t->bit_string = 0x%x\n", t->bit_string);
				//fprintf(stdout, "t->bit_length = 0x%x\n", 8 + t->bit_length);
				//fprintf(stdout, "t->state      = 0x%x\n", t->state);
				tx->state = t->state;
				tx_state_count[tx->state]++;
				tx->residue |= t->bit_string << tx->rbits;
				tx->rbits += 8 + t->bit_length;
				goto drain_rbits;
			}
			//fprintf(stdout, "Transmitting bcc 1 byte\n");
			/* finished message: add 1st bcc byte */
			tx->bcc = ~(tx->bcc);
			t = tx_lookup(tx->state, tx->bcc);
			tx->state = t->state;
			tx_state_count[tx->state]++;
			tx->residue |= t->bit_string << tx->rbits;
			tx->rbits += 8 + t->bit_length;
			tx->mode = TX_MODE_BCC;
			goto drain_rbits;
		case TX_MODE_BCC:
			//fprintf(stdout, "Transmitting bcc 2 byte\n");
			t = tx_lookup(tx->state, tx->bcc >> 8);
			tx->state = t->state;
			tx_state_count[tx->state]++;
			tx->residue |= t->bit_string << tx->rbits;
			tx->rbits += 8 + t->bit_length;
			tx->mode = TX_MODE_FLAG;
			goto drain_rbits;
		}
		fprintf(stderr, "Software error.\n");
	}
	return (tx->msg == tx_msg + msg_size && tx->mode == TX_MODE_FLAG);
}

#define RX_MODE_IDLE 0
#define RX_MODE_HUNT 1
#define RX_MODE_SYNC 2
#define RX_MODE_MOF  3

void
ch_rx_block(void)
{
	struct cd_path *rx = &cd_rx;
	const struct rx_old_entry *r;

	rx->buf = buf;

	while (rx->buf < buf + buf_size) {
		r = rx_lookup(rx->state, cd_rev(*rx->buf++));
		rx->state = r->state;
		rx_state_count[rx->state]++;
		switch (rx->mode) {
		case RX_MODE_MOF:
			// fprintf(stdout, "Middle of frame\n");
			if (!r->sync && r->bit_length) {
				rx->residue |= r->bit_string << rx->rbits;
				rx->rbits += r->bit_length;
			}
			if (!r->flag) {
				if (r->hunt || r->idle)
					goto aborted;
				while (rx->rbits > 16) {
					if (rx->msg == rx_msg + msg_size)
						goto buffer_overflow;
					rx->bcc = (rx->bcc >> 8)
					    ^ bc_old_table[(rx->bcc ^ rx->residue) & 0x00ff];
					*rx->msg++ = rx->residue;
					rx->residue >>= 8;
					rx->rbits -= 8;
					rx->bytes++;
					if (rx->bytes > msg_size)
						goto frame_too_long;
				}
			} else {
				if (rx->rbits != 16)
					goto residue_error;
				if ((~rx->bcc & 0xffff) != (rx->residue & 0xffff))
					goto crc_error;
				if (rx->bytes < 1)
					goto frame_too_short;
			      new_frame:
				rx->mode = RX_MODE_SYNC;
				rx->residue = 0;
				rx->rbits = 0;
				rx->bytes = 0;
				rx->bcc = 0xffff;
				if (r->sync) {
				      begin_frame:
					if (r->bit_length) {
						rx->mode = RX_MODE_MOF;
						rx->residue = r->bit_string;
						rx->rbits = r->bit_length;
						rx->bytes = 0;
						rx->bcc = 0xffff;
					}
				}
			}
			break;
		case RX_MODE_SYNC:
			// fprintf(stdout, "Synced\n");
			if (!r->hunt && !r->idle)
				goto begin_frame;
		      start_hunt:
			if (r->idle) {
				rx->mode = RX_MODE_IDLE;
				fprintf(stderr, "hdlc idle\n");
			} else {
				rx->mode = RX_MODE_HUNT;
				fprintf(stderr, "hdlc abort\n");
			}
			rx->octets = 0;
			break;
		case RX_MODE_IDLE:
			fprintf(stdout, "Idle\n");
			if (!r->idle) {
				rx->mode = RX_MODE_HUNT;
				fprintf(stderr, "hdlc not idle\n");
			}
			/* fall through */
		case RX_MODE_HUNT:
			fprintf(stdout, "Hunt\n");
			if (!r->flag) {
				if (r->idle && rx->mode != RX_MODE_IDLE) {
					rx->mode = RX_MODE_IDLE;
					fprintf(stderr, "hdlc idle\n");
				}
				break;
			}
			fprintf(stdout, "Flag\n");
			goto new_frame;
		      frame_too_long:
			fprintf(stderr, "frame too long\n");
			goto abort_frame;
		      buffer_overflow:
			fprintf(stderr, "buffer overflow\n");
			goto abort_frame;
		      aborted:
			fprintf(stderr, "aborted\n");
			goto abort_frame;
		      frame_too_short:
			fprintf(stderr, "frame too short\n");
			goto abort_frame;
		      crc_error:
			fprintf(stderr, "crc error\n");
			goto abort_frame;
		      residue_error:
			fprintf(stderr, "residue error\n");
			goto abort_frame;
		      abort_frame:
			if (r->flag)
				goto new_frame;
			goto start_hunt;
		default:
			fprintf(stderr, "Software error\n");
			goto abort_frame;
		}
	}
}

int
main()
{
	int i, j, k, l;

	(void) j;
	for (l = 0; l < 100000; l++) {
		while (!(msg_size = (random() & 0xff))) ;

		for (i = 0; i < msg_size; i++)
			tx_msg[i] = random();

		cd_tx.msg = tx_msg;
		cd_rx.msg = rx_msg;

		for (i = 0; ; i++) {
			int ret;

			ret = cd_tx_block();
#if 0
			// fprintf(stdout, "tx->mode = %d\n", cd_tx.mode);
			for (j = 0; j < buf_size; j++) {
				unsigned char byte = buf[j];

				for (k = 0; k < 8; k++) {
					if (byte & 0x80)
						fprintf(stdout, "1");
					else
						fprintf(stdout, "0");
					byte <<= 1;
				}
			}
			fprintf(stdout, "\n");
#endif
			// buf[(random()&0x1f)] = 0xff;
			ch_rx_block();
			cycles++;
			if (ret)
				break;
		}
		cd_tx_block();
		ch_rx_block();
		cycles++;
#if 0
		fprintf(stdout, "Transmit took %d iterations.\n", i);
		for (i = 0; i < msg_size; i++)
			fprintf(stdout, " %02x", tx_msg[i]);
		fprintf(stdout, "\n");
		for (i = 0; i < msg_size; i++)
			fprintf(stdout, " %02x", rx_msg[i]);
		fprintf(stdout, "\n");
#endif
		for (i = 0; i < msg_size; i++)
			if (tx_msg[i] != rx_msg[i]) {
				fprintf(stderr, "tx and rx messages %d differ at byte position %d\n",
					l, i);
				for (j = 0; j < buf_size; j++) {
					unsigned char byte = buf[j];

					for (k = 0; k < 8; k++) {
						if (byte & 0x80)
							fprintf(stdout, "1");
						else
							fprintf(stdout, "0");
						byte <<= 1;
					}
				}
				fprintf(stdout, "\n");
				for (i = 0; i < msg_size; i++)
					fprintf(stdout, " %02x", tx_msg[i]);
				fprintf(stdout, "\n");
				for (i = 0; i < msg_size; i++)
					fprintf(stdout, " %02x", rx_msg[i]);
				fprintf(stdout, "\n");
				return (1);
			}
	}
#if 0
	for (j = 0; j < 16; j++)
		fprintf(stdout, "TX state machine was in state %02d %ld times\n", j, tx_state_count[j]);
	for (j = 0; j < 16; j++)
		fprintf(stdout, "RX state machine was in state %02d %ld times\n", j, rx_state_count[j]);
#endif
	fprintf(stdout, "%d random messages successful\n", l);
	fprintf(stdout, "%d cycles of %d bytes each for %d bits\n", cycles, buf_size, cycles*buf_size*8);
	return (0);
}
