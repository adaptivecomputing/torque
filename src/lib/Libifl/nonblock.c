/*
 * Defns of nonblocking read,write.
 * Headers redefine read/write to name these instead, before inclusion
 * of stdio.h, so system declaration is used.
 */
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

/*
 * Assumes full-block read/write.  No accounting for partial blocks,
 * this would have had to be handled by the main pbs code anyway.
 */

ssize_t write_nonblocking_socket(

  int         fd,    /* I */
  const void *buf,   /* I */
  ssize_t     count) /* I */

  {
  ssize_t i;
 
  for (;;) 
    {
    i = write(fd,buf,count);

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
    }

  /* NOTE:  what is current SIGPIPE handling behavior? */

  /* non-blocking socket not ready.  no bytes written */

  return(0);
  }  /* END write_nonblocking_socket() */



 
ssize_t read_nonblocking_socket(

  int     fd, 
  void   *buf, 
  ssize_t count)

  {
  int     flags;
  ssize_t i;

  /* verify socket is non-blocking */

  /* NOTE:  under some circumstances, a blocking fd will be passed */

  if ((flags = fcntl(fd,F_GETFL)) == -1)
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
    
    if (fcntl(fd,F_SETFL,flags) == -1)
      {
      return(-1);
      }
    }    /* END else (flags & BLOCK) */
 
  for (;;) 
    {
    i = read(fd,buf,count);

    if (i >= 0) 
      {
      return(i);
      }

    if (errno != EAGAIN) 
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
  return(read(fd,buf,count));
  }

