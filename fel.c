#include <fel.h>

extern struct chip_t a10;
extern struct chip_t a13_a10s_r8;
extern struct chip_t a20;
extern struct chip_t a23;
extern struct chip_t a31;
extern struct chip_t a33_r16;
extern struct chip_t a40i_r40;
extern struct chip_t a64;
extern struct chip_t a80;
extern struct chip_t a83t;
extern struct chip_t d1_f133;
extern struct chip_t f1c100s_f1c200s_f1c500s;
extern struct chip_t h2_h3;
extern struct chip_t h5;
extern struct chip_t h6;
extern struct chip_t h616;
extern struct chip_t r328;
extern struct chip_t r329;
extern struct chip_t r528_t113;
extern struct chip_t t507;
extern struct chip_t v3s_s3;
extern struct chip_t v536;
extern struct chip_t v831;
extern struct chip_t v853;

static struct chip_t * chips[] = {
	&a10,
	&a13_a10s_r8,
	&a20,
	&a23,
	&a31,
	&a33_r16,
	&a40i_r40,
	&a64,
	&a80,
	&a83t,
	&d1_f133,
	&f1c100s_f1c200s_f1c500s,
	&h2_h3,
	&h5,
	&h6,
	&h616,
	&r328,
	&r329,
	&r528_t113,
	&t507,
	&v3s_s3,
	&v536,
	&v831,
	&v853,
};

struct usb_request_t {
	char magic[8];
	uint32_t length;
	uint32_t unknown1;
	uint16_t request;
	uint32_t length2;
	char pad[10];
} __attribute__((packed));

struct fel_request_t {
	uint32_t request;
	uint32_t address;
	uint32_t length;
	uint32_t pad;
} __attribute__((packed));

static inline void usb_bulk_send(libusb_device_handle * hdl, int ep, const char * buf, size_t len)
{
	size_t max_chunk = 128 * 1024;
	size_t chunk;
	int r, bytes;

	while(len > 0)
	{
		chunk = len < max_chunk ? len : max_chunk;
		r = libusb_bulk_transfer(hdl, ep, (void *)buf, chunk, &bytes, 10000);
		if(r != 0)
		{
			printf("usb bulk send error\r\n");
			exit(-1);
		}
		len -= bytes;
		buf += bytes;
	}
}

static inline void usb_bulk_recv(libusb_device_handle * hdl, int ep, char * buf, size_t len)
{
	int r, bytes;

	while(len > 0)
	{
		r = libusb_bulk_transfer(hdl, ep, (void *)buf, len, &bytes, 10000);
		if(r != 0)
		{
			printf("usb bulk recv error\r\n");
			exit(-1);
		}
		len -= bytes;
		buf += bytes;
	}
}

static inline void send_usb_request(struct xfel_ctx_t * ctx, int type, size_t length)
{
	struct usb_request_t req = {
		.magic = "AWUC",
		.request = cpu_to_le16(type),
		.length = cpu_to_le32(length),
		.unknown1 = cpu_to_le32(0x0c000000)
	};
	req.length2 = req.length;
	usb_bulk_send(ctx->hdl, ctx->epout, (const char *)&req, sizeof(struct usb_request_t));
}

static inline void read_usb_response(struct xfel_ctx_t * ctx)
{
	char buf[13];

	usb_bulk_recv(ctx->hdl, ctx->epin, (char *)buf, sizeof(buf));
	assert(strcmp(buf, "AWUS") == 0);
}

static inline void usb_write(struct xfel_ctx_t * ctx, const void * buf, size_t len)
{
	send_usb_request(ctx, 0x12, len);
	usb_bulk_send(ctx->hdl, ctx->epout, (const char *)buf, len);
	read_usb_response(ctx);
}

static inline void usb_read(struct xfel_ctx_t * ctx, const void * data, size_t len)
{
	send_usb_request(ctx, 0x11, len);
	usb_bulk_send(ctx->hdl, ctx->epin, (const char *)data, len);
	read_usb_response(ctx);
}

static inline void send_fel_request(struct xfel_ctx_t * ctx, int type, uint32_t addr, uint32_t length)
{
	struct fel_request_t req = {
		.request = cpu_to_le32(type),
		.address = cpu_to_le32(addr),
		.length = cpu_to_le32(length)
	};
	usb_write(ctx, &req, sizeof(struct fel_request_t));
}

static inline void read_fel_status(struct xfel_ctx_t * ctx)
{
	char buf[8];
	usb_read(ctx, buf, sizeof(buf));
}

static inline int fel_version(struct xfel_ctx_t * ctx)
{
	int i;

	send_fel_request(ctx, 0x001, 0, 0);
	usb_read(ctx, &ctx->version, sizeof(ctx->version));
	read_fel_status(ctx);
	ctx->version.id = le32_to_cpu(ctx->version.id);
	ctx->version.firmware = le32_to_cpu(ctx->version.firmware);
	ctx->version.protocol = le16_to_cpu(ctx->version.protocol);
	ctx->version.scratchpad = le32_to_cpu(ctx->version.scratchpad);
	for(i = 0; i < ARRAY_SIZE(chips); i++)
	{
		ctx->chip = chips[i];
		if(fel_chip_detect(ctx, ctx->version.id))
			return 1;
	}
	printf("WARNING: Not yet support this device ID 0x%08x\r\n", ctx->version.id);
	return 0;
}

int fel_init(struct xfel_ctx_t * ctx)
{
	if(ctx && ctx->hdl)
	{
		struct libusb_config_descriptor * config;
		int if_idx, set_idx, ep_idx;
		const struct libusb_interface * iface;
		const struct libusb_interface_descriptor * setting;
		const struct libusb_endpoint_descriptor * ep;
		if(libusb_claim_interface(ctx->hdl, 0) == 0)
		{
			if(libusb_get_active_config_descriptor(libusb_get_device(ctx->hdl), &config) == 0)
			{
				for(if_idx = 0; if_idx < config->bNumInterfaces; if_idx++)
				{
					iface = config->interface + if_idx;
					for(set_idx = 0; set_idx < iface->num_altsetting; set_idx++)
					{
						setting = iface->altsetting + set_idx;
						for(ep_idx = 0; ep_idx < setting->bNumEndpoints; ep_idx++)
						{
							ep = setting->endpoint + ep_idx;
							if((ep->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) != LIBUSB_TRANSFER_TYPE_BULK)
								continue;
							if((ep->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK) == LIBUSB_ENDPOINT_IN)
								ctx->epin = ep->bEndpointAddress;
							else
								ctx->epout = ep->bEndpointAddress;
						}
					}
				}
				libusb_free_config_descriptor(config);
				return fel_version(ctx);
			}
		}
	}
	return 0;
}

void fel_exec(struct xfel_ctx_t * ctx, uint32_t addr)
{
	send_fel_request(ctx, 0x102, addr, 0);
	read_fel_status(ctx);
}

static inline void fel_read_raw(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len)
{
	send_fel_request(ctx, 0x103, addr, len);
	usb_read(ctx, buf, len);
	read_fel_status(ctx);
}

static inline void fel_write_raw(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len)
{
	send_fel_request(ctx, 0x101, addr, len);
	usb_write(ctx, buf, len);
	read_fel_status(ctx);
}

uint32_t fel_read32(struct xfel_ctx_t * ctx, uint32_t addr)
{
	uint32_t val;
	fel_read_raw(ctx, addr, &val, sizeof(uint32_t));
	return val;
}

void fel_write32(struct xfel_ctx_t * ctx, uint32_t addr, uint32_t val)
{
	fel_write_raw(ctx, addr, &val, sizeof(uint32_t));
}

void fel_read(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len)
{
	size_t n;

	while(len > 0)
	{
		n = len > 65536 ? 65536 : len;
		fel_read_raw(ctx, addr, buf, n);
		addr += n;
		buf += n;
		len -= n;
	}
}

void fel_write(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len)
{
	size_t n;

	while(len > 0)
	{
		n = len > 65536 ? 65536 : len;
		fel_write_raw(ctx, addr, buf, n);
		addr += n;
		buf += n;
		len -= n;
	}
}

void fel_read_progress(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len)
{
	struct progress_t p;
	size_t n;

	progress_start(&p, len);
	while(len > 0)
	{
		n = len > 65536 ? 65536 : len;
		fel_read_raw(ctx, addr, buf, n);
		addr += n;
		buf += n;
		len -= n;
		progress_update(&p, n);
	}
	progress_stop(&p);
}

void fel_write_progress(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len)
{
	struct progress_t p;
	size_t n;

	progress_start(&p, len);
	while(len > 0)
	{
		n = len > 65536 ? 65536 : len;
		fel_write_raw(ctx, addr, buf, n);
		addr += n;
		buf += n;
		len -= n;
		progress_update(&p, n);
	}
	progress_stop(&p);
}

int fel_spi_init(struct xfel_ctx_t * ctx, uint32_t * swapbuf, uint32_t * swaplen, uint32_t * cmdlen)
{
	uint8_t cbuf[2];

	if(!fel_chip_spi_init(ctx, swapbuf, swaplen, cmdlen))
		return 0;
	cbuf[0] = SPI_CMD_INIT;
	cbuf[1] = SPI_CMD_END;
	if(!fel_chip_spi_run(ctx, cbuf, sizeof(cbuf)))
		return 0;
	return 1;
}

int fel_spi_xfer(struct xfel_ctx_t * ctx, uint32_t swapbuf, uint32_t swaplen, uint32_t cmdlen, void * txbuf, uint32_t txlen, void * rxbuf, uint32_t rxlen)
{
	uint8_t cbuf[256];
	uint32_t clen;
	uint32_t n;

	if((txlen <= swaplen) && (rxlen <= swaplen))
	{
		clen = 0;
		cbuf[clen++] = SPI_CMD_SELECT;
		if(txlen > 0)
		{
			cbuf[clen++] = SPI_CMD_TXBUF;
			cbuf[clen++] = (swapbuf >>  0) & 0xff;
			cbuf[clen++] = (swapbuf >>  8) & 0xff;
			cbuf[clen++] = (swapbuf >> 16) & 0xff;
			cbuf[clen++] = (swapbuf >> 24) & 0xff;
			cbuf[clen++] = (txlen >>  0) & 0xff;
			cbuf[clen++] = (txlen >>  8) & 0xff;
			cbuf[clen++] = (txlen >> 16) & 0xff;
			cbuf[clen++] = (txlen >> 24) & 0xff;
		}
		if(rxlen > 0)
		{
			cbuf[clen++] = SPI_CMD_RXBUF;
			cbuf[clen++] = (swapbuf >>  0) & 0xff;
			cbuf[clen++] = (swapbuf >>  8) & 0xff;
			cbuf[clen++] = (swapbuf >> 16) & 0xff;
			cbuf[clen++] = (swapbuf >> 24) & 0xff;
			cbuf[clen++] = (rxlen >>  0) & 0xff;
			cbuf[clen++] = (rxlen >>  8) & 0xff;
			cbuf[clen++] = (rxlen >> 16) & 0xff;
			cbuf[clen++] = (rxlen >> 24) & 0xff;
		}
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_END;
		if(txlen > 0)
			fel_write(ctx, swapbuf, txbuf, txlen);
		if((clen > cmdlen) || !fel_chip_spi_run(ctx, cbuf, clen))
			return 0;
		if(rxlen > 0)
			fel_read(ctx, swapbuf, rxbuf, rxlen);
	}
	else
	{
		clen = 0;
		cbuf[clen++] = SPI_CMD_SELECT;
		cbuf[clen++] = SPI_CMD_END;
		if(!fel_chip_spi_run(ctx, cbuf, clen))
			return 0;
		while(txlen > 0)
		{
			n = txlen > swaplen ? swaplen : txlen;
			clen = 0;
			cbuf[clen++] = SPI_CMD_TXBUF;
			cbuf[clen++] = (swapbuf >>  0) & 0xff;
			cbuf[clen++] = (swapbuf >>  8) & 0xff;
			cbuf[clen++] = (swapbuf >> 16) & 0xff;
			cbuf[clen++] = (swapbuf >> 24) & 0xff;
			cbuf[clen++] = (n >>  0) & 0xff;
			cbuf[clen++] = (n >>  8) & 0xff;
			cbuf[clen++] = (n >> 16) & 0xff;
			cbuf[clen++] = (n >> 24) & 0xff;
			cbuf[clen++] = SPI_CMD_END;
			fel_write(ctx, swapbuf, txbuf, n);
			if(!fel_chip_spi_run(ctx, cbuf, clen))
				return 0;
			txbuf += n;
			txlen -= n;
		}
		while(rxlen > 0)
		{
			n = rxlen > swaplen ? swaplen : rxlen;
			clen = 0;
			cbuf[clen++] = SPI_CMD_RXBUF;
			cbuf[clen++] = (swapbuf >>  0) & 0xff;
			cbuf[clen++] = (swapbuf >>  8) & 0xff;
			cbuf[clen++] = (swapbuf >> 16) & 0xff;
			cbuf[clen++] = (swapbuf >> 24) & 0xff;
			cbuf[clen++] = (n >>  0) & 0xff;
			cbuf[clen++] = (n >>  8) & 0xff;
			cbuf[clen++] = (n >> 16) & 0xff;
			cbuf[clen++] = (n >> 24) & 0xff;
			cbuf[clen++] = SPI_CMD_END;
			if(!fel_chip_spi_run(ctx, cbuf, clen))
				return 0;
			fel_read(ctx, swapbuf, rxbuf, n);
			rxbuf += n;
			rxlen -= n;
		}
		clen = 0;
		cbuf[clen++] = SPI_CMD_DESELECT;
		cbuf[clen++] = SPI_CMD_END;
		if((clen > cmdlen) || !fel_chip_spi_run(ctx, cbuf, clen))
			return 0;
	}
	return 1;
}
