#objdump: -dw
#name: x86-64 LWP

.*: +file format .*

Disassembly of section .text:

0+ <_start>:
[ 	]*[a-f0-9]+:	8f e9 78 12 c0[ 	]+llwpcb %ax
[ 	]*[a-f0-9]+:	8f e9 78 12 c1[ 	]+llwpcb %cx
[ 	]*[a-f0-9]+:	8f e9 78 12 c2[ 	]+llwpcb %dx
[ 	]*[a-f0-9]+:	8f e9 78 12 c3[ 	]+llwpcb %bx
[ 	]*[a-f0-9]+:	8f e9 78 12 c4[ 	]+llwpcb %sp
[ 	]*[a-f0-9]+:	8f e9 78 12 c5[ 	]+llwpcb %bp
[ 	]*[a-f0-9]+:	8f e9 78 12 c6[ 	]+llwpcb %si
[ 	]*[a-f0-9]+:	8f e9 78 12 c7[ 	]+llwpcb %di
[ 	]*[a-f0-9]+:	8f e9 7c 12 c0[ 	]+llwpcb %eax
[ 	]*[a-f0-9]+:	8f e9 7c 12 c1[ 	]+llwpcb %ecx
[ 	]*[a-f0-9]+:	8f e9 7c 12 c2[ 	]+llwpcb %edx
[ 	]*[a-f0-9]+:	8f e9 7c 12 c3[ 	]+llwpcb %ebx
[ 	]*[a-f0-9]+:	8f e9 7c 12 c4[ 	]+llwpcb %esp
[ 	]*[a-f0-9]+:	8f e9 7c 12 c5[ 	]+llwpcb %ebp
[ 	]*[a-f0-9]+:	8f e9 7c 12 c6[ 	]+llwpcb %esi
[ 	]*[a-f0-9]+:	8f e9 7c 12 c7[ 	]+llwpcb %edi
[ 	]*[a-f0-9]+:	8f e9 f8 12 c0[ 	]+llwpcb %rax
[ 	]*[a-f0-9]+:	8f e9 f8 12 c1[ 	]+llwpcb %rcx
[ 	]*[a-f0-9]+:	8f e9 f8 12 c2[ 	]+llwpcb %rdx
[ 	]*[a-f0-9]+:	8f e9 f8 12 c3[ 	]+llwpcb %rbx
[ 	]*[a-f0-9]+:	8f e9 f8 12 c4[ 	]+llwpcb %rsp
[ 	]*[a-f0-9]+:	8f e9 f8 12 c5[ 	]+llwpcb %rbp
[ 	]*[a-f0-9]+:	8f e9 f8 12 c6[ 	]+llwpcb %rsi
[ 	]*[a-f0-9]+:	8f e9 f8 12 c7[ 	]+llwpcb %rdi
[ 	]*[a-f0-9]+:	8f e9 f8 12 cf[ 	]+slwpcb %rdi
[ 	]*[a-f0-9]+:	8f e9 f8 12 ce[ 	]+slwpcb %rsi
[ 	]*[a-f0-9]+:	8f e9 f8 12 cd[ 	]+slwpcb %rbp
[ 	]*[a-f0-9]+:	8f e9 f8 12 cc[ 	]+slwpcb %rsp
[ 	]*[a-f0-9]+:	8f e9 f8 12 cb[ 	]+slwpcb %rbx
[ 	]*[a-f0-9]+:	8f e9 f8 12 ca[ 	]+slwpcb %rdx
[ 	]*[a-f0-9]+:	8f e9 f8 12 c9[ 	]+slwpcb %rcx
[ 	]*[a-f0-9]+:	8f e9 f8 12 c8[ 	]+slwpcb %rax
[ 	]*[a-f0-9]+:	8f e9 7c 12 cf[ 	]+slwpcb %edi
[ 	]*[a-f0-9]+:	8f e9 7c 12 ce[ 	]+slwpcb %esi
[ 	]*[a-f0-9]+:	8f e9 7c 12 cd[ 	]+slwpcb %ebp
[ 	]*[a-f0-9]+:	8f e9 7c 12 cc[ 	]+slwpcb %esp
[ 	]*[a-f0-9]+:	8f e9 7c 12 cb[ 	]+slwpcb %ebx
[ 	]*[a-f0-9]+:	8f e9 7c 12 ca[ 	]+slwpcb %edx
[ 	]*[a-f0-9]+:	8f e9 7c 12 c9[ 	]+slwpcb %ecx
[ 	]*[a-f0-9]+:	8f e9 7c 12 c8[ 	]+slwpcb %eax
[ 	]*[a-f0-9]+:	8f e9 78 12 cf[ 	]+slwpcb %di
[ 	]*[a-f0-9]+:	8f e9 78 12 ce[ 	]+slwpcb %si
[ 	]*[a-f0-9]+:	8f e9 78 12 cd[ 	]+slwpcb %bp
[ 	]*[a-f0-9]+:	8f e9 78 12 cc[ 	]+slwpcb %sp
[ 	]*[a-f0-9]+:	8f e9 78 12 cb[ 	]+slwpcb %bx
[ 	]*[a-f0-9]+:	8f e9 78 12 ca[ 	]+slwpcb %dx
[ 	]*[a-f0-9]+:	8f e9 78 12 c9[ 	]+slwpcb %cx
[ 	]*[a-f0-9]+:	8f e9 78 12 c8[ 	]+slwpcb %ax
[ 	]*[a-f0-9]+:	8f ea 78 12 c0 34 12[ 	]+lwpins \$0x1234,%eax,%ax
[ 	]*[a-f0-9]+:	8f ea 70 12 c1 34 12[ 	]+lwpins \$0x1234,%ecx,%cx
[ 	]*[a-f0-9]+:	8f ea 68 12 c2 34 12[ 	]+lwpins \$0x1234,%edx,%dx
[ 	]*[a-f0-9]+:	8f ea 60 12 c3 34 12[ 	]+lwpins \$0x1234,%ebx,%bx
[ 	]*[a-f0-9]+:	8f ea 58 12 c4 34 12[ 	]+lwpins \$0x1234,%esp,%sp
[ 	]*[a-f0-9]+:	8f ea 50 12 c5 34 12[ 	]+lwpins \$0x1234,%ebp,%bp
[ 	]*[a-f0-9]+:	8f ea 48 12 c6 34 12[ 	]+lwpins \$0x1234,%esi,%si
[ 	]*[a-f0-9]+:	8f ea 40 12 c7 34 12[ 	]+lwpins \$0x1234,%edi,%di
[ 	]*[a-f0-9]+:	8f ea 7c 12 c7 78 56 34 12[ 	]+lwpins \$0x12345678,%edi,%eax
[ 	]*[a-f0-9]+:	8f ea 74 12 c6 78 56 34 12[ 	]+lwpins \$0x12345678,%esi,%ecx
[ 	]*[a-f0-9]+:	8f ea 6c 12 c5 78 56 34 12[ 	]+lwpins \$0x12345678,%ebp,%edx
[ 	]*[a-f0-9]+:	8f ea 64 12 c4 78 56 34 12[ 	]+lwpins \$0x12345678,%esp,%ebx
[ 	]*[a-f0-9]+:	8f ea 5c 12 c3 78 56 34 12[ 	]+lwpins \$0x12345678,%ebx,%esp
[ 	]*[a-f0-9]+:	8f ea 54 12 c2 78 56 34 12[ 	]+lwpins \$0x12345678,%edx,%ebp
[ 	]*[a-f0-9]+:	8f ea 4c 12 c1 78 56 34 12[ 	]+lwpins \$0x12345678,%ecx,%esi
[ 	]*[a-f0-9]+:	8f ea 44 12 c0 78 56 34 12[ 	]+lwpins \$0x12345678,%eax,%edi
[ 	]*[a-f0-9]+:	8f ea f8 12 c0 78 56 34 12[ 	]+lwpins \$0x12345678,%eax,%rax
[ 	]*[a-f0-9]+:	8f ea f0 12 c1 78 56 34 12[ 	]+lwpins \$0x12345678,%ecx,%rcx
[ 	]*[a-f0-9]+:	8f ea e8 12 c2 78 56 34 12[ 	]+lwpins \$0x12345678,%edx,%rdx
[ 	]*[a-f0-9]+:	8f ea e0 12 c3 78 56 34 12[ 	]+lwpins \$0x12345678,%ebx,%rbx
[ 	]*[a-f0-9]+:	8f ea d8 12 c4 78 56 34 12[ 	]+lwpins \$0x12345678,%esp,%rsp
[ 	]*[a-f0-9]+:	8f ea d0 12 c5 78 56 34 12[ 	]+lwpins \$0x12345678,%ebp,%rbp
[ 	]*[a-f0-9]+:	8f ea c8 12 c6 78 56 34 12[ 	]+lwpins \$0x12345678,%esi,%rsi
[ 	]*[a-f0-9]+:	8f ea c0 12 c7 78 56 34 12[ 	]+lwpins \$0x12345678,%edi,%rdi
[ 	]*[a-f0-9]+:	8f ea 78 12 c8 34 12[ 	]+lwpval \$0x1234,%eax,%ax
[ 	]*[a-f0-9]+:	8f ea 70 12 c9 34 12[ 	]+lwpval \$0x1234,%ecx,%cx
[ 	]*[a-f0-9]+:	8f ea 68 12 ca 34 12[ 	]+lwpval \$0x1234,%edx,%dx
[ 	]*[a-f0-9]+:	8f ea 60 12 cb 34 12[ 	]+lwpval \$0x1234,%ebx,%bx
[ 	]*[a-f0-9]+:	8f ea 58 12 cc 34 12[ 	]+lwpval \$0x1234,%esp,%sp
[ 	]*[a-f0-9]+:	8f ea 50 12 cd 34 12[ 	]+lwpval \$0x1234,%ebp,%bp
[ 	]*[a-f0-9]+:	8f ea 48 12 ce 34 12[ 	]+lwpval \$0x1234,%esi,%si
[ 	]*[a-f0-9]+:	8f ea 40 12 cf 34 12[ 	]+lwpval \$0x1234,%edi,%di
[ 	]*[a-f0-9]+:	8f ea 7c 12 cf 78 56 34 12[ 	]+lwpval \$0x12345678,%edi,%eax
[ 	]*[a-f0-9]+:	8f ea 74 12 ce 78 56 34 12[ 	]+lwpval \$0x12345678,%esi,%ecx
[ 	]*[a-f0-9]+:	8f ea 6c 12 cd 78 56 34 12[ 	]+lwpval \$0x12345678,%ebp,%edx
[ 	]*[a-f0-9]+:	8f ea 64 12 cc 78 56 34 12[ 	]+lwpval \$0x12345678,%esp,%ebx
[ 	]*[a-f0-9]+:	8f ea 5c 12 cb 78 56 34 12[ 	]+lwpval \$0x12345678,%ebx,%esp
[ 	]*[a-f0-9]+:	8f ea 54 12 ca 78 56 34 12[ 	]+lwpval \$0x12345678,%edx,%ebp
[ 	]*[a-f0-9]+:	8f ea 4c 12 c9 78 56 34 12[ 	]+lwpval \$0x12345678,%ecx,%esi
[ 	]*[a-f0-9]+:	8f ea 44 12 c8 78 56 34 12[ 	]+lwpval \$0x12345678,%eax,%edi
[ 	]*[a-f0-9]+:	8f ea f8 12 c8 78 56 34 12[ 	]+lwpval \$0x12345678,%eax,%rax
[ 	]*[a-f0-9]+:	8f ea f0 12 c9 78 56 34 12[ 	]+lwpval \$0x12345678,%ecx,%rcx
[ 	]*[a-f0-9]+:	8f ea e8 12 ca 78 56 34 12[ 	]+lwpval \$0x12345678,%edx,%rdx
[ 	]*[a-f0-9]+:	8f ea e0 12 cb 78 56 34 12[ 	]+lwpval \$0x12345678,%ebx,%rbx
[ 	]*[a-f0-9]+:	8f ea d8 12 cc 78 56 34 12[ 	]+lwpval \$0x12345678,%esp,%rsp
[ 	]*[a-f0-9]+:	8f ea d0 12 cd 78 56 34 12[ 	]+lwpval \$0x12345678,%ebp,%rbp
[ 	]*[a-f0-9]+:	8f ea c8 12 ce 78 56 34 12[ 	]+lwpval \$0x12345678,%esi,%rsi
[ 	]*[a-f0-9]+:	8f ea c0 12 cf 78 56 34 12[ 	]+lwpval \$0x12345678,%edi,%rdi
[ 	]*[a-f0-9]+:	67 8f ea 78 12 00 34 12[ 	]+addr32 lwpins \$0x1234,\(%eax\),%ax
[ 	]*[a-f0-9]+:	67 8f ea 70 12 01 34 12[ 	]+addr32 lwpins \$0x1234,\(%ecx\),%cx
[ 	]*[a-f0-9]+:	67 8f ea 68 12 02 34 12[ 	]+addr32 lwpins \$0x1234,\(%edx\),%dx
[ 	]*[a-f0-9]+:	67 8f ea 60 12 03 34 12[ 	]+addr32 lwpins \$0x1234,\(%ebx\),%bx
[ 	]*[a-f0-9]+:	67 8f ea 58 12 04 24 34 12[ 	]+addr32 lwpins \$0x1234,\(%esp\),%sp
[ 	]*[a-f0-9]+:	67 8f ea 50 12 45 00 34 12[ 	]+addr32 lwpins \$0x1234,0x0\(%ebp\),%bp
[ 	]*[a-f0-9]+:	67 8f ea 48 12 06 34 12[ 	]+addr32 lwpins \$0x1234,\(%esi\),%si
[ 	]*[a-f0-9]+:	67 8f ea 40 12 07 34 12[ 	]+addr32 lwpins \$0x1234,\(%edi\),%di
[ 	]*[a-f0-9]+:	67 8f ea 7c 12 07 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,\(%edi\),%eax
[ 	]*[a-f0-9]+:	67 8f ea 74 12 06 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,\(%esi\),%ecx
[ 	]*[a-f0-9]+:	67 8f ea 6c 12 45 00 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,0x0\(%ebp\),%edx
[ 	]*[a-f0-9]+:	67 8f ea 64 12 04 24 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,\(%esp\),%ebx
[ 	]*[a-f0-9]+:	67 8f ea 5c 12 03 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,\(%ebx\),%esp
[ 	]*[a-f0-9]+:	67 8f ea 54 12 02 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,\(%edx\),%ebp
[ 	]*[a-f0-9]+:	67 8f ea 4c 12 01 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,\(%ecx\),%esi
[ 	]*[a-f0-9]+:	67 8f ea 44 12 00 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,\(%eax\),%edi
[ 	]*[a-f0-9]+:	67 8f ea f8 12 00 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,\(%eax\),%rax
[ 	]*[a-f0-9]+:	67 8f ea f0 12 01 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,\(%ecx\),%rcx
[ 	]*[a-f0-9]+:	67 8f ea e8 12 02 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,\(%edx\),%rdx
[ 	]*[a-f0-9]+:	67 8f ea e0 12 03 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,\(%ebx\),%rbx
[ 	]*[a-f0-9]+:	67 8f ea d8 12 04 24 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,\(%esp\),%rsp
[ 	]*[a-f0-9]+:	67 8f ea d0 12 45 00 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,0x0\(%ebp\),%rbp
[ 	]*[a-f0-9]+:	67 8f ea c8 12 06 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,\(%esi\),%rsi
[ 	]*[a-f0-9]+:	67 8f ea c0 12 07 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,\(%edi\),%rdi
[ 	]*[a-f0-9]+:	67 8f ea 78 12 08 34 12[ 	]+addr32 lwpval \$0x1234,\(%eax\),%ax
[ 	]*[a-f0-9]+:	67 8f ea 70 12 09 34 12[ 	]+addr32 lwpval \$0x1234,\(%ecx\),%cx
[ 	]*[a-f0-9]+:	67 8f ea 68 12 0a 34 12[ 	]+addr32 lwpval \$0x1234,\(%edx\),%dx
[ 	]*[a-f0-9]+:	67 8f ea 60 12 0b 34 12[ 	]+addr32 lwpval \$0x1234,\(%ebx\),%bx
[ 	]*[a-f0-9]+:	67 8f ea 58 12 0c 24 34 12[ 	]+addr32 lwpval \$0x1234,\(%esp\),%sp
[ 	]*[a-f0-9]+:	67 8f ea 50 12 4d 00 34 12[ 	]+addr32 lwpval \$0x1234,0x0\(%ebp\),%bp
[ 	]*[a-f0-9]+:	67 8f ea 48 12 0e 34 12[ 	]+addr32 lwpval \$0x1234,\(%esi\),%si
[ 	]*[a-f0-9]+:	67 8f ea 40 12 0f 34 12[ 	]+addr32 lwpval \$0x1234,\(%edi\),%di
[ 	]*[a-f0-9]+:	67 8f ea 7c 12 0f 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,\(%edi\),%eax
[ 	]*[a-f0-9]+:	67 8f ea 74 12 0e 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,\(%esi\),%ecx
[ 	]*[a-f0-9]+:	67 8f ea 6c 12 4d 00 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,0x0\(%ebp\),%edx
[ 	]*[a-f0-9]+:	67 8f ea 64 12 0c 24 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,\(%esp\),%ebx
[ 	]*[a-f0-9]+:	67 8f ea 5c 12 0b 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,\(%ebx\),%esp
[ 	]*[a-f0-9]+:	67 8f ea 54 12 0a 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,\(%edx\),%ebp
[ 	]*[a-f0-9]+:	67 8f ea 4c 12 09 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,\(%ecx\),%esi
[ 	]*[a-f0-9]+:	67 8f ea 44 12 08 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,\(%eax\),%edi
[ 	]*[a-f0-9]+:	67 8f ea f8 12 08 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,\(%eax\),%rax
[ 	]*[a-f0-9]+:	67 8f ea f0 12 09 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,\(%ecx\),%rcx
[ 	]*[a-f0-9]+:	67 8f ea e8 12 0a 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,\(%edx\),%rdx
[ 	]*[a-f0-9]+:	67 8f ea e0 12 0b 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,\(%ebx\),%rbx
[ 	]*[a-f0-9]+:	67 8f ea d8 12 0c 24 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,\(%esp\),%rsp
[ 	]*[a-f0-9]+:	67 8f ea d0 12 4d 00 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,0x0\(%ebp\),%rbp
[ 	]*[a-f0-9]+:	67 8f ea c8 12 0e 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,\(%esi\),%rsi
[ 	]*[a-f0-9]+:	67 8f ea c0 12 0f 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,\(%edi\),%rdi
[ 	]*[a-f0-9]+:	67 8f ea 78 12 80 fe ca 00 00 34 12[ 	]+addr32 lwpins \$0x1234,0xcafe\(%eax\),%ax
[ 	]*[a-f0-9]+:	67 8f ea 70 12 81 fe ca 00 00 34 12[ 	]+addr32 lwpins \$0x1234,0xcafe\(%ecx\),%cx
[ 	]*[a-f0-9]+:	67 8f ea 68 12 82 fe ca 00 00 34 12[ 	]+addr32 lwpins \$0x1234,0xcafe\(%edx\),%dx
[ 	]*[a-f0-9]+:	67 8f ea 60 12 83 fe ca 00 00 34 12[ 	]+addr32 lwpins \$0x1234,0xcafe\(%ebx\),%bx
[ 	]*[a-f0-9]+:	67 8f ea 58 12 84 24 fe ca 00 00 34 12[ 	]+addr32 lwpins \$0x1234,0xcafe\(%esp\),%sp
[ 	]*[a-f0-9]+:	67 8f ea 50 12 85 fe ca 00 00 34 12[ 	]+addr32 lwpins \$0x1234,0xcafe\(%ebp\),%bp
[ 	]*[a-f0-9]+:	67 8f ea 48 12 86 fe ca 00 00 34 12[ 	]+addr32 lwpins \$0x1234,0xcafe\(%esi\),%si
[ 	]*[a-f0-9]+:	67 8f ea 40 12 87 fe ca 00 00 34 12[ 	]+addr32 lwpins \$0x1234,0xcafe\(%edi\),%di
[ 	]*[a-f0-9]+:	67 8f ea 7c 12 87 fe ca 00 00 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,0xcafe\(%edi\),%eax
[ 	]*[a-f0-9]+:	67 8f ea 74 12 86 fe ca 00 00 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,0xcafe\(%esi\),%ecx
[ 	]*[a-f0-9]+:	67 8f ea 6c 12 85 fe ca 00 00 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,0xcafe\(%ebp\),%edx
[ 	]*[a-f0-9]+:	67 8f ea 64 12 84 24 fe ca 00 00 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,0xcafe\(%esp\),%ebx
[ 	]*[a-f0-9]+:	67 8f ea 5c 12 83 fe ca 00 00 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,0xcafe\(%ebx\),%esp
[ 	]*[a-f0-9]+:	67 8f ea 54 12 82 fe ca 00 00 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,0xcafe\(%edx\),%ebp
[ 	]*[a-f0-9]+:	67 8f ea 4c 12 81 fe ca 00 00 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,0xcafe\(%ecx\),%esi
[ 	]*[a-f0-9]+:	67 8f ea 44 12 80 fe ca 00 00 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,0xcafe\(%eax\),%edi
[ 	]*[a-f0-9]+:	67 8f ea f8 12 80 fe ca 00 00 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,0xcafe\(%eax\),%rax
[ 	]*[a-f0-9]+:	67 8f ea f0 12 81 fe ca 00 00 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,0xcafe\(%ecx\),%rcx
[ 	]*[a-f0-9]+:	67 8f ea e8 12 82 fe ca 00 00 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,0xcafe\(%edx\),%rdx
[ 	]*[a-f0-9]+:	67 8f ea e0 12 83 fe ca 00 00 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,0xcafe\(%ebx\),%rbx
[ 	]*[a-f0-9]+:	67 8f ea d8 12 84 24 fe ca 00 00 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,0xcafe\(%esp\),%rsp
[ 	]*[a-f0-9]+:	67 8f ea d0 12 85 fe ca 00 00 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,0xcafe\(%ebp\),%rbp
[ 	]*[a-f0-9]+:	67 8f ea c8 12 86 fe ca 00 00 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,0xcafe\(%esi\),%rsi
[ 	]*[a-f0-9]+:	67 8f ea c0 12 87 fe ca 00 00 78 56 34 12[ 	]+addr32 lwpins \$0x12345678,0xcafe\(%edi\),%rdi
[ 	]*[a-f0-9]+:	67 8f ea 78 12 88 fe ca 00 00 34 12[ 	]+addr32 lwpval \$0x1234,0xcafe\(%eax\),%ax
[ 	]*[a-f0-9]+:	67 8f ea 70 12 89 fe ca 00 00 34 12[ 	]+addr32 lwpval \$0x1234,0xcafe\(%ecx\),%cx
[ 	]*[a-f0-9]+:	67 8f ea 68 12 8a fe ca 00 00 34 12[ 	]+addr32 lwpval \$0x1234,0xcafe\(%edx\),%dx
[ 	]*[a-f0-9]+:	67 8f ea 60 12 8b fe ca 00 00 34 12[ 	]+addr32 lwpval \$0x1234,0xcafe\(%ebx\),%bx
[ 	]*[a-f0-9]+:	67 8f ea 58 12 8c 24 fe ca 00 00 34 12[ 	]+addr32 lwpval \$0x1234,0xcafe\(%esp\),%sp
[ 	]*[a-f0-9]+:	67 8f ea 50 12 8d fe ca 00 00 34 12[ 	]+addr32 lwpval \$0x1234,0xcafe\(%ebp\),%bp
[ 	]*[a-f0-9]+:	67 8f ea 48 12 8e fe ca 00 00 34 12[ 	]+addr32 lwpval \$0x1234,0xcafe\(%esi\),%si
[ 	]*[a-f0-9]+:	67 8f ea 40 12 8f fe ca 00 00 34 12[ 	]+addr32 lwpval \$0x1234,0xcafe\(%edi\),%di
[ 	]*[a-f0-9]+:	67 8f ea 7c 12 8f fe ca 00 00 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,0xcafe\(%edi\),%eax
[ 	]*[a-f0-9]+:	67 8f ea 74 12 8e fe ca 00 00 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,0xcafe\(%esi\),%ecx
[ 	]*[a-f0-9]+:	67 8f ea 6c 12 8d fe ca 00 00 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,0xcafe\(%ebp\),%edx
[ 	]*[a-f0-9]+:	67 8f ea 64 12 8c 24 fe ca 00 00 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,0xcafe\(%esp\),%ebx
[ 	]*[a-f0-9]+:	67 8f ea 5c 12 8b fe ca 00 00 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,0xcafe\(%ebx\),%esp
[ 	]*[a-f0-9]+:	67 8f ea 54 12 8a fe ca 00 00 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,0xcafe\(%edx\),%ebp
[ 	]*[a-f0-9]+:	67 8f ea 4c 12 89 fe ca 00 00 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,0xcafe\(%ecx\),%esi
[ 	]*[a-f0-9]+:	67 8f ea 44 12 88 fe ca 00 00 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,0xcafe\(%eax\),%edi
[ 	]*[a-f0-9]+:	67 8f ea f8 12 88 fe ca 00 00 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,0xcafe\(%eax\),%rax
[ 	]*[a-f0-9]+:	67 8f ea f0 12 89 fe ca 00 00 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,0xcafe\(%ecx\),%rcx
[ 	]*[a-f0-9]+:	67 8f ea e8 12 8a fe ca 00 00 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,0xcafe\(%edx\),%rdx
[ 	]*[a-f0-9]+:	67 8f ea e0 12 8b fe ca 00 00 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,0xcafe\(%ebx\),%rbx
[ 	]*[a-f0-9]+:	67 8f ea d8 12 8c 24 fe ca 00 00 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,0xcafe\(%esp\),%rsp
[ 	]*[a-f0-9]+:	67 8f ea d0 12 8d fe ca 00 00 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,0xcafe\(%ebp\),%rbp
[ 	]*[a-f0-9]+:	67 8f ea c8 12 8e fe ca 00 00 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,0xcafe\(%esi\),%rsi
[ 	]*[a-f0-9]+:	67 8f ea c0 12 8f fe ca 00 00 78 56 34 12[ 	]+addr32 lwpval \$0x12345678,0xcafe\(%edi\),%rdi
#pass
