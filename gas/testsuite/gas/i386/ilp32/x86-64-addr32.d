#source: ../x86-64-addr32.s
#as: -J
#objdump: -drw
#name: x86-64 (ILP32) 32-bit addressing

.*: +file format .*

Disassembly of section .text:

0+ <.text>:
[ 	]*[a-f0-9]+:	67 48 8d 80 00 00 00 00[ 	]+lea[ 	]+0x0\(%eax\),%rax.*
[ 	]*[a-f0-9]+:	67 49 8d 80 00 00 00 00[ 	]+lea[ 	]+0x0\(%r8d\),%rax.*
[ 	]*[a-f0-9]+:	67 48 8d 05 00 00 00 00[ 	]+lea[ 	]+0x0\(%eip\),%rax.*
[ 	]*[a-f0-9]+:	67 48 8d 04 25 00 00 00 00[ 	]+addr32 lea[ 	]+0x0,%rax.*
[ 	]*[a-f0-9]+:	67 a0 98 08 60 00    	addr32 mov 0x600898,%al
[ 	]*[a-f0-9]+:	67 66 a1 98 08 60 00 	addr32 mov 0x600898,%ax
[ 	]*[a-f0-9]+:	67 a1 98 08 60 00    	addr32 mov 0x600898,%eax
[ 	]*[a-f0-9]+:	67 48 a1 98 08 60 00 	addr32 mov 0x600898,%rax
[ 	]*[a-f0-9]+:	67 48 a1 98 08 80 00 	addr32 mov 0x800898,%rax
[ 	]*[a-f0-9]+:	67 48 8b 1c 25 98 08 80 00 	addr32 mov 0x800898,%rbx
[ 	]*[a-f0-9]+:	67 a2 98 08 60 00    	addr32 mov %al,0x600898
[ 	]*[a-f0-9]+:	67 66 a3 98 08 60 00 	addr32 mov %ax,0x600898
[ 	]*[a-f0-9]+:	67 a3 98 08 60 00    	addr32 mov %eax,0x600898
[ 	]*[a-f0-9]+:	67 48 a3 98 08 60 00 	addr32 mov %rax,0x600898
[ 	]*[a-f0-9]+:	67 48 a3 98 08 80 00 	addr32 mov %rax,0x800898
[ 	]*[a-f0-9]+:	67 48 89 1c 25 98 08 80 00 	addr32 mov %rbx,0x800898
#pass
