	.text
	.globl _start
_start:
	.byte	0x66
	leaq	foo@TLSGD(%rip), %rdi
	.word	0x6666
	rex64
	call	__tls_get_addr
