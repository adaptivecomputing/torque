#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */
#include "log.h" /* LOG_BUF_SIZE */

char log_buffer[LOG_BUF_SIZE];

void log_err(int errnum, const char *routine, const char *text) {}

void log_event(int eventtype, int objclass, const char *objname, const char *text) {}

void log_ext(int errnum, const char *routine, const char *text, int severity) { }

void log_record(int eventtype, int objclass, const char *objname, const char *text) { }



