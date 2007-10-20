#ifndef	lint
static char *RCSid = "$Source: /xtel/isode/isode/others/ntp/RCS/test.c,v $ $Revision: 9.0 $ $Date: 1992/06/16 12:42:48 $";
#endif

/*
 * $Log: test.c,v $
 * Revision 9.0  1992/06/16  12:42:48  isode
 * Release 8.0
 *
 */

#include "ntp.h"

#define	TRUE	1
#define	FALSE	0

int test1(), test2(), test3(), test4();
int	debug;
char	*myname;

double value[8] = {5.1, -5.1, 1.5, -1.5, 0.5, -0.5, -0.05, 0.0};
main(argc, argv)
	int argc;
	char **argv;
{
	myname = argv[0];
	if (argc > 1 && strcmp(argv[1], "-v") == 0) {
		exit(test1(1)
		     + test2(1)
		     + test3(1)
		     + test4(1));
	} else {
		if (test3(0))
			exit(3);
		if (test4(0))
			exit(4);
	}
	exit(0);
}


test1()
{
	int i;
	double l_fixed_to_double();
	struct l_fixedpt sample;
	double s_fixed_to_double();
	struct s_fixedpt s_sample;

	for (i = 0; i < 8; i++) {
		(void) printf(" %4.2f ", value[i]);
		double_to_l_fixed(&sample, value[i]);
		(void) printf(" x%#8X.%#8X ", sample.int_part, sample.fraction);
#if	0
		(void) printf(" %4.2f", l_fixed_to_double(&sample));
#endif
		(void) printf("\t");
		double_to_s_fixed(&s_sample, value[i]);
		(void) printf(" x%#4X.%#4X ", s_sample.int_part, s_sample.fraction);
		(void) printf(" %4.2f\n", s_fixed_to_double(&s_sample));
	}
	return 0;
}

test2()
{
	struct timeval tp;
	struct l_fixedpt time_lm;

	(void)gettimeofday(&tp, (struct timezone *) 0);
	tstamp(&time_lm, &tp);

	(void) printf("tv_sec:  %d tv_usec:  %d \n", tp.tv_sec, tp.tv_usec);
	(void) printf("intpart: %lu fraction: %lu \n",
	       ntohl(time_lm.int_part), ntohl(time_lm.fraction));
	(void) printf("intpart: %lX fraction: %lX \n",
	       ntohl(time_lm.int_part), ntohl(time_lm.fraction));
	return 0;
}

test3(v)
	int v;
{
	unsigned long ul = 0x80000001;
	double dbl;

#ifdef	GENERIC_UNS_BUG
	/*
	 *  Hopefully, we can avoid the unsigned issue altogether.  Make sure
	 *  that the high-order (sign) bit is zero, and fiddle from there 
	 */
	dbl = (long)((ul >> 1) & 0x7fffffff);
	dbl *= 2.0;
	if (ul & 1)
		dbl += 1.0;
#else
	dbl = ul;
#ifdef	VAX_COMPILER_FLT_BUG
	if (dbl < 0.0) dbl += 4.294967296e9;
#endif
#endif
	if (dbl != 2147483649.0) {
		(void) printf("test3 fails: can't convert from unsigned long to float\n");
		(void) printf("             (%lu != %15g)\n", ul, dbl);
		(void) printf(
  "Try defining VAX_COMPILER_FLT_BUG or GENERIC_UNS_BUG in the Makefile.\n");
		return 1;
	} else {
		if (v)
			(void) printf("test3 passes\n");
		return 0;
	}
}

test4(v)
	int v;
{
	double dbl = 1024.0 * 1024.0 * 1024.0;	/* 2^30 */
#ifdef SUN_FLT_BUG
	int l = 1.5 * dbl;
	u_long ul = (l<<1);
#else
	u_long ul = 3.0 * dbl;			/* between 2^31 and 2^32 */
#endif
	if (v)
		(void) printf("test4: 3.0*1024.0*1024.0*1024.0 = 0x%08x\n", ul);

	if (ul != 0xc0000000) {
		(void) printf("test4 fails:\n");
		(void) printf("Can't convert unsigned long to double.\n");
		(void) printf("Try defining SUN_FLT_BUG in the Makefile\n");
		return 1;
	} else {
		if (v)
			(void) printf("test4 passes\n");
			return 0;
	}
}
