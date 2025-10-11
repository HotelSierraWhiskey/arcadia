#ifndef VERSION_H
#define VERSION_H


#define VERSION_MAJOR			(0)
#define VERSION_MINOR			(3)
#define VERSION_PATCH			(0)

#define VERSION_FREERTOS 		"202406.01-LTS"

#ifdef AUDIO_SWITCH
#define VERSION_PROJECT_NAME	"Audio Switch"
#endif // AUDIO_SWITCH

#ifdef DEV_BOARD
#define VERSION_PROJECT_NAME	"Dev Board"
#endif // DEV_BOARD

#endif // VERSION_H
