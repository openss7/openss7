/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : hardexp.h 
 * Description                  : hardware externs
 *
 *
 **********************************************************************/
/*
 * ---------- External definitions ----------
 */

#define _A4_AIDIS       0x40        /* Auto Increment Disable R/W */
#define _A4_PCTOKREQ    0x80        /* PC Token Request R/W */
#define _A4_PCIRQ       0x100       /* DSP-to-PC Interrupt Pending R/W */
#if defined(BIG_ANT) || defined(LINUX)
#define _A4_NO_SWAP     0x8000
#endif

#define _A4_16BIT_PORT	0x200	/* use 16-bit i/o - isa boards */

#define A4_DSP_MAX         4           /* Maximum number of DSPs */
#define A4_DSP_MIN         2           /* Minimum number of DSPs */
#define A4_MIN_PORT        0x0200      /* Lowest posible port address */
#define A4_MAX_PORT        0xf278      /* Highest possible port address */
#define A4_TOTALPORTS      0x0100
#define MAX_VPRO_BOARDS    16          /* Max # of boards in a system */


/****************************************************************

		HARDWARE FUNCTIONS & MACROS

****************************************************************/

VOID init_hardware(SHORT rdwrt);

WORD  A4_read16Bits(WORD);
VOID  A4_write16Bits(WORD,WORD);
ULONG A4_readWord(WORD);
ULONG A4_read1Word(WORD,ULONG);
VOID  A4_writeWord(WORD,ULONG );
#if defined(_SCO_) || defined(LINUX)
VOID  A4_readData(WORD,ULONG ,WORD,ULONG  far *);
VOID  A4_writeData(WORD,ULONG ,WORD,ULONG far *);
#else
VOID  A4_readData(WORD,ULONG ,WORD,ULONG  *);
VOID  A4_writeData(WORD,ULONG ,WORD,ULONG *);
#endif /* _SCO_ */
WORD  A4_readControlPort(WORD);
VOID  A4_writeControlPort(WORD,WORD);
VOID  A4_setAddress(WORD,ULONG );
ULONG A4_getAddress(WORD);
BYTE  A4_present(WORD);

/* Low level functions */
VOID aoutpw(WORD,WORD);
WORD ainpw(WORD);
VOID wctrl(WORD,BYTE);
VOID wask(WORD,BYTE);
VOID wfree(WORD,BYTE);
WORD is_bit(WORD,WORD);

#if defined(_SCO_) || defined(LINUX)
WORD h_getds(VOID);

VOID a4_auto_write(WORD,WORD,ULONG,WORD far *);
VOID a4_auto_read(WORD,WORD,ULONG,WORD far *);
VOID a4_write(WORD,WORD,ULONG,WORD far *);
VOID a4_read(WORD,WORD,ULONG,WORD far *);

#else
#ifdef VME_ANT
VOID a4_auto_write(caddr_t,WORD,ULONG *,WORD);
VOID a4_auto_read(caddr_t,WORD,ULONG *,WORD);
#else
VOID a4_auto_write(WORD,WORD,ULONG,WORD *);
VOID a4_auto_read(WORD,WORD,ULONG,WORD *);
#endif
VOID a4_write(WORD,WORD,ULONG,WORD *);
VOID a4_read(WORD,WORD,ULONG,WORD *);
#endif /* _SCO_ */

/****************************************************************
*        NAME : IS_IRQ
* DESCRIPTION : Finds wether board was interrupted
*       INPUT : Port address
*      OUTPUT : 
*     RETURNS : TRUE/FALSE
*    CAUTIONS : 
****************************************************************/
#define is_irq_isr(port)  is_bit_isr(port,(WORD)_A4_PCIRQ)
#define is_irq(port)  is_bit(port,(WORD)_A4_PCIRQ)

/****************************************************************
*        NAME : CLEAR_IRQ
* DESCRIPTION : Clears board irq
*       INPUT : Port address
*      OUTPUT : 
*     RETURNS : void
*    CAUTIONS : 
****************************************************************/
#define clear_irq_isr(port) wctrl_isr(port,(BYTE)1);wctrl_isr(port,(BYTE)8)
#define clear_irq(port) wctrl(port,(BYTE)1);wctrl(port,(BYTE)8)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    Routine: A4_interruptDSP

Description: Interrupt the specified DSP

-------------------------------------------------------------------*/
#define A4_interruptDSP(port,dsp) wctrl(port,(BYTE)(0x10 << dsp))

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    Routine: A4_releaseToken

Description: Release the PC/DSP token from the specified Antares

-------------------------------------------------------------------*/
#define A4_releaseToken(port) 	wfree(port,(BYTE)(~_A4_PCTOKREQ))

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    Routine: A4_requestToken

Description: Request the PC/DSP token from the specified Antares

-------------------------------------------------------------------*/
#define A4_requestToken(port)  	wask(port,(BYTE)(_A4_PCTOKREQ))

#define A4_token_grant(port,lport,retcode) \
	lport = A4_CONTROL_PORT(port);	   \
	retcode = A4_TOKEN_GRANT((WORD)ainpw(lport))

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    Routine: A4_clearAutoIncrement

Description: Set the specified Antares NOT to use auto-increment mode
             for addressing data.

-------------------------------------------------------------------*/
#define A4_clearAutoIncrement(port) wask(port,(BYTE)(_A4_AIDIS))

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    Routine: A4_setAutoIncrement

Description: Set the specified Antares to use auto-increment mode
             for addressing data.

-------------------------------------------------------------------*/
#define A4_setAutoIncrement(port)  wfree(port,(BYTE)(~_A4_AIDIS));

#ifdef _SCO_
#define HCLI() \
        _asm pushf \
        _asm cli

#define HSTI() \
        _asm popf
#endif /* _SCO_ */
