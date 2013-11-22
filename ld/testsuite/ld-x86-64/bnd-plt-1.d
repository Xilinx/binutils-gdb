#source: bnd-branch-1.s
#as: --64
#ld: -shared -melf_x86_64
#objdump: -dw

.*: +file format .*


Disassembly of section .plt:

0+2b0 <foo2@plt-0x10>:
[ 	]*[a-f0-9]+:	ff 35 82 01 20 00    	pushq  0x200182\(%rip\)        # 200438 <_GLOBAL_OFFSET_TABLE_\+0x8>
[ 	]*[a-f0-9]+:	f2 ff 25 83 01 20 00 	bnd jmpq \*0x200183\(%rip\)        # 200440 <_GLOBAL_OFFSET_TABLE_\+0x10>
[ 	]*[a-f0-9]+:	0f 1f 00             	nopl   \(%rax\)

0+2c0 <foo2@plt>:
[ 	]*[a-f0-9]+:	ff 25 82 01 20 00    	jmpq   \*0x200182\(%rip\)        # 200448 <_GLOBAL_OFFSET_TABLE_\+0x18>
[ 	]*[a-f0-9]+:	68 00 00 00 00       	pushq  \$0x0
[ 	]*[a-f0-9]+:	e9 e0 ff ff ff       	jmpq   2b0 <foo2@plt-0x10>

0+2d0 <foo3@plt>:
[ 	]*[a-f0-9]+:	f2 ff 25 79 01 20 00 	bnd jmpq \*0x200179\(%rip\)        # 200450 <_GLOBAL_OFFSET_TABLE_\+0x20>
[ 	]*[a-f0-9]+:	68 01 00 00 00       	pushq  \$0x1
[ 	]*[a-f0-9]+:	f2 e9 ce ff ff ff    	bnd jmpq 2b0 <foo2@plt-0x10>
[ 	]*[a-f0-9]+:	0f 1f 80 00 00 00 00 	nopl   0x0\(%rax\)
[ 	]*[a-f0-9]+:	0f 1f 80 00 00 00 00 	nopl   0x0\(%rax\)

0+2f0 <foo1@plt>:
[ 	]*[a-f0-9]+:	f2 ff 25 69 01 20 00 	bnd jmpq \*0x200169\(%rip\)        # 200460 <_GLOBAL_OFFSET_TABLE_\+0x30>
[ 	]*[a-f0-9]+:	68 02 00 00 00       	pushq  \$0x2
[ 	]*[a-f0-9]+:	f2 e9 ae ff ff ff    	bnd jmpq 2b0 <foo2@plt-0x10>
[ 	]*[a-f0-9]+:	0f 1f 80 00 00 00 00 	nopl   0x0\(%rax\)
[ 	]*[a-f0-9]+:	0f 1f 80 00 00 00 00 	nopl   0x0\(%rax\)

0+310 <foo4@plt>:
[ 	]*[a-f0-9]+:	ff 25 5a 01 20 00    	jmpq   \*0x20015a\(%rip\)        # 200470 <_GLOBAL_OFFSET_TABLE_\+0x40>
[ 	]*[a-f0-9]+:	68 03 00 00 00       	pushq  \$0x3
[ 	]*[a-f0-9]+:	e9 90 ff ff ff       	jmpq   2b0 <foo2@plt-0x10>

Disassembly of section .text:

0+320 <_start>:
[ 	]*[a-f0-9]+:	f2 e9 ca ff ff ff    	bnd jmpq 2f0 <foo1@plt>
[ 	]*[a-f0-9]+:	e8 95 ff ff ff       	callq  2c0 <foo2@plt>
[ 	]*[a-f0-9]+:	e9 a0 ff ff ff       	jmpq   2d0 <foo3@plt>
[ 	]*[a-f0-9]+:	e8 db ff ff ff       	callq  310 <foo4@plt>
[ 	]*[a-f0-9]+:	f2 e8 95 ff ff ff    	bnd callq 2d0 <foo3@plt>
[ 	]*[a-f0-9]+:	e9 d0 ff ff ff       	jmpq   310 <foo4@plt>
#pass
