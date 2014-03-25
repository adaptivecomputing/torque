#include <stdlib.h>
#include <stdio.h>

#include "batch_request.h"

bool exit_called = false;
batch_request_holder    brh;


void log_err(int errnum, const char *routine, const char *text) {}
