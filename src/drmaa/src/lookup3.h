/* $Id: lookup3.h,v 1.2 2006/09/08 18:18:08 ciesnik Exp $ */

/**
 * @file lookup3.h
 * 32bit hash function implementation.
 *
 * Taken from:
 * http://burtleburtle.net/bob/hash/
 */

#ifndef __LOOKUP3_H
#define __LOOKUP3_H

#ifndef HAVE_CONFIG_H
# include <pbs_config.h>
#endif

#ifdef HAVE_STDINT_H
# include <stdint.h>
#else
# ifdef HAVE_INTTYPES_H
#  include <inttypes.h>
# endif
#endif

uint32_t hashword(const uint32_t *k, size_t length, uint32_t initval);
uint32_t hashlittle(const void *key, size_t length, uint32_t initval);
uint32_t hashbig(const void *key, size_t length, uint32_t initval);

#if BYTEORDER == 1234 /* little endian */
# define hashstr hashlittle
#endif

#if BYTEORDER == 4321  /* big endian */
# define hashstr hashbig
#endif

#endif /* __LOOKUP3_H */

