from camcap import CamCapManager, CamCapDeviceError, CamCapDevicePixelFormat

import sys

def enumerate_devices( ):
	manager = CamCapManager( )
	
	num_devices = len(manager.devices)
	
	print 'Enumerating %d devices...' % (num_devices,)
	
	for i, device in enumerate(manager.devices):
		print ' [%d] %s' % (i, device.name)
	
	print
	print 'To capture an image: %s <deviceIndex> <filename>' % (sys.argv[0],)

def capture_image( device_specifier, filename ):
	try:
		chosen_device = int(device_specifier)
	except ValueError:
		print >> sys.stderr, 'Usage: %s <deviceIndex> <filename>' % (sys.argv[0],)
		sys.exit( 1 )
	
	manager = CamCapManager( )

	num_devices = len(manager.devices)
	
	if chosen_device >= num_devices:
		print >> sys.stderr, 'Error: No such device, run this program with no arguments to enumerate devices.'
		sys.exit( 1 )
	
	print 'Capturing image from device %d...' % (chosen_device,)
	
	device = manager.devices[chosen_device]
	
	try:
		device.open( )
	except CamCapDeviceError as e:
		print >> sys.stderr, 'Error: Could not get exclusive access to device: %s' % e.message
		sys.exit( 1 )
	
	device.request_format( CamCapDevicePixelFormat.ARGB32, 1024, 768 )
	
	device.capture_start( )
	
	with device.capture_frame( timeout=2 ) as frame:
		if frame is None:
			print >> sys.stderr, 'No frame captured after waiting for 2 seconds.'
			sys.exit( 1 )
	
		print 'Got image %dx%d' % (frame.width, frame.height)
		sys.stdout.flush()
		
		# discard alpha
		data = ''.join(b for i, b in enumerate(frame.data) if i % 4 != 0)
		stride = frame.stride / 4 * 3
		
		print 'Writing to %r...' % (filename,)
		import pyjpeg
		pyjpeg.write_file( filename, data, frame.width, frame.height, stride )
	
	device.capture_stop( )
	device.close( )

if __name__ == '__main__':
	if len(sys.argv) == 1:
		enumerate_devices( )
	elif len(sys.argv) == 3:
		capture_image( sys.argv[1], sys.argv[2] )
	else:
		print >> sys.stderr, 'Usage:'
		print >> sys.stderr, '  Enumerate cameras:     %s' % (sys.argv[0],)
		print >> sys.stderr, '  Capture a raw image:   %s <deviceIndex> <filename>' % (sys.argv[0],)
		sys.exit( 1 )