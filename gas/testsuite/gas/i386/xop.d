#objdump: -dw
#name: i386 XOP

.*: +file format .*

Disassembly of section .text:

0+ <_start>:
[ 	]*[a-f0-9]+:	8f e9 78 81 ff[ 	]+vfrczpd %xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 81 f0[ 	]+vfrczpd %xmm0,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 81 03[ 	]+vfrczpd \(%ebx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 81 3e[ 	]+vfrczpd \(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 81 c0[ 	]+vfrczpd %xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 81 38[ 	]+vfrczpd \(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 81 c7[ 	]+vfrczpd %xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 81 f1[ 	]+vfrczpd %xmm1,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 81 c1[ 	]+vfrczpd %xmm1,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 81 f8[ 	]+vfrczpd %xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 81 30[ 	]+vfrczpd \(%eax\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 81 f9[ 	]+vfrczpd %xmm1,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 81 06[ 	]+vfrczpd \(%esi\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 81 3b[ 	]+vfrczpd \(%ebx\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 81 36[ 	]+vfrczpd \(%esi\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 81 00[ 	]+vfrczpd \(%eax\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 7c 81 ff[ 	]+vfrczpd %ymm7,%ymm7
[ 	]*[a-f0-9]+:	8f e9 7c 81 f0[ 	]+vfrczpd %ymm0,%ymm6
[ 	]*[a-f0-9]+:	8f e9 7c 81 03[ 	]+vfrczpd \(%ebx\),%ymm0
[ 	]*[a-f0-9]+:	8f e9 7c 81 3e[ 	]+vfrczpd \(%esi\),%ymm7
[ 	]*[a-f0-9]+:	8f e9 7c 81 c0[ 	]+vfrczpd %ymm0,%ymm0
[ 	]*[a-f0-9]+:	8f e9 7c 81 38[ 	]+vfrczpd \(%eax\),%ymm7
[ 	]*[a-f0-9]+:	8f e9 7c 81 c7[ 	]+vfrczpd %ymm7,%ymm0
[ 	]*[a-f0-9]+:	8f e9 7c 81 f1[ 	]+vfrczpd %ymm1,%ymm6
[ 	]*[a-f0-9]+:	8f e9 7c 81 c1[ 	]+vfrczpd %ymm1,%ymm0
[ 	]*[a-f0-9]+:	8f e9 7c 81 f8[ 	]+vfrczpd %ymm0,%ymm7
[ 	]*[a-f0-9]+:	8f e9 7c 81 30[ 	]+vfrczpd \(%eax\),%ymm6
[ 	]*[a-f0-9]+:	8f e9 7c 81 f9[ 	]+vfrczpd %ymm1,%ymm7
[ 	]*[a-f0-9]+:	8f e9 7c 81 06[ 	]+vfrczpd \(%esi\),%ymm0
[ 	]*[a-f0-9]+:	8f e9 7c 81 3b[ 	]+vfrczpd \(%ebx\),%ymm7
[ 	]*[a-f0-9]+:	8f e9 7c 81 36[ 	]+vfrczpd \(%esi\),%ymm6
[ 	]*[a-f0-9]+:	8f e9 7c 81 00[ 	]+vfrczpd \(%eax\),%ymm0
[ 	]*[a-f0-9]+:	8f e9 78 80 ff[ 	]+vfrczps %xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 80 f0[ 	]+vfrczps %xmm0,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 80 03[ 	]+vfrczps \(%ebx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 80 3e[ 	]+vfrczps \(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 80 c0[ 	]+vfrczps %xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 80 38[ 	]+vfrczps \(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 80 c7[ 	]+vfrczps %xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 80 f1[ 	]+vfrczps %xmm1,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 80 c1[ 	]+vfrczps %xmm1,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 80 f8[ 	]+vfrczps %xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 80 30[ 	]+vfrczps \(%eax\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 80 f9[ 	]+vfrczps %xmm1,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 80 06[ 	]+vfrczps \(%esi\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 80 3b[ 	]+vfrczps \(%ebx\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 80 36[ 	]+vfrczps \(%esi\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 80 00[ 	]+vfrczps \(%eax\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 7c 80 ff[ 	]+vfrczps %ymm7,%ymm7
[ 	]*[a-f0-9]+:	8f e9 7c 80 f0[ 	]+vfrczps %ymm0,%ymm6
[ 	]*[a-f0-9]+:	8f e9 7c 80 03[ 	]+vfrczps \(%ebx\),%ymm0
[ 	]*[a-f0-9]+:	8f e9 7c 80 3e[ 	]+vfrczps \(%esi\),%ymm7
[ 	]*[a-f0-9]+:	8f e9 7c 80 c0[ 	]+vfrczps %ymm0,%ymm0
[ 	]*[a-f0-9]+:	8f e9 7c 80 38[ 	]+vfrczps \(%eax\),%ymm7
[ 	]*[a-f0-9]+:	8f e9 7c 80 c7[ 	]+vfrczps %ymm7,%ymm0
[ 	]*[a-f0-9]+:	8f e9 7c 80 f1[ 	]+vfrczps %ymm1,%ymm6
[ 	]*[a-f0-9]+:	8f e9 7c 80 c1[ 	]+vfrczps %ymm1,%ymm0
[ 	]*[a-f0-9]+:	8f e9 7c 80 f8[ 	]+vfrczps %ymm0,%ymm7
[ 	]*[a-f0-9]+:	8f e9 7c 80 30[ 	]+vfrczps \(%eax\),%ymm6
[ 	]*[a-f0-9]+:	8f e9 7c 80 f9[ 	]+vfrczps %ymm1,%ymm7
[ 	]*[a-f0-9]+:	8f e9 7c 80 06[ 	]+vfrczps \(%esi\),%ymm0
[ 	]*[a-f0-9]+:	8f e9 7c 80 3b[ 	]+vfrczps \(%ebx\),%ymm7
[ 	]*[a-f0-9]+:	8f e9 7c 80 36[ 	]+vfrczps \(%esi\),%ymm6
[ 	]*[a-f0-9]+:	8f e9 7c 80 00[ 	]+vfrczps \(%eax\),%ymm0
[ 	]*[a-f0-9]+:	8f e9 78 83 ff[ 	]+vfrczsd %xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 83 f0[ 	]+vfrczsd %xmm0,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 83 03[ 	]+vfrczsd \(%ebx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 83 3e[ 	]+vfrczsd \(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 83 c0[ 	]+vfrczsd %xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 83 38[ 	]+vfrczsd \(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 83 c7[ 	]+vfrczsd %xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 83 f1[ 	]+vfrczsd %xmm1,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 83 c1[ 	]+vfrczsd %xmm1,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 83 f8[ 	]+vfrczsd %xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 83 30[ 	]+vfrczsd \(%eax\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 83 f9[ 	]+vfrczsd %xmm1,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 83 06[ 	]+vfrczsd \(%esi\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 83 3b[ 	]+vfrczsd \(%ebx\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 83 36[ 	]+vfrczsd \(%esi\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 83 00[ 	]+vfrczsd \(%eax\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 82 ff[ 	]+vfrczss %xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 82 f0[ 	]+vfrczss %xmm0,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 82 03[ 	]+vfrczss \(%ebx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 82 3e[ 	]+vfrczss \(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 82 c0[ 	]+vfrczss %xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 82 38[ 	]+vfrczss \(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 82 c7[ 	]+vfrczss %xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 82 f1[ 	]+vfrczss %xmm1,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 82 c1[ 	]+vfrczss %xmm1,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 82 f8[ 	]+vfrczss %xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 82 30[ 	]+vfrczss \(%eax\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 82 f9[ 	]+vfrczss %xmm1,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 82 06[ 	]+vfrczss \(%esi\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 82 3b[ 	]+vfrczss \(%ebx\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 82 36[ 	]+vfrczss \(%esi\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 82 00[ 	]+vfrczss \(%eax\),%xmm0
[ 	]*[a-f0-9]+:	8f e8 40 a2 c7 00[ 	]+vpcmov %xmm0,%xmm7,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 a2 06 70[ 	]+vpcmov %xmm7,\(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 a2 06 10[ 	]+vpcmov %xmm1,\(%esi\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 a2 e8 10[ 	]+vpcmov %xmm1,%xmm0,%xmm0,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 a2 c6 10[ 	]+vpcmov %xmm1,%xmm6,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 a2 fe 10[ 	]+vpcmov %xmm1,%xmm6,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 a2 3a 10[ 	]+vpcmov %xmm1,\(%edx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 a2 f8 70[ 	]+vpcmov %xmm7,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 a2 3e 70[ 	]+vpcmov %xmm7,\(%esi\),%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 a2 fe 70[ 	]+vpcmov %xmm7,%xmm6,%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 a2 c7 70[ 	]+vpcmov %xmm7,%xmm7,%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 a2 02 00[ 	]+vpcmov %xmm0,\(%edx\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 40 a2 2a 10[ 	]+vpcmov %xmm1,\(%edx\),%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 a2 ef 10[ 	]+vpcmov %xmm1,%xmm7,%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 a2 c7 10[ 	]+vpcmov %xmm1,%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 a2 2e 70[ 	]+vpcmov %xmm7,\(%esi\),%xmm3,%xmm5
[ 	]*[a-f0-9]+:	8f e8 44 a2 c7 00[ 	]+vpcmov %ymm0,%ymm7,%ymm7,%ymm0
[ 	]*[a-f0-9]+:	8f e8 7c a2 06 70[ 	]+vpcmov %ymm7,\(%esi\),%ymm0,%ymm0
[ 	]*[a-f0-9]+:	8f e8 64 a2 06 10[ 	]+vpcmov %ymm1,\(%esi\),%ymm3,%ymm0
[ 	]*[a-f0-9]+:	8f e8 7c a2 e8 10[ 	]+vpcmov %ymm1,%ymm0,%ymm0,%ymm5
[ 	]*[a-f0-9]+:	8f e8 7c a2 c6 10[ 	]+vpcmov %ymm1,%ymm6,%ymm0,%ymm0
[ 	]*[a-f0-9]+:	8f e8 7c a2 fe 10[ 	]+vpcmov %ymm1,%ymm6,%ymm0,%ymm7
[ 	]*[a-f0-9]+:	8f e8 7c a2 3a 10[ 	]+vpcmov %ymm1,\(%edx\),%ymm0,%ymm7
[ 	]*[a-f0-9]+:	8f e8 7c a2 f8 70[ 	]+vpcmov %ymm7,%ymm0,%ymm0,%ymm7
[ 	]*[a-f0-9]+:	8f e8 64 a2 3e 70[ 	]+vpcmov %ymm7,\(%esi\),%ymm3,%ymm7
[ 	]*[a-f0-9]+:	8f e8 64 a2 fe 70[ 	]+vpcmov %ymm7,%ymm6,%ymm3,%ymm7
[ 	]*[a-f0-9]+:	8f e8 64 a2 c7 70[ 	]+vpcmov %ymm7,%ymm7,%ymm3,%ymm0
[ 	]*[a-f0-9]+:	8f e8 64 a2 02 00[ 	]+vpcmov %ymm0,\(%edx\),%ymm3,%ymm0
[ 	]*[a-f0-9]+:	8f e8 44 a2 2a 10[ 	]+vpcmov %ymm1,\(%edx\),%ymm7,%ymm5
[ 	]*[a-f0-9]+:	8f e8 44 a2 ef 10[ 	]+vpcmov %ymm1,%ymm7,%ymm7,%ymm5
[ 	]*[a-f0-9]+:	8f e8 7c a2 c7 10[ 	]+vpcmov %ymm1,%ymm7,%ymm0,%ymm0
[ 	]*[a-f0-9]+:	8f e8 64 a2 2e 70[ 	]+vpcmov %ymm7,\(%esi\),%ymm3,%ymm5
[ 	]*[a-f0-9]+:	8f e8 40 a2 c6 00[ 	]+vpcmov %xmm0,%xmm6,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 f8 a2 06 70[ 	]+vpcmov \(%esi\),%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 e0 a2 00 70[ 	]+vpcmov \(%eax\),%xmm7,%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 a2 e8 70[ 	]+vpcmov %xmm7,%xmm0,%xmm0,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 a2 c0 70[ 	]+vpcmov %xmm7,%xmm0,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 a2 f8 70[ 	]+vpcmov %xmm7,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 f8 a2 38 60[ 	]+vpcmov \(%eax\),%xmm6,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 f8 a2 3e 00[ 	]+vpcmov \(%esi\),%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 e0 a2 3b 70[ 	]+vpcmov \(%ebx\),%xmm7,%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 e0 a2 3b 00[ 	]+vpcmov \(%ebx\),%xmm0,%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 e0 a2 06 60[ 	]+vpcmov \(%esi\),%xmm6,%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 a2 c7 10[ 	]+vpcmov %xmm1,%xmm7,%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 c0 a2 28 70[ 	]+vpcmov \(%eax\),%xmm7,%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 a2 ee 70[ 	]+vpcmov %xmm7,%xmm6,%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 a2 c6 70[ 	]+vpcmov %xmm7,%xmm6,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 e0 a2 2b 70[ 	]+vpcmov \(%ebx\),%xmm7,%xmm3,%xmm5
[ 	]*[a-f0-9]+:	8f e8 44 a2 c6 00[ 	]+vpcmov %ymm0,%ymm6,%ymm7,%ymm0
[ 	]*[a-f0-9]+:	8f e8 fc a2 06 70[ 	]+vpcmov \(%esi\),%ymm7,%ymm0,%ymm0
[ 	]*[a-f0-9]+:	8f e8 e4 a2 00 70[ 	]+vpcmov \(%eax\),%ymm7,%ymm3,%ymm0
[ 	]*[a-f0-9]+:	8f e8 7c a2 e8 70[ 	]+vpcmov %ymm7,%ymm0,%ymm0,%ymm5
[ 	]*[a-f0-9]+:	8f e8 7c a2 c0 70[ 	]+vpcmov %ymm7,%ymm0,%ymm0,%ymm0
[ 	]*[a-f0-9]+:	8f e8 7c a2 f8 70[ 	]+vpcmov %ymm7,%ymm0,%ymm0,%ymm7
[ 	]*[a-f0-9]+:	8f e8 fc a2 38 60[ 	]+vpcmov \(%eax\),%ymm6,%ymm0,%ymm7
[ 	]*[a-f0-9]+:	8f e8 fc a2 3e 00[ 	]+vpcmov \(%esi\),%ymm0,%ymm0,%ymm7
[ 	]*[a-f0-9]+:	8f e8 e4 a2 3b 70[ 	]+vpcmov \(%ebx\),%ymm7,%ymm3,%ymm7
[ 	]*[a-f0-9]+:	8f e8 e4 a2 3b 00[ 	]+vpcmov \(%ebx\),%ymm0,%ymm3,%ymm7
[ 	]*[a-f0-9]+:	8f e8 e4 a2 06 60[ 	]+vpcmov \(%esi\),%ymm6,%ymm3,%ymm0
[ 	]*[a-f0-9]+:	8f e8 64 a2 c7 10[ 	]+vpcmov %ymm1,%ymm7,%ymm3,%ymm0
[ 	]*[a-f0-9]+:	8f e8 c4 a2 28 70[ 	]+vpcmov \(%eax\),%ymm7,%ymm7,%ymm5
[ 	]*[a-f0-9]+:	8f e8 44 a2 ee 70[ 	]+vpcmov %ymm7,%ymm6,%ymm7,%ymm5
[ 	]*[a-f0-9]+:	8f e8 7c a2 c6 70[ 	]+vpcmov %ymm7,%ymm6,%ymm0,%ymm0
[ 	]*[a-f0-9]+:	8f e8 e4 a2 2b 70[ 	]+vpcmov \(%ebx\),%ymm7,%ymm3,%ymm5
[ 	]*[a-f0-9]+:	8f e8 78 cc 38 03[ 	]+vpcomb \$0x3,\(%eax\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 cc c8 ff[ 	]+vpcomb \$0xff,%xmm0,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 cc cd ff[ 	]+vpcomb \$0xff,%xmm5,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 cc cd 00[ 	]+vpcomb \$0x0,%xmm5,%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 cc cd 00[ 	]+vpcomb \$0x0,%xmm5,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 40 cc c8 00[ 	]+vpcomb \$0x0,%xmm0,%xmm7,%xmm1
[ 	]*[a-f0-9]+:	8f e8 40 cc f8 03[ 	]+vpcomb \$0x3,%xmm0,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 cc fd 00[ 	]+vpcomb \$0x0,%xmm5,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 cc ff ff[ 	]+vpcomb \$0xff,%xmm7,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 cc ff 00[ 	]+vpcomb \$0x0,%xmm7,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 cc c7 03[ 	]+vpcomb \$0x3,%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 cc cf ff[ 	]+vpcomb \$0xff,%xmm7,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 cc 08 ff[ 	]+vpcomb \$0xff,\(%eax\),%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 cc 08 03[ 	]+vpcomb \$0x3,\(%eax\),%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 cc f8 03[ 	]+vpcomb \$0x3,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 50 cc c7 ff[ 	]+vpcomb \$0xff,%xmm7,%xmm5,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 ce 38 03[ 	]+vpcomd \$0x3,\(%eax\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 ce c8 ff[ 	]+vpcomd \$0xff,%xmm0,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 ce cd ff[ 	]+vpcomd \$0xff,%xmm5,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 ce cd 00[ 	]+vpcomd \$0x0,%xmm5,%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 ce cd 00[ 	]+vpcomd \$0x0,%xmm5,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 40 ce c8 00[ 	]+vpcomd \$0x0,%xmm0,%xmm7,%xmm1
[ 	]*[a-f0-9]+:	8f e8 40 ce f8 03[ 	]+vpcomd \$0x3,%xmm0,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 ce fd 00[ 	]+vpcomd \$0x0,%xmm5,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 ce ff ff[ 	]+vpcomd \$0xff,%xmm7,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 ce ff 00[ 	]+vpcomd \$0x0,%xmm7,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 ce c7 03[ 	]+vpcomd \$0x3,%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 ce cf ff[ 	]+vpcomd \$0xff,%xmm7,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 ce 08 ff[ 	]+vpcomd \$0xff,\(%eax\),%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 ce 08 03[ 	]+vpcomd \$0x3,\(%eax\),%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 ce f8 03[ 	]+vpcomd \$0x3,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 50 ce c7 ff[ 	]+vpcomd \$0xff,%xmm7,%xmm5,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 cf 38 03[ 	]+vpcomq \$0x3,\(%eax\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 cf c8 ff[ 	]+vpcomq \$0xff,%xmm0,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 cf cd ff[ 	]+vpcomq \$0xff,%xmm5,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 cf cd 00[ 	]+vpcomq \$0x0,%xmm5,%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 cf cd 00[ 	]+vpcomq \$0x0,%xmm5,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 40 cf c8 00[ 	]+vpcomq \$0x0,%xmm0,%xmm7,%xmm1
[ 	]*[a-f0-9]+:	8f e8 40 cf f8 03[ 	]+vpcomq \$0x3,%xmm0,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 cf fd 00[ 	]+vpcomq \$0x0,%xmm5,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 cf ff ff[ 	]+vpcomq \$0xff,%xmm7,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 cf ff 00[ 	]+vpcomq \$0x0,%xmm7,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 cf c7 03[ 	]+vpcomq \$0x3,%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 cf cf ff[ 	]+vpcomq \$0xff,%xmm7,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 cf 08 ff[ 	]+vpcomq \$0xff,\(%eax\),%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 cf 08 03[ 	]+vpcomq \$0x3,\(%eax\),%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 cf f8 03[ 	]+vpcomq \$0x3,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 50 cf c7 ff[ 	]+vpcomq \$0xff,%xmm7,%xmm5,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 ec 38 03[ 	]+vpcomub \$0x3,\(%eax\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 ec c8 ff[ 	]+vpcomub \$0xff,%xmm0,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 ec cd ff[ 	]+vpcomub \$0xff,%xmm5,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 ec cd 00[ 	]+vpcomub \$0x0,%xmm5,%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 ec cd 00[ 	]+vpcomub \$0x0,%xmm5,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 40 ec c8 00[ 	]+vpcomub \$0x0,%xmm0,%xmm7,%xmm1
[ 	]*[a-f0-9]+:	8f e8 40 ec f8 03[ 	]+vpcomub \$0x3,%xmm0,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 ec fd 00[ 	]+vpcomub \$0x0,%xmm5,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 ec ff ff[ 	]+vpcomub \$0xff,%xmm7,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 ec ff 00[ 	]+vpcomub \$0x0,%xmm7,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 ec c7 03[ 	]+vpcomub \$0x3,%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 ec cf ff[ 	]+vpcomub \$0xff,%xmm7,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 ec 08 ff[ 	]+vpcomub \$0xff,\(%eax\),%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 ec 08 03[ 	]+vpcomub \$0x3,\(%eax\),%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 ec f8 03[ 	]+vpcomub \$0x3,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 50 ec c7 ff[ 	]+vpcomub \$0xff,%xmm7,%xmm5,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 ee 38 03[ 	]+vpcomud \$0x3,\(%eax\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 ee c8 ff[ 	]+vpcomud \$0xff,%xmm0,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 ee cd ff[ 	]+vpcomud \$0xff,%xmm5,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 ee cd 00[ 	]+vpcomud \$0x0,%xmm5,%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 ee cd 00[ 	]+vpcomud \$0x0,%xmm5,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 40 ee c8 00[ 	]+vpcomud \$0x0,%xmm0,%xmm7,%xmm1
[ 	]*[a-f0-9]+:	8f e8 40 ee f8 03[ 	]+vpcomud \$0x3,%xmm0,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 ee fd 00[ 	]+vpcomud \$0x0,%xmm5,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 ee ff ff[ 	]+vpcomud \$0xff,%xmm7,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 ee ff 00[ 	]+vpcomud \$0x0,%xmm7,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 ee c7 03[ 	]+vpcomud \$0x3,%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 ee cf ff[ 	]+vpcomud \$0xff,%xmm7,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 ee 08 ff[ 	]+vpcomud \$0xff,\(%eax\),%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 ee 08 03[ 	]+vpcomud \$0x3,\(%eax\),%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 ee f8 03[ 	]+vpcomud \$0x3,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 50 ee c7 ff[ 	]+vpcomud \$0xff,%xmm7,%xmm5,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 ef 38 03[ 	]+vpcomuq \$0x3,\(%eax\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 ef c8 ff[ 	]+vpcomuq \$0xff,%xmm0,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 ef cd ff[ 	]+vpcomuq \$0xff,%xmm5,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 ef cd 00[ 	]+vpcomuq \$0x0,%xmm5,%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 ef cd 00[ 	]+vpcomuq \$0x0,%xmm5,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 40 ef c8 00[ 	]+vpcomuq \$0x0,%xmm0,%xmm7,%xmm1
[ 	]*[a-f0-9]+:	8f e8 40 ef f8 03[ 	]+vpcomuq \$0x3,%xmm0,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 ef fd 00[ 	]+vpcomuq \$0x0,%xmm5,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 ef ff ff[ 	]+vpcomuq \$0xff,%xmm7,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 ef ff 00[ 	]+vpcomuq \$0x0,%xmm7,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 ef c7 03[ 	]+vpcomuq \$0x3,%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 ef cf ff[ 	]+vpcomuq \$0xff,%xmm7,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 ef 08 ff[ 	]+vpcomuq \$0xff,\(%eax\),%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 ef 08 03[ 	]+vpcomuq \$0x3,\(%eax\),%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 ef f8 03[ 	]+vpcomuq \$0x3,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 50 ef c7 ff[ 	]+vpcomuq \$0xff,%xmm7,%xmm5,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 ed 38 03[ 	]+vpcomuw \$0x3,\(%eax\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 ed c8 ff[ 	]+vpcomuw \$0xff,%xmm0,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 ed cd ff[ 	]+vpcomuw \$0xff,%xmm5,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 ed cd 00[ 	]+vpcomuw \$0x0,%xmm5,%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 ed cd 00[ 	]+vpcomuw \$0x0,%xmm5,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 40 ed c8 00[ 	]+vpcomuw \$0x0,%xmm0,%xmm7,%xmm1
[ 	]*[a-f0-9]+:	8f e8 40 ed f8 03[ 	]+vpcomuw \$0x3,%xmm0,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 ed fd 00[ 	]+vpcomuw \$0x0,%xmm5,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 ed ff ff[ 	]+vpcomuw \$0xff,%xmm7,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 ed ff 00[ 	]+vpcomuw \$0x0,%xmm7,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 ed c7 03[ 	]+vpcomuw \$0x3,%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 ed cf ff[ 	]+vpcomuw \$0xff,%xmm7,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 ed 08 ff[ 	]+vpcomuw \$0xff,\(%eax\),%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 ed 08 03[ 	]+vpcomuw \$0x3,\(%eax\),%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 ed f8 03[ 	]+vpcomuw \$0x3,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 50 ed c7 ff[ 	]+vpcomuw \$0xff,%xmm7,%xmm5,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 cd 38 03[ 	]+vpcomw \$0x3,\(%eax\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 cd c8 ff[ 	]+vpcomw \$0xff,%xmm0,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 cd cd ff[ 	]+vpcomw \$0xff,%xmm5,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 cd cd 00[ 	]+vpcomw \$0x0,%xmm5,%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 cd cd 00[ 	]+vpcomw \$0x0,%xmm5,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 40 cd c8 00[ 	]+vpcomw \$0x0,%xmm0,%xmm7,%xmm1
[ 	]*[a-f0-9]+:	8f e8 40 cd f8 03[ 	]+vpcomw \$0x3,%xmm0,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 cd fd 00[ 	]+vpcomw \$0x0,%xmm5,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 cd ff ff[ 	]+vpcomw \$0xff,%xmm7,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 40 cd ff 00[ 	]+vpcomw \$0x0,%xmm7,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 cd c7 03[ 	]+vpcomw \$0x3,%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 cd cf ff[ 	]+vpcomw \$0xff,%xmm7,%xmm0,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 cd 08 ff[ 	]+vpcomw \$0xff,\(%eax\),%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 50 cd 08 03[ 	]+vpcomw \$0x3,\(%eax\),%xmm5,%xmm1
[ 	]*[a-f0-9]+:	8f e8 78 cd f8 03[ 	]+vpcomw \$0x3,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 50 cd c7 ff[ 	]+vpcomw \$0xff,%xmm7,%xmm5,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c2 ff[ 	]+vphaddbd %xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c2 f0[ 	]+vphaddbd %xmm0,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 c2 03[ 	]+vphaddbd \(%ebx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c2 3e[ 	]+vphaddbd \(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c2 c0[ 	]+vphaddbd %xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c2 38[ 	]+vphaddbd \(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c2 c7[ 	]+vphaddbd %xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c2 f1[ 	]+vphaddbd %xmm1,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 c2 c1[ 	]+vphaddbd %xmm1,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c2 f8[ 	]+vphaddbd %xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c2 30[ 	]+vphaddbd \(%eax\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 c2 f9[ 	]+vphaddbd %xmm1,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c2 06[ 	]+vphaddbd \(%esi\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c2 3b[ 	]+vphaddbd \(%ebx\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c2 36[ 	]+vphaddbd \(%esi\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 c2 00[ 	]+vphaddbd \(%eax\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c3 ff[ 	]+vphaddbq %xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c3 f0[ 	]+vphaddbq %xmm0,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 c3 03[ 	]+vphaddbq \(%ebx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c3 3e[ 	]+vphaddbq \(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c3 c0[ 	]+vphaddbq %xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c3 38[ 	]+vphaddbq \(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c3 c7[ 	]+vphaddbq %xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c3 f1[ 	]+vphaddbq %xmm1,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 c3 c1[ 	]+vphaddbq %xmm1,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c3 f8[ 	]+vphaddbq %xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c3 30[ 	]+vphaddbq \(%eax\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 c3 f9[ 	]+vphaddbq %xmm1,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c3 06[ 	]+vphaddbq \(%esi\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c3 3b[ 	]+vphaddbq \(%ebx\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c3 36[ 	]+vphaddbq \(%esi\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 c3 00[ 	]+vphaddbq \(%eax\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c1 ff[ 	]+vphaddbw %xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c1 f0[ 	]+vphaddbw %xmm0,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 c1 03[ 	]+vphaddbw \(%ebx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c1 3e[ 	]+vphaddbw \(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c1 c0[ 	]+vphaddbw %xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c1 38[ 	]+vphaddbw \(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c1 c7[ 	]+vphaddbw %xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c1 f1[ 	]+vphaddbw %xmm1,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 c1 c1[ 	]+vphaddbw %xmm1,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c1 f8[ 	]+vphaddbw %xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c1 30[ 	]+vphaddbw \(%eax\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 c1 f9[ 	]+vphaddbw %xmm1,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c1 06[ 	]+vphaddbw \(%esi\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c1 3b[ 	]+vphaddbw \(%ebx\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c1 36[ 	]+vphaddbw \(%esi\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 c1 00[ 	]+vphaddbw \(%eax\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 cb ff[ 	]+vphadddq %xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 cb f0[ 	]+vphadddq %xmm0,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 cb 03[ 	]+vphadddq \(%ebx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 cb 3e[ 	]+vphadddq \(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 cb c0[ 	]+vphadddq %xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 cb 38[ 	]+vphadddq \(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 cb c7[ 	]+vphadddq %xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 cb f1[ 	]+vphadddq %xmm1,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 cb c1[ 	]+vphadddq %xmm1,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 cb f8[ 	]+vphadddq %xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 cb 30[ 	]+vphadddq \(%eax\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 cb f9[ 	]+vphadddq %xmm1,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 cb 06[ 	]+vphadddq \(%esi\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 cb 3b[ 	]+vphadddq \(%ebx\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 cb 36[ 	]+vphadddq \(%esi\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 cb 00[ 	]+vphadddq \(%eax\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d2 ff[ 	]+vphaddubd %xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d2 f0[ 	]+vphaddubd %xmm0,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 d2 03[ 	]+vphaddubd \(%ebx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d2 3e[ 	]+vphaddubd \(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d2 c0[ 	]+vphaddubd %xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d2 38[ 	]+vphaddubd \(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d2 c7[ 	]+vphaddubd %xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d2 f1[ 	]+vphaddubd %xmm1,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 d2 c1[ 	]+vphaddubd %xmm1,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d2 f8[ 	]+vphaddubd %xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d2 30[ 	]+vphaddubd \(%eax\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 d2 f9[ 	]+vphaddubd %xmm1,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d2 06[ 	]+vphaddubd \(%esi\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d2 3b[ 	]+vphaddubd \(%ebx\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d2 36[ 	]+vphaddubd \(%esi\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 d2 00[ 	]+vphaddubd \(%eax\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d3 ff[ 	]+vphaddubq %xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d3 f0[ 	]+vphaddubq %xmm0,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 d3 03[ 	]+vphaddubq \(%ebx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d3 3e[ 	]+vphaddubq \(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d3 c0[ 	]+vphaddubq %xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d3 38[ 	]+vphaddubq \(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d3 c7[ 	]+vphaddubq %xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d3 f1[ 	]+vphaddubq %xmm1,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 d3 c1[ 	]+vphaddubq %xmm1,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d3 f8[ 	]+vphaddubq %xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d3 30[ 	]+vphaddubq \(%eax\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 d3 f9[ 	]+vphaddubq %xmm1,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d3 06[ 	]+vphaddubq \(%esi\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d3 3b[ 	]+vphaddubq \(%ebx\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d3 36[ 	]+vphaddubq \(%esi\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 d3 00[ 	]+vphaddubq \(%eax\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d1 ff[ 	]+vphaddubw %xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d1 f0[ 	]+vphaddubw %xmm0,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 d1 03[ 	]+vphaddubw \(%ebx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d1 3e[ 	]+vphaddubw \(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d1 c0[ 	]+vphaddubw %xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d1 38[ 	]+vphaddubw \(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d1 c7[ 	]+vphaddubw %xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d1 f1[ 	]+vphaddubw %xmm1,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 d1 c1[ 	]+vphaddubw %xmm1,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d1 f8[ 	]+vphaddubw %xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d1 30[ 	]+vphaddubw \(%eax\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 d1 f9[ 	]+vphaddubw %xmm1,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d1 06[ 	]+vphaddubw \(%esi\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d1 3b[ 	]+vphaddubw \(%ebx\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d1 36[ 	]+vphaddubw \(%esi\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 d1 00[ 	]+vphaddubw \(%eax\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 db ff[ 	]+vphaddudq %xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 db f0[ 	]+vphaddudq %xmm0,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 db 03[ 	]+vphaddudq \(%ebx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 db 3e[ 	]+vphaddudq \(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 db c0[ 	]+vphaddudq %xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 db 38[ 	]+vphaddudq \(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 db c7[ 	]+vphaddudq %xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 db f1[ 	]+vphaddudq %xmm1,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 db c1[ 	]+vphaddudq %xmm1,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 db f8[ 	]+vphaddudq %xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 db 30[ 	]+vphaddudq \(%eax\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 db f9[ 	]+vphaddudq %xmm1,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 db 06[ 	]+vphaddudq \(%esi\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 db 3b[ 	]+vphaddudq \(%ebx\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 db 36[ 	]+vphaddudq \(%esi\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 db 00[ 	]+vphaddudq \(%eax\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d6 ff[ 	]+vphadduwd %xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d6 f0[ 	]+vphadduwd %xmm0,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 d6 03[ 	]+vphadduwd \(%ebx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d6 3e[ 	]+vphadduwd \(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d6 c0[ 	]+vphadduwd %xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d6 38[ 	]+vphadduwd \(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d6 c7[ 	]+vphadduwd %xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d6 f1[ 	]+vphadduwd %xmm1,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 d6 c1[ 	]+vphadduwd %xmm1,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d6 f8[ 	]+vphadduwd %xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d6 30[ 	]+vphadduwd \(%eax\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 d6 f9[ 	]+vphadduwd %xmm1,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d6 06[ 	]+vphadduwd \(%esi\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d6 3b[ 	]+vphadduwd \(%ebx\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d6 36[ 	]+vphadduwd \(%esi\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 d6 00[ 	]+vphadduwd \(%eax\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d7 ff[ 	]+vphadduwq %xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d7 f0[ 	]+vphadduwq %xmm0,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 d7 03[ 	]+vphadduwq \(%ebx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d7 3e[ 	]+vphadduwq \(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d7 c0[ 	]+vphadduwq %xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d7 38[ 	]+vphadduwq \(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d7 c7[ 	]+vphadduwq %xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d7 f1[ 	]+vphadduwq %xmm1,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 d7 c1[ 	]+vphadduwq %xmm1,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d7 f8[ 	]+vphadduwq %xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d7 30[ 	]+vphadduwq \(%eax\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 d7 f9[ 	]+vphadduwq %xmm1,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d7 06[ 	]+vphadduwq \(%esi\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 d7 3b[ 	]+vphadduwq \(%ebx\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 d7 36[ 	]+vphadduwq \(%esi\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 d7 00[ 	]+vphadduwq \(%eax\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c6 ff[ 	]+vphaddwd %xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c6 f0[ 	]+vphaddwd %xmm0,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 c6 03[ 	]+vphaddwd \(%ebx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c6 3e[ 	]+vphaddwd \(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c6 c0[ 	]+vphaddwd %xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c6 38[ 	]+vphaddwd \(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c6 c7[ 	]+vphaddwd %xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c6 f1[ 	]+vphaddwd %xmm1,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 c6 c1[ 	]+vphaddwd %xmm1,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c6 f8[ 	]+vphaddwd %xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c6 30[ 	]+vphaddwd \(%eax\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 c6 f9[ 	]+vphaddwd %xmm1,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c6 06[ 	]+vphaddwd \(%esi\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c6 3b[ 	]+vphaddwd \(%ebx\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c6 36[ 	]+vphaddwd \(%esi\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 c6 00[ 	]+vphaddwd \(%eax\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c7 ff[ 	]+vphaddwq %xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c7 f0[ 	]+vphaddwq %xmm0,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 c7 03[ 	]+vphaddwq \(%ebx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c7 3e[ 	]+vphaddwq \(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c7 c0[ 	]+vphaddwq %xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c7 38[ 	]+vphaddwq \(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c7 c7[ 	]+vphaddwq %xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c7 f1[ 	]+vphaddwq %xmm1,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 c7 c1[ 	]+vphaddwq %xmm1,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c7 f8[ 	]+vphaddwq %xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c7 30[ 	]+vphaddwq \(%eax\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 c7 f9[ 	]+vphaddwq %xmm1,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c7 06[ 	]+vphaddwq \(%esi\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 c7 3b[ 	]+vphaddwq \(%ebx\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 c7 36[ 	]+vphaddwq \(%esi\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 c7 00[ 	]+vphaddwq \(%eax\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 e1 ff[ 	]+vphsubbw %xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 e1 f0[ 	]+vphsubbw %xmm0,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 e1 03[ 	]+vphsubbw \(%ebx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 e1 3e[ 	]+vphsubbw \(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 e1 c0[ 	]+vphsubbw %xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 e1 38[ 	]+vphsubbw \(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 e1 c7[ 	]+vphsubbw %xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 e1 f1[ 	]+vphsubbw %xmm1,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 e1 c1[ 	]+vphsubbw %xmm1,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 e1 f8[ 	]+vphsubbw %xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 e1 30[ 	]+vphsubbw \(%eax\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 e1 f9[ 	]+vphsubbw %xmm1,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 e1 06[ 	]+vphsubbw \(%esi\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 e1 3b[ 	]+vphsubbw \(%ebx\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 e1 36[ 	]+vphsubbw \(%esi\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 e1 00[ 	]+vphsubbw \(%eax\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 e3 ff[ 	]+vphsubdq %xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 e3 f0[ 	]+vphsubdq %xmm0,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 e3 03[ 	]+vphsubdq \(%ebx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 e3 3e[ 	]+vphsubdq \(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 e3 c0[ 	]+vphsubdq %xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 e3 38[ 	]+vphsubdq \(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 e3 c7[ 	]+vphsubdq %xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 e3 f1[ 	]+vphsubdq %xmm1,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 e3 c1[ 	]+vphsubdq %xmm1,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 e3 f8[ 	]+vphsubdq %xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 e3 30[ 	]+vphsubdq \(%eax\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 e3 f9[ 	]+vphsubdq %xmm1,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 e3 06[ 	]+vphsubdq \(%esi\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 e3 3b[ 	]+vphsubdq \(%ebx\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 e3 36[ 	]+vphsubdq \(%esi\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 e3 00[ 	]+vphsubdq \(%eax\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 e2 ff[ 	]+vphsubwd %xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 e2 f0[ 	]+vphsubwd %xmm0,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 e2 03[ 	]+vphsubwd \(%ebx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 e2 3e[ 	]+vphsubwd \(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 e2 c0[ 	]+vphsubwd %xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 e2 38[ 	]+vphsubwd \(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 e2 c7[ 	]+vphsubwd %xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 e2 f1[ 	]+vphsubwd %xmm1,%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 e2 c1[ 	]+vphsubwd %xmm1,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 e2 f8[ 	]+vphsubwd %xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 e2 30[ 	]+vphsubwd \(%eax\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 e2 f9[ 	]+vphsubwd %xmm1,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 e2 06[ 	]+vphsubwd \(%esi\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 e2 3b[ 	]+vphsubwd \(%ebx\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 e2 36[ 	]+vphsubwd \(%esi\),%xmm6
[ 	]*[a-f0-9]+:	8f e9 78 e2 00[ 	]+vphsubwd \(%eax\),%xmm0
[ 	]*[a-f0-9]+:	8f e8 40 9e c7 00[ 	]+vpmacsdd %xmm0,%xmm7,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 9e 06 70[ 	]+vpmacsdd %xmm7,\(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 9e 06 10[ 	]+vpmacsdd %xmm1,\(%esi\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 9e e8 10[ 	]+vpmacsdd %xmm1,%xmm0,%xmm0,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 9e c6 10[ 	]+vpmacsdd %xmm1,%xmm6,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 9e fe 10[ 	]+vpmacsdd %xmm1,%xmm6,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 9e 3a 10[ 	]+vpmacsdd %xmm1,\(%edx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 9e f8 70[ 	]+vpmacsdd %xmm7,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 9e 3e 70[ 	]+vpmacsdd %xmm7,\(%esi\),%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 9e fe 70[ 	]+vpmacsdd %xmm7,%xmm6,%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 9e c7 70[ 	]+vpmacsdd %xmm7,%xmm7,%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 9e 02 00[ 	]+vpmacsdd %xmm0,\(%edx\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 40 9e 2a 10[ 	]+vpmacsdd %xmm1,\(%edx\),%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 9e ef 10[ 	]+vpmacsdd %xmm1,%xmm7,%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 9e c7 10[ 	]+vpmacsdd %xmm1,%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 9e 2e 70[ 	]+vpmacsdd %xmm7,\(%esi\),%xmm3,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 9f c7 00[ 	]+vpmacsdqh %xmm0,%xmm7,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 9f 06 70[ 	]+vpmacsdqh %xmm7,\(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 9f 06 10[ 	]+vpmacsdqh %xmm1,\(%esi\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 9f e8 10[ 	]+vpmacsdqh %xmm1,%xmm0,%xmm0,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 9f c6 10[ 	]+vpmacsdqh %xmm1,%xmm6,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 9f fe 10[ 	]+vpmacsdqh %xmm1,%xmm6,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 9f 3a 10[ 	]+vpmacsdqh %xmm1,\(%edx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 9f f8 70[ 	]+vpmacsdqh %xmm7,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 9f 3e 70[ 	]+vpmacsdqh %xmm7,\(%esi\),%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 9f fe 70[ 	]+vpmacsdqh %xmm7,%xmm6,%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 9f c7 70[ 	]+vpmacsdqh %xmm7,%xmm7,%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 9f 02 00[ 	]+vpmacsdqh %xmm0,\(%edx\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 40 9f 2a 10[ 	]+vpmacsdqh %xmm1,\(%edx\),%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 9f ef 10[ 	]+vpmacsdqh %xmm1,%xmm7,%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 9f c7 10[ 	]+vpmacsdqh %xmm1,%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 9f 2e 70[ 	]+vpmacsdqh %xmm7,\(%esi\),%xmm3,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 97 c7 00[ 	]+vpmacsdql %xmm0,%xmm7,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 97 06 70[ 	]+vpmacsdql %xmm7,\(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 97 06 10[ 	]+vpmacsdql %xmm1,\(%esi\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 97 e8 10[ 	]+vpmacsdql %xmm1,%xmm0,%xmm0,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 97 c6 10[ 	]+vpmacsdql %xmm1,%xmm6,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 97 fe 10[ 	]+vpmacsdql %xmm1,%xmm6,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 97 3a 10[ 	]+vpmacsdql %xmm1,\(%edx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 97 f8 70[ 	]+vpmacsdql %xmm7,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 97 3e 70[ 	]+vpmacsdql %xmm7,\(%esi\),%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 97 fe 70[ 	]+vpmacsdql %xmm7,%xmm6,%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 97 c7 70[ 	]+vpmacsdql %xmm7,%xmm7,%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 97 02 00[ 	]+vpmacsdql %xmm0,\(%edx\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 40 97 2a 10[ 	]+vpmacsdql %xmm1,\(%edx\),%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 97 ef 10[ 	]+vpmacsdql %xmm1,%xmm7,%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 97 c7 10[ 	]+vpmacsdql %xmm1,%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 97 2e 70[ 	]+vpmacsdql %xmm7,\(%esi\),%xmm3,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 8e c7 00[ 	]+vpmacssdd %xmm0,%xmm7,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 8e 06 70[ 	]+vpmacssdd %xmm7,\(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 8e 06 10[ 	]+vpmacssdd %xmm1,\(%esi\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 8e e8 10[ 	]+vpmacssdd %xmm1,%xmm0,%xmm0,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 8e c6 10[ 	]+vpmacssdd %xmm1,%xmm6,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 8e fe 10[ 	]+vpmacssdd %xmm1,%xmm6,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 8e 3a 10[ 	]+vpmacssdd %xmm1,\(%edx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 8e f8 70[ 	]+vpmacssdd %xmm7,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 8e 3e 70[ 	]+vpmacssdd %xmm7,\(%esi\),%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 8e fe 70[ 	]+vpmacssdd %xmm7,%xmm6,%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 8e c7 70[ 	]+vpmacssdd %xmm7,%xmm7,%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 8e 02 00[ 	]+vpmacssdd %xmm0,\(%edx\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 40 8e 2a 10[ 	]+vpmacssdd %xmm1,\(%edx\),%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 8e ef 10[ 	]+vpmacssdd %xmm1,%xmm7,%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 8e c7 10[ 	]+vpmacssdd %xmm1,%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 8e 2e 70[ 	]+vpmacssdd %xmm7,\(%esi\),%xmm3,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 8f c7 00[ 	]+vpmacssdqh %xmm0,%xmm7,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 8f 06 70[ 	]+vpmacssdqh %xmm7,\(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 8f 06 10[ 	]+vpmacssdqh %xmm1,\(%esi\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 8f e8 10[ 	]+vpmacssdqh %xmm1,%xmm0,%xmm0,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 8f c6 10[ 	]+vpmacssdqh %xmm1,%xmm6,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 8f fe 10[ 	]+vpmacssdqh %xmm1,%xmm6,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 8f 3a 10[ 	]+vpmacssdqh %xmm1,\(%edx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 8f f8 70[ 	]+vpmacssdqh %xmm7,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 8f 3e 70[ 	]+vpmacssdqh %xmm7,\(%esi\),%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 8f fe 70[ 	]+vpmacssdqh %xmm7,%xmm6,%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 8f c7 70[ 	]+vpmacssdqh %xmm7,%xmm7,%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 8f 02 00[ 	]+vpmacssdqh %xmm0,\(%edx\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 40 8f 2a 10[ 	]+vpmacssdqh %xmm1,\(%edx\),%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 8f ef 10[ 	]+vpmacssdqh %xmm1,%xmm7,%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 8f c7 10[ 	]+vpmacssdqh %xmm1,%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 8f 2e 70[ 	]+vpmacssdqh %xmm7,\(%esi\),%xmm3,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 87 c7 00[ 	]+vpmacssdql %xmm0,%xmm7,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 87 06 70[ 	]+vpmacssdql %xmm7,\(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 87 06 10[ 	]+vpmacssdql %xmm1,\(%esi\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 87 e8 10[ 	]+vpmacssdql %xmm1,%xmm0,%xmm0,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 87 c6 10[ 	]+vpmacssdql %xmm1,%xmm6,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 87 fe 10[ 	]+vpmacssdql %xmm1,%xmm6,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 87 3a 10[ 	]+vpmacssdql %xmm1,\(%edx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 87 f8 70[ 	]+vpmacssdql %xmm7,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 87 3e 70[ 	]+vpmacssdql %xmm7,\(%esi\),%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 87 fe 70[ 	]+vpmacssdql %xmm7,%xmm6,%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 87 c7 70[ 	]+vpmacssdql %xmm7,%xmm7,%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 87 02 00[ 	]+vpmacssdql %xmm0,\(%edx\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 40 87 2a 10[ 	]+vpmacssdql %xmm1,\(%edx\),%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 87 ef 10[ 	]+vpmacssdql %xmm1,%xmm7,%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 87 c7 10[ 	]+vpmacssdql %xmm1,%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 87 2e 70[ 	]+vpmacssdql %xmm7,\(%esi\),%xmm3,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 86 c7 00[ 	]+vpmacsswd %xmm0,%xmm7,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 86 06 70[ 	]+vpmacsswd %xmm7,\(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 86 06 10[ 	]+vpmacsswd %xmm1,\(%esi\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 86 e8 10[ 	]+vpmacsswd %xmm1,%xmm0,%xmm0,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 86 c6 10[ 	]+vpmacsswd %xmm1,%xmm6,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 86 fe 10[ 	]+vpmacsswd %xmm1,%xmm6,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 86 3a 10[ 	]+vpmacsswd %xmm1,\(%edx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 86 f8 70[ 	]+vpmacsswd %xmm7,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 86 3e 70[ 	]+vpmacsswd %xmm7,\(%esi\),%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 86 fe 70[ 	]+vpmacsswd %xmm7,%xmm6,%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 86 c7 70[ 	]+vpmacsswd %xmm7,%xmm7,%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 86 02 00[ 	]+vpmacsswd %xmm0,\(%edx\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 40 86 2a 10[ 	]+vpmacsswd %xmm1,\(%edx\),%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 86 ef 10[ 	]+vpmacsswd %xmm1,%xmm7,%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 86 c7 10[ 	]+vpmacsswd %xmm1,%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 86 2e 70[ 	]+vpmacsswd %xmm7,\(%esi\),%xmm3,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 85 c7 00[ 	]+vpmacssww %xmm0,%xmm7,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 85 06 70[ 	]+vpmacssww %xmm7,\(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 85 06 10[ 	]+vpmacssww %xmm1,\(%esi\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 85 e8 10[ 	]+vpmacssww %xmm1,%xmm0,%xmm0,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 85 c6 10[ 	]+vpmacssww %xmm1,%xmm6,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 85 fe 10[ 	]+vpmacssww %xmm1,%xmm6,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 85 3a 10[ 	]+vpmacssww %xmm1,\(%edx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 85 f8 70[ 	]+vpmacssww %xmm7,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 85 3e 70[ 	]+vpmacssww %xmm7,\(%esi\),%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 85 fe 70[ 	]+vpmacssww %xmm7,%xmm6,%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 85 c7 70[ 	]+vpmacssww %xmm7,%xmm7,%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 85 02 00[ 	]+vpmacssww %xmm0,\(%edx\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 40 85 2a 10[ 	]+vpmacssww %xmm1,\(%edx\),%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 85 ef 10[ 	]+vpmacssww %xmm1,%xmm7,%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 85 c7 10[ 	]+vpmacssww %xmm1,%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 85 2e 70[ 	]+vpmacssww %xmm7,\(%esi\),%xmm3,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 96 c7 00[ 	]+vpmacswd %xmm0,%xmm7,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 96 06 70[ 	]+vpmacswd %xmm7,\(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 96 06 10[ 	]+vpmacswd %xmm1,\(%esi\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 96 e8 10[ 	]+vpmacswd %xmm1,%xmm0,%xmm0,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 96 c6 10[ 	]+vpmacswd %xmm1,%xmm6,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 96 fe 10[ 	]+vpmacswd %xmm1,%xmm6,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 96 3a 10[ 	]+vpmacswd %xmm1,\(%edx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 96 f8 70[ 	]+vpmacswd %xmm7,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 96 3e 70[ 	]+vpmacswd %xmm7,\(%esi\),%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 96 fe 70[ 	]+vpmacswd %xmm7,%xmm6,%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 96 c7 70[ 	]+vpmacswd %xmm7,%xmm7,%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 96 02 00[ 	]+vpmacswd %xmm0,\(%edx\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 40 96 2a 10[ 	]+vpmacswd %xmm1,\(%edx\),%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 96 ef 10[ 	]+vpmacswd %xmm1,%xmm7,%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 96 c7 10[ 	]+vpmacswd %xmm1,%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 96 2e 70[ 	]+vpmacswd %xmm7,\(%esi\),%xmm3,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 95 c7 00[ 	]+vpmacsww %xmm0,%xmm7,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 95 06 70[ 	]+vpmacsww %xmm7,\(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 95 06 10[ 	]+vpmacsww %xmm1,\(%esi\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 95 e8 10[ 	]+vpmacsww %xmm1,%xmm0,%xmm0,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 95 c6 10[ 	]+vpmacsww %xmm1,%xmm6,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 95 fe 10[ 	]+vpmacsww %xmm1,%xmm6,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 95 3a 10[ 	]+vpmacsww %xmm1,\(%edx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 95 f8 70[ 	]+vpmacsww %xmm7,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 95 3e 70[ 	]+vpmacsww %xmm7,\(%esi\),%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 95 fe 70[ 	]+vpmacsww %xmm7,%xmm6,%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 95 c7 70[ 	]+vpmacsww %xmm7,%xmm7,%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 95 02 00[ 	]+vpmacsww %xmm0,\(%edx\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 40 95 2a 10[ 	]+vpmacsww %xmm1,\(%edx\),%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 95 ef 10[ 	]+vpmacsww %xmm1,%xmm7,%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 95 c7 10[ 	]+vpmacsww %xmm1,%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 95 2e 70[ 	]+vpmacsww %xmm7,\(%esi\),%xmm3,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 a6 c7 00[ 	]+vpmadcsswd %xmm0,%xmm7,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 a6 06 70[ 	]+vpmadcsswd %xmm7,\(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 a6 06 10[ 	]+vpmadcsswd %xmm1,\(%esi\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 a6 e8 10[ 	]+vpmadcsswd %xmm1,%xmm0,%xmm0,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 a6 c6 10[ 	]+vpmadcsswd %xmm1,%xmm6,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 a6 fe 10[ 	]+vpmadcsswd %xmm1,%xmm6,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 a6 3a 10[ 	]+vpmadcsswd %xmm1,\(%edx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 a6 f8 70[ 	]+vpmadcsswd %xmm7,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 a6 3e 70[ 	]+vpmadcsswd %xmm7,\(%esi\),%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 a6 fe 70[ 	]+vpmadcsswd %xmm7,%xmm6,%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 a6 c7 70[ 	]+vpmadcsswd %xmm7,%xmm7,%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 a6 02 00[ 	]+vpmadcsswd %xmm0,\(%edx\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 40 a6 2a 10[ 	]+vpmadcsswd %xmm1,\(%edx\),%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 a6 ef 10[ 	]+vpmadcsswd %xmm1,%xmm7,%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 a6 c7 10[ 	]+vpmadcsswd %xmm1,%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 a6 2e 70[ 	]+vpmadcsswd %xmm7,\(%esi\),%xmm3,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 b6 c7 00[ 	]+vpmadcswd %xmm0,%xmm7,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 b6 06 70[ 	]+vpmadcswd %xmm7,\(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 b6 06 10[ 	]+vpmadcswd %xmm1,\(%esi\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 b6 e8 10[ 	]+vpmadcswd %xmm1,%xmm0,%xmm0,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 b6 c6 10[ 	]+vpmadcswd %xmm1,%xmm6,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 b6 fe 10[ 	]+vpmadcswd %xmm1,%xmm6,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 b6 3a 10[ 	]+vpmadcswd %xmm1,\(%edx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 b6 f8 70[ 	]+vpmadcswd %xmm7,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 b6 3e 70[ 	]+vpmadcswd %xmm7,\(%esi\),%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 b6 fe 70[ 	]+vpmadcswd %xmm7,%xmm6,%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 b6 c7 70[ 	]+vpmadcswd %xmm7,%xmm7,%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 b6 02 00[ 	]+vpmadcswd %xmm0,\(%edx\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 40 b6 2a 10[ 	]+vpmadcswd %xmm1,\(%edx\),%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 b6 ef 10[ 	]+vpmadcswd %xmm1,%xmm7,%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 b6 c7 10[ 	]+vpmadcswd %xmm1,%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 b6 2e 70[ 	]+vpmadcswd %xmm7,\(%esi\),%xmm3,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 a3 c6 00[ 	]+vpperm %xmm0,%xmm6,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 f8 a3 06 70[ 	]+vpperm \(%esi\),%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 e0 a3 00 70[ 	]+vpperm \(%eax\),%xmm7,%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 a3 e8 70[ 	]+vpperm %xmm7,%xmm0,%xmm0,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 a3 c0 70[ 	]+vpperm %xmm7,%xmm0,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 a3 f8 70[ 	]+vpperm %xmm7,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 f8 a3 38 60[ 	]+vpperm \(%eax\),%xmm6,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 f8 a3 3e 00[ 	]+vpperm \(%esi\),%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 e0 a3 3b 70[ 	]+vpperm \(%ebx\),%xmm7,%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 e0 a3 3b 00[ 	]+vpperm \(%ebx\),%xmm0,%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 e0 a3 06 60[ 	]+vpperm \(%esi\),%xmm6,%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 a3 c7 10[ 	]+vpperm %xmm1,%xmm7,%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 c0 a3 28 70[ 	]+vpperm \(%eax\),%xmm7,%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 a3 ee 70[ 	]+vpperm %xmm7,%xmm6,%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 a3 c6 70[ 	]+vpperm %xmm7,%xmm6,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 e0 a3 2b 70[ 	]+vpperm \(%ebx\),%xmm7,%xmm3,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 a3 c7 00[ 	]+vpperm %xmm0,%xmm7,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 a3 06 70[ 	]+vpperm %xmm7,\(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 a3 06 10[ 	]+vpperm %xmm1,\(%esi\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 a3 e8 10[ 	]+vpperm %xmm1,%xmm0,%xmm0,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 a3 c6 10[ 	]+vpperm %xmm1,%xmm6,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 a3 fe 10[ 	]+vpperm %xmm1,%xmm6,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 a3 3a 10[ 	]+vpperm %xmm1,\(%edx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 a3 f8 70[ 	]+vpperm %xmm7,%xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 a3 3e 70[ 	]+vpperm %xmm7,\(%esi\),%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 a3 fe 70[ 	]+vpperm %xmm7,%xmm6,%xmm3,%xmm7
[ 	]*[a-f0-9]+:	8f e8 60 a3 c7 70[ 	]+vpperm %xmm7,%xmm7,%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 a3 02 00[ 	]+vpperm %xmm0,\(%edx\),%xmm3,%xmm0
[ 	]*[a-f0-9]+:	8f e8 40 a3 2a 10[ 	]+vpperm %xmm1,\(%edx\),%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 40 a3 ef 10[ 	]+vpperm %xmm1,%xmm7,%xmm7,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 a3 c7 10[ 	]+vpperm %xmm1,%xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 60 a3 2e 70[ 	]+vpperm %xmm7,\(%esi\),%xmm3,%xmm5
[ 	]*[a-f0-9]+:	8f e9 40 90 d8[ 	]+vprotb %xmm7,%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 90 fe[ 	]+vprotb %xmm7,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 90 c0[ 	]+vprotb %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 90 1e[ 	]+vprotb %xmm1,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 90 c7[ 	]+vprotb %xmm0,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 90 df[ 	]+vprotb %xmm0,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 90 c6[ 	]+vprotb %xmm0,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 90 c6[ 	]+vprotb %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 90 df[ 	]+vprotb %xmm7,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 90 3e[ 	]+vprotb %xmm0,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 90 fe[ 	]+vprotb %xmm0,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 90 1e[ 	]+vprotb %xmm7,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 90 02[ 	]+vprotb %xmm7,\(%edx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 90 3e[ 	]+vprotb %xmm1,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 70 90 c7[ 	]+vprotb %xmm1,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 90 1a[ 	]+vprotb %xmm0,\(%edx\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 90 1b[ 	]+vprotb \(%ebx\),%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 90 3b[ 	]+vprotb \(%ebx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 f8 90 06[ 	]+vprotb \(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 90 18[ 	]+vprotb \(%eax\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 90 c6[ 	]+vprotb %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 90 de[ 	]+vprotb %xmm1,%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 90 c0[ 	]+vprotb %xmm1,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 90 c0[ 	]+vprotb %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c8 90 1e[ 	]+vprotb \(%esi\),%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 90 ff[ 	]+vprotb %xmm1,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 90 f8[ 	]+vprotb %xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c0 90 1b[ 	]+vprotb \(%ebx\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 c0 90 03[ 	]+vprotb \(%ebx\),%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 90 38[ 	]+vprotb \(%eax\),%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c8 90 00[ 	]+vprotb \(%eax\),%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 90 df[ 	]+vprotb %xmm1,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e8 78 c0 d5 03[ 	]+vprotb \$0x3,%xmm5,%xmm2
[ 	]*[a-f0-9]+:	8f e8 78 c0 c0 ff[ 	]+vprotb \$0xff,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 c0 fd ff[ 	]+vprotb \$0xff,%xmm5,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 c0 fd 00[ 	]+vprotb \$0x0,%xmm5,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 c0 ff 00[ 	]+vprotb \$0x0,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 c0 d0 00[ 	]+vprotb \$0x0,%xmm0,%xmm2
[ 	]*[a-f0-9]+:	8f e8 78 c0 c5 ff[ 	]+vprotb \$0xff,%xmm5,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 c0 c0 03[ 	]+vprotb \$0x3,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 c0 c5 03[ 	]+vprotb \$0x3,%xmm5,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 c0 f8 00[ 	]+vprotb \$0x0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 c0 c7 ff[ 	]+vprotb \$0xff,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 c0 d0 ff[ 	]+vprotb \$0xff,%xmm0,%xmm2
[ 	]*[a-f0-9]+:	8f e8 78 c0 d7 ff[ 	]+vprotb \$0xff,%xmm7,%xmm2
[ 	]*[a-f0-9]+:	8f e8 78 c0 ff 03[ 	]+vprotb \$0x3,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 c0 d5 ff[ 	]+vprotb \$0xff,%xmm5,%xmm2
[ 	]*[a-f0-9]+:	8f e8 78 c0 d0 03[ 	]+vprotb \$0x3,%xmm0,%xmm2
[ 	]*[a-f0-9]+:	8f e9 40 92 d8[ 	]+vprotd %xmm7,%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 92 fe[ 	]+vprotd %xmm7,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 92 c0[ 	]+vprotd %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 92 1e[ 	]+vprotd %xmm1,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 92 c7[ 	]+vprotd %xmm0,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 92 df[ 	]+vprotd %xmm0,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 92 c6[ 	]+vprotd %xmm0,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 92 c6[ 	]+vprotd %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 92 df[ 	]+vprotd %xmm7,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 92 3e[ 	]+vprotd %xmm0,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 92 fe[ 	]+vprotd %xmm0,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 92 1e[ 	]+vprotd %xmm7,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 92 02[ 	]+vprotd %xmm7,\(%edx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 92 3e[ 	]+vprotd %xmm1,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 70 92 c7[ 	]+vprotd %xmm1,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 92 1a[ 	]+vprotd %xmm0,\(%edx\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 92 1b[ 	]+vprotd \(%ebx\),%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 92 3b[ 	]+vprotd \(%ebx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 f8 92 06[ 	]+vprotd \(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 92 18[ 	]+vprotd \(%eax\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 92 c6[ 	]+vprotd %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 92 de[ 	]+vprotd %xmm1,%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 92 c0[ 	]+vprotd %xmm1,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 92 c0[ 	]+vprotd %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c8 92 1e[ 	]+vprotd \(%esi\),%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 92 ff[ 	]+vprotd %xmm1,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 92 f8[ 	]+vprotd %xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c0 92 1b[ 	]+vprotd \(%ebx\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 c0 92 03[ 	]+vprotd \(%ebx\),%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 92 38[ 	]+vprotd \(%eax\),%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c8 92 00[ 	]+vprotd \(%eax\),%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 92 df[ 	]+vprotd %xmm1,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e8 78 c2 ff 00[ 	]+vprotd \$0x0,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 c2 3b 00[ 	]+vprotd \$0x0,\(%ebx\),%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 c2 e8 00[ 	]+vprotd \$0x0,%xmm0,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 c2 c5 ff[ 	]+vprotd \$0xff,%xmm5,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 c2 c0 03[ 	]+vprotd \$0x3,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 c2 c7 03[ 	]+vprotd \$0x3,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 c2 ed 00[ 	]+vprotd \$0x0,%xmm5,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 c2 f8 00[ 	]+vprotd \$0x0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 c2 00 03[ 	]+vprotd \$0x3,\(%eax\),%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 c2 03 ff[ 	]+vprotd \$0xff,\(%ebx\),%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 c2 38 00[ 	]+vprotd \$0x0,\(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 c2 ff ff[ 	]+vprotd \$0xff,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 c2 ed ff[ 	]+vprotd \$0xff,%xmm5,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 c2 2b ff[ 	]+vprotd \$0xff,\(%ebx\),%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 c2 c7 ff[ 	]+vprotd \$0xff,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 c2 38 03[ 	]+vprotd \$0x3,\(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 93 d8[ 	]+vprotq %xmm7,%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 93 fe[ 	]+vprotq %xmm7,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 93 c0[ 	]+vprotq %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 93 1e[ 	]+vprotq %xmm1,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 93 c7[ 	]+vprotq %xmm0,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 93 df[ 	]+vprotq %xmm0,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 93 c6[ 	]+vprotq %xmm0,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 93 c6[ 	]+vprotq %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 93 df[ 	]+vprotq %xmm7,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 93 3e[ 	]+vprotq %xmm0,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 93 fe[ 	]+vprotq %xmm0,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 93 1e[ 	]+vprotq %xmm7,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 93 02[ 	]+vprotq %xmm7,\(%edx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 93 3e[ 	]+vprotq %xmm1,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 70 93 c7[ 	]+vprotq %xmm1,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 93 1a[ 	]+vprotq %xmm0,\(%edx\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 93 1b[ 	]+vprotq \(%ebx\),%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 93 3b[ 	]+vprotq \(%ebx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 f8 93 06[ 	]+vprotq \(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 93 18[ 	]+vprotq \(%eax\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 93 c6[ 	]+vprotq %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 93 de[ 	]+vprotq %xmm1,%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 93 c0[ 	]+vprotq %xmm1,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 93 c0[ 	]+vprotq %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c8 93 1e[ 	]+vprotq \(%esi\),%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 93 ff[ 	]+vprotq %xmm1,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 93 f8[ 	]+vprotq %xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c0 93 1b[ 	]+vprotq \(%ebx\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 c0 93 03[ 	]+vprotq \(%ebx\),%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 93 38[ 	]+vprotq \(%eax\),%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c8 93 00[ 	]+vprotq \(%eax\),%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 93 df[ 	]+vprotq %xmm1,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e8 78 c3 ff 00[ 	]+vprotq \$0x0,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 c3 3b 00[ 	]+vprotq \$0x0,\(%ebx\),%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 c3 e8 00[ 	]+vprotq \$0x0,%xmm0,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 c3 c5 ff[ 	]+vprotq \$0xff,%xmm5,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 c3 c0 03[ 	]+vprotq \$0x3,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 c3 c7 03[ 	]+vprotq \$0x3,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 c3 ed 00[ 	]+vprotq \$0x0,%xmm5,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 c3 f8 00[ 	]+vprotq \$0x0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 c3 00 03[ 	]+vprotq \$0x3,\(%eax\),%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 c3 03 ff[ 	]+vprotq \$0xff,\(%ebx\),%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 c3 38 00[ 	]+vprotq \$0x0,\(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 c3 ff ff[ 	]+vprotq \$0xff,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 c3 ed ff[ 	]+vprotq \$0xff,%xmm5,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 c3 2b ff[ 	]+vprotq \$0xff,\(%ebx\),%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 c3 c7 ff[ 	]+vprotq \$0xff,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 c3 38 03[ 	]+vprotq \$0x3,\(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 91 d8[ 	]+vprotw %xmm7,%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 91 fe[ 	]+vprotw %xmm7,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 91 c0[ 	]+vprotw %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 91 1e[ 	]+vprotw %xmm1,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 91 c7[ 	]+vprotw %xmm0,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 91 df[ 	]+vprotw %xmm0,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 91 c6[ 	]+vprotw %xmm0,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 91 c6[ 	]+vprotw %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 91 df[ 	]+vprotw %xmm7,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 91 3e[ 	]+vprotw %xmm0,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 91 fe[ 	]+vprotw %xmm0,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 91 1e[ 	]+vprotw %xmm7,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 91 02[ 	]+vprotw %xmm7,\(%edx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 91 3e[ 	]+vprotw %xmm1,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 70 91 c7[ 	]+vprotw %xmm1,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 91 1a[ 	]+vprotw %xmm0,\(%edx\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 91 1b[ 	]+vprotw \(%ebx\),%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 91 3b[ 	]+vprotw \(%ebx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 f8 91 06[ 	]+vprotw \(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 91 18[ 	]+vprotw \(%eax\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 91 c6[ 	]+vprotw %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 91 de[ 	]+vprotw %xmm1,%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 91 c0[ 	]+vprotw %xmm1,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 91 c0[ 	]+vprotw %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c8 91 1e[ 	]+vprotw \(%esi\),%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 91 ff[ 	]+vprotw %xmm1,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 91 f8[ 	]+vprotw %xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c0 91 1b[ 	]+vprotw \(%ebx\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 c0 91 03[ 	]+vprotw \(%ebx\),%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 91 38[ 	]+vprotw \(%eax\),%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c8 91 00[ 	]+vprotw \(%eax\),%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 91 df[ 	]+vprotw %xmm1,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e8 78 c1 ff 00[ 	]+vprotw \$0x0,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 c1 3b 00[ 	]+vprotw \$0x0,\(%ebx\),%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 c1 e8 00[ 	]+vprotw \$0x0,%xmm0,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 c1 c5 ff[ 	]+vprotw \$0xff,%xmm5,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 c1 c0 03[ 	]+vprotw \$0x3,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 c1 c7 03[ 	]+vprotw \$0x3,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 c1 ed 00[ 	]+vprotw \$0x0,%xmm5,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 c1 f8 00[ 	]+vprotw \$0x0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 c1 00 03[ 	]+vprotw \$0x3,\(%eax\),%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 c1 03 ff[ 	]+vprotw \$0xff,\(%ebx\),%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 c1 38 00[ 	]+vprotw \$0x0,\(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 c1 ff ff[ 	]+vprotw \$0xff,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e8 78 c1 ed ff[ 	]+vprotw \$0xff,%xmm5,%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 c1 2b ff[ 	]+vprotw \$0xff,\(%ebx\),%xmm5
[ 	]*[a-f0-9]+:	8f e8 78 c1 c7 ff[ 	]+vprotw \$0xff,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e8 78 c1 38 03[ 	]+vprotw \$0x3,\(%eax\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 98 d8[ 	]+vpshab %xmm7,%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 98 fe[ 	]+vpshab %xmm7,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 98 c0[ 	]+vpshab %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 98 1e[ 	]+vpshab %xmm1,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 98 c7[ 	]+vpshab %xmm0,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 98 df[ 	]+vpshab %xmm0,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 98 c6[ 	]+vpshab %xmm0,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 98 c6[ 	]+vpshab %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 98 df[ 	]+vpshab %xmm7,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 98 3e[ 	]+vpshab %xmm0,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 98 fe[ 	]+vpshab %xmm0,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 98 1e[ 	]+vpshab %xmm7,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 98 02[ 	]+vpshab %xmm7,\(%edx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 98 3e[ 	]+vpshab %xmm1,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 70 98 c7[ 	]+vpshab %xmm1,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 98 1a[ 	]+vpshab %xmm0,\(%edx\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 98 1b[ 	]+vpshab \(%ebx\),%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 98 3b[ 	]+vpshab \(%ebx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 f8 98 06[ 	]+vpshab \(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 98 18[ 	]+vpshab \(%eax\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 98 c6[ 	]+vpshab %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 98 de[ 	]+vpshab %xmm1,%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 98 c0[ 	]+vpshab %xmm1,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 98 c0[ 	]+vpshab %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c8 98 1e[ 	]+vpshab \(%esi\),%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 98 ff[ 	]+vpshab %xmm1,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 98 f8[ 	]+vpshab %xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c0 98 1b[ 	]+vpshab \(%ebx\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 c0 98 03[ 	]+vpshab \(%ebx\),%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 98 38[ 	]+vpshab \(%eax\),%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c8 98 00[ 	]+vpshab \(%eax\),%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 98 df[ 	]+vpshab %xmm1,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 9a d8[ 	]+vpshad %xmm7,%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 9a fe[ 	]+vpshad %xmm7,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 9a c0[ 	]+vpshad %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 9a 1e[ 	]+vpshad %xmm1,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 9a c7[ 	]+vpshad %xmm0,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 9a df[ 	]+vpshad %xmm0,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 9a c6[ 	]+vpshad %xmm0,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 9a c6[ 	]+vpshad %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 9a df[ 	]+vpshad %xmm7,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 9a 3e[ 	]+vpshad %xmm0,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 9a fe[ 	]+vpshad %xmm0,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 9a 1e[ 	]+vpshad %xmm7,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 9a 02[ 	]+vpshad %xmm7,\(%edx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 9a 3e[ 	]+vpshad %xmm1,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 70 9a c7[ 	]+vpshad %xmm1,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 9a 1a[ 	]+vpshad %xmm0,\(%edx\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 9a 1b[ 	]+vpshad \(%ebx\),%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 9a 3b[ 	]+vpshad \(%ebx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 f8 9a 06[ 	]+vpshad \(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 9a 18[ 	]+vpshad \(%eax\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 9a c6[ 	]+vpshad %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 9a de[ 	]+vpshad %xmm1,%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 9a c0[ 	]+vpshad %xmm1,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 9a c0[ 	]+vpshad %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c8 9a 1e[ 	]+vpshad \(%esi\),%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 9a ff[ 	]+vpshad %xmm1,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 9a f8[ 	]+vpshad %xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c0 9a 1b[ 	]+vpshad \(%ebx\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 c0 9a 03[ 	]+vpshad \(%ebx\),%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 9a 38[ 	]+vpshad \(%eax\),%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c8 9a 00[ 	]+vpshad \(%eax\),%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 9a df[ 	]+vpshad %xmm1,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 9b d8[ 	]+vpshaq %xmm7,%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 9b fe[ 	]+vpshaq %xmm7,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 9b c0[ 	]+vpshaq %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 9b 1e[ 	]+vpshaq %xmm1,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 9b c7[ 	]+vpshaq %xmm0,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 9b df[ 	]+vpshaq %xmm0,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 9b c6[ 	]+vpshaq %xmm0,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 9b c6[ 	]+vpshaq %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 9b df[ 	]+vpshaq %xmm7,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 9b 3e[ 	]+vpshaq %xmm0,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 9b fe[ 	]+vpshaq %xmm0,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 9b 1e[ 	]+vpshaq %xmm7,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 9b 02[ 	]+vpshaq %xmm7,\(%edx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 9b 3e[ 	]+vpshaq %xmm1,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 70 9b c7[ 	]+vpshaq %xmm1,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 9b 1a[ 	]+vpshaq %xmm0,\(%edx\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 9b 1b[ 	]+vpshaq \(%ebx\),%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 9b 3b[ 	]+vpshaq \(%ebx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 f8 9b 06[ 	]+vpshaq \(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 9b 18[ 	]+vpshaq \(%eax\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 9b c6[ 	]+vpshaq %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 9b de[ 	]+vpshaq %xmm1,%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 9b c0[ 	]+vpshaq %xmm1,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 9b c0[ 	]+vpshaq %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c8 9b 1e[ 	]+vpshaq \(%esi\),%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 9b ff[ 	]+vpshaq %xmm1,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 9b f8[ 	]+vpshaq %xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c0 9b 1b[ 	]+vpshaq \(%ebx\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 c0 9b 03[ 	]+vpshaq \(%ebx\),%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 9b 38[ 	]+vpshaq \(%eax\),%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c8 9b 00[ 	]+vpshaq \(%eax\),%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 9b df[ 	]+vpshaq %xmm1,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 99 d8[ 	]+vpshaw %xmm7,%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 99 fe[ 	]+vpshaw %xmm7,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 99 c0[ 	]+vpshaw %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 99 1e[ 	]+vpshaw %xmm1,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 99 c7[ 	]+vpshaw %xmm0,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 99 df[ 	]+vpshaw %xmm0,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 99 c6[ 	]+vpshaw %xmm0,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 99 c6[ 	]+vpshaw %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 99 df[ 	]+vpshaw %xmm7,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 99 3e[ 	]+vpshaw %xmm0,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 99 fe[ 	]+vpshaw %xmm0,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 99 1e[ 	]+vpshaw %xmm7,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 99 02[ 	]+vpshaw %xmm7,\(%edx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 99 3e[ 	]+vpshaw %xmm1,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 70 99 c7[ 	]+vpshaw %xmm1,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 99 1a[ 	]+vpshaw %xmm0,\(%edx\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 99 1b[ 	]+vpshaw \(%ebx\),%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 99 3b[ 	]+vpshaw \(%ebx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 f8 99 06[ 	]+vpshaw \(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 99 18[ 	]+vpshaw \(%eax\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 99 c6[ 	]+vpshaw %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 99 de[ 	]+vpshaw %xmm1,%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 99 c0[ 	]+vpshaw %xmm1,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 99 c0[ 	]+vpshaw %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c8 99 1e[ 	]+vpshaw \(%esi\),%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 99 ff[ 	]+vpshaw %xmm1,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 99 f8[ 	]+vpshaw %xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c0 99 1b[ 	]+vpshaw \(%ebx\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 c0 99 03[ 	]+vpshaw \(%ebx\),%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 99 38[ 	]+vpshaw \(%eax\),%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c8 99 00[ 	]+vpshaw \(%eax\),%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 99 df[ 	]+vpshaw %xmm1,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 94 d8[ 	]+vpshlb %xmm7,%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 94 fe[ 	]+vpshlb %xmm7,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 94 c0[ 	]+vpshlb %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 94 1e[ 	]+vpshlb %xmm1,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 94 c7[ 	]+vpshlb %xmm0,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 94 df[ 	]+vpshlb %xmm0,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 94 c6[ 	]+vpshlb %xmm0,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 94 c6[ 	]+vpshlb %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 94 df[ 	]+vpshlb %xmm7,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 94 3e[ 	]+vpshlb %xmm0,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 94 fe[ 	]+vpshlb %xmm0,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 94 1e[ 	]+vpshlb %xmm7,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 94 02[ 	]+vpshlb %xmm7,\(%edx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 94 3e[ 	]+vpshlb %xmm1,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 70 94 c7[ 	]+vpshlb %xmm1,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 94 1a[ 	]+vpshlb %xmm0,\(%edx\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 94 1b[ 	]+vpshlb \(%ebx\),%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 94 3b[ 	]+vpshlb \(%ebx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 f8 94 06[ 	]+vpshlb \(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 94 18[ 	]+vpshlb \(%eax\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 94 c6[ 	]+vpshlb %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 94 de[ 	]+vpshlb %xmm1,%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 94 c0[ 	]+vpshlb %xmm1,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 94 c0[ 	]+vpshlb %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c8 94 1e[ 	]+vpshlb \(%esi\),%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 94 ff[ 	]+vpshlb %xmm1,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 94 f8[ 	]+vpshlb %xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c0 94 1b[ 	]+vpshlb \(%ebx\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 c0 94 03[ 	]+vpshlb \(%ebx\),%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 94 38[ 	]+vpshlb \(%eax\),%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c8 94 00[ 	]+vpshlb \(%eax\),%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 94 df[ 	]+vpshlb %xmm1,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 96 d8[ 	]+vpshld %xmm7,%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 96 fe[ 	]+vpshld %xmm7,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 96 c0[ 	]+vpshld %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 96 1e[ 	]+vpshld %xmm1,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 96 c7[ 	]+vpshld %xmm0,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 96 df[ 	]+vpshld %xmm0,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 96 c6[ 	]+vpshld %xmm0,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 96 c6[ 	]+vpshld %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 96 df[ 	]+vpshld %xmm7,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 96 3e[ 	]+vpshld %xmm0,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 96 fe[ 	]+vpshld %xmm0,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 96 1e[ 	]+vpshld %xmm7,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 96 02[ 	]+vpshld %xmm7,\(%edx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 96 3e[ 	]+vpshld %xmm1,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 70 96 c7[ 	]+vpshld %xmm1,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 96 1a[ 	]+vpshld %xmm0,\(%edx\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 96 1b[ 	]+vpshld \(%ebx\),%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 96 3b[ 	]+vpshld \(%ebx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 f8 96 06[ 	]+vpshld \(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 96 18[ 	]+vpshld \(%eax\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 96 c6[ 	]+vpshld %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 96 de[ 	]+vpshld %xmm1,%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 96 c0[ 	]+vpshld %xmm1,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 96 c0[ 	]+vpshld %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c8 96 1e[ 	]+vpshld \(%esi\),%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 96 ff[ 	]+vpshld %xmm1,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 96 f8[ 	]+vpshld %xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c0 96 1b[ 	]+vpshld \(%ebx\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 c0 96 03[ 	]+vpshld \(%ebx\),%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 96 38[ 	]+vpshld \(%eax\),%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c8 96 00[ 	]+vpshld \(%eax\),%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 96 df[ 	]+vpshld %xmm1,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 97 d8[ 	]+vpshlq %xmm7,%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 97 fe[ 	]+vpshlq %xmm7,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 97 c0[ 	]+vpshlq %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 97 1e[ 	]+vpshlq %xmm1,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 97 c7[ 	]+vpshlq %xmm0,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 97 df[ 	]+vpshlq %xmm0,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 97 c6[ 	]+vpshlq %xmm0,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 97 c6[ 	]+vpshlq %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 97 df[ 	]+vpshlq %xmm7,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 97 3e[ 	]+vpshlq %xmm0,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 97 fe[ 	]+vpshlq %xmm0,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 97 1e[ 	]+vpshlq %xmm7,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 97 02[ 	]+vpshlq %xmm7,\(%edx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 97 3e[ 	]+vpshlq %xmm1,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 70 97 c7[ 	]+vpshlq %xmm1,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 97 1a[ 	]+vpshlq %xmm0,\(%edx\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 97 1b[ 	]+vpshlq \(%ebx\),%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 97 3b[ 	]+vpshlq \(%ebx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 f8 97 06[ 	]+vpshlq \(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 97 18[ 	]+vpshlq \(%eax\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 97 c6[ 	]+vpshlq %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 97 de[ 	]+vpshlq %xmm1,%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 97 c0[ 	]+vpshlq %xmm1,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 97 c0[ 	]+vpshlq %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c8 97 1e[ 	]+vpshlq \(%esi\),%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 97 ff[ 	]+vpshlq %xmm1,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 97 f8[ 	]+vpshlq %xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c0 97 1b[ 	]+vpshlq \(%ebx\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 c0 97 03[ 	]+vpshlq \(%ebx\),%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 97 38[ 	]+vpshlq \(%eax\),%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c8 97 00[ 	]+vpshlq \(%eax\),%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 97 df[ 	]+vpshlq %xmm1,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 95 d8[ 	]+vpshlw %xmm7,%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 95 fe[ 	]+vpshlw %xmm7,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 95 c0[ 	]+vpshlw %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 95 1e[ 	]+vpshlw %xmm1,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 95 c7[ 	]+vpshlw %xmm0,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 95 df[ 	]+vpshlw %xmm0,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 95 c6[ 	]+vpshlw %xmm0,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 95 c6[ 	]+vpshlw %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 95 df[ 	]+vpshlw %xmm7,%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 78 95 3e[ 	]+vpshlw %xmm0,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 95 fe[ 	]+vpshlw %xmm0,%xmm6,%xmm7
[ 	]*[a-f0-9]+:	8f e9 40 95 1e[ 	]+vpshlw %xmm7,\(%esi\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 40 95 02[ 	]+vpshlw %xmm7,\(%edx\),%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 95 3e[ 	]+vpshlw %xmm1,\(%esi\),%xmm7
[ 	]*[a-f0-9]+:	8f e9 70 95 c7[ 	]+vpshlw %xmm1,%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 78 95 1a[ 	]+vpshlw %xmm0,\(%edx\),%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 95 1b[ 	]+vpshlw \(%ebx\),%xmm0,%xmm3
[ 	]*[a-f0-9]+:	8f e9 f8 95 3b[ 	]+vpshlw \(%ebx\),%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 f8 95 06[ 	]+vpshlw \(%esi\),%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 95 18[ 	]+vpshlw \(%eax\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 95 c6[ 	]+vpshlw %xmm1,%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 95 de[ 	]+vpshlw %xmm1,%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 95 c0[ 	]+vpshlw %xmm1,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 40 95 c0[ 	]+vpshlw %xmm7,%xmm0,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c8 95 1e[ 	]+vpshlw \(%esi\),%xmm6,%xmm3
[ 	]*[a-f0-9]+:	8f e9 70 95 ff[ 	]+vpshlw %xmm1,%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 78 95 f8[ 	]+vpshlw %xmm0,%xmm0,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c0 95 1b[ 	]+vpshlw \(%ebx\),%xmm7,%xmm3
[ 	]*[a-f0-9]+:	8f e9 c0 95 03[ 	]+vpshlw \(%ebx\),%xmm7,%xmm0
[ 	]*[a-f0-9]+:	8f e9 c0 95 38[ 	]+vpshlw \(%eax\),%xmm7,%xmm7
[ 	]*[a-f0-9]+:	8f e9 c8 95 00[ 	]+vpshlw \(%eax\),%xmm6,%xmm0
[ 	]*[a-f0-9]+:	8f e9 70 95 df[ 	]+vpshlw %xmm1,%xmm7,%xmm3
#pass
