/* $Id: error.h,v 1.4 2006/09/08 18:18:08 ciesnik Exp $ */
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

/**
 * @file error.h
 * Rasing errors.
 */

#ifndef __DRMAA__ERROR_H
#define __DRMAA__ERROR_H

#ifdef HAVE_CONFIG_H
# include <pbs_config.h>
#endif

#include <stddef.h>


int drmaa_get_errno_error(char *error_diagnosis, size_t error_diag_len);
int drmaa_get_drmaa_error(char *error_diagnosis, size_t error_diag_len,
                          int error_code);
int drmaa_get_pbs_error(char *error_diagnosis, size_t error_diag_len);

int drmaa_map_pbs_error(int pbs_errcode);

void drmaa_log(const char *fmt, ...)
__attribute__((format(printf, 1, 2)));


#if DRMAA_DEBUG
# define DEBUG( args )  do{ drmaa_log args ; }while(0)
#else
# define DEBUG( args )  /* nothing */
#endif

#define STORE_ERRNO_MSG() \
  do{  drmaa_get_errno_error( errmsg, errlen );  }while(0)

#define RAISE_ERRNO( retcode ) do {   \
    STORE_ERRNO_MSG();                \
    return retcode;                   \
    } while(0)

#define SET_DRMAA_ERROR( retcode ) \
  do{  drmaa_get_drmaa_error( errmsg, errlen, (retcode) );  } while(0)

#define RAISE_DRMAA( retcode ) do { \
    SET_DRMAA_ERROR( retcode );     \
    return retcode;                 \
    } while(0)

#define RAISE_NO_MEMORY() \
  RAISE_DRMAA( DRMAA_ERRNO_NO_MEMORY )

#define RAISE_PBS() \
  do{  return drmaa_get_pbs_error( errmsg, errlen );  } while(0)

#define SET_PBS_ERROR( rc_var ) \
  do{  (rc_var) = drmaa_get_pbs_error( errmsg, errlen );  } while(0)


#endif /* __DRMAA__ERROR_H */

