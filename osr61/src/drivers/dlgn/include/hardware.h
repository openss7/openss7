/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : hardware.h
 * Description                  : hardware specific
 *
 *
 **********************************************************************/

#ifdef VME_ANT
#define BRD_TYPE_OFFSET  0x33
#define MEM_TYPE_OFFSET  0x37
#define CTRL_PORT_OFFSET 0xc0
#define INTR_VEC_OFFSET  0xc3
#define A32_REG_OFFSET   0xc5
#else
/*  DSP I/O structure */
#define _A4_CTRL_PORT_OFFSET  0
#define _A4_DATA_OFFSET       2
#define _A4_ADDR_LOW_OFFSET   4
#define _A4_ADDR_HI_OFFSET    6
#endif
/*
 * Define the ANTARES control port bits
 */
#define _A4_D0RS        0x1         /* DSP0 Reset R/W */
#define _A4_D1RS        0x2         /* DSP1 Reset R/W */
#define _A4_D2RS        0x4         /* DSP2 Reset R/W */
#define _A4_D3RS        0x8         /* DSP3 Reset R/W */
#define _A4_MCBL_MP     0x10        /* MCBP/MP (Boot Mode) R/W */
#define _A4_PCMRS       0x20        /* Reset PCM interface R/W */

#if 0
/* declared in hardware.exp */
#define _A4_AIDIS       0x40        /* Auto Increment Disable R/W */
#define _A4_PCTOKREQ    0x80        /* PC Token Request R/W */
#define _A4_PCIRQ       0x100       /* DSP-to-PC Interrupt Pending R/W */
#endif

#define _A4_PCSEMA4     0x200       /* DSP-to-PC Semaphore R/O */
#define _A4_DSPSEMA4    0x400       /* PC-to-DSP Semaphore R/W */
#define _A4_PCTOKGRNT   0x800       /* PC Token Grant R/O */
#define _A4_SYNCLOSS    0x8000      /* Loss of synchronization R/O */
#define _A4_D0IRQ       0x1000      /* PC to DSP0 Interrupt W/O */
#define _A4_D1IRQ       0x2000      /* PC to DSP1 Interrupt W/O */
#define _A4_D2IRQ       0x4000      /* PC to DSP2 Interrupt W/O */
#define _A4_D3IRQ       0x8000      /* PC to DSP3 Interrupt W/O */
#define _A4_NO_DSP_IRQ  (_A4_D0IRQ | _A4_D1IRQ | _A4_D2IRQ | _A4_D3IRQ)

/*
 * The most-significant bit in the address
 * port controls whether the access fetches
 * the high 16 bits or the low 16 bits.
 * The hardware automatically toggles this
 * bit, but the PC can also manipulate it directly.
 */
#define _A4_HIGH_16BITS             0x80000000UL

#ifdef VME_ANT
#define VME_CONTROL_PORT(kva) ((WORD *) ((LONG) (kva) + (LONG)CTRL_PORT_OFFSET))
#else
/*
 * Macros to access the PC I/O ports
 */
#define A4_CONTROL_PORT(port)    ((port) + (WORD)_A4_CTRL_PORT_OFFSET)
#define A4_DATA_PORT(port)       ((port) + (WORD)_A4_DATA_OFFSET)
#define A4_LOW_ADDRESS(port)     ((port) + (WORD)_A4_ADDR_LOW_OFFSET)
#define A4_HIGH_ADDRESS(port)    ((port) + (WORD)_A4_ADDR_HI_OFFSET)
#endif
/*
 * Macros for testing the ANTARES control port bits
 */
#define A4_DSP0_RESET(ctrl)      ((ctrl) & _A4_D0RS)
#define A4_DSP1_RESET(ctrl)      ((ctrl) & _A4_D1RS)
#define A4_DSP2_RESET(ctrl)      ((ctrl) & _A4_D2RS)
#define A4_DSP3_RESET(ctrl)      ((ctrl) & _A4_D3RS)
#define A4_BOOT_LOAD(ctrl)       ((ctrl) & _A4_MCBL_MP)
#define A4_PCM_RESET(ctrl)       ((ctrl) & _A4_PCMRS)
#define A4_AUDIO_ENABLE(ctrl)    !((ctrl) & _A4_PCMRS)

#if 1
#define A4_AUTO_INCREMENT(ctrl)  !((ctrl) & _A4_AIDIS)
#endif
#if 0
#define A4_AUTO_INCREMENT(ctrl)  0
#endif

#define A4_TOKEN_REQUEST(ctrl)   ((ctrl) & _A4_PCTOKREQ)
#define A4_INTR_PENDING(ctrl)    ((ctrl) & _A4_PCIRQ)
#define A4_PC_SEMAPHORE(ctrl)    ((ctrl) & _A4_PCSEMA4)
#define A4_DSP_SEMAPHORE(ctrl)   ((ctrl) & _A4_DSPSEMA4)
#define A4_TOKEN_GRANT(ctrl)     ((ctrl) & _A4_PCTOKGRNT)
#define A4_SYNC_LOSS(ctrl)       ((ctrl) & _A4_SYNCLOSS)

/*
 * This mask should be used to access only the RESET bits
 */
#define A4_RESET_ALL   (_A4_D0RS | _A4_D1RS | _A4_D2RS | _A4_D3RS)

/*
 * A number of bits in the control port have overloaded functionality -
 *    PCIRQ    - if read as 1 indicates an interrupt is pending;
 *               if written as 1 clears the pending interrupt
 *    PCSEMA4  - if read as 1 indicates the DSP wants to send a message;
 *               a write of 1 clears this bit
 *    DSPSEMA4 - a write of 1 indicates that the PC wants to send a message;
 *               a write of 0 has no effect
 *    D?IRQ    - these bits don't exist on read,
 *               a write of 1 interrupts the corresponding DSP
 *    SYNCLOSS - read-only (overloaded on D3IRQ); this bit indicates
 *               some sort of general failure of the board
 *
 * The following macros attempt to remove side-effects
 * from a value read from a control port so that the
 * value may be safely written to the control port
 */

/*
 * No side effects
 */
#define A4_MASK_CTRL_PORT(ctrl)  ((ctrl) & ~(_A4_NO_DSP_IRQ | _A4_PCIRQ    \
                                                | _A4_PCSEMA4))

/*
 * Clear the PC semaphore, presumably because the host
 * software got the message. Note that if the PC semaphore
 * bit was not set, it will not be written.
 */
#define A4_CLEAR_SEMA4(ctrl)     ((ctrl) & ~(_A4_NO_DSP_IRQ | _A4_PCIRQ))

/*
 * Clear the interrupt request, presumably because the host
 * software got the interrupt. Note that if the IRQ
 * bit was not set, it will not be written.
 */
#define A4_CLEAR_IRQ(ctrl)       ((ctrl) & ~(_A4_NO_DSP_IRQ | _A4_PCSEMA4))

/*
 * Macros to force a particular 16 bits in a memory fetch
 */
#define A4_FORCE_LOW_16BITS(addr)   ((addr) & ~_A4_HIGH_16BITS)
#define A4_FORCE_HIGH_16BITS(addr)  ((addr) |  _A4_HIGH_16BITS)

#define A4_DEFAULT_IRQ     5
#define A4_ALTERNATE_IRQ   11
