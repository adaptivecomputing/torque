#include "license_pbs.h" /* See here for the software license */
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
  int      len;
  char      *jk;
  char     *mpchild;
  char     winid;

  static char *job_key = "MP_PARTITION=";
  static char job_win[15+SWLINELEN] = "MP_MPI_NETWORK=";

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

  len = strlen(argv[1]) + strlen(job_key) + 1;

  if ((jk  = (char *)calloc(1, len)) == 0)
    {
    fprintf(stderr, "%s: cannot allocate memory\n", argv[0]);
    return 1;
    }

  (void)strcpy(jk, job_key);
  (void)strcat(jk, argv[1]);
  putenv(jk);


  winid = get_winid(argv[2], mpchild);

  if (winid == '\0')
    {
    fprintf(stderr, "%s: unable to obtain switch window id\n",
            argv[0]);
    return 1;
    }

  len = strlen(job_win);

  job_win[len] = winid;
  job_win[len+1] = '\0';
  putenv(job_win);

  /* now exec the real program with its args */

  if (execvp(argv[3], &argv[3]) < 0)
    {
    fprintf(stderr, "%s: unable to exec %s\n", argv[0], argv[3]);
    perror("error:");
    return 1;
    }

  return 0;
  }
