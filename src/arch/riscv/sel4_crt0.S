#include "autoconf.h"

.section .text
.global _sel4_start
_sel4_start:

/* Set gp for relaxation. See
 * https://www.sifive.com/blog/2017/08/28/all-aboard-part-3-linker-relaxation-in-riscv-toolchain/
 */
.option push
.option norelax
1:auipc gp, %pcrel_hi(__global_pointer$)
  addi  gp, gp, %pcrel_lo(1b)
.option pop

	la sp, __thread_base
	jal __sel4_start_root
	/* should not return */
1:
	j 1b

.section .bss
__thread_base:
	.align 16
	.space CONFIG_SEL4RUNTIME_ROOT_STACK
__thread_top:
