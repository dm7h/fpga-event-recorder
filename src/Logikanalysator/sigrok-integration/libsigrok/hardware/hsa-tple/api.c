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

/*
#include <glib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
*/
#include <string.h>
#include "libsigrok.h"
#include "libsigrok-internal.h" /* needed for sr_serial_dev_inst_new */
#include "protocol.h"

/* The HSA TPLE uses this */
#define SERIALCOMM "9600/8n1"

// HSA TPLE currently only supports 8 probes (will be up to 24 in the future)
#define NUM_PROBES 8

/* Note: The hsa-tple always samples at 6.25 MHz. */
static const uint64_t samplerate = SR_MHZ(6.25);

/* We name the probes 0-7 on our driver. */
static const char *probe_names[NUM_PROBES + 1] = {
	"0", "1", "2", "3", "4", "5", "6", "7",
	NULL,
};

static const int32_t hwopts[] = {
	SR_CONF_CONN,        // Specification on how to connect to a device.
	SR_CONF_SERIALCOMM,  // Serial communication specification
	/*SR_CONF_SAMPLERATE // device supports setting its samplerate, in Hz */
	/*SR_CONF_RLE        // device supports Run Length Encoding */
};

static const int32_t hwcaps[] = {
	SR_CONF_LOGIC_ANALYZER, // device class (libsigrok.h)
	SR_CONF_LIMIT_SAMPLES,
	SR_CONF_LIMIT_MSEC,
};

SR_PRIV struct sr_dev_driver hsa_tple_driver_info;
static struct sr_dev_driver *di = &hsa_tple_driver_info;

static int init(struct sr_context *sr_ctx)
{
	return std_init(sr_ctx, di, LOG_PREFIX);
}

static GSList *scan(GSList *options)
{
	struct drv_context *drvc;
	struct dev_context *devc;
	struct sr_config *src;
	struct sr_probe *probe;	
	struct sr_dev_inst *sdi;
	struct sr_serial_dev_inst *serial;
	GSList *devices, *l;
	GPollFD probefd;
	const char *conn, *serialcomm;
	int len, i;
	char *buf;
	(void)options;

	drvc = di->priv;
	drvc->instances = NULL;

	devices = NULL;
	conn = serialcomm = NULL;

	sr_dbg("Scanning for hsa-tple devices...");

	// check for options
	for (l = options; l; l = l->next) {
		src = l->data;
		switch (src->key) {
		case SR_CONF_CONN:
			conn = g_variant_get_string(src->data, NULL);
			sr_dbg("SR_CONF_CONN option given.");
			break;
		case SR_CONF_SERIALCOMM:
			serialcomm = g_variant_get_string(src->data, NULL);
			sr_dbg("SR_CONF_SERIALCOMM option given.");
			break;
		}
	}
	if (!conn)
		return NULL;
	if (!serialcomm)
		serialcomm = SERIALCOMM; // if no comm specs provided try the default
	
	if (!(serial = sr_serial_dev_inst_new(conn, serialcomm)))
	{
		sr_dbg("sr_serial_dev_inst_new failed.");
		return NULL;
	}

	if (serial_open(serial, SERIAL_RDWR | SERIAL_NONBLOCK) != SR_OK)
	{
		sr_dbg("serial_open failed.");
		return NULL;
	}

	serial_flush(serial);

	//  let's check if the device is really a hsa-tple device!
	// 'i' is arranged by the protocol as identify command	
	if (serial_write(serial, "i", 1) == -1) {
		sr_err("Unable to send identification string.");
		return NULL;
	}
	else
		sr_dbg("Identification command was sent.");
	
	// response should be "HSA-TPLE"!
	len = 128;
	if (!(buf = g_try_malloc(len))) {
		sr_err("Serial buffer malloc failed.");
		return NULL;
	}

	/* Wait 100ms for a response. */
	g_usleep(100000);

	probefd.fd = serial->fd;
	probefd.events = G_IO_IN;
	g_poll(&probefd, 1, 1);

	if (probefd.revents != G_IO_IN)	{
		sr_spew("No G_IO_IN event!");
		return NULL;
	}
	if (serial_read(serial, buf, 8) != 8) {
		sr_spew("could not read 8 bytes from serial");
		return NULL;
	}
	
	if (strncmp(buf, "HSA-TPLE", 8))	{
		sr_spew("identification string does not match");
		g_free(buf);
		return NULL;
	}
	else {
		g_free(buf);
		sr_spew("identification string matches");
	}
	
	sr_info("Found device on port %s.", conn);
	
	// register the device with libsigrok	
	if (!(sdi = sr_dev_inst_new(0, SR_ST_INACTIVE, "HS Augsburg",
			"TPLE", NULL))) {
		sr_warn("new device instance could not be created");
		return NULL;
	}
	sr_dbg("new device instance successfully created");

	// malloc memory for the device context
	if (!(devc = g_try_malloc0(sizeof(struct dev_context)))) {
		sr_dbg("Device context malloc failed.");
		return NULL;
	}

	// init device context
	devc->num_samples = 0;
	devc->last_timestamp_seen = 0;
	devc->last_sample_seen = 0;
	devc->limit_samples = 0;
	devc->limit_msec = 0;
	devc->last_record_received = FALSE;
	devc->first_sample_sent = 0;
	devc->state = ST_IDENTIFIED;
	
	// init driver instance
	sdi->inst_type = SR_INST_SERIAL;
	sdi->conn = serial;
	sdi->priv = devc;
	sdi->driver = di;
	
	// let's append our probes
	for (i = 0; probe_names[i]; i++) {
		if (!(probe = sr_probe_new(i, SR_PROBE_LOGIC, TRUE,
				probe_names[i])))
			return NULL;
		sdi->probes = g_slist_append(sdi->probes, probe);
	}
	
	drvc->instances = g_slist_append(drvc->instances, sdi);
	devices = g_slist_append(devices, sdi);
	
	serial_close(serial);
	if (!devices)
		sr_serial_dev_inst_free(serial);

	return devices;
}

static GSList *dev_list(void)
{
	return ((struct drv_context *)(di->priv))->instances;
}

static int dev_clear(void)
{
	return std_dev_clear(di, NULL);
}

static int dev_open(struct sr_dev_inst *sdi)
{
	struct drv_context *drvc;
	struct sr_serial_dev_inst *serial;

	sr_dbg("opening hsa-tple device...");
	
	if (!(drvc = di->priv)) {
		sr_err("dev_open: driver was not initialized.");
		return SR_ERR;
	}
	
	serial = sdi->conn;
	if (serial_open(serial, SERIAL_RDWR | SERIAL_NONBLOCK) != SR_OK)
	{
		sr_err("dev_open: serial device could not be opened");
		return SR_ERR;
	}

	// FIXME/TODO: send reset command here? this only resets the CPLD!

	// "The device instance is actively in use in a session."
	sdi->status = SR_ST_ACTIVE;

	return SR_OK;
}

static int dev_close(struct sr_dev_inst *sdi)
{
	struct sr_serial_dev_inst *serial;

	if (!di->priv) {
		sr_err("dev_close: driver was not initialized.");
		return SR_ERR;
	}
		
	sr_dbg("closing hsa-tple device...");

	// get handle from sdi->conn and close it
	serial = sdi->conn;
	if (serial && serial->fd != -1) {
		serial_close(serial);
		sdi->status = SR_ST_INACTIVE;
	}

	return SR_OK;
}

static int cleanup(void)
{
	int ret;
	struct drv_context *drvc;

	if (!(drvc = di->priv))
		/* Can get called on an unused driver, doesn't matter. */
		return SR_OK;

	ret = dev_clear();
	g_free(drvc);
	di->priv = NULL;

	return ret;
}

static int config_set(int key, GVariant *data, const struct sr_dev_inst *sdi,
		const struct sr_probe_group *probe_group)
{
	struct dev_context *devc;
	int ret;

	(void)data;
	(void)probe_group;

	if (sdi->status != SR_ST_ACTIVE)
		return SR_ERR_DEV_CLOSED;
	
	if (!(devc = sdi->priv)) {
		sr_err("sdi->priv was NULL.");
		return SR_ERR_BUG;
	}
	
	ret = SR_OK;
	switch (key) {
	case SR_CONF_LIMIT_SAMPLES:
		devc->limit_samples = g_variant_get_uint64(data);
		devc->limit_msec = (uint64_t)((double)devc->limit_samples/(double)samplerate*1000);
		if( devc->limit_msec == 0 )
			devc->limit_msec = 1;

		sr_dbg("samplerate: %d", samplerate);
		sr_dbg("calculated devc->limit_msec: %d", devc->limit_msec);
		break;
	case SR_CONF_LIMIT_MSEC:
		devc->limit_msec = g_variant_get_uint64(data);
		devc->limit_samples = (uint64_t)(samplerate*((double)devc->limit_msec/1000));
		if( devc->limit_samples == 0 )
			devc->limit_samples = 1;
		
		sr_dbg("samplerate: %d", samplerate);
		sr_dbg("calculated devc->limit_samples: %d", devc->limit_samples);
		break;
	default:
		ret = SR_ERR_NA;
	}

	return ret;
}

// config_get fixes vcd problem
static int config_get(int id, GVariant **data, const struct sr_dev_inst *sdi,
		const struct sr_probe_group *probe_group)
{
	(void)sdi;
	(void)probe_group;

	switch (id) {
	/*
	case SR_CONF_LIMIT_SAMPLES:
		*data = g_variant_new_uint64(devc->limit_samples);
		break;
	case SR_CONF_LIMIT_MSEC:
		*data = g_variant_new_uint64(devc->limit_msec);
		break;
	case SR_CONF_NUM_TIMEBASE:
		*data = g_variant_new_int32(devc->num_timebases);
		break;*/
	case SR_CONF_SAMPLERATE:
		/* The tple samplerate is 62.5MHz and can't be changed. */
		*data = g_variant_new_uint64(SR_MHZ(6.25));
		break;
	default:
		return SR_ERR_NA;
	}

	return SR_OK;
}

static int config_list(int key, GVariant **data, const struct sr_dev_inst *sdi,
		const struct sr_probe_group *probe_group)
{
	int ret;

	(void)sdi;
	(void)data;
	(void)probe_group;

	ret = SR_OK;
	switch (key) {
	case SR_CONF_DEVICE_OPTIONS:
		*data = g_variant_new_fixed_array(G_VARIANT_TYPE_INT32,
				hwcaps, ARRAY_SIZE(hwcaps), sizeof(int32_t));
		break;
	case SR_CONF_SCAN_OPTIONS:
		*data = g_variant_new_fixed_array(G_VARIANT_TYPE_INT32,
				hwopts, ARRAY_SIZE(hwopts), sizeof(int32_t));
		break;
	// case SR_CONF_SAMPLERATE:
	//      ...
	//      break;
	default:
		return SR_ERR_NA;
	}


	return ret;
}

static int dev_acquisition_start(const struct sr_dev_inst *sdi,
				    void *cb_data)
{
	struct dev_context *devc;
	struct sr_serial_dev_inst *serial;
	
	(void)sdi;
	(void)cb_data;

	if (!sdi || !cb_data || !(devc = sdi->priv))
		return SR_ERR_BUG;
	
	if (sdi->status != SR_ST_ACTIVE)
		return SR_ERR_DEV_CLOSED;

	/*
	 * Reset the number of samples to take. If we've already collected our
	 * quota, but we start a new session, and don't reset this, we'll just
	 * quit without acquiring any new samples.
	 */
	devc->num_samples = 0; // set number of samples to zero
	devc->last_timestamp_seen = 0;
	devc->last_sample_seen = 0;
	devc->last_record_received = FALSE;
	devc->cb_data = cb_data;
	devc->buflen = 0;
	devc->first_sample_sent = 0;

	// start acquisition
	sr_dbg("Acquisition has started now!");
	devc->state = ST_START;

	/* Send header packet to the session bus. */
	std_session_send_df_header(cb_data, LOG_PREFIX);

	/* Poll every 50ms, or whenever some data comes in. */
	serial = sdi->conn;
	sr_source_add(serial->fd, G_IO_IN, 50, hsa_tple_receive_data, (void *)sdi);
	
	return SR_OK;
}


static int dev_acquisition_stop(struct sr_dev_inst *sdi, void *cb_data)
{
	struct dev_context *devc;
	struct sr_datafeed_packet packet;

	sr_dbg("Stopping acquisition.");
	
	if (!di->priv) {
		sr_err("Driver was not initialized.");
		return SR_ERR;
	}

	if (sdi->status != SR_ST_ACTIVE) {
		sr_spew("sdi->status: %d", sdi->status);		
		sr_err("Device inactive, can't stop acquisition.");
		return SR_ERR;
	}
	//sdi->status = SR_ST_STOPPING; // makes std_... function fail
	
	devc = sdi->priv;
	devc->cb_data = cb_data;

	// TODO/FIXME: sr_source_remove? channel remove?
	
	/* Send end packet to the session bus. */
	sr_dbg("Sending SR_DF_END packet.");
	packet.type = SR_DF_END;
	sr_session_send(cb_data, &packet);

	return std_dev_acquisition_stop_serial(sdi, cb_data, dev_close, sdi->conn, LOG_PREFIX);
}

SR_PRIV struct sr_dev_driver hsa_tple_driver_info = {
	.name = "hsa-tple",
	.longname = "HS Augsburg tple-logicanalyzer",
	.api_version = 1,
	.init = init,
	.cleanup = cleanup,
	.scan = scan,
	.dev_list = dev_list,
	.dev_clear = dev_clear,
	.config_get = config_get,
	.config_set = config_set,
	.config_list = config_list,
	.dev_open = dev_open,
	.dev_close = dev_close,
	.dev_acquisition_start = dev_acquisition_start,
	.dev_acquisition_stop = dev_acquisition_stop,
	.priv = NULL,
};
