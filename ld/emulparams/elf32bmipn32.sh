# This is an ELF platform.
SCRIPT_NAME=elf

# Handle both big- and little-ended 32-bit MIPS objects.
ARCH=mips
OUTPUT_FORMAT="elf32-bigmips"
BIG_OUTPUT_FORMAT="elf32-bigmips"
LITTLE_OUTPUT_FORMAT="elf32-littlemips"

TEMPLATE_NAME=elf32

TEXT_START_ADDR=0x10000000
MAXPAGESIZE=0x100000
ENTRY=__start

# GOT-related settings.  
OTHER_GOT_SYMBOLS='
  _gp = ALIGN(16) + 0x7ff0;
'
OTHER_GOT_SECTIONS='
  .lit8 : { *(.lit8) }
  .lit4 : { *(.lit4) }
  .srdata : { *(.srdata) }
'

# Magic symbols.
TEXT_START_SYMBOLS='_ftext = . ;'
DATA_START_SYMBOLS='_fdata = . ;'
OTHER_BSS_SYMBOLS='_fbss = .;'
# IRIX6 defines these symbols.  0x34 is the size of the ELF header.
EXECUTABLE_SYMBOLS="
  __dso_displacement = 0;
  __elf_header = ${TEXT_START_ADDR};
  __program_header_table = ${TEXT_START_ADDR} + 0x34;
"

# There are often dynamic relocations against the .rodata section.
# Setting DT_TEXTREL in the .dynamic section does not convince the
# IRIX6 linker to permit relocations against the text segment.
# Following the IRIX linker, we simply put .rodata in the data
# segment.
WRITABLE_RODATA=

