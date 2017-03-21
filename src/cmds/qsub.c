#include "license_pbs.h" /* See here for the software license */
#include "qsub_functions.h"
#include "net_cache.h"
#include <iostream>
/** 
 * qsub main - processing moved to another function to facilitate unit testing
 */

int main(

  int    argc,  /* I */
  char **argv,  /* I */
  char **envp)  /* I */

  {
  try
    {
    main_func(argc, argv, envp);
    }
  catch(std::exception& e)
    {
    //what goes here?
    std::cout<<e.what();
    }
  return(0);
  } /* END main() */


