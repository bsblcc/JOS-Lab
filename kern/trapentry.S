/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */

TRAPHANDLER_NOEC(h_divide , T_DIVIDE)
TRAPHANDLER_NOEC(h_debug , T_DEBUG)
TRAPHANDLER_NOEC(h_nmi , T_NMI)
TRAPHANDLER_NOEC(h_brkpt , T_BRKPT)
TRAPHANDLER_NOEC(h_oflow , T_OFLOW)
TRAPHANDLER_NOEC(h_bound , T_BOUND)
TRAPHANDLER_NOEC(h_illop , T_ILLOP)
TRAPHANDLER_NOEC(h_device , T_DEVICE)
TRAPHANDLER(h_dblflt , T_DBLFLT)
// reserved 
TRAPHANDLER(h_tss , T_TSS)
TRAPHANDLER(h_segnp , T_SEGNP)
TRAPHANDLER(h_stack , T_STACK)
TRAPHANDLER(h_gpflt , T_GPFLT)
TRAPHANDLER(h_pgflt , T_PGFLT)
// reserved
TRAPHANDLER_NOEC(h_fperr , T_FPERR)
TRAPHANDLER(h_align , T_ALIGN)
TRAPHANDLER_NOEC(h_mchk , T_MCHK)
TRAPHANDLER_NOEC(h_simderr , T_SIMDERR)

TRAPHANDLER_NOEC(irq_timer , IRQ_OFFSET + IRQ_TIMER)
TRAPHANDLER_NOEC(irq_kbd , IRQ_OFFSET + IRQ_KBD)
TRAPHANDLER_NOEC(irq_2 , IRQ_OFFSET + 2)
TRAPHANDLER_NOEC(irq_3 , IRQ_OFFSET + 3)
TRAPHANDLER_NOEC(irq_serial , IRQ_OFFSET + IRQ_SERIAL)
TRAPHANDLER_NOEC(irq_5 , IRQ_OFFSET + 5)
TRAPHANDLER_NOEC(irq_6 , IRQ_OFFSET + 6)
TRAPHANDLER_NOEC(irq_spurious , IRQ_OFFSET + IRQ_SPURIOUS)
TRAPHANDLER_NOEC(irq_8 , IRQ_OFFSET + 8)
TRAPHANDLER_NOEC(irq_9 , IRQ_OFFSET + 9)
TRAPHANDLER_NOEC(irq_10 , IRQ_OFFSET + 10)
TRAPHANDLER_NOEC(irq_11 , IRQ_OFFSET + 11)
TRAPHANDLER_NOEC(irq_12 , IRQ_OFFSET + 12)
TRAPHANDLER_NOEC(irq_13 , IRQ_OFFSET + 13)
TRAPHANDLER_NOEC(irq_ide , IRQ_OFFSET + IRQ_IDE)
TRAPHANDLER_NOEC(irq_15 , IRQ_OFFSET + 15)

// and you tell me I debugged for 2 days
// just because the additional _NOEC on this crap:?
//???????????????????????????????????????????????????????????
//TRAPHANDLER(h_syscall , T_SYSCALL)
//screw you system call!!!!
TRAPHANDLER_NOEC(h_syscall , T_SYSCALL)

/*
 * Lab 3: Your code here for _alltraps
 */

_alltraps:
	pushl %ds
	pushl %es
	pushal 

	movl $GD_KD, %eax
	movw %ax, %ds
	movw %ax, %es

	push %esp
	call trap	




