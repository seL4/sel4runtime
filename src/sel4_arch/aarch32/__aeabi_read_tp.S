/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2005-2014 Rich Felker, et al.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * This has been implemented in assembly to ensure that the following
 * requirements from section 4.3.5 of the Run-time ABI for ARM
 * Architecture are met:
 *
 * Implementations of this function should corrupt only the result
 * register (r0) and the non-parameter integer core registers allowed to
 * be corrupted by the [AAPCS] (ip, lr, and CPSR). Registers r1 -r3 must
 * be preserved.
 */
.syntax unified
.global __aeabi_read_tp
.type __aeabi_read_tp,%function
__aeabi_read_tp:
	push {r1,r2,r3,lr}
	bl __aeabi_read_tp_c
	pop {r1,r2,r3,lr}
	bx lr
