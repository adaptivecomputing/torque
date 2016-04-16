/* dummy calls for the tcp_dis set of calls.
 * This set of calls keeps bytes "written" in a buffer
 * which can then be "read" with the same "file handle"
 */

#include <pthread.h>
#include <memory.h>
#include <stdlib.h>
#include <deque>
#include <map>
#include <stdio.h>
#include <errno.h>
#include "dis.h"
#include "pbs_error.h"


class tcpData
  {
  private:
  pthread_mutex_t   mutex;
  std::deque<char> data;

  public:
  tcpData()
    {
    memset(&mutex,0,sizeof(mutex));
    }

  int read(char **bf,long long *len)
    {
    pthread_mutex_lock(&mutex);
    int readCnt = 0;
    *bf = (char *)calloc(1,data.size() + 1);
    char *ind = *bf;
    while(data.size() != 0)
      {
      *ind++ = data.front();
      data.pop_front();
      readCnt++;
      }
    *len = (long long)readCnt;
    pthread_mutex_unlock(&mutex);
    return PBSE_NONE;
    }

  int write(char *bf,long long len)
    {
    pthread_mutex_lock(&mutex);
    int writeCnt = 0;
    while(len > 0)
      {
      data.push_back(*bf++);
      len--;
      writeCnt++;
      }
    pthread_mutex_unlock(&mutex);
    return writeCnt;
    }
  };

std::map<int,tcpData *> fds;

int debug_read(int sock,char **bf,long long *len)
  {
  return fds[sock]->read(bf,len);
  }

int debug_write(int sock,char *bf,long long len)
  {
  return fds[sock]->write(bf,len);
  }

void DIS_tcp_settimeout(long timeout)  /* I */
  {
  }  /* END DIS_tcp_settimeout() */

/*
 * tcp_pack_buff - pack existing data into front of buffer
 *
 * Moves "uncommited" data to front of buffer and adjusts pointers.
 * Does a character by character move since data may over lap.
 */

static void tcp_pack_buff(

  struct tcpdisbuf *tp)

  {
  size_t amt;
  size_t start;
  size_t i;

  start = tp->tdis_trailp - tp->tdis_thebuf;

  if (start != 0)
    {
    amt  = tp->tdis_eod - tp->tdis_trailp;

    for (i = 0;i < amt;++i)
      {
      *(tp->tdis_thebuf + i) = *(tp->tdis_thebuf + i + start);
      }
    *(tp->tdis_thebuf + amt) = '\0';

    tp->tdis_leadp  -= start;

    tp->tdis_trailp -= start;
    tp->tdis_eod    -= start;
    }

  return;
  }  /* END tcp_pack_buff() */


/*
 * tcp_read - read data from tcp stream to "fill" the buffer
 * Update the various buffer pointers.
 *
 * Return: >0 number of characters read
 *   0 if EOD (no data currently avalable)
 *  -1 if error
 *  -2 if EOF (stream closed)
 */

int tcp_read(

  struct tcp_chan *chan,
  long long       *read_len,
  long long       *avail_len)

  {
  int               rc = PBSE_NONE;
  unsigned long     newsize;
  char             *ptr;
  int               tdis_buf_len = 0;
  int               max_read_len = 0;
  char             *new_data = NULL;
  struct tcpdisbuf *tp;
  int               tmp_leadp = 0;
  int               tmp_trailp = 0;
  int               tmp_eod = 0;

  tcpData *data = fds[chan->sock];

  tp = &chan->readbuf;

  /* must compact any uncommitted data into bottom of buffer */
  tcp_pack_buff(tp);

  chan->IsTimeout = 0;
  chan->SelectErrno = 0;
  chan->ReadErrno = 0;
  tdis_buf_len = tp->tdis_bufsize;
  max_read_len = tp->tdis_bufsize - (tp->tdis_eod - tp->tdis_thebuf);

  /*
   * we don't want to be locked out by an attack on the port to
   * deny service, so we time out the read, the network had better
   * deliver promptly
   */

  if ((rc = data->read(&new_data, read_len)) != PBSE_NONE)
    {
    switch (rc)
      {
      case PBSE_TIMEOUT:

        chan->IsTimeout = 1;

        break;

      default:

        chan->SelectErrno = rc;
        chan->ReadErrno = rc;

        break;
      }

    if (new_data != NULL)
      free(new_data);

    return(rc);
    }
  /* data read is less than buffer size */
  else if (max_read_len > *read_len)
    {
    memcpy(tp->tdis_eod, new_data, *read_len);
    tp->tdis_eod += *read_len;
    *tp->tdis_eod = '\0';
    *avail_len = tp->tdis_eod - tp->tdis_leadp;
    max_read_len = tp->tdis_eod - tp->tdis_thebuf;

    /**********
    if (max_read_len > tdis_buf_len)
      {
      snprintf(err_msg, sizeof(err_msg),
        "eod ptr BEYOND end of buffer!! (fit) Remaining buffer = %d, read_len = %lld",
        max_read_len, *read_len);
      log_err(PBSE_INTERNAL,__func__,err_msg);
      }
      *****************/

    free(new_data);
    }
  /* data read is greater than buffer size */
  else if (max_read_len <= *read_len)
    {
    newsize = (tdis_buf_len + *read_len) * 2;
    if ((ptr = (char *)calloc(1, newsize+1)) == NULL)
      {
      rc = PBSE_MEM_MALLOC;
      free(new_data);
      return rc;
      }

    tmp_leadp = tp->tdis_leadp - tp->tdis_thebuf;
    tmp_trailp = tp->tdis_trailp - tp->tdis_thebuf;
    tmp_eod = tp->tdis_eod - tp->tdis_thebuf;

    snprintf(ptr, newsize, "%s%s", tp->tdis_thebuf, new_data);
    free(tp->tdis_thebuf);
    tp->tdis_thebuf = ptr;
    tp->tdis_bufsize = newsize;
    tp->tdis_eod = tp->tdis_thebuf + tmp_eod + *read_len;
    tp->tdis_trailp = tp->tdis_thebuf + tmp_trailp;
    tp->tdis_leadp = tp->tdis_thebuf + tmp_leadp;
    *avail_len = tp->tdis_eod - tp->tdis_leadp;

    max_read_len = tp->tdis_eod - tp->tdis_thebuf;
    tdis_buf_len = newsize;

    /************
    if (max_read_len > tdis_buf_len)
      {
      snprintf(err_msg, sizeof(err_msg), "eod ptr BEYOND end of buffer!!(expand) Remaining buffer = %d, read_len = %lld", max_read_len, *read_len);
      log_err(PBSE_INTERNAL,__func__,err_msg);
      }
      *********************/

    free(new_data);
    }

  return(rc);
  }  /* END tcp_read() */





/*
 * DIS_tcp_wflush - flush tcp/dis write buffer
 *
 * Writes "committed" data in buffer to file discriptor,
 * packs remaining data (if any), resets pointers
 * Returns: 0 on success, -1 on error
 *      NOTE:  does not close fd
 *
 */

int DIS_tcp_wflush(

  struct tcp_chan *chan)  /* I */

  {
  size_t            ct;
  int               i;
  char             *pb = NULL;

  struct tcpdisbuf *tp;
  tcpData *data = fds[chan->sock];

  tp = &chan->writebuf;
  pb = tp->tdis_thebuf;
  ct = tp->tdis_trailp - tp->tdis_thebuf;


  while ((i = data->write(pb, ct)) != (ssize_t)ct)
    {
    if (i == -1)
      {
      if (errno == EINTR)
        {
        continue;
        }

      /* FAILURE */

      /************
      if (pbs_debug != NULL)
        {
        fprintf(stderr,
          "TCP write of %d bytes (%.32s) [sock=%d] failed, errno=%d (%s)\n",
          (int)ct, pb, chan->sock, errno, strerror(errno));
        }
        ***********************/

      return(-1);
      }  /* END if (i == -1) */
    else
      {
      ct -= i;
      pb += i;
      }
    }  /* END while (i) */

  /* SUCCESS */

  tp->tdis_eod = tp->tdis_leadp;

  tcp_pack_buff(tp);

  return(0);
  }  /* END DIS_tcp_wflush() */





/*
 * DIS_tcp_clear - reset tpc/dis buffer to empty
 */

static void DIS_tcp_clear(

  struct tcpdisbuf *tp)

  {
  tp->tdis_leadp  = tp->tdis_thebuf;
  tp->tdis_trailp = tp->tdis_thebuf;
  tp->tdis_eod    = tp->tdis_thebuf;

  return;
  }





void DIS_tcp_reset(

  struct tcp_chan *chan,
  int i)

  {
  if (i == 0)
    DIS_tcp_clear(&chan->readbuf);
  else
    DIS_tcp_clear(&chan->writebuf);
  return;
  }  /* END DIS_tcp_reset() */





/*
 * tcp_rskip - tcp/dis support routine to skip over data in read buffer
 *
 * Returns: 0 on success, -1 on error
 */

int tcp_rskip(

  struct tcp_chan *chan,
  size_t ct)

  {
  struct tcpdisbuf *tp;
  tp = &chan->readbuf;
  if (tp->tdis_leadp - tp->tdis_eod < (ssize_t)ct)
    {
    /* this isn't the best thing to do, but this isn't used, so */
    return(-1);
    }
  tp->tdis_leadp += ct;
  return(0);
  }



/*
 * tcp_gets - tcp/dis support routine to get a string from read buffer
 *
 * Return: number of characters read (>=1)
 *  -1 if error
 *  -2 if EOF/EOD (stream closed)
 */

int tcp_gets(

  struct tcp_chan *chan,
  char            *str,
  size_t           ct,
  unsigned int     timeout)

  {
  int               rc = 0;
  struct tcpdisbuf *tp;
  long long         data_read = 0;
  long long         data_avail = 0;

  tp = &chan->readbuf;
  /* length of usable data in current buffer */
  data_avail = tp->tdis_eod - tp->tdis_leadp;

  while ((size_t)data_avail < ct)
    {
    /* not enough data, try to get more */
    if ((rc = tcp_read(chan,&data_read, &data_avail)) != PBSE_NONE)
      {
      if (data_read == 0)
        rc = -2;
      else
        rc = -1;
      return(rc);  /* Error or EOF */
      }
    }
  memcpy((char *)str, tp->tdis_leadp, ct);
  tp->tdis_leadp += ct;
  return((int)ct);
  }  /* END tcp_gets() */


/*
 * tcp_getc - see tcp_gets
 */

int tcp_getc(

  struct tcp_chan *chan,
  unsigned int     timeout)

  {
  int rc = DIS_SUCCESS;
  char ret_val;
  if ((rc = tcp_gets(chan, &ret_val, 1, timeout)) < 0)
    return rc;
  return (int)ret_val;
  }  /* END tcp_getc() */



/*
 * tcp_puts - tcp/dis support routine to put a counted string of characters
 * into the write buffer.
 *
 * Returns: >= 0, the number of characters placed
 *   -1 if error
 */

int tcp_puts(

  struct tcp_chan *chan,  /* I */
  const char *str, /* I */
  size_t      ct)  /* I */

  {
  struct tcpdisbuf *tp = NULL;
  char             *temp = NULL;
  int               leadpct;
  int               trailpct;
  size_t            newbufsize;

  /* NOTE:  currently, failures may occur if THE_BUF_SIZE is not large enough */
  /*        this should be changed to allow proper operation with degraded    */
  /*        performance (how?) */

  tp = &chan->writebuf;
  if ((tp->tdis_thebuf + tp->tdis_bufsize - tp->tdis_leadp) < (ssize_t)ct)
    {
    /* not enough room, reallocate the buffer */
    leadpct = (int)(tp->tdis_thebuf - tp->tdis_leadp);
    trailpct = (int)(tp->tdis_thebuf - tp->tdis_trailp);
    newbufsize = tp->tdis_bufsize + THE_BUF_SIZE + ct*2;
    temp = (char *)calloc(1, newbufsize+1);
    if (!temp)
      {
      /* FAILURE */
      /***********
      snprintf(log_buf,sizeof(log_buf),
        "out of space in buffer and cannot calloc message buffer (bufsize=%ld, buflen=%d, ct=%d)\n",
        tp->tdis_bufsize,
        (int)(tp->tdis_leadp - tp->tdis_thebuf),
        (int)ct);
      log_err(ENOMEM, __func__, log_buf);
      *************************/
      return(-1);
      }

    memcpy(temp, tp->tdis_thebuf, tp->tdis_bufsize);
    /*************
    if (strlen(tp->tdis_thebuf) > tp->tdis_bufsize)
      {
      snprintf(log_buf, sizeof(log_buf),
          "line #%d, The length of the string is GREATER than the size of buf",
          __LINE__);
      log_err(ENOMEM, __func__, log_buf);
      }
      **************************/
    free(tp->tdis_thebuf);
    tp->tdis_thebuf = temp;
    tp->tdis_bufsize = newbufsize;
    tp->tdis_leadp = tp->tdis_thebuf - leadpct;
    tp->tdis_trailp = tp->tdis_thebuf - trailpct;
    tp->tdis_eod = tp->tdis_thebuf + newbufsize;

    }

  memcpy(tp->tdis_leadp, (char *)str, ct);

  tp->tdis_leadp += ct;

  return(ct);
  }  /* END tcp_puts() */




/*
 * tcp_rcommit - tcp/dis support routine to commit/uncommit read data
 */

int tcp_rcommit(

  struct tcp_chan *chan,
  int commit_flag)

  {

  struct tcpdisbuf *tp;

  tp = &chan->readbuf;

  if (commit_flag)
    {
    /* commit by moving trailing up */

    tp->tdis_trailp = tp->tdis_leadp;
    }
  else
    {
    /* uncommit by moving leading back */

    tp->tdis_leadp = tp->tdis_trailp;
    }

  return(0);
  }  /* END tcp_rcommit() */





/*
 * tcp_wcommit - tcp/dis support routine to commit/uncommit write data
 */

int tcp_wcommit(

  struct tcp_chan *chan,
  int commit_flag)

  {

  struct tcpdisbuf *tp;

  tp = &chan->writebuf;

  if (commit_flag)
    {
    /* commit by moving trailing up */

    tp->tdis_trailp = tp->tdis_leadp;
    }
  else
    {
    /* uncommit by moving leading back */

    tp->tdis_leadp = tp->tdis_trailp;
    }
  return(0);
  }



int tcp_chan_has_data(
    struct tcp_chan *chan)
  {
  int rc = FALSE;
  struct tcpdisbuf *tp;
  tp = &chan->readbuf;

  if (tp->tdis_eod != tp->tdis_leadp)
    rc = TRUE;
  return rc;
  }



/*
 * DIS_tcp_setup - setup supports routines for dis, "data is strings", to
 * use tcp stream I/O.  Also initializes an array of pointers to
 * buffers and a buffer to be used for the given fd.
 *
 * NOTE:  tmpArray is global
 *
 * NOTE:  does not return FAILURE - FIXME
 */

struct tcp_chan * DIS_tcp_setup(

  int fd)

  {
  struct tcp_chan  *chan = NULL;
  struct tcpdisbuf *tp = NULL;

  /* check for bad file descriptor */
  if (fd < 0)
    {
    return(NULL);
    }

  if ((chan = (struct tcp_chan *)calloc(1, sizeof(struct tcp_chan))) == NULL)
    {
    return(NULL);
    }

  /* Assign socket to struct */
  tcpData *data = new tcpData();
  fds.insert(std::pair<int,tcpData *>(fd,data));
  chan->sock = fd;

  /* Setting up the read buffer */
  tp = &chan->readbuf;
  if ((tp->tdis_thebuf = (char *)calloc(1, THE_BUF_SIZE+1)) == NULL)
    {
    delete data;
    free(chan);
    return(NULL);
    }

  tp->tdis_bufsize = THE_BUF_SIZE;
  DIS_tcp_clear(tp);

  /* Setting up the write buffer */
  tp = &chan->writebuf;
  if ((tp->tdis_thebuf = (char *)calloc(1, THE_BUF_SIZE+1)) == NULL)
    {
    delete data;
    free(chan->readbuf.tdis_thebuf);
    free(chan);
    return(NULL);
    }

  tp->tdis_bufsize = THE_BUF_SIZE;
  DIS_tcp_clear(tp);

  return(chan);
  }  /* END DIS_tcp_setup() */



void DIS_tcp_cleanup(

  struct tcp_chan *chan)

  {
  struct tcpdisbuf *tp = NULL;

  if (chan == NULL)
    return;
  tp = &chan->readbuf;
  free(tp->tdis_thebuf);

  tp = &chan->writebuf;
  free(tp->tdis_thebuf);
  tcpData *data = fds[chan->sock];
  if(data != NULL) delete data;
  free(chan);
  }

void DIS_tcp_close(

  struct tcp_chan *chan)

  {
  DIS_tcp_cleanup(chan);
  }

