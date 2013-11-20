bfd/

2013-12-01  Igor Zamyatin  <igor.zamyatin@intel.com>
	    H.J. Lu  <hongjiu.lu@intel.com>

	* elf64-x86-64.c (elf_x86_64_bnd_plt0_entry): New.
	(elf_x86_64_legacy_plt_entry): Likewise.
	(elf_x86_64_bnd_plt_entry): Likewise.
	(elf_x86_64_legacy_plt2_entry): Likewise.
	(elf_x86_64_bnd_plt2_entry): Likewise.
	(elf_x86_64_bnd_arch_bed): Likewise.
	(elf_x86_64_link_hash_entry): Add has_bnd_reloc and plt_bnd.
	(elf_x86_64_link_hash_table): Add plt_bnd.
	(elf_x86_64_link_hash_newfunc): Initialize has_bnd_reloc and
	plt_bnd.
	(elf_x86_64_copy_indirect_symbol): Also copy has_bnd_reloc.
	(elf_x86_64_check_relocs): Create the second PLT for Intel MPX
	in 64-bit mode.
	(elf_x86_64_allocate_dynrelocs): Handle the second PLT for IFUNC
	symbols.  Resolve call to the second PLT if it is created.
	(elf_x86_64_size_dynamic_sections): Keep the second PLT section.
	(elf_x86_64_relocate_section): Resolve PLT references to the
	second PLT if it is created.
	(elf_x86_64_finish_dynamic_symbol): Use BND PLT0 and fill the
	second PLT entry for BND relocation.
	(elf_x86_64_finish_dynamic_sections): Use MPX backend data if
	the second PLT is created.
	(elf_x86_64_get_synthetic_symtab): New.
	(bfd_elf64_get_synthetic_symtab): Likewise.  Undefine for NaCl.

ld/

2013-12-01  Igor Zamyatin  <igor.zamyatin@intel.com>
	    H.J. Lu  <hongjiu.lu@intel.com>

	* emulparams/elf_x86_64.sh (TINY_READONLY_SECTION): New.

ld/testsuite/

2013-12-01  Igor Zamyatin  <igor.zamyatin@intel.com>
	    H.J. Lu  <hongjiu.lu@intel.com>

	* ld-x86-64/mpx.exp: Run bnd-ifunc-1 and bnd-plt-1.
	* ld-x86-64/bnd-ifunc-1.d: New file.
	* ld-x86-64/bnd-ifunc-1.s: Likewise.
	* ld-x86-64/bnd-plt-1.d: Likewise.
