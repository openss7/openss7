/* template.c - your comments here */

/* 
 * $Header: /xtel/isode/isode/others/quipu/uips/de/RCS/bind.h,v 9.0 1992/06/16 12:45:59 isode Rel $
 *
 *
 * $Log: bind.h,v $
 * Revision 9.0  1992/06/16  12:45:59  isode
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


#ifndef _xdua_h_
#define _xdua_h_

#include "types.h"

typedef enum {
  EXTERNAL_AUTH,
  STRONG_AUTH,
  PROTECTED_AUTH,
  SIMPLE_AUTH
  } auth_bind_type;

int bind_to_ds();

#endif _xdua_h_
