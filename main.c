#include <fel.h>
#include <sha256.h>
#include <ecdsa256.h>
#include <spinor.h>
#include <spinand.h>
#include <libusb.h>

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

static inline unsigned char hex_to_bin(char c)
{
	if((c >= 'a') && (c <= 'f'))
		return c - 'a' + 10;
	if((c >= '0') && (c <= '9'))
		return c - '0';
	if((c >= 'A') && (c <= 'F'))
		return c - 'A' + 10;
	return 0;
}

static unsigned char hex_string(const char * s, int o)
{
	return (hex_to_bin(s[o]) << 4) | hex_to_bin(s[o + 1]);
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
	printf("xfel(v1.3.0) - https://github.com/xboot/xfel\r\n");
	printf("usage:\r\n");
	printf("    xfel version                                        - Show chip version\r\n");
	printf("    xfel hexdump <address> <length>                     - Dumps memory region in hex\r\n");
	printf("    xfel dump <address> <length>                        - Binary memory dump to stdout\r\n");
	printf("    xfel read32 <address>                               - Read 32-bits value from device memory\r\n");
	printf("    xfel write32 <address> <value>                      - Write 32-bits value to device memory\r\n");
	printf("    xfel read <address> <length> <file>                 - Read memory to file\r\n");
	printf("    xfel write <address> <file>                         - Write file to memory\r\n");
	printf("    xfel exec <address>                                 - Call function address\r\n");
	printf("    xfel reset                                          - Reset device using watchdog\r\n");
	printf("    xfel sid                                            - Show sid information\r\n");
	printf("    xfel jtag                                           - Enable jtag debug\r\n");
	printf("    xfel ddr [type]                                     - Initial ddr controller with optional type\r\n");
	printf("    xfel sign <public-key> <private-key> <file>         - Generate ecdsa256 signature file for sha256 of sid\r\n");
	printf("    xfel spinor                                         - Detect spi nor flash\r\n");
	printf("    xfel spinor erase <address> <length>                - Erase spi nor flash\r\n");
	printf("    xfel spinor read <address> <length> <file>          - Read spi nor flash to file\r\n");
	printf("    xfel spinor write <address> <file>                  - Write file to spi nor flash\r\n");
	printf("    xfel spinand                                        - Detect spi nand flash\r\n");
	printf("    xfel spinand erase <address> <length>               - Erase spi nand flash\r\n");
	printf("    xfel spinand read <address> <length> <file>         - Read spi nand flash to file\r\n");
	printf("    xfel spinand write <address> <file>                 - Write file to spi nand flash\r\n");
	printf("    xfel spinand splwrite <split-size> <address> <file> - Write file to spi nand flash with split support\r\n");
	printf("    xfel extra [...]                                    - The extra commands\r\n");
}

int main(int argc, char * argv[])
{
	struct xfel_ctx_t ctx = { 0 };
    libusb_context *context = NULL;


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

	libusb_device **list = NULL;
	libusb_init(&context);
	int count = libusb_get_device_list(context, &list);
	assert(count > 0);

	for (size_t i = 0; i < count; ++i) {
		libusb_device *device = list[i];
		struct libusb_device_descriptor desc;
		int rc = libusb_get_device_descriptor(device, &desc);
		if(rc != 0)
			{
			printf("ERROR: Can't get device list: %d\r\n", rc);
		}
		if(desc.idVendor == 0x1f3a && desc.idProduct == 0xefe8) {
			int rc = libusb_open(device, &ctx.hdl);
			if(rc != 0)
			{
				printf("ERROR: Can't connect to device: %d\r\n", rc);
			}
			break;
		}
	}

	if(!fel_init(&ctx))
	{
		printf("ERROR: Can't found any FEL device\r\n");
		if(ctx.hdl)
			libusb_close(ctx.hdl);
		libusb_exit(NULL);
		return -1;
	}
	if(!strcmp(argv[1], "version"))
	{
		printf("%.8s ID=0x%08x(%s) dflag=0x%02x dlength=0x%02x scratchpad=0x%08x\r\n",
			ctx.version.magic, ctx.version.id, ctx.chip->name, ctx.version.dflag,
			ctx.version.dlength, ctx.version.scratchpad);
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
		if(fel_chip_ddr(&ctx, (argc == 1) ? argv[0] : ""))
			printf("Initial ddr controller succeeded\r\n");
		else
			printf("Failed to initial ddr controller\r\n");
	}
	else if(!strcmp(argv[1], "sign"))
	{
		argc -= 2;
		argv += 2;
		if(argc == 3)
		{
			uint8_t public_key[33] = {
				0x03, 0xcf, 0xd1, 0x8e, 0x4a, 0x4b, 0x40, 0xd6,
				0x52, 0x94, 0x48, 0xaa, 0x2d, 0xf8, 0xbb, 0xb6,
				0x77, 0x12, 0x82, 0x58, 0xb8, 0xfb, 0xfc, 0x5b,
				0x9e, 0x49, 0x2f, 0xbb, 0xba, 0x4e, 0x84, 0x83,
				0x2f,
			};
			uint8_t private_key[32] = {
				0xdc, 0x57, 0xb8, 0xa9, 0xe0, 0xe2, 0xb7, 0xf8,
				0xb4, 0xc9, 0x29, 0xbd, 0x8d, 0xb2, 0x84, 0x4e,
				0x53, 0xf0, 0x1f, 0x17, 0x1b, 0xbc, 0xdf, 0x6e,
				0x62, 0x89, 0x08, 0xdb, 0xf2, 0xb2, 0xe6, 0xa9,
			};
			char * p = argv[0];
			if(p && (strcmp(p, "") != 0) && (strlen(p) == sizeof(public_key) * 2))
			{
				for(int i = 0; i < sizeof(public_key); i++)
					public_key[i] = hex_string(p, i * 2);
			}
			char * q = argv[1];
			if(q && (strcmp(q, "") != 0) && (strlen(q) == sizeof(private_key) * 2))
			{
				for(int i = 0; i < sizeof(private_key); i++)
					private_key[i] = hex_string(q, i * 2);
			}
			char sid[256];
			uint8_t sha256[32];
			uint8_t signature[64];
			if(fel_chip_sid(&ctx, sid))
			{
				sha256_hash(sid, strlen(sid), sha256);
				ecdsa256_sign(private_key, sha256, signature);
				printf("Unique ID:\r\n\t");
				printf("%s\r\n", sid);
				printf("Sha256 digest:\r\n\t");
				for(int i = 0; i < sizeof(sha256); i++)
					printf("%02x", sha256[i]);
				printf("\r\n");
				printf("Ecdsa256 public key:\r\n\t");
				for(int i = 0; i < sizeof(public_key); i++)
					printf("%02x", public_key[i]);
				printf("\r\n");
				printf("Ecdsa256 private key:\r\n\t");
				for(int i = 0; i < sizeof(private_key); i++)
					printf("%02x", private_key[i]);
				printf("\r\n");
				printf("Ecdsa256 signature:\r\n\t");
				for(int i = 0; i < sizeof(signature); i++)
					printf("%02x", signature[i]);
				printf("\r\n");
				if(ecdsa256_verify(public_key, sha256, signature))
				{
					file_save(argv[2], signature, sizeof(signature));
					printf("Ecdsa256 signature verify successed and saved to '%s'.\r\n", argv[2]);
				}
				else
					printf("Ecdsa256 signature verify failed, please check the ecdsa256 public and private key.\r\n");
			}
			else
				printf("The '%s' chip don't support sid command\r\n", ctx.chip->name);
		}
		else
			usage();
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
			if(!strcmp(argv[0], "erase") && (argc == 3))
			{
				argc -= 1;
				argv += 1;
				uint64_t addr = strtoull(argv[0], NULL, 0);
				uint64_t len = strtoull(argv[1], NULL, 0);
				if(!spinor_erase(&ctx, addr, len))
					printf("Can't erase spi nor flash\r\n");
			}
			else if(!strcmp(argv[0], "read") && (argc == 4))
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
			if(!strcmp(argv[0], "erase") && (argc == 3))
			{
				argc -= 1;
				argv += 1;
				uint64_t addr = strtoull(argv[0], NULL, 0);
				uint64_t len = strtoull(argv[1], NULL, 0);
				if(!spinand_erase(&ctx, addr, len))
					printf("Can't erase spi nand flash\r\n");
			}
			else if(!strcmp(argv[0], "read") && (argc == 4))
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
				uint32_t splitsz = strtoul(argv[0], NULL, 0);
				uint64_t addr = strtoull(argv[1], NULL, 0);
				uint64_t len;
				void * buf = file_load(argv[2], &len);
				if(buf)
				{
					if(!spinand_splwrite(&ctx, splitsz, addr, buf, len))
						printf("Can't write spi nand flash with split support\r\n");
					free(buf);
				}
			}
			else
				usage();
		}
	}
	else if(!strcmp(argv[1], "extra"))
	{
		argc -= 2;
		argv += 2;
		if(!fel_chip_extra(&ctx, argc, argv))
			printf("Not support any extra commands\r\n");
	}
	else
		usage();
	if(ctx.hdl)
		libusb_close(ctx.hdl);
	libusb_exit(NULL);

	return 0;
}
