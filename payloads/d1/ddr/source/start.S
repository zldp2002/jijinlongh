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

#include <linkage.h>
#include <riscv64.h>

	.global _start
_start:
	li t1, 0x1 << 22
	csrs mxstatus, t1
	li t1, 0x30013
	csrs mcor, t1
	j reset

ddr_param:
	.word 792
	.word 3
	.word 0x7b7bfb
	.word 0x01
	.word 0x000010d2
	.word 0x0000
	.word 0x1c70
	.word 0x042
	.word 0x18
	.word 0x0
	.word 0x004A2195
	.word 0x02423190
	.word 0x0008B061
	.word 0xB4787896
	.word 0x0
	.word 0x48484848
	.word 0x00000048
	.word 0x1620121e
	.word 0x0
	.word 0x0
	.word 0x0
	.word 0x00870000
	.word 0x00000024
	.word 0x34050100
	.word 0, 0, 0, 0, 0, 0, 0, 0

reset:
	addi sp, sp, -32
	sd s0, 8(sp)
	sd s1, 16(sp)
	sd ra, 24(sp)
	mv s0, a0

	jal sys_uart_init
	jal sys_clock_init
	la a0, ddr_param
	jal sys_dram_init

	ld ra, 24(sp)
	ld s0, 8(sp)
	ld s1, 16(sp)
	addi sp, sp, 32
	ret
