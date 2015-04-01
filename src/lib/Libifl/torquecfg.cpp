#include <pbs_config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "pbs_ifl.h"
#include "pbs_error.h"

#define TCONST_CFGFILE "torque.cfg"

#define TRQ_IFNAME "TRQ_IFNAME"

int load_config(

  char *config_buf, /* O */
  int   BufSize)    /* I */

  {
  FILE *config_stream;
  char home_dir[MAXPATHLEN];
  int  length = strlen(PBS_SERVER_HOME) + strlen(TCONST_CFGFILE) + 1;
  char *ptr;

  if (length >= MAXPATHLEN)
    {
    /* FAILURE */

    return(1);
    }

  memset(config_buf, 0, BufSize);

  snprintf(home_dir, sizeof(home_dir), "%s/%s",
    PBS_SERVER_HOME, TCONST_CFGFILE);

  if ((config_stream = fopen(home_dir, "r")) == NULL)
    {
    /* FAILURE */
    return(1);
    }

  if ((fread(config_buf, BufSize - 1, 1, config_stream) <= 0) &&
      (ferror(config_stream) != 0))
    {
    /* FAILURE */
    fclose(config_stream);
    return(1);
    }

  ptr = config_buf;

  while ((ptr = strchr(ptr, '#')) != NULL)
    {
    ptr++;
    for (;(*ptr != '\0') && (*ptr != '\n');ptr++)
      {
      *ptr = ' ';
      }
    } /* END while ((ptr = strchr(ptr,'#')) != NULL) */

  /* SUCCESS */
  fclose(config_stream);

  return(PBSE_NONE);
  }  /* END load_config() */




char *get_trq_param(

  const char *param,      /* I */
  const char *config_buf) /* I */

  {
  char tmpLine[1024];

  const char *param_val;
  char *new_val = NULL;

  /* FORMAT:  <PARAM> <WS> <VALUE> \n */

  /* NOTE:  does not support comments */

  /* if (strcasestr() == NULL) */

  /* NOTE: currently case-sensitive (FIXME) */

  if ((param_val = strstr(config_buf, param)) == NULL)
    {
    return(NULL);
    }

  snprintf(tmpLine, sizeof(tmpLine), "%s", param_val);

  strtok(tmpLine, " \t\n=");

  if ((new_val = (char *)strtok(NULL, "\t =\n")) == NULL)
    {
    return(NULL);
    }

  return(new_val);
  }  /* END get_trq_param() */


/* trq_get_if_name:  This function parses the torque.cfg file
 * for the TRQ_IFNAME keyword and if set returns the value. 
 * The variable if_name is allocated space for the new 
 * interface name. It must be released by the called function. 
 */
char *trq_get_if_name()
  {
  struct stat filestruct;
  char home_dir[MAXPATHLEN];
  char *torque_cfg_buf;
  char *if_name, *ptr;
  int  length = strlen(PBS_SERVER_HOME) + strlen(TCONST_CFGFILE) + 1;
  int  file_size;
  int  rc;

  if (length > MAXPATHLEN)
    return(NULL); /* How are we going to return error information */

  home_dir[0] = '\0';
  snprintf(home_dir, sizeof(home_dir), "%s/%s",
    PBS_SERVER_HOME, TCONST_CFGFILE);

  rc = stat(home_dir, &filestruct);
  if (rc < 0)
    return(NULL); /* this is not an error. torque.cfg file is optional */

  file_size = filestruct.st_size;

  /* we know the size of the torque.cfg file. Allocate some space for it */
  torque_cfg_buf = (char *)calloc(1, file_size+1);
  if (torque_cfg_buf == NULL)
    {
    fprintf(stderr, "failed to allocate memory in trq_get_if_name\n");
    return(NULL);
    }

  rc = load_config(torque_cfg_buf, file_size);
  if (rc)
    {
	if (torque_cfg_buf)
      free(torque_cfg_buf);
    return(NULL);
	}

  ptr = get_trq_param((char *)TRQ_IFNAME, torque_cfg_buf);
  if (ptr == NULL)
    {
    if (torque_cfg_buf)
	    free(torque_cfg_buf);
    return(NULL);
    }

  /* we have a name. We need to copy it to permanent storage */
  if_name = (char *)calloc(1, strlen(ptr)+1);
  if (if_name == NULL)
    {
	fprintf(stderr, "failed to allocate memory in trq_get_if_name for if_name\n");
	if (torque_cfg_buf)
	  free(torque_cfg_buf);
    return(NULL);
    }

  strcpy(if_name, ptr);

  if (torque_cfg_buf)
	free(torque_cfg_buf);

  return(if_name);
  }
