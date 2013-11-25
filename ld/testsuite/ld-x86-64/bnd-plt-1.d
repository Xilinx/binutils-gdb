#source: bnd-branch-1.s
#as: --64
#ld: -shared -melf_x86_64
#objdump: -dw

.*: +file format .*


Disassembly of section .plt:

0+2b0 <.plt>:
[      ]*[a-f0-9]+:	ff 35 a2 01 20 00    	pushq  0x2001a2\(%rip\)        # 200458 <_GLOBAL_OFFSET_TABLE_\+0x8>
[      ]*[a-f0-9]+:	f2 ff 25 a3 01 20 00 	bnd jmpq \*0x2001a3\(%rip\)        # 200460 <_GLOBAL_OFFSET_TABLE_\+0x10>
[      ]*[a-f0-9]+:	0f 1f 00             	nopl   \(%rax\)
[      ]*[a-f0-9]+:	68 00 00 00 00       	pushq  \$0x0
[      ]*[a-f0-9]+:	e9 e6 ff ff ff       	jmpq   2b0 <foo2@plt-0x50>
[      ]*[a-f0-9]+:	66 0f 1f 44 00 00    	nopw   0x0\(%rax,%rax,1\)
[      ]*[a-f0-9]+:	68 01 00 00 00       	pushq  \$0x1
[      ]*[a-f0-9]+:	f2 e9 d5 ff ff ff    	bnd jmpq 2b0 <foo2@plt-0x50>
[      ]*[a-f0-9]+:	0f 1f 44 00 00       	nopl   0x0\(%rax,%rax,1\)
[      ]*[a-f0-9]+:	68 02 00 00 00       	pushq  \$0x2
[      ]*[a-f0-9]+:	f2 e9 c5 ff ff ff    	bnd jmpq 2b0 <foo2@plt-0x50>
[      ]*[a-f0-9]+:	0f 1f 44 00 00       	nopl   0x0\(%rax,%rax,1\)
[      ]*[a-f0-9]+:	68 03 00 00 00       	pushq  \$0x3
[      ]*[a-f0-9]+:	e9 b6 ff ff ff       	jmpq   2b0 <foo2@plt-0x50>
[      ]*[a-f0-9]+:	66 0f 1f 44 00 00    	nopw   0x0\(%rax,%rax,1\)

Disassembly of section .plt.bnd:

0+300 <foo2@plt>:
[      ]*[a-f0-9]+:	ff 25 62 01 20 00    	jmpq   \*0x200162\(%rip\)        # 200468 <_GLOBAL_OFFSET_TABLE_\+0x18>
[      ]*[a-f0-9]+:	0f 1f 44 00 00       	nopl   0x0\(%rax,%rax,1\)
[      ]*[a-f0-9]+:	0f 1f 44 00 00       	nopl   0x0\(%rax,%rax,1\)

0+310 <foo3@plt>:
[      ]*[a-f0-9]+:	f2 ff 25 59 01 20 00 	bnd jmpq \*0x200159\(%rip\)        # 200470 <_GLOBAL_OFFSET_TABLE_\+0x20>
[      ]*[a-f0-9]+:	0f 1f 44 00 00       	nopl   0x0\(%rax,%rax,1\)
[      ]*[a-f0-9]+:	0f 1f 40 00          	nopl   0x0\(%rax\)

0+320 <foo1@plt>:
[      ]*[a-f0-9]+:	f2 ff 25 51 01 20 00 	bnd jmpq \*0x200151\(%rip\)        # 200478 <_GLOBAL_OFFSET_TABLE_\+0x28>
[      ]*[a-f0-9]+:	0f 1f 44 00 00       	nopl   0x0\(%rax,%rax,1\)
[      ]*[a-f0-9]+:	0f 1f 40 00          	nopl   0x0\(%rax\)

0+330 <foo4@plt>:
[      ]*[a-f0-9]+:	ff 25 4a 01 20 00    	jmpq   \*0x20014a\(%rip\)        # 200480 <_GLOBAL_OFFSET_TABLE_\+0x30>
[      ]*[a-f0-9]+:	0f 1f 44 00 00       	nopl   0x0\(%rax,%rax,1\)
[      ]*[a-f0-9]+:	0f 1f 44 00 00       	nopl   0x0\(%rax,%rax,1\)

Disassembly of section .text:

0+340 <_start>:
[      ]*[a-f0-9]+:	f2 e9 da ff ff ff    	bnd jmpq 320 <foo1@plt>
[      ]*[a-f0-9]+:	e8 b5 ff ff ff       	callq  300 <foo2@plt>
[      ]*[a-f0-9]+:	e9 c0 ff ff ff       	jmpq   310 <foo3@plt>
[      ]*[a-f0-9]+:	e8 db ff ff ff       	callq  330 <foo4@plt>
[      ]*[a-f0-9]+:	f2 e8 b5 ff ff ff    	bnd callq 310 <foo3@plt>
[      ]*[a-f0-9]+:	e9 d0 ff ff ff       	jmpq   330 <foo4@plt>
#pass
