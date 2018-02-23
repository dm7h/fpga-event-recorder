/*
 * This file is part of the libsigrok project.
 *
 * Copyright (C) 2013 Matthias Heidbrink <m-sigrok@heidbrink.biz>
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

#ifndef LIBSIGROK_HARDWARE_GMC_MH_1X_2X_PROTOCOL_H
#define LIBSIGROK_HARDWARE_GMC_MH_1X_2X_PROTOCOL_H

#include <stdint.h>
#include <glib.h>
#include "libsigrok.h"
#include "libsigrok-internal.h"

/* Message logging helpers with subsystem-specific prefix string. */
#define LOG_PREFIX "gmc-mh-1x-2x: "
#define sr_log(l, s, args...) sr_log(l, LOG_PREFIX s, ## args)
#define sr_spew(s, args...) sr_spew(LOG_PREFIX s, ## args)
#define sr_dbg(s, args...) sr_dbg(LOG_PREFIX s, ## args)
#define sr_info(s, args...) sr_info(LOG_PREFIX s, ## args)
#define sr_warn(s, args...) sr_warn(LOG_PREFIX s, ## args)
#define sr_err(s, args...) sr_err(LOG_PREFIX s, ## args)

#define GMC_BUFSIZE  266

/** Message ID bits 4, 5 */
#define MSGID_MASK 0x30	/**< Mask to get message ID bits */
#define MSGID_INF  0x00 /**< Start of message with device info */
#define MSGID_D10  0x10	/**< Start of data message, non-displayed intermediate */
#define MSGID_DTA  0x20 /**< Start of data message, displayed, averaged */
#define MSGID_DATA 0x30	/**< Data byte in message */

#define MSGC_MASK 0x0f	/**< Mask to get message byte contents */

#define MSGSRC_MASK 0xc0 /**< Mask to get bits related to message source */

#define bc(x) (x & MSGC_MASK) /**< Get contents from a byte */

#define MASK_6BITS 0x3f /**< Mask lower six bits. */

/** Internal multimeter model codes. In opposite to the multimeter models from
 *  protocol (see decode_model()), these codes allow working with ranges.
 */
enum model {
	SR_METRAHIT_NONE	= 0,	/**< Value for uninitialized variable */
	SR_METRAHIT_12S		= 12,
	SR_METRAHIT_13S14A	= 13,
	SR_METRAHIT_14S		= 14,
	SR_METRAHIT_15S		= 15,
	SR_METRAHIT_16S		= 16,
	SR_METRAHIT_16I		= 17,
	SR_METRAHIT_16X = SR_METRAHIT_16I,  /**< All Metrahit 16 */
	/* A Metrahit 17 exists, but seems not to have an IR interface. */
	SR_METRAHIT_18S		= 18,
	SR_METRAHIT_2X		= 20,	/**< For model type comparisons */
	SR_METRAHIT_22SM	= 22,
	SR_METRAHIT_23S		= 23,
	SR_METRAHIT_24S		= 24,
	SR_METRAHIT_25SM	= 25,
	SR_METRAHIT_26S		= 26,
	SR_METRAHIT_28S		= 28,
	SR_METRAHIT_29S		= 29,
};

/** Convert GMC model code in send mode to sigrok-internal one. */
SR_PRIV int sr_gmc_decode_model_sm(guchar mcode);

/** Convert GMC model code in bidirectional mode to sigrok-internal one.
    \param[in] mcode Model code.

    \return Model code
*/
SR_PRIV int sr_gmc_decode_model_bidi(guchar mcode);

/** Create 14-bytes (42 byte) message used in bidirectional mode.
 *  \param[in] addr Target address. 0..15, 0=Broadcast.
 *  \param[in] func Function code.
 *  \param[in] params Further parameters, &char[9]. Unused bytes must be 0.
 *  \param[out] buf &guchar[42] Output buffer (3*14).
 *
 */
void create_msg_14(guchar addr, guchar func, guchar* params, guchar* buf);

/** Get model string from sigrok-internal model code.
 */
SR_PRIV const char* sr_gmc_model_str(enum model mcode);

/** Private, per-device-instance driver context. */
struct dev_context {
	/* Model-specific information */
	enum model model;	/**< Model code. */

	/* Acquisition settings */
	uint64_t limit_samples;	/**< Target number of samples */
	uint64_t limit_msec;	/**< Target sampling time */

	/* Opaque pointer passed in by frontend. */
	void *cb_data;

	/* Operational state */
	gboolean settings_ok;	/**< Settings msg received yet. */
	int msg_type;	/**< Message type (MSGID_INF, ...). */
	int msg_len;	/**< Message lengh (valid when msg, curr. type known).*/
	int mq;		/**< Measured quantity */
	int unit;		/**< Measured unit */
	uint64_t mqflags;	/**< Measured quantity flags */
	float value;		/**< Measured value */
	float scale;		/**< Scale for value. */
	int8_t scale1000;   /**< Additional scale factor 1000^x. */
	gboolean vmains_29S;	/**< Measured ctmv is V mains (29S only). */

	/* Temporary state across callbacks */
	uint64_t num_samples;	/**< Current #samples for limit_samples */
	GTimer* elapsed_msec;	/**< Used for sampling with limit_msec  */
	unsigned char buf[GMC_BUFSIZE];	/**< Buffer for read callback */
	int buflen;			/**< Data len in buf */
};

SR_PRIV int gmc_mh_1x_2x_receive_data(int fd, int revents, void *cb_data);

#endif
