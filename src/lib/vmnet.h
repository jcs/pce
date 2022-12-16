/*****************************************************************************
 * pce                                                                       *
 *****************************************************************************/

/*****************************************************************************
 * File name:   src/lib/vmnet.h                                              *
 * Created:     2022-09-18 by joshua stein <jcs@jcs.org>                     *
 * Copyright:   (C) 2022 joshua stein <jcs@jcs.org>                          *
 *****************************************************************************/

/*****************************************************************************
 * This program is free software. You can redistribute it and / or modify it *
 * under the terms of the GNU General Public License version 2 as  published *
 * by the Free Software Foundation.                                          *
 *                                                                           *
 * This program is distributed in the hope  that  it  will  be  useful,  but *
 * WITHOUT  ANY   WARRANTY,   without   even   the   implied   warranty   of *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU  General *
 * Public License for more details.                                          *
 *****************************************************************************/

#ifndef PCE_LIB_VMNET_H
#define PCE_LIB_VMNET_H 1

#include <sys/uio.h>
#include <vmnet/vmnet.h>

struct pce_vmnet_interface {
	interface_ref interface;
	dispatch_queue_t if_queue;
	unsigned int packets_available;
};

struct pce_vmnet_interface * pce_vmnet_start (char *bridge_if);
void pce_vmnet_stop (struct pce_vmnet_interface *vi);
int pce_vmnet_write (struct pce_vmnet_interface *vi, void *buf, size_t size);
size_t pce_vmnet_read (struct pce_vmnet_interface *vi, void *buf, size_t size);

#endif
