// i386.cc -- i386 target support for gold.

#include "gold.h"

#include <cstring>

#include "elfcpp.h"
#include "reloc.h"
#include "i386.h"
#include "object.h"
#include "symtab.h"
#include "layout.h"
#include "output.h"
#include "target.h"
#include "target-reloc.h"
#include "target-select.h"

namespace
{

using namespace gold;

class Output_data_plt_i386;

// The i386 target class.

class Target_i386 : public Sized_target<32, false>
{
 public:
  Target_i386()
    : Sized_target<32, false>(&i386_info),
      got_(NULL), plt_(NULL), got_plt_(NULL)
  { }

  // Scan the relocations to look for symbol adjustments.
  void
  scan_relocs(const General_options& options,
	      Symbol_table* symtab,
	      Layout* layout,
	      Sized_relobj<32, false>* object,
	      unsigned int data_shndx,
	      unsigned int sh_type,
	      const unsigned char* prelocs,
	      size_t reloc_count,
	      size_t local_symbol_count,
	      const unsigned char* plocal_symbols,
	      Symbol** global_symbols);

  // Relocate a section.
  void
  relocate_section(const Relocate_info<32, false>*,
		   unsigned int sh_type,
		   const unsigned char* prelocs,
		   size_t reloc_count,
		   unsigned char* view,
		   elfcpp::Elf_types<32>::Elf_Addr view_address,
		   off_t view_size);

 private:
  // The class which scans relocations.
  struct Scan
  {
    inline void
    local(const General_options& options, Symbol_table* symtab,
	  Layout* layout, Target_i386* target,
	  Sized_relobj<32, false>* object,
	  unsigned int data_shndx,
	  const elfcpp::Rel<32, false>& reloc, unsigned int r_type,
	  const elfcpp::Sym<32, false>& lsym);

    inline void
    global(const General_options& options, Symbol_table* symtab,
	   Layout* layout, Target_i386* target,
	   Sized_relobj<32, false>* object,
	   unsigned int data_shndx,
	   const elfcpp::Rel<32, false>& reloc, unsigned int r_type,
	   Symbol* gsym);
  };

  // The class which implements relocation.
  class Relocate
  {
   public:
    Relocate()
      : skip_call_tls_get_addr_(false)
    { }

    ~Relocate()
    {
      if (this->skip_call_tls_get_addr_)
	{
	  // FIXME: This needs to specify the location somehow.
	  fprintf(stderr, _("%s: missing expected TLS relocation\n"),
		  program_name);
	  gold_exit(false);
	}
    }

    // Do a relocation.  Return false if the caller should not issue
    // any warnings about this relocation.
    inline bool
    relocate(const Relocate_info<32, false>*, Target_i386*, size_t relnum,
	     const elfcpp::Rel<32, false>&,
	     unsigned int r_type, const Sized_symbol<32>*,
	     elfcpp::Elf_types<32>::Elf_Addr,
	     unsigned char*, elfcpp::Elf_types<32>::Elf_Addr,
	     off_t);

   private:
    // Do a TLS relocation.
    inline void
    relocate_tls(const Relocate_info<32, false>*, size_t relnum,
		 const elfcpp::Rel<32, false>&,
		 unsigned int r_type, const Sized_symbol<32>*,
		 elfcpp::Elf_types<32>::Elf_Addr,
		 unsigned char*, elfcpp::Elf_types<32>::Elf_Addr, off_t);

    // Do a TLS Initial-Exec to Local-Exec transition.
    static inline void
    tls_ie_to_le(const Relocate_info<32, false>*, size_t relnum,
		 Output_segment* tls_segment,
		 const elfcpp::Rel<32, false>&, unsigned int r_type,
		 elfcpp::Elf_types<32>::Elf_Addr value,
		 unsigned char* view,
		 off_t view_size);

    // Do a TLS Global-Dynamic to Local-Exec transition.
    inline void
    tls_gd_to_le(const Relocate_info<32, false>*, size_t relnum,
		 Output_segment* tls_segment,
		 const elfcpp::Rel<32, false>&, unsigned int r_type,
		 elfcpp::Elf_types<32>::Elf_Addr value,
		 unsigned char* view,
		 off_t view_size);

    // Check the range for a TLS relocation.
    static inline void
    check_range(const Relocate_info<32, false>*, size_t relnum,
		const elfcpp::Rel<32, false>&, off_t, off_t);

    // Check the validity of a TLS relocation.  This is like assert.
    static inline void
    check_tls(const Relocate_info<32, false>*, size_t relnum,
	      const elfcpp::Rel<32, false>&, bool);

    // This is set if we should skip the next reloc, which should be a
    // PLT32 reloc against ___tls_get_addr.
    bool skip_call_tls_get_addr_;
  };

  // Adjust TLS relocation type based on the options and whether this
  // is a local symbol.
  static unsigned int
  optimize_tls_reloc(const General_options*, bool is_final, int r_type);

  // Get the GOT section, creating it if necessary.
  Output_data_got<32, false>*
  got_section(const General_options*, Symbol_table*, Layout*);

  // Create a PLT entry for a global symbol.
  void
  make_plt_entry(const General_options* options, Symbol_table*,
		 Layout*, Symbol*);

  // Get the PLT section.
  Output_data_plt_i386*
  plt_section() const
  {
    gold_assert(this->plt_ != NULL);
    return this->plt_;
  }

  // Copy a relocation against a global symbol.
  void
  copy_reloc(const General_options*, Sized_relobj<32, false>*, unsigned int,
	     Symbol*, const elfcpp::Rel<32, false>&);

  // Information about this specific target which we pass to the
  // general Target structure.
  static const Target::Target_info i386_info;

  // The GOT section.
  Output_data_got<32, false>* got_;
  // The PLT section.
  Output_data_plt_i386* plt_;
  // The GOT PLT section.
  Output_data_space* got_plt_;
};

const Target::Target_info Target_i386::i386_info =
{
  32,			// size
  false,		// is_big_endian
  elfcpp::EM_386,	// machine_code
  false,		// has_make_symbol
  false,		// has_resolve
  "/usr/lib/libc.so.1",	// dynamic_linker
  0x08048000,		// text_segment_address
  0x1000,		// abi_pagesize
  0x1000		// common_pagesize
};

// Get the GOT section, creating it if necessary.

Output_data_got<32, false>*
Target_i386::got_section(const General_options* options, Symbol_table* symtab,
			 Layout* layout)
{
  if (this->got_ == NULL)
    {
      gold_assert(options != NULL && symtab != NULL && layout != NULL);

      this->got_ = new Output_data_got<32, false>(options);

      layout->add_output_section_data(".got", elfcpp::SHT_PROGBITS,
				      elfcpp::SHF_ALLOC, this->got_);

      // The old GNU linker creates a .got.plt section.  We just
      // create another set of data in the .got section.  Note that we
      // always create a PLT if we create a GOT, although the PLT
      // might be empty.
      this->got_plt_ = new Output_data_space(4);
      layout->add_output_section_data(".got", elfcpp::SHT_PROGBITS,
				      elfcpp::SHF_ALLOC, this->got_plt_);

      // The first three entries are reserved.
      this->got_plt_->set_space_size(3 * 4);

      // Define _GLOBAL_OFFSET_TABLE_ at the start of the PLT.
      symtab->define_in_output_data(this, "_GLOBAL_OFFSET_TABLE_",
				    this->got_plt_,
				    0, 0, elfcpp::STT_OBJECT,
				    elfcpp::STB_GLOBAL,
				    elfcpp::STV_HIDDEN, 0,
				    false, false);
    }

  return this->got_;
}

// A class to handle the PLT data.

class Output_data_plt_i386 : public Output_section_data
{
 public:
  typedef Output_data_reloc<elfcpp::SHT_REL, true, 32, false> Reloc_section;

  Output_data_plt_i386(Layout*, Output_data_space*, bool is_shared);

  // Add an entry to the PLT.
  void
  add_entry(Symbol* gsym);

 private:
  // The size of an entry in the PLT.
  static const int plt_entry_size = 16;

  // The first entry in the PLT for an executable.
  static unsigned char exec_first_plt_entry[plt_entry_size];

  // The first entry in the PLT for a shared object.
  static unsigned char dyn_first_plt_entry[plt_entry_size];

  // Other entries in the PLT for an executable.
  static unsigned char exec_plt_entry[plt_entry_size];

  // Other entries in the PLT for a shared object.
  static unsigned char dyn_plt_entry[plt_entry_size];

  // Set the final size.
  void
  do_set_address(uint64_t, off_t)
  { this->set_data_size((this->count_ + 1) * plt_entry_size); }

  // Write out the PLT data.
  void
  do_write(Output_file*);

  // The reloc section.
  Reloc_section* rel_;
  // The .got.plt section.
  Output_data_space* got_plt_;
  // The number of PLT entries.
  unsigned int count_;
  // Whether we are generated a shared object.
  bool is_shared_;
};

// Create the PLT section.  The ordinary .got section is an argument,
// since we need to refer to the start.  We also create our own .got
// section just for PLT entries.

Output_data_plt_i386::Output_data_plt_i386(Layout* layout,
					   Output_data_space* got_plt,
					   bool is_shared)
  : Output_section_data(4), got_plt_(got_plt), is_shared_(is_shared)
{
  this->rel_ = new Reloc_section();
  layout->add_output_section_data(".rel.plt", elfcpp::SHT_REL,
				  elfcpp::SHF_ALLOC, this->rel_);
}

// Add an entry to the PLT.

void
Output_data_plt_i386::add_entry(Symbol* gsym)
{
  gold_assert(!gsym->has_plt_offset());

  // Note that when setting the PLT offset we skip the initial
  // reserved PLT entry.
  gsym->set_plt_offset((this->count_ + 1) * plt_entry_size);

  ++this->count_;

  off_t got_offset = this->got_plt_->data_size();

  // Every PLT entry needs a GOT entry which points back to the PLT
  // entry (this will be changed by the dynamic linker, normally
  // lazily when the function is called).
  this->got_plt_->set_space_size(got_offset + 4);

  // Every PLT entry needs a reloc.
  this->rel_->add_global(gsym, elfcpp::R_386_JUMP_SLOT, this->got_plt_,
			 got_offset);

  // Note that we don't need to save the symbol.  The contents of the
  // PLT are independent of which symbols are used.  The symbols only
  // appear in the relocations.
}

// The first entry in the PLT for an executable.

unsigned char Output_data_plt_i386::exec_first_plt_entry[plt_entry_size] =
{
  0xff, 0x35,	// pushl contents of memory address
  0, 0, 0, 0,	// replaced with address of .got + 4
  0xff, 0x25,	// jmp indirect
  0, 0, 0, 0,	// replaced with address of .got + 8
  0, 0, 0, 0	// unused
};

// The first entry in the PLT for a shared object.

unsigned char Output_data_plt_i386::dyn_first_plt_entry[plt_entry_size] =
{
  0xff, 0xb3, 4, 0, 0, 0,	// pushl 4(%ebx)
  0xff, 0xa3, 8, 0, 0, 0,	// jmp *8(%ebx)
  0, 0, 0, 0			// unused
};

// Subsequent entries in the PLT for an executable.

unsigned char Output_data_plt_i386::exec_plt_entry[plt_entry_size] =
{
  0xff, 0x25,	// jmp indirect
  0, 0, 0, 0,	// replaced with address of symbol in .got
  0x68,		// pushl immediate
  0, 0, 0, 0,	// replaced with offset into relocation table
  0xe9,		// jmp relative
  0, 0, 0, 0	// replaced with offset to start of .plt
};

// Subsequent entries in the PLT for a shared object.

unsigned char Output_data_plt_i386::dyn_plt_entry[plt_entry_size] =
{
  0xff, 0xa3,	// jmp *offset(%ebx)
  0, 0, 0, 0,	// replaced with offset of symbol in .got
  0x68,		// pushl immediate
  0, 0, 0, 0,	// replaced with offset into relocation table
  0xe9,		// jmp relative
  0, 0, 0, 0	// replaced with offset to start of .plt
};

// Write out the PLT.  This uses the hand-coded instructions above,
// and adjusts them as needed.  This is all specified by the i386 ELF
// Processor Supplement.

void
Output_data_plt_i386::do_write(Output_file* of)
{
  const off_t offset = this->offset();
  const off_t oview_size = this->data_size();
  unsigned char* const oview = of->get_output_view(offset, oview_size);

  const off_t got_file_offset = this->got_plt_->offset();
  const off_t got_size = this->got_plt_->data_size();
  unsigned char* const got_view = of->get_output_view(got_file_offset,
						      got_size);

  unsigned char* pov = oview;

  elfcpp::Elf_types<32>::Elf_Addr plt_address = this->address();
  elfcpp::Elf_types<32>::Elf_Addr got_address = this->got_plt_->address();

  if (this->is_shared_)
    memcpy(pov, dyn_first_plt_entry, plt_entry_size);
  else
    {
      memcpy(pov, exec_first_plt_entry, plt_entry_size);
      elfcpp::Swap_unaligned<32, false>::writeval(pov + 2, got_address + 4);
      elfcpp::Swap<32, false>::writeval(pov + 8, got_address + 8);
    }
  pov += plt_entry_size;

  unsigned char* got_pov = got_view;

  memset(got_pov, 0, 12);
  got_pov += 12;

  const int rel_size = elfcpp::Elf_sizes<32>::rel_size;

  unsigned int plt_offset = plt_entry_size;
  unsigned int plt_rel_offset = 0;
  unsigned int got_offset = 12;
  const unsigned int count = this->count_;
  for (unsigned int i = 0;
       i < count;
       ++i,
	 pov += plt_entry_size,
	 got_pov += 4,
	 plt_offset += plt_entry_size,
	 plt_rel_offset += rel_size,
	 got_offset += 4)
    {
      // Set and adjust the PLT entry itself.

      if (this->is_shared_)
	{
	  memcpy(pov, dyn_plt_entry, plt_entry_size);
	  elfcpp::Swap_unaligned<32, false>::writeval(pov + 2, got_offset);
	}
      else
	{
	  memcpy(pov, exec_plt_entry, plt_entry_size);
	  elfcpp::Swap_unaligned<32, false>::writeval(pov + 2,
						      (got_address
						       + got_offset));
	}

      elfcpp::Swap_unaligned<32, false>::writeval(pov + 7, plt_rel_offset);
      elfcpp::Swap<32, false>::writeval(pov + 12,
					- (plt_offset + plt_entry_size));

      // Set the entry in the GOT.
      elfcpp::Swap<32, false>::writeval(got_pov, plt_address + plt_offset + 6);
    }

  gold_assert(pov - oview == oview_size);
  gold_assert(got_pov - got_view == got_size);

  of->write_output_view(offset, oview_size, oview);
  of->write_output_view(got_file_offset, got_size, got_view);
}

// Create a PLT entry for a global symbol.

void
Target_i386::make_plt_entry(const General_options* options,
			    Symbol_table* symtab, Layout* layout, Symbol* gsym)
{
  if (gsym->has_plt_offset())
    return;

  if (this->plt_ == NULL)
    {
      // Create the GOT sections first.
      this->got_section(options, symtab, layout);

      this->plt_ = new Output_data_plt_i386(layout, this->got_plt_,
					    options->is_shared());
    }

  this->plt_->add_entry(gsym);
}

// Handle a relocation against a non-function symbol defined in a
// dynamic object.  The traditional way to handle this is to generate
// a COPY relocation to copy the variable at runtime from the shared
// object into the executable's data segment.  However, this is
// undesirable in general, as if the size of the object changes in the
// dynamic object, the executable will no longer work correctly.  If
// this relocation is in a writable section, then we can create a
// dynamic reloc and the dynamic linker will resolve it to the correct
// address at runtime.  However, we do not want do that if the
// relocation is in a read-only section, as it would prevent the
// readonly segment from being shared.  And if we have to eventually
// generate a COPY reloc, then any dynamic relocations will be
// useless.  So this means that if this is a writable section, we need
// to save the relocation until we see whether we have to create a
// COPY relocation for this symbol for any other relocation.

void
Target_i386::copy_reloc(const General_options* options,
			Sized_relobj<32, false>* object,
			unsigned int data_shndx, Symbol* gsym,
			const elfcpp::Rel<32, false>&)
{
  if (!Relocate_functions<32, false>::need_copy_reloc(options, object,
						      data_shndx, gsym))
    {
      // So far we do not need a COPY reloc.  Save this relocation.
      // If it turns out that we never a COPY reloc for this symbol,
      // then we emit the relocation.
    }

}

// Optimize the TLS relocation type based on what we know about the
// symbol.  IS_FINAL is true if the final address of this symbol is
// known at link time.

unsigned int
Target_i386::optimize_tls_reloc(const General_options* options,
				bool is_final,
				int r_type)
{
  // If we are generating a shared library, then we can't do anything
  // in the linker.
  if (options->is_shared())
    return r_type;

  switch (r_type)
    {
    case elfcpp::R_386_TLS_GD:
    case elfcpp::R_386_TLS_GOTDESC:
    case elfcpp::R_386_TLS_DESC_CALL:
      // These are Global-Dynamic which permits fully general TLS
      // access.  Since we know that we are generating an executable,
      // we can convert this to Initial-Exec.  If we also know that
      // this is a local symbol, we can further switch to Local-Exec.
      if (is_final)
	return elfcpp::R_386_TLS_LE_32;
      return elfcpp::R_386_TLS_IE_32;

    case elfcpp::R_386_TLS_LDM:
      // This is Local-Dynamic, which refers to a local symbol in the
      // dynamic TLS block.  Since we know that we generating an
      // executable, we can switch to Local-Exec.
      return elfcpp::R_386_TLS_LE_32;

    case elfcpp::R_386_TLS_LDO_32:
      // Another type of Local-Dynamic relocation.
      return elfcpp::R_386_TLS_LE;

    case elfcpp::R_386_TLS_IE:
    case elfcpp::R_386_TLS_GOTIE:
    case elfcpp::R_386_TLS_IE_32:
      // These are Initial-Exec relocs which get the thread offset
      // from the GOT.  If we know that we are linking against the
      // local symbol, we can switch to Local-Exec, which links the
      // thread offset into the instruction.
      if (is_final)
	return elfcpp::R_386_TLS_LE_32;
      return r_type;
	
    case elfcpp::R_386_TLS_LE:
    case elfcpp::R_386_TLS_LE_32:
      // When we already have Local-Exec, there is nothing further we
      // can do.
      return r_type;

    default:
      gold_unreachable();
    }
}

// Scan a relocation for a local symbol.

inline void
Target_i386::Scan::local(const General_options& options,
			 Symbol_table* symtab,
			 Layout* layout,
			 Target_i386* target,
			 Sized_relobj<32, false>* object,
			 unsigned int,
			 const elfcpp::Rel<32, false>&,
			 unsigned int r_type,
			 const elfcpp::Sym<32, false>&)
{
  switch (r_type)
    {
    case elfcpp::R_386_NONE:
    case elfcpp::R_386_GNU_VTINHERIT:
    case elfcpp::R_386_GNU_VTENTRY:
      break;

    case elfcpp::R_386_32:
    case elfcpp::R_386_16:
    case elfcpp::R_386_8:
      // FIXME: If we are generating a shared object we need to copy
      // this relocation into the object.
      gold_assert(!options.is_shared());
      break;

    case elfcpp::R_386_PC32:
    case elfcpp::R_386_PC16:
    case elfcpp::R_386_PC8:
      break;

    case elfcpp::R_386_GOTOFF:
    case elfcpp::R_386_GOTPC:
      // We need a GOT section.
      target->got_section(&options, symtab, layout);
      break;

    case elfcpp::R_386_COPY:
    case elfcpp::R_386_GLOB_DAT:
    case elfcpp::R_386_JUMP_SLOT:
    case elfcpp::R_386_RELATIVE:
    case elfcpp::R_386_TLS_TPOFF:
    case elfcpp::R_386_TLS_DTPMOD32:
    case elfcpp::R_386_TLS_DTPOFF32:
    case elfcpp::R_386_TLS_TPOFF32:
    case elfcpp::R_386_TLS_DESC:
      fprintf(stderr, _("%s: %s: unexpected reloc %u in object file\n"),
	      program_name, object->name().c_str(), r_type);
      gold_exit(false);
      break;

    case elfcpp::R_386_TLS_IE:
    case elfcpp::R_386_TLS_GOTIE:
    case elfcpp::R_386_TLS_LE:
    case elfcpp::R_386_TLS_GD:
    case elfcpp::R_386_TLS_LDM:
    case elfcpp::R_386_TLS_LDO_32:
    case elfcpp::R_386_TLS_IE_32:
    case elfcpp::R_386_TLS_LE_32:
    case elfcpp::R_386_TLS_GOTDESC:
    case elfcpp::R_386_TLS_DESC_CALL:
      r_type = Target_i386::optimize_tls_reloc(&options,
					       !options.is_shared(),
					       r_type);
      switch (r_type)
	{
	case elfcpp::R_386_TLS_LE:
	case elfcpp::R_386_TLS_LE_32:
	  // FIXME: If generating a shared object, we need to copy
	  // this relocation into the object.
	  gold_assert(!options.is_shared());
	  break;

	case elfcpp::R_386_TLS_IE:
	case elfcpp::R_386_TLS_GOTIE:
	case elfcpp::R_386_TLS_GD:
	case elfcpp::R_386_TLS_LDM:
	case elfcpp::R_386_TLS_LDO_32:
	case elfcpp::R_386_TLS_IE_32:
	case elfcpp::R_386_TLS_GOTDESC:
	case elfcpp::R_386_TLS_DESC_CALL:
	  fprintf(stderr,
		  _("%s: %s: unsupported reloc %u against local symbol\n"),
		  program_name, object->name().c_str(), r_type);
	  break;
	}
      break;

    case elfcpp::R_386_GOT32:
    case elfcpp::R_386_PLT32:
    case elfcpp::R_386_32PLT:
    case elfcpp::R_386_TLS_GD_32:
    case elfcpp::R_386_TLS_GD_PUSH:
    case elfcpp::R_386_TLS_GD_CALL:
    case elfcpp::R_386_TLS_GD_POP:
    case elfcpp::R_386_TLS_LDM_32:
    case elfcpp::R_386_TLS_LDM_PUSH:
    case elfcpp::R_386_TLS_LDM_CALL:
    case elfcpp::R_386_TLS_LDM_POP:
    case elfcpp::R_386_USED_BY_INTEL_200:
    default:
      fprintf(stderr, _("%s: %s: unsupported reloc %u against local symbol\n"),
	      program_name, object->name().c_str(), r_type);
      break;
    }
}

// Scan a relocation for a global symbol.

inline void
Target_i386::Scan::global(const General_options& options,
			  Symbol_table* symtab,
			  Layout* layout,
			  Target_i386* target,
			  Sized_relobj<32, false>* object,
			  unsigned int data_shndx,
			  const elfcpp::Rel<32, false>& reloc,
			  unsigned int r_type,
			  Symbol* gsym)
{
  switch (r_type)
    {
    case elfcpp::R_386_NONE:
    case elfcpp::R_386_GNU_VTINHERIT:
    case elfcpp::R_386_GNU_VTENTRY: 
      break;

    case elfcpp::R_386_32:
    case elfcpp::R_386_PC32:
    case elfcpp::R_386_16:
    case elfcpp::R_386_PC16:
    case elfcpp::R_386_8:
    case elfcpp::R_386_PC8:
      // FIXME: If we are generating a shared object we may need to
      // copy this relocation into the object.  If this symbol is
      // defined in a shared object, we may need to copy this
      // relocation in order to avoid a COPY relocation.
      gold_assert(!options.is_shared());

      if (gsym->is_defined_in_dynobj())
	{
	  // This symbol is defined in a dynamic object.  If it is a
	  // function, we make a PLT entry.  Otherwise we need to
	  // either generate a COPY reloc or copy this reloc.
	  if (gsym->type() == elfcpp::STT_FUNC)
	    target->make_plt_entry(&options, symtab, layout, gsym);
	  else
	    target->copy_reloc(&options, object, data_shndx, gsym, reloc);
	}

      break;

    case elfcpp::R_386_GOT32:
      // The symbol requires a GOT entry.
      if (target->got_section(&options, symtab, layout)->add_global(gsym))
	{
	  // If this symbol is not fully resolved, we need to add a
	  // dynamic relocation for it.
	  if (!gsym->final_value_is_known(&options))
	    gold_unreachable();
	}
      break;

    case elfcpp::R_386_PLT32:
      // If the symbol is fully resolved, this is just a PC32 reloc.
      // Otherwise we need a PLT entry.
      if (gsym->final_value_is_known(&options))
	break;
      target->make_plt_entry(&options, symtab, layout, gsym);
      break;

    case elfcpp::R_386_GOTOFF:
    case elfcpp::R_386_GOTPC:
      // We need a GOT section.
      target->got_section(&options, symtab, layout);
      break;

    case elfcpp::R_386_COPY:
    case elfcpp::R_386_GLOB_DAT:
    case elfcpp::R_386_JUMP_SLOT:
    case elfcpp::R_386_RELATIVE:
    case elfcpp::R_386_TLS_TPOFF:
    case elfcpp::R_386_TLS_DTPMOD32:
    case elfcpp::R_386_TLS_DTPOFF32:
    case elfcpp::R_386_TLS_TPOFF32:
    case elfcpp::R_386_TLS_DESC:
      fprintf(stderr, _("%s: %s: unexpected reloc %u in object file\n"),
	      program_name, object->name().c_str(), r_type);
      gold_exit(false);
      break;

    case elfcpp::R_386_TLS_IE:
    case elfcpp::R_386_TLS_GOTIE:
    case elfcpp::R_386_TLS_LE:
    case elfcpp::R_386_TLS_GD:
    case elfcpp::R_386_TLS_LDM:
    case elfcpp::R_386_TLS_LDO_32:
    case elfcpp::R_386_TLS_IE_32:
    case elfcpp::R_386_TLS_LE_32:
    case elfcpp::R_386_TLS_GOTDESC:
    case elfcpp::R_386_TLS_DESC_CALL:
      {
	const bool is_final = gsym->final_value_is_known(&options);
	r_type = Target_i386::optimize_tls_reloc(&options, is_final, r_type);
	switch (r_type)
	  {
	  case elfcpp::R_386_TLS_LE:
	  case elfcpp::R_386_TLS_LE_32:
	    // FIXME: If generating a shared object, we need to copy
	    // this relocation into the object.
	    gold_assert(!options.is_shared());
	    break;

	  case elfcpp::R_386_TLS_IE:
	  case elfcpp::R_386_TLS_GOTIE:
	  case elfcpp::R_386_TLS_GD:
	  case elfcpp::R_386_TLS_LDM:
	  case elfcpp::R_386_TLS_LDO_32:
	  case elfcpp::R_386_TLS_IE_32:
	  case elfcpp::R_386_TLS_GOTDESC:
	  case elfcpp::R_386_TLS_DESC_CALL:
	    fprintf(stderr,
		    _("%s: %s: unsupported reloc %u "
		      "against global symbol %s\n"),
		    program_name, object->name().c_str(), r_type,
		    gsym->name());
	    break;
	  }
      }
      break;

    case elfcpp::R_386_32PLT:
    case elfcpp::R_386_TLS_GD_32:
    case elfcpp::R_386_TLS_GD_PUSH:
    case elfcpp::R_386_TLS_GD_CALL:
    case elfcpp::R_386_TLS_GD_POP:
    case elfcpp::R_386_TLS_LDM_32:
    case elfcpp::R_386_TLS_LDM_PUSH:
    case elfcpp::R_386_TLS_LDM_CALL:
    case elfcpp::R_386_TLS_LDM_POP:
    case elfcpp::R_386_USED_BY_INTEL_200:
    default:
      fprintf(stderr,
	      _("%s: %s: unsupported reloc %u against global symbol %s\n"),
	      program_name, object->name().c_str(), r_type, gsym->name());
      break;
    }
}

// Scan relocations for a section.

void
Target_i386::scan_relocs(const General_options& options,
			 Symbol_table* symtab,
			 Layout* layout,
			 Sized_relobj<32, false>* object,
			 unsigned int data_shndx,
			 unsigned int sh_type,
			 const unsigned char* prelocs,
			 size_t reloc_count,
			 size_t local_symbol_count,
			 const unsigned char* plocal_symbols,
			 Symbol** global_symbols)
{
  if (sh_type == elfcpp::SHT_RELA)
    {
      fprintf(stderr, _("%s: %s: unsupported RELA reloc section\n"),
	      program_name, object->name().c_str());
      gold_exit(false);
    }

  gold::scan_relocs<32, false, Target_i386, elfcpp::SHT_REL,
		    Target_i386::Scan>(
    options,
    symtab,
    layout,
    this,
    object,
    data_shndx,
    prelocs,
    reloc_count,
    local_symbol_count,
    plocal_symbols,
    global_symbols);
}

// Perform a relocation.

inline bool
Target_i386::Relocate::relocate(const Relocate_info<32, false>* relinfo,
				Target_i386* target,
				size_t relnum,
				const elfcpp::Rel<32, false>& rel,
				unsigned int r_type,
				const Sized_symbol<32>* gsym,
				elfcpp::Elf_types<32>::Elf_Addr value,
				unsigned char* view,
				elfcpp::Elf_types<32>::Elf_Addr address,
				off_t view_size)
{
  if (this->skip_call_tls_get_addr_)
    {
      if (r_type != elfcpp::R_386_PLT32
	  || gsym == NULL
	  || strcmp(gsym->name(), "___tls_get_addr") != 0)
	{
	  fprintf(stderr, _("%s: %s: missing expected TLS relocation\n"),
		  program_name,
		  relinfo->location(relnum, rel.get_r_offset()).c_str());
	  gold_exit(false);
	}

      this->skip_call_tls_get_addr_ = false;

      return false;
    }

  // Pick the value to use for symbols defined in shared objects.
  if (gsym != NULL && gsym->is_defined_in_dynobj())
    {
      if (gsym->has_plt_offset())
	address = target->plt_section()->address() + gsym->plt_offset();
      else
	gold_unreachable();
    }

  switch (r_type)
    {
    case elfcpp::R_386_NONE:
    case elfcpp::R_386_GNU_VTINHERIT:
    case elfcpp::R_386_GNU_VTENTRY:
      break;

    case elfcpp::R_386_32:
      Relocate_functions<32, false>::rel32(view, value);
      break;

    case elfcpp::R_386_PC32:
      Relocate_functions<32, false>::pcrel32(view, value, address);
      break;

    case elfcpp::R_386_16:
      Relocate_functions<32, false>::rel16(view, value);
      break;

    case elfcpp::R_386_PC16:
      Relocate_functions<32, false>::pcrel16(view, value, address);
      break;

    case elfcpp::R_386_8:
      Relocate_functions<32, false>::rel8(view, value);
      break;

    case elfcpp::R_386_PC8:
      Relocate_functions<32, false>::pcrel8(view, value, address);
      break;

    case elfcpp::R_386_PLT32:
      gold_assert(gsym->has_plt_offset()
		  || gsym->final_value_is_known(relinfo->options));
      Relocate_functions<32, false>::pcrel32(view, value, address);
      break;

    case elfcpp::R_386_GOT32:
      // Local GOT offsets not yet supported.
      gold_assert(gsym);
      gold_assert(gsym->has_got_offset());
      value = gsym->got_offset();
      Relocate_functions<32, false>::rel32(view, value);
      break;

    case elfcpp::R_386_GOTOFF:
      value -= target->got_section(NULL, NULL, NULL)->address();
      Relocate_functions<32, false>::rel32(view, value);
      break;

    case elfcpp::R_386_GOTPC:
      value = target->got_section(NULL, NULL, NULL)->address();
      Relocate_functions<32, false>::pcrel32(view, value, address);
      break;

    case elfcpp::R_386_COPY:
    case elfcpp::R_386_GLOB_DAT:
    case elfcpp::R_386_JUMP_SLOT:
    case elfcpp::R_386_RELATIVE:
    case elfcpp::R_386_TLS_TPOFF:
    case elfcpp::R_386_TLS_DTPMOD32:
    case elfcpp::R_386_TLS_DTPOFF32:
    case elfcpp::R_386_TLS_TPOFF32:
    case elfcpp::R_386_TLS_DESC:
      fprintf(stderr, _("%s: %s: unexpected reloc %u in object file\n"),
	      program_name,
	      relinfo->location(relnum, rel.get_r_offset()).c_str(),
	      r_type);
      gold_exit(false);
      break;

    case elfcpp::R_386_TLS_IE:
    case elfcpp::R_386_TLS_GOTIE:
    case elfcpp::R_386_TLS_LE:
    case elfcpp::R_386_TLS_GD:
    case elfcpp::R_386_TLS_LDM:
    case elfcpp::R_386_TLS_LDO_32:
    case elfcpp::R_386_TLS_IE_32:
    case elfcpp::R_386_TLS_LE_32:
    case elfcpp::R_386_TLS_GOTDESC:
    case elfcpp::R_386_TLS_DESC_CALL:
      this->relocate_tls(relinfo, relnum, rel, r_type, gsym, value, view,
			 address, view_size);
      break;

    case elfcpp::R_386_32PLT:
    case elfcpp::R_386_TLS_GD_32:
    case elfcpp::R_386_TLS_GD_PUSH:
    case elfcpp::R_386_TLS_GD_CALL:
    case elfcpp::R_386_TLS_GD_POP:
    case elfcpp::R_386_TLS_LDM_32:
    case elfcpp::R_386_TLS_LDM_PUSH:
    case elfcpp::R_386_TLS_LDM_CALL:
    case elfcpp::R_386_TLS_LDM_POP:
    case elfcpp::R_386_USED_BY_INTEL_200:
    default:
      fprintf(stderr, _("%s: %s: unsupported reloc %u\n"),
	      program_name,
	      relinfo->location(relnum, rel.get_r_offset()).c_str(),
	      r_type);
      // gold_exit(false);
      break;
    }

  return true;
}

// Perform a TLS relocation.

inline void
Target_i386::Relocate::relocate_tls(const Relocate_info<32, false>* relinfo,
				    size_t relnum,
				    const elfcpp::Rel<32, false>& rel,
				    unsigned int r_type,
				    const Sized_symbol<32>* gsym,
				    elfcpp::Elf_types<32>::Elf_Addr value,
				    unsigned char* view,
				    elfcpp::Elf_types<32>::Elf_Addr,
				    off_t view_size)
{
  Output_segment* tls_segment = relinfo->layout->tls_segment();
  if (tls_segment == NULL)
    {
      fprintf(stderr, _("%s: %s: TLS reloc but no TLS segment\n"),
	      program_name,
	      relinfo->location(relnum, rel.get_r_offset()).c_str());
      gold_exit(false);
    }

  const bool is_final = (gsym == NULL
			 ? !relinfo->options->is_shared()
			 : gsym->final_value_is_known(relinfo->options));
  const unsigned int opt_r_type =
    Target_i386::optimize_tls_reloc(relinfo->options, is_final, r_type);
  switch (r_type)
    {
    case elfcpp::R_386_TLS_LE_32:
      value = tls_segment->vaddr() + tls_segment->memsz() - value;
      Relocate_functions<32, false>::rel32(view, value);
      break;

    case elfcpp::R_386_TLS_LE:
      value = value - (tls_segment->vaddr() + tls_segment->memsz());
      Relocate_functions<32, false>::rel32(view, value);
      break;

    case elfcpp::R_386_TLS_IE:
    case elfcpp::R_386_TLS_GOTIE:
    case elfcpp::R_386_TLS_IE_32:
      if (opt_r_type == elfcpp::R_386_TLS_LE_32)
	{
	  Target_i386::Relocate::tls_ie_to_le(relinfo, relnum, tls_segment,
					      rel, r_type, value, view,
					      view_size);
	  break;
	}
      fprintf(stderr, _("%s: %s: unsupported reloc type %u\n"),
	      program_name,
	      relinfo->location(relnum, rel.get_r_offset()).c_str(),
	      r_type);
      // gold_exit(false);
      break;

    case elfcpp::R_386_TLS_GD:
      if (opt_r_type == elfcpp::R_386_TLS_LE_32)
	{
	  this->tls_gd_to_le(relinfo, relnum, tls_segment,
			     rel, r_type, value, view,
			     view_size);
	  break;
	}
      fprintf(stderr, _("%s: %s: unsupported reloc %u\n"),
	      program_name,
	      relinfo->location(relnum, rel.get_r_offset()).c_str(),
	      r_type);
      // gold_exit(false);
      break;

    case elfcpp::R_386_TLS_LDM:
    case elfcpp::R_386_TLS_LDO_32:
    case elfcpp::R_386_TLS_GOTDESC:
    case elfcpp::R_386_TLS_DESC_CALL:
      fprintf(stderr, _("%s: %s: unsupported reloc %u\n"),
	      program_name,
	      relinfo->location(relnum, rel.get_r_offset()).c_str(),
	      r_type);
      // gold_exit(false);
      break;
    }
}

// Do a relocation in which we convert a TLS Initial-Exec to a
// Local-Exec.

inline void
Target_i386::Relocate::tls_ie_to_le(const Relocate_info<32, false>* relinfo,
				    size_t relnum,
				    Output_segment* tls_segment,
				    const elfcpp::Rel<32, false>& rel,
				    unsigned int r_type,
				    elfcpp::Elf_types<32>::Elf_Addr value,
				    unsigned char* view,
				    off_t view_size)
{
  // We have to actually change the instructions, which means that we
  // need to examine the opcodes to figure out which instruction we
  // are looking at.
  if (r_type == elfcpp::R_386_TLS_IE)
    {
      // movl %gs:XX,%eax  ==>  movl $YY,%eax
      // movl %gs:XX,%reg  ==>  movl $YY,%reg
      // addl %gs:XX,%reg  ==>  addl $YY,%reg
      Target_i386::Relocate::check_range(relinfo, relnum, rel, view_size, -1);
      Target_i386::Relocate::check_range(relinfo, relnum, rel, view_size, 4);

      unsigned char op1 = view[-1];
      if (op1 == 0xa1)
	{
	  // movl XX,%eax  ==>  movl $YY,%eax
	  view[-1] = 0xb8;
	}
      else
	{
	  Target_i386::Relocate::check_range(relinfo, relnum, rel,
					     view_size, -2);

	  unsigned char op2 = view[-2];
	  if (op2 == 0x8b)
	    {
	      // movl XX,%reg  ==>  movl $YY,%reg
	      Target_i386::Relocate::check_tls(relinfo, relnum, rel,
					       (op1 & 0xc7) == 0x05);
	      view[-2] = 0xc7;
	      view[-1] = 0xc0 | ((op1 >> 3) & 7);
	    }
	  else if (op2 == 0x03)
	    {
	      // addl XX,%reg  ==>  addl $YY,%reg
	      Target_i386::Relocate::check_tls(relinfo, relnum, rel,
					       (op1 & 0xc7) == 0x05);
	      view[-2] = 0x81;
	      view[-1] = 0xc0 | ((op1 >> 3) & 7);
	    }
	  else
	    Target_i386::Relocate::check_tls(relinfo, relnum, rel, 0);
	}
    }
  else
    {
      // subl %gs:XX(%reg1),%reg2  ==>  subl $YY,%reg2
      // movl %gs:XX(%reg1),%reg2  ==>  movl $YY,%reg2
      // addl %gs:XX(%reg1),%reg2  ==>  addl $YY,$reg2
      Target_i386::Relocate::check_range(relinfo, relnum, rel, view_size, -2);
      Target_i386::Relocate::check_range(relinfo, relnum, rel, view_size, 4);

      unsigned char op1 = view[-1];
      unsigned char op2 = view[-2];
      Target_i386::Relocate::check_tls(relinfo, relnum, rel,
				       (op1 & 0xc0) == 0x80 && (op1 & 7) != 4);
      if (op2 == 0x8b)
	{
	  // movl %gs:XX(%reg1),%reg2  ==>  movl $YY,%reg2
	  view[-2] = 0xc7;
	  view[-1] = 0xc0 | ((op1 >> 3) & 7);
	}
      else if (op2 == 0x2b)
	{
	  // subl %gs:XX(%reg1),%reg2  ==>  subl $YY,%reg2
	  view[-2] = 0x81;
	  view[-1] = 0xe8 | ((op1 >> 3) & 7);
	}
      else if (op2 == 0x03)
	{
	  // addl %gs:XX(%reg1),%reg2  ==>  addl $YY,$reg2
	  view[-2] = 0x81;
	  view[-1] = 0xc0 | ((op1 >> 3) & 7);
	}
      else
	Target_i386::Relocate::check_tls(relinfo, relnum, rel, 0);
    }

  value = tls_segment->vaddr() + tls_segment->memsz() - value;
  if (r_type == elfcpp::R_386_TLS_IE || r_type == elfcpp::R_386_TLS_GOTIE)
    value = - value;

  Relocate_functions<32, false>::rel32(view, value);
}

// Do a relocation in which we convert a TLS Global-Dynamic to a
// Local-Exec.

inline void
Target_i386::Relocate::tls_gd_to_le(const Relocate_info<32, false>* relinfo,
				    size_t relnum,
				    Output_segment* tls_segment,
				    const elfcpp::Rel<32, false>& rel,
				    unsigned int,
				    elfcpp::Elf_types<32>::Elf_Addr value,
				    unsigned char* view,
				    off_t view_size)
{
  // leal foo(,%reg,1),%eax; call ___tls_get_addr
  //  ==> movl %gs,0,%eax; subl $foo@tpoff,%eax
  // leal foo(%reg),%eax; call ___tls_get_addr
  //  ==> movl %gs:0,%eax; subl $foo@tpoff,%eax

  Target_i386::Relocate::check_range(relinfo, relnum, rel, view_size, -2);
  Target_i386::Relocate::check_range(relinfo, relnum, rel, view_size, 9);

  unsigned char op1 = view[-1];
  unsigned char op2 = view[-2];

  Target_i386::Relocate::check_tls(relinfo, relnum, rel,
				   op2 == 0x8d || op2 == 0x04);
  Target_i386::Relocate::check_tls(relinfo, relnum, rel,
				   view[4] == 0xe8);

  int roff = 5;

  if (op2 == 0x04)
    {
      Target_i386::Relocate::check_range(relinfo, relnum, rel, view_size, -3);
      Target_i386::Relocate::check_tls(relinfo, relnum, rel,
				       view[-3] == 0x8d);
      Target_i386::Relocate::check_tls(relinfo, relnum, rel,
				       ((op1 & 0xc7) == 0x05
					&& op1 != (4 << 3)));
      memcpy(view - 3, "\x65\xa1\0\0\0\0\x81\xe8\0\0\0", 12);
    }
  else
    {
      Target_i386::Relocate::check_tls(relinfo, relnum, rel,
				       (op1 & 0xf8) == 0x80 && (op1 & 7) != 4);
      if (rel.get_r_offset() + 9 < view_size && view[9] == 0x90)
	{
	  // There is a trailing nop.  Use the size byte subl.
	  memcpy(view - 2, "\x65\xa1\0\0\0\0\x81\xe8\0\0\0", 12);
	  roff = 6;
	}
      else
	{
	  // Use the five byte subl.
	  memcpy(view - 2, "\x65\xa1\0\0\0\0\x2d\0\0\0", 11);
	}
    }

  value = tls_segment->vaddr() + tls_segment->memsz() - value;
  Relocate_functions<32, false>::rel32(view + roff, value);

  // The next reloc should be a PLT32 reloc against __tls_get_addr.
  // We can skip it.
  this->skip_call_tls_get_addr_ = true;
}

// Check the range for a TLS relocation.

inline void
Target_i386::Relocate::check_range(const Relocate_info<32, false>* relinfo,
				   size_t relnum,
				   const elfcpp::Rel<32, false>& rel,
				   off_t view_size, off_t off)
{
  off_t offset = rel.get_r_offset() + off;
  if (offset < 0 || offset > view_size)
    {
      fprintf(stderr, _("%s: %s: TLS relocation out of range\n"),
	      program_name,
	      relinfo->location(relnum, rel.get_r_offset()).c_str());
      gold_exit(false);
    }
}

// Check the validity of a TLS relocation.  This is like assert.

inline void
Target_i386::Relocate::check_tls(const Relocate_info<32, false>* relinfo,
				 size_t relnum,
				 const elfcpp::Rel<32, false>& rel,
				 bool valid)
{
  if (!valid)
    {
      fprintf(stderr,
	      _("%s: %s: TLS relocation against invalid instruction\n"),
	      program_name,
	      relinfo->location(relnum, rel.get_r_offset()).c_str());
      gold_exit(false);
    }
}

// Relocate section data.

void
Target_i386::relocate_section(const Relocate_info<32, false>* relinfo,
			      unsigned int sh_type,
			      const unsigned char* prelocs,
			      size_t reloc_count,
			      unsigned char* view,
			      elfcpp::Elf_types<32>::Elf_Addr address,
			      off_t view_size)
{
  gold_assert(sh_type == elfcpp::SHT_REL);

  gold::relocate_section<32, false, Target_i386, elfcpp::SHT_REL,
			 Target_i386::Relocate>(
    relinfo,
    this,
    prelocs,
    reloc_count,
    view,
    address,
    view_size);
}

// The selector for i386 object files.

class Target_selector_i386 : public Target_selector
{
public:
  Target_selector_i386()
    : Target_selector(elfcpp::EM_386, 32, false)
  { }

  Target*
  recognize(int machine, int osabi, int abiversion);

 private:
  Target_i386* target_;
};

// Recognize an i386 object file when we already know that the machine
// number is EM_386.

Target*
Target_selector_i386::recognize(int, int, int)
{
  if (this->target_ == NULL)
    this->target_ = new Target_i386();
  return this->target_;
}

Target_selector_i386 target_selector_i386;

} // End anonymous namespace.
