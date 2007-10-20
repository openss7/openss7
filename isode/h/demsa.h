/* demsb.h - DEC DEMSA router interface (part a!) */

/* 
 * $Header: /xtel/isode/isode/h/RCS/demsa.h,v 9.0 1992/06/16 12:17:57 isode Rel $
 *
 *
 * $Log: demsa.h,v $
 * Revision 9.0  1992/06/16  12:17:57  isode
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

/* read write and error handlers, defined in compat/select.c */
void xhandler();
void rhandler();
void whandler();
#define   our_TCP_IP_SOCKET 1112
#define   our_X25_SOCKET    1113
#define   our_TABLE_SIZE    100
typedef struct {
 int TCP_IP_OR_X25_SOCKET;
 int descriptor;
} t_list_of_conn_desc;

