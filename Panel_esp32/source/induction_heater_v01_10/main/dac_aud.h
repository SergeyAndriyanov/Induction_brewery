//---------------------------------------------------------------------------------------------------
/*
*/
//---------------------------------------------------------------------------------------------------
#ifndef DAC_AUD_H_
#define DAC_AUD_H_
//-----------------------------------------------------------------------------//

#define LOAD_AUDIO 0
#define KEYPRESS_AUDIO 1
#define MESHRUN_AUDIO 2
#define BOILINRUN_AUDIO 3
#define TERMPPAUSENRUN_AUDIO 4
#define MESHDONE_AUDIO 5
#define HOPSADD_AUDIO 6
#define BOILINGDONE_AUDIO 7
#define ALARMSENSOR_AUDIO 8 
#define DACSAMPLERAIT 11025
#define STACK_SIZE_AUDIO_TASK 2000
#define STOPPLAY  0
#define STARTPLAY 1



typedef struct _audio_info
{
	uint32_t sampleRate;
	size_t dataLength;
	uint8_t *data;
	size_t bytewrite;
	uint8_t startplay;
} audio_info_t;


extern  audio_info_t sound_wav_info ;
extern SemaphoreHandle_t ReplaySemaphore;

void selectplayaudio(uint8_t numaud);
void task_audio(void *arg);
void runtaskaudio();
void installdacdma();
void unistaldacdma();
void createsemaphre();










//-----------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------------------------------


