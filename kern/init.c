/* See COPYRIGHT for copyright information. */

#include <inc/stdio.h>
#include <inc/string.h>
#include <inc/assert.h>

#include <kern/monitor.h>
#include <kern/tsc.h>
#include <kern/console.h>
#include <kern/pmap.h>
#include <kern/env.h>
#include <kern/trap.h>
#include <kern/sched.h>
#include <kern/cpu.h>
#include <kern/picirq.h>
#include <kern/kclock.h>

void
i386_init(void)
{
	size_t i;
	extern char edata[], end[];

	// Before doing anything else, complete the ELF loading process.
	// Clear the uninitialized global data (BSS) section of our program.
	// This ensures that all static/global variables start out zero.
	for (i = 0; i < end - edata; i++)
		edata[i] = 0;

	// Perform global constructor initialisation (e.g. asan)
	// This must be done as early as possible
	extern void (*__ctors_start)();
	extern void (*__ctors_end)();
	void (**ctor)() = &__ctors_start;
	while (ctor < &__ctors_end) {
		(*ctor)();
		ctor++;
	}

	// Initialize the console.
	// Can't call cprintf until after we do this!
	cons_init();

	tsc_calibrate();

	cprintf("6828 decimal is %o octal!\n", 6828);
	cprintf("END: %p\n", end);

#ifndef CONFIG_KSPACE
	// Lab 6 memory management initialization functions
	mem_init();
#endif

	// user environment initialization functions
	env_init();

	clock_idt_init();

	pic_init();
	rtc_init();
	// Enable the IRQ_CLOCK interrupt
	irq_setmask_8259A(irq_mask_8259A & ~(1 << IRQ_CLOCK));

#ifdef CONFIG_KSPACE
	// Touch all you want.
	//ENV_CREATE_KERNEL_TYPE(prog_test1);
	//ENV_CREATE_KERNEL_TYPE(prog_test2);
	//ENV_CREATE_KERNEL_TYPE(prog_test3);
	//ENV_CREATE_KERNEL_TYPE(prog_test4);
	//ENV_CREATE_KERNEL_TYPE(prog_test5);
	//ENV_CREATE_KERNEL_TYPE(prog_test6);
#endif

	// Schedule and run the first user environment!
	sched_yield();
}


/*
 * Variable panicstr contains argument to first call to panic; used as flag
 * to indicate that the kernel has already called panic.
 */
const char *panicstr;

/*
 * Panic is called on unresolvable fatal errors.
 * It prints "panic: mesg", and then enters the kernel monitor.
 */
void
_panic(const char *file, int line, const char *fmt,...)
{
	va_list ap;

	if (panicstr)
		goto dead;
	panicstr = fmt;

	// Be extra sure that the machine is in as reasonable state
	__asm __volatile("cli; cld");

	va_start(ap, fmt);
	cprintf("kernel panic at %s:%d: ", file, line);
	vcprintf(fmt, ap);
	cprintf("\n");
	va_end(ap);

dead:
	/* break into the kernel monitor */
	while (1)
		monitor(NULL);
}

/* like panic, but don't */
void
_warn(const char *file, int line, const char *fmt,...)
{
	va_list ap;

	va_start(ap, fmt);
	cprintf("kernel warning at %s:%d: ", file, line);
	vcprintf(fmt, ap);
	cprintf("\n");
	va_end(ap);
}
