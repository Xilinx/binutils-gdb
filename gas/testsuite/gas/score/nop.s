/*
 * test relax
 * nop <-> nop!
	
 * Author: ligang
 */

.macro tran insn32, insn16
/* This block transform 32b instruction to 16b. */
.align 4
	
  \insn32               #32b -> 16b
  \insn16

  \insn32               #32b -> 16b
  \insn32               #32b -> 16b

  \insn16      
  \insn32               #32b -> 16b

  \insn32               #No transform
  add r18, r20, r24

/* This block transform 16b instruction to 32b. */
.align 4
	
  \insn16               #No transform
  \insn32

  \insn16               #No transform
  \insn16

  \insn16               #16b -> 32b
  xor r18, r20, r24
	
.endm

  tran "nop", "nop!"
