#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <poll.h>

void log_err(int errnum, const char *func_id, const char *msg) {}

int   global_poll_rc = 0;
short global_poll_revents = 0;

int poll(struct pollfd *fds, nfds_t nfds, int timeout)
  {
  fds->revents = global_poll_revents;

  return(global_poll_rc);
  }
