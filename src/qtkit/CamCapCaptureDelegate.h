#include "camcap.h"
#include "camcap-native.h"

@interface CamCapCaptureDelegate : NSObject

- (id)initWithNativeDevice:(camcap_native_device_t *)nativeDevice;

- (void)setFrame:(camcap_frame_t *)frame;

- (void)prepare;
- (void)startRunning;

@end