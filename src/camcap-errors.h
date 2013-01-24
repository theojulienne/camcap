#include "camcap-native.h"

struct camcap_error_ {
	camcap_native_error_t *native_error;
	char *message;
};

camcap_error_t *camcap_error_from_native( camcap_native_error_t *native_error );
