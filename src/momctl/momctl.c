/* momctl */

/* build w/

  cc momctl.c -o momctl -L ../lib/Libnet -L ../lib/Libpbs -lnet -lpbs -I ../include

 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>

extern char *optarg;

#include "net_connect.h"
#include "dis.h"
#include "mcom.h"

#include "pbs_error.h"
#include "pbs_ifl.h"
#include "resmon.h"
#include "rm.h"
#include "log.h"
#include "../lib/Liblog/chk_file_sec.h"

#define MAX_QUERY  128

char *LocalHost = "localhost";

char *DiagPtr  = "diag";

char *Query[MAX_QUERY];
int   QueryI   = 0;

char *FPtr     = NULL;
char *JPtr     = NULL;

char HostFile[256];
char ConfigBuf[65536];

int MOMPort = 0;  /* use default PBS MOM port */

mbool_t IsVerbose = FALSE;


enum MOMCmdEnum
  {
  momNONE = 0,
  momClear,
  momQuery,
  momReconfig,
  momShutdown
  };

enum MOMCmdEnum CmdIndex = momNONE;


/* prototypes */

void MCShowUsage(char *);
int do_mom(char *, int, int);

/* END prototypes */





int main(

  int    ArgC,  /* I */
  char **ArgV)  /* I */

  {
  const char *OptString = "c:Cd:f:h:p:q:r:sv";

  char  HostList[65536];

  char *HPtr;


  int c;
  int rc = PBSE_NONE;
  int local_errno = 0;

  int HostCount;
  int FailCount;

  /* initialize */

  HostList[0]     = '\0';
  ConfigBuf[0] = '\0';

  if (IamRoot() == 0)
    {
    exit(EXIT_FAILURE);
    }

  while ((c = getopt(ArgC, ArgV, OptString)) != EOF)
    {
    switch (c)
      {

      case 'c':

        /* clear stale job */

        JPtr = optarg;

        CmdIndex = momClear;

        break;

      case 'C':

        /* force cycle */

        CmdIndex = momQuery;

        Query[QueryI] = strdup("cycle");

        QueryI++;

        break;

      case 'd':

        /* diagnose */
        /* FORMAT:  momctl -d<X> */

        CmdIndex = momQuery;

        if ((Query[QueryI] = calloc(strlen(DiagPtr) + 3, sizeof(char))) == NULL)
      	  {
          fprintf(stderr,"ERROR:    could not calloc %d bytes!\n",
            (int)strlen(DiagPtr) + 3);

          exit(EXIT_FAILURE);
      	  }

        if (optarg == NULL)
          {
          strncpy(Query[QueryI],DiagPtr,strlen(DiagPtr));
          }
        else
          {
          snprintf(Query[QueryI],strlen(DiagPtr) + 2,"%s%s",
            DiagPtr,
            optarg);
          }

        QueryI++;

        break;

      case 'f':

        {
        int   rc;

        FILE *fp;

        long   size;

        if ((fp = fopen(optarg, "r")) == NULL)
          {
          fprintf(stderr, "ERROR:    cannot open file '%s', errno: %d (%s)\n",
            optarg,
            errno,
            strerror(errno));

          exit(EXIT_FAILURE);
          }

        rc = fread(HostList, sizeof(HostList), 1, fp);

        if ((rc == 0) && (!feof(fp)))
          {
          fprintf(stderr, "ERROR:    cannot read file '%s', errno: %d (%s)\n",
            optarg,
            errno,
            strerror(errno));

          exit(EXIT_FAILURE);
          }

        if ((size = ftell(fp)) < 0)
          size = 0;

        HostList[MIN(size,(long)sizeof(HostList) - 1)] = '\0';

        fclose(fp);
        }  /* END BLOCK */

      break;

      case 'h':

        /* connect to specified host */

        strncpy(HostList,optarg,sizeof(HostList));

        break;

      case 'p':

        /* port */

        if (optarg == NULL)
          MCShowUsage("port not specified");

        MOMPort = (int)strtol(optarg, NULL, 10);

        if (MOMPort == 0)
          MCShowUsage("invalid port specified");

        break;

      case 'q':

        /* query resources */

        if (optarg == NULL)
          {
          MCShowUsage("query not specified");

          Query[QueryI] = strdup(DiagPtr);
          }
        else
          {
          Query[QueryI] = strdup(optarg);
          }

        QueryI++;

        CmdIndex = momQuery;

        break;

      case 'r':

        /* reconfigure */

        {
        CmdIndex = momReconfig;

        /* NOTE:  specify remote file to load -> 'fname' */
        /*        specify local file to stage -> 'LOCAL:fname' */

        if (optarg == NULL)
          MCShowUsage("file not specified");

        if (!strncmp(optarg, "LOCAL:", strlen("LOCAL:")))
          {
          FILE *fp;

          int   size;

          int   rc;

          char *ptr;
          char *cptr;

          strcpy(ConfigBuf, "CONFIG:");

          cptr = ConfigBuf + strlen(ConfigBuf);

          ptr = optarg + strlen("LOCAL:");

          if ((fp = fopen(ptr, "r")) == NULL)
            {
            fprintf(stderr, "ERROR:    cannot open file '%s', errno: %d (%s)\n",
                    optarg,
                    errno,
                    strerror(errno));

            exit(EXIT_FAILURE);
            }

          rc = fread(cptr, sizeof(ConfigBuf) - strlen(ConfigBuf), 1, fp);

          if ((rc == 0) && (!feof(fp)))
            {
            fprintf(stderr, "ERROR:    cannot read file '%s', errno: %d (%s)\n",
                    optarg,
                    errno,
                    strerror(errno));

            exit(EXIT_FAILURE);
            }

          size = ftell(fp);

          ConfigBuf[MIN(size + strlen("CONFIG:"),sizeof(ConfigBuf) - 1)] = '\0';

          fclose(fp);
          }
        else
          {
          strncpy(ConfigBuf, optarg, sizeof(ConfigBuf));
          }
        }  /* END (case 'r') */

      break;

      case 's':

        /* shutdown */

        CmdIndex = momShutdown;

        break;

      case 'v':

        /* report verbose logging */

        IsVerbose = TRUE;

        break;
      }  /* END switch (c) */
    }    /* END while (c = getopt()) */

  if (CmdIndex == momNONE)
    {
    MCShowUsage("no command specified");
    }

  if (HostList[0] == '\0')
    strcpy(HostList, LocalHost);

  HPtr = strtok(HostList, ", \t\n");

  HostCount = 0;

  FailCount = 0;

  /* at this point, all args processing and setup is completed ...
   * ... now we run through each comma-delimited word in HPtr */

  while (HPtr != NULL)
    {
    if ((*HPtr == ':') && (*(HPtr + 1) != '\0'))
      {
      /* finds nodes with this property */

      int con;
      char *def_server, *pserver, *servername;

      struct batch_status *bstatus, *pbstat;

      struct attrl *nodeattrs;

      def_server = pbs_default();

      if ((pserver = strchr(HPtr,'@')) != NULL)
        {
        *pserver = '\0';
        servername = pserver + 1;
        }
      else
        {
        servername = def_server;
        }

      con = pbs_connect(servername);

      if (con < 0)
        {
        fprintf(stderr,"failed to connect to pbs_server:%s\n",
          servername);

        exit(EXIT_FAILURE);
        }

      /* get a batch_status entry for each node in ":property" */

      bstatus = pbs_statnode_err(con,HPtr,NULL,NULL, &local_errno);

      if (bstatus != NULL)
        {
        for (pbstat = bstatus;pbstat != NULL;pbstat = pbstat->next)
          {
          /* check state first, only do_mom() if not down */

          for (nodeattrs = pbstat->attribs;nodeattrs != NULL; nodeattrs = nodeattrs->next)
            {
            if (!strcmp(nodeattrs->name, ATTR_NODE_state))
              {
              if (!strstr(nodeattrs->value, ND_down))
                {
                rc = do_mom(pbstat->name, MOMPort, CmdIndex) >= 0 ? HostCount++ : FailCount++;
                }
              else
                {
                fprintf(stderr,"%12s:   skipping down node\n",
                  pbstat->name);
                }

              break;
              }  /* END if (attrib name eq state) */
            }    /* END for (nodeattrs) */
          }      /* END for (pbstat) */

        pbs_statfree(bstatus);
        }  /* END if (bstatus != NULL) */
      else
        {
        fprintf(stderr,"no nodes found in %s on %s\n",
          HPtr,
          servername);
        }

      pbs_disconnect(con);

      if (pserver != NULL)
        *pserver = '@';
      }
    else
      {
      rc = do_mom(HPtr, MOMPort, CmdIndex);
      if (rc >= 0)
        HostCount++;
      else
        FailCount++;
      } /* END if (*HPtr == ':') */

    HPtr = strtok(NULL, ", \t\n");
    }  /* END while (HPtr != NULL) */


  if (IsVerbose == TRUE)
    {
    fprintf(stdout, "Node Summary:  %d Successful  %d Failed\n",
            HostCount,
            FailCount);
    }

  /* test success of do_mom before returning success */
  if (rc != PBSE_NONE)
    exit(EXIT_FAILURE);
    
  /* SUCCESS */

  exit(EXIT_SUCCESS);
  }  /* END main() */




int send_command(

  struct tcp_chan *chan,
  int              cmd)

  {
  int rc;

  rc = diswsi(chan,cmd);

  if (cmd == RM_CMD_CONFIG)
    {
    diswst(chan,ConfigBuf);
    }

  DIS_tcp_wflush(chan);
  if (cmd == RM_CMD_CLOSE)
    DIS_tcp_close(chan);

  return(rc);
  } /* END send_command() */





/*
 * send_command_str
 *
 * @param stream - the stream we're writing to
 * @param cmd - the command we're writing
 */

int send_command_str(

  struct tcp_chan *chan,
  int              cmd,
  char            *query) /* I */

  {
  int rc = send_command(chan, cmd);

  if ((rc != DIS_SUCCESS) ||
      (cmd == RM_CMD_CLOSE))
    return(rc);

  rc = diswcs(chan, query, strlen(query));

  DIS_tcp_wflush(chan);

  return(rc);
  } /* END send_command_str() */





int check_success(

  struct tcp_chan *chan)

  {
  int rc;
  int local_errno;
  int status = disrsi(chan,&rc);

  if (rc != DIS_SUCCESS)
    return(rc);
  else if (status != RM_RSP_OK)
    {
#ifdef ENOMSG
    local_errno = ENOMSG;
#else
    local_errno = EINVAL;
#endif

    return(-1 * local_errno);
    }
  else
    return(PBSE_NONE);
  } /* END check_success() */




char *read_mom_reply(

  int             *local_errno, /* O */
  struct tcp_chan *chan)

  {
  int   rc;
  char *value = NULL;

  if (check_success(chan))
    return(NULL);

  value = disrst(chan,&rc);

  if (rc != DIS_SUCCESS)
    {
    if (value != NULL)
      free(value);

    *local_errno = EIO;

    return(NULL);
    }

  return(value);
  } /* END read_mom_reply() */




int start_dialogue(

  struct tcp_chan *chan)

  {
  int rc;

  rc = diswsi(chan, RM_PROTOCOL);

  if (rc != DIS_SUCCESS)
    return(rc);

  rc = diswsi(chan, RM_PROTOCOL_VER);

  if (rc != DIS_SUCCESS)
    return(rc);

  rc = diswsi(chan, QueryI);

  return(rc);
  } /* END start_dialogue() */




int do_mom(

  char *HPtr,
  int   MOMPort,
  int   CmdIndex)

  {
  int socket;
  int local_errno = 0;
  struct tcp_chan *chan = NULL;

  if ((socket = openrm(HPtr, MOMPort)) < 0)
    {
    /* FAILURE */

    extern char TRMEMsg[];

    fprintf(stderr, "cannot connect to MOM on node '%s', errno=%d (%s)\n",
      HPtr,
      errno,
      strerror(errno));

    if (TRMEMsg[0] != '\0')
      {
      fprintf(stderr, " %s\n",
              TRMEMsg);
      }

    return(socket);
    }
  else if ((chan = DIS_tcp_setup(socket)) == NULL)
    {
    fprintf(stderr, "%s: can not allocate memory of socket buffers\n", __func__);
    return -1;
    }

  /* send protocol and version, plus how many queries we're sending */
  if (QueryI == 0)
    QueryI = 1;

  if (start_dialogue(chan) != DIS_SUCCESS)
    {
    fprintf(stderr,"ERROR:    Unable to write the number of queries to %s (errno=%d-%s)\n",
        HPtr,
        errno,
        strerror(errno));
    
    send_command(chan,RM_CMD_CLOSE);
    
    return(-1);
    }

  if (IsVerbose == TRUE)
    {
    fprintf(stderr, "INFO:     successfully connected to %s\n",
            HPtr);
    }

  switch (CmdIndex)
    {

    case momClear:

      {
      char tmpLine[1024];

      char *Value;

      snprintf(tmpLine, 1024, "clearjob=%s",
               (JPtr != NULL) ? JPtr : "all");

      if (send_command_str(chan, RM_CMD_REQUEST, tmpLine) != 0)
        {
        /* FAILURE */

        fprintf(stderr,"ERROR:    cannot request job clear on %s (errno=%d-%s)\n",
          HPtr,
          errno,
          strerror(errno));

        send_command(chan,RM_CMD_CLOSE);

        return(-1);
        }

      if ((Value = (char *)read_mom_reply(&local_errno, chan)) == NULL)
        {
        /* FAILURE */

        fprintf(stderr,"ERROR:    job clear failed on %s (errno=%d - %s: %d - %s)\n",
          HPtr,
          errno,
          pbs_strerror(errno),
          local_errno,
          pbs_strerror(local_errno));

        send_command(chan,RM_CMD_CLOSE);

        return(-1);
        }

      /* job cleared */

      fprintf(stdout,"job clear request successful on %s\n",
        HPtr);

      free(Value);
      }  /* END BLOCK (case momClear) */

    break;

    case momShutdown:

      {
      if ((send_command(chan,RM_CMD_SHUTDOWN) != PBSE_NONE) ||
          (check_success(chan) != PBSE_NONE))
        {
        /* FAILURE */

        fprintf(stderr,"ERROR:    cannot shutdown mom daemon on %s (errno=%d-%s)\n",
          HPtr,
          errno,
          pbs_strerror(errno));

        send_command(chan,RM_CMD_CLOSE);

        exit(EXIT_FAILURE);
        }

      fprintf(stdout, "shutdown request successful on %s\n",
        HPtr);
      }    /* END BLOCK */

    break;

    case momReconfig:

      {
      if ((send_command(chan,RM_CMD_CONFIG) != PBSE_NONE) ||
          (check_success(chan) != PBSE_NONE))
        {
        /* FAILURE */

        fprintf(stderr,"ERROR:    cannot reconfigure mom on %s (errno=%d-%s)\n",
          HPtr,
          errno,
          pbs_strerror(errno));

        send_command(chan,RM_CMD_CLOSE);

        return(-1);
        }

      fprintf(stdout, "reconfig successful on %s\n",
        HPtr);
      }  /* END BLOCK (case momReconfig) */

    break;

    case momQuery:

    default:

      {
      char *ptr;

      int  rindex;

      char *Value;

      for (rindex = 0; rindex < QueryI; rindex++)
        {
        if (send_command_str(chan, RM_CMD_REQUEST, Query[rindex]) != 0)
          {
          fprintf(stderr,"ERROR:    cannot add query for '%s' on %s (errno=%d-%s)\n",
            Query[rindex],
            HPtr,
            errno,
            pbs_strerror(errno));
          }
        }

      for (rindex = 0;rindex < QueryI;rindex++)
        {
        if ((ptr = strchr(Query[rindex],'=')) != NULL)
          {
          *ptr = '\0';
          }

        if ((Value = (char *)read_mom_reply(&local_errno, chan)) == NULL)
          {
          fprintf(stderr, "ERROR:    query[%d] '%s' failed on %s (errno=%d - %s : %d - %s)\n",
            rindex,
            Query[rindex],
            HPtr,
            errno,
            pbs_strerror(errno),
            local_errno,
            pbs_strerror(local_errno));
          }
        else
          {
          if (!strncmp(Query[rindex], "diag", strlen("diag")))
            {
            fprintf(stdout, "%s\n",
              Value);
            }
          else if (!strncmp(Query[rindex], "cycle", strlen("cycle")))
            {
            fprintf(stdout, "mom %s successfully cycled %s\n",
              HPtr,
              Value);
            }
          else
            {
            fprintf(stdout, "%12s: %12s = '%s'\n",
              HPtr,
              Query[rindex],
              Value);
            }
          }

        free(Value);

        if (ptr != NULL)
          {
          *ptr = '=';
          }
        }  /* END for (rindex) */
      }    /* END BLOCK (case momQuery) */

    break;
    }  /* END switch(CmdIndex) */

  send_command(chan,RM_CMD_CLOSE);

  return(0);
  } /* END do_mom() */







void MCShowUsage(

  char *Msg)  /* I (optional) */

  {
  if (Msg != NULL)
    fprintf(stderr, "  %s\n",
      Msg);

  fprintf(stderr, "USAGE:  momctl <ARGS>\n");

  fprintf(stderr, "            [ -c {JOB|'all'} ]    // CLEAR STALE JOB\n");

  fprintf(stderr, "            [ -C ]                // CYCLE\n");

  fprintf(stderr, "            [ -d DIAGLEVEL ]      // DIAGNOSE (0 - 3)\n");

  fprintf(stderr, "            [ -f HOSTFILE ]       // FILE CONTAINING HOSTLIST\n");

  fprintf(stderr, "            [ -h HOST[,HOST]... ] // HOSTLIST\n");

  fprintf(stderr, "            [ -p PORT ]           // PORT\n");

  fprintf(stderr, "            [ -q ATTR ]           // QUERY ATTRIBUTE\n");

  fprintf(stderr, "            [ -r FILE ]           // RECONFIG\n");

  fprintf(stderr, "            [ -s ]                // SHUTDOWN\n");

  fprintf(stderr, "\n");

  fprintf(stderr, " Only one of c, C, d, q, r, or s must be specified, but -q may\n");

  fprintf(stderr, " be used multiple times. HOST may be a hostname or \":property\".\n");

  exit(EXIT_FAILURE);
  }  /* END MCShowUsage() */



/* END momctl.c */

