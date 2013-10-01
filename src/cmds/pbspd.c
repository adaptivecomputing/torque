#include "license_pbs.h" /* See here for the software license */
#include "utils.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> /* execvp */

/*
 * Used by PBS on IBM SP systems to set environment for tasks spawned by poe as:
 *
 * pbspd job_key window_id_file cmd args
 *
 * This results in the execution of "cmd" with the arguments "args"
 * The environ variable MP_PARTITION is set to "job_key"
 * The environ variable MP_CHILD is used to index into the text file
 * window_id_file to obtain the window id which becomes the value of the
 * environ variable MP_MPI_NETWORK.
 */

#define SWLINELEN 5

char
get_winid(char *winidfile, char *mpchild)
  {
  FILE *wf;
  int   tid;
  char  buf[SWLINELEN+1];
  char  swid = '\0';


  if ((wf = fopen(winidfile, "r")) == NULL)
    return '\0';

  tid = atoi(mpchild) + 1;

  while (tid--)
    {
    if (fgets(buf, SWLINELEN, wf) == NULL)
      {
      fclose(wf);
      return '\0';
      }
    }

  fclose(wf);

  swid = buf[0];
  return (swid);
  }


int main(int argc, char **argv)
  {
  char     *mpchild;
  char     winid[2];

  static const char *job_key = "MP_PARTITION";
  static const char *job_win = "MP_MPI_NETWORK";

  /* Check that calling args are correct */

  if (argc < 4)
    {
    fprintf(stderr, "%s: insufficent arguments\n", argv[0]);
    return 1;
    }

  if ((mpchild = getenv("MP_CHILD")) == NULL)
    {
    fprintf(stderr, "%s: MP_CHILD not in environment\n", argv[0]);
    return 1;
    }

  /*
   * Build up job key and window variables and put into environment.
   * The job key is the first argument.
   * The switch window_id_file is the second argument.
   */

  if (put_env_var(job_key, argv[1]))
    {
    fprintf(stderr, "%s: unable to set environment variable %s\n", argv[0], job_key);
    return 1;
    }

  winid[0] = get_winid(argv[2], mpchild);

  /* null terminate winid */
  winid[1] = '\0';

  if (winid[0] == '\0')
    {
    fprintf(stderr, "%s: unable to obtain switch window id\n",
            argv[0]);
    return 1;
    }

  if (put_env_var(job_win, winid))
    {
    fprintf(stderr, "%s: unable to set environment variable %s\n", argv[0], job_win);
    return 1;
    }

  /* now exec the real program with its args */

  if (execvp(argv[3], &argv[3]) < 0)
    {
    fprintf(stderr, "%s: unable to exec %s\n", argv[0], argv[3]);
    perror("error:");
    return 1;
    }

  return 0;
  }
