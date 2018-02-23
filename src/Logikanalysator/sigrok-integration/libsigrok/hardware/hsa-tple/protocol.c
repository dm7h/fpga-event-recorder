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

#include "protocol.h"
#include <stdlib.h>     /* malloc, free, rand */

/* one packet consists of 32 bit of data */
struct data_packet {
	uint8_t  data;
	uint16_t timestamp;
	uint8_t  status;
};

/* send command to the device, this is one ASCII token */
SR_PRIV int send_command(struct sr_serial_dev_inst *serial, uint8_t command)
{
	char buf[1];

	sr_dbg("Sending cmd 0x%.2x / '%c'.", command, command);
	buf[0] = command;
	serial_flush(serial);
	if (serial_write(serial, buf, 1) != 1) {
		sr_spew("could not write serial");
		return SR_ERR;
	}

	sr_spew("serial written, command sent");
	return SR_OK;
}

// parse raw data on the serial interface and get data records out of it
static int packet_parse(struct sr_dev_inst *sdi, struct data_packet *packet)
{
	struct dev_context *devc;
	char strbuf[128];

	devc = sdi->priv;

	// we need to work with blocks of 4 byte
	if(devc->buflen < 4) {
		sr_spew("  packet_parse: PACKET_NEED_MORE_DATA; buflen: %d", devc->buflen);
		return PACKET_NEED_MORE_DATA;
	}
	
	// byte 4 represents the status byte
	packet->status = devc->buf[3];
	packet->data = devc->buf[0];
	packet->timestamp = (devc->buf[1] << 8) | devc->buf[2];
	
	sprintf(strbuf, "  RAW: d: 0x%02X, t: 0x%04X / %d, s: 0x%02X", packet->data, packet->timestamp, packet->timestamp, packet->status);
	sr_spew("%s", strbuf);

	// modify buffer! If we have any data left, move it to the beginning of our buffer.
	// dest, source, length
	memmove(devc->buf, devc->buf + 4, devc->buflen - 4);
	devc->buflen -= 4;

	// FIXME: don't ignore invalid packets
	if( (packet->status != DATA_RECORD_MORE) &&
            (packet->status != DATA_RECORD_LAST) ) {
		sr_warn("  packet_parse: PACKET_INVALID, buflen: %d", devc->buflen);
		sr_spew("%s", strbuf);
		devc->last_record_received = TRUE;
		return PACKET_INVALID;
	}
			
	sr_spew("  packet_parse: PACKET_OK, buflen: %d", devc->buflen);
	return PACKET_OK;
}

// interprete and send data packets as packets to the session bus
static int handle_packet(struct sr_dev_inst *sdi)
{
	struct sr_datafeed_packet packet;
	struct sr_datafeed_logic logic;
	struct dev_context *devc;
	struct data_packet dpacket;
	int ret, num_of_duplicates;
	char * buffer;

	devc = sdi->priv;

	while( packet_parse(sdi, &dpacket) == PACKET_OK ) {
		memset(&logic, 0, sizeof(struct sr_datafeed_logic) );

		packet.type = SR_DF_LOGIC;
		packet.payload = &logic;

		logic.unitsize = 1;
		
		// calculate number of duplicates to send to the session bus
		if( devc->last_timestamp_seen == 0xFFFF ) // overflow exception
		{
			num_of_duplicates = dpacket.timestamp;
			sr_spew("  /!\\ Timestamp overflow detected");
		}
		else
		{
			num_of_duplicates = dpacket.timestamp - devc->last_timestamp_seen;
		}
		devc->num_samples += num_of_duplicates;

		//sr_spew("   last timestamp seen: %d, current timestamp: %d, diff=num of duplicates: %d", devc->last_timestamp_seen, dpacket.timestamp, num_of_duplicates);
					
		// trying to implement correct timestamp/ implement RLE:
		if( num_of_duplicates < 0 )
		{
			sr_err("   negative or zero duplicate value");
			continue;
		}
		else if( num_of_duplicates == 0 && dpacket.timestamp == 0 )
		{
			num_of_duplicates = 1;
		}
		
		logic.length = num_of_duplicates;
		buffer = (char*)malloc(num_of_duplicates);
		memset(buffer, devc->last_sample_seen, num_of_duplicates); // duplicate last value, not current value
		logic.data = buffer;
		sr_session_send(devc->cb_data, &packet);
		free(buffer);
		
		devc->last_sample_seen = dpacket.data;
		devc->last_timestamp_seen = dpacket.timestamp;
		
		if( dpacket.status == DATA_RECORD_LAST ) {	
			devc->last_record_received = TRUE;
			
			buffer = (char*)malloc(1);
			*buffer = devc->last_sample_seen;
			logic.length = 1;
			sr_session_send(devc->cb_data, &packet);
			free(buffer);
			
			devc->num_samples++;
			break;
		}
	}

	return SR_OK;
}

/** handle new data bytes on serial interface */
static void handle_new_data(struct sr_dev_inst *sdi)
{
	struct dev_context *devc;
	struct sr_serial_dev_inst *serial;
	uint64_t time;
	int len;
	int bufferempty;
	char c = 0; // single character buffer

	devc = sdi->priv;
	serial = sdi->conn;
	
	/* Try to get as much data as the buffer can hold. */
	len = BUFSIZE - devc->buflen;
	if (len < 0) {
		sr_err("Local buffer overflow in handle_new_data.");
		return;
	}
	
	len = serial_read(serial, devc->buf + devc->buflen, len);
	if (len < 0) {
		sr_err("Serial port read error: %d.", len);
		return;
	}
	else {
		devc->buflen += len;
		sr_spew("%d Bytes read from serial, buflen: %d", len, devc->buflen);
	}
	
	if( devc->buflen > 0 ) {
		bufferempty = FALSE;
	}
	else
	{
		bufferempty = TRUE;
	}

	//sr_spew("buffer length: %d", devc->buflen);	
	//sr_spew("handle new data, state: %d", devc->state);
	
	
	/* Now try to interprete this data, dependent from the state of communication. */
	if (devc->state == ST_IDENTIFIED) {
		sr_spew("ST_IDENTIFIED should be ST_START instead");
	}
	else if (devc->state == ST_START) {
		sr_spew("ST_START");
		devc->state = ST_SENDCMD_START_MEASUREMENT;
	}
	else if (devc->state == ST_SENDCMD_START_MEASUREMENT) {
		sr_spew("ST_SENDCMD_START_MEASUREMENT");

		// send command to start measurement
		if( send_command(serial, CMD_GO) == SR_OK ) {
			// set start time of acquisition session; FIXME: wait for trigger signal!
			devc->starttime = g_get_monotonic_time();
			sr_spew("  start time: %d", devc->starttime);

			sr_spew("  next state: ST_GETRESP_START_MEASUREMENT");
			devc->state = ST_GETRESP_START_MEASUREMENT;
		}
		else {
			sr_spew("  unable to send command: ST_COM_ERROR");
			devc->state = ST_COM_ERROR;
		}		
	}
	else if (devc->state == ST_GETRESP_START_MEASUREMENT) {
		sr_spew("ST_GETRESP_START_MEASUREMENT");

		// get response of start measurement command
		if( !bufferempty ) {
			c = devc->buf[0];
			devc->buflen = 0; // buffer can be overwritten, so index will be 0 again
			if( c == STATUS_MEASUREMENT_STARTED ) {
				sr_spew("  received: STATUS_MEASUREMENT_STARTED");
				devc->state = ST_WAIT_ENDCONDITIONS;
			}
			else if( c == STATUS_MEASUREMENT_NOT_STARTED ) {
				sr_spew("  received: STATUS_MEASUREMENT_NOT_STARTED");
				// TODO: retry
				devc->state = ST_COM_ERROR;
			}
			else {
				sr_spew("  received: unknown STATUS");
				devc->state = ST_COM_ERROR;
			}
		}
	}
	else if (devc->state == ST_WAIT_ENDCONDITIONS) {
		sr_spew("ST_WAIT_ENDCONDITIONS");
		// wait until our end conditions are met
		
		// check if we got a character
		if( !bufferempty ) {
			c = devc->buf[0];
			devc->buflen = 0; // buffer can be overwritten, so index will be 0 again
			if( c == STATUS_MEMFULL ) {
				sr_spew("  received: STATUS_MEMFULL, next state: ST_SENDCMD_STOP_MEASUREMENT");
				devc->state = ST_SENDCMD_STOP_MEASUREMENT;
			}
			else {
				sr_spew("  next state: ST_COM_ERROR");
				devc->state = ST_COM_ERROR;
			}
		}
		// or if time has elapsed
		else {
			time = g_get_monotonic_time();
			sr_spew("  start time: %d:", devc->starttime);
			sr_spew("  current time: %d", devc->starttime);
			time = time - devc->starttime;
			sr_spew("  difference: %d", time);
			sr_spew("  time limit msec: %d", devc->limit_msec);
			
			// both are times in milliseconds (hopefully)
			if( time > devc->limit_msec ) {
				sr_spew("  time limit exceeded; next state: ST_SENDCMD_STOP_MEASUREMENT");
				devc->state = ST_SENDCMD_STOP_MEASUREMENT;
			}
		}
	}
	else if (devc->state == ST_SENDCMD_STOP_MEASUREMENT) {
		sr_spew("ST_SENDCMD_STOP_MEASUREMENT");
		
		// send command to stop measurement
		if( send_command(serial, CMD_STOP) == SR_OK ) {
			devc->state = ST_GETRESP_STOP_MEASUREMENT;
			sr_spew("  sent: CMD_STOP command");
		}
		else {
			devc->state = ST_COM_ERROR;
			sr_spew("  command CMD_STOP could not be sent");
		}		
	}
	else if (devc->state == ST_GETRESP_STOP_MEASUREMENT) {
		sr_spew("ST_GETRESP_STOP_MEASUREMENT");
		
		// get response of stop measurement command		
		if( !bufferempty ) {
			c = devc->buf[0];
			devc->buflen = 0; // buffer can be overwritten, so index will be 0 again
			if( c == STATUS_MEASUREMENT_NOT_STARTED ) {
				sr_spew("  received: STATUS_MEASUREMENT_NOT_STARTED = measurement stopped");
				sr_spew("  next state: ST_SENDCMD_DUMP_DATA");
				devc->state = ST_SENDCMD_DUMP_DATA;
			}
			else if( c == STATUS_MEASUREMENT_STARTED ) {
				// this is: still running; TODO: retry stopping
				sr_spew("  received: STATUS_MEASUREMENT_STARTED");
				devc->state = ST_COM_ERROR;
			}
			else {
				sr_spew("  received: unknown STATUS");
				devc->state = ST_COM_ERROR;
			}
		}
	}
	else if (devc->state == ST_SENDCMD_DUMP_DATA) {
		sr_spew("ST_SENDCMD_DUMP_DATA");
		
		// send command to dump data
		if( send_command(serial, CMD_DUMP) == SR_OK ) {
			sr_spew("  sent: CMD_DUMP command, next state: ST_GETRESP_DUMP_DATA");
			devc->state = ST_GETRESP_DUMP_DATA;
		}
		else {
			sr_spew("  command CMD_DUMP could not be sent");
			devc->state = ST_COM_ERROR;
		}		
	}
	else if (devc->state == ST_GETRESP_DUMP_DATA) {
		sr_spew("ST_GETRESP_DUMP_DATA");
		
		// get response of dump data command
		if( !bufferempty ) {
			c = devc->buf[0];
			// devc->buflen = 0; // there might be useful data!
			memmove(devc->buf, devc->buf + 1, devc->buflen - 1);
			devc->buflen -= 1;
					
			if( c == STATUS_MEASUREMENT_NOT_STARTED ) {
				sr_spew("  received: STATUS_MEASUREMENT_NOT_STARTED");
				devc->state = ST_GET_DUMP;
			}
			else if( c == STATUS_MEASUREMENT_STARTED ) {
				// this is: still running; TODO: retry stopping/ dumping
				sr_spew("  received: STATUS_MEASUREMENT_STARTED");
				devc->state = ST_COM_ERROR;
			}
			else {
				sr_spew("  received: unknown STATUS");
				devc->state = ST_COM_ERROR;
			}
		}
	}
	else if (devc->state == ST_GET_DUMP) {
		sr_spew("ST_GET_DUMP");

		// get raw data which gets dumped
		handle_packet(sdi);
		if( devc->last_record_received )
		{
			// FIXME/HELPME: this number seems to be a bit low?!
			sr_spew("  devc->num_samples: %d", devc->num_samples);

			sr_spew("  received last record, next state: ST_FINISHED");
			devc->state = ST_FINISHED;
		}
	}
	else if (devc->state == ST_FINISHED) {
		sr_spew("ST_FINISHED");
		// data dump has finished, now send all data? currently already sent before!
	}
	else if (devc->state == ST_COM_ERROR) {
		sr_spew("ST_COM_ERROR");

		// communication error: unexpected response from device
		sr_dbg("  Communication error: unexpected response from device");
	}
}

// hsa_tple_receive_data from serial interface, this function gets polled
SR_PRIV int hsa_tple_receive_data(int fd, int revents, void *cb_data)
{
	struct sr_dev_inst *sdi;
	struct dev_context *devc;
	struct sr_serial_dev_inst *serial;

	(void)serial;
	(void)fd;
	(void)revents;

	if (!(sdi = cb_data))
		return TRUE;

	if (!(devc = sdi->priv))
		return TRUE;

	serial = sdi->conn;

	// check if finished -- before we handle new data
	if (devc->state == ST_FINISHED) {
		sdi->driver->dev_acquisition_stop(sdi, cb_data);
		sr_spew("hsa_tple_receive_data returns successfully");
		return TRUE;
	}
	else if (devc->state == ST_COM_ERROR) {
		sr_spew("hsa_tple_receive_data returns with an error");
		return FALSE;
	}

	// don't care if serial data has arrived or not
	// (some states require also change after time -- at least to calculate end of measurement)
	handle_new_data(sdi);

	return TRUE;
}

