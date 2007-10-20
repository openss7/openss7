/* $Header: /xtel/isode/isode/others/quipu/uips/pod/RCS/defs.h,v 9.0 1992/06/16 12:44:54 isode Rel $ */

#ifndef PODDEFS
#define PODDEFS

#ifndef NULLCP
#define NULLCP (char *) 0
#endif

typedef short bool;

typedef enum {rfc822, greybook} mailtype;

typedef enum {
  Okay, 
  timelimit, 
  timelimit_w_partial,
  listsizelimit,
  adminlimit,
  adminlimit_w_partial,
  nothingfound, 
  localdsaerror,
  remotedsaerror,
  duaerror,
  attributerror,
  namerror,
  security,
  updaterror,
  serviceerror,
  error_string_given
  } dsEnqError;

typedef struct dsErrorStruct {
  dsEnqError error;
  char *err_mess;
} dsErrorStruct;

#define RESBUF 20000
#define MAXARGS 20
#define STRINGLEN 1000
#define SMALLSTRING 255
#define MAXTYPES  255

#endif

