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
