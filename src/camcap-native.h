#ifndef NATIVE_H
#define NATIVE_H

#include <stdint.h>

typedef struct camcap_native_device_ camcap_native_device_t;
typedef struct camcap_native_error_ camcap_native_error_t;

typedef struct {
	void (*enumerate_devices)( void );
	uint32_t (*get_num_capture_devices)( void );
	camcap_native_device_t *(*get_device_at_index)( uint32_t );
	
	void (*device_destroy)( camcap_native_device_t * );
	char *(*device_get_name_utf8)( camcap_native_device_t * );
	bool (*device_open)( camcap_native_device_t *, camcap_native_error_t ** );
	void (*device_close)( camcap_native_device_t * );
	void (*device_request_format)( camcap_native_device_t *, uint32_t, uint32_t, uint32_t );
	void (*device_capture_start)( camcap_native_device_t * );
	void (*device_capture_stop)( camcap_native_device_t * );
	bool (*device_capture_frame)( camcap_native_device_t *, camcap_frame_t *, float );
	
	void (*error_destroy)( camcap_native_error_t * );
	char *(*error_get_message_utf8)( camcap_native_error_t * );
} camcap_native_funcs_t;

camcap_native_funcs_t *camcap_native_init( void );

#endif
