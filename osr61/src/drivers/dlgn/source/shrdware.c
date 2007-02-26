/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : shrdware.c 
 * Description                  : SRAM hardware specific
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
#include "hardexp.h"
#ifdef BIG_ANT
#include "byteswap.h"
#endif

#define	A4_HIGH_ADDRESS 6
#define	A4_SUB_ADDRESS 2

#if (defined(AIX_ANT) || defined(WNT_ANT))
VOID outw( WORD, WORD );
WORD inw( WORD );
#endif

#ifdef WNT_ANT
#define GN_MAX_PHYS_IO_PORT  0x400
extern ULONG *GnPhysToVirtIoPortMap; 
extern VOID cmn_err(ULONG,...);
#endif 

#ifdef SOL_ANT
extern kmutex_t inthw_lock;
extern BYTE in_inter;
extern BYTE in_mutex;
#endif

/**********************************************************************
**  Redefinition of inw(...) and outw(...) for BIG Endian systems
**  BIG Endian systems do not have i/o mapped i/o; everything is mem mapped
**********************************************************************/
#ifdef SPARC_PCI	/* get pc x86 macros */

VOID outl(int port, ULONG data)
{
#ifdef FORCE_16_BITS /* assume that auto-incr is on */
    WORD data0, data1;

    data0 = (data & 0xffff); /* low  16 bits */
    data1 = (data >> 16); /* high 16 bits */

    (*(WORD *)port) = data1; /* high 16 bits */
    (*(WORD *)port) = data0; /* low  16 bits */
#else
    (*(ULONG *)port) = data;
#endif /* FORCE_16_BITS */
    return;
}

ULONG inl(int port)
{
    ULONG data;

#ifdef FORCE_16_BITS /* assume that auto-incr is on */
    WORD data0, data1;

    data1 = *(WORD *)(port); /* high 16 bits */
    data0 = *(WORD *)(port); /* low  16 bits */
    data = (data1 << 16) | data0;
#else
    data = *(ULONG *)(port);
#endif /* FORCE_16_BITS */

    return(data);
}

VOID outw(int port,WORD data)
{
    (*(WORD *)port) = data;
}

WORD inw(int port)
{
    return(*((WORD *)port));
}
#endif /* SPARC_PCI */

#ifdef AIX_ANT
extern volatile WORD *virtual_port;

VOID outw(WORD port,WORD data)
{
    WORD *virtual_addr;

    virtual_addr = (WORD *) (virtual_port + ((port - A4_MIN_PORT)/2));

    (*virtual_addr) = data;
    __iospace_eieio();
    return;
}

WORD inw(WORD port)
{
    WORD data;
    WORD *virtual_addr;

    virtual_addr = (WORD *) (virtual_port + ((port - A4_MIN_PORT)/2));

    data = *(virtual_addr);
    __iospace_eieio();

    return(data);
}
#endif

#ifdef WNT_ANT 
/**********************************************************************
 **  Redefinition of inw(...) and outw(...) for NT 
 **********************************************************************/
USHORT inw(WORD port)
{
    PVOID   ntport;

    if (port >= GN_MAX_PHYS_IO_PORT) {
	dlgn_msg(CE_NOTE,"inw(): Bad Port number %x\n",port);
	return(0);
    }

   /* get virtual port address from our internal map */
    if ((ntport = (PVOID) GnPhysToVirtIoPortMap[port]) == 0) {
	dlgn_msg(CE_NOTE,"inw(): IO Port Map not initialized for port %x\n",
                 port);
	return(0);
    }

   /* return the short to caller */
    return(READ_PORT_USHORT(ntport));
}

VOID outw(WORD port, WORD data)
{
    PVOID   ntport;

    if (port > GN_MAX_PHYS_IO_PORT) {
	dlgn_msg(CE_NOTE,"outw(): Bad Port number %x\n",port);
	return;
    }

   /* get virtual port address from our internal map */
    if ((ntport = (PVOID) GnPhysToVirtIoPortMap[port]) == 0) {
	dlgn_msg(CE_NOTE,"outw(): IO Port Map not initialized for port %x\n",
                 port);
	return;
    }

   /* write the data */
    WRITE_PORT_USHORT(ntport,data);
}
#endif

#ifdef LFS
ULONG get_virt_port(ushort port);
#endif
VOID aoutpl(WORD port, ULONG data)
{
    int portaddr;

#ifdef BIG_ANT
    portaddr = (int)(get_virt_port(port));	/* 16 or 32 bit mode */
#elif defined PCI_ANT /* LE with PCI support */
    if (!(port & _A4_16BIT_PORT)) {	/* 32 bit port, i.e. PCI */
        portaddr = (int)(get_virt_port(port));
    }
    else {	/* 16 bit ISA port */
        portaddr = (int)(port);
    }
#else /* LE with ISA only support */
    {	/* 16 bit ISA port */
        portaddr = (int)(port);
    }
#endif /* BIG_ANT */

#ifdef BIG_ANT
    if ((port & _A4_NO_SWAP) == 0) {
        outl(portaddr,LBYTESWAP(data));
    } else {
        outl(portaddr,data);
    }
#else
#ifdef FORCE_16_BITS
    outw(portaddr, (data & 0xffff)); /* low  16 bits */
    outw(portaddr, (data >> 16)); /* high 16 bits */
#else
#ifdef LINUX
    outl(data,portaddr);
#else
    outl(portaddr,data);
#endif /* LINUX */
#endif /* FORCE_16_BITS */
#endif
}

ULONG ainpl(WORD port)
{
    ULONG data;
    int portaddr;

#ifdef BIG_ANT
    portaddr = (int)(get_virt_port(port));	/* 16 or 32 bit mode */
#elif defined PCI_ANT /* LE with PCI support */
    if (!(port & _A4_16BIT_PORT)) {	/* 32 bit port, i.e. PCI */
        portaddr = (int)(get_virt_port(port));
    }
    else {	/* 16 bit ISA port */
        portaddr = (int)(port);
    }
#else /* LE with ISA only support */
    {	/* 16 bit ISA port */
        portaddr = (int)(port);
    }
#endif /* BIG_ANT */

#ifdef BIG_ANT
    data = inl(portaddr);
    if ((port & _A4_NO_SWAP) == 0) {
        data = LBYTESWAP(data);
    }
#else
#ifdef FORCE_16_BITS
    {
	WORD data0, data1;

    	data0 = inw(portaddr); /* low  16 bits */
    	data1 = inw(portaddr); /* high 16 bits */
    	data = (data1 << 16) | data0;
    }
#else
    data = inl(portaddr);
#endif /* FORCE_16_BITS */
#endif

    return(data);
}

VOID aoutpw(WORD port, WORD data)
{
    int portaddr;


#ifdef BIG_ANT
    portaddr = (int)(get_virt_port(port));	/* 16 or 32 bit mode */
#elif defined PCI_ANT /* LE with PCI support */
    if (!(port & _A4_16BIT_PORT)) {	/* 32 bit port, i.e. PCI */
        portaddr = (int)(get_virt_port(port));
    }
    else {	/* 16 bit ISA port */
        portaddr = (int)(port);
    }
#else /* LE with ISA only support */
    {	/* 16 bit ISA port */
        portaddr = (int)(port);
    }
#endif /* BIG_ANT */

#ifdef BIG_ANT
    if ((port & _A4_NO_SWAP) == 0) {
        outw(portaddr,WBYTESWAP(data));
    } else {
        outw(portaddr,data);
    }
#else
#ifdef LINUX
    outw(data, portaddr);
#else
    outw(portaddr,data);
#endif /* LINUX */
#endif
}

WORD ainpw(WORD port)
{
    WORD data;
    int portaddr;


#ifdef BIG_ANT
    portaddr = (int)(get_virt_port(port));	/* 16 or 32 bit mode */
#elif defined PCI_ANT /* LE with PCI support */
    if (!(port & _A4_16BIT_PORT)) {	/* 32 bit port, i.e. PCI */
        portaddr = (int)(get_virt_port(port));
    }
    else {	/* 16 bit ISA port */
        portaddr = (int)(port);
    }
#else /* LE with ISA only support */
    {	/* 16 bit ISA port */
        portaddr = (int)(port);
    }
#endif /* BIG_ANT */

#ifdef BIG_ANT
    data = inw(portaddr);
    if ((port & _A4_NO_SWAP) == 0) {
        data = WBYTESWAP(data);
    }
#else
    data = inw(portaddr);
#endif
    return(data);
}

#ifdef BIG_ANT 
repinsd(WORD port, ULONG *buf, WORD size)
{
    int i;

    for (i = 0; i < size; i++) {
        buf[i] = ainpl(port);
    }
}

repoutsd(WORD port,ULONG *buf,WORD size)
{
    int i;

    for (i = 0; i < size; i++) {
        aoutpl(port,buf[i]);
    }
}

repinsw(WORD port, WORD *buf, WORD size)
{
    int i;


    for (i = 0 ; i < size; i += 2) {
        if (port & _A4_NO_SWAP) {
            buf[i] = ainpw(port);
            buf[i+1] = ainpw(port);
        } else { /* 16 to 32 bit swap */
            buf[i+1] = ainpw(port);
            buf[i] = ainpw(port);
        }
    }
}

repoutsw(WORD port,WORD *buf,WORD size)
{
    int i;


    for (i = 0 ; i < size; i += 2) {
        if (port & _A4_NO_SWAP) {
            aoutpw(port,buf[i]);
            aoutpw(port,buf[i+1]);
        } else { /* 16 to 32 bit swap */
            aoutpw(port,buf[i+1]);
            aoutpw(port,buf[i]);
        }
    }
}
#endif /* BIG_ANT */

#ifdef LINUX
#ifdef LFS
void
#endif
repinsd(WORD port, ULONG *buf, WORD size)
{
    int i;

    for (i = 0; i < size; i++) {
        buf[i] = ainpl(port);
    }  
}

#ifdef LFS
void
#endif
repoutsd(WORD port,ULONG *buf,WORD size)
{
    int i;

    for (i = 0; i < size; i++) {
        aoutpl(port,buf[i]);
    }  
}

#ifdef LFS
void
#endif
repinsw(WORD port, WORD *buf, WORD size)
{
    int i;


    for (i = 0 ; i < size; i += 2) {
            buf[i] = ainpw(port);
            buf[i+1] = ainpw(port);
    }  
}

#ifdef LFS
void
#endif
repoutsw(WORD port,WORD *buf,WORD size)
{
    int i;


    for (i = 0 ; i < size; i += 2) {
            aoutpw(port,buf[i]);
            aoutpw(port,buf[i+1]);
    }
}      
#endif /* LINUX */

#ifdef WNT_ANT
VOID repinsw(int port, PUSHORT Buffer, ULONG Count)
{
    PVOID   ntport;

    if (port >= GN_MAX_PHYS_IO_PORT) {
	dlgn_msg(CE_NOTE,"repinsw(): Bad Port number %x\n",port);
	return;
    }

   /* get virtual port address from our internal map */
    if ((ntport = (PVOID)  GnPhysToVirtIoPortMap[port]) == 0) {
	dlgn_msg(CE_NOTE,"repinsw(): IO Port Map not initialized for port %x\n",
                 port);
	return;
    }

   /* return the short to caller */
    READ_PORT_BUFFER_USHORT(ntport,Buffer,Count);

    return;
}

VOID repoutsw(int port, PUSHORT Buffer, ULONG Count)
{
    PVOID   ntport;

    if (port >= GN_MAX_PHYS_IO_PORT) {
	dlgn_msg(CE_NOTE,"repoutsw(): Bad Port number %x\n",port);
	return;
    }

   /* get virtual port address from our internal map */
    if ((ntport = (PVOID) GnPhysToVirtIoPortMap[port]) == 0) {
	dlgn_msg(CE_NOTE,"repoutsw(): IO Port Map not initialized for port %x\n"
                 ,port);
	return;
    }

   /* return the short to caller */
    WRITE_PORT_BUFFER_USHORT(ntport,Buffer,Count);

    return;
}
#endif /* WNT_ANT */

VOID a4_auto_read(WORD port, WORD size, ULONG addr, WORD *buf)
{
    int portaddr;
    WORD haddr, laddr;
#ifdef WNT_ANT
    KIRQL oldspl;
#else
    int oldspl;
#endif

#ifdef AIX_ANT
#else
#ifdef SOL_ANT
#if 0  /* SMP FIX */
    if ((!in_inter) && (!in_mutex)) {
        mutex_enter(&inthw_lock);
    }
#endif 
#else
    DLGCSPLSTR(oldspl);
#endif
#endif
    haddr = (WORD) ((addr >> 16) & 0xffff);
    laddr = (WORD) (addr & 0xffff);
#ifdef BIG_ANT
    port += A4_HIGH_ADDRESS;
    aoutpw(port&~_A4_NO_SWAP,haddr);
    port -= A4_SUB_ADDRESS;
    aoutpw(port&~_A4_NO_SWAP,laddr);
#else
    port += A4_HIGH_ADDRESS;
    aoutpw(port,haddr);
    port -= A4_SUB_ADDRESS;
    aoutpw(port,laddr);
#endif
    port -= A4_SUB_ADDRESS;

/* ainp routines will get right address for BIG_ANT */
/* on x86 os routines get called straight; need right addr for PCI */
#if (!defined(BIG_ANT) && defined(PCI_ANT) && !defined(LINUX))
    if (!(port & _A4_16BIT_PORT)) {	/* 32 bit port, i.e. PCI */
        portaddr = (int)(get_virt_port(port));
    }
    else
#endif /* not BIG_ANT & PCI_ANT */
    {	/* 16 bit ISA port */
        portaddr = (int)(port);	/* no _A4_NO_SWAP on little endians */
    }

#ifdef PCI_ANT
    if (!(port & _A4_16BIT_PORT))	/* Use 32-bit i/o for PCI board */
#ifdef FORCE_16_BITS
        repinsw(portaddr, buf, 2*size);
#else
        repinsd(portaddr, (ULONG *)buf, size);
#endif /* FORCE_16_BITS */
    else
#endif /* PCI_ANT */
        repinsw(portaddr, buf, 2*size);

#ifdef AIX_ANT
#else
#ifdef SOL_ANT
#if 0  /* SMP FIX */
    if ((!in_inter) && (!in_mutex)) {
        mutex_exit(&inthw_lock);
    }
#endif
#else
    DLGCSPLX(oldspl);
#endif
#endif
    return;
}

VOID a4_auto_write(WORD port, WORD size, ULONG addr, WORD *buf)
{
    int portaddr;
    WORD haddr, laddr;
#ifdef WNT_ANT
    KIRQL oldspl;
#else
    int oldspl;
#endif

#ifdef AIX_ANT
#else
#ifdef SOL_ANT
#if 0
    if ((!in_inter) && (!in_mutex)) {
        mutex_enter(&inthw_lock);
    }
#endif
#else
    DLGCSPLSTR(oldspl);
#endif
#endif
    haddr = (WORD) ((addr >> 16) & 0xffff);
    laddr = (WORD) (addr & 0xffff);
#ifdef BIG_ANT
    port += A4_HIGH_ADDRESS;
    aoutpw(port&~_A4_NO_SWAP,haddr);
    port -= A4_SUB_ADDRESS;
    aoutpw(port&~_A4_NO_SWAP,laddr);
#else
    port += A4_HIGH_ADDRESS;
    aoutpw(port,haddr);
    port -= A4_SUB_ADDRESS;
    aoutpw(port,laddr);
#endif
    port -= A4_SUB_ADDRESS;

/* aoutp routines will get right address for BIG_ANT */
/* on x86 os routines get called straight; need right addr for PCI */
#if (!defined(BIG_ANT) && defined(PCI_ANT) && !defined(LINUX))
    if (!(port & _A4_16BIT_PORT)) {	/* 32 bit port, i.e. PCI */
        portaddr = (int)(get_virt_port(port));
    }
    else
#endif /* not BIG_ANT & PCI_ANT */
    {	/* 16 bit ISA port */
        portaddr = (int)(port);	/* no _A4_NO_SWAP on little endians */
    }

    if (!(port & _A4_16BIT_PORT))	/* Use 32-bit i/o for PCI board */
#ifdef FORCE_16_BITS
        repoutsw(portaddr, buf, 2*size);
#else
        repoutsd(portaddr, (ULONG *)buf, size);
#endif /* FORCE_16_BITS */
    else
        repoutsw(portaddr, buf, 2*size);

#ifdef AIX_ANT
#else
#ifdef SOL_ANT
#if 0
    if ((!in_inter) && (!in_mutex)) {
        mutex_exit(&inthw_lock);
    }
#endif
#else
    DLGCSPLX(oldspl);
#endif
#endif
    return;
}

VOID a4_write(WORD port, WORD size, ULONG addr, WORD *buf)
{
    cmn_err(CE_WARN,"Antares hardware problem using port 0x%X",port);
}

VOID a4_read(WORD port, WORD size, ULONG addr, WORD *buf)
{
    cmn_err(CE_WARN,"Antares hardware problem using port 0x%X",port);
}

VOID wctrl_isr(WORD port, BYTE data)
{
    WORD jdata, idata;

    idata = (WORD) ainpw(port);
    idata &= 0x00FF;
    jdata = (WORD) (data & 0xff);
    jdata <<= 8;
    idata |= jdata;
    aoutpw(port,idata);

    return;
}

VOID wctrl(WORD port, BYTE data)
{
#ifdef WNT_ANT
    KIRQL oldspl;
#else
    int oldspl;
#endif

#ifdef AIX_ANT
#else
#ifdef SOL_ANT
#if 0
    if ((!in_inter) && (!in_mutex)) {
        mutex_enter(&inthw_lock);
    }
#endif
#else
    DLGCSPLSTR(oldspl);
#endif
#endif

    wctrl_isr(port, data);

#ifdef AIX_ANT
#else
#ifdef SOL_ANT
#if 0
    if ((!in_inter) && (!in_mutex)) {
        mutex_exit(&inthw_lock);
    }
#endif
#else
    DLGCSPLX(oldspl);
#endif
#endif
    return;
}

VOID wask_isr(WORD port, BYTE data)
{
    WORD jdata, idata;

    idata = (WORD) ainpw(port);
    idata &= 0xff;
    jdata = (WORD) (data & 0xff);
    idata |= jdata;

    aoutpw(port,idata);

    return;
}

VOID wask(WORD port, BYTE data)
{
#ifdef WNT_ANT
    KIRQL oldspl;
#else
    int oldspl;
#endif

#ifdef AIX_ANT
#else
#ifdef SOL_ANT
#if 0
    if ((!in_inter) && (!in_mutex)) {
        mutex_enter(&inthw_lock);
    }
#endif
#else
    DLGCSPLSTR(oldspl);
#endif
#endif

    wask_isr(port, data);

#ifdef AIX_ANT
#else
#ifdef SOL_ANT
#if 0
    if ((!in_inter) && (!in_mutex)) {
        mutex_exit(&inthw_lock);
    }
#endif
#else
    DLGCSPLX(oldspl);
#endif
#endif
    return;
}

VOID wfree_isr(WORD port, BYTE data)
{
    WORD jdata, idata;

    idata = (WORD) ainpw(port);
    jdata = (WORD) (data & 0xff);
    idata &= jdata;
    aoutpw(port,idata);

    return;
}

VOID wfree(WORD port, BYTE data)
{
#ifdef WNT_ANT
    KIRQL oldspl;
#else
    int oldspl;
#endif

#ifdef AIX_ANT
#else
#ifdef SOL_ANT
#if 0
    if ((!in_inter) && (!in_mutex)) {
        mutex_enter(&inthw_lock);
    }
#endif
#else
    DLGCSPLSTR(oldspl);
#endif
#endif

    wfree_isr(port, data);

#ifdef AIX_ANT
#else
#ifdef SOL_ANT
#if 0
    if ((!in_inter) && (!in_mutex)) {
        mutex_exit(&inthw_lock);
    }
#endif
#else
    DLGCSPLX(oldspl);
#endif
#endif
    return;
}

WORD is_bit_isr(WORD port, WORD data)
{
    WORD idata;

    idata = (WORD) ainpw(port);
    idata &= data;

    return(idata);
}

WORD is_bit(WORD port, WORD data)
{
    WORD idata;

#ifdef WNT_ANT
    KIRQL oldspl;
#else
    int oldspl;
#endif

#ifdef AIX_ANT
#else
#ifdef SOL_ANT
#if 0
    if ((!in_inter) && (!in_mutex)) {
        mutex_enter(&inthw_lock);
    }
#endif
#else
    DLGCSPLSTR(oldspl);
#endif
#endif

    idata = is_bit_isr(port, data);

#ifdef AIX_ANT
#else
#ifdef SOL_ANT
#if 0
    if ((!in_inter) && (!in_mutex)) {
        mutex_exit(&inthw_lock);
    }
#endif
#else
    DLGCSPLX(oldspl);
#endif
#endif
    return(idata);
}

