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

#include "mcom.h"

#include "pbs_error.h"
#include "pbs_ifl.h"
#include "resmon.h"
#include "rm.h"

char *LocalHost = "localhost";

char *DiagPtr  = "diag";

char QueryString[1024];

char *FPtr     = NULL;
char *JPtr     = NULL;

char HostFile[256];

int MOMPort = 0;  /* use default PBS MOM port */

mbool_t IsVerbose = FALSE;

#define MAX_QUERY  128

enum MOMCmdEnum {
  momNONE = 0,
  momClear,
  momQuery,
  momReconfig,
  momShutdown };

enum MOMCmdEnum CmdIndex = momNONE;


/* prototypes */

int MCShowUsage(char *);

/* END prototypes */





int main(

  int    ArgC,  /* I */
  char **ArgV)  /* I */

  {
  const char *OptString = "c:Cd:f:h:p:q:r:sv";

  char  HostList[65536];
  char  tmpConfigBuf[65536];

  char *HPtr;

  char *HTok;
  char *QTok;
 
  int sd;

  int c;

  int HostCount;
  int FailCount;

  /* initialize */

  HostList[0]     = '\0';
  tmpConfigBuf[0] = '\0';

  QueryString[0]  = '\0';

  if (getuid() != 0)
    {
    fprintf(stderr,"ERROR:  must be root to run this command\n");

    exit(1);
    }

  while ((c = getopt(ArgC,ArgV,OptString)) != EOF)
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

        strcpy(QueryString,"cycle");

        break;

      case 'd':

        CmdIndex = momQuery;

        if (optarg == NULL)        
          {
          strcpy(QueryString,DiagPtr);
          }
        else 
          {
          snprintf(QueryString,sizeof(QueryString),"%s%s",
            DiagPtr,
            optarg);
          }

        break;

      case 'f':

        {
        int   rc;

        FILE *fp;

        int   size;

        if ((fp = fopen(optarg,"r")) == NULL)
          {
          fprintf(stderr,"ERROR:    cannot open file '%s', errno: %d (%s)\n",
            optarg,
            errno,
            strerror(errno));

          exit(1);
          }

        rc = fread(HostList,sizeof(HostList),1,fp);

        if ((rc == 0) && (!feof(fp)))
          {
          fprintf(stderr,"ERROR:    cannot read file '%s', errno: %d (%s)\n",
            optarg,
            errno,
            strerror(errno));

          exit(1);
          }

        size = ftell(fp);

        HostList[MIN(size,sizeof(HostList) - 1)] = '\0';

        fclose(fp);
        }  /* END BLOCK */

        break;
      
      case 'h':

        strncpy(HostList,optarg,sizeof(HostList));

        break; 

      case 'p':

        /* port */

        if (optarg == NULL)
          MCShowUsage("port not specified");

        MOMPort = (int)strtol(optarg,NULL,10);

        if (MOMPort == 0)
          MCShowUsage("invalid port specified");

        break;

      case 'q':

        /* query resources */

        if (optarg == NULL)
          {
          MCShowUsage("query not specified");

          strcpy(QueryString,DiagPtr);
          }
        else
          {
          strncpy(QueryString,optarg,sizeof(QueryString));
          }
 
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

        if (!strncmp(optarg,"LOCAL:",strlen("LOCAL:")))
          {
          FILE *fp;

          int   size;

          int   rc;

          char *ptr;
          char *cptr;

          strcpy(tmpConfigBuf,"CONFIG:");

          cptr = tmpConfigBuf + strlen(tmpConfigBuf);

          ptr = optarg + strlen("LOCAL:");

          if ((fp = fopen(ptr,"r")) == NULL)
            {
            fprintf(stderr,"ERROR:    cannot open file '%s', errno: %d (%s)\n",
              optarg,
              errno,
              strerror(errno));

            exit(1);
            }

          rc = fread(cptr,sizeof(tmpConfigBuf) - strlen(tmpConfigBuf),1,fp);

          if ((rc == 0) && (!feof(fp)))
            {
            fprintf(stderr,"ERROR:    cannot read file '%s', errno: %d (%s)\n",
              optarg,
              errno,
              strerror(errno));

            exit(1);
            }

          size = ftell(fp);

          tmpConfigBuf[MIN(size + strlen("CONFIG:"),sizeof(tmpConfigBuf) - 1)] = '\0';

          fclose(fp);
          }
        else
          {
          strncpy(tmpConfigBuf,optarg,sizeof(tmpConfigBuf));
          }
        }  /* END (case 'r') */

        break;

      case 's':

        /* shutdown */

        CmdIndex = momShutdown;

        break;
 
      case 'v':

        IsVerbose = TRUE;

        break;
      }  /* END switch (c) */
    }    /* END while (c = getopt()) */

  if (CmdIndex == momNONE)
    {
    MCShowUsage("no command specified");
    }

  if (HostList[0] == '\0')
    strcpy(HostList,LocalHost);

  HPtr = strtok_r(HostList,", \t\n",&HTok);

  HostCount = 0;
  FailCount = 0;

  while (HPtr != NULL)
    {
    if ((sd = openrm(HPtr,MOMPort)) < 0)
      {
      fprintf(stderr,"cannot connect to MOM on node '%s', rc=%d\n",
        HPtr,
        sd);

      HPtr = strtok_r(NULL,", \t\n",&HTok);

      FailCount++;

      continue;
      }

    if (IsVerbose == TRUE)
      {
      fprintf(stderr,"INFO:     successfully connected to %s\n",
        HPtr);
      }

    switch(CmdIndex)
      {
      case momClear:

        {
        char tmpLine[1024];

        char *Value;

        snprintf(tmpLine,1024,"clearjob=%s",
          (JPtr != NULL) ? JPtr : "all");
  
        if (addreq(sd,tmpLine) != 0)
          {
          fprintf(stderr,"ERROR:    cannot request job clear on %s (errno=%d:%d)\n",
            HPtr,
            errno,
            pbs_errno);

          FailCount++;

          closerm(sd);

          HPtr = strtok_r(NULL,", \t\n",&HTok);

          continue;
          }

        if ((Value = (char *)getreq(sd)) == NULL)
          {
          fprintf(stderr,"ERROR:    job clear failed on %s (errno: %d:%d)\n",
            HPtr,
            errno,
            pbs_errno);

          FailCount++;

          closerm(sd);

          HPtr = strtok_r(NULL,", \t\n",&HTok);

          continue;
          }

        /* job cleared */

        fprintf(stdout,"job clear request successful on %s\n",
          HPtr);
        }  /* END BLOCK (case momClear) */

        break;

      case momShutdown:

        {
        int rc;

        rc = downrm(sd);

        if (rc != 0)
          {
          fprintf(stderr,"ERROR:    cannot shutdown mom daemon on %s (errno=%d:%d)\n",
            HPtr,
            errno,
            pbs_errno);

          FailCount++;

          closerm(sd);

          HPtr = strtok_r(NULL,", \t\n",&HTok);
  
          continue;
          }

        fprintf(stdout,"shutdown request successful on %s\n",
          HPtr);
        }    /* END BLOCK */

        break;
  
      case momReconfig:

        {
        int rc;

        rc = configrm(sd,tmpConfigBuf);

        if (rc != 0)
          {
          fprintf(stderr,"ERROR:    cannot reconfigure mom on %s (errno=%d:%d)\n",
            HPtr,
            errno,
            pbs_errno);

          FailCount++;

          closerm(sd);

          HPtr = strtok_r(NULL,", \t\n",&HTok);

          continue;
          }

        fprintf(stdout,"reconfig successful on %s\n",
          HPtr);
        }  /* END BLOCK (case momReconfig) */

        break;
 
      case momQuery:
      default:

        {
        char *Query[MAX_QUERY];

        char *ptr;

        int  RCount;
        int  rindex;

        char *Value;

        rindex = 0;

        ptr = strtok_r(QueryString,", \t\n",&QTok);

        while (ptr != NULL)
          {
          if (addreq(sd,ptr) != 0)
            {
            fprintf(stderr,"ERROR:    cannot add query for '%s' on %s (errno=%d:%d)\n",
              ptr,
              HPtr,
              errno,
              pbs_errno);
            }

          Query[rindex] = ptr;

          rindex++;

          ptr = strtok_r(NULL,", \t\n",&QTok);
          }

        RCount = rindex;

        Query[rindex] = NULL;

        for (rindex = 0;rindex < RCount;rindex++)
          {
          if ((Value = (char *)getreq(sd)) == NULL)
            {
            fprintf(stderr,"ERROR:    query[%d] '%s' failed on %s (errno: %d:%d)\n",
              rindex,
              Query[rindex],
              HPtr,
              errno,
              pbs_errno);
            }
          else
            {
            if (!strncmp(Query[rindex],"diag",strlen("diag")))
              {
              fprintf(stdout,"%s\n",
                Value);
              }
            else if (!strncmp(Query[rindex],"cycle",strlen("cycle")))
              {
              fprintf(stdout,"mom %s successfully cycled %s\n",
                HPtr,
                Value);
              }
            else
              {
              fprintf(stdout,"%12s: %12s = '%s'\n",
                HPtr,
                Query[rindex],  
                Value);
              }
            }
          }  /* END for (rindex) */
        }    /* END BLOCK (case momQuery) */
  
        break;
      }  /* END switch(CmdIndex) */

    closerm(sd);

    HPtr = strtok_r(NULL,", \t\n",&HTok);

    HostCount++;
    }  /* END while (HPtr != NULL) */

  if (IsVerbose == TRUE)
    {
    fprintf(stdout,"Node Summary:  %d Successful  %d Failed\n",
      HostCount,
      FailCount);
    }

  return(SUCCESS);
  }  /* END main() */





int MCShowUsage(

  char *Msg)  /* I (optional) */

  {
  fprintf(stderr,"USAGE:  momctl <ARGS>\n");

  fprintf(stderr,"            [ -c {JOB|'all'} ]    // CLEAR STALE JOB\n");
  fprintf(stderr,"            [ -C ]                // CYCLE\n");
  fprintf(stderr,"            [ -d DIAGLEVEL ]      // DIAGNOSE (0 - 3)\n");
  fprintf(stderr,"            [ -f HOSTFILE ]       // FILE CONTAINING HOSTLIST\n");
  fprintf(stderr,"            [ -h HOST[,HOST]... ] // HOSTLIST\n");
  fprintf(stderr,"            [ -p PORT ]           // PORT\n");
  fprintf(stderr,"            [ -q ATTR ]           // QUERY ATTRIBUTE\n");
  fprintf(stderr,"            [ -r FILE ]           // RECONFIG\n");
  fprintf(stderr,"            [ -s ]                // SHUTDOWN\n");
  fprintf(stderr,"\n");

  fprintf(stderr," one of c, C, d, q, r, or s must be specified\n");

  if (Msg != NULL)
    fprintf(stderr,"  %s\n",
      Msg);

  exit(1);
  }  /* END MCShowUsage() */

