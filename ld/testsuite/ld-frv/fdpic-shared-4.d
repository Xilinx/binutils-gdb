#name: FRV uClinux PIC relocs to protected symbols, shared linking
#source: fdpic4.s
#objdump: -DR -j .text -j .data -j .got -j .plt
#as: -mfdpic
#ld: -shared

.*:     file format elf.*frv.*

Disassembly of section \.text:

00000604 <F4>:
 604:	80 3c 00 01 	call 608 <PF0>

00000608 <PF0>:
 608:	80 40 f0 10 	addi gr15,16,gr0
 60c:	80 fc 00 20 	setlos 0x20,gr0
 610:	80 f4 00 1c 	setlo 0x1c,gr0
 614:	80 f8 00 00 	sethi hi\(0x0\),gr0
 618:	80 40 f0 0c 	addi gr15,12,gr0
 61c:	80 fc 00 24 	setlos 0x24,gr0
 620:	80 f4 00 18 	setlo 0x18,gr0
 624:	80 f8 00 00 	sethi hi\(0x0\),gr0
 628:	80 40 ff f8 	addi gr15,-8,gr0
 62c:	80 fc ff f0 	setlos 0xfffffff0,gr0
 630:	80 f4 ff e8 	setlo 0xffe8,gr0
 634:	80 f8 ff ff 	sethi 0xffff,gr0
 638:	80 40 ff 68 	addi gr15,-152,gr0
 63c:	80 fc ff 68 	setlos 0xffffff68,gr0
 640:	80 f4 ff 68 	setlo 0xff68,gr0
 644:	80 f8 ff ff 	sethi 0xffff,gr0
 648:	80 f4 00 14 	setlo 0x14,gr0
 64c:	80 f8 00 00 	sethi hi\(0x0\),gr0
Disassembly of section \.data:

00010654 <D4>:
   10654:	00 00 00 04 	add\.p gr0,gr4,gr0
			10654: R_FRV_32	\.data

00010658 <PD0>:
   10658:	00 00 00 04 	add\.p gr0,gr4,gr0
			10658: R_FRV_FUNCDESC	\.text
   1065c:	00 00 00 04 	add\.p gr0,gr4,gr0
			1065c: R_FRV_32	\.text
Disassembly of section \.got:

000106d8 <_GLOBAL_OFFSET_TABLE_-0x18>:
   106d8:	00 00 00 04 	add\.p gr0,gr4,gr0
			106d8: R_FRV_FUNCDESC_VALUE	\.text
   106dc:	00 00 00 00 	add\.p gr0,gr0,gr0
   106e0:	00 00 00 04 	add\.p gr0,gr4,gr0
			106e0: R_FRV_FUNCDESC_VALUE	\.text
   106e4:	00 00 00 00 	add\.p gr0,gr0,gr0
   106e8:	00 00 00 04 	add\.p gr0,gr4,gr0
			106e8: R_FRV_FUNCDESC_VALUE	\.text
   106ec:	00 00 00 00 	add\.p gr0,gr0,gr0

000106f0 <_GLOBAL_OFFSET_TABLE_>:
	\.\.\.
   106fc:	00 00 00 04 	add\.p gr0,gr4,gr0
			106fc: R_FRV_FUNCDESC	\.text
   10700:	00 00 00 04 	add\.p gr0,gr4,gr0
			10700: R_FRV_32	\.text
   10704:	00 00 00 04 	add\.p gr0,gr4,gr0
			10704: R_FRV_32	\.data
   10708:	00 00 00 04 	add\.p gr0,gr4,gr0
			10708: R_FRV_FUNCDESC	\.text
   1070c:	00 00 00 04 	add\.p gr0,gr4,gr0
			1070c: R_FRV_32	\.text
   10710:	00 00 00 04 	add\.p gr0,gr4,gr0
			10710: R_FRV_32	\.text
   10714:	00 00 00 04 	add\.p gr0,gr4,gr0
			10714: R_FRV_FUNCDESC	\.text
