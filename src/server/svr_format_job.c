/*
 * svr_format_job.c - simple printf like command to output information about a
 * pbs job 
 */

#include <pbs_config.h>   /* the master config generated by configure */

#include "pbs_ifl.h"
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list_link.h"
#include "attribute.h"
#include "server_limits.h"
#include "pbs_job.h"
#include "log.h"
#include "server.h"
#include "array.h"
#include "mail_throttler.hpp"

/* Global Data */

extern char *msg_job_abort;
extern char *msg_job_start;
extern char *msg_job_end;
extern char *msg_job_del;
extern char *msg_job_stageinfail;
extern char *msg_job_copychkptfail;
extern char *msg_job_otherfail;



void svr_format_job(
     
  FILE        *fh,  /* output file handle */
  mail_info   *mi,  /* I */
  const char *fmt) /* printf-like format description */

  {
  int   mailpoint = mi->mail_point;
  const char *text = mi->text.c_str();
  const char *p;
  char *stdmessage = NULL;
  char *reason = NULL;
  
  /* First get the "standard" message */
  
  switch (mailpoint)
    {
    
    case MAIL_ABORT:
      
      stdmessage = msg_job_abort;
      reason = msg_job_abort;
      
      break;
      
    case MAIL_BEGIN:
      
      stdmessage = msg_job_start;
      reason = msg_job_start;
      
      break;
      
    case MAIL_END:
      
      stdmessage = msg_job_end;
      reason = msg_job_end;
      
      break;
      
    case MAIL_DEL:
      
      stdmessage = msg_job_del;
      reason = msg_job_del;
      
      break;
      
    case MAIL_STAGEIN:
      
      stdmessage = msg_job_stageinfail;
      reason = pbse_to_txt(PBSE_STAGEIN); /* NB: short version used */
      
      break;
      
    case MAIL_CHKPTCOPY:
      
      stdmessage = msg_job_copychkptfail;
      reason = msg_job_copychkptfail;
      
      break;
      
    case MAIL_OTHER:
      
    default:
      
      stdmessage = msg_job_otherfail;
      reason = msg_job_otherfail;
      
      break;
    }  /* END switch (mailpoint) */
    
  p = fmt;
  while (*p)
    {
    if (*p == '\\')      /* escape sequences */
      {
      switch(p[1])
        {
        case 'n':  /* newline */

          fputc('\n', fh);
          p += 2;

          break;

        case 't':  /* tab */

          fputc('\t', fh);

          p += 2;

          break;

        case '\\': /* backslash */
        case '\'': /* quote */
        case '\"': /* double-quote */


          fputc(p[1], fh);
          p += 2;
  
          break;

        default:  /* we don't recognise this escape, ignore it (catches p[1]==0) */

          fputc(*p++, fh);

          break;
        } /* END switch(p[1]) */
      }
    else if (*p == '%') /* format statement */
      {
      switch(p[1])
        {
        case 'd':  /* details */

          if (text != NULL)
            {
            fprintf(fh, "%s", text);
            }
          p += 2;

          break;

        case 'h':  /* host */

          if (mi->exec_host.size() != 0)
            {
            fprintf(fh, "%s", mi->exec_host.c_str());
            }
          p += 2;

          break;

        case 'i':  /* jobId */

          fprintf(fh, "%s", mi->jobid.c_str());
          p += 2;

          break;

        case 'j':  /* jobname */

          if (mi->jobname.size() != 0)
            fprintf(fh, "%s", mi->jobname.c_str());

          p += 2;

          break;

        case 'k':  /* error File */

          if (mi->errFile.size() != 0)
            fprintf(fh, "%s", mi->errFile.c_str());

          p += 2;

          break;


        case 'l':  /* output File */

          if (mi->outFile.size() != 0)
            fprintf(fh, "%s", mi->outFile.c_str());

          p += 2;

          break;

        case 'm':  /* stdmessage */

          if (stdmessage != NULL)
            {
            fprintf(fh, "%s", stdmessage);
            }
          p += 2;

          break;

        case 'r':  /* reason */

          if (reason != NULL)
            {
            fprintf(fh, "%s", reason);
            }
          p += 2;

          break;

        default:  /* we don't recognise this format, ignore it (catches p[1]==0) */

          fputc(*p++, fh);

          break;
        } /* END switch(p[1]) */
      }
    else               /* ordinary character */
      {
      fputc(*p++, fh);
      }
    } /* END while (*p) */
  } /* END format_job() */

