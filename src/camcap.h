#ifndef CAMCAP_H
#define CAMCAP_H

#include <stdint.h>
#include <stdbool.h>

typedef struct camcap_device_ camcap_device_t;
typedef struct camcap_error_ camcap_error_t;
typedef struct camcap_frame_ camcap_frame_t;

enum {
	ccDevicePixelFormatARGB32=0,
};

void camcap_init( void );
uint32_t camcap_device_count( void );
camcap_device_t *camcap_get_device_by_index( uint32_t index );
void camcap_device_destroy( camcap_device_t *device );

char *camcap_device_get_name_utf8( camcap_device_t *device );
bool camcap_device_open( camcap_device_t *device, camcap_error_t **error );
void camcap_device_close( camcap_device_t *device );
void camcap_device_request_format( camcap_device_t *device, uint32_t pixel_format, uint32_t width, uint32_t height );
void camcap_device_capture_start( camcap_device_t *device );
void camcap_device_capture_stop( camcap_device_t *device );

camcap_frame_t *camcap_device_capture_frame( camcap_device_t *device, float timeoutSeconds );
void camcap_frame_destroy( camcap_frame_t *frame );

char *camcap_error_get_message_utf8( camcap_error_t *error );
void camcap_error_destroy( camcap_error_t *error );

uint32_t camcap_frame_get_width( camcap_frame_t *frame );
uint32_t camcap_frame_get_height( camcap_frame_t *frame );
uint32_t camcap_frame_get_stride( camcap_frame_t *frame );
uint8_t *camcap_frame_get_image_data( camcap_frame_t *frame );

#endif
