/*
 * This file is part of the libsigrok project.
 *
 * Copyright (C) 2013 Bert Vermeulen <bert@biot.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBSIGROK_HARDWARE_KECHENG_KC_330B_PROTOCOL_H
#define LIBSIGROK_HARDWARE_KECHENG_KC_330B_PROTOCOL_H

#include <stdint.h>
#include <glib.h>
#include "libsigrok.h"
#include "libsigrok-internal.h"

/* Message logging helpers with subsystem-specific prefix string. */
#define LOG_PREFIX "kecheng-kc-330b: "
#define sr_log(l, s, args...) sr_log(l, LOG_PREFIX s, ## args)
#define sr_spew(s, args...) sr_spew(LOG_PREFIX s, ## args)
#define sr_dbg(s, args...) sr_dbg(LOG_PREFIX s, ## args)
#define sr_info(s, args...) sr_info(LOG_PREFIX s, ## args)
#define sr_warn(s, args...) sr_warn(LOG_PREFIX s, ## args)
#define sr_err(s, args...) sr_err(LOG_PREFIX s, ## args)

#define EP_IN 0x80 | 1
#define EP_OUT 2

/* 500ms */
#define DEFAULT_SAMPLE_INTERVAL 0
#define DEFAULT_ALARM_LOW 40
#define DEFAULT_ALARM_HIGH 120
#define DEFAULT_WEIGHT_TIME SR_MQFLAG_SPL_TIME_WEIGHT_F
#define DEFAULT_WEIGHT_FREQ SR_MQFLAG_SPL_FREQ_WEIGHT_A
/* Live */
#define DEFAULT_DATA_SOURCE DATA_SOURCE_LIVE

enum {
	LIVE_SPL_IDLE,
	LIVE_SPL_WAIT,
	LOG_DATA_IDLE,
	LOG_DATA_WAIT,
};

enum {
	CMD_CONFIGURE = 0x01,
	CMD_IDENTIFY = 0x02,
	CMD_SET_DATE_TIME = 0x03,
	CMD_GET_STATUS = 0x04,
	CMD_GET_LOG_INFO = 0x05,
	CMD_GET_LOG_DATA = 0x07,
	CMD_GET_LIVE_SPL = 0x08,
};

enum {
	DATA_SOURCE_LIVE,
	DATA_SOURCE_MEMORY,
};

enum {
	DEVICE_ACTIVE,
	DEVICE_INACTIVE,
};

/** Private, per-device-instance driver context. */
struct dev_context {
	/* Acquisition settings */
	uint64_t limit_samples;
	int sample_interval;
	int alarm_low;
	int alarm_high;
	uint64_t mqflags;
	int data_source;

	/* Operational state */
	int state;
	gboolean config_dirty;
	uint64_t num_samples;
	uint64_t stored_samples;
	void *cb_data;
	int usbfd[10];
	struct libusb_transfer *xfer;
	unsigned char buf[128];

	/* Temporary state across callbacks */
	gint64 last_live_request;

};

SR_PRIV int kecheng_kc_330b_handle_events(int fd, int revents, void *cb_data);
SR_PRIV void kecheng_kc_330b_receive_transfer(struct libusb_transfer *transfer);
SR_PRIV int kecheng_kc_330b_configure(const struct sr_dev_inst *sdi);
SR_PRIV int kecheng_kc_330b_set_date_time(struct sr_dev_inst *sdi);
SR_PRIV int kecheng_kc_330b_recording_get(const struct sr_dev_inst *sdi,
		gboolean *tmp);
SR_PRIV int kecheng_kc_330b_status_get(const struct sr_dev_inst *sdi,
		int *status);
SR_PRIV int kecheng_kc_330b_log_info_get(const struct sr_dev_inst *sdi,
		unsigned char *buf);

#endif
