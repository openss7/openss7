/* template.c - your comments here */

/* 
 * $Header: /xtel/isode/isode/others/quipu/uips/de/RCS/filter.h,v 9.0 1992/06/16 12:45:59 isode Rel $
 *
 *
 * $Log: filter.h,v $
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


struct s_filter * andfilter();
struct s_filter * orfilter();
struct s_filter * eqfilter();
struct s_filter * subsfilter();
struct s_filter * presfilter();
