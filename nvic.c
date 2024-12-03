#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*
 *	M A C R O S
 */

/// This macro is used to alias undefined handlers as irqEMPTY_DEF until they're actually used
#define NVIC_WEAK_IRQ(name) extern void __attribute__((weak, alias("irqEMPTY_DEF"))) name()
#define NVIC_VECTOR_TABLE_SIZE (155)

/*
 *	T Y P E D E F S
 */

/// IRQ function pointer
/// 
///	Our vector table contains 155 of these. Those that are not strongly linked will be irqEMPTY_DEF
typedef void (* NVIC_irq_function_t)();

/// NVIC table struct typedef
///
/// Contains a pointer to the top of the stack as well as our `NVIC_irq_function_t`s
typedef struct _NVIC_table_t
{
	const void *    	kpv_stack_top;
    NVIC_irq_function_t	p_vector_table[NVIC_VECTOR_TABLE_SIZE];
} NVIC_table_t;


/*
 *	F U N C T I O N   P R O T O T Y P E S
 */

/// Weakly linked *irqSysTick* handler
NVIC_WEAK_IRQ(irqSysTick);

/// Weakly linked *irqSYSTEM* handler
NVIC_WEAK_IRQ(irqSYSTEM);

/// Weakly linked *irqWDT* handler
NVIC_WEAK_IRQ(irqWDT);

/// Weakly linked *irqRTC* handler
NVIC_WEAK_IRQ(irqRTC);

/// Weakly linked *irqEIC* handler
NVIC_WEAK_IRQ(irqEIC);

/// Weakly linked *irqFREQM* handler
NVIC_WEAK_IRQ(irqFREQM);

/// Weakly linked *irqTSENS* handler
NVIC_WEAK_IRQ(irqTSENS);

/// Weakly linked *irqNVMCTRL* handler
NVIC_WEAK_IRQ(irqNVMCTRL);

/// Weakly linked *irqDMAC* handler
NVIC_WEAK_IRQ(irqDMAC);

/// Weakly linked *irqEVSYS* handler
NVIC_WEAK_IRQ(irqEVSYS);

/// Weakly linked *irqSERCOM0* handler
NVIC_WEAK_IRQ(irqSERCOM0);

/// Weakly linked *irqSERCOM1* handler
NVIC_WEAK_IRQ(irqSERCOM1);

/// Weakly linked *irqSERCOM2* handler
NVIC_WEAK_IRQ(irqSERCOM2);

/// Weakly linked *irqSERCOM3* handler
NVIC_WEAK_IRQ(irqSERCOM3);

/// Weakly linked *irqSERCOM4* handler
NVIC_WEAK_IRQ(irqSERCOM4);

/// Weakly linked *irqSERCOM5* handler
NVIC_WEAK_IRQ(irqSERCOM5);

/// Weakly linked *irqCAN0* handler
NVIC_WEAK_IRQ(irqCAN0);

/// Weakly linked *irqCAN1* handler
NVIC_WEAK_IRQ(irqCAN1);

/// Weakly linked *irqTCC0* handler
NVIC_WEAK_IRQ(irqTCC0);

/// Weakly linked *irqTCC1* handler
NVIC_WEAK_IRQ(irqTCC1);

/// Weakly linked *irqTCC2* handler
NVIC_WEAK_IRQ(irqTCC2);

/// Weakly linked *irqTC0* handler
NVIC_WEAK_IRQ(irqTC0);

/// Weakly linked *irqTC1* handler
NVIC_WEAK_IRQ(irqTC1);

/// Weakly linked *irqTC2* handler
NVIC_WEAK_IRQ(irqTC2);

/// Weakly linked *irqTC3* handler
NVIC_WEAK_IRQ(irqTC3);

/// Weakly linked *irqTC4* handler
NVIC_WEAK_IRQ(irqTC4);

/// Weakly linked *irqADC0* handler
NVIC_WEAK_IRQ(irqADC0);

/// Weakly linked *irqADC1* handler
NVIC_WEAK_IRQ(irqADC1);

/// Weakly linked *irqAC* handler
NVIC_WEAK_IRQ(irqAC);

/// Weakly linked *irqDAC* handler
NVIC_WEAK_IRQ(irqDAC);

/// Weakly linked *irqSDADC* handler
NVIC_WEAK_IRQ(irqSDADC);

/// Weakly linked *irqPTC* handler
NVIC_WEAK_IRQ(irqPTC);

void irqRESET();
void irqNMI();
void irqEMPTY_DEF();
void irqHARD_FAULT();

extern int main(void);

/*
 *	P R I V A T E   V A R I A B L E S
 */

/// *u32_stack_top* section variable (externed, provided by linkerscript)
extern uint32_t u32_stack_top;

/// *u32_begin_text* section variable (externed, provided by linkerscript)
extern uint32_t u32_begin_text;

/// *u32_end_text* section variable (externed, provided by linkerscript)
extern uint32_t u32_end_text;

/// *u32_begin_data* section variable (externed, provided by linkerscript)
extern uint32_t u32_begin_data;

/// *u32_end_data* section variable (externed, provided by linkerscript)
extern uint32_t u32_end_data;

/// *u32_begin_bss* section variable (externed, provided by linkerscript)
extern uint32_t u32_begin_bss;

/// *u32_end_bss* section variable (externed, provided by linkerscript)
extern uint32_t u32_end_bss;

/// The actual NVIC table, stored in the .nvic_table section of RAM
///
/// The vector table contains the initial stack pointer as well as the addresses of
/// ISRs relevant to the Cortex M0+ core, as well as vendor-specific ISRs (peripherals, etc.).
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
		irqEMPTY_DEF,       // SV Call
		irqEMPTY_DEF,       // Debug Monitor
		NULL,               // Reserved
		irqEMPTY_DEF,       // Pending SV
		irqSysTick,         // SysTick

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


/*
 *	F U N C T I O N   D E F I N I T I O N S
 */

/// Minimal memset implementation
void* memset(void* s, int c, size_t n) {
    unsigned char* p = (unsigned char*)s;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return s;
}

/// Minimal memcpy implementation
void* memcpy(void* dest, const void* src, size_t n) {
    unsigned char* d = (unsigned char*)dest;
    const unsigned char* s = (const unsigned char*)src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

/// Basic "do nothing forever" irq
void irqEMPTY_DEF()
{
	while(true);
}

/// Basic "do nothing forever" irq
void irqNMI()
{
	while(true);
}

/// The reset handler IRQ. Called upon power up.
/// Copies initialized source program data in flash to RAM, and then initializes the BSS section to zero.
/// Once data is copied, main is called.
void irqRESET()
{
	while(true)
	{
		uint32_t *pu32_src = &u32_end_text;

		for (uint32_t *pu32_dst = &u32_begin_data; pu32_dst < &u32_end_data; ++pu32_dst, ++pu32_src)
			*pu32_dst = *pu32_src;

		for (uint32_t *pu32_dst = &u32_begin_bss; pu32_dst < &u32_end_bss; ++pu32_dst)
			*pu32_dst = 0;

		main();
	}
}

/// Something has gone wrong. Get the process stack pointer or the main stack pointer, whichever's in use.
/// Save the stack, trigger a breakpoint, spin in a deadloop. 
void irqHARD_FAULT(void)
{
    __asm__(
        "movs    r0, #4            \n\t"
        "movs    r1, lr            \n\t"
        "tst     r0, r1            \n\t"
        "beq     _MSP              \n\t"
        "mrs     r0, psp           \n\t"
        "b       _HALT             \n\t"
        "_MSP:                     \n\t"
        "mrs     r0, msp           \n\t"
        "_HALT:                    \n\t"
        "ldr     r1, [r0, #0x00]   \n\t" /* r0 */
        "ldr     r2, [r0, #0x04]   \n\t" /* r1 */
        "ldr     r3, [r0, #0x08]   \n\t" /* r2 */
        "ldr     r4, [r0, #0x0C]   \n\t" /* r3 */
        "ldr     r5, [r0, #0x10]   \n\t" /* r12 */
        "ldr     r6, [r0, #0x14]   \n\t" /* lr */
        "ldr     r7, [r0, #0x1C]   \n\t" /* xpsr */
        "mov     r8, r7            \n\t"
        "ldr     r7, [r0, #0x18]   \n\t" /* pc */
        "bkpt    #0                \n"
        "_DEADLOOP:                \n\t"
        "b       _DEADLOOP         "
    );
}