#ifndef MEMPOOL_H
#define MEMPOOL_h

#include "common.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define MEMPOOL_BUFFER_SIZE_256			(256U)
#define MEMPOOL_BUFFER_SIZE_512			(512U)
#define MEMPOOL_MINIMUM_BLOCK_SIZE		MEMPOOL_BUFFER_SIZE_256

/**
 *	Generic mempool buffer typedef
 */
typedef void * MEMPOOL_buffer_t;

/**
 *	Fixed buffer size IDs for allocation
 */
typedef enum _MEMPOOL_buffer_size_id
{
	MEMPOOL_BUFFER_SIZE_ID_256 = 0,
	MEMPOOL_BUFFER_SIZE_ID_512,
	//////////
	MEMPOOL_BUFFER_SIZE_ID_NUM_SIZES
} MEMPOOL_buffer_size_id_t;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void				MEMPOOL_init	(void);
MEMPOOL_buffer_t	MEMPOOL_alloc	(MEMPOOL_buffer_size_id_t buffer_size_id);
void				MEMPOOL_free	(MEMPOOL_buffer_t buffer);

#endif // MEMPOOL_H
