/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : hardware.c
 * Description                  : hardware spceific
 *
 *
 **********************************************************************/

#ifdef WNT_ANT
#include <gnwinnt.h>
#include <sys/types.h>
#include <sysmacro.h>
#include <stream.h>
#include <stropts.h>
#include <cmn_err.h>
#else
#ifdef LINUX
#include "dlgclinux.h"
#define _SCO_
#else
#include <sys/types.h>
#include <sys/stream.h>
#ifndef AIX_ANT
#include <sys/cmn_err.h>
#else
#include "ant_aix.h" 
#endif
#endif /* LINUX */
#endif
#include "dlgcos.h"
#include "include.h"
#include "hardware.h"
#include "hardexp.h"

#ifdef WNT_ANT
extern VOID cmn_err(ULONG,...);
#endif

#ifdef SOL_ANT
extern kmutex_t inthw_lock;
extern BYTE in_inter;
extern BYTE in_mutex;
#endif

#ifdef LINUX
#ifdef LFS
extern spinlock_t inthw_lock;
#else
extern lis_spin_lock_t inthw_lock;
#endif
#endif /* LINUX */

/****************************************************************
*
*        NAME : SHARED MEMORY I/O routines 
*
****************************************************************/

SHORT hw_rdwrt = 0;

/****************************************************************
*        NAME : init_hardware
* DESCRIPTION : Initiation of hardware variables. (called from antares.c)
*       INPUT : I/O continuous operations 
*      OUTPUT : 
*     RETURNS : void
*    CAUTIONS : 
****************************************************************/
VOID init_hardware(SHORT rdwrt)
{
/* 
    For now hw_rdwrt is initiated - which is number of continuous 
    I/O operations protected from interrupt 
*/

    hw_rdwrt = rdwrt;
    return;
}

/****************************************************************
*        NAME : a4_readcontrol
* DESCRIPTION : Reads control port of board
*       INPUT : Port address
*      OUTPUT : 
*     RETURNS : Value read from port
*    CAUTIONS : 
****************************************************************/
WORD A4_readControlPort(WORD  port)
{
    WORD lport;
#ifdef BIG_ANT
    port = port & ~_A4_NO_SWAP;
#endif
    lport = A4_CONTROL_PORT(port);
    return (WORD)ainpw(lport);
}

/****************************************************************
*        NAME : a4_writecontrol
* DESCRIPTION : Writes control port of board
*       INPUT : Port address,value to be written
*      OUTPUT : 
*     RETURNS : void
*    CAUTIONS : 
****************************************************************/
VOID A4_writeControlPort(WORD port,WORD   value)
{
    WORD lport;
#ifdef BIG_ANT
    port = port & ~_A4_NO_SWAP;
#endif
    lport = A4_CONTROL_PORT(port);
    aoutpw(lport, (WORD)value);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    Routine: A4_setAddress

Description: Set the address pointers for reading and writing
             data to the specified Antares.  This routine does
             not affect the state of the auto-increment mode.
             This routine does not validate or manipulate
             the address in any way.  If the caller provides an
             invalid address, the results are unpredictable.

-------------------------------------------------------------------*/
VOID A4_setAddress(WORD port,ULONG address)
{
    BYTESWAP mix;
    WORD lport;
#ifdef WNT_ANT
    KIRQL oldspl;
#else
#ifdef LFS
    unsigned long oldspl;
#else
    int oldspl;
#endif
#endif

    mix.fourbytes = address;
   /* 
    *  Set address is protected by CLI / STI because background (interrupt)
    *  functions may use this function 
    */
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
    if ((!in_inter) && (!in_mutex)) {
        mutex_enter(&inthw_lock);
    }
#elif LINUX
    DLGCSPINLOCK(inthw_lock, oldspl);
#else
    DLGCSPLSTR(oldspl);
#endif
#endif
    lport = A4_HIGH_ADDRESS(port);
#ifdef BIG_ANT
    aoutpw(lport,mix.twobytes[0]);
#else
    aoutpw(lport,mix.twobytes[1]);
#endif
    lport = A4_LOW_ADDRESS(port);
#ifdef BIG_ANT
    aoutpw(lport,mix.twobytes[1]);
#else
    aoutpw(lport,mix.twobytes[0]);
#endif
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
    if ((!in_inter) && (!in_mutex)) {
        mutex_exit(&inthw_lock);
    }
#elif LINUX
    DLGCSPINUNLOCK(inthw_lock, oldspl);
#else
    DLGCSPLX(oldspl);
#endif
#endif
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    Routine: A4_getAddress

Description: Get the address pointers for reading and writing
             data to the specified Antares.  This routine does
             not affect the state of the auto-increment mode.
             This routine does not validate or manipulate
             the address in any way.

-------------------------------------------------------------------*/
ULONG A4_getAddress(WORD port)
{
    BYTESWAP mix;
    WORD lport;
#ifdef WNT_ANT
    KIRQL oldspl;
#else
#ifdef LFS
    unsigned long oldspl;
#else
    int oldspl;
#endif
#endif

#ifdef AIX_ANT
#else
#ifdef SOL_ANT
    if ((!in_inter) && (!in_mutex)) {
        mutex_enter(&inthw_lock);
    }
#elif LINUX
    DLGCSPINLOCK(inthw_lock, oldspl);
#else
    DLGCSPLSTR(oldspl);
#endif
#endif
    lport = A4_HIGH_ADDRESS(port);
#ifdef BIG_ANT
    mix.twobytes[0] = ainpw(lport);	/* Gets low word of long */
#else
    mix.twobytes[1] = ainpw(lport);	/* Gets high word of long */
#endif
    lport = A4_LOW_ADDRESS(port);
#ifdef BIG_ANT
    mix.twobytes[1] = ainpw(lport);	/* Gets high word of long */
#else
    mix.twobytes[0] = ainpw(lport);	/* Gets low word of long */
#endif
#ifdef AIX_ANT
#else
#ifdef SOL_ANT
    if ((!in_inter) && (!in_mutex)) {
        mutex_exit(&inthw_lock);
    }
#elif LINUX
    DLGCSPINUNLOCK(inthw_lock, oldspl);
#else
    DLGCSPLX(oldspl);
#endif
#endif
    return(mix.fourbytes);
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    Routine: A4_read16Bits

Description: Read 16 bits from the current address on the specified
             Antares.  This routine does not affect the state of
             nor care about either auto-increment mode or the half-word
             selection bit of the address pointer.

    Returns: Datum read at current address of specified card.

-------------------------------------------------------------------*/
WORD A4_read16Bits(WORD  port)
{
    return(WORD) ainpw((WORD)A4_DATA_PORT(port));
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    Routine: A4_write16Bits

Description: Write 16 bits to the current address on the specified
             Antares.  This routine does not affect the state of
             nor care about either auto-increment mode or the half-word
             selection bit of the address pointer.

-------------------------------------------------------------------*/
VOID A4_write16Bits(WORD port,WORD value)
{
    WORD lport;

    lport = A4_DATA_PORT(port);
    aoutpw(lport, value);
}

#ifdef LFS
ULONG ainpl(WORD port);
#endif
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    Routine: A4_readWord

Description: Read a 32-bit word from the current address on the specified
             Antares.  This routine does not affect the state of
             nor care about the auto-increment mode.

    Returns: Datum read at current address of specified card.

-------------------------------------------------------------------*/
ULONG A4_readWord(WORD port)
{
    BYTESWAP mix;
    WORD lport;

    lport = A4_DATA_PORT(port);

#ifndef FORCE_16_BITS
#ifdef PCI_ANT
    if (!(lport & _A4_16BIT_PORT)) {	/* Use 32-bit i/o for PCI board */
        return(ainpl(lport));
    }
#endif /* PCI_ANT */
#endif /* FORCE_16_BITS */

#ifdef BIG_ANT
    mix.twobytes[1] = ainpw(lport);
    mix.twobytes[0] = ainpw(lport);
#else
    mix.twobytes[0] = ainpw(lport);
    mix.twobytes[1] = ainpw(lport);
#endif

    return mix.fourbytes;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    Routine: A4_read1Word

    Description: same as A4_readWord but address is determined before reading

-------------------------------------------------------------------*/
ULONG A4_read1Word(WORD  port,ULONG  address)
{

    ULONG rword;
#ifdef _SCO_
    A4_readData(port, address, 1, (ULONG far *)&rword);
#else
    A4_readData(port, address, 1, (ULONG *)&rword);
#endif /* _SCO_ */
    return(rword);
}

#ifdef LFS
VOID aoutpl(WORD port, ULONG data);
#endif
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    Routine: A4_writeWord

Description: Write a 32-bit word to the current address on the specified
             Antares.  This routine does not affect the state of
             nor care about the auto-increment mode.

-------------------------------------------------------------------*/
VOID A4_writeWord(WORD port,ULONG datum)
{
    BYTESWAP mix;
    WORD lport;

    lport = A4_DATA_PORT(port);

#ifndef FORCE_16_BITS
#ifdef PCI_ANT
    if (!(lport & _A4_16BIT_PORT)) { /* Use 32-bit i/o for PCI board */
        aoutpl(lport, datum);
	return;
    }
#endif /* PCI_ANT */
#endif /* FORCE_16_BITS */

    mix.fourbytes = datum;

#ifdef BIG_ANT
    aoutpw(lport, mix.twobytes[1]);
    aoutpw(lport, mix.twobytes[0]);
#else
    aoutpw(lport, mix.twobytes[0]);
    aoutpw(lport, mix.twobytes[1]);
#endif
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    Routine: A4_readData

Description: Read 32-bit words from the specified location in the
             specified Antares.  If auto-increment mode is set, this
             routine will not set the I/O address before every read.

-------------------------------------------------------------------*/

VOID A4_readData(WORD port,ULONG address,WORD nWords,ULONG far *data)
{
    WORD nw;
    WORD rsize; 
    ULONG iaddress;
#ifdef _SCO_
    WORD far *idata;
#else
    WORD *idata;
#endif /* _SCO_ */

    if (nWords == 0) {		/* Empty buffer */
	return;
    }
    nw = nWords;

    rsize = hw_rdwrt;	/* Number of continuous I/O operations */

   /*
    * Force address to low-order halfword for first write
    */
    address = A4_FORCE_LOW_16BITS(address);
    iaddress = address;

    idata = (WORD far *)data;

    if (A4_AUTO_INCREMENT(A4_readControlPort(port))) {
       /* Auto increament option */
	while (TRUE) {
	    if (nw > rsize) {
	       /* Read in loop; each read operation is protected from
		   interrupts; Interrupts allowed between read operations 
		*/

		a4_auto_read(port,rsize,iaddress,idata);
		idata += (rsize * 2);
		iaddress += rsize;
		nw -= rsize;
	    }
	    else {
		a4_auto_read(port,nw,iaddress,idata);
		break;
	    }
	}
    }
    else {
	while (TRUE) {
	   /* No auto_increament option */
           /* shouldn't get here!!! Nick Shah, 9/4/97 */
	    if (nw > rsize) {
	        a4_read(port,rsize,iaddress,idata);
		idata += (rsize * 2);
		iaddress += rsize;
		nw -= rsize;
	    }
	    else {
		a4_read(port,nw,iaddress,idata);
		break;
	    }
        }
    }
    return;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    Routine: A4_writeData

Description: Write 32-bit words to the specified location in the
             specified Antares.  If auto-increment mode is set, this
             routine will not set the I/O address before every write.

-------------------------------------------------------------------*/
VOID A4_writeData(WORD port,ULONG address,WORD nWords,ULONG far *data)
{
    WORD nw;
    WORD wsize; 
    ULONG iaddress;
#ifdef _SCO_
    WORD far *idata;
#else
    WORD *idata;
#endif /* _SCO_ */

    if (nWords == 0) {		 /* Empty buffer */
        return;
    }
    nw = nWords;
#if 0
    if ((address >= 0xFFFF00) && (address <= 0xFFFF58)) {
        for (wsize = 0; wsize < 3; wsize++) {
            ant_id_click();
        }
    }

    if ((address >= 0xFFFC00) && (address <= 0xFFFE4C)) {
        for (wsize = 0; wsize < 3; wsize++) {
            ant_id_click();
        }
        send_error_msg(0,0,0,-3000,__LINE__);
    }
#endif
    wsize = hw_rdwrt;	/* Number of continuous I/O operations */
   /*
    * Force address to low-order halfword for first write
    */
    address = A4_FORCE_LOW_16BITS(address);
    iaddress = address;

#ifdef _SCO_
    idata = (WORD far *)data;
#else
    idata = (WORD *)data;
#endif /* _SCO_ */

    if (A4_AUTO_INCREMENT(A4_readControlPort(port))) {
       /* Auto increament option */
	while (TRUE) {
	    if (nw > wsize) {
	       /* Write in loop; each write operation is protected from
		   interrupts; Interrupts allowed between write operations 
		*/
		a4_auto_write(port,wsize,iaddress,idata);
		idata += (wsize * 2);
		iaddress += wsize;
		nw -= wsize;
	    }
	    else {
		a4_auto_write(port,nw,iaddress,idata);
		break;
	    }
	}
    }
    else {
       /* No auto_increament option */
       /* shouldn't get here!!! Nick Shah, 9/4/97 */
	while (TRUE) {
	    if (nw > wsize) {
	        a4_write(port,wsize,iaddress,idata);
		idata += (wsize * 2);
		iaddress += wsize;
		nw -= wsize;
	    }
	    else {
		a4_write(port,nw,iaddress,idata);
		break;
	    }
	}
    }
    return;
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    Routine: A4_present

Description: Determine if Antares is accessible at given address.
             This routine attempts a non-destructive manipulation
             of the control port.  If the device which responds
             at the given address is not a Antares, this routine
             could have unfortunate consequences.

    Returns: TRUE  - a Antares is present at the specified address
             FALSE - a Antares is NOT present at the specified address

--------------------------------------------------------------------*/
BYTE A4_present(WORD port)
{
    WORD  ctrl;
    WORD  status;
    BYTE  iauto;

    ctrl = A4_readControlPort(port);
   /*
    * First we are going to attempt to change the state
    * of the auto-increment bit.
    */

    if (A4_AUTO_INCREMENT(ctrl)) {
	iauto = TRUE;
        A4_clearAutoIncrement(port);
    }
    else {
        A4_setAutoIncrement(port);
	iauto = FALSE;
    }

   /*
    * Read control port and XOR with saved value
    */
    status = A4_readControlPort(port);
    status ^= ctrl;

   /*
    * At this point if we have a Antares, status should have _A4_AIDIS bit
    * set - if not, punt.  Don't attempt to restore anything, we may have
    * already done enough damage.
    */
    if (!(status & _A4_AIDIS)) {
        cmn_err(0, "Board at 0x%x failed with ctrl=0x%x not set with %x\n",
		port, status, _A4_AIDIS);
        return FALSE;
    }

    A4_writeControlPort(port, (WORD)A4_MASK_CTRL_PORT(ctrl));

    if (iauto) {
        A4_setAutoIncrement(port);
    }
    else {
        A4_clearAutoIncrement(port);
    }
    return TRUE;
}
