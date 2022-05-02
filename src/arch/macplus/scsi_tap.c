/*****************************************************************************
 * pce                                                                       *
 *****************************************************************************/

/*****************************************************************************
 * File name:   src/arch/macplus/scsi_tap.c                                  *
 * Created:     2022-05-01 by joshua stein <jcs@jcs.org>                     *
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

#include "main.h"
#include "scsi.h"

#include <stdlib.h>

#include <lib/log.h>

#include <errno.h>
#include <string.h>
#include <poll.h>
#include <sys/fcntl.h>
#include <sys/types.h>
int mac_scsi_ethernet_tap_open (mac_scsi_t *scsi, mac_scsi_dev_t *dev)
{
	char tap_cmd_sh[1024];

	dev->tap_fd = -1;

	if ((dev->tap_fd = open (dev->tap_dev, O_RDWR)) < 0) {
		pce_log (MSG_ERR, "*** opening %s failed (%s)\n", dev->tap_dev, strerror(errno));
		return - 1;
	}

	if (dev->tap_cmd[0] != '\0') {
		/* run command with args "/dev/tap0" "00:00:00:00:00:00" */
		snprintf (tap_cmd_sh, sizeof(tap_cmd_sh), "%s \"%s\" \"%02x:%02x:%02x:%02x:%02x:%02x\"",
			dev->tap_cmd, dev->tap_dev,
			dev->mac_addr[0], dev->mac_addr[1],
			dev->mac_addr[2], dev->mac_addr[3],
			dev->mac_addr[4], dev->mac_addr[5]);
		pce_log_tag (MSG_INF,
			"SCSI:",
			"running tap command: %s\n",
			tap_cmd_sh);

		if (system (tap_cmd_sh) != 0) {
			pce_log (MSG_ERR, "*** tap command failed (%s)\n", strerror(errno));
		}
	}

	return 0;
}

int mac_scsi_ethernet_data_avail (mac_scsi_dev_t *dev)
{
	struct pollfd fds;

	fds.fd = dev->tap_fd;
	fds.events = POLLIN | POLLERR;
	if (poll (&fds, 1, 0) < 1) {
		return 0;
	}
	if ((fds.revents & POLLIN)) {
		return 1;
	}

	return 0;
}

size_t mac_scsi_ethernet_read (mac_scsi_dev_t *dev, unsigned char *buf)
{
	if (dev->tap_fd < 0) {
		return 0;
	}

	if (!mac_scsi_ethernet_data_avail (dev)) {
		return 0;
	}

	return read (dev->tap_fd, buf, 1514);
}

size_t mac_scsi_ethernet_write (mac_scsi_dev_t *dev, unsigned char *buf, size_t len)
{
	if (dev->tap_fd < 0) {
		return 0;
	}

	return write (dev->tap_fd, buf, len);
}
