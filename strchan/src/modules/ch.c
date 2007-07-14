/*****************************************************************************

 @(#) $RCSfile: ch.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/07/14 01:35:37 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2007/07/14 01:35:37 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: ch.c,v $
 Revision 0.9.2.3  2007/07/14 01:35:37  brian
 - make license explicit, add documentation

 Revision 0.9.2.2  2007/03/25 19:00:54  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.1  2006/11/30 13:01:05  brian
 - added working files

 *****************************************************************************/

#ident "@(#) $RCSfile: ch.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/07/14 01:35:37 $"

static char const ident[] = "$RCSfile: ch.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/07/14 01:35:37 $";

/*
 *  This is a CH module.  The purpose of the module is for it to be pushed over a (circuit-based) MX
 *  stream and provide statistical multiplexing access to one or more channels in one or more spans.
 */

#define _LFS_SOURCE
#define _MPS_SOURCE

#include <sys/os8/compat.h>

#ifndef DB_TYPE
#define DB_TYPE(__mp) ((__mp)->b_datap->db_type)
#endif

#if 1
#define STRLOG(mid, sid, level, flags, ...) strlog(mid, sid, level, flags, __VA_ARGS__)
#else
#define STRLOG(mid, sid, level, flags, ...) do { } while (0)
#endif

#define CHLOGST	 1		/* log CH state transitions */
#define MXLOGST	 2		/* log MX state transitions */
#define CHLOGTO	 3		/* log CH timeouts */
#define MXLOGTO	 4		/* log MX timeouts */
#define CHLOGRX	 5		/* log primitives received from CH user */
#define CHLOGTX	 6		/* log primitives issued to CH user */
#define MXLOGRX	 7		/* log primitives received from MX provider */
#define MXLOGTX	 8		/* log primitives issued to MX provider */
#define CHLOGTE	 9		/* log CH timer events */
#define MXLOGTE	10		/* log MX timer events */
#define CHLOGDA	11		/* log CH data */
#define MXLOGDA	12		/* log MX data */

#include <sys/chi.h>
#include <sys/mxi.h>

#define CH_DESCRIP	"UNIX SYSTEM V RELEASE 4.2 FAST STREAMS FOR LINUX"
#define CH_COPYRIGHT	"Copyright (c) 1997-2006  OpenSS7 Corporation.  All Rights Reserved."
#define CH_REVISION	"OpenSS7 $RCSfile: ch.c,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2007/07/14 01:35:37 $"
#define CH_DEVICE	"SVR 4.2 STREAMS Channel Module (CH)"
#define CH_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define CH_LICENSE	"GPL v2"
#define CH_BANNER	CH_DESCRIP	"\n" \
			CH_COPYRIGHT	"\n" \
			CH_REVISION	"\n" \
			CH_DEVICE	"\n" \
			CH_CONTACT
#define CH_SPLASH	CH_DEVICE	" - " \
			CH_REVISION

#ifdef LINUX
MODULE_AUTHOR(CH_CONTACT);
MODULE_DESCRIPTION(CH_DESCRIP);
MODULE_SUPPORTED_DEVICE(CH_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(CH_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-ch");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#ifndef CH_MOD_NAME
#define CH_MOD_NAME		"ch"
#endif

#ifndef CH_MOD_ID
#define CH_MOD_ID		0
#endif

#ifndef CH_DRV_NAME
#define CH_DRV_NAME		"ch"
#endif

#ifndef CH_DRV_ID
#define CH_DRV_ID		0
#endif

/*
 *  STREAMS Definitions
 */

#define MOD_ID		CH_MOD_ID
#define MOD_NAME	CH_MOD_NAME
#define DRV_ID		CH_DRV_ID
#define DRV_NAME	CH_DRV_NAME

#ifdef MODULE
#define MOD_BANNER	CH_BANNER
#define DRV_BANNER	CH_BANNER
#else				/* MODULE */
#define MOD_BANNER	CH_SPLASH
#define DRV_BANNER	CH_SPLASH
#endif				/* MODULE */

static modID_t modid = MOD_ID;

struct ch {
	bcid_t bid;			/* buffer callback for write side */
	struct CH_info_ack info;	/* information */
	uint32_t addrs;			/* addresses */
	struct CH_parms_circuit parms;	/* parmaeters */
	struct ch_config config;	/* configuration */
	struct ch_stats stats;		/* statistics */
	struct ch_statem state;		/* state machine */
	struct ch_notify notify;	/* notification events */
};

#if 0
/* The slotmap maps timeslot numbers to slot numbers.  For the V40XP card, the slot maps are as
 * follows: */
/* slots for timeslots 0 through 95 */
static const uint16_t v40xp_t1_slotmap[] = {
	  1,   2,   3,   5,   6,   7,   9,  10,  11,  13,  14,  15,
	 17,  18,  19,  21,  22,  23,  25,  26,  27,  29,  30,  31,
	 33,  34,  35,  37,  38,  39,  41,  42,  43,  45,  46,  47,
	 49,  50,  51,  53,  54,  55,  57,  58,  59,  61,  62,  63,
	 65,  66,  67,  69,  70,  71,  73,  74,  75,  77,  78,  79,
	 81,  82,  83,  85,  86,  87,  89,  90,  91,  93,  94,  95,
	 97,  98,  99, 101, 102, 103, 105, 106, 107, 109, 110, 111,
	113, 114, 115, 117, 118, 119, 121, 122, 123, 125, 126, 127
};
/* better as a bit mask 0xeeeeeeee 0xeeeeeeee 0xeeeeeeee 0xeeeeeeee */
/* slots for timeslots 0 through 123 */
static const uint16_t v40xp_e1_slotmap[] = {
	  1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
	 16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  27,  28,  29,  30,  31,
	 33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
	 48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
	 65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
	 80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,  91,  92,  93,  94,  95,
	 97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
	112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127
};
/* better as a bit mask 0xfffffffe 0xfffffffe 0xfffffffe 0xfffffffe */
/* The chanmap maps sample numbers and slot numbers to an offset within the bit block.  For the
 * V40XP card, the channel maps are a span interleaved map as follows: */
/* better as a span offset and increment 0 [4], 1 [4], 2 [4], 3 [4], or even base, span increment
 * and slot increment 0 [1] [4] */
static const uint16_t v40xp_chanmap[] = {
	   0,   32,   64,   96,   1,   33,   65,   97,   2,   34,   66,   98,   3,   35,   67,   99,
	   4,   36,   68,  100,   5,   37,   69,  101,   6,   38,   70,  102,   7,   39,   71,  103,
	   8,   40,   72,  104,   9,   41,   73,  105,  10,   42,   74,  106,  11,   43,   75,  107,
	  12,   44,   76,  108,  13,   45,   77,  109,  14,   46,   78,  110,  15,   47,   79,  111,
	  16,   48,   80,  112,  17,   49,   81,  113,  18,   50,   82,  114,  19,   51,   83,  115,
	  20,   52,   84,  116,  21,   53,   85,  117,  22,   54,   86,  118,  23,   55,   87,  119,
	  24,   56,   88,  120,  25,   57,   89,  121,  26,   58,   90,  122,  27,   59,   91,  123,
	  28,   60,   92,  124,  29,   61,   93,  125,  30,   62,   94,  126,  31,   63,   95,  127,
	 128,  160,  192,  224, 129,  161,  193,  225, 130,  162,  194,  226, 131,  163,  195,  227,
	 132,  164,  196,  228, 133,  165,  197,  229, 134,  166,  198,  230, 135,  167,  199,  231,
	 136,  168,  200,  232, 137,  169,  201,  233, 138,  170,  202,  234, 139,  171,  203,  235,
	 140,  172,  204,  236, 141,  173,  205,  237, 142,  174,  206,  238, 143,  175,  207,  239,
	 144,  176,  208,  240, 145,  177,  209,  241, 146,  178,  210,  242, 147,  179,  211,  243,
	 148,  180,  212,  244, 149,  181,  213,  245, 150,  182,  214,  246, 151,  183,  215,  247,
	 152,  184,  216,  248, 153,  185,  217,  249, 154,  186,  218,  250, 155,  187,  219,  251,
	 156,  188,  220,  252, 157,  189,  221,  253, 158,  190,  222,  254, 159,  191,  223,  255,
	 256,  288,  320,  352, 257,  289,  321,  353, 258,  290,  322,  354, 259,  291,  323,  355,
	 260,  292,  324,  356, 261,  293,  325,  357, 262,  294,  326,  358, 263,  295,  327,  359,
	 264,  296,  328,  360, 265,  297,  329,  361, 266,  298,  330,  362, 267,  299,  331,  363,
	 268,  300,  332,  364, 269,  301,  333,  365, 270,  302,  334,  366, 271,  303,  335,  367,
	 272,  304,  336,  368, 273,  305,  337,  369, 274,  306,  338,  370, 275,  307,  339,  371,
	 276,  308,  340,  372, 277,  309,  341,  373, 278,  310,  342,  374, 279,  311,  343,  375,
	 280,  312,  344,  376, 281,  313,  345,  377, 282,  314,  346,  378, 283,  315,  347,  379,
	 284,  316,  348,  380, 285,  317,  349,  381, 286,  318,  350,  382, 287,  319,  351,  383,
	 384,  416,  448,  480, 385,  417,  449,  481, 386,  418,  450,  482, 387,  419,  451,  483,
	 388,  420,  452,  484, 389,  421,  453,  485, 390,  422,  454,  486, 391,  423,  455,  487,
	 392,  424,  456,  488, 393,  425,  457,  489, 394,  426,  458,  490, 395,  427,  459,  491,
	 396,  428,  460,  492, 397,  429,  461,  493, 398,  430,  462,  494, 399,  431,  463,  495,
	 400,  432,  464,  496, 401,  433,  465,  497, 402,  434,  466,  498, 403,  435,  467,  499,
	 404,  436,  468,  500, 405,  437,  469,  501, 406,  438,  470,  502, 407,  439,  471,  503,
	 408,  440,  472,  504, 409,  441,  473,  505, 410,  442,  474,  506, 411,  443,  475,  507,
	 412,  444,  476,  508, 413,  445,  477,  509, 414,  446,  478,  510, 415,  447,  479,  511,
	 512,  544,  576,  608, 513,  545,  577,  609, 514,  546,  578,  610, 515,  547,  579,  611,
	 516,  548,  580,  612, 517,  549,  581,  613, 518,  550,  582,  614, 519,  551,  583,  615,
	 520,  552,  584,  616, 521,  553,  585,  617, 522,  554,  586,  618, 523,  555,  587,  619,
	 524,  556,  588,  620, 525,  557,  589,  621, 526,  558,  590,  622, 527,  559,  591,  623,
	 528,  560,  592,  624, 529,  561,  593,  625, 530,  562,  594,  626, 531,  563,  595,  627,
	 532,  564,  596,  628, 533,  565,  597,  629, 534,  566,  598,  630, 535,  567,  599,  631,
	 536,  568,  600,  632, 537,  569,  601,  633, 538,  570,  602,  634, 539,  571,  603,  635,
	 540,  572,  604,  636, 541,  573,  605,  637, 542,  574,  606,  638, 543,  575,  607,  639,
	 640,  672,  704,  736, 641,  673,  705,  737, 642,  674,  706,  738, 643,  675,  707,  739,
	 644,  676,  708,  740, 645,  677,  709,  741, 646,  678,  710,  742, 647,  679,  711,  743,
	 648,  680,  712,  744, 649,  681,  713,  745, 650,  682,  714,  746, 651,  683,  715,  747,
	 652,  684,  716,  748, 653,  685,  717,  749, 654,  686,  718,  750, 655,  687,  719,  751,
	 656,  688,  720,  752, 657,  689,  721,  753, 658,  690,  722,  754, 659,  691,  723,  755,
	 660,  692,  724,  756, 661,  693,  725,  757, 662,  694,  726,  758, 663,  695,  727,  759,
	 664,  696,  728,  760, 665,  697,  729,  761, 666,  698,  730,  762, 667,  699,  731,  763,
	 668,  700,  732,  764, 669,  701,  733,  765, 670,  702,  734,  766, 671,  703,  735,  767,
	 768,  800,  832,  864, 769,  801,  833,  865, 770,  802,  834,  866, 771,  803,  835,  867,
	 772,  804,  836,  868, 773,  805,  837,  869, 774,  806,  838,  870, 775,  807,  839,  871,
	 776,  808,  840,  872, 777,  809,  841,  873, 778,  810,  842,  874, 779,  811,  843,  875,
	 780,  812,  844,  876, 781,  813,  845,  877, 782,  814,  846,  878, 783,  815,  847,  879,
	 784,  816,  848,  880, 785,  817,  849,  881, 786,  818,  850,  882, 787,  819,  851,  883,
	 788,  820,  852,  884, 789,  821,  853,  885, 790,  822,  854,  886, 791,  823,  855,  887,
	 792,  824,  856,  888, 793,  825,  857,  889, 794,  826,  858,  890, 795,  827,  859,  891,
	 796,  828,  860,  892, 797,  829,  861,  893, 798,  830,  862,  894, 799,  831,  863,  895,
	 896,  928,  960,  992, 897,  929,  961,  993, 898,  930,  962,  994, 899,  931,  963,  995,
	 900,  932,  964,  996, 901,  933,  965,  997, 902,  934,  966,  998, 903,  935,  967,  999,
	 904,  936,  968, 1000, 905,  937,  969, 1001, 906,  938,  970, 1002, 907,  939,  971, 1003,
	 908,  940,  972, 1004, 909,  941,  973, 1005, 910,  942,  974, 1006, 911,  943,  975, 1007,
	 912,  944,  976, 1008, 913,  945,  977, 1009, 914,  946,  978, 1010, 915,  947,  979, 1011,
	 916,  948,  980, 1012, 917,  949,  981, 1013, 918,  950,  982, 1014, 919,  951,  983, 1015,
	 920,  952,  984, 1016, 921,  953,  985, 1017, 922,  954,  986, 1018, 923,  955,  987, 1019,
	 924,  956,  988, 1020, 925,  957,  989, 1021, 926,  958,  990, 1022, 927,  959,  991, 1023,
};
#endif

/*
 *  MX Addresses:  CH and MX addresses used for attach are identical except in the behavior of more
 *  specific components.  The ppa (Physical Point of Attachment) designator is a 32-bit number
 *  broken into 4 eight-bit fields as follows:
 *
 *    +---------------+---------------+---------------+---------------+
 *    |   Card Type   |  Card Number  |  Span Number  |Timeslot Number|
 *    +---------------+---------------+---------------+---------------+
 *
 *  The "Card Type" identifies the supported cards under a single driver.  The Tormenta card type is
 *  supported by the V40XP driver and supports the V400P (Tormenta II) and V401P (Tormenta III)
 *  cards.  0x00 means all Card Types, 0xff means masked Card Types.  The "Card Number" is the
 *  instance of a card in the system supported under the driver.  0x00 means all cards, 0xff means
 *  masked cards. (Card masks are not currenlty supported.)  0x01 through 0x7f are card instances 1
 *  through 127.  "Span Number" is a span on the card(s).  0x00 means all spans, 0xff means masked
 *  spans.  0x01 through 0x7f are spans 1 through 127.  Tormenta II and Tormenta III cards only
 *  support 4 spans, 1-4.  "Timeslot Number" is a timeslot number within the selected spans.  0x00
 *  means all timeslots, 0xff means masked time slots.  0x00 through 0x7f select time slot 1 through
 *  127.  T1/J1 spans have 24 timeslots numbered 1 through 24.  E1 spans have 31 timeslots numbered
 *  1 through 31.  T3's are addressed by the T1 tributary.  The low order 2 bits of the span number
 *  is the tributary T1 number.  T3's has tributarys 1 through 28.  The high order 3 bits of the
 *  span number are the DS3 span.  Cards can have DS3 spans 1 through 63.
 *
 *     3                   2                   1                   0
 *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |   Card Type   |  Card Number  |     Span Number     | TS Numb |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |   Card Type   |  Card Number  |  OC3  | E3  |E2 |E1 |   TS    |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *  |   Card Type   |  Card Number  |  OC3  |DS3|    T1   |   TS    |
 *  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *  T1 = 24 timeslots			    1ms frame is  192 bytes minimum  256 bytes nominal
 *  E1 = 31 timeslots			    1ms frame is  248 bytes minimum  256 bytes nominal
 *  E2 = 4 x E1s = 124 timeslots	    1ms frame is  992 bytes minimum 1024 bytes nominal
 *  E3 = 4 x E2s = 16 x E1s = 496 timeslots 1ms frame is 3968 bytes minimum 4096 bytes nominal
 *  T3 = 28 x T1s = 672 timeslots	    1ms frame is 5376 bytes minimum 8192 bytes nominal
 *
 *  When a span or timeslot number field is masked, the address is extended by one 32-bit mask for
 *  the span, and one 32-bit mask for timeslots in each span selected.  Therefore, ppa 0x0001ff00,
 *  would be extended by one 32-bit span mask (but no timeslot masks as all timeslots are selected
 *  in each span).  The ppa, 0x000100ff, would be extended by 4 32-bit timeslot masks, one for each
 *  of the possible spans.  A missing timeslot mask is assumed to select all timeslots.  Therefore,
 *  ppa 0x000100ff without any mask extensions is the same as 0x00010000.
 *
 *  When a CH Stream is attached to a PPA, the channels selected by the PPA and any span and channel
 *  masks, are statistically multiplexed together in ascending span and channel order.  Non-
 *  ascending orders are not currently supported.  For a 56 or 64 kbps channel, the PPA is specific
 *  down to the timeslot.  For example, ppa 0x03010218 selects timeslot 24, of span 2, of card 1, of
 *  type Tormenta III.  For a full 1.544 or 2.048 Mbps channel, the PPA is specific only to the
 *  span.  For example, ppa 0x03010200 selects all timeslots of span 2 of card 1 of type Tormenta
 *  III.  After attaching, the sample rate of the channel indicates whether this is a 1.544 or 2.048
 *  Mbps channel.   The foregoing examples are useful for SS7 (which is either a single channel or a
 *  full span).  For fractional T1 (e.g. X.25) applications, or multiple T1 (e.g. Frame Relay, or
 *  GSM Abis) applications, specific timeslots can be specified with channel masks.  For example,
 *  PPA 0x030103ff, mask 0x00000fff, selects timeslots 1 - 12 of span 3 of card 1 of type Tormenta
 *  III.  mask 0x7fff7fff would select all timeslots 1-15 and 16-31 of an E1.  For channels
 *  statistically multiplexed across multiple spans on the same card, PPA 0x03010000 will multiplex
 *  across all timeslots in all (4) spans of card 1.
 *
 *  When an MX Streams can be attached to the same PPA as a CH Stream, it is only the interpretation
 *  that is different.  If any timeslot is selected in any span, that span is enabled when the MX
 *  stream is enabled.  Also, timeslot data for all active timeslots for all spans is delivered to
 *  each MX stream attached to the same card number.  With CH Streams, only those channels that are
 *  selected are statistically multiplexed into the data flow for the CH stream.
 */
static const uint16_t
struct mx {
	bcid_t bid;			/* buffer callback for read side */
	struct MX_info_ack info;	/* information */
	uint32_t addrs;			/* addresses */
	uint32_t smask;			/* Span mask, 4 bits now, 8 later. */
	uint32_t cmask[8];		/* 4 for now, 8 for later */
	struct MX_parms_circuit parms;	/* parameters */
	int spans;			/* Number of spans in span mask (up to 8) */
	int channels;			/* Number of channels in channels masks. */
//	int mapsize;			/* number of channel map entries */
//	uint16_t *chanmap;		/* channel map */
//	int slots;
//	uint16_t *slotmap;		/* slot map */
	struct mx_config config;	/* configuration */
	struct mx_stats stats;		/* statistics */
	struct mx_statem state;		/* state machine */
	struct mx_notify notify;	/* notification events */
};

struct priv {
	int sid;			/* stream identifier */
	int flags;			/* flag word */
	struct ch ch;			/* CH upper half */
	struct mx mx;			/* MX lower half */
};

#define PRIV(__q)	((struct priv *)(__q)->q_ptr)
#define CH_PRIV(__q)	(&PRIV(__q)->ch)
#define MX_PRIV(__q)	(&PRIV(__q)->mx)

#define CHMX(__ch)	((struct mx *)(&(__ch) + 1));
#define MXCH(__mx)	((struct ch *)(__mx) - 1);

#define PRIVCH(__p)	(&(__p)->ch)
#define PRIVMX(__p)	(&(__p)->mx)
#define CHPRIV(__ch)	((struct priv *)(__ch))
#define MXPRIV(__mx)	((struct priv *)MXCH(__mx))

static __unlikely void
priv_init(struct priv *priv, int sid)
{
	bzero(priv, sizeof(*priv));
	priv->sid = sid;
	{
		struct ch *ch = PRIVCH(priv);

		ch->info.primitive = CH_INFO_ACK;
		ch->info.ch_addr_length = 0;
		ch->info.ch_addr_offset = 0;
		ch->info.ch_parm_length = 0;
		ch->info.ch_parm_offset = 0;
		ch->info.ch_prov_flags = 0;
		ch->info.ch_prov_class = CH_CIRCUIT;
		ch->info.ch_style = CH_STYLE1;
		ch->info.ch_version = CH_VERSION;
		ch->info.ch_state = CHS_UNUSABLE;

		ch->parms.cp_type = CH_PARMS_CIRCUIT;
		ch->parms.cp_encoding = CH_ENCODING_NONE;
		ch->parms.cp_block_size = 64;
		ch->parms.cp_samples = 8;
		ch->parms.cp_sample_size = 8;
		ch->parms.cp_rate = 8000;
		ch->parms.cp_tx_channels = 1;
		ch->parms.cp_rx_channels = 1;
		ch->parms.cp_opt_flags = 0;
	}
	{
		struct mx *mx = PRIVMX(priv);

		mx->info.mx_primitive = 0;	/* indicates no ack yet */
		mx->info.mx_addr_length = 0;
		mx->info.mx_addr_offset = 0;
		mx->info.mx_parm_length = 0;
		mx->info.mx_parm_offset = 0;
		mx->info.mx_prov_flags = 0;
		mx->info.mx_prov_class = MX_CIRCUIT;
		mx->info.mx_style = MX_STYLE1;
		mx->info.mx_version = MX_VERSION;
		mx->info.mx_state = MXS_UNUSABLE;

		mx->parms.mp_encoding = CH_ENCODING_NONE;
		mx->parms.mp_block_size = 2048;
		mx->parms.mp_samples = 8;
		mx->parms.mp_sample_size = 8;
		mx->parms.mp_rate = 8000;
		mx->parms.mp_tx_channels = 31;
		mx->parms.mp_rx_channels = 31;
		mx->parms.mp_opt_flags = 0;
	}
}

/*
 *  Handle buffer callbacks
 */
STATIC streamscall __unlikely void
cx_qenable(long data)
{
	queue_t *q = (queue_t *) data;
	bcid_t *bidp;

	bidp = (q->q_flag & QREADR) ? &MX_PRIV(q)->bid : &CH_PRIV(q)->bid;
	unbufcall(xchg(bidp, 0));
	qenable(q);
}

STATIC mblk_t *
cx_allocb(queue_t *q, size_t size, int pri)
{
	mblk_t *mp;

	if (unlikely(!(mp = allocb(size, pri)))) {
		bcid_t *bidp = (q->q_flag & QREADR) ? &MX_PRIV(q)->bid : &CH_PRIV(q)->bid;
		bcid_t bid = bufcall(size, pri, cx_qenable, (long) q);

		if (likely(bid))
			unbufcall(xchg(bidp, bid));
		else if (!*bidp)
			qenable(q);	/* try again later */
	}
	return (mp);
}

/*
 *  CH State handling.
 *  ------------------
 */
static const char *
ch_state_string(ch_ulong state)
{
	switch (state) {
	case CHS_UNINIT:
		return ("CHS_UNINIT");
	case CHS_UNUSABLE:
		return ("CHS_UNUSABLE");
	case CHS_DETACHED:
		return ("CHS_DETACHED");
	case CHS_WACK_AREQ:
		return ("CHS_WACK_AREQ");
	case CHS_WACK_UREQ:
		return ("CHS_WACK_UREQ");
	case CHS_ATTACHED:
		return ("CHS_ATTACHED");
	case CHS_WACK_EREQ:
		return ("CHS_WACK_EREQ");
	case CHS_WCON_EREQ:
		return ("CHS_WCON_EREQ");
	case CHS_WACK_RREQ:
		return ("CHS_WACK_RREQ");
	case CHS_WCON_RREQ:
		return ("CHS_WCON_RREQ");
	case CHS_ENABLED:
		return ("CHS_ENABLED");
	case CHS_WACK_CREQ:
		return ("CHS_WACK_CREQ");
	case CHS_WCON_CREQ:
		return ("CHS_WCON_CREQ");
	case CHS_WACK_DREQ:
		return ("CHS_WACK_DREQ");
	case CHS_WCON_DREQ:
		return ("CHS_WCON_DREQ");
	case CHS_CONNECTED:
		return ("CHS_CONNECTED");
	default:
		swerr();
		return ("CHS_????");
	}
}

static inline fastcall ch_ulong
ch_get_state(struct ch *ch)
{
	return ch->info.ch_state;
}

static inline fastcall uint
ch_get_statef(struct ch *ch)
{
	return (1 << (2 + ch_get_state(ch)));
}

static ch_ulong
ch_set_state(struct ch *ch, ch_ulong newstate)
{
	ch_ulong oldstate = ch_get_state(ch);

	if (newstate != oldstate) {
		/* state changes are at trace level 1 */
		STRLOG(modid, CHPRIV(ch)->sid, CHLOGST, SL_TRACE, "%s -> %s",
		       ch_state_string(oldstate), ch_state_string(newstate));
		ch->info.ch_state = newstate;
	}
	return (newstate);
}

/*
 *  MX State handling.
 *  ------------------
 */
static const char *
mx_state_string(mx_ulong state)
{
	switch (state) {
	case MXS_UNINIT:
		return ("MXS_UNINIT");
	case MXS_UNUSABLE:
		return ("MXS_UNUSABLE");
	case MXS_DETACHED:
		return ("MXS_DETACHED");
	case MXS_WACK_AREQ:
		return ("MXS_WACK_AREQ");
	case MXS_WACK_UREQ:
		return ("MXS_WACK_UREQ");
	case MXS_ATTACHED:
		return ("MXS_ATTACHED");
	case MXS_WACK_EREQ:
		return ("MXS_WACK_EREQ");
	case MXS_WCON_EREQ:
		return ("MXS_WCON_EREQ");
	case MXS_WACK_RREQ:
		return ("MXS_WACK_RREQ");
	case MXS_WCON_RREQ:
		return ("MXS_WCON_RREQ");
	case MXS_ENABLED:
		return ("MXS_ENABLED");
	case MXS_WACK_CREQ:
		return ("MXS_WACK_CREQ");
	case MXS_WCON_CREQ:
		return ("MXS_WCON_CREQ");
	case MXS_WACK_DREQ:
		return ("MXS_WACK_DREQ");
	case MXS_WCON_DREQ:
		return ("MXS_WCON_DREQ");
	case MXS_CONNECTED:
		return ("MXS_CONNECTED");
	default:
		swerr();
		return ("MXS_????");
	}
}

static inline fastcall mx_ulong
mx_get_state(struct mx *mx)
{
	return mx->info.mx_state;
}

static inline fastcall uint
mx_get_statef(struct mx *mx)
{
	return (1 << (2 + mx_get_state(mx)));
}

static mx_ulong
mx_set_state(struct mx *mx, mx_ulong newstate)
{
	mx_ulong oldstate = mx_get_state(mx);

	/* state changes are at trace level 1 */
	STRLOG(modid, MXPRIV(mx)->sid, MXLOGST, SL_TRACE, "%s -> %s", mx_state_string(oldstate),
	       mx_state_string(newstate));
	mx->info.mx_state = newstate;
	return (newstate);
}

/*
 *  CH Primitives issued up to CH user.
 *  -----------------------------------
 */

/**
 * ch_info_ack: issue CH_INFO_ACK message
 * @ch: private structure
 * @q: active queue (write queue)
 * @msg: message to free on success
 */
STATIC __unlikely int
ch_info_ack(struct ch *ch, queue_t *q, mblk_t *msg)
{
	size_t alen = ch->info.ch_addr_length;
	size_t plen = ch->info.ch_parm_length;
	caddr_t aptr = (caddr_t) &ch->addrs;
	caddr_t pptr = (caddr_t) &ch->parms;

	struct CH_info_ack *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p) + alen + plen, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		*p = ch->info;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		bcopy(pptr, mp->b_wptr, plen);
		mp->b_wptr += plen;
		STRLOG(modid, PRIV(q)->sid, CHLOGTX, SL_TRACE, "<- CH_INFO_ACK");
		freemsg(msg);
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_optmgmt_ack: issue CH_OPTMGMT_ACK message
 * @ch: private structure
 * @q: active queue (write queue)
 * @msg: message to free on success
 * @flags: management flags
 * @olen: options length
 * @optr: options pointer
 */
STATIC __unlikely int
ch_optmgmt_ack(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, size_t olen, caddr_t optr)
{
	struct CH_optmgmt_ack *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p) + olen, BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_OPTMGMT_ACK;
		p->ch_opt_length = olen;
		p->ch_opt_offset = olen ? sizeof(*p) : 0;
		p->ch_mgmt_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(optr, mp->b_wptr, olen);
		mp->b_wptr += olen;
		STRLOG(modid, PRIV(q)->sid, CHLOGTX, SL_TRACE, "<- CH_OPTMGMT_ACK");
		freemsg(msg);
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_ok_ack: issue CH_OK_ACK message
 * @ch: private structure
 * @q: active queue (write queue)
 * @msg: message to free on success
 * @prim: correct primitive
 * @state: resulting state
 */
STATIC __unlikely int
ch_ok_ack(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong prim, ch_ulong state)
{
	struct CH_ok_ack *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_OK_ACK;
		p->ch_correct_prim = prim;
		p->ch_state = state;
		mp->b_wptr += sizeof(*p);
		ch_set_state(ch, state);
		STRLOG(modid, PRIV(q)->sid, CHLOGTX, SL_TRACE, "<- CH_OK_ACK");
		freemsg(msg);
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_error_ack: issue CH_ERROR_ACK message
 * @ch: private structure
 * @q: active queue (write queue)
 * @msg: message to free on success
 * @prim: primitive in error
 * @state: resulting state
 * @type: error type
 * @error: UNIX error
 */
STATIC __unlikely int
ch_error_ack(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong prim, ch_ulong state, ch_ulong type,
	     ch_ulong error)
{
	struct CH_error_ack *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_ERROR_ACK;
		p->ch_error_primitive = prim;
		p->ch_error_type = type;
		p->ch_unix_error = error;
		p->ch_state = state;
		mp->b_wptr += sizeof(*p);
		ch_set_state(ch, state);
		STRLOG(modid, PRIV(q)->sid, CHLOGTX, SL_TRACE, "<- CH_ERROR_ACK");
		freemsg(msg);
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_enable_con: issue CH_ENABLE_CON message
 * @ch: private structure
 * @q: active queue (read queue)
 * @msg: message to free on success
 */
STATIC __unlikely int
ch_enable_con(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_enable_con *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_ENABLE_CON;
		mp->b_wptr += sizeof(*p);
		ch_set_state(ch, CHS_ENABLED);
		STRLOG(modid, PRIV(q)->sid, CHLOGTX, SL_TRACE, "<- CH_ENABLE_CON");
		freemsg(msg);
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_connect_con: issue CH_CONNECT_CON message
 * @ch: private structure
 * @q: active queue (read queue)
 * @msg: message to free on success
 * @flags: connect flags
 * @slot: connect slot
 */
STATIC __unlikely int
ch_connect_con(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, ch_ulong slot)
{
	struct CH_connect_con *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_CONNECT_CON;
		p->ch_conn_flags = flags;
		p->ch_slot = slot;
		mp->b_wptr += sizeof(*p);
		ch_set_state(ch, CHS_CONNECTED);
		STRLOG(modid, PRIV(q)->sid, CHLOGTX, SL_TRACE, "<- CH_CONNECT_CON");
		freemsg(msg);
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_data_ind: issue CH_DATA_IND message
 * @ch: private structure
 * @q: active queue (read queue)
 * @msg: message to free on success (plus data)
 * @slot: slot within channel
 */
STATIC __unlikely int
ch_data_ind(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong slot)
{
	struct CH_data_ind *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DATA_IND;
		p->ch_slot = slot;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = XCHG(&msg->b_cont, NULL);
		STRLOG(modid, PRIV(q)->sid, CHLOGDA, SL_TRACE, "<- CH_DATA_IND");
		freemsg(msg);
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_disconnect_ind: issue CH_DISCONNECT_IND message
 * @ch: private structure
 * @q: active queue (read queue)
 * @msg: message to free on success
 * @flags: connect flags
 * @slot: connect slot
 * @cause: disconnect cause
 */
STATIC __unlikely int
ch_disconnect_ind(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, ch_ulong slot,
		  ch_ulong cause)
{
	struct CH_disconnect_ind *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DISCONNECT_IND;
		p->ch_conn_flags = flags;
		p->ch_slot = slot;
		p->ch_cause = cause;
		mp->b_wptr += sizeof(*p);
		STRLOG(modid, PRIV(q)->sid, CHLOGTX, SL_TRACE, "<- CH_DISCONNECT_IND");
		freemsg(msg);
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_disconnect_con: issue CH_DISCONNECT_CON message
 * @ch: private structure
 * @q: active queue (read queue)
 * @msg: message to free on success
 * @flags: connect flags
 * @slot: connect slot
 */
STATIC __unlikely int
ch_disconnect_con(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong flags, ch_ulong slot)
{
	struct CH_disconnect_con *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DISCONNECT_CON;
		p->ch_conn_flags = flags;
		p->ch_slot = slot;
		mp->b_wptr += sizeof(*p);
		ch_set_state(ch, CHS_ENABLED);
		STRLOG(modid, PRIV(q)->sid, CHLOGTX, SL_TRACE, "<- CH_DISCONNECT_CON");
		freemsg(msg);
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_disable_ind: issue CH_DISABLE_IND message
 * @ch: private structure
 * @q: active queue (read queue)
 * @msg: message to free on success
 * @cause: disable cause
 */
STATIC __unlikely int
ch_disable_ind(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong cause)
{
	struct CH_disable_ind *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DISABLE_IND;
		p->ch_cause = cause;
		mp->b_wptr += sizeof(*p);
		ch_set_state(ch, CHS_ATTACHED);
		STRLOG(modid, PRIV(q)->sid, CHLOGTX, SL_TRACE, "<- CH_DISABLE_IND");
		freemsg(msg);
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_disable_con: issue CH_DISABLE_CON message
 * @ch: private structure
 * @q: active queue (read queue)
 * @msg: message to free on success
 */
STATIC __unlikely int
ch_disable_con(struct ch *ch, queue_t *q, mblk_t *msg)
{
	struct CH_disable_con *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_DISABLE_CON;
		mp->b_wptr += sizeof(*p);
		ch_set_state(ch, CHS_ATTACHED);
		STRLOG(modid, PRIV(q)->sid, CHLOGTX, SL_TRACE, "<- CH_DISABLE_CON");
		freemsg(msg);
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * ch_event_ind: issue CH_EVENT_IND message
 * @ch: private structure
 * @q: active queue (read queue)
 * @msg: message to free on success
 * @event: event indicated
 * @slot: slot for event
 */
STATIC __unlikely int
ch_event_ind(struct ch *ch, queue_t *q, mblk_t *msg, ch_ulong event, ch_ulong slot)
{
	struct CH_event_ind *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->ch_primitive = CH_EVENT_IND;
		p->ch_event = event;
		p->ch_slot = slot;
		mp->b_wptr += sizeof(*p);
		STRLOG(modid, PRIV(q)->sid, CHLOGTX, SL_TRACE, "<- CH_EVENT_IND");
		freemsg(msg);
		putnext(RD(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  MX Primitives issued down to MX provider.
 *  -----------------------------------------
 */

/**
 * mx_info_req: issue MX_INFO_REQ message
 * @mx: private structure
 * @q: active queue (write queue)
 * @msg: message to free on success
 */
STATIC __unlikely int
mx_info_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_info_req *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PCPROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_INFO_REQ;
		mp->b_wptr += sizeof(*p);
		STRLOG(modid, PRIV(q)->sid, MXLOGTX, SL_TRACE, "MX_INFO_REQ ->");
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_optmgmt_req: issue MX_OPTMGMT_REQ message
 * @mx: private structure
 * @q: active queue (write queue)
 * @msg: message to free on success
 */
STATIC __unlikely int
mx_optmgmt_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_optmgmt_req *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_OPTMGMT_REQ;
		mp->b_wptr += sizeof(*p);
		STRLOG(modid, PRIV(q)->sid, MXLOGTX, SL_TRACE, "MX_INFO_REQ ->");
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_attach_req: issue MX_ATTACH_REQ message
 * @mx: private structure
 * @q: active queue (write queue)
 * @msg: message to free on success
 * @flags: attach flags
 * @alen: address length
 * @aptr: address pointer
 */
STATIC __unlikely int
mx_attach_req(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong flags, size_t alen, caddr_t aptr)
{
	struct MX_attach_req *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p) + alen, BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_ATTACH_REQ;
		p->mx_addr_length = alen;
		p->mx_addr_offset = alen ? sizeof(*p) : 0;
		p->mx_flags = flags;
		mp->b_wptr += sizeof(*p);
		bcopy(aptr, mp->b_wptr, alen);
		mp->b_wptr += alen;
		STRLOG(modid, PRIV(q)->sid, MXLOGTX, SL_TRACE, "MX_INFO_REQ ->");
		mx_set_state(mx, MXS_WACK_AREQ);
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_enable_req: issue MX_ENABLE_REQ message
 * @mx: private structure
 * @q: active queue (write queue)
 * @msg: message to free on success
 */
STATIC __unlikely int
mx_enable_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_enable_req *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_ENABLE_REQ;
		mp->b_wptr += sizeof(*p);
		STRLOG(modid, PRIV(q)->sid, MXLOGTX, SL_TRACE, "MX_ENABLE_REQ ->");
		mx_set_state(mx, MXS_WACK_EREQ);
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_connect_req: issue MX_CONNECT_REQ message
 * @mx: private structure
 * @q: active queue (write queue)
 * @msg: message to free on success
 * @flags: connection flags
 * @slot: slot within multiplex
 */
STATIC __unlikely int
mx_connect_req(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong flags, mx_ulong slot)
{
	struct MX_connect_req *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_CONNECT_REQ;
		p->mx_conn_flags = flags;
		p->mx_slot = slot;
		mp->b_wptr += sizeof(*p);
		STRLOG(modid, PRIV(q)->sid, MXLOGTX, SL_TRACE, "MX_CONNECT_REQ ->");
		mx_set_state(mx, MXS_WACK_CREQ);
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_data_req: issue MX_DATA_REQ message
 * @mx: private structure
 * @q: active queue (write queue)
 * @msg: message to free on success (also data).
 * @slot: slot for data
 */
STATIC __unlikely int
mx_data_req(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong slot)
{
	struct MX_data_req *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DATA_REQ;
		p->mx_slot = slot;
		mp->b_wptr += sizeof(*p);
		mp->b_cont = XCHG(&msg->b_cont, NULL);
		STRLOG(modid, PRIV(q)->sid, MXLOGDA, SL_TRACE, "MX_DATA_REQ ->");
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_disconnect_req: issue MX_DISCONNECT_REQ message
 * @mx: private structure
 * @q: active queue (write queue)
 * @msg: message to free on success
 */
STATIC __unlikely int
mx_disconnect_req(struct mx *mx, queue_t *q, mblk_t *msg, mx_ulong flags, mx_ulong slot)
{
	struct MX_disconnect_req *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DISCONNECT_REQ;
		p->mx_conn_flags = flags;
		p->mx_slot = slot;
		mp->b_wptr += sizeof(*p);
		STRLOG(modid, PRIV(q)->sid, MXLOGTX, SL_TRACE, "MX_DISCONNECT_REQ ->");
		mx_set_state(mx, MXS_WACK_DREQ);
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_disable_req: issue MX_DISABLE_REQ message
 * @mx: private structure
 * @q: active queue (write queue)
 * @msg: message to free on success
 */
STATIC __unlikely int
mx_disable_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_disable_req *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DISABLE_REQ;
		mp->b_wptr += sizeof(*p);
		STRLOG(modid, PRIV(q)->sid, MXLOGTX, SL_TRACE, "MX_DISABLE_REQ ->");
		mx_set_state(mx, MXS_WACK_RREQ);
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/**
 * mx_detach_req: issue MX_DETACH_REQ message
 * @mx: private structure
 * @q: active queue (write queue)
 * @msg: message to free on success
 */
STATIC __unlikely int
mx_detach_req(struct mx *mx, queue_t *q, mblk_t *msg)
{
	struct MX_detach_req *p;
	mblk_t *mp;

	if ((mp = cx_allocb(q, sizeof(*p), BPRI_MED))) {
		mp->b_datap->db_type = M_PROTO;
		p = (typeof(p)) mp->b_wptr;
		p->mx_primitive = MX_DETACH_REQ;
		mp->b_wptr += sizeof(*p);
		STRLOG(modid, PRIV(q)->sid, MXLOGTX, SL_TRACE, "MX_DETACH_REQ ->");
		mx_set_state(mx, MXS_WACK_UREQ);
		freemsg(msg);
		putnext(WR(q), mp);
		return (0);
	}
	return (-ENOBUFS);
}

/*
 *  CH Primitives received from CH User above.
 *  ------------------------------------------
 */

/**
 * ch_info_req: process CH_INFO_REQ message
 * @ch: private structure
 * @q: active queue (write queue)
 * @mp: CH_INFO_REQ message
 */
STATIC noinline __unlikely int
ch_info_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	return ch_info_ack(ch, q, mp);
}

/**
 * ch_optmgmt_req: process CH_OPTMGMT_REQ message
 * @ch: private structure
 * @q: active queue (write queue)
 * @mp: CH_OPTMGMT_REQ message
 */
STATIC noinline __unlikely int
ch_optmgmt_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	/* There really are no options to manage at the moment. */
	return ch_error_ack(ch, q, mp, CH_OPTMGMT_REQ, ch_get_state(ch), CHNOTSUPP, EOPNOTSUPP);
}

/**
 * ch_attach_req: process CH_ATTACH_REQ message
 * @ch: private structure
 * @q: active queue (write queue)
 * @mp: CH_ATTACH_REQ message
 *
 * If the module has been pushed over an unattached style 2 MX stream, then the user can attach.
 * Otherwise, it is the wrong style.  Use the information returned from an MX_INFO_REQ sent when
 * pushed.  Valid only in state CHS_DETACHED.  If supported, issue an MX_ATTACH_REQ to the provider
 * and await an MX_OK_ACK or MX_ERROR_ACK response.  The address for the CH_ATTACH_REQ is the same
 * as for the MX_ATTACH_REQ.
 */
STATIC noinline __unlikely int
ch_attach_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_attach_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (ch->info.ch_style != CH_STYLE2)
		goto notsupport;
	if (ch_get_state(ch) != CHS_DETACHED)
		goto outstate;
	ch_set_state(ch, CHS_WACK_AREQ);
	return mx_attach_req(CHMX(ch), q, mp);
      outstate:
	return ch_error_ack(ch, q, mp, ch_get_state(ch), CH_ATTACH_REQ, CHOUTSTATE, EPROTO);
      notsupport:
	return ch_error_ack(ch, q, mp, ch_get_state(ch), CH_ATTACH_REQ, CHNOTSUPP, EPROTO);
      badprim:
	return ch_error_ack(ch, q, mp, ch_get_state(ch), CH_ATTACH_REQ, CHBADPRIM, EMSGSIZE);
}

/**
 * ch_enable_req: process CH_ENABLE_REQ message
 * @ch: private structure
 * @q: active queue (write queue)
 * @mp: CH_ENABLE_REQ message
 */
STATIC noinline __unlikely int
ch_enable_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_enable_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (ch_get_state(ch) != CHS_ATTACHED)
		goto outstate;
	ch_set_state(ch, CHS_WACK_EREQ);
	return mx_enable_req(CHMX(ch), q, mp);
      outstate:
	return ch_error_ack(ch, q, mp, ch_get_state(ch), CH_ENABLE_REQ, CHOUTSTATE, EPROTO);
      badprim:
	return ch_error_ack(ch, q, mp, ch_get_state(ch), CH_ENABLE_REQ, CHBADPRIM, EMSGSIZE);
}

/**
 * ch_connect_req: process CH_CONNECT_REQ message
 * @ch: private structure
 * @q: active queue (write queue)
 * @mp: CH_CONNECT_REQ message
 */
STATIC noinline __unlikely int
ch_connect_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_connect_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (ch_get_state(ch) != CHS_ENABLED)
		goto outstate;
	ch_set_state(ch, CHS_WACK_CREQ);
	return mx_connect_req(CHMX(ch), q, mp, p->ch_conn_flags, p->ch_slot);
      outstate:
	return ch_error_ack(ch, q, mp, ch_get_state(ch), CH_CONNECT_REQ, CHOUTSTATE, EPROTO);
      badprim:
	return ch_error_ack(ch, q, mp, ch_get_state(ch), CH_CONNECT_REQ, CHBADPRIM, EMSGSIZE);
}

/**
 * ch_data_req: process CH_DATA_REQ message
 * @ch: private structure
 * @q: active queue (write queue)
 * @mp: CH_DATA_REQ message
 */
STATIC noinline __unlikely int
ch_data_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_data_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (ch_get_state(ch) != CHS_CONNECTED) {
		freemsg(mp);
		return (0);	/* discard */
	}
	return mx_data_req(ch, q, mp, p->ch_slot);
      badprim:
	return ch_error_ack(ch, q, mp, ch_get_state(ch), CH_DATA_REQ, CHBADPRIM, EMSGSIZE);
}

/**
 * ch_disconnect_req: process CH_DISCONNECT_REQ message
 * @ch: private structure
 * @q: active queue (write queue)
 * @mp: CH_DISCONNECT_REQ message
 */
STATIC noinline __unlikely int
ch_disconnect_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disconnect_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (ch_get_state(ch) != CHS_CONNECTED)
		goto outstate;
	ch_set_state(ch, CHS_WACK_DREQ);
	return mx_disconnect_req(CHMX(ch), q, mp, p->ch_conn_flags, p->ch_slot);
      outstate:
	return ch_error_ack(ch, q, mp, ch_get_state(ch), CH_DISCONNECT_REQ, CHOUTSTATE, EPROTO);
      notsupport:
	return ch_error_ack(ch, q, mp, ch_get_state(ch), CH_DISCONNECT_REQ, CHNOTSUPP, EPROTO);
      badprim:
	return ch_error_ack(ch, q, mp, ch_get_state(ch), CH_DISCONNECT_REQ, CHBADPRIM, EMSGSIZE);
}

/**
 * ch_disable_req: process CH_DISABLE_REQ message
 * @ch: private structure
 * @q: active queue (write queue)
 * @mp: CH_DISABLE_REQ message
 */
STATIC noinline __unlikely int
ch_disable_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_disable_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (ch_get_state(ch) != CHS_ENABLED)
		goto outstate;
	ch_set_state(ch, CHS_WACK_RREQ);
	return mx_disable_req(CHMX(ch), q, mp);
      outstate:
	return ch_error_ack(ch, q, mp, ch_get_state(ch), CH_DISABLE_REQ, CHOUTSTATE, EPROTO);
      notsupport:
	return ch_error_ack(ch, q, mp, ch_get_state(ch), CH_DISABLE_REQ, CHNOTSUPP, EPROTO);
      badprim:
	return ch_error_ack(ch, q, mp, ch_get_state(ch), CH_DISABLE_REQ, CHBADPRIM, EMSGSIZE);
}

/**
 * ch_detach_req: process CH_DETACH_REQ message
 * @ch: private structure
 * @q: active queue (write queue)
 * @mp: CH_DETACH_REQ message
 *
 * If the module has been pushed over an unattached style 2 MX stream, then
 * the user can attach and detach.  Otherwise, it is the wrong style.  Use the
 * information returned from an MX_INFO_REQ sent when pushed.  Valid only in
 * state CHS_ATTACHED.  If supported, issue an MX_DETACH_REQ to the provider
 * and await an MX_OK_ACK or MX_ERROR_ACK reponse.
 */
STATIC noinline __unlikely int
ch_detach_req(struct ch *ch, queue_t *q, mblk_t *mp)
{
	struct CH_detach_req *p = (typeof(p)) mp->b_rptr;

	if (mp->b_wptr < mp->b_rptr + sizeof(*p))
		goto badprim;
	if (ch->info.ch_style != CH_STYLE2)
		goto notsupport;
	if (ch_get_state(ch) != CHS_ATTACHED)
		goto outstate;
	ch_set_state(ch, CHS_WACK_UREQ);
	return mx_detach_req(CHMX(ch), q, mp);
      outstate:
	return ch_error_ack(ch, q, mp, ch_get_state(ch), CH_DETACH_REQ, CHOUTSTATE, EPROTO);
      notsupport:
	return ch_error_ack(ch, q, mp, ch_get_state(ch), CH_DETACH_REQ, CHNOTSUPP, EPROTO);
      badprim:
	return ch_error_ack(ch, q, mp, ch_get_state(ch), CH_DETACH_REQ, CHBADPRIM, EMSGSIZE);
}

/*
 *  MX primitives received from MX Provider below.
 *  ----------------------------------------------
 */

/**
 * mx_info_ack: process MX_INFO_ACK message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_INFO_ACK message
 */
STATIC noinline __unlikely int
mx_info_ack(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_info_ack *p = (typeof(p)) mp->b_rptr;

	dassert(mp->b_wptr > mp->b_rptr + sizeof(*p));

	mx->info.mx_prov_flags = p->mx_prov_flags;
	mx->info.mx_prov_class = p->mx_prov_class;
	mx->info.mx_style = p->mx_style;
	mx->info.mx_version = p->mx_version;
	mx_set_state(mx, p->mx_state);

	if (p->mx_addr_length <= sizeof(mx->addrs)) {
		bcopy(mp->b_rptr + p->mx_addr_offset, &mx->addrs, p->mx_addr_length);
		mx->info.mx_addr_length = p->mx_addr_length;
		mx->info.mx_addr_offset = sizeof(mx->info);
	}
	if (p->mx_parm_length == sizeof(mx->parms)
	    && *(mx_ulong *) (mp->b_rptr + p->mx_parm_offset) == mx->parms.mp_type) {
		bcopy(mp->b_rptr + p->mx_parm_offset, &mx->parms, p->mx_parm_length);
		mx->info.mx_parm_length = p->mx_parm_length;
		mx->info.mx_parm_offset = sizeof(mx->info) + mx->info.mx_addr_length;
	}

	if (mx->info.mx_primitive == 0) {
		struct ch *ch = MXCH(mx);

		mx->info.mx_primitive = MX_INFO_ACK;
		/* First MX_INFO_ACK: the first MX_INFO_ACK gives us information about the MX
		   stream over which we have been pushed.  If the MX Stream is a style II Stream
		   and the Stream has not yet been attached, the parameters information (above)
		   will be unknown.  In that case, we must issue another information request after
		   the MX Stream has been attached. */
		switch (mx_get_state(mx)) {
		case MXS_DETACHED:
		case MXS_ENABLED:
			MXCH(mx)->info.ch_style = mx->info.mx_style;
			break;
		case MXS_ATTACHED:
			MXCH(mx)->info.ch_style = CH_STYLE1;
			break;
		}
		ch->info.ch_prov_flags = mx->info.mx_prov_flags;
		ch->info.ch_prov_class = mx->info.mx_prov_class;
		/* TODO: check provider class: it must be MX_CIRCUIT */
	}
	{
		/* Subsequent MX_INFO_ACK: a subsequent request is necessary immediately after an
		   attach if the Stream was not attached during the last request.  Or, at any time
		   that we need to synchronize state or other parameters, a subsequent request can
		   be issued. */
		switch (mx_get_state(mx)) {
		case MXS_DETACHED:
			ch_set_state(MXCH(mx), CHS_DETACHED);
			break;
		case MXS_ATTACHED:
			ch_set_state(MXCH(mx), CHS_ATTACHED);
			/* addr and parms should be valid */
			break;
		case MXS_ENABLED:
			ch_set_state(MXCH(mx), CHS_ENABLED);
			/* addr and parms should be valid */
			break;
		case MXS_CONNECTED:
			ch_set_state(MXCH(mx), CHS_CONNECTED);
			/* addr and parms should be valid */
			break;
		default:
			break;
		}
		switch (mx_get_state(mx)) {
		case MXS_ATTACHED:
		case MXS_ENABLED:
			/* addr and parms should be valid in this state */
			if (mx->info.mx_parm_length = sizeof(mx->parms)) {
				struct ch *ch = MXCH(mx);

				ch->parms.cp_type = CH_PARMS_CIRCUIT;
				ch->parms.cp_encoding = mx->parms.mp_encoding;
				ch->parms.cp_block_size = ((mp->parms.mp_sample_size + 7) & 0x7)
				    * mx->parms.mp_samples;
				ch->parms.cp_samples = mx->parms.mp_samples;
				ch->parms.cp_sample_size = mx->parms.mp_sample_size;
				ch->parms.cp_rate = mx->parms.mp_rx_channels * mx->parms.mp_rate;
				ch->parms.cp_tx_channels = mx->parms.mp_rx_channels ? 1 : 0;
				ch->parms.cp_rx_channels = mx->parms.mp_tx_channels ? 1 : 0;
				/* should record in options flags whether these are bit reversed or 
				   not */
				ch->parms.cp_opt_flags = mx->parms.mx_opt_flags;
				/* Need a channel sample map providing the offset of each sample
				   for each channel within a block.  We do not do this yet, and
				   just assume the V40XP channel mapping. */
				/* Pre-calculate some values used by the RX/TX procedures. */
				mx->offset = mx->info.mp_spans * mx->info.mp_slots;
				mx->mapsize = mx->info.mp_samples * mx->offset;
			}
			break;
		case MXS_DETACHED:
			/* addr and parms are not valid in this state */
			break;
		}
	}
	freemsg(mp);
	return (0);
}

/**
 * mx_optmgmt_ack: process MX_OPTMGMT_ACK message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_OPTMGMT_ACK message
 */
STATIC noinline __unlikely int
mx_optmgmt_ack(struct mx *mx, queue_t *q, mblk_t *mp)
{
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * mx_ok_ack: process MX_OK_ACK message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_OK_ACK message
 */
STATIC noinline __unlikely int
mx_ok_ack(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_ok_ack *p = (typeof(p)) mp->b_rptr;
	mx_ulong state;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));

	state = mx_get_state(mx);
	mx_set_state(mx, p->mx_state);

	switch (state) {
	case MXS_WACK_AREQ:
		return ch_ok_ack(MXCH(mx), q, mp, CH_ATTACH_REQ, CHS_ATTACHED);
	case MXS_WACK_UREQ:
		return ch_ok_ack(MXCH(mx), q, mp, CH_DETACH_REQ, CHS_DETACHED);
	case MXS_WACK_EREQ:
		return ch_ok_ack(MXCH(mx), q, mp, CH_ENABLE_REQ, CHS_WCON_EREQ);
	case MXS_WACK_RREQ:
		return ch_ok_ack(MXCH(mx), q, mp, CH_DISABLE_REQ, CHS_WCON_RREQ);
	case MXS_WACK_CREQ:
		return ch_ok_ack(MXCH(mx), q, mp, CH_CONNECT_REQ, CHS_WCON_CREQ);
	case MXS_WACK_DREQ:
		return ch_ok_ack(MXCH(mx), q, mp, CH_DISCONNECT_REQ, CHS_WCON_DREQ);
	default:
		swerr();
		freemsg(mp);
		return (0);
	}
}

/**
 * mx_error_ack: process MX_ERROR_ACK message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_ERROR_ACK message
 */
STATIC noinline __unlikely int
mx_error_ack(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_ok_ack *p = (typeof(p)) mp->b_rptr;
	mx_ulong state;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));

	state = mx_get_state(mx);
	mx_set_state(mx, p->mx_state);

	switch (state) {
	case MXS_WACK_AREQ:
		return ch_error_ack(MXCH(mx), q, mp, CH_ATTACH_REQ, CHS_DETACHED,
				    p->mx_error_type, p->mx_unix_error);
	case MXS_WACK_UREQ:
		return ch_error_ack(MXCH(mx), q, mp, CH_DETACH_REQ, CHS_ATTACHED,
				    p->mx_error_type, p->mx_unix_error);
	case MXS_WACK_EREQ:
		return ch_error_ack(MXCH(mx), q, mp, CH_ENABLE_REQ, CHS_ATTACHED,
				    p->mx_error_type, p->mx_unix_error);
	case MXS_WACK_RREQ:
		return ch_error_ack(MXCH(mx), q, mp, CH_DISABLE_REQ, CHS_ENABLED,
				    p->mx_error_type, p->mx_unix_error);
	case MXS_WACK_CREQ:
		return ch_error_ack(MXCH(mx), q, mp, CH_CONNECT_REQ, CHS_ENABLED,
				    p->mx_error_type, p->mx_unix_error);
	case MXS_WACK_DREQ:
		return ch_error_ack(MXCH(mx), q, mp, CH_DISCONNECT_REQ, CHS_CONNECTED,
				    p->mx_error_type, p->mx_unix_error);
	}
	swerr();
	freemsg(mp);
	return (0);
}

/**
 * mx_enable_con: process MX_ENABLE_CON message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_ENABLE_CON message
 */
STATIC noinline __unlikely int
mx_enable_con(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_enable_con *p = (typeof(p)) mp->b_rptr;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));
	mx_set_state(mx, MXS_ENABLED);
	return ch_enable_con(MXCH(mx), q, mp);
}

/**
 * mx_connect_con: process MX_CONNECT_CON message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_CONNECT_CON message
 */
STATIC noinline __unlikely int
mx_connect_con(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_connect_con *p = (typeof(p)) mp->b_rptr;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));
	mx_set_state(mx, MXS_CONNECTED);
	return ch_connect_con(MXCH(mx), q, mp, p->mx_conn_flags, p->mx_slot);
}

/**
 * mx_data_ind: process MX_DATA_IND message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_DATA_IND message
 */
STATIC noinline __unlikely int
mx_data_ind(struct mx *mx, queue_t *q, mblk_t *mp)
{
	/* must be discarding */
	freemsg(mp);
	return (0);
}

/**
 * mx_disconnect_ind: process MX_DISCONNECT_IND message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_DISCONNECT_IND message
 */
STATIC noinline __unlikely int
mx_disconnect_ind(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disconnect_ind *p = (typeof(p)) mp->b_rptr;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));
	mx_set_state(mx, MXS_ENABLED);
	return ch_disconnect_ind(MXCH(mx), q, mp, p->mx_conn_flags, p->mx_slot, p->mx_cause);
}

/**
 * mx_disconnect_con: process MX_DISCONNECT_CON message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_DISCONNECT_CON message
 */
STATIC noinline __unlikely int
mx_disconnect_con(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disconnect_con *p = (typeof(p)) mp->b_rptr;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));
	mx_set_state(mx, MXS_ENABLED);
	return ch_disconnect_con(MXCH(mx), q, mp, p->mx_conn_flags, p->mx_slot);
}

/**
 * mx_disable_ind: process MX_DISABLE_IND message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_DISABLE_IND message
 */
STATIC noinline __unlikely int
mx_disable_ind(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disable_ind *p = (typeof(p)) mp->b_rptr;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));
	mx_set_state(mx, MXS_ATTACHED);
	return ch_disable_ind(MXCH(mx), q, mp, p->mx_cause);
}

/**
 * mx_disable_con: process MX_DISABLE_CON message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_DISABLE_CON message
 */
STATIC noinline __unlikely int
mx_disable_con(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_disable_con *p = (typeof(p)) mp->b_rptr;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));
	mx_set_state(mx, MXS_ATTACHED);
	return ch_disable_con(MXCH(mx), q, mp);
}

/**
 * mx_event_ind: process MX_EVENT_IND message
 * @mx: private structure
 * @q: active queue (read queue)
 * @mp: MX_EVENT_IND message
 */
STATIC noinline __unlikely int
mx_event_ind(struct mx *mx, queue_t *q, mblk_t *mp)
{
	struct MX_event_ind *p = (typeof(p)) mp->b_rptr;

	dassert(mp->b_wptr >= mp->b_rptr + sizeof(*p));
	return ch_event_ind(MXCH(mx), q, mp, p->mx_event, p->mx_slot);
}

/**
 * ch_w_msg: - process MX message other than M_DATA
 * @ch: private structure
 * @q: active queue (read queue)
 * @mp: the message to process
 */
STATIC noinline fastcall __unlikely int
ch_w_msg(struct ch *ch, queue_t *q, mblk_t *mp)
{
	switch (DB_TYPE(mp)) {
	case M_PROTO:
	case M_PCPROTO:
	{
		ch_ulong oldstate = ch_get_state(ch);
		struct priv *priv;
		int err;

		dassert(mp->b_wptr >= mp->b_rptr + sizeof(ch_ulong));

		priv = CHPRIV(ch);
		if (!test_and_set_bit(0, &priv->flags)) {
			switch (*(ch_ulong *) mp->b_rptr) {
			case CH_INFO_REQ:
				STRLOG(mid, priv->sid, CHLOGRX, SL_TRACE, "-> CH_INFO_REQ");
				err = ch_info_req(ch, q, mp);
				break;
			case CH_OPTMGMT_REQ:
				STRLOG(mid, priv->sid, CHLOGRX, SL_TRACE, "-> CH_OPTMGMT_REQ");
				err = ch_optmgmt_req(ch, q, mp);
				break;
			case CH_ATTACH_REQ:
				STRLOG(mid, priv->sid, CHLOGRX, SL_TRACE, "-> CH_ATTACH_REQ");
				err = ch_attach_req(ch, q, mp);
				break;
			case CH_ENABLE_REQ:
				STRLOG(mid, priv->sid, CHLOGRX, SL_TRACE, "-> CH_ENABLE_REQ");
				err = ch_enable_req(ch, q, mp);
				break;
			case CH_CONNECT_REQ:
				STRLOG(mid, priv->sid, CHLOGRX, SL_TRACE, "-> CH_CONNECT_REQ");
				err = ch_connect_req(ch, q, mp);
				break;
			case CH_DATA_REQ:
				STRLOG(mid, priv->sid, CHLOGDA, SL_TRACE, "-> CH_DATA_REQ");
				err = ch_data_req(ch, q, mp);
				break;
			case CH_DISCONNECT_REQ:
				STRLOG(mid, priv->sid, CHLOGRX, SL_TRACE, "-> CH_DISCONNECT_REQ");
				err = ch_disconnect_req(ch, q, mp);
				break;
			case CH_DISABLE_REQ:
				STRLOG(mid, priv->sid, CHLOGRX, SL_TRACE, "-> CH_DISABLE_REQ");
				err = ch_disable_req(ch, q, mp);
				break;
			case CH_DETACH_REQ:
				STRLOG(mid, priv->sid, CHLOGRX, SL_TRACE, "-> CH_DETACH_REQ");
				err = ch_detach_req(ch, q, mp);
				break;
			default:
				swerr();
				STRLOG(mid, priv->sid, CHLOGRX, SL_TRACE | SL_ERROR,
				       "-> CH_????_???");
				freemsg(mp);
				err = 0;
				break;
			}
			/* revert to previous state on error */
			if (err)
				ch_set_state(ch, oldstate);
			clear_bit(0, &priv->flags);
		} else {
			err = -EDEADLOCK;
			qenable(q);	/* come back later */
		}
		return (err);
	}
	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHW) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
		}
		putnext(q, mp);
		return (0);
	case M_CTL:
		swerr();
		freemsg(mp);	/* for now */
		return (0);

	case M_IOCTL:
	{
		struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

		switch (ioc->ioc_cmd) {
		case CH_IOCGCONFIG:
		case CH_IOCSCONFIG:
		case CH_IOCTCONFIG:
		case CH_IOCCCONFIG:
		case CH_IOCGSTATEM:
		case CH_IOCCMRESET:
		case CH_IOCGSTATSP:
		case CH_IOCGSTATS:
		case CH_IOCCSTATS:
		case CH_IOCGNOTIFY:
		case CH_IOCSNOTIFY:
		case CH_IOCCNOTIFY:
		case CH_IOCCMGMT:
		default:
			break;
		}
		/* intentionally do not check for flow control */
		putnext(q, mp);
		return (0);
	      nak:
		DB_TYPE(mp) = M_IOCNAK;
		ioc->ioc_count = ioc->ioc_count == TRANSPARENT ? : 0;
		ioc->ioc_rval = -1;
		ioc->ioc_error = EINVAL;
		qreply(q, mp);
		return (0);
	}
	default:
		if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
}

/**
 * mx_r_msg: - process MX message other than M_DATA
 * @q: active queue (read queue)
 * @mp: the message to process
 */
STATIC noinline fastcall int
mx_r_msg(queue_t *q, mblk_t *mp)
{
	struct mx *mx = MX_PRIV(q);

	switch (DB_TYPE(mp)) {
	case M_PROTO:
	case M_PCPROTO:
	{
		ch_ulong ch_oldstate = ch_get_state(MXCH(mx));
		mx_ulong mx_oldstate = mx_get_state(mx);
		struct priv *priv;
		int err;

		dassert(mp->b_wptr >= mp->b_rptr + sizeof(mx_ulong));

		priv = MXPRIV(mx);
		if (!test_and_set_bit(0, &priv->flags)) {
			switch (*(mx_ulong *) mp->b_rptr) {
			case MX_INFO_ACK:
				STRLOG(mid, priv->sid, MXLOGRX, SL_TRACE, "MX_INFO_ACK <-");
				err = mx_info_ack(mx, q, mp);
				break;
			case MX_OPTMGMT_ACK:
				STRLOG(mid, priv->sid, MXLOGRX, SL_TRACE, "MX_OPTMGMT_ACK <-");
				err = mx_optmgmt_ack(mx, q, mp);
				break;
			case MX_OK_ACK:
				STRLOG(mid, priv->sid, MXLOGRX, SL_TRACE, "MX_OK_ACK <-");
				err = mx_ok_ack(mx, q, mp);
				break;
			case MX_ERROR_ACK:
				STRLOG(mid, priv->sid, MXLOGRX, SL_TRACE, "MX_ERROR_ACK <-");
				err = mx_error_ack(mx, q, mp);
				break;
			case MX_ENABLE_CON:
				STRLOG(mid, priv->sid, MXLOGRX, SL_TRACE, "MX_ENABLE_CON <-");
				err = mx_enable_con(mx, q, mp);
				break;
			case MX_CONNECT_CON:
				STRLOG(mid, priv->sid, MXLOGRX, SL_TRACE, "MX_CONNECT_CON <-");
				err = mx_connect_con(mx, q, mp);
				break;
			case MX_DATA_IND:
				STRLOG(mid, priv->sid, MXLOGDA, SL_TRACE, "MX_DATA_IND <-");
				err = mx_data_ind(mx, q, mp);
				break;
			case MX_DISCONNECT_IND:
				STRLOG(mid, priv->sid, MXLOGRX, SL_TRACE, "MX_DISCONNECT_IND <-");
				err = mx_disconnect_ind(mx, q, mp);
				break;
			case MX_DISCONNECT_CON:
				STRLOG(mid, priv->sid, MXLOGRX, SL_TRACE, "MX_DISCONNECT_CON <-");
				err = mx_disconnect_con(mx, q, mp);
				break;
			case MX_DISABLE_IND:
				STRLOG(mid, priv->sid, MXLOGRX, SL_TRACE, "MX_DISABLE_IND <-");
				err = mx_disable_ind(mx, q, mp);
				break;
			case MX_DISABLE_CON:
				STRLOG(mid, priv->sid, MXLOGRX, SL_TRACE, "MX_DISABLE_CON <-");
				err = mx_disable_con(mx, q, mp);
				break;
			case MX_EVENT_IND:
				STRLOG(mid, priv->sid, MXLOGRX, SL_TRACE, "MX_EVENT_IND <-");
				err = mx_event_ind(mx, q, mp);
				break;
			default:
				STRLOG(mid, priv->sid, MXLOGRX, SL_TRACE | SL_ERROR,
				       "MX_????_?? <-");
				freemsg(mp);
				err = 0;
				break;
			}
			if (err) {
				mx_set_state(mx, mx_oldstate);
				ch_set_state(MXCH(mx), ch_oldstate);
			}
			clear_bit(0, &priv->flags);
		} else {
			err = -EDEADLOCK;
			qenable(q);	/* come back later */
		}
		return (err);
	}
	case M_FLUSH:
		if (mp->b_rptr[0] & FLUSHR) {
			if (mp->b_rptr[0] & FLUSHBAND)
				flushband(q, mp->b_rptr[1], FLUSHDATA);
			else
				flushq(q, FLUSHDATA);
		}
		putnext(q, mp);
		return (0);
	case M_IOCACK:
	case M_IOCNAK:
	case M_COPYIN:
	case M_COPYOUT:
	{
		struct iocblk *ioc = (typeof(ioc)) mp->b_rptr;

		switch (ioc->ioc_cmd) {
		case MX_IOCGCONFIG:
		case MX_IOCSCONFIG:
		case MX_IOCTCONFIG:
		case MX_IOCCCONFIG:
		case MX_IOCGSTATEM:
		case MX_IOCMRESET:
		case MX_IOCGSTATSP:
		case MX_IOCSSTATSP:
		case MX_IOCGSTATS:
		case MX_IOCCSTATS:
		case MX_IOCGNOTIFY:
		case MX_IOCSNOTIFY:
		case MX_IOCCNOTIFY:
		case MX_IOCCMGMT:
		default:
			break;
		}
		putnext(q, mp);
		return (0);
	}
	case M_CTL:
		swerr();
		freemsg(mp);	/* for now */
		return (0);
	default:
		if (pcmsg(DB_TYPE(mp)) || bcanputnext(q, mp->b_band)) {
			putnext(q, mp);
			return (0);
		}
		return (-EBUSY);
	}
}

/**
 * ch_w_data: - process write data
 * @q: active queue (write queue)
 * @mp: data
 */
STATIC inline fastcall int
ch_w_data(register queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);
	mblk_t *db;

	if ((db = ch->tx_frame)) {
		unsigned char *wptr = db->b_wptr;

		for (frame = 0; frame < 1024; frame += 128) {
			for (smask = ch->smask; span = 0; smask && span < spans;
			     span++, smask >>= 1) {
				if (!(smask & 0x01))
					continue;
			}
		}
	}
	return (1);		/* put (back) on queue */
}

/**
 * ch_w_data: - process write data
 * @q: active queue (write queue)
 * @mp: data
 *
 * We really really really want single message blocks of exact multiples of the number of samples
 * per block (cp_samples).  So, if cp_samples is 8 and a 128 byte message block is provided, that is
 * fine.  If it is 1024 and a 128 byte messages block is provided, that is not fine.  Now, we
 * reported cp_samples to the user and it is their responsibility to provide data blocks of this
 * size.  If they do not, M_ERROR them.
 */
STATIC inline fastcall void
ch_w_data(register queue_t *q, mblk_t *mp)
{
	struct ch *ch = CH_PRIV(q);

	if (likely(ch_get_state(ch) == CHS_CONNECTED)) {
		if (likely(mp->b_cont == NULL)) {
			int bytes = ch->parms.cp_samples;
			int size = msgdsize(mp);

			if (likely(size == bytes) || likely(size > bytes && size % bytes == 0)) {
				putq(q, mp);
				return;
			}
			STRLOG(modid, PRIV(q)->sid, 0, SL_TRACE | SL_ERROR,
			       "block size %d, expecting %d", size, bytes);
		} else {
			int count;
			mblk_t *bp;

			for (count = 1, bp = mp; (bp = bp->b_cont); count++) ;
			STRLOG(modid, PRIV(q)->sid, 0, SL_TRACE | SL_ERROR,
			       "single block expected, %d found", count);
		}
	} else {
		STRLOG(modid, PRIV(q)->sid, CHLOGDA, SL_TRACE | SL_ERROR,
		       "discarding data in state %s", ch_state_string(ch_get_state(ch)));
	}
	freemsg(mp);
	putnextctl1(_RD(q), M_ERROR, EPROTO);
	ch_set_state(ch, CHS_UNUSABLE);
	return;
}

/**
 * ch_wput: - write put procedure, message from above
 * @q: active queue (write queue)
 * @mp: message to put
 *
 * M_DATA message from the CH user are processed immediately.  Other messages are processed as
 * normal.
 */
STATIC streamscall int
ch_wput(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		ch_w_data(q, mp);
	else if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || ch_w_msg(q, mp))
		putq(q, mp);
	return (0);
}

/**
 * ch_wsrv: - service write queue
 * @q: queue to service (write queue)
 *
 * Simple queueing, flow through service procedure with the wrinkle that it awlays puts M_DATA
 * message blocks back on the queue.
 */
STATIC streamscall int
ch_wsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q)))
		if (likely(DB_TYPE(mp) == M_DATA && ch_get_state(CH_PRIV(q)) == CHS_CONNECTED) ||
		    ch_w_msg(q, mp)) {
			putbq(q, mp);
			break;
		}
	return (0);
}

/**
 * mx_getq: - pull a data message from the write queue
 * @q: the write queue from which to pull
 *
 * This function is used to pull a message from the write queue in the read queue put procedure.
 * Note that the write put procedure makes sure that M_DATA message blocks have exactly one block
 * containing an exact multiple of cp_samples bytes.
 */
STATIC noinline fastcall __hot_in mblk_t *
mx_getq(queue_t *q)
{
	mblk_t *mp;
	pl_t pl;

	dassert(mp != NULL);
	pl = freezestr(q);
	for (mp = q->q_first; mp && mp->b_datap->db_type != M_DATA; mp = mp->b_next) ;
	if (mp) {
		rmvq(q, mp);
		prefetch(mp->b_rptr);
	}
	unfreezestr(q, pl);
	return (mp);
}

/**
 * mx_putbq: - put a data message back on the write queue
 * @q: the write queue to put back to
 *
 * This function is used to place a transmit message back on the write queue from the read queue put
 * procedure.  It is almost never called.  Transmit message blocks should be properly sized so that
 * each is consumed entirely.  Nevertheless this could happen when sample sizes are much smaller
 * than a FASTBUF.
 */
STATIC noinline fastcall __hot_in void
mx_putbq(queue_t *q, mblk_t *mp)
{
	mblk_t *emp;
	int rtn;

	dassert(mp != NULL);

	if (unlikely(mp->b_wptr > mp->b_rptr)) {

		if (mp->b_wptr > mp->b_rptr + CH_PRIV(q)->ch.cp_samples) {
			pl_t pl;

			pl = freezestr(q);
			{
				for (emp = q->q_first;
				     emp && (pcmsg(DB_TYPE(emp)) || emp->b_band > 0);
				     emp = emp->b_next) ;
				mp->b_band = 0;
				insq(q, emp, mp);	/* must always be successful with band 0 */
			}
			unfreezestr(q, pl);
			return (rtn);
		}
		STRLOG(modid, PRIV(q)->sid, CHLOGDA, SL_TRACE | SL_ERROR,
		       "block is non-integral number of samples");
		putnextctl1(_RD(q), M_ERROR, EPROTO);
		mx_set_state(MX_PRIV(q), MXS_UNUSABLE);
	}
	freemsg(mp);
	return;
}

/**
 * mx_rmvq: - pull a message from the tail of the read queue
 * @q: the read queue from which to pull
 * @bytes: minimum number of bytes available
 *
 * This function is used to pull a message from the tail of the read queue in the read queue put
 * procedure.  We check if there is a message on the tail of the queue that will meet our needs,
 * otherwise, a new message block of size "bytes" is allocated.  If bytes is greater than half of a
 * FASTBUF, then there will never be any available messages and one is simply allocated.
 */
STATIC noinline fastcall __hot_in mblk_t *
mx_rmvq(queue_t *q, int bytes)
{
	if (unlikely((bytes << 1) <= FASTBUF)) {
		mblk_t *mp;
		pl_t pl;

		pl = freezestr(q);
		{
			for (mp = q->q_last; mp && DB_TYPE(mp) != M_DATA; mp = mp->b_prev) ;
			if (mp) {
				if (mp->b_datap->db_lim >= mp->b_wptr + bytes)
					rmvq(q, mp);
				else
					mp = NULL;
			}
		}
		unfreezestr(q, pl);
		if (mp)
			return (mp);
	}
	return allocb(bytes, BPRI_HI);
}

/**
 * mx_r_data: - process M_DATA put from below.
 * @q: read queue
 * @mp: M_DATA to process
 *
 * This is a V40XP only version of this procedure.  It relies upon vector of 32-bit words containing
 * a bit mask of selected slots and follows the V40XP (Tormenta) slot mapping only.
 */
STATIC inline fastcall __unlikely void
mx_r_data(queue_t *q, mblk_t *msg)
{
	struct mx *mx = MX_PRIV(q);
	static const int samples = 8;
	static const int spans = 4;
	static const int slots = 32;
	static const int framesize = 128;	/* slots * spans */
	static const int blocksize = 1024;	/* samples * spans * slots */

	int bytes = mx->channels * samples;

	/* Examples: T1 card, fractional T1: span 3 of 4, ts 1-4: smask = 0x4, cmask[0] = 0,
	   cmask[1] = 0, cmask[2] = 0x0000002e, cmask[3] = 0, channels = 4, bytes = 32 */

	if (mx_get_state(mx) == MXS_CONNECTED) {
		unsigned char *block = msg->b_rptr;

		if (likely(canputnext(q)) && likely((bp = mp = mx_rmvq(q, bytes)))) {
			const unsigned char *frame;
			int sample;

			for (frame = block, sample = 0; sample < samples;
			     sample++, frame += framesize) {
				uint32_t smask;
				int span;

				prefetch(frame);
				prefetch(frame + 32);
				prefetch(frame + 64);
				prefetch(frame + 96);

				for (smask = mx->smask, span = 0;
				     span < spans && smask; span++, smask >>= 1) {
					const unsigned char *pos;
					uint32_t cmask;
					int slot;

					if (!(smask & 0x1))
						continue;
					for (cmask = mx->cmasks[span], pos = frame + span, slot = 0;
					     slot < slots && cmask;
					     slot++, pos += spans, cmask >>= 1) {
						if (!(cmask & 0x1))
							continue;
						*bp->b_wptr++ = *pos;
					}
				}
			}
			putnext(q, mp);
		} else {
			CH_PRIV(q)->stats.overflows += bytes;
		}
		block += blocksize;
		if (likely(!!(bp = mp = mx_getq(_WR(q), bytes)))) {
			unsigned char *frame;
			int sample;

			for (frame = block, sample = 0; sample < samples;
			     sample++, frame += framesize) {
				uint32_t smask;
				int span;

				prefetchw(frame);
				prefetchw(frame + 32);
				prefetchw(frame + 64);
				prefetchw(frame + 96);

				for (smask = mx->smask, span = 0;
				     span < spans && smask; span++, smask >>= 1) {
					unsigned char *pos;
					uint32_t cmask;
					int slot;

					if (!(smask & 0x1))
						continue;
					for (cmask = mx->cmasks[span], pos = frame + span, slot = 0;
					     slot < slots && cmask;
					     slot++, pos += spans, cmask >>= 1) {
						if (!(cmask & 0x1))
							continue;
						*pos = *bp->b_rptr++;
					}
				}
			}
			mx_putbq(_WR(q), mp);	/* why not just freemsg? */
		} else {
			CH_PRIV(q)->stats.underruns += bytes;
		}
	}
      underrun:
	/* always reply */
	qreply(q, msg);
	return;
}

#if 0
/**
 * mx_r_data: - process M_DATA put from below.
 * @q: active queue (read queue)
 * @mp: M_DATA to put
 *
 * M_DATA messages from the MX are special.  We must process them immediate from the put procedure
 * and return the block to allow the MX to proceed.  This procedure is likely called from a high
 * tasklet, so minimal processing should be performed.
 *
 * M_DATA messages contain two bit block: an RX bit block followed by a TX bit block.  The channel
 * map defines the the offsets of channels and samples within the bit block.  For a bit block
 * containing N samples (parms.mp_samples), for M channels (parms.mp_rx_channels) the channel map
 * contains N * M entries.  So, for example, a 31 channel E1 span MX delivering bit blocks with 8
 * frames, will have a channel map with 248 entires.  The channel map entry for a given channel will
 * be chanmap[sample * M + chan].  Currently we have not implemented the MX driver passing its
 * channel map to its user, we therefore, assume the V40XP channel maps and generate CH specific
 * channel maps at connection time.
 *
 * M_DATA messages from the MX are processed immediately with an RX run and then a TX run.  Note
 * that for data messages we are likely running from a high tasklet.  Write timing is synchronized
 * off of the read side.
 */
STATIC inline fastcall __unlikely void
mx_r_data(queue_t *q, mblk_t *msg)
{
	struct mx *mx = MX_PRIV(q);
	struct ch *ch = CH_PRIV(q);

	if (mx_get_state(mx) == MXS_CONNECTED) {
		int channels = mx->channels;	/* only connected channels */
		int bytes = ch->parms.ch_samples;	/* also channels * mx->parms.mp_samples */
		int frame, channel;
		mblk_t *rx, *tx, *bp;
		int samples = mx->info.mp_samples; /* samples per slot per bit block (8) */
		int spans = mx->info.mp_spans; /* number of spans per block (4) */
		int slots = mx->info.mp_slots; /* number of slots per span (32) */
		int offset = spans * slots; /* offset into chanmap for each sample */
		int mapsize = samples * offset; /* total chanmap size */

		/* RX run */
		/* mx_rmvq checks sufficient bytes in the (single) message block. */
		if (likely(canputnext(q)) && likely((bp = rx = mx_rmvq(q, bytes)))) {
			for (frame = 0; frame < mx->mapsize; frame += offset)
				for (channel = 0; channel < channels; channel++)
					*bp->b_rptr++ = msg->b_rptr[mx->chanmap[frame + mx->slotmap[channel]]];
			putnext(q, rx);
		} else {
			ch->stats.overflows += bytes;
			/* notification ??? */
		}

		/* TX run */
		/* mx_getq checks sufficient bytes in the (single) message block. */
		if (likely(!!(bp = tx = mx_getq(_WR(q), bytes)))) {
			for (frame = 0; frame < mx->mapsize; frame += offset)
				for (channel = 0; channel < channels; channel++)
					msg->b_rptr[mx->chanmap[frame + mx->slotmap[channel]]] = *bp->b_rptr++;
			mx_putbq(_WR(q), tx);	/* why not just freemsg? */
		} else {
			ch->stats.underruns += bytes;
			/* notification ??? */
		}
	}
      underrun:
	qreply(q, msg);		/* always return downstream */
}
#endif


/**
 * mx_rput: - read put procedure, message from below
 * @q: active queue (read queue)
 * @mp: message to put
 *
 * M_DATA messages from the MX provider are processed immediately.  Other messages are processed as
 * normal.
 */
STATIC streamscall int
mx_rput(queue_t *q, mblk_t *mp)
{
	if (likely(DB_TYPE(mp) == M_DATA))
		mx_r_data(q, mp);
	else if ((!pcmsg(DB_TYPE(mp)) && (q->q_first || (q->q_flag & QSVCBUSY))) || mx_r_msg(q, mp))
		putq(q, mp);
	return (0);
}

/**
 * mx_rsrv: - service write queue
 * @q: queue to service (read queue)
 *
 * The only thing that we queue on the read queue is primitivies from below that are awaiting
 * processing for some reason (locks, buffers, flow control).
 */
STATIC streamscall int
mx_rsrv(queue_t *q)
{
	mblk_t *mp;

	while ((mp = getq(q)))
		if (unlikely(mx_r_msg(q, mp))) {
			putbq(q, mp);
			break;
		}
	return (0);
}

/*
 * STREAMS open and close procedures.
 */
static caddr_t ch_head;
static atomic_t ch_numb;

static streamscall __unlikely int
ch_qopen(queue_t *q, dev_t *devp, int oflags, int sflag, cred_t *crp)
{
	struct MX_info_req *p;
	struct priv *priv;
	mblk_t *mp;
	int err;

	if (!(mp = allocb(sizeof(*p), BPRI_WAITOK)))
		return (ENOSR);

	if ((err = mi_open_comm(&ch_head, sizeof(*priv), q, devp, oflags, sflag, crp))) {
		freeb(mp);
		return (err);
	}
	priv = PRIV(q);
	priv_init(priv, atomic_add_return(&ch_numb, 1));

	mp->b_datap->db_type = M_PCPROTO;
	p = (typeof(p)) mp->w_rptr;
	mp->b_wptr += sizeof(*p);
	p->mx_primitive = MX_INFO_REQ;

	qprocson(q);
	putnext(q, mp);		/* generate immediate info request */
	return (0);
}

static streamscall __unlikely int
ch_qclose(queue_t *q, dev_t *devp, int oflags)
{
	int err;

	qprocsoff(q);
	if ((err = mi_close_comm(&ch_head, q)))
		return (err);
	return (0);
}

/*
 *  STREAMS Definitions.
 */
static struct module_info ch_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_lowat = STRLOW,
	.mi_hiwat = STRHIGH,
};

static struct module_stat ch_mstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat mx_mstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit ch_winit = {
	.qi_putp = ch_wput,
	.qi_srvp = ch_wsrv,
	.qi_minfo = &ch_minfo,
	.qi_mstat = &ch_mstat,
};

static struct qinit ch_rinit = {
	.qi_putp = mx_rput,
	.qi_srvp = mx_rsrv,
	.qi_qopen = ch_qopen,
	.qi_qclose = ch_qclose,
	.qi_minfo = &ch_minfo,
	.qi_mstat = &mx_mstat,
};

struct streamtab chinfo = {
	.st_rdinit = &ch_rinit,
	.st_wrinit = &ch_winit,
};

/*
 * Linux and STREAMS registration.
 */

#ifdef module_param
module_param(modid, ushort, 0444);
#else				/* module_param */
MODULE_PARM(modid, "h");
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for the CH-MOD module. (0 for allocation.)");

static struct fmodsw ch_fmod = {
	.f_name = MOD_NAME,
	.f_str = &chinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static void
chterminate(void)
{
	int err;

	if ((err = unregister_strmod(modid, &ch_fmod)) < 0)
		STRLOG(modid, 0, 0, SL_ERROR | SL_NOPUTBUF, "unregister_strmod() failed %d", -err);
	return;
}
static int
chinit(void)
{
	int err;

	atomic_set(&ch_numb, 0);
	if ((err = register_strmod(modid, &ch_fmod)) < 0)
		return (err);
	if (modid == 0)
		modid = err;
	return (0);
}

module_init(chinit);
module_exit(chterminate);
