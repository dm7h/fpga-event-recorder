/*
 * This file is part of the libsigrok project.
 *
 * Copyright (C) 2013-2014 Matthias Weber <matthias.weber@hs-augsburg.de>
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

#ifndef LIBSIGROK_HARDWARE_HSA_TPLE_PROTOCOL_H
#define LIBSIGROK_HARDWARE_HSA_TPLE_PROTOCOL_H

#include <stdint.h>
#include <string.h>
#include <glib.h>
#include "libsigrok.h"
#include "libsigrok-internal.h"

/* Message logging helpers with subsystem-specific prefix string. */
#define LOG_PREFIX "hsa-tple: "
#define sr_log(l, s, args...) sr_log(l, LOG_PREFIX s, ## args)
#define sr_spew(s, args...) sr_spew(LOG_PREFIX s, ## args)
#define sr_dbg(s, args...) sr_dbg(LOG_PREFIX s, ## args)
#define sr_info(s, args...) sr_info(LOG_PREFIX s, ## args)
#define sr_warn(s, args...) sr_warn(LOG_PREFIX s, ## args)
#define sr_err(s, args...) sr_err(LOG_PREFIX s, ## args)

/* Command opcodes (ASCII tokens) */
#define CMD_IDENTIFY   'i'  // identify
#define CMD_RESET      'r'  // reset device
#define CMD_GO         'g'  // start measurement
#define CMD_STOP       's'  // stop measurement
#define CMD_DUMP       'd'  // dump data
#define CMD_TXTDUMP    'D'  // dump data as VCD in ASCII format (debug dump)
#define CMD_IDENTIFY   'i'  // prompt the device to identify itself as 'HSA-TPLE'
#define CMD_GETSTATUS  'S'  // request status information

/* Status codes (ASCII tokens) */
#define STATUS_MEMFULL                 'f' // RAM memory full
#define STATUS_MEASUREMENT_STARTED     'r' // aka running
#define STATUS_MEASUREMENT_NOT_STARTED 's' // aka stopped

/* Record status byte codes */
#define DATA_RECORD_MORE (uint8_t)0x00
#define DATA_RECORD_LAST (uint8_t)0xFF

/* Size of internal (host-side) serial buffer */
#define BUFSIZE 1024

/** Private, per-device-instance driver context. */
struct dev_context {
	/* --- Acquisition settings ------------------------------ */

	/** The current sampling limit (in number of samples). */
	uint64_t limit_samples;

	/** The current sampling limit (in ms). */
	uint64_t limit_msec;
	
	/* --- Operational state --------------------------------- */
	int state;

	/** The current number of already received (this will be "sent" for us) samples. */
	uint64_t num_samples; // aka sample_counter

	/** Save the last timestamp that has been seen; needed for RLE/ duplicaton */
	uint16_t last_timestamp_seen;

	/** Save the last sample value seen */
	uint8_t  last_sample_seen;

	/** Start time of acquisition session */
	int64_t starttime;

	/** Opaque pointer passed in by the frontend. */
	void *cb_data; // "callback data"
	
	/** host side sample buffer */
	uint8_t buf[BUFSIZE];
	int buflen; // current buffer length

	/** flag if last record/ data packet was received, so we have finished */
	int last_record_received;
	int first_sample_sent;
};

/* Parser state machine. */
enum {
	ST_UNKNOWN,
	ST_IDENTIFIED,
	ST_START,
	ST_SENDCMD_RESET,
	ST_SENDCMD_START_MEASUREMENT,
	ST_GETRESP_START_MEASUREMENT,
	ST_WAIT_ENDCONDITIONS,
	ST_SENDCMD_STOP_MEASUREMENT,
	ST_GETRESP_STOP_MEASUREMENT,
	ST_SENDCMD_DUMP_DATA,
	ST_GETRESP_DUMP_DATA,
	ST_GET_DUMP,
	ST_FINISHED,
	ST_COM_ERROR,
};

/* Packet parser return code. */
enum {
	PACKET_OK,
	PACKET_NEED_MORE_DATA,
	PACKET_INVALID,
};

SR_PRIV int hsa_tple_receive_data(int fd, int revents, void *cb_data);

#endif
