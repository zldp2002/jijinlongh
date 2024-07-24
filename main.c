#include <fel.h>

static void hexdump(uint32_t offset, void * buf, size_t len)
{
	unsigned char * p = buf;
	size_t i, j;

	for(j = 0; j < len; j += 16)
	{
		printf("%08zx: ", offset + j);
		for(i = 0; i < 16; i++)
		{
			if(j + i < len)
				printf("%02x ", p[j + i]);
			else
				printf("__ ");
		}
		putchar(' ');
		for(i = 0; i < 16; i++)
		{
			if(j + i >= len)
				putchar('.');
			else
				putchar(isprint(p[j + i]) ? p[j + i] : '.');
		}
		printf("\r\n");
	}
}

static void usage(void)
{
	printf("usage:\r\n");
	printf("    xfel help                    - Print this usage summary and exit\r\n");
	printf("    xfel version                 - Show brom version\r\n");
	printf("    xfel hexdump <addr> <length> - Show brom version\r\n");
}

int main(int argc, char * argv[])
{
	struct xfel_ctx_t ctx;

	if(argc < 2)
	{
		usage();
		return -1;
	}
	libusb_init(NULL);
	ctx.hdl = libusb_open_device_with_vid_pid(NULL, 0x1f3a, 0xefe8);
	if(!fel_init(&ctx))
	{
		printf("ERROR: Can't found any FEL device\r\n");
		if(ctx.hdl)
			libusb_close(ctx.hdl);
		libusb_exit(NULL);
		return -1;
	}
	if(!ctx.chip)
	{
		printf("WARNNING: Not yet support this device\r\n");
		printf("%.8s soc=%08x %08x ver=%04x %02x %02x scratchpad=%08x %08x %08x\r\n",
			ctx.ver.signature, ctx.ver.id, ctx.ver.unknown_0a,
			ctx.ver.protocol, ctx.ver.unknown_12, ctx.ver.unknown_13,
			ctx.ver.scratchpad, ctx.ver.pad[0], ctx.ver.pad[1]);
		if(ctx.hdl)
			libusb_close(ctx.hdl);
		libusb_exit(NULL);
		return -1;
	}
	if(!strcmp(argv[1], "help"))
	{
		usage();
	}
	else if(!strcmp(argv[1], "version"))
	{
		printf("%.8s soc=%08x(%s) %08x ver=%04x %02x %02x scratchpad=%08x %08x %08x\r\n",
			ctx.ver.signature, ctx.ver.id, ctx.chip->name, ctx.ver.unknown_0a,
			ctx.ver.protocol, ctx.ver.unknown_12, ctx.ver.unknown_13,
			ctx.ver.scratchpad, ctx.ver.pad[0], ctx.ver.pad[1]);
	}
	else if(!strcmp(argv[1], "hexdump"))
	{
		argc -= 2;
		argv += 2;
		if(argc >= 2)
		{
			uint32_t offset = strtoul(argv[0], NULL, 0);
			size_t len = strtoul(argv[1], NULL, 0);
			unsigned char buf[len];
			fel_read(&ctx, offset, buf, len);
			hexdump(offset, buf, len);
		}
		else
			usage();
	}
	else
		usage();
	if(ctx.hdl)
		libusb_close(ctx.hdl);
	libusb_exit(NULL);

	return 0;
}
