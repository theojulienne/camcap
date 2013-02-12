#include "camcap.h"
#include "camcap-native.h"

@interface CamCapCaptureDelegate : NSObject

- (id)initWithNativeDevice:(camcap_native_device_t *)nativeDevice;

- (void)requestFrameFill:(camcap_frame_t *)frame withTimeout:(float)timeoutSeconds;

- (void)prepare;
- (void)startRunning;

@end