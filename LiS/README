		    Linux STREAMS Package

		          L i S

NOTE

This is a very abbreviated version of the Lis documenation.  The complete
documentation is now in html format.  It comes with the LiS distribution
in the directory htdocs.  It is available over the Internet at
http://www.gcom.com/LiS.

INSTALLATION

We recommend unpacking the tar archive in the directory /usr/src.
While logged in as root, use the following command:

    tar xzf LiS-2.x.tgz  ('x' is the specific LiS version extension)

It will make a subdirectory named LiS-2.7

Change directories to LiS-2.x and type in "make".  The LiS package
will install itself.  After the make completes, type in "make install"
and LiS will install all the modules and utility programs in the
proper places on your system.


KERNEL COMPATIBILITY

This version of LiS will install in any kernel version from 2.2.x
onward (through 2.4.x).  LiS no longer supports 2.0.x kernels.
It is best when used with 2.4.x kernels.

The LiS software resides outside the kernel source tree.  It runs
as a loadable module.

SPARC PORT OF LIS (Could be obsolete)

This is compliments of Jürgen Magin <juergen.magin@octogon.de>

RedHat Linux 5.2 (UltraPenguin 1.1.9, kernel 2.2.2) on SparcStation5

  In this kernel version there is a problem with some symbols,
  which are included in the kernel but not exported.
  First look in /proc/ksyms after < mmu_p2v >. If it´s
  not included, there is a patch in the Kernel recommended. 
  In the file

	/usr/src/linux/arch/sparc/kernel/sparc_ksyms.c 

  you have to add the line: ( line 176 )

	EXPORT_SYMBOL_NOVERS(BTFIXUP_CALL(mmu_p2v)); 

  (In newer kernel version it may be exported) and in the file 

	/usr/include/asm/unistd.h

  add the lines:

	#define __NR_getpmsg            151    /* LiS specific */
	#define __NR_putpmsg            152    /* LiS specific */
	#define __NR_poll               153    /* LiS specific */
	#define __NR_streams            154    /* LiS specific */

  and then compile a new kernel. For the sparc there is a 
  collision in head/syscalls.c. The number 188 and 189 are used by
  the kernel for < __NR_sysconf > and < __NR_uname >. 


David Grothe
<dave@gcom.com>
