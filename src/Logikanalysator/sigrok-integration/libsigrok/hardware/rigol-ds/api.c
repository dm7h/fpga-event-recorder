/*
 * This file is part of the libsigrok project.
 *
 * Copyright (C) 2012 Martin Ling <martin-git@earth.li>
 * Copyright (C) 2013 Bert Vermeulen <bert@biot.com>
 * Copyright (C) 2013 Mathias Grimmberger <mgri@zaphod.sax.de>
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

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include "libsigrok.h"
#include "libsigrok-internal.h"
#include "protocol.h"

static const int32_t hwopts[] = {
	SR_CONF_CONN,
};

static const int32_t hwcaps[] = {
	SR_CONF_OSCILLOSCOPE,
	SR_CONF_TIMEBASE,
	SR_CONF_TRIGGER_SOURCE,
	SR_CONF_TRIGGER_SLOPE,
	SR_CONF_HORIZ_TRIGGERPOS,
	SR_CONF_NUM_TIMEBASE,
};

static const int32_t analog_hwcaps[] = {
	SR_CONF_NUM_VDIV,
	SR_CONF_VDIV,
	SR_CONF_COUPLING,
	SR_CONF_DATA_SOURCE,
};

static const uint64_t timebases[][2] = {
	/* nanoseconds */
	{ 2, 1000000000 },
	{ 5, 1000000000 },
	{ 10, 1000000000 },
	{ 20, 1000000000 },
	{ 50, 1000000000 },
	{ 100, 1000000000 },
	{ 500, 1000000000 },
	/* microseconds */
	{ 1, 1000000 },
	{ 2, 1000000 },
	{ 5, 1000000 },
	{ 10, 1000000 },
	{ 20, 1000000 },
	{ 50, 1000000 },
	{ 100, 1000000 },
	{ 200, 1000000 },
	{ 500, 1000000 },
	/* milliseconds */
	{ 1, 1000 },
	{ 2, 1000 },
	{ 5, 1000 },
	{ 10, 1000 },
	{ 20, 1000 },
	{ 50, 1000 },
	{ 100, 1000 },
	{ 200, 1000 },
	{ 500, 1000 },
	/* seconds */
	{ 1, 1 },
	{ 2, 1 },
	{ 5, 1 },
	{ 10, 1 },
	{ 20, 1 },
	{ 50, 1 },
	{ 100, 1 },
	{ 200, 1 },
	{ 500, 1 },
	/* { 1000, 1 }, Confuses other code? */
};

static const uint64_t vdivs[][2] = {
	/* microvolts */
	{ 500, 1000000 },
	/* millivolts */
	{ 1, 1000 },
	{ 2, 1000 },
	{ 5, 1000 },
	{ 10, 1000 },
	{ 20, 1000 },
	{ 50, 1000 },
	{ 100, 1000 },
	{ 200, 1000 },
	{ 500, 1000 },
	/* volts */
	{ 1, 1 },
	{ 2, 1 },
	{ 5, 1 },
	{ 10, 1 },
};

#define NUM_TIMEBASE  ARRAY_SIZE(timebases)
#define NUM_VDIV      ARRAY_SIZE(vdivs)

static const char *trigger_sources[] = {
	"CH1",
	"CH2",
	"EXT",
	"AC Line",
	"D0",
	"D1",
	"D2",
	"D3",
	"D4",
	"D5",
	"D6",
	"D7",
	"D8",
	"D9",
	"D10",
	"D11",
	"D12",
	"D13",
	"D14",
	"D15",
};

static const char *coupling[] = {
	"AC",
	"DC",
	"GND",
};

/* Do not change the order of entries */
static const char *data_sources[] = {
	"Live",
	"Memory",
	"Segmented",
};

/* 
 * name, series, protocol flavor, min timebase, max timebase, min vdiv,
 * digital channels, number of horizontal divs
 */
static const struct rigol_ds_model supported_models[] = {
	{"DS1052E", RIGOL_DS1000, PROTOCOL_LEGACY, {5, 1000000000}, {50, 1}, {2, 1000}, false, 12},
	{"DS1102E", RIGOL_DS1000, PROTOCOL_LEGACY, {2, 1000000000}, {50, 1}, {2, 1000}, false, 12},
	{"DS1152E", RIGOL_DS1000, PROTOCOL_LEGACY, {2, 1000000000}, {50, 1}, {2, 1000}, false, 12},
	{"DS1052D", RIGOL_DS1000, PROTOCOL_LEGACY, {5, 1000000000}, {50, 1}, {2, 1000}, true, 12},
	{"DS1102D", RIGOL_DS1000, PROTOCOL_LEGACY, {2, 1000000000}, {50, 1}, {2, 1000}, true, 12},
	{"DS1152D", RIGOL_DS1000, PROTOCOL_LEGACY, {2, 1000000000}, {50, 1}, {2, 1000}, true, 12},
	{"DS2072", RIGOL_DS2000, PROTOCOL_IEEE488_2, {5, 1000000000}, {500, 1}, {500, 1000000}, false, 14},
	{"DS2102", RIGOL_DS2000, PROTOCOL_IEEE488_2, {5, 1000000000}, {500, 1}, {500, 1000000}, false, 14},
	{"DS2202", RIGOL_DS2000, PROTOCOL_IEEE488_2, {2, 1000000000}, {500, 1}, {500, 1000000}, false, 14},
};

SR_PRIV struct sr_dev_driver rigol_ds_driver_info;
static struct sr_dev_driver *di = &rigol_ds_driver_info;

static void clear_helper(void *priv)
{
	struct dev_context *devc;

	devc = priv;
	g_free(devc->data);
	g_free(devc->buffer);
	g_free(devc->coupling[0]);
	g_free(devc->coupling[1]);
	g_free(devc->trigger_source);
	g_free(devc->trigger_slope);
	g_slist_free(devc->analog_groups[0].probes);
	g_slist_free(devc->analog_groups[1].probes);
	g_slist_free(devc->digital_group.probes);
}

static int dev_clear(void)
{
	return std_dev_clear(di, clear_helper);
}

static int set_cfg(const struct sr_dev_inst *sdi, const char *format, ...)
{
	va_list args;
	char buf[256];

	va_start(args, format);
	vsnprintf(buf, 255, format, args);
	va_end(args);
	if (rigol_ds_send(sdi, buf) != SR_OK)
		return SR_ERR;

	/* When setting a bunch of parameters in a row, the DS1052E scrambles
	 * some of them unless there is at least 100ms delay in between. */
	sr_spew("delay %dms", 100);
	g_usleep(100000);

	return SR_OK;
}

static int init(struct sr_context *sr_ctx)
{
	return std_init(sr_ctx, di, LOG_PREFIX);
}

static int probe_port(const char *port, GSList **devices)
{
	struct dev_context *devc;
	struct sr_dev_inst *sdi;
	struct sr_usbtmc_dev_inst *usbtmc;
	struct sr_probe *probe;
	unsigned int i;
	int len, num_tokens;
	const char *manufacturer, *model_name, *version;
	const struct rigol_ds_model *model = NULL;
	char buf[256];
	gchar **tokens, *channel_name;

	*devices = NULL;
	if (!(usbtmc = sr_usbtmc_dev_inst_new(port)))
		return SR_ERR_MALLOC;

	if ((usbtmc->fd = open(usbtmc->device, O_RDWR)) < 0)
		return SR_ERR;
	len = write(usbtmc->fd, "*IDN?", 5);
	len = read(usbtmc->fd, buf, sizeof(buf));
	if (close(usbtmc->fd) < 0)
		return SR_ERR;

	sr_usbtmc_dev_inst_free(usbtmc);

	if (len == 0)
		return SR_ERR_NA;

	buf[len] = 0;
	tokens = g_strsplit(buf, ",", 0);
	sr_dbg("response: %s [%s]", port, buf);

	for (num_tokens = 0; tokens[num_tokens] != NULL; num_tokens++);

	if (num_tokens < 4) {
		g_strfreev(tokens);
		return SR_ERR_NA;
	}

	manufacturer = tokens[0];
	model_name = tokens[1];
	version = tokens[3];

	if (strcasecmp(manufacturer, "Rigol Technologies")) {
		g_strfreev(tokens);
		return SR_ERR_NA;
	}

	for (i = 0; i < ARRAY_SIZE(supported_models); i++) {
		if (!strcmp(model_name, supported_models[i].name)) {
			model = &supported_models[i];
			break;
		}
	}

	if (!model || !(sdi = sr_dev_inst_new(0, SR_ST_ACTIVE,
					      manufacturer, model_name, version))) {
		g_strfreev(tokens);
		return SR_ERR_NA;
	}

	g_strfreev(tokens);

	if (!(sdi->conn = sr_usbtmc_dev_inst_new(port)))
		return SR_ERR_MALLOC;
	sdi->driver = di;
	sdi->inst_type = SR_INST_USBTMC;

	if (!(devc = g_try_malloc0(sizeof(struct dev_context))))
		return SR_ERR_MALLOC;
	devc->limit_frames = 0;
	devc->model = model;

	for (i = 0; i < 2; i++) {
		channel_name = (i == 0 ? "CH1" : "CH2");
		if (!(probe = sr_probe_new(i, SR_PROBE_ANALOG, TRUE, channel_name)))
			return SR_ERR_MALLOC;
		sdi->probes = g_slist_append(sdi->probes, probe);
		devc->analog_groups[i].name = channel_name;
		devc->analog_groups[i].probes = g_slist_append(NULL, probe);
		sdi->probe_groups = g_slist_append(sdi->probe_groups,
				&devc->analog_groups[i]);
	}

	if (devc->model->has_digital) {
		for (i = 0; i < 16; i++) {
			if (!(channel_name = g_strdup_printf("D%d", i)))
				return SR_ERR_MALLOC;
			probe = sr_probe_new(i, SR_PROBE_LOGIC, TRUE, channel_name);
			g_free(channel_name);
			if (!probe)
				return SR_ERR_MALLOC;
			sdi->probes = g_slist_append(sdi->probes, probe);
			devc->digital_group.probes = g_slist_append(
					devc->digital_group.probes, probe);
		}
		devc->digital_group.name = "LA";
		sdi->probe_groups = g_slist_append(sdi->probe_groups,
				&devc->digital_group);
	}

	for (i = 0; i < NUM_TIMEBASE; i++) {
		if (!memcmp(&devc->model->min_timebase, &timebases[i], sizeof(uint64_t[2])))
			devc->timebases = &timebases[i];
		if (!memcmp(&devc->model->max_timebase, &timebases[i], sizeof(uint64_t[2])))
			devc->num_timebases = &timebases[i] - devc->timebases + 1;
	}

	for (i = 0; i < NUM_VDIV; i++) {
		if (!memcmp(&devc->model->min_vdiv, &vdivs[i], sizeof(uint64_t[2]))) {
			devc->vdivs = &vdivs[i];
			devc->num_vdivs = NUM_VDIV - (&vdivs[i] - &vdivs[0]);
		}
	}

	if (!(devc->buffer = g_try_malloc(ACQ_BUFFER_SIZE)))
		return SR_ERR_MALLOC;
	if (!(devc->data = g_try_malloc(ACQ_BUFFER_SIZE * sizeof(float))))
		return SR_ERR_MALLOC;

	devc->data_source = DATA_SOURCE_LIVE;

	sdi->priv = devc;

	*devices = g_slist_append(NULL, sdi);

	return SR_OK;
}

static GSList *scan(GSList *options)
{
	struct drv_context *drvc;
	struct sr_config *src;
	GSList *l, *devices;
	GDir *dir;
	int ret;
	const gchar *dev_name;
	gchar *port = NULL;

	drvc = di->priv;

	for (l = options; l; l = l->next) {
		src = l->data;
		if (src->key == SR_CONF_CONN) {
			port = (char *)g_variant_get_string(src->data, NULL);
			break;
		}
	}

	devices = NULL;
	if (port) {
		if (probe_port(port, &devices) == SR_ERR_MALLOC)
			return NULL;
	} else {
		if (!(dir = g_dir_open("/sys/class/usbmisc/", 0, NULL)))
			if (!(dir = g_dir_open("/sys/class/usb/", 0, NULL)))
				return NULL;
		while ((dev_name = g_dir_read_name(dir))) {
			if (strncmp(dev_name, "usbtmc", 6))
				continue;
			port = g_strconcat("/dev/", dev_name, NULL);
			ret = probe_port(port, &devices);
			g_free(port);
			if (ret == SR_ERR_MALLOC) {
				g_dir_close(dir);
				return NULL;
			}
		}
		g_dir_close(dir);
	}

	/* Tack a copy of the newly found devices onto the driver list. */
	l = g_slist_copy(devices);
	drvc->instances = g_slist_concat(drvc->instances, l);

	return devices;
}

static GSList *dev_list(void)
{
	return ((struct drv_context *)(di->priv))->instances;
}

static int dev_open(struct sr_dev_inst *sdi)
{
	struct sr_usbtmc_dev_inst *usbtmc = sdi->conn;

	if ((usbtmc->fd = open(usbtmc->device, O_RDWR)) < 0)
		return SR_ERR;

	if (rigol_ds_get_dev_cfg(sdi) != SR_OK)
		return SR_ERR;

	sdi->status = SR_ST_ACTIVE;

	return SR_OK;
}

static int dev_close(struct sr_dev_inst *sdi)
{
	struct sr_usbtmc_dev_inst *usbtmc;

	usbtmc = sdi->conn;
	if (usbtmc && usbtmc->fd != -1) {
		close(usbtmc->fd);
		usbtmc->fd = -1;
		sdi->status = SR_ST_INACTIVE;
	}

	return SR_OK;
}

static int cleanup(void)
{
	return dev_clear();
}

static int config_get(int id, GVariant **data, const struct sr_dev_inst *sdi,
		const struct sr_probe_group *probe_group)
{
	struct dev_context *devc;
	unsigned int i;

	if (!sdi || !(devc = sdi->priv))
		return SR_ERR_ARG;

	/* If a probe group is specified, it must be a valid one. */
	if (probe_group) {
		if (probe_group != &devc->analog_groups[0]
				&& probe_group != &devc->analog_groups[1]) {
			sr_err("Invalid probe group specified.");
			return SR_ERR;
		}
	}

	switch (id) {
	case SR_CONF_NUM_TIMEBASE:
		*data = g_variant_new_int32(devc->num_timebases);
		break;
	case SR_CONF_NUM_VDIV:
		if (!probe_group) {
			sr_err("No probe group specified.");
			return SR_ERR_PROBE_GROUP;
		}
		for (i = 0; i < 2; i++) {
			if (probe_group == &devc->analog_groups[i]) {
				*data = g_variant_new_int32(devc->num_vdivs);
				return SR_OK;
			}
		}
		return SR_ERR_NA;
	case SR_CONF_DATA_SOURCE:
		if (devc->data_source == DATA_SOURCE_LIVE)
			*data = g_variant_new_string("Live");
		else if (devc->data_source == DATA_SOURCE_MEMORY)
			*data = g_variant_new_string("Memory");
		else
			*data = g_variant_new_string("Segmented");
		break;
	default:
		return SR_ERR_NA;
	}

	return SR_OK;
}

static int config_set(int id, GVariant *data, const struct sr_dev_inst *sdi,
		const struct sr_probe_group *probe_group)
{
	struct dev_context *devc;
	uint64_t tmp_u64, p, q;
	double t_dbl;
	unsigned int i, j;
	int ret;
	const char *tmp_str;

	if (!(devc = sdi->priv))
		return SR_ERR_ARG;

	if (sdi->status != SR_ST_ACTIVE)
		return SR_ERR_DEV_CLOSED;

	/* If a probe group is specified, it must be a valid one. */
	if (probe_group) {
		if (probe_group != &devc->analog_groups[0]
				&& probe_group != &devc->analog_groups[1]) {
			sr_err("Invalid probe group specified.");
			return SR_ERR;
		}
	}

	ret = SR_OK;
	switch (id) {
	case SR_CONF_LIMIT_FRAMES:
		devc->limit_frames = g_variant_get_uint64(data);
		break;
	case SR_CONF_TRIGGER_SLOPE:
		tmp_u64 = g_variant_get_uint64(data);
		if (tmp_u64 != 0 && tmp_u64 != 1)
			return SR_ERR;
		g_free(devc->trigger_slope);
		devc->trigger_slope = g_strdup(tmp_u64 ? "POS" : "NEG");
		ret = set_cfg(sdi, ":TRIG:EDGE:SLOP %s", devc->trigger_slope);
		break;
	case SR_CONF_HORIZ_TRIGGERPOS:
		t_dbl = g_variant_get_double(data);
		if (t_dbl < 0.0 || t_dbl > 1.0)
			return SR_ERR;
		devc->horiz_triggerpos = t_dbl;
		/* We have the trigger offset as a percentage of the frame, but
		 * need to express this in seconds. */
		t_dbl = -(devc->horiz_triggerpos - 0.5) * devc->timebase * devc->num_timebases;
		ret = set_cfg(sdi, ":TIM:OFFS %.6f", t_dbl);
		break;
	case SR_CONF_TIMEBASE:
		g_variant_get(data, "(tt)", &p, &q);
		for (i = 0; i < devc->num_timebases; i++) {
			if (devc->timebases[i][0] == p && devc->timebases[i][1] == q) {
				devc->timebase = (float)p / q;
				ret = set_cfg(sdi, ":TIM:SCAL %.9f", devc->timebase);
				break;
			}
		}
		if (i == devc->num_timebases)
			ret = SR_ERR_ARG;
		break;
	case SR_CONF_TRIGGER_SOURCE:
		tmp_str = g_variant_get_string(data, NULL);
		for (i = 0; i < ARRAY_SIZE(trigger_sources); i++) {
			if (!strcmp(trigger_sources[i], tmp_str)) {
				g_free(devc->trigger_source);
				devc->trigger_source = g_strdup(trigger_sources[i]);
				if (!strcmp(devc->trigger_source, "AC Line"))
					tmp_str = "ACL";
				else if (!strcmp(devc->trigger_source, "CH1"))
					tmp_str = "CHAN1";
				else if (!strcmp(devc->trigger_source, "CH2"))
					tmp_str = "CHAN2";
				else
					tmp_str = (char *)devc->trigger_source;
				ret = set_cfg(sdi, ":TRIG:EDGE:SOUR %s", tmp_str);
				break;
			}
		}
		if (i == ARRAY_SIZE(trigger_sources))
			ret = SR_ERR_ARG;
		break;
	case SR_CONF_VDIV:
		if (!probe_group) {
			sr_err("No probe group specified.");
			return SR_ERR_PROBE_GROUP;
		}
		g_variant_get(data, "(tt)", &p, &q);
		for (i = 0; i < 2; i++) {
			if (probe_group == &devc->analog_groups[i]) {
				for (j = 0; j < ARRAY_SIZE(vdivs); j++) {
					if (vdivs[j][0] != p || vdivs[j][1] != q)
						continue;
					devc->vdiv[i] = (float)p / q;
					return set_cfg(sdi, ":CHAN%d:SCAL %.3f", i + 1,
							devc->vdiv[i]);
				}
				return SR_ERR_ARG;
			}
		}
		return SR_ERR_NA;
	case SR_CONF_COUPLING:
		if (!probe_group) {
			sr_err("No probe group specified.");
			return SR_ERR_PROBE_GROUP;
		}
		tmp_str = g_variant_get_string(data, NULL);
		for (i = 0; i < 2; i++) {
			if (probe_group == &devc->analog_groups[i]) {
				for (j = 0; j < ARRAY_SIZE(coupling); j++) {
					if (!strcmp(tmp_str, coupling[j])) {
						g_free(devc->coupling[i]);
						devc->coupling[i] = g_strdup(coupling[j]);
						return set_cfg(sdi, ":CHAN%d:COUP %s", i + 1,
								devc->coupling[i]);
					}
				}
				return SR_ERR_ARG;
			}
		}
		return SR_ERR_NA;
	case SR_CONF_DATA_SOURCE:
		tmp_str = g_variant_get_string(data, NULL);
		if (!strcmp(tmp_str, "Live"))
			devc->data_source = DATA_SOURCE_LIVE;
		else if (!strcmp(tmp_str, "Memory"))
			devc->data_source = DATA_SOURCE_MEMORY;
		else if (devc->model->protocol == PROTOCOL_IEEE488_2
			 && !strcmp(tmp_str, "Segmented"))
			devc->data_source = DATA_SOURCE_SEGMENTED;
		else
			return SR_ERR;
		break;
	default:
		ret = SR_ERR_NA;
		break;
	}

	return ret;
}

static int config_list(int key, GVariant **data, const struct sr_dev_inst *sdi,
		const struct sr_probe_group *probe_group)
{
	GVariant *tuple, *rational[2];
	GVariantBuilder gvb;
	unsigned int i;
	struct dev_context *devc = NULL;

	if (sdi)
		devc = sdi->priv;

	if (key == SR_CONF_SCAN_OPTIONS) {
		*data = g_variant_new_fixed_array(G_VARIANT_TYPE_INT32,
				hwopts, ARRAY_SIZE(hwopts), sizeof(int32_t));
		return SR_OK;
	} else if (key == SR_CONF_DEVICE_OPTIONS && probe_group == NULL) {
		*data = g_variant_new_fixed_array(G_VARIANT_TYPE_INT32,
			hwcaps, ARRAY_SIZE(hwcaps), sizeof(int32_t));
		return SR_OK;
	}

	/* Every other option requires a valid device instance. */
	if (!sdi || !(devc = sdi->priv))
		return SR_ERR_ARG;

	/* If a probe group is specified, it must be a valid one. */
	if (probe_group) {
		if (probe_group != &devc->analog_groups[0]
				&& probe_group != &devc->analog_groups[1]) {
			sr_err("Invalid probe group specified.");
			return SR_ERR;
		}
	}

	switch (key) {
		break;
	case SR_CONF_DEVICE_OPTIONS:
		if (!probe_group) {
			sr_err("No probe group specified.");
			return SR_ERR_PROBE_GROUP;
		}
		if (probe_group == &devc->digital_group) {
			*data = g_variant_new_fixed_array(G_VARIANT_TYPE_INT32,
				NULL, 0, sizeof(int32_t));
			return SR_OK;
		} else {
			for (i = 0; i < 2; i++) {
				if (probe_group == &devc->analog_groups[i]) {
					*data = g_variant_new_fixed_array(G_VARIANT_TYPE_INT32,
						analog_hwcaps, ARRAY_SIZE(analog_hwcaps), sizeof(int32_t));
					return SR_OK;
				}
			}
			return SR_ERR_NA;
		}
		break;
	case SR_CONF_COUPLING:
		if (!probe_group) {
			sr_err("No probe group specified.");
			return SR_ERR_PROBE_GROUP;
		}
		*data = g_variant_new_strv(coupling, ARRAY_SIZE(coupling));
		break;
	case SR_CONF_VDIV:
		if (!devc)
			/* Can't know this until we have the exact model. */
			return SR_ERR_ARG;
		if (!probe_group) {
			sr_err("No probe group specified.");
			return SR_ERR_PROBE_GROUP;
		}
		g_variant_builder_init(&gvb, G_VARIANT_TYPE_ARRAY);
		for (i = 0; i < devc->num_vdivs; i++) {
			rational[0] = g_variant_new_uint64(devc->vdivs[i][0]);
			rational[1] = g_variant_new_uint64(devc->vdivs[i][1]);
			tuple = g_variant_new_tuple(rational, 2);
			g_variant_builder_add_value(&gvb, tuple);
		}
		*data = g_variant_builder_end(&gvb);
		break;
	case SR_CONF_TIMEBASE:
		if (!devc)
			/* Can't know this until we have the exact model. */
			return SR_ERR_ARG;
		g_variant_builder_init(&gvb, G_VARIANT_TYPE_ARRAY);
		for (i = 0; i < devc->num_timebases; i++) {
			rational[0] = g_variant_new_uint64(devc->timebases[i][0]);
			rational[1] = g_variant_new_uint64(devc->timebases[i][1]);
			tuple = g_variant_new_tuple(rational, 2);
			g_variant_builder_add_value(&gvb, tuple);
		}
		*data = g_variant_builder_end(&gvb);
		break;
	case SR_CONF_TRIGGER_SOURCE:
		if (!devc)
			/* Can't know this until we have the exact model. */
			return SR_ERR_ARG;
		*data = g_variant_new_strv(trigger_sources,
				devc->model->has_digital ? ARRAY_SIZE(trigger_sources) : 4);
		break;
	case SR_CONF_DATA_SOURCE:
		if (!devc)
			/* Can't know this until we have the exact model. */
			return SR_ERR_ARG;
		/* This needs tweaking by series/model! */
		if (devc->model->series == RIGOL_DS2000)
			*data = g_variant_new_strv(data_sources, ARRAY_SIZE(data_sources));
		else
			*data = g_variant_new_strv(data_sources, ARRAY_SIZE(data_sources) - 1);
		break;
	default:
		return SR_ERR_NA;
	}

	return SR_OK;
}

static int dev_acquisition_start(const struct sr_dev_inst *sdi, void *cb_data)
{
	struct sr_usbtmc_dev_inst *usbtmc;
	struct dev_context *devc;
	struct sr_probe *probe;
	GSList *l;
	char cmd[256];

	if (sdi->status != SR_ST_ACTIVE)
		return SR_ERR_DEV_CLOSED;

	usbtmc = sdi->conn;
	devc = sdi->priv;

	if (devc->data_source == DATA_SOURCE_LIVE) {
		if (rigol_ds_send(sdi, ":RUN") != SR_OK)
			return SR_ERR;
	} else if (devc->data_source == DATA_SOURCE_MEMORY) {
		if (devc->model->series != RIGOL_DS2000) {
			sr_err("Data source 'Memory' not supported for this device");
			return SR_ERR;
		}
	} else if (devc->data_source == DATA_SOURCE_SEGMENTED) {
		sr_err("Data source 'Segmented' not yet supported");
		return SR_ERR;
	}

	for (l = sdi->probes; l; l = l->next) {
		probe = l->data;
		sr_dbg("handling probe %s", probe->name);
		if (probe->type == SR_PROBE_ANALOG) {
			if (probe->enabled)
				devc->enabled_analog_probes = g_slist_append(
						devc->enabled_analog_probes, probe);
			if (probe->enabled != devc->analog_channels[probe->index]) {
				/* Enabled channel is currently disabled, or vice versa. */
				sprintf(cmd, ":CHAN%d:DISP %s", probe->index + 1,
						probe->enabled ? "ON" : "OFF");
				if (rigol_ds_send(sdi, cmd) != SR_OK)
					return SR_ERR;
			}
		} else if (probe->type == SR_PROBE_LOGIC) {
			if (probe->enabled)
				devc->enabled_digital_probes = g_slist_append(
						devc->enabled_digital_probes, probe);
			if (probe->enabled != devc->digital_channels[probe->index]) {
				/* Enabled channel is currently disabled, or vice versa. */
				sprintf(cmd, ":DIG%d:TURN %s", probe->index,
						probe->enabled ? "ON" : "OFF");
				if (rigol_ds_send(sdi, cmd) != SR_OK)
					return SR_ERR;
			}
		}
	}
	if (!devc->enabled_analog_probes && !devc->enabled_digital_probes)
		return SR_ERR;

	sr_source_add(usbtmc->fd, G_IO_IN, 50, rigol_ds_receive, (void *)sdi);

	/* Send header packet to the session bus. */
	std_session_send_df_header(cb_data, LOG_PREFIX);

	if (devc->model->protocol == PROTOCOL_LEGACY) {
		/* Fetch the first frame. */
		if (devc->enabled_analog_probes) {
			devc->analog_frame_size = DS1000_ANALOG_LIVE_WAVEFORM_SIZE;
			devc->channel_frame = devc->enabled_analog_probes->data;
			if (rigol_ds_send(sdi, ":WAV:DATA? CHAN%d",
					devc->channel_frame->index + 1) != SR_OK)
				return SR_ERR;
		} else {
			devc->channel_frame = devc->enabled_digital_probes->data;
			if (rigol_ds_send(sdi, ":WAV:DATA? DIG") != SR_OK)
				return SR_ERR;
		}

		devc->num_frame_bytes = 0;
	} else {
		if (devc->enabled_analog_probes) {
			if (devc->data_source == DATA_SOURCE_MEMORY)
			{
				if (g_slist_length(devc->enabled_analog_probes) == 1)
					devc->analog_frame_size = DS2000_ANALOG_MEM_WAVEFORM_SIZE_1C;
				else
					devc->analog_frame_size = DS2000_ANALOG_MEM_WAVEFORM_SIZE_2C;
				/* Apparently for the DS2000 the memory
				 * depth can only be set in Running state -
				 * this matches the behaviour of the UI. */
				if (rigol_ds_send(sdi, ":RUN") != SR_OK)
					return SR_ERR;
				if (rigol_ds_send(sdi, "ACQ:MDEP %d", devc->analog_frame_size) != SR_OK)
					return SR_ERR;
				if (rigol_ds_send(sdi, ":STOP") != SR_OK)
					return SR_ERR;
			} else
				devc->analog_frame_size = DS2000_ANALOG_LIVE_WAVEFORM_SIZE;
			devc->channel_frame = devc->enabled_analog_probes->data;
			if (rigol_ds_capture_start(sdi) != SR_OK)
				return SR_ERR;
		}
	}

	return SR_OK;
}

static int dev_acquisition_stop(struct sr_dev_inst *sdi, void *cb_data)
{
	struct dev_context *devc;
	struct sr_usbtmc_dev_inst *usbtmc;

	(void)cb_data;

	devc = sdi->priv;

	if (sdi->status != SR_ST_ACTIVE) {
		sr_err("Device inactive, can't stop acquisition.");
		return SR_ERR;
	}

	g_slist_free(devc->enabled_analog_probes);
	g_slist_free(devc->enabled_digital_probes);
	devc->enabled_analog_probes = NULL;
	devc->enabled_digital_probes = NULL;
	usbtmc = sdi->conn;
	sr_source_remove(usbtmc->fd);

	return SR_OK;
}

SR_PRIV struct sr_dev_driver rigol_ds_driver_info = {
	.name = "rigol-ds",
	.longname = "Rigol DS",
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
