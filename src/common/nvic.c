#include <unistd.h>
#include "interrupts.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

/**
 *	This macro is used to alias undefined handlers as irqEMPTY_DEF until they're actually used
 */
#define NVIC_WEAK_IRQ(name) extern void __attribute__((weak, alias("irqEMPTY_DEF"))) name()
#define NVIC_VECTOR_TABLE_SIZE (155)

/**
 *	IRQ function pointer
 *
 * 	Our vector table contains 155 of these. Those that are not strongly linked will be irqEMPTY_DEF
 */
typedef void (* NVIC_irq_function_t)(void);

/**
 *	NVIC table struct typedef
 *
 * 	Contains a pointer to the top of the stack as well as our `NVIC_irq_function_t`s
 */
typedef struct _NVIC_table_t
{
	const void *    	kpv_stack_top;
    NVIC_irq_function_t	p_vector_table[NVIC_VECTOR_TABLE_SIZE];
} NVIC_table_t;

/****************************************************************************************************
 *	F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

NVIC_WEAK_IRQ(SysTick_Handler);
NVIC_WEAK_IRQ(irqSYSTEM);
NVIC_WEAK_IRQ(irqWDT);
NVIC_WEAK_IRQ(irqRTC);
NVIC_WEAK_IRQ(irqEIC);
NVIC_WEAK_IRQ(irqFREQM);
NVIC_WEAK_IRQ(irqTSENS);
NVIC_WEAK_IRQ(irqNVMCTRL);
NVIC_WEAK_IRQ(irqDMAC);
NVIC_WEAK_IRQ(irqEVSYS);
NVIC_WEAK_IRQ(irqSERCOM0);
NVIC_WEAK_IRQ(irqSERCOM1);
NVIC_WEAK_IRQ(irqSERCOM2);
NVIC_WEAK_IRQ(irqSERCOM3);
NVIC_WEAK_IRQ(irqSERCOM4);
NVIC_WEAK_IRQ(irqSERCOM5);
NVIC_WEAK_IRQ(irqCAN0);
NVIC_WEAK_IRQ(irqCAN1);
NVIC_WEAK_IRQ(irqTCC0);
NVIC_WEAK_IRQ(irqTCC1);
NVIC_WEAK_IRQ(irqTCC2);
NVIC_WEAK_IRQ(irqTC0);
NVIC_WEAK_IRQ(irqTC1);
NVIC_WEAK_IRQ(irqTC2);
NVIC_WEAK_IRQ(irqTC3);
NVIC_WEAK_IRQ(irqTC4);
NVIC_WEAK_IRQ(irqADC0);
NVIC_WEAK_IRQ(irqADC1);
NVIC_WEAK_IRQ(irqAC);
NVIC_WEAK_IRQ(irqDAC);
NVIC_WEAK_IRQ(irqSDADC);
NVIC_WEAK_IRQ(irqPTC);

NVIC_WEAK_IRQ(SVC_Handler);
NVIC_WEAK_IRQ(PendSV_Handler);

void irqRESET();
void irqNMI();
void irqEMPTY_DEF();
void irqHARD_FAULT();

extern int main(void);

/****************************************************************************************************
 *	V A R I A B L E S
 ****************************************************************************************************/

/**
 *	Section variables (externed, provided by linkerscript)
 */
extern uint32_t u32_stack_top;
extern uint32_t u32_begin_text;
extern uint32_t u32_end_text;
extern uint32_t u32_begin_data;
extern uint32_t u32_end_data;
extern uint32_t u32_begin_bss;
extern uint32_t u32_end_bss;

/**
 *	The actual NVIC table, stored in the .nvic_table section of RAM
 *
 * 	The vector table contains the initial stack pointer as well as the addresses of
 *	ISRs relevant to the Cortex M0+ core, as well as vendor-specific ISRs (peripherals, etc.).
 */
static const NVIC_table_t NVIC_table __attribute__((section(".nvic_table"), used)) =
{
    &u32_stack_top,
    {
        irqRESET,           // Reset handler
		irqNMI,             // NMI handler
		irqHARD_FAULT,      // Hard Fault handler

		/* Configurable priority handlers */
		irqEMPTY_DEF,       // MMU Fault handler
		irqEMPTY_DEF,       // Bus Fault handler
		irqEMPTY_DEF,       // Usage Fault
		NULL,               // Reserved
		NULL,               // Reserved
		NULL,               // Reserved
		NULL,               // Reserved
		SVC_Handler,       	// SV Call (Supervisor Call, i.e. svc instruction executed)
		irqEMPTY_DEF,       // Debug Monitor
		NULL,               // Reserved
		PendSV_Handler,    	// Pending SV
		SysTick_Handler,   	// SysTick

        /* Vendor-specific handlers */
		irqSYSTEM,			// SYSTEM
		irqWDT,				// WDT
		irqRTC,				// RTC
		irqEIC,				// EIC
		irqFREQM,			// FREQM
		irqTSENS,			// TSENS
		irqNVMCTRL,			// NVMCTRL
		irqDMAC,			// DMAC
		irqEVSYS,			// EVSYS
		irqSERCOM0,			// SERCOM0
		irqSERCOM1,			// SERCOM1
		irqSERCOM2,			// SERCOM2
		irqSERCOM3,			// SERCOM3
		irqSERCOM4,			// SERCOM4
		irqSERCOM5,			// SERCOM5
		irqCAN0,			// CAN0
		irqCAN1,			// CAN1
		irqTCC0,			// TCC0
		irqTCC1,			// TCC1
		irqTCC2,			// TCC2
		irqTC0,				// TC0
		irqTC1,				// TC1
		irqTC2,				// TC2
		irqTC3,				// TC3
		irqTC4,				// TC4
		irqADC0,			// ADC0
		irqADC1,			// ADC1
		irqAC,				// AC
		irqDAC,				// DAC
		irqSDADC,			// SDADC
		irqPTC,				// PTC
    }
};

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Basic "do nothing forever" irq
 ****************************************************************************************************/
void irqEMPTY_DEF()
{
	while(1);
}

/****************************************************************************************************
 *	Basic "do nothing forever" irq
 ****************************************************************************************************/
void irqNMI()
{
	while(1);
}

/****************************************************************************************************
 *	The reset handler IRQ. Called upon power up.
 *
 *	Copies initialized source program data in flash to RAM, and then initializes the BSS section to zero.
 *	Once data is copied, main is called.
 ****************************************************************************************************/
void irqRESET()
{
	while (1)
	{
		uint32_t *pu32_src = &u32_end_text;

		for (uint32_t *pu32_dst = &u32_begin_data; pu32_dst < &u32_end_data; ++pu32_dst, ++pu32_src)
		{
			*pu32_dst = *pu32_src;
		}

		for (uint32_t *pu32_dst = &u32_begin_bss; pu32_dst < &u32_end_bss; ++pu32_dst)
		{
			*pu32_dst = 0;
		}

		main();
	}
}

/****************************************************************************************************
 *	Something has gone badly wrong.
 *
 *	TODO:
 *	Stub for bootloader. Let the application code define this properaly upon relocation
 * 
 *	Get the process stack pointer or the main stack pointer, whichever's in use.
 *	Save the stack, trigger a breakpoint, spin in a deadloop.
 ****************************************************************************************************/
void __attribute__((weak)) irqHARD_FAULT(void)
{
	while(1);
}
// {
// __asm__(
// 	"movs    r0, #4            \n\t"
// 	"movs    r1, lr            \n\t"
// 	"tst     r0, r1            \n\t"
// 	"beq     _MSP              \n\t"
// 	"mrs     r0, psp           \n\t"
// 	"b       _HALT             \n\t"
// 	"_MSP:                     \n\t"
// 	"mrs     r0, msp           \n\t"
// 	"_HALT:                    \n\t"
// 	"ldr     r1, [r0, #0x00]   \n\t" /* r0 */
// 	"ldr     r2, [r0, #0x04]   \n\t" /* r1 */
// 	"ldr     r3, [r0, #0x08]   \n\t" /* r2 */
// 	"ldr     r4, [r0, #0x0C]   \n\t" /* r3 */
// 	"ldr     r5, [r0, #0x10]   \n\t" /* r12 */
// 	"ldr     r6, [r0, #0x14]   \n\t" /* lr */
// 	"ldr     r7, [r0, #0x1C]   \n\t" /* xpsr */
// 	"mov     r8, r7            \n\t"
// 	"ldr     r7, [r0, #0x18]   \n\t" /* pc */
// 	"bkpt    #0                \n"
// 	"_DEADLOOP:                \n\t"
// 	"b       _DEADLOOP         "
// );
// }

/****************************************************************************************************
 *	Newlib syscall stubs to make the compiler happy
 ****************************************************************************************************/
int 		__attribute__((weak)) 	_close		(int file) 								{ return -1; }
int 		__attribute__((weak)) 	_fstat		(int file, struct stat *st) 			{ return 0; }
pid_t 		__attribute__((weak)) 	_getpid		(void) 									{ return 1; }
int 		__attribute__((weak)) 	_isatty		(int file) 								{ return 1; }
int 		__attribute__((weak)) 	_kill		(pid_t pid, int sig) 					{ return -1; }
off_t 		__attribute__((weak)) 	_lseek		(int file, off_t offset, int whence) 	{ return -1; }
ssize_t 	__attribute__((weak)) 	_read		(int file, void *ptr, size_t len) 		{ return 0; }
ssize_t 	__attribute__((weak)) 	_write		(int file, const void *ptr, size_t len) { return len; }
