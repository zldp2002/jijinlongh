/*
 * sys-spi.c
 *
 * Copyright(c) 2007-2021 Jianjun Jiang <8192542@qq.com>
 * Official site: http://xboot.org
 * Mobile phone: +86-18665388956
 * QQ: 8192542
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <xboot.h>

enum {
	SPI_GCR	= 0x04,
	SPI_TCR	= 0x08,
	SPI_IER	= 0x10,
	SPI_ISR	= 0x14,
	SPI_FCR	= 0x18,
	SPI_FSR	= 0x1c,
	SPI_WCR	= 0x20,
	SPI_CCR	= 0x24,
	SPI_MBC	= 0x30,
	SPI_MTC	= 0x34,
	SPI_BCC	= 0x38,
	SPI_TXD	= 0x200,
	SPI_RXD	= 0x300,
};

static void sys_spi_init(void)
{
	virtual_addr_t addr;
	u32_t val;

	/* Config GPIOC0, GPIOC1, GPIOC2 and GPIOC3 */
	addr = 0x01c20848 + 0x00;
	val = read32(addr);
	val &= ~(0xf << ((0 & 0x7) << 2));
	val |= ((0x2 & 0x7) << ((0 & 0x7) << 2));
	write32(addr, val);

	val = read32(addr);
	val &= ~(0xf << ((1 & 0x7) << 2));
	val |= ((0x2 & 0x7) << ((1 & 0x7) << 2));
	write32(addr, val);

	val = read32(addr);
	val &= ~(0xf << ((2 & 0x7) << 2));
	val |= ((0x2 & 0x7) << ((2 & 0x7) << 2));
	write32(addr, val);

	val = read32(addr);
	val &= ~(0xf << ((3 & 0x7) << 2));
	val |= ((0x2 & 0x7) << ((3 & 0x7) << 2));
	write32(addr, val);

	/* Deassert spi0 reset */
	addr = 0x01c202c0;
	val = read32(addr);
	val |= (1 << 20);
	write32(addr, val);

	/* Open the spi0 bus gate */
	addr = 0x01c20000 + 0x60;
	val = read32(addr);
	val |= (1 << 20);
	write32(addr, val);

	/* Set spi clock rate control register, divided by 4 */
	addr = 0x01c05000;
	write32(addr + SPI_CCR, 0x00001001);

	/* Enable spi0 and do a soft reset */
	addr = 0x01c05000;
	val = read32(addr + SPI_GCR);
	val |= (1 << 31) | (1 << 7) | (1 << 1) | (1 << 0);
	write32(addr + SPI_GCR, val);
	while(read32(addr + SPI_GCR) & (1 << 31));

	val = read32(addr + SPI_TCR);
	val &= ~(0x3 << 0);
	val |= (1 << 6) | (1 << 2);
	write32(addr + SPI_TCR, val);

	val = read32(addr + SPI_FCR);
	val |= (1 << 31) | (1 << 15);
	write32(addr + SPI_FCR, val);
}

static void sys_spi_select(void)
{
	virtual_addr_t addr = 0x01c05000;
	u32_t val;

	val = read32(addr + SPI_TCR);
	val &= ~((0x3 << 4) | (0x1 << 7));
	val |= ((0 & 0x3) << 4) | (0x0 << 7);
	write32(addr + SPI_TCR, val);
}

static void sys_spi_deselect(void)
{
	virtual_addr_t addr = 0x01c05000;
	u32_t val;

	val = read32(addr + SPI_TCR);
	val &= ~((0x3 << 4) | (0x1 << 7));
	val |= ((0 & 0x3) << 4) | (0x1 << 7);
	write32(addr + SPI_TCR, val);
}

static inline void sys_spi_write_txbuf(u8_t * buf, int len)
{
	virtual_addr_t addr = 0x01c05000;
	int i;

	write32(addr + SPI_MTC, len & 0xffffff);
	write32(addr + SPI_BCC, len & 0xffffff);
	if(buf)
	{
		for(i = 0; i < len; i++)
			write8(addr + SPI_TXD, *buf++);
	}
	else
	{
		for(i = 0; i < len; i++)
			write8(addr + SPI_TXD, 0xff);
	}
}

static void sys_spi_transfer(void * txbuf, void * rxbuf, u32_t len)
{
	virtual_addr_t addr = 0x01c05000;
	u8_t * tx = txbuf;
	u8_t * rx = rxbuf;
	u8_t val;
	int n, i;

	while(len > 0)
	{
		n = (len <= 64) ? len : 64;
		write32(addr + SPI_MBC, n);
		sys_spi_write_txbuf(tx, n);
		write32(addr + SPI_TCR, read32(addr + SPI_TCR) | (1 << 31));
		while((read32(addr + SPI_FSR) & 0xff) < n);
		for(i = 0; i < n; i++)
		{
			val = read8(addr + SPI_RXD);
			if(rx)
				*rx++ = val;
		}
		if(tx)
			tx += n;
		len -= n;
	}
}

enum {
	SPI_CMD_END				= 0x00,
	SPI_CMD_INIT			= 0x01,
	SPI_CMD_SELECT			= 0x02,
	SPI_CMD_DESELECT		= 0x03,
	SPI_CMD_TXBUF			= 0x04,
	SPI_CMD_RXBUF			= 0x05,
	SPI_CMD_SPINOR_WAIT		= 0x06,
	SPI_CMD_SPINAND_WAIT	= 0x07,
};

void sys_spi_run(void * cmdbuf)
{
	uint8_t tx[8], rx[8];
	u8_t c, * p = cmdbuf;
	u32_t addr, len;

	while(1)
	{
		c = *p++;
		if(c == SPI_CMD_INIT)
		{
			sys_spi_init();
		}
		else if(c == SPI_CMD_SELECT)
		{
			sys_spi_select();
		}
		else if(c == SPI_CMD_DESELECT)
		{
			sys_spi_deselect();
		}
		else if(c == SPI_CMD_TXBUF)
		{
			addr = (p[0] << 0) | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
			len  = (p[4] << 0) | (p[5] << 8) | (p[6] << 16) | (p[7] << 24);
			sys_spi_transfer((void *)addr, NULL, len);
			p += 8;
		}
		else if(c == SPI_CMD_RXBUF)
		{
			addr = (p[0] << 0) | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
			len  = (p[4] << 0) | (p[5] << 8) | (p[6] << 16) | (p[7] << 24);
			sys_spi_transfer(NULL, (void *)addr, len);
			p += 8;
		}
		else if(c == SPI_CMD_SPINOR_WAIT)
		{
			tx[0] = 0x05;
			do {
				sys_spi_transfer((void *)&tx[0], NULL, 1);
				sys_spi_transfer(NULL, (void *)&rx[0], 1);
			} while((rx[0] & 0x1) == 0x1);
		}
		else if(c == SPI_CMD_SPINAND_WAIT)
		{
			tx[0] = 0x0f;
			tx[1] = 0xc0;
			do {
				sys_spi_transfer((void *)&tx[0], NULL, 2);
				sys_spi_transfer(NULL, (void *)&rx[0], 1);
			} while((rx[0] & 0x1) == 0x1);
		}
		else
		{
			return;
		}
	}
}
