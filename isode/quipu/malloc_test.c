#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/malloc_test.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/quipu/RCS/malloc_test.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: malloc_test.c,v $
 * Revision 9.0  1992/06/16  12:34:01  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#include <stdio.h>

int	mem_heap;

int	start, finish;

main(argc, argv)
int	argc;
char	**argv;
{
	int	times = 1000;
	int	i;
	char	*big;

	if (argc != 1 && argc != 2) {
		printf("usage: %s [number]\n");
		exit(1);
	}

	if (argc == 2)
		times = atoi(argv[1]);

	start = (int) sbrk(0);
	printf("Before starting, sbrk is %d\n", start);

	big = (char *) malloc(sizeof(char *));
	for (i = 1; i < times + 1; i++) {
		big = (char *) realloc(big, i * sizeof(char *));
	}

	finish = (int) sbrk(0);
	printf("After %d calls to realloc sbrk is %d\n", times, finish);
	printf("Total bytes actually allocated is %d\n", times*sizeof(char *));
	printf("Total memory growth is %d\n", finish - start );
}

attempt_restart()
{
	finish = (int) sbrk(0);
	printf("trouble...sbrk is %d, total growth is %d\n", finish, finish - start);
	exit(1);
}
