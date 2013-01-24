#ifndef _CAMCAP_QTKIT_NATIVE_STRUCTS_H
#define _CAMCAP_QTKIT_NATIVE_STRUCTS_H

#import <QTKit/QTKit.h>

@class CamCapCaptureDelegate;

struct camcap_native_device_ {
	QTCaptureDevice *capture_device;
	
	QTCaptureSession *capture_session;
	QTCaptureDeviceInput *device_input;
	QTCaptureDecompressedVideoOutput *video_output;
	
	CamCapCaptureDelegate *capture_delegate;
	
	NSDictionary *pixel_format_dict;
};

struct camcap_native_error_ {
	NSError *error;
};

#endif
