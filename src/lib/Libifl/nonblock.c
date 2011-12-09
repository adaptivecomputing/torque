/*
 * Defns of nonblocking read,write.
 * Headers redefine read/write to name these instead, before inclusion
 * of stdio.h, so system declaration is used.
 */
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>



ssize_t write_nonblocking_socket(

  int     fd,
  void   *buf,
  ssize_t count)

  {
  ssize_t i;
  time_t  start, now;

  /* NOTE:  under some circumstances, a blocking fd will be passed */

  /* Set a timer to prevent an infinite loop here. */
  time(&now);
  start = now;

  for (;;)
    {
    i = write(fd, buf, count);

    if (i >= 0)
      {
      /* successfully wrote 'i' bytes */

      return(i);
      }

    if (errno != EAGAIN)
      {
      /* write failed */

      return(i);
      }


    time(&now);
    if ((now - start) > 30)
      {
      /* timed out */

      return(i);
      }
    }    /* END for () */

  /*NOTREACHED*/

  return(0);
  }  /* END write_nonblocking_socket() */



ssize_t read_nonblocking_socket(

  int     fd,
  void   *buf,
  ssize_t count)

  {
  int     flags;
  ssize_t i;
  time_t  start, now;

  /* verify socket is non-blocking */

  /* NOTE:  under some circumstances, a blocking fd will be passed */

  if ((flags = fcntl(fd, F_GETFL)) == -1)
    {
    return(-1);
    }

#if defined(FNDELAY) && !defined(__hpux)
  if (flags & FNDELAY)
#else
  if (flags & O_NONBLOCK)
#endif
    {
    /* flag already set */

    /* NO-OP */
    }
  else
    {
    /* set no delay */

#if defined(FNDELAY) && !defined(__hpux)
    flags |= FNDELAY;
#else
    flags |= O_NONBLOCK;
#endif

    /* NOTE:  the pbs scheduling API passes in a blocking socket which
              should be a non-blocking socket in pbs_disconnect.  Also,
              qsub passes in a blocking socket which must remain
              non-blocking */

    /* the below non-blocking socket flag check should be rolled into
       pbs_disconnect and removed from here (NYI) */

    /*
    if (fcntl(fd,F_SETFL,flags) == -1)
      {
      return(-1);
      }
    */
    }    /* END else (flags & BLOCK) */

  /* Set a timer to prevent an infinite loop here. */

  start = -1;

  for (;;)
    {
    i = read(fd, buf, count);

    if (i >= 0)
      {
      return(i);
      }

    if (errno != EAGAIN)
      {
      return(i);
      }

    time(&now);

    if (start == -1)
      {
      start = now;
      }
    else if ((now - start) > 30)
      {
      return(i);
      }
    }    /* END for () */

  /*NOTREACHED*/

  return(0);
  }  /* END read_nonblocking_socket() */





/*
 * Call the real read, for things that want to block.
 */

ssize_t read_blocking_socket(

  int      fd,
  void    *buf,
  ssize_t  count)

  {
  return(read(fd, buf, count));
  }

