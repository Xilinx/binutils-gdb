#source: opcode.s
#as: -J
#objdump: -dwMsuffix
#name: i386 opcodes (w/ suffix)

.*: +file format .*

Disassembly of section .text:

0+000 <foo>:
 *[0-9a-f]+:	00 90 90 90 90 90[ 	]+addb[ 	]+%dl,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	01 90 90 90 90 90[ 	]+addl[ 	]+%edx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	02 90 90 90 90 90[ 	]+addb[ 	]+-0x6f6f6f70\(%eax\),%dl
 *[0-9a-f]+:	03 90 90 90 90 90[ 	]+addl[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	04 90[ 	]+addb[ 	]+\$0x90,%al
 *[0-9a-f]+:	05 90 90 90 90[ 	]+addl[ 	]+\$0x90909090,%eax
 *[0-9a-f]+:	06[ 	]+pushl[ 	]+%es
 *[0-9a-f]+:	07[ 	]+popl[ 	]+%es
 *[0-9a-f]+:	08 90 90 90 90 90[ 	]+orb[ 	]+%dl,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	09 90 90 90 90 90[ 	]+orl[ 	]+%edx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0a 90 90 90 90 90[ 	]+orb[ 	]+-0x6f6f6f70\(%eax\),%dl
 *[0-9a-f]+:	0b 90 90 90 90 90[ 	]+orl[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0c 90[ 	]+orb[ 	]+\$0x90,%al
 *[0-9a-f]+:	0d 90 90 90 90[ 	]+orl[ 	]+\$0x90909090,%eax
 *[0-9a-f]+:	0e[ 	]+pushl[ 	]+%cs
 *[0-9a-f]+:	10 90 90 90 90 90[ 	]+adcb[ 	]+%dl,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	11 90 90 90 90 90[ 	]+adcl[ 	]+%edx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	12 90 90 90 90 90[ 	]+adcb[ 	]+-0x6f6f6f70\(%eax\),%dl
 *[0-9a-f]+:	13 90 90 90 90 90[ 	]+adcl[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	14 90[ 	]+adcb[ 	]+\$0x90,%al
 *[0-9a-f]+:	15 90 90 90 90[ 	]+adcl[ 	]+\$0x90909090,%eax
 *[0-9a-f]+:	16[ 	]+pushl[ 	]+%ss
 *[0-9a-f]+:	17[ 	]+popl[ 	]+%ss
 *[0-9a-f]+:	18 90 90 90 90 90[ 	]+sbbb[ 	]+%dl,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	19 90 90 90 90 90[ 	]+sbbl[ 	]+%edx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	1a 90 90 90 90 90[ 	]+sbbb[ 	]+-0x6f6f6f70\(%eax\),%dl
 *[0-9a-f]+:	1b 90 90 90 90 90[ 	]+sbbl[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	1c 90[ 	]+sbbb[ 	]+\$0x90,%al
 *[0-9a-f]+:	1d 90 90 90 90[ 	]+sbbl[ 	]+\$0x90909090,%eax
 *[0-9a-f]+:	1e[ 	]+pushl[ 	]+%ds
 *[0-9a-f]+:	1f[ 	]+popl[ 	]+%ds
 *[0-9a-f]+:	20 90 90 90 90 90[ 	]+andb[ 	]+%dl,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	21 90 90 90 90 90[ 	]+andl[ 	]+%edx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	22 90 90 90 90 90[ 	]+andb[ 	]+-0x6f6f6f70\(%eax\),%dl
 *[0-9a-f]+:	23 90 90 90 90 90[ 	]+andl[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	24 90[ 	]+andb[ 	]+\$0x90,%al
 *[0-9a-f]+:	25 90 90 90 90[ 	]+andl[ 	]+\$0x90909090,%eax
 *[0-9a-f]+:	27[ 	]+daa[ 	]+
 *[0-9a-f]+:	28 90 90 90 90 90[ 	]+subb[ 	]+%dl,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	29 90 90 90 90 90[ 	]+subl[ 	]+%edx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	2a 90 90 90 90 90[ 	]+subb[ 	]+-0x6f6f6f70\(%eax\),%dl
 *[0-9a-f]+:	2b 90 90 90 90 90[ 	]+subl[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	2c 90[ 	]+subb[ 	]+\$0x90,%al
 *[0-9a-f]+:	2d 90 90 90 90[ 	]+subl[ 	]+\$0x90909090,%eax
 *[0-9a-f]+:	2f[ 	]+das[ 	]+
 *[0-9a-f]+:	30 90 90 90 90 90[ 	]+xorb[ 	]+%dl,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	31 90 90 90 90 90[ 	]+xorl[ 	]+%edx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	32 90 90 90 90 90[ 	]+xorb[ 	]+-0x6f6f6f70\(%eax\),%dl
 *[0-9a-f]+:	33 90 90 90 90 90[ 	]+xorl[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	34 90[ 	]+xorb[ 	]+\$0x90,%al
 *[0-9a-f]+:	35 90 90 90 90[ 	]+xorl[ 	]+\$0x90909090,%eax
 *[0-9a-f]+:	37[ 	]+aaa[ 	]+
 *[0-9a-f]+:	38 90 90 90 90 90[ 	]+cmpb[ 	]+%dl,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	39 90 90 90 90 90[ 	]+cmpl[ 	]+%edx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	3a 90 90 90 90 90[ 	]+cmpb[ 	]+-0x6f6f6f70\(%eax\),%dl
 *[0-9a-f]+:	3b 90 90 90 90 90[ 	]+cmpl[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	3c 90[ 	]+cmpb[ 	]+\$0x90,%al
 *[0-9a-f]+:	3d 90 90 90 90[ 	]+cmpl[ 	]+\$0x90909090,%eax
 *[0-9a-f]+:	3f[ 	]+aas[ 	]+
 *[0-9a-f]+:	40[ 	]+incl[ 	]+%eax
 *[0-9a-f]+:	41[ 	]+incl[ 	]+%ecx
 *[0-9a-f]+:	42[ 	]+incl[ 	]+%edx
 *[0-9a-f]+:	43[ 	]+incl[ 	]+%ebx
 *[0-9a-f]+:	44[ 	]+incl[ 	]+%esp
 *[0-9a-f]+:	45[ 	]+incl[ 	]+%ebp
 *[0-9a-f]+:	46[ 	]+incl[ 	]+%esi
 *[0-9a-f]+:	47[ 	]+incl[ 	]+%edi
 *[0-9a-f]+:	48[ 	]+decl[ 	]+%eax
 *[0-9a-f]+:	49[ 	]+decl[ 	]+%ecx
 *[0-9a-f]+:	4a[ 	]+decl[ 	]+%edx
 *[0-9a-f]+:	4b[ 	]+decl[ 	]+%ebx
 *[0-9a-f]+:	4c[ 	]+decl[ 	]+%esp
 *[0-9a-f]+:	4d[ 	]+decl[ 	]+%ebp
 *[0-9a-f]+:	4e[ 	]+decl[ 	]+%esi
 *[0-9a-f]+:	4f[ 	]+decl[ 	]+%edi
 *[0-9a-f]+:	50[ 	]+pushl[ 	]+%eax
 *[0-9a-f]+:	51[ 	]+pushl[ 	]+%ecx
 *[0-9a-f]+:	52[ 	]+pushl[ 	]+%edx
 *[0-9a-f]+:	53[ 	]+pushl[ 	]+%ebx
 *[0-9a-f]+:	54[ 	]+pushl[ 	]+%esp
 *[0-9a-f]+:	55[ 	]+pushl[ 	]+%ebp
 *[0-9a-f]+:	56[ 	]+pushl[ 	]+%esi
 *[0-9a-f]+:	57[ 	]+pushl[ 	]+%edi
 *[0-9a-f]+:	58[ 	]+popl[ 	]+%eax
 *[0-9a-f]+:	59[ 	]+popl[ 	]+%ecx
 *[0-9a-f]+:	5a[ 	]+popl[ 	]+%edx
 *[0-9a-f]+:	5b[ 	]+popl[ 	]+%ebx
 *[0-9a-f]+:	5c[ 	]+popl[ 	]+%esp
 *[0-9a-f]+:	5d[ 	]+popl[ 	]+%ebp
 *[0-9a-f]+:	5e[ 	]+popl[ 	]+%esi
 *[0-9a-f]+:	5f[ 	]+popl[ 	]+%edi
 *[0-9a-f]+:	60[ 	]+pushal 
 *[0-9a-f]+:	61[ 	]+popal[ 	]+
 *[0-9a-f]+:	62 90 90 90 90 90[ 	]+boundl %edx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	63 90 90 90 90 90[ 	]+arpl[ 	]+%dx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	68 90 90 90 90[ 	]+pushl[ 	]+\$0x90909090
 *[0-9a-f]+:	69 90 90 90 90 90 90 90 90 90[ 	]+imull[ 	]+\$0x90909090,-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	6a 90[ 	]+pushl[ 	]+\$0xffffff90
 *[0-9a-f]+:	6b 90 90 90 90 90 90[ 	]+imull[ 	]+\$0xffffff90,-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	6c[ 	]+insb[ 	]+\(%dx\),%es:\(%edi\)
 *[0-9a-f]+:	6d[ 	]+insl[ 	]+\(%dx\),%es:\(%edi\)
 *[0-9a-f]+:	6e[ 	]+outsb[ 	]+%ds:\(%esi\),\(%dx\)
 *[0-9a-f]+:	6f[ 	]+outsl[ 	]+%ds:\(%esi\),\(%dx\)
 *[0-9a-f]+:	70 90[ 	]+jo[ 	]+(0x)?df.*
 *[0-9a-f]+:	71 90[ 	]+jno[ 	]+(0x)?e1.*
 *[0-9a-f]+:	72 90[ 	]+jb[ 	]+(0x)?e3.*
 *[0-9a-f]+:	73 90[ 	]+jae[ 	]+(0x)?e5.*
 *[0-9a-f]+:	74 90[ 	]+je[ 	]+(0x)?e7.*
 *[0-9a-f]+:	75 90[ 	]+jne[ 	]+(0x)?e9.*
 *[0-9a-f]+:	76 90[ 	]+jbe[ 	]+(0x)?eb.*
 *[0-9a-f]+:	77 90[ 	]+ja[ 	]+(0x)?ed.*
 *[0-9a-f]+:	78 90[ 	]+js[ 	]+(0x)?ef.*
 *[0-9a-f]+:	79 90[ 	]+jns[ 	]+(0x)?f1.*
 *[0-9a-f]+:	7a 90[ 	]+jp[ 	]+(0x)?f3.*
 *[0-9a-f]+:	7b 90[ 	]+jnp[ 	]+(0x)?f5.*
 *[0-9a-f]+:	7c 90[ 	]+jl[ 	]+(0x)?f7.*
 *[0-9a-f]+:	7d 90[ 	]+jge[ 	]+(0x)?f9.*
 *[0-9a-f]+:	7e 90[ 	]+jle[ 	]+(0x)?fb.*
 *[0-9a-f]+:	7f 90[ 	]+jg[ 	]+(0x)?fd.*
 *[0-9a-f]+:	80 90 90 90 90 90 90[ 	]+adcb[ 	]+\$0x90,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	81 90 90 90 90 90 90 90 90 90[ 	]+adcl[ 	]+\$0x90909090,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	83 90 90 90 90 90 90[ 	]+adcl[ 	]+\$0xffffff90,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	84 90 90 90 90 90[ 	]+testb[ 	]+%dl,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	85 90 90 90 90 90[ 	]+testl[ 	]+%edx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	86 90 90 90 90 90[ 	]+xchgb[ 	]+%dl,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	87 90 90 90 90 90[ 	]+xchgl[ 	]+%edx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	88 90 90 90 90 90[ 	]+movb[ 	]+%dl,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	89 90 90 90 90 90[ 	]+movl[ 	]+%edx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	8a 90 90 90 90 90[ 	]+movb[ 	]+-0x6f6f6f70\(%eax\),%dl
 *[0-9a-f]+:	8b 90 90 90 90 90[ 	]+movl[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	8c 90 90 90 90 90[ 	]+movw[ 	]+%ss,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	8d 90 90 90 90 90[ 	]+leal[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	8e 90 90 90 90 90[ 	]+movw[ 	]+-0x6f6f6f70\(%eax\),%ss
 *[0-9a-f]+:	8f 80 90 90 90 90[ 	]+popl[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	90[ 	]+nop
 *[0-9a-f]+:	91[ 	]+xchgl[ 	]+%eax,%ecx
 *[0-9a-f]+:	92[ 	]+xchgl[ 	]+%eax,%edx
 *[0-9a-f]+:	93[ 	]+xchgl[ 	]+%eax,%ebx
 *[0-9a-f]+:	94[ 	]+xchgl[ 	]+%eax,%esp
 *[0-9a-f]+:	95[ 	]+xchgl[ 	]+%eax,%ebp
 *[0-9a-f]+:	96[ 	]+xchgl[ 	]+%eax,%esi
 *[0-9a-f]+:	97[ 	]+xchgl[ 	]+%eax,%edi
 *[0-9a-f]+:	98[ 	]+cwtl[ 	]+
 *[0-9a-f]+:	99[ 	]+cltd[ 	]+
 *[0-9a-f]+:	9a 90 90 90 90 90 90[ 	]+lcalll \$0x9090,\$0x90909090
 *[0-9a-f]+:	9b[ 	]+fwait
 *[0-9a-f]+:	9c[ 	]+pushfl 
 *[0-9a-f]+:	9d[ 	]+popfl[ 	]+
 *[0-9a-f]+:	9e[ 	]+sahf[ 	]+
 *[0-9a-f]+:	9f[ 	]+lahf[ 	]+
 *[0-9a-f]+:	a0 90 90 90 90[ 	]+movb[ 	]+0x90909090,%al
 *[0-9a-f]+:	a1 90 90 90 90[ 	]+movl[ 	]+0x90909090,%eax
 *[0-9a-f]+:	a2 90 90 90 90[ 	]+movb[ 	]+%al,0x90909090
 *[0-9a-f]+:	a3 90 90 90 90[ 	]+movl[ 	]+%eax,0x90909090
 *[0-9a-f]+:	a4[ 	]+movsb[ 	]+%ds:\(%esi\),%es:\(%edi\)
 *[0-9a-f]+:	a5[ 	]+movsl[ 	]+%ds:\(%esi\),%es:\(%edi\)
 *[0-9a-f]+:	a6[ 	]+cmpsb[ 	]+%es:\(%edi\),%ds:\(%esi\)
 *[0-9a-f]+:	a7[ 	]+cmpsl[ 	]+%es:\(%edi\),%ds:\(%esi\)
 *[0-9a-f]+:	a8 90[ 	]+testb[ 	]+\$0x90,%al
 *[0-9a-f]+:	a9 90 90 90 90[ 	]+testl[ 	]+\$0x90909090,%eax
 *[0-9a-f]+:	aa[ 	]+stosb[ 	]+%al,%es:\(%edi\)
 *[0-9a-f]+:	ab[ 	]+stosl[ 	]+%eax,%es:\(%edi\)
 *[0-9a-f]+:	ac[ 	]+lodsb[ 	]+%ds:\(%esi\),%al
 *[0-9a-f]+:	ad[ 	]+lodsl[ 	]+%ds:\(%esi\),%eax
 *[0-9a-f]+:	ae[ 	]+scasb[ 	]+%es:\(%edi\),%al
 *[0-9a-f]+:	af[ 	]+scasl[ 	]+%es:\(%edi\),%eax
 *[0-9a-f]+:	b0 90[ 	]+movb[ 	]+\$0x90,%al
 *[0-9a-f]+:	b1 90[ 	]+movb[ 	]+\$0x90,%cl
 *[0-9a-f]+:	b2 90[ 	]+movb[ 	]+\$0x90,%dl
 *[0-9a-f]+:	b3 90[ 	]+movb[ 	]+\$0x90,%bl
 *[0-9a-f]+:	b4 90[ 	]+movb[ 	]+\$0x90,%ah
 *[0-9a-f]+:	b5 90[ 	]+movb[ 	]+\$0x90,%ch
 *[0-9a-f]+:	b6 90[ 	]+movb[ 	]+\$0x90,%dh
 *[0-9a-f]+:	b7 90[ 	]+movb[ 	]+\$0x90,%bh
 *[0-9a-f]+:	b8 90 90 90 90[ 	]+movl[ 	]+\$0x90909090,%eax
 *[0-9a-f]+:	b9 90 90 90 90[ 	]+movl[ 	]+\$0x90909090,%ecx
 *[0-9a-f]+:	ba 90 90 90 90[ 	]+movl[ 	]+\$0x90909090,%edx
 *[0-9a-f]+:	bb 90 90 90 90[ 	]+movl[ 	]+\$0x90909090,%ebx
 *[0-9a-f]+:	bc 90 90 90 90[ 	]+movl[ 	]+\$0x90909090,%esp
 *[0-9a-f]+:	bd 90 90 90 90[ 	]+movl[ 	]+\$0x90909090,%ebp
 *[0-9a-f]+:	be 90 90 90 90[ 	]+movl[ 	]+\$0x90909090,%esi
 *[0-9a-f]+:	bf 90 90 90 90[ 	]+movl[ 	]+\$0x90909090,%edi
 *[0-9a-f]+:	c0 90 90 90 90 90 90[ 	]+rclb[ 	]+\$0x90,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	c1 90 90 90 90 90 90[ 	]+rcll[ 	]+\$0x90,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	c2 90 90[ 	]+retl[ 	]+\$0x9090
 *[0-9a-f]+:	c3[ 	]+retl[ 	]+
 *[0-9a-f]+:	c4 90 90 90 90 90[ 	]+lesl[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	c5 90 90 90 90 90[ 	]+ldsl[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	c6 80 90 90 90 90 90[ 	]+movb[ 	]+\$0x90,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	c7 80 90 90 90 90 90 90 90 90[ 	]+movl[ 	]+\$0x90909090,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	c8 90 90 90[ 	]+enterl \$0x9090,\$0x90
 *[0-9a-f]+:	c9[ 	]+leavel 
 *[0-9a-f]+:	ca 90 90[ 	]+lretl[ 	]+\$0x9090
 *[0-9a-f]+:	cb[ 	]+lretl[ 	]+
 *[0-9a-f]+:	cc[ 	]+int3[ 	]+
 *[0-9a-f]+:	cd 90[ 	]+int[ 	]+\$0x90
 *[0-9a-f]+:	ce[ 	]+into[ 	]+
 *[0-9a-f]+:	cf[ 	]+iretl[ 	]+
 *[0-9a-f]+:	d0 90 90 90 90 90[ 	]+rclb[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	d1 90 90 90 90 90[ 	]+rcll[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	d2 90 90 90 90 90[ 	]+rclb[ 	]+%cl,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	d3 90 90 90 90 90[ 	]+rcll[ 	]+%cl,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	d4 90[ 	]+aam[ 	]+\$0x90
 *[0-9a-f]+:	d5 90[ 	]+aad[ 	]+\$0x90
 *[0-9a-f]+:	d7[ 	]+xlat[ 	]+%ds:\(%ebx\)
 *[0-9a-f]+:	d8 90 90 90 90 90[ 	]+fcoms[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	d9 90 90 90 90 90[ 	]+fsts[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	da 90 90 90 90 90[ 	]+ficoml -0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	db 90 90 90 90 90[ 	]+fistl[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	dc 90 90 90 90 90[ 	]+fcoml[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	dd 90 90 90 90 90[ 	]+fstl[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	de 90 90 90 90 90[ 	]+ficom[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	df 90 90 90 90 90[ 	]+fist[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	e0 90[ 	]+loopnel (0x)?25c.*
 *[0-9a-f]+:	e1 90[ 	]+loopel (0x)?25e.*
 *[0-9a-f]+:	e2 90[ 	]+loopl[ 	]+(0x)?260.*
 *[0-9a-f]+:	e3 90[ 	]+jecxz[ 	]+(0x)?262.*
 *[0-9a-f]+:	e4 90[ 	]+inb[ 	]+\$0x90,%al
 *[0-9a-f]+:	e5 90[ 	]+inl[ 	]+\$0x90,%eax
 *[0-9a-f]+:	e6 90[ 	]+outb[ 	]+%al,\$0x90
 *[0-9a-f]+:	e7 90[ 	]+outl[ 	]+%eax,\$0x90
 *[0-9a-f]+:	e8 90 90 90 90[ 	]+calll[ 	]+(0x)?9090936f.*
 *[0-9a-f]+:	e9 90 90 90 90[ 	]+jmpl[ 	]+(0x)?90909374.*
 *[0-9a-f]+:	ea 90 90 90 90 90 90[ 	]+ljmpl[ 	]+\$0x9090,\$0x90909090
 *[0-9a-f]+:	eb 90[ 	]+jmp[ 	]+(0x)?27d.*
 *[0-9a-f]+:	ec[ 	]+inb[ 	]+\(%dx\),%al
 *[0-9a-f]+:	ed[ 	]+inl[ 	]+\(%dx\),%eax
 *[0-9a-f]+:	ee[ 	]+outb[ 	]+%al,\(%dx\)
 *[0-9a-f]+:	ef[ 	]+outl[ 	]+%eax,\(%dx\)
 *[0-9a-f]+:	f4[ 	]+hlt[ 	]+
 *[0-9a-f]+:	f5[ 	]+cmc[ 	]+
 *[0-9a-f]+:	f6 90 90 90 90 90[ 	]+notb[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	f7 90 90 90 90 90[ 	]+notl[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	f8[ 	]+clc[ 	]+
 *[0-9a-f]+:	f9[ 	]+stc[ 	]+
 *[0-9a-f]+:	fa[ 	]+cli[ 	]+
 *[0-9a-f]+:	fb[ 	]+sti[ 	]+
 *[0-9a-f]+:	fc[ 	]+cld[ 	]+
 *[0-9a-f]+:	fd[ 	]+std[ 	]+
 *[0-9a-f]+:	ff 90 90 90 90 90[ 	]+calll[ 	]+\*-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f 00 90 90 90 90 90[ 	]+lldt[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f 01 90 90 90 90 90[ 	]+lgdtl[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f 02 90 90 90 90 90[ 	]+larl[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f 03 90 90 90 90 90[ 	]+lsll[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f 06[ 	]+clts[ 	]+
 *[0-9a-f]+:	0f 08[ 	]+invd[ 	]+
 *[0-9a-f]+:	0f 09[ 	]+wbinvd 
 *[0-9a-f]+:	0f 0b[ 	]+ud2[ 	]+
 *[0-9a-f]+:	0f 20 d0[ 	]+movl[ 	]+%cr2,%eax
 *[0-9a-f]+:	0f 21 d0[ 	]+movl[ 	]+%db2,%eax
 *[0-9a-f]+:	0f 22 d0[ 	]+movl[ 	]+%eax,%cr2
 *[0-9a-f]+:	0f 23 d0[ 	]+movl[ 	]+%eax,%db2
 *[0-9a-f]+:	0f 24 d0[ 	]+movl[ 	]+%tr2,%eax
 *[0-9a-f]+:	0f 26 d0[ 	]+movl[ 	]+%eax,%tr2
 *[0-9a-f]+:	0f 30[ 	]+wrmsr[ 	]+
 *[0-9a-f]+:	0f 31[ 	]+rdtsc[ 	]+
 *[0-9a-f]+:	0f 32[ 	]+rdmsr[ 	]+
 *[0-9a-f]+:	0f 33[ 	]+rdpmc[ 	]+
 *[0-9a-f]+:	0f 40 90 90 90 90 90[ 	]+cmovol[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f 41 90 90 90 90 90[ 	]+cmovnol -0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f 42 90 90 90 90 90[ 	]+cmovbl[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f 43 90 90 90 90 90[ 	]+cmovael -0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f 44 90 90 90 90 90[ 	]+cmovel[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f 45 90 90 90 90 90[ 	]+cmovnel -0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f 46 90 90 90 90 90[ 	]+cmovbel -0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f 47 90 90 90 90 90[ 	]+cmoval[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f 48 90 90 90 90 90[ 	]+cmovsl[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f 49 90 90 90 90 90[ 	]+cmovnsl -0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f 4a 90 90 90 90 90[ 	]+cmovpl[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f 4b 90 90 90 90 90[ 	]+cmovnpl -0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f 4c 90 90 90 90 90[ 	]+cmovll[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f 4d 90 90 90 90 90[ 	]+cmovgel -0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f 4e 90 90 90 90 90[ 	]+cmovlel -0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f 4f 90 90 90 90 90[ 	]+cmovgl[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f 60 90 90 90 90 90[ 	]+punpcklbw -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f 61 90 90 90 90 90[ 	]+punpcklwd -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f 62 90 90 90 90 90[ 	]+punpckldq -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f 63 90 90 90 90 90[ 	]+packsswb -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f 64 90 90 90 90 90[ 	]+pcmpgtb -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f 65 90 90 90 90 90[ 	]+pcmpgtw -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f 66 90 90 90 90 90[ 	]+pcmpgtd -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f 67 90 90 90 90 90[ 	]+packuswb -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f 68 90 90 90 90 90[ 	]+punpckhbw -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f 69 90 90 90 90 90[ 	]+punpckhwd -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f 6a 90 90 90 90 90[ 	]+punpckhdq -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f 6b 90 90 90 90 90[ 	]+packssdw -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f 6e 90 90 90 90 90[ 	]+movd[ 	]+-0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f 6f 90 90 90 90 90[ 	]+movq[ 	]+-0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f 71 d0 90[ 	]+psrlw[ 	]+\$0x90,%mm0
 *[0-9a-f]+:	0f 72 d0 90[ 	]+psrld[ 	]+\$0x90,%mm0
 *[0-9a-f]+:	0f 73 d0 90[ 	]+psrlq[ 	]+\$0x90,%mm0
 *[0-9a-f]+:	0f 74 90 90 90 90 90[ 	]+pcmpeqb -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f 75 90 90 90 90 90[ 	]+pcmpeqw -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f 76 90 90 90 90 90[ 	]+pcmpeqd -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f 77[ 	]+emms[ 	]+
 *[0-9a-f]+:	0f 7e 90 90 90 90 90[ 	]+movd[ 	]+%mm2,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f 7f 90 90 90 90 90[ 	]+movq[ 	]+%mm2,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f 80 90 90 90 90[ 	]+jo[ 	]+909094e2 <foo\+0x909094e2>
 *[0-9a-f]+:	0f 81 90 90 90 90[ 	]+jno[ 	]+909094e8 <foo\+0x909094e8>
 *[0-9a-f]+:	0f 82 90 90 90 90[ 	]+jb[ 	]+909094ee <foo\+0x909094ee>
 *[0-9a-f]+:	0f 83 90 90 90 90[ 	]+jae[ 	]+909094f4 <foo\+0x909094f4>
 *[0-9a-f]+:	0f 84 90 90 90 90[ 	]+je[ 	]+909094fa <foo\+0x909094fa>
 *[0-9a-f]+:	0f 85 90 90 90 90[ 	]+jne[ 	]+90909500 <foo\+0x90909500>
 *[0-9a-f]+:	0f 86 90 90 90 90[ 	]+jbe[ 	]+90909506 <foo\+0x90909506>
 *[0-9a-f]+:	0f 87 90 90 90 90[ 	]+ja[ 	]+9090950c <foo\+0x9090950c>
 *[0-9a-f]+:	0f 88 90 90 90 90[ 	]+js[ 	]+90909512 <foo\+0x90909512>
 *[0-9a-f]+:	0f 89 90 90 90 90[ 	]+jns[ 	]+90909518 <foo\+0x90909518>
 *[0-9a-f]+:	0f 8a 90 90 90 90[ 	]+jp[ 	]+9090951e <foo\+0x9090951e>
 *[0-9a-f]+:	0f 8b 90 90 90 90[ 	]+jnp[ 	]+90909524 <foo\+0x90909524>
 *[0-9a-f]+:	0f 8c 90 90 90 90[ 	]+jl[ 	]+9090952a <foo\+0x9090952a>
 *[0-9a-f]+:	0f 8d 90 90 90 90[ 	]+jge[ 	]+90909530 <foo\+0x90909530>
 *[0-9a-f]+:	0f 8e 90 90 90 90[ 	]+jle[ 	]+90909536 <foo\+0x90909536>
 *[0-9a-f]+:	0f 8f 90 90 90 90[ 	]+jg[ 	]+9090953c <foo\+0x9090953c>
 *[0-9a-f]+:	0f 90 80 90 90 90 90[ 	]+seto[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f 91 80 90 90 90 90[ 	]+setno[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f 92 80 90 90 90 90[ 	]+setb[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f 93 80 90 90 90 90[ 	]+setae[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f 94 80 90 90 90 90[ 	]+sete[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f 95 80 90 90 90 90[ 	]+setne[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f 96 80 90 90 90 90[ 	]+setbe[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f 97 80 90 90 90 90[ 	]+seta[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f 98 80 90 90 90 90[ 	]+sets[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f 99 80 90 90 90 90[ 	]+setns[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f 9a 80 90 90 90 90[ 	]+setp[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f 9b 80 90 90 90 90[ 	]+setnp[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f 9c 80 90 90 90 90[ 	]+setl[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f 9d 80 90 90 90 90[ 	]+setge[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f 9e 80 90 90 90 90[ 	]+setle[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f 9f 80 90 90 90 90[ 	]+setg[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f a0[ 	]+pushl[ 	]+%fs
 *[0-9a-f]+:	0f a1[ 	]+popl[ 	]+%fs
 *[0-9a-f]+:	0f a2[ 	]+cpuid[ 	]+
 *[0-9a-f]+:	0f a3 90 90 90 90 90[ 	]+btl[ 	]+%edx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f a4 90 90 90 90 90 90[ 	]+shldl[ 	]+\$0x90,%edx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f a5 90 90 90 90 90[ 	]+shldl[ 	]+%cl,%edx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f a8[ 	]+pushl[ 	]+%gs
 *[0-9a-f]+:	0f a9[ 	]+popl[ 	]+%gs
 *[0-9a-f]+:	0f aa[ 	]+rsm[ 	]+
 *[0-9a-f]+:	0f ab 90 90 90 90 90[ 	]+btsl[ 	]+%edx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f ac 90 90 90 90 90 90[ 	]+shrdl[ 	]+\$0x90,%edx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f ad 90 90 90 90 90[ 	]+shrdl[ 	]+%cl,%edx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f af 90 90 90 90 90[ 	]+imull[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f b0 90 90 90 90 90[ 	]+cmpxchgb %dl,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f b1 90 90 90 90 90[ 	]+cmpxchgl %edx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f b2 90 90 90 90 90[ 	]+lssl[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f b3 90 90 90 90 90[ 	]+btrl[ 	]+%edx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f b4 90 90 90 90 90[ 	]+lfsl[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f b5 90 90 90 90 90[ 	]+lgsl[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f b6 90 90 90 90 90[ 	]+movzbl -0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f b7 90 90 90 90 90[ 	]+movzwl -0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f b9[ 	]+ud1[ 	]+
 *[0-9a-f]+:	0f bb 90 90 90 90 90[ 	]+btcl[ 	]+%edx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f bc 90 90 90 90 90[ 	]+bsfl[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f bd 90 90 90 90 90[ 	]+bsrl[ 	]+-0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f be 90 90 90 90 90[ 	]+movsbl -0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f bf 90 90 90 90 90[ 	]+movswl -0x6f6f6f70\(%eax\),%edx
 *[0-9a-f]+:	0f c0 90 90 90 90 90[ 	]+xaddb[ 	]+%dl,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f c1 90 90 90 90 90[ 	]+xaddl[ 	]+%edx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	0f c8[ 	]+bswap[ 	]+%eax
 *[0-9a-f]+:	0f c9[ 	]+bswap[ 	]+%ecx
 *[0-9a-f]+:	0f ca[ 	]+bswap[ 	]+%edx
 *[0-9a-f]+:	0f cb[ 	]+bswap[ 	]+%ebx
 *[0-9a-f]+:	0f cc[ 	]+bswap[ 	]+%esp
 *[0-9a-f]+:	0f cd[ 	]+bswap[ 	]+%ebp
 *[0-9a-f]+:	0f ce[ 	]+bswap[ 	]+%esi
 *[0-9a-f]+:	0f cf[ 	]+bswap[ 	]+%edi
 *[0-9a-f]+:	0f d1 90 90 90 90 90[ 	]+psrlw[ 	]+-0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f d2 90 90 90 90 90[ 	]+psrld[ 	]+-0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f d3 90 90 90 90 90[ 	]+psrlq[ 	]+-0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f d5 90 90 90 90 90[ 	]+pmullw -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f d8 90 90 90 90 90[ 	]+psubusb -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f d9 90 90 90 90 90[ 	]+psubusw -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f db 90 90 90 90 90[ 	]+pand[ 	]+-0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f dc 90 90 90 90 90[ 	]+paddusb -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f dd 90 90 90 90 90[ 	]+paddusw -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f df 90 90 90 90 90[ 	]+pandn[ 	]+-0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f e1 90 90 90 90 90[ 	]+psraw[ 	]+-0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f e2 90 90 90 90 90[ 	]+psrad[ 	]+-0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f e5 90 90 90 90 90[ 	]+pmulhw -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f e8 90 90 90 90 90[ 	]+psubsb -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f e9 90 90 90 90 90[ 	]+psubsw -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f eb 90 90 90 90 90[ 	]+por[ 	]+-0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f ec 90 90 90 90 90[ 	]+paddsb -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f ed 90 90 90 90 90[ 	]+paddsw -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f ef 90 90 90 90 90[ 	]+pxor[ 	]+-0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f f1 90 90 90 90 90[ 	]+psllw[ 	]+-0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f f2 90 90 90 90 90[ 	]+pslld[ 	]+-0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f f3 90 90 90 90 90[ 	]+psllq[ 	]+-0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f f5 90 90 90 90 90[ 	]+pmaddwd -0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f f8 90 90 90 90 90[ 	]+psubb[ 	]+-0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f f9 90 90 90 90 90[ 	]+psubw[ 	]+-0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f fa 90 90 90 90 90[ 	]+psubd[ 	]+-0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f fc 90 90 90 90 90[ 	]+paddb[ 	]+-0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f fd 90 90 90 90 90[ 	]+paddw[ 	]+-0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	0f fe 90 90 90 90 90[ 	]+paddd[ 	]+-0x6f6f6f70\(%eax\),%mm2
 *[0-9a-f]+:	66 01 90 90 90 90 90[ 	]+addw[ 	]+%dx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 03 90 90 90 90 90[ 	]+addw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 05 90 90[ 	]+addw[ 	]+\$0x9090,%ax
 *[0-9a-f]+:	66 06[ 	]+pushw[ 	]+%es
 *[0-9a-f]+:	66 07[ 	]+popw[ 	]+%es
 *[0-9a-f]+:	66 09 90 90 90 90 90[ 	]+orw[ 	]+%dx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 0b 90 90 90 90 90[ 	]+orw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0d 90 90[ 	]+orw[ 	]+\$0x9090,%ax
 *[0-9a-f]+:	66 0e[ 	]+pushw[ 	]+%cs
 *[0-9a-f]+:	66 11 90 90 90 90 90[ 	]+adcw[ 	]+%dx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 13 90 90 90 90 90[ 	]+adcw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 15 90 90[ 	]+adcw[ 	]+\$0x9090,%ax
 *[0-9a-f]+:	66 16[ 	]+pushw[ 	]+%ss
 *[0-9a-f]+:	66 17[ 	]+popw[ 	]+%ss
 *[0-9a-f]+:	66 19 90 90 90 90 90[ 	]+sbbw[ 	]+%dx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 1b 90 90 90 90 90[ 	]+sbbw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 1d 90 90[ 	]+sbbw[ 	]+\$0x9090,%ax
 *[0-9a-f]+:	66 1e[ 	]+pushw[ 	]+%ds
 *[0-9a-f]+:	66 1f[ 	]+popw[ 	]+%ds
 *[0-9a-f]+:	66 21 90 90 90 90 90[ 	]+andw[ 	]+%dx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 23 90 90 90 90 90[ 	]+andw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 25 90 90[ 	]+andw[ 	]+\$0x9090,%ax
 *[0-9a-f]+:	66 29 90 90 90 90 90[ 	]+subw[ 	]+%dx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 2b 90 90 90 90 90[ 	]+subw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 2d 90 90[ 	]+subw[ 	]+\$0x9090,%ax
 *[0-9a-f]+:	66 31 90 90 90 90 90[ 	]+xorw[ 	]+%dx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 33 90 90 90 90 90[ 	]+xorw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 35 90 90[ 	]+xorw[ 	]+\$0x9090,%ax
 *[0-9a-f]+:	66 39 90 90 90 90 90[ 	]+cmpw[ 	]+%dx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 3b 90 90 90 90 90[ 	]+cmpw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 3d 90 90[ 	]+cmpw[ 	]+\$0x9090,%ax
 *[0-9a-f]+:	66 40[ 	]+incw[ 	]+%ax
 *[0-9a-f]+:	66 41[ 	]+incw[ 	]+%cx
 *[0-9a-f]+:	66 42[ 	]+incw[ 	]+%dx
 *[0-9a-f]+:	66 43[ 	]+incw[ 	]+%bx
 *[0-9a-f]+:	66 44[ 	]+incw[ 	]+%sp
 *[0-9a-f]+:	66 45[ 	]+incw[ 	]+%bp
 *[0-9a-f]+:	66 46[ 	]+incw[ 	]+%si
 *[0-9a-f]+:	66 47[ 	]+incw[ 	]+%di
 *[0-9a-f]+:	66 48[ 	]+decw[ 	]+%ax
 *[0-9a-f]+:	66 49[ 	]+decw[ 	]+%cx
 *[0-9a-f]+:	66 4a[ 	]+decw[ 	]+%dx
 *[0-9a-f]+:	66 4b[ 	]+decw[ 	]+%bx
 *[0-9a-f]+:	66 4c[ 	]+decw[ 	]+%sp
 *[0-9a-f]+:	66 4d[ 	]+decw[ 	]+%bp
 *[0-9a-f]+:	66 4e[ 	]+decw[ 	]+%si
 *[0-9a-f]+:	66 4f[ 	]+decw[ 	]+%di
 *[0-9a-f]+:	66 50[ 	]+pushw[ 	]+%ax
 *[0-9a-f]+:	66 51[ 	]+pushw[ 	]+%cx
 *[0-9a-f]+:	66 52[ 	]+pushw[ 	]+%dx
 *[0-9a-f]+:	66 53[ 	]+pushw[ 	]+%bx
 *[0-9a-f]+:	66 54[ 	]+pushw[ 	]+%sp
 *[0-9a-f]+:	66 55[ 	]+pushw[ 	]+%bp
 *[0-9a-f]+:	66 56[ 	]+pushw[ 	]+%si
 *[0-9a-f]+:	66 57[ 	]+pushw[ 	]+%di
 *[0-9a-f]+:	66 58[ 	]+popw[ 	]+%ax
 *[0-9a-f]+:	66 59[ 	]+popw[ 	]+%cx
 *[0-9a-f]+:	66 5a[ 	]+popw[ 	]+%dx
 *[0-9a-f]+:	66 5b[ 	]+popw[ 	]+%bx
 *[0-9a-f]+:	66 5c[ 	]+popw[ 	]+%sp
 *[0-9a-f]+:	66 5d[ 	]+popw[ 	]+%bp
 *[0-9a-f]+:	66 5e[ 	]+popw[ 	]+%si
 *[0-9a-f]+:	66 5f[ 	]+popw[ 	]+%di
 *[0-9a-f]+:	66 60[ 	]+pushaw 
 *[0-9a-f]+:	66 61[ 	]+popaw[ 	]+
 *[0-9a-f]+:	66 62 90 90 90 90 90[ 	]+boundw %dx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 68 90 90[ 	]+pushw[ 	]+\$0x9090
 *[0-9a-f]+:	66 69 90 90 90 90 90 90 90[ 	]+imulw[ 	]+\$0x9090,-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 6a 90[ 	]+pushw[ 	]+\$0xff90
 *[0-9a-f]+:	66 6b 90 90 90 90 90 90[ 	]+imulw[ 	]+\$0xff90,-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 6d[ 	]+insw[ 	]+\(%dx\),%es:\(%edi\)
 *[0-9a-f]+:	66 6f[ 	]+outsw[ 	]+%ds:\(%esi\),\(%dx\)
 *[0-9a-f]+:	66 81 90 90 90 90 90 90 90[ 	]+adcw[ 	]+\$0x9090,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 83 90 90 90 90 90 90[ 	]+adcw[ 	]+\$0xff90,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 85 90 90 90 90 90[ 	]+testw[ 	]+%dx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 87 90 90 90 90 90[ 	]+xchgw[ 	]+%dx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 89 90 90 90 90 90[ 	]+movw[ 	]+%dx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 8b 90 90 90 90 90[ 	]+movw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	8c 90 90 90 90 90[ 	]+movw[ 	]+%ss,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 8d 90 90 90 90 90[ 	]+leaw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 8f 80 90 90 90 90[ 	]+popw[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 91[ 	]+xchgw[ 	]+%ax,%cx
 *[0-9a-f]+:	66 92[ 	]+xchgw[ 	]+%ax,%dx
 *[0-9a-f]+:	66 93[ 	]+xchgw[ 	]+%ax,%bx
 *[0-9a-f]+:	66 94[ 	]+xchgw[ 	]+%ax,%sp
 *[0-9a-f]+:	66 95[ 	]+xchgw[ 	]+%ax,%bp
 *[0-9a-f]+:	66 96[ 	]+xchgw[ 	]+%ax,%si
 *[0-9a-f]+:	66 97[ 	]+xchgw[ 	]+%ax,%di
 *[0-9a-f]+:	66 98[ 	]+cbtw[ 	]+
 *[0-9a-f]+:	66 99[ 	]+cwtd[ 	]+
 *[0-9a-f]+:	66 9a 90 90 90 90[ 	]+lcallw \$0x9090,\$0x9090
 *[0-9a-f]+:	66 9c[ 	]+pushfw 
 *[0-9a-f]+:	66 9d[ 	]+popfw[ 	]+
 *[0-9a-f]+:	66 a1 90 90 90 90[ 	]+movw[ 	]+0x90909090,%ax
 *[0-9a-f]+:	66 a3 90 90 90 90[ 	]+movw[ 	]+%ax,0x90909090
 *[0-9a-f]+:	66 a5[ 	]+movsw[ 	]+%ds:\(%esi\),%es:\(%edi\)
 *[0-9a-f]+:	66 a7[ 	]+cmpsw[ 	]+%es:\(%edi\),%ds:\(%esi\)
 *[0-9a-f]+:	66 a9 90 90[ 	]+testw[ 	]+\$0x9090,%ax
 *[0-9a-f]+:	66 ab[ 	]+stosw[ 	]+%ax,%es:\(%edi\)
 *[0-9a-f]+:	66 ad[ 	]+lodsw[ 	]+%ds:\(%esi\),%ax
 *[0-9a-f]+:	66 af[ 	]+scasw[ 	]+%es:\(%edi\),%ax
 *[0-9a-f]+:	66 b8 90 90[ 	]+movw[ 	]+\$0x9090,%ax
 *[0-9a-f]+:	66 b9 90 90[ 	]+movw[ 	]+\$0x9090,%cx
 *[0-9a-f]+:	66 ba 90 90[ 	]+movw[ 	]+\$0x9090,%dx
 *[0-9a-f]+:	66 bb 90 90[ 	]+movw[ 	]+\$0x9090,%bx
 *[0-9a-f]+:	66 bc 90 90[ 	]+movw[ 	]+\$0x9090,%sp
 *[0-9a-f]+:	66 bd 90 90[ 	]+movw[ 	]+\$0x9090,%bp
 *[0-9a-f]+:	66 be 90 90[ 	]+movw[ 	]+\$0x9090,%si
 *[0-9a-f]+:	66 bf 90 90[ 	]+movw[ 	]+\$0x9090,%di
 *[0-9a-f]+:	66 c1 90 90 90 90 90 90[ 	]+rclw[ 	]+\$0x90,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 c2 90 90[ 	]+retw[ 	]+\$0x9090
 *[0-9a-f]+:	66 c3[ 	]+retw[ 	]+
 *[0-9a-f]+:	66 c4 90 90 90 90 90[ 	]+lesw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 c5 90 90 90 90 90[ 	]+ldsw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 c7 80 90 90 90 90 90 90[ 	]+movw[ 	]+\$0x9090,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 c8 90 90 90[ 	]+enterw \$0x9090,\$0x90
 *[0-9a-f]+:	66 c9[ 	]+leavew 
 *[0-9a-f]+:	66 ca 90 90[ 	]+lretw[ 	]+\$0x9090
 *[0-9a-f]+:	66 cb[ 	]+lretw[ 	]+
 *[0-9a-f]+:	66 cf[ 	]+iretw[ 	]+
 *[0-9a-f]+:	66 d1 90 90 90 90 90[ 	]+rclw[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 d3 90 90 90 90 90[ 	]+rclw[ 	]+%cl,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 e5 90[ 	]+inw[ 	]+\$0x90,%ax
 *[0-9a-f]+:	66 e7 90[ 	]+outw[ 	]+%ax,\$0x90
 *[0-9a-f]+:	66 e8 8f 90[ 	]+callw[ 	]+(0x)?9918.*
 *[0-9a-f]+:	66 ea 90 90 90 90[ 	]+ljmpw[ 	]+\$0x9090,\$0x9090
 *[0-9a-f]+:	66 ed[ 	]+inw[ 	]+\(%dx\),%ax
 *[0-9a-f]+:	66 ef[ 	]+outw[ 	]+%ax,\(%dx\)
 *[0-9a-f]+:	66 f7 90 90 90 90 90[ 	]+notw[ 	]+-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 ff 90 90 90 90 90[ 	]+callw[ 	]+\*-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 0f 02 90 90 90 90 90[ 	]+larw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f 03 90 90 90 90 90[ 	]+lslw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f 40 90 90 90 90 90[ 	]+cmovow[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f 41 90 90 90 90 90[ 	]+cmovnow -0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f 42 90 90 90 90 90[ 	]+cmovbw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f 43 90 90 90 90 90[ 	]+cmovaew -0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f 44 90 90 90 90 90[ 	]+cmovew[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f 45 90 90 90 90 90[ 	]+cmovnew -0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f 46 90 90 90 90 90[ 	]+cmovbew -0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f 47 90 90 90 90 90[ 	]+cmovaw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f 48 90 90 90 90 90[ 	]+cmovsw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f 49 90 90 90 90 90[ 	]+cmovnsw -0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f 4a 90 90 90 90 90[ 	]+cmovpw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f 4b 90 90 90 90 90[ 	]+cmovnpw -0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f 4c 90 90 90 90 90[ 	]+cmovlw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f 4d 90 90 90 90 90[ 	]+cmovgew -0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f 4e 90 90 90 90 90[ 	]+cmovlew -0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f 4f 90 90 90 90 90[ 	]+cmovgw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f a0[ 	]+pushw[ 	]+%fs
 *[0-9a-f]+:	66 0f a1[ 	]+popw[ 	]+%fs
 *[0-9a-f]+:	66 0f a3 90 90 90 90 90[ 	]+btw[ 	]+%dx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 0f a4 90 90 90 90 90 90[ 	]+shldw[ 	]+\$0x90,%dx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 0f a5 90 90 90 90 90[ 	]+shldw[ 	]+%cl,%dx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 0f a8[ 	]+pushw[ 	]+%gs
 *[0-9a-f]+:	66 0f a9[ 	]+popw[ 	]+%gs
 *[0-9a-f]+:	66 0f ab 90 90 90 90 90[ 	]+btsw[ 	]+%dx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 0f ac 90 90 90 90 90 90[ 	]+shrdw[ 	]+\$0x90,%dx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 0f ad 90 90 90 90 90[ 	]+shrdw[ 	]+%cl,%dx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 0f af 90 90 90 90 90[ 	]+imulw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f b1 90 90 90 90 90[ 	]+cmpxchgw %dx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 0f b2 90 90 90 90 90[ 	]+lssw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f b3 90 90 90 90 90[ 	]+btrw[ 	]+%dx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 0f b4 90 90 90 90 90[ 	]+lfsw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f b5 90 90 90 90 90[ 	]+lgsw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f b6 90 90 90 90 90[ 	]+movzbw -0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f bb 90 90 90 90 90[ 	]+btcw[ 	]+%dx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 0f bc 90 90 90 90 90[ 	]+bsfw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f bd 90 90 90 90 90[ 	]+bsrw[ 	]+-0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f be 90 90 90 90 90[ 	]+movsbw -0x6f6f6f70\(%eax\),%dx
 *[0-9a-f]+:	66 0f c1 90 90 90 90 90[ 	]+xaddw[ 	]+%dx,-0x6f6f6f70\(%eax\)
 *[0-9a-f]+:	66 90[ 	]+xchgw[ 	]+%ax,%ax
 *[0-9a-f]+:	0f 00 c0[ 	]+sldtl[ 	]+%eax
 *[0-9a-f]+:	66 0f 00 c0[ 	]+sldtw[ 	]+%ax
 *[0-9a-f]+:	0f 00 00[ 	]+sldtw[ 	]+\(%eax\)
 *[0-9a-f]+:	0f 01 e0[ 	]+smswl[ 	]+%eax
 *[0-9a-f]+:	66 0f 01 e0[ 	]+smsww[ 	]+%ax
 *[0-9a-f]+:	0f 01 20[ 	]+smsww[ 	]+\(%eax\)
 *[0-9a-f]+:	0f 00 c8[ 	]+strl[ 	]+%eax
 *[0-9a-f]+:	66 0f 00 c8[ 	]+strw[ 	]+%ax
 *[0-9a-f]+:	0f 00 08[ 	]+strw[ 	]+\(%eax\)
 *[0-9a-f]+:	0f ad d0 [ 	]*shrdl[ 	]+%cl,%edx,%eax
 *[0-9a-f]+:	0f a5 d0 [ 	]*shldl[ 	]+%cl,%edx,%eax
 *[0-9a-f]+:	85 c3 [ 	]*testl[ 	]+%eax,%ebx
 *[0-9a-f]+:	85 d8 [ 	]*testl[ 	]+%ebx,%eax
 *[0-9a-f]+:	85 18 [ 	]*testl[ 	]+%ebx,\(%eax\)
[ 	]*[a-f0-9]+:	0f 4a 90 90 90 90 90 	cmovpl -0x6f6f6f70\(%eax\),%edx
[ 	]*[a-f0-9]+:	0f 4b 90 90 90 90 90 	cmovnpl -0x6f6f6f70\(%eax\),%edx
[ 	]*[a-f0-9]+:	66 0f 4a 90 90 90 90 90 	cmovpw -0x6f6f6f70\(%eax\),%dx
[ 	]*[a-f0-9]+:	66 0f 4b 90 90 90 90 90 	cmovnpw -0x6f6f6f70\(%eax\),%dx
#pass
