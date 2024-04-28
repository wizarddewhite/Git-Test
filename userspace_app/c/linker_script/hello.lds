OUTPUT_FORMAT("elf64-x86-64", "elf64-x86-64",
	      "elf64-x86-64")
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)
SECTIONS
{
  PROVIDE (__executable_start = SEGMENT_START("text-segment", 0x400000));
  . = SEGMENT_START("text-segment", 0x400000) + SIZEOF_HEADERS;
  .text         : { *(.text) }
  .data         : { *(.data) }
  . = ALIGN(0x1000);
  __init_begin = .;
  .meminit.text : { *(.meminit.text) }
  . = ALIGN(0x1000);
  __init_end = .;
  .rodata       : { *(.rodata) }
}
