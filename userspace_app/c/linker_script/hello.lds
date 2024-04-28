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
  .meminit.text : { *(.meminit.text) }
  .rodata       : { *(.rodata) }
}
