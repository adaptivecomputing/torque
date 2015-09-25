#include "license_pbs.h" /* See here for the softare license */
#include <stdlib.h>
#include <stdio.h> /* fprintf */

#include "libpbs.h" /* connect_handle */

struct connect_handle connection[10];
const char *dis_emsg[] =
  {
  "No error",
  "Input value too large to convert to this type",
  "Tried to write floating point infinity",
  "Negative sign on an unsigned datum",
  "Input count or value has leading zero",
  "Non-digit found where a digit was expected",
  "Input string has an embedded ASCII NUL",
  "Premature end of message",
  "Unable to calloc enough space for string",
  "Supporting protocol failure",
  "Protocol failure in commit",
  "End of File",
  "Invalid condition in code"
  };

bool timeout = false;
bool setup_fail = false;
int  decode_rc;

int decode_DIS_replyCmd(struct tcp_chan *chan, struct batch_reply *reply)
  {
  if (timeout)
    chan->IsTimeout = TRUE;
  else
    chan->IsTimeout = FALSE;
  
  return(decode_rc);
  }

struct tcp_chan *DIS_tcp_setup(int fd)
  {
  if (setup_fail == true)
    return(NULL);
  else
    return((struct tcp_chan *)calloc(1, sizeof(tcp_chan)));
  }

void DIS_tcp_cleanup(struct tcp_chan *chan)
  {
  }

char *pbs_strerror(

  int err)  /* I */

  {
  if (err == PBSE_TIMEOUT)
    return((char *)"We timed out!");

  return(NULL);
  }
