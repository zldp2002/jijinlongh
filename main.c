#include <fel.h>
#include <spinor.h>
#include <spinand.h>

static uint64_t file_save(const char * filename, void * buf, uint64_t len)
{
	FILE * out = fopen(filename, "wb");
	int r;
	if(!out)
	{
		perror("Failed to open output file");
		exit(-1);
	}
	r = fwrite(buf, len, 1, out);
	fclose(out);
	return r;
}

static void * file_load(const char * filename, uint64_t * len)
{
	uint64_t offset = 0, bufsize = 8192;
	char * buf = malloc(bufsize);
	FILE * in;
	if(strcmp(filename, "-") == 0)
		in = stdin;
	else
		in = fopen(filename, "rb");
	if(!in)
	{
		perror("Failed to open input file");
		exit(-1);
	}
	while(1)
	{
		uint64_t len = bufsize - offset;
		uint64_t n = fread(buf + offset, 1, len, in);
		offset += n;
		if(n < len)
			break;
		bufsize *= 2;
		buf = realloc(buf, bufsize);
		if(!buf)
		{
			perror("Failed to resize load_file() buffer");
			exit(-1);
		}
	}
	if(len)
		*len = offset;
	if(in != stdin)
		fclose(in);
	return buf;
}

static void hexdump(uint32_t addr, void * buf, size_t len)
{
	unsigned char * p = buf;
	size_t i, j;

	for(j = 0; j < len; j += 16)
	{
		printf("%08x: ", (uint32_t)(addr + j));
		for(i = 0; i < 16; i++)
		{
			if(j + i < len)
				printf("%02x ", p[j + i]);
			else
				printf("   ");
		}
		putchar(' ');
		for(i = 0; i < 16; i++)
		{
			if(j + i >= len)
				putchar(' ');
			else
				putchar(isprint(p[j + i]) ? p[j + i] : '.');
		}
		printf("\r\n");
	}
}

static void usage(void)
{
	printf("xfel(v1.2.1) - https://github.com/xboot/xfel\r\n");
	printf("usage:\r\n");
	printf("    xfel version                                             - Show chip version\r\n");
	printf("    xfel hexdump <address> <length>                          - Dumps memory region in hex\r\n");
	printf("    xfel dump <address> <length>                             - Binary memory dump to stdout\r\n");
	printf("    xfel exec <address>                                      - Call function address\r\n");
	printf("    xfel read32 <address>                                    - Read 32-bits value from device memory\r\n");
	printf("    xfel write32 <address> <value>                           - Write 32-bits value to device memory\r\n");
	printf("    xfel read <address> <length> <file>                      - Read memory to file\r\n");
	printf("    xfel write <address> <file>                              - Write file to memory\r\n");
	printf("    xfel reset                                               - Reset device using watchdog\r\n");
	printf("    xfel sid                                                 - Show sid information\r\n");
	printf("    xfel jtag                                                - Enable jtag debug\r\n");
	printf("    xfel ddr [type]                                          - Initial ddr controller with optional type\r\n");
	printf("    xfel spinor                                              - Detect spi nor flash\r\n");
	printf("    xfel spinor read <address> <length> <file>               - Read spi nor flash to file\r\n");
	printf("    xfel spinor write <address> <file>                       - Write file to spi nor flash\r\n");
	printf("    xfel spinand                                             - Detect spi nand flash\r\n");
	printf("    xfel spinand read <address> <length> <file>              - Read spi nand flash to file\r\n");
	printf("    xfel spinand write <address> <file>                      - Write file to spi nand flash\r\n");
	printf("    xfel spinand splwrite <valid-page-size> <address> <file> - Write file to spi nand flash with spl mode\r\n");
}

int main(int argc, char * argv[])
{
	struct xfel_ctx_t ctx;

	if(argc < 2)
	{
		usage();
		return 0;
	}
	for(int i = 1; i < argc; i++)
	{
		if(!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help"))
		{
			usage();
			return 0;
		}
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
		printf("%.8s soc=0x%08x 0x%08x ver=0x%04x 0x%02x 0x%02x scratchpad=0x%08x\r\n",
			ctx.version.magic, ctx.version.id, ctx.version.firmware,
			ctx.version.protocol, ctx.version.dflag, ctx.version.dlength, ctx.version.scratchpad);
		if(ctx.hdl)
			libusb_close(ctx.hdl);
		libusb_exit(NULL);
		return -1;
	}
	if(!strcmp(argv[1], "version"))
	{
		printf("%.8s soc=0x%08x(%s) 0x%08x ver=0x%04x 0x%02x 0x%02x scratchpad=0x%08x\r\n",
			ctx.version.magic, ctx.version.id, ctx.chip->name, ctx.version.firmware,
			ctx.version.protocol, ctx.version.dflag, ctx.version.dlength, ctx.version.scratchpad);
	}
	else if(!strcmp(argv[1], "hexdump"))
	{
		argc -= 2;
		argv += 2;
		if(argc == 2)
		{
			uint32_t addr = strtoul(argv[0], NULL, 0);
			size_t len = strtoul(argv[1], NULL, 0);
			char * buf = malloc(len);
			if(buf)
			{
				fel_read(&ctx, addr, buf, len);
				hexdump(addr, buf, len);
				free(buf);
			}
		}
		else
			usage();
	}
	else if(!strcmp(argv[1], "dump"))
	{
		argc -= 2;
		argv += 2;
		if(argc == 2)
		{
			uint32_t addr = strtoul(argv[0], NULL, 0);
			size_t len = strtoul(argv[1], NULL, 0);
			char * buf = malloc(len);
			if(buf)
			{
				fel_read(&ctx, addr, buf, len);
				fwrite(buf, len, 1, stdout);
				free(buf);
			}
		}
		else
			usage();
	}
	else if(!strcmp(argv[1], "exec"))
	{
		argc -= 2;
		argv += 2;
		if(argc == 1)
		{
			uint32_t addr = strtoul(argv[0], NULL, 0);
			fel_exec(&ctx, addr);
		}
		else
			usage();
	}
	else if(!strcmp(argv[1], "read32"))
	{
		argc -= 2;
		argv += 2;
		if(argc == 1)
		{
			uint32_t addr = strtoul(argv[0], NULL, 0);
			printf("0x%08x\r\n", fel_read32(&ctx, addr));
		}
		else
			usage();
	}
	else if(!strcmp(argv[1], "write32"))
	{
		argc -= 2;
		argv += 2;
		if(argc == 2)
		{
			uint32_t addr = strtoul(argv[0], NULL, 0);
			size_t val = strtoul(argv[1], NULL, 0);
			fel_write32(&ctx, addr, val);
		}
		else
			usage();
	}
	else if(!strcmp(argv[1], "read"))
	{
		argc -= 2;
		argv += 2;
		if(argc == 3)
		{
			uint32_t addr = strtoul(argv[0], NULL, 0);
			size_t len = strtoul(argv[1], NULL, 0);
			char * buf = malloc(len);
			if(buf)
			{
				fel_read_progress(&ctx, addr, buf, len);
				file_save(argv[2], buf, len);
				free(buf);
			}
		}
		else
			usage();
	}
	else if(!strcmp(argv[1], "write"))
	{
		argc -= 2;
		argv += 2;
		if(argc == 2)
		{
			uint32_t addr = strtoul(argv[0], NULL, 0);
			uint64_t len;
			void * buf = file_load(argv[1], &len);
			if(buf)
			{
				fel_write_progress(&ctx, addr, buf, len);
				free(buf);
			}
		}
		else
			usage();
	}
	else if(!strcmp(argv[1], "reset"))
	{
		if(!fel_chip_reset(&ctx))
			printf("The '%s' chip don't support reset command\r\n", ctx.chip->name);
	}
	else if(!strcmp(argv[1], "sid"))
	{
		char sid[256];
		if(fel_chip_sid(&ctx, sid))
			printf("%s\r\n", sid);
		else
			printf("The '%s' chip don't support sid command\r\n", ctx.chip->name);
	}
	else if(!strcmp(argv[1], "jtag"))
	{
		if(!fel_chip_jtag(&ctx))
			printf("The '%s' chip don't support jtag command\r\n", ctx.chip->name);
	}
	else if(!strcmp(argv[1], "ddr"))
	{
		argc -= 2;
		argv += 2;
		if(fel_chip_ddr(&ctx, (argc == 1) ? argv[0] : NULL))
			printf("Initial ddr controller sucessed\r\n");
		else
			printf("Failed to initial ddr controller\r\n");
	}
	else if(!strcmp(argv[1], "spinor"))
	{
		argc -= 2;
		argv += 2;
		if(argc == 0)
		{
			char name[128];
			uint64_t capacity;

			if(spinor_detect(&ctx, name, &capacity))
				printf("Found spi nor flash '%s' with %lld bytes\r\n", name, (long long)capacity);
			else
				printf("Can't detect any spi nor flash\r\n");
		}
		else
		{
			if(!strcmp(argv[0], "read") && (argc == 4))
			{
				argc -= 1;
				argv += 1;
				uint64_t addr = strtoull(argv[0], NULL, 0);
				uint64_t len = strtoull(argv[1], NULL, 0);
				char * buf = malloc(len);
				if(buf)
				{
					if(spinor_read(&ctx, addr, buf, len))
						file_save(argv[2], buf, len);
					else
						printf("Can't read spi nor flash\r\n");
					free(buf);
				}
			}
			else if(!strcmp(argv[0], "write") && (argc == 3))
			{
				argc -= 1;
				argv += 1;
				uint64_t addr = strtoull(argv[0], NULL, 0);
				uint64_t len;
				void * buf = file_load(argv[1], &len);
				if(buf)
				{
					if(!spinor_write(&ctx, addr, buf, len))
						printf("Can't write spi nor flash\r\n");
					free(buf);
				}
			}
			else
				usage();
		}
	}
	else if(!strcmp(argv[1], "spinand"))
	{
		argc -= 2;
		argv += 2;
		if(argc == 0)
		{
			char name[128];
			uint64_t capacity;

			if(spinand_detect(&ctx, name, &capacity))
				printf("Found spi nand flash '%s' with %lld bytes\r\n", name, (long long)capacity);
			else
				printf("Can't detect any spi nand flash\r\n");
		}
		else
		{
			if(!strcmp(argv[0], "read") && (argc == 4))
			{
				argc -= 1;
				argv += 1;
				uint64_t addr = strtoull(argv[0], NULL, 0);
				uint64_t len = strtoull(argv[1], NULL, 0);
				char * buf = malloc(len);
				if(buf)
				{
					if(spinand_read(&ctx, addr, buf, len))
						file_save(argv[2], buf, len);
					else
						printf("Can't read spi nand flash\r\n");
					free(buf);
				}
			}
			else if(!strcmp(argv[0], "write") && (argc == 3))
			{
				argc -= 1;
				argv += 1;
				uint64_t addr = strtoull(argv[0], NULL, 0);
				uint64_t len;
				void * buf = file_load(argv[1], &len);
				if(buf)
				{
					if(!spinand_write(&ctx, addr, buf, len))
						printf("Can't write spi nand flash\r\n");
					free(buf);
				}
			}
			else if(!strcmp(argv[0], "splwrite") && (argc == 4))
			{
				argc -= 1;
				argv += 1;
				uint32_t pagesz = strtoul(argv[0], NULL, 0);
				uint64_t addr = strtoull(argv[1], NULL, 0);
				uint64_t len;
				void * buf = file_load(argv[2], &len);
				if(buf)
				{
					if(!spinand_splwrite(&ctx, pagesz, addr, buf, len))
						printf("Can't write spi nand flash with spl mode\r\n");
					free(buf);
				}
			}
			else
				usage();
		}
	}
	else
		usage();
	if(ctx.hdl)
		libusb_close(ctx.hdl);
	libusb_exit(NULL);

	return 0;
}
