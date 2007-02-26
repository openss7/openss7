#!/bin/bash
#**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
# Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1.      Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#
# 2.      Redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in
# the documentation and/or other materials provided with the
# distribution.
#
# 3.      Neither the name Intel Corporation nor the names of its contributors
# may be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
#####################################################################
#
#    Script to Build and Install Intel(R) Dialogic(R)
#    and Intel(R) NetStructure(TM) driver on Linux
#
#####################################################################

# Check for file containing our environment variables.
# If it is not found, abort the configuration.
OURVARS=/etc/profile.d/ct_intel.sh
if [ ! -x ${OURVARS} ]
then
   echo "$0: FATAL ERROR: Unable to find ${OURVARS}."
   echo "$0: FATAL ERROR: Configuration aborted due to invalid installation."
   echo "$0: FATAL ERROR: Please reinstall the software by running install.sh."
   exit 1
fi

# get our environment variables
. /etc/profile.d/ct_intel.sh

# GLOBAL ENVIRONMENT VARIABLES
LiS_HOME=/usr/src/LiS
LIS_DRV_DIR=/usr/src/LiS/modules/
DRV_LOG=${INTEL_DIALOGIC_LOG}/drv.log
INTEL_DIALOGIC_DRV_DIR=${INTEL_DIALOGIC_DIR}/drivers

######################################################################
# Check if user is root
######################################################################
check_root()
{
   if [ `id -u` != 0 ]
   then
      echo This program requires you to be a Superuser.
      echo Please log in as root and and re-run $0.
      exit 1
   fi
}

######################################################################
# Check if LiS is installed
######################################################################
check_lis()
{
   if [ ! -f /lib/libLiS.so -a ! -f /usr/lib/libLiS.so ]
   then
      echo "The Intel(R) Drivers requires Linux Streams (LiS) software to be installed."
      echo
      echo "Please install LiS and then rerun $0."
      echo
      exit 1
   fi

   # Verify LiS was compiled
   if [ ! -f /lib/modules/${KERNEL_VER}/misc/streams.${OBJ} ]
   then
      echo "The Intel(R) Drivers requires Linux Streams (LiS) software to be installed."
      echo
      echo "Please install LiS and then rerun config.sh"
      echo
      exit 1
   fi

   # The kernel misc driver source directory.
   KERNEL_DRV_DIR=/lib/modules/${KERNEL_VER}/misc
}

######################################################################
# check for gcc compiler is installed.
######################################################################
check_for_gcc()
{
   # invoke gcc for version directing output to /dev/null.
   gcc -v &> /dev/null
   RC=$?

   # Check if the gcc invoke passed.
   if [ ${RC} -ne 0 ]
   then
      echo "ERROR: gcc compiler was not found."
      echo "Please install the gcc compilier and re-run $0."
      exit 1
   fi
}

######################################################################
# check for the kernel source directory is installed.
######################################################################
check_for_kernel_source()
{
   # Get kernel version and isolate major and minor numbers
   VERSION=`uname -r`
   MOD_DIR=/lib/modules/${VERSION}/misc
   K_MAJOR=${VERSION%%.*}
   K_MINTMP=${VERSION##$K_MAJOR.}
   K_MINOR=${K_MINTMP%%.[0-9]*}

   # Get the kernel version running.
   KERNEL_VER=`uname -r`

   # The kernel source directory.
   KERNEL_SRC_DIR=/lib/modules/${KERNEL_VER}/build/include

   # Check the existence of kernel source
   if [ ! -d ${KERNEL_SRC_DIR} ]
   then
      echo "ERROR: The kernel source directory ${KERNEL_SRC_DIR} not found."
      exit 1
   fi

   # Check if the kernel source is configured
   if [ ! -e ${KERNEL_SRC_DIR}/linux/autoconf.h ]
   then
      echo "ERROR: The kernel source directory is not configured."
      echo "ERROR: Missing ${KERNEL_SRC_DIR}/linux/autoconf.h file."
      exit 1
   fi

   # Assume K_MAJOR is 2 for now so just check K_MINOR
   case ${K_MINOR} in
     4)
        # 2.4 kernel
        OBJ=o
        ;;
     6)
        # 2.6 kernel
        OBJ=ko
        ;;
     *)
        # Error unknown
        echo "ERROR: Unknown Linux kernel minor version ${K_MINOR}"
        exit 1
        ;;
   esac
}

#####################################################################
# Build and Install the ctimod driver.
#####################################################################
bld_ins_ctimod()
{
   # Save the current directory.
   SAVE_DIR=`pwd`

   # change to the ctimod source directory to build and install it.
   cd ${INTEL_DIALOGIC_DRV_DIR}/ctimod

   # Build the ctimod drivers.
   echo "Building ctimod driver..."
   make >> ${DRV_LOG} 2>&1

   # Save the return code from the make.
   RC=$?

   # Revert back to the saved orignal directory.
   cd ${SAVE_DIR}

   # Perform error handling of make.
   if [ ${RC} -ne 0 ]
   then
      echo "ERROR: Failed to build and install the ctimod driver."
      exit 1
   fi

   # The kernel misc driver source directory.
   KERNEL_DRV_DIR=/lib/modules/${KERNEL_VER}/misc
}

#####################################################################
# Build and Install the DM3 driver.
#####################################################################
bld_ins_mercd()
{
   # Save the current directory.
   SAVE_DIR=`pwd`

   # change to the mercd source directory to build and install it.
   cd ${INTEL_DIALOGIC_DRV_DIR}/mercd

   # Build the mercd drivers.
   echo "Building mercd driver..."
   make >> ${DRV_LOG} 2>&1

   # Save the return code from the make.
   RC=$?

   # Revert back to the saved orignal directory.
   cd ${SAVE_DIR}

   # Perform error handling of make.
   if [ ${RC} -ne 0 ]
   then
      echo "ERROR: Failed to build and install the DM3 driver."
      BUILD_CHECK=1
      return
   fi

   # Check for existing file
   if [ -e ${KERNEL_DRV_DIR}/mercd.${OBJ} ]
   then
      # Check md5sum - necessary for a reload
      MDNEW=`/usr/bin/md5sum ${INTEL_DIALOGIC_DRV_DIR}/mercd/mercd.${OBJ} 2>/dev/null | awk '{ print $1 }'`
      MDORG=`/usr/bin/md5sum ${KERNEL_DRV_DIR}/mercd.${OBJ} 2>/dev/null | awk '{ print $1 }'`
      if [ "${MDNEW}" != "${MDORG}" ]
      then
          # Copy the driver to the system modules directory
          cp -f  ${INTEL_DIALOGIC_DRV_DIR}/mercd/mercd.${OBJ} ${KERNEL_DRV_DIR}
          /sbin/depmod -a &> /dev/null
      fi
   else
          # Copy the driver to the system modules directory
          cp -f  ${INTEL_DIALOGIC_DRV_DIR}/mercd/mercd.${OBJ} ${KERNEL_DRV_DIR}
          /sbin/depmod -a &> /dev/null
   fi

   # Check if driver is loaded
   grep mercd /proc/devices > /dev/null
   RC=$?
   if [ $RC -eq 1 ]
   then
      ${INTEL_DIALOGIC_BIN}/drvload dm3 > /dev/null
      return
   fi

   # Reload driver if idle
   if [ ! -f ${INTEL_DIALOGIC_BIN}/listboards ]
   then
      return
   fi 

   STATES="-e CONFIGED -e STARTED -e DOWNLOADED -e MAPPED -e CRASHED -e  SUSPENDED -e PROBED -e ???? -e OUT"   
   ${INTEL_DIALOGIC_BIN}/listboards | grep ${STATES} > /dev/null 2>&1
   RC=$?
   if [ $RC -ne 0 ]
   then
      # Driver is inactive, reload
      ${INTEL_DIALOGIC_BIN}/drvunload dm3 > /dev/null
      ${INTEL_DIALOGIC_BIN}/drvload dm3 > /dev/null
   else
      # Driver is active, have user restart
      echo "DM3 driver modified and needs to be reloaded."
      DRIVERS_REQ_LOADING=1
   fi
}

#####################################################################
# Build and Install the PMAC driver.
#####################################################################
bld_ins_pmacd()
{
   # Save the current directory.
   SAVE_DIR=`pwd`

   # change to the mercd source directory to build and install it.
   cd ${INTEL_DIALOGIC_DRV_DIR}/pmacd

   # Build the pmac drivers.
   echo "Building pmac driver..."
   make >> ${DRV_LOG} 2>&1

   # Save the return code from the make.
   RC=$?

   # Revert back to the saved orignal directory.
   cd ${SAVE_DIR}

   # Perform error handling of make.
   if [ ${RC} -ne 0 ]
   then
      echo "ERROR: Failed to build and install the PMAC driver."
      BUILD_CHECK=1
      return
   fi

   # Check for existing file
   if [ -e ${KERNEL_DRV_DIR}/pmac.${OBJ} ]
   then
      # Check md5sum - necessary for a reload
      MDNEW=`/usr/bin/md5sum ${INTEL_DIALOGIC_DRV_DIR}/pmacd/pmac.${OBJ} 2>/dev/null | awk '{ print $1 }'`
      MDORG=`/usr/bin/md5sum ${KERNEL_DRV_DIR}/pmac.${OBJ} 2>/dev/null | awk '{ print $1 }'`
      if [ "${MDNEW}" != "${MDORG}" ]
      then
         # Copy the driver to the system modules directory
         cp -f  ${INTEL_DIALOGIC_DRV_DIR}/pmacd/pmac.${OBJ} ${KERNEL_DRV_DIR}
         /sbin/depmod -a &> /dev/null
      fi
   else
         # Copy the driver to the system modules directory
         cp -f  ${INTEL_DIALOGIC_DRV_DIR}/pmacd/pmac.${OBJ} ${KERNEL_DRV_DIR}
         /sbin/depmod -a &> /dev/null
   fi

   # Check if driver is loaded
   grep pmac /proc/devices > /dev/null
   RC=$?
   if [ $RC -eq 1 ]
   then
      ${INTEL_DIALOGIC_BIN}/drvload pmac > /dev/null
      return
   fi

   # Reload driver if idle
   if [ ! -f ${INTEL_DIALOGIC_BIN}/pmacadmin ]
   then
      return
   fi 
	
   ${INTEL_DIALOGIC_BIN}/pmacadmin -l  | grep "No PMAC boards found." &> /dev/null
   RC=$?
   if [ $RC -eq 0 ]
   then
	   return
   fi

   STATES="-e Reset -e Failed -e Configured -e Running -e Quiesced -e Boot -e Stopped -e Null"
   ${INTEL_DIALOGIC_BIN}/pmacadmin -l  | grep ${STATES} > /dev/null
   RC=$?
   if [ $RC -eq 0 ]
   then
      # Driver is inactive, reload
      ${INTEL_DIALOGIC_BIN}/drvunload pmac > /dev/null
      ${INTEL_DIALOGIC_BIN}/drvload pmac > /dev/null
   else
      # Driver is active, have user restart
      echo "PMAC driver modified and needs to be reloaded."
      DRIVERS_REQ_LOADING=1
   fi
}

#####################################################################
# Build and Install the Springware drivers.
#####################################################################
bld_ins_springware()
{
   DRV_MODIFIED=0

   # Prerequisite.  Check for LiS.
   check_lis

   # Build all Springware drivers
   for DRIVER in dlgn gncfd dvbm gpio sctmr
   do
       # Save the current directory.
       SAVE_DIR=`pwd`

       # change to the driver ource directory to build it
       cd ${INTEL_DIALOGIC_DRV_DIR}/${DRIVER}

       # Build the driver
       echo "Building ${DRIVER} driver..."
       make >> ${DRV_LOG} 2>&1

       # Save the return code from the make.
       RC=$?

       # Revert back to the saved orignal directory.
       cd ${SAVE_DIR}

       # Perform error handling of make.
       if [ ${RC} -ne 0 ]
       then
          echo "ERROR: Failed to build and install the ${DRIVER} driver."
	  BUILD_CHECK=1
          return
       fi
       # For kernel 2.4 copy the driver to the system modules directory
       if [ ${K_MINOR} -eq 4 ]
       then
          # Check for existing file
   	  if [ -e ${KERNEL_DRV_DIR}/streams-${DRIVER}Driver.o ]
   	  then
      	     # Check md5sum - necessary for a reload
	     MDNEW=`/usr/bin/md5sum ${INTEL_DIALOGIC_DRV_DIR}/${DRIVER}/streams-${DRIVER}Driver.o 2>/dev/null | awk '{ print $1 }'`
	     MDORG=`/usr/bin/md5sum ${KERNEL_DRV_DIR}/streams-${DRIVER}Driver.o 2>/dev/null | awk '{ print $1 }'`
	     if [ ${MDNEW} = ${MDORG} ]
             then
                # No need to copy or reload this driver
                continue
             fi
          fi
	  cp -f ${INTEL_DIALOGIC_DRV_DIR}/${DRIVER}/streams-${DRIVER}Driver.o ${KERNEL_DRV_DIR}
       fi

       # For kernel 2.6 copy the driver to the LiS modules directory
       if [ ${K_MINOR} -eq 6 ]
       then 
          # Check for existing file
          if [ -e ${LIS_DRV_DIR}/streams-${DRIVER}Driver.o ]
          then
             # Check md5sum - necessary for a reload
	     MDNEW=`/usr/bin/md5sum ${INTEL_DIALOGIC_DRV_DIR}/${DRIVER}/streams-${DRIVER}Driver.o 2>/dev/null | awk '{ print $1 }'`
	     MDORG=`/usr/bin/md5sum ${LIS_DRV_DIR}/streams-${DRIVER}Driver.o 2>/dev/null | awk '{ print $1 }'`
	     if [ "${MDNEW}" = "${MDORG}" ]
             then
                # No need to copy or reload this driver
                continue
             fi
          fi
          cp -f ${INTEL_DIALOGIC_DRV_DIR}/${DRIVER}/streams-${DRIVER}Driver.o ${LIS_DRV_DIR}
       fi
       DRV_MODIFIED=1
   done

   # For 2.6 kernel - need to make LiS to convert SW driver to .ko
   if [ ${K_MINOR} -eq 6 ]
   then
      if [ ${DRV_MODIFIED} -eq 1 ]
      then 
          # Save the current directory.
          SAVE_DIR=`pwd`

          # change to the LiS directory to build and install it.
          cd /usr/src/LiS/
          echo "Converting Springware driver to kernel object..."
          make install >> ${DRV_LOG} 2>&1

          # Save the return code from the make.
          RC=$?

          # Revert back to the saved orignal directory.
          cd ${SAVE_DIR}

          # Perform error handling of make.
          if [ ${RC} -ne 0 ]
          then
             echo "ERROR: Failed to convert Springware Driver to .ko."
	     BUILD_CHECK=1
             return
          fi
      fi
   fi

   if [ ${DRV_MODIFIED} -eq 1 ]
   then
      /sbin/depmod -a &> /dev/null
   fi

   # Check if driver is loaded
   grep dlgn /proc/devices > /dev/null
   RC=$?
   if [ $RC -eq 1 ]
   then
      ${INTEL_DIALOGIC_BIN}/drvload springware > /dev/null
      return
   else 
      # Do not know the SW driver state, have user restart
      echo "Springware drivers modified and needs to be reloaded."
      DRIVERS_REQ_LOADING=1
   fi
}

#####################################################################
# Build and install the drivers.
#####################################################################
bld_ins_drivers()
{
   # Variables indicating installed technology.
   BID_SPRINGWARE_INSTALLED=0
   BID_DM_INSTALLED=0
   BID_IPT_INSTALLED=0
   DRIVERS_REQ_LOADING=0
   BUILD_CHECK=0

   # PACKAGE COMMANDS
   BID_TEST_PKG="rpm -q --quiet"

   # Prerequisites
   check_for_gcc 
   check_for_kernel_source

   ${BID_TEST_PKG} DLGCdev && BID_SPRINGWARE_INSTALLED=1
   ${BID_TEST_PKG} DLGCdmdev && BID_DM_INSTALLED=1
   ${BID_TEST_PKG} DLGCpmac && BID_IPT_INSTALLED=1

   # Install the ctimod drivers.
   [ ${BID_DM_INSTALLED} -eq 1 -o ${BID_IPT_INSTALLED} -eq 1 ] && bld_ins_ctimod

   # Install the DM3 drivers if DLGCdmdev is installed
   [ ${BID_DM_INSTALLED} -eq 1 ] && bld_ins_mercd

   # Install the PMAC drivers if DLGCpmac is installed
   [ ${BID_IPT_INSTALLED} -eq 1 ] && bld_ins_pmacd

   # Install the Springware drivers if DLGCdev is installed
   [ ${BID_SPRINGWARE_INSTALLED} -eq 1 ] && bld_ins_springware

   # Check for compile error
   if [ ${BUILD_CHECK} -eq 1 ]
   then
      echo "ERROR: driver build failed - check ${DRV_LOG}"
      return 1
   fi

   # Check if user need to reload drivers
   if [ ${DRIVERS_REQ_LOADING} -eq 1 ]
   then
      /sbin/depmod -a &> /dev/null
      return 2
   fi
  
   return 0
}

#####################################################################
# Clean and remove the drivers.
#####################################################################
cln_rmv_drivers()
{
   # Get kernel version and isolate major and minor numbers
   VERSION=`uname -r`
   MOD_DIR=/lib/modules/${VERSION}/misc
   K_MAJOR=${VERSION%%.*}
   K_MINTMP=${VERSION##$K_MAJOR.}
   K_MINOR=${K_MINTMP%%.[0-9]*}

   SAVE_DIR=`pwd`

   # Perforn a make clean on the driver source.
   for DRIVER in ctimod mercd pmacd dlgn gncfd dvbm gpio sctmr
   do
       if [ -d ${INTEL_DIALOGIC_DRV_DIR}/${DRIVER} ]
       then
          cd ${INTEL_DIALOGIC_DRV_DIR}/${DRIVER}
 	  echo "Cleaning ${DRIVER}..."
          make clean >> ${DRV_LOG} 2>&1
       fi
   done

   cd ${SAVE_DIR}

   # Remove binaries from system driver directory.
   rm -f ${MOD_DIR}/ctimod* ${MOD_DIR}/mercd.*  ${MOD_DIR}/pmac.* ${MOD_DIR}/streams-*Driver.*

   # For 2.6 kernel - need to remove SW drivers from LiS modules directory
   if [ ${K_MINOR} -eq 6 ]
   then
      rm -f ${LIS_DRV_DIR}/streams-*Driver.o
   fi
}


#####################################################################
# Begining of script
#####################################################################
RET=0
case $1 in
install)
   >${DRV_LOG}
   echo ""
   echo "Building and Installing Intel Dialogic Drivers"
   check_root
   bld_ins_drivers
   RET=$?
   ;;
remove)
   echo ""
   echo "Removing Intel Dialogic Drivers"
   check_root
   cln_rmv_drivers
   rm -f ${DRV_LOG}
   ;;
*)
   RET=1
   ;;
esac

# RET values: 0=>Success, 1=>Failure, 2=>Reload Needed
exit ${RET}

