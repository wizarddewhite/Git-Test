
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

extern	int			ticks;
extern	int			disp_pos;
extern	t_8			gdt_ptr[6];	// 0~15:Limit  16~47:Base
extern	DESCRIPTOR		gdt[GDT_SIZE];
extern	t_8			idt_ptr[6];	// 0~15:Limit  16~47:Base
extern	GATE			idt[IDT_SIZE];

extern	t_32			k_reenter;

extern	TSS		tss;
extern	PROCESS*	p_proc_ready;
extern	int		nr_current_console;

extern	PROCESS		proc_table[];
extern	char		task_stack[];
extern	TASK		task_table[];
extern	TTY			tty_table[NR_CONSOLES];
extern	CONSOLE			console_table[NR_CONSOLES];

extern	t_pf_irq_handler	irq_table[NR_IRQ];
extern	t_sys_call		sys_call_table[];
