
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "proto.h"
#include "proc.h"
#include "global.h"

int			disp_pos;
t_8			gdt_ptr[6];	// 0~15:Limit  16~47:Base
DESCRIPTOR		gdt[GDT_SIZE];
t_8			idt_ptr[6];	// 0~15:Limit  16~47:Base
GATE			idt[IDT_SIZE];

TSS			tss;
PROCESS*		p_proc_ready;

PUBLIC	PROCESS			proc_table[NR_TASKS];
PUBLIC	char			task_stack[STACK_SIZE_TOTAL];
