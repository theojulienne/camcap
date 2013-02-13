from ctypes import cdll, string_at, c_void_p, c_uint, c_char_p, c_bool, c_float
import os

class CamCapDevicePixelFormat:
	ARGB32 = 0

class CamCapFrame(object):
	def __init__( self, manager, frame_ptr ):
		self.manager = manager
		self.frame_ptr = frame_ptr
	
	def __enter__( self ):
		return self
	
	def __exit__( self, exc_type, exc_value, traceback ):
		self.manager.camcap.camcap_frame_destroy( self.frame_ptr )
		self.frame_ptr = 0

	@property
	def width( self ):
		return self.manager.camcap.camcap_frame_get_width( self.frame_ptr )
		
	@property
	def height( self ):
		return self.manager.camcap.camcap_frame_get_height( self.frame_ptr )

	@property
	def stride( self ):
		return self.manager.camcap.camcap_frame_get_stride( self.frame_ptr )

	@property
	def data_ptr( self ):
		return self.manager.camcap.camcap_frame_get_image_data( self.frame_ptr )

	@property
	def data( self ):
		return string_at(self.data_ptr, self.stride * self.height)

class CamCapDevice(object):
	def __init__( self, manager, device_ptr ):
		self.manager = manager
		self.device_ptr = device_ptr
	
	@property
	def name( self ):
		return self.manager.camcap.camcap_device_get_name_utf8( self.device_ptr )
	
	def open( self ):
		return self.manager.camcap.camcap_device_open( self.device_ptr )
	
	def close( self ):
		return self.manager.camcap.camcap_device_close( self.device_ptr )
	
	def request_format( self, pixel_format, width, height ):
		return self.manager.camcap.camcap_device_request_format( self.device_ptr, pixel_format, width, height )
	
	def capture_start( self ):
		return self.manager.camcap.camcap_device_capture_start( self.device_ptr )
	
	def capture_stop( self ):
		return self.manager.camcap.camcap_device_capture_stop( self.device_ptr )
	
	def capture_frame( self, timeout=1 ):
		frame_ptr = self.manager.camcap.camcap_device_capture_frame( self.device_ptr, timeout )
		
		if frame_ptr is None:
			return None
		
		return CamCapFrame( self.manager, frame_ptr )

class CamCapManager(object):
	class DeviceIterator(object):
		def __init__( self, enumerator ):
			self.enumerator = enumerator
			self.device_index = 0
		
		def next( self ):
			device_ptr = self.enumerator.manager.camcap.camcap_get_device_by_index( self.device_index )
			self.device_index += 1
			
			if device_ptr is None:
				raise StopIteration
			
			return CamCapDevice( self.enumerator.manager, device_ptr )
	
	class DeviceEnumerator(object):
		def __init__( self, manager ):
			self.manager = manager
		
		def __len__( self ):
			return self.manager.camcap.camcap_device_count( )
		
		def __iter__( self ):
			return self.manager.DeviceIterator( self )
		
		def __getitem__( self, index ):
			device_ptr = self.manager.camcap.camcap_get_device_by_index( index )
			
			if device_ptr is None:
				raise IndexError
			
			return CamCapDevice( self.manager, device_ptr )
	
	def __init__( self ):
		# FIXME: only call this once per process, rather than on each manager creation
		camcap_path = os.path.join( os.path.dirname( __file__ ), '_camcap.so' )
		
		self.camcap = cdll.LoadLibrary( camcap_path )
		
		self.camcap.camcap_device_count.restype = c_uint
		
		self.camcap.camcap_get_device_by_index.restype = c_void_p
		
		self.camcap.camcap_device_get_name_utf8.restype = c_char_p
		self.camcap.camcap_device_get_name_utf8.argtypes = [c_void_p]
		
		self.camcap.camcap_device_open.restype = c_bool
		self.camcap.camcap_device_open.argtypes = [c_void_p]
		
		self.camcap.camcap_device_close.argtypes = [c_void_p]
		
		self.camcap.camcap_device_request_format.argtypes = [c_void_p, c_uint, c_uint, c_uint]
		
		self.camcap.camcap_device_capture_start.argtypes = [c_void_p]
		
		self.camcap.camcap_device_capture_stop.argtypes = [c_void_p]
		
		self.camcap.camcap_device_capture_frame.restype = c_void_p
		self.camcap.camcap_device_capture_frame.argtypes = [c_void_p, c_float]
		
		self.camcap.camcap_frame_destroy.argtypes = [c_void_p]
		
		self.camcap.camcap_frame_get_width.restype = c_uint
		self.camcap.camcap_frame_get_width.argtypes = [c_void_p]
		
		self.camcap.camcap_frame_get_height.restype = c_uint
		self.camcap.camcap_frame_get_height.argtypes = [c_void_p]
		
		self.camcap.camcap_frame_get_stride.restype = c_uint
		self.camcap.camcap_frame_get_stride.argtypes = [c_void_p]
		
		self.camcap.camcap_frame_get_image_data.restype = c_void_p
		self.camcap.camcap_frame_get_image_data.argtypes = [c_void_p]

		self.camcap.camcap_init( )
	
	@property
	def devices( self ):
		return self.DeviceEnumerator( self )

class CamCapDeviceError(Exception):
	pass