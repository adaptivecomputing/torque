#include "license_pbs.h" /* See here for the software license */
/*
 * pbspoe
 * A general purpose replacement for POE commands like poe and pdbx
 * that usually reserve nodes through the job manager but can
 * also use -procs N -hostfile foo.
 * If running interactively, invokes these commands directly.
 * If running under PBS, looks at PBS hostfile, creates an argument
 * list with -hostfile, and runs job.
 *
 * Originally written 4/13/95 by Bill Saphir, NAS/CSC
 * Based on "pbspoe" program written by Tom Proett
 * Modified by Ed Hook for LaRC's SP2 system, July 1995
 *
 * Modified 3/99 to support PBS 2.0 under IBM PSSP 3.1 software.
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <sys/wait.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

#include "pbs_ifl.h"


#define POE_PATH           "/usr/lpp/ppe.poe/bin/poe"
#define PDBX_PATH          "/usr/lpp/ppe.poe/bin/pdbx"
#define XPDBX_PATH         "/usr/lpp/ppe.poe/bin/xpdbx"
#define INTERACTIVE_FILE   "/usr/local/pbs/etc/interactive"

void   set_if_undefined(char *name, char *val);
void   check_environment();
int    is_pbs_job();
char **pbs_setup(char *command, int argc, char **argv);
void   run_command(char *command, char **argv);
char  *get_real_command(char *argv0);
int    is_interactive_ok();
void   refuse_interactive();

/* global Data */

char *mcn;  /* my command name, set in main for messages */
char  ncmdfname[PATH_MAX+1];
char *nhfile = 0;
char *ocmdfile = 0;
char *pbsjobkey;

int
is_pbs_job(void)
  {
  char *val;
  val = getenv("PBS_JOBID");

  if (val == NULL) return(0);

  if ((pbsjobkey = getenv("PBS_JOB_KEY")) == NULL)
    {
    fprintf(stderr, "%s: Unable to obtain Job Key for switch setup\n", mcn);
    exit(1);
    }

  return(1);
  }

void set_if_undefined(char *name, char *val)
  {
  char *ret;
  int result;
  ret = getenv(name);

  if (ret != NULL) return;

  result = putenv(val);

  if (result != 0)
    {
    fprintf(stderr,
            "%s: Fatal Error: unable to set environment variable %s", mcn, name);
    exit(1);
    }

  fprintf(stderr, "%s: Warning: added variable %s to environment\n", mcn, name);
  }

void
check_environment(void)
  {
  set_if_undefined("MP_PULSE", "MP_PULSE=0");
  set_if_undefined("MP_EUILIB", "MP_EUILIB=us");
  set_if_undefined("MP_RESD", "MP_RESD=no");
  }


#define LINEBUFSIZ 1024

int fix_cmdfile(char *ocmd, char *ncmd)
  {
  FILE *new;
  FILE *old;
  int   len;
  char  linebuf[LINEBUFSIZ];
  char *pl;

  if ((old = fopen(ocmd, "r")) == NULL)
    {
    fprintf(stderr, "%s: Unable to open command file: %s\n", mcn, ocmd);
    return (-1);
    }

  sprintf(ncmd, "/tmp/pbsCtemp.%s", getenv("PBS_JOBID"));

  if ((new = fopen(ncmd, "w")) == NULL)
    {
    fprintf(stderr, "%s: Unable to open %s for new command file\n", mcn, ncmd);
    return (-1);
    }

  strcpy(linebuf, PBSPD); /* prefix "command" with pbspd */

  strcat(linebuf, " ");
  strcat(linebuf, pbsjobkey); /* and job key   */
  strcat(linebuf, " ");
  pl = linebuf + strlen(linebuf);
  len = (int)(LINEBUFSIZ - ((long)pl - (long)linebuf));

  while (fgets(pl, len, old) != NULL)
    {
    fputs(linebuf, new);
    }

  fclose(new);

  fclose(old);
  return (0);
  }

/*
 * clean_up - dont leave files we created lying arround
 */

void clean_up(int exval)
  {
  if (nhfile)
    (void)unlink(nhfile);

  if (ocmdfile)
    (void) unlink(ncmdfname);

  exit(exval);
  }





char **pbs_setup(

  char *command, 
  int   argc, 
  char *argv[])

  {
  char *jobid;
  int   have_command = 0;
  static char hostfile[PATH_MAX+1];
  static char nhoststring[10];
  static char nodename[PATH_MAX+1];
  FILE  *nf;
  char **newargs;
  char  *nodefile;
  int    nhosts = 0, nprocs, i, j;
  int    nprocs_specified;
  char **hfile_mem;
  int    hfile_size = 16;
  int    index;
  char  *pcolon = NULL;
  char  *pcomma;
  char  *offset;

  /* print out this message to remind folks they're running under PBS.
   * Especially important for interactive users
   */
  fprintf(stderr, "%s: Running job under PBS\n", mcn);
  jobid = getenv("PBS_JOBID");

  if (jobid == NULL)
    {
    /* should be an impossible error since we've already checked for this */
    fprintf(stderr, "%s: Fatal Error: can't get JOBID from environment\n", mcn);
    exit(1);
    }

  /* fetch list of nodes allocated to this job by pbs_mom */

  nodefile = getenv("PBS_NODEFILE");

  if (nodefile == NULL)
    {
    fprintf(stderr, "%s: PBS_NODEFILE not in the environment\n", mcn);
    exit(EXIT_FAILURE);
    }

  strcpy(hostfile, nodefile);

  nf = fopen(nodefile, "r");

  if (nf == NULL)
    {
    fprintf(stderr, "%s: Can't open PBS hostfile %s\n", mcn, hostfile);
    exit(EXIT_FAILURE);
    }

  hfile_mem = (char **)calloc(hfile_size, sizeof(char *));

  if (hfile_mem == NULL)
    {
    fprintf(stderr, "%s: Can't calloc memory",
      mcn);

    exit(EXIT_FAILURE);
    }

  while (fgets(nodename, PATH_MAX, nf) != NULL)
    {
    hfile_mem[nhosts++] = strdup(nodename);

    if (nhosts == hfile_size)
      {
      char **tmpHFileMem;

      hfile_size *= 2;

      tmpHFileMem = (char **)realloc(hfile_mem,hfile_size * sizeof(char *));

      if (tmpHFileMem == NULL)
        {
        fprintf(stderr, "%s: Can't calloc memory", 
          mcn);

        exit(EXIT_FAILURE);
        }

      hfile_mem = tmpHFileMem;
      }
    }    /* END while (fgets() != NULL) */

  fclose(nf);

  if (nhosts == 0)
    {
    fprintf(stderr, "%s: Fatal Error: malformed resource request(s)\n",
      mcn);

    exit(EXIT_FAILURE);
    }

  sprintf(nhoststring, "%d",
    nhosts);

  /* do we have an command file specified in the environment */

  ocmdfile = getenv("MP_CMDFILE");

  if (ocmdfile && (*ocmdfile == '\0'))
    ocmdfile = 0;  /* if null file name, null the pointer */

  /* make an argument list */

  newargs = calloc((argc + 8), sizeof(char *));

  if (newargs == NULL)
    {
    perror("calloc");

    fprintf(stderr, "%s: Unable to allocate space for argument list\n",
      mcn);

    exit(1);
    }

  /* Must pre-scan arguments to see if cmdfile specified */

  for (i = 1; i < argc; i++)
    {
    if (strcmp(argv[i], "-cmdfile") == 0)
      {
      if (i + 1 == argc)
        {
        fprintf(stderr, "%s: -cmdfile must have argument\n", mcn);
        exit(1);
        }

      ocmdfile = argv[i+1];
      }
    }

  /* if first argument is not an option, or if first is -h and second arg */
  /* is not an option, then user specified a program to run,  this takes  */
  /* preceedent over any command file specified.     */

  if ((argc > 1) &&
      ((*argv[1] != '-')  ||
       ((strcmp(argv[1], "-h") == 0) && (argc > 2) && (*argv[2] != '-'))))
    {

    /* have a command specified for poe to run */

    have_command = 1;

    if (ocmdfile)
      {
      fprintf(stderr, "%s: Progam specified on command line overrides -cmdfile option\n",
        mcn);

      ocmdfile = 0;
      }
    }

  /* if neither command nor cmdfile, then cannot run */

  if ((have_command == 0) && (ocmdfile == 0))
    {
    fprintf(stderr, "%s: requires either command argument or -cmdfile option\n",
            mcn);
    exit(1);
    }

  newargs[0] = strrchr(command, '/');

  if (newargs[0] == NULL)
    {
    fprintf(stderr, "%s:Impossible command error! Contact system administrator\n",
      mcn);

    exit(1);
    }
  else
    {
    /* advance past / character */

    (newargs[0])++;
    }

  j = 1;

  if (ocmdfile == 0)
    {
    newargs[j++] = PBSPD;
    newargs[j++] = pbsjobkey;
    }

  /* copy user arguments to new array, taking note of certain ones */

  nprocs_specified = 0;

  nprocs = nhosts;

  for (i = 1; i < argc; i++)
    {

    if (strcmp(argv[i], "-procs") == 0)
      {
      nprocs_specified = 1;
      newargs[j++] = argv[i++]; /* -procs       */

      if (i < argc)
        {
        newargs[j++] = argv[i]; /* num_of_procs */
        nprocs = strtol(argv[i], &pcolon, 10);

        if (nprocs <= 0)
          {
          fprintf(stderr, "%s: Value specified by -procs must be greater than zero\n",
            mcn);

          exit(1);
          }

        /* pass along user's request */
        sprintf(nhoststring, "%d", nprocs);
        }
      else
        {
        fprintf(stderr, "%s: Error, -procs must specify a value\n", mcn);
        exit(1);
        }

      }
    else if ((strcmp(argv[i], "-hostfile") == 0) ||
             (strcmp(argv[i], "-hfile") == 0))
      {
      /* skip it, will add our own */

      i++;
      }
    else if (strcmp(argv[i], "-cmdfile") == 0)
      {
      /* skip over it this time, will add our own at end */

      ++i;
      }
    else
      {
      newargs[j++] = argv[i];
      }
    }

  if (ocmdfile)
    {
    /* have cmdfile specified,  copy and modify it */

    if (fix_cmdfile(ocmdfile, ncmdfname) == -1)
      {
      fprintf(stderr, "%s: unable to update command file\n", mcn);

      exit(1);
      }

    newargs[j++] = "-cmdfile";

    newargs[j++] = ncmdfname;

    }

  if (nhosts < nprocs)
    {
    fprintf(stderr, "%s: Warning, (number of host nodes) < (number of procs)\n\t-- assigning multiple processes per node\n\t-- not using high-performance switch mode\n", mcn);

    putenv("MP_EUILIB=ip");

    sprintf(hostfile, "/tmp/pbsHtemp.%s", getenv("PBS_JOBID"));

    nf = fopen(hostfile, "w");

    if (nf == NULL)
      {
      fprintf(stderr, "%s: Can't open PBS hostfile %s\n", mcn, hostfile);
      exit(1);
      }

    /* build new hostfile */

    for (i = index = 0; i < nprocs; i++)
      {
      fprintf(nf, "%s\n", hfile_mem[index++]);

      if (index == nhosts)
        index = 0; /* wrap around */
      }

    fclose(nf);
    }
  else if (nhosts >= nprocs && pcolon && *pcolon)
    {
    /* wants a specific subset */

    sprintf(hostfile, "/tmp/pbsTemp.%s", getenv("PBS_JOBID"));
    nf = fopen(hostfile, "w");

    if (nf == NULL)
      {
      fprintf(stderr, "%s: Can't open PBS hostfile %s\n", mcn, hostfile);
      clean_up(1);
      }

    nhfile = hostfile;

    i = 0;  /* counts current length of host list */
    offset = pcolon + 1; /* point to first index */

    while (1)
      {
      index = atoi(offset); /* index of current candidate */
      /* can't be greater than the total number of nodes */

      if (index >= nhosts)
        {
        fprintf(stderr, "%s: \"%s\": Index [%d] out-of-range\n", mcn, pcolon, index);
        clean_up(1);
        }

      /* can't specify more nodes than asked for by "<k>:" */
      if (++i > nprocs)
        {
        fprintf(stderr, "%s: \"%s\" specifies more than %d nodes\n", mcn, pcolon, nprocs);
        clean_up(1);
        }

      /* can't use any node more than once */
      if (hfile_mem[index][0] == '@')
        {
        fprintf(stderr, "%s: \"%s\": duplicate index [%d]\n", mcn, pcolon, index);
        clean_up(1);
        }

      fprintf(nf, "%s\n", hfile_mem[index]); /* add to our hostfile */

      hfile_mem[index][0] = '@'; /* remember using it */

      pcomma = strchr(offset, ',');

      if (pcomma)     /* still more in the list */
        offset = pcomma + 1;
      else
        break;
      }

    /* did user specify as many nodes as processes ?? */
    if (i < nprocs)
      {
      fprintf(stderr, "%s: \"%d%s\" has too few nodes listed\n", mcn, nprocs, pcolon);
      clean_up(1);
      }

    fclose(nf);
    }

  for (i = 0 ; i < nhosts ; ++i)
    free(hfile_mem[i]);

  free(hfile_mem);

  newargs[j++]   = "-hostfile";

  newargs[j++]   = hostfile;

  if (! nprocs_specified)
    {
    newargs[j++] = "-procs";
    newargs[j++] = nhoststring;
    };

  newargs[j] = NULL;

  return(newargs);
  }





void run_command(

  char *command, 
  char *argv[])

  {
  int junk = 0;
  int   i;
  char *psyscmd;
  int   syssz;

#ifdef DEBUG
  char **p;
  printf("pbspoe exec-ing: ");

  for (p = argv; *p; ++p)
    printf("%s ", *p);

  printf("\n");

#endif /* DEBUG */

  fflush(stdout);

  fflush(stderr);

  if (ocmdfile || nhfile)
    {

    /* must use system() call to start real command so can remove files */

    syssz = strlen(command) + 1;

    i = 1;

    while (argv[i])
      syssz += strlen(argv[i++]) + 1;

    if ((psyscmd = (char *)calloc(1, syssz + 1)) == NULL)
      {
      fprintf(stderr, "%s: unable to calloc memory\n", mcn);
      clean_up(1);
      }

    strcpy(psyscmd, command);

    i = 1;

    while (argv[i])
      {
      strcat(psyscmd, " ");
      strcat(psyscmd, argv[i++]);
      }

    junk = system(psyscmd);

    clean_up(0);
    }
  else
    {
    /* run real command directly */

    execv(command, argv);

    /* not reached unless error */

    fprintf(stderr, "%s: Error, exec of %s failed\n",
      mcn,
      command);

    perror("execv");

    exit(1);
    }

  return;
  }    /* END run_command() */





char *get_real_command(

  char *argv0)

  {
  char *command_name;
  command_name = strrchr(argv0, '/');

  if (command_name != NULL)
    command_name++; /* advance past "/" */
  else
    command_name = argv0;

  if (!strcmp(command_name, "poe")) return(POE_PATH);

  if (!strcmp(command_name, "pbspoe")) return(POE_PATH);

  if (!strcmp(command_name, "pdbx")) return(PDBX_PATH);

  if (!strcmp(command_name, "xpdbx")) return XPDBX_PATH;

  if (!strcmp(command_name, "newpoe")) return(POE_PATH);

  fprintf(stderr, "\"%s\" unknown to the PBS front end.\n", argv0);

  exit(1);
  }





int is_interactive_ok(void)

  {

  struct stat sb;
  int status;
  status = stat(INTERACTIVE_FILE, &sb);

  if (status == -1)
    {
    return(0);
    }

  return(1);
  }




void refuse_interactive(void)

  {
  char *nasty_gram =
    {
    "\n"
    "                           WARNING !\n"
    "      To run a job on the SP2, you must use PBS.   If you\n"
    "      wish to run interactively, \"qsub -I\" is available.\n"
    "      For further information, type \"man qsub\".\n"
    "\n"
    "\n"
    };

  fputs(nasty_gram, stderr);

  exit(1);
  }





int main(

  int   argc, 
  char *argv[])

  {
  char **args, *command;

  mcn = argv[0];

  /* make sure certain environment variables are set */
  check_environment();

  /* determine if we're running poe or pdbx */
  command = get_real_command(argv[0]);

  /* add extra arguments if pbs job */

  if (is_pbs_job())
    {
    args = pbs_setup(command, argc, argv);

    run_command(command, args);
    }
  else if (is_interactive_ok())
    {
    run_command(command, argv);
    }
  else
    {
    refuse_interactive();
    }

  return (0);
  }
