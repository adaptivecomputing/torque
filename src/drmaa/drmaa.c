/***************************************************************************
                          drmaa.c  -  description
                             -------------------
    begin                : mar sep 20 2005
    copyright            : (C) 2005 by Cargnelli Matthieu
    email                : matthieu.cargnelli@eads.net
***************************************************************************/

/**

DRMAA library for OpenPBS/Torque
Copyright (C) 2005 EADS CCR

This library is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or (at
your option) any later version.

This library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation,
Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Author: Matthieu Cargnelli <matthieu.cargnelli@eads.net>

*/ 

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

#ifdef sun
#include <sys/stream.h>
#endif /* sun */

#if defined(HAVE_SYS_IOCTL_H)
#include <sys/ioctl.h>
#endif	/* HAVE_SYS_IOCTL_H */

#if !defined(sgi) && !defined(_AIX) && !defined(linux)
#include <sys/tty.h>
#endif

#if defined(FD_SET_IN_SYS_SELECT_H)
#  include <sys/select.h>
#endif

#include <pthread.h>

#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "drmaa.h"
#include "pbs_ifl.h"

#define min(a, b)       ((a) < (b) ? (a) : (b))

char* replacePattern(char* source, char* pattern,char* replacement);

int _connect = -1;
char* pbs_contact;

drmaa_attr_names_t* _supported_attr_names;
drmaa_attr_names_t* _supported_attr_v_names;
static pthread_mutex_t _supported_attr_names_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t _supported_attr_v_names_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t _submit_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t _wait_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t _job_list_lock = PTHREAD_MUTEX_INITIALIZER;

struct drmaa_attr_names_s
{
  char* attr_name;
  drmaa_attr_names_t* next;
  drmaa_attr_names_t* iterator;  
};

struct drmaa_attr_values_s
{
  char* attr_value;
  drmaa_attr_values_t* next;
  drmaa_attr_values_t* iterator; // pointer used in the the get_next function
};

struct drmaa_job_ids_s
{
  char* job_ids;
  drmaa_job_ids_t* next;
  drmaa_job_ids_t* iterator;
};

// job template struct definition
struct drmaa_job_template_s
{
  drmaa_attr_names_t* _attr_names;
  drmaa_attr_names_t* _attr_v_names;
  drmaa_attr_values_t* _attr_values;
  drmaa_attr_values_t* _attr_v_values[3];
};

struct job_list_s{
  drmaa_job_template_t* job;
  char* job_id;
  struct job_list_s* next;
} *job_list = NULL;

int drmaa_get_next_attr_name(drmaa_attr_names_t* values, char *value, int value_len)
{
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;

  //printf("next value from %p asked\n",values);

  if (values && values->iterator){
    strncpy(value,values->iterator->attr_name,value_len-1);
    value[value_len-1]='\0';
    values->iterator = values->iterator->next;
    return DRMAA_ERRNO_SUCCESS;
  }
  else{
    //printf("no more elements\n");
    return DRMAA_ERRNO_NO_MORE_ELEMENTS;
  }
}

int drmaa_get_next_attr_value(drmaa_attr_values_t* values, char *value, int value_len)
{
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;

  //printf ("in drmaa_get_next_attr_value, values = %p, values->iterator = %p\n");

  if (values){
    strncpy(value,values->iterator->attr_value,value_len-1);
    value[value_len-1]='\0';
    values->iterator = values->iterator->next;
    return DRMAA_ERRNO_SUCCESS;
  }
  else
    return DRMAA_ERRNO_NO_MORE_ELEMENTS;
}

int drmaa_get_next_job_id(drmaa_job_ids_t* values, char *value, int value_len)
{
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;

  if (values->iterator){
    strncpy(value,values->iterator->job_ids,value_len-1);
    value[value_len-1]='\0';
    values->iterator = values->iterator->next;    
    return DRMAA_ERRNO_SUCCESS;
  }
  else{
    //printf("no more element\n");
    return DRMAA_ERRNO_NO_MORE_ELEMENTS;
  }
}

int add_attr_name(drmaa_attr_names_t* values, char *value)
{
  //printf("Add_attr_name\n");
  //printf("adding %s to values\n",value);
  drmaa_attr_names_t* current = values;
  if (!values->attr_name)
    {
      if ((current->attr_name = (char*)calloc(1,(strlen(value)+1)*sizeof(char)))==NULL)
        return DRMAA_ERRNO_NO_MEMORY;
      strncpy(current->attr_name,value,strlen(value)+1);
      current->next = NULL;
      current->iterator = current;
    }
  else{
    //printf("Non empty list\n");
    while (current->next)
      current=current->next;
    if ((current->next = (drmaa_attr_names_t*) calloc(1,sizeof (drmaa_attr_names_t)))==NULL)
      return DRMAA_ERRNO_NO_MEMORY;   
    if ((current->next->attr_name = (char*)calloc(1,(strlen(value)+1)*sizeof(char)))==NULL)
      return DRMAA_ERRNO_NO_MEMORY;
    strncpy(current->next->attr_name,value,strlen(value)+1);
    current->next->next = NULL;
  }
  return DRMAA_ERRNO_SUCCESS;
}

/*
 * release opaque string vector
 *
 * Opaque string vectors can be used without any constraint
 * until the release function has been called.
 */
void drmaa_release_attr_names( drmaa_attr_names_t* values )
{
  if (!values) return;
  drmaa_attr_names_t* next = values->next;
  while (values){
    free (values->attr_name);
    free (values);
    values = next;
    if (values)
      next = values->next;
  }
}
void drmaa_release_attr_values(drmaa_attr_values_t* values)
{
  if (!values) return;
  drmaa_attr_values_t* next;
  while (values){
    next = values->next;
    //printf("freeing %s\n",values->attr_value);
    free (values->attr_value);
    free (values);
    values = next;
  }
}
void drmaa_release_job_ids( drmaa_job_ids_t* values )
{
  drmaa_job_ids_t* current = values;
  while (values){
    current = values->next;
    free(values->job_ids);
    //values->job_ids=NULL;
    free(values);
    values = current;
  }
}

/* ------------------- init/exit routines ------------------- */
/*
 * Initialize DRMAA API library and create a new DRMAA Session. 'Contact'
 * is an implementation dependent string which may be used to specify
 * which DRM system to use. This routine must be called before any
 * other DRMAA calls, except for drmaa_version().
 * If 'contact' is NULL, the default DRM system will be used.
 */
int drmaa_init(const char *contact, char *error_diagnosis, size_t error_diag_len)
{
  if (_connect != -1)
    return DRMAA_ERRNO_ALREADY_ACTIVE_SESSION;
  fprintf(stderr,"libdrmaa for pbs version 0.0.1, init...\n");
  if ((_supported_attr_names = (drmaa_attr_names_t*)calloc(1,sizeof(drmaa_attr_names_t)))==NULL)
    return DRMAA_ERRNO_NO_MEMORY;
  //printf("Filling attr_names\n");
  add_attr_name(_supported_attr_names,"drmaa_remote_command");
  add_attr_name(_supported_attr_names,"drmaa_js_state");
  add_attr_name(_supported_attr_names,"drmaa_wd");
  add_attr_name(_supported_attr_names,"drmaa_job_name");
  add_attr_name(_supported_attr_names,"drmaa_input_path");
  add_attr_name(_supported_attr_names,"drmaa_output_path");
  add_attr_name(_supported_attr_names,"drmaa_error_path");
  add_attr_name(_supported_attr_names,"drmaa_join_files");
  add_attr_name(_supported_attr_names,"drmaa_job_category");
  add_attr_name(_supported_attr_names,"drmaa_native_specification");
  add_attr_name(_supported_attr_names,"drmaa_block_email");
  add_attr_name(_supported_attr_names,"drmaa_start_time");
  add_attr_name(_supported_attr_names,"drmaa_transfer_files");
  add_attr_name(_supported_attr_names,"drmaa_deadline_time");
  add_attr_name(_supported_attr_names,"drmaa_wct_hlimit");
  add_attr_name(_supported_attr_names,"drmaa_wct_slimit");
  add_attr_name(_supported_attr_names,"drmaa_duration_hlimit");
  add_attr_name(_supported_attr_names,"drmaa_duration_slimit");
  //printf("filling done\n");

  if ((_supported_attr_v_names = (drmaa_attr_names_t*)calloc(1,sizeof(drmaa_attr_names_t)))==NULL)
    return DRMAA_ERRNO_NO_MEMORY;
  add_attr_name(_supported_attr_v_names,"drmaa_v_env");
  add_attr_name(_supported_attr_v_names,"drmaa_v_argv");
  add_attr_name(_supported_attr_v_names,"drmaa_v_email");

  if (contact){
    size_t len = strlen(contact)+1;
    if ((pbs_contact=(char*)calloc(1,len*sizeof(char)))==NULL)
      return DRMAA_ERRNO_NO_MEMORY;
    strncpy(pbs_contact,contact,len);
  }
  else{
    if ((pbs_contact=(char*)calloc(1,10*sizeof(char)))==NULL)
      return DRMAA_ERRNO_NO_MEMORY;
    strcpy(pbs_contact,"localhost");
  }
  fprintf (stderr,"connecting to %s\n",pbs_contact);
  _connect = pbs_connect((char *)contact);
  //printf("Connect = %d\n",_connect);
  if (_connect < 0) { // si erreur
    fprintf(stderr,"Error!\n");
    char* errmsg = pbs_geterrmsg(_connect);
    fprintf(stderr,"Error is: %s\n",errmsg);
    char* msg;
    if ((msg = (char*)calloc(1,DRMAA_ERROR_STRING_BUFFER*sizeof(char)))==NULL){
      drmaa_release_attr_names((drmaa_attr_names_t*)_supported_attr_names);
      return DRMAA_ERRNO_NO_MEMORY;
    }
    strcpy(msg,"PBS Server on host \"");
    strcat(msg,pbs_contact);
    strcat(msg,"\" : ");
    strcat(msg,(errmsg ? errmsg : "Reason unknown"));
    strncpy(error_diagnosis,msg,error_diag_len);
    fprintf(stderr,"Error diag=%s\n",error_diagnosis);
    fprintf(stderr,"freeing attr_names\n");
    drmaa_release_attr_names(_supported_attr_names);
    drmaa_release_attr_names(_supported_attr_v_names);
    return DRMAA_ERRNO_DRMS_INIT_FAILED;
  }
  //fprintf(stderr,"OK\n");
  return DRMAA_ERRNO_SUCCESS;

}


/*
 * Disengage from DRMAA library and allow the DRMAA library to perform
 * any necessary internal clean up.
 * This routine ends this DRMAA Session, but does not effect any jobs (e.g.,
 * queued and running jobs remain queued and running).
 */
int drmaa_exit(char *error_diagnosis, size_t error_diag_len)
{
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;
  //drmaa_release_job_ids();
  // On se deconnecte du serveur PBS
  int rc = pbs_disconnect(_connect);
  if (rc < 0) { // si erreur
    char* msg = "PBS Server";
    char* errmsg = pbs_geterrmsg(_connect);
    strcat(msg,(errmsg ? errmsg : "Reason unknown"));
    strcpy(error_diagnosis,msg);
    error_diag_len = strlen(error_diagnosis);
    return DRMAA_ERRNO_DRMS_EXIT_ERROR;
  }
  free (pbs_contact);

  /*****************************************
   * free job_list
   ****************************************/

  pthread_mutex_lock(&_job_list_lock);
  int ret;
  struct job_list_s* current = job_list;
  //fprintf(stderr,"Freeing the running jobs list\n");
  while(job_list){
    current = job_list;
    job_list = job_list->next;
    if (current->job){
      //fprintf(stderr,"The job %s has its job template still defined, calling drmaa_delete_job_template\n",current->job_id);
      ret = drmaa_delete_job_template(current->job, error_diagnosis,error_diag_len);
      if ( ret != DRMAA_ERRNO_SUCCESS )
	return ret;
    }
    free(current->job_id);
    free(current);
  }
  pthread_mutex_unlock(&_job_list_lock);

  pthread_mutex_lock(&_supported_attr_names_lock);
  drmaa_release_attr_names(_supported_attr_names);
  pthread_mutex_unlock(&_supported_attr_names_lock);

  pthread_mutex_lock(&_supported_attr_v_names_lock);
  drmaa_release_attr_names(_supported_attr_v_names);
  pthread_mutex_unlock(&_supported_attr_v_names_lock);

  _connect = -1;

  return DRMAA_ERRNO_SUCCESS;
}

/* ------------------- job template routines ------------------- */

/*
 * Allocate a new job template.
 */
int drmaa_allocate_job_template(drmaa_job_template_t **jt, char *error_diagnosis, size_t error_diag_len)
{
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;
  *jt = (drmaa_job_template_t*) calloc(1,sizeof(drmaa_job_template_t));
  if (!jt)
    return DRMAA_ERRNO_NO_MEMORY;
  else  
    return DRMAA_ERRNO_SUCCESS;

}

/*
 * Deallocate a job template. This routine has no effect on jobs.
 */
int drmaa_delete_job_template(drmaa_job_template_t *jt, char *error_diagnosis, size_t error_diag_len)
{
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;
  //fprintf(stderr,"In drmaa_delete_job_template: deleting job attr_names\n");
  drmaa_release_attr_names(jt->_attr_names);
  //fprintf(stderr,"deleting job attr_v_names\n");
  drmaa_release_attr_names(jt->_attr_v_names);
  //fprintf(stderr,"deleting job attr_values\n");
  drmaa_release_attr_values(jt->_attr_values);
  int i = 0;
  while (i < 3){
    drmaa_release_attr_values(jt->_attr_v_values[i++]);
  }

  //for each job in job_list, set the job pointer to NULL if job == jt
  struct job_list_s* current = job_list;
  while(current){
    if (current->job == jt){
      //fprintf(stderr,"In drmaa_delete_job_template: \"job\" param for job %s in job_list will be set to NULL\n",current->job_id);
      pthread_mutex_lock(&_job_list_lock);
      current->job = NULL;
      pthread_mutex_unlock(&_job_list_lock);      
    }
    current = current->next;
  }
  if (jt)
    free(jt);
  return DRMAA_ERRNO_SUCCESS;
}


/*
 * Adds ('name', 'value') pair to list of attributes in job template 'jt'.
 * Only non-vector attributes may be passed.
 */
int drmaa_set_attribute(drmaa_job_template_t *jt, const char *name, const char *value, char *error_diagnosis, size_t error_diag_len)
{
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;
  if (!strcmp(DRMAA_REMOTE_COMMAND,name) || !strcmp(DRMAA_JS_STATE,name)
      || !strcmp(DRMAA_WD,name) || !strcmp(DRMAA_JOB_CATEGORY,name)
      || !strcmp(DRMAA_NATIVE_SPECIFICATION,name) ||!strcmp(DRMAA_BLOCK_EMAIL,name)
      || !strcmp(DRMAA_START_TIME,name) || !strcmp(DRMAA_JOB_NAME,name)
      || !strcmp(DRMAA_INPUT_PATH,name) || ! strcmp(DRMAA_OUTPUT_PATH,name)
      || !strcmp(DRMAA_ERROR_PATH,name) || !strcmp(DRMAA_JOIN_FILES,name)
      || !strcmp(DRMAA_TRANSFER_FILES,name) || !strcmp(DRMAA_DEADLINE_TIME,name)
      || !strcmp(DRMAA_WCT_HLIMIT,name) ||  !strcmp(DRMAA_WCT_SLIMIT,name)
      || !strcmp(DRMAA_DURATION_HLIMIT,name) || !strcmp(DRMAA_DURATION_SLIMIT,name))
    {
	
      //fprintf(stderr,"set_attribute %s to %s\n",name,value);
      drmaa_attr_names_t* current_attr_name = jt->_attr_names;
      drmaa_attr_values_t* current_attr_value = jt->_attr_values;

      if (jt->_attr_names == NULL){
	// first element
	//fprintf(stderr,"Attribute %s doesn't exist, adding it as first element\n",name);
	if ((jt->_attr_names = (drmaa_attr_names_t*) calloc(1,sizeof(drmaa_attr_names_t)))==NULL)
	  return DRMAA_ERRNO_NO_MEMORY;
	jt->_attr_names->next = NULL;//(drmaa_attr_names_t*) calloc(1,sizeof (drmaa_attr_names_t));
	if ((jt->_attr_names->attr_name = (char*)calloc(1,(strlen(name)+1)*sizeof(char)))==NULL)
	  return DRMAA_ERRNO_NO_MEMORY;    
	strncpy(jt->_attr_names->attr_name,name,strlen(name)+1);
	jt->_attr_names->iterator = jt->_attr_names;
	//fprintf(stderr,"Name %s added to the attr_name list, which is now %s\n",name,jt->_attr_names->attr_name);
	//fprintf(stderr,"The iterator of %p is now %p\n",jt->_attr_names,jt->_attr_names->iterator);
	//fprintf(stderr,"So jt->_attr_names->iterator->attr_name = %s\n",jt->_attr_names->iterator->attr_name);

	if ((jt->_attr_values = (drmaa_attr_values_t*) calloc(1,sizeof (drmaa_attr_values_t)))==NULL)
	  return DRMAA_ERRNO_NO_MEMORY;    
	jt->_attr_values->next = NULL;//(drmaa_attr_values_t*) calloc(1,sizeof (drmaa_attr_values_t));
	if ((jt->_attr_values->attr_value = (char*)calloc(1,(strlen(value)+1)*sizeof(char)))==NULL)
	  return DRMAA_ERRNO_NO_MEMORY;
	strncpy(jt->_attr_values->attr_value,value,strlen(value)+1);
	jt->_attr_values->iterator = jt->_attr_values;
	//fprintf(stderr,"Value %s added to the attr_value list, which is now %s\n",value,jt->_attr_values->attr_value);
      }
      else{
	//fprintf(stderr,"locating attribute %s\n",name);
	while (current_attr_name->next && strcmp(current_attr_name->attr_name,name)){
	  //fprintf(stderr,"one loop\n");
	  current_attr_name=current_attr_name->next;
	  current_attr_value=current_attr_value->next;
	}
	if (current_attr_name->attr_name != NULL && !strcmp(current_attr_name->attr_name,name)){
	  //fprintf(stderr,"Attribute %s already exists, modifying\n",name);
	  //value is modified
	  if (current_attr_value->attr_value)
	    free(current_attr_value->attr_value);
	  if ((current_attr_value->attr_value = (char*) calloc(1,(strlen(value)+1)*sizeof(char)))==NULL)
	    return DRMAA_ERRNO_NO_MEMORY;
	  strncpy(current_attr_value->attr_value,value,strlen(value)+1);
	}
	else{
	  //value is added at the end of the vector
	  //fprintf(stderr,"Attribute %s doesn't exist, adding it to the non null list\n",name);
	  if ((current_attr_name->next = (drmaa_attr_names_t*) calloc(1,sizeof (drmaa_attr_names_t)))==NULL)
	    return DRMAA_ERRNO_NO_MEMORY;
	  if ((current_attr_name->next->attr_name = (char*)calloc(1,(strlen(name)+1)*sizeof(char)))==NULL)
	    return DRMAA_ERRNO_NO_MEMORY;
	  strncpy(current_attr_name->next->attr_name,name,strlen(name)+1);
	  current_attr_name->next->next = NULL;
	  //fprintf(stderr,"Name %s added to the attr_name list\n",current_attr_name->next->attr_name);

	  if ((current_attr_value->next = (drmaa_attr_values_t*) calloc(1,sizeof (drmaa_attr_values_t)))==NULL)
	    return DRMAA_ERRNO_NO_MEMORY;
	  if ((current_attr_value->next->attr_value = (char*)calloc(1,(strlen(value)+1)*sizeof(char)))==NULL)
	    return DRMAA_ERRNO_NO_MEMORY;
	  strncpy(current_attr_value->next->attr_value,value,strlen(value)+1);
	  current_attr_value->next->next = NULL;
	  //fprintf(stderr,"Value %s added to the attr_value list\n",current_attr_value->next->attr_value);
	}
      }
      return DRMAA_ERRNO_SUCCESS;
    }
  else return DRMAA_ERRNO_INVALID_ARGUMENT;
}


/*
 * If 'name' is an existing non-vector attribute name in the job
 * template 'jt', then the value of 'name' is returned; otherwise,
 * NULL is returned.
 */
int drmaa_get_attribute(drmaa_job_template_t *jt, const char *name, char *value, size_t value_len, char *error_diagnosis, size_t error_diag_len)
{
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;
  if (jt->_attr_names && (!strcmp(DRMAA_REMOTE_COMMAND,name) || !strcmp(DRMAA_JS_STATE,name)
			  || !strcmp(DRMAA_WD,name) || !strcmp(DRMAA_JOB_CATEGORY,name)
			  || !strcmp(DRMAA_NATIVE_SPECIFICATION,name) ||!strcmp(DRMAA_BLOCK_EMAIL,name)
			  || !strcmp(DRMAA_START_TIME,name) || !strcmp(DRMAA_JOB_NAME,name)
			  || !strcmp(DRMAA_INPUT_PATH,name) || ! strcmp(DRMAA_OUTPUT_PATH,name)
			  || !strcmp(DRMAA_ERROR_PATH,name) || !strcmp(DRMAA_JOIN_FILES,name)
			  || !strcmp(DRMAA_TRANSFER_FILES,name) || !strcmp(DRMAA_DEADLINE_TIME,name)
			  || !strcmp(DRMAA_WCT_HLIMIT,name) ||  !strcmp(DRMAA_WCT_SLIMIT,name)
			  || !strcmp(DRMAA_DURATION_HLIMIT,name) || !strcmp(DRMAA_DURATION_SLIMIT,name)) )
    {
      drmaa_attr_names_t* current_attr_name = jt->_attr_names;
      drmaa_attr_values_t* current_attr_value = jt->_attr_values;
      while (current_attr_name->next && strcmp(current_attr_name->attr_name,name)){
	current_attr_name=current_attr_name->next;
	current_attr_value=current_attr_value->next;
      }
      if (!strcmp(current_attr_name->attr_name,name)){
	//printf ("found %s: current_attr_value->attr_value = %s\n",name, current_attr_value->attr_value);
	//value is modified
	strncpy(value,current_attr_value->attr_value,value_len);
	return DRMAA_ERRNO_SUCCESS;
      }
      else{
	return DRMAA_ERRNO_INVALID_ARGUMENT;
      }
    }
  else return DRMAA_ERRNO_INVALID_ARGUMENT;
}

/* Adds ('name', 'values') pair to list of vector attributes in job template 'jt'.
 * Only vector attributes may be passed.
 */
int drmaa_set_vector_attribute(drmaa_job_template_t *jt, const char *name, const char *value[], char *error_diagnosis, size_t error_diag_len)
{
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;

  //printf("\n\ndrmaa_set_vector_attribute\n\n\n");

  int i,count = 0;
  //fprintf(stderr,"setting a vector attribute\n");
  while ( value[count] != NULL )
    count++;
  //printf ("%d values found\n",count);
  if (!strcmp(DRMAA_V_ARGV,name) || !strcmp(DRMAA_V_ENV,name) || !strcmp(DRMAA_V_EMAIL,name))
    {
      drmaa_attr_names_t* current_attr_name = jt->_attr_v_names;
      drmaa_attr_values_t **current_attr_value, *c, *prec_attr_value;
      current_attr_value = jt->_attr_v_values;
      if (jt->_attr_v_names == NULL){
	//printf("No value yet for vector attributes (names), adding a new name for %s\n",name);
	// first element
	if ((jt->_attr_v_names = (drmaa_attr_names_t*) calloc(1,sizeof(drmaa_attr_names_t)))==NULL)
	  return DRMAA_ERRNO_NO_MEMORY;
	jt->_attr_v_names->next = NULL;
	if ((jt->_attr_v_names->attr_name = (char*)calloc(1,(strlen(name)+1)*sizeof(char)))==NULL)
	  return DRMAA_ERRNO_NO_MEMORY;
	strncpy(jt->_attr_v_names->attr_name,name,strlen(name)+1);
	jt->_attr_v_names->iterator = jt->_attr_v_names;
	current_attr_value = jt->_attr_v_values;
      }
      else{
	//printf("A vector attribute names list is already defined, looking for %s...\n",name);

        //printf("current attr_v_name is %s\n",current_attr_name->attr_name);

	while (current_attr_name->next && strcmp(current_attr_name->attr_name,name)){
	  current_attr_name=current_attr_name->next;
	  current_attr_value++; // current_attr_value is an array of drmaa_attr_value lists
          //printf("current attr_v_name is %s\n",current_attr_name->attr_name);     
	}
	if (current_attr_name->attr_name != NULL && !strcmp(current_attr_name->attr_name,name)){
	  //value is modified, erase the list
          //printf("A value list is already defined for %s... Clean it\n",name);
	  if ((*current_attr_value) && (*current_attr_value)->attr_value != NULL){
		c=(*current_attr_value);
		while (c){
		      //printf("Cleaning %s\n",c->attr_value);
		      prec_attr_value = c;
		      c=c->next;
		      free(prec_attr_value->attr_value);
		      free(prec_attr_value);
	     	}
	   }
	}
	else{
          //printf("No value yet for %s, adding a new value list\n",name);
	  //value is added at the end of the vector
	  if ((current_attr_name->next = (drmaa_attr_names_t*) calloc(1,sizeof (drmaa_attr_names_t)))==NULL)
	    return DRMAA_ERRNO_NO_MEMORY;
	  if ((current_attr_name->next->attr_name = (char*)calloc(1,(strlen(name)+1)*sizeof(char)))==NULL)
	    return DRMAA_ERRNO_NO_MEMORY;
	  strncpy(current_attr_name->next->attr_name,name,strlen(name)+1);
	  current_attr_name->next->next = NULL;
	}
      }
      //printf("Name %s exists but has no value (not yet filled or just cleaned), adding a new value list\n",name);
      // first
      if ((*current_attr_value = (drmaa_attr_values_t*)calloc(1,sizeof(drmaa_attr_values_t)))==NULL)
        return DRMAA_ERRNO_NO_MEMORY;
      c = *current_attr_value;
      if ((c->attr_value = (char*) calloc(1,(strlen(value[0])+1)*sizeof(char)))==NULL)
        return DRMAA_ERRNO_NO_MEMORY;
      c->next = NULL;
      strncpy(c->attr_value,value[0],strlen(value[0])+1);
      //printf("Added value %s\n",c->attr_value);
      prec_attr_value = c;
      for ( i = 1 ; i < count ; i++ ){
	if ((c=(drmaa_attr_values_t*)calloc(1,sizeof(drmaa_attr_values_t)))==NULL)
	  return DRMAA_ERRNO_NO_MEMORY;
	if ((c->attr_value = (char*) calloc(1,(strlen(value[i])+1)*sizeof(char)))==NULL)
	  return DRMAA_ERRNO_NO_MEMORY;
	c->next = NULL;
	prec_attr_value->next = c;
	strncpy(c->attr_value,value[i],strlen(value[i])+1);
	//printf("Added value %s\n",c->attr_value);
	prec_attr_value = c;
      }
      return DRMAA_ERRNO_SUCCESS;
    }
  else return DRMAA_ERRNO_INVALID_ARGUMENT;
}

/*
 * If 'name' is an existing vector attribute name in the job template 'jt',
 * then the values of 'name' are returned; otherwise, NULL is returned.
 */
int drmaa_get_vector_attribute(drmaa_job_template_t *jt, const char *name, drmaa_attr_values_t **values, char *error_diagnosis, size_t error_diag_len)
{
  //printf("\n\nIn drmaa_get_vector_attribute\n\n\n");

  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;
  if (strcmp(DRMAA_V_ARGV,name) && strcmp(DRMAA_V_ENV,name) && strcmp(DRMAA_V_EMAIL,name)){
    fprintf(stderr,"arg invalide\n");
    return DRMAA_ERRNO_INVALID_ARGUMENT;
  }
  drmaa_attr_names_t* current_attr_name = jt->_attr_v_names;
  drmaa_attr_values_t** current_attr_value = jt->_attr_v_values, *ref, *c,*p;
  if (!jt->_attr_v_values){
    //printf("jt->_attr_v_values is null\n");
    values = NULL;
    return DRMAA_ERRNO_SUCCESS;
  }
  //printf ("current_attr_name->attr_name = %s\n",current_attr_name->attr_name);
  while (current_attr_name->attr_name && strcmp(current_attr_name->attr_name,name)){
    //printf ("current_attr_name->attr_name = %s\n",current_attr_name->attr_name);
    current_attr_name=current_attr_name->next;
    current_attr_value++;
  }
  if (!strcmp(current_attr_name->attr_name,name)){
    //printf("found %s, copying values to \"values\": ",name);
    ref = (*current_attr_value);
    if (ref->attr_value){
      // first
      if ((*values = (drmaa_attr_values_t*) calloc(1,sizeof(drmaa_attr_values_t)))==NULL)
        return DRMAA_ERRNO_NO_MEMORY;
      c = *values;
      if ((c->attr_value = (char*) calloc(1,(strlen((*current_attr_value)->attr_value)+1)*sizeof(char)))==NULL)
        return DRMAA_ERRNO_NO_MEMORY;
      strncpy(c->attr_value,(*current_attr_value)->attr_value,strlen((*current_attr_value)->attr_value)+1);
      //printf("%s; ",c->attr_value);
      c->next = NULL;
      c->iterator = c;
      p = c;
      ref=ref->next;
      while (ref && ref->attr_value){
	if ((c = (drmaa_attr_values_t*) calloc(1,sizeof(drmaa_attr_values_t)))==NULL)
	  return DRMAA_ERRNO_NO_MEMORY;
	if ((c->attr_value = (char*) calloc(1,(strlen(ref->attr_value)+1)*sizeof(char)))==NULL)
	  return DRMAA_ERRNO_NO_MEMORY;
	strncpy(c->attr_value,ref->attr_value,strlen(ref->attr_value)+1);
	c->next = NULL;
        //printf("%s; ",c->attr_value);
	p->next = c;
	p = c;
	ref = ref->next;
      }
    }
    //printf("\n");
  }
  else{
    //printf("jt->_attr_v_values[%s] is null\n",name);
    values = NULL;
  }
  return DRMAA_ERRNO_SUCCESS;
}




/*
 * Returns the set of supported attribute names whose associated
 * value type is String. This set will include supported DRMAA reserved
 * attribute names and native attribute names.
 */
int drmaa_get_attribute_names( drmaa_attr_names_t **values, char *error_diagnosis, size_t error_diag_len)
{
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;
  drmaa_attr_names_t* current_attr_name = _supported_attr_names;
  if (((*values) = (drmaa_attr_names_t*) calloc(1,sizeof(drmaa_attr_names_t)))==NULL)
    return DRMAA_ERRNO_NO_MEMORY;
  //fprintf(stderr,"Getting names\n");
  while (current_attr_name != NULL){
    if (current_attr_name->attr_name!=NULL){
      //printf ("name=%s\n",current_attr_name->attr_name);
    }
    else{
      fprintf(stderr,"name is null, error\n");
    }
    add_attr_name(*values,current_attr_name->attr_name);
    current_attr_name=current_attr_name->next;
  }
  return DRMAA_ERRNO_SUCCESS;
}

/*
 * Returns the set of supported attribute names whose associated
 * value type is String Vector.  This set will include supported DRMAA reserved
 * attribute names and native attribute names.
 */
int drmaa_get_vector_attribute_names(drmaa_attr_names_t **values, char *error_diagnosis, size_t error_diag_len)
{
  //should copy the whole thing
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;
  // test
  /*printf("_supported_attr_v_names (%p -> %p) = ", &_supported_attr_v_names,_supported_attr_v_names );
  drmaa_attr_names_t* current = _supported_attr_v_names->iterator;
  while (current){
	//printf("%s ; ",current->attr_name);
	current = current->next;
  }
  //printf("\n");*/

  (*values) = _supported_attr_v_names;
  return DRMAA_ERRNO_SUCCESS;
}


int addAttrop(struct attropl** target, char* name, char* value){
  //printf("In addatrop, target = %p, name = %s, value = %s\n",*target,name,value);
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;
  struct attropl* current = NULL;
  if (*target == NULL){
    //fprintf(stderr,"target == NULL\n");
    if ((*target = (struct attropl*)calloc(1,sizeof(struct attropl)))==NULL)
      return DRMAA_ERRNO_NO_MEMORY;
    current = *target;
  }
  else{
    //fprintf(stderr,"target != NULL\n");
    current = *target;
    struct attropl* prev = NULL;
    //seek the last entry
    while (current ){
      prev = current;
      current=current->next;
    }
    //fprintf(stderr,"allocating current\n");
    fflush(stdout);
    if ((current = (struct attropl*)calloc(1,sizeof(struct attropl)))==NULL){
      return DRMAA_ERRNO_NO_MEMORY;
    }
    //fprintf(stderr,"setting prev->next = current\n");
    fflush(stdout);
    prev->next = current;
  }
  //fprintf(stderr,"Allocating ressources for name and value\n");
  if ((current->name = (char*)calloc(1,(strlen(name)+1)*sizeof(char)))==NULL)
    return DRMAA_ERRNO_NO_MEMORY;
  if ((current->value = (char*)calloc(1,(strlen(value)+1)*sizeof(char)))==NULL)
    return DRMAA_ERRNO_NO_MEMORY;
  //fprintf(stderr,"Copying name and value\n");
  strncpy(current->name,name,strlen(name)+1);
  strncpy(current->value,value,strlen(value)+1);
  //fprintf(stderr,"setting current->next=NULL (normally useless as calloc was used\n");
  current->next = NULL;
  //fprintf(stderr,"Done\n");
  return DRMAA_ERRNO_SUCCESS;
}

int addAttrop_v(struct attropl** target, char* name, drmaa_attr_values_t** value){
  //fprintf(stderr,"addAttrop_v (%p,%s, %s)\n",target,name,(*value)->attr_value);
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;
  if (!*value){
    fprintf(stderr,"Error, *value == NULL\n");
    return DRMAA_ERRNO_INVALID_ARGUMENT;
  }

  if (!*target)
    if ((*target = (struct attropl*)calloc(1,sizeof(struct attropl)))==NULL)
      return DRMAA_ERRNO_NO_MEMORY;   
  struct attropl* current = *target;
  struct attropl* prev = NULL;
  int i,count=0;
  drmaa_attr_values_t* values = *value;
  int len = 0;
  while (values){
    //fprintf(stderr,"value = %s\nlen(value) = %d\n",values->attr_value,strlen(values->attr_value)+1);
    len+=(strlen(values->attr_value)+1);
    //fprintf(stderr,"total length = %d\n",len);
    count++;
    values=values->next;
  }
  current = *target;
  while (current){
    prev = current;
    current = current->next;
  }
  values = *value;
  char *array;
  if (( array = (char*)calloc(len+1,sizeof(char)))==NULL)
    return DRMAA_ERRNO_NO_MEMORY;
  //printf("%d bytes alloc'd, copying %d chars of %s in it\n",len,strlen(values->attr_value)+1,values->attr_value);
  strncpy(array,values->attr_value, strlen(values->attr_value)+1);
  strcat(array,",");
  //fprintf(stderr,"0 : %s\n",array);
  values = values->next;
  for (i = 1 ; i < count ; ++i){
    //fprintf(stderr,"adding %d chars of %s\n",strlen(values->attr_value), values->attr_value);
    strcat(array,(values->attr_value));
    strcat(array,",");
    //fprintf(stderr,"%d : %s\n",i,array);
    values = values->next;
  }
  //replace last ',' by '\0' to finish the string
  array[len-1]='\0';
  //fprintf(stderr,"finally : %s\n",array);

  if ((current = (struct attropl*)calloc(1,sizeof(struct attropl)))==NULL)
    return DRMAA_ERRNO_NO_MEMORY;
  if (prev)
    prev->next = current;
  if ((current->name = (char*)calloc(1,(strlen(name)+1)*sizeof(char)))==NULL)
    return DRMAA_ERRNO_NO_MEMORY;
  strncpy(current->name,name,strlen(name)+1);
  current->value = array;
  //fprintf(stderr,"value = %s\n",current->value);
  return DRMAA_ERRNO_SUCCESS;
}


/* ------------------- job submission routines ------------------- */

/*
 * Submit a job with attributes defined in the job template 'jt'.
 * The job identifier 'job_id' is a printable, NULL terminated string,
 * identical to that returned by the underlying DRM system.
 */
int drmaa_run_job(char *job_id, size_t job_id_len, drmaa_job_template_t *jt, char *error_diagnosis, size_t error_diag_len)
{
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;
  /*
   *  binding DRMAA-PBS
   */

  char* pbs_native_attribs[] = {ATTR_l,ATTR_p,ATTR_q,ATTR_c,ATTR_k,ATTR_r,ATTR_u,ATTR_v,ATTR_stagein,ATTR_stageout,
				ATTR_exechost,ATTR_comment,ATTR_cookie,NULL};

  //fprintf(stderr,"Entering drmaa_run_job\n");
  struct attropl *attroplist = NULL;
  drmaa_attr_names_t* attr_names = jt->_attr_names;
  drmaa_attr_names_t* attr_v_names = jt->_attr_v_names;
  drmaa_attr_values_t* attr_values = jt->_attr_values;
  drmaa_attr_values_t* attr_v_values;
  char *script;
  char *new_param_hd, *new_param;

  //defaults
  addAttrop (&attroplist,ATTR_c,CHECKPOINT_UNSPECIFIED);
  //fprintf(stderr,"Defining default values2\n");
  addAttrop (&attroplist,ATTR_h,NO_HOLD);
  //fprintf(stderr,"Defining default values3\n");
  addAttrop (&attroplist,ATTR_j,NO_JOIN);
  //fprintf(stderr,"Defining default values4\n");
  addAttrop (&attroplist,ATTR_k,NO_KEEP);
  //  fprintf(stderr,"Defining default values5\n");
  addAttrop (&attroplist,ATTR_m,MAIL_AT_ABORT);
  //  fprintf(stderr,"Defining default values6\n");
  addAttrop (&attroplist,ATTR_p,"0");
  //  fprintf(stderr,"Defining default values7\n");
  addAttrop (&attroplist,ATTR_r,"TRUE");

  //fprintf(stderr,"In drmaa_run_job: defining pbs job\n");
  while (attr_names){
    // add the correct pbs attrop
    //fprintf(stderr,"handling %s\n",attr_names->attr_name);
    // replace placeholders for home and execution dir
    new_param_hd = replacePattern(attr_values->attr_value,DRMAA_PLACEHOLDER_HD,getenv("HOME"));
    if (!new_param_hd)
      return DRMAA_ERRNO_NO_MEMORY;
    new_param = replacePattern(new_param_hd,DRMAA_PLACEHOLDER_WD,getenv("PWD"));
    if (new_param_hd)
      free(new_param_hd);
    if (!new_param)
      return DRMAA_ERRNO_NO_MEMORY;    
    
    if (!strcmp(DRMAA_REMOTE_COMMAND,attr_names->attr_name)){
      //addAttrop (&attroplist,ATTR_N,new_param);
      script=strdup(new_param);
      //fprintf(stderr,"script: %s\n",script);
      // todo : verify the script presence
    }
    else if (!strcmp(DRMAA_JS_STATE,attr_names->attr_name)){
	//printf("DRMAA_JS_STATE, new_param = %s\n",new_param);
      if (!strcmp(new_param,DRMAA_SUBMISSION_STATE_HOLD))
	addAttrop (&attroplist,ATTR_h,"u");
    }
    else if (!strcmp(DRMAA_WD,attr_names->attr_name)){
    }
    else if (!strcmp(DRMAA_JOB_CATEGORY,attr_names->attr_name)){
    }
    else if (!strcmp(DRMAA_NATIVE_SPECIFICATION,attr_names->attr_name)){

      // handle different properties, parse attr_values->attr_value, looks like "attrib=value"
  
      //printf("in run_job: DRMAA_NATIVE_SPECIFICATION, attr_name = %s\nattr_value = %s\n",attr_names->attr_name,attr_values->attr_value);

      // we'll use ';' as a separator for different native spec, for example "Resource_List=nodes=3;Rerunable=true"
      // split the attribute value for ';'
      
      char* values_parser = attr_values->attr_value;
      char* current_value;
      int i;

      while ( index (values_parser,';') ){
        size_t len = index(values_parser,';') - values_parser;
	current_value = malloc(len*sizeof(char));
	strncpy(current_value,values_parser,len);
	//printf("extracted %s from %s\n",current_value, values_parser);
	

        i = 0;
	int found = 0;
      	while (pbs_native_attribs[i] != NULL && !found){
	  char* native_attr = pbs_native_attribs[i];
	  if (!strncasecmp(current_value,native_attr,min(strlen(current_value),strlen(native_attr)))){
	     //printf("\"%s\" found, adding %s to the pbs property\n",native_attr,strchr(current_value,'=')+1);
	     found = 1;
	     if (strchr(current_value,'='))
	        addAttrop (&attroplist,native_attr,strchr(current_value,'=')+1);
	  } 
	  i++;
        }
	if (!found)
	   printf("Error, %s is not a valid/implemented property\n",current_value);
	free(current_value);
	current_value=NULL;
	values_parser = index (values_parser,';');
	// if values_parser is not NULL, increment it so as to skip the ';' char
	if (values_parser){
	   values_parser++;
	   //printf("values_parser = %s\n",values_parser);
	}
      }
      // last element
      current_value = values_parser;
      i = 0;

      while (pbs_native_attribs[i] != NULL){
        char* native_attr = pbs_native_attribs[i];
	if (!strncasecmp(current_value,native_attr,min(strlen(current_value),strlen(native_attr)))){
             //printf("\"%s\" found, adding %s to the pbs property\n",native_attr,strchr(current_value,'=')+1);
             if (strchr(current_value,'='))
                addAttrop (&attroplist,native_attr,strchr(current_value,'=')+1);
        }
        i++;
      }


    }
    else if (!strcmp(DRMAA_BLOCK_EMAIL,attr_names->attr_name)){
    }
    else if (!strcmp(DRMAA_START_TIME,attr_names->attr_name)){
      addAttrop (&attroplist,ATTR_a,new_param);
    }
    else if (!strcmp(DRMAA_JOB_NAME,attr_names->attr_name)){
      addAttrop (&attroplist,ATTR_N,new_param);
    }
    else if (!strcmp(DRMAA_INPUT_PATH,attr_names->attr_name)){
    }
    else if (!strcmp(DRMAA_OUTPUT_PATH,attr_names->attr_name)){
      addAttrop (&attroplist,ATTR_o,new_param);
    }
    else if (!strcmp(DRMAA_ERROR_PATH,attr_names->attr_name)){
      addAttrop (&attroplist,ATTR_e,new_param);
    }
    else if (!strcmp(DRMAA_JOIN_FILES,attr_names->attr_name)){
      addAttrop (&attroplist,ATTR_j,new_param);
    }
    else if (!strcmp(DRMAA_TRANSFER_FILES,attr_names->attr_name)){
      //addAttrop (&attroplist,ATTR_j,new_param);
    }
    else if (!strcmp(DRMAA_DEADLINE_TIME,attr_names->attr_name)){
    }
    else if (!strcmp(DRMAA_WCT_HLIMIT,attr_names->attr_name)){
    }
    else if (!strcmp(DRMAA_WCT_SLIMIT,attr_names->attr_name)){
    }
    else if (!strcmp(DRMAA_DURATION_HLIMIT,attr_names->attr_name)){
    }
    else if(!strcmp(DRMAA_DURATION_SLIMIT,attr_names->attr_name)){
    }
    attr_names = attr_names->next;
    attr_values = attr_values->next;
    if (new_param){
      free(new_param);
      new_param=NULL;
    }
    
  }
  //fprintf(stderr,"handling attr_v\n");
  int val = 0;
  while (attr_v_names){
    attr_v_values = jt->_attr_v_values[val];
    if (!strcmp(DRMAA_V_ARGV,attr_v_names->attr_name)){
      //addAttrop_v (&attroplist,ATTR_v,attr_v_values);
    }
    else if (!strcmp(DRMAA_V_ENV,attr_v_names->attr_name)){
      //fprintf(stderr,"found DRMAA_V_ENV, adding it to the attr_v\n");
      addAttrop_v (&attroplist,ATTR_v,&attr_v_values);
    }
    else if (!strcmp(DRMAA_V_EMAIL,attr_v_names->attr_name)){
      addAttrop_v (&attroplist,ATTR_M,&attr_v_values);
      //fprintf(stderr,"found DRMAA_V_EMAIL, adding it to the attr_v\n");
    }
    attr_v_names = attr_v_names->next;
    //*attr_v_values = (*attr_v_values)->next;
    val++;
  }
  //fprintf(stderr,"job ready\n");
  char destination[30]; /* FORMAT:  <QUEUE>@<HOST>:<PORT> */
  //fprintf(stderr,"In drmaa_run_job, submitting job\n");
  //fprintf(stderr,"\t_connect = %d\n\tattroplist=%p\n\tscrit=%s\n",_connect,&attroplist,script);
  
  /**
   * debug
   */
  //{
    //struct attropl* currentAttropl = attroplist;
    //while(currentAttropl){
      //fprintf(stderr,"\tname:%s\t",currentAttropl->name);
      //fprintf(stderr,"\tvalue:%s\n",currentAttropl->value);
      //fflush(stdout);
      //currentAttropl = currentAttropl->next;
    //}
  //}
  /***************************************************/

  fprintf(stderr,"Submitting\n");
  char* id=NULL;
  
  //fprintf(stderr,"id defined as NULL\n");
  pthread_mutex_lock(&_submit_lock);
  id = pbs_submit(_connect, attroplist, script, NULL, NULL);
  fprintf(stderr,"submit done\n");
  fprintf(stderr,"id = %s\n",id);
  pthread_mutex_unlock(&_submit_lock);

  /**************************************************
   *  clean attroplist
   *************************************************/
  struct attropl* currentAttropl = attroplist;
  while(attroplist){
    attroplist = attroplist->next;
    free(currentAttropl->value);
    free(currentAttropl->name);
    free(currentAttropl);
    currentAttropl = attroplist;
  }
  
  if (script){
    free(script);
    script = NULL;
  }

  /*************************************************/

  if (!id){
    char* msg = pbs_geterrmsg(_connect);
    if (!msg)
      msg = strdup("No error message\n");
    
    strncpy(error_diagnosis,msg,error_diag_len);
    fprintf(stderr,"Error: %s\n", error_diagnosis);
    free (msg);
    // we don't know that for sure
    return DRMAA_ERRNO_DENIED_BY_DRM;
  }
  else{
    //printf("job submitted: ID=%s\n",id);
    strncpy(job_id,id,job_id_len);
    //printf("adding to the list\n");
    if(id){
      free(id);
      id=NULL;
    }
    if (!job_list){
      pthread_mutex_lock(&_job_list_lock);
      //printf("job list is empty, allocating memory\n");
      if ((job_list = (struct job_list_s*)calloc(1,sizeof(struct job_list_s)))==NULL)
        return DRMAA_ERRNO_NO_MEMORY;
      job_list->job = jt;
      job_list->job_id = (char*)strdup(job_id);
      job_list->next = NULL;
      pthread_mutex_unlock(&_job_list_lock);
    }
    else{
      struct job_list_s* current = job_list;
      while(current->next){
    	current = current->next;
      }
      pthread_mutex_lock(&_job_list_lock);
      if ((current->next = (struct job_list_s*)calloc(1,sizeof(struct job_list_s)))==NULL)
        return DRMAA_ERRNO_NO_MEMORY;
      current->next->job_id = (char*)strdup(job_id);
      current->next->job = jt;
      current->next->next = NULL;
      pthread_mutex_unlock(&_job_list_lock);
    }
    //printf("return\n");
    return DRMAA_ERRNO_SUCCESS;
  }
}

/*
 * Submit a set of parametric jobs, dependent on the implied loop index, each
 * with attributes defined in the job template 'jt'.
 * The job identifiers 'job_ids' are all printable,
 * NULL terminated strings, identical to those returned by the underlying
 * DRM system. Nonnegative loop bounds are mandated to avoid file names
 * that start with minus sign like command line options.
 * The special index placeholder is a DRMAA defined string
 * drmaa_incr_ph == $incr_pl$
 * that is used to construct parametric job templates.
 * For example:
 * drmaa_set_attribute(pjt, "stderr", drmaa_incr_ph + ".err" ); (C++/java string syntax used)
 */
int drmaa_run_bulk_jobs( drmaa_job_ids_t **jobids, drmaa_job_template_t *jt, int start, int end, int incr, char *error_diagnosis, size_t error_diag_len)
{
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;
  //  fprintf(stderr,"In libdrmaa: drmaa_run_bulk_jobs (jobids, template, %d, %d, %d, diag, len)\n",start,end,incr);

  if (incr < 1 || start < 1 || end > 2147483647 || end < start) 
    return DRMAA_ERRNO_CONFLICTING_ATTRIBUTE_VALUES;

  //  fprintf(stderr,"start, end, incr, and job template OK\n");

  if ( !jt ){
    //printf("jt is null!\n");
    return DRMAA_ERRNO_INVALID_ARGUMENT;
  }
  else if (!(jt->_attr_names) || !(jt->_attr_values) ){
    //printf("!(jt->_attr_names) || !(jt->_attr_values)\n");
    return DRMAA_ERRNO_INVALID_ARGUMENT;
  }

  int res;
  char current_job_id[DRMAA_JOBNAME_BUFFER];

  /*
   * look for attributes containing the "drmaa_inc_ph", then put them appart to give a parametric property (in param_names and param_values lists)
   */
  drmaa_attr_names_t *names_init = jt->_attr_names, *param_names=NULL,*param_names_current=NULL;
  drmaa_attr_values_t* values_init = jt->_attr_values, *param_values=NULL, *param_values_current=NULL;
  //  fprintf(stderr,"Searching parametrics properties in the job template...\n");
  while (names_init){
    if (strstr(values_init->attr_value,DRMAA_PLACEHOLDER_INCR)){
      //printf("%s = %s contains the DRMAA_PLACEHOLDER_INCR value, adding it to the list\n",names_init->attr_name, values_init->attr_value);
      // a parametric property has been found
      if (!param_names){
	//	printf("First element of the list..\n");
	if ((param_names = (drmaa_attr_names_t *)calloc(1,sizeof(drmaa_attr_names_t)))==NULL)
	  return DRMAA_ERRNO_NO_MEMORY;
	if ((param_names->attr_name = (char*)calloc(1,(strlen(names_init->attr_name)+1)*sizeof(char)))==NULL)
	  return DRMAA_ERRNO_NO_MEMORY;
	strncpy(param_names->attr_name,names_init->attr_name,strlen(names_init->attr_name)+1);
	param_names->next = NULL;
	param_names_current = param_names;

	if ((param_values = (drmaa_attr_values_t *)calloc(1,sizeof(drmaa_attr_values_t)))==NULL)
	  return DRMAA_ERRNO_NO_MEMORY;
	if ((param_values->attr_value = (char*)calloc(1,(strlen(values_init->attr_value)+1)*sizeof(char)))==NULL)
	  return DRMAA_ERRNO_NO_MEMORY;
	strncpy(param_values->attr_value,values_init->attr_value,strlen(values_init->attr_value)+1);
	param_values->next = NULL;
	param_values_current = param_values;
      }
      else{
	//	printf("The list is not empty...\n");
	if ((param_names_current->next = (drmaa_attr_names_t *)calloc(1,sizeof(drmaa_attr_names_t)))==NULL)
	  return DRMAA_ERRNO_NO_MEMORY;
	if((param_names_current->next->attr_name = (char*)calloc(1,(strlen(names_init->attr_name)+1)*sizeof(char)))==NULL)
	  return DRMAA_ERRNO_NO_MEMORY;
	strncpy(param_names_current->next->attr_name,names_init->attr_name,strlen(names_init->attr_name)+1);
	param_names_current->next->next = NULL;
	param_names_current=param_names_current->next;

	if((param_values_current->next = (drmaa_attr_values_t *)calloc(1,sizeof(drmaa_attr_values_t)))==NULL)
	  return DRMAA_ERRNO_NO_MEMORY;
	if((param_values_current->next->attr_value = (char*)calloc(1,(strlen(values_init->attr_value)+1)*sizeof(char)))==NULL)
	  return DRMAA_ERRNO_NO_MEMORY;
	strncpy(param_values->next->attr_value,values_init->attr_value,strlen(values_init->attr_value)+1);
	param_values_current->next->next = NULL;
	param_values_current=param_values_current->next;
      }
    }
    
    names_init=names_init->next;
    values_init=values_init->next;
  }
  /*
   *  now param_names contains parametric properties. If non null, we can update the job property for each iteration
   */
  // fprintf(stderr,"Launching jobs\n");
  int i = start;
  if ((*jobids = (drmaa_job_ids_t *)calloc(1,sizeof(drmaa_job_ids_t)))==NULL)
    return DRMAA_ERRNO_NO_MEMORY;

  
  /****************************************************************
   * loop through the jobs, replacing its attribute present in the
   * param list by the one with DRMAA_PLACEHOLDER_INCR replaced by
   * its value
   ***************************************************************/

  char *new_param_value=NULL;
  drmaa_job_ids_t* currentJob = *jobids;
  char replacement[100];
  if ((currentJob->job_ids = (char*)calloc(1,DRMAA_JOBNAME_BUFFER*sizeof(char)))==NULL)
    return DRMAA_ERRNO_NO_MEMORY;
  currentJob->next = NULL;
  //printf("run_bulk_jobs follows\n");

  sprintf(replacement,"%d",i);

  //for each param, replace DRMAA_PLACEHOLDER_INCR by i
  param_names_current = param_names;
  param_values_current = param_values;
  while (param_names_current){
    if (new_param_value){
      free(new_param_value);
      new_param_value=NULL;
    }
    new_param_value = replacePattern(param_values_current->attr_value,DRMAA_PLACEHOLDER_INCR,replacement);
    drmaa_set_attribute(jt,param_names_current->attr_name,new_param_value,error_diagnosis,error_diag_len);
    param_names_current = param_names_current->next;
    param_values_current = param_values_current->next;
  }
  if (new_param_value){
    free(new_param_value);
    new_param_value=NULL;
  }

  res = drmaa_run_job(current_job_id, DRMAA_JOBNAME_BUFFER, jt, error_diagnosis, error_diag_len);
  if (res != DRMAA_ERRNO_SUCCESS){
    fprintf(stderr,"Error, %s\n",error_diagnosis);
    return res;
  }
  strncpy(currentJob->job_ids,current_job_id,DRMAA_JOBNAME_BUFFER);
  currentJob->iterator = currentJob;
  i+=incr;

  while(i <= end){
    sprintf(replacement,"%d",i);   
    if ((currentJob->next = (drmaa_job_ids_t *)calloc(1,sizeof(drmaa_job_ids_t)))==NULL)
      return DRMAA_ERRNO_NO_MEMORY;
    currentJob->next->next = NULL;
    //printf("In drmaa_run_bulk_jobs, running job %d\n",i);

    /******************************************************************************
     * for each param, replace DRMAA_PLACEHOLDER_INCR by i
     ******************************************************************************/

    // place param_(names/values)_current on the first element
    param_names_current = param_names;
    param_values_current = param_values;
    // for each element call replacePattern, replacing DRMAA_PLACEHOLDER_INCR by the incr value (as string)
    while (param_names_current){
      if (new_param_value)
	free(new_param_value);
      new_param_value = replacePattern(param_values_current->attr_value,DRMAA_PLACEHOLDER_INCR,replacement);
      // set the current_job_attribute to the new value (erasing the previous one)
      drmaa_set_attribute(jt,param_names_current->attr_name,new_param_value,error_diagnosis,error_diag_len);
      // go for the next param_(name/value)
      param_names_current = param_names_current->next;
      param_values_current = param_values_current->next;
    }
    if (new_param_value)
      free(new_param_value);
    // run the job

    //printf("run the job %d\n",i);
    res = drmaa_run_job(current_job_id, DRMAA_JOBNAME_BUFFER, jt, error_diagnosis, error_diag_len);
    //printf("job %d submitted\n",i);
    if (res != DRMAA_ERRNO_SUCCESS){
      fprintf(stderr,"Error, %s\n",error_diagnosis);
      return res;
    }
    // and add it to the job_ids list
    if ((currentJob->next->job_ids = (char*)calloc(1,DRMAA_JOBNAME_BUFFER*sizeof(char)))==NULL)
      return DRMAA_ERRNO_NO_MEMORY;
    strncpy(currentJob->next->job_ids,current_job_id,DRMAA_JOBNAME_BUFFER);
    i+=incr;
    // go to next job
    currentJob = currentJob->next;
  }
  
  //printf ("Finished submitting jobs\n");

  /*
   *  free memory
   */
  param_names_current = param_names;
  param_values_current = param_values;
  //printf("Begining free\n");

  while (param_names_current){
    //printf("freeing param : %s = %s\n",param_names_current->attr_name,param_values_current->attr_value);
    param_names = param_names_current->next;
    param_values = param_values_current->next;
    free (param_names_current->attr_name);
    free (param_values_current->attr_value);
    free (param_names_current);
    free (param_values_current);
    param_names_current = param_names;
    param_values_current = param_values;
    //printf("freed\n");
  }
  param_names = NULL;
  param_values = NULL;
  
  //printf("Memory freed\n");
  return DRMAA_ERRNO_SUCCESS;
}

/* ------------------- job control routines ------------------- */

/*
 * Start, stop, restart, or kill the job identified by 'job_id'.
 * If 'job_id' is DRMAA_JOB_IDS_SESSION_ALL, then this routine
 * acts on all jobs *submitted* during this DRMAA session.
 * The legal values for 'action' and their meanings are:
 * DRMAA_CONTROL_SUSPEND: stop the job,
 * DRMAA_CONTROL_RESUME: (re)start the job,
 * DRMAA_CONTROL_HOLD: put the job on-hold,
 * DRMAA_CONTROL_RELEASE: release the hold on the job, and
 * DRMAA_CONTROL_TERMINATE: kill the job.
 * This routine returns once the action has been acknowledged by
 * the DRM system, but does not necessarily wait until the action
 * has been completed.
 */
int drmaa_control(const char *jobid, int action, char *error_diagnosis, size_t error_diag_len)
{
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;
  /*
   * Have to indicate a default DRMAA queue
   */
  char* job_id_cpy = (char*)strdup(jobid);

  int (*function)(int,char *,char *,char *)=NULL;
  char arg3[100];
  arg3[0]='\0';

  switch (action){
  case DRMAA_CONTROL_SUSPEND:
    free (job_id_cpy);
    job_id_cpy=NULL;
    //return DRMAA_ERRNO_SUSPEND_INCONSISTENT_STATE;
    break;
  case DRMAA_CONTROL_RESUME:
    if (strcmp(jobid,DRMAA_JOB_IDS_SESSION_ALL))
	pbs_runjob(_connect, job_id_cpy,NULL,NULL);
    else
	function = &pbs_runjob;   
    break;
  case DRMAA_CONTROL_HOLD:
    if (strcmp(jobid,DRMAA_JOB_IDS_SESSION_ALL))
	pbs_holdjob (_connect, job_id_cpy, USER_HOLD, NULL);
    else{
	function = &pbs_holdjob;
	strcpy(arg3,USER_HOLD);
    }
    break;
  case DRMAA_CONTROL_RELEASE:
    if (strcmp(jobid,DRMAA_JOB_IDS_SESSION_ALL))
        pbs_rlsjob(_connect, job_id_cpy, USER_HOLD, NULL);
    else{
	//printf("In drmaa_control, jobid is %s, setting function to pbs_rljob\n",jobid);
        function = &pbs_rlsjob;
        strcpy(arg3,USER_HOLD);
    }
    break;
  case DRMAA_CONTROL_TERMINATE:
    if (!strcmp(jobid,DRMAA_JOB_IDS_SESSION_ALL)){
        //printf("terminate all jobs in the session\n");
        struct job_list_s* cur = job_list, *prec=NULL;
        while (cur){
            //printf("deleting job %s\n",cur->job_id);
            pbs_deljob(_connect,cur->job_id,NULL);
            cur = cur->next;
        }
    }
    else{
      pbs_deljob(_connect,job_id_cpy,NULL);
    }
    break;
  }

  // do the job

  if (function){
     struct job_list_s* cur = job_list;
     while (cur){
        function(_connect,cur->job_id,arg3,NULL);
        cur = cur->next;
     }
  }

  if (job_id_cpy)
    free(job_id_cpy);
  if (errno){
    //printf("In drmaa_control(%s,%d), error: %s\n",jobid, action, pbs_geterrmsg(_connect)); 
    strncpy(error_diagnosis,pbs_geterrmsg(_connect),error_diag_len);
    return DRMAA_ERRNO_INTERNAL_ERROR;
  }
  return DRMAA_ERRNO_SUCCESS;
  
}


/*
 * Wait until all jobs specified by 'job_ids' have finished
 * execution. If 'job_ids' is DRMAA_JOB_IDS_SESSION_ALL, then this routine
 * waits for all jobs *submitted* during this DRMAA session. To prevent
 * blocking indefinitely in this call the caller could use timeout specifying
 * after how many seconds to time out in this call.
 * If the call exits before timeout all the jobs have been waited on
 * or there was an interrupt.
 * If the invocation exits on timeout, the return code is DRMAA_ERRNO_EXIT_TIMEOUT.
 * The caller should check system time before and after this call
 * in order to check how much time has passed.
 * Dispose parameter specifies how to treat reaping information:
 * True=1 "fake reap", i.e. dispose of the rusage data
 * False=0 do not reap
 */
int drmaa_synchronize(const char *job_ids[], signed long timeout, int dispose, char *error_diagnosis, size_t error_diag_len)
{
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;
  char waitForAll = !strcmp(job_ids[0],DRMAA_JOB_IDS_SESSION_ALL);
  //printf("In drmaa_synchronize, waitForAll = %d\n",waitForAll);
  signed long callTime,returnTime;
  char id_out[DRMAA_JOBNAME_BUFFER];
  int status=-1;
  int ret;
  callTime = time(NULL);
  returnTime = callTime;
  if (timeout == DRMAA_TIMEOUT_NO_WAIT)
    timeout=0;

  if ( waitForAll ){
    //wait for all jobs
    //fprintf(stderr,"[drmaa_synchronize]Waiting for all jobs\n");
    struct job_list_s *current_job = job_list;
    while (current_job){
      callTime = time(NULL);
      //printf("[drmaa_synchronize]: now waiting for job %s\n",current_job->job_id);
      ret = drmaa_wait_job (current_job->job_id,id_out,DRMAA_JOBNAME_BUFFER,&status,timeout,NULL, dispose,error_diagnosis,error_diag_len);
      if (ret != DRMAA_ERRNO_SUCCESS){
	//printf("[drmaa_synchronize]drmaa_wait returned %d\n",ret);
	return ret;
      }
      returnTime = time(NULL);
      if (!dispose){
	// advance in the list
        //printf("[drmaa_synchronize]Job won\'t be reaped off, switching to the next job\n");
	current_job = current_job->next;
      }
      else{
	// the first job has been waited and reaped off, reset current_job to the first of the list
        //printf("[drmaa_synchronize]Job should now be reaped off, setting current_job to job_list again\n");
	current_job = job_list;
      }
      if ( timeout != DRMAA_TIMEOUT_WAIT_FOREVER ){
        timeout -= ( returnTime - callTime );
        if ( timeout <= 0 ){
          //printf ("[drmaa_synchronize]Timeout!\n");
          return DRMAA_ERRNO_EXIT_TIMEOUT;
        }
      }
    }
  }
  else{
    //fprintf(stderr,"drmaa_synchronize(): waiting for provided jobs:\n");
    //fprintf(stderr,"received job_ids = %p\n",job_ids);  
    int i = 0;
    while (job_ids[i]){
      //fprintf(stderr,"[drmaa_synchronize]job_ids[%d] = %s \n",i,job_ids[i]);
      i++;
    }
    //fprintf(stderr,"\n");
    i = 0;
    while (job_ids[i]){
      fprintf(stderr,"[drmaa_synchronize]waiting for job %s\n",job_ids[i]);
      /*if ( timeout != DRMAA_TIMEOUT_WAIT_FOREVER ){
	timeout -= ( returnTime - callTime );
	if ( timeout <= 0 ){
	  //printf ("[drmaa_synchronize]Timeout!\n");
	  return DRMAA_ERRNO_EXIT_TIMEOUT;
	}
      }*/
      callTime = time(NULL);
      ret = drmaa_wait_job (job_ids[i],id_out,DRMAA_JOBNAME_BUFFER,&status,timeout,NULL,dispose,error_diagnosis,error_diag_len);
      if (ret != DRMAA_ERRNO_SUCCESS)
	return ret;
      //fprintf(stderr,"[drmaa_synchronize]%s finished\n",job_ids[i]);
      returnTime = time(NULL);
      if ( timeout != DRMAA_TIMEOUT_WAIT_FOREVER ){
        timeout -= ( returnTime - callTime );
        if ( timeout <= 0 ){
          //printf ("[drmaa_synchronize]Timeout!\n");
          return DRMAA_ERRNO_EXIT_TIMEOUT;
        }
      }
      i++;
    }
  }
  return DRMAA_ERRNO_SUCCESS;
}



/*
 * This routine waits for a job with job_id to fail or finish execution. Passing a special string
 * DRMAA_JOB_IDS_SESSION_ANY instead job_id waits for any job. If such a job was
 * successfully waited its job_id is returned as a second parameter. This routine is
 * modeled on wait3 POSIX routine. To prevent
 * blocking indefinitely in this call the caller could use timeout specifying
 * after how many seconds to time out in this call.
 * If the call exits before timeout the job has been waited on
 * successfully or there was an interrupt.
 * If the invocation exits on timeout, the return code is DRMAA_ERRNO_EXIT_TIMEOUT.
 * The caller should check system time before and after this call
 * in order to check how much time has passed.
 * The routine reaps jobs on a successful call, so any subsequent calls
 * to drmaa_wait should fail returning an error DRMAA_ERRNO_INVALID_JOB meaning
 * that the job has been already reaped. This error is the same as if the job was
 * unknown. Failing due to an elapsed timeout has an effect that it is possible to
 * issue drmaa_wait multiple times for the same job_id.
 */

int drmaa_wait(const char *jobid, char *job_id_out, size_t job_id_out_len, int *stat,
               signed long timeout, drmaa_attr_values_t **rusage,
               char *error_diagnosis, size_t error_diagnosis_len){
	//kind of wrapper, just to allow a drmaa_wait function with a conditional reap of the job (useful for drmaa_synchronize)

	return drmaa_wait_job(jobid, job_id_out, job_id_out_len, stat, timeout, rusage, 1, error_diagnosis, error_diagnosis_len);
}


int drmaa_wait_job(const char *jobid, char *job_id_out, size_t job_id_out_len, int *stat,
	       signed long timeout, drmaa_attr_values_t **rusage, int dispose,
	       char *error_diagnosis, size_t error_diagnosis_len){
  if (_connect == -1){
    strncpy(error_diagnosis, "No active session", error_diagnosis_len);
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;
  }

  signed long deadline = time(NULL) + timeout;
  char *job_id = (char*)strdup(jobid);

  //fprintf(stderr,"in drmaa_wait: id %s provided\n",job_id);
  if (strcmp(job_id,DRMAA_JOB_IDS_SESSION_ANY)){
    // first verify if job is in current job_list:
    char found = 0;
    //printf("asking for job_list_lock\n");
    pthread_mutex_lock(&_job_list_lock);
    //printf("Got lock on job list\n");
    struct job_list_s* cur = job_list, *prec=NULL;
    while (!found && cur){
      //printf("Looking for %s, current job_id is %s\n",job_id,cur->job_id);
      if (!strcmp(cur->job_id,job_id)){
	found = 1;
	//printf("Found a match\n");
      }
      else{
	printf("getting next job from the list\n");
	prec = cur;
	cur = cur->next;
	//printf("The current job_id is now %s\n",cur->job_id);
      }
    }
    //printf("releasing job_list_lock\n");
    pthread_mutex_unlock(&_job_list_lock);
    if (!found){
      // job id is not in the list, return DRMAA_ERRNO_INVALID_JOB
      if(job_id){
	free(job_id);
	job_id=NULL;
      }
      //printf("Job not found\n");
	strncpy(error_diagnosis, "The job ", error_diagnosis_len);
      strncat(error_diagnosis, jobid, error_diagnosis_len-8);
      strncat(error_diagnosis, " is not queued/unknown.",error_diagnosis_len-8-strlen(jobid));
      return DRMAA_ERRNO_INVALID_JOB;
    }

    //fprintf(stderr,"[drmaa_wait]Waiting for job %s to finish\n",job_id);

    //wait for job job_id to finish for a maximum time of timeout
    char finished = 0;
    struct batch_status* state = NULL;
    #define STATUS_SIZE 2
    char status[STATUS_SIZE],formerStatus[STATUS_SIZE];
    formerStatus[0]='\0';
    status[0]='\0';

    //define a attribute list containing only job_state - that makes the server return only this attribute
    struct attrl* att;
    if ((att= (struct attrl*)calloc(1,sizeof (struct attrl)))==NULL)
      return DRMAA_ERRNO_NO_MEMORY;
    att->name="job_state";
    //fprintf(stderr,"[drmaa_wait]Asking pbs server for the job state\n");
    signed long endTime;
    do{
      //printf("asking for wait_lock\n");
      pthread_mutex_lock(&_wait_lock);
      //printf("got wait_lock\n");

      //printf("calling pbs_statjob(%d,%s,att,NULL)\n",_connect,job_id);
      state = pbs_statjob(_connect,(char*)job_id,att,NULL);
      //printf("server answered\n");
      pthread_mutex_unlock(&_wait_lock);
      if ( state == NULL ){
	//printf("[drmaa_wait]job state is NULL, job has finished\n");
	strncpy(job_id_out,job_id,job_id_out_len);
	//printf("errno = %d\n",pbs_errno);
	//char* errmsg = pbs_geterrmsg(_connect);
	//if (errmsg){
	//   printf("error message is: \"%s\"\n",errmsg);
	//}
	if (stat)
	   *stat = DRMAA_PS_DONE; //DRMAA_PS_UNDETERMINED; /* process status is finished, but cannot be determined as success or failure */
	finished = 1;
        pbs_statfree(state);
	state=NULL;
      }
      else{
	strncpy(status,state->attribs->value,STATUS_SIZE);
        pbs_statfree(state);
	state=NULL;

	//printf("Comparing status and formerStatus\n");
	if (strcmp(status,formerStatus)){ // display change
	  //printf("\nstatus->name = %s\n",state->name);
	  strncpy(formerStatus,status,STATUS_SIZE);
	  //fprintf(stderr,"\njob status = %s\n",status);
	  //printf("errno is %d\n",pbs_errno);
	} 
	else if (!strcmp(status,"R")||!strcmp(status,"E")){
        if (stat)
	   *stat = DRMAA_PS_RUNNING; /* job is running */
        if (!strcmp(status,"R") && rusage) {
		/* ask for resources used */
	        struct attrl* resources;
	        if ((resources = (struct attrl*)calloc(1,sizeof (struct attrl)))==NULL){
		  pthread_mutex_unlock(&_wait_lock);
	          return DRMAA_ERRNO_NO_MEMORY;
		}
	        resources->name=ATTR_l;
	        //fprintf(stderr,"Asking pbs server for the job resources\n");
	        state = pbs_statjob(_connect,(char*)job_id,resources,NULL);
	        if (state){
	           struct attrl* current_resource = state->attribs;
		   do{
	                //fprintf(stderr,"Got %s = %s\n",current_resource->resource,current_resource->value);
			// translate to drmaa values
			// todo
	                current_resource = current_resource->next;
	           } while (current_resource->next);
		   free(state);
	          }
	    }
	}
	else{
	  //fprintf(stderr,".");
	  //fflush(stderr);
	}
	if (timeout != DRMAA_TIMEOUT_NO_WAIT)
	  sleep(1);
	else
	  break;
	 
      }
      //pthread_mutex_unlock(&_wait_lock);
    }while((!finished && ((timeout == DRMAA_TIMEOUT_WAIT_FOREVER) || ((endTime = time(NULL)) < deadline))));
    //printf("while loop exiting, finished = %d\n",finished);

    pthread_mutex_lock(&_wait_lock);

    if (state)
	pbs_statfree(state);
    if (finished)
 	strncpy(job_id_out,job_id,job_id_out_len);
    if (timeout == DRMAA_TIMEOUT_NO_WAIT){
      //printf("in drmaa_wait: no wait\n");
      if (!strcmp(status,"Q")){
	if (stat)
	   *stat = DRMAA_PS_QUEUED_ACTIVE; /* job is queued and active */
      }
      else if (!strcmp(status,"H")){
	if (stat)
	   *stat = DRMAA_PS_USER_SYSTEM_ON_HOLD; /* job is queued and in user and system hold */
      }
      else if (!strcmp(status,"R")||!strcmp(status,"E")){
	if (stat)
	   *stat = DRMAA_PS_RUNNING; /* job is running */
        pbs_statfree(state);
        free(state);
        /* ask for resources used */
        struct attrl* resources;
        if ((resources = (struct attrl*)calloc(1,sizeof (struct attrl)))==NULL){
	  pthread_mutex_unlock(&_wait_lock);
          return DRMAA_ERRNO_NO_MEMORY;
	}
        resources->name=ATTR_l;
        //fprintf(stderr,"Asking pbs server for the job resources\n");
        state = pbs_statjob(_connect,(char*)job_id,resources,NULL);
        if (state){
	   struct attrl* current_resource = state->attribs;
	   do{
		//fprintf(stderr,"Got %s = %s\n",state->attribs->resource,state->attribs->value);
		current_resource = current_resource->next;
	   } while (current_resource->next);	
	  
	}
      }
      else if (!strcmp(status,"S")){
	if (stat)
	   *stat = DRMAA_PS_USER_SUSPENDED; /* job is user suspended */
      }
      else{
	if (stat)
	   *stat = DRMAA_PS_DONE; //DRMAA_PS_UNDETERMINED; /* process status cannot be determined */
      }
    }

    pthread_mutex_unlock(&_wait_lock);

    if (!finished){
      //printf("in drmaa_wait: timeout\n");
      pbs_statfree(state);
      free(state);
      if(job_id)
	free(job_id);
      if (att)
	free(att);
      return DRMAA_ERRNO_EXIT_TIMEOUT;
    }
    else if (dispose){
	/*
	 * reap off the job
	 */
      pthread_mutex_lock(&_job_list_lock);
      //printf ("Reap off the finished job:\n");
      if (!prec){
	//printf ("prec pointer not pointing to anything, setting job_list to current->next\n");
	job_list = cur->next;
      }
      else{
	//printf("prec pointer (%p) is %s\nSetting prec to current->next\n",prec,prec->job_id);
	prec->next = cur->next;
      }
      if (cur){
	if (cur->job_id){
	  free(cur->job_id);
	  cur->job_id = NULL;
	}
	free(cur);
	cur = NULL;
      }
      pthread_mutex_unlock(&_job_list_lock);
    }
    else
	//printf("No reap\n");

    pbs_statfree(state);
    state = NULL;
    //free(state);
    if (job_id){
      free(job_id);
      job_id=0;
    }
    //fprintf(stderr,"freeing att\n");
    if (att)
      free(att);
    //printf("returning from drmaa_wait\n");
    return DRMAA_ERRNO_SUCCESS;
  }
  else{
    // wait for any job
    // loops through the list of executing jobs and call drmaa_wait(NO_WAIT) until one is finished

    struct job_list_s* jobs = job_list;

    if (!jobs){
	//empty list, no job running
	return DRMAA_ERRNO_INVALID_JOB;
    }

    signed long callTime,returnTime;
    char id_out[DRMAA_JOBNAME_BUFFER];
    int job_status=-1;
    int finished = 0;
    int ret;
    callTime = time(NULL);
    returnTime = callTime;
    //    fprintf(stderr,"Waiting for all jobs\n");
    while (!finished){
	while(jobs && !finished){
	    callTime = time(NULL);
      	    // drmaa_wait_job in itself is thread safe
	    ret = drmaa_wait_job (job_list->job_id,id_out,DRMAA_JOBNAME_BUFFER,&job_status,DRMAA_TIMEOUT_NO_WAIT,rusage,dispose,error_diagnosis,error_diagnosis_len);
	    if (ret == DRMAA_ERRNO_SUCCESS){
		finished = 1;
		strncpy(job_id_out,id_out,job_id_out_len);
       	    }
	    returnTime = time(NULL);
            if ( timeout != DRMAA_TIMEOUT_WAIT_FOREVER ){
                timeout -= ( returnTime - callTime );
                if ( timeout <= 0 && !finished){
                  //printf ("Timeout!\n");
                  return DRMAA_ERRNO_EXIT_TIMEOUT;
                }
            }
    	}
	//sleep(1);
	jobs = job_list;
     }
     return ret;
   }
}

/*
 * Evaluates into 'exited' a non-zero value if stat was returned for a
 * job that terminated normally. A zero value can also indicate that
 * altough the job has terminated normally an exit status is not available
 * or that it is not known whether the job terminated normally. In both
 * cases drmaa_wexitstatus() will not provide exit status information.
 * A non-zero 'exited' value indicates more detailed diagnosis can be provided
 * by means of drmaa_wifsignaled(), drmaa_wtermsig() and drmaa_wcoredump().
 */
int drmaa_wifexited(int *exited, int stat, char *error_diagnosis, size_t error_diag_len)
{
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;
  strncpy(error_diagnosis,"Not implemented",error_diag_len);
  //return DRMAA_ERRNO_INTERNAL_ERROR;
  *exited = 1;
  //printf ("wifexited sets exited to %d\n",*exited);
  return DRMAA_ERRNO_SUCCESS;
}

/*
 * If the OUT parameter 'exited' of drmaa_wifexited() is non-zero,
 * this function evaluates into 'exit_code' the exit code that the
 * job passed to _exit() (see exit(2)) or exit(3C), or the value that
 * the child process returned from main.
 */
int drmaa_wexitstatus(int *exit_status, int stat, char *error_diagnosis, size_t error_diag_len)
{
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;
  strncpy(error_diagnosis,"Not implemented",error_diag_len);
  //return DRMAA_ERRNO_INTERNAL_ERROR;
  //*exit_status = 0;
  return DRMAA_ERRNO_SUCCESS;
}


/*
 * Evaluates into 'signaled' a non-zero value if status was returned
 * for a job that terminated due to the receipt of a signal. A zero value
 * can also indicate that altough the job has terminated due to the receipt
 * of a signal the signal is not available or that it is not known whether
 * the job terminated due to the receipt of a signal. In both cases
 * drmaa_wtermsig() will not provide signal information.
 */
int drmaa_wifsignaled(int *signaled, int stat, char *error_diagnosis, size_t error_diag_len)
{
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;
  strncpy(error_diagnosis,"Not implemented",error_diag_len);
  //return DRMAA_ERRNO_INTERNAL_ERROR;
  return DRMAA_ERRNO_SUCCESS;
}


/*
 * If the OUT parameter 'signaled' of drmaa_wifsignaled(stat) is
 * non-zero, this function evaluates into signal a string representation of the signal
 * that caused the termination of the job. For signals declared by POSIX, the symbolic
 * names are returned (e.g., SIGABRT, SIGALRM).
 * For signals not declared by POSIX, any other string may be returned.
 */
int drmaa_wtermsig(char *signal, size_t signal_len, int stat, char *error_diagnosis, size_t error_diag_len)
{
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;
  strncpy(error_diagnosis,"Not implemented",error_diag_len);
  //return DRMAA_ERRNO_INTERNAL_ERROR;
  return DRMAA_ERRNO_SUCCESS;
}


/*
 * If the OUT parameter 'signaled' of drmaa_wifsignaled(stat) is
 * non-zero, this function evaluates into 'core_dumped' a non-zero value
 * if a core image of the terminated job was created.
 */
int drmaa_wcoredump(int *core_dumped, int stat, char *error_diagnosis, size_t error_diag_len)
{
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;
  strncpy(error_diagnosis,"Not implemented",error_diag_len);
  //return DRMAA_ERRNO_INTERNAL_ERROR;
  return DRMAA_ERRNO_SUCCESS;
}


/*
 * Evaluates into 'aborted' a non-zero value if 'stat'
 * was returned for a job that ended before entering the running state.
 */
int drmaa_wifaborted(int *aborted, int stat, char *error_diagnosis, size_t error_diag_len)
{
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;
  strncpy(error_diagnosis,"Not implemented",error_diag_len);
  //return DRMAA_ERRNO_INTERNAL_ERROR;
  return DRMAA_ERRNO_SUCCESS;
}




/*
 * Get the program status of the job identified by 'job_id'.
 * The possible values returned in 'remote_ps' and their meanings are:
 * DRMAA_PS_UNDETERMINED = 00H : process status cannot be determined,
 * DRMAA_PS_QUEUED_ACTIVE = 10H : job is queued and active,
 * DRMAA_PS_SYSTEM_ON_HOLD = 11H : job is queued and in system hold,
 * DRMAA_PS_USER_ON_HOLD = 12H : job is queued and in user hold,
 * DRMAA_PS_USER_SYSTEM_ON_HOLD = 13H : job is queued and in user and system hold,
 * DRMAA_PS_RUNNING = 20H : job is running,
 * DRMAA_PS_SYSTEM_SUSPENDED = 21H : job is system suspended,
 * DRMAA_PS_USER_SUSPENDED = 22H : job is user suspended,
 * DRMAA_PS_USER_SYSTEM_SUSPENDED = 23H : job is user and system suspended,
 * DRMAA_PS_DONE = 30H : job finished normally, and
 * DRMAA_PS_FAILED = 40H : job finished, but failed.
 */
int drmaa_job_ps( const char *job_id, int *remote_ps, char *error_diagnosis, size_t error_diag_len){
  if (_connect == -1)
    return DRMAA_ERRNO_NO_ACTIVE_SESSION;
  int status;
  char job_id_out[50];
  int ret = drmaa_wait(job_id,job_id_out,50,&status,DRMAA_TIMEOUT_NO_WAIT,NULL,error_diagnosis,error_diag_len) ;
  if ( ret != DRMAA_ERRNO_SUCCESS )
    return ret;

  if (status == DRMAA_PS_QUEUED_ACTIVE || status == DRMAA_PS_USER_SYSTEM_ON_HOLD || 
      status ==  DRMAA_PS_RUNNING || status == DRMAA_PS_USER_SUSPENDED || status == DRMAA_PS_DONE )
    *remote_ps = status;
  else
    *remote_ps = DRMAA_PS_UNDETERMINED;
  return DRMAA_ERRNO_SUCCESS;
}

/* ------------------- auxiliary routines ------------------- */

/*
 * Get the error message text associated with the errno number.
 */
const char *drmaa_strerror(int drmaa_errno)
{
  if (_connect == -1)
    return "DRMAA_ERRNO_NO_ACTIVE_SESSION";
  printf("in drmaa_strerror(%d)\n",drmaa_errno);
  if ( drmaa_errno < DRMAA_ERRNO_SUCCESS || drmaa_errno > DRMAA_NO_ERRNO )
	return NULL;
  switch (drmaa_errno){
	case DRMAA_ERRNO_SUCCESS: return "Routine returned normally with success.";
	case DRMAA_ERRNO_INTERNAL_ERROR: return"Unexpected or internal DRMAA error like memory allocation, system call failure, etc.";
	case DRMAA_ERRNO_DRM_COMMUNICATION_FAILURE: return "Could not contact DRM system for this request.";
   	case DRMAA_ERRNO_AUTH_FAILURE: return "The specified request is not processed successfully due to authorization failure.";
	case DRMAA_ERRNO_INVALID_ARGUMENT: return "The input value for an argument is invalid.";
	case DRMAA_ERRNO_NO_ACTIVE_SESSION: return "Exit routine failed because there is no active session";
	case DRMAA_ERRNO_NO_MEMORY: return "Failed allocating memory";
	case DRMAA_ERRNO_INVALID_CONTACT_STRING: return "Initialization failed due to invalid contact string.";
   	case DRMAA_ERRNO_DEFAULT_CONTACT_STRING_ERROR: return "DRMAA could not use the default contact string to connect to DRM system.";
   	case DRMAA_ERRNO_NO_DEFAULT_CONTACT_STRING_SELECTED: return "DRMAA could not use the default contact string to connect to DRM system.";
   	case DRMAA_ERRNO_DRMS_INIT_FAILED: return "Initialization failed due to failure to init DRM system.";
   	case DRMAA_ERRNO_ALREADY_ACTIVE_SESSION: return "Initialization failed due to existing DRMAA session.";
   	case DRMAA_ERRNO_DRMS_EXIT_ERROR: return "DRM system disengagement failed.";
   	case DRMAA_ERRNO_INVALID_ATTRIBUTE_FORMAT: return "The format for the job attribute value is invalid.";
   	case DRMAA_ERRNO_INVALID_ATTRIBUTE_VALUE: return "The value for the job attribute is invalid.";
   	case DRMAA_ERRNO_CONFLICTING_ATTRIBUTE_VALUES: return "The value of this attribute is conflicting with a previously set attributes.";
   	case DRMAA_ERRNO_NO_MORE_ELEMENTS: return "No more element in the attribute name/value list";
   	case DRMAA_ERRNO_TRY_LATER: return "Could not pass job now to DRM system. A retry may succeed however (saturation)."; 
   	case DRMAA_ERRNO_DENIED_BY_DRM: return "The submission failed, either because the DRM system rejected the job or for communication problem."; 
   	case DRMAA_ERRNO_INVALID_JOB: return "The job specified by the 'jobid' does not exist.";
   	case DRMAA_ERRNO_RESUME_INCONSISTENT_STATE: return "The job has not been suspended. The RESUME request will not be processed.";
   	case DRMAA_ERRNO_SUSPEND_INCONSISTENT_STATE: return "The job has not been running, and it cannot be suspended."; 
   	case DRMAA_ERRNO_HOLD_INCONSISTENT_STATE: return "The job cannot be moved to a HOLD state.";
   	case DRMAA_ERRNO_RELEASE_INCONSISTENT_STATE: return "The job is not in a HOLD state.";
   	case DRMAA_ERRNO_EXIT_TIMEOUT: return "We have encountered a time-out condition for drmaa_synchronize or drmaa_wait.";
   	case DRMAA_NO_ERRNO: return "No errno...";

  }

  return NULL;
}

/*
 * Current contact information for DRM system (string)
 */
int drmaa_get_contact(char *contact, size_t contact_len, char *error_diagnosis, size_t error_diag_len)
{
  if (pbs_contact){
	strncpy(contact,pbs_contact,contact_len);
	return DRMAA_ERRNO_SUCCESS;
  }
  else{
	strncpy(error_diagnosis,"contact not yet initialized",error_diag_len);
        strncpy(contact,"localhost",contact_len);
	return DRMAA_ERRNO_SUCCESS;
  }
}

/*
 * OUT major - major version number (non-negative integer)
 * OUT minor - minor version number (non-negative integer)
 * Returns the major and minor version numbers of the DRMAA library;
 * for DRMAA 1.0, 'major' is 1 and 'minor' is 0.
 */
int drmaa_version(unsigned int *major, unsigned int *minor, char *error_diagnosis, size_t error_diag_len)
{
  *major=1;
  *minor=0;
  return DRMAA_ERRNO_SUCCESS;
}


/*
 * returns DRM system implementation information
 * Output (string) is implementation dependent and could contain the DRM system and the
 * implementation vendor as its parts.
 */
int drmaa_get_DRM_system(char *drm_system, size_t drm_system_len, char *error_diagnosis, size_t error_diag_len)
{
  strncpy(drm_system,"PBS/Torque",drm_system_len);
  return DRMAA_ERRNO_SUCCESS;
}

int drmaa_get_DRMAA_implementation(char *drmaa_implementation, size_t drmaa_implementation_len, char *error_diagnosis, size_t error_diag_len)
{
  strncpy(drmaa_implementation,"PBS/Torque drmaa v1.0 implementation",drmaa_implementation_len);
  return DRMAA_ERRNO_SUCCESS;
}


char* replacePattern(char* source, char* pattern,char* replacement){
  char *dest=NULL;
  char *temp=NULL;
  int size,diff;
  char end = 0;
  char* index=NULL;
  int times = 0;

  // compute the final size of dest
  index = source;
  do{
    index = strstr(index,pattern);
    if (index){
      times++;
      index++;
    }
  }while (index);
  
  if (!times){
    //printf("No occurence of %s in %s\n",pattern,source);
    return strdup(source);
  }

  // diff is the size difference between pattern and remplacement
  diff = strlen(pattern) - strlen(replacement);

  // size is the predicted size of the destination buffer
  size = strlen(source)-(times*diff);
  //printf("dest size = %d - ((%d) X (%d)) = %d\n",strlen(source),times,diff,size);

  // we work on the destination buffer, so its size must be at least equal to the
  // source's size, greater if strlen(remplacement)>strlen(pattern)
  
  dest = (char*) calloc ((size > strlen(source))?size+1:strlen(source)+1,sizeof(char));
  temp = (char*) calloc ((size > strlen(source))?size+1:strlen(source)+1,sizeof(char));
  strncpy (temp, source, strlen(source)+1);
  
  while(!end){
    //printf("source temp = %s\nSize = %d\n\n",temp,strlen(temp));
    
    //printf("DRMAA_PLACEHOLDER_INCR = %s\nSize = %d\n\n",DRMAA_PLACEHOLDER_INCR,strlen(DRMAA_PLACEHOLDER_INCR ));
    
    //printf("incr = %s\nSize = %d\n\n",replacement,strlen(replacement));
    index = strstr(temp,pattern);
    //printf("At index, temp = %s\n\n",index);
    if (!index){
      //printf("No %s found in %s\n",pattern,temp);
      end = 1;
    }
    else{
      size = index-temp;
      //printf("size = index-temp = %d\n",size);
      //printf("strncpy(%s,%s,%d)\n",dest,temp,size);
      strncpy(dest,temp,size);
      dest[size]='\0';
      //printf("dest = %s\n\n",dest);
      
      // add the replacement
      strcat(dest,replacement);
      
      // add the end of the source
      // place index on the first char following the pattern
      index += (strlen(pattern));
      //printf("Next char = %c\n",*index);
      //printf("%d chars left\n",strlen(index));
      strncat(dest,index,strlen(index)+1);
      //printf("Dest = %s\n",dest);
      
      strncpy(temp,dest,strlen(dest)+1);
    }
  }
  
  // realloc to set the buffer size to the string size
  size = strlen(dest)+1;
  realloc(dest,size*sizeof(char));
  free (temp);
  //printf("dest = %s\n",dest);
  return dest;
}


#ifdef  __cplusplus
}
#endif

//#endif /* __DRMAA_H */


