/* $Id: submit.c,v 1.16 2006/09/05 07:07:04 ciesnik Exp $ */
/*
 *  DRMAA library for Torque/PBS
 *  Copyright (C) 2006  Poznan Supercomputing and Networking Center
 *                      DSP team <dsp-devel@hedera.man.poznan.pl>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef HAVE_CONFIG_H
# include <pbs_config.h>
#endif

#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <drmaa_impl.h>
#include <jobs.h>
#include <attrib.h>
#include <compat.h>

#ifndef lint
static char rcsid[]
# ifdef __GNUC__
__attribute__((unused))
# endif
= "$Id: submit.c,v 1.16 2006/09/05 07:07:04 ciesnik Exp $";
#endif

int
drmaa_run_job(
  char *job_id, size_t job_id_len,
  const drmaa_job_template_t *jt,
  char *errmsg, size_t errlen
)
  {
  DEBUG(("-> drmaa_run_job"));
  return drmaa_run_job_impl(job_id, job_id_len, jt, -1, errmsg, errlen);
  }


int
drmaa_run_bulk_jobs(
  drmaa_job_ids_t **job_ids,
  const drmaa_job_template_t *jt,
  int start, int end, int incr,
  char *errmsg, size_t errlen
)
  {
  unsigned n_jobs;
  unsigned i;
  char **j;
  int rc = DRMAA_ERRNO_SUCCESS;

  DEBUG(("-> drmaa_run_bulk_jobs(start=%d,end=%d,incr=%d)", start, end, incr));

  /* Be conform with general DRMAA specifiaction
   * -- accept negative incr with end <= start.
   */

  if (incr < 0)
    {
    /* swap(start,end) */
    int tmp;
    tmp   = start;
    start = end;
    end   = tmp;
    incr  = - incr;
    }

  if (0 < start  &&  start <= end  &&  0 < incr)
      {}
  else return DRMAA_ERRNO_INVALID_ARGUMENT;

  n_jobs = (end - start) / incr + 1;

  *job_ids = (drmaa_job_ids_t*)malloc(sizeof(drmaa_job_ids_t));

  if (*job_ids == NULL)
    RAISE_NO_MEMORY();

  (*job_ids)->list = (*job_ids)->iter = (char**) calloc(n_jobs + 1, sizeof(char*));

  if ((*job_ids)->list == NULL)
    {
    free(*job_ids);
    RAISE_NO_MEMORY();
    }

  j = (*job_ids)->list;

  for (i = start;  i <= (unsigned)end;  i += incr)
    {
    char *job_id = (char*)malloc(DRMAA_JOBNAME_BUFFER);

    if (job_id == NULL)
      {
      drmaa_release_job_ids(*job_ids);
      RAISE_NO_MEMORY();
      }

    rc = drmaa_run_job_impl(job_id, DRMAA_JOBNAME_BUFFER, jt, i, errmsg, errlen);

    *j++ = job_id;

    if (rc)
      {
      drmaa_release_job_ids(*job_ids);
      return rc;
      }
    }

  *j++ = NULL;

  DEBUG(("<- drmaa_run_bulk_jobs() =%d, job_ids=%p", rc, (void*)*job_ids));
  return DRMAA_ERRNO_SUCCESS;
  }


/**
 * Submits job.  In addtion do drmaa_run_job() it has @a bulk_no which
 * should be -1 for submiting single job or bulk job index for bulk jobs.
 * @see drmaa_run_job
 * @see drmaa_run_bulk_jobs
 */
int
drmaa_run_job_impl(
  char *job_id, size_t job_id_len,
  const drmaa_job_template_t *jt, int bulk_no,
  char *errmsg, size_t errlen
)
  {
  drmaa_session_t *c      = NULL;
  drmaa_submission_context_t *sc = NULL;
  char *pbs_job_id;
  drmaa_job_t *job = NULL;
  int rc = DRMAA_ERRNO_SUCCESS;

  rc = drmaa_create_submission_context(&sc, jt, bulk_no, errmsg, errlen);

  if (rc)
    return rc;

  pthread_mutex_lock((pthread_mutex_t*)&jt->mutex);

  if (rc == DRMAA_ERRNO_SUCCESS)
    rc = drmaa_set_job_std_attribs(sc, errmsg, errlen);

  if (rc == DRMAA_ERRNO_SUCCESS)
    rc = drmaa_create_job_script(sc, errmsg, errlen);

  if (rc == DRMAA_ERRNO_SUCCESS)
    rc = drmaa_set_job_submit_state(sc, errmsg, errlen);

  if (rc == DRMAA_ERRNO_SUCCESS)
    rc = drmaa_set_job_environment(sc, errmsg, errlen);

  if (rc == DRMAA_ERRNO_SUCCESS)
    rc = drmaa_set_job_files(sc, errmsg, errlen);

  if (rc == DRMAA_ERRNO_SUCCESS)
    rc = drmaa_set_file_staging(sc, errmsg, errlen);

  if (rc == DRMAA_ERRNO_SUCCESS)
    rc = drmaa_set_job_email_notication(sc, errmsg, errlen);

  if (rc == DRMAA_ERRNO_SUCCESS)
    job = (drmaa_job_t*)malloc(sizeof(drmaa_job_t));

  pthread_mutex_unlock((pthread_mutex_t*)&jt->mutex);

  if (rc == DRMAA_ERRNO_SUCCESS  &&  job == NULL)
    {
    STORE_ERRNO_MSG();
    rc = DRMAA_ERRNO_NO_MEMORY;
    }

  /* check for error */
  if (rc != DRMAA_ERRNO_SUCCESS)
    {
    drmaa_free_submission_context(sc);
    return rc;
    }

#if DRMAA_DEBUG
    {

    struct attropl *i;

    for (i = sc->pbs_attribs;  i != NULL;  i = i->next)
      DEBUG(("job attr: %s=%s", i->name, i->value));
    }
#endif

  c = jt->session;

  pthread_mutex_lock(&c->conn_mutex);

  pbs_job_id = pbs_submit(c->pbs_conn, sc->pbs_attribs, sc->script_filename,
                          "", NULL);

  pthread_mutex_unlock(&c->conn_mutex);

  drmaa_free_submission_context(sc);

  if (pbs_job_id == NULL)
    RAISE_PBS();

  strlcpy(job_id, pbs_job_id, job_id_len);

  DEBUG(("job_id=%s", pbs_job_id));

  job->jobid      = pbs_job_id;

  job->terminated = false;

  job->suspended  = false;

  drmaa_add_job(c, job);

  return DRMAA_ERRNO_SUCCESS;
  }


int
drmaa_create_submission_context(
  drmaa_submission_context_t **c,
  const drmaa_job_template_t *jt, int bulk_no,
  char *errmsg, size_t errlen)
  {
  drmaa_submission_context_t *sc;
  sc = (drmaa_submission_context_t*)malloc(sizeof(drmaa_submission_context_t));

  if (sc == NULL)
    RAISE_NO_MEMORY();

  sc->jt = jt;

  sc->pbs_attribs = NULL;

  sc->script_filename = NULL;

  sc->home_directory    = strdup(getenv("HOME"));

  if (jt->attrib[ATTR_JOB_WORKING_DIR] != NULL)
    sc->working_directory = strdup(jt->attrib[ATTR_JOB_WORKING_DIR]);
  else
    sc->working_directory = strdup(sc->home_directory);

  asprintf(&sc->bulk_incr_no, "%d", bulk_no);

  *c = sc;

  return DRMAA_ERRNO_SUCCESS;
  }


void
drmaa_free_submission_context(drmaa_submission_context_t *c)
  {
  if (c->script_filename != NULL)
    {
    unlink(c->script_filename);
    free(c->script_filename);
    }

  free(c->working_directory);

  free(c->home_directory);
  free(c->bulk_incr_no);
  drmaa_free_attropl(c->pbs_attribs);
  free(c);
  }



struct drmaa_def_attr_s
  {
  int         attr;
  const char *value;
  };

int
drmaa_set_job_std_attribs(
  drmaa_submission_context_t *c,
  char *errmsg, size_t errlen
)
  {

  static const struct drmaa_def_attr_s tab[] =
    {
      {
      ATTR_CHECKPOINT, "u"
      },

    { ATTR_KEEP_FILES, "n" },
    { ATTR_PRIORITY,   "0" },
    { ATTR_RERUNABLE,  "y" },
    { ATTR_SHELL,      "/bin/sh" },
    { -1, NULL }
    };

  const struct drmaa_def_attr_s *i;
  void **attrib = c->jt->attrib;
  const char *job_name;
  int rc;

  job_name = attrib[ATTR_JOB_NAME];

  if (job_name != NULL)
    {
    rc = drmaa_add_pbs_attr(c, ATTR_JOB_NAME, strdup(attrib[ATTR_JOB_NAME]),
                            DRMAA_PLACEHOLDER_MASK_INCR, errmsg, errlen);

    if (rc)  return rc;
    }

  for (i = tab;  i->attr != -1;  i++)
    {
    rc = drmaa_add_pbs_attr(c, i->attr, strdup(i->value), 0, errmsg, errlen);

    if (rc)  return rc;
    }

  return DRMAA_ERRNO_SUCCESS;
  }



/** Writes job's PBS script to temporary file. */
int
drmaa_create_job_script(
  drmaa_submission_context_t *c,
  char *errmsg, size_t errlen
)
  {
  size_t script_size;
  const char *job, *wd, **argv;
  char *input_path;
  char *script, *s;
  const char **i;
  int rc;
  void **attrib = c->jt->attrib;

  job         = (const char*) attrib[ ATTR_JOB_PATH        ];
  wd          = (const char*) attrib[ ATTR_JOB_WORKING_DIR ];
  argv        = (const char**)attrib[ ATTR_ARGV            ];
  input_path  =               attrib[ ATTR_INPUT_PATH      ];

  if (job == NULL)
    RAISE_DRMAA(DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE);

  /* compute script length */
  script_size = 0;

  if (wd != NULL)
    script_size += strlen("cd ") + strlen(wd) + strlen("; ");

  script_size += strlen("exec ") + strlen(job);

  if (argv != NULL)
    for (i = argv;  *i != NULL;  i++)
      script_size += 1 + strlen(*i);

  if (input_path != NULL)
    script_size += sizeof(" <") + strlen(input_path);

  script = (char*)malloc(script_size + 1);

  if (script == NULL)
    RAISE_NO_MEMORY();

  s = script;

  if (wd != NULL)
    s += sprintf(s, "cd %s; ", wd);

  s += sprintf(s, "exec %s", job);

  if (argv != NULL)
    for (i = argv;  *i != NULL;  i++)
      s += sprintf(s, " %s", *i);

  if (input_path != NULL)
    {
    char *colon = strchr(input_path, ':');

    if (colon != NULL)
      input_path = colon + 1;

    s += sprintf(s, " <%s", input_path);
    }

  DEBUG(("script before substitution = {%s}", script));

  script = drmaa_expand_placeholders(c, script,
                                     DRMAA_PLACEHOLDER_MASK_HD |
                                     DRMAA_PLACEHOLDER_MASK_WD |
                                     DRMAA_PLACEHOLDER_MASK_INCR);
  DEBUG(("script after substitution = {%s}", script));

  rc = drmaa_write_tmpfile(&c->script_filename,
                           script, strlen(script), errmsg, errlen);
  free(script);
  return rc;
  }


/**
 * Set files where job's standard input/output/error
 * streams will be redirected.
 */
int
drmaa_set_job_files(
  drmaa_submission_context_t *c,
  char *errmsg, size_t errlen
)
  {
  void **attrib = c->jt->attrib;
  const char *join_files = attrib[ATTR_JOIN_FILES];
  bool b_join_files;
  int i;
  int rc;

  for (i = 0;  i < 2;  i++)
    {
    int attr = i ? ATTR_OUTPUT_PATH : ATTR_ERROR_PATH;
    char *path = (char*)attrib[attr];

    if (path != NULL)
      {
      char *colon;
      colon = strchr(path, ':');

      if (colon != NULL)
        path = colon + 1;

      rc = drmaa_add_pbs_attr(c, attr, strdup(path),
                              DRMAA_PLACEHOLDER_MASK_HD
                              | DRMAA_PLACEHOLDER_MASK_WD
                              | DRMAA_PLACEHOLDER_MASK_INCR, errmsg, errlen
                             );

      if (rc)
        return rc;
      }
    }

  b_join_files = join_files != NULL  &&  !strcmp(join_files, "1");

  rc = drmaa_add_pbs_attr(c, ATTR_JOIN_FILES, strdup(b_join_files ? "y" : "n"),
                          0, errmsg, errlen);

  if (rc)  return rc;

  return DRMAA_ERRNO_SUCCESS;
  }


/** Sets job environment variables. */
int
drmaa_set_job_environment(
  drmaa_submission_context_t *c,
  char *errmsg, size_t errlen
)
  {
  void **attrib = c->jt->attrib;
  size_t s;
  char *env;
  int rc;

  env = strdup("");
  s = strlen(env);

  if (attrib[ATTR_ENV] != NULL)
    {
    char *value = drmaa_explode(attrib[ATTR_ENV], ',');

    if (value == NULL)
      {
      free(env);
      RAISE_NO_MEMORY();
      }

    env = realloc(env, s + strlen(value) + 1);

    strcpy(env + s, value);
    free(value);
    }
  else env[s-1] = 0;

  rc = drmaa_add_pbs_attr(c, ATTR_ENV, env,
                          DRMAA_PLACEHOLDER_MASK_INCR, errmsg, errlen);

  if (rc)
    return rc;

  return DRMAA_ERRNO_SUCCESS;
  }


/**
 * Set e-mail notification about job state.
 */
int
drmaa_set_job_email_notication(
  drmaa_submission_context_t *c,
  char *errmsg, size_t errlen
)
  {
  void **attrib = c->jt->attrib;
  const char *block_email = (const char*)attrib[ATTR_BLOCK_EMAIL];
  int rc;

  if (block_email != NULL  &&  !strcmp(block_email, "1"))
    {
    rc = drmaa_add_pbs_attr(c, ATTR_MAIL_POINTS, strdup(""), 0, errmsg, errlen);

    if (rc)
      return rc;
    }
  else
    {
    const char **mail_list = (const char**)attrib[ATTR_EMAIL];
    char *pbs_mail_list = NULL;

    if (mail_list != NULL)
      {
      pbs_mail_list = drmaa_explode(mail_list, ',');

      if (pbs_mail_list == NULL)
        RAISE_NO_MEMORY();

      rc = drmaa_add_pbs_attr(c, ATTR_EMAIL, pbs_mail_list, 0, errmsg, errlen);

      if (rc)
        {
        free(pbs_mail_list);
        return rc;
        }

      /* a  mail is sent when the job is aborted by the batch system.
         b  mail is sent when the job begins execution.
         e  mail is sent when the job terminates. */
      rc = drmaa_add_pbs_attr(c, ATTR_MAIL_POINTS, strdup("ae"), 0, errmsg, errlen);

      if (rc)
        return rc;
      }
    }

  return DRMAA_ERRNO_SUCCESS;
  }


int
drmaa_set_job_submit_state(
  drmaa_submission_context_t *c,
  char *errmsg, size_t errlen
)
  {
  const char *submit_state = c->jt->attrib[ATTR_JOB_SUBMIT_STATE];
  const char *hold_types;
  int rc = DRMAA_ERRNO_SUCCESS;

  if (submit_state == NULL || !strcmp(submit_state, DRMAA_SUBMISSION_STATE_ACTIVE))
    hold_types = "n";
  else if (!strcmp(submit_state, DRMAA_SUBMISSION_STATE_HOLD))
    hold_types = "u";
  else
    RAISE_DRMAA(DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE);

  rc = drmaa_add_pbs_attr(c, ATTR_HOLD_TYPES, strdup(hold_types), 0, errmsg, errlen);

  return rc;
  }

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 255
#endif

char *
drmaa_translate_staging(const char *stage)
  {
  char hostname[ HOST_NAME_MAX+1 ];
  const char *host = NULL, *filename = NULL;
  size_t hostlen = 0;
  char *result, *p;

  p = strchr(stage, ':');

  if (p == NULL)
    {
    host = NULL;
    filename = stage;
    }
  else
    {
    if (p == stage)
      host = NULL;
    else
      {
      host    = stage;
      hostlen = p - stage;
      }

    filename = p + 1;
    }

  if (host == NULL)
    {
    gethostname(hostname, sizeof(hostname));
    host = hostname;
    hostlen = strlen(hostname);
    }

  asprintf(&result, "%s@%.*s:%s", filename, hostlen, host, filename);

  return result;
  }


int
drmaa_set_file_staging(
  drmaa_submission_context_t *c,
  char *errmsg, size_t errlen
)
  {
  void **attrib = c->jt->attrib;
  char *transfer_files = (char*)attrib[ ATTR_TRANSFER_FILES ];
  char *input_file  = NULL;
  char *output_file = NULL;
  char *error_file  = NULL;
  char *output_staging = NULL;
  char *error_staging  = NULL;
  char *stageout = NULL;
  int rc = DRMAA_ERRNO_SUCCESS;

  if (transfer_files)
    {
    char *i;

    for (i = transfer_files;  *i;  i++)
      switch (*i)
        {

        case 'i':
          input_file  = (char*)attrib[ ATTR_INPUT_PATH  ];
          break;

        case 'o':
          output_file = (char*)attrib[ ATTR_OUTPUT_PATH ];
          break;

        case 'e':
          error_file  = (char*)attrib[ ATTR_ERROR_PATH  ];
          break;

        default:
          RAISE_DRMAA(DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE);
        }
    }
  else
    return DRMAA_ERRNO_SUCCESS;

  if (input_file)
    {
    rc = drmaa_add_pbs_attr(c, ATTR_STAGEIN,
                            drmaa_translate_staging(input_file),
                            DRMAA_PLACEHOLDER_MASK_HD |
                            DRMAA_PLACEHOLDER_MASK_WD |
                            DRMAA_PLACEHOLDER_MASK_INCR,
                            errmsg, errlen);

    if (rc)
      return rc;
    }

  if (output_file)
    output_staging = drmaa_translate_staging(output_file);

  if (error_file)
    error_staging = drmaa_translate_staging(error_file);

  if (output_staging && error_staging)
    asprintf(&stageout, "%s,%s", output_staging, error_staging);
  else if (output_staging)
    stageout = output_staging;
  else if (error_staging)
    stageout = error_staging;

  rc = drmaa_add_pbs_attr(c, ATTR_STAGEOUT, stageout,
                          DRMAA_PLACEHOLDER_MASK_HD |
                          DRMAA_PLACEHOLDER_MASK_WD |
                          DRMAA_PLACEHOLDER_MASK_INCR,
                          errmsg, errlen);

  if (output_staging  &&  output_staging != stageout)
    free(output_staging);

  if (error_staging  &&  error_staging != stageout)
    free(error_staging);

  return rc;
  }


/**
 * Adds jobs PBS attributes list.
 * @param attr_code  Attribute code (@ref drmaa_attribute_t).
 * @param value  Attribute value -- null terminated string allocated with
 *  malloc();  <b>callee</b> is responsible for freeing it.
 * @param placeholders  Placeholders to substitute inside value.
 *  It have following bits:
 *  - @ref DRMAA_PLACEHOLDER_MASK_HD -- replaces @ref DRMAA_PLACEHOLDER_HD with
 *    user's home direcotry,
 *  - @ref DRMAA_PLACEHOLDER_MASK_WD -- replaces @ref DRMAA_PLACEHOLDER_WD with
 *    job's working direcotry,
 *  - @ref DRMAA_PLACEHOLDER_MASK_INCR -- replaces @ref DRMAA_PLACEHOLDER_INCR
 *    with bulk job index.
 */
int
drmaa_add_pbs_attr(
  drmaa_submission_context_t *c,
  int attr_code, char *value,
  unsigned placeholders,
  char *errmsg, size_t errlen
)
  {

  struct attropl *attr = NULL;

  value = drmaa_expand_placeholders(c, value, placeholders);

  if (value == NULL)
    goto cleanup;

  attr = (struct attropl*)malloc(sizeof(*attr));

  if (attr == NULL)
    goto cleanup;

  attr->next     = c->pbs_attribs;

  attr->name     = strdup(drmaa_attr_table[attr_code].pbs_name);

  if (attr->name == NULL)
    goto cleanup;

  attr->value    = value;

  attr->resource = NULL;

  attr->op       = SET;

  c->pbs_attribs = attr;

  return DRMAA_ERRNO_SUCCESS;

cleanup:
  if (value != NULL)
    free(value);

  if (attr != NULL)
    free(attr);

  RAISE_NO_MEMORY();
  }


/**
 * Writes temporary file.
 * @param filename  Upon successful return temporary
 *                  file name will be stored here.
 * @param content   Buffer with content to write.
 * @param len       Buffer's length.
 * @return DRMAA return code.
 */
int
drmaa_write_tmpfile(
  char **filename, const char *content, size_t len,
  char *errmsg, size_t errlen
)
  {
  char *name;
  int fd;
#define TMPFILE_TEMPLATE "/tmp/pbs_drmaa.XXXXXX"

  DEBUG(("-> drmaa_write_tmpfile(content=\"%.*s\")", len, content));

  name = strdup(TMPFILE_TEMPLATE);

  if (name == NULL)
    RAISE_NO_MEMORY();

  fd = mkstemp(name);

  if (fd < 0)
    RAISE_ERRNO(DRMAA_ERRNO_INTERNAL_ERROR);

  while (len > 0)
    {
    size_t written = write(fd, content, len);

    if (written != (size_t) - 1)
      {
      content += written;
      len     -= written;
      }
    else
      {
      free(name);
      close(fd);
      RAISE_ERRNO(DRMAA_ERRNO_INTERNAL_ERROR);
      }
    }

  if (close(fd))
    {
    free(name);
    RAISE_ERRNO(DRMAA_ERRNO_INTERNAL_ERROR);
    }

  DEBUG(("<- drmaa_write_tmpfile; filename=%s", name));

  *filename = name;
  return DRMAA_ERRNO_SUCCESS;
  }


char *
drmaa_explode(const char **vector, char glue)
  {
  char *result, *s;
  const char **i;
  size_t size = 0;

  for (i = vector;  *i != NULL;  i++)
    {
    if (i != vector)
      size++;

    size += strlen(*i);
    }

  result = (char*)malloc(size + 1);

  if (result == NULL)
    return NULL;

  s = result;

  for (i = vector;  *i != NULL;  i++)
    {
    if (i != vector)
      *s++ = glue;

    strcpy(s, *i);

    s += strlen(*i);
    }

  return result;
  }


void
drmaa_free_attropl(struct attropl *attr)
  {
  while (attr != NULL)
    {

    struct attropl *p = attr;
    attr = attr->next;
    free(p->name);
    free(p->value);
    free(p->resource);
    free(p);
    }
  }


char *
drmaa_expand_placeholders(
  drmaa_submission_context_t *c, char *s, unsigned set)
  {
  unsigned mask;

  for (mask = 1;  ;  mask *= 2)
    {
    const char *ph;
    const char *value;
    char *r;

    switch (mask)
      {

      case DRMAA_PLACEHOLDER_MASK_HD:
        ph = DRMAA_PLACEHOLDER_HD;
        value = c->home_directory;
        break;

      case DRMAA_PLACEHOLDER_MASK_WD:
        ph = DRMAA_PLACEHOLDER_WD;
        value = c->working_directory;
        break;

      case DRMAA_PLACEHOLDER_MASK_INCR:
        ph = DRMAA_PLACEHOLDER_INCR;
        value = c->bulk_incr_no;
        break;

      default:
        return s;
      }

    if (mask & set)
      {
      r = drmaa_replace(s, ph, value);

      if (r != NULL)
        s = r;
      else
        {
        free(s);
        return NULL;
        }
      }
    }
  }


#if 0
char *
drmaa_quote_shell_command(char *word)
  {
  unsigned char *i;
  char *quoted = NULL;

  for (i = (unsigned char*)word;  *i;  i++)
    if (!isalnum(*i))
      switch (*i)
        {

        case '/':

        case '.':

        case '_':

        case '-':
          break;

        default:
          {
          if (quoted)

            }
        }
  }

#endif

