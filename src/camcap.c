#include <stdlib.h>
#include <assert.h>

#include "camcap.h"
#include "camcap-native.h"
#include "camcap-errors.h"
#include "camcap-internal.h"

struct camcap_device_ {
	camcap_native_device_t *native_device;
	char *name;
};

camcap_native_funcs_t *camcap_native_functions;

void camcap_init( void ) {
	camcap_native_functions = camcap_native_init( );
	
	camcap_native_functions->enumerate_devices( );
}

void camcap_destroy( void ) {
	//
}

void camcap_device_destroy( camcap_device_t *device ) {
	if ( device->native_device != NULL ) {
		camcap_native_functions->device_destroy( device->native_device );
	}
	if ( device->name != NULL ) {
		free( device->name );
	}
	free( device );
}

uint32_t camcap_device_count( void ) {
	return camcap_native_functions->get_num_capture_devices( );
}

camcap_device_t *camcap_get_device_by_index( uint32_t index ) {
	if ( index >= camcap_device_count( ) ) {
		return NULL;
	}
	
	camcap_native_device_t *native_device = camcap_native_functions->get_device_at_index( index );
	if ( native_device == NULL ) {
		return NULL; // eek
	}
	
	assert( native_device != NULL );
	
	camcap_device_t *device = malloc( sizeof(camcap_device_t) );
	assert( device != NULL );
	
	device->native_device = native_device;
	device->name = NULL;
	
	return device;
}

char *camcap_device_get_name_utf8( camcap_device_t *device ) {
	assert( device != NULL );
	
	if ( device->name == NULL ) {
		device->name = camcap_native_functions->device_get_name_utf8( device->native_device );
	}
	
	return device->name;
}

bool camcap_device_open( camcap_device_t *device, camcap_error_t **error ) {
	assert( device != NULL );
	
	camcap_native_error_t *native_error;
	
	bool success = camcap_native_functions->device_open( device->native_device, &native_error );
	
	if ( !success ) {
		if ( error != NULL ) {
			// user wants the error, so pass it through
			*error = camcap_error_from_native( native_error );
		} else {
			// user is being lazy, kill the native error
			camcap_native_functions->error_destroy( native_error );
		}
	}
	
	return success;
}

void camcap_device_close( camcap_device_t *device ) {
	assert( device != NULL );
	
	camcap_native_functions->device_close( device->native_device );
}

void camcap_device_request_format( camcap_device_t *device, uint32_t pixel_format, uint32_t width, uint32_t height ) {
	assert( device != NULL );
	
	camcap_native_functions->device_request_format( device->native_device, pixel_format, width, height );
}

void camcap_device_capture_start( camcap_device_t *device ) {
	assert( device != NULL );
	
	camcap_native_functions->device_capture_start( device->native_device );
}

void camcap_device_capture_stop( camcap_device_t *device ) {
	assert( device != NULL );
	
	camcap_native_functions->device_capture_stop( device->native_device );
}

camcap_frame_t *camcap_device_capture_frame( camcap_device_t *device, float timeoutSeconds ) {
	camcap_frame_t *frame = malloc( sizeof(camcap_frame_t) );
	assert( frame != NULL );
	
	frame->image_data = NULL;
	
	bool success = camcap_native_functions->device_capture_frame( device->native_device, frame, timeoutSeconds );
	
	if ( !success ) {
		free( frame );
		return NULL;
	}
	
	return frame;
}

void camcap_frame_destroy( camcap_frame_t *frame ) {
	free( frame->image_data );
	free( frame );
}

uint32_t camcap_frame_get_width( camcap_frame_t *frame ) {
	return frame->width;
}

uint32_t camcap_frame_get_height( camcap_frame_t *frame ) {
	return frame->height;
}

uint32_t camcap_frame_get_stride( camcap_frame_t *frame ) {
	return frame->stride;
}

uint8_t *camcap_frame_get_image_data( camcap_frame_t *frame ) {
	return frame->image_data;
}