/* $Id: util.c,v 1.6 2006/09/05 13:50:00 ciesnik Exp $ */
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <drmaa_impl.h>
#include <attrib.h>

#ifndef lint
static char rcsid[]
# ifdef __GNUC__
__attribute__((unused))
# endif
= "$Id: util.c,v 1.6 2006/09/05 13:50:00 ciesnik Exp $";
#endif


int
drmaa_get_attribute_names(drmaa_attr_names_t **values, char *errmsg, size_t errlen)
  {
  return drmaa_impl_get_attribute_names(values,
                                        ATTR_F_VECTOR | ATTR_F_IMPL, ATTR_F_IMPL,
                                        errmsg, errlen);
  }


int
drmaa_get_vector_attribute_names(drmaa_attr_names_t **values, char *errmsg, size_t errlen)
  {
  return drmaa_impl_get_attribute_names(values,
                                        ATTR_F_VECTOR | ATTR_F_IMPL, ATTR_F_VECTOR | ATTR_F_IMPL,
                                        errmsg, errlen);
  }


int
drmaa_impl_get_attribute_names(
  drmaa_attr_names_t **values,
  unsigned f_mask, unsigned flags,
  char *errmsg, size_t errlen
)
  {
  char **result;
  unsigned i, j;
  result = (char**)calloc(N_DRMAA_ATTRIBS + 1, sizeof(char*));

  if (result == NULL)
    RAISE_NO_MEMORY();

  for (i = 0, j = 0;  i < N_ATTRIBS;  i++)
    {
    const drmaa_attrib_info_t *attr = drmaa_attr_table + i;

    if (attr->drmaa_name != NULL  && (attr->flags & f_mask) == flags)
      {
      result[j] = strdup(attr->drmaa_name);

      if (result[j] == NULL)
        {
        drmaa_free_vector(result);
        RAISE_NO_MEMORY();
        }

      j++;
      }
    }

  result[j] = NULL;

  *values = (drmaa_attr_names_t*)malloc(sizeof(drmaa_attr_names_t));

  if (*values == NULL)
    {
    drmaa_free_vector(result);
    RAISE_NO_MEMORY();
    }

  (*values)->list = (*values)->iter = result;

  return DRMAA_ERRNO_SUCCESS;
  }



#define DRMAA_GET_NEXT_VALUE()                         \
  if( values == NULL || *values->iter == NULL )        \
    return DRMAA_ERRNO_NO_MORE_ELEMENTS;               \
  strlcpy( value, *values->iter++, value_len );  \
  return DRMAA_ERRNO_SUCCESS;
int drmaa_get_next_attr_name(drmaa_attr_names_t* values, char *value, size_t value_len)
  {
  DRMAA_GET_NEXT_VALUE();
  }

int drmaa_get_next_attr_value(drmaa_attr_values_t* values, char *value, size_t value_len)
  {
  DRMAA_GET_NEXT_VALUE();
  }

int drmaa_get_next_job_id(drmaa_job_ids_t* values, char *value, size_t value_len)
  {
  DRMAA_GET_NEXT_VALUE();
  }

#define DRMAA_GET_NUM_VALUES()   \
  char **i;                      \
  size_t cnt = 0;                \
  if( values != NULL )           \
    for( i = values->list;  *i != NULL;  i++ ) \
      cnt++;                     \
  *size = cnt;                   \
  return DRMAA_ERRNO_SUCCESS;
int drmaa_get_num_attr_names(drmaa_attr_names_t* values, size_t *size)
  {
  DRMAA_GET_NUM_VALUES();
  }

int drmaa_get_num_attr_values(drmaa_attr_values_t* values, size_t *size)
  {
  DRMAA_GET_NUM_VALUES();
  }

int drmaa_get_num_job_ids(drmaa_job_ids_t* values, size_t *size)
  {
  DRMAA_GET_NUM_VALUES();
  }


void drmaa_release_attr_names(drmaa_attr_names_t* values)
  {
  drmaa_free_vector(values->list);
  free(values);
  }

void drmaa_release_attr_values(drmaa_attr_values_t* values)
  {
  drmaa_free_vector(values->list);
  free(values);
  }

void drmaa_release_job_ids(drmaa_job_ids_t* values)
  {
  drmaa_free_vector(values->list);
  free(values);
  }


void
drmaa_free_vector(char **vector)
  {
  char **i;

  if (vector == NULL)
    return;

  for (i = vector;  *i != NULL;  i++)
    free(*i);

  free(vector);
  }


int
drmaa_get_contact(
  char *contact, size_t contact_len,
  char *errmsg, size_t errlen
)
  {
  pthread_mutex_lock(&drmaa_session_mutex);

  if (drmaa_session != NULL)
    strlcpy(contact, drmaa_session->contact, contact_len);
  else
    strlcpy(contact, "localhost", contact_len);

  pthread_mutex_unlock(&drmaa_session_mutex);

  return DRMAA_ERRNO_SUCCESS;
  }


int
drmaa_version(
  unsigned int *major, unsigned int *minor,
  char *errmsg, size_t errlen
)
  {
  *major = 1;
  *minor = 0;
  return DRMAA_ERRNO_SUCCESS;
  }


int
drmaa_get_DRM_system(
  char *drm_system, size_t drm_system_len,
  char *errmsg, size_t errlen
)
  {
  strlcpy(drm_system, "Torque/PBS", drm_system_len);
  return DRMAA_ERRNO_SUCCESS;
  }


int
drmaa_get_DRMAA_implementation(
  char *drmaa_impl, size_t drmaa_impl_len,
  char *errmsg, size_t errlen
)
  {
  strlcpy(
    drmaa_impl, PACKAGE_NAME" v. "PACKAGE_VERSION" (by DSP Team)",
    drmaa_impl_len);
  return DRMAA_ERRNO_SUCCESS;
  }


char *
drmaa_replace(char *str, const char *placeholder, const char *value)
  {
  size_t ph_len, v_len;
  char *found = NULL;

  ph_len = strlen(placeholder);
  v_len  = strlen(value);

  do
    {
    size_t str_len;
    str_len = strlen(str);
    found = strstr(str, placeholder);

    if (found)
      {
      char *result;
      size_t pos = found - str;
      result = (char*)malloc(str_len - ph_len + v_len + 1);

      if (result == NULL)
        return NULL;

      memcpy(result, str, pos);

      memcpy(result + pos, value, v_len);

      memcpy(result + pos + v_len, str + pos + ph_len, str_len - pos - ph_len);

      result[ str_len-ph_len+v_len ] = 0;

      free(str);

      str = result;
      }
    }
  while (found);

  return str;
  }


