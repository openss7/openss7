
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>

#define TX_STATES 5
#define RX_STATES 16

#define CRC_TABLE_LENGTH    512
#define TX_TABLE_LENGTH	    (2 * TX_STATES * 256)	/* 2560 bytes (1 page) */
#define RX_TABLE_LENGTH	    (2 * RX_STATES * 256)	/* 14336 bytes (4 pages) */

static uint16_t _bc_table[256];

static uint16_t
bc_table_value(int bit_string, int bit_length)
{
	int pos;

	for (pos = 0; pos < bit_length; pos++) {
		/* long division basically */
		if (bit_string & 0x1)
			bit_string = (bit_string >> 1) ^ 0x8408;
		else
			bit_string >>= 1;
	}
	return (bit_string);
}

static void
bc_table_generate(void)
{
	int pos = 0, bit_string, bit_length = 8, bit_mask = 0x100;

	do {
		for (bit_string = 0; bit_string < bit_mask; bit_string++, pos++)
			_bc_table[pos] = bc_table_value(bit_string, bit_length);
		bit_length--;
	} while ((bit_mask >>= 1));
};

struct _rx_entry {
	uint bit_string:12  __attribute__ ((packed));
	uint bit_length:4   __attribute__ ((packed));
	uint state:4        __attribute__ ((packed));
	uint sync:1	    __attribute__ ((packed));
	uint hunt:1	    __attribute__ ((packed));
	uint flag:1	    __attribute__ ((packed));
	uint idle:1	    __attribute__ ((packed));
} _rx_table7[RX_STATES * 256], _rx_table8[RX_STATES * 256];

union _rx_union {
	struct _rx_entry __attribute__ ((packed));
	uint value:24 __attribute__ ((packed));
};

static struct _rx_entry
rx_table_valueN(int state, uint8_t byte, int len)
{
	struct _rx_entry result = { 0, };
	int bit_mask = 1;

	result.state = state;
	while (len--) {
		switch (result.state) {
		case 0:	/* 0 */	/* zero not belonging to shared flag nor stuffing bit deletion */
			if (byte & 0x80) {
				result.state = 1;
			} else {
				bit_mask <<= 1;
				result.bit_length += 1;
				result.state = 0;
			}
			break;
		case 1:	/* 01 */
			if (byte & 0x80) {
				result.state = 2;
			} else {
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 2;
				result.state = 0;
			}
			break;
		case 2:	/* 011 */
			if (byte & 0x80) {
				result.state = 3;
			} else {
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 3;
				result.state = 0;
			}
			break;
		case 3:	/* 0111 */
			if (byte & 0x80) {
				result.state = 4;
			} else {
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 4;
				result.state = 0;
			}
			break;
		case 4:	/* 01111 */
			if (byte & 0x80) {
				result.state = 5;
			} else {
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 5;
				result.state = 0;
			}
			break;
		case 5:	/* 011111 */
			if (byte & 0x80) {
				result.state = 7;
			} else {
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 6;
				result.state = 6;
			}
			break;
		case 6:	/* [0]11111[0] */ /* bit deletion */
			if (byte & 0x80) {
				result.state = 9;
			} else {
				result.state = 0;
			}
			break;
		case 7:	/* 0111111 */
			result.sync = 0;
			result.idle = 0;
			if (byte & 0x80) {
				bit_mask <<= 1;
				result.bit_length += 1;
				result.flag = 0;
				result.hunt = 1;
				result.state = 15;
			} else {
				result.flag = 1;
				result.hunt = 0;
				result.state = 8;
			}
			break;
		case 8: /* 0111110 */
			bit_mask = 1;
			result.bit_string = 0;
			result.bit_length = 0;
			result.sync = 1;
			result.flag = 1;
			result.hunt = 0;
			result.idle = 0;
			if (byte & 0x80) {
				result.state = 9;
			} else {
				result.state = 0;
			}
			break;
		case 9:	/* [0]1 */ /* zero from end of flag or bit deletion */
			result.idle = 0;
			if (byte & 0x80) {
				result.state = 10;
			} else {
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 1;
				result.state = 0;
			}
			break;
		case 10:	/* [0]11 */
			result.idle = 0;
			if (byte & 0x80) {
				result.state = 11;
			} else {
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 2;
				result.state = 0;
			}
			break;
		case 11:	/* [0]111 */
			result.idle = 0;
			if (byte & 0x80) {
				result.state = 12;
			} else {
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 3;
				result.state = 0;
			}
			break;
		case 12:	/* [0]1111 */
			result.idle = 0;
			if (byte & 0x80) {
				result.state = 13;
			} else {
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 4;
				result.state = 0;
			}
			break;
		case 13:	/* [0]11111 */
			result.idle = 0;
			if (byte & 0x80) {
				result.state = 14;
			} else {
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_string |= bit_mask;
				bit_mask <<= 1;
				result.bit_length += 5;
				result.state = 6;
			}
			break;
		case 14:	/* [0]111111 */
			result.sync = 0;
			result.idle = 0;
			if (byte & 0x80) {
				result.flag = 0;
				result.hunt = 1;
				result.state = 15;
			} else {
				result.flag = 1;
				result.hunt = 0;
				result.state = 8;
			}
			break;
		case 15:	/* ...1111111 */ /* 7 ones (or 8 ones) */
			result.sync = 0;
			result.flag = 0;
			result.hunt = 1;
			if (byte & 0x80) {
				result.idle = 1;
				result.state = 15;
			} else {
				result.idle = 0;
				result.state = 0;
			}
			break;
		}
		byte <<= 1;
	}
	return result;
}

static struct _rx_entry
rx_table_value7(int state, uint8_t byte)
{
	return rx_table_valueN(state, byte, 7);
}
static struct _rx_entry *
rx_index7(uint j, uint k)
{
	return &_rx_table7[(j << 8) | k];
}
static void
rx_table_generate7(void)
{
	int j, k;

	for (j = 0; j < RX_STATES; j++)
		for (k = 0; k < 256; k++) 
			*rx_index7(j, k) = rx_table_value7(j, k);
}

static struct _rx_entry
rx_table_value8(int state, uint8_t byte)
{
	return rx_table_valueN(state, byte, 8);
}
static struct _rx_entry *
rx_index8(uint j, uint k)
{
	return &_rx_table8[(j << 8) | k];
}
static void
rx_table_generate8(void)
{
	int j, k;

	for (j = 0; j < RX_STATES; j++)
		for (k = 0; k < 256; k++)
			*rx_index8(j, k) = rx_table_value8(j, k);
}

struct _tx_entry {
	ushort bit_string:10	__attribute__ ((packed));	/* the output string */
	ushort bit_length:2	__attribute__ ((packed));	/* length in excess of 8 bits of output string */
	ushort state:3		__attribute__ ((packed));	/* new state */
} _tx_table[TX_STATES * 256];


static struct _tx_entry *
tx_index(uint j, uint k)
{
	return &_tx_table[(j << 8) | k];
}

static struct _tx_entry
tx_table_value(int state, uint8_t byte)
{
	struct _tx_entry result = { 0, };
	int bit_mask = 0x80;
	int len = 8;

	result.state = state;
	result.bit_length = 0;
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
	return result;
}

static void
tx_table_generate(void)
{
	int j, k;

	for (j = 0; j < TX_STATES; j++)
		for (k = 0; k < 256; k++) {
			*tx_index(j, k) = tx_table_value(j, k);
		}
}

static void
bc_table_print(void)
{
	int i;

	bc_table_generate();
	fprintf(stdout, "static const uint16_t bc_table[%d] __attribute__((__aligned__(512))) = {", 256);
	for (i = 0; i < 256; i++) {
		if (!(i & 0x7))
			fprintf(stdout, "\n\t");
		else
			fprintf(stdout, " ");
		fprintf(stdout, "0x%04hx", _bc_table[i]);
		if (i != 255)
			fprintf(stdout, ",");
	}
	fprintf(stdout, "\n};\n\n");
}

static void
tx_table_print(void)
{
	int i;

	tx_table_generate();
	fprintf(stdout, "static const struct tx_entry tx_table[%d] __attribute__((__aligned__(4096))) = {", TX_STATES * 256);
	for (i = 0; i < TX_STATES * 256; i++) {
		if (!(i & 0x3))
			fprintf(stdout, "\n\t");
		else
			fprintf(stdout, " ");
		fprintf(stdout, "{ 0x%03x, 0x%01x, 0x%01x }", 
				_tx_table[i].bit_string, _tx_table[i].bit_length, _tx_table[i].state);
		if (i != TX_STATES * 256 - 1)
			fprintf(stdout, ",");
	}
	fprintf(stdout, "\n};\n\n");
}

static void
rx_table_print7(void)
{
	int i;

	rx_table_generate7();
	fprintf(stdout, "static const struct rx_entry rx_table7[%d] __attribute__((__aligned__(4096))) = {", RX_STATES * 256);
	for (i = 0; i < RX_STATES * 256; i++) {
		if (!(i & 0x1))
			fprintf(stdout, "\n\t");
		else
			fprintf(stdout, " ");
		fprintf(stdout, "{ 0x%03x, 0x%01x, 0x%01x, %d, %d, %d, %d }",
			_rx_table7[i].bit_string, _rx_table7[i].bit_length, _rx_table7[i].state,
			_rx_table7[i].sync, _rx_table7[i].hunt, _rx_table7[i].flag, _rx_table7[i].idle);
		if (i != RX_STATES * 256 - 1)
			fprintf(stdout, ",");
	}
	fprintf(stdout, "\n};\n\n");
}

static void
rx_table_print8(void)
{
	int i;

	rx_table_generate8();
	fprintf(stdout, "static const struct rx_entry rx_table8[%d] __attribute__((__aligned__(4096))) = {", RX_STATES * 256);
	for (i = 0; i < RX_STATES * 256; i++) {
		if (!(i & 0x1))
			fprintf(stdout, "\n\t");
		else
			fprintf(stdout, " ");
		fprintf(stdout, "{ 0x%03x, 0x%01x, 0x%01x, %d, %d, %d, %d }",
			_rx_table8[i].bit_string, _rx_table8[i].bit_length, _rx_table8[i].state,
			_rx_table8[i].sync, _rx_table8[i].hunt, _rx_table8[i].flag, _rx_table8[i].idle);
		if (i != RX_STATES * 256 - 1)
			fprintf(stdout, ",");
	}
	fprintf(stdout, "\n};\n\n");
}

static void
rx_struct_print(void)
{
	fprintf(stdout, "\nstruct rx_entry {\n");
	fprintf(stdout, "\tuint bit_string:12  __attribute__ ((packed));\n");
	fprintf(stdout, "\tuint bit_length:4   __attribute__ ((packed));\n");
	fprintf(stdout, "\tuint state:4        __attribute__ ((packed));\n");
	fprintf(stdout, "\tuint sync:1\t    __attribute__ ((packed));\n");
	fprintf(stdout, "\tuint hunt:1\t    __attribute__ ((packed));\n");
	fprintf(stdout, "\tuint flag:1\t    __attribute__ ((packed));\n");
	fprintf(stdout, "\tuint idle:1\t    __attribute__ ((packed));\n");
	fprintf(stdout, "};\n\n");
}

static void
tx_struct_print(void)
{
	fprintf(stdout, "\nstruct tx_entry {\n");
	fprintf(stdout, "\tushort bit_string:10\t__attribute__ ((packed));\t/* the output string */\n");
	fprintf(stdout, "\tushort bit_length:2\t__attribute__ ((packed));\t/* length in excess of 8 bits of output string */\n");
	fprintf(stdout, "\tushort state:3\t\t__attribute__ ((packed));\t/* new state */\n");
	fprintf(stdout, "};\n\n");
}

static void
tx_table_invariant(void)
{
	int i;
	ushort mask = 0;
	ushort last = *(ushort *) &_tx_table[0];

	for (i = 1; i < TX_STATES * 256; i++) {
		mask |= last ^ *(ushort *) &_tx_table[i];
		last = *(ushort *) &_tx_table[i];
	}
	fprintf(stdout, "/* tx_table size is %d */\n", (int) sizeof(_tx_table));
	if (mask != 0xffff) {
		for (i = 0; i < 16; i++)
			if (!((mask >> i) & 1))
				fprintf(stdout, "/* tx_table invariant in bit position %d */\n", i);
	}
	fprintf(stdout, "\n");
}

static void
rx_table_invariant8(void)
{
	int i;
#if 0
	int j;
#endif
	uint mask = 0;
	uint last = (*(union _rx_union *) &_rx_table8[0]).value;
	uint min = _rx_table8[0].bit_length;
	uint mid = 16;
	uint max = _rx_table8[0].bit_length;
	uint states[RX_STATES][RX_STATES] = { { 0, }, };
	states[0][_rx_table8[0].state]++;

	for (i = 1; i < RX_STATES * 256; i++) {
		mask |= last ^ (*(union _rx_union *) &_rx_table8[i]).value;
		last = (*(union _rx_union *) &_rx_table8[i]).value;
		if (_rx_table8[i].bit_length < min)
			min = _rx_table8[i].bit_length;
		if (_rx_table8[i].bit_length < mid && _rx_table8[i].bit_length != 0)
			mid = _rx_table8[i].bit_length;
		if (_rx_table8[i].bit_length > max)
			max = _rx_table8[i].bit_length;
		states[i>>8][_rx_table8[i].state]++;
	}
	fprintf(stdout, "/* rx_table8 size is %d */\n", (int) sizeof(_rx_table8));
#if 0
	fprintf(stdout, "/* rx_table8 minimum bit length is %u */\n", min);
	fprintf(stdout, "/* rx_table8 lowest  bit length is %u */\n", mid);
	fprintf(stdout, "/* rx_table8 maximum bit length is %u */\n", max);
	for (i = 0; i < RX_STATES; i++)
		for (j = 0; j < RX_STATES; j++)
			fprintf(stdout, "/* rx_table8 state %02d moves to state %02d, %u times */\n", i, j, states[i][j]);
	for (i = 0; i < RX_STATES; i++)
		for (j = 0; j < RX_STATES; j++)
			if (!states[i][j])
				fprintf(stdout, "/* rx_table8 state %02d never moves to state %02d */\n", i, j);
	for (i = 0; i < RX_STATES * 256; i++)
		for (j = 0; j < RX_STATES * 256; j++)
			if (i != j)
				if ((*(union _rx_union *) &_rx_table8[i]).value ==
				    (*(union _rx_union *) &_rx_table8[j]).value)
					fprintf(stdout, "/* rx_table8 state %02d byte 0x%01x same as state %02d byte 0x%01x, new state %02d */\n",
							(i>>8), (i&0xff), (j>>8), (j&0xff), _rx_table8[i].state);
#endif
	if (mask != 0x00ffffff) {
		for (i = 0; i < 24; i++)
			if (!((mask >> i) & 1))
				fprintf(stdout, "/* rx_table8 invariant in bit position %d */\n", i);
	}
	fprintf(stdout, "\n");
}

static void
rx_table_invariant7(void)
{
	int i;
#if 0
	int j;
#endif
	uint mask = 0;
	uint last = (*(union _rx_union *) &_rx_table7[0]).value;
	uint min = _rx_table7[0].bit_length;
	uint mid = 16;
	uint max = _rx_table7[0].bit_length;
	uint states[RX_STATES][RX_STATES] = { { 0, }, };
	states[0][_rx_table7[0].state]++;

	for (i = 1; i < RX_STATES * 256; i++) {
		mask |= last ^ (*(union _rx_union *) &_rx_table7[i]).value;
		last = (*(union _rx_union *) &_rx_table7[i]).value;
		if (_rx_table7[i].bit_length < min)
			min = _rx_table7[i].bit_length;
		if (_rx_table7[i].bit_length < mid && _rx_table7[i].bit_length != 0)
			mid = _rx_table7[i].bit_length;
		if (_rx_table7[i].bit_length > max)
			max = _rx_table7[i].bit_length;
		states[i>>8][_rx_table7[i].state]++;
	}
	fprintf(stdout, "/* rx_table7 size is %d */\n", (int) sizeof(_rx_table7));
#if 0
	fprintf(stdout, "/* rx_table7 minimum bit length is %u */\n", min);
	fprintf(stdout, "/* rx_table7 lowest  bit length is %u */\n", mid);
	fprintf(stdout, "/* rx_table7 maximum bit length is %u */\n", max);
	for (i = 0; i < RX_STATES; i++)
		for (j = 0; j < RX_STATES; j++)
			fprintf(stdout, "/* rx_table7 state %02d moves to state %02d, %u times */\n", i, j, states[i][j]);
	for (i = 0; i < RX_STATES; i++)
		for (j = 0; j < RX_STATES; j++)
			if (!states[i][j])
				fprintf(stdout, "/* rx_table7 state %02d never moves to state %02d */\n", i, j);
#endif
	if (mask != 0x00ffffff) {
		for (i = 0; i < 24; i++)
			if (!((mask >> i) & 1))
				fprintf(stdout, "/* rx_table7 invariant in bit position %d */\n", i);
	}
	fprintf(stdout, "\n");
}

#include "tab.h"    /* check */

int main()
{
	tx_struct_print();
	tx_table_print();
	tx_table_invariant();
	bc_table_print();
	rx_struct_print();
	rx_table_print8();
	rx_table_invariant8();
	rx_table_print7();
	rx_table_invariant7();
	return (0);
}
