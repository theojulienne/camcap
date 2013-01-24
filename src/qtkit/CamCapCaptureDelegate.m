#include "qtkit-native-structs.h"

#include "CamCapCaptureDelegate.h"

#include "camcap-internal.h"

@interface CamCapCaptureDelegate ()
{
	camcap_native_device_t *native;
	camcap_frame_t *frame;
}
@end

@implementation CamCapCaptureDelegate

- (id)initWithNativeDevice:(camcap_native_device_t *)nativeDevice
{
	self = [super init];
	if ( self ) {
		native = nativeDevice;
	}
	return self;
}

- (void)prepare {
	NSError *error = nil;
	
	NSLog( @"starting!" );
	
	if ( native->device_input == nil ) {
		native->device_input = [[QTCaptureDeviceInput deviceInputWithDevice:native->capture_device] retain];
		assert( native->device_input != nil );
	}
	
	NSLog( @"creating video output..." );
	
	if ( native->video_output == nil ) {
		native->video_output = [[[QTCaptureDecompressedVideoOutput alloc] init] retain];
		native->video_output.delegate = native->capture_delegate;

		[native->video_output setPixelBufferAttributes:native->pixel_format_dict];
	}
	
	NSLog( @"creating session..." );

	if ( native->capture_session == nil ) {
		native->capture_session = [[[QTCaptureSession alloc] init] retain];
		NSLog( @"adding input" );
		[native->capture_session addInput:native->device_input error:&error];
		assert( error == nil );
		NSLog( @"adding output" );
		[native->capture_session addOutput:native->video_output error:&error];
		assert( error == nil );
	}
	
}

- (void)startRunning {
	NSLog( @"Going to start running..." );

	[native->capture_session startRunning];
	assert( [native->capture_session isRunning] );
	
	//[native->capture_session performSelectorInBackground:@selector(startRunning) withObject:nil];
	[[NSRunLoop mainRunLoop] runUntilDate:[NSDate distantFuture]];
	
	NSLog( @"rawr" );
}

- (void)captureOutput:(QTCaptureOutput *)captureOutput didOutputVideoFrame:(CVImageBufferRef)videoFrame withSampleBuffer:(QTSampleBuffer *)sampleBuffer fromConnection:(QTCaptureConnection *)connection {
	if ( frame != NULL ) {
		NSLog( @"frame" );
		CVPixelBufferLockBaseAddress( videoFrame, 0 ); 
	
		uint8_t *data = (uint8_t *)CVPixelBufferGetBaseAddress( videoFrame );
		
		frame->stride = CVPixelBufferGetBytesPerRow( videoFrame ); 
		frame->width = CVPixelBufferGetWidth( videoFrame ); 
		frame->height = CVPixelBufferGetHeight( videoFrame ); 
		
		size_t image_bytes = frame->stride * frame->height;
		if ( frame->image_data == NULL ) {
			frame->image_data = malloc( image_bytes );
		}
		assert( frame->image_data != NULL );
		memcpy( frame->image_data, data, image_bytes );
		
		CVPixelBufferUnlockBaseAddress( videoFrame, 0 );
	}
	
	//CFRunLoopStop( CFRunLoopGetCurrent() );
}

- (void)setFrame:(camcap_frame_t *)_frame {
	frame = _frame;
}

@end