#ifndef INTERRUPTS_H
#define INTERRUPTS_H

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void	irqSERCOM0	(void);
void	irqSERCOM1	(void);
void	irqSERCOM2	(void);
void	irqSERCOM3	(void);
void	irqSERCOM4	(void);
void	irqSERCOM5	(void);
void 	irqTC0		(void);
void 	irqTC1		(void);
void 	irqTC2		(void);
void 	irqTC3		(void);
void 	irqTC4		(void);
void	irqEIC		(void);

#endif // INTERRUPTS_H
