/* See COPYRIGHT for copyright information. */

#include <inc/x86.h>
#include <kern/kclock.h>

int gettime(void)
{
	nmi_disable();
	// LAB 12: your code here

	nmi_enable();
	return 0;
}

void
rtc_init(void)
{
	uint8_t reg;

	nmi_disable();
	// LAB 4: your code here
	outb(IO_RTC_CMND, RTC_BREG);
	reg = inb(IO_RTC_DATA);
	reg |= RTC_PIE;
	outb(IO_RTC_DATA, reg);

	outb(IO_RTC_CMND, RTC_AREG);
	reg = inb(IO_RTC_DATA);
	// Set RS0 to 1, RS1 to 1, RS2 to 1, and RS3 to 1
	// in order to set the interrupt period to 500ms
	reg |= 0xF;
	outb(IO_RTC_DATA, reg);

	nmi_enable();
}

uint8_t
rtc_check_status(void)
{
	uint8_t status = 0;
	// LAB 4: your code here
	outb(IO_RTC_CMND, RTC_CREG);
	status = inb(IO_RTC_DATA);

	return status;
}

unsigned
mc146818_read(unsigned reg)
{
	outb(IO_RTC_CMND, reg);
	return inb(IO_RTC_DATA);
}

void
mc146818_write(unsigned reg, unsigned datum)
{
	outb(IO_RTC_CMND, reg);
	outb(IO_RTC_DATA, datum);
}

