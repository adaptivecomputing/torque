#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>

#include "list_link.h" /* list_link */

void im_dying()
  {
    fprintf(stderr,"I'm in %s and I'm dying.\n",__FILE__);
    exit(1);
  }

