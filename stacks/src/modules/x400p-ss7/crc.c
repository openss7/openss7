
/*
 *  Adds crc-16 to bytes p[len] and p[len+1]
 */
void addcrc(register unsigned char *p, register unsigned int len)
{
	register unsigned int i, bit, dat, bcc = 0x000000ff;
	while (len--) {
		dat = *(p++);
		i = 8;
		while (i--) {
			bit = (bcc ^ (dat & 0x1)) & 0x1;
			bcc >>= 1;
			dat >>= 1;
			if (bit)
				bcc ^= 0x00008408;
		}
	}
	*(p++) = bcc & 0xff;
	bcc >>= 8;
	*(p++) = bcc & 0xff;
	return;
}

#define DO_ONEBIT(bit, dat, bcc) \
do { \
    bit = (bcc ^ (dat & 0x1)) & 0x1; \
    bcc >>= 1; \
    dat >>= 1; \
    if (bit) \
	bcc ^= 0x00008408; \
} while(0)

#define DO_1(bit, dat, bcc) \
do { \
    DO_ONEBIT(bit, dat, bcc); \
    DO_ONEBIT(bit, dat, bcc); \
    DO_ONEBIT(bit, dat, bcc); \
    DO_ONEBIT(bit, dat, bcc); \
    DO_ONEBIT(bit, dat, bcc); \
    DO_ONEBIT(bit, dat, bcc); \
    DO_ONEBIT(bit, dat, bcc); \
    DO_ONEBIT(bit, dat, bcc); \
} while(0)

#define DO_4(bit, dat, bcc) \
do { \
    DO_1(bit, dat, bcc); \
    DO_1(bit, dat, bcc); \
    DO_1(bit, dat, bcc); \
    DO_1(bit, dat, bcc); \
} while(0)

/*
 *  Faster no loop
 */
void addcrc2(register unsigned char *p, register unsigned int len)
{
	register unsigned int bit, dat, bcc = 0x000000ff;
	while (((unsigned int) p & 0x3) && len--) {
		dat = *p++;
		DO_1(bit, dat, bcc);
	}
	while (len >= 4) {
		dat = *((unsigned int *) p)++;
		len -= 4;
		DO_4(bit, dat, bcc);
	}
	while (len--) {
		dat = *p++;
		DO_1(bit, dat, bcc);
	}
	*(p++) = bcc & 0xff;
	bcc >>= 8;
	*(p++) = bcc & 0xff;
	return;
}

/*
 *  Faster, table lookup instead of bit manipulation
 */
#define DOT_4(dat, bcc) \
do { \
    DOT_1(dat, bcc); \
    dat >>= 8; \
    DOT_1(dat, bcc); \
    dat >>= 8; \
    DOT_1(dat, bcc); \
    dat >>= 8; \
    DOT_1(dat, bcc); \
} while(0);

void addcrc3(register unsigned char *p, register unsigned int len)
{
	register unsigned int bit, dat, bcc = 0x000000ff;
	while (((unsigned int) p & 0x3) && len--) {
		dat = *p++;
		DO_1(bit, dat, bcc);
	}
	while (len >= 4) {
		dat = *((unsigned int *) p)++;
		len -= 4;
		DO_4(bit, dat, bcc);
	}
	while (len--) {
		dat = *p++;
		DO_1(bit, dat, bcc);
	}
	*(p++) = bcc & 0xff;
	bcc >>= 8;
	*(p++) = bcc & 0xff;
	return;
}

/*
 *  BIT DELETION ALGORITHM
 *  -------------------------------------------------------------------------
 *  This is a tabular approach similar to below, except that it does flag
 *  detection, SOF/EOF detection, and abort detection at the same time.
 */

//  State       Input       Output      Next State
//  0           0           0           1
//  0           1           1           0
//  1           0           0           1
//  1           1           1           2
//  2           0           0           1
//  2           1           1           3
//  3           0           0
//
//
//  10          0           0           10
//  10          1           1           11
//  11          0           0           10
//  11          1           1           12
//  12          0           0           10
//  12          1           1           13
//  13          0           0           10
//  13          1           1           14
//  14          0           0           10
//  14          1           1           10

/*
 *  BIT INSERTION ALORITHM
 *  -------------------------------------------------------------------------
 *  This is an investigation of a tabular approach for bit insertion based on
 *  byte sized input data and a finite state machine.  A the byte based state
 *  table is acheived by determining the chain of state transitions in a bit
 *  based FSM that would result from a given input byte.
 */

//  State       Input       Output      Next State
//  0           0           0           0
//  0           1           1           1
//  1           0           0           0
//  1           1           1           2
//  2           0           0           0
//  2           1           1           3
//  3           0           0           0
//  3           1           1           4
//  4           0           0           0
//  4           1           10          0

/*
 *  The byte tabular approach would be to process 8 input bits at a time
 *  resuling in a new state, and an output bit stream between 8 bits and 10
 *  bits inclusive.  It takes 3 bits to specify the new state, 2 bits to
 *  specify the output bit string length, and 10 bits max for the output
 *  string: this is 15 bits that can be contained in a 16 bit tabular result.
 *
 *  Each table point can be calculated as follows:
 */

uint16_t table_value(int cur_state, uint8_t inp_byte)
{
	int new_state = cur_state;
	int len = 8;
	int out_string = 0;
	int ext_length = 0;
	int bit_mask = 1;
	while (len--) {
		if (inp_byte & 0x1) {
			out_string |= bit_mask;
			if (new_state++ == 4) {
				new_state = 0;
				ext_length++;
				bit_mask <<= 1;
			}
		} else {
			new_state = 0;
		}
		bit_mask <<= 1;
	}
	return (out_string | ((ext_length & 0x3) << 10) | ((new_state & 0x7) << 12));
}
