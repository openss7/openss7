#ident "@(#) bind_clns.c 1.3 95/01/31 SMI"
/*
 * This example program shows how to use TLI over CLNS.
 * The main emphasis is on the address format to set in the
 * netbuf structure.
 *
 * Copyright 1995 Sun Microsystems, Inc.  All Rights Reserved.
 */

#include <tiuser.h>
#include <fcntl.h>
#include <stdio.h>

#define CHAR2INT(n) ((n > 0x39)?(n - 'a' + 0xa) : (n - 0x30))
#define MAX_LEN 40

int main(int argc, char *argv[])
{
}
