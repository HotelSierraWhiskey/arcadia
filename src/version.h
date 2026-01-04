#ifndef VERSION_H
#define VERSION_H

/****************************************************************************************************
 *	D E F I N E S   &   T Y P E D E F S
 ****************************************************************************************************/

#define VERSION_MAJOR				(0)
#define VERSION_MINOR				(5)
#define VERSION_PATCH				(0)

#define VERSION_FREERTOS 			"202406.01-LTS"
#define VERSION_PART_ATSAMC21E18A 	"ATSAMC21E18A"
#define VERSION_PART_ATSAMC21N18A 	"ATSAMC21N18A"

#ifdef DEV_BOARD
#define VERSION_PROJECT_NAME		"Dev Board"
#define VERSION_PART				VERSION_PART_ATSAMC21E18A
#endif // DEV_BOARD

#endif // VERSION_H
