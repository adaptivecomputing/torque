#include "license_pbs.h" /* See here for the software license */
#include "trq_auth_daemon.h"

/* Wrapper for main, allows for unit testing */
int main(
    int argc,
    char **argv,
    char **envp)
  {
  return trq_main(argc, argv, envp);
  }
