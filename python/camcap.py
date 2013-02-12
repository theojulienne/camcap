from ctypes import cdll
import os

class CamCapDevicePixelFormat:
	ccDevicePixelFormatARGB32 = 0

class CamCapManager(object):
	def __init__( self ):
		# FIXME: only call this once per process, rather than on each manager creation
		camcap_path = os.path.join( os.path.dirname( __file__ ), '_camcap.so' )
		self.camcap = cdll.LoadLibrary( camcap_path )
		self.camcap.camcap_init( )

class CamCapDeviceError(Exception):
	pass