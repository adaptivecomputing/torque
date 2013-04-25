#include "license_pbs.h" /* See here for the software license */
#include "qsub_functions.h"
#include "net_cache.h"

/** 
 * qsub main - processing moved to another function to facilitate unit testing
 */

int main(

  int    argc,  /* I */
  char **argv,  /* I */
  char **envp)  /* I */

  {
  initialize_network_info();

  main_func(argc, argv, envp);

  return(0);
  } /* END main() */


