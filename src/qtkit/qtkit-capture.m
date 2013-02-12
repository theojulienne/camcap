#include <stdlib.h>
#include <assert.h>

#import <QTKit/QTKit.h>

#include "camcap.h"
#include "camcap-native.h"
#include "camcap-internal.h"
#include "qtkit-native-structs.h"

#include "CamCapCaptureDelegate.h"

static void camcap_qtkit_enumerate( void );
static uint32_t camcap_qtkit_get_num_capture_devices( void );
static camcap_native_device_t *camcap_qtkit_get_device_at_index( uint32_t index );

static void camcap_qtkit_device_destroy( camcap_native_device_t *native );
static char *camcap_qtkit_device_get_name_utf8( camcap_native_device_t *native );
static bool camcap_qtkit_device_open( camcap_native_device_t *native, camcap_native_error_t **error );
static void camcap_qtkit_device_close( camcap_native_device_t *native );
static void camcap_qtkit_device_capture_start( camcap_native_device_t *native );
static void camcap_qtkit_device_capture_stop( camcap_native_device_t *native );
static bool camcap_qtkit_device_capture_frame( camcap_native_device_t *native, camcap_frame_t *frame, float timeoutSeconds );
static void camcap_qtkit_device_request_format( camcap_native_device_t *native, uint32_t pixel_format, uint32_t width, uint32_t height );

static void camcap_qtkit_error_destroy( camcap_native_error_t *native );
static char *camcap_qtkit_error_get_message_utf8( camcap_native_error_t *native );

static NSArray *enumerated_devices = nil;

static camcap_native_funcs_t qtkit_funcs = {
	.enumerate_devices = camcap_qtkit_enumerate,
	.get_num_capture_devices = camcap_qtkit_get_num_capture_devices,
	.get_device_at_index = camcap_qtkit_get_device_at_index,
	
	.device_destroy = camcap_qtkit_device_destroy,
	.device_get_name_utf8 = camcap_qtkit_device_get_name_utf8,
	.device_open = camcap_qtkit_device_open,
	.device_close = camcap_qtkit_device_close,
	.device_request_format = camcap_qtkit_device_request_format,
	.device_capture_start = camcap_qtkit_device_capture_start,
	.device_capture_stop = camcap_qtkit_device_capture_stop,
	.device_capture_frame = camcap_qtkit_device_capture_frame,
	
	.error_destroy = camcap_qtkit_error_destroy,
	.error_get_message_utf8 = camcap_qtkit_error_get_message_utf8,
};

camcap_native_funcs_t *camcap_native_init( void ) {
	return &qtkit_funcs;
}

static void camcap_qtkit_enumerate( void ) {
	enumerated_devices = [QTCaptureDevice inputDevicesWithMediaType:QTMediaTypeVideo];
}

static uint32_t camcap_qtkit_get_num_capture_devices( void ) {
	return [enumerated_devices count];
}

static camcap_native_device_t *camcap_qtkit_get_device_at_index( uint32_t index ) {
	QTCaptureDevice *device = [enumerated_devices objectAtIndex:index];
	assert( device != nil );
	
	camcap_native_device_t *native = malloc( sizeof(camcap_native_device_t) );
	assert( native != NULL );
	
	native->capture_device = [device retain];
	native->capture_session = nil;
	native->video_output = nil;
	native->device_input = nil;
	native->capture_delegate = nil;
	
	// default
	native->pixel_format_dict = @{
		(id)kCVPixelBufferWidthKey: [NSNumber numberWithDouble:320.0],
		(id)kCVPixelBufferHeightKey: [NSNumber numberWithDouble:240.0],
		(id)kCVPixelBufferPixelFormatTypeKey: [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32ARGB],
	};
	[native->pixel_format_dict retain];
	
	return native;
}

static void camcap_qtkit_device_destroy( camcap_native_device_t *native ) {
	[native->capture_device release];
	
	free( native );
}

static char *camcap_qtkit_device_get_name_utf8( camcap_native_device_t *native ) {
	return strdup( [[native->capture_device localizedDisplayName] UTF8String] );
}

static camcap_native_error_t *camcap_qtkit_map_error( NSError *error ) {
	camcap_native_error_t *native = malloc( sizeof(camcap_native_error_t) );
	assert( native != NULL );
	
	native->error = [error retain];
	
	return native;
}

static bool camcap_qtkit_device_open( camcap_native_device_t *native, camcap_native_error_t **error ) {
	QTCaptureDevice *device = native->capture_device;
	
	NSError *nserror = nil;
	bool success = [device open:&nserror];
	
	if ( !success ) {
		// map the error over
		*error = camcap_qtkit_map_error( nserror );
	}
	
	return success;
}

static void camcap_qtkit_device_close( camcap_native_device_t *native ) {
	QTCaptureDevice *device = native->capture_device;
	[device close];
}

static void camcap_qtkit_device_request_format( camcap_native_device_t *native, uint32_t pixel_format, uint32_t width, uint32_t height ) {
	[native->pixel_format_dict release];
	
	assert( pixel_format == ccDevicePixelFormatARGB32 );
	native->pixel_format_dict = @{
		(id)kCVPixelBufferWidthKey: [NSNumber numberWithDouble:width],
		(id)kCVPixelBufferHeightKey: [NSNumber numberWithDouble:height],
		(id)kCVPixelBufferPixelFormatTypeKey: [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32ARGB],
	};
	
	[native->pixel_format_dict retain];
	
	if ( native->video_output != nil ) {
		[native->video_output setPixelBufferAttributes:native->pixel_format_dict];
	}
}

static void camcap_qtkit_device_capture_start( camcap_native_device_t *native ) {
	NSError *error = nil;
	
	if ( native->capture_delegate == nil ) {
		native->capture_delegate = [[[CamCapCaptureDelegate alloc] initWithNativeDevice:native] retain];
		assert( native->capture_delegate != nil );
	}
	
	NSLog( @"performing initAndStartRunning" );
	[native->capture_delegate prepare];
	[native->capture_delegate startRunning];
//	[native->capture_delegate performSelectorInBackground:@selector(startRunning) withObject:nil];
//	[native->capture_delegate initAndStartRunning];
}

static void camcap_qtkit_device_capture_stop( camcap_native_device_t *native ) {
	[native->capture_session stopRunning];
}

static bool camcap_qtkit_device_capture_frame( camcap_native_device_t *native, camcap_frame_t *frame, float timeoutSeconds ) {
	if ( ![native->capture_session isRunning] ) {
		return false;
	}
	
	frame->image_data = NULL;
	[native->capture_delegate requestFrameFill:frame withTimeout:timeoutSeconds];
	
	return true;
}

static void camcap_qtkit_error_destroy( camcap_native_error_t *native ) {
	[native->error release];
	
	free( native );
}

static char *camcap_qtkit_error_get_message_utf8( camcap_native_error_t *native ) {
	return strdup( [[native->error localizedDescription] UTF8String] );
}
