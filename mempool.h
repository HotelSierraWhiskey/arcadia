#ifndef MEMPOOL_H
#define MEMPOOL_h

#include "common.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

typedef void * MEMPOOL_buffer_t;

typedef enum _MEMPOOL_buffer_size_id
{
	MEMPOOL_BUFFER_SIZE_ID_256,
	MEMPOOL_BUFFER_SIZE_ID_512,
	//////////
	MEMPOOL_BUFFER_SIZE_ID_NUM_SIZES
} MEMPOOL_buffer_size_id_t;

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

void				MEMPOOL_init	(void);
MEMPOOL_buffer_t	MEMPOOL_alloc	(MEMPOOL_buffer_size_id_t buffer_size_id);
void				MEMPOOL_free	(MEMPOOL_buffer_t p_buffer);

#endif // MEMPOOL_H
