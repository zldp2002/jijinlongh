/*
 * start.S
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

	.global _start
_start:
	b reset

spi_type:
	.word 0
spi_transfer_txbuf:
	.word 0
spi_transfer_rxbuf:
	.word 0
spi_transfer_length:
	.word 0

reset:
	ldr r0, =0x00037ff8
	str sp, [r0, #0]
	str lr, [r0, #4]
	mrs lr, cpsr
	str lr, [r0, #8]
	mrc p15, 0, lr, c1, c0, 0
	str lr, [r0, #12]
	mrc p15, 0, lr, c12, c0, 0
	str lr, [r0, #16]
	mrc p15, 0, lr, c1, c0, 0
	str lr, [r0, #20]

	ldr r1, spi_type
	mov r0, #4
	cmp r1, r0
	beq _spi_transfer
	mov r0, #3
	cmp r1, r0
	beq _spi_deselect
	mov r0, #2
	cmp r1, r0
	beq _spi_select
	mov r0, #1
	cmp r1, r0
	beq _spi_init
	b _exit
_spi_init:
	bl sys_spi_init
	b _exit
_spi_select:
	bl sys_spi_select
	b _exit
_spi_deselect:
	bl sys_spi_deselect
	b _exit
_spi_transfer:
	ldr r0, spi_transfer_txbuf
	ldr r1, spi_transfer_rxbuf
	ldr r2, spi_transfer_length
	bl sys_spi_transfer
	b _exit

_exit:
	ldr r0, =0x00037ff8
	ldr sp, [r0, #0]
	ldr lr, [r0, #4]
	ldr r1, [r0, #20]
	mcr p15, 0, r1, c1, c0, 0
	ldr r1, [r0, #16]
	mcr p15, 0, r1, c12, c0, 0
	ldr r1, [r0, #12]
	mcr p15, 0, r1, c1, c0, 0
	ldr r1, [r0, #8]
	msr cpsr, r1
	bx lr

