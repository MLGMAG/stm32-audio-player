#ifndef __COMMONS_H
#define __COMMONS_H

#include "stdint.h"

typedef enum {
	UAL_STATUS_OK, UAL_STATUS_ERROR,
} UAL_STATUS_t;

typedef struct {
	uint64_t data_section_pointer;
	uint32_t file_size;
	uint32_t data_size;
	uint32_t sample_rate;
	uint32_t bytes_per_second;
	uint16_t audio_format;
	uint16_t channels_num;
	uint16_t bytes_per_sample;
	uint16_t bits_per_sample_per_channel;
} WAV_FILE_METADATA_t;

void UAL_Error_Handler(void);

#endif /* __COMMONS_H */
