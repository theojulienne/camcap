#include <stdio.h>
#include <unistd.h>

#include <camcap.h>

static char *programName = "capture";

static int enumerate_devices( void );
static int capture_image( char *device_specifier, char *filename );

int main( int argc, char *argv[] ) {
	camcap_init( );
	
	programName = argv[0];
	
	if ( argc == 1 ) {
		return enumerate_devices( );
	} else if ( argc == 3 ) {
		return capture_image( argv[1], argv[2] );
	} else {
		fprintf( stderr, "Usage:\n" );
		fprintf( stderr, "  Enumerate cameras:     %s\n", programName );
		fprintf( stderr, "  Capture a raw image:   %s <deviceIndex> <filename>\n", programName );
		return 1;
	}
	
	return 0;
}


int enumerate_devices( void ) {
	uint32_t num_devices = camcap_device_count( );
	
	printf( "Enumerating %d devices...\n", num_devices );
	
	for ( uint32_t i = 0; i < num_devices; i++ ) {
		camcap_device_t *device = camcap_get_device_by_index( i );
		
		printf( " [%d] %s\n", i, camcap_device_get_name_utf8( device ) );

		camcap_device_destroy( device );
	}
	
	printf( "\n" );
	printf( "To capture an image: %s <deviceIndex> <filename>\n", programName );
	
	return 0;
}

int capture_image( char *device_specifier, char *filename ) {
	uint32_t chosen_device;
	
	if ( sscanf( device_specifier, "%d", &chosen_device ) != 1 ) {
		fprintf( stderr, "Usage: %s <deviceIndex> <filename>\n", programName );
		return 1;
	}
	
	uint32_t num_devices = camcap_device_count( );
	if ( chosen_device >= num_devices ) {
		fprintf( stderr, "Error: No such device, run this program with no arguments to enumerate devices.\n" );
		return 1;
	}
	
	printf( "Capturing image from device %d...\n", chosen_device );
	
	camcap_device_t *device = camcap_get_device_by_index( chosen_device );
	
	// initialise: get control of the device and check for errors
	camcap_error_t *error = NULL;
	if ( !camcap_device_open( device, &error ) ) {
		fprintf( stderr, "Error: Could not get exclusive access to device: %s\n",
			camcap_error_get_message_utf8( error )
		);
		
		camcap_error_destroy( error );
		camcap_device_close( device );
		return 1;
	}
	
	camcap_device_request_format( device, ccDevicePixelFormatARGB32, 1024, 768 );
	
	camcap_device_capture_start( device );
	
	for ( int i = 0; i < 10; i++ ) {
		camcap_frame_t *frame = camcap_device_capture_frame( device, 2 );
		if ( frame == NULL ) {
			fprintf( stderr, "No frame captured after waiting for 2 seconds.\n" );
			camcap_device_capture_stop( device );
			camcap_error_destroy( error );
			camcap_device_close( device );
			return 1;
		}
	
		printf( "got image %dx%d\n", camcap_frame_get_width(frame), camcap_frame_get_height(frame) );
	/*
		FILE *fp = fopen( filename, "wb" );
		fwrite( camcap_frame_get_image_data(frame), camcap_frame_get_stride(frame), camcap_frame_get_height(frame), fp );
		fclose( fp );
		*/
		camcap_frame_destroy( frame );
	}
	
	camcap_device_capture_stop( device );
	
	// we're done! clean up
	camcap_device_close( device );
	camcap_device_destroy( device );
	
	return 0;
}
