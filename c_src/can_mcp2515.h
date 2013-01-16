/*
 * Controller Area Network (CAN) infrastructure.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

// Slightly modified by Feuerlabs to avoid naming collisions with /usr/include/linux/can.h
#ifndef __CAN_MCP2515_H__
#define __CAN_MCP2515_H__

#define MCP2515_CAN_FRAME_MAX_DATA_LEN 8
#define MCP2515_CAN_FILTER_REG_NUM     6

#define MCP2515_CAN_IOC_MAGIC	'C'
#define MCP2515_CAN_IOCTRESET			_IO(MCP2515_CAN_IOC_MAGIC, 0)			/* reset device */
#define MCP2515_CAN_IOCTWAKEUP			_IO(MCP2515_CAN_IOC_MAGIC, 1)			/* wakup device */
#define MCP2515_CAN_IOCTNORMALMODE		_IO(MCP2515_CAN_IOC_MAGIC, 2)			/* go normal mode */
#define MCP2515_CAN_IOCTLOOPBACKMODE	_IO(MCP2515_CAN_IOC_MAGIC, 3)			/* go loopback mode */
#define MCP2515_CAN_IOCTLISTENONLYMODE	_IO(MCP2515_CAN_IOC_MAGIC, 4)			/* go listen only mode */
#define MCP2515_CAN_IOCTSLEEPMODE		_IO(MCP2515_CAN_IOC_MAGIC, 5)			/* go sleep mode */
#define MCP2515_CAN_IOCSRATE			_IOW(MCP2515_CAN_IOC_MAGIC, 6, int)		/* set baud rate */
#define MCP2515_CAN_IOCSFILTER			_IOW(MCP2515_CAN_IOC_MAGIC, 7, long)	/* set receive filter */
#define MCP2515_CAN_IOCGRATE			_IOR(MCP2515_CAN_IOC_MAGIC, 8, int)		/* get baud rate */
#define MCP2515_CAN_IOCGFILTER			_IOR(MCP2515_CAN_IOC_MAGIC, 9, long)	/* get receive filter */


/**
 * struct mcp2515_can_frame_header - Extended CAN frame header fields
 * @id: 11 bit standard ID
 * @srr: Substitute Remote Request bit
 * @ide: Extended frame bit
 * @eid: 18 bit extended ID
 * @rtr: Remote Transmit Request bit
 * @rb1: Reserved Bit 1
 * @rb0: Reserved Bit 0
 * @dlc: 4 bit Data Length Code
 *
 * For a standard frame ensure that ide == 0 and fill in only id, rtr and dlc.
 *
 * srr, rb1, and rb0 are unused and should be set to zero.
 *
 * Note: The memory layout does not correspond to the on-the-wire format for a
 * CAN frame header.
 */
typedef struct _mcp2515_can_frame_header {
	unsigned id:11;
	unsigned srr:1;
	unsigned ide:1;
	unsigned eid:18;
	unsigned rtr:1;
	unsigned rb1:1;
	unsigned rb0:1;
	unsigned dlc:4;
} mcp2515_can_frame_header;

/**
 * struct mcp2515_can_frame - CAN frame header fields and data
 * @header: CAN frame header
 * @data: 8 bytes of data
 *
 * User space transmits and receives struct mcp2515_can_frame's via the network device
 * socket interface.
 *
 * Note: The memory layout does not correspond to the on-the-wire format for a
 * CAN frame.
 */
typedef struct _mcp2515_can_frame {
	mcp2515_can_frame_header header;
	unsigned char data[MCP2515_CAN_FRAME_MAX_DATA_LEN];
} mcp2515_can_frame;

/**
 * mask/filter register table
 * MASK    FILTER    RECVID    RECEIVE
 *  0       x          x         yes
 *  1       0          0         yes
 *  1       0          1         no
 *  1       1          0         no
 *  1       1          1         yes
 */

/**
 * struct mcp2515_id_filter - CAN device receive filter setup
 * @id: filter of standard frame id.
 * @ide: enable extend frame id filter
 * @eid: filter of extend frame id or standard frame data(data-0 data-1)
 * @active: active this setup, if not all this struct fields will be zero
 */
struct mcp2515_id_filter {
	unsigned id:11;
	unsigned ide:1;
	unsigned eid:18;
	unsigned active:1;
};

/**
 * struct mcp2515_can_filter - CAN device receive filter setup
 *
 * MCP2515 have 6 filter register RXF0 -- RXF5.
 * It have 2 mask register,we set them to same RXM0=RXM1.
 *
 * @fid: see struct mcp2515_id_filter.
 * @sidmask: mask for standard frame id
 * @eidmask: if mcp2515_id_filter.ide=1,it mask for extend frame id. else it mask for standard frame data.
 * @mode: receive filter mode.
 *        0 - Receive all valid messages using either standard or extended identifiers that meet filter
 *        1 - Receive only valid messages with standard identifiers that meet filter criteria
 *        2 - Receive only valid messages with extended identifiers that meet filter criteria
 *        3 - Turn mask/filters off; receive any message
 */
struct mcp2515_can_filter {
	struct mcp2515_id_filter fid[MCP2515_CAN_FILTER_REG_NUM];
	unsigned sidmask:11;
	unsigned eidmask:18;
	unsigned mode:2;
};


#endif /* !__CAN_MCP2515_H__ */
