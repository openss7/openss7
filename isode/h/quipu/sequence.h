/* sequence.h - */

/* 
 * $Header: /xtel/isode/isode/h/quipu/RCS/sequence.h,v 9.0 1992/06/16 12:23:11 isode Rel $
 *
 *
 * $Log: sequence.h,v $
 * Revision 9.0  1992/06/16  12:23:11  isode
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


struct dua_sequence {
	char * ds_name;
	struct dua_seq_entry *ds_data;
	struct dua_seq_entry *ds_last;
	struct dua_sequence  *ds_next;
};

#define ds_alloc() (struct dua_sequence *) smalloc(sizeof (struct dua_sequence))
#define NULL_DS ((struct dua_sequence *)0)
DN sequence_dn ();

struct dua_seq_entry {
	DN	de_name;
	struct  dua_seq_entry *de_next;
};

#define de_alloc() (struct dua_seq_entry *) smalloc(sizeof (struct dua_seq_entry))
#define NULL_DE ((struct dua_seq_entry *)0)
