/* $Id: compat.h,v 1.2 2006/09/08 18:18:08 ciesnik Exp $ */
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
 * @file compat.h
 * System compatibility functions.
 */

#ifndef __DRMAA__COMPAT_H
#define __DRMAA__COMPAT_H

#ifdef HAVE_CONFIG_H
# include <pbs_config.h>
#endif

#include <stddef.h>
#include <stdarg.h>

#ifndef HAVE_STRLCPY
size_t strlcpy(char *dest, const char *src, size_t size);
#endif

#ifndef HAVE_ASPRINTF
int asprintf(char **strp, const char *fmt, ...);
#endif

#ifndef HAVE_VASPRINTF
int vasprintf(char **strp, const char *fmt, va_list ap);
#endif

#ifndef HAVE_STDBOOL_H
# ifndef bool
#  define bool int
# endif
# ifndef true
#  define true 1
# endif
# ifndef false
#  define false 0
# endif
#endif

#endif /* __DRMAA__COMPAT_H */

