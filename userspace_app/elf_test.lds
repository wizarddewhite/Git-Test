SECTIONS 
{
	.interp :
	{
		*(.interp);
	}
	.note.ABI-tag :
	{
		*(.note.ABI-tag);
	}
	.note.gnu.build-in :
	{
		*(.note.gnu.build-in)
	}
	.gnu.hash :
	{
		*(.gnu.hash);
	}
	.dynsym :
	{
		*(.dynsym);
	}
	.dynstr : 
	{
		*(.dynstr);
	}
	.gnu.version :
	{
		*(.gnu.version);
	}
	.gnu.version_r :
	{
		*(.gnu.version_r);
	}
	.rel.dyn :
	{
		*(.rel.dyn);
	}
	.rel.plt :
	{
		*(.rel.plt);
	}
	.init :
	{
		*(.init);
	}
	.plt :
	{
		*(.plt);
	}
	.text : 
	{
		*(.text);
	}
	.fini :
	{
		*(.fini);
	}
	.rodata :
	{
		*(.rodata);
	}
	.eh_frame_hdr :
	{
		*(.eh_frame_hdr);
	}
	.eh_frame :
	{
		*(.eh_frame);
	}
	.ctors :
	{
		*(.ctors);
	}
	.dtors :
	{
		*(.dtors);
	}
	.jcr :
	{
		*(.jcr);
	}
	.dynamic :
	{
		*(.dynamic);
	}
	.got :
	{
		*(.got);
	}
	.got.plt :
	{
		*(.got.plt);
	}
	.data : 
	{
		*(.data);
	}
	.bss :
	{
		*(.bss);
	}
	.comment :
	{
		*(.comment);
	}
	.debug_aranges :
	{
		*(.debug_aranges);
	}
	.debug_info :
	{
		*(.debug_info);
	}
	.debug_abbrev :
	{
		*(.debug_abbrev);
	}
	.debug_line :
	{
		*(.debug_line);
	}
	.debug_str :
	{
		*(.debug_str);
	}
	.debug_loc :
	{
		*(.debug_loc);
	}
	.shstrtab :
	{
		*(.shstrtab);
	}
	.symtab :
	{
		*(.symtab);
	}
	.strtab :
	{
		*(.strtab);
	}
}
