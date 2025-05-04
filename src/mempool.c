#include "mempool.h"
#include "utils.h"

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define MEMPOOL_TOTAL_SIZE				(4096U)
#define MEMPOOL_TOTAL_BLOCKS			(MEMPOOL_TOTAL_SIZE / MEMPOOL_MINIMUM_BLOCK_SIZE)

#define MEMPOOL_semaphore_take() 		xSemaphoreTake(MEMPOOL_info.pool_mutex, portMAX_DELAY)
#define MEMPOOL_semaphore_give() 		xSemaphoreGive(MEMPOOL_info.pool_mutex)

/**
 *	Memory pool info typedef
 */
typedef struct _MEMPOOL_info
{
	uint8_t				pu8_buffer_pool[MEMPOOL_TOTAL_SIZE];
	uint32_t			pu32_allocation_registry[MEMPOOL_TOTAL_BLOCKS];
	uint32_t			pool_bitmask;
	SemaphoreHandle_t 	pool_mutex;
	StaticSemaphore_t 	pool_mutex_buffer;
} MEMPOOL_info_t;

/****************************************************************************************************
 *	P R I V A T E   V A R I A B L E S
 ****************************************************************************************************/

/**
 *	Module info struct
 */
static MEMPOOL_info_t MEMPOOL_info;

/**
 *	Actual integer buffer sizes for internal use 
 */
static const uint32_t kpu8_buffer_sizes[MEMPOOL_BUFFER_SIZE_ID_NUM_SIZES] =
{
	[MEMPOOL_BUFFER_SIZE_ID_256]	= MEMPOOL_MINIMUM_BLOCK_SIZE,
	[MEMPOOL_BUFFER_SIZE_ID_512]	= MEMPOOL_BUFFER_SIZE_512,
	[MEMPOOL_BUFFER_SIZE_ID_1K] 	= MEMPOOL_BUFFER_SIZE_1024
};

/****************************************************************************************************
 *	P R I V A T E   F U N C T I O N   P R O T O T Y P E S
 ****************************************************************************************************/

static bool 	MEMPOOL_is_block_free	(uint8_t u8_block_index);
static void 	MEMPOOL_alloc_block		(uint8_t u8_block_index);
static void 	MEMPOOL_free_block		(uint8_t u8_block_index);

/****************************************************************************************************
 *	F U N C T I O N S
 ****************************************************************************************************/

/****************************************************************************************************
 *	Initializes the module
 * 
 * 	Creates a mutex for the memory pool and zeros out the registry.
 *
 ****************************************************************************************************/
void MEMPOOL_init(void)
{
	MEMPOOL_info.pool_mutex = xSemaphoreCreateMutexStatic(&MEMPOOL_info.pool_mutex_buffer);
	memset(MEMPOOL_info.pu32_allocation_registry, 0, sizeof(uint32_t));
}

/****************************************************************************************************
 *	Allocates a buffer from the memory pool
 *  
 * 	@warning Don't call this in ISR context
 * 
 *	@param[in] buffer_size_id A size ID, indicating the size of the desired buffer
 *
 * 	@return a generic buffer if one was available, else `NULL`
 ****************************************************************************************************/
MEMPOOL_buffer_t MEMPOOL_alloc(MEMPOOL_buffer_size_id_t buffer_size_id)
{
	ASSERT(buffer_size_id < MEMPOOL_BUFFER_SIZE_ID_NUM_SIZES);

	MEMPOOL_semaphore_take();

	MEMPOOL_buffer_t 	buffer;
	uint8_t				u8_blocks_required = kpu8_buffer_sizes[buffer_size_id] / MEMPOOL_MINIMUM_BLOCK_SIZE;
	uint8_t 			u8_consecutive_free = 0;
	uint8_t 			u8_start_index = 0;

	for (uint8_t i = 0; i < MEMPOOL_TOTAL_BLOCKS; i++)
	{
		if (MEMPOOL_is_block_free(i))
		{
			u8_consecutive_free++;

			if (u8_consecutive_free == u8_blocks_required)
			{
				u8_start_index = i - u8_blocks_required + 1;
				break;
			}
		}
		else
		{
			u8_consecutive_free = 0;
		}
	}

	if (u8_consecutive_free < u8_blocks_required)
	{
		return NULL;
	}

	for (uint8_t i = u8_start_index; i < u8_start_index + u8_blocks_required; i++)
	{
		MEMPOOL_alloc_block(i);
	}

	// Store the size allocated in the respective index of the registry
	MEMPOOL_info.pu32_allocation_registry[u8_start_index] = kpu8_buffer_sizes[buffer_size_id];

	buffer = &MEMPOOL_info.pu8_buffer_pool[u8_start_index * MEMPOOL_MINIMUM_BLOCK_SIZE];

	MEMPOOL_semaphore_give();

	return buffer;
}

/****************************************************************************************************
 *	Returns a buffer to the memory pool
 *  
 * 	@warning Don't call this in ISR context
 * 
 *	@param[in] p_buffer A pointer to the buffer to free
 *
 ****************************************************************************************************/
void MEMPOOL_free(MEMPOOL_buffer_t buffer)
{
	ASSERT(buffer);

    // Validate pointer
    ASSERT((uint8_t *)buffer >= MEMPOOL_info.pu8_buffer_pool && (uint8_t *)buffer < MEMPOOL_info.pu8_buffer_pool + MEMPOOL_TOTAL_SIZE);

    // Check alignment
    ASSERT(((uint8_t *)buffer - MEMPOOL_info.pu8_buffer_pool) % MEMPOOL_MINIMUM_BLOCK_SIZE == 0);

	MEMPOOL_semaphore_take();

	uint8_t u8_block_index = ((uint8_t *)buffer - MEMPOOL_info.pu8_buffer_pool) / MEMPOOL_MINIMUM_BLOCK_SIZE;
	uint8_t u8_num_blocks_to_free = (MEMPOOL_info.pu32_allocation_registry[u8_block_index] + MEMPOOL_MINIMUM_BLOCK_SIZE - 1) / MEMPOOL_MINIMUM_BLOCK_SIZE;

	for (uint8_t i = u8_block_index; i < u8_block_index + u8_num_blocks_to_free; i++)
	{
		MEMPOOL_free_block(i);
	}

	MEMPOOL_info.pu32_allocation_registry[u8_block_index] = 0;

	MEMPOOL_semaphore_give();
}

/****************************************************************************************************
 *	Checks whether a given block is free or not
 *  
 *	@param[in] u8_block_index The index of the block to check
 *
 * 	@return `true` if the block's index in the pool bitmask is 0, else `false`
 ****************************************************************************************************/
static bool MEMPOOL_is_block_free(uint8_t u8_block_index)
{
	return (MEMPOOL_info.pool_bitmask & (1 << u8_block_index)) == 0;
}

/****************************************************************************************************
 *	Marks a block of memory as allocated in the pool bitmask
 *  
 *	@param[in] u8_block_index The index of the block to allocate
 *
 ****************************************************************************************************/
static void MEMPOOL_alloc_block(uint8_t u8_block_index)
{
	MEMPOOL_info.pool_bitmask |= (1 << u8_block_index);
}

/****************************************************************************************************
 *	Marks a block of memory as free in the pool bitmask
 *  
 *	@param[in] u8_block_index The index of the block to free
 *
 ****************************************************************************************************/
static void MEMPOOL_free_block(uint8_t u8_block_index)
{
	MEMPOOL_info.pool_bitmask &= ~(1 << u8_block_index);
}
