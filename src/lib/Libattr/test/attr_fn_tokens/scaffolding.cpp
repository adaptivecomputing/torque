#include "license_pbs.h" /* See here for the software license */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "attribute.h" /* pbs_attribute */
#include "pbs_error.h"

int decode_str(

  pbs_attribute *patr,   /* (I modified, allocated ) */
  const char  *name,   /* (I - optional) pbs_attribute name */
  const char *rescn,  /* resource name - unused here */
  const char    *val,    /* pbs_attribute value */
  int            perm)   /* only used for resources */
        
  {     
  size_t len;
          
  if (patr->at_val.at_str != NULL)
    {
    free(patr->at_val.at_str);
    patr->at_val.at_str = NULL;
    }
          
  if ((val != NULL) && ((len = strlen(val) + 1) > 1))
    {
    patr->at_val.at_str = (char *)calloc(1, (unsigned)len);

    if (patr->at_val.at_str == NULL)
      {
      /* FAILURE - cannot allocate memory */

      return(PBSE_SYSTEM);
      }

    strcpy(patr->at_val.at_str, val);

    patr->at_flags |= ATR_VFLAG_SET | ATR_VFLAG_MODIFY;
    }
  else
    {
    /* string is empty */

    patr->at_flags = (patr->at_flags & ~ATR_VFLAG_SET) | ATR_VFLAG_MODIFY;
                      patr->at_val.at_str = NULL;
    }

    /* SUCCESS */

    return(0);
    }  /* END decode_str() */

