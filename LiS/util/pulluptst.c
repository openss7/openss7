/************************************************************************
*                   Test Program for Pullupmsg                          *
************************************************************************/
/*
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 * MA 02139, USA.
 * 
 */

#ident "@(#) LiS pulluptst.c 2.2 12/15/02 18:00:05 "

#include <sys/stream.h>


/************************************************************************
*                             xit                                       *
*************************************************************************
*									*
* This routine is called to exit the program when a test fails.		*
*									*
************************************************************************/
void	xit(void)
{
    printf("\n\n\n");
    printf("****************************************************\n");
    printf("*                  Test Failed                     *\n");
    printf("****************************************************\n\n");

    printf("Dump of memory areas in use:\n\n") ;

    port_print_mem() ;
    exit(1) ;

} /* xit */

/************************************************************************
*                          set_debug_mask                               *
*************************************************************************
*									*
* Use stream ioctl to set the debug mask for streams.			*
*									*
************************************************************************/
void	set_debug_mask(long msk)
{
    int		fd ;
    int		rslt ;

    fd = user_open("loop.1", 0, 0) ;
    if (fd < 0)
    {
	printf("loop.1: %s\n", strerror(-fd)) ;
	xit() ;
    }

    rslt = user_ioctl(fd, I_LIS_SDBGMSK, msk) ;
    if (rslt < 0)
    {
	printf("loop.1: I_LIS_SDBGMSK: %s\n", strerror(-rslt)) ;
	xit() ;
    }

    printf("\nSTREAMS debug mask set to 0x%08lx\n", msk) ;

    user_close(fd) ;

} /* set_debug_mask */

void pullupmsg_test(void)
{
    mblk_t	*mp ;
    mblk_t	*mp2 ;
    int		 i ;

    printf("\nTwo blocks, each with 10 bytes, pullupmsg(15)\n") ;
    mp = allocb(10, BPRI_MED) ;
    if (mp == (mblk_t *) NULL)
    {
	printf("Failed to allocate message\n") ;
	xit() ;
    }

    for (i = 0; i < 10; i++)
	*mp->b_wptr++ = (unsigned char) i ;

    mp2 = allocb(10, BPRI_MED) ;
    if (mp2 == (mblk_t *) NULL)
    {
	printf("Failed to allocate message\n") ;
	xit() ;
    }

    for (i = 0; i < 10; i++)
	*mp2->b_wptr++ = (unsigned char) i ;

    mp->b_cont = mp2 ;

    lis_print_msg(mp, "before", PRINT_DATA_ENTIRE) ;
    pullupmsg(mp, 15) ;
    lis_print_msg(mp, "after", PRINT_DATA_ENTIRE) ;
    freemsg(mp) ;
    printf("\nMemory at end of test\n") ;
    port_print_mem() ;


    printf("\n\nTwo blocks, each with 80 bytes, pullupmsg(100)\n") ;
    mp = allocb(80, BPRI_MED) ;
    if (mp == (mblk_t *) NULL)
    {
	printf("Failed to allocate message\n") ;
	xit() ;
    }

    for (i = 0; i < 80; i++)
	*mp->b_wptr++ = (unsigned char) i ;

    mp2 = allocb(80, BPRI_MED) ;
    if (mp2 == (mblk_t *) NULL)
    {
	printf("Failed to allocate message\n") ;
	xit() ;
    }

    for (i = 0; i < 80; i++)
	*mp2->b_wptr++ = (unsigned char) i ;

    mp->b_cont = mp2 ;

    lis_print_msg(mp, "before", PRINT_DATA_ENTIRE) ;
    pullupmsg(mp, 100) ;
    lis_print_msg(mp, "after", PRINT_DATA_ENTIRE) ;
    freemsg(mp) ;
    printf("\nMemory at end of test\n") ;
    port_print_mem() ;


    printf("\n\nTwo blocks, each with 80 bytes, pullupmsg(-1)\n") ;
    mp = allocb(80, BPRI_MED) ;
    if (mp == (mblk_t *) NULL)
    {
	printf("Failed to allocate message\n") ;
	xit() ;
    }

    for (i = 0; i < 80; i++)
	*mp->b_wptr++ = (unsigned char) i ;

    mp2 = allocb(80, BPRI_MED) ;
    if (mp2 == (mblk_t *) NULL)
    {
	printf("Failed to allocate message\n") ;
	xit() ;
    }

    for (i = 0; i < 80; i++)
	*mp2->b_wptr++ = (unsigned char) i ;

    mp->b_cont = mp2 ;

    lis_print_msg(mp, "before", PRINT_DATA_ENTIRE) ;
    pullupmsg(mp, -1) ;
    lis_print_msg(mp, "after", PRINT_DATA_ENTIRE) ;
    freemsg(mp) ;
    printf("\nMemory at end of test\n") ;
    port_print_mem() ;

} /* pullupmsg_test */

void msgpullup_test(void)
{
    mblk_t	*mp ;
    mblk_t	*mp2 ;
    mblk_t	*mpnew ;
    int		 i ;

    printf("\nTwo blocks, each with 10 bytes, msgpullup(30) should fail\n") ;
    mp = allocb(10, BPRI_MED) ;
    if (mp == (mblk_t *) NULL)
    {
	printf("Failed to allocate message\n") ;
	xit() ;
    }

    for (i = 0; i < 10; i++)
	*mp->b_wptr++ = (unsigned char) i ;

    mp2 = allocb(10, BPRI_MED) ;
    if (mp2 == (mblk_t *) NULL)
    {
	printf("Failed to allocate message\n") ;
	xit() ;
    }

    for (i = 0; i < 10; i++)
	*mp2->b_wptr++ = (unsigned char) i ;

    mp->b_cont = mp2 ;

    lis_print_msg(mp, "before", PRINT_DATA_ENTIRE) ;
    mpnew = msgpullup(mp, 30) ;
    if (mpnew != NULL)
    {
	printf("\nERROR: msgpullup should have returned NULL\n") ;
	lis_print_msg(mp, "after", PRINT_DATA_ENTIRE) ;
	lis_print_msg(mpnew, "new", PRINT_DATA_ENTIRE) ;
	freemsg(mpnew) ;
    }
    else
	printf("msgpullup return NULL pointer correctly\n") ;

    freemsg(mp) ;
    printf("\nMemory at end of test\n") ;
    port_print_mem() ;


    printf("\nTwo blocks, each with 10 bytes, msgpullup(15)\n") ;
    mp = allocb(10, BPRI_MED) ;
    if (mp == (mblk_t *) NULL)
    {
	printf("Failed to allocate message\n") ;
	xit() ;
    }

    for (i = 0; i < 10; i++)
	*mp->b_wptr++ = (unsigned char) i ;

    mp2 = allocb(10, BPRI_MED) ;
    if (mp2 == (mblk_t *) NULL)
    {
	printf("Failed to allocate message\n") ;
	xit() ;
    }

    for (i = 0; i < 10; i++)
	*mp2->b_wptr++ = (unsigned char) i ;

    mp->b_cont = mp2 ;

    lis_print_msg(mp, "before", PRINT_DATA_ENTIRE) ;
    mpnew = msgpullup(mp, 15) ;
    lis_print_msg(mp, "after", PRINT_DATA_ENTIRE) ;
    lis_print_msg(mpnew, "new", PRINT_DATA_ENTIRE) ;
    freemsg(mp) ;
    freemsg(mpnew) ;
    printf("\nMemory at end of test\n") ;
    port_print_mem() ;


    printf("\n\nTwo blocks, each with 80 bytes, msgpullup(100)\n") ;
    mp = allocb(80, BPRI_MED) ;
    if (mp == (mblk_t *) NULL)
    {
	printf("Failed to allocate message\n") ;
	xit() ;
    }

    for (i = 0; i < 80; i++)
	*mp->b_wptr++ = (unsigned char) i ;

    mp2 = allocb(80, BPRI_MED) ;
    if (mp2 == (mblk_t *) NULL)
    {
	printf("Failed to allocate message\n") ;
	xit() ;
    }

    for (i = 0; i < 80; i++)
	*mp2->b_wptr++ = (unsigned char) i ;

    mp->b_cont = mp2 ;

    lis_print_msg(mp, "before", PRINT_DATA_ENTIRE) ;
    mpnew = msgpullup(mp, 100) ;
    lis_print_msg(mp, "after", PRINT_DATA_ENTIRE) ;
    lis_print_msg(mpnew, "new", PRINT_DATA_ENTIRE) ;
    freemsg(mp) ;
    freemsg(mpnew) ;
    printf("\nMemory at end of test\n") ;
    port_print_mem() ;


    printf("\n\nTwo blocks, each with 80 bytes, msgpullup(-1)\n") ;
    mp = allocb(80, BPRI_MED) ;
    if (mp == (mblk_t *) NULL)
    {
	printf("Failed to allocate message\n") ;
	xit() ;
    }

    for (i = 0; i < 80; i++)
	*mp->b_wptr++ = (unsigned char) i ;

    mp2 = allocb(80, BPRI_MED) ;
    if (mp2 == (mblk_t *) NULL)
    {
	printf("Failed to allocate message\n") ;
	xit() ;
    }

    for (i = 0; i < 80; i++)
	*mp2->b_wptr++ = (unsigned char) i ;

    mp->b_cont = mp2 ;

    lis_print_msg(mp, "before", PRINT_DATA_ENTIRE) ;
    mpnew = msgpullup(mp, -1) ;
    lis_print_msg(mp, "after", PRINT_DATA_ENTIRE) ;
    lis_print_msg(mpnew, "new", PRINT_DATA_ENTIRE) ;
    freemsg(mp) ;
    freemsg(mpnew) ;


    printf("\n\nThree blocks, each with 80 bytes, msgpullup(100)\n") ;
    mp = allocb(80, BPRI_MED) ;
    if (mp == (mblk_t *) NULL)
    {
	printf("Failed to allocate message\n") ;
	xit() ;
    }

    for (i = 0; i < 80; i++)
	*mp->b_wptr++ = (unsigned char) i ;

    mp2 = allocb(80, BPRI_MED) ;
    if (mp2 == (mblk_t *) NULL)
    {
	printf("Failed to allocate message\n") ;
	xit() ;
    }

    for (i = 0; i < 80; i++)
	*mp2->b_wptr++ = (unsigned char) i ;

    mp->b_cont = mp2 ;

    mp2 = allocb(80, BPRI_MED) ;
    if (mp2 == (mblk_t *) NULL)
    {
	printf("Failed to allocate message\n") ;
	xit() ;
    }

    for (i = 0; i < 80; i++)
	*mp2->b_wptr++ = (unsigned char) i ;

    mp->b_cont->b_cont = mp2 ;

    lis_print_msg(mp, "before", PRINT_DATA_ENTIRE) ;
    mpnew = msgpullup(mp, 100) ;
    lis_print_msg(mp, "after", PRINT_DATA_ENTIRE) ;
    lis_print_msg(mpnew, "new", PRINT_DATA_ENTIRE) ;
    freemsg(mp) ;
    freemsg(mpnew) ;
    printf("\nMemory at end of test\n") ;
    port_print_mem() ;


    printf("\nMemory at end of test\n") ;
    port_print_mem() ;

} /* msgpullup_test */

void main(void)
{
    port_init() ;			/* stream head init routine */
    make_nodes() ;
    set_debug_mask(0xFFFFFFFF) ;

    pullupmsg_test();
    msgpullup_test();

}
