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

#ident "@(#) LiS module-skeleton.c 2.3 09/13/04 10:12:31 "

#ifdef LINUX

static int svi_major;
char kernel_version
= UTS_RELEASE;

int init_module( void )
{
        svi_major = lis_register_strdev (0, &sviinfo, 0, "svi");
        if      (svi_major < 0)
                return -EIO;
        sviinit ();
        return  0;
}

void cleanup_module( void )
{
        lis_unregister_strdev (svi_major, &sviinfo, 0);
}

void * bzero(void * s,size_t count)
{
        char *xs = (char *) s;

        while (count--)
                *xs++ = '\0';

        return s;
}

char * bcopy(const char * src, char * dest, int count)
{
        char *tmp = dest;

        while (count--)
                *tmp++ = *src++;

        return dest;
}
#endif /* LINUX */

