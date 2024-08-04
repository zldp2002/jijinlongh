#include <fel.h>

extern struct chip_t f1c100s;
extern struct chip_t d1;
extern struct chip_t v831;

static struct chip_t * chips[] = {
	&f1c100s,
	&d1,
	&v831,
};

struct usb_request_t {
	char signature[8];
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

static inline void usb_bulk_send(libusb_device_handle * hdl, int ep, const void * buf, size_t len)
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

static inline void usb_bulk_recv(libusb_device_handle * hdl, int ep, void * buf, size_t len)
{
	int r, bytes;

	while(len > 0)
	{
		r = libusb_bulk_transfer(hdl, ep, buf, len, &bytes, 10000);
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
		.signature = "AWUC",
		.request = cpu_to_le16(type),
		.length = cpu_to_le32(length),
		.unknown1 = cpu_to_le32(0x0c000000)
	};
	req.length2 = req.length;
	usb_bulk_send(ctx->hdl, ctx->epout, &req, sizeof(struct usb_request_t));
}

static inline void read_usb_response(struct xfel_ctx_t * ctx)
{
	char buf[13];

	usb_bulk_recv(ctx->hdl, ctx->epin, buf, sizeof(buf));
	assert(strcmp(buf, "AWUS") == 0);
}

static inline void usb_write(struct xfel_ctx_t * ctx, const void * buf, size_t len)
{
	send_usb_request(ctx, 0x12, len);
	usb_bulk_send(ctx->hdl, ctx->epout, buf, len);
	read_usb_response(ctx);
}

static inline void usb_read(struct xfel_ctx_t * ctx, const void * data, size_t len)
{
	send_usb_request(ctx, 0x11, len);
	usb_bulk_send(ctx->hdl, ctx->epin, data, len);
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
	ctx->version.unknown_0a = le32_to_cpu(ctx->version.unknown_0a);
	ctx->version.protocol = le16_to_cpu(ctx->version.protocol);
	ctx->version.scratchpad = le32_to_cpu(ctx->version.scratchpad);
	ctx->version.pad[0] = le32_to_cpu(ctx->version.pad[0]);
	ctx->version.pad[1] = le32_to_cpu(ctx->version.pad[1]);
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
		struct libusb_device * usb = libusb_get_device(ctx->hdl);
		struct libusb_config_descriptor * config;
		int if_idx, set_idx, ep_idx;
		const struct libusb_interface * iface;
		const struct libusb_interface_descriptor * setting;
		const struct libusb_endpoint_descriptor * ep;
		if(libusb_get_active_config_descriptor(usb, &config) == 0)
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
	return 0;
}

struct progressbar_t {
	size_t total;
	size_t done;
	double start;
};

static inline double gettime(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + (double)tv.tv_usec / 1000000.0;
}

static inline const char * format_eta(double remaining)
{
	static char result[6] = "";
	int seconds = remaining + 0.5;
	if(seconds >= 0 && seconds < 6000)
	{
		snprintf(result, sizeof(result), "%02d:%02d", seconds / 60, seconds % 60);
		return result;
	}
	return "--:--";
}

static void progressbar_start(struct progressbar_t * bar, size_t total)
{
	if(bar && (total > 0))
	{
		bar->total = total;
		bar->done = 0;
		bar->start = gettime();
	}
}

static void progressbar_update(struct progressbar_t * bar, size_t bytes)
{
	if(bar)
	{
		bar->done += bytes;
		double ratio = bar->total > 0 ? (double)bar->done / (double)bar->total : 0.0;
		double speed = (double)bar->done / (gettime() - bar->start);
		double eta = speed > 0 ? (bar->total - bar->done) / speed : 0;
		int i, pos = 48 * ratio;
		printf("\r%3.0f%% [", ratio * 100);
		for(i = 0; i < pos; i++)
			putchar('=');
		for(i = pos; i < 48; i++)
			putchar(' ');
		if(bar->done < bar->total)
			printf("]%6.1f kB/s, ETA %s ", speed / 1000.0, format_eta(eta));
		else
			printf("] %5.0f kB, %6.1f kB/s", bar->done / 1000.0, speed / 1000.0);
		fflush(stdout);
	}
}

static void progressbar_stop(struct progressbar_t * bar)
{
	if(bar)
		printf("\r\n");
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
	if(len > 0)
	{
		send_fel_request(ctx, 0x101, addr, len);
		usb_write(ctx, buf, len);
		read_fel_status(ctx);
	}
}

uint32_t fel_read32(struct xfel_ctx_t * ctx, uint32_t addr)
{
	uint32_t val;
	fel_read_raw(ctx, addr, &val, sizeof(val));
	return val;
}

void fel_write32(struct xfel_ctx_t * ctx, uint32_t addr, uint32_t val)
{
	fel_write_raw(ctx, addr, &val, sizeof(val));
}

void fel_read(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len, int progress)
{
	struct progressbar_t bar;
	size_t n;

	if(progress)
		progressbar_start(&bar, len);
	while(len > 0)
	{
		n = len > 256 ? 256 : len;
		fel_read_raw(ctx, addr, buf, n);
		addr += n;
		buf += n;
		len -= n;
		if(progress)
			progressbar_update(&bar, n);
	}
	if(progress)
		progressbar_stop(&bar);
}

void fel_write(struct xfel_ctx_t * ctx, uint32_t addr, void * buf, size_t len, int progress)
{
	struct progressbar_t bar;
	size_t n;

	if(progress)
		progressbar_start(&bar, len);
	while(len > 0)
	{
		n = len > 256 ? 256 : len;
		fel_write_raw(ctx, addr, buf, n);
		addr += n;
		buf += n;
		len -= n;
		if(progress)
			progressbar_update(&bar, n);
	}
	if(progress)
		progressbar_stop(&bar);
}

int fel_chip_sid(struct xfel_ctx_t * ctx)
{
	if(ctx && ctx->chip && ctx->chip->sid)
		return ctx->chip->sid(ctx);
	return 0;
}

int fel_chip_jtag(struct xfel_ctx_t * ctx)
{
	if(ctx && ctx->chip && ctx->chip->jtag)
		return ctx->chip->jtag(ctx);
	return 0;
}

int fel_chip_ddr(struct xfel_ctx_t * ctx)
{
	if(ctx && ctx->chip && ctx->chip->ddr)
		return ctx->chip->ddr(ctx);
	return 0;
}

int fel_chip_reset(struct xfel_ctx_t * ctx)
{
	if(ctx && ctx->chip && ctx->chip->reset)
		return ctx->chip->reset(ctx);
	return 0;
}
