/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msddbg.c
 * Description                  : Driver Debug
 *
 *
 **********************************************************************/

#include "msd.h"
#define _MSDDBG_C_
#include "msdextern.h"
#undef _MSDDBG_C_

/*
 * Local variables
 */

static int i_bperline = 16;
#define RMARGIN 80
#define BPERLINE 16
#undef  isprint
#define isprint(C)      (((C) >= 0x20) && ((C) < 0x7F))
static char i_bprintf_Id[] = "$Id: i_bprintf.c,v 1.8 2001/08/12 05:52:06 xxxxx Exp $";

#define STRING_BUF_SIZE 1024
char *string_buf = NULL;

merc_uint_t varkdb = 0;


//============================================================
//	DEBUG FUNCTIONS
//============================================================


/******************************************************************************
 * Function Name                : is_endstring
 * Function Type                : Support Function for Utilities
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :  is_endstring -- standard sprintf function
 *                              :
 *                              :
 * Additional comments          :
 *******************************************************************************/
int
is_endstring(char *str)
{
    if (*str) return 0;
    return 1;
}

/******************************************************************************
 * Function Name                : i_sprintf
 * Function Type                : Support Function for Utilities
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :  i_sprintf -- standard sprintf function
 *                              :
 *                              :
 * Additional comments          :
 *******************************************************************************/
char *
i_sprintf(char *s, char *fmt, ... )
{
    va_list arg;
    unsigned int arg_int, used;
    char *sp, *dp;

    dp = s;
    sp = fmt;

    va_start(arg, fmt);

    for(sp = fmt; *sp; sp++) {

        if (*sp == '%') {
            for (used = convert_arg(&sp, &dp, arg); used; used--)
                arg_int = va_arg(arg, int);
            continue;
        }
            *dp++ = *sp;
    }
    *dp = 0;
    va_end(arg);
    return dp;
}

/******************************************************************************
 * Function Name                : i_bprintf
 * Function Type                : Support Function for Utilities
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :  Prints a buffer in a semi-cooked mode where
 *                              :  each byte is displayed in hexadecimal in a
 *                              :  table with the printable ascii notated on the
 *                              :  side.
 *                              :
 * Additional comments          :  abuf    - address of the buffer to print
 *                              :  size    - size in bytes of the buffer
 *******************************************************************************/

int
i_bprintf( unsigned char *abuf, unsigned int size)
{
    int       i, index, outi;
    static char ascii[80];
    static char outline[160];

    ascii[i_bperline] = 0;
    index = 0;
    i = 0;
    outi = 0;

    while (index < size) {
        i_sprintf(&(outline[outi]), "%x%x %c",
                (abuf[index] >> 4) & 0xf, abuf[index] & 0xf, 0);
        outi = strlen(outline);
        ascii[i] = (isprint(abuf[index]) ? abuf[index] : '.');
        if (++i == i_bperline) {
            i_sprintf(&(outline[outi]), "  %s\n%c", ascii, 0);
            /* osm_printmsg(outline); */
            printk(outline);
            outi = 0;
            i = 0;
        }
        index++;
    }

    if (i > 0) {
        ascii[i] = 0;
        for (; i < i_bperline; i++)
            i_sprintf(&(outline[outi]), "   %c", 0);
        i_sprintf(&(outline[strlen(outline)]), "  %s\n%c", ascii, 0);
        /* osm_printmsg(outline); */
        printk(outline);
    }
    return i_bprintf_Id[0];     /* satisfy compiler nitpicking */
}

/******************************************************************************
 * Function Name                : i_vsprintf
 * Function Type                : Support Function for Utilities
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :  
 *                              :
 *                              : 
 *                              : 
 *                              :
 * Additional comments          :  
 *                              :
 *******************************************************************************/

char *
i_vsprintf(char *s, char *fmt, va_list arg )
{
    unsigned int arg_int, used;
    char *sp, *dp;

    dp = s;
    sp = fmt;

    for(sp = fmt; *sp; sp++) {
        if (*sp == '%') {
            for (used = convert_arg(&sp, &dp, arg); used; used--)
                arg_int = va_arg(arg, int);
            continue;
        }
        *dp++ = *sp;
    }
    *dp = 0;

    return dp;
}

/******************************************************************************
 * Function Name                : convert_arg
 * Function Type                : Support Function for Utilities
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :  Conver the args to their natual format
 *                              :
 *                              :
 * Additional comments          :
 *******************************************************************************/
int
convert_arg(char **src,char **dst, va_list arg)
{
    static const char digits[]="0123456789ABCDEF";
    char            tnbuf[0x10];
    char  *tp = &tnbuf[0];
    char  *s = *src;
    char  *d = *dst;
    int    ndig = 0, res=-1, lz=0;
    int    i, argsused=0;
    unsigned int val;


    /* Skip '%' */
    s++;

try_again:
    switch (*s) {

    case 'd':
    case 'D':
        argsused=1;
        for (val = va_arg(arg, int); val; val /= 10) {
            *tp++ = digits[val%10];
            ndig++;
        }
        if (!ndig) {
            *tp++ = '0';
            ndig++;
        }
        *tp = 0;
        break;

    case 'x':
    case 'X':
        if (res == -1) res = 9;
        argsused=1;
        i = 0;
        for (val = va_arg(arg, int); i < res && val != 0; val >>= 4, i++) {
            *tp++ = digits[val&0xf];
            ndig++;
        }
        /* Removed following line.  Who knows what it was supposed to
         * accomplish, but it was definitely doing the wrong thing
         * on negative numbers.  If this sort of behavior is really
         * important to someone, you probably need to make a new format
         * type.  Perhaps %w for "wierd negative formatting".
         *
         *if (val != 0) *(tp-1) = '<';
         */
        if (!ndig) {
            *tp++ = '0';
            ndig++;
        }
        if (val == 0 && ndig < res && lz) {
            while (ndig++ < res)
                *tp++ = '0';
            ndig--;
        }
        *tp = 0;
        break;

    case 's':
    case 'S': {
        register char *src_p = va_arg(arg, char *);
        if (src_p == NULL) src_p = "[NULL]";
        while ((*d++ = *src_p++));
        *dst = --d;
        *src = s;
        return 1;
    }

    case 'c':
        *src = s;
        *d++ = (va_arg(arg, int) & 0xff);
        *d = 0;
        *dst = d;
        return 1;

    case '0': if (s == *src+1) lz = 1;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9' :  res = (res == -1 ? 0 : res * 10) + *s - '0'; s++;
                goto try_again;

                /* %b takes 2 arguments.  The first is the numerical
                 * value to be interpreted and the second is the
                 * interpretation string whose BNR specification is:
                 *
                 *    istring = [','] bitspec [',' bitspec] [',']
                 *    bitspec = ['~'] bit-number '=' namestring  |
                 *              bit-number '-' bit-number '=' fieldnamestring
                 *    bit-number = decimal number 0-32
                 *    namestring = string to print
                 *    fieldnamestring = namestring [fieldspec ...]
                 *    fieldspec = ':' fieldvalue '=' valuename
                 *    fieldvalue = decimal number >= 0
                 *    fieldvalue = decimal number >= 0
                 *    valuename = string to print if field == fieldvalue
                 *
                 * If bitspec is of the first form, then namestring is
                 * printed between '<' '>' characters if the bit is set
                 * (or not set if '~' is specified) in the first numerical
                 * value and nothing is printed otherwise.
                 * If bitspec is of the second form, then the namestring is
                 * printed followed by '=' and then the hexadecimal value
                 * of the specified range of bits (the entire field again
                 * being enclosed by '<' '>' characters.
                 */
    case 'b': {
        unsigned int val;
        int bnum, negated;
        char *xlate, *xp, startchar='<';
        val = va_arg(arg, unsigned int);
        xlate = va_arg(arg, char *);
        bnum = 0;
        negated = 0;
        for (xp = xlate; *xp; bnum = 0, negated=0) {
            if (*xp == ',') {
                xp++;
                continue;
            }
            if (*xp == '~') {
                negated = 1;
                xp++;
            }
            while (*xp != '=' && *xp != '-')
                bnum = (bnum * 10) + (*xp++ - '0');
            if (*xp == '-') {
                xp++;
                lz = 0;
                while (*xp != '=') lz = (lz * 10) + (*xp++ - '0');
                xp++;
                for (ndig = 0; lz >= bnum; lz--) ndig |= (1<<lz);
                *d++ = startchar; startchar = ',';
                while (*xp != ',' && !is_endstring(xp) && *xp != ':')
                    *d++ = *xp++;
                *d++ = '=';
                lz = (val & ndig) >> bnum;
                d = i_sprintf(d,"%X",lz);
                if (*xp == ':') {
                    while (*xp != ',' && !is_endstring(xp)) {
                        bnum = 0;
                        while (*xp != '=' && *xp != ':')
                            bnum = (bnum * 10) + (*xp++ - '0');
                        if (lz == bnum) {
                            *d++ = ':';
                            xp++;
                            while (*xp != ':' && *xp != ',' &&
                                  !is_endstring(xp))
                                *d++ = *xp++;
                        } else {
                            while (*xp != ':' && *xp != ',' &&
                                   !is_endstring(xp))
                                xp++;
                        }
                        if (*xp == ':') xp++;
                    }
                }
            } else {
                xp++;
                if (((negated == 0) &&
                     (bnum < sizeof(val)*8 && val & (1<<bnum))) ||
                    ((negated == 1) &&
                     (bnum < sizeof(val)*8 && ((val & (1<<bnum)) == 0)))) {
                    *d++ = startchar; startchar = ',';
                    while (*xp != ',' && !is_endstring(xp)) *d++ = *xp++;
                } else {
                    while (*xp != ',' && !is_endstring(xp)) xp++;
                }
            }
        }
        if (startchar != '<') *d++ = '>';
        *dst = d;
        *src = s;
        return 2;
    }

    default: break;
    }
    if (ndig) {
        for (i = 0; i < ndig; i++) {
            *d++ = *--tp;
        }
    } else
        *d++ = *s;

    *dst = d;
    *src = s;
    return argsused;
}

void
i_printmsg(char *fmt, ...)
{
     va_list arg;

    if (!varkdb)
       return;

    if (string_buf == NULL) {
        string_buf = mercd_allocator(STRING_BUF_SIZE);
        if (string_buf == NULL) {
            return;
        }
    }
   
    MSD_ZERO_MEMORY(string_buf, STRING_BUF_SIZE);
    va_start(arg, fmt);
    i_vsprintf(string_buf, fmt, arg);
    va_end(arg);
    if (string_buf[strlen(string_buf)-1] != '\n') {
        string_buf[strlen(string_buf)+1] = 0;
        string_buf[strlen(string_buf)] = '\n';
    }
    cmn_err(CE_CONT, "%s", string_buf);

}
