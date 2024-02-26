
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

extern	int			disp_pos;
extern	t_8			gdt_ptr[6];	// 0~15:Limit  16~47:Base
extern	DESCRIPTOR		gdt[GDT_SIZE];
extern	t_8			idt_ptr[6];	// 0~15:Limit  16~47:Base
extern	GATE			idt[IDT_SIZE];

extern	TSS		tss;
extern	PROCESS*	p_proc_ready;

extern	PROCESS		proc_table[];
extern	char		task_stack[];
