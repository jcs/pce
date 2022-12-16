/*****************************************************************************
 * pce                                                                       *
 *****************************************************************************/

/*****************************************************************************
 * File name:   src/lib/vmnet.m                                              *
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

#include "log.h"

#import "vmnet.h"

struct pce_vmnet_interface * pce_vmnet_start (char *bridge_if)
{
	__block struct pce_vmnet_interface *ret;
	__block vmnet_return_t if_status;

	ret = malloc (sizeof(struct pce_vmnet_interface));
	if (ret == NULL) {
		return NULL;
	}

	memset (ret, 0, sizeof(struct pce_vmnet_interface));

	ret->if_queue = dispatch_queue_create ("ch.hampa.pce.vmnet", DISPATCH_QUEUE_SERIAL);
	dispatch_semaphore_t if_start_sem = dispatch_semaphore_create (0);

	xpc_object_t if_desc = xpc_dictionary_create (NULL, NULL, 0);

	if (bridge_if == NULL || bridge_if[0] == '\0') {
		xpc_dictionary_set_uint64 (if_desc, vmnet_operation_mode_key, VMNET_SHARED_MODE);
		xpc_dictionary_set_string (if_desc, vmnet_start_address_key, "10.10.10.1");
		xpc_dictionary_set_string (if_desc, vmnet_end_address_key, "10.10.10.5");
		xpc_dictionary_set_string (if_desc, vmnet_subnet_mask_key, "255.255.255.0");
		pce_log_tag (MSG_INF,
			"SCSI:",
			"shared net ip=%s netmask=%s\n",
			"10.10.10.1", "255.255.255.0"
		);
	} else {
		xpc_dictionary_set_uint64 (if_desc, vmnet_operation_mode_key, VMNET_BRIDGED_MODE);
		xpc_dictionary_set_string (if_desc, vmnet_shared_interface_name_key, bridge_if);
		pce_log_tag (MSG_INF,
			"SCSI:",
			"bridged net if=%s\n",
			bridge_if
		);
	}

	ret->interface = vmnet_start_interface (if_desc, ret->if_queue,
		^(vmnet_return_t status, xpc_object_t if_param) {
			if_status = status;

			if (status != VMNET_SUCCESS) {
				dispatch_semaphore_signal (if_start_sem);
				if (status == VMNET_FAILURE) {
					pce_log (MSG_ERR, "*** vmnet_start_interface failed, "
						"try with root privileges?\n");
				}
				return;
			}

			dispatch_semaphore_signal (if_start_sem);
		});

	dispatch_semaphore_wait (if_start_sem, DISPATCH_TIME_FOREVER);
	dispatch_release (if_start_sem);
	xpc_release (if_desc);

	if (if_status != VMNET_SUCCESS) {
		dispatch_release (ret->if_queue);
		free (ret);
		return NULL;
	}

	vmnet_interface_set_event_callback (ret->interface,
		VMNET_INTERFACE_PACKETS_AVAILABLE, ret->if_queue,
		^(interface_event_t event_id, xpc_object_t event) {
			unsigned int navail = xpc_dictionary_get_uint64 (event,
				vmnet_estimated_packets_available_key);
			ret->packets_available = navail;
		});

	return ret;
}

void pce_vmnet_stop (struct pce_vmnet_interface *vi)
{
	if (vi == NULL || vi->interface == NULL) {
		return;
	}

	dispatch_semaphore_t if_stop_sem = dispatch_semaphore_create (0);

	vmnet_interface_set_event_callback(vi->interface,
		VMNET_INTERFACE_PACKETS_AVAILABLE, NULL, NULL);

	vmnet_stop_interface (vi->interface, vi->if_queue,
		^(vmnet_return_t status) {
			dispatch_semaphore_signal (if_stop_sem);
		});

	dispatch_semaphore_wait (if_stop_sem, DISPATCH_TIME_FOREVER);
	dispatch_release (vi->if_queue);

	free (vi);
}

int pce_vmnet_write (struct pce_vmnet_interface *vi, void *buf, size_t size)
{
	struct iovec packets_iovec = {
		.iov_base = buf,
		.iov_len = size,
	};

	struct vmpktdesc packets = {
		.vm_pkt_size = size,
		.vm_pkt_iov = &packets_iovec,
		.vm_pkt_iovcnt = 1,
		.vm_flags = 0,
	};

	int count = packets.vm_pkt_iovcnt;

	if (vi == NULL || vi->interface == NULL) {
		return -1;
	}

	if (vmnet_write (vi->interface, &packets, &count) != VMNET_SUCCESS) {
		return -1;
	}

	return 0;
}

size_t pce_vmnet_read (struct pce_vmnet_interface *vi, void *buf, size_t size)
{
	struct iovec packets_iovec = {
		.iov_base = buf,
		.iov_len = size,
	};

	struct vmpktdesc packets = {
		.vm_pkt_size = size,
		.vm_pkt_iov = &packets_iovec,
		.vm_pkt_iovcnt = 1,
		.vm_flags = 0,
	};

	int count = 1;

	vmnet_return_t status = vmnet_read (vi->interface, &packets, &count);

	if (status != VMNET_SUCCESS) {
		return 0;
	}

	if (count == 0) {
		return 0;
	}

	if (vi->packets_available > 0)
		vi->packets_available--;

	return packets.vm_pkt_size;
}
