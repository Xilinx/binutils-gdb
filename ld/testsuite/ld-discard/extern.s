.globl data
	.section	.data.exit,"aw"
data:
.globl text
	.section	.text.exit,"aw"
text:
.text
.globl _start
_start:
	.long	data
	.section	.debug_info
	.long	text
