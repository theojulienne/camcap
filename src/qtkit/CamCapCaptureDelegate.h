#include "camcap.h"
#include "camcap-native.h"

@interface CamCapCaptureDelegate : NSObject
{
	camcap_native_device_t *native;
	camcap_frame_t *frame;
	
	NSMutableArray *frameQueue;
}

- (id)initWithNativeDevice:(camcap_native_device_t *)nativeDevice;

- (void)requestFrameFill:(camcap_frame_t *)frame withTimeout:(float)timeoutSeconds;

- (void)prepare;
- (void)startRunning;

@end