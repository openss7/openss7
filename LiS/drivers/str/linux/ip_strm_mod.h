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


#if defined(__KERNEL__)

#include <linux/skbuff.h>
#include <linux/netdevice.h>

/*
 * Somewhere between 2.3.35 and 2.3.51 tbusy disappeared and was
 * replaced by the netif_start/stop_queue mechanism.  We assume
 * that 2.3.x means the newer style.  So if you have problems with
 * this with a 2.2.35-ish kernel, download a newer kernel source.
 */
#define	ism_dev		net_device
#ifndef enet_statistics
#define enet_statistics	net_device_stats
#endif

typedef struct ip_to_streams
{
	unsigned long dl_magic;
	unsigned long dl_sap ;
	queue_t	*dl_q ;
	queue_t	*dl_rdq ;
        queue_t *dl_wrq ;
        int	 dl_err_prim ;
	int	 dlstate;
	int	 dl_m_error;
	int	 dl_tli_err;
	int	 dl_unix_err;
	int	 dl_reason;
	int	 dl_retry_proto;
	int	 dl_lower_ptr;
	int	 dl_bufcall_id;
	int	 contype;
	int	 ip_open ;
	int	 dev_registered ;
	struct enet_statistics  stats ;
	char	 myname[16] ;	/* something for mydev.name to point to */
	struct ism_dev	mydev;	/* a device struct, not a pointer */
} ip_to_streams_minor_t, *ip_to_streams_minor_p;

#endif

#define DBG_OPEN 	0x001
#define DBG_WPUT 	0x002
#define DBG_PUT 	0x004
#define DBG_SQUAWK 	0x008
#define DBG_UPR_PROTOS 	0x010
#define DBG_SVC 	0x020
#define DBG_DATA 	0x040
#define DBG_ALLOCB 	0x080
#define DBG_MERROR 	0x100
#define DBG_ERROR_ACK	0x200
#define DBG_SQUAWKP	0x400

#define DL_MAGIC	0x12345

/*
 * Private ioctl to set interface name.
 *
 * Might be defined in sockios.h already.  We will use the same
 * value that sockios.h would use.
 */
#ifndef SIOCSIFNAME
#define SIOCSIFNAME     0x8923		/* ioctl code */
#endif

#define IP	0x1
