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
    std::string error = e.what();
    if(error == "")
      {
      return(0);
      }
    else if(error.find_first_not_of( "0123456789" ) == std::string::npos)
      {
      int exit_code = atoi(error.c_str());
      return(exit_code);
      }
    else
      {
      std::cout<<error;
      return (-1);
      }
    }
  return(0);
  } /* END main() */


