
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "proto.h"
#include "proc.h"
#include "global.h"

int			ticks;
int			disp_pos;
t_8			gdt_ptr[6];	// 0~15:Limit  16~47:Base
DESCRIPTOR		gdt[GDT_SIZE];
t_8			idt_ptr[6];	// 0~15:Limit  16~47:Base
GATE			idt[IDT_SIZE];

t_32			k_reenter;

TSS			tss;
PROCESS*		p_proc_ready;
int			nr_current_console;

PUBLIC	PROCESS		proc_table[NR_TASKS];
PUBLIC	char		task_stack[STACK_SIZE_TOTAL];

PUBLIC	TASK		task_table[NR_TASKS] = {
				{task_tty, STACK_SIZE_TTY, "tty",  15},
				{TestA, STACK_SIZE_TESTA, "TestA", 15},
				{TestB, STACK_SIZE_TESTB, "TestB",  5},
				{TestC, STACK_SIZE_TESTC, "TestC",  3}};

PUBLIC	TTY			tty_table[NR_CONSOLES];
PUBLIC	CONSOLE			console_table[NR_CONSOLES];

PUBLIC	t_pf_irq_handler	irq_table[NR_IRQ];

PUBLIC	t_sys_call		sys_call_table[NR_SYS_CALL] = {sys_get_ticks};
