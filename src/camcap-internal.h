#ifndef _CAMCAP_INTERNAL_H
#define _CAMCAP_INTERNAL_H

struct camcap_frame_ {
	uint32_t width;
	uint32_t height;
	uint32_t stride;
	
	uint8_t *image_data;
};

#endif
