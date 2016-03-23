/* pam_pbssimpleauth module */

#define NEED_BLOCKING_CONNECTIONS
#include "pbs_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <dirent.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "portability.h"
#include "list_link.h"
#include "pbs_ifl.h"
#include "lib_ifl.h"
#include "attribute.h"
#include "server_limits.h"
#include "pbs_job.h"
#include "utils.h"

#define MODNAME "pam_pbssimpleauth"

/*
 * here, we make a definition for the externally accessible function
 * in this file (this definition is required for static a module
 * but strongly encouraged generally) it is used to instruct the
 * modules include file to define the function prototypes.
 */

#define PAM_SM_AUTH
#define PAM_SM_ACCOUNT

#ifdef HAVE_SECURITY_PAM_APPL_H
#include <security/pam_appl.h>
#endif

#ifdef HAVE_SECURITY_PAM_MODULES_H
#include <security/pam_modules.h>
#else
#ifdef HAVE_PAM_PAM_MODULES_H
#include <pam/pam_modules.h>
#endif
#endif

/* this isn't defined on solaris */
#ifndef PAM_EXTERN
#define PAM_EXTERN
#endif

int job_read_xml(const char *filename, job *pjob, char *log_buf, size_t buf_len);

/* --- authentication management functions (only) --- */

PAM_EXTERN
int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc,
                        const char **argv)
  {
  int retval = PAM_SERVICE_ERR;
  pam_get_user_2nd_arg_t *username;

  struct passwd *user_pwd;
  char *ubuf = NULL;

  struct dirent *jdent;
  DIR *jobdir = NULL;
  int fp;

  struct job xjob;
  ssize_t amt;
  char jobpath[PATH_MAX+1];
  char jobdirpath[PATH_MAX+1];
  int debug = 0;

  char log_buf[LOCAL_LOG_BUF_SIZE];

  openlog(MODNAME, LOG_PID, LOG_USER);
  strcpy(jobdirpath, PBS_SERVER_HOME "/mom_priv/jobs");

  /* step through arguments */

  for (; argc-- > 0; ++argv)
    {
    if (!strcmp(*argv, "debug"))
      debug = 1;
    else if (!strncmp(*argv, "jobdir=", strlen("jobdir=")))
      strncpy(jobdirpath, (*argv) + strlen("jobdir="), PATH_MAX);
    else
      syslog(LOG_ERR, "unknown option: %s", *argv);
    }

  if (debug) syslog(LOG_INFO, "opening %s", jobdirpath);

  if ((jobdir = opendir(jobdirpath)) == NULL)
    {
    if (debug) syslog(LOG_INFO, "failed to open jobs dir: %s", strerror(errno));

    closelog();

    return PAM_IGNORE;
    }

  /* get the username and passwd, allow uid 0 */
  retval = pam_get_user(pamh, (const char **)&username, NULL);

#if defined(PAM_CONV_AGAIN) && defined(PAM_INCOMPLETE)
  if (retval == PAM_CONV_AGAIN)
    {
    closelog();
    return PAM_INCOMPLETE;
    }

#endif

  if ((retval != PAM_SUCCESS) || !username)
    {
    syslog(LOG_ERR, "failed to retrieve username");
    closelog();
    return PAM_SERVICE_ERR;
    }

  user_pwd = getpwnam((char *)username);

  /* no early returns from this point on because we need to free ubuf */

  if (debug) syslog(LOG_INFO, "username %s, %s", username, user_pwd ? "known" : "unknown");

  if (!user_pwd)
    {
    retval = PAM_USER_UNKNOWN;
    }
  else if (user_pwd->pw_uid == 0)
    {
    if (debug) syslog(LOG_INFO, "allowing uid 0");

    retval = PAM_SUCCESS;
    }
  else
    {
    retval = PAM_AUTH_ERR;

    while ((jdent = readdir(jobdir)) != NULL)
      {
      if (strstr(jdent->d_name, ".JB") == NULL)
        continue;

      snprintf(jobpath, PATH_MAX - 1, "%s/%s", jobdirpath, jdent->d_name);

      if (debug) syslog(LOG_INFO, "opening %s", jobpath);

      /* try to read the JB file in XML format first */
      if (job_read_xml(jobpath, &xjob, log_buf, sizeof(log_buf)) != PBSE_NONE)
        {
        /* XML read failed, try binary version read */

        if (debug)
          {
          syslog(LOG_INFO, "failed to read JB file in XML format: %s", log_buf);
          syslog(LOG_INFO, "trying to read JB file in binary format");
          }

        fp = open(jobpath, O_RDONLY, 0);
  
        if (fp < 0)
  	  {
  	  syslog(LOG_ERR, "error opening job file");
  	  continue;
  	  }
  
        amt = read(fp, &xjob.ji_qs, sizeof(xjob.ji_qs));
  
        if (amt != sizeof(xjob.ji_qs))
  	  {
  	  close(fp);
  	  syslog(LOG_ERR, "short read of job file");
  	  continue;
  	  }
  
        if (xjob.ji_qs.ji_un_type != JOB_UNION_TYPE_MOM)
  	  {
  	  /* odd, this really should be JOB_UNION_TYPE_MOM */
  	  close(fp);
  	  syslog(LOG_ERR, "job file corrupt");
  	  continue;
  	  }
        close(fp);
        }
  
      if (debug) syslog(LOG_INFO, "state=%d, substate=%d", xjob.ji_qs.ji_state, xjob.ji_qs.ji_substate);

      if ((xjob.ji_qs.ji_un.ji_momt.ji_exuid == user_pwd->pw_uid) &&
          ((xjob.ji_qs.ji_substate == JOB_SUBSTATE_PRERUN) ||
           (xjob.ji_qs.ji_substate == JOB_SUBSTATE_STARTING) ||
           (xjob.ji_qs.ji_substate == JOB_SUBSTATE_RUNNING)))
        {
        /* success! */

        if (debug) syslog(LOG_INFO, "allowed by %s", jdent->d_name);

        retval = PAM_SUCCESS;

        break;
        }

      } /* END while (readdir(jobdir)) */

    if (jobdir)
      closedir(jobdir);
    }

  if (ubuf)
    free(ubuf);

  if (debug) syslog(LOG_INFO, "returning %s", retval == PAM_SUCCESS ? "success" : "failed");

  closelog();

  return retval;
  }

PAM_EXTERN
int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc,
                   const char **argv)
  {
  return PAM_SUCCESS;
  }

PAM_EXTERN
int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc,
                     const char **argv)
  {
  return pam_sm_authenticate(pamh, flags, argc, argv);
  }



#ifdef PAM_STATIC

/* static module data */
/* this only exists for the unlikely event that this built *with* libpam */

struct pam_module _pam_pbssimpleauth_modstruct =
  {
  MODNAME,
  pam_sm_authenticate,
  pam_sm_setcred,
  pam_sm_acct_mgmt,
  NULL,
  NULL,
  NULL,
  };

#endif

int job_read_xml(

  const char *filename,  /* I */   /* pathname to job save file */
  job  *pjob,     /* M */   /* pointer to a pointer of job structure to fill info */
  char *log_buf,   /* O */   /* buffer to hold error message */
  size_t buf_len)  /* I */   /* len of the error buffer */
  {
  xmlDoc *doc = NULL;
  xmlNode *root_element = NULL;

  xmlNode *cur_node = NULL;
  xmlChar  *content;
  bool     execution_uid_found = false;
  bool     state_found = false;
  bool     substate_found = false;

  /*parse the file and get the DOM */
  doc = xmlReadFile(filename, NULL, 0);

  if (doc == NULL)
    return(PBSE_INVALID_SYNTAX);

  /*Get the root element node */
  root_element = xmlDocGetRootElement(doc);
  if (strcmp((const char *) root_element->name, "job"))
    {
    snprintf(log_buf, buf_len, "missing root tag job in xml");

    /* set return code of -1 as we do have a JB xml but it did not have the right root elem. */
    xmlFreeDoc(doc);
    return(-1);
    }

  for (cur_node = root_element->children; cur_node != NULL; cur_node = cur_node->next)
    {
    /* skip text children, only process elements */
    if (!strcmp((const char *)cur_node->name, "text"))
      continue;

    if (!(strcmp((const char*)cur_node->name, "execution_uid")))
      {
      content = xmlNodeGetContent(cur_node);
      errno = 0;
      pjob->ji_qs.ji_un.ji_momt.ji_exuid = (uid_t) strtoul((const char *) content, NULL, 10);

      if (errno != 0)
        {
        snprintf(log_buf, buf_len, "invalid execution_uid");

        xmlFreeDoc(doc);
        return(-1);
        }

      execution_uid_found = true;
      }
    else if (!(strcmp((const char*)cur_node->name, "state")))
      {
      content = xmlNodeGetContent(cur_node);
      errno = 0;
      pjob->ji_qs.ji_state = (int) strtol((const char *) content, NULL, 10);

      if (errno != 0)
        {
        snprintf(log_buf, buf_len, "invalid state");

        xmlFreeDoc(doc);
        return(-1);
        }

      state_found = true;
      }
    else if (!(strcmp((const char*)cur_node->name, "substate")))
      {
      content = xmlNodeGetContent(cur_node);
      errno = 0;
      pjob->ji_qs.ji_substate = (int) strtol((const char *) content, NULL, 10);

      if (errno != 0)
        {
        snprintf(log_buf, buf_len, "invalid substate");

        xmlFreeDoc(doc);
        return(-1);
        }

      substate_found = true;
      }
    }

  xmlFreeDoc(doc);

  if (execution_uid_found == false)
    {
    snprintf(log_buf, buf_len, "%s", "Error: execution uid not found");
    return(-1);
    }

  if (state_found == false)
    {
    snprintf(log_buf, buf_len, "%s", "Error: state not found");
    return(-1);
    }

  if (substate_found == false)
    {
    snprintf(log_buf, buf_len, "%s", "Error: substate not found");
    return(-1);
    }

  return(PBSE_NONE);
  }
/* end of module definition */
