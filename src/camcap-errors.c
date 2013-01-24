#include <stdlib.h>
#include <assert.h>

#include "camcap.h"
#include "camcap-native.h"
#include "camcap-errors.h"

extern camcap_native_funcs_t *camcap_native_functions;

camcap_error_t *camcap_error_from_native( camcap_native_error_t *native_error ) {
	camcap_error_t *error = malloc( sizeof(camcap_error_t) );
	assert( error != NULL );
	
	error->native_error = native_error;
	error->message = NULL;
	
	return error;
}

char *camcap_error_get_message_utf8( camcap_error_t *error ) {
	assert( error != NULL );
	
	if ( error->message == NULL ) {
		error->message = camcap_native_functions->error_get_message_utf8( error->native_error );
	}
	
	return error->message;
}

void camcap_error_destroy( camcap_error_t *error ) {
	assert( error != NULL );
	
	if ( error->native_error != NULL ) {
		camcap_native_functions->error_destroy( error->native_error );
	}
	
	if ( error->message != NULL ) {
		free( error->message );
	}
	
	free( error );
}
