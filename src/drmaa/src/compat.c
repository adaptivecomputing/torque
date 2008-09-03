/* $Id: compat.c,v 1.1 2006/08/29 10:24:38 ciesnik Exp $ */
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

#include <compat.h>
#include <stdio.h>
#include <stdlib.h>


#ifndef lint
static char rcsid[]
# ifdef __GNUC__
__attribute__((unused))
# endif
= "$Id: compat.c,v 1.1 2006/08/29 10:24:38 ciesnik Exp $";
#endif


#ifndef HAVE_STRLCPY
size_t
strlcpy(char *dest, const char *src, size_t size)
  {
  size_t result = 0;

  if (size == 0)
    return 0;

  while (*src  &&  --size > 0)
    {
    *dest++ = *src++;
    result++;
    }

  *dest++ = '\0';

  return result;
  }

#endif /* ! HAVE_STRLCPY */


#ifndef HAVE_ASPRINTF
int
asprintf(char **strp, const char *fmt, ...)
  {
  va_list args;
  int result;
  va_start(args, fmt);
  result = vasprintf(strp, fmt, args);
  va_end(args);
  return result;
  }

#endif /* ! HAVE_ASPRINTF */


#ifndef HAVE_VASPRINTF
int
vasprintf(char **strp, const char *fmt, va_list ap)
  {
  size_t size;
  char *buf;
  buf = (char*)malloc(size = 128);

  if (buf == NULL)
    return -1;

  while (1)
    {
    int len;
    char *oldbuf;

#  if defined(HAVE_VA_COPY) || defined(HAVE___VA_COPY)
    va_list args;
    va_copy(args, ap);
    len = sprintfv(fmt, args);
#  else /* ! HAVE_VA_COPY */
    len = sprintfv(fmt, ap);
#  endif

    if (len < size)
      {
      buf = realloc(buf, len + 1);
      *strp = buf;
      return len;
      }

    if (len == -1)
      size *= 2;
    else size = len + 1;

    buf = realloc(oldbuf = buf, size *= 2);

    if (buf == NULL)
      {
      free(oldbuf);
      return -1;
      }
    }
  }

#endif /* ! HAVE_VASPRINTF */


