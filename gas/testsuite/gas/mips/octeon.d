#as: -march=octeon
#objdump: -M reg-names=numeric -dr
#name: MIPS octeon instructions

.*:     file format .*

Disassembly of section .text:

[0-9a-f]+ <foo>:
.*:	72538828 	baddu	\$17,\$18,\$19
.*:	70431028 	baddu	\$2,\$2,\$3
.*:	ca76fffd 	bbit0	\$19,0x16,[0-9a-f]+ <foo>
.*:	00000000 	nop
.*:	dbcbfffb 	bbit032	\$30,0xb,[0-9a-f]+ <foo>
.*:	00000000 	nop
.*:	d90afff9 	bbit032	\$8,0xa,[0-9a-f]+ <foo>
.*:	00000000 	nop
.*:	e87ffff7 	bbit1	\$3,0x1f,[0-9a-f]+ <foo>
.*:	00000000 	nop
.*:	fb0afff5 	bbit132	\$24,0xa,[0-9a-f]+ <foo>
.*:	00000000 	nop
.*:	f9cefff3 	bbit132	\$14,0xe,[0-9a-f]+ <foo>
.*:	00000000 	nop
.*:	715915b2 	cins	\$25,\$10,0x16,0x2
.*:	7129ec72 	cins	\$9,\$9,0x11,0x1d
.*:	704f44b3 	cins32	\$15,\$2,0x12,0x8
.*:	72d6b273 	cins32	\$22,\$22,0x9,0x16
.*:	73f8f833 	cins32	\$24,\$31,0x0,0x1f
.*:	71ef2973 	cins32	\$15,\$15,0x5,0x5
.*:	731c9803 	dmul	\$19,\$24,\$28
.*:	72b9a803 	dmul	\$21,\$21,\$25
.*:	73847efa 	exts	\$4,\$28,0x1b,0xf
.*:	71ef347a 	exts	\$15,\$15,0x11,0x6
.*:	71a442bb 	exts32	\$4,\$13,0xa,0x8
.*:	71efa2fb 	exts32	\$15,\$15,0xb,0x14
.*:	70874dbb 	exts32	\$7,\$4,0x16,0x9
.*:	7339c97b 	exts32	\$25,\$25,0x5,0x19
.*:	400dc800 	mfc0	\$13,\$25
.*:	400d5807 	mfc0	\$13,\$11,7
.*:	40861000 	mtc0	\$6,\$2
.*:	40954806 	mtc0	\$21,\$9,6
.*:	4023e800 	dmfc0	\$3,\$29
.*:	402ba005 	dmfc0	\$11,\$20,5
.*:	40b71000 	dmtc0	\$23,\$2
.*:	40a77002 	dmtc0	\$7,\$14,2
.*:	41606000 	di
.*:	41606020 	ei
.*:	48230084 	dmfc2	\$3,0x84
.*:	48a84200 	dmtc2	\$8,0x4200
.*:	73400008 	mtm0	\$26
.*:	7260000c 	mtm1	\$19
.*:	7240000d 	mtm2	\$18
.*:	72000009 	mtp0	\$16
.*:	7320000a 	mtp1	\$25
.*:	7120000b 	mtp2	\$9
.*:	7260402c 	pop	\$8,\$19
.*:	7040102c 	pop	\$2,\$2
.*:	72c0782d 	dpop	\$15,\$22
.*:	7180602d 	dpop	\$12,\$12
.*:	72f8e82a 	seq	\$29,\$23,\$24
.*:	70dc302a 	seq	\$6,\$6,\$28
.*:	71f1802e 	seqi	\$17,\$15,-512
.*:	721009ae 	seqi	\$16,\$16,38
.*:	7085bbae 	seqi	\$5,\$4,-274
.*:	718c7fee 	seqi	\$12,\$12,511
.*:	3b3e0200 	xori	\$30,\$25,0x200
.*:	2fde0001 	sltiu	\$30,\$30,1
.*:	65820309 	daddiu	\$2,\$12,777
.*:	2c420001 	sltiu	\$2,\$2,1
.*:	3c010001 	lui	\$1,0x1
.*:	73c1502a 	seq	\$10,\$30,\$1
.*:	3c01ffff 	lui	\$1,0xffff
.*:	342146fa 	ori	\$1,\$1,0x46fa
.*:	7321f02a 	seq	\$30,\$25,\$1
.*:	7042302b 	sne	\$6,\$2,\$2
.*:	72f4b82b 	sne	\$23,\$23,\$20
.*:	7204b1ef 	snei	\$4,\$16,-313
.*:	735a7fef 	snei	\$26,\$26,511
.*:	72f5802f 	snei	\$21,\$23,-512
.*:	718c146f 	snei	\$12,\$12,81
.*:	39c40379 	xori	\$4,\$14,0x379
.*:	0004202b 	sltu	\$4,\$0,\$4
.*:	65b80201 	daddiu	\$24,\$13,513
.*:	0018c02b 	sltu	\$24,\$0,\$24
.*:	3c010001 	lui	\$1,0x1
.*:	3421d1d2 	ori	\$1,\$1,0xd1d2
.*:	73c1502b 	sne	\$10,\$30,\$1
.*:	24018000 	li	\$1,-32768
.*:	7321f02b 	sne	\$30,\$25,\$1
.*:	0000008f 	synciobdma
.*:	0000018f 	syncs
.*:	0000010f 	syncw
.*:	0000014f 	syncws
.*:	7155a811 	v3mulu	\$21,\$10,\$21
.*:	728aa011 	v3mulu	\$20,\$20,\$10
.*:	72701810 	vmm0	\$3,\$19,\$16
.*:	73e9f810 	vmm0	\$31,\$31,\$9
.*:	7151e80f 	vmulu	\$29,\$10,\$17
.*:	7366d80f 	vmulu	\$27,\$27,\$6
#pass
