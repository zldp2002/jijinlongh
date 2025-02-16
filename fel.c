#include <fel.h>

extern struct chip_t d1;
extern struct chip_t f1c100s;
extern struct chip_t h3;
extern struct chip_t r329;
extern struct chip_t t507;
extern struct chip_t v3s;
extern struct chip_t v831;

static struct chip_t * chips[] = {
	&d1,
	&f1c100s,
	&h3,
	&r329,
	&t507,
	&v3s,
	&v831,
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

static inline void fel_version(struct xfel_ctx_t * ctx)
{
	int i;

	send_fel_request(ctx, 0x001, 0, 0);
	usb_read(ctx, &ctx->version, sizeof(ctx->version));
	read_fel_status(ctx);
	ctx->version.id = le32_to_cpu(ctx->version.id);
	ctx->version.firmware = le32_to_cpu(ctx->version.firmware);
	ctx->version.protocol = le16_to_cpu(ctx->version.protocol);
	ctx->version.scratchpad = le32_to_cpu(ctx->version.scratchpad);
	ctx->chip = NULL;
	for(i = 0; i < ARRAY_SIZE(chips); i++)
	{
		if(chips[i]->id == ctx->version.id)
		{
			ctx->chip = chips[i];
			break;
		}
	}
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
				fel_version(ctx);
				return 1;
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
	if(len > 0)
	{
		send_fel_request(ctx, 0x103, addr, len);
		usb_read(ctx, buf, len);
		read_fel_status(ctx);
	}
}

static inline void fel_write_raw(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len)
{
	if(len > 0)
	{
		send_fel_request(ctx, 0x101, addr, len);
		usb_write(ctx, buf, len);
		read_fel_status(ctx);
	}
}

uint8_t fel_read8(struct xfel_ctx_t * ctx, uint32_t addr)
{
	uint8_t val;
	fel_read_raw(ctx, addr, &val, sizeof(uint8_t));
	return val;
}

uint16_t fel_read16(struct xfel_ctx_t * ctx, uint32_t addr)
{
	uint16_t val;
	fel_read_raw(ctx, addr, &val, sizeof(uint16_t));
	return val;
}

uint32_t fel_read32(struct xfel_ctx_t * ctx, uint32_t addr)
{
	uint32_t val;
	fel_read_raw(ctx, addr, &val, sizeof(uint32_t));
	return val;
}

void fel_write8(struct xfel_ctx_t * ctx, uint32_t addr, uint8_t val)
{
	fel_write_raw(ctx, addr, &val, sizeof(uint8_t));
}

void fel_write16(struct xfel_ctx_t * ctx, uint32_t addr, uint16_t val)
{
	fel_write_raw(ctx, addr, &val, sizeof(uint16_t));
}

void fel_write32(struct xfel_ctx_t * ctx, uint32_t addr, uint32_t val)
{
	fel_write_raw(ctx, addr, &val, sizeof(uint32_t));
}

void fel_read(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len, int progress)
{
	struct progress_t p;
	size_t n;

	if(progress)
		progress_start(&p, len);
	while(len > 0)
	{
		n = len > 256 ? 256 : len;
		fel_read_raw(ctx, addr, buf, n);
		addr += n;
		buf += n;
		len -= n;
		if(progress)
			progress_update(&p, n);
	}
	if(progress)
		progress_stop(&p);
}

void fel_write(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len, int progress)
{
	struct progress_t p;
	size_t n;

	if(progress)
		progress_start(&p, len);
	while(len > 0)
	{
		n = len > 256 ? 256 : len;
		fel_write_raw(ctx, addr, buf, n);
		addr += n;
		buf += n;
		len -= n;
		if(progress)
			progress_update(&p, n);
	}
	if(progress)
		progress_stop(&p);
}

int fel_chip_reset(struct xfel_ctx_t * ctx)
{
	if(ctx && ctx->chip && ctx->chip->reset)
		return ctx->chip->reset(ctx);
	return 0;
}

int fel_chip_sid(struct xfel_ctx_t * ctx, uint32_t * sid)
{
	if(ctx && ctx->chip && ctx->chip->sid)
		return ctx->chip->sid(ctx, sid);
	return 0;
}

int fel_chip_jtag(struct xfel_ctx_t * ctx)
{
	if(ctx && ctx->chip && ctx->chip->jtag)
		return ctx->chip->jtag(ctx);
	return 0;
}

int fel_chip_ddr(struct xfel_ctx_t * ctx, const char * type)
{
	if(ctx && ctx->chip && ctx->chip->ddr)
		return ctx->chip->ddr(ctx, type);
	return 0;
}

int fel_chip_spi_init(struct xfel_ctx_t * ctx)
{
	if(ctx && ctx->chip && ctx->chip->spi_init)
		return ctx->chip->spi_init(ctx);
	return 0;
}

int fel_chip_spi_exit(struct xfel_ctx_t * ctx)
{
	if(ctx && ctx->chip && ctx->chip->spi_exit)
		return ctx->chip->spi_exit(ctx);
	return 0;
}

int fel_chip_spi_select(struct xfel_ctx_t * ctx)
{
	if(ctx && ctx->chip && ctx->chip->spi_select)
		return ctx->chip->spi_select(ctx);
	return 0;
}

int fel_chip_spi_deselect(struct xfel_ctx_t * ctx)
{
	if(ctx && ctx->chip && ctx->chip->spi_deselect)
		return ctx->chip->spi_deselect(ctx);
	return 0;
}

int fel_chip_spi_xfer(struct xfel_ctx_t * ctx, void * txbuf, int txlen, void * rxbuf, int rxlen)
{
	if(ctx && ctx->chip && ctx->chip->spi_xfer)
		return ctx->chip->spi_xfer(ctx, txbuf, txlen, rxbuf, rxlen);
	return 0;
}
